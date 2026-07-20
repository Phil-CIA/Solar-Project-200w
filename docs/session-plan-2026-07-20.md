# Solar Project 200W - Session Plan (2026-07-20)

Owners: Phil + Copilot
Session intent: clean up the legacy single-MOSFET-era scaffolding now that LM51772 is the active control path, and decide what stays in hardware, what is firmware-only, and what is deleted.

## 1. Session Objective

Reduce the schematic and notes to the minimum set of functions the current LM51772-based architecture still needs.

Primary question for this cleanup pass:
- Does the function still need to exist, and if so, who owns it?

## 2. Cleanup Rules

Use this ownership split for every leftover item:

- Hardware owns safety-critical behavior, power-path behavior, and anything that must work even if firmware is absent.
- Firmware owns supervision, telemetry, sequencing, and non-safety policy.
- Delete any net, symbol, or note that only existed to support the older single-MOSFET concept and no longer serves the current architecture.

## 2.1 Default Ownership Matrix

Use these as the starting assumptions for the backlog review. Change them only if the schematic proves a different owner is required.

| Item | Default decision | Reason |
|---|---|---|
| `CTRL_PWM_MAIN` | Keep in hardware; firmware drives it | It is still the main controller output into the power stage. |
| `CTRL_EN_CHG` | Keep in hardware; firmware drives it | Enable remains a useful real control signal for the MCU. |
| `FAULT_OCP` | Keep in hardware; firmware mirrors it | OCP must exist as a protection signal even if firmware also reacts. |
| `FAULT_OVP` | Keep in hardware; firmware mirrors it | OVP must exist as a protection signal even if firmware also reacts. |
| `V_in_ON` | Delete unless the Q2 branch is intentionally retained | It only belongs to the older gate-control concept unless proven otherwise. |
| `U2` control-owner role | Delete or deprecate | Current docs already favor U4 as the Rev 0 `CTRL_3V3` owner. |
| `U6_VCC2` | Keep if U6 remains active | The LM51772 helper rail still needs a real decoupling path if used. |
| `U6_FLT` | Keep if U6 remains active | The fault output needs a pull-up and a destination. |
| `U6_SYNC` / `U6_DTRK` / `U6_RST` | Keep only if the strap is intentional | These are physical configuration pins, not firmware features. |

## 2.2 Likely Outcome By Item

This is the working answer before schematic inspection. If the schematic disagrees, change the row and record why.

| Item | Likely outcome | Follow-up |
|---|---|---|
| `CTRL_PWM_MAIN` | Keep | This is the live controller drive path into the power stage. |
| `CTRL_EN_CHG` | Keep | Keep as a real MCU-controlled enable line unless the schematic proves it is unused. |
| `FAULT_OCP` | Keep | Keep as a real protection signal and mirror it in firmware if useful. |
| `FAULT_OVP` | Keep | Keep as a real protection signal and mirror it in firmware if useful. |
| `V_in_ON` | Delete | Remove unless the legacy Q2 path is intentionally preserved. |
| `U2` control-owner role | Delete/deprecate | U4 is already the chosen Rev 0 owner path in the current docs. |
| `U6_VCC2` | Keep if U6 stays | Needs a real decoupler if LM51772 remains active. |
| `U6_FLT` | Keep if U6 stays | Needs a pull-up and a destination pin if U6 remains active. |
| `U6_SYNC` | Keep only if intentional | Keep only if it is still the chosen strap behavior. |
| `U6_DTRK` | Keep only if intentional | Keep only if the disabled-tracking strap is still desired. |
| `U6_RST` | Keep only if intentional | Keep only if the reset strap is still desired. |

## 2.3 Review Rule

- If the function still affects safe power behavior without firmware, it stays in hardware.
- If firmware only needs to observe it, mirror it in firmware but keep the hardware signal.
- If it only exists because of the old MOSFET-era path, delete it.

## 3. Session 1 - Sheet 2 Power-Stage Cleanup

Goal:
- Confirm the power stage only needs the modern controller-drive path.

Actions:
1. Review `hardware/mppt/buck-power-stage-wiring-map.md`.
2. Keep `CTRL_PWM_MAIN` as the only drive input needed for Q1.
3. Keep the asynchronous freewheel path for Rev 0.
4. Remove any stale control labels or notes that only supported the old MOSFET-only concept.

Stop condition:
- Sheet 2 no longer implies an old control path that competes with the LM51772 path.

## 4. Session 2 - Control And Fault Ownership Review

Goal:
- Decide which signals are MCU-commanded, which are hardware protection, and which are legacy holdovers.

