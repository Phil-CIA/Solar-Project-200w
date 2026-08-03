# Solar Project 200W - Handoff (2026-07-28)

## Session outcome

PV-input gate-drive blocker is functionally resolved in schematic and simulation.

- Topology migrated from legacy NMOS switch attempt to PMOS high-side path using Q4.
- Gate-drive behavior is now validated with ngspice as a real OFF/ON Vgs control window.
- Q4 candidate is selected: HSBA0119 (LCSC C53244070).

## What was verified

1. Netlist wiring for PMOS path is coherent:
- Q4 source on /PV_IN_POS_FUSED
- Q4 drain on PV_IN_POS
- D1 clamped gate-to-source
- Q2 pull-down control path via R40-R43 and V_in_ON

2. SPICE validation:
- OFF state keeps Vgs near 0 V and output off.
- ON state drives Vgs near -12 V clamp region and output tracks input.
- Evidence in spice results log.

3. Device selection decision:
- Selected Q4: HSBA0119, 100 V class, +/-20 V Vgs class, 23 mOhm typ Rds(on).
- First-pass thermal table added in schematic notes for 5 A / 8 A / 10 A corners.

## Open checks (next session)

1. Confirm Q2 Vds/transient margin at OFF-state stress corner.
2. Confirm final worst-case hot Rds(on) and thermal rise for HSBA0119 before procurement lock.
3. Run fresh ERC from KiCad GUI and capture new delta after Q4 migration.
4. Stage a clean commit set (schematic + docs + spice logs) without autosave/lock artifacts.

## Source-of-truth files

- HANDOFF overview: HANDOFF.md
- Schematic notes: hardware/mppt/schematic-notes.md
- Gate-drive validation log: hardware/kicad/solar-project/spice/pv_switch_gate_drive_results.txt
- Stress-check log: hardware/kicad/solar-project/spice/pv_switch_gate_drive_stress_results.txt
- Selected part shortlist: hardware/mppt/rev0-component-shortlist-2026-07-10.md

## First moves next session

1. Run git status --short.
2. Open Q4/Q2 region in Solar Project.kicad_sch and verify no accidental net label drift.
3. Run ERC in KiCad and export updated Solar Project.net.
4. If clean enough, prepare commit split:
- commit A: schematic/netlist changes
- commit B: notes/handoff updates
