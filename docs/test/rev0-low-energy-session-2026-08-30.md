# Rev 0 Low-Energy Session Log - 2026-08-30

Scope: recovered Rev 0 STM32 bring-up using SWD only. No flash erase, flash write, firmware upload, PV, battery, charger output, inverter, AC wiring, or load was used.

## Target Identification

STM32CubeProgrammer read-only SWD attach succeeded after the board was attached/powered:

```text
ST-LINK SN  : B55B5A1A0000000020AFF501
ST-LINK FW  : V2J37S7
Voltage     : 3.24V
Device ID   : 0x469
Device name : STM32G47x/G48x/G414
NVM size    : 512 KBytes
Device CPU  : Cortex-M4
```

This matches the recovered Rev 0 STM32G47x-class target expectation.

Note: one CubeProgrammer `-rdu` command was run during target checking. The target was already at RDP `0xAA`; CubeProgrammer reported that option bytes were unchanged and no data was downloaded.

## Tooling

- STM32CubeProgrammer v2.23.0 is installed and working for target identification.
- GNU Arm Embedded Toolchain installation through `winget` was canceled on the Windows side, so no local firmware build was attempted.
- xPack OpenOCD v0.12.0-7 was installed through `winget` and used for volatile SWD register access.
- Portable xPack ARM GCC was later downloaded under the ignored local `.tools/` directory. The bare-metal stage-1 firmware built successfully.

## Safety Gate

User confirmed before GPIO control tests: PV, battery, charger output, inverter/AC wiring, and loads were disconnected. Only low-energy control power/debug/test connections were in scope.

## SWD Register Tests

OpenOCD identified the target over ST-LINK at about 3.23 V to 3.24 V and successfully halted the Cortex-M4.

Initial halted register read:

```text
RCC_AHB2ENR 0x4002104C: 00000007
GPIOA 0x48000000: a82aaaaf 00000100 0c2aaaa0 6415aa50 0000c74c 00000000
GPIOB 0x48000400: aafffebf 00000000 aa0000c0 aa000100 00004010 00000000
```

Configured safe low outputs through volatile GPIO register writes:

- `PA5` / legacy `V_in_ON`: output, low
- `PA8` / `CTRL_PWM_MAIN`: output, low
- `PB5` / `CTRL_EN_CHG`: output, low

Readback after safe-output configuration:

```text
GPIOA 0x48000000: a829a6af 00000100 0c2aaaa0 6415aa50 0000c64c 00000000
GPIOB 0x48000400: aafff6bf 00000000 aa0000c0 aa000100 00004010 00000000
```

Configured `PB6` / `I2C DATA` and `PB7` / `I2C CLK` as open-drain outputs with pullups. Readback:

```text
Both released:  GPIOB_IDR = 000040d0
PB7 driven low: GPIOB_IDR = 00004050
Both released:  GPIOB_IDR = 000040d0
GPIOB regs:     aaff56bf 000000c0 aa0000c0 aa005100 000040d0 000000c0
```

Interpretation: `PB7` changed state when commanded low and returned high when released; `PB6` stayed high during this check. This verifies basic SWD control of the Rev 0 software-I2C pins at the MCU/register level.

Brief control-output toggle checks:

```text
PB5 high: GPIOB_IDR = 000040f0
PB5 low:  GPIOB_IDR = 000040d0
PA5 high: GPIOA_IDR = 0000c66c
PA5 low:  GPIOA_IDR = 0000c64c
```

Interpretation: `PB5` and `PA5` can be controlled through the MCU GPIO peripheral and read back at the input register. `PA8` / `CTRL_PWM_MAIN` was deliberately not toggled high in this session.

## Result

Pass for low-energy SWD communication and basic GPIO control:

- MCU is alive and identifiable over SWD.
- Target rail observed by ST-LINK is approximately 3.23 V to 3.24 V.
- GPIO peripheral register writes work through OpenOCD.
- `PB6` / `PB7` are usable as software-I2C GPIOs at the MCU/register level.
- `PB5` and `PA5` can be driven and read back.
- `PA8` remains low and was not pulsed.

The repeatable scripts in `firmware/scripts/` were added and validated:

