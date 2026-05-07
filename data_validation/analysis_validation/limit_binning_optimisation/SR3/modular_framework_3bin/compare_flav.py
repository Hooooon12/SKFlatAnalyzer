#!/usr/bin/env python3

import re
import sys
import math
from collections import defaultdict

if len(sys.argv) < 2:
    print("Usage: python script.py input.txt")
    sys.exit(1)

filename = sys.argv[1]

# Storage: (mass, flavour) -> nbin -> sum(Z^2)
data_4 = defaultdict(lambda: defaultdict(float))
data_6 = defaultdict(lambda: defaultdict(float))

regions_4 = [
    "LowJet_LT_MET2_LTcut",
    "HighJet_LT_MET2_LTcut",
    "LowJet_LT_MET2_GTcut",
    "HighJet_LT_MET2_GTcut",
]

regions_6 = [
    "LowJet_HT_LT_MET2_bin1",
    "LowJet_HT_LT_MET2_bin2",
    "LowJet_HT_LT_MET2_bin3",
    "HighJet_HT_LT_MET2_bin1",
    "HighJet_HT_LT_MET2_bin2",
    "HighJet_HT_LT_MET2_bin3",
]

pattern = re.compile(
    r"(\d+)\s+bin\s+(\d+)\s+(\w+)\s+MET\d+\s+(\S+)\s+Z=([0-9.]+)"
)

with open(filename) as f:
    for line in f:
        m = pattern.search(line)
        if not m:
            continue

        nbin   = int(m.group(1))
        mass   = int(m.group(2))
        flav   = m.group(3)
        region = m.group(4)
        z      = float(m.group(5))

        key = (mass, flav)

        if region in regions_4:
            data_4[key][nbin] += z * z

        if region in regions_6:
            data_6[key][nbin] += z * z


def percent_diff(new, old):
    if old == 0:
        return 0.0
    return 100.0 * (new - old) / old


# ---- Print ----

for (mass, flav) in sorted(data_4.keys()):

    print("====================================================")
    print(f"Mass {mass}  Flavour {flav}")
    print("====================================================")

    results_4 = {}
    results_6 = {}

    for nbin in [3, 4, 5]:

        z4 = math.sqrt(data_4[(mass, flav)].get(nbin, 0.0))
        z6 = math.sqrt(data_6[(mass, flav)].get(nbin, 0.0))

        results_4[nbin] = z4
        results_6[nbin] = z6

        ratio = z6 / z4 if z4 > 0 else 0.0

        print(
            f"{nbin} bin: "
            f"SumZ(4r) = {z4:.4f}   "
            f"SumZ(6r) = {z6:.4f}   "
            f"6/4 r = {ratio:.3f}"
        )

    # ---- improvements ----
    print("\nImprovements:")

    for label, results in [("4-region", results_4), ("6-region", results_6)]:

        d43 = percent_diff(results[4], results[3])
        d54 = percent_diff(results[5], results[4])
        d53 = percent_diff(results[5], results[3])

        print(
            f"{label}: "
            f"4 vs 3 = {d43:+.2f}%   "
            f"5 vs 4 = {d54:+.2f}%   "
            f"5 vs 3 = {d53:+.2f}%"
        )

    # ---- best bins ----
    best_4 = max(results_4, key=results_4.get)
    best_6 = max(results_6, key=results_6.get)

    print("\nBest bin choice:")
    print(f"4-region best = {best_4} bin (Z = {results_4[best_4]:.4f})")
    print(f"6-region best = {best_6} bin (Z = {results_6[best_6]:.4f})")

    print()
