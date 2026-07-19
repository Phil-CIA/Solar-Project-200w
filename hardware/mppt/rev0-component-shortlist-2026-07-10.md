# Rev 0 Component Shortlist (2026-07-10)

Owners: Phil + Copilot
Purpose: lock a practical Rev 0 candidate set for currently drawn schematic blocks while keeping revision flexibility.

## 1. Design Envelope Used For Margin Checks

- PV baseline: 2S Renogy 100W string, 45.58 Voc nominal at STC.
- Conservative PV voltage class for Rev 0 parts: 100 V minimum on switching and PV protection path.
- Battery baseline: 4S LiFePO4, 12.8 V nominal, up to charge-voltage region.
- Current planning bound: 15 A continuous (Q-001 still open).

## 2. Rev 0 Candidate Table

| Ref / Function | Candidate (Rev 0) | Minimum Rating Target | Margin Check Basis | Package / Footprint Class | Sourcing Risk | Status |
|---|---|---|---|---|---|---|
| Q1 high-side switch | N-channel MOSFET, 100 V class, low Rds(on), >= 30 A class | Vds >= 100 V, Id >= 30 A | >2x over PV Voc baseline and thermal headroom vs 15 A planning current | TO-220 or D2PAK class (final pick during footprint lock) | Medium | Shortlisted |
| D3 freewheel diode (asynchronous path) | Schottky diode, 100 V class, >= 10 A average, surge-capable | Vr >= 100 V, If(avg) >= 10 A | Voltage headroom on switch-node events, current class aligned to Rev 0 asynchronous path | TO-220AC / DPAK power diode class | Medium | Shortlisted |
| L1 buck inductor | Shielded inductor, 33 uH to 68 uH, low DCR, >= 15 A Isat | Isat >= 15 A, Irms >= design ripple current | Supports 15 A planning bound; exact value to tune after PWM frequency lock | Through-hole drum or SMD shielded power inductor class | Medium | Shortlisted |
| C4 input decoupling | Ceramic X7R 10 uF, 100 V | 100 V DC | >2x vs PV nominal operating voltage class | 1210 or larger HV MLCC class | Low | Shortlisted |
| C5 input HF decoupling | Ceramic X7R 1 uF to 4.7 uF, 100 V | 100 V DC | HV ripple support near switch loop | 0805/1206 HV MLCC class | Low | Shortlisted |
| C6 output bulk | Low-ESR electrolytic or polymer, 100 uF to 220 uF, >= 35 V | 35 V DC | >2x over battery domain voltage | Radial electrolytic or SMD polymer class | Low | Shortlisted |
| C7 output bulk / parallel damping | Low-ESR electrolytic/polymer or ceramic mix, 47 uF to 220 uF, >= 35 V | 35 V DC | Output ripple and transient support with thermal margin | Same class as C6 or mixed bank | Low | Shortlisted |
| R27/R28 PV divider path | Precision resistor pair for PV scaling (10:1 current placeholder ratio) | Resistor voltage and dissipation within divider limits | Maintains ADC input range intent on 3.3 V domain with filter headroom | 0603 or 0805, 1% metal film class | Low | Shortlisted |
| R2/R3 battery divider path | Precision resistor pair for battery scaling (10:1 placeholder ratio) | 1% tolerance minimum | Stable ADC scaling and calibration simplification | 0603 or 0805, 1% metal film class | Low | Shortlisted |
| F1 PV input fuse | Time-delay fuse, 10 A class (holder or cartridge style) | Current rating consistent with PV path and inrush policy | Protects front-end path before switch stage | 5x20 holder or automotive blade holder class | Low | Shortlisted |
| D1 reverse-polarity element | Series Schottky rectifier, 100 V / 10 A class | Vr >= 100 V, If >= 10 A | Matches Rev 0 reverse-polarity strategy (DEC-012 deferred upgrade path) | TO-220 / power diode class | Medium | Shortlisted |
| D2 surge suppressor | TVS diode, 64 V standoff class (SMBJ/SMCJ family) | Standoff appropriate to 2S PV with transient clamp margin | Supports REQ-SAF-004 transient strategy | SMBJ or SMCJ class | Low | Shortlisted |

## 3. Open Selection Items Before Hard Part Lock

1. Confirm exact Q1 and D3 thermal dissipation with switching frequency target and expected duty range.
2. Confirm L1 value using ripple target after final PWM frequency is fixed on STM32G431 control path.
3. Confirm resistor divider absolute values against final ADC front-end filtering and sample timing.
4. Close Q-001 to finalize current-sense element class and any fuse-current refinements.

## 4. Pre-Routing Readiness Note

This shortlist is suitable for symbol-to-footprint refinement and layout planning. It is not yet a procurement lock BOM.

- Allowed now: footprint assignment, placement experiments, thermal-zone planning.
- Blocked until open items close: final MOSFET/diode exact PN, final inductor value lock, and current-sense topology lock.
