# Rev 1 Implementation Plan

Status: active implementation plan
Date: 2026-09-09

## Objective

Implement Rev 1 as a controlled reconfiguration of the existing charger, not a clean-sheet power-stage redesign.

Target envelope:

- 125 W continuous charger output
- 150 W maximum PV operating power
- 8 A to 10 A nominal charge current
- Approximately 12 A hardware charge-current protection ceiling
- At least 20 A BMS charge capability
- Existing 2x100 W series PV array compatibility across its complete cold-`Voc` and `Isc` envelope

Retain unless evidence forces a new decision:

- One LM51772 four-switch converter
- Existing main power-stage partition and magnetics concept
- Q5/Q6 output switching and reverse-flow subsystem
- R16 1 mOhm four-terminal output-current shunt
- F2 charger-output branch fuse
- Retained PV protection concept
- STM32 charger supervision and safety authority

## Phase Gates

### Gate 0 - Schematic Baseline Freeze

Purpose: establish the copied Rev 1 schematic as a valid, reviewable electrical source before considering PCB implementation.

Actions:

1. Confirm `Solar Project Rev1.kicad_sch` and `Solar Project Rev1.net` are the active schematic files. Treat the PCB as deferred copied artwork, not as an implementation baseline.
2. Confirm no KiCad schematic lock file exists before editing.
3. Archive the current Rev 1 schematic and netlist with timestamps.
4. Export a fresh netlist and run schematic ERC to prove the file loads and parses.
5. Record the current high-current path and all open decisions.
6. Treat the exported netlist as connectivity evidence, not as proof that the copied schematic is electrically correct or that switch-state behavior is valid.

Exit criteria:

- The source schematic loads without corruption and exports a netlist.
- The schematic has a documented ERC baseline.
- The current Rev 1 schematic is recoverable.
- No unresolved ambiguity remains about which files will be edited.

The PCB is intentionally excluded from this gate. It will not be used to approve or reject the schematic.

Authoritative baseline: the project `ERC.rpt` generated at 2026-09-09 11:31:02 reports 0 errors and 11 warnings. The warnings are documented library/link mismatches plus two `ground_pin_not_ground` findings on U6 AGND/GND. A separate CLI run produced 231 environment-dependent library warnings and is not authoritative for this project because it did not resolve the project library configuration the same way as the KiCad project export. The U6 ground findings are now a required schematic correction: AGND, PGND, and the exposed GND thermal pad must share the `PWR_NEG` DC reference.

### Gate 1 - Component And Stress Review

Purpose: determine whether the existing parts survive the lower voltage/current Rev 1 envelope and its transients.

Review order:

1. PV connector, F1, reverse-polarity protection, TVS, and input capacitors.
2. Q5/Q6 output-switch and reverse-flow stage.
3. R16 power and Kelvin connections.
4. F2 charger-output short-circuit protection.
5. L2, switching-node parts, and output capacitor bank.
6. U6, gate-drive support, ILIM/soft-start, UVLO, and configuration straps.
7. Battery connector, BMS interface, control-power converter, and USB isolation.
8. Voltage/current/temperature sensing and MCU support.

For every part record:

- Actual manufacturer part number and package.
- Maximum steady-state and transient voltage.
- Average, RMS, peak, and fault current.
- Worst-case dissipation and temperature.
- Required margin and calculation source.
- Schematic, footprint, layout, or firmware action.

Exit criteria:

- Q5, Q6, R16, F2, L2, output capacitors, and U6 have completed review rows.
- Every `REWORK` or `HOLD` item has an owner and next action.
- No part is downgraded solely because nominal voltage or current is lower.

Primary artifact: [rev-one-component-derating-review.md](rev-one-component-derating-review.md)

### Gate 2 - Approved Schematic Corrections

Purpose: make only corrections supported by Gate 1 and the accepted Rev 1 decisions.

Required correction groups:

