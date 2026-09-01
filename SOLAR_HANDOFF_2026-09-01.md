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

## Update (2026-09-01, later same day): Bench-Only Rework, Schematic Not Yet Updated

The `R33 -> 0 Ohm` bodge and the `PGND`-to-`AGND` jumper described above have been performed **only on the physical bench board**. The KiCad source files (`Solar Project Rev0.kicad_sch`/`.kicad_pcb`, `Solar Project Rev1.kicad_sch`/`.kicad_pcb`) still show the original `R33 = 6.49k` value and still show `PGND` (net 19) and `PWR_NEG`/`AGND` as separate, unconnected nets. Treat the schematic as **out of sync with the bench hardware** until it is explicitly edited. This is expected/acceptable for a bench bodge, but do not assume the checked-in design files reflect the current board state.

**Action item (open):** update `R33` to `0` (or DNP + wire link) in the KiCad schematic, and add either a dedicated `PGND`-`AGND` link (e.g., a 0 Ohm resistor or ferrite bead at a single point near U6, per datasheet Section 9.9.1.3 guidance) once the bench result confirms this is the desired production topology, or roll it into the Rev 1 delta plan instead of Rev 0 as-built docs.

### Follow-up Finding: PGND/AGND Ground-Domain Mismatch Affects More Than Just R33

Re-checking all U6 config-strap resistors against the datasheet's stated return node (Section 7.3, pin descriptions) found that the `PGND`-vs-`AGND` split was not unique to `R33`/`ADDR_SLOPE_CFG1`. The datasheet requires:

- `RT` (pin 12, frequency set): resistor to `AGND`.
- `CDC` (pin 10, cable-drop/current-monitor gain): resistor to `AGND` (or tie to ground if unused).
- `ADDR/SLOPE (CFG1)` (pin 9): resistor to `AGND` for slope select, or direct short to `GND`/`VCC2` for I2C address.
- `CFG2` (pin 8): resistor to `GND` (datasheet text says "GND", less strict than the others).
- `ILIMCOMP/ISET` (pin 16): compensation/setpoint node, `R32` return.

Netlist evidence (`hardware/kicad/solar-project/report.txt`):

| Resistor | U6 net | Returns to | Datasheet wants |
|---|---|---|---|
| `R28` (RT) | `Net-(U6-RT)` | `PGND` | `AGND` |
| `R29` (CFG2) | `Net-(U6-CFG2)` | `PGND` | `GND` (either acceptable) |
| `R30` (CDC) | `Net-(U6-CDC)` | `PGND` | `AGND` |
| `R32` (ILIMCOMP/ISET) | `Net-(U6-ILIMCOMP_ISET)` | `PWR_NEG` (= U6 pin 17 `AGND`) | `AGND` - **correct** |
| `R33` (ADDR/SLOPE CFG1) | `Net-(U6-ADDR_SLOPE_CFG1)` | `PGND` | `AGND` (for slope) / direct short (for I2C) |

`PGND` (net 19, U6 pin 28 only) and `PWR_NEG` (the system ground used for U6 `AGND` pin 17, the STM32 `GND`, and nearly every other ground return on the board) are **two separate KiCad nets with no direct tie between them anywhere else in the design** — they were only expected to meet through the power path (source/drain returns of the switching FETs), not as a clean single-point analog reference. So `R28`, `R29`, `R30`, and `R33` were all referencing the noisier power-ground island instead of the clean analog ground the datasheet specifies for these threshold-sensing config pins, even before the I2C-specific finding.

**Practical implication:** your bench `PGND`-to-`AGND` jumper does not just fix the I2C address strap — it is effectively the single-point ground tie the datasheet calls for in Section 9.9.1.3 ("connect these ground nodes at any place close to one of the ground pins of the IC"), and it corrects the reference node for all four affected config resistors at once. This is a reasonable bench validation step, but it should be captured as an intentional Rev 1 layout decision (a deliberate, short, single-point `PGND`-`AGND` link near U6) rather than a permanent direct tie-everywhere, to preserve the noise-isolation intent of separate ground pours during switching operation.

### New Finding: `MODE` Pin Shorted to `ILIMCOMP/ISET` Node

Checking all U6 pin-to-net connections in `report.txt` for unexpected sharing (excluding the intentionally-common `PWR_NEG` ground net and the intentional `VIN`/`BIAS` tie at `PV_IN_POS`) found exactly one anomaly:

- `U6` pin 7 (`MODE`) and `U6` pin 16 (`ILIMCOMP/ISET`) are **both on the same net**, `Net-(U6-ILIMCOMP_ISET)`, alongside `R32` (100 kOhm to `PWR_NEG`/`AGND`).

Per the datasheet, `MODE` is a **digital logic input** ("pulled low = power save mode; pulled high = forced PWM/CCM... Do not leave this pin floating") and must not be tied to the `ILIMCOMP/ISET` analog setpoint/compensation node. As wired, `MODE` is not a clean logic level — its state is whatever voltage the `ISET` divider/compensation node settles to, and `MODE` additionally loads that analog node, which can also disturb the current-limit setpoint or comparator compensation. This is a genuine schematic/net wiring defect distinct from the `ADDR_SLOPE_CFG1` finding, and it exists in both `Solar Project Rev0.kicad_pcb` and (need to verify) Rev 1.

**Action item (open, not yet fixed on bench or in schematic):**

1. On the schematic, separate `MODE` (pin 7) onto its own net with a direct strap to `GND` (PSM, low-power default) or `VCC2`/`CTRL_3V3`-derived logic high (forced PWM/CCM), per the desired default operating mode — do not leave it sharing a node with `ILIMCOMP/ISET`.
2. On the bench board, verify with a meter whether `MODE` (U6 pin 7) is truly shorted to `ISET` (U6 pin 16)/`R32` via a trace, or whether this is purely a schematic/netlist labeling artifact from CAD editing. If a real trace short exists, this needs a cut-and-jumper bodge similar to the `R33` rework before `MODE` will behave as a clean digital input.
3. Re-check current-limit behavior (already-cautious H3 static test) is not being affected by this shared node before increasing test voltage/current further.

This finding is independent of the already-fixed I2C address strap and should be treated as a second, separate hardware defect to resolve before trusting `MODE`-dependent behavior (PSM vs. forced PWM) or the `ISET` current-limit threshold.
