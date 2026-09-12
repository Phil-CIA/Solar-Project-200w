# Rev 1 Battery-First Power Path

Status: design review artifact; no KiCad edits authorized yet
Date: 2026-09-09

## Evidence From The Manufactured Rev 0 Netlist

The authoritative artifact is `hardware/kicad/solar-project/Solar Project Rev0 - Manufactured First Build.net`.

| Net | Relevant nodes | Meaning |
|---|---|---|
| `V_out` | Converter output capacitors, M4 drain, R16 pins 3 and 4, U10 positive sense | Converter capacitor/output side of the charge shunt |
| `BAT_BUS_POS` | J3.2, F2.1, F2.2, R16 pins 1 and 2, J5.1, U6 VOUT, U5 D1 pins, D4.2, D6.2, and battery-domain sense/protection taps | Battery bus side, but presently shorted across F2 and R16 power terminals |
| J3 return | J3.1 and the common return net | Battery negative/common return |

The manufactured netlist therefore proves two separate facts:

1. The intended converter charge path is `V_out -> R16 -> BAT_BUS_POS`.
2. F2 is ineffective because both fuse pins are already on `BAT_BUS_POS`. It is not a series battery fuse in the manufactured build.

## Rev 1 Netlist Baseline

The current build target, `hardware/kicad/solar-project/Solar Project Rev1.net`, has corrected the Rev 0 F2 short but still does not implement the intended battery-first charge path:

- `F2.1` is on `CHG_OUT_POS` with U6 `VOUT`, D6, Q6, and the fan connector; `F2.2` is on `BAT_BUS_POS` with the battery connector. F2 is therefore series-connected and is correctly understood as the charger-output branch fuse: its purpose is to clear a charger-side short before board or wiring damage can create a fire hazard.
- R16 is part of the switched high-current path. Its high-current pads 3/4 are on `V_out`; its Kelvin-side pads 1/2 are on `V_out_sense`, which reaches Q5. The switching path continues through the shared `SRC` node and Q6 to `CHG_OUT_POS`, then through F2 to `BAT_BUS_POS`. This is an intentional output-switch/reverse-flow topology and must be reviewed by MOSFET state and body-diode behavior, not by direct net-label adjacency alone.
- The Rev 1 netlist still has no `CTRL_SUPPLY_IN` or `CTRL_EN_CHG` net. Battery-powered control-domain sourcing and positive default-off U6 inhibit remain unimplemented deltas.

Therefore the first schematic modification pass must document and verify the existing switched R16/Q5/SRC/Q6 path and its disabled-state reverse-flow behavior. It must retain F2 as charger-output short-circuit protection; a separate main battery fuse is a distinct system-level protection decision and must not be conflated with F2.

The four-pad R16 has Kelvin/sense pads. Rev 1 must preserve the 1 mOhm current-shunt function while ensuring that the two high-current terminals, not the Kelvin pads, carry the battery charging current.

## Rev 0 Bench Lessons Learned (Root Cause of PV Startup Failure)

Rev 0 bench testing at ~6 V input (FP4) revealed that attempting autonomous startup directly from a current-limited input into uncharged output capacitors (~1.1 mF) fails:
- **Energy Deficit:** Transferring energy into 0 V output capacitors collapses the input capacitor bank and input voltage below the hardware UVLO threshold (~5.4 V falling) within 50–80 us.
- **UVLO Bursting:** The converter repeatedly cycled in brief bursts without `SS_ATRK` ramping or reaching steady regulation.
- **Battery-First Resolution:** When a 4S LiFePO4 battery is connected first, the output capacitor bank (`V_out`) is passively precharged to the battery resting voltage (~12.8–13.4 V). When the converter starts, it operates across an incremental voltage delta ($\Delta V = 14.6\text{ V} - V_{\text{bat}}$) rather than a full 0 V short-circuit step.

## Intended Rev 1 Power Path

The positive path shall be:

```text
Battery connector positive
    -> F2 main battery fuse, physically close to the connector
    -> battery reverse-polarity / ideal-diode protection as allocated by the retained protection design
    -> protected BAT_BUS_POS
        -> battery bus output connector and distribution branches
        -> battery-domain control-power converter (U5)
        -> battery voltage/current/temperature and fault sensing
        -> R16 battery-side high-current terminal
    <- R16 converter-side high-current terminal
    <- V_out converter output capacitor bank
    <- retained LM51772 four-switch stage
```

The negative return remains the common battery/converter return unless the protection review explicitly allocates a different return element. The output capacitors remain on `V_out`, so a connected and correctly protected battery can establish the converter output bus through R16. This battery connection is a prerequisite for converter operation, not permission for U6 to switch.

Control power shall be derived from protected `BAT_BUS_POS` into the normal battery-powered control domain. 

## Dual USB Architecture (DEC-032)

To prevent ground loops, backfeed into converter bias rails, and PC damage:
1. **USB Port 1 (Telemetry / COM Only):**
   - Connected to CP2102N UART bridge.
   - Powered exclusively by host PC VBUS.
   - Signal lines (TX/RX) isolated / level-shifted to STM32.
   - Zero connection to internal power or gate-drive rails.
2. **USB Port 2 (Aux 5V Power Only):**
   - Type-C power sink (with CC pulldowns) or dedicated bench 5 V input.
   - Powers `CTRL_5V` / `CTRL_3V3` buck/LDO tree for bench firmware development without battery.
   - Reverse-blocked to prevent backfeeding high-voltage buses.
   - Firmware interlock permanently inhibits LM51772 switching while powered from Aux USB.

