# Solar Project 200W - Wi-Fi Architecture Handoff (2026-08-08)

## Session Outcome

Rev 1 now includes an ESP32-C3 development module for Wi-Fi. The control-power architecture and processor ownership boundary were reviewed and accepted for this pass.

## Accepted Architecture

- Power the ESP32 development module from `CTRL_5V` through its `5V` pin.
- Leave the module's exposed `3.3V` pin unused; its onboard regulator powers the ESP32.
- U5 generates the separate `CTRL_3V3` rail for the STM32 and other board circuitry.
- Treat the ESP32 as a network coprocessor only.
- Keep the STM32 as the sole owner of MPPT, PWM, protection, limits, faults, and safe charger operation.
- Connect the processors with UART using `TXD_ESP` and `RXD_ESP`; retain `RTS_ESP` and `CTS_ESP` as optional flow control.
- Use versioned COBS framing with sequence, length, and CRC16 fields.
- Provision Wi-Fi through a temporary setup access point and store credentials in ESP32 NVS.
- Start remote telemetry with MQTT; require authenticated encrypted transport before remote control.
- Allow ESP32 OTA initially. Keep STM32 updates on SWD until the interprocessor protocol is proven.

Full architecture: [docs/wifi-coprocessor-architecture.md](docs/wifi-coprocessor-architecture.md)

## Schematic Review Result

The intended arrangement is coherent for a 5 V-powered development module:

- U2 is the ESP32-C3 development module.
- U4 produces `CTRL_5V`.
- U5 produces `CTRL_3V3` independently.
- D10 provides the `VBUS` to `CTRL_5V` bench-power path.
- UART and optional RTS/CTS signals are present.

No schematic edit was made during this review.

## Required Verification Before Freeze

1. Regenerate ERC after the latest Rev 1 schematic edits. The checked-in `ERC.rpt` reports 0 errors and 0 warnings but is dated 2026-08-04 and is not fresh evidence for the current schematic.
2. Confirm D10 polarity implements only `VBUS -> CTRL_5V` and prevents reverse feed into USB.
3. Calculate USB bench-power load for the complete `CTRL_5V` domain, including ESP32 Wi-Fi current peaks.
4. Verify the ESP32 module antenna keepout in placement, copper, and enclosure planning.
5. Decide whether host-controlled ESP32 reset/boot pins are required before routing freeze.
6. Close the existing C52 voltage-rating margin item.

## Next Session Objective

Freeze the STM32-to-ESP32 interface and complete the hardware verification gates before PCB routing:

1. Export fresh ERC evidence from `Solar Project Rev1.kicad_sch`.
2. Verify D10 direction and the `CTRL_5V` current budget.
3. Map STM32 UART pins to ESP32 GPIOs and confirm TX/RX crossing.
4. Decide RTS/CTS and reset/boot-control requirements.
5. Define the initial packet fields and MQTT telemetry schema.
6. Add antenna keepout and power-integrity checks to the PCB checklist.

## Working-Tree Caution

The repository already had uncommitted changes at handoff time, including the Rev 1 schematic and earlier handoff files. Preserve them and avoid destructive cleanup. KiCad lock, autosave, and cache files remain out of commits.

## Primary Files

- [hardware/kicad/solar-project/Solar Project Rev1.kicad_sch](hardware/kicad/solar-project/Solar%20Project%20Rev1.kicad_sch)
- [hardware/kicad/solar-project/ERC.rpt](hardware/kicad/solar-project/ERC.rpt)
- [docs/wifi-coprocessor-architecture.md](docs/wifi-coprocessor-architecture.md)
- [docs/decisions-log.md](docs/decisions-log.md)
- [docs/rev-one-delta-plan-2026-08-04.md](docs/rev-one-delta-plan-2026-08-04.md)
- [SOLAR_HANDOFF_2026-08-04.md](SOLAR_HANDOFF_2026-08-04.md)