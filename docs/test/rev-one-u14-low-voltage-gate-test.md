# Rev 1 U14 Low-Voltage Gate Test

Date: 2026-11-07 (review update)
Status: Phil considers Q5/Q6 working and reports a full-current pass; further Q5/Q6 bench testing is stopped for now. The schematic's D5 polarity symbol is reported wrong, the intended orientation is unresolved, and exact current/test conditions are not documented. Standalone gate-drive, default-off, clamp, and reverse-flow validation are deferred. **Do not run this procedure until Section 1.1 is cleared.**
Scope: Verify the LM74502 U14 gate path for the BSC040N10NS5 Q5/Q6 back-to-back pair before applying PV or enabling the LM51772 power stage.

This procedure covers standalone U14 gate testing and does not authorize PV input, battery connection, charger switching, or loaded operation. The separately reported timed output-path functional check is recorded in `docs/test/rev1-bridge-bench-findings-2026-10-31.md`, Section 11.

The old 2026-09-17 note that Rev 1 hardware did not exist is superseded by later bench records. `docs/test/rev1-bridge-bench-findings-2026-10-31.md` records converter operation with TP1 (`V_out_sense`) jumpered to `CHG_OUT_POS` and PA8 locked low. That jumper bypasses Q5, Q6, and U14, so those converter runs do not establish Q5/Q6 gate-drive or reverse-flow behavior. The installed U14/Q5/Q6 parts and the physical disabled-state path remain unverified.

The 2026-11-07 timed run with the bypass jumper removed reportedly produced 12.78 V at both TP1 and `CHG_OUT_POS` with a 100 Ohm load. D5 was described as backwards, but the schematic's D5 polarity symbol is reported wrong, so its orientation relative to design intent is unresolved. Phil subsequently reported a full-current pass and accepts Q5/Q6 as good for now; detailed current and test conditions were not recorded. Further Q5/Q6 testing is stopped while work moves to ESP32 Wi-Fi. Standalone gate-drive and disabled-state checks below are deferred.

## 1. Hard Limits

- Keep PV, battery, charger output, inverter, and external loads disconnected.
- Do not connect any bench supply until the board-level injection point, isolation, grounding, and connected-node ratings have been reviewed as required by Section 1.1.
- Do not connect the 48 V PV source during this test.
- Do not command LM51772 switching or any MCU charger-enable output.
- Stop for current-limit events, unexpected heating, uncontrolled Q5/Q6 gate voltage, or any `GATE-SRC` voltage approaching U14's 15 V absolute maximum.

### 1.1 Preconditions Before Any Energization

The Rev 1 PCB maps U14 `VS` to `V_out_sense`, U14 `GND` to `PWR_NEG`, U14 `EN_UVLO` to `V_out _on`, and U14 `SRC` to the Q5/Q6 common-source node. Therefore, supplying U14 at `VS` is a board-level injection into the output-side network, not an isolated IC-only test input.

Do not run this test until all of the following are true:

1. The actual board supply entry and all paths it can energize have been traced against the production release and the board's wiring.
2. The bench supply's output isolation and grounding behavior are known, and its current limit is appropriate for the identified injection point. Set and monitor this limit manually; the firmware's 1-second timeout does not enforce a supply-current limit.
3. The selected test voltage is justified against every connected component and board node. U14's own operating range is not a board-level voltage recommendation.
4. The intended `V_out _on` enable/disable method is known to keep U14 off by default and cannot enable LM51772 switching.
5. Scope probing can measure `GATE-SRC` and `VCAP-VS` without grounding or shorting either floating node.

The October board notes list a generic 30 V / 10 A bench supply but do not establish output isolation or a safe U14 test connection. Do not infer either from the supply's front-panel ratings.

## 2. Required Equipment

| Item | Requirement |
|---|---|
| Bench supply | Isolated, current-limited; connection, voltage, and limit selected only after Section 1.1 is cleared |
| Oscilloscope | At least two channels; differential probes rated for the test voltage to measure GATE-to-SRC and VCAP-to-VS |
| Current measurement | Bench-supply current readback or ammeter with mA resolution |
| DMM | Supply and board-node checks; measure VCAP relative to VS where applicable |
| Board state | Rev 1 board; verify the fitted U14, Q5/Q6, R2, R65, and D5 before testing |

