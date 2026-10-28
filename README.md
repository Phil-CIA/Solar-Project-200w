# Solar Project 200W

Solar Project 200W is a prototype-first hardware and firmware effort for a 200W-class solar charging system built around a 4S LiFePO4 battery domain.

## What this repository is for

- Designing and validating a safe MPPT charger path
- Defining control, telemetry, and protection behavior
- Tracking design decisions, handoffs, and bring-up evidence

## Current Rev 1 focus

- Only Rev 0 and Rev 1 exist; Rev 1 is the current production revision. The production release at `C:\Users\user\OneDrive\JLCPCB files\Solar Charger\Rev 1` is authoritative for board identity, manufacturing and board-specific tests. Repository copies support continued development; the schematic, project and BOM differences are described in the board notes.
- Rev 1 is assembled; the supplied manufacturing package and both-side photos are recorded in [board notes](docs/rev-one-board-notes.md).
- Continue with practical low-energy bring-up and Rev 1 firmware, using the [development tracker](https://github.com/Phil-CIA/Solar-Project-200w/issues/1) and [working plan](docs/rev-one-closure-plan-2026-09-16.md#6-assembled-board-execution-and-issue-1-closure).
- This is a hobby prototype: keep useful notes and relevant checks, not a formal lab dossier. Assembly does not establish working charging or full-power performance.
- Rev 1 STM32G473 bring-up firmware through `ff89791` is already on `main`; see [firmware notes](firmware/README.md). That is development progress, not proof of board compatibility or a passing powered test.

## System baseline

- PV class: 2x100W panels in parallel (DEC-051)
- Battery class: 12.8V LiFePO4 (4S)
- Rev 1 design targets: 125 W continuous output, 150 W maximum PV operating power, 8-10 A nominal charging, approximately 12 A hardware protection pending verification

## Project structure

- [docs/](docs/): requirements, architecture, decisions, test plans, phase reviews
- [hardware/](hardware/): KiCad files, MPPT notes, wiring maps, implementation checklists
- [firmware/](firmware/): firmware workspace and bring-up software notes

## Start here

1. [HANDOFF.md](HANDOFF.md) - current checkpoint and next step
2. [docs/rev-one-board-notes.md](docs/rev-one-board-notes.md) - working board baseline and known differences
3. [docs/README.md](docs/README.md) - engineering references
4. [docs/decisions-log.md](docs/decisions-log.md) - retained design decisions
5. [SOLAR_HANDOFF_2026-09-19.md](SOLAR_HANDOFF_2026-09-19.md) and [SOLAR_HANDOFF_2026-09-18.md](SOLAR_HANDOFF_2026-09-18.md) for the historical Rev 1 fabrication closeout
6. [hardware/kicad/solar-project/REVISION-INDEX.md](hardware/kicad/solar-project/REVISION-INDEX.md) for KiCad source-of-truth rules

## Simulation workflow

The PV switch gate-drive concept should be verified with SPICE as part of the normal review loop. The project-local LTspice assets are in [hardware/kicad/solar-project/spice](hardware/kicad/solar-project/spice), including the schematic draft [hardware/kicad/solar-project/spice/pv_switch_gate_drive.asc](hardware/kicad/solar-project/spice/pv_switch_gate_drive.asc) and the launcher [hardware/kicad/solar-project/spice/run_ltspice_pv_gate_drive.cmd](hardware/kicad/solar-project/spice/run_ltspice_pv_gate_drive.cmd).

A provisional LTspice integration harness is also present at [hardware/kicad/solar-project/spice/90_system_integration_harness_ltspice.cir](hardware/kicad/solar-project/spice/90_system_integration_harness_ltspice.cir). It is a simplified, staged model of the PV input, buck power stage, and battery boundary. The current run converged to about 13.64 V at the output node and 13.98 V at the battery bus at 4 ms, which is consistent with a basic charging path but still not hardware-validated.

Earlier simulation identified a gate-drive margin problem in the arrangement it modeled. That historical result is not a measured failure of the assembled Rev 1 U14/Q5/Q6 protection circuit; confirm the topology against the Rev 1 production release and use matching low-energy checks before drawing that conclusion.

## Safety notice

This project includes high-current DC power electronics and planned AC-domain integration.
Use staged, current-limited bring-up and treat all safety checks as mandatory gates.

## Session continuity

- [HANDOFF.md](HANDOFF.md)
- [NEXT_CHAT_PROMPT.md](NEXT_CHAT_PROMPT.md)
