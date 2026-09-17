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