#!/usr/bin/env python3

import ROOT, os, math, itertools, argparse, sys, datetime
from tqdm import tqdm
import ctypes
import time
import importlib
ROOT.gROOT.SetBatch(True)



#### Build data
from data_format import hist_to_array, bins_to_array_with_err, build_data

from helper import debug_data_summary, get_latest_dir,ReadConfig,ConvertConfPath,list_available_configs
from plotter import make_mass_plot_multi, convert_results_for_plot
from default_config import RUN_REF,RUN_SCANS,BASE_DIR
import default_config
from ref_fom_utils import evaluate_ref_bins_no_fakecorr, evaluate_ref_bins_fakecorr, compare_fake_impact
from fom_utils import *
from config_utils import validate_config
# =========================================================
# TIMER HELPER
# =========================================================

def check_stat_violations_ref(results, tol=1e-6):
    print("\n==============================")
    print(" REF STAT CHECK")
    print("==============================\n")

    failures = []

    for entry in results:

        flav = entry["flav"]
        mass = entry["mass"]

        print(f"\n[CHECK] Flav={flav} Mass={mass}")

        per_era = entry["bin_stats"]["per_era"]
        run2    = entry["bin_stats"]["Run2"]

        n_bins = len(run2)

        for i in range(n_bins):

            # -------------------------
            # Run2 check
            # -------------------------
            B_run2 = run2[i]["B"]

            if B_run2 < (1.0 - tol):
                print(f"[FAIL][Run2] Bin {i} B={B_run2:.6f} < 1")
                failures.append((flav, mass, i, "Run2_B"))

            # -------------------------
            # Per-era checks
            # -------------------------
            for era in per_era:

                B = per_era[era][i]["B"]
                E = per_era[era][i]["E"]

                rel = math.sqrt(E)/B if B > 0 else float("inf")

                print(f"Bin {i} | {era:12s} | B={B:.6f} | rel={rel:.6f}")

                if B < (0.15 - tol):
                    print(f"  --> FAIL: B < 0.15")
                    failures.append((flav, mass, i, era, "Era_B"))

                if rel > (0.5 + tol):
                    print(f"  --> FAIL: rel > 0.5")
                    failures.append((flav, mass, i, era, "RelUnc"))

                # --- very useful diagnostics
                if abs(B - 0.15) < 0.02:
                    print(f"  --> WARNING: near B threshold")

                if abs(rel - 0.5) < 0.05:
                    print(f"  --> WARNING: near rel threshold")

    print("\n==============================")
    print(f"TOTAL FAILURES: {len(failures)}")
    print("==============================\n")

    return failures

def check_stat_violations(results, data, tol=1e-6, verbose=True):
    """
    Check stat requirements for each (flav, mass, bin, era)

    Requirements:
      - Run2: B > 1
      - Era:  B > 0.15
      - Era:  sqrt(E)/B < 0.5

    tol: small tolerance for floating precision
    """

    print("\n==============================")
    print(" STAT CHECK VALIDATION")
    print("==============================\n")

    failures = []

    for flav in results:

        for entry in results[flav]:

            mass = entry["mass"]
            edges = entry["binning"]["Run2"]
            bin_lo = data["edges"][:-1]

            print(f"\n[CHECK] Flav={flav} Mass={mass}")

            for i in range(len(edges)-1):

                lo, hi = edges[i], edges[i+1]

                if i == len(edges)-2:
                    mask = (bin_lo >= lo) & (bin_lo <= hi)
                else:
                    mask = (bin_lo >= lo) & (bin_lo < hi)

                # -------------------------
                # Run2 totals
                # -------------------------
                B_run2 = 0.0

                for era in ERAS:
                    B = data["background"][flav][era][mask].sum()
                    F = data["fake"][flav][era][mask].sum()

                    if USE_FAKE_CORR:
                        F, B = fix_fake_and_bkg(F, B, FAKE_FLOOR, flavour=flav, era=era)

                    B_run2 += B

                # Run2 check
                if B_run2 < (1.0 - tol):
                    msg = f"[FAIL][Run2] Bin {i} B={B_run2:.6f} < 1"
                    failures.append((flav, mass, i, "Run2_B"))
                    if verbose:
                        print(msg)

                # -------------------------
                # Per-era checks
                # -------------------------
                for era in ERAS:

                    B = data["background"][flav][era][mask].sum()
                    E = data["bkg_err2"][flav][era][mask].sum()
                    F = data["fake"][flav][era][mask].sum()

                    if USE_FAKE_CORR:
                        F, B = fix_fake_and_bkg(F, B, FAKE_FLOOR, flavour=flav, era=era)

                    rel = math.sqrt(E)/B if B > 0 else float("inf")

                    # --- B > 0.15
                    if B < (0.15 - tol):
                        msg = (f"[FAIL][Era_B] Flav={flav} Mass={mass} Bin={i} Era={era} "
                               f"B={B:.6f} < 0.15")
                        failures.append((flav, mass, i, era, "Era_B"))
                        if verbose:
                            print(msg)

                    # --- rel unc < 0.5
                    if rel > (0.5 + tol):
                        msg = (f"[FAIL][RelUnc] Flav={flav} Mass={mass} Bin={i} Era={era} "
                               f"rel={rel:.6f} > 0.5 (B={B:.6f}, E={E:.6f})")
                        failures.append((flav, mass, i, era, "RelUnc"))
                        if verbose:
                            print(msg)

                    # --- Near-fail (very useful!)
                    if abs(B - 0.15) < 0.02:
                        print(f"[WARN][Near B] Flav={flav} Mass={mass} Bin={i} Era={era} "
                              f"B={B:.6f}")

                    if abs(rel - 0.5) < 0.05:
                        print(f"[WARN][Near rel] Flav={flav} Mass={mass} Bin={i} Era={era} "
                              f"rel={rel:.6f}")

    print("\n==============================")
    print(f" TOTAL FAILURES: {len(failures)}")
    print("==============================\n")

    return failures

