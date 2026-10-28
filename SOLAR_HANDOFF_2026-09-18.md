# Solar Project Session Handoff - 2026-09-18

## Session Closeout & Status

Short session, high-value outcome: **Track E (Gate 4 PCB layout) is closed.** Phil completed the full floorplan/route pass on `Solar Project Rev1.kicad_pcb` since the last session (4-layer stackup: F.Cu/In1.Cu/In2.Cu/B.Cu). This session's work was verification and cleanup, not new layout: reviewed the closure-plan status, ran ERC/DRC, found and fixed a real schematic metadata defect, confirmed the two remaining DRC warnings were benign, generated fab-ready Gerbers/drill files, and Phil ran those through JLCPCB's own DFM checker with no issues flagged.

**Bottom line: Tracks A, B, C, and E are all now closed. Rev 1 has met every Definition-of-Done criterion in `docs/rev-one-closure-plan-2026-09-16.md` and is ready to place the initial board order.** The only work still open is explicitly non-blocking for the order itself (see below).

---

## What happened this session

### 1. Status review and closure-plan refresh

Started by reviewing where the project stood: confirmed (via `git log`/`git diff`, repo memory, and `SOLAR_HANDOFF_2026-09-17.md`) that Tracks A/B/C were closed as of yesterday but not yet committed, and that the PCB file's only on-disk changes at that point were the `J8` connector footprint syncing in from the schematic (DEC-061) plus a handful of pre-existing traces - Track E had **not** actually been started yet. Updated `docs/rev-one-closure-plan-2026-09-16.md` with a new Section 0 ("Path to Ordering the Board") and refreshed track status banners so the doc matched reality (Tracks A/B/C closed, Track E identified as the one real remaining gate).

### 2. ERC/DRC due diligence, using "our SOP" at a scope appropriate to the finding

Phil asked to do the DRC/ERC due-diligence pass "using our SOP" (the multi-task-packet pattern from `docs/track-b-bom-sourcing-tasks.md`). Ran the automated checks first to see how much was actually wrong before committing to a heavier multi-session process:

- **ERC** (`kicad-cli sch erc`): 0 errors, 337 warnings.
- **DRC** (`kicad-cli pcb drc --severity-all`): 0 errors, 0 unconnected items, 168 warnings.

Given the findings were small in number and concrete, Phil chose direct resolution over spinning up the full multi-model SOP process (a "no, DRC/ERC clean is enough" call) - consistent with the accepted hybrid pattern from the 2026-09-17 session (SOP for large sweeps, direct resolution for small findings).

### 3. Root-cause fix: duplicate `"LCSC "` (trailing-space) schematic field (DEC-064)

Phil correctly identified that most of the ERC/DRC noise traced back to a duplicated metadata field. Investigation confirmed: **172 of the schematic's symbol instances carried two different-looking `LCSC` fields** - the real `"LCSC"` field (holds the actual part number) and a second, always-empty field literally named `"LCSC "` (trailing space) that KiCad treats as a distinct field name. This was the direct cause of 166 `field_name_whitespace` ERC warnings.

