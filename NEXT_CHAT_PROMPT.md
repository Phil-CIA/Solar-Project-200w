Continue the Solar Project 200W repository. LM51772 migration cleanup and ERC correction are complete for this pass.

## Current state to assume

- U6 is LM51772QRHARQ1 in the KiCad schematic and netlist.
- ERC is at **7 errors / 54 warnings** as of 2026-07-19T07:52.
- All 7 remaining errors are explicitly deferred power-authority semantics (see list below).
- Migration checklist at `hardware/mppt/lm851772-lm51772-migration-checklist.md` is updated through the 2026-07-19 pass.

## What was completed in the last session (2026-07-19)

1. U6 miswires corrected:
   - RST (pin 38) → CTRL_3V3 (pulled high for normal operation)
   - SYNC (pin 3) → PWR_NEG (free-running internal oscillator, Rev 0 strap)
   - DTRK (pin 4) → PWR_NEG (dynamic tracking disabled, Rev 0 strap)
   - VCC2 (pin 29) → placeholder net `U6_VCC2` (output, decoupling cap still needed)
2. VIN/BIAS strategy decided: both pin 36 (VIN) and pin 40 (BIAS) on PV_IN_POS — intentional for Rev 0 simplicity.
3. Net-label hygiene: `UART_RX_CFG` and `V_in_ADC` trailing/embedded spaces removed.
4. Gate-drive `pin_not_driven` errors for M1–M4 fixed: U6 HO1/LO1/HO2/LO2 pin types changed to `output` in embedded lib_symbols.
5. R32 pin 2 `pin_not_connected` fixed: `PWR_NEG` label added at (93.98, 219.71).

## 7 remaining deferred errors (all power-authority semantics)

| Symbol | Pin | Error | Waiver reason |
|---|---|---|---|
| U6 | 36 VIN | power_pin_not_driven | PV_IN_POS has no PWR_FLAG; deferred end-of-phase |
| U6 | 28 PGND | power_pin_not_driven | PGND has no PWR_FLAG; deferred end-of-phase |
| U1 | 1 VBAT | power_pin_not_driven | CTRL_3V3 power authority; deferred from 2026-07-14 |
| U4 | 2 VIN | power_pin_not_driven | CTRL_SUPPLY_IN power authority; deferred from 2026-07-14 |
| D3 | 2 | power_pin_not_driven | USB ESD diode library symbol type |
| D3 | 4 | pin_not_driven | USB ESD diode library symbol type |
| D3 | 6 | pin_not_driven | USB ESD diode library symbol type |

One new warning: `lib_symbol_mismatch` for U6 — embedded symbol pin types differ from library copy because HO1/LO1/HO2/LO2 were updated in-schematic. Intentional, accept for Rev 0.

## Recommended next session focus

Choose one of:
A. **U6_VCC2 decoupling** — add a 100nF decoupling cap from U6_VCC2 to PWR_NEG/AGND in the schematic. This closes the VCC2 placeholder and enables tying BIAS to VCC2 for lower quiescent dissipation.
B. **U6_FLT pull-up** — add a pull-up resistor from U6_FLT to CTRL_3V3 (or VCC2) so the open-drain fault output is usable.
C. **Migration checklist Section 4 values** — fill UVLO/EN divider recalculation (Section 4.2), soft-start cap (Section 4.3), and loop compensation (Section 4.4) for the LM51772.
D. **PWR_FLAG cleanup** — add proper power-source markers using KiCad GUI (not text edit) to close the 5 remaining power_pin_not_driven errors in one pass.

## First actions next session

1. Read this file and `SOLAR_HANDOFF_2026-07-19.md`.
2. Run `git status --short` and verify state.
3. Open `hardware/kicad/solar-project/ERC.rpt` and confirm 7-error baseline.
4. Pick focus A, B, C, or D above and proceed.

