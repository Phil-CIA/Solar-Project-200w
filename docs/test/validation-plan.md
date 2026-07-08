# Solar Project 200W - Validation Plan (Phase 0 Baseline)

Last updated: 2026-07-06
Purpose: Define the first executable validation set tied to requirement IDs and architecture interfaces.

## 1. Validation Scope

This plan covers the first six high-value checks that can be run as planning transitions to implementation exploration.

## 2. Test Cases

| Test ID | Requirement IDs | Architecture Link | Test Method | Pass Criteria | Evidence |
|---|---|---|---|---|---|
| VAL-001 | REQ-PV-001 | 3.1 PV Input Stage | Review selected panel nameplate and power budget envelope. | PV class aligns to 180W to 220W envelope. | Datasheet snapshot + worksheet entry |
| VAL-002 | REQ-CHG-003 | 3.2 MPPT Charger Stage | Calculate expected peak charge current with stated assumptions. | Estimated current falls in documented 12A to 13A range for 200W baseline. | Calculation sheet and assumptions block |
| VAL-003 | REQ-SAF-001, REQ-SAF-002 | 3.3/3.4 Protection Boundaries | Verify planned fuse locations and branch protection allocation in architecture/schematic notes. | Main battery fuse near source and branch protection defined for major paths. | Marked architecture notes or schematic annotation |
| VAL-004 | REQ-TEL-001, REQ-TEL-002 | 3.6 Monitoring/Telemetry/Control | Verify signal list includes PV V/I, battery V/I, temperatures and local serial logging path. | All minimum observability signals mapped with logging path. | Signal map table |
| VAL-005 | REQ-SAF-006 | 3.6 + Bring-up Procedure | Execute staged current-limited sequence from bring-up checklist at low-risk boundary. | Each stage completed with no unresolved safety stop events. | Completed checklist with timestamps |
| VAL-006 | REQ-VAL-001, REQ-VAL-002 | Phase gate traceability | Audit requirement-to-test mapping and deferred decision owner/date coverage. | No requirement without planned verification; deferred decisions have owner + target date. | Traceability matrix review note |

## 3. Execution Order

1. VAL-001 and VAL-002 (baseline electrical sanity)
2. VAL-003 and VAL-004 (protection and observability readiness)
3. VAL-005 (controlled bring-up execution)
4. VAL-006 (phase gate traceability audit)

## 4. Preconditions

- Requirements, architecture, and decisions log are up to date.
- Open items Q-001 and Q-002 are reviewed before any high-current activity.
- Instrumentation limits are known for selected test depth.

## 5. Closeout Criteria

- All six validation tests have recorded result status.
- Any failures produce decision-log entries with owner and target date.
- Phase transition recommendation is documented in the handoff.
