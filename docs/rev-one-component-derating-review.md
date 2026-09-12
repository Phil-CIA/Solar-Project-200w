# Rev 1 Component Derating Review

Status: active review worksheet; no component rating is considered locked by this document
Date: 2026-09-09

## Review Basis

Rev 1 is a targeted reconfiguration of Rev 0. The retained topology is reviewed against this envelope:

- 125 W continuous charger output
- 150 W maximum PV operating power
- 8 A to 10 A nominal battery charge current
- Approximately 12 A hardware charge-current protection ceiling
- At least 20 A BMS charge capability
- PV input parts rated for the complete connected-array cold-`Voc` and `Isc` envelope

A part passes only when its worst-case voltage, continuous current, transient current, power loss, temperature, package, PCB copper, and fault duty are documented. Typical datasheet values alone are insufficient.

## Rebaseline Trigger: 12 A Peak Converter Envelope

The legacy converter architecture was carried forward with higher-voltage and higher-current assumptions than the current Rev 1 requirement supports. The present design baseline is a 12 A peak charger, not a 20 A-class power stage. That means the converter path must be re-baselined as a single system around the actual 125 W / 10 A nominal / 12 A ceiling envelope.

For the converter review, this is the governing rule:

- 12 A is the peak ceiling for power-path sizing and protection
- 10 A is the normal operating band
- 125 W continuous is the design target
- legacy 100 V-class, 20 A-class assumptions are not valid unless explicitly re-justified against the lower envelope

This rebaseline applies to the converter support stack: Q5/Q6, R16, L2, F2, output capacitor bank, and U6 current-limit network. The component review will proceed from this lower-current platform and not from the earlier oversized power-stage assumptions.

## Review Status

Use these dispositions:

- `OPEN`: data or calculation is missing
- `PASS`: rating and margin are documented for the Rev 1 envelope
- `REWORK`: schematic, footprint, value, or part selection must change
- `HOLD`: depends on an upstream decision or measurement

## Power-Path Review Order

Review in this order so each component is evaluated in its actual current path:

1. PV connector, F1, PV protection, TVS, and input capacitors
2. Q5/Q6 output switching and reverse-flow stage
3. R16 output-current shunt and Kelvin sense routing
4. F2 charger-output short-circuit fuse and its holder
5. Output capacitors, inductor, and switching-node components
6. LM51772 controller, gate-drive support, current-limit network, and UVLO/enable network
7. Battery connector, bus conductors, battery protection, and control-power pickup
8. Voltage/current/temperature sensing and MCU/control-domain parts

## Component Matrix

