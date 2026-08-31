# Rev 0 Hardware Bring-Up Execution Plan - 2026-08-30

Status: bench execution plan
Board: recovered Rev 0 with fitted STM32G47x-class MCU
Priority: prove hardware paths and fitted devices; firmware is only test stimulus and observation

Progress update 2026-08-31: H1 USB hardware repair, enumeration, and UART pass. D3 was wired incorrectly and solder bridges were found at the USB connector. After correction, the onboard CP210x enumerates as `COM5` and returns the expected firmware command response at 115200 8N1. H2 passes 5/5 cold-power cycles and 5/5 reset-button cycles after correcting the stage-1 Thumb reset vector and changing `nSWBOOT0` from `1` to `0` so the existing `nBOOT0=1` option bit forces user-flash boot.

## 1. Objective and Order

Execute one stage at a time and record evidence before advancing:

1. Repair or bypass the onboard USB-to-UART debug path.
2. Make normal reset boot user flash reliably.
3. Perform the minimum static U6 power check needed to establish `U6_VCC2`.
4. Prove D8 power, DIN waveform, and visible heartbeat.
5. Prove the shared software-I2C bus electrically.
6. Identify and read U7/AHT20.
7. Identify and exercise U8/SSD1306 OLED.
8. Close the session in a known safe state.

The U6 static check must precede D8 because D8 VDD is connected to `U6_VCC2`; D8 is not powered from `CTRL_3V3`. This plan does not authorize U6 gate-drive or converter switching.

## 2. Fixed Hardware Facts

| Item | Rev 0 implementation |
|---|---|
| Verified SWD probe | ST-LINK serial `B55B5A1A0000000020AFF501` |
| Verified MCU response | Device ID `0x469`, STM32G47x/G48x/G414, approximately 3.23-3.24 V |
| UART | USART1, 115200 8N1; STM32 `PA9` TX to U3 RXD, STM32 `PA10` RX from U3 TXD |
| USB bridge | U3 CP2102; USB-C data path crosses D3/USBLC6-2SC6 |
| Reset boot issue | `PB8/BOOT0` floats and caused reset into system memory; Rev 0 now uses `nSWBOOT0=0`, `nBOOT0=1` |
| D8 | WS2812B-compatible 1615 RGB LED; `PB2 -> DIN`; VDD is `U6_VCC2`; GND is `PWR_NEG` |
| Software I2C | STM32 `PB6 = I2C DATA/SDA`; `PB7 = I2C CLK/SCL` |
| I2C devices | U6 LM51772 configuration interface, U7 AHT20, U8 0.96-inch SSD1306 OLED |
| U7 power | `CTRL_3V3` and `PWR_NEG` |
| U8 power | `CTRL_3V3` and `PWR_NEG` |
| U6 fault | `U6_FLT`, pulled up by R34 and intended to land at STM32 `PC13` |

The STM32 hardware I2C peripheral cannot be used with the routed pin assignment. All Rev 0 I2C tests use open-drain software I2C on PB6/PB7.

## 3. Non-Negotiable Limits

- Keep the real PV panels, battery, charger output, inverter, AC wiring, and loads disconnected.
- M1-M4 are unpopulated. Do not attempt inverter or bridge operation.
- Keep `PA8/CTRL_PWM_MAIN`, `PB5/CTRL_EN_CHG`, and `PA5/V_in_ON` low.
- Never configure `PB11` as an output; it is tied to `PWR_NEG` on Rev 0.
- Do not probe U6 switch nodes, command gate drive, or pulse PWM in this plan.
- Use one power source at a time unless a step explicitly defines otherwise.
- Before every flash, verify all of the following and stop on any mismatch:
  - ST-LINK serial `B55B5A1A0000000020AFF501`
  - Device ID `0x469`
  - STM32G47x/G48x/G414-class target
  - target voltage in the expected approximately 3.2-3.3 V range
- Do not select another COM port because it appears available. Identify the connected adapter by USB identity and connection change.
- Stop immediately for current limiting, rail collapse, unexpected heating, odor, unstable reset, unexplained fault state, or any control output going active.

## 4. Equipment and Pre-Session Record

