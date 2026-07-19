# Solar Project 200W - Handoff (2026-07-14)

## Session Summary

**Date**: 2026-07-14  
**Phase**: Phase 1 implementation refinement (Rev 0)  
**Focus**: clear high-value schematic blockers, stabilize ERC workflow, and pivot back to DEC-013 control-supply design

---

## What Is Already Pushed To Remote

- Commit `728ef1a` remains on `main` and `origin/main`.
- DEC-005 is closed for Rev 0 with STM32G431 baseline in `docs/decisions-log.md`.

---

## Local (Uncommitted) State At Stop

From `git status --short` at stop:

- Modified: `docs/decisions-log.md`
- Modified: `docs/session-plan-2026-07-10.md`
- Modified: `hardware/kicad/solar-project/Solar Project.kicad_sch`
- Modified: `hardware/mppt/kicad-implementation-checklist.md`
- Modified: `hardware/mppt/net-plan.md`
- Modified: `hardware/mppt/schematic-notes.md`
- Modified: `hardware/mppt/sheet4-sensing-control-wiring-map.md`
- Untracked: `hardware/mppt/pre-routing-blockers-2026-07-10.md`
- Untracked: `hardware/mppt/rev0-component-shortlist-2026-07-10.md`

Important:
- The working tree includes intentional schematic and documentation edits.
- The recent `power:PWR_FLAG` experiment was rolled back because it destabilized KiCad/ERC when hand-edited into the schematic.
- Do not reintroduce manual `power:PWR_FLAG` symbol instances by text editing.

---

## Technical State Snapshot

### What Was Closed This Session

- Original pre-routing blocker set is closed in practical terms:
  - `SENSE_PV_I` collision with `CHG_OUT_POS` cleared.
  - `SENSE_BAT_I` collision with `BAT_BUS_POS` cleared.
  - Dangling `FAULT_OCP` / `FAULT_OVP` label condition cleared for this schematic phase.
- ERC workflow policy is now explicit:
  - fix high-value wiring issues immediately,
  - add flags/`no_connect` markers as you work,
  - defer low-value warning cleanup to end-of-phase rabbit-hole pass.
- KiCad/ERC is stable again after rollback of the manual `PWR_FLAG` insertion attempt.

### Current ERC State

Latest stable snapshot: `hardware/kicad/solar-project/ERC.rpt` at `2026-07-14T10:37:04-0500`

Current total:
- 4 errors
- 14 warnings

Remaining errors are intentionally deferred for later ERC cleanup:
1. `U1 VBAT` power-pin-driven
2. `U1 VSS` power-pin-driven
3. `U2 VIN` power-pin-driven
4. `U4 VIN` power-pin-driven

Interpretation:
- These are now treated as ERC power-authority semantics, not active wiring blockers for the immediate design step.

### DEC-013 / Converter State

DEC-013 remains the active design topic.

Current schematic state:
- `CTRL_SUPPLY_IN` now exists in the schematic as the post-diode control-supply boundary.
- `USB_5V_IN` and `BAT_BUS_POS` are now attached as explicit candidate inputs into the existing diode fan-in scaffold ahead of the control supply.
- U2 is still the **provisional live `CTRL_3V3` owner** in the drawn schematic.
- U4 now references `CTRL_SUPPLY_IN` as the **wide-input candidate path**, but is not yet the actual `CTRL_3V3` owner.

Open decision:
- `Q-004` in `docs/decisions-log.md`
- Decide whether Rev 0 keeps U2 as the actual 3.3 V converter or promotes U4 to the real wide-input control-supply owner.

---

## Best Restart Priority

Do **not** spend the next session on more ERC cleanup first.

Start here instead:
1. Read `SOLAR_HANDOFF_2026-07-14.md`.
2. Review `docs/decisions-log.md` for `DEC-013` and `Q-004`.
3. Review `hardware/mppt/schematic-notes.md` current DEC-013 notes.
4. Open `hardware/kicad/solar-project/Solar Project.kicad_sch` and inspect U2, U4, the diode fan-in, and the new `CTRL_SUPPLY_IN` / `USB_5V_IN` labels.
5. Decide the converter owner:
   - keep U2 as Rev 0 control-rail owner, or
   - promote U4 to the actual wide-input Rev 0 control-rail owner.
6. Only after that decision, update the schematic and notes to reflect a single-owner `CTRL_3V3` path.

---

## Recommended Next Session Objective

**Close Q-004 for DEC-013.**

Concretely:
- choose U2 vs U4 as the actual Rev 0 `CTRL_3V3` owner,
- align the schematic to that one-owner choice,
- document source-priority behavior for `CHG_OUT_POS`, `BAT_BUS_POS`, and `USB_5V_IN`,
- leave deferred ERC authority/library cleanup for the later rabbit-hole pass.

---

## Suggested Commit Plan (When Ready)

1. Docs/state commit:
   - `docs/decisions-log.md`
   - `hardware/mppt/schematic-notes.md`
   - `hardware/mppt/net-plan.md`
   - `hardware/mppt/sheet4-sensing-control-wiring-map.md`
   - `hardware/mppt/kicad-implementation-checklist.md`
   - `hardware/mppt/pre-routing-blockers-2026-07-10.md`
2. Schematic DEC-013 commit:
   - `hardware/kicad/solar-project/Solar Project.kicad_sch`
   - optional updated `hardware/kicad/solar-project/ERC.rpt`

---

## Closeout

This session should be treated as the ERC-stabilization and DEC-013 scaffold session. The next chat should start with converter-owner selection, not with more schematic housekeeping.