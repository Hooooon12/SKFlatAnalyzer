import ROOT
import os
import math
import itertools
import argparse
import time
from tqdm import tqdm
import sys

def calculate_fom(s, b, mass):
    if mass in ["500", "600", "700"]:
        s /= 10
    return math.sqrt(2 * ((s + b) * math.log(1 + s / b) - s)) if b > 0 and s > 0 else (math.sqrt(2 * s) if s > 0 else 0)

def get_bin_info(hist):
    bins = []
    for i in range(1, hist.GetNbinsX()+1):
        x_low = hist.GetBinLowEdge(i)
        x_high = hist.GetBinLowEdge(i+1)
        bkg = hist.GetBinContent(i)
        err = hist.GetBinError(i)
        rel_err = err / bkg if bkg > 0 else float('inf')
        bins.append((i, x_low, x_high, bkg, rel_err))
    return bins

def build_sig_bin_cache(sig_hist):
    return [(sig_hist.GetBinLowEdge(i), sig_hist.GetBinContent(i)) for i in range(1, sig_hist.GetNbinsX()+1)]

def strict_merge_from_right(bins, min_bkg=1.0, max_rel_err=0.05):
    acc_bkg = 0.0
    err2_sum = 0.0
    right_idx = len(bins) - 1
    for i in range(len(bins) - 1, -1, -1):
        _, x_low, x_high, bkg, rel_err = bins[i]
        acc_bkg += bkg
        if math.isfinite(rel_err):
            err2_sum += (bkg * rel_err)**2
        rel_err_acc = math.sqrt(err2_sum) / acc_bkg if acc_bkg > 0 else float('inf')
        if acc_bkg > 0.5 and (acc_bkg > min_bkg or rel_err_acc < max_rel_err):
            right_idx = i
            break
    return bins[right_idx:], bins[:right_idx]

def strict_merge_subrange(sub_bins, sig_bin_cache, mass):
    acc_bkg = 0.0
    acc_sig = 0.0
    err2_sum = 0.0
    for _, x_low, x_high, bkg, rel_err in sub_bins:
        acc_bkg += bkg
        if math.isfinite(rel_err):
            err2_sum += (bkg * rel_err)**2
        sig = sum(val for bx, val in sig_bin_cache if x_low <= bx < x_high)
        acc_sig += sig
    rel_err = math.sqrt(err2_sum) / acc_bkg if acc_bkg > 0 else float('inf')
    fom = calculate_fom(acc_sig, acc_bkg, mass)
    return acc_bkg, acc_sig, rel_err, fom

def exhaustive_binning(bins, last_bin, sig_hist, n_bins_total, mass, log_print, base_path):
    start_time = time.time()
    sig_bin_cache = build_sig_bin_cache(sig_hist)

    x_min = None
    for i in range(len(bins)):
        acc_bkg = acc_sig = err2_sum = 0.0
        for j in range(i, len(bins)):
            _, x_low, x_high, bkg, rel_err = bins[j]
            acc_bkg += bkg
            if math.isfinite(rel_err):
                err2_sum += (bkg * rel_err)**2
            rel_err_acc = math.sqrt(err2_sum) / acc_bkg if acc_bkg > 0 else float('inf')
            if acc_bkg > 0.5 and (acc_bkg > 1.0 or rel_err_acc < 0.05):
                x_min = bins[i][1]
                break
        if x_min is not None:
            break

    x_last = last_bin[0][1]
    log_print(f"[DEBUG] ({base_path}) Last bin starts at X = {x_last:.1f} GeV")
    log_print(f"[DEBUG] ({base_path}) Maximum X for bin 1 = {x_last:.1f} GeV")

    all_edges = sorted(set([b[1] for b in bins if x_min <= b[1] < x_last]))

    best_fom = -1
    best_edges = None

    all_combos = list(itertools.combinations(all_edges, n_bins_total - 1))
    total_combos = len(all_combos)
    
    log_print(f"[INFO] ({base_path}) Scanning {total_combos} combinations...")

    
    use_tqdm = sys.stdout.isatty()
    
    for idx, edges in enumerate(tqdm(all_combos, desc=f"Scanning ({base_path})", disable=not use_tqdm)):
        iter_start = time.time()
        edges = sorted(edges) + [x_last]
        valid = True
        prev_edge = x_min
        total_fom = 0.0
        for edge in edges:
            if edge - prev_edge < 20:
                valid = False
                break
            subrange = [b for b in bins if prev_edge <= b[1] < edge]
            if not subrange:
                valid = False
                break
            bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_bin_cache, mass)
            if (bkg < 0.5 and rel_err >= 0.05) or bkg < 1.0:
                valid = False
                break
            total_fom += fom
            prev_edge = edge
        if not valid:
            continue
        _, _, _, last_fom = strict_merge_subrange(last_bin, sig_bin_cache, mass)
        total_fom += last_fom
        if total_fom > best_fom:
            best_fom = total_fom
            best_edges = edges
        iter_end = time.time()
        #log_print(f"[TRACE] Combination {idx} took {iter_end - iter_start:.2f} sec")

    total_time = time.time() - start_time
    log_print(f"[RESULT] Exhaustive scan completed in {total_time:.2f} seconds")
    if best_edges is not None:
        log_print(f"  Best FOM = {best_fom:.2f} with edges: {[x_min]+list(best_edges)}")
        full_edges = [x_min] + list(best_edges)
        log_print("  Bin-by-bin summary:")
        for i in range(len(full_edges) - 1):
            lo = full_edges[i]
            hi = full_edges[i + 1]
            subrange = [b for b in bins if lo <= b[1] < hi]
            bkg, sig, rel_err, fom = strict_merge_subrange(subrange, sig_bin_cache, mass)
            log_print(f"    Bin {i+1}: Range = [{lo:.1f}, {hi:.1f}) | Bkg = {bkg:.2f}, Sig = {sig:.2f}, FOM = {fom:.2f}, RelErr = {rel_err:.2f}")
        bkg, sig, rel_err, fom = strict_merge_subrange(last_bin, sig_bin_cache, mass)
        lo = full_edges[-1]
        hi = last_bin[-1][2]
        log_print(f"    Bin {len(full_edges)}: Range = [{lo:.1f}, {hi:.1f}) | Bkg = {bkg:.2f}, Sig = {sig:.2f}, FOM = {fom:.2f}, RelErr = {rel_err:.2f}")
    else:
        log_print(f"[WARN] No valid binning configuration found for {base_path} with {n_bins_total} bins.")


