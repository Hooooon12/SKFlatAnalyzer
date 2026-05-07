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



    
def print_sr3_scan_table_from_results(result, data):

    import math
    import numpy as np

    # ----------------------------------------
    # Handle list input
    # ----------------------------------------
    if isinstance(result, list):
        for r in result:
            print_sr3_scan_table_from_results(r, data)
        return

    flav = result["flav"]
    mass = result["mass"]
    met  = result["met"]

    print("\n========================================================")
    print(" SCAN TABLE (RUN2, per-era merged fake fix) | {} | mass={} | MET={}".format(flav, mass, met))
    print("========================================================")

    header = (
        "Bin | Jet     | MET   | LT range        | "
        "   S      B      rel_unc     Z"
    )
    print(header)
    print("-" * len(header))

    idx = 0
    total_B = 0.0
    total_S = 0.0
    total_Z2 = 0.0

    # ======================================================
    # LOOP OVER REGIONS
    # ======================================================
    for cat, info in result["regions"].items():

        edges = info["bins"]
        sub = data[met][cat]

        edges_full = np.array(sub["edges"])
        bin_lo = edges_full[:-1]

        print("\n--------------------------------------------------------")
        print(" REGION: {}".format(cat))
        print("--------------------------------------------------------")

        region_B = 0.0
        region_S = 0.0
        region_Z2 = 0.0

        jet = "LowJet" if "LowJet" in cat else "HighJet"
        met_label = "<" if "bin1" in cat else ">="

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
                S_arr = sub["signal_nonorm"][flav][mass][era]
                B_arr = sub["background"][flav][era]
                F_arr = sub["fake"][flav][era]
                E_arr = sub["bkg_err2"][flav][era]

                s = float(S_arr[mask].sum())
                b = float(B_arr[mask].sum())
                f = float(F_arr[mask].sum())
                e = float(E_arr[mask].sum())

                # Apply fake fix per era (CORRECT)
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

            print("{:3d} | {:7s} | {:3s}{:>2s} | [{:5.0f},{:5.0f}] | {:7.3f} {:7.3f} {:6.3f} {:6.3f}".format(
                idx, jet, met_label, str(met), lo, hi, S_tot, B_tot, rel, Z
            ))

            idx += 1
            region_B += B_tot
            region_S += S_tot
            region_Z2 += Z * Z

        region_Z = math.sqrt(region_Z2)

        print("\n>>> Region B = {:.3f} | Region Z = {:.4f}".format(region_B, region_Z))

        total_B += region_B
        total_S += region_S
        total_Z2 += region_Z2

    # ----------------------------------------
    # TOTAL
    # ----------------------------------------
    total_Z = math.sqrt(total_Z2)

    print("\n========================================================")
    print(" TOTAL B (Run2) = {:.3f}".format(total_B))
    print(" TOTAL S (Run2) = {:.3f}".format(total_S))
    
    print(" TOTAL Z (Run2) = {:.4f}".format(total_Z))
    print("========================================================")
    

