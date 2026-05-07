import re
from collections import defaultdict

# containers
data_4 = defaultdict(lambda: defaultdict(float))  # (mass -> nbin -> sumZ)
data_6 = defaultdict(lambda: defaultdict(float))

pattern = re.compile(
    r"(\d+)\s+bin\s+(\d+)\s+\w+\s+MET\d+\s+(\S+)\s+Z=([0-9.]+)"
)

with open("input.txt") as f:
    for line in f:
        m = pattern.search(line)
        if not m:
            continue

        nbin   = int(m.group(1))
        mass   = int(m.group(2))
        region = m.group(3)
        z      = float(m.group(4))

        # classify region
        if "LTcut" in region or "GTcut" in region:
            data_4[mass][nbin] += z

        elif "HT_LT_MET2_bin" in region:
            data_6[mass][nbin] += z

# -------- printing --------

def pct_diff(a, b):
    if b == 0:
        return 0.0
    return 100.0 * (a - b) / b

for mass in sorted(set(list(data_4.keys()) + list(data_6.keys()))):

    print(f"Mass {mass}")

    for nbin in [3, 4, 5]:

        z4 = data_4[mass].get(nbin, 0.0)
        z6 = data_6[mass].get(nbin, 0.0)

        print(f"  {nbin} bin: SumZ(4r) = {z4:.4f}   SumZ(6r) = {z6:.4f}")

    # percent improvements (relative to 3-bin)
    z4_3 = data_4[mass].get(3, 0.0)
    z4_4 = data_4[mass].get(4, 0.0)
    z4_5 = data_4[mass].get(5, 0.0)

    z6_3 = data_6[mass].get(3, 0.0)
    z6_4 = data_6[mass].get(4, 0.0)
    z6_5 = data_6[mass].get(5, 0.0)

    print("  --- % change vs 3-bin ---")
    print(f"    4 bin: 4r = {pct_diff(z4_4, z4_3):+.2f}%   6r = {pct_diff(z6_4, z6_3):+.2f}%")
    print(f"    5 bin: 4r = {pct_diff(z4_5, z4_3):+.2f}%   6r = {pct_diff(z6_5, z6_3):+.2f}%")

    print()
