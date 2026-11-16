#include "wifi_status.h"

#define CHECK(condition) do { if (!(condition)) return __LINE__; } while (0)

static bool equal(const char *left, const char *right) {
    while (*left != '\0' && *left == *right) {
        ++left;
        ++right;
    }
    return *left == *right;
}

int main(void) {
    wifi_status_t status = {WIFI_STATUS_UNKNOWN, false, 0};
    char label[15];

    CHECK(wifi_status_parse("other log", &status) == WIFI_RECORD_OTHER);
    CHECK(status.state == WIFI_STATUS_UNKNOWN);
    CHECK(wifi_status_parse("WIFI_STATUS S NA", &status) == WIFI_RECORD_VALID);
    CHECK(status.state == WIFI_STATUS_SEARCHING && !status.rssi_valid);
    CHECK(wifi_status_parse("WIFI_STATUS A -127", &status) == WIFI_RECORD_VALID);
    CHECK(status.state == WIFI_STATUS_SETUP && status.rssi_valid && status.rssi_dbm == -127);
    CHECK(wifi_status_parse("WIFI_STATUS C -86", &status) == WIFI_RECORD_VALID);
    CHECK(status.state == WIFI_STATUS_CONNECTED && status.rssi_valid && status.rssi_dbm == -86);
    wifi_rssi_label(&status, true, label);
    CHECK(equal(label, "RSSI -86 DBM"));
    wifi_rssi_label(&status, false, label);
    CHECK(equal(label, "RSSI -- DBM"));

    CHECK(wifi_status_parse("WIFI_STATUS C", &status) == WIFI_RECORD_INVALID);
    CHECK(wifi_status_parse("WIFI_STATUS C -", &status) == WIFI_RECORD_INVALID);
    CHECK(wifi_status_parse("WIFI_STATUS C -129", &status) == WIFI_RECORD_INVALID);
    CHECK(wifi_status_parse("WIFI_STATUS C 128", &status) == WIFI_RECORD_INVALID);
    CHECK(wifi_status_parse("WIFI_STATUS C -86 extra", &status) == WIFI_RECORD_INVALID);
    CHECK(wifi_status_parse("WIFI_STATUS X NA", &status) == WIFI_RECORD_INVALID);
    CHECK(wifi_status_parse("WIFI_STATUS S -86", &status) == WIFI_RECORD_INVALID);
    CHECK(status.state == WIFI_STATUS_CONNECTED && status.rssi_dbm == -86);
    CHECK(wifi_status_parse("WIFI_STATUS C 0", &status) == WIFI_RECORD_VALID);
    wifi_rssi_label(&status, true, label);
    CHECK(equal(label, "RSSI 0 DBM"));
    CHECK(wifi_status_parse("WIFI_STATUS C -128", &status) == WIFI_RECORD_VALID);
    wifi_rssi_label(&status, true, label);
    CHECK(equal(label, "RSSI -128 DBM"));
    CHECK(wifi_status_parse("WIFI_STATUS C 127", &status) == WIFI_RECORD_VALID);
    wifi_rssi_label(&status, true, label);
    CHECK(equal(label, "RSSI 127 DBM"));
    CHECK(wifi_status_parse("WIFI_STATUS E NA", &status) == WIFI_RECORD_VALID);
    CHECK(status.state == WIFI_STATUS_UNKNOWN && !status.rssi_valid);

    CHECK(!wifi_status_is_recent(false, 0U));
    CHECK(wifi_status_is_recent(true, 4999U));
    CHECK(!wifi_status_is_recent(true, 5000U));

    wifi_led_state_t led = {WIFI_STATUS_UNKNOWN, false};
    CHECK(!wifi_led_advance(&led, WIFI_STATUS_UNKNOWN, 1000U));
    CHECK(wifi_led_advance(&led, WIFI_STATUS_SEARCHING, 0U) && led.on);
    wifi_led_pattern_t pattern = wifi_led_pattern(led.state);
    CHECK(pattern.blue != 0U && pattern.red == 0U && pattern.green == 0U);
    CHECK(!wifi_led_advance(&led, WIFI_STATUS_SEARCHING, 499U) && led.on);
    CHECK(wifi_led_advance(&led, WIFI_STATUS_SEARCHING, 500U) && !led.on);
    CHECK(wifi_led_advance(&led, WIFI_STATUS_SEARCHING, 500U) && led.on);

    CHECK(wifi_led_advance(&led, WIFI_STATUS_SETUP, 0U) && led.on);
    pattern = wifi_led_pattern(led.state);
    CHECK(pattern.red != 0U && pattern.green != 0U && pattern.blue == 0U);
    CHECK(!wifi_led_advance(&led, WIFI_STATUS_SETUP, 149U) && led.on);
    CHECK(wifi_led_advance(&led, WIFI_STATUS_SETUP, 150U) && !led.on);

    CHECK(wifi_led_advance(&led, WIFI_STATUS_CONNECTED, 0U) && led.on);
    pattern = wifi_led_pattern(led.state);
    CHECK(pattern.green != 0U && pattern.red == 0U && pattern.blue == 0U);
    CHECK(wifi_led_advance(&led, WIFI_STATUS_CONNECTED, 150U) && !led.on);
    CHECK(!wifi_led_advance(&led, WIFI_STATUS_CONNECTED, 2849U) && !led.on);
    CHECK(wifi_led_advance(&led, WIFI_STATUS_CONNECTED, 2850U) && led.on);
    CHECK(wifi_led_advance(&led, WIFI_STATUS_UNKNOWN, 0U) && !led.on);
    return 0;
}
