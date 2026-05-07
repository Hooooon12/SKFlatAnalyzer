import math
import os,sys
from python.config.default_config import ERAS, FLAVOURS, FAKE_FLOOR
from python.utils.ref_bins import  get_sr3_ref_edges
from python.config.NBIN import get_nbins_for_region
# Default fallback values (will be overridden at runtime)                                                                                                                                                                                                                                                                                                                                                                                                                               
Bin_NBKG_REQ = 1.0
Bin_NBKG_REQ_Tight = 1.0
Bin_BKG_RelUnc = 0.5

import numpy as np
import math

from tqdm import tqdm


DEBUG = False

def build_mass_weights_from_ref(ref_results, mode="run2"):

    weights = {}

    for r in ref_results:
        mass = r["mass"]
        Z = r[mode]

        if Z > 0:
            weights[mass] = 1.0 / (Z * Z)
        else:
            weights[mass] = 1.0  # fallback

    return weights

def recompute_per_mass_with_fixed_binning(data, global_results):

    import math

    import numpy as np

    # ----------------------------------------

    # FLATTEN INPUT (CRITICAL FIX)

    # ----------------------------------------

    flat_results = []

    for item in global_results:

        if isinstance(item, list):

            flat_results.extend(item)

        else:

            flat_results.append(item)

    output = []

    # ----------------------------------------

    # LOOP OVER FLAT RESULTS

    # ----------------------------------------

    for res in flat_results:

        flavs    = res.get("flavs", [])
        masses   = res.get("masses", [])
        met      = res["best_met"]
        regions  = res["best_regions"]

        for flav in flavs:
            for mass in masses:

                total_Z2 = 0.0

                new_result = {
                    "flav": flav,
                    "mass": mass,
                    "met": met,
                    "regions": {},
                }

                # ----------------------------------------
                # LOOP REGIONS (fixed binning)
                # ----------------------------------------
                for cat, info in regions.items():

                    edges = info["bins"]

                    sub = data[met][cat]

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

                            if flav not in sub["signal_nonorm"]:
                                continue
                            if mass not in sub["signal_nonorm"][flav]:
                                continue
                            if era not in sub["signal_nonorm"][flav][mass]:
                                continue
                            
                            S_arr = sub["signal_nonorm"][flav][mass][era]
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

                        if B_tot > 0:
                            Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot)
                        else:
                            Z = 0.0

                        region_Z2 += Z * Z

                    total_Z2 += region_Z2

                    new_result["regions"][cat] = {
                        "bins": edges
                    }

                
                new_result["run2"] = math.sqrt(total_Z2)
                new_result["quad"] = new_result["run2"]
                output.append(new_result)

    return output


def print_bkg_per_bin(data, scan_outputs, config):

    import numpy as np
    import math

    print("\n==============================")
    print(" BKG PER BIN DEBUG")
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
        best_met = res["best_met"]
        regions  = res["best_regions"]

        for flav in flavs:
            for mass in masses:

                print("\n--------------------------------------------------")
                print(f"{flav} | mass={mass} | MET={best_met} | mode={opt_mode}")
                print("--------------------------------------------------")

                for cat in regions:

                    print(f"\n[{cat}]")

                    bins = regions[cat]["bins"]

                    sub = data[best_met][cat]
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

                                # correct place
                                f, b = fix_fake_and_bkg(
                                    f, b, FAKE_FLOOR,
                                    flavour=flav,
                                    era=era
                                )

                                B_tot += b
                                E_tot += e

                            rel = math.sqrt(E_tot)/B_tot if B_tot > 0 else 0

                            line = f"[{lo:5.0f},{hi:5.0f}]  B={B_tot:8.3f}  rel={rel:6.3f}"

                            if B_tot < 1.0:
                                print(red(line))
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

                                line = f"[{lo:5.0f},{hi:5.0f}] {era:10s}  B={b:8.3f}  rel={rel:6.3f}"

                                if b < 1.0:
                                    print(red(line))
                                else:
                                    print(line)

                        else:
                            raise ValueError(f"Unknown opt_mode: {opt_mode}")

