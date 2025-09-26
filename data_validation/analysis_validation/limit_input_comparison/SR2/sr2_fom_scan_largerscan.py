import ROOT
import os
import math
import itertools
import argparse
import time
import random
from tqdm import tqdm


n_samples = 2000000


# -------------------------------
# FOM + helpers
# -------------------------------
def calculate_fom(s, b, mass):
    if mass in ["500", "600", "700"]:
        s /= 10
    if b > 0 and s > 0:
        return math.sqrt(2 * ((s + b) * math.log(1 + s / b) - s))
    return math.sqrt(2 * s) if s > 0 else 0.0

def get_bin_info(hist):
    bins = []
    for i in range(1, hist.GetNbinsX() + 1):
        x_low = hist.GetBinLowEdge(i)
        x_high = hist.GetBinLowEdge(i + 1)
        bkg = hist.GetBinContent(i)
        err = hist.GetBinError(i)
        rel_err = (err / bkg) if bkg > 0 else float('inf')
        bins.append((i, x_low, x_high, bkg, rel_err))
    return bins

def build_sig_bin_cache(sig_hist):
    return {
        sig_hist.GetBinLowEdge(i): sig_hist.GetBinContent(i)
        for i in range(1, sig_hist.GetNbinsX() + 1)
    }

def strict_merge_subrange(sub_bins, sig_bin_cache, mass):
    acc_bkg = 0.0
    acc_sig = 0.0
    err2_sum = 0.0
    for _, x_low, x_high, bkg, rel_err in sub_bins:
        acc_bkg += bkg
        if math.isfinite(rel_err):
            err2_sum += (bkg * rel_err) ** 2
        sig = sum(val for bx, val in sig_bin_cache.items() if x_low <= bx < x_high)
        acc_sig += sig
    rel_err = math.sqrt(err2_sum) / acc_bkg if acc_bkg > 0 else float('inf')
    fom = calculate_fom(acc_sig, acc_bkg, mass)
    return acc_bkg, acc_sig, rel_err, fom

def bin_is_valid(bkg, rel_err):
    # Require bkg > 0.5 always, and also (bkg > 1.0 OR rel_err < 0.3)
    return (bkg > 0.5) and (bkg > 1.0 or rel_err < 0.3)

# ------------------------------------------
# Greedy/Exhaustive without pre-picked bins
# ------------------------------------------
def greedy_binning_simple(bins, sig_hist, n_bins_total, mass, log_print, label):
    log_print(f"[INFO] (Greedy:{label}) Starting greedy binning for {n_bins_total} bins")
    sig_bin_cache = build_sig_bin_cache(sig_hist)

    x_min = bins[0][1]
    x_max = bins[-1][2]
    candidate_edges = sorted(set([b[1] for b in bins if x_min < b[1] < x_max]))

    best_edges = []
    prev_edge = x_min

    for _ in tqdm(range(n_bins_total - 1), desc=f"Greedy ({label})"):
        best_local_fom = -1.0
        best_cut = None
        for edge in candidate_edges:
            if edge <= prev_edge or edge >= x_max:
                continue
            subrange = [b for b in bins if prev_edge <= b[1] < edge]
            if not subrange:
                continue
            bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_bin_cache, mass)
            if not bin_is_valid(bkg, rel_err):
                continue
            if fom > best_local_fom:
                best_local_fom = fom
                best_cut = edge
        if best_cut is None:
            log_print(f"[WARN] (Greedy:{label}) No valid cut found; aborting greedy binning.")
            return None, None
        best_edges.append(best_cut)
        prev_edge = best_cut

    full_edges = [x_min] + best_edges + [x_max]
    total_fom = 0.0
    #log_print(f"  Bin-by-bin summary (Greedy:{label}):")
    for i in range(len(full_edges) - 1):
        lo = full_edges[i]
        hi = full_edges[i + 1]
        subrange = [b for b in bins if lo <= b[1] < hi]
        bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_bin_cache, mass)
        total_fom += fom
        #log_print(f"    Bin {i+1}: Range=[{lo:.1f},{hi:.1f}) | Bkg={bkg:.2f}, Sig={sig:.2f}, FOM={fom:.2f}, RelErr={rel_err:.2f}")
    log_print(f"[INFO] (Greedy:{label}) Total FOM with {n_bins_total} bins: {total_fom:.2f}")
    return total_fom, full_edges

