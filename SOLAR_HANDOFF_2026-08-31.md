# Solar Project 200W - Rev 0 Bring-Up Handoff (2026-08-31)

## Current State

The recovered Rev 0 board is running the overnight low-energy stability image from USB power only. The OLED shows software time, reset-relative uptime, AHT20 temperature/humidity, successful sample count, and cumulative I2C/display errors. D8 runs the accepted dim red/green/blue MCU heartbeat.

Verified target:

- ST-LINK serial: `B55B5A1A0000000020AFF501`
- Device ID: `0x469`
- Device: STM32G47x/G48x/G414, 512 KB
- Target voltage: approximately 3.23-3.24 V
- USB-UART: onboard CP2102 on `COM5`, 115200 8N1

## Completed Gates

- USB-C/CP2102 hardware repaired; `COM5` enumeration and bidirectional UART pass.
- Reset vector corrected to a Thumb entry point.
- `nSWBOOT0=0`, `nBOOT0=1`; cold boot passed 5/5 and S1 reset passed 5/5.
- Safe outputs verified low: `PA8`, `PB5`, and `PA5`.
- D8 moved from prohibited LM51772 VCC2 loading to USB VBUS.
- H4 D8 RGB heartbeat: pass after fixed-cycle 800 kHz sender correction.
- H5 software-I2C scan: pass; only AHT20 `0x38` and SSD1306 `0x3C` found.
- H6 AHT20: pass; calibrated, plausible readings.
- H7 SSD1306: pass; checkerboard visually confirmed and shared bus remained healthy.
- Overnight dashboard firmware built cleanly, flashed after exact target verification, and byte-verified.

## Overnight Run

Start state on 2026-08-31:

- Software clock set from PC to `07:44:44`; it is HSI-derived and not RTC-grade.
- AHT20 cadence: 10 seconds.
- OLED refresh cadence: 1 second.
- Initial reading: status `0x18`, 27.1 C, 39.5% RH.
- UART remained responsive during automatic OLED refresh.
- Safety status: `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`.

At the next session, record before resetting or reflashing:

1. Displayed `UP` uptime.
2. `N` successful sample count.
3. `E` error count.
4. Current temperature and humidity.
5. Whether OLED content and RGB heartbeat remain normal.
6. Whether `COM5` still answers `status` and `aht20`.

A reset is visible as restarted uptime and counters. The software wall clock also loses its setting after reset or power loss.

## Next Hardware Step

After recording the overnight result, resume H3 at one bounded LM51772 static point only:

- Source setting: 6.5 V.
- Current limit: 20 mA.
- M1-M4 remain unpopulated.
- No switching, PWM, battery, output load, inverter, or AC.
- Record source current, U6 temperature/abnormal behavior, `PV_IN_POS`, `VCC1`, `VCC2`, `EN_UVLO`, `RST`, and `FLT`.
- Stop immediately for current limiting, unexpected heating, odor, unstable rails, or any unsafe control-output state.

The prior 6.0 V / 20 mA point showed no current limit or heating: `PV_IN_POS=5.993 V`, `VCC1=3.998 V`, `VCC2=3.5921 V`, `EN_UVLO=0.804 V`, `RST=3.299 V`, and `FLT=0.023 V`. The source was turned off afterward.

## Non-Negotiable Safety

- Keep PV, battery, charger output, inverter, AC wiring, and loads disconnected unless a later test procedure explicitly authorizes one bounded source.
- Never configure `PB11` as an output; it is tied to `PWR_NEG`.
- Keep `PA8`, `PB5`, and `PA5` low during static tests.
- Before every flash, verify ST-LINK `B55B5A1A0000000020AFF501`, Device ID `0x469`, STM32G47x/G48x/G414, 512 KB NVM, and expected target voltage. Stop on any mismatch.
- Do not switch COM ports opportunistically; the verified onboard bridge is `COM5`.
- Use software I2C with PB6 as SDA and PB7 as SCL; Rev 0 routing is swapped relative to hardware I2C1 alternate functions.
- D8 may use USB VBUS only; never reconnect it to LM51772 VCC2.

## Key Files

- `docs/test/rev0-hardware-bringup-execution-plan-2026-08-30.md`: gate sequence and acceptance criteria.
- `docs/test/rev0-low-energy-session-2026-08-30.md`: detailed measurements and evidence.
- `docs/test/rev0-low-energy-comm-control-test.md`: low-energy test procedure.
- `firmware/bringup/baremetal/stm32g4_rev0_stage1.c`: active flashed firmware source.
- `firmware/bringup/baremetal/build.ps1`: reproducible bare-metal build.
- `firmware/scripts/`: target identification and read-only/volatile SWD helpers.

## Working Tree Note

The staged edits to `SOLAR_HANDOFF_2026-07-20.md` and `hardware/kicad/solar-project/Solar Project Rev1.kicad_pcb` predate this bring-up work. Preserve them and keep them out of the Rev 0 bring-up commit.
