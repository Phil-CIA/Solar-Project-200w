# Solar Project 200W - Decisions Log

Last updated: 2026-07-08
Decision owners: Phil + Copilot
Decision policy: move fast, log every non-trivial choice, and defer only with a target date.

## How We Use This Log

1. Record each meaningful technical or process choice the same day it is made.
2. If a decision is deferred, assign an owner and target decision date.
3. Link each decision to impacted requirements or architecture sections once those docs are created.
4. Review unresolved items at least once per week or at the start of any design session.

## Status Legend

- Decided: accepted baseline for current phase.
- Deferred: intentionally delayed until prerequisite information exists.
- Open: needs discussion now; blocks near-term progress.

## Decision Register

| ID | Date | Topic | Decision | Status | Rationale | Impact | Owner | Target Date |
|---|---|---|---|---|---|---|---|---|
| DEC-001 | 2026-07-06 | Planning approach | Use agile, short-cycle planning for concept phase with decision-first documentation. | Decided | Team size is two; high communication bandwidth supports rapid iteration. | Faster concept-to-design transition with explicit traceability. | Phil + Copilot | 2026-07-06 |
| DEC-002 | 2026-07-06 | Current phase scope | Complete planning docs before hardware or firmware implementation expansion. | Decided | Reduces rework and safety risk by clarifying constraints first. | Focuses effort on requirements, architecture, and decision clarity. | Phil + Copilot | 2026-07-06 |
| DEC-003 | 2026-07-08 | MPPT topology and revision scope | Lock Rev 0 to asynchronous buck implementation; defer synchronous buck and dedicated gate-driver architecture to Rev 1 planning. | Decided | Keeps current schematic and routing work stable while preserving a clear upgrade path after bring-up data. | Unblocks Rev 0 Sheet 2 completion and routing; creates explicit Rev 1 efficiency backlog. | Phil + Copilot | 2026-07-08 |
| DEC-004 | 2026-07-06 | Panel wiring strategy | Use two Renogy 100W panels in series as the Phase 1 design baseline. | Decided | Series reduces PV-side current, simplifies current handling, and aligns naturally with a buck-oriented charger path into a 4S LiFePO4 battery. | Establishes the working PV input envelope for MPPT design and protection sizing. | Phil + Copilot | 2026-07-06 |
| DEC-005 | 2026-07-06 | Charger control approach | Control strategy deferred (analog power control + MCU supervision vs integrated digital controller). | Deferred | Need architecture and telemetry boundary definition first. | Impacts firmware scope, BOM, and complexity. | Phil + Copilot | 2026-07-13 |
| DEC-006 | 2026-07-06 | Telemetry baseline | Start with local serial logging and reserve wireless integration as future extension. | Decided | Keeps bring-up simple while preserving growth path. | Simplifies early firmware and test infrastructure. | Phil + Copilot | 2026-07-06 |
| DEC-007 | 2026-07-06 | Safety-first gating | Require protection strategy coverage before design expansion (fusing, UVLO/OVP/OCP/OTP, reverse polarity, transient protection). | Decided | Safety constraints should be first-class requirements, not afterthoughts. | Guides requirements and architecture definitions immediately. | Phil + Copilot | 2026-07-06 |
| DEC-008 | 2026-07-06 | Validation style | Use staged bench bring-up with current-limited progression and logged checkpoints. | Decided | Reduces risk during early integration and debugging. | Affects future test plan and bring-up checklist structure. | Phil + Copilot | 2026-07-06 |
| DEC-009 | 2026-07-06 | Phase 0 exit | Declare Phase 0 complete once requirements, architecture, decisions, traceability, bring-up checklist, and validation plan are present. | Decided | Establishes a clear phase gate and prevents planning churn. | Enables transition to Phase 1 implementation exploration. | Phil + Copilot | 2026-07-06 |
| DEC-010 | 2026-07-06 | Phase 1 execution mode | Use rapid decision closure with conservative fallback assumptions when blocker data is unavailable in-session. | Decided | Maintains momentum while preserving safety boundaries and traceability. | Prevents schedule stall during early implementation exploration. | Phil + Copilot | 2026-07-06 |
| DEC-011 | 2026-07-06 | Temporary MPPT bounds | Apply temporary planning bounds: 15A continuous charge path assumption and dual panel-wiring envelope modeling until Q-001 and DEC-004 are closed. | Decided | Enables immediate schematic exploration while waiting for final battery and wiring constraints. | Allows Phase 1 design activity without unsafe implicit assumptions. | Phil + Copilot | 2026-07-06 |
| DEC-012 | 2026-07-08 | PV reverse-path optimization | Keep series Schottky reverse-polarity path for Rev 0; evaluate ideal-diode MOSFET replacement in Rev 1. | Deferred | Rev 0 prioritizes simple bring-up and lower integration risk. | Ideal-diode footprint and topology changes move to Rev 1 efficiency optimization work. | Phil + Copilot | 2026-07-20 |

