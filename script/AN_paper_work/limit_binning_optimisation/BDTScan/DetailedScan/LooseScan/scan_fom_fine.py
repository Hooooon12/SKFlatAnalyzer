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
    if mass == "200":
        s = s / 100
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

    log_print("\n[INFO] Comparing summed FOMs from LimitBins histograms:")
    for flav in flavours:
        for mass in masses:
            for version_tag in ["V2", "V3"]:
                bkg_path = f"LimitExtractionBDT/HNL_ULIDv2/{flav}_{version_tag}/M{mass}/LimitBins"
                sig_path = f"LimitExtractionBDT/HNL_ULIDv2/{flav}_{version_tag}/M{mass}/LimitBins"

                bkg_dir = bkg_file.Get(bkg_path)
                sig_dir = sig_file.Get(sig_path) if sig_file else None

                if not bkg_dir:
                    log_print(f"  [Missing Bkg] {bkg_path}")
                    continue
                if not sig_dir:
                    log_print(f"  [Missing Sig] {sig_path}")
                    continue

                total_fom = 0.0
                for key in bkg_dir.GetListOfKeys():
                    hname = key.GetName()
                    if "SingleBin" in hname:
                        continue
                    bkg_hist = bkg_dir.Get(hname)
                    sig_hist = sig_dir.Get(hname)
                    if not bkg_hist or not sig_hist:
                        continue
                    for i in range(1, bkg_hist.GetNbinsX() + 1):
                        bkg = bkg_hist.GetBinContent(i)
                        sig = sig_hist.GetBinContent(i)
                        total_fom += calculate_fom(sig, bkg, mass)
                log_print(f"  [{flav}_{version_tag} M{mass}] Summed Azimov FOM from LimitBins: {total_fom:.2f}")


def scan_optimal_variable_binning_dual_step(hist, sig_hist, log, nbins, 
                                             x_min, x_split, x_max, 
                                             step1, step2, edge_bin_fom,max_trials,mass):
    def log_print(msg): print(msg); log.write(msg + "\n")


    # Create edges for each range                                                                                            
    edges_low = np.arange(x_min + step1, x_split, step1)
    edges_high = np.arange(x_split, x_max, step2)
    all_edges = np.concatenate((edges_low, edges_high))

    # Print configuration info                                                                                               
    log_print(f"\n[SCAN] x_min = {x_min:.3f}, x_split = {x_split:.3f}, x_max = {x_max:.3f}")
    log_print(f"[SCAN] Step1 = {step1:.3f}, Step2 = {step2:.3f}")
    log_print(f"[SCAN] Edges low ({len(edges_low)}): {edges_low}")
    log_print(f"[SCAN] Edges high ({len(edges_high)}): {edges_high}")
    log_print(f"\n[SCAN] Total candidate internal edges: {len(all_edges)}")

    total_possible = int(math.comb(len(all_edges), nbins - 1))
    num_to_sample = min(max_trials, total_possible)

    log_print(f"[SCAN] Total combinations for {nbins} bins: {total_possible:,}")
    log_print(f"[SCAN] Will sample {num_to_sample:,} combinations randomly")

    # Get random combinations                                                                                                
    combinations = get_random_combinations(all_edges, nbins - 1, num_to_sample)

    best_fom = edge_bin_fom
    best_edges = None

    start_time = time.time()

    for i, mids in enumerate(combinations):
        if i % 1000 == 0:
            log_print(f"[SCAN] Progress: {i}/{num_to_sample} | Current Best FOM = {best_fom:.2f}")

        bin_edges = [x_min] + list(mids) + [x_max]
        fom_sum = edge_bin_fom
        valid_scan = True

        for j in range(nbins):
            x_low = bin_edges[j]
            x_high = bin_edges[j + 1]
            bkg = sig = sumw2 = 0.0

            for k in range(1, hist.GetNbinsX() + 1):
                center = hist.GetBinCenter(k)
                if x_low <= center < x_high:
                    b = hist.GetBinContent(k)
                    s = sig_hist.GetBinContent(k) if sig_hist else 0
                    bkg += b
                    sig += s
                    sumw2 += hist.GetBinError(k) ** 2

            rel_err = math.sqrt(sumw2) / bkg if bkg > 0 else float('inf')
            fom = calculate_fom(sig, bkg, mass)

            # Bin quality filters                                                                                                                                                                                  
                                                                                                                                                                    
            if bkg < 1.5 or not (rel_err < 0.1 or bkg > 2.0) or fom == 0:
                valid_scan = False
                break

            fom_sum += fom

        if valid_scan and fom_sum > best_fom:
            best_fom = fom_sum
            best_edges = bin_edges

    elapsed = time.time() - start_time
    log_print(f"[SCAN] Finished scan in {elapsed:.1f} seconds.")
    log_print(f"[SCAN] Best FOM found: {best_fom:.2f}")

    log_print(f"\n[RESULT] Best total FOM: {best_fom:.2f} using {nbins} bins with dual-step scan:")
    if best_edges:
        # Bin 0 (below x_min)                                                                                                                                                                                                                                                                                                                                                          
        bkg_below = sig_below = err2_below = 0.0
        for i in range(1, hist.GetNbinsX() + 1):
            if hist.GetBinLowEdge(i + 1) <= x_min:
                b = hist.GetBinContent(i)
                s = sig_hist.GetBinContent(i)
                bkg_below += b
                sig_below += s
                err2_below += hist.GetBinError(i) ** 2
        err_below = math.sqrt(err2_below) if bkg_below > 0 else 0
        fom_below = calculate_fom(sig_below, bkg_below, mass)
        log_print(f"  Bin  0: X = [{x_min:.3f}, -1.000) "
                  f"Bkg = {bkg_below:.2f}, Sig = {sig_below:.2f}, Err = {err_below:.2f}, FOM = {fom_below:.2f}")

        # Main scanned bins                                                                                                                                                                                                                                                                                                                                                            
        for i in range(nbins):
            x_high = best_edges[i + 1]
            x_low = best_edges[i]
            bkg = sig = err2 = 0.0
            for j in range(1, hist.GetNbinsX() + 1):
                x = hist.GetBinCenter(j)
                if x_low <= x < x_high:
                    b = hist.GetBinContent(j)
                    s = sig_hist.GetBinContent(j)
                    bkg += b
                    sig += s
                    err2 += hist.GetBinError(j) ** 2
            err = math.sqrt(err2) if bkg > 0 else 0
            fom = calculate_fom(sig, bkg, mass)
            log_print(f"  Bin {i+1:2d}: X = [{x_high:.3f}, {x_low:.3f}) "
                      f"Bkg = {bkg:.2f}, Sig = {sig:.2f}, Err = {err:.2f}, FOM = {fom:.2f}")

        # Bin nbins+1 (above x_max)                                                                                                                                                                                                                                                                                                                                                    
        bkg_above = sig_above = err2_above = 0.0
        for i in range(1, hist.GetNbinsX() + 1):
            if hist.GetBinLowEdge(i) >= x_max:
                b = hist.GetBinContent(i)
                s = sig_hist.GetBinContent(i)
                bkg_above += b
                sig_above += s
                err2_above += hist.GetBinError(i) ** 2
        err_above = math.sqrt(err2_above) if bkg_above > 0 else 0
        fom_above = calculate_fom(sig_above, bkg_above, mass)
        log_print(f"  Bin {nbins+1:2d}: X = [1.000, {x_max:.3f}) "
                  f"Bkg = {bkg_above:.2f}, Sig = {sig_above:.2f}, Err = {err_above:.2f}, FOM = {fom_above:.2f}")



    return best_edges, best_fom
    
                
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
    print(f"[INFO] Selected newest directory: {newest_dir}")
    return newest_dir


