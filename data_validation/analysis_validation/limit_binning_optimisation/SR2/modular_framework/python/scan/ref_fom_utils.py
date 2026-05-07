from python.config.default_config import ERAS, FLAVOURS, FAKE_FLOOR
from python.utils.helper import (
    fix_fake_and_bkg,
    compute_bin_Z_with_unc,
    pass_stat_and_err
)

# =========================================================                                                                                                                                                                         
# PREDEFINED SCAN                                                                                                                                                                                                                    
# =========================================================

def compute_sr2_z_from_cuts(data, sr2_cuts, masses, flavours):

    import numpy as np
    import math

    results = []

    for flav in flavours:
        for mass in masses:

            total_Z2 = 0.0

            # ----------------------------------------
            # DEFINE GLOBAL BINNING (use reference era)
            # ----------------------------------------
            ref_era = "2018"  # or ERAS[0]

            if ref_era not in sr2_cuts or flav not in sr2_cuts[ref_era]:
                continue

            cuts = sr2_cuts[ref_era][flav]

            region_bins = {
                "low":  [0.0, cuts[0], cuts[1], 10.0],
                "high": [0.0, cuts[2], cuts[3], 10.0]
            }

            # ----------------------------------------
            # LOOP REGIONS
            # ----------------------------------------
            for region in ["low", "high"]:

                sub = data[region]

                edges_full = np.array(sub["edges"])
                bin_lo = edges_full[:-1]

                edges = region_bins[region]

                region_Z2 = 0.0

                # ----------------------------------------
                # LOOP BINS
                # ----------------------------------------
                for i in range(len(edges) - 1):

                    lo = edges[i]
                    hi = edges[i + 1]

                    if i == len(edges) - 2:
                        mask = (bin_lo >= lo) & (bin_lo <= hi)
                    else:
                        mask = (bin_lo >= lo) & (bin_lo < hi)

                    S_tot = 0.0
                    B_tot = 0.0
                    E_tot = 0.0

                    # ----------------------------------------
                    # SUM OVER ERAS
                    # ----------------------------------------
                    for era in ERAS:

                        if era not in sr2_cuts:
                            continue
                        if flav not in sr2_cuts[era]:
                            continue

                        S_arr = sub["signal_norm_flav"][flav][mass][era]
                        B_arr = sub["background"][flav][era]
                        F_arr = sub["fake"][flav][era]
                        E_arr = sub["bkg_err2"][flav][era]

                        S = float(S_arr[mask].sum())
                        B = float(B_arr[mask].sum())
                        F = float(F_arr[mask].sum())
                        E = float(E_arr[mask].sum())

                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav,
                            era=era
                        )

                        S_tot += S
                        B_tot += B
                        E_tot += E

                    if B_tot > 0:
                        Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot)
                    else:
                        Z = 0.0

                    region_Z2 += Z * Z

                total_Z2 += region_Z2

            Z_run2 = math.sqrt(total_Z2)

            results.append({
                "flav": flav,
                "mass": mass,
                "run2": Z_run2,
                "quad": Z_run2,
                "regions": {
                    "low":  {"bins": region_bins["low"]},
                    "high": {"bins": region_bins["high"]}
                }
            })

    return results

