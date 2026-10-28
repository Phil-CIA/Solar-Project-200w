# Solar Project 200W - Current Handoff

Updated: 2026-10-24

## Current state

- Rev 1 design closure and fabrication preparation are documented in [SOLAR_HANDOFF_2026-09-18.md](SOLAR_HANDOFF_2026-09-18.md) and [SOLAR_HANDOFF_2026-09-19.md](SOLAR_HANDOFF_2026-09-19.md). The September 19 handoff records that the Rev 1 PCB order was submitted.
- Only Rev 0 and Rev 1 exist. Rev 1 is the current production revision; do not refer to it as Rev 2.
- The Rev 1 production release sent to the manufacturer is the source of truth at `C:\Users\user\OneDrive\JLCPCB files\Solar Charger\Rev 1`.
- The release package contains `Kicad`, `BOM`, and `GERBERS` folders. Its PCB and Gerbers match the repository copies byte-for-byte, but the schematic, project file, and BOM do not. Treat the release package as authoritative for the board being built and bench-tested; reconcile repository copies deliberately before relying on them.
- KiCad lock files were present in the release `Kicad` folder during inspection. Do not edit or remove them unless KiCad is confirmed closed.
- The working tree was clean before this handoff refresh; the repository branch was at the same commit as `origin/main`.

## Before bench testing

1. Identify the physical board against the Rev 1 production release package and its Gerbers/BOM.
2. Use the release package as the reference for assembly and board-specific testing; do not assume repository schematic/BOM copies are identical.
3. Record bench setup, measurements, and any differences from the released Rev 1 design in a dated test record.

## Project references

- [Requirements](docs/requirements.md)
- [Architecture](docs/architecture.md)
- [Decisions log](docs/decisions-log.md)
- [Rev 1 closure plan](docs/rev-one-closure-plan-2026-09-16.md)
- [KiCad revision index](hardware/kicad/solar-project/REVISION-INDEX.md)

Historical session notes remain in the dated `SOLAR_HANDOFF_*.md` files.
