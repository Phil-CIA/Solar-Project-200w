# Solar Project 200W - Handoff

Latest active handoff: [SOLAR_HANDOFF_2026-07-13.md](SOLAR_HANDOFF_2026-07-13.md)

Session status: Phase 1 implementation refinement. Rev 0 scope and STM32G431 controller baseline are locked; pre-routing blockers remain open.

## Current Objective At Stop

Resolve pre-routing blockers in the schematic (sense-net collisions and fault-net ownership), then re-run ERC and capture clean status.

## Verified State At Stop

- Rev boundary is explicitly documented:
  - Rev 0: asynchronous buck baseline
  - Rev 1: synchronous converter + dedicated gate-driver upgrade path
- Controller baseline for Rev 0 is locked:
  - DEC-005: STM32G431 selected
- New local planning/validation artifacts are present for next step execution:
  - `hardware/mppt/rev0-component-shortlist-2026-07-10.md`
  - `hardware/mppt/pre-routing-blockers-2026-07-10.md`
- Active KiCad project remains under:
  - `hardware/kicad/solar-project/`

## Next Session Priority Order

1. Review `SOLAR_HANDOFF_2026-07-13.md` and confirm local file state.
2. Resolve `SENSE_PV_I` and `SENSE_BAT_I` net-collision issues in schematic.
3. Resolve `FAULT_OCP`/`FAULT_OVP` ownership and dangling-label handling.
4. Re-run ERC and update `hardware/mppt/schematic-notes.md` with fresh results.
5. Commit docs and schematic changes in separate commits.

## First Actions Next Session

1. Read this file and `SOLAR_HANDOFF_2026-07-13.md`.
2. Run `git status --short` and verify modified/untracked files before edits.
3. Open `hardware/kicad/solar-project/Solar Project.kicad_sch` and fix blocker nets first.
4. Keep `_autosave-Solar Project.kicad_sch` out of commits.
