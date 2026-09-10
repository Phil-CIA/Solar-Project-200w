Continue the Solar Project Rev 1 converter rebaseline from `SOLAR_HANDOFF_2026-09-10.md`. Read it and `docs/rev-one-component-derating-review.md` before proposing schematic changes.

Rev 1 is a targeted reconfiguration of Rev 0, not a clean-sheet redesign. Retain one LM51772 four-switch stage, the LM74502H PV protection concept, sensing functions, R16 output-current-shunt function, and STM32 supervisory architecture unless analysis proves a change necessary.

The accepted envelope is 125 W continuous charger output, 150 W maximum PV operating power, 8 A to 10 A nominal charge current, approximately 12 A hardware protection, and at least 20 A BMS charge capability.

The current converter basis is 20 V qualified PV input, provisional 48 V maximum, 14.6 V intended charge output, 12 A hardware ceiling, 3.3 uH MMPC1206-3R3MT L2, CSD18540Q5B M1-M4, 2N7002K Q2/Q3, three 47 uF input capacitors with 3.8 A RMS each, and matched larger 1 mOhm R16/R22 shunts.

The OLED U6-status firmware was built and flashed previously after exact target verification; it displays the cached U6 status and poll-failure count. Ordinary LM51772 I2C reads and named, reversible configuration writes are now authorized for the next integration session. Before each write, record the register, old value, new value, expected effect, and rollback/power-cycle condition. Never write LM51772 `CLEAR_FAULTS` at `0x03`.
First confirm the user has saved the open schematic. Then review the proposed paired R3/R18 = 76.8 kOhm update for 14.6 V, using R25/R17 = 5.62 kOhm, without overwriting unsaved work. After that, close Q4 gate control, output-capacitor, R16/R22 Kelvin, F2, and ERC/DRC items. Do not replace the converter topology.

Use DEC-023 through DEC-029 as the controlling decisions. Do not edit KiCad until the power-and-control path and acceptance checks are agreed.

Next session: keep PV off, update the LTspice model to approximately 1100 uF, then use the I2C bus to read back the controller configuration/status baseline. Follow with only named, reversible configuration integration writes if needed, and verify each write by readback. Do not increase the current limit or alter hardware. The low-energy CH1=`EN_UVLO`, CH2=`SS_ATRK`, CH3=`PV_IN_POS` capture remains the bench check for whether PV droop precedes EN_UVLO dropout.

`LO1`/`LO2` tracking U6 `VCC2` during the prior below-UVLO observation is treated as an internal LM51772 driver-domain behavior, not proof of external backfeed. Do not re-open that theory without new evidence.

U6 R28=51.7 kOhm, R29=20.5 kOhm, R33=0 ohm confirmed undisturbed. VOUT setpoint is confirmed at approximately 12.83 V (R3=66.5k/R25=5.62k, V_ref=1.00V). Keep `PA8`, `PB5`, and `PA5` low; never drive `PB11`. No real panel, battery, inverter, AC, or uncontrolled load.

Use only ST-LINK serial `B55B5A1A0000000020AFF501`, Device ID `0x469`, and the exact CP210x `USB\VID_10C4&PID_EA60\0001` on COM5. Stop on any identity mismatch; do not select another COM port.
If any Rev 0 bench work is proposed, first reread `SOLAR_HANDOFF_2026-09-08.md` and preserve all safety, identity, probing, and no-`CLEAR_FAULTS` restrictions. PV remains off and disconnected at handoff.

