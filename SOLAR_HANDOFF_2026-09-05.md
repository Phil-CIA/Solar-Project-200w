# Solar Project 200W - Rev 0 First-Power Session Handoff (2026-09-05)

## Stop State

The board is disconnected from the PV bench source, which was confirmed bled to ≤0.12 V at all measured nodes after the FP2 abort. **The USB and ST-LINK connection state at the very end of the 2026-09-04 session was not confirmed** — the board owner declined to complete the decisive USB-removal test before stopping. Before any action this session, physically verify USB and ST-LINK are disconnected, then re-measure `VCC2` (U6 pin 29) and `LO1`/`LO2`-to-`PWR_NEG` at rest. Do not reconnect PV, a battery, an output source/load, a panel, an inverter, or AC. Do not command gate drive or switching.

This is not a fresh start: FP0 and FP1 of the staged first-power plan passed on 2026-09-04. FP2 was attempted, hit a real stop condition, and was aborted correctly. **FP2 is not resolved and must not be re-attempted until the open finding below is closed.**

## Open Safety Finding (Blocking) - Live `VCC2`/`LO1` Rail After Power Removal

During the FP2 abort sequence, with the PV source disconnected and bled to ≤0.12 V, `HO1`/`HO2` correctly read 0 V, but `LO1`/`LO2`-to-`PWR_NEG` still read **4.77 V**. This is not a floating/capacitively-coupled artifact: a 10 kΩ load only pulled it to 4.11 V (a true high-impedance node would collapse toward 0 V under that load). Direct measurement at U6 `VCC2` (pin 29) showed the identical behavior (4.77 V → 4.11 V under the same 10 kΩ load), confirming `LO1` rides directly on `VCC2`.

Root cause is **not confirmed**. The leading hypothesis, not yet tested: the board's own bench history (`docs/test/rev0-low-energy-session-2026-08-30.md`, 2026-08-31 rework entry) documents a residual, never-fully-isolated **~300 kΩ path from D8 pad 2 to U6 pin 29 (`VCC2`)** left over from the D8-to-`VCC2` rework; D8 now sits on USB `VBUS`. If USB remains connected, `VBUS` could be backfeeding `VCC2` through that residual path. The decisive test — fully removing USB and ST-LINK and confirming `VCC2` collapses toward 0 V — was **not completed** in the prior session.

A secondary theory floated during the session (that this is an intentional LM51772 "fail-safe" behavior to discharge the SW nodes) was checked against the datasheet and is **not supported**: Section 7.3.9 "Output Voltage Discharge" is a register-controlled internal circuit that discharges `VOUT` via the output capacitor node, not something documented as actively driving the `LO1`/`LO2` external gate pins. Do not reuse that theory without new evidence.

**This finding blocks FP2 re-attempt, FP4, and FP3 until either:**
(a) `VCC2` is confirmed to fully collapse to near 0 V with USB and ST-LINK both physically disconnected, ruling out an external backfeed, or
(b) the actual backfeed/retention path is identified and closed.

## Current Board Configuration

- U6 is an LM51772-Q1 in the RHA-40 package. R28 = 51.7 kΩ (`RT`), R29 = 20.5 kΩ (`CFG2`), R33 = 0 Ω (`ADDR/SLOPE`, selects I2C `0x6A`) — all confirmed undisturbed this session.
- `CFG2` = 20.5 kΩ decodes (datasheet Table 7-5, row 13 of 16) to `EN_SYNC_OUT=DISABLED`, `SYNC_IN_FALLING=DISABLED`, `VDET_EN=ENABLED`, `PCM_EN=ENABLED` (~30% input-voltage pass-through window centered near the confirmed 12.83 V output target, roughly 9-16.6 V input). This window does not engage at the FP2/FP4 bench test points (5.0-6.5 V input); it only matters for later normal-PV-voltage charging characterization.
- U6's actual VOUT regulation setpoint is now **confirmed by hardware analysis, not assumed**: `FB_INT` (U6 pin 14) is fed by an external divider, `R3` = 66.5 kΩ (top, from the output node) and `R25` = 5.62 kΩ (bottom, to `PWR_NEG`). With the datasheet's `V(REF) = 1.00 V`, this gives V_out = 1.00 × (1 + 66.5/5.62) ≈ **12.83 V**, matching the 12.8 V WEBENCH design point.
- F1, U2, and U5 protection functions remain bypassed (jumpered); this disposition is accepted for FP0/FP1/FP2/FP4 (and FP3 if performed) under the plan's compensating controls, but the FP4-specific waiver (restore F1/U2/U5, or record a written bench-only waiver) is still an open decision.
- M1-M4 are installed `BSC030N04NSGATMA1` devices; Rev 0 has no series gate resistors.
- FP4 preload is finalized: two 100 Ω / 10 W resistors in series (200 Ω / 20 W combined), sized against the confirmed 12.83 V setpoint — this remains valid and does not need rework.

