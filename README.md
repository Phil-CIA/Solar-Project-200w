# Solar Project 200W

Solar Project 200W is a prototype-first hardware and firmware effort for a 200W-class solar charging system built around a 4S LiFePO4 battery domain.

## What this repository is for

- Designing and validating a safe MPPT charger path
- Defining control, telemetry, and protection behavior
- Tracking design decisions, handoffs, and bring-up evidence

## Current project state

Only Rev 0 and Rev 1 exist. The Rev 1 production release sent to the manufacturer is the source of truth at `C:\Users\user\OneDrive\JLCPCB files\Solar Charger\Rev 1`. This repository contains Rev 1 copies and supporting history, but some tracked files differ from that release; use the release package for board identity and manufacturing questions until the copies are reconciled.

## System baseline

- PV class: 2x100W panel setup
- Battery class: 12.8V LiFePO4 (4S)
- Charge-path target: 15A minimum, 20A preferred growth path

## Project structure

- [docs/](docs/): requirements, architecture, decisions, test plans, phase reviews
- [hardware/](hardware/): KiCad files, MPPT notes, wiring maps, implementation checklists
- [firmware/](firmware/): firmware workspace and bring-up software notes

## Start here

1. [docs/requirements.md](docs/requirements.md)
2. [docs/architecture.md](docs/architecture.md)
3. [docs/decisions-log.md](docs/decisions-log.md)
4. [HANDOFF.md](HANDOFF.md) for current repository status
5. [SOLAR_HANDOFF_2026-09-19.md](SOLAR_HANDOFF_2026-09-19.md) and [SOLAR_HANDOFF_2026-09-18.md](SOLAR_HANDOFF_2026-09-18.md) for the latest recorded Rev 1 closeout
6. [hardware/kicad/solar-project/REVISION-INDEX.md](hardware/kicad/solar-project/REVISION-INDEX.md) for KiCad source-of-truth rules

## Simulation workflow

The PV switch gate-drive concept should be verified with SPICE as part of the normal review loop. The project-local LTspice assets are in [hardware/kicad/solar-project/spice](hardware/kicad/solar-project/spice), including the schematic draft [hardware/kicad/solar-project/spice/pv_switch_gate_drive.asc](hardware/kicad/solar-project/spice/pv_switch_gate_drive.asc) and the launcher [hardware/kicad/solar-project/spice/run_ltspice_pv_gate_drive.cmd](hardware/kicad/solar-project/spice/run_ltspice_pv_gate_drive.cmd).

A provisional LTspice integration harness is also present at [hardware/kicad/solar-project/spice/90_system_integration_harness_ltspice.cir](hardware/kicad/solar-project/spice/90_system_integration_harness_ltspice.cir). It is a simplified, staged model of the PV input, buck power stage, and battery boundary. The current run converged to about 13.64 V at the output node and 13.98 V at the battery bus at 4 ms, which is consistent with a basic charging path but still not hardware-validated.

The earlier simulation found inadequate Vgs margin for the modeled gate-drive arrangement across the tested PV input range. Treat that as evidence about the modeled circuit only; confirm the topology against the Rev 1 production release before applying the result to a physical board.

## Safety notice

This project includes high-current DC power electronics and planned AC-domain integration.
Use staged, current-limited bring-up and treat all safety checks as mandatory gates.

## Session continuity

- [HANDOFF.md](HANDOFF.md)
- [NEXT_CHAT_PROMPT.md](NEXT_CHAT_PROMPT.md)
