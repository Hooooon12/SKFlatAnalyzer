# =========================================================
# EVALUATORs
# =========================================================

from default_config import ERAS, FLAVOURS, FAKE_FLOOR, MIN_SIGNAL_FRAC,NO_CUMSUM,TEST_COMPARE,MIN_WIDTH
from helper import fix_fake_and_bkg,compute_bin_Z,compute_run2_fom_for_edges,debug_check_path,compute_bin_Z_with_unc
from ref_bins import sr1bins_mm_byMass,sr1bins_ee_byMass,sr1bins_em_byMass

# Default fallback values (will be overridden at runtime)
Bin_NBKG_REQ = 1.0
Bin_NBKG_REQ_Tight = 1.0
Bin_BKG_RelUnc = 0.5

import numpy as np
import math

from tqdm import tqdm


DEBUG = False

def set_stat_config(cfg):
    global Bin_NBKG_REQ, Bin_NBKG_REQ_Tight, Bin_BKG_RelUnc

    Bin_NBKG_REQ = getattr(cfg, "Bin_NBKG_REQ", Bin_NBKG_REQ)
    Bin_NBKG_REQ_Tight = getattr(cfg, "Bin_NBKG_REQ_Tight", Bin_NBKG_REQ_Tight)
    Bin_BKG_RelUnc = getattr(cfg, "Bin_BKG_RelUnc", Bin_BKG_RelUnc)

    print("\n[STAT CONFIG]")
    print(f"  Bin_NBKG_REQ       = {Bin_NBKG_REQ}")
    print(f"  Bin_NBKG_REQ_Tight = {Bin_NBKG_REQ_Tight}")
    print(f"  Bin_BKG_RelUnc     = {Bin_BKG_RelUnc}")
    
def pass_stat_era(B,rel):
    return (B >= 0.15) and  (rel < 0.5)


def pass_stat_and_err(B, rel):
    return (B >= Bin_NBKG_REQ) and (rel < Bin_BKG_RelUnc)

#=============================================
### Scan functions rundp_*
#=============================================



def validate_binning_with_stat(
    edges,
    data,
    flav,
    mass,
    use_fake_corr,
    verbose=False
):
    bin_lo = data["edges"][:-1]

    for i in range(len(edges) - 1):
        lo = edges[i]
        hi = edges[i + 1]

        # inclusive last bin
        if i == len(edges) - 2:
            mask = (bin_lo >= lo) & (bin_lo <= hi)
        else:
            mask = (bin_lo >= lo) & (bin_lo < hi)

        B_run2 = 0.0
        E_run2 = 0.0

        # -----------------------------
        # PER-ERA CHECK (STRICT)
        # -----------------------------
        for era in ERAS:
            B = data["background"][flav][era][mask].sum()
            F = data["fake"][flav][era][mask].sum()
            E = data["bkg_err2"][flav][era][mask].sum()

            if use_fake_corr:
                F, B = fix_fake_and_bkg(
                    F, B, FAKE_FLOOR,
                    flavour=flav,
                    era=era
                )

            rel = math.sqrt(E) / B if B > 0 else float("inf")

            if not pass_stat_era(B, rel):
                if verbose:
                    reasons = []
                    if B < 0.15:
                        reasons.append("lowB")
                    if rel >= 0.5:
                        reasons.append("highRel")

                    print(
                        f"[FAIL][ERA] flav={flav} mass={mass} "
                        f"bin={i} [{lo:.1f}, {hi:.1f}] era={era} "
                        f"B={B:.3f}, rel={rel:.3f} "
                        f"reason={','.join(reasons)}"
                    )
                return False

            B_run2 += B
            E_run2 += E

        # -----------------------------
        # RUN2 CHECK (LOOSER)
        # -----------------------------
        rel_run2 = math.sqrt(E_run2) / B_run2 if B_run2 > 0 else float("inf")

        if not pass_stat_and_err(B_run2, rel_run2):
            if verbose:
                reasons = []
                if B_run2 < Bin_NBKG_REQ:
                    reasons.append("lowB")
                if rel_run2 >= Bin_BKG_RelUnc:
                    reasons.append("highRel")

                print(
                    f"[FAIL][RUN2] flav={flav} mass={mass} "
                    f"bin={i} [{lo:.1f}, {hi:.1f}] "
                    f"B={B_run2:.3f}, rel={rel_run2:.3f} "
                    f"reason={','.join(reasons)}"
                )
            return False

        elif verbose:
            print(
                f"[PASS] flav={flav} mass={mass} "
                f"bin={i} [{lo:.1f}, {hi:.1f}] "
                f"B={B_run2:.3f}, rel={rel_run2:.3f}"
            )

    return True


def compare_fixed_vs_permass(
    data,
    fixed_edges,
    sr1bins_mm_byMass,
    sr1bins_ee_byMass,
    sr1bins_em_byMass,
    use_fake_corr=True,
    run_z_no_unc=True
):

    print("\n==============================================")
    print(" FIXED vs PER-MASS BINNING COMPARISON")
    print("==============================================")
    print(f"[FIXED] {fixed_edges}")
    print("==============================================\n")

    bin_lo = data["edges"][:-1]

    def compute_Z(edges, flav, mass):

        # ---- QUAD ----
        quad_Z2 = 0.0

        for era in ERAS:

            S_arr = data["signal"][flav][mass][era]
            B_arr = data["background"][flav][era]
            F_arr = data["fake"][flav][era]
            E_arr = data["bkg_err2"][flav][era]

            for i in range(len(edges)-1):

                lo, hi = edges[i], edges[i+1]

                if i == len(edges)-2:
                    mask = (bin_lo >= lo) & (bin_lo <= hi)
                else:
                    mask = (bin_lo >= lo) & (bin_lo < hi)

                S = S_arr[mask].sum()
                B = B_arr[mask].sum()
                F = F_arr[mask].sum()
                E = E_arr[mask].sum()

                if use_fake_corr:
                    F, B = fix_fake_and_bkg(F, B, FAKE_FLOOR, flavour=flav, era=era)

                if S > 0 and B > 0:
                    Z = compute_bin_Z_with_unc(S, B, E, run_z_no_unc=run_z_no_unc)
                    quad_Z2 += Z * Z

        quad = math.sqrt(quad_Z2)

        # ---- Run2 ----
        run2_Z2 = 0.0

        for i in range(len(edges)-1):

            lo, hi = edges[i], edges[i+1]

            if i == len(edges)-2:
                mask = (bin_lo >= lo) & (bin_lo <= hi)
            else:
                mask = (bin_lo >= lo) & (bin_lo < hi)

            S_tot = B_tot = E_tot = 0.0

            for era in ERAS:

                S = data["signal"][flav][mass][era][mask].sum()
                B = data["background"][flav][era][mask].sum()
                F = data["fake"][flav][era][mask].sum()
                E = data["bkg_err2"][flav][era][mask].sum()

                if use_fake_corr:
                    F, B = fix_fake_and_bkg(F, B, FAKE_FLOOR, flavour=flav, era=era)

                S_tot += S
                B_tot += B
                E_tot += E

            if S_tot > 0 and B_tot > 0:
                Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot, run_z_no_unc=run_z_no_unc)
                run2_Z2 += Z * Z

        run2 = math.sqrt(run2_Z2)

        return quad, run2


    for flav in FLAVOURS:

        print("\n==============================================")
        print(f"FLAVOUR: {flav}")
        print("==============================================")

        for mass in data["signal_combined_mass"][flav]:

            # ---- pick correct per-mass binning ----
            if flav == "MuMu":
                edges_pm = sr1bins_mm_byMass[mass]
            elif flav == "EE":
                edges_pm = sr1bins_ee_byMass[mass]
            elif flav == "EMu":
                edges_pm = sr1bins_em_byMass[mass]
            else:
                continue

            # ---- compute ----
            quad_pm, run2_pm = compute_Z(edges_pm, flav, mass)
            quad_fx, run2_fx = compute_Z(fixed_edges, flav, mass)

            # ---- ratios ----
            ratio_pm = run2_pm / quad_pm if quad_pm > 0 else 0
            ratio_fx = run2_fx / quad_fx if quad_fx > 0 else 0

            rel_loss = run2_fx / run2_pm if run2_pm > 0 else 0

            # ---- print ----
            print("----------------------------------------")
            print(f"{flav}  Mass={mass}")
            print("----------------------------------------")

            print("[PER-MASS]")
            print(f"Run2  = {run2_pm:.4f}   QUAD = {quad_pm:.4f}   Ratio = {ratio_pm:.4f}")

            print("[FIXED ]")
            print(f"Run2  = {run2_fx:.4f}   QUAD = {quad_fx:.4f}   Ratio = {ratio_fx:.4f}")

            print(f"[LOSS ] Fixed / PerMass = {rel_loss:.4f}")
            print("")