| Ref / group | Function and Rev 1 path | Required checks | Evidence | Disposition |
|---|---|---|---|---|
| J1, F1 | PV entry and branch protection | Connector DC voltage/current; fuse DC interrupt rating, time-current curve, cold-`Voc` voltage, PV short-circuit current, inrush duty | Datasheets, panel envelope, fuse curve | OPEN |
| D1/D4 and retained PV protection | PV reverse-polarity / ideal-diode boundary | Reverse voltage including transients, forward current, surge current, dissipation at maximum PV current, thermal path | Datasheet plus loss/temperature calculation | OPEN |
| D2 and C52 | PV/output transient suppression and protected-node capacitance | Standoff voltage, clamp voltage at fault current, pulse energy, capacitor voltage rating, leakage, placement, and C52 margin | D2=`SMBJ60CA` is connected from `PV_IN_POS_FUSED` to `PWR_NEG` and has a listed maximum clamp of `103 V`; it is an input surge clamp, not a sub-60 V Q5/Q6 switch-node clamp. C52 and the switch-node overshoot path still require voltage/transient analysis | HOLD |
| C1-C6 and input ceramic bank | PV-side switching-loop decoupling | DC bias derating, effective capacitance, ripple RMS current, voltage transients, package temperature, loop placement | Capacitor DC-bias data and layout; prototype measurement required | HOLD |
| Q5/Q6 | Accepted schematic topology: `V_out -> R16 -> Q5 -> SRC -> Q6 -> CHG_OUT_POS -> F2 -> BAT_BUS_POS`; output switching and reverse-flow MOSFET stage driven by U14 LM74502 | VDS in both switching and disabled states, VGS limits, body-diode reverse current, continuous and pulsed current, RDS(on) at temperature, SOA, gate-drive timing, reverse-flow blocking state; rebaseline under the 12 A peak / 125 W design envelope | Current netlist now identifies both as `BSC040N10NS5`, 100 V / 4.0 mOhm class, typical Qg 58 nC, TDSON-8 5x6 mm, with U14 `LM74502HDDFR` retaining gate ownership. Infineon product data confirms 100 V, 4 mOhm maximum, 58 nC typical Qg, and SuperSO8 5x6/PG-TDSON-8 package. Candidate selection resolves the nominal 48 V VDS-rating blocker; U14 gate-charge capability, common-source body-diode orientation, VGS clamp behavior, thermal loss, and switching overshoot remain to be verified | HOLD |
| R16 | 1 mOhm output-current shunt; high-current pads 1/4, Kelvin pads 2/3 per actual footprint mapping | Continuous and transient I2R loss, pulse energy, 4-terminal Kelvin routing, sense polarity, copper current sharing, temperature coefficient, 5 W rating at actual thermal conditions | Current netlist, part data, PCB copper review, current-limit calculation | HOLD |
| F2 | Charger-output branch short-circuit protection: `CHG_OUT_POS -> F2 -> BAT_BUS_POS` | Fuse DC voltage rating (14.6V charging max), interrupt rating, continuous 10A / transient 12A ceiling, startup/inrush behavior (soft-start or hard cold-start?), clearing time for charger-side short, holder contact rating, thermal rise, physical placement near CHG_OUT_POS | Current netlist has only a generic `Fuse`, LCSC `C142864`, `Part - USA = TBD`, and a Littelfuse 100-series 5x25 mm holder footprint. The fitted/desired fuse clips are not the selected standard clip style. Replace the holder footprint with the correct standard clips, then select the matching fuse body and verify pin spacing, current/voltage rating, time-current curve, interrupt rating, startup compatibility, and holder thermal/contact limits | REWORK |
| J3 and battery conductors | Battery/BMS interface and protected bus | Connector and cable current, voltage class, contact heating, polarity, creepage/clearance, BMS 20 A minimum capability, battery-disconnect transients | Netlist identifies J3 as a generic two-pin Phoenix MKDS-1,5-2-5.08 horizontal terminal block, LCSC `C8439`, on `BAT_BUS_POS` and `PWR_NEG`. It is accepted only as the current prototype connector; final connector selection requires comparison against the intended battery/BMS connector practice, current/voltage rating, cable data, contact heating, and battery-disconnect transients | HOLD |
| L2 / power inductor | Main energy-storage element in LM51772 stage: 3.3 uH MMPC1206-3R3MT in the buck path | Saturation current at peak ripple plus load, RMS current, copper/core loss, DCR heating, insulation voltage, transient recovery, acoustic/thermal limits, ripple/current fit to selected switching frequency | Current netlist identifies `MMPC1206-3R3MT`, LCSC `C2638566`, 3.3 uH, 5.8 mOhm, 17 A listed current class, APV_APH1240 footprint. Effective inductance at bias, saturation margin, core loss, and loop fit remain open | HOLD |
| C17/C18/C21/C30-C35 | `V_out` output capacitor bank | Effective capacitance at bias, ripple RMS current, ESR/ESL, voltage rating, transient energy, startup/inrush, loop stability, discharge path | Capacitor data, VAL-008, compensation analysis | HOLD |
| U6 LM51772 | Four-switch converter controller | VIN/BIAS voltage class, pin absolute maximums, switching frequency, gate-drive source/sink current, current-sense common-mode range, UVLO thresholds, thermal conditions, configuration/readback | Current netlist identifies U6 as LM51772 with C22/C23=`100 nF` bootstrap capacitors, COMP network C24=`220 nF`, C25=`1.5 nF`, R31=`2.43 kOhm`, and CSA/CSB filters C20=`180 pF`, R26/R27=`10 Ohm`. These values require compensation, gate-timing, current-sense, thermal, and register-configuration verification | HOLD |
| R32/U6 ILIMCOMP_ISET | Hardware/configurable current-limit path | Initial low-current setting, approximately 12 A protection ceiling, tolerance, pin voltage range, startup default, readback/configuration order | The pins are separated and the external-ISET series pair is R68=`1.3 kOhm` plus R46=`82 kOhm`, for `83.3 kOhm` total. The current netlist ties U6 pins 17 (AGND), 28 (PGND), and 41 (GND) together on `PWR_NEG`, so the pair's `PWR_NEG` return is the common analog/power ground required by the datasheet. MODE pin 7 is separately pulled high through R32=`100 kOhm`. Complete the tolerance and current-sense offset calculation before acceptance | HOLD |
| R33/R29 and U6 configuration straps | Address, slope, mode, and controller configuration | Strap voltage windows, tolerance, reset state, I2C ownership, accidental mode selection | R33=`0 Ohm` on ADDR/SLOPE/CFG1 matches the datasheet R2D setting #0, and R29=`20.5 kOhm` on CFG2 matches the documented R2D setting #12 resistance. R30=`40.2 kOhm` on CDC still requires the full CDC configuration-table mapping and cable-drop/current-monitor intent; keep the strap block `HOLD` until that mapping and reset/I2C behavior are confirmed | HOLD |
| EN_UVLO divider and enable path | Automatic hardware startup with firmware supervision | Divider voltage rating, threshold tolerance, hysteresis, PV/battery transients, defined no-battery behavior, soft-start/inrush, MCU reset/unpowered behavior, firmware disable path, no USB-only switching | Current netlist uses R23=`75 kOhm` from `PV_IN_POS` to U6 EN/UVLO, with R61=`20 kOhm` plus R60=`1.5 kOhm` to `PWR_NEG`. Using the datasheet thresholds gives approximately `5.61 V` rising and `5.38 V` falling input thresholds. Automatic startup is accepted as the control strategy; startup/inrush, no-battery behavior, MCU supervision, and qualified PV operating-floor enforcement remain to be validated | HOLD |
| Gate resistors, bootstrap, and driver capacitors | Q5/Q6 and main bridge gate-drive support | Peak gate current, resistor pulse rating, bootstrap voltage, capacitor bias/temperature, dead-time, Miller turn-on, shoot-through margin | Current netlist now has R69/R70/R71/R72=`10 Ohm` series resistors: HO1->M1, LO1->M2, HO2->M4, and LO2->M3. Bootstrap capacitors C22/C23 are `100 nF / 100 V`. Structural gate isolation is present; timing, ringing, resistor pulse stress, bootstrap behavior, dead-time, and shoot-through margin still require scope/thermal validation | HOLD |
| PV/battery voltage dividers | ADC and protection telemetry | Maximum input voltage, resistor working voltage, divider dissipation, tolerance, RC settling, ADC clamp current, fault-open behavior | Schematic calculation and ADC timing | OPEN |
| Current-sense amplifier/input network | R16 Kelvin measurement and controller current sense | Common-mode range, differential range, filtering, input protection, polarity, offset versus 1 mOhm signal, open/short fault response | U6/U10 data, scope and calibration plan | OPEN |
| Temperature sensors and thermal paths | Power-stage and board thermal protection | Sensor placement, trip threshold, response time, MOSFET/inductor/shunt temperature rise, copper and heatsinking | Thermal calculation and measurement plan | OPEN |
| Control-power converter and USB service branch | Battery-powered normal control domain; isolated USB service | Battery input voltage range, startup current, reverse/backfeed blocking, USB-only inhibit, converter thermal load, MCU brownout behavior | Schematic/netlist and bench test | REWORK |

