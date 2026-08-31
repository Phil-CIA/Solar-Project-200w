# Rev 0 Low-Energy Communication and Control Test

Date: 2026-08-30
Status: bench-ready draft for recovered Rev 0 boards
Scope: STM32 alive check, serial logging, safe GPIO control, ADC readout, fault inputs, and software I2C discovery only.

Use this with [rev0-bringup-addendum.md](rev0-bringup-addendum.md). This procedure does not authorize PV, battery, charger switching, inverter switching, or AC testing.

## 1. Hard Limits

- Power only through the intended `VBUS` / USB-control-power entry using an isolated current-limited bench supply.
- Keep PV, battery, charger output, inverter wiring, and AC wiring disconnected.
- `PB11` is tied to `PWR_NEG` on Rev 0. Do not configure or probe it as a driven output.
- `CTRL_PWM_MAIN` on `PA8`, `CTRL_EN_CHG` on `PB5`, and legacy `V_in_ON` on `PA5` must boot low and stay low unless a named low-energy test step explicitly pulses them.
- Hardware `I2C1` is not valid on Rev 0 because the routed clock/data labels are swapped for the STM32 alternate functions. Use software I2C only: `PB6` as routed `I2C DATA`, `PB7` as routed `I2C CLK`.
- Stop for current limit, rail collapse, unexpected heating, no reset control, active PWM at boot, or any fault input state that cannot be explained.

## 2. Required Setup

| Item | Setting / evidence |
|---|---|
| Bench supply | 5.0 V target, current limit recorded before connection |
| Serial | USART1, 115200 8N1, `PA9` TX, `PA10` RX, common `PWR_NEG` reference |
| SWD | Verified target must be `STM32G431`/`STM32G473` class on the expected probe and board only |
| Scope or logic probe | Confirm `PA8`, `PB5`, and `PA5` idle low after reset |
| Firmware | Start with the bare-metal stage-1 image at [stm32g4_rev0_stage1.c](../../firmware/bringup/baremetal/stm32g4_rev0_stage1.c). Use [stm32g4_rev0_safe_bringup.c](../../firmware/bringup/stm32g4_rev0_safe_bringup.c) later after a Cube/HAL project exists. |

Before flashing, verify the detected chip and probe target. If the detected device, probe, or board does not match the recovered Rev 0 target, stop.

Read-only target identification with STM32CubeProgrammer:

```powershell
& 'C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe' --list
& 'C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe' -c port=SWD mode=UR freq=4000
```

If this reports `DEV_CONNECT_ERR`, do not flash. Check target-side `CTRL_3V3`, ST-LINK `VTref`, common `PWR_NEG`, `PA13`/SWDIO continuity, `PA14`/SWCLK continuity, `NRST` state, and soldering around the STM32 before retrying target identification.

If no firmware build toolchain is available yet, OpenOCD can be used for volatile SWD register checks without flashing. Use this only after confirming PV, battery, charger output, inverter/AC wiring, and loads are disconnected. Record the full command output in the bench log.

Observed-good scripted OpenOCD checks:

```powershell
./firmware/scripts/rev0-openocd-identify.ps1
./firmware/scripts/rev0-openocd-safe-idle.ps1
./firmware/scripts/rev0-openocd-i2c-gpio-check.ps1
./firmware/scripts/rev0-openocd-control-gpio-check.ps1
```

Run only one OpenOCD script at a time. Each script opens the same ST-LINK/SWD target and concurrent access can invalidate the test.

The 2026-08-30 SWD-only register test results are recorded in [rev0-low-energy-session-2026-08-30.md](rev0-low-energy-session-2026-08-30.md).

Current Rev 0 caveats from live testing:

- Normal reset/run may enter system memory if `PB8`/BOOT0 is sampled high. Check physical `PB8`/BOOT0 before changing option bytes.
- USB can power the board while no USB serial device enumerates. The exported netlist shows USB connector D+/D- and CP2102 D+/D- on separate nets through `D3`/`USBLC6-2SC6`; verify continuity before chasing Windows drivers.

## 3. Firmware Behavior Under Test

Expected boot behavior:

