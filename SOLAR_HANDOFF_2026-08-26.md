# Solar Project 200W - Rev 0 Bring-Up Failure Analysis Handoff (2026-08-26)

## Session Outcome

Rev 0 bench bring-up was blocked by a persistent `CTRL_3V3`-to-`PWR_NEG` short at U1. Root cause was found and confirmed: the board was routed against the wrong MCU symbol. Parts are on order to recover the existing boards, and the Rev 1 corrective path is defined.

## Root Cause (DEC-018)

The Rev 0 board was routed against the `1My_MPUs:STM32G431CBU6` symbol while `STM32F411CEU6` parts were fitted.

During the DEC-014 part migration, only the `Value` and `MP` text fields were changed. The symbol `lib_id` was never updated, so the routed pin map is G4, not F4.

Verified against KiCad's official `MCU_ST_STM32F4` library:

| Pad | Board net | G431 (routed) | F411 (fitted) | Result |
|---|---|---|---|---|
| 23 | `CTRL_3V3` | VDD | **VSS** | 3V3 into chip ground |
| 35 | `CTRL_3V3` | VDD | **VSS** | 3V3 into chip ground |
| 24 | `PWR_NEG` | PB11 | **VDD** | Ground into chip VDD |
| 48 | `CTRL_3V3` | VDD | VDD | correct |
| 49 (EP) | `PWR_NEG` | VSS | VSS | correct |

Two independent short paths exist through the die. The approximately 0.7 ohm reading is bond-wire and internal metallization resistance. No rework could ever have fixed it.

Additional consequences of the same mismatch on F411: `VSSA` (pad 8) and `VDDA` (pad 9) unconnected, `VCAP1` (pad 22) has no capacitor, `BOOT0` (pad 44) driven by `I2C DATA`, `BOOT1` (pad 20) tied high, and `VDD` (pad 36) wired to `SWDIO`. The chip could not have run even without the short.

### Debugging Path (for traceability)

Approximately seven reflow cycles, chip removal and replacement, pad cleaning, wick, and flux work were performed before the cause was found. Cross-testing (old chip on original board, new chip on original board, old chip on a second bare board) correctly ruled out both a single bad chip and a single bad board. DEC-017 briefly and incorrectly concluded a footprint clearance defect before DEC-018 identified the symbol mismatch.

## Parts Ordered

Two `STM32G473CEU6` (UFQFPN-48). Verified pin-identical to the routed `STM32G431CBU6` map across all 49 pins, so it drops into the existing Rev 0 boards.

- Must be the `U6` suffix (UFQFPN-48 7x7). Do not substitute `CET6`/`CBT6` (LQFP-48) - it will not fit.
- Lead time approximately 5-6 days from order.
- Quantity held at two intentionally per DEC-020, because the longer-term direction is F411, not G4.

Other verified pin-compatible options if needed: `STM32G431C6U6`, `STM32G431C8U6`, `STM32G431CBU6`, `STM32G441CBU6`, `STM32G473C[B/C]U6`, `STM32G474C[B/C/E]U6`, `STM32G483CEU6`, `STM32G484CEU6`, `STM32G491C[C/E]U6`.

## Known Rev 0 Board Constraints

Carry these into firmware and bring-up:

1. **I2C is swapped (DEC-019).** `PB6` (pad 44) carries `I2C DATA` and `PB7` (pad 45) carries `I2C CLK`, but `PB6` is `I2C1_SCL` only and `PB7` is `I2C1_SDA` only. Hardware `I2C1` is unusable as routed. Use a software bit-banged I2C driver for Rev 0. A two-trace cut-and-jumper rework is the alternative.
2. **`PB11` (pad 24) is tied to `PWR_NEG`.** Never configure it as an output.
3. **`PB8`/BOOT0 (pad 46) floats.** Acceptable at default G4 option bytes (`nBOOT_SEL=1`). Do not clear `nBOOT_SEL` without adding a pulldown.
4. **UART is correct.** U3 pin names are from the bridge perspective: `PA9`/TX to bridge RXD and `PA10`/RX to bridge TXD is proper wiring.
5. **M1-M4 are not fitted** due to a separate footprint mismatch. The inverter bridge is nonfunctional and all AC/inverter testing remains blocked. See `docs/test/rev0-bringup-addendum.md`.

## Process Change

Added Section 7.2 **Part Substitution Gate (Mandatory)** to `hardware/mppt/kicad-implementation-checklist.md`.

Core rule: **the same package never implies the same pinout.** Editing `Value`/`MP` fields does not change the routed pin mapping. The gate requires replacing the actual symbol, pin-by-pin datasheet verification, explicit re-check of all power/boot/debug pins, and a fresh netlist and ERC export. It is cross-linked from the Pre-PCB Routing Gate.

## Next Session Options

Pick up on either track depending on whether parts have arrived.

### Track A - Rev 0 Recovery (parts arrived)

1. Fit one `STM32G473CEU6` to a Rev 0 board using the established hot air process.
2. Verify `CTRL_3V3`-to-`PWR_NEG` resistance is high before applying power. This is the pass/fail gate for DEC-018.
3. Run Stage A and Stage B of `docs/test/rev0-bringup-addendum.md`: unpowered checks, then current-limited `VBUS` control-rail bring-up.
4. Confirm SWD attach, reset, and a serial log with all power-stage outputs inactive.
5. Plan the software I2C driver per DEC-019 before attempting display or sensor bring-up.

### Track B - Rev 1 Design (parts delayed)

1. Replace the MCU symbol with KiCad's official `STM32F411CEUx`. Do not reuse the SnapEDA G4 symbol.
2. Re-assign every net against the real F411 pinout under the Section 7.2 gate.
3. Add the `VCAP1` capacitor, connect `VDDA`/`VSSA`, and resolve `BOOT0`/`BOOT1` handling.
4. Fix the I2C assignment so `SCL` lands on an SCL-capable pin and `SDA` on an SDA-capable pin.
5. Correct the M1-M4 switching footprint to match available devices.
6. Apply DEC-016 exposed-pad improvements (relief vias, wider mask dams) as general QFN practice.
7. Re-export ERC and DRC before any routing.

## Key Files

- [docs/decisions-log.md](docs/decisions-log.md) - DEC-014, DEC-016 through DEC-020, Q-005
- [docs/test/rev0-bringup-addendum.md](docs/test/rev0-bringup-addendum.md) - Rev 0 staged bench procedure
- [docs/test/bringup-checklist.md](docs/test/bringup-checklist.md) - Rev 1 bench plan
- [hardware/mppt/kicad-implementation-checklist.md](hardware/mppt/kicad-implementation-checklist.md) - Section 7.2 Part Substitution Gate
- [hardware/kicad/solar-project/REVISION-INDEX.md](hardware/kicad/solar-project/REVISION-INDEX.md) - Rev 0 vs Rev 1 file map

## Working-Tree Caution

Preserve local KiCad edits. Keep lock, autosave, and cache artifacts out of commits; `~*.lck` is now gitignored. Untracked stray files in the KiCad directory (`.kicad_sch` root placeholder and the `GitHub_300w sine inverter kicad pcb.*` reference schematic) are intentionally not committed.
