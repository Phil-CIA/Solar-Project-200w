# STM32 Support Implementation Checklist (Rev 0)

Last updated: 2026-07-21
Owners: Phil + Copilot
Purpose: implement the minimum reliable STM32 support network in schematic before board placement freeze.

## 1. Scope

This checklist covers only MCU support hardware around U1:

1. Reset network and reset button.
2. External high-speed crystal network.
3. SWD/bring-up header verification.
4. BOOT behavior note.
5. Navigation button pad integration sanity checks.

## 2. Required Nets

Use these net names exactly:

- MCU_RST_N
- MCU_HSE_IN
- MCU_HSE_OUT
- BTN_UP_N
- BTN_DN_N
- BTN_LT_N
- BTN_RT_N
- BTN_ENT_N
- CTRL_3V3
- PWR_NEG

## 3. Components To Add Or Verify

## 3.1 Reset Path

- SW_RST: momentary push button from MCU_RST_N to PWR_NEG.
- R_RST: 10k pull-up from MCU_RST_N to CTRL_3V3.
- C_RST: 100 nF from MCU_RST_N to PWR_NEG (optional but recommended for cleaner POR/manual reset).

Pin landing:

- U1 pin PG10/~RST -> MCU_RST_N

## 3.2 HSE Crystal Path

- Y1: 8 MHz or 16 MHz crystal (select one and keep firmware clock tree aligned later).
- C_HSE1: load capacitor from MCU_HSE_IN to PWR_NEG.
- C_HSE2: load capacitor from MCU_HSE_OUT to PWR_NEG.
- Optional R_HSE_SER: 0R to 33R series damping placeholder on MCU_HSE_OUT if needed.

Starting values:

- C_HSE1/C_HSE2: 18 pF to 22 pF, C0G/NP0, then tune against selected crystal CL spec and estimated stray C.

Pin landing:

- U1 PF0-OSC_IN -> MCU_HSE_IN
- U1 PF1-OSC_OUT -> MCU_HSE_OUT

## 3.3 Bring-Up Header

Verify accessible pins on debug/programming header:

- SWDIO (PA13)
- SWCLK (PA14)
- MCU_RST_N
- CTRL_3V3 (target reference)
- PWR_NEG

## 3.4 BOOT Behavior

- Confirm PB8-BOOT0 default state and document expected boot mode.
- If recovery flexibility is desired, add a jumper or test pad strategy for BOOT control.

## 3.5 Navigation Pad

Already present in current schematic work:

- SW1-SW5 and pull-up resistors R54-R58.
- BTN_UP_N / BTN_DN_N / BTN_LT_N / BTN_RT_N / BTN_ENT_N nets.

Verify each BTN net is active-low at MCU input and has deterministic idle-high behavior.

## 4. Placement And Routing Rules (Board Side)

1. Place Y1 and C_HSE1/C_HSE2 as close as physically possible to PF0/PF1 pins.
2. Keep crystal loop short, symmetric, and away from switch-node/high-dVdt copper.
3. Give C_HSE1/C_HSE2 short returns to quiet ground (PWR_NEG near MCU ground region).
4. Place SW_RST near user access edge but keep MCU_RST_N trace short and clean.
5. Keep BTN traces away from power switch loop and add series placeholders later only if EMI/noise testing needs them.

## 5. Exit Criteria

- MCU reset button path exists and is net-labeled MCU_RST_N.
- HSE crystal and both load capacitors are wired on MCU_HSE_IN/OUT.
- SWD plus reset access is available on a physical header or test points.
- BOOT default behavior is documented.
- Button nets and MCU landings are confirmed in netlist.