def run_dp_on_arrays_mass_with_weight(
        S_mass_era, B, E, F,
        bin_lo,
        n_bins=6,
        use_fake_corr=True,
        flav=None,
        run_z_no_unc=True,
        mass_weights=None   
):

    N = len(bin_lo)
    if len(bin_lo) < 2:
        raise ValueError("bin_lo must have at least 2 entries")
    bin_hi = np.append(bin_lo[1:], bin_lo[-1] + (bin_lo[1] - bin_lo[0]))

    masses = list(S_mass_era.keys())
    eras = list(B.keys())

    # =========================================
    # BUILD CUMSUM ARRAYS
    # =========================================
    S_cum = {}
    B_cum = {}
    E_cum = {}
    F_cum = {}

    for mass in masses:
        S_cum[mass] = {}
        for era in eras:
            arr = S_mass_era[mass][era]
            tmp = np.zeros(N+1)
            tmp[1:] = np.cumsum(arr)
            S_cum[mass][era] = tmp

    for era in eras:
        for src, dst in [(B, B_cum), (E, E_cum), (F, F_cum)]:
            arr = src[era]
            tmp = np.zeros(N+1)
            tmp[1:] = np.cumsum(arr)
            dst[era] = tmp

    # =========================================
    # DP ARRAYS
    # =========================================
    NEG = -1e300
    dp  = np.full((n_bins+1, N+1), NEG)
    prv = np.full((n_bins+1, N+1), -1)

    dp[0, 0] = 0.0

    # =========================================
    # DP LOOP
    # =========================================
    for j in range(1, n_bins+1):
        for i in range(1, N+1):

            best = NEG
            best_p = -1

            for p in range(i):

                # Skip invalid DP state early
                if dp[j-1, p] <= NEG/2:
                    continue
                
                # Width cut (cheap, do early)
                if (bin_hi[i-1] - bin_lo[p]) < MIN_WIDTH:
                    continue

                Z_bin2 = 0.0

                
                B_int = 0.0
                E_int = 0.0
                # ----------------------------------                                                             
                # LOOP OVER ERAS                                                                                 
                # ----------------------------------                                                             
                good_bin=True
                for era in eras:
                    B_e = B_cum[era][i] - B_cum[era][p]
                    F_e = F_cum[era][i] - F_cum[era][p]
                    E_e = E_cum[era][i] - E_cum[era][p]
                    
                    if use_fake_corr:
                        F_e, B_e = fix_fake_and_bkg(
                            F_e, B_e, FAKE_FLOOR,
                            flavour=flav, era=era
                        )
                        
                    B_int += B_e
                    E_int += E_e
                    rel_e = math.sqrt(E_e) / B_e
                    if not pass_stat_era(B_e, rel_e):
                        good_bin=False
                        break
                
                # ----------------------------------                                                             
                # STAT CHECK                                                                                     
                # ----------------------------------                                                             
                if B_int <= 1e-6:
                    continue
                if not good_bin:
                    continue
                rel = math.sqrt(E_int) / B_int
                
                if not pass_stat_and_err(B_int, rel):
                    continue
                    
                # ==================================                                                        
                # LOOP OVER MASSES                                                                        
                # ==================================
                for mass in masses:
                    S_int = 0.0
                    Z2_mass = 0

                    # ----------------------------------
                    # LOOP OVER ERAS
                    # ----------------------------------
                    for era in eras:

                        S_e = S_cum[mass][era][i] - S_cum[mass][era][p]
                        S_int += S_e
                        

                    # ----------------------------------
                    # Z per mass
                    # ----------------------------------
                    if S_int > 0 and B_int > 0:
                        Z = compute_bin_Z_with_unc(
                            S_int, B_int, E_int,
                            run_z_no_unc=run_z_no_unc
                        )
                        w= mass_weights.get(mass)
                        Z2 = Z * Z *w*w
                        Z_bin2 += Z2

                # Skip empty bins
                if Z_bin2 <= 0:
                    continue

                val = dp[j-1, p] + Z_bin2

                if val > best:
                    best = val
                    best_p = p

            dp[j, i] = best
            prv[j, i] = best_p

    # =========================================
    # BACKTRACK
    # =========================================
    if dp[n_bins, N] <= NEG/2:
        return None, None

    
    idx = []
    j, i = n_bins, N

    while j > 0:
        p = prv[j, i]
        if p < 0:
            return None, None
        idx.append(p)
        i = p
        j -= 1

    idx = list(reversed(idx))

    edges = [bin_lo[0]]
    for k in idx:
        if k != 0:
            edges.append(bin_lo[k])
    edges.append(bin_hi[-1])

    best_Z2 = dp[n_bins, N]
    best_Z  = math.sqrt(best_Z2)
    
    print("\n======================================")
    print("[DP Weighted RESULT]")
    print(f"Edges : {edges}")
    print(f"Best Z^2 = {best_Z2:.6f}")
    print(f"Best Z   = {best_Z:.6f}")
    print("======================================\n")
    return edges, best_Z





def run_dp_on_arrays_mass(
        S_mass_era, B, E, F,
        bin_lo,
        n_bins=6,
        use_fake_corr=True,
        flav=None,
        run_z_no_unc=True,
):

    if NO_CUMSUM:
        print("[DEBUG] Using SLOW DP (no cumsum)")
        return run_dp_on_arrays_mass_slow(
            S_mass_era,
            B,
            E,
            F,
            bin_lo,
            n_bins=n_bins,
            use_fake_corr=use_fake_corr,
            flav=flav,
            run_z_no_unc=run_z_no_unc,
        )
    else:
        print("[DEBUG] Using FAST DP (cumsum)")

        
    N = len(bin_lo)
    if len(bin_lo) < 2:
        raise ValueError("bin_lo must have at least 2 entries")
    bin_hi = np.append(bin_lo[1:], bin_lo[-1] + (bin_lo[1] - bin_lo[0]))

    masses = list(S_mass_era.keys())
    eras = list(B.keys())

    # =========================================
    # BUILD CUMSUM ARRAYS
    # =========================================
    S_cum = {}
    B_cum = {}
    E_cum = {}
    F_cum = {}

    for mass in masses:
        S_cum[mass] = {}
        for era in eras:
            arr = S_mass_era[mass][era]
            tmp = np.zeros(N+1)
            tmp[1:] = np.cumsum(arr)
            S_cum[mass][era] = tmp

    for era in eras:
        for src, dst in [(B, B_cum), (E, E_cum), (F, F_cum)]:
            arr = src[era]
            tmp = np.zeros(N+1)
            tmp[1:] = np.cumsum(arr)
            dst[era] = tmp

    # =========================================
    # DP ARRAYS
    # =========================================
    NEG = -1e300
    dp  = np.full((n_bins+1, N+1), NEG)
    prv = np.full((n_bins+1, N+1), -1)

    dp[0, 0] = 0.0

    # =========================================
    # DP LOOP
    # =========================================
    for j in range(1, n_bins+1):
        for i in range(1, N+1):

            best = NEG
            best_p = -1

            for p in range(i):

                # Skip invalid DP state early
                if dp[j-1, p] <= NEG/2:
                    continue
                
                # Width cut (cheap, do early)
                if (bin_hi[i-1] - bin_lo[p]) < MIN_WIDTH:
                    continue

                Z_bin2 = 0.0

                
                B_int = 0.0
                E_int = 0.0
                # ----------------------------------                                                             
                # LOOP OVER ERAS                                                                                 
                # ----------------------------------                                                             
                good_bin=True
                for era in eras:
                    B_e = B_cum[era][i] - B_cum[era][p]
                    F_e = F_cum[era][i] - F_cum[era][p]
                    E_e = E_cum[era][i] - E_cum[era][p]
                    
                    if use_fake_corr:
                        F_e, B_e = fix_fake_and_bkg(
                            F_e, B_e, FAKE_FLOOR,
                            flavour=flav, era=era
                        )
                        
                    B_int += B_e
                    E_int += E_e
                    rel_e = math.sqrt(E_e) / B_e
                    
                    if not pass_stat_era(B_e, rel_e):
                        good_bin=False
                        break

                # ----------------------------------                                                             
                # STAT CHECK                                                                                     
                # ----------------------------------                                                             
                if B_int <= 1e-6:
                    continue
                if not good_bin:
                    continue
                rel = math.sqrt(E_int) / B_int
                
                if not pass_stat_and_err(B_int, rel):
                    continue
                    
                # ==================================                                                        
                # LOOP OVER MASSES                                                                        
                # ==================================
                for mass in masses:
                    S_int = 0.0
                    # ----------------------------------
                    # LOOP OVER ERAS
                    # ----------------------------------
                    for era in eras:

                        S_e = S_cum[mass][era][i] - S_cum[mass][era][p]
                        S_int += S_e
                        

                    # ----------------------------------
                    # Z per mass
                    # ----------------------------------
                    if S_int > 0 and B_int > 0:
                        Z = compute_bin_Z_with_unc(
                            S_int, B_int, E_int,
                            run_z_no_unc=run_z_no_unc
                        )
                        Z_bin2 += Z * Z

                # Skip empty bins
                if Z_bin2 <= 0:
                    continue

                val = dp[j-1, p] + Z_bin2

                if val > best:
                    best = val
                    best_p = p

            dp[j, i] = best
            prv[j, i] = best_p

    # =========================================
    # BACKTRACK
    # =========================================
    if dp[n_bins, N] <= NEG/2:
        return None, None

    
    idx = []
    j, i = n_bins, N

    while j > 0:
        p = prv[j, i]
        if p < 0:
            return None, None
        idx.append(p)
        i = p
        j -= 1

    idx = list(reversed(idx))

    edges = [bin_lo[0]]
    for k in idx:
        if k != 0:
            edges.append(bin_lo[k])
    edges.append(bin_hi[-1])

    best_Z2 = dp[n_bins, N]
    best_Z  = math.sqrt(best_Z2)

    print("\n======================================")
    print("[DP RESULT]")
    print(f"Edges : {edges}")
    print(f"Best Z^2 = {best_Z2:.6f}")
    print(f"Best Z   = {best_Z:.6f}")
    print("======================================\n")
    return edges, best_Z


