Continue the Solar Project 200W repository from the latest stale-session handoff and close the open pre-routing blockers first.

Known state:
- Rev 0 remains asynchronous buck.
- DEC-005 is closed: STM32G431 baseline for Rev 0.
- There are uncommitted local docs + schematic changes plus a KiCad autosave artifact.

Tasks:
1) Read these first:
   - `HANDOFF.md`
   - `SOLAR_HANDOFF_2026-07-13.md`
   - `hardware/mppt/pre-routing-blockers-2026-07-10.md`
   - `hardware/mppt/rev0-component-shortlist-2026-07-10.md`
   - `hardware/mppt/schematic-notes.md`
2) Run `git status --short` and confirm local file state before editing.
3) Open `hardware/kicad/solar-project/Solar Project.kicad_sch` and fix must-close blockers in order:
   - `SENSE_PV_I` collision with `CHG_OUT_POS`
   - `SENSE_BAT_I` collision with `BAT_BUS_POS`
   - dangling `FAULT_OCP` and `FAULT_OVP` ownership/termination
4) Re-run ERC and update `hardware/mppt/schematic-notes.md` with a dated result snapshot.
5) Stage commits in two chunks:
   - docs-only
   - schematic/ERC updates
6) Do not include `_autosave-Solar Project.kicad_sch` in commits.

Keep output concise, blocker-focused, and implementation-first.
