# Solar Project KiCad Revision Index

Last updated: 2026-11-20

## Rev2 - copied development starting baseline (not qualified)

Phil selected **retain LM51772 for Rev2** on 2026-11-09 ([DEC-067](../../../docs/decisions-log.md), [#20 decision comment](https://github.com/Phil-CIA/Solar-Project-200w/issues/20#issuecomment-6737135332)). Open this distinct matching set for new Rev2 work:

- `Solar Project Rev2.kicad_pro`
- `Solar Project Rev2.kicad_sch`
- `Solar Project Rev2.kicad_pcb`

Copied from the three `Solar Project Rev1.*` repository development files at merged-main commit **`32243b5789a5f2400ddd37af67e152e77bece215`**, tag **`rev2-development-baseline-2026-11-09`**, after [PR #25](https://github.com/Phil-CIA/Solar-Project-200w/pull/25). This is not a copy of an independently inspected external manufacturing package. Rev1 files and `fab-output-2026-09-18` remain unchanged.

The initial copy changed project identity, schematic project-instance names, PCB `sheetfile` links, title/revision warnings and the BOM output target. Issue #22 now adds a deliberate control-supply change; the Rev2 BOM target remains the project-local `Solar Project Rev2 - DEVELOPMENT ONLY.csv`, not the inherited external production BOM path. The schematic root/component UUIDs and PCB paths are deliberately preserved as matched links in the copied set. Same-basename project/schematic/PCB files select Rev2; no Rev1 filename references remain in Rev2 source.

The files have since acquired sensing, feedback, BOOT0, #22 control-supply and #33 interface changes; they are no longer an electrically identical Rev1 copy. D4 is removed; D6 feeds U4 VIN from `CHG_OUT_POS` through the explicitly named `CTRL_SUPPLY_IN`; D7 feeds `CTRL_5V` directly from USB `VBUS`. The Rev2 title blocks still warn **NOT QUALIFIED; not for fabrication**. No bench rework was imported. Do not generate manufacturing files from this set until the [open design gates](../../../docs/rev-two-consolidation.md#rev2-change-register) are implemented and reviewed; #19, #21, #15 and the other register entries remain unresolved. #20 remains open because its evidence-based comparison was not performed. No BQ25756 evaluation or purchase is requested.

### Baseline structural checks (2026-11-09)

KiCad CLI **10.0.6** successfully exported both schematic netlists and ran `sch erc --format json --severity-all` and `pcb drc --schematic-parity --format json --severity-all` on both sets, without zone refill or board saving. Components (174), library parts (67), libraries (14) and nets (172) are identical after normalizing the intended sheet-name/file metadata. Reversing the documented metadata changes reproduces all three Rev1 source files byte-for-byte; the Rev1 source matches the merged baseline.

Both sets have the same normalized findings: ERC **181 warnings, 0 errors** (133 library issues, 10 footprint-link issues, 1 library-symbol mismatch, 33 pin-to-pin warnings, 4 off-grid endpoints); DRC **12 warnings, 0 errors, 0 unconnected items, 0 schematic-parity issues** (10 footprint-library issues, 2 silk-over-copper warnings). These are inherited findings under this machine's configuration, not newly introduced Rev2 defects or proof of electrical safety. Shared library tables retain historical absolute paths; CLI parsing uses embedded symbols/footprints, but library relinking/update portability is not established. Do not replace historical library mappings or dismiss the other warnings as library-only findings.

### Issue #22 control-supply update (2026-11-16)

The schematic netlist confirms D6.2=`CHG_OUT_POS`, D6.1 and U4.2=`CTRL_SUPPLY_IN`, D7.2=`VBUS`, and D7.1=`CTRL_5V`; D4 is absent. The PCB's D7 pad 1 is assigned to `CTRL_5V`, its U4-input net is named `CTRL_SUPPLY_IN`, and KiCad reports zero schematic-parity issues. With KiCad CLI 10.0.5, compared with the saved pre-change report from the same version, DRC findings decreased from 222 to 221 and unconnected items from 382 to 380; no new DRC violation category was introduced. ERC has 314 warnings and zero errors, versus 316 baseline warnings; the additional single-pin `BAT_BUS_POS` label warning follows removal of D4. These CAD checks confirm connectivity and parity only. Battery-only startup through `CHG_OUT_POS`, USB-only voltage after D7's forward drop, USB biasing of the U4/L3 output, both-source interaction, and diode ratings/thermal margin remain unqualified; no powered tests were performed.

### Issue #33 saved PCB recovery (2026-11-17)

Recovered the saved placement/routing from preserved commit
`f0ccffb47862a661c2661137533926cc4dd4969c`, relative to its parent
`0ec58b745596dd84ac5a93d619e614df93003ae9`, onto main `80a6a0d`.
The current schematic remains the electrical source of truth. The only
three-way conflict was the saved movement of D4 versus its removal in #22;
D4 remains removed. D6/D7/U4 pad nets retain #22, and the recovered sense-net
names were normalized to the current schematic, including its existing
`V_out_SNS_P ` trailing space. No new placement or routing was designed.

KiCad 10.0.6 loads the recovered board. Compared with main under the same
configuration, DRC reports 62 rather than 81 violations, 347 rather than
380 unconnected items, and the same four schematic-parity findings: missing
JP1/JP2 footprints, the feedback-net mismatch and U10's value mismatch.
These counts do not establish electrical correctness. In particular, the
saved source already contains a short between NT8 pad 1 (`Net-(J3-Pin_2)`)
and the `I_BATT_-` track at approximately (146.32, 130.60) mm; recovery
preserves it for the later PCB effort, not as an accepted connection.

The recovered board's Git blob is
`97bad31abc292edf088c00e9710441cf21ff1331`. Its 198 footprint placements
(excluding the deliberately removed D4), 228 track/via geometries and 16
zone identities were compared with the saved source; #22 pad nets were
checked separately. Freeze the PCB at this recovery boundary for the
schematic/footprint-only portion of #33. Do not update it from the edited
schematic, replace footprints, refill/save zones or continue routing in
that portion. Expected schematic/PCB differences must be listed for the
later layout handoff; #33 remains open and this is not a fabrication release.

### Issue #33 schematic/footprint-only handoff (2026-11-17)

The PCB was frozen at the blob above for the issue #33 schematic handoff.
The root schematic includes `Rev2 ESP32 STM32 UART.kicad_sch` and the portable Rev2-only
`Rev2_Interfaces` symbol library. The user-confirmed STM32G473CEU6 uses USART2
PB3/PB4 AF7 for data, with SW2/SW4 moved to PA11/PA12; UART0/USART3 remains a
separate console. U15 TXU0204 adds power-off-safe buffering, local bias/series
parts and test pads. Manual ESP RST/BOOT replaces the earlier controlled-EN
proposal. The separate issue #17 connector replacement is recorded below.

KiCad 10.0.6 exports the hierarchy; netlist checks verify exact endpoints,
unchanged safety pins, all 49 MCU pads and added footprint pad coverage.
All 16 existing module pad coordinates/numbers match the frozen board's
embedded footprint, and Phil confirmed direct physical Rev1 fit. Final ERC
has 52 warnings and zero errors, the same category/count distribution as
the current-main schematic. Read-only DRC remains 62 violations and 347
unconnected items; intentional schematic parity is now 53 rather than 4.
The [handoff](../../../docs/esp32-stm32-firmware-delivery-plan.md#rev2-schematic-only-handoff-issue-33)
enumerates the 26 added components, net/metadata changes, inherited
differences, antenna/service-access gates and firmware pin changes.
No PCB synchronization, placement/routing, firmware or powered test was
performed within this #33 schematic handoff after recovery. #33 remains open.

### Issue #17 keyed SWD connector replacement (2026-11-19)

This separately authorized update replaces redundant J4 with J9 placement/routing
in J4's former area and includes the reset-net rename needed to match the
schematic; unrelated PCB objects remain unchanged. It does not synchronize the
issue #33 schematic changes to the PCB.

J9 is provisionally placed at `(171.12, 135.70) mm`, rotated 90 degrees, and
routed for SWD-only operation. J4's Rev2 symbol/footprint are removed. The keyed
Samtec FTSH-105-01-L-DV-007-K candidate uses the project-local
`Rev2_Debug_Footprints` library and manufacturer-based land pattern.

KiCad 10.0.6 DRC with zone refill reports 60 violations (14 errors, 46 warnings)
and 344 unconnected items, versus 62 violations (16 errors, 46 warnings) and
347 unconnected items before replacement. There are no new physical findings;
removing J4 eliminates two copper-edge errors. J9 has no physical DRC or
unconnected-item findings. Schematic parity remains at 86 issues; the only J9
finding is the intentional missing pad 7 key. Schematic ERC reports 0 errors
and 55 warnings.

Actual probe/cable mating, any external 2.54 mm adapter mapping, enclosure
access, and bench attach/program validation remain open. See the
[connector pinout and release checks](../../../docs/rev2-swd-debug-connector.md).
Issue #17 remains open; this is not a fabrication or bench acceptance.

### Rev2 PCB synchronization and placement handoff (2026-11-20)

The PCB-order PR incorporates current main `d61b845` and synchronizes the
approved schematic packets to the controller PCB. The earlier PCB freezes
above describe historical stage boundaries, not this new handoff.

The board now contains all **227 schematic components**, adding the 29 previously
missing footprints. R75 uses the selected Susumu long-side-terminal footprint;
R78/R79 use the selected Yezhan two-pad land pattern. Component values, sourcing
fields, schematic links and pad nets are reconciled, including U1's corrected
STM32G473CEU6 identity, the separate ESP supply domain, UART/button changes,
feedback jumpers and battery-voltage divider. The physical key position at J9
still deliberately has no pad 7.

**Phil owns physical placement.** Newly added parts are staged outside the
outline to the right at x=214-250 mm / y=46-116 mm. This is a synchronization
handoff, not completed placement/routing or an order package. Review the
replacement shunt pads as well as the new parts before routing.

All 198 pre-existing component centers, orientations and sides are preserved.
The 259 track/via and 16 zone identities are retained; only the known shorting
end of `I_BATT_-` track `4dc76fe0-a257-4ab2-b935-738aea01b59e` is trimmed to
NT8 pad 2. Zone boundaries and the outline are unchanged; zones are refilled
and saved. Schematic endpoint matching and CAD UUID uniqueness were checked.
Rev1 and its fabrication package are unchanged.

KiCad 10.0.6 now reports **59 physical DRC findings (13 errors, 46 warnings)**,
**394 unconnected items**, and **one schematic-parity finding** for J9's
intentionally absent key pad 7. The 29 added parts explain the increased
unconnected count; their routing is not complete. All missing-component,
value/field and required pad-net parity findings are resolved. No shorting-item
finding remains. The remaining errors are twelve INA241 pad-clearance
findings (0.15 mm pad gaps against the configured 0.20 mm rule) and the existing
C3/C18 courtyard overlap; they have not been waived.

The [current walkthrough](../../../docs/rev-two-change-plan.md#current-implementation-handoff---2026-11-20)
records the remaining placement, electrical review, routing and fabrication
checks. DEC-076 finalizes the fuse by disposition; DEC-077 reserves the future
BMS connection at existing J8 I2C without making the TBD model a prototype-order
hold. Neither decision establishes powered compatibility or operation.

**Do not manufacture this staged board.** No new controller Gerbers/drills or
order ZIP are generated by this synchronization.

Phil's saved placement edits were subsequently recovered from the issue #17
worktree without replacing the synchronized PCB. The transfer applied 43
positions/orientations (21 staged and 22 existing footprints); TP19-TP26
remain staged. No source project settings or track deletion were imported.
After placement recovery, KiCad reports 90 physical findings (13 errors,
77 warnings), 395 unconnected items and the same intentional J9 key-pad
parity finding. Routing and fabrication checks remain open.

Phil subsequently excluded TP19-TP26 from this Rev2 board. Their schematic
symbols and standalone stubs and their PCB footprints were removed; the
UART circuits and other test points are unchanged. The PCB now has 219
schematic-linked footprints, with no placement or routing changes in this pass.

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