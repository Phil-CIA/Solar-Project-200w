"""Generate and validate the original, power-isolated ST-Link clone adapter.

Run with KiCad 10's bundled Python. No upstream adapter CAD is imported.
"""

import csv
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import uuid
import xml.etree.ElementTree as ET
import zipfile

import pcbnew


ROOT = Path(__file__).resolve().parent
BUILD = ROOT / "build"
LIB = ROOT / "Adapter.pretty"
NAME = "STLink-Clone-SWD-A1"
KICAD = Path(sys.executable).resolve().parent.parent
CLI = KICAD / "bin" / "kicad-cli.exe"
NAMESPACE = uuid.UUID("61fe2e54-e063-4a14-9e09-29540b67f01d")
SHEET = str(uuid.uuid5(NAMESPACE, "sheet"))
PARTS = {
    "J1": {
        "symbol": "Clone_10",
        "value": "SSW-105-02-G-D",
        "footprint": "SSW-105-02-G-D",
        "description": "Female 2x5 2.54 mm socket; clone type-B pinout only",
        "datasheet": "https://www.samtec.com/products/ssw-105-02-g-d",
        "position": (63.5, 69.85),
    },
    "J2": {
        "symbol": "Cortex_Keyed_10",
        "value": "FTSH-105-01-L-DV-007-K",
        "footprint": "FTSH-105-01-L-DV-007-K",
        "description": "Keyed 1.27 mm Cortex header; pin 7 omitted; VTref isolated",
        "datasheet": "https://www.samtec.com/products/ftsh-105-01-l-dv-007-k",
        "position": (144.78, 69.85),
    },
    "JP1": {
        "symbol": "Reset_Link_Open",
        "value": "RESET_LINK_OPEN",
        "footprint": "Reset-Link-Open",
        "description": "Normally open solder link; close only after verifying clone STM32 reset",
        "datasheet": "",
        "position": (105.41, 110.49),
    },
}
PINS = {
    "Clone_10": {
        "1": "RST",
        "2": "SWCLK",
        "3": "SWIM",
        "4": "SWDIO",
        "5": "GND",
        "6": "GND",
        "7": "3V3_OUT",
        "8": "3V3_OUT",
        "9": "5V_OUT",
        "10": "5V_OUT",
    },
    "Cortex_Keyed_10": {
        "1": "VTREF_NC",
        "2": "SWDIO",
        "3": "GND",
        "4": "SWCLK",
        "5": "GND",
        "6": "SWO_NC",
        "8": "TDI_NC",
        "9": "GND",
        "10": "NRST",
    },
    "Reset_Link_Open": {"1": "~", "2": "~"},
}
NETS = {
    ("J1", "1"): "CLONE_RST",
    ("J1", "2"): "SWCLK",
    ("J1", "4"): "SWDIO",
    ("J1", "5"): "GND",
    ("J1", "6"): "GND",
    ("J2", "2"): "SWDIO",
    ("J2", "3"): "GND",
    ("J2", "4"): "SWCLK",
    ("J2", "5"): "GND",
    ("J2", "9"): "GND",
    ("J2", "10"): "TARGET_NRST",
    ("JP1", "1"): "CLONE_RST",
    ("JP1", "2"): "TARGET_NRST",
}


def uid(name):
    return str(uuid.uuid5(NAMESPACE, name))


def esc(text):
    return text.replace("\\", "\\\\").replace('"', '\\"')


def write(path, text):
    path.write_text(text, encoding="utf-8", newline="\n")


def run(*args):
    result = subprocess.run(
        [str(CLI), *map(str, args)], capture_output=True, text=True, check=False
    )
    if result.returncode:
        raise RuntimeError(f"KiCad failed: {args}\n{result.stdout}\n{result.stderr}")
    return result.stdout.strip()


