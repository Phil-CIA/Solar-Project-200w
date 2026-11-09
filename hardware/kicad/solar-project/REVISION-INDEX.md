# Solar Project KiCad Revision Index

Last updated: 2026-11-09

## Rev2 - copied development starting baseline (not qualified)

Phil selected **retain LM51772 for Rev2** on 2026-11-09 ([DEC-067](../../../docs/decisions-log.md), [#20 decision comment](https://github.com/Phil-CIA/Solar-Project-200w/issues/20#issuecomment-6737135332)). Open this distinct matching set for new Rev2 work:

- `Solar Project Rev2.kicad_pro`
- `Solar Project Rev2.kicad_sch`
- `Solar Project Rev2.kicad_pcb`

Copied from the three `Solar Project Rev1.*` repository development files at merged-main commit **`32243b5789a5f2400ddd37af67e152e77bece215`**, tag **`rev2-development-baseline-2026-11-09`**, after [PR #25](https://github.com/Phil-CIA/Solar-Project-200w/pull/25). This is not a copy of an independently inspected external manufacturing package. Rev1 files and `fab-output-2026-09-18` remain unchanged.

Only project identity, schematic project-instance names, PCB `sheetfile` links, title/revision warnings and the BOM output target changed. The Rev2 BOM target is the project-local `Solar Project Rev2 - DEVELOPMENT ONLY.csv`, not the inherited external production BOM path. The schematic root/component UUIDs and PCB paths are deliberately preserved as matched links in the copied set. Same-basename project/schematic/PCB files select Rev2; no Rev1 filename references remain in Rev2 source.

Circuit values, connectivity, routing, copper and silkscreen artwork are still the copied Rev1 design. The Rev2 title blocks say **copied baseline, NOT QUALIFIED; not for fabrication**. No bench rework was imported. Do not generate manufacturing files from this set until the [open design gates](../../../docs/rev-two-consolidation.md#rev2-change-register) are implemented and reviewed; #19, #21, #15 and the other register entries remain unresolved. #20 remains open because its evidence-based comparison was not performed. No BQ25756 evaluation or purchase is requested.

### Baseline structural checks (2026-11-09)

KiCad CLI **10.0.6** successfully exported both schematic netlists and ran `sch erc --format json --severity-all` and `pcb drc --schematic-parity --format json --severity-all` on both sets, without zone refill or board saving. Components (174), library parts (67), libraries (14) and nets (172) are identical after normalizing the intended sheet-name/file metadata. Reversing the documented metadata changes reproduces all three Rev1 source files byte-for-byte; the Rev1 source matches the merged baseline.

Both sets have the same normalized findings: ERC **181 warnings, 0 errors** (133 library issues, 10 footprint-link issues, 1 library-symbol mismatch, 33 pin-to-pin warnings, 4 off-grid endpoints); DRC **12 warnings, 0 errors, 0 unconnected items, 0 schematic-parity issues** (10 footprint-library issues, 2 silk-over-copper warnings). These are inherited findings under this machine's configuration, not newly introduced Rev2 defects or proof of electrical safety. Shared library tables retain historical absolute paths; CLI parsing uses embedded symbols/footprints, but library relinking/update portability is not established. Do not replace historical library mappings or dismiss the other warnings as library-only findings.

## Current production source - Rev 1

Only Rev 0 and Rev 1 have been manufactured. The Rev 1 production release sent to the manufacturer is the source of truth at:

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

Open `Solar Project Rev1.kicad_pro` when working specifically on Rev 1. Preserve these files and any local uncommitted changes as the Rev1 development reference; new Rev2 design work belongs in the separate Rev2 set above. Distinguish development copies from the identified manufactured baseline.

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