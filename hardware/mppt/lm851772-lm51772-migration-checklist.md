# LM851772-Q1 to LM51772-Q1 Migration Checklist

Last updated: 2026-07-19
Owners: Phil + Copilot
Purpose: verify pin compatibility and update design values safely when changing controller between LM851772-Q1 and LM51772-Q1.

## 1. How To Use This Checklist

1. Open both datasheets side by side at Pin Configuration and Functions and Specifications.
2. Fill Sections 2 through 6 in order.
3. Do not reuse prior resistor or protection values without recalculating and recording results.
4. Treat any unknown as FAIL until confirmed.

Known-now prefill status (2026-07-18):
- Verified from TI product and datasheet feature sections:
	- Both devices are 40-pin RHA package class and same controller family style.
	- LM51772-Q1 VIN class: 0V (with bias) to 55V.
	- LM851772-Q1 VIN class: 3.5V to 80V.
	- LM51772-Q1 typical operating IQ: 60uA.
	- LM851772-Q1 typical operating IQ: 75uA.
	- Both support 100kHz to 2.2MHz switching class, 2A gate-drive class, I2C diagnostics path.
- Not yet verified from extracted evidence in this file:
	- Exact pin-number to pin-name mapping table for all 40 pins.
	- Explicit vendor statement of guaranteed pin-to-pin drop-in equivalence.

In-progress implementation note (2026-07-19):
- Conservative schematic isolation pass removed direct U6 ties for previously identified miswires and assigned temporary nets (`U6_RST`, `U6_SYNC`, `U6_DTRK`, `U6_VCC2`) until final destination nets are confirmed.
- Fresh KiCad ERC and netlist export are required before marking these pin actions PASS.

### 1.1 WEBENCH Report Comparison Snapshot (from project PDFs)

Sources used:
- docs/WBDesignLM51772.pdf (created 2026-07-18 04:14:05.756)
- docs/WBDesignLM851772.pdf (created 2026-07-15 11:43:04.325)

| Item | LM51772 WEBENCH report | LM851772 WEBENCH report | Impact to migration |
|---|---|---|---|
| Power target | 6V to 42V in, 12.8V out, 20A | 6V to 42V in, 12.8V out, 20A | Same design target; functional compatibility still depends on pin mapping. |
| Topology | Buck_Boost | Buck_Boost | No topology-driven delta from these reports. |
| BOM count | 37 | 37 | Similar complexity at report level. |
| I2C/control label style | SDA_CFG3, SCL_CFG4, ADDR_SLOPE_CFG1 | SDA/SCL style labels, ADDR_CFG1, explicit I2C pull-up nets | Treat control/config pins as non-drop-in until datasheet pin table confirms equivalence. |
| Fault/status label style | FLT | FLT_N_INT_N | Confirm polarity, interrupt behavior, and pull-up strategy before reuse. |
| Feedback and limit label style | VIN-FB, FB_INT, ILIMCOMP_ISET | FB_SEL_intFB, ILIMCOMP | Reconfirm divider and limit network equations and register behavior. |
| Enable/bias label presentation | EN_UVLO and BIAS visible as separate labels | BIAS and EN_UVLO appear adjacent in extracted text | Confirm schematic symbol pin names directly in datasheet pin table, not PDF text adjacency. |

## 2. Pin-By-Pin Mapping Table (Required)

Pass rule:
- PASS if pin name (or equivalent function), electrical type, and functional behavior match for each required signal.
- Pin number equality is not required for this migration because footprint/package is being changed intentionally.