def evaluate_per_era(edges_low, edges_high, low, high, cache,
                     fake_low_per_era, fake_high_per_era):

    results = {}

    # =========================================================                                                                                                                                                                                                                       
    # TRUE RUN2 MODE: FOM(s_Run2, sum b_era)                                                                                                                                                                                                                                          
    # =========================================================                                                                                                                                                                                                                       
    if USE_TRUE_RUN2_FOM:

        total = 0

        for m, c in cache.items():

            f_bins = []

            # -------------------------                                                                                                                                                                                                                                               
            # LOW bins                                                                                                                                                                                                                                                                
            # -------------------------                                                                                                                                                                                                                                               
            for i in range(len(edges_low)-1):
                lo = edges_low[i]
                hi = edges_low[i+1]

                bkg_sum = 0
                for era2 in ERAS:
                    sub = [b for b in low[era2] if lo <= b[0] < hi]
                    bkg_sum += correct_bkg(lo, hi, sub, fake_low_per_era[era2])

                sig = sum(v for x, v in c.items() if lo <= x < hi)
                f_bins.append(fom(sig, bkg_sum))

            # -------------------------                                                                                                                                                                                                                                               
            # HIGH bins                                                                                                                                                                                                                                                               
            # -------------------------                                                                                                                                                                                                                                               
            for i in range(len(edges_high)-1):
                lo = edges_high[i]
                hi = edges_high[i+1]

                bkg_sum = 0
                for era2 in ERAS:
                    sub = [b for b in high[era2] if lo <= b[0] < hi]
                    bkg_sum += correct_bkg(lo, hi, sub, fake_high_per_era[era2])

                sig = sum(v for x, v in c.items() if lo <= x < hi)
                f_bins.append(fom(sig, bkg_sum))

            total += sum(x*x for x in f_bins)

        f_total = math.sqrt(total)

        # In Run2 mode, per-era breakdown is not meaningful                                                                                                                                                                                                                           
        # Assign same value for compatibility with existing code                                                                                                                                                                                                                      
        for era in ERAS:
            results[era] = f_total

        return results

    # =========================================================                                                                                                                                                                                                                       
    # ORIGINAL MODE: per-era FOM                                                                                                                                                                                                                                                      
    # =========================================================                                                                                                                                                                                                                       
    for era in ERAS:

        total = 0

        for m, c in cache.items():

            f_bins = []

            # -------------------------                                                                                                                                                                                                                                               
            # LOW bins                                                                                                                                                                                                                                                                
            # -------------------------                                                                                                                                                                                                                                               
            for i in range(len(edges_low)-1):
                lo = edges_low[i]
                hi = edges_low[i+1]

                sub = [b for b in low[era] if lo <= b[0] < hi]
                bkg = correct_bkg(lo, hi, sub, fake_low_per_era[era])

                sig = sum(v for x, v in c.items() if lo <= x < hi)
                f_bins.append(fom(sig, bkg))

            # -------------------------                                                                                                                                                                                                                                               
            # HIGH bins                                                                                                                                                                                                                                                               
            # -------------------------                                                                                                                                                                                                                                               
            for i in range(len(edges_high)-1):
                lo = edges_high[i]
                hi = edges_high[i+1]

                sub = [b for b in high[era] if lo <= b[0] < hi]
                bkg = correct_bkg(lo, hi, sub, fake_high_per_era[era])

                sig = sum(v for x, v in c.items() if lo <= x < hi)
                f_bins.append(fom(sig, bkg))

            total += sum(x*x for x in f_bins)

        results[era] = math.sqrt(total)

    return results


