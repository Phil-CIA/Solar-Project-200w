Continue the Solar Project 200W (Rev 1) from `SOLAR_HANDOFF_2026-09-12.md`. Read it along with `docs/decisions-log.md`, `docs/rev-one-implementation-plan.md`, `docs/rev-one-bom.md`, and `docs/rev-one-bench-learning-matrix.md`.

### Current State:
- **Gate 2 & Gate 3 (Schematic Corrections, Footprint Verification & BOM Alignment):** Completed with **0 ERC Errors** on `Solar Project Rev1.kicad_sch`.
- All 172 components are physically verified and mapped to their authoritative LCSC part numbers in [hardware/kicad/solar-project/bom/Rev1_BOM.csv](hardware/kicad/solar-project/bom/Rev1_BOM.csv).
- Battery-First architecture confirmed; hardware inhibit Q7 (2N7002K) installed on LM51772 EN_UVLO; 4-pad Kelvin shunts (R16, R22) wired; 14.6 V feedback dividers (R3, R18 = 76.8k) updated; STM32G431CBU6 package aligned to UFQFPN-48.

### Focus for This Session: PCB Layout Synchronization & Routing (Gate 4)
1. **Synchronize PCB Layout:**
   - Open `Solar Project Rev1.kicad_pcb` in KiCad PCB Editor.
   - Run **Update PCB from Schematic (`F8`)** to import the verified netlist and all 172 footprints onto the board.
   - Confirm zero unresolved footprint errors or missing net connections.
2. **Component Placement & Floorplanning:**
   - Place and optimize the high-current Buck-Boost power stage: Input protection FETs (Q5/Q6) $\rightarrow$ Switching bridge (M1–M4) $\rightarrow$ Main inductor (L2) $\rightarrow$ Output Kelvin shunt (R16) $\rightarrow$ Battery fuse (F2).
   - Establish clean separation between high-current switching loops and sensitive analog/Kelvin sense signals.
3. **DRC Validation:**
   - Set 4-layer stackup design rules and run PCB DRC.

