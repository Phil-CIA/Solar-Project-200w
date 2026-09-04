Continue the Solar Project 200W Rev 0 bring-up from `SOLAR_HANDOFF_2026-09-04.md`. Read that handoff before proposing or performing any hardware operation.

Stop state: the board is unpowered and discharged. M1-M4 are installed `BSC030N04NSGATMA1` devices and passed installation, passive resistance/diode checks, visual inspection, and photo capture, but the populated bridge has never been powered. F1, U2, and U5 protection functions remain bypassed. Do not connect USB, ST-LINK, PV, a battery, an output source/load, a panel, an inverter, or AC. Do not command gate drive or switching.

U6 communication at `0x6A` is working with R28=51.7 kOhm, R29=20.5 kOhm, and R33=0 ohms. Historical pre-installation status was `0x58` at 6.49 V and `0x48` below UVLO at 5.0 V. Never access `CLEAR_FAULTS` at `0x03`. Keep `PA8`, `PB5`, and `PA5` low and never drive `PB11`.

The next task is documentation and review only: write a separate staged first-power plan covering the bypassed protection paths, source and output configuration, safe firmware state, scope strategy, bootstrap network, dead time, missing gate resistors, thermal/ringing checks, stop thresholds, and discharge procedure. Separate populated-board static power from any later switching stage. Do not execute the plan while drafting it.

Use only ST-LINK serial `B55B5A1A0000000020AFF501`, Device ID `0x469`, and the exact CP210x `USB\VID_10C4&PID_EA60\0001` on COM5. Stop on any identity mismatch; do not select another COM port.