- `rev0-openocd-identify.ps1`: pass; target halted/readable at about 3.23 V.
- `rev0-openocd-safe-idle.ps1`: pass; `PA5`, `PA8`, and `PB5` low, `PB6`/`PB7` released high.
- `rev0-openocd-i2c-gpio-check.ps1`: pass; `PB7` changed from released high to driven low and back high, while `PB6` remained high.
- `rev0-openocd-control-gpio-check.ps1`: pass; `PB5` and `PA5` each changed high/low and read back correctly. `PA8` was not toggled.

Run these scripts one at a time. They all claim the same ST-LINK/SWD target, and concurrent access is not a valid test condition.

## Stage-1 Firmware Flash and Execution

The bare-metal stage-1 image was built from `firmware/bringup/baremetal/`:

```text
text=1976 data=0 bss=100 dec=2076 hex=81c
```

Pre-flash target verification still matched the recovered STM32G47x-class MCU:

```text
Voltage     : 3.23V to 3.24V
Device ID   : 0x469
Device name : STM32G47x/G48x/G414
NVM size    : 512 KBytes
```

Flash write and verify succeeded:

```text
File          : solar_rev0_stage1.bin
Size          : 1.93 KB
Address       : 0x08000000
Download verified successfully
```

Flash vector table readback was valid:

```text
0x08000000 : 20018000 08000040
```

An initial reset/run probe halted in system memory at `PC=0x1fff3b4a`, and the SRAM probe did not contain the firmware magic. Option-byte readback showed:

```text
nBOOT1   : 0x1 (Boot from Flash if BOOT0 = 0, otherwise system memory)
nSWBOOT0 : 0x1 (BOOT0 taken from PB8/BOOT0 pin)
nBOOT0   : 0x1
```

Do not change option bytes yet. The safer next bench check is the physical `PB8`/BOOT0 level and whether it needs a temporary pulldown for normal reset boot.

Manual SWD launch from the programmed flash vector succeeded:

```text
MSP loaded: 0x20018000
PC loaded:  0x08000040
Halted PC:  0x08000304
0x20000000: 53523030 00000001 001085d3 0000a64c 000040d1 00000000 000000c0 00000000
```

Interpretation: magic `0x53523030`, version `1`, and nonzero heartbeat prove the stage-1 firmware executes when started from user flash by SWD.

## 2026-08-31 Normal-Reset Follow-Up

`PB8`/BOOT0 measured approximately 0.30 V while reset was pressed. Five read-only reset checks loaded the flash vector (`PC=0x08000040`, `MSP=0x20018000`), so the current board is selecting user flash rather than system memory.

The original stage-1 vector stored even reset address `0x08000040`. On normal hardware reset this caused `INVSTATE` (`CFSR=0x00020000`) and trapped in the default handler. The startup handlers were marked as Thumb functions, producing corrected vector `0x08000041`.

Before reflashing, target identity was verified as ST-LINK `B55B5A1A0000000020AFF501`, 3.23 V, Device ID `0x469`, STM32G47x/G48x/G414, 512 KB. The corrected 1.93 KB image downloaded and verified successfully. Normal reset/run then produced SRAM magic `0x53523030`, an increasing heartbeat, and zero `CFSR/HFSR` without manually loading MSP or PC.

The repaired onboard CP210x at verified `COM5` returned the complete expected help response at 115200 8N1. `status` reported `OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`.

Five later physical USB power cycles did not reproduce that user-flash boot. The CP210x continued to enumerate on the exact verified `COM5` identity, but UART was silent. A non-resetting SWD snapshot found `PC=0x1FFF3B08` in system ROM and no SRAM heartbeat. Pressing and releasing S1 did not change the outcome; the next snapshot remained in system ROM at `PC=0x1FFF3B24`. H2 remains blocked pending direct NRST voltage and continuity checks plus BOOT0 observation at power-on.

Direct NRST measurement then passed: the node identified as STM32 pin 7 / `PG10` measured approximately 3.2 V with S1 released and 0.2 V with S1 pressed. This rules out an open or non-actuating reset-button path and leaves BOOT0 sampling at reset release/power-on as the leading hardware issue.

