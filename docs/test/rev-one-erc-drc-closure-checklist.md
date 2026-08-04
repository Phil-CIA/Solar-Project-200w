# Rev One ERC/DRC Closure Checklist

Date created: 2026-08-04  
Owners: Phil + Copilot

## Preconditions

- Active design files are Rev 1 source of truth.
- Rev 0 recovery files are present for historical comparison.
- KiCad lock files are absent before exports.

## Export Evidence Set

- [ ] Export netlist from active Rev 1 schematic.
- [ ] Run ERC and save hardware/kicad/solar-project/ERC.rpt.
- [ ] Run DRC and save hardware/kicad/solar-project/DRC.rpt.
- [ ] Confirm export timestamps are newer than active schematic and board edits.

## Electrical Rule Closure

- [ ] ERC errors are zero, or each non-zero item is explicitly waived with owner and rationale.
- [ ] No new hard-error class appears compared to Rev 0 closeout baseline.
- [ ] LM74502H wiring remains intact: VS on V_out_sense, OV divider R37/R66/R67, EN_UVLO pull-down R38.
- [ ] Back-to-back N-MOS reverse-protection path remains unchanged.
- [ ] C52 working-voltage decision is recorded (keep or raise) before freeze.

## Layout Rule Closure

- [ ] DRC errors are zero, or each non-zero item is explicitly waived with owner and rationale.
- [ ] Battery boundary connectivity remains CHG_OUT_POS -> F2 -> BAT_BUS_POS with PWR_NEG return.
- [ ] Critical high-current paths have no accidental net splits after Rev 1 edits.

## Net Continuity Spot Checks

- [ ] Sheet 1 protection boundary nets are continuous and canonical.
- [ ] Sheet 2 CTRL_PWM_MAIN remains the only gate-drive command path.
- [ ] Sheet 3 battery interface handoff nets remain canonical.
- [ ] Sheet 4 FAULT_OCP and FAULT_OVP continuity to MCU landing remains intact.

## Release Gate Decision

- [ ] Ready for Rev 1 board release candidate.
- [ ] Blocked pending listed closure items.

## Notes

Record waivers and closure rationale directly in this file with date and owner initials.