def run_dp_on_arrays_mass_slow(
        S_mass_era, B, E, F,
        bin_lo,
        n_bins=6,
        use_fake_corr=True,
        flav=None,
        run_z_no_unc=True,
):

    N = len(bin_lo)
    if len(bin_lo) < 2:
        raise ValueError("bin_lo must have at least 2 entries")
    bin_hi = np.append(bin_lo[1:], bin_lo[-1] + (bin_lo[1] - bin_lo[0]))

    masses = list(S_mass_era.keys())

    NEG = -1e300
    dp  = np.full((n_bins+1, N+1), NEG)
    prv = np.full((n_bins+1, N+1), -1)

    dp[0,0] = 0.0

    for j in range(1, n_bins+1):
        for i in range(1, N+1):

            best = NEG
            best_p = -1

            for p in range(i):

                Z_bin2 = 0.0

                for mass in masses:

                    S_int = 0.0
                    B_int = 0.0
                    E_int = 0.0

                    for era in S_mass_era[mass]:

                        S_e = S_mass_era[mass][era][p:i].sum()
                        B_e = B[era][p:i].sum()
                        F_e = F[era][p:i].sum()
                        E_e = E[era][p:i].sum()

                        if use_fake_corr:
                            F_e, B_e = fix_fake_and_bkg(
                                F_e, B_e, FAKE_FLOOR,
                                flavour=flav, era=era
                            )

                        S_int += S_e
                        B_int += B_e
                        E_int += E_e

                    if S_int > 0 and B_int > 0:
                        Z = compute_bin_Z_with_unc(
                            S_int, B_int, E_int,
                            run_z_no_unc=run_z_no_unc
                        )
                        Z_bin2 += Z * Z

                if Z_bin2 <= 0:
                    continue

                val = dp[j-1, p] + Z_bin2

                if val > best:
                    best = val
                    best_p = p

            dp[j, i] = best
            prv[j, i] = best_p

    if dp[n_bins, N] <= NEG/2:
        return None, None

    idx = []
    j, i = n_bins, N

    while j > 0:
        p = prv[j, i]
        if p < 0:
            return None, None
        idx.append(p)
        i = p
        j -= 1

    idx = list(reversed(idx))

    edges = [bin_lo[0]]
    for k in idx:
        if k != 0:
            edges.append(bin_lo[k])
    edges.append(bin_hi[-1])


    best_Z2 = dp[n_bins, N]
    best_Z  = math.sqrt(best_Z2)

    return edges, best_Z

def run_dp_on_arrays_with_flav_stat(
        bin_lo,
        n_bins=6,
        use_fake_corr=True,
        S_flav_era=None,
        B_flav_era=None,
        E_flav_era=None,
        F_flav_era=None,
        run_z_no_unc=True,
):

    if NO_CUMSUM:
        return run_dp_on_arrays_with_flav_stat_slow( bin_lo,
	                                             n_bins=n_bins,
                                                     use_fake_corr=use_fake_corr,
	                                             S_flav_era=S_flav_era,
                                                     B_flav_era=B_flav_era,
                                                     E_flav_era=E_flav_era,
                                                     F_flav_era=F_flav_era,
	                                             run_z_no_unc=run_z_no_unc
                                                     )

    else:
        print("[DEBUG] Using FAST DP (cumsum)")
        
    
    nbins = len(bin_lo)

    # =====================================================
    # PRECOMPUTE CUMSUMS
    # =====================================================
    S_cum = {}
    B_cum = {}
    E_cum = {}
    F_cum = {}

    for f in FLAVOURS:
        S_cum[f] = {}
        B_cum[f] = {}
        E_cum[f] = {}
        F_cum[f] = {}

        for era in S_flav_era[f]:
            S_cum[f][era] = np.concatenate(([0], np.cumsum(S_flav_era[f][era])))
            B_cum[f][era] = np.concatenate(([0], np.cumsum(B_flav_era[f][era])))
            E_cum[f][era] = np.concatenate(([0], np.cumsum(E_flav_era[f][era])))
            F_cum[f][era] = np.concatenate(([0], np.cumsum(F_flav_era[f][era])))

    # =====================================================
    # DP SETUP
    # =====================================================
    dp = np.full((nbins + 1, n_bins + 1), -np.inf)
    prev = [[-1]*(n_bins+1) for _ in range(nbins+1)]

    dp[0][0] = 0.0

    # =====================================================
    # DP LOOP
    # =====================================================
    for i in range(nbins):
        for k in range(n_bins):

            if dp[i][k] == -np.inf:
                continue

            for j in range(i+1, nbins+1):

                valid = True

                # =========================================
                # STAT CHECK
                # =========================================
                for f in FLAVOURS:

                    B_tot = 0.0
                    E_tot = 0.0
                    good_bin=True
                    for era in S_cum[f]:

                        # FAST RANGE SUM
                        b_e = B_cum[f][era][j] - B_cum[f][era][i]
                        f_e = F_cum[f][era][j] - F_cum[f][era][i]
                        e_e = E_cum[f][era][j] - E_cum[f][era][i]

                        if use_fake_corr:
                            f_e, b_e = fix_fake_and_bkg(
                                f_e, b_e, FAKE_FLOOR,
                                flavour=f, era=era
                            )

                        B_tot += b_e
                        E_tot += e_e

                        rel_e = math.sqrt(e_e) / b_e
                        if not pass_stat_and_err(b_e, rel_e):
                            good_bin=False
                            break
                        
                    if B_tot < 1e-6:
                        valid = False
                        break
                    if not good_bin:
                        valid = False
                        break

                    rel = math.sqrt(E_tot) / B_tot

                    if not pass_stat_and_err(B_tot, rel):
                        valid = False
                        break

                if not valid:
                    continue

                # =========================================
                # Z CALCULATION
                # =========================================
                Z_bin = 0.0

                for f in FLAVOURS:

                    S_f = 0.0
                    B_f = 0.0
                    E_f = 0.0

                    for era in S_cum[f]:

                        s_e = S_cum[f][era][j] - S_cum[f][era][i]
                        b_e = B_cum[f][era][j] - B_cum[f][era][i]
                        f_e = F_cum[f][era][j] - F_cum[f][era][i]
                        e_e = E_cum[f][era][j] - E_cum[f][era][i]

                        if use_fake_corr:
                            f_e, b_e = fix_fake_and_bkg(
                                f_e, b_e, FAKE_FLOOR,
                                flavour=f, era=era
                            )

                        S_f += s_e
                        B_f += b_e
                        E_f += e_e

                    if S_f > 0 and B_f > 0:
                        Z_f = compute_bin_Z_with_unc(
                            S_f, B_f, E_f,
                            run_z_no_unc=run_z_no_unc
                        )
                        Z_bin += Z_f * Z_f

                # =========================================
                # DP UPDATE
                # =========================================
                if dp[i][k] + Z_bin > dp[j][k+1]:
                    dp[j][k+1] = dp[i][k] + Z_bin
                    prev[j][k+1] = i

    # =====================================================
    # FIND BEST
    # =====================================================
    best_score = -np.inf
    best_k = -1

    for k in range(1, n_bins+1):
        if dp[nbins][k] > best_score:
            best_score = dp[nbins][k]
            best_k = k

    if best_k == -1:
        return None, None

    # =====================================================
    # BACKTRACK
    # =====================================================
    edges_idx = []
    j = nbins
    k = best_k

    while k > 0:
        i = prev[j][k]
        edges_idx.append(j)
        j = i
        k -= 1

    edges_idx.append(0)
    edges_idx = sorted(edges_idx)

    # =====================================================
    # BUILD EDGES
    # =====================================================
    if len(bin_lo) < 2:
        raise ValueError("bin_lo must have at least 2 entries")
    bin_hi = np.append(bin_lo[1:], bin_lo[-1] + (bin_lo[1] - bin_lo[0]))

    edges = [bin_lo[0]]

    for idx in edges_idx[1:-1]:
        edges.append(bin_lo[idx])

    edges.append(bin_hi[-1])

    # =====================================================
    # FINAL Z
    # =====================================================
    best_Z2 = dp[nbins][best_k]
    best_Z  = math.sqrt(best_Z2)

    return edges, best_Z




