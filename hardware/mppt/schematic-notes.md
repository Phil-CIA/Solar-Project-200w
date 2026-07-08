# MPPT Schematic Notes (First Pass)

Last updated: 2026-07-06
Owners: Phil + Copilot
Status: buck-oriented exploration scaffold for KiCad schematic partitioning.

## 1. Scope

This file defines how to partition the first buck MPPT schematic draft so safety, sensing, and power paths are explicit before component lock.

## 2. Sheet Partition Plan

Recommended initial KiCad sheet split:

1. Sheet A - PV Input and Protection
- PV connector and polarity marking: 2-position pluggable terminal block
- Reverse polarity strategy: series Schottky diode for first pass
- Transient/surge suppression: 64 V TVS diode
- Input filter boundary: 100 V capacitor bank close to connector and diode

Symbol order:
1. J1 connector
2. F1 fuse
3. D1 series Schottky diode
4. D2 TVS diode
5. C1/C2/C3 input capacitors

2. Sheet B - MPPT Power Stage
- High-side buck switch stage
- Inductor energy-storage path
- Freewheel diode or synchronous low-side path
- Output filter boundary

3. Sheet C - Battery Interface and Protection Handoff
- Charger output boundary
- Battery connector boundary
- Main fuse handoff note
- Current-limit envelope note tied to Q-001/DEC-011

4. Sheet D - Sensing and Control I/O
- PV voltage/current sensing points
- Battery voltage/current sensing points
- Thermal sensing points
- Serial telemetry signal breakout baseline

## 3. Net Class and Naming Plan

Use stable, purpose-based net names from the first draft:

Power domain nets:
- PV_IN_POS
- PV_IN_NEG
- MPPT_SW_NODE
- CHG_OUT_POS
- CHG_OUT_NEG
- BAT_BUS_POS
- BAT_BUS_NEG

Sense nets:
- SENSE_PV_V
- SENSE_PV_I
- SENSE_BAT_V
- SENSE_BAT_I
- SENSE_TEMP_PWR
- SENSE_TEMP_BOARD

Control and logging nets:
- CTRL_PWM_MAIN
- CTRL_EN_CHG
- FAULT_OCP
- FAULT_OVP
- UART_TX_LOG
- UART_RX_CFG

## 4. Schematic Annotation Rules

- Tag all temporary assumptions with "ASSUME:" prefix.
- Tag unresolved decisions with "DECISION:" plus ID (DEC-003, DEC-005).
- Tag safety ownership points with "SAFETY:".
- Tag verification-critical nodes with "VERIFY:".

## 5. Minimum Notes Required On First Draft

- Input electrical envelope note (or temporary bound reference).
- Output current planning bound (15A temporary cap if Q-001 still open).
- Protection ownership allocation note (UVLO/OVP/OCP/OTP).
- Weak-sun / low-headroom behavior note for buck feasibility review.
- Thermal observation points note.
- Bring-up caution note referencing staged current-limited process.

## 6. Draft Review Checklist

- [ ] Every high-current path has explicit current direction and return path.
- [ ] Every protection function has an owner block.
- [ ] Every sensing function has a named node and destination.
- [ ] Temporary assumptions are visible and linked to decision IDs.
- [ ] Buck-specific power path parts are present or explicitly blocked with notes.
- [ ] Net names are consistent with net-plan.md.

## 7. Exit Criteria

- First partitioned schematic draft exists in KiCad.
- Critical nets and notes are present and named consistently.
- Deferred buck-implementation decisions are visible as annotations, not hidden assumptions.