def select_directory(base_path):
    print(f"[INFO] Scanning for subdirectories in: {base_path}")
    directories = [
        os.path.join(base_path, d)
        for d in os.listdir(base_path)
        if os.path.isdir(os.path.join(base_path, d))
    ]
    if not directories:
        raise RuntimeError(f"No directories found in {base_path}")
    newest_dir = max(directories, key=os.path.getmtime)
    print(f"[INFO] Automatically selected newest directory: {os.path.basename(newest_dir)}")
    return newest_dir

def select_directory_manually(base_path):
    print(f"[INFO] Scanning for subdirectories in: {base_path}")
    directories = sorted([
        d for d in os.listdir(base_path)
        if os.path.isdir(os.path.join(base_path, d))
    ])
    if not directories:
        raise RuntimeError(f"No directories found in {base_path}")
    print("Available directories:")
    for idx, d in enumerate(directories):
        print(f"  [{idx}] {d}")
    selected_idx = int(input("Select directory index: "))
    selected_dir = os.path.join(base_path, directories[selected_idx])
    print(f"[INFO] Selected directory: {selected_dir}")
    return selected_dir

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

    total_fom = 0.0
    total_allbins = 0.0
    log_print(f"[INFO] Bin-by-bin FOMs from {path}:")
    for i in range(1, bkg_hist.GetNbinsX() + 1):
        b = bkg_hist.GetBinContent(i)
        s = sig_hist.GetBinContent(i)
        fom = calculate_fom(s, b, mass)
        total_allbins += fom
        if i >= 4:
            total_fom += fom
        log_print(f"  Bin {i:2d}: Bkg = {b:.2f}, Sig = {s:.2f}, FOM = {fom:.2f}")
    log_print(f"[INFO] Existing summed FOM from {path} (excluding first 3 bins): {total_fom:.2f}")
    log_print(f"[INFO] Total FOM over all bins: {total_allbins:.2f}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--mass', default='500')
    parser.add_argument('--flavour', default='MuMu')
    parser.add_argument('--era', default='2016preVFP')
    parser.add_argument('--n_bins', type=int, default=-1)
    parser.add_argument('--n_bins_min', type=int, default=2)
    parser.add_argument('--n_bins_max', type=int, default=2)
    parser.add_argument('--mnonly', action='store_true', help="If set, use only PassSR1; otherwise use PassSR1_MNBins")

    args = parser.parse_args()

    if args.n_bins > 0:
        n_bins_min = args.n_bins
        n_bins_max = args.n_bins
    
    base_dir = "/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter"
    selected_dir = select_directory(base_dir)
    sig_file_path = os.path.join(selected_dir, args.era, f"HNL_SignalRegion_Plotter_HNL_{args.mass}.root")
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
        log_file = f"log_scan/scan_{args.era}_{args.flavour}_{log_suffix}_M{args.mass}_{hname}.log"    
        os.makedirs("log_scan", exist_ok=True)

        with open(log_file, 'w') as log:
            def log_print(msg):
                print(msg)
                log.write(msg + "\n")

            log_print(f"[INFO] Starting scan for {args.era} {args.flavour} M{args.mass} {hname} ({base_path})")
            get_existing_fom(f_sig, f_bkg, args.flavour, args.mass, log_print)

            bins = get_bin_info(bkg_hist)
            last_bin, remaining_bins = strict_merge_from_right(bins)

            for n_bins in range(n_bins_min, n_bins_max + 1):
                log_print(f"\n[INFO] ({base_path}) Testing {n_bins} total bins:")
                exhaustive_binning(remaining_bins, last_bin, sig_hist, n_bins, args.mass, log_print, base_path)

    f_sig.Close()
    f_bkg.Close()

if __name__ == "__main__":
    main()
