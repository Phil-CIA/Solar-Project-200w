import os
import pyvisa

resource = 'USB0::0x1AB1::0x04CE::DS1ZA172517961::INSTR'
out_path = os.path.join('.', 'rigol_scope_capture_2026-09-07.txt')

lines = []
lines.append('RESOURCE=' + resource)

rm = pyvisa.ResourceManager()
inst = rm.open_resource(resource)
inst.timeout = 20000
inst.write_termination = '\n'
inst.read_termination = '\n'


def q(cmd: str) -> str:
    try:
        return inst.query(cmd).strip()
    except Exception as e:
        return f'ERROR:{type(e).__name__}:{e}'


for cmd in [
    '*IDN?',
    '*CLS',
    '*OPC?',
    ':SYSTEM:ERR?',
    ':ACQ:MODE?',
    ':TIM:SCAL?',
    ':TIM:DEL?',
    ':CHAN1:SCAL?',
    ':CHAN1:OFFS?',
    ':CHAN1:COUP?',
    ':MEAS:VPP? CHAN1',
    ':MEAS:VRMS? CHAN1',
    ':MEAS:FREQ? CHAN1',
    ':WAV:FORM?',
    ':WAV:POIN?',
    ':WAV:PRE?',
]:
    lines.append(f'{cmd}={q(cmd)}')

for source in ['CHAN1', 'CHAN2', 'CHAN3', 'CHAN4']:
    for form in ['ASCII', 'BYTE', 'WORD']:
        try:
            inst.write(f':WAV:SOUR {source}')
            inst.write(f':WAV:FORM {form}')
            inst.write(':WAV:POIN:MODE NORM')
            val = inst.query(':WAV:DATA?')
            lines.append(f'{source}_{form}_WAVE={str(val)[:2000]}')
        except Exception as e:
            lines.append(f'{source}_{form}_WAVE=ERROR:{type(e).__name__}:{e}')

inst.close()

with open(out_path, 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines) + '\n')

print(out_path)
for line in lines[:25]:
    print(line)
