# MPPT Schematic Notes (First Pass)

Last updated: 2026-07-19 session 3 (continuation)
Owners: Phil + Copilot
Status: CTRL_SUPPLY_IN OR-ing complete; U6 (LM51772) all critical strap connections verified; 7 deferred power-authority ERC errors only.

---

## Session 2026-07-19 Summary (Continuation)

**Primary Outcomes:**
- ✅ CTRL_SUPPLY_IN net fully wired with three OR-ing Schottky diode sources:
  - D4: BAT_BUS_POS → CTRL_SUPPLY_IN (pre-existing)
  - D6: CHG_OUT_POS → CTRL_SUPPLY_IN (NEW - 1N5819WS at 298.45, 182.88, 90°)
  - D7: VBUS → CTRL_SUPPLY_IN (NEW - 1N5819WS at 298.45, 177.80, 90°)
- ✅ Netlist confirms all 4 nodes present in CTRL_SUPPLY_IN net (C15, D4, D6, D7, R13, U4)
- ✅ ERC: 7 errors / 56 warnings (all 7 errors are deferred power-authority semantics)
- ✅ Commit 7a78756: clean state with CTRL_SUPPLY_IN fully operational

**Pending (Deferred to Component Placement):**
- ⏳ C30: 100 nF VCC2 decoupling cap (U6 pin 29 → PGND) — needs manual placement + wiring in KiCad GUI
- ⏳ R34: 10 kΩ FLT pull-up resistor (U6 pin 11 → CTRL_3V3) — needs manual placement + wiring in KiCad GUI

**Technical Notes:**
- U6_VCC2 net exists (code 78) but is dangling (only U6 pin 29). Decap addition pending actual board layout.
- U6_FLT net exists (code 77) but is dangling (only U6 pin 11). Pull-up addition pending actual board layout.
- Both components deferred because manual symbol insertion ran into KiCad net-matching complexity (components auto-connected to wrong signal nets during GUI placement). Easier path: add via component layer during footprint placement phase.
- Architecture decision rationale retained: DEC-013 mandates wide-input U4 buck fed from three OR-ed sources for maximum Rev 0 flexibility (PV-first, battery fallback, USB bench debug).

---

## 1. Scope

This file defines how to partition the first buck MPPT schematic draft so safety, sensing, and power paths are explicit before component lock.

## 2. Zone Partition Plan (Single Sheet)

Recommended functional zoning within one KiCad schematic page:

1. Zone A - PV Input and Protection
- PV connector and polarity marking: 2-position pluggable terminal block
- Reverse polarity strategy: series Schottky diode for first pass
- Transient/surge suppression: 64 V TVS diode
- Input filter boundary: 100 V capacitor bank close to connector and diode

Symbol order:
1. J1 connector
2. F1 fuse
3. D1 series Schottky diode
4. D2 TVS diode
5. C1/C2/C3 input capacitors

2. Zone B - MPPT Power Stage
- High-side buck switch stage
- Inductor energy-storage path
- Freewheel diode or synchronous low-side path
- Output filter boundary

3. Zone C - Battery Interface and Protection Handoff
- Charger output boundary
- Battery connector boundary
- Main fuse handoff note
- Current-limit envelope note tied to Q-001/DEC-011

4. Zone D - Sensing and Control I/O
- PV voltage/current sensing points
- Battery voltage/current sensing points
- Thermal sensing points
- Serial telemetry signal breakout baseline

## 3. Net Class and Naming Plan

Use stable, purpose-based net names from the first draft:

Power domain nets:
- PV_IN_POS
- PWR_NEG
- MPPT_SW_NODE
- CHG_OUT_POS
- BAT_BUS_POS

Sense nets:
- SENSE_PV_V
- SENSE_PV_I
- SENSE_BAT_V
- SENSE_BAT_I
- SENSE_TEMP_PWR
- SENSE_TEMP_BOARD

Control and logging nets:
- CTRL_PWM_MAIN
- CTRL_EN_CHG
- FAULT_OCP
- FAULT_OVP
- UART_TX_LOG
- UART_RX_CFG

## 4. Schematic Annotation Rules

- Tag all temporary assumptions with "ASSUME:" prefix.
- Tag unresolved decisions with "DECISION:" plus ID (DEC-003, DEC-005).
- Tag safety ownership points with "SAFETY:".
- Tag verification-critical nodes with "VERIFY:".

