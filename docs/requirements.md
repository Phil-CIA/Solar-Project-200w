# Solar Project 200W - Requirements

Last updated: 2026-11-24
Scope: Project-level planning baseline for a 125W continuous / 150W maximum PV charger; Rev2-specific implementation and qualification are tracked in `rev-two-change-plan.md`.
Traceability: Link each requirement to architecture sections and decisions in [decisions-log.md](decisions-log.md).

## 1. Requirement Method

Each requirement includes:
- ID: stable reference for design and test traceability.
- Metric and target: measurable engineering expectation.
- Verification: planned pass/fail method.
- Source: concept baseline or decision reference.

## 2. System Baseline

- PV input compatibility: selected 2x100W parallel (2P) array, with harvested power limited by the charger (DEC-051).
- Battery class: 12.8V LiFePO4 (4S), 40Ah nominal.
- Charger output rating: 125W continuous, 150W maximum operating power.
- Charge-current target: 8A to 10A nominal; the external-ISET limit is approximately 11.9A nominal with an estimated 9.1A-15.2A range, not a guaranteed 12A ceiling (DEC-071).
- Battery/BMS charge-current capability: at least 20A.
- Inverter envelope: 300W pure sine target.

## 3. Electrical Requirements

| ID | Requirement | Target / Limit | Verification Method | Source |
|---|---|---|---|---|
| REQ-PV-001 | Charger input hardware shall tolerate the selected PV array while control limits harvested power. | Support the selected 2P `RSP100DCT` array: 28.5V STC Voc, approximately 33.9V estimated cold Voc, 8.90A Isc; limit maximum PV operating power to 150W. | Panel cold-`Voc`/`Isc` review + protection derating + power-limit test. | DEC-049 through DEC-052 |
| REQ-PV-002 | PV operating window shall include the selected module maximum-power region during normal conditions. | 2P baseline is approximately 23.6V Vmp and 8.48A Imp; verify the input range against this point and the actual low-light/hot-panel floor. | Architecture and charger input-range review against selected panel data, then source testing. | DEC-049 through DEC-051 |
| REQ-CHG-001 | Charger shall deliver 125W continuously at the defined thermal design point. | 125W at 12.8V nominal (9.77A), 40C maximum ambient. U7 board-temperature rise target is 25C and maximum is 30C; component hotspots must remain within derated datasheet limits. | First-article rated-power thermal soak after PCB fabrication, before rated-power use; record U7 and direct power-component temperatures. | DEC-078 |
| REQ-CHG-002 | Charger shall limit maximum operating power and charge current. | 150W maximum PV operating power; 8A to 10A nominal charge target. External-ISET is approximately 11.9A nominal with an estimated 9.1A-15.2A range, not a guaranteed 12A ceiling. | Power/current tests across battery voltage range + measured current-limit and protection verification. | DEC-023, DEC-027, DEC-028, DEC-071, DEC-078 |
| REQ-CHG-003 | Expected charge current shall be documented across the battery range. | At 150W input and 93% assumed efficiency: approximately 9.6A at 14.6V and 10.9A at 12.8V; commanded current remains within configured limits. | Calculation plus bench comparison using measured efficiency. | DEC-023, DEC-028 |
| REQ-CHG-004 | Charger startup shall be battery-qualified and source-aware. | Validate battery and PV before enable; begin at low current; reduce demand before PV reaches hardware UVLO; weak-source retry shall not produce sustained UVLO packet bursts. | Startup captures with PV voltage, `EN_UVLO`, `SS_ATRK`, output voltage, and current under strong and weak source cases. | DEC-025 through DEC-029 |
| REQ-CHG-005 | Rev 1 output capacitance shall be justified by converter stability and transient requirements. | Do not carry the Rev 0 approximately 1.1mF to 1.2mF bank forward without analysis and measurement. | Loop/stability analysis + startup and load-transient tests using fitted capacitance and ESR. | DEC-024, DEC-025 |
| REQ-BAT-001 | Battery nominal chemistry and stack shall remain LiFePO4 4S for Phase 0/1 planning. | 12.8V nominal, 40Ah class. | BOM and requirements consistency check. | Concept baseline |
| REQ-BAT-002 | Battery charge/discharge operational limits shall be constrained by the selected BMS datasheet. | BMS charge-current capability shall be at least 20A; continuous and peak charge/discharge limits shall be captured before hardware lock. | Datasheet extraction checklist + charger/BMS coordination review. | Q-001 + DEC-023 + DEC-025 |
| REQ-BAT-003 | Rev 1 shall not implement a BMS communication link (UART/CAN/SMBus or a discrete permissive/fault input); charge/discharge coordination with the pack relies solely on the current-capability assumption in REQ-BAT-002. | No BMS-side comms or fault-signal wiring exists in Rev 1; feature explicitly deferred, not silently missing. | Requirements/architecture text review against the as-built schematic. | DEC-062 |
| REQ-INV-001 | Inverter power envelope shall target 300W continuous output class. | 300W continuous target at defined ambient and cooling assumptions. | Architecture power budget and thermal assumption review. | Concept baseline |
| REQ-INV-002 | Inverter input current expectation shall be documented for 300W target operation. | I_in estimate and design margin documented at battery voltage range. | Calculation worksheet with efficiency assumption. | Concept baseline |

