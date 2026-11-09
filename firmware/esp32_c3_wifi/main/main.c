#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "driver/usb_serial_jtag.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs.h"
#include "nvs_flash.h"

#define WIFI_MAXIMUM_RETRY 8
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define WIFI_PROV_BIT      BIT2

#define WIFI_NAMESPACE     "wifi_cfg"
#define WIFI_SSID_KEY      "ssid"
#define WIFI_PASS_KEY      "pass"
#define MAX_SSID_LEN       32
#define MAX_PASS_LEN       63
#define MAX_FORM_BODY_LEN  192

static const char *TAG = "wifi_prov";

typedef struct {
    char ssid[MAX_SSID_LEN + 1];
    char pass[MAX_PASS_LEN + 1];
    bool valid;
} wifi_creds_t;

static EventGroupHandle_t s_wifi_event_group;
static httpd_handle_t s_httpd_server;
static wifi_creds_t s_prov_creds;
static int s_retry_num;

static const char *wifi_reason_to_str(uint8_t reason) {
    switch (reason) {
        case WIFI_REASON_AUTH_EXPIRE: return "AUTH_EXPIRE";
        case WIFI_REASON_AUTH_LEAVE: return "AUTH_LEAVE";
        case WIFI_REASON_ASSOC_EXPIRE: return "ASSOC_EXPIRE";
        case WIFI_REASON_ASSOC_TOOMANY: return "ASSOC_TOOMANY";
        case WIFI_REASON_NOT_AUTHED: return "NOT_AUTHED";
        case WIFI_REASON_NOT_ASSOCED: return "NOT_ASSOCED";
        case WIFI_REASON_ASSOC_LEAVE: return "ASSOC_LEAVE";
        case WIFI_REASON_ASSOC_NOT_AUTHED: return "ASSOC_NOT_AUTHED";
        case WIFI_REASON_DISASSOC_PWRCAP_BAD: return "DISASSOC_PWRCAP_BAD";
        case WIFI_REASON_DISASSOC_SUPCHAN_BAD: return "DISASSOC_SUPCHAN_BAD";
        case WIFI_REASON_IE_INVALID: return "IE_INVALID";
        case WIFI_REASON_MIC_FAILURE: return "MIC_FAILURE";
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT: return "4WAY_TIMEOUT";
        case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT: return "GROUP_KEY_TIMEOUT";
        case WIFI_REASON_IE_IN_4WAY_DIFFERS: return "IE_IN_4WAY_DIFFERS";
        case WIFI_REASON_GROUP_CIPHER_INVALID: return "GROUP_CIPHER_INVALID";
        case WIFI_REASON_PAIRWISE_CIPHER_INVALID: return "PAIRWISE_CIPHER_INVALID";
        case WIFI_REASON_AKMP_INVALID: return "AKMP_INVALID";
        case WIFI_REASON_UNSUPP_RSN_IE_VERSION: return "UNSUPP_RSN_IE_VERSION";
        case WIFI_REASON_INVALID_RSN_IE_CAP: return "INVALID_RSN_IE_CAP";
        case WIFI_REASON_802_1X_AUTH_FAILED: return "8021X_AUTH_FAILED";
        case WIFI_REASON_CIPHER_SUITE_REJECTED: return "CIPHER_SUITE_REJECTED";
        case WIFI_REASON_BEACON_TIMEOUT: return "BEACON_TIMEOUT";
        case WIFI_REASON_NO_AP_FOUND: return "NO_AP_FOUND";
        case WIFI_REASON_AUTH_FAIL: return "AUTH_FAIL";
        case WIFI_REASON_ASSOC_FAIL: return "ASSOC_FAIL";
        case WIFI_REASON_HANDSHAKE_TIMEOUT: return "HANDSHAKE_TIMEOUT";
        case WIFI_REASON_CONNECTION_FAIL: return "CONNECTION_FAIL";
        case WIFI_REASON_AP_TSF_RESET: return "AP_TSF_RESET";
        case WIFI_REASON_ROAMING: return "ROAMING";
        default: return "UNKNOWN";
    }
}

