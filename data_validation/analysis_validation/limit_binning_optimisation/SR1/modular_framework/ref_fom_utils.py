# =========================================================
# EVALUATORs
# =========================================================

from ref_bins import REF_BINS,REF_BINS
from default_config import ERAS, FLAVOURS, FAKE_FLOOR
from logger import fmt_ratio
from helper import fix_fake_and_bkg,compute_bin_Z
import math

def compare_fake_impact(results_no_fake, results_fake):

    print("\n==============================")
    print(" FAKE CORRECTION IMPACT (Run2)")
    print("==============================")

    print("Flavour   Mass    Run2_noFake   Run2_fake   Ratio")
    print("-------------------------------------------------------")

    # Convert to dict for easy lookup
    dict_no = {(r["flav"], r["mass"]): r["run2"] for r in results_no_fake}
    dict_fx = {(r["flav"], r["mass"]): r["run2"] for r in results_fake}

    for key in sorted(dict_no.keys()):

        flav, mass = key

        if key not in dict_fx:
            continue

        run2_no = dict_no[key]
        run2_fx = dict_fx[key]

        ratio = run2_fx / run2_no if run2_no > 0 else 0.0

        print(f"{flav:7s} {mass:6s} {run2_no:12.4f} {run2_fx:11.4f} {ratio:8.4f}")
        

