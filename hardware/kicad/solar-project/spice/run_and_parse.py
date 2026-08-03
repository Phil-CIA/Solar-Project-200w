import os
import subprocess
import re
from pathlib import Path

root = Path(__file__).resolve().parent
ngspice = r"C:\ProgramData\chocolatey\bin\ngspice.exe"

for vin in [12, 24, 36, 50]:
    for vctrl in [0, 3.3]:
        name = f"pv_gate_vin{vin}_vctrl{vctrl:.1f}".replace('.', '_')
        cir = root / f"{name}.cir"
        if not cir.exists():
            cir.write_text(f"""* auto-generated\nVpv pv_in 0 DC {vin}\nVctrl ctrl_in 0 DC {vctrl}\nV3 ctrl3v3 0 DC 3.3\nR40 ctrl3v3 q2g 100k\nR41 ctrl_in q2g 10\nR43 pv_in q2d 100k\nR42 q2d u2gate 10\nM2 q2d q2g 0 0 NMOS2\nRsrc u2gate 0 1Meg\n.model NMOS2 NMOS (VTO=1.6 KP=0.5M RD=0.1 RS=0.1)\n.op\n.end\n""", encoding='utf-8')
        out = root / f"{name}.raw"
        result = subprocess.run([ngspice, '-b', '-r', str(out), str(cir)], capture_output=True, text=True)
        if result.returncode != 0:
            print(name, 'FAILED', result.stderr)
            continue
        print(f"=== {name} ===")
        if out.exists():
            data = out.read_bytes()
            print(data[:200])
            print('raw-bytes', len(data))