static const char *wifi_authmode_to_str(wifi_auth_mode_t mode) {
    switch (mode) {
        case WIFI_AUTH_OPEN: return "OPEN";
        case WIFI_AUTH_WEP: return "WEP";
        case WIFI_AUTH_WPA_PSK: return "WPA_PSK";
        case WIFI_AUTH_WPA2_PSK: return "WPA2_PSK";
        case WIFI_AUTH_WPA_WPA2_PSK: return "WPA_WPA2_PSK";
        case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2_ENTERPRISE";
        case WIFI_AUTH_WPA3_PSK: return "WPA3_PSK";
        case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2_WPA3_PSK";
        case WIFI_AUTH_WAPI_PSK: return "WAPI_PSK";
        default: return "UNKNOWN";
    }
}

static void log_target_scan_diagnostics(const wifi_creds_t *creds) {
    wifi_scan_config_t scan_cfg = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true
    };
    esp_err_t err = esp_wifi_scan_start(&scan_cfg, true);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Scan start failed: %s", esp_err_to_name(err));
        return;
    }

    uint16_t ap_count = 0;
    err = esp_wifi_scan_get_ap_num(&ap_count);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Scan count read failed: %s", esp_err_to_name(err));
        return;
    }

    enum { SCAN_RECORD_LIMIT = 20 };
    wifi_ap_record_t *records = calloc(SCAN_RECORD_LIMIT, sizeof(*records));
    if (records == NULL) {
        ESP_LOGW(TAG, "Scan records allocation failed");
        return;
    }

    uint16_t fetched = SCAN_RECORD_LIMIT;
    err = esp_wifi_scan_get_ap_records(&fetched, records);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Scan records read failed: %s", esp_err_to_name(err));
        free(records);
        return;
    }

    uint16_t matches = 0;
    int best_rssi = -127;
    uint8_t best_chan = 0;
    wifi_auth_mode_t best_auth = WIFI_AUTH_OPEN;

    for (uint16_t i = 0; i < fetched; ++i) {
        if (strncmp((const char *)records[i].ssid, creds->ssid, sizeof(records[i].ssid)) == 0) {
            matches++;
            if (records[i].rssi > best_rssi) {
                best_rssi = records[i].rssi;
                best_chan = records[i].primary;
                best_auth = records[i].authmode;
            }
        }
    }

    ESP_LOGI(TAG, "Pre-connect scan: ap_count=%u fetched=%u target_matches=%u",
             (unsigned)ap_count, (unsigned)fetched, (unsigned)matches);
    if (matches > 0) {
        ESP_LOGI(TAG, "Best target match: rssi=%d dBm channel=%u auth=%s",
                 best_rssi, (unsigned)best_chan, wifi_authmode_to_str(best_auth));
    } else {
        ESP_LOGW(TAG, "Target AP not found in scan snapshot");
    }

    free(records);
}

static const char k_form_html[] =
    "<!doctype html><html><head><meta charset='utf-8'><title>Solar Wi-Fi Setup</title></head>"
    "<body><h2>Solar Wi-Fi Setup</h2>"
    "<form method='POST' action='/save'>"
    "<label>SSID (1-32 chars)</label><br><input name='ssid' maxlength='32' required><br><br>"
    "<label>Password (8-63 chars for WPA/WPA2, leave empty for open)</label><br>"
    "<input name='pass' type='password' maxlength='63'><br><br>"
    "<button type='submit'>Save and Connect</button></form><br>"
    "<form method='POST' action='/clear'><button type='submit'>Clear Saved Credentials</button></form>"
    "</body></html>";

static uint8_t hex_to_nibble(char c) {
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c - '0');
    }
    c = (char)tolower((unsigned char)c);
    if (c >= 'a' && c <= 'f') {
        return (uint8_t)(10 + c - 'a');
    }
    return 0xFF;
}

static size_t url_decode(const char *in, size_t in_len, char *out, size_t out_len) {
    size_t w = 0;
    for (size_t r = 0; r < in_len && w + 1 < out_len; ++r) {
        char c = in[r];
        if (c == '+') {
            out[w++] = ' ';
            continue;
        }
        if (c == '%' && r + 2 < in_len) {
            const uint8_t hi = hex_to_nibble(in[r + 1]);
            const uint8_t lo = hex_to_nibble(in[r + 2]);
            if (hi != 0xFF && lo != 0xFF) {
                out[w++] = (char)((hi << 4U) | lo);
                r += 2;
                continue;
            }
        }
        out[w++] = c;
    }
    out[w] = '\0';
    return w;
}