A later non-resetting snapshot found the CPU still in system ROM at `PC=0x1FFF3B22` while `GPIOB_IDR=0x00004010` showed PB8 low at observation time. Because PB8 is unconnected at QFN pad 46, this supports a brief floating or high state during the boot sampling window rather than a steady-state high level visible to a DMM.

Direct QFN-pad rework was judged disproportionate. After explicit approval, the target was reverified as ST-LINK `B55B5A1A0000000020AFF501`, 3.23-3.24 V, Device ID `0x469`, STM32G47x/G48x/G414. Only `nSWBOOT0` was programmed from `1` to `0`, selecting the retained `nBOOT0=1` option value. Independent full readback confirmed `nSWBOOT0=0`, `nBOOT0=1`, and unchanged remaining option bytes. A subsequent hardware reset ran user flash at `PC=0x080002E0`; SRAM contained magic `0x53523030`, version `1`, and a live heartbeat. A true cold-power cycle is still required before H2 closes.

The first post-correction cold-power cycle passed. After the user removed and restored board USB power, a COM5-only query returned `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0` without SWD reset or manual launch. This proves user-flash startup from zero power and preserves the safe low state of all three power-control outputs. H2 cold-power progress is 1/5.

Four additional board-USB power cycles then passed, closing cold-power startup at 5/5. Five physical S1 press/release cycles also passed. A COM5-only status query after the final reset returned `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`; no SWD reset or manual launch was used. H2 is closed, with user-flash boot reliable and all power-control outputs inactive.

Rev 1 shall include a populated 10 kOhm BOOT0-to-`PWR_NEG` resistor footprint and an accessible BOOT0 test point. The programmed option-byte state is defense in depth and does not replace deterministic hardware biasing.

## 2026-08-31 U6 First Static-Power Point

H3 entry checks passed for a bounded first point: U6 top marking was `LM51772 / TI 4AB / C9EP G4`, its package dot aligned with the PCB pin-1 marker, M1-M4 were physically unpopulated, all external sources and loads were disconnected, and `PV_IN_POS` to `PWR_NEG` measured 180 kOhm unpowered. Firmware reported `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0` immediately before application.

With USB maintaining `CTRL_3V3` and U6 RST, a current-limited 6.0 V / 20 mA source was applied to `PV_IN_POS`. The source remained stable and displayed 6.00 V / 0 mA, with no abnormal U6 heating over 5-10 seconds. Measurements relative to `PWR_NEG` were: `PV_IN_POS=5.993 V`, `VCC1=3.998 V`, `VCC2=3.5921 V`, `EN_UVLO=0.804 V`, `RST=3.299 V`, and `FLT=0.023 V`.

H3 remains on hold. Live Rev 0 files corrected the EN lower divider references to R61 = 20 kOhm in series with R60 = 1.5 kOhm; the older exported netlist's R24 reference was stale. R60 measured 1.48 kOhm, while R61 measured about 17 kOhm and the total about 18 kOhm in circuit because U6 pin 37 presents a parallel semiconductor path. TI Rev. A specifies a 1.23-1.27 V rising threshold and a 4-6 uA hysteresis sinking current. With R23 = 75 kOhm and a 21.5 kOhm lower leg, the typical predicted VIN turn-on is about 5.99 V. The observed 0.804 V EN at 5.993 V VIN remains unexplained.

The exact TI LM51772-Q1 Rev. A datasheet then exposed a controlling design violation: Section 7.3.7.2 states, "Do not connect an external load to the VCC2-PIN." Both the live Rev 0 PCB and exported netlist connect fitted D8 VDD directly to `U6_VCC2`. No further U6 power increase is authorized until D8 pad 2 is isolated from VCC2.

The fitted D8 is XL-1615RGBC-WS2812B, LCSC C5349954. Its datasheet specifies a 3.5-5.5 V application supply and VIH = 2.8 V at VDD = 5 V. The conservative Rev 0 remedy is to isolate D8 pad 2 from `U6_VCC2` and feed it from a separately qualified 5 V rail with local decoupling; `CTRL_3V3` is not the default remedy. Rev 1 must make this connection correctly and include a deliberate D8 supply isolation link or resistor footprint.

