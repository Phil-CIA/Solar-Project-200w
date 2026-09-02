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

The 6.5 V / 20 mA point was completed with no current limit, heating, odor, or instability. Correction recorded 2026-09-01: the source display and J1 measurement (`6.50 V`, approximately `6.49 V`) were at `PV_IN_POS_RAW`, not at U6's `PV_IN_POS`. The protected rail feeding U6 was approximately `4.54 V`. Measurements relative to `PWR_NEG` were `VCC1=4.5471 V`, `VCC2=4.5415 V`, `EN_UVLO=1.030 V`, `RST=3.29 V`, and `FLT=0.0296 V`. The source was turned off afterward, and the input/U6 rails decayed below 0.1 V. Gate outputs were not checked.

Do not increase the PV source above 6.5 V. The corrected `PV_IN_POS` value makes `EN_UVLO=1.030 V` broadly consistent with the documented R23/R61/R60 divider and below the LM51772 rising threshold; the prior apparent divider discrepancy is withdrawn. The active blocker is the unexpected approximately 1.95 V loss from J1 `PV_IN_POS_RAW` to U6 `PV_IN_POS` through the PV front end. Keep all sources disconnected until `PV_IN_POS_FUSED`, both sides of U2, U2 gate/source voltages, and the fitted U2 pinout/control topology are verified. Do not command `V_in_ON`/PA5 with PV present until that review establishes a safe expected response. A later powered point must record `PV_IN_POS_RAW`, `PV_IN_POS_FUSED`, and `PV_IN_POS` separately and include a no-switching check on HO1, LO1, HO2, and LO2.

U2 review 2026-09-01: the fitted BSC112N06LD is an N+N device used as a common-source back-to-back NMOS switch. The Rev 0 gate network cannot produce a valid ON state. With safe firmware holding `V_in_ON`/PA5 low, Q2 is off and R43 pulls the U2 gates only toward `PV_IN_POS_FUSED`; the floating common source therefore has no gate overdrive above the input rail. Driving PA5 high would turn Q2 on and pull the U2 gates low, which is also an OFF command for the high-side NMOS pair. This agrees with the earlier SPICE conclusion that the U2/Q2 topology does not establish useful VGS and can explain the reduced protected-rail voltage.

Next bounded diagnostic: keep USB control power unchanged and PA5/PA8/PB5 low. Apply only the previously tolerated 6.5 V / 20 mA source at J1. Relative to `PWR_NEG`, record `PV_IN_POS_RAW`, `PV_IN_POS_FUSED`, `PV_IN_POS`, U2 gate at D1 pin 1 / R42 side, and U2 common source at D1 pin 2. Use D1's accessible pads rather than probing U2. Record source current and temperature, then turn the source off and confirm discharge below 0.1 V. Do not pulse PA5. These readings separate fuse/contact loss from the expected U2 gate-drive failure; they do not authorize a higher-voltage U6 point.

U2 diagnostic result 2026-09-01: confirmed gate-drive failure. At a 6.5 V / 20 mA source setting, `PV_IN_POS_RAW=6.492 V`, `PV_IN_POS_FUSED=6.492 V`, and `PV_IN_POS=4.543 V`; source current was 416 uA. The U2 gate measured 6.42 V relative to `PWR_NEG`, and D1 measured 1.92 V from anode to cathode across the U2 common-source/gate network. These values imply approximately 1.92 V positive U2 gate-to-common-source bias and a common-source voltage near 4.50 V, matching the protected output. U2 is self-biased near its threshold rather than enhanced. F1 was intentionally jumpered for current-limited bench testing, so the zero raw-to-fused drop does not qualify the fuse or its contacts.

The U2 diagnosis is closed. Turn the PV source off and confirm `PV_IN_POS_RAW`, `PV_IN_POS_FUSED`, and `PV_IN_POS` are below 0.1 V. H3 must not continue through the fitted U2 path. The next hardware action requires a deliberate choice between a temporary fused-to-protected-rail bypass for bounded bench-only U6 testing or permanent U2/Q2 front-end rework. Any temporary bypass removes U2 reverse blocking and must be installed and removed only with every source disconnected; it does not authorize battery, panel, load, switching, or operation above the separately approved low-energy point.

