from python.config.default_config import ERAS, FLAVOURS, FAKE_FLOOR


from python.utils.helper import fix_fake_and_bkg,compute_bin_Z_with_unc,parse_sr3_category,get_met_boundary,pass_stat_and_err,pass_stat_era

import math 

def evaluate_scan_results(data, scan_outputs, config):

    import math
    import numpy as np

    from python.scan_global.scan import build_arrays_multi, evaluate_strategy

    results = []

    # =========================================
    # PRINT SCAN HEADER
    # =========================================
    scan_name = config.get("scan_name", "Unknown")

    print("\n==============================")
    print(f" FINAL EVALUATION [{scan_name}]")
    print("==============================")

    print(" Config summary:")
    print(f"  scan_type        = {config.get('scan_type')}")
    print(f"  opt_mode         = {config.get('opt_mode')}")
    print(f"  nbin_mode        = {config.get('nbin_mode')}")
    print(f"  mets             = {config.get('mets')}")
    print(f"  min_bin_width    = {config.get('min_bin_width')}")
    print(f"  min_lt_first_edge= {config.get('min_lt_first_edge')}")
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

        flavs    = res.get("flavs", [])
        masses   = res.get("masses", [])
        best_met = res["best_met"]
        regions  = res["best_regions"]
        per_met  = res.get("per_met", [])

        for flav in flavs:
            for mass in masses:

                print("\n==============================")
                print(f" RESULTS [{scan_name}] | {flav} {mass}")
                print("==============================")

                # =========================================
                # BEST MET
                # =========================================
                total_Z2_run2 = 0.0
                region_outputs = {}

                for cat in regions:

                    bins = regions[cat]["bins"]

                    # rebuild arrays EXACTLY like scan
                    arrays, bin_lo = build_arrays_multi(
                        data, best_met, cat, [flav], [mass]
                    )

                    # evaluate using SAME logic as scan
                    Z2_cat, bin_info = evaluate_strategy(
                        bins, arrays, bin_lo, [flav], [mass], config
                    )

                    Z_cat = math.sqrt(Z2_cat)
                    total_Z2_run2 += Z2_cat

                    region_outputs[cat] = {
                        "bins": bins,
                        "run2_bins": bin_info,
                        "Z": Z_cat
                    }

                run2 = math.sqrt(total_Z2_run2)

                print("\n[Best MET]")
                print(f"[{scan_name}] {flav} {mass} -> MET={best_met} Run2={run2:.4f}")

                print("  --- BINNING ---")
                for cat in regions:
                    bins = regions[cat]["bins"]
                    edges_str = ", ".join([str(int(b)) for b in bins])
                    print(f"  {cat}: [{edges_str}]")

                # =========================================
                # ALL MET RESULTS
                # =========================================
                print("\n[All MET results]")

                for met_entry in per_met:

                    met_i = met_entry["met"]
                    regions_i = met_entry.get("regions", {})

                    total_Z2_i = 0.0

                    for cat in regions_i:

                        bins = regions_i[cat]["bins"]

                        arrays, bin_lo = build_arrays_multi(
                            data, met_i, cat, [flav], [mass]
                        )

                        Z2_cat, _ = evaluate_strategy(
                            bins, arrays, bin_lo, [flav], [mass], config
                        )

                        total_Z2_i += Z2_cat

                    Z_i = math.sqrt(total_Z2_i)

                    print(f"\nMET {met_i}:")
                    print(f"[{scan_name}] {flav} {mass} -> MET={met_i} Run2={Z_i:.4f}")

                    if str(met_i) == str(best_met):
                        print("  --> BEST")

                    print("  --- BINNING ---")

                    for cat in regions_i:
                        bins = regions_i[cat]["bins"]
                        edges_str = ", ".join([str(int(b)) for b in bins])
                        print(f"  {cat}: [{edges_str}]")

                # =========================================
                # STORE RESULT (PLOTTING COMPATIBLE)
                # =========================================
                results.append({
                    "flav": flav,
                    "mass": mass,
                    "met": best_met,
                    "run2": run2,
                    "quad": run2,   # keep same for now
                    "regions": region_outputs
                })

    # =========================================
    # COMBINED SIGNIFICANCE
    # =========================================
    Z2_combined = sum(r["run2"]**2 for r in results)
    Z_combined = math.sqrt(Z2_combined)

    print("\nCombined Run2 significance (quadrature): {:.4f}".format(Z_combined))

    return results

