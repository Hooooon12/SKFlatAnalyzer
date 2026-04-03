import os
import sys
import math
import ROOT
import time
import numpy as np
import math
import itertools

import random

def get_random_combinations(edges, k, n_samples):
    """Generate n_samples random combinations of k edges from the list."""
    edges = list(edges)
    seen = set()
    while len(seen) < n_samples:
        combo = tuple(sorted(random.sample(edges, k)))
        if combo not in seen:
            seen.add(combo)
            yield combo

# Colored output
def print_in_color(msg, is_red=False):
    print(f"\033[31m{msg}\033[0m" if is_red else msg)

def calculate_fom(s, b,mass):
    if mass == "100":
        s = s / 100
    if mass == "125":
        s = s / 125
    if mass == "150":
        s = s / 150

    if mass == "200":
        s = s / 100
    if mass == "250":
        s = s / 10
    if mass == "300":
        s = s / 10
    if mass == "400":
        s = s / 10
    if mass == "500":
        s = s / 10        
        
    return math.sqrt(2 * ((s + b) * math.log(1 + s / b) - s)) if b > 0 and s > 0 else (math.sqrt(2 * s) if s > 0 else 0)

def compare_limitbins_fom(bkg_file, sig_file, flavours, masses, log):
    def log_print(msg):
        print(msg)
        log.write(msg + "\n")

    log_print("\n[INFO] Comparing summed FOMs from LimitBins histograms (bin-by-bin breakdown):")
    for flav in flavours:
        for mass in masses:
            for version_tag in ["V3", "V4"]:
                bkg_path = f"LimitExtractionBDT/HNL_ULIDv2/{flav}_{version_tag}/M{mass}/LimitBins"
                sig_path = bkg_path  # same directory for signal and background

                bkg_dir = bkg_file.Get(bkg_path)
                sig_dir = sig_file.Get(sig_path) if sig_file else None

                if not bkg_dir:
                    log_print(f"  [Missing Bkg] {bkg_path}")
                    continue
                if not sig_dir:
                    log_print(f"  [Missing Sig] {sig_path}")
                    continue

                total_fom = 0.0
                log_print(f"\n  >>> {flav}_{version_tag} M{mass}:")
                for key in bkg_dir.GetListOfKeys():
                    hname = key.GetName()
                    if "SingleBin" in hname:
                        continue
                    bkg_hist = bkg_dir.Get(hname)
                    sig_hist = sig_dir.Get(hname)
                    if not bkg_hist or not sig_hist:
                        continue

                    log_print(f"    Histogram: {hname}")
                    hist_fom = 0.0

                    for i in range(1, bkg_hist.GetNbinsX() + 1):
                        bkg = bkg_hist.GetBinContent(i)
                        sig = sig_hist.GetBinContent(i)
                        fom = calculate_fom(sig, bkg, mass)
                        total_fom += fom
                        hist_fom += fom
                        log_print(f"      Bin {i:3d}: Bkg = {bkg:.2f}, Sig = {sig:.2f}, FOM = {fom:.2f}")

                    log_print(f"    [Histogram total FOM] {hname}: {hist_fom:.2f}\n")

                log_print(f"  [SUMMARY] [{flav}_{version_tag} M{mass}] Summed Azimov FOM from LimitBins: {total_fom:.2f}")

def merge_bins(bin_list, merge_indices, mass, calculate_fom):
    merged_bins = []
    group = []
    bkg_sum = sig_sum = err2_sum = 0.0

    merge_set = set(merge_indices)
    i = 0
    while i < len(bin_list):
        g, bkg, sig, rel_err, _ = bin_list[i]
        group.extend(g)
        bkg_sum += bkg
        sig_sum += sig
        err2_sum += (bkg * rel_err) ** 2

        if i not in merge_set:
            rel_err_merged = math.sqrt(err2_sum) / bkg_sum if bkg_sum > 0 else float('inf')
            fom_merged = calculate_fom(sig_sum, bkg_sum, mass)
            merged_bins.append((group[:], bkg_sum, sig_sum, rel_err_merged, fom_merged))
            group = []
            bkg_sum = sig_sum = err2_sum = 0.0

        i += 1

    if group:
        rel_err_merged = math.sqrt(err2_sum) / bkg_sum if bkg_sum > 0 else float('inf')
        fom_merged = calculate_fom(sig_sum, bkg_sum, mass)
        merged_bins.append((group[:], bkg_sum, sig_sum, rel_err_merged, fom_merged))

    return merged_bins

