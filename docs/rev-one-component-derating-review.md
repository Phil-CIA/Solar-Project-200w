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
| D2 and C52 | PV/output transient suppression and protected-node capacitance | Standoff voltage, clamp voltage at fault current, pulse energy, capacitor voltage rating, leakage, placement, and C52 margin | Datasheets, transient worksheet, layout review | OPEN |
| C1-C6 and input ceramic bank | PV-side switching-loop decoupling | DC bias derating, effective capacitance, ripple RMS current, voltage transients, package temperature, loop placement | Capacitor DC-bias data and layout | OPEN |
| Q5/Q6 | Accepted schematic topology: `V_out -> R16 -> Q5 -> SRC -> Q6 -> CHG_OUT_POS -> F2 -> BAT_BUS_POS`; output switching and reverse-flow MOSFET stage | VDS in both switching and disabled states, VGS limits, body-diode reverse current, continuous and pulsed current, RDS(on) at temperature, SOA, gate-drive timing, reverse-flow blocking state; rebaseline under the 12 A peak / 125 W design envelope | Netlist identifies `Q5` and `Q6` as `XRS60N10F`, LCSC `C42457069`, 100 V / 60 A class, PQFN-8 6x5 mm. This is a legacy oversized selection relative to the current 12 A peak requirement and must be re-justified or reselected under the lower converter envelope | HOLD |
| R16 | 1 mOhm output-current shunt; high-current pads 1/4, Kelvin pads 2/3 per actual footprint mapping | Continuous and transient I2R loss, pulse energy, 4-terminal Kelvin routing, sense polarity, copper current sharing, temperature coefficient, 5 W rating at actual thermal conditions | Current netlist, part data, PCB copper review, current-limit calculation | HOLD |
| F2 | Charger-output branch short-circuit protection: `CHG_OUT_POS -> F2 -> BAT_BUS_POS` | Fuse DC voltage rating (14.6V charging max), interrupt rating, continuous 10A / transient 12A ceiling, startup/inrush behavior (soft-start or hard cold-start?), clearing time for charger-side short, holder contact rating, thermal rise, physical placement near CHG_OUT_POS | Littelfuse 100 series 5x25mm, 10A time-delay, 63V minimum per schematic; actual part number and I-t curve required; startup inrush profile and fuse-holder thermal limit review; fault-current clearing time and holder contact 12A cycling test | HOLD |
| J3 and battery conductors | Battery/BMS interface and protected bus | Connector and cable current, voltage class, contact heating, polarity, creepage/clearance, BMS 20 A minimum capability, battery-disconnect transients | Connector/cable/BMS data | OPEN |
| L2 / power inductor | Main energy-storage element in LM51772 stage: 1uH HC3-1R0-R in the buck path | Saturation current at peak ripple plus load, RMS current, copper/core loss, DCR heating (about 0.42 mΩ), insulation voltage, transient recovery, acoustic/thermal limits, ripple/current fit to selected switching frequency | Live schematic L2 is 1uH `HC3-1R0-R`; existing notes cite ~78 A Irms / Isat and 0.42 mΩ DCR from vendor data, so current class is not the blocker; remaining work is value/ripple and control-loop fit | HOLD |
| C17/C18/C21/C30-C35 | `V_out` output capacitor bank | Effective capacitance at bias, ripple RMS current, ESR/ESL, voltage rating, transient energy, startup/inrush, loop stability, discharge path | Capacitor data, VAL-008, compensation analysis | HOLD |
| U6 LM51772 | Four-switch converter controller | VIN/BIAS voltage class, pin absolute maximums, switching frequency, gate-drive source/sink current, current-sense common-mode range, UVLO thresholds, thermal conditions, configuration/readback | LM51772 datasheet, schematic, firmware register map | OPEN |
| R32/U6 ILIMCOMP_ISET | Hardware/configurable current-limit path | Initial low-current setting, approximately 12 A protection ceiling, tolerance, pin voltage range, startup default, readback/configuration order | Datasheet, tolerance calculation, firmware test | OPEN |
| R33/R29 and U6 configuration straps | Address, slope, mode, and controller configuration | Strap voltage windows, tolerance, reset state, I2C ownership, accidental mode selection | LM51772 datasheet and netlist | OPEN |
| EN_UVLO divider and enable path | Independent hardware UVLO plus normal startup authorization | Divider voltage rating, threshold tolerance, hysteresis, PV/battery transients, default-off state, MCU reset/unpowered behavior, no USB-only switching | Schematic, scope captures, VAL-007 | REWORK |
| Gate resistors, bootstrap, and driver capacitors | Q5/Q6 and main bridge gate-drive support | Peak gate current, resistor pulse rating, bootstrap voltage, capacitor bias/temperature, dead-time, Miller turn-on, shoot-through margin | Controller timing, gate captures, datasheets | OPEN |
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

