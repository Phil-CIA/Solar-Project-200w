# Solar Project 200W - Handoff

Latest active handoff: [SOLAR_HANDOFF_2026-09-12.md](SOLAR_HANDOFF_2026-09-12.md)

## Current status

- Gate 2 & Gate 3 are complete: Schematic corrections, footprint verification, and full BOM alignment are closed with **0 ERC Errors**.
- All 172 components in `Solar Project Rev1.kicad_sch` are verified against physical packages and mapped to authoritative LCSC part numbers in `hardware/kicad/solar-project/bom/Rev1_BOM.csv`.
- Netlist `Solar Project Rev1.net` is synchronized.
- Ready for **Gate 4 (PCB Layout Sync & Routing)**.

## Locked Baselines

- Retain one LM51772 four-switch conversion stage and rerate it for Rev 1.
- LM74502H plus back-to-back N-MOS input protection (BSC040N10NS5) remains retained.
- Protected battery power is the normal runtime control source and startup prerequisite.
- STM32G431CBU6 retains charger supervision, safety policy, and telemetry ownership.
- Hardware inhibit Q7 (2N7002K) positively clamps `EN_UVLO` low during startup/boot.
- 4-pad Kelvin shunts (R16, R22) are wired for pure differential current sensing.

## Next session objective

Synchronize the PCB layout (`Solar Project Rev1.kicad_pcb`) from the verified schematic (`F8`), verify component footprints on the layout canvas, and begin power-stage component placement and routing.

Full detail: [SOLAR_HANDOFF_2026-09-12.md](SOLAR_HANDOFF_2026-09-12.md)

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
