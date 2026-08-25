# Solar Project 200W - Rev 0 Bring-Up Addendum

Last updated: 2026-08-24
Applies to: as-manufactured Rev 0 board
Authoritative source: `C:\Users\forch\OneDrive\JLCPCB files\Solar Charger\Solar Project Rev0 - Manufactured First Build`

Use this addendum with the general safety practices in `bringup-checklist.md`. It overrides any general-plan step that assumes a complete switching bridge.

## Board Limitation

M1, M2, M3, and M4 are not fitted because their intended `IPZ40N` TDSON-8 footprint does not match the available devices. The Rev 0 schematic places these MOSFETs on the two gate-drive pairs `HO1`/`LO1` and `HO2`/`LO2`.

Consequences:

- The inverter bridge is incomplete and must be treated as nonfunctional.
- Do not apply battery power, DC-link power, or any source intended to energize the inverter bridge.
- Do not connect an AC load, transformer, or inverter output.
- Do not attempt open-loop, closed-loop, or software-controlled inverter switching.
- Do not fit substitute MOSFETs with flying leads or improvised adapters. Correct footprint/device selection belongs in a documented revision or approved rework plan.

This limitation does not, by itself, prove that the separate charger/control sections are unusable. Their tests remain separately gated by fitted parts, schematic review, and controlled source/sink capability.

## Permitted Rev 0 Bench Scope

| Test area | Status | Conditions |
|---|---|---|
| Visual inspection and passive continuity | Allowed | Record M1-M4 as deliberately unpopulated; inspect empty pads for bridges or contamination. |
| USB/`VBUS` control-power bring-up | Allowed | Use an isolated, current-limited bench supply through the intended USB/`VBUS` path. PV, battery, and inverter connections remain open. |
| `CTRL_3V3`, MCU, SWD, serial, buttons, display, I2C, fan-control logic | Allowed | Firmware must default all power-stage commands inactive. Test a peripheral at a time. |
| Fault and measurement signal readout | Allowed | Use low-energy stimulus only; do not defeat hardware pullups, comparators, or protection paths. |
| Gate-drive logic observation | Conditionally allowed | Keep inverter energy inputs disconnected. Before enabling a drive signal, verify the gate-driver supply/enable requirements and use a scope-only test mode. No attempt may energize the incomplete bridge. |
| Charger static-path checks | Conditionally allowed | Use only a current-limited source and a controlled output sink/simulator after confirming the actual fitted charger power parts and protection path. PWM remains inactive. |
| Charger switching | Blocked pending review | Requires a separate entry checklist that identifies the fitted charger MOSFETs, gate driver/controller, inductor, diode/synchronous path, current sense, output capacitors, source/sink ratings, and safe firmware limits. M1-M4 absence does not authorize or prohibit this test by itself. |
| Battery charging or real PV input | Blocked | No real battery or panel until the charger-specific checklist passes and BMS/panel limits are recorded. |
| Inverter / AC operation | Blocked | Requires a complete, reviewed M1-M4 bridge and a separate AC hazard procedure. |

## Rev 0 Test Sequence

### R0-A - Unpowered Inspection

1. Photograph both board sides and record the assembly revision.
2. Mark M1-M4 as deliberately unpopulated in the bench record. Confirm all four footprints are clean, with no solder bridges across pads.
3. Check for shorts from `VBUS` and `CTRL_3V3` to `PWR_NEG`; record stabilized resistance readings.
4. Check `HO1`, `LO1`, `HO2`, and `LO2` for an unintended short to a supply or return. Do not infer a normal resistance without the gate-driver schematic and fitted-part data.
5. Verify connector polarity, fuse values, polarized capacitors, TVS devices, and the USB protection path against the Rev 0 schematic and fitted BOM.

**Stop:** any power-to-return near-short, damaged empty MOSFET pad, reversed polarized part, or unknown fitted substitution.

### R0-B - Low-Voltage Control Bring-Up

1. Disconnect PV, battery, charger output load, inverter wiring, and AC wiring.
2. Set the isolated bench supply to 0 V and enable a conservative current limit. Record the limit before connecting it to the intended `VBUS`/USB power entry.
3. Raise the source only to the designed USB-equivalent input voltage. Record source voltage/current and `CTRL_3V3`.
4. Stop immediately for current limiting, voltage collapse, unexpected heating, or a control rail outside the actual regulator tolerance.
5. Program and attach to the STM32 through SWD. Confirm reset, serial logging, and that all PWM/inverter enable outputs stay inactive after boot and reset.
6. Test low-energy peripherals one at a time: button input, display/I2C activity, and fan-control logic with a suitable low-power test load.

### R0-C - Measurement and Safe-State Verification

1. Apply only low-energy, bounded stimulus to measurement inputs using a documented divider, calibrator, or simulator.
2. Verify reported values, `FAULT_OCP`, `FAULT_OVP`, and the safe firmware response without creating a power-stage demand.
3. Capture control outputs after reset, normal idle, and an injected low-energy fault. All inverter-drive commands must remain disabled while M1-M4 are absent.

### R0-D - Optional Charger Review Gate

Do not attempt this stage until a reviewer documents that the charger power path is electrically independent of M1-M4 and identifies every fitted switching/current-carrying component in that path. The reviewer must also define the specific source voltage/current limits, output sink, firmware image, scope method, stop thresholds, and evidence required.

## Required Bench Record

| Item | Record |
|---|---|
| Board identifier and photographs |  |
| M1-M4 status | Unpopulated due to footprint mismatch |
| Bench supply model, voltage, and current limit |  |
| `VBUS` and `CTRL_3V3` readings |  |
| Firmware revision and SWD result |  |
| Safe-state observation after boot/reset/fault |  |
| Peripheral and telemetry results |  |
| Charger review-gate decision | Pass / Fail / Not attempted |
| Inverter/AC status | Blocked |

## Closeout

Log the M1-M4 footprint mismatch as a Rev 0 hardware limitation and as a Rev 1 corrective-action input. Do not clear the inverter/AC block until a board with the correct switching footprint and a separate high-energy test plan is available.