- `PA8` / `CTRL_PWM_MAIN`: push-pull output low.
- `PB5` / `CTRL_EN_CHG`: push-pull output low.
- `PA5` / `V_in_ON`: push-pull output low.
- `PB0` / `FAULT_OCP` and `PB1` / `FAULT_OVP`: inputs.
- `PB6` / `I2C DATA` and `PB7` / `I2C CLK`: software open-drain I2C lines released high when idle.
- USART1 prints a boot banner and accepts text commands.

## 4. Test Order

### R0-L1 - Power and Safe Idle

1. Confirm unpowered resistance from `VBUS` and `CTRL_3V3` to `PWR_NEG` is not a near-short.
2. Apply current-limited 5 V at `VBUS`.
3. Record supply current, `VBUS`, and `CTRL_3V3`.
4. Reset the STM32 and confirm `PA8`, `PB5`, and `PA5` remain low.

Pass: stable rails, no heating, no current-limit event, all control outputs low.

### R0-L2 - SWD and Serial Log

1. Attach with SWD after verifying the expected STM32G4 target.
2. Flash the safe bring-up image.
3. Open serial at 115200 8N1.
4. Press reset and record the boot banner.
5. Send `?` and record the status response.

Pass: reproducible SWD attach, reset, banner, and command response.

### R0-L3 - Fault Inputs and ADC Readout

1. Send `fault` and record `PB0` / `PB1` states.
2. For the bare-metal stage-1 image, send `adc` and confirm it reports that ADC is intentionally not enabled yet.
3. After moving to the HAL/Cube image, send `adc` and record raw `PA4`, `PA6`, and `PA7` readings.
4. If using any stimulus, keep it low-energy and bounded to the ADC input range through documented resistance.

Pass: fault states are stable and ADC readings are plausible for the board state.

### R0-L4 - Software I2C Discovery

1. Confirm `PB6` and `PB7` idle high at the display or controller connector.
2. Send `i2cscan`.
3. Record detected 7-bit addresses.
4. If no devices respond, scope `PB6` and `PB7` and verify software clock/data movement before suspecting the peripheral.

Pass: software I2C toggles the routed nets and any populated I2C device acknowledges at a plausible address.

### R0-L5 - Low-Energy Control Pulses

Only perform these tests with PV, battery, charger output, inverter, and AC wiring disconnected.

1. Send `pulse en` and verify `PB5` pulses high briefly then returns low.
2. Send `pulse legacy` only if the U2/Q2 legacy path is safe to observe with no energy source connected.
3. Do not pulse `pwm` unless a scope is attached to `PA8` and the charger power path is de-energized.

Pass: each commanded line changes only when requested and returns low immediately.

## 5. Command Summary

| Command | Expected response / action |
|---|---|
| `?` | Print help and current safe-state summary |
| `status` | Print fault inputs and control output latch states |
| `fault` | Print `FAULT_OCP` and `FAULT_OVP` input states |
| `adc` | Print raw ADC samples for `SENSE_PV_V`, `SENSE_TEMP_PWR`, and `SENSE_TEMP_BOARD` |
| `i2cscan` | Scan software I2C addresses using `PB6` data and `PB7` clock |
| `pulse en` | Pulse `CTRL_EN_CHG` / `PB5` for 100 ms, then force low |
| `pulse legacy` | Pulse `V_in_ON` / `PA5` for 100 ms, then force low |
| `pulse pwm` | Pulse `CTRL_PWM_MAIN` / `PA8` for 10 ms, then force low; scope-only, no power path connected |
| `safe` | Force all control outputs low |

## 6. Bench Record

| Check | Result / observation |
|---|---|
| Board ID and fitted MCU marking |  |
| Bench supply model and current limit |  |
| `VBUS`, `CTRL_3V3`, supply current |  |
| SWD probe and detected target |  |
| Firmware build/source revision |  |
| Boot banner captured |  |
| `PA8`, `PB5`, `PA5` idle-low verified |  |
| `fault` output |  |
| `adc` output |  |
| `i2cscan` output |  |
| Low-energy pulse result, if attempted |  |
