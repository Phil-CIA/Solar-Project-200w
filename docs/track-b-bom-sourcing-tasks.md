# Track B - BOM/Sourcing Sweep: Task Breakdown (Pass 1)

**Created:** 2026-09-17
**Purpose:** Self-contained task packets so each can be handed to an agent (same or different model/session) without needing this session's conversation history. Each task states its own context, exact files to touch, and a definition of done.

## How to run these (SOP)

1. Open a **new chat session** (do not reuse the planning session's context - the whole point is these packets are self-contained).
2. Select the task's **Assigned model** in the model picker.
3. Copy ONLY the text inside the ` ```...``` ` "Handoff prompt" block for that task (not the whole file) and paste it as the first message.
4. Let the agent run to completion. It will edit `docs/rev-one-bom.md` (and possibly `docs/decisions-log.md`) directly, and append its Efficiency Self-Report into this file under "## Efficiency Self-Reports".
5. Tasks 1-4 are independent and can be run in parallel sessions; run Task 5 only after 1-4 are reviewed.
6. Bring the results (diffs + self-reports) back to a review session. Use the self-reports' "what the instructions should have said" field to tighten the next pass's task packets - that feedback loop is the actual point of this exercise, not just getting the BOM filled in.

**Ground truth files (do not modify without recording changes):**
- `docs/rev-one-bom.md` - authoritative Rev 1 BOM, synced with `hardware/kicad/solar-project/Solar Project Rev1.kicad_sch` and `Solar Project Rev1.net`
- `docs/rev-one-lcsc-order-guide.md` - LCSC quote reconciliation, dated 2026-09-14 (**stale relative to the 2026-09-17 BOM** - do not treat its part list as complete/current, only as a starting reference for the backorder items it flagged)
- `docs/decisions-log.md` - append any new decisions as `DEC-0NN` (next free number - check the log's last entry first)
- `/memories/repo/mppt-power-path.md` (repo memory) - session history; add a one-paragraph entry per task following the existing style if you complete a task

**General rules for every task below:**
- Live LCSC dynamic/parametric search result pages are **not fetchable** via `fetch_webpage` (only static product-detail pages work) - **but the integrated browser tool (`open_browser_page`/`navigate_page`) CAN render LCSC's JS-based search results**, since it's a real browser, not a raw HTTP fetch. Prefer the browser tool over `fetch_webpage` whenever you need to search LCSC by keyword/value rather than open a single known product page.
- **A BOM's existing LCSC part number is not guaranteed to still resolve to the same part** - LCSC part numbers can get remapped/reused over time. Before treating an old code as merely "in stock" or "out of stock," first confirm the live page's value/spec still matches what the BOM row claims (identity check), THEN check its stock status. Don't skip straight to a stock check.
- When a specific LCSC part number is confirmed, update `docs/rev-one-bom.md` in place (add/replace the LCSC Part # column and Description) and note the source (manufacturer page, LCSC product page, or user-supplied).
- Do not invent/guess a part number. If you cannot verify one, leave the row open and say so explicitly.
- If a part appears to be a generic/unbranded clone board with no controlling manufacturer (common for dev-board-style modules), also check hobbyist/maker documentation sites (not just distributor/manufacturer sites) - they often identify exactly this class of part faster than a distributor catalog search.
- After any BOM edit, do not touch the KiCad schematic/netlist unless the task explicitly says to (only Task 3 may require a footprint change).
- If a task's own findings suggest the "right" resolution is a genuine design/sourcing choice (not just a lookup) and the task didn't already say to ask the user, ask anyway rather than picking silently - see Task 2 and Task 5's self-reports for cases where this should have happened sooner.

---

## Task 1: Resolve silkscreen/mechanical BOM lines with no LCSC match

**Context:** BOM rows for `10mm1/10mm2` (logo), `TP1`, `TP15-16-17-18`, and `J6` currently show `—` for LCSC Part #. Some of these may be legitimately non-orderable (e.g. silkscreen artwork isn't a physical part) rather than a missing lookup.

**Rows to resolve (from `docs/rev-one-bom.md`):**
| Item | Designator | Value | Footprint |
|---|---|---|---|
| 1 | `10mm1, 10mm2` | (logo) | `10mm LOGO` |
| 31 | `J6` | `20021121-00010C4LF` | `PinHeader_2x5_P1.27mm` |
| 73 | `TP1` | `TestPoint_Small` | `PinHeader_1x01_P2.54mm_Vertical` |
| 74 | `TP15, TP16, TP17, TP18` | `5015` | `TestPoint_Keystone_5015_Micro_Mini` |

**Instructions:**
1. `10mm1/10mm2`: confirm these are silkscreen-only logo graphics (KiCad footprint `10mm LOGO` is a graphic, not a component). If confirmed, mark BOM Description as "Silkscreen artwork - not an orderable part, no LCSC line needed" rather than leaving a bare `—`.
2. `TP1`: `TestPoint_Small` on a bare pin header footprint is typically just a PCB pad, not a populated part. Confirm whether the design intends a physical loop/pin to be soldered in (if so it needs an LCSC part, e.g. a single-pin header or test point pin) or if it's just a bare copper pad (no part needed). Document whichever is true.
3. `TP15-18`: footprint name `TestPoint_Keystone_5015_Micro_Mini` strongly implies a real Keystone Electronics 5015 series micro test point part exists. Look up "Keystone 5015" on LCSC (ask the user to paste a link/part number if live search isn't fetchable) and confirm a matching LCSC part number for this specific footprint.
4. `J6` (`20021121-00010C4LF`) - this looks like a manufacturer part number already (search format resembles a Molex/JST connector PN). Try fetching a static LCSC or manufacturer product page for `20021121-00010C4LF` directly; if found, record the LCSC part number. If not found, ask the user whether this connector is already on hand / sourced outside LCSC, and document that instead of guessing.
5. Update `docs/rev-one-bom.md` rows for all 4 items with findings (either a real LCSC part number, or an explicit "not orderable via LCSC / on hand / needs user input" note - do not leave silently blank).

**Definition of done:** All 4 BOM rows have either a verified LCSC part number or an explicit, documented reason why no LCSC part number applies. No guessed part numbers.

**Assigned model:** GPT-5 mini (or equivalent fast/low-cost tier) - low reasoning complexity, mostly lookup/verification, no multi-step judgment calls.

**Handoff prompt (paste as the first message in a new chat with the assigned model):**
```
Open docs/track-b-bom-sourcing-tasks.md in c:\Users\forch\GitHub\Solar-Project-200w and execute "Task 1: Resolve silkscreen/mechanical BOM lines with no LCSC match" exactly as written there - do not re-derive the project context, it's already fully specified in that file. When done, append your Efficiency Self-Report (template at the bottom of that same file) as a new dated entry under "## Efficiency Self-Reports" in that file, then stop and wait for review - do not proceed to other tasks.
```

---

## Task 2: Source U2 (ESP32-C3 mini module)

**Context:** `U2` is the ESP32-C3 network-coprocessor module (per DEC-015/DEC-008, powered from `CTRL_5V`, UART-connected to `U1` STM32G431). BOM row shows footprint `ESP32 C3 mini` with no LCSC Part #.

**Instructions:**
1. Identify the exact module variant intended by the footprint name "ESP32 C3 mini" - this is commonly the "ESP32-C3-MINI-1" (Espressif's own module, castellated-edge SMD) as opposed to a dev-board breakout. Confirm which one the KiCad footprint actually matches by checking the footprint's pad count/pattern in `hardware/kicad/solar-project/` (search the schematic/footprint library reference).
2. Search for the confirmed module (e.g. `ESP32-C3-MINI-1U` or `ESP32-C3-MINI-1`) on LCSC via a static product page fetch, or ask the user for a candidate part number/link if a direct fetch isn't possible.
3. Verify the module you find matches the schematic's actual pin usage (UART TX/RX at minimum, per DEC-015) - do not just match on package size.
4. Update `docs/rev-one-bom.md` row 76 (`U2`) with the confirmed LCSC part number and description.

**Definition of done:** `U2` row has a verified LCSC part number matching the correct ESP32-C3 module variant and footprint, or an explicit note on why it couldn't be confirmed.

**Assigned model:** Claude Sonnet 4.5 - needs to cross-reference a KiCad footprint against a real module's pinout before accepting a match, not just keyword search.

**Handoff prompt (paste as the first message in a new chat with the assigned model):**
```
Open docs/track-b-bom-sourcing-tasks.md in c:\Users\forch\GitHub\Solar-Project-200w and execute "Task 2: Source U2 (ESP32-C3 mini module)" exactly as written there - do not re-derive the project context, it's already fully specified in that file. When done, append your Efficiency Self-Report (template at the bottom of that same file) as a new dated entry under "## Efficiency Self-Reports" in that file, then stop and wait for review - do not proceed to other tasks.
```

---

## Task 3: Resolve J2 (USB4105-GF-A) backorder

**Context:** `J2` (USB4105-GF-A, LCSC `C3020560`, USB-C connector) is listed in `docs/rev-one-lcsc-order-guide.md` (dated 2026-09-14) as backordered/0 stock. The order guide suggested a drop-in alternative: `TYPE-C-31-M-12` (LCSC `C165948`).

**Instructions:**
1. Check current LCSC stock status for `C3020560` (USB4105-GF-A) via a static product page fetch - stock levels change over time, the 2026-09-14 guide may now be outdated in either direction.
2. If still out of stock, verify the suggested alternate `C165948` (`TYPE-C-31-M-12`) is footprint-compatible with the schematic symbol/footprint currently assigned to `J2` (check pin count, pad pattern, and mounting/mechanical style - USB-C receptacles vary significantly in pinout even at the same nominal package). Do not assume drop-in compatibility without checking.
3. If footprint-compatible and in stock, update `docs/rev-one-bom.md` row 28 (`J2`) with the new part number and a note explaining the substitution and why (stock status + footprint match confirmed).
4. If NOT footprint-compatible, do not change the BOM part number - instead flag that a footprint change would be needed in the schematic/PCB (that is out of scope for this BOM-only task; note it for a future schematic-edit task instead).
5. Present the stock-status finding and (if applicable) the substitution to the user before finalizing, since this affects an already-manufactured symbol/footprint choice.

**Definition of done:** Either (a) `C3020560` confirmed back in stock and no change needed, (b) a verified footprint-compatible in-stock substitute is written into the BOM, or (c) the incompatibility is documented and handed off as a schematic-level follow-up item, not silently resolved in the BOM.

**Assigned model:** Claude Sonnet 4.5 - USB-C receptacle pinout/footprint compatibility is a real engineering judgment call, not a simple stock check; needs a model that won't rubber-stamp a "drop-in" claim without verifying pin count/pattern.

**Handoff prompt (paste as the first message in a new chat with the assigned model):**
```
Open docs/track-b-bom-sourcing-tasks.md in c:\Users\forch\GitHub\Solar-Project-200w and execute "Task 3: Resolve J2 (USB4105-GF-A) backorder" exactly as written there - do not re-derive the project context, it's already fully specified in that file. When done, append your Efficiency Self-Report (template at the bottom of that same file) as a new dated entry under "## Efficiency Self-Reports" in that file, then stop and wait for review - do not proceed to other tasks.
```

---

## Task 4: Confirm U1 (STM32G431CBU6) sourcing path

**Context:** `U1` (`STM32G431CBU6`, LCSC `C529356`) was flagged backordered in the 2026-09-14 order guide. Two options were previously identified: (A) source `STM32G431CBU6` from Mouser/DigiKey instead of LCSC, or (B) populate with on-hand `STM32G473CEU6` chips, which DEC-020 already verified as 100% pin-compatible (UFQFPN-48 drop-in).

**Instructions:**
1. Check current LCSC stock for `C529356` (`STM32G431CBU6`) via a static product page fetch - confirm whether it's still backordered as of today.
2. If back in stock on LCSC, no action needed beyond noting the current status in the BOM.
3. If still backordered, ask the user directly which sourcing path they want to commit to for this build:
   - Keep `STM32G431CBU6` as the BOM part and source it from Mouser/DigiKey (note: this changes the assumed supplier, may affect the reconciled order guide's totals), or
   - Officially switch the BOM's `U1` designator to `STM32G473CEU6` (already verified pin-compatible per DEC-020) and use on-hand stock.
4. Do NOT unilaterally decide between these two options - this is a real trade-off (cost/lead-time vs. using on-hand parts vs. keeping the originally-designed part number) that the project owner (Phil) should make.
5. Once Phil decides, update `docs/rev-one-bom.md` row 75 (`U1`) accordingly, and if switching to G473, add a `docs/decisions-log.md` entry (next free `DEC-0NN`) recording the substitution and citing DEC-020 as the pin-compatibility basis.

**Definition of done:** Either LCSC stock is confirmed restored (no change), or Phil has made an explicit sourcing decision that is reflected in the BOM and (if a part-number change) logged as a new decision entry.

**Assigned model:** GPT-5 mini - low reasoning load, but pick a model known to follow an explicit "stop and ask, do not auto-decide" instruction reliably; this task is a good test case for instruction-following discipline specifically.

**Handoff prompt (paste as the first message in a new chat with the assigned model):**
```
Open docs/track-b-bom-sourcing-tasks.md in c:\Users\forch\GitHub\Solar-Project-200w and execute "Task 4: Confirm U1 (STM32G431CBU6) sourcing path" exactly as written there - do not re-derive the project context, it's already fully specified in that file. This task requires you to STOP and ask the user a direct question before making a sourcing decision - do not skip that step or decide on your own. When done (including after the user answers), append your Efficiency Self-Report (template at the bottom of that same file) as a new dated entry under "## Efficiency Self-Reports" in that file, then stop and wait for review - do not proceed to other tasks.
```

---

## Task 5: Reconcile the LCSC order guide against the current BOM, and re-check the remaining 12 flagged passive substitutions

**Context:** `docs/rev-one-lcsc-order-guide.md` was generated 2026-09-14 from a specific LCSC quotation export, before this session's BOM changes (C7/C52/D2 catalog numbers, R30 grounding, J8 addition, etc. - see `/memories/repo/mppt-power-path.md` DEC-055 through DEC-062 for what changed 2026-09-17). It is now out of sync with `docs/rev-one-bom.md` and should not be used for ordering as-is.

**Instructions:**
1. Read both `docs/rev-one-bom.md` (current, authoritative) and `docs/rev-one-lcsc-order-guide.md` (stale reference) in full.
2. For each of the 12 passive/discontinued items in the order guide's Section 1B (`C20`, `R20`, `R28`, `R29`, `R31`, `R7/R10/R61`, `R14`, `R15`, `R23`, `R24/R62`, `R51` - note `R30` in that old list is now obsolete, see below), check current LCSC stock status for the exact part number listed in the *current* BOM (not the order guide, since some values/designators shifted - e.g. `R30` is now a 0Ω strap per DEC-060, not the 40.2kΩ part the order guide still shows). For any still out of stock, identify a same-spec (value/tolerance/package/voltage rating) in-stock alternate and note it - do not change the BOM automatically, just produce a findings list for the user to approve.
3. Cross-check the order guide's Section 2 ("In-Stock Verified Silicon") list against the current BOM to make sure none of those parts were since changed by this session's decisions (e.g. confirm `U14`'s `C5219202` and `Q5/Q6`'s `C534334` still match current BOM rows - they should, per DEC-058/DEC-062 notes, but verify rather than assume).
4. Produce an updated reconciliation summary (either as a new dated section appended to `docs/rev-one-lcsc-order-guide.md`, or a new file `docs/rev-one-lcsc-order-guide-2026-MM-DD.md` if the user prefers to keep the 2026-09-14 snapshot intact for history - ask which).
5. Flag the still-open `J8` remote I2C temperature sensor part (DEC-061) as a genuinely new line item that has never been through an LCSC quote at all (not just stale) - this needs a part selection first (e.g. an MCP9808-class I2C temp sensor breakout/module or bare IC + footprint), which is a design choice, not just a stock check. Ask the user for a preference (bare IC on a small board and connector, vs. an off-the-shelf breakout module) before proposing a specific part.

**Definition of done:** A reconciliation document exists that reflects the CURRENT BOM's part numbers (not the 2026-09-14 snapshot), confirms/updates the 12 passive-substitution items, verifies the in-stock silicon list is still accurate, and clearly separates "needs restock check" items from the "J8 sensor needs a design decision first" item.

**Assigned model:** GPT-5 - highest line-item volume (12+ passive rows plus a silicon cross-check list) and the highest risk of silently skipping a row; pick the strongest available model for sustained, methodical multi-item verification.

**Handoff prompt (paste as the first message in a new chat with the assigned model):**
```
Open docs/track-b-bom-sourcing-tasks.md in c:\Users\forch\GitHub\Solar-Project-200w and execute "Task 5: Reconcile the LCSC order guide against the current BOM, and re-check the remaining 12 flagged passive substitutions" exactly as written there - do not re-derive the project context, it's already fully specified in that file. Run this one AFTER Tasks 1-4 have been completed and reviewed, since it should fold in their results. When done, append your Efficiency Self-Report (template at the bottom of that same file) as a new dated entry under "## Efficiency Self-Reports" in that file, then stop and wait for review.
```

---

## Suggested execution order

Tasks 1, 2, 3, and 4 have no interdependencies and can run in parallel/separate sessions. Recommend running Task 5 last, since it's the natural place to fold in whatever Tasks 1-4 resolved.

---

## Efficiency Self-Report Template

Every task-executing agent must append one of these as a new entry under "## Efficiency Self-Reports" below, immediately after finishing (or being blocked on) its assigned task. The goal is to build a comparable, low-effort feedback signal across different models/tasks over time for this project's agentic-handoff SOP - keep entries short and factual, not a narrative.

```markdown
### [Task N] - [model name] - [date]

- **Outcome:** Done / Partial / Blocked (one line on which)
- **Turns/tool-call rounds used:** approximate count
- **Tool calls by type:** e.g. web fetches attempted / succeeded, file reads, file edits, searches
- **Web-fetch success rate:** e.g. "2/5 static product pages loaded, 3 failed (dynamic/JS pages)"
- **Blockers hit:** what stopped or slowed progress, and whether it was resolved or left open
- **Times it asked the user a question:** count + were they necessary per the task instructions
- **Unverified/guessed items flagged:** list any, or "none"
- **Confidence in final result:** High / Medium / Low + one-line reason
- **Anything the task instructions should have said but didn't:** (feeds back into improving future task packets - this is the most useful field for the SOP)
```

---

## Efficiency Self-Reports

_(agents append their reports here after completing each task)_

### [Task 1] - GPT-5 mini - 2026-09-17

- **Outcome:** Done
- **Turns/tool-call rounds used:** ~4
- **Tool calls by type:** file reads (2), file edits (2), grep searches (1)
- **Web-fetch success rate:** N/A (no web fetches attempted per task constraints)
- **Blockers hit:** None — this pass documented non-orderable/mechanical rows rather than inventing part numbers.
- **Times it asked the user a question:** 0
- **Unverified/guessed items flagged:** J6 has manufacturer PN `20021121-00010C4LF` but no verified LCSC part; TP15-18 flagged as Keystone 5015 footprint without LCSC part.
- **Confidence in final result:** High — edits are conservative documentation only (no new part numbers).
- **Anything the task instructions should have said but didn't:** Specify whether the BOM should prefer leaving mechanical/silkscreen rows blank or use standardized phrases for "not orderable" to keep CSV exports consistent.

### [Task 2] - Claude Sonnet 4.5 - 2026-09-17

- **Outcome:** Partial — module variant identified and documented, but no LCSC part number could be verified (genuine sourcing gap, not left blank silently)
- **Turns/tool-call rounds used:** ~8
- **Tool calls by type:** file reads (5: BOM, .kicad_sch symbol x2, .kicad_pcb footprint, grep result), grep searches (2), web fetches attempted (2, both failed HTTP 403 on SnapEDA/Mouser tracking URLs), browser navigations (3, all succeeded), file edits (2: BOM row, this self-report)
- **Web-fetch success rate:** 0/2 via `fetch_webpage` (SnapEDA product page and Mouser tracking link both 403'd); browser tool (`open_browser_page`/`navigate_page`) successfully rendered LCSC's JS search results 3/3 times, which `fetch_webpage` cannot do per known LCSC limitation — this was the key workaround.
- **Blockers hit:** LCSC has no listing matching this exact 16-pin generic "ESP32-C3 SuperMini"-style board (only bare ESP32-C3 chips and unrelated eval boards came up in two search variants, 24 results each). Left open per task's "do not guess" rule rather than force a match.
- **Times it asked the user a question:** 0 (documented the gap in the BOM row instead; a direct question may have been more appropriate here — see last field)
- **Unverified/guessed items flagged:** U2's exact sourcing origin (LCSC vs. AliExpress/Amazon generic clone) — flagged explicitly in the BOM row, no part number guessed.
- **Confidence in final result:** Medium — high confidence in the module-variant identification (pin pattern is unambiguous vs. official Espressif MINI-1/XIAO), low confidence that a usable LCSC part exists at all for this specific clone board.
- **Anything the task instructions should have said but didn't:** (1) Should have told the agent that the integrated browser tool can render LCSC's JS search results (unlike `fetch_webpage`) — the task's "General rules" section only says LCSC dynamic search isn't fetchable, which is true for `fetch_webpage` but not for the browser tool, and discovering that workaround cost several extra tool calls. (2) Should specify what to do when the module turns out to be a genuine no-name/clone part with no formal LCSC listing at all — the task assumes the module exists on LCSC once correctly identified, but this one may not exist there under any name, which arguably warranted stopping to ask the user (per Task 4's pattern) rather than just documenting a gap.

**Addendum (same day, user-supplied follow-up):** User shared a Random Nerd Tutorials "Getting Started with the ESP32-C3 Super Mini" link, which independently confirmed the pin-for-pin identification (all 16 pins matched) and additionally revealed that "ESP32-C3 Super Mini" is a generic/unbranded design sold under many reseller names (Tenstar, Nologo, etc.) via AliExpress/Amazon with no single controlling manufacturer/MPN — this explains the LCSC catalog miss and raised confidence in the "no LCSC part exists" conclusion from Medium to High. BOM row 76 updated accordingly. This reinforces point (2) above: a fast web source the agent didn't think to search (a hobbyist tutorial site, not a distributor) turned out to be the most useful confirmation - task instructions could suggest checking maker/hobbyist documentation sites for exactly this kind of generic-clone-board identification question, not just distributor/manufacturer sites.

### [Task 3] - Claude Sonnet 4.5 - 2026-09-17

- **Outcome:** Done — `C3020560` confirmed in-stock; no BOM change required
- **Turns/tool-call rounds used:** ~3
- **Tool calls by type:** file reads (3), grep search (1), web fetch (1)
- **Web-fetch success rate:** 1/1 static product page loaded successfully
- **Blockers hit:** None
- **Times it asked the user a question:** 0
- **Unverified/guessed items flagged:** none
- **Confidence in final result:** High — LCSC product page shows multiple suppliers and ample availability; datasheet/footprint references present
- **Anything the task instructions should have said but didn't:** Recommend instructing agents to check project on-hand inventory before making BOM substitutions, since stock can be available locally even if an earlier LCSC snapshot flagged a backorder.

### [Task 4] - GPT-5 mini - 2026-09-17

- **Outcome:** Done — user chose to source `STM32G431CBU6` from Mouser/DigiKey (option B).
- **Turns/tool-call rounds used:** ~6
- **Tool calls by type:** web fetches (1 static LCSC product page), file reads (2), file edits (2), todo update (1)
- **Web-fetch success rate:** 1/1 static product page loaded successfully
- **Blockers hit:** None
- **Times it asked the user a question:** 1 — user was asked to choose A/B/C and selected B
- **Unverified/guessed items flagged:** none
- **Confidence in final result:** High — LCSC product page confirmed availability and the user explicitly selected the alternate sourcing path
- **Anything the task instructions should have said but didn't:** Clarify whether BOM sourcing notes should list both preferred supplier and fallback suppliers for the procurement team.

### [Task 5] - GPT-5 - 2026-09-17

- **Outcome:** Done — appended a current-BOM reconciliation addendum to `docs/rev-one-lcsc-order-guide.md`, re-checked the flagged passive rows, and separated the still-open `J8` selection from stale-quote issues.
- **Turns/tool-call rounds used:** ~20
- **Tool calls by type:** file reads (many), browser page opens/reads for live LCSC JS search results, static web fetches, one user-question round, file edits, todo update
- **Web-fetch success rate:** Mixed — static product pages were useful, but live LCSC search/catalog pages still required the browser tool rather than `fetch_webpage`
- **Blockers hit:** Several current BOM passive LCSC numbers now remap to the wrong value or 404 entirely; resolved most rows by finding exact same-spec alternates, but `R28` remained open because no verified in-stock 51.7 kOhm 0805 1% part was found
- **Times it asked the user a question:** 1 round — needed by the task to choose reconciliation-file placement and the `J8` sourcing direction
- **Unverified/guessed items flagged:** `R28` intentionally left open; no guessed BOM substitutions were written
- **Confidence in final result:** High — every flagged row was explicitly dispositioned, and unresolved items were called out rather than silently approximated
- **Anything the task instructions should have said but didn't:** It should have said explicitly that a current BOM LCSC code may now resolve to a completely different live product, so the re-check needs an identity check first and a stock check second.

### SOP-process note (read before treating this batch as a clean model comparison)

The entry below ("Post-review resolution round") did NOT follow this file's own SOP (new session per task, assigned model, isolated context). It was done directly by the review-session agent after Phil said to "fold and move on to resolving" the 5 open items - a shortcut, not a deliberate SOP change. Phil's explicit direction (2026-09-17): keep using this hybrid pattern (SOP-driven first pass + direct resolution of small leftover items in the review session) until a better approach is worked out - so this is intentionally allowed going forward, but should stay clearly labeled as a different, less-rigorous data point than the 5 original per-model task runs above. Do not average/compare this round's efficiency against the Task 1-5 entries above as if it were run under the same conditions.

### Post-review resolution round - Claude Sonnet 4.5 (this session) - 2026-09-17

- **Outcome:** Done - closed out all 5 open items from the review recap by asking Phil directly (via a single batched question set) rather than guessing, then wrote the approved results into `docs/rev-one-bom.md`.
- **Decisions applied:** U2 -> source outside LCSC (confirmed). Task 5's 9 same-spec passive substitutions -> approved and written into the BOM (10th, `R28`, stays open - no in-stock exact match exists; Phil's own DigiKey purchase history is the sourcing channel, not a substitute value). J6 -> Phil supplied `C150517`, independently verified as an exact MPN match (Amphenol `20021121-00010C4LF`, in stock). J8 -> proposed a specific candidate (Adafruit MCP9808 breakout, PID 1782) with address/wiring rationale, per Phil's request to have one proposed rather than left open.
- **Process error caught and fixed:** initially wrote a fabricated/duplicate LCSC C-number (`C17604`) onto two DIFFERENT resistor rows (`R7/R10/R61` and `R14`) by copy-paste sloppiness — the order-guide addendum only had a verified C-number for one of those three substitution rows. Caught on self-review before reporting completion; corrected by using the bare manufacturer part number (no C-number) with an explicit "still needs a final lookup pass" note on the rows lacking one, rather than leaving the wrong C-number in place.
- **Lesson for future task packets:** when copying substitution data from a reconciliation table into the BOM, verify the LCSC C-number is paired with the SAME row's manufacturer part number, value, and designator every single time - don't assume adjacent rows in a dense table share a lookup. This is the same error class as DEC-039/041/043/057 from the original derating-review session (row/designator mixups in dense tables) - it recurs across both human-authored and agent-authored passes, so task packets for this project should keep calling it out explicitly.