| Item | Setting or record |
|---|---|
| Board identifier / photos |  |
| Isolated current-limited supply | Model: ___; initial current limit: ___ |
| DMM | Model: ___ |
| Oscilloscope / logic analyzer | Model: ___; probe ground verified at `PWR_NEG` |
| SWD probe | Expected serial recorded above |
| USB cable | Known data-capable cable: Pass / Fail |
| External 3.3 V USB-UART adapter | Model and USB identity: ___ |
| Firmware build/hash |  |
| Evidence directory | `evidence/2026-08-30-rev0-board___/` |

Before power, confirm and record:

- [ ] PV disconnected.
- [ ] Battery disconnected.
- [ ] Charger output disconnected.
- [ ] Inverter and AC wiring disconnected.
- [ ] No load connected.
- [ ] `VBUS` to `PWR_NEG` is not a near-short: ___ ohm.
- [ ] `CTRL_3V3` to `PWR_NEG` is not a near-short: ___ ohm.
- [ ] U3, D3, D8, U6, U7, and U8 have no visible bridge, wrong orientation, or damaged joint.

## 5. Stage H0 - Establish Safe Idle

1. Apply the intended USB/VBUS-equivalent control power through a current-limited source.
2. Record source voltage, source current, `VBUS`, and `CTRL_3V3`.
3. Run `firmware/scripts/rev0-openocd-identify.ps1`.
4. Confirm the expected probe, device ID, target family, and voltage.
5. Run `firmware/scripts/rev0-openocd-safe-idle.ps1`.
6. Scope or measure `PA8`, `PB5`, and `PA5`; each must be low.

Pass:

- Stable control rails, no abnormal heating, expected SWD identity, and all three control outputs low.

Record: `VBUS` ___ V; `CTRL_3V3` ___ V; source current ___ mA; result ___

## 6. Stage H1 - USB Debug Port Diagnosis and Repair

### H1.1 Unpowered data-path checks

Remove all power and cables before continuity measurements.

1. Verify the fitted D3 package marking and physical pin-1 orientation against the exact USBLC6-2SC6 datasheet and PCB footprint.
2. Measure end-to-end continuity:
   - J5 USB-C D+ contact group to U3 CP2102 pin 4/D+.
   - J5 USB-C D- contact group to U3 CP2102 pin 5/D-.
3. Measure each segment on both paths across D3 to locate an open or footprint/pin-map mismatch.
4. Check D+ to D-, D+ to ground, and D- to ground for an unintended short.

Do not install a jumper until D3 pin numbering, connector-side nets, and U3-side nets have been physically identified. A wrong jumper can short or swap the USB pair.

Record:

| Check | Reading / result |
|---|---|
| J5 D+ to U3 pin 4 |  |
| J5 D- to U3 pin 5 |  |
| D+ to D- |  |
| D+ to `PWR_NEG` |  |
| D- to `PWR_NEG` |  |
| D3 marking and pin-1 orientation |  |

### H1.2 Powered U3 checks

1. Restore current-limited VBUS power.
2. Measure U3 `REGIN`/pin 7 and `VBUS`/pin 8; both should reflect USB VBUS as designed.
3. Measure U3 `VDD`/pin 6; record the actual regulated value.
4. Check U3 temperature for abnormal heating.
5. Run `firmware/scripts/windows-list-serial-usb.ps1` before and after connecting the board with the known data cable.

### H1.3 Repair decision

- If U3 power is wrong, repair the U3 supply/ground path before touching D+/D-.
- If U3 power is correct and one or both data paths are open at D3, document and implement only the smallest verified rework that restores straight-through D+ and D- while retaining or deliberately bypassing D3.
- If D+/D- are swapped, correct both paths as a pair.
- If continuity and power pass but enumeration fails, inspect signal-path soldering and test with the same known host/cable before considering U3 replacement.
- If onboard repair is not reliable today, use the identified external 3.3 V USB-UART adapter: adapter RX to STM32 `PA9`, adapter TX to STM32 `PA10`, and adapter GND to `PWR_NEG`. Do not connect the adapter power pin.

Pass:

- A uniquely identified CP2102 or external adapter appears when connected and disappears when removed.
- At 115200 8N1, STM32 TX is visible and a received character can reach STM32 RX.
- The selected debug path is recorded; no COM-port guessing is used.

Selected debug path: onboard U3 / external adapter
USB identity: Silicon Labs CP210x, `USB\VID_10C4&PID_EA60\0001`, status `OK`
COM port after identity check: `COM5`
Repair details and photos: D3 wiring corrected and USB connector solder bridges removed; photos: ___

