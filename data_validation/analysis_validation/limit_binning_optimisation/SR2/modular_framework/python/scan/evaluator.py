from python.config.default_config import ERAS, FLAVOURS, FAKE_FLOOR


from python.utils.helper import fix_fake_and_bkg,compute_bin_Z_with_unc,pass_stat_and_err,pass_stat_era

import math



def evaluate_scan_results_sr2(data, scan_outputs, config):

    import math
    import numpy as np

    from python.scan.scan import (
        build_arrays_sr2,
        evaluate_strategy_sr2
    )

    results = []

    # =========================================
    # PRINT SCAN HEADER
    # =========================================
    scan_name = config.get("scan_name", "SR2")

    print("\n==============================")
    print(f" FINAL EVALUATION [{scan_name}]")
    print("==============================")

    print(" Config summary:")
    print(f"  scan_type        = {config.get('scan_type')}")
    print(f"  opt_mode         = {config.get('opt_mode')}")
    print(f"  nbin_mode        = {config.get('nbin_mode')}")
    print(f"  min_bin_width    = {config.get('min_bin_width')}")
    print("==============================\n")

    # ----------------------------------------
    # Flatten outputs
    # ----------------------------------------
    flat_outputs = []

    for item in scan_outputs:
        if isinstance(item, list):
            flat_outputs.extend(item)
        else:
            flat_outputs.append(item)

    # ======================================================
    # LOOP OVER RESULTS
    # ======================================================
    for res in flat_outputs:

        if res is None:
            continue

        flavs   = res.get("flavs", [])
        masses  = res.get("masses", [])
        regions = res["best_regions"]

        for flav in flavs:
            for mass in masses:

                print("\n==============================")
                print(f" RESULTS [{scan_name}] | {flav} {mass}")
                print("==============================")

                total_Z2_run2 = 0.0
                region_outputs = {}

                # =========================================
                # LOOP REGIONS (low/high)
                # =========================================
                for region in ["low", "high"]:

                    bins = regions[region]["bins"]

                    arrays, bin_lo = build_arrays_sr2(
                        data, region, [flav], [mass]
                    )

                    Z2_region, bin_info = evaluate_strategy_sr2(
                        bins, arrays, bin_lo, [flav], [mass], config
                    )

                    Z_region = math.sqrt(Z2_region)
                    total_Z2_run2 += Z2_region

                    region_outputs[region] = {
                        "bins": bins,
                        "run2_bins": bin_info,
                        "Z": Z_region
                    }

                run2 = math.sqrt(total_Z2_run2)

                # =========================================
                # PRINT RESULTS
                # =========================================
                print(f"\n[{scan_name}] {flav} {mass} -> Run2 = {run2:.4f}")

                print("\n--- BINNING ---")
                for region in ["low", "high"]:
                    bins = regions[region]["bins"]
                    edges_str = ", ".join([f"{b:.2f}" for b in bins])
                    print(f"  {region}: [{edges_str}]")
                    
                # =========================================
                # STORE RESULT
                # =========================================
                results.append({
                    "flav": flav,
                    "mass": mass,
                    "run2": run2,
                    "quad": run2,
                    "regions": region_outputs
                })

    # =========================================
    # COMBINED SIGNIFICANCE
    # =========================================
    Z2_combined = sum(r["run2"]**2 for r in results)
    Z_combined = math.sqrt(Z2_combined)

    print("\nCombined Run2 significance (quadrature): {:.4f}".format(Z_combined))

    return results
