# Rev 1 U6 PCM-OFF Retest (2026-11-06)

## Scope

Retest LM51772 behavior at `VOUT=12.8 V` target after adding `U6 PCM OFF|DEFAULT` and running `U6 PCM OFF` immediately after `ARM`.

## Firmware / command context

- Branch: `phil-cia-lm51772-12v8-bringup`
- Commit under test: `11a1f52`
- New command verified: `U6 PCM OFF|DEFAULT`
- `U6 PCM OFF` response verified: `OK PCM_WINDOW_LOW=0 D9=0x20`
- `U6 BOOST ON` used for all runs below
- Test command form: `U6 TEST 12800 <VinV>`

## Bench results

| Vin setpoint | Measured TP1 / Vout | PSU current | STATUS_BYTE (0x78) | I2C behavior |
|---|---:|---:|---|---|
| 8 V | 12.70 V | 83 mA | `0x08` (UV) | Stable for full run |
| 10 V | 12.78 V | 133 mA | `0x08` (UV) | Stable for full run |
| 12 V | 12.77 V | 123 mA | `0x08` (UV) | Stable for full run |

## Outcome

With `PCM_WINDOW_LOW=0` (`D9=0x20`), the converter reached and held ~12.8 V at 8/10/12 V input in these bench runs, with no I2C dropouts observed and no OC-dominant status pattern during the successful measurements.
