# =========================================================
# EVALUATORs
# =========================================================                                                                                                                                                                                                                                                                                                                                                                                                                             

from python.config.default_config import ERAS, FLAVOURS, FAKE_FLOOR

from python.utils.ref_bins import get_sr3_ref_edges

from python.utils.helper import fix_fake_and_bkg,compute_bin_Z_with_unc,parse_sr3_category,get_met_boundary

import math


def print_bin_summary():
    print("\n==============================")
    print(" SR3 BINNING DEBUG")
    print("==============================")
    
    for flav in FLAVOURS:
        for era in ERAS:
            for jet in ["LowJet","HighJet"]:

                edges = get_sr3_ref_edges(flav, era, jet, True)
                print(f"{flav} | {era} | {jet} |  MET <  | Edges:", edges)
                edges = get_sr3_ref_edges(flav, era, jet, False)            
                print(f"{flav} | {era} | {jet} |  MET >  | Edges:", edges)




def evaluate_sr3_run2_with_boundary(data):

    results = []

    print("\n==============================")
    print(" REF (RUN2 vs QUAD CORRECT)")
    print("==============================")

    example_met = next(iter(data))
    example_cat = next(iter(data[example_met]))
    masses = data[example_met][example_cat]["signal_nonorm"][FLAVOURS[0]].keys()

    CATEGORIES = [
        "LowJet_LT_MET{X}_LTcut",
        "HighJet_LT_MET{X}_LTcut",
        "LowJet_LT_MET{X}_GTcut",
        "HighJet_LT_MET{X}_GTcut",
    ]

    for flav in FLAVOURS:

        for mass in masses:

            total_run2_Z2 = 0.0
            total_quad_Z2 = 0.0

            region_results = {}
            total_b=0.0
            for era in ERAS:

                boundary = get_met_boundary(flav, era)

                for cat_template in CATEGORIES:

                    cat = cat_template.format(X=boundary)

                    sub = data[boundary][cat]

                    jet, is_ltcut = parse_sr3_category(cat)

                    edges_full = sub["edges"]
                    bin_lo = edges_full[:-1]

                    edges_ref = get_sr3_ref_edges(flav, era, jet, is_ltcut)

                    # init region if needed
                    if cat not in region_results:
                        region_results[cat] = {
                            "bins": edges_ref,
                            "per_era": {},
                            "run2_bins": [],
                            "Z_quad": 0.0,
                            "Z_run2": 0.0,
                        }

                    # store per-era bins
                    region_results[cat]["per_era"][era] = []

                    # per-bin accumulation containers for Run2
                    if len(region_results[cat]["run2_bins"]) == 0:
                        for i in range(len(edges_ref) - 1):
                            region_results[cat]["run2_bins"].append({
                                "S": 0.0,
                                "B": 0.0,
                                "E": 0.0
                            })

                    S_arr = sub["signal_nonorm"][flav][mass][era]
                    B_arr = sub["background"][flav][era]
                    F_arr = sub["fake"][flav][era]
                    E_arr = sub["bkg_err2"][flav][era]

                    
                    
                    era_Z2 = 0.0

                    for i in range(len(edges_ref) - 1):

                        lo = edges_ref[i]
                        hi = edges_ref[i+1]

                        if i == len(edges_ref) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        # --- sum bin FIRST ---
                        S = S_arr[mask].sum()
                        B = B_arr[mask].sum()
                        F = F_arr[mask].sum()
                        E = E_arr[mask].sum()
                        total_b+=B
                        # --- fake fix AFTER summing ---
                        F, B = fix_fake_and_bkg(F, B, FAKE_FLOOR,
                                                flavour=flav, era=era)

                        # =========================
                        # QUAD (per era)
                        # =========================
                        if S > 0 and B > 0:
                            Z = compute_bin_Z_with_unc(S, B, E)
                            era_Z2 += Z * Z
                        else:
                            Z = 0.0

                        # store per-era bin
                        region_results[cat]["per_era"][era].append({
                            "lo": lo,
                            "hi": hi,
                            "S": S,
                            "B": B,
                            "E": E,
                            "Z": Z
                        })

                        # =========================
                        # RUN2 accumulation
                        # =========================
                        region_results[cat]["run2_bins"][i]["S"] += S
                        region_results[cat]["run2_bins"][i]["B"] += B
                        region_results[cat]["run2_bins"][i]["E"] += E

                    # accumulate QUAD at region level
                    region_results[cat]["Z_quad"] += era_Z2

            # =========================
            # FINALIZE per region
            # =========================
            for cat in region_results:

                # ---- QUAD ----
                region_results[cat]["Z_quad"] = math.sqrt(region_results[cat]["Z_quad"])
                total_quad_Z2 += region_results[cat]["Z_quad"] ** 2

                # ---- RUN2 ----
                run2_Z2 = 0.0
                run2_bin_details = []

                bins = region_results[cat]["run2_bins"]
                edges = region_results[cat]["bins"]

                for i in range(len(bins)):

                    S = bins[i]["S"]
                    B = bins[i]["B"]
                    E = bins[i]["E"]

                    if S > 0 and B > 0:
                        Z = compute_bin_Z_with_unc(S, B, E)
                        run2_Z2 += Z * Z
                    else:
                        Z = 0.0

                    run2_bin_details.append({
                        "lo": edges[i],
                        "hi": edges[i+1],
                        "S": S,
                        "B": B,
                        "E": E,
                        "Z": Z
                    })

                region_results[cat]["run2_bins"] = run2_bin_details
                region_results[cat]["Z_run2"] = math.sqrt(run2_Z2)

                total_run2_Z2 += region_results[cat]["Z_run2"] ** 2

            # =========================
            # FINAL totals
            # =========================
            total_run2 = math.sqrt(total_run2_Z2)
            total_quad = math.sqrt(total_quad_Z2)

            print(f"{flav} {mass}  Run2={total_run2:.4f}  Quad={total_quad:.4f}")

            results.append({
                "flav": flav,
                "mass": mass,
                "run2": total_run2,
                "quad": total_quad,
                "met": "boundary",
                "regions": region_results,
            })

    return results