def exhaustive_binning_simple(bins, sig_hist, n_bins_total, mass, log_print, label):
    start_time = time.time()
    sig_bin_cache = build_sig_bin_cache(sig_hist)

    x_min = bins[0][1]
    x_max = bins[-1][2]

    #all_edges = sorted(set([b[1] for b in bins if x_min < b[1] < x_max]))
    all_edges = sorted(set([b[1] for b in bins] + [b[2] for b in bins]))
    all_edges = [e for e in all_edges if x_min < e < x_max]
    if n_bins_total <= 1:
        log_print(f"[WARN] (Exhaustive:{label}) n_bins_total must be >= 2")
        return None, None

    all_combos = list(itertools.combinations(all_edges, n_bins_total - 1))
    #if n_bins_total >= 5 and len(all_combos) > n_samples:
    #    log_print(f"[INFO] (Exhaustive:{label}) Sampling 200000 combinations out of {len(all_combos)}")
    #    all_combos = random.sample(all_combos, n_samples)

    best_fom = -1.0
    best_edges = None

    for edges in tqdm(all_combos, desc=f"Exhaustive ({label})"):
        edges = [x_min] + sorted(edges) + [x_max]
        total_fom = 0.0
        valid = True
        for i in range(len(edges) - 1):
            lo = edges[i]
            hi = edges[i + 1]
            if (hi - lo) < 0.1:
                valid = False
                break
            subrange = [b for b in bins if lo <= b[1] < hi]
            if not subrange:
                valid = False
                break
            bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_bin_cache, mass)
            if not bin_is_valid(bkg, rel_err):
                valid = False
                break
            total_fom += fom
        if not valid:
            continue
        if total_fom > best_fom:
            best_fom = total_fom
            best_edges = edges

    total_time = time.time() - start_time
    log_print(f"[RESULT] (Exhaustive:{label}) Completed in {total_time:.2f} s")
    if best_edges is not None:
        log_print(f"  Best Total FOM = {best_fom:.2f} with edges: {best_edges}")
        #log_print(f"  Bin-by-bin summary (Exhaustive:{label}):")
        for i in range(len(best_edges) - 1):
            lo = best_edges[i]
            hi = best_edges[i + 1]
            subrange = [b for b in bins if lo <= b[1] < hi]
            bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_bin_cache, mass)
            #log_print(f"    Bin {i+1}: Range=[{lo:.1f},{hi:.1f}) | Bkg={bkg:.2f}, Sig={sig:.2f}, FOM={fom:.2f}, RelErr={rel_err:.2f}")
        return best_fom, best_edges
    else:
        log_print(f"[WARN] (Exhaustive:{label}) No valid configuration found for {n_bins_total} bins.")
        return None, None

# ------------------------------------------
# EXISTING FOM over SR2 (updated paths)
# ------------------------------------------
def compute_existing_total_fom_allbins(sig_file, bkg_file, flavour, mass):
    path_map = {
        "MuMu": "LimitExtraction/HNL_ULIDv2/MuMu/LimitBins/MuonSR2",
        "EE":   "LimitExtraction/HNL_ULIDv2/EE/LimitBins/ElectronSR2",
        "EMu":  "LimitExtraction/HNL_ULIDv2/EMu/LimitBins/ElectronMuonSR2",
    }
    path = path_map.get(flavour)
    if not path:
        return 0.0
    sig_hist = sig_file.Get(path)
    bkg_hist = bkg_file.Get(path)
    if not sig_hist or not isinstance(sig_hist, ROOT.TH1):
        return 0.0
    if not bkg_hist or not isinstance(bkg_hist, ROOT.TH1):
        return 0.0
    total_fom = 0.0
    if mass =="0":
        sig_hist.Scale(100000)
    for i in range(1, bkg_hist.GetNbinsX() + 1):
        b = bkg_hist.GetBinContent(i)
        s = sig_hist.GetBinContent(i)
        total_fom += calculate_fom(s, b, mass)
    return total_fom

