import subprocess
from pathlib import Path

root = Path(__file__).resolve().parent
ngspice = r"C:\ProgramData\chocolatey\bin\ngspice.exe"
files = sorted(root.glob("pv_gate_vin*_vctrl*.cir"))
for path in files:
    print(f"=== {path.name} ===")
    result = subprocess.run([ngspice, '-b', str(path)], capture_output=True, text=True)
    print(f"returncode={result.returncode}")
    if result.stdout:
        print(result.stdout)
    if result.stderr:
        print(result.stderr)
