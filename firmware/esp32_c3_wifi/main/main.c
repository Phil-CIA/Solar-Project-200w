#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_random.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

#define WIFI_NAMESPACE "wifi_config"
#define WIFI_SSID_KEY "ssid"
#define WIFI_PASSWORD_KEY "password"
#define WIFI_SSID_MAX_BYTES 32
#define WIFI_PASSWORD_MAX_BYTES 63
#define WIFI_RETRY_LIMIT 5
#define FORM_BODY_MAX_BYTES 384
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FALLBACK_BIT BIT1
#define PORTAL_JOIN_FAILED_BIT BIT2

static const char *TAG = "solar_wifi";
static EventGroupHandle_t s_wifi_events;
static httpd_handle_t s_http_server;
static volatile bool s_wifi_started;
static volatile bool s_portal_active;
static volatile bool s_station_join_requested;
static volatile unsigned s_retry_count;

static const char SETUP_PAGE[] =
    "<!doctype html><html><head><meta name=\"viewport\" "
    "content=\"width=device-width,initial-scale=1\"><title>Solar Wi-Fi setup</title>"
    "</head><body><h1>Solar Wi-Fi setup</h1>"
    "<form method=\"post\" action=\"/save\">"
    "<label>Network name (SSID)<br><input name=\"ssid\" maxlength=\"32\" "
    "autocomplete=\"username\" required></label><br><br>"
    "<label>Network password<br><input name=\"password\" type=\"password\" "
    "maxlength=\"63\" autocomplete=\"new-password\"></label><br><br>"
    "<button type=\"submit\">Save and connect</button></form></body></html>";

static esp_err_t save_credentials(const char *ssid, const char *password)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(WIFI_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_str(handle, WIFI_SSID_KEY, ssid);
    if (err == ESP_OK) {
        err = nvs_set_str(handle, WIFI_PASSWORD_KEY, password);
    }
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}

static bool load_credentials(char *ssid, size_t ssid_size,
                             char *password, size_t password_size)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(WIFI_NAMESPACE, NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return false;
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Cannot open Wi-Fi settings in NVS: %s", esp_err_to_name(err));
        return false;
    }

    size_t ssid_length = ssid_size;
    size_t password_length = password_size;
    err = nvs_get_str(handle, WIFI_SSID_KEY, ssid, &ssid_length);
    if (err == ESP_OK) {
        err = nvs_get_str(handle, WIFI_PASSWORD_KEY, password, &password_length);
    }
    nvs_close(handle);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return false;
    }
    if (err != ESP_OK || ssid_length < 2 || ssid_length > ssid_size ||
        password_length < 1 || password_length > password_size) {
        ESP_LOGW(TAG, "Stored Wi-Fi settings are missing or invalid; opening setup AP");
        return false;
    }
    return true;
}