For the same provisional envelope, the workbook reports approximately `73 uF` minimum output capacitance and `28 uF` minimum input capacitance at the original 12.8 V example. Recalculating the dominant 14.6 V cases gives approximately `81 uF` minimum output capacitance in boost operation and `28 uF` minimum input capacitance at the high-input buck case. The existing output bank is approximately `1.3 mF` nominal and remains on `HOLD`: it must not be removed solely from the small-signal minimum-capacitance result because startup inrush, effective capacitance, ESR, ripple-current rating, transient response, and compensation still require closure. The existing input-side bank should be checked against an effective-capacitance target of at least `100 uF` selected capacitance, rather than relying on the present approximately `54 uF` nominal combination.

Disposition from this pass: `L2=1 uH` is `REWORK`; `L2=3.3 uH` is a provisional `HOLD` pending the finalized PV envelope; input capacitance is `HOLD` pending DC-bias and ripple-current data; output capacitance is `HOLD` pending transient and stability analysis. No capacitor bank was deleted from the schematic in this pass.

### Rebaselined workbook run: 20 V qualified input floor

A separate workbook copy, `hardware/LM51772_Buck-Boost_Quickstart_Tool_Rev1_20V_14V6_12A.xlsm`, was created with `VIN(min)=20 V`, `VIN(nom)=20 V`, provisional `VIN(max)=48 V`, `VOUT=14.6 V`, `IOUT(max)=12 A`, `fSW=600 kHz`, and the MMPC1206-3R3MT values `LF=3.3 uH`, `LF,eff=2.64 uH`, and `DCR=5.8 mOhm`. The original TI workbook was not modified. The approximately `6 V` hardware UVLO is a protection boundary and is not entered as the sizing-model minimum input voltage.

The recalculated workbook reports approximately `3.47 uF` minimum output capacitance, `27.78 uF` minimum input capacitance, `59.9 mOhm` maximum permitted output-capacitor ESR, and `8.54 mOhm` maximum permitted input-capacitor ESR. With the selected 100 uF input and output values, it reports approximately `8.56 A rms` input-capacitor current, `1.86 A rms` output-capacitor current, `80.5 mVpk-pk` input ripple, and `35.0 mVpk-pk` output ripple. The 20 V and 48 V endpoint ripple and peak-current results remain approximately `2.50 Apk-pk / 13.25 A peak` and `6.45 Apk-pk / 15.23 A peak`, respectively.

Disposition from this run: the 20 V qualified floor makes the MMPC1206-3R3MT materially more suitable than the prior 6 V sizing case, and the current 100 uF nominal capacitor targets pass the simplified ripple/minimum-capacitance calculation. These are not final acceptance results until Excel recalculation, capacitor DC-bias/ripple data, startup/inrush, transient response, and loop stability are verified.

### Gate-network finding

The shared Q5/Q6 gate network is:

- R2: `100 kOhm` from `SRC` to the shared gate, providing the default-off pull-up when the external gate driver is inactive.
- R65: `10 Ohm` between U14 `GATE` and the shared Q5/Q6 gate, limiting gate-drive edge current.
- D5: `1SMA4742A`, nominal 12 V zener across the source/gate network for VGS clamp protection.
- U14: the active gate-drive owner for Q5/Q6; U6 does not directly drive this output/reverse-flow pair.

Schematic-level result: default-off gate bias is provisionally `PASS`. The Q5/Q6 body diodes are accepted as drain-to-source oriented in the source-coupled back-to-back arrangement, so the two body diodes oppose one another and block reverse flow in either direction when the gates are OFF. The official LM74502H datasheet states that U14 does **not** provide reverse-current blocking; it provides reverse-polarity/load-disconnect control and over/undervoltage shutdown. Therefore reverse-current safety while Q5/Q6 are ON must come from the charger power-stage control and an explicit U14 disable decision, not from U14 alone. The following remain open before the Q5/Q6 row can pass: U14 power-loss/default behavior, VGS clamp current and pulse energy, U14/Q5/Q6 timing during converter startup and shutdown, and the complete ON-state reverse-current control strategy.

### R16 finding

R16 is a `0.001 ohm`, 5 W, four-terminal Kelvin shunt. The accepted current path uses pads 1/4 as the power element from `V_out` toward Q5; pads 2/3 are the Kelvin `V_out_sense` pair. R63/R64 connect the U6 ISNSN/ISNSP inputs into the sense network. Nominal shunt dissipation is approximately 0.10 W at 10 A and 0.144 W at 12 A, so the nominal Rev 1 current envelope is comfortably below the stated 5 W rating. This remains `HOLD` until sense polarity, copper current sharing, pulse energy, and current-limit calibration are verified.

Required schematic-first action before Q5/Q6 can become `PASS`:

1. Confirm the exact manufacturer part number behind LCSC `C42457069` and obtain its datasheet and package pin map.
2. Confirm the custom `1My_Basic components:IPZ40N` symbol pin mapping against that part and the selected Q5/Q6 footprint pad mapping.
3. Verify Q5/Q6 drain/source/gate/thermal-pad intent and body-diode orientation in the schematic.
4. Complete the voltage/current/thermal/reverse-flow calculation at the schematic level.
5. Defer PCB reference reconciliation, footprint confirmation, routing, and DRC until the schematic is accepted.
