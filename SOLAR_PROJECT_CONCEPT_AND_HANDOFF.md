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
3. Active KiCad project imported from the historical source at:
  - `C:/Users/forch/GitHub/Solar Project`
4. Current authoritative KiCad source location in this repo:
  - `hardware/kicad/solar-project/`
  - `hardware/kicad/solar-project/Solar Project.kicad_pro`
  - `hardware/kicad/solar-project/Solar Project.kicad_sch`
  - `hardware/kicad/solar-project/Solar Project.kicad_pcb`
5. To open the live schematic in KiCad, use:
  - `hardware/kicad/solar-project/Solar Project.kicad_sch`
5. Transient KiCad artifacts were intentionally excluded from source control.

Current continuation focus:
- Keep the KiCad work anchored to the repository copy above.
- Continue the schematic validation pass and refresh docs only when the exported state changes.
- Use the next few sessions to decide which legacy single-MOSFET-era functions still belong in hardware, which ones are firmware-only, and which ones can be removed now that LM51772 is the active controller path.

## Cleanup Plan For The Next Few Sessions

### Session 1 - Function Inventory And Ownership

Goal:
- Make a complete list of the leftover control, enable, fault, and legacy hold nets around U2/U6/Q2.

Questions to answer:
- Does the function still need to exist?
- If yes, is the right owner hardware, firmware, or both?
- If firmware is involved, is it just supervision/telemetry, or does it need to make a safety decision?

Decision rule:
- Hardware owns anything that must still work safely when the MCU is not running.
- Firmware owns only coordination, reporting, and non-safety policy.
- Remove any net or block that only existed to support the earlier single-MOSFET concept and no longer contributes to the LM51772 architecture.

### Session 2 - Schematic Cleanup

Goal:
- Remove or repurpose the nets that were only there for the older control concept.

Expected outcomes:
- Keep `CTRL_PWM_MAIN` and `CTRL_EN_CHG` if the MCU still needs to command the controller.
- Keep `FAULT_OCP` and `FAULT_OVP` as hardware-first safety indications, then mirror them in firmware only for logging or graceful shutdown.
- Remove or freeze any legacy `V_in_ON` / Q2-style path if it no longer has a real job.
- Finish the U6 helper-net cleanup only if those nets still represent real hardware needs.

### Session 3 - Verification And Simplification

Goal:
- Re-run ERC, compare the exported netlist against the intended function list, and delete any remaining scaffolding that does not support the current architecture.

Exit criteria:
- Every remaining net has a clear owner.
- No net exists solely because it was part of the old single-MOSFET concept.
- The docs say explicitly whether the function lives in hardware, firmware, or not at all.

### Session 4 - Sheet 2 And Legacy Output Cleanup

Goal:
- Clean the power-stage side first, because it is the simplest place to separate active function from old scaffolding.

Targets:
- Confirm `CTRL_PWM_MAIN` is the only needed drive input for the buck stage.
- Keep the async first-pass path in Sheet 2 and avoid reintroducing the old Q2-style control gate path.
- Remove any stale labels that only existed to support the earlier MOSFET-only control idea.

Expected result:
- Sheet 2 reflects the modern LM51772-driven control path without leftover naming from the first concept pass.

### Session 5 - Control And Fault Path Cleanup

Goal:
- Decide what the MCU actually owns versus what the hardware must enforce by itself.

Targets:
- Keep `CTRL_EN_CHG` if the MCU still needs an enable line.
- Keep `FAULT_OCP` and `FAULT_OVP` as hardware signals, then mirror them in firmware only if useful.
- Review `V_in_ON` and the `U2/Q2` legacy branch and either retire it or explicitly document why it stays.

Expected result:
- The control/fault sheet reads like a modern supervisor interface, not a half-retained legacy gate hack.

### Session 6 - U6 Helper Nets And Final ERC Pass

Goal:
- Finish the LM51772 helper nets only if they still support the selected architecture.

Targets:
- Close `U6_VCC2` and `U6_FLT` with real passives if U6 remains active.
- Keep `U6_SYNC`, `U6_DTRK`, and `U6_RST` only if they are still intentional straps.
- Re-run ERC/netlist export and compare the result against the classified backlog.

Expected result:
- Any net that survived this pass has a real hardware reason to exist.
- Any net that does not have a reason is removed rather than preserved out of habit.

### First-Pass Function Classification

Use this as the working backlog for the next sessions. The final answer can change, but each item needs an owner decision.

| Item | Current best guess | Why |
|---|---|---|
| `CTRL_PWM_MAIN` | Hardware stays, firmware drives it | The MCU should still command the converter, but the PWM signal itself is a hardware interface. |
| `CTRL_EN_CHG` | Hardware stays, firmware drives it | Enable/disable needs to exist in the schematic; firmware may assert it. |
| `FAULT_OCP` | Hardware stays, firmware monitors it | Over-current must remain a real protection signal even if firmware also logs or reacts to it. |
| `FAULT_OVP` | Hardware stays, firmware monitors it | Same logic as OCP: keep the hardware fault path, let firmware supervise the result. |
| `V_in_ON` | Likely delete or retire | It still belongs to the legacy Q2 gate path and only stays if that path is intentionally retained. |
| `U2` as control-rail owner | Delete or deprecate | Current docs already select U4 as the live `CTRL_3V3` owner. |
| `U6_VCC2` | Hardware stays if U6 stays | Needs a real decoupling path if the LM51772 gate-driver rail remains active. |
| `U6_FLT` | Hardware stays if U6 stays | Fault output needs a pull-up and a destination pin; firmware can only observe it after hardware exists. |
| `U6_RST` | Hardware stays if used | Reset is a real pin-level strap or control input, not a firmware-only concept. |
| `U6_SYNC` | Hardware stays if used | Sync/strap behavior is a physical configuration choice. |
| `U6_DTRK` | Hardware stays if used | Dynamic tracking is a physical pin-level mode decision. |

### Session Sequence By Owner Type

1. Hardware-only cleanup session:
- Finish `U6_VCC2`, `U6_FLT`, and any strap nets that still need real passives.
- Remove obsolete U2 owner wiring if it is no longer part of the chosen control tree.
- Delete `V_in_ON` if the Q2 legacy control path is not intentionally kept.

2. Firmware interface session:
- Map `CTRL_PWM_MAIN`, `CTRL_EN_CHG`, `FAULT_OCP`, and `FAULT_OVP` to the MCU behavior that should remain.
- Decide whether firmware only logs faults or also performs controlled shutdown / restart policy.
- Keep the firmware model as a supervisor, not the safety owner, unless a specific hardware path is missing and documented.

3. Final simplification session:
- Remove any leftover symbols, labels, or note text that only describe the old single-MOSFET concept.
- Re-run ERC and compare the live schematic against this table.
- Update decisions log only for items that truly changed ownership or function.

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
