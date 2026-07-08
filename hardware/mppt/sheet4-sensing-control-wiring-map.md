# Sheet 4 Sensing and Control Wiring Map

Last updated: 2026-07-08
Owners: Phil + Copilot
Scope: define first-pass, implementation-ready zone plan for sensing and control I/O in the single-sheet KiCad schematic, starting with voltage and current measuring circuits.

## 1. Purpose

Start Zone D (Sheet 4 intent) with stable net boundaries and placeholders, beginning with voltage and current measuring circuits without forcing DEC-005 closure.

## 2. First-Pass Symbol Set

| Ref | Function | First-Pass Type | Notes |
|---|---|---|---|
| U_CTRL | Control placeholder block | Generic control symbol or hierarchical placeholder | Use as owner for control and telemetry nets while DEC-005 is deferred |
| TP_PV_V | PV voltage sense tap | Test point | Optional but recommended |
| TP_PV_I | PV current sense tap | Test point | Optional but recommended |
| TP_BAT_V | Battery voltage sense tap | Test point | Optional but recommended |
| TP_BAT_I | Battery current sense tap | Test point | Optional but recommended |
| TH_PWR | Power thermal sensor placeholder | NTC or sensor placeholder symbol | Bind to SENSE_TEMP_PWR |
| TH_BOARD | Board thermal sensor placeholder | NTC or sensor placeholder symbol | Bind to SENSE_TEMP_BOARD |
| J_LOG | Serial logging header | 1x03 or 1x04 header | Include TX, RX, and return reference |

## 2.1 First Build Priority

Start with the measurement circuits first:

1. PV voltage sense path.
2. PV current sense path.
3. Battery voltage sense path.
4. Battery current sense path.
5. Thermal sense placeholders.
6. Then connect U_CTRL, logging header, and control/fault placeholders.

## 3. Canonical Net Rules

Use these global nets exactly:

Power and boundary references:

- PV_IN_POS
- CHG_OUT_POS
- BAT_BUS_POS
- PWR_NEG

Control and sensing nets:

- CTRL_PWM_MAIN
- CTRL_EN_CHG
- FAULT_OCP
- FAULT_OVP
- SENSE_PV_V
- SENSE_PV_I
- SENSE_BAT_V
- SENSE_BAT_I
- SENSE_TEMP_PWR
- SENSE_TEMP_BOARD
- UART_TX_LOG
- UART_RX_CFG

## 4. Minimal Connection Intent

1. U_CTRL outputs CTRL_PWM_MAIN and CTRL_EN_CHG.
2. U_CTRL inputs FAULT_OCP and FAULT_OVP (placeholder-safe if source not yet implemented).
3. TP_PV_V connects to SENSE_PV_V, sourced from a divider/tap on PV_IN_POS to PWR_NEG.
4. TP_BAT_V connects to SENSE_BAT_V, sourced from a divider/tap on BAT_BUS_POS to PWR_NEG.
5. TP_PV_I and TP_BAT_I connect to SENSE_PV_I and SENSE_BAT_I placeholders (final current-shunt topology deferred).
6. TH_PWR and TH_BOARD connect to SENSE_TEMP_PWR and SENSE_TEMP_BOARD.
7. J_LOG provides UART_TX_LOG, UART_RX_CFG, and PWR_NEG reference.

## 5. Required On-Zone Notes

Add these notes in Zone D:

- ASSUME: U_CTRL is a placeholder owner until DEC-005 control partition is closed.
- DECISION: DEC-005 remains deferred; only net/interface boundaries are being locked in this pass.
- VERIFY: Sense divider scaling and ADC range compatibility must be checked before part lock.
- VERIFY: Current-sense element placement and Kelvin routing strategy required before PCB routing.

## 6. Gate Constraints

- DEC-005 is still deferred, so avoid committing to a final controller IC architecture.
- Q-001 affects current-sense full-scale targets; keep current-sense component values provisional.
- Maintain global-label consistency across all zones on the single-sheet schematic.

## 7. Done Criteria For First Pass

- All Zone D nets exist as global labels and are connected to explicit placeholder owners.
- UART logging boundary is present with a return reference.
- Thermal sense placeholders exist for both power-stage and board-level monitoring.
- No duplicate local/global names on canonical nets.
