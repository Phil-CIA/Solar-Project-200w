# Solar Project 200W - Current Handoff

Current checkpoint: 2026-10-28. Start with [Rev 1 board notes](docs/rev-one-board-notes.md) and the [development tracker](https://github.com/Phil-CIA/Solar-Project-200w/issues/1).

## Current state

- Only Rev 0 and Rev 1 exist. Rev 1 is the current production revision, not Rev 2. The September [18](SOLAR_HANDOFF_2026-09-18.md) and [19](SOLAR_HANDOFF_2026-09-19.md) handoffs record design closure, fabrication preparation and the PCB order.
- Phil reports Rev 1 assembly complete and supplied the manufacturing folder plus front/rear photos. The package PCB and fabrication outputs match the repository copies; the schematic differs only by empty supplier metadata. See the board notes for details.
- The production release sent to the manufacturer is authoritative for the physical Rev 1 board. Its current recorded location is `C:\Users\user\OneDrive\JLCPCB files\Solar Charger\Rev 1`, containing `Kicad`, `BOM`, and `GERBERS`; board notes retain the earlier machine's location. Repository schematic, project and BOM files are not byte-identical to the release. Preserve the release and reconcile relevant differences before board-specific tests; use repository copies for continued development.
- This is a hobby project. Phil requested a practical "good enough" standard: short notes and checks needed for the next step, not a professional lab dossier. Optional photos, full part audits, and formal reports are not blanket prerequisites.
- Firmware development through `ff89791` is already on `main`, including Rev 1 STM32G473 minimal and staged bring-up targets; see [firmware notes](firmware/README.md). Code availability does not establish fitted-MCU compatibility, safe switching behavior, or successful charging. No physical tests were performed during this documentation reconciliation. Historical `OPEN`/`HOLD` entries are not automatically confirmed faults of this board.
- The supplied manufacturing BOM is a better working assembly reference than the stale procurement master CSV. C52 and D2 have the later design values; C7's grouped supplier entries remain ambiguous. Do not silently rewrite the BOM or infer installed values from it.
- F1/F2 clips appear empty and J8 appears unpopulated in the supplied front photo. Check what matters for the intended operation; no additional photo campaign is required.
- U14 testing is no longer blocked on Rev 1 hardware not existing. It still needs an applicable low-energy procedure before execution.
- Keep the [existing closure plan](docs/rev-one-closure-plan-2026-09-16.md#6-assembled-board-execution-and-issue-1-closure) as the sequencing reference; GitHub issues track the actual work.

## Locked Baselines

- Retain one LM51772 four-switch conversion stage and rerate it for Rev 1.
- LM74502H plus back-to-back N-MOS (Q5/Q6, `BSC040N10NS5`) output-switch/reverse-flow protection remains retained.
- Protected battery power is the normal runtime control source and startup prerequisite.
- STM32 retains charger supervision, safety policy, and telemetry ownership.
- USB service power must not independently authorize power-stage switching.
- ESP32 module power is `CTRL_5V` into its `5V` pin; its exposed `3.3V` pin is unused.
- ESP32 owns networking, provisioning, telemetry transport, and its own OTA only.
- PV array wiring is 2P (parallel), not 2S (series) — DEC-051, supersedes the original DEC-004 baseline.
- Battery-pack temperature sensing is a remote I2C module via connector `J8`, not an NTC/ADC channel — DEC-061 (every MCU GPIO is already in use except `PB8`/`BOOT0`).
- BMS communication link is explicitly deferred to Rev 2 — DEC-062; Rev 1 relies solely on the Q-001 `>=20A` current-capability assumption.
- F1 retains the Keystone `3522-2` clip (DEC-054). Cold-Voc load mitigation remains unverified; do not assume firmware increases component ratings or change the fuse family without new information.
- Design targets remain 125 W continuous output, at most 150 W PV operating power, and 8-10 A nominal charging with approximately 12 A hardware protection pending verification; these are not measured results.

## Next session objective

Continue [#4](https://github.com/Phil-CIA/Solar-Project-200w/issues/4): reconcile the board-specific low-energy bring-up instructions against the production release. The 2026-10-25 checkpoint records Phil's never-powered report and generic 30 V / 10 A CC/CV bench supply; it is not evidence of board state after later development. No later physical-test result is established here. Carry forward that equipment information and confirm only changes that affect the next action. Keep unpowered shorts/polarity checks before energizing; those supply ratings are not first-power settings.

Do not run Rev 0 flash/control scripts on Rev 1 without checking the fitted MCU and pin map. Keep basic shorts/polarity checks, current limiting and default-off control; reduce paperwork, not power-up precautions.

The latest `stage_aht20_display_uart.c` explicitly drives PA5 (`V_in_ON`) high at initialization. Do not treat its "status-only" name as an inert/default-off guarantee or run it on a connected power path without reconciling that behavior against the board and intended test.

Use a short dated issue comment or board-note entry for the setup, relevant measurements, firmware and deviations from the release. KiCad lock files were reported in the release `Kicad` folder during inspection; do not edit or remove them unless KiCad is confirmed closed.

## Primary evidence files

- [docs/rev-one-closure-plan-2026-09-16.md](docs/rev-one-closure-plan-2026-09-16.md)
- [docs/decisions-log.md](docs/decisions-log.md)
- [docs/rev-one-component-derating-review.md](docs/rev-one-component-derating-review.md)
- [docs/rev-one-bom.md](docs/rev-one-bom.md)
- [hardware/mppt/envelope-decision-worksheet.md](hardware/mppt/envelope-decision-worksheet.md)
- [docs/test/rev-one-u14-low-voltage-gate-test.md](docs/test/rev-one-u14-low-voltage-gate-test.md)
- [hardware/kicad/solar-project/REVISION-INDEX.md](hardware/kicad/solar-project/REVISION-INDEX.md)
- [hardware/kicad/solar-project/Solar Project Rev1.kicad_sch](hardware/kicad/solar-project/Solar%20Project%20Rev1.kicad_sch)
- [hardware/kicad/solar-project/Solar Project Rev0.kicad_sch](hardware/kicad/solar-project/Solar%20Project%20Rev0.kicad_sch)
- [docs/test/rev0-bringup-addendum.md](docs/test/rev0-bringup-addendum.md)
- [docs/test/bringup-checklist.md](docs/test/bringup-checklist.md)
- [hardware/mppt/kicad-implementation-checklist.md](hardware/mppt/kicad-implementation-checklist.md)
- [hardware/mppt/schematic-notes.md](hardware/mppt/schematic-notes.md)
- [docs/rev-one-delta-plan-2026-08-04.md](docs/rev-one-delta-plan-2026-08-04.md)
- [docs/test/rev-one-erc-drc-closure-checklist.md](docs/test/rev-one-erc-drc-closure-checklist.md)
- [docs/wifi-coprocessor-architecture.md](docs/wifi-coprocessor-architecture.md)
- [SOLAR_HANDOFF_2026-09-17.md](SOLAR_HANDOFF_2026-09-17.md)
- [SOLAR_HANDOFF_2026-09-18.md](SOLAR_HANDOFF_2026-09-18.md)
- [SOLAR_HANDOFF_2026-09-19.md](SOLAR_HANDOFF_2026-09-19.md)
- [SOLAR_HANDOFF_2026-09-12.md](SOLAR_HANDOFF_2026-09-12.md)
- [SOLAR_HANDOFF_2026-09-09.md](SOLAR_HANDOFF_2026-09-09.md)

## Note

Historical session details remain in dated SOLAR_HANDOFF files and project notes; this file stays intentionally short to reduce context load for the next chat.
