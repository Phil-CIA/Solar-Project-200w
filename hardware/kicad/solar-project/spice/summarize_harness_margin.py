from pathlib import Path
import re

log_path = Path(__file__).with_name("90_system_integration_harness_ltspice.log")
text = log_path.read_bytes().decode("utf-8", errors="replace")
lines = text.splitlines()

step_lines = []
for line in lines:
    if line.startswith(".step "):
        step_lines.append(line[6:].strip())

measurements = {}
current = None
for line in lines:
    if line.startswith("Measurement:"):
        current = line.split(":", 1)[1].strip()
        measurements[current] = []
    elif current is not None:
        if re.match(r"^\s*\d+", line):
            tokens = line.split()
            if len(tokens) >= 2:
                try:
                    measurements[current].append(float(tokens[1]))
                except ValueError:
                    pass
        elif line.startswith("Date:") or line.startswith("Total elapsed"):
            current = None

if not measurements:
    raise SystemExit(f"No measurement blocks found in {log_path}")

if len(step_lines) != len(measurements.get("vout_final", [])):
    raise SystemExit(
        f"Step count mismatch: {len(step_lines)} step lines but {len(measurements.get('vout_final', []))} measurement rows"
    )

print("LTspice harness margin summary")
print("-" * 80)
print(f"Log: {log_path.name}")
print(f"Steps: {len(step_lines)}")
print()

for idx, step in enumerate(step_lines, start=1):
    vout = measurements["vout_final"][idx - 1]
    bat = measurements["bat_final"][idx - 1]
    ichg = measurements["ichg_final"][idx - 1]
    print(f"{idx:>2}. {step}")
    print(f"    VOUT={vout:8.4f} V   BAT={bat:8.4f} V   ICHG={ichg:8.4f} A")
    print(f"    Drop(VOUT-BAT)={vout - bat:6.4f} V")

print()
print("Worst-case values")
print("-" * 80)
print(f"Min VOUT: {min(measurements['vout_final']):.4f} V")
print(f"Min BAT:  {min(measurements['bat_final']):.4f} V")
print(f"Max drop: {max(m2 - m1 for m1, m2 in zip(measurements['bat_final'], measurements['vout_final'])):.4f} V")
print(f"Max charge current magnitude: {max(abs(v) for v in measurements['ichg_final']):.4f} A")
print()
print("Interpretation")
print("-" * 80)
print("This is still a simplified sanity-check view, not a hardware-validated proof.")
print("The model shows the hottest/most lossy corner reduces output headroom noticeably.")
