#!/usr/bin/env python3

import ROOT

fname = "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_EE_card_input.root"

f = ROOT.TFile.Open(fname)

if not f or f.IsZombie():
    raise RuntimeError("Cannot open file")

eras = [
    "2016preVFP",
    "2016postVFP",
    "2017",
    "2018"
]

processes = [
    "fake",
    "cf",
    "zg",
    "wz",
    "zz",
    "ww",
    "signalDY",
    "signalVBF",
    "signalSSWW",
    "prompt_others",
    "conv_others"
]

for era in eras:

    print("")
    print("=" * 140)
    print("ERA =", era)
    print("=" * 140)

    for proc in processes:

        hname = "%s_%s" % (proc, era)

        h = f.Get(hname)

        if not h:
            print("\nMissing:", hname)
            continue

        print("")
        print("-" * 140)
        print("PROCESS =", proc)
        print("HIST    =", hname)
        print("-" * 140)

        print(
            "Bin".ljust(6) +
            "Yield".rjust(15) +
            "StatErr".rjust(15) +
            "Err/Yield".rjust(15) +
            "Flag".rjust(20)
        )

        print("-" * 75)

        nbins = h.GetNbinsX()

        for ibin in range(1, nbins + 1):

            val = h.GetBinContent(ibin)
            err = h.GetBinError(ibin)

            if val > 0.0:
                rel = err / val

                if rel > 1.0:
                    flag = "ERR>100%"
                elif rel > 0.5:
                    flag = "ERR>50%"
                else:
                    flag = ""

                print(
                    ("%3d" % ibin).ljust(6) +
                    ("%14.6f" % val) +
                    ("%14.6f" % err) +
                    ("%14.3f" % rel) +
                    flag.rjust(20)
                )

            else:

                flag = "ZERO"

                print(
                    ("%3d" % ibin).ljust(6) +
                    ("%14.6f" % val) +
                    ("%14.6f" % err) +
                    ("%14s" % "N/A") +
                    flag.rjust(20)
                )

f.Close()
