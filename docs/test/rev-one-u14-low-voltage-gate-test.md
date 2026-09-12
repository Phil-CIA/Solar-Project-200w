# Rev 1 U14 Low-Voltage Gate Test

Date: 2026-09-11
Status: bench procedure draft
Scope: Verify the LM74502 U14 gate path for the BSC040N10NS5 Q5/Q6 back-to-back pair before applying PV or enabling the LM51772 power stage.

This procedure does not authorize PV input, battery connection, charger switching, or loaded operation.

## 1. Hard Limits

- Keep PV, battery, charger output, inverter, and external loads disconnected.
- Use only an isolated, current-limited auxiliary bench supply for the control/protection domain.
- Do not connect the 48 V PV source during this test.
- Do not command LM51772 switching or any MCU charger-enable output.
- Stop for current-limit events, unexpected heating, uncontrolled Q5/Q6 gate voltage, or any gate-source voltage above the MOSFET absolute maximum.

## 2. Required Equipment

| Item | Requirement |
|---|---|
| Bench supply | Isolated, current-limited, set to the minimum valid U14 operating voltage from the LM74502 datasheet; record limit before connection |
| Oscilloscope | At least two channels; differential or properly rated probes for GATE-to-SRC and VCAP-to-GND |
| Current measurement | Bench-supply current readback or ammeter with mA resolution |
| DMM | Supply, SRC, VCAP, and gate continuity checks |
| Board state | Saved schematic/netlist with Q5/Q6=`BSC040N10NS5`, R2=`100 kOhm`, R65=`10 Ohm`, and D5=`1SMA4742A` |

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
