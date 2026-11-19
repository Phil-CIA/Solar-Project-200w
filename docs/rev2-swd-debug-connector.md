# Rev2 keyed 10-pin SWD connector

J9 follows the keyed 10-position ARM Cortex debug interface at 1.27 mm pitch.
The accepted scope is **SWD only**: do not implement full JTAG or SWO. Preserve
SWD on PA13/PA14 and keep PB3/PB4 allocated to USART2.

## Pin mapping

| J9 pin | ARM Cortex signal | Rev2 connection |
|---:|---|---|
| 1 | VTref | `CTRL_3V3` (sense only; not a power output) |
| 2 | SWDIO | `SWDIO` |
| 3 | GND | `PWR_NEG` |
| 4 | SWCLK | `SWCLK` |
| 5 | GND | `PWR_NEG` |
| 6 | SWO | No connect |
| 7 | Key | Physically omitted; no copper pad |
| 8 | NC/TDI | No connect |
| 9 | GND | `PWR_NEG` |
| 10 | nRESET | `STM_NRST` |

Pin 1 is target-voltage sensing only; do not use it to power the board. Pins 3,
5, and 9 are ground. Select **SWD** in the probe host tool and leave SWO/SWV
disabled. The schematic symbol marks pin 7 as the key/no-connect position, and
the footprint omits pad 7.

## Connector candidate and land pattern

Samtec `FTSH-105-01-L-DV-007-K` is a candidate, not a locked BOM choice. Its
manufacturer product page describes a 1.27 mm, 2-row, 10-position SMT header
with a keying notch and pin 7 omitted. The `-K` option is specified for mating
with the Samtec FFSD IDC ribbon-cable family. The actual ST-Link cable supplied
for this project has not been identified or verified as an FFSD-compatible mate.

The project-local footprint
`Rev2_Debug_Footprints:FTSH-105-01-L-DV-007-K` follows Samtec's recommended PCB
land-pattern drawing: 1.27 mm position pitch, 3.43 mm row spacing, 0.74 x
2.79 mm surface-mount lands, and a 6.35 x 6.86 mm connector outline. It places
pad 1 at the lower-left of the drawing orientation and omits the pin-7 land.
The official drawing and part print remain the source of truth for assembly;
there is no third-party generic header footprint or claim that NC alone creates
the physical key.

## Adapting a 10-pin 2.54 mm probe connection

Keep the pitch adapter off the controller board: use a passive adapter PCB or
breakout cable between the probe's 2x5 2.54 mm connection and a keyed 2x5
1.27 mm socket that mates with J9. Pitch conversion needs no active electronics
when the probe supports the target's 3.3 V SWD signal levels.

Do not assume that every 10-pin 2.54 mm probe uses the Cortex pinout. Identify
the actual ST-Link model and connector pinout first, then map by signal:
probe VTref to J9.1, SWDIO to J9.2, SWCLK to J9.4, nRESET to J9.10, and probe
ground to J9.3/5/9. If the probe uses ground detection, verify its requirements.
A straight-through 1-to-1 pitch adapter is appropriate only when both pinouts
match. Do not connect a probe power-output pin to VTref; power the target
normally. Leave SWO/TDI unused and preserve pin-1 and key orientation.

The adapter part number, socket compatibility, and probe-side mapping remain
unconfirmed until the actual probe/cable is identified.

## PCB placement, routing, and release status

J9 replaces the redundant four-pin J4 header on Rev2 and is provisionally
placed in J4's former board area at `(171.12, 135.70) mm`, rotated 90 degrees.
J4's symbol, footprint, three directly attached labels, and unused-pin
no-connect marker have been removed; Rev1 is unchanged.

The previous J9 routes were replaced, not left as stubs. SWDIO/SWCLK/NRST
connect to U1 pads 36/37/7, VTref to U1 pad 35 (`CTRL_3V3`), and all three
ground pins to nearby D9 pad 1 (`PWR_NEG`). The replacement uses 23 track
segments and eight vias. The reset net retains the scoped rename from legacy
`Net-(S1-A)` to `STM_NRST`; existing reset components were retained. All 480
unrelated top-level PCB objects were preserved without object-content changes,
and all non-J4 schematic netlist endpoint assignments are unchanged.

KiCad 10.0.6 DRC with zone refill reports 60 violations (14 errors, 46
warnings) and 344 unconnected items, versus 62 violations (16 errors, 46
warnings) and 347 unconnected items immediately before the J4 replacement.
There are no new physical DRC findings: removing J4 eliminates its two existing
copper-to-board-edge errors and three unrouted endpoints. J9 has no physical
DRC or unconnected-item findings; copper connectivity was checked for all seven
active pads. Schematic parity remains at 86 issues; the only J9 parity finding
is the intentional missing pad 7 used for keying. Schematic ERC remains at
0 errors and 55 warnings. The board still has baseline unrouted endpoints on
shared nets, so these J9 traces do not qualify the whole power/reset network.

The actual ST-Link cable/mate and enclosure opening, access, and cable bend
clearance remain unverified. No bench attach or programming validation is
claimed. Keep issue #17 open until cable mating, mechanical access, and SWD
attach/program acceptance are verified.

References: [Samtec FTSH-105-01-L-DV-007-K product page](https://www.samtec.com/products/ftsh-105-01-l-dv-007-k);
[recommended PCB land pattern](https://suddendocs.samtec.com/prints/ftsh-1xx-xx-xxx-dv-xxx-footprint.pdf);
[FTSH series print](https://suddendocs.samtec.com/prints/ftsh-1xx-xx-xxx-dv-xxx-xxx-x-xx-mkt.pdf).
