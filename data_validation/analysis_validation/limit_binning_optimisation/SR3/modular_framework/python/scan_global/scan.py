from python.config.default_config import ERAS, FLAVOURS, FAKE_FLOOR

from python.utils.helper import fix_fake_and_bkg,compute_bin_Z_with_unc,parse_sr3_category,get_met_boundary,generate_binnings,get_nbins_for_region,pass_stat_and_err,build_run2_bkg_per_flavour,pass_stat_era

def run_scan_multi(args):
    data, flavs, masses, config = args

    return scan_multi_sr3(
        data,
        flavs=flavs,
        masses=masses,
        config=config
    )



def scan_multi_sr3(data, flavs, masses, config):

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


def evaluate_single_binning(args):
    edges, arrays, bin_lo, flavs, masses, config = args

    import math

    Z2, info = evaluate_strategy(
        edges, arrays, bin_lo, flavs, masses, config
    )

    return edges, math.sqrt(Z2), info



from multiprocessing import Pool
from tqdm import tqdm

def process_single_met(args):
    data, flavs, masses, config, met = args

    import numpy as np
    import math

    flav_label = "+".join(flavs)
    mass_label = "+".join(map(str, masses))

    total_Z2 = 0.0
    regions_out = {}

    for cat in data[met]:

        sub = data[met][cat]
        edges_full = np.array(sub["edges"])
        bin_lo = edges_full[:-1]

        arrays, bin_lo = build_arrays_multi(data, met, cat, flavs, masses)

        B_dict, E_dict = build_run2_bkg_per_flavour(arrays, flavs)

        binnings = generate_binnings(
            edges_full,
            config["nbin_mode"],
            cat,
            min_width=config["min_bin_width"],
            min_first_edge=config["min_lt_first_edge"],
            B_dict=B_dict,
            E_dict=E_dict,
            bin_lo=bin_lo
        )

        best_Z_cat = -1.0
        best_edges = None
        best_info = None

        # Progress bar per category
        for edges in tqdm(
            binnings,
            desc=f"{flav_label} | MET {met} | {cat}",
            leave=False,
            dynamic_ncols=True
        ):
            Z2, info = evaluate_strategy(
                edges, arrays, bin_lo, flavs, masses, config
            )

            Z_cat = math.sqrt(Z2)

            if Z_cat > best_Z_cat:
                best_Z_cat = Z_cat
                best_edges = edges
                best_info = info

        if best_edges is None:
            best_edges = [0, 1200]
            best_info = []
            best_Z_cat = 0.0

        regions_out[cat] = {
            "bins": best_edges,
            "bin_info": best_info,
            "Z": best_Z_cat
        }

        total_Z2 += best_Z_cat * best_Z_cat

    Z_met = math.sqrt(total_Z2)

    return {
        "met": met,
        "Z": Z_met,
        "regions": regions_out
    }


def run_scan_group(data, flavs, masses, config):

    import math

    n_workers = config.get("NWORKERS", 4)

    flav_label = "+".join(flavs)

    tasks = [
        (data, flavs, masses, config, met)
        for met in config["mets"]
    ]

    per_met_results = []

    best_global = {"Z": -1.0, "met": None, "regions": None}

    # Outer progress bar (MET level)
    with Pool(n_workers) as pool:
        for res in tqdm(
            pool.imap_unordered(process_single_met, tasks),
            total=len(tasks),
            desc=f"{flav_label} | MET scan",
            dynamic_ncols=True
        ):
            per_met_results.append(res)

            if res["Z"] > best_global["Z"]:
                best_global["Z"] = res["Z"]
                best_global["met"] = res["met"]
                best_global["regions"] = res["regions"]

    return {
        "flavs": flavs,
        "masses": masses,
        "scan_type": config["scan_type"],
        "best_met": best_global["met"],
        "Z_run2": best_global["Z"],
        "Z_quad": best_global["Z"],
        "best_regions": best_global["regions"],
        "per_met": per_met_results
    }




