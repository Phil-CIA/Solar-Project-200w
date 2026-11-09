# ESP32-C3 Wi-Fi Provisioning (ESP-IDF)

This project is intentionally isolated from STM32 charger/safety control code.

Implemented scope:
- First-boot setup AP + local web form for SSID/password
- Credential validation and storage in NVS
- Boot reconnect using saved credentials
- Fallback to setup AP after repeated connection failures
- Setup AP disabled after station gets an IP address

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
