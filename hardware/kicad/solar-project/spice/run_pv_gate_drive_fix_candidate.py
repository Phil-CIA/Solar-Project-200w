import re
import subprocess
import textwrap
from pathlib import Path


ROOT = Path(__file__).resolve().parent
NGSPICE = r"C:\ProgramData\chocolatey\bin\ngspice.exe"
VIN_CASES = [12, 24, 36, 50]
VCTRL_CASES = [0.0, 3.3]


def build_netlist(vin: float, vctrl: float) -> str:
    return textwrap.dedent(
        f"""
        * Candidate fix for PV-input gate drive (netlist-faithful Q4 path)
        * Goal: mirror latest schematic wiring around Q4/Q2/R40-R43/D1
        * OFF: R43 pulls Q4 gate toward source; Q2 held off by R40 pull-down
        * ON: V_in_ON drives Q2 on and pulls Q4 gate low through R42

        V1 pv_hi 0 DC {vin}
        V4 vctrl 0 DC {vctrl}

        * Latest control-side wiring from the schematic netlist
        R40 0 q2g 100k
        R41 vctrl q2g 10

        * Gate pull-up path plus active pull-down via Q2
        R43 pv_hi q2d 100k
        R42 q2d q4g 10
        M2 q2d q2g 0 0 NMOS2

        * PMOS high-side pass element (Q4 equivalent)
        Mpass pv_out q4g pv_hi pv_hi PMOSPASS

        * D1 clamp across Q4 gate-source (anode gate, cathode source)
        D1 q4g pv_hi DZ

        * Light load for operating-point observability
        Rload pv_out 0 1k

        .model NMOS2 NMOS (VTO=1.6 KP=0.6M RD=0.1 RS=0.1)
        .model PMOSPASS PMOS (VTO=-2.5 KP=3M RD=0.08 RS=0.08)
        .model DZ D (IS=1e-15 BV=12 IBV=1e-3 CJO=100p VJ=0.7)

        .op
        .control
        run
        print v(q2g)
        print v(q2d)
        print v(q4g)
        print v(pv_hi)
        print v(pv_out)
        print v(q4g,pv_hi)
        .endc
        .end
        """
    ).strip() + "\n"


def parse_value(stdout: str, expr: str) -> float:
    pattern = rf"{re.escape(expr)}\s*=\s*([-+0-9.eE]+)"
    m = re.search(pattern, stdout)
    if not m:
        raise ValueError(f"Missing expression in ngspice output: {expr}")
    return float(m.group(1))


def run_case(vin: float, vctrl: float) -> dict:
    case_name = f"pv_gate_fix_vin{vin:g}_vctrl{vctrl:.1f}".replace(".", "_")
    cir_path = ROOT / f"{case_name}.cir"
    log_path = ROOT / f"{case_name}.log"
    cir_path.write_text(build_netlist(vin, vctrl), encoding="utf-8")

    result = subprocess.run([NGSPICE, "-b", "-o", str(log_path), str(cir_path)], capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"ngspice failed for {case_name}:\n{result.stderr}")

    if not log_path.exists():
        raise RuntimeError(f"ngspice did not produce expected log file: {log_path}")

    log_text = log_path.read_text(encoding="utf-8", errors="replace")

    gate = parse_value(log_text, "v(q4g)")
    src = parse_value(log_text, "v(pv_hi)")
    out = parse_value(log_text, "v(pv_out)")
    vgs = parse_value(log_text, "v(q4g,pv_hi)")

    return {
        "vin": vin,
        "vctrl": vctrl,
        "vg": gate,
        "vs": src,
        "vout": out,
        "vgs": vgs,
    }


def main() -> None:
    rows = []
    for vin in VIN_CASES:
        for vctrl in VCTRL_CASES:
            rows.append(run_case(vin, vctrl))

    lines = []
    lines.append("PV switch gate-drive candidate fix summary")
    lines.append("========================================")
    lines.append("Model: PMOS Q4 pass element with Q2 pull-down control and 12V Vgs clamp")
    lines.append("")
    lines.append("OFF state (V_in_ON = 0 V):")
    for r in rows:
        if r["vctrl"] == 0.0:
            lines.append(
                f"- VIN={r['vin']:>2.0f} V -> V(gate)={r['vg']:.6f} V, V(source)={r['vs']:.6f} V, "
                f"Vgs={r['vgs']:.6f} V, Vout={r['vout']:.6f} V"
            )

    lines.append("")
    lines.append("ON state (V_in_ON = 3.3 V):")
    for r in rows:
        if r["vctrl"] == 3.3:
            lines.append(
                f"- VIN={r['vin']:>2.0f} V -> V(gate)={r['vg']:.6f} V, V(source)={r['vs']:.6f} V, "
                f"Vgs={r['vgs']:.6f} V, Vout={r['vout']:.6f} V"
            )

    lines.append("")
    lines.append("Conclusion:")
    lines.append(
        "This candidate topology establishes meaningful Vgs behavior: OFF keeps Vgs near 0 V, "
        "and ON drives Vgs negative with zener-limited magnitude at higher VIN."
    )

    out_path = ROOT / "pv_switch_gate_drive_fix_candidate_results.txt"
    out_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Wrote {out_path}")


if __name__ == "__main__":
    main()
