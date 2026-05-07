import math


from python.config.default_config import ERAS, FLAVOURS, FAKE_FLOOR
from python.utils.helper import fix_fake_and_bkg,compute_bin_Z_with_unc,pass_stat_and_err


def format_edges(edges):
    return ", ".join(f"{e:.0f}" for e in edges)

def fmt_bkg(val):
    if val < 0.5:
        return f"\033[91m{val:.3f}\033[0m"
    return f"{val:.3f}"

def normalize_results(raw):
    if isinstance(raw, dict):
        return raw

    # convert list -> dict
    out = {}
    for r in raw:
        flav = r["flav"]
        out.setdefault(flav, []).append(r)
    return out



def print_sr2_scan_table_from_results(result, data):

    import math
    import numpy as np

    # ----------------------------------------
    # Handle list input
    # ----------------------------------------
    if isinstance(result, list):
        for r in result:
            print_sr2_scan_table_from_results(r, data)
        return

    # ----------------------------------------
    # Enforce SR3-style schema
    # ----------------------------------------
    if "flav" not in result or "mass" not in result:
        raise KeyError("Result must contain 'flav' and 'mass'")

    if "regions" not in result:
        raise KeyError("Result must contain 'regions'")

    flav = result["flav"]
    mass = result["mass"]
    regions = result["regions"]

    print("\n========================================================")
    print(f" SCAN TABLE (SR2 Run2, per-era fake fix) | {flav} | mass={mass}")
    print("========================================================")

    header = (
        "Bin | Region | LT range        | "
        "   S      B      rel_unc     Z"
    )
    print(header)
    print("-" * len(header))

    idx = 0
    total_B = 0.0
    total_Z2 = 0.0

    # ======================================================
    # LOOP OVER REGIONS (low / high)
    # ======================================================
    for region, info in regions.items():

        edges = info["bins"]
        sub = data[region]

        edges_full = np.array(sub["edges"])
        bin_lo = edges_full[:-1]

        print("\n--------------------------------------------------------")
        print(f" REGION: {region.upper()}")
        print("--------------------------------------------------------")

        region_B = 0.0
        region_Z2 = 0.0

        # ----------------------------------------
        # LOOP MERGED BINS
        # ----------------------------------------
        for i in range(len(edges) - 1):

            lo = edges[i]
            hi = edges[i + 1]

            if i == len(edges) - 2:
                mask = (bin_lo >= lo) & (bin_lo <= hi)
            else:
                mask = (bin_lo >= lo) & (bin_lo < hi)

            S_tot = 0.0
            B_tot = 0.0
            E_tot = 0.0

            # ----------------------------------------
            # PER ERA: merge -> fake fix -> sum
            # ----------------------------------------
            for era in ERAS:

                S_arr = sub["signal_norm_flav"][flav][mass][era]
                B_arr = sub["background"][flav][era]
                F_arr = sub["fake"][flav][era]
                E_arr = sub["bkg_err2"][flav][era]

                s = float(S_arr[mask].sum())
                b = float(B_arr[mask].sum())
                f = float(F_arr[mask].sum())
                e = float(E_arr[mask].sum())

                f, b = fix_fake_and_bkg(
                    f, b, FAKE_FLOOR,
                    flavour=flav,
                    era=era
                )

                S_tot += s
                B_tot += b
                E_tot += e

            # ----------------------------------------
            # COMPUTE Z
            # ----------------------------------------
            if B_tot > 0:
                Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot)
                rel = math.sqrt(E_tot) / B_tot
            else:
                Z = 0.0
                rel = 0.0

            print("{:3d} | {:6s} | [{:5.2f},{:5.2f}] | {:7.3f} {:7.3f} {:6.3f} {:6.3f}".format(
                idx, region, lo, hi, S_tot, B_tot, rel, Z
            ))

            idx += 1
            region_B += B_tot
            region_Z2 += Z * Z

        region_Z = math.sqrt(region_Z2)

        print(f"\n>>> Region B = {region_B:.3f} | Region Z = {region_Z:.4f}")

        total_B += region_B
        total_Z2 += region_Z2

    # ----------------------------------------
    # TOTAL
    # ----------------------------------------
    total_Z = math.sqrt(total_Z2)

    print("\n========================================================")
    print(f" TOTAL B (Run2) = {total_B:.3f}")
    print(f" TOTAL Z (Run2) = {total_Z:.4f}")
    print("========================================================")
            
