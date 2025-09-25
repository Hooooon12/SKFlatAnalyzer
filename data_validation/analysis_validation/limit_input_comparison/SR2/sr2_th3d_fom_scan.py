#!/usr/bin/env python3

import ROOT
import os
import math
import itertools
import argparse
from tqdm import tqdm
from array import array

def calculate_fom(s, b, mass):
    if mass in ["500", "600", "700"]:
        s /= 10

    return math.sqrt(2 * ((s + b) * math.log(1 + s / b) - s)) if b > 0 and s > 0 else (math.sqrt(2 * s) if s > 0 else 0)

def get_yield_and_error(hist, xcut_range, ycut_range, zcut_range):
    xbin_low = hist.GetXaxis().FindBin(xcut_range[0])
    xbin_high = hist.GetXaxis().FindBin(xcut_range[1]) - 1
    ybin_low = hist.GetYaxis().FindBin(ycut_range[0])
    ybin_high = hist.GetYaxis().FindBin(ycut_range[1]) - 1
    zbin_low = hist.GetZaxis().FindBin(zcut_range[0])
    zbin_high = hist.GetZaxis().FindBin(zcut_range[1]) - 1
    err = array('d', [0.0])
    val = hist.IntegralAndError(xbin_low, xbin_high, ybin_low, ybin_high, zbin_low, zbin_high, err)
    return val, float(err[0])


def evaluate_regions(sig_hist, bkg_hist, mass, regions,loose_critera):
    summary = []
    total_fom = 0.0

    for ht_range, dphi_range, pt_range in regions:
        s, _ = get_yield_and_error(sig_hist, ht_range, dphi_range, pt_range)
        b, b_err = get_yield_and_error(bkg_hist, ht_range, dphi_range, pt_range)
        rel_err = b_err / b if b > 0 else float('inf')

        # Veto this configuration if any bin fails the criteria
        if loose_critera:
            if b < 1. or (b < 2. and rel_err > 0.3):
                return -1.0, []  # Immediately discard this configuration
        else:
            if b < 0.5 or (b < 1. and rel_err > 0.3):
                return -1.0, []  # Immediately discard this configuration                                                                                          
   
        fom = calculate_fom(s, b, mass)
        total_fom += fom
        summary.append((ht_range, dphi_range, pt_range, s, b, fom, rel_err))

    return total_fom, summary


def try_pt_splits(sig_hist, bkg_hist, mass, base_regions,loose_criteria_scan):
    pt_split_values = [60.0,70, 80.0,90.0, 100.0,110, 120.0]
    best_fom, best_summary = evaluate_regions(sig_hist, bkg_hist, mass, base_regions,loose_criteria_scan)

    for i, (ht_range, dphi_range, pt_range) in enumerate(base_regions):
        if pt_range != (0.0, 200.0):
            continue

        for pt_split in pt_split_values:
            new_regions = (
                base_regions[:i] +
                [
                    (ht_range, dphi_range, (0.0, pt_split)),
                    (ht_range, dphi_range, (pt_split, 200.0))
                ] +
                base_regions[i+1:]
            )

            if len(new_regions) > len(base_regions) + 1:
                continue

            new_fom, new_summary = evaluate_regions(sig_hist, bkg_hist, mass, new_regions,loose_criteria_scan)
            if new_fom > best_fom:
                best_fom, best_summary = new_fom, new_summary

    return best_fom, best_summary

def sr2_th3_scan(sig_hist, bkg_hist, mass, nbins, dphi_scan_vals, log_print,loose_criteria_scan):
    from tqdm import tqdm

    htpt_bins_all = [0.0, 0.5, 1.0,1.25, 1.5,1.75, 2.0,2.25, 2.5] + list(range(3, 11))
    best_fom = -1
    best_config = None

    # Estimate total number of scan combinations
    total_combos = 0
    for n_low in range(1, nbins):
        n_high = nbins - n_low
        total_combos += (
            len(list(itertools.combinations(htpt_bins_all[1:-1], n_low - 1))) *
            len(list(itertools.combinations(htpt_bins_all[1:-1], n_high - 1)))
        )
    total_combos *= len(dphi_scan_vals)

    with tqdm(total=total_combos, desc="Scanning bin configurations") as pbar:
        for dphi_cut in dphi_scan_vals:
            dphi_low = (0.0, dphi_cut)
            dphi_high = (dphi_cut, 5.0)

            for n_low in range(1, nbins):
                n_high = nbins - n_low
                for low_edges in itertools.combinations(htpt_bins_all[1:-1], n_low - 1):
                    htpt_bins_low = [0.0] + list(low_edges) + [10.0]

                    for high_edges in itertools.combinations(htpt_bins_all[1:-1], n_high - 1):
                        htpt_bins_high = [0.0] + list(high_edges) + [10.0]

                        regions = []
                        for i in range(len(htpt_bins_low) - 1):
                            ht_range = (htpt_bins_low[i], htpt_bins_low[i + 1])
                            regions.append((ht_range, dphi_low, (0.0, 200.0)))

                        for i in range(len(htpt_bins_high) - 1):
                            ht_range = (htpt_bins_high[i], htpt_bins_high[i + 1])
                            regions.append((ht_range, dphi_high, (0.0, 200.0)))

                        if len(regions) != nbins:
                            pbar.update(1)
                            continue

                        total_fom, region_summary = try_pt_splits(sig_hist, bkg_hist, mass, regions,loose_criteria_scan)

                        if total_fom > best_fom:
                            best_fom = total_fom
                            best_config = (dphi_cut, htpt_bins_low, htpt_bins_high, region_summary)

                        pbar.update(1)

    if best_config:
        dphi_cut, bins_low, bins_high, summary = best_config
        log_print(f"[RESULT] Best configuration (nbins={nbins}):")
        log_print(f"  dphi_cut = {dphi_cut:.2f}")
        log_print(f"  HT/Pt bins for dphi < {dphi_cut:.2f} = {bins_low}")
        log_print(f"  HT/Pt bins for dphi > {dphi_cut:.2f} = {bins_high}")
        log_print(f"  Total FOM = {best_fom:.2f}")
        log_print("  Bin-by-bin summary:")
        for idx, (ht, dphi, pt, s, b, fom, rel_err) in enumerate(summary, 1):
            log_print(f"    Bin {idx}: HT/Pt=[{ht[0]:.2f},{ht[1]:.2f}) dphi=[{dphi[0]:.2f},{dphi[1]:.2f}) Pt=[{pt[0]:.1f},{pt[1]:.1f}) | S={s:.2f} B={b:.2f} FOM={fom:.2f} RelErr={rel_err:.2f}")
    else:
        log_print("[WARN] No valid configuration found.")