H1 status 2026-08-31: Pass. After the H2 startup correction, `COM5` returned the complete expected help response to `?` at 115200 8N1. A `status` command returned `OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. The three power-control outputs are safely low; the observed fault-input states must be explained before any later power-stage test.

## 7. Stage H2 - Normal Reset / BOOT0

1. With firmware already verified in flash, measure `PB8/BOOT0` at idle and during reset.
2. If it is not reliably low, power down and fit a documented temporary pulldown suitable for the BOOT0 input.
3. Power up, press reset, and observe the UART banner or SWD SRAM heartbeat without manually loading MSP/PC.
4. Repeat at least five power cycles and five reset-button cycles.
5. If direct QFN rework is disproportionate, change only `nSWBOOT0` after exact target verification, retain `nBOOT0=1`, and independently read back all option bytes before continuing.

Pass:

- `PB8/BOOT0` is reliably low at every reset sample.
- User flash starts on 5/5 power cycles and 5/5 reset cycles without a manual SWD launch.

Record 2026-08-31: BOOT0 while reset is pressed approximately 0.30 V and changes by only a few millivolts. No pulldown was added. Read-only debug-reset checks loaded `PC=0x08000040` and `MSP=0x20018000` on 5/5 attempts, proving flash selection under that reset method. The original image then trapped with `CFSR=0x00020000` (`INVSTATE`) because its vector contained even reset address `0x08000040`. Adding `.thumb_func`/function type annotations to the startup handlers generated corrected vector `0x08000041`. After strictly verifying ST-LINK `B55B5A1A0000000020AFF501`, Device ID `0x469`, STM32G47x/G48x/G414, and 3.23 V, the corrected image was flashed and verified. Debug-driven normal reset then produced SRAM magic `0x53523030`, an increasing heartbeat, `PC=0x08000300`, and zero `CFSR/HFSR`.

Physical power-cycle result 2026-08-31: Fail / Blocked, 0/5 confirmed user-flash boots. The user disconnected and reconnected USB power five times. After the fifth cycle, the exact CP210x identity enumerated on `COM5`, but UART returned no bytes. A non-resetting SWD snapshot found `PC=0x1FFF3B08` in system ROM and no SRAM heartbeat. Pressing and releasing S1 did not restore UART; a second SWD snapshot remained in system ROM at `PC=0x1FFF3B24`. Next gate: measure STM32 `NRST` directly. It must be near `CTRL_3V3` with S1 released and near 0 V while S1 is held; then check S1-to-NRST continuity with power removed.

NRST follow-up 2026-08-31: Pass. The reset node identified as STM32 pin 7 / `PG10` measured approximately 3.2 V with S1 released and 0.2 V while S1 was pressed. This proves the button drives the MCU reset node low. The remaining physical boot failure is therefore routed to BOOT0 behavior at release/power-on, not an open reset-button path.

BOOT0 option-byte correction 2026-08-31: Before the persistent write, target identity was reverified as ST-LINK `B55B5A1A0000000020AFF501`, 3.23-3.24 V, Device ID `0x469`, STM32G47x/G48x/G414. Only `nSWBOOT0` was changed from `1` to `0`; `nBOOT0=1` and all other option bytes were retained. Independent readback confirmed the new state. A hardware reset then executed at `PC=0x080002E0` with SRAM magic `0x53523030` and a live heartbeat. H2 remains open pending a true cold-power cycle without debugger-assisted reset.

Post-correction cold-power result 2026-08-31: Pass 1/5. The user removed board power and restarted from USB. Without an SWD reset or launch, `COM5` returned `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. This confirms user-flash firmware ran from the zero-power start and the three power-control outputs remained safely low. Four additional cold-power cycles remain for the 5/5 H2 gate.

H2 closure 2026-08-31: Pass. Four additional board-USB power cycles completed without failure, bringing cold-power startup to 5/5. Five S1 press/release cycles also completed without failure. A COM5-only query after the final cycle returned `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`, confirming application execution and safe inactive control outputs without SWD assistance.

Next-revision requirement: add a populated 10 kOhm resistor footprint from MCU BOOT0 to `PWR_NEG` and an accessible BOOT0 test point. Treat the option-byte setting as defense in depth, not the sole hardware boot-state control.

