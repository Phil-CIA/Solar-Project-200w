# Solar Project 200W - Current Handoff

Current checkpoint: 2026-11-09. Consolidation is merged; Phil selected LM51772 for Rev2. A separate copied Rev2 starting set now exists, not an implemented or qualified redesign. See the [consolidation record and Rev2 gates](docs/rev-two-consolidation.md).

## Current state

- Rev1 remains the manufactured hardware baseline. Preserve the manufacturing release at `C:\Users\user\OneDrive\JLCPCB files\Solar Charger\Rev 1`; repository development copies differ from that release as documented in the [board notes](docs/rev-one-board-notes.md) and [revision index](hardware/kicad/solar-project/REVISION-INDEX.md).
- Open `hardware\kicad\solar-project\Solar Project Rev2.kicad_pro` for new Rev2 work. Its matching schematic/PCB are copied from the repository Rev1 development set at `32243b5789a5f2400ddd37af67e152e77bece215`, tagged `rev2-development-baseline-2026-11-09`. Only identity/export metadata changed; circuit values, connectivity, routing, UUID links, shared libraries, Rev1 source and fabrication files are preserved. It is not for fabrication.
- [PR #25](https://github.com/Phil-CIA/Solar-Project-200w/pull/25) merged the recent LM51772 bring-up work, local-main Q5/Q6 firmware settings, and unique older troubleshooting/research notes into GitHub `main` at `32243b5`. This session fetched that baseline without accessing or modifying the separate local main checkout.
- Latest converter evidence: U6 reached approximately 12.8 V across the reported 8-30 V input range with PCM disabled and modified peak-current sensing. The normal CSA/CSB path remains unresolved. Shorting or attenuating that signal does not establish a safe production current limit or rated-power operation. See the [November findings](docs/test/rev1-lm51772-12v8-findings-2026-11-06.md) and [PCM-OFF retest](docs/test/rev1-u6-pcm-off-retest-2026-11-06.md).
- Q5/Q6 transferred 12.78 V in the reported 100 Ohm test with the bypass jumper removed. Phil considers that path good for now and has deferred further testing. D5 schematic polarity and quantified default-off, reverse-flow, clamp and full-current evidence remain unresolved; do not silently claim they passed. See [bench section 11](docs/test/rev1-bridge-bench-findings-2026-10-31.md#11-q5q6-timed-output-path-check-and-bench-disposition-2026-11-07).
- Consolidated STM32 bench source uses `BUILD u6q5q6-10s`, a 10-second test/start limit, `BENCH_JUMPER_FITTED=0`, BOOST/PCM commands, and output inhibition on invalid I2C speed. It is not a production charger image. PA5 (`V_in_ON`) is driven high at initialization; do not interpret a status-oriented filename as inert/default-off behavior.
- The [ESP32-C3 application](firmware/esp32_c3_wifi/README.md) provides password-protected local Wi-Fi provisioning. It does not implement STM32 control, telemetry transport, remote commands, or OTA.

## Rev2 next step

Phil explicitly chose **retain LM51772 for Rev2**, recorded in [DEC-067](docs/decisions-log.md) and [#20](https://github.com/Phil-CIA/Solar-Project-200w/issues/20#issuecomment-6737135332). The evidence-based controller comparison requested by #20 has not been performed; that issue remains open. No BQ25756 evaluation or purchase is requested. Resolve peak-current sensing (#19), feedback mode (#21), actual measured telemetry (#15), and the protection/startup requirements before treating Rev2 as complete. Existing Rev-next issues also cover BOOT0, rail protection, debug access, supply connections, C24 verification, and firmware delivery; see the consolidation record rather than duplicating trackers.

The separate Rev2 set is a starting baseline only; none of those outstanding circuit changes is implemented by copying it. Do not overwrite Rev1 source or fabrication files. Bench changes such as FB-to-VCC2 wiring, VIN-FB divider changes, ISET rewiring, 22 Ohm gates, and 690 nF soft start are not automatically approved Rev2 values.

## Retained design boundaries

- STM32 owns charger supervision, safety policy, and measured telemetry. ESP32 owns networking/provisioning and its own OTA; it does not gain charger authority.
- Protected battery power is the normal runtime control source/startup prerequisite. USB service power must not independently authorize switching.
- Rev2 retains the LM51772 conversion stage by Phil's explicit selection. The copied starting set also retains LM74502H with Q5/Q6 output protection; retention does not close its outstanding qualification work.
- PV array wiring is 2P, not 2S (DEC-051). Existing targets are 125 W continuous output, at most 150 W PV operating power, and 8-10 A nominal charging with approximately 12 A hardware protection pending verification, not measured ratings.
- Battery temperature is a remote I2C sensor through J8 (DEC-061); BMS communications were deferred to Rev2 (DEC-062). Revisit the interface deliberately, not by assuming an available ADC pin.
- Preserve the Keystone `3522-2` F1 decision (DEC-054) and unresolved cold-Voc qualification. Firmware does not raise component ratings.

## Bench and history rules

No powered test, flashing, or hardware rework was performed during consolidation. Before future powered work, establish the actual fitted wiring, board/MCU identity, current limiting, and applicable low-energy procedure. The latest findings explicitly flag unvalidated ISET wiring and current-sense bypasses.

September handoffs and the migration checklist are dated historical evidence, not current physical-board instructions. Start here and follow the current evidence links instead of an old `NEXT_CHAT_PROMPT` or the latest-looking historical filename. Keep notes practical and short; reduce paperwork, not electrical precautions.
