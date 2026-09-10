# Rev One Delta Plan (2026-08-04)

Owners: Phil + Copilot  
Status: Active Rev 1 definition

## Revision Strategy

Per DEC-024, Rev 1 is a targeted reconfiguration and correction of Rev 0, not a clean-sheet redesign. The existing conversion architecture remains the starting point; changes are limited to rating updates, confirmed defect corrections, startup/control authority, protection, and value or layout changes supported by analysis and test.

Retain, subject to verification:

- One LM51772 and the four-switch conversion topology.
- Main power-stage partition, magnetics concept, and gate-drive architecture.
- LM74502H plus back-to-back N-MOS PV input-protection concept.
- PV voltage/current, battery voltage/current, and temperature sensing functions.
- R16 output-current-shunt function and the direct converter-output-to-battery-bus concept.
- STM32 ownership of charger supervision, safety policy, and telemetry.

Reconfigure or correct:

- Lower the operating rating to 125 W continuous and 150 W maximum PV power.
- Make battery-first operation an explicit prerequisite and normal control-power source.
- Correct the F2 fuse bypass and verify the complete protected battery path.
- Add default-off MCU inhibit authority over U6 startup.
- Configure and verify low-current startup before enabling conversion.
- Coordinate PV operating-floor regulation above hardware UVLO.
- Recalculate output capacitance and all affected current, thermal, and protection values.
- Correct previously logged MCU symbol, I2C, BOOT0, D8 supply, and footprint implementation defects.

A replacement converter topology, multiple parallel charger stages, or a substantially new power-stage partition is outside the accepted Rev 1 scope unless later evidence forces a new decision.

## Charger Rating And Startup Baseline

- Continuous charger output rating: 125 W at the defined thermal design point.
- Maximum PV operating power: 150 W. If a larger array is connected, control shall limit harvested power to 150 W while the input hardware remains rated for the array's full `Voc` and `Isc` envelope.
- Nominal battery charge-current target: 8 A to 10 A for the 4S LiFePO4 battery range.
- Hardware charge-current protection ceiling: approximately 12 A, pending final tolerance, transient, thermal, and BMS-limit analysis.
- Battery/BMS charge-current capability shall be at least 20 A so normal charger operation does not depend on the BMS current trip.
- Retain one LM51772 power-conversion stage. Multiple charger stages are not required by the current Rev 0 evidence.
- Recalculate required output capacitance for Rev 1; do not carry the approximately 1.1 mF to 1.2 mF Rev 0 bank forward without transient and stability justification.

## Ten-Step Startup Definition And Rev 0 Delta

Rev 0 did connect the converter output to `BAT_BUS_POS` through the 1 mOhm R16 current shunt. A connected battery could therefore establish the output bus and precharge the approximately 1.1 mF to 1.2 mF output capacitor bank. However, this behavior was not made an explicit startup prerequisite, battery qualification did not gate U6, and first switching was tested without a battery. In the authoritative manufactured-build netlist, both F2 pins are also on `BAT_BUS_POS`, so F2 does not provide the documented series fuse boundary and must be corrected in Rev 1.

