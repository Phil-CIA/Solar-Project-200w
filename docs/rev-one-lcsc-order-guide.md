# Solar Project 200W - Rev 1 LCSC Quotation & Reconciled Order Guide

**Date:** 2026-09-14  
**LCSC Quotation Source:** `c:\Users\forch\Downloads\export_project_20260914_151559.xls`  
**Status:** Reconciled Against LCSC Live Inventory  

## Executive Summary

- **Total Line Items Submitted:** 81 unique line items (representing all 163 mapped PCB components)
- **In-Stock & Matched:** **65 items** ($64.70 USD)
- **Action Required (Backordered / Discontinued / Missing):** **16 items** ($13.19 USD)
- **Total Quoted Baseline:** **$77.89 USD**

---

## 1. Action Items: Reconciling the 16 Unmatched or Backordered Parts

LCSC's automated importer flagged 16 parts as backordered, discontinued, or missing. Below are the recommended engineering resolutions:

### A. Key ICs & Active Silicon (4 Items)
| Designator | Requested Part | LCSC Status | Recommended Action / Resolution |
| :--- | :--- | :--- | :--- |
| `U1` | `STM32G431CBU6` (`C529356`) | **Backordered** (0 stock) | **Option A:** Populate using the spare `STM32G473CEU6` chips in hand (UFQFPN-48, 100% pin-compatible drop-in). <br>**Option B:** Purchase `STM32G431CBU6` from Mouser / DigiKey. |
| `U6` | `LM51772QRHARQ1` (`C41383743`) | **Backordered** (0 stock) | **Option A:** Order `LM51772` directly from TI.com or Mouser. <br>**Option B:** Salvage `U6` from Rev 0 board if required. |
| `U5` | `LP5912-3.3DRV` (`C114569`) | **No Match** | Search LCSC by manufacturer PN `LP591233DRVR` or substitute equivalent 3.3V 500mA ultra-low-noise LDO in WSON-6 / SOT-23. |
| `J2` | `USB4105-GF-A` (`C3020560`) | **Backordered** (0 stock) | Swap to in-stock 16-pin USB-C SMD receptacle drop-in (e.g. `TYPE-C-31-M-12` or `C165948`). |

### B. Passives & Discontinued Parts (12 Items)
| Designator | Value / Function | Requested Part | LCSC Status | Recommended Resolution |
| :--- | :--- | :--- | :--- | :--- |
| `C20` | 180 pF 0805 MLCC | `C1794` | **Discontinued** | Swap to standard in-stock 180 pF 0805 C0G cap (e.g. Uniroyal/Yageo 0805 180pF). |
| `R20` | 499 kΩ 0805 1% | `C119857` | **No Match** | Select any standard 0805 1% 499 kΩ resistor on LCSC. |
| `R28` | 51.7 kΩ 0805 1% (UVLO) | `C119851` | **No Match** | Select any standard 0805 1% 51.7 kΩ resistor on LCSC. |
| `R29` | 20.5 kΩ 0805 1% | `C119842` | **No Match** | Select any standard 0805 1% 20.5 kΩ resistor on LCSC. |
| `R30` | 40.2 kΩ 0805 1% | `C119848` | **No Match** | Select any standard 0805 1% 40.2 kΩ resistor on LCSC. |
| `R31` | 2.43 kΩ 0805 1% (Comp Zero) | `C119853` | **Backordered** | Select alternative in-stock 0805 1% 2.43 kΩ resistor on LCSC. |
| `R7, R10, R61` | 20 kΩ 0805 1% | `C17523` | **Backordered** | Swap to `C17522` or standard 0805 1% 20 kΩ resistor on LCSC. |
| `R14` | 330 kΩ 0805 1% | `C17651` | **Backordered** | Swap to standard 0805 1% 330 kΩ resistor on LCSC. |
| `R15` | 2 kΩ 0805 1% | `C17537` | **Backordered** | Swap to standard 0805 1% 2 kΩ resistor on LCSC. |
| `R23` | 75 kΩ 0805 1% | `C17743` | **Backordered** | Swap to standard 0805 1% 75 kΩ resistor on LCSC. |
| `R24, R62` | 5.1 kΩ 0805 1% | `C17700` | **Backordered** | Swap to `C17699` or standard 0805 1% 5.1 kΩ resistor on LCSC. |
| `R51` | 2.7 kΩ 0805 1% | `C17546` | **Backordered** | Swap to standard 0805 1% 2.7 kΩ resistor on LCSC. |