## Open Questions

| ID | Date | Question | Why It Matters | Owner | Target Date |
|---|---|---|---|---|---|
| Q-001 | 2026-07-06 | What is the confirmed continuous and peak charge/discharge current supported by the selected 12.8V LiFePO4 pack and BMS? | Sets hard current limits for charger and inverter operating envelopes. | Phil + Copilot | 2026-07-13 |
| Q-002 | 2026-07-06 | What minimum instrumentation is available for bring-up (bench supply, DMM count, current probe, scope channels)? | Determines achievable verification depth in early test plans. | Phil + Copilot | 2026-07-13 |
| Q-003 | 2026-07-08 | Should Rev 1 replace the Rev 0 series Schottky PV reverse-polarity stage with an ideal-diode MOSFET block? | Defines the next-revision efficiency path and required schematic/footprint migration work. | Phil + Copilot | 2026-07-20 |

## Change Log

- 2026-07-08: Locked converter revision scope: Rev 0 asynchronous buck baseline; synchronous converter and gate-driver architecture deferred to Rev 1.
- 2026-07-08: Reframed DEC-012/Q-003 as Rev 1 optimization items and removed them as Rev 0 pre-routing blockers.
- 2026-07-07: Added Sheet 2 buck power-stage component picks (Q1, D3, L1, C4-C7, R1/C8) with starter electrical targets for immediate symbol placement.
- 2026-07-07: Added Sheet 2 buck power-stage wiring map and extended net plan with implementation nets for protected input and gate-drive path.
- 2026-07-07: Added PV input wiring map with explicit net-by-net symbol connectivity for Sheet 1 implementation.
- 2026-07-07: Added DEC-012/Q-003 to track ideal-diode upgrade work.
- 2026-07-07: Chosen first-pass PV input stage parts: 2-pin terminal block, 10 A fuse, series Schottky reverse protection, 64 V TVS, and 100 V input capacitors.
- 2026-07-07: Added buck symbol placement checklist to drive the first KiCad schematic draw order.
- 2026-07-06: Added GitHub reference review notes for MPPT layout and schematic study, with usage constraints to avoid blind copying.
- 2026-07-06: Added buck topology decision notes and explicit low-irradiance/headroom checks before fully closing DEC-003.
- 2026-07-06: Closed DEC-004 with series panel wiring baseline and narrowed DEC-003 to a buck-preferred path pending controller voltage-class confirmation.
- 2026-07-06: Captured Renogy 100W N-Type panel electrical data in the MPPT envelope worksheet and interface assumptions for DEC-003/DEC-004 analysis.
- 2026-07-06: Added MPPT envelope decision worksheet and decision update template to accelerate DEC-003/DEC-004 closure.
- 2026-07-06: Added KiCad implementation checklist to enforce draw-annotate-verify save-point workflow for MPPT exploration.
- 2026-07-06: Added MPPT schematic execution scaffold (schematic-notes, net-plan, component-shortlist-template) for rapid KiCad start.
- 2026-07-06: Added MPPT first-pass block diagram with requirement/decision mapping to start Phase 1 schematic exploration.
- 2026-07-06: Added MPPT hardware kickoff artifacts and DEC-011 temporary bounds policy for active exploration.
- 2026-07-06: Added Phase 1 kickoff plan and DEC-010 rapid execution policy with conservative fallback rules.
- 2026-07-06: Added Phase 0 exit decision (DEC-009), validation-plan, and phase-0-exit-review artifacts.
- 2026-07-06: Added compact requirements traceability matrix and initial test/bringup-checklist tied to REQ-SAF-006 and REQ-VAL-001.
- 2026-07-06: Added `requirements.md` and `architecture.md`; decision references now mapped for Phase 0.
- 2026-07-06: Initial log created from handoff baseline and concept open decisions.
