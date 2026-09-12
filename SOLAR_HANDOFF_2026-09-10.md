# Solar Project Rev 1 Handoff - 2026-09-10

## Session result

This session rebaselined the Rev 1 LM51772 converter around the actual lower-current design envelope and reviewed the current schematic/netlist in batches. The active schematic was kept open during the final work; later checks were read-only or used temporary KiCad netlist exports.

## Governing design basis

- 125 W continuous charger output.
- 150 W maximum PV operating power.
- 8 A to 10 A nominal charge current.
- Approximately 12 A hardware protection ceiling.
- Provisional qualified/nominal PV operating point: 20 V.
- Provisional PV maximum for sizing: 48 V.
- LM51772 hardware UVLO remains approximately 6 V as a last-resort boundary, not the normal MPPT operating point.
- Intended LiFePO4 4S charge target: 14.6 V, pending final battery/BMS confirmation.
- Switching frequency model: 600 kHz.

Controlling decision: DEC-030 in `docs/decisions-log.md`.

## Working source-of-truth workbook

Working copy:
`hardware/LM51772_Buck-Boost_Quickstart_Tool_Rev1_20V_14V6_12A.xlsm`

The original TI workbook remains unchanged. The working copy was updated and recalculated in Excel with:

- VIN(min) = 20 V
- VIN(nom) = 20 V
- VIN(max) = 48 V provisional
- VOUT = 14.6 V
- IOUT(max) = 12 A
- L2 = 3.3 uH
- Effective minimum inductance = 2.64 uH
- L2 DCR = 5.8 mOhm
- Input capacitance model = 100 uF
- Output capacitance model = 100 uF

Excel-recalculated results recorded in `docs/rev-one-component-derating-review.md`:

- Minimum input capacitance: about 27.78 uF.
- Minimum output capacitance: about 3.47 uF in the workbook run.
- Input-capacitor RMS current: about 8.56 A.
- Output-capacitor RMS current: about 1.86 A.
- Input ripple: about 80.5 mV p-p.
- Output ripple: about 35.0 mV p-p.
- Endpoint inductor results: about 2.50 A p-p / 13.25 A peak at 20 V and 6.45 A p-p / 15.23 A peak at 48 V.

## Component decisions and current netlist state

### L2
Selected provisional part: `MMPC1206-3R3MT`.

- 3.3 uH nominal, +/-20%.
- 2.64 uH minimum used in the workbook.
- 5.8 mOhm DCR.
- 17 A rated current and 35 A saturation rating per LCSC listing.
- Keep provisional pending thermal, core-loss, and final PV-envelope validation.

### Input capacitors
Current netlist places C19, C21, C30, and C31 across `PV_IN_POS` to `PWR_NEG`.

- Each is 47 uF and rated for 2.6 A RMS per the datasheet.
- Combined ripple-current rating: 10.4 A RMS.
- This exceeds the workbook result of about 8.56 A RMS by about 21.5%.
- C7 is 1 uF, while C8 and C28 are 100 nF on the same input node.
- Current bulk input total is 188 uF nominal plus ceramics.

### Output capacitors
Current C18 is 560 uF, C3 is 100 uF, C34-C35 are 2.2 uF local/output-side ceramics, and C33 is 100 nF, all on `V_out`. The current nominal output bank is approximately 664.4 uF, not 1.3 mF. C3 is rated 25 V and C34-C35 are rated 50 V; C18 voltage, ripple-current, and ESR data still need confirmation. The output bank is not yet fully dispositioned. Do not delete or reduce additional output capacitance until startup inrush, effective capacitance, ESR, transient response, and loop stability are reviewed.

### M1-M4 bridge MOSFETs
Current netlist confirms all four are now `CSD18540Q5B`:

- 60 V, 2.2 mOhm class, SON 5x6 mm.
- Existing footprint: `Package_TO_SOT_SMD:TDSON-8-1`.
- User confirmed pin orientation and 5x6 SON footprint are acceptable.
- Temporary KiCad netlist exports regenerated successfully.
- Remaining checks: PCB copper/thermal pad, switching overshoot, gate timing, and VGS margin.

