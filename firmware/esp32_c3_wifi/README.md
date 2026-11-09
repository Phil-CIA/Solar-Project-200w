# ESP32-C3 Wi-Fi Provisioning Firmware

This ESP-IDF application implements the initial Wi-Fi setup flow from the accepted coprocessor architecture:

- On first boot, start a temporary WPA2 setup access point and a local setup page.
- Save the supplied SSID/password in NVS and join the network.
- Stop the setup access point after the station receives an IP address.
- Retry saved network credentials at boot; reopen the setup access point after repeated connection failures.

The setup SSID and a randomly generated password are printed to the USB Serial/JTAG console. Connect to that network and open `http://192.168.4.1/`. Open networks are supported; otherwise enter a WPA/WPA2 password of 8-63 characters.

This first stage does not implement STM32 UART messages, MQTT/HTTPS telemetry, remote commands, or OTA. The STM32 remains the sole owner of charging and safety control. To reset stored credentials during development, erase flash and reflash the application.

## Consolidated implementation

The Rev2 consolidation retains the bring-up branch's `b62498e` implementation: a WPA2 setup AP with a random password, explicit ESP-IDF component dependencies, and a persistent event loop for reconnect/fallback after initial setup. The alternative implementation from local-main commit `61886c3` is preserved in Git history, but its open setup AP and web credential-clear endpoint are not carried into the active source.

The implementations use different NVS namespaces (`wifi_config` here versus `wifi_cfg` in the alternative). Credentials are not migrated automatically: after switching from the alternative image, reconnect to the protected setup AP and provision again. The old namespace is left untouched; the erase-flash command below removes all saved flash data, not just Wi-Fi credentials.

## Build and flash

Use an ESP-IDF installation with ESP32-C3 support:

```powershell
cd firmware\esp32_c3_wifi
idf.py set-target esp32c3
idf.py build
idf.py -p COMx flash monitor
```

Replace `COMx` with the ESP32-C3 board's USB Serial/JTAG port. The project selects USB Serial/JTAG for logs so the ESP32's UART pins remain available for the later STM32 link.

To erase credentials and start provisioning again:

```powershell
idf.py -p COMx erase-flash flash monitor
```