import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--mass', required=True)
parser.add_argument('--flavour', required=True)
parser.add_argument('--era', default='2016preVFP')
parser.add_argument('--n_bins_scan', type=int, default=15, help='Number of bins to use in scan')
parser.add_argument('--n_maxiter_scan', type=int, default=50000, help='Maximum number of scan iterations')

args = parser.parse_args()
n_bins_scan = args.n_bins_scan
max_trials = args.n_maxiter_scan

mass = args.mass
flavour = args.flavour
era = args.era
            
# Setup
base_dir = '/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter/'
eras = [era]#, '2016postVFP', '2017', '2018']
masses = [mass]#, '200', '300', '400', '500']
flavours = [flavour]#, 'MuMu', 'EMu']
version = 'V3'
os.makedirs("log_scan", exist_ok=True)
dir_path = select_directory(base_dir)

# mergiung criteria
n_bin_minbkg=2.0
n_bin_relerr=0.10

lower_bdt_interval=0.01
upper_bdt_interval=0.005

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
                        if sum_bkg > n_bin_minbkg or rel_err_accum < n_bin_relerr and sum_bkg >= 1.5:
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


                    # Strict merge based on dynamic threshold
                    threshold = 0.025 * total_fom_loose
                    log_print(f"\n[INFO] Dynamic strict FOM threshold = {threshold:.2f} (2.5% of loose FOM)")
                    strict_bins = []
                    
                    current_group = []
                    bkg_sum = sig_sum = err2_sum = 0.0
                    
                    for group, sum_bkg, sum_sig, rel_err in loose_bins:
                        # Append this group of bins to current accumulation
                        current_group.extend(group)
                        bkg_sum += sum_bkg
                        sig_sum += sum_sig
                        err2_sum += (sum_bkg * rel_err) ** 2
                        
                        rel_err_accum = math.sqrt(err2_sum) / bkg_sum if bkg_sum > 0 else float('inf')
                        fom_accum = calculate_fom(sig_sum, bkg_sum,mass)

                        # If merged bin passes threshold, save it
                        if fom_accum >= threshold:
                            strict_bins.append((current_group[:], bkg_sum, sig_sum, rel_err_accum, fom_accum))
                            current_group.clear()
                            bkg_sum = sig_sum = err2_sum = 0.0
                            
                    # Final fallback bin (if leftover)
                    if current_group:
                        rel_err_accum = math.sqrt(err2_sum) / bkg_sum if bkg_sum > 0 else float('inf')
                        fom_accum = calculate_fom(sig_sum, bkg_sum,mass)
                        strict_bins.append((current_group[:], bkg_sum, sig_sum, rel_err_accum, fom_accum))

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

                    x_high_strict = strict_bins[0][0][-1][1]  # first strict bin high edge
                    x_low_strict = strict_bins[-1][0][0][2]  # last strict bin low edge
                    
                    first_last_bin_fom = first_bin_fom+last_bin_fom
                    scan_optimal_variable_binning_dual_step(
                        hist, sig_hist, log,
                        n_bins_scan,
                        x_low_strict,
                        (x_low_strict + x_high_strict) / 2,
                        x_high_strict,
                        lower_bdt_interval,
                        upper_bdt_interval,
                        first_last_bin_fom,
                        max_trials,
                        mass
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
