# Solar Project 200W - Rev 0 Populated-Bridge Staged First-Power Plan

Drafted: 2026-09-04
Status: DRAFT - NOT REVIEWED - NOT AUTHORIZED FOR EXECUTION
Applies to: recovered Rev 0 board with M1-M4 (`BSC030N04NSGATMA1`) installed per `rev0-m1-m4-installation-plan-2026-09-04.md`

This plan is documentation only. No stage may be executed until this plan is reviewed and the stage's entry gate is explicitly signed off in the bench record. Drafting and execution are separate sessions by rule.

## 1. Scope and Structure

The populated bridge changes the hazard model: crossing U6 `EN_UVLO` rising threshold with M1-M4 fitted can start real converter switching, which was impossible during all prior powered work. This plan therefore separates:

- **FP0** - review, bypass disposition, and pre-power passive recheck (unpowered).
- **FP1** - control power only over USB (no PV; U6 unpowered).
- **FP2** - below-UVLO PV static (U6 alive on I2C, switching impossible).
- **FP4** - first controlled converter start (switching). **Blocked**: defined here only as an entry gate and hazard bound; it requires its own reviewed authorization and may not be run in the same session as FP0-FP2.
- **FP3** - above-UVLO PV static with U6 held disabled (switching inhibited by EN_UVLO hold-low), performed **after** FP4 rather than before it (see Section 1a).

Each stage has an entry gate, bounded conditions, stop thresholds, and a recorded pass/fail result. A stage pass never implies the next stage is authorized.

## 1a. Stage Sequencing Decision (Review 2026-09-04)

Review decision: FP3 is reordered to run after FP4 passes, not before it. Rationale recorded from the board owner: the board has already absorbed significant rework stress, and the priority is proving the converter switches correctly before investing further attended bench time in a static-only EN_UVLO hold-low characterization step. Consequence accepted by this reordering: the first time `EN_UVLO` is allowed to cross its rising threshold with the bridge populated (in FP4), U6 will attempt to start switching directly — there is no intermediate proof of "above-UVLO but not switching" before that first switching attempt. This makes the FP4 entry gate (Section 15) carry the full weight that FP3 would otherwise have partly absorbed; FP4's bounded low-VIN/low-current/short-duration/preloaded conditions (Sections 8 and 11) are the compensating control for removing that intermediate step. FP3, if still performed later, becomes a post-FP4 static characterization exercise rather than a pre-switching safety gate.

## 2. Standing Prohibitions (all stages)

- No real panel, battery, inverter, AC wiring, or uncontrolled load.
- No unattended operation while any source is connected.
- Keep `PA8`, `PB5`, and legacy `PA5` low. Never drive `PB11` (tied to `PWR_NEG` on Rev 0).
- No LM51772 register writes. Never access `CLEAR_FAULTS` at `0x03`.
- SWD only via ST-LINK serial `B55B5A1A0000000020AFF501`; verify Device ID `0x469`, STM32G47x/G48x family, 512 KB NVM, plausible target voltage before every operation. Serial only via CP210x `USB\VID_10C4&PID_EA60\0001` on COM5, 115200 8N1. Stop on any identity mismatch; never select another COM port.
- No ground-referenced oscilloscope lead on `SW_1`, `SW_2`, `HO1`, `HO2`, `HB1`, or `HB2`.
- The prior 6.49 V / 20 mA authorization predates M1-M4 installation and is void; each stage below re-authorizes its own source point.

## 3. Item 1 - F1 / U2 / U5 Bypass Disposition

Current state: a jumper spans F1 (input fuse defeated); U2 (input reverse-blocking `BSC112N06LD` common-source pair) and U5 (output-path equivalent) are jumpered because of the Rev 0 invalid gate-drive defect. The R31-pad-2 to R32-pad-2 `PGND`-to-`PWR_NEG` jumper is a required Rev 0 repair and stays.

Disposition for this plan:

