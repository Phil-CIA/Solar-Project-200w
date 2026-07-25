# Solar Project Handoff - 2026-07-25

Status: PCB routed and DRC-clean. Schematic complete with STM32 support hardware. Working tree still has local edits to review and commit.

Date: 2026-07-25
Phase: Phase 1 - Rev 0 schematic complete, PCB routed
Outcome: Full schematic-to-layout pass completed this session. Pre-route snapshot preserved. DRC is clean; next session is a review pass before commit/push.

---

## 1) What Is Solid Right Now

1. Schematic is complete for Rev 0 scope:
   - STM32G431 HSE crystal: X2 (8 MHz/20 pF), C49/C50 (33 pF NP0), correctly wired shunt topology
   - STM32G431 reset: S1 button + R59 pull-up + C47 debounce cap on MCU_RST_N / PG10
   - 5-way nav pad: SW1–SW5 (tactile), R54–R58 (10k pull-ups), BTN_UP/DN/LT/RT/ENT on PB3/PB4/PB9/PA15/PC14
   - OCP/OVP protection nets landed on STM32 PB0/PB1
   - U6_FLT landed on PC13
   - CTRL_SUPPLY_IN OR-ing diodes D4/D6/D7 wired
   - Control rail U4 (LM5163) as CTRL_3V3 owner
   - Display U8, temp sensor U7, USB bridge U3 all present
2. PCB placement and routing are done:
   - Pre-route snapshot: `hardware/kicad/solar-project/Solar Project PreRoute save.kicad_pcb`
   - Post-route board: `hardware/kicad/solar-project/Solar Project.kicad_pcb`
3. Freerouting DSN export present for reference: `hardware/kicad/solar-project/freerouting.dsn`

---

## 2) What Is Not Closed

1. The board is DRC-clean, but the changes still need a review pass before the next clean commit.
2. The working tree still has local edits that should be reviewed and committed together with the updated handoff.
3. Net names for crystal and reset remain auto-generated (`Net-(U1-PF0-OSC_IN)`, `Net-(S1-A)`) unless you want a cosmetic rename before commit.

---

## 3) Working Tree At Stop

Modified (uncommitted):
- `HANDOFF.md`
- `SOLAR_HANDOFF_2026-07-20.md`
- `docs/decisions-log.md`
- `hardware/kicad/solar-project/Solar Project.kicad_pcb` ← post-route board
- `hardware/kicad/solar-project/Solar Project.kicad_pro`
- `hardware/kicad/solar-project/Solar Project.kicad_sch`
- `hardware/kicad/solar-project/fp-info-cache`
- `hardware/mppt/kicad-implementation-checklist.md`
- `hardware/mppt/net-plan.md`
- `hardware/mppt/schematic-notes.md`
- `hardware/mppt/sheet2-component-picks.md`
- `hardware/mppt/sheet4-sensing-control-wiring-map.md`

Untracked (do NOT commit):
- `_autosave-Solar Project.kicad_pcb`
- `~_autosave-Solar Project.kicad_pcb.lck`
- `freerouting.dsn`
- `report.txt`

Untracked (commit only after DRC clean):
- `Solar Project PreRoute save.kicad_pcb` (keep as safety snapshot)
- `hardware/mppt/stm32-support-implementation-checklist.md`

---

## 4) Safe Restart Sequence

1. Run `git status --short` — confirm the post-review working tree.
2. Open `hardware/kicad/solar-project/Solar Project.kicad_pcb` in KiCad.
3. Do a review pass on placement, net naming, and any remaining documentation deltas.
4. Commit docs first, then schematic, then PCB as separate commits.
5. Do not export gerbers or order until the review pass is complete and the commit is pushed.

---

## 5) Review Priorities

Focus the next pass on:
- board placement quality around the power stage and connector edges
- any schematic naming cleanup you still want before locking the commit
- ensuring the docs match the clean DRC state and the finished routing status
- confirming no stale generated files are accidentally committed

---

## 6) Ownership Decisions Locked This Session

| Decision | Outcome |
|---|---|
| Q2 / V_in_ON | Keep as reserved future input-protection option; non-primary in Rev 0 |
| U2/Q2 branch | Quarantined legacy, non-primary |
| CHG_KILL_N shutdown path | Deferred for this revision |
| Crystal load caps C49/C50 | 33 pF NP0 for 20 pF crystal CL |
| Crystal reference | X2, 8 MHz, Yangxing X49SM8MSD2SC |

---

## 7) Next Session Goals

1. Review the routed PCB and schematic one more time before locking the snapshot.
2. Commit the current working tree in clean, ordered commits.
3. Push the commit so the next session starts from a stable baseline.
4. Continue with board review only if something still looks off after commit.