Power-off status: USB, ST-LINK, and PV leads were disconnected; `VBUS`, `CTRL_3V3`, `PV_IN_POS_FUSED`, and `PV_IN_POS` were confirmed below 0.1 V. Only the pre-existing F1 test jumper is installed. No U2 bypass is installed; the measured approximately 195 kOhm from the fused rail to TP4 confirms U2 remains in circuit. Do not reapply power until a separate insulated `PV_IN_POS_FUSED`-to-TP4 bypass is installed and verifies near lead resistance.

Bypass update 2026-09-01: temporary jumpers were installed around both U2 and U5. The U2 bypass makes TP4 / `PV_IN_POS` equal to J1 `PV_IN_POS_RAW` at the bounded bench point. The U5 bypass addresses the same invalid common-source NMOS gate-drive topology on the output path, but does not authorize an output source, battery, or load. Both bypasses defeat their original blocking functions and are bench-only rework.

First bypassed U6 point: with the input held at 6.49 V and the 20 mA source limit unchanged, `_Undervoltage Feedback` / U6 `EN_UVLO` measured 1.43 V, above the 1.23-1.27 V rising threshold. Supply indication was approximately 1 mA; an inline meter measured 2.1 mA and is the conservative recorded value. Relative to `PWR_NEG`, `VCC1=5.0375 V`, `VCC2=4.9706 V`, `FLT=0.02 V`, and `RST=3.29 V`. U6 has crossed UVLO and its VCC rails are near 5 V, but `FLT` remains asserted low. Do not increase the input. Thermal behavior and static HO1/LO1/HO2/LO2 state remain required before this point can pass.

H3 bounded static closure: U6 and both bypass jumpers remained cool with no odor, instability, or current limiting. DMM measurements at the unpopulated MOSFET footprints found M2 gate-to-source / LO1, M3 gate-to-source / LO2, M1 gate-to-source / HO1-to-SW1, and M4 gate-to-source / HO2-to-SW2 all at 0 V. The 6.49 V enabled static point passes for no-heating and no-gate-activity behavior. `FLT=0.02 V` remains asserted and requires explanation before switching. Do not increase voltage or install M1-M4. The temporary F1, U2, and U5 bypasses prohibit panel, battery, output source/load, unattended operation, or any higher-energy test.

Powered-point closeout: the PV source was switched off, and `PV_IN_POS_RAW`, `PV_IN_POS_FUSED`, TP4 / `PV_IN_POS`, `VCC1`, and `VCC2` were each confirmed below 0.1 V. Leave the board USB-only. The next H3 action is an FLT-assertion diagnosis at the same or lower bounded energy; no higher input point is authorized.

U6 diagnostic-interface finding 2026-09-01: fitted R33 is 6.49 kOhm from U6 `ADDR_SLOPE_CFG1` to PGND. This selects row 8 of LM51772-Q1 Table 7-4: slope setting 1.0 and I2C disabled. Therefore the fitted Rev 0 configuration cannot expose `STATUS_BYTE`; an I2C scan must not be used to infer a missing or damaged U6 while R33 is fitted. The datasheet-defined diagnostic configuration is `ADDR_SLOPE_CFG1=GND`, which selects I2C address `0x6A` and default slope 0.875.

Read-only diagnostic firmware was added and verified. Command `u6 status` performs the datasheet single-register-read transaction for `STATUS_BYTE` at `0x78` and decodes `BUSY`, `OFF`, `VOUT`, `IOUT`, `INPUT`, `TEMPERATURE`, `CML`, and `OTHER`; it never accesses `CLEAR_FAULTS` at `0x03` and makes no register writes. Build passed with warnings as errors. Before flashing, ST-LINK `B55B5A1A0000000020AFF501`, 3.24 V, device `0x469`, STM32G47x/G48x/G414, and 512 KB NVM were verified. Programming and byte verification passed. With PV off, COM5 returned safe low output states and the expected `U6 STATUS_BYTE read failed at I2C address 0x6A` because R33 still disables I2C.

