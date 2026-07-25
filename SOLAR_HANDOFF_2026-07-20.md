# Solar Project Handoff - 2026-07-20 Recovery Stop

Status: Session went off-track during mixed schematic/erc cleanup attempts. Documentation cleanup progressed, but schematic-level closure is not yet complete. Treat this handoff as the authoritative restart point.

Date: 2026-07-20
Phase: Phase 1 - Rev 0 schematic cleanup
Outcome: Planning and ownership rules are clearer; schematic closure and clean ERC are still open.

---

## 1) What Is Solid Right Now

1. Cleanup ownership rules are documented and aligned across planning files.
2. Sheet 2 docs now state that CTRL_PWM_MAIN is the active Rev 0 control-drive path.
3. Control/fault defaults are explicit:
   - keep CTRL_EN_CHG
   - keep FAULT_OCP
   - keep FAULT_OVP
   - V_in_ON defaults to delete unless legacy branch is intentionally retained
4. U6 helper-net defaults are explicit:
   - U6_VCC2 should be closed with decoupling
   - U6_FLT should be closed with pull-up
   - U6 strap nets remain keep-only-if-intentional

---

## 2) What Is Not Closed

1. Fresh ERC still reports active errors and warnings from the current on-disk state.
2. Schematic closure is not complete enough to call the migration cleanup done.
3. The session introduced high churn while trying to combine schematic edits and ERC closure in one pass.

---

## 3) Current Working-Tree Snapshot

Modified files at stop include:

1. HANDOFF.md
2. SOLAR_PROJECT_CONCEPT_AND_HANDOFF.md
3. docs/decisions-log.md
4. docs/session-plan-2026-07-20.md
5. hardware/mppt/buck-power-stage-wiring-map.md
6. hardware/mppt/kicad-implementation-checklist.md
7. hardware/mppt/schematic-notes.md
8. hardware/mppt/sheet2-component-picks.md
9. hardware/kicad/solar-project/Solar Project.kicad_sch
10. hardware/kicad/solar-project/fp-info-cache

---

## 4) Recovery Rules For Next Session

1. Treat on-disk schematic as source of truth for this restart.
2. Before any KiCad schematic edit, check for `hardware/kicad/solar-project/~Solar Project.kicad_sch.lck`; if it exists, stop and close KiCad first so GUI saves do not overwrite text edits.
3. Do not batch many schematic edits before re-running ERC.
4. Make one focused class of fix at a time:
   - power-authority
   - symbol-pin typing
   - legacy net retirement
5. After each focused class, re-run ERC and log delta in notes.
6. Keep docs and schematic commits separate.

---

## 5) Safe Restart Sequence

1. Run git status --short and confirm file state.
2. Check whether `hardware/kicad/solar-project/~Solar Project.kicad_sch.lck` exists; if it does, close KiCad before any text edit to the schematic.
3. Open hardware/kicad/solar-project/Solar Project.kicad_sch and visually verify U6, R34, and local helper rails before edits.
4. Re-run ERC from KiCad GUI and export fresh ERC.rpt and Solar Project.net.
5. Use that fresh report as baseline and fix only one error class first.
6. Update hardware/mppt/schematic-notes.md with evidence after each pass.

---

## 6) First Focus When Resuming

Start with ERC hard errors only (not warnings), then stop and reassess before touching warning cleanup.

If the hard errors drop cleanly, proceed to net-retirement decisions (V_in_ON and legacy U2/Q2 branch) with explicit ownership notes.

---

## 7) Exit Criteria For The Next Session

1. New ERC baseline captured with timestamp.
2. At least one hard-error class resolved and documented.
3. No ambiguous ownership language left for CTRL_EN_CHG, FAULT_OCP, FAULT_OVP, and V_in_ON.
4. Next action captured in HANDOFF.md.

---

## 8) Session Closeout Addendum (Same Date)

1. The parser-corrupted schematic attempts were not used as the final source.
2. `hardware/kicad/solar-project/Solar Project.kicad_sch` was restored from:
   - `hardware/kicad/solar-project/Solar Project-backups/Solar Project-2026-07-20_050545.zip`
3. KiCad-path sync to GitHub was completed in commit:
   - `b8839b9` (`kicad: sync schematic and cache from local recovery`)
4. This handoff remains the authoritative restart logic for ERC-first recovery sequencing.

---

## 9) Fault-Block Snapshot (Added Same Date)

Current local schematic/netlist state:

1. A real first-pass fault block is now drawn in the live schematic:
   - `U9` = `LMV393` dual comparator
   - `U10` = `INA213AIDCKR` current-shunt amplifier
