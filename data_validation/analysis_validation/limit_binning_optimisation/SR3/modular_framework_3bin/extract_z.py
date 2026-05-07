import re
from collections import defaultdict

data = defaultdict(list)

pattern = re.compile(
    r"(\d+)\s+bin\s+(\d+)\s+(\w+)\s+MET\d+\s+(\S+)\s+Z=([0-9.]+)"
)

with open("input.txt") as f:
    for line in f:
        m = pattern.search(line)
        if not m:
            continue

        nbin   = int(m.group(1))
        mass   = m.group(2)
        flav   = m.group(3)
        region = m.group(4)
        z      = float(m.group(5))

        key = (mass, flav, region)
        data[key].append((nbin, z))

# ---- print sorted ----

for key in sorted(data, key=lambda x: (int(x[0]), x[1], x[2])):
    mass, flav, region = key

    print(f"{mass} {flav} {region}")

    entries = sorted(data[key], key=lambda x: x[1], reverse=True)

    for nbin, z in entries:
        print(f"  {nbin} bin Z={z:.4f}")

    print()
