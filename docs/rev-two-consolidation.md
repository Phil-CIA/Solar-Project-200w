# Rev2 consolidation and design gates

Checkpoint: 2026-11-09. This records repository integration, not new hardware validation.

## Merged baseline and Rev2 controller decision

[PR #25](https://github.com/Phil-CIA/Solar-Project-200w/pull/25) is merged into GitHub `main` at **`32243b5789a5f2400ddd37af67e152e77bece215`**. The published tag **`rev2-development-baseline-2026-11-09`** marks that reconciled source. The Rev2 session fetched origin and fast-forwarded its own branch to that baseline before editing; the separate local-main checkout was not accessed or changed.

Phil explicitly selected **retain LM51772 for Rev2** ([DEC-067](decisions-log.md), [#20 comment](https://github.com/Phil-CIA/Solar-Project-200w/issues/20#issuecomment-6737135332)). This is a user-directed architecture choice, not a completed evidence-based comparison with BQ25756. #20 stays open for its remaining acceptance criteria; no BQ25756 evaluation or purchase is requested.

The distinct `Solar Project Rev2.kicad_pro`, `.kicad_sch` and `.kicad_pcb` set now lives beside Rev1 in `hardware\kicad\solar-project`. It copies the repository Rev1 development set at that commit, with only identity/export metadata changes; it is **not an implemented or qualified Rev2 and is not for fabrication**. Shared libraries, historical sources, manufacturing references and fabrication outputs are untouched. No circuit values, routing or temporary bench modifications were imported. See the [revision index](../hardware/kicad/solar-project/REVISION-INDEX.md) for exact identities and the paired KiCad 10.0.6 structural checks: identical electrical netlists and inherited ERC/DRC findings, with zero unconnected or schematic-parity items in both sets.

## Branch disposition

The integration branch is `phil-cia-rev2-branch-consolidation`, starting from remote-main commit `11aa4dc`. Local backup tags under `backup/rev2-consolidation-2026-11-09/` preserve the inventoried source tips. No source branch or session has been deleted or archived.

| Source | Disposition |
|---|---|
| `phil-cia-lm51772-12v8-bringup` at `b62498e` | Integrated as the recent firmware/evidence baseline. |
| Local `main` at `61886c3`, including `923e8fb` | Merged with explicit tree reconciliation in `9dca4ef`; the separate main checkout was not modified. |
| `origin/copilot/troubleshoot-lm51772-communication` at `12644b1` | Four unique commits merged in `e77150d`; historical handoff and migration checklist carry explicit scope warnings. |
| `origin/phil-cia-converter-voltage-research` at `7a95879` | Research handoff merged in `538e5b6`; its earlier voltage-ceiling hypothesis is marked superseded by later tests. |
| Bring-up preservation commit `df3660d` | Earlier uncommitted one-second Q5/Q6 source saved by its owning session, merged into ancestry without replacing the later 10-second default, and backed up with the `q5q6-1s` tag. |
| Handoff `ad33498`, safe firmware `aa842b3`, bench prep `24f7b8e`, substitutions `bb561d6`, latest-handoff `1d474d3`, development-issues `7ebaf85` | Already ancestors of remote main at inventory time; committed history is retained. |

Owning sessions reported clean Git worktrees for handoff, safe firmware, and bench prep. The bring-up session's intentional source variant is now committed as `df3660d`; its generated `build-q5q6` images remain untouched and untracked in that worktree. Unsaved external editor buffers cannot be certified from Git status. Do not delete that worktree or its generated images as cleanup.

## Firmware reconciliation

- STM32: preserve BOOST/PCM commands and the 10-second start/status window; take local-main `BENCH_JUMPER_FITTED=0`, build ID `u6q5q6-10s`, and fault-latched shutdown on an invalid I2C-speed override. The earlier bring-up worktree's 1-second Q5/Q6 variant is historical, not the consolidated default.
- ESP32: retain `b62498e` protected setup AP, explicit component dependencies, and persistent reconnect/fallback event loop. The open-AP implementation from `61886c3` stays in history, not in a second competing active firmware folder. Its web credential-clear endpoint is not retained; reconfiguration/erase behavior and incompatible NVS namespaces are documented in the [application README](../firmware/esp32_c3_wifi/README.md).
- Documentation: preserve both Q5/Q6 accounts with provenance rather than erasing the unresolved D5 polarity discrepancy or presenting merged notes as a new verified bench test.

## Rev2 change register

The proposed [Rev2 change plan](rev-two-change-plan.md) separates fixes, additions, open scope decisions and implementation order. It does not approve optional features or apply electrical changes.

| Work | Existing tracker / gate |
|---|---|
| Controller selection recorded; comparison still outstanding | Phil selected LM51772 for Rev2 on 2026-11-09. [#20](https://github.com/Phil-CIA/Solar-Project-200w/issues/20) remains open: its evidence-based comparison has not been performed. No BQ25756 evaluation or purchase requested. |
| Correct and qualify peak-current sensing | [#19](https://github.com/Phil-CIA/Solar-Project-200w/issues/19); diagnostic sense bypasses are not production protection. |
| Match feedback wiring to firmware | [#21](https://github.com/Phil-CIA/Solar-Project-200w/issues/21); internal feedback bench jumper is not an existing Rev1 feature. |
| Actual measured voltage/current, not target readback | [#15](https://github.com/Phil-CIA/Solar-Project-200w/issues/15). |
| Control-supply D4/D6 connections | [#22](https://github.com/Phil-CIA/Solar-Project-200w/issues/22). |
| C24 and compensation | [#23](https://github.com/Phil-CIA/Solar-Project-200w/issues/23), [#10](https://github.com/Phil-CIA/Solar-Project-200w/issues/10); 22 nF is the documented design value pending fitted-part/loop verification. |
| BOOT0, control-rail TVS, debug connector | [#14](https://github.com/Phil-CIA/Solar-Project-200w/issues/14), [#16](https://github.com/Phil-CIA/Solar-Project-200w/issues/16), [#17](https://github.com/Phil-CIA/Solar-Project-200w/issues/17). |
| ESP32/STM32 link and firmware delivery | [#11](https://github.com/Phil-CIA/Solar-Project-200w/issues/11), [#24](https://github.com/Phil-CIA/Solar-Project-200w/issues/24); retain STM32 charger authority. |
| Output protection and D5 schematic polarity | [#9](https://github.com/Phil-CIA/Solar-Project-200w/issues/9); preserve Phil's bench disposition without claiming undocumented qualification. |
| Battery/BMS boundary and deferred interfaces | [#4](https://github.com/Phil-CIA/Solar-Project-200w/issues/4), [#12](https://github.com/Phil-CIA/Solar-Project-200w/issues/12), DEC-062. |

Also carry the historical ADDR/SLOPE, MODE/ISET separation, grounding, RT and configuration-strap lessons into review of the chosen design. They must be checked against current source and datasheets, not copied from the September netlist. Temporary ISET-to-VCC2 wiring, 22 Ohm gate resistors, and C26 = 690 nF remain bench experiments, not approved Rev2 selections.

## Landing and Rev2 gates

Both firmware builds completed during consolidation: the four Rev1 STM32 stages using the installed GNU Arm toolchain, and the ESP32-C3 application using the installed ESP-IDF 5.5 environment with its matching RISC-V compiler. STM32 linking still reports syscall-stub and RWX-segment warnings; build success is not a hardware safety/qualification result. No flashing or powered tests were performed.

1. Retain the published backup refs and the original bench images. All inventoried unique committed source histories are incorporated; this is not permission to delete worktrees.
2. Completed: consolidation landed through [PR #25](https://github.com/Phil-CIA/Solar-Project-200w/pull/25) at `32243b5`. The builds above cover the selected tree. No flashing or powered work was performed.
3. Reconcile the separate local-main checkout in its owning context after landing, without resetting local work. That checkout was not inspected or modified by this consolidation session; its tracked files, untracked files, and editor buffers need their own preservation check before synchronization.
4. Completed: recorded the merged baseline and copied the distinct Rev2 KiCad set described above, preserving Rev1 sources and fab outputs. This does not implement any unresolved design changes.
5. LM51772 retention is selected; next implement the approved Rev2 changes with schematic and PCB edits kept together. Current-sense #19, feedback #21, measured telemetry #15 and all other unresolved register entries remain open. Retire old branches/sessions only after all work is accounted for and the user approves cleanup; none were deleted or archived here.