def find_best_bin_merge(strict_bins, n_final_bins, mass, calculate_fom, log_print):
    n_bins = len(strict_bins)
    n_merges = n_bins - n_final_bins

    if n_merges < 0:
        log_print(f"[WARNING] Requested more bins ({n_final_bins}) than available strict bins ({n_bins}). Skipping merge.")
        return None, None, None

    best_total_fom = -float('inf')
    best_merge = None
    best_bins = None

    for merge_points in itertools.combinations(range(n_bins - 1), n_merges):
        merged_bins = merge_bins(strict_bins, list(merge_points), mass, calculate_fom)
        total_fom = sum(b[-1] for b in merged_bins)

        DEBUG = False
        if DEBUG:
            merge_str = ','.join(map(str, merge_points))
            log_print(f"[DEBUG] Merge indices: [{merge_str}] ->Total FOM: {total_fom:.2f}")

        if total_fom > best_total_fom:
            best_total_fom = total_fom
            best_merge = merge_points
            best_bins = merged_bins

    if best_bins is None:
        log_print("[ERROR] No valid bin merging configuration found. Skipping result printout.")
        return None, None, None

    sum_sig = sum(b[2] for b in best_bins)
    sum_bkg = sum(b[1] for b in best_bins)

    log_print(f"\n[RESULT] Best total FOM: {best_total_fom:.2f} using {len(best_bins)} bins with strict-bin merge scan:")
    for i, (group, bkg, sig, rel_err, fom) in enumerate(best_bins):
        x_high = group[0][2]
        x_low = group[-1][1]
        log_print(f"  Bin {i:2d}: X = [{x_high:.3f}, {x_low:.3f}) Bkg = {bkg:.2f}, Sig = {sig:.2f}, RelErr = {rel_err:.2f}, FOM = {fom:.2f}")

    log_print(f"[SUMMARY] Total Signal = {sum_sig:.2f}, Total Background = {sum_bkg:.2f}")
    return best_bins, best_total_fom, best_merge

def select_directory(base_path):
    print(f"[INFO] Scanning for subdirectories in: {base_path}")
    
    directories = sorted([
        d for d in os.listdir(base_path)
        if os.path.isdir(os.path.join(base_path, d))
    ])

    if not directories:
        raise RuntimeError(f"No directories found in {base_path}")

    print("\nAvailable subdirectories:")
    for i, d in enumerate(directories):
        print(f"  [{i}] {d}")
    
    while True:
        try:
            choice = int(input("\nEnter the number of the directory to use: "))
            if 0 <= choice < len(directories):
                selected = os.path.join(base_path, directories[choice])
                print(f"[INFO] Selected directory: {selected}")
                return selected
            else:
                print("[ERROR] Invalid selection.")
        except ValueError:
            print("[ERROR] Please enter a number.")

import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--mass', required=True)
parser.add_argument('--flavour', required=True)
parser.add_argument('--era', default='2016preVFP')
parser.add_argument('--n_bins_final', type=int, default=10, help='Number of bins to use in scan')
parser.add_argument('--n_bins_min', type=int, default=15, help='Number of bins to use in scan')

args = parser.parse_args()
n_bins_min = args.n_bins_min
n_bins_final = args.n_bins_final

mass = args.mass
flavour = args.flavour
era = args.era
            
# Setup
base_dir = '/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter/'
eras = [era]#, '2016postVFP', '2017', '2018']
masses = [mass]#, '200', '300', '400', '500']
flavours = [flavour]#, 'MuMu', 'EMu']
version = 'V4'
os.makedirs("log_scan", exist_ok=True)
dir_path = select_directory(base_dir)

# mergiung criteria
n_bin_minbkg=1.0
n_bin_relerr=0.30