## Immediate Review Rules

- Do not lower a voltage rating solely because the nominal operating voltage is lower. Include cold-`Voc`, switching overshoot, ringing, surge clamp tolerance, and disconnected-load conditions.
- Do not accept a current rating solely from the DC datasheet headline. Check RMS current, peak current, pulse duration, thermal resistance, copper, connector contacts, and fuse clearing time.
- Treat Q5/Q6 as a switching and reverse-flow subsystem. Their body-diode orientation, gate default state, dead time, and disabled-state leakage must be reviewed together.
- Treat R16 as a four-terminal shunt according to the actual Rev 1 footprint mapping: pads 1/4 are the power element and pads 2/3 are Kelvin sense. Do not infer its path from direct net names without tracing Q5/Q6.
- Retain F2 as charger-output short-circuit protection unless a separate design decision changes its role. A main battery fuse, if required, is a distinct protection function.
- Any `REWORK` item blocks schematic freeze until the revised netlist and affected calculations are regenerated.

## First Acceptance Gate

Before editing component values or footprints, produce one completed review row each for Q5, Q6, R16, F2, L2, the output capacitor bank, and U6. Each row must include:

1. Actual part number and package/footprint.
2. Maximum steady-state and transient voltage.
3. Maximum RMS, average, and peak current.
4. Worst-case dissipation and junction/case/PCB temperature.
5. Required margin and the calculation or measurement supporting it.
6. Resulting schematic, footprint, layout, or firmware action.