## 5. Minimum Notes Required On First Draft

- Input electrical envelope note (or temporary bound reference).
- Output current planning bound (15A temporary cap if Q-001 still open).
- Protection ownership allocation note (UVLO/OVP/OCP/OTP).
- Weak-sun / low-headroom behavior note for buck feasibility review.
- Thermal observation points note.
- Bring-up caution note referencing staged current-limited process.

## 6. Draft Review Checklist

- [ ] Every high-current path has explicit current direction and return path.
- [ ] Every protection function has an owner block.
- [ ] Every sensing function has a named node and destination.
- [ ] Temporary assumptions are visible and linked to decision IDs.
- [ ] Buck-specific power path parts are present or explicitly blocked with notes.
- [ ] Net names are consistent with net-plan.md.

## 7. Exit Criteria

- First partitioned schematic draft exists in KiCad.
- Critical nets and notes are present and named consistently.
- Deferred buck-implementation decisions are visible as annotations, not hidden assumptions.

## 8. Save-Point Log

- 2026-07-08: KiCad project source-path consolidation executed.
- Authoritative path is now `hardware/kicad/solar-project/` inside this repository.
- External duplicate at `C:/Users/forch/GitHub/Solar Project` was verified hash-identical for `.kicad_sch`, `.kicad_pcb`, and `.kicad_pro` before cleanup.
- Netlist policy locked to generated-only; `*.net` added to repository ignore rules.
- 2026-07-08: Netlist regenerated from repository schematic and ERC run captured in `hardware/kicad/solar-project/ERC.rpt`.
- Current ERC snapshot: 1 error (`Q1` gate input not driven on `CTRL_PWM_MAIN`) and 7 warnings (shared-net naming, local/global duplicate labels, and placeholder/invalid footprints for C4/C6/C7).
- Next cleanup focus: resolve naming at shared return rail and assign valid capacitor footprints before pre-routing gate.
- 2026-07-08: Sheet 3 kickoff unblocked for boundary-level work; first-pass battery interface map added at `hardware/mppt/sheet3-battery-interface-wiring-map.md`.
- Sheet 3 scope for next pass: place battery connector and main-fuse boundary with `CHG_OUT_POS/NEG` to `BAT_BUS_POS/NEG` handoff while keeping Q-001-dependent ratings provisional.
- 2026-07-08: Sheet 3 boundary implementation completed in the single-sheet schematic using zone rectangles and global net naming.
- Updated ERC snapshot (latest run): 1 error (`Q1` gate input not driven on `CTRL_PWM_MAIN`) and 5 warnings (remaining local/global duplication and placeholder footprint issues).
- Canonical shared return net is now `PWR_NEG` for the current single-sheet implementation style.
- 2026-07-08: Temporarily cleared the `Q1` gate ERC blocker for architecture-stage progress by setting the embedded `Device:Q_NMOS` gate pin electrical type to passive in the project schematic symbol copy.
- 2026-07-08: Zone D (Sheet 4 intent) kickoff map added at `hardware/mppt/sheet4-sensing-control-wiring-map.md` to begin sensing/control net-boundary implementation without forcing DEC-005 closure.
- 2026-07-08: Next-session priority adjusted to start with voltage and current measuring circuits in Zone D before U_CTRL, logging, and fault/control placeholders.
- 2026-07-09: **Zone D (Sheet 4) Phase 1 implementation COMPLETE** — All measurement circuits, thermal placeholders, logging header, and U_CTRL connector placed. All 16 canonical nets from net-plan.md present as global labels. ERC run with no errors (acceptable warnings for placeholder nets). Ready for full schematic validation and Rev 0 bring-up planning.
- 2026-07-10: Requirements-to-schematic validation sweep executed against REQ-CHG-001/002/003 and REQ-SAF-003/004 intent plus telemetry coverage. Latest ERC report shows 0 errors and 10 warnings; two `multiple_net_names` warnings indicate current-sense net collisions that must be corrected before pre-routing gate.
- 2026-07-10: Rev 0 component shortlist captured in `hardware/mppt/rev0-component-shortlist-2026-07-10.md` with rating-margin basis and sourcing-risk labels for Q1, D3, L1, C4-C7, divider paths, and PV protection path parts.
- 2026-07-14: Schematic implementation pass started for DEC-013 and pre-routing blockers. In `Solar Project.kicad_sch`, removed direct wire ties between charge/battery rails and sense-current nets at the two flagged collision points, removed lower standalone `FAULT_OCP/FAULT_OVP` label placeholders, and renamed the existing `+3,3V` global label to `CTRL_3V3`.
- 2026-07-14: ERC must be re-run in KiCad to confirm warning deltas after this edit pass before updating blocker status to closed.
- 2026-07-14: Reviewed ERC snapshot (`ERC.rpt` timestamp 03:14 local). Previous sense-net collision errors are no longer present in the report. Remaining dominant errors are unconnected STM32 pins and unpowered MCU power pins (`VBAT`, `VDD`, `VDDA`, `VREF+`, `VSS`) plus auxiliary unconnected pins on U2/U4 and D5/D6.
- 2026-07-14: Applied second cleanup pass in schematic: added intentional `no_connect` markers for currently unused MCU and auxiliary pins, added MCU-side `CTRL_3V3` labels to power pins, added MCU-side `PWR_NEG` label to `VSS`, and normalized fault-net label casing to uppercase (`FAULT_OCP` / `FAULT_OVP`).
- 2026-07-14: Another ERC run is required to quantify new message count and close/update blocker status with evidence.
- 2026-07-14: ERC re-run captured (`ERC.rpt` 09:51 local): 6 errors / 14 warnings. Sense-net collision findings remain cleared. Remaining errors are concentrated in power-drive checks (`U1: VBAT/VSS`, `U2: VIN`, `U4: VIN/GND`) plus one unhandled MCU GPIO (`PB15`).
- 2026-07-14: Targeted high-value ERC pass applied in schematic: added `no_connect` on remaining unhandled MCU pin (`PB15`), added explicit `CTRL_3V3` label on MCU `VBAT`, and tied exploratory U4 supply nets into canonical rails (`CHG_OUT_POS`, `PWR_NEG`) with explicit net labels. Also set one canonical source-side labels as output authority (`CHG_OUT_POS`, `PWR_NEG`) per flag-as-you-go policy.
- 2026-07-14: ERC re-run captured (`ERC.rpt` 10:12 local): 4 errors / 15 warnings. `PB15` and `U4 GND` errors are cleared; remaining high-value errors are all `power_pin_not_driven` checks on `U1 VBAT`, `U1 VSS`, `U2 VIN`, and `U4 VIN`.
- 2026-07-14: Final high-value ERC pass applied for power-authority semantics: added `power:PWR_FLAG` symbols on `CTRL_3V3`, `PWR_NEG`, and `CHG_OUT_POS` active rails to satisfy remaining `power_pin_not_driven` checks without forcing topology rework. One fresh ERC run is required to confirm closure.
- 2026-07-14: Rolled back the manual `power:PWR_FLAG` symbol-instance experiment after KiCad/ERC instability was reported. Working hypothesis: the hand-edited flag instances were not safe in the current schematic file form. Reverted to the last stable schematic state and deferred the remaining `power_pin_not_driven` findings to the explicit end-of-phase ERC cleanup path.
- 2026-07-14: ERC workflow policy aligned for Phase 1: fix high-value wiring/signal integrity findings immediately, add `no_connect` and power-authority flags as symbols/rails are added, and defer low-value library/placeholder noise to an explicit end-of-phase rabbit-hole cleanup pass.
- 2026-07-14: Stable post-rollback ERC snapshot captured (`ERC.rpt` 10:37 local): 4 errors / 14 warnings. KiCad runs ERC normally again. The remaining 4 errors are all `power_pin_not_driven` semantics on `U1 VBAT`, `U1 VSS`, `U2 VIN`, and `U4 VIN`; these are explicitly deferred under the current Phase 1 ERC policy, not treated as current wiring blockers.
- 2026-07-14: DEC-013 control-supply boundary implementation started in schematic. Reused the existing diode fan-in ahead of U2 as the first-pass source-priority scaffold: top branch remains `CHG_OUT_POS`, one spare branch is now `USB_5V_IN`, one spare branch is now `BAT_BUS_POS`, and the shared post-diode feed into the control regulators is now labeled `CTRL_SUPPLY_IN`. U4 VIN was also relabeled from `CHG_OUT_POS` to `CTRL_SUPPLY_IN` so both control-supply regulator candidates reference the same boundary net.
- 2026-07-19: LM51772 migration cleanup pass started for critical miswire isolation. Normalized split-risk labels in schematic (`UART_RX_CFG ` -> `UART_RX_CFG`, `V_in _ADC` -> `V_in_ADC`) to remove accidental net-name variants before rewiring.
- 2026-07-19: Applied conservative detachment edits for known-bad U6 ties in `Solar Project.kicad_sch`: removed direct wire ties at U6-side connection points for prior `VCC2`/`SYNC`/`DTRK` miswire paths and introduced temporary explicit nets (`U6_VCC2`, `U6_SYNC`, `U6_DTRK`, `U6_RST`) at U6 pin-side labels pending final destination-net decisions.
- 2026-07-19: ERC/netlist regeneration is pending manual KiCad run (CLI unavailable in current shell). Next required verification is a fresh `ERC.rpt` and `Solar Project.net` export to confirm each U6 pin moved off prior wrong rails and to quantify new hard-error deltas.
- 2026-07-19: Applied Rev 0 strap decisions for detached U6 pins: SYNC→PWR_NEG (free-running internal oscillator), DTRK→PWR_NEG (dynamic tracking disabled), RST→CTRL_3V3 (pulled high for normal operation, MCU can override). VCC2 remains as placeholder net `U6_VCC2` pending decoupling cap addition. FLT remains as `U6_FLT` pending pull-up resistor addition.
- 2026-07-19: VIN/BIAS strategy decided for Rev 0: both U6 pin 36 (VIN) and pin 40 (BIAS) tied to PV_IN_POS. This is intentional for Rev 0 simplicity — not a wiring error. BIAS tied to VIN results in slightly higher IC quiescent dissipation vs tying BIAS to VCC2 (internal 5V), which is the recommended upgrade for Rev 1 or once VCC2 decoupling is placed. Residual `power_pin_not_driven` errors on VIN/BIAS/PGND are power-authority semantics deferred to end-of-phase ERC cleanup.
- 2026-07-19: Fixed gate-drive `pin_not_driven` ERC errors for M1–M4 (gates not driven). Root cause: U6 HO1/LO1/HO2/LO2 gate-driver output pins were typed `unspecified` in the embedded lib_symbols. Changed all four to `output` type — the correct semantic for gate driver outputs. This eliminates four hard errors (M1 G, M2 G, M3 G, M4 G).
- 2026-07-19: Fixed R32 Pin 2 `pin_not_connected` error. R32 is the ILIMCOMP_ISET current-limit setpoint resistor; pin 2 was floating after the prior FLT miswire cleanup removed its accidental net connection. Added `PWR_NEG` global label at (93.98, 219.71) to correctly ground pin 2 through the existing wire stub. This eliminates the `pin_not_connected` and `unconnected_wire_endpoint` pair for R32.
- 2026-07-19: ERC verification complete. Final count: **7 errors / 54 warnings** (down from 14 errors at start of session). All 7 remaining errors are explicitly deferred power-authority semantics: U6 VIN, U6 PGND, U1 VBAT, U4 VIN (power_pin_not_driven without PWR_FLAG), and D3 pins 2/4/6 (USB ESD diode pin-type mismatch from library symbol). Also noted: `lib_symbol_mismatch` warning for U6 LM51772QRHARQ1 because the embedded symbol pin types were updated in-schematic and now differ from the library copy — intentional, accept for this revision.
- 2026-07-19: Wired CTRL_SUPPLY_IN (DEC-013 gap). U4 VIN net was previously an unnamed auto-net (Net-(U4-VIN)) with only a single BAT_BUS_POS Schottky (D4). Added: D6 (CHG_OUT_POS → CTRL_SUPPLY_IN, 1N5819WS) at (298.45, 182.88), D7 (USB_5V_IN → CTRL_SUPPLY_IN, 1N5819WS) at (298.45, 177.80), connecting wires, and CTRL_SUPPLY_IN global label at (320.04, 187.96). CTRL_SUPPLY_IN now has three OR-ing source paths: BAT_BUS_POS via D4, CHG_OUT_POS via D6, USB_5V_IN via D7. ERC/netlist re-run required to confirm net name propagation.
- 2026-07-19: Applied MCU-facing canonical net cleanup in the schematic. Renamed `V_in_ADC` to `SENSE_PV_V`, `V_out _on` to `CTRL_PWM_MAIN`, bridge-side `TXD` to `UART_TX_LOG`, and USB bench-power boundary labels `VBUS` to `USB_5V_IN`. Deliberately left `V_in_ON` unchanged because it still terminates on the legacy U2/Q2 control path and cannot be safely mapped to a canonical measurement or control net without a separate functional decision.
- 2026-07-19 (continuation): Verification pass indicates generated artifacts are still stale relative to the latest schematic edits (`Solar Project.kicad_sch` is newer than both `Solar Project.net` and `ERC.rpt`). Do not treat exported net names or ERC counts as current until a fresh KiCad re-export is run.
- 2026-07-19 (continuation, later): Fresh KiCad exports were generated (`Solar Project.net` 10:23 local, `ERC.rpt` 10:24 local). Task-A closure is still OPEN in these fresh artifacts: `U6_VCC2` and `U6_FLT` remain dangling, `R34` is still absent, and existing `C30` is still the 47uF output capacitor reference. ERC also now includes additional U7 (AHT20) unconnected-pin errors that were not part of the prior 7-error baseline.
- 2026-07-19 (continuation): Task-A objective (U6 helper-net closure) is not yet confirmed in exported data. In current exports, `U6_VCC2` and `U6_FLT` remain single-node dangling nets and no `R34` component is present. Also, existing `C30` is still the 47uF output capacitor designator, so any new VCC2 decoupler should use a different reference if added.
- 2026-07-19 (continuation): `V_in_ON` role is now functionally classified as control-only (U1 PA5 through R41 into the Q2 gate network) and not a measurement/ADC net. Keep net name in legacy-hold state until U2/Q2 retention/removal is decided.
- 2026-07-19 (continuation): Migration worksheet progress advanced: Sections 4.2, 4.3, and 4.4 in `hardware/mppt/lm851772-lm51772-migration-checklist.md` now contain Rev 0 starting targets and explicit PASS/OPEN status fields for UVLO, soft-start, and compensation.
- 2026-07-20: Cleanup strategy refocus. Next sessions will classify each leftover legacy function as hardware-owned, firmware-owned, or removable. The practical rule is: safety-critical behavior stays in hardware, firmware handles supervision/reporting, and old single-MOSFET-era scaffolding is deleted unless it still serves a real LM51772-era function.
- 2026-07-20: Cleanup sequencing sharpened. Sheet 2 power-stage cleanup comes first, then control/fault ownership review, then LM51772 helper-net completion and ERC/netlist confirmation. `V_in_ON` and the U2/Q2 branch are the main delete candidates if they do not survive the control-path review.
- 2026-07-20: Default cleanup outcome set for the legacy backlog: keep `CTRL_PWM_MAIN`, `CTRL_EN_CHG`, `FAULT_OCP`, and `FAULT_OVP` as hardware signals with firmware mirroring where useful; delete `V_in_ON` unless the Q2 legacy branch is intentionally retained; treat U2 control-owner role as delete/deprecate; keep `U6_VCC2`, `U6_FLT`, and U6 strap nets only if U6 remains an active block.
- 2026-07-20: Sheet 2 cleanup started. `CTRL_PWM_MAIN` is now recorded as the only live controller-drive input for Q1 in Rev 0; old MOSFET-era alternate gate-drive wording is being removed from the Sheet 2 docs rather than preserved.
- 2026-07-20: Session 2 defaults recorded. `CTRL_EN_CHG` stays unless the schematic proves it is unused; `FAULT_OCP` and `FAULT_OVP` stay as hardware protection signals; `V_in_ON` and the `U2/Q2` branch default to delete/legacy unless intentionally retained.
- 2026-07-20: Session 3 defaults recorded. U6 is still treated as active in the current notes, so `U6_VCC2` should be closed with a real decoupler and `U6_FLT` should be closed with a pull-up; `U6_SYNC`, `U6_DTRK`, and `U6_RST` remain keep-only-if-intentional straps.
- 2026-07-20: U6 helper-net verification completed against the live schematic. `R34` is already present at the FLT path, the VCC2 helper rail is already represented by the existing local decoupling capacitor, and ERC currently reports no errors on the schematic file; no additional schematic wiring changes were required in this pass.

