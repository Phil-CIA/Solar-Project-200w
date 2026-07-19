# MPPT KiCad Implementation Checklist

Last updated: 2026-07-14
Owners: Phil + Copilot
Purpose: provide a repeatable sequence for building the first MPPT schematic safely and traceably.

## 1. Session Start Gate

- [ ] Open and review hardware/mppt/block-diagram.md.
- [ ] Open and review hardware/mppt/schematic-notes.md.
- [ ] Open and review hardware/mppt/buck-schematic-parts.md.
- [ ] Open and review hardware/mppt/net-plan.md.
- [ ] Confirm current temporary bounds and open items in docs/phase-1-kickoff.md.
- [ ] Confirm decision status for DEC-003 and DEC-005 in docs/decisions-log.md.

## 2. Draw Order (Per Session)

1. Draw PV boundary and protection blocks first.
2. Draw MPPT conversion core and output boundary second.
3. Draw battery interface and fuse handoff notes third.
4. Draw sensing and telemetry I/O paths last.

Rule:
- Do not start symbol-level optimization before all four areas are present at block connectivity level.

## 3. Annotation Pass

- [ ] Add ASSUME notes for temporary bounds.
- [ ] Add DECISION notes for unresolved decisions (DEC-003, DEC-005).
- [ ] Add SAFETY notes at PV protection, conversion protection, and battery handoff boundaries.
- [ ] Add VERIFY notes for each measurement-critical node.

## 4. Net Naming Pass

- [ ] Apply power net names from net-plan.md.
- [ ] Apply sensing and control net names from net-plan.md.
- [ ] Verify no duplicate or ambiguous names on critical nets.
- [ ] If a new net is introduced, add it to net-plan.md the same session.

## 5. Verification Pass Before Save

- [ ] Each high-current path has explicit forward and return path.
- [ ] UVLO/OVP/OCP/OTP ownership is represented in notes or blocks.
- [ ] PV reverse polarity and transient handling are present.
- [ ] Buck-specific parts are represented: switch, freewheel path, inductor, output capacitors, control placeholder.
- [ ] Required sense points exist: PV V/I, BAT V/I, thermal points.
- [ ] Serial logging boundary exists for bring-up telemetry.
- [ ] Control-power boundary is explicit (`CTRL_SUPPLY_IN`, `CTRL_3V3`, optional `CTRL_3V3A`) and tied to DEC-013 intent.
- [ ] USB bench fallback path is protected against reverse current when runtime rail is active.
- [ ] Any intentionally unused pins touched in this pass are marked with explicit `no_connect` markers.
- [ ] New active rails introduced in this pass include explicit power authority markers (power flag or clear power-output ownership).

## 5.1 ERC Triage Rule During Implementation

- [ ] Fix high-value ERC findings immediately: `multiple_net_names`, unconnected wire endpoints in active zones, and missing ownership on fault/control nets.
- [ ] Defer low-value cleanup findings (footprint library noise, placeholder warnings) to end-of-phase cleanup unless they hide functional risk.
- [ ] Record deferred ERC items with rationale in `hardware/mppt/schematic-notes.md`.

## 6. Save-Point Protocol

At each save-point:
- [ ] Add a short note in hardware/mppt/schematic-notes.md describing what changed.
- [ ] If assumptions changed, update hardware/mppt/interface-assumptions.md.
- [ ] If trade-offs were made, add a dated entry or update in docs/decisions-log.md.

## 7. End-of-Session Exit Gate

- [ ] Unresolved blockers are listed with owner and next target date.
- [ ] Current schematic state is compatible with REQ-CHG-001/002/003 and REQ-SAF-003/004 intent.
- [ ] No hidden safety-critical assumptions remain unannotated.
- [ ] Next session first action is captured at bottom of this file.

## 8. Pre-PCB Routing Gate

- [ ] Rev 0 reverse-polarity path is fixed to series Schottky and reflected in symbols/footprints.
- [ ] Rev 0 Sheet 2 freewheel path is asynchronous and reflected in symbols/notes.
- [ ] Any ideal-diode or synchronous upgrade is captured as Rev 1 backlog in docs/decisions-log.md.
- [ ] DEC-013 startup behavior is validated: PV-only startup and USB-only startup both pass with no unintended backfeed.
- [ ] Full ERC rabbit-hole pass is completed: all remaining findings are either fixed or explicitly waived with rationale.

## 9. Pre-Rev 1 Planning Gate

- [ ] Revisit DEC-012 and Q-003 with measured Rev 0 thermal and efficiency data.
- [ ] Define migration impact for ideal-diode input path (symbols, footprints, routing).
- [ ] Define migration impact for synchronous power stage and gate-driver architecture.

## 10. Next Session First Action

Fill this line before ending each session:
- Next action: **Close Q-004 for DEC-013**. Decide whether U2 remains the Rev 0 `CTRL_3V3` owner or U4 is promoted to the actual wide-input control-supply path, then update the schematic and notes to match that single-owner choice.
