# Rev One Delta Plan (2026-08-04)

Owners: Phil + Copilot  
Status: Drafted from validated Rev 0 input-protection baseline

## Scope Guardrails

- Keep LM74502H plus back-to-back high-side N-MOS input path intact unless an explicit decision log says otherwise.
- Treat C52 voltage-rating margin review as the only open carry-over from Rev 0 closure.
- Do not reopen unrelated architecture branches in this pass.

## Rev One Delta Table By Schematic Sheet

| Sheet | Change Item | Priority | Owner | Intent | Evidence / Reference |
|---|---|---|---|---|---|
| Sheet 1 - PV Input / Protection | Re-label active schematic lineage so current design is Rev 1 and recovered JLC-era design is Rev 0 | Must-have | Phil | Prevent revision ambiguity before board updates | hardware/kicad/solar-project/Solar Project.kicad_sch, hardware/kicad/solar-project/Solar Project Rev1.kicad_sch, hardware/kicad/solar-project/Solar Project Rev0.kicad_sch |
| Sheet 1 - PV Input / Protection | Confirm C52 voltage-rating margin policy (keep vs raise) and document as freeze gate | Must-have | Phil | Close final protection-margin open item | hardware/kicad/solar-project/Solar Project.kicad_sch, hardware/mppt/pv-input-wiring-map.md |
| Sheet 1 - PV Input / Protection | Replace any residual legacy net labels tied to bench naming if they still appear after ERC export | Optional | Copilot | Improve netlist readability without functional change | hardware/kicad/solar-project/Solar Project.net |
| Sheet 2 - Buck / Main Power Stage | Keep current Rev 0 validated protection feed into power stage unchanged while routing updates proceed | Must-have | Phil | Avoid regressions while introducing board-level refinements | hardware/mppt/buck-power-stage-wiring-map.md |
| Sheet 2 - Buck / Main Power Stage | Optional cleanup of stale placeholder notes that imply alternate gate-drive ownership | Optional | Copilot | Keep documentation aligned with active CTRL_PWM_MAIN ownership | hardware/mppt/buck-power-stage-wiring-map.md, hardware/mppt/schematic-notes.md |
| Sheet 3 - Battery Interface | Re-verify boundary continuity (CHG_OUT_POS -> F2 -> BAT_BUS_POS and PWR_NEG return) after any board edits | Must-have | Phil | Ensure battery boundary remains stable in Rev 1 | hardware/mppt/sheet3-battery-interface-wiring-map.md |
| Sheet 3 - Battery Interface | Add any board-assembly convenience testpoint placement notes found during Rev 1 layout pass | Optional | Phil | Improve bring-up workflow | docs/test/bringup-checklist.md |
| Sheet 4 - Sensing / Control | Re-run fault-net and control-net continuity checks after Rev 1 relabel to guard against accidental disconnects | Must-have | Phil | Keep protection telemetry behavior intact | hardware/mppt/sheet4-sensing-control-wiring-map.md, hardware/kicad/solar-project/Solar Project.net |
| Sheet 4 - Sensing / Control | Optional canonical-name cleanup for any remaining non-canonical helper labels not tied to functional ambiguity | Optional | Copilot | Reduce naming debt without topology churn | hardware/mppt/schematic-notes.md |

## Approved Rev One Working References

- Schematic active file: hardware/kicad/solar-project/Solar Project.kicad_sch
- Board active file: hardware/kicad/solar-project/Solar Project.kicad_pcb
- Rev 1 archive copies: hardware/kicad/solar-project/Solar Project Rev1.kicad_sch and hardware/kicad/solar-project/Solar Project Rev1.kicad_pcb
- Rev 0 recovered baseline: hardware/kicad/solar-project/Solar Project Rev0.kicad_sch and hardware/kicad/solar-project/Solar Project Rev0.kicad_pcb

## Commit Split Plan

### Commit A: Schematic / Netlist Changes

Include only design-source and generated-evidence changes:

- hardware/kicad/solar-project/Solar Project.kicad_sch
- hardware/kicad/solar-project/Solar Project.kicad_pcb (if changed during Rev 1 edits)
- hardware/kicad/solar-project/Solar Project.net
- hardware/kicad/solar-project/ERC.rpt
- hardware/kicad/solar-project/DRC.rpt
- hardware/kicad/solar-project/Solar Project Rev0.kicad_sch
- hardware/kicad/solar-project/Solar Project Rev0.kicad_pcb
- hardware/kicad/solar-project/Solar Project Rev1.kicad_sch
- hardware/kicad/solar-project/Solar Project Rev1.kicad_pcb

Exclude lock/autosave artifacts:

- hardware/kicad/solar-project/~Solar Project.kicad_sch.lck
- hardware/kicad/solar-project/~Solar Project.kicad_pcb.lck
- hardware/kicad/solar-project/#auto_saved_files#/

### Commit B: Notes / Handoff / Checklists

Include only documentation updates:

- docs/rev-one-delta-plan-2026-08-04.md
- docs/test/rev-one-erc-drc-closure-checklist.md
- hardware/mppt/schematic-notes.md
- HANDOFF.md
- SOLAR_HANDOFF_2026-08-04.md
- NEXT_CHAT_PROMPT.md

## Immediate Execution Sequence

1. Export fresh netlist, ERC, and DRC from current Rev 1 active files.
2. Run closure checklist in docs/test/rev-one-erc-drc-closure-checklist.md.
3. Stage and commit Commit A first.
4. Stage and commit Commit B second.