def run_dp_on_arrays_with_flav_stat_slow(
        bin_lo,
        n_bins=6,
        use_fake_corr=True,
        S_flav_era=None,
        B_flav_era=None,
        E_flav_era=None,
        F_flav_era=None,
        run_z_no_unc=True,
        
):

    nbins = len(bin_lo)

    dp = np.full((nbins + 1, n_bins + 1), -np.inf)
    prev = [[-1]*(n_bins+1) for _ in range(nbins+1)]

    dp[0][0] = 0.0

    for i in range(nbins):
        for k in range(n_bins):

            if dp[i][k] == -np.inf:
                continue

            for j in range(i+1, nbins+1):

                mask = slice(i, j)

                valid = True

                # =========================================
                # STAT CHECK (PER FLAVOUR, CORRECTED FAKE)
                # =========================================
                for f in FLAVOURS:

                    B_tot = 0.0
                    E_tot = 0.0

                    good_bin=True
                    for era in ERAS:

                        B_arr = B_flav_era[f][era]
                        F_arr = F_flav_era[f][era]
                        E_arr = E_flav_era[f][era]

                        b_e = B_arr[mask].sum()
                        f_e = F_arr[mask].sum()
                        e_e = E_arr[mask].sum()

                        if use_fake_corr:
                            f_e, b_e = fix_fake_and_bkg(
                                f_e, b_e, FAKE_FLOOR,
                                flavour=f, era=era
                            )

                        rel_e = math.sqrt(e_e) / b_e

                        if not pass_stat_and_err(b_e, rel_e):
                            good_bin=False
                            break

                        B_tot += b_e
                        E_tot += e_e

                    if B_tot < 1e-6:
                        valid = False
                        break

                    rel = math.sqrt(E_tot) / B_tot

                    if not good_bin:
                        valid = False
                        break
                    
                    if not pass_stat_and_err(B_tot, rel):
                        valid = False
                        break

                if not valid:
                    continue

                # =========================================
                # Z CALCULATION (PER FLAVOUR, SUMMED)
                # =========================================
                Z_bin = 0.0

                for f in FLAVOURS:

                    S_f = 0.0
                    B_f = 0.0
                    E_f = 0.0

                    for era in ERAS:

                        S_arr = S_flav_era[f][era]
                        B_arr = B_flav_era[f][era]
                        F_arr = F_flav_era[f][era]
                        E_arr = E_flav_era[f][era]

                        s_e = S_arr[mask].sum()
                        b_e = B_arr[mask].sum()
                        f_e = F_arr[mask].sum()
                        e_e = E_arr[mask].sum()

                        if use_fake_corr:
                            f_e, b_e = fix_fake_and_bkg(
                                f_e, b_e, FAKE_FLOOR,
                                flavour=f, era=era
                            )

                        S_f += s_e
                        B_f += b_e
                        E_f += e_e

                    if S_f > 0 and B_f > 0:
                        Z_f = compute_bin_Z_with_unc(S_f, B_f, E_f,run_z_no_unc=run_z_no_unc)
                        Z_bin += Z_f*Z_f

                if dp[i][k] + Z_bin > dp[j][k+1]:
                    dp[j][k+1] = dp[i][k] + Z_bin
                    prev[j][k+1] = i

    # =========================
    # FIND BEST SOLUTION
    # =========================
    best_score = -np.inf
    best_k = -1

    for k in range(1, n_bins+1):
        if dp[nbins][k] > best_score:
            best_score = dp[nbins][k]
            best_k = k

    if best_k == -1:
        return None, None

    # =========================
    # BACKTRACK
    # =========================
    edges_idx = []
    j = nbins
    k = best_k

    while k > 0:
        i = prev[j][k]
        edges_idx.append(j)
        j = i
        k -= 1

    edges_idx.append(0)
    edges_idx = sorted(edges_idx)

    # =========================
    # BUILD EDGES
    # =========================
    if len(bin_lo) < 2:
        raise ValueError("bin_lo must have at least 2 entries")
    bin_hi = np.append(bin_lo[1:], bin_lo[-1] + (bin_lo[1] - bin_lo[0]))

    edges = [bin_lo[0]]

    for idx in edges_idx[1:-1]:
        edges.append(bin_lo[idx])

    edges.append(bin_hi[-1])

    best_Z2 = dp[nbins][best_k]
    best_Z  = math.sqrt(best_Z2)

    return edges, best_Z


def run_dp_on_arrays(
        S, B, E, F,
        bin_lo,
        n_bins=6,
        use_fake_corr=True,
        flav=None,
        use_low_edge_cut=True,
        run_z_no_unc=True,
):


    if NO_CUMSUM:
        return run_dp_on_arrays_slow(
	    S,
            B,
            E,
            F,
            bin_lo,
	    n_bins=n_bins,
            use_fake_corr=use_fake_corr,
            flav=flav,
            use_low_edge_cut=use_low_edge_cut,
            run_z_no_unc=run_z_no_unc,
        )
    else:
        print("[DEBUG] Using FAST DP (cumsum)")

        
    N = len(bin_lo)

    # ----------------------------------
    # Build bin_hi
    # ----------------------------------
    if len(bin_lo) < 2:
        raise ValueError("bin_lo must have at least 2 entries")
    bin_hi = np.append(bin_lo[1:], bin_lo[-1] + (bin_lo[1] - bin_lo[0]))

    # =====================================================
    # PRECOMPUTE CUMSUMS
    # =====================================================
    S_cum = {}
    B_cum = {}
    E_cum = {}
    F_cum = {}

    for era in S:
        S_cum[era] = np.concatenate(([0], np.cumsum(S[era])))
        B_cum[era] = np.concatenate(([0], np.cumsum(B[era])))
        E_cum[era] = np.concatenate(([0], np.cumsum(E[era])))
        F_cum[era] = np.concatenate(([0], np.cumsum(F[era])))

    # ----------------------------------
    # LOW-EDGE SIGNAL FRACTION CUT
    # ----------------------------------
    S_total = sum(arr.sum() for arr in S.values())

    min_edge_idx = 0
    if use_low_edge_cut:
        running = 0.0
        for i in range(N):
            running += sum(arr[i] for arr in S.values())
            if S_total > 0 and (running / S_total) > MIN_SIGNAL_FRAC:
                min_edge_idx = i
                break
        if bin_lo[min_edge_idx] > 500:
            print(f"[INFO] Capping min edge from {bin_lo[min_edge_idx]:.1f} -> 500")
            
            for i in range(len(bin_lo)):
                if bin_lo[i] >= 500:
                    min_edge_idx = i
                    break

    else:
        min_edge_idx = 0

    # ----------------------------------
    # HIGH-EDGE STAT CUT (PER ERA FIX)
    # ----------------------------------
    max_edge_idx = N

    for i in reversed(range(N)):

        B_tail = 0.0
        E_tail = 0.0

        for era in S:

            # FAST tail sums
            B_e = B_cum[era][-1] - B_cum[era][i]
            F_e = F_cum[era][-1] - F_cum[era][i]
            E_e = E_cum[era][-1] - E_cum[era][i]

            if use_fake_corr:
                F_e, B_e = fix_fake_and_bkg(
                    F_e, B_e, FAKE_FLOOR,
                    flavour=flav, era=era
                )

            B_tail += B_e
            E_tail += E_e

        if B_tail <= 1e-6:
            continue

        rel_tail = math.sqrt(E_tail) / B_tail

        if pass_stat_and_err(B_tail, rel_tail):
            max_edge_idx = i
            break
    upper_edge = bin_hi[-1]
    max_edge_val = bin_hi[-1] if max_edge_idx == N else bin_lo[max_edge_idx]

    print(
        f"[SCAN WINDOW] "
        f"[{bin_lo[0]:.1f}, {upper_edge:.1f}] --> "
        f"[{bin_lo[min_edge_idx]:.1f}, {max_edge_val:.1f}]"
    )

    print(
        f"[SCAN REDUCTION] "
        f"{len(bin_lo)} bins -> {max_edge_idx - min_edge_idx} bins"
    )
    
    if DEBUG:
        print("\n[DEBUG WINDOW DETAILS]")
        print(f"min_edge_idx = {min_edge_idx} (edge = {bin_lo[min_edge_idx]:.1f})")
        print(f"max_edge_idx = {max_edge_idx} (edge = {bin_lo[max_edge_idx]:.1f})")
        
        print("\n[TAIL BACKGROUND CHECK]")
        for i in range(len(bin_lo)):
            B_tail = sum(B_cum[era][-1] - B_cum[era][i] for era in B_cum)
            print(f"edge {bin_lo[i]:.1f} -> tail B = {B_tail:.4f}")

    # ----------------------------------
    # DP arrays
    # ----------------------------------
    NEG = -1e300
    dp  = np.full((n_bins+1, N+1), NEG)
    prv = np.full((n_bins+1, N+1), -1)

    dp[0,0] = 0.0

    # ----------------------------------
    # DP loop
    # ----------------------------------
    for j in range(1, n_bins+1):
        for i in range(1, N+1):

            best = NEG
            best_p = -1

            for p in range(i):

                # ----------------------------------
                # Compute interval (FAST via cumsum)
                # ----------------------------------
                S_int = 0.0
                B_int = 0.0
                E_int = 0.0
                good_bin=True
                for era in S:

                    S_e = S_cum[era][i] - S_cum[era][p]
                    B_e = B_cum[era][i] - B_cum[era][p]
                    F_e = F_cum[era][i] - F_cum[era][p]
                    E_e = E_cum[era][i] - E_cum[era][p]

                    if use_fake_corr:
                        F_e, B_e = fix_fake_and_bkg(
                            F_e, B_e, FAKE_FLOOR,
                            flavour=flav, era=era
                        )

                    rel_e = math.sqrt(E_e) / B_e if B_e > 0 else 999.0

                    if not pass_stat_era(B_e, rel_e):
                        good_bin=False
                        break

                        
                    S_int += S_e
                    B_int += B_e
                    E_int += E_e

                if B_int <= 1e-6:
                    continue
                if not good_bin:
                    continue
                # ----------------------------------
                # Width cut
                # ----------------------------------
                if (bin_hi[i-1] - bin_lo[p]) < MIN_WIDTH:
                    continue

                rel = math.sqrt(E_int) / B_int if B_int > 0 else 999.0

                if not pass_stat_and_err(B_int, rel):
                    continue

                # ----------------------------------
                # LOW EDGE
                # ----------------------------------
                if j == 1 and i <= min_edge_idx:
                    continue

                # ----------------------------------
                # HIGH EDGE
                # ----------------------------------
                if j == n_bins and p > max_edge_idx:
                    continue

                # ----------------------------------
                # DP validity
                # ----------------------------------
                if dp[j-1, p] <= NEG/2:
                    continue

                # ----------------------------------
                # Z calculation
                # ----------------------------------
                Z = compute_bin_Z_with_unc(
                    S_int, B_int, E_int,
                    run_z_no_unc=run_z_no_unc
                )
                z2 = Z * Z

                val = dp[j-1, p] + z2

                if val > best:
                    best = val
                    best_p = p

            dp[j, i] = best
            prv[j, i] = best_p

    # ----------------------------------
    # Check solution
    # ----------------------------------
    if dp[n_bins, N] <= NEG/2:
        tqdm.write("[DP WARNING] No valid binning found")
        return None,None

    # ----------------------------------
    # Backtrack
    # ----------------------------------
    idx = []
    j, i = n_bins, N

    while j > 0:
        p = prv[j, i]
        if p < 0:
            return None, None
        idx.append(p)
        i = p
        j -= 1

    idx = list(reversed(idx))

    # ----------------------------------
    # Build edges
    # ----------------------------------
    edges = [bin_lo[0]]

    for k in idx:
        if k != 0:
            edges.append(bin_lo[k])

    edges.append(bin_hi[-1])

    best_Z2 = dp[n_bins, N]
    best_Z  = math.sqrt(best_Z2)

    #### TEST edge
    #test_edges = [0.0, 1375.0, 1795.0, 5000.0]
    
    #debug_check_path(dp, prv, test_edges, bin_lo)
    #debug_check_path(dp, prv, edges, bin_lo)
    
    return edges, best_Z

