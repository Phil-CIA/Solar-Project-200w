# Solar Project KiCad Revision Index

Last updated: 2026-10-24

## Current production source - Rev 1

Only Rev 0 and Rev 1 exist. The Rev 1 production release sent to the manufacturer is the source of truth at:

`C:\Users\user\OneDrive\JLCPCB files\Solar Charger\Rev 1`

It contains `Kicad`, `BOM`, and `GERBERS` folders. Use that package to identify and test the production board. The repository PCB and Gerbers are byte-identical to the release, but its schematic, project, and BOM copies differ from the release. Do not use those differing copies to infer the as-manufactured design until they have been reconciled against the release.

## Rev 0 - Manufactured First Build

The physical first-build manufacturing package is outside this repository:

`C:\Users\forch\OneDrive\JLCPCB files\Solar Charger\Solar Project Rev0 - Manufactured First Build`

Open the matching project files only:

- `Solar Project Rev0 - Manufactured First Build.kicad_pro`
- `Solar Project Rev0 - Manufactured First Build.kicad_sch`
- `Solar Project Rev0 - Manufactured First Build.kicad_pcb`

This package is the as-manufactured Rev 0 evidence. Do not use it as the active Rev 1 source.

## Rev 1 - Production release

Repository copies of the Rev 1 design set are in this directory:

- `Solar Project Rev1.kicad_pro`
- `Solar Project Rev1.kicad_sch`
- `Solar Project Rev1.kicad_pcb`

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