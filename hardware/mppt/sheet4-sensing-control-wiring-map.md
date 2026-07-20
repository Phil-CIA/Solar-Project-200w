# Sheet 4 Sensing and Control Wiring Map

Last updated: 2026-07-14
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
| TH_BOARD | Board thermal sensor placeholder | NTC or sensor placeholder symbol | Bind to SENSE_TEMP_BOARD; U7 is the current board-temp device placeholder |
| U8 | Display module | 0.96in OLED I2C module | Connect to I2C CLK/DATA, CTRL_3V3, and GND |
| J_LOG | Serial logging header | 1x03 or 1x04 header | Include TX, RX, and return reference |
| U_PWR_CTRL | Control-rail regulator block | Wide-input buck + optional post-filter stage | Own `CTRL_SUPPLY_IN` to `CTRL_3V3` conversion per DEC-013 |
| J_USB_DBG | USB bench-power boundary | USB connector or 5V header placeholder | Bench fallback source for control rail when PV is absent |

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
- CTRL_SUPPLY_IN
- USB_5V_IN
- CTRL_3V3
- CTRL_3V3A

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
8. U_PWR_CTRL converts `CTRL_SUPPLY_IN` to `CTRL_3V3` and feeds MCU supply pins.
9. `USB_5V_IN` enters only through protected bench-fallback path; no direct backfeed into USB boundary is allowed.

Current schematic state note:
- The drawn schematic presently uses U2 as the provisional `CTRL_3V3` owner.
- U4 now references `CTRL_SUPPLY_IN` and represents the intended wide-input candidate path, but it is not yet wired as the active `CTRL_3V3` source.
- Q-004 exists to decide which block becomes the actual Rev 0 control-rail owner.

## 5. Required On-Zone Notes

Add these notes in Zone D:

- ASSUME: U_CTRL remains a schematic owner placeholder until exact STM32G431 package and footprint are locked.
- DECISION: DEC-005 is now closed to STM32G431 for Rev 0, with revision-flexible re-evaluation allowed in future revs.
- DECISION: DEC-013 fixes Rev 0 control-power architecture to single wide-input buck primary path with PV-first runtime behavior and protected USB bench fallback.
- VERIFY: Sense divider scaling and ADC range compatibility must be checked before part lock.
- VERIFY: Current-sense element placement and Kelvin routing strategy required before PCB routing.
- VERIFY: Startup checks must pass for PV-only and USB-only bench modes before pre-routing gate closure.

## 6. Gate Constraints

- DEC-005 is closed for Rev 0 baseline (STM32G431), but exact package/pin lock still requires variant verification.
- Q-001 affects current-sense full-scale targets; keep current-sense component values provisional.
- Maintain global-label consistency across all zones on the single-sheet schematic.

## 7. Done Criteria For First Pass

- All Zone D nets exist as global labels and are connected to explicit placeholder owners.
- UART logging boundary is present with a return reference.
- Thermal sense placeholders exist for both power-stage and board-level monitoring.
- No duplicate local/global names on canonical nets.

## 8. STM32G431 Rev 0 Interface Mapping (Audited 2026-07-19)

Purpose:
- Replace the earlier first-pass budget with the live U1 wiring audit from the current schematic.
- Normalize MCU-facing nets to canonical names where the function is unambiguous.

| Status | Zone D Net | STM32G431 Role | Live Pin | Notes |
|---|---|---|---|---|
| Canonicalized | SENSE_PV_V | ADC | PA4 | Live schematic renamed from `V_in_ADC`; sourced from the PV divider on `/PV_IN_POS_FUSED`. |
| Legacy hold (role clarified) | V_in_ON | GPIO output to legacy gate-control path | PA5 | Confirmed as control-only path (PA5 -> R41 -> Q2 gate net), not an ADC channel. Keep name unchanged until the U2/Q2 legacy path is either removed or intentionally retained. |
| Canonical | SENSE_TEMP_PWR | ADC | PA6 | Power-stage thermal input. |
| Canonical | SENSE_TEMP_BOARD | ADC | PA7 | Board thermal input. |
| Canonicalized | CTRL_PWM_MAIN | TIM1 PWM / gate-drive command | PA8 | Live schematic renamed from `V_out _on`; this is the MCU-side gate-drive command path. |
| Canonicalized | UART_TX_LOG | USART1_TX | PA9 | Bridge-side label canonicalized from `TXD`. |
| Canonicalized | UART_RX_CFG | USART1_RX | PA10 | Bridge-side label canonicalized from `RXD`. |
| Canonical | FAULT_OCP | GPIO in / EXTI | PB0 | Over-current fault input. |
| Canonical | FAULT_OVP | GPIO in / EXTI | PB1 | Over-voltage fault input. |
| Canonical | CTRL_EN_CHG | GPIO out | PB5 | Charger enable control. |
| Canonical | I2C DATA | I2C SDA | PB6 | Used for LM51772 configuration/control in the current schematic. |
| Canonical | I2C CLK | I2C SCL | PB7 | Used for LM51772 configuration/control in the current schematic. |

Reserved pins:
- PA13: SWDIO
- PA14: SWCLK
- PG10/~RST: hardware reset

Deferred pin-allocation items:
- PA0, PA1, PA2, and PA3 are still free in the live schematic and are the best candidates for the missing canonical measurement channels (`SENSE_BAT_V`, `SENSE_PV_I`, `SENSE_BAT_I`) once those sensing front ends are finalized.
- PC13 is the preferred landing point for `U6_FLT` once the pull-up is added.
- U7 is currently intended as the board temperature sensor, placed near the warmest representative board spot, and should be tied to `SENSE_TEMP_BOARD`.
- U8 is the display module and should remain on the I2C control bus with the control rail and ground return.

Notes:
- This audited table supersedes the older provisional pin budget as the practical Rev 0 reference.
- Canonical cleanup is intentionally limited to MCU-facing and boundary-facing nets in this pass. U6-local helper nets such as `_OUTPUT Feedback`, `_Undervoltage Feedback`, `_CSA_2`, and `_CSB_2` still need a separate cleanup decision if you want them renamed too.
