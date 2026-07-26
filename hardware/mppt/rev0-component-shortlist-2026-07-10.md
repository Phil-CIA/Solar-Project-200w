# Rev 0 Component Shortlist (2026-07-10)

Owners: Phil + Copilot
Purpose: lock a practical Rev 0 candidate set for currently drawn schematic blocks while keeping revision flexibility.

## 1. Design Envelope Used For Margin Checks

- PV baseline: 2S Renogy 100W string, 45.58 Voc nominal at STC.
- Conservative PV voltage class for Rev 0 parts: 40 V minimum on the synchronous switch path, with U2 overvoltage shutdown providing the primary input protection behavior.
- Battery baseline: 4S LiFePO4, 12.8 V nominal, up to charge-voltage region.
- Current planning bound: 15 A continuous (Q-001 still open).

## 2. Rev 0 Candidate Table

| Ref / Function | Candidate (Rev 0) | Minimum Rating Target | Margin Check Basis | Package / Footprint Class | Sourcing Risk | Status |
|---|---|---|---|---|---|---|
| Q1 high-side switch | N-channel MOSFET, 40 V class, low Rds(on), >= 30 A class | Vds >= 40 V, Id >= 30 A | TI calculator points to about 25.6 V Vds and U2 shuts down on overvoltage, so 40 V class is sufficient without paying for unused headroom | TO-220 or D2PAK class (final pick during footprint lock) | Medium | Shortlisted |
| D3 freewheel diode (asynchronous path) | Schottky diode, 40 V to 60 V class, >= 10 A average, surge-capable | Vr >= 40 V, If(avg) >= 10 A | Voltage headroom on switch-node events, current class aligned to Rev 0 asynchronous path | TO-220AC / DPAK power diode class | Medium | Shortlisted |
| L1 buck inductor (live schematic ref: L2) | Shielded inductor, currently HC3-1R0-R (1uH), low DCR, high-current class | Isat >= 15 A, Irms >= design ripple current | Live schematic `L2` maps to HC3-1R0-R; vendor data indicates about 78 A Irms / 78 A Isat and about 0.42 mΩ DCR, so current class is acceptable. Remaining check is value/ripple/control-loop fit. | Through-hole drum or SMD shielded power inductor class | Medium | Shortlisted |
| C4 input decoupling | Ceramic X7R 10 uF, 63 V | 63 V DC | Covers 50 V worst-case input envelope with standard MLCC margin | 1210 or larger MLCC class | Low | Shortlisted |
| C5 input HF decoupling | Ceramic X7R 1 uF to 4.7 uF, 63 V | 63 V DC | Ripple support near switch loop without unnecessary over-rating | 0805/1206 MLCC class | Low | Shortlisted |
| C6 output bulk | Low-ESR electrolytic or polymer, 100 uF to 220 uF, >= 35 V | 35 V DC | >2x over battery domain voltage | Radial electrolytic or SMD polymer class | Low | Shortlisted |
| C7 output bulk / parallel damping | Low-ESR electrolytic/polymer or ceramic mix, 47 uF to 220 uF, >= 35 V | 35 V DC | Output ripple and transient support with thermal margin | Same class as C6 or mixed bank | Low | Shortlisted |
| R27/R28 PV divider path | Precision resistor pair for PV scaling (10:1 current placeholder ratio) | Resistor voltage and dissipation within divider limits | Maintains ADC input range intent on 3.3 V domain with filter headroom | 0603 or 0805, 1% metal film class | Low | Shortlisted |
| R2/R3 battery divider path | Precision resistor pair for battery scaling (10:1 placeholder ratio) | 1% tolerance minimum | Stable ADC scaling and calibration simplification | 0603 or 0805, 1% metal film class | Low | Shortlisted |
| F1 PV input fuse | Time-delay fuse, 10 A class, 5x25 format (match installed holder footprint) | DC voltage rating >= 63 V and time-current curve consistent with PV path/inrush policy | Protects front-end path before switch stage; holder is placed, fuse element still needs exact part lock | 5x25 holder footprint is the active mechanical constraint | Low | Shortlisted |
| D1 reverse-polarity element | Series Schottky rectifier, 60 V / 10 A class | Vr >= 60 V, If >= 10 A | Matches Rev 0 reverse-polarity strategy and the stated input envelope | TO-220 / power diode class | Medium | Shortlisted |
| D2 surge suppressor | TVS diode, 64 V standoff class (SMBJ/SMCJ family) | Standoff appropriate to 2S PV with transient clamp margin | Supports REQ-SAF-004 transient strategy | SMBJ or SMCJ class | Low | Shortlisted |

## 3. Open Selection Items Before Hard Part Lock

1. Confirm exact Q1 and D3 thermal dissipation with switching frequency target and expected duty range.
2. Confirm L1 value using ripple target after final PWM frequency is fixed on STM32G431 control path; keep live schematic `L2` unless control-loop or thermal evidence requires a different inductance.
3. Confirm resistor divider absolute values against final ADC front-end filtering and sample timing.
4. Close Q-001 to finalize current-sense element class and any fuse-current refinements.
5. Lock the exact 5x25 fuse element part number with final blow characteristic + voltage rating for the selected holder footprint.

### 3.1 F1 Holder-Matched Rule (Locked)

Mechanical rule:
- Use 5x25 fuse elements to match the currently selected holder footprint.

Preference note:
- Bussmann MDL remains a personal preference family, but it is parked for this revision because it does not match the active 5x25 holder format.

Electrical lock checks:
1. Confirm selected 5x25 fuse has DC voltage rating >= 63 V for the stated 50 V worst-case input envelope.
2. Confirm time-delay behavior and current rating (10 A class baseline) remain consistent with startup/inrush behavior.

## 4. Pre-Routing Readiness Note

This shortlist is suitable for symbol-to-footprint refinement and layout planning. It is not yet a procurement lock BOM.

- Allowed now: footprint assignment, placement experiments, thermal-zone planning.
- Blocked until open items close: final MOSFET/diode exact PN, final inductor value lock, and current-sense topology lock.
