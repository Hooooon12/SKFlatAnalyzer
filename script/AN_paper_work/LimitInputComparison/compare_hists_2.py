#!/usr/bin/env python3
 
import ROOT
import os
import math
 
# ==========================
# Configuration
# ==========================
 
eras = ["2016preVFP", "2016postVFP", "2017", "2018"]
flavs = ["MuMu", "EE"]
 
base_dir_1 = "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"
base_dir_2 = "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets_Decorr_JetDecorr"
 
signal_hist = "signalSSWW"
data_hist   = "data_obs"
 
# ==========================
# Header
# ==========================
 
print("==================================================")
print("Directory mapping:")
print(f"  Old = {base_dir_1}")
print(f"  New = {base_dir_2}")
print("==================================================\n")
 
# ==========================
# Helpers
# ==========================
 
def get_hist(file_path, hist_name):
    if not os.path.exists(file_path):
        print("  [WARNING] File not found:", file_path)
        return None
 
    f = ROOT.TFile.Open(file_path)
    if not f or f.IsZombie():
        print("  [WARNING] Cannot open:", file_path)
        return None
 
    h = f.Get(hist_name)
    if not h:
        print("  [WARNING] Histogram not found:", hist_name, "in", file_path)
        f.Close()
        return None
 
    h_clone = h.Clone()
    h_clone.SetDirectory(0)
    f.Close()
    return h_clone
 
 
def asimov_significance(S, B):
    S = max(S, 0.0)
    B = max(B, 1e-9)
 
    try:
        val = 2.0 * ((S + B) * math.log(1.0 + S / B) - S)
        if val <= 0:
            return 0.0
        return math.sqrt(val)
    except:
        return 0.0
 
 
def compute_total_fom(sig_hist, bkg_hist):
    nbins = sig_hist.GetNbinsX()
    sum_z2 = 0.0
 
    for i in range(1, nbins + 1):
    #for i in range(1, 2):
        S = sig_hist.GetBinContent(i)
        B = bkg_hist.GetBinContent(i)
 
        if B < 1e-9:
            continue
 
        Z = asimov_significance(S, B)
        sum_z2 += Z * Z
 
    return math.sqrt(sum_z2)
 
 
def print_yields(label, sig, data):
    #y_sig  = sig.Integral()
    #y_data = data.Integral()
    y_sig  = sig.GetBinContent(1)
    y_data = data.GetBinContent(1)
 
    print(f"{label}:")
    print(f"  Signal     = {y_sig:.6f}")
    print(f"  Total Bkg  = {y_data:.6f} (from data_obs)")
 
 
# ==========================
# Storage for summary
# ==========================
 
results = []
 
# ==========================
# Main loop
# ==========================
 
for era in eras:
    for flav in flavs:
 
        file1 = f"{base_dir_1}/{era}/sr2/M1000_{flav}_card_input.root"
        file2 = f"{base_dir_2}/{era}/sr2/M1000_{flav}_card_input.root"
 
        print("==================================================")
        print(f"[Old vs New] Era: {era}, Flavour: {flav}")
        print("--------------------------------------------------")
 
        sig1  = get_hist(file1, signal_hist)
        data1 = get_hist(file1, data_hist)
 
        sig2  = get_hist(file2, signal_hist)
        data2 = get_hist(file2, data_hist)
 
        if not sig1 or not data1 or not sig2 or not data2:
            print("  [ERROR] Missing inputs")
            continue
 
        # --- Yields ---
        print_yields("Old", sig1, data1)
        print_yields("New", sig2, data2)
        print("")
 
        # --- Differences ---
        #sig_old = sig1.Integral()
        #sig_new = sig2.Integral()
        #bkg_old = data1.Integral()
        #bkg_new = data2.Integral()
        sig_old = sig1.GetBinContent(1)
        sig_new = sig2.GetBinContent(1)
        bkg_old = data1.GetBinContent(1)
        bkg_new = data2.GetBinContent(1)
 
        sig_diff = sig_new - sig_old
        bkg_diff = bkg_new - bkg_old
 
        sig_pct = (sig_diff / sig_old * 100.0) if sig_old != 0 else 0.0
        bkg_pct = (bkg_diff / bkg_old * 100.0) if bkg_old != 0 else 0.0
 
        print("Differences (New - Old) [%]:")
        print(f"  Signal     = {sig_diff:+.6f} ({sig_pct:+.2f}%)")
        print(f"  Total Bkg  = {bkg_diff:+.6f} ({bkg_pct:+.2f}%)")
        print("")
 
        # --- FOM ---
        fom1 = compute_total_fom(sig1, data1)
        fom2 = compute_total_fom(sig2, data2)
 
        fom_diff = fom2 - fom1
        fom_pct = (fom_diff / fom1 * 100.0) if fom1 != 0 else 0.0
 
        print("Asimov FOM:")
        print(f"  Old = {fom1:.6f}")
        print(f"  New = {fom2:.6f}")
        print(f"  Diff = {fom_diff:+.6f} ({fom_pct:+.2f}%)")
        print("")
 
        # Store for table
        results.append({
            "era": era,
            "flav": flav,
            "sig_pct": sig_pct,
            "bkg_pct": bkg_pct,
            "fom_pct": fom_pct
        })
 
 
# ==========================
# Summary table
# ==========================
 
print("\n================ SUMMARY TABLE ================\n")
 
header = f"{'Era':<12} {'Flav':<6} {'dSig (%)':>10} {'dBkg (%)':>10} {'dFOM (%)':>10}"
print(header)
print("-" * len(header))
 
for r in results:
    print(f"{r['era']:<12} {r['flav']:<6} {r['sig_pct']:>10.2f} {r['bkg_pct']:>10.2f} {r['fom_pct']:>10.2f}")
 
print("")
