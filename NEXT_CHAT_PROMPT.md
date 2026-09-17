Continue the Solar Project Rev 1 closure work. Read `SOLAR_HANDOFF_2026-09-17.md` in full before doing anything else - it covers all five phases of the 2026-09-17 session.

**Status recap: Track A (Gate 1 component derating) is now effectively CLOSED, and Track C (safety-scope decisions) is now fully CLOSED too.** The U6/D2 overvoltage crisis (DEC-049/050) was fully resolved by rewiring the PV array 2P (DEC-051/052). F1 fuse settled on the Keystone `3522-2` clip plus a firmware backlog item (DEC-054). C7/C52/D2 all have confirmed real LCSC catalog numbers (DEC-058). J1/J3 connector part-identity is closed as an accepted bench/bring-up disposition (DEC-059). R30 CDC strap ambiguity is closed by grounding the pin to 0 Ohm (DEC-060, schematic edited). U14/Q5/Q6 gate test is confirmed BLOCKED on hardware - do not re-propose Rev 0 workarounds. **Track C is closed (DEC-061/062): battery-pack temperature sensing is implemented as connector `J8`, a remote I2C-bus tap (not an ADC/NTC channel - every MCU GPIO was already in use); the BMS communication link is deferred to Rev 2.**

**Only two genuinely open Track A items remain, both bench-only:**
1. **C18 output-cap ripple-current rating** - datasheet text extraction failed (CJK encoding); only a low-frequency 270mA@120Hz rating is confirmed vs. an estimated ~1.33A high-frequency demand. This is the one item that could still block Track A exit if a clean datasheet or bench measurement isn't available.
2. **L2 core-loss / R16 pulse-energy** - graphical/raster datasheet curves only, no further desk research will close these.

**We are ready to move to the next tracks.** Pick from, in priority order:

1. **Track B BOM/sourcing sweep**: resolve the ~9 remaining non-matched BOM lines (J2 USB4105-GF-A, J4/J6 headers, U2 ESP32-C3 module - confirm genuinely non-orderable vs. missing lookups), reconcile `docs/rev-one-lcsc-order-guide.md` against the final BOM, confirm U1 `STM32G431CBU6` vs. `STM32G473CEU6` sourcing, and pick the remote I2C temperature-sensor part/address for the new `J8` connector (DEC-061). Note: `docs/rev-one-bom.md` also has a stale `J5` row (`Conn_01x03_Socket`) that no longer exists in the schematic - clean that up too.
2. **C18 ripple-current rating** - if Phil has a clean datasheet or bench setup, close the last real Track A blocker.
3. **Track E Gate 4 PCB layout kickoff** - start: Update PCB From Schematic (F8), floorplan the power loop (input protection -> switch nodes -> L2 -> R16 -> F2), isolate analog/Kelvin sensing routing (including the new `J8` remote-cable run to the battery pack), set 4-layer stackup rules, run DRC.
4. **Track D Rev 0 evidence mining** - can run in parallel with any of the above since it uses different hardware/time.

**Housekeeping notes to carry forward:**
- `docs/decisions-log.md` now runs through **DEC-062** in the file itself - no more memory-only decisions pending.
- Before any KiCad schematic edit, check for `~Solar Project Rev1.kicad_sch.lck` / `.kicad_pcb.lck` / `.kicad_pro.lck` AND confirm no `kicad.exe`/`eeschema.exe` process is actually running (`Get-Process`) before treating a lock file as stale.
- When hand-editing a `.kicad_sch` file, validate with `kicad-cli sch erc` and `kicad-cli sch export netlist --format kicadsexpr` before/after (baseline via `git show HEAD:<path>`) rather than trusting hand-computed pin coordinates - see `/memories/kicad-schematic-editing.md`.
- Live LCSC parametric search is NOT fetchable via the web-fetch tool (only static product/datasheet pages work reliably) - for any new open catalog-number item, ask Phil to search and paste a candidate part number, then verify it against the live product page.
- **PV array wiring is 2P (parallel), not 2S (series)** - see DEC-051. `hardware/mppt/envelope-decision-worksheet.md` Section 2.1 is the active baseline.
- **F1 stays on the Keystone `3522-2` clip** (DEC-054) - do not propose swapping it to a different fuse family again without new information.
- **Battery-pack temperature sensing is connector `J8`** (remote I2C sensor tap, DEC-061), not an NTC/ADC channel - do not re-propose an ADC-based approach; there is no free ADC-capable GPIO on this MCU as wired.
- Do not re-propose running the U14/Q5/Q6 gate test on Rev 0 - confirmed impossible (Rev 0 uses a different discrete gate topology, and no physical Rev 1 board exists yet).

PV remains off and disconnected at handoff. If any Rev 0 bench work is proposed, first reread the September 8 safety restrictions referenced in `docs/rev-one-closure-plan-2026-09-16.md` Track D and preserve all safety, identity, probing, and no-`CLEAR_FAULTS` restrictions.
