# MPPT Component Shortlist Template

Last updated: 2026-07-06
Purpose: capture candidate parts without premature lock-in.

## 1. Usage Rules

- Add at least two candidates for each critical function.
- Record rationale for preferred candidate and why alternatives were rejected.
- Do not mark "selected" until DEC-003/004/005 constraints are compatible.

## 2. Candidate Table

| Function | Candidate Part | Key Ratings | Pros | Risks | Status | Notes |
|---|---|---|---|---|---|---|
| Controller / control IC | TBD | TBD | TBD | TBD | Open | Depends on DEC-005 |
| Main switch element | TBD | TBD | TBD | TBD | Open | Depends on DEC-003 |
| Freewheel / sync element | TBD | TBD | TBD | TBD | Open | Depends on DEC-003 |
| Inductor | TBD | TBD | TBD | TBD | Open | Bound to ripple/current assumptions |
| Input protection device | TBD | TBD | TBD | TBD | Open | Must satisfy REQ-SAF-004 |
| Transient suppression device | TBD | TBD | TBD | TBD | Open | Must satisfy REQ-SAF-004 |
| Current sense element | TBD | TBD | TBD | TBD | Open | Must satisfy REQ-TEL-002 |
| Voltage sense divider path | TBD | TBD | TBD | TBD | Open | Must satisfy REQ-TEL-002 |
| Temperature sensor | TBD | TBD | TBD | TBD | Open | Must satisfy REQ-TEL-002 |

## 3. Shortlist Exit Criteria

- At least two viable options listed for each critical function.
- Risks and assumptions are documented per candidate.
- Preferred options identified but not committed until constraints close.
