# Solar Learning Project — Concept Capture + Handoff (Rev 0)

## Session Update - 2026-07-06

Completed since initial concept capture:

1. New repository initialized and pushed:
  - `https://github.com/Phil-CIA/Solar-Project-200w`
2. Base project scaffold created:
  - `README.md`
  - `.gitignore`
  - `docs/README.md`
  - `hardware/README.md`
  - `firmware/README.md`
3. Active KiCad project imported from:
  - `C:/Users/forch/GitHub/Solar Project`
4. Current KiCad source location in this repo:
  - `hardware/kicad/solar-project/`
  - `Solar Project.kicad_pro`
  - `Solar Project.kicad_sch`
  - `Solar Project.kicad_pcb`
5. Transient KiCad artifacts were intentionally excluded from source control.

Current continuation focus:
- Build planning docs (`requirements`, `architecture`, `decisions`) before firmware scaffolding.

## 1) Project Intent
This project is a **learning-first** build, not a cost-optimization build.  
Goal: understand and eventually design at PCB level:

1. Solar power capture (panel behavior, Vmp/Voc/Imp/Isc)
2. MPPT charging into LiFePO4 battery
3. DC energy storage/distribution
4. DC-to-AC inversion for common loads
5. System telemetry, protection, and safe bring-up

---

## 2) Selected System Concept (Current Baseline)
- Solar array: **200W total** (2 × 100W panels)
- Battery: **12.8V 40Ah LiFePO4** (4S)
- Charger path target: **15A minimum, 20A preferred**
- Inverter target: **300W pure sine**

This is the current concept baseline for initial architecture and documentation.

---

## 3) Key Learning Definitions Captured
- **Vmp (Vmpp):** voltage at maximum power point
- **Imp (Impp):** current at maximum power point
- **Voc:** open-circuit panel voltage
- **Isc:** short-circuit panel current
- **MPPT:** Maximum Power Point Tracking (operate panel near max power point as conditions change)

---

## 4) High-Level Architecture
1. PV input stage (protection + filtering)
2. MPPT charger stage (buck or buck-boost path)
3. Battery stage (LiFePO4 + BMS + fuse/disconnect)
4. DC distribution stage
5. Inverter stage (12V DC to 120VAC pure sine)
6. Monitoring/telemetry/control stage

---

## 5) Sizing Notes Captured
### Charge current from solar estimate
Use:
- I_charge ≈ (P_solar × efficiency) / V_charge
- For 4S LiFePO4, practical shortcut: **I_charge ≈ P_solar × 0.063**

For 200W:
- I_charge ≈ 12–13A expected peak
- Therefore charger should be **15A+** (20A preferred for margin/growth)

### Controller class implication
- 5A-class charger is too limiting for 200W
- 15A–20A-class charger aligns with chosen system

---

## 6) Safety & Design Priorities
- Fuse near battery positive terminal
- Branch fusing for subsystems
- Proper wire gauge for expected current
- UVLO/OVP/OCP/OTP protections
- Reverse polarity and transient protection on PV input
- Treat inverter output as hazardous mains-level AC
- Use staged bring-up and current-limited bench testing

---

## 7) Scope Strategy (One Piece at a Time)
### Phase A (current focus)
- Concept documentation
- Project structure setup
- MPPT charger design exploration (block level first)

### Phase B
- MPPT schematic development
- Simulation/math validation
- Firmware architecture for charge control + telemetry

### Phase C
- Inverter design exploration and implementation

---

## 8) Open Design Decisions (To Resolve in Next Sessions)
1. MPPT topology final: buck vs buck-boost
2. Controller approach:
   - analog power controller + MCU supervision
   - or highly integrated digital charger controller
3. Panel wiring strategy for final build: parallel vs series
4. Telemetry stack:
   - local display only
   - serial logging
   - CAN/UART/BLE/Wi-Fi extension
5. Battery current limits from selected BMS/cell data sheet

---

## 9) Deliverables Requested for Next Chat (VS Code Agent)
1. Create project folder structure (docs, calculations, hardware, firmware, test)
2. Create starter markdown docs/templates
3. Add engineering checklist files for:
   - requirements
   - design decisions
   - safety
   - bring-up/testing
4. Add initial calculator placeholders (markdown or scripts) for:
   - solar-to-charge current
   - fuse sizing worksheet
   - inverter input current estimation

---

## 10) Initial Folder Structure Target
```text
solar-learning-project/
  README.md
  docs/
    concept/
      system-overview.md
      requirements.md
      assumptions.md
      decisions-log.md
    handoff/
      next-chat-handoff.md
    safety/
      hazard-analysis.md
      protection-checklist.md
    test/
      bringup-checklist.md
      validation-plan.md
  calculations/
    charger/
      charge-current-sizing.md
      mppt-sizing-notes.md
    inverter/
      dc-input-current-sizing.md
  hardware/
    mppt/
      block-diagram.md
      schematic-notes.md
      bom-placeholder.csv
    inverter/
      block-diagram.md
      schematic-notes.md
      bom-placeholder.csv
  firmware/
    mppt/
      state-machine.md
      telemetry-signals.md
    inverter/
      control-architecture.md
  project-management/
    roadmap.md
    milestones.md
```

---

## 11) Definition of Done for “Project Setup” Task
- Folder tree created
- All listed starter docs created with section headers
- README links to major docs
- Handoff doc for next technical session completed
- No hardware commits yet; documentation scaffold only

---

## 12) Notes
This is a concept-building phase.  
Proceed incrementally, prioritizing understanding, safety, and reproducible documentation.