Rev 0 rework update 2026-08-31: D8 VDD was moved from `U6_VCC2` to USB `VBUS`. This is not yet electrically accepted. Before USB or PV power is restored, verify D8 pad 2 is open to U6 pin 29, continuous to `VBUS`, not shorted to `PWR_NEG` or adjacent D8 pads, and that U6 pin 29 remains connected to C29/C45. C29 and C45 are U6-local bypass capacitors; install a separate 100 nF ceramic directly across D8 pads 2 and 3. For D8 testing, USB shall be the only VBUS source; no external 5 V source may be connected concurrently.

Rework measurements: D8 pad 2 to U6 pin 29 = 300 kOhm; D8 pad 2 to `VBUS` = 0.4 Ohm; D8 pad 2 was reported not shorted to pads 1 or 4; U6 pin 29 retained continuity to C29 and C45. D8 pad 2-to-`PWR_NEG` resistance was not supplied, and the local 100 nF D8 bypass is not fitted; these are accepted as noncritical Rev 0 risks and remain Rev 1 corrections. USB operation showed no abnormal heat, smell, COM5 instability, or control-output behavior.

H3 second static point 2026-08-31: Before applying PV power, the overnight image showed `UP 06:13:23`, `N 1543`, and `E 0`; the OLED and RGB heartbeat were normal. The verified onboard CP2102 remained on `COM5` with identity `USB\VID_10C4&PID_EA60\0001`. UART returned `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`, and AHT20 returned status `0x18`, 26.6 C, and 32.0% RH.

With USB control power unchanged, the isolated PV source was set to 6.5 V with a 20 mA limit and applied at J1 `PV_IN_POS_RAW`/`PWR_NEG`. It remained stable and displayed 6.50 V / 0 mA with no abnormal U6 heating, odor, or instability. Correction recorded 2026-09-01: the approximately 6.49 V DMM reading was also at J1 `PV_IN_POS_RAW`, not at U6's `PV_IN_POS`; `PV_IN_POS` was approximately 4.54 V. Measurements relative to `PWR_NEG` were `VCC1=4.5471 V`, `VCC2=4.5415 V`, `EN_UVLO=1.030 V`, `RST=3.29 V`, and `FLT=0.0296 V`. The PV source was then switched off, and the input/U6 rails decayed below 0.1 V. Gate-output activity was not checked at this point.

H3 remains partial. At the corrected `PV_IN_POS` of approximately 4.54 V, `EN_UVLO=1.030 V` is broadly consistent with the documented R23/R61/R60 divider and remains below the LM51772 1.23-1.27 V rising threshold; the earlier apparent divider discrepancy was caused by comparing EN/UVLO against the raw J1 voltage. Do not increase the PV source above 6.5 V. The active blocker is the unexpected approximately 1.95 V drop between `PV_IN_POS_RAW` and `PV_IN_POS` through the PV front end. Before another powered point, verify the fitted U2 pinout and the U2/Q2/R40-R43/D1 control topology unpowered. Do not command `V_in_ON`/PA5 with PV present until its safe behavior is established. Any later powered point must separately record `PV_IN_POS_RAW`, `PV_IN_POS_FUSED`, `PV_IN_POS`, U2 gate/source voltages, and no-switching status on HO1, LO1, HO2, and LO2.

U2 topology review 2026-09-01: the fitted BSC112N06LD is an N+N common-source back-to-back switch. With `V_in_ON`/PA5 low, Q2 is off and R43 can pull the U2 gates only toward `PV_IN_POS_FUSED`, leaving no positive gate overdrive above the floating common source. PA5 high would turn Q2 on and pull the U2 gates low, also turning the high-side NMOS pair off. This confirms the earlier SPICE finding that the U2/Q2/R40-R43/D1 network has no valid enhanced ON state. A 6.5 V / 20 mA diagnostic repeat is permitted only with PA5/PA8/PB5 held low and measurements of raw, fused, protected, U2 gate, and U2 common-source nodes; use D1 pin 1 for the gate net and D1 pin 2 for the common-source net rather than probing U2. Do not pulse PA5 or increase the source.

