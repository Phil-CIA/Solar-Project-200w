# Solar Project 200W — Handoff (2026-09-19)

Date: 2026-09-19

Summary
- Final procurement actions completed by engineer: passive and silicon parts manually corrected and ordered; Rev 1 PCB has been submitted to fab.
- I applied vetted substitutions from the order guide to the master BOM (five explicit `Cxxxxx` updates) and regenerated the supplier upload CSVs.

Files changed / created in this session
- Updated: docs/rev-one-bom-master.csv (applied 5 substitutions)
- Backup: docs/rev-one-bom-master.bak.csv (pre-change backup)
- Generated: docs/rev-one-bom-lcsc-upload.csv
- Generated: docs/rev-one-bom-digikey.csv
- Created: docs/rev-one-bom-proposed-substitutions.csv
- Created: scripts/extract_order_guide_substitutions.py
- Created: scripts/apply_proposed_substitutions.py

Key outcomes
- Parts: passives/actives corrected and ordered by engineer (manual corrections took precedence when discrepancies were found).
- PCBs: Rev 1 PCB order placed (fab order submitted).
- Procurement CSVs updated from the current `docs/rev-one-bom-master.csv` and are available for audit.

Outstanding / follow-up items
- Capture and store vendor order confirmations and ETAs in `docs/orders/` (create) when received.
- Reconcile deliveries against `docs/rev-one-bom-inventory-reconcile.csv` and update `InInventory`/`OnOrder` fields.
- `R28` remains a special sourcing case historically noted as unresolved — re-check after supplier confirmations if needed.
- Run a full LCSC-description verification pass for substituted PNs once live pages are re-checked (some LCSC pages require rendered JS; manual verification recommended).

Next recommended steps for the receiver
1. Save vendor order confirmations into `docs/orders/` and update the inventory reconcile sheet.  
2. When parts arrive, run `scripts/generate_inventory_reconcile.py` and re-generate procurement reports if substitutions or quantities change.  
3. If you want the substitution decisions reversed or further constrained (manufacturer-only, footprint-only), revert to `docs/rev-one-bom-master.bak.csv` and re-run the remap pipeline.

Contact
- If anything is unclear, open an issue or ping the author in the repo history for clarification.

End of handoff.
