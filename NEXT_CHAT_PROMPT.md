Continue the Solar Project 200W repository from the latest handoff and continue Rev One board planning.

## Current priority

Define Rev One circuit-board deltas from the now-validated Rev 0 input-protection baseline while keeping Rev 1 as the active schematic lineage.

## State to assume

- LM74502H input-protection wiring is verified and ERC-clean.
- Back-to-back N-MOS high-side reverse-protection path is retained.
- One deferred item remains: C52 voltage-rating margin review.
- Rev 1 schematic and board copies exist in-tree alongside recovered Rev 0 copies.
- Working tree may contain local edits; avoid destructive cleanup.

## What to do first

1. Read HANDOFF.md and SOLAR_HANDOFF_2026-08-04.md.
2. Run git status --short and capture baseline.
3. Export fresh netlist/ERC/DRC evidence before new edits.
4. Build a Rev One delta table by schematic sheet and note owner/intent for each change.

## Session deliverables

1. Rev One scoped change list (must-have vs optional).
2. Updated schematic notes and wiring-map references for approved deltas.
3. ERC/DRC closure checklist for Rev One release.
4. Next commit plan split into:
   - commit A: schematic/netlist changes
   - commit B: notes/handoff/checklist updates
5. Explicit revision labeling in the active schematic title block and archive copies.

## Constraints

1. Keep autosave and lock artifacts out of commits.
2. Keep Rev One scope disciplined; do not reopen unrelated architecture tracks without explicit decision logging.

