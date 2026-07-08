# Solar Project 200W - Phase 1 Kickoff

Date: 2026-07-06
Owners: Phil + Copilot
Phase goal: begin MPPT implementation exploration with controlled risk and rapid decision closure.

## 1. Immediate Objective

Resolve the two blocking unknowns and lock the PV/MPPT path decisions required for schematic exploration:

1. Q-001: battery and BMS current envelope
2. Q-002: available instrumentation capability
3. DEC-004: panel wiring strategy (series vs parallel)
4. DEC-003: MPPT topology (buck vs buck-boost)

## 2. Fast Execution Plan (Next 1-2 Sessions)

1. Collect constraints:
- capture battery/BMS continuous and peak current limits from datasheet
- capture test bench capability (supply, DMM, scope channels, current measurement)

2. Lock electrical envelope:
- panel wiring strategy selected: series baseline (DEC-004)
- compute charger input/output envelope using baseline assumptions

3. Topology decision:
- decide MPPT topology from locked envelope and protection complexity
- log rationale and rejected alternative in decisions log

4. Schematic exploration start:
- define first-pass MPPT power stage blocks
- define sensing and protection allocation points

## 3. Default Fallback Rules (Do Not Stall)

Use these temporary rules if any item cannot be closed in the current session:

- If Q-001 is unresolved: cap planning current to conservative 15A continuous for charger path assumptions.
- If Q-002 is unresolved: limit test plan to low-current staged checks and logging validation only.
- If DEC-004 is unresolved: model both wiring options in calculation notes, but do not select components yet.
- If DEC-003 is unresolved: continue architecture-level partitioning only; no topology-specific part lock.

## 4. Required Outputs For Phase 1 Entry

- Updated decisions log entries for DEC-003 and DEC-004 (decided or explicitly deferred with new date).
- Constraint snapshot added to this file for Q-001 and Q-002 status.
- MPPT schematic exploration checklist created under hardware docs.

## 5. Constraint Snapshot

| Item | Status | Value / Notes | Source | Date |
|---|---|---|---|---|
| Q-001 BMS continuous current | Bounded (temporary) | Use 15A continuous planning cap until datasheet confirmation. | DEC-011 + Battery/BMS datasheet | 2026-07-06 |
| Q-001 BMS peak current | Open | TBD | Battery/BMS datasheet | 2026-07-06 |
| Q-002 Bench supply capability | Open | TBD | Lab inventory | 2026-07-06 |
| Q-002 Measurement capability | Open | TBD | Lab inventory | 2026-07-06 |
| DEC-004 Panel wiring strategy | Closed | Two 100W Renogy panels in series (2S) baseline. | DEC-004 | 2026-07-06 |
| DEC-003 Topology direction | Decided | Rev 0 locked to asynchronous buck baseline; synchronous converter and gate-driver path deferred to Rev 1. | DEC-003 | 2026-07-08 |

## 6. Revision Strategy

- Rev 0: complete a functional asynchronous buck baseline and staged bring-up validation.
- Rev 1: evaluate and implement synchronous converter plus dedicated gate-driver architecture as a controlled efficiency upgrade.
- Rev 2: cleanup and optimization based on measured thermal, efficiency, and reliability data.

## 7. Exit Criteria For Phase 1 Kickoff

- Q-001 and Q-002 are closed or bounded with explicit conservative limits.
- DEC-004 is closed and DEC-003 is closed with explicit Rev 0 and Rev 1 scope boundaries.
- MPPT exploration can begin without unresolved safety-critical ambiguity.
