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

def greedy_optimize_edges(initial_edges, bin_cache, nbins, x_min, x_max, tail_cut, scan_era, max_iter=20):

    step = 0.005
    edges = list(initial_edges)

    result = evaluate_combination_fnct(
        (edges[1:-1], bin_cache, nbins, x_min, x_max, tail_cut, scan_era)
    )

    if result is None:
        return initial_edges, -1

    best_fom, _ = result

    for iteration in range(max_iter):

        improved = False

        indices = list(range(1, len(edges)-1))
        random.shuffle(indices)

        for i in indices:
            
            for direction in [-2, -1, +1, +2]:
                new_edges = edges[:]
                new_val = round(round((new_edges[i] + direction*step)/step)*step, 5)

                if abs(new_val - new_edges[i-1]) < 1e-6:
                    
                    continue
                
                if abs(new_val - new_edges[i+1]) < 1e-6:
                    
                    continue
                
                new_edges[i] = new_val

                MIN_WIDTH = 0.005

                if not (new_edges[i-1] + MIN_WIDTH < new_val < new_edges[i+1] - MIN_WIDTH):
                    
                    continue
                

                if any(e > tail_cut for e in new_edges[1:-1]):
                    continue

                result = evaluate_combination_fnct(
                    (new_edges[1:-1], bin_cache, nbins, x_min, x_max, tail_cut, scan_era)
                )

                if result is None:
                    continue

                fom, _ = result

                if fom > best_fom:
                    best_fom = fom
                    edges = new_edges
                    improved = True

        if not improved:
            print(f"[GREEDY] Converged after {iteration} iterations")
            break

    return edges, best_fom



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


def build_cumsum_cache(bin_cache):

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

            for k in range(1, h_bkg.GetNbinsX() + 1):

                x = h_bkg.GetBinCenter(k)

                if not (x_low <= x < x_high):
                    continue

                b = h_bkg.GetBinContent(k)
                f = h_fake.GetBinContent(k)
                s = h_sig.GetBinContent(k)

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

    mids, bin_cache, nbins, x_min, x_max, tail_cut, scan_era = args

    edges = [x_min] + list(mids) + [x_max]

    # tail cut check
    if edges[-2] > tail_cut:
        return None

    total_fom = 0.0

    for j in range(nbins):

        x_low  = edges[j]
        x_high = edges[j+1]

        run2_sig = 0.0
        run2_bkg = 0.0
        scan_sig = 0.0
        scan_bkg = 0.0

        for era, (centers, cumsum_b, cumsum_f, cumsum_s, _) in bin_cache.items():

            # map edges to indices
            i_low  = np.searchsorted(centers, x_low,  side='left')
            i_high = np.searchsorted(centers, x_high, side='left')

            # safety check
            if i_low >= i_high or i_high > len(centers):
                return None

            # cumulative sums
            bkg  = cumsum_b[i_high-1] - (cumsum_b[i_low-1] if i_low > 0 else 0.0)
            fake = cumsum_f[i_high-1] - (cumsum_f[i_low-1] if i_low > 0 else 0.0)
            sig  = cumsum_s[i_high-1] - (cumsum_s[i_low-1] if i_low > 0 else 0.0)

            # fake correction
            fake, bkg = fix_fake_and_bkg(fake, bkg, FAKE_FLOOR)

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
    test_edges = np.round(test_edges, 5)
    pass_count = 0

    for x in test_edges:

        run2_bkg = 0
        valid = True

        for era, (h, hs, hf) in histograms.items():

            bkg = 0
            fake = 0
            for k in range(1, h.GetNbinsX()+1):
                
                xc = h.GetBinCenter(k)

                if not (x <= xc <= 0.4):
                    continue

                b = h.GetBinContent(k)
                f = hf.GetBinContent(k)

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
    
    tmp_edges_med  = np.arange(0.1, 0.11, 0.01)

    tmp_edges_high = np.arange(0.11, 0.4, 0.005)

    tmp_edges_all = np.concatenate((tmp_edges_low, tmp_edges_med, tmp_edges_high))

    tmp_edges_all = np.unique(np.round(tmp_edges_all, 5))

    return tmp_edges_all
    