def get_existing_fom(sig_file, bkg_file, flavour, mass, log_print):
    # Detailed per-bin printout WITH relative errors (now SR2)
    path_map = {
        "MuMu": "LimitExtraction/HNL_ULIDv2/MuMu/LimitBins/MuonSR2",
        "EE":   "LimitExtraction/HNL_ULIDv2/EE/LimitBins/ElectronSR2",
        "EMu":  "LimitExtraction/HNL_ULIDv2/EMu/LimitBins/ElectronMuonSR2",
    }
    path = path_map.get(flavour)
    if not path:
        log_print(f"[WARN] No FOM path mapping for flavour: {flavour}")
        return

    sig_hist = sig_file.Get(path)
    bkg_hist = bkg_file.Get(path)

    if not sig_hist or not isinstance(sig_hist, ROOT.TH1):
        log_print(f"[WARN] Could not load signal histogram at: {path}")
        return
    if not bkg_hist or not isinstance(bkg_hist, ROOT.TH1):
        log_print(f"[WARN] Could not load background histogram at: {path}")
        return
    if mass =="0":
        sig_hist.Scale(100000)

    total_allbins = 0.0
    log_print(f"[INFO] Bin-by-bin FOMs from {path}:")
    for i in range(1, bkg_hist.GetNbinsX() + 1):
        b = bkg_hist.GetBinContent(i)
        s = sig_hist.GetBinContent(i)
        err = bkg_hist.GetBinError(i)
        rel_err = (err / b) if b > 0 else float('inf')
        fom = calculate_fom(s, b, mass)
        total_allbins += fom
        if math.isfinite(rel_err):
            log_print(f"  Bin {i:2d}: Bkg={b:.2f}, Sig={s:.2f}, FOM={fom:.2f}, RelErr={rel_err:.2f}")
        else:
            log_print(f"  Bin {i:2d}: Bkg={b:.2f}, Sig={s:.2f}, FOM={fom:.2f}, RelErr=inf")

    log_print(f"[INFO] Total EXISTING FOM over all bins (SR2): {total_allbins:.2f}")

# --------------------------------------------------
# Evaluation: apply fixed edges per region across masses
# --------------------------------------------------
def evaluate_binning_for_masses_dual(selected_dir, era, flavour,
                                     h_low, h_high,
                                     edges_low, edges_high,
                                     log_print, bkg_file):
    masses = ["0",  "SSWW_600", "SSWW_1000",  "SSWW_20000"]
    per_mass = {}
    summed_fom = 0.0
    summed_existing_all = 0.0
    summed_ratios = 0.0

    bins_low = get_bin_info(h_low)
    bins_high = get_bin_info(h_high)

    for m in masses:
        sig_sample = "Weinberg" if m == "0" else f"HNL_{m}"
        sig_path = os.path.join(selected_dir, era, f"HNL_SignalRegion_Plotter_{sig_sample}.root")
        f_sig_tmp = ROOT.TFile(sig_path)


        base_sig_path = f"PassSR2/HNL_ULIDv2/{flavour}/MainPlots"
        sig_low = f_sig_tmp.Get(f"{base_sig_path}/HT_PT1_LowDPhi")
        sig_high = f_sig_tmp.Get(f"{base_sig_path}/HT_PT1_HighDphi")
        if not sig_low or not sig_high:
            log_print(f"[EVAL][WARN] Missing signal histogram(s) for mass {m}")
            f_sig_tmp.Close()
            continue

        if m == "0":
            sig_low.Scale(100000.0)
            sig_high.Scale(100000.0)
            
        sig_cache_low = build_sig_bin_cache(sig_low)
        sig_cache_high = build_sig_bin_cache(sig_high)

        total_fom_m = 0.0

        # Low dphi
        for i in range(len(edges_low) - 1):
            lo = edges_low[i]
            hi = edges_low[i + 1]
            subrange = [b for b in bins_low if lo <= b[1] < hi]
            if not subrange:
                continue
            _, _, _, fom = strict_merge_subrange(subrange, sig_cache_low, m)
            total_fom_m += fom

        # High dphi
        for i in range(len(edges_high) - 1):
            lo = edges_high[i]
            hi = edges_high[i + 1]
            subrange = [b for b in bins_high if lo <= b[1] < hi]
            if not subrange:
                continue
            _, _, _, fom = strict_merge_subrange(subrange, sig_cache_high, m)
            total_fom_m += fom

        # Existing totals over ALL SR2 bins
        existing_all = compute_existing_total_fom_allbins(f_sig_tmp, bkg_file, flavour, m)

        ratio = (total_fom_m / existing_all) if existing_all > 0 else float('inf')

        per_mass[m] = (total_fom_m, existing_all, ratio)
        summed_fom += total_fom_m
        summed_existing_all += existing_all
        summed_ratios += ratio

        log_print(f"[EVAL] Mass {m}: Total FOM={total_fom_m:.2f} | Existing={existing_all:.2f} | Ratio={ratio:.2f}")

        f_sig_tmp.Close()

    log_print(f"[EVAL] SUMMED FOM over masses {masses}: {summed_fom:.2f}")
    log_print(f"[EVAL] SUMMED EXISTING FOM (all bins, SR2) over masses {masses}: {summed_existing_all:.2f}")
    log_print(f"[EVAL] SUM OF RATIOS over masses {masses}: {summed_ratios:.2f}")
    return per_mass, summed_fom, summed_existing_all


