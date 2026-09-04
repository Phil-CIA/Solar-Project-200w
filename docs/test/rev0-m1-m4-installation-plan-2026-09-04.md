# Rev 0 M1-M4 Installation Plan

Date: 2026-09-04
Scope: unpowered installation and passive verification only

## Approved Assembly Substitution

The available devices are ten Infineon `BSC030N04NSGATMA1` N-channel MOSFETs. This replaces the original `IPZ40N04S5L4R8ATMA1` selection for M1-M4.

Official Infineon data identifies `BSC030N04NS G` as:

- `PG-TDSON-8-1` / SuperSO8 5x6 package, 5.15 mm x 5.9 mm, 1.27 mm terminal pitch.
- 40 V maximum drain-source voltage.
- 3 mOhm maximum on-resistance at 10 V gate drive.
- 46 nC typical total gate charge at 10 V.
- Pads 1-3 source, pad 4 gate, and the exposed power pad drain.

The manufactured Rev 0 PCB uses the matching `Package_TO_SOT_SMD:TDSON-8-1` geometry and electrical mapping: pads 1-3 source, pad 4 gate, and large pad 5 drain. The LM51772-Q1 has a 2 A gate-driver class, so the 46 nC gate charge does not block physical installation. Rev 0 has no series gate resistors; ringing, transition time, dead time, and thermal behavior remain first-switch validation items.

This approval covers fitment and unpowered verification only. It does not authorize applying PV, an output source, a battery, a panel, a load, or any switching command.

## Bridge Mapping

| Position | Drain | Source | Gate |
|---|---|---|---|
| M1 | `PV_IN_POS` | `SW_1` | `HO1` |
| M2 | `SW_1` | `PWR_NEG` | `LO1` |
| M3 | `SW_2` | `PWR_NEG` | `LO2` |
| M4 | output rail | `SW_2` | `HO2` |

## Entry Gate

- [x] Exact candidate identified as `BSC030N04NSGATMA1`.
- [x] Ten matching devices available.
- [x] Package family and PCB land pattern match.
- [x] Board sources removed and U6 rails reported below 0.1 V.
- [x] USB, ST-LINK, PV, output, battery, panel, inverter, and AC leads visibly removed immediately before soldering.
- [x] U2 and U5 bypass jumpers identified and left unchanged.
- [x] M1-M4 pads confirmed clean, flat, and free of bridges before installation.

## Loose-Part Check

Test one device before applying paste:

1. Confirm the package orientation mark is visible.
2. Verify gate-to-source and gate-to-drain do not show a low resistance in either polarity.
3. In diode mode, verify one body-diode polarity between source and drain and blocking in the reverse polarity after discharging the gate to source.
4. Reject the lot for mixed markings, a gate short, drain-source conduction in both polarities, or inconsistent readings among sampled parts.

Record the meter model and readings. Do not infer a device failure from a slowly changing resistance caused by gate or meter charging; short gate to source before repeating the drain-source test.

## Installation Sequence

Install and inspect one device at a time in this order: M2, M3, M1, M4.

For each position:

1. Align the device orientation mark with PCB pad 1. Do not use neighboring component orientation as the reference.
2. Reflow with the minimum heat and time needed for complete wetting. Avoid lateral pressure that can bridge the large drain pad to source or gate pads.
3. Allow the board to cool before measuring.
4. Inspect pads 1-4 under magnification. Confirm pad 4 is not bridged to pads 1-3 or the exposed drain pad.
5. Check gate-to-source and gate-to-drain for a low-resistance short.
6. Check the expected body-diode polarity across that half-bridge device.
7. Check that the newly populated switch node is not a near-short to either adjacent power rail.
8. Stop on any ambiguous orientation, solder bridge, lifted pad, abnormal diode reading, or near-short. Do not install the next device until the result is resolved.

## Final Passive Gate

After all four devices are fitted and the board is cool:

- [x] M1-M4 orientation marks match their pad-1 indicators.
- [x] `HO1-SW_1`, `LO1-PWR_NEG`, `LO2-PWR_NEG`, and `HO2-SW_2` have no gate short.
- [x] `PV_IN_POS-SW_1`, `SW_1-PWR_NEG`, `SW_2-PWR_NEG`, and output-to-`SW_2` show the expected body-diode polarity and block in reverse after gate discharge.
- [x] `PV_IN_POS-PWR_NEG` is not a near-short.
- [x] Output-to-`PWR_NEG` is not a near-short.
- [x] `SW_1` and `SW_2` are not shorted together.
- [x] U2/U5 bypasses and the PGND-to-PWR_NEG jumper were not disturbed.
- [x] Both board sides and all four joints are photographed.

Installation result 2026-09-04: M2 and M3 were installed and passed visual, gate-short, body-diode, and switch-node resistance checks. M1 then passed the corresponding high-side checks, followed by M4. Final qualitative resistance checks found no near-short from input or output to `PWR_NEG` and no short between `SW_1` and `SW_2`. All four body diodes conducted in the expected direction and blocked when reversed after gate discharge. All orientation marks and visible joints passed magnified inspection. U2/U5 bypasses and the PGND-to-PWR_NEG jumper remained unchanged. Photographs covering both board sides and all four M1-M4 joints were captured. The installation-only gate is complete; this does not authorize power application or switching.

## Stop Point

Remove all meter leads after the final passive checks and leave the board unpowered. The next operation requires a separate first-switch entry plan covering protection-path disposition, output sink/preload, firmware state machine and limits, oscilloscope connections, bootstrap behavior, dead time, source limits, and immediate stop thresholds.