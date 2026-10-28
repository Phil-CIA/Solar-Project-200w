#!/usr/bin/env python3
import csv
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MASTER = ROOT / 'docs' / 'rev-one-bom-master.csv'
PROPOSED = ROOT / 'docs' / 'rev-one-bom-proposed-substitutions.csv'
BACKUP = ROOT / 'docs' / 'rev-one-bom-master.bak.csv'

def normalize_designator(s):
    return ','.join(sorted([d.strip() for d in s.split(',')])) if s else ''

if not MASTER.exists():
    raise SystemExit('Master BOM not found: ' + str(MASTER))
if not PROPOSED.exists():
    raise SystemExit('Proposed substitutions not found: ' + str(PROPOSED))

with PROPOSED.open(newline='', encoding='utf-8') as f:
    r = csv.DictReader(f)
    prop_rows = list(r)

# build map: normalized designator -> first candidate Cxxxxx
subs = {}
for row in prop_rows:
    cand = (row.get('Candidates') or '').strip()
    if not cand:
        continue
    # pick first candidate token (semicolon separated or space)
    first = cand.split(';')[0].strip()
    # sanitize (remove surrounding quotes)
    first = first.strip('"').strip("'")
    if not first.startswith('C'):
        # try to find Cnnnnn token inside string
        import re
        m = re.search(r'\b(C[0-9]{5,7})\b', first)
        if m:
            first = m.group(1)
        else:
            continue
    designator_field = row.get('Designator') or ''
    # some rows contain multiple designators like "R7, R10, R61"
    for d in designator_field.split(','):
        nd = d.strip()
        if nd:
            subs[nd] = first

# read master
with MASTER.open(newline='', encoding='utf-8') as f:
    r = csv.DictReader(f)
    master_rows = list(r)
    fieldnames = r.fieldnames

# backup original
with BACKUP.open('w', newline='', encoding='utf-8') as f:
    writer = csv.DictWriter(f, fieldnames=fieldnames)
    writer.writeheader()
    writer.writerows(master_rows)

changed = 0
for row in master_rows:
    designator = row.get('Designator','')
    # designator may be like "R7,R10,R61" or single
    for d in [x.strip() for x in designator.split(',') if x.strip()]:
        if d in subs:
            new_c = subs[d]
            # update LCSC Part # field (common header is 'LCSC Part #' per scripts)
            # find a matching header case-insensitively
            key = None
            for k in fieldnames:
                if k.lower().strip() == 'lcsc part #'.lower().strip() or k.lower().strip() == 'lcsc part #':
                    key = k
                    break
            if key is None:
                # fall back to exact 'LCSC Part #'
                key = 'LCSC Part #'
            old = row.get(key,'')
            if old != new_c:
                row[key] = new_c
                # add note column
                note_key = 'SubstitutionSource'
                if note_key not in fieldnames:
                    fieldnames = fieldnames + [note_key]
                row[note_key] = f'Applied from proposed substitutions ({PROPOSED.name})'
                changed += 1
            break

if changed == 0:
    print('No substitutions applied (no matching candidates or already applied).')
else:
    # write updated master
    with MASTER.open('w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(master_rows)
    print(f'Applied {changed} substitutions and updated {MASTER}. Backup at {BACKUP}.')