def evaluate_ref_bins_fakecorr(data,DEBUG=False):

    print("\n==============================")
    print(" REF BIN EVALUATION (FAKE CORR)")
    print("==============================")
    
    results = []
    corrected_bins = []
    
    edges = data["edges"]
    bin_lo = edges[:-1]
    bin_hi = edges[1:]

    
    for flav in FLAVOURS:

        print(f"\n================ {flav} =================")

        for mass in data["signal_combined_mass"][flav].keys():

            # ----------------------------------
            # Use 900 bins for high masses
            # ----------------------------------
            ref_mass = mass
            if int(mass) > 1000:
                ref_mass = "1000"

            print(f"\n--- Mass {mass} (using REF {ref_mass} mass={mass}) ---")

            quad_total = 0.0

            S_run2 = None
            B_run2 = None

            # ----------------------------------
            # Loop eras
            # ----------------------------------
            bin_stats_per_era = {era: [] for era in ERAS}
            
            for era in ERAS:

                if ref_mass not in REF_BINS[era][flav]:
                    print(f"[SKIP] Missing bins for {era} {flav} {ref_mass}")
                    continue

                edges_ref = REF_BINS[era][flav][ref_mass]

                print(f"\n[{era}]")
                print("Edges:", edges_ref)

                print(" Bin |     S       B       Z")
                print("--------------------------------")

                S_arr = data["signal"][flav][mass][era]
                B_arr = data["background"][flav][era]
                F_arr = data["fake"][flav][era]
                E_arr = data["bkg_err2"][flav][era]
                Z2_era = 0.0

                # ----------------------------------
                # Loop bins
                # ----------------------------------
                for i in range(len(edges_ref) - 1):

                    lo = edges_ref[i]
                    hi = edges_ref[i+1]

                    mask = (bin_lo >= lo) & (bin_lo < hi)

                    S = S_arr[mask].sum()
                    B = B_arr[mask].sum()
                    F = F_arr[mask].sum()
                    E = E_arr[mask].sum()
                    
                    # Apply fake correction
                    F_orig = F
                    B_orig = B

                    F, B = fix_fake_and_bkg(F, B, FAKE_FLOOR, era=era, flavour=flav)

                    bin_stats_per_era[era].append({
                        "S": S,
                        "B": B,
                        "E": E
                    })
                    
                    # Record only if something changed
                    if (abs(F - F_orig) > 1e-6) or (abs(B - B_orig) > 1e-6):
                        corrected_bins.append({
                            "era": era,
                            "flav": flav,
                            "mass": mass,
                            "bin": i,
                            "lo": lo,
                            "hi": hi,
                            "F_before": F_orig,
                            "B_before": B_orig,
                            "F_after": F,
                            "B_after": B,
                        })

                    Z = compute_bin_Z(S, B)
                    Z2_era += Z*Z

                    print(f"{i:4d} | {S:7.4f} {B:7.2f} {Z:7.3f}")

                Z_era = math.sqrt(Z2_era)
                print(f"[{era}] QUAD FOM = {Z_era:.4f}")

                quad_total += Z2_era

                # ----------------------------------
                # Build Run2 arrays
                # ----------------------------------
                if S_run2 is None:
                    S_run2 = S_arr.copy()
                    B_run2 = B_arr.copy()
                    F_run2 = F_arr.copy()
                    E_run2 = E_arr.copy()
                else:
                    S_run2 += S_arr
                    B_run2 += B_arr
                    F_run2 += F_arr
                    E_run2 += E_arr

            quad_total = math.sqrt(quad_total)

            # ----------------------------------
            # Run2 FOM
            # ----------------------------------
            run2_Z2 = 0.0
            bin_stats_run2 = []
            
            if S_run2 is not None:

                edges_ref = REF_BINS[ERAS[-1]][flav].get(ref_mass, None)

                if edges_ref is not None:

                    for i in range(len(edges_ref) - 1):

                        lo = edges_ref[i]
                        hi = edges_ref[i+1]

                        mask = (bin_lo >= lo) & (bin_lo < hi)

                        S = S_run2[mask].sum()
                        B = B_run2[mask].sum()
                        F = F_run2[mask].sum()
                        E = E_run2[mask].sum()
                                                
                                                
                        # Apply fake correction
                        F_orig = F
                        B_orig = B
                        
                        F, B = fix_fake_and_bkg(F, B, FAKE_FLOOR, flavour=flav, era="Run2")

                        bin_stats_run2.append({
                            "S": S,
                            "B": B,
                            "E": E
                        })
                        if (abs(F - F_orig) > 1e-6) or (abs(B - B_orig) > 1e-6):
                            corrected_bins.append({
                                "era": "Run2",
                                "flav": flav,
                                "mass": mass,
                                "bin": i,
                                "lo": lo,
                                "hi": hi,
                                "F_before": F_orig,
                                "B_before": B_orig,
                                "F_after": F,
                                "B_after": B,
                            })
                        
                        if S > 0 and B > 0:
                            run2_Z2 += 2*((S+B)*math.log(1+S/B) - S)

            run2_total = math.sqrt(run2_Z2)

            print("\n[SUMMARY]")
            print(f"QUAD FOM (all eras) = {quad_total:.4f}")
            print(f"Run2 FOM = {run2_total:.4f}")

            ratio = run2_total / quad_total if quad_total > 0 else 0.0

            n_bins = len(edges_ref) - 1
            results.append({
                "flav": flav,
                "mass": mass,
                "nbins": n_bins,
                "quad": quad_total,
                "run2": run2_total,
                "ratio": ratio,
                "bin_stats": {
                    "Run2": bin_stats_run2,
                    "per_era": bin_stats_per_era
                },
                "binning": {
                    "Run2": None,
                    "per_era": {
                        era: REF_BINS[era][flav][ref_mass]
                        for era in ERAS if ref_mass in REF_BINS[era][flav]
                    }
                }
            })
            
    # =====================================================
    # FINAL TABLE
    # =====================================================
    print("\n==============================")
    print(" FINAL SUMMARY TABLE (FAKE CORR)")
    print("==============================")

    print("Flavour   Mass    QUAD      Run2      Ratio")
    print("------------------------------------------------")

    for r in results:

        flav  = r["flav"]
        mass  = r["mass"]
        nbins = r["nbins"]
        quad  = r["quad"]
        run2  = r["run2"]
        ratio = r["ratio"]
        
        print(f"{flav:7s} {mass:6s} {nbins:6d} {quad:8.4f} {run2:8.4f} {fmt_ratio(ratio):>8}")

    if DEBUG:    
        print("\n==============================")
        print(" CORRECTED BINS SUMMARY")
        print("==============================")
        
        if len(corrected_bins) == 0:
            print("No fake corrections applied.")
        else:
            print("Era   Flav   Mass   Bin   [lo, hi]        F_before  -> F_after    B_before  -> B_after")
            print("--------------------------------------------------------------------------------------")
            
            for c in corrected_bins:
                print(f"{c['era']:5s} {c['flav']:6s} {c['mass']:6s} {c['bin']:4d} "
                      f"[{c['lo']:6.1f},{c['hi']:6.1f}] "
                      f"{c['F_before']:8.3f} -> {c['F_after']:8.3f}   "
                      f"{c['B_before']:8.3f} -> {c['B_after']:8.3f}")
        from collections import defaultdict

        summary = defaultdict(int)
        for c in corrected_bins:
            key = (c["era"], c["flav"])
            summary[key] += 1
            
        print("\n--- Corrections per (era, flavour) ---")
        for (era, flav), n in summary.items():
            print(f"{era:5s} {flav:6s} : {n} bins corrected")
            
    return results


