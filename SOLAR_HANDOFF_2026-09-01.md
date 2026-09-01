# Solar Project 200W - LM51772 (U6) I2C Silence Root Cause (2026-09-01)

## Summary

The H5 software-I2C scan (2026-08-30/31) only found `0x38` (AHT20) and `0x3C` (SSD1306) on the shared bus. U6 (LM51772-Q1) never acknowledged. The chip is not dead; the board's ADDR/SLOPE strap disables the I2C interface by design.

## Root Cause

Per the TI LM51772-Q1 datasheet (`docs/lm51772-datasheet.pdf`, Section 7.5.1 and Table 7-4, "ADDR/Slope Pin (R2D-CH1) Configuration Overview"):

- I2C is enabled **only** when U6 pin 9 (`ADDR/SLOPE`, net `ADDR_SLOPE_CFG1`) is a **direct short** to `GND` (7-bit address `0x6A`) or to `VCC2` (7-bit address `0x6B`).
- Any resistor placed from that pin to GND/VCC2 instead selects one of 14 slope-compensation factors and **explicitly disables I2C**, repurposing `SDA/CFG3` (pin 5) and `SCL/CFG4` (pin 6) as `CFG3`/`CFG4` configuration strap inputs instead of I2C lines (datasheet line ~5240: "The channels CFG3 and CFG4 are multiplexed with the SDA/SCL pins, and can only be used when I2C function is disabled.").

This project's schematic/netlist (`hardware/kicad/solar-project/report.txt` line 1248, confirmed against `Solar Project Rev0.kicad_sch`/`Rev1.kicad_sch`) shows:

- `R33` = 6.49 kOhm from `ADDR_SLOPE_CFG1` (U6 pin 9) to `PGND`.
- U6 pin 5 (`SDA_CFG3`) and pin 6 (`SCL_CFG4`) are wired to the same `I2C DATA`/`I2C CLK` nets used by the STM32 software I2C bus, AHT20, and SSD1306 (`report.txt` lines 1920, 1943).

6.49 kOhm lands on the 6.5 kOhm row of Table 7-4, which selects `msc = 1` slope compensation and **I2C DISABLED** — not a direct GND short. This is a hard, strap-level block: it holds regardless of VCC1/VCC2 health, EN_UVLO state, or firmware correctness, and fully explains the H5 scan result.

## Why the Board Still Looked "Alive"

The 2026-08-30 H3 static point (6.0 V / 20 mA) showed sane rails (`VCC1=3.998 V`, `VCC2=3.5921 V`, `RST=3.299 V`) with no current-limit or heating. The digital core is powered and the part is functioning as a strapped (non-I2C) controller; it was never going to answer on `0x6A`/`0x6B` given the R33 strap, independent of that health.

## Rework / Next Step Options

To restore I2C communication with U6, the `ADDR_SLOPE_CFG1` net must be a direct short, not a resistor divider:

1. Remove/bypass `R33` and bodge U6 pin 9 directly to `PGND` for address `0x6A`, or directly to `VCC2` for address `0x6B`.
2. After the bodge, repeat the H5 `i2cscan` low-energy test (board still USB-powered only, PV/battery/loads disconnected) and confirm `0x6A` (or `0x6B`) now acknowledges alongside `0x38`/`0x3C`.
3. Current-limit slope compensation previously implied by the R33 resistor value must be replaced with the equivalent I2C register setting (`ILIM_THRESHOLD`/slope DAC path) once I2C is enabled; do not leave the current-limit behavior undefined after removing R33.

## Rev 1 Correction

- Replace `R33` with a direct 0 Ohm link (or trace) from `ADDR_SLOPE_CFG1` to `GND` or `VCC2` per the chosen I2C address; do not use a resistor divider value on this net if I2C control of U6 is required.
- Update `hardware/mppt/lm851772-lm51772-migration-checklist.md` Section 3 (I2C/control row) accordingly; this file now records the confirmed FAIL status and rework guidance.

## Non-Negotiable Safety (unchanged)

Continue to follow the safety limits in `SOLAR_HANDOFF_2026-08-31.md` and `docs/test/rev0-low-energy-comm-control-test.md`: PV/battery/charger output/inverter/AC/loads stay disconnected; `PA8`/`PB5`/`PA5` stay low; verify ST-LINK/target identity before every flash; use software I2C on `PB6`/`PB7` only.
