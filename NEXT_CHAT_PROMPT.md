Continue Solar Project Rev2 preparation. Read `HANDOFF.md`, then `docs/rev-two-consolidation.md` for the current branch integration state and existing issue links.

Check `git status` and current refs before editing. Preserve intentional local work, the manufactured Rev1 release, and dated bench evidence. Do not follow old September bring-up instructions as current board state.

Finish the consolidation review/merge before creating a distinct Rev2 KiCad design set. Resolve the LM51772-versus-alternative controller decision in issue #20 before detailed converter layout. Current-sense bypasses and temporary bench values are not approved production choices.

Do not flash firmware or perform powered tests as part of repository cleanup. The STM32 bench image is not inert: its initialization enables the input path. For any separately requested bench work, first verify the fitted wiring, MCU, supply limits, and applicable procedure.
