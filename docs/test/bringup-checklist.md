# Solar Project 200W - Rev 1 Bench Bring-Up Plan

Last updated: 2026-08-24
Status: first-board procedure; high-energy stages require the listed gates
Purpose: safely establish that the fabricated Rev 1 board can be powered, programmed, observed, and progressively exercised without bypassing its protection boundaries.

This procedure supersedes the Phase 0 planning checklist. It applies to the Rev 1 board only. Record completed rows, instrument settings, firmware revision, and photographs in a dated bench log before advancing a stage.

## 1. Boundaries and Non-Negotiable Rules

- The STM32 remains the sole owner of PWM, charging, limits, and fault handling. The ESP32 is a network coprocessor only (DEC-015).
- First energization uses a current-limited, isolated bench supply. Do not use a battery, PV panel, USB host port, or inverter as an uncontrolled first source.
- Do not inject power directly into `CTRL_5V`, `CTRL_3V3`, `PV_IN_POS`, `CHG_OUT_POS`, or `BAT_BUS_POS` unless a documented rework/isolation plan says otherwise. Use the intended connector or `VBUS` bench-power entry.
- Do not connect a real battery until Q-001 is closed with the pack/BMS charge, discharge, voltage, and fault limits in the bench log.
- Do not connect real panels until the PV source voltage/current envelope, F1 rating, D1/D2 ratings, and C52 voltage margin have been checked against the actual fitted parts.
- Do not connect or test an AC load or inverter in this procedure. AC-domain work needs its own approved hazard plan (REQ-SAF-005).
- Remove PV and battery sources before moving probes, changing jumpers, loading firmware, or altering board configuration.

## 2. Required Equipment and Records

| Item | Minimum capability | Record before use |
|---|---|---|
| Isolated bench supply | Current limiting, voltage readback, remote output enable | Model, calibration status, voltage/current limits |
| DMMs | Two preferred: supply and rail measurements | Model and input jack/fuse check |
| Oscilloscope | Differential or properly isolated probing for switching nodes | Probe type, attenuation, ground reference |
| Current measurement | Supply readback for low energy; current probe or shunt method for converter tests | Range, bandwidth, shunt value if used |
| Electronic load or bidirectional battery simulator | Suitable for planned `CHG_OUT_POS`/battery test energy | Sink/source limits and cooling plan |
| SWD programmer and serial adapter | STM32 programming and local logs | Firmware commit/hash and port settings |
| Thermal check | IR camera or contact probe | Emissivity/attachment method |
| Safety equipment | Eye protection, insulated probes, accessible supply disconnect, appropriate fuses | Emergency-off method and operator |

Create one evidence directory per board and session, for example `evidence/YYYY-MM-DD-rev1-boardNN/`. Store the filled table in Section 10, photographs of both sides before power, scope captures, firmware hashes, and any fault report there.

## 3. Mandatory Gates Before Energizing

| Gate | Pass criterion | Status / evidence |
|---|---|---|
| Design identity | Board revision, assembly revision, and fitted BOM are recorded; no unapproved substitutions are present. |  |
| Visual assembly | No solder bridges, damaged parts, reversed polarized parts, loose hardware, or contamination. Inspect all power semiconductors, fuses, connectors, U4/U5, STM32, and ESP32. |  |
| Documentation | Latest schematic, DRC/ERC evidence, and board layout are available at the bench. Any unresolved ERC/DRC item is explicitly waived for this test. |  |
| C52 | Fitted C52 voltage rating is documented and exceeds its worst-case applied voltage with the selected derating policy. |  |
| BMS envelope | Q-001 is closed before a real battery is connected. Until then, only a controlled simulator may represent the battery domain. |  |
| Firmware safe state | Test firmware boots with `CTRL_PWM_MAIN` inactive, charge enable inactive, and all fault responses latched safe until explicitly commanded. |  |
| Test topology | Only the source(s), sink(s), fuses, and connections for the selected stage are connected. All other energy ports are open and labelled. |  |

Stop and correct the issue before proceeding if any gate fails.

## 4. Stage A - Unpowered Inspection and Continuity

**Configuration:** all external power removed; PV, battery, inverter, USB host, SWD, and ESP32 external programming cables disconnected unless needed for a passive measurement.

