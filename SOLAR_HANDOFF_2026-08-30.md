# Solar Project 200W - Rev 0 Bring-Up Handoff (2026-08-30)

## Current State

The recovered Rev 0 board fitted with an STM32G47x-class MCU is alive, programmable, and executing test firmware.

Verified target:

- ST-LINK serial: `B55B5A1A0000000020AFF501`
- Target voltage: approximately 3.23 V to 3.24 V
- Device ID: `0x469`
- Device: `STM32G47x/G48x/G414`, 512 KB, Cortex-M4

## Completed

- SWD attach and register access pass.
- `PA5`, `PA8`, and `PB5` configured low safely.
- `PB5` and `PA5` high/low GPIO readback pass.
- `PB6`/`PB7` open-drain software-I2C GPIO readback pass.
- `PA8` / `CTRL_PWM_MAIN` was never pulsed high.
- Portable xPack ARM GCC installed locally under ignored `.tools/`.
- Bare-metal stage-1 firmware builds successfully.
- Stage-1 firmware flashed to `0x08000000` and verified.
- Firmware execution proved by SWD SRAM probe:

```text
0x20000000: 53523030 00000001 <nonzero-heartbeat> ...
```

## Two Open Hardware Findings

### 1. Normal Reset - Root Cause Corrected 2026-08-31

Normal reset entered STM32 system memory (`PC=0x1fff3b4a`) instead of user flash. Manually loading the programmed MSP/PC over SWD runs the firmware correctly.

Relevant option-byte state:

```text
nBOOT1   = 1
nSWBOOT0 = 1   # BOOT0 taken from PB8 pin
nBOOT0   = 1
```

Rev 0 `PB8`/BOOT0 is floating. The initial physical diagnosis was completed before changing option bytes.

**Next physical test:** measure `PB8`/BOOT0 voltage during reset. If it is not reliably low, add a temporary pulldown and confirm normal reset boots user flash.

2026-08-31 result: `PB8`/BOOT0 measured approximately 0.30 V while reset was pressed and changed by only a few millivolts. Five read-only reset checks loaded the user-flash vector (`PC=0x08000040`, `MSP=0x20018000`), proving BOOT0 was sampled low and flash was selected.

The actual normal-run failure was in `firmware/bringup/baremetal/startup_stm32g4xx.s`: the handlers lacked Thumb-function annotations, so the generated reset vector was even (`0x08000040`). Hardware reset therefore raised `INVSTATE` (`CFSR=0x00020000`) and entered the default handler at `0x08000084`. Adding `.type ... %function` and `.thumb_func` generated the required reset vector `0x08000041`.

The corrected 1.93 KB image was flashed only after verifying ST-LINK `B55B5A1A0000000020AFF501`, target voltage 3.23 V, Device ID `0x469`, and STM32G47x/G48x/G414. Download verification passed. A normal reset without manual MSP/PC loading then produced:

```text
PC:        0x08000300
MSP:       0x20017fe8
SRAM:      53523030 00000001 <increasing-heartbeat> ...
CFSR:      00000000
HFSR:      00000000
```

Debug-driven reset boot passes 5/5, but the physical power-cycle gate failed on 2026-08-31. After five USB disconnect/reconnect cycles, the verified CP210x returned on `COM5` but UART was silent. A non-resetting SWD snapshot showed the MCU in system ROM at `PC=0x1FFF3B08` with no SRAM heartbeat. Pressing and releasing S1 did not restore UART; the subsequent snapshot remained in system ROM at `PC=0x1FFF3B24`.

Do not mark H2 complete. Next measure the STM32 `NRST` node directly: record its voltage with S1 released and while S1 is held. Expected behavior is approximately `CTRL_3V3` released and near 0 V pressed. With power removed, verify continuity from S1 to the MCU NRST pad. The earlier 0.30 V BOOT0 measurement during button press does not prove BOOT0 state at the initial power-on sample.

