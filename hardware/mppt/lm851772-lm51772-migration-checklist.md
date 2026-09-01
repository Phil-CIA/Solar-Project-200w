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
| I2C/control | SDA/SCL, address/config, reset/fault pins aligned | FAIL (bench bodge applied, schematic not yet updated) | Root cause found 2026-09-01 for the "no I2C ack from U6" bring-up finding. Per LM51772-Q1 datasheet Section 7.5.1/Table 7-4, I2C on U6 pin 9 (`ADDR/SLOPE`, net `ADDR_SLOPE_CFG1`) is enabled **only** when that pin is a direct short to GND (address `0x6A`) or to VCC2 (address `0x6B`). Any resistor from that pin instead selects a slope-compensation factor and explicitly **disables** the I2C interface, repurposing `SDA/CFG3` and `SCL/CFG4` (U6 pins 5/6, routed to the shared software-I2C bus alongside AHT20 `0x38` and SSD1306 `0x3C`) as CFG3/CFG4 strap pins. The checked-in schematic/netlist (`report.txt` line 1248) still shows `R33` = 6.49 kOhm from `ADDR_SLOPE_CFG1` to `PGND`, which lands on the 6.5 kOhm row of Table 7-4 (`msc = 1`, I2C DISABLED) instead of a direct GND short. This fully explained why `i2cscan`/H5 only found `0x38`/`0x3C` and never `0x6A`/`0x6B`, independent of VCC1/VCC2/EN_UVLO health. On the physical bench board (2026-09-01), `R33` was replaced with a 0 Ohm link and `PGND` was jumpered to `AGND`, which also fixes the ground-reference mismatch described below for `RT`/`CDC`/`CFG2`. **The KiCad schematic/PCB source files have not been updated to match this bench rework** — do not assume the checked-in design reflects current board state; re-run `i2cscan` to confirm `0x6A` now acknowledges, then update the schematic (`R33` value/DNP + an explicit single-point `PGND`-`AGND` link near U6) before Rev 1 lock. The current-limit slope compensation previously implied by the R33 resistor value must be replaced with the equivalent I2C `ILIM_THRESHOLD`/DAC register setting once I2C is confirmed enabled. |

Ground-reference check (2026-09-01): re-verifying all U6 config-strap resistors against their datasheet-required return node found that `R33` was not unique. `R28` (RT, pin 12), `R29` (CFG2, pin 8), and `R30` (CDC, pin 10) also return to the `PGND` net (U6 pin 28 only) instead of the `AGND`/`PWR_NEG` net the datasheet specifies for `RT` and `CDC` (`CFG2` datasheet wording says "GND", less strict). Only `R32` (`ILIMCOMP/ISET`) correctly returns to `PWR_NEG`. `PGND` and `PWR_NEG` are separate KiCad nets with no direct tie anywhere else in the design, so these dividers were referencing a noisier, effectively-floating-relative-to-AGND power-ground island rather than a clean analog reference. The bench `PGND`-`AGND` jumper is effectively the single-point ground tie the datasheet calls for in Section 9.9.1.3 and corrects all four resistors' reference at once; carry this forward as an intentional Rev 1 layout decision (a short, single-point link near U6), not an ad hoc bodge. See `SOLAR_HANDOFF_2026-09-01.md` for the full net-by-net table.

Separate finding (2026-09-01, unrelated to I2C/ADDR strap): `U6` pin 7 (`MODE`) and pin 16 (`ILIMCOMP/ISET`) share one net, `Net-(U6-ILIMCOMP_ISET)`, alongside `R32`. `MODE` must be a clean digital logic input per the datasheet ("Do not leave this pin floating") and must not share a node with the analog `ISET`/compensation pin. This is a distinct wiring defect, not yet fixed on bench or in schematic; verify with a meter whether U6 pin 7 is truly shorted to pin 16 on the physical board, and separate them in the schematic regardless. See `SOLAR_HANDOFF_2026-09-01.md` for rework steps.
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
| High-side disconnect FET Vds rating | Sized for PV worst-case plus transient margin; 60V class is a better fit for the stated 50V ceiling | Sized for PV worst-case plus transient margin; 60V class is a better fit for the stated 50V ceiling | Worst-case PV + transient | PARTIAL |

### 4.2 UVLO/EN divider values

Use datasheet equations for the selected part and record calculated values.

