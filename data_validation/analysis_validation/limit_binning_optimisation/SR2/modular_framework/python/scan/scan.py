from python.config.default_config import ERAS, FLAVOURS, FAKE_FLOOR

from python.utils.helper import (
    fix_fake_and_bkg,
    compute_bin_Z_with_unc,
    generate_binnings,
    pass_stat_and_err,
    pass_stat_era,
    build_run2_bkg_per_flavour
)

def run_scan_multi(args):
    data, flavs, masses, config = args

    return scan_multi_sr2(
        data,
        flavs=flavs,
        masses=masses,
        config=config
    )



def scan_multi_sr2(data, flavs, masses, config):

    from tqdm import tqdm

    results = []
    
    # ----------------------------------------
    # Decide flavour loop
    # ----------------------------------------
    scan_type = config["scan_type"]


    if scan_type == "PerMassPerFlav":
        
        flav_groups = [[f] for f in flavs]
        
        mass_groups = [[m] for m in masses]

    elif scan_type == "GlobalMassPerFlav":
        
        flav_groups = [[f] for f in flavs]
        
        mass_groups = [masses]
        
    elif scan_type == "PerMassGlobalFlav":
        
        flav_groups = [flavs]
        
        mass_groups = [[m] for m in masses]
        
    elif scan_type == "GlobalMassGlobalFlav":
        
        flav_groups = [flavs]
        
        mass_groups = [masses]
        
    else:
        
        raise ValueError(f"Unknown scan_type: {scan_type}")


    for flav_group in tqdm(flav_groups, desc="Flavour groups"):

        for mass_group in tqdm(mass_groups, desc="Mass groups", leave=False):

            result = run_scan_group(data, flav_group, mass_group, config)

            results.append(result)

    return results




def build_arrays_sr2(data, region, flavs, masses):

    import numpy as np

    arrays = {}

    sub = data[region]

    edges = sub["edges"]
    bin_lo = edges[:-1]
    nbins = len(bin_lo)

    for flav in flavs:
        for mass in masses:

            era_dict = {}

            for era in ERAS:

                S = np.array(sub["signal_norm_flav"][flav][mass][era])
                B = np.array(sub["background"][flav][era])
                F = np.array(sub["fake"][flav][era])
                E = np.array(sub["bkg_err2"][flav][era])

                era_dict[era] = (S, B, F, E)

            arrays[(flav, mass)] = {
                "Era": era_dict
            }

    return arrays, bin_lo


def run_scan_group(data, flavs, masses, config):

    import numpy as np
    import math
    from tqdm import tqdm

    regions = ["low", "high"]

    flav_label = "+".join(flavs)
    mass_label = "+".join(map(str, masses))

    region_results = {}

    # =========================================
    # LOOP REGIONS (low / high)
    # =========================================
    for region in regions:

        sub = data[region]

        edges_full = np.array(sub["edges"])
        
        # ----------------------------------------
        # BUILD ARRAYS
        # ----------------------------------------
        arrays, bin_lo = build_arrays_sr2(data, region, flavs, masses)

        B_dict, E_dict = build_run2_bkg_per_flavour(arrays, flavs)

        # ----------------------------------------
        # BINNING
        # ----------------------------------------

        nbin_mode = config["nbin_mode"]

        if isinstance(nbin_mode, dict):
    
            nbin_mode_region = nbin_mode[region]
            
        else:

            nbin_mode_region = nbin_mode
            
        binnings = generate_binnings(
            edges_full,
            nbin_mode_region,
            region,
            min_width=config["min_bin_width"],
            B_dict=B_dict,
            E_dict=E_dict,
            bin_lo=bin_lo,
            coarse_grid=config.get("COARSE_GRID", None)   
        )

        best_Z_region = -1.0
        best_edges = None
        best_info = None

        for edges in tqdm(
            binnings,
            desc=f"{flav_label} | {region}",
            leave=False
        ):
            
            Z2, info = evaluate_strategy_sr2(
                edges, arrays, bin_lo, flavs, masses, config
            )

            Z = math.sqrt(Z2)

            if Z > best_Z_region:
                best_Z_region = Z
                best_edges = edges
                best_info = info

        if best_edges is None:
            best_edges = [0, 1200]
            best_info = []
            best_Z_region = 0.0

        region_results[region] = {
            "bins": best_edges,
            "bin_info": best_info,
            "Z": best_Z_region
        }

    # =========================================
    # COMBINE LOW + HIGH
    # =========================================
    total_Z2 = sum(
        region_results[r]["Z"]**2 for r in regions
    )

    total_Z = math.sqrt(total_Z2)

    return {
        "flavs": flavs,
        "masses": masses,
        "scan_type": config["scan_type"],
        "Z_run2": total_Z,
        "Z_quad": total_Z,
        "best_regions": region_results
    }




