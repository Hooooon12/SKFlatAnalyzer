#!/usr/bin/env python3

import subprocess
import itertools

# Configurations
flavours = ['MuMu', 'EE', 'EMu']
masses = ['0', 'SSWW_600', 'SSWW_1000', 'SSWW_20000']
nbins_list = ['6']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']

# Script to run
script = 'sr2_fom_scan.py'

# Run combinations with and without --loose_criteria_scan
for flavour, mass, nbins, era in itertools.product(flavours, masses, nbins_list, eras):
    cmd = [
        'python3', script,
        '--flavour', flavour,
        '--mass', mass,
        '--n_bins', nbins,
        '--era', era,
    ]

    print(f"\n[INFO] Running: {' '.join(cmd)}\n")
    subprocess.run(cmd)