| Item | Target | Calculated | Implemented | PASS/FAIL |
|---|---:|---:|---:|---|
| UVLO rising threshold | 11.0V to 11.5V (Rev 0 keep converter off below weak-panel region) | 11.2V using R23=1.00M and R24=121k with EN threshold assumed 1.21V | Current schematic still uses R23=75k and R24=21.5k; update pending | PARTIAL |
| UVLO falling threshold | 10.0V to 10.6V | 10.4V target after adding hysteresis resistor (bench trim expected) | Not yet implemented in schematic | OPEN |
| Hysteresis width | 0.7V to 1.0V | 0.8V target from EN hysteresis injection path | Not yet implemented in schematic | OPEN |
| EN pull-up/down network | High-value divider to reduce standby loss at high VIN | Divider current ~= 45uA at 50V with 1.121M total (1.00M + 121k) | Current divider is low-value legacy set (75k/21.5k); migrate during next value pass | PARTIAL |

Assumptions and notes:
- UVLO numbers above are Rev 0 starting targets for LM51772 bench bring-up, not final production limits.
- The 1.00M/121k divider aligns with the low-Iq guidance for wide-input operation and is intentionally conservative for early hardware protection.
- Falling threshold and hysteresis are marked OPEN until the exact datasheet hysteresis equation is applied and bench-verified.

### 4.3 Power-stage timing and control values

| Item | Old Value | New Value | Reason for Change | PASS/FAIL |
|---|---:|---:|---|---|
| Switching frequency setting components | R28=51.7k (current schematic, RT pin) | Keep R28=51.7k for first Rev 0 bring-up pass | Preserves known-good WEBENCH starting point while migration risk is isolated to control-rail and naming changes | FAIL |

Correction (2026-09-01): `R28`=51.7 kOhm is not the LM51772-Q1 WEBENCH-recommended value; it exactly matches the **LM851772-Q1** WEBENCH export's `Rt` (51.7 kOhm) instead of the LM51772-specific WEBENCH `Rt` (51.0 kOhm). This is evidence `R28` was never recalculated when migrating from LM851772 to LM51772 and is carried over from the earlier design. The two values are close enough that switching frequency is probably not grossly wrong, but this should be confirmed/recalculated for LM51772-Q1 specifically before Rev 1 lock rather than assumed correct. See `SOLAR_HANDOFF_2026-09-01.md` WEBENCH comparison table.
| Soft-start capacitor | C26=33nF (current schematic) | C26=100nF target | Extend startup ramp toward ~100ms class startup to reduce inrush stress during first hardware tests | OPEN |
| Slope compensation related values | Existing network tied to MODE/ILIMCOMP_ISET (R29/R32/C29 net context) | Keep existing slope path for first power-up; tune only if subharmonic behavior appears | Avoids coupled loop changes before baseline startup is verified | PARTIAL |
| Current limit scaling | R32=100k (current schematic value; R33 belongs to `ADDR_SLOPE_CFG1`, see I2C/control row, not to ISET scaling) | Keep initial R32 value; adjust after measured current-limit threshold test | Migration changes controller family variant; threshold transfer must be validated on bench data | FAIL |

Correction (2026-09-01): the `ILIMCOMP/ISET` net (`R32`, pin 16) was found netlist-shorted to `MODE` (pin 16 and pin 7 share `Net-(U6-ILIMCOMP_ISET)` in `hardware/kicad/solar-project/report.txt`), and bench meter inspection confirmed this is a genuine physical PCB trace short, not a labeling artifact. `MODE` is a datasheet-required digital logic input ("Do not leave this pin floating") and must not share a node with the analog `ISET`/compensation pin. This must be separated (cut trace + reroute `MODE`) before the current-limit threshold can be trusted. After cutting, `ILIMCOMP/ISET` must **not** be left floating (see `SOLAR_HANDOFF_2026-09-01.md` for the floating-pin risk and the two acceptable terminations: keep `R32` to AGND for active ISET current limiting, or tie directly to VCC2/<50kOhm pull-up to VCC2 to disable the block per datasheet Section 7.3.12).

WEBENCH cross-check (2026-09-01): neither the LM51772-Q1 nor the LM851772-Q1 TI WEBENCH reference design (`docs/WBDesignLM51772.pdf`, `docs/WBDesignLM851772.pdf`) includes an ISET resistor in its BOM — both tie `ILIMCOMP`/`ILIMCOMP_ISET` directly to `VCC2` to disable current limiting/monitoring by default. This project's `R32` = 100 kOhm to AGND is a deliberate addition beyond both WEBENCH baselines to enable real average current limiting; it is a legitimate design choice on its own but was evidently never checked for interaction with the neighboring `MODE` pin, which is how the trace short went unnoticed. See `SOLAR_HANDOFF_2026-09-01.md` "Cross-Check Against TI WEBENCH Reference Schematics" for the full config-resistor comparison table (`R28`/`R29`/`R30`/`R32`/`R33` vs. both WEBENCH exports), including the finding that `R28` (RT = 51.7 kOhm) exactly matches the LM851772 WEBENCH value rather than the LM51772-specific 51.0 kOhm value, suggesting it is an unmigrated leftover from the original LM851772-based design.

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
