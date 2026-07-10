# Solar Project 200W - Session Plan (2026-07-10)

Owners: Phil + Copilot  
Session intent: continue schematic development and component selection without forcing premature control-IC lock, while preparing a clean STM32 path.

## 1. Session Objective

Close as much Rev 0 schematic/component work as possible today, and define a data-driven gate for selecting STM32 at the right time.

## 2. Today Priorities (In Order)

1. Full schematic requirements validation pass
- Cross-check current schematic against REQ-CHG-001/002/003 and REQ-SAF-003/004.
- Confirm all canonical nets and boundary notes remain consistent.
- Log any requirement gaps in `hardware/mppt/schematic-notes.md`.

2. Rev 0 component shortlist lock for currently drawn blocks
- Finalize candidate parts for Q1, D3, L1, C4-C7, divider resistors, and protection parts.
- For each selected part, record: voltage/current/thermal margin, package, and sourcing risk.
- Keep current-sense and final controller IC as deferred placeholders if envelope data is still missing.

3. PCB-readiness gate review (pre-routing prep, not full routing)
- Confirm high-current forward/return paths are explicit and review placement intent notes.
- Confirm measurement path constraints (especially future Kelvin routing requirements).
- Identify any symbol/footprint blockers before PCB placement starts.

4. STM32 decision-prep work (without forcing DEC-005 closure today)
- Create STM32 candidate shortlist (for example STM32G4/F3/F0 class) based on:
  - ADC channel count and resolution needs
  - PWM timer capability for buck control
  - UART availability for logging/config
  - 3.3V I/O compatibility with current sensing/control net assumptions
- Define a decision gate checklist for DEC-005 closure in a follow-up session.

## 3. Explicit Not-Today Boundaries

- Do not force final DEC-005 closure unless all prerequisite data is available.
- Do not commit to final current-sense topology until Q-001 envelope is confirmed.
- Do not begin optimization-heavy Rev 1 architecture work.

## 4. Deliverables Expected By End Of Session

1. Updated requirement-validation notes with pass/gap status.
2. Updated component shortlist with at least one viable candidate per Rev 0 placed function block.
3. PCB-readiness blocker list (empty is acceptable if fully clean).
4. Draft STM32 decision gate checklist and candidate table.
5. Updated `docs/decisions-log.md` if any decision status/date changes.

## 5. Definition Of Done For Today

- No unresolved safety-critical schematic ambiguity for Rev 0 boundaries.
- Rev 0 component selection has documented rationale and sourcing sanity check.
- STM32 path is prepared with clear criteria and no premature lock-in risk.

## 6. Suggested Session Flow (Time-Boxed)

1. 30-45 min: requirements/schematic validation sweep
2. 45-75 min: component shortlist closure for Rev 0 blocks
3. 30-45 min: PCB-readiness and blockers review
4. 30-45 min: STM32 candidate + decision-gate draft
5. 10-15 min: update notes, decisions log, and next-action handoff

## 7. STM32G431 First-Pass Pin Budget (Rev 0)

Decision context:
- DEC-005 direction is now STM32G431 for Rev 0 prototype control.
- MCU choice is revision-flexible and can change in future revs.

### 7.1 Net-to-MCU Mapping (First Pass)

| Project Net | MCU Function | Candidate Pin | Notes |
|---|---|---|---|
| SENSE_PV_V | ADC input | PA0 (ADC1_IN1) | PV voltage divider input |
| SENSE_PV_I | ADC input | PA1 (ADC1_IN2) | PV current sense input |
| SENSE_BAT_V | ADC input | PA4 (ADC1_IN5) | Battery voltage divider input |
| SENSE_BAT_I | ADC input | PA5 (ADC1_IN6) | Battery current sense input |
| SENSE_TEMP_PWR | ADC input | PA6 (ADC2-capable on many G431 variants) | Power-stage thermal channel |
| SENSE_TEMP_BOARD | ADC input | PA7 (ADC2-capable on many G431 variants) | Board thermal channel |
| CTRL_PWM_MAIN | PWM output | PA8 (TIM1_CH1) | Main buck PWM output |
| CTRL_EN_CHG | GPIO output | PB5 | Charger enable control |
| FAULT_OCP | GPIO input + EXTI | PB0 | Over-current fault input |
| FAULT_OVP | GPIO input + EXTI | PB1 | Over-voltage fault input |
| UART_TX_LOG | UART TX | PA9 (USART1_TX) | Logging TX |
| UART_RX_CFG | UART RX | PA10 (USART1_RX) | Config RX |

### 7.2 Reserved/Required Pins

- SWDIO: PA13
- SWCLK: PA14
- NRST: reset pin
- Keep at least 2 spare ADC channels for calibration/future sensing
- Keep at least 1 spare timer-capable output for future Rev 1 experiments

### 7.3 Validation Before Schematic Lock

- Confirm exact STM32G431 package variant and alternate-function compatibility.
- Confirm ADC pin availability is unchanged after SWD/UART/debug reservations.
- Confirm timer channel choice aligns with intended PWM frequency and control strategy.
