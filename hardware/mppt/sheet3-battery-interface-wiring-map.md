# Sheet 3 Battery Interface Wiring Map

Last updated: 2026-07-08
Owners: Phil + Copilot
Scope: define first-pass, placement-ready wiring intent for Sheet 3 battery interface and protection handoff.

## 1. Purpose

Provide a minimal, safe Sheet 3 implementation path that can start now while Q-001 remains open.

## 2. Symbol Set For First Pass

| Ref | Function | First-Pass Type | Notes |
|---|---|---|---|
| J2 | Battery connector boundary | 2-pin terminal or battery connector symbol | Interface handoff to pack/BMS domain |
| F2 | Main battery fuse boundary | Fuse symbol placeholder | Final rating depends on Q-001 closure |
| TP_BAT_POS | Positive bus test point | Test point symbol | Optional but recommended for bring-up |
| TP_BAT_NEG | Negative bus test point | Test point symbol | Optional but recommended for bring-up |
| NOTE_BAT_BOUND | Boundary note | Text note | Capture assumptions and safety ownership |

## 3. Net Handoff Rules

Use these nets exactly as named:

- CHG_OUT_POS enters Sheet 3 from Sheet 2.
- CHG_OUT_NEG enters Sheet 3 from Sheet 2.
- BAT_BUS_POS leaves Sheet 3 toward battery and distribution domains.
- BAT_BUS_NEG leaves Sheet 3 toward battery and distribution domains.

Connection intent:

1. CHG_OUT_POS -> F2 -> BAT_BUS_POS.
2. CHG_OUT_NEG -> BAT_BUS_NEG (direct return path).
3. J2 pin 1 -> BAT_BUS_POS.
4. J2 pin 2 -> BAT_BUS_NEG.
5. TP_BAT_POS on BAT_BUS_POS and TP_BAT_NEG on BAT_BUS_NEG.

## 4. Required On-Sheet Notes

Add these notes on Sheet 3:

- ASSUME: 15A continuous planning bound until Q-001 is closed.
- SAFETY: Main battery fuse F2 must be physically placed near battery positive terminal in layout.
- VERIFY: Confirm fuse rating and I2t against final BMS continuous and peak current limits.
- VERIFY: Confirm connector current and creepage/clearance margins for battery domain.

## 5. Current Gate Constraints

- Q-001 remains open, so do not lock final F2 rating yet.
- DEC-005 does not block Sheet 3 boundary placement.
- Keep Sheet 3 focused on power boundary and protection handoff only.

## 6. Done Criteria For Sheet 3 First Pass

- J2 and F2 are placed and wired with explicit forward and return paths.
- BAT_BUS_POS and BAT_BUS_NEG are present as canonical boundary nets.
- Assumption and safety notes are visible on the sheet.
- No ambiguous duplicate names on the battery boundary nets.