def print_sr3_z_per_boundary(data, MASSES, FLAVOURS, ERAS):

    print("\n====================================================")
    print(" SR3 Z PER BOUNDARY (FIXED BINNING: [0,200,400,1200])")
    print("====================================================")

    colw = 12

    target_edges = [0, 200, 400, 1200]
    n_target_bins = len(target_edges) - 1

    CATEGORIES = [
        "LowJet_HT_LT_MET{X}_bin1",
        "LowJet_HT_LT_MET{X}_bin2",
        "LowJet_HT_LT_MET{X}_bin3",
        "HighJet_HT_LT_MET{X}_bin1",
        "HighJet_HT_LT_MET{X}_bin2",
        "HighJet_HT_LT_MET{X}_bin3",

    ]

    for flav in FLAVOURS:

        print("\n====================================================")
        print(f" FLAVOUR: {flav}")
        print("====================================================")

        for boundary in data:

            print("\n----------------------------------------------------")
            print(f" MET boundary = {boundary}")
            print("----------------------------------------------------")

            header = (
                f"{'Mass':>6} | "
                + "".join([f"{era:>{colw}}" for era in ERAS])
                + f" | {'QUAD':>{colw}} {'Run2':>{colw}}"
            )
            print(header)
            print("-" * len(header))

            for mass in MASSES:

                Z_era = {era: 0.0 for era in ERAS}
                total_quad_Z2 = 0.0

                # Run2 bins (fixed binning)
                run2_bins = [{"S": 0.0, "B": 0.0, "E": 0.0} for _ in range(n_target_bins)]

                # ----------------------------
                # LOOP ERA + CAT
                # ----------------------------
                for era in ERAS:

                    era_Z2 = 0.0

                    # per-era bins
                    era_bins = [{"S": 0.0, "B": 0.0, "E": 0.0} for _ in range(n_target_bins)]

                    for cat_template in CATEGORIES:

                        cat = cat_template.format(X=boundary)

                        if cat not in data[boundary]:
                            continue

                        sub = data[boundary][cat]

                        edges_full = sub["edges"]
                        S_arr = sub["signal_nonorm"][flav][mass][era]
                        B_arr = sub["background"][flav][era]
                        E_arr = sub["bkg_err2"][flav][era]

                        for i in range(len(edges_full) - 1):

                            lo = edges_full[i]
                            hi = edges_full[i + 1]

                            S = S_arr[i]
                            B = B_arr[i]
                            E = E_arr[i]

                            # find which target bin this belongs to
                            for j in range(n_target_bins):
                                t_lo = target_edges[j]
                                t_hi = target_edges[j + 1]

                                if lo >= t_lo and hi <= t_hi:
                                    era_bins[j]["S"] += S
                                    era_bins[j]["B"] += B
                                    era_bins[j]["E"] += E

                                    run2_bins[j]["S"] += S
                                    run2_bins[j]["B"] += B
                                    run2_bins[j]["E"] += E
                                    break

                    # compute era Z
                    for j in range(n_target_bins):
                        S = era_bins[j]["S"]
                        B = era_bins[j]["B"]
                        E = era_bins[j]["E"]

                        if B <= 0:
                            continue

                        Z = compute_bin_Z_with_unc(S, B, E)
                        era_Z2 += Z * Z

                    Z_era[era] = math.sqrt(era_Z2)
                    total_quad_Z2 += era_Z2

                # ----------------------------
                # RUN2
                # ----------------------------
                run2_Z2 = 0.0

                for j in range(n_target_bins):
                    S = run2_bins[j]["S"]
                    B = run2_bins[j]["B"]
                    E = run2_bins[j]["E"]

                    if B <= 0:
                        continue

                    Z = compute_bin_Z_with_unc(S, B, E)
                    run2_Z2 += Z * Z

                Z_quad = math.sqrt(total_quad_Z2)
                Z_run2 = math.sqrt(run2_Z2)

                line = (
                    f"{str(mass):>6} | "
                    + "".join([f"{Z_era[e]:{colw}.3f}" for e in ERAS])
                    + f" | {Z_quad:{colw}.3f} {Z_run2:{colw}.3f}"
                )

                print(line)




def print_sr3_z_summary_per_metcat_flat(data, MASSES, FLAVOURS, ERAS):

    import math

    colw = 10

    print("\n====================================================")
    print(" SR3 Z SUMMARY PER MET + CATEGORY (FLAT TABLE)")
    print("====================================================\n")

    # -------------------------------
    # HEADER
    # -------------------------------
    header = (
        f"{'Flav':>6} {'MET':>4} {'Category':>28} {'Mass':>6} | "
        + "".join([f"{era:>{colw}}" for era in ERAS])
        + f" | {'QUAD':>{colw}} {'Run2':>{colw}} || "
        + "".join([f"{era:>{colw}}" for era in ERAS])
        + f" | {'QUAD':>{colw}} {'Run2':>{colw}}"
    )

    print(header)
    print("-" * len(header))

    # ===============================
    # LOOP
    # ===============================
    for flav in FLAVOURS:
        for boundary in data:
            for cat in data[boundary]:

                for mass in MASSES:

                    sub = data[boundary][cat]

                    nbins = len(sub["edges"]) - 1

                    # -----------------------
                    # INIT
                    # -----------------------
                    Z2_era_fine = {era: 0.0 for era in ERAS}
                    Z2_era_1bin = {era: 0.0 for era in ERAS}

                    run2_bins = [{"S": 0.0, "B": 0.0, "E": 0.0} for _ in range(nbins)]

                    S_run2_1bin = 0.0
                    B_run2_1bin = 0.0
                    E_run2_1bin = 0.0

                    # -----------------------
                    # LOOP ERAS
                    # -----------------------
                    for era in ERAS:

                        S_arr = sub["signal_nonorm"][flav][mass][era]
                        B_arr = sub["background"][flav][era]
                        E_arr = sub["bkg_err2"][flav][era]

                        # ---- fine binning ----
                        for i in range(nbins):

                            s = S_arr[i]
                            b = B_arr[i]
                            e = E_arr[i]

                            if b <= 0:
                                continue

                            Z = compute_bin_Z_with_unc(s, b, e)
                            Z2_era_fine[era] += Z * Z

                            run2_bins[i]["S"] += s
                            run2_bins[i]["B"] += b
                            run2_bins[i]["E"] += e

                        # ---- 1-bin ----
                        s_tot = sum(S_arr)
                        b_tot = sum(B_arr)
                        e_tot = sum(E_arr)

                        if b_tot > 0:
                            Z = compute_bin_Z_with_unc(s_tot, b_tot, e_tot)
                            Z2_era_1bin[era] += Z * Z

                        S_run2_1bin += s_tot
                        B_run2_1bin += b_tot
                        E_run2_1bin += e_tot

                    # -----------------------
                    # FINAL CALCULATIONS
                    # -----------------------
                    Z_era_fine = {era: math.sqrt(Z2_era_fine[era]) for era in ERAS}
                    Z_quad_fine = math.sqrt(sum(Z2_era_fine.values()))

                    Z2_run2_fine = 0.0
                    for i in range(nbins):
                        S = run2_bins[i]["S"]
                        B = run2_bins[i]["B"]
                        E = run2_bins[i]["E"]

                        if B <= 0:
                            continue

                        Z = compute_bin_Z_with_unc(S, B, E)
                        Z2_run2_fine += Z * Z

                    Z_run2_fine = math.sqrt(Z2_run2_fine)

                    Z_era_1bin = {era: math.sqrt(Z2_era_1bin[era]) for era in ERAS}
                    Z_quad_1bin = math.sqrt(sum(Z2_era_1bin.values()))

                    if B_run2_1bin > 0:
                        Z_run2_1bin = compute_bin_Z_with_unc(
                            S_run2_1bin, B_run2_1bin, E_run2_1bin
                        )
                    else:
                        Z_run2_1bin = 0.0

                    # -----------------------
                    # PRINT
                    # -----------------------
                    line = (
                        f"{flav:>6} {boundary:>4} {cat:>28} {mass:>6} | "
                        + "".join([f"{Z_era_fine[e]:{colw}.3f}" for e in ERAS])
                        + f" | {Z_quad_fine:{colw}.3f} {Z_run2_fine:{colw}.3f} || "
                        + "".join([f"{Z_era_1bin[e]:{colw}.3f}" for e in ERAS])
                        + f" | {Z_quad_1bin:{colw}.3f} {Z_run2_1bin:{colw}.3f}"
                    )

                    print(line)
            
