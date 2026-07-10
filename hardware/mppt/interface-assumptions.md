# MPPT Interface Assumptions (Working Draft)

Last updated: 2026-07-10
Status: temporary working bounds for rapid Phase 1 progress.

## 1. Purpose

Track explicit assumptions used during MPPT exploration so decisions and schematic changes remain traceable.

## 2. Current Working Bounds

| Interface | Working Assumption | Status | Notes |
|---|---|---|---|
| PV -> MPPT input | Two Renogy 100W N-Type panels in series form the Phase 1 baseline input. | Baseline selected | 2S envelope: 39.94Vmp / 5.01A Imp / 45.58Voc / 5.31A Isc. Controller and switch voltage class must include adequate cold-Voc margin. |
| MPPT -> Battery | Use conservative 15A continuous planning bound until BMS limits are confirmed. | Temporary | Tied to Q-001 and fallback policy in phase-1-kickoff. |
| Control and telemetry owner | Use STM32G431 as Rev 0 baseline MCU for control and logging interface ownership. | Baseline selected | DEC-005 closed for Rev 0. Final package/pin lock still requires variant and routing checks. |
| Protection layer | Include UVLO/OVP/OCP/OTP allocation at architecture level before part lock. | Required | Tied to REQ-SAF-003 and DEC-007. |
| Telemetry baseline | Include PV V/I, battery V/I, thermal channels with serial logging path. | Required | Tied to REQ-TEL-001/002 and DEC-006. |

Revision flexibility rule:
- MCU selection is intentionally re-openable each hardware revision for prototype learning and tradeoff optimization.

## 3. Open Closures Needed

1. Close Q-001 with datasheet-backed BMS continuous and peak current limits.
2. Close Q-002 with bench instrumentation capability snapshot.
3. Close DEC-003 by confirming the controller voltage class and final buck implementation boundary.

## 4. Update Rule

Any time an assumption changes, update this file and add a dated note in docs/decisions-log.md.
