import math
import os,sys
from python.config.default_config import ERAS, FLAVOURS, FAKE_FLOOR

Bin_NBKG_REQ = 1.0
Bin_NBKG_REQ_Tight = 1.
Bin_BKG_RelUnc = 0.5

import numpy as np
import math

from tqdm import tqdm


DEBUG = False

def build_mass_weights_from_ref(ref_results, mode="run2"):

    weights = {}

    # handle both flat list and dict-of-lists
    if isinstance(ref_results, dict):
        all_results = []
        for flav in ref_results:
            all_results.extend(ref_results[flav])
    else:
        all_results = ref_results

    for r in all_results:
        mass = r["mass"]
        Z = r.get(mode, 0.0)

        if Z > 0:
            weights[mass] = 1.0 / (Z * Z)
        else:
            weights[mass] = 1.0

    return weights


def compute_sr2_z_for_binning_global(data, bins_dict, flav, mass):

    import numpy as np
    import math

    total_Z2 = 0.0

    for region in ["low", "high"]:

        sub = data[region]

        edges = bins_dict[region]

        edges_full = np.array(sub["edges"])
        bin_lo = edges_full[:-1]

        region_Z2 = 0.0

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

            for era in ERAS:

                S_arr = sub["signal_norm_flav"][flav][mass][era]
                B_arr = sub["background"][flav][era]
                F_arr = sub["fake"][flav][era]
                E_arr = sub["bkg_err2"][flav][era]

                s = S_arr[mask].sum()
                b = B_arr[mask].sum()
                f = F_arr[mask].sum()
                e = E_arr[mask].sum()

                f, b = fix_fake_and_bkg(f, b, FAKE_FLOOR,
                                        flavour=flav, era=era)

                S_tot += s
                B_tot += b
                E_tot += e

            if B_tot > 0:
                Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot)
            else:
                Z = 0.0

            region_Z2 += Z * Z

        total_Z2 += region_Z2

    return math.sqrt(total_Z2)


def recompute_per_mass_with_fixed_binning_sr2(data, scan_results):

    import math
    import numpy as np

    output = []

    # ----------------------------------------
    # FLATTEN INPUT
    # ----------------------------------------
    flat = []
    for item in scan_results:
        if isinstance(item, list):
            flat.extend(item)
        else:
            flat.append(item)

    # ----------------------------------------
    # LOOP OVER RESULTS
    # ----------------------------------------
    for res in flat:

        if res is None:
            continue

        flavs  = res.get("flavs", [])
        masses = res.get("masses", [])
        regions = res.get("best_regions", {})

        if not regions:
            continue

        for flav in flavs:
            for mass in masses:

                total_Z2 = 0.0

                new_result = {
                    "flav": flav,
                    "mass": mass,
                    "regions": {}
                }

                # =========================================
                # LOOP REGIONS
                # =========================================
                for region, info in regions.items():

                    edges = info["bins"]

                    sub = data[region]

                    edges_full = np.array(sub["edges"])
                    bin_lo = edges_full[:-1]

                    region_Z2 = 0.0

                    for i in range(len(edges) - 1):

                        lo = edges[i]
                        hi = edges[i + 1]

                        if i == len(edges) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        s_tot = 0.0
                        b_tot = 0.0
                        e_tot = 0.0

                        for era in ERAS:

                            if flav not in sub["signal_norm_flav"]:
                                continue
                            if mass not in sub["signal_norm_flav"][flav]:
                                continue
                            if era not in sub["signal_norm_flav"][flav][mass]:
                                continue

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

                            s_tot += s
                            b_tot += b
                            e_tot += e

                        if b_tot > 0:
                            z = compute_bin_Z_with_unc(s_tot, b_tot, e_tot)
                        else:
                            z = 0.0

                        region_Z2 += z * z

                    total_Z2 += region_Z2

                    new_result["regions"][region] = {
                        "bins": edges
                    }

                new_result["run2"] = math.sqrt(total_Z2)
                new_result["quad"] = new_result["run2"]

                output.append(new_result)

    return output



