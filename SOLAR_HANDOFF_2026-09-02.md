# Solar Project 200W - Rev 0 U6 Handoff (2026-09-02)

## Stop State

PV is off. The replacement U6 is installed. Do not repeat I2C scans, replace U6 again, write LM51772 registers, or access `CLEAR_FAULTS` at `0x03`.

> **Correction recorded 2026-09-03:** The earlier zero-ohm rework was performed on R28 (`RT`), not R33 (`ADDR_SLOPE_CFG1`). R29 (`CFG2`) remained at its intended 20.5 kOhm. R33 remained 6.49 kOhm and therefore disabled I2C during every NACK result documented below. After R33 was actually changed to zero ohms, one address-only `u6 probe` returned `U6 0x6A ACK`. The prior board-level I2C-failure diagnosis is withdrawn. See the final correction section for the updated state.

The board remains modified for low-energy bench diagnosis:

- F1, U2, and U5 protection functions are bypassed.
- R33 is replaced by a zero-ohm link.
- Rev 0 `PGND` is joined to `PWR_NEG` from R31 pad 2 to R32 pad 2.
- M1-M4 are absent.
- No battery, output source/load, panel, inverter, AC, or switching is authorized.

## Decisive Result

The original U6 physically received raw I2C address byte `0xD4` at pins 5/6 and NACKed. U6 was replaced to test the remaining local-device hypothesis.

At the bounded replacement-U6 test point:

- PV source: 6.49 V maximum, 20 mA current limit.
- Source current: approximately 3 mA.
- `VCC2`: 4.95 V.
- Pin 9 `ADDR_SLOPE_CFG1`: 0.009 V relative to `PWR_NEG`.
- Firmware safety state: `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`.
- Board: cool and stable, with no current limiting or unexpected gate activity.

One address-only `u6 probe` returned `U6 0x6A NACK`. A logic-analyzer capture connected directly to replacement-U6 pin 5 SDA and pin 6 SCL showed:

- Eight SDA address samples: `11010100`, raw byte `0xD4`.
- Seven-bit address: `0x6A`, write direction.
- SDA high during the ninth SCL-high period: NACK.
- A valid STOP after the NACK.

The replacement package therefore receives the correct address transaction at its physical pins but does not acknowledge it. This independently closes firmware scanning, address encoding, bus routing, analyzer channel assignment, and a one-off damaged original U6 as explanations.

## New Observation (post-session, unverified timing)

On replacement U6, PV source current was seen flashing/fluctuating between approximately 3 mA and 4 mA immediately after power-up, before any I2C transaction. The operator's recollection is that current dropped to near-zero at or around the time of the first I2C write/probe attempt, though exact timing relative to the transaction is not confirmed.

Current has not recovered since:

- PV was power-cycled (fully removed and reapplied) after the drop.
- Current remained low after the power cycle.
- Current reading at time of this note: approximately 1.3 mA.

This is significant because a power cycle should clear a simple internal fault-latch condition if U6's bias/LDO shutdown were caused only by an I2C-triggered internal fault. Persistence through a power cycle suggests either:

1. A genuine persistent fault-latch requiring an explicit register-level clear (which is intentionally not being attempted per the stop-state rules), or
2. A hardware-level change coincident with the rework/probe session (e.g., a disturbed connection, solder joint, or component condition) that reduced quiescent current draw independent of I2C activity, or
3. A measurement/test-point change (source, cabling, or current-limit setting) that is unrelated to U6 internal state.

This has not yet been reconciled with the RST/AGND/PGND/exposed-pad measurement plan below. Both should be pursued together: confirm whether current draw at ~3-4 mA vs ~1.3 mA correlates with any voltage change at pins 38/17/28/exposed pad, and whether the timing of the current drop can be pinned down more precisely (e.g., against firmware log timestamps or scope/analyzer capture, if available) before ruling in or out an internal fault latch.

## Leading Diagnosis

The common cause is now a persistent board-level startup, reset, ground/reference, configuration-latch, or package-pad condition. The next powered operation must be voltage/reference measurements only, before any further I2C command.

At the same bounded source point, measure relative to `PWR_NEG`:

1. U6 pin 38 `RST`.
2. U6 pin 17 `AGND`.
3. U6 pin 28 `PGND`.
4. An accessible exposed-pad/thermal-via ground point.

Then reassess startup sampling of pin 5 `SDA_CFG3` and pin 6 `SCL_CFG4`, including whether always-powered 3.3 V bus pull-ups establish an unintended state before U6 `VCC2` rises. Confirm datasheet behavior before isolating pull-ups or performing more rework.

## Safety Limits

- PV: 6.49 V maximum with 20 mA current limit.
- Keep `PA8`, `PB5`, and legacy `PA5` low.
- Never drive `PB11`; it is tied to `PWR_NEG`.
- Stop for current limiting, heating, odor, unstable rails, or unexpected gate activity.
- Use verified `COM5` at 115200 8N1, one CR-terminated command at a time.
- Before flashing, verify ST-LINK `B55B5A1A0000000020AFF501`, device ID `0x469`, STM32G47x/G48x/G414, 512 KB NVM, and expected target voltage.

## Relevant Files

- `SOLAR_HANDOFF_2026-08-31.md`: complete chronological bring-up record.
- `docs/test/rev0-low-energy-session-2026-08-30.md`: detailed measurements and test evidence.
- `docs/test/rev0-hardware-bringup-execution-plan-2026-08-30.md`: gate status and acceptance criteria.
- `firmware/bringup/baremetal/stm32g4_rev0_stage1.c`: active diagnostic firmware.
- `NEXT_CHAT_PROMPT.md`: compact continuation prompt.