| Step | Rev 1 required behavior | Difference from Rev 0 | Rev 1 implementation and verification intent |
|---|---|---|---|
| 1. Connect and protect the battery | Battery connection establishes the charger output bus before PV conversion is permitted. | Rev 0 had a low-impedance `V_out -> R16 -> BAT_BUS_POS` path, but battery presence was not required for the first switching test and F2 is bypassed in the manufactured netlist. | Correct the series fuse path, define safe connector sequencing, and verify battery reverse-polarity protection. Measure capacitor-connection inrush and add battery-side precharge or inrush limiting only if the connector, fuse, shunt, or capacitor rating requires it. |
| 2. Power control from the battery | The protected battery bus powers the MCU, sensing, and converter-supervisory control before U6 can switch. | Rev 0 used a flexible PV-first, battery/USB-assisted control supply. Control availability therefore did not prove that a valid battery was attached. | Make protected battery power the normal runtime control source. Keep USB as an isolated service source that cannot authorize power-stage operation by itself. Verify control startup with battery only and zero PV. |
| 3. Validate the battery domain | Firmware confirms battery presence, plausible voltage, allowed LiFePO4 temperature, BMS permission when available, and no blocking hardware fault. | Rev 0 measured battery-related signals but did not make a completed battery qualification a hardware-enforced prerequisite to autonomous U6 startup. | Define voltage and temperature limits with hysteresis and debounce. A missing, disconnected, overvoltage, undervoltage, or too-cold battery keeps the converter disabled and records a reason code. |
| 4. Hold the converter disabled | U6 remains positively inhibited while control initializes and both energy sources are being qualified. | Rev 0 U6 started autonomously when the analog `EN_UVLO` divider crossed threshold; the STM32 did not gate individual pulses and `CTRL_EN_CHG` was not the effective first-power enable. | Add a real MCU-controlled enable/inhibit path with a hardware-safe default-off state. Hardware UVLO remains independent protection, not the normal start command. Verify that PV can rise to maximum normal `Voc` without switching while inhibit is asserted. |
| 5. Measure and qualify PV open-circuit voltage | With U6 disabled, measure unloaded PV voltage and require it to exceed a battery-relative startup threshold for a defined qualification time. | Rev 0 relied mainly on a fixed analog `EN_UVLO` crossing near the observed 6 V bench point; it did not first qualify source headroom relative to the battery. | Begin with a commercial-style threshold of approximately `Vbat + 5 V`, then tune from LM51772 headroom and panel tests. Apply hysteresis, debounce, and cold-`Voc` protection. |
| 6. Start at minimum commanded current | Release inhibit with the LM51772 configured for a low initial average-current or power demand and a validated soft-start ramp. | Rev 0 `SS_ATRK` did not visibly ramp during the failed bursts. The fitted ILIMCOMP/ISET condition was near VCC2 before startup, so the intended programmable average-current limit was not demonstrated. | Configure and read back U6 limits before enable. Select the initial current experimentally, starting in the sub-ampere range, and verify monotonic `SS_ATRK`, output-current, and PV-voltage behavior. Do not depend on output-voltage soft-start alone to protect a finite PV source. |
| 7. Establish input-voltage regulation before MPPT | A fast inner source-protection loop holds PV above a configured operating floor as current begins to rise. | Rev 0 hardware UVLO stopped switching only after `EN_UVLO` fell; no validated loop reduced demand before dropout. | Use the LM51772 input-voltage/current features where suitable, with MCU supervision as a slower outer limit. Set the normal PV floor above hardware UVLO with enough margin for ripple and transients. |
| 8. Ramp toward available power | Increase current or power in bounded increments only while PV voltage, battery current, output power, and temperature remain valid. | Rev 0 transitioned directly into autonomous switching and packet retries; it had no demonstrated controlled progression from a small source load to useful charging power. | Use a timed ramp with rate limits and explicit ceilings: 8 A to 10 A nominal charge target, 125 W continuous output, 150 W maximum PV operating power, and approximately 12 A hardware protection. Capture every limiting reason in telemetry. |
| 9. Enable MPPT and battery charge-state control | After stable conversion is established, the MPPT algorithm adjusts the source operating point while battery CC/CV, power, current, and thermal limits retain priority. | Rev 0 had sensing and controller infrastructure, but stable startup was never reached and coordinated MPPT/charge-loop ownership was not validated. | Run MPPT slower than the electrical regulation loops. The most restrictive active limit wins. For LiFePO4, use battery-manufacturer charge voltage, low-temperature cutoff, and termination guidance rather than lead-acid assumptions. |
| 10. Back off, stop, and retry deliberately | Weak PV first causes current reduction; persistent collapse causes an orderly stop, source recovery wait, and delayed retry. | Rev 0 repeatedly crossed UVLO in approximately 80 us switching packets with millisecond-scale retries and no visible soft-start ramp. | Define separate backoff, stop, and restart thresholds with hysteresis and a minimum off-time. Verify sunrise, cloud-edge, partial-shade, battery-disconnect, and BMS-trip cases without sustained UVLO packet bursts. |

