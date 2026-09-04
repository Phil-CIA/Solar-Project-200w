# Solar Project 200W - Rev 0 Populated Bridge Handoff (2026-09-04)

## Stop State

The board is unpowered and discharged. M1-M4 are installed but have never been powered. Do not connect USB, ST-LINK, PV, a battery, an output source or load, a panel, an inverter, or AC until a separate first-power plan is written and reviewed.

The installation-only gate is complete. This is not approval for static power, gate drive, converter switching, battery charging, inverter operation, or AC operation.

## Current Board Configuration

- U6 is an LM51772-Q1 in the RHA-40 package.
- R28 is restored to 51.7 kOhm from U6 `RT` to `PGND`.
- R29 is verified at 20.5 kOhm from U6 `CFG2` to `PGND`.
- R33 is zero ohms from U6 `ADDR_SLOPE_CFG1` to `PGND`, selecting I2C address `0x6A`.
- The Rev 0 `PGND`-to-`PWR_NEG` jumper remains installed from R31 pad 2 to R32 pad 2.
- F1, U2, and U5 protection functions remain bypassed.
- U2/U5 bypasses and the PGND jumper were not disturbed during MOSFET installation.
- M1-M4 are Infineon `BSC030N04NSGATMA1` N-channel MOSFETs in `PG-TDSON-8-1` / SuperSO8 5x6.
- Rev 0 has no series gate resistors.

Bridge mapping:

| Position | Drain | Source | Gate |
|---|---|---|---|
| M1 | `PV_IN_POS` | `SW_1` | `HO1` |
| M2 | `SW_1` | `PWR_NEG` | `LO1` |
| M3 | `SW_2` | `PWR_NEG` | `LO2` |
| M4 | output rail | `SW_2` | `HO2` |

## M1-M4 Installation Result

M2, M3, M1, and M4 were installed sequentially with all sources removed and U6 rails below 0.1 V. The following installation checks passed:

- Package orientation and visible solder joints under magnification.
- Gate-to-source and gate-to-drain short checks at all four devices.
- Expected body-diode direction and reverse blocking after gate discharge.
- No near-short from `PV_IN_POS` to `PWR_NEG`.
- No near-short from the output rail to `PWR_NEG`.
- No short between `SW_1` and `SW_2`.
- U2/U5 bypasses and the PGND-to-PWR_NEG jumper remained unchanged.
- Photographs cover both board sides and all four M1-M4 joints.

The installed bridge has not received any powered validation.

## U6 Communication and Fault Findings

The prior LM51772 NACK diagnosis was caused by a resistor-reference mistake. R28, not R33, had initially been changed to zero ohms. The still-fitted 6.49 kOhm R33 selected the datasheet configuration with I2C disabled. After restoring R28 to 51.7 kOhm and changing R33 to zero ohms, U6 acknowledged at `0x6A`.

Before M1-M4 installation:

- Two cold starts at PV=6.49 V, limited to 20 mA, returned `STATUS_BYTE=0x58` from register `0x78`: `OFF=1 IOUT=1 INPUT=1`.
- The R26/R27/C20 current-sense network passed unpowered checks.
- Powered measurements were `CSA=3.2041 V`, `CSB=3.2043 V`, and approximately 0.1 mV directly across C20.
- A separate cold start at PV=5.0 V drew less than 1 mA and held `EN_UVLO=1.0241 V`, below the rising threshold.
- A read-only SWD-driven software-I2C access then returned `STATUS_BYTE=0x48`: `OFF=1 IOUT=0 INPUT=1`.

This evidence associates the old `IOUT` latch with crossing UVLO while the bridge was absent. It does not prove behavior with the newly populated bridge. The approximately 3-4 mA startup fluctuation and later no-more-than-1.3 mA reading remain unexplained, but they are not evidence of an I2C communication failure or a sustained current-sense differential.

No LM51772 register was written. Never access `CLEAR_FAULTS` at `0x03` without a new, explicit authorization and reviewed reason.

## Control and Debug State

