#!/usr/bin/env python3

import os
import glob
import argparse

BASE = "/data6/Users/jalmond/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter"

ERAS = [
    "2016preVFP",
    "2016postVFP",
    "2017",
    "2018",
]

PREFIX = "HNL_SignalRegion_Plotter_SkimTree_"


def get_signal_names(directory):

    files = glob.glob(directory + "/HNL_SignalRegion_Plotter_SkimTree_*.root")

    names = []

    for f in files:

        name = os.path.basename(f)

        name = name.replace(PREFIX, "")
        name = name.replace(".root", "")

        names.append(name)

    return set(names)


def compare(era):

    dir_nom = f"{BASE}/{era}/RunSignal__"
    dir_syst = f"{BASE}/{era}/RunSyst__SR_FlavEraDep__RunSignal__"


    if not os.path.isdir(dir_nom):
        print("Missing directory:", dir_nom)
        return

    if not os.path.isdir(dir_syst):
        print("Missing directory:", dir_syst)
        return

    nominal = get_signal_names(dir_nom)
    syst = get_signal_names(dir_syst)

    missing = sorted(nominal - syst)
    extra = sorted(syst - nominal)

    print("\n==============================")
    print("Era:", era)
    print("==============================")

    print("\nMissing in RunSyst__Merged__RunSignal__")

    if not missing:
        print("  None")

    for m in missing:
        print(" ", m)

    print("\nExtra in RunSyst__Merged__RunSignal__")

    if not extra:
        print("  None")

    for e in extra:
        print(" ", e)


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("--era", default="all")

    args = parser.parse_args()

    if args.era == "all":

        for era in ERAS:
            compare(era)

    else:

        compare(args.era)


if __name__ == "__main__":
    main()