class Timer:
    def __init__(self):
        self.times = {}

    def start(self, key):
        self.times[key] = {"start": time.time(), "elapsed": None}

    def stop(self, key):
        if key in self.times and self.times[key]["elapsed"] is None:
            self.times[key]["elapsed"] = time.time() - self.times[key]["start"]

    def summary(self):
        print("\n==============================")
        print(" TIMING SUMMARY")
        print("==============================")
        for k, v in self.times.items():
            if v["elapsed"] is not None:
                print(f"{k:45s} : {v['elapsed']:.2f} s")
        print("==============================\n")


# =========================================================
# LOGGER
# =========================================================
class SimpleLogger:
    def __init__(self, filename):
        self.file = open(filename, "w")

    def write(self, message):
        self.file.write(message)

    def flush(self):
        self.file.flush()
        
class TeeLogger:
    def __init__(self, filename):
        self.file = open(filename, "w")
        self.stdout = sys.stdout

    def write(self, message):
        self.stdout.write(message)
        self.file.write(message)

    def flush(self):
        self.stdout.flush()
        self.file.flush()


from logger import print_final_summary, print_scan_summary_table, print_scan_binning_table,print_config_file


# =========================================================
# MAIN
# =========================================================

def main():

    timer = Timer()
    timer.start("TOTAL")

    # ----------------------------------
    # Use latest production directory
    # ----------------------------------

    if BASE_DIR is not None:
        if not os.path.isdir(BASE_DIR):
            raise ValueError(f"[ERROR] BASE_DIR does not exist: {BASE_DIR}")

        base = BASE_DIR
        print(f"[INFO] Using BASE_DIR from config: {base}")
    else:
        base = get_latest_dir("/data6/Users/jalmond/HNL/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter")
        print(f"[INFO] Using latest directory: {base}")

        
    parser = argparse.ArgumentParser(description="FOM scan runner")
    parser.add_argument("--runRef", action="store_true", default=None)
    parser.add_argument("--runDP",  action="store_true", default=None)
    parser.add_argument("--config", default=None)
    parser.add_argument("--tag", default=None, help="Tag string for this run")
    args = parser.parse_args()
    

    # ----------------------------------
    # Config handling
    # ----------------------------------

    if args.config is None:
        print("[ERROR] No config provided\n")
        list_available_configs("config")
        sys.exit(1)
        
    conf_path = ConvertConfPath(args.config)

    import importlib.util
    if importlib.util.find_spec(conf_path) is None:
        print(f"[ERROR] Config module '{conf_path}' not found\n")
        list_available_configs("config")
        sys.exit(1)
        
    config_module = importlib.import_module(conf_path)

    print(f"[INFO] Using config: {conf_path}")
    validate_config(config_module)
    
    MASSES, NBINS_TO_SCAN, USE_FAKE_FIX, RUN_Z_NO_UNC, LOG_TAG, TAG = ReadConfig(config_module)
    
    import fom_utils
    fom_utils.set_stat_config(config_module)
    
    run_ref = args.runRef if args.runRef is not None else RUN_REF
    run_dp  = args.runDP  if args.runDP  is not None else RUN_SCANS
    
    # ----------------------------------
    # Tag logic
    # ----------------------------------
    if args.tag is not None:
        tag = args.tag
    else:
        tag = TAG

    
    # ----------------------------------
    # Setup logging
    # ----------------------------------
    ts = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    os.makedirs("logs", exist_ok=True)
    os.makedirs(f"logs/{tag}", exist_ok=True)
    outtag = f"{tag}/{LOG_TAG}_PerMass"
    os.makedirs(f"logs/{outtag}", exist_ok=True)

    log_file = f"logs/{outtag}/build_data_{ts}.txt"
    sys.stdout = TeeLogger(log_file)
    
    print_config_file(default_config, title="DEFAULT CONFIG")
    print_config_file(config_module, title=f"USER CONFIG ({conf_path})")

    results_log_file = f"logs/{outtag}/results_{ts}.txt"
    results_logger = SimpleLogger(results_log_file)
    
    print("==============================")
    if run_ref:
        print("Plot Reference bins in ref_bins.py")
    if run_dp:
        print("Run Scans")
    print("==============================")
    print("[INFO] Log file:", log_file)
    print("[INFO] Using input directory:", base)

    # ----------------------------------
    # Mass list
    # ----------------------------------
    print("\n[INFO] Masses:", MASSES)

    # ----------------------------------
    # Build data
    # ----------------------------------
    print("\n[STEP] Building data...")
    timer.start("Build data")
    data = build_data(base, MASSES)
    timer.stop("Build data")

    debug_data_summary(data, 5)

    # ----------------------------------
    # Norm check
    # ----------------------------------
    print("\n[NORM VALUES]")
    for m in MASSES:
        print(f"  {m} -> {data['norm'][m]:.6f}")

    print("\n==============================")
    print(" DONE")
    print("==============================")

    # =====================================================
    # START SCANNING
    # =====================================================

    results_for_plots = []
    scan_results_for_plots = []


    # =====================================================
    # REF
    # =====================================================
    if run_ref:

        timer.start("Reference evaluation")

        print("==============================")
        print("Plot Reference bins in ref_bins.py")
        print("==============================")

        print("\n[STEP] Evaluating REF bins...")

        res_no_raw = evaluate_ref_bins_no_fakecorr(data)
        res_fx_raw = evaluate_ref_bins_fakecorr(data)
        
        failures = check_stat_violations_ref(res_fx_raw)        
        compare_fake_impact(res_no_raw, res_fx_raw)

        res_no_run2 = convert_results_for_plot(res_no_raw, mode="run2")

        results_for_plots.append({
            "results": res_no_run2,
            "raw": res_no_raw,
            "label": "No Fake (Run2)",
        })

        results_for_plots.append({
            "results": convert_results_for_plot(res_fx_raw, mode="run2"),
            "raw": res_fx_raw,
            "label": "Fake Corr (Run2)",
        })

        results_for_plots.append({
            "results": convert_results_for_plot(res_no_raw, mode="quad"),
            "raw": res_no_raw,
            "label": "No Fake (Quad)",
        })

        results_for_plots.append({
            "results": convert_results_for_plot(res_fx_raw, mode="quad"),
            "raw": res_fx_raw,
            "label": "Fake Corr (Quad)",
        })

        scan_results_for_plots.append({
            "results": convert_results_for_plot(res_fx_raw, mode="run2"),
            "raw": res_fx_raw,
            "label": "ANv6 Mass Binning",
        })
        timer.stop("Reference evaluation")
        timer.start("Reference plotting")

        for flav in res_no_run2:

            print("\n==============================")
            print(f"Plotting flavour: {flav}")
            print("==============================")

            make_mass_plot_multi(
                results_list=results_for_plots,
                flav=flav,
                LOG_TAG=outtag,
                out_tag="fake_comparison"
            )
        timer.stop("Reference plotting")

    # =====================================================
    # DP SCANS
    # =====================================================

    # ----------------------------------
    # Per flavour per mass
    # ----------------------------------

    for nb in NBINS_TO_SCAN:
        timer.start(f"DP per flavour per mass (nb={nb})")
        print("\n======================================")
        print(f" Running DP scan for nBins = {nb}")

        
        res_dp_raw = evaluate_dp_per_flavour_per_mass_run2(data, nb,use_fake_corr=USE_FAKE_FIX,run_z_no_unc=RUN_Z_NO_UNC)
        
        
        print_final_summary(res_dp_raw,"per_mass")


        if nb == NBINS_TO_SCAN[-1]:
            
            scan_results_for_plots.append({
                "results": convert_results_for_plot(res_dp_raw, mode="run2"),
                "raw": res_dp_raw,
                "label": f"Scan (Run2, N_bins={nb})"
            })

        timer.stop(f"DP per flavour per mass (nb={nb})")
    

    # =====================================================
    # PLOTTING
    # =====================================================
    timer.start("Plotting")

    for flav in FLAVOURS:

        print("\n==============================")
        print(f"Plotting flavour: {flav}")
        print("==============================")
        make_mass_plot_multi(
            results_list=scan_results_for_plots,
            flav=flav,
            LOG_TAG=outtag,
            out_tag="scan_results"
        )
     


    timer.stop("Plotting")

    # =====================================================
    # SUMMARY
    # =====================================================
    
    print("\n==============================")
    print("[INFO] Writing clean results summary...")
    print("==============================")
    
    # Save current stdout
    original_stdout = sys.stdout
    
    # Redirect to results file
    sys.stdout = results_logger
    
    print("====================================")
    print(" FINAL SUMMARY")
    print("====================================\n")
    
    print_scan_summary_table(scan_results_for_plots)
    print("\n------------------------------------\n")
    print_scan_summary_table(scan_results_for_plots, use_quad=True)
    print("\n------------------------------------\n")
    print_scan_binning_table(scan_results_for_plots)
    
    print("\n====================================")
    print(" END OF SUMMARY")
    print("====================================\n")
    
    # Restore stdout
    sys.stdout = original_stdout

    print(f"[INFO] Results summary saved to: {results_log_file}")
    
    print("[INFO] Log file saved to:")
    print(log_file)
    print("==============================")

    # FINAL TIMER OUTPUT
    timer.stop("TOTAL")
    timer.summary()


if __name__ == "__main__":
    main()
