# Sheet 4 Sensing and Control Wiring Map

Last updated: 2026-07-10
Owners: Phil + Copilot
Scope: define first-pass, implementation-ready zone plan for sensing and control I/O in the single-sheet KiCad schematic, starting with voltage and current measuring circuits.

## 1. Purpose

Start Zone D (Sheet 4 intent) with stable net boundaries and placeholders, beginning with voltage and current measuring circuits and now adding a provisional STM32G431 pin-map baseline.

## 2. First-Pass Symbol Set

| Ref | Function | First-Pass Type | Notes |
|---|---|---|---|
| U_CTRL | Control placeholder block | Generic control symbol or hierarchical placeholder | Use as owner for control and telemetry nets; map signals to STM32G431 candidates in Section 8 |
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

- ASSUME: U_CTRL remains a schematic owner placeholder until exact STM32G431 package and footprint are locked.
- DECISION: DEC-005 is now closed to STM32G431 for Rev 0, with revision-flexible re-evaluation allowed in future revs.
- VERIFY: Sense divider scaling and ADC range compatibility must be checked before part lock.
- VERIFY: Current-sense element placement and Kelvin routing strategy required before PCB routing.

## 6. Gate Constraints

- DEC-005 is closed for Rev 0 baseline (STM32G431), but exact package/pin lock still requires variant verification.
- Q-001 affects current-sense full-scale targets; keep current-sense component values provisional.
- Maintain global-label consistency across all zones on the single-sheet schematic.

## 7. Done Criteria For First Pass

- All Zone D nets exist as global labels and are connected to explicit placeholder owners.
- UART logging boundary is present with a return reference.
- Thermal sense placeholders exist for both power-stage and board-level monitoring.
- No duplicate local/global names on canonical nets.

## 8. STM32G431 Rev 0 Interface Mapping (Provisional)

Purpose:
- Replace generic U_CTRL ownership with a concrete Rev 0 MCU interface direction.
- Keep mapping provisional until exact package/footprint lock.

| Zone D Net | STM32G431 Role | Candidate Pin |
|---|---|---|
| SENSE_PV_V | ADC | PA0 |
| SENSE_PV_I | ADC | PA1 |
| SENSE_BAT_V | ADC | PA4 |
| SENSE_BAT_I | ADC | PA5 |
| SENSE_TEMP_PWR | ADC | PA6 |
| SENSE_TEMP_BOARD | ADC | PA7 |
| CTRL_PWM_MAIN | TIM1 PWM | PA8 |
| CTRL_EN_CHG | GPIO out | PB5 |
| FAULT_OCP | GPIO in / EXTI | PB0 |
| FAULT_OVP | GPIO in / EXTI | PB1 |
| UART_TX_LOG | USART1_TX | PA9 |
| UART_RX_CFG | USART1_RX | PA10 |

Notes:
- SWD and reset pins are mandatory and must remain reserved.
- Final pin assignment can move if required by package availability or routing quality.
- Mapping supports Rev 0 prototype bring-up and remains revision-flexible.
