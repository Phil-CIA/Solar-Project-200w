# MPPT Buck Topology Notes

Last updated: 2026-07-06
Owners: Phil + Copilot
Status: decision-support note for closing DEC-003.

## 1. Current Position

Selected PV baseline:
- Two Renogy 100W panels in series
- Nominal PV operating point: 39.94V at 5.01A
- Nominal PV open-circuit voltage: 45.58V

Battery-side baseline:
- 4S LiFePO4
- Charge target planning range: 14.4V to 14.6V

This strongly favors a buck MPPT because the nominal PV voltage is well above the battery charge voltage.

## 2. Legitimate Concern: Low-Irradiance and Weak-Sun Operation

The remaining question is not whether buck works at nominal conditions. It does.

The real question is whether the series PV voltage stays high enough above battery charge voltage under weak-sun, hot-panel, startup, and partial-load conditions to avoid leaving useful energy unharvested.

That means DEC-003 should stay deferred until these checks are accepted:

1. The minimum useful PV operating voltage under expected low-light conditions is still above battery charging voltage plus converter headroom.
2. Startup behavior is acceptable when PV power is available but limited.
3. The controller approach can handle reduced margin without unstable hunting or premature dropout.

## 3. Buck-Preferred Decision Rule

Keep buck as the preferred topology if all of the following remain true:

1. Minimum practical PV operating voltage remains meaningfully above 14.6V plus control and conversion headroom.
2. Low-light conditions mainly reduce available current/power before they collapse PV voltage below charger usefulness.
3. System goal prioritizes a simpler, lower-risk learning design over maximum energy harvest in marginal conditions.

Reopen buck-boost only if one of these becomes true:

1. Measured or datasheet-backed weak-sun operation often pulls usable PV voltage near battery charge voltage.
2. The selected control method cannot recover gracefully from low-headroom conditions.
3. Field use case strongly prioritizes squeezing energy from marginal light over architecture simplicity.

## 4. Required Validation Before Closing DEC-003

1. Define a temporary minimum PV operating voltage review threshold for series mode.
2. Confirm controller voltage class comfortably exceeds worst-case cold Voc.
3. Confirm buck control strategy for startup, UVLO, and dropout behavior.
4. Record whether low-light extraction is a primary project goal or a secondary optimization.

## 5. Working Recommendation

Proceed with buck-oriented schematic exploration now.

Do not lock DEC-003 as fully decided until the low-irradiance/headroom review is explicitly documented.

## 6. Follow-On Files

- Update `hardware/mppt/envelope-decision-worksheet.md` when headroom criteria are chosen.
- Update `docs/decisions-log.md` when DEC-003 is fully closed.
- Update `hardware/mppt/interface-assumptions.md` if low-light targets change the design baseline.