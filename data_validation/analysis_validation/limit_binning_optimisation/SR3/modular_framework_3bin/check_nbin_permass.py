#!/usr/bin/env python3

import re
from collections import defaultdict

# structure:
# (flav, region) -> mass -> nbin -> Z
data = defaultdict(lambda: defaultdict(dict))

pattern = re.compile(
    r"(\d+)\s+bin\s+(\d+)\s+(\w+)\s+MET\d+\s+(\S+)\s+Z=([0-9.]+)"
)

with open("input.txt") as f:
    for line in f:
        m = pattern.search(line)
        if not m:
            continue

        nbin   = int(m.group(1))
        mass   = int(m.group(2))
        flav   = m.group(3)
        region = m.group(4)
        Z      = float(m.group(5))

        data[(flav, region)][mass][nbin] = Z

# print
for (flav, region) in sorted(data.keys()):

    masses = sorted(data[(flav, region)].keys())

    print("\n{} {}".format(flav, region))

    # header row
    print("mass -> ", end="")
    for m in masses:
        print("{:>8}".format(m), end="")
    print()

    # collect rows
    rows = {3: [], 4: [], 5: [], "43": [], "54": []}

    for m in masses:
        bins = data[(flav, region)][m]

        z3 = bins.get(3, 0.0)
        z4 = bins.get(4, 0.0)
        z5 = bins.get(5, 0.0)

        rows[3].append(z3)
        rows[4].append(z4)
        rows[5].append(z5)

        rows["43"].append(z4 / z3 if z3 > 0 else 0.0)
        rows["54"].append(z5 / z4 if z4 > 0 else 0.0)

    # print rows nicely aligned
    def print_row(label, values):
        print("{:<10}".format(label), end="")
        for v in values:
            print("{:>8.4f}".format(v), end="")
        print()

    print_row("3 bin Z", rows[3])
    print_row("4 bin Z", rows[4])
    print_row("4/3",     rows["43"])
    print_row("5 bin Z", rows[5])
    print_row("5/4",     rows["54"])