def symbol(name):
    reset = name == "Reset_Link_Open"
    graphics = (
        '(circle (center -0.635 0) (radius 0.508) '
        '(stroke (width 0.254) (type default)) (fill (type none)))'
        '(circle (center 0.635 0) (radius 0.508) '
        '(stroke (width 0.254) (type default)) (fill (type none)))'
        if reset
        else '(rectangle (start -7.62 7.62) (end 7.62 -7.62) '
        '(stroke (width 0.254) (type default)) (fill (type background)))'
    )
    if name == "Cortex_Keyed_10":
        graphics += (
            '(text "KEY: NO PIN 7" (at -2.54 -2.54 0) '
            '(effects (font (size 0.8 0.8))))'
        )
    pins = []
    for number, label in PINS[name].items():
        if reset:
            x, y = (-5.08 if number == "1" else 5.08), 0
            length = 4.445
        else:
            n = int(number)
            x = -10.16 if n % 2 else 10.16
            y = 5.08 - ((n - 1) // 2) * 2.54
            length = 2.54
        angle = 0 if x < 0 else 180
        pins.append(
            f'(pin passive line (at {x} {y} {angle}) (length {length}) '
            f'(name "{label}" (effects (font (size 0.8 0.8)))) '
            f'(number "{number}" (effects (font (size 0.8 0.8)))))'
        )
    pin_names = '(pin_names (offset 0.508) (hide yes))' if reset else '(pin_names (offset 0.508))'
    return (
        f'(symbol "{name}" {pin_names} '
        '(exclude_from_sim no) (in_bom yes) (on_board yes) '
        '(property "Reference" "J" (at 0 10.16 0) '
        '(effects (font (size 1.27 1.27)))) '
        f'(property "Value" "{name}" (at 0 -10.16 0) '
        '(effects (font (size 1.27 1.27)))) '
        '(property "Footprint" "" (at 0 0 0) '
        '(effects (font (size 1.27 1.27)) (hide yes))) '
        '(property "Datasheet" "" (at 0 0 0) '
        '(effects (font (size 1.27 1.27)) (hide yes))) '
        '(property "Description" "" (at 0 0 0) '
        '(effects (font (size 1.27 1.27)) (hide yes))) '
        f'(symbol "{name}_0_1" {graphics}) '
        f'(symbol "{name}_1_1" {" ".join(pins)}))'
    )


def schematic():
    symbols = {name: symbol(name) for name in PINS}
    write(
        ROOT / "Adapter.kicad_sym",
        '(kicad_symbol_lib (version 20241209) (generator "kicad_symbol_editor")\n'
        + "\n".join(symbols.values())
        + "\n)\n",
    )
    embedded = [
        text.replace(f'(symbol "{name}"', f'(symbol "Adapter:{name}"', 1)
        for name, text in symbols.items()
    ]
    items = []
    for ref, part in PARTS.items():
        name = part["symbol"]
        x, y = part["position"]
        properties = {
            "Reference": ref,
            "Value": part["value"],
            "Footprint": "Adapter:" + part["footprint"],
            "Datasheet": part["datasheet"],
            "Description": part["description"],
        }
        fields = []
        for i, (key, value) in enumerate(properties.items()):
            at_y = y - 11.43 + i * 2.54 if i < 2 else y
            hidden = "" if i < 2 else "(hide yes)"
            fields.append(
                f'(property "{key}" "{esc(value)}" (at {x} {at_y} 0) '
                f'(effects (font (size 1 1)) {hidden}))'
            )
        pin_ids = " ".join(
            f'(pin "{n}" (uuid "{uid(ref + ":" + n)}"))' for n in PINS[name]
        )
        items.append(
            f'(symbol (lib_id "Adapter:{name}") (at {x} {y} 0) (unit 1) '
            '(exclude_from_sim no) (in_bom yes) (on_board yes) '
            f'(dnp no) (uuid "{uid(ref)}") {" ".join(fields)} {pin_ids} '
            f'(instances (project "{NAME}" (path "/{SHEET}" '
            f'(reference "{ref}") (unit 1)))))'
        )
        for n in PINS[name]:
            if name == "Reset_Link_Open":
                px, py = (-5.08 if n == "1" else 5.08), 0
            else:
                num = int(n)
                px = -10.16 if num % 2 else 10.16
                py = 5.08 - ((num - 1) // 2) * 2.54
            ex, ey = x + px, y - py
            net = NETS.get((ref, n))
            if net:
                justify = "right bottom" if px < 0 else "left bottom"
                items.append(
                    f'(label "{net}" (at {ex} {ey} 0) '
                    f'(effects (font (size 1 1)) (justify {justify})) '
                    f'(uuid "{uid("label:" + ref + ":" + n)}"))'
                )
            else:
                items.append(
                    f'(no_connect (at {ex} {ey}) '
                    f'(uuid "{uid("nc:" + ref + ":" + n)}"))'
                )
    for i, (text, x, y) in enumerate(
        (
            ("ORIGINAL TYPE-B CLONE ADAPTER - A1", 105, 30),
            ("Probe outputs 3.3V / 5V are NOT connected to the target.", 105, 40),
            ("J1: female socket faces probe. Verify actual clone pinout first.", 105, 90),
            ("J2: standard Cortex SWD. VTref intentionally isolated; target self-powered.", 105, 96),
            ("JP1: LEAVE OPEN. Bridge only after verifying STM32 NRST support.", 105, 125),
            ("No SWIM, SWO or JTAG. Pin 7 physically omitted on J2.", 105, 135),
        )
    ):
        items.append(
            f'(text "{text}" (at {x} {y} 0) '
            '(effects (font (size 1.27 1.27))) '
            f'(uuid "{uid("note:" + str(i))}"))'
        )
    write(
        ROOT / (NAME + ".kicad_sch"),
        '(kicad_sch (version 20260306) (generator "eeschema") (generator_version "10.0") '
        f'(uuid "{SHEET}") (paper "A4") '
        '(title_block (title "Power-isolated ST-Link clone to Cortex SWD") '
        '(rev "A1") (comment 1 "Prototype: verify clone pinout and cable orientation")) '
        f'(lib_symbols {" ".join(embedded)})\n'
        + "\n".join(items)
        + "\n(embedded_fonts no))\n",
    )
    write(
        ROOT / "sym-lib-table",
        '(sym_lib_table (version 7) (lib (name "Adapter") (type "KiCad") '
        '(uri "${KIPRJMOD}/Adapter.kicad_sym") (options "") '
        '(descr "Original clone adapter symbols")))\n',
    )
    write(
        ROOT / "fp-lib-table",
        '(fp_lib_table (version 7) (lib (name "Adapter") (type "KiCad") '
        '(uri "${KIPRJMOD}/Adapter.pretty") (options "") '
        '(descr "Manufacturer-dimensioned adapter footprints")))\n',
    )


def vec(xy):
    return pcbnew.VECTOR2I(*(pcbnew.FromMM(n) for n in xy))


def xy(point):
    return (pcbnew.ToMM(point.x), pcbnew.ToMM(point.y))


def layers(*ids):
    result = pcbnew.LSET()
    for layer in ids:
        result.AddLayer(layer)
    return result


def text(parent, value, position, layer, size=1, mirrored=False):
    item = pcbnew.PCB_TEXT(parent)
    item.SetText(value)
    item.SetPosition(vec(position))
    item.SetTextSize(vec((size, size)))
    item.SetTextThickness(pcbnew.FromMM(0.15))
    item.SetLayer(layer)
    item.SetMirrored(mirrored)
    parent.Add(item)


def footprint(name, pads, body, courtyard, ref_pos, smd=False):
    fp = pcbnew.FOOTPRINT(None)
    fp.SetFPID(pcbnew.LIB_ID("Adapter", name))
    fp.SetReference("REF**")
    fp.SetValue(name)
    fp.SetAttributes(pcbnew.FP_SMD if all(not p[3] for p in pads) else pcbnew.FP_THROUGH_HOLE)
    fp.Reference().SetPosition(vec(ref_pos))
    fp.Reference().SetTextSize(vec((1, 1)))
    fp.Reference().SetTextThickness(pcbnew.FromMM(0.15))
    fp.Reference().SetLayer(pcbnew.F_SilkS)
    fp.Value().SetVisible(False)
    for layer, box, width in (
        (pcbnew.F_Fab, body, 0.1),
        (pcbnew.F_CrtYd, courtyard, 0.05),
    ):
        shape = pcbnew.PCB_SHAPE(fp)
        shape.SetShape(pcbnew.SHAPE_T_RECT)
        shape.SetStart(vec(box[:2]))
        shape.SetEnd(vec(box[2:]))
        shape.SetWidth(pcbnew.FromMM(width))
        shape.SetLayer(layer)
        fp.Add(shape)
    for number, position, size, drill in pads:
        pad = pcbnew.PAD(fp)
        pad.SetNumber(str(number))
        pad.SetPosition(vec(position))
        pad.SetSize(vec(size))
        pad.SetShape(pcbnew.PAD_SHAPE_RECT if number == 1 or not drill else pcbnew.PAD_SHAPE_CIRCLE)
        if drill:
            pad.SetAttribute(pcbnew.PAD_ATTRIB_PTH)
            pad.SetDrillSize(vec((drill, drill)))
            layer_set = pcbnew.LSET.AllCuMask()
            layer_set.AddLayer(pcbnew.F_Mask)
            layer_set.AddLayer(pcbnew.B_Mask)
            pad.SetLayerSet(layer_set)
        else:
            pad.SetAttribute(pcbnew.PAD_ATTRIB_SMD)
            pad.SetLayerSet(layers(pcbnew.F_Cu, pcbnew.F_Mask))
            if smd:
                pad.SetLayerSet(layers(pcbnew.F_Cu, pcbnew.F_Paste, pcbnew.F_Mask))
        fp.Add(pad)
    plugin = pcbnew.PCB_IO_MGR.FindPlugin(pcbnew.PCB_IO_MGR.KICAD_SEXP)
    plugin.FootprintSave(str(LIB), fp)


def footprints():
    LIB.mkdir(exist_ok=True)
    footprint(
        "SSW-105-02-G-D",
        [(n, (0 if n % 2 else -2.54, ((n - 1) // 2) * 2.54), (1.7, 1.7), 1.02)
         for n in range(1, 11)],
        (-3.745, -1.525, 1.205, 11.685),
        (-4.0, -1.8, 1.5, 11.95),
        (-1.27, 13.3),
    )
    footprint(
        "FTSH-105-01-L-DV-007-K",
        [(n, (-2.54 + ((n - 1) // 2) * 1.27, 1.715 if n % 2 else -1.715), (0.74, 2.79), 0)
         for n in range(1, 11) if n != 7],
        (-3.175, -3.43, 3.175, 3.43),
        (-3.425, -3.68, 3.425, 3.68),
        (0, -5.2),
        smd=True,
    )
    footprint(
        "Reset-Link-Open",
        [(1, (-0.65, 0), (1, 1.5), 0), (2, (0.65, 0), (1, 1.5), 0)],
        (-1.15, -0.75, 1.15, 0.75),
        (-1.4, -1, 1.4, 1),
        (0, -2),
    )


def board(netlist):
    b = pcbnew.BOARD()
    b.SetCopperLayerCount(2)
    b.GetDesignSettings().SetBoardThickness(pcbnew.FromMM(1.6))
    net_names = sorted({n.get("name") for n in netlist.findall("nets/net")})
    for name in net_names:
        b.Add(pcbnew.NETINFO_ITEM(b, name))
    endpoint_nets = {
        (node.get("ref"), node.get("pin")): net.get("name")
        for net in netlist.findall("nets/net")
        for node in net.findall("node")
    }
    positions = {"J1": (107, 105), "J2": (124, 110), "JP1": (121, 119)}
    fps = {}
    for ref, part in PARTS.items():
        fp = pcbnew.FootprintLoad(str(LIB), part["footprint"])
        if fp is None:
            raise RuntimeError(f"Cannot load {part['footprint']}")
        fp.SetFPID(pcbnew.LIB_ID("Adapter", part["footprint"]))
        fp.SetReference(ref)
        fp.SetValue(part["value"])
        fp.SetField("Datasheet", part["datasheet"])
        fp.SetField("Description", part["description"])
        for key in ("Datasheet", "Description"):
            fp.GetField(key).SetVisible(False)
        fp.SetPath(pcbnew.KIID_PATH(f"/{SHEET}/{uid(ref)}"))
        fp.SetPosition(vec(positions[ref]))
        b.Add(fp)
        fps[ref] = fp
        for pad in fp.Pads():
            net = endpoint_nets.get((ref, pad.GetNumber()))
            if net:
                pad.SetNet(b.FindNet(net))

    def padpos(ref, n):
        return xy(next(p for p in fps[ref].Pads() if p.GetNumber() == str(n)).GetPosition())

    def track(net, points, layer, width=0.25):
        for start, end in zip(points, points[1:]):
            t = pcbnew.PCB_TRACK(b)
            t.SetStart(vec(start))
            t.SetEnd(vec(end))
            t.SetLayer(layer)
            t.SetWidth(pcbnew.FromMM(width))
            t.SetNet(b.FindNet(net))
            b.Add(t)

    def via(net, point):
        v = pcbnew.PCB_VIA(b)
        v.SetPosition(vec(point))
        v.SetViaType(pcbnew.VIATYPE_THROUGH)
        v.SetWidth(pcbnew.FromMM(0.6))
        v.SetDrill(pcbnew.FromMM(0.3))
        v.SetLayerPair(pcbnew.F_Cu, pcbnew.B_Cu)
        v.SetNet(b.FindNet(net))
        b.Add(v)

    # Physical routing is independent of the upstream adapter's geometry.
    track("/SWCLK", [padpos("J1", 2), (106, 103.46), (120.19, 103.46), (122.73, 106)], pcbnew.B_Cu)
    via("/SWCLK", (122.73, 106))
    track("/SWCLK", [(122.73, 106), padpos("J2", 4)], pcbnew.F_Cu)
    track("/SWDIO", [padpos("J1", 4), (103.2, 106.28), (116.9, 106.28), (118.905, 108.285), (119.5, 108.285)], pcbnew.B_Cu)
    via("/SWDIO", (119.5, 108.285))
    track("/SWDIO", [(119.5, 108.285), padpos("J2", 2)], pcbnew.F_Cu)
    track("/CLONE_RST", [padpos("J1", 1), (110, 105), (112, 107), (112, 117), (114, 119), padpos("JP1", 1)], pcbnew.F_Cu)
    track("/TARGET_NRST", [padpos("JP1", 2), (129, 119), (129, 110.745), padpos("J2", 10)], pcbnew.F_Cu)
    track("/GND", [padpos("J2", 3), padpos("J2", 5), padpos("J2", 9)], pcbnew.F_Cu)

    for start, end in (
        ((100, 100), (132, 100)),
        ((132, 100), (132, 124)),
        ((132, 124), (100, 124)),
        ((100, 124), (100, 100)),
    ):
        shape = pcbnew.PCB_SHAPE(b)
        shape.SetShape(pcbnew.SHAPE_T_SEGMENT)
        shape.SetStart(vec(start))
        shape.SetEnd(vec(end))
        shape.SetLayer(pcbnew.Edge_Cuts)
        shape.SetWidth(pcbnew.FromMM(0.05))
        b.Add(shape)
    for layer in (pcbnew.F_Cu, pcbnew.B_Cu):
        zone = pcbnew.ZONE(b)
        zone.SetLayer(layer)
        zone.SetNet(b.FindNet("/GND"))
        zone.SetLocalClearance(pcbnew.FromMM(0.25))
        zone.SetThermalReliefGap(pcbnew.FromMM(0.25))
        zone.SetThermalReliefSpokeWidth(pcbnew.FromMM(0.3))
        zone.Outline().NewOutline()
        for corner in ((100.75, 100.75), (131.25, 100.75), (131.25, 123.25), (100.75, 123.25)):
            point = vec(corner)
            zone.Outline().Append(point.x, point.y)
        b.Add(zone)
    text(b, "CLONE V2 -> SWD  A1", (116, 101.5), pcbnew.F_SilkS)
    text(b, "NO PROBE POWER", (116, 122.3), pcbnew.F_SilkS)
    text(b, "1", (108.8, 103.7), pcbnew.F_SilkS)
    text(b, "1", (119.4, 112.6), pcbnew.F_SilkS)
    text(b, "RESET: OPEN", (124, 120.7), pcbnew.F_SilkS, 0.9)
    text(b, "TYPE-B PINOUT ONLY", (116, 119.5), pcbnew.B_SilkS, mirrored=True)
    text(b, "TARGET SELF-POWERED", (116, 122), pcbnew.B_SilkS, mirrored=True)
    pcbnew.ZONE_FILLER(b).Fill(b.Zones())
    pcbnew.SaveBoard(str(ROOT / (NAME + ".kicad_pcb")), b)


def validate():
    print(run("sch", "erc", "--severity-all", "--exit-code-violations", "--format", "json",
              "--output", BUILD / "erc.json", ROOT / (NAME + ".kicad_sch")))
    print(run("pcb", "drc", "--refill-zones", "--schematic-parity", "--severity-all",
              "--exit-code-violations", "--format", "json", "--output", BUILD / "drc.json",
              ROOT / (NAME + ".kicad_pcb")))


def connected_pads(connectivity, start):
    pending = [start]
    visited = set()
    result = set()
    while pending:
        item = pending.pop()
        identity = item.m_Uuid.AsString()
        if identity in visited:
            continue
        visited.add(identity)
        if isinstance(item, pcbnew.PAD):
            result.add((item.GetParentFootprint().GetReference(), item.GetNumber()))
        pending.extend(connectivity.GetConnectedItems(item))
    return result


def verify():
    root = ET.parse(BUILD / "netlist.xml").getroot()
    groups = [
        {(n.get("ref"), n.get("pin")) for n in net.findall("node")}
        for net in root.findall("nets/net")
    ]
    expected_groups = [
        {("J1", "2"), ("J2", "4")},
        {("J1", "4"), ("J2", "2")},
        {("J1", "5"), ("J1", "6"), ("J2", "3"), ("J2", "5"), ("J2", "9")},
        {("J1", "1"), ("JP1", "1")},
        {("J2", "10"), ("JP1", "2")},
    ]
    assert {frozenset(g) for g in groups if len(g) > 1} == {
        frozenset(g) for g in expected_groups
    }, "Schematic does not implement the independently specified wiring"
    b = pcbnew.LoadBoard(str(ROOT / (NAME + ".kicad_pcb")))
    b.BuildConnectivity()
    connectivity = b.GetConnectivity()
    pads = {
        (fp.GetReference(), pad.GetNumber()): pad
        for fp in b.GetFootprints()
        for pad in fp.Pads()
    }
    assert len(pads) == 21 and ("J2", "7") not in pads
    for group in expected_groups:
        for endpoint in group:
            assert connected_pads(connectivity, pads[endpoint]) == group, endpoint
    isolated = {
        ("J1", "3"), ("J1", "7"), ("J1", "8"), ("J1", "9"), ("J1", "10"),
        ("J2", "1"), ("J2", "6"), ("J2", "8"),
    }
    for endpoint in isolated:
        assert connected_pads(connectivity, pads[endpoint]) == {endpoint}, endpoint
    assert b.GetCopperLayerCount() == 2
    assert abs(pcbnew.ToMM(b.GetDesignSettings().GetBoardThickness()) - 1.6) < 1e-6
    edges = [s for s in b.GetDrawings() if s.GetLayer() == pcbnew.Edge_Cuts]
    assert len(edges) == 4
    endpoints = [xy(s.GetStart()) for s in edges] + [xy(s.GetEnd()) for s in edges]
    assert max(p[0] for p in endpoints) - min(p[0] for p in endpoints) == 32
    assert max(p[1] for p in endpoints) - min(p[1] for p in endpoints) == 24
    assert all(endpoints.count(p) == 2 for p in set(endpoints)), "Outline is not closed"
    for pad in (p for (ref, _), p in pads.items() if ref == "J1"):
        assert abs(pcbnew.ToMM(pad.GetDrillSize().x) - 1.02) < 1e-6
    for number in ("1", "2", "3", "4", "5", "6", "8", "9", "10"):
        assert xy(pads["J2", number].GetSize()) == (0.74, 2.79)
    erc = json.loads((BUILD / "erc.json").read_text())
    drc = json.loads((BUILD / "drc.json").read_text())
    assert not [v for sheet in erc["sheets"] for v in sheet["violations"]]
    assert not drc["violations"] and not drc["unconnected_items"] and not drc["schematic_parity"]
    result = {
        "kicad_version": pcbnew.GetBuildVersion(),
        "erc_findings": 0,
        "drc_findings": 0,
        "unconnected_items": 0,
        "schematic_parity_findings": 0,
        "board_mm": [32, 24],
        "layers": 2,
        "thickness_mm": 1.6,
        "copper_connected_groups": [sorted(g) for g in expected_groups],
        "electrically_isolated_pads": sorted(isolated),
        "reset_link": "Open; CLONE_RST and TARGET_NRST copper-isolated",
        "fabrication_status": "Prototype files; physical fit and bench operation not tested",
    }
    write(BUILD / "validation.json", json.dumps(result, indent=2) + "\n")
    print("PASS: independent netlist and copper checks, power isolation, reset isolation, outline")


def package():
    gerbers = BUILD / "gerbers"
    gerbers.mkdir(exist_ok=True)
    run("pcb", "export", "gerbers", "--layers",
        "F.Cu,B.Cu,F.Mask,B.Mask,F.Silkscreen,B.Silkscreen,Edge.Cuts",
        "--subtract-soldermask", "--check-zones", "--output", str(gerbers) + "\\",
        ROOT / (NAME + ".kicad_pcb"))
    run("pcb", "export", "drill", "--format", "excellon", "--excellon-units", "mm",
        "--excellon-separate-th", "--generate-report",
        "--report-path", BUILD / "drill-report.txt", "--output", str(gerbers) + "\\",
        ROOT / (NAME + ".kicad_pcb"))
    required = {
        "F_Cu.gtl", "B_Cu.gbl", "F_Mask.gts", "B_Mask.gbs",
        "F_Silkscreen.gto", "B_Silkscreen.gbo", "Edge_Cuts.gm1",
        "PTH.drl", "NPTH.drl", "job.gbrjob",
    }
    assert {p.name for p in gerbers.iterdir()} == {NAME + "-" + s for s in required}
    drills = list(gerbers.glob("*.drl"))
    assert drills, "Missing Excellon drills"
    drill_text = "\n".join(p.read_text() for p in drills)
    assert "1.020" in drill_text and "0.300" in drill_text, "Missing connector or via drill tool"
    for path in gerbers.iterdir():
        if path.suffix in (".gbr", ".gtl", ".gbl", ".gts", ".gbs", ".gto", ".gbo", ".gm1"):
            assert path.stat().st_size > 100 and "M02*" in path.read_text()
    fab_zip = BUILD / (NAME + "-gerbers.zip")
    with zipfile.ZipFile(fab_zip, "w", zipfile.ZIP_DEFLATED) as archive:
        for path in sorted(gerbers.iterdir()):
            archive.write(path, path.name)
    with zipfile.ZipFile(fab_zip) as archive:
        assert archive.testzip() is None
    run("sch", "export", "pdf", "--output", BUILD / (NAME + "-schematic.pdf"),
        ROOT / (NAME + ".kicad_sch"))
    for side, layers_arg in (
        ("top", "F.Cu,F.Silkscreen,F.Fab,Edge.Cuts"),
        ("bottom", "B.Cu,B.Silkscreen,Edge.Cuts"),
    ):
        args = ["pcb", "export", "svg", "--layers", layers_arg,
                "--page-size-mode", "2", "--exclude-drawing-sheet", "--mode-single",
                "--output", BUILD / (NAME + "-" + side + ".svg")]
        if side == "bottom":
            args.append("--mirror")
        run(*args, ROOT / (NAME + ".kicad_pcb"))
    with (BUILD / "bom.csv").open("w", newline="", encoding="utf-8") as file:
        writer = csv.writer(file)
        writer.writerow(["Reference", "Quantity", "Manufacturer", "Part", "Description", "URL"])
        for ref in ("J1", "J2"):
            part = PARTS[ref]
            writer.writerow([ref, 1, "Samtec", part["value"], part["description"], part["datasheet"]])
        writer.writerow(["JP1", 0, "", "DO NOT BRIDGE", "Bare PCB solder link; leave open", ""])
        writer.writerow(["Cable", 1, "Samtec", "FFSD-05-D-04.00-01-N",
                         "Candidate 4 inch dual-ended 1.27 mm ribbon; verify key and 1-to-1 continuity",
                         "https://www.samtec.com/products/ffsd-05-d-04.00-01-n"])
    sources = [
        path for path in sorted(ROOT.rglob("*"))
        if path.is_file() and "build" not in path.relative_to(ROOT).parts
        and not {".history", ".git", "__pycache__"}.intersection(path.relative_to(ROOT).parts)
        and path.suffix not in (".kicad_prl", ".lck")
    ]
    hashes = {
        str(path.relative_to(ROOT)).replace("\\", "/"): hashlib.sha256(path.read_bytes()).hexdigest()
        for path in sources
    }
    hashes[fab_zip.name] = hashlib.sha256(fab_zip.read_bytes()).hexdigest()
    write(BUILD / "sha256.json", json.dumps(hashes, indent=2) + "\n")
    packet = BUILD / (NAME + "-order-package.zip")
    with zipfile.ZipFile(packet, "w", zipfile.ZIP_DEFLATED) as archive:
        for path in sources:
            archive.write(path, "source/" + str(path.relative_to(ROOT)).replace("\\", "/"))
        for path in (
            fab_zip, BUILD / "bom.csv", BUILD / "validation.json", BUILD / "sha256.json",
            BUILD / "erc.json", BUILD / "drc.json", BUILD / "netlist.xml",
            BUILD / "drill-report.txt", BUILD / (NAME + "-schematic.pdf"),
            BUILD / (NAME + "-top.svg"), BUILD / (NAME + "-bottom.svg"),
        ):
            archive.write(path, path.name)
    with zipfile.ZipFile(packet) as archive:
        assert archive.testzip() is None
    print(f"Fabrication ZIP: {fab_zip}")
    print(f"Order package: {packet}")


def main():
    if not pcbnew.GetBuildVersion().startswith("10."):
        raise RuntimeError("Use KiCad 10's bundled Python to generate these files")
    BUILD.mkdir(exist_ok=True)
    schematic()
    footprints()
    project = {
        "meta": {"filename": NAME + ".kicad_pro", "version": 1},
        "board": {"design_settings": {"rules": {
            "min_clearance": 0.2,
            "min_copper_edge_clearance": 0.5,
            "min_track_width": 0.25,
            "min_through_hole_diameter": 0.3,
            "min_via_diameter": 0.6,
            "min_via_annular_width": 0.15,
        }}},
    }
    write(ROOT / (NAME + ".kicad_pro"), json.dumps(project, indent=2) + "\n")
    run("sch", "export", "netlist", "--format", "kicadxml", "--output",
        BUILD / "netlist.xml", ROOT / (NAME + ".kicad_sch"))
    board(ET.parse(BUILD / "netlist.xml").getroot())
    validate()
    verify()
    package()
    print("Prototype order files generated; no purchase submitted")


if __name__ == "__main__":
    main()
