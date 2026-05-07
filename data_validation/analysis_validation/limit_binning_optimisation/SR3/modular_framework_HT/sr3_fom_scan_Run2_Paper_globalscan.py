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
    list_available_configs, print_sr3_bin_table,
    debug_fixed_binning_stat_failures, print_sr3_z_ref_summary,
    build_mass_weights_from_ref, build_fixed_met_results,
    recompute_per_met_results, print_bkg_per_bin,
    recompute_per_mass_with_fixed_binning
)

from python.plotter.plotter import (
    make_mass_plot_multi, convert_results_for_plot,
    build_sr3_plot_results
)

from python.config.default_config import BASE_DIR, FLAVOURS, NCORE, ERAS
import python.config.default_config

from python.config.config_utils import GetScanName

from python.utils.logger import (
    print_config_file, print_sr3_scan_table_from_results,
    debug_print_yields_integral
)

from python.scan_global.ref_fom_utils import *
from python.scan_global.fom_utils import *
from python.scan_global.evaluator import (
    evaluate_scan_results, find_best_met_per_flavour
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
        base = BASE_DIR
    else:
        base = get_latest_dir(
            "/data6/Users/jalmond/HNL/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter"
        )

    # ----------------------------------
    # Args
    # ----------------------------------
    parser = argparse.ArgumentParser()
    parser.add_argument("--debug", action="store_true")
    parser.add_argument("--config", required=True)
    parser.add_argument("--tag", default=None)

    args = parser.parse_args()

    conf_path = ConvertConfPath(args.config)
    config_module = importlib.import_module(conf_path)

    MASSES, _, _, LOG_TAG, TAG, _, _ = ReadConfig(config_module)

    import python.utils.helper
    python.utils.helper.set_stat_config(config_module)

    tag = args.tag if args.tag else TAG

    ts = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    LOG_TAG = LOG_TAG + "_globalscan"

    os.makedirs(f"output/logs/{tag}/{LOG_TAG}", exist_ok=True)
    outtag = f"{tag}/{LOG_TAG}"

    log_file = f"output/logs/{outtag}/build_data_{ts}.txt"
    results_file = f"output/logs/{outtag}/results_{ts}.txt"

    sys.stdout = TeeLogger(log_file)
    
    results_logger = SimpleLogger(results_file)

    print("[INFO] Input:", base)

    # ----------------------------------
    # Build data
    # ----------------------------------
    timer.start("Build data")
    data = build_data_sr3(base, MASSES, sig_name="HNL")
    timer.stop("Build data")

    # ----------------------------------
    # Reference
    # ----------------------------------
    ref_results = evaluate_sr3_run2_with_boundary(data)
    print_sr3_z_ref_summary(ref_results)

    ref_plot = build_sr3_plot_results(data)


    # =========================================================
    # GLOBAL NBIN STUDY
    # =========================================================
    nbin_modes = [5]
    comparison_plot_results = []

    for nbin in nbin_modes:

        print("\n==============================")
        print(f" GLOBAL SCAN nbin={nbin}")
        print("==============================")

        cfg = {
            "scan_type": "GlobalMassPerFlav",
            "nbin_mode": nbin,
            "opt_mode": "Run2",
            "mets": ["2","3"],
            "min_bin_width": 20.0,
            "min_lt_first_edge": 120.0,
        }

        cfg["scan_name"] = GetScanName(cfg)
        cfg["mass_weights"] = build_mass_weights_from_ref(ref_results)

        timer.start(f"Global scan {nbin}")

        outputs = run_parallel_global_scans(
            data, FLAVOURS, MASSES, cfg, n_workers=NCORE
        )

        final = evaluate_scan_results(data, outputs, cfg)

        print_sr3_scan_table_from_results(final, data)

        # BEST MET PRINT
        best_map = find_best_met_per_flavour(outputs, cfg["mass_weights"])

        for flav in best_map:
            print(f"\n[nbin={nbin}] {flav} best MET = {best_map[flav]['best_met']}")

        # recompute
        recomputed = recompute_per_mass_with_fixed_binning(data, outputs)

        comparison_plot_results.append({
            "results": convert_results_for_plot(recomputed, "run2"),
            "label": f"Global nbin={nbin}"
        })

        print_bkg_per_bin(data, outputs, cfg)

        timer.stop(f"Global scan {nbin}")

    # ----------------------------------
    # FINAL PLOTS
    # ----------------------------------
    for flav in FLAVOURS:

        make_mass_plot_multi(
            results_list=comparison_plot_results,
            flav=flav,
            LOG_TAG=outtag,
            out_tag="compare_all_nbins"
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
