# Solar Project Session Handoff - 2026-09-17

## Session Closeout & Status

This was a long, high-output session in six phases. **Phase 1** closed the F1/F2 fuse-clip metadata defect and set provisional bench fuse sizes. **Phase 2** completed a full Track A (Gate 1 component derating) pass, then a user-supplied panel-spec correction reopened three rows as urgent, safety-relevant findings. **Phase 3** resolved that entire urgent chain end-to-end - the project is no longer blocked on a safety-critical open item. **Phase 4** closed out essentially every remaining Track A row: real LCSC catalog numbers for C7/C52/D2 (DEC-058), the J1/J3 connector part-identity question (DEC-059), and the R30 CDC strap ambiguity (DEC-060, via a schematic edit). **Phase 5** reviewed and closed Track C (safety/sensing gaps): decided and implemented a dedicated battery-pack temperature input (DEC-061, a new I2C-bus-tap connector `J8`) and formally deferred the BMS communication link to Rev 2 (DEC-062). **Phase 6** (this update) closed Track B (BOM/sourcing sweep) using a new multi-model agentic-handoff SOP (see below) - DEC-063. Three KiCad schematic edits were made this session total (R30 value/part fields in Phase 4; the new `J8` connector + 4 global labels in Phase 5) - all other changes are docs/BOM. No PCB layout work was started; PV remains off/disconnected.