def evaluate_strategy_sr2(edges, arrays, bin_lo, flavs, masses, config):

    import math

    total_Z2 = 0.0
    all_bin_info = []

    opt_mode = config.get("opt_mode", "Run2")

    mass_weights = config.get("mass_weights", {})
    scan_type = config.get("scan_type", "")

    use_mass_weights = scan_type in [
        "GlobalMassPerFlav",
        "GlobalMassGlobalFlav"
    ]

    # =====================================================
    # LOOP OVER BINS
    # =====================================================
    for i in range(len(edges) - 1):

        lo = edges[i]
        hi = edges[i + 1]

        if i == len(edges) - 2:
            mask = (bin_lo >= lo) & (bin_lo <= hi)
        else:
            mask = (bin_lo >= lo) & (bin_lo < hi)

        Z2_bin_total = 0.0

        # =================================================
        # LOOP OVER FLAVOURS + MASSES
        # =================================================
        for flav in flavs:
            for mass in masses:

                weight = mass_weights.get(mass, 1.0) if use_mass_weights else 1.0

                # =========================================
                # RUN2 MODE
                # =========================================
                if opt_mode == "Run2":

                    s_total = 0.0
                    b_total = 0.0
                    e_total = 0.0
                    
                    bin_ok = True
                    for era in ERAS:

                        if era not in arrays[(flav, mass)]["Era"]:
                            continue

                        S_arr, B_arr, F_arr, E_arr = arrays[(flav, mass)]["Era"][era]

                        s = float(S_arr[mask].sum())
                        b = float(B_arr[mask].sum())
                        f = float(F_arr[mask].sum())
                        e = float(E_arr[mask].sum())

                        # ---- fake correction per era ----
                        f, b = fix_fake_and_bkg(
                            f, b, FAKE_FLOOR,
                            flavour=flav,
                            era=era
                        )

                        rel_e = math.sqrt(e) / b if b > 0 else 0

                        if not pass_stat_era(b, rel_e):

                            bin_ok = False
                            
                            break

                        
                        s_total += s
                        b_total += b
                        e_total += e

                    # ----------------------------------------
                    # STAT + UNCERTAINTY CUT
                    # ----------------------------------------
                    if b_total <= 0:
                        return 0.0, []
                    if not bin_ok:
                        return 0.0, []
                    
                    rel = math.sqrt(e_total) / b_total if b_total > 0 else 0

                    if not pass_stat_and_err(b_total, rel):
                        return 0.0, []

                    # ----------------------------------------
                    # COMPUTE Z
                    # ----------------------------------------
                    z = compute_bin_Z_with_unc(s_total, b_total, e_total)

                    Z2_bin_total += weight * (z * z)

                    all_bin_info.append({
                        "lo": lo,
                        "hi": hi,
                        "flav": flav,
                        "mass": mass,
                        "era": "Run2",
                        "S": s_total,
                        "B": b_total,
                        "E": e_total,
                        "Z": z
                    })

                # =========================================
                # ERA MODE
                # =========================================
                elif opt_mode == "Era":

                    for era in ERAS:

                        if era not in arrays[(flav, mass)]["Era"]:
                            continue

                        S_arr, B_arr, F_arr, E_arr = arrays[(flav, mass)]["Era"][era]

                        s = float(S_arr[mask].sum())
                        b = float(B_arr[mask].sum())
                        f = float(F_arr[mask].sum())
                        e = float(E_arr[mask].sum())

                        f, b = fix_fake_and_bkg(
                            f, b, FAKE_FLOOR,
                            flavour=flav,
                            era=era
                        )

                        if b > 0:
                            z = compute_bin_Z_with_unc(s, b, e)

                            Z2_bin_total += weight * (z * z)

                            all_bin_info.append({
                                "lo": lo,
                                "hi": hi,
                                "flav": flav,
                                "mass": mass,
                                "era": era,
                                "S": s,
                                "B": b,
                                "E": e,
                                "Z": z
                            })

                else:
                    raise ValueError(f"Unknown opt_mode: {opt_mode}")

        total_Z2 += Z2_bin_total

    return total_Z2, all_bin_info