2026-08-31 NRST result: pass. The reset node identified as STM32 pin 7 / `PG10` measured approximately 3.2 V released and 0.2 V while S1 was pressed. The button and reset-node path are functional. Continue with BOOT0-at-release/power-on diagnosis; do not replace or bypass S1 based on current evidence.

2026-08-31 BOOT0 correction: after exact probe and target reverification, only `nSWBOOT0` was changed from `1` to `0`; the existing `nBOOT0=1` value now selects user flash without sampling floating PB8. Independent readback passed. A hardware reset ran user flash at `PC=0x080002E0` with valid SRAM magic and heartbeat. Perform a true cold-power cycle before closing H2 or advancing to H3.

The first post-correction cold-power cycle passed without debugger assistance. After board USB power was removed and restored, a COM5-only query returned the valid stage-1 status and confirmed `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. H2 cold-power progress is 1/5; complete four more cycles before advancing to H3.

H2 is now closed. Four additional board-USB power cycles passed for 5/5 total, followed by 5/5 physical S1 reset cycles. The final COM5-only query returned `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`, proving application startup without SWD assistance and confirming all controlled power outputs remained inactive. Proceed to the H3 U6 static-power gate only; converter switching remains prohibited.

H3 reached one bounded static point at 6.0 V / 20 mA: no current-limit event or heating; `PV_IN_POS=5.993 V`, `VCC1=3.998 V`, `VCC2=3.5921 V`, `EN_UVLO=0.804 V`, `RST=3.299 V`, and `FLT=0.023 V`. The source is off. Do not apply a higher U6 voltage yet.

Resolved routing blocker: TI LM51772-Q1 Rev. A Section 7.3.7.2 prohibits any external load on VCC2, so D8 VDD was isolated from `U6_VCC2` and moved to USB `VBUS`. Rev 1 must formalize the D8 supply and provide an isolation link or resistor footprint.

D8 rework update: D8 VDD has now been moved to USB `VBUS`, pending power-off electrical acceptance. Do not reconnect USB or PV until D8 pad 2 is proven open to U6 pin 29, continuous to `VBUS`, and not shorted to ground or adjacent pads. C29/C45 must remain connected to U6 VCC2; D8 needs a new local 100 nF capacitor across pads 2-3. When testing D8, USB is the sole VBUS source; do not attach an external 5 V supply at the same time.

Measured after rework: D8 pad 2 to U6 pin 29 = 300 kOhm; D8 pad 2 to `VBUS` = 0.4 Ohm; no pad-1/pad-4 shorts reported; U6 pin 29 still continuous to C29/C45. The unmeasured pad-2-to-ground resistance and omitted local 100 nF capacitor are accepted as noncritical Rev 0 risks and remain Rev 1 corrections. USB operation showed no observed abnormal behavior.

D8 is now explicitly non-blocking. An automatic dim red/green/blue heartbeat image (each pulse about 250 ms on / 750 ms off) was built and verified, then flashed only after confirming ST-LINK `B55B5A1A0000000020AFF501`, target 3.24 V, device `0x469`, STM32G47x/G48x/G414, and 512 KB NVM. Post-reset COM5 status passed with PA8, PB5, and PA5 low. Do not spend more bring-up time diagnosing D8 appearance; continue with U6 static validation and shared I2C hardware.

D8 H4 closure: Pass. The compiler-dependent sender was measured from disassembly at roughly 667 kHz and replaced with a balanced fixed-cycle Thumb-2 800 kHz sender for the 16 MHz clock. The reflashed red/green/blue heartbeat is visibly correct, and its brightness and 250 ms on / 750 ms off cadence were accepted. Preserve these settings as the Rev 0 baseline.

H5 I2C address scan: Pass. With PV off and USB control power only, the Rev 0 open-drain software-I2C driver on PB6=SDA and PB7=SCL found exactly `0x38` and `0x3C`. R44/R45 are 4.7 kOhm pull-ups to `CTRL_3V3`. Continue with an AHT20 functional sample, followed by SSD1306 initialization.

H6 AHT20 initial functionality: Pass. The new `aht20` command returned calibrated/not-busy status `0x18`, temperature 26.7 C, and humidity 39.9% RH from address `0x38`. PA8, PB5, and PA5 remained low. Continue with SSD1306 initialization at the scanned address `0x3C`.

H7 SSD1306 initial functionality: Pass. The new `oled test` command initialized address `0x3C` at low contrast and transferred a full-screen checkerboard without NACK. COM5 returned `SSD1306 checkerboard drawn`, and the checkerboard was visually confirmed on the panel. A follow-up AHT20 read still passed at 26.7 C / 39.5% RH, confirming shared-bus health.

H8 overnight stability run is active on USB control power only. The OLED auto-starts with `TIME` (PC-set software wall clock), `UP` (reset-relative uptime), `T/H` (AHT20 sample every 10 seconds), and `N/E` (successful sample/error counters). The display refreshes every second and D8 retains the RGB heartbeat. Clock set at `07:44:44`; post-start checks passed with AHT20 status `0x18`, 27.1 C / 39.5% RH, responsive UART, and PA8/PB5/PA5 low. Keep PV, battery, charger output, inverter, AC, and loads disconnected overnight. In the morning, record displayed uptime, sample count, error count, current T/H, RGB heartbeat state, and any display corruption or reset evidence.

Rev 1 requirement: fit a 10 kOhm BOOT0 pulldown to `PWR_NEG` and provide an accessible BOOT0 test point. Do not rely solely on programmed option bytes for the normal boot state.

### 2. USB-C / CP2102 Enumeration - Closed 2026-08-31

The initial failure was traced to incorrect D3 wiring and solder bridges at the USB connector. After hardware correction, Windows identifies the onboard bridge as:

```text
Silicon Labs CP210x USB to UART Bridge (COM5)
USB\VID_10C4&PID_EA60\0001
Status: OK
```

Enumeration and UART traffic are proven. After the reset-vector correction, a 2026-08-31 check at 115200 8N1 returned the complete expected help response to `?`. `status` returned `OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`; control outputs are safely low, while the fault-input states remain to be explained before power-stage work. Do not use another COM port unless the connected hardware identity changes and is verified.

Original diagnosis retained for traceability:

The exported netlist shows USB connector D+/D- and CP2102 D+/D- as separate nets through `D3` (`USBLC6-2SC6`):

- Connector D-: J5 A7/B7 -> D3 pin 1
- Connector D+: J5 A6/B6 -> D3 pin 3
- CP2102 D+: U3 pin 4 -> D3 pin 4
- CP2102 D-: U3 pin 5 -> D3 pin 6

The bench findings confirmed that this path and the connector soldering required correction.

## Safety Limits

- Keep PV, battery, charger output, inverter, AC wiring, and loads disconnected.
- `PB11` is tied to `PWR_NEG`; never configure it as an output.
- Hardware I2C1 is unusable as routed. Use software I2C: `PB6` data, `PB7` clock.
- Keep `PA8` / `CTRL_PWM_MAIN` low.
- Before every flash, verify Device ID `0x469`, STM32G47x-class target, expected ST-LINK, and target voltage.
- Do not switch COM ports opportunistically. Identify the actual adapter first.

## Key Files

- `docs/test/rev0-low-energy-session-2026-08-30.md` - detailed evidence
- `docs/test/rev0-low-energy-comm-control-test.md` - test procedure
- `firmware/bringup/baremetal/` - flashed stage-1 source/build
- `firmware/scripts/rev0-openocd-run-probe.ps1` - proves firmware execution via SRAM
- `firmware/scripts/rev0-openocd-safe-idle.ps1` - restores safe GPIO state
- `firmware/scripts/windows-list-serial-usb.ps1` - lists current USB/serial devices

## Working Tree

Current relevant uncommitted work includes `.gitignore`, firmware/docs/scripts above. Existing modifications to `SOLAR_HANDOFF_2026-07-20.md` and `hardware/kicad/solar-project/Solar Project Rev1.kicad_pcb` predate this work; preserve them.