def debug_print_yields_integral_sr2(data):

    print("\n========================================================")
    print(" DEBUG YIELD DUMP (INTEGRALS) [SR2]")
    print("========================================================")

    for region in ["low", "high"]:

        sub = data[region]

        print("\n--------------------------------------------------------")
        print(f" REGION: {region}")
        print("--------------------------------------------------------")

        # ----------------------------------------
        # BACKGROUND
        # ----------------------------------------
        for flav in sub["background"]:
            for era in sub["background"][flav]:

                arr = sub["background"][flav][era]
                integral = arr.sum()

                print(f"Bkg   | {era:12s} | {flav:5s} | {region:10s} | {integral:10.4f}")

        # ----------------------------------------
        # FAKE
        # ----------------------------------------
        for flav in sub["fake"]:
            for era in sub["fake"][flav]:

                arr = sub["fake"][flav][era]
                integral = arr.sum()

                print(f"Fake  | {era:12s} | {flav:5s} | {region:10s} | {integral:10.4f}")

        # ----------------------------------------
        # SIGNAL
        # ----------------------------------------
        for flav in sub["signal_norm_flav"]:
            for mass in sub["signal_norm_flav"][flav]:
                for era in sub["signal_norm_flav"][flav][mass]:

                    arr = sub["signal_norm_flav"][flav][mass][era]

                    if arr is None:
                        integral = 0.0
                    else:
                        integral = arr.sum()

                    print(f"Sig{mass:<4} | {era:12s} | {flav:5s} | {region:10s} | {integral:10.4f}")

    # ----------------------------------------
    # GLOBAL CHECK (very useful)
    # ----------------------------------------
    print("\n--------------------------------------------------------")
    print(" GLOBAL TOTALS")
    print("--------------------------------------------------------")

    total_B = 0.0
    total_F = 0.0

    for region in ["low", "high"]:
        sub = data[region]

        for flav in sub["background"]:
            for era in sub["background"][flav]:
                total_B += sub["background"][flav][era].sum()

        for flav in sub["fake"]:
            for era in sub["fake"][flav]:
                total_F += sub["fake"][flav][era].sum()

    print(f"Total Background = {total_B:.4f}")
    print(f"Total Fake       = {total_F:.4f}")

def print_config_file(module, title="CONFIG"):
    import inspect

    print("\n====================================")
    print(f" {title}")
    print("====================================")

    try:
        source = inspect.getsource(module)
        print(source)
    except Exception as e:
        print(f"[WARNING] Could not read source: {e}")

    print("====================================\n")





    
    
def print_scan_binning_table(scan_results_for_plots):

    print("\n==============================")
    print(" SCAN BINNING TABLE")
    print("==============================")

    # ----------------------------------
    # Collect all (flav, mass)
    # ----------------------------------
    keys = set()

    for entry in scan_results_for_plots:
        raw = normalize_results(entry["raw"])
        for flav in raw:
            for r in raw[flav]:
                keys.add((flav, r["mass"]))

    keys = sorted(keys, key=lambda x: (x[0], float(x[1])))

    # ----------------------------------
    # Loop
    # ----------------------------------
    for flav, mass in keys:

        print("\n======================================")
        print(f"{flav}  Mass={mass}")
        print("======================================")

        for entry in scan_results_for_plots:

            label = entry["label"]
            raw = normalize_results(entry["raw"])

            for r in raw[flav]:
                if r["mass"] != mass:
                    continue

                print(f"\n[{label}]")

                # ------------------------
                # Run2 binning
                # ------------------------
                edges = r["binning"]["Run2"]
                print(f"Run2: {edges}")

                # ------------------------
                # Per-era binning + BKG
                # ------------------------
                for era, edges in r["binning"]["per_era"].items():

                    print(f"{era:10s}: {edges}")

                    # If stats exist, print B
                    if "bin_stats" in r:
                        bins = r["bin_stats"]["per_era"][era]

                        print("B = [" + ", ".join(fmt_bkg(b["B"]) for b in bins) + "]")


                break

