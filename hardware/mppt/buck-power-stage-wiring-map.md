# Buck Power Stage Wiring Map (Sheet 2)

Last updated: 2026-07-07
Owners: Phil + Copilot
Purpose: define exact first-pass symbol connectivity for the buck conversion stage in KiCad.

## 1. Symbols

- Q1: High-side buck switch MOSFET
- D3: Freewheel diode (asynchronous first pass)
- L1: Buck inductor
- C4: Buck input decoupling capacitor #1
- C5: Buck input decoupling capacitor #2
- C6: Buck output capacitor #1
- C7: Buck output capacitor #2
- R1/C8: Snubber placeholder across switch node

Component selection reference:
- See `sheet2-component-picks.md` for first-pass part classes and starter targets.

## 2. Net Naming

Use these nets on Sheet 2:
- PV_IN_POS
- PV_IN_NEG
- MPPT_SW_NODE
- CHG_OUT_POS
- CHG_OUT_NEG

Control and measurement placeholders for this sheet:
- CTRL_PWM_MAIN
- SENSE_PV_I

Controller-drive rule for this sheet:
- `CTRL_PWM_MAIN` is the only live controller drive input for Q1 in Rev 0.
- Do not reintroduce any old MOSFET-era gate-control label or alternate drive net on this sheet.

## 3. Connectivity Map

1. Q1 drain -> `PV_IN_POS`
2. Q1 source -> `MPPT_SW_NODE`
3. Q1 gate -> `CTRL_PWM_MAIN`
4. Q1 source reference return -> `PV_IN_NEG`

5. D3 cathode -> `PV_IN_NEG`
6. D3 anode -> `MPPT_SW_NODE`

7. L1 input -> `MPPT_SW_NODE`
8. L1 output -> `CHG_OUT_POS`

9. C4 positive -> `PV_IN_POS`
10. C4 negative -> `PV_IN_NEG`

11. C5 positive -> `PV_IN_POS`
12. C5 negative -> `PV_IN_NEG`

13. C6 positive -> `CHG_OUT_POS`
14. C6 negative -> `CHG_OUT_NEG`

15. C7 positive -> `CHG_OUT_POS`
16. C7 negative -> `CHG_OUT_NEG`

17. CHG_OUT_NEG -> `PV_IN_NEG` (single-point power return tie for first pass)

18. R1 in series with C8 across `MPPT_SW_NODE` to `PV_IN_NEG` (snubber placeholder)

## 4. Placement Intent

- Place Q1, D3, and L1 as a compact triangle around `MPPT_SW_NODE`.
- Place C4/C5 close to Q1 drain/source loop.
- Place C6/C7 close to L1 output and return path.
- Keep the snubber placeholder physically adjacent to Q1 and switch node.

## 5. Sheet Notes To Add

- ASSUME: asynchronous buck for first pass (freewheel diode path)
- DECISION: Rev 0 remains asynchronous; synchronous implementation is a Rev 1 architecture change.
- DECISION: `CTRL_PWM_MAIN` is the only controller drive path for Q1 in Rev 0.
- VERIFY: Q1 voltage class covers worst-case cold Voc margin
- VERIFY: freewheel diode reverse-voltage and current margin are sufficient
- VERIFY: switch loop and output loop are compact and obvious

## 6. Handoff To Sheet 3/4

Export to next sheets:
- `CHG_OUT_POS`
- `CHG_OUT_NEG`
- `SENSE_PV_I` placeholder from current-sense strategy
- `CTRL_PWM_MAIN` from controller sheet

This keeps power conversion and control boundaries explicit while allowing later refinement.
