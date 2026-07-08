# PV Input Stage (Chosen First-Pass Parts)

Last updated: 2026-07-07
Owners: Phil + Copilot
Purpose: define the first schematic symbols to place at the PV input boundary for the series-panel buck MPPT baseline.

## 1. Selected First-Pass Parts

| Function | Selected Part Class | Notes |
|---|---|---|
| PV input connector | 2-position pluggable terminal block, 5.08 mm pitch, 300 V / 16 A class | Use a generic KiCad terminal block symbol and an equivalent footprint from the library set. This is the board-side entry point for the panel harness. |
| Input fuse | 10 A slow-blow fuse in holder or inline fuse placeholder | Current is only about 5.3 A at STC, so this gives headroom while still protecting the harness and input stage. |
| Reverse polarity protection | Series Schottky rectifier, 100 V / 10 A class | Chosen for simplicity in the first schematic pass. It is acceptable for a learning prototype even though it adds some loss. |
| Transient suppression | 64 V TVS diode, SMBJ/SMCJ class | Keep the standoff above normal PV operating voltage while providing surge suppression. |
| Input filtering | 2 x 10 uF / 100 V X7R plus 100 nF / 100 V ceramic | Place close to the connector / protection boundary. |

## 2. Placement Order On Sheet 1

1. J1 - PV input connector
2. F1 - 10 A fuse
3. D1 - series Schottky reverse-polarity diode
4. D2 - TVS suppression diode
5. C1/C2/C3 - input capacitor bank
6. Net labels for PV_IN_POS and PV_IN_NEG

## 3. Why This Stack

- The series PV baseline keeps the input voltage above the 4S charge voltage, so a simple protected input front end is sufficient for the first schematic pass.
- The series diode is easier to understand and place than an ideal-diode MOSFET block.
- The TVS and fuse keep the boundary explicit before the buck power stage is added.

## 4. Layout Notes

- Keep the connector, fuse, diode, TVS, and input caps physically close together.
- Route the PV current path as a short loop with obvious forward and return legs.
- Keep the protection devices at the edge of the sheet so their ownership is visually obvious.

## 5. Follow-On Improvement Path

If efficiency or thermal loss becomes a concern, replace the series Schottky with an ideal-diode MOSFET block in a later revision. The schematic boundary can stay the same.

Tracking rule:
- This is tracked as DEC-012 / Q-003 and must be resolved before PCB trace routing begins.
