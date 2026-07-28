#!/usr/bin/env python3

import ROOT

fname = "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_EE_card_input.root"

f = ROOT.TFile.Open(fname)

if not f or f.IsZombie():
    raise RuntimeError("Cannot open file: %s" % fname)

eras = [
    "2016preVFP",
    "2016postVFP",
    "2017",
    "2018"
]

processes = [
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
    print("=" * 150)
    print("ERA =", era)
    print("=" * 150)

    syst = "CMS_scale_j_%s_sr2" % era

    for proc in processes:

        nominal_name = "%s_%s" % (proc, era)
        up_name      = "%s_%s_%sUp" % (proc, era, syst)
        down_name    = "%s_%s_%sDown" % (proc, era, syst)

        h_nom  = f.Get(nominal_name)
        h_up   = f.Get(up_name)
        h_down = f.Get(down_name)

        if not h_nom:
            print("\nMissing nominal histogram:", nominal_name)
            continue

        print("")
        print("-" * 150)
        print("PROCESS :", proc)
        print("NOMINAL :", nominal_name)
        print("UP      :", up_name)
        print("DOWN    :", down_name)
        print("-" * 150)

        if not h_up:
            print("WARNING: Missing Up histogram")

        if not h_down:
            print("WARNING: Missing Down histogram")

        header = (
            "Bin".ljust(6) +
            "Nominal".rjust(15) +
            "StatErr".rjust(15) +
            "Up".rjust(15) +
            "Up-Nom".rjust(15) +
            "Up/Nom".rjust(15) +
            "Down".rjust(15) +
            "Down-Nom".rjust(15) +
            "Down/Nom".rjust(15)
        )

        print(header)
        print("-" * len(header))

        nbins = h_nom.GetNbinsX()

        for ibin in range(1, nbins + 1):

            nom = h_nom.GetBinContent(ibin)
            err = h_nom.GetBinError(ibin)

            up = None
            down = None

            if h_up:
                up = h_up.GetBinContent(ibin)

            if h_down:
                down = h_down.GetBinContent(ibin)

            up_diff = up - nom if up is not None else None
            down_diff = down - nom if down is not None else None

            if nom != 0.0:
                up_ratio = up / nom if up is not None else None
                down_ratio = down / nom if down is not None else None
            else:
                up_ratio = None
                down_ratio = None

            line = (
                ("%3d" % ibin).ljust(6) +
                ("%14.6f" % nom) +
                ("%14.6f" % err)
            )

            if up is not None:
                line += (
                    ("%14.6f" % up) +
                    ("%14.6f" % up_diff) +
                    ("%14.6f" % up_ratio if up_ratio is not None else "%14s" % "N/A")
                )
            else:
                line += (
                    "%14s" % "N/A" +
                    "%14s" % "N/A" +
                    "%14s" % "N/A"
                )

            if down is not None:
                line += (
                    ("%14.6f" % down) +
                    ("%14.6f" % down_diff) +
                    ("%14.6f" % down_ratio if down_ratio is not None else "%14s" % "N/A")
                )
            else:
                line += (
                    "%14s" % "N/A" +
                    "%14s" % "N/A" +
                    "%14s" % "N/A"
                )

            print(line)

f.Close()
