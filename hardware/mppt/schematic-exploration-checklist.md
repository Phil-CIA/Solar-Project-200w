# MPPT Schematic Exploration Checklist

Last updated: 2026-07-06
Owners: Phil + Copilot
Purpose: define a safe, traceable first-pass MPPT schematic exploration flow.

## 1. Inputs and Constraints

- [ ] Confirm current constraint snapshot from docs/phase-1-kickoff.md.
- [ ] Confirm relevant requirement IDs from docs/requirements.md (REQ-CHG-001/002/003, REQ-SAF-003/004, REQ-TEL-002).
- [ ] Confirm deferred decisions status in docs/decisions-log.md (DEC-003/004/005).

## 2. Topology and Envelope Framing

- [ ] Capture candidate topology options (buck, buck-boost) with one-line rationale each.
- [ ] Capture PV input voltage/current envelope assumptions.
- [ ] Capture battery-side voltage/current envelope assumptions.
- [ ] Record chosen temporary envelope bounds if final data is still open.

## 3. Power Stage Draft

- [ ] Draw block-level power stage path (input filter, switch stage, inductor, output filter).
- [ ] Allocate provisional switch and diode/synchronous element stress points.
- [ ] Allocate provisional inductor current ripple target.
- [ ] Allocate provisional thermal observation points.

## 4. Protection Allocation

- [ ] Place reverse polarity strategy on PV boundary.
- [ ] Place transient suppression strategy on PV boundary.
- [ ] Allocate UVLO/OVP/OCP/OTP ownership across controller/power path.
- [ ] Mark fuse/protection boundaries that connect to battery and distribution domains.

## 5. Sensing and Telemetry Allocation

- [ ] Place PV voltage/current measurement points.
- [ ] Place battery voltage/current measurement points.
- [ ] Place at least two thermal sensing points.
- [ ] Define baseline serial logging signal set for bring-up.

## 6. Design Review Gate

- [ ] Cross-check each block against mapped requirement IDs.
- [ ] Log any unresolved blockers as decision-log entries with owner and target date.
- [ ] Mark scope boundary between exploration-only and part-lock decisions.
- [ ] Confirm no safety-critical control is unassigned.

## 7. Exit Criteria

- [ ] First-pass MPPT schematic exploration diagram exists.
- [ ] Interfaces and protection ownership are explicitly marked.
- [ ] Deferred decisions are either closed or re-dated with rationale.
- [ ] Ready to start component-level shortlist work.
