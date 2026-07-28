#!/usr/bin/env python3

import os
import ROOT

ROOT.gROOT.SetBatch(True)

base_dir = "/data9/Users/HNL_public/SUS-24-014/LimitInputs"

eras = [
    "2016preVFP",
    "2016postVFP",
    "2017",
    "2018",
]

for dirname in sorted(os.listdir(base_dir)):

    full_dir = os.path.join(base_dir, dirname)

    if not os.path.isdir(full_dir):
        continue

    print(f"\n===== {dirname} =====")

    for era in eras:

        root_path = os.path.join(
            full_dir,
            era,
            "cr1_InvBJet",
            "M1000_EE_card_input.root"
        )

        if not os.path.isfile(root_path):
            print(f"{era:12s} : [MISSING]")
            continue

        f = ROOT.TFile.Open(root_path)

        if not f or f.IsZombie():
            print(f"{era:12s} : [BAD FILE]")
            continue

        h = f.Get("data_obs")

        if not h:
            print(f"{era:12s} : [NO HIST]")
            f.Close()
            continue

        integral = h.Integral()

        print(f"{era:12s} : data_obs Integral = {integral}")

        f.Close()