## Test Points and Measurement Pads (DEC-031)

To ensure safe, repeatable bench measurements without risk of probe slips or inductive lead artifacts:
1. **High-Speed Switch Nodes:** `SW1` and `SW2` test pads with adjacent local `PGND` pads for low-inductance oscilloscope ground springs; `U14_GATE` and `SRC` test points.
2. **Analog / Controller Diagnostics:** SMD loop / pad test points for `EN_UVLO`, `SS_ATRK`, `COMP`, `ILIMCOMP`, `VCC1`, `VCC2`, `FLT`, and `MODE`.
3. **Power Rails & Shunts:** Dedicated Kelvin test points across R16 and R22, plus clip-friendly pads on `PV_IN_POS_FUSED`, `PV_IN_POS`, `V_out`, `BAT_BUS_POS`, `CHG_OUT_POS`, and `PWR_NEG`.
4. **Digital / MCU Signals:** `BOOT0` (with 10 kOhm pull-down), `NRST`, `CTRL_EN_CHG`, `I2C1_SCL`, `I2C1_SDA`, `UART_TX`, `UART_RX`, `CTRL_3V3`, `CTRL_3V3A`, and `CTRL_5V`.

## Required Rev 1 Schematic Deltas

These are the bounded functional changes. They do not replace the converter topology.

1. **Make F2 a true series battery fuse.** Put the battery connector positive on one fuse pin and the protected battery-entry/protection node on the other. Remove every alternate copper/net-label path that joins the two sides. Place the fuse near the battery entry and document its rating and interrupt requirement.
2. **Restore the charge boundary.** Keep the high-current path `V_out -> R16 -> BAT_BUS_POS`; verify R16 current direction, Kelvin sense polarity, and that no other copper bypasses the shunt. Keep `BAT_BUS_POS` on the protected side of the battery-entry fuse/protection boundary.
3. **Add battery-connection inrush treatment only if required.** First calculate and measure the output-capacitor connection transient. Add precharge, controlled hot-swap, or a current-limited battery-entry element only if connector, fuse, R16, protection FET, capacitor, or BMS stress exceeds its limit. Do not add series impedance speculatively.
4. **Add battery-powered control startup.** Derive the normal control supply from protected `BAT_BUS_POS`, with defined undervoltage behavior. Keep USB service power isolated and explicitly unable to backfeed `BAT_BUS_POS`, U6 power, gate-drive supply, or the converter enable path.
5. **Add a hardware-safe default-off U6 inhibit.** The inhibit must hold U6 off while the STM32 is reset, unpowered, booting, battery-unqualified, USB-only, or PV-only. A released analog `EN_UVLO` threshold is not sufficient as the normal start command. Hardware UVLO remains an independent last-resort protection function.
6. **Add controlled startup and source regulation.** Configure and read back a low initial LM51772 average-current limit before releasing inhibit; start in the sub-ampere range pending bench closure. Add the PV qualification threshold relative to battery voltage, a PV operating floor above hardware UVLO, a bounded current/power ramp, and delayed weak-source retry behavior.

## Acceptance Checks Before KiCad Freeze

- With the battery disconnected, continuity inspection proves no path from the battery connector positive to protected `BAT_BUS_POS` through a bypass around F2.
- With F2 installed, the only high-current route from battery entry to `BAT_BUS_POS` crosses F2 and the retained reverse-polarity protection boundary.
- Four-wire measurement proves charging current flows from `V_out` through R16 into `BAT_BUS_POS`; Kelvin sense polarity agrees with firmware sign conventions.
- Battery connection at minimum and maximum qualified battery voltage has measured capacitor inrush, peak current, duration, and energy. Each is below the connector, fuse, protection, R16, capacitor, and BMS limits, or the selected precharge limit is documented.
- Battery-only power starts the STM32 and sensing domain with PV disconnected. USB-only service cannot raise the converter enable or produce gate-drive switching.
- MCU reset, MCU unpowered, invalid battery, BMS inhibit, PV absent, and PV present while U6 is inhibited all produce no switching.
- Before U6 release, firmware reads back the low initial current-limit configuration and records battery/PV qualification status.
- A strong source produces monotonic `SS_ATRK`, output-current, and PV-voltage startup. A weak source reduces demand before hardware UVLO and retries after the defined off-time without sustained UVLO packet bursts.
- The normal PV operating floor remains above hardware UVLO under ripple and transient margin, and the final limits remain 8 A to 10 A nominal charge current, 125 W continuous output, 150 W maximum PV operating power, and approximately 12 A hardware protection.
- Fresh netlist, ERC, and DRC evidence show F2 separation, R16 continuity, no USB-to-power-stage authorization path, and complete inhibit connectivity.

## Open Review Items

- Exact fuse rating and physical connector-side placement.
- Exact retained protection ordering relative to F2 and the definition of the protected battery-entry node.
- Whether measured output-capacitor inrush requires precharge or only connector sequencing guidance.
- Exact U6 inhibit implementation and its reset/power-loss default state.
- LM51772 current-limit register/strap value and the readback mechanism.
- Final output-capacitance, compensation, thermal, and component-rating calculations for the Rev 1 envelope.
