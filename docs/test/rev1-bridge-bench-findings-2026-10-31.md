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
