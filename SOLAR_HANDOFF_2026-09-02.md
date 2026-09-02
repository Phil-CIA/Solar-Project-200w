# Solar Project 200W - Rev 0 U6 Handoff (2026-09-02)

## Stop State

PV is off. The replacement U6 is installed. Do not repeat I2C scans, replace U6 again, write LM51772 registers, or access `CLEAR_FAULTS` at `0x03`.

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

## Worktree Note

The staged edits to `SOLAR_HANDOFF_2026-07-20.md` and `hardware/kicad/solar-project/Solar Project Rev1.kicad_pcb` predate this U6 work. Preserve them and keep them out of the Rev 0 U6 commit.