def print_bkg_per_bin_sr2(data, scan_outputs, config):

    import numpy as np
    import math

    print("\n==============================")
    print(" BKG PER BIN DEBUG (SR2)")
    print("==============================")

    opt_mode = config.get("opt_mode", "Run2")

    # ----------------------------------------
    # flatten outputs
    # ----------------------------------------
    flat = []
    for item in scan_outputs:
        if isinstance(item, list):
            flat.extend(item)
        else:
            flat.append(item)

    for res in flat:

        if res is None:
            continue

        flavs  = res["flavs"]
        masses = res["masses"]
        regions = res["best_regions"]

        for flav in flavs:
            for mass in masses:

                print("\n--------------------------------------------------")
                print(f"{flav} | mass={mass} | mode={opt_mode}")
                print("--------------------------------------------------")

                # =====================================
                # LOOP REGIONS (low / high)
                # =====================================
                for region in regions:

                    print(f"\n[{region}]")

                    bins = regions[region]["bins"]

                    sub = data[region]

                    edges_full = np.array(sub["edges"])
                    bin_lo = edges_full[:-1]

                    for i in range(len(bins) - 1):

                        lo = bins[i]
                        hi = bins[i+1]

                        if i == len(bins) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        # =====================================
                        # RUN2 MODE
                        # =====================================
                        if opt_mode == "Run2":

                            B_tot = 0.0
                            E_tot = 0.0

                            for era in ERAS:

                                if flav not in sub["background"]:
                                    continue
                                if era not in sub["background"][flav]:
                                    continue

                                B_arr = sub["background"][flav][era]
                                F_arr = sub["fake"][flav][era]
                                E_arr = sub["bkg_err2"][flav][era]

                                b = B_arr[mask].sum()
                                f = F_arr[mask].sum()
                                e = E_arr[mask].sum()

                                f, b = fix_fake_and_bkg(
                                    f, b, FAKE_FLOOR,
                                    flavour=flav,
                                    era=era
                                )

                                B_tot += b
                                E_tot += e

                            rel = math.sqrt(E_tot)/B_tot if B_tot > 0 else 0

                            line = f"[{lo:5.2f},{hi:5.2f}]  B={B_tot:8.3f}  rel={rel:6.3f}"

                            if B_tot < 1.0:
                                print(line)  # replace red(line) if needed
                            else:
                                print(line)

                        # =====================================
                        # ERA MODE
                        # =====================================
                        elif opt_mode == "Era":

                            for era in ERAS:

                                if flav not in sub["background"]:
                                    continue
                                if era not in sub["background"][flav]:
                                    continue

                                B_arr = sub["background"][flav][era]
                                F_arr = sub["fake"][flav][era]
                                E_arr = sub["bkg_err2"][flav][era]

                                b = B_arr[mask].sum()
                                f = F_arr[mask].sum()
                                e = E_arr[mask].sum()

                                f, b = fix_fake_and_bkg(
                                    f, b, FAKE_FLOOR,
                                    flavour=flav,
                                    era=era
                                )

                                rel = math.sqrt(e)/b if b > 0 else 0

                                line = f"[{lo:5.2f},{hi:5.2f}] {era:10s}  B={b:8.3f}  rel={rel:6.3f}"

                                if b < 1.0:
                                    print(line)
                                else:
                                    print(line)

                        else:
                            raise ValueError(f"Unknown opt_mode: {opt_mode}")