| Check | Method | Pass criterion |
|---|---|---|
| Connector polarity | Compare connector pin 1/polarity marks with the Rev 1 schematic and silkscreen. | Every external power connector is positively identified. |
| Protective parts | Verify F1/F2 values, D1/D2 polarity, D10 polarity, TVS orientation, MOSFET orientation, and electrolytic/polymer capacitor polarity against the fitted BOM. | No discrepancy. |
| Power-to-return resistance | Measure resistance from `VBUS`, `CTRL_5V`, `CTRL_3V3`, `PV_IN_POS`, `CHG_OUT_POS`, and `BAT_BUS_POS` to `PWR_NEG`. | No unexpected near-short. Record readings after they stabilize. |
| Adjacent-net shorts | Check adjacent fine-pitch pins on STM32 and ESP32, and supply/ground pins on U4 and U5. | No short other than intentional net ties. |
| Fuse/path continuity | Check only the intended protected paths: PV connector through F1/D1, charger output through F2 to `BAT_BUS_POS`, and the USB/`VBUS` path through D10 toward `CTRL_5V`. | Continuity agrees with schematic; no bypass is found. |
| D10 reverse isolation | In diode mode, record forward and reverse readings across D10 and compare its installed polarity to the schematic. | Forward direction is `VBUS -> CTRL_5V`; reverse direction does not permit USB backfeed. |
| Control signals | Verify `CTRL_PWM_MAIN`, charge-enable path, `FAULT_OCP`, and `FAULT_OVP` are not shorted to power or return. | No unintended short; expected pull states are recorded. |

**Stop criteria:** any low-resistance power short, reversed part, missing fuse, or D10 direction mismatch.

## 5. Stage B - Control-Power Rails and Programming

**Configuration:** PV, battery, charger load, and inverter remain disconnected. Supply the designed `VBUS` entry with an isolated bench supply. Do not rely on a computer USB port for power.

1. Set the supply to 0 V, output off, and a conservative current limit that is below the connector/fuse rating but sufficient for the fitted ESP32 module's documented boot current. Record the value.
2. Connect supply positive to `VBUS` and return to `PWR_NEG`; verify leads before enabling output.
3. Raise to the intended USB-equivalent voltage. If the supply reaches current limit, voltage droops, or a part warms unexpectedly, turn output off and return to Stage A.
4. Record `VBUS`, `CTRL_5V`, `CTRL_3V3`, and supply current. Confirm U4/U5 output voltages against their actual fitted-part datasheets.
5. With `VBUS` unpowered, confirm the control side does not hold `VBUS` at a backfeed voltage. This is the powered complement to the Stage A D10 check.
6. Program the STM32 through SWD with the safe-state test firmware. Confirm reset, SWD attach, a local serial log, and that `CTRL_PWM_MAIN` remains inactive on the scope.
7. Boot the ESP32. Confirm it is powered from its `5V` pin, its exposed `3.3V` pin is unused, and no part of the ESP32 module or its regulator overheats.
8. At 115200 baud, verify crossed UART traffic: STM32 `TXD_ESP` is received at ESP32 `RXD_ESP`, and ESP32 `TXD_ESP` is received at STM32 `RXD_ESP`. Record the actual MCU and ESP32 pins. Keep RTS/CTS disabled unless the firmware and wiring test explicitly enable them.

**Pass criteria:** stable intended rail voltages, no current-limit event, no abnormal heating, reproducible SWD/programming, safe PWM state, and bidirectional UART traffic. Wi-Fi association, MQTT, and OTA are out of scope for power-stage release and must not alter charger control outputs.

## 6. Stage C - Battery-Domain Static Checks

**Configuration:** PV disconnected; inverter disconnected; PWM and charger enable forced inactive. Use a protected, current-limited battery simulator only. A real battery is prohibited until the Section 3 BMS gate passes.

1. Set simulator voltage to the documented low-energy value inside the final LiFePO4 operating range and set a conservative current limit. Record both values and the simulator's ability to absorb current.
2. Connect through the intended battery connector and main protection path. Keep an accessible disconnect at the source.
3. Enable the simulator and record source voltage/current, `BAT_BUS_POS`, `CHG_OUT_POS`, control rails, and all control/fault states.
4. Verify no reverse current flows into `VBUS` or the disconnected PV port.
5. Verify the firmware reports battery voltage plausibly and retains `CTRL_PWM_MAIN` inactive.
6. Turn the simulator off, wait for stored energy to discharge, then inspect the board thermally and visually.

**Stop criteria:** unexpected current, rail backfeed, battery-path heating, a fault line in an unexplained state, or active PWM.

## 7. Stage D - PV Front End and Charger Static Checks

**Configuration:** battery source removed unless a rated battery simulator or electronic load is explicitly connected as the output energy sink. Inverter disconnected. `CTRL_PWM_MAIN` and charger enable remain inactive.