def print_scan_binning_tableOLD(scan_results_for_plots):

    print("\n==============================")
    print(" SCAN BINNING TABLE")
    print("==============================")

    # ----------------------------------
    # Collect all (flav, mass)
    # ----------------------------------
    keys = set()

    for entry in scan_results_for_plots:
        raw = normalize_results(entry["raw"])
        for flav in raw:
            for r in raw[flav]:
                keys.add((flav, r["mass"]))

    keys = sorted(keys, key=lambda x: (x[0], float(x[1])))

    # ----------------------------------
    # Loop
    # ----------------------------------
    for flav, mass in keys:

        print("\n======================================")
        print(f"{flav}  Mass={mass}")
        print("======================================")

        for entry in scan_results_for_plots:

            label = entry["label"]
            raw = normalize_results(entry["raw"])

            for r in raw[flav]:
                if r["mass"] == mass:

                    print(f"\n[{label}]")

                    # Run2 binning
                    edges = r["binning"]["Run2"]
                    print(f"Run2: {edges}")

                    # Per-era binning
                    for era, edges in r["binning"]["per_era"].items():
                        print(f"{era:10s}: {edges}")

                    break
                



def print_sr2_scan_summary(results):

    print("\n==============================")
    print(" SR2 SCAN SUMMARY")
    print("==============================")

    # ----------------------------------------
    # flatten (handle nested lists)
    # ----------------------------------------
    flat = []
    for r in results:
        if isinstance(r, list):
            flat.extend(r)
        else:
            flat.append(r)

    # ----------------------------------------
    # group by flavour
    # ----------------------------------------
    results_by_flav = {}

    for r in flat:

        flavs = r.get("flavs", [])

        for flav in flavs:
            results_by_flav.setdefault(flav, []).append(r)

    # ----------------------------------------
    # print per flavour
    # ----------------------------------------
    for flav in sorted(results_by_flav.keys()):

        print(f"\n================ {flav} =================")
        print("Mass     Run2(Z)   Quad(Z)")
        print("----------------------------------------")

        subset = results_by_flav[flav]

        # expand masses (important for global scans)
        expanded = []

        for r in subset:
            masses = r.get("masses", [])
            for m in masses:
                expanded.append((m, r))

        # sort by mass
        expanded = sorted(expanded, key=lambda x: float(x[0]))

        for mass, r in expanded:

            print(f"{str(mass):6s}   "
                  f"{r['Z_run2']:8.4f}   {r['Z_quad']:8.4f}")



def print_sr2_scan_details(results, flav, mass):

    # ----------------------------------------
    # flatten results
    # ----------------------------------------
    flat = []
    for r in results:
        if isinstance(r, list):
            flat.extend(r)
        else:
            flat.append(r)

    found = False

    for r in flat:

        # ----------------------------------------
        # enforce SR3-style schema
        # ----------------------------------------
        if "flav" not in r or "mass" not in r or "regions" not in r:
            continue

        if r["flav"] != flav or r["mass"] != mass:
            continue

        found = True

        print("\n========================================")
        print(f"DETAILS | {flav} | mass={mass}")
        print("========================================")

        regions = r["regions"]

        # =========================================
        # LOOP REGIONS (low / high)
        # =========================================
        for region, info in regions.items():

            print(f"\n--- {region.upper()} ---")
            print(f"Bins: {info.get('bins', [])}")
            print(f"Z = {info.get('Z', 0.0):.4f}")

            # ----------------------------------------
            # bin-level info (from scan)
            # ----------------------------------------
            bin_info = info.get("bin_info", [])

            if not bin_info:
                print("  [No bin_info stored]")
                continue

            for b in bin_info:

                lo = b.get("lo", 0)
                hi = b.get("hi", 0)
                S  = b.get("S", 0.0)
                B  = b.get("B", 0.0)
                Z  = b.get("Z", 0.0)
                era = b.get("era", "Run2")

                print(
                    f"[{lo:5.2f},{hi:5.2f}]"
                    f"{era:>6s}  "
                    f"S={S:.3e} "
                    f"B={B:.3e} "
                    f"Z={Z:.3f}"
                )

                
    if not found:
        print(f"[WARNING] No result found for flav={flav}, mass={mass}")


        