def run_dp_on_arrays_slow(
        S, B, E, F,
        bin_lo,
        n_bins=6,
        use_fake_corr=True,
        flav=None,
        use_low_edge_cut=True,
        run_z_no_unc=True,
):

    N = len(bin_lo)

    # ----------------------------------
    # Build bin_hi
    # ----------------------------------
    if len(bin_lo) < 2:
        raise ValueError("bin_lo must have at least 2 entries")
    bin_hi = np.append(bin_lo[1:], bin_lo[-1] + (bin_lo[1] - bin_lo[0]))

    # ----------------------------------
    # LOW-EDGE SIGNAL FRACTION CUT
    # ----------------------------------
    S_total = sum(S[era].sum() for era in S)
    min_edge_idx = 0
    if use_low_edge_cut:
        running = 0.0
        for i in range(N):
            running += sum(S[era][i] for era in S)
            if S_total > 0 and (running / S_total) > MIN_SIGNAL_FRAC:
                min_edge_idx = i
                break

    # ----------------------------------
    # HIGH-EDGE STAT CUT (PER ERA FIX)
    # ----------------------------------
    max_edge_idx = N

    for i in reversed(range(N)):

        B_tail = 0.0
        E_tail = 0.0

        for era in S:

            B_e = B[era][i:].sum()
            F_e = F[era][i:].sum()
            E_e = E[era][i:].sum()

            if use_fake_corr:
                F_e, B_e = fix_fake_and_bkg(
                    F_e, B_e, FAKE_FLOOR,
                    flavour=flav, era=era
                )

            B_tail += B_e
            E_tail += E_e

        if B_tail <= 1e-6:
            continue

        rel_tail = math.sqrt(E_tail) / B_tail

        if pass_stat_and_err(B_tail, rel_tail):
            max_edge_idx = i
            break

    # ----------------------------------
    # DP arrays
    # ----------------------------------
    NEG = -1e300
    dp  = np.full((n_bins+1, N+1), NEG)
    prv = np.full((n_bins+1, N+1), -1)

    dp[0,0] = 0.0

    # ----------------------------------
    # DP loop
    # ----------------------------------
    for j in range(1, n_bins+1):
        for i in range(1, N+1):

            best = NEG
            best_p = -1

            for p in range(i):

                # ----------------------------------
                # Compute interval (PER ERA FIX)
                # ----------------------------------
                S_int = 0.0
                B_int = 0.0
                E_int = 0.0

                good_bin=True
                for era in S:

                    S_e = S[era][p:i].sum()
                    B_e = B[era][p:i].sum()
                    F_e = F[era][p:i].sum()
                    E_e = E[era][p:i].sum()

                    if use_fake_corr:
                        F_e, B_e = fix_fake_and_bkg(
                            F_e, B_e, FAKE_FLOOR,
                            flavour=flav, era=era
                        )

                    rel_e = math.sqrt(E_e) / B_e

                    if not pass_stat_era(B_e, rel_e):
                        good_bin=False
                        break

                    S_int += S_e
                    B_int += B_e
                    E_int += E_e

                if B_int <= 1e-6:
                    continue
                if not good_bin:
                    continue
                # ----------------------------------
                # Width cut
                # ----------------------------------
                if (bin_hi[i-1] - bin_lo[p]) < MIN_WIDTH:
                    continue

                rel = math.sqrt(E_int) / B_int

                if not pass_stat_and_err(B_int, rel):
                    continue

                # ----------------------------------
                # LOW EDGE
                # ----------------------------------
                if j == 1 and i <= min_edge_idx:
                    continue

                # ----------------------------------
                # HIGH EDGE
                # ----------------------------------
                if j == n_bins and p > max_edge_idx:
                    continue

                # ----------------------------------
                # DP validity
                # ----------------------------------
                if dp[j-1, p] <= NEG/2:
                    continue

                # ----------------------------------
                # Z calculation (NO DUPLICATION)
                # ----------------------------------
                Z = compute_bin_Z_with_unc(S_int, B_int, E_int,run_z_no_unc=run_z_no_unc)
                z2 = Z * Z

                val = dp[j-1, p] + z2

                if val > best:
                    best = val
                    best_p = p

            dp[j, i] = best
            prv[j, i] = best_p

    # ----------------------------------
    # Check solution
    # ----------------------------------
    if dp[n_bins, N] <= NEG/2:
        tqdm.write("[DP WARNING] No valid binning found")
        return None,None

    # ----------------------------------
    # Backtrack
    # ----------------------------------
    idx = []
    j, i = n_bins, N

    while j > 0:
        p = prv[j, i]
        if p < 0:
            return None,None
        idx.append(p)
        i = p
        j -= 1

    idx = list(reversed(idx))

    # ----------------------------------
    # Build edges
    # ----------------------------------
    edges = [bin_lo[0]]

    for k in idx:
        if k != 0:
            edges.append(bin_lo[k])

    edges.append(bin_hi[-1])

    best_Z2 = dp[n_bins, N]
    best_Z  = math.sqrt(best_Z2)
    
    return edges, best_Z




# =========================================================


def evaluate_dp_per_flavour_per_mass_run2(data, n_bins=6, use_fake_corr=None,run_z_no_unc=True):

    print("\n==============================")
    print(" DP BINNING (PER MASS)")
    print("==============================")
    print(" - Optimisation per (flavour, mass)")
    print(" - Signal = single mass (Run2 combined via per-era)")
    print(" - Background = per-era (fake fixed per era)")
    print(" - Produces mass-dependent binning")
    print("==============================\n")

    if use_fake_corr is None:
        raise ValueError("use_fake_corr must be provided from config")
    if run_z_no_unc is None:
        raise ValueError("run_z_no_unc must be provided from config")

    results = {flav: [] for flav in FLAVOURS}

    bin_lo = data["edges"][:-1]

    total_jobs = sum(len(data["signal_combined_mass"][flav]) for flav in FLAVOURS)
    pbar = tqdm(total=total_jobs, desc="DP Scan")

    for flav in FLAVOURS:

        tqdm.write(f"[DP SCAN] Flavour = {flav}")

        for mass in data["signal_combined_mass"][flav]:

            tqdm.write(f"\n[Mass] {mass}")

            # ----------------------------------
            # Build PER-ERA arrays (for DP)
            # ----------------------------------
            S_per_era = {}
            B_per_era = {}
            E_per_era = {}
            F_per_era = {}
            
            for era in ERAS:
                S_per_era[era] = data["signal"][flav][mass][era]
                B_per_era[era] = data["background"][flav][era]
                E_per_era[era] = data["bkg_err2"][flav][era]
                F_per_era[era] = data["fake"][flav][era]
                
            print("\n==============================")
            print(f"[DEBUG] Signal yields for {flav}, mass={mass}")
            print("==============================")
            
            total_run2 = 0.0
            
            for era in ERAS:

                S_arr = data["signal"][flav][mass][era]
                S_sum = S_arr.sum()
                
                print(f"{era:12s}: S_sum = {S_sum:.6e}")
                
                total_run2 += S_sum

            print("----------------------------------")
            print(f"Run2 total: S_sum = {total_run2:.6e}")
            print("==================================\n")
                
            # ----------------------------------
            # Run DP (PER-ERA CORRECT)
            # ----------------------------------
            best_edges, best_Z_dp = run_dp_on_arrays(
                S_per_era,
                B_per_era,
                E_per_era,
                F_per_era,
                bin_lo,
                n_bins=n_bins,
                use_fake_corr=use_fake_corr,
                flav=flav,
                run_z_no_unc=run_z_no_unc
            )
            if TEST_COMPARE:
                edges_slow, best_Z_dp = run_dp_on_arrays_slow( S_per_era,
                                                    B_per_era,
                                                    E_per_era,
                                                    F_per_era,
                                                    bin_lo,
                                                    n_bins=n_bins,
                                                    use_fake_corr=use_fake_corr,
                                                    flav=flav,
                                                    run_z_no_unc=run_z_no_unc
                                                   )
                
                if best_edges != edges_slow:
                    print("[WARNING] FAST vs SLOW mismatch!")
                    print("FAST:", best_edges)
                    print("SLOW:", edges_slow)
                else:
                    print("[NOTE] FAST vs SLOW match!")

                    


            if best_edges is None:
                pbar.update(1)
                continue
            
            edges_run2 = best_edges
            edges_per_era = {era: best_edges for era in ERAS}

            # ==================================
            # BIN STATS CONTAINERS
            # ==================================
            bin_stats_per_era = {era: [] for era in ERAS}
            bin_stats_run2 = []

            # ----------------------------------
            # QUAD (per era evaluation)
            # ----------------------------------
            quad_Z2 = 0.0

            for era in ERAS:

                S_arr = data["signal"][flav][mass][era]
                B_arr = data["background"][flav][era]
                F_arr = data["fake"][flav][era]
                E_arr = data["bkg_err2"][flav][era]

                for i in range(len(best_edges)-1):

                    lo = best_edges[i]
                    hi = best_edges[i+1]

                    if i == len(best_edges)-2:
                        mask = (bin_lo >= lo) & (bin_lo <= hi)
                    else:
                        mask = (bin_lo >= lo) & (bin_lo < hi)

                    S = S_arr[mask].sum()
                    B = B_arr[mask].sum()
                    F = F_arr[mask].sum()
                    E = E_arr[mask].sum()

                    if use_fake_corr:
                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav, era=era
                        )

                    bin_stats_per_era[era].append({
                        "S": S,
                        "B": B,
                        "E": E
                    })

                    if S > 0 and B > 0:
                        Z = compute_bin_Z_with_unc(S, B, E,run_z_no_unc=run_z_no_unc)
                        quad_Z2 += Z * Z

            quad_total = math.sqrt(quad_Z2)

            # ----------------------------------
            # Run2 evaluation (sum AFTER fix)
            # ----------------------------------
            run2_Z2 = 0.0

            for i in range(len(best_edges)-1):

                lo = best_edges[i]
                hi = best_edges[i+1]

                if i == len(best_edges)-2:
                    mask = (bin_lo >= lo) & (bin_lo <= hi)
                else:
                    mask = (bin_lo >= lo) & (bin_lo < hi)

                S_tot = 0.0
                B_tot = 0.0
                E_tot = 0.0

                for era in ERAS:

                    S = data["signal"][flav][mass][era][mask].sum()
                    B = data["background"][flav][era][mask].sum()
                    F = data["fake"][flav][era][mask].sum()
                    E = data["bkg_err2"][flav][era][mask].sum()

                    if use_fake_corr:
                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav, era=era
                        )

                    S_tot += S
                    B_tot += B
                    E_tot += E

                bin_stats_run2.append({
                    "S": S_tot,
                    "B": B_tot,
                    "E": E_tot
                })

                if S_tot > 0 and B_tot > 0:
                    Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot,run_z_no_unc=run_z_no_unc)
                    run2_Z2 += Z * Z

            run2_total = math.sqrt(run2_Z2)
            print("--------------------------------------------------")
            print(f"[DEBUG] DP Z (optimised) = {best_Z_dp:.6f}")
            print(f"[DEBUG] Eval Run2 Z      = {run2_total:.6f}")
            print(f"[DEBUG] Difference       = {run2_total - best_Z_dp:.6f}")
            print("--------------------------------------------------")
            
            ratio = run2_total / quad_total if quad_total > 0 else 0.0

            tqdm.write(
                f"[SUMMARY] nBins={n_bins} QUAD={quad_total:.4f} | Run2={run2_total:.4f}"
            )

            results[flav].append({
                "flav": flav,
                "mass": mass,
                "nbins": len(best_edges) - 1,
                "quad": quad_total,
                "run2": run2_total,
                "ratio": ratio,
                "binning": {
                    "Run2": edges_run2,
                    "per_era": edges_per_era
                },
                "bin_stats": {
                    "Run2": bin_stats_run2,
                    "per_era": bin_stats_per_era
                }
            })

            pbar.update(1)

    pbar.close()
    
    return results