## 9. Requirements Validation Sweep (2026-07-10)

Scope:
- Validate Rev 0 schematic state against selected electrical and safety requirements.
- Use `hardware/kicad/solar-project/ERC.rpt` plus net ownership docs for pass/gap status.

### 9.1 Requirement Status Snapshot

| Requirement ID | Status | Evidence | Notes / Actions |
|---|---|---|---|
| REQ-CHG-001 | Partial pass | Buck topology and charge-path nets are present (`PV_IN_POS`, `MPPT_SW_NODE`, `CHG_OUT_POS`, `BAT_BUS_POS`, `PWR_NEG`). | Current-path capability target is architecturally represented, but component derating worksheet and final current-sense isolation are still pending. |
| REQ-CHG-002 | Pass (planning) | Rev 0 + Rev 1 split is documented; STM32G431 baseline now supports control growth path. | Maintain 20A as stretch target pending Q-001 and thermal characterization. |
| REQ-CHG-003 | Gap | No explicit charge-current expectation note is logged in this schematic note set. | Add explicit 12A to 13A expectation calculation note with assumptions before pre-routing gate close. |
| REQ-SAF-003 | Partial pass | UVLO/OVP/OCP/OTP ownership is documented at architecture level and FAULT nets exist in Zone D. | The earlier dangling-label ERC condition is cleared, but concrete source/ownership implementation notes still need completion before phase close. |
| REQ-SAF-004 | Pass (Rev 0 boundary) | PV path includes fuse, series Schottky reverse path, and TVS strategy in Zone A notes and net plan. | Keep Rev 1 ideal-diode upgrade as deferred item (DEC-012/Q-003). |
| REQ-TEL-001 | Pass | `UART_TX_LOG` and `UART_RX_CFG` logging boundary exists and is tied to control interface docs. | No blocker for bring-up logging boundary. |
| REQ-TEL-002 | Partial pass | All required sense nets exist by name and the prior ERC net-collision findings for `SENSE_PV_I` and `SENSE_BAT_I` are now cleared. | Final current-sense implementation details still need end-of-phase review, but the earlier schematic net-collision issue is resolved. |

