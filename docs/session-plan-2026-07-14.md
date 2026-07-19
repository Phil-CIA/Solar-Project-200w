# Solar Project 200W - Session Plan (2026-07-14)

Owners: Phil + Copilot  
Session intent: close DEC-013 Q-004 by selecting the actual Rev 0 CTRL_3V3 converter owner using battery-bus-native criteria and a new-chip candidate path.

## 1. Session Objective

Select and document the right control-supply converter for Rev 0 based on native battery-bus power behavior, especially low-voltage cutoff/startup constraints, then align schematic intent to one owner path.

## 2. Decision Baseline For This Session

- Input-source thinking is now battery-bus-first for control-rail requirements.
- U4-class wide-input behavior is preferred direction, but exact part number remains open.
- U2 can remain only if it passes the same battery-bus criteria and startup/cutoff requirements.
- Final choice is data-driven, not placeholder-driven.

## 3. Today Priorities (In Order)

1. Lock native control-power requirements from battery bus
- Define control-supply input envelope from BAT_BUS_POS, including:
  - low-voltage cutoff/startup floor,
  - nominal operating range,
  - high/charge-region ceiling,
  - transient and reverse-condition margin assumptions.
- Record target UVLO threshold and hysteresis behavior for safe restart.

2. Lock control-rail performance requirements
- Define CTRL_3V3 load budget (MCU + sensing + margin).
- Define startup requirement at battery low threshold.
- Define quiescent current goal for idle battery protection.
- Define ripple/noise acceptance target for ADC stability.

3. Build converter selection matrix
- Compare three paths minimum:
  - Current U2 path,
  - Current U4 path,
  - At least one new converter candidate.
- Score each path against weighted criteria (Section 5).
- Eliminate candidates that fail hard limits (Section 6) before weighted scoring.

4. Select single Rev 0 owner and document source behavior
- Choose one converter owner for CTRL_3V3.
- Capture source-priority behavior across CHG_OUT_POS, BAT_BUS_POS, and USB_5V_IN.
- Confirm no unintended USB backfeed path in intended architecture.

5. Update implementation docs for alignment
- Update decisions rationale and Q-004 closure notes.
- Update schematic notes and net-plan notes for one-owner path.
- Keep deferred ERC power-authority semantics on separate cleanup track.

## 4. Explicit Not-Today Boundaries

- Do not spend session time on broad ERC warning cleanup unless it blocks converter decision evidence.
- Do not force final procurement lock BOM for every support component.
- Do not expand into Rev 1 optimization architecture work.

## 5. Weighted Selection Criteria

Use this weighting for candidates that pass hard limits:

- 35%: Input-range and UVLO fit to battery-bus envelope
- 20%: Efficiency at expected operating points
- 15%: Quiescent current (battery-idle impact)
- 10%: Output ripple/noise and ADC friendliness
- 10%: Integration risk (compensation complexity, external parts, bring-up confidence)
- 10%: Availability and footprint/layout fit

Scoring rule:
- Score each category 1 to 5.
- Weighted total = sum(category_score x weight).
- Highest total wins after hard-limit pass.

## 6. Hard-Limit Pass/Fail Gates (Non-Negotiable)

Reject candidate immediately if any item fails:

1. Cannot start and regulate CTRL_3V3 at defined battery low threshold.
2. Cannot tolerate full defined BAT_BUS_POS high range with margin.
3. Lacks workable UVLO behavior for chosen cutoff strategy.
4. Creates unacceptable backfeed risk into USB_5V_IN boundary.
5. Requires unsupported complexity for Rev 0 schedule and bring-up confidence.

## 7. Deliverables Expected By End Of Session

1. Battery-bus-native control-power requirement block (with thresholds and assumptions).
2. Converter comparison matrix with pass/fail gates and weighted scores (tracked in `hardware/mppt/control-converter-selection-worksheet-2026-07-14.md`).
3. Selected Rev 0 CTRL_3V3 owner path (U2, U4, or new candidate) with rationale.
4. Updated decision/state notes reflecting Q-004 closure status.
5. Clear next-action list for schematic alignment and verification checks.

## 8. Definition Of Done For Today

- One converter owner path is selected using explicit criteria.
- Selection rationale is documented and traceable.
- Source-priority behavior is clearly stated for runtime and bench modes.
- No unresolved ambiguity remains about who owns CTRL_3V3 in Rev 0 intent.

## 9. Suggested Session Flow (Time-Boxed)

1. 25-35 min: battery-bus requirement and cutoff/UVLO lock
2. 30-45 min: candidate data capture (U2, U4, new part)
3. 30-40 min: hard-gate screening + weighted scoring
4. 20-30 min: owner selection and decision-note updates
5. 10-15 min: define schematic alignment actions and next-session handoff

## 10. Next Actions Immediately After Selection

1. Apply one-owner control-supply path in schematic intent notes.
2. Ensure CTRL_SUPPLY_IN and CTRL_3V3 ownership text is consistent across docs.
3. Queue startup verification checks:
- PV present, battery absent (if supported mode),
- battery present at low threshold,
- USB-only bench fallback,
- no reverse backfeed into USB path.