## Correction and First Valid ACK (2026-09-03)

Physical rework review established that R28, not R33, had previously been changed to zero ohms. This reference-designator error invalidates the premise used for the original-device and replacement-device I2C diagnosis:

- R28 connects U6 pin 12 `RT` to `PGND` and is intended to be 51.7 kOhm, not zero ohms.
- R29 connects U6 pin 8 `CFG2` to `PGND` and remains at its intended 20.5 kOhm value.
- R33 connects U6 pin 9 `ADDR_SLOPE_CFG1` to `PGND`.
- The still-fitted R33 value was 6.49 kOhm, selecting Table 7-4 row 8 with I2C disabled.
- R33 was subsequently changed to zero ohms, selecting I2C address `0x6A` after configuration latching.

At the bounded powered point, static measurements relative to `PWR_NEG` were `nRST=3.29 V`, `AGND=0.0 V`, `PGND=0.0 V`, accessible exposed-pad copper `=0.0 V`, `VCC2=4.96 V`, `VCC1=5.06 V`, and `EN_UVLO=1.43 V`. The source current was below the bench supply display resolution and no more than approximately 1.3 mA.

One CR-terminated `u6 probe` command was sent through verified `COM5`. Firmware returned `U6 0x6A ACK`. This was an address-only transaction; no register address was sent, no register was read or written, and `CLEAR_FAULTS` was not accessed.

The earlier pin-level NACK captures are now expected behavior for the I2C-disabled 6.49 kOhm R33 configuration. They do not indicate a defective original U6, defective replacement U6, bad bus routing, reset failure, or exposed-pad failure. The LM51772 communication blocker is provisionally closed pending one controlled repeat after a documented power cycle. `FLT` remains unresolved, and no register access is yet authorized.

Before that repeat, restore R28 from the accidental zero-ohm link to its intended 51.7 kOhm `RT` value with every source removed and all rails discharged. Leave R29 at 20.5 kOhm, keep R33 at zero ohms for address `0x6A`, and retain the PGND-to-PWR_NEG jumper. Verify R28, R29, R33, and the ground jumper unpowered. If one address-only probe ACKs after a complete cold power cycle, the next bounded diagnostic is the existing read-only `u6 status` command for `STATUS_BYTE` register `0x78`; continue to prohibit all register writes and any access to `CLEAR_FAULTS` at `0x03`.

R28 was restored to 51.7 kOhm, R29 was verified at 20.5 kOhm, and R33 was verified at zero ohms. With PV applied at the existing bounded point, COM5 reported `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. The first probe command produced no captured UART text, but a non-I2C status query immediately proved UART remained responsive and safety outputs remained low. One controlled repeat with a longer receive window returned `U6 0x6A ACK`.

The authorized read-only `u6 status` command then returned `U6 STATUS_BYTE=0x58 BUSY=0 OFF=1 VOUT=0 IOUT=1 INPUT=1 TEMP=0 CML=0 OTHER=0`. TI defines `OFF` as output below power-good or converter disabled, `IOUT` as latched peak-current-limit detection, and `INPUT` as latched VDET/UVLO-below-threshold detection. No register was written and `CLEAR_FAULTS` was not accessed. Communication is now closed as a blocker. Switch PV off before further work. Do not clear faults or authorize switching; determine next whether `IOUT` repeats after a clean power cycle with R28 restored and M1-M4 still absent.

PV was then fully power-cycled with R28 restored. COM5 again reported `PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`. A second read-only `u6 status` returned the identical `STATUS_BYTE=0x58`, proving that `IOUT` relatches in the corrected configuration rather than persisting from the zero-ohm RT state. TI exposes no more detailed latched IOUT status register; `USB_PD_STATUS_0` at `0x21` reports only instantaneous constant-current operation. Remove PV and inspect the R26/R27/C20 `CSA`/`CSB` network unpowered before considering a static differential measurement. Do not clear faults or attempt switching.

Follow-up 2026-09-04: the R26/R27/C20 network passed unpowered checks. At the 6.49 V bounded point, `CSA=3.2041 V`, `CSB=3.2043 V`, and the direct reading across C20 was approximately `0.1 mV`. The balanced DC inputs rule out a sustained peak-current comparator condition. After a full discharge, PV was set to 5.0 V / 20 mA; source current was less than 1 mA and `EN_UVLO=1.0241 V`, below the rising threshold. MCU status remained `OCP=1 OVP=0 PA8_PWM=0 PB5_EN=0 PA5_LEGACY=0`.

COM5 then stopped accepting text commands. A read-only SWD diagnostic was bound to the verified ST-LINK serial `B55B5A1A0000000020AFF501`; target voltage was 3.246 V and STM32 Device ID readback was `0x469`. The MCU was halted to avoid I2C contention, PB6/PB7 state was saved, and a single software-I2C read of U6 register `0x78` returned `STATUS_BYTE=0x48`: `OFF=1 IOUT=0 INPUT=1`, with all other summary bits zero. PB6/PB7 state was restored and the MCU resumed. No flash, LM51772 register, or option byte was written, and register `0x03` was not accessed. This discriminates the earlier `0x58`: `IOUT` is latched when the 6.49 V test crosses UVLO and U6 attempts startup with M1-M4 absent, not during inhibited 5.0 V power-up and not from a sustained DC sense differential. Do not advance to switching; the bypassed U2/U5 protection paths and incomplete power stage require a separate reviewed entry plan.

## Worktree Note

The staged edits to `SOLAR_HANDOFF_2026-07-20.md` and `hardware/kicad/solar-project/Solar Project Rev1.kicad_pcb` predate this U6 work. Preserve them and keep them out of the Rev 0 U6 commit.