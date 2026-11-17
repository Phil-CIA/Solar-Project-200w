#ifndef WIFI_STATUS_H
#define WIFI_STATUS_H

#include <stdbool.h>
#include <stdint.h>

#define WIFI_STATUS_STALE_MS 5000U

typedef enum {
    WIFI_STATUS_UNKNOWN,
    WIFI_STATUS_SEARCHING,
    WIFI_STATUS_SETUP,
    WIFI_STATUS_CONNECTED
} wifi_status_state_t;

typedef struct {
    wifi_status_state_t state;
    bool rssi_valid;
    int8_t rssi_dbm;
} wifi_status_t;

typedef enum {
    WIFI_RECORD_OTHER,
    WIFI_RECORD_VALID,
    WIFI_RECORD_INVALID
} wifi_record_result_t;

typedef struct {
    uint16_t on_ms;
    uint16_t off_ms;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} wifi_led_pattern_t;

typedef struct {
    wifi_status_state_t state;
    bool on;
} wifi_led_state_t;

static bool wifi_status_is_recent(bool valid, uint32_t elapsed_ms) {
    return valid && elapsed_ms < WIFI_STATUS_STALE_MS;
}

static wifi_record_result_t wifi_status_parse(const char *line, wifi_status_t *out) {
    static const char prefix[] = "WIFI_STATUS ";
    uint32_t pos = 0U;
    while (prefix[pos] != '\0') {
        if (line[pos] != prefix[pos]) {
            return WIFI_RECORD_OTHER;
        }
        ++pos;
    }

    wifi_status_t parsed = {WIFI_STATUS_UNKNOWN, false, 0};
    switch (line[pos++]) {
    case 'E': parsed.state = WIFI_STATUS_UNKNOWN; break;
    case 'S': parsed.state = WIFI_STATUS_SEARCHING; break;
    case 'A': parsed.state = WIFI_STATUS_SETUP; break;
    case 'C': parsed.state = WIFI_STATUS_CONNECTED; break;
    default: return WIFI_RECORD_INVALID;
    }
    if (line[pos++] != ' ') {
        return WIFI_RECORD_INVALID;
    }

    if (line[pos] == 'N') {
        ++pos;
        if (line[pos++] != 'A' || line[pos] != '\0') {
            return WIFI_RECORD_INVALID;
        }
    } else {
        bool negative = false;
        uint32_t magnitude = 0U;
        if (line[pos] == '-') {
            negative = true;
            ++pos;
        }
        if (line[pos] < '0' || line[pos] > '9') {
            return WIFI_RECORD_INVALID;
        }
        while (line[pos] >= '0' && line[pos] <= '9') {
            magnitude = magnitude * 10U + (uint32_t)(line[pos] - '0');
            if (magnitude > (negative ? 128U : 127U)) {
                return WIFI_RECORD_INVALID;
            }
            ++pos;
        }
        if (line[pos] != '\0') {
            return WIFI_RECORD_INVALID;
        }
        parsed.rssi_valid = true;
        parsed.rssi_dbm = (int8_t)(negative ? -(int32_t)magnitude : (int32_t)magnitude);
    }
    if (parsed.state == WIFI_STATUS_SEARCHING && parsed.rssi_valid) {
        return WIFI_RECORD_INVALID;
    }
    *out = parsed;
    return WIFI_RECORD_VALID;
}

static void wifi_rssi_label(const wifi_status_t *status, bool fresh, char out[15]) {
    uint32_t pos = 0U;
    static const char prefix[] = "RSSI ";
    for (uint32_t i = 0U; prefix[i] != '\0'; ++i) {
        out[pos++] = prefix[i];
    }
    if (fresh && status->rssi_valid) {
        int32_t value = status->rssi_dbm;
        if (value < 0) {
            out[pos++] = '-';
            value = -value;
        }
        if (value >= 100) {
            out[pos++] = (char)('0' + value / 100);
        }
        if (value >= 10) {
            out[pos++] = (char)('0' + (value / 10) % 10);
        }
        out[pos++] = (char)('0' + value % 10);
    } else {
        out[pos++] = '-';
        out[pos++] = '-';
    }
    out[pos++] = ' ';
    out[pos++] = 'D';
    out[pos++] = 'B';
    out[pos++] = 'M';
    out[pos] = '\0';
}

static wifi_led_pattern_t wifi_led_pattern(wifi_status_state_t state) {
    switch (state) {
    case WIFI_STATUS_SEARCHING:
        return (wifi_led_pattern_t){500U, 500U, 0U, 0U, 0x08U};
    case WIFI_STATUS_SETUP:
        return (wifi_led_pattern_t){150U, 150U, 0x08U, 0x04U, 0U};
    case WIFI_STATUS_CONNECTED:
        return (wifi_led_pattern_t){150U, 2850U, 0U, 0x08U, 0U};
    case WIFI_STATUS_UNKNOWN:
    default:
        return (wifi_led_pattern_t){0U, 0U, 0U, 0U, 0U};
    }
}

static bool wifi_led_advance(wifi_led_state_t *led, wifi_status_state_t state,
                             uint32_t elapsed_ms) {
    if (state != led->state) {
        led->state = state;
        led->on = (state != WIFI_STATUS_UNKNOWN);
        return true;
    }
    if (state == WIFI_STATUS_UNKNOWN) {
        return false;
    }
    const wifi_led_pattern_t pattern = wifi_led_pattern(state);
    if (elapsed_ms < (led->on ? pattern.on_ms : pattern.off_ms)) {
        return false;
    }
    led->on = !led->on;
    return true;
}

#endif
