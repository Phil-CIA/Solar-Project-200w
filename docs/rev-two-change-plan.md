# Rev2 change plan

Planning checkpoint: 2026-11-09. Proposed work, not authorization to apply every option below. No electrical CAD changes are made by this plan.

## Baseline and scope

- Retain LM51772 (Phil's decision, DEC-067). No BQ25756 evaluation or purchase is required for this plan; the unperformed comparison in issue #20 is not a new blocker to the selected architecture.
- BMS communications is included in Rev2 (Phil confirmed 2026-11-09), fulfilling the Rev1 deferral in DEC-062. Only its implementation details remain open; do not defer it again or treat it as an optional feature.
- Start from `Solar Project Rev2.kicad_pro` and its matching schematic/PCB in `hardware\kicad\solar-project`, merged at `488441a` and tagged `rev2-kicad-starting-baseline-2026-11-09`.
- Rev2 currently copies Rev1 circuitry and routing; it is not qualified or ready for fabrication. Preserve Rev1 and its manufacturing package.
- Keep the existing 4S LiFePO4 / 2P panel baseline and current design targets: 125 W continuous output, up to 150 W PV operating power, 8-10 A nominal charging, approximately 12 A hardware protection pending verification. The project name does not establish a 200 W output rating.
- Work toward a functional, serviceable charger prototype. Do not silently add an expanded communications/UI feature set or require a formal lab dossier.

## A. Fix or reconcile

CSA/CSB work has started with the [saved-source investigation](rev-two-current-sense-review.md): polarity/connectivity, inherited routing geometry, filter bandwidth and nominal peak-current scaling. No electrical change is implemented yet.

| Item | Planned work | Done when / tracker |
|---|---|---|
| Peak-current sense path | Resolve the CSA/CSB false-trip versus real-peak-current question. Select R22 and R26/R27/C20 from the intended operating point; implement deliberate Kelvin pickup, correct polarity, short paired routing and separation from switching/gate nets. Do not prescribe a capacitor value from attenuation alone. | Calculated current threshold and measured behavior agree; intended protection remains effective. [#19](https://github.com/Phil-CIA/Solar-Project-200w/issues/19). |
| Feedback wiring versus firmware | Recommended: internal I2C-controlled feedback, with FB-to-VCC2 established before startup and matching DIV20 configuration. Confirm this choice before editing; remove or deliberately mark unused divider parts rather than leaving ambiguous options. | Hardware mode, firmware target range/readback and independently measured Vout agree. [#21](https://github.com/Phil-CIA/Solar-Project-200w/issues/21). |
| Average-current limiting / ISET | Explicitly choose the pin-controlled or register-controlled limit and design/program it accordingly. Reconcile R68/R46, ISET, MODE and D9 selection; calculate limits separately from the CSA/CSB peak-current limit. Do not carry the bench VCC2 jumper forward as an approved limit. | Both average and peak protection have defined limits, startup states and validation evidence. [#8](https://github.com/Phil-CIA/Solar-Project-200w/issues/8), [#19](https://github.com/Phil-CIA/Solar-Project-200w/issues/19), [#10](https://github.com/Phil-CIA/Solar-Project-200w/issues/10). |
| Input sensing / UVLO / configuration | Recalculate VIN-FB and EN/UVLO for the intended source range; investigate the persistent INPUT status without assuming it caused the earlier stall. Check current ADDR/SLOPE, MODE, RT, configuration straps and ground returns against the actual Rev2 source, not September net descriptions. Define PCM behavior for charging. | Thresholds, divider values, switching mode/frequency and reset/startup settings are intentional and documented. #8, #10. |
| Control-supply power path | Trace D4/D6, U4, battery and charger-output supplies. Resolve source priority, startup dependence and backfeed with battery-only, PV-only, USB-only and missing-source cases. | Schematic and state table agree; service power cannot authorize switching. [#22](https://github.com/Phil-CIA/Solar-Project-200w/issues/22), [#4](https://github.com/Phil-CIA/Solar-Project-200w/issues/4). |
| D5 / output protection documentation | Resolve the reported D5 symbol/polarity discrepancy against the intended clamp function, footprint pin mapping and Rev2 nets. Retain U14/Q5/Q6 unless review identifies a specific required change; do not redesign a path solely because its full qualification is undocumented. | Unambiguous schematic/BOM/PCB polarity; remaining default-off, reverse-flow and clamp qualification stays visible. [#9](https://github.com/Phil-CIA/Solar-Project-200w/issues/9). |
| Compensation, soft start and gates | Carry C24 = 22 nF as the documented starting value, not 220 nF. Confirm the fitted Rev1 network where needed to interpret tests. Calculate and validate compensation/soft start; assess 10 versus 22 Ohm gates against switching loss/ringing. | Selected values have a reason and applicable startup/load/thermal checks. C26 = 690 nF and 22 Ohm gates are not accepted merely because they were fitted on the bench. [#23](https://github.com/Phil-CIA/Solar-Project-200w/issues/23), #10. |
| Safe firmware defaults | Separate bench overrides from the charging application. Define inhibited boot/reset/watchdog/update behavior and battery/PV qualification; review initialization that currently drives PA5 high. | No USB-only, invalid-source or recovery state unintentionally enables charging. [#5](https://github.com/Phil-CIA/Solar-Project-200w/issues/5), #8. |

These are a mixture of confirmed discrepancies and unresolved design/qualification questions, not a claim that every listed circuit is defective.

## B. Add

| Addition | Planned scope | Decision or constraint |
|---|---|---|
| Defined BOOT0 | Add the documented 10 kOhm pull-down at U1 pin 46, with accessible recovery provisions. | Check option-byte/update assumptions; include part and routing in BOM/PCB. [#14](https://github.com/Phil-CIA/Solar-Project-200w/issues/14). |
| Actual voltage/current telemetry | Define required PV/battery/output measurements and reuse suitable existing channels. Add the missing safe measurement front-end, calibration and explicit SET versus MEAS reporting. | MCU pins are constrained: compare ADC/multiplexer and I2C-monitor approaches before picking parts. Check full common-mode/transient range, shunt loss and address allocation; never connect switching-referenced sense nodes directly to the MCU. [#15](https://github.com/Phil-CIA/Solar-Project-200w/issues/15), [#6](https://github.com/Phil-CIA/Solar-Project-200w/issues/6). |
| Control-rail transient protection | Select coordinated protection for CTRL_5V/CTRL_3V3 and relevant external entries, with appropriate current limiting/return paths. | Check actual clamp voltage at expected pulse current against IC limits; a nominal TVS voltage alone is not protection proof. [#16](https://github.com/Phil-CIA/Solar-Project-200w/issues/16). |
| Keyed debug connector | Add accessible IDC debug access with VTref, grounds and reset. | Confirm full JTAG versus SWD cable/probe standard, MCU pin availability and conflicts before assigning nets; retain recovery access. [#17](https://github.com/Phil-CIA/Solar-Project-200w/issues/17). |
| Useful test access | While modifying each block, provide accessible control rails, output/battery voltage, inhibit/fault and sense measurement points where needed. | Keep current-sense pads small and paired; no long stubs or added pickup loops on sensitive nets. Test access is not permission to use earth-grounded probes on switching nodes. |
| Robust local telemetry link | Integrate the existing protected ESP32 provisioning application with the STM32 telemetry protocol, fault/status reporting and disconnect handling. | STM32 retains charger authority. Treat networking firmware progress separately from PCB changes. [#11](https://github.com/Phil-CIA/Solar-Project-200w/issues/11). |
| BMS communications | Add the hardware interface, connector/protection and firmware protocol for the actual selected BMS. Include the link in the MCU pin/peripheral and power-domain budgets before layout. | Confirm BMS model, connector pinout, electrical levels, protocol, cable and isolation requirements before selecting parts. Define stale/missing-data and BMS fault handling; received data cannot bypass local charger protections. DEC-062, #4, #8. |

## C. Decide before freezing connector/pin assignments

| Open choice | Proposed handling |
|---|---|
| Internal feedback | Recommend internal I2C control to match the successful bench configuration; obtain approval before the schematic edit. |
| Measurement hardware | Agree which buses/currents must be measured and their error targets, then select the front-end using actual voltage, current and pin budgets. No monitor IC selected yet. |
| Debug interface | Confirm desired probe/cable and whether full JTAG is required; do not silently substitute SWD or consume assigned pins. |
| BMS communications implementation | Included in Rev2 by Phil's explicit direction. Identify the actual BMS model, electrical interface and protocol, then allocate pins and select the connector/transceiver/protection. Do not assume TTL UART, RS485, CAN or SMBus from a connector shape or product family. Define supported messages, timeout behavior and startup/charge-permission policy from actual BMS capabilities. |
| ESP32-assisted STM32 updates | Decide build-now versus later. If included, freeze transport/reset/boot/memory/recovery needs before layout. Until qualified, SWD remains recovery; charging stays inhibited during update. [#24](https://github.com/Phil-CIA/Solar-Project-200w/issues/24). |
| Other features | Explicitly choose scope for coulomb-counting/SOC, load-output/LVD, CAN/RS485/NMEA2000, persistent history/UI and outdoor packaging. None is implicitly approved. [#12](https://github.com/Phil-CIA/Solar-Project-200w/issues/12). |

## D. Work order

1. **Freeze the change list and pin/interface budget.** Identify the included BMS interface first, then resolve feedback, measurement approach, debug connector and which remaining optional interfaces need hardware now. Keep the unselected optional items explicitly deferred; BMS communications is not one of them.
2. **Resolve converter and supply design.** Work through CSA/CSB, ISET, feedback, input sensing/UVLO, startup and D4/D6 as one consistent power/control design. Use existing bench evidence; request only new measurements that change a design decision. If the peak-current root cause remains unresolved, leave that block open rather than guessing a permanent filter.
3. **Update the Rev2 schematic and BOM.** Add agreed boot/debug/telemetry/protection parts, correct polarity/net documentation, and review the complete pin/address/power budget. Separate approved changes from experimental options.
4. **Update placement and routing.** Prioritize shunt Kelvin pickup, U6 grounding and switching loops, gate returns, decoupling, protection return paths and connector clearance. Keep schematic and PCB changes paired.
5. **Close design/release checks.** Compare ERC/DRC/parity against the inherited baseline; review every new finding and disposition existing relevant warnings. Reconcile BOM, footprints, pin/polarity mapping, part ratings and library portability before generating a distinct Rev2 fabrication package.
6. **Bring up in stages after manufacture.** Verify safe states, control rails, debug, calibrated measurements, low-power regulation/protection, source recovery and then load/thermal performance. Keep new-board testing separate from this planning task.

## Fabrication and operating gates

- Do not order from the copied baseline or with peak-current/average-current protection undefined.
- Check C18 ripple/overshoot margin, L2 loss/temperature, R16 stress, MOSFET/gate timing and compensation for the selected envelope; old open items are qualification questions, not automatic replacement instructions (#10).
- Reconcile the existing F1/cold-Voc concern before the intended PV environment is claimed safe. Retain the selected Keystone 3522-2 decision unless new evidence changes it; firmware loading does not increase a fuse-holder voltage rating (#7).
- Retain remote I2C battery-temperature sensing at J8; do not assume a spare MCU ADC/NTC channel. Confirm sensor/interface implementation and actual battery/BMS limits before charging.
- Require unambiguous SET/MEAS, fault reporting, default-off/recovery behavior and relevant protection checks before increasing power. A successful low-power run does not establish rated power or outdoor readiness.

The next implementation packet should contain only the first approved schematic block and its required supporting changes, not all optional features at once.