1. **Output and protection path**
   - Preserve Q5/Q6 as the output-switch/reverse-flow subsystem.
   - Preserve R16's actual footprint mapping: pads 3/4 are the high-current element and pads 1/2 are Kelvin sense.
   - Trace and document the complete switched path through Q5/Q6 before changing nets.
   - Preserve F2 as `CHG_OUT_POS -> F2 -> BAT_BUS_POS` charger-output short-circuit protection.
   - Confirm whether a separate main battery fuse is required by the system safety boundary; do not repurpose F2 without a decision.

2. **Startup and control authority**
   - Make protected battery power the normal control-domain source (DEC-025).
   - Implement dual USB architecture (DEC-032): isolated host COM port vs reverse-blocked Aux 5V bench power port.
   - Add a hardware-safe default-off inhibit (`CTRL_EN_CHG`) for U6 during MCU reset, MCU unpowered state, invalid battery, USB-only service, and PV qualification (DEC-026).
   - Keep hardware UVLO as independent protection, not as the normal start command (DEC-027, DEC-028, DEC-030).

3. **Controlled startup**
   - Configure and read back a low initial LM51772 current limit before release.
   - Define battery-relative PV qualification.
   - Define a PV operating floor above hardware UVLO.
   - Define bounded current/power ramp, backoff, stop, and delayed retry (DEC-029).

4. **Ratings, testability, and values**
   - Recalculate output capacitance, ESR, compensation, ripple current, inrush, thermal limits, and affected component ratings (DEC-023).
   - Add dedicated test points and switch-node ground spring pads (DEC-031).
   - Resolve C52 working-voltage margin.
   - Correct only confirmed MCU, I2C, BOOT0, D8, footprint, and net-label defects.

Exit criteria:

- Each edit has a linked review row, requirement, or decision.
- No topology replacement or unrelated cleanup is included.
- Net names and protection ownership are documented before editing.

### Gate 3 - Electrical Evidence Regeneration

Purpose: prove the edited schematic still represents the intended design.

Actions:

1. Export a fresh Rev 1 netlist.
2. Run ERC and save `ERC.rpt`.
3. Compare critical net membership before and after edits.
4. Spot-check Q5/Q6 pins, R16 pads, F2 sides, `V_out`, `V_out_sense`, `CHG_OUT_POS`, and `BAT_BUS_POS`.
5. Verify gate-drive, fault, current-sense, UVLO, and MCU landing continuity.

Exit criteria:

- Zero unwaived ERC errors.
- Every remaining waiver has rationale, owner, and date.
- No critical net split, accidental bypass, or unconnected protection path.

### Gate 4 - PCB And Thermal Closure (Deferred)

Purpose: confirm the accepted schematic ratings survive physical implementation. This gate begins only after schematic acceptance.

The current PCB is a copied historical artifact and is not evidence that the Rev 1 schematic is implemented. Do not use its M1-M4 population, footprints, or routing to drive schematic decisions.

Actions:

1. Run PCB DRC and save `DRC.rpt`.
2. Review Q5/Q6 current loops, gate loops, and reverse-flow path.
3. Review R16 copper geometry, Kelvin pickup, pad sharing, and thermal path.
4. Review F2 holder contact/current rating and physical charger-output placement.
5. Review L2, input/output capacitor loop areas, connector contacts, and battery conductors.
6. Confirm creepage, clearance, copper ampacity, via current, thermal relief, and temperature-sensor placement.
7. Recheck all substituted parts with the part-substitution gate.

Exit criteria:

- Zero unwaived DRC errors.
- Critical copper and thermal paths have documented current capacity.
- Component, footprint, and routed pin maps agree.
- No safety-critical part relies on a typical-only rating.

### Gate 5 - Firmware And Bench Readiness

Purpose: make the first power test controlled and observable.

Actions:

1. Implement battery qualification and reason codes.
2. Hold U6 inhibited by default.
3. Verify current-limit configuration and readback before enable.
4. Implement PV open-circuit qualification and hysteresis.
5. Implement PV-floor regulation above hardware UVLO.
6. Implement bounded current/power ramp and weak-source backoff/retry.
7. Log PV voltage/current, battery voltage/current, output voltage, `EN_UVLO`, `SS_ATRK`, temperatures, faults, and current-limit state.
8. Prepare the staged current-limited bench procedure.

