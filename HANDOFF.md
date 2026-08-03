# Solar Project 200W - Handoff

Latest active handoff: [SOLAR_HANDOFF_2026-07-28.md](SOLAR_HANDOFF_2026-07-28.md)

## Current status

- The MPPT charger SPICE work has been moved from a generic placeholder model to a more schematic-informed sanity-check harness for the B0/B1/B2 power path.
- The current harness is stable and produces a plausible charging-like operating point for the present assumptions.
- Latest verified LTspice result from [hardware/kicad/solar-project/spice/90_system_integration_harness_ltspice.log](hardware/kicad/solar-project/spice/90_system_integration_harness_ltspice.log): VOUT ≈ 15.43 V, BAT bus ≈ 11.04 V, ICHG ≈ -12 A.
- The battery boundary is currently anchored to a 12.8 V battery-like assumption, and the output-voltage behavior is still treated as something the control loop can influence.
- The main remaining design concern from the model is thermal/loss margin rather than basic topology feasibility.

## Pause point

This session should pause at the current verified SPICE baseline. No further model changes are needed unless the next session specifically wants to deepen the thermal-loss or control-loop treatment.

## Next session objective

Pick up from the current baseline and decide whether to:
1. Keep the present assumptions as-is for the prototype sanity check.
2. Increase the thermal/loss realism further.
3. Move from SPICE sanity-checking to KiCad-side validation and documentation.

## Primary evidence files

- [SOLAR_HANDOFF_2026-07-28.md](SOLAR_HANDOFF_2026-07-28.md)
- [hardware/mppt/schematic-notes.md](hardware/mppt/schematic-notes.md)
- [hardware/kicad/solar-project/spice/90_system_integration_harness_ltspice.cir](hardware/kicad/solar-project/spice/90_system_integration_harness_ltspice.cir)
- [hardware/kicad/solar-project/spice/90_system_integration_harness_ltspice.log](hardware/kicad/solar-project/spice/90_system_integration_harness_ltspice.log)
- [hardware/kicad/solar-project/spice/summarize_harness_margin.py](hardware/kicad/solar-project/spice/summarize_harness_margin.py)

## Note

Historical session details remain in dated SOLAR_HANDOFF files and project notes; this file is intentionally short to reduce context load for the next chat.
