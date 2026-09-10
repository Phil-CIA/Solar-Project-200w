# Solar Project 200W - Architecture

Last updated: 2026-09-09
Scope: Rev 1 block-level architecture for planning and traceability.
Related documents: [requirements.md](requirements.md), [decisions-log.md](decisions-log.md)

## 1. Architecture Intent

Define subsystem boundaries and interfaces early so requirement allocation, safety controls, and deferred decisions remain explicit while implementation stays agile.

Rev 1 follows DEC-024: reconfigure and correct the verified Rev 0 architecture rather than replace it. Retain one LM51772 four-switch stage, the PV protection concept, sensing functions, output-current shunt function, and STM32 supervisory partition unless a failed analysis or validation result forces a new decision.

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
- Accept the existing 2x100W series PV source while permitting the charger to limit harvested power to 150W.
- Provide reverse polarity and transient protection at boundary.

Boundary assumptions:
- Final panel wiring strategy (series vs parallel) is deferred (DEC-004).
- Provisional nominal/qualified PV operating point is `20 V` for Rev 1 converter sizing and normal MPPT operation (DEC-030).
- LM51772 hardware UVLO remains approximately `6 V` as an independent protection boundary; it is not the normal MPPT operating point.

Requirement mapping:
- REQ-PV-001, REQ-PV-002, REQ-SAF-004.

### 3.2 MPPT Charger Stage

Responsibilities:
- Convert PV input to battery charging profile with MPPT behavior.
- Provide 125W continuous output and limit PV operating power to 150W.
- Target 8A to 10A nominal battery charge current with an approximately 12A hardware protection ceiling.
- Use one LM51772 conversion stage with battery-qualified, source-aware startup.
- Coordinate soft-start, input-voltage regulation, and hardware UVLO so demand is reduced before source collapse.
- Hold the normal source-protection floor above hardware UVLO; use `20 V` as the provisional nominal/qualified operating reference until the panel model is locked.
- Use only the output capacitance justified by stability and transient validation.

Boundary assumptions:
- Retain the LM51772 four-switch power stage and rerate it for the Rev 1 envelope (DEC-003, DEC-023, DEC-024).
- Retain the STM32 as charger supervisor and telemetry owner while U6 owns the fast electrical regulation loops (DEC-005, DEC-024, DEC-028, DEC-029).
- Battery-first sequencing and startup control are governed by DEC-025 through DEC-029.

Startup sequence:
1. Power the control domain from the battery and validate battery voltage, temperature, and faults.
2. Keep conversion disabled while PV open-circuit voltage is qualified with hysteresis and delay.
3. Start at a low current command through the LM51772 soft-start path.
4. Ramp charge current only while PV remains above its configured operating floor.
5. Ramp down and retry after a delay when the source cannot sustain operation.

Requirement mapping:
- REQ-CHG-001, REQ-CHG-002, REQ-CHG-003, REQ-CHG-004, REQ-CHG-005, REQ-SAF-003.

### 3.3 Battery and Protection Stage

Responsibilities:
- Host 12.8V LiFePO4 storage domain with BMS-constrained operating envelope.
- Require at least 20A BMS charge-current capability so normal charger current limiting remains below the BMS trip boundary.
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

- Q-001: Confirm BMS current envelope.
- Q-002: Confirm instrumentation capability for validation depth.

## 7. Phase 0 Architecture Exit Criteria

- All major subsystem boundaries are documented with responsibilities.
- Requirement mappings are established at subsystem level.
- Safety and protection controls are allocated at planning level.
- Deferred decisions are explicit and linked to owners/target dates in decisions log.
