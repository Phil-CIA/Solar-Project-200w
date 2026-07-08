# Solar Project 200W - Requirements

Last updated: 2026-07-06
Scope: Phase 0 planning baseline for 200W-class learning system.
Traceability: Link each requirement to architecture sections and decisions in [decisions-log.md](decisions-log.md).

## 1. Requirement Method

Each requirement includes:
- ID: stable reference for design and test traceability.
- Metric and target: measurable engineering expectation.
- Verification: planned pass/fail method.
- Source: concept baseline or decision reference.

## 2. System Baseline

- PV input class: 200W nominal array (2x100W modules).
- Battery class: 12.8V LiFePO4 (4S), 40Ah nominal.
- Charge path target: 15A minimum, 20A preferred capability class.
- Inverter envelope: 300W pure sine target.

## 3. Electrical Requirements

| ID | Requirement | Target / Limit | Verification Method | Source |
|---|---|---|---|---|
| REQ-PV-001 | System shall support a PV nameplate power class of 200W. | 180W to 220W accepted design envelope. | Nameplate review + input power budget check. | Concept baseline |
| REQ-PV-002 | PV operating window shall include module maximum power region during normal conditions. | PV MPP tracking region shall be documented and covered by charger input range. | Architecture and charger input-range review against selected panel datasheets. | Concept + DEC-003/DEC-004 |
| REQ-CHG-001 | Charger architecture shall target 15A minimum continuous charge capability. | >=15A continuous design target at rated thermal conditions. | Schematic current-path review + component derating worksheet. | Concept baseline |
| REQ-CHG-002 | Charger solution should preserve growth margin toward 20A class when feasible. | 20A preferred target documented as stretch objective. | Design review note with constraints and tradeoffs. | Concept baseline |
| REQ-CHG-003 | Estimated peak solar charge current expectation shall be documented. | For 200W baseline, expected peak approximately 12A to 13A. | Calculation using I_charge ~= (P_solar * eta) / V_charge with assumptions recorded. | Concept sizing note |
| REQ-BAT-001 | Battery nominal chemistry and stack shall remain LiFePO4 4S for Phase 0/1 planning. | 12.8V nominal, 40Ah class. | BOM and requirements consistency check. | Concept baseline |
| REQ-BAT-002 | Battery charge/discharge operational limits shall be constrained by selected BMS datasheet. | Continuous and peak current limits explicitly captured before hardware lock. | Datasheet extraction checklist (open item closure). | Decisions Q-001 |
| REQ-INV-001 | Inverter power envelope shall target 300W continuous output class. | 300W continuous target at defined ambient and cooling assumptions. | Architecture power budget and thermal assumption review. | Concept baseline |
| REQ-INV-002 | Inverter input current expectation shall be documented for 300W target operation. | I_in estimate and design margin documented at battery voltage range. | Calculation worksheet with efficiency assumption. | Concept baseline |

## 4. Telemetry and Control Requirements

| ID | Requirement | Target / Limit | Verification Method | Source |
|---|---|---|---|---|
| REQ-TEL-001 | Initial telemetry baseline shall support local serial logging for bring-up. | UART/serial logging path defined for charger and battery observability. | Architecture interface review + firmware signal list placeholder. | DEC-006 |
| REQ-TEL-002 | Core electrical observability shall include panel, battery, and thermal signals. | At minimum: PV voltage/current, battery voltage/current, key temperature points. | Signal map review against architecture boundary definitions. | Concept + DEC-007 |
| REQ-TEL-003 | Telemetry extension shall be designed as optional and non-blocking for first bring-up. | Wireless or extended interfaces deferred without blocking core path. | Decisions-log review of deferred scope. | DEC-006 |

## 5. Safety and Protection Requirements

