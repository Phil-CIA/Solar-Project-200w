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