The first three steps establish the battery domain. Steps 4 through 6 create a controlled start. Steps 7 through 9 transfer operation into regulated charging and MPPT. Step 10 handles insufficient energy without using hardware UVLO as the normal operating controller.

Decision allocation:

- Steps 1-2: DEC-025, battery-first output and control domain.
- Steps 3-4: DEC-026, battery qualification and converter inhibit.
- Steps 5-6: DEC-027, PV qualification and bounded initial startup.
- Steps 7-8: DEC-028, source regulation and controlled power ramp.
- Steps 9-10: DEC-029, MPPT hierarchy and weak-source recovery.

The Rev 0 approximately 6 V, 200 mA bench result is evidence of source collapse at the enable transition, not proof that a real PV array lacks the total energy needed to charge the output capacitors. Rev 1 closure therefore requires separate validation of soft-start, PV-voltage regulation, UVLO coordination, and output-capacitor sizing.

## Scope Guardrails

- Keep LM74502H plus back-to-back high-side N-MOS input path intact unless an explicit decision log says otherwise.
- Keep C52 voltage-rating margin review as an open protection carry-over.
- Treat the battery fuse bypass, converter inhibit authority, average-current-limit configuration, source-aware startup, and output-capacitance justification as newly identified mandatory Rev 1 deltas.
- Do not reopen unrelated architecture branches in this pass.

## Rev One Delta Table By Schematic Sheet

| Sheet | Change Item | Priority | Owner | Intent | Evidence / Reference |
|---|---|---|---|---|---|
| Sheet 1 - PV Input / Protection | Re-label active schematic lineage so current design is Rev 1 and recovered JLC-era design is Rev 0 | Must-have | Phil | Prevent revision ambiguity before board updates | hardware/kicad/solar-project/Solar Project.kicad_sch, hardware/kicad/solar-project/Solar Project Rev1.kicad_sch, hardware/kicad/solar-project/Solar Project Rev0.kicad_sch |
| Sheet 1 - PV Input / Protection | Confirm C52 voltage-rating margin policy (keep vs raise) and document as freeze gate | Must-have | Phil | Close final protection-margin open item | hardware/kicad/solar-project/Solar Project.kicad_sch, hardware/mppt/pv-input-wiring-map.md |
| Sheet 1 - PV Input / Protection | Replace any residual legacy net labels tied to bench naming if they still appear after ERC export | Optional | Copilot | Improve netlist readability without functional change | hardware/kicad/solar-project/Solar Project.net |
| Sheet 2 - Buck / Main Power Stage | Keep current Rev 0 validated protection feed into power stage unchanged while routing updates proceed | Must-have | Phil | Avoid regressions while introducing board-level refinements | hardware/mppt/buck-power-stage-wiring-map.md |
| Sheet 2 - Buck / Main Power Stage | Recalculate the output capacitor bank and document required capacitance, ESR, ripple-current, and startup-inrush limits | Must-have | Phil + Copilot | Avoid carrying the approximately 1.1mF to 1.2mF Rev 0 bank forward without stability and transient evidence | REQ-CHG-005, VAL-008 |
| Sheet 2 - Buck / Main Power Stage | Optional cleanup of stale placeholder notes that imply alternate gate-drive ownership | Optional | Copilot | Keep documentation aligned with active CTRL_PWM_MAIN ownership | hardware/mppt/buck-power-stage-wiring-map.md, hardware/mppt/schematic-notes.md |
| Sheet 3 - Battery Interface | Remove the manufactured Rev 0 F2 bypass and implement a real series battery fuse boundary | Must-have | Phil | Ensure battery protection is electrically effective; the authoritative Rev 0 netlist places both F2 pins on `BAT_BUS_POS` | hardware/kicad/solar-project/Solar Project Rev0 - Manufactured First Build.net, REQ-SAF-001 |
| Sheet 3 - Battery Interface | Verify `V_out -> current shunt -> protected BAT_BUS_POS` continuity and characterize battery-to-output-capacitor connection inrush | Must-have | Phil + Copilot | Let the battery establish the output bus without overstressing the connector, fuse, shunt, or capacitor bank | REQ-CHG-004, VAL-007, VAL-008 |
| Sheet 3 - Battery Interface | Add any board-assembly convenience testpoint placement notes found during Rev 1 layout pass | Optional | Phil | Improve bring-up workflow | docs/test/bringup-checklist.md |
| Sheet 4 - Sensing / Control | Add a hardware-safe default-off MCU inhibit path that positively prevents U6 switching during initialization and source qualification | Must-have | Phil + Copilot | Separate the normal start command from analog hardware UVLO; Rev 0 switched autonomously at the UVLO threshold | REQ-CHG-004, VAL-007 |
| Sheet 4 - Sensing / Control | Implement and verify programmable low-current startup through the LM51772 average-current-limit controls | Must-have | Phil + Copilot | Ensure startup begins at bounded demand and that configuration can be read back before enable | REQ-CHG-002, REQ-CHG-004 |
| Sheet 4 - Sensing / Control | Define battery-relative PV start, backoff, stop, and delayed-retry thresholds above hardware UVLO | Must-have | Phil + Copilot | Prevent weak-source operation from devolving into repeated UVLO packet bursts | REQ-CHG-004, VAL-007 |
| Sheet 4 - Sensing / Control | Re-run fault-net and control-net continuity checks after Rev 1 relabel to guard against accidental disconnects | Must-have | Phil | Keep protection telemetry behavior intact | hardware/mppt/sheet4-sensing-control-wiring-map.md, hardware/kicad/solar-project/Solar Project.net |
| Sheet 4 - Sensing / Control | Optional canonical-name cleanup for any remaining non-canonical helper labels not tied to functional ambiguity | Optional | Copilot | Reduce naming debt without topology churn | hardware/mppt/schematic-notes.md |

