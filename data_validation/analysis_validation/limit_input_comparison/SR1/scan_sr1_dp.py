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

def check_custom_edges(edges, PB, PE, PS, edge_list, log_print,
                       name="custom", min_width=20.0,
                       min_bkg=0.5, strong_bkg=1.0, max_rel_err=0.3):
    """
    Evaluate a given list of bin edges on the same histogram used by DP.
    Prints per-bin B, S, relErr, FOM and total FOM.
    Also checks whether each bin would satisfy the DP constraints.
    """
    # Map requested edges to nearest indices in the fine-bin edges array
    idxs = [int(np.argmin(np.abs(edges - e))) for e in edge_list]

    log_print(f"[CHECK-{name}] Requested edges = {edge_list}")
    log_print(f"[CHECK-{name}] Mapped to indices/edges:")
    for e, idx in zip(edge_list, idxs):
        log_print(f"  target {e:.1f} -> idx {idx}, edges[idx] = {edges[idx]:.1f}")

    total_fom = 0.0
    log_print(f"[CHECK-{name}] Bin-by-bin:")
    for k in range(len(idxs) - 1):
        p = idxs[k]
        q = idxs[k + 1]
        lo = edges[p]
        hi = edges[q]
        width = hi - lo
        B = PB[q] - PB[p]
        E = PE[q] - PE[p]
        S = PS[q] - PS[p]
        rel = math.sqrt(E) / B if B > 0 else float("inf")
        fom = calculate_fom(S, B, mass="NA")
        total_fom += fom

        # Check DP-style constraints for this interval
        passes_width = (width >= min_width)
        passes_stats = (B >= min_bkg) and ((B >= strong_bkg) or (rel <= max_rel_err))

        log_print(
            f"  Bin {k+1}: [{lo:.1f}, {hi:.1f}) "
            f"W={width:.1f}, B={B:.3f}, S={S:.3f}, relErr={rel:.3f}, "
            f"FOM={fom:.2f}, width_ok={passes_width}, stats_ok={passes_stats}"
        )

    log_print(f"[CHECK-{name}] Total FOM = {total_fom:.2f}")
    return total_fom

def calculate_fom(s, b, mass):
    # Mass-dependent signal scaling

    if b > 0.0 and s > 0.0:
        return (2.0 * ((s + b) * math.log(1.0 + s / b) - s))
        #return math.sqrt(2.0 * ((s + b) * math.log(1.0 + s / b) - s))
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
    # FIX: if bkg == 0, do not add variance
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
        total_fom_print = 0.0
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
            total_fom_print += fom
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
        "MuMu": "LimitExtraction/HNL_ULIDv2/MuMu/LimitBins/MuonSR1",
        "EE": "LimitExtraction/HNL_ULIDv2/EE/LimitBins/ElectronSR1",
        "EMu": "LimitExtraction/HNL_ULIDv2/EMu/LimitBins/ElectronMuonSR1",
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



import ROOT
import math

def get_existing_fom(sig_file, bkg_file, flavour, mass, log_print):
    path_map = {
        "MuMu": "LimitExtraction/HNL_ULIDv2/MuMu/LimitBins/MuonSR1",
        "EE":   "LimitExtraction/HNL_ULIDv2/EE/LimitBins/ElectronSR1",
        "EMu":  "LimitExtraction/HNL_ULIDv2/EMu/LimitBins/ElectronMuonSR1",
    }

    path = path_map.get(flavour)
    log_print("=== DEBUG inside get_existing_fom ===")
    log_print(f"  flavour = {flavour}")
    log_print(f"  mass    = {mass}")
    log_print(f"  base path = {path}")

    sig_obj = sig_file.Get(path)
    bkg_obj = bkg_file.Get(path)

    if not sig_obj:
        log_print("  ERROR: sig_file.Get(path) returned None")
    else:
        log_print(f"  sig_obj: class={sig_obj.ClassName()}, name={sig_obj.GetName()}")

    if not bkg_obj:
        log_print("  ERROR: bkg_file.Get(path) returned None")
    else:
        log_print(f"  bkg_obj: class={bkg_obj.ClassName()}, name={bkg_obj.GetName()}")

    # If these are directories, list their contents so we can see the actual hist name
    if sig_obj and isinstance(sig_obj, ROOT.TDirectory):
        log_print("  sig_obj is a TDirectory, listing keys:")
        for key in sig_obj.GetListOfKeys():
            log_print(f"    [sig] key: {key.GetName()} ({key.GetClassName()})")

    if bkg_obj and isinstance(bkg_obj, ROOT.TDirectory):
        log_print("  bkg_obj is a TDirectory, listing keys:")
        for key in bkg_obj.GetListOfKeys():
            log_print(f"    [bkg] key: {key.GetName()} ({key.GetClassName()})")

    # If they are not TH1, do not try GetBinContent
    if not (sig_obj and isinstance(sig_obj, ROOT.TH1)):
        log_print("  ERROR: signal object is not a TH1, cannot compute FOM")
        return
    if not (bkg_obj and isinstance(bkg_obj, ROOT.TH1)):
        log_print("  ERROR: background object is not a TH1, cannot compute FOM")
        return

    sig_hist = sig_obj
    bkg_hist = bkg_obj

    total_allbins = 0.0
    log_print(f"[INFO] Bin-by-bin FOMs from {path}:")
    nbins = bkg_hist.GetNbinsX()
    log_print(f"  Histogram nbins = {nbins}")

    for i in range(1, nbins + 1):
        b = bkg_hist.GetBinContent(i)
        s = sig_hist.GetBinContent(i)
        err = bkg_hist.GetBinError(i)
        rel_err = (err / b) if b > 0 else float('inf')
        fom = calculate_fom(s, b, mass)
        total_allbins += fom

        if math.isfinite(rel_err):
            log_print(
                f"  Bin {i:2d}: Bkg = {b:.2f}, Sig = {s:.2f}, "
                f"FOM = {fom:.2f}, RelErr = {rel_err:.2f}"
            )
        else:
            log_print(
                f"  Bin {i:2d}: Bkg = {b:.2f}, Sig = {s:.2f}, "
                f"FOM = {fom:.2f}, RelErr = inf"
            )

    log_print(f"[INFO] Total EXISTING FOM over all bins: {total_allbins:.2f}")