def recompute_per_met_results(data, scan_outputs, met_target):

    import math
    import numpy as np

    results = []

    # flatten
    flat = []
    for item in scan_outputs:
        if isinstance(item, list):
            flat.extend(item)
        else:
            flat.append(item)

    for res in flat:

        if res is None:
            continue

        flavs  = res.get("flavs", [])
        masses = res.get("masses", [])

        # find this MET entry
        per_met = res.get("per_met", [])

        met_entry = None
        for m in per_met:
            if str(m["met"]) == str(met_target):
                met_entry = m
                break

        if met_entry is None:
            continue

        regions = met_entry["regions"]

        # ----------------------------------------
        # NOW recompute per (flav, mass)
        # ----------------------------------------
        for flav in flavs:
            for mass in masses:

                total_Z2 = 0.0

                for cat in regions:

                    bins = regions[cat]["bins"]
                    sub = data[str(met_target)][cat]

                    edges_full = np.array(sub["edges"])
                    bin_lo = edges_full[:-1]

                    Z2_region = 0.0

                    for i in range(len(bins) - 1):

                        lo = bins[i]
                        hi = bins[i + 1]

                        if i == len(bins) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        S_tot = 0.0
                        B_tot = 0.0
                        E_tot = 0.0

                        for era in ERAS:

                            if flav not in sub["signal_nonorm"]:
                                continue
                            if mass not in sub["signal_nonorm"][flav]:
                                continue
                            if era not in sub["signal_nonorm"][flav][mass]:
                                continue

                            S_arr = sub["signal_nonorm"][flav][mass][era]
                            B_arr = sub["background"][flav][era]
                            F_arr = sub["fake"][flav][era]
                            E_arr = sub["bkg_err2"][flav][era]

                            s = S_arr[mask].sum()
                            b = B_arr[mask].sum()
                            f = F_arr[mask].sum()
                            e = E_arr[mask].sum()

                            f, b = fix_fake_and_bkg(
                                f, b, FAKE_FLOOR,
                                flavour=flav,
                                era=era
                            )

                            S_tot += s
                            B_tot += b
                            E_tot += e

                        if S_tot > 0 and B_tot > 0:
                            Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot)
                            Z2_region += Z * Z

                    total_Z2 += Z2_region

                Z_final = math.sqrt(total_Z2)

                results.append({
                    "flav": flav,
                    "mass": mass,
                    "run2": Z_final,
                    "quad": Z_final,
                    "met": met_target,
                    "regions": regions
                })

    return results


def build_fixed_met_results(data, scan_outputs, best_met_map, config):

    import math
    import numpy as np

    results = []

    # flatten
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
        per_met = res.get("per_met", [])

        for flav in flavs:

            best_met = str(best_met_map[flav]["best_met"])

            # find MET binning
            met_entry = None
            for m in per_met:
                if str(m["met"]) == best_met:
                    met_entry = m
                    break

            if met_entry is None:
                continue

            regions = met_entry["regions"]

            # ----------------------------------------
            # RECOMPUTE Z per (flav, mass)
            # ----------------------------------------
            for mass in masses:

                total_Z2 = 0.0

                for cat in regions:

                    bins = regions[cat]["bins"]
                    sub = data[best_met][cat]

                    edges_full = np.array(sub["edges"])
                    bin_lo = edges_full[:-1]

                    Z2_region = 0.0

                    for i in range(len(bins) - 1):

                        lo = bins[i]
                        hi = bins[i + 1]

                        if i == len(bins) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        S_tot = 0.0
                        B_tot = 0.0
                        E_tot = 0.0

                        for era in ERAS:

                            if flav not in sub["signal_nonorm"]:
                                continue
                            if mass not in sub["signal_nonorm"][flav]:
                                continue
                            if era not in sub["signal_nonorm"][flav][mass]:
                                continue

                            S_arr = sub["signal_nonorm"][flav][mass][era]
                            B_arr = sub["background"][flav][era]
                            F_arr = sub["fake"][flav][era]
                            E_arr = sub["bkg_err2"][flav][era]

                            s = S_arr[mask].sum()
                            b = B_arr[mask].sum()
                            f = F_arr[mask].sum()
                            e = E_arr[mask].sum()

                            f, b = fix_fake_and_bkg(
                                f, b, FAKE_FLOOR,
                                flavour=flav,
                                era=era
                            )

                            S_tot += s
                            B_tot += b
                            E_tot += e

                        if S_tot > 0 and B_tot > 0:
                            Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot)
                            Z2_region += Z * Z

                    total_Z2 += Z2_region

                Z_final = math.sqrt(total_Z2)

                results.append({
                    "flav": flav,
                    "mass": mass,
                    "run2": Z_final,
                    "quad": Z_final,
                    "met": best_met,
                    "regions": regions
                })

    return results