### Q4 PV protection MOSFET
Current part is `HSBA0119`, 100 V P-channel, about 23 mOhm, 5x6 mm class.

- Source: `PV_IN_POS_FUSED`.
- Drain: `PV_IN_POS`.
- Gate: D1/R42 control network.
- Provisional pass for voltage/current class.
- Remaining checks: actual RDS(on) at gate bias, gate transient/VGS clamp, thermal copper, and pinout.

### Q2/Q3 gate/control transistors
Current netlist confirms both are now `2N7002K`, 60 V, SOT-23, LCSC `C22379475`.

- Q2 remains in Q4 gate-control branch.
- Q3 remains in the separate J5/control branch.
- Temporary netlist export succeeded; no BSS138/AO3400A remains for Q2/Q3.
- Confirm Q3 current and J5 external-voltage assumptions before final acceptance.

### R16/R22 shunts
The user requested larger identical shunts for measurement margin. Current netlist confirms:

- R16 and R22: same 1 mOhm C41382682 WSK2512 shunt part.
- Both now use `Resistor_SMD:R_Shunt_Vishay_WSK2512_6332Metric_T2.66mm`.
- R16 is the output/battery-current shunt.
- R22 is the inductor peak-current shunt.
- Existing Kelvin nets must remain distinct and polarity must be verified separately.

### Voltage dividers
Current values still reflect the old 12.8 V / 12.83 V design point:

- Output divider: R3 = 66.5 kOhm, R25 = 5.62 kOhm, about 12.83 V.
- Input VIN-FB divider: R18 = 66.5 kOhm, R17 = 5.62 kOhm.
- Proposed matched 14.6 V update, not yet applied: R3 and R18 to 76.8 kOhm; R25 and R17 remain 5.62 kOhm.
- Do not apply this while the schematic has unsaved user edits unless the user confirms it is saved.

Hardware UVLO divider remains:

- R23 = 75 kOhm.
- R61 = 20 kOhm.
- R60 = 1.5 kOhm.
- Hardware rising threshold is near 6 V input; retain as independent protection, not normal MPPT control.

## Important file-state rule

The user keeps the KiCad schematic open and may have unsaved edits. Do not overwrite `Solar Project Rev1.kicad_sch` implicitly. Before any schematic edit, ask the user to save/confirm the current file state. Temporary netlist exports are safe for validation. The current generated `Solar Project Rev1.net` reflects recent user edits and must be reread before any new change.

## Open items for next session

1. User-confirmed save point, then apply or review R3/R18 = 76.8 kOhm and verify the 14.6 V setpoint.
2. Verify R17/R18 VIN-FB gain matching against LM51772 PCM requirements.
3. Complete Q4 gate-control review, including Q2/Q3 VGS and R42/R43/D1 behavior.
4. Verify C19/C21/C30 exact datasheet voltage, ESR, ripple-current, and thermal sharing data.
5. Resolve output-capacitor bank: C3/C18/C33-C35 plus any retained bulk, with inrush and loop-stability evidence.
6. Complete R16/R22 Kelvin pad mapping and sense polarity against the WSK2512 datasheet.
7. Rework F2: replace the current nonstandard/unwanted fuse-clip footprint with the required standard clips, then select the matching fuse and verify true series placement, fuse curve, rating, interrupt capability, and holder thermal/contact limits.
8. Compare final battery/BMS connector options used in comparable designs; J3's current Phoenix terminal block is prototype-only until that interface decision is made.
9. Rework Q5/Q6: replace the current 60 V CSD18563Q5A pair with a matched 100 V N-channel pair for the provisional 48 V PV ceiling, or formally reduce the PV maximum and recalculate all transient margins.
10. Export fresh ERC/DRC only after the user confirms the schematic is saved and the next edit batch is agreed.

## Recommended first move

Read this handoff, inspect the saved current netlist, and ask the user to confirm whether the open schematic has been saved. Then handle the paired R3/R18 14.6 V divider update as one controlled batch, followed by a temporary netlist export and threshold calculation.