Exit criteria:

- Battery-only control startup works with PV disconnected.
- USB-only service cannot authorize switching.
- U6 remains off for reset, invalid battery, PV absent, and inhibited PV-present cases.
- Scope channels and firmware logging capture VAL-007 signals.
- September 8 safety restrictions are reread before any Rev 0 bench activity.

### Gate 6 - Controlled Validation

Purpose: validate the implemented Rev 1 design without using hardware UVLO as normal control.

Execution order:

1. Continuity and polarity checks with all energy sources disconnected.
2. Control-domain power-up from battery only.
3. U6 inhibit/default-off checks.
4. Low-current current-limit configuration/readback.
5. PV open-circuit qualification with U6 inhibited.
6. Minimum-current startup with a controlled source.
7. PV-floor regulation and bounded ramp.
8. Output-capacitor inrush and load-transient checks.
9. Strong-source rated-power test.
10. Weak-source backoff, stop, and delayed retry test.
11. Thermal soak at the defined 125 W point.

Pass criteria:

- No sustained UVLO packet bursts.
- `SS_ATRK` shows the intended controlled startup.
- Charge current remains within the configured target and hardware ceiling.
- PV operating power remains at or below 150 W.
- Output remains stable across the qualified battery range.
- No component, connector, fuse, copper, or protection boundary exceeds its reviewed limit.

Primary validation references: VAL-007, VAL-008, REQ-CHG-001 through REQ-CHG-005, and REQ-SAF-001 through REQ-SAF-004.

## Implementation Stop Conditions

Stop and review before proceeding if any of these occur:

- Q5/Q6 body-diode or gate-state behavior is not proven.
- R16 current direction or Kelvin polarity is ambiguous.
- F2 is changed from charger-output protection without a recorded decision.
- Any power MOSFET, fuse, connector, inductor, or capacitor has no worst-case stress calculation.
- USB or MCU reset can release U6 unintentionally.
- PV voltage regulation depends on crossing hardware UVLO.
- ERC/DRC reveals a new safety-critical error.
- Bench work would violate the September 8 safety stop or `CLEAR_FAULTS` restriction.

## First Three Work Sessions

### Session 1 - Finish Baseline Review

- Classify the 231-violation ERC baseline and resolve the U6 AGND/GND grounding intent before changing component values.
- Complete Q5/Q6, R16, F2, L2, output-capacitor, and U6 review rows.
- Confirm actual manufacturer part numbers and footprints from schematic/PCB.
- Calculate Rev 1 peak/RMS current and voltage stress.
- Resolve which items are `PASS`, `REWORK`, or `HOLD`.

### Session 2 - Approve Schematic Delta

- Review only the `REWORK` rows.
- Agree the Q5/Q6/R16/F2 path in the schematic.
- Define battery control-power source and default-off U6 inhibit.
- Define current-limit, PV-floor, and startup-control interfaces.
- Record any changed decisions before editing.

### Session 3 - Edit And Regenerate Schematic Evidence

- Check for the KiCad schematic lock file.
- Edit the copied Rev 1 schematic in bounded blocks.
- Export netlist and ERC after each logical block.
- Defer PCB and DRC work until the schematic topology and ratings are accepted.
- Run critical-net spot checks and update the closure checklist.

## Schematic Definition Of Done

The Rev 1 schematic is ready for PCB reconciliation when:

- All required component review rows are `PASS` or explicitly waived.
- The schematic implements the approved path and control boundaries.
- Netlist and ERC evidence are current.
- Output capacitance, compensation, current limits, thermal margins, and inrush are justified.
- Battery-first startup, default-off inhibit, PV qualification, and weak-source recovery are implemented.
- The copied origin of the schematic is documented and no unresolved schematic safety blockers remain.

PCB reconciliation, footprint verification, DRC, and routed thermal closure are a later implementation phase.
