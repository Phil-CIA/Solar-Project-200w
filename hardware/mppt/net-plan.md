# MPPT Net Plan (Draft)

Last updated: 2026-07-08
Purpose: keep schematic net naming consistent and traceable from day one.

## 1. Naming Conventions

- Power nets: ALL_CAPS with domain and polarity suffix.
- Sense nets: SENSE_<DOMAIN>_<MEASURED>
- Control nets: CTRL_<FUNCTION>
- Fault nets: FAULT_<TYPE>
- Logging nets: UART_<DIR>_<ROLE>
- KiCad implementation style: use global labels for canonical inter-zone nets in the single-sheet schematic.
- Layout style: rectangles are used as functional zones (Sheet 1/2/3/4 intent), not separate hierarchical sheets.

## 2. Candidate Net List

| Net Name | Domain | Direction | Purpose | Requirement Link |
|---|---|---|---|---|
| PV_IN_POS_RAW | PV Input | In | Raw positive input before fuse and reverse protection | REQ-SAF-004 |
| PV_IN_POS_FUSED | PV Input | Internal | Positive input after fuse and before reverse protection element | REQ-SAF-004 |
| PV_IN_POS | PV Input | In | Positive PV input rail | REQ-PV-001 |
| PWR_NEG | Shared Return | Bidirectional | Canonical shared negative return rail across PV, charger output, and battery interface | REQ-PV-001, REQ-CHG-001, REQ-BAT-001 |
| MPPT_SW_NODE | Power Stage | Internal | Main switching node for conversion stage | REQ-CHG-001 |
| CHG_OUT_POS | Charger Output | Out | Charger positive output to battery interface | REQ-CHG-001 |
| BAT_BUS_POS | Battery Bus | Bidirectional | Battery positive bus boundary | REQ-BAT-001 |
| SENSE_PV_V | Sensing | In | PV voltage feedback channel | REQ-TEL-002 |
| SENSE_PV_I | Sensing | In | PV current feedback channel | REQ-TEL-002 |
| SENSE_BAT_V | Sensing | In | Battery voltage feedback channel | REQ-TEL-002 |
| SENSE_BAT_I | Sensing | In | Battery current feedback channel | REQ-TEL-002 |
| SENSE_TEMP_PWR | Sensing | In | Power stage thermal monitor | REQ-TEL-002 |
| SENSE_TEMP_BOARD | Sensing | In | Board ambient/control thermal monitor | REQ-TEL-002 |
| CTRL_PWM_MAIN | Control | Out | Main PWM drive/control command | REQ-CHG-001 |
| GATE_Q1 | Control | Out | Gate drive net to buck high-side MOSFET | REQ-CHG-001 |
| CTRL_EN_CHG | Control | Out | Charger enable/disable control | REQ-SAF-003 |
| FAULT_OCP | Protection | Out | Over-current fault signal | REQ-SAF-003 |
| FAULT_OVP | Protection | Out | Over-voltage fault signal | REQ-SAF-003 |
| UART_TX_LOG | Telemetry | Out | Serial logging transmitter | REQ-TEL-001 |
| UART_RX_CFG | Telemetry | In | Serial configuration/command receiver | REQ-TEL-001 |

## 3. Notes

- Final net names may evolve, but ID continuity should be preserved where possible.
- If a net name changes, update this file and record the reason in decisions-log.
- Current canonical negative return net is `PWR_NEG`; legacy per-zone negative names are retired for this single-sheet implementation.
