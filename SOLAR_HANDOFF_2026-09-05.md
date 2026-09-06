# Solar Project 200W - Rev 0 First-Power Session Handoff (2026-09-05)

## Stop State

The board is paused for SWD communication troubleshooting. Keep PV off and disconnected while resolving this; do not leave the board energized unattended. Before later PV work, verify `PV_IN_POS_RAW`, `PV_IN_POS`, `CHG_OUT_POS`, `SW_1`, `SW_2`, `VCC1`, and `VCC2` are below 0.1 V relative to `PWR_NEG`.

FP0 and FP1 passed on 2026-09-04. FP2's former gate-voltage stop criterion is now treated as a criterion mismatch: the observation that `LO1`/`LO2` tracked the LM51772 internal `VCC2` rail does not, by itself, establish external backfeed or unsafe switching. FP2 is not formally reclassified in the original plan, but it is no longer the active blocker for the controlled FP4 characterization.

## Current Blocking Finding - FP4 Startup Retry / Current Limit

FP4 was attempted today with USB/JTAG and the controlled PV source present. At about 6.0 V input, U6 began switching; its approximate 610 kHz switching cycles appeared in packets separated by about 200 ms. A two-probe DS1054 math measurement (`CH1=HO1`, `CH2=SW_1`, both grounds at `PWR_NEG`, `Math=CH1-CH2`) showed a centered approximately 5-10 Vpp `HO1-SW_1` waveform during packets. This is a reasonable low-voltage characterization substitute, but not a calibrated differential-probe overshoot measurement.

With the original 200 ohm preload connected, `CHG_OUT_POS` was approximately 0.986 V and the PV source limited at about 229 mA when raised above 6.0 V. Replacing the preload with 1 kohm did not establish regulation: `CHG_OUT_POS` rose only to 1.36 V at `PV_IN_POS_RAW=5.98 V` and approximately 266 mA input. The source current is substantially greater than the output power can explain; do not raise the current limit as a next step.

The most complete powered snapshot, taken during the retry behavior, was:

| Net / observation | Result |
|---|---|
| `PV_IN_POS_RAW` | 5.9767 V |
| `PV_IN_POS` | 5.961 V |
| `VCC1` | 5.0679 V |
| `VCC2` | 4.9250 V |
| `SW_1` (DMM DC) | 0.07 V |
| `LO1` (DMM DC) | 0.02 V |
| Switching cadence | approximately 610 kHz cycles in approximately 200 ms-separated packets |

The roughly 16 mV difference between `PV_IN_POS_RAW` and `PV_IN_POS` rules out the previously observed input-front-end voltage drop as the immediate cause at this point. The result is consistent with a start/stop/retry condition, but its cause is not yet determined. FP4 has **not passed** because the converter did not regulate toward the confirmed 12.83 V target and the supply current-limited.

The requested next discriminator, not completed before leaving the bench, is a read-only capture of U6 `STATUS_BYTE` (`0x78`) and `FLT` voltage while the retry behavior is active. `firmware/scripts/rev0-openocd-u6-status.ps1` reads only `0x78`, restores GPIOB, and resumes the MCU; it must be preceded by exact ST-LINK/target identity verification. Never write `CLEAR_FAULTS` (`0x03`).

## Current Blocking Finding - SWD Target Communication

On 2026-09-06, STM32CubeProgrammer `--list` found exactly one ST-LINK, serial `B55B5A1A0000000020AFF501` (firmware `V2J37S7`), and the onboard CP2102 on COM5. A subsequent **read-only**, serial-bound connection command,

```powershell
& 'C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe' -c port=SWD sn=B55B5A1A0000000020AFF501 mode=UR freq=4000
```

reported ST-LINK target voltage `3.23 V`, followed by `Unable to get core ID` and `No STM32 target found`. The expected Device ID `0x469`, STM32G47x/G48x/G414 family, and 512 KB NVM were consequently **not** confirmed. No flash, option byte, RAM, or LM51772 register write occurred. Do not select another ST-LINK, COM port, or target as a workaround.

