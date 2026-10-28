# Rev 1 U14 Low-Voltage Gate Test

Date: 2026-09-11
Status: bench procedure draft
Scope: Verify the LM74502 U14 gate path for the BSC040N10NS5 Q5/Q6 back-to-back pair before applying PV or enabling the LM51772 power stage.

This procedure does not authorize PV input, battery connection, charger switching, or loaded operation.

**2026-09-17 note: this test cannot be run on the Rev 0 board.** Checked the as-manufactured Rev 0 schematic copy in this directory (`hardware/kicad/solar-project/Solar Project Rev0 - Manufactured First Build.kicad_sch`) directly: zero instances of `LM74502`, `U14`, or `BSC040N10NS5` exist anywhere in it. Rev 0 used a different, discrete P-MOS/N-MOS high-side switching arrangement for this function (confirmed by `docs/rev-one-bench-learning-matrix.md` row 5, which documents the LM74502H+Q5/Q6 back-to-back design as a Rev 1 rework of Rev 0's ambiguous discrete gate-reference approach, not a carried-over circuit). Rev 0 does have three `1SMA4742A` zener instances, but in that different topology, not in this U14/Q5/Q6 configuration - do not substitute those for this test's D5. This test requires actual Rev 1 hardware, which has not been fabricated yet (Gate 4 PCB layout has not started as of this date) - it stays blocked until Rev 1 boards exist and are assembled.

## 1. Hard Limits

- Keep PV, battery, charger output, inverter, and external loads disconnected.
- Use only an isolated, current-limited auxiliary bench supply for the control/protection domain.
- Do not connect the 48 V PV source during this test.
- Do not command LM51772 switching or any MCU charger-enable output.
- Stop for current-limit events, unexpected heating, uncontrolled Q5/Q6 gate voltage, or any gate-source voltage above the MOSFET absolute maximum.

## 2. Required Equipment

| Item | Requirement |
|---|---|
| Bench supply | Isolated, current-limited, set within the 4 V-60 V VS operating window (see Section 2.1); record limit before connection |
| Oscilloscope | At least two channels; differential or properly rated probes for GATE-to-SRC and VCAP-to-GND |
| Current measurement | Bench-supply current readback or ammeter with mA resolution |
| DMM | Supply, SRC, VCAP, and gate continuity checks |
| Board state | Saved schematic/netlist with Q5/Q6=`BSC040N10NS5`, R2=`100 kOhm`, R65=`10 Ohm`, and D5=`1SMA4742A` |

### 2.1 Datasheet Reference Values

Extracted from the local LM74502H datasheet (`hardware/kicad/solar-project/lm74502.pdf`, Rev. SNOSDE5A, revised May 2022; confirmed text-extractable with `pypdf` `extraction_mode="layout"` on 2026-09-17) and the D5 datasheet (`hardware/kicad/solar-project/1sma4742a-zener-series.pdf`, ICHJC 1SMA4728A-4764A family). See `docs/decisions-log.md` DEC-037/DEC-038 for the D5 margin decision and a variant-row correction.

U14 is confirmed `LM74502HDDFR` (the "H" variant, LCSC `C5219202` per `docs/rev-one-bom.md`) - use the LM74502H-specific rows below, not the base LM74502 rows, where the datasheet lists both.

| Parameter | Value | Notes |
|---|---|---|
| VS to GND absolute max | -65 V to 65 V | Do not approach; bench supply must stay well under 60 V recommended max |
| VS recommended operating | 4 V to 60 V | Set bench supply within this window; 4 V is the minimum for regulated U14 operation |
| **GATE to SRC absolute max** | **0 V to 15 V** | Confirmed boundary this test must never exceed |
| VCAP to VS absolute max | -0.3 V to 15 V | External cap >=0.1 uF required for VCAP |
| VCAP-VS charge pump turn-on / turn-off voltage | 10.3-13 V / 11-13.9 V | Raw (unclamped) charge-pump range; datasheet overview text states approximately 13 V nominal gate-drive design target - D5 clamps this down, it is not just a fault backstop |
| External MOSFET max VGS rating (datasheet's own design assumption) | 15 V | Matches BSC040N10NS5 VGS rating intent |
| GATE peak source current (LM74502H) | 3 mA min / 11 mA max (no typ given) | Confirmed variant-specific row; the base-part row (40/60/77 uA) does NOT apply to our fitted `LM74502HDDFR` |
| GATE peak sink current (turn-off, EN High->Low) | 2370 mA max (~2.37 A) | Via internal 0.4-2 Ohm discharge switch pulling GATE to SRC; consistent with V/R physics (~5 V / ~2 Ohm). Does not flow through D5 - relevant to R65/Q5/Q6 thermal in U14-4, not the D5 clamp check |
| D5 (`1SMA4742A`) Vz min/nom/max | 11.4 / 12.0 / 12.6 V | At IZT=21 mA test current |
| D5 IZM (max admissible DC current) | 79 mA | Datasheet gives no zener pulse-energy/IFSM rating - clamp pulse energy is a bench-only measurement (U14-3). U14's own 11 mA max GATE source current stays below D5's 21 mA IZT, so this 79 mA figure is an abstract ceiling on D5's own rating, not a current U14 can actually deliver |
| D5 margin vs. 15 V GATE-to-SRC abs max | approx 2.4 V (~19%) under confirmed normal-operation current (<=11 mA, below IZT) | Tight by typical >=20% derating practice but expected to hold under normal (non-fault) U14 drive; still a bench-verification item for pulse energy, not a closed row |

## 3. Pre-Power Checks

1. Confirm no PV, battery, charger-output, or external load wiring is connected.
2. Verify Q5 and Q6 are both BSC040N10NS5 and use the intended TDSON-8 footprint.
3. Verify the common source node is `SRC`.
4. Verify Q5 and Q6 gates share the gate node.
5. Verify U14 GATE reaches the shared gate through R65=`10 Ohm`.
6. Verify R2=`100 kOhm` connects from the shared gate to `SRC`.
7. Verify D5 is connected between the shared gate and `SRC` with the intended polarity.
8. Measure unpowered resistance from GATE to SRC and record it.

Pass: no unexpected short, no gate-to-source diode orientation error, and no connection from U6 gate outputs to Q5/Q6.

**2026-09-17 desk pre-check (netlist-verified, not a substitute for step 8's physical continuity measurement):** re-verified items 2-7 directly against `Solar Project Rev1.net` (the confirmed-current netlist, not the stale `-current.net` snapshot - see DEC-043). All PASS: Q5/Q6 are `BSC040N10NS5` (netlist part/datasheet fields confirmed). Net `SRC` (code 101) ties D5 pin 1, Q5 source pins 1-3, Q6 source pins 1-3, R2 pin 1, and U14 pin 8 (`SRC_8`) together - the common source node is correct. Net `Net-(Q5-G)` (code 42) ties D5 pin 2, Q5 pin 4 (`G_4`), Q6 pin 4 (`G_4`), R2 pin 2, and R65 pin 2 together - confirms Q5/Q6 gates share one node, R2 (100 kOhm) bridges gate-to-SRC, and D5 sits directly across gate-to-SRC. Net `Net-(U14-GATE)` (code 87) ties R65 pin 1 to U14 pin 6 (`GATE_6`) - confirms the `R65` series path from U14's `GATE` output to the shared gate node. Item 8 (unpowered GATE-SRC resistance) is a physical bench measurement and remains unrecorded until the bench session runs.

## 4. Test Sequence

### U14-1 - Auxiliary Power and Default-Off

1. Apply the isolated current-limited auxiliary supply with the board otherwise de-energized.
2. Record supply voltage, current, U14 `VS/SRC`, and `VCAP`.
3. With U14 disabled or before its enable condition is met, measure `GATE-SRC`.

Pass: current is stable, VCAP is within the LM74502 operating range, and `GATE-SRC` remains near 0 V.

### U14-2 - Controlled Enable

1. Enable U14 using the documented EN/UVLO condition without applying PV or battery power.
2. Scope `GATE-SRC`, `VCAP`, and `SRC` simultaneously.
3. Record the gate rise/fall waveform and peak `GATE-SRC` voltage.
4. Confirm both Q5 and Q6 gates move together.

Pass: gate drive is controlled, both gates track, and peak `GATE-SRC` remains below the BSC040N10NS5 VGS rating.

### U14-3 - Clamp Check

1. Capture the first gate turn-on and turn-off transitions at maximum allowed auxiliary test voltage.
2. Measure D5 clamp voltage directly as `GATE-SRC`.
3. Estimate clamp pulse energy from the measured current or the gate-charge transient and pulse width.
4. Record D5 and R65 temperature after repeated enable/disable cycles.

Pass: D5 clamps the gate-source voltage without excessive ringing or heating. Do not accept the nominal 12 V zener value without pulse-energy evidence.

### U14-4 - Gate-Load and Timing Check

1. Repeat the enable/disable test with the two BSC040N10NS5 gates connected.
2. Measure gate transition time, overshoot, undershoot, and ringing.
3. Repeat at least ten enable/disable cycles while monitoring supply current and temperature.
4. Do not apply switching-node voltage or load current during this test.

Pass: no abnormal current growth, latch-up, uncontrolled gate state, or thermal rise; gate timing is suitable for the intended U14 control function.

## 5. Stop Conditions

Stop immediately if any of the following occurs:

- `GATE-SRC` exceeds the BSC040N10NS5 VGS absolute maximum.
- U14 or the MOSFET gates remain partially driven after disable.
- Q5/Q6 gates do not track each other.
- D5 or R65 overheats or shows visible stress.
- Auxiliary supply current reaches its limit unexpectedly.
- Any PV, battery, or charger voltage appears on the power stage.

## 6. Bench Record

| Measurement | Result |
|---|---|
| Auxiliary supply voltage/current limit |  |
| U14 VS/SRC voltage |  |
| U14 VCAP voltage |  |
| U14 disabled GATE-SRC |  |
| Enabled GATE-SRC peak |  |
| Q5 gate waveform |  |
| Q6 gate waveform |  |
| D5 clamp voltage |  |
| Gate rise/fall time |  |
| Repeated-cycle current |  |
| Q5/Q6/R65/D5 temperature |  |
| Pass/fail and anomalies |  |
