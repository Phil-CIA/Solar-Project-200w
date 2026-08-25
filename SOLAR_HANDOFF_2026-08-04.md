# Solar Project 200W - Rev One Kickoff Handoff (2026-08-04)

## Session outcome

Rev 1 is now the active schematic lineage for board work, and Rev 0 input-protection closure is complete enough to transition focus to Rev One board planning.

## Rev One changes started

The following Rev One work has already been started and is recorded in the repository:

- Canonical schematic title block relabeled to Rev 1.
- Rev 0 schematic and board restored into explicit archive files.
- Rev 1 schematic and board archive files created.
- Rev One delta plan and ERC/DRC closure checklist added under docs/.

## What is locked from this session

1. Rev 1 schematic and board copies are present alongside recovered Rev 0 copies for comparison and release planning.
2. LM74502H input-protection topology is implemented in schematic and validated by netlist review.
3. Back-to-back N-MOS path is retained for high-side reverse protection.
4. Control/sense wiring is in the expected state:
   - VS referenced to V_out_sense
   - OV divider using R37/R66/R67
   - EN_UVLO default-off pull-down using R38
5. ERC report is clean (0 errors, 0 warnings).

## Deferred item

1. Review C52 working-voltage margin and raise rating if desired before schematic freeze.

## Rev One objective

Define and execute a controlled Rev One board update that builds on the validated Rev 0 protection path.

## Rev One planning scope

1. Freeze change list for Rev One (must-have vs optional).
2. Confirm schematic deltas for each sheet and update wiring maps.
3. Run ERC/DRC closure plan and define board-level validation checklist.
4. Prepare manufacturing outputs only after checklist closure.

## Rev One planning artifacts

1. docs/rev-one-delta-plan-2026-08-04.md
2. docs/test/rev-one-erc-drc-closure-checklist.md
3. hardware/mppt/schematic-notes.md

## First actions next session

1. Read HANDOFF.md and this file.
2. Build a Rev One delta table by sheet:
   - Sheet 2 power path
   - Sheet 3 battery interface
   - Sheet 4 sensing/control
3. Decide whether C52 rating change is mandatory for Rev One gate.
4. Open a Rev One checklist in docs for completion tracking.

## Primary evidence

1. hardware/kicad/solar-project/Solar Project.kicad_sch
2. hardware/kicad/solar-project/Solar Project Rev1.kicad_sch
3. hardware/kicad/solar-project/Solar Project Rev0.kicad_sch
4. hardware/kicad/solar-project/Solar Project.net
5. hardware/kicad/solar-project/ERC.rpt
6. hardware/mppt/schematic-notes.md