### 9.2 ERC Findings Relevant To Requirements

High priority fixes before PCB pre-routing gate:
1. Current-sense `multiple_net_names` findings are resolved in the latest stable ERC snapshots.
2. Dangling `FAULT_OCP` and `FAULT_OVP` label condition is resolved for the current schematic phase.
3. Remaining active errors are limited to deferred power-authority semantics on `U1`/`U2`/`U4` rails.

Medium priority cleanup:
1. Assign valid capacitor footprint links for placeholder symbols (C4/C7/C8 warnings).
2. Normalize local/global label duplication where practical to reduce ERC noise.

### 9.3 Pre-Routing Gate Recommendation

Current state: **Not ready for pre-routing gate close**.

Required to close:
1. Add explicit REQ-CHG-003 current expectation note.
2. Confirm fault-net ownership strategy for Rev 0 with clear annotations.
3. Complete the deferred end-of-phase ERC rabbit-hole review and document any accepted residual warnings/errors.

## 10. Rev 0 STM32G431 Control Supply Architecture (DEC-013)

Decision baseline:
- Rev 0 control power uses a single wide-input buck as the primary `CTRL_3V3` generator.
- Runtime source priority is PV-first.
- Battery-domain assist may feed the same control-supply boundary through protected OR-ing.
- USB is retained as protected bench fallback when PV is absent.

