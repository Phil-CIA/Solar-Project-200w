# Solar Project KiCad Revision Index

Last updated: 2026-08-24

## Rev 0 - Manufactured First Build

The physical first-build manufacturing package is outside this repository:

`C:\Users\forch\OneDrive\JLCPCB files\Solar Charger\Solar Project Rev0 - Manufactured First Build`

Open the matching project files only:

- `Solar Project Rev0 - Manufactured First Build.kicad_pro`
- `Solar Project Rev0 - Manufactured First Build.kicad_sch`
- `Solar Project Rev0 - Manufactured First Build.kicad_pcb`

This package is the as-manufactured Rev 0 evidence. Do not use it as the active Rev 1 source.

## Rev 1 - Active Development

The active Rev 1 design set is in this directory:

- `Solar Project Rev1.kicad_pro`
- `Solar Project Rev1.kicad_sch`
- `Solar Project Rev1.kicad_pcb`
- `Solar Project Rev1.net`

Open `Solar Project Rev1.kicad_pro` when working on Rev 1. The Rev 1 schematic is the active source of truth; its local uncommitted changes must be preserved.

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