## Approved Rev One Working References

- Schematic active file: hardware/kicad/solar-project/Solar Project.kicad_sch
- Board active file: hardware/kicad/solar-project/Solar Project.kicad_pcb
- Rev 1 archive copies: hardware/kicad/solar-project/Solar Project Rev1.kicad_sch and hardware/kicad/solar-project/Solar Project Rev1.kicad_pcb
- Rev 0 recovered baseline: hardware/kicad/solar-project/Solar Project Rev0.kicad_sch and hardware/kicad/solar-project/Solar Project Rev0.kicad_pcb

## Commit Split Plan

### Commit A: Schematic / Netlist Changes

Include only design-source and generated-evidence changes:

- hardware/kicad/solar-project/Solar Project.kicad_sch
- hardware/kicad/solar-project/Solar Project.kicad_pcb (if changed during Rev 1 edits)
- hardware/kicad/solar-project/Solar Project.net
- hardware/kicad/solar-project/ERC.rpt
- hardware/kicad/solar-project/DRC.rpt
- hardware/kicad/solar-project/Solar Project Rev0.kicad_sch
- hardware/kicad/solar-project/Solar Project Rev0.kicad_pcb
- hardware/kicad/solar-project/Solar Project Rev1.kicad_sch
- hardware/kicad/solar-project/Solar Project Rev1.kicad_pcb

Exclude lock/autosave artifacts:

- hardware/kicad/solar-project/~Solar Project.kicad_sch.lck
- hardware/kicad/solar-project/~Solar Project.kicad_pcb.lck
- hardware/kicad/solar-project/#auto_saved_files#/

### Commit B: Notes / Handoff / Checklists

Include only documentation updates:

- docs/rev-one-delta-plan-2026-08-04.md
- docs/test/rev-one-erc-drc-closure-checklist.md
- hardware/mppt/schematic-notes.md
- HANDOFF.md
- SOLAR_HANDOFF_2026-08-04.md
- NEXT_CHAT_PROMPT.md

## Immediate Execution Sequence

1. Export fresh netlist, ERC, and DRC from current Rev 1 active files.
2. Run closure checklist in docs/test/rev-one-erc-drc-closure-checklist.md.
3. Stage and commit Commit A first.
4. Stage and commit Commit B second.
