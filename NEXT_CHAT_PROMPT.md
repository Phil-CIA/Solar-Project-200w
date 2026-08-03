Continue the Solar Project 200W repository from the latest SPICE handoff.

## Current priority

Close the SPICE capability-profile story while preserving the baseline PV-limited profile.

## State to assume

- Automation is working for sweep, logs, CSV summary, and waveform dashboard.
- Latest handoff: [SOLAR_HANDOFF_2026-07-30.md](SOLAR_HANDOFF_2026-07-30.md).
- B1 currently shows req_chg_001 PASS and req_chg_003 FAIL in [hardware/kicad/solar-project/spice/corner_summary.csv](hardware/kicad/solar-project/spice/corner_summary.csv).
- Working tree is dirty; avoid destructive cleanup.

## What to do first

1. Read [HANDOFF.md](HANDOFF.md) and [SOLAR_HANDOFF_2026-07-30.md](SOLAR_HANDOFF_2026-07-30.md).
2. Run git status --short and confirm baseline.
3. Run [hardware/kicad/solar-project/spice/run_spice_corner_sweeps.ps1](hardware/kicad/solar-project/spice/run_spice_corner_sweeps.ps1) with GeneratePlots false for a quick health check.
4. Verify [hardware/kicad/solar-project/spice/corner_summary.csv](hardware/kicad/solar-project/spice/corner_summary.csv) and [hardware/kicad/solar-project/spice/plots/index.html](hardware/kicad/solar-project/spice/plots/index.html).

## Session deliverables

1. Add and validate a dedicated 20A capability stress profile.
2. Clarify or correct B1 input-current sign convention and warning semantics.
3. Keep requirement reporting explicit per profile in summary outputs.

## Constraints

1. Keep autosave and lock artifacts out of commits.
2. Do not regress current sweep, parsing, and plot generation flow.

