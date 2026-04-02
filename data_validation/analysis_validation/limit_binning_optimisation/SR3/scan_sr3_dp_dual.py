#!/usr/bin/env python3
import ROOT
import os
import math
import argparse
import time
import numpy as np
from tqdm import tqdm

# -------------------------
# FOM and histogram helpers
# -------------------------

def calculate_fom(s, b, mass):
    if b > 0.0 and s > 0.0:
        return (2.0 * ((s + b) * math.log(1.0 + s / b) - s))
        # return math.sqrt(2.0 * ((s + b) * math.log(1.0 + s / b) - s))
    return math.sqrt(2.0 * s) if s > 0.0 else 0.0


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


def extract_edges_and_arrays(bkg_hist, sig_hist):
    nb = bkg_hist.GetNbinsX()
    edges = np.array([bkg_hist.GetBinLowEdge(i) for i in range(1, nb + 2)], dtype=float)
    bkg   = np.array([bkg_hist.GetBinContent(i) for i in range(1, nb + 1)], dtype=float)
    err   = np.array([bkg_hist.GetBinError(i)   for i in range(1, nb + 1)], dtype=float)
    # If bkg == 0, do not add variance
    err2  = np.where(bkg > 0.0, err * err, 0.0)
    sig   = np.array([sig_hist.GetBinContent(i) for i in range(1, nb + 1)], dtype=float)
    return edges, bkg, err2, sig


def prefix_sums(arr):
    ps = np.zeros(len(arr) + 1, dtype=float)
    if len(arr) > 0:
        ps[1:] = np.cumsum(arr)
    return ps


def strict_merge_from_right(bins, min_bkg=1.0, max_rel_err=0.3):
    acc_bkg = 0.0
    err2_sum = 0.0
    right_idx = len(bins) - 1
    for i in range(len(bins) - 1, -1, -1):
        _, _, _, bkg, rel_err = bins[i]
        acc_bkg += bkg
        if math.isfinite(rel_err):
            err2_sum += (bkg * rel_err) ** 2
        rel_err_acc = math.sqrt(err2_sum) / acc_bkg if acc_bkg > 0 else float('inf')
        if acc_bkg > 0.5 and (acc_bkg > min_bkg or rel_err_acc < max_rel_err):
            right_idx = i
            break

    right_chunk = bins[right_idx:]
    remaining = bins[:right_idx]
    x_last_min = right_chunk[0][1]
    x_upper = right_chunk[-1][2]
    return right_chunk, remaining, x_last_min, x_upper


# -------------------------
# DP scan core
# -------------------------

def build_valid_matrix(edges, PB, PE, min_width=20.0, min_bkg=0.5, strong_bkg=1.0, max_rel_err=0.3):
    M = len(edges) - 1
    valid = np.zeros((M + 1, M + 1), dtype=np.bool_)
    for p in range(0, M):
        i_start = p + 1
        while i_start <= M and (edges[i_start] - edges[p] < min_width):
            i_start += 1
        if i_start > M:
            continue
        for i in range(i_start, M + 1):
            B = PB[i] - PB[p]
            if B < min_bkg:
                continue
            E = PE[i] - PE[p]
            relErr = (math.sqrt(E) / B) if B > 0 else float('inf')
            if (B >= strong_bkg) or (relErr <= max_rel_err):
                valid[p, i] = True
    return valid


