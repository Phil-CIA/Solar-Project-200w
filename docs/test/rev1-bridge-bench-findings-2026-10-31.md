# Rev 1 Bridge Bench Findings

Date: 2026-10-31
Status: findings and open hypotheses; not a pass record
Scope: Rev 1 LM51772 (U6) bridge first-output bring-up, no battery, `V_out_sense` jumpered to `CHG_OUT_POS`, PA8 locked low in firmware.

This note records bench evidence and analysis. Items marked **hypothesis** are unverified. M1-M4 are treated as good by the board owner; no FET checks were repeated.

## 1. Bench Observations

| Run | Conditions | Observation |
|---|---|---|
| A | 12.8 V / 5 A PSU, no load | `TP1` about 3.1 V, `CHG_OUT_POS` about 0.8 V, PSU current limiting, `STATUS_BYTE` = 0x68 |
| B | 12.8 V / 2 A PSU, 10 ohm load fitted | `CHG_OUT_POS` peaks 8.0 V (target about 4.84 V), min -0.88 V; roughly 2 kHz burst pattern |
| C | 12.8 V / 2 A PSU, no load | Output ramps about 0 to 3.6 V in about 2 ms, holds about 0.8 ms, then breaks down; `PV_IN_POS` swings 7.76 V to 16.4 V |
| D | 6.5 V to 9.5 V, 300 ms timed START | Stable; PSU about 75-81 mA; `TP1` about 3.38 V on DMM; `STATUS_BYTE` stayed 0x48; ARM set `D7` = 0x23 |
| E | about 10.5 V | Output ramps to about 3.2 V, holds about 0.9 ms, disturbance at about 2.9 ms after start; `PV_IN_POS` 13.3 V max, 8.56 V min; output then sits near 3.4 V with large ripple. Outcome beyond the 6 ms window not recorded. |

## 2. Findings

1. **IVP is disabled.** Snapshot `MFR_SPECIFIC_D3` (0xD3) = 0x20, so `EN_IVP` (bit 7) = 0. `IVP_VOLTAGE` (0xDA) = 0xFF, about 36.6 V (4.75 V + 0.125 V x code). The LM51772 IVP is intended for output-to-input current when the source cannot sink. `PV_IN_POS` rising above the supply (13.3 V and 16.4 V) is consistent with that reverse flow in forced PWM (`MODE` strapped high). **Hypothesis:** this explains the input spikes, not the root cause of the loss of control.
2. **Output regulates near 3.4 V, not 4.84-5.0 V**, in both the scope plateau and the DMM reading. Cause unknown. Next check: measure U6 pin 14 (FB) at the plateau. About 1.0 V means the divider/reference is the issue; well below 1.0 V means something is limiting the output.
3. **Event timing repeats.** Runs C and E both show ramp, plateau, then a disturbance about 3 ms after start; only amplitude changes with Vin. The behavior is Vin-dependent (clean below about 10 V in run D, disturbed at about 10.5 V and above).
4. **Shoot-through is a suspect, not shown.** Series gate resistors R69-R72 (10 ohm) with CSD18540Q5B Ciss 3.25-4.23 nF give a gate time constant of about 41-53 ns. LM51772 shoot-through detection senses the IC pin (37% of VCC), upstream of those resistors, with a minimum dead time of about 20 ns (`SEL_MIN_DEADTIME_GDRV`, `D6` bits [3:2], read 0x15). Rev 0 had no gate resistors and showed a similar symptom, so the resistors alone cannot be the common cause. Run D suggests no gross shoot-through at low Vin.
5. **Gate and body-diode charge do not explain amps of supply current.** CSD18540Q5B: Qg 20 nC typ / 26 nC max at 4.5 V, Qgd 6.7 nC, Qrr 145 nC (at 28 A, 300 A/us), Qoss 83 nC. Gate drive is about 12-16 mA per FET at 600 kHz.

## 3. Datasheet Corrections to Earlier Notes

- R53 is the 10 kohm `CTRL_3V3` to `FAULT_OCP` pull-up on U6. It must remain fitted and is not a feedback pull-up.
- M1 and M4 body diodes block `PV_IN_POS` to `V_out` pre-charge when off.

## 4. Planned Next Steps (none authorized to run without board-owner approval)

