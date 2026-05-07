#!/usr/bin/env python3

import math
import re
from collections import defaultdict

# storage: (flavour, region, nbin) -> list of Z
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
        mass   = int(m.group(2))   # not used except grouping implicitly
        flav   = m.group(3)
        region = m.group(4)
        Z      = float(m.group(5))

        key = (flav, region, nbin)
        data[key].append(Z)

# compute quadrature sums
results = defaultdict(dict)

for (flav, region, nbin), zlist in data.items():
    z_quad = math.sqrt(sum(z*z for z in zlist))
    results[(flav, region)][nbin] = z_quad

# print nicely
for (flav, region) in sorted(results.keys()):
    print("\n{} {}".format(flav, region))

    for nbin in sorted(results[(flav, region)]):
        z = results[(flav, region)][nbin]
        print("  {} bin Z_quad = {:.4f}".format(nbin, z))
