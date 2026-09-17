# Solar Project 200W - Handoff

Latest active handoff: [SOLAR_HANDOFF_2026-09-17.md](SOLAR_HANDOFF_2026-09-17.md)

## Current status

- Rev 0 is on the bench; Rev 1 schematic/BOM work is underway. No physical Rev 1 board exists yet (Gate 4 PCB layout has not started).
- [docs/rev-one-closure-plan-2026-09-16.md](docs/rev-one-closure-plan-2026-09-16.md) is the living tracked checklist for closing Gate 1 component derating, BOM/sourcing, safety-scope decisions, Rev 0 evidence mining, and Gate 4 PCB entry. Update it in place; do not create parallel plan docs.
- Gate 1 (component derating) has NOT actually exited even though Gate 2/3 schematic work proceeded — several components are still `OPEN`/`HOLD`. Do not start Gate 4 PCB floorplanning until Track A of the closure plan closes or gets written waivers.
- **The 2026-09-17 U6 VIN/BIAS and D2 overvoltage crisis is fully RESOLVED** (DEC-051 through DEC-057): the PV array is now wired 2P (parallel), not 2S (series) — see `hardware/mppt/envelope-decision-worksheet.md` Section 2.1, now the active baseline. Remaining open items are ordinary BOM-sourcing gaps (C7, C52, D2 replacement exact catalog numbers) and a new, not-yet-implemented firmware requirement (active parasitic-load Voc control for F1's cold-Voc margin, DEC-054) — not open safety questions.
- F1 stays on the Keystone `3522-2` blade-fuse clip (DEC-054, reversed an earlier DEC-053 fuse-family swap decision) — do not re-propose changing it without new information.
- The U14/Q5/Q6 gate test is confirmed BLOCKED on hardware: Rev 0 has none of the relevant parts (LM74502/U14/BSC040N10NS5), and no physical Rev 1 board exists yet either.
- **Track C (safety/sensing gaps) is now CLOSED** (DEC-061/062): a dedicated battery-pack temperature input was added and implemented as a new connector `J8` tapping the existing I2C bus (not a new ADC/NTC channel - every MCU GPIO was already in use); the BMS communication link is formally deferred to Rev 2. Remote I2C sensor part selection for `J8` is still open (Track B/BOM item).

## Locked Baselines

- Retain one LM51772 four-switch conversion stage and rerate it for Rev 1.
- LM74502H plus back-to-back N-MOS (Q5/Q6, `BSC040N10NS5`) input/output protection remains retained.
- Protected battery power is the normal runtime control source and startup prerequisite.
- STM32 retains charger supervision, safety policy, and telemetry ownership.
- USB service power must not independently authorize power-stage switching.
- ESP32 module power is `CTRL_5V` into its `5V` pin; its exposed `3.3V` pin is unused.
- ESP32 owns networking, provisioning, telemetry transport, and its own OTA only.
- PV array wiring is 2P (parallel), not 2S (series) — DEC-051, supersedes the original DEC-004 baseline.
- Battery-pack temperature sensing is a remote I2C module via connector `J8`, not an NTC/ADC channel — DEC-061 (every MCU GPIO is already in use except `PB8`/`BOOT0`).
- BMS communication link is explicitly deferred to Rev 2 — DEC-062; Rev 1 relies solely on the Q-001 `>=20A` current-capability assumption.

## Next session objective

Track A (component derating) and Track C (safety-scope decisions) are both effectively closed. Work down the closure-plan priority list: Track B BOM/sourcing sweep (resolve remaining non-matched BOM lines, pick the `J8` remote I2C temperature-sensor part/address, confirm U1 sourcing), then either C18's ripple-current rating or Track E Gate 4 PCB layout kickoff (Update PCB From Schematic, power-loop floorplan, DRC).

Full detail: [SOLAR_HANDOFF_2026-09-17.md](SOLAR_HANDOFF_2026-09-17.md)

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
- [hardware/kicad/solar-project/ERC.rpt](hardware/kicad/solar-project/ERC.rpt)
- [docs/test/rev0-bringup-addendum.md](docs/test/rev0-bringup-addendum.md)
- [docs/test/bringup-checklist.md](docs/test/bringup-checklist.md)
- [hardware/mppt/kicad-implementation-checklist.md](hardware/mppt/kicad-implementation-checklist.md)
- [hardware/mppt/schematic-notes.md](hardware/mppt/schematic-notes.md)
- [docs/rev-one-delta-plan-2026-08-04.md](docs/rev-one-delta-plan-2026-08-04.md)
- [docs/test/rev-one-erc-drc-closure-checklist.md](docs/test/rev-one-erc-drc-closure-checklist.md)
- [docs/wifi-coprocessor-architecture.md](docs/wifi-coprocessor-architecture.md)
- [SOLAR_HANDOFF_2026-09-17.md](SOLAR_HANDOFF_2026-09-17.md)
- [SOLAR_HANDOFF_2026-09-12.md](SOLAR_HANDOFF_2026-09-12.md)
- [SOLAR_HANDOFF_2026-09-09.md](SOLAR_HANDOFF_2026-09-09.md)

## Note

Historical session details remain in dated SOLAR_HANDOFF files and project notes; this file stays intentionally short to reduce context load for the next chat.