The active bare-metal stage-1 firmware source has an unflashed OLED enhancement in `firmware/bringup/baremetal/stm32g4_rev0_stage1.c`. It was built successfully on 2026-09-06 (`text=7364`, `data=4`, `bss=1160`) and would display a cached U6 `STATUS_BYTE` as `U6 xx Nnnn`, polling U6 read-only every two seconds. Do not flash it until SWD identity has again passed exactly.

First SWD recovery checks, with PV disconnected, are physical and local: confirm `CTRL_3V3` at the MCU supply, common `PWR_NEG` continuity between board and ST-LINK, `NRST` released high, and continuity/no shorts on STM32 `PA13`/SWDIO and `PA14`/SWCLK. The observed 3.23 V is VTref evidence only; it does not prove that the MCU core is powered, out of reset, or responding. After these checks, repeat only the exact serial-bound read-only command above. Stop and record the result if the required identity still does not return.

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
| FP2 (below-UVLO PV static) | Attempted and aborted under a gate-voltage criterion now considered inapplicable to `LOx` tracking `VCC2`; no further external-backfeed conclusion was established |
| FP4 (first switching) | **Attempted, not passed** — switching/retry packets began near 6.0 V; source limited before output regulation |
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
- Do not increase the PV source current limit above the approximately 200 mA FP4 bound while the retry/current-limit cause remains unknown.
- Do not command PWM, enable gate drive, or probe a high-side gate with a ground-referenced oscilloscope lead.
- Stop for current limiting, unexpected current rise, rail collapse, heating, odor, unstable reset, unexpected gate voltage, or any active control output.

## Next Session Objective

1. Keep PV off/disconnected. Restore USB control power and the approved ST-LINK connection only as needed for the SWD recovery checks. Confirm the board's `CTRL_3V3`, common `PWR_NEG`, released `NRST`, `PA13`/SWDIO, and `PA14`/SWCLK physical conditions.
2. Run the exact serial-bound, read-only STM32CubeProgrammer connection documented above. Proceed only if it reports ST-LINK `B55B5A1A0000000020AFF501`, Device ID `0x469`, STM32G47x/G48x/G414 family, 512 KB NVM, and plausible target voltage. Stop on any mismatch or absent core ID.
3. Once identity passes, flash and verify the already-built OLED-status image only after recording the successful identity output. Verify that the OLED shows `U6 xx Nnnn` and that safe outputs remain low. Do not combine firmware flashing with PV work.
4. After SWD/OLED verification is complete, resume the bounded approximately 5.98 V / 200 mA / 1 kohm-preload FP4 retry observation for no more than 30 seconds. Record `FLT`, `STATUS_BYTE`, source-current-limit state, input current, and `CHG_OUT_POS`; then turn source off and discharge. Do not run FP3 or increase the current limit.

## Relevant Files

- `docs/test/rev0-first-power-plan-2026-09-04.md`: staged FP0-FP4 plan; its FP2 and FP4 gating text predates today's observations and needs review before another energized run.
- `docs/test/rev0-low-energy-session-2026-08-30.md`: source of the documented D8-to-`VCC2` residual-path measurement.
- `docs/lm51772-datasheet.pdf`: local copy; use `pypdf` for text extraction (TI's web viewer is unreadable via available tools — do not retry fetching it).
- `SOLAR_HANDOFF_2026-09-04.md`: prior handoff, useful for M1-M4 installation history.
- `NEXT_CHAT_PROMPT.md`: compact prompt for the new session.
- `firmware/bringup/baremetal/stm32g4_rev0_stage1.c`: built but unflashed OLED U6-status display update.

## Worktree State at Handoff

At handoff, this session's plan-document updates are committed. `SOLAR_HANDOFF_2026-07-20.md` and `hardware/kicad/solar-project/Solar Project Rev1.kicad_pcb` remain older staged changes that predate this work; do not revert or combine them casually with the Rev 0 first-power session work.
