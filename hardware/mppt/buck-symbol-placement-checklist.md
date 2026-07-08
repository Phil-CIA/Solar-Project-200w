# Buck MPPT Symbol Placement Checklist

Last updated: 2026-07-07
Owners: Phil + Copilot
Purpose: define the order and contents for the first KiCad symbol placement pass for the buck MPPT path.

## 1. Placement Order

### Sheet 1 - PV Input and Protection

Place these first:
- PV input connector
- Input fuse or protected input placeholder
- Reverse polarity protection element
- TVS / surge suppression device
- Input filter capacitors
- Optional input filtering placeholder if needed later

Placement notes:
- Keep the high-current PV loop compact.
- Put protection parts physically and logically at the connector boundary.
- Add ASSUME and SAFETY notes on the sheet before adding the power stage.

### Sheet 2 - Buck Power Stage

Place these next:
- High-side buck MOSFET
- Freewheel diode or synchronous low-side MOSFET
- Buck inductor
- Switching-node snubber placeholder
- Input decoupling capacitors near the switch loop
- Output capacitor bank

Placement notes:
- Keep input caps close to the switch loop.
- Keep the inductor and switch node visually obvious.
- Make the current path easy to trace from input to battery side.

### Sheet 3 - Controller and Drive

Place these after the power stage is visible:
- MPPT controller / control IC placeholder
- Gate driver support parts if required
- Soft-start / compensation / timing network
- UVLO / enable boundary
- Fault outputs and enable control pins

Placement notes:
- Place control around the power stage, not hidden off-page.
- Show all enable, fault, and timing nets clearly.
- Keep DEC-005 visible as a still-open decision if the controller type is not yet locked.

### Sheet 4 - Output and Battery Interface

Place these next:
- Charger output connector / battery interface boundary
- Output current sense element
- Main fuse handoff note to battery protection domain
- Optional reverse-current / disconnect placeholder if later needed

Placement notes:
- Make the charger-to-battery current path explicit.
- Tie the output boundary to the 15A planning cap note until Q-001 is closed.

### Sheet 5 - Sensing and Telemetry

Place these last:
- PV voltage sense divider/filter
- PV current sense path
- Battery voltage sense divider/filter
- Battery current sense path
- Power-stage temperature sensor
- Board/control temperature sensor
- UART/debug header

Placement notes:
- Keep sense points close to the measured nodes.
- Route telemetry and debug out of the high-current loop visually and electrically.
- Ensure every sensing symbol maps to a named net from net-plan.md.

## 2. What Must Be Visible On The First Draft

- PV input boundary and protection ownership
- Buck power-stage parts and current flow
- Controller placeholder and its relation to the power stage
- Battery interface and fuse handoff
- Sense nodes and telemetry header
- All temporary assumptions called out as notes

## 3. Suggested Sheet Annotations

- ASSUME: two Renogy 100W panels in series
- ASSUME: 15A continuous planning cap until Q-001 closes
- DECISION: DEC-003 pending final buck closure
- SAFETY: PV boundary protection ownership
- SAFETY: battery fuse handoff ownership
- VERIFY: cold-Voc margin and low-irradiance headroom
- VERIFY: startup / UVLO / dropout behavior

## 4. Definition of Done

- Each sheet contains the symbols needed for a readable first-pass schematic.
- The buck current path is visually traceable without guessing.
- No safety-critical function is hidden in an unlabeled block.
- The checklist can be used as a live session guide while drawing in KiCad.
