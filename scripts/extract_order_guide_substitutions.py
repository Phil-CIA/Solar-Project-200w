import re
import csv
from pathlib import Path

SRC = Path('docs/rev-one-lcsc-order-guide.md')
OUT = Path('docs/rev-one-bom-proposed-substitutions.csv')

if not SRC.exists():
    print('Source not found:', SRC)
    raise SystemExit(1)

rows = []
with SRC.open(encoding='utf-8') as f:
    for line in f:
        line = line.strip()
        # match table rows starting with | `Designator` |
        m = re.match(r'^\|\s*`([^`]+)`\s*\|\s*([^|]+)\|\s*`([^`]+)`\s*\|\s*([^|]+)\|\s*([^|]+)\|', line)
        if m:
            designator = m.group(1).strip()
            value = m.group(2).strip()
            current_lcsc = m.group(3).strip()
            status = m.group(4).strip()
            recommended = m.group(5).strip()
            # find any Cnnnnnn codes in recommended
            candidates = re.findall(r'`(C[0-9]+)`', recommended)
            # also find code-like tokens C###### without backticks
            more = re.findall(r'\b(C[0-9]{5,7})\b', recommended)
            for c in more:
                if c not in candidates:
                    candidates.append(c)
            rows.append({'Designator':designator, 'Value':value, 'Current_LCSC':current_lcsc, 'Candidates': ';'.join(candidates), 'RecommendedText': recommended})

with OUT.open('w', newline='', encoding='utf-8') as f:
    if rows:
        writer = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)
    else:
        writer = csv.DictWriter(f, fieldnames=['Designator','Value','Current_LCSC','Candidates','RecommendedText'])
        writer.writeheader()

print(f'Wrote: {OUT} ({len(rows)} rows)')
