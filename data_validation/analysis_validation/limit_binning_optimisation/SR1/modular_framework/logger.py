import math

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
        print (label)
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

        

def print_final_summary(results, tab):

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
        print(f"{flav}  Mass={mass}  Nbins={nbins} {tab}")
        print("----------------------------------------")

        print(f"QUAD  = {quad:.4f} {tab}")
        print(f"Run2  = {run2:.4f} {tab}")
        print(f"Ratio = {ratio:.4f}")

        # ------------------------
        # Binning
        # ------------------------
        binning = r["binning"]

        if binning["Run2"] is not None:
            print("\n[Run2 binning] {tab}")
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
