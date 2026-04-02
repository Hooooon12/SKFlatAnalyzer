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

# Expected files for RunPrompt
PROMPT_FILES = [
"WZTo3LNu_amcatnlo",
"ttHToNonbb",
"VBF_HToZZTo4L",
"tZq",
"VHToNonbb",
"TTTT",
"GluGluToZZto2e2tau",
"ttZToLLNuNu",
"TTZZ",
"GluGluToZZto2mu2tau",
"GluGluToZZto4mu",
"GluGluToZZto4e",
"GluGluToZZto4tau",
"GluGluToZZto2e2mu",
"GluGluHToZZTo4L",
"ttWToLNu",
"WpWp_QCD",
"WpWp_EWK",
"WZZ",
"ZZZ",
"WWZ",
"WWW",
"WWTo2L2Nu_DS",
"WZ_EWK",
"ZZTo4L_powheg"
]

# Expected RunConv files
CONV_FILES = [
"DYJets_MG",
"DYJets10to50_MG",
"ZGToLLG",
"ZGToLLG_PtG_130",
"TTG",
"TG",
"WZG",
"WWG",
"WGJJToLNu",
"WGToLNuG",
"WGToLNuG_MG",
"WGToLNuG_01J_PtG_130",
"WGToLNuG_01J_PtG_300",
"WGToLNuG_01J_PtG_500"
]


def check_files(directory, expected, prefix):

    missing = []

    for name in expected:

        pattern = os.path.join(
            directory,
            f"{prefix}{name}.root"
        )

        files = glob.glob(pattern)

        if not files:
            missing.append(name)

    return missing


def run_prompt(tag):

    print("\nChecking RunPrompt")

    for era in ERAS:

        d = f"{BASE}/{era}/{tag}RunPrompt__"

        prefix = "HNL_SignalRegion_Plotter_SkimTree_HNMultiLepBDT_"

        missing = check_files(d, PROMPT_FILES, prefix)

        nfound = len(PROMPT_FILES) - len(missing)

        print(f"{era:12s} found {nfound}/25")

        if missing:
            print("  Missing:")
            for m in missing:
                print("   ", m)


def run_conv(tag):

    print("\nChecking RunConv")

    for era in ERAS:

        d = f"{BASE}/{era}/{tag}RunConv__"

        missing = []

        for name in CONV_FILES:

            pattern = os.path.join(
                d,
                f"HNL_SignalRegion_Plotter_SkimTree_*{name}.root"
            )

            files = glob.glob(pattern)

            if not files:
                missing.append(name)

        nfound = len(CONV_FILES) - len(missing)

        print(f"{era:12s} found {nfound}/14")

        if missing:
            print("  Missing:")
            for m in missing:
                print("   ", m)


def run_signal(tag):

    print("\nChecking RunSignal")

    for era in ERAS:

        d = f"{BASE}/{era}/{tag}RunSignal__"

        files = glob.glob(d + "/HNL_SignalRegion_Plotter_SkimTree_*.root")

        print(f"{era:12s} {len(files)} files")


def run_fake(tag):

    print("\nChecking RunFake DATA")

    for era in ERAS:

        d = f"{BASE}/{era}/{tag}RunFake__/DATA"

        files = glob.glob(d + "/*.root")

        print(f"{era:12s} {len(files)} files")

        if len(files) == 0:
            print("  Directory empty:", d)


def run_cf(tag):

    print("\nChecking RunCF DATA")

    for era in ERAS:

        d = f"{BASE}/{era}/{tag}RunCF__/DATA"

        files = glob.glob(d + "/*.root")

        print(f"{era:12s} {len(files)} files")

        if len(files) == 0:
            print("  Directory empty:", d)


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("--tag", default="", help="Tag prefix such as RunSyst_")

    args = parser.parse_args()

    tag = args.tag

    print("\nUsing tag:", tag if tag else "(none)")

    run_prompt(tag)
    run_conv(tag)
    run_signal(tag)
    run_fake(tag)
    run_cf(tag)


if __name__ == "__main__":
    main()
