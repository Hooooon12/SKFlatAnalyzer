#!/usr/bin/env python3

import os
import sys
import math
import ROOT
import time
import numpy as np
import random

ROOT.gROOT.SetBatch(True)

FAKE_FLOOR = 0.15 * 0.645

class Tee:
    def __init__(self, *files):
        self.files = files

    def write(self, obj):
        for f in self.files:
            f.write(obj)
            f.flush()

    def flush(self):
        for f in self.files:
            f.flush()
            

def format_edges_str(edges, ndigits=3):

    out = []

    for x in edges:
        if abs(x) < 1e-10:
            x = 0.0

        out.append(f"{x:.{ndigits}f}")

    return "[" + ", ".join(out) + "]"


def find_last_n_bins(histograms,bin_cache, n_tail_bins=2, x_min=-0.4, x_max=0.4):

    edges = [x_max]

    current_high = x_max

    for _ in range(n_tail_bins):

        found = False

        # scan downward in your grid
        for x in np.arange(current_high - 0.005, x_min, -0.005):

            run2_bkg = 0.0
            valid = True

            for era, (centers, cumsum_b, cumsum_f, _, edge_indices) in bin_cache.items():

                bkg = 0.0
                fake = 0.0

                edge_grid = np.arange(-0.4, 0.405, 0.005)

                idx_low  = np.searchsorted(edge_grid, x, side='left')
            
                idx_high = np.searchsorted(edge_grid, current_high, side='left')

                
                i_low  = edge_indices[idx_low]
                
                i_high = edge_indices[idx_high]
                
                bkg  = cumsum_b[i_high-1] - (cumsum_b[i_low-1] if i_low > 0 else 0.0)
                
                fake = cumsum_f[i_high-1] - (cumsum_f[i_low-1] if i_low > 0 else 0.0)

                fake, bkg = fix_fake_and_bkg(fake, bkg, FAKE_FLOOR)

                if bkg < 0.15:
                    valid = False
                    break

                run2_bkg += bkg

            if valid and run2_bkg >= 1:
                edges.append(x)
                current_high = x
                found = True
                break

        if not found:
            return None  # failed

    return sorted(edges)

def build_cumsum_cache(bin_cache, all_edges):

    new_cache = {}

    edge_grid = np.round(np.arange(-0.4, 0.405, 0.005), 5)
    
    for era, (centers, bvals, fvals, svals) in bin_cache.items():

        order = np.argsort(centers)

        centers = centers[order]

        bvals = bvals[order]

        fvals = fvals[order]

        svals = svals[order]

        cumsum_b = np.cumsum(bvals)

        cumsum_f = np.cumsum(fvals)

        cumsum_s = np.cumsum(svals)

       
        edge_index_array = np.searchsorted(centers, edge_grid, side='left')
        
        new_cache[era] = (

            centers,

            cumsum_b,

            cumsum_f,

            cumsum_s,

            edge_index_array

        )

    return new_cache
        

# ---------------------------------------
# Fake correction
# ---------------------------------------
def fix_fake_and_bkg(F, B, FAKE_FLOOR, flavour=None, era=None, debug=False):
    if F <= 0:
        delta = FAKE_FLOOR - F
        F = FAKE_FLOOR
        B += delta
    # If B is still < 0 other bkg are negative and so bin set to FAKE_FLOOR
    if B < FAKE_FLOOR:
        B=FAKE_FLOOR
    return F, B

# ---------------------------------------
# Asimov FOM
# ---------------------------------------
def calculate_fom(s, b):
    if b <= 0:
        return 0
    term = (s + b) * math.log(1 + s / b) - s
    return 2 * term if term > 0 else 0