---

## 2. In-Stock Verified Silicon & Power Stage Devices

The critical power stage, FETs, shunts, and auxiliary silicon are **100% In Stock** on LCSC:

| Designator | Qty/Board | Value / Part | LCSC Part # | Unit Price (USD) | Ext Price (USD) | Purpose / Notes |
| :--- | :---: | :--- | :--- | :---: | :---: | :--- |
| `M1, M2, M3, M4` | 4 | `CSD18540Q5B (60V 100A)` | **C86513** | $1.12 | $4.48 | Buck-Boost FETs (Replaces 40V 5N04 parts) |
| `Q5, Q6` | 2 | `BSC040N10NS5 (100V 100A)` | **C534334** | $0.85 | $1.70 | Input Protection N-MOSFETs |
| `Q4` | 1 | `HSBA0119 (-40V -100A)` | **C53244070** | $0.42 | $0.42 | Battery Path P-MOSFET |
| `R16, R22` | 2 | `0.001 Ω 5W 4-Pad` | **C41382682** | $0.89 | $1.78 | NEW WSK2512 Kelvin Shunt Resistors |
| `U14` | 1 | `LM74502HDDFR` | **C5219202** | $1.15 | $1.15 | Ideal Diode Controller |
| `U4` | 1 | `LM5163 (100V Buck)` | **C2873264** | $1.85 | $1.85 | Control Rail Regulator |
| `U3` | 1 | `CP2102-GMR` | **C6568** | $1.95 | $1.95 | USB-UART Transceiver |
| `U9` | 1 | `LMV393 Dual Comp` | **C7984** | $0.22 | $0.22 | Hardware OVP/OCP Comparator |
| `U10` | 1 | `INA213AIDCKR` | **C129346** | $0.92 | $0.92 | Current Sense Amplifier |
| `L2` | 1 | `3.3 uH 12x10mm Inductor` | **C49308530** | $1.45 | $1.45 | Main Power Inductor |
| `D2` | 1 | `SMBJ60CA 600W TVS` | **C19077593** | $0.18 | $0.18 | Input TVS Surge Diode |

---

## 3. Summary Recommendation

1. **Proceed with the 65 In-Stock Items ($64.70 USD):** Place the order for all matched MOSFETs, shunts, inductors, connectors, and passives.
2. **For MCU `U1` & Controller `U6`:** Use the `STM32G473CEU6` chips in hand or order `STM32G431CBU6` and `LM51772` directly from TI.com/Mouser.
3. **For standard 0805 resistors:** Swap the 11 backordered/unmatched resistor part numbers to active Uniroyal/Yageo 0805 1% alternatives directly in the LCSC cart.

---

## 4. 2026-09-17 Current BOM Reconciliation Addendum

This addendum supersedes the 2026-09-14 snapshot for purchasing purposes. The current authority is `docs/rev-one-bom.md`, which now differs from the old quote in several material ways:

- `D2` is no longer the old `SMBJ60CA` / `C19077593`; the current BOM is `SMBJ48CA` / `C151264`, and live LCSC stock was verified at 8,705.
- `R30` is no longer a 40.2 kOhm sourcing problem; it is now a 0 Ohm CDC-disable strap (`C17888`) per DEC-060, so the old order-guide row is obsolete.
- `U1` sourcing was resolved after the old quote: keep `STM32G431CBU6` in the BOM and source it from Mouser/DigiKey rather than treating LCSC status as the deciding input.
- `J8` is a genuinely new line item added after the old quote. It has never been through an LCSC quote and needs a part selection first; current direction is an off-the-shelf breakout-module solution on the remote I2C temperature-sensor header.

### 4.1 Passive Re-check Against the Current BOM

No BOM changes were made in this pass. The table below records live status for the exact current-BOM LCSC numbers and, where needed, a same-spec alternate for user approval.