def build_run2_bkg_per_flavour(arrays, flavs):

    import numpy as np

    B_run2_dict = {}
    E_run2_dict = {}

    for flav in flavs:

        B_run2 = None
        E_run2 = None

        # background is identical across masses, pick one
        masses_available = [m for (f, m) in arrays if f == flav]

        if not masses_available:
            continue

        mass0 = masses_available[0]
        era_dict = arrays[(flav, mass0)]["Era"]

        for era in era_dict:

            S_arr, B_arr, F_arr, E_arr = era_dict[era]

            B_corr = np.zeros_like(B_arr)

            for i in range(len(B_arr)):

                f = F_arr[i]
                b = B_arr[i]

                f, b = fix_fake_and_bkg(
                    f, b, FAKE_FLOOR,
                    flavour=flav,
                    era=era
                )

                B_corr[i] = b

            if B_run2 is None:
                B_run2 = B_corr.copy()
                E_run2 = E_arr.copy()  # E_arr is variance (sigma^2)
            else:
                B_run2 += B_corr
                E_run2 += E_arr

        B_run2_dict[flav] = B_run2
        E_run2_dict[flav] = E_run2

    return B_run2_dict, E_run2_dict
                        

def generate_binnings(edges, nbin_mode, cat,
                      min_width=0.1,
                      B_dict=None,
                      E_dict=None,
                      bin_lo=None,
                      coarse_grid=None):

    import itertools
    import math

    edge_lo = edges[0]
    edge_hi = edges[-1]
    #internal_edges = edges[1:-1]

    # ----------------------------------------
    # Build internal edges
    # ----------------------------------------
    if coarse_grid is not None:
        
        edge_lo = edges[0]
        edge_hi = edges[-1]
        
        # build grid: 0.5, 1.0, ..., 9.5
        internal_edges = [
            round(edge_lo + coarse_grid * i, 6)
            for i in range(1, int((edge_hi - edge_lo) / coarse_grid))
        ]
        
    else:
        internal_edges = edges[1:-1]

    if coarse_grid is not None:
        
        print("\n[DEBUG] Using coarse grid =", coarse_grid)
        
        print("[DEBUG] internal_edges (first 10):", internal_edges[:10])
        
    #if len(edges[1:-1]) > 100:
    #    tqdm.write(f"[DEBUG] edges before = {len(edges[1:-1])}, after = {len(internal_edges)}")
    #    
    #    tqdm.write(f"[DEBUG] first edges: {internal_edges[:10]}")
    #    
    #    tqdm.write(f"[DEBUG] last edges : {internal_edges[-10:]}")

    # ----------------------------------------
    # Decide nbins list
    # ----------------------------------------
    if isinstance(nbin_mode, int):
        nbins_list = [nbin_mode]

    elif nbin_mode == "scan":
        nbins_list = [3, 4,5]

    else:
        raise ValueError(f"Unknown nbin_mode: {nbin_mode}")

    
    # ----------------------------------------
    # Generate all valid binnings
    # ----------------------------------------
    all_binnings = []

    for nbins in nbins_list:
        tqdm.write(f"[INFO] Generating {nbins} bins")
                
        for combo in itertools.combinations(internal_edges, nbins - 1):
            
            tail_ok = True
            # Tail stat check
            for flav in B_dict:

                B_run2 = B_dict[flav]
                
                E_run2 = E_dict[flav]
                
                tail_lo = combo[-1]
                
                tail_hi = edges[-1]
                
                mask = (bin_lo >= tail_lo) & (bin_lo <= tail_hi)
                
                b = B_run2[mask].sum()
                
                e = E_run2[mask].sum()
                
                if b <= 0:
                    tail_ok = False
                    break

                rel = math.sqrt(e) / b
                
                if not pass_stat_and_err(b, rel):
                    tail_ok = False
                    break

            if not tail_ok:
                continue
            
            # Build binning
            full = (edge_lo,) + combo + (edge_hi,)
            
            # Min width check
            valid = True
            for i in range(len(full) - 1):
                if full[i+1] - full[i] < min_width:
                    valid = False
                    break

            if valid:
                all_binnings.append(full)

    return all_binnings