1. Build-only firmware flag `ENABLE_IVP_BENCH` (default off): at ARM, while inhibited, set `D3` bit 7 only (0x20 to 0xA0) and `IVP_VOLTAGE` = 0x4A (about 14.0 V), with readback and abort on mismatch.
2. Optional `D6` bits [3:2] = 0b11 (0x1D, longer dead time) A/B test at low Vin and low current limit, comparing supply current at the output plateau.
3. Scope at 1-2 us/div on `SW_1` and `SW_2` (J6 pins 9 and 10, spring ground) around the plateau, then gate-at-FET versus gate-at-IC-pin probing.
4. Probe U6 pin 14 at the plateau; record `STATUS_BYTE` after each run without clearing faults.

## 5. Update 2026-11-01/02 (loop, compensation, sense-resistor and divider checks)

| Item | Result |
|---|---|
| IVP bench flag | Implemented; pre-run read `D3` = 0xA0, `DA` = 0x4A while inhibited. Post-run and STAT reads showed `D3` = 0x20, `DA` = 0xFF, `0x78` = 0x48 (reset values). Whether ARM writes survive EN release is **unresolved**. |
| 9.5 V scope | Switching about 605 kHz (period about 1.64 us), SW swing about -1.9 V to +12 V with ringing; FB about 0.75 V at plateau; COMP reported pinned low with about 0.8 Vpp activity. |
| SS/ATRK | About 27 Mohm to `PWR_NEG` unpowered (no leakage); about 3.0 V and still rising at 500 us/div. Stall theory **disproved**. |
| Compensation | BOM values R31 = 2.43 kohm, C24 = **22 nF**, C25 = 1.5 nF (`docs/rev-one-component-derating-review.md` previously said 220 nF; corrected). Simple model: zero about 3.0 kHz, crossover about 7 kHz at 664 uF, phase margin about 74 deg (upper bound). Compensation is not the cause. |
| R22 (peak-current shunt) | 1 mohm gives about 1.7 mV at 1.65 A peak; datasheet limit is 50 mV. Replaced with 10 mohm (THT): **same response, output about 3.5 V**. R22 value is not the cause. Slope comp (`D7`) was not re-tuned for 10 mohm (0x2B suggested). |
| FB divider | R3 changed to 82 kohm (R25 20 kohm, 5.1 V target). FB measured 0.697 V at the 3.5 V plateau, consistent with the divider, so the divider is working. |
| Output path | `V_out_sense` is jumpered to `CHG_OUT_POS`, so Q5/Q6/U14 are bypassed. 3.5 V is the converter output. R16 carries the output current. |

Status: the output stops near 3.5 V regardless of the R22 value or the FB target, with FB about 0.7 V against a 1.0 V reference. That points at COMP, SS/ATRK or internal U6 limiting rather than the power stage. Next: scope COMP, SS/ATRK, FB and `CHG_OUT_POS` together at the plateau; measure U6 pin 19 vs pin 20 (ISNSN/ISNSP) to rule out the average-current limiter; confirm the register readback after ARM and during START. R22 (10 mohm) and R3 (82 kohm) are bench rework that differ from the Rev 1 BOM.

Limits for any powered run: low input voltage, low PSU current limit, no load, jumper fitted and later removed before battery tests, `PB12` re-inhibit within 50 ms.

## 6. Update 2026-11-02/03 (external-FB sweeps, switch to I2C/internal FB, Vin dependence)

Bench data was logged by the VS Code agent and relayed by the board owner. Measurements are DMM values unless noted. This section supersedes the "Next" list in section 5.

### 6.1 Theories tested and ruled out

| Change or check | Result |
|---|---|
| SS/ATRK soft-start | Completes: SS reaches 1.0 V in about 3 ms and peaks at about 4 V (clamp). Not the cause. |
| COMP | About 1.08 V at the plateau; COMP is not stuck at a rail. |
| Average current limiter (ILIM) disabled (R68 lifted, U6 pin 16 to VCC2) | Output unchanged at 3.56 V. |
| `D7` slope comp 0x2B for the 10 mohm R22 | Output unchanged at 3.56 V. |
| `VOUT_TARGET1` written to 0x03E8 with external FB | No change (FB 0.69 V, output 3.56 V); with external FB the register is ignored. |
| `D9` PCM window | `D9` already read 0x20 (PCM disabled); writing it changed nothing. The earlier "output = 0.70 x target" PCM theory was a coincidence and is **retracted**. |
| `STATUS_BYTE` 0x58 / 0x48 / 0x08 | Appears on passing and failing runs alike; it is not a failure indicator. |
| START window 300 ms to 1000 ms | Output held at about 4.0 V for the whole window (not still rising, no hiccup). |

