# Wi-Fi Coprocessor Architecture

Last updated: 2026-08-08  
Status: Accepted Rev 1 baseline  
Decision: DEC-015

## Initial Firmware Scope

The first ESP32-C3 firmware stage is in `firmware/esp32_c3_wifi/`. It implements temporary setup-AP provisioning, credential storage in NVS, Wi-Fi association, retry, and setup-AP fallback. It does not yet implement the STM32 UART protocol, MQTT/HTTPS telemetry, remote commands, or OTA.

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

STM32 firmware updates remain through SWD for the initial implementation. ESP32-assisted STM32 updates are deferred until the communications protocol is proven. The draft delivery plan for [#24](https://github.com/Phil-CIA/Solar-Project-200w/issues/24) is in [esp32-stm32-firmware-delivery-plan.md](esp32-stm32-firmware-delivery-plan.md); it records that Rev1 PA11/PA12 (`RXD_ESP`/`TXD_ESP`) have no USART TX/RX function, so the Rev1 data link would use USART3 on PB10/PB11.

## Hardware Interface

These bullets record the historical Rev1 baseline. Its `RTS_ESP`/`CTS_ESP` labels actually connect ESP UART0 to STM32 USART3, not hardware flow control; the proposed PA11/PA12 data pair has no USART TX/RX capability. Do not implement the original flow-control assumption.

- Power the ESP32 development module through its `5V` pin from `CTRL_5V`.
- Leave the module's exposed `3.3V` pin unpowered; the module uses its onboard regulator.
- Use common `PWR_NEG` reference.
- Use UART `TXD_ESP` and `RXD_ESP` for the data channel.
- Keep `RTS_ESP` and `CTS_ESP` routed as optional hardware flow control.
- Initial UART rate: 115200 baud; raise only after validation.
- Preserve the module antenna keepout in PCB placement and copper pours.

### Rev2 schematic selection (issue #33)

The [Rev2 handoff](esp32-stm32-firmware-delivery-plan.md#rev2-schematic-only-handoff-issue-33) supersedes the historical transport allocation: two-wire data on STM32 USART2 PB3/PB4 AF7 to ESP GPIO0/GPIO1, with separate UART0 console GPIO20/GPIO21 to USART3 PB10/PB11. No RTS/CTS. PB10 stays high-impedance until firmware intentionally uses it. SW2/SW4 move to PA11/PA12; SWD is retained, full JTAG/SWO is not.

U15 TXU0204 provides power-off-safe buffering for all four wires. The module still receives CTRL_5V, but its regulator output powers only the ESP-side buffer/bias domain; it is not joined to CTRL_3V3. Manual on-module reset/BOOT is the selected recovery method; automatic EN control is deferred. ESP has no connection to STM32 reset/BOOT0 or direct charger-control outputs.

Only the schematic and footprint/library assignments implement this selection. The recovered PCB is frozen pending a later synchronization/layout step; no Rev2 firmware or hardware qualification is implied.

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
- Host-controlled ESP32 reset or boot-mode control (explicitly deferred for Rev2; manual buttons retained)
- STM32 firmware delivery through the ESP32