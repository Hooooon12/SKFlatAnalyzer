import ROOT
import os
import math
import itertools
from itertools import combinations
import argparse
import time
import random
from tqdm import tqdm


def calculate_fom(s, b, mass):
    # Mass-dependent signal scaling (as in your original)
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


def strict_merge_from_right(bins, min_bkg=1.0, max_rel_err=0.3):
    """
    Merge from the right until the accumulated last bin satisfies:
      acc_bkg > 0.5 and (acc_bkg > min_bkg or rel_err_acc < max_rel_err)

    Returns:
      right_chunk_bins, remaining_bins, x_last_min, x_upper
    """
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
    return (bkg > 0.5) and (bkg > 1.0 or rel_err < 0.3)


def greedy_binning(bins, x_last_min, x_upper, sig_hist, n_bins_total, mass, log_print, base_path):
    log_print(f"[INFO] (Greedy) Starting greedy binning for {n_bins_total} bins")
    if not bins:
        log_print("[WARN] (Greedy) No bins available to scan.")
        return None, None

    sig_bin_cache = build_sig_bin_cache(sig_hist)
    all_edges = sorted(set(b[1] for b in bins))
    x_min = bins[0][1]

    best_edges = []
    prev_edge = x_min
    k = n_bins_total - 1

    for cut_idx in tqdm(range(k), desc=f"Greedy ({base_path})"):
        best_local_fom = -1.0
        best_cut = None
        for edge in all_edges:
            if edge <= prev_edge or edge >= x_upper:
                continue
            if cut_idx == k - 1 and edge > x_last_min:
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
            log_print("[WARN] (Greedy) No valid cut found; aborting greedy binning.")
            return None, None
        best_edges.append(best_cut)
        prev_edge = best_cut

    full_edges = [x_min] + best_edges + [x_upper]
    total_fom = 0.0
    log_print("  Bin-by-bin summary (Greedy):")
    for i in range(len(full_edges) - 1):
        lo = full_edges[i]
        hi = full_edges[i + 1]
        subrange = [b for b in bins if lo <= b[1] < hi]
        bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_bin_cache, mass)
        total_fom += fom
        log_print(f"    Bin {i+1}: Range = [{lo:.1f}, {hi:.1f}) | Bkg = {bkg:.2f}, Sig = {sig:.2f}, FOM = {fom:.2f}, RelErr = {rel_err:.2f}")
    log_print(f"[INFO] (Greedy) Total FOM with {n_bins_total} bins: {total_fom:.2f}")
    return total_fom, full_edges


def exhaustive_binning(bins, x_last_min, x_upper, sig_hist, n_bins_total, mass, log_print, base_path):
    start_time = time.time()
    sig_bin_cache = build_sig_bin_cache(sig_hist)

    if not bins:
        log_print("[WARN] (Exhaustive) No bins available to scan.")
        return None, None

    x_min = bins[0][1]
    all_edges = sorted(set(b[1] for b in bins if x_min <= b[1] < x_upper))

    k = n_bins_total - 1
    comb_count = math.comb(len(all_edges), k) if len(all_edges) >= k and k >= 0 else 0
    if comb_count == 0:
        log_print("[WARN] (Exhaustive) No edge combinations possible.")
        return None, None

    max_combos = 2_000_000
    comb_iter = combinations(all_edges, k)

    if n_bins_total >= 5 and comb_count > max_combos:
        log_print(f"[INFO] Sampling {max_combos} combinations out of {comb_count} via reservoir sampling")
        sample = []
        t_seen = 0
        for combo in comb_iter:
            if combo and max(combo) > x_last_min:
                continue
            t_seen += 1
            if len(sample) < max_combos:
                sample.append(combo)
            else:
                j = random.randrange(t_seen)
                if j < max_combos:
                    sample[j] = combo
        all_combos = sample
        total_for_tqdm = len(sample)
    else:
        def pruned():
            for combo in comb_iter:
                if combo and max(combo) > x_last_min:
                    continue
                yield combo
        all_combos = pruned()
        total_for_tqdm = comb_count

    best_fom = -1.0
    best_edges = None

    for edges in tqdm(all_combos, total=total_for_tqdm, desc=f"Exhaustive ({base_path})"):
        edges = sorted(edges) + [x_upper]
        prev_edge = x_min
        total_fom = 0.0
        valid = True

        for edge in edges:
            if edge - prev_edge < 20:
                valid = False
                break
            subrange = [b for b in bins if prev_edge <= b[1] < edge]
            if not subrange:
                valid = False
                break
            bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_bin_cache, mass)
            if not bin_is_valid(bkg, rel_err):
                valid = False
                break
            total_fom += fom
            prev_edge = edge

        if not valid:
            continue

        if total_fom > best_fom:
            best_fom = total_fom
            best_edges = [x_min] + sorted(edges)

    total_time = time.time() - start_time
    log_print(f"[RESULT] Exhaustive scan completed in {total_time:.2f} seconds")
    if best_edges is None:
        log_print(f"[WARN] No valid binning configuration found for {base_path} with {n_bins_total} bins.")
        return None, None

    log_print(f"  Best Total FOM = {best_fom:.2f} with edges: {best_edges}")
    log_print("  Bin-by-bin summary (Exhaustive):")
    for i in range(len(best_edges) - 1):
        lo = best_edges[i]
        hi = best_edges[i + 1]
        subrange = [b for b in bins if lo <= b[1] < hi]
        bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_bin_cache, mass)
        log_print(f"    Bin {i+1}: Range = [{lo:.1f}, {hi:.1f}) | Bkg = {bkg:.2f}, Sig = {sig:.2f}, FOM = {fom:.2f}, RelErr = {rel_err:.2f}")
    return best_fom, best_edges


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