Actions:
1. Review `hardware/mppt/sheet4-sensing-control-wiring-map.md` and `hardware/mppt/net-plan.md`.
2. Keep `CTRL_EN_CHG` if the MCU still needs an enable line.
3. Keep `FAULT_OCP` and `FAULT_OVP` as hardware protection signals, with firmware allowed to observe or log them.
4. Review `V_in_ON` and the `U2/Q2` branch and decide whether they are retired or intentionally retained.

Stop condition:
- Every control/fault net has a clear owner and no longer looks like leftover prototype glue.

## 5. Session 3 - U6 Helper Nets And ERC Confirmation

Goal:
- Finish the LM51772 helper nets only if they still serve a real hardware purpose.

Actions:
1. Review `hardware/mppt/lm851772-lm51772-migration-checklist.md`.
2. Close `U6_VCC2` and `U6_FLT` with real passives if U6 stays active.
3. Keep `U6_SYNC`, `U6_DTRK`, and `U6_RST` only if they are still intentional straps.
4. Re-export ERC and netlist, then compare against the backlog.

Stop condition:
- Any remaining helper net has a documented reason to exist.
- Any helper net without a reason is removed.

## 6. Expected Outputs

1. A cleaned-up schematic narrative that no longer references the old single-MOSFET concept unless it is intentionally retained.
2. A clear hardware vs firmware vs delete decision for `CTRL_PWM_MAIN`, `CTRL_EN_CHG`, `FAULT_OCP`, `FAULT_OVP`, `V_in_ON`, and the U2/Q2 holdover path.
3. A fresh ERC/netlist snapshot after the cleanup pass.
4. Updated notes in `hardware/mppt/schematic-notes.md` reflecting the ownership decisions.

## 7. Exit Criteria

- No leftover net exists only because it belonged to the older control concept.
- The docs clearly say whether each remaining function lives in hardware, firmware, or is deleted.
- The next session can begin from the cleaned list without re-litigating the same ownership questions.

## 8. Session 2 Review Order

Use this order when you start the control/fault review:

1. Confirm `CTRL_EN_CHG` ownership and keep it only if the MCU still needs a live enable line.
2. Confirm `FAULT_OCP` and `FAULT_OVP` remain hardware protection signals.
3. Inspect `V_in_ON` and the `U2/Q2` branch for deletion or explicit retention.
4. Record the result in `hardware/mppt/schematic-notes.md` before moving on.

### 8.1 Default Outcome For Session 2

| Item | Default outcome | Reason |
|---|---|---|
| `CTRL_EN_CHG` | Keep | The MCU still appears to need a real enable control into the charging path. |
| `FAULT_OCP` | Keep | Over-current must remain a hardware protection signal even if firmware also watches it. |
| `FAULT_OVP` | Keep | Over-voltage must remain a hardware protection signal even if firmware also watches it. |
| `V_in_ON` | Delete unless the legacy branch is intentionally retained | It still belongs to the older Q2 control concept unless proven otherwise. |
| `U2/Q2` branch | Delete or explicitly mark legacy | The current control architecture already favors the U4-wide-input path. |

### 8.2 Session 2 Edit Rule

- If the signal must protect power behavior without firmware, keep it in hardware.
- If firmware only needs to know about it, mirror it in firmware but do not remove the hardware signal.
- If it only exists because of the old prototype control path, remove it.

## 9. Session 3 Review Order

Use this order when you start the LM51772 helper-net pass:

1. Confirm whether `U6` is still an active block in the live schematic.
2. If yes, close `U6_VCC2` and `U6_FLT` first.
3. Then check whether `U6_SYNC`, `U6_DTRK`, and `U6_RST` are still intentional straps.
4. Re-export ERC and netlist only after the helper-net decision is settled.

### 9.1 Default Outcome For Session 3

| Item | Default outcome | Reason |
|---|---|---|
| `U6_VCC2` | Keep and close with a real decoupler | LM51772 gate-driver/bias helper rail still needs a proper capacitor path if U6 remains active. |
| `U6_FLT` | Keep and close with a pull-up | The fault output still needs a defined destination and pull-up if U6 remains active. |
| `U6_SYNC` | Keep only if intentional strap | Strap pins should only remain if they still match the chosen Rev 0 behavior. |
| `U6_DTRK` | Keep only if intentional strap | Dynamic tracking should only remain if it still matches the chosen Rev 0 behavior. |
| `U6_RST` | Keep only if intentional strap | Reset should only remain if it still matches the chosen Rev 0 behavior. |

### 9.2 Session 3 Edit Rule

- If U6 is still active, close the helper nets with real passives rather than leaving dangling placeholders.
- If a strap is not required for Rev 0, delete or retire it instead of keeping a decorative net.
- Re-export ERC/netlist only after the helper-net outcome is recorded.