# Solar Project - Validation Plan

Last updated: 2026-09-09
Purpose: Define the first executable validation set tied to requirement IDs and architecture interfaces.

## 1. Validation Scope

This plan covers the high-value checks that can be run as planning transitions to implementation exploration.

## 2. Test Cases

| Test ID | Requirement IDs | Architecture Link | Test Method | Pass Criteria | Evidence |
|---|---|---|---|---|---|
| VAL-001 | REQ-PV-001 | 3.1 PV Input Stage | Review selected panel nameplate, cold `Voc`, `Isc`, protection derating, and operating-power control. | Hardware tolerates the full connected-array envelope and control limits PV operating power to no more than 150W. | Datasheet snapshot + worksheet entry + limit-test record |
| VAL-002 | REQ-CHG-001, REQ-CHG-002, REQ-CHG-003 | 3.2 MPPT Charger Stage | Calculate expected current, then operate across the battery range at rated power and the defined thermal point. | 125W continuous output is sustained; PV operating power remains at or below 150W; charge current and hardware protection operate within documented tolerances. | Calculation, thermal record, and captured power/current data |
| VAL-003 | REQ-SAF-001, REQ-SAF-002 | 3.3/3.4 Protection Boundaries | Verify planned fuse locations and branch protection allocation in architecture/schematic notes. | Main battery fuse near source and branch protection defined for major paths. | Marked architecture notes or schematic annotation |
| VAL-004 | REQ-TEL-001, REQ-TEL-002 | 3.6 Monitoring/Telemetry/Control | Verify signal list includes PV V/I, battery V/I, temperatures and local serial logging path. | All minimum observability signals mapped with logging path. | Signal map table |
| VAL-005 | REQ-SAF-006 | 3.6 + Bring-up Procedure | Execute staged current-limited sequence from bring-up checklist at low-risk boundary. | Each stage completed with no unresolved safety stop events. | Completed checklist with timestamps |
| VAL-006 | REQ-VAL-001, REQ-VAL-002 | Phase gate traceability | Audit requirement-to-test mapping and deferred decision owner/date coverage. | No requirement without planned verification; deferred decisions have owner + target date. | Traceability matrix review note |
| VAL-007 | REQ-CHG-004 | 3.1/3.2 Startup Path | Capture battery qualification, PV qualification, `EN_UVLO`, `SS_ATRK`, output voltage, and current during strong- and weak-source startup. | Strong source starts through a controlled ramp; weak source causes demand reduction and delayed retry without sustained UVLO packet bursts. | Oscilloscope captures + firmware log |
| VAL-008 | REQ-CHG-005 | 3.2 MPPT Charger Stage | Verify fitted output capacitance and ESR using startup, load-transient, and loop-stability testing. | Capacitance is sufficient for transient and stability targets without causing startup collapse; Rev 0 capacitance is not copied without evidence. | BOM values + transient captures + stability evidence |

## 3. Execution Order

1. VAL-001 and VAL-002 (baseline electrical sanity and rating)
2. VAL-003 and VAL-004 (protection and observability readiness)
3. VAL-007 and VAL-008 (startup and output-capacitor qualification)
4. VAL-005 (controlled bring-up execution)
5. VAL-006 (phase gate traceability audit)

## 4. Preconditions

- Requirements, architecture, and decisions log are up to date.
- Open items Q-001 and Q-002 are reviewed before any high-current activity.
- Instrumentation limits are known for selected test depth.

## 5. Closeout Criteria

- All eight validation tests have recorded result status.
- Any failures produce decision-log entries with owner and target date.
- Phase transition recommendation is documented in the handoff.