def compute_existing_fom_first3(sig_file, bkg_file, flavour, mass):
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
    for i in range(1, min(4, bkg_hist.GetNbinsX() + 1)):
        b = bkg_hist.GetBinContent(i)
        s = sig_hist.GetBinContent(i)
        total_fom += calculate_fom(s, b, mass)
    return total_fom


def evaluate_binning_for_masses(selected_dir, era, flavour, base_path, hname,
                                bins, full_edges, log_print, f_bkg, mnonly):
    masses = ["500", "600", "700", "800","900","1000"]
    per_mass = {}
    summed_fom = 0.0
    summed_existing_all = 0.0
    summed_ratios = 0.0

    for m in masses:
        sig_path = os.path.join(selected_dir, era, f"HNL_SignalRegion_Plotter_HNL_{m}.root")
        f_sig_tmp = ROOT.TFile(sig_path)

        sig_hist = f_sig_tmp.Get(f"{base_path}/HNL_ULIDv2/{flavour}/AK8/AK8J_Unbinned_Mass/{hname}")
        if not sig_hist:
            log_print(f"[EVAL][WARN] Missing signal hist for mass {m}")
            f_sig_tmp.Close()
            continue

        sig_bin_cache = build_sig_bin_cache(sig_hist)
        total_fom_m = 0.0
        for i in range(len(full_edges) - 1):
            lo = full_edges[i]
            hi = full_edges[i + 1]
            subrange = [b for b in bins if lo <= b[1] < hi]
            if not subrange:
                continue
            _, _, _, fom = strict_merge_subrange(subrange, sig_bin_cache, m)
            total_fom_m += fom

        existing_all = compute_existing_total_fom_allbins(f_sig_tmp, f_bkg, flavour, m)

        first3_contrib = 0.0
        if not mnonly:
            first3_contrib = compute_existing_fom_first3(f_sig_tmp, f_bkg, flavour, m)
            total_fom_m += first3_contrib

        ratio = (total_fom_m / existing_all) if existing_all > 0 else float('inf')

        per_mass[m] = (total_fom_m, existing_all, ratio, first3_contrib)
        summed_fom += total_fom_m
        summed_existing_all += existing_all
        summed_ratios += ratio

        if first3_contrib > 0:
            log_print(f"[EVAL] Mass {m}: Total FOM = {total_fom_m:.2f}. : Existing FOM = {existing_all:.2f} (bins 1-3 contribution = {first3_contrib:.2f}) | Ratio = {ratio:.2f}")
        else:
            log_print(f"[EVAL] Mass {m}: Total FOM = {total_fom_m:.2f}. : Existing FOM = {existing_all:.2f} | Ratio = {ratio:.2f}")

        f_sig_tmp.Close()

    log_print(f"[EVAL] SUMMED FOM over masses {masses}: {summed_fom:.2f}")
    log_print(f"[EVAL] SUMMED EXISTING FOM (all bins) over masses {masses}: {summed_existing_all:.2f}")
    log_print(f"[EVAL] SUM OF RATIOS over masses {masses}: {summed_ratios:.2f}")
    return per_mass, summed_fom, summed_existing_all


