# ST-Link clone to Cortex SWD adapter - A1

Original **32 x 24 mm, two-layer prototype** for the clone pinout in Phil's
product photo. This is a separate accessory; it does not change controller
J9 or any solar-controller CAD.

The previously researched `noahp/stlink-clone-adapter` project was reference
research only. Its CAD, routing, artwork and third-party assets are not copied
into this design or the order package. This design uses the observed signal
mapping, original schematic/PCB geometry, and published connector dimensions.

**Prototype fabrication files, not a bench-qualified accessory.** Confirm the
actual clone pinout, connector orientation and housing fit before connection.
No PCB or components have been ordered by this tooling.

## Saved prototype status - 2026-11-20

The user reports ordering the adapter PCB. The supplier, uploaded file,
quantity and order confirmation have not been independently recorded.
Physical fit, cable continuity and bench operation remain unverified;
issue #17 stays open.

The committed PCB preserves the user's subsequent KiCad edits: rotated and
relocated J1/J2, rerouted copper, a rounded outline approximately
31.93 x 23.85 mm, four vias, and an optional local J1 STEP-model reference.
The STEP model itself is not included or needed for fabrication.

KiCad 10.0.6 checks on this saved layout report **zero ERC findings, zero
unconnected items and zero schematic/PCB parity findings**, but **three DRC
track-width violations**: three SWCLK segments are 0.15 mm wide against the
configured 0.20 mm minimum. Independent copper checks confirm the intended
signal mapping, isolated probe power outputs/VTref and open reset link.
The width violations remain outstanding; this snapshot is not DRC-clean.

The earlier generated fabrication ZIP and attached order package describe
the original rectangular layout, not this edited layout. The earlier Gerber
ZIP SHA-256 is
`b8cfb77ba3782303a6683d0b4219555f69d448b7bfb49cd07f1b22f03e529070`.
Do not assume it is the file actually submitted to the supplier.
Retain the actual uploaded ZIP with the order confirmation.

**Do not run `generate.py` over the saved layout to export it.** That script
recreates the original rectangular design, overwriting the user's CAD edits.
Use it only in a separate copy when reproducing the original baseline.
Before another fabrication release, resolve the width violations and export
fresh Gerbers/drills from the reviewed saved PCB in KiCad.

## What to order

Run `generate.py` with KiCad 10's bundled Python to create the `build` folder.
The settings below describe that original generated baseline, not the edited
snapshot above. For example, from a separate copy of this directory in PowerShell:

```powershell
& 'C:\Users\user\AppData\Local\Programs\KiCad\10.0\bin\python.exe' '.\generate.py'
```

Upload **`build\STLink-Clone-SWD-A1-gerbers.zip`**, not the entire source or
order-package archive, to the PCB manufacturer's Gerber uploader.

| Fabrication setting | Prototype selection |
|---|---|
| Board type | Bare PCB; assemble connectors by hand |
| Quantity | Suggested first order: 5 |
| Dimensions | 32 x 24 mm, one rectangular board |
| Layers | 2 |
| Material / thickness | FR-4, 1.6 mm |
| Copper | 1 oz |
| Finish | Lead-free HASL; ENIG is also suitable |
| Mask / legend | Both sides; green mask / white legend is fine |
| Smallest track / clearance | 0.25 mm / 0.20 mm design minimum |
| Smallest finished hole | 0.30 mm plated via |
| Connector holes | Ten 1.02 mm finished plated holes |
| Special processing | None; no impedance control, castellations or slots |
| Stencil / assembly | Not required for the proposed hand-assembled order |

Review the manufacturer's rendered layers before paying: check two copper
layers, mask openings, legible pin-1 markers, a single 32 x 24 mm outline and
12 plated holes. There are ten connector holes and two vias, with no NPTH
holes. Do not select an SMT assembly service without supplying and reviewing
its separate assembly requirements.

### Purchased parts, per assembled adapter

| Item | Qty | Part / requirement |
|---|---:|---|
| J1 | 1 | Samtec **SSW-105-02-G-D**, female 2x5 2.54 mm through-hole socket |
| J2 | 1 | Samtec **FTSH-105-01-L-DV-007-K**, keyed 2x5 1.27 mm SMT male header, pin 7 omitted |
| JP1 | 0 | PCB solder-link pads only; leave open |
| Cable | 1 | Samtec **FFSD-05-D-04.00-01-N** candidate: dual-ended 1.27 mm ribbon, nominal 4 inches |

The cable's official product page lists FTSH as a mating family, and the FTSH
print specifies `-K` for FFSD mating. This is manufacturer-document evidence,
not a physical cable check. Use the normal wiring version, not `-RW`/reversed
variants; check pin-1-to-pin-1 continuity and key alignment before use.

J2 intentionally uses the same candidate as the controller's standard J9, so
the ribbon has female sockets at both ends. J1 is a **female socket**, not
another male pin header. Fit it on the top/component side. Its socket pin 1
is the square PCB pad marked `1`; align that cavity with the clone's actual
pin 1. Its mating-face view is mirrored relative to its PCB solder view.
Do not force a socket into an incompatible clone shroud. Check shroud opening,
male post dimensions, insertion depth, and clearance to the adapter board.
An unkeyed J1 can be reversed; its pin-1 marker must be followed.

