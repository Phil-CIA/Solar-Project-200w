# TI E2E support request draft: LM51772 output collapses above ~9 V Vin (2026-11-03)

Draft for posting on TI E2E (Power Management). Items in [brackets] must be filled in by Phil before sending. Attach: schematic PDF, PCB layout screenshots/PDF, scope shots, the TI Quickstart workbook, and `docs/test/rev1-bridge-bench-findings-2026-10-31.md` if desired.

## Title

LM51772 4-switch buck-boost: output regulates at Vin <= 9 V but droops/faults at higher Vin; I2C reads fail while switching

## Summary

Custom board with LM51772 (QFN, I2C address 0x6A) for charging a Li battery from a PV input: 18-30 V in, 11.8-14 V out (up to 5 V / 1 A for bench bring-up). The converter regulates correctly at low Vin but loses regulation as Vin rises, and I2C reads fail during switching at higher Vin. We have eliminated many causes and would like your help finding what we are missing.

## Configuration on the bench

- Internal feedback divider selected through I2C: FB (pin 14) is jumpered to VCC2 (pin 29) with a wire (bench rework, not in the PCB design), `D8` = 0x84 (DIV20), Vout set by registers `0x0C/0x0D` (code x 20 mV; 5000 mV = 0x00FA).
- Original external divider R3 76.8 k / R25 5.62 k; R3 lifted for these tests.
- `V_in_FB` (pin 15) still connected to a Vin divider R18 76.8 k / R17 5.62 k (the design was for an external-divider + PCM). The datasheet says to tie it to AGND when using the internal divider; we have not changed this yet.
- `D3` = 0x20 (IVP disabled), `D7` = 0x2F (slope comp max), `D9` = 0x20 in the cases where it was read (PCM window = 0), `DA` = 0xFF.
- Current sense: R22 = 10 mohm (the BOM/calculator value is 1 mohm), on the SW1 side in series with the inductor; CSA/CSB go through R26/R27 with C20 = 180 pF + 180 pF in parallel across CSA/CSB.
- Compensation: C24 = 22 nF [add Rcomp/other values from schematic]. ILIM was disabled temporarily during testing; [confirm restored].
- Power stage: [FET part numbers], [inductor value and part], [Cin/Cout], gate drive from the LM51772. VIN/BIAS are fed from the PV input, so VCC2 comes from the internal LDO.
- Load for the tests: [load resistor value, or none]. Supply: [bench PSU, current limit].

## Symptoms

1. With the external divider: regulated 5.10 V at Vin up to 9.0 V, failed at 9.25 V and above.
2. With internal FB and `D7` = 0x2B: output capped near 4.7 V at 8 V; `SW_1` showed alternating wide/narrow pulses (subharmonic oscillation).
3. With `D7` = 0x2F, 5000 mV target: Vout = 4.97 V at 7 V, 4.96 V at 8 V and 9.25 V, 4.70 V at 9.5 V, 4.42 V at 10 V; PSU current 77-85 mA. At 10 V the `SW_1` waveform is clean, duty below 50 %, no alternating pulses. Cause of the droop is unknown.
4. Target 10 V out from 14 V in: the register write and readback succeed (0x01F4). The converter switches for about 350 ms at about 600 kHz, about 35 % duty, Vin swings about 1 Vpk without collapsing. Our MCU's first I2C poll at 50 ms fails (reads of 0x78/0x21 fail; after a failure, 7 of 8 register reads failed). Registers are intact afterwards (no reset; `D3/D7/D8/DA/0D/0C` unchanged). Slowing the bit-banged I2C clock to 10 kHz did not help. One read returned `D8` = 0x85 instead of 0x84 with no write. The I2C bus is under 50 mm long.

   We do **not** know the cause of the I2C failure. We have not shown that switching noise corrupts the bus; the I2C failure may only be a symptom of something else, for example the chip entering a shutdown, protection or fault state (or a logic-supply dip) at that moment. We have not captured SDA/SCL or VCC2 during the event, and the status registers we read (0x78/0x21) have not been diagnostic. We are asking what internal state could make the device stop responding or return corrupted data shortly after the start of switching.

## What we have ruled out

- Soft-start/SS stall, start window length, compensation capacitor value, R22 value, ILIM disable, VOUT_A under external FB, and `0x78` status bits (0x18/0x58 appear in both passing and failing cases).
- PCM was disabled (`D9` = 0x20) in the earlier 4 V run.
- Reverse-flow with a load fitted: not supported (output went lower with the load).
- Slope compensation was a real cause of the 8-9.25 V failures; `D7` = 0x2F fixed those but not the droop at 9.5 V and above.

## Questions

1. What could cause Vout to droop in a clean, below-50 % duty buck region as Vin rises above about 9.5 V, with `D7` at maximum? Does slope compensation, which scales with R_CS, behave unexpectedly with a 10 mohm sense resistor and what is the valid range of R_CS?
2. When using the internal Vout divider, what should the VIN-FB (pin 15) pin be connected to? We have a Vin divider of 0.068 there. Can PCM or the VIN-FB path affect regulation in this mode? (An earlier E2E post in Chinese, thread 943530, got no guidance for internal FB with PCM.)
3. What conditions make the LM51772 stop answering I2C, or return wrong data, shortly after switching starts (shutdown, UVLO, internal supply dip, protection latch)? How can we tell which from the registers? For reference, our CSA and CSB traces are about 30-35 mm long on different layers (In1.Cu and B.Cu) and not a tight Kelvin pair; we do not know whether that is related.
4. Is there a recommended way to check VCC2 and the internal logic supply at high Vin? With VIN/BIAS at 14 V, are there dissipation limits on the VCC2 LDO we should check?
5. Anything in the register state we should read (other than STATUS 0x78/0x21) that distinguishes the cause of the droop?

## Data we can provide

Scope captures of `SW_1`, `SW_2`, `CHG_OUT_POS`, `PV_IN_POS`; full register dumps; Quickstart workbook; schematic and layout; UART logs.
