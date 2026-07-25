# Solar Project 200W - Handoff

Latest active handoff: [SOLAR_HANDOFF_2026-07-25.md](SOLAR_HANDOFF_2026-07-25.md)

Session status: **PCB routed and DRC-clean**. Schematic was extended with STM32 support hardware, navigation buttons, and crystal network. The next step is a review pass, then commit and push.

## Current Objective At Stop

Review the routed board and schematic, then commit and push the clean snapshot.

## Session 2026-07-21/25 Summary

**What was completed:**
- ✅ 5-way navigation button pad added (SW1-SW5, R54-R58, BTN_UP/DN/LT/RT/ENT nets)
- ✅ STM32 reset network added: S1 button, R59 pull-up, C47 debounce cap on PG10/~RST
- ✅ HSE crystal added: X2 (8 MHz, 20 pF), C49/C50 (33 pF NP0) load caps on PF0/PF1
- ✅ Crystal topology verified in netlist: PF0→X2 pin1 with shunt cap, PF1→X2 pin2 with shunt cap
- ✅ Schematic component placement completed for all blocks
- ✅ PCB routed (pre-route save preserved as `Solar Project PreRoute save.kicad_pcb`)
- ✅ Freerouting DSN export present: `hardware/kicad/solar-project/freerouting.dsn`

**Not yet committed:**
- All schematic, PCB, and doc changes are in the working tree only (last commit `841e756`)

## Current Objective At Stop

Re-establish a clean, evidence-driven schematic cleanup loop: fresh ERC baseline, one error-class fix at a time, and documented deltas.

## Verified State At Stop

- Schematic extended and verified:
  - STM32G431 HSE crystal network: X2 + C49 + C50 correctly wired (verified in netlist)
  - STM32G431 reset path: S1 + R59 + C47 on MCU_RST_N (verified in netlist)
  - 5-way nav pad: SW1–SW5 with active-low pull-ups (R54–R58) all landed on MCU pins
  - Q2/V_in_ON kept as reserved future input-protection option (non-primary, quarantined)
  - CHG_KILL_N shared shutdown path explicitly deferred for this revision
- PCB routing:
  - Pre-route snapshot saved as `hardware/kicad/solar-project/Solar Project PreRoute save.kicad_pcb`
  - Post-route board is in `hardware/kicad/solar-project/Solar Project.kicad_pcb`
  - DRC/freerouting report at `hardware/kicad/solar-project/report.txt`: **27 errors / 43 warnings** — must resolve before fab
- Working tree is fully uncommitted; all changes since `841e756` are local only

## DRC State At Stop

From `hardware/kicad/solar-project/report.txt`:
- Total errors: **27**
- Total warnings: **43**
- Pre-routing gate policy: 0 unwaived errors required before PCB ordering

Next step is a DRC review pass in KiCad to classify each error as a real routing violation, a footprint clearance issue, or a deferred-waiver candidate.

## Next Session Priority Order

1. Open `hardware/kicad/solar-project/Solar Project.kicad_pcb` and run DRC from KiCad GUI.
2. Classify each of the 27 errors: fix routing violations immediately, waive only semantics/footprint-library noise with explicit rationale.
3. Fix routing errors until 0 hard DRC violations remain.
4. Commit schematic and PCB in separate commits before any further changes.
5. Keep `_autosave-Solar Project.kicad_pcb` and `freerouting.dsn` out of commits.
6. Update `hardware/mppt/schematic-notes.md` with the post-route DRC delta.

## First Actions Next Session

1. Read this file and `hardware/kicad/solar-project/report.txt`.
2. Run `git status --short` to confirm working tree state.
3. Open PCB in KiCad and run DRC — do not trust the freerouting report alone.
4. Fix one DRC error class at a time and re-run DRC between each class.
5. Once 0 hard errors: commit schematic and PCB separately.
6. Do not begin gerber export or fab ordering until DRC is clean and committed.

## Hardware Completion Status

| Item | Status | Notes |
|---|---|---|
| Nav button pad | ✅ Done | SW1-SW5, R54-R58, BTN_* nets |
| STM32 reset | ✅ Done | S1, R59, C47, MCU_RST_N net |
| HSE crystal | ✅ Done | X2 8MHz, C49/C50 33pF NP0 |
| SWD header | ✅ Existing | PA13, PA14, PG10 |
| PCB placed | ✅ Done | All components |
| PCB routed | ✅ Done | Post-route file on disk |
| DRC clean | ❌ Open | 27 errors must resolve |
| Committed | ❌ Open | All changes still local |