def build_run2_bkg_per_flavour(arrays, flavs):

    import numpy as np

    B_run2_dict = {}
    E_run2_dict = {}

    for flav in flavs:

        B_run2 = None
        E_run2 = None

        # pick ANY mass just to access structure
        # (background does not depend on mass)
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
                E_run2 = E_arr.copy()
            else:
                B_run2 += B_corr
                E_run2 += E_arr

        B_run2_dict[flav] = B_run2
        E_run2_dict[flav] = E_run2

    return B_run2_dict, E_run2_dict

def filter_edges_by_region(edges):

    filtered = []

    for e in edges:

        if e < 300:
            if e % 10 == 0:
                filtered.append(e)

        elif e < 500:
            if e % 20 == 0:
                filtered.append(e)

        elif e < 900:
            if e % 50 == 0:
                filtered.append(e)

        else:
            if e % 100 == 0:
                filtered.append(e)

    return sorted(set(filtered))

def generate_binnings(edges, nbin_mode, cat,
                      min_width=20,
                      min_first_edge=120,
                      B_dict=None,
                      E_dict=None,
                      bin_lo=None):

    import itertools
    import math

    edge_lo = edges[0]
    edge_hi = edges[-1]
    #internal_edges = edges[1:-1]
    internal_edges = filter_edges_by_region(edges[1:-1])

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

    elif nbin_mode == "fixed":
        nbins_list = [get_nbins_for_region(cat)]

    elif nbin_mode == "scan":
        if "LTcut" in cat:
            nbins_list = [4, 5, 6, 7]
        else:
            nbins_list = [3, 4, 5]

    else:
        raise ValueError(f"Unknown nbin_mode: {nbin_mode}")

    
    # ----------------------------------------
    # Generate all valid binnings
    # ----------------------------------------
    all_binnings = []

    for nbins in nbins_list:
        tqdm.write(f"[INFO] Generating {nbins} bins")
                
        for combo in itertools.combinations(internal_edges, nbins - 1):

            # First edge cut
            if combo[0] < min_first_edge:
                continue

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
        print(f"  Bin_BKG_RelUnc     = {Bin_BKG_RelUnc}")
        
    return (B >= Bin_NBKG_REQ) and (rel < Bin_BKG_RelUnc)




def red(text):
    return f"\033[91m{text}\033[0m"

def green(text):
    return f"\033[92m{text}\033[0m"


def get_met_boundary(flav, era):
    # default                                                                                                                                                                                                                                
    boundary = 5

    if flav == "EE":
        if "2016" in era:
            boundary = 4
        else:
            boundary = 3

    return str(boundary)


def parse_sr3_category(cat):

    parts = cat.split("_")

    jet = parts[0]                  # LowJet / HighJet                                                                                                                                                                                       
    is_ltcut = "LTcut" in parts[3]  # True/False                                                                                                                                                                                             

    return jet, is_ltcut


import math
import numpy as np