def evaluate_dp_per_flavour_global_mass_run2_weighted_refbins(
        data, n_bins=6, use_fake_corr=None, run_z_no_unc=True, mass_list=None):

    print("\n==============================")
    print(" GLOBAL DP BINNING (PER FLAVOUR, WEIGHTED)")
    print("==============================")
    print(" - Optimisation uses combined signal across all masses")
    print(" - Masses weighted by reference Z (custom bins)")
    print(" - One binning per flavour")
    print("==============================\n")

    if use_fake_corr is None:
        raise ValueError("use_fake_corr must be provided from config")
    if run_z_no_unc is None:
        raise ValueError("run_z_no_unc must be provided from config")

    # Reference binning
    REF_EDGES = [0.0, 400.0, 600.0, 800.0,1200., 5000.0]

    results = {flav: [] for flav in FLAVOURS}
    bin_lo = data["edges"][:-1]

    for flav in FLAVOURS:

        print("\n[GLOBAL DP] Flavour = {}".format(flav))

        if mass_list:
            masses = mass_list
        else:
            masses = list(data["signal_combined_mass"][flav].keys())

        # ----------------------------------
        # Compute mass weights
        # ----------------------------------
        mass_weights = {}

        print("\n[INFO] Computing reference Z weights")

        for mass in masses:

            Z_ref = compute_reference_Z_per_mass(
                data, flav, mass,
                REF_EDGES,
                use_fake_corr,
                run_z_no_unc
            )

            if Z_ref > 1e-6:
                mass_weights[mass] = 1.0 / Z_ref
            else:
                mass_weights[mass] = 1.0

                
            print("  Mass {} : Z_ref={:.4f} -> weight={:.4f}".format(
                mass, Z_ref, mass_weights[mass]))

            # Normalize weights
        #norm = sum(mass_weights.values())
        #if norm > 0:
        #    for m in mass_weights:
        #        mass_weights[m] /= norm


        #print("\n[INFO] Normalized mass weights")
        #for m in mass_weights:
        #   print("  Mass {} : weight_norm={:.4f}".format(m, mass_weights[m]))
            
        # ----------------------------------
        # Build arrays
        # ----------------------------------
        S_mass_flav_era = {}
        B_flav_era = {}
        E_flav_era = {}
        F_flav_era = {}

        for mass in masses:
            S_mass_flav_era[mass] = {}
            for era in ERAS:
                S_mass_flav_era[mass][era] = data["signal"][flav][mass][era]

            print("\n==============================")
            print(f"[DEBUG] GLOB Signal yields for {flav}, mass={mass}")
            print("==============================")

            total_run2 = 0.0

            for era in ERAS:

                S_arr = data["signal"][flav][mass][era]
                S_sum = S_arr.sum()

                print(f"{era:12s}: S_sum = {S_sum:.6e}")

                total_run2 += S_sum

            print("----------------------------------")
            print(f"Run2 total: S_sum = {total_run2:.6e}")
            print("==================================\n")

                
        for era in ERAS:
            B_flav_era[era] = data["background"][flav][era]
            E_flav_era[era] = data["bkg_err2"][flav][era]
            F_flav_era[era] = data["fake"][flav][era]

        # ----------------------------------
        # Run DP with weights
        # ----------------------------------
        best_edges, best_Z_dp = run_dp_on_arrays_mass_with_weight(
            S_mass_flav_era,
            B_flav_era,
            E_flav_era,
            F_flav_era,
            bin_lo,
            n_bins=n_bins,
            use_fake_corr=use_fake_corr,
            flav=flav,
            run_z_no_unc=run_z_no_unc,
            mass_weights=mass_weights
        )

        if best_edges is None:
            print("[WARNING] No binning found for {}".format(flav))
            continue

        print("[GLOBAL BINNING] {}: {}".format(flav, best_edges))

        # ----------------------------------
        # Evaluate per mass
        # ----------------------------------
        for mass in masses:

            print("\n[Mass] {} (using global binning)".format(mass))

            edges_run2 = best_edges
            edges_per_era = {era: best_edges for era in ERAS}

            quad_Z2 = 0.0
            bin_stats_per_era = {era: [] for era in ERAS}
            bin_stats_run2 = []

            # ---- QUAD
            for era in ERAS:

                S_arr = data["signal"][flav][mass][era]
                B_arr = data["background"][flav][era]
                F_arr = data["fake"][flav][era]
                E_arr = data["bkg_err2"][flav][era]

                for i in range(len(best_edges)-1):

                    lo = best_edges[i]
                    hi = best_edges[i+1]

                    if i == len(best_edges)-2:
                        mask = (bin_lo >= lo) & (bin_lo <= hi)
                    else:
                        mask = (bin_lo >= lo) & (bin_lo < hi)

                    S = S_arr[mask].sum()
                    B = B_arr[mask].sum()
                    F = F_arr[mask].sum()
                    E = E_arr[mask].sum()

                    if use_fake_corr:
                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav, era=era
                        )

                    bin_stats_per_era[era].append({
                        "S": S,
                        "B": B,
                        "E": E
                    })

                    if S > 0 and B > 0:
                        Z = compute_bin_Z_with_unc(
                            S, B, E,
                            run_z_no_unc=run_z_no_unc
                        )
                        quad_Z2 += Z * Z

            quad_total = math.sqrt(quad_Z2)

            # ---- Run2
            run2_Z2 = 0.0

            for i in range(len(best_edges)-1):

                lo = best_edges[i]
                hi = best_edges[i+1]

                if i == len(best_edges)-2:
                    mask = (bin_lo >= lo) & (bin_lo <= hi)
                else:
                    mask = (bin_lo >= lo) & (bin_lo < hi)

                S_tot = 0.0
                B_tot = 0.0
                E_tot = 0.0

                for era in ERAS:

                    S = data["signal"][flav][mass][era][mask].sum()
                    B = data["background"][flav][era][mask].sum()
                    F = data["fake"][flav][era][mask].sum()
                    E = data["bkg_err2"][flav][era][mask].sum()

                    if use_fake_corr:
                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav, era=era
                        )

                    S_tot += S
                    B_tot += B
                    E_tot += E

                bin_stats_run2.append({
                    "S": S_tot,
                    "B": B_tot,
                    "E": E_tot
                })

                if S_tot > 0 and B_tot > 0:
                    Z = compute_bin_Z_with_unc(
                        S_tot, B_tot, E_tot,
                        run_z_no_unc=run_z_no_unc
                    )
                    run2_Z2 += Z * Z

            run2_total = math.sqrt(run2_Z2)

            print("--------------------------------------------------")
            print(f"[DEBUG] DP ZW (optimised) = {best_Z_dp:.6f}")
            print(f"[DEBUG] Eval Run2 Z      = {run2_total:.6f}")
            print(f"[DEBUG] Difference       = {run2_total - best_Z_dp:.6f}")
            print("--------------------------------------------------")


            
            ratio = run2_total / quad_total if quad_total > 0 else 0.0

            print("[SUMMARY] QUAD={:.4f} | Run2={:.4f}".format(
                quad_total, run2_total))

            results[flav].append({
                "flav": flav,
                "mass": mass,
                "nbins": len(best_edges) - 1,
                "quad": quad_total,
                "run2": run2_total,
                "ratio": ratio,
                "binning": {
                    "Run2": edges_run2,
                    "per_era": edges_per_era
                },
                "bin_stats": {
                    "Run2": bin_stats_run2,
                    "per_era": bin_stats_per_era
                }
            })

    return results