## 8. Stage H3 - U6 Static Power and VCC2 Gate

This is a static IC bring-up only. It authorizes no switching. D8 has been removed from `U6_VCC2` and does not gate this stage.

### H3.1 Pre-power review

Before applying any U6 input source, record from the fitted board and LM51772 datasheet:

| Gate | Required record |
|---|---|
| U6 marking/orientation | `LM51772 / TI 4AB / C9EP G4`; package dot aligned with PCB pin-1 marker |
| U6 VIN operating range | Repository migration record: 0 V with external bias to 55 V; exact datasheet conditions still require direct table review |
| Existing EN/UVLO divider and predicted threshold | Live Rev 0: R23 = 75 kOhm high side, R61 = 20 kOhm and R60 = 1.5 kOhm low side. TI Rev. A gives 1.23-1.27 V rising threshold and 4-6 uA hysteresis sink; typical predicted VIN turn-on is about 5.99 V. Bench EN was 0.804 V at 5.993 V VIN. |
| Maximum safe source voltage for every connected fitted part |  |
| Chosen source voltage | 6.0 V first static point |
| Chosen current limit | 20 mA |
| Expected VCC2 voltage and tolerance |  |
| Confirm M1-M4 absent and output ports open | Pass; M1-M4 unpopulated and all external sources/loads disconnected |
| Confirm `PA8`, `PB5`, `PA5` low | Pass; COM5 status immediately before PV application |

Do not proceed until every row is filled and the source setting is inside all verified limits.

### H3.2 Static measurements

1. Keep USB/debug ground referenced to `PWR_NEG`; prevent two supplies from backfeeding each other.
2. Apply a current-limited source to the intended `PV_IN_POS` entry at the predeclared low-energy setting.
3. Record input current and inspect U6 thermally.
4. Measure U6 VIN pin 36, BIAS pin 40, RST pin 38, VCC1 pin 1, VCC2 pin 29, FLT pin 11, EN/UVLO pin 37, AGND pin 17, and PGND pin 28 relative to the correct return.
5. Confirm HO1, LO1, HO2, and LO2 show no switching waveform.
6. Remove the U6 input source after measurements and confirm stored voltages decay safely.

Pass:

- No current-limit event or abnormal heating.
- VIN, BIAS, reset, VCC1, VCC2, EN/UVLO, and FLT agree with the datasheet and fitted strap network.
- VCC2 is stable with no external load.
- All four gate outputs remain inactive.

Record:

| Node | Expected | Measured |
|---|---:|---:|
| U6 VIN pin 36 | 6.0 V source setting | 5.993 V at `PV_IN_POS`; VIN and BIAS share this net |
| U6 BIAS pin 40 |  |  |
| U6 RST pin 38 |  |  |
| U6 VCC1 pin 1 | Datasheet expectation pending | 3.998 V |
| U6 VCC2 pin 29 | Datasheet expectation pending; not yet accepted for D8 | 3.5921 V |
| U6 FLT pin 11 | Pulled up to `CTRL_3V3` when inactive | 0.023 V; asserted low |
| U6 EN/UVLO pin 37 | Approximately 1.25 V near the datasheet-grounded predicted turn-on point of 5.99 V VIN | 0.804 V; discrepancy under investigation |
| Input current | Below 20 mA limit | Supply displayed 0 mA |
| U6 temperature | No abnormal heating | Pass by observation over 5-10 seconds |

H3 first static point 2026-08-31: Partial. The 6.0 V, 20 mA application was stable with no current-limit event or abnormal heating. RST measured 3.299 V. VCC1 and VCC2 were present, but FLT remained asserted and EN/UVLO did not match the datasheet-grounded divider calculation. The PV source was switched off. Live-board review corrected the lower divider references to R61 = 20 kOhm in series with R60 = 1.5 kOhm; R60 measured 1.48 kOhm and the in-circuit total measured about 18 kOhm because U6 provides a parallel semiconductor path.

TI LM51772-Q1 Rev. A Section 7.3.7.2 explicitly states, "Do not connect an external load to the VCC2-PIN." D8 has been isolated from `U6_VCC2`, measuring 300 kOhm from D8 VDD to U6 pin 29, so U6 static characterization can resume. Next point: 6.5 V with a 20 mA current limit; measure EN/UVLO, VCC1, VCC2, FLT, input current, temperature, and confirm no gate-output switching.

