# Solar Project Handoff - 2026-07-26

Status: Session closed intentionally. Fuse policy is aligned for Rev 0 first build. Next session priority is MCU replacement discovery due STM32G431CBU6 stock-out.

Date: 2026-07-26
Phase: Phase 1 - Rev 0 first-build closeout + controller sourcing pivot
Outcome: Documentation and schematic are aligned on the 5x25 fuse holder rule; next workstream is selecting an in-stock MCU alternative.

---

## 1) What Is Solid Right Now

1. F1 fuse policy is now aligned to the installed holder footprint and Rev 0 envelope:
   - Schematic F1 value updated to a neutral class placeholder: `Fuse_TD_10A_5x25_63VDC_MIN`
   - Legacy MDL-specific metadata removed from active F1 symbol fields
2. Fuse decision notes are consistent in MPPT docs:
   - `hardware/mppt/rev0-component-shortlist-2026-07-10.md` now requires `>=63 V` DC rating for the 50 V worst-case input envelope.
   - `hardware/mppt/schematic-notes.md` mirrors the same holder-first + `>=63 VDC` rule.
3. No deep component lock churn was started beyond first-build readiness intent.

---

## 2) New Blocker / Pivot

1. Primary blocker for next session: no stock for `STM32G431CBU6`.
2. Session direction change approved: pause deeper optimization work and switch to finding an in-stock replacement MCU for Rev 0 continuity.

---

## 3) Scope For Next Session (Do This First)

1. Re-open DEC-005 practically (supply exception) and shortlist 2 to 4 in-stock MCU alternatives.
2. Rank alternatives by:
   - package/footprint compatibility with current U1 implementation
   - pin/peripheral fit for currently landed nets (PWM, ADC, UART, fault inputs, buttons)
   - toolchain/firmware migration risk
   - immediate distributor availability
3. Pick one preferred alternate and one backup.
4. Only after MCU choice is confirmed:
   - update schematic U1 part value/metadata
   - update decision/docs/handoff notes in one pass

---

## 4) Guardrails For Next Session

1. Do not reopen MPPT power-stage architecture unless MCU change forces it.
2. Do not spend time on exact fuse manufacturer PN lock unless procurement asks now.
3. Keep first-build momentum: choose the least-disruptive in-stock MCU that satisfies current interfaces.

---

## 5) First Actions Next Session

1. Run `git status --short` and confirm working-tree baseline.
2. Inspect current U1 implementation and landed MCU nets in schematic/netlist.
3. Build the alternate-MCU shortlist with stock evidence and migration risk notes.
4. Propose final A/B recommendation and apply schematic/doc updates once approved.

---

## 6) Files Touched In This Session

1. `hardware/kicad/solar-project/Solar Project.kicad_sch`
2. `hardware/mppt/rev0-component-shortlist-2026-07-10.md`
3. `hardware/mppt/schematic-notes.md`

---

## 7) MCU Replacement Shortlist (Preliminary, Stock-Driven)

Evidence source used: ST eStore listing pages queried on 2026-07-26.

### Current part status

- `STM32G431CBU6` (UFQFPN-48 7x7): Out of Stock.

### In-stock candidates

1. `STM32F411CEU6` (UFQFPN-48 7x7): In Stock.
   - Why it is first choice now: vanilla F4 path, package-compatible with current U1 footprint class, strong ADC/control headroom, and minimal board disruption.
   - Cost/risk: firmware/peripheral migration from G4 to F4 still required, but lower hardware churn.
2. `STM32F401CEU6` (UFQFPN-48 7x7): In Stock.
   - Why it is backup A: same vanilla F4 direction, package-compatible, available.
   - Cost/risk: lower performance/peripheral headroom than F411, but likely enough for reduced requirements.
3. `STM32G431CBT6` (LQFP-48 7x7): In Stock.
   - Why it is backup B: lowest firmware disruption if staying in G4 family.
   - Cost/risk: requires package migration from UFQFPN to LQFP and local reroute around U1.
4. `STM32G441CBT6` (LQFP-48 7x7): In Stock.
   - Why it is fallback: still close to current family and available.
   - Cost/risk: package migration plus moderate firmware/peripheral validation.

### Checked but not currently available

- `STM32G441CBU6` (UFQFPN-48 7x7): Out of Stock.
- `STM32G474CEU6` (UFQFPN-48 7x7): Out of Stock.
- `STM32G431CBU6` (UFQFPN-48 7x7): Out of Stock.

### Recommendation for next session

1. Use `STM32F411CEU6` as preferred path (vanilla F4 + in-stock + package-compatible).
2. Keep `STM32F401CEU6` as immediate backup.
3. Keep `STM32G431CBT6` as family-continuity fallback if firmware risk dominates.

### Revised selection intent (user direction)

1. Original STM32G431 choice was primarily a fast prototype baseline with strong ADC/control headroom and easy firmware growth, not a hard production lock.
2. With external control silicon now covering more of the switching behavior, MCU requirements are reduced.
3. Package match is preferred but not mandatory for this revision.
4. Next selection pass should optimize for:
   - enough ADC channels for PV/battery/thermal sensing
   - basic timers/PWM and GPIO for control/fault handling
   - I2C/UART/SWD availability
   - immediate stock
   - lowest total reroute and firmware rework

### First implementation step after approval

1. Swap U1 to the selected UFQFPN-48 F4 symbol/part first (no package-class change).
2. Re-run pin-map audit for currently landed nets (PWM, ADC, I2C, UART, FAULT, SWD, reset).
3. Perform localized reroute around U1 and re-run ERC/DRC.