def debug_compare_binnings_per_boundary(data, MASSES, FLAVOURS, ERAS):

    print("\n====================================================")
    print(" DEBUG BINNING COMPARISON (PER MET BOUNDARY)")
    print("====================================================")

    CATEGORIES = [
        "LowJet_LT_MET{X}_LTcut",
        "HighJet_LT_MET{X}_LTcut",
        "LowJet_LT_MET{X}_GTcut",
        "HighJet_LT_MET{X}_GTcut",
    ]

    fixed_edges = [0,200,400,1200]

    for flav in FLAVOURS:

        print("\n====================================================")
        print(f" FLAVOUR: {flav}")
        print("====================================================")

        for mass in MASSES:

            print("\n----------------------------------------------------")
            print(f" MASS: {mass}")
            print("----------------------------------------------------")

            for boundary in data:

                print("\n####################################################")
                print(f" MET boundary = {boundary}")
                print("####################################################")

                # =========================
                # PER ERA
                # =========================
                for era in ERAS:

                    print(f"\n================ ERA: {era} =================")

                    Z2_ref_era = 0.0
                    Z2_fix_era = 0.0

                    for cat_template in CATEGORIES:

                        cat = cat_template.format(X=boundary)

                        if cat not in data[boundary]:
                            continue

                        sub = data[boundary][cat]

                        jet, is_ltcut = parse_sr3_category(cat)
                        met_label = "<" if is_ltcut else ">="

                        edges_full = sub["edges"]
                        bin_lo = edges_full[:-1]

                        S_arr = sub["signal_nonorm"][flav][mass][era]
                        B_arr = sub["background"][flav][era]
                        E_arr = sub["bkg_err2"][flav][era]

                        print("\n----------------------------------------")
                        print(f"{cat} | MET {met_label}{boundary}")
                        print("----------------------------------------")

                        # =========================
                        # REF BINNING
                        # =========================
                        edges_ref = get_sr3_ref_edges(flav, era, jet, is_ltcut)

                        Z2_cat_ref = 0.0

                        for i in range(len(edges_ref) - 1):

                            lo = edges_ref[i]
                            hi = edges_ref[i + 1]
                            
                            if i == len(edges_ref) - 2:
                                mask = (bin_lo >= lo) & (bin_lo <= hi)
                            else:
                                mask = (bin_lo >= lo) & (bin_lo < hi)
                                
                            S = S_arr[mask].sum()
                            B = B_arr[mask].sum()
                            E = E_arr[mask].sum()

                            Z = compute_bin_Z_with_unc(S, B, E) if B > 0 else 0.0

                            print(f"[REF  {lo:4.0f},{hi:4.0f}]  S={S:7.3f}  B={B:7.3f}  Z={Z:6.3f}")

                            Z2_cat_ref += Z * Z

                        Z_cat_ref = Z2_cat_ref ** 0.5
                        Z2_ref_era += Z2_cat_ref

                        print(f"--> REF CAT Z = {Z_cat_ref:.4f}")

                        # =========================
                        # FIXED BINNING (200 GeV)
                        # =========================
                        Z2_cat_fix = 0.0

                        for i in range(len(fixed_edges) - 1):

                            lo = fixed_edges[i]
                            hi = fixed_edges[i + 1]

                            mask = (bin_lo >= lo) & (bin_lo < hi)

                            S = S_arr[mask].sum()
                            B = B_arr[mask].sum()
                            E = E_arr[mask].sum()

                            Z = compute_bin_Z_with_unc(S, B, E) if B > 0 else 0.0

                            print(f"[FIX  {lo:4.0f},{hi:4.0f}]  S={S:7.3f}  B={B:7.3f}  Z={Z:6.3f}")

                            Z2_cat_fix += Z * Z

                        Z_cat_fix = Z2_cat_fix ** 0.5
                        Z2_fix_era += Z2_cat_fix

                        print(f"--> FIX CAT Z = {Z_cat_fix:.4f}")

                    print(f"\n==== ERA REF Z ({era}) = {Z2_ref_era ** 0.5:.4f}")
                    print(f"==== ERA FIX Z ({era}) = {Z2_fix_era ** 0.5:.4f}")

                # =========================
                # RUN2 SUMMARY (per boundary)
                # =========================
                print("\n================ RUN2 (PER BOUNDARY) =================")
                print("NOTE: Run2 bin merging not implemented in this debug block")