| Designator | Current BOM target | Current BOM LCSC | Live status of current BOM code | Same-spec alternate found in stock | Result |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `C20` | 180 pF, 0805, C0G, 50 V | `C1794` | Resolves to Samsung 15 pF C0G 0805 and is not available now; not the BOM part | `CC0805JRNPO9BN181` (`C113826`), YAGEO, 25,750 in stock; also `0805CG181J500NT` (`C501859`), FH, 2,550 in stock | Open for BOM substitution approval |
| `R20` | 499 kOhm, 0805, 1% | `C119857` | Current code could not be verified as a valid live match for the BOM row | `0805W8F4993T5E`, UNI-ROYAL, 64,700 in stock | Open for BOM substitution approval |
| `R28` | 51.7 kOhm, 0805, 1% | `C119851` | HTTP 404 / no current live match | No same-spec in-stock 0805 1% exact-value replacement found in this pass. Closest exact-value hit was `RT0603DRE0751K7L`, 0603, +/-0.5%, 4,900 in stock, which is not package-compatible | Still unresolved |
| `R29` | 20.5 kOhm, 0805, 1% | `C119842` | HTTP 404 / no current live match | `ERJ6ENF2052V` (`C2092167`), PANASONIC, 4,300 in stock | Open for BOM substitution approval |
| `R31` | 2.43 kOhm, 0805, 1% | `C119853` | Resolves to a through-hole 200 Ohm carbon-film resistor; not the BOM part | `0805W8F2431T5E`, UNI-ROYAL, 5,000 in stock | Open for BOM substitution approval |
| `R7, R10, R61` | 20 kOhm, 0805, 1% | `C17523` | Resolves to 2.37 kOhm and is out of stock; not the BOM part | `0805W8F2002T5E`, UNI-ROYAL, 2,296,400 in stock | Open for BOM substitution approval |
| `R14` | 330 kOhm, 0805, 1% | `C17651` | Resolves to 38.3 Ohm and is out of stock; not the BOM part | `0805W8F3303T5E`, UNI-ROYAL, 104,100 in stock | Open for BOM substitution approval |
| `R15` | 2 kOhm, 0805, 1% | `C17537` | Resolves to 20.5 kOhm and is out of stock; not the BOM part | `0805W8F2001T5E` (`C17604`), UNI-ROYAL, 4,708,600 in stock | Open for BOM substitution approval |
| `R23` | 75 kOhm, 0805, 1% | `C17743` | Resolves to 523 Ohm and is out of stock; not the BOM part | `0805W8F7502T5E`, UNI-ROYAL, 709,200 in stock | Open for BOM substitution approval |
| `R24, R62` | 5.1 kOhm, 0805, 1% | `C17700` | Resolves to 442 Ohm and is out of stock; not the BOM part | `0805W8F5101T5E`, UNI-ROYAL, 2,665,400 in stock | Open for BOM substitution approval |
| `R51` | 2.7 kOhm, 0805, 1% | `C17546` | Resolves to 21.5 Ohm and is out of stock; not the BOM part | `0805W8F2701T5E`, UNI-ROYAL, 1,710,800 in stock | Open for BOM substitution approval |

### 4.2 Section 2 Cross-Check Against the Current BOM

The old Section 2 silicon/power-stage list is still substantially valid against the current BOM after the 2026-09-17 decisions, with one important identity correction:

- Still matches current BOM and was live-verified in stock in this pass: `M1-M4 C86513`, `Q5/Q6 C534334`, `Q4 C53244070`, `R16/R22 C41382682`, `U3 C6568`, `U4 C2873264`, `U9 C7984`, `U10 C129346`, `U14 C5219202`, `L2 C49308530`.
- `D2` does not match the old Section 2 line anymore. The old guide still shows `SMBJ60CA` / `C19077593`; the current BOM correctly uses `SMBJ48CA` / `C151264`.
- `J2` was separately resolved after the old quote and is no longer a backorder-driven action item; current BOM still uses `C3020560`, and Task 3 already confirmed live availability.

### 4.3 What Still Needs User Approval or Follow-Up

1. Approve any desired BOM substitutions for the 10 passive rows with valid same-spec alternates found above.
2. Leave `R28` open for a future sourcing pass unless a verified 51.7 kOhm, 0805, 1% live LCSC part is found.
3. Select the actual `J8` remote temperature-sensor module/header BOM item next; this line is new, not merely stale.