### 6.2 External-FB Vin sweep (R3 = 82 kohm, target about 5.1 V, no load)

| Vin | CHG_OUT_POS | FB | PSU current |
|---:|---:|---:|---:|
| 7.0 V | 5.10 V | 0.998 V | 71 mA |
| 8.0 V | 5.10 V | 0.998 V | 54 mA |
| 8.5 V | 5.10 V | 0.998 V | 54 mA |
| 9.0 V | 5.10 V | 0.998 V | 54 mA |
| 9.25 V | 4.27 V | 0.835 V | 55 mA |
| 9.5 V | 4.00 V | 0.778 V | 56 mA |

The converter regulated correctly up to Vin = 9.0 V and failed sharply between 9.0 V and 9.25 V. The earlier "3.5 V plateau" runs were at higher Vin. Scope at Vin = 9.25 V (500 us/div; channel assignment assumed `PV_IN_POS` / `SW_1` / `CHG_OUT_POS`): input flat at about 9.4 V with no spikes; `SW_1` about -1.4 V to +11 V and steady; output a band of about 3.4 V to 5.0 V (scope max 5.08 V) with a centre near the DMM reading. No hiccup or restart. Whether the output band is real jitter or probe pickup is **not resolved**; a 2 us/div shot with a ground spring and 20 MHz BW limit at 9.25 V and 8.5 V was proposed but not taken.

### 6.3 Control mode finding: the converter is configured for I2C

U6 was being run with the external FB divider (R3/R25), which is the standalone mode. The board is built for I2C control (address 0x6A). With internal FB selected, `VOUT_TARGET1` (0x0C LSB, 0x0D MSB[3:0]) sets Vout as code x 20 mV when `D8` = 0x84 (DIV20). The reset value 0x0258 is therefore **12.0 V**, not 0.6 V; any firmware that enables the converter must write and verify a target first. 5.0 V is code 250 (0x00FA). Earlier VOUT_A test writes were harmless only because the external divider ignored them.

Hardware change made for this mode (bench rework, not in the Rev 1 BOM/PCB): R3 lifted and U6 pin 14 (FB) wired to VCC2 (pin 29). Internal FB latches at EN or power-on reset. Before this change, `u6 test 3300/4000/5000` all read 5.1 V (external divider, targets ignored).

Firmware added by the agent: `u6 test <mV>` (refuses targets outside 3300-5500 mV, requires `D8` DIV20, writes and reads back `VOUT_TARGET1`, runs a 1000 ms START with polls, re-inhibits via `PB12`, reports `0x78` and `0x21`, never reads 0x03).

### 6.4 Internal-FB results (no load)

| Vin | Target | CHG_OUT_POS | PSU current |
|---:|---:|---:|---:|
| 8.0 V | 3.30 V | 3.28 V | 53 mA |
| 8.0 V | 4.00 V | 3.97 V | 53 mA |
| 8.0 V | 5.00 V | 4.71 V | 53 mA |
| 7.0 V | 4.20 V | 4.18 V | 54 mA |
| 7.0 V | 4.50 V | 4.48 V | 54 mA |
| 7.0 V | 4.80 V | 4.77 V | 71 mA |
| 7.0 V | 5.00 V | 4.97 V | not recorded |
| 8.0 V | 4.20 V | 4.18 V | 56 mA |
| 8.0 V | 4.50 V | 4.47 V | 56 mA |
| 8.0 V | 4.80 V | 4.71 V | 57 mA |

Vout follows the I2C target within about 1 % at 7 V and below. At 8 V the output stops near 4.7 V. Register reads before the sweep: `D3` = 0x20, `DA` = 0xFF, `D7` = 0x28, `D9` = 0x2C. An earlier 8 V run with `D3` = 0xA0, `DA` = 0x4A, `D7` = 0x2B also capped at 4.71 V, so IVP and slope settings are not the cause. `0x78` read 0x18 at every poll.

### 6.5 Internal-FB results with a load fitted