# ---------------------------------------
#  Print Tables
# ---------------------------------------
def print_run2_bkg_per_edge(histograms, edges_all, x_max=0.4):

    print("\n=== RUN2 BKG PER EDGE ===")

    edges = sorted(edges_all)
    edges = list(edges) + [x_max]  # close final bin

    for i in range(len(edges) - 1):

        x_low  = edges[i]
        x_high = edges[i+1]

        run2_bkg = 0.0
        run2_fake = 0.0
        run2_sig = 0.0

        for era, (h_bkg, h_sig, h_fake) in histograms.items():

            bkg = 0.0
            fake = 0.0
            sig = 0.0

            for ibin in range(1, h_bkg.GetNbinsX() + 1):

                x = h_bkg.GetBinCenter(ibin)

                if not (x_low <= x < x_high):
                    continue

                b = h_bkg.GetBinContent(ibin)
                f = h_fake.GetBinContent(ibin)
                s = h_sig.GetBinContent(ibin)

                bkg  += b
                fake += f
                sig  += s

            # apply fake correction AFTER summing
            #fake, bkg = fix_fake_and_bkg(fake, bkg, FAKE_FLOOR)

            run2_bkg  += bkg
            run2_fake += fake
            run2_sig  += sig

        print(f"[EDGE {i:2d}] [{x_low:.3f}, {x_high:.3f})  "
              f"B={run2_bkg:.3f}  F={run2_fake:.3f}  S={run2_sig:.3f}")

# ---------------------------------------
# Random combinations
# ---------------------------------------
def get_random_combinations(edges, k, n_samples):

    edges = list(edges)

    for _ in range(n_samples):

        yield tuple(sorted(random.sample(edges, k)))
        
def get_unique_random_combinations(edges, k, n_samples):
    edges = list(edges)
    seen = set()
    while len(seen) < n_samples:
        combo = tuple(sorted(random.sample(edges, k)))
        if combo not in seen:
            seen.add(combo)
            yield combo

# ---------------------------------------
# Evaluate the bin combination
# ---------------------------------------
            
def evaluate_combination(args):

    try:
        
        return evaluate_combination_fnct(args)

    except Exception as e:

        print("[WORKER ERROR]", e)

        return None
            
def evaluate_combination_fnct(args):
    mids, bin_cache, nbins, x_min, x_max,scan_era, tail_edges = args

    edges = [x_min] + list(mids) + tail_edges
    edges = sorted(set(edges))

    tail_start = tail_edges[0]
    
    total_fom = 0
    valid = True

    for j in range(len(edges) - 1):
        x_low  = edges[j]

        x_high = edges[j+1]

        is_tail_bin = (x_low >= tail_edges[0])

        run2_sig = 0
        run2_bkg = 0
        scan_sig = 0
        scan_bkg = 0

        for era, (centers, cumsum_b, cumsum_f, cumsum_s, edge_indices) in bin_cache.items():

            #centers, cumsum_b, cumsum_f, cumsum_s = bin_cache[era]

            edge_grid = np.arange(-0.4, 0.405, 0.005)

            idx_low  = np.searchsorted(edge_grid, x_low, side='left')
            
            idx_high = np.searchsorted(edge_grid, x_high, side='left')
            
            if idx_low < 0 or idx_high >= len(edge_indices):
                
                return None
            
            i_low  = edge_indices[idx_low]
            
            i_high = edge_indices[idx_high]


            bkg  = cumsum_b[i_high-1] - (cumsum_b[i_low-1] if i_low > 0 else 0.0)

            fake = cumsum_f[i_high-1] - (cumsum_f[i_low-1] if i_low > 0 else 0.0)
            
            sig  = cumsum_s[i_high-1] - (cumsum_s[i_low-1] if i_low > 0 else 0.0)


            
            fake, bkg = fix_fake_and_bkg(fake, bkg, FAKE_FLOOR)

            if is_tail_bin:
                if bkg < 0.15:
                    sys.exit()
            else:
                if bkg < 0.15:
                    return None

            run2_sig += sig
            run2_bkg += bkg
            if scan_era == "Run2" or era == scan_era:
                scan_sig += sig
                scan_bkg += bkg

        
        if run2_bkg < 1:
            return None

        total_fom += calculate_fom(scan_sig, scan_bkg)
        
    return (total_fom, edges)


# ---------------------------------------
# Find tail cut  that reduces scan load
# ---------------------------------------