def print_scan_summary_table(scan_results, use_quad=False):

    title = "QUAD" if use_quad else "Run2"

    print("\n==============================")
    print(f" SCAN SUMMARY TABLE ({title})")
    print("==============================")

    # ----------------------------------
    # Collect all labels
    # ----------------------------------
    labels = [entry["label"] for entry in scan_results]

    # ----------------------------------
    # Build lookup: (flav, mass) -> {label: Z}
    # ----------------------------------
    table = {}

    for entry in scan_results:
        label = entry["label"]
        raw = normalize_results(entry["raw"])

        for flav in raw:
            for r in raw[flav]:
                key = (flav, r["mass"])
                table.setdefault(key, {})

                val = r["quad"] if use_quad else r["run2"]
                table[key][label] = val

    # ----------------------------------
    # Sort keys
    # ----------------------------------
    keys = sorted(table.keys(), key=lambda x: (x[0], float(x[1])))

    # ----------------------------------
    # Header
    # ----------------------------------
    header = f"{'Flavour':8s} | {'Mass':6s}"
    for label in labels:
        header += f" | {label:25s}"
    print(header)
    print("-" * len(header))

    # ----------------------------------
    # Rows
    # ----------------------------------
    for flav, mass in keys:

        row = f"{flav:8s} | {str(mass):6s}"

        for label in labels:
            val = table[(flav, mass)].get(label, None)
            if val is None:
                row += f" | {'-':25s}"
            else:
                row += f" | {val:25.4f}"

        print(row)
        
def print_scan_summary_tableOLD(scan_results):

    print("\n==============================")
    print(" SCAN SUMMARY TABLE")
    print("==============================")

    for entry in scan_results:

        label = entry["label"]
        raw = normalize_results(entry["raw"])

        print(f"\n--- {label} ---")

        # ----------------------------------
        # NORMALIZE FORMAT
        # ----------------------------------
        if isinstance(raw, list):
            # convert list -> dict grouped by flavour
            tmp = {}
            for r in raw:
                flav = r["flav"]
                tmp.setdefault(flav, []).append(r)
            raw = tmp

        # ----------------------------------
        # LOOP SAFELY
        # ----------------------------------
        for flav in raw:
            for r in raw[flav]:
                print(f"{flav} {r['mass']} Z={r['run2']:.4f}")

        

def print_final_summary(results):

    print("\n==============================")
    print(" FINAL SUMMARY (WITH BINNING)")
    print("==============================")

    # Handle dict or list
    if isinstance(results, dict):
        iterable = []
        for flav in results:
            iterable.extend(results[flav])
    else:
        iterable = results

    for r in iterable:

        flav = r["flav"]
        mass = r["mass"]
        nbins = r["nbins"]
        quad = r["quad"]
        run2 = r["run2"]
        ratio = r["ratio"]

        print("\n----------------------------------------")
        print(f"{flav}  Mass={mass}  Nbins={nbins}")
        print("----------------------------------------")

        print(f"QUAD  = {quad:.4f}")
        print(f"Run2  = {run2:.4f}")
        print(f"Ratio = {ratio:.4f}")

        # ------------------------
        # Binning
        # ------------------------
        binning = r["binning"]

        if binning["Run2"] is not None:
            print("\n[Run2 binning]")
            print(binning["Run2"])

        # ------------------------
        # Bin stats (preferred)
        # ------------------------
        if "bin_stats" in r:

            print("\n[Per-era binning + BKG (from stats)]")

            for era, bins in r["bin_stats"]["per_era"].items():

                edges = r["binning"]["per_era"][era]

                B_vals = [round(b["B"], 3) for b in bins]
                rel_vals = [
                    round(math.sqrt(b["E"]) / b["B"], 3) if b["B"] > 0 else 0.0
                    for b in bins
                ]

                print(f"{era:10s} : {edges}")
                print(f"{'':10s}   B   = {B_vals}")
                print(f"{'':10s}   rel = {rel_vals}")



                
def fmt_ratio(r):
    if r < 0.9:
        return f"\033[91m{r:.4f}\033[0m"   # red                                                                                                                                    
    elif r < 0.98:
        return f"\033[93m{r:.4f}\033[0m"   # yellow                                                                                                                                 
    else:
        return f"{r:.4f}"
