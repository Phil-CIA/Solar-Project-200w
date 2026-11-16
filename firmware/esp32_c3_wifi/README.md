# ESP32-C3 Wi-Fi Provisioning (ESP-IDF)

This project is intentionally isolated from STM32 charger/safety control code.

Implemented scope:
- First-boot setup AP + local web form for SSID/password
- Credential validation and storage in NVS
- Boot reconnect using saved credentials
- Router retries never stop: exponential backoff 1 s → 60 s between attempts, including after a drop once connected
- Setup AP (`SolarSetup-XXXXXX`, WPA2, http://192.168.4.1/) runs alongside the station (APSTA), so router retries continue while it is open. Its 12-character password is generated randomly once per boot and logged with the SSID each time the AP opens (`Setup AP open (...): SSID=... password=...`), on USB and on the STM32 COM relay. It opens when:
  - no credentials are saved
  - the router has been unreachable for 5 minutes (closes automatically once the router connects)
  - the module BOOT button (GPIO9, unused on Rev1) is held for 3 s after boot. It stays open until new credentials are saved, or for 10 minutes while connected.
- The setup page and its save and clear actions require browser sign-in with username `setup` and the same setup-AP password. Bench tests rejected unauthenticated and wrong-password requests, and a phone signed in using a temporary test password. A phone later joined the AP using the normal password and reached the sign-in prompt; successful browser sign-in with that password and unauthorized `/clear` rejection remain untested.
- The portal is also network-reachable via the ESP's router-network address while the AP is open. HTTP Basic authentication does not encrypt credentials; connect through the WPA2 setup AP at `http://192.168.4.1/` and use only a trusted, isolated bench network.

Credentials are stored in NVS namespace `wifi_cfg`. The Rev2-consolidation image `b62498e` used `wifi_config`; credentials are not migrated, so a board flashed with that image must be provisioned again.

The STM32 remains the sole owner of charging and safety control.

Wi-Fi status (connected ESP reports, an OLED RSSI reading and a green D11 flash observed on Rev1; other states and LED timing remain untested):
- The ESP emits `WIFI_STATUS <state> <RSSI|NA>` on its existing console about once per second. States are `S` (searching), `A` (setup authentication or credentials pending), `C` (router connected), and `E` (setup server error). Router RSSI is reported in dBm only while connected.
- With STM32 build `u6q5q6-10s-wifi1`, the OLED shows router RSSI or `RSSI -- DBM` when unavailable or stale. The code drives D8 as a heartbeat, but the user observed it solid green rather than blinking; investigation is deferred. D11 blinks blue while searching (500 ms on/off), amber while setup authentication or credentials are pending (150 ms on/off), and green when connected (150 ms on, 2850 ms off). D11 is off when status is unknown or reports a setup-server error.
- This uses the existing one-way ESP console relay, not the planned framed STM32 data link. `ESPLOG OFF` hides relayed console lines but status processing continues. Relay/status updates pause while U6 is armed or running; after a pause or receive overflow, buffered status is discarded and a fresh report is required. The U6 test screen remains unchanged.

Out of scope in this project:
- STM32 UART control path
- MQTT/HTTPS cloud features
- Remote commands
- OTA updates

Build (ESP-IDF environment required):

```powershell
cd firmware\esp32_c3_wifi
idf.py set-target esp32c3
idf.py build
.\flash-app.ps1 -Port COM9 -ExpectedMac 'aa:bb:cc:dd:ee:ff' -PlanOnly
```

The MAC above is an example, not the board's MAC. `-PlanOnly` checks the image and prints the proposed operation without accessing the device. The helper has been tested in PowerShell 7; script execution is subject to the computer's execution policy. To use it, activate the ESP-IDF Python environment and omit `-PlanOnly`; use the actual ESP32-C3 USB Serial/JTAG port and independently confirmed MAC. `-BuildDir` can select an alternate ESP-IDF build directory.

The helper first requires `CHECK` before resetting and identifying the ESP, then verifies its MAC and compares its partition table with the build. It requires `FLASH` before writing and verifying only the factory application partition. One app-only flash on Rev1 passed these checks; other hardware has not been qualified. It does not erase the whole flash or intentionally erase NVS. A compatible bootloader must already be installed; a different partition layout is rejected. The application itself still erases NVS if initialization reports no free NVS pages or an incompatible NVS version, so credential retention is not guaranteed. Before hardware access, confirm the converter is inhibited and PV, battery, and load are disconnected; follow the bench supply checks separately.

`sdkconfig` is generated and gitignored; delete it after changing `sdkconfig.defaults` so the new defaults apply.

Boot/reset notes:
- On ESP32-C3 USB Serial/JTAG, pressing RESET briefly disconnects and re-enumerates the COM port; this is expected USB behavior and firmware cannot prevent it. Use a terminal that auto-reopens the port (or an external USB-UART on U0TXD/U0RXD for a port that never drops).
- At boot the app waits up to ~3 s for the USB host, then 1.5 s for the terminal to reopen, and logs `===== BOOT: reset_reason=... =====`. ROM/bootloader lines before that are still lost over native USB.
- The console is on UART0 (GPIO21 TX, 115200) with USB Serial/JTAG as a secondary mirror. On Rev1, GPIO21 goes to STM32 PB11 (USART3_RX). STM32 firmware builds `u6q5q6-10s-esplog` and `u6q5q6-10s-wifi1` relay it to the PC UART with an `ESP> ` prefix (`ESPLOG ON|OFF`), so the ROM/bootloader/app log is visible across a physical RESET.
- If logs show `Guru Meditation Error ... Stack protection fault` in task `main`, rebuild and flash the latest image. Scan diagnostics now use heap allocation to avoid main-task stack overrun.