def evaluate_ref_bins_no_fakecorr(data):

    print("\n==============================")
    print(" REF BIN EVALUATION")
    print("==============================")

    results = []

    edges = data["edges"]
    bin_lo = edges[:-1]   # length N                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
    bin_hi = edges[1:]    # length N                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              

    for flav in FLAVOURS:

        print(f"\n================ {flav} =================")

        for mass in data["signal_combined_mass"][flav].keys():

            # ----------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
            # Use 900 bins for high masses                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
            # ----------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
            ref_mass = mass
            if int(mass) > 900:
                ref_mass = "900"

            print(f"\n--- Mass {mass} (using REF {ref_mass}) ---")

            quad_total = 0.0

            S_run2 = None
            B_run2 = None

            # ----------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
            # Loop eras                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
            # ----------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
            for era in ERAS:

                if ref_mass not in REF_BINS[era][flav]:
                    print(f"[SKIP] Missing bins for {era} {flav} {ref_mass}")
                    continue

                edges_ref = REF_BINS[era][flav][ref_mass]

                print(f"\n[{era}]")
                print("Edges:", edges_ref)

                print(" Bin |     S       B       Z")
                print("--------------------------------")

                S_arr = data["signal"][flav][mass][era]
                B_arr = data["background"][flav][era]

                Z2_era = 0.0

                # ----------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
                # Loop bins                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
                # ----------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
                for i in range(len(edges_ref) - 1):

                    lo = edges_ref[i]
                    hi = edges_ref[i+1]

                    mask = (bin_lo >= lo) & (bin_lo < hi)

                    S = S_arr[mask].sum()
                    B = B_arr[mask].sum()

                    Z = compute_bin_Z(S, B)
                    Z2_era += Z*Z

                    print(f"{i:4d} | {S:7.4f} {B:7.2f} {Z:7.3f}")

                Z_era = math.sqrt(Z2_era)
                print(f"[{era}] QUAD FOM = {Z_era:.4f}")

                quad_total += Z2_era

                # ----------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
                # Build Run2 arrays                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
                # ----------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
                if S_run2 is None:
                    S_run2 = S_arr.copy()
                    B_run2 = B_arr.copy()
                else:
                    S_run2 += S_arr
                    B_run2 += B_arr

            quad_total = math.sqrt(quad_total)

            # ----------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
            # Run2 FOM                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
            # ----------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
            run2_Z2 = 0.0

            if S_run2 is not None:

                edges_ref = REF_BINS[ERAS[-1]][flav].get(ref_mass, None)

                if edges_ref is not None:

                    for i in range(len(edges_ref) - 1):

                        lo = edges_ref[i]
                        hi = edges_ref[i+1]

                        mask = (bin_lo >= lo) & (bin_lo < hi)

                        S = S_run2[mask].sum()
                        B = B_run2[mask].sum()

                        if S > 0 and B > 0:
                            run2_Z2 += 2*((S+B)*math.log(1+S/B) - S)

            run2_total = math.sqrt(run2_Z2)

            print("\n[SUMMARY]")
            print(f"QUAD FOM (all eras) = {quad_total:.4f}")
            print(f"Run2 FOM = {run2_total:.4f}")

            ratio = run2_total / quad_total if quad_total > 0 else 0.0
            n_bins = len(edges_ref) - 1
            
            results.append({
                "flav": flav,
                "mass": mass,
                "nbins": n_bins,
                "quad": quad_total,
                "run2": run2_total,
                "ratio": ratio,
                "binning": {
                    "Run2": None,
                    "per_era": {era: REF_BINS[era][flav][ref_mass] for era in ERAS if ref_mass in REF_BINS[era][flav]}
                }
            })


    # =====================================================                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
    # FINAL TABLE                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    # =====================================================                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
    print("\n==============================")
    print(" FINAL SUMMARY TABLE")
    print("==============================")

    print("Flavour   Mass    QUAD      Run2      Ratio")
    print("------------------------------------------------")

    for r in results:

        flav  = r["flav"]
        mass  = r["mass"]
        nbins = r["nbins"]
        quad  = r["quad"]
        run2  = r["run2"]
        ratio = r["ratio"]
        
        print(f"{flav:7s} {mass:6s} {nbins:6d} {quad:8.4f} {run2:8.4f} {fmt_ratio(ratio):>8}")


    return results