- **Accepted for FP0, FP1, FP2, FP4, and (if later performed) FP3**, under these compensating controls: isolated bench source, hard source current limit (the only overcurrent protection with F1 bypassed), attended operation, and stop thresholds below.
- **U5 bypass consequence**: the output rail has no reverse blocking. Therefore no output-side source of any kind (battery, second supply, charged capacitor bank) may be connected in any stage of this plan. Output connections are limited to a passive resistive preload or an electronic load in constant-resistance mode.
- **FP4 precondition**: before FP4, a reviewer must either (a) restore/rework F1, U2, and U5, or (b) record a written waiver stating why bench-only switching with bypasses is acceptable, with the source current limit and preload named as the substitute protection. Option (b) is acceptable only for the first bounded switching characterization; any battery/panel/inverter work requires (a). This waiver carries more weight than originally drafted because FP3's pre-switching static proof is no longer a prerequisite (Section 1a).

## 4. Item 2 - First-Stage Selection

The first powered stage is **FP1 control power only** (USB VBUS path, PV open). Rationale: it re-validates the control domain against the newly populated bridge with U6's power path completely unpowered, and it reuses the already-verified COM5/stage1 configuration. Below-UVLO PV (FP2) follows only after FP1 passes.

## 5. Item 5 - Firmware Image and Safe-State Proof

- Image: the currently flashed `stm32g4_rev0_stage1` bare-metal diagnostic firmware (source `firmware/bringup/baremetal/stm32g4_rev0_stage1.c`). No new image may be flashed as part of this plan; if a reflash becomes necessary, it is a stop-and-review event.
- Proof required at FP1, and re-checked at every later stage entry:
  1. Cold boot, then COM5 `status` must report `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`.
  2. Press S1 reset, repeat `status`, same result. Repeat the boot/reset pair twice.
  3. DMM at the MOSFET footprints: M1 `HO1`-to-`SW_1`, M2 `LO1`-to-`PWR_NEG`, M3 `LO2`-to-`PWR_NEG`, M4 `HO2`-to-`SW_2` all ≤ 0.1 V.
- Read-only U6 access is limited to `STATUS_BYTE` (`0x78`) via the stage1 UART path or `firmware/scripts/rev0-openocd-u6-status.ps1` (which saves/restores GPIOB and resumes the MCU).

### 5.1 FP1 SWD Identity Verification (2026-09-04)

STM32CubeProgrammer CLI, read-only `-c port=SWD` connect (no flash/memory write): ST-LINK SN `B55B5A1A0000000020AFF501` (matches approved), Device ID `0x469` (matches), Device name `STM32G47x/G48x/G414` (matches), NVM size 512 KBytes (matches), target voltage 3.23 V (plausible for `CTRL_3V3`). All four required identity checks pass. Two earlier connection failures (OpenOCD, multiple speeds, with and without reset-assert; a first CubeProgrammer attempt) are now attributed to the board not actually being powered during those attempts, not a hardware SWD fault.

### 5.2 FP1 Safe-State Proof (2026-09-04, in progress)

COM5 `status` results, read-only:

| Check | Result |
|---|---|
| Cold boot | `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0` — matches required |
| S1 reset #1 | `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0` — matches required |
| S1 reset #2 | `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0` — matches required |

Cold boot plus both required reset cycles pass with an identical, matching safe-state result. DMM at the MOSFET footprints: `HO1`-to-`SW_1` (M1), `LO1`-to-`PWR_NEG` (M2), `LO2`-to-`PWR_NEG` (M3), `HO2`-to-`SW_2` (M4) all read 0 V — pass.

**FP1 status: PASSED.** SWD identity confirmed (5.1), safe-state proof confirmed across cold boot and two reset cycles with all four gate outputs at 0 V (5.2). FP2 (below-UVLO PV static) is authorized to begin.

### 5.3 FP2 Attempt and Abort (2026-09-04) - UNRESOLVED OPEN FINDING

FP2 was attempted at PV 5.0 V / 20 mA current limit. Input current stayed below 1 mA and `EN_UVLO` read 1.0263 V (both within bounds), but `HO1`-`SW_1` and `HO2`-`SW_2` initially read 4.31 V and `LO1`/`LO2`-to-`PWR_NEG` read 4.77 V — all far above the 0.1 V stop threshold. **Abort executed per Section 14**: source disconnected, all bench nodes confirmed ≤0.12 V after bleed-off.

