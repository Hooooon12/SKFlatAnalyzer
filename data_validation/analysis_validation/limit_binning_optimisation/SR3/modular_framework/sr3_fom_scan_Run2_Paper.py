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
from python.utils.data_format import build_data_sr3

from python.utils.helper import (
    debug_data_summary, get_latest_dir, ReadConfig, ConvertConfPath,
    list_available_configs, print_sr3_bin_table,debug_fixed_binning_stat_failures,print_sr3_z_ref_summary,debug_compare_binnings_per_boundary,build_mass_weights_from_ref,build_fixed_met_results,recompute_per_met_results,print_bkg_per_bin,recompute_per_mass_with_fixed_binning
)
from python.plotter.plotter import make_mass_plot_multi, convert_results_for_plot,build_sr3_plot_results,convert_scan_results_for_plot

from python.config.default_config import BASE_DIR, FLAVOURS, NCORE, ERAS
import python.config.default_config

from python.config.config_utils import GetScanName

from python.utils.logger import (
    print_config_file, print_scan_summary, print_scan_details,print_sr3_scan_table_from_results,debug_print_yields_integral,print_sr3_z_summary_per_metcat_flat,print_sr3_z_per_boundary
)

from python.scan.ref_fom_utils import *
from python.scan.fom_utils import *
from python.scan.evaluator import evaluate_scan_results,find_best_met_per_flavour

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
    parser = argparse.ArgumentParser(description="FOM scan runner")
    parser.add_argument("--debug", action="store_true", default=None)
    parser.add_argument("--config", default=None)
    parser.add_argument("--tag", default=None)
    parser.add_argument("--test", type=int, default=None,
                    help="Run in test mode with test ID")


    args = parser.parse_args()
    
    if args.test is not None:
        print(f"[INFO] Running in TEST mode (test={args.test})")
        print("[INFO] Overriding config with test_scan for test mode")
        args.config = "python.config.test_scan"
        
    if args.config is None:
        print("[ERROR] No config provided")
        list_available_configs("python/config")
        sys.exit(1)

    conf_path = ConvertConfPath(args.config)
    config_module = importlib.import_module(conf_path)

    MASSES, USE_FAKE_FIX, RUN_Z_NO_UNC, LOG_TAG, TAG, NBinScan, RunGlobalSig = ReadConfig(config_module)

    import python.utils.helper
    python.utils.helper.set_stat_config(config_module)

    tag = args.tag if args.tag else getattr(config_module, "TAG", "Default")
    scan_type = "Nbin" if NBinScan else ""

    # ----------------------------------
    # Logging setup
    # ----------------------------------
    ts = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")

    os.makedirs(f"output/logs/{tag}/{LOG_TAG}", exist_ok=True)
    outtag = f"{tag}/{LOG_TAG}"

    log_file = f"output/logs/{outtag}/build_data_{ts}.txt"
    sys.stdout = TeeLogger(log_file)

    results_log_file = f"output/logs/{outtag}/results_{ts}.txt"
    results_logger = SimpleLogger(results_log_file)

    print_config_file(python.config.default_config, title="DEFAULT CONFIG")
    print_config_file(config_module, title=f"USER CONFIG ({conf_path})")

    print("[INFO] Using input directory:", base)
    print("[INFO] Masses:", MASSES)
    print("[INFO] Flavours:", FLAVOURS)

    # ----------------------------------
    # Build data
    # ----------------------------------
    timer.start("Build data")
    data = build_data_sr3(base, MASSES,sig_name="HNL")
    timer.stop("Build data")

    if args.debug:
        debug_data_summary(data, 5)

        print("\n[NORM VALUES PER REGION]")
        for met in data:
            for cat in data[met]:
                print(f"\n--- MET {met} | {cat} ---")
                for m in MASSES:
                    print(f"  {m} -> {data[met][cat]['norm'][m]:.6f}")
                    
    print_bin_summary()


    if args.debug:
        debug_print_yields_integral(data)

        
        print (100*"#")
        print(f"RUNNING [print_sr3_z_summary] [per MET Cat]")
        print (100*"#")
        print_sr3_z_summary_per_metcat_flat(data, MASSES, FLAVOURS,ERAS)
        print (100*"#")
        print(f"RUNNING [print_sr3_z_per_boundary] [[0,200,400,1200]")
        print (100*"#")
        print_sr3_z_per_boundary(data, MASSES, FLAVOURS,ERAS)
        
        debug_fixed_binning_stat_failures(data, MASSES, FLAVOURS)
        
        
    # ----------------------------------
    # Reference
    # ----------------------------------
    print (100*"#")
    print (100*"#")
    ref_results = evaluate_sr3_run2_with_boundary(data)

    if args.debug:
                        
        print (100*"#")
        print(f"RUNNING [print_sr3_z_ref_summary] Reference binning")
        print (100*"#")
        print_sr3_z_ref_summary(ref_results)
        
        print (100*"#")
        print (100*"#")
        print (100*"#")
        print (100*"#")
    else:
        print_sr3_z_ref_summary(ref_results)

    ref_results_for_plots = build_sr3_plot_results(data)

    for flav in ref_results_for_plots[0]["results"]:
        print_sr3_bin_table(data, flav=flav, mass="2000")

        make_mass_plot_multi(
            results_list=ref_results_for_plots,
            flav=flav,
            LOG_TAG=outtag,
            out_tag="sr3_ref"
        )

        
    standard_config = {
        # -------------------------------
        # Scan strategy
        # -------------------------------
        "scan_type": "PerMassPerFlav",
        # options:
        # "PerMassPerFlav"
        # "GlobalMassPerSig"
        # "PerMassGlobalSig"
        # "GlobalMassGlobalSig"
        
        # -------------------------------
        # Bin strategy
        # -------------------------------
        "nbin_mode": "fixed",
        # options:
        # "fixed"  -> use get_nbins_for_region
        # "scan"   -> try 3-7 bins
        # int      -> force number of bins
        
        # -------------------------------
        # Optimisation level
        # -------------------------------
        "opt_mode": "Run2", 
        # options:
        # "Run2"  -> combine eras before Z
        # "Era"   -> optimise per era separately
        
        # -------------------------------
        # MET selection
        # -------------------------------
        "mets": ["2", "3", "4", "5"],
        
        # -------------------------------                                                                                                                               
        # Scan parameters
        # -------------------------------       
        "min_bin_width": 10.0,
        "min_lt_first_edge": 120.0,
    }
    standard_config["scan_name"] = GetScanName(standard_config)
    standard_config["mass_weights"] = build_mass_weights_from_ref(ref_results)
    
    timer.start("Standard scan")
        
    scan_outputs = run_parallel_scans(data, FLAVOURS, MASSES, standard_config,n_workers=NCORE)
    final_scan = evaluate_scan_results(data, scan_outputs, standard_config)
    print("\n==============================")
    print(" STANDARD SCAN TABLES")
    print("==============================")

    print_sr3_scan_table_from_results(final_scan, data)
    mass_weights = standard_config["mass_weights"]
    best_met_map = find_best_met_per_flavour(scan_outputs, mass_weights)

    fixed_met_results = build_fixed_met_results(
        data,
        scan_outputs,
        best_met_map,
        standard_config
    )
    
    plot_results = [

    {
        "results": convert_results_for_plot(final_scan, "run2"),
        "label": "Per-Mass/Flavour optimised Best MET",
    },

    {
        "results": ref_results_for_plots[0]["results"],
        "label": "Reference"
    },
    {
        "results": convert_results_for_plot(fixed_met_results, "run2"),
        "label": "Fixed MET (per flavour)"
    }
    ]
    
    for flav in FLAVOURS:            
        print("\n==============================")
        print(f"Plotting flavour: {flav} (SCAN)")
        print("==============================")
        
        make_mass_plot_multi(
            
            results_list=plot_results,
            
            flav=flav,
            
            LOG_TAG=outtag,
            
            out_tag="sr3_scan_"+standard_config["scan_name"] 
            
        )


    print("\n==============================")
    print(" BEST MET PER FLAVOUR (WEIGHTED)")
    print("==============================")

    met_plot_results = []
    for met in ["2","3","4","5"]:
        
        met_results = recompute_per_met_results(data, scan_outputs, met)
        
        met_plot_results.append({
            
            "results": convert_results_for_plot(met_results, "run2"),
            
            "label": f"MET {met}"

        })
        
    
    for flav in best_met_map:
        
        info = best_met_map[flav]
        
        print(f"\n{flav}: BEST MET = {info['best_met']}")
        
        for met, val in sorted(info["scores"].items()):
            print(f"  MET {met} -> Z = {val:.4f}")


        make_mass_plot_multi(
            
            results_list=met_plot_results,

            flav=flav,

            LOG_TAG=outtag,
            
            out_tag="sr3_metscan_"+standard_config["scan_name"]
        )


    print_bkg_per_bin(data, scan_outputs, standard_config)
    timer.stop("Standard scan")
    # =========================================================
    # GLOBAL MASS SCAN (ADDITIONAL)
    # =========================================================

    global_mass_config = {
        "scan_type": "GlobalMassPerFlav",
        "nbin_mode": "fixed",
        "opt_mode": "Run2",
        "mets": ["2", "3", "4", "5"],
        "min_bin_width": 10.0,
        "min_lt_first_edge": 120.0,
    }
    global_binscan_mass_config = {
     	"scan_type": "GlobalMassPerFlav",
        "nbin_mode": "scan",
        "opt_mode": "Run2",
        "mets": ["2", "3", "4", "5"],
	"min_bin_width": 10.0,
        "min_lt_first_edge": 120.0,
    }

    
    global_mass_config["scan_name"] = GetScanName(global_mass_config)
    global_mass_config["mass_weights"] = standard_config["mass_weights"]

    global_binscan_mass_config["scan_name"] = GetScanName(global_binscan_mass_config)
    global_binscan_mass_config["mass_weights"] = standard_config["mass_weights"]

    
    print("\n==============================")
    print(" RUNNING GLOBAL MASS SCAN")
    print("==============================")

    timer.start("GlobalMass scan")

    global_scan_outputs = run_parallel_scans( data, FLAVOURS, MASSES, global_mass_config, n_workers=NCORE )
    global_final_scan = evaluate_scan_results(data, global_scan_outputs, global_mass_config )
    
    global_binscan_scan_outputs = run_parallel_scans( data, FLAVOURS, MASSES, global_binscan_mass_config, n_workers=NCORE )
    global_binscan_final_scan = evaluate_scan_results(data, global_binscan_scan_outputs, global_binscan_mass_config )


    print("\n==============================")
    print(" GLOBAL MASS SCAN TABLES")
    print("==============================")

    print_sr3_scan_table_from_results(global_final_scan, data)
    
    print("\n==============================")
    print(" GLOBAL BinScan MASS SCAN TABLES")
    print("==============================")

    print_sr3_scan_table_from_results(global_binscan_final_scan, data)
    
    # ----------------------------------
    # BEST MET (GLOBAL)
    # ----------------------------------
    global_best_met_map = find_best_met_per_flavour(
        global_scan_outputs,
        global_mass_config["mass_weights"]
    )

    global_fixed_met_results = build_fixed_met_results(
        data,
        global_scan_outputs,
        global_best_met_map,
        global_mass_config
    )

    # ----------------------------------                                                                                                                         
    # BEST MET (GLOBAL) BINSCAN                                                                                                                                 
    # ----------------------------------                                                                                                                             
    global_binscan_best_met_map = find_best_met_per_flavour(
        global_binscan_scan_outputs,
        global_binscan_mass_config["mass_weights"]
    )

    global_binscan_fixed_met_results = build_fixed_met_results(
        data,
        global_binscan_scan_outputs,
        global_binscan_best_met_map,
        global_binscan_mass_config
    )

    
    # ----------------------------------
    # COMPARISON PLOT (KEY ADDITION)
    # ----------------------------------
    global_final_scan_recomputed = recompute_per_mass_with_fixed_binning(data, global_scan_outputs)
    global_binscan_final_scan_recomputed = recompute_per_mass_with_fixed_binning(data, global_binscan_scan_outputs)
        
    comparison_plot_results = [

        {
            "results": convert_results_for_plot(final_scan, "run2"),
            "label": "Per-Mass/Flavour optimised" 
        },

        {
            "results": convert_results_for_plot(global_final_scan_recomputed, "run2"),
            "label": "Global-Mass optimised"
        },

        {
            "results": ref_results_for_plots[0]["results"],
            "label": "Reference Binning"
        }
    ]

    comparison_plot_binscan_results = [

        {
            "results": convert_results_for_plot(global_final_scan_recomputed, "run2"),
            "label": "Global-Mass optimised Fixed NBin"
        },
        {
            "results": convert_results_for_plot(global_binscan_final_scan_recomputed, "run2"),
            "label": "Global-Mass optimised Scanned NBin"
        },
    ]


    for flav in FLAVOURS:
        print("\n==============================")
        print(f"Plotting flavour: {flav} (STANDARD vs GLOBAL)")
        print("==============================")

        make_mass_plot_multi(

            results_list=comparison_plot_results,

            flav=flav,

            LOG_TAG=outtag,

            out_tag="sr3_compare_permass_vs_global" 
        )
        make_mass_plot_multi(
            
            results_list=comparison_plot_binscan_results,

            flav=flav,

            LOG_TAG=outtag,

            out_tag="sr3_compare_global_binscan"
        )

    # ----------------------------------
    # GLOBAL MET SCAN (same as standard)
    # ----------------------------------
    print("\n==============================")
    print(" GLOBAL BEST MET PER FLAVOUR (WEIGHTED)")
    print("==============================")

    global_met_plot_results = []

    for met in ["2","3","4","5"]:

        met_results = recompute_per_met_results(
            data, global_scan_outputs, met
        )

        global_met_plot_results.append({

            "results": convert_results_for_plot(met_results, "run2"),
            "label": f"MET {met}"

        })

    global_binscan_met_plot_results = []

    for met in ["2","3","4","5"]:

        met_results_binscan = recompute_per_met_results(
            data, global_binscan_scan_outputs, met
        )

        global_binscan_met_plot_results.append({
            
            "results": convert_results_for_plot(met_results_binscan, "run2"),
            "label": f"MET {met}"

        })

        
        
    for flav in global_best_met_map:

        info = global_best_met_map[flav]

        print(f"\n{flav}: BEST MET = {info['best_met']}")

        for met, val in sorted(info["scores"].items()):
            print(f"  MET {met} -> Z = {val:.4f}")

        make_mass_plot_multi(

            results_list=global_met_plot_results,

            flav=flav,

            LOG_TAG=outtag,

            out_tag="sr3_global_vs_permass_metscan"
        )
        
    for flav in global_binscan_best_met_map:

        info = global_binscan_best_met_map[flav]

        print(f"\n{flav}: BEST MET = {info['best_met']}")

        for met, val in sorted(info["scores"].items()):
            print(f"  MET {met} -> Z = {val:.4f}")

        make_mass_plot_multi(

            results_list=global_binscan_met_plot_results,

            flav=flav,

            LOG_TAG=outtag,
            
            out_tag="sr3_compare_binscan_metscan"
        )


    # ----------------------------------
    # PRINT BINNING + BKG (GLOBAL)
    # ----------------------------------
    print_bkg_per_bin(data, global_scan_outputs, global_mass_config)
    print_bkg_per_bin(data, global_binscan_scan_outputs, global_binscan_mass_config)

    timer.stop("GlobalMass scan")
    
    

            
    timer.summary()
    print("\n==============================")
    print(" OUTPUT PATHS")
    print("==============================")
    print(f"[INFO] Log (stdout)   : output/logs/{outtag}/build_data_{ts}.txt")
    print(f"[INFO] Results log   : output/logs/{outtag}/results_{ts}.txt")
    
if __name__ == "__main__":
    main()
