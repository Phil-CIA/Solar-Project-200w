Continue the Solar Project 200W repository from the latest handoff.

## Current priority

Find and select an in-stock replacement for `STM32G431CBU6` for Rev 0 continuity.

## State to assume

- Fuse policy is aligned for first build:
   - F1 schematic value is `Fuse_TD_10A_5x25_63VDC_MIN`
   - Holder rule is locked: 5x25 time-delay class with `>=63 VDC`
- LM51772 migration context remains valid; do not reopen it unless the MCU swap requires interface changes.
- Working tree has active local edits; avoid destructive cleanup.
- Preliminary MCU stock sweep already done from ST eStore:
   - Out of stock: `STM32G431CBU6`, `STM32G441CBU6`, `STM32G474CEU6`
   - In stock (package-compatible vanilla F4 path): `STM32F411CEU6`, `STM32F401CEU6`
   - In stock (LQFP alternates): `STM32G431CBT6`, `STM32G441CBT6`

## What to do first

1. Read `SOLAR_HANDOFF_2026-07-26.md`.
2. Run `git status --short` and confirm baseline.
3. Audit the live U1 net/peripheral usage from schematic/netlist.
4. Produce a shortlist of 2 to 4 in-stock MCU alternatives.

## Selection criteria

1. Satisfy reduced functional needs first (ADC coverage, control GPIO/timer, I2C, UART, SWD).
2. Package/pin compatibility is preferred but not mandatory.
3. Immediate distributor stock.
4. Lowest combined schematic/PCB reroute + firmware migration effort.

## Output expected from next session

1. Preferred MCU replacement + one backup option.
2. Explicit risk summary (hardware changes needed, firmware delta, sourcing confidence).
3. If approved, apply U1 part update plus decision/doc updates in one pass.

## Current recommendation to validate first

1. Preferred: `STM32F411CEU6` (vanilla F4, in stock, UFQFPN-48 package-compatible).
2. Backup: `STM32F401CEU6`.
3. Family-continuity fallback: `STM32G431CBT6` if firmware migration risk dominates.

