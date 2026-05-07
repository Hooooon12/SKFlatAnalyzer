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
    list_available_configs,
    build_mass_weights_from_ref
)

from python.config.default_config import BASE_DIR, FLAVOURS, NCORE

from python.config.config_utils import GetScanName

from python.utils.logger import print_config_file

from python.scan.ref_fom_utils import compute_sr2_z_from_cuts
from python.scan.evaluator import evaluate_scan_results_sr2
from python.scan.fom_utils import run_parallel_global_scans


# =========================================================
# TIMER
# =========================================================
class Timer:
    def __init__(self):
        self.times = {}

    def start(self, key):
        self.times[key] = time.time()

    def stop(self, key):
        self.times[key] = time.time() - self.times[key]

    def summary(self):
        print("\n==============================")
        print(" TIMING SUMMARY")
        print("==============================")
        for k, v in self.times.items():
            print(f"{k:40s} : {v:.2f} s")
        print("==============================\n")


# =========================================================
# MAIN
# =========================================================
def main():

    timer = Timer()
    timer.start("TOTAL")

    # ----------------------------------
    # Input dir
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
    parser.add_argument("--config", required=True)
    parser.add_argument("--tag", default=None)
    args = parser.parse_args()

    conf_path = ConvertConfPath(args.config)
    config_module = importlib.import_module(conf_path)

    MASSES, _, LOG_TAG, TAG = ReadConfig(config_module)

    tag = args.tag if args.tag else TAG
    LOG_TAG = LOG_TAG + "_global_only"

    print_config_file(config_module, title=f"CONFIG ({conf_path})")

    print("[INFO] Input:", base)
    print("[INFO] Masses:", MASSES)
    print("[INFO] Flavours:", FLAVOURS)

    # ----------------------------------
    # Build data
    # ----------------------------------
    timer.start("Build data")
    data = build_data(base, MASSES, sig_name="HNL")
    timer.stop("Build data")

    # ----------------------------------
    # Reference weights (optional)
    # ----------------------------------
    ref_results = compute_sr2_z_from_cuts(
        data,
        {},   # you can keep empty or pass sr2_cuts
        MASSES,
        FLAVOURS
    )

    weights = build_mass_weights_from_ref(ref_results)

    # =========================================================
    # GLOBAL MASS SCAN: nbins = 3,4,5
    # =========================================================
    print("\n====================================")
    print(" GLOBAL MASS SCAN (nbins = 3,4,5)")
    print("====================================")

    for nbins in [3, 4, 5]:

        config = {
            "scan_type": "GlobalMassPerFlav",
            "nbin_mode": nbins,
            "opt_mode": "Run2",
            "min_bin_width": 0.5,
            "mass_weights": weights
        }

        config["scan_name"] = GetScanName(config)

        timer.start(f"Scan nbins={nbins}")

        scan_outputs = run_parallel_global_scans(
            data, FLAVOURS, MASSES, config, n_workers=NCORE
        )

        final_scan = evaluate_scan_results_sr2(
            data, scan_outputs, config
        )

        timer.stop(f"Scan nbins={nbins}")

        # ----------------------------------
        # PRINT RESULTS
        # ----------------------------------
        print("\n------------------------------------")
        print(f" NBINS = {nbins}")
        print("------------------------------------")

        for r in final_scan:

            flav = r["flav"] if "flav" in r else r["flavs"][0]
            mass = r["mass"] if "mass" in r else r["masses"][0]
            Z = r.get("run2", r.get("Z_run2", 0.0))

            print(f"{flav:4s} | mass={mass:>4} | Z = {Z:.4f}")

    # ----------------------------------
    # DONE
    # ----------------------------------
    timer.summary()

    print("[DONE]")


if __name__ == "__main__":
    main()