static int hex_value(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

static bool url_decode(char *destination, size_t destination_size,
                       const char *source, size_t source_length)
{
    size_t output_length = 0;
    for (size_t i = 0; i < source_length; ++i) {
        unsigned char value = (unsigned char)source[i];
        if (value == '+') {
            value = ' ';
        } else if (value == '%') {
            if (i + 2 >= source_length) {
                return false;
            }
            int high = hex_value(source[i + 1]);
            int low = hex_value(source[i + 2]);
            if (high < 0 || low < 0) {
                return false;
            }
            value = (unsigned char)((high << 4) | low);
            i += 2;
        }

        if (value == '\0' || output_length + 1 >= destination_size) {
            return false;
        }
        destination[output_length++] = (char)value;
    }
    destination[output_length] = '\0';
    return true;
}

static bool parse_form(char *body, char *ssid, size_t ssid_size,
                       char *password, size_t password_size)
{
    bool found_ssid = false;
    bool found_password = false;
    char *save_pointer = NULL;

    for (char *pair = strtok_r(body, "&", &save_pointer);
         pair != NULL; pair = strtok_r(NULL, "&", &save_pointer)) {
        char *equals = strchr(pair, '=');
        if (equals == NULL) {
            return false;
        }
        *equals++ = '\0';
        char *destination = NULL;
        size_t destination_size = 0;
        bool *found = NULL;
        if (strcmp(pair, "ssid") == 0) {
            destination = ssid;
            destination_size = ssid_size;
            found = &found_ssid;
        } else if (strcmp(pair, "password") == 0) {
            destination = password;
            destination_size = password_size;
            found = &found_password;
        } else {
            continue;
        }

        if (*found || !url_decode(destination, destination_size, equals, strlen(equals))) {
            return false;
        }
        *found = true;
    }
    return found_ssid && found_password;
}

static esp_err_t handle_index(httpd_req_t *request)
{
    esp_err_t err = httpd_resp_set_type(request, "text/html; charset=utf-8");
    if (err == ESP_OK) {
        err = httpd_resp_set_hdr(request, "Cache-Control", "no-store");
    }
    if (err == ESP_OK) {
        err = httpd_resp_send(request, SETUP_PAGE, HTTPD_RESP_USE_STRLEN);
    }
    return err;
}

static esp_err_t handle_save(httpd_req_t *request)
{
    if (request->content_len == 0 || request->content_len > FORM_BODY_MAX_BYTES) {
        return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, "Invalid form size");
    }

    char body[FORM_BODY_MAX_BYTES + 1];
    size_t received = 0;
    while (received < request->content_len) {
        int count = httpd_req_recv(request, body + received,
                                   request->content_len - received);
        if (count <= 0) {
            return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST,
                                       "Could not read form");
        }
        received += (size_t)count;
    }
    body[received] = '\0';

    char ssid[WIFI_SSID_MAX_BYTES + 1] = {0};
    char password[WIFI_PASSWORD_MAX_BYTES + 1] = {0};
    if (!parse_form(body, ssid, sizeof(ssid), password, sizeof(password)) ||
        strlen(ssid) == 0 ||
        (strlen(password) != 0 && strlen(password) < 8)) {
        return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST,
                                   "Enter an SSID and a valid Wi-Fi password");
    }

    esp_err_t err = save_credentials(ssid, password);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Cannot save Wi-Fi settings: %s", esp_err_to_name(err));
        return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR,
                                   "Could not save Wi-Fi settings");
    }

    wifi_config_t station_config = {0};
    memcpy(station_config.sta.ssid, ssid, strlen(ssid));
    memcpy(station_config.sta.password, password, strlen(password));

    err = esp_wifi_set_mode(WIFI_MODE_APSTA);
    if (err == ESP_OK) {
        err = esp_wifi_set_config(WIFI_IF_STA, &station_config);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Cannot configure station: %s", esp_err_to_name(err));
        return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR,
                                   "Could not start Wi-Fi connection");
    }

    s_retry_count = 0;
    s_station_join_requested = true;
    xEventGroupClearBits(s_wifi_events,
                         WIFI_CONNECTED_BIT | WIFI_FALLBACK_BIT | PORTAL_JOIN_FAILED_BIT);
    err = esp_wifi_connect();
    if (err != ESP_OK) {
        s_station_join_requested = false;
        ESP_LOGE(TAG, "Cannot start Wi-Fi connection: %s", esp_err_to_name(err));
        return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR,
                                   "Could not start Wi-Fi connection");
    }

    static const char response[] =
        "<!doctype html><html><body><h1>Settings saved</h1>"
        "<p>Trying to join the network. The setup page will close after connection.</p>"
        "<p>If connection fails, reconnect to the setup network and try again.</p>"
        "</body></html>";
    err = httpd_resp_set_type(request, "text/html; charset=utf-8");
    if (err != ESP_OK) {
        return err;
    }
    return httpd_resp_send(request, response, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t start_http_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 6144;
    config.max_uri_handlers = 2;

    esp_err_t err = httpd_start(&s_http_server, &config);
    if (err != ESP_OK) {
        return err;
    }

    const httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = handle_index,
        .user_ctx = NULL
    };
    const httpd_uri_t save_uri = {
        .uri = "/save",
        .method = HTTP_POST,
        .handler = handle_save,
        .user_ctx = NULL
    };
    err = httpd_register_uri_handler(s_http_server, &index_uri);
    if (err == ESP_OK) {
        err = httpd_register_uri_handler(s_http_server, &save_uri);
    }
    if (err != ESP_OK) {
        httpd_stop(s_http_server);
        s_http_server = NULL;
    }
    return err;
}