On re-inspection after the abort, with the PV source disconnected: `HO1`/`HO2` read 0 V (as expected), but `LO1`/`LO2`-to-`PWR_NEG` still read 4.77 V with real drive strength — a 10 kΩ load only pulled it to 4.11 V (a floating/capacitively-coupled node would collapse toward 0 V under this load; this did not). Direct measurement at U6 `VCC2` (pin 29) showed the same behavior: a 10 kΩ load there also only pulled `VCC2` to 4.11 V. `LO1` is therefore riding directly on `VCC2`, not floating independently.

Root cause is **not confirmed**. The board's own bench history (`docs/test/rev0-low-energy-session-2026-08-30.md`) documents a residual, never-fully-isolated ~300 kΩ path from D8 pad 2 to U6 pin 29 (`VCC2`) after the D8-to-`VCC2` rework; D8 now sits on USB `VBUS`. This is a plausible explanation (`VBUS` backfeeding `VCC2` through that residual path while USB is connected) but was **not verified this session** — the decisive test (fully removing USB and confirming `VCC2` collapses) was not completed; board owner elected to stop pursuing this diagnostic for this session.

**Disposition:** FP2 is not passed. This finding blocks FP2 re-attempt, FP4, and FP3 until resolved: either (a) confirm `VCC2` fully collapses to near 0 V with USB and ST-LINK both physically disconnected, or (b) identify and close the actual backfeed path. A live gate-driver bias rail with real source impedance, present even with PV fully removed, is not an accepted precondition for any further powered stage on this plan.

## 6. Item 6 - Oscilloscope Strategy

- Static stages (FP1-FP2, and FP3 if later performed): DMM measurements suffice; a ground-referenced scope probe is permitted only on `PWR_NEG`-referenced nets (`CTRL_3V3`, `VBUS`, `EN_UVLO`, `LO1`, `LO2`, `PV_IN_POS`).
- High-side gate-source (`HO1`-`SW_1`, `HO2`-`SW_2`) and `HB1`-`SW_1` / `HB2`-`SW_2` measurements require a differential probe or a battery-isolated scope. If neither is available, high-side dynamic V_GS is simply not measured; FP4 may not proceed without one.
- FP4 minimum scope set: CH1 differential on `HO1`-`SW_1` (or `HO2`-`SW_2` for the boost leg), CH2 ground-referenced on `SW_1` with a short ground spring (no long ground clip), single-shot capture armed before enabling the source.

## 7. Item 7 - Bootstrap Network

- `C22` 100 nF 100 V X7R 0805, `HB1` (U6 pin 25) to `SW_1`. `C23` 100 nF 100 V X7R 0805, `HB2` (U6 pin 31) to `SW_2`. Bootstrap diodes are internal to the LM51772.
- FP0 unpowered check: `HB1`-to-`SW_1` and `HB2`-to-`SW_2` must read as a capacitor (no short, no open drift to 0 Ω); record stabilized resistance.
- Expected static behavior (FP2/FP3): with the low-side devices off and no switching, HB-SW may sit anywhere from 0 V up to roughly VCC minus a diode drop depending on leakage; any HB-SW reading above ~6 V, or a gate output following HB, is a stop condition.
- Expected FP4 behavior: HB-SW charges to approximately VCC minus the internal diode drop during each low-side on-time; a first-start capture showing HB-SW collapsing below ~3 V during high-side on-time indicates bootstrap starvation and is a stop condition.

## 8. Item 8 - Dead Time, Startup Duty, Pulse Count, Run Duration

- Dead time is internal and adaptive in the LM51772 gate drivers; it is not firmware-adjustable. The STM32 does not gate individual pulses; once `EN_UVLO` crosses its rising threshold with valid VCC, U6 starts autonomously through its internal soft-start. Single-pulse testing is therefore **not available** on this hardware.
- The bounded substitute for FP4: lowest practical VIN just above the UVLO rising point (target 6.5 V, matching the previously characterized source point), hard source current limit (Section 11), passive preload fitted, first run duration ≤ 30 s attended, then source off and full discharge before inspection. Maximum cumulative first-session switching time: 5 minutes.
- Shoot-through protection relies entirely on U6's internal adaptive dead time; the compensating bound is the source current limit, which must be low enough that a full shoot-through event current-limits the source rather than the silicon (Section 11).

## 9. Item 9 - Missing Series Gate Resistors

Rev 0 has no series gate resistors; U6 drives the `BSC030N04NSGATMA1` gates (Qg ≈ 25 nC class, 40 V V_DS rating) directly.

