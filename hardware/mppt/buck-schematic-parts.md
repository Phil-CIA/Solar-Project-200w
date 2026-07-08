# MPPT Buck Schematic Parts Checklist

Last updated: 2026-07-06
Owners: Phil + Copilot
Purpose: identify the functional parts and support circuits that need to appear in the first buck MPPT schematic draft.

## 1. PV Input and Protection

Required functional parts:
- PV input connector: 2-position pluggable terminal block, 5.08 mm pitch, 300 V / 16 A class
- Input fuse: 10 A slow-blow fuse or fuse holder placeholder
- Reverse polarity protection element: series Schottky rectifier, 100 V / 10 A class
- TVS or surge suppression device: 64 V TVS diode, SMBJ/SMCJ class
- Input filter capacitor bank: 2 x 10 uF / 100 V X7R plus 100 nF / 100 V
- Optional common-mode or differential filtering placeholder if EMI mitigation is needed later

Placement note:
- Use these as the first symbols placed on Sheet 1 before any buck switch-stage symbols are added.

Required notes:
- ASSUME: 2S Renogy panel baseline
- SAFETY: PV reverse polarity ownership
- SAFETY: PV transient suppression ownership
- VERIFY: worst-case cold Voc margin against all front-end parts

Recommended sheet-order summary:
- J1 connector -> F1 fuse -> D1 reverse diode -> D2 TVS -> C1/C2/C3 input caps

## 2. Buck Power Stage

Required functional parts:
- High-side buck switching MOSFET
- Freewheel path element: diode for asynchronous buck or low-side MOSFET for synchronous buck
- Buck inductor
- Switching-node snubber placeholder
- Input decoupling capacitors near switching loop
- Output capacitor bank

Required notes:
- DECISION: synchronous vs asynchronous implementation if not yet locked
- VERIFY: switch voltage rating margin
- VERIFY: inductor ripple-current target
- VERIFY: thermal observation point near switch and inductor

## 3. Charger Control and Drive

Required functional parts:
- MPPT controller or control IC placeholder
- Gate driver path if controller cannot drive power switch directly
- Bootstrap or driver support parts if required by selected approach
- Soft-start / compensation / timing network placeholders
- UVLO and enable boundary

Required notes:
- DECISION: DEC-005 controller partition still open
- VERIFY: startup behavior under weak-sun conditions
- VERIFY: UVLO/dropout strategy for low-irradiance headroom cases

## 4. Output and Battery Interface

Required functional parts:
- Charger output connector or battery interface boundary
- Output current sense element
- Optional output disconnect / reverse-current blocking placeholder if architecture needs it
- Main fuse handoff note to battery-domain protection

Required notes:
- ASSUME: 15A continuous planning bound until Q-001 closes
- SAFETY: battery fuse ownership handoff
- VERIFY: output current path and return path are explicit

## 5. Sensing and Telemetry

Required functional parts:
- PV voltage sense divider/filter
- PV current sense path
- Battery voltage sense divider/filter
- Battery current sense path
- Power-stage temperature sensor
- Board/control temperature sensor
- UART logging header or debug connector placeholder

Required notes:
- VERIFY: all required sense signals map to REQ-TEL-002
- VERIFY: serial logging path exists for first bring-up

## 6. First-Draft Minimum Part Categories

Minimum categories that must exist in the first schematic even if exact part numbers are TBD:
- Connector
- Fuse / protection placeholder
- TVS / suppression placeholder
- MOSFET / diode switching path
- Inductor
- Input capacitors
- Output capacitors
- Sense resistor or current-sense element
- Voltage divider networks
- Temperature sensors
- Controller / gate-drive placeholder
- Debug / serial header

## 7. Exit Gate

- Every category above appears in the schematic as a real symbol or an explicit placeholder block.
- Every safety-critical omission is called out as a visible note, not left implicit.
- Every unresolved part-class decision is referenced back to the decisions log.
