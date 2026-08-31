# Firmware

Store embedded source, build configuration, and flashing instructions here.

Suggested structure:
- `src/`
- `include/`
- `lib/`
- `scripts/`
- `docs/`

## Rev 0 Safe Bring-Up

- Source: [bringup/stm32g4_rev0_safe_bringup.c](bringup/stm32g4_rev0_safe_bringup.c)
- Bare-metal stage-1 source: [bringup/baremetal/stm32g4_rev0_stage1.c](bringup/baremetal/stm32g4_rev0_stage1.c)
- Bench procedure: [../docs/test/rev0-low-energy-comm-control-test.md](../docs/test/rev0-low-energy-comm-control-test.md)
- Target: recovered Rev 0 board fitted with an STM32G431/STM32G473-class UFQFPN-48 MCU.
- Framework: the active Rev 0 image is freestanding bare-metal firmware for the verified STM32G47x-class target. The separate HAL-oriented source remains a future integration reference.

The bare-metal stage-1 image avoids STM32Cube/HAL dependencies and can be built once GNU Arm Embedded Toolchain is installed:

```powershell
./firmware/bringup/baremetal/build.ps1
```

Stage 1 provides UART diagnostics, safe output state, fault input reads, open-drain software I2C, AHT20 measurements, SSD1306 output, and the D8 RGB heartbeat. Its automatic dashboard shows software wall time, reset-relative uptime, sensor data, successful samples, and I2C/display errors. ADC remains a placeholder for a later calibrated acquisition pass.

## Rev 0 SWD-Only Checks

The scripts in [scripts/](scripts/) use xPack OpenOCD and ST-LINK to run volatile register checks without flashing firmware:

```powershell
./firmware/scripts/rev0-openocd-identify.ps1
./firmware/scripts/rev0-openocd-safe-idle.ps1
./firmware/scripts/rev0-openocd-i2c-gpio-check.ps1
./firmware/scripts/rev0-openocd-control-gpio-check.ps1
./firmware/scripts/rev0-openocd-run-probe.ps1
./firmware/scripts/windows-list-serial-usb.ps1
```

Run these only with PV, battery, charger output, inverter/AC wiring, and loads disconnected. Run them one at a time because they share the same ST-LINK/SWD target. The control GPIO check briefly toggles `PB5` and `PA5`; it does not pulse `PA8`.

After flashing the bare-metal stage-1 image, `rev0-openocd-run-probe.ps1` starts user flash from the programmed vector table, then reads the SRAM probe block at `0x20000000`. A first word of `0x53523030` and an increasing heartbeat word prove the firmware is executing even if the USB-UART bridge is not enumerating.

Rev 0 normal boot is repaired and verified: the startup vector uses a Thumb reset entry, and option bytes use `nSWBOOT0=0`, `nBOOT0=1` so floating PB8 is not sampled. Cold boot and S1 reset each passed 5/5. Rev 1 still requires a populated 10 kOhm BOOT0 pulldown and accessible test point. The repaired onboard CP2102 is verified on `COM5` at 115200 8N1.

Before any flash/upload, verify the programmer is attached to the expected recovered Rev 0 target and that the detected chip is the expected STM32G4 device. If the target does not match, stop instead of trying a different COM port or probe.