static void trim_ascii_whitespace(char *s) {
    size_t len = strlen(s);
    size_t start = 0;
    while (start < len && isspace((unsigned char)s[start])) {
        start++;
    }
    size_t end = len;
    while (end > start && isspace((unsigned char)s[end - 1])) {
        end--;
    }
    if (start > 0 || end < len) {
        memmove(s, s + start, end - start);
        s[end - start] = '\0';
    }
}

static bool parse_form_value(const char *body, const char *key, char *out, size_t out_len) {
    const size_t key_len = strlen(key);
    const char *cur = body;
    while (*cur != '\0') {
        const char *next = strchr(cur, '&');
        const size_t part_len = next ? (size_t)(next - cur) : strlen(cur);
        if (part_len > key_len + 1 && strncmp(cur, key, key_len) == 0 && cur[key_len] == '=') {
            const char *value = cur + key_len + 1;
            const size_t value_len = part_len - (key_len + 1);
            url_decode(value, value_len, out, out_len);
            return true;
        }
        if (!next) {
            break;
        }
        cur = next + 1;
    }
    out[0] = '\0';
    return false;
}

static esp_err_t save_creds_to_nvs(const wifi_creds_t *creds) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(WIFI_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        return err;
    }
    err = nvs_set_str(nvs_handle, WIFI_SSID_KEY, creds->ssid);
    if (err == ESP_OK) {
        err = nvs_set_str(nvs_handle, WIFI_PASS_KEY, creds->pass);
    }
    if (err == ESP_OK) {
        err = nvs_commit(nvs_handle);
    }
    nvs_close(nvs_handle);
    return err;
}

static esp_err_t load_creds_from_nvs(wifi_creds_t *creds) {
    memset(creds, 0, sizeof(*creds));
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(WIFI_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        return err;
    }

    size_t ssid_len = sizeof(creds->ssid);
    size_t pass_len = sizeof(creds->pass);
    err = nvs_get_str(nvs_handle, WIFI_SSID_KEY, creds->ssid, &ssid_len);
    if (err == ESP_OK) {
        err = nvs_get_str(nvs_handle, WIFI_PASS_KEY, creds->pass, &pass_len);
    }
    nvs_close(nvs_handle);
    if (err != ESP_OK) {
        return err;
    }

    const size_t ssid_actual_len = strnlen(creds->ssid, sizeof(creds->ssid));
    const size_t pass_actual_len = strnlen(creds->pass, sizeof(creds->pass));
    if (ssid_actual_len == 0 || ssid_actual_len > MAX_SSID_LEN) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (pass_actual_len > 0 && (pass_actual_len < 8 || pass_actual_len > MAX_PASS_LEN)) {
        return ESP_ERR_INVALID_SIZE;
    }
    creds->valid = true;
    return ESP_OK;
}

static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, k_form_html, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t save_post_handler(httpd_req_t *req) {
    if (req->content_len <= 0 || req->content_len > MAX_FORM_BODY_LEN) {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "Invalid form size.");
    }

    char body[MAX_FORM_BODY_LEN + 1];
    int total = 0;
    while (total < req->content_len) {
        int recv_len = httpd_req_recv(req, body + total, req->content_len - total);
        if (recv_len <= 0) {
            httpd_resp_set_status(req, "400 Bad Request");
            return httpd_resp_sendstr(req, "Could not read form.");
        }
        total += recv_len;
    }
    body[total] = '\0';

    wifi_creds_t incoming = {0};
    bool has_ssid = parse_form_value(body, "ssid", incoming.ssid, sizeof(incoming.ssid));
    (void)parse_form_value(body, "pass", incoming.pass, sizeof(incoming.pass));
    trim_ascii_whitespace(incoming.ssid);
    trim_ascii_whitespace(incoming.pass);
    if (!has_ssid) {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "SSID missing.");
    }

    size_t ssid_len = strnlen(incoming.ssid, sizeof(incoming.ssid));
    size_t pass_len = strnlen(incoming.pass, sizeof(incoming.pass));
    if (ssid_len == 0 || ssid_len > MAX_SSID_LEN) {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "SSID must be 1-32 chars.");
    }
    if (pass_len > 0 && (pass_len < 8 || pass_len > MAX_PASS_LEN)) {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_sendstr(req, "Password must be 8-63 chars or empty.");
    }
    incoming.valid = true;

    esp_err_t err = save_creds_to_nvs(&incoming);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS save failed: %s", esp_err_to_name(err));
        httpd_resp_set_status(req, "500 Internal Server Error");
        return httpd_resp_sendstr(req, "Failed to save credentials.");
    }

    s_prov_creds = incoming;
    ESP_LOGI(TAG, "Provisioned SSID received (len=%u), password len=%u",
             (unsigned)ssid_len, (unsigned)pass_len);
    xEventGroupSetBits(s_wifi_event_group, WIFI_PROV_BIT);
    return httpd_resp_sendstr(req, "Saved. Device is connecting to Wi-Fi now.");
}