| Pin # | LM51772-Q1 Pin Name | LM51772-Q1 Function Summary | LM851772-Q1 Pin Name | LM851772-Q1 Function Summary | Match (Y/N) | Action Needed |
|---|---|---|---|---|---|---|
| 1 | VCC1 | Aux 5V regulator output; decouple locally | Aux 5V regulator output; decouple locally | Y | Keep same function; verify local decoupling guidance in selected datasheet. |
| 2 | SS_ATRK | Soft-start programming and analog tracking node | Soft-start programming and analog tracking node | Y | Same soft-start and tracking intent; carry network after value check. |
| 3 | SYNC | Sync clock input or output function | Sync clock input or output function | Y | Same sync function; keep pull strategy per selected mode. |
| 4 | DTRK | Digital PWM input for dynamic tracking | Digital PWM input for dynamic tracking | Y | Same dynamic tracking function; keep or strap as required. |
| 5 | SDA_CFG3 | I2C data line with configuration multiplexing | I2C serial data line | Y | Route to I2C SDA; review CFG3 dual-use behavior on LM51772. |
| 6 | SCL_CFG4 | I2C clock line with configuration multiplexing | I2C serial clock line | Y | Route to I2C SCL; review CFG4 dual-use behavior on LM51772. |
| 7 | MODE | Selects PSM versus forced PWM/CCM behavior | Selects PSM versus forced PWM/CCM behavior | Y | Same mode control concept; verify logic default and strap resistor values. |
| 8 | CFG2 | Device configuration input via strap | Device configuration input via strap | Y | Same config concept; recompute strap value per chosen operating mode. |
| 9 | ADDR_SLOPE_CFG1 | Address and slope or config selection input | I2C address and config selection input | Y | Functionally equivalent address/config path; verify slope/config options used. |
| 10 | CDC | Cable drop compensation or current monitor output | Cable drop compensation or current monitor output | Y | Same CDC and current monitor role; verify gain resistor value. |
| 11 | FLT | Open-drain fault or interrupt status output | Open-drain fault or interrupt status output | Y | Treat as open-drain fault and optional interrupt; confirm polarity and firmware handling. |
| 12 | RT | Frequency programming pin with resistor to AGND | Frequency programming pin with resistor to AGND | Y | Same frequency set function; recompute RT for target frequency. |
| 13 | COMP | Error amplifier output for loop compensation | Error amplifier output for loop compensation | Y | Same loop compensation node; redesign compensation for final power stage. |
| 14 | FB_INT | Feedback input with internal feedback select behavior | Feedback input and internal feedback select pin | Y | Feedback selection behavior differs by naming; verify internal/external FB selection state. |
| 15 | VIN-FB | Input voltage feedback sense input | Analog ground return | N | LM51772 VIN-FB function is not at same pin; implement dedicated VIN-FB path per LM51772 requirements. |
| 16 | ILIMCOMP/ISET | Current-limit loop compensation or setpoint node | Current-limit compensation or threshold set node | Y | Equivalent current-limit loop intent; confirm DAC and analog set mode implementation. |
| 17 | AGND | Analog ground reference | Analog ground reference | Y | Keep analog ground strategy and Kelvin return discipline. |
| 18 | VOUT | Output voltage sense input | Output voltage sense input | Y | Same output-sense role; keep divider sense routing and filtering. |
| 19 | ISNSN | Current-sense negative input | Current-sense negative input | Y | Same current-sense negative input; keep differential routing pair. |
| 20 | ISNSP | Current-sense positive input | Current-sense positive input | Y | Same current-sense positive input; keep differential routing pair. |
| 21 | CSB | Current monitor differential input B | Current monitor differential input B | Y | Same current-sense amplifier node; preserve RC filter intent. |
| 22 | CSA | Current monitor differential input A | Current monitor differential input A | Y | Same current-sense amplifier node; preserve RC filter intent. |
| 23 | SW1 | Buck half-bridge switch node | Buck half-bridge switch node | Y | Same switch-node role; preserve high dv/dt layout constraints. |
| 24 | HO1 | Buck high-side gate driver output | Buck high-side gate driver output | Y | Same high-side gate drive role for leg 1. |
| 25 | HB1 | Bootstrap supply for HO1 referenced to SW1 | Bootstrap supply for HO1 referenced to SW1 | Y | Same bootstrap supply for leg 1; keep close bootstrap capacitor. |
| 26 | NC | Not connected | Not connected | Y | Leave unconnected per datasheet guidance. |
| 27 | LO1 | Buck low-side gate driver output | Buck low-side gate driver output | Y | Same low-side gate drive role for leg 1. |
| 28 | PGND | Power ground return for gate drivers | Power ground return for gate drivers | Y | Same power ground role; maintain short high-current return path. |
| 29 | VCC2 | Internal bias regulator output for logic and drivers | Internal bias regulator output for logic and drivers | Y | Same internal bias output role; verify minimum C and placement. |
| 30 | LO2 | Boost low-side gate driver output | Boost low-side gate driver output | Y | Same low-side gate drive role for leg 2. |
| 31 | HB2 | Bootstrap supply for HO2 referenced to SW2 | Bootstrap supply for HO2 referenced to SW2 | Y | Same bootstrap supply for leg 2; keep close bootstrap capacitor. |
| 32 | HO2 | Boost high-side gate driver output | Boost high-side gate driver output | Y | Same high-side gate drive role for leg 2. |
| 33 | SW2 | Boost half-bridge switch node | Boost half-bridge switch node | Y | Same switch-node role; preserve high dv/dt layout constraints. |
| 34 | NC | Not connected | Not connected | Y | Leave unconnected per datasheet guidance. |
| 35 | DRV1 | Configurable external FET drive output stage | Configurable external FET drive output stage | Y | Same DRV path concept; confirm selected DRV mode and external network. |
| 36 | VIN | Main input supply and sense input | Optional bias regulator input | N | VIN and BIAS are swapped between parts at pins 36 and 40; rewire rails accordingly. |
| 37 | EN_UVLO | Enable plus UVLO comparator input | Enable plus UVLO comparator input | Y | Same enable and UVLO intent; recalculate divider thresholds for selected part. |
| 38 | RST | Digital reset or logic enable input | Digital reset or logic enable input | Y | Confirm reset polarity and pull network because naming differs. |
| 39 | NC | Not connected | Not connected | Y | Leave unconnected per datasheet guidance. |
| 40 | BIAS | Optional external bias input to reduce high VIN losses | Main input supply and sense input | N | VIN and BIAS are swapped between parts at pins 40 and 36; rewire rails accordingly. |