Verified all 172 instances were empty before touching anything, then removed each block with a small paren-balanced Python script (safer than 172 manual edits given each block's `(at ...)` coordinates differ). Verified the fix three ways:
- Paren-balance check on the edited file (unchanged, still valid s-expression).
- Re-ran ERC: 337 -> 171 warnings, with **`field_name_whitespace` dropping to exactly 0** and every other violation category (footprint links, pin-to-pin, endpoint-off-grid, lib-symbol issues) completely unchanged - strong evidence the fix was purely cosmetic with zero netlist/connectivity impact.
- Diffed a before/after netlist export (`kicad-cli sch export netlist --format kicadsexpr`) to further confirm no electrical change.

Net result: **1892 lines deleted from `Solar Project Rev1.kicad_sch`**, zero functional change. Logged as DEC-064.

### 4. Confirmed the remaining DRC/ERC noise is benign

After the fix, remaining items are all non-issues:
- 131 `lib_footprint_issues`/`footprint_link_issues` (ERC+DRC) - a headless-CLI artifact (`kicad-cli` on this machine doesn't have the project's `PCM_JLCPCB` footprint library table configured), not a real project defect.
- 33 `pin_to_pin`, 4 `endpoint_off_grid`, 2 `lib_symbol_issues`, 1 `lib_symbol_mismatch` (Q4's `Q_PMOS` symbol differs slightly from the stock `Device` library copy) - pre-existing, minor, non-blocking.
- **2 `silk_over_copper`** on U6 pin 1, both literally described by KiCad as `"Silkscreen clipped by solder mask"` - an informational note (not a defect) confirming the fab automatically clips that silkscreen segment at the solder-mask opening. Phil's own assessment matched this exactly.
- Earlier in this same working session (before this handoff) a large stray 32x32mm silkscreen rectangle and a small `V_out_sense` dangling-track stub had also been flagged - both are gone from the final DRC run, addressed directly by Phil in the GUI.

### 5. Fab output generation and JLCPCB DFM check (DEC-065)

Confirmed the board is a 4-layer stackup (F.Cu/In1.Cu/In2.Cu/B.Cu). Generated a full JLCPCB-format fab output set in `hardware/kicad/solar-project/fab-output-2026-09-18/`:
- 11 Gerber layers (F/In1/In2/B copper, paste, silkscreen, mask, edge cuts) with Protel filename extensions.
- 1 combined Excellon drill file (`.drl`) + a drill map PDF for reference.
- A `.gbrjob` job file.
- Zipped flat (no subfolders, matching JLCPCB's expected format) as `Solar-Project-Rev1-Gerbers-2026-09-18.zip`.

Phil uploaded the zip to JLCPCB's own DFM checker on their website: **no real issues noted.** Logged as DEC-065 - this is the formal close of Track E and the last Definition-of-Done gate for "Rev 1 ready to fabricate."

### Docs updated this session

- `docs/decisions-log.md` - DEC-064, DEC-065 added; header line updated.
- `docs/rev-one-closure-plan-2026-09-16.md` - Section 0 rewritten to reflect Track E closure; Track E checklist marked complete; Section 3 (near-term sequencing) and Section 4 (Definition of Done) updated to show Rev 1 has met every DoD criterion.
- `hardware/kicad/solar-project/Solar Project Rev1.kicad_sch` - 172 duplicate empty `"LCSC "` fields removed (DEC-064).
- `hardware/kicad/solar-project/fab-output-2026-09-18/` - new fab-output directory (Gerbers, drill, job file, zip) - not committed to git (regeneratable from source), kept locally for reference/re-upload.

---

## Current Open Items (all explicitly non-blocking for the board order)

1. **C18 output-cap ripple-current rating** (Track A, bench-only) - still needs either a clean datasheet or a bench measurement; genuinely can't be closed by more desk research.
2. **L2 core-loss / R16 pulse-energy** (Track A, bench-only) - same, graphical-only datasheet curves.
3. **R28** (51.7kOhm resistor, Track B) - no in-stock exact-match LCSC part; source from Phil's existing DigiKey channel at order time.
4. **J8 remote sensor part** (Track B/C) - Adafruit MCP9808 breakout (PID 1782) proposed, needs Phil's final go-ahead and a mating-cable decision.
5. **Track D Rev 0 evidence mining** - not yet started this cycle; **this is the next session's focus per Phil's direction** (see below).
6. **Track F firmware** - not started, includes the DEC-054 parasitic-load-control requirement (needed before real cold-weather PV exposure, not before ordering bare boards).
7. **Uncommitted working tree** - all of today's and yesterday's changes (BOM, order guide, decisions log, closure plan, schematic cleanup, routed PCB) are still uncommitted as of this handoff. Commit before or alongside placing the order so the ordered revision matches a real commit.

---

## Next Session Objective: Track D - Rev 0 Evidence Mining

Phil's explicit direction for the next session: **look at the Rev 0 remaining testing (Track D)**, not Track E (which is now closed) or a new Track. Per `docs/rev-one-closure-plan-2026-09-16.md` Track D, the open items are:

- [ ] Finish the `VCC2` backfeed isolation test: fully disconnect USB and ST-LINK, confirm the rail collapses to near 0V. Started 2026-09-04, never completed.
- [ ] Isolate the FP4 current-limit stall (input current-limited at ~230-266mA, output only ~1-1.36V) independent of the inrush-collapse explanation. Check `MODE`/`ISET`/`CDC` strap behavior and soft-start timing at low bench voltage.
- [ ] Confirm full analysis of `rigol_scope_capture_2026-09-07.txt` (switching frequency, gate signal integrity, ripple) is captured somewhere durable, not just eyeballed once.
- [ ] Use Rev 0 to shake out firmware startup-state-machine logic at bench-safe power before Rev 1 boards arrive, decoupling firmware risk from the new hardware spin.

**Before proposing or running any Rev 0 bench work, re-read the September 8 safety restrictions referenced in `docs/rev-one-closure-plan-2026-09-16.md` Track D** and preserve all safety, identity, probing, and no-`CLEAR_FAULTS` restrictions. PV remains off and disconnected at this handoff.

---

## Important Working Rules Carried Forward

- `docs/decisions-log.md` now runs through **DEC-065** in the file itself.
- Before any KiCad edit, check for `.lck` files AND confirm no `kicad.exe` process is actually running (`Get-Process`) before treating a lock file as stale - but note: **read-only `kicad-cli` checks (ERC/DRC/netlist export/gerber export) are safe to run even while KiCad has the project open**, since they only read the saved on-disk file and write separate report/output files.
- When investigating "why are there so many ERC/DRC warnings," check for **duplicate/near-duplicate property field names** (e.g. trailing whitespace) before assuming every warning represents a distinct real issue - one root cause can generate a large fraction of the total count. Confirm affected fields are actually empty/redundant before bulk-removing them, and re-run ERC/DRC afterward to confirm only the expected violation category changed.
- KiCad's own DRC message text `"Silkscreen clipped by solder mask"` is informational, not a defect - it confirms the fab will auto-clip that silkscreen at the mask opening.
- `kicad-cli pcb export gerbers`/`export drill` with `--layers "F.Cu,In1.Cu,In2.Cu,B.Cu,F.Paste,B.Paste,F.Silkscreen,B.Silkscreen,F.Mask,B.Mask,Edge.Cuts"` and Excellon decimal-mm drill output produces a JLCPCB-ready file set with the expected Protel extensions; zip flat (no subfolders) for upload.
- Live LCSC parametric search is NOT reliably fetchable via `fetch_webpage` - use the browser tool for keyword/value searches.
- **PV array wiring is 2P (parallel), not 2S (series)** - see DEC-051.
- **F1 stays on the Keystone `3522-2` clip** (DEC-054) - do not propose swapping it again without new information.
- **Battery-pack temperature sensing is connector `J8`** (remote I2C sensor tap, DEC-061), not an NTC/ADC channel.
- Do not re-propose running the U14/Q5/Q6 gate test on Rev 0 - confirmed impossible in the 2026-09-17 session (different discrete gate topology on Rev 0).
- Watch for row/designator-mixup errors when copying data between dense tables - recurred 6+ times across this project.

PV remains off and disconnected at handoff.