def debug_compare_binnings(data, masses, flavours, ERAS, mode="ref"):

    print("\n====================================================")
    print(f" DEBUG BINNING COMPARISON ({mode.upper()})")
    print("====================================================")

    CATEGORIES = [
        "LowJet_LT_MET{X}_LTcut",
        "HighJet_LT_MET{X}_LTcut",
        "LowJet_LT_MET{X}_GTcut",
        "HighJet_LT_MET{X}_GTcut",
    ]

    for flav in flavours:

        print("\n====================================================")
        print(f" FLAVOUR: {flav}")
        print("====================================================")

        for mass in masses:

            print("\n----------------------------------------------------")
            print(f" MASS: {mass}")
            print("----------------------------------------------------")

            total_run2_Z2 = 0.0

            for era in ERAS:

                era_Z2 = 0.0

                boundary = get_met_boundary(flav, era)

                print(f"\n================ ERA: {era} =================")

                for cat_template in CATEGORIES:

                    cat = cat_template.format(X=boundary)
                    sub = data[boundary][cat]

                    jet, is_ltcut = parse_sr3_category(cat)
                    met_label = "<" if is_ltcut else ">="

                    edges_full = sub["edges"]
                    bin_lo = edges_full[:-1]

                    # -----------------------------
                    # choose binning
                    # -----------------------------
                    if mode == "ref":
                        edges = get_sr3_ref_edges(flav, era, jet, is_ltcut)

                    elif mode == "fixed200":
                        edges = np.arange(0, 1200 + 1, 200)

                    else:
                        raise ValueError("mode must be 'ref' or 'fixed200'")

                    S_arr = sub["signal_nonorm"][flav][mass][era]
                    B_arr = sub["background"][flav][era]
                    E_arr = sub["bkg_err2"][flav][era]

                    cat_Z2 = 0.0

                    print("\n----------------------------------------")
                    print(f"{cat} | MET {met_label}{boundary}")
                    print("----------------------------------------")

                    for i in range(len(edges) - 1):

                        lo = edges[i]
                        hi = edges[i+1]

                        if i == len(edges) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        S = S_arr[mask].sum()
                        B = B_arr[mask].sum()
                        E = E_arr[mask].sum()

                        # ignore empty bins
                        if B <= 0:
                            Z = 0.0
                        else:
                            Z = compute_bin_Z_with_unc(S, B, E)

                        print(f"[{lo:4.0f},{hi:4.0f}]  S={S:7.3f}  B={B:8.3f}  Z={Z:6.3f}")

                        cat_Z2 += Z * Z

                    cat_Z = math.sqrt(cat_Z2)
                    era_Z2 += cat_Z2

                    print(f"--> CAT Z ({cat}) = {cat_Z:.4f}")

                era_Z = math.sqrt(era_Z2)
                print(f"\n==== ERA TOTAL Z ({era}) = {era_Z:.4f} ====\n")

            # =========================
            # RUN2 (sum over eras)
            # =========================
            print("\n================ RUN2 =================")

            for cat_template in CATEGORIES:

                cat_Z2 = 0.0

                boundary = get_met_boundary(flav, ERAS[0])
                cat = cat_template.format(X=boundary)

                sub = data[boundary][cat]
                edges_full = sub["edges"]
                bin_lo = edges_full[:-1]

                if mode == "ref":
                    # use first era for edges (same structure assumption)
                    jet, is_ltcut = parse_sr3_category(cat)
                    edges = get_sr3_ref_edges(flav, ERAS[0], jet, is_ltcut)
                else:
                    edges = np.arange(0, 1200 + 1, 200)

                print("\n----------------------------------------")
                print(f"{cat} (Run2)")
                print("----------------------------------------")

                for i in range(len(edges) - 1):

                    lo = edges[i]
                    hi = edges[i+1]

                    S = 0.0
                    B = 0.0
                    E = 0.0

                    for era in ERAS:

                        boundary = get_met_boundary(flav, era)
                        cat_e = cat_template.format(X=boundary)
                        sub_e = data[boundary][cat_e]

                        S_arr = sub_e["signal_nonorm"][flav][mass][era]
                        B_arr = sub_e["background"][flav][era]
                        E_arr = sub_e["bkg_err2"][flav][era]

                        edges_full = sub_e["edges"]
                        bin_lo = edges_full[:-1]

                        if i == len(edges) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        S += S_arr[mask].sum()
                        B += B_arr[mask].sum()
                        E += E_arr[mask].sum()

                    if B <= 0:
                        Z = 0.0
                    else:
                        Z = compute_bin_Z_with_unc(S, B, E)

                    print(f"[{lo:4.0f},{hi:4.0f}]  S={S:7.3f}  B={B:8.3f}  Z={Z:6.3f}")

                    cat_Z2 += Z * Z

                cat_Z = math.sqrt(cat_Z2)
                total_run2_Z2 += cat_Z2

                print(f"--> CAT Z (Run2 {cat}) = {cat_Z:.4f}")

            total_run2 = math.sqrt(total_run2_Z2)

            print("\n=======================================")
            print(f" TOTAL RUN2 Z = {total_run2:.4f}")
            print("=======================================")


