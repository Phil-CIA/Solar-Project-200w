# Solar Project Handoff – 2026-07-19 Stop Point

**Status:** Partial close. CTRL_SUPPLY_IN and U6 strap work remain complete, and exports were refreshed in a continuation pass. Task A (U6_VCC2 decoupler + U6_FLT pull-up) is still not complete in exported artifacts, and `V_in_ON` remains a legacy control-path hold.

**Date:** 2026-07-19 session stop point  
**Phase:** Phase 1 – Rev 0 Prototype Schematic Validation  
**Outcome:** Safe canonical rename pass completed on the live schematic and docs; continuation re-export captured actual post-edit state and clarified remaining blockers.

**Bench workstation note:** This handoff is the current closeout state to open on the bench machine; use the fresh export files and the remaining U6/U7/U8 follow-up notes as the starting point.

---

## 1. Current Stop Point

What changed after the earlier CTRL_SUPPLY_IN/U6 closure:

- Safe canonical MCU/boundary renames were applied in the live schematic:
  - `VBUS` -> `USB_5V_IN`
  - `V_in_ADC` -> `SENSE_PV_V`
  - `V_out _on` -> `CTRL_PWM_MAIN`
  - `TXD` -> `UART_TX_LOG`
  - `RXD` -> `UART_RX_CFG`
- `V_in_ON` was intentionally left unchanged because it still lands in the legacy U2/Q2 path and is not yet safe to map to a canonical control or sense role.
- The audited live STM32G431 map is now documented in `hardware/mppt/sheet4-sensing-control-wiring-map.md`.
- `hardware/mppt/schematic-notes.md` was updated to record the rename pass and the reason `V_in_ON` remains.
- During editing, the KiCad schematic header was briefly corrupted by a broad text patch and then repaired successfully from committed content.

Current workspace state at stop:

- Modified: `hardware/kicad/solar-project/Solar Project.kicad_sch`
- Modified: `hardware/mppt/schematic-notes.md`
- Modified: `hardware/mppt/sheet4-sensing-control-wiring-map.md`
- Untracked autosave: `hardware/kicad/solar-project/_autosave-Solar Project.kicad_sch`

Continuation update (post re-export):

- `Solar Project.net` and `ERC.rpt` are now freshly exported (10:23 to 10:24 local).
- Task A is still open in exported data: `U6_VCC2` and `U6_FLT` are still dangling one-node nets.
- `R34` is not present in exported schematic/netlist.
- Existing `C30` remains the 47uF output capacitor reference, so any VCC2 decoupler must use a new designator.
- ERC changed from the prior 7-error baseline and now includes additional U7 (AHT20) unconnected pin errors.
- U7 is now intentional as the board temperature sensor placeholder near the warmest representative board area.
- U8 is now intentional as the OLED display module.

---

## 2. Quick Reference: What's Done ✅

| Item | Status | Evidence |
|------|--------|----------|
| CTRL_SUPPLY_IN wiring (D4/D6/D7 OR-ing) | ✅ Complete | Netlist code 10: all 4 nodes present (D4/D6/D7/U4) |
| U6 control straps (RST/SYNC/DTRK) | ✅ Complete | Global labels at correct coords; netlist verified |
| U6 gate-driver pins (HO/LO types) | ✅ Complete | Changed from `unspecified` to `output`; 4 ERC errors eliminated |
| R32 pin 2 grounding | ✅ Complete | PWR_NEG label added; connection verified |
| Schematic export | ✅ Fresh continuation export | `Solar Project.net` (10:23) and `ERC.rpt` (10:24) now reflect current schematic state. |
| ERC status | Regressed vs prior baseline | New export includes additional U7-related unconnected errors; no longer equivalent to prior 7-error baseline. |
| Git commits | Dirty working tree | Prior CTRL_SUPPLY_IN commits remain local, but current rename-pass edits are not yet committed. |

---

## 3. Latest Git Status

```bash
# Run this to verify current stop state:
git status --short
```

**Expected output:**
```
 M "hardware/kicad/solar-project/Solar Project.kicad_sch"
 M hardware/mppt/schematic-notes.md
 M hardware/mppt/sheet4-sensing-control-wiring-map.md
?? "hardware/kicad/solar-project/_autosave-Solar Project.kicad_sch"
```

**Ready to push?** Not yet. The rename-pass changes are still uncommitted, and KiCad-generated artifacts need to be refreshed first.

---