def evaluate_scan_results_v0(data, scan_outputs, config):

    import math
    import numpy as np

    results = []

    # =========================================
    # PRINT SCAN HEADER
    # =========================================
    scan_name = config.get("scan_name", "Unknown")

    print("\n==============================")
    print(f" FINAL EVALUATION [{scan_name}]")
    print("==============================")

    print(" Config summary:")
    print(f"  scan_type        = {config.get('scan_type')}")
    print(f"  opt_mode         = {config.get('opt_mode')}")
    print(f"  nbin_mode        = {config.get('nbin_mode')}")
    print(f"  mets             = {config.get('mets')}")
    print(f"  min_bin_width    = {config.get('min_bin_width')}")
    print(f"  min_lt_first_edge= {config.get('min_lt_first_edge')}")
    print("==============================\n")

    # ----------------------------------------
    # Flatten outputs (parallel safe)
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
        best_met = res["best_met"]
        regions  = res["best_regions"]
        per_met  = res.get("per_met", [])

        for flav in flavs:
            for mass in masses:

                total_Z2_run2 = 0.0
                total_Z2_quad = 0.0

                region_outputs = {}

                # =========================================
                # BEST MET EVALUATION
                # =========================================
                for cat in regions:

                    bins = regions[cat]["bins"]

                    sub = data[best_met][cat]

                    edges_full = np.array(sub["edges"])
                    bin_lo = edges_full[:-1]

                    Z2_region = 0.0
                    bin_details = []

                    for i in range(len(bins) - 1):

                        lo = bins[i]
                        hi = bins[i + 1]

                        if i == len(bins) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        S_tot = 0.0
                        B_tot = 0.0
                        E_tot = 0.0

                        for era in ERAS:

                            if flav not in sub["signal"]:
                                continue
                            if mass not in sub["signal"][flav]:
                                continue
                            if era not in sub["signal"][flav][mass]:
                                continue

                            S_arr = sub["signal"][flav][mass][era]
                            B_arr = sub["background"][flav][era]
                            F_arr = sub["fake"][flav][era]
                            E_arr = sub["bkg_err2"][flav][era]

                            s = S_arr[mask].sum()
                            b = B_arr[mask].sum()
                            f = F_arr[mask].sum()
                            e = E_arr[mask].sum()

                            f, b = fix_fake_and_bkg(
                                f, b, FAKE_FLOOR,
                                flavour=flav,
                                era=era
                            )

                            rel_e = math.sqrt(e) / b if b > 0 else 0
                            if not pass_stat_era(b,rel_e):
                                continue
                        
                            
                            S_tot += s
                            B_tot += b
                            E_tot += e

                        if B_tot > 0:
                            rel = math.sqrt(E_tot) / B_tot if B_tot > 0 else 0

                            if pass_stat_and_err(B_tot, rel) and S_tot > 0:
                                Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot)
                                Z2_region += Z * Z
                            else:
                                Z = 0.0
                        else:
                            Z = 0.0

                        bin_details.append({
                            "lo": lo,
                            "hi": hi,
                            "S": S_tot,
                            "B": B_tot,
                            "E": E_tot,
                            "Z": Z
                        })

                    total_Z2_run2 += Z2_region

                    # QUAD
                    Z2_quad_region = 0.0

                    for era in ERAS:

                        Z2_era = 0.0

                        for i in range(len(bins) - 1):

                            lo = bins[i]
                            hi = bins[i + 1]

                            if i == len(bins) - 2:
                                mask = (bin_lo >= lo) & (bin_lo <= hi)
                            else:
                                mask = (bin_lo >= lo) & (bin_lo < hi)

                            if flav not in sub["signal"]:
                                continue
                            if mass not in sub["signal"][flav]:
                                continue
                            if era not in sub["signal"][flav][mass]:
                                continue

                            S_arr = sub["signal"][flav][mass][era]
                            B_arr = sub["background"][flav][era]
                            F_arr = sub["fake"][flav][era]
                            E_arr = sub["bkg_err2"][flav][era]

                            s = S_arr[mask].sum()
                            b = B_arr[mask].sum()
                            f = F_arr[mask].sum()
                            e = E_arr[mask].sum()

                            f, b = fix_fake_and_bkg(
                                f, b, FAKE_FLOOR,
                                flavour=flav,
                                era=era
                            )

                            if s > 0 and b > 1e-12:
                                z = compute_bin_Z_with_unc(s, b, e)
                                Z2_era += z * z

                        Z2_quad_region += Z2_era

                    total_Z2_quad += Z2_quad_region

                    region_outputs[cat] = {
                        "bins": bins,
                        "run2_bins": bin_details,
                        "Z": math.sqrt(Z2_region)
                    }

                run2 = math.sqrt(total_Z2_run2)
                quad = math.sqrt(total_Z2_quad)

                print("\n==============================")
                print(f" RESULTS [{scan_name}] | {flav} {mass}")
                print("==============================")

                # =========================================
                # BEST MET
                # =========================================
                print("\n[Best MET]")
                print(f"[{scan_name}] {flav} {mass} -> MET={best_met} Run2={run2:.4f} Quad={quad:.4f}")

                print("  --- BINNING ---")
                for cat in regions:
                    bins = regions[cat]["bins"]
                    edges_str = ", ".join([str(int(b)) for b in bins])
                    print(f"  {cat}: [{edges_str}]")

                # =========================================
                # ALL MET RESULTS
                # =========================================
                print("\n[All MET results]")

                for met_entry in per_met:

                    met_i = met_entry["met"]
                    Z_i = met_entry["Z"]
                    regions_i = met_entry.get("regions", {})

                    print(f"\nMET {met_i}:")
                    print(f"[{scan_name}] {flav} {mass} -> MET={met_i} Run2={Z_i:.4f}")

                    if str(met_i) == str(best_met):
                        print("  --> BEST")

                    print("  --- BINNING ---")

                    for cat in regions_i:
                        bins = regions_i[cat]["bins"]
                        edges_str = ", ".join([str(int(b)) for b in bins])
                        print(f"  {cat}: [{edges_str}]")

                results.append({
                    "flav": flav,
                    "mass": mass,
                    "met": best_met,
                    "run2": run2,
                    "quad": quad,
                    "regions": region_outputs
                })

    # =========================================
    # COMBINED SIGNIFICANCE
    # =========================================
    Z2_combined = sum(r["run2"]**2 for r in results)
    Z_combined = math.sqrt(Z2_combined)

    print("\nCombined Run2 significance (quadrature): {:.4f}".format(Z_combined))

    return results