static esp_err_t clear_post_handler(httpd_req_t *req) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(WIFI_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        httpd_resp_set_status(req, "500 Internal Server Error");
        return httpd_resp_sendstr(req, "NVS open failed.");
    }
    err = nvs_erase_key(nvs_handle, WIFI_SSID_KEY);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        err = ESP_OK;
    }
    if (err == ESP_OK) {
        esp_err_t pass_err = nvs_erase_key(nvs_handle, WIFI_PASS_KEY);
        if (pass_err != ESP_OK && pass_err != ESP_ERR_NVS_NOT_FOUND) {
            err = pass_err;
        }
    }
    if (err == ESP_OK) {
        err = nvs_commit(nvs_handle);
    }
    nvs_close(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS clear failed: %s", esp_err_to_name(err));
        httpd_resp_set_status(req, "500 Internal Server Error");
        return httpd_resp_sendstr(req, "Failed to clear credentials.");
    }
    memset(&s_prov_creds, 0, sizeof(s_prov_creds));
    ESP_LOGW(TAG, "Saved Wi-Fi credentials cleared");
    return httpd_resp_sendstr(req, "Saved credentials cleared. Re-enter Wi-Fi credentials.");
}

static esp_err_t start_http_server(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_open_sockets = 3;
    config.uri_match_fn = httpd_uri_match_wildcard;

    esp_err_t err = httpd_start(&s_httpd_server, &config);
    if (err != ESP_OK) {
        return err;
    }

    const httpd_uri_t root_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_get_handler,
        .user_ctx = NULL
    };
    const httpd_uri_t save_uri = {
        .uri = "/save",
        .method = HTTP_POST,
        .handler = save_post_handler,
        .user_ctx = NULL
    };
    const httpd_uri_t clear_uri = {
        .uri = "/clear",
        .method = HTTP_POST,
        .handler = clear_post_handler,
        .user_ctx = NULL
    };

    httpd_register_uri_handler(s_httpd_server, &root_uri);
    httpd_register_uri_handler(s_httpd_server, &save_uri);
    httpd_register_uri_handler(s_httpd_server, &clear_uri);
    return ESP_OK;
}

static void stop_http_server(void) {
    if (s_httpd_server) {
        httpd_stop(s_httpd_server);
        s_httpd_server = NULL;
    }
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        return;
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *disconn = (wifi_event_sta_disconnected_t *)event_data;
        ESP_LOGW(TAG, "STA disconnected: reason=%u (%s)", disconn->reason, wifi_reason_to_str(disconn->reason));
        if (s_retry_num < WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGW(TAG, "Wi-Fi reconnect attempt %d/%d", s_retry_num, WIFI_MAXIMUM_RETRY);
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        return;
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static esp_err_t wifi_try_connect_sta(const wifi_creds_t *creds) {
    wifi_config_t wifi_cfg = {0};
    strlcpy((char *)wifi_cfg.sta.ssid, creds->ssid, sizeof(wifi_cfg.sta.ssid));
    strlcpy((char *)wifi_cfg.sta.password, creds->pass, sizeof(wifi_cfg.sta.password));
    wifi_cfg.sta.threshold.authmode = WIFI_AUTH_OPEN;
    wifi_cfg.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
    wifi_cfg.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
    if (creds->pass[0] == '\0') {
        wifi_cfg.sta.threshold.authmode = WIFI_AUTH_OPEN;
    }
    wifi_cfg.sta.pmf_cfg.capable = true;
    wifi_cfg.sta.pmf_cfg.required = false;

    ESP_LOGI(TAG, "Attempting STA connect using saved credentials (password_len=%u)",
             (unsigned)strnlen(creds->pass, sizeof(creds->pass)));
    xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);
    s_retry_num = 0;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());
    log_target_scan_diagnostics(creds);
    ESP_ERROR_CHECK(esp_wifi_connect());

    const EventBits_t bits = xEventGroupWaitBits(
        s_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdTRUE,
        pdFALSE,
        pdMS_TO_TICKS(45000));

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "STA connected, provisioning AP is disabled");
        return ESP_OK;
    }

    ESP_LOGW(TAG, "STA connection failed; reopening provisioning AP");
    ESP_ERROR_CHECK(esp_wifi_stop());
    return ESP_FAIL;
}