def evaluate_dp_per_flavour_global_mass_run2(data, n_bins=6, use_fake_corr=None,run_z_no_unc=True):

    print("\n==============================")
    print(" GLOBAL DP BINNING (PER FLAVOUR)")
    print("==============================")
    print(" - Optimisation uses combined signal across all masses")
    print(" - Background = per-era (fake fixed per era)")
    print(" - One binning per flavour")
    print(" - Same binning applied to all masses")
    print("==============================\n")

    if use_fake_corr is None:
        raise ValueError("use_fake_corr must be provided from config")
    if run_z_no_unc is None:
    	raise ValueError("run_z_no_unc must be provided from config")


    results = {flav: [] for flav in FLAVOURS}

    bin_lo = data["edges"][:-1]

    for flav in FLAVOURS:

        print(f"\n[GLOBAL DP] Flavour = {flav}")
        
        # ==========================================
        # Build MASS + ERA structure
        # ==========================================
        S_mass_flav_era = {}
        B_flav_era = {}
        E_flav_era = {}
        F_flav_era = {}

        masses = data["signal_combined_mass"][flav]
        
        for mass in masses:
            S_mass_flav_era[mass] = {}
            for era in ERAS:
                S_mass_flav_era[mass][era] = data["signal"][flav][mass][era]

        for era in ERAS:
            B_flav_era[era] = data["background"][flav][era]
            E_flav_era[era] = data["bkg_err2"][flav][era]
            F_flav_era[era] = data["fake"][flav][era]

        # ==========================================
        # RUN DP (NEW MASS-AWARE VERSION)
        # ==========================================
        best_edges, best_Z_dp = run_dp_on_arrays_mass(
            S_mass_flav_era,
            B_flav_era,
            E_flav_era,
            F_flav_era,
            bin_lo,
            n_bins=n_bins,
            use_fake_corr=use_fake_corr,
            flav=flav,
            run_z_no_unc=run_z_no_unc
        )


        if best_edges is None:
            print(f"[WARNING] No binning found for {flav}")
            continue

        print(f"[GLOBAL BINNING] {flav}: {best_edges}")

        # ==================================
        # Evaluate per mass
        # ==================================
        for mass in masses:

            print(f"\n[Mass] {mass} (using global binning)")

            edges_run2 = best_edges
            edges_per_era = {era: best_edges for era in ERAS}

            bin_stats_per_era = {era: [] for era in ERAS}
            bin_stats_run2 = []

            # ----------------------------------
            # QUAD (per era)
            # ----------------------------------
            quad_Z2 = 0.0

            for era in ERAS:

                S_arr = data["signal"][flav][mass][era]
                B_arr = data["background"][flav][era]
                F_arr = data["fake"][flav][era]
                E_arr = data["bkg_err2"][flav][era]

                for i in range(len(best_edges)-1):

                    lo = best_edges[i]
                    hi = best_edges[i+1]

                    if i == len(best_edges)-2:
                        mask = (bin_lo >= lo) & (bin_lo <= hi)
                    else:
                        mask = (bin_lo >= lo) & (bin_lo < hi)

                    S = S_arr[mask].sum()
                    B = B_arr[mask].sum()
                    F = F_arr[mask].sum()
                    E = E_arr[mask].sum()

                    if use_fake_corr:
                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav, era=era
                        )

                    bin_stats_per_era[era].append({
                        "S": S,
                        "B": B,
                        "E": E
                    })

                    if S > 0 and B > 0:
                        Z = compute_bin_Z_with_unc(S, B, E,run_z_no_unc=run_z_no_unc)
                        quad_Z2 += Z * Z

            quad_total = math.sqrt(quad_Z2)

            # ----------------------------------
            # Run2 evaluation (sum AFTER per-era fix)
            # ----------------------------------
            run2_Z2 = 0.0

            for i in range(len(best_edges)-1):

                lo = best_edges[i]
                hi = best_edges[i+1]

                if i == len(best_edges)-2:
                    mask = (bin_lo >= lo) & (bin_lo <= hi)
                else:
                    mask = (bin_lo >= lo) & (bin_lo < hi)

                S_tot = 0.0
                B_tot = 0.0
                E_tot = 0.0

                for era in ERAS:

                    S = data["signal"][flav][mass][era][mask].sum()
                    B = data["background"][flav][era][mask].sum()
                    F = data["fake"][flav][era][mask].sum()
                    E = data["bkg_err2"][flav][era][mask].sum()

                    if use_fake_corr:
                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav, era=era
                        )

                    S_tot += S
                    B_tot += B
                    E_tot += E

                bin_stats_run2.append({
                    "S": S_tot,
                    "B": B_tot,
                    "E": E_tot
                })

                if S_tot > 0 and B_tot > 0:
                    Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot,run_z_no_unc=run_z_no_unc)
                    run2_Z2 += Z * Z

            run2_total = math.sqrt(run2_Z2)
            print("--------------------------------------------------")
            print(f"[DEBUG] DP Z (optimised) = {best_Z_dp:.6f}")
            print(f"[DEBUG] Eval Run2 Z      = {run2_total:.6f}")
            print(f"[DEBUG] Difference       = {run2_total - best_Z_dp:.6f}")
            print("--------------------------------------------------")
            
            ratio = run2_total / quad_total if quad_total > 0 else 0.0

            print(f"[SUMMARY] QUAD={quad_total:.4f} | Run2={run2_total:.4f}")

            results[flav].append({
                "flav": flav,
                "mass": mass,
                "nbins": len(best_edges) - 1,
                "quad": quad_total,
                "run2": run2_total,
                "ratio": ratio,
                "binning": {
                    "Run2": edges_run2,
                    "per_era": edges_per_era
                },
                "bin_stats": {
                    "Run2": bin_stats_run2,
                    "per_era": bin_stats_per_era
                }
            })

    return results


def compute_reference_Z_per_mass(data, flav, mass, ref_edges, use_fake_corr, run_z_no_unc):

    bin_lo = data["edges"][:-1]

    Z2 = 0.0

    for i in range(len(ref_edges) - 1):

        lo = ref_edges[i]
        hi = ref_edges[i+1]

        if i == len(ref_edges) - 2:
            mask = (bin_lo >= lo) & (bin_lo <= hi)
        else:
            mask = (bin_lo >= lo) & (bin_lo < hi)

        S_tot = 0.0
        B_tot = 0.0
        E_tot = 0.0

        for era in ERAS:

            S = data["signal"][flav][mass][era][mask].sum()
            B = data["background"][flav][era][mask].sum()
            F = data["fake"][flav][era][mask].sum()
            E = data["bkg_err2"][flav][era][mask].sum()

            if use_fake_corr:
                F, B = fix_fake_and_bkg(F, B, FAKE_FLOOR,
                                        flavour=flav, era=era)

            S_tot += S
            B_tot += B
            E_tot += E

        if S_tot > 0 and B_tot > 0:
            Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot,
                                      run_z_no_unc=run_z_no_unc)
            Z2 += Z * Z

    return math.sqrt(Z2)