| ID | Requirement | Target / Limit | Verification Method | Source |
|---|---|---|---|---|
| REQ-SAF-001 | Main battery positive protection shall include a fuse located near battery terminal. | Fuse placement documented at battery positive entry point. | Architecture placement review. | Concept safety priorities |
| REQ-SAF-002 | Subsystem branch protection shall be included for major load paths. | Branch fusing strategy documented for charger and inverter paths. | Protection strategy checklist review. | Concept safety priorities |
| REQ-SAF-003 | Charger and distribution design shall include UVLO/OVP/OCP/OTP coverage. | Protection functions explicitly allocated to circuits or controller features. | Protection allocation table review. | Concept + DEC-007 |
| REQ-SAF-004 | PV input path shall include reverse polarity and transient protection strategy. | Reverse polarity and surge/transient mitigation documented at PV boundary. | Interface/protection schematic review (planning-level in Phase 0). | Concept + DEC-007 |
| REQ-SAF-005 | AC inverter output shall be treated as hazardous mains-level domain. | Isolation, enclosure, and handling warnings documented before integration testing. | Safety checklist review prior to integration phase. | Concept safety priorities |
| REQ-SAF-006 | Bring-up shall be staged with current-limited progression and logged checkpoints. | Stepwise current progression and stop criteria defined before hardware bring-up. | Bring-up procedure review against decisions log. | DEC-008 |

## 6. Validation Gate Requirements

| ID | Requirement | Target / Limit | Verification Method | Source |
|---|---|---|---|---|
| REQ-VAL-001 | Every requirement shall map to a planned verification activity. | No requirement left without a verification method. | Traceability matrix review. | Planning policy |
| REQ-VAL-002 | Deferred decisions affecting requirements shall include owner and target date. | 100% of deferred, requirement-impacting decisions have owner/date. | Decisions-log audit. | DEC-001 policy |
| REQ-VAL-003 | Phase transition to implementation shall require requirements and architecture review pass. | Review checklist completed with no unresolved safety blockers. | Phase gate checklist sign-off (Phil + Copilot). | DEC-002 |

## 7. Open Items Blocking Requirement Closure

- Confirm battery/BMS continuous and peak current limits (Q-001).
- Confirm available test instrumentation capability (Q-002).
- Finalize PV wiring strategy and MPPT topology coupling (DEC-003, DEC-004).

## 8. Definition of Done (Phase 0 Requirements)

- Requirement IDs are stable and uniquely named.
- Every requirement includes measurable target/limit and verification method.
- Safety-critical controls are represented in explicit requirements.
- Open blockers are listed with decision-log references and target dates.

## 9. Compact Traceability Matrix

| Requirement ID | Architecture Boundary / Interface | Decision / Open Item Link |
|---|---|---|
| REQ-PV-001 | 3.1 PV Input Stage | DEC-004 |
| REQ-PV-002 | 3.1 PV Input Stage, IF-PV-CHG-01 | DEC-003, DEC-004 |
| REQ-CHG-001 | 3.2 MPPT Charger Stage, IF-CHG-BAT-01 | DEC-003 |
| REQ-CHG-002 | 3.2 MPPT Charger Stage, IF-CHG-BAT-01 | DEC-003, DEC-005 |
| REQ-CHG-003 | 3.2 MPPT Charger Stage | DEC-003 |
| REQ-BAT-001 | 3.3 Battery and Protection Stage, IF-BAT-DIST-01 | Q-001 |
| REQ-BAT-002 | 3.3 Battery and Protection Stage, IF-BAT-DIST-01 | Q-001 |
| REQ-INV-001 | 3.5 Inverter Stage, IF-DIST-INV-01 | DEC-002 |
| REQ-INV-002 | 3.5 Inverter Stage, IF-DIST-INV-01 | Q-001 |
| REQ-TEL-001 | 3.6 Monitoring/Telemetry/Control, IF-CTRL-LOG-01 | DEC-006 |
| REQ-TEL-002 | 3.6 Monitoring/Telemetry/Control, IF-SENSE-CTRL-01 | DEC-007 |
| REQ-TEL-003 | 3.6 Monitoring/Telemetry/Control | DEC-006 |
| REQ-SAF-001 | 3.3 Battery and Protection Stage | DEC-007 |
| REQ-SAF-002 | 3.4 DC Distribution Stage | DEC-007 |
| REQ-SAF-003 | 3.2 MPPT Charger Stage, 3.4 DC Distribution Stage | DEC-007 |
| REQ-SAF-004 | 3.1 PV Input Stage | DEC-007 |
| REQ-SAF-005 | 3.5 Inverter Stage | DEC-007 |
| REQ-SAF-006 | 3.6 Monitoring/Telemetry/Control | DEC-008 |
| REQ-VAL-001 | 7. Phase 0 Architecture Exit Criteria | DEC-001 |
| REQ-VAL-002 | 6. Deferred Architectural Decisions | DEC-001 |
| REQ-VAL-003 | 7. Phase 0 Architecture Exit Criteria | DEC-002 |