### H3A - Required D8 Isolation Rework Gate

PCB evidence: D8 pad 2 is at KiCad coordinate `(214.90, 127.27)`. Its only copper departure is a 0.20 mm F.Cu trace running left to `(182.77, 127.27)` before reaching the VCC2 via network. Cutting this dedicated branch is preferred over heating and lifting a pad on the 1.6 mm LED.

1. Keep PV off. Disconnect USB and every external lead; verify `PV_IN_POS`, `VBUS`, `CTRL_3V3`, and D8 VDD are below 0.1 V.
2. Identify D8 pad 2 electrically by continuity to U6 pin 29. Do not rely on visual package orientation alone.
3. Under magnification, make two cuts through only the 0.20 mm top-layer trace leaving D8 pad 2. Remove a 0.3-0.5 mm copper sliver while retaining a 1-2 mm pad-side stub for the temporary supply wire. Do not disturb D8 pads 1, 3, or 4.
4. Inspect for copper burrs. Confirm D8 pad 2 to U6 pin 29 is open circuit; confirm U6 pin 29 remains connected to its VCC2 decoupling capacitors.
5. Confirm D8 pad 2 is not shorted to D8 pad 3 / `PWR_NEG`, pad 4 / DIN, or pad 1 / DOUT.
6. Confirm D8 pad 2 now has continuity to USB `VBUS`. Record close-up before/after photographs and resistance results. First resume U6 static characterization with USB disconnected so D8 and VBUS are unpowered.

Pass: D8 pad 2 is isolated from U6 VCC2, connected to `VBUS`, and has no adjacent-pad short, while the U6 VCC2 local network remains intact.

Record: D8 pad 2 to U6 pin 29 before ___ Ohm; after ___ Ohm; pad 2 to `VBUS` ___ Ohm; pad 2 to ground ___ Ohm; U6 pin 29 to VCC2 capacitor ___ Ohm; inspection ___

Rework status 2026-08-31: D8 VDD was moved to USB `VBUS`. Power-off checks measured 300 kOhm from D8 pad 2 to U6 pin 29 and 0.4 Ohm from D8 pad 2 to `VBUS`; D8 pad 2 was reported not shorted to pads 1 or 4. U6 pin 29 retained continuity to both C29 and C45. D8 pad 2-to-ground resistance was not measured, and the local D8 100 nF capacitor is not fitted; retain these as noncritical Rev 0 risks and correct them in Rev 1. USB operation showed no heat, smell, unstable USB/COM5, or unexpected control-output behavior.

## 9. Stage H4 - D8 Hardware Heartbeat

D8 is an addressable RGB LED. A static high/low test on PB2 is not a valid functional test.

