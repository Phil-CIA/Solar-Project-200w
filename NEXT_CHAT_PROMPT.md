Continue the Solar Project 200W Rev 0 bring-up from `SOLAR_HANDOFF_2026-08-31.md`.

First capture the overnight stability result before resetting, power-cycling, or flashing: OLED `UP`, `N`, `E`, temperature/humidity, display condition, RGB heartbeat condition, and whether verified `COM5` still answers `status` and `aht20`.

If the overnight result passes, resume H3 at only the planned LM51772 6.5 V / 20 mA static point. M1-M4 remain unpopulated; converter switching is prohibited.

Keep PV, battery, charger output, inverter, AC wiring, and loads disconnected except for the explicitly bounded bench source. Keep `PA8`, `PB5`, and `PA5` low. Never drive `PB11`. Before any flash, verify ST-LINK `B55B5A1A0000000020AFF501`, Device ID `0x469`, STM32G47x/G48x/G414, 512 KB NVM, and expected target voltage; stop on any mismatch. Do not switch away from `COM5` opportunistically.

