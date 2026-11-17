# Rev1 and Rev2 library setup

The Rev1 and Rev2 projects in the parent directory share the same project
library tables. They use KiCad 10 and the standard libraries supplied with it.
On each computer:

1. Install KiCad 10 and its standard symbol, footprint and 3D libraries.
2. In KiCad's Plugin and Content Manager, install the JLCPCB KiCad Library
   package (`com.github.CDFER.JLCPCB-Kicad-Library`). Version 2025.07.18 was
   installed when these paths were checked. Use the same version on both
   computers where available; the version is not pinned by this project.
3. Check out the branch containing the desired design and this directory,
   then open `Solar Project Rev1.kicad_pro` or `Solar Project Rev2.kicad_pro`.

The project tables use `${KIPRJMOD}` for the library snapshots here and
`${KICAD10_3RD_PARTY}` for the installed JLCPCB package. Standard KiCad
libraries come from each computer's global library tables. Do not copy
machine-specific absolute paths into the project tables.

The personal symbol and footprint libraries here are snapshots, not live
links to the OneDrive originals. Changes to either copy do not automatically
change the other. Review and commit project-library edits along with the
design, and pull them before editing on another computer.

The DigiKey directory contains only the two footprints referenced by the
current Rev1/Rev2 design, not the complete DigiKey library. The included
`footprints/digikey-LICENSE.md` contains its attribution and license.

`symbols/legacy-3522-2.kicad_sym` was recovered from the identical embedded
Rev1 and Rev2 schematic definitions because the original library was not
found. Its historical default footprint name `1My_Parts_ Passive:3522-2_KEY`
is not present in the copied library. The existing F1 and F2 instances instead
reference `1My_Parts_ Passive:Automotive fuse holder keystone 3522-2_KEY`.
Review the footprint before placing a new symbol or updating existing symbols.

Some copied footprints and board items reference 3D models using old absolute
paths or KiCad 9 variables. These model files have not been made portable;
missing 3D models do not establish that the electrical library links are
correct. Do not update symbols or footprints from libraries without reviewing
the resulting changes.
