# Solar Project 200W - Architecture

Last updated: 2026-07-06
Scope: Phase 0 block-level architecture for planning and traceability.
Related documents: [requirements.md](requirements.md), [decisions-log.md](decisions-log.md)

## 1. Architecture Intent

Define subsystem boundaries and interfaces early so requirement allocation, safety controls, and deferred decisions remain explicit while implementation stays agile.

## 2. Block-Level System View

1. PV input stage
2. MPPT charger stage
3. Battery and protection stage
4. DC distribution stage
5. Inverter stage
6. Monitoring/telemetry/control stage

Power path overview:
- PV source -> PV protection/filtering -> MPPT charger -> battery bus -> DC distribution -> inverter/load path.

Control and telemetry overview:
- Sensing at PV, charger, battery, and thermal nodes.
- Local serial telemetry baseline for bring-up (DEC-006).
- Optional extended telemetry deferred.

## 3. Subsystem Boundaries and Responsibilities

### 3.1 PV Input Stage

Responsibilities:
- Accept 200W-class PV source and condition entry to charger stage.
- Provide reverse polarity and transient protection at boundary.

Boundary assumptions:
- Final panel wiring strategy (series vs parallel) is deferred (DEC-004).
- Input voltage/current envelope will be finalized when panel model is locked.

Requirement mapping:
- REQ-PV-001, REQ-PV-002, REQ-SAF-004.

### 3.2 MPPT Charger Stage

Responsibilities:
- Convert PV input to battery charging profile with MPPT behavior.
- Support 15A minimum charge capability; 20A class preferred where practical.

Boundary assumptions:
- Topology decision (buck vs buck-boost) deferred (DEC-003).
- Controller partition (analog + MCU vs integrated controller) deferred (DEC-005).

Requirement mapping:
- REQ-CHG-001, REQ-CHG-002, REQ-CHG-003, REQ-SAF-003.

### 3.3 Battery and Protection Stage

Responsibilities:
- Host 12.8V LiFePO4 storage domain with BMS-constrained operating envelope.
- Provide primary battery fuse near battery positive terminal.

Boundary assumptions:
- Continuous/peak BMS limits pending closure of Q-001.

Requirement mapping:
- REQ-BAT-001, REQ-BAT-002, REQ-SAF-001.

### 3.4 DC Distribution Stage

Responsibilities:
- Route battery-domain power to charger auxiliaries, control domain, and inverter input.
- Apply branch protection strategy for major current paths.

Boundary assumptions:
- Final branch current ratings depend on BMS limits and inverter transients.

Requirement mapping:
- REQ-SAF-002, REQ-SAF-003.

### 3.5 Inverter Stage

Responsibilities:
- Convert battery-domain DC to 120VAC pure sine output in 300W-class envelope.
- Define AC hazard boundary and integration controls.

Boundary assumptions:
- Inverter internal topology is outside Phase 0 and will be detailed in later phases.

Requirement mapping:
- REQ-INV-001, REQ-INV-002, REQ-SAF-005.

### 3.6 Monitoring, Telemetry, and Control Stage

Responsibilities:
- Measure key electrical and thermal signals for safe control and diagnostics.
- Provide local serial logging baseline for bring-up.

Boundary assumptions:
- Extended telemetry interfaces are deferred and non-blocking for initial bring-up.

Requirement mapping:
- REQ-TEL-001, REQ-TEL-002, REQ-TEL-003, REQ-SAF-006.

## 4. Protection Strategy Outline

Minimum strategy for Phase 0 architecture allocation:
- Battery entry protection: main fuse close to battery positive.
- Path protection: branch fusing for charger and inverter paths.
- Electrical protections: UVLO, OVP, OCP, OTP assigned to charger/control stack.
- PV front-end: reverse polarity and transient mitigation.
- AC boundary discipline: hazardous domain handling and test controls for inverter output.

## 5. Power and Signal Interface Summary

| Interface | Domain | Direction | Key Signals / Constraints | Notes |
|---|---|---|---|---|
| IF-PV-CHG-01 | PV -> Charger | Power | PV voltage/current envelope, transient behavior | Depends on DEC-004 |
| IF-CHG-BAT-01 | Charger -> Battery | Power | Charge current capability, charge profile limits | Depends on DEC-003/DEC-005 |
| IF-BAT-DIST-01 | Battery -> Distribution | Power | Main fuse boundary, branch protection | Depends on Q-001 |
| IF-DIST-INV-01 | Distribution -> Inverter | Power | High current DC feed, startup current behavior | Inverter phase detail later |
| IF-SENSE-CTRL-01 | Plant -> Control | Telemetry | PV V/I, battery V/I, thermal channels | Baseline for REQ-TEL-002 |
| IF-CTRL-LOG-01 | Control -> Host | Telemetry | Local serial logs for bring-up and diagnostics | Defined by DEC-006 |

## 6. Deferred Architectural Decisions

- DEC-003: MPPT topology final selection.
- DEC-004: Panel wiring strategy final selection.
- DEC-005: Charger control partition final selection.
- Q-001: Confirm BMS current envelope.
- Q-002: Confirm instrumentation capability for validation depth.

## 7. Phase 0 Architecture Exit Criteria

- All major subsystem boundaries are documented with responsibilities.
- Requirement mappings are established at subsystem level.
- Safety and protection controls are allocated at planning level.
- Deferred decisions are explicit and linked to owners/target dates in decisions log.
