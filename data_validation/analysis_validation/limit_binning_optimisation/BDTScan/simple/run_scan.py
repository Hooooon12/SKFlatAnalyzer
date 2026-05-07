#!/usr/bin/env python3

import subprocess

#masses = [85, 90, 95, 100, 125, 150, 200, 250, 300, 400, 500]
masses = [350]
flavours = ["MuMu", "EE", "EMu"]

for mass in masses:
    for flav in flavours:

        log_file = f"logs/run/run_logs_scan_m{mass}_{flav}.txt"

        print("=======================================")
        print(f"Running mass={mass}, flavour={flav}")
        print("=======================================")

        with open(log_file, "w") as f:
            subprocess.run(
                ["python3", "scan_run2.py", "--mass", str(mass), "--flavour", flav],
                stdout=f,
                stderr=subprocess.STDOUT
            )