Consequences to expect and bound:

- Fast edges → high dV/dt on `SW_1`/`SW_2`, ringing and overshoot with layout inductance. Stop threshold: any SW-node overshoot above **32 V** (80% of the 40 V rating) at the FP4 source point is an immediate stop; do not raise VIN to "see where it fails".
- Gate ringing → possible Miller-induced (Cdv/dt) turn-on of the off device. Watch low-side V_GS on CH2-class ground-referenced probing; any off-state gate excursion above ~1.5 V is a stop condition.
- EMI: expect it; it is not itself a stop condition at bench scale, but COM5/SWD instability during switching is (it indicates coupling into the control domain).
- Disposition: series gate resistors are a Rev 1 correction. FP4 accepts the risk only at the minimum-VIN, minimum-load, time-bounded point above.

## 10. Item 10 - Thermal and Ringing Observations

At each powered stage, log before/after (and at 60 s intervals during FP4, and during FP3 if later performed): M1-M4 cases, L2, R22 shunt, U6, the U2/U5 bypass jumpers, and the F1 jumper — by IR thermometer or calibrated touch. Stop thresholds: any component more than 15 °C above ambient in FP1-FP2 (nothing should be dissipating), or more than 40 °C above ambient in FP4 or FP3, or any odor, discoloration, or audible instability.

## 11. Items 3-4 - Source, Output, and Stop Thresholds per Stage

| Stage | Source and limit | Output side | Key stop thresholds |
|---|---|---|---|
| FP1 | USB VBUS only (single source rule for D8) | Open | VBUS current limiting, CTRL_3V3 outside 3.3 V ±5%, safe-state proof failure |
| FP2 | Isolated bench PV at **5.0 V / 20 mA**, raised from 0 V | Open | Input current > 5 mA sustained, `EN_UVLO` ≥ 1.2 V, any gate ≥ 0.1 V, `STATUS_BYTE` unreadable |
| FP4 | Isolated bench PV at **6.5 V / 200 mA** (confirmed) | Resistive preload: two 100 Ω / 10 W resistors in series (200 Ω / 20 W combined), sized against confirmed 12.83 V setpoint, see Section 11.1 | SW overshoot > 32 V, off-gate > 1.5 V, bootstrap collapse, current limit, thermal limits, COM5/SWD dropout, run time > 30 s |
| FP3 (post-FP4) | Isolated bench PV at **6.5 V / 20 mA**, with `EN_UVLO` held low (Section 12) | Open | Input current > 10 mA sustained, any gate ≥ 0.1 V, any SW-node movement, HB-SW > 6 V |

Universal aborts (all stages): source current limiting, unexpected current rise, rail collapse, heating, odor, smoke, unstable reset, unexpected gate voltage, any active control output, or any reading that contradicts this plan's expectations.

### 11.1 FP4 Preload Sizing (Resolved 2026-09-04)