Implementation note:
- LM51772 pin names in this table are synchronized to the actual KiCad symbol used in the project (`U6`), including underscore naming and aliases.
- KiCad symbol also includes pin 41 `GND` (exposed pad). Keep this tied to ground strategy per layout and datasheet guidance.

Schematic migration rule for this project:
- Rewire by function, not by pin number.
- For every control and power signal in LM851772 design, identify one LM51772 destination pin with equivalent behavior.
- If no equivalent exists, mark FAIL and document circuit change (divider/network/register/update) before schematic lock.

## 3. High-Risk Pin Groups (Spot Check)

Mark each as PASS only after direct datasheet confirmation.

| Group | Check Item | PASS/FAIL | Notes |
|---|---|---|---|
| Gate drivers | HOx, LOx, SWx, BOOTx mapping identical | PARTIAL | Both reports expose HO1/LO1/HB1/SW1 and HO2/LO2/HB2/SW2 style paths; exact pin numbers still unverified. |
| Supply rails | VIN, VCC1, VCC2, GND/PGND pins compatible | PARTIAL | Both reports use VIN/VCC1/VCC2/GND/PGND labels, but controller VIN class differs (55V vs 80V family spec context). |
| Current monitor | Sense and monitor pins same transfer behavior | PARTIAL | LM51772 report text includes ILIMCOMP_ISET while LM851772 report shows ILIMCOMP naming; verify transfer and register mapping. |
| UVLO/EN | EN, UVLO, divider behavior and thresholds confirmed | PARTIAL | LM51772 report shows EN_UVLO and BIAS labels; LM851772 extracted text shows adjacent BIAS/EN_UVLO labeling. Confirm real pin names from datasheet table. |
| I2C/control | SDA/SCL, address/config, reset/fault pins aligned | PARTIAL | LM51772 report uses SDA_CFG3/SCL_CFG4 and ADDR_SLOPE_CFG1; LM851772 report uses SDA/SCL style and ADDR_CFG1. Treat as non-drop-in until pin table confirmation. |
| DRV path | DRV pin electrical behavior and external FET drive mode aligned | PARTIAL | DRV1 appears in both reports; mode-selection and any related pin behavior still require datasheet confirmation. |
| Fault/status | nFLT/nINT and status signaling behavior aligned | PARTIAL | LM51772 report uses FLT naming, LM851772 report uses FLT_N_INT_N naming. Verify polarity and interrupt behavior before reuse. |

## 4. Design Value Recalculation Checklist (Required)

Complete all fields before schematic lock.

### 4.1 Input protection and disconnect thresholds

| Parameter | Old Value | New Value | Basis | PASS/FAIL |
|---|---:|---:|---|---|
| PV overvoltage disconnect threshold | 58V to 60V (LM851772 start point) | 50V to 51V (LM51772 start point) | Controller VIN class + margin | PREFILLED |
| PV reconnect threshold | 54V to 56V (LM851772 start point) | 46V to 47V (LM51772 start point) | Hysteresis stability | PREFILLED |
| Input TVS standoff voltage | >= max normal PV input and below unwanted clamp region | >= max normal PV input and below unwanted clamp region | Normal max VIN below standoff | OPEN |
| Input TVS clamp level | Keep clamp comfortably below controller absolute max | Keep clamp comfortably below controller absolute max | Clamp below absolute max with margin | OPEN |
| High-side disconnect FET Vds rating | Sized for PV worst-case plus transient margin; 100V class is common in this project envelope | Sized for PV worst-case plus transient margin; 100V class is common in this project envelope | Worst-case PV + transient | PARTIAL |

### 4.2 UVLO/EN divider values

Use datasheet equations for the selected part and record calculated values.

