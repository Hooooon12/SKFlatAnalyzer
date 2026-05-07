#!/usr/bin/env python3

import ROOT, os, argparse, sys, datetime, time, importlib
from contextlib import contextmanager

ROOT.gROOT.SetBatch(True)

@contextmanager
def redirect_stdout(target):
    old_stdout = sys.stdout
    sys.stdout = target
    try:
        yield
    finally:
        sys.stdout = old_stdout


# ---- Imports ----
from python.utils.data_format import build_data
from python.utils.helper import (
    get_latest_dir, ReadConfig, ConvertConfPath,
    list_available_configs, print_sr2_bin_table,
    build_mass_weights_from_ref,
    print_bkg_per_bin_sr2,recompute_per_mass_with_fixed_binning_sr2
)

from python.plotter.plotter import (
    make_mass_plot_multi, convert_results_for_plot,
    build_sr2_plot_results
)

from python.config.default_config import BASE_DIR, FLAVOURS, NCORE, ERAS
import python.config.default_config

from python.config.config_utils import GetScanName

from python.utils.logger import (
    print_config_file, print_sr2_scan_table_from_results,
    debug_print_yields_integral_sr2
)

from python.scan.ref_fom_utils import *
from python.scan.fom_utils import *
from python.scan.evaluator import (
    evaluate_scan_results_sr2
)


# =========================================================
# TIMER
# =========================================================
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
# LOGGING
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



        