static esp_err_t start_provisioning_ap(void) {
    uint8_t mac[6] = {0};
    ESP_ERROR_CHECK(esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP));

    char ap_ssid[33];
    snprintf(ap_ssid, sizeof(ap_ssid), "SolarSetup-%02X%02X%02X", mac[3], mac[4], mac[5]);

    wifi_config_t ap_cfg = {0};
    strlcpy((char *)ap_cfg.ap.ssid, ap_ssid, sizeof(ap_cfg.ap.ssid));
    ap_cfg.ap.ssid_len = (uint8_t)strlen(ap_ssid);
    ap_cfg.ap.channel = 1;
    ap_cfg.ap.max_connection = 4;
    ap_cfg.ap.authmode = WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGW(TAG, "Provisioning AP active: SSID=%s", ap_ssid);
    return start_http_server();
}

static const char *reset_reason_to_str(esp_reset_reason_t reason) {
    switch (reason) {
        case ESP_RST_POWERON: return "POWERON";
        case ESP_RST_EXT: return "EXTERNAL_PIN";
        case ESP_RST_SW: return "SOFTWARE";
        case ESP_RST_PANIC: return "PANIC";
        case ESP_RST_INT_WDT: return "INT_WDT";
        case ESP_RST_TASK_WDT: return "TASK_WDT";
        case ESP_RST_WDT: return "OTHER_WDT";
        case ESP_RST_DEEPSLEEP: return "DEEPSLEEP";
        case ESP_RST_BROWNOUT: return "BROWNOUT";
        case ESP_RST_SDIO: return "SDIO";
        case ESP_RST_USB: return "USB";
        case ESP_RST_JTAG: return "JTAG";
        default: return "UNKNOWN";
    }
}

// USB Serial/JTAG re-enumerates on every chip reset, so early boot logs are lost
// before the host terminal reopens COM. Hold briefly so the banner is visible.
static void wait_for_usb_console_and_log_boot(void) {
    const TickType_t start = xTaskGetTickCount();
    while (!usb_serial_jtag_is_connected() &&
           (xTaskGetTickCount() - start) < pdMS_TO_TICKS(3000)) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    if (usb_serial_jtag_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
    const esp_reset_reason_t reason = esp_reset_reason();
    ESP_LOGW(TAG, "===== BOOT: reset_reason=%s (%d) =====", reset_reason_to_str(reason), (int)reason);
}

void app_main(void) {
    wait_for_usb_console_and_log_boot();

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    s_wifi_event_group = xEventGroupCreate();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_creds_t boot_creds = {0};
    err = load_creds_from_nvs(&boot_creds);
    if (err == ESP_OK && boot_creds.valid && wifi_try_connect_sta(&boot_creds) == ESP_OK) {
        ESP_LOGI(TAG, "Boot Wi-Fi connected from saved credentials");
        return;
    }

    while (true) {
        if (start_provisioning_ap() != ESP_OK) {
            ESP_LOGE(TAG, "Provisioning AP start failed; retrying in 3 seconds");
            vTaskDelay(pdMS_TO_TICKS(3000));
            continue;
        }

        xEventGroupClearBits(s_wifi_event_group, WIFI_PROV_BIT);
        xEventGroupWaitBits(s_wifi_event_group, WIFI_PROV_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        stop_http_server();
        ESP_ERROR_CHECK(esp_wifi_stop());

        if (s_prov_creds.valid && wifi_try_connect_sta(&s_prov_creds) == ESP_OK) {
            ESP_LOGI(TAG, "Provisioning complete");
            return;
        }
    }
}
