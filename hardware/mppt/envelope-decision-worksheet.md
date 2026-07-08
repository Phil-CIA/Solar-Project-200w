# MPPT Envelope Decision Worksheet

Last updated: 2026-07-06
Purpose: close DEC-004 (panel wiring) and DEC-003 (MPPT topology) with traceable calculations.

## 1. Input Data (Fill In)

| Parameter | Symbol | Value | Unit | Source |
|---|---|---|---|---|
| Panel rated power (single) | P_panel | 100 | W | Renogy Amazon product page |
| Panel Vmp (single) | Vmp_panel | 19.97 | V | Renogy Amazon product page |
| Panel Imp (single) | Imp_panel | 5.01 | A | Renogy Amazon product page |
| Panel Voc (single) | Voc_panel | 22.79 | V | Renogy Amazon product page |
| Panel Isc (single) | Isc_panel | 5.31 | A | Renogy Amazon product page |
| Number of panels | N | 2 | count | System baseline |
| Battery nominal voltage | Vbat_nom | 12.8 | V | Baseline |
| Battery charge voltage target | Vchg | 14.4 to 14.6 | V | Typical 4S LiFePO4 planning assumption pending battery datasheet |
| BMS continuous charge current limit | Ibat_cont | TBD | A | Q-001 closure |
| Planning charger current cap (temporary) | Ichg_cap | 15 | A | DEC-011 |
| Efficiency assumption | eta | 0.90 to 0.95 | ratio | Engineering assumption |

## 2. Wiring Option Calculations

### 2.1 Parallel Option (2P)

- Vmp_parallel = Vmp_panel
- Imp_parallel = N * Imp_panel
- Voc_parallel = Voc_panel
- Isc_parallel = N * Isc_panel
- P_parallel = Vmp_parallel * Imp_parallel

Calculated with current panel data:
- Vmp_parallel = 19.97V
- Imp_parallel = 10.02A
- Voc_parallel = 22.79V
- Isc_parallel = 10.62A
- P_parallel = approximately 200W

### 2.2 Series Option (2S)

- Vmp_series = N * Vmp_panel
- Imp_series = Imp_panel
- Voc_series = N * Voc_panel
- Isc_series = Isc_panel
- P_series = Vmp_series * Imp_series

Calculated with current panel data:
- Vmp_series = 39.94V
- Imp_series = 5.01A
- Voc_series = 45.58V
- Isc_series = 5.31A
- P_series = approximately 200W

## 3. Charger-Side Implications

Estimated battery-side charging current:

I_charge_est = (P_in * eta) / Vchg

Quick check baseline reference:
- For 200W class at 4S LiFePO4, expected peak is around 12A to 13A.

Calculated planning current with these panels:
- At 200W, 90% efficiency, and 14.6V charge target: I_charge_est = approximately 12.3A
- At 200W, 95% efficiency, and 14.4V charge target: I_charge_est = approximately 13.2A

## 4. Decision Table

| Criterion | Parallel (2P) | Series (2S) | Notes |
|---|---|---|---|
| Input voltage stress margin | Lower voltage stress | Higher voltage stress | 2S requires comfortable margin above 45.58V Voc plus cold-weather headroom |
| Input current stress margin | Higher current stress | Lower current stress | 2P doubles PV-side operating current and short-circuit current |
| Cold Voc risk margin | Lower risk | Must be explicitly checked | 2S requires verification that worst-case cold Voc stays below selected controller and switch absolute max |
| MPPT control complexity | Lower input-range complexity | Cleaner buck candidate if voltage margin is acceptable | 2S naturally favors a buck-only charger into 4S battery |
| Protection complexity | Higher PV current path burden | Higher voltage-rating burden | Trade current-handling complexity against voltage-rating complexity |
| Noise/EMI risk tendency | TBD | TBD | Qualitative early assessment |
| Compatibility with 15A planning cap | Compatible | Compatible | Both are compatible with estimated 12A to 13A battery-side charge current |

## 5. Topology Selection Aid (DEC-003)

Use selected wiring envelope from Section 4 first, then score:

| Topology | Envelope Fit | Control Complexity | Component Stress | Expandability | Recommendation |
|---|---|---|---|---|---|
| Buck | Strong nominal fit; requires low-irradiance headroom review | Lower | Lower switch current on PV side, but higher input-voltage class | Good for current baseline | Preferred pending low-light/headroom confirmation |
| Buck-boost | Works across wider PV/battery overlap conditions | Higher | Higher complexity and broader stress envelope | More flexible but heavier architecture | Fallback only if buck headroom proves inadequate |

### 5.1 Minimum Buck Feasibility Checks

- Check 1: worst-case cold Voc for 2S input remains below selected controller and switch absolute maximum with margin.
- Check 2: minimum useful PV operating voltage in low-light conditions remains above battery charge voltage plus converter/control headroom.
- Check 3: startup, UVLO, and dropout behavior are acceptable when PV power is weak but non-zero.
- Check 4: project priority remains learning simplicity first, not maximum marginal-light extraction.

## 6. Final Decision Record

- DEC-004 outcome:
  - Selected wiring: Series (2S)
  - Rationale: User-selected baseline and preferred engineering path because it lowers PV-side current and supports a buck-oriented charger architecture into the 4S battery.
  - Risks accepted: Higher PV input voltage and cold-Voc margin must be designed explicitly.

- DEC-003 outcome:
  - Selected topology: Buck preferred, pending final closure
  - Rationale: Series PV baseline gives nominal PV voltage substantially above LiFePO4 charge voltage, which strongly favors a buck path while keeping architecture simpler.
  - Risks accepted: Final confirmation still required for controller voltage class, startup behavior, and low-irradiance headroom edge cases.

## 7. Signoff

- Prepared by: Phil + Copilot
- Date: 2026-07-06
- Decision log updated: [ ] Yes [ ] No
- Related files updated:
  - [x] docs/decisions-log.md
  - [x] docs/phase-1-kickoff.md
  - [x] hardware/mppt/interface-assumptions.md
