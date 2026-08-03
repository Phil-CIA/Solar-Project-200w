import re
import subprocess
import textwrap
from pathlib import Path


ROOT = Path(__file__).resolve().parent
NGSPICE = r"C:\ProgramData\chocolatey\bin\ngspice.exe"

VIN_CASES = [12.0, 24.0, 36.0, 50.0]
VCTRL_OFF = 0.0
VCTRL_ON = 3.3

# First-pass conduction corners for Q4 at ON state.
# Resistance values are chosen to approximate 1 A / 3 A / 5 A / 8 A load demand near 50 V.
LOAD_RESISTANCE_CASES = [50.0, 16.7, 10.0, 6.25]


def parse_value(text: str, expr: str) -> float:
    pattern = rf"{re.escape(expr)}\s*=\s*([-+0-9.eE]+)"
    m = re.search(pattern, text)
    if not m:
        raise ValueError(f"Missing expression in ngspice output: {expr}")
    return float(m.group(1))


def run_netlist(case_name: str, netlist_text: str) -> str:
    cir_path = ROOT / f"{case_name}.cir"
    log_path = ROOT / f"{case_name}.log"
    cir_path.write_text(netlist_text, encoding="utf-8")

    result = subprocess.run([NGSPICE, "-b", "-o", str(log_path), str(cir_path)], capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"ngspice failed for {case_name}:\n{result.stderr}")

    if not log_path.exists():
        raise RuntimeError(f"Expected log was not produced: {log_path}")

    return log_path.read_text(encoding="utf-8", errors="replace")


def build_voltage_stress_deck(vin: float, vctrl: float) -> str:
    return textwrap.dedent(
        f"""
        * Q4 path voltage stress sweep point
        V1 pv_hi 0 DC {vin}
        V4 vctrl 0 DC {vctrl}

        R40 0 q2g 100k
        R41 vctrl q2g 10

        R43 pv_hi q2d 100k
        R42 q2d q4g 10
        M2 q2d q2g 0 0 NMOS2

        Mpass pv_out q4g pv_hi pv_hi PMOSPASS
        D1 q4g pv_hi DZ

        * Light load for stable OFF/ON operating points.
        Rload pv_out 0 1k

        .model NMOS2 NMOS (VTO=1.6 KP=0.6M RD=0.1 RS=0.1)
        .model PMOSPASS PMOS (VTO=-2.5 KP=3M RD=0.08 RS=0.08)
        .model DZ D (IS=1e-15 BV=12 IBV=1e-3 CJO=100p VJ=0.7)

        .op
        .control
        run
        print v(q4g)
        print v(pv_hi)
        print v(pv_out)
        print v(q4g,pv_hi)
        print v(pv_hi,pv_out)
        print v(q2d)
        .endc
        .end
        """
    ).strip() + "\n"


def build_conduction_deck(vin: float, rload: float) -> str:
    return textwrap.dedent(
        f"""
        * Q4 ON-state conduction corner
        V1 pv_hi 0 DC {vin}
        V4 vctrl 0 DC {VCTRL_ON}

        R40 0 q2g 100k
        R41 vctrl q2g 10

        R43 pv_hi q2d 100k
        R42 q2d q4g 10
        M2 q2d q2g 0 0 NMOS2

        Mpass pv_out q4g pv_hi pv_hi PMOSPASS
        D1 q4g pv_hi DZ

        * Resistive load corner for stable conduction estimation.
        Rload pv_out 0 {rload}

        .model NMOS2 NMOS (VTO=1.6 KP=0.6M RD=0.1 RS=0.1)
        .model PMOSPASS PMOS (VTO=-2.5 KP=3M RD=0.08 RS=0.08)
        .model DZ D (IS=1e-15 BV=12 IBV=1e-3 CJO=100p VJ=0.7)

        .op
        .control
        run
        print v(q4g)
        print v(pv_hi)
        print v(pv_out)
        print v(q4g,pv_hi)
        print v(pv_hi,pv_out)
        .endc
        .end
        """
    ).strip() + "\n"


def main() -> None:
    stress_rows = []
    for vin in VIN_CASES:
        for vctrl in [VCTRL_OFF, VCTRL_ON]:
            case_name = f"pv_gate_stress_vin{vin:g}_vctrl{vctrl:.1f}".replace(".", "_")
            log_text = run_netlist(case_name, build_voltage_stress_deck(vin, vctrl))

            stress_rows.append(
                {
                    "vin": vin,
                    "vctrl": vctrl,
                    "vgs_q4": parse_value(log_text, "v(q4g,pv_hi)"),
                    "vds_q4": parse_value(log_text, "v(pv_hi,pv_out)"),
                    "vds_q2": parse_value(log_text, "v(q2d)"),
                    "vout": parse_value(log_text, "v(pv_out)"),
                }
            )

    conduction_rows = []
    for rload in LOAD_RESISTANCE_CASES:
        case_name = f"pv_gate_cond_vin50_r{rload:.2f}".replace(".", "_")
        log_text = run_netlist(case_name, build_conduction_deck(50.0, rload))

        vdrop = parse_value(log_text, "v(pv_hi,pv_out)")
        vout = parse_value(log_text, "v(pv_out)")
        iload = vout / rload
        conduction_rows.append(
            {
                "rload": rload,
                "iload": iload,
                "vgs_q4": parse_value(log_text, "v(q4g,pv_hi)"),
                "vdrop_q4": vdrop,
                "p_q4": vdrop * iload,
            }
        )

    lines = []
    lines.append("PV gate-drive stress-check summary")
    lines.append("================================")
    lines.append("Model: netlist-faithful Q4 PMOS path with Q2 pull-down and D1 clamp")
    lines.append("")

    lines.append("Voltage stress corners (Rload = 1k):")
    for row in stress_rows:
        state = "OFF" if row["vctrl"] == VCTRL_OFF else "ON"
        lines.append(
            f"- {state}, VIN={row['vin']:>2.0f} V -> Vgs(Q4)={row['vgs_q4']:.6f} V, "
            f"Vds(Q4)={row['vds_q4']:.6f} V, Vds(Q2)={row['vds_q2']:.6f} V, Vout={row['vout']:.6f} V"
        )

    lines.append("")
    lines.append("ON-state conduction corners at VIN=50 V (resistive loads):")
    for row in conduction_rows:
        lines.append(
            f"- Rload={row['rload']:.2f} ohm -> Iload~{row['iload']:.3f} A, Vgs(Q4)={row['vgs_q4']:.6f} V, "
            f"Vdrop(Q4)={row['vdrop_q4']:.6f} V, P(Q4)~{row['p_q4']:.6f} W"
        )

    lines.append("")
    lines.append("Interpretation notes:")
    lines.append("- Vds(Q2) in OFF state approximates the small-transistor voltage stress envelope.")
    lines.append("- P(Q4) values are model-based first-pass estimates and should be replaced with datasheet-level Rds(on) curves for final sizing.")

    out_path = ROOT / "pv_switch_gate_drive_stress_results.txt"
    out_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Wrote {out_path}")


if __name__ == "__main__":
    main()