U2 diagnostic result 2026-09-01: at the unchanged 6.5 V / 20 mA source point, `PV_IN_POS_RAW=6.492 V`, `PV_IN_POS_FUSED=6.492 V`, `PV_IN_POS=4.543 V`, and source current was 416 uA. The U2 gate measured 6.42 V relative to `PWR_NEG`; D1 measured 1.92 V anode-to-cathode across the U2 common-source/gate network. The inferred approximately +1.92 V U2 gate-to-common-source bias places the device pair near threshold, and the inferred common-source voltage near 4.50 V agrees with `PV_IN_POS`. This confirms that the 1.949 V front-end loss is caused by the invalid U2 gate drive. F1 was intentionally jumpered for this current-limited bench test, so the equal raw and fused readings do not validate the fuse or its contacts.

U2 diagnosis is closed and H3 remains blocked. Remove PV power and verify all three PV nodes below 0.1 V. Do not continue U6 characterization through U2. Select and document either a temporary bench-only bypass from `PV_IN_POS_FUSED` to `PV_IN_POS` or permanent U2/Q2 rework before another powered point. A temporary bypass defeats U2 reverse blocking and is limited to the isolated, current-limited, no-battery, no-load, no-switching bench configuration.

Power-off preparation: USB, ST-LINK, and PV leads were removed, and `VBUS`, `CTRL_3V3`, `PV_IN_POS_FUSED`, and `PV_IN_POS` were confirmed below 0.1 V. The existing jumper spans F1 only. No separate U2 bypass has been fitted; approximately 195 kOhm from `PV_IN_POS_FUSED` to TP4 confirms this. Keep the board unpowered until the separate bypass is installed and verifies near lead resistance.

Temporary bypass update 2026-09-01: U2 and U5 were both bypassed. At the bounded 6.49 V input point, TP4 / `PV_IN_POS` equaled `PV_IN_POS_RAW`. U5 uses the same BSC112N06LD common-source pair on the output path and has the same invalid gate-drive issue; its bypass remains relevant only for later output-path work because M1-M4 are unpopulated and no output source, battery, or load is authorized. Both jumpers remove their original reverse-blocking behavior and are restricted to attended, isolated, current-limited bench testing.

First bypassed H3 measurement: `_Undervoltage Feedback`, confirmed by the netlist as U6 pin 37 `EN_UVLO`, measured 1.43 V at `PV_IN_POS=6.49 V`. This is above the 1.23-1.27 V rising threshold. The bench supply displayed approximately 1 mA while the inline meter measured 2.1 mA; record 2.1 mA conservatively. Relative to `PWR_NEG`, `VCC1=5.0375 V`, `VCC2=4.9706 V`, `FLT=0.02 V`, and `RST=3.29 V`. The VCC rails reached their enabled-state region, but FLT remained asserted. Keep the source at or below this point. Thermal stability and no-switching/static state of HO1, LO1, HO2, and LO2 remain open.

H3 bounded static observation completed: U6 and the U2/U5 bypass jumpers remained cool, with no odor, instability, or current limiting. DMM checks at the empty power-MOSFET footprints measured 0 V for M2 gate-to-source / LO1, M3 gate-to-source / LO2, M1 gate-to-source / HO1-to-SW1, and M4 gate-to-source / HO2-to-SW2. The 6.49 V point therefore passes the bounded thermal and no-gate-activity checks. It does not authorize switching: `FLT` remains asserted at 0.02 V, M1-M4 remain unpopulated, and the F1/U2/U5 bypasses defeat normal protection boundaries.

Closeout: PV power was removed. `PV_IN_POS_RAW`, `PV_IN_POS_FUSED`, TP4 / `PV_IN_POS`, `VCC1`, and `VCC2` were all confirmed below 0.1 V. Continue with USB-only control power. The next bounded H3 task is to determine why U6 FLT remains asserted; do not raise the PV input.

U6 I2C configuration diagnosis: R33 is fitted as 6.49 kOhm between U6 pin 9 `ADDR_SLOPE_CFG1` and PGND. LM51772-Q1 Table 7-4 maps the nominal 6.5 kOhm row to slope setting 1.0 with I2C disabled. Only the GND and VCC2 rows enable I2C, at `0x6A` and `0x6B` respectively. The fitted Rev 0 board therefore cannot read the fault reason without changing the R2D strap and power-cycling U6.

