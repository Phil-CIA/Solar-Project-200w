# ESP32-C3 Wi-Fi Provisioning (ESP-IDF)

This project is intentionally isolated from STM32 charger/safety control code.

Implemented scope:
- First-boot setup AP + local web form for SSID/password
- Credential validation and storage in NVS
- Boot reconnect using saved credentials
- Router retries never stop: exponential backoff 1 s → 60 s between attempts, including after a drop once connected
- Setup AP (`SolarSetup-XXXXXX`, http://192.168.4.1/) runs alongside the station (APSTA), so router retries continue while it is open. It opens when:
  - no credentials are saved
  - the router has been unreachable for 5 minutes (closes automatically once the router connects)
  - the module BOOT button (GPIO9, unused on Rev1) is held for 3 s after boot. It stays open until new credentials are saved, or for 10 minutes while connected.

Out of scope in this project:
- STM32 UART control path
- MQTT/HTTPS cloud features
- Remote commands
- OTA updates

Build (ESP-IDF environment required):

```powershell
idf.py set-target esp32c3
idf.py build
```

Boot/reset notes:
- On ESP32-C3 USB Serial/JTAG, pressing RESET briefly disconnects and re-enumerates the COM port; this is expected USB behavior and firmware cannot prevent it. Use a terminal that auto-reopens the port (or an external USB-UART on U0TXD/U0RXD for a port that never drops).
- At boot the app waits up to ~3 s for the USB host, then 1.5 s for the terminal to reopen, and logs `===== BOOT: reset_reason=... =====`. ROM/bootloader lines before that are still lost over native USB.
- The console is on UART0 (GPIO21 TX, 115200) with USB Serial/JTAG as a secondary mirror. On Rev1, GPIO21 goes to STM32 PB11 (USART3_RX). STM32 firmware build `u6q5q6-10s-esplog` relays it to its PC UART with an `ESP> ` prefix (`ESPLOG ON|OFF`), so the full ROM/bootloader/app log is visible across a physical RESET.
- If logs show `Guru Meditation Error ... Stack protection fault` in task `main`, rebuild and flash the latest image. Scan diagnostics now use heap allocation to avoid main-task stack overrun.