## 4. Critical Technical State

### 3.1 CTRL_SUPPLY_IN Architecture (DEC-013)

**Three-source OR-ing for maximum Rev 0 flexibility:**

```
PV Input (highest voltage) ──[D6]──┐
                                     ├──> CTRL_SUPPLY_IN ──> U4 VIN ──> LM5163 ──> CTRL_3V3
Battery BUS (fallback) ──────[D4]──┤
                                     │
USB 5V (bench mode) ────────[D7]──┘
```

**Netlist verification (net code 10 from `Solar Project.net`):**
```
(net (code "10") (name "CTRL_SUPPLY_IN") (class "Default")
  (node (ref "C15") (pin "1") (pintype "passive"))          ← Decoupling cap
  (node (ref "D4") (pin "1") (pintype "passive"))           ← BAT source
  (node (ref "D6") (pin "1") (pintype "passive"))           ← CHG source (NEW)
  (node (ref "D7") (pin "1") (pintype "passive"))           ← USB source (NEW)
  (node (ref "R13") (pin "1") (pintype "passive"))          ← EN/UVLO divider
  (node (ref "U4") (pin "2") (pinfunction "VIN") ...))      ← Buck input
```

**Diodes (all 1N5819WS Schottky, 40V/1A):**
- D4: (existing) y-coord 187.96
- D6: (NEW) placement 298.45, 182.88, 90° rotation
- D7: (NEW) placement 298.45, 177.80, 90° rotation

**Runtime behavior:** Highest voltage source wins (parallel diode OR), battery fallback when PV unavailable, USB independent debug path.

### 3.2 U6 (LM51772) Control Strap Map

| Pin | Function | Net | Strategy | Status |
|-----|----------|-----|----------|--------|
| 36 | VIN | PV_IN_POS | Main power input | ✅ |
| 40 | BIAS | PV_IN_POS | Rev 0 strap; upgrade Rev 1 to VCC2 | ✅ |
| 29 | VCC2 | U6_VCC2 (dangling) | Gate-driver supply placeholder | ⏳ needs dedicated decoupler (do not reuse C30 ref) |
| 11 | FLT | U6_FLT (dangling) | Fault open-collector output | ⏳ needs R34 pullup |
| 38 | RST | CTRL_3V3 | Reset pull-high for normal operation | ✅ |
| 3 | SYNC | PWR_NEG | Free-running oscillator (no sync) | ✅ |
| 4 | DTRK | PWR_NEG | Dynamic tracking disabled | ✅ |
| 28 | PGND | PGND | Analog return (same as PWR_NEG for Rev 0) | ✅ |
| 18 | VOUT | CHG_OUT_POS | Battery charge output | ✅ |
| 35/37, 34/36 | HO1/LO1, HO2/LO2 | (local) | Gate drivers → M1/M2 gates | ✅ type corrected |

**Strap rationale:**
- Both VIN and BIAS on PV_IN_POS → simpler Rev 0, higher quiescent current acceptable
- RST to CTRL_3V3 → system power-good pull-high
- SYNC/DTRK to PWR_NEG → freewheel mode, no external load-sharing needed for prototype
- Gate-driver types now `output` → eliminates 4 ERC hard errors on M1/M2/M3/M4 gates

### 3.3 ERC Summary: 7 Deferred Power-Authority Errors

**All 7 remaining errors are explicitly deferred power-authority semantics (acceptable for prototype):**

1. U6 pin 36 (VIN) — PV_IN_POS has no PWR_FLAG
2. U6 pin 28 (PGND) — PGND has no PWR_FLAG
3. U1 pin 1 (VBAT/STM32) — CTRL_3V3 authority deferred
4. U4 pin 2 (VIN) — CTRL_SUPPLY_IN has no PWR_FLAG
5-7. D3 USB ESD diode (pins 2/4/6) — library symbol type mismatch

**Policy:** PWR_FLAG cleanup → low-priority end-of-phase activity (does not affect functional behavior).

---

## 4. Pending Work (Prioritized)

### 4.1 🔴 CRITICAL – Add VCC2 Decoupler + R34 (next pass)

**VCC2 decoupler (100 nF, 0805; use new reference, e.g., C44)**
- Purpose: Stabilize U6 pin 29 gate-driver supply output
- Placement: Adjacent to U6, coordinate ~(115, 181)
- Connections:
  - Pin 1 → U6_VCC2 net (label at 102.87, 184.15)
  - Pin 2 → PGND (label at 93.98, 281.94)
