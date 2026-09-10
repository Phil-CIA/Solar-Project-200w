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

## Intended Rev 1 Power Path

The positive path shall be:

```text
Battery connector positive
    -> F2 main battery fuse, physically close to the connector
    -> battery reverse-polarity / ideal-diode protection as allocated by the retained protection design
    -> protected BAT_BUS_POS
        -> battery bus output connector and distribution branches
        -> battery-domain control-power converter
        -> battery voltage/current/temperature and fault sensing
        -> R16 battery-side high-current terminal
    <- R16 converter-side high-current terminal
    <- V_out converter output capacitor bank
    <- retained LM51772 four-switch stage
```

The negative return remains the common battery/converter return unless the protection review explicitly allocates a different return element. The output capacitors remain on `V_out`, so a connected and correctly protected battery can establish the converter output bus through R16. This battery connection is a prerequisite for converter operation, not permission for U6 to switch.

Control power shall be derived from protected `BAT_BUS_POS` into the normal battery-powered control domain. USB may power an isolated service/debug branch, but USB alone shall not power or release the converter gate-drive/start path. The STM32 owns qualification and charger policy; U6 retains the fast electrical regulation loops.

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
