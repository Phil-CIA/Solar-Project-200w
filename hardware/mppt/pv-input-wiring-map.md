# PV Input Wiring Map (Sheet 1)

Last updated: 2026-07-07
Owners: Phil + Copilot
Purpose: define exact first-pass symbol connectivity for the PV input boundary in KiCad.

## 1. Symbols

- J1: PV input connector (2-pin)
- F1: Input fuse
- D1: Series Schottky reverse-polarity diode
- D2: TVS diode
- C1: Input bulk capacitor #1
- C2: Input bulk capacitor #2
- C3: High-frequency bypass capacitor

## 2. Net Naming

Use these power nets for Sheet 1:
- PV_IN_POS_RAW
- PV_IN_NEG
- PV_IN_POS_FUSED
- PV_IN_POS

Note:
- `PV_IN_POS` and `PV_IN_NEG` are the handoff nets into the buck stage.

## 3. Connectivity Map

1. J1 pin 1 -> `PV_IN_POS_RAW`
2. J1 pin 2 -> `PV_IN_NEG`

3. F1 in -> `PV_IN_POS_RAW`
4. F1 out -> `PV_IN_POS_FUSED`

5. D1 anode -> `PV_IN_POS_FUSED`
6. D1 cathode -> `PV_IN_POS`

7. D2 cathode -> `PV_IN_POS`
8. D2 anode -> `PV_IN_NEG`

9. C1 positive -> `PV_IN_POS`
10. C1 negative -> `PV_IN_NEG`

11. C2 positive -> `PV_IN_POS`
12. C2 negative -> `PV_IN_NEG`

13. C3 positive -> `PV_IN_POS`
14. C3 negative -> `PV_IN_NEG`

## 4. Placement Intent

- Place J1 and F1 at the board-entry edge.
- Place D1 immediately after F1 in the positive path.
- Place D2 as a shunt between `PV_IN_POS` and `PV_IN_NEG` close to D1 output.
- Place C1/C2/C3 close to the D1 output and the buck-stage input handoff.

## 5. Sheet Notes To Add

- SAFETY: PV boundary protection ownership (F1/D1/D2)
- ASSUME: 2S Renogy panel baseline
- VERIFY: worst-case cold Voc margin vs part ratings
- DECISION: DEC-012/Q-003 ideal-diode upgrade unresolved before PCB routing

## 6. Handoff To Sheet 2

Export only these two nets to the buck power-stage sheet:
- `PV_IN_POS`
- `PV_IN_NEG`

This keeps the boundary clean and prevents accidental bypass of input protection.