## 4. Telemetry and Control Requirements

| ID | Requirement | Target / Limit | Verification Method | Source |
|---|---|---|---|---|
| REQ-TEL-001 | Initial telemetry baseline shall support local serial logging for bring-up. | UART/serial logging path defined for charger and battery observability. | Architecture interface review + firmware signal list placeholder. | DEC-006 |
| REQ-TEL-002 | Core electrical observability shall include panel, battery, and thermal signals. | At minimum: PV voltage/current, battery voltage/current, board-ambient temperature (`U7` AHT20), and a dedicated battery-pack temperature input independent of BMS-reported temperature (`J8` I2C bus tap for a remote sensor module, DEC-061). | Signal map review against architecture boundary definitions. | Concept + DEC-007 + DEC-061 |
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
- Select the remote battery-pack I2C temperature-sensor part/address and mating cable for `J8`, and rebuild the BOM entry (DEC-061), before Gate 4 PCB commit.

## 8. Definition of Done (Phase 0 Requirements)

- Requirement IDs are stable and uniquely named.
- Every requirement includes measurable target/limit and verification method.
- Safety-critical controls are represented in explicit requirements.
- Open blockers are listed with decision-log references and target dates.

## 9. Compact Traceability Matrix

| Requirement ID | Architecture Boundary / Interface | Decision / Open Item Link |
|---|---|---|
| REQ-PV-001 | 3.1 PV Input Stage | DEC-004, DEC-023, DEC-024, DEC-028 |
| REQ-PV-002 | 3.1 PV Input Stage, IF-PV-CHG-01 | DEC-003, DEC-004 |
| REQ-CHG-001 | 3.2 MPPT Charger Stage, IF-CHG-BAT-01 | DEC-003, DEC-023, DEC-024 |
| REQ-CHG-002 | 3.2 MPPT Charger Stage, IF-CHG-BAT-01 | DEC-003, DEC-005, DEC-023, DEC-027, DEC-028 |
| REQ-CHG-003 | 3.2 MPPT Charger Stage | DEC-023, DEC-028 |
| REQ-CHG-004 | 3.1 PV Input Stage, 3.2 MPPT Charger Stage | DEC-025 through DEC-029 |
| REQ-CHG-005 | 3.2 MPPT Charger Stage | DEC-024, DEC-025 |
| REQ-BAT-001 | 3.3 Battery and Protection Stage, IF-BAT-DIST-01 | Q-001 |
| REQ-BAT-002 | 3.3 Battery and Protection Stage, IF-BAT-DIST-01 | Q-001, DEC-023, DEC-025 |
| REQ-BAT-003 | 3.3 Battery and Protection Stage, IF-BAT-DIST-01 | DEC-062 |
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
