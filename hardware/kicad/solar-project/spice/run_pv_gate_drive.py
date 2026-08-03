import os
import subprocess
import textwrap
from pathlib import Path

root = Path(__file__).resolve().parent
ngspice = r"C:\ProgramData\chocolatey\bin\ngspice.exe"

for vin in [12, 24, 36, 50]:
    for vctrl in [0, 3.3]:
        name = f"pv_gate_vin{vin}_vctrl{vctrl:.1f}".replace('.', '_')
        netlist = textwrap.dedent(f"""
        * PV switch gate drive, vin={vin}V, vctrl={vctrl}V
        Vpv pv_in 0 DC {vin}
        Vctrl ctrl_in 0 DC {vctrl}
        V3 ctrl3v3 0 DC 3.3

        R40 ctrl3v3 q2g 100k
        R41 ctrl_in q2g 10
        R43 pv_in q2d 100k
        R42 q2d u2gate 10

        M2 q2d q2g 0 0 NMOS2
        D1 u2gate u2src DZ
        Rsrc u2src 0 1Meg

        .model NMOS2 NMOS (VTO=1.6 KP=0.5M RD=0.1 RS=0.1)
        .model DZ D (IS=1e-15 BV=12 IBV=1e-3 CJO=20p VJ=0.7)

        .op
        .control
        run
        print "VIN=" {vin}
        print "VCTRL=" {vctrl}
        print "V(q2g)=" v(q2g)
        print "V(q2d)=" v(q2d)
        print "V(u2gate)=" v(u2gate)
        print "V(u2src)=" v(u2src)
        print "Vgs=" v(u2gate,u2src)
        .endc
        .end
        """).strip() + "\n"

        cir_path = root / f"{name}.cir"
        cir_path.write_text(netlist, encoding="utf-8")
        result = subprocess.run([ngspice, '-b', str(cir_path)], capture_output=True, text=True)
        print(f"=== {name} ===")
        print(result.stdout)
        if result.returncode != 0:
            print(result.stderr)
            raise SystemExit(result.returncode)
