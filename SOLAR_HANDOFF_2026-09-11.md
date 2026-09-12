# Solar Project Session Handoff - 2026-09-11

## Session closeout

This session completed a broad Rev 1 LM51772 converter review against the current saved netlist and corrected several stale assumptions:
- Evaluated Rev 0 bench testing and synthesized the root cause of input collapse (inrush to uncharged output caps) into the formal [rev-one-bench-learning-matrix.md](docs/rev-one-bench-learning-matrix.md).
- Confirmed the Battery-First architecture (DEC-025) and hardware default-off inhibit switch Q7 (2N7002K on `EN_UVLO`).
- Mapped and verified 4-pad Kelvin shunts R16 (battery current) and R22 (inductor current) with zero net collisions.
- Updated feedback dividers R3 and R18 to 76.8 kOhm for 14.6 V LiFePO4 regulation target.
- Restored all missing/generic component values across the entire schematic (R30=40.2k, R28=51.7k, C18=560uF, C3=100uF, C19/C21/C30/C31=47uF, U8=SSD1306_OLED, compensation network, etc.).
- Cleaned up property whitespace and achieved 0 ERC Errors on the authoritative `Solar Project Rev1.kicad_sch`.

## Rev 1 current decisions

- Converter basis: 125 W continuous, 150 W maximum PV sizing power, 8-10 A nominal charge current, approximately 12 A target ceiling.
- Qualified PV operating range remains provisionally 20-48 V. The LM51772 approximately 6 V UVLO is only a protection boundary.
- Intended output target: 14.6 V for 4S LiFePO4, pending final battery/BMS confirmation.
- Battery-First architecture (DEC-025): battery establishes `V_out` precharge, avoiding 0 V inrush input collapse.
- Dedicated test points and switch-node ground spring lands added (DEC-031).
- Split USB power/telemetry: 2nd USB port deferred to Rev 2; inline USB-C tester (KWS-2303C) adopted for bench work.
- R16 / R22 current shunts: custom 4-pad split footprint (`R_2512 Shunt 4 pad resistor`) with dedicated power pins (1 & 4) and Kelvin sense pins (2 & 3).
- L2: MMPC1206-3R3MT, 3.3 uH, 5.8 mOhm, provisional HOLD.
- Input bank: C19/C21/C30/C31, four 47 uF / 100 V aluminum-polymer capacitors. 188 uF nominal, 10.4 A RMS combined rated current using the 2.6 A per-part datasheet value. HOLD for ESR, thermal sharing, and effective behavior.
- Output bank: C18=560 uF, C3=100 uF, C34/C35=2.2 uF, C33=100 nF, approximately 664.4 uF nominal. HOLD for C18 data, ESR, ripple, inrush, and stability.
- Q5/Q6: both changed to BSC040N10NS5, 100 V, 4 mOhm max class, 58 nC typical gate charge, TDSON-8/SuperSO8 5x6 package. U14 LM74502HDDFR owns their shared gate.
- Q5/Q6 gate path: R2=100 kOhm gate-to-SRC default-off pull-up, R65=10 Ohm U14 gate resistor, D5=1SMA4742A gate-source zener. HOLD for gate charge, VGS clamp energy, thermal behavior, body-diode orientation, and switching overshoot.
- M1-M4 gate paths now have R69/R70/R71/R72=10 Ohm series resistors: HO1->M1, LO1->M2, HO2->M4, LO2->M3.
- LM51772 external ISET: MODE and ISET are separated. R68=1.3 kOhm in series with R46=82 kOhm, total 83.3 kOhm to the common PWR_NEG ground. Nominal current-limit target is approximately 12 A; HOLD for tolerance, offset, and calibration.
- LM51772 EN/UVLO: automatic hardware startup with firmware supervision selected. R23=75 kOhm, R61=20 kOhm, R60=1.5 kOhm; calculated rising threshold approximately 5.61 V and falling threshold approximately 5.38 V. HOLD for startup/inrush, no-battery behavior, and firmware disable validation.
- LM51772 AGND, PGND, and GND are tied together on PWR_NEG, consistent with the datasheet grounding recommendation.
- F2 remains REWORK because the current holder/clip style is not the desired standard. J3 remains HOLD as a prototype-only battery connector.
- Complete Rev 0 to Rev 1 translation matrix synthesized in [rev-one-bench-learning-matrix.md](docs/rev-one-bench-learning-matrix.md).

## Rev 1 remaining blockers