def build_edges_from_tailcut(Xcut):

    edges_low = [-0.05]

    # --- case 1: very tight tail ---
    if Xcut < 0.25:

        edges_med = np.arange(0.0, 0.06, 0.02)
        edges_high = np.arange(0.06, Xcut -0.1, 0.01)
        edges_vhigh = np.arange(Xcut-0.1, Xcut , 0.005)

        edges_all = np.concatenate((edges_low, edges_med,edges_high, edges_vhigh))


    # --- case 2: medium tail ---
    elif Xcut < 0.30:

        edges_med  = np.arange(0.0, 0.10, 0.02)
        edges_high = np.arange(0.10, Xcut -0.1, 0.01)
        edges_vhigh = np.arange(Xcut-0.1, Xcut , 0.005)
        
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

def scan_optimal_variable_binning(scan_era,histograms, nbins, max_trials, flav=None):

    x_min, x_max = -0.4, 0.4

    tail_cut = find_tail_cut(histograms) 
    tail_cut = round(tail_cut, 4) 
    
    print("[INFO] Tail cutoff at:", tail_cut)

    # --- build bin_cache (same as before) ---
    bin_cache = {}

    for era, (h, hs, hf) in histograms.items():
        
        #if scan_era != "Run2" and era != scan_era:
        #    continue
        
        centers = []
        bvals = []
        fvals = []
        svals = []

        for k in range(1, h.GetNbinsX()+1):

            x = h.GetBinCenter(k)

            if not (-0.4 <= x <= 0.4):
                continue

            b = h.GetBinContent(k)
            s = hs.GetBinContent(k)
            f = hf.GetBinContent(k)

            centers.append(x)
            bvals.append(b)
            fvals.append(f)
            svals.append(s)

        bin_cache[era] = (
            np.array(centers),
            np.array(bvals),
            np.array(fvals),
            np.array(svals),
        )

    
    all_edges = np.round(np.arange(-0.4, 0.405, 0.005), 5)
    bin_cache=build_cumsum_cache(bin_cache)
    
    # ---------------------------------------
    # --- edges ---
    # ---------------------------------------
    from math import comb
    k = nbins - 1
    
    edges_simple = build_edges(tail_cut)
    edges_simple = np.array(edges_simple)
    edges_simple = edges_simple[edges_simple < tail_cut]
   
    print_run2_bkg_per_edge(histograms, edges_simple)
    edges_simple = edges_simple[edges_simple<tail_cut]
    N_simple = len(edges_simple)

    ncomb_simple_after = comb(N_simple, k) if N_simple >= k else 0
    
    edges_all = build_edges_from_tailcut(tail_cut)

    edges_all = np.array(edges_all)
    edges_all = edges_all[edges_all < tail_cut]
    
    print(f"All bins : {format_edges_str(edges_all)}")
    print_run2_bkg_per_edge(histograms, edges_all)

    N_after = len(edges_all)
    
    ncomb_after = comb(N_after, k) if N_after >= k else 0
    print(f"[INFO] edges after tail cut: {N_after}")
    print(f"[INFO] combinations after cut: {ncomb_after:.3e}")
    print(f"Skimmed bins : {format_edges_str(edges_all)}")
    

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

    ncpu = 1
    
    print(f"[INFO] Using {ncpu} cores")
    
    args_case1 = (
    
        (mids, bin_cache, nbins, x_min, x_max, tail_cut,scan_era)
        
        for mids in comb_simple

    )
    args_case2 = (
        
        (mids, bin_cache, nbins, x_min, x_max, tail_cut,scan_era)
        
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


    if best_edges is None:
        
        print("[ERROR] No valid binning found in scan")
        
        return None, -1
    
    for _ in range(5):  # try multiple starting points

        edges_all_list = list(edges_all)
        seed_edges = best_edges if _ == 0 else sorted(random.sample(edges_all_list, nbins-1))
        
        seed_edges = [x_min] + list(seed_edges) + [x_max]
        
        g_edges, g_fom = greedy_optimize_edges(

            seed_edges,
            
            bin_cache,
            
            nbins,
            
            x_min,
            
            x_max,
            
            tail_cut,
            
            scan_era
            
        )
        
        if g_fom > best_fom:
            print(f"[GREEDY] Improved FOM: {best_fom:.4f} -> {g_fom:.4f}")
                    
            best_fom = g_fom
            
            best_edges = g_edges
            
        
            #def snap_to_grid(edges, step=0.005):

            #return [round(round(x/step)*step, 5) for x in edges]

            #best_edges = snap_to_grid(best_edges)
        
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
parser.add_argument('--n_trials', type=int, default=100000)
parser.add_argument('--seed', type=int, default=None)
parser.add_argument('--job_id', type=int, default=0)
parser.add_argument('--n_jobs', type=int, default=1)

args = parser.parse_args()
seed = args.job_id + 12345

random.seed(seed)

np.random.seed(seed)


mass = args.mass
flavour = args.flavour

if args.seed is not None:

    random.seed(args.seed)

    np.random.seed(args.seed)
    

os.makedirs("logs_greedy", exist_ok=True)
log_name = f"logs_greedy/scan_{mass}_{flavour}_job{args.job_id}.txt"

log_file = open(log_name, "w")

sys.stdout = Tee(sys.stdout, log_file)

sys.stderr = Tee(sys.stderr, log_file)

print(f"[LOG] Writing output to {log_name}")




base_dir = "/data9/Users/HNL_public/SUS-24-014/SKFlatOutput/Scan/BDTFine/"
eras = ["2016preVFP","2016postVFP","2017","2018"]

histograms = {}

# Load histograms

def prepare_histogram(h, xmin=-0.39, xmax=0.39):

    h = h.Clone()
    h.SetDirectory(0)

    nb = h.GetNbinsX()

    # ---------------------------------------
    # Find boundary bins
    # ---------------------------------------
    bin_low_edge  = h.FindBin(xmin)
    bin_high_edge = h.FindBin(xmax)

    # ---------------------------------------
    # Merge EVERYTHING below xmin into xmin bin
    # ---------------------------------------
    sum_low = 0.0
    err2_low = 0.0

    for i in range(0, bin_low_edge):  # includes underflow (0)
        sum_low += h.GetBinContent(i)
        err2_low += h.GetBinError(i)**2

        h.SetBinContent(i, 0)
        h.SetBinError(i, 0)

    h.SetBinContent(bin_low_edge,
        h.GetBinContent(bin_low_edge) + sum_low)

    h.SetBinError(bin_low_edge,
        math.sqrt(h.GetBinError(bin_low_edge)**2 + err2_low))


    # ---------------------------------------
    # Merge EVERYTHING above xmax into xmax bin
    # ---------------------------------------
    sum_high = 0.0
    err2_high = 0.0

    for i in range(bin_high_edge+1, nb+2):  # includes overflow (nb+1)
        sum_high += h.GetBinContent(i)
        err2_high += h.GetBinError(i)**2

        h.SetBinContent(i, 0)
        h.SetBinError(i, 0)

    h.SetBinContent(bin_high_edge,
        h.GetBinContent(bin_high_edge) + sum_high)

    h.SetBinError(bin_high_edge,
        math.sqrt(h.GetBinError(bin_high_edge)**2 + err2_high))


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

edges_run2 = [round(round(x/0.005)*0.005, 5) for x in edges_run2]

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


os.makedirs("results_greedy", exist_ok=True)

np.save(
    f"results_greedy/results_{mass}_{flavour}_job{args.job_id}.npy",
    {
        "mass": mass,
        "flavour": flavour,
        "job_id": args.job_id,

        "fom_best": float(fom_scan_run2),
        "fom_existing": float(fom_existing),

        "edges_best": [round(x, 5) for x in edges_run2],

        "fom_greedy": float(fom_scan_run2),
        "edges_greedy": [round(x, 5) for x in edges_run2],

        # --- useful metadata ---
        "n_bins": args.n_bins,
        "n_trials": args.n_trials,
        "scan_type": "Greedy",
        "seed": seed,
    }
)    

# ---------------------------------------
# Output
# ---------------------------------------


print("\n==============================")
print(" FINAL RESULT")
print("==============================")
print("Best FOM Run2(scan):      ", round(fom_scan_run2,5))
print("Best edges:           ", format_edges_str(edges_run2))
print("")
print("Existing FOM:         ", round(fom_existing,5))
print("==============================")
