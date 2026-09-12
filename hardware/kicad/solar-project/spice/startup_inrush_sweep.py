from pathlib import Path
import csv

COUT = 1400e-6
VOUT = 6.49
VPV = 6.49
IPV_MAX = 20e-3
EFF = 0.80

ramps = [10e-3, 100e-3, 300e-3, 500e-3, 1.0]
p_available = VPV * IPV_MAX
p_out_available = p_available * EFF
energy_out = 0.5 * COUT * VOUT**2
energy_limited_time = energy_out / p_out_available

rows = []
for tss in ramps:
    iout = COUT * VOUT / tss
    pout = VOUT * iout
    iin = pout / (EFF * VPV)
    rows.append({
        "TSS_s": tss,
        "Iout_cap_A": iout,
        "Pout_W": pout,
        "Iin_required_A": iin,
        "PV_limited": iin > IPV_MAX,
    })

out = Path(__file__).with_name("startup_inrush_sweep.csv")
with out.open("w", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=rows[0].keys())
    writer.writeheader()
    writer.writerows(rows)

print(f"Output capacitor energy: {energy_out * 1e3:.2f} mJ")
print(f"Available PV power: {p_available * 1e3:.2f} mW")
print(f"Available output power at {EFF:.0%}: {p_out_available * 1e3:.2f} mW")
print(f"Energy-limited minimum charge time: {energy_limited_time * 1e3:.1f} ms")
print()
print("TSS (ms) | Icap (A) | Pout (W) | Iin required (A) | PV limit")
for row in rows:
    print(
        f"{row['TSS_s'] * 1e3:8.0f} | "
        f"{row['Iout_cap_A']:8.3f} | "
        f"{row['Pout_W']:8.3f} | "
        f"{row['Iin_required_A']:16.3f} | "
        f"{'YES' if row['PV_limited'] else 'NO'}"
    )
