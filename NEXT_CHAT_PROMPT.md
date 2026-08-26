Continue the Solar Project 200W repository from the 2026-08-26 Rev 0 failure-analysis handoff.

## Current priority

Recover the Rev 0 boards with the replacement MCU, or advance the Rev 1 F411 migration if parts have not arrived.

## State to assume

- Rev 0 boards are built but have never been powered.
- The U1 `CTRL_3V3`-to-`PWR_NEG` short is solved (DEC-018): the board was routed against the `STM32G431CBU6` symbol while `STM32F411CEU6` parts were fitted, creating hard supply-pin conflicts through the die. No rework can fix it; the wrong part was fitted.
- Two `STM32G473CEU6` (UFQFPN-48) are on order, verified pin-identical to the routed G4 map across all 49 pins.
- Rev 1 will migrate to `STM32F411` properly, with the real symbol (DEC-020).
- Rev 0 I2C is swapped at the MCU (DEC-019): `PB6` carries `I2C DATA`, `PB7` carries `I2C CLK`. Hardware `I2C1` is unusable; use software bit-banged I2C.
- `PB11` (pad 24) is tied to `PWR_NEG` and must never be an output.
- `PB8`/BOOT0 (pad 46) floats; acceptable at default G4 option bytes.
- UART wiring is correct.
- M1-M4 are unpopulated due to a separate footprint mismatch; inverter and AC testing stay blocked.
- Working tree may contain local KiCad edits; avoid destructive cleanup.

## What to do first

1. Read HANDOFF.md and SOLAR_HANDOFF_2026-08-26.md.
2. Run git status --short and capture baseline.
3. Ask which track applies: Track A (parts arrived, Rev 0 recovery) or Track B (Rev 1 design work).

## Track A - Rev 0 recovery

1. Fit one `STM32G473CEU6` using the established hot air process.
2. Before applying any power, measure `CTRL_3V3`-to-`PWR_NEG`. A high reading is the pass/fail gate for DEC-018.
3. Run Stage A and Stage B of docs/test/rev0-bringup-addendum.md.
4. Confirm SWD attach, reset, and serial logging with all power-stage outputs inactive.
5. Plan the software I2C driver before display or sensor bring-up.

## Track B - Rev 1 design

1. Replace the MCU symbol with KiCad's official `STM32F411CEUx`; do not reuse the SnapEDA G4 symbol.
2. Re-assign every net against the real F411 pinout under Section 7.2 of hardware/mppt/kicad-implementation-checklist.md.
3. Add the `VCAP1` capacitor, connect `VDDA`/`VSSA`, resolve `BOOT0`/`BOOT1`.
4. Fix the I2C assignment so SCL and SDA land on capable pins.
5. Correct the M1-M4 switching footprint to match available devices.
6. Apply DEC-016 exposed-pad relief vias and wider mask dams.
7. Re-export ERC and DRC before routing.

## Constraints

1. Keep autosave and lock artifacts out of commits.
2. Never conclude a part substitution is safe without pin-by-pin verification against a trusted library or datasheet.
3. Do not move charger or safety authority into the ESP32.
4. Do not energize any board that reads a low `CTRL_3V3`-to-`PWR_NEG` resistance.
5. Log architecture and part changes before implementation.

