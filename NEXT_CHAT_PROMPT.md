Continue the Solar Project 200W Rev 0 bring-up from `SOLAR_HANDOFF_2026-09-05.md`. Read that handoff before proposing or performing any hardware operation.

Current stop state: FP4 was attempted. At about 6.0 V, LM51772 switching began in approximately 610 kHz bursts spaced about 200 ms apart, but `CHG_OUT_POS` only reached 0.986 V with a 200 ohm preload and 1.36 V with a 1 kohm preload. The source reached approximately 229-266 mA current limiting; FP4 did not regulate and is not passed. Do not increase the current limit.

The immediate next measurement is read-only: after confirming the board is discharged, recreate the bounded approximately 5.98 V / 200 mA / 1 kohm-preload setup for no more than 30 seconds. Verify the exact approved ST-LINK first, then record U6 `STATUS_BYTE` using `firmware/scripts/rev0-openocd-u6-status.ps1`, `FLT` to `PWR_NEG`, source-current-limit state, input current, and `CHG_OUT_POS`; turn source off and discharge immediately afterward. Never write LM51772 `CLEAR_FAULTS` at `0x03`.

`LO1`/`LO2` tracking U6 `VCC2` during the prior below-UVLO observation is treated as an internal LM51772 driver-domain behavior, not proof of external backfeed. Do not re-open that theory without new evidence.

U6 R28=51.7 kOhm, R29=20.5 kOhm, R33=0 ohm confirmed undisturbed. VOUT setpoint is confirmed at approximately 12.83 V (R3=66.5k/R25=5.62k, V_ref=1.00V). Keep `PA8`, `PB5`, and `PA5` low; never drive `PB11`. No real panel, battery, inverter, AC, or uncontrolled load.

Use only ST-LINK serial `B55B5A1A0000000020AFF501`, Device ID `0x469`, and the exact CP210x `USB\VID_10C4&PID_EA60\0001` on COM5. Stop on any identity mismatch; do not select another COM port.