**VOUT setpoint confirmed by hardware analysis, no register access needed.** Corrected finding: `FB_INT` (U6 pin 14, `FB/SEL_intFB`) is **not** floating — netlist net `_OUTPUT Feedback` shows it fed by an external resistor divider: `R3` = 66.5 kΩ from `V_out` (the converter's raw output node, same copper as `CHG_OUT_POS` through F2) to `FB_INT`, and `R25` = 5.62 kΩ from `FB_INT` to `PWR_NEG`. The LM51772-Q1 datasheet confirms this is the documented external-feedback mode (internal feedback would instead require tying this pin to `VCC2`, which is not done here) and specifies the feedback reference as `V(REF) = 1.00 V ±10 mV`.

Calculated setpoint: V_out = V(REF) × (1 + R3/R25) = 1.00 V × (1 + 66.5k/5.62k) ≈ **12.83 V**. This matches the 12.8 V WEBENCH design point almost exactly and supersedes the earlier 12.8-14.6 V planning band — treat **12.83 V** as the confirmed FP4 target, not an assumption.

Local pypdf extraction of `docs/lm51772-datasheet.pdf` (TI's web document-viewer remained unreadable via available tools after repeated attempts — do not retry that path) also decoded **Table 7-5 "CFG2 Pin (R2D-CH2) Configuration Overview"**: the fitted `CFG2` strap (R29 = 20.5 kΩ, row 13 of 16) sets `EN_SYNC_OUT=DISABLED`, `SYNC_IN_FALLING=DISABLED`, `VDET_EN=ENABLED`, `PCM_EN=ENABLED` (~30% window). `VDET_EN` adds a second internal input-voltage detector alongside the external `EN_UVLO` divider. `PCM_EN` creates an input-voltage pass-through window centered near the 12.83 V output target (input ≈ output, both high-side FETs on, no real switching) — since FP2/FP4 operate at 5.0-6.5 V input, well below that ~9-16.6 V window, PCM pass-through is not expected to engage during this plan's bounded tests; it only becomes relevant for later normal-PV-voltage charging characterization. Cross-check: this same decode method correctly reproduced the already-confirmed `R33 = 0 Ω → I2C enabled at 0x6A` result from Table 7-4, validating the row/column decode.

Bounded calculation for preload sizing at the confirmed 12.83 V setpoint, using the FP4 source limit (6.5 V / 200 mA, so P_in ≈ 1.3 W) and an assumed 85% conversion efficiency:

| V_out (confirmed) | Natural I_out = V_out / R | R = 200 Ω → I_out | P per 100 Ω resistor | Required P_in / I_in at 6.5 V |
|---|---|---|---|---|
| 12.83 V | V_out / R | ≈ 64 mA | ≈ 0.41 W | ≈ 0.97 W / ≈ 149 mA |

**Selected part (confirmed on hand): two 100 Ω / 10 W resistors in series = 200 Ω / 20 W combined rating.** At 200 Ω the natural, voltage-regulated output current (≈64 mA) requires only ≈149 mA of input current — comfortably under the 200 mA limit (≈26% margin) — so the converter is expected to reach real closed-loop regulation at its actual 12.83 V target rather than sit input-current-limited. Per-resistor dissipation (≈0.4 W) is roughly 25x under each resistor's 10 W rating. This closes the preload-sizing open item; no further register read is required before FP4.

## 12. FP3 EN_UVLO Hold-Low Provision (Now Post-FP4, Section 1a)

FP3 exists to prove the populated board's static rails above UVLO without permitting switching. Per the 2026-09-04 reorder, it is performed after FP4 passes rather than before it, as a follow-on static characterization rather than a pre-switching gate. Method: a documented, photographed temporary jumper pulling the `EN_UVLO` node to `PGND` **installed and verified unpowered** before the source is raised past 5.5 V, and removed only after the source is off and rails are discharged. If review rejects grounding `EN_UVLO` (e.g., divider loading concern), FP3 is dropped entirely rather than folded into FP4, since FP4 no longer depends on it. Rationale: with M1-M4 installed, "above-UVLO static" does not otherwise exist — U6 will attempt startup.

Note: the historical `IOUT` latch (`STATUS_BYTE=0x58`) was associated with UVLO crossing while the bridge was absent. FP4 (and FP3, if performed) must re-read `STATUS_BYTE` and record it; a latched fault with the populated bridge is new information, not a repeat, and is a stop-and-review event. It is not a justification to write `CLEAR_FAULTS`.

## 13. Item 11 - FP0 Pre-Power Rechecks and Evidence

Repeat, unpowered and discharged, immediately before FP1 (not reusing 2026-09-04 results if any rework has occurred since):

1. `PV_IN_POS` to `PWR_NEG`, output rail to `PWR_NEG`, `SW_1` to `SW_2`: no near-short; record stabilized values.
2. Gate-to-source at all four devices; body-diode direction and reverse blocking after gate discharge.
3. `HB1`-`SW_1` / `HB2`-`SW_2` bootstrap checks (Section 7).
4. Confirm F1 jumper, U2/U5 bypasses, and the PGND jumper are intact and photograph them.
5. Confirm R28 = 51.7 kΩ node, R29 = 20.5 kΩ node, R33 = 0 Ω are undisturbed (visual + resistance at the U6 pins).
6. Photograph both board sides; file with the bench record.

Evidence capture per stage: source model/voltage/current-limit setting, meter readings named per net, `status` output text, `STATUS_BYTE` value with timestamp, thermal log, photos of any temporary jumper, and an explicit pass/fail line signed in the bench record.

### 13.1 FP0 Recorded Results (2026-09-04, in progress)

| Item | Reading | Assessment |
|---|---|---|
| `PV_IN_POS`-to-`PWR_NEG` | 43 kΩ | Pass — no near-short |
| Output rail (`CHG_OUT_POS`)-to-`PWR_NEG` | 2 MΩ, rising ("charging") | Pass — output cap bank charging through meter, no near-short |
| `SW_1`-to-`PWR_NEG` | ≥10 MΩ | Pass — M2 off, no leakage |
| `SW_2`-to-`PWR_NEG` | ≥10 MΩ | Pass — M3 off, no leakage |
| `SW_1`-to-`SW_2` | 0.39 Ω | Pass — expected. This is not a short between two independent nodes: the netlist shows `SW_1` reaches `SW_2` only through `R22` (pin4→pin1, in-line current-sense element) in series with `L2` (the single buck-boost inductor; there is no populated `L1` in this design). 0.39 Ω is plausible combined sense-resistor + inductor DCR, not a bridge defect. |
| Gate-to-source: M1 | 10 MΩ | Pass |
| Gate-to-source: M2 | 13.4 MΩ | Pass |
| Gate-to-source: M3 | 7.67 MΩ | Pass |
| Gate-to-source: M4 | 10 MΩ | Pass |
| Body-diode direction/reverse-blocking, M1-M4 | Confirmed reverse blocking on all four | Pass |
| `HB1`-to-`SW_1` | 3.2 MΩ, rising ("charging") | Pass — capacitor-charging behavior as expected, not a short or dead open |
| `HB2`-to-`SW_2` | 2.62 MΩ, rising ("charging") | Pass — same |
| F1 / U2 / U5 bypass jumpers intact | Confirmed, documented | Pass |
| `PGND`-to-`PWR_NEG` jumper intact | Confirmed, documented | Pass |
| R28 = 51.7 kΩ (`RT`) | Confirmed undisturbed | Pass |
| R29 = 20.5 kΩ (`CFG2`) | Confirmed undisturbed | Pass |
| R33 = 0 Ω (`ADDR/SLOPE`) | Confirmed undisturbed | Pass |
| Photographs, both board sides | Waived by board owner ("prototype, not a Falcon 9") | Waived — not blocking |

**FP0 status: PASSED.** All passive rechecks, jumper/strap confirmations, and gate-source/body-diode checks pass. Photo documentation is explicitly waived by the board owner for this session and is no longer a blocking item. FP0 is signed off; FP1 (USB-only control power, PV still disconnected) is authorized to begin.

## 14. Item 12 - Abort and Discharge Procedure

On any stop condition, in order:

1. Source output off (or physically disconnect if the supply lacks an output switch). Do not "try again" at the same point.
2. Remove USB and ST-LINK.
3. Wait ≥ 60 s, then verify `PV_IN_POS_RAW`, `PV_IN_POS`, `VBUS`, `CTRL_3V3`, the output rail, `SW_1`, `SW_2`, and `HB1`/`HB2` (to `PWR_NEG`) are all below 0.1 V. If any node holds charge, bleed it through a ≥ 1 kΩ resistor, never a direct short.
4. Record the trigger, all instrument readings at the time, and the board state before any inspection or rework.
5. Re-entry to any stage after an abort requires review of the recorded trigger.

## 15. Stage Gate Summary

| Stage | Entry gate | Authorized by this plan? |
|---|---|---|
| FP0 | Plan reviewed and signed | **Yes - passed 2026-09-04 (Section 13.1)** |
| FP1 | FP0 pass recorded | **Yes - passed 2026-09-04 (Section 5.2)** |
| FP2 | FP1 pass recorded | **Attempted, ABORTED, unresolved (Section 5.3) - do not re-attempt until `VCC2` backfeed finding is closed** |
| FP4 | FP2 pass + bypass disposition waiver (Section 3) + differential probe available + preload value signed (Section 11.1, resolved) | **No - requires separate authorization session** |
| FP3 (optional, post-FP4) | FP4 pass recorded + EN_UVLO hold-low method approved | No - review pending |

## 16. Rev 1 Corrections Reaffirmed (not blocking FP0-FP2)

Series gate resistors on HO1/LO1/HO2/LO2; F1/U2/U5 proper protection restoration; D8 local 100 nF bypass; PB11 net repair; PGND net repair made permanent.