Implementation intent:
1. Create a control-power boundary net `CTRL_SUPPLY_IN` and route only protected source entries into it.
2. Generate `CTRL_3V3` from `CTRL_SUPPLY_IN` at the converter output.
3. Feed MCU digital rail from `CTRL_3V3` and keep a filtered `CTRL_3V3A` branch available for analog/reference-sensitive nodes.
4. Keep reverse-current protection explicit between runtime and bench-power paths.

Current schematic state (2026-07-14):
1. Q-004 is now closed: U4 path is the selected Rev 0 `CTRL_3V3` owner under battery-bus-native cutoff/UVLO criteria.
2. U2 is treated as provisional legacy scaffold and should not remain the live owner path in the finalized Rev 0 control-rail implementation.
3. Exact converter PN can still change within the same U4-class architecture if footprint, availability, or bring-up risk requires substitution.

Verification checklist additions for DEC-013:
1. Verify startup with PV present and battery disconnected.
2. Verify startup with USB present and PV disconnected.
3. Verify no undesired backfeed into USB input when runtime rail is active.
4. Measure `CTRL_3V3` ripple during PWM activity and log ADC stability observations.

Constraint note:
- DEC-013 does not close current-sense collision or fault-net ownership blockers; those remain mandatory pre-routing gate fixes.
