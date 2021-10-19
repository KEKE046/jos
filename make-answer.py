with open('qemu.log', 'r') as f:
    lines = f.read().split('\n')

pgdir = set()
data = []
for line in lines:
    if not line.startswith('pgdir'):
        continue
    _, idx, va, pa, comment = line.split(' ')
    if idx in pgdir:
        continue
    else:
        pgdir.add(idx)
        data.append([idx, va, pa, comment])

import pandas as pd
print(pd.DataFrame(data, columns=['idx', 'va', 'pa', 'comment']).to_markdown(index=False))