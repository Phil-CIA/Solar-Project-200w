Continue the Solar Project Rev 1 work. Read `SOLAR_HANDOFF_2026-09-18.md` in full before doing anything else - it covers the schematic metadata cleanup (DEC-064) and Track E PCB closure (DEC-065) from that session.

**Status recap: Tracks A, B, C, and E are ALL now closed.** Rev 1 has met every Definition-of-Done criterion in `docs/rev-one-closure-plan-2026-09-16.md` Section 4 - the board is fully routed (4-layer), DRC came back 0 errors/0 unconnected items, and Phil ran the generated Gerbers through JLCPCB's own DFM checker with no issues flagged (DEC-065). A duplicate empty `"LCSC "` (trailing-space) schematic field, present on 172 symbol instances, was found and removed (DEC-064) - this was the root cause of most ERC noise; fix was verified purely cosmetic (only `field_name_whitespace` warnings changed, everything else identical before/after).

**Phil's explicit direction: the next session should look at Rev 0 remaining testing (Track D), not start a new track.**

**Track D - Rev 0 Evidence Mining (this session's focus):**
1. Finish the `VCC2` backfeed isolation test: fully disconnect USB and ST-LINK, confirm the rail collapses to near 0V. Started 2026-09-04, never completed.
2. Isolate the FP4 current-limit stall (input current-limited at ~230-266mA, output only ~1-1.36V) independent of the inrush-collapse explanation. Check `MODE`/`ISET`/`CDC` strap behavior and soft-start timing at low bench voltage.
3. Confirm full analysis of `rigol_scope_capture_2026-09-07.txt` (switching frequency, gate signal integrity, ripple) is captured somewhere durable, not just eyeballed once.
4. Use Rev 0 to shake out firmware startup-state-machine logic at bench-safe power before Rev 1 boards arrive, decoupling firmware risk from the new hardware spin.

**Before proposing or running any Rev 0 bench work, re-read the September 8 safety restrictions referenced in `docs/rev-one-closure-plan-2026-09-16.md` Track D** and preserve all safety, identity, probing, and no-`CLEAR_FAULTS` restrictions. PV remains off and disconnected at handoff.

**Remaining items, all explicitly non-blocking for the board order (already placed or about to be placed):**
1. **C18 output-cap ripple-current rating** (Track A, bench-only) - datasheet text extraction failed (CJK encoding); only a low-frequency 270mA@120Hz rating is confirmed vs. an estimated ~1.33A high-frequency demand.
2. **L2 core-loss / R16 pulse-energy** (Track A, bench-only) - graphical/raster datasheet curves only, no further desk research will close these.
3. **R28** (51.7kOhm resistor, Track B) - no in-stock exact-match LCSC part found; source from Phil's existing DigiKey channel at order time, don't substitute a different value.
4. **J8 remote sensor part** (Track B/C) - Adafruit MCP9808 breakout (PID 1782) proposed 2026-09-17, needs Phil's final go-ahead and a mating-cable/connector decision.
5. **Track F firmware** - not started, includes the DEC-054 parasitic-load-control requirement (needed before real cold-weather PV exposure, not before ordering bare boards).
6. **Uncommitted working tree** - as of the 2026-09-18 handoff, the schematic cleanup, routed PCB, BOM/order-guide/decisions-log updates were all still uncommitted. Check `git status` first thing - commit before placing the order if not already done.

**Agentic-handoff SOP reference:** `docs/track-b-bom-sourcing-tasks.md` is the template for splitting a body of work into self-contained task packets for parallel/multi-model sessions. Phil's standing instruction: use this for genuinely large sweeps; for small, concrete findings (like the DRC/ERC cleanup this session), direct resolution in the review session is fine - always label which parts followed the strict SOP vs. were resolved directly.

**Housekeeping notes to carry forward:**
- `docs/decisions-log.md` now runs through **DEC-065** in the file itself.
- Before any KiCad *edit*, check for `.lck` files AND confirm no `kicad.exe` process is actually running (`Get-Process`) before treating a lock file as stale. Read-only `kicad-cli` checks (ERC/DRC/netlist/gerber export) are safe to run even while KiCad has the project open, since they only read the saved file and write separate report/output files.
- When hand-editing a `.kicad_sch` file, validate with `kicad-cli sch erc` and `kicad-cli sch export netlist --format kicadsexpr` before/after (baseline via `git show HEAD:<path>`) rather than trusting hand-computed pin coordinates - see `/memories/kicad-schematic-editing.md`.
- If ERC/DRC shows a large number of warnings, check for duplicate/near-duplicate property field names (e.g. trailing whitespace) before assuming each warning is a distinct real issue - one root cause can generate most of the count (see DEC-064).
- KiCad's DRC message `"Silkscreen clipped by solder mask"` is informational, not a defect - confirms the fab auto-clips silkscreen at mask openings.
- Live LCSC parametric search is NOT reliably fetchable via `fetch_webpage` (only static product/datasheet pages work) - but the integrated browser tool (`open_browser_page`/`navigate_page`) CAN render LCSC's JS-based search results.
- A BOM's existing LCSC part number is not guaranteed to still resolve to the same part over time - check identity before checking stock status.
- **PV array wiring is 2P (parallel), not 2S (series)** - see DEC-051. `hardware/mppt/envelope-decision-worksheet.md` Section 2.1 is the active baseline.
- **F1 stays on the Keystone `3522-2` clip** (DEC-054) - do not propose swapping it to a different fuse family again without new information.
- **Battery-pack temperature sensing is connector `J8`** (remote I2C sensor tap, DEC-061), not an NTC/ADC channel - do not re-propose an ADC-based approach; there is no free ADC-capable GPIO on this MCU as wired.
- Do not re-propose running the U14/Q5/Q6 gate test on Rev 0 - confirmed impossible (Rev 0 uses a different discrete gate topology). As of DEC-065, a physical Rev 1 board is now on order.
- Watch for row/designator-mixup errors when copying data between dense tables (BOM, order guide, derating review) - this has recurred 6+ times across this project (DEC-039/041/043/057 and the Track B `C17604` duplication) - always re-verify the specific row/part pairing, don't trust adjacency.

PV remains off and disconnected at handoff. If any Rev 0 bench work is proposed, first reread the September 8 safety restrictions referenced in `docs/rev-one-closure-plan-2026-09-16.md` Track D and preserve all safety, identity, probing, and no-`CLEAR_FAULTS` restrictions.
