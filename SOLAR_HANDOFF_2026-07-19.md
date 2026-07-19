# Solar Project 200W - Handoff (2026-07-19)

## Session Summary

**Date**: 2026-07-19  
**Phase**: Phase 1 implementation refinement (Rev 0)  
**Focus**: LM51772 (U6) migration cleanup and ERC hard-error reduction

---

## What Is Already Pushed To Remote

- Commit `728ef1a` remains on `main` and `origin/main`.
- All work in this session is uncommitted local state.

---

## Local (Uncommitted) State At Stop

From `git status --short` at stop:

Modified:
- `NEW_CHAT_HANDOFF.rmd`
- `NEW_CHAT_HANDOFF_SHORT.rmd`
- `NEXT_CHAT_PROMPT.md`
- `docs/decisions-log.md`
- `docs/session-plan-2026-07-10.md`
- `hardware/kicad/solar-project/Solar Project.kicad_pro`
- `hardware/kicad/solar-project/Solar Project.kicad_sch`
- `hardware/mppt/kicad-implementation-checklist.md`
- `hardware/mppt/net-plan.md`
- `hardware/mppt/schematic-notes.md`
- `hardware/mppt/sheet4-sensing-control-wiring-map.md`

Untracked (all intentional, commit with docs):
- `SOLAR_HANDOFF_2026-07-14.md`
- `SOLAR_HANDOFF_2026-07-19.md`
- `docs/WBDesignLM51772.pdf`
- `docs/WBDesignLM851772.pdf`
- `docs/lm51772-datasheet.pdf`
- `docs/lm851772-datasheet.pdf`
- `docs/lm851772-q1.pdf`
- `docs/session-plan-2026-07-14.md`
- `hardware/kicad/solar-project/Solar Project.kicad_sch-bak`
- `hardware/kicad/solar-project/fp-info-cache`
- `hardware/mppt/control-converter-selection-worksheet-2026-07-14.md`
- `hardware/mppt/lm851772-lm51772-migration-checklist.md`
- `hardware/mppt/pre-routing-blockers-2026-07-10.md`
- `hardware/mppt/rev0-component-shortlist-2026-07-10.md`

Do not commit `Solar Project.kicad_sch-bak` (KiCad autosave backup).

---

## ERC State At Stop

File: `hardware/kicad/solar-project/ERC.rpt`  
Timestamp: 2026-07-19T07:52:06-0500  
**Total: 7 errors / 54 warnings**

All 7 errors are explicitly deferred power-authority semantics:

| Symbol | Pin | Error | Waiver |
|---|---|---|---|
| U6 | 36 VIN | power_pin_not_driven | PV_IN_POS no PWR_FLAG; deferred end-of-phase |
| U6 | 28 PGND | power_pin_not_driven | PGND no PWR_FLAG; deferred end-of-phase |
| U1 | 1 VBAT | power_pin_not_driven | CTRL_3V3 authority; carried from 2026-07-14 |
| U4 | 2 VIN | power_pin_not_driven | CTRL_SUPPLY_IN authority; carried from 2026-07-14 |
| D3 | 2 | power_pin_not_driven | USB ESD diode library symbol |
| D3 | 4 | pin_not_driven | USB ESD diode library symbol |
| D3 | 6 | pin_not_driven | USB ESD diode library symbol |

Notable new warning: `lib_symbol_mismatch` for U6 — embedded symbol differs from library because HO1/LO1/HO2/LO2 were updated to `output` type in-schematic. Intentional; accept for Rev 0.

---

## Technical Changes This Session

### Schematic (`Solar Project.kicad_sch`)

1. **U6 miswire isolation and Rev 0 strapping**
   - Pin 38 RST: removed from PWR_NEG, strapped to `CTRL_3V3`
   - Pin 3 SYNC: removed isolated stub, explicit `PWR_NEG` label (free-run mode)
   - Pin 4 DTRK: explicit `PWR_NEG` label (tracking disabled)
   - Pin 29 VCC2: isolated to placeholder net `U6_VCC2` (output; decoupling cap needed)
   - Pin 11 FLT: isolated to placeholder net `U6_FLT` (pull-up resistor needed)

2. **VIN/BIAS decision (Rev 0)**
   - Pin 36 VIN: remains on `PV_IN_POS` ✓
   - Pin 40 BIAS: remains on `PV_IN_POS` — intentional Rev 0 strap (higher IQ, simpler wiring); upgrade to VCC2 in Rev 1

3. **Net-label hygiene**
   - `UART_RX_CFG ` (trailing space) → `UART_RX_CFG`
   - `V_in _ADC` (embedded space) → `V_in_ADC`
   - Both labels normalized at all instances in the schematic

4. **Gate-driver pin types**
   - U6 lib_symbols: HO1, LO1, HO2, LO2 changed from `unspecified` to `output`
   - Eliminates 4 hard errors: M1/M2/M3/M4 gate `pin_not_driven`

5. **R32 pin 2 grounded**
   - `PWR_NEG` label added at (93.98, 219.71)
   - Eliminates `pin_not_connected` and `unconnected_wire_endpoint` pair for R32

### Docs (`hardware/mppt/schematic-notes.md`)

- Added 2026-07-19 entries covering all actions above with rationale and ERC evidence.

### Docs (`hardware/mppt/lm851772-lm51772-migration-checklist.md`)

- Added in-progress implementation note (2026-07-19) confirming conservative isolation pass and pending verification items.

---

## U6 Net Map At Stop (from fresh netlist 2026-07-19T07:43)

| Pin | Function | Net |
|---|---|---|
| 36 | VIN | PV_IN_POS |
| 40 | BIAS | PV_IN_POS |
| 17 | AGND | PWR_NEG |
| 3 | SYNC | PWR_NEG |
| 4 | DTRK | PWR_NEG |
| 41 | GND | PWR_NEG |
| 28 | PGND | PGND |
| 38 | RST | CTRL_3V3 |
| 29 | VCC2 | U6_VCC2 (placeholder) |
| 11 | FLT | U6_FLT (placeholder) |
| 37 | EN_UVLO | _Undervoltage Feedback |
| 18 | VOUT | CHG_OUT_POS |
| 15 | VIN-FB | V_in_FB |
| 14 | FB_INT | _OUTPUT Feedback |

---

## Recommended Next Session Focus

Pick one:

**A. U6_VCC2 decoupling** — place 100 nF cap from U6_VCC2 to AGND/PWR_NEG. Closes VCC2 placeholder and enables BIAS→VCC2 upgrade.

**B. U6_FLT pull-up** — add pull-up resistor (10 kΩ typ) from U6_FLT to CTRL_3V3 or VCC2. Makes fault output usable for MCU.

**C. Migration checklist value calculations** — fill Sections 4.2 (UVLO/EN divider), 4.3 (soft-start cap, switching frequency), 4.4 (loop compensation) in `hardware/mppt/lm851772-lm51772-migration-checklist.md`.

**D. PWR_FLAG cleanup** — add power-source markers via KiCad GUI (not text edit) to close remaining 5 power_pin_not_driven errors.

---

## First Actions Next Session

1. Read `SOLAR_HANDOFF_2026-07-19.md` and `NEXT_CHAT_PROMPT.md`.
2. Run `git status --short` to verify working tree.
3. Open `hardware/kicad/solar-project/ERC.rpt` — confirm 7-error baseline.
4. Choose focus A, B, C, or D and proceed.