2. `U10` is powered from `CTRL_3V3` and `PWR_NEG`, senses `ISNSP` / `ISNSN`, and outputs `OCP_MEAS`.
3. `U9` is powered from `CTRL_3V3` and `PWR_NEG`.
4. `FAULT_OCP` and `FAULT_OVP` now have real open-collector comparator outputs with pull-ups:
   - `R53` pulls `FAULT_OCP` to `CTRL_3V3`
   - `R19` pulls `FAULT_OVP` to `CTRL_3V3`
5. `U6_FLT` is already landed on STM32 `PC13` in the current netlist, which matches the earlier backup-fault intent.

What looks good:

1. The architecture split is now materially correct:
   - current-shunt amplifier path for OCP
   - dual comparator channels for OCP and OVP
   - `U6_FLT` as controller-reported backup/status into the MCU
2. No new hard ERC errors were introduced by this block itself; the current 4 ERC errors are still the pre-existing power-authority issues (`U6 VCC2` flag conflict, `U4 VIN`, and `D3` power/pin-type semantics).
3. `OCP_REF` is already set by a usable divider:
   - `R51 = 2.7k`
   - `R52 = 1k`
   - with `CTRL_3V3`, this gives about `0.892V`
4. That `0.892V` reference is a sensible first-pass match for an `INA213`-class 50 V/V current-shunt amplifier with `R16 = 1 mOhm`:
   - trip current is about `0.892V / (50 * 0.001 ohm) ~= 17.8A`
   - this is a reasonable first-pass hardware OCP threshold above the 15A planning bound.

What is still incomplete or likely wrong:

1. STM32 fault landing is only half done:
   - `U6_FLT -> PC13` is present
   - `PB0` and `PB1` are still `no_connect` in the netlist
   - `FAULT_OCP` is not yet wired to `PB0`
   - `FAULT_OVP` is not yet wired to `PB1`
2. `OVP_REF` is currently generated by:
   - `R21 = 102k`
   - `R48 = 10k`
   - this gives about `0.295V`, not the earlier 1.25V planning example
   - this is not automatically wrong, but it is a much lower threshold reference and therefore more sensitive to comparator offset and noise.
3. `OVP_SENSE` divider values are not finalized:
   - `R20` and `R47` still show generic placeholder values in the netlist
   - the actual battery-trip point cannot be trusted until these are set and checked against the chosen `OVP_REF`.
4. `OCP_SENSE` is presently created through a divider from `OCP_MEAS` using placeholder parts:
   - `R49` and `R50` still show generic placeholder values in the netlist
   - if the goal is to compare `INA213 OUT` directly against the existing `0.892V` reference, this divider is probably unnecessary
   - leaving that divider in place without intentional values will obscure the actual OCP threshold.
5. `FAULT_OCP` and `FAULT_OVP` are presently separate comparator outputs only; there is not yet a documented or drawn shared hardware kill net such as `FAULT_SUM_N` or `CHG_KILL_N`.

Recommended next-session order for this block:

1. Check for `hardware/kicad/solar-project/~Solar Project.kicad_sch.lck` before any edit.
2. Keep `U10` + `U9` as the selected first-pass protection architecture; do not reopen the “single vs dual comparator” question unless the schematic proves a blocker.
3. Finish the MCU landing:
   - wire `FAULT_OCP -> PB0`
   - wire `FAULT_OVP -> PB1`
   - keep `U6_FLT -> PC13`
4. Decide whether `OCP_MEAS` should feed the comparator directly:
   - simplest Rev 0 path is `OCP_MEAS -> OCP_SENSE` with no attenuation
   - if so, replace the `R49/R50` placeholder divider with a direct connection or a clearly intentional RC filter
5. Finalize the OVP threshold pair:
   - either keep the current low-reference strategy and calculate `R20/R47` explicitly for the intended trip voltage
   - or raise `OVP_REF` to a sturdier threshold before sizing the battery divider
6. Add or document the hardwired shutdown path driven by comparator faults so the system still protects itself if firmware is absent.

Working recommendation for Rev 0:

1. Treat `U10` as the shunt-conditioning stage and keep its high-impedance Kelvin input role.
2. Keep `U9A` as OVP and `U9B` as OCP.
3. Use `CTRL_3V3` / `PWR_NEG` as the comparator and pull-up rail domain.
4. Prefer direct `OCP_MEAS` comparison into the OCP comparator unless a deliberate filter/attenuation network is justified and calculated.
5. Keep `FAULT_OCP`, `FAULT_OVP`, and `U6_FLT` as distinct diagnosis/status nets even if they are later OR-ed into one hardware shutdown path.