### 2.1 Datasheet Reference Values

Extracted from the local LM74502H datasheet (`hardware/kicad/solar-project/lm74502.pdf`, Rev. SNOSDE5A, revised May 2022; confirmed text-extractable with `pypdf` `extraction_mode="layout"` on 2026-09-17) and the D5 datasheet (`hardware/kicad/solar-project/1sma4742a-zener-series.pdf`, ICHJC 1SMA4728A-4764A family). See `docs/decisions-log.md` DEC-037/DEC-038 for the D5 margin decision and a variant-row correction.

U14 is confirmed `LM74502HDDFR` (the "H" variant, LCSC `C5219202` per `docs/rev-one-bom.md`) - use the LM74502H-specific rows below, not the base LM74502 rows, where the datasheet lists both.

| Parameter | Value | Notes |
|---|---|---|
| VS to GND absolute max | -65 V to 65 V | U14 pin limit only; not a board-level test range |
| VS recommended operating | 4 V to 60 V | U14 pin range only; it does not select or authorize a board test voltage |
| **GATE to SRC absolute max** | **0 V to 15 V** | Confirmed boundary this test must never exceed |
| VCAP to VS absolute max | -0.3 V to 15 V | External cap >=0.1 uF required for VCAP |
| VCAP-VS charge pump turn-on / turn-off voltage | 10.3-13 V / 11-13.9 V | Raw (unclamped) charge-pump range; datasheet overview text states approximately 13 V nominal gate-drive design target - D5 clamps this down, it is not just a fault backstop |
| BSC040N10NS5 Q5/Q6 VGS absolute maximum | -20 V to 20 V | MOSFET rating; U14's lower 15 V GATE-to-SRC absolute maximum is the limiting gate-drive constraint |
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

### 3.1 Disabled-State Reverse-Flow Expectation (Desk Review Only)

The current Rev 1 PCB data maps Q5 pads 1-3 to `SRC`, pad 4 to the shared gate, and pad 5 to `V_out_sense`; Q6 pads 1-3 also map to `SRC`, pad 4 to the shared gate, and pad 5 to `CHG_OUT_POS`. This is the intended common-source, back-to-back N-MOS arrangement.

With both gates held at `SRC`, the opposing body diodes should block steady-state current through the pair in either direction. When both gates are driven on, the MOSFET channels provide a bidirectional path. This is a schematic/PCB expectation only; it does not prove the fitted devices, soldering, absence of alternate board-level paths, or actual reverse-flow behavior. Physical closure still requires the separate low-energy static check in issue #9 before battery or PV connection.

### 3.2 Unpowered Endpoint Check (2026-11-07)

With the TP1-to-`CHG_OUT_POS` jumper removed, the bench supply switched off and disconnected, and no battery or PV connected, the reported DMM readings were:

| Check | Result |
|---|---|
| `PV_IN_POS` to `PWR_NEG` | 0 V |
| TP1 (`V_out_sense`) to `PWR_NEG` | 0 V |
| TP1 to `CHG_OUT_POS`, one probe orientation | approx. 500 kOhm |
| TP1 to `CHG_OUT_POS`, probes reversed | approx. 420 kOhm |

These readings show no direct low-resistance connection between the endpoints in this unpowered state. The polarity-dependent high resistance may include other board circuitry; it does not verify each MOSFET, the gate state, or reverse-flow blocking under voltage. Keep the hardware result **open**.

### 3.3 Timed Output-Path Check and Bench Disposition (2026-11-07)

Phil reports a timed run with the TP1-to-`CHG_OUT_POS` bypass jumper removed and a 100 Ohm load: TP1 (`V_out_sense`) measured 12.78 V and `CHG_OUT_POS` measured 12.78 V. D5 was described as backwards, but its schematic polarity symbol is reported wrong, so whether the fitted diode was reversed relative to design intent is unresolved. Phil's current disposition is that Q5/Q6 work and passed full current; he accepts the result as good for now and is moving on to ESP32 Wi-Fi. The exact current, supply limit, firmware build ID, command, run duration, and load connection point were not recorded. The full-current pass is recorded as Phil's reported bench result, not a quantified rating. Default-off behavior, reverse-flow blocking, gate waveform, D5 clamp pulse energy, thermal margin, and rated-current details are deferred.

