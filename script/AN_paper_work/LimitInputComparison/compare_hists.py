#!/usr/bin/env python3

import os
import argparse
import ROOT

ROOT.gROOT.SetBatch(True)


def get_histograms(root_file):
    hists = {}
    for key in root_file.GetListOfKeys():
        obj = key.ReadObj()
        if obj.InheritsFrom("TH1"):
            #hists[obj.GetName()] = obj.Integral()
            hists[obj.GetName()] = obj.GetBinContent(1)
    return hists


def process_file(file1_path, file2_path, histname=None):
    f1 = ROOT.TFile.Open(file1_path)
    f2 = ROOT.TFile.Open(file2_path)

    if not f1 or f1.IsZombie():
        print("  Failed to open:", file1_path)
        return
    if not f2 or f2.IsZombie():
        print("  Failed to open:", file2_path)
        return

    hists1 = get_histograms(f1)
    hists2 = get_histograms(f2)

    # If specific histogram requested
    if histname:
        val1 = hists1.get(histname, None)
        val2 = hists2.get(histname, None)

        if val1 is None and val2 is None:
            print("    Histogram not found in either file:", histname)
        else:
            v1 = val1 if val1 is not None else 0.0
            v2 = val2 if val2 is not None else 0.0

            diff_tag = ""
            if abs(v1 - v2) > 1e-6:
                diff_tag = " <--- DIFF"

            print("    {0:30s} {1:12.3f} {2:12.3f}{3}".format(histname, v1, v2, diff_tag))

        f1.Close()
        f2.Close()
        return

    # Default: print all histograms
    all_hists = sorted(set(hists1.keys()) | set(hists2.keys()))

    print("    {0:30s} {1:12s} {2:12s}".format("Source", "Dir1", "Dir2"))
    print("    " + "-" * 60)

    for h in all_hists:
        val1 = hists1.get(h, 0.0)
        val2 = hists2.get(h, 0.0)

        diff_tag = ""
        if abs(val1 - val2) > 1e-6:
            diff_tag = " <--- DIFF"

        print("    {0:30s} {1:12.3f} {2:12.3f}{3}".format(h, val1, val2, diff_tag))

    f1.Close()
    f2.Close()


def main():
    parser = argparse.ArgumentParser(description="Compare ROOT histogram integrals")
    parser.add_argument("--dir1", required=True)
    parser.add_argument("--dir2", required=True)
    parser.add_argument("--mass", required=True)
    parser.add_argument("--histname", default=None, help="Only compare this histogram")

    args = parser.parse_args()

    #channels = ["EE", "MuMu", "EMu"]
    channels = ["EE"]

    eras = sorted(os.listdir(args.dir1))

    for era in eras:
        era_path1 = os.path.join(args.dir1, era)
        era_path2 = os.path.join(args.dir2, era)

        if not os.path.isdir(era_path1):
            continue

        print("\n============================================================")
        print("Era:", era)
        print("============================================================")

        subdirs = sorted(os.listdir(era_path1))

        for subdir in subdirs:
            sub_path1 = os.path.join(era_path1, subdir)
            sub_path2 = os.path.join(era_path2, subdir)

            if not os.path.isdir(sub_path1):
                continue

            print("\n  Region:", subdir)
            print("  --------------------------------------------------------")

            for ch in channels:
                filename = "M{0}_{1}_card_input.root".format(args.mass, ch)

                file1 = os.path.join(sub_path1, filename)
                file2 = os.path.join(sub_path2, filename)

                if not os.path.exists(file1) or not os.path.exists(file2):
                    print("    Missing:", filename)
                    continue

                print("\n    File:", filename)
                process_file(file1, file2, args.histname)


if __name__ == "__main__":
    main()
