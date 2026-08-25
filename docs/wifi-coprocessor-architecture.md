# Wi-Fi Coprocessor Architecture

Last updated: 2026-08-08  
Status: Accepted Rev 1 baseline  
Decision: DEC-015

## System Boundary

The STM32 is the charge-controller and safety owner. The ESP32-C3 development module is a network coprocessor. A Wi-Fi outage, ESP32 reset, or communications failure must not interrupt safe charger operation.

### STM32 Owns

- MPPT and charge-state control
- PWM and power-stage enable control
- Measurements, limits, and fault handling
- Fan, buttons, display, and local operation
- Validation and execution of all remote requests

### ESP32 Owns

- Wi-Fi provisioning and reconnection
- Credential storage in ESP32 NVS
- Network time synchronization
- MQTT or HTTPS connectivity
- Telemetry publication and remote-request forwarding
- OTA updates for ESP32 firmware

STM32 firmware updates remain through SWD for the initial implementation. ESP32-assisted STM32 updates are deferred until the communications protocol is proven.

## Hardware Interface

- Power the ESP32 development module through its `5V` pin from `CTRL_5V`.
- Leave the module's exposed `3.3V` pin unpowered; the module uses its onboard regulator.
- Use common `PWR_NEG` reference.
- Use UART `TXD_ESP` and `RXD_ESP` for the data channel.
- Keep `RTS_ESP` and `CTS_ESP` routed as optional hardware flow control.
- Initial UART rate: 115200 baud; raise only after validation.
- Preserve the module antenna keepout in PCB placement and copper pours.

## UART Protocol Baseline

Use binary, versioned, framed messages rather than an unframed text stream:

`COBS(version | message_type | sequence | payload_length | payload | CRC16)`

Initial message set:

- `PROTOCOL_INFO`: protocol and firmware versions
- `HEARTBEAT`: link health
- `STATUS`: controller state, active faults, and uptime
- `TELEMETRY`: PV, battery, power, and temperature values
- `GET_CONFIG`: read approved controller settings
- `SET_CONFIG`: request approved setting changes
- `COMMAND`: request start, stop, or permitted fault clear

The STM32 validates every setting and command against local safety limits. Missing ESP32 heartbeats do not change charger operation. Missing STM32 heartbeats cause the ESP32 to report the controller offline.

## Wi-Fi and Remote Access

1. On first boot or explicit credential reset, the ESP32 starts a temporary setup access point.
2. A local setup page collects the network SSID and password.
3. Credentials are stored in ESP32 NVS.
4. The setup access point is disabled during normal operation.
5. Telemetry is published periodically; faults are published immediately.
6. Remote control requires authenticated, encrypted transport before it is enabled.

MQTT is the preferred first remote telemetry transport. Topic naming, payload schema, authentication, and broker selection remain firmware-session tasks.

## Failure Rules

- Charger safety does not depend on the ESP32 or network.
- ESP32 commands are requests, not direct actuator control.
- Invalid, stale, duplicate, or out-of-range requests are rejected by the STM32.
- Protocol versions are exchanged before commands are accepted.
- ESP32 reset and update activity must not toggle charge-control nets.

## Deferred Details

- MQTT broker and topic hierarchy
- Telemetry sampling and publication rates
- Authentication and certificate provisioning
- Whether RTS/CTS is enabled after initial UART testing
- Host-controlled ESP32 reset or boot-mode control
- STM32 firmware delivery through the ESP32