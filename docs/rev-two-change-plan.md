# Rev2 change plan

Planning checkpoint: 2026-11-09; current-sense status updated 2026-11-16; issue #33 interface/recovery status updated 2026-11-17. Proposed work, not authorization to apply every option below. The Rev2 schematic and PCB have since changed; routing is under review and the board is not qualified.

## Copy-only routing, stitching and shunt review

- **Existing vias retained.** Phil confirmed Rev1 used 0.60 mm diameter / 0.30 mm drill vias with 2 oz copper and was accepted. This is useful precedent, not a guarantee of Rev2 acceptance; resolve any order-specific DFM feedback. Do not enlarge or remove vias solely on the basis of the ambiguous annular-ring guidance.
- **2 oz routing trial:** on a copy of the current board, 132 outer-layer tracks below 0.16 mm were widened to 0.18 mm, with local moves of the STM_NRST, RXD and CTRL_3V3 vias and one CTRL_5V corner. A refill and DRC using 0.20 mm minimum clearance and 0.16 mm minimum track width reported no clearance or width findings and zero unconnected items. The ten remaining findings are seven pre-existing physical/library findings plus three trial INA footprint-library mismatches. This is not a full electrical or manufacturing approval.
- **INA241 trial:** U10/U11/U12 use 1.05 x 0.45 mm pads at 0.65 mm pitch and 2.60 mm row-center spacing, with 0.05 mm corner radius, as in TI's DDF land and 0.125 mm stencil examples. Existing track endpoints remain inside the moved pads, and the trial copper, mask and paste exports reflect the trial pad geometry. The 0.20 mm pad gap is exactly JLCPCB's published 2 oz mask-bridge minimum, with no nominal margin. JLCPCB generates its own SMT stencil data; confirm final apertures and stencil thickness with the assembler. Review all three device body/courtyard alignments and local connections before acceptance. The trial still uses the original library ID and produces three mismatch warnings; any approved implementation needs a maintained project-local footprint and matching schematic references. Existing 0.15 mm footprint/pad local clearance overrides were not changed; decide their disposition explicitly rather than silently changing the project rules.
- **Ground stitching:** removing two vias from the five-via PWR_NEG row near (163.10, 138.33) and (164.85, 138.33) mm on a separate copy did not introduce DRC findings or change the measured zone areas/outline counts. A CTRL_5V track on In2 is approximately 2.44 mm from the row center. Neither observation establishes equivalent high-frequency return current or EMI. Keep the row for now and assess other stitching by function; do not treat proximity-based inventory counts as redundant vias or thin switching/power arrays for price alone. The observed hole density is below the published surcharge threshold, subject to JLCPCB's actual quote and counting rules.
- **Battery shunt land pattern:** R78/R79 now use the project-local `R_2512_Yezhan_DSLR-M-3-1F_2pad` footprint in the schematic and PCB, matching the manufacturer's 1.8 x 3.4 mm pad example. The prior generic 2512 pads were 1.225 x 3.35 mm with a 4.7 mm inner gap, versus the example's 3.4 mm gap. This difference alone did not prove an assembly defect. The older current-sense review's description of a frozen four-pad PCB is historical, not the current board. Assembly fit and Kelvin pickups still require review.
- **Power capacity remains open:** the shunts, FETs, connector and fuse contacts, copper necks, layer transitions, current sharing and thermal conditions need review as a complete path. For example, two ideal 10 mOhm R76/R77 shunts sharing 12 A equally dissipate 0.36 W each; this hypothetical input-path calculation assumes equal sharing and is not an output-current rating or a thermal qualification. Check part derating, actual input current, unequal sharing and heat spreading. Zero unconnected items and high via counts do not prove capacity.

### R78/R79 follow-up - 2026-11-24