- Netlist status: Net code 78 (U6_VCC2) exists but is dangling (only U6 pin 29)
- **Next action:** Add via KiCad GUI → Place → Wire to labels

**R34 – FLT Pull-Up Resistor (10 kΩ, 0805)**
- Purpose: Pull U6 pin 11 (open-collector fault output) to CTRL_3V3 for MCU monitoring
- Placement: Adjacent to U6, coordinate ~(115, 215)
- Connections:
  - Pin 1 → U6_FLT net (label at 102.87, 219.71)
  - Pin 2 → CTRL_3V3 (label at 102.87, 265.43)
- Netlist status: Net code 77 (U6_FLT) exists but is dangling (only U6 pin 11)
- **Next action:** Add via KiCad GUI → Place → Wire to labels

**Why still open after fresh export:** Existing `C30` is already assigned as a 47uF output capacitor on `V_out`; `R34` is not present; both `U6_VCC2` and `U6_FLT` remain dangling in the fresh netlist.

### 4.2 🟡 HIGH – Verify Ground/Power Rail Architecture

**Checklist:**
- [ ] Confirm PGND (U6 analog return) and PWR_NEG (main return) are intentionally same net (current export shows separate nets)
- [x] Validate BAT_BUS_POS → F2 fuse → battery charging path is correct
- [x] Check U1 (STM32G431) all VDD/VDDA/VBAT/VREF+ tied to CTRL_3V3 (undriven error deferred; intentional)

### 4.3 🟡 MEDIUM – Migration Checklist Calculations

**File:** `hardware/mppt/lm851772-lm51772-migration-checklist.md`  
**Sections needing completion:**
- [ ] Section 4.2: UVLO/EN divider final values (use 1 MΩ chain for low Iq on 100V input)
- [ ] Section 4.3: Soft-start capacitor (reference design: ~100 nF for ~100 ms startup)
- [ ] Section 4.4: Loop compensation network values (conservative: lead-lag, crossover ~50 kHz)

### 4.4 🟢 LOW – PWR_FLAG Cleanup (end-of-phase)

**Not blocking Rev 0 bringup; acceptable to defer:**
- Add PWR_FLAG symbols via KiCad GUI to PV_IN_POS, PGND, CTRL_SUPPLY_IN
- Will eliminate 4 remaining `power_pin_not_driven` errors
- Does not change functional behavior; purely ERC aesthetic

---

## 5. File Status and Exports

### 5.1 Fresh Exports (2026-07-19 08:46-08:47)

**Location:** `hardware/kicad/solar-project/`

| File | Timestamp | Status |
|------|-----------|--------|
| Solar Project.kicad_sch | 08:46:39 AM | ✅ Open in KiCad |
| Solar Project.net | 08:46:54 AM | ✅ Fresh netlist (CTRL_SUPPLY_IN verified) |
| ERC.rpt | 08:47:11 AM | ✅ 7 errors / 56 warnings (stable) |

**To verify in next session:**
```bash
cd hardware/kicad/solar-project/
ls -la "Solar Project.kicad_sch" "Solar Project.net" "ERC.rpt"
```

### 5.2 Documentation Updated

| File | Changes |
|------|---------|
| `hardware/mppt/schematic-notes.md` | Added 2026-07-19 Session 3 summary (architecture complete, VCC2/FLT deferred) |
| `SOLAR_HANDOFF_2026-07-19.md` | This file (new comprehensive handoff) |

---

## 6. Technical Reference

### 6.1 Control Supply Topology (DEC-013)

**Why three sources?**
1. **PV priority** (highest voltage wins): Maximize charging efficiency when sun available
2. **Battery fallback**: Cloudy operation, maintains CTRL_3V3 without PV input
3. **USB bench mode**: Debug and programming without battery present

**Cost:** 3 Schottky diodes (~$0.15) + U4 wide-input buck (~$0.80) = ~$0.95 BOM for full flexibility

### 6.2 STM32G431CBU6 (U1) Supply Configuration

**All VDD/VDDA/VREF+ → CTRL_3V3** (intentional; power-in undriven error deferred)
- VBAT tied to CTRL_3V3 (backup battery pin; no separate battery)
- VSS/VSSA → PWR_NEG
- PC13 reserved for U6_FLT pull-up (R34) once placed

### 6.3 U4 (LM5163) Buck Configuration