Firmware added command `u6 status`, which reads only `STATUS_BYTE` register `0x78` at address `0x6A` using write-address/register-pointer, repeated START, read-address, one data byte, nACK, and STOP. It does not access `CLEAR_FAULTS` register `0x03`, whose read or write would clear latched flags. The build passed with `text=5976`, `data=0`, `bss=1156`. Exact pre-flash identification passed for ST-LINK `B55B5A1A0000000020AFF501`, 3.24 V, device `0x469`, STM32G47x/G48x/G414, and 512 KB NVM. The 5.84 KB image programmed and verified successfully. With PV off, COM5 returned `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`, followed by the expected `U6 STATUS_BYTE read failed at I2C address 0x6A` while R33 remained fitted.

R33 was removed and replaced by a zero-ohm link while unpowered. Continuity across the pads measured 0 ohms and inspection found no bridge to adjacent pads. USB-only serial checks initially suffered command-pacing timeouts, but a CR-terminated powered scan completed normally. At the previously qualified 6.49 V / 20 mA-limit point, source indication was approximately 1 mA, the board remained cool and stable, and VCC2 measured 4.97 V. `u6 status` received no acknowledgement at `0x6A`; `i2cscan` found only `0x38` and `0x3C`.

The diagnostic premise was then corrected: R33 pad 2 belongs to `PGND`, while the bench and controller reference used for measurements is `PWR_NEG`. The exported netlist lists these as separate nets with no joining component. U6 pin 9 initially measured 0.63 V relative to `PWR_NEG` despite the zero-ohm R33 link, proving the link did not establish the required logic-low strap and exposing a broader Rev 0 grounding defect.

PGND repair: a jumper was installed between R31 pad 2 (`PGND`) and R32 pad 2 (`PWR_NEG`). PCB and netlist inspection confirmed both pad numbers and verified that the signal-side pads were not bypassed: R31 remains 2.43 kOhm in the compensation network and R32 remains 100 kOhm from `ILIMCOMP_ISET` to PWR_NEG. Unpowered checks confirmed the intended return short and retained resistor paths. Zero-ohm R33 remained installed, connecting U6 pin 9 to the now-referenced PGND net.

After USB-only startup, COM5 returned `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. At the bounded 6.49 V / 20 mA-limit point, the source display showed 0 mA at its available resolution, all hardware remained cool and stable, and U6 pin 9 measured 0.0001 V relative to PWR_NEG. A powered `i2cscan` completed normally and found only `0x38` and `0x3C`; U6 still did not acknowledge at `0x6A`. The working shared bus and valid low strap leave U6 SDA/SCL pin attachment, fitted-device identity, or local U6 soldering as the remaining diagnostic boundary. PV was removed and the user confirmed no backfeed.

U6 pin-area inspection removed debris but found no visible short or defect expected to affect the bus. Unpowered continuity and isolation checks passed for U6 pin 5 (`SDA_CFG3`) and pin 6 (`SCL_CFG4`), after which their solder joints were reworked. The first post-rework scan was USB-only because PV had not been restored; its `0x38`/`0x3C` result did not test U6. PV was then explicitly applied at the bounded 6.49 V / 20 mA-limit point. With U6 powered and `VCC2=4.97 V`, a second paced scan still found only `0x38` and `0x3C`. U6 did not acknowledge at `0x6A`. PV was switched off afterward. Device identity and orientation remain consistent with the earlier recorded `LM51772 / TI 4AB / C9EP G4` marking. Static continuity is no longer discriminating; the next check is dynamic SCL/SDA waveform arrival at U6 pins 6/5.

On 2026-09-01, a verbose `i2cscan full` command was added to enumerate every legal 7-bit address from `0x08` through `0x77` and report each ACK or NACK. Reserved address groups were intentionally excluded. The updated image built cleanly with `-Wall -Wextra -Werror`; pre-flash identity matched ST-LINK `B55B5A1A0000000020AFF501`, target voltage 3.23-3.24 V, device ID `0x469`, STM32G47x/G48x/G414, and 512 KB NVM. Programming and byte verification succeeded. The USB-only functional check found ACKs only at `0x38` and `0x3C`. After the user restored and confirmed the bounded 6.49 V / 20 mA-limit PV state, firmware reported `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`, and the powered full scan again found only `0x38` and `0x3C`. All other legal addresses NACKed, specifically including both `0x6A` and `0x6B`. The scan sent address-write probes only and did not access any register. The user switched PV off immediately afterward.

On 2026-09-02, firmware command `i2cscan unknown repeat` was added for a long logic-analyzer clock capture. The command disables automatic AHT20/OLED traffic, scans every legal 7-bit address four times, excludes `0x38` and `0x3C`, and reports only unexpected ACKs. The image built cleanly; ST-LINK `B55B5A1A0000000020AFF501`, target voltage 3.23-3.25 V, device ID `0x469`, STM32G47x/G48x/G414, and 512 KB NVM were verified before programming, and download verification passed. After periodic traffic was explicitly stopped, safety outputs remained `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. The user confirmed the analyzer armed and PV stable within 6.49 V / 20 mA. Four sweeps produced 440 address-only probes and zero ACKs. The analyzer capture was saved and PV was switched off. No register was addressed or fault status cleared.

