Continue the Solar Project 200W repository from the current handoff state.

Known state:
- Rev boundary is locked:
  - Rev 0: asynchronous buck baseline
  - Rev 1: synchronous converter + dedicated gate-driver upgrade path
- Decision and Sheet 2 implementation docs are already updated for this scope.
- Next objective is practical KiCad schematic progress, not architecture expansion.

Tasks:
1) Read these first:
   - `HANDOFF.md`
   - `docs/SOLAR_HANDOFF_2026-07-08.md`
   - `hardware/mppt/sheet2-component-picks.md`
   - `hardware/mppt/buck-power-stage-wiring-map.md`
   - `hardware/mppt/kicad-implementation-checklist.md`
2) Open `hardware/kicad/solar-project/Solar Project.kicad_sch` and place Sheet 2 symbols in order:
   - Q1, D3, L1
   - C4/C5
   - C6/C7
   - R1/C8
3) Wire Sheet 2 nets exactly per map:
   - PV_IN_POS
   - PV_IN_NEG
   - MPPT_SW_NODE
   - CHG_OUT_POS
   - CHG_OUT_NEG
4) Add sheet notes:
   - ASSUME async freewheel path
   - DECISION Rev 0 async lock
   - VERIFY margin checks for Q1 and D3
5) Perform save-point protocol updates:
   - update `hardware/mppt/schematic-notes.md`
   - update `docs/decisions-log.md` only if a new trade-off is introduced

Keep output concise, implementation-focused, and fully aligned to the locked Rev 0 scope.