def evaluate_fixed_bins(edges_low, edges_high, low, high, cache,
                        fake_low_per_era, fake_high_per_era):

    total = 0

    for m, c in cache.items():

        f_bins = []

        # =========================================================                                                                                                                                        
        # TRUE RUN2 MODE                                                                                                                                                                                   
        # =========================================================                                                                                                                                        
        if USE_TRUE_RUN2_FOM:

            # -------------------------                                                                                                                                                                    
            # LOW bins                                                                                                                                                                                     
            # -------------------------                                                                                                                                                                    
            for i in range(len(edges_low)-1):
                lo = edges_low[i]
                hi = edges_low[i+1]

                bkg_sum = 0
                for era in ERAS:
                    sub = [b for b in low[era] if lo <= b[0] < hi]
                    bkg_sum += correct_bkg(lo, hi, sub, fake_low_per_era[era])

                sig = sum(v for x, v in c.items() if lo <= x < hi)
                f_bins.append(fom(sig, bkg_sum))

            # -------------------------                                                                                                                                                                    
            # HIGH bins                                                                                                                                                                                    
            # -------------------------                                                                                                                                                                    
            for i in range(len(edges_high)-1):
                lo = edges_high[i]
                hi = edges_high[i+1]

                bkg_sum = 0
                for era in ERAS:
                    sub = [b for b in high[era] if lo <= b[0] < hi]
                    bkg_sum += correct_bkg(lo, hi, sub, fake_high_per_era[era])

                sig = sum(v for x, v in c.items() if lo <= x < hi)
                f_bins.append(fom(sig, bkg_sum))

        # =========================================================                                                                                                                                        
        # ORIGINAL PER-ERA MODE                                                                                                                                                                            
        # =========================================================                                                                                                                                        
        else:

            for era in ERAS:

                # LOW                                                                                                                                                                                      
                for i in range(len(edges_low)-1):
                    lo = edges_low[i]
                    hi = edges_low[i+1]

                    sub = [b for b in low[era] if lo <= b[0] < hi]
                    bkg = correct_bkg(lo, hi, sub, fake_low_per_era[era])

                    sig = sum(v for x, v in c.items() if lo <= x < hi)
                    f_bins.append(fom(sig, bkg))

                # HIGH                                                                                                                                                                                     
                for i in range(len(edges_high)-1):
                    lo = edges_high[i]
                    hi = edges_high[i+1]

                    sub = [b for b in high[era] if lo <= b[0] < hi]
                    bkg = correct_bkg(lo, hi, sub, fake_high_per_era[era])

                    sig = sum(v for x, v in c.items() if lo <= x < hi)
                    f_bins.append(fom(sig, bkg))

        total += sum(x*x for x in f_bins)

    return math.sqrt(total)




def scan_predefined_verbose(bins_low, bins_high,
                           sig_cache, mode,
                            all_low, fake_low_per_era, fake_high_per_era):

    print("\n==============================")
    print("[REFERENCE] Predefined bin scan")
    print("==============================")

    boundaries = [1.0,1.5,2.0,2.5,3.0,3.5,4.0,5.0,7.5]

    results = []

    for b1 in boundaries:
        for b2 in boundaries:
            if b1 >= b2: continue

            for b3 in boundaries:
                for b4 in boundaries:
                    if b3 >= b4: continue

                    low  = [0, b1, b2, 10]
                    high = [0, b3, b4, 10]

                    r_low  = worker((low, bins_low, sig_cache, mode, all_low, fake_low_per_era))
                    r_high = worker((high, bins_high, sig_cache, mode, all_low, fake_high_per_era))

                    if not r_low or not r_high:
                        continue

                    per_era_tmp = evaluate_per_era(
                        low, high,
                        bins_low, bins_high,
                        sig_cache,
                        fake_low_per_era,
                        fake_high_per_era
                    )

                    f_run = combine_per_era_fom(per_era_tmp)

                    # per-era FOM                                                                                                                                                                                                    
                    f_per = evaluate_fixed_bins(
                        low, high,
                        bins_low, bins_high,
                        sig_cache,
                        fake_low_per_era,
                        fake_high_per_era
                    )

                    # --- STAT CHECKS ---                                                                                                                                                                                            
                    pass_run2 = check_run2_per_flavour(low, all_low) and \
                        check_run2_per_flavour(high, all_low)

                    pass_per  = check_perera(low, bins_low) and \
                        check_perera(high, bins_high)

                    # --- BKG ---                                                                                                                                                                                                    
                    def get_bkg(edges, bins):
                        vals = []
                        for i in range(len(edges)-1):
                            lo, hi = edges[i], edges[i+1]
                            sub = [b for b in bins if lo <= b[0] < hi]
                            vals.append(round(sum(x[2] for x in sub),2))
                        return vals

                    bkg_low  = get_bkg(low,  list(bins_low.values())[0])
                    bkg_high = get_bkg(high, list(bins_high.values())[0])

                    results.append({
                        "low": low,
                        "high": high,
                        "f_run": f_run,
                        "f_per": f_per,
                        "pass_run2": pass_run2,
                        "pass_per": pass_per,
                        "bkg_low": bkg_low,
                        "bkg_high": bkg_high
                    })

    # -------------------------                                                                                                                                                                                                      
    # SORT by Run2 FOM                                                                                                                                                                                                               
    # -------------------------                                                                                                                                                                                                      
    results.sort(key=lambda x: x["f_run"], reverse=True)

    # -------------------------                                                                                                                                                                                                      
    # PRINT                                                                                                                                                                                                                          
    # -------------------------                                                                                                                                                                                                      
    for r in results:

        status = ""
        if r["pass_run2"]:
            status = " ----> PASS STAT REQ"
        elif r["pass_per"]:
            status = " ----> PASS PER-ERA ONLY"

        print(
            "REFERENCE RESULT Predefined | "
            f"Low {r['low']} | High {r['high']} | "
            f"BkgLow {r['bkg_low']} | BkgHigh {r['bkg_high']} | "
            f"FOM_EraCombined {round(r['f_per'],2)} | "
            f"FOM_Run2 {round(r['f_run'],2)}"
            + status
        )

    best = results[0]
    return best["f_run"], (best["low"], best["high"])