The [Yezhan DSLR datasheet](https://datasheet.lcsc.com/datasheet/pdf/fe6999f98bcef072d9153478eaf1d170.pdf?productCode=C41382682), page 2, confirms the project-local pad dimensions above and illustrates sense connections from the inner edges of the pads. Its nominal 2512 part is 6.3 mm long with 1.2 mm terminals. Assuming a centered nominal part, the former pad pattern overlaps each terminal by only about 0.8 mm along its length, compared with about 1.2 mm in the manufacturer's pattern. This is a geometric comparison, not proof that the former assembly would fail.

On a scratch copy, changing only the four R78/R79 pads to the manufacturer's geometry produced two new 0.20 mm clearance violations between R79 pad 2 and nearby opposite-net vias (actual 0.13 and 0.15 mm). It also caused eight silkscreen warnings and two library mismatch warnings because the trial intentionally retained the original footprint graphics and library ID. There were zero unconnected items. In addition, two tented vias near R78 pad 1 have drill edges approximately 0.04 and 0.07 mm inside the enlarged pad's mask opening; this creates a potential solder-wicking concern that DRC alone did not flag.

A second scratch trial moved those two R78-area vias 0.35 mm away from the pad, and moved the two R79-area vias 0.10 mm away from pad 2. After refill it had no clearance violations or unconnected items, while the same trial-only library and silkscreen warnings remained. The pad mask expansion was zero and the moved R78-area drill edges were outside the opening.

**Applied after Phil's request:** the schematic and PCB now use the complete project-local footprint for R78/R79, the four vias were moved as above, and R78 was moved 0.10 mm upward to clear the new courtyards. The replacement footprint has no 3D model; the generic 2512 model was removed rather than presented as an exact match. The four moved vias remain in same-net filled copper on all four layers in the checked copy. A refill and schematic-parity DRC on the applied files reports the same five inherited physical/library warnings and 200 parity warnings as the comparison baseline, with zero unconnected items. This does not qualify current sharing, soldering or the electrical measurement; verify final manufacturing outputs and power-path thermal performance.

The U11 sense inputs are connected through NT7/NT8 into the larger F.Cu power fills, not separately routed to the resistor-side edges shown in the datasheet. NT7's power-side pad center is about 2.8 mm from R79 pad 2's center; NT8's is about 5.9 mm from R79 pad 1's center. Copper and contact drops between those points and the shunt electrodes can affect the reading; no measurement error has yet been measured. At 10 A through two ideal 1 mOhm parallel shunts, the nominal differential signal is only 5 mV. Review where to pick up the parallel pair and how current is shared before rerouting the sense pair or relying on calibration. The 8-10 A charge target does not by itself establish the maximum current through this battery path in every operating mode.

### MOSFET assembly and conduction review - 2026-11-24

Phil reports that the other routing looks fine. This visual review does not by itself establish current sharing, Kelvin accuracy, EMI or thermal performance.

**Prototype disposition:** Phil sees no problem with the existing MOSFET footprints; retain them for Rev2. Rev1 CAD uses the same source/gate pad dimensions for these six devices. The comparisons below are recorded for context, not a requirement to redesign the footprints. Normal assembly inspection and any JLCPCB DFM feedback still apply.

The current PCB uses `Package_TO_SOT_SMD:TDSON-8-1` for **both** the four TI `CSD18540Q5B` converter FETs M1-M4 and the two Infineon `BSC040N10NS5` output switches Q5/Q6. All six map pads 1-3 to source, pad 4 to gate and the large pad 5 to drain; the observed net assignments are consistent with that mapping. Their shared footprint has 0.85 x 0.50 mm source/gate copper pads, with the current paste export using approximately 0.75 x 0.40 mm rounded apertures. These devices do not have identical manufacturer-recommended patterns:

- TI's [CSD18540Q5B datasheet](https://www.ti.com/lit/ds/symlink/csd18540q5b.pdf), section 7.2, recommends 1.372 x 0.710 mm source/gate lands and, in section 7.3, 1.072 x 0.562 mm source/gate stencil openings. The existing M1-M4 pads and paste openings are appreciably smaller. Compare the entire drain copper, mask and stencil pattern too, not only the source/gate lands, before changing these footprints.
- Infineon's [BSC040N10NS5 datasheet](https://www.infineon.com/assets/row/public/documents/24/49/infineon-bsc040n10ns5-datasheet-en.pdf), Figure 3, shows 0.925 x 0.50 mm source/gate lands and 0.825 x 0.40 mm stencil openings. Q5/Q6's present corresponding dimensions are closer to that example than to TI's, but still not identical; the rounded aperture area is also not identical to the rectangular example. Review the drain pad/mask/paste pattern separately. Do not copy a TI-specific footprint to Q5/Q6 just because all six currently share a library footprint.
- JLCPCB generates its SMT stencil from verified assembly data rather than directly from the supplied paste Gerber. Confirm its proposed lands and stencil for the **actual selected parts** and whether all six devices are being placed; provide part-specific manufacturer drawings. Differences from an example are a DFM/assembly question, not proof of failure.

For orientation only, the Infineon part specifies up to 4.0 mOhm at VGS=10 V and 25 C, and 5.6 mOhm at VGS=6 V and 25 C, under the datasheet's stated test currents. If 12 A continuously passes through **each** fully enhanced series switch, these cold-resistance values imply approximately 0.58 W per switch at 10 V, or 0.81 W per switch at 6 V, excluding copper, transient and switching losses. Actual current, VGS and junction temperature are not established by this calculation; resistance rises with temperature and the two devices thermally interact. The datasheet's junction-to-ambient figure uses a specified 6 cm2 70 um copper test board and cannot be assigned directly to this layout. Check maximum credible current and gate drive, copper heat spreading, protection and a rated-load thermal test rather than treating nominal charge current or package current rating as a pass.

### Output power-path review - 2026-11-24

The current board's output sequence is `V_out` -> R16/R80 -> `V_out_sense` -> Q5 -> `SRC` -> Q6 -> `CHG_OUT_POS` -> F2 -> R78/R79 -> J3 `BATT_PACK_POS`. On a refilled copy, the output-shunt and Q5/Q6 source areas have same-net copper on multiple layers and the four recently moved battery vias remain present. The F2-side `CHG_OUT_POS` and `Net-(F2-Pad1)` zone fills are on F.Cu and B.Cu, not the inner layers. The through-hole F2 pads also connect layers; a projection of copper under a surface pad on an inner layer alone does not establish a connection.

Of the 14 `CHG_OUT_POS` vias, eight have filled same-net copper at their centers on both outer layers; six have no filled-zone copper at their centers and may serve tracks or other functions. All six `Net-(F2-Pad1)` vias have filled copper at their centers on both outer layers. `SW_1_-` has 119 vias, 117 with outer-layer zone copper at their centers; `SW_2` has 151 vias with zone copper on F.Cu, In1.Cu and B.Cu at their centers. These are geometric observations from this refill, not proof of via current sharing, a current rating or redundancy. Do not remove power or switching vias based on these counts. Examine physical necks and local current crowding, and verify thermal performance at the actual current.

R16/R80 are two parallel [5 mOhm, 1 W Susumu parts](https://www.lcsc.com/product-detail/C2073455.html). With ideal equal sharing at 12 A total, each dissipates 0.18 W; with 70/30 sharing, the hotter part dissipates about 0.35 W. These examples do not establish temperature rise or the maximum operating current; account for copper resistance, shunt tolerance, actual current and derating. R78/R79 are 1 mOhm, 5 W each; with ideal equal sharing at 12 A, each dissipates 0.036 W.

L2's schematic and PCB value fields now say `3.3uH`, correcting the former generic `L`; the existing `C49308530` sourcing field identifies [MetalLions MMPC1206-3R3MT](https://www.lcsc.com/product-detail/C49308530.html), 3.3 uH, 17 A typical temperature-rise current, 35 A typical saturation current and 7.1 mOhm maximum DCR in the documented datasheet. Confirm the actually ordered part matches. At an illustrative 12 A RMS, 7.1 mOhm gives about 1.02 W winding loss before hot-resistance increase and core loss. Actual inductor RMS and peak currents depend on operating point and ripple; rated-load thermal testing and core loss remain open. J1/J3's generic prototype terminal selection remains as previously accepted by Phil (DEC-059); check the actual fitted connector and wire ratings rather than treating the catalog field as proof of the installed part.

**Additional copper and sourcing check:** in the refilled copy, sampled vertical sections of the `CHG_OUT_POS` pour between Q6 and F2 around x=148-149 mm show approximately 7 mm of zone fill on each outer layer, before accounting for drill holes and local current crowding. This is not a minimum cross-section, a complete path model or an ampacity result; the FET terminals, fuse contacts, other sections and real current sharing still matter. No vias were removed.

R76/R77 remain specified as the 175 C `KRL2012E-C-R010-F-T5`; no LCSC code has been assigned to that exact part. LCSC lists a related 155 C [KRL2012E-M-R010-F-T5, C5342352](https://www.lcsc.com/product-detail/C5342352.html), also 10 mOhm, 1 W and 1% in 0508. It is a **candidate**, not an automatic substitute: confirm actual assembly availability, temperature derating and suitability before changing the BOM. The exact C variant is also listed by other distributors; choose sourcing based on the assembly plan.

**Ordering metadata corrected:** R6, R19, R34, R36, R47, R48, R53-R59, R82 and R86 are 10 kOhm resistors; their schematic correctly identifies LCSC `C17414` (10 kOhm), but the PCB fields incorrectly identified `C17415` (10 Ohm). Updated those 15 PCB sourcing fields to match the schematic and the component values; no routing changed. KiCad's remaining parity count stays at 200 because the 15 resolved mismatches are replaced by pre-existing missing `LCSC ` field warnings on those footprints. The whitespace-suffixed field is separate from the corrected `LCSC` field and needs separate reconciliation, not a component substitution.

**Gate-drive geometry:** the four U6 driver-output nets to R69/R70/R71/R72 have approximately 22.3/37.4/24.6/31.2 mm total track length respectively, each with two vias and most of the length on In2. The corresponding resistor-to-FET gate nets add approximately 3.8/3.5/3.2/1.9 mm on F.Cu. These are total net lengths, not measured loop areas. Bootstrap capacitors C22/C23 connect to HB1/HB2 through F.Cu tracks without vias; their HB pad centers are approximately 3.8/5.1 mm from the U6 pins. VCC2 capacitors C29/C45 are about 7.6/8.4 mm from the U6 VCC2 pin by straight-line pad-center distance. TI recommends minimizing each complete gate/return and decoupling loop; these dimensions alone do not establish a pass or failure. Phil has visually reviewed the routing; switching behavior and heating still need prototype verification.

## Prototype order-readiness review - 2026-11-23

**Status: not ready to order.** This review uses the saved CAD checkpoint
`a16ab6b`, merged to main as `206b7c9`. It is an automated CAD and
document review, not a complete visual, electrical, thermal, BOM or DFM
approval. The earlier placement and pre-route handoffs below are historical
and their counts and unrouted statements do not describe this checkpoint.

The board is now routed as far as KiCad connectivity is concerned: KiCad
10.0.6, with zones refilled for the check but not saved, reports **zero
unconnected items** and five physical warnings. There are also 206
schematic-parity warnings and 246 ERC warnings. Isolated-copper reporting
is disabled in the saved project; enabling it on a separate copy reports
14 isolated-copper warnings. Zero unconnected items does not establish
current capacity, signal integrity, or correct physical routing.

**C62/C63/J8 reconciliation:** Phil chose to retain J8's existing 2.54 mm
four-pin header rather than change the routed footprint to the 2.5 mm JST
XH. The schematic now selects the existing footprints and matching values
for J8 and the 100 nF C62/C63 capacitors. The obsolete JST sourcing and
datasheet fields on J8 were cleared; select a compatible 2.54 mm header and
matching cable for the BOM before ordering. The schematic's embedded J8
symbol is still named `B4B-XH-A`, but its footprint and sourcing fields no
longer specify that connector; its four pin assignments are unchanged.
Review J8's mechanical fit and the capacitor land patterns against the
actual selected parts. The three empty `LCSC ` fields were removed; the
other sourcing-field warnings remain a separate BOM task. No PCB routing
was altered in this reconciliation.

### Copper review - saved checkpoint `4d2fd09`

This is a read-only layout review, not a current-capacity or fabrication
approval. KiCad 10.0.6 refilled a separate copy of the saved board with
isolated-copper reporting enabled. It reported zero unconnected items and
20 isolated-copper warnings: 14 in `PWG_NEG_ SWNET` (`PWR_NEG`), two in
`agnd_1` (`PWR_NEG`), and one each in `BATT +` (`BATT_PACK_POS`),
`PV_IN_POS`, `SW_2`, and `SW1_5V` (`Net-(U4-SW)`). The former misleading
`BATT +_1` ground-zone name from the previous checkpoint has been changed
to `PWG_NEG_ SWNET`. The project still ignores isolated-copper warnings.
The DRC report identifies each zone, but repeats the zone position rather
than identifying the exact polygon; use layer-by-layer highlighting.

1. **Check the stackup before sizing copper.** Checkpoint `4d2fd09`
   specified 0.070 mm outer copper, 0.0175 mm In1 and 0.175 mm In2.
   Phil confirmed In2 should be about 0.5 oz, not 5 oz; the board metadata
   now specifies 0.0175 mm for both inner layers. With the existing
   dielectrics unchanged and 2 oz outer copper, total modeled thickness is
   1.635 mm. The prototype decision is recorded below.
   Confirm the actual stackup, finished copper and via plating before
   relying on these values in current/thermal calculations.
2. **Inspect floating fills, especially the larger ones.** The refilled
   copy contains isolated contours on all four layers. Examples include
   a `PWR_NEG` island on In2 around x=126.89-137.86, y=98.81-112.79 mm
   and a `PV_IN_POS` island on In2 around x=127.87-137.04,
   y=76.33-78.73 mm. Inspect each polygon on its own layer. Remove unused
   copper or connect it only where it serves an intentional purpose and
   is safe; avoid automatically extending the switching nodes `SW_2` and
   `Net-(U4-SW)`. Rerun DRC with this warning enabled after any edits.
3. **Review current-sense pickup before accepting routing.** For the
   LM51772 peak shunts R22/R75, NT1 has `SW_1_-` on both pads; R27's
   sense input shares that power net rather than a separately named net
   up to the pickup. NT1/NT2 are between the parallel shunts, not at each
   resistor pad. Check where the filled copper carries their pickup
   currents and make the CSA/CSB connections genuinely Kelvin at the
   intended shunt electrodes. TI's layout guidance calls for Kelvin
   connections and paired CSA/CSB and ISNSP/ISNSN traces
   (`lm51772-datasheet.txt`, sections 9.9.1.1-9.9.1.3). The input, output,
   and battery INA241 sense taps NT3-NT8 likewise connect via power-net
   copper; check their exact pickup at the shunt electrodes. For example,
   NT8's power-side pad center is about 5.9 mm from R79 pad 1's center.
   The battery pair's sense tracks are on F.Cu; the other pairs have layer
   transitions. These observations require layout review, not a claim
   that a measurement error has already been measured.
4. **Review power and driver loops.** The power nets use zones in addition
   to traces; therefore track width or the number of vias alone does not
   establish the limiting path. Trace the filled current paths from input
   and output capacitors through the MOSFETs, shunts, inductor and returns,
   including necks and via transitions on every layer. The saved power
   zones specify solid pad connections; verify actual copper geometry and
   ampacity against the confirmed stackup. `SW_2` has 151 and `SW_1_-` has
   119 vias, each specified as 0.6 mm diameter with 0.3 mm drill; these
   counts do not prove that the required layers are connected or that the
   current is evenly shared. U6 HO1/LO1/HO2/LO2 nets each have two vias
   and approximately 22/37/25/31 mm of total track respectively, before
   any additional gate-resistor-side routing. Check gate and return loops
   together against TI's short, paired-routing guidance.

After layout changes, refill and repeat DRC, parity and visual layer
inspection. Record the actual copper stack, via construction, limiting
cross-sections and current/thermal assumptions before an order decision.
The initial review changed only the user-confirmed In2 stackup metadata
and total thickness; the subsequent `agnd_1` cleanup is recorded below.

#### Individual island review

Phil chose to review the six zones individually rather than remove every
island at once. Each trial below changed only that zone's island-removal
setting in a separate board copy and refilled it. Every trial reported zero
unconnected items, but a refill can change lower-priority fills; this does
not prove current capacity or establish that a changed fill is desirable.
Positions are board coordinates in mm, not the DRC marker position.

| Zone and warnings at `4d2fd09` | Location and individual trial | Review before applying |
|---|---|---|
| `PWG_NEG_ SWNET` / `PWR_NEG`: 14 | Floating areas appear on all four layers. The In2 example spans x=126.89-137.86, y=98.81-112.79. Removing this zone's islands on the copy leaves six warnings in other zones. | The reported ground islands were removed as described below; review the associated routing and local exclusions after layout changes. |
| `agnd_1` / `PWR_NEG`: 2 | Two small F.Cu islands were near C23/R23 and R27/R26. Removing only these allowed a new `PWG_NEG_ SWNET` island to form near R27/R26. | Cleaned up together as described below; revisit the cutout if the nearby layout or ground zones change. |
| `BATT +` / `BATT_PACK_POS`: 1 | In1 island near R78 at x=146.14-146.84, y=120.28-120.71. The zone outline also touched itself. | The outline repair below connects this island. Review the battery and neighboring current paths as part of the remaining power-path review. |
| `PV_IN_POS`: 1 | In2 island near M1 at x=127.87-137.04, y=76.33-78.73, about 12.85 mm2. Removing it allows ground fill. | Connected with the revised via below; confirm fabrication and assembly details and review power and gate loops. |
| `SW_2`: 1 | F.Cu island near R71/M4, about 0.20 mm2. Its removal lets about 0.44 mm2 of `V_out` fill appear on F.Cu. | Removed as described below, without extending the switching node. Review the gate and output current loops separately. |
| `SW1_5V` / `Net-(U4-SW)`: 1 | In1 island near L3, about 1.49 mm2. Its removal lets about 3.44 mm2 of `Cntrl GND` fill appear on In1. | Removed as described below, without extending the switching node. Review the U4/L3 loop and control routing separately. |

The reloaded fill data exposes more island-marked contours than the DRC
warning count for the main ground zone, so do not treat the reported
coordinates as a one-to-one mapping of DRC warnings to contours. Recheck
each resulting fill and DRC after a zone change. The individual trials
themselves changed no workspace copper.

**`agnd_1` cleanup:** Set this zone to remove islands and added a
0.71 x 0.66 mm cutout to `PWG_NEG_ SWNET` near R26/R27 to prevent the
replacement floating fill. The cutout belongs to the multilayer zone and
therefore applies on all four copper layers, including future refills.
Against the refilled baseline, the two removed F.Cu islands account for
approximately 0.592 mm2; other ground fill differed only by negligible
rounding on In1 (about 0.00000037 mm2). The saved board was refilled.
With isolated-copper reporting enabled on a copy, DRC reports 18 remaining
isolated-copper warnings, zero unconnected items and no new non-isolated
findings. The main ground zone still has 14 warnings. The project itself
continues to ignore isolated-copper warnings. This cleanup does not
establish power-path capacity or complete the remaining copper review.

**Battery-zone review:** The `BATT +` outline doubles back to a repeated
vertex at x=146.815, y=120.77 mm, creating a self-touching loop near the
island. Two alternative refills were tested on copies of the board after
the `agnd_1` cleanup:

- Setting this four-layer zone to remove islands removes the 0.224 mm2
  In1 battery island and allows approximately 0.400 mm2 of connected
  `Cntrl GND` fill to appear on In1. The self-touching outline remains.
- Removing the doubled-back loop instead produces a simple outline and
  connects the existing island to the battery fill. This adds about
  0.770 mm2 of battery copper on In1 and 0.038 mm2 on In2, and displaces
  about 0.351 mm2 of connected `Cntrl GND` and 0.011 mm2 of connected
  `SRC` on In1. `SRC` includes the Q5/Q6 source pads. This is a
  power-zone geometry change, not just warning cleanup.

Both trials report 17 isolated-copper warnings, zero unconnected items
and no new non-isolated DRC findings compared with the board after the
`agnd_1` cleanup.
Phil chose the outline-repair approach. The repaired outline and refilled
copper are now saved in the board; island removal for this zone remains
set to never. The In1 and In2 fills were visually compared, and the
connected-contour counts for the battery, `SRC`, and ground zones did not
decrease on any layer. The project still ignores isolated-copper warnings;
the warning count comes from the copy with reporting enabled. These checks
do not prove shunt pickup quality, clearances beyond configured DRC rules,
or current capacity. Review those paths before ordering.

**PV input island review:** The `PV_IN_POS` zone has one isolated In2
contour near M1 and input capacitors C7/C8. M1's PV pad is on F.Cu; the
island has no connection to the PV net and therefore is not currently a
current-carrying path. The zone outline is not self-intersecting. On a
copy after the battery-outline repair but before the `SW_2` cleanup,
setting the four-layer zone to remove islands
removes the 12.85 mm2 PV island and lets approximately 14.19 mm2 of
connected `PWR_NEG` fill appear on In2. The new ground is near U6 gate
traces, so this is not electrically identical to leaving floating copper;
review the gate and return loops separately. A very small distant change
to `PV_FUSED_POS` In2 fill (about 0.0002 mm2 removed) also occurred.
The trial reported 16 isolated-copper warnings, zero unconnected items,
and no new non-isolated DRC findings. No PV-zone change has been applied
to the workspace. Neither removing the island nor the DRC result proves
that the existing PV power path has adequate current capacity.

Phil asked to investigate connecting the PV island. A separate copy adds
one `PV_IN_POS` through via at x=134.40, y=76.80 mm, with 0.60 mm diameter
and 0.30 mm drill. It is outside M1's pad and its F.Fab body outline; its
outer edge is only about 0.14 mm from the outer edge of that drawn body
outline. In the trial, the via joins the island to the same F.Cu PV fill
as M1 pad 5 and to the PV fills on the other layers. Existing filled-zone
geometry changes only by negligible rounding; the island remains copper
instead of being replaced by ground. DRC reports 16 isolated-copper
warnings, zero unconnected items and no new non-isolated findings.

The board requests front and back via tenting, not plugging or filling.
One 0.30 mm drill via is not evidence that the island or input path can
carry the required current, and this trial does not validate the input
capacitor, switch, gate-drive or return loops. The earlier via at
x=134.40, y=76.80 mm was not applied.

**PV connection:** Added the 0.60/0.30 mm via at x=135.00, y=76.80 mm
after testing it on a copy. Its copper edge is about
0.50 mm outside the closest drawn component courtyard, rather than
overlapping M1's courtyard as in the earlier trial. Its closest routed
other-net copper is about 0.60 mm away, on the U6 HO1 driver net. The
via joins the In2 island to the existing PV fill on the other layers;
M1's drain and C8's positive pad are on that same F.Cu fill, and the
F.Cu PV fill also touches C7's positive pad. Compared with the current
board, zone-fill differences are negligible, no other DRC findings were
added, and the isolated-copper and unconnected-item counts are both zero
with isolated-copper reporting enabled on the review copy. The saved
project still ignores isolated-copper warnings. KiCad's board settings
request front and back tenting; confirm the chosen fabricator's actual
process.
The new connection changes the island's electrical potential and does
not prove current capacity or acceptable input and gate-loop behavior.

The M1 footprint also warrants a separate assembly check: its large-pad
paste pattern uses four 1.5 mm square windows and four smaller windows,
whereas the CSD18540Q5B [TI datasheet](https://www.ti.com/lit/ds/symlink/csd18540q5b.pdf)
section 7.3 shows eight large-pad windows and four smaller ones. This is
not by itself proof of a defect, but compare the actual stencil aperture
areas and thickness with the assembler before ordering.

**`SW_2` cleanup:** The F.Cu island near R71 and M4 was disconnected
switching-node copper. Set the `SW_2` zone to remove islands and saved the
refilled board; the setting also applies to this zone's In1 and B.Cu layers
on future refills. The cleanup removes approximately 0.199 mm2 of floating
`SW_2` and allows approximately 0.438 mm2 of connected `V_out` fill to
appear nearby on F.Cu. The connected `SW_2` contours are preserved, and
the connected-contour counts for `SW_2` and `Vout_1` did not decrease.
A separate approximately 0.000076 mm2 of `V_out` fill was removed farther
away during refill. After this cleanup, with isolated-copper reporting
enabled on a copy, DRC reported 16 isolated-copper warnings: 14
main-ground, one PV input, and one U4 switch-node warning; it also reported
zero unconnected items and no new non-isolated findings. The PV trial above was based on the
earlier board and is not included in this count. The project still ignores
isolated-copper warnings. Gate-loop and power-path qualification remain
open.

**5 V switch-node cleanup:** The `SW1_5V` In1 island beside L3 and the
R9/R12 control trace was disconnected. Set the four-layer zone to remove
islands and saved the refilled board; the setting also applies on future
refills. Removing approximately 1.491 mm2 of floating switch copper lets
approximately 3.438 mm2 of connected `Cntrl GND` fill appear on In1.
The connected switch and ground contour counts are unchanged, and no
other zone fill changed significantly. The DRC copy with isolated-copper
reporting enabled reports 15 remaining warnings: 14 main-ground and one
PV-input island, with zero unconnected items and no new non-isolated
findings at that checkpoint. The project still ignores isolated-copper warnings. The ground
boundary and nearby control trace should still be considered during the
U4/L3 switching-loop and noise review.

**Small main-ground islands:** Two overlapping, disconnected `PWR_NEG`
fragments between the L2 `SW_1_-` via arrays were removed: one on In1 and
one on In2, each approximately 0.212 mm2 at x=110.14-110.66,
y=81.25-81.87 mm. A further isolated In2 fragment near R22, approximately
0.242 mm2 at x=118.67-119.12, y=79.61-80.44 mm, was removed after
checking its physical connections. At that checkpoint, `PWG_NEG_ SWNET`
was set to remove islands below 0.25 mm2. Compared with the board before
this cleanup, these three fragments were the only significant filled-zone
changes. A copy with isolated-copper reporting enabled reported 12 warnings:
11 main-ground and one PV-input island, with zero unconnected items and no
new non-isolated findings.

The cutoff applies to this zone on all four layers on future refills, so
recheck its effect after layout changes. KiCad's reloaded island flags
and DRC counts are not one-to-one: an F.Cu contour measured at about
0.016 mm2 still appeared after a separate 0.05 mm2 cutoff trial, despite
being marked as an island. Also, the R22 fragment lost its saved island
flag after the first two fragments were removed, although it had been
marked before and the geometry/contact checks found no connection to
ground copper. Do not rely solely on these flags or the size setting to
identify every island. The project still ignores isolated-copper warnings.

**Output-sense ground fragments:** Three disconnected `PWR_NEG` fragments
between NT5 and NT6 near R16 were removed: approximately 0.507 mm2 on
F.Cu, 0.400 mm2 on In2 and 0.507 mm2 on B.Cu. Contact checks found no
connections from these fragments to ground vias, tracks, pads or other
ground contours. The sense tracks and connected copper were unchanged;
one larger In1 fragment remains in the same area for separate review.
At that checkpoint, the ground-zone cutoff was 0.53 mm2. With isolated-copper
reporting enabled on a copy, DRC reported nine warnings: eight main-ground
and one PV-input island, zero unconnected items and no new non-isolated
findings. The project itself
still ignores isolated-copper warnings. The output-shunt Kelvin routing
and analog noise performance remain open reviews.

**Peak-current sense-area fragment:** Removed a disconnected 0.624 mm2
`PWR_NEG` fragment on In2 near M2 and the `SW_1_+`/`SW_1_-` sense tracks,
at x=129.38-130.34, y=83.49-84.51 mm. Contact checks found no ground vias,
tracks, pads or ground contours touching it. It was the only significant
filled-zone change in the trial; the sense tracks and connected copper
were unchanged. At that checkpoint, the ground-zone cutoff was 0.64 mm2 on all four
layers and will affect future refills. With isolated-copper reporting
enabled on a copy, DRC reports eight warnings: seven main-ground and one
PV-input island, zero unconnected items and no new non-isolated findings.
The R22/R75 Kelvin pickup and gate/power-loop reviews remain open.

**Further main-ground fragments:** Removed four disconnected `PWR_NEG`
contours: approximately 0.858 mm2 on In1 near NT5/NT6, 0.825 mm2 on In2
near R26/R27, and 1.224 and 1.994 mm2 on In1 near R22/R75. Contact checks
found no ground vias, tracks, pads or ground contours touching them.
Comparisons showed no significant changes to other zone fills; the sense
tracks were unchanged. The saved `PWG_NEG_ SWNET` cutoff is now 2.10 mm2
on all four layers and also removes smaller disconnected islands on future
refills, so review its effect after layout changes. With isolated-copper
reporting enabled on a copy, DRC reports three main-ground warnings and
one PV-input warning, zero unconnected items and no new non-isolated
findings. The saved project still ignores isolated-copper warnings.

**Larger main-ground islands:** Three disconnected contours were removed:
approximately 6.235 mm2 on F.Cu near Q3/J7, 12.028 mm2 on In1 near
output-sense routing, and 32.184 mm2 on In2 near the output and U6 HO2
gate-drive routing. Their layer geometry was visually reviewed; the In2
contour ran alongside the output copper and gate-drive route.

Added one named, single-layer copper-pour exclusion over each contour,
retaining the 2.10 mm2 ground-zone cutoff rather than automatically
removing every future ground island. The refill removed only those three
contours, with no other significant zone-fill changes, no unconnected
items and no new non-isolated DRC findings; only the PV island warning
remained. The exclusions permit tracks, vias, pads and footprints, but
prevent **all nets' zone fills** within those local areas on future
refills. Inspect or revise them whenever neighboring layout or intended
copper changes. Their removal does not establish Kelvin, gate-loop, EMI
or current-path adequacy.

### JLCPCB fabrication, assembly and power-path review

Phil selected JLCPCB for fabrication and assembly and confirmed **2 oz
outer copper** for the prototype, superseding the earlier 1 oz selection.
The CAD stackup models 0.070 mm outer copper and retains 0.0175 mm
(about 0.5 oz) inner copper.
With the previously modeled dielectrics and mask unchanged, the modeled
total thickness is 1.635 mm; this is not confirmation of JLCPCB's actual
finished board thickness or dielectric stack. Confirm the exact order
stackup, finished copper and via plating, and reevaluate current/thermal
capacity for the selected copper.
JLCPCB's [copper-weight guide](https://jlcpcb.com/help/article/jlcpcb-copper-weight)
currently specifies a 0.16 mm minimum trace width and spacing for 2 oz
FR4; confirm the capabilities for the actual order. A scratch DRC using
0.16 mm board-wide minimums reported 132 outer-layer track segments below
that width and 12 different-net 0.15 mm pad-gap violations on U10/U11/U12.
These scratch rules are not saved to the project; resolve these potential
manufacturing violations against the confirmed order specifications.

A separate scratch trial widened those 132 tracks to 0.18 mm. It created
four violations of the existing 0.20 mm clearance rule that need local
rerouting if that widening were used; it did not alter the workspace
board and is not an approved prototype change. On another copy, reducing
the three INA241 footprints' pad widths from 0.50 to 0.45 mm cleared the
12 pad-gap violations. The [INA241A datasheet](https://www.ti.com/lit/ds/symlink/ina241a.pdf)
DDF land-pattern example uses 0.45 mm-wide pads at 0.65 mm pitch, but the
trial retains the existing pad lengths and centers and is not the complete
TI pattern. It also produces three library-mismatch warnings. Review land
geometry, solder mask, paste and JLCPCB assembly requirements, and use a
maintained footprint rather than silently overriding the library. Neither
trial has been applied.

**Further 2 oz trials:** On a fresh copy of the current board, combining
the 0.18 mm track widening with local moves of three vias and one track
corner cleared those four clearance findings. A separate change to all
three INA241 footprints modeled the TI DDF example's 1.05 x 0.45 mm pads,
2.60 mm row-center spacing and 0.05 mm pad corner radius. The combined
copy reported no track-width, clearance, isolated-copper or unconnected
findings under the 0.16 mm scratch minimums, but added three footprint
library-mismatch warnings. The 0.20 mm INA pad gaps have no margin over
JLCPCB's published 2 oz minimum pad spacing for a solder-mask bridge;
confirm the mask and assembly treatment. The trial is a feasibility check,
not an approved footprint, final routing or manufacturing sign-off.
No trial copper changes were applied to the workspace.

**Annular rings and via guidance:** JLCPCB's detailed
[PCB capabilities](https://jlcpcb.com/capabilities/pcb-capabilities)
table lists 0.254 mm or greater under PTH annular ring for 2 oz multilayer
boards. This is a stated size figure, not a manufacturing tolerance; the
table does not explicitly resolve its relationship to ordinary vias.
Separately, its via guidance says the via diameter should exceed the
hole diameter by 0.10 mm, with 0.15 mm preferred, for multilayer boards.
All 847 board vias are nominally 0.60 mm diameter with 0.30 mm holes:
a 0.30 mm diameter difference and a 0.15 mm radial ring. They meet that
general via-size guidance, but it does not explicitly specify the 2 oz
case or guarantee the finished ring.

Another 27 plated footprint holes have size-minus-drill radial rings below
0.254 mm: 23 small thermal holes in U3/U4/U5/U9 and four slotted J2 shield
holes. Confirm how JLCPCB applies its PTH guidance to these holes and to
the 2 oz via stackup; do not assume KiCad's existing 0.10 mm minimum
establishes compliance.
Phil confirms that JLCPCB accepted Rev1 with these via dimensions and 2 oz
copper. This is relevant precedent, though not confirmation of acceptance
of the exact Rev2 files and order settings. Retain the current via size and
review any DFM feedback at submission rather than making blanket routing
changes or treating the 0.254 mm figure as a confirmed rejection threshold.
For scale, a scratch-only change of the 847 via diameters to 0.81 mm
(0.255 mm nominal radial ring), without resizing footprint holes, produced
297 clearance violations, 14 shorts, two copper-sliver findings and two
unconnected items. Blanket enlargement is not a viable fix.

**Smaller via changes and source ambiguity:** Do not treat 0.81 mm as a
confirmed minimum via diameter. JLCPCB's PTH table does not explicitly
resolve whether its 0.254 mm figure applies to ordinary vias on this
order, while its [annular-ring design article](https://jlcpcb.com/blog/annular-rings-pcb-design)
gives generic Class 2 design targets of 0.15 mm outer and 0.175 mm inner.
Those are general targets, not acceptance of this particular 2 oz stackup.
During the order-specific DFM review, clarify any flags or proposed pad
changes for the tented 0.60/0.30 mm vias, 0.20 mm plated thermal holes
and J2 slots, including effects on clearance, cost and testing. If needed,
ask how the PTH figure relates to the separate via guidance.

Using the current board and the same 0.16 mm scratch manufacturing
minimums, every through via was increased uniformly on separate refilled
copies, keeping the 0.30 mm hole and all other geometry. Compared by item
identity to the 0.60 mm baseline, results were:

| Via diameter | Nominal radial ring | New findings over baseline | Unconnected items |
| --- | --- | --- | --- |
| 0.60 mm | 0.150 mm | Baseline: 12 clearances and 132 narrow tracks | 0 |
| 0.61 mm | 0.155 mm | 103 clearances | 1 |
| 0.62 mm | 0.160 mm | 120 clearances, 2 copper slivers | 1 |
| 0.65 mm | 0.175 mm | 161 clearances | 1 |
| 0.70 mm | 0.200 mm | 214 clearances | 1 |
| 0.80 mm | 0.250 mm | 310 clearances, 2 copper slivers | 2 |
| 0.81 mm | 0.255 mm | 306 clearances, 14 shorts, 2 copper slivers | 2 |

These are DRC findings, not counts of required independent reroutes.
Many are small infringements of the existing 0.20 mm net-clearance rule;
the newly unconnected reports identify the `Cntrl GND` zone rather than
the exact disconnected contour. Refilling and electrical inspection are
needed before assessing each change. A separate local-only trial enlarged
five `PWR_NEG` vias in the debug-area stitching row to 0.65 mm; it added
no DRC findings or unconnected items, but does not prove reliability or
approve a board change. Selective enlargement might be feasible where
space permits, not as a substitute for confirmed fabrication requirements.
Decreasing the drill instead changes barrel geometry and may incur
small-hole charges; do not assume equal power-current capacity or accept
that tradeoff without review. None of these trials changed the workspace.

**Via count, stitching and pricing review:** The current board has 847
through vias. A conservative geometric inventory found 370 touching or
near a same-net track or pad and 477 with no such nearby track or pad.
The latter are candidates for individual inspection, not confirmed
redundant stitching: they may connect filled copper across layers or carry
power, return or thermal current. Among those 477 are 149 `PWR_NEG`, 106
`SW_2`, 63 `SW_1_-`, 46 `PV_IN_POS`, 37 `V_out` and 34 `SW_1` vias.
Preserve the power and switch-node arrays until current sharing, loop
geometry and thermal effects are reviewed; a connectivity-only check
cannot justify removing them.

The scratch drill report has 930 holes and slots total: 847 vias, 81 other
plated holes or slots and two non-plated holes. The KiCad board outline
area is approximately 95.46 cm2, giving roughly 97,400 holes/m2 when all
930 are counted. JLCPCB's [extra-charge guidance](https://jlcpcb.com/help/article/in-what-cases-will-there-be-charged-extra)
lists a high-drill-density surcharge above 150,000 holes/m2. This
single-board estimate is below that threshold, but the quote, panel
geometry and JLCPCB's counting method govern the actual price; no saving
from removing vias is established. The 23 plated 0.20 mm footprint holes
may incur a separate small-hole charge: JLCPCB describes standard holes
as at least 0.30 mm. Confirm both the quote and whether its annular-ring
requirement applies to these holes and the ordinary vias.

For a limited stitching feasibility trial, removed two `PWR_NEG` vias at
(163.101849, 138.33) and (164.851849, 138.33) mm from a five-via row
near the debug area on a copy. Three vias remain in that row. After refill,
the scratch 0.16 mm-rule DRC retained the same 151 violations and zero
unconnected items; isolated-copper reporting was enabled. Filled-zone
area and outline counts did not change measurably. This does not establish
equivalent return-current, EMI or transient behavior, and no removal was
applied. Inspect stitching by function and group, including neighboring
signals and return paths, rather than automatically deleting vias that
lack track connections.

The same capabilities table describes 2 oz multilayer trace/space as
0.15 mm, while the separate copper-weight guide above lists 0.16 mm for
2 oz. The 0.18 mm widening trial provides some width margin over both,
but JLCPCB must confirm the actual rules and tolerance for this order.

The four converter MOSFETs M1-M4 use `TDSON-8-1`. Their source/gate copper
pads are 0.85 x 0.50 mm and the exported rounded paste apertures are
0.75 x 0.40 mm. TI's CSD18540Q5B example specifies 1.372 x 0.710 mm
source/gate lands and 1.072 x 0.562 mm paste apertures. The actual rounded
apertures have approximately 49% of the example aperture area; the drain
paste geometry also differs. This is a material assembly review item, not
proof that the boards cannot be assembled. JLCPCB says its
[SMT assembly stencil](https://jlcpcb.com/help/article/smt-stencil-data-prepared-for-smt-orders)
is engineered from its SMT data rather than directly from the supplied
paste Gerber. Confirm the copper land pattern and generated stencil with
JLCPCB for all four MOSFETs before ordering.

The added PV via has a 0.30 mm drill, and the saved board requests front
and back tenting; scratch mask Gerbers have no via flash at that position.
JLCPCB's [via-covering guidance](https://jlcpcb.com/help/article/pcb-via-covering)
lists tenting for this drill size, but tenting is not plugging and does
not guarantee a sealed hole. Confirm the intended via treatment and
production files with JLCPCB.

On F.Cu, C7 positive and M1 drain touch the same PV fill, C7 negative and
M2 source touch the same ground fill, and M1 source and M2 drain touch the
same switching-node fill. These contacts do not establish adequate
cross-sections, via sharing, ripple current or loop inductance. Power-path
capacity remains open pending the actual stackup, current envelope,
limiting necks and via transitions.

The gate routes also need a focused layout review. Stored segment lengths
from U6 to the gate resistors total approximately 22 mm HO1, 37 mm LO1,
25 mm HO2 and 31 mm LO2, excluding resistor-to-gate segments and via
barrel lengths; totals can include branches. HO1 and HO2 run largely on
In2, while their SW references and bootstrap connections include F.Cu
tracks. Compare the gate/return and local capacitor loops against TI's
guidance; these observations alone do not establish switching performance.

**PV shunt ordering correction:** R76/R77 were intended as two 10 mOhm
`KRL2012E-C-R010-F-T5` parts, but their schematic value, datasheet,
description and LCSC `C2073455` selected a 5 mOhm part; their PCB value
and datasheet fields also named that 5 mOhm part. Aligned the schematic and PCB metadata
with the existing 10 mOhm `Part` and `Resistance` fields and cleared the
incorrect schematic LCSC code. Embedded symbol and footprint tags retain
historical 5 mOhm library names as provenance; do not use those names as
the order part number. No verified JLCPCB code has yet been selected for
the intended 10 mOhm parts. If two 5 mOhm parts are fitted instead, the
effective nominal shunt is 2.5 mOhm rather than 5 mOhm, changing PV-current
calibration by a factor of two.

Work through these steps before preparing an order:

1. **Reconcile CAD and BOM.** Confirm the selected C62/C63 parts fit the
   retained footprints and select J8's exact 2.54 mm header and mating
   connector. Verify J8's existing BMS pin mapping before use. At the
   checkpoint, among 199 field-parity warnings, 179 concerned a missing
   `LCSC ` field, and 20 concerned differing `LCSC` fields: R6, R19, R34,
   R36, R47, R48, R53-R59, R82 and R86 have PCB `C17415` versus
   schematic `C17414`; at that checkpoint R46, R76, R77, F1 and F2 had a
   schematic LCSC number but none on the PCB. R76/R77 have since been
   corrected as described above; re-run parity after the latest edits.
   Reconcile sourcing against actual part specifications rather than
   copying numbers blindly. The remaining checkpoint parity warning is
   J9's intentionally omitted key pad 7.
2. **Review zone islands and power paths.** Enable isolated-copper
   reporting for the review, refill, and inspect each island on its actual
   layer; connect it to the correct net where needed or remove it where
   unused. The previously reported islands have been addressed as described
   above; repeat the review after subsequent edits. Do not blanket-connect
   switching-node copper or remove copper without reviewing its purpose.
   Check power-zone continuity, necks, pad/thermal connections, layer
   transitions and via current capacity against the intended current and
   copper weight. The saved PCB has 0.15-0.20 mm tracks on `PV_IN_POS`
   and 0.20 mm tracks on `V_out`; those tracks alone do not establish the
   current path or its capacity, since zones may also carry current.
3. **Review critical routing and hardware.** Inspect LM51772 input/output
   high-current loops, switch-node area, gate-drive and return paths, and
   independent Kelvin pickups to R22/R75 and R78/R79. Check D5 polarity
   and connections, feedback-jumper defaults, startup/configuration
   straps, ADC power-off injection/backfeed, and the selected component
   voltage/current/thermal ratings. The nominal ISET limit is not a
   guaranteed 12 A shutdown and `FAULT_OCP` is an MCU signal. Verify any
   unresolved design question capable of changing the board before order;
   powered protection and compensation qualification remain bring-up work.
4. **Resolve remaining CAD and mechanical findings.** At the earlier
   checkpoint, review identified five physical warnings: U2/U8 differ from
   their library footprints, U5 has a silkscreen overlap, and U6 silkscreen
   is clipped at pad 1. At that checkpoint, ERC reported 213 field-name
   whitespace warnings (`LCSC `), 32 pin-type warnings and
   one symbol-library mismatch; clean up or explicitly review them. Check
   INA241 local 0.15 mm clearance overrides against the selected fab's
   capabilities; also inspect C3/C18 placement, connector mating and
   access, ESP antenna clearance, test/rework access and component polarity.
5. **Finalize the prototype order package.** Verify manufacturer parts,
   footprints, sourcing, DNPs, jumper defaults and assembly instructions;
   confirm board outline, four-layer stackup, copper weight, thickness,
   drills and fab capabilities with the chosen manufacturer. Rerun
   ERC/DRC/parity with deliberate settings and review all remaining
   warnings. Generate fresh Rev2 fabrication outputs and inspect the
   Gerbers, drills and assembly/BOM against the checked CAD before ordering.

After assembly, independently verify SWD/programming, rail sequencing,
converter startup and protection, buck/boost transients, calibration and
thermal behavior before rated-power use. F1 remains accepted by Phil's
disposition (DEC-076); an exact BMS model is not a prototype-order blocker
(DEC-077). Neither decision claims powered or field qualification.

## PCB-order walkthrough - 2026-11-20

Review the existing implementation before proposing more changes. The statuses below come from merged handoffs and decisions, not a fresh whole-board CAD audit. Issue bodies and closed/open labels can lag those handoffs.

Phil finalized F1 as a non-issue for this Rev2 order. Retain the selected fuse/holder, keep #7 closed by disposition, and remove the cold-Voc mitigation task as an ordering prerequisite (DEC-076). This records a scope decision, not a new voltage-rating or test result.

Phil also confirmed that the future BMS will use I2C through existing J8. The exact model is TBD and is not a Rev2 prototype PCB-order blocker (DEC-077). Retain the existing J8 pinout and shared PB6/PB7 bus; do not add a speculative UART/CAN/RS485 transceiver. This reserves the intended connection, not compatibility with an unidentified BMS.

Work through one row at a time. Record each result here as **complete**, **specific remaining work**, **decision needed**, or **post-build check**, with a short source reference. Do not reopen an accepted choice merely because an older issue still asks for it. Separate fabrication readiness from firmware completion and powered qualification.

| Step | Existing progress to verify | Completion needed for the PCB order |
|---|---|---|
| 1. Establish the exact CAD baseline | Merged #33/#15 schematic packets, #22 power topology, and #17 board edits already exist. Earlier schematic-only PCB freezes were stage boundaries, not completed layout. | Confirm the latest saved Rev2 schematic/PCB and any newer work are accounted for before editing. Inventory schematic-to-PCB differences without broadly synchronizing or changing the board in this planning pass. Preserve Rev1. |
| 2. Close the pin and connector budget | USART2 PB3/PB4, retained USART3 console, TXU0204, manual ESP recovery, PA0/PA1/PC4 current sensing, PA4/PA2/PA3 voltage sensing, and R86 BOOT0 pull-down are documented. SWD-only J9 is provisionally placed/routed. BMS is assigned to existing J8 I2C (DEC-077). | Verify those choices in current CAD rather than redesigning them. The exact BMS is TBD, not a prototype-order hold. Confirm actual SWD mating/adapter mapping and connector/enclosure access. |
| 3. Close the converter and supply schematic | Internal-feedback JP1/JP2 defaults, R46=32.4kOhm plus R68=1.3kOhm, OCP scaling, R22/R75 shunts/filter starting values, and #22 topology are documented. | Check actual connectivity, startup/configuration straps, D5 polarity, ratings, compensation and power-off/ADC behavior. Decide only genuine remaining design holds. Carry forward the estimated ISET range and the fact that FAULT_OCP only reports to the MCU; do not describe them as a guaranteed 12A shutdown. |
| 4. Reconcile BOM and footprints | Three INA241A3 channels, Susumu shunts, and the corrected two-pad Yezhan footprint are in the schematic handoff. No new TVS is selected for this board (DEC-074); C24=22nF remains the starting value. | Verify exact parts, footprint/pin mapping, DNP/jumper defaults and availability. Identify only differences still missing from the PCB, including the legacy battery-shunt geometry. |
| 5. Finish PCB implementation | Some board work is already present; the handoffs explicitly leave other schematic changes unrouted or unsynchronized. | After the review, synchronize the agreed changes deliberately. Complete placement and routing, paired Kelvin pickup, switching/gate loops, returns, antenna keepout, test access and connector clearances. Recheck completed areas instead of replacing them wholesale. |
| 6. Check and generate the order package | Historical ERC/DRC counts are partial-baseline comparisons, not final acceptance. | Resolve shorts, required unconnected nets, schematic/PCB mismatches and fabrication-relevant findings; review remaining warnings explicitly. Check current/thermal assumptions, board stackup, drill/outline, polarity, assembly/BOM and DFM. Generate and inspect a distinct Rev2 fabrication package from the checked source. |
| 7. Keep post-build work separate | Calibration, telemetry/update/charging firmware, SWD bench attach, supply sequencing, converter startup/protection and load/thermal qualification are not established by CAD. | Retain these as post-build tasks where they do not change the required hardware. Any unresolved question that can still change topology, component ratings or protection design returns to steps 2-4 before ordering; new-board powered tests are not prerequisites to manufacturing that board. |

The initial walkthrough did not itself authorize PCB edits. Phil subsequently requested this PR's implementation, then reserved physical part placement for himself. The current handoff below supersedes the earlier schematic-only PCB freezes for this synchronization; it does not authorize fabrication.

### Current implementation handoff - 2026-11-20

- **Baseline complete:** fetched current main and incorporated `d61b845` (#38, external ST-Link adapter) without accessing or changing another worktree. The controller CAD baseline before synchronization matched the #17 handoff: 198 footprints, 259 track/via objects, 16 zones, 60 physical DRC findings, 344 unconnected items and 86 schematic-parity findings.
- **Schematic-to-PCB synchronization complete:** the board now contains all 227 schematic components. Added the 29 missing footprints, corrected R75's Susumu footprint and R78/R79's Yezhan two-pad land patterns, and reconciled values, sourcing fields, net assignments and schematic UUID links. Existing #22 power topology, BOOT0, J9 and the two separate ESP/STM32 supply domains are retained. The existing J8 netlist confirms the BMS connection recorded in DEC-077.
- **Placement belongs to Phil:** the 29 added footprints are deliberately staged outside the board to the right, at x=214-250 mm / y=46-116 mm. They are not placed, routed or part of a fabrication-ready layout. R75/R78/R79 retain their old centers/orientations but their replacement pad geometry needs placement/routing review. No new in-board placement or automatic routing was attempted.
- **Known short corrected:** trimmed only the erroneous end of `I_BATT_-` track `4dc76fe0-a257-4ab2-b935-738aea01b59e` to NT8 pad 2 instead of reaching pad 1. This removes the previously documented battery-sense short without changing its intended net.
- **Preservation checks:** all 198 existing component centers, orientations and sides are unchanged. All 259 track/via identities and 16 zone identities remain; only the known short's track geometry changed. Existing zone boundaries and the board outline are retained, and zones were refilled and saved. The exported schematic's 700 endpoints match the board except J9's deliberately absent physical key pad 7. CAD object UUID uniqueness was checked.
- **Current checks, not release approval:** KiCad 10.0.6 reports 59 physical findings (13 errors / 46 warnings), 394 unconnected items and one intentional key-pad parity finding. No required component/value/field/net mismatch remains. The unconnected count increases because the previously missing circuitry is now present and needs routing. Remaining physical errors are 12 INA241-package pad-clearance findings against the current 0.20 mm rule and the existing C3/C18 courtyard overlap; these require actual review, not blanket exclusions. The unchanged schematic ERC baseline is 0 errors / 55 warnings.

**Placement recovery:** Phil's saved placements from the issue #17 worktree were transferred onto the synchronized board: 21 staged footprints and 22 existing footprints. Only footprint positions and rotations were transferred. The original worktree was left untouched; its project settings and its deletion of one `PWR_NEG` segment near J9/D9 were not imported. The synchronized nets, shunt footprints and NT8 short correction are retained. Updated KiCad results: 90 physical findings (13 errors / 77 warnings), 395 unconnected items and the same intentional J9 key-pad parity finding.

**Current phase: placement and pre-route review.** Phil owns placement; the recovered positions are a work in progress, not a placement sign-off. At Phil's direction, TP19-TP26 are omitted from this revision; all UART signal paths are unchanged. The earlier handoff also proposed LED-chain test points TP27/TP28, but neither is in the current CAD; this change adds neither. While placement is flexible, settle any schematic, protection, BOM/footprint, antenna-clearance or connector-access decisions that could force parts to move. Review the three replacement shunt footprints, INA241 pad-clearance rules and C3/C18 courtyard overlap now. Leave the existing copper alone during this review; revisit the J9/D9 ground connection and other affected nets during routing.

**After placement and layout-affecting decisions are settled:** finish routing, then complete final ERC/DRC/parity/DFM checks before generating an order package. Firmware, powered qualification and exact BMS compatibility remain separate checks; the fuse and TBD BMS model are not prototype-order blockers.

**2026-11-21 checkpoint:** Phil has placed the components for now and corrected D5 in both CAD views. Review and clean up DRC findings as work proceeds, with a pre-route pass and a final post-route pass before ordering. Review the INA241 pad clearance against the intended fab process and the C3/C18 courtyard overlap, then finish any LM51772 protection, ADC power-off and mechanical/BOM decisions that could still change the layout. Route the power and Kelvin sense paths first, including the corrected D5 gate/source connections.

## Baseline and scope

- Retain LM51772 (Phil's decision, DEC-067). No BQ25756 evaluation or purchase is required for this plan; the unperformed comparison in issue #20 is not a new blocker to the selected architecture.
- BMS communications is included in Rev2 (Phil confirmed 2026-11-09), fulfilling the Rev1 deferral in DEC-062. Only its implementation details remain open; do not defer it again or treat it as an optional feature.
- Continue from the Rev2 consolidation lineage at `20aa6fa`, preserving its preceding CAD edits and portable libraries. The saved-source investigation in [rev-two-current-sense-review.md](rev-two-current-sense-review.md) describes the earlier PCB baseline at `488441a`; it is not the current schematic state.
- Rev2 began as a copy of Rev1 and has since been revised; it is not qualified or ready for fabrication. Preserve Rev1 and its manufacturing package.
- Keep the existing 4S LiFePO4 / 2P panel baseline and current design targets: 125 W continuous output, up to 150 W PV operating power, 8-10 A nominal charging, approximately 12 A hardware protection pending verification. The project name does not establish a 200 W output rating.
- Work toward a functional, serviceable charger prototype. Do not silently add an expanded communications/UI feature set or require a formal lab dossier.

## A. Fix or reconcile

The [current-sense review](rev-two-current-sense-review.md) records the historical baseline and the newer shunt/filter layout. The current PCB still has unconnected CSA/CSB filter nets and has not been electrically qualified or approved for fabrication.

| Item | Planned work | Done when / tracker |
|---|---|---|
| Peak-current sense path | Retain R26/R27 = 10 Ohm and C20 = 180 pF as prototype starting values, with C61 DNP and R22/R75 = 0.005 Ohm each. Complete the unconnected filter and shunt-side routing, review Kelvin pickup, and calculate the filter against the operating-point minimum pulse. Resolve false-trip versus real-peak-current behavior by measurement. | Calculated current threshold and measured behavior agree; intended protection remains effective. [#19](https://github.com/Phil-CIA/Solar-Project-200w/issues/19). |
| Feedback wiring versus firmware | The schematic selects internal feedback with JP1 bridged from U6 FB to VCC2 and JP2 open to the R3/R25 external divider. For external feedback, open JP1 and bridge JP2; never bridge both or leave both open. The assigned footprints match those defaults; the PCB implementation and routing remain in progress. Update the PCB and assembly instructions, and verify VIN-FB scaling and DIV20 configuration. | Hardware mode, firmware target range/readback and independently measured Vout agree. [#21](https://github.com/Phil-CIA/Solar-Project-200w/issues/21). |
| Average-current limiting / ISET | Explicitly choose the pin-controlled or register-controlled limit and design/program it accordingly. Reconcile R68/R46, ISET, MODE and D9 selection; calculate limits separately from the CSA/CSB peak-current limit. Do not carry the bench VCC2 jumper forward as an approved limit. | Both average and peak protection have defined limits, startup states and validation evidence. [#8](https://github.com/Phil-CIA/Solar-Project-200w/issues/8), [#19](https://github.com/Phil-CIA/Solar-Project-200w/issues/19), [#10](https://github.com/Phil-CIA/Solar-Project-200w/issues/10). |
| Input sensing / UVLO / configuration | Recalculate VIN-FB and EN/UVLO for the intended source range; investigate the persistent INPUT status without assuming it caused the earlier stall. Check current ADDR/SLOPE, MODE, RT, configuration straps and ground returns against the actual Rev2 source, not September net descriptions. Define PCM behavior for charging. | Thresholds, divider values, switching mode/frequency and reset/startup settings are intentional and documented. #8, #10. |
| Control-supply power path | Initial Rev2 topology is implemented per DEC-068: remove D4; feed U4 VIN from `CHG_OUT_POS` through D6; feed `CTRL_5V` directly from USB `VBUS` through D7. Qualify battery-only startup through the battery/output path, USB-only rail voltage and U4 behavior when its output is externally biased, both-source interaction, and diode current/thermal margins. | Netlist and PCB agree; source-state behavior and ratings are measured/verified before the control supply is treated as qualified. USB service power must not authorize charger switching. [#22](https://github.com/Phil-CIA/Solar-Project-200w/issues/22), [#4](https://github.com/Phil-CIA/Solar-Project-200w/issues/4), [DEC-068](decisions-log.md). |
| D5 / output protection | Polarity correction complete in the saved Rev2 schematic and PCB: pin/pad 1 (cathode) is on the shared Q5/Q6 gate, and pin/pad 2 (anode) is on `SRC`. | D5 copper connections are still unrouted; verify them and the assembly marking during layout review. Default-off, reverse-flow and clamp qualification remain separate from this CAD correction. [#9](https://github.com/Phil-CIA/Solar-Project-200w/issues/9) is already closed. |
| Compensation, soft start and gates | Start Rev2 with C24 = 22 nF, R31 = 2.43 kOhm and C25 = 1.5 nF. All three are separate 0805 footprints on the top side, allowing same-footprint value substitutions during bring-up without a PCB spin. Keep them accessible for rework after assembly. The old 220 nF entry was a documentation error; Rev1's fitted C24 remains unverified and does not block Rev2. | Record any Rev2 substitutions in the bring-up notes and verify regulation and load transients in buck and boost before rated-power use. C26 = 690 nF and 22 Ohm gates are not accepted merely because they were fitted on the bench. [#23](https://github.com/Phil-CIA/Solar-Project-200w/issues/23), #10. |
| Safe firmware defaults | Separate bench overrides from the charging application. Define inhibited boot/reset/watchdog/update behavior and battery/PV qualification; review initialization that currently drives PA5 high. | No USB-only, invalid-source or recovery state unintentionally enables charging. [#5](https://github.com/Phil-CIA/Solar-Project-200w/issues/5), #8. |

These are a mixture of confirmed discrepancies and unresolved design/qualification questions, not a claim that every listed circuit is defective.

For internal feedback, JP1 must connect FB to VCC2 before U6 startup. The Rev1 bench `U6 TEST` checks D8 DIV20 and verifies target-register readback; its `VOUT_mV` is the programmed target, not a measured output. Neither this bench behavior nor the schematic jumper symbols establish Rev2 startup, regulation, or firmware qualification. Confirm measured output independently at multiple targets and input voltages.

## B. Add

| Addition | Planned scope | Decision or constraint |
|---|---|---|
| Defined BOOT0 | R86 = 10 kOhm is in the Rev2 schematic, PCB and exported BOM, with pad 1 assigned to U1 pin 46/PB8-BOOT0 and pad 2 to `PWR_NEG`. | [#14](https://github.com/Phil-CIA/Solar-Project-200w/issues/14) covers adding the pull-down. Route both pads and resolve the R86 silkscreen overlap in the board DRC passes. Verify normal boot at bring-up and review option bytes when OTA work starts. |
| Actual voltage/current telemetry | The Rev2 schematic maps INA241A3 output/PV/battery current to PA0/PA1/PC4; PV voltage is on PA4, output voltage is assigned to PA2, and an independent battery-terminal divider from J3.2 before the shunts is assigned to PA3. The PA2/PA3 assignments are accepted for these channels; the BMS interface pin allocation remains a separate open decision. R78/R79 use a project-local two-pad footprint matching the Yezhan land pattern; the frozen PCB still has the old four-pad geometry and awaits later synchronization. | This branch's #15 handoff is schematic-only: no PCB sync, placement/routing, firmware/calibration, or powered qualification. Keep ADC power-off behavior, component ratings, current-limit accuracy, peak-current validation, PCB routing/review, and safe fault behavior open. [#15](https://github.com/Phil-CIA/Solar-Project-200w/issues/15), [#6](https://github.com/Phil-CIA/Solar-Project-200w/issues/6). |
| Control-rail transient protection | Select coordinated protection for CTRL_5V/CTRL_3V3 and relevant external entries, with appropriate current limiting/return paths. | Check actual clamp voltage at expected pulse current against IC limits; a nominal TVS voltage alone is not protection proof. [#16](https://github.com/Phil-CIA/Solar-Project-200w/issues/16). |
| Keyed debug connector | J9 replaces redundant J4 in its former board area, using the keyed 10-position ARM Cortex SWD mapping at 1.27 mm pitch. Samtec FTSH-105-01-L-DV-007-K remains a candidate with pin 7 omitted and a manufacturer-based project footprint. | [#17](https://github.com/Phil-CIA/Solar-Project-200w/issues/17) is closed for the CAD addition. SWD-only/no JTAG/SWO; PB3/PB4 remain USART2. Final routing, cable/adapter mating and signal mapping, enclosure access, and bench SWD remain separate checks. |
| Useful test access | While modifying each block, provide accessible control rails, output/battery voltage, inhibit/fault and sense measurement points where needed. | Keep current-sense pads small and paired; no long stubs or added pickup loops on sensitive nets. Test access is not permission to use earth-grounded probes on switching nodes. |
| Robust local telemetry link | Integrate the existing protected ESP32 provisioning application with the STM32 telemetry protocol, fault/status reporting and disconnect handling. | STM32 retains charger authority. Treat networking firmware progress separately from PCB changes. [#11](https://github.com/Phil-CIA/Solar-Project-200w/issues/11). |
| ESP data UART and service access | [#33 schematic handoff](esp32-stm32-firmware-delivery-plan.md#rev2-schematic-only-handoff-issue-33): USART2 PB3/PB4 AF7, separate retained USART3 console, TXU0204 isolation, manual RST/BOOT, endpoint/LED-chain pads. | Saved PCB recovery is complete, retaining #22; the resulting board is frozen. Only subsequent schematic/footprint work is included. Later PCB update/routing, verified antenna keepout, access and firmware remain open. |
| BMS communications | Retain J8's existing I2C connection for the future BMS (DEC-077): pin 1 SCL/PB7, 2 PWR_NEG, 3 CTRL_3V3, 4 SDA/PB6. No additional peripheral pins or speculative transceiver are selected. | Exact BMS TBD is not a Rev2 prototype-order blocker. Before connection, confirm electrical levels, grounding/isolation, addresses, cable loading, connector mapping and protocol against the selected device and the existing shared bus. Define stale/missing-data and fault handling before use; received data cannot bypass local protections. |

## C. Decide before freezing connector/pin assignments

| Open choice | Proposed handling |
|---|---|
| Internal feedback | Already represented by the schematic's JP1 bridged / JP2 open default described in section A. Verify PCB implementation and assembly instructions rather than requesting the same schematic change again. |
| Measurement hardware | The schematic now captures three current channels and independent PV/output/battery voltage channels with the #15 MCU assignments and reconciled changed-part BOM/footprints. Calibration, input protection/power-off behavior, accuracy targets, board routing, and powered review remain open; do not treat the scoped schematic handoff as issue closure. |
| Debug interface | SWD-only/no SWO selected under DEC-070. Redundant J4 is removed and keyed 10-position J9 is added in its former area using the Samtec FTSH candidate. #17 is closed for the header addition; see the keyed debug connector row for remaining checks. |
| BMS communications implementation | DEC-077 assigns the intended BMS connection to existing J8 I2C and keeps the model TBD without blocking the prototype order. No new UART/CAN/RS485 hardware or MCU allocation is needed for that decision. Actual device compatibility and firmware remain open until the BMS is selected. |
| ESP32-assisted STM32 updates | Decide build-now versus later. If included, freeze transport/reset/boot/memory/recovery needs before layout. Until qualified, SWD remains recovery; charging stays inhibited during update. [#24](https://github.com/Phil-CIA/Solar-Project-200w/issues/24). |
| Other features | Explicitly choose scope for coulomb-counting/SOC, load-output/LVD, CAN/RS485/NMEA2000, persistent history/UI and outdoor packaging. None is implicitly approved. [#12](https://github.com/Phil-CIA/Solar-Project-200w/issues/12). |

## D. Work order

1. **Freeze the change list and pin/interface budget.** Retain the intended BMS connection at J8 I2C (DEC-077), then verify the implemented feedback, measurement and debug choices and which remaining optional interfaces need hardware now. The exact BMS model is not a prototype-order blocker. Keep unselected optional interfaces explicitly deferred.
2. **Resolve converter and supply design.** Work through CSA/CSB, ISET, feedback, input sensing/UVLO, startup and D4/D6 as one consistent power/control design. Use existing bench evidence; request only new measurements that change a design decision. If the peak-current root cause remains unresolved, leave that block open rather than guessing a permanent filter.
3. **Update the Rev2 schematic and BOM.** The current-sensing packet is schematic-only. Review it and, after agreement, update the matching PCB and BOM; add other agreed boot/debug/telemetry/protection parts, correct polarity/net documentation, and review the complete pin/address/power budget. Separate approved changes from experimental options.
4. **Update placement and routing.** Prioritize shunt Kelvin pickup, U6 grounding and switching loops, gate returns, decoupling, protection return paths and connector clearance. Keep schematic and PCB changes paired.
5. **Close design/release checks.** Compare ERC/DRC/parity against the inherited baseline; review every new finding and disposition existing relevant warnings. Reconcile BOM, footprints, pin/polarity mapping, part ratings and library portability before generating a distinct Rev2 fabrication package.
6. **Bring up in stages after manufacture.** Verify safe states, control rails, debug, calibrated measurements, low-power regulation/protection, source recovery and then load/thermal performance. Keep new-board testing separate from this planning task.

**Historical issue #15 schematic-only stop point:** the earlier handoff deliberately froze the PCB while preparing the schematic and changed-part BOM/footprints. That stage restriction is superseded by the 2026-11-20 synchronization handoff above. Phil now owns placement of the staged parts. Issue #15 remains open for placement/routing, remaining electrical review, firmware calibration, protection verification and powered acceptance.

## Fabrication and operating gates

- Do not order from the copied baseline or with peak-current/average-current protection undefined.
- Check C18 ripple/overshoot margin, L2 loss/temperature, R16 stress, MOSFET/gate timing and compensation for the selected envelope; old open items are qualification questions, not automatic replacement instructions (#10).
- F1 is finalized by Phil's 2026-11-20 disposition (DEC-076): retain the selected Keystone 3522-2 and keep #7 closed. Cold-Voc mitigation work is not a Rev2 PCB-order prerequisite. This disposition does not change component ratings or establish new operating/test evidence.
- Retain remote I2C battery-temperature sensing at J8; do not assume a spare MCU ADC/NTC channel. Confirm sensor/interface implementation and actual battery/BMS limits before charging.
- Require unambiguous SET/MEAS, fault reporting, default-off/recovery behavior and relevant protection checks before increasing power. A successful low-power run does not establish rated power or outdoor readiness.

The current-sensing and #33 interface packets are schematic-only implementation blocks, not a PCB release. After the one-time saved-layout recovery, #33 explicitly excludes PCB synchronization, placement, routing and keepout editing. Its expected parity differences and later actions are listed in the [handoff](esp32-stm32-firmware-delivery-plan.md#rev2-schematic-only-handoff-issue-33). Review open electrical choices before carrying either packet to the board; keep remaining optional features separate.