def set_stat_config(cfg):
    global Bin_NBKG_REQ, Bin_NBKG_REQ_Tight, Bin_BKG_RelUnc

    Bin_NBKG_REQ = getattr(cfg, "Bin_NBKG_REQ", Bin_NBKG_REQ)
    Bin_NBKG_REQ_Tight = getattr(cfg, "Bin_NBKG_REQ_Tight", Bin_NBKG_REQ_Tight)
    Bin_BKG_RelUnc = getattr(cfg, "Bin_BKG_RelUnc", Bin_BKG_RelUnc)

    print("\n[STAT CONFIG]")
    print(f"  Bin_NBKG_REQ       = {Bin_NBKG_REQ}")
    print(f"  Bin_NBKG_REQ_Tight = {Bin_NBKG_REQ_Tight}")
    print(f"  Bin_BKG_RelUnc     = {Bin_BKG_RelUnc}")

def pass_stat_era(B,rel):
    return (B >= 0.15) and (rel < 0.5)

def pass_stat_and_err(B, rel, debug=False):
    if debug:
        print(f"  Bin_NBKG_REQ       = {Bin_NBKG_REQ}")
        print(f"  Bin_NBKG_REQ_Tight = {Bin_NBKG_REQ_Tight}")
        print(f"  Bin_BKG_RelUnc     = {Bin_BKG_RelUnc}")
        
    return (B >= Bin_NBKG_REQ) and (rel < Bin_BKG_RelUnc)




def red(text):
    return f"\033[91m{text}\033[0m"

def green(text):
    return f"\033[92m{text}\033[0m"



import math
import numpy as np



def debug_compare_binning_sr2(data, flavs, masses):

    import numpy as np

    print("\n====================================")
    print(" SR2 BINNING DEBUG (FIXED vs RAW)")
    print("====================================")

    fixed_edges = [0, 1,2,3,4,5,10]

    for flav in flavs:
        for mass in masses:

            print("\n------------------------------------")
            print(f"{flav} | mass={mass}")
            print("------------------------------------")

            for region in ["low", "high"]:

                print(f"\n[{region}]")

                sub = data[region]

                edges_full = np.array(sub["edges"])
                bin_lo = edges_full[:-1]

                Z2 = 0.0

                for i in range(len(fixed_edges) - 1):

                    lo = fixed_edges[i]
                    hi = fixed_edges[i+1]

                    if i == len(fixed_edges) - 2:
                        mask = (bin_lo >= lo) & (bin_lo <= hi)
                    else:
                        mask = (bin_lo >= lo) & (bin_lo < hi)

                    S_tot = 0.0
                    B_tot = 0.0
                    E_tot = 0.0

                    for era in ERAS:

                        S = sub["signal_norm_flav"][flav][mass][era][mask].sum()
                        B = sub["background"][flav][era][mask].sum()
                        F = sub["fake"][flav][era][mask].sum()
                        E = sub["bkg_err2"][flav][era][mask].sum()

                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav,
                            era=era
                        )

                        S_tot += S
                        B_tot += B
                        E_tot += E

                    if B_tot > 0:
                        Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot)
                    else:
                        Z = 0.0

                    print(f"[{lo:5.2f},{hi:5.2f}] S={S_tot:7.3f} B={B_tot:7.3f} Z={Z:6.3f}")

                    Z2 += Z * Z

                print(f"--> TOTAL Z = {Z2**0.5:.4f}")


