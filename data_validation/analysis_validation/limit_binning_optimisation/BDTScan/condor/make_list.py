import numpy as np
import glob

# ---------------------------------------
# Collect best results
# ---------------------------------------
best = {}

files = glob.glob("results_*/*.npy")

for f in files:

    d = np.load(f, allow_pickle=True).item()

    key = (d["mass"], d["flavour"])

    if key not in best or d["fom_best"] > best[key]["fom_best"]:
        best[key] = d

# ---------------------------------------
# Print grouped output
# ---------------------------------------
for flav in ["MuMu", "EE", "EMu"]:

    print(f"\n// ===== {flav} =====")

    for (mass, f) in sorted(best):

        if f != flav:
            continue

        d = best[(mass, f)]

        edges = d["edges_best"]

        # remove -0.4 and replace last edge with 1.0
        edges_out = edges[1:-1] + [1.0]

        edge_str = ", ".join(f"{x:.3f}" for x in edges_out)

        print(f'map_bdt_limit_bins["SR3BDT_{flav}_{mass}_V3_Strict_15_Bin"] = {{{edge_str}}};')