def evaluate_dp_flavour_per_mass_combined_stat_split_run2(
        data, n_bins=6, use_fake_corr=None, run_z_no_unc=True):

    print("\n==============================")
    print(" GLOBAL DP BINNING (ALL FLAVOURS)")
    print("==============================")
    print(" - Optimisation uses ALL flavours combined")
    print(" - One binning per mass")
    print(" - Same binning applied to each flavour")
    print(" - Stat check = per flavour (Run2 combined)")
    print("==============================\n")

    if use_fake_corr is None:
        raise ValueError("use_fake_corr must be provided from config")
    if run_z_no_unc is None:
        raise ValueError("run_z_no_unc must be provided from config")

    results = {}

    bin_lo = data["edges"][:-1]
    masses = list(data["signal_global"].keys())

    pbar = tqdm(total=len(masses), desc="DP Global Scan")

    for mass in masses:

        tqdm.write("\n[Mass] {} (combined flavours)".format(mass))

        # ==================================================
        # BUILD PER-ERA ARRAYS (for DP)
        # ==================================================
        S_flav_era = {}
        B_flav_era = {}
        E_flav_era = {}
        F_flav_era = {}

        for flav in FLAVOURS:

            S_flav_era[flav] = {}
            B_flav_era[flav] = {}
            E_flav_era[flav] = {}
            F_flav_era[flav] = {}

            for era in ERAS:

                S_flav_era[flav][era] = data["signal"][flav][mass][era]
                B_flav_era[flav][era] = data["background"][flav][era]
                E_flav_era[flav][era] = data["bkg_err2"][flav][era]
                F_flav_era[flav][era] = data["fake"][flav][era]

        # ==================================================
        # RUN DP
        # ==================================================
        best_edges, best_Z_dp = run_dp_on_arrays_with_flav_stat(
            bin_lo,
            n_bins=n_bins,
            use_fake_corr=use_fake_corr,
            S_flav_era=S_flav_era,
            B_flav_era=B_flav_era,
            E_flav_era=E_flav_era,
            F_flav_era=F_flav_era,
            run_z_no_unc=run_z_no_unc
        )

        if best_edges is None:
            pbar.update(1)
            continue

        # ==================================================
        # EVALUATION PER FLAVOUR (FIXED)
        # ==================================================
        for flav in FLAVOURS:

            run2_Z2 = 0.0
            bin_stats_run2 = []

            for i in range(len(best_edges) - 1):

                lo = best_edges[i]
                hi = best_edges[i + 1]

                if i == len(best_edges) - 2:
                    mask = (bin_lo >= lo) & (bin_lo <= hi)
                else:
                    mask = (bin_lo >= lo) & (bin_lo < hi)

                S_tot = 0.0
                B_tot = 0.0
                E_tot = 0.0

                # =========================================
                # FIX: per-era fake correction BEFORE sum
                # =========================================
                for era in ERAS:

                    S = data["signal"][flav][mass][era][mask].sum()
                    B = data["background"][flav][era][mask].sum()
                    F = data["fake"][flav][era][mask].sum()
                    E = data["bkg_err2"][flav][era][mask].sum()

                    if use_fake_corr:
                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav,
                            era=era   # <-- FIXED (was "Run2")
                        )

                    S_tot += S
                    B_tot += B
                    E_tot += E

                bin_stats_run2.append({
                    "S": S_tot,
                    "B": B_tot,
                    "E": E_tot
                })

                if S_tot > 0 and B_tot > 0:
                    Z = compute_bin_Z_with_unc(
                        S_tot, B_tot, E_tot,
                        run_z_no_unc=run_z_no_unc
                    )
                    run2_Z2 += Z * Z

            run2_total = math.sqrt(run2_Z2)

            print("--------------------------------------------------")
            print(f"[DEBUG] DP Z (optimised) = {best_Z_dp:.6f}")
            print(f"[DEBUG] Eval Run2 Z      = {run2_total:.6f}")
            print(f"[DEBUG] Difference       = {run2_total - best_Z_dp:.6f}")
            print("--------------------------------------------------")

            # ==================================================
            # QUAD (unchanged, already correct)
            # ==================================================
            quad_Z2 = 0.0
            bin_stats_per_era = {}

            for era in ERAS:

                S_arr = data["signal"][flav][mass][era]
                B_arr = data["background"][flav][era]
                E_arr = data["bkg_err2"][flav][era]
                F_arr = data["fake"][flav][era]

                stats = []

                for i in range(len(best_edges) - 1):

                    lo = best_edges[i]
                    hi = best_edges[i + 1]

                    if i == len(best_edges) - 2:
                        mask = (bin_lo >= lo) & (bin_lo <= hi)
                    else:
                        mask = (bin_lo >= lo) & (bin_lo < hi)

                    S = S_arr[mask].sum()
                    B = B_arr[mask].sum()
                    F = F_arr[mask].sum()
                    E = E_arr[mask].sum()

                    if use_fake_corr:
                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav,
                            era=era
                        )

                    stats.append({"S": S, "B": B, "E": E})

                    if S > 0 and B > 0:
                        Z = compute_bin_Z_with_unc(
                            S, B, E,
                            run_z_no_unc=run_z_no_unc
                        )
                        quad_Z2 += Z * Z

                bin_stats_per_era[era] = stats

            quad_total = math.sqrt(quad_Z2)

            ratio = run2_total / quad_total if quad_total > 0 else 0.0

            tqdm.write("[{}] Run2={:.4f} | QUAD={:.4f}".format(
                flav, run2_total, quad_total))

            results.setdefault(flav, []).append({
                "flav": flav,
                "mass": mass,
                "nbins": len(best_edges) - 1,
                "run2": run2_total,
                "quad": quad_total,
                "ratio": ratio,
                "binning": {
                    "Run2": best_edges,
                    "per_era": {era: best_edges for era in ERAS}
                },
                "bin_stats": {
                    "Run2": bin_stats_run2,
                    "per_era": bin_stats_per_era
                }
            })

        pbar.update(1)

    pbar.close()

    return results



def evaluate_dp_per_mass_per_flavour_per_era(data, n_bins=6, use_fake_corr=None,run_z_no_unc=True):

    print("\n==============================")
    print(" DP BINNING (PER MASS, PER FLAVOUR, PER ERA)")
    print("==============================")
    print(" - Optimisation per (flavour, mass, era)")
    print(" - Each era has independent binning")
    print(" - Stat check done per era")
    print(" - Run2 FOM recomputed from combined bins")
    print("==============================\n")

    if use_fake_corr is None:
        raise ValueError("use_fake_corr must be provided from config")
    if run_z_no_unc is None:
    	raise ValueError("run_z_no_unc must be provided from config")

        
    results = {flav: [] for flav in FLAVOURS}

    bin_lo = data["edges"][:-1]

    total_jobs = sum(len(data["signal_combined_mass"][flav]) for flav in FLAVOURS)
    pbar = tqdm(total=total_jobs, desc="DP Scan (Per Era)")

    for flav in FLAVOURS:

        tqdm.write(f"[DP SCAN] Flavour = {flav}")

        for mass in data["signal_combined_mass"][flav]:

            tqdm.write(f"\n[Mass] {mass}")

            best_edges_per_era = {}
            bin_stats_per_era = {}

            quad_Z2 = 0.0

            # ==========================================
            # LOOP ERAS (OPTIMISATION)
            # ==========================================
            for era in ERAS:

                S_arr = data["signal"][flav][mass][era]
                B_arr = data["background"][flav][era]
                E_arr = data["bkg_err2"][flav][era]
                F_arr = data["fake"][flav][era]

                # ----------------------------------
                # Wrap into per-era dicts (NEW)
                # ----------------------------------
                S_dict = {era: S_arr}
                B_dict = {era: B_arr}
                E_dict = {era: E_arr}
                F_dict = {era: F_arr}

                best_edges, best_Z_dp = run_dp_on_arrays(
                    S_dict,
                    B_dict,
                    E_dict,
                    F_dict,
                    bin_lo,
                    n_bins=n_bins,
                    use_fake_corr=use_fake_corr,
                    flav=flav,
                    use_low_edge_cut=False,
                    run_z_no_unc=run_z_no_unc
                )

                if best_edges is None:
                    continue

                best_edges_per_era[era] = best_edges
                bin_stats_per_era[era] = []

                # ----------------------------------
                # Evaluate bins (this era)
                # ----------------------------------
                for i in range(len(best_edges)-1):

                    lo = best_edges[i]
                    hi = best_edges[i+1]

                    if i == len(best_edges)-2:
                        mask = (bin_lo >= lo) & (bin_lo <= hi)
                    else:
                        mask = (bin_lo >= lo) & (bin_lo < hi)

                    S = S_arr[mask].sum()
                    B = B_arr[mask].sum()
                    F = F_arr[mask].sum()
                    E = E_arr[mask].sum()

                    if use_fake_corr:
                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav,
                            era=era
                        )

                    bin_stats_per_era[era].append({
                        "S": S,
                        "B": B,
                        "E": E
                    })

                    if S > 0 and B > 0:
                        Z = compute_bin_Z_with_unc(S, B, E,run_z_no_unc=run_z_no_unc)
                        quad_Z2 += Z * Z

            quad_total = math.sqrt(quad_Z2)

            # ==========================================
            # RUN2 FOM (sum AFTER per-era fix)
            # ==========================================
            run2_Z2 = 0.0

            if len(best_edges_per_era) == 0:
                pbar.update(1)
                continue

            n_bins_eff = len(next(iter(best_edges_per_era.values()))) - 1

            for i in range(n_bins_eff):

                S_tot = 0.0
                B_tot = 0.0
                E_tot = 0.0

                for era in ERAS:

                    if era not in best_edges_per_era:
                        continue

                    edges = best_edges_per_era[era]

                    lo = edges[i]
                    hi = edges[i+1]

                    if i == len(edges)-2:
                        mask = (bin_lo >= lo) & (bin_lo <= hi)
                    else:
                        mask = (bin_lo >= lo) & (bin_lo < hi)

                    S = data["signal"][flav][mass][era][mask].sum()
                    B = data["background"][flav][era][mask].sum()
                    F = data["fake"][flav][era][mask].sum()
                    E = data["bkg_err2"][flav][era][mask].sum()

                    if use_fake_corr:
                        F, B = fix_fake_and_bkg(
                            F, B, FAKE_FLOOR,
                            flavour=flav,
                            era=era
                        )

                    S_tot += S
                    B_tot += B
                    E_tot += E

                if S_tot > 0 and B_tot > 0:
                    Z = compute_bin_Z_with_unc(S_tot, B_tot, E_tot,run_z_no_unc=run_z_no_unc)
                    run2_Z2 += Z * Z

            run2_total = math.sqrt(run2_Z2)
            print("--------------------------------------------------")
            print(f"[DEBUG] DP Z (optimised) = {best_Z_dp:.6f}")
            print(f"[DEBUG] Eval Run2 Z      = {run2_total:.6f}")
            print(f"[DEBUG] Difference       = {run2_total - best_Z_dp:.6f}")
            print("--------------------------------------------------")
            ratio = run2_total / quad_total if quad_total > 0 else 0.0

            tqdm.write(
                f"[SUMMARY] QUAD={quad_total:.4f} | Run2={run2_total:.4f}"
            )

            results[flav].append({
                "flav": flav,
                "mass": mass,
                "nbins": n_bins_eff,
                "quad": quad_total,
                "run2": run2_total,
                "ratio": ratio,
                "binning": {
                    "Run2": None,
                    "per_era": best_edges_per_era
                },
                "bin_stats": {
                    "Run2": None,
                    "per_era": bin_stats_per_era
                }
            })

            pbar.update(1)

    pbar.close()

    return results