def find_tail_cut(histograms, n_consecutive=0):
    
    test_edges = np.arange(0.4, -0.4, -0.005)

    pass_count = 0

    for x in test_edges:

        run2_bkg = 0
        valid = True

        for era, (h, hs, hf) in histograms.items():

            bkg = 0
            fake = 0
            for ibin in range(1, h.GetNbinsX()+1):
                
                xc = h.GetBinCenter(ibin)

                if not (x <= xc <= 0.4):
                    continue

                b = h.GetBinContent(ibin)
                f = hf.GetBinContent(ibin)

                #f, b = fix_fake_and_bkg(f, b, FAKE_FLOOR)

                bkg += b
                fake += f

            fake,bkg = fix_fake_and_bkg(fake, bkg, FAKE_FLOOR)   
            if bkg < 0.15:
                valid = False
                break

            run2_bkg += bkg

        if valid and run2_bkg >= 1:
            pass_count += 1
            if pass_count >= n_consecutive:
                print(f"bkg={bkg} x = {x} ")
                return x
        else:
            pass_count = 0

    return 0.4

# ---------------------------------------
# Build edges for comb list
# ---------------------------------------

def build_edges(Xcut):
    
    tmp_edges_low  = np.arange(0.0, 0.1, 0.02)
    
    tmp_edges_med  = np.arange(0.1, 0.2, 0.01)

    tmp_edges_high = np.arange(0.2, 0.4, 0.005)

    tmp_edges_all = np.concatenate((tmp_edges_low, tmp_edges_med, tmp_edges_high))

    tmp_edges_all = np.unique(np.round(tmp_edges_all, 5))

    return tmp_edges_all
    

def build_edges_from_tailcut(Xcut):

    edges_low = [-0.05]

    # --- case 1: very tight tail ---
    if Xcut < 0.25:

        edges_med = np.arange(0.0, 0.06, 0.02)
        edges_high = np.arange(0.06, Xcut -0.07, 0.01)
        edges_vhigh = np.arange(Xcut-0.07, Xcut , 0.005)

        edges_all = np.concatenate((edges_low, edges_med,edges_high, edges_vhigh))


    # --- case 2: medium tail ---
    elif Xcut < 0.30:

        edges_med  = np.arange(0.0, 0.10, 0.02)
        edges_high = np.arange(0.10, Xcut -0.07, 0.01)
        edges_vhigh = np.arange(Xcut-0.07, Xcut , 0.005)
        
        edges_all = np.concatenate((edges_low, edges_med, edges_high,edges_vhigh))


    # --- case 3: long tail (your main case) ---
    else:

        edges_med  = np.arange(0.0, 0.16, 0.02)

        # bulk region
        edges_high = np.arange(0.16, Xcut - 0.10, 0.01)

        # dense region near cutoff (important!)
        edges_vhigh = np.arange(Xcut - 0.10, Xcut , 0.005)

        edges_all = np.concatenate((edges_low, edges_med, edges_high, edges_vhigh))


    # --- cleanup ---
    edges_all = np.unique(np.round(edges_all, 5))

    return edges_all

# ---------------------------------------
# Scan binning (multi-era)
# ---------------------------------------
from multiprocessing import Pool, cpu_count