A subsequent bounded read-only attempt targeted `STATUS_BYTE` register `0x78` at 7-bit address `0x6A`. Automatic bus traffic was stopped after PV application, safety outputs remained low, and the analyzer was armed. Firmware returned `U6 STATUS_BYTE read failed at I2C address 0x6A`, meaning the transaction did not complete and no register value was obtained. The implementation aborts on a NACK and does not access `CLEAR_FAULTS` register `0x03`. The analyzer capture was saved, and the user confirmed PV off afterward. The capture should be decoded to locate the failed ACK phase; this result must not be interpreted as a U6 status or fault value.

The saved logic-analyzer capture resolved the failed phase. KingsVIS decoded `Setup Write to [0xD4] + NAK`; `0xD4` is the raw 8-bit write address corresponding to 7-bit address `0x6A`. Nine clean SCL pulses were present at U6 pin 6, the address data pattern was present at U6 pin 5, and SDA remained high during the ninth clock. U6 did not pull SDA low for ACK. The firmware therefore correctly stopped before transmitting register pointer `0x78`, and no register data or fault state was accessed. This pin-level evidence rules out a missing transaction at U6 and supports a local U6 device/package assembly failure after the previously verified supply, address strap, PGND repair, orientation, continuity, and solder rework. PV remained off after capture.

U6 was subsequently replaced on 2026-09-02. The user confirmed correct orientation, retained R33 zero-ohm address link and PGND-to-PWR_NEG jumper, passed unpowered short checks, and restored only the bounded input at 6.49 V maximum / 20 mA. Source current was approximately 3 mA; U6 remained cool and stable with no current limiting or unexpected gate activity. Powered measurements were `VCC2=4.95 V` and pin 9 `ADDR_SLOPE_CFG1=0.009 V` relative to PWR_NEG. Firmware stopped automatic bus traffic and reported `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. A single address-only probe again returned `U6 0x6A NACK`. The test was repeated once with analyzer channels connected directly to replacement-U6 pin 5 SDA and pin 6 SCL. The captured nine-clock transaction samples SDA as `11010100` (`0xD4`, corresponding to 7-bit address `0x6A` plus write), with SDA released high during the ninth clock for NACK followed by STOP. This independently confirms that the replacement package receives the correct address transaction at its physical pins but does not acknowledge it. The user switched PV off afterward. Because the replacement device exhibits the same pin-level address-phase failure, the prior local-device/die-attachment conclusion is withdrawn. The remaining diagnosis is a persistent board-level reset, ground/reference, configuration-latch, or package-pad connection condition. Verify powered RST pin 38 and AGND/PGND/exposed-pad reference integrity before any further I2C attempt.

D8 disposition update: D8 is a noncritical MPU-running indicator and no longer gates bring-up. Stage-1 firmware now sends an automatic minimum-brightness RGB heartbeat on PB2: red, green, and blue pulses approximately 250 ms on and 750 ms off. Build passed with warnings treated as errors. Before programming, the exact ST-LINK `B55B5A1A0000000020AFF501`, 3.24 V target, device `0x469`, STM32G47x/G48x/G414, and 512 KB NVM were reverified. Programming and byte verification succeeded, and the post-reset COM5 query returned `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. Proceed with U6 and I2C hardware work regardless of D8 appearance.