def print_sr3_z_ref_summary(results):

    import math

    colw = 12
    blockw = colw * len(ERAS)

    print("\n====================================================")
    print(" SR3 Z SUMMARY (REFERENCE BINNING)")
    print("====================================================")

    # group by flavour
    results_by_flav = {}
    for r in results:
        results_by_flav.setdefault(r["flav"], []).append(r)

    for flav in results_by_flav:

        print("\n====================================================")
        print(f" FLAVOUR: {flav}")
        print("====================================================")

        header = (
            f"{'Mass':>6} | "
            + f"{'Reference Binning':^{blockw}}"
            + f" | {'QUAD':>{colw}} {'Run2':>{colw}}"
        )

        subheader = (
            " " * 7 + " | "
            + "".join([f"{era:>{colw}}" for era in ERAS])
            + " |"
        )

        print(header)
        print(subheader)
        print("-" * (len(header) + 10))

        # ----------------------------------------
        # OPTIONAL: print bin edges once per flavour
        # ----------------------------------------
        print("\n[DEBUG] Reference bin edges per era:")

        for era in ERAS:
            print(f"\n  ERA: {era}")

            printed = set()

            # use first result as template (all share same structure)
            sample_regions = results_by_flav[flav][0]["regions"]

            for cat in sample_regions:

                jet, is_ltcut = parse_sr3_category(cat)

                key = (jet, is_ltcut)
                if key in printed:
                    continue
                printed.add(key)

                edges = get_sr3_ref_edges(flav, era, jet, is_ltcut)

                met_label = "<" if is_ltcut else ">="

                print(f"    {jet:7s} MET {met_label}X : {edges}")

        # ===============================
        # LOOP MASSES
        # ===============================
        for r in sorted(results_by_flav[flav], key=lambda x: float(x["mass"])):

            mass = r["mass"]
            regions = r["regions"]

            # ----------------------------------------
            # compute per-era Z (correct QUAD per era)
            # ----------------------------------------
            Z2_era = {era: 0.0 for era in ERAS}

            for cat in regions:

                for era in ERAS:

                    if "per_era" not in regions[cat]:
                        continue
                    if era not in regions[cat]["per_era"]:
                        continue

                    bins = regions[cat]["per_era"][era]

                    for b in bins:
                        z = b["Z"]
                        Z2_era[era] += z * z

            Z_era = {era: math.sqrt(Z2_era[era]) for era in ERAS}

            # ----------------------------------------
            # final numbers (already computed correctly)
            # ----------------------------------------
            Z_quad = r["quad"]
            Z_run2 = r["run2"]

            line = (
                f"{str(mass):>6} | "
                + "".join([f"{Z_era[era]:{colw}.3f}" for era in ERAS])
                + f" | {Z_quad:{colw}.3f} {Z_run2:{colw}.3f}"
            )

            print(line)


            