## 4. Test Sequence

**Firmware gate:** Use only a build configured for the TP1-to-`CHG_OUT_POS` bypass jumper to be removed. The local `u6q5q6-1` build leaves PA8 (`V_out _on`) low at boot and enables the output path only during the timed `U6 TEST` run; the run timeout is 1 second, and stop/fault paths re-inhibit U6 and turn the output path off. The reported 12.78 V run's build ID was not recorded, so do not assume it used `u6q5q6-1`; verify the UART `BUILD` ID before repeating. The prior `u6diag-2` build holds PA8 low and cannot exercise Q5/Q6.

### U14-1 - Auxiliary Power and Default-Off

1. Apply only the reviewed, isolated, current-limited supply at the approved board injection point, with the board otherwise de-energized.
2. Record the approved injection point, supply voltage/current limit, U14 `VS`, `SRC`, and `VCAP-VS`.
3. With `V_out _on` held in its reviewed disabled state, measure `GATE-SRC`.

Pass: current is stable, `VCAP-VS` is within the LM74502 operating range, and `GATE-SRC` remains near 0 V.

### U14-2 - Controlled Enable

1. Enable U14 only through the reviewed `V_out _on` control method; do not jumper or inject directly at U14 pins.
2. Scope `GATE-SRC` and `VCAP-VS` simultaneously using the reviewed probe setup.
3. Record the gate rise/fall waveform and peak `GATE-SRC` voltage.
4. Confirm both Q5 and Q6 gates move together.

Pass: gate drive is controlled, both gates track, and peak `GATE-SRC` remains below U14's 15 V absolute maximum with the required margin; the MOSFET's higher VGS rating does not relax this limit.

### U14-3 - Clamp Check

1. Capture gate turn-on and turn-off transitions at the highest board-level test voltage approved under Section 1.1; do not use U14's 60 V operating maximum as the test voltage.
2. Measure D5 clamp voltage directly as `GATE-SRC`.
3. Do not claim D5 pulse-energy closure from clamp voltage or temperature alone. Record any validated current-measurement method and waveform needed to estimate pulse energy; do not insert a shunt or alter the gate network without a separate review.
4. Record D5 and R65 temperature after repeated enable/disable cycles.

Pass: D5 clamps the gate-source voltage without excessive ringing or heating. Do not accept the nominal 12 V zener value without pulse-energy evidence.

### U14-4 - Gate-Load and Timing Check

1. Repeat the enable/disable test with both BSC040N10NS5 gates connected.
2. Measure gate transition time, overshoot, undershoot, and ringing.
3. Repeat at least ten enable/disable cycles while monitoring supply current and temperature.
4. Do not apply switching-node voltage or load current during this test.

Pass: no abnormal current growth, latch-up, uncontrolled gate state, or thermal rise; gate timing is suitable for the intended U14 control function.

## 5. Stop Conditions

Stop immediately if any of the following occurs:

- `GATE-SRC` approaches or exceeds U14's 15 V absolute maximum.
- The probe setup cannot measure `GATE-SRC` or `VCAP-VS` without connecting a probe ground to a floating node.
- U14 or the MOSFET gates remain partially driven after disable.
- Q5/Q6 gates do not track each other.
- D5 or R65 overheats or shows visible stress.
- Auxiliary supply current reaches its limit unexpectedly.
- Any PV, battery, or charger voltage appears on the power stage.
- The board supply entry, isolation, or enable-control behavior differs from the reviewed setup.

## 6. Bench Record

| Measurement | Result |
|---|---|
| Approved supply injection point and isolation |  |
| Auxiliary supply voltage/current limit |  |
| U14 VS/SRC voltage |  |
| U14 VCAP-VS voltage |  |
| U14 disabled GATE-SRC |  |
| Enabled GATE-SRC peak |  |
| Q5 gate waveform |  |
| Q6 gate waveform |  |
| D5 clamp voltage |  |
| Probe type and reference points |  |
| Gate rise/fall time |  |
| Repeated-cycle current |  |
| Q5/Q6/R65/D5 temperature |  |
| Pass/fail and anomalies |  |