def dp_optimal_bins(edges, PB, PE, PS_massscaled, n_bins_total, x_last_min,
                    min_width=20.0, min_bkg=0.5, strong_bkg=1.0, max_rel_err=0.3,
                    log_print=None):
    M = len(edges) - 1
    if M <= 0 or n_bins_total <= 0:
        return None, None

    valid = build_valid_matrix(edges, PB, PE, min_width, min_bkg, strong_bkg, max_rel_err)

    def interval_fom(p, i):
        B = PB[i] - PB[p]
        S = PS_massscaled[i] - PS_massscaled[p]
        if B <= 0.0:
            return None
        return calculate_fom(S, B, mass="NA")

    NEG = -1e300
    dp = np.full((n_bins_total + 1, M + 1), NEG, dtype=float)
    prv = np.full((n_bins_total + 1, M + 1), -1, dtype=int)
    dp[0, 0] = 0.0

    idx_last_min = int(np.searchsorted(edges, x_last_min, side='right') - 1)
    idx_last_min = max(0, min(idx_last_min, M - 1))

    for j in range(1, n_bins_total + 1):
        for i in range(1, M + 1):
            if j == n_bins_total:
                p_max_allowed = min(i - 1, idx_last_min)
            else:
                p_max_allowed = i - 1

            best_val = NEG
            best_p = -1
            if (j < n_bins_total) and (i == M):
                continue
            for p in range(0, p_max_allowed + 1):
                if dp[j - 1, p] <= NEG / 2:
                    continue
                if not valid[p, i]:
                    continue
                fom = interval_fom(p, i)
                if fom is None:
                    continue
                val = dp[j - 1, p] + fom
                if val > best_val:
                    best_val = val
                    best_p = p
            dp[j, i] = best_val
            prv[j, i] = best_p

    best_fom = dp[n_bins_total, M]
    if best_fom <= NEG / 2:
        if log_print:
            log_print("[WARN] DP: No feasible solution found under constraints.")
        return None, None

    # Backtrack
    cut_idxs = []
    j = n_bins_total
    i = M
    while j > 0:
        p = prv[j, i]
        cut_idxs.append(p)
        i = p
        j -= 1
    cut_idxs = list(reversed(cut_idxs))
    best_edges = [edges[0]] + [edges[idx] for idx in cut_idxs[1:]] + [edges[-1]]

    # Pretty print
    if log_print:
        log_print(f"[RESULT] DP scan completed")
        log_print(f"  Best Total FOM = {best_fom:.2f} with edges: {best_edges}")
        log_print(f"  Bin-by-bin summary (DP):")
        idxs = [int(np.argmin(np.abs(edges - e))) for e in best_edges]
        for k in range(len(idxs) - 1):
            p = idxs[k]
            q = idxs[k + 1]
            lo = edges[p]
            hi = edges[q]
            B = PB[q] - PB[p]
            E = PE[q] - PE[p]
            S = PS_massscaled[q] - PS_massscaled[p]
            relErr = (math.sqrt(E) / B) if B > 0 else float('inf')
            fom = calculate_fom(S, B, mass="NA")
            if math.isfinite(relErr):
                log_print(f"    Bin {k+1}: Range = [{lo:.1f}, {hi:.1f}) | "
                          f"Bkg = {B:.2f}, Sig = {S:.2f}, FOM = {fom:.2f}, RelErr = {relErr:.2f}")
            else:
                log_print(f"    Bin {k+1}: Range = [{lo:.1f}, {hi:.1f}) | "
                          f"Bkg = {B:.2f}, Sig = {S:.2f}, FOM = {fom:.2f}, RelErr = inf")

    return best_fom, best_edges


# -------------------------
# Existing FOM utilities
# -------------------------