**Bottom line: Track A, Track B, and Track C are all now closed (Track A effectively, Track B and C fully).** Remaining open items are two genuinely bench-only Track A items (C18 ripple-current rating, R16/L2 pulse-energy/core-loss), one firmware backlog item (DEC-054's parasitic-load mitigation), R28 (51.7kOhm resistor, no in-stock exact match), and the final J8 remote sensor part confirmation. **The project is ready to move to Track E (Gate 4 PCB layout) and/or Track D (Rev 0 evidence mining).**

---

## Phase 6: Track B BOM/sourcing sweep closed - new agentic-handoff SOP piloted (DEC-063)

**New process, not just a new result:** built `docs/track-b-bom-sourcing-tasks.md`, a reusable SOP template for splitting a body of work into self-contained task packets, each with its own ground-truth context, exact files to touch, a definition of done, an assigned model, and a paste-ready handoff prompt for a fresh chat session. Every executing agent appends a standardized Efficiency Self-Report (outcome, tool-call counts, web-fetch success rate, blockers, times it had to ask the user a question, confidence, and "what the instructions should have said") so multiple models/tasks are comparable over time. This is intended as the template for future multi-task handoffs, not a one-off.

**Results:** all 5 tasks executed (Task 1 GPT-5 mini, Task 2 Claude Sonnet 4.5, Task 3 Claude Sonnet 4.5, Task 4 GPT-5 mini, Task 5 GPT-5). Findings included a genuinely unresolvable item (U2 is a generic unbranded "ESP32-C3 Super Mini" clone board with no real LCSC listing at all) and a new risk class worth remembering: an existing BOM's LCSC part number can silently remap to a completely different live part over time, not just go in/out of stock - identity-check first, then stock-check.

**Process deviation, explicitly accepted going forward:** after reviewing the 5 results, Phil asked to "fold and move on to resolving" the remaining 5 small open sub-items (U2 sourcing channel, approving 9 passive substitutions, R28, J6, J8 sensor pick) - these were resolved directly in the review session rather than re-run through separate agent sessions per the SOP. Phil's explicit instruction: keep using this hybrid pattern (SOP-driven first pass, then direct resolution of small leftovers in the review session) **until a better approach is found** - this is now accepted practice, not a lapse, but it's flagged in the task file so this batch isn't mistaken for a clean multi-model comparison data point.

**A real mistake caught mid-process:** while writing the approved substitutions into the BOM, the reviewing agent pasted the same fabricated/duplicate LCSC C-number (`C17604`) onto two different resistor rows (`R7/R10/R61` and `R14`) - caught on self-review before reporting done, corrected to bare manufacturer part numbers with an explicit "still needs a final lookup pass" note where no verified C-number existed. Same recurring error class as the DEC-039/041/043/057 row/designator mixups from earlier in this project - worth calling out explicitly in future task packets since it keeps recurring across both human and agent passes.

**Final Track B disposition:**
- `J6` confirmed: Amphenol `20021121-00010C4LF` = LCSC `C150517`, exact MPN match, in stock.
- `U1`: keep `STM32G431CBU6`, source from Mouser/DigiKey (Phil's decision).
- `U2`: confirmed generic clone board, source outside LCSC (AliExpress/Amazon).
- 9 of 10 flagged passive substitutions (C20, R7/R10/R61, R14, R15, R20, R23, R24/R62, R29, R31, R51) written into `docs/rev-one-bom.md` with live-verified in-stock replacements.
- `R28` (51.7kOhm, 0805, 1%) stays genuinely open - no in-stock exact match found on LCSC; source from Phil's existing DigiKey channel at order time.
- `J8` remote sensor: proposed a specific candidate (Adafruit MCP9808 breakout, PID 1782, address `0x18`) - final confirmation still open.

**Docs updated this phase:** `docs/rev-one-bom.md` (all Track B rows), `docs/rev-one-lcsc-order-guide.md` (§4 reconciliation addendum), `docs/track-b-bom-sourcing-tasks.md` (new SOP file + 6 self-reports), `docs/decisions-log.md` (DEC-063).

---

## Phase 5: Track C safety-scope decisions - decided and implemented (DEC-061, DEC-062)

### DEC-061 - Added a dedicated battery-pack temperature-sensing input to Rev 1

Phil chose to build this now (schematic-only change, still cheap pre-Gate-4) rather than defer or document BMS-only reliance. **The original design plan changed mid-implementation**: the assumption that MCU `PA6`/`PA7` were spare (per the old 2026-07-10 signal map, and confirmed by a plain-text grep that only found the pins' `lib_symbols` definitions) turned out to be wrong. Exporting the actual netlist with `kicad-cli sch export netlist` proved both pins are already wired to USART1 `CTS`/`RTS` (DEC-015, to `U3`) via global labels - a real conflict invisible to a literal text search. A full sweep of every `U1` netlist node further showed **every `PA`/`PB` GPIO is already in use** except `PB8` (shared with `BOOT0`, unsuitable to repurpose) - there is no free ADC-capable pin on this MCU as currently wired.

**Design pivot:** instead of a new ADC channel with an NTC/resistor divider, added a new 4-pin connector `J8` (`Connector_Generic:Conn_01x04`, same footprint family as `J4`) tapping the *existing* shared I2C bus (`I2C CLK`/`I2C DATA`, already carrying `U6`/`U7`/`U8`) plus `CTRL_3V3`/`PWR_NEG`, for a remote I2C temperature-sensor module (e.g. `MCP9808`-class, at a distinct I2C address from `U7`'s AHT20 default `0x38`) mounted at/near the battery pack. No new MCU pin needed at all - the existing bus pull-ups serve any additional device.

**Note:** the new connector is `J8`, not `J5` - `J5` already exists as a stale BOM row (`Conn_01x03_Socket`, a different 3-pin part) that isn't actually present in the current schematic/netlist; reusing it would have created a designator collision, so `J8` (the true next-free designator, confirmed against both the schematic and the BOM) was used instead.

**Validation method (new for this project):** edited `Solar Project Rev1.kicad_sch` directly via raw text edit after confirming `~Solar Project Rev1.kicad_sch.lck`/`.kicad_pro.lck` reflected a closed KiCad session. Rather than trust hand-computed pin coordinates, validated with `kicad-cli sch erc` and `kicad-cli sch export netlist --format kicadsexpr` before (via a `git show HEAD:...` baseline) and after the edit: 0 new ERC errors (only 1 pre-existing, unrelated baseline error - a hidden pin on Q4 - and one new cosmetic `endpoint_off_grid` warning on `J8` pin 1, harmless). The first attempt at label placement actually put two labels on the wrong pins (a naive local-coordinate calculation didn't match the connector symbol's real pin layout); this was caught immediately via the netlist re-export and ERC's `pin_not_connected`/`label_dangling` messages, then corrected and re-verified - confirmed final mapping: `J8` pin1=`I2C CLK`, pin2=`PWR_NEG`, pin3=`CTRL_3V3`, pin4=`I2C DATA`.

**Still open:** exact remote I2C sensor module part/address and mating cable/connector (BOM/order-time item); the cosmetic `endpoint_off_grid` ERC warning on `J8` pin 1 (nudge the symbol onto grid next time the schematic is open in the GUI - purely cosmetic, not an electrical issue).

### DEC-062 - Deferred the BMS communication link to Rev 2

Phil chose to defer the full link, and even a minimal permissive/fault GPIO input, rather than add any new hardware this revision, since no specific BMS part/signal has been selected yet. Rev 1 continues to rely solely on the Q-001 `>=20A` BMS current-capability assumption. Added `REQ-BAT-003` to `requirements.md` and updated `architecture.md` (Section 3.3/3.6, interface table, deferred-decisions list) to state this explicitly as a documented scope boundary, not a silent gap.

### Docs updated this phase

- `docs/decisions-log.md`: added DEC-061, DEC-062, and backfilled DEC-058/059/060 (previously only in repo memory) as formal log entries.
- `docs/requirements.md`: REQ-TEL-002 reworded, new `REQ-BAT-003`, traceability matrix and open-items list updated.
- `docs/architecture.md`: Section 3.3 (Battery and Protection Stage), Section 3.6 interface table, and Section 6 (Deferred Architectural Decisions) updated.
- `docs/rev-one-closure-plan-2026-09-16.md`: Track C section marked decided-and-implemented; Definition of Done updated.
- `docs/rev-one-bom.md`: new `J8` BOM line added (item 31a), status/count header updated.
- Repo memory `/memories/repo/mppt-power-path.md` and new `/memories/kicad-schematic-editing.md` (user-scope, generalizable KiCad raw-edit validation technique).

### A note on a GAL backend / blank-canvas scare (resolved, not a real issue)

After this edit, Phil's KiCad GUI briefly showed a blank canvas, then an "Error switching GAL backend" error on reopen. This turned out to be caused by a stale KiCad session/lock state from Phil's own earlier editing (not a file-corruption problem) - `kicad-cli sch erc`/`export netlist`/`export pdf` all ran clean against the file throughout, and parenthesis balance/UUID-uniqueness checks were clean too. Fix: confirmed no `kicad.exe`/`eeschema.exe` process was still running (Task Manager / `Get-Process`), then deleted the stale `~Solar Project Rev1.kicad_sch.lck` and `~Solar Project Rev1.kicad_pro.lck` files. Reopening then worked and Phil confirmed the `J8` edit is visible. **Lesson for next time:** if a KiCad GUI shows a graphics/backend error right after an on-disk edit, check for lock files and lingering KiCad processes before suspecting the file content itself.

---

## Phase 4: Closing out the last Track A catalog-number and ambiguity items (DEC-058 through DEC-060)

### DEC-058 - Real LCSC catalog numbers confirmed for C7, C52, D2

Phil supplied candidate/on-hand LCSC part numbers; verified each via live LCSC product-page fetch (static product-detail pages ARE fetchable, unlike dynamic parametric search). All three were **exact matches** to the targets from DEC-055/056/057:
- **C7** = YAGEO `CC1206KKX7R0BB225` (`C577211`): 2.2uF/100V/X7R/1206
- **C52** = Samsung `CL21B224KBFNNNE` (`C5378`): 220nF/50V/X7R/0805
- **D2** = Littelfuse `SMBJ48CA` (`C151264`): Vrwm=48V/Vc=77.4V@Ipp=7.8A/DO-214AA

All three written into `docs/rev-one-bom.md`. This closes the last open BOM-sourcing gaps from DEC-055/056/057.

### DEC-059 - J1/J3 connector part-identity question closed as a bring-up/prototype disposition

Checked the actual schematic (not just the BOM's grouped row): the two identical `Screw_Terminal_01x02`/`C8439` 5.08mm connectors are **J1 and J3** (Phil's question referenced "J2", but J2 is actually the unrelated `USB4105-GF-A` USB connector - a designator mix-up, not a real conflict). Phil confirmed both are intentionally generic Amazon-sourced 5.08mm screw terminals for bring-up testing, with final connector selection explicitly deferred until enclosure design (not yet started). Current/voltage rating stays a confirmed PASS regardless of exact manufacturer identity.

### DEC-060 - R30 CDC strap closed by grounding the pin, not by resolving the table mismatch

Rather than chase whether R30's 40.2kOhm value maps to a real CDC_GAIN setting (DEC-047's open question), Phil chose to ground CDC instead. **Changed R30 from 40.2kOhm to 0 Ohm** (same part as R33/R49/R63/R64, LCSC `C17888`) directly in `Solar Project Rev1.kicad_sch` (Value/Datasheet/LCSC/Description properties updated), and merged it into that BOM group. This is an explicitly datasheet-documented valid state (LM51772 pin 10: "in case the current monitor is disabled connect CDC to ground") - cleanly disables the whole cable-drop-compensation/current-monitor feature rather than leaving it in an ambiguous, unverified state. Flagged as a **deliberate placeholder, not a final answer** - revisit and pick a real `CDC_GAIN` setting once bring-up testing clarifies whether current-monitor telemetry or cable-drop compensation is actually wanted.

---

## Phase 3: Resolving the DEC-049/050 Crisis (DEC-051 through DEC-057)

### DEC-051 - Root-cause fix: rewire the PV array 2P instead of 2S

Re-reading the LM51772 datasheet's Recommended Operating Conditions (not just Absolute Maximum Ratings) found `V(VIN)` recommended max is only **48V typ/55V max** - the corrected 57.0V STC 2S string Voc already exceeded the *recommended* range at STC alone, not only the 59V abs max under cold conditions. Presented three options to Phil: (A) rewire 2P, (B) upstream VIN clamp/pre-regulator + keep 2S, (C) replace U6 with a higher-VIN part + keep 2S. **Phil selected (A).** Under 2P, `PV_IN_POS` sees single-panel Voc (28.5V STC, ~33.9V at -40°C) instead of the string Voc - comfortably inside U6's recommended range and D2's 64V standoff at every disclosed temperature, with no new parts required.

Trade-off: PV-side current roughly doubles (`Isc_parallel`≈8.90A vs the old 4.45A single-string figure) - this reopened every current-based margin computed under 2S assumptions.

### DEC-052 - Re-verified the reopened current-based margins (with a correction to DEC-051's own assumption)

Installed `pywin32` and used Excel COM automation to actually recompute the LM51772 Quickstart Tool workbook at the 2P operating point (VIN 20/23.6/34V), rather than assuming every figure doubles along with `Isc`:
- **Input-bank ripple current: actually IMPROVED** (7.75A demand vs the old 8.56A) - ripple current is driven by the converter's own operating point, not directly by panel `Isc`. Margin against the ~15.2A RMS bank rating improved to ~48.8% - **PASS, closed**.
- **Q4/D1 current margin:** simple ratio, `(22A-8.90A)/22A`≈59.5% - **PASS**, still comfortable though tighter than the old ~76%.
- **F1 fuse amperage:** re-sized to a provisional **15A** (125% of `Isc_parallel`).

### DEC-053 → DEC-054 - F1 fuse family: proposed then reversed

DEC-053 initially selected a MIDI/AMI 58V-class bolt-down fuse family for F1. **Phil then reversed this (DEC-054):** keep F1 on the existing Keystone `3522-2` 32V-class clip (same as F2, no footprint/BOM change). The residual cold-Voc exceedance (~1.9V/~5.9% over 32V, only below ~-25°C) is accepted and will be mitigated by an **active, firmware-controlled parasitic load** on the panel (display/fan/status LEDs, a few mA) that pulls Voc down whenever elevated no-load cold Voc is a concern. **This is a new, not-yet-implemented Track F firmware requirement** - added to the closure plan. Residual risk explicitly flagged: if firmware fails to apply the load, F1 could still see up to ~1.9V over its 32V rating. Exact mA load needed is NOT yet calculated (needs the panel's near-Voc I-V curve, not published).

### U14/Q5/Q6 gate test confirmed BLOCKED on hardware (not just unscheduled)

Checked the as-manufactured Rev 0 schematic directly: **zero instances of `LM74502`, `U14`, or `BSC040N10NS5` anywhere in it.** Rev 0 used a different discrete P-MOS/N-MOS gate arrangement for this function (confirmed via `docs/rev-one-bench-learning-matrix.md` row 5 - the LM74502H+Q5/Q6 design is the documented *rework* of Rev 0's approach, not a carried-over circuit). Rev 1 also has not reached Gate 4 PCB layout, so **no physical Rev 1 board exists either.** This test cannot run on any hardware that currently exists. Did complete a desk/netlist pre-check of Section 3's pre-power items (all PASS) so the procedure is ready to execute the moment hardware exists.

### DEC-055 - C7 split out of its shared BOM line

`docs/rev-one-bom.md` had grouped C7 with C15/C34/C35 as one 2.2uF/50V part (matched only on value/footprint). Netlist-confirmed C15/C34/C35 sit on ≤14.6V nets and are fine at 50V. C7 sits on `PV_IN_POS` and needs a **100V-rated replacement** (its 50V rating is exceeded by STC Voc alone under either wiring option). Split into its own BOM line; **exact LCSC catalog number is open** (live LCSC parametric search isn't fetchable with available tools).

### DEC-056 - C52 split and bumped to 220-330nF

Same BOM-grouping issue as C7. C52 (U14's VCAP-VS charge-pump reservoir cap) needed a capacitance bump per the earlier DEC-042 finding (LM74502H's own `CVCAP>=10x Ciss` guidance needs ~106nF at Q5/Q6's worst-case Ciss; 100nF fell ~6% short). Bumped to 220-330nF, split into its own line, **exact part number open**.

### DEC-057 - Found and corrected a real data error in D2's numbers, then replaced the part anyway

While resolving D2's last open sub-item (clamp voltage vs. Q4's VDS), downloaded D2's actual manufacturer (Hongjiacheng) SMB family datasheet. **Found that DEC-042 had cited the wrong table row**: the installed `SMBJ60CA`'s real numbers are `Vrwm`=60V/`VC`=96.8V/`Ipp`=6.20A (confirmed against the BOM's own LCSC description text), not the `Vrwm`=64V/`VC`=103V/`Ipp`=5.83A previously cited - those are the *adjacent* `SMBJ64A`/`64CA` row, a row-transcription error. This means D2's real clamp voltage was already under Q4's 100V VDS abs max, just with a thin ~3.2% margin, not exceeding it as previously stated. Asked Phil how to resolve this; **Phil chose to replace the part anyway** for comfortable margin: **`SMBJ48CA`/`SMBJ48A`** (same family/footprint) - `Vrwm`=48V, `VC`=77.4V @ `Ipp`=7.75A, giving ~41.6% standoff margin and ~22.6% clamp margin. This closes the D2/C52 row's last analytical sub-item; **exact LCSC catalog number is open**.

---

## Current Open Items (Track A is effectively closed; remaining items are bench-only, firmware backlog, or other tracks)

1. **C18 output-cap ripple-current rating** (Output cap bank row, still `HOLD`) - datasheet text extraction failed (CJK font encoding); only a low-frequency 270mA@120Hz rating is confirmed vs an estimated ~1.33A high-frequency demand. The single remaining genuinely-blocking Track A item; needs either better datasheet data or a bench measurement.
2. **New firmware requirement (DEC-054, not yet implemented):** active parasitic-load Voc control on the PV input during cold-weather startup qualification and whenever Q4 is held off. Exact mA load not yet sized.
3. **L2 thermal/core-loss, R16 pulse energy** - genuinely bench-only items, no further desk research will close them.
4. **U14/Q5/Q6 gate test** - blocked until Rev 1 boards are fabricated and assembled (see Phase 3 above).
5. Track B BOM/sourcing closure - ~9 remaining non-matched BOM lines (J2 USB, J4/J5/J6 headers, U2 module), U1 STM32G431 vs G473 sourcing confirmation, order-guide reconciliation, plus the new `J8` remote I2C temperature-sensor part/address (DEC-061).
6. Track C safety-scope decisions - **CLOSED this session (DEC-061/062)**; only the remote I2C sensor part selection remains, tracked under Track B/BOM above.
7. Track D Rev 0 evidence mining (VCC2 backfeed isolation test, FP4 current-limit stall root cause) - not yet started.
8. Track E Gate 4 PCB layout - not yet started; Track A and Track C are now far enough along to seriously consider starting this.

---

## Current Decisions Log State

`docs/decisions-log.md` now runs through **DEC-062** in the log file itself - DEC-058 through DEC-062 (LCSC catalog-number confirmations, J1/J3 connector closure, R30 CDC grounding, and the Phase 5 Track C decisions) were backfilled into the formal log this session; no more memory-only decisions are pending.

---

## Next Session Objective

**Track A is effectively closed and Track C is fully closed.** The next session should pick one (or more, in parallel) of:

1. **Track B BOM/sourcing sweep**: resolve the ~9 remaining non-matched BOM lines, reconcile `docs/rev-one-lcsc-order-guide.md` against the final BOM, confirm U1 sourcing path, and pick the remote I2C temperature-sensor part/address for `J8` (DEC-061).
2. **C18 ripple-current rating** - if Phil has access to a clean (non-CJK-garbled) datasheet or a bench setup, this is the last real Track A blocker.
3. **Track E Gate 4 PCB layout kickoff** - start floorplanning: Update PCB From Schematic, power-loop floorplan, DRC setup. `J8`'s placement (near the battery pack, off-board cable run) should be considered during floorplanning.
4. **Track D Rev 0 evidence mining** - VCC2 backfeed isolation test, FP4 current-limit stall root cause - can run in parallel with any of the above since it uses different hardware/time.

Do not re-propose running the U14/Q5/Q6 gate test on Rev 0 - confirmed impossible, see Phase 3 above.

---

## Important Working Rules Carried Forward

- Check for `~Solar Project Rev1.kicad_sch.lck` / `.kicad_pcb.lck` / `.kicad_pro.lck` before any KiCad edit. If a lock file exists, confirm no `kicad.exe`/`eeschema.exe` process is actually running (`Get-Process`) before treating it as stale and deleting it.
- When hand-editing a `.kicad_sch` file via raw text edit, validate with `kicad-cli sch erc` and `kicad-cli sch export netlist --format kicadsexpr` before and after (baseline via `git show HEAD:<path>`) rather than trusting hand-computed pin coordinates or paren-balance alone - see `/memories/kicad-schematic-editing.md`.
- Treat `docs/rev-one-closure-plan-2026-09-16.md` as the living tracking doc - update dispositions in place, don't spawn parallel plan docs.
- **Use `Solar Project Rev1.net` (no suffix) as the current netlist, not `Solar Project Rev1-current.net`, which is stale despite its name** (see DEC-043 / `kicad-workflow.md` repo memory).
- **PV array wiring is now 2P (parallel), not 2S (series)** - see DEC-051. `hardware/mppt/envelope-decision-worksheet.md` Section 2.1 is the active baseline; Section 2.2 (2S) is retained for history only.
- **F1 stays on the Keystone `3522-2` clip** (DEC-054, reversed DEC-053) - do not propose swapping it to a different fuse family again without new information; the open item is the firmware parasitic-load mitigation, not the fuse itself.
- When re-verifying an old finding, don't just re-cite a prior DEC entry's numbers at face value - if a manufacturer family/parametric table is available, pull the actual row for the specific installed part and cross-check against the BOM's own description text. Adjacent-row transcription errors in dense datasheet tables are a real, recurring risk in this project (5+ instances this session alone).
- Live LCSC parametric search is NOT fetchable via the available web-fetch tool (only static product/datasheet PDF pages work reliably) - for open catalog-number items, either ask Phil to search and paste a candidate, or find another lookup method.
- When citing an external part/panel source, always record the exact product URL/ASIN/model, not just "brand + Amazon page."

---

## Primary Evidence / Artifacts Touched This Session

- [docs/rev-one-closure-plan-2026-09-16.md](docs/rev-one-closure-plan-2026-09-16.md) (every Track A row updated across all three phases; Track C section closed in Phase 5)
- [docs/decisions-log.md](docs/decisions-log.md) (DEC-033 through DEC-062)
- [docs/rev-one-component-derating-review.md](docs/rev-one-component-derating-review.md) (every remaining Track A row, plus new finding sections)
- [docs/rev-one-bom.md](docs/rev-one-bom.md) (C7/C52/D2 real LCSC part numbers written in per DEC-058; R30 merged into the 0-ohm resistor group per DEC-060; new `J8` connector line added per DEC-061)
- [docs/requirements.md](docs/requirements.md) (REQ-TEL-002 reworded, new REQ-BAT-003, DEC-061/062)
- [docs/architecture.md](docs/architecture.md) (Section 3.3/3.6, interface table, deferred-decisions list, DEC-061/062)
- [hardware/kicad/solar-project/Solar Project Rev1.kicad_sch](hardware/kicad/solar-project/Solar%20Project%20Rev1.kicad_sch) (R30 changed from 40.2kOhm to 0 Ohm, DEC-060; new connector `J8` + 4 global labels tapping `I2C CLK`/`I2C DATA`/`CTRL_3V3`/`PWR_NEG`, DEC-061)
- [hardware/mppt/envelope-decision-worksheet.md](hardware/mppt/envelope-decision-worksheet.md) (2P is now the active Section 2.1 baseline; 2S retained in Section 2.2 for history)
- [docs/datasheets/manifest.yaml](docs/datasheets/manifest.yaml) (8 entries, including the newly added `hongjiacheng-smbj-series` family datasheet used for the DEC-057 correction)
- [docs/test/rev-one-u14-low-voltage-gate-test.md](docs/test/rev-one-u14-low-voltage-gate-test.md) (Section 3 desk pre-check added; blocked-on-hardware note added)
- `hardware/kicad/solar-project/*.pdf` (8 locally-archived datasheet PDFs, including the new SMBJ family datasheet)
- `hardware/LM51772_Buck-Boost_Quickstart_Tool_Rev1_2P_28V5_14V6_12A.xlsm` (new workbook copy for the 2P re-baseline, DEC-052)
- [NEXT_CHAT_PROMPT.md](NEXT_CHAT_PROMPT.md) (rewritten to point to R30/J3/part-sourcing as the next actionable items)
- `/memories/repo/mppt-power-path.md` (full session narrative and lessons learned recorded for future sessions)
- `/memories/excel-com-automation.md` (new user-level memory: pywin32/Excel COM automation tips)

No KiCad PCB/project files were edited this session; the schematic (`Solar Project Rev1.kicad_sch`) had exactly one edit (R30, DEC-060) - all other changes are documentation, BOM, and a new workbook copy.
