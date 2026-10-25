# Engineering Docs Guide

This folder is the working engineering record for Solar Project 200W.

## Read order for active work

Rev 1 assembly was reported complete on 2026-10-25; electrical and performance qualification are not established by that report. Start with the [assembled-board execution and closure plan](rev-one-closure-plan-2026-09-16.md#6-assembled-board-execution-and-issue-1-closure) and [development tracker](https://github.com/Phil-CIA/Solar-Project-200w/issues/1) for current sequencing. Reconcile the older requirements and bench procedures against the as-built board before relying on them for powered work.

The [board notes](rev-one-board-notes.md) identify Phil's manufacturing package, supplied photos and useful BOM differences. Use the hobby-scale process Phil requested: short notes and relevant checks, not a mandatory lab dossier. Older documentation requirements do not override that current scope; electrical precautions still apply.

1. [requirements.md](requirements.md)
2. [architecture.md](architecture.md)
3. [decisions-log.md](decisions-log.md)
4. [test/bringup-checklist.md](test/bringup-checklist.md)
5. [test/validation-plan.md](test/validation-plan.md)

## What to update during each session

- Decisions and tradeoffs: [decisions-log.md](decisions-log.md)
- Design state and open blockers: [../hardware/mppt/schematic-notes.md](../hardware/mppt/schematic-notes.md)
- Session continuity and next-step intent: latest handoff at repository root

## Hardware design trail

Use these when working MPPT implementation details:

- [../hardware/mppt/block-diagram.md](../hardware/mppt/block-diagram.md)
- [../hardware/mppt/net-plan.md](../hardware/mppt/net-plan.md)
- [../hardware/mppt/pv-input-wiring-map.md](../hardware/mppt/pv-input-wiring-map.md)
- [../hardware/mppt/buck-power-stage-wiring-map.md](../hardware/mppt/buck-power-stage-wiring-map.md)
- [../hardware/mppt/sheet4-sensing-control-wiring-map.md](../hardware/mppt/sheet4-sensing-control-wiring-map.md)
- [../hardware/mppt/kicad-implementation-checklist.md](../hardware/mppt/kicad-implementation-checklist.md)
- [../hardware/mppt/lm851772-lm51772-migration-checklist.md](../hardware/mppt/lm851772-lm51772-migration-checklist.md)

## Working rules

- Keep assumptions visible and dated.
- Mark unresolved items with owner and target date.
- Do not treat deferred ERC and power-authority findings as closed until explicitly waived.
- Preserve traceability between requirements, decisions, and validation evidence.
- Follow the [Datasheet Handling SOP](datasheets/datasheet-sop.md) for all datasheet, reference manual, and PDF artifacts.