def get_existing_fom(sig_file, bkg_file, flavour, mass, log_print):
    path_map = {
        "MuMu": "LimitExtraction/HNL_ULIDv2/MuMu/LimitBins/MuonSR2",
        "EE": "LimitExtraction/HNL_ULIDv2/EE/LimitBins/ElectronSR2",
        "EMu": "LimitExtraction/HNL_ULIDv2/EMu/LimitBins/ElectronMuonSR2",
    }
    path = path_map.get(flavour)
    sig_hist = sig_file.Get(path)
    bkg_hist = bkg_file.Get(path)
    if not sig_hist or not isinstance(sig_hist, ROOT.TH1): return
    if not bkg_hist or not isinstance(bkg_hist, ROOT.TH1): return

    if mass ==	"0":
        sig_hist.Scale(10000.)

    
    total_fom = 0.0
    log_print(f"[INFO] Bin-by-bin FOMs from {path}:")
    for i in range(1, bkg_hist.GetNbinsX() + 1):
        b = bkg_hist.GetBinContent(i)
        s = sig_hist.GetBinContent(i)
        fom = calculate_fom(s, b, mass)
        total_fom += fom
        log_print(f"  Bin {i:2d}: Bkg = {b:.2f}, Sig = {s:.2f}, FOM = {fom:.2f}")
    log_print(f"[INFO] Total FOM over all bins: {total_fom:.2f}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--flavour', default='MuMu')
    parser.add_argument('--mass', default='500')
    parser.add_argument('--era', default='2016preVFP')
    parser.add_argument('--nbins', type=int, default=5)
    parser.add_argument('--sr2scan', action='store_true')
    parser.add_argument('--loose_criteria_scan', action='store_true')
    parser.add_argument('--dphi_cuts', nargs='+', type=float, default=[ 1.8,1.9,2.0,2.1,2.2])
    args = parser.parse_args()

    base_dir = "/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter"
    selected_dir = max([
        os.path.join(base_dir, d) for d in os.listdir(base_dir)
        if os.path.isdir(os.path.join(base_dir, d))
    ], key=os.path.getmtime)

    sig_sample = "Weinberg" if args.mass == "0" else f"HNL_"+args.mass
    sig_file_path = os.path.join(selected_dir, args.era, f"HNL_SignalRegion_Plotter_{sig_sample}.root")
    bkg_file_path = os.path.join(selected_dir, args.era, "HNL_SignalRegion_Plotter_Bkg.root")

    f_sig = ROOT.TFile(sig_file_path)
    f_bkg = ROOT.TFile(bkg_file_path)

    if args.sr2scan:
        path = f"PassSR2/HNL_ULIDv2/{args.flavour}/MainPlots/SR2_Scan"
        sig_hist = f_sig.Get(path)
        bkg_hist = f_bkg.Get(path)
        if not sig_hist or not isinstance(sig_hist, ROOT.TH3): return
        if not bkg_hist or not isinstance(bkg_hist, ROOT.TH3): return

        if args.mass == "0":
            sig_hist.Scale(10000.)
        
        os.makedirs("log_scan", exist_ok=True)

        log_file = (
            f"log_scan/sr2scan_{args.era}_{sig_sample}_nbins{args.nbins}_{args.flavour}.log"
            if not args.loose_criteria_scan
            else f"log_scan/sr2scan_loose_{args.era}_{sig_sample}_nbins{args.nbins}_{args.flavour}.log"
        )
        
        with open(log_file, 'w') as log:
            def log_print(msg):
                print(msg, flush=True)
                log.write(msg + "\n")

            get_existing_fom(f_sig, f_bkg, args.flavour, args.mass, log_print)
            log_print(f"[INFO] Starting SR2 scan for {args.era} mass={sig_sample} nbins={args.nbins}")
            sr2_th3_scan(sig_hist, bkg_hist, sig_sample, args.nbins, args.dphi_cuts, log_print,args.loose_criteria_scan)

    f_sig.Close()
    f_bkg.Close()

if __name__ == "__main__":
    main()
