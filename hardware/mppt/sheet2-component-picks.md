# Sheet 2 Component Picks (Buck Power Stage)

Last updated: 2026-07-07
Owners: Phil + Copilot
Scope: choose first-pass components for Sheet 2 only (power stage), so symbols can be placed now.

## 1. First-Pass Picks

| Ref | Function | First-Pass Part Class | Starter Electrical Target | Placement Intent |
|---|---|---|---|---|
| Q1 | High-side buck switch | N-channel MOSFET | Vds >= 100V, Id >= 30A, low Rds(on) class | Place close to D3 and L1 around switch node |
| D3 | Freewheel path (asynchronous) | Schottky diode | Vr >= 100V, If(avg) >= 10A, surge-capable package | Place tight to Q1 source and return path |
| L1 | Buck inductor | Shielded power inductor | 33uH to 68uH, Isat >= 15A, low DCR class | Place directly after switch node toward output caps |
| C4 | Input decoupling #1 | Ceramic capacitor | 4.7uF to 10uF, 100V, X7R | Place very close to Q1 drain-source loop |
| C5 | Input decoupling #2 | Ceramic capacitor | 1uF to 4.7uF, 100V, X7R | Place beside C4 for HF support |
| C6 | Output bulk #1 | Low-ESR electrolytic/polymer | 100uF to 220uF, >=35V | Place close to L1 output and CHG_OUT return |
| C7 | Output bulk #2 | Low-ESR electrolytic/polymer or ceramic mix | 47uF to 220uF, >=35V | Place parallel to C6 near output handoff |
| R1 | Snubber resistor (placeholder) | Thick-film resistor | 5 ohm to 22 ohm, >=0.25W | Keep adjacent to Q1/switch node |
| C8 | Snubber capacitor (placeholder) | Ceramic capacitor | 470pF to 2.2nF, 100V NP0/X7R | In series with R1, close to switch loop |

## 2. Symbol Placement Sequence

1. Place Q1, D3, L1 first and connect `MPPT_SW_NODE` path.
2. Place C4/C5 next and connect to `PV_IN_POS` and `PV_IN_NEG`.
3. Place C6/C7 on `CHG_OUT_POS` and `CHG_OUT_NEG`.
4. Place R1/C8 snubber placeholder from `MPPT_SW_NODE` to `PV_IN_NEG`.

## 3. Nets To Use

- `PV_IN_POS`
- `PV_IN_NEG`
- `MPPT_SW_NODE`
- `CHG_OUT_POS`
- `CHG_OUT_NEG`
- `CTRL_PWM_MAIN` (or `GATE_Q1` if separated later)

## 4. Notes Before Part Lock

- These are placement-ready starter classes, not final procurement selections.
- Final value tuning (inductor, snubber, capacitor mix) will occur after controller and switching frequency are fixed.
- Rev 0 build lock: keep asynchronous freewheel path (D3) for this revision.
- Defer synchronous converter and dedicated gate-driver implementation to Rev 1.
