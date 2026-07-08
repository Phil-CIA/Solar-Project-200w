# MPPT GitHub Reference Notes

Last updated: 2026-07-06
Purpose: track external open-source references that are useful for architecture and layout study without blindly copying designs.

## 1. How To Use These References

Use them to study:
- sheet partitioning
- high-current path placement
- sensing and control separation
- connector and protection placement
- thermal/mechanical layout strategy

Do not use them as drop-in designs.
Re-evaluate every borrowed idea against this project's voltage, current, safety, and learning goals.

## 2. Recommended Reference Repos

### Reference A

Repo:
- https://github.com/junglejim8/opensolar

Why it is useful:
- Full MPPT solar charge controller project
- KiCad PCB and project files are present
- Useful for seeing how a larger controller is partitioned and physically organized

What to inspect first:
- overall board floorplan
- separation between power stage and control/telemetry section
- placement of high-current connectors and bus structures
- input/output capacitor clustering near the switching path

What not to copy blindly:
- absolute ratings and topology choices, because it is a much larger 100V / 65A-class design
- mechanical assumptions tied to its current level and cooling needs

### Reference B

Repo:
- https://github.com/MarcelMo/MPPT_Kicad_Schematics

Why it is useful:
- Contains KiCad schematic, PCB, footprints, and README notes
- Better fit for studying how a complete MPPT charger project is packaged in KiCad
- Includes rendered board views and explicit notes about design changes

What to inspect first:
- MPPT_Charger.kicad_sch for sheet structure
- MPPT_Charger.kicad_pcb for placement and current-path organization
- README for documented modifications and layout tradeoffs

What not to copy blindly:
- any component values or footprints that depend on its source design assumptions
- any ground-splitting or copper-bar decisions without checking our current and measurement strategy first

## 3. Which One Is Better For Our Current Phase

Best schematic structure reference:
- MarcelMo/MPPT_Kicad_Schematics

Best layout-scale cautionary reference:
- junglejim8/opensolar

Recommended use for this project right now:
- Start with MarcelMo for KiCad organization and schematic/PCB packaging ideas.
- Cross-check opensolar only for power-stage placement patterns and high-current layout discipline.

## 4. What To Mirror In Our Own Repo

Safe ideas to mirror conceptually:
- clear separation of power path, sensing path, and control path
- visible high-current loops and capacitor placement discipline
- explicit documentation of what changed and why
- keeping project-specific symbols/footprints with the KiCad project when needed

## 5. Next Review Step

When the first local buck schematic sheet exists, compare it against these references only for:
- loop compactness
- measurement point placement
- connector/protection placement
- serviceability and note clarity