1. F2: select the desired standard fuse clips and actual fuse, then verify spacing, rating, interrupt capability, curve, and thermal/contact limits.
2. Q5/Q6: verify BSC040N10NS5 manufacturer pad drawing against the footprint and confirm common-source back-to-back diode orientation.
3. U14 gate test: execute the new [rev-one-u14-low-voltage-gate-test.md](docs/test/rev-one-u14-low-voltage-gate-test.md).
4. Q5/Q6 dynamic validation: measure GATE-SRC, D5 clamp, gate timing, temperature, and VDS overshoot before 48 V operation.
5. LM51772: verify R30=40.2 kOhm CDC configuration mapping; complete compensation, current-sense, thermal, and current-limit validation.
6. Capacitors: close input ESR/thermal/effective-capacitance evidence and output C18 rating/ESR/ripple/inrush/stability evidence.
7. Control-power branch remains REWORK pending startup and USB reverse/backfeed behavior.

## Rev 0 bench evidence to mine next session

### Passed and reusable improvements

- USB/CP210x path was repaired and COM5 communication worked at 115200 8N1.
- STM32 target identity and SWD procedure were made explicit: approved ST-LINK serial, device ID, target voltage, and NVM checks.
- Firmware boot was corrected by fixing the Thumb reset vector and programming nSWBOOT0 while retaining nBOOT0; later testing reached 5/5 cold-power cycles and 5/5 reset cycles with PA8/PB5/PA5 safely low.
- Software I2C routing was confirmed as PB6=data/SDA and PB7=clock/SCL; AHT20 at 0x38 and SSD1306 at 0x3C worked.
- D8 was moved from U6 VCC2 to USB VBUS after the Rev 0 power-domain problem. Rev 1 should preserve explicit USB/control-rail isolation and local decoupling.
- M1-M4 installation planning established useful passive checks, pin/orientation inspection, and staged power authorization.

### Rev 0 findings that must become Rev 1 improvements

- U2/U5 common-source gate-control topology was invalid in its original form. Do not reuse that gate-drive assumption; Rev 1 Q4/U14 and any protection FET control must be reviewed from the actual driver reference and gate-source behavior.
- A residual D8-to-U6 VCC2 backfeed path was suspected after the USB/VCC2 rework. Rev 1 must explicitly verify USB cannot back-power converter bias rails or create hidden gate-driver activity.
- PGND/PWR_NEG grounding and U6 local pin-area rework mattered. Rev 1 layout must preserve the common-ground intent while controlling high-current gate-driver return currents.
- U6 I2C/FLT behavior and local solder/attachment boundaries were not initially obvious. Rev 1 bring-up should include bus ownership, device acknowledgment, fault-state capture, and no accidental register writes.
- FP4 first switching did not pass. At about 6 V input, switching packets were observed around 610 kHz, but the source current-limited at about 229-266 mA, output reached only about 0.99-1.36 V with 200 Ohm/1 kOhm preload, and regulation did not reach the expected target. Do not increase source current until the current-limit/startup cause is understood.
- The first FP4 scope setup used ground-referenced probes and math for high-side gate-source observation. This was acceptable only for a low-voltage first observation and is not adequate for switch-node overshoot acceptance. Rev 1 must use differential probing or an isolated scope for HO-SW and VDS measurements.
- The Rev 0 EN/UVLO measurement did not immediately match the divider calculation because the fitted IC and in-circuit lower-leg loading mattered. Rev 1 EN/UVLO review should use the actual pin loading and measured thresholds, not resistor-only arithmetic.
- Rev 0 staged testing required hard source current limits, passive preload, attended operation, discharge verification, and explicit stage gates. Carry those controls into Rev 1 bench testing.

## Next session plan

1. Read the completed Rev 0 bench documents and build a compact evidence table: observation, confirmed cause, recommended improvement, Rev 1 applicability, and verification method.
2. Separate improvements into three classes: schematic change, PCB/layout change, and test-procedure/firmware change.
3. Do not import a Rev 0 workaround into Rev 1 without checking whether Rev 1 topology is different.
4. Prioritize changes that prevent hidden power/backfeed paths, unsafe gate activity, invalid measurement setups, and ambiguous startup states.
5. Update Rev 1 documents first; edit the Rev 1 schematic only after the improvement has a concrete owner, evidence, and validation step.
6. End the next session with a short Rev 0-to-Rev 1 improvement matrix and an updated Rev 1 test plan.

## Important working rules

- KiCad schematic may be open with unsaved user edits. Ask for save confirmation before schematic edits.
- Treat the regenerated current netlist as authoritative for connectivity checks.
- Do not call a component PASS when a required rating, tolerance, thermal, transient, or layout criterion is only assumed.
- Keep prototype-only connector and fuse decisions separate from final-release decisions.
- Use HOLD when the schematic is plausible but bench or datasheet evidence remains.
