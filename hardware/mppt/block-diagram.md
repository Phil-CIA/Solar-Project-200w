# MPPT Block Diagram (First Pass)

Last updated: 2026-07-06
Owners: Phil + Copilot
Status: exploration baseline for Phase 1 (not a part-lock design).

## 1. Purpose

Provide a block-level MPPT power and control blueprint that is immediately usable for schematic exploration and requirement traceability.

## 2. Functional Flow

PV Input -> Input Protection and Filtering -> MPPT Power Stage -> Output Filtering -> Battery Bus Interface

Control/Sensing Path:
- PV sensing (V/I)
- Battery sensing (V/I)
- Thermal sensing
- Control logic and telemetry logging path

## 3. Block Definitions

### 3.1 PV Input Boundary

Responsibilities:
- Accept the selected two-panel series PV source baseline.
- Define electrical entry point for protections.

Inputs:
- PV+ / PV-

Outputs:
- Protected and filtered input rail to MPPT stage.

Requirement links:
- REQ-PV-001, REQ-PV-002, REQ-SAF-004

Decision links:
- DEC-004 (decided series baseline), DEC-011 (temporary charge-path bound policy)

### 3.2 Input Protection and Filtering

Responsibilities:
- Reverse polarity strategy at PV boundary.
- Transient suppression and input filtering.

Inputs:
- Raw PV rail

Outputs:
- Conditioned rail to power conversion stage.

Requirement links:
- REQ-SAF-004, REQ-SAF-003

Decision links:
- DEC-007, DEC-011

### 3.3 MPPT Power Conversion Stage

Responsibilities:
- Track operating point and regulate conversion into battery charging path.
- Support 15A continuous planning path with growth toward 20A class as feasible.

Inputs:
- Conditioned PV rail
- Control commands

Outputs:
- Regulated charger output to battery interface.

Requirement links:
- REQ-CHG-001, REQ-CHG-002, REQ-CHG-003

Decision links:
- DEC-003 (open), DEC-005 (open), DEC-011

### 3.4 Output Filtering and Current Control Boundary

Responsibilities:
- Smooth output current/voltage delivered to battery interface.
- Establish boundary for overcurrent and stability checks.

Inputs:
- MPPT stage output

Outputs:
- Charger output rail to battery domain.

Requirement links:
- REQ-CHG-001, REQ-SAF-003

Decision links:
- DEC-003, Q-001

### 3.5 Battery Bus Interface

Responsibilities:
- Define charger-to-battery electrical interface and fuse boundary handoff.
- Respect BMS-limited operating envelope.

Inputs:
- Charger output rail

Outputs:
- Battery bus integration point.

Requirement links:
- REQ-BAT-001, REQ-BAT-002, REQ-SAF-001

Decision links:
- Q-001, DEC-011

### 3.6 Sensing and Control Layer

Responsibilities:
- Acquire PV, battery, and thermal signals for control and telemetry.
- Provide local serial telemetry baseline for bring-up.

Inputs:
- Sense points from PV, power stage, and battery boundary

Outputs:
- Control actions and telemetry logs

Requirement links:
- REQ-TEL-001, REQ-TEL-002, REQ-TEL-003, REQ-SAF-006

Decision links:
- DEC-006, DEC-008

## 4. Interface Mapping

| Interface ID | From | To | Intent | Constraint Links |
|---|---|---|---|---|
| IF-PV-CHG-01 | PV boundary | MPPT input | Deliver PV power to conversion chain | REQ-PV-002, DEC-004 |
| IF-CHG-BAT-01 | MPPT output | Battery interface | Deliver regulated charging power | REQ-CHG-001, Q-001 |
| IF-SENSE-CTRL-01 | Plant signals | Control layer | Observability for control and diagnostics | REQ-TEL-002 |
| IF-CTRL-LOG-01 | Control layer | Serial host/log sink | Bring-up telemetry and audit trail | REQ-TEL-001, DEC-006 |

## 5. Safety Allocation Snapshot

- PV boundary: reverse polarity + transient handling.
- Conversion boundary: UVLO/OVP/OCP/OTP assignment required before part lock.
- Battery boundary: fuse and current envelope handoff to BMS-defined limits.
- Bring-up path: staged current-limited sequence per bring-up checklist.

## 6. Open Items Before Component Lock

1. Close DEC-003 topology selection.
2. Confirm cold-Voc design margin for the selected series input baseline.
3. Close Q-001 with BMS datasheet current limits.
4. Close Q-002 instrumentation capability for validation depth.

## 7. Exit Gate For This Artifact

- Block diagram supports a first schematic partition pass.
- Every block has requirement and decision traceability.
- Safety ownership is explicit at boundaries.
