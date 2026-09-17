# MPPT Envelope Decision Worksheet

Last updated: 2026-09-17 (DEC-051 panel-wiring correction: 2S superseded by 2P)
Purpose: close DEC-004 (panel wiring) and DEC-003 (MPPT topology) with traceable calculations.

**2026-09-17 correction (DEC-049):** the panel figures below were sourced from a Renogy Amazon product-page listing and were WRONG. Phil supplied the actual panel nameplate/spec-sheet values (screenshot), which are now used as the authoritative input data. The corrected Voc is materially higher than the old figure and changes several downstream margin conclusions - see DEC-049 in `docs/decisions-log.md`.

**2026-09-17 follow-up (DEC-050):** panel identified as Renogy `RSP100DCT` ("Renogy Bifacial Solar Panel 100W N-Type, 16BB", ASIN `B0H4Z6ZJ2D`, https://www.amazon.com/dp/B0H4Z6ZJ2D). Registered in `docs/datasheets/manifest.yaml` as `renogy-rsp100dct` (web-listing citation; no downloadable PDF datasheet was found). The listing discloses a temperature coefficient of -0.29%/degC (labeled as the panel's power/`Pmax` coefficient, not explicitly `Voc`-specific - used here as a working approximation). See Section 1a below for the resulting cold-Voc reference table.

**2026-09-17 correction (DEC-051): active wiring baseline changed from 2S to 2P.** Re-reading the LM51772 datasheet's own Recommended Operating Conditions showed `V(VIN)` recommended max is only 48V typ/55V max - the corrected 57.0V STC 2S string Voc already exceeded that RECOMMENDED range at STC alone, not just the 59V abs max under cold conditions. Phil selected rewiring the array 2P (parallel) instead, which drops `PV_IN_POS` to single-panel Voc (28.5V STC, ~33.9V at -40 degC) - comfortably inside U6's recommended range and D2's 64V standoff at every disclosed operating temperature, with no new parts needed. Section 2.2's 2S numbers are retained below for reference/history; **Section 2.1 (2P) is now the active baseline.** PV-side current doubles under 2P (Isc ~8.90A, Imp ~8.48A) - this reopens current-based margins (F1 fuse, input-bank ripple, Q4/D1 ID) for re-verification, tracked in `docs/rev-one-closure-plan-2026-09-16.md` Track A.

## 1. Input Data (Fill In)

| Parameter | Symbol | Value | Unit | Source |
|---|---|---|---|---|
| Panel model | - | Renogy `RSP100DCT` | - | Amazon ASIN `B0H4Z6ZJ2D` (2026-09-17, DEC-050) |
| Panel rated power (single) | P_panel | 100 | W | Panel spec sheet (2026-09-17, DEC-049; supersedes earlier unattributed Amazon-listing figure) |
| Panel Vmp (single) | Vmp_panel | 23.6 | V | Panel spec sheet (2026-09-17, DEC-049) |
| Panel Imp (single) | Imp_panel | 4.24 | A | Panel spec sheet (2026-09-17, DEC-049) |
| Panel Voc (single) | Voc_panel | 28.5 | V | Panel spec sheet (2026-09-17, DEC-049) |
| Panel Isc (single) | Isc_panel | 4.45 | A | Panel spec sheet (2026-09-17, DEC-049) |
| Panel max system voltage | Vsys_max | 600 | VDC | Panel spec sheet (2026-09-17, DEC-049) |
| Panel max series fuse rating | Ifuse_max | 25 | A | Panel spec sheet (2026-09-17, DEC-049) |
| Panel temperature coefficient (Pmax, used as Voc proxy) | Tcoef | -0.29 | %/degC | Amazon listing marketing text (2026-09-17, DEC-050); not explicitly labeled as Voc-specific |
| Panel operating temperature range | - | -40 to +85 | degC | Panel spec sheet (2026-09-17, DEC-049) |
| Number of panels | N | 2 | count | System baseline |
| Battery nominal voltage | Vbat_nom | 12.8 | V | Baseline |
| Battery charge voltage target | Vchg | 14.4 to 14.6 | V | Typical 4S LiFePO4 planning assumption pending battery datasheet |
| BMS continuous charge current limit | Ibat_cont | TBD | A | Q-001 closure |
| Planning charger current cap (temporary) | Ichg_cap | 15 | A | DEC-011 |
| Efficiency assumption | eta | 0.90 to 0.95 | ratio | Engineering assumption |

## 1a. Cold-Voc Reference Table (2026-09-17, DEC-050)

Using `Voc(T) = Voc_panel x (1 + |Tcoef| x (25 - T))` for T < 25 degC (STC reference), applied to the 2S string:

| Temperature | Single-panel Voc | 2S string Voc | Margin vs D2 (`SMBJ60CA`, 64V Vrwm) | Margin vs U6 VIN/BIAS (59V abs max) |
|---|---|---|---|---|
| 25 degC (STC) | 28.5 V | 57.0 V | ~10.9% | ~3.4% |
| -10 degC (mild, plausible everyday cold) | ~31.4 V | ~62.8 V | ~1.9% (razor-thin) | **EXCEEDED by ~3.8V** |
| -25 degC | ~32.6 V | ~65.3 V | **EXCEEDED by ~1.3V** | **EXCEEDED by ~6.3V** |
| -40 degC (panel's rated operating minimum) | ~33.9 V | ~67.7 V | **EXCEEDED by ~3.7V** | **EXCEEDED by ~8.7V** |

This table replaces the earlier generic "+15-20% cold-weather extrapolation" placeholder used across DEC-041/042/043/046 with a disclosed-data-backed estimate. It confirms those earlier approximations were reasonable in magnitude, but sharpens the conclusion: **U6's VIN/BIAS absolute maximum is exceeded starting at just -10 degC**, well within the panel's own disclosed -40 to +85 degC operating range - not only under an extreme-cold edge case. D2's standoff margin is razor-thin at -10 degC and exceeded by -25 degC. Caveat: -0.29%/degC is disclosed as the panel's power (`Pmax`) coefficient, not explicitly a `Voc`-specific coefficient; typical crystalline-silicon panels have a slightly smaller-magnitude `Voc` coefficient than `Pmax`, so this may modestly over-state the true cold-Voc rise - but no `Voc`-specific figure is published to refine this further.

## 2. Wiring Option Calculations

### 2.1 Parallel Option (2P) - active baseline as of DEC-051 (2026-09-17)

- Vmp_parallel = Vmp_panel
- Imp_parallel = N * Imp_panel
- Voc_parallel = Voc_panel
- Isc_parallel = N * Isc_panel
- P_parallel = Vmp_parallel * Imp_parallel

Calculated with corrected panel data (2026-09-17, DEC-049):
- Vmp_parallel = 23.6V
- Imp_parallel = 8.48A
- **Voc_parallel = 28.5V (STC; ~33.9V at the panel's rated -40 degC minimum per the DEC-050 temp-coefficient method) - this is the PV_IN_POS voltage U6/D2/C7 now see**
- Isc_parallel = 8.90A (this is the new PV-side current basis - reopens F1 fuse sizing, input-bank ripple current, and Q4/D1 ID margin for re-verification, per DEC-051)
- P_parallel = approximately 200W

### 2.2 Series Option (2S) - superseded by DEC-051, retained for reference/history only

- Vmp_series = N * Vmp_panel
- Imp_series = Imp_panel
- Voc_series = N * Voc_panel
- Isc_series = Isc_panel
- P_series = Vmp_series * Imp_series

Calculated with corrected panel data (2026-09-17, DEC-049):
- Vmp_series = 47.2V
- Imp_series = 4.24A
- **Voc_series = 57.0V (STC nameplate; supersedes the old 45.58V figure - cold-weather Voc will be higher still and remains unverified)**
- Isc_series = 4.45A
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
| Input voltage stress margin | Lower voltage stress | Higher voltage stress | 2S requires comfortable margin above 57.0V Voc (corrected 2026-09-17, DEC-049) plus cold-weather headroom |
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

- DEC-004 outcome (superseded 2026-09-17 by DEC-051):
  - Selected wiring: **Parallel (2P)**, superseding the original Series (2S) baseline.
  - Rationale: 2S's 57.0V STC string Voc (DEC-049 corrected figure) exceeded the LM51772's own recommended VIN operating range (48-55V) at STC alone, and its 59V absolute max under cold conditions (DEC-050) - not fixable without either extra clamp parts, a different converter IC, or a lower operating-voltage-class wiring choice. 2P keeps PV_IN_POS at single-panel Voc (28.5V STC / ~33.9V cold), comfortably inside both U6's recommended range and D2's 64V standoff, with no new parts required.
  - Risks accepted: PV-side current doubles (Isc ~8.90A, Imp ~8.48A) - F1 fuse sizing, input-bank (C19/C21/C30/C31) ripple current, and Q4/D1 ID margin, all previously verified only under 2S current levels, must be re-verified under the new 2P current level before this can be treated as closed.

- DEC-003 outcome:
  - Selected topology: Buck preferred, pending final closure
  - Rationale: Series PV baseline gives nominal PV voltage substantially above LiFePO4 charge voltage, which strongly favors a buck path while keeping architecture simpler. Re-evaluate this rationale against the new 2P baseline's lower PV voltage (28.5V vs the ~14.4-14.6V battery charge target) - still buck-favorable, but with less voltage headroom than 2S provided; the existing four-switch LM51772 buck-boost topology is retained regardless (DEC-024), so this affects headroom framing only.
  - Risks accepted: Final confirmation still required for controller voltage class, startup behavior, and low-irradiance headroom edge cases.

## 7. Signoff

- Prepared by: Phil + Copilot
- Date: 2026-07-06
- Decision log updated: [ ] Yes [ ] No
- Related files updated:
  - [x] docs/decisions-log.md
  - [x] docs/phase-1-kickoff.md
  - [x] hardware/mppt/interface-assumptions.md