Only after this gate should values or footprints be changed.

## First Slice Finding: Q5/Q6

The Rev 1 schematic/netlist defines both Q5 and Q6 as `XRS60N10F`, LCSC `C42457069`, using the `Package_DFN_QFN:PQFN-8-EP_6x5mm_P1.27mm_Generic` footprint. The current export now gives both devices the LCSC datasheet URL. The part identity is therefore documented at the schematic level, but the manufacturer part/package pin map still requires confirmation.

The current `Solar Project Rev1.kicad_pcb` contains no Q5, Q6, `XRS60N10F`, or `C42457069` entries. It contains M1-M4 `IPZ40N` power-MOSFET footprints instead. This is deferred because the PCB is a copied historical artifact and is not the current implementation target. It does not invalidate the schematic review, but it does mean that no component result is yet PCB-ready.

The updated netlist and schematic review confirm the intended switched path: `V_out -> R16 pads 3/4 -> Q5 drain -> SRC -> Q6 -> CHG_OUT_POS -> F2 -> BAT_BUS_POS`. The Q5/Q6 symbol pinout and this main power-flow path are accepted for schematic review. R16 is therefore a current-path shunt within the Q5/Q6 output-switch/reverse-flow subsystem, not a bypassed component.

### TI LM51772 quick-start sizing pass

The TI LM51772 Quickstart Tool V1.2 workbook was read from the project copy. Its cached example inputs are `VIN(min)=6 V`, `VIN(nom)=13 V`, `VIN(max)=48 V`, `VOUT=12.8 V`, `IOUT=12 A`, and `fSW=600 kHz`; these are not yet the locked Rev 1 operating limits and must not be treated as final requirements.

Using the workbook equations with the Rev 1 charge target adjusted to `VOUT=14.6 V`, the current `L2=1 uH` produces approximately `17.0 Apk-pk` inductor ripple at the provisional 48 V input case. A `3.3 uH` part reduces that to approximately `5.2 Apk-pk` and is therefore the current provisional L2 selection, subject to confirmation against the finalized PV minimum/maximum range, effective inductance at bias, saturation current, DCR loss, and loop compensation.

