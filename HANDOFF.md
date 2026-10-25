# Solar Project 200W - Handoff

Current checkpoint: 2026-10-25. Start with [Rev 1 board notes](docs/rev-one-board-notes.md) and the [development tracker](https://github.com/Phil-CIA/Solar-Project-200w/issues/1).

## Current status

- Phil reports Rev 1 assembly complete and supplied the manufacturing folder plus front/rear photos. The package PCB and fabrication outputs match the repository copies; the schematic differs only by empty supplier metadata. See the board notes for details.
- This is a hobby project. Phil requested a practical "good enough" standard: short notes and checks needed for the next step, not a professional lab dossier. Optional photos, full part audits, and formal reports are not blanket prerequisites.
- Electrical operation and firmware compatibility have not been established in this session. Historical `OPEN`/`HOLD` entries are not automatically confirmed faults of this board.
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

Continue [#4](https://github.com/Phil-CIA/Solar-Project-200w/issues/4): reconcile the board-specific low-energy bring-up instructions. Phil confirms the board has never been powered and has a generic 30 V / 10 A CC/CV bench supply. Start with unpowered shorts/polarity checks; those supply ratings are not first-power settings. Confirm only the lead/settings and programmer information needed for the next action, not another evidence dossier.

Do not run Rev 0 flash/control scripts on Rev 1 without checking the fitted MCU and pin map. Keep basic shorts/polarity checks, current limiting and default-off control; reduce paperwork, not power-up precautions.

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
- [SOLAR_HANDOFF_2026-09-19.md](SOLAR_HANDOFF_2026-09-19.md)
- [SOLAR_HANDOFF_2026-09-12.md](SOLAR_HANDOFF_2026-09-12.md)
- [SOLAR_HANDOFF_2026-09-09.md](SOLAR_HANDOFF_2026-09-09.md)

## Note

Historical session details remain in dated SOLAR_HANDOFF files and project notes; this file stays intentionally short to reduce context load for the next chat.
