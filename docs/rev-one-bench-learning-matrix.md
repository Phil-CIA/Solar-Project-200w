# Rev 0 Bench Lessons Learned & Rev 1 Improvement Matrix

Status: Approved Architectural Synthesis  
Date: 2026-09-11  
Governing Decisions: [DEC-023](decisions-log.md#L43) through [DEC-032](decisions-log.md#L51)

---

## 1. Executive Summary

During Rev 0 bench bring-up, the solar charge controller power stage was never successfully operated at full rated voltage/power from the PV input alone. Detailed root-cause investigation across hardware, firmware, and test instrumentation revealed that **a PV panel (and current-limited bench supplies) cannot supply the instantaneous inrush energy required to charge an uncharged output capacitor bank (~1.1 mF)**, causing repeated input collapse and UVLO packet bursting before soft-start can establish regulation.

This document synthesizes all verified Rev 0 bench observations, isolates their true root causes, and maps them to concrete, actionable Rev 1 design improvements classified by implementation domain (**Schematic**, **PCB/Layout**, and **Firmware/Test Procedure**).

---

## 2. Rev 0 to Rev 1 Improvement Matrix

| # | System Area | Rev 0 Bench Observation & Root Cause | Rev 1 Actionable Improvement | Implementation Domain | Verification & Test Gate |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **1** | **Power Path & Energy Architecture** | Attempting startup from PV into 0 V output capacitors (~1.1 mF) collapsed input supply below UVLO (~5.4 V) within 50–80 µs. | **Battery-First Power Path (DEC-025):** Output caps (`V_out`) precharged to $V_{\text{bat}}$ upon battery connection before switching starts. Reduced output bank to ~660 µF nominal (DEC-023). | **Schematic & Architecture** | Measure battery connection inrush current and verify monotonic $V_{\text{out}}$ startup into $V_{\text{bat}}$. |
| **2** | **Converter Inhibit / Enable** | LM51772 started autonomously via passive `EN_UVLO` divider; STM32 had no positive hardware inhibit to hold the converter off during boot/faults. | **Hardware-Safe Default-Off Inhibit (DEC-026):** Active-high `CTRL_EN_CHG` with default-off pull-down holding `EN_UVLO` low during MCU reset, unpowered state, or weak PV. | **Schematic** | Verify `EN_UVLO` ≤ 0.2 V with MCU unpowered, during reset, and with PV present. |
| **3** | **Battery Protection Fuse (F2)** | Both pins of F2 landed on `BAT_BUS_POS` copper trace, completely bypassing the fuse in the manufactured build. | **True Series Battery Fuse (DEC-025):** F2 placed strictly in series between `J3` (Battery +) and protected `BAT_BUS_POS`. | **Schematic & PCB** | Continuity inspection with F2 removed confirms open circuit between battery input and internal power rail. |
| **4** | **Current Shunt Footprint & Sensing (R16, R22)** | Merging Kelvin sense and power pads caused KiCad ERC net-conflicts and layout short-circuit errors. | **4-Pad Split Shunt Footprint:** Custom `R_2512 Shunt 4 pad` footprint with isolated power pads (1 & 4) and dedicated Kelvin sense pads (2 & 3). | **Schematic & PCB** | Netlist confirms 4 unique nets; clean ERC and 4-wire differential measurement at INA180 and LM51772. |
| **5** | **PV Reverse Protection & Gate Drive** | Discrete P-MOS / N-MOS high-side switching had ambiguous gate references and required rework. | **LM74502H (U14) + Back-to-Back N-MOS (Q5/Q6):** Integrated charge pump driving BSC040N10NS5 common-source FETs with gate pull-up and clamp. | **Schematic & Layout** | Execute [rev-one-u14-low-voltage-gate-test.md](test/rev-one-u14-low-voltage-gate-test.md) ($V_{\text{GS}} \approx 10\text{--}12\text{ V}$). |
| **6** | **MOSFET Gate Damping** | M1–M4 high-side and low-side switch nodes risked high $dV/dt$ ringing without series damping. | **10 Ω Gate Series Resistors (R69–R72):** Added series damping resistors in HO1, LO1, HO2, and LO2 gate drive traces. | **Schematic & Layout** | Measure gate rise/fall times and switch-node overshoot on oscilloscope. |
| **7** | **USB & Internal Bias Isolation** | USB 5 V backfed into LM51772 `VCC2` and bias rails through D8 rework; PC ground loops posed hazard during switching. | **Isolated Telemetry USB + Aux Power Strategy (DEC-032):** CP2102N host-powered only; D8 powered from dedicated 5 V buck; zero path from USB to gate driver. | **Schematic** | Measure 0 V on `VCC1`, `VCC2`, and gate outputs with only USB telemetry plugged in. |
| **8** | **Diagnostics & Scope Lands** | Probing fine-pitch IC pins caused shorting hazard; long ground clips distorted fast switch-node waveforms. | **Dedicated Test Points & Ground Spring Lands (DEC-031):** Dedicated pads on `SW1`, `SW2` with local `PGND` rings for scope ground springs; analog pin loops. | **Schematic & PCB Layout** | Visual inspection and physical probe clearance verification on bare PCB. |
| **9** | **Digital Boot & I2C Reliability** | STM32 BOOT0 floated (causing system memory boot loops); I2C SDA/SCL pins were swapped at MCU. | **Hardware Boot Resistor & Correct I2C Mapping (DEC-019, DEC-021):** Populated 10 kΩ pull-down on BOOT0; PB6=SCL, PB7=SDA hardware I2C pinout. | **Schematic** | 10/10 clean cold boots into user flash; hardware I2C ACK on AHT20 (0x38), OLED (0x3C), LM51772 (0x78). |
| **10** | **Converter Sizing & Nominal Envelope** | Sizing around 6 V UVLO forced unrealistic component stress; 15 A / 20 A targets were oversized for 200 W panel. | **20 V Nominal PV / 125 W Continuous (DEC-023, DEC-030):** Sized for 20 V nominal, 14.6 V / 12 A max output with MMPC1206-3R3MT 3.3 µH inductor. | **Design Sizing / Calculations** | TI Quickstart Tool workbook validation and thermal validation at 125 W steady-state. |

---

## 3. Implementation Breakdown by Engineering Domain

### A. Schematic Deltas
1. **LM51772 Inhibit Switch:** Add small N-channel FET (2N7002K) across `EN_UVLO` driven by STM32 `CTRL_EN_CHG` (active high to run, default-off pull-down).
2. **F2 Series Fuse:** Connect `J3.2` $\to$ `F2.1`, `F2.2` $\to$ `BAT_BUS_POS` (break all direct parallel copper paths).
3. **R16 & R22 Wiring:** Assign 4 distinct nets per shunt (Pads 1 & 4 for power flow; Pads 2 & 3 for Kelvin sense to U10 / U6).
4. **Divider Updates for 14.6 V LiFePO4:** Change R3 and R18 to $76.8\text{ k}\Omega$; keep R25 and R17 at $5.62\text{ k}\Omega$.
5. **USB Isolation:** Isolate CP2102N `VDD`/`VIO` completely from board `CTRL_3V3`.
6. **Test Points:** Instantiate explicit `TP_*` symbols for all analog signals and switch nodes.

### B. PCB / Layout Deltas
1. **Switch Node Ground Spring Lands:** Place exposed `PGND` annular rings/pads directly adjacent to `SW1` and `SW2` test pads.
2. **Kelvin Differential Routing:** Route pads 2 and 3 of R16 and R22 as tightly coupled differential trace pairs directly to amplifier inputs.
3. **QFN Exposed Pad Relief:** Apply DEC-016 footprint relief vias and wider solder-mask dams for U1 (STM32) and U6 (LM51772).
4. **Power Loop Minimization:** Keep high-frequency buck/boost loops (M1–M4, C19–C31, C18) tightly packed on Top Copper with solid PGND plane underneath.

### C. Firmware & Test Procedure Deltas
1. **Staged Startup State Machine:**
   - Boot on Battery $\to$ Check Battery Voltage ($10\text{--}14.6\text{ V}$) $\to$ Hold `CTRL_EN_CHG` LOW.
   - Detect PV Open Circuit Voltage ($V_{\text{PV}} \ge V_{\text{bat}} + 2.0\text{ V}$ and $V_{\text{PV}} \ge 20\text{ V}$).
   - I2C configure LM51772 for low initial average current ($1\text{--}2\text{ A}$).
   - Assert `CTRL_EN_CHG` $\to$ Monitor soft-start ramp $\to$ Engage MPPT tracking.
2. **Test Instrumentation Rule:** All high-side gate ($V_{\text{GS}}$) and switch-node measurements must use low-inductance ground springs or differential/isolated probes. Never clip standard ground leads to high-speed switching nodes.