A load resistor was added from `CHG_OUT_POS` to `PWR_NEG` (about 100 ohm was requested; **the fitted value was not recorded** and must be confirmed). The load was meant to test whether no-load reverse flow in forced PWM was the cause.

| Vin | Target | CHG_OUT_POS | PSU current |
|---:|---:|---:|---:|
| 7.0 V | 5.00 V | 4.14 V | 78 mA |
| 8.0 V | 5.00 V | 4.70 V | 75 mA |
| 9.25 V | 5.00 V | 4.02 V | 72 mA |
| 9.5 V | 4.50 V | 3.89 V | 72 mA |
| 7.0 V | 4.20 / 4.50 / 4.80 V | 4.14 V for all three | 72 / 71 / 78 mA |
| 8.0 V | 4.20 / 4.50 / 4.80 V | 4.17 / 4.46 / 4.68 V | 73 / 75 / 80 mA |

The load did not fix the shortfall. At 7 V the output was lower with the load (4.14 V flat) than without (4.97 V at 5.0 V target), and 9.25 V and 9.5 V still failed. The reverse-flow theory is **not supported**. No abort condition was reported.

### 6.6 Current understanding and open items

- Confirmed: I2C/internal-FB Vout control works for 3.3 to about 5.0 V at Vin of 7 V or below with no load, accurate to about 1 %.
- Confirmed: with the external divider, the converter regulated 5.10 V at Vin up to 9.0 V and failed at 9.25 V and above. With internal FB and no load, 8 V already capped near 4.7 V. The failure threshold therefore depends on more than Vin and target alone and is **not understood**.
- With the load fitted, output drops and PSU current rises to about 72-80 mA at every Vin tested; the cause is unresolved.
- Open items: confirm the load resistor value; record the Vin used for the 3.56 V run (not logged); the 2 us/div zoom shots; PSU current for the 7.0 V / 5000 mV no-load run; whether `D3`/`DA` ARM writes survive EN release (the two sweeps show different register states).
- Bench rework not in the Rev 1 BOM/PCB: R22 = 10 mohm THT, R3 lifted (82 kohm during the external-FB tests), FB (pin 14) to VCC2, ILIM wiring (R68 lifted, pin 16 to VCC2) was a temporary test; confirm whether it was restored.
- Bring-up ceiling used for reliable operation so far: Vin <= 7 V for targets up to 5 V.
- Constraints unchanged: Vin <= 9.5 V, PSU <= 0.5 A, jumper fitted, START <= 1000 ms with `PB12` re-inhibit, no 0x03 reads, no battery, no flashing or KiCad edits without approval.

## 7. Slope compensation, 18-30 V target range, I2C poll failures (2026-11-03)

### 7.1 Slope compensation (D7)

- Subharmonic oscillation (alternating wide/narrow `SW_1` pulses) seen at 8 V / 5000 mV with `D7` = 0x2B. `D7` = 0x2F gave uniform pulses, about 60 % duty, Vout about 5.04 V.
- Slope comp scales with R_CS (datasheet 7.3.16). The shunt is 10 mohm (BOM 1 mohm), so the TI calculator assumption does not apply; sweep `D7` rather than trusting defaults.
- `D7` = 0x2F, 5000 mV sweep: 7 V -> 4.97 V; 8 V -> 4.96 V; 9.25 V -> 4.96 V; 9.5 V -> 4.70 V; 10 V -> 4.42 V (PSU 77-85 mA). At 10 V the waveform is clean, duty < 50 %, no alternating pulse. Cause of the droop is **unexplained**.
- C20 = 180 pF + 180 pF in parallel, unchanged for the whole test series (not new data).

### 7.2 Real operating range

- 18-30 V in, 11.8-14 V out (Li battery charging). Buck duty 0.39 (30 V -> 11.8 V) to 0.78 (18 V -> 14 V). The 5 V / 7-10 V work is only a proxy.
- Proposed staged points: 18->12, 18->14, 24->12, 30->12. Needs new abort limits (Vout abort = target + 1.0 V, PSU abort 0.3 A) and a >= 18 V supply.
- Codes: Vout mV = code x 20; 12000 mV = 0x0258, 14000 mV = 0x02BC.

### 7.3 `u6 test 10000 14` poll failure