## Session Progress: Staged First-Power Plan

The full plan lives at `docs/test/rev0-first-power-plan-2026-09-04.md` (FP0 through FP4, with FP3 reordered to run after FP4 per an earlier review decision). Status as of this handoff:

| Stage | Result |
|---|---|
| FP0 (unpowered passive recheck) | **PASSED** (Section 13.1) |
| FP1 (USB-only control power, safe-state proof) | **PASSED** (Sections 5.1, 5.2) — SWD identity confirmed, `status` matched across cold boot + 2 resets, all four gates 0 V |
| FP2 (below-UVLO PV static) | **Attempted, ABORTED, unresolved** (Section 5.3) — see the blocking finding above |
| FP4 (first switching) | Not attempted; still requires the F1/U2/U5 waiver decision plus FP2 resolution |
| FP3 (post-FP4 static, optional) | Not attempted |

## Control and Debug State

- Active MCU family evidence: Device ID `0x469`, STM32G47x/G48x/G414-class Cortex-M4, 512 KB NVM.
- Exact approved ST-LINK serial: `B55B5A1A0000000020AFF501`. Exact CP210x identity: `USB\VID_10C4&PID_EA60\0001` on `COM5` only, 115200 8N1.
- **Lesson from this session**: when SWD "unable to connect to the target" persists across multiple tools (OpenOCD, STM32CubeProgrammer CLI), multiple speeds, and with/without reset-assert, but a plausible target voltage is still reported, explicitly re-confirm the board is actually powered before doing deeper hardware troubleshooting — that was the actual cause here, not a hardware fault. STM32CubeProgrammer CLI (`STM32_Programmer_CLI.exe -c port=SWD`) is a useful independent cross-check against OpenOCD; use `python -m pip show <pkg>` in the actual terminal to confirm a package install target lands in the interpreter you expect.
- Working method to query COM5 `status` non-interactively: PowerShell `System.IO.Ports.SerialPort` (115200 8N1), single-line command form (multi-line here-strings echoed garbled in this session's terminal), `WriteLine("status")` then loop `ReadLine()` with try/catch on timeout.
- No LM51772 register was written this session. Never access `CLEAR_FAULTS` at `0x03`.

## Hard Safety Limits

- Verify the exact ST-LINK serial, Device ID `0x469`, target family, and 512 KB NVM before every SWD operation. Stop on any mismatch; never switch COM ports opportunistically.
- Keep `PA8`, `PB5`, and legacy `PA5` low. Never drive `PB11` (tied to `PWR_NEG` on Rev 0).
- Do not connect a real panel, battery, inverter, AC wiring, or an uncontrolled load.
- Do not reuse the FP2 5.0 V/20 mA point as if it already passed — it did not; it aborted.
- Do not command PWM, enable gate drive, or probe a high-side gate with a ground-referenced oscilloscope lead.
- Stop for current limiting, unexpected current rise, rail collapse, heating, odor, unstable reset, unexpected gate voltage, or any active control output.

## Next Session Objective

1. Physically confirm USB and ST-LINK are disconnected, then measure `VCC2` (U6 pin 29) and `LO1`/`LO2`-to-`PWR_NEG` at rest. If they read near 0 V with everything disconnected, reconnect USB only (no ST-LINK) and re-measure to isolate whether USB/`VBUS` alone reproduces the live rail via the documented D8-to-`VCC2` residual path.
2. If the backfeed is confirmed, decide on a remedy (e.g., true isolation of the residual D8-to-`VCC2` path) before any FP2 re-attempt. If `VCC2` fully collapses with everything removed and does not reappear with USB-only power, FP2 may be re-attempted with this evidence recorded.
3. Only after FP2 passes cleanly: resolve the still-open F1/U2/U5 bypass waiver decision (Section 3 of the plan) before FP4 can be authorized.

## Relevant Files

- `docs/test/rev0-first-power-plan-2026-09-04.md`: the staged FP0-FP4 plan and all bench results to date (FP0/FP1 passed, FP2 aborted/unresolved).
- `docs/test/rev0-low-energy-session-2026-08-30.md`: source of the documented D8-to-`VCC2` residual-path measurement.
- `docs/lm51772-datasheet.pdf`: local copy; use `pypdf` for text extraction (TI's web viewer is unreadable via available tools — do not retry fetching it).
- `SOLAR_HANDOFF_2026-09-04.md`: prior handoff, useful for M1-M4 installation history.
- `NEXT_CHAT_PROMPT.md`: compact prompt for the new session.

## Worktree State at Handoff

At handoff, this session's plan-document updates are committed. `SOLAR_HANDOFF_2026-07-20.md` and `hardware/kicad/solar-project/Solar Project Rev1.kicad_pcb` remain older staged changes that predate this work; do not revert or combine them casually with the Rev 0 first-power session work.