| Item | Target | Calculated | Implemented | PASS/FAIL |
|---|---:|---:|---:|---|
| UVLO rising threshold | 11.0V to 11.5V (Rev 0 keep converter off below weak-panel region) | 11.2V using R23=1.00M and R24=121k with EN threshold assumed 1.21V | Current schematic still uses R23=75k and R24=21.5k; update pending | PARTIAL |
| UVLO falling threshold | 10.0V to 10.6V | 10.4V target after adding hysteresis resistor (bench trim expected) | Not yet implemented in schematic | OPEN |
| Hysteresis width | 0.7V to 1.0V | 0.8V target from EN hysteresis injection path | Not yet implemented in schematic | OPEN |
| EN pull-up/down network | High-value divider to reduce standby loss at high VIN | Divider current ~= 90uA at 100V with 1.121M total (1.00M + 121k) | Current divider is low-value legacy set (75k/21.5k); migrate during next value pass | PARTIAL |

Assumptions and notes:
- UVLO numbers above are Rev 0 starting targets for LM51772 bench bring-up, not final production limits.
- The 1.00M/121k divider aligns with the low-Iq guidance for wide-input operation and is intentionally conservative for early hardware protection.
- Falling threshold and hysteresis are marked OPEN until the exact datasheet hysteresis equation is applied and bench-verified.

### 4.3 Power-stage timing and control values

| Item | Old Value | New Value | Reason for Change | PASS/FAIL |
|---|---:|---:|---|---|
| Switching frequency setting components | R28=51.7k (current schematic, RT pin) | Keep R28=51.7k for first Rev 0 bring-up pass | Preserves known-good WEBENCH starting point while migration risk is isolated to control-rail and naming changes | PARTIAL |
| Soft-start capacitor | C26=33nF (current schematic) | C26=100nF target | Extend startup ramp toward ~100ms class startup to reduce inrush stress during first hardware tests | OPEN |
| Slope compensation related values | Existing network tied to MODE/ILIMCOMP_ISET (R29/R32/C29 net context) | Keep existing slope path for first power-up; tune only if subharmonic behavior appears | Avoids coupled loop changes before baseline startup is verified | PARTIAL |
| Current limit scaling | R32=100k, R33=6.49k (current schematic values) | Keep initial values; adjust after measured current-limit threshold test | Migration changes controller family variant; threshold transfer must be validated on bench data | PARTIAL |

### 4.4 Compensation and stability

| Item | Old Value | New Value | Verification Method | PASS/FAIL |
|---|---:|---:|---|---|
| Loop compensation network | Existing Rev 0 seed network (R31/C24/C25 around COMP) | Keep seed values for first power-up, then retune using measured transients | Start with load-step test and confirm no sustained ringing before value changes | PARTIAL |
| Crossover target | Not documented | 50kHz conservative target | FRA/Bode injection if available; otherwise infer from load-step response and settling | OPEN |
| Phase margin target | Not documented | >=55 degrees (prefer 60 degrees) | Verify from measured Bode where possible, otherwise use transient damping as proxy | OPEN |

Stability workflow note:
- Do not tune compensation in the same edit pass as UVLO and soft-start changes unless startup behavior is already stable.
- Recommended order: (1) UVLO and startup validation, (2) current-limit sanity check, (3) compensation tuning to 50kHz crossover target.

## 5. Layout and Footprint Verification

| Check | PASS/FAIL | Notes |
|---|---|---|
| Package footprint pin-1 orientation verified | PARTIAL | Both are RHA 40-pin class; still confirm exact package drawing and pin-1 marker. |
| Thermal pad and land pattern match device package note | PARTIAL | Verify paste mask and exposed pad guidance for the selected datasheet revision. |
| Gate-drive loops remain compact after any reroute | TBD | TBD |
| Sense lines remain Kelvin and away from switch node | TBD | TBD |
| High dv/dt nets keep clearance and return control | TBD | TBD |

## 6. Bring-Up Validation Plan Delta

| Test | Target | PASS/FAIL | Notes |
|---|---|---|---|
| No-load startup at nominal PV emulation | Stable startup | TBD | TBD |
| Startup near UVLO threshold | No chatter or latch fault | TBD | TBD |
| OV disconnect event and recovery | Clean cutoff and reconnect | TBD | TBD |
| Load transient response | No unstable oscillation | TBD | TBD |
| Thermal check at expected charge power | Within planned limits | TBD | TBD |

## 7. Decision and Signoff

| Item | Value |
|---|---|
| Selected controller | TBD |
| Decision date | TBD |
| Rationale summary | Primary discriminator is required PV voltage headroom vs sourcing maturity risk. |
| Open risks | LM851772-Q1 preproduction maturity risk; LM51772-Q1 tighter VIN headroom margin risk. |
| Owner signoff | TBD |

## 8. Exit Criteria

All must be true before schematic lock:

- Section 2 has no unresolved functional mismatches (pin-number changes allowed).
- Section 4 has completed recalculations with documented values.
- Section 5 layout checks are all PASS.
- Section 6 critical bring-up tests are PASS or explicitly deferred with owner/date.