R33 diagnostic result: R33 was removed and replaced by a zero-ohm link; continuity across its pads measured 0 ohms and visual inspection found no adjacent-pad bridge. With U6 powered at the unchanged bounded 6.49 V / 20 mA-limit point, VCC2 measured 4.97 V but U6 pin 9 measured 0.63 V relative to `PWR_NEG`. A correctly paced powered `i2cscan` found only the established `0x38` and `0x3C` devices, and `u6 status` received no acknowledgement at `0x6A`.

Critical correction and rework: R33 pad 2 is `PGND`, not `PWR_NEG`. The exported netlist and PCB define these as separate nets and contain no original joining component. A jumper was installed from R31 pad 2 (`PGND`) to R32 pad 2 (`PWR_NEG`). Netlist and PCB review confirms these are the return-side pads; R31 pad 1 remains on the compensation network and R32 pad 1 remains on `ILIMCOMP_ISET`. Unpowered checks confirmed the resistors remained in circuit, the intended PGND-to-PWR_NEG short was present, and U6 pin 9 remained connected to PGND through the zero-ohm R33 replacement.

Post-rework powered result: MCU safety outputs remained low. At the unchanged 6.49 V / 20 mA-limit point the source display showed 0 mA at its available resolution, the board remained cool and stable, and U6 pin 9 measured 0.0001 V relative to PWR_NEG. This confirms the PGND return repair and valid Table 7-4 low address strap. Nevertheless, a correctly paced powered scan still found only `0x38` and `0x3C`; U6 did not acknowledge at `0x6A` or any other address. The shared bus and strap are now demonstrated, leaving U6 pins 5/6 physical attachment, fitted-device identity, or local U6 soldering as the remaining I2C blocker. PV was switched off and the user confirmed no backfeed. Do not infer a STATUS_BYTE value or clear any fault.

U6 pin-area inspection follow-up: debris was removed, but no visible short or bus-affecting defect was found. Unpowered continuity and isolation checks at U6 pins 5 and 6 passed, and their solder joints were reworked. The first post-rework scan was mistakenly performed USB-only with PV off; it found `0x38` and `0x3C` but was not a valid U6 test. PV was then explicitly restored at the bounded point. A second scan with U6 powered and `VCC2=4.97 V` again found only `0x38` and `0x3C`; U6 remained absent at `0x6A`. PV was switched off after the measurements. The expected `LM51772 / TI 4AB / C9EP G4` marking and pin-1 orientation were reconfirmed from the prior entry. The remaining discriminating check is dynamic SCL/SDA activity directly at U6 pins 6/5 during a scan.

Full-address scan follow-up 2026-09-01: firmware command `i2cscan full` was added to report ACK or NACK for every legal 7-bit address from `0x08` through `0x77`; protocol-reserved addresses remain excluded. The image built with warnings as errors, and the expected ST-LINK `B55B5A1A0000000020AFF501`, 3.23-3.24 V target, device ID `0x469`, STM32G47x/G48x/G414 identity, and 512 KB NVM were verified before programming. Download and byte verification passed. A USB-only command check and a separate valid scan with PV restored at the bounded 6.49 V / 20 mA-limit point both found exactly two devices: `0x38` and `0x3C`. During the powered scan every other legal address NACKed, including `0x6A` and `0x6B`. Safety outputs were low immediately beforehand. No register was accessed. PV was switched off after the scan. This rules out U6 responding at another legal 7-bit address; dynamic waveform arrival at U6 pins 5/6 remains the next discriminating test.