- Active MCU family evidence: Device ID `0x469`, STM32G47x/G48x/G414-class Cortex-M4, 512 KB NVM.
- Exact approved ST-LINK serial: `B55B5A1A0000000020AFF501`.
- Exact onboard CP210x identity: `USB\VID_10C4&PID_EA60\0001` on `COM5` only, 115200 8N1.
- Software I2C uses STM32 PB6=SDA and PB7=SCL.
- Last UART safety status: `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`.
- COM5 is healthy. Its apparent failure was stale partial parser input; one unknown command cleared the buffer and a fresh `status` command passed.
- `firmware/scripts/rev0-openocd-u6-status.ps1` provides a read-only fallback for U6 register `0x78`. It binds the exact ST-LINK, saves and restores GPIOB state, and resumes the MCU.

Before every future flash or SWD operation, verify the exact ST-LINK serial, Device ID `0x469`, target family, 512 KB NVM, and plausible target voltage. Stop on any mismatch. Do not switch COM ports opportunistically.

## Hard Safety Limits

- Keep the board unpowered until the first-power plan is reviewed.
- Keep `PA8`, `PB5`, and legacy `PA5` low.
- Never drive `PB11`; it is tied to `PWR_NEG` on Rev 0.
- Do not connect a real panel, battery, inverter, AC wiring, or an uncontrolled load.
- Do not reuse the old 6.49 V / 20 mA authorization automatically; that test applied before M1-M4 were installed.
- Do not command PWM, enable gate drive, or probe a high-side gate with a ground-referenced oscilloscope lead.
- Treat F1/U2/U5 bypasses as unresolved first-power hazards, not normal operating configuration.
- Stop for current limiting, unexpected current rise, rail collapse, heating, odor, unstable reset, unexpected gate voltage, or any active control output.

## Next Session Objective

Write and review a separate staged first-power plan. Do not execute it in the same step in which it is drafted. The plan must resolve or explicitly bound:

1. F1, U2, and U5 bypass disposition before energizing the bridge.
2. Whether the first stage is control-power-only, below-UVLO PV, or another non-switching condition.
3. Current-limited source connection, initial voltage, current limit, and stop thresholds.
4. Output isolation and the required sink or preload; no uncontrolled backfeed path.
5. The exact firmware image and proof that `PA8`, `PB5`, and `PA5` remain low at boot and reset.
6. Scope connection strategy, including differential or isolated measurement for high-side gate-source voltage.
7. Bootstrap capacitor values, connectivity, and expected charge behavior.
8. Dead time, startup duty cycle, pulse count, and maximum run duration before any switching authorization.
9. Consequences of the missing series gate resistors: edge rate, ringing, overshoot, EMI, and shoot-through margin.
10. MOSFET, inductor, controller, and current-sense thermal observations.
11. Required pre-power resistance rechecks and evidence capture.
12. Immediate abort and discharge procedure.

The lowest-risk structure is expected to separate populated-board static power from any later single-pulse or repetitive-switching stage. Each stage needs its own entry gate and recorded pass result.

## Relevant Files

- `docs/test/rev0-m1-m4-installation-plan-2026-09-04.md`: completed installation and passive-verification record.
- `docs/test/rev0-low-energy-session-2026-08-30.md`: chronological bench evidence.
- `docs/test/rev0-hardware-bringup-execution-plan-2026-08-30.md`: H0-H8 low-energy plan; closed within its constrained scope.
- `docs/test/rev0-bringup-addendum.md`: Rev 0 limitations and overrides.
- `SOLAR_HANDOFF_2026-09-02.md`: detailed U6 diagnosis history; its early “M1-M4 absent” statements are historical.
- `firmware/scripts/rev0-openocd-u6-status.ps1`: verified read-only U6 status fallback.
- `firmware/bringup/baremetal/stm32g4_rev0_stage1.c`: active diagnostic firmware source.
- `NEXT_CHAT_PROMPT.md`: compact prompt for the new session.

## Worktree State at Handoff

At handoff, current-session documentation and the new SWD script are not all committed. Preserve them. `SOLAR_HANDOFF_2026-07-20.md` and `hardware/kicad/solar-project/Solar Project Rev1.kicad_pcb` contain older staged changes that predate this work; do not revert or combine them casually with the Rev 0 handoff changes.

Latest committed revision observed at handoff: `50b29b7 docs: record PV current-collapse observation on replacement U6`.