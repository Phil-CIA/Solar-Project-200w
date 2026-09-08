Continue the Solar Project 200W Rev 0 bring-up from `SOLAR_HANDOFF_2026-09-08.md`. Read that handoff before proposing or performing any hardware operation.

Current stop state: the bench session is paused for handoff. The key finding is UVLO dropout at startup: near `PV_IN_POS=5.99 V`, `EN_UVLO` reaches about `1.27 V`, switching begins, `EN_UVLO` falls first, `PV_IN_POS` follows, `SS_ATRK` does not visibly ramp, and switching stops after about `80 us`. `STATUS_BYTE` remains `0x48`; no `0x58` IOUT status was observed. Keep PV off/disconnected until the next session.

With PV off/disconnected, verify at the board: MCU `CTRL_3V3`, common `PWR_NEG` to ST-LINK, `NRST` released high, and STM32 `PA13`/SWDIO and `PA14`/SWCLK continuity/no shorts. Then repeat only the exact approved serial-bound read-only connection. Do not flash unless the exact identity succeeds.

The authoritative current design artifact is `hardware/kicad/solar-project/Solar Project Rev0 - Manufactured First Build.net`, exported 2026-09-07 04:36:50. It confirms separate MODE and ILIMCOMP nets, each pulled to U6 VCC2 through its own 100 kOhm resistor (`R32` and `R65`). Do not use the stale July `Solar Project.net` for diagnosis.

The OLED U6-status firmware was built and flashed previously after exact target verification; it displays the cached U6 status and poll-failure count. Ordinary LM51772 I2C reads and named, reversible configuration writes are now authorized for the next integration session. Before each write, record the register, old value, new value, expected effect, and rollback/power-cycle condition. Never write LM51772 `CLEAR_FAULTS` at `0x03`.

FP4 remains not passed. Earlier runs showed packeted switching and source limiting without regulation. The prior SPICE model used about 242 uF output capacitance, but the manufactured Rev 0 output bank is approximately 1100 uF, so correct the model before drawing further startup-current conclusions.

Next session: keep PV off, update the LTspice model to approximately 1100 uF, then use the I2C bus to read back the controller configuration/status baseline. Follow with only named, reversible configuration integration writes if needed, and verify each write by readback. Do not increase the current limit or alter hardware. The low-energy CH1=`EN_UVLO`, CH2=`SS_ATRK`, CH3=`PV_IN_POS` capture remains the bench check for whether PV droop precedes EN_UVLO dropout.

`LO1`/`LO2` tracking U6 `VCC2` during the prior below-UVLO observation is treated as an internal LM51772 driver-domain behavior, not proof of external backfeed. Do not re-open that theory without new evidence.

U6 R28=51.7 kOhm, R29=20.5 kOhm, R33=0 ohm confirmed undisturbed. VOUT setpoint is confirmed at approximately 12.83 V (R3=66.5k/R25=5.62k, V_ref=1.00V). Keep `PA8`, `PB5`, and `PA5` low; never drive `PB11`. No real panel, battery, inverter, AC, or uncontrolled load.

Use only ST-LINK serial `B55B5A1A0000000020AFF501`, Device ID `0x469`, and the exact CP210x `USB\VID_10C4&PID_EA60\0001` on COM5. Stop on any identity mismatch; do not select another COM port.