1. Use a current-limited programmable supply as the PV emulator through the intended PV connector and F1 path. Start at 0 V and a low current limit.
2. Increase only to a voltage that is inside the verified fitted-part PV-input envelope. Record the source setting, `PV_IN_POS_RAW`, `PV_IN_POS_FUSED`, `PV_IN_POS`, and source current at each increment.
3. Confirm F1, D1, D2, input capacitors, and the LM74502H/back-to-back MOSFET protection block have the expected voltage drops and no abnormal temperature rise.
4. Verify the control rail behavior and that D10 prevents power from appearing at the USB/`VBUS` port in the reverse direction.
5. With the output sink connected and PWM still disabled, verify `CHG_OUT_POS` remains at the expected static state and no energy is delivered unexpectedly.

**Pass criteria:** protected PV progression agrees with the schematic, control rails remain stable, no reverse feed, and no switching waveform is present.

## 8. Stage E - Controlled Converter Switching

**Entry gate:** Stages A-D pass; fitted Q1, D3, L1, C4-C7, output capacitor ratings, current-sense scaling, firmware current/voltage limits, and output sink capability are documented. Do not enter this stage from an unknown or unverified BOM.

1. Use the PV emulator and a rated electronic load or bidirectional battery simulator at the charger output. Do not use a real battery for initial switching.
2. Install temporary branch fusing sized for the planned low-power test. Set the PV source current limit to the lowest value that supports observable switching; record it.
3. Command the minimum-duty or controlled soft-start mode. Probe `MPPT_SW_NODE` only with a safe probing method that cannot short the switch node to protective earth.
4. Capture `PV_IN_POS`, `MPPT_SW_NODE`, `CHG_OUT_POS`, output current, `CTRL_PWM_MAIN`, and fault lines. Verify duty cycle, frequency, overshoot, ripple, and current-sense sign against firmware expectations.
5. Increase one variable at a time: input voltage, duty/current command, or output load. At every step, log input/output power, temperatures at Q1/D3/L1, and any fault response.
6. Deliberately exercise only approved low-energy fault injection methods. Confirm `FAULT_OCP` and `FAULT_OVP` force the documented safe state and require the intended recovery sequence.
7. Stop at the predeclared energy limit for the session. Thermal/efficiency characterization and charge-current ramping are separate tests after this stage is repeatable.

**Immediate stop criteria:** current-limit trip, uncontrolled duty cycle, output overvoltage, incorrect current direction, ringing beyond fitted-part ratings, loss of fault response, smoke/odor/noise, or any component exceeding its approved temperature limit.

## 9. Deferred Tests and Release Gates

| Test area | Not part of first board power-up | Entry requirement |
|---|---|---|
| Real battery charging | Yes | Q-001 closed; charge profile, BMS limits, cable/fuse ratings, and controlled charge plan approved. |
| Full 200 W PV operation | Yes | Panel envelope and all thermal/current derating calculations completed; electronic source/sink and instrumentation are rated. |
| ESP32 Wi-Fi/MQTT/OTA | Yes | UART safe-state behavior proven; network test cannot toggle charger-control nets. |
| Remote commands | Yes | Authenticated/encrypted transport and STM32 command validation are implemented and tested. |
| Inverter or AC output | Yes | Separate AC hazard procedure, enclosure, load, isolation, and emergency-disconnect review approved. |

## 10. Session Record

| Stage | Date/time | Board / firmware | Source and limits | Result | Evidence / observations | Operator |
|---|---|---|---|---|---|---|
| A - Unpowered |  |  |  | Pass / Fail / Blocked |  |  |
| B - Control power |  |  |  | Pass / Fail / Blocked |  |  |
| C - Battery static |  |  |  | Pass / Fail / Blocked |  |  |
| D - PV static |  |  |  | Pass / Fail / Blocked |  |  |
| E - Switching |  |  |  | Pass / Fail / Blocked |  |  |

## 11. Traceability and Closeout

| Requirement | Evidence from this procedure |
|---|---|
| REQ-SAF-001, REQ-SAF-002 | Stages A/C and source/fuse records |
| REQ-SAF-003, REQ-SAF-004 | Stages A/D/E protection and fault captures |
| REQ-SAF-006 | Completed staged record with stop criteria |
| REQ-TEL-001, REQ-TEL-002 | Stage B serial log and Stage C-E measurement records |
| REQ-VAL-001 | Dated evidence directory and completed result table |

At session close, power down all sources, verify stored energy has discharged, photograph the board, archive raw captures, and add every failure or waiver to the decisions log with an owner and target date. A failed stage is a useful result; do not advance around it.
