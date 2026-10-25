# Rev 1 Board Notes

Updated: 2026-10-25
Related work: [#3](https://github.com/Phil-CIA/Solar-Project-200w/issues/3), [development tracker](https://github.com/Phil-CIA/Solar-Project-200w/issues/1)

## Working Baseline

This is Phil's assembled Rev 1 hobby prototype. Keep practical notes here; a full inspection dossier, part-by-part certification, and formal lab reports are not required. Record what is known, leave unknowns visible, and check safety-relevant details when they affect the next operation.

Phil identified this local folder as the files used for manufacture:

`C:\Users\forch\OneDrive\JLCPCB files\Solar Charger\Rev 1`

The folder was read without modifying it. Its `Kicad`, `GERBERS`, and `BOM` files establish the working design baseline; they do not prove every installed component matches the BOM.

## What We Established

| Item | Finding |
|---|---|
| Board identity | Rear photo reads `Build : Rev 1` and silkscreen date `20260915`. That is a board marking, not an independently known assembly date. |
| Photos supplied by Phil | `IMG_2886.jpeg` is the component side; `IMG_2887.jpeg` is the rear. Both are in the local manufacturing folder. Originals remain local. |
| PCB source | Package `Kicad\Solar Project Rev1.kicad_pcb` matches the repository PCB byte-for-byte. The repository copy is present in commit `fb765d197bd30d30961d699cf8284cfb41d7052c`; the exact commit used at order time is not known or needed to start practical bring-up. |
| Schematic | Package schematic matches the repository schematic after removing 176 empty `LCSC ` properties. No remaining text difference was found after that normalization. Do not overwrite either source to make its hash match. |
| Project settings | Differences are limited to the BOM export filename and ordering of `used_designators`. |
| Fabrication files | All 14 loose files in the repository's `hardware\kicad\solar-project\fab-output-2026-09-18` match the supplied package. All 13 files inside its Gerber ZIP match the loose package files; the drill-map PDF is outside the ZIP. |
| Export metadata | Gerber job identifies KiCad 10.0.5, export date 2026-09-18, four copper layers, and an approximately 100.08 x 100.04 mm outline. |
| Stackup caveat | Job metadata gives 1.7925 mm total thickness and unequal inner copper thicknesses (0.0175 / 0.175 mm). Treat these as export settings, not confirmed manufacturer stackup; do not use them as measured copper thickness for current/thermal claims. |
| Historical checks | September notes report clean DRC and a JLCPCB DFM pass. Matching original ERC/DRC reports and a vendor stackup confirmation were not established here; no new electrical check was performed. |

For recognizing the saved package later, its `Solar-Project-Rev1-Gerbers-2026-09-18.zip` SHA-256 is `0da7e3ec911d7c1379927af26a67ee70bce4f8a30de00ea0698967c398ac2701`. There is no requirement to hash each future photo or hobby bench session.

## Useful Assembly Notes

The supplied front photo shows the display, ESP32-style development board, main ICs, power components, and screw terminals fitted. It is not sufficient to declare every solder joint, hidden pad, polarity, or MCU marking verified.

- F1/F2 clips appear empty in the photo. Confirm the actual fuse installation and ratings when planning a powered test; do not infer them from the holder footprint.
- J8 appears unpopulated and no remote battery-temperature sensor is connected in the photo. That does not block disconnected, low-energy control development, but pack-temperature protection must be addressed before charging.
- The BOM names U1 `STM32G431CBU6`, U2 `ESP32_C3_mini`, and U14 `LM74502HDDFR`. Those are design identities, not independently confirmed chip markings. Confirm the MCU target before flashing.
- No powered behavior, firmware compatibility, shorts check, or charging performance has been established in this session.

## BOM Differences Worth Keeping Visible

Use the supplied manufacturing BOM as the working assembly reference, not the stale procurement master CSV. Do not silently replace the master CSV or claim the physical parts were checked.

| Part | Supplied manufacturing BOM | Practical follow-up |
|---|---|---|
| C7 | Grouped with C15/C34/C35 as 2.2 uF; lists both `C50254` (50 V) and `C577211` (100 V). | The grouped row does not say which was fitted at C7. Check the parts used if needed for the intended PV envelope; retain the 100 V design selection from DEC-058 until explicitly changed. |
| C52 | 220 nF, `C5378`. | Resolves the saved BOM's intended value; master CSV still groups C52 with 100 nF parts. Installed value is not proven by the photo. |
| D2 | `SMBJ48CA`, `C151264`. | Matches the later design decision; do not treat the older SMBJ60CA listing as a confirmed installed part. |
| R28 | 51.7 kOhm, `C119851`. | A catalog field exists in the supplied BOM; no supplier availability or actual installed resistance was checked. Do not reopen sourcing solely because older notes say it was missing. |
| R30 | In a 0 Ohm group listing `C17477,C17888`. | Intended CDC-grounded configuration is recorded; do not guess a per-reference supplier mapping from the grouped row. |
| F1/F2 | Keystone `3522-2` holder footprints. | Holder identification does not establish fuse amperage or DC interrupt rating. |
| J8 | Four-pin header only, no sensor module specified. | Sensor choice and installation remain separate from the bare-board BOM. |

## Next Step

Reconcile the low-energy bring-up instructions under [#4](https://github.com/Phil-CIA/Solar-Project-200w/issues/4). Ask only for information needed for the next action: whether the board has already been powered, the available current-limited supply/programmer, and any known assembly changes. No need to reconstruct every purchase or produce additional photos unless a specific uncertainty needs one.