def scan_optimal_variable_binning(scan_era, histograms, nbins, max_trials, flav=None):

    # ---------------------------------------

    # Step 1: build bin_cache FIRST

    # ---------------------------------------

    bin_cache = {}

    for era, (h, hs, hf) in histograms.items():

        centers = []

        bvals = []

        fvals = []

        svals = []

        for ibin in range(1, h.GetNbinsX()+1):

            x = h.GetBinCenter(ibin)

            if not (-0.4 <= x <= 0.4):

                continue

            centers.append(x)

            bvals.append(h.GetBinContent(ibin))

            fvals.append(hf.GetBinContent(ibin))

            svals.append(hs.GetBinContent(ibin))

        bin_cache[era] = (

            np.array(centers),

            np.array(bvals),

            np.array(fvals),

            np.array(svals),

        )

    all_edges = np.round(np.arange(-0.4, 0.405, 0.005), 5)

    bin_cache = build_cumsum_cache(bin_cache, all_edges)

    # ---------------------------------------

    # Step 2: NOW find tail bins

    # ---------------------------------------

    tail_edges = find_last_n_bins(histograms, bin_cache, n_tail_bins=2)

    if tail_edges is None:

        print("[ERROR] Could not determine tail bins")

        return None, 0

    print("[INFO] Fixed tail bins:", tail_edges)

    # ---------------------------------------

    # Step 3: continue as before

    # ---------------------------------------

    nbins_total = nbins

    nbins_scan = nbins_total - 2

    k = nbins_scan - 1

    x_min = -0.4

    x_max = tail_edges[0]

    # ---------------------------------------
    # --- edges ---
    # ---------------------------------------
    from math import comb
    
    edges_simple = build_edges(x_max)
    print_run2_bkg_per_edge(histograms, edges_simple)
    N_precut_simple = len(edges_simple)
    print(f"[INFO] simple edges before tail cut: {N_precut_simple}")
    ncomb_simple_before = comb(N_precut_simple, k) if N_precut_simple >= k else 0
    
    ### Remove post tail edges
    edges_simple = edges_simple[edges_simple<x_max]
    N_simple = len(edges_simple)

    ncomb_simple_after = comb(N_simple, k) if N_simple >= k else 0
    print(f"[INFO] edges after tail cut: {N_simple}")
    print(f"[INFO] combinations simple: {ncomb_simple_after:.3e}")

    print(f"Skimmed simple bins : {format_edges_str(edges_simple)}")

    edges_all    = build_edges_from_tailcut(x_max)
    
    edges_all = edges_all[edges_all < x_max]
    
    print(f"All bins : {format_edges_str(edges_all)}")
    print_run2_bkg_per_edge(histograms, edges_all)

    N_after = len(edges_all)
    
    ncomb_after = comb(N_after, k) if N_after >= k else 0
    print(f"[INFO] edges after tail cut: {N_after}")
    print(f"[INFO] combinations after cut: {ncomb_after:.3e}")
    print(f"Skimmed bins : {format_edges_str(edges_all)}")

    print(f"[DEBUG] len(edges_all) = {len(edges_all)}, k = {k}")
    

    from itertools import combinations
    from math import comb
    
    # ---------- edges_all ----------
    max_possible = comb(len(edges_all), k)

    if max_possible <= max_trials:
        print(f"[INFO] Full scan (all): {max_possible}")
        comb_all = combinations(edges_all, k)
        n_trials_eff = max_possible
    else:
        n_trials_eff = max_trials
        comb_all = get_random_combinations(edges_all, k, n_trials_eff)
    
        
    # ---------- edges_simple ----------
    max_simple_possible = comb(len(edges_simple), k)
    
    if max_simple_possible <= max_trials:
        print(f"[INFO] Full scan (simple): {max_simple_possible}")
        comb_simple = combinations(edges_simple, k)
        n_trials_simple_eff = max_simple_possible
    else:
        n_trials_simple_eff = max_trials
        comb_simple = get_random_combinations(edges_simple, k, n_trials_simple_eff)
        
    
    total_trials = n_trials_eff + n_trials_simple_eff

    ncpu = 6
    
    print(f"[INFO] Using {ncpu} cores")
    
    args_case1 = (
    
        (mids, bin_cache, nbins, x_min, x_max, scan_era,tail_edges)
        
        for mids in comb_simple

    )
    args_case2 = (
        
        (mids, bin_cache, nbins, x_min, x_max,scan_era,tail_edges)
        
        for mids in comb_all

    )
    
    best_fom = -1

    best_edges = None
    
    chunksize = max(1, total_trials // (ncpu * 20))
    last_print_time = time.time()
    with Pool(ncpu) as pool:

        for i, result in enumerate(
                pool.imap_unordered(evaluate_combination, args_case1,chunksize=2000)
        ):
            
            if i % 100000 == 0:
                now = time.time()

                dt = now - last_print_time
                
                last_print_time = now
                
                print(f"[SCAN1] {i}/{max_trials}, best={best_fom:.3f}, dt={dt:.2f}s")
               
            if result is None:
                continue
            fom, edges = result
        
            if fom > best_fom:
                best_fom = fom
                best_edges = edges

    with Pool(ncpu) as pool:

        for i, result in enumerate(
                pool.imap_unordered(evaluate_combination, args_case2,chunksize=2000)
        ):

            if i % 20000 == 0:
                now = time.time()
                    
                dt = now - last_print_time
                
                last_print_time = now
                
                print(f"[SCAN2] {i}/{max_trials}, best={best_fom:.3f}, dt={dt:.2f}s")
                
            if result is None:
                continue

            fom, edges = result

            if fom > best_fom:
                best_fom = fom
                best_edges = edges

                
    return best_edges, best_fom
    


# ---------------------------------------
# Existing binning FOM
# ---------------------------------------
def compute_existing_limitbins_fom(histograms, mass, flavour, base_dir):

    if flavour == "MuMu":
        hname = "MuonSR3BDT"
    elif flavour == "EE":
        hname = "ElectronSR3BDT"
    else:
        hname = "ElectronMuonSR3BDT"

    # ---------------------------------------
    # Load all hists first
    # ---------------------------------------
    era_hists = {}

    for era in eras:

        print(f"[INFO] Loading {era}")

        f_bkg  = ROOT.TFile.Open(f"{base_dir}/{era}/HNL_SignalRegion_Plotter_Bkg.root")
        f_sig  = ROOT.TFile.Open(f"{base_dir}/{era}/HNL_SignalRegion_Plotter_HNL_{mass}.root")
        f_fake = ROOT.TFile.Open(f"{base_dir}/{era}/HNL_SignalRegion_Plotter_Fake.root")

        path = f"LimitExtractionBDT/HNL_ULIDv2/{flavour}_V3_Strict_15_Bin/M{mass}/LimitBins/{hname}"

        h_bkg  = f_bkg.Get(path)
        h_sig  = f_sig.Get(path)
        #h_sig.Scale(1/h_sig.Integral())
        h_fake = f_fake.Get(path)
        
        h_bkg.SetDirectory(0)
        
        h_sig.SetDirectory(0)
        
        h_fake.SetDirectory(0)
        if h_bkg is None:

            print(f"[FATAL] h_bkg missing for {era}")
            
            f_bkg.ls()
            
            continue

        if h_sig is None:
            
            print(f"[FATAL] h_sig missing for {era}")
            
            f_sig.ls()
            
            continue
        
        if h_fake is None:
            
            print(f"[FATAL] h_fake missing for {era}")
        
            f_fake.ls()
            
            continue

        # extra protection (PyROOT weirdness)

        if not hasattr(h_bkg, "GetNbinsX"):

            print(f"[FATAL] h_bkg invalid object for {era}")
            
            continue
        
        if h_bkg is None:
            print(f"[ERROR] Missing h_bkg: {era}")
        if h_sig is None:
            print(f"[ERROR] Missing h_sig: {era}")
        if h_fake is None:
            print(f"[ERROR] Missing h_fake: {era}")
        if h_bkg is None or h_sig is None or h_fake is None:
            print(f"[WARNING] Missing hist for {era}")
            continue

        era_hists[era] = (h_bkg, h_sig, h_fake)

    if not era_hists:
        print("[ERROR] No histograms loaded")
        return 0

    # ---------------------------------------
    # Loop over bins FIRST
    # ---------------------------------------
    nbins = None

    for era, (h_bkg, _, _) in era_hists.items():
        
        if h_bkg is not None:
            
            nbins = h_bkg.GetNbinsX()
            
            break

    if nbins is None:
        
        print("[ERROR] No valid histograms found")
        
        return 0

    total_fom_run2 = 0.0
    total_fom_per_era = {era: 0.0 for era in era_hists}

    print("\n=== BIN-BY-BIN FOM ===")

    for i in range(1, nbins + 1):

        run2_sig = 0.0
        run2_bkg = 0.0

        # --- loop eras ---
        for era, (h_bkg, h_sig, h_fake) in era_hists.items():

            b = h_bkg.GetBinContent(i)
            s = h_sig.GetBinContent(i)
            f = h_fake.GetBinContent(i)

            # fake correction per bin (same as existing logic)
            f, b = fix_fake_and_bkg(f, b, FAKE_FLOOR)

            # accumulate Run2
            run2_sig += s
            run2_bkg += b

            if b < 0.15:
                print(f"[BIN {i:3d}] Existing Failed B={b:.2f}") 
            # per-era FOM
            fom_era = calculate_fom(s, b)
            total_fom_per_era[era] += fom_era
            
        # --- Run2 FOM ---
        if run2_bkg > 0:
            
            fom_run2 = calculate_fom(run2_sig, run2_bkg)
            total_fom_run2 += fom_run2
        else:
            fom_run2 = 0

        print(f"[BIN {i:3d}] Existing Run2: S={run2_sig:.2f}, B={run2_bkg:.2f}, FOM={fom_run2:.3f}")

    # ---------------------------------------
    # Summary
    # ---------------------------------------
    print("\n=== SUMMARY ===")

    print(f"[Run2] Total FOM (sum Z^2) = {total_fom_run2:.3f}")

    for era in total_fom_per_era:
        print(f"[{era}] Total FOM (sum Z^2) = {total_fom_per_era[era]:.3f}")

    return total_fom_run2


# ---------------------------------------
# MAIN
# ---------------------------------------



import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--mass', required=True)
parser.add_argument('--flavour', required=True)
parser.add_argument('--n_bins', type=int, default=15)
parser.add_argument('--n_trials', type=int, default=5000000)
args = parser.parse_args()

mass = args.mass
flavour = args.flavour



log_name = f"logs/scan_log_mass{mass}_{flavour}.txt"

log_file = open(log_name, "w")

sys.stdout = Tee(sys.stdout, log_file)

sys.stderr = Tee(sys.stderr, log_file)

print(f"[LOG] Writing output to {log_name}")




base_dir = "/data9/Users/HNL_public/SUS-24-014/SKFlatOutput/Scan/BDT/"
eras = ["2016preVFP","2016postVFP","2017","2018"]

histograms = {}

# Load histograms
def prepare_histogram(h, xmin=-0.4, xmax=0.4):

    h = h.Clone()
    h.SetDirectory(0)

    nb = h.GetNbinsX()

    # --- merge underflow into first bin ---
    uf = h.GetBinContent(0)
    uf_err = h.GetBinError(0)

    h.SetBinContent(1, h.GetBinContent(1) + uf)
    h.SetBinError(1, math.sqrt(h.GetBinError(1)**2 + uf_err**2))

    # --- merge overflow into last bin ---
    of = h.GetBinContent(nb+1)
    of_err = h.GetBinError(nb+1)

    h.SetBinContent(nb, h.GetBinContent(nb) + of)
    h.SetBinError(nb, math.sqrt(h.GetBinError(nb)**2 + of_err**2))

    # --- zero bins outside [-0.4, 0.4] ---
    for i in range(1, nb+1):
        x_low = h.GetBinLowEdge(i)
        x_high = h.GetBinLowEdge(i+1)

        if x_high <= xmin or x_low >= xmax:
            h.SetBinContent(i, 0)
            h.SetBinError(i, 0)

    return h

for era in eras:

    f_bkg = ROOT.TFile.Open(f"{base_dir}/{era}/HNL_SignalRegion_Plotter_Bkg.root")
    f_sig = ROOT.TFile.Open(f"{base_dir}/{era}/HNL_SignalRegion_Plotter_HNL_{mass}.root")
    f_fake = ROOT.TFile.Open(f"{base_dir}/{era}/HNL_SignalRegion_Plotter_Fake.root")

    path = f"LimitExtraction/HNL_ULIDv2/{flavour}/SR3BDTBDT"

    d_sig = f_sig.Get(path)

    for key in d_sig.GetListOfKeys():
        name = key.GetName()
        if f"M{mass}" in name:
            break
        
    h_bkg = prepare_histogram(f_bkg.Get(path).Get(name))

    h_sig = prepare_histogram(f_sig.Get(path).Get(name))
    h_fake = prepare_histogram(f_fake.Get(path).Get(name))
    
    histograms[era] = (h_bkg, h_sig, h_fake)
# ---------------------------------------
# Run scan
# ---------------------------------------

edges_run2, fom_scan_run2 = scan_optimal_variable_binning(

    "Run2",

    histograms,

    nbins=args.n_bins,

    max_trials=args.n_trials,

    flav=flavour

)

if edges_run2 is None:
    print("[ERROR] No valid binning found!")
else:
    print(f"Run2 : {format_edges_str(edges_run2)}")
print(f"fom_scan_run2={fom_scan_run2}")

    
# ---------------------------------------
# Existing comparison
# ---------------------------------------

fom_existing = compute_existing_limitbins_fom(
    histograms,
    mass,
    flavour,
    base_dir
)

# ---------------------------------------
# Output
# ---------------------------------------

print("\n==============================")
print(" FINAL RESULT")
print("==============================")
print("Best FOM Run2(scan):      ", round(fom_scan_run2,5))
if edges_run2 is None:

    print("Best edges:           NONE")

else:

    print("Best edges:           ", format_edges_str(edges_run2))
print("")
print("Existing FOM:         ", round(fom_existing,5))
print("==============================")
