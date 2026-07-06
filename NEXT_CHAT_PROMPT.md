Continue the Solar Project 200W repository from the current baseline.

Known state:
- Repo scaffold exists and is committed.
- KiCad project sources are present under `hardware/kicad/solar-project/`.
- Concept baseline is documented in `SOLAR_PROJECT_CONCEPT_AND_HANDOFF.md`.

Tasks:
1) Create planning docs:
   - `docs/requirements.md`
   - `docs/architecture.md`
   - `docs/decisions-log.md`
2) In `docs/requirements.md`, define measurable targets and constraints for:
   - PV input (200W class)
   - battery and charging (12.8V LiFePO4, MPPT 15-20A target)
   - inverter envelope (300W target)
   - telemetry and safety requirements
3) In `docs/architecture.md`, define:
   - block diagram narrative
   - power path assumptions
   - control/telemetry boundaries
   - protection strategy outline
4) In `docs/decisions-log.md`, add initial decisions and open questions with date and rationale.
5) Keep all content concise, engineer-oriented, and traceable to testable requirements.