def debug_compare_binnings_sr2(data, scan_results, masses, flavours):

    import numpy as np
    import math

    print("\n====================================")
    print(" SR2 BINNING COMPARISON")
    print("====================================")

    fixed_edges = [0, 200, 400, 1200]

    for res in scan_results:

        flavs = res["flavs"]
        masses_res = res["masses"]
        regions = res["best_regions"]

        for flav in flavs:
            for mass in masses_res:

                print("\n------------------------------------")
                print(f"{flav} | mass={mass}")
                print("------------------------------------")

                total_Z2 = 0.0

                for region in ["low", "high"]:

                    sub = data[region]

                    edges_scan = regions[region]["bins"]
                    edges_full = np.array(sub["edges"])
                    bin_lo = edges_full[:-1]

                    print(f"\n[{region}]")

                    # -------------------------
                    # SCAN BINNING
                    # -------------------------
                    Z2_scan = 0.0

                    print("\n-- SCAN BINNING --")

                    for i in range(len(edges_scan) - 1):

                        lo = edges_scan[i]
                        hi = edges_scan[i+1]

                        if i == len(edges_scan) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        S_tot = 0.0
                        B_tot = 0.0
                        E_tot = 0.0

                        for era in ERAS:

                            S = sub["signal_norm_flav"][flav][mass][era][mask].sum()
                            B = sub["background"][flav][era][mask].sum()
                            F = sub["fake"][flav][era][mask].sum()
                            E = sub["bkg_err2"][flav][era][mask].sum()

                            F, B = fix_fake_and_bkg(
                                F, B, FAKE_FLOOR,
                                flavour=flav,
                                era=era
                            )

                            S_tot += S
                            B_tot += B
                            E_tot += E

                        Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot) if B_tot > 0 else 0.0

                        print(f"[SCAN {lo:5.2f},{hi:5.2f}] S={S_tot:7.3f} B={B_tot:7.3f} Z={Z:6.3f}")

                        Z2_scan += Z * Z

                    # -------------------------
                    # FIXED BINNING
                    # -------------------------
                    Z2_fix = 0.0

                    print("\n-- FIXED BINNING --")

                    for i in range(len(fixed_edges) - 1):

                        lo = fixed_edges[i]
                        hi = fixed_edges[i+1]

                        if i == len(fixed_edges) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        S_tot = 0.0
                        B_tot = 0.0
                        E_tot = 0.0

                        for era in ERAS:

                            S = sub["signal_norm_flav"][flav][mass][era][mask].sum()
                            B = sub["background"][flav][era][mask].sum()
                            F = sub["fake"][flav][era][mask].sum()
                            E = sub["bkg_err2"][flav][era][mask].sum()

                            F, B = fix_fake_and_bkg(
                                F, B, FAKE_FLOOR,
                                flavour=flav,
                                era=era
                            )

                            S_tot += S
                            B_tot += B
                            E_tot += E

                        Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot) if B_tot > 0 else 0.0

                        print(f"[FIX  {lo:5.2f},{hi:5.2f}] S={S_tot:7.3f} B={B_tot:7.3f} Z={Z:6.3f}")

                        Z2_fix += Z * Z

                    print(f"\n--> SCAN Z = {math.sqrt(Z2_scan):.4f}")
                    print(f"--> FIX  Z = {math.sqrt(Z2_fix):.4f}")

                    total_Z2 += Z2_scan

                print("\n====================================")
                print(f" TOTAL Z = {math.sqrt(total_Z2):.4f}")
                print("====================================")

def print_sr2_scan_summary(results):

    import math

    print("\n====================================================")
    print(" SR2 Z SUMMARY (SCAN RESULTS)")
    print("====================================================")

    for res in results:

        flavs = res["flavs"]
        masses = res["masses"]
        regions = res["best_regions"]
        Z_total = res["Z_run2"]

        for flav in flavs:
            for mass in masses:

                print("\n----------------------------------------------------")
                print(f" FLAVOUR: {flav} | MASS: {mass}")
                print("----------------------------------------------------")

                total_Z2 = 0.0

                for region in ["low", "high"]:

                    Z_region = regions[region]["Z"]
                    bins = regions[region]["bins"]

                    print(f"\n[{region}]")
                    print(f"  Z = {Z_region:.4f}")
                    print(f"  bins = {bins}")

                    total_Z2 += Z_region * Z_region

                print("\n------------------------------------")
                print(f" TOTAL Z = {math.sqrt(total_Z2):.4f}")
                print("------------------------------------")

                
