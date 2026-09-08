# Solar Project 200W - Rev 0 Bring-Up Handoff (2026-09-08)

## Stop State

Stop the bench session here. PV output must be off and disconnected; USB/ST-LINK may remain connected only for low-energy control work. Do not raise PV voltage or source current, do not clear LM51772 faults, and do not leave the board energized unattended.

The current authoritative design artifact is `hardware/kicad/solar-project/Solar Project Rev0 - Manufactured First Build.net`, exported 2026-09-07 04:36:50 from `Solar Project Rev0 - Manufactured First Build.kicad_sch`. The older July `Solar Project.net` and older Rev0 PCB snapshot are stale for the MODE/ILIMCOMP rework and must not be used to diagnose the manufactured board.

## Current Main Finding - UVLO Dropout Before Soft-Start

At the controlled approximately 5.99 V PV point, the observed sequence is:

1. `EN_UVLO` reaches approximately 1.27 V.
2. Switching begins.
3. `EN_UVLO` falls first, with `PV_IN_POS` following within the measurement resolution.
4. `SS_ATRK` does not visibly ramp.
5. Switching stops after roughly 80 us total, with the main EN_UVLO fall about 50 us into the burst and stop roughly 30 us later.
6. The controller retries in packeted bursts.

Below the switching threshold, `EN_UVLO`, `SS_ATRK`, and `PV_IN_POS` are steady and no waveform is present. This establishes that the abnormal behavior starts at the UVLO-to-startup transition. It is not yet proven whether the EN_UVLO fall is caused by PV input droop, an EN divider/transient path issue, or another internal startup inhibit; do not infer nanosecond causality beyond the scope/probe limits.

Powered pre-switch snapshot at PV approximately 5.004 V:

```text
PV_IN_POS       5.004 V
VCC1            5.0023 V
VCC2            4.7726 V
MODE            4.7726 V
ILIMCOMP        4.7299 V
COMP            0.7495 V
SS_ATRK         0.003 V
EN_UVLO         1.0275 V
FLT             0.0032 V
```

Snapshot near startup at PV approximately 5.99 V:

```text
PV_IN_POS       5.99 V
VCC1            5.002 V
VCC2            4.7736 V
MODE            4.73 V
ILIMCOMP        4.73 V
COMP            0.7495 V
SS_ATRK         0.0015 V
EN_UVLO         1.27 V
FLT             0.033 V
STATUS_BYTE     0x48
CHG_OUT_POS     0.635 V
```

`STATUS_BYTE=0x48` remains `OFF=1, INPUT=1, IOUT=0`; no `0x58` IOUT status was observed. MODE and ILIMCOMP are near VCC2 before startup and are not being clamped low in the measured state.

## Authoritative Manufactured-Build Netlist Correction

The current manufactured-build netlist shows separate nets:

```text
Net-(U6-MODE):
  U6 pin 7 MODE
  R32 pin 1

Net-(U6-ILIMCOMP_ISET):
  U6 pin 16 ILIMCOMP_ISET
  R65 pin 2

U6_VCC2:
  U6 pin 29 VCC2
  C29 pin 1
  C45 pin 1
  R32 pin 2
  R65 pin 1
```

`R32=100 kOhm` and `R65=100 kOhm`. The earlier conclusion that MODE and ILIMCOMP were shorted together came from a stale July netlist/PCB export and is withdrawn.

## SPICE Model Correction

The earlier startup model used approximately 242 uF output capacitance (`220 uF + 22 uF`), while the manufactured Rev 0 output capacitor bank is approximately **1100 uF**. The earlier simulation therefore cannot represent the real output-cap charging transient or startup demand.

The next SPICE session must use:

- `COUT` approximately 1100 uF
- documented/estimated actual capacitor ESR/ESL
- 2 kOhm preload used in the latest bench observation
- 6 V input and the bounded 200 mA source limit
- measured 610 kHz switching behavior only as an observation, not as a complete LM51772 model
- UVLO dropout/retry behavior and the actual 80 us startup packet envelope

Do not use the previous SPICE output as a hardware prediction until this capacitance correction is implemented.

## Bench Results Retained

- FP0 passed.
- FP1 passed: approved ST-LINK identity, safe firmware state, all four gates low before bridge startup.
- FP2 was attempted and aborted under the old all-gates-low interpretation; the LO/VCC2 relationship is treated as internal controller-domain behavior, not proof of external backfeed.
- FP4 first switching was attempted but did not pass: output did not regulate toward approximately 12.83 V and the source current-limited.
- Earlier measured bursts were approximately 610 kHz internal cycles grouped into approximately 200 us packets, repeating on the millisecond scale.
- Informal, non-record-quality observation at approximately 10 V and above 1.5 A source limit raised output only to roughly 4 V while the supply still limited. Do not treat this as a formal test result, but it argues against simply adding more source current as the fix.

## Next Session Plan

1. Keep PV off and discharge the board.
2. Treat the manufactured-build netlist as authoritative.
3. Correct the LTspice model to approximately 1100 uF output capacitance before more simulation conclusions.
4. At the next controlled bench point, use CH1=`EN_UVLO`, CH2=`SS_ATRK`, CH3=`PV_IN_POS` and capture only the first threshold-crossing packet. Record voltage minima and event order, not just waveform appearance.
5. Decide whether PV input droop precedes EN_UVLO dropout. If PV is stable while EN_UVLO falls, inspect the EN divider/transient path. If PV falls first, inspect source/input delivery and local input energy.
6. Do not increase PV current limit, do not alter the preload or hardware, and never write LM51772 `CLEAR_FAULTS` at `0x03`.

## Hardware and Identity Limits

- Approved ST-LINK serial: `B55B5A1A0000000020AFF501` only.
- Expected target: Device ID `0x469`, STM32G47x/G48x/G414, 512 KB NVM.
- Approved CP210x: `USB\VID_10C4&PID_EA60\0001` on COM5 only.
- Keep `PA8`, `PB5`, and `PA5` low. Never drive `PB11`.
- No real panel, battery, inverter, AC, or uncontrolled load.
- Do not probe high-side or switch nodes with a ground clip; all Rigol probe grounds remain at `PWR_NEG`.

## Files Included in This Handoff

- `Solar Project Rev0 - Manufactured First Build.kicad_sch`
- `Solar Project Rev0 - Manufactured First Build.kicad_pcb`
- `Solar Project Rev0 - Manufactured First Build.kicad_pro`
- `rigol_capture.py`
- `rigol_scope_capture_2026-09-07.txt`
- `NEXT_CHAT_PROMPT.md`
- `SOLAR_HANDOFF_2026-09-08.md`
