#!/usr/bin/env python3

import os
import math
import ctypes
import ROOT

ROOT.gROOT.SetBatch(True)

DIR1 = "/data9/Users/HNL_public/SUS-24-014/SKFlatOutput/Systematic_Run/HNL_SignalRegion_Plotter_ANv7_ConvUpdate_PR192"
DIR2 = "/data9/Users/HNL_public/SUS-24-014/SKFlatOutput/Systematic_Run/HNL_SignalRegion_Plotter_ANv7_NewBinning_PR192"

ERAS = [
    "2016preVFP",
    "2016postVFP",
    "2017",
    "2018",
]

HIST_PATHS = [
    "LimitExtraction/HNL_ULIDv2/EE/LimitBins/ElectronSR3",
    "LimitExtraction/HNL_ULIDv2/MuMu/LimitBins/MuonSR3",
    "LimitExtraction/HNL_ULIDv2/EMu/LimitBins/ElectronMuonSR3",

    "LimitExtraction/HNL_ULIDv2/EE/LimitBins/ElectronSR2",
    "LimitExtraction/HNL_ULIDv2/MuMu/LimitBins/MuonSR2",
    "LimitExtraction/HNL_ULIDv2/EMu/LimitBins/ElectronMuonSR2",

    "LimitExtraction/HNL_ULIDv2/EE/M1000/LimitBins/ElectronSR1",
    "LimitExtraction/HNL_ULIDv2/MuMu/M1000/LimitBins/MuonSR1",
    "LimitExtraction/HNL_ULIDv2/EMu/M1000/LimitBins/ElectronMuonSR1",
]


def get_hist_yield_and_error(hist):

    err = ctypes.c_double(0.0)

    yld = hist.IntegralAndError(
        1,
        hist.GetNbinsX(),
        err,
    )

    return float(yld), float(err.value)


def collect_directory(base_dir, hist_path):

    results = {}

    for era in ERAS:

        era_dir = os.path.join(base_dir, era, "RunSyst__RunConv__")

        if not os.path.isdir(era_dir):
            print("[WARNING] Missing:", era_dir)
            continue

        for fname in sorted(os.listdir(era_dir)):

            if not fname.endswith(".root"):
                continue

            full_path = os.path.join(era_dir, fname)

            f = ROOT.TFile.Open(full_path)

            if not f or f.IsZombie():
                print("[WARNING] Cannot open:", full_path)
                continue

            h = f.Get(hist_path)

            if not h:
                f.Close()
                continue

            yld, err = get_hist_yield_and_error(h)

            if fname not in results:
                results[fname] = {
                    "yield": 0.0,
                    "err2": 0.0,
                }

            results[fname]["yield"] += yld
            results[fname]["err2"] += err * err

            f.Close()

    return results


for hist_path in HIST_PATHS:

    print("")
    print("=" * 180)
    print(hist_path)
    print("=" * 180)

    dir1_results = collect_directory(DIR1, hist_path)
    dir2_results = collect_directory(DIR2, hist_path)

    all_samples = sorted(
        set(dir1_results.keys()) |
        set(dir2_results.keys())
    )

    print(
        "{:<80s} {:>22s} {:>22s} {:>12s}".format(
            "Sample",
            "ConvUpdate",
            "NewBinning",
            "Diff (%)"
        )
    )

    print("-" * 180)

    total_y1 = 0.0
    total_e1_sq = 0.0

    total_y2 = 0.0
    total_e2_sq = 0.0

    for sample in all_samples:

        y1 = dir1_results.get(sample, {}).get("yield", 0.0)
        e1 = math.sqrt(dir1_results.get(sample, {}).get("err2", 0.0))

        y2 = dir2_results.get(sample, {}).get("yield", 0.0)
        e2 = math.sqrt(dir2_results.get(sample, {}).get("err2", 0.0))

        total_y1 += y1
        total_e1_sq += e1 * e1

        total_y2 += y2
        total_e2_sq += e2 * e2

        #
        # Percentage difference
        #
        if abs(y1) > 1e-12:
            diff = 100.0 * (y2 - y1) / y1
        else:
            #
            # If old yield is zero, report 100% if new is nonzero,
            # otherwise 0%.
            #
            if abs(y2) > 1e-12:
                diff = 100.0
            else:
                diff = 0.0

        print(
            "{:<80s} {:10.3f} +/- {:8.3f} {:10.3f} +/- {:8.3f} {:+10.2f}".format(
                sample,
                y1,
                e1,
                y2,
                e2,
                diff,
            )
        )

    total_e1 = math.sqrt(total_e1_sq)
    total_e2 = math.sqrt(total_e2_sq)

    if abs(total_y1) > 1e-12:
        total_diff = 100.0 * (total_y2 - total_y1) / total_y1
    else:
        if abs(total_y2) > 1e-12:
            total_diff = 100.0
        else:
            total_diff = 0.0

    print("-" * 180)

    print(
        "{:<80s} {:10.3f} +/- {:8.3f} {:10.3f} +/- {:8.3f} {:+10.2f}".format(
            "TOTAL",
            total_y1,
            total_e1,
            total_y2,
            total_e2,
            total_diff,
        )
    )

    print("=" * 180)
    print("")