def debug_fixed_binning_stat_failures(data, masses, flavours):

    import math

    print("\n====================================================")
    print(" DEBUG: FIXED BINNING STAT FAILURES (WITH FAKE FIX)")
    print("====================================================")

    fixed_edges = [0,200,400,1200]

    for flav in flavours:

        print("\n====================================================")
        print(f" FLAVOUR: {flav}")
        print("====================================================")

        for mass in masses:

            print(f"\n--- MASS: {mass} ---")

            for met in data:

                for cat in data[met]:

                    sub = data[met][cat]
                    edges_full = sub["edges"]
                    bin_lo = edges_full[:-1]

                    for i in range(len(fixed_edges) - 1):

                        lo = fixed_edges[i]
                        hi = fixed_edges[i + 1]

                        # mask
                        if i == len(fixed_edges) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        # =========================
                        # ERA LOOP (CORRECT)
                        # =========================
                        B_run2 = 0.0
                        E_run2 = 0.0

                        for era in ERAS:

                            S_arr = sub["signal_nonorm"][flav][mass][era]
                            B_arr = sub["background"][flav][era]
                            F_arr = sub["fake"][flav][era]
                            E_arr = sub["bkg_err2"][flav][era]

                            b = B_arr[mask].sum()
                            f = F_arr[mask].sum()
                            e = E_arr[mask].sum()

                            # apply fake fix PER ERA
                            f, b = fix_fake_and_bkg(
                                f, b, FAKE_FLOOR,
                                flavour=flav, era=era
                            )

                            rel = math.sqrt(e) / b if b > 0 else 0.0

                            if not pass_stat_and_err(b, rel):
                                print(
                                    f"[ERA FAIL] {era:10s} | {flav:5s} | {cat:25s} | "
                                    f"[{lo:4.0f},{hi:4.0f}] | B={b:7.3f} rel={rel:6.3f}"
                                )

                            # accumulate AFTER fix
                            B_run2 += b
                            E_run2 += e

                        # =========================
                        # RUN2 (CONSISTENT)
                        # =========================
                        rel_run2 = math.sqrt(E_run2) / B_run2 if B_run2 > 0 else 0.0

                        if not pass_stat_and_err(B_run2, rel_run2):
                            print(
                                f"[RUN2 FAIL] {'':10s} | {flav:5s} | {cat:25s} | "
                                f"[{lo:4.0f},{hi:4.0f}] | B={B_run2:7.3f} rel={rel_run2:6.3f}"
                            )

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
        get("USE_FAKE_FIX"),
        get("RUN_Z_NO_UNC"),
        get("LOG_TAG"),
        get("TAG"),
        get("NBinScan"),
        get("RunGlobalSig"),
        
    )


def get_latest_dir(base):
    dirs = [os.path.join(base, d)
            for d in os.listdir(base)
            if os.path.isdir(os.path.join(base, d))]
    return max(dirs, key=os.path.getmtime)




def debug_data_summary(data, max_bins=5):

    print("\n==============================")
    print(" DEBUG DATA SUMMARY (SR3)")
    print("==============================")

    for met in data:
        for cat in data[met]:

            sub = data[met][cat]

            print("\n========================================")
            print(f" REGION: MET={met} | {cat}")
            print("========================================")

            # ----------------------------------
            # Edges
            # ----------------------------------
            edges = sub["edges"]

            print("\n[EDGES]")
            print("N bins =", len(edges) - 1)
            print("First edges:", edges[:max_bins+1])
            print("Last edges:", edges[-(max_bins+1):])

            # ----------------------------------
            # Background + Fake
            # ----------------------------------
            for flav in FLAVOURS:

                print(f"\n================ {flav} =================")

                for era in ERAS:

                    print(f"\n[{era}]")

                    B = sub["background"][flav][era]
                    F = sub["fake"][flav][era]

                    print(f"  Bkg:  sum={B.sum():.3f}, min={B.min():.3f}, max={B.max():.3f}")
                    print(f"  Fake: sum={F.sum():.3f}, min={F.min():.3f}, max={F.max():.3f}")

                    # Negative fake bins
                    neg_bins = [(i, F[i]) for i in range(len(F)) if F[i] <= 0]

                    if neg_bins:
                        print(f"  [WARN] {len(neg_bins)} fake bins <= 0 (showing first {max_bins})")
                        print("   ", neg_bins[:max_bins])
                    else:
                        print("  Fake all positive")

                    print("  First bins:")
                    for i in range(min(max_bins, len(B))):
                        print(f"    bin {i}: B={B[i]:.3f}, F={F[i]:.3f}")

                # ----------------------------------
                # Signal checks
                # ----------------------------------
                print(f"\n[{flav}] SIGNAL CHECK")

                for mass in list(sub["signal_combined_mass"][flav].keys())[:3]:

                    S = sub["signal_combined_mass"][flav][mass]

                    print(f"  Mass {mass}: sum={S.sum():.4f}, min={S.min():.4e}, max={S.max():.4e}")

            # ----------------------------------
            # Norm check (NEW - VERY IMPORTANT)
            # ----------------------------------
            print("\n[NORM CHECK]")
            for mass in list(sub["norm"].keys())[:5]:
                print(f"  Mass {mass}: norm = {sub['norm'][mass]:.6f}")

            # ----------------------------------
            # Quick total background sanity
            # ----------------------------------
            totalB = sum(
                sub["background"][flav][era].sum()
                for flav in FLAVOURS
                for era in ERAS
            )

            print(f"\n[TOTAL BKG] = {totalB:.3f}")
            

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


