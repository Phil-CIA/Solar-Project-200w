# Solar Project 200W — Handoff (2026-11-05)

Date: 2026-11-05

Summary
- Rev 1 U6 (LM51772) bring-up on the bench: J1 supply at 19 V / 0.25 A limit, J2 USB for control and COM5, F1 jumper fitted, no PV, battery, or loads.
- Fixed `VIN ADC init FAIL`, found and fixed a real-time START-window bug, and narrowed the 5 V output cap to a startup OC event. Root cause is not yet found.

Firmware state
- File: `firmware/bringup/rev1_g473_stages/stage_aht20_display_uart.c`. These changes are uncommitted.
- Flashed image: `solar_rev1_stage_aht20_display_uart.bin`, 24,128 B, SHA-256 `F5F1B539E70E4614B3EC28898638266106B2256BC184719FA01B35AF10A700EA`. The readback hash matched. Erase range is `0x08000000` + `0x6000` (sectors 0..11); each flash had a full-range backup in `firmware/bringup/rev1_g473_stages/backup/`.
- VIN ADC: PA4 (`SENSE_PV_V`) is ADC2_IN17 on the STM32G473. The code now uses ADC2 channel 17 and ADC clock mode CKMODE=01 (HCLK/1). The old code used ADC1 channel 4 (PA3) with the async clock, which has no source by default.
- Real-time clock: `delay_ms()` is an uncalibrated busy loop and runs several times slower than its name. Before the fix, the "1000 ms" START window lasted about 15 s real time, and the 100 ms log lines arrived about 1.57 s apart. `U6 TEST` now uses the DWT cycle counter (16 MHz): re-inhibit at a real 1000 ms from PB12 release, real-ms log timestamps, and no OLED redraw during START. The 16 MHz assumption was confirmed against the host clock. Earlier "≤1000 ms" results from this feature, including the 3.3 V and 5 V "peak" numbers, ran with the long window.
- OC pre-clear: before releasing PB12, `U6 TEST` writes `0x18` to STATUS_BYTE (0x78), which clears OC and leaves the idle UV set.
- `U6 FAULTS` still uses the address-only CLEAR_FAULTS (0x03) write. It returns OK but did not clear a latched OC.

Circuit and datasheet findings
- Enable path: PB12 high turns on Q7 and holds EN/UVLO low. PB12 is a safety clamp, not the start control. With the clamp released, the R23 (75 kΩ) / R61 (20 kΩ) + R60 (1.5 kΩ) divider enables U6 at about 5.6 V of PV_IN. At 19 V, EN/UVLO is about 4.2 V.
- EN/UVLO measured 0.263 V with PB12 high. That is well under the 1.25 V threshold, but above DEC-026's `≤0.2 V` check. Likely a clamp resistance of about 1 kΩ (Q7 through R60?). Review the Q7 wiring.
- The UV flag (STATUS_BYTE bit 3, INPUT) is set whenever EN/UVLO is below its falling threshold. UV at idle is the clamp working, not a fault.
- OC (bit 4, IOUT) means the inductor peak current limit was reached. It is latched until cleared.
- Measured at idle with J1 at 19 V: U6 VIN = 18.99 V, BIAS (pin 40) = 18.99 V, nFLT LOW. ISNSP–ISNSN was not measured (needs a scope).
- Config snapshot: `0A=0x8C` (ILIM 70 mV, 7 A at 10 mΩ; the reset default is `0x64`), `D3=0x20`, `D7=0x28`, `D8=0x84` (DIV20), `DA=0xFF`, `0D/0C=0x00FA` (5.000 V).

Test results
- `U6 TEST 5000 19` with VIN 18.9–19.4 V: Vout stalls below 5 V. 4.76 V peak was seen over the old long window, and 4.45 V in the blind 1 s run. `U6 TEST 3300 19` reaches about 3.28 V.
- OC sets on every real start, with no load and PSU current not limiting (peak not captured, under 0.25 A). Cleared at idle before each run (`78=0x48`), then `0x58` again after about 100 ms or after the 1 s blind run.
- With OC pre-cleared, U6 stops answering I²C at the first read, 100 ms after release. The same happens with an in-run CLEAR_FAULTS at 30 ms and at 100 ms. Reads recovered once the converter was re-inhibited. The cause is unknown: U6 reset or brownout, switching noise on the bus, or the bit-bang I²C lacking clock-stretch and bus recovery.
- Input starvation is ruled out. Noise on the measurement is unlikely.

Next steps (no converter runs until the first two are done)
1. Check the ISNSP/ISNSN sense resistor: fitted value and soldering against the schematic (the datasheet assumes 10 mΩ). Scope the ISNSP–ISNSN pair during a blind 1 s start.
2. List everything on the output rail (TVS, zener, bleeder, output FET) for a clamp near 5 V.
3. Capture the PSU peak current on the next run (peak-hold or a series meter).
4. Scope BIAS and VCC1 at startup to check whether U6 resets when the converter starts.
5. Optional: add I²C bus recovery and clock-stretch handling to the bit-bang driver. Log nFLT (PC13) by GPIO during START to avoid polling the bus.
6. Re-score or review the Q7 clamp resistance against the DEC-026 `≤0.2 V` check.

Stop rules in force
- START window 1000 ms, then PB12 re-inhibit. PSU abort at 0.3 A. Vout abort at target + 1 V. No PV, no battery, no loads.
- State at end of session: PB12 high, converter held off, `78=0x58`.

End of handoff.