def list_available_configs(config_dir="config", script_name=None):
    print("\n====================================")
    print(" AVAILABLE CONFIG FILES")
    print("====================================\n")

    if not os.path.isdir(config_dir):
        print(f"[ERROR] Config directory not found: {config_dir}")
        return

    files = sorted(os.listdir(config_dir))
    py_files = [f for f in files if f.endswith(".py") and not f.startswith("__")]

    if len(py_files) == 0:
        print("[WARNING] No config files found")
        return

    if script_name is None:
        script_name = os.path.basename(sys.argv[0])

    for f in py_files:
        mod = f.replace(".py", "")
        print(f"python {script_name} --config python.config.{mod}")

    print("\n====================================\n")
    
def ConvertConfPath(path):
    # remove .py if present
    if path.endswith(".py"):
        path = path[:-3]
    else:
        return path

    # replace / with .
    path = path.replace("/", ".")

    # remove leading ./ if present
    if path.startswith("."):
        path = path[1:]

    return path

def ReadConfig(cfg):

    def get(name):
        if not hasattr(cfg, name):
            raise AttributeError(f"[CONFIG ERROR] Missing '{name}' in config")
        return getattr(cfg, name)
    
    return (
        get("MASSES"),
        get("RUN_Z_NO_UNC"),
        get("LOG_TAG"),
        get("TAG"),        
    )


def get_latest_dir(base):
    dirs = [os.path.join(base, d)
            for d in os.listdir(base)
            if os.path.isdir(os.path.join(base, d))]
    return max(dirs, key=os.path.getmtime)



def compute_bin_Z(s, b):
    if s <= 0 or b <= 0:
        return 0.0
    return math.sqrt(2*((s+b)*math.log(1+s/b) - s))

def fix_fake_and_bkg(F, B, FAKE_FLOOR, flavour=None, era=None, debug=False):

    if F <= 0.0:
        delta = FAKE_FLOOR - F

        if debug:
            print("[fix_fake_and_bkg] era={} flavour={}, fake_before={:.6f}, fake_floor={:.6f}, delta={:.6f}".format(
                era, flavour, F, FAKE_FLOOR, delta
            ))

        if debug and era == "Run2":
            print("[DEBUG] Before:  F =", F, "B =", B)

        F = FAKE_FLOOR
        B += delta

        if debug and era == "Run2":
            print("[DEBUG] After:   F =", F, "B =", B)

    return F, B



def compute_bin_Z_with_unc(S, B, E,run_z_no_unc=True):
                
    if run_z_no_unc:
        return compute_bin_Z(S,B)
    
    if S <= 0 or B <= 0:
        return 0.0

    if B <= 1e-9:
        return 0.0

    sigma2 = E

    if sigma2 <= 0:
        return compute_bin_Z(S, B)

    term1 = (S + B) * math.log((S + B)*(B + sigma2) / (B*B + (S + B)*sigma2))
    term2 = (B*B / sigma2) * math.log(1 + sigma2*S / (B*(B + sigma2)))

    Z2 = 2 * (term1 - term2)

    return math.sqrt(Z2) if Z2 > 0 else 0.0