def compute_existing_total_fom_allbins(sig_file, bkg_file, flavour, mass):
    path_map = {
        "MuMu": "LimitExtraction/HNL_ULIDv2/MuMu/LimitBins/MuonSR3",
        "EE":   "LimitExtraction/HNL_ULIDv2/EE/LimitBins/ElectronSR3",
        "EMu":  "LimitExtraction/HNL_ULIDv2/EMu/LimitBins/ElectronMuonSR3",
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
    for i in range(1, bkg_hist.GetNbinsX() + 1):
        b = bkg_hist.GetBinContent(i)
        s = sig_hist.GetBinContent(i)
        total_fom += calculate_fom(s, b, mass)
    return total_fom


def get_existing_fom(sig_file, bkg_file, flavour, mass, log_print):
    path_map = {
        "MuMu": "LimitExtraction/HNL_ULIDv2/MuMu/LimitBins/MuonSR3",
        "EE":   "LimitExtraction/HNL_ULIDv2/EE/LimitBins/ElectronSR3",
        "EMu":  "LimitExtraction/HNL_ULIDv2/EMu/LimitBins/ElectronMuonSR3",
    }
    path = path_map.get(flavour)
    sig_hist = sig_file.Get(path)
    bkg_hist = bkg_file.Get(path)
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
            log_print(f"  Bin {i:2d}: Bkg = {b:.2f}, Sig = {s:.2f}, "
                      f"FOM = {fom:.2f}, RelErr = {rel_err:.2f}")
        else:
            log_print(f"  Bin {i:2d}: Bkg = {b:.2f}, Sig = {s:.2f}, "
                      f"FOM = {fom:.2f}, RelErr = inf")
    log_print(f"[INFO] Total EXISTING FOM over all bins: {total_allbins:.2f}")


# -------------------------
# Cross-mass evaluation
# -------------------------

def evaluate_binning_for_masses(selected_dir, era, flavour, base_path, hpath,
                                edges, bkg, err2, best_edges, log_print, f_bkg, sig_tag):
    masses = ["400", "500", "600", "700", "800", "900", "1000"]
    PB = prefix_sums(bkg)
    cut_indices = [int(np.argmin(np.abs(edges - e))) for e in best_edges]

    for m in masses:
        sig_path = os.path.join(selected_dir, era, f"HNL_SignalRegion_Plotter_HNL_{sig_tag}{m}.root")
        f_sig_tmp = ROOT.TFile(sig_path)
        sig_hist = f_sig_tmp.Get(f"{base_path}/HNL_ULIDv2/{flavour}/{hpath}")
        if not sig_hist:
            f_sig_tmp.Close()
            continue
        bkg_hist_for_shape = f_bkg.Get(f"{base_path}/HNL_ULIDv2/{flavour}/{hpath}")
        _, _, _, sig_arr = extract_edges_and_arrays(bkg_hist_for_shape, sig_hist)
        sig_arr = sig_arr[:len(bkg)]
        PS = prefix_sums(sig_arr.copy())
        PB_local = prefix_sums(np.array(bkg, dtype=float))
        total_fom_m = 0.0
        for i in range(len(cut_indices) - 1):
            p = cut_indices[i]
            q = cut_indices[i + 1]
            B = PB_local[q] - PB_local[p]
            S = PS[q] - PS[p]
            if B <= 0.0:
                continue
            total_fom_m += calculate_fom(S, B, mass=m)
        existing_all = compute_existing_total_fom_allbins(f_sig_tmp, f_bkg, flavour, m)
        ratio = (total_fom_m / existing_all) if existing_all > 0 else float('inf')
        log_print(f"[EVAL] Mass {m}: Total FOM = {total_fom_m:.2f}, Existing = {existing_all:.2f}, Ratio = {ratio:.2f}")
        f_sig_tmp.Close()


# -------------------------
# A helper to run one scan
# -------------------------

def run_scan(label, base_path, hpath, args, selected_dir, log_dir="log_scan"):
    """
    label: 'LowJet' or 'DiJet'
    base_path: 'PassSR3_LowJet' or 'PassSR3_DiJet'
    hpath: e.g. 'MainPlots/L_T' or 'Mass/DiJet_M_l1jj'
    """
    os.makedirs(log_dir, exist_ok=True)
    n_bins = args.n_bins_lowjet if label.lower() == "lowjet" else args.n_bins_dijet
    log_file = os.path.join(log_dir, f"scanDP_{label}_{args.era}_{args.flavour}_M{args.mass}_{hpath.replace('/','-')}_{n_bins}.log")

    sig_file_path = os.path.join(selected_dir, args.era, f"HNL_SignalRegion_Plotter_HNL_{args.sig_tag}{args.mass}.root")
    bkg_file_path = os.path.join(selected_dir, args.era, "HNL_SignalRegion_Plotter_Bkg.root")

    f_sig = ROOT.TFile(sig_file_path)
    f_bkg = ROOT.TFile(bkg_file_path)

    best_fom, best_edges = None, None

    with open(log_file, 'w') as log:
        def log_print(msg):
            print(f"[{label}] {msg}", flush=True)
            log.write(msg + "\n")

        # Existing FOM (limit binning)
        get_existing_fom(f_sig, f_bkg, args.flavour, args.mass, log_print)

        # Histograms for this scan
        path = f"{base_path}/HNL_ULIDv2/{args.flavour}/{hpath}"
        sig_hist = f_sig.Get(path)
        bkg_hist = f_bkg.Get(path)

        if (not sig_hist) or (not bkg_hist):
            log_print(f"[ERROR] Missing histogram at {path}")
            f_sig.Close()
            f_bkg.Close()
            return {
                "label": label, "best_fom": None, "best_edges": None,
                "n_bins": n_bins, "base_path": base_path, "hpath": hpath
            }

        # Right-merge constraints for the final bin
        bins_info = get_bin_info(bkg_hist)
        _, _, x_last_min, x_upper = strict_merge_from_right(
            bins_info, min_bkg=args.min_bkg, max_rel_err=args.max_rel_err
        )
        log_print(f"[INFO] Right-merge candidate last-bin range: [{x_last_min:.1f}, {x_upper:.1f}) "
                  f"(used only as constraints: final bin ends at {x_upper:.1f} and its lower edge <= {x_last_min:.1f})")

        # Arrays and prefix sums
        edges, bkg_arr, err2_arr, sig_arr = extract_edges_and_arrays(bkg_hist, sig_hist)
        PB = prefix_sums(bkg_arr)
        PE = prefix_sums(err2_arr)
        PS = prefix_sums(sig_arr.copy())

        # Run DP
        start = time.time()
        best_fom, best_edges = dp_optimal_bins(
            edges, PB, PE, PS,
            n_bins_total=n_bins,
            x_last_min=x_last_min,
            min_width=args.min_width,
            min_bkg=args.min_bkg,
            strong_bkg=args.strong_bkg,
            max_rel_err=args.max_rel_err,
            log_print=log_print
        )
        elapsed = time.time() - start
        log_print(f"[INFO] DP completed in {elapsed:.2f} s")

        if best_edges is not None:
            log_print("\n[EVAL] Using DP binning for cross-mass evaluation:")
            log_print(f"[EVAL] Fixed edges: {best_edges}")
            evaluate_binning_for_masses(
                selected_dir, args.era, args.flavour,
                base_path, hpath,
                edges, bkg_arr, err2_arr, best_edges,
                log_print, f_bkg, args.sig_tag
            )

    f_sig.Close()
    f_bkg.Close()

    return {
        "label": label,
        "best_fom": best_fom,
        "best_edges": best_edges,
        "n_bins": n_bins,
        "base_path": base_path,
        "hpath": hpath,
    }


def _format_edges(edges):
    if not edges:
        return "[]"
    return "[" + ", ".join(f"{e:.1f}" for e in edges) + "]"


# -------------------------
# Main
# -------------------------

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--mass', default='500')
    parser.add_argument('--flavour', default='MuMu')
    parser.add_argument('--era', default='2016preVFP')

    # Two separate n_bins args, one per scan
    parser.add_argument('--n_bins_lowjet', type=int, default=6, help='Number of bins for LowJet scan')
    parser.add_argument('--n_bins_dijet',  type=int, default=6, help='Number of bins for DiJet scan')

    # DP constraints/common knobs
    parser.add_argument('--min_width', type=float, default=20.0)
    parser.add_argument('--min_bkg', type=float, default=0.5)
    parser.add_argument('--strong_bkg', type=float, default=1.0)
    parser.add_argument('--max_rel_err', type=float, default=0.3)

    # Ensure consistent signal file flavor across the whole script
    parser.add_argument('--sig-tag', default='DYVBF_',
                        help="Tag inserted after 'HNL_' in the signal filename, e.g. '' or 'DYVBF_'")

    args = parser.parse_args()

    base_dir = "/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter"
    selected_dir = max([os.path.join(base_dir, d) for d in os.listdir(base_dir)
                        if os.path.isdir(os.path.join(base_dir, d))], key=os.path.getmtime)

    # --- Scan 1: LowJet with MainPlots/L_T ---
    res_lowjet = run_scan(
        label="LowJet",
        base_path="PassSR3_LowJet",
        hpath="MainPlots/L_T",
        args=args,
        selected_dir=selected_dir
    )

    # --- Scan 2: DiJet with Mass/DiJet_M_l1jj ---
    res_dijet = run_scan(
        label="DiJet",
        base_path="PassSR3_DiJet",
        hpath="Mass/DiJet_M_l1jj",
        args=args,
        selected_dir=selected_dir
    )

    # ---- Combined summary (stdout + file) ----
    os.makedirs("log_scan", exist_ok=True)
    summary_path = os.path.join(
        "log_scan",
        f"scanDP_summary_{args.era}_{args.flavour}_M{args.mass}.txt"
    )

    L_fom = res_lowjet["best_fom"] if res_lowjet["best_fom"] is not None else 0.0
    D_fom = res_dijet["best_fom"]  if res_dijet["best_fom"]  is not None else 0.0
    combined_fom = L_fom + D_fom

    lines = []
    lines.append("========== DP Scan Summary ==========")
    lines.append(f"Era: {args.era}  Flavour: {args.flavour}  Mass: {args.mass}  SigTag: {args.sig_tag}")
    lines.append("")
    lines.append(f"[LowJet]  n_bins={res_lowjet['n_bins']}")
    lines.append(f"  Best Total FOM : {L_fom:.2f}" if res_lowjet["best_fom"] is not None else "  Best Total FOM : N/A")
    lines.append(f"  Chosen edges   : {_format_edges(res_lowjet['best_edges'])}")
    lines.append("")
    lines.append(f"[DiJet]   n_bins={res_dijet['n_bins']}")
    lines.append(f"  Best Total FOM : {D_fom:.2f}" if res_dijet["best_fom"] is not None else "  Best Total FOM : N/A")
    lines.append(f"  Chosen edges   : {_format_edges(res_dijet['best_edges'])}")
    lines.append("")
    lines.append(f"COMBINED FOM (LowJet + DiJet): {combined_fom:.2f}")
    lines.append("=====================================")

    text = "\n".join(lines)

    # Print to stdout
    print("\n" + text + "\n")

    # Save to file
    with open(summary_path, "w") as f:
        f.write(text + "\n")

    print(f"[SUMMARY] Written combined summary to: {summary_path}")


if __name__ == "__main__":
    main()