D8 closure: the first RGB image did not visibly update D8 because the compiled C sender produced roughly 667 kHz instead of the required 800 kHz. `d8_send_byte` was replaced with a balanced fixed-cycle Thumb-2 implementation for the known 16 MHz clock. After a clean rebuild, machine-code inspection, exact probe/target reverification, successful programming, and byte verification, the RGB heartbeat was visually confirmed correct at an appropriate brightness. H4 passes with the current red/green/blue 250 ms pulse and 750 ms dark cadence as the Rev 0 baseline.

H5 I2C result: Pass at the address/logic level. PV remained off; USB powered `CTRL_3V3`. The software-I2C driver correctly accommodates the Rev 0 PB6/PB7 route swap and uses open-drain outputs. R44 and R45 are both 4.7 kOhm pull-ups to `CTRL_3V3`. `i2cscan` on COM5 reported exactly `0x38` and `0x3C`, consistent with the fitted AHT20 and SSD1306. No unexpected address or stuck bus was observed.

H6 AHT20 initial functionality: Pass. Firmware added an `aht20` command that writes trigger bytes `AC 33 00`, waits 100 ms, reads six bytes, rejects busy status, and decodes the two 20-bit measurements. After exact target reverification and verified programming, COM5 returned `AHT20 status=0x18 temperature=26.7C humidity=39.9%RH`. Status `0x18` shows calibrated and not busy. The immediately preceding safety response remained `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`.

H7 SSD1306 initial functionality: Pass. Firmware added `oled test`, which initializes scanned address `0x3C` at low contrast and sends a full 1024-byte checkerboard in 16-byte I2C data chunks. After exact target reverification and verified programming, COM5 returned `SSD1306 checkerboard drawn` with no NACK, and the checkerboard was visually confirmed on the panel. A follow-up AHT20 sample returned status `0x18`, 26.7 C, and 39.5% RH, proving the shared bus remained functional after the display transfer. Power-control outputs remained low before the test.

H8 overnight stability run started 2026-08-31. Firmware now auto-starts an OLED dashboard showing software time of day, reset-relative uptime, AHT20 temperature/humidity, successful sample count, and cumulative error count. It samples AHT20 every 10 seconds and refreshes the display every second while retaining the RGB heartbeat. UART capture was added inside software-I2C delays after the first dashboard image exposed receive overruns during full-screen transfers. The corrected image built cleanly, was programmed only after exact target reverification, and passed byte verification. PC local time `07:44:44` was accepted with `Clock set`; subsequent COM5 commands returned safe output states and `AHT20 status=0x18 temperature=27.1C humidity=39.5%RH`. Run with USB control power only and all energy-path connections absent.

## USB-UART Enumeration Finding

Windows showed no present CP2102/Silicon Labs/USB serial device after the board USB was connected. The active netlist explains the symptom:

- USB connector D-: `J5 A7/B7` connects to `D3` pin 1 on `Net-(D3-Pad1)`.
- USB connector D+: `J5 A6/B6` connects to `D3` pin 3 on `Net-(D3-Pad3)`.
- CP2102 D+: `U3` pin 4 connects to `D3` pin 4 on `Net-(U3-D+)`.
- CP2102 D-: `U3` pin 5 connects to `D3` pin 6 on `Net-(U3-D-)`.

Those are separate nets in the exported netlist. Unless the `USBLC6-2SC6` footprint intentionally shorts those pad pairs internally, the USB connector data pair does not reach the CP2102. This matches the bench observation: USB can provide VBUS/control power, but no USB serial device enumerates.

Next bench checks for this issue:

1. With power removed, measure continuity from USB-C D+ to CP2102 pin 4 and USB-C D- to CP2102 pin 5.
2. With USB connected, verify CP2102 `REGIN`/`VBUS` have USB 5 V and CP2102 `VDD` is at its expected 3.3 V state.
3. If D+/D- continuity is open, use an external USB-UART adapter on the STM32 UART nets instead of the onboard USB-C data path.

Next step: install or enable a firmware build toolchain, then flash a safe serial bring-up image only after repeating target identity verification.