For the same provisional envelope, the workbook reports approximately `73 uF` minimum output capacitance and `28 uF` minimum input capacitance at the original 12.8 V example. Recalculating the dominant 14.6 V cases gives approximately `81 uF` minimum output capacitance in boost operation and `28 uF` minimum input capacitance at the high-input buck case. The current netlist output bank is approximately `664.4 uF` nominal: C18=`560 uF`, C3=`100 uF`, C34/C35=`2.2 uF` each, and C33=`100 nF`. It remains on `HOLD`: it must not be removed or reduced solely from the small-signal minimum-capacitance result because startup inrush, effective capacitance, ESR, ripple-current rating, transient response, and compensation still require closure. The netlist documents C3 at 25 V and C34/C35 at 50 V; C18 voltage, ripple-current, and ESR data remain missing. The existing input-side bank should be checked against an effective-capacitance target of at least `100 uF` selected capacitance.

Disposition from this pass: `L2=1 uH` is `REWORK`; `L2=3.3 uH` is a provisional `HOLD` pending the finalized PV envelope; input capacitance is `HOLD` pending DC-bias and ripple-current data; output capacitance is `HOLD` pending transient and stability analysis. No capacitor bank was deleted from the schematic in this pass.

### Rebaselined workbook run: 20 V qualified input floor

A separate workbook copy, `hardware/LM51772_Buck-Boost_Quickstart_Tool_Rev1_20V_14V6_12A.xlsm`, was created with `VIN(min)=20 V`, `VIN(nom)=20 V`, provisional `VIN(max)=48 V`, `VOUT=14.6 V`, `IOUT(max)=12 A`, `fSW=600 kHz`, and the MMPC1206-3R3MT values `LF=3.3 uH`, `LF,eff=2.64 uH`, and `DCR=5.8 mOhm`. The original TI workbook was not modified. The approximately `6 V` hardware UVLO is a protection boundary and is not entered as the sizing-model minimum input voltage.

The recalculated workbook reports approximately `3.47 uF` minimum output capacitance, `27.78 uF` minimum input capacitance, `59.9 mOhm` maximum permitted output-capacitor ESR, and `8.54 mOhm` maximum permitted input-capacitor ESR. With the selected 100 uF input and output values, it reports approximately `8.56 A rms` input-capacitor current, `1.86 A rms` output-capacitor current, `80.5 mVpk-pk` input ripple, and `35.0 mVpk-pk` output ripple. The 20 V and 48 V endpoint ripple and peak-current results remain approximately `2.50 Apk-pk / 13.25 A peak` and `6.45 Apk-pk / 15.23 A peak`, respectively.

Disposition from this run: the 20 V qualified floor makes the MMPC1206-3R3MT materially more suitable than the prior 6 V sizing case, and the current 100 uF nominal capacitor targets pass the simplified ripple/minimum-capacitance calculation. These are not final acceptance results until Excel recalculation, capacitor DC-bias/ripple data, startup/inrush, transient response, and loop stability are verified.

### Input capacitor bank prototype-use decision

The saved Rev 1 netlist places C19, C21, C30, and C31 as four parallel 47 uF, 100 V aluminum-polymer capacitors on `PV_IN_POS` to `PWR_NEG`, with C7, C8, and C28 providing local ceramic bypass. Using the datasheet values of 28 mOhm ESR and 2.6 A RMS ripple current for each bulk capacitor gives approximately 7.0 mOhm combined bulk ESR and 10.4 A RMS combined ripple-current rating, versus workbook limits of approximately 8.54 mOhm and 8.56 A RMS. This leaves approximately 21.5% ripple-current margin. The ESR calculation is acceptable only if 28 mOhm is the guaranteed maximum at the applicable temperature, frequency, and bias conditions. Otherwise the bank remains `HOLD`; prototype use is not a substitute for final qualification. Final release also requires measured input ripple, capacitor temperature, startup behavior, and high-frequency impedance confirmation.

### LM51772 configuration-pin finding

