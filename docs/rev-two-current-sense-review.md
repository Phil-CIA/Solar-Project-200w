# Rev2 CSA/CSB investigation

Date: 2026-11-09. Tracker: [#19](https://github.com/Phil-CIA/Solar-Project-200w/issues/19). Read-only electrical investigation of the Rev2 copied baseline at `488441a`; no circuit values, placement or routing changed.

## Current schematic status (schematic-only)

The investigation below remains a historical description of the copied PCB baseline. A later schematic-only packet now captures three INA241A3 current channels: output on PA0 across R16/R80 (0.005 Ohm each), PV on PA1 across R76/R77 (0.010 Ohm each) between `PV_FUSED_POS` and `PV_Q4_SOURCE` after F1 and before Q4, and battery on PC4 across R78/R79 (0.001 Ohm each) between `BAT_PACK_POS` and `BAT_BUS_POS`. U10/U11/U12 are entered as INA241A3IDDFR with TSOT-23-8 footprints. These are schematic values and connections, not a released BOM or qualified measurement design.

The CSA/CSB schematic retains R26/R27 at 10 Ohm and C20 at 180 pF, with DNP C61 provided across the sense nets. R22/R75 are each shown as 0.005 Ohm; the peak-current threshold and filter timing still require calculation and review. R81 pin 1 is on `CHG_OUT_POS`, preserving the existing output-voltage divider input. After the user's graphical cleanup, the PV source-sense connection, U10 REF2, and J3 pin 1 were restored to their intended nets; the latest netlist verifies the current-sensing connections. The PCB remains unchanged, so none of these shunts, sensor routes, or filter provisions is implemented or validated on copper. KiCad 10.0.6 ERC reports 0 errors and 53 warnings. PCB DRC reports two inherited courtyard overlaps (C3/C18 and R72/C18) and 412 inherited unconnected items; schematic parity reports 59 issues, up from 36 before the schematic-only changes. The warnings and parity findings remain open.

## Confirmed source findings

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

### Routing

Segment lengths calculated directly from saved PCB coordinates, excluding vertical via length and pad-internal paths:

| Net | Summed copper segment length | Layers |
|---|---:|---|
| `_CSA_2`, shunt to R26 | 40.553 mm | 3.454 mm F.Cu, 37.099 mm In1.Cu |
| `_CSB_2`, shunt to R27 | 36.859 mm | 2.654 mm F.Cu, 34.205 mm B.Cu |
| U6-side CSA net | 9.120 mm | F.Cu |
| U6-side CSB net | 12.551 mm | F.Cu |

Each shunt-to-resistor net has two through vias. The two signals follow independent inner/bottom-layer routes rather than one closely coupled same-layer pair. R26 and R27 are spread apart at approximately (165.72, 95.7071) and (170.67, 102.315) mm; C20 is at (165.74, 99.0114) mm. U6 pins 22/21 are at (161.135, 99.73)/(161.135, 100.23) mm.

This is a concrete layout weakness relative to TI's differential-pair guidance. It does not prove induced noise caused the observed current-limit event. Segment totals also include branches; they are not flight-time or impedance measurements.

## Datasheet implications

Source: `docs/lm51772-datasheet.txt`, sections 7.3.10, 7.3.11 and layout guidance around lines 2447-2497 and 6105-6110.

- TI recommends 10 Ohm series resistors for most applications, with the differential capacitor calculated from the selected operating point and minimum buck/boost on-time.
- The copied 10 Ohm + 10 Ohm / 180 pF filter has a 3.6 ns differential RC time constant and approximately 44.2 MHz RC corner under the simple ideal-source model. This alone neither proves the filter is wrong nor selects its replacement; account for operating frequency, duty range, parasitics and the datasheet equation.
- The stated nominal steady-state peak threshold is 50 mV across R22. A 1 mOhm shunt therefore corresponds to nominal 50 A peak inductor current; 10 mOhm corresponds to 5 A. Neither is automatically the right design value.
- The roughly 12 A output protection target is not identical to inductor peak current, especially in boost operation. Size R22 using the input/output envelope, ripple, efficiency, tolerances, transient needs and inductor/MOSFET limits. Define average output-current limiting separately.
- TI calls out shunt parasitic inductance and suggests parallel sense elements where appropriate for high currents. A different shunt arrangement is an option to evaluate, not an already selected fix.

## What the bench evidence does and does not establish

Normal sensing produced an IOUT-dominant stall; shorting or attenuating the signal permitted regulation. Twisted Kelvin wires and added small capacitance did not remove the stall; very heavy capacitance caused noisy operation. These observations keep both actual excessive current and transient/common-mode pickup on the investigation list.

Do not install a permanent CSA/CSB short or attenuating resistor as a fix. Do not select 470 nF or another large capacitor simply because it suppresses a trip. The exact current-sense arrangement during the successful loaded run was not recorded, and the latest ISET bench wiring is not a validated current-limit configuration.

## Next design packet

1. Establish the intended normal converter input range, output/charge range, switching frequency, inductance under bias and average/peak limits. Separate the reported 8-30 V bench range from the intended PV/charging operating envelope.
2. Calculate nominal and worst-case inductor peaks in buck, boost and transition operation. Select R22 value/package/tolerance and dissipation with sufficient normal-operation margin and an effective protective limit.
3. Use a suitable differential measurement at U6 CSA/CSB, and an independent current measurement where available, to distinguish a real current peak from a transient. Establish exact installed sense/filter/ISET parts and test conditions first. This document does not authorize or prescribe a powered test; do not attach an earth-grounded scope clip to switching-referenced sense nodes.
4. Rework Rev2 placement so the filter is compact at U6 pins 21/22. Route both Kelvin signals together with matched transitions and an intentional adjacent reference plane, away from switch/gate routes; do not treat these switching-common-mode signals as ground-referenced ADC inputs.
5. Select C20 using the datasheet's operating-point equation and measured problem. Check that the selected filter preserves current-limit timing; do not trade protection away for regulation.
6. Validate protection and stable operation across the chosen envelope before fabrication release. If measurements are unavailable, record the proposed layout correction as unverified, not a proven root-cause fix.

## Tool limitation

The available KiCad 9 Python loader did not load this newer PCB source. Findings above came from read-only parsing of its saved S-expression pads/nets/segments, not a PCB save or an ERC/DRC run. No downgrade or source rewrite was attempted.