def build_arrays_multi(data, met, cat, flavs, masses):

    import numpy as np

    arrays = {}

    sub = data[met][cat]

    edges_full = sub["edges"]
    bin_lo = edges_full[:-1]
    nbins = len(bin_lo)

    for flav in flavs:
        for mass in masses:

            # ----------------------------------------
            # INIT RUN2 ARRAYS
            # ----------------------------------------
            S_run2 = np.zeros(nbins)
            B_run2 = np.zeros(nbins)
            F_run2 = np.zeros(nbins)
            E_run2 = np.zeros(nbins)

            # ----------------------------------------
            # INIT ERA ARRAYS
            # ----------------------------------------
            era_dict = {}

            for era in ERAS:

                # SAFETY CHECK (this is what was crashing you)
                if flav not in sub["signal_global_norm"]:
                    continue
                if mass not in sub["signal_global_norm"][flav]:
                    continue
                if era not in sub["signal_global_norm"][flav][mass]:
                    continue

                # ----------------------------------------
                # GET ARRAYS
                # ----------------------------------------
                S_arr = np.array(sub["signal_global_norm"][flav][mass][era])
                B_arr = np.array(sub["background"][flav][era])
                F_arr = np.array(sub["fake"][flav][era])
                E_arr = np.array(sub["bkg_err2"][flav][era])

                # ----------------------------------------
                # STORE ERA
                # ----------------------------------------
                era_dict[era] = (S_arr, B_arr, F_arr, E_arr)

                # ----------------------------------------
                # BUILD RUN2 (NO FAKE FIX HERE)
                # ----------------------------------------
                S_run2 += S_arr
                B_run2 += B_arr
                F_run2 += F_arr
                E_run2 += E_arr

            # ----------------------------------------
            # FINAL STORE
            # ----------------------------------------
            arrays[(flav, mass)] = {
                #"Run2": (S_run2, B_run2, F_run2, E_run2), --> forces fake corr on era
                "Era": era_dict
            }

    return arrays, bin_lo


def evaluate_strategy(edges, arrays, bin_lo, flavs, masses, config):

    import math

    total_Z2 = 0.0
    all_bin_info = []

    opt_mode = config["opt_mode"]  # "Run2" or "Era"

    mass_weights = config.get("mass_weights", {})
    scan_type = config.get("scan_type", "")
    
    mass_weights = config.get("mass_weights", {})
    
    use_mass_weights = scan_type in [
        
        "GlobalMassPerFlav",
        
        "GlobalMassGlobalFlav"
        
    ]
    
    # =====================================================
    # LOOP BINS
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
        # LOOP OVER FLAVOURS AND MASSES
        # =================================================
        for flav in flavs:
            for mass in masses:

                if use_mass_weights:
                    weight = mass_weights.get(mass, 1.0)
                else:
                    weight = 1.0
                    
                # -----------------------------------------
                # RUN2 MODE
                # -----------------------------------------
                if opt_mode == "Run2":
                    
                    s_total = 0.0
                    b_total = 0.0
                    e_total = 0.0
                    
                    for era in ERAS:
                        
                        if era not in arrays[(flav, mass)]["Era"]:
                            
                            continue

                        S_arr, B_arr, F_arr, E_arr = arrays[(flav, mass)]["Era"][era]
                        s = float(S_arr[mask].sum())
                        b = float(B_arr[mask].sum())
                        f = float(F_arr[mask].sum())
                        e = float(E_arr[mask].sum())

                        
                        # APPLY FIX PER ERA (correct place)
                        
                        f, b = fix_fake_and_bkg(
                            f, b, FAKE_FLOOR,
                            flavour=flav,
                            era=era
                        )
                        rel_e = math.sqrt(e) / b if b > 0 else 0

                        if not pass_stat_era(b,rel_e):
                            return 0.0, []   # reject this binning entirely    
                        
                        s_total += s
                        b_total += b
                        e_total += e

                    # ----------------------------------------
                    # STAT CHECK (ON TOTAL ONLY)
                    # ----------------------------------------
                    if b_total <= 0:
                        return 0.0, []
                                        
                    rel = math.sqrt(e_total) / b_total if b_total > 0 else 0
                    
                    if not pass_stat_and_err(b_total, rel):
                    
                        return 0.0, []   # reject this binning entirely

                    

                    # ----------------------------------------
                    # FINAL Z (ONLY ON TOTAL)
                    # ----------------------------------------
                    z = compute_bin_Z_with_unc(s_total, b_total, e_total)
                    Z2_bin_total +=  weight *(z * z)
                    # ---------------------------------------
                    # STORE INFO
                    # ----------------------------------------
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

                # -----------------------------------------
                # ERA MODE
                # -----------------------------------------
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
                    raise ValueError("Unknown opt_mode: {}".format(opt_mode))

        total_Z2 += Z2_bin_total

    return total_Z2, all_bin_info

def evaluate_bins(edges, S, B, F, E, bin_lo, flavs, masses, config):

    import math

    Z2 = 0.0
    info = []

    for i in range(len(edges)-1):

        lo, hi = edges[i], edges[i+1]

        if i == len(edges)-2:
            mask = (bin_lo >= lo) & (bin_lo <= hi)
        else:
            mask = (bin_lo >= lo) & (bin_lo < hi)

        s = S[mask].sum()
        b = B[mask].sum()
        f = F[mask].sum()
        e = E[mask].sum()

        # ----------------------------------------
        # FAKE FIX (CORRECT PLACE)
        # ----------------------------------------
        f, b = fix_fake_and_bkg(
            f, b, FAKE_FLOOR,
            flavour="combined",
            era="Run2"
        )

        if b > 0:
            z = compute_bin_Z_with_unc(s, b, e)
            Z2 += z*z
        else:
            z = 0.0

        info.append({
            "lo": lo,
            "hi": hi,
            "S": s,
            "B": b,
            "E": e,
            "Z": z
        })

    return Z2, info