# --- Helper to evaluate one scheme ---
def evaluate_manual_binning(name, edges_low, edges_high, bins_low, bins_high, sig_low, sig_high, mass, log_print):
    log_print(f"\n[EVAL][MANUAL] Evaluating predefined binning: {name}")

    total_fom = 0.0

    # Low dphi
    sig_cache_low = build_sig_bin_cache(sig_low)
    log_print("[EVAL][MANUAL] Low dphi bin details:")
    low_fom_total = 0.0

    for i in range(len(edges_low) - 1):
        lo, hi = edges_low[i], edges_low[i+1]
        subrange = [b for b in bins_low if lo <= b[1] < hi]
        bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_cache_low, mass)
        low_fom_total += fom
        total_fom += fom
        log_print(f"  Bin {i+1}: Range=[{lo:.1f},{hi:.1f}) | "
                  f"Bkg={bkg:.2f}, Sig={sig:.2f}, FOM={fom:.2f}, RelErr={rel_err:.2f}")
    log_print(f"[EVAL][MANUAL] Total FOM (Low dphi) = {low_fom_total:.2f}")

    # High dphi
    sig_cache_high = build_sig_bin_cache(sig_high)
    log_print("[EVAL][MANUAL] High dphi bin details:")
    high_fom_total = 0.0
    for i in range(len(edges_high) - 1):
        lo, hi = edges_high[i], edges_high[i+1]
        subrange = [b for b in bins_high if lo <= b[1] < hi]
        bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_cache_high, mass)
        high_fom_total += fom
        total_fom += fom
        log_print(f"  Bin {i+1}: Range=[{lo:.1f},{hi:.1f}) | "
                  f"Bkg={bkg:.2f}, Sig={sig:.2f}, FOM={fom:.2f}, RelErr={rel_err:.2f}")

    log_print(f"[EVAL][MANUAL] Total FOM (High dphi) = {high_fom_total:.2f}")

    log_print(f"[EVAL][MANUAL] Total FOM across all predefined bins = {total_fom:.2f}")
    return total_fom


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--mass', default='500')
    parser.add_argument('--flavour', default='MuMu')
    parser.add_argument('--era', default='2016preVFP')
    parser.add_argument('--n_bins', type=int, default=2,
                        help="Total number of bins (Low+High dphi). High gets the extra when odd.")
    args = parser.parse_args()

    base_dir = "/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter"
    selected_dir = max([
        os.path.join(base_dir, d)
        for d in os.listdir(base_dir)
        if os.path.isdir(os.path.join(base_dir, d))
    ], key=os.path.getmtime)

    sig_sample = "Weinberg" if args.mass == "0" else f"HNL_{args.mass}"
    sig_file_path = os.path.join(selected_dir, args.era, f"HNL_SignalRegion_Plotter_{sig_sample}.root")
    bkg_file_path = os.path.join(selected_dir, args.era, "HNL_SignalRegion_Plotter_Bkg.root")

    f_sig = ROOT.TFile(sig_file_path)
    f_bkg = ROOT.TFile(bkg_file_path)

    # Two scan histograms (SR2)
    base_path_scan = f"PassSR2/HNL_ULIDv2/{args.flavour}/MainPlots"
    hname_low = "HT_PT1_LowDPhi"
    hname_high = "HT_PT1_HighDphi"

    sig_low = f_sig.Get(f"{base_path_scan}/{hname_low}")
    sig_high = f_sig.Get(f"{base_path_scan}/{hname_high}")
    bkg_low = f_bkg.Get(f"{base_path_scan}/{hname_low}")
    bkg_high = f_bkg.Get(f"{base_path_scan}/{hname_high}")

    if not bkg_low or not isinstance(bkg_low, ROOT.TH1):
        print(f"[ERROR] Could not load histogram: {base_path_scan}/{hname_low} from {bkg_file_path}")
        return
    if not bkg_high or not isinstance(bkg_high, ROOT.TH1):
        print(f"[ERROR] Could not load histogram: {base_path_scan}/{hname_high} from {bkg_file_path}")
        return
    if not sig_low or not isinstance(sig_low, ROOT.TH1):
        print(f"[ERROR] Could not load histogram: {base_path_scan}/{hname_low} from {sig_file_path}")
        return
    if not sig_high or not isinstance(sig_high, ROOT.TH1):
        print(f"[ERROR] Could not load histogram: {base_path_scan}/{hname_high} from {sig_file_path}")
        return

    if args.mass == "0":
        sig_low.Scale(100000.0)
        sig_high.Scale(100000.0)
    
    # Logging
    os.makedirs("log_scan", exist_ok=True)
    log_file = f"log_scan/scan_{args.era}_{args.flavour}_SR2_M{args.mass}_{args.n_bins}_HTPT1.log"

    # --- Define multiple predefined schemes ---
    predefined_binnings = {
        "SchemeA": {
            "low":  [0.0, 1.0, 2., 10.0],
            "high": [0.0, 1.0, 2., 10.0],
        },
        "SchemeB": {
            "low":  [0.0, 1., 2.5, 10.0],
            "high": [0.0, 1., 2.5, 10.0],
        },
        "SchemeC": {
            "low":  [0.0, 1, 3, 10.0],
            "high": [0.0, 1, 3, 10.0],
        },
        "SchemeD": {
            "low":  [0.0, 1.5, 2.,  10.0],
            "high": [0.0, 1.5, 2., 10.0],
        },

        "SchemeE": {
            "low":  [0.0, 1.5, 2.5,  10.0],
            "high": [0.0, 1.5, 2.5, 10.0],
        },
        "SchemeF": {
            "low":  [0.0, 1.5, 3.0,  10.0],
            "high": [0.0, 1.5, 3.0, 10.0],
        },
        
        "SchemeG": {
            "low":  [0.0, 1.5, 3.5, 10.0],
            "high": [0.0, 1.5, 3.5, 10.0],
        },
        
        "SchemeH": {
            "low":  [0.0, 1.5, 4., 10.0],
            "high": [0.0, 1.5, 4., 10.0],
        },
        "SchemeI": {
            "low":  [0.0, 2., 2.5, 10.0],
            "high": [0.0, 2., 2.5, 10.0],
        },
        "SchemeJ": {
            "low":  [0.0, 2., 3.0, 10.0],
            "high": [0.0, 2., 3.0, 10.0],
        },
        "SchemeK": {
            "low":  [0.0, 2., 4.0, 10.0],
            "high": [0.0, 2., 4.0, 10.0],
        },

    }
    
    with open(log_file, 'w') as log:
        def log_print(msg):
            print(msg, flush=True)
            log.write(msg + "\n")

        log_print(f"[INFO] Running SR2 dual-scan for MASS={args.mass}, FLAV={args.flavour}, ERA={args.era}, NBIN(total)={args.n_bins}")
        # Existing SR2 per-bin dump
        get_existing_fom(f_sig, f_bkg, args.flavour, args.mass, log_print)

        bins_low = get_bin_info(bkg_low)
        bins_high = get_bin_info(bkg_high)
        logs_final = []


        for scheme_name, edges in predefined_binnings.items():
            evaluate_manual_binning(
                scheme_name,
                edges["low"], edges["high"],
                bins_low, bins_high,
                sig_low, sig_high,
                args.mass, log_print
            )

        
        
        # Decide bin split: High gets the extra bin if odd
        n_high = (args.n_bins + 1) // 2
        n_low = args.n_bins // 2
        log_print(f"[INFO] Bin split -> Low dphi: {n_low} bins, High dphi: {n_high} bins")


        # ---- Low dphi scan ----
        edges_low = [bins_low[0][1], bins_low[-1][2]]
        if n_low >= 2:
            log_print(f"\n[INFO] Scanning Low dphi with {n_low} bins:")
            #g_fom_low, g_edges_low = greedy_binning_simple(bins_low, sig_low, n_low, args.mass, log_print, "Low dphi")
            e_fom_low, e_edges_low = exhaustive_binning_simple(bins_low, sig_low, n_low, args.mass, log_print, "Low dphi")
            if e_edges_low is not None:
                edges_low = e_edges_low
                log_print(f"[EVAL] Low dphi using Exhaustive edges: {edges_low}")
            #elif g_edges_low is not None:
            #    edges_low = g_edges_low
            #    log_print(f"[EVAL] Low dphi using Greedy edges: {edges_low}")
            else:
                log_print("[WARN] Low dphi: No valid binning found; single bin used.")
            sig_cache_low = build_sig_bin_cache(sig_low)
            log_print(f"[EVAL][FINAL] Low dphi bin details:")
            for i in range(len(edges_low) - 1):
                lo = edges_low[i]
                hi = edges_low[i + 1]
                subrange = [b for b in bins_low if lo <= b[1] < hi]
                bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_cache_low, args.mass)
                logs_final.append(f"  Bin {i+1}: Range=[{lo:.1f},{hi:.1f}) | Bkg={bkg:.2f}, Sig={sig:.2f}, FOM={fom:.2f}, RelErr={rel_err:.2f}")                                                                                                                                               
                log_print(f"  Bin {i+1}: Range=[{lo:.1f},{hi:.1f}) | "
                          f"Bkg={bkg:.2f}, Sig={sig:.2f}, FOM={fom:.2f}, RelErr={rel_err:.2f}")
        else:
            log_print(f"[INFO] Low dphi uses a single full-range bin.")

        # ---- High dphi scan ----
        edges_high = [bins_high[0][1], bins_high[-1][2]]
        if n_high >= 2:
            log_print(f"\n[INFO] Scanning High dphi with {n_high} bins:")
            #g_fom_high, g_edges_high = greedy_binning_simple(bins_high, sig_high, n_high, args.mass, log_print, "High dphi")
            e_fom_high, e_edges_high = exhaustive_binning_simple(bins_high, sig_high, n_high, args.mass, log_print, "High dphi")
            if e_edges_high is not None:
                edges_high = e_edges_high
                log_print(f"[EVAL] High dphi using Exhaustive edges: {edges_high}")
            #elif g_edges_high is not None:
            #    edges_high = g_edges_high
            #    log_print(f"[EVAL] High dphi using Greedy edges: {edges_high}")
            else:
                log_print("[WARN] High dphi: No valid binning found; single bin used.")
            sig_cache_high = build_sig_bin_cache(sig_high)
            log_print(f"[EVAL][FINAL] High dphi bin details:")
            for i in range(len(edges_high) - 1):
                lo = edges_high[i]
                hi = edges_high[i + 1]
                subrange = [b for b in bins_high if lo <= b[1] < hi]
                bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_cache_high, args.mass)
                logs_final.append(f"  Bin {i+1}: Range=[{lo:.1f},{hi:.1f}) | Bkg={bkg:.2f}, Sig={sig:.2f}, FOM={fom:.2f}, RelErr={rel_err:.2f}")                                                                                                                                               
                log_print(f"  Bin {i+1}: Range=[{lo:.1f},{hi:.1f}) | "
                          f"Bkg={bkg:.2f}, Sig={sig:.2f}, FOM={fom:.2f}, RelErr={rel_err:.2f}")
        else:
            log_print(f"[INFO] High dphi uses a single full-range bin.")


        # ---- Combined summary ----
        combined_fom = 0.0
        log_print("\n[EVAL][FINAL][COMBINED] Binning summary:")

        # Low dphi contribution
        sig_cache_low = build_sig_bin_cache(sig_low)
        for i in range(len(edges_low) - 1):
            lo = edges_low[i]
            hi = edges_low[i + 1]
            subrange = [b for b in bins_low if lo <= b[1] < hi]
            bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_cache_low, args.mass)
            combined_fom += fom

        # High dphi contribution
        sig_cache_high = build_sig_bin_cache(sig_high)
        for i in range(len(edges_high) - 1):
            lo = edges_high[i]
            hi = edges_high[i + 1]
            subrange = [b for b in bins_high if lo <= b[1] < hi]
            bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_cache_high, args.mass)
            combined_fom += fom

        # Final print
        log_print(f"  Low dphi edges : {edges_low}")
        log_print(f"  High dphi edges: {edges_high}")
        log_print(f"  --> Combined Total FOM = {combined_fom:.2f}")

        for log_binning in logs_final:
            print (log_binning)
        
        # ---- Cross-mass evaluation ----
        log_print("\n[EVAL] Cross-mass evaluation using fixed edges per region:")
        log_print(f"[EVAL] Fixed edges (Low dphi):  {edges_low}")
        log_print(f"[EVAL] Fixed edges (High dphi): {edges_high}")
        evaluate_binning_for_masses_dual(
            selected_dir, args.era, args.flavour,
            bkg_low, bkg_high,
            edges_low, edges_high,
            log_print, f_bkg
        )

    f_sig.Close()
    f_bkg.Close()

if __name__ == "__main__":
    main()        