def print_sr3_bin_table(data, flav, mass):

    print("\n========================================================")
    print(f" BIN TABLE (SR3) | {flav} | mass={mass}")
    print("========================================================")

    header = (
        "Bin | Era       | Jet     | MET   | LT range        | "
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

        boundary = get_met_boundary(flav, era)

        print("\n--------------------------------------------------------")
        print(f" ERA: {era}   (MET boundary = {boundary})")
        print("--------------------------------------------------------")

        bin_counter = 0
        total_B_era = 0.0

        cats = [
            f"LowJet_LT_MET{boundary}_LTcut",
            f"HighJet_LT_MET{boundary}_LTcut",
            f"LowJet_LT_MET{boundary}_GTcut",
            f"HighJet_LT_MET{boundary}_GTcut",
        ]

        for cat in cats:

            sub = data[boundary][cat]

            jet, is_ltcut = parse_sr3_category(cat)
            met_label = "<" if is_ltcut else ">="

            edges_full = sub["edges"]
            bin_lo = edges_full[:-1]

            edges_ref = get_sr3_ref_edges(flav, era, jet, is_ltcut)

            S_arr = sub["signal_nonorm"][flav][mass][era]
            B_arr = sub["background"][flav][era]
            F_arr = sub["fake"][flav][era]
            E_arr = sub["bkg_err2"][flav][era]

            for i in range(len(edges_ref) - 1):

                lo = edges_ref[i]
                hi = edges_ref[i+1]

                if i == len(edges_ref) - 2:
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
                    f"{bin_counter:3d} | {era:9s} | {jet:7s} | {met_label:3s}{boundary:2s} | "
                    f"[{lo:5.0f},{hi:5.0f}] | "
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
        "Bin | Jet     | MET   | LT range        | "
        "   S      B      rel_unc     Z"
    )
    print(header)
    print("-" * len(header))

    bin_counter = 0

    # assume same boundary structure across eras
    boundary = get_met_boundary(flav, ERAS[0])

    cats = [
        f"LowJet_LT_MET{boundary}_LTcut",
        f"HighJet_LT_MET{boundary}_LTcut",
        f"LowJet_LT_MET{boundary}_GTcut",
        f"HighJet_LT_MET{boundary}_GTcut",
    ]

    for cat in cats:

        print("\n--------------------------------------------------------")
        print(f" REGION: {cat}")
        print("--------------------------------------------------------")

        sub = data[boundary][cat]

        jet, is_ltcut = parse_sr3_category(cat)
        met_label = "<" if is_ltcut else ">="

        edges_full = sub["edges"]
        bin_lo = edges_full[:-1]

        # use reference edges from first era (structure only)
        edges_ref = get_sr3_ref_edges(flav, ERAS[0], jet, is_ltcut)

        for i in range(len(edges_ref) - 1):

            lo = edges_ref[i]
            hi = edges_ref[i+1]

            if i == len(edges_ref) - 2:
                mask = (bin_lo >= lo) & (bin_lo <= hi)
            else:
                mask = (bin_lo >= lo) & (bin_lo < hi)

            S = 0.0
            B = 0.0
            E = 0.0

            for era in ERAS:

                S_arr = sub["signal_nonorm"][flav][mass][era]
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

            print(f"{bin_counter:3d} | {jet:7s} | {met_label:3s}{boundary:2s} | "
                  f"[{lo:5.0f},{hi:5.0f}] | "
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