def debug_print_yields_integral(data):

    print("\n========================================================")
    print(" DEBUG YIELD DUMP (INTEGRALS)")
    print("========================================================")

    for met in sorted(data.keys()):

        for region in data[met]:

            sub = data[met][region]

            print("\n--------------------------------------------------------")
            print(f" REGION: {region} | MET={met}")
            print("--------------------------------------------------------")

            # ----------------------------------------
            # BACKGROUND
            # ----------------------------------------
            for flav in sub["background"]:
                for era in sub["background"][flav]:

                    arr = sub["background"][flav][era]
                    integral = arr.sum()

                    print(f"Bkg   | {era:12s} | {flav:5s} | {region:25s} | {integral:10.4f}")

            # ----------------------------------------
            # FAKE
            # ----------------------------------------
            for flav in sub["fake"]:
                for era in sub["fake"][flav]:

                    arr = sub["fake"][flav][era]
                    integral = arr.sum()

                    print(f"Fake  | {era:12s} | {flav:5s} | {region:25s} | {integral:10.4f}")

            # ----------------------------------------
            # SIGNAL
            # ----------------------------------------
            for flav in sub["signal_nonorm"]:
                for mass in sub["signal_nonorm"][flav]:
                    for era in sub["signal_nonorm"][flav][mass]:

                        arr = sub["signal_nonorm"][flav][mass][era]
                        integral = arr.sum()

                        print(f"Sig{mass:<4} | {era:12s} | {flav:5s} | {region:25s} | {integral:10.4f}")

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
                



def print_scan_summary(results):

    print("\n==============================")
    print(" SR3 SCAN SUMMARY (PER FLAV)")
    print("==============================")

    flavours = sorted(set(r["flav"] for r in results))

    for flav in flavours:

        print(f"\n================ {flav} =================")
        print("Mass     MET   Run2(Z)   Quad(Z)")
        print("----------------------------------------")

        subset = [r for r in results if r["flav"] == flav]
        subset = sorted(subset, key=lambda x: float(x["mass"]))

        for r in subset:

            print(f"{r['mass']:6s}   {r['met']:>3s}   "
                  f"{r['run2']:8.4f}   {r['quad']:8.4f}")

def print_scan_details(results, flav, mass):

    for r in results:
        if r["flav"] == flav and r["mass"] == mass:

            print("\n========================================")
            print(f"DETAILS | {flav} | mass={mass} | MET={r['met']}")
            print("========================================")

            for cat, info in r["regions"].items():

                print(f"\n--- {cat} ---")
                print(f"Bins: {info['bins']}")
                print(f"Z = {info['Z']:.4f}")

                for b in info["per_era"]["Run2"]:
                    print(f"[{b['lo']:4.0f},{b['hi']:4.0f}] "
                          f"S={b['S']:.3e} "
                          f"B={b['B']:.3e} "
                          f"Z={b['Z']:.3f}")
            
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
