# =========================================================                                                                                                                                                                                  
# EVALUATORs                                                                                                                                                                                                                                 
# =========================================================                                                                                                                                                                                
from python.scan_global.scan import run_scan_multi

import math
import itertools
from tqdm import tqdm


def run_parallel_global_scans(data, flavs, masses, config, n_workers=None):

    import os

    # ----------------------------------------
    # Decide number of workers safely
    # ----------------------------------------
    if n_workers is None:
        n_workers = int(
            os.environ.get(
                "SLURM_CPUS_PER_TASK",   # use cluster allocation if available
                config.get("NWORKERS", 8)  # fallback default
            )
        )

    # ----------------------------------------
    # Print config once
    # ----------------------------------------
    if config.get("_printed", False) is False:

        print("\n==============================")
        print(" SCAN CONFIG")
        print("==============================")
        print(f" scan_type   = {config.get('scan_type')}")
        print(f" opt_mode    = {config.get('opt_mode')}")
        print(f" nbin_mode   = {config.get('nbin_mode')}")
        print(f" mets        = {config.get('mets')}")
        print(f" scan_name   = {config.get('scan_name')}")
        print(f" min_bin_width:  {config.get('min_bin_width')}")
        print(f" min_lt_first_edge:  {config.get('min_lt_first_edge')}")
        print(f" n_workers (limit) = {n_workers}")
        print("==============================\n")

        config["_printed"] = True

    # ----------------------------------------
    # Build tasks (GlobalMassPerFlav)
    # ----------------------------------------
    tasks = []

    if config["scan_type"] == "PerMassPerFlav":

         for flav in flavs:
              for mass in masses:
                   tasks.append((data, [flav], [mass], config))
                   
    elif config["scan_type"] == "GlobalMassPerFlav":

         for flav in flavs:
              tasks.append((data, [flav], masses, config))
              

    elif config["scan_type"] == "PerMassGlobalFlav":

         for mass in masses:
              tasks.append((data, flavs, [mass], config))
              
    elif config["scan_type"] == "GlobalMassGlobalFlav":

         tasks.append((data, flavs, masses, config))
    else:
        raise ValueError("run_parallel_global_scans only supports GlobalMassPerFlav")

    # ----------------------------------------
    # NO multiprocessing here (important)
    # ----------------------------------------
    results = []

    for t in tasks:
        results.append(run_scan_multi(t))

    return results