Status 2026-08-31: Pass. D8 VDD was moved from prohibited `U6_VCC2` to USB `VBUS`. The original compiler-dependent sender produced about 667 kHz and did not update the LED. It was replaced with a fixed-cycle Thumb-2 sender producing the required 800 kHz stream at the 16 MHz MCU clock. Firmware now transmits an automatic low-brightness RGB heartbeat on PB2: red, green, and blue pulses approximately 250 ms on and 750 ms off. The image was built, programmed, verified, and reset on the exact expected target; COM5 returned `STATUS OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. Visual bench confirmation found the RGB sequence correct and the brightness appropriate.

1. Complete and pass H3A. Keep the PV source and every external 5 V source disconnected throughout H4; USB must be the only VBUS source.
2. With all power removed, verify continuity from STM32 `PB2` to D8 DIN pin 4 and verify DIN is not shorted to VDD or ground.
3. Attach a 100 nF ceramic capacitor directly between D8 pad 2 and pad 3.
4. Use test firmware that keeps every power-control output low, configures PB2 low at startup, and drives only PB2 with D8-compatible timing.
5. Connect the already verified USB cable. Stop for failed enumeration, unstable voltage, unexpected heating, or materially increased USB current. Measure `VBUS`, D8 VDD at pad 2, `CTRL_3V3`, and D8 ground at pad 3.
6. Scope D8 DIN at pin 4. Record logic-low, logic-high, bit timing, reset-low interval, and update rate.
7. Command a low-brightness, low-duty heartbeat using red, green, and blue separately, then a repeating single-color heartbeat.
8. Compare USB input current with D8 off and at each low-brightness color. Disconnect USB before altering any D8 rework connection.

Pass:

- Correct D8 supply and ground.
- USB remains stable and D8 current increase is consistent with the commanded low brightness.
- PB2 waveform reaches DIN with valid amplitude and timing.
- Red, green, and blue each illuminate on command.
- Repeating heartbeat runs for five minutes without reset, flicker, heating, or control-output activity.

Record: D8 VDD ___ V; DIN high ___ V; bit timing ___; reset low ___; off current ___ mA; lit current ___ mA; five-minute result ___

## 10. Stage H5 - Shared I2C Electrical Test

U6, U7, and U8 share the bus. Treat a stuck line as a hardware fault to isolate, not as a reason to increase drive strength.

1. Remove the U6 PV-input source unless the exact U6 datasheet confirms its I2C pins cannot back-power or clamp the bus while U6 is unpowered. Record the chosen state.
2. Keep U7/U8 powered from stable `CTRL_3V3`.
3. Confirm R44 and R45 are fitted and record their values.
4. Measure idle voltage at STM32 pins and at U6/U7/U8:
   - PB6 / `I2C DATA` / SDA.
   - PB7 / `I2C CLK` / SCL.
5. Run `firmware/scripts/rev0-openocd-i2c-gpio-check.ps1` only if doing the existing volatile line test; otherwise use safe firmware software I2C.
6. Scope SDA and SCL during `i2cscan`. Confirm open-drain behavior, clean low levels, released-high levels, and clock activity.
7. Record all acknowledged 7-bit addresses. Do not assume device identity from address alone.

Pass:

- SDA and SCL both idle high and can each be pulled low and released.
- No device holds either line low.
- Waveforms reach every device without excessive ringing or a slow rise that violates the selected bus rate.

Record: R44 ___; R45 ___; SDA idle ___ V; SCL idle ___ V; bus rate ___; SDA rise ___; SCL rise ___; addresses ___

H5 result 2026-08-31: Pass at the address/logic level. With PV off and USB powering `CTRL_3V3`, the Rev 0 software-I2C driver used PB6 as SDA and PB7 as SCL in open-drain mode. R44 and R45 are each 4.7 kOhm to `CTRL_3V3`. The COM5 `i2cscan` returned exactly `0x38` and `0x3C`, matching the fitted AHT20 and SSD1306 candidates. Scope rise-time measurements remain optional characterization and do not block low-speed functional testing.

## 11. Stage H6 - U7/AHT20

1. Measure U7 VDD pin 2 and GND pin 5 at the package.
2. Confirm SDA at pin 4 and SCL at pin 3 have continuity to the shared bus.
3. Verify the expected AHT20 7-bit address from its datasheet, then match it to the scan result.
4. Send the documented initialization/status transaction and record the response bytes.
5. Read temperature and humidity repeatedly for five minutes.
6. Apply a gentle, non-condensing environmental change and confirm the readings move in the expected direction.

Pass:

- Correct rail and ground, valid acknowledgment, valid status, and plausible changing measurements.
- No bus lockup or self-heating trend that indicates a hardware fault.

Record: VDD ___ V; address ___; status bytes ___; initial T/RH ___; final T/RH ___; result ___

H6 initial functional result 2026-08-31: Pass. The `aht20` command triggered a measurement at address `0x38` and returned status `0x18` (calibrated and not busy), temperature 26.7 C, and humidity 39.9% RH. The preceding status query confirmed `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. Extended environmental-response and five-minute stability characterization remain future coverage, not a blocker for initial hardware functionality.

## 12. Stage H7 - U8/SSD1306 OLED

1. Measure U8 VCC pin 2 and GND pin 1 at the module.
2. Confirm SDA at pin 4 and SCL at pin 3 have continuity to the shared bus.
3. Determine the fitted module's actual 7-bit address from the scan; do not assume `0x3C` or `0x3D` without evidence.
4. Send the SSD1306 initialization sequence appropriate to the fitted 128x64 module.
5. Display full-off, full-on briefly at low contrast, vertical stripes, horizontal stripes, checkerboard, and a one-pixel border.
6. Inspect for missing rows/columns, unstable contrast, random pixels, bus errors, or resets.
7. Run a low-contrast changing test pattern for five minutes while monitoring `CTRL_3V3` and current.