def print_sr2_bin_table(data, flav, mass):

    import numpy as np
    import math

    print("\n========================================================")
    print(f" BIN TABLE (SR2) | {flav} | mass={mass}")
    print("========================================================")

    header = (
        "Bin | Era       | Region | Range           | "
        "   S      B      rel_unc     Z"
    )
    print(header)
    print("-" * len(header))

    total_Z2 = 0.0
    total_B_run2 = 0.0

    # ======================================================
    # PER-ERA TABLES
    # ======================================================
    for era in ERAS:

        print("\n--------------------------------------------------------")
        print(f" ERA: {era}")
        print("--------------------------------------------------------")

        bin_counter = 0
        total_B_era = 0.0

        for region in ["low", "high"]:

            sub = data[region]

            edges_full = np.array(sub["edges"])
            bin_lo = edges_full[:-1]

            S_arr = sub["signal_norm_flav"][flav][mass][era]
            B_arr = sub["background"][flav][era]
            F_arr = sub["fake"][flav][era]
            E_arr = sub["bkg_err2"][flav][era]

            edges = edges_full  # or use best binning if available

            for i in range(len(edges) - 1):

                lo = edges[i]
                hi = edges[i + 1]

                if i == len(edges) - 2:
                    mask = (bin_lo >= lo) & (bin_lo <= hi)
                else:
                    mask = (bin_lo >= lo) & (bin_lo < hi)

                S = S_arr[mask].sum()
                B = B_arr[mask].sum()
                F = F_arr[mask].sum()
                E = E_arr[mask].sum()

                F, B = fix_fake_and_bkg(
                    F, B, FAKE_FLOOR,
                    flavour=flav, era=era
                )

                total_B_era += B
                total_B_run2 += B

                rel_unc = math.sqrt(E)/B if B > 0 else 0.0
                stat_ok = pass_stat_and_err(B, rel_unc)

                if S > 0 and B > 0:
                    Z = compute_bin_Z_with_unc(S, B, E)
                    total_Z2 += Z * Z
                else:
                    Z = 0.0

                line = (
                    f"{bin_counter:3d} | {era:9s} | {region:6s} | "
                    f"[{lo:5.2f},{hi:5.2f}] | "
                    f"{S:6.3f} {B:7.2f} {rel_unc:9.3f} {Z:7.3f}"
                )

                if not stat_ok:
                    print(red(line))
                else:
                    print(line)

                bin_counter += 1

        print(f"\n>>> Total B (era {era}) = {total_B_era:.3f}")

    # ======================================================
    # RUN2 COMBINED TABLE
    # ======================================================
    print("\n========================================================")
    print(" RUN2 COMBINED TABLE")
    print("========================================================")

    header = (
        "Bin | Region | Range           | "
        "   S      B      rel_unc     Z"
    )
    print(header)
    print("-" * len(header))

    bin_counter = 0

    for region in ["low", "high"]:

        print("\n--------------------------------------------------------")
        print(f" REGION: {region}")
        print("--------------------------------------------------------")

        sub = data[region]

        edges_full = np.array(sub["edges"])
        bin_lo = edges_full[:-1]

        edges = edges_full

        for i in range(len(edges) - 1):

            lo = edges[i]
            hi = edges[i + 1]

            if i == len(edges) - 2:
                mask = (bin_lo >= lo) & (bin_lo <= hi)
            else:
                mask = (bin_lo >= lo) & (bin_lo < hi)

            S = 0.0
            B = 0.0
            E = 0.0

            for era in ERAS:

                S_arr = sub["signal_norm_flav"][flav][mass][era]
                B_arr = sub["background"][flav][era]
                F_arr = sub["fake"][flav][era]
                E_arr = sub["bkg_err2"][flav][era]

                s = S_arr[mask].sum()
                b = B_arr[mask].sum()
                f = F_arr[mask].sum()
                e = E_arr[mask].sum()

                f, b = fix_fake_and_bkg(
                    f, b, FAKE_FLOOR,
                    flavour=flav, era=era
                )

                S += s
                B += b
                E += e

            rel_unc = math.sqrt(E)/B if B > 0 else 0.0

            if S > 0 and B > 0:
                Z = compute_bin_Z_with_unc(S, B, E)
            else:
                Z = 0.0

            print(f"{bin_counter:3d} | {region:6s} | "
                  f"[{lo:5.2f},{hi:5.2f}] | "
                  f"{S:6.3f} {B:7.2f} {rel_unc:9.3f} {Z:7.3f}")

            bin_counter += 1

    # ======================================================
    # FINAL TOTALS
    # ======================================================
    total_Z = math.sqrt(total_Z2)

    print("\n========================================================")
    print(f" TOTAL Z (Run2) = {total_Z:.4f}")
    print(f" TOTAL B (Run2) = {total_B_run2:.3f}")
    print("========================================================")