def scan_predefined_binnings(bins_low, bins_high,
                            bins_low_per_era, bins_high_per_era,
                             sig_cache, mode, all_low, fake_low_per_era, fake_high_per_era):

    print(f"\n[REFERENCE] Predefined ({mode})")

    boundaries = [1.0,1.5,2.0,2.5,3.0,3.5,4.0,5.0,7.5]

    best = (-1, None)

    for b1 in boundaries:
        for b2 in boundaries:
            if b1 >= b2:
                continue

            for b3 in boundaries:
                for b4 in boundaries:
                    if b3 >= b4:
                        continue

                    # independent LOW and HIGH binning                                                                                                                                                                               
                    low  = [0, b1, b2, 10]
                    high = [0, b3, b4, 10]

                    r_low  = worker((low,  bins_low,  sig_cache, mode, all_low, fake_low_per_era))
                    r_high = worker((high, bins_high, sig_cache, mode, all_low, fake_high_per_era))

                    if not r_low or not r_high:
                        continue

                    # combine LOW + HIGH FOM                                                                                                                                                                                         
                    f = math.sqrt(r_low[0]**2 + r_high[0]**2)

                    if f > best[0]:
                        best = (f, (low, high))

    return best



def print_sr2_ref_bin_details(data, ref_results, flav, mass):

    import numpy as np
    import math

    print("\n========================================")
    print(f"REF BIN DETAILS | {flav} | mass={mass}")
    print("========================================")

    for r in ref_results:

        if r["flav"] != flav or r["mass"] != mass:
            continue

        regions = r["regions"]

        for region in ["low", "high"]:

            print(f"\n--- {region.upper()} ---")

            edges = regions[region]["bins"]
            sub = data[region]

            edges_full = np.array(sub["edges"])
            bin_lo = edges_full[:-1]

            for i in range(len(edges) - 1):

                lo = edges[i]
                hi = edges[i + 1]

                if i == len(edges) - 2:
                    mask = (bin_lo >= lo) & (bin_lo <= hi)
                else:
                    mask = (bin_lo >= lo) & (bin_lo < hi)

                S_tot = 0.0
                B_tot = 0.0
                E_tot = 0.0

                for era in ERAS:

                    S = sub["signal_norm_flav"][flav][mass][era][mask].sum()
                    B = sub["background"][flav][era][mask].sum()
                    F = sub["fake"][flav][era][mask].sum()
                    E = sub["bkg_err2"][flav][era][mask].sum()

                    F, B = fix_fake_and_bkg(
                        F, B, FAKE_FLOOR,
                        flavour=flav,
                        era=era
                    )

                    S_tot += S
                    B_tot += B
                    E_tot += E

                if B_tot > 0:
                    Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot)
                    rel = math.sqrt(E_tot) / B_tot
                else:
                    Z = 0.0
                    rel = 0.0

                print(f"[{lo:5.2f},{hi:5.2f}] "
                      f"S={S_tot:8.3f} "
                      f"B={B_tot:8.3f} "
                      f"rel={rel:6.3f} "
                      f"Z={Z:6.3f}")
