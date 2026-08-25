# Solar Project 200W - Handoff

Latest active handoff: [SOLAR_HANDOFF_2026-08-08.md](SOLAR_HANDOFF_2026-08-08.md)

## Current status

- Rev 1 is the active schematic lineage for board work.
- An ESP32-C3 development module has been added as a 5 V-powered Wi-Fi coprocessor.
- STM32 retains all charger, PWM, protection, limit, and fault authority.
- UART TX/RX is the interprocessor data path; RTS/CTS remains optional.
- The Wi-Fi/protocol baseline is recorded in docs/wifi-coprocessor-architecture.md and DEC-015.
- The checked-in ERC report is clean but dated 2026-08-04; regenerate it against the current schematic.

## Locked Baselines

- LM74502H plus back-to-back N-MOS input protection remains retained.
- ESP32 module power is `CTRL_5V` into its `5V` pin; its exposed `3.3V` pin is unused.
- U5 owns the separate `CTRL_3V3` rail.
- ESP32 owns networking, provisioning, telemetry transport, and its own OTA only.
- C52 voltage-rating margin remains an open pre-freeze item.

## Next session objective

Freeze and verify the STM32-to-ESP32 integration before PCB routing:
1. Run fresh ERC on the Rev 1 schematic.
2. Verify D10 polarity and the complete `CTRL_5V`/USB current budget.
3. Confirm UART pin mapping, TX/RX crossing, and whether RTS/CTS is enabled.
4. Decide whether STM32-controlled ESP32 reset/boot signals are required.
5. Define the first UART packet and MQTT telemetry schemas.
6. Add ESP32 antenna keepout checks to the PCB closure checklist.

## Primary evidence files

- [hardware/kicad/solar-project/Solar Project.kicad_sch](hardware/kicad/solar-project/Solar%20Project.kicad_sch)
- [hardware/kicad/solar-project/Solar Project Rev1.kicad_sch](hardware/kicad/solar-project/Solar%20Project%20Rev1.kicad_sch)
- [hardware/kicad/solar-project/Solar Project Rev0.kicad_sch](hardware/kicad/solar-project/Solar%20Project%20Rev0.kicad_sch)
- [hardware/kicad/solar-project/Solar Project.net](hardware/kicad/solar-project/Solar%20Project.net)
- [hardware/kicad/solar-project/ERC.rpt](hardware/kicad/solar-project/ERC.rpt)
- [hardware/mppt/schematic-notes.md](hardware/mppt/schematic-notes.md)
- [docs/rev-one-delta-plan-2026-08-04.md](docs/rev-one-delta-plan-2026-08-04.md)
- [docs/test/rev-one-erc-drc-closure-checklist.md](docs/test/rev-one-erc-drc-closure-checklist.md)
- [docs/wifi-coprocessor-architecture.md](docs/wifi-coprocessor-architecture.md)
- [SOLAR_HANDOFF_2026-08-08.md](SOLAR_HANDOFF_2026-08-08.md)
- [SOLAR_HANDOFF_2026-08-04.md](SOLAR_HANDOFF_2026-08-04.md)

## Note

Historical session details remain in dated SOLAR_HANDOFF files and project notes; this file stays intentionally short to reduce context load for the next chat.