The prior shared MODE/ISET net has been corrected in the saved netlist. MODE pin 7 is now on its own net and pulled to `U6_VCC2` through R32=`100 kOhm`. ILIMCOMP/ISET pin 16 is also separate, with R68=`1.3 kOhm` in series with R46=`82 kOhm` to the common `PWR_NEG` ground. The datasheet recommends tying AGND, PGND, and GND together; the current netlist implements that common tie on `PWR_NEG`, so the ISET return is acceptable at the net level. The `83.3 kOhm` total targets approximately `12.0 A` nominal with a 1 mOhm shunt. Using the datasheet bounds VSET=`0.95-1.05 V`, gm(ISET)=`0.9-1.1 mS`, resistor tolerances, shunt tolerance, and sense offset, the guaranteed range remains wider than the nominal target. The current-limit implementation remains `HOLD`, and the layout must confirm a quiet analog-ground return.

### LM51772 controller closure status

The saved netlist is structurally complete enough for schematic review: U6 VIN/BIAS, VCC2, common AGND/PGND/GND, matched external FB and VIN-FB dividers, EN/UVLO divider, MODE/configuration straps, external-ISET resistor pair, bootstrap capacitors, COMP network, and CSA/CSB filters are all present and connected. U6 remains `HOLD`, not `PASS`, because the remaining evidence is behavioral: configuration-table confirmation for R29/R30/R33, compensation stability over the 20-48 V envelope, gate timing/dead-time and bootstrap behavior, current-sense polarity/filter response, current-limit calibration, thermal rise, and startup/no-battery testing.

### Gate-network finding

The shared Q5/Q6 gate network is:

- R2: `100 kOhm` from `SRC` to the shared gate, providing the default-off pull-up when the external gate driver is inactive.
- R65: `10 Ohm` between U14 `GATE` and the shared Q5/Q6 gate, limiting gate-drive edge current.
- D5: `1SMA4742A`, nominal 12 V zener across the source/gate network for VGS clamp protection.
- U14: the active gate-drive owner for Q5/Q6; U6 does not directly drive this output/reverse-flow pair.

Schematic-level result: default-off gate bias is provisionally `PASS`. The updated netlist identifies Q5/Q6 as `BSC040N10NS5` and its symbol pin mapping as source pins 1-3, gate pin 4, and drain pin 5. The Q5/Q6 body diodes are accepted as drain-to-source oriented in the source-coupled back-to-back arrangement, so the two body diodes oppose one another and block reverse flow in either direction when the gates are OFF. The official LM74502H datasheet states that U14 does **not** provide reverse-current blocking; it provides reverse-polarity/load-disconnect control and over/undervoltage shutdown. Therefore reverse-current safety while Q5/Q6 are ON must come from the charger power-stage control and an explicit U14 disable decision, not from U14 alone. The following remain open before the Q5/Q6 row can pass: exact BSC040N10NS5 package/pad confirmation, U14 power-loss/default behavior, VGS clamp current and pulse energy, U14/Q5/Q6 timing during converter startup and shutdown, thermal loss, switching overshoot, and the complete ON-state reverse-current control strategy.

### R16 finding

R16 is a `0.001 ohm`, 5 W, four-terminal Kelvin shunt. The accepted current path uses pads 1/4 as the power element from `V_out` toward Q5; pads 2/3 are the Kelvin `V_out_sense` pair. R63/R64 connect the U6 ISNSN/ISNSP inputs into the sense network. Nominal shunt dissipation is approximately 0.10 W at 10 A and 0.144 W at 12 A, so the nominal Rev 1 current envelope is comfortably below the stated 5 W rating. This remains `HOLD` until sense polarity, copper current sharing, pulse energy, and current-limit calibration are verified.

Required schematic-first action before Q5/Q6 can become `PASS`:

1. Confirm the BSC040N10NS5 manufacturer package/pad drawing against the selected Q5/Q6 footprint.
2. Verify Q5/Q6 drain/source/gate/thermal-pad intent and body-diode orientation against the manufacturer drawing.
3. Complete the voltage/current/thermal/reverse-flow calculation at the schematic level.
4. Verify U14 gate-charge capability, VGS clamp pulse energy, startup/shutdown timing, and switching overshoot.
5. Defer PCB reference reconciliation, routing, and DRC until the schematic and replacement footprint are accepted.
