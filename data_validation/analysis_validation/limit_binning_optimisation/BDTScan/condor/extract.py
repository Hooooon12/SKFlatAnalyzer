#!/usr/bin/env python3

import os
import numpy as np
from glob import glob

# ---------------------------------------
# Helper: extract FOM + edges robustly
# ---------------------------------------
def get_best_fom_and_edges(d):

    # --- FOM priority ---
    fom = (
        d.get("fom_best")
        or d.get("fom_greedy")
        or d.get("fom_snapped")
        or d.get("fom_dp")
        or d.get("fom_coarse")
        or d.get("fom")
    )

    # --- edges priority ---
    edges = (
        d.get("edges_best")
        or d.get("edges_greedy")
        or d.get("edges_snapped")
        or d.get("edges_dp")
        or d.get("edges_coarse")
        or d.get("edges")
        or []
    )

    return fom, edges


# ---------------------------------------
# Collect all result files
# ---------------------------------------
all_files = []

for d in [
    "results_coarse",
    "results_greedy",
    "results_dp",
    "results_twostep",
]:
    if os.path.exists(d):
        all_files += glob(f"{d}/*.npy")

if not all_files:
    print("[ERROR] No result files found")
    exit(1)


# ---------------------------------------
# Group by (mass, flavour)
# ---------------------------------------
results = {}

for fpath in all_files:

    try:
        data = np.load(fpath, allow_pickle=True).item()
    except Exception as e:
        print(f"[WARNING] Failed to load {fpath}: {e}")
        continue

    mass = str(data.get("mass", "UNKNOWN"))
    flav = str(data.get("flavour", "UNKNOWN"))

    key = (mass, flav)

    fom, edges = get_best_fom_and_edges(data)

    if fom is None:
        print(f"[WARNING] No usable FOM in {fpath}")
        continue

    if edges is None or len(edges) == 0:
        print(f"[WARNING] Empty edges in {fpath}")
        continue

    # store best per (mass, flavour)
    if key not in results or fom > results[key]["fom"]:
        results[key] = {
            "fom": fom,
            "edges": edges,
            "file": fpath,
            "job": data.get("job_id", "?"),
            "existing": data.get("fom_existing", None),
            "method": data.get("scan_type", "unknown"),
        }


# ---------------------------------------
# Print results
# ---------------------------------------
print("\n=== BEST RESULTS PER (mass, flavour) ===\n")

for (mass, flav), r in sorted(results.items()):

    print(f"{flav} M{mass}")
    print(f"  FOM            = {r['fom']:.5f}")

    if r["existing"] is not None:
        print(f"  Existing FOM   = {r['existing']:.5f}")

    print(f"  edges          = {[round(x,3) for x in r['edges']]}")
    print(f"  job            = {r['job']}")
    print(f"  method         = {r['method']}")
    print(f"  file           = {r['file']}")
    print("")