# Loop
for era in eras:
    bkg_path = os.path.join(dir_path, era, 'HNL_SignalRegion_Plotter_Bkg.root')
    if not os.path.exists(bkg_path):
        print_in_color(f"Missing: {bkg_path}", is_red=True)
        continue
    bkg_file = ROOT.TFile.Open(bkg_path)
    if not bkg_file or bkg_file.IsZombie():
        print_in_color(f"Cannot open ROOT file: {bkg_path}", is_red=True)
        continue

    for mass in masses:

        sig_path = os.path.join(dir_path, era, f'HNL_SignalRegion_Plotter_HNL_{mass}.root')
        sig_file = ROOT.TFile.Open(sig_path) if os.path.exists(sig_path) else None

        for flav in flavours:

            flav_dir = f'LimitExtraction/HNL_ULIDv2/{flav}/SR3BDTBDT'
            hist_dir = bkg_file.Get(flav_dir)
            if not hist_dir:
                print_in_color(f"Missing: {flav_dir}", is_red=True)
                continue

            for key in hist_dir.GetListOfKeys():
                name = key.GetName()
                if "Binned" in name or f"M{mass}" not in name or version not in name:
                    continue
                hist = hist_dir.Get(name)
                if not hist or not isinstance(hist, ROOT.TH1):
                    continue

                log_file = f"log_scan/{era}_{flav}_{name}.log"
                with open(log_file, "w") as log:
                    def log_print(msg): print(msg); log.write(msg + "\n")
                    
                    log_print(f"[INFO] Processing: {era} {flav} {name}")
                    compare_limitbins_fom(bkg_file, sig_file, [flav], [mass], log)

                    sig_hist = None
                    if sig_file:
                        sig_dir = sig_file.Get(flav_dir)
                        if sig_dir:
                            sig_hist = sig_dir.Get(name)

                    nbins = hist.GetNbinsX()
                    all_bins = []

                    total_fom_unmerged = 0.0
                    for i in range(nbins, 0, -1):
                        x_low = hist.GetBinLowEdge(i)
                        x_high = hist.GetBinLowEdge(i + 1)
                        bkg = hist.GetBinContent(i)
                        sig = sig_hist.GetBinContent(i) if sig_hist else 0
                        rel_err = hist.GetBinError(i) / bkg if bkg > 0 else 0
                        fom = calculate_fom(sig, bkg,mass)
                        total_fom_unmerged += fom
                        all_bins.append((i, x_low, x_high, bkg, sig, rel_err, fom))
                        if bkg > 0:
                            log_print(f"[BIN {i:3d}] X = [{x_low:.3f}, {x_high:.3f}) "
                                      f"Bkg = {bkg:.2f}, Sig = {sig:.2f}, RelErr = {rel_err*100:.2f}%, FOM = {fom:.2f}")
                    log_print(f"[SUMMARY] Total Azimov FOM with unmerged bins {nbins}: {total_fom_unmerged:.2f}")
                    
                    # Loose merging
                    loose_bins = []
                    accum = []
                    sum_bkg = sum_sig = sum_err2 = 0
                    for f in all_bins:
                        idx, x_low, x_high, bkg, sig, rel_err, fom = f
                        accum.append(f)
                        sum_bkg += bkg
                        sum_sig += sig
                        sum_err2 += (bkg * rel_err)**2
                        rel_err_accum = math.sqrt(sum_err2) / sum_bkg if sum_bkg > 0 else 0
                        if sum_bkg > n_bin_minbkg or rel_err_accum < n_bin_relerr and sum_bkg >= 0.5:
                            loose_bins.append((accum[:], sum_bkg, sum_sig, rel_err_accum))
                            accum.clear(); sum_bkg = sum_sig = sum_err2 = 0
                    if accum:
                        loose_bins.append((accum[:], sum_bkg, sum_sig, rel_err_accum))

                    log_print(f"\n[INFO] Loose merged bins (right to left):")
                    total_fom_loose = 0.0
                    for bin_num, (group, sum_bkg, sum_sig, rel_err) in enumerate(loose_bins, 1):
                        x_high = group[0][2]; x_low = group[-1][1]
                        fom = calculate_fom(sum_sig, sum_bkg,mass)
                        total_fom_loose += fom
                        log_print(f"  Bin {bin_num:3d}: X = [{x_high:.3f}, {x_low:.3f}) Bkg = {sum_bkg:.2f}, Sig = {sum_sig:.2f}, RelErr = {rel_err*100:.2f}%, FOM = {fom:.2f}")
                    log_print(f"[SUMMARY] Total Azimov FOM after loose merge {len(loose_bins)}: {total_fom_loose:.2f}")

                    # Start from the initial threshold percentage
                    threshold_percent = 2.5
                    while True:
                        threshold = (threshold_percent / 100.0) * total_fom_loose
                        log_print(f"\n[INFO] Trying threshold = {threshold:.4f} ({threshold_percent:.1f}% of loose FOM)")
                        
                        strict_bins = []
                        current_group = []
                        bkg_sum = sig_sum = err2_sum = 0.0

                        for group, sum_bkg, sum_sig, rel_err in loose_bins:
                            current_group.extend(group)
                            bkg_sum += sum_bkg
                            sig_sum += sum_sig
                            err2_sum += (sum_bkg * rel_err) ** 2
        
                            rel_err_accum = math.sqrt(err2_sum) / bkg_sum if bkg_sum > 0 else float('inf')
                            fom_accum = calculate_fom(sig_sum, bkg_sum, mass)

                            if fom_accum >= threshold:
                                strict_bins.append((current_group[:], bkg_sum, sig_sum, rel_err_accum, fom_accum))
                                current_group.clear()
                                bkg_sum = sig_sum = err2_sum = 0.0

                        # Final fallback bin
                        if current_group:
                            rel_err_accum = math.sqrt(err2_sum) / bkg_sum if bkg_sum > 0 else float('inf')
                            fom_accum = calculate_fom(sig_sum, bkg_sum, mass)
                            strict_bins.append((current_group[:], bkg_sum, sig_sum, rel_err_accum, fom_accum))

                        log_print(f"[INFO] Number of strict bins = {len(strict_bins)} at {threshold_percent:.1f}% threshold")

                        # Check if we have reached the required number of bins
                        if len(strict_bins) >= n_bins_min or threshold_percent <= 0.0:
                            break

                        # Lower threshold percentage and retry
                        threshold_percent *= 0.95

                    # Print strict bins
                    total_fom_strict = 0.0
                    first_bin_fom = last_bin_fom = None

                    log_print(f"\n[INFO] Strict merged bins (right to left):")
                    for i, (group, sum_bkg, sum_sig, rel_err, fom) in enumerate(strict_bins, 1):
                        x_high = group[0][2]
                        x_low = group[-1][1]
                        log_print(f"  Bin {i:3d}: X = [{x_high:.3f}, {x_low:.3f}) Bkg = {sum_bkg:.2f}, Sig = {sum_sig:.2f}, RelErr = {rel_err*100:.2f}%, FOM = {fom:.2f}")
                        total_fom_strict += fom
                        if i == 1:
                            first_bin_fom = fom
                        if i == len(strict_bins):
                            last_bin_fom = fom

                    log_print(f"[SUMMARY] Total Azimov FOM after strict merge: {total_fom_strict:.2f}")

                    log_print(f"[DEBUG] strict_bins count = {len(strict_bins)}")
                    log_print(f"[DEBUG] Target final bins = {n_bins_final}")
                    
                    merged_final_bins, best_fom_final, merge_indices = find_best_bin_merge(
                        strict_bins,
                        n_bins_final,
                        mass,
                        calculate_fom,
                        log_print
                    )

                    
                    # Fix final bin to -1.000 if needed
                    if strict_bins:
                        last_bin = strict_bins[-1][0]
                        x_low_last = last_bin[-1][1]
                        if x_low_last > -1.0:
                            last_bin[-1] = (*last_bin[-1][:1], -1.000, *last_bin[-1][3:])

                    log_print("\n[INFO] Final merged bin boundaries:")
                    for i, (group, *_rest) in enumerate(strict_bins, 1):
                        x_high = group[0][2]
                        x_low = group[-1][1]
                        log_print(f"  Bin {i:3d}: X = [{x_high:.3f}, {x_low:.3f})")

                        

                        
                        
    bkg_file.Close()
    if sig_file:
        sig_file.Close()

print("\n[Done] Bin merging analysis complete.")
