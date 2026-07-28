#!/usr/bin/env python3

import os
import glob
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True


ERAS = [
    "2016preVFP",
    "2016postVFP",
    "2017",
    "2018"
]


BASE = "/data9/Users/HNL_public/SUS-24-014/SKFlatOutput/Systematic_Run/HNL_SignalRegion_Plotter_ANv7_NewBinning_PR192"

HIST_PATH = "LimitExtraction/HNL_ULIDv2/MuMu/M1000/LimitBins/MuonSR1"


def get_integral(rootfile, histname):

    h = rootfile.Get(histname)

    if not h:

        return None

    return h.Integral()


def main():

    #
    # collect all unique ROOT filenames
    #

    all_files = set()

    for era in ERAS:

        pattern = os.path.join(
            BASE,
            era,
            "RunSyst__RunPrompt__",
            "*.root"
        )

        for f in glob.glob(pattern):

            all_files.add(
                os.path.basename(f)
            )


    all_files = sorted(list(all_files))


    print("")
    print("=" * 120)
    print("RUN2 INTEGRALS")
    print("=" * 120)
    print("")


    #
    # loop over ROOT filenames
    #

    for fname in all_files:

        run2_integral = 0.0

        found = False

        era_vals = {}


        for era in ERAS:

            path = os.path.join(
                BASE,
                era,
                "RunSyst__RunPrompt__",
                fname
            )

            if not os.path.isfile(path):

                continue


            f = ROOT.TFile.Open(path)

            if not f or f.IsZombie():

                continue


            val = get_integral(
                f,
                HIST_PATH
            )

            f.Close()


            if val is None:

                continue


            found = True

            era_vals[era] = val

            run2_integral += val


        if not found:

            continue


        print("=" * 120)
        print(fname)
        print("=" * 120)

        for era in ERAS:

            if era in era_vals:

                print(
                    "  {:<15} : {:.6f}".format(
                        era,
                        era_vals[era]
                    )
                )

        print("")
        print(
            "  {:<15} : {:.6f}".format(
                "Run2",
                run2_integral
            )
        )

        print("")


if __name__ == "__main__":

    main()
