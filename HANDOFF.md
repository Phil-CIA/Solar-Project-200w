# Solar Project 200W - Handoff

Latest active handoff: [SOLAR_HANDOFF_2026-08-26.md](SOLAR_HANDOFF_2026-08-26.md)

## Current status

- Rev 0 boards are built but not yet powered. Bring-up is blocked pending replacement MCUs.
- Root cause of the U1 `CTRL_3V3` short is confirmed (DEC-018): the board was routed against the `STM32G431CBU6` symbol while `STM32F411CEU6` parts were fitted, causing hard supply-pin conflicts through the die.
- Two `STM32G473CEU6` are on order to recover the existing Rev 0 boards; verified pin-identical to the routed G4 map.
- Rev 1 will carry a properly implemented `STM32F411` migration (DEC-020).
- Rev 0 I2C is swapped at the MCU (DEC-019); use software bit-banged I2C.
- M1-M4 are unpopulated due to a separate footprint mismatch; inverter/AC testing remains blocked.
- A mandatory Part Substitution Gate now exists in the KiCad implementation checklist.

## Locked Baselines

- LM74502H plus back-to-back N-MOS input protection remains retained.
- ESP32 module power is `CTRL_5V` into its `5V` pin; its exposed `3.3V` pin is unused.
- U5 owns the separate `CTRL_3V3` rail.
- ESP32 owns networking, provisioning, telemetry transport, and its own OTA only.
- C52 voltage-rating margin remains an open pre-freeze item.

## Next session objective

Pick the track that matches parts availability:

**Track A - Rev 0 recovery (parts arrived):** fit one `STM32G473CEU6`, verify `CTRL_3V3`-to-`PWR_NEG` is high before power, then run Stage A/B of the Rev 0 bring-up addendum.

**Track B - Rev 1 design (parts delayed):** replace the MCU symbol with KiCad's official `STM32F411CEUx`, re-assign all nets under the Part Substitution Gate, fix the I2C assignment, and correct the M1-M4 footprint.

Full detail: [SOLAR_HANDOFF_2026-08-26.md](SOLAR_HANDOFF_2026-08-26.md)

## Primary evidence files

- [hardware/kicad/solar-project/REVISION-INDEX.md](hardware/kicad/solar-project/REVISION-INDEX.md)
- [hardware/kicad/solar-project/Solar Project Rev1.kicad_sch](hardware/kicad/solar-project/Solar%20Project%20Rev1.kicad_sch)
- [hardware/kicad/solar-project/Solar Project Rev0.kicad_sch](hardware/kicad/solar-project/Solar%20Project%20Rev0.kicad_sch)
- [hardware/kicad/solar-project/ERC.rpt](hardware/kicad/solar-project/ERC.rpt)
- [docs/test/rev0-bringup-addendum.md](docs/test/rev0-bringup-addendum.md)
- [docs/test/bringup-checklist.md](docs/test/bringup-checklist.md)
- [hardware/mppt/kicad-implementation-checklist.md](hardware/mppt/kicad-implementation-checklist.md)
- [hardware/mppt/schematic-notes.md](hardware/mppt/schematic-notes.md)
- [docs/rev-one-delta-plan-2026-08-04.md](docs/rev-one-delta-plan-2026-08-04.md)
- [docs/test/rev-one-erc-drc-closure-checklist.md](docs/test/rev-one-erc-drc-closure-checklist.md)
- [docs/wifi-coprocessor-architecture.md](docs/wifi-coprocessor-architecture.md)
- [SOLAR_HANDOFF_2026-08-26.md](SOLAR_HANDOFF_2026-08-26.md)
- [SOLAR_HANDOFF_2026-08-08.md](SOLAR_HANDOFF_2026-08-08.md)

Note: the as-manufactured Rev 0 design files live outside this repository at
`C:\Users\forch\OneDrive\JLCPCB files\Solar Charger\Solar Project Rev0 - Manufactured First Build`.

## Note

Historical session details remain in dated SOLAR_HANDOFF files and project notes; this file stays intentionally short to reduce context load for the next chat.
