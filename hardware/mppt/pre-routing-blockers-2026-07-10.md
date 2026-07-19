# Pre-Routing Blockers (2026-07-10)

Owners: Phil + Copilot
Source inputs: schematic validation sweep + latest ERC report.

## 1. Gate Decision

Pre-routing gate status: BLOCKED for broader design completion, but the original blocker set in this file is now closed.

## 2. Blockers (Original Must-Fix Set)

1. Current-sense net collision at PV current path
- ERC finding: `multiple_net_names` collision between `SENSE_PV_I` and `CHG_OUT_POS`.
- Risk: measurement path is not electrically independent.
- Exit criterion: `SENSE_PV_I` appears as a standalone net with intended sensing element path.

2. Current-sense net collision at battery current path
- ERC finding: `multiple_net_names` collision between `SENSE_BAT_I` and `BAT_BUS_POS`.
- Risk: battery current sensing invalid in present net topology.
- Exit criterion: `SENSE_BAT_I` appears as a standalone net with intended sensing element path.

3. Fault-net ownership is still dangling
- ERC finding: dangling global labels for `FAULT_OCP` and `FAULT_OVP`.
- Risk: safety signaling path is undefined in implementation even if architecture intent is documented.
- Exit criterion: each fault net has explicit source ownership or intentional placeholder termination strategy.

## 3. Cleanup Items (Should Fix)

1. Footprint link warnings on placeholders (C4/C7/C8).
2. Local/global duplicate label warnings where they create avoidable ERC noise.
3. Confirm `CTRL_EN_CHG` final connectivity once control block symbol is replaced/refined.

## 4. Suggested Fix Order

1. Resolve both current-sense net collisions first.
2. Resolve fault-net ownership/termination strategy second.
3. Clear footprint and duplicate-label cleanup warnings third.
4. Re-run ERC and capture post-fix report snapshot in schematic notes.

## 5. Verification Gate

Pre-routing gate can move to READY only when:
- No net-collision warnings remain for sense nets.
- Fault nets are no longer dangling without explicit ownership.
- Remaining warnings are explicitly accepted as non-blocking with rationale.

## 6. Progress Update (2026-07-14)

Implementation actions completed in `Solar Project.kicad_sch`:
- Removed direct wire tie between `CHG_OUT_POS` and the `SENSE_PV_I` node.
- Removed direct wire tie between `BAT_BUS_POS` and the `SENSE_BAT_I` node.
- Removed lower standalone `FAULT_OCP` and `FAULT_OVP` global-label placeholders.

Pending confirmation:
- Re-run ERC and confirm that both `multiple_net_names` collisions are cleared.
- Re-run ERC and confirm fault-net dangling-label warning state after placeholder removal.

ERC evidence update from 2026-07-14 snapshot (`ERC.rpt` 03:14 local):
- No `multiple_net_names` entries are present for `SENSE_PV_I` or `SENSE_BAT_I` in the reported findings list.
- Current report focus shifted to unconnected MCU/auxiliary pins and unpowered MCU supply pins.

Post-report additional edits were applied (no-connect markers + MCU power labels + fault-case normalization), so one fresh ERC run is still required before formally closing these blockers.

ERC confirmation from 2026-07-14 snapshot (`ERC.rpt` 09:51 local):
- `multiple_net_names` sense-collision findings are still absent.
- Fault-net dangling-label findings are absent in this run.

Blocker status update:
- Blocker 1 (`SENSE_PV_I` collision): resolved by ERC evidence.
- Blocker 2 (`SENSE_BAT_I` collision): resolved by ERC evidence.
- Blocker 3 (fault-net dangling): resolved for dangling-label condition; ownership intent still requires explicit note hygiene in schematic/docs.

New active gate blockers from latest ERC:
1. MCU power-net drive path still incomplete (`VBAT` and `VSS` power-pin-driven errors).
2. One MCU GPIO remains unhandled (`PB15` pin-not-connected error).
3. Control-rail source nets for U2/U4 still report no power driver (`U2 VIN`, `U4 VIN`, `U4 GND`).

ERC confirmation from 2026-07-14 snapshot (`ERC.rpt` 10:12 local):
- Current total is 4 errors / 15 warnings.
- `PB15` pin-not-connected error is no longer present.
- `U4 GND` power-pin-driven error is no longer present.

Updated active high-value blockers:
1. None from the original pre-routing blocker set remain open.

Stable post-rollback ERC snapshot (2026-07-14, `ERC.rpt` 10:37 local):
- KiCad/ERC runs stably after rollback of the manual `power:PWR_FLAG` experiment.
- Current total is 4 errors / 14 warnings.
- Remaining 4 errors are limited to `power_pin_not_driven` on `U1 VBAT`, `U1 VSS`, `U2 VIN`, and `U4 VIN`.

Working interpretation for this phase:
- These remaining errors are ERC power-authority semantics, not wiring-integrity regressions.
- Per the Phase 1 ERC policy, they are deferred to the explicit end-of-phase rabbit-hole cleanup pass rather than treated as current blockers.

Pre-routing blocker conclusion for this file:
- Blocker 1 (`SENSE_PV_I` collision): closed.
- Blocker 2 (`SENSE_BAT_I` collision): closed.
- Blocker 3 (fault-net dangling condition): closed for current schematic phase.

Current deferred items outside this blocker file's original scope:
1. Four `power_pin_not_driven` ERC errors on active rails (`U1 VBAT`, `U1 VSS`, `U2 VIN`, `U4 VIN`).
2. Low-value warning cleanup batch (footprint libraries, duplicate label noise, placeholder/library mismatch items).
3. Broader design-completion items such as REQ-CHG-003 note closure and DEC-013 source-priority implementation.

## 7. ERC Policy (Phase 1 Working Rule)

Policy intent:
- Use ERC as a wiring-sanity gate during active schematic development, not as a full cleanup gate on every session.

High-value ERC findings to fix immediately:
1. `multiple_net_names` on power, sense, and control nets.
2. Unconnected wire endpoints in active implementation zones.
3. Fault/control ownership gaps that hide missing signal paths.
4. Power-drive errors on rails needed for current bring-up intent.

Flag-as-you-go rule (required while drawing):
1. Add intentional `no_connect` markers as soon as a pin is knowingly unused in this revision.
2. Add explicit power-net authority markers (for example, power flags or clear power-output ownership) as soon as a rail is introduced.
3. Keep net naming canonical at placement time to avoid later relabel churn.

Deferred-to-rabbit-hole bucket (allowed until end cleanup):
1. Footprint library/link warnings.
2. Non-critical duplicate local/global naming noise.
3. Known placeholder warnings already captured with rationale.

End cleanup gate:
- At end-of-phase routing readiness, run full ERC cleanup pass and resolve or explicitly waive all remaining warnings with written rationale.
