# Rev2 CSA/CSB investigation

Tracker: [#19](https://github.com/Phil-CIA/Solar-Project-200w/issues/19). The original 2026-11-09 investigation below describes the copied baseline at `488441a`. The 2026-11-16 review below supersedes its statements about the current Rev2 source. No circuit values or PCB geometry were changed by this review.

## 2026-11-16 component and layout review

**Decision: retain R26/R27 = 10 Ohm and C20 = 180 pF as the initial prototype values, and leave C61 unpopulated.** The 10 Ohm resistors match TI's general recommendation; 180 pF is a reasonable starting point, not yet a calculated or validated final value. Do not populate C61 or change C20 merely to suppress a trip. A better layout is expected to reduce pickup, but Rev1's actual trip mechanism and Rev2 performance remain unmeasured.

| Item | Review |
|---|---|
| R26, R27 | 10 Ohm each, 0805, schematic LCSC `C17414`; appropriate TI-recommended starting resistance. Confirm actual ordering metadata and ratings before release: the schematic's datasheet URL ends in `C17415`, whereas its LCSC field says `C17414`. |
| C20 | 180 pF, 0805, schematic LCSC `C1794`. Use a stable C0G/NP0 capacitor with suitable tolerance and voltage rating; the Rev1 BOM lists Yageo `CC0805JRNPO9BN181` / `C113826` as a replacement for discontinued `C1794`. Confirm the actual Rev2 part and availability before release; the schematic metadata has not been updated by this review. |
| C61 | Parallel footprint on the IC-side differential nets, schematic value `DNP` and KiCad DNP attribute set. Leave unpopulated unless a measured and recalculated filter change is needed; choose an actual part only for an intentional experiment. |
| R22/R75 | Two 5 mOhm shunts in parallel give 2.5 mOhm nominal. The 45/50/55 mV positive peak thresholds imply approximately 17.8/20.0/22.2 A including 1% shunt resistance tolerance, before slope compensation, parasitics and other effects. This is inductor **peak**, not output average, current. Validate operating and fault margins independently. |

### Filter calculation and limits

TI's [LM51772-Q1 datasheet](lm51772-datasheet.pdf), section 7.3.10, recommends 10 Ohm for each series resistor and specifies `C_DIFF = t_on,min / (2 * pi * (R_DIFF1 + R_DIFF2) * 10)`, using the minimum on-time in buck or boost operation at the selected operating point. For 10 + 10 Ohm and 180 pF, the ideal differential time constant is 3.6 ns and the corner is approximately 44.2 MHz. Rearranging TI's equation, 180 pF corresponds to a minimum operating on-time of approximately 226 ns. This is **not** a measurement of the actual minimum pulse. Determine that pulse over the intended VIN, VOUT, frequency and operating modes before calling the capacitance compliant; account for component tolerances and parasitics. Do not confuse the RC time constant with TI's stated filter criterion or substitute the device's minimum controllable on-time for the actual operating-point pulse.

The saved Rev2 PCB at `11395a9` contains the two shunt footprints and the placed filter components, and the shunt-side sense traces are on In2.Cu. This is a substantial change from the historical layout below and is consistent with the user's effort to follow the TI EVM layout. It is not yet a completed or verified copy: the saved PCB has **no copper tracks or zones on the IC-side CSA/CSB nets** between R26/R27, C20/C61 and U6. KiCad 10.0.6 DRC reports those pads unconnected, including U6 pins 22/21. The shunt-side CSB trace also has a dangling end and a reported missing connection to NT1. Complete and inspect the copper, Kelvin pickup, net ties, and any ground reference before claiming the filter or protection is functional. The same DRC run reports 222 violations and 382 unconnected items overall; it does not establish a fabrication-ready board.

### Remaining acceptance work

- Calculate the minimum operating pulse and worst-case inductor peaks for the intended envelope, including inductance under bias and slope compensation; check the resulting current limit against component and fault ratings.
- Complete the paired Kelvin routing and short IC-side filter connections, then review the actual copper and DRC. Confirm C61 is DNP in the assembly output.
- With suitable differential probing and/or an independent inductor-current measurement, compare the shunt and U6 sense signals; do not attach an earth-grounded scope clip to switching-referenced nodes.
- Verify startup, regulation and peak-current protection across the intended input/load range, recording VIN, VOUT, current, STATUS_BYTE and temperature. A CSA/CSB short or attenuating resistor is diagnostic only and is not an accepted production solution.

## Historical investigation: copied baseline at `488441a`

### Confirmed source findings

The PCB source contains these connections:

| Connection | Baseline |
|---|---|
| R22 power pads | Pad 4 = SW_1; pad 1 = inductor L2 pad 1 net |
| R22 Kelvin pads | Pad 3 = `_CSA_2`; pad 2 = `_CSB_2` |
| CSA input | R22 pad 3 through R26 (10 Ohm) to U6 pin 22 |
| CSB input | R22 pad 2 through R27 (10 Ohm) to U6 pin 21 |
| Differential capacitor | C20 = 180 pF between the U6-side CSA/CSB nets |
| Copied shunt value | R22 = 0.001 Ohm; four-terminal Kelvin footprint |

This agrees with the recorded polarity trace; no simple CSA/CSB swap is established. The PCB's L2 value field is only `L`; the component review identifies a provisional 3.3 uH part. Confirm actual inductance and operating-point assumptions before calculating ripple/current limits.

#### Routing

Segment lengths calculated directly from saved PCB coordinates, excluding vertical via length and pad-internal paths:

| Net | Summed copper segment length | Layers |
|---|---:|---|
| `_CSA_2`, shunt to R26 | 40.553 mm | 3.454 mm F.Cu, 37.099 mm In1.Cu |
| `_CSB_2`, shunt to R27 | 36.859 mm | 2.654 mm F.Cu, 34.205 mm B.Cu |
| U6-side CSA net | 9.120 mm | F.Cu |
| U6-side CSB net | 12.551 mm | F.Cu |

Each shunt-to-resistor net has two through vias. The two signals follow independent inner/bottom-layer routes rather than one closely coupled same-layer pair. R26 and R27 are spread apart at approximately (165.72, 95.7071) and (170.67, 102.315) mm; C20 is at (165.74, 99.0114) mm. U6 pins 22/21 are at (161.135, 99.73)/(161.135, 100.23) mm.

This is a concrete layout weakness relative to TI's differential-pair guidance. It does not prove induced noise caused the observed current-limit event. Segment totals also include branches; they are not flight-time or impedance measurements.

### Datasheet implications

Source: `docs/lm51772-datasheet.txt`, sections 7.3.10, 7.3.11 and layout guidance around lines 2447-2497 and 6105-6110.

- TI recommends 10 Ohm series resistors for most applications, with the differential capacitor calculated from the selected operating point and minimum buck/boost on-time.
- The copied 10 Ohm + 10 Ohm / 180 pF filter has a 3.6 ns differential RC time constant and approximately 44.2 MHz RC corner under the simple ideal-source model. This alone neither proves the filter is wrong nor selects its replacement; account for operating frequency, duty range, parasitics and the datasheet equation.
- The stated nominal steady-state peak threshold is 50 mV across R22. A 1 mOhm shunt therefore corresponds to nominal 50 A peak inductor current; 10 mOhm corresponds to 5 A. Neither is automatically the right design value.
- The roughly 12 A output protection target is not identical to inductor peak current, especially in boost operation. Size R22 using the input/output envelope, ripple, efficiency, tolerances, transient needs and inductor/MOSFET limits. Define average output-current limiting separately.
- TI calls out shunt parasitic inductance and suggests parallel sense elements where appropriate for high currents. A different shunt arrangement is an option to evaluate, not an already selected fix.

### What the bench evidence does and does not establish

Normal sensing produced an IOUT-dominant stall; shorting or attenuating the signal permitted regulation. Twisted Kelvin wires and added small capacitance did not remove the stall; very heavy capacitance caused noisy operation. These observations keep both actual excessive current and transient/common-mode pickup on the investigation list.

Do not install a permanent CSA/CSB short or attenuating resistor as a fix. Do not select 470 nF or another large capacitor simply because it suppresses a trip. The exact current-sense arrangement during the successful loaded run was not recorded, and the latest ISET bench wiring is not a validated current-limit configuration.

### Original next design packet

1. Establish the intended normal converter input range, output/charge range, switching frequency, inductance under bias and average/peak limits. Separate the reported 8-30 V bench range from the intended PV/charging operating envelope.
2. Calculate nominal and worst-case inductor peaks in buck, boost and transition operation. Select R22 value/package/tolerance and dissipation with sufficient normal-operation margin and an effective protective limit.
3. Use a suitable differential measurement at U6 CSA/CSB, and an independent current measurement where available, to distinguish a real current peak from a transient. Establish exact installed sense/filter/ISET parts and test conditions first. This document does not authorize or prescribe a powered test; do not attach an earth-grounded scope clip to switching-referenced sense nodes.
4. Rework Rev2 placement so the filter is compact at U6 pins 21/22. Route both Kelvin signals together with matched transitions and an intentional adjacent reference plane, away from switch/gate routes; do not treat these switching-common-mode signals as ground-referenced ADC inputs.
5. Select C20 using the datasheet's operating-point equation and measured problem. Check that the selected filter preserves current-limit timing; do not trade protection away for regulation.
6. Validate protection and stable operation across the chosen envelope before fabrication release. If measurements are unavailable, record the proposed layout correction as unverified, not a proven root-cause fix.

### Original tool limitation

The available KiCad 9 Python loader did not load this newer PCB source. Findings above came from read-only parsing of its saved S-expression pads/nets/segments, not a PCB save or an ERC/DRC run. No downgrade or source rewrite was attempted.