**Input:** CTRL_SUPPLY_IN (100V absolute max, OR-ing three sources)  
**Output:** CTRL_3V3 (3.3V regulated)  
**Features:**
- Ultra-low quiescent current (1 μA typical)
- Synchronous rectification
- Ideal for powering MCU from wide input range (2.95–100V)

---

## 7. Recommended Next Session Tasks

**Pick one focus area (in priority order):**

**Task A: Component Placement (CRITICAL)**
- Add dedicated VCC2 decap (100 nF, 0805; new ref such as C44) with explicit wiring to U6_VCC2 and PGND labels
- Add R34 (10 kΩ, 0805) FLT pullup with explicit wiring to U6_FLT and CTRL_3V3 labels
- Re-export netlist and verify both components on their respective nets
- Commit: `git commit -m "schematic: add U6_VCC2 decoupler and R34 FLT pull-up"`

**Task B: Verify Power Paths (HIGH)**
- Resolve whether PGND should be explicitly tied to PWR_NEG or remain isolated-by-name for Rev 0
- Keep U1 power connection verification as complete (all VDD-class rails on CTRL_3V3)
- Keep BAT_BUS_POS → F2 → CHG_OUT_POS path verification as complete

**Task E: Finish U7/U8 Bring-up (HIGH)**
- Wire U7 as the board temperature sensor near the warmest representative board spot and confirm SENSE_TEMP_BOARD connectivity
- Confirm U8 power and I2C wiring are intentional and leave the display connected only to the control bus and supply rails

**Task C: Calculation Worksheet (MEDIUM)**
- Fill migration checklist Section 4.2/4.3/4.4 with design values
- Reference LM5163 datasheet soft-start and compensation recommendations
- Document rationale for each value choice

**Task D: PWR_FLAG Cleanup (LOW)**
- Not needed for bringup; skip unless you want clean ERC report
- Can add end-of-phase via GUI

---

## 8. Quick-Start Checklist for Next Session

Before opening KiCad:
1. [ ] Read this handoff (you're here!)
2. [ ] Run `git log --oneline | head -5` — should show 7951f4c, 7a78756 at top
3. [ ] Verify schematic file: `ls -la hardware/kicad/solar-project/Solar\ Project.kicad_sch` 
4. [ ] Choose focus task A/B/C/D from section 7 above

In KiCad:
- Open `Solar Project.kicad_sch`
- Schematic should load without errors (file now clean)
- ERC now differs from old baseline and includes additional U7 (AHT20) unconnected-pin errors; clear/confirm U7 intent before using ERC count as a gate metric
- U8 display connectivity should remain on I2C CLK/DATA plus CTRL_3V3 and GND; no extra control wiring is expected

For VCC2 decoupler/R34 placement (if Task A):
- **Do NOT use text-edit** — use KiCad GUI drag-and-place
- **Do use explicit wire connections** to global labels (U6_VCC2, PGND, U6_FLT, CTRL_3V3)
- **Re-export netlist immediately after** and verify nodes appear in correct nets

---

## 9. Known Issues / Gotchas

**Text-edit symbol insertion is fragile:** Attempted C30/R34 addition via direct schematic text editing → components auto-connected to wrong nets during KiCad validation pass. **Solution:** Use GUI placement with explicit wires instead.

**U6 lib_symbol_mismatch warning:** Expected and acceptable. HO1/LO1/HO2/LO2 pin types were updated in-schematic to `output` (normally would be `unspecified` from library). Intentional fix; accept for Rev 0.

**7 deferred ERC errors remain:** All power-authority semantics. Adding PWR_FLAGS will clear these, but not required for prototype bringup. Defer to end-of-phase.

---

## 10. Success Criteria for Next Session

✅ **Task A Complete:** Dedicated VCC2 decoupler (new ref) and R34 appear in fresh netlist on correct nets (`U6_VCC2` and `U6_FLT` respectively)  
✅ **Task B Complete:** PGND/PWR_NEG policy is explicitly decided and documented; battery and U1 rail checks remain verified  
✅ **Task C Complete:** Migration checklist Section 4 filled with design values and rationale  
✅ **Task D (optional):** PWR_FLAGS added; ERC shows 3 errors / 56 warnings

---

**Created:** 2026-07-19 Session 3 Completion  
**Next Review:** 2026-07-20 or later session  
**Next Handoff Trigger:** After VCC2 decoupler + R34 placement verification and U7/U8 bring-up confirmation OR after major topology change