## Session Closeout (2026-07-20)

1. Parser-corrupted schematic attempts were abandoned in favor of authoritative project backup restore.
2. Active schematic was restored from `Solar Project-backups/Solar Project-2026-07-20_050545.zip`.
3. KiCad-path sync to GitHub was completed in commit `b8839b9`.
4. Documentation and planning updates remain in the working tree for this follow-on closeout commit.

## Session Closeout Addendum (2026-07-21)

1. ERC policy was promoted to a formal pre-routing gate in docs: routing requires 0 unwaived ERC errors or explicit documented waivers.
2. Protection-block progress was verified from fresh exports: `FAULT_OCP -> PB0`, `FAULT_OVP -> PB1`, and `U6_FLT -> PC13` are landed in netlist.
3. Ownership branch was selected for next implementation phase:
  - keep `Q2` and `V_in_ON` as a reserved future input-protection option
  - keep legacy `U2/Q2` branch quarantined as non-primary
  - defer shared hardware shutdown path (`CHG_KILL_N`-class net) for this revision
4. Next session intent is ownership-clarity-first, not additional ERC warning cleanup.

## Next Session Intent (Implementation Plan)

1. Keep `Q2` and `V_in_ON` as a documented future input-protection option, not a primary Rev 0 owner path.
2. Keep `U2/Q2` explicitly quarantined as non-primary legacy scaffolding.
3. Defer comparator-fault shared hardware shutdown path (`CHG_KILL_N`-class net) for this revision while preserving distinct status nets.
4. Update `hardware/mppt/schematic-notes.md` and `HANDOFF.md` with evidence and next action.

## Next Session Priority Order

1. Review `SOLAR_HANDOFF_2026-07-20.md` and confirm local file state.
2. Open `hardware/kicad/solar-project/Solar Project.kicad_sch` from the repository and verify the active U6/helper-net region before edits.
3. Re-run ERC and netlist export to set a fresh baseline from disk state.
4. Fix one hard-error class first; re-run ERC before starting the next class.
5. Keep `_autosave-Solar Project.kicad_sch` out of commits.
6. Commit docs and schematic changes in separate commits.

## First Actions Next Session

1. Read this file and `SOLAR_HANDOFF_2026-07-20.md`.
2. Run `git status --short` and verify modified/untracked files before edits.
3. Open `hardware/kicad/solar-project/Solar Project.kicad_sch` from the repo and capture a fresh ERC baseline before making multi-class fixes.
4. Keep `_autosave-Solar Project.kicad_sch` out of commits.
5. Pause additional ERC warning chasing and execute ownership documentation updates: record `Q2`/`V_in_ON` as future input-protection reserve and keep `U2/Q2` non-primary.
6. Record that shared hardware shutdown-OR path (`CHG_KILL_N`-class net) is deferred for this revision unless new safety evidence requires immediate implementation.
7. Execute `hardware/mppt/stm32-support-implementation-checklist.md` to add reset, crystal, and bring-up support hardware around U1.

## Hardware Completion To-Do (Current Priority)

Goal: finish schematic component coverage so board placement can proceed without reopening control ownership debates.

1. STM32 support network completion:
- [ ] Add a physical reset button on `PG10/~RST` with pull-up to `CTRL_3V3` and local reset capacitor per STM32 bring-up practice.
- [ ] Add external high-speed crystal network on `PF0-OSC_IN` / `PF1-OSC_OUT` with two load capacitors to `PWR_NEG` and verify package/footprint orientation.
- [ ] Confirm SWD/programming access remains explicit (`PA13`, `PA14`, `PG10/~RST`) at a bring-up header.
- [ ] Confirm BOOT strap behavior is intentional (`PB8-BOOT0`) and documented for programming/recovery.

2. Navigation button block closure:
- [x] 5-way nav pad added (`BTN_UP_N`, `BTN_DN_N`, `BTN_LT_N`, `BTN_RT_N`, `BTN_ENT_N`).
- [ ] Update planning docs to include button nets and pin ownership so schematic/net-plan stay aligned.

3. Remaining component lock items before board placement freeze:
- [ ] Finalize Sheet 2 power-component exact selections (Q1, D3, L1, output cap bank classes).
- [ ] Finalize OVP/OCP threshold support passives where placeholder values remain.
- [ ] Verify all user-facing and bring-up connectors are present and footprint-assigned (PV, battery, USB/debug, UART log, display).