# =========================================================
# MAIN
# =========================================================
def main():

    timer = Timer()
    timer.start("TOTAL")

    # ----------------------------------
    # Input directory
    # ----------------------------------
    if BASE_DIR is not None:
        if not os.path.isdir(BASE_DIR):
            raise ValueError(f"[ERROR] BASE_DIR does not exist: {BASE_DIR}")
        base = BASE_DIR
    else:
        base = get_latest_dir("/data6/Users/jalmond/HNL/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter")


    

    # ----------------------------------
    # Args
    # ----------------------------------
    parser = argparse.ArgumentParser()
    parser.add_argument("--debug", action="store_true")
    parser.add_argument("--config", default=None)
    parser.add_argument("--tag", default=None)

    args = parser.parse_args()

    
    if args.config is None:
        print("[ERROR] No config provided")
        list_available_configs("python/config")
        sys.exit(1)
    
    conf_path = ConvertConfPath(args.config)
    config_module = importlib.import_module(conf_path)

    MASSES, RunZ_With_Err, LOG_TAG, TAG = ReadConfig(config_module)

    import python.utils.helper
    python.utils.helper.set_stat_config(config_module)

    tag = args.tag if args.tag else TAG

    ts = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    LOG_TAG = LOG_TAG + "_globalscan"

    os.makedirs(f"output/logs/{tag}/{LOG_TAG}", exist_ok=True)
    outtag = f"{tag}/{LOG_TAG}"

    log_file = f"output/logs/{outtag}/build_data_{ts}.txt"
    results_file = f"output/logs/{outtag}/results_{ts}.txt"
    
    print_config_file(python.config.default_config, title="DEFAULT CONFIG")
    print_config_file(config_module, title=f"USER CONFIG ({conf_path})")

    print("[INFO] Using input directory:", base)
    print("[INFO] Masses:", MASSES)
    print("[INFO] Flavours:", FLAVOURS)

    
    sys.stdout = TeeLogger(log_file)
    
    results_logger = SimpleLogger(results_file)

    print("[INFO] Input:", base)

    # ----------------------------------
    # Build data
    # ----------------------------------
    timer.start("Build data")
    data = build_data(base, MASSES, sig_name="HNL")
    
    timer.stop("Build data")

    # ----------------------------------
    # Reference
    # ----------------------------------
    sr2_cuts = {
        "2016preVFP": {
            "MuMu": (2.0, 2.5, 1.5, 2.5),
            "EE":   (1.6, 2.4, 1.0, 2.0),
            "EMu":  (1.5, 2.5, 1.2, 1.7),
        },
        "2016postVFP": {
            "MuMu": (2.0, 3.0, 1.7, 2.4),
            "EE":   (1.5, 2.0, 1.5, 2.5),
            "EMu":  (1.5, 2.5, 1.1, 1.6),
        },
        "2017": {
            "MuMu": (1.5, 2.5, 1.5, 2.5),
            "EE":   (1.5, 2.5, 1.0, 2.0),
            "EMu":  (1.5, 2.5, 1.0, 2.0),
        },
        "2018": {
            "MuMu": (1.5, 2.5, 1.0, 2.0),
            "EE":   (1.5, 2.5, 1.0, 2.0),
            "EMu":  (1.5, 2.5, 1.0, 2.0),
        }
    }

    ref_results = compute_sr2_z_from_cuts(

        data,
        
        sr2_cuts,
        
        MASSES,

	FLAVOURS

    )
    print_sr2_ref_bin_details(data, ref_results, "MuMu", "0")

    # ----------------------------------
    # SCANS STEP 1
    # ----------------------------------
    standard_config = {
        "scan_type": "PerMassPerFlav",
        "nbin_mode": 4,
        "opt_mode": "Run2",
        "min_bin_width": 0.5
    }
    global_mass_config = {
        "scan_type": "GlobalMassPerFlav",
	"nbin_mode": 4,
        "opt_mode": "Run2",
        "min_bin_width": 0.5
    }
    global_flav_config = {
	"scan_type": "GlobalMassGlobalFlav",
        "nbin_mode": 4,
        "opt_mode": "Run2",
        "min_bin_width": 0.5
    }
    
    standard_config["scan_name"] = GetScanName(standard_config)
    standard_config["mass_weights"] = build_mass_weights_from_ref(ref_results)
    
    global_mass_config["scan_name"] = GetScanName(global_mass_config)
    global_mass_config["mass_weights"] = build_mass_weights_from_ref(ref_results)

    #global_flav_config["scan_name"] = GetScanName(global_flav_config)
    #global_flav_config["mass_weights"] = build_mass_weights_from_ref(ref_results)

    timer.start("Standard scan")

    scan_outputs = run_parallel_global_scans(
        data, FLAVOURS, MASSES, standard_config, n_workers=NCORE
    )
    global_mass_scan_outputs = run_parallel_global_scans(
        data, FLAVOURS, MASSES, global_mass_config, n_workers=NCORE
    )
    #global_flav_scan_outputs = run_parallel_global_scans(
    #data, FLAVOURS, MASSES, global_flav_config, n_workers=NCORE
    #)

    final_scan = evaluate_scan_results_sr2(data, scan_outputs, standard_config)
    final_global_mass_scan = evaluate_scan_results_sr2(data, global_mass_scan_outputs, global_mass_config)
    #final_global_flav_scan = evaluate_scan_results_sr2(data, global_flav_scan_outputs, global_flav_config)

    with redirect_stdout(results_logger):
        print_sr2_scan_table_from_results(final_scan, data)
        print_bkg_per_bin_sr2(data, scan_outputs, standard_config)

        print_sr2_scan_table_from_results(final_global_mass_scan, data)
        print_bkg_per_bin_sr2(data, global_mass_scan_outputs, global_mass_config)

        #print_sr2_scan_table_from_results(final_global_flav_scan, data)
        #print_bkg_per_bin_sr2(data, global_flav_scan_outputs, global_flav_config)

    print_sr2_scan_table_from_results(final_scan, data)
    print_bkg_per_bin_sr2(data, scan_outputs, standard_config)

    print_sr2_scan_table_from_results(final_global_mass_scan, data)
    print_bkg_per_bin_sr2(data, global_mass_scan_outputs, global_mass_config)

    #print_sr2_scan_table_from_results(final_global_flav_scan, data)
    #print_bkg_per_bin_sr2(data, global_flav_scan_outputs, global_flav_config)
    

    global_mass_final_scan_recomputed = recompute_per_mass_with_fixed_binning_sr2(data,global_mass_scan_outputs)
    global_flav_final_scan_recomputed = recompute_per_mass_with_fixed_binning_sr2(data,global_mass_scan_outputs)

    #recompute_per_mass_with_fixed_binning  --> for global 
    timer.stop("Standard scan")


    sr2_cuts = {
        "2016preVFP": {
            "MuMu": (2.0, 2.5, 1.5, 2.5),
            "EE":   (1.6, 2.4, 1.0, 2.0),
            "EMu":  (1.5, 2.5, 1.2, 1.7),
        },
        "2016postVFP": {
            "MuMu": (2.0, 3.0, 1.7, 2.4),
            "EE":   (1.5, 2.0, 1.5, 2.5),
            "EMu":  (1.5, 2.5, 1.1, 1.6),
        },
        "2017": {
            "MuMu": (1.5, 2.5, 1.5, 2.5),
            "EE":   (1.5, 2.5, 1.0, 2.0),
            "EMu":  (1.5, 2.5, 1.0, 2.0),
        },
        "2018": {
            "MuMu": (1.5, 2.5, 1.0, 2.0),
            "EE":   (1.5, 2.5, 1.0, 2.0),
            "EMu":  (1.5, 2.5, 1.0, 2.0),
        }
    }
    

    with redirect_stdout(results_logger):

        for r in ref_results:
            print(f"{r['flav']} | mass={r['mass']} | Z={r['run2']:.4f}")
    for r in ref_results:
        print(f"{r['flav']} | mass={r['mass']} | Z={r['run2']:.4f}")

    comparison_plot_results = [
        
        {            "results": convert_results_for_plot(final_scan, "run2"),
                     "label": "Per-Mass/Flavour optimised" 
         },
        {            "results": convert_results_for_plot(global_mass_final_scan_recomputed, "run2"),
                    "label": "Global-Mass"
         },
        {            "results": convert_results_for_plot(global_flav_final_scan_recomputed, "run2"),
                     "label": "Global-flav"
         },
        {
            "results": convert_results_for_plot(ref_results,"run2"),
            "label": "Reference Binning"
        }
    ]
    for flav in FLAVOURS:
        print("\n==============================")
        print(f"Plotting flavour: {flav} (STANDARD vs REF)")
        print("==============================")

        make_mass_plot_multi(

            results_list=comparison_plot_results,

            flav=flav,

            LOG_TAG=outtag,

            out_tag="sr2_compare_ref_vs_standard" 
        )

        
    # ----------------------------------
    # DONE
    # ----------------------------------
    timer.summary()

    print("[DONE]")
    print("\n==============================")

    print(" OUTPUT PATHS")
    
    print("==============================")
    
    print(f"[INFO] Log (stdout)   : {log_file}")
    
    print(f"[INFO] Results log   : {results_file}")

if __name__ == "__main__":
    main()