Pass:

- Correct rail and ground, valid acknowledgment, stable initialization, and all test patterns render correctly.
- No missing lines, random corruption, rail instability, or bus lockup.

Record: VCC ___ V; address ___; off current ___ mA; active current ___ mA; pattern results ___; five-minute result ___

H7 initial functionality result 2026-08-31: Pass. The `oled test` command initialized address `0x3C` at low contrast, enabled the SSD1306 charge pump and horizontal addressing, and transferred a full 1024-byte checkerboard without a NACK. Firmware returned `SSD1306 checkerboard drawn`, and the checkerboard was visually confirmed on the panel. A subsequent AHT20 transaction still passed with status `0x18`, 26.7 C, and 39.5% RH, confirming that the shared bus remained operational after the display transfer.

## 13. Stage H8 - Integrated Low-Energy Run

Only enter after H1-H7 pass individually.

Overnight setup 2026-08-31: Active. Firmware automatically initializes the OLED and displays four rows:

- `TIME HH:MM:SS`: software wall clock initialized from the PC with `clock HH:MM:SS`.
- `UP HHH:MM:SS`: uptime since the most recent MCU reset.
- `T xx.xC Hxxx.x%`: latest AHT20 temperature and relative humidity.
- `Nnnnnn Ennn`: successful AHT20 sample count and cumulative I2C/display error count.

The AHT20 is sampled every 10 seconds and the OLED refreshes every second. D8 continues the accepted RGB heartbeat. The wall clock uses the 16 MHz MCU clock and is approximate; uptime and counters, not wall-clock precision, are the stability evidence. The dashboard clock was set to `07:44:44`. UART remained responsive during automatic OLED transfers, AHT20 returned status `0x18`, 27.1 C, and 39.5% RH, and COM5 reported `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. Keep PV, battery, output loads, inverter, and AC disconnected for the entire run.

1. Start by normal reset, not manual SWD launch.
2. Run D8 at low-brightness heartbeat.
3. Read AHT20 at a modest interval and update the OLED without saturating the I2C bus.
4. Keep `PA8`, `PB5`, and `PA5` low and monitor them during the run.
5. Run for 15 minutes while recording supply current, `CTRL_3V3`, `U6_VCC2`, temperatures, I2C errors, UART errors, and reset count.

Pass:

- No resets, bus lockups, display corruption, missed heartbeat, abnormal heating, or active control output.

## 14. Stop, Fault Isolation, and Closeout

When a stage fails:

1. Stop at that stage; do not skip ahead.
2. Force safe idle through SWD if communication remains available.
3. Remove the U6/PV-input source first, then control power.
4. Verify stored voltages have discharged.
5. Record the exact failing measurement, source settings, waveform, photos, and last passing stage.
6. Change one hardware variable at a time before repeating the failed stage.

At normal closeout:

- [ ] `PA8`, `PB5`, and `PA5` confirmed low.
- [ ] U6 input source removed.
- [ ] VBUS/control power removed.
- [ ] Stored voltages discharged.
- [ ] Board inspected for heating or damage.
- [ ] Evidence files and firmware hash archived.
- [ ] Rework recorded with before/after continuity readings and photos.

## 15. Session Result

| Stage | Result | Evidence / key measurements |
|---|---|---|
| H0 Safe idle | Pass / Fail / Blocked |  |
| H1 USB debug | Pass / Fail / Bypassed / Blocked |  |
| H2 Reset / BOOT0 | Pass | 5/5 cold-power and 5/5 S1 reset cycles; final COM5 status valid with all control outputs low |
| H3 U6 static / VCC2 | Pass / Fail / Blocked |  |
| H4 D8 heartbeat | Pass | Fixed-cycle 800 kHz RGB heartbeat visually confirmed; brightness and cadence accepted; safe control outputs verified |
| H5 I2C electrical | Pass / Fail / Blocked |  |
| H6 AHT20 | Pass / Fail / Blocked |  |
| H7 SSD1306 | Pass / Fail / Blocked |  |
| H8 Integrated run | Pass / Fail / Not attempted |  |

Next authorized stage after this document: none. U6 converter switching, charger-path energization, battery connection, real PV connection, and inverter/AC work each require a separate reviewed entry plan.
