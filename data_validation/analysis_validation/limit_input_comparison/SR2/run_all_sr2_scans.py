#!/usr/bin/env python3

import subprocess
import itertools

# Configurations
flavours = ['MuMu', 'EE', 'EMu']
masses = ['0', '500', '1000', '2000', '20000']
nbins_list = ['4', '5', '6']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']

# Script to run
script = 'sr2_th3d_fom_scan.py'

# Run combinations with and without --loose_criteria_scan
for loose_flag in [False, True]:
    for flavour, mass, nbins, era in itertools.product(flavours, masses, nbins_list, eras):
        cmd = [
            'python3', script,
            '--sr2scan',
            '--flavour', flavour,
            '--mass', mass,
            '--nbins', nbins,
            '--era', era,
        ]
        if loose_flag:
            cmd.append('--loose_criteria_scan')

        print(f"\n[INFO] Running: {' '.join(cmd)}\n")
        subprocess.run(cmd)
