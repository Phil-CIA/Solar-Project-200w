Continue the Solar Project 200W Rev 0 bring-up from `SOLAR_HANDOFF_2026-09-05.md`. Read that handoff before proposing or performing any hardware operation.

Stop state: PV source disconnected and bled to ≤0.12 V, but the USB/ST-LINK connection state at the very end of the prior session was not confirmed. Before anything else, verify USB and ST-LINK are physically disconnected, then measure `VCC2` (U6 pin 29) and `LO1`/`LO2`-to-`PWR_NEG` at rest.

Open blocking safety finding: with PV fully removed, `LO1`/`LO2` and U6 `VCC2` (pin 29) still read 4.77 V with real drive strength (a 10 kOhm load only pulled it to 4.11 V, not floating). Root cause is unconfirmed. Leading hypothesis: a previously-documented, never-fully-isolated ~300 kOhm path from D8 pad 2 to U6 pin 29 may be backfeeding `VCC2` from USB `VBUS`. This is not a documented LM51772 "fail-safe" behavior (checked against datasheet Section 7.3.9, which targets VOUT discharge, not LO1/LO2). This finding blocks any FP2 re-attempt, FP4, and FP3 until resolved.

FP0 and FP1 of the staged first-power plan passed on 2026-09-04. FP2 was attempted at 5.0 V/20 mA, hit a real stop condition (gates at 4.3-4.8 V vs. the 0.1 V threshold), and was correctly aborted — it did not pass. Do not treat FP2 as cleared.

U6 (LM51772-Q1) R28=51.7 kOhm, R29=20.5 kOhm, R33=0 ohm confirmed undisturbed. VOUT setpoint is confirmed by hardware analysis at ~12.83 V (external FB divider R3=66.5k/R25=5.62k, V_ref=1.00V). FP4 preload (two 100 Ohm/10 W resistors in series = 200 Ohm) remains valid once FP2 is resolved. Never write LM51772 `CLEAR_FAULTS` at `0x03`. Keep `PA8`, `PB5`, and `PA5` low and never drive `PB11`.

Use only ST-LINK serial `B55B5A1A0000000020AFF501`, Device ID `0x469`, and the exact CP210x `USB\VID_10C4&PID_EA60\0001` on COM5. Stop on any identity mismatch; do not select another COM port. If SWD "unable to connect" persists across tools/speeds despite a plausible target voltage, re-confirm the board is actually powered before deeper troubleshooting.