- Code 0x01F4 write/readback matched, then `FAULT_LATCHED: U6 test poll read fail` at the first poll (t = 50 ms). Reproduced; early logs were from the old build, diagnostics added as `u6diag-1/2`.
- I2C is bit-bang (no HAL error code). Post-fail dump failed 7 of 8 reads; registers survived (D3/D7/D8/DA, 0D/0C intact; `stat` read cleanly afterward).
- Converter switches about 350 ms at about 600 kHz, about 35 % duty, Vin swing about 1 Vpk, no collapse. Not "one pulse then dead".
- At 10 kHz I2C (`i2c speed 10`) the failure persists (first failed reg moved 0x78 -> 0x21). `D8` read 0x85 vs 0x84 with no write: a read bit-error. U6 answers but data/edges are corrupted during switching; likely noise or ground offset, not timing. 0x78 = 0x58/0x18 is not diagnostic.

### 7.4 PCM / VIN-FB research

- `V_in_FB` (pin 15) divider R18 76.8k / R17 5.62k (gain 0.0682); datasheet says tie VIN-FB to AGND when using the internal Vout divider. Rework (internal FB) leaves it mismatched.
- TI E2E China thread (943530): TI said there is no such use case (internal FB + PCM) and advised an external divider. No TI guidance exists.
- PCM lead is plausible but weak: with the internal 1/20 divider, VIN-FB at 0.068 should read Vin high, keeping it out of the window at Vin above target. Cheapest test: `D9` = 0x20 (PCM off). Fallback (Phil's decision): lift R18, tie pin 15 to AGND.
- E2E 1660263 (TI): CSA line sharing SW1 injects gate-return current; gate loops/switch-node area. E2E 1692810: logic-level FETs only, phase margin > 60 deg wanted.

### 7.5 Current-sense routing (read-only PCB check)

- R22 at about (151, 71) mm; U6 CSA/CSB, R26/R27, C20 at about (166-171, 95-102) mm: runs of about 30-35 mm.
- `_CSA_2` mainly In1.Cu, `_CSB_2` on B.Cu, 0.2 mm wide: different layers and paths, not a tight Kelvin pair. Proximity to SW nodes not checked. With the 10 mohm shunt the sense signal is 10x the BOM design, so this is a weaker suspect.

### 7.6 Next tests (one variable per run, 10 V / 5000 mV)

1. Baseline `D7` = 0x2F, log D9, real Vin/Vout/PSU current, VCC2.
2. `D9` = 0x20.
3. `D7` sweep 0x2B...0x2F and lower.
4. Regression `u6 test 5000 8` on `u6diag-2` (default and 10 kHz).
5. Blind `u6 test` (no I2C during START), scope `SW_1`, `CHG_OUT_POS`, `PV_IN_POS`; scope SDA/SCL with ground at U6 AGND; DMM MCU GND to U6 AGND; DMM VCC2 at 8/10/14 V.
6. Unrecorded: load resistor value, PSU limit, ILIM rework restored?, D9 during `D7` = 0x2F runs.

## 8. VIN-FB divider mismatch: likely cause of Vin-dependent failure (2026-11-04)

- TI LM51772EVM-HP user guide (SNVU852B, schematic p.13): the EVM uses **matched** dividers, FB R1 82.0k / R2 4.30k and VIN-FB R44 82.0k / R18 4.30k (ratio 0.0498, about the internal 1/20). In I2C mode JP6 pins 4-5 put VCC2 on the R_FB node (FB pin 14 through R46 0 ohm) and the VIN-FB divider stays fitted. EVM also uses 3.3 uH, 600 kHz, R6 = 10 mohm, 3 ohm gate resistors, 10 ohm / 680 pF CSA/CSB filters.
- Rev 1 VIN-FB divider was R18 76.8k / R17 5.62k = 0.0682 (about 1/14.7), mismatched to the internal 1/20 divider. This made U6 see Vin about 1.36x too high.
- Rework: R18 = 100k, R17 = 5.1k (ratio 0.0485, within about 3 % of 1/20). Nothing else changed.
- **Result (Phil, 2026-11-04): the converter now runs regardless of Vin.** [Record exact Vin/Vout/PSU current points tested and the D7/D9 values used.]
- Supersedes 7.4: the datasheet advice to tie VIN-FB to AGND is not what TI does on the EVM; a matched about 1/20 divider is. Open: confirm D7 can be lowered toward the EVM default, re-check efficiency (earlier 25-40 %), and the 18-30 V / 11.8-14 V target range.
- Bench rework list now also includes R18 = 100k, R17 = 5.1k.

- **Update 2026-11-04:** after the R18/R17 rework, Vout still falls as Vin rises. A D7 (slope compensation) sweep left Vout unchanged, so slope compensation is ruled out as the droop cause (supersedes 7.1 suspicion). Remaining suspects: CSA/CSB sense noise (EVM filter is 10 ohm / 680 pF, Rev 1 is 180 pF + 180 pF), AGND vs power-ground offset, residual VIN-FB / reference path. [Record the D7 values swept and the Vout/Vin points.]

## 9. CSA/CSB filter capacitor: Vout recovers (2026-11-04)

- Rework: 1 nF added across CSA-CSB at U6 (on the U6 side of R26/R27, which are 10 ohm, same as the EVM). Existing C20 = 180 pF + 180 pF.
- Result (Phil): Vout rose to almost 5 V (target 5000 mV). This supports the sense-noise explanation: a switching spike on CSA/CSB ended pulses early at higher Vin, so Vout fell as Vin rose.
- [Record Vin, exact Vout, PSU current, D7/D9, and whether Vout holds across the Vin range.]
- Open: scope CSA-CSB before/after to confirm the spike; decide the permanent filter (EVM uses 680 pF to ground on each line); check CSA/CSB layout (long, different layers); recheck the 18-30 V / 11.8-14 V range; rerun u6 test 10000 14 for the I2C poll failure; re-sweep D7 now that the sense noise is reduced.

## 10. Shunt size, 2.2 nF, Vout ceiling and fault flags (2026-11-05/06)

- Shunt: 1 mohm made the droop worse (10x less sense signal); returned to 10 mohm. Note 50 mV / R_CS sets the peak limit (10 mohm = 5 A), too low for the 11.8-14 V / ~14 A target; the datasheet example uses 2.5 mohm (2 x 5 mohm). Shunt refdes (R21 vs R22 in the PCB file) unconfirmed.
- Scope: the "800 mV spike" was the Rigol math Diff(CH1) trace at 200 MV/s per div, a dV/dt of a switching edge, not sense noise. CH1/CH2 are ground-referenced at 5 V/div and cannot show mV-level sense noise. Needs a true differential probe at 10-20 mV/div or CSA/CSB to AGND at the U6 pins.
- With 10 mohm and 2.2 nF across CSA/CSB: Vin 19 V, request 5000 mV, Vout = 4.74 V (earlier 4.42 V at 10 V Vin).
- Vout will not go much above about 5.5 V regardless of the request. D8 = 0x84 (DIV20, CDC off), so the divider setting is not the cap.
- STATUS_BYTE (0x78) read as 0x58: OFF (bit 6), IOUT_OC (bit 4), VIN_UV (bit 3) set. This points to protection events and restarts rather than pure regulation noise, and may explain the I2C poll failures. Unknown whether the flags were live or latched; Vin/PSU current limit during the event not recorded.
- Next (one change at a time): add OLED/serial fault display (STATUS_BYTE, STATUS_WORD, detail registers; CLEAR_FAULTS command code to be confirmed from the datasheet), log STATUS at 100 ms for 5 s from a clean start at 12 V and 19 V Vin to see which flag sets first; read back the Vout target register after a 10000 mV request; check Vout ceiling at 12/24 V Vin; confirm the PSU is not in current limit.

## 11. Q5/Q6 switched-output path retest (2026-11-07)

- Test firmware was updated to a timed 10 s output window (`BUILD u6q5q6-10s`) for bench capture.
- With the TP1-to-CHG_OUT_POS bypass jumper removed, early runs showed weak gate enhancement and low output transfer through Q5/Q6.
- During debug, measured voltage across D5 during the run was about 30 mV (clamp not active), and measured Q5/Q6 gate-to-source peak was about 2.39 V in the failing state.
- After rework correction, controlled run at 18 V in, target 12.8 V, with a 100 ohm load gave:
  - `TP1` = 12.78 V
  - `CHG_OUT_POS` = 12.78 V
  - PSU current about 153 mA
- Outcome: Q5/Q6/U14 output path now transfers the commanded voltage during the timed run under load.
- Scope limit: this is an initial functional pass only; it does not by itself prove default-off behavior, reverse-flow blocking, or D5 pulse-energy margin.