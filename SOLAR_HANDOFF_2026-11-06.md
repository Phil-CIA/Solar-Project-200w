# Solar Project 200W - Handoff (2026-11-06)

Date: 2026-11-06

## Summary

Phil reports a significant Rev 1 LM51772 bring-up improvement after correcting the input-voltage divider and changing C20 in the peak-current-sense filter. The output rose from about 3.4 V to 4.87 V and remained there during the reported test. This is bench-reported evidence, not an independently observed or repeated measurement.

## Latest bench report

- The input-voltage divider was corrected to a 1/20 ratio at the input-voltage sense pin. Phil reports the converter now appears to operate without the earlier apparent input-voltage boundary. The prior approximately 9.5 V observation is not established as a device or design threshold.
- In the `U6 test 5000 10V` test, Vout was approximately 3.4 V before the C20 change.
- C20 was changed to 1 nF as a current-sense-noise test.
- After that change, Vout rose and remained at 4.87 V.
- The reported external output-feedback values are R3 = 76.8 kOhm and R25 = 20 kOhm, giving a nominal target of approximately 4.84 V with the LM51772's 1 V external-FB reference. The measured 4.87 V is consistent with that target.

The close match between the reported output and divider target makes C20/current-sense filtering a strong lead. The result does not by itself prove the noise source or exact mechanism; current-limit events, current-sense waveforms, and repeatability have not yet been documented here. Do not treat the reported test as full-power or protection validation.

The checked-in Rev 1 PCB/BOM records C20 as 180 pF. The 1 nF value is a reported physical rework and has not been applied to the schematic, PCB, or BOM in this handoff. Preserve that distinction until the board change is confirmed and the filter is checked against the controller's current-sense timing requirements.

## Configuration clarification

- R33 = 0 Ohm on ADDR/SLOPE/CFG1 selects I2C-enabled operation at address 0x6A.
- That does not select internal output-voltage feedback. The FB/SEL_intFB pin independently selects external versus internal feedback at startup.
- With the external R3/R25 divider selected, the divider sets Vout; the I2C VOUT target does not override it. I2C can still configure other controller settings.
- The approximately 9.5 V behavior was a bench observation, not a confirmed LM51772 UVLO threshold. Phil reports the input-divider correction changed that behavior.

## TI E2E family research

Related controllers have forum reports of similar symptoms, but these do not establish that our board has the same root cause:

- [LM5175: Weird switching behaviour / high Iq at some input voltages](https://e2e.ti.com/support/power-management-group/power-management/f/power-management-forum/1032992/lm5175-weird-switching-behaviour-high-iq-at-some-input-voltages) describes input-dependent erratic switching, ringing, elevated no-load current, and heating. No confirmed resolution was established from the retrieved thread.
- [LM5175: Output very weak until reaching target voltage](https://e2e.ti.com/support/power-management-group/power-management/f/power-management-forum/1659182/lm5175-lm5175-output-very-weak-until-reaching-target-voltage) describes output stalling below target and abnormal switching. TI discussed gate-drive/supply loading, timing-pin disturbance, and layout; a final confirmed fix was not established from the retrieved thread.
- [LM5175 bring-up](https://e2e.ti.com/support/power-management-group/power-management/f/power-management-forum/855356/lm5175-lm5175-bring-up) identifies current-sense resistor parasitic inductance, filtering, input/output resonance, and loop compensation as checks for unstable operation.
- [LM51772 DTRK / feedback-selection discussion](https://e2e.ti.com/support/power-management-group/power-management/f/power-management-forum/1610574/lm51772-dtrk) confirms that internal versus external feedback is selected at startup and cannot be switched dynamically through I2C.

TI's LM51772 datasheet recommends a differential CSA/CSB filter selected for minimum on-time, short Kelvin sense connections, and consideration of the current-sense resistor's parasitic inductance. C20 at 1 nF should therefore be evaluated against the actual operating range and protection response before higher-power operation.

## TI product status

As checked on 2026-11-06, TI's [LM51772 product page](https://www.ti.com/product/LM51772) continues to list the controller and its documentation. The page lists a TI application brief dated 2026-02-20, [Implementing Reverse Current Operation in the LM51772 Using MODE Function](https://www.ti.com/lit/pdf/SBAA772). No TI discontinuation or phase-out notice was found in the reviewed product information; this is not a formal lifecycle guarantee.

## Next useful checks

1. Repeat the same low-energy test with the input-divider correction and C20 = 1 nF, recording input voltage at the board, Vout, source current, load, and controller status without changing other variables.
2. Record the as-fitted C20 and confirm the physical divider values; keep the reported board rework distinct from repository design files.
3. If safely equipped, compare the CSA/CSB differential signal at U6 with the voltage directly across R22. Avoid grounding either switching-referenced sense node with an earth-grounded probe clip.
4. Before increasing power, verify that the 1 nF differential filter preserves the required current-limit response over the intended operating range.

No powered test, hardware change, or firmware change was performed while preparing this handoff.
