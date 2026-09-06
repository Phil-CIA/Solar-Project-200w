Continue the Solar Project 200W Rev 0 bring-up from `SOLAR_HANDOFF_2026-09-05.md`. Read that handoff before proposing or performing any hardware operation.

Current stop state: SWD communication is blocked. On 2026-09-06, the approved ST-LINK `B55B5A1A0000000020AFF501` was detected and reported 3.23 V VTref, but serial-bound, read-only STM32CubeProgrammer connection failed with `Unable to get core ID` / `No STM32 target found`. Device ID `0x469`, STM32G47x/G48x/G414 family, and 512 KB NVM were not confirmed; no flash or register write occurred. Do not choose another probe, target, or COM port.

With PV off/disconnected, verify at the board: MCU `CTRL_3V3`, common `PWR_NEG` to ST-LINK, `NRST` released high, and STM32 `PA13`/SWDIO and `PA14`/SWCLK continuity/no shorts. Then repeat only the exact approved serial-bound read-only connection. Do not flash unless the exact identity succeeds.

An OLED U6-status screen was added and built but not flashed: `firmware/bringup/baremetal/stm32g4_rev0_stage1.c` built successfully (`text=7364`, `data=4`, `bss=1160`). Once SWD identity is restored, flash/verify it with PV still disconnected, then confirm the OLED row `U6 xx Nnnn`; it performs a read-only U6 `STATUS_BYTE` poll every two seconds and does not access `CLEAR_FAULTS`.

FP4 remains not passed: at about 6.0 V, LM51772 switching began in approximately 610 kHz bursts spaced about 200 ms apart, but `CHG_OUT_POS` only reached 0.986 V with a 200 ohm preload and 1.36 V with a 1 kohm preload. The source reached approximately 229-266 mA current limiting. Do not increase the current limit. Only after SWD/OLED recovery, recreate the bounded approximately 5.98 V / 200 mA / 1 kohm-preload setup for no more than 30 seconds and collect `FLT`, `STATUS_BYTE`, source-current-limit state, input current, and `CHG_OUT_POS`; then turn source off and discharge.

`LO1`/`LO2` tracking U6 `VCC2` during the prior below-UVLO observation is treated as an internal LM51772 driver-domain behavior, not proof of external backfeed. Do not re-open that theory without new evidence.

U6 R28=51.7 kOhm, R29=20.5 kOhm, R33=0 ohm confirmed undisturbed. VOUT setpoint is confirmed at approximately 12.83 V (R3=66.5k/R25=5.62k, V_ref=1.00V). Keep `PA8`, `PB5`, and `PA5` low; never drive `PB11`. No real panel, battery, inverter, AC, or uncontrolled load.

Use only ST-LINK serial `B55B5A1A0000000020AFF501`, Device ID `0x469`, and the exact CP210x `USB\VID_10C4&PID_EA60\0001` on COM5. Stop on any identity mismatch; do not select another COM port.

