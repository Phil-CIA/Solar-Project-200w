# SPICE / LTspice workflow

This folder contains a small, project-local validation deck for the PV-input NMOS switch gate-drive network.

## Files
- pv_switch_gate_drive.cir - ngspice-compatible netlist used for operating-point checks
- pv_switch_gate_drive.asc - LTspice schematic draft for the same topology
- run_pv_gate_drive.py - helper to run a few VIN/VCTRL cases with ngspice
- summarize.py - helper to parse outputs and print a compact summary

## Recommended workflow
1. Open pv_switch_gate_drive.asc in LTspice and run the operating-point analysis.
2. Repeat for the relevant VIN and control states (12 V / 24 V / 36 V / 50 V, OFF versus ON).
3. Record the resulting Vgs margin in the design review notes.

## Current outcome
The earlier validation showed that the current gate-drive arrangement does not provide a usable Vgs margin across the tested VIN range. That result should be treated as a design blocker until the gate-drive topology is revised.

## Current harness baseline
The staged LTspice integration harness in [90_system_integration_harness_ltspice.cir](90_system_integration_harness_ltspice.cir) now produces a stable, charging-like baseline for the simplified B0/B1/B2 path. From the latest completed run, the model reported approximately 15.17 V at the output node, 12.46 V at the battery bus, and about 12 A of charge current at 4 ms in the simulator’s sign convention. This is suitable for concept-level sanity checking, but it is still not hardware-validated.

The latest block values were also aligned to the current KiCad schematic intent: B0 uses the PV fuse/TVS clamp path around F1, D1, and D2; B1 uses the selected 1uH power-stage inductor and a bulk output-capacitance branch with extra series-loss elements to reflect conduction and heating-related loss; and B2 uses a more conservative battery-side fuse and pack boundary model.

Compared with the project requirement target in [docs/requirements.md](docs/requirements.md), the current result lands near the expected 12 A to 13 A charging band for the 200W-class baseline and is therefore directionally consistent with the intended charger envelope. It is still a simplified model and should not be treated as a final design proof.

## Validation summary
The latest run gives a stable charging-like operating point at 4 ms with VOUT ≈ 15.17 V, BAT_BUS ≈ 12.46 V, and ICHG ≈ 12 A (negative sign in LTspice due to current direction convention). That corresponds to roughly $15.17 \times 12 \approx 182\ \text{W}$ of delivered charge power, which is still within the 180 W to 220 W design envelope in [docs/requirements.md](docs/requirements.md). The run remained stable across the leakage-corner sweep from 600 Ω to 1000 Ω, and the added hot-loss terms now pull the output and battery bus downward in a way that is more consistent with real power-stage losses. The result is still useful for checking topology and schematic mapping, but it should remain an engineering sanity check rather than a validated design value.

Design-review statement: the current LTspice harness supports the view that the proposed MPPT charger concept is feasible for the documented 200 W-class charging envelope, with the caveat that the model is still a simplified abstraction and requires later hardware validation.

## New modular schematic-to-SPICE flow

The full schematic partition for block-based SPICE modeling is documented in `SCHEMATIC_SPICE_PARTITION.md`.

Starter block decks were added:

- `00_block_b0_pv_input.cir`
- `01_block_b1_power_stage.cir`
- `02_block_b2_battery_boundary.cir`
- `03_block_b3_control_rail.cir`
- `04_block_b4_fault_network.cir`
- `05_block_b5_measurement_frontend.cir`
- `90_system_integration_harness.cir`

Recommended sequence:

1. Make each block deck runnable on its own.
2. Replace placeholders with real values/models from the KiCad schematic.
3. Integrate blocks progressively in `90_system_integration_harness.cir`.

## Automated corner runs

Use the PowerShell runner to execute key decks and produce a compact CSV summary:

- Script: `run_spice_corner_sweeps.ps1`
- Summary output: `corner_summary.csv`
- Per-deck logs: `logs/*.log`
- Waveform CSV exports: `waveforms/*.csv`
- Plot dashboard: `plots/index.html`

Summary columns now include:

- `status` pass/fail classification from simulator/log health checks
- `vout_avg`, `iin_avg` from direct deck measurements when available
- `iout_avg`, `pin_avg`, `pout_avg`, `eff_pct` for B1, derived from measured values and deck params when needed
- `req_chg_001`, `req_chg_003` requirement-oriented status fields for B1:
	- `req_chg_001`: pass/fail against 15A minimum target
	- `req_chg_003`: pass/fail against 12A to 13A expected baseline band
- `bat_trip`, `ocp_trip` from B4 threshold sweeps
- `notes` with per-deck log path and optional warnings

Current warning tags appended in `notes`:

- `iin_avg_negative`: B1 input current sign is negative in this operating convention
- `pin_avg<=0`, `pout_avg<=0`: non-physical average power direction/magnitude
- `eff_pct_out_of_range`: computed efficiency is outside 0 to 100%
- `measure_failed`: ngspice reported one or more measurement failures in the deck log

Example from this folder:

```powershell
powershell -ExecutionPolicy Bypass -File .\run_spice_corner_sweeps.ps1
```

Run with warning gate enabled (nonzero exit if selected warnings are present):

```powershell
powershell -ExecutionPolicy Bypass -File .\run_spice_corner_sweeps.ps1 -FailOnWarnings $true
```

Generate plots only from existing waveform CSV files:

```powershell
powershell -ExecutionPolicy Bypass -File .\generate_spice_plots.ps1
```

Current runner deck set:

- `00_block_b0_pv_input.cir`
- `01_block_b1_power_stage.cir`
- `02_block_b2_battery_boundary.cir`
- `04_block_b4_fault_network.cir`
- `90_system_integration_harness.cir`

`04_block_b4_fault_network.cir` now reports measured threshold points as `BAT_TRIP` and `OCP_TRIP` in the run log, and the runner parses those into the summary CSV when available.

## Startup-current experiment

`03_startup_current_limit.cir` is a separate behavioral transient deck for the Rev 0 bring-up question: can a nominal 6 V source with an approximately 200 mA current ceiling build current in the 1 uH inductor during a soft-start duty ramp? It uses a 610 kHz switching period, a configurable soft-start interval, the 1 kOhm preload, and a 30 ohm source resistance as a first-order approximation of the bench supply current limit.

This deck is not an LM51772 vendor model and does not prove the controller's exact soft-start or current-limit behavior. Its useful outputs are `IIN_PEAK`, `IIN_AVG`, `IL_PEAK`, `IL_AVG`, and `VOUT_FINAL`. Vary `TSS`, `DUTY_END`, `LVAL`, and `RSOURCE` one at a time when comparing the model to the bench capture.

Run it from this directory with:

```powershell
ngspice -b -o logs/03_startup_current_limit.log 03_startup_current_limit.cir
```