static esp_err_t configure_setup_ap(void)
{
    static const char alphabet[] =
        "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz23456789";
    char ap_ssid[33];
    char ap_password[13];
    uint8_t mac[6];

    ESP_ERROR_CHECK(esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP));
    snprintf(ap_ssid, sizeof(ap_ssid), "Solar-Setup-%02X%02X%02X",
             mac[3], mac[4], mac[5]);
    for (size_t i = 0; i < sizeof(ap_password) - 1; ++i) {
        ap_password[i] = alphabet[esp_random() % (sizeof(alphabet) - 1)];
    }
    ap_password[sizeof(ap_password) - 1] = '\0';

    wifi_config_t ap_config = {0};
    memcpy(ap_config.ap.ssid, ap_ssid, strlen(ap_ssid));
    ap_config.ap.ssid_len = (uint8_t)strlen(ap_ssid);
    memcpy(ap_config.ap.password, ap_password, strlen(ap_password) + 1);
    ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ap_config.ap.max_connection = 2;

    s_station_join_requested = false;
    s_portal_active = true;
    esp_err_t err = esp_wifi_set_mode(WIFI_MODE_APSTA);
    if (err == ESP_OK) {
        err = esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    }
    if (err == ESP_OK) {
        err = esp_wifi_set_mode(WIFI_MODE_AP);
    }
    if (err == ESP_OK && !s_wifi_started) {
        err = esp_wifi_start();
        if (err == ESP_OK) {
            s_wifi_started = true;
        }
    }
    if (err != ESP_OK) {
        s_portal_active = false;
        return err;
    }

    err = start_http_server();
    if (err != ESP_OK) {
        s_portal_active = false;
        return err;
    }

    ESP_LOGI(TAG, "Setup network: %s", ap_ssid);
    ESP_LOGI(TAG, "Setup password: %s", ap_password);
    ESP_LOGI(TAG, "Open http://192.168.4.1/ to configure Wi-Fi");
    return ESP_OK;
}

static void wifi_event_handler(void *argument, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    (void)argument;
    (void)event_data;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupClearBits(s_wifi_events, WIFI_CONNECTED_BIT);
        if (!s_station_join_requested) {
            return;
        }
        if (s_retry_count < WIFI_RETRY_LIMIT) {
            ++s_retry_count;
            esp_err_t err = esp_wifi_connect();
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "Reconnect request failed: %s", esp_err_to_name(err));
            }
            return;
        }

        s_station_join_requested = false;
        if (s_portal_active) {
            xEventGroupSetBits(s_wifi_events, PORTAL_JOIN_FAILED_BIT);
        } else {
            xEventGroupSetBits(s_wifi_events, WIFI_FALLBACK_BIT);
        }
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        s_retry_count = 0;
        ESP_LOGI(TAG, "Connected; IP address: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_events, WIFI_CONNECTED_BIT);
    }
}

void app_main(void)
{
    char ssid[WIFI_SSID_MAX_BYTES + 1] = {0};
    char password[WIFI_PASSWORD_MAX_BYTES + 1] = {0};

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    if (esp_netif_create_default_wifi_sta() == NULL ||
        esp_netif_create_default_wifi_ap() == NULL) {
        ESP_LOGE(TAG, "Could not create Wi-Fi network interfaces");
        abort();
    }

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    s_wifi_events = xEventGroupCreate();
    if (s_wifi_events == NULL) {
        ESP_LOGE(TAG, "Could not create Wi-Fi event group");
        abort();
    }
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                               wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                               wifi_event_handler, NULL));

    if (load_credentials(ssid, sizeof(ssid), password, sizeof(password))) {
        wifi_config_t station_config = {0};
        memcpy(station_config.sta.ssid, ssid, strlen(ssid));
        memcpy(station_config.sta.password, password, strlen(password));
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &station_config));
        s_station_join_requested = true;
        ESP_ERROR_CHECK(esp_wifi_start());
        s_wifi_started = true;
        ESP_ERROR_CHECK(esp_wifi_connect());
        ESP_LOGI(TAG, "Trying saved network: %s", ssid);
    } else {
        ESP_ERROR_CHECK(configure_setup_ap());
    }

    for (;;) {
        EventBits_t bits = xEventGroupWaitBits(
            s_wifi_events,
            WIFI_CONNECTED_BIT | WIFI_FALLBACK_BIT | PORTAL_JOIN_FAILED_BIT,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY);

        if ((bits & WIFI_CONNECTED_BIT) != 0) {
            s_retry_count = 0;
            if (s_portal_active) {
                vTaskDelay(pdMS_TO_TICKS(1500));
                if (s_http_server != NULL) {
                    ESP_ERROR_CHECK(httpd_stop(s_http_server));
                    s_http_server = NULL;
                }
                ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
                s_portal_active = false;
                ESP_LOGI(TAG, "Setup AP stopped; station mode active");
            }
        } else if ((bits & WIFI_FALLBACK_BIT) != 0) {
            ESP_LOGW(TAG, "Saved network unavailable; starting setup AP");
            ESP_ERROR_CHECK(configure_setup_ap());
        } else if ((bits & PORTAL_JOIN_FAILED_BIT) != 0) {
            ESP_LOGW(TAG, "Could not join network; setup AP remains available");
        }
    }
}