# -------------------------
# Cross-mass evaluation
# -------------------------

def evaluate_binning_for_masses(selected_dir, era, flavour, base_path, hname,
                                edges, bkg, err2, best_edges, log_print, f_bkg, mnonly):
    masses = ["400", "450","500", "600", "700", "800", "900", "1000"]
    PB = prefix_sums(bkg)
    PE = prefix_sums(err2)
    cut_indices = [int(np.argmin(np.abs(edges - e))) for e in best_edges]

    for m in masses:
        sig_path = os.path.join(selected_dir, era, f"HNL_SignalRegion_Plotter_HNL_DYVBF_{m}.root")
        f_sig_tmp = ROOT.TFile(sig_path)
        sig_hist = f_sig_tmp.Get(f"{base_path}/HNL_ULIDv2/{flavour}/AK8/AK8J_Unbinned_Mass/{hname}")
        if not sig_hist:
            f_sig_tmp.Close()
            continue
        _, _, _, sig_arr = extract_edges_and_arrays(f_bkg.Get(f"{base_path}/HNL_ULIDv2/{flavour}/AK8/AK8J_Unbinned_Mass/{hname}"), sig_hist)
        sig_arr = sig_arr[:len(bkg)]
        sig_scaled = sig_arr.copy()
        PS = prefix_sums(sig_scaled)
        total_fom_m = 0.0
        for i in range(len(cut_indices) - 1):
            p = cut_indices[i]
            q = cut_indices[i + 1]
            B = PB[q] - PB[p]
            S = PS[q] - PS[p]
            if B <= 0.0:
                continue
            total_fom_m += calculate_fom(S, B, mass=m)
        existing_all = compute_existing_total_fom_allbins(f_sig_tmp, f_bkg, flavour, m)
        ratio = (total_fom_m / existing_all) if existing_all > 0 else float('inf')
        log_print(f"[EVAL] Mass {m}: Total FOM = {total_fom_m:.2f}, "
                  f"Existing = {existing_all:.2f}, Ratio = {ratio:.2f}")
        f_sig_tmp.Close()


