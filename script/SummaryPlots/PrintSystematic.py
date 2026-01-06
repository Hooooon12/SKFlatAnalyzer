#!/usr/bin/env python3
import sys
import argparse
import ROOT

def walk_dirs(tdir, base_path, results, hist_name, include_overflow=False):
    """Depth-first walk; when encountering a 'LimitBins' directory, read hist_name and record its integral."""
    keys = tdir.GetListOfKeys()
    if not keys:
        return
    for k in keys:
        obj = tdir.Get(k.GetName())
        if not obj:
            continue

        if obj.InheritsFrom("TDirectory"):
            name = obj.GetName()
            path = f"{base_path}/{name}" if base_path else name

            if name == "LimitBins":
                h = obj.Get(hist_name)
                if h and h.InheritsFrom("TH1"):
                    if include_overflow:
                        nb = h.GetNbinsX()
                        integral = h.Integral(0, nb + 1)
                    else:
                        integral = h.Integral()
                    results.append((f"{path}/{hist_name}", float(integral)))
            # Recurse
            walk_dirs(obj, path, results, hist_name, include_overflow=include_overflow)

def main():
    ap = argparse.ArgumentParser(
        description="Print integrals of ElectronSR3 (LimitExtraction) and ElectronSR3BDT (LimitExtractionBDT) under any LimitBins/ directory."
    )
    ap.add_argument("file", nargs="?", default="/data9/Users/HNL_public/SUS-24-014/SKFlatOutput/Systematic_Run/HNL_SignalRegion_Plotter_ANv5_BDTV2to4/2017/RunSyst__RunSignal__/HNL_SignalRegion_Plotter_SkimTree_HNMultiLepBDT_VBFTypeI_DF_M400_private.root",
                    help="Path to ROOT file.")
    ap.add_argument("--include-overflow", action="store_true",
                    help="Include underflow/overflow in integrals.")
    ap.add_argument("--summary", action="store_true",
                    help="Print per-section and grand totals.")
    args = ap.parse_args()

    ROOT.gROOT.SetBatch(True)
    f = ROOT.TFile.Open(args.file)
    if not f or f.IsZombie():
        sys.stderr.write(f"ERROR: Could not open file: {args.file}\n")
        sys.exit(1)

    sections = [
        ("LimitExtraction",    "ElectronMuonSR2"),
        ("LimitExtraction",    "ElectronSR3"),
        ("LimitExtractionBDT", "ElectronSR3BDT"),
    ]

    all_results = []
    for top, hist_name in sections:
        d = f.Get(top)
        if not d or not d.InheritsFrom("TDirectory"):
            sys.stderr.write(f"WARNING: Missing directory: {top}\n")
            continue

        results = []
        walk_dirs(d, top, results, hist_name, include_overflow=args.include_overflow)

        if results:
            print(f"\n=== {top} ({hist_name}) ===")
            print("# Path".ljust(64) + "Integral")
            for path, val in sorted(results):
                print(f"{path:64s} {val:.6f}")
        else:
            print(f"\n=== {top} ({hist_name}) ===")
            print("No histograms found under this section.")

        all_results.append((top, results))

    f.Close()

    if args.summary:
        print("\n=== SUMMARY ===")
        grand_total = 0.0
        for top, results in all_results:
            sect_total = sum(v for _, v in results)
            print(f"{top:24s} total = {sect_total:.6f}")
            grand_total += sect_total
        print(f"{'GRAND TOTAL':24s} = {grand_total:.6f}")

if __name__ == "__main__":
    main()
