# Control Converter Selection Worksheet (2026-07-14)

Owners: Phil + Copilot  
Purpose: execute Q-004 closure using battery-bus-native control-power criteria.

## 1. Requirement Inputs To Lock First

Fill these before scoring candidates.

| Parameter | Target | Notes |
|---|---|---|
| BAT_BUS_POS minimum operating voltage | 11.2 V | First-pass Rev 0 protect-battery floor assumption for control domain |
| BAT_BUS_POS startup floor | 10.8 V | Converter must cold-start and regulate from this threshold |
| BAT_BUS_POS maximum voltage | 14.8 V | Covers high charge region with margin for 4S LiFePO4 context |
| UVLO falling threshold | 11.0 V | Cut converter before deep battery draw in control path |
| UVLO rising threshold | 11.8 V | ~0.8 V hysteresis target to prevent chatter |
| CTRL_3V3 load current (steady) | 120 mA | STM32 + sensing + logic baseline with moderate margin |
| CTRL_3V3 load current (peak) | 300 mA | Startup and transient headroom target |
| Max acceptable ripple at CTRL_3V3 | <= 30 mVpp | Keep ADC-friendly rail during PWM activity |
| Max acceptable quiescent current | <= 100 uA from BAT_BUS_POS | Idle battery drain target for always-connected control path |

## 2. Candidate Set

Minimum set for this session:

1. U2 current path
2. U4 current path
3. NEW_CANDIDATE_1 (U4-replacement class: wide-input, low-IQ buck, >=0.5 A output)

Optional:

4. NEW_CANDIDATE_2

## 3. Hard-Limit Gate Check (Pass/Fail)

Any FAIL rejects candidate before weighted scoring.

| Gate | U2 | U4 | NEW_CANDIDATE_1 | Notes |
|---|---|---|---|---|
| Starts and regulates at startup floor | PASS | PASS | PASS | U2/U4 symbol choices are within plausible electrical class; final bench proof required |
| Operates across full BAT_BUS_POS range | PASS | PASS | PASS | All three candidates can cover 10.8 V to 14.8 V class envelope |
| UVLO behavior supports cutoff strategy | FAIL | PASS | PASS | U2 path currently lacks explicit cutoff/UVLO ownership in implementation intent |
| No unacceptable USB_5V_IN backfeed risk | PASS | PASS | PASS | Requires maintained protected OR-ing boundary into CTRL_SUPPLY_IN |
| Rev 0 integration complexity acceptable | PASS | PASS | PASS | U4 and U4-class replacement are acceptable with bounded external network |

## 4. Weighted Scoring Matrix (Only For Gate-Pass Candidates)

Scoring scale: 1 (poor) to 5 (excellent).

| Criterion | Weight | U2 Score | U4 Score | NEW_CANDIDATE_1 Score | Notes |
|---|---:|---:|---:|---:|---|
| Input-range and UVLO fit | 0.35 | 2 | 5 | 4 | U2 path fails UVLO implementation criteria in current architecture state |
| Efficiency at expected operating points | 0.20 | 2 | 4 | 4 | U4-class devices are better aligned to always-on control path efficiency goals |
| Quiescent current | 0.15 | 2 | 5 | 4 | U4 value notes indicate ultra-low-IQ class |
| Ripple/noise and ADC compatibility | 0.10 | 3 | 4 | 3 | U4 class plus optional post-filter supports cleaner ADC rail intent |
| Integration risk and bring-up confidence | 0.10 | 3 | 4 | 3 | U4 already present in schematic as control-supply candidate |
| Availability and footprint/layout fit | 0.10 | 2 | 4 | 3 | U2 footprint is currently unset; U4 has a concrete footprint assigned |

Weighted total formula:
- Total = Sum(Score x Weight)

Weighted totals:
- U2 = 2.25 (gate fail)
- U4 = 4.55 (gate pass)
- NEW_CANDIDATE_1 = 3.80 (gate pass)

## 5. Decision Output

| Item | Result |
|---|---|
| Selected Rev 0 CTRL_3V3 owner | U4 path (`LM5163`) as Rev 0 architecture owner; exact PN can still be revised within U4-class constraints |
| Reason for selection | Best weighted score among gate-pass candidates and strongest alignment with battery-bus UVLO + low-IQ criteria |
| Why non-selected options were rejected | U2 fails UVLO/cutoff implementation criterion in current intent and has weaker footprint/readiness alignment |
| Required schematic/doc updates | Remove U2 as live CTRL_3V3 owner, tie active converter output ownership to U4 path, retain U2 only if explicitly marked non-owner/deprecated |
| Verification checks to queue next | Startup at 10.8 V, UVLO fall/rise behavior near 11.0/11.8 V, USB backfeed check, ripple capture under PWM load |

## 6. Source-Priority Behavior Statement (Draft)

Complete this after selecting owner:

- Runtime priority: PV/charger-domain feed into CTRL_SUPPLY_IN first when present; battery bus supplies control rail when runtime path drops below UVLO policy.
- Battery-assist behavior into CTRL_SUPPLY_IN: Allowed through protected OR-ing path and treated as primary native fallback for control continuity.
- USB bench fallback behavior: USB_5V_IN is bench-only fallback source into CTRL_SUPPLY_IN when PV/runtime path is absent.
- Reverse-backfeed prevention behavior: Keep diode/ideal-diode isolation so active control rail cannot back-drive USB_5V_IN.

## 7. Closure Checklist

- [x] Requirement inputs in Section 1 are filled.
- [x] Hard-limit gates completed and fails removed.
- [x] Weighted scores calculated for remaining candidates.
- [x] Single owner selected with rationale.
- [x] Q-004 decision status updated.
- [x] Notes aligned across decisions log, net plan, and schematic notes.
