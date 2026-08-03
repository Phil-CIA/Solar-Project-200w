# Schematic Partition For SPICE Modeling

Last updated: 2026-07-30
Scope: split the current single-sheet KiCad schematic into simulation blocks that can be validated independently and then composed.

## 1. Why This Partition

- Keep switching-power convergence problems isolated from control/sensing circuits.
- Let you swap fidelity by block (ideal, averaged, switched).
- Keep net naming aligned with your existing project conventions.

## 2. SPICE-Safe Net Name Normalization

Some live labels include spaces or leading underscore patterns that are awkward in SPICE decks. Use this alias map when building testbenches.

| KiCad Label | SPICE Alias |
|---|---|
| V_out _on | V_out_on |
| Fan Control | Fan_Control |
| I2C CLK | I2C_CLK |
| I2C DATA | I2C_DATA |
| _OUTPUT Feedback | OUTPUT_Feedback |
| _Undervoltage Feedback | Undervoltage_Feedback |

Keep canonical power/boundary nets unchanged where possible:

- PV_IN_POS_RAW
- PV_IN_POS_FUSED
- PV_IN_POS
- CHG_OUT_POS
- BAT_BUS_POS
- PWR_NEG
- CTRL_3V3

## 3. Block Decomposition

## 3.1 B0: PV Input + Front-End Protection

Goal:
- Verify survivability and drop from PV entry to the fused/conditioned input rail.

Owns:
- PV connector boundary, fuse path, reverse-polarity/protection input network.

Primary interfaces:
- Inputs: PV_IN_POS_RAW, PWR_NEG
- Outputs: PV_IN_POS_FUSED, PV_IN_POS

Suggested analyses:
- DC sweep of VIN (for example 8 V to 60 V)
- Line step transient
- Reverse polarity sanity checks

## 3.2 B1: Main Buck Power Stage (Plant)

Goal:
- Validate conversion behavior and stress of switch node components.

Owns:
- High-side switch path, freewheel path/synchronous path, inductor, output caps, snubber.

Primary interfaces:
- Inputs: PV_IN_POS, PWR_NEG
- Control in: gate-drive command (for example CTRL_PWM_MAIN or LM51772 HO/LO net set)
- Outputs: CHG_OUT_POS, switching-node observables

Suggested analyses:
- Operating point at duty ratio corners
- Transient startup/load steps
- Efficiency and device stress at min/nom/max VIN

## 3.3 B2: Output Boundary + Battery Interface

Goal:
- Verify battery-side boundary behavior and fuse-path assumptions.

Owns:
- Output fuse/connector/test-point side and boundary load models.

Primary interfaces:
- Input: CHG_OUT_POS
- Boundary: BAT_BUS_POS, PWR_NEG

Suggested analyses:
- Battery model sweeps (low/high SOC equivalent voltage)
- Output short/near-short boundary behavior with current limiting assumptions

## 3.4 B3: Control-Rail Supply Path

Goal:
- Validate generation and startup robustness of CTRL_3V3 across source modes.

Owns:
- Wide-input control supply stage and local decoupling.

Primary interfaces:
- Inputs: CTRL_SUPPLY_IN, USB_5V_IN (if modeled as bench fallback)
- Outputs: CTRL_3V3

Suggested analyses:
- UVLO/startup sweep
- Source handoff behavior (PV present/absent)
- Load transient on 3V3 rail

## 3.5 B4: Fault Comparator and Threshold Network

Goal:
- Validate OVP/OCP threshold trip points and hysteresis margins.

Owns:
- Comparator chains and reference-divider thresholds.

Primary interfaces:
- Inputs: OVP_SENSE, OCP_SENSE, OCP_MEAS, OVP_REF, OCP_REF
- Outputs: FAULT_OVP, FAULT_OCP, U6_FLT

Suggested analyses:
- DC sweep to extract trip points
- Noise injection around thresholds
- PVT sensitivity using component tolerance corners

## 3.6 B5: Measurement Front Ends

Goal:
- Validate ADC-facing scaling, filtering, and source impedance.

Owns:
- Dividers and filters feeding V_in_FB / V_out_sense / related ADC nodes.

Primary interfaces:
- Inputs: PV_IN_POS_FUSED, CHG_OUT_POS, BAT_BUS_POS
- Outputs: SENSE_PV_V, SENSE_BAT_V, SENSE_PV_I, SENSE_BAT_I

Suggested analyses:
- Transfer curve linearity
- RC settling time for ADC sampling window assumptions
- Error budget sweep with resistor tolerances

## 3.7 B6: MCU + Digital I/O (Behavioral)

Goal:
- Replace firmware details with behavioral sources while exercising analog interfaces.

Owns:
- PWM command source, enable pins, pullups/pulldowns, simple button emulation.

Primary interfaces:
- Outputs: CTRL_PWM_MAIN, CTRL_EN_CHG
- Inputs: FAULT_OVP, FAULT_OCP, SENSE_*

Suggested analyses:
- Duty step response into B1 plant
- Fault response timing (assert/deassert)

## 4. Integration Order

1. B0 alone (input integrity)
2. B1 with ideal gate drive and ideal source/load
3. B1 + B2 (plant + battery boundary)
4. B3 (control rail) independently
5. B4 and B5 independently
6. B1 + B4 + B5 (closed-loop analog protection sensing)
7. Add B6 behavioral control for system-level runs

## 5. File Layout In This Folder

- 00_block_b0_pv_input.cir
- 01_block_b1_power_stage.cir
- 02_block_b2_battery_boundary.cir
- 03_block_b3_control_rail.cir
- 04_block_b4_fault_network.cir
- 05_block_b5_measurement_frontend.cir
- 90_system_integration_harness.cir

Each file is a starter stub and should be populated incrementally from the schematic.

## 6. Practical Rule While Translating From KiCad

- Translate one block at a time into a self-running deck with local sources/loads.
- Do not bring in MCU, UI, or comms nets unless needed for that block.
- Keep each block deck runnable with one command before integrating.