def evaluate_sr3_run2_fom(data):

    print("\n==============================")
    print(" SR3 Run2 FOM (REF CORRECT)")
    print("==============================")

    results = []

    example_met = next(iter(data))
    example_cat = next(iter(data[example_met]))
    masses = data[example_met][example_cat]["signal_nonorm"][FLAVOURS[0]].keys()

    CATEGORIES = [
        "LowJet_LT_MET{X}_LTcut",
        "HighJet_LT_MET{X}_LTcut",
        "LowJet_LT_MET{X}_GTcut",
        "HighJet_LT_MET{X}_GTcut",
    ]

    for flav in FLAVOURS:

        print(f"\n================ {flav} =================")

        for mass in masses:

            print(f"\n--- Mass {mass} ---")

            total_run2_Z2 = 0.0
            total_quad_Z2 = 0.0

            region_results = {}

            for era in ERAS:

                boundary = get_met_boundary(flav, era)

                for cat_template in CATEGORIES:

                    cat = cat_template.format(X=boundary)

                    sub = data[boundary][cat]

                    jet, is_ltcut = parse_sr3_category(cat)

                    edges_full = sub["edges"]
                    bin_lo = edges_full[:-1]

                    edges_ref = get_sr3_ref_edges(flav, era, jet, is_ltcut)

                    # init region
                    if cat not in region_results:
                        region_results[cat] = {
                            "bins": edges_ref,
                            "per_era": {},
                            "run2_bins": [],
                            "Z_quad": 0.0,
                            "Z_run2": 0.0,
                        }

                    region_results[cat]["per_era"][era] = []

                    # init run2 bins
                    if len(region_results[cat]["run2_bins"]) == 0:
                        for i in range(len(edges_ref) - 1):
                            region_results[cat]["run2_bins"].append({
                                "S": 0.0,
                                "B": 0.0,
                                "E": 0.0
                            })

                    S_arr = sub["signal_nonorm"][flav][mass][era]
                    B_arr = sub["background"][flav][era]
                    F_arr = sub["fake"][flav][era]
                    E_arr = sub["bkg_err2"][flav][era]

                    era_Z2 = 0.0

                    for i in range(len(edges_ref) - 1):

                        lo = edges_ref[i]
                        hi = edges_ref[i+1]

                        if i == len(edges_ref) - 2:
                            mask = (bin_lo >= lo) & (bin_lo <= hi)
                        else:
                            mask = (bin_lo >= lo) & (bin_lo < hi)

                        # ---- sum FIRST ----
                        S = S_arr[mask].sum()
                        B = B_arr[mask].sum()
                        F = F_arr[mask].sum()
                        E = E_arr[mask].sum()

                        # ---- fake fix AFTER ----
                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav, era=era
                        )

                        # ---- QUAD ----
                        if S > 0 and B > 0:
                            Z = compute_bin_Z_with_unc(S, B, E)
                            era_Z2 += Z * Z
                        else:
                            Z = 0.0

                        region_results[cat]["per_era"][era].append({
                            "lo": lo,
                            "hi": hi,
                            "S": S,
                            "B": B,
                            "E": E,
                            "Z": Z
                        })

                        # ---- RUN2 accumulate ----
                        region_results[cat]["run2_bins"][i]["S"] += S
                        region_results[cat]["run2_bins"][i]["B"] += B
                        region_results[cat]["run2_bins"][i]["E"] += E

                    region_results[cat]["Z_quad"] += era_Z2

            # =========================
            # FINALIZE REGIONS
            # =========================
            for cat in region_results:

                # ---- QUAD ----
                region_results[cat]["Z_quad"] = math.sqrt(region_results[cat]["Z_quad"])
                total_quad_Z2 += region_results[cat]["Z_quad"] ** 2

                # ---- RUN2 ----
                run2_Z2 = 0.0
                run2_bin_details = []

                bins = region_results[cat]["run2_bins"]
                edges = region_results[cat]["bins"]

                for i in range(len(bins)):

                    S = bins[i]["S"]
                    B = bins[i]["B"]
                    E = bins[i]["E"]

                    if S > 0 and B > 0:
                        Z = compute_bin_Z_with_unc(S, B, E)
                        run2_Z2 += Z * Z
                    else:
                        Z = 0.0

                    run2_bin_details.append({
                        "lo": edges[i],
                        "hi": edges[i+1],
                        "S": S,
                        "B": B,
                        "E": E,
                        "Z": Z
                    })

                region_results[cat]["run2_bins"] = run2_bin_details
                region_results[cat]["Z_run2"] = math.sqrt(run2_Z2)

                total_run2_Z2 += region_results[cat]["Z_run2"] ** 2

            run2 = math.sqrt(total_run2_Z2)
            quad = math.sqrt(total_quad_Z2)

            print(f"  >>> Run2 = {run2:.4f}, Quad = {quad:.4f}")

            results.append({
                "flav": flav,
                "mass": mass,
                "run2": run2,
                "quad": quad,
                "met": "boundary",
                "regions": region_results,
            })

    return results




def print_sr3_fom_summary(results):

    print("\n==============================")
    print(" SR3 RUN2 FOM SUMMARY")
    print("==============================")

    # collect flavours
    flavours = sorted(set(r["flav"] for r in results))

    for flav in flavours:

        print(f"\n================ {flav} =================")
        print("Mass     Run2 FOM    Quad FOM    Ratio")
        print("------------------------------------------------")

        # filter + sort by mass
        subset = [r for r in results if r["flav"] == flav]
        subset = sorted(subset, key=lambda x: float(x["mass"]))

        for r in subset:

            run2 = r["run2"]
            quad = r["quad"]
            ratio = (run2 / quad) if quad > 0 else 0.0

            print(f"{r['mass']:6s}   {run2:10.4f}   {quad:10.4f}   {ratio:6.3f}")