def get_existing_fom(sig_file, bkg_file, flavour, mass, log_print):
    path_map = {
        "MuMu": "LimitExtraction/HNL_ULIDv2/MuMu/LimitBins/MuonSR1",
        "EE": "LimitExtraction/HNL_ULIDv2/EE/LimitBins/ElectronSR1",
        "EMu": "LimitExtraction/HNL_ULIDv2/EMu/LimitBins/ElectronMuonSR1",
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
            log_print(f"  Bin {i:2d}: Bkg = {b:.2f}, Sig = {s:.2f}, FOM = {fom:.2f}, RelErr = {rel_err:.2f}")
        else:
            log_print(f"  Bin {i:2d}: Bkg = {b:.2f}, Sig = {s:.2f}, FOM = {fom:.2f}, RelErr = inf")

    log_print(f"[INFO] Total EXISTING FOM over all bins: {total_allbins:.2f}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--mass', default='500')
    parser.add_argument('--flavour', default='MuMu')
    parser.add_argument('--era', default='2016preVFP')
    parser.add_argument('--n_bins', type=int, default=2)
    parser.add_argument('--mnonly', action='store_true', help="Use only PassSR1")
    args = parser.parse_args()

    base_dir = "/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter"
    selected_dir = max([
        os.path.join(base_dir, d)
        for d in os.listdir(base_dir)
        if os.path.isdir(os.path.join(base_dir, d))
    ], key=os.path.getmtime)

    sig_file_path = os.path.join(selected_dir, args.era, f"HNL_SignalRegion_Plotter_HNL_DYVBF_{args.mass}.root")
    bkg_file_path = os.path.join(selected_dir, args.era, "HNL_SignalRegion_Plotter_Bkg.root")

    f_sig = ROOT.TFile(sig_file_path)
    f_bkg = ROOT.TFile(bkg_file_path)

    base_paths = ["PassSR1"] if args.mnonly else ["PassSR1_MNBins"]

    for base_path in base_paths:
        path = f"{base_path}/HNL_ULIDv2/{args.flavour}/AK8/AK8J_Unbinned_Mass"
        hname = "l1J"

        sig_hist = f_sig.Get(f"{path}/{hname}")
        bkg_hist = f_bkg.Get(f"{path}/{hname}")

        if not bkg_hist or not isinstance(bkg_hist, ROOT.TH1):
            print(f"[ERROR] Could not load histogram: {path}/{hname} from {bkg_file_path}")
            continue

        if not sig_hist or not isinstance(sig_hist, ROOT.TH1):
            print(f"[ERROR] Could not load histogram: {path}/{hname} from {sig_file_path}")
            continue

        log_suffix = "MNonly" if args.mnonly else "MNBins"
        os.makedirs("log_scan", exist_ok=True)
        log_file = f"log_scan/scan_{args.era}_{args.flavour}_{log_suffix}_M{args.mass}_{hname}_{args.n_bins}.log"

        with open(log_file, 'w') as log:
            def log_print(msg):
                print(msg, flush=True)
                log.write(msg + "\n")

            log_print(f"[INFO] Running scan for MASS={args.mass}, FLAV={args.flavour}, ERA={args.era}, NBIN={args.n_bins}")
            log_print(f"[INFO] Starting scan for {args.era} {args.flavour} M{args.mass} {hname} ({base_path})")

            # Existing per-bin dump

            get_existing_fom(f_sig, f_bkg, args.flavour, args.mass, log_print)

            bins = get_bin_info(bkg_hist)
            right_chunk, remaining_bins, x_last_min, x_upper = strict_merge_from_right(bins)
            log_print(f"[INFO] Right-merge candidate last-bin range: [{x_last_min:.1f}, {x_upper:.1f}) "
                      f"(used only as constraints: final bin ends at {x_upper:.1f} and its lower edge <= {x_last_min:.1f})")


            # Eligible bins for scanning (all with low edge < x_upper)
            scan_bins = [b for b in bins if b[1] < x_upper]

            log_print(f"\n[INFO] ({base_path}) Testing {args.n_bins} total bins:")

            best_fom, best_edges = exhaustive_binning(
                scan_bins, x_last_min, x_upper, sig_hist, args.n_bins, args.mass, log_print, base_path
            )

            if best_edges is not None:
                log_print("\n[EVAL] Using Exhaustive binning for cross-mass evaluation:")
                log_print(f"[EVAL] Fixed edges: {best_edges}")
                evaluate_binning_for_masses(
                    selected_dir, args.era, args.flavour, base_path, hname,
                    scan_bins, best_edges, log_print, f_bkg, args.mnonly
                )
            else:
                log_print("[WARN] No valid binning found by either method; skipping cross-mass evaluation.")

    f_sig.Close()
    f_bkg.Close()


if __name__ == "__main__":
    main()