The SSW tail drawing specifies approximately 0.79 x 0.41 mm flat tails;
1.02 mm finished holes and 1.70 mm annular pads are the selected prototype
land pattern. J1 uses 2.54 mm position and row pitch, a 13.21 x 4.95 mm
nominal body, and a 4.93 mm tail. Trim soldered tails if needed for clearance.
J2 uses the official 1.27 mm pitch / 3.43 mm row-spacing SMT land pattern
with 0.74 x 2.79 mm lands, a 6.35 x 6.86 mm outline, and pad 7 omitted.

## Electrical mapping

This is the **type-B/Variant-2** clone mapping shown on the supplied photo.
It is not a universal adapter for every stick marked ST-LINK/V2.

| Clone J1 | Signal | Cortex J2 / controller J9 |
|---:|---|---|
| 1 | Clone RST | Pin 10 only if JP1 is bridged |
| 2 | SWCLK | 4 |
| 3 | SWIM | Isolated |
| 4 | SWDIO | 2 |
| 5, 6 | Ground | 3, 5, 9 |
| 7, 8 | Probe 3.3 V outputs | Isolated |
| 9, 10 | Probe 5 V outputs | Isolated |
| None | Target VTref | J2.1 is isolated |
| None | SWO / TDI | J2.6 / J2.8 are isolated |
| None | Key | J2 pin 7 physically omitted |

**Power the controller normally.** There is no probe-to-target power path.
This adapter is specifically for the fixed-level clone in the photo, which
shows no VTref input. It is not a universal replacement for an adapter used
with voltage-sensing professional probes. Do not use it to program targets
with incompatible logic voltage.

**Leave JP1 open initially.** Some clones expose an STM8/SWIM reset signal
rather than STM32 NRST. Only bridge JP1 after verifying the actual probe's
STM32 reset support. With JP1 open, use normal SWD attach, not an expectation
of connect-under-reset recovery. The target's own reset circuit remains intact.

## Assembly and first-use checks

1. Inspect bare boards and verify that the power pads listed as isolated do
   not have continuity to ground or any target-side contact. Verify JP1 is open.
2. Solder J1 and J2 on the marked component side, following each pin-1 marker.
   Do not populate pin 7 on J2 or bridge JP1 during routine assembly.
3. Before plugging into the target, verify J1.2 to J2.4, J1.4 to J2.2, and
   J1.5/6 to J2.3/5/9. Verify J1.1 does not reach J2.10 with JP1 open.
4. With the actual probe unpowered, confirm its pinout rather than trusting
   casing text alone; some clone batches are mislabeled. Ground-pair
   continuity alone is a useful check, not full pinout verification.
5. Verify the cable numbering/keying and physical fit, then attach to a
   normally powered 3.3 V target and try a conservative SWD clock such as
   100 kHz. Bench attach/programming remains to be demonstrated.

Do not treat the solar controller as bench-ready simply because this adapter
passes CAD checks. Its separate board-completion and powered-test gates remain.

## Original generated baseline validation

The generator runs KiCad 10 netlist export, ERC, zone-refilled DRC and
schematic parity with violation exit codes enabled. It separately verifies
the exact schematic pin groups and actual copper connectivity, including
isolation of every probe power output, VTref and the open reset link.

The original rectangular A1 outputs were generated with **KiCad 10.0.6**.
These results do not describe the subsequently edited, committed PCB; see
the saved prototype status above.

| Check | Result |
|---|---|
| ERC | 0 findings |
| DRC | 0 findings |
| Unconnected items | 0 |
| Schematic/PCB parity | 0 findings |
| Active copper mapping | Matches the table above |
| Probe power / target VTref / open reset | Copper-isolated as specified |
| Outline / layers / thickness | Closed 32 x 24 mm / 2 / 1.6 mm |
| Drill export | Ten 1.02 mm holes and two 0.30 mm vias |

Reports, a BOM, schematic PDF, top/bottom SVG previews and source/Gerber
hashes are in `build`. The order-package ZIP includes these and editable
source; the smaller Gerber ZIP is the file to upload for bare PCB fabrication.
Do not upload an old ZIP after changing the CAD: export and check fresh files
from the intended layout, without overwriting it with the baseline generator.

No enclosure fit, clone housing fit, actual cable continuity, or bench
attach/program validation has been performed. Order only a small prototype
batch until those physical checks pass.

## Manufacturer references

- [SSW-105-02-G-D product](https://www.samtec.com/products/ssw-105-02-g-d)
- [SSW through-hole print](https://suddendocs.samtec.com/prints/ssw-1xx-xx-xxx-x-xx-xxx-xx-mkt.pdf)
- [FTSH-105-01-L-DV-007-K product](https://www.samtec.com/products/ftsh-105-01-l-dv-007-k)
- [FTSH recommended land pattern](https://suddendocs.samtec.com/prints/ftsh-1xx-xx-xxx-dv-xxx-footprint.pdf)
- [FTSH print and FFSD key option](https://suddendocs.samtec.com/prints/ftsh-1xx-xx-xxx-dv-xxx-xxx-x-xx-mkt.pdf)
- [FFSD-05-D-04.00-01-N cable candidate](https://www.samtec.com/products/ffsd-05-d-04.00-01-n)
- [FFSD cable print / wiring options](https://suddendocs.samtec.com/prints/ffsd-xx-x-xx.xx-01-x-x-xxx-mkt.pdf)
