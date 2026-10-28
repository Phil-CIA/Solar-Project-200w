# Solar Project KiCad Revision Index

Last updated: 2026-10-28

## Current production source - Rev 1

Only Rev 0 and Rev 1 exist. The Rev 1 production release sent to the manufacturer is the source of truth at:

`C:\Users\user\OneDrive\JLCPCB files\Solar Charger\Rev 1`

It contains `Kicad`, `BOM`, and `GERBERS` folders. Use that package to identify and test the production board. Phil supplied both-side photos; the saved PCB and all 14 loose fabrication files match the repository copies in `fab-output-2026-09-18`. The schematic matches after removing 176 empty `LCSC ` metadata properties; project differences are limited to the BOM export filename and `used_designators` ordering. The manufacturing BOM has meaningful differences from the procurement master CSV.

These are not byte-identical schematic/project/BOM copies or proof of fitted parts. Reconcile relevant differences against the production release before board-specific tests. See [Rev 1 board notes](../../../docs/rev-one-board-notes.md) for the earlier machine's source location, comparison details, BOM differences and limits.

Use the repository Rev 1 files for continued development without overwriting the saved manufacturing package. The saved BOM is an assembly reference, not proof of each fitted part; the procurement master CSV is not an as-built record.

## Rev 0 - Manufactured First Build

The physical first-build manufacturing package is outside this repository:

`C:\Users\forch\OneDrive\JLCPCB files\Solar Charger\Solar Project Rev0 - Manufactured First Build`

Open the matching project files only:

- `Solar Project Rev0 - Manufactured First Build.kicad_pro`
- `Solar Project Rev0 - Manufactured First Build.kicad_sch`
- `Solar Project Rev0 - Manufactured First Build.kicad_pcb`

This package is the as-manufactured Rev 0 evidence. Do not use it as the active Rev 1 source.

## Rev 1 - Repository development copies

Repository copies of the Rev 1 design set are in this directory:

- `Solar Project Rev1.kicad_pro`
- `Solar Project Rev1.kicad_sch`
- `Solar Project Rev1.kicad_pcb`

Open `Solar Project Rev1.kicad_pro` when working on Rev 1. The Rev 1 schematic is the active development source; preserve any local uncommitted changes. Distinguish future edits from the identified manufactured baseline.

The September 18-19 handoffs record the Rev 1 closure checks, DFM result, and board order. For manufacturing and bench-test questions, the external production release above takes precedence over repository copies that differ.

## Historical Artifacts

These are retained as dated evidence only. Do not open or modify them for active development:

- `Solar Project Rev1 - 2026-08-04 Snapshot.kicad_sch`
- `Solar Project Rev1 - 2026-08-04 Snapshot.net`
- `Solar Project Rev1 - 2026-07-27 PCB Baseline.kicad_pcb`
- `Solar Project Rev1 - 2026-07-15 Backup.kicad_sch-bak`
- `Solar Project PreRoute save.*`
- `Solar Project Rev0.*` in this directory, which are historical repository copies and not the as-manufactured Rev 0 package.

## File Handling Rules

- Close KiCad and confirm no `~*.lck` files remain before renaming or relocating design files.
- Preserve autosave, lock, cache, and personal-layout artifacts outside commits.
- Never generate fabrication outputs from a snapshot or the Rev 0 manufacturing package when Rev 1 is intended.
- During inspection, KiCad lock files were present in the production release's `Kicad` folder. Confirm KiCad is closed before considering them stale; do not delete them as cleanup.