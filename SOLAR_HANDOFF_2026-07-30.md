# Solar Project 200W - SPICE Handoff (2026-07-30)

## Session outcome

SPICE simulation flow is now repeatable with automated summaries and visuals, and a capability-oriented B1 operating point was tuned to exceed the 15A minimum current target.

## What was completed

1. Stable automated sweep and parsing flow:
- Runner: [hardware/kicad/solar-project/spice/run_spice_corner_sweeps.ps1](hardware/kicad/solar-project/spice/run_spice_corner_sweeps.ps1)
- Summary output: [hardware/kicad/solar-project/spice/corner_summary.csv](hardware/kicad/solar-project/spice/corner_summary.csv)
- Per-deck logs: [hardware/kicad/solar-project/spice/logs](hardware/kicad/solar-project/spice/logs)

2. Automated waveform export and dashboard:
- Waveforms: [hardware/kicad/solar-project/spice/waveforms](hardware/kicad/solar-project/spice/waveforms)
- Dashboard generator: [hardware/kicad/solar-project/spice/generate_spice_plots.ps1](hardware/kicad/solar-project/spice/generate_spice_plots.ps1)
- Dashboard output: [hardware/kicad/solar-project/spice/plots/index.html](hardware/kicad/solar-project/spice/plots/index.html)

3. Integration stability fixes:
- Removed duplicate ideal branch references by replacing block-local ground reference voltage sources with high-conductance resistive ties in:
  - [hardware/kicad/solar-project/spice/01_block_b1_power_stage.cir](hardware/kicad/solar-project/spice/01_block_b1_power_stage.cir)
  - [hardware/kicad/solar-project/spice/02_block_b2_battery_boundary.cir](hardware/kicad/solar-project/spice/02_block_b2_battery_boundary.cir)

4. Requirement-aware summary fields:
- Added B1 requirement status columns in CSV:
  - req_chg_001: 15A minimum capability check
  - req_chg_003: 12A to 13A expected baseline band check

## Latest verified results

From [hardware/kicad/solar-project/spice/corner_summary.csv](hardware/kicad/solar-project/spice/corner_summary.csv):

- 01_block_b1_power_stage.cir:
  - status: ok
  - vout_avg: 1.38821e+01
  - iout_avg: 1.54246e+01
  - pin_avg: 2.53541e+02
  - pout_avg: 2.14125e+02
  - eff_pct: 8.44538e+01
  - req_chg_001: PASS
  - req_chg_003: FAIL
  - notes warning: iin_avg_negative

- 90_system_integration_harness.cir:
  - status: ok
  - waveform file present in [hardware/kicad/solar-project/spice/waveforms](hardware/kicad/solar-project/spice/waveforms)

- 04_block_b4_fault_network.cir:
  - bat_trip and ocp_trip are consistently parsed into CSV

## Interpretation agreed in session

1. Current major limiter for real operation is PV input power budget, not path duty control.
2. Team accepts PV input power as a system constraint.
3. Current path is intended to handle up to 20A capability class when source power supports it.

## Open technical items for next return

1. Define and run a dedicated 20A path-capability stress profile (separate from 200W PV-limited operating profile).
2. Resolve B1 input current sign convention so iin_avg_negative is either expected-by-definition or removed via corrected measurement orientation.
3. Expand requirement mapping beyond current REQ-CHG-001 and REQ-CHG-003 checks if needed.
4. Optionally add dual profile labeling in summary (baseline profile vs capability profile).

## Recommended first steps next session

1. Run sweep without plots for quick validation:
- [hardware/kicad/solar-project/spice/run_spice_corner_sweeps.ps1](hardware/kicad/solar-project/spice/run_spice_corner_sweeps.ps1) with GeneratePlots false

2. Open current summary:
- [hardware/kicad/solar-project/spice/corner_summary.csv](hardware/kicad/solar-project/spice/corner_summary.csv)

3. Open latest waveform dashboard:
- [hardware/kicad/solar-project/spice/plots/index.html](hardware/kicad/solar-project/spice/plots/index.html)

4. If enforcing CI-style warning gates:
- Run runner with FailOnWarnings true and explicit WarningGateTags for the profile being validated.