def find_best_met_per_flavour(scan_outputs, mass_weights):

    best_met_per_flav = {}

    # flatten (same as evaluator)
    flat = []
    for item in scan_outputs:
        if isinstance(item, list):
            flat.extend(item)
        else:
            flat.append(item)

    # ----------------------------------------
    # collect per flavour
    # ----------------------------------------
    per_flav = {}

    for res in flat:

        if res is None:
            continue

        flavs = res["flavs"]
        masses = res["masses"]

        for flav in flavs:
            for mass in masses:

                key = (flav, mass)

                if key not in per_flav:
                    per_flav[key] = res["per_met"]

    # ----------------------------------------
    # group by flavour
    # ----------------------------------------
    flav_to_mass = {}

    for (flav, mass), per_met in per_flav.items():

        if flav not in flav_to_mass:
            flav_to_mass[flav] = []

        flav_to_mass[flav].append((mass, per_met))

    # ----------------------------------------
    # compute best MET per flavour
    # ----------------------------------------
    for flav in flav_to_mass:

        met_scores = {}

        for mass, per_met_list in flav_to_mass[flav]:

            weight = mass_weights.get(mass, 1.0)

            for entry in per_met_list:

                met = entry["met"]
                Z   = entry["Z"]

                if met not in met_scores:
                    met_scores[met] = 0.0

                # combine in quadrature with weight
                met_scores[met] += (weight * Z) ** 2

        # final sqrt
        for met in met_scores:
            met_scores[met] = math.sqrt(met_scores[met])

        best_met = max(met_scores, key=met_scores.get)

        best_met_per_flav[flav] = {
            "best_met": best_met,
            "scores": met_scores
        }

    return best_met_per_flav
