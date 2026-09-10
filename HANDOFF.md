# Solar Project 200W - Handoff

Latest active handoff: [SOLAR_HANDOFF_2026-09-10.md](SOLAR_HANDOFF_2026-09-10.md)

## Current status

- Rev 0 first switching did not pass; U6 entered short UVLO dropout/retry bursts before a visible `SS_ATRK` ramp.
- Preserve the Rev 0 bench stop state in `SOLAR_HANDOFF_2026-09-08.md`; no hardware work was performed in the September 9 design session.
- Rev 1 is now a targeted Rev 0 reconfiguration, not a clean-sheet redesign (DEC-024).
- DEC-023 sets 125 W continuous output, 150 W maximum PV operation, 8 A to 10 A nominal charging, and an approximately 12 A hardware protection ceiling.
- DEC-025 through DEC-029 define battery-first startup, positive converter inhibit, PV qualification, controlled current ramp, MPPT hierarchy, and weak-source recovery.
- The manufactured Rev 0 netlist confirms `V_out -> R16 -> BAT_BUS_POS`; it also shows both F2 pins on `BAT_BUS_POS`, bypassing the intended fuse boundary.

## Locked Baselines

- Retain one LM51772 four-switch conversion stage and rerate it for Rev 1.
- LM74502H plus back-to-back N-MOS input protection remains retained.
- Protected battery power is the normal runtime control source and startup prerequisite.
- STM32 retains charger supervision, safety policy, and telemetry ownership.
- USB service power must not independently authorize power-stage switching.
- ESP32 module power is `CTRL_5V` into its `5V` pin; its exposed `3.3V` pin is unused.
- ESP32 owns networking, provisioning, telemetry transport, and its own OTA only.
- C52 voltage-rating margin remains an open pre-freeze item.

## Next session objective

Continue the converter rebaseline from the 2026-09-10 handoff. First confirm the open KiCad schematic has been saved, then review the paired R3/R18 change for the 14.6 V output target. Follow with Q4 gate control, capacitor evidence, shunt Kelvin mapping, F2, and fresh ERC/DRC.

Full detail: [SOLAR_HANDOFF_2026-09-10.md](SOLAR_HANDOFF_2026-09-10.md)

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
- [SOLAR_HANDOFF_2026-09-09.md](SOLAR_HANDOFF_2026-09-09.md)
- [SOLAR_HANDOFF_2026-09-10.md](SOLAR_HANDOFF_2026-09-10.md)
- [SOLAR_HANDOFF_2026-09-08.md](SOLAR_HANDOFF_2026-09-08.md)
- [SOLAR_HANDOFF_2026-08-26.md](SOLAR_HANDOFF_2026-08-26.md)

## Note

Historical session details remain in dated SOLAR_HANDOFF files and project notes; this file stays intentionally short to reduce context load for the next chat.