GitHub research follow-up 2026-09-02: three public repositories were found for the part. `karu2003/LM51772_py` defaults to 7-bit address `0x6A` and uses ordinary SMBus register transactions. `DavidTobar456/LM51772Control` defines only `0x6A` and `0x6B` and polls readiness with an address-level quick-write probe equivalent to the local scanner's ACK test. `toxCat/TI_LM51772_SPICE` contains simulation material but no I2C diagnostic path. No public issue or discussion describing a special wake-up, reset workaround, or alternate address was found. Some C repository examples use unrelated test address `0x50` and contain unfinished code, so they are not treated as design authority; the useful address and probe behavior nevertheless agree with the TI datasheet and local implementation. This search produced no software-side explanation for U6's NACK and does not change the next dynamic pin-level waveform test.

Analyzer-oriented scan follow-up 2026-09-02: firmware command `i2cscan unknown repeat` was added and flashed after exact target verification and successful byte readback. It automatically suppresses periodic OLED/AHT20 traffic, performs four address-only sweeps over legal addresses `0x08` through `0x77`, skips the known devices at `0x38` and `0x3C`, and sends no register byte. With PV bounded at 6.49 V maximum and the 20 mA limit, the board cool/stable, and safety outputs low, the command generated 440 probes and received zero ACKs. The user monitored U6 pin 6/SCL and saved the analyzer capture. PV was switched off immediately afterward. This confirms that no unknown legal address, including U6, responded during repeated bus activity; it does not by itself replace inspection of SDA on the ninth clock at U6 pin 5.

Read-only register attempt 2026-09-02: with automatic I2C traffic stopped, safety outputs low, PV bounded at 6.49 V maximum / 20 mA, and the analyzer armed, one `u6 status` command attempted the datasheet `STATUS_BYTE` read at register `0x78` and 7-bit address `0x6A`. Firmware returned `U6 STATUS_BYTE read failed at I2C address 0x6A`; no status value was obtained. The helper stops immediately on a NACK before reading data, never accesses `CLEAR_FAULTS` at `0x03`, and performs no configuration write. The analyzer capture was saved and PV was switched off. Decode the capture to identify whether the initial address-write, register pointer, or repeated-start address-read phase failed; prior address probes make initial address NACK the expected result.

Decisive analyzer decode 2026-09-02: the saved capture shows `Setup Write to [0xD4] + NAK`. KingsVIS displays the raw 8-bit address byte, so `0xD4` is 7-bit U6 address `0x6A` with the write bit. The capture shows nine clean SCL pulses at physical U6 pin 6 and the expected address data at physical U6 pin 5; SDA remains released high during the ninth SCL-high period. U6 therefore physically receives the address transaction but does not acknowledge it. Because the initial address phase NACKs, register pointer `0x78`, the repeated START, and the read phase are never sent. This closes scanner implementation, alternate address, and signal-delivery timing as explanations. With power, low address strap, grounding, identity/orientation, static continuity, solder rework, and pin-level bus arrival already verified, the remaining blocker is local U6 device or package/die-to-pad assembly failure. Do not repeat software scans or attempt register writes; PV remains off.

Replacement U6 result 2026-09-02: U6 was replaced. Post-replacement orientation, R33 zero-ohm link, PGND-to-PWR_NEG jumper, and unpowered short checks passed. At the bounded input reduced to 6.49 V maximum with the 20 mA limit, source current was approximately 3 mA, the board remained cool/stable with no current limiting or unexpected gate activity, `VCC2=4.95 V`, and U6 pin 9 measured 0.009 V relative to PWR_NEG. Firmware then stopped automatic I2C traffic and confirmed `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. One address-only `0x6A` probe still returned `U6 0x6A NACK`. A repeated single probe with the analyzer connected directly to replacement-U6 pin 5 SDA and pin 6 SCL independently confirmed the result: the nine SCL-high samples show address byte `11010100` (`0xD4`, or 7-bit `0x6A` write), and SDA remains high during the ninth clock for NACK before the STOP. PV was switched off afterward. The identical pin-level failure with a replacement device falsifies the prior single-device/die-attachment diagnosis and shifts the blocker to a persistent board-level U6 condition. Do not replace U6 again or repeat scans. Next checks should verify powered `RST` at pin 38 and ground/reference integrity at AGND pin 17, PGND pin 28, and the exposed pad before another address probe.

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
