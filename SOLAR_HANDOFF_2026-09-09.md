# Solar Project - Rev 1 Architecture Handoff (2026-09-09)

## Stop State

No hardware work was performed in this design session. Preserve the Rev 0 bench stop state from `SOLAR_HANDOFF_2026-09-08.md`: keep PV off and disconnected, do not raise source current, do not alter hardware, and never write LM51772 `CLEAR_FAULTS` at `0x03`.

The next session is a Rev 1 design-analysis session, not a continuation of high-energy Rev 0 testing unless that scope is explicitly chosen and the September 8 controls are reread first.

## Rev 1 Direction

Rev 1 is a targeted reconfiguration and correction of Rev 0, not a clean-sheet charger redesign (DEC-024).

Retain, subject to value, rating, layout, and netlist verification:

- One LM51772 and the existing four-switch conversion topology.
- Main power-stage partition, magnetic concept, and gate-drive architecture.
- LM74502H plus back-to-back N-MOS PV input-protection concept.
- PV voltage/current, battery voltage/current, and temperature sensing functions.
- R16 output-current-shunt function and the converter-output-to-battery-bus concept.
- STM32 charger supervision, safety policy, and telemetry ownership.

A replacement converter topology or multiple charger stages is outside the accepted Rev 1 scope unless later evidence forces a new decision.

## Accepted Electrical Envelope

DEC-023 establishes:

- 125 W continuous charger output at the defined thermal design point.
- 150 W maximum PV operating power.
- Existing 2x100 W series array remains compatible if the input hardware tolerates its full cold-`Voc` and `Isc` envelope while control clips operating power.
- 8 A to 10 A nominal battery charge-current target.
- Approximately 12 A hardware charge-current protection ceiling, pending tolerance and transient analysis.
- At least 20 A BMS charge-current capability.
- Output capacitance must be recalculated; do not automatically retain the Rev 0 approximately 1.1 mF to 1.2 mF bank.

## Startup Decisions

The ten startup steps are allocated to these decisions:

1. DEC-025: connect/protect the battery and power the normal control domain from the protected battery bus.
2. DEC-026: validate battery conditions and hold U6 positively inhibited with a hardware-safe default-off MCU path.
3. DEC-027: qualify PV open-circuit voltage relative to battery voltage and begin at a verified low current.
4. DEC-028: establish PV input-voltage regulation above hardware UVLO and ramp power within explicit limits.
5. DEC-029: enable MPPT only after stable conversion, then back off, stop, and retry deliberately under weak-source conditions.

The detailed ten-step table, Rev 0 comparison, and verification intent are in `docs/rev-one-delta-plan-2026-08-04.md`.

## Corrected Rev 0 Understanding

The authoritative artifact remains:

`hardware/kicad/solar-project/Solar Project Rev0 - Manufactured First Build.net`

Confirmed output-path facts:

- The converter capacitor/output node is `V_out`.
- `V_out` reaches `BAT_BUS_POS` through the 1 mOhm R16 current shunt.
- A connected battery can therefore establish the output bus and precharge the output capacitor bank.
- Both F2 pins are on `BAT_BUS_POS` in the manufactured netlist, so F2 is electrically bypassed and does not implement the documented series fuse boundary.
- U6 pin 18 senses `BAT_BUS_POS` as its output rail.

Confirmed startup-control facts:

- Rev 0 U6 starts autonomously when analog `EN_UVLO` crosses its threshold.
- The STM32 did not provide effective positive inhibit for the first switching test and does not control individual switching pulses.
- `SS_ATRK` did not visibly ramp during the failed startup bursts.
- The average-current-limit behavior was not demonstrated before startup.
- The approximately 6 V, 200 mA bench test showed collapse at the enable transition; it did not prove that a real PV array lacks the total capacitor-charging energy.

## Mandatory Rev 1 Hardware Deltas

1. Correct F2 into a real series battery fuse path.
2. Verify `V_out -> R16 -> protected BAT_BUS_POS` continuity and current direction.
3. Measure battery-connection capacitor inrush; add precharge or inrush limiting only if stress analysis requires it.
4. Make protected battery power the normal runtime control source.
5. Keep USB service power isolated so USB alone cannot authorize converter switching.
6. Add a hardware-safe default-off MCU-controlled U6 inhibit.
7. Configure and read back a low initial LM51772 average-current limit before enable.
8. Coordinate normal PV operating-floor regulation above hardware UVLO.
9. Recalculate output capacitance, compensation, current limits, thermal margins, and affected component ratings for 125 W continuous / 150 W maximum.
10. Carry forward the logged MCU symbol, I2C, BOOT0, D8 supply, and footprint corrections.

## Recommended Next Session

Start with the battery/output boundary before changing the schematic:

1. Trace the authoritative manufactured netlist and schematic from `V_out` through R16, F2, connectors, protection parts, and `BAT_BUS_POS`.
2. Draw the intended Rev 1 battery-first power path and mark fuse, reverse-polarity, disconnect, shunt, capacitor, and control-power boundaries.
3. Decide whether direct battery precharge of the output capacitors is acceptable from calculated and measured inrush stress.
4. Select the exact U6 inhibit method and prove its default-off behavior for MCU reset, unpowered MCU, USB-only service, and PV-present conditions.
5. Map LM51772 average-current, input-voltage, soft-start, and fault features to startup steps 5 through 10.
6. Only after those boundaries are agreed, modify the Rev 1 KiCad schematic and regenerate netlist/ERC/DRC evidence.

Do not begin by changing the main MOSFET/inductor topology. The first objective is a reviewed Rev 1 power-and-control path diagram plus a small schematic delta list.

## Controlling Documents

- `docs/decisions-log.md`: DEC-023 through DEC-029.
- `docs/rev-one-delta-plan-2026-08-04.md`: active Rev 1 definition and ten-step startup table.
- `docs/requirements.md`: Rev 1 power, current, startup, BMS, and capacitance requirements.
- `docs/architecture.md`: retained architecture and control ownership.
- `docs/test/validation-plan.md`: VAL-007 startup and VAL-008 output-capacitor validation.
- `SOLAR_HANDOFF_2026-09-08.md`: authoritative Rev 0 bench evidence and safety stop state.
