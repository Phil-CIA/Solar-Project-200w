Continue the Solar Project 200W repository from the 2026-08-08 Wi-Fi architecture handoff.

## Current priority

Freeze and verify the Rev 1 STM32-to-ESP32 Wi-Fi coprocessor integration before PCB routing.

## State to assume

- ESP32-C3 development module is intentionally powered from `CTRL_5V` through its `5V` pin.
- The module's exposed `3.3V` pin remains unused; U5 separately owns `CTRL_3V3`.
- STM32 owns charging and safety; ESP32 is a network coprocessor per DEC-015.
- UART TX/RX is required and RTS/CTS is optional.
- The checked-in ERC report is dated 2026-08-04 and must be regenerated.
- D10 polarity, USB/`CTRL_5V` current budget, antenna keepout, reset/boot control, and C52 margin remain verification items.
- Working tree may contain local edits; avoid destructive cleanup.

## What to do first

1. Read HANDOFF.md, SOLAR_HANDOFF_2026-08-08.md, and docs/wifi-coprocessor-architecture.md.
2. Run git status --short and capture baseline.
3. Export fresh ERC evidence from Solar Project Rev1.kicad_sch before new edits.
4. Verify D10 direction and calculate the complete `CTRL_5V` current budget, including ESP32 Wi-Fi peaks.

## Session deliverables

1. Confirmed STM32 and ESP32 UART pin map with TX/RX crossing.
2. Decision on RTS/CTS and host-controlled reset/boot signals.
3. Initial COBS/CRC16 packet schema and MQTT telemetry schema.
4. Updated PCB checklist covering antenna keepout and ESP32 supply integrity.
5. Fresh ERC evidence and a list of remaining pre-freeze blockers.

## Constraints

1. Keep autosave and lock artifacts out of commits.
2. Do not move charger or safety authority into the ESP32.
3. Keep Rev One scope disciplined; log architecture changes before implementation.

