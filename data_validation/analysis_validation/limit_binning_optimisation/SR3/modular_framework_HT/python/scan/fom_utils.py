# =========================================================                                                                                                                                                                                  
# EVALUATORs                                                                                                                                                                                                                                 
# =========================================================                                                                                                                                                                                
from python.scan.scan import run_scan_multi

import math
import itertools
from tqdm import tqdm


from multiprocessing import Pool, cpu_count

def run_parallel_scans(data, flavs, masses, config, n_workers=None):

    if n_workers is None:
        n_workers = max(1, cpu_count() - 1)

    tasks = []

    
    if config.get("_printed", False) is False:

        print("\n==============================")
        print(" SCAN CONFIG")
        print("==============================")
        print(f" scan_type   = {config.get('scan_type')}")
        print(f" opt_mode    = {config.get('opt_mode')}")
        print(f" nbin_mode   = {config.get('nbin_mode')}")
        print(f" mets        = {config.get('mets')}")
        print(f" scan_name        = {config.get('scan_name')}")
        print(f" min_bin_width:  {config.get('min_bin_width')}")
        print(f" min_lt_first_edge:  {config.get('min_lt_first_edge')}")
        print(f" n_workers  = {n_workers}")
        print("==============================\n")

        config["_printed"] = True  # prevents spam in loops                                                                                                                                                        


    
    
    # ----------------------------------------
    # Build tasks depending on mode
    # ----------------------------------------
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

    # ----------------------------------------
    # Run pool
    # ----------------------------------------
    
    with Pool(n_workers) as pool:
        print("pool.map(run_scan_multi, tasks)")
        results = pool.map(run_scan_multi, tasks)

    return results