# -------------------------
# Main
# -------------------------

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--mass', default='800')
    parser.add_argument('--flavour', default='EMu')
    parser.add_argument('--era', default='2016preVFP')
    parser.add_argument('--n_bins', type=int, default=6)
    parser.add_argument('--min_width', type=float, default=20.0)
    parser.add_argument('--min_bkg', type=float, default=0.5)
    parser.add_argument('--strong_bkg', type=float, default=1.0)
    parser.add_argument('--max_rel_err', type=float, default=0.3)
    args = parser.parse_args()

    base_dir = "/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter"
    selected_dir = max([os.path.join(base_dir, d) for d in os.listdir(base_dir)
                        if os.path.isdir(os.path.join(base_dir, d))], key=os.path.getmtime)

    sig_file_path = os.path.join(selected_dir, args.era, f"HNL_SignalRegion_Plotter_HNL_DYVBF_{args.mass}.root")
    bkg_file_path = os.path.join(selected_dir, args.era, "HNL_SignalRegion_Plotter_Bkg.root")

    f_sig = ROOT.TFile(sig_file_path)
    f_bkg = ROOT.TFile(bkg_file_path)

    path = f"PassSR1/HNL_ULIDv2/{args.flavour}/AK8/AK8J_Unbinned_Mass"
    hname = "l1J"
    full_path = f"{path}/{hname}"     # <-- define it here

    sig_hist = f_sig.Get(f"{path}/{hname}")
    bkg_hist = f_bkg.Get(f"{path}/{hname}")

    os.makedirs("log_scan", exist_ok=True)
    log_file = f"log_scan/scanDP_{args.era}_{args.flavour}_M{args.mass}_{hname}_{args.n_bins}.log"
    with open(log_file, 'w') as log:
        def log_print(msg):
            print(msg, flush=True)
            log.write(msg + "\n")

        log_print("=== DEBUG before get_existing_fom ===")
        log_print(f"sig_file_path = {sig_file_path}")
        log_print(f"bkg_file_path = {bkg_file_path}")
        log_print(f"full_path    = {full_path}")

        if not sig_hist:
            log_print("ERROR: sig_hist is None")
        else:
            log_print(f"sig_hist: type={type(sig_hist)}, class={sig_hist.ClassName()}, name={sig_hist.GetName()}")

        if not bkg_hist:
            log_print("ERROR: bkg_hist is None")
        else:
            log_print(f"bkg_hist: type={type(bkg_hist)}, class={bkg_hist.ClassName()}, name={bkg_hist.GetName()}")

        log_print("Calling get_existing_fom...")

            
        get_existing_fom(f_sig, f_bkg, args.flavour, args.mass, log_print)

        # Right-merge to get constraints for the final bin
        bins_info = get_bin_info(bkg_hist)
        _, _, x_last_min, x_upper = strict_merge_from_right(bins_info)
        log_print(f"[INFO] Right-merge candidate last-bin range: [{x_last_min:.1f}, {x_upper:.1f}) "
                  f"(used only as constraints: final bin ends at {x_upper:.1f} and its lower edge <= {x_last_min:.1f})")
        
        # Arrays and prefix sums
        edges, bkg_arr, err2_arr, sig_arr = extract_edges_and_arrays(bkg_hist, sig_hist)
        PB = prefix_sums(bkg_arr)
        PE = prefix_sums(err2_arr)
        
        # Mass scaling (use your policy; example shown)
        sig_scaled = sig_arr.copy()
        # if args.mass in ["500","600","700"]:
        #     sig_scaled = sig_arr / 10.0
        
        PS = prefix_sums(sig_scaled)

        # ==========================================================
        # CHECK: evaluate predefined MuMu SR1 binning on this l1J hist
        # ==========================================================
        if args.flavour == "MuMu" and args.mass == "450":
            predef_edges_450 = [0.0, 395.0, 420.0, 450.0, 540.0, 635.0, 850.0, 5000.0]
            log_print("")
            log_print("[CHECK] Evaluating predefined sr1bins_mm_byMass[\"450\"] on l1J histogram:")
            check_custom_edges(
                edges=edges,
                PB=PB,
                PE=PE,
                PS=PS,
                edge_list=predef_edges_450,
                log_print=log_print,
                name="predef_450",
                min_width=args.min_width,
                min_bkg=args.min_bkg,
                strong_bkg=args.strong_bkg,
                max_rel_err=args.max_rel_err,
            )
            log_print("")
        
        # Run DP
        start = time.time()
        best_fom, best_edges = dp_optimal_bins(
            edges, PB, PE, PS,
            n_bins_total=args.n_bins,
            x_last_min=x_last_min,
            min_width=args.min_width,
            min_bkg=args.min_bkg,
            strong_bkg=args.strong_bkg,
            max_rel_err=args.max_rel_err,
            log_print=log_print
        )
        elapsed = time.time() - start
        log_print(f"[INFO] DP completed in {elapsed:.2f} s")

        # Optional: echo x_upper next to edges line for clarity (inside dp_optimal_bins print)
        # log_print(f"  Best Total FOM = {best_fom:.2f} with edges: {best_edges} (x_upper = {x_upper:.1f})")
            
        if best_edges is not None:
            log_print("\n[EVAL] Using DP binning for cross-mass evaluation:")
            log_print(f"[EVAL] Fixed edges: {best_edges}")
            evaluate_binning_for_masses(selected_dir, args.era, args.flavour, "PassSR1", hname,
                                        edges, bkg_arr, err2_arr, best_edges, log_print, f_bkg, True)

    f_sig.Close()
    f_bkg.Close()


if __name__ == "__main__":
    main()
