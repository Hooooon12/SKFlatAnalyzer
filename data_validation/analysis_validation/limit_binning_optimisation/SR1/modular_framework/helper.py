import math
import os,sys
from default_config import ERAS, FLAVOURS, FAKE_FLOOR

def list_available_configs(config_dir="config", script_name=None):
    print("\n====================================")
    print(" AVAILABLE CONFIG FILES")
    print("====================================\n")

    if not os.path.isdir(config_dir):
        print(f"[ERROR] Config directory not found: {config_dir}")
        return

    files = sorted(os.listdir(config_dir))
    py_files = [f for f in files if f.endswith(".py") and not f.startswith("__")]

    if len(py_files) == 0:
        print("[WARNING] No config files found")
        return

    if script_name is None:
        script_name = os.path.basename(sys.argv[0])

    for f in py_files:
        mod = f.replace(".py", "")
        print(f"python {script_name} --config config.{mod}")

    print("\n====================================\n")
    
def ConvertConfPath(path):
    # remove .py if present
    if path.endswith(".py"):
        path = path[:-3]
    else:
        return path

    # replace / with .
    path = path.replace("/", ".")

    # remove leading ./ if present
    if path.startswith("."):
        path = path[1:]

    return path

def ReadConfig(cfg):

    def get(name):
        if not hasattr(cfg, name):
            raise AttributeError(f"[CONFIG ERROR] Missing '{name}' in config")
        return getattr(cfg, name)
    
    return (
        get("MASSES"),
        get("NBINS_TO_SCAN"),
        get("USE_FAKE_FIX"),
        get("RUN_Z_NO_UNC"),
        get("LOG_TAG"),
        get("TAG"),
    )


def get_latest_dir(base):
    dirs = [os.path.join(base, d)
            for d in os.listdir(base)
            if os.path.isdir(os.path.join(base, d))]
    return max(dirs, key=os.path.getmtime)




def debug_data_summary(data, max_bins=5):

    print("\n==============================")
    print(" DEBUG DATA SUMMARY")
    print("==============================")

    edges = data["edges"]
    print("\n[EDGES]")
    print("N bins =", len(edges) - 1)
    print("First edges:", edges[:max_bins+1])
    print("Last edges:", edges[-(max_bins+1):])

    for flav in FLAVOURS:

        print(f"\n================ {flav} =================")

        for era in ERAS:

            print(f"\n[{era}]")

            B = data["background"][flav][era]
            F = data["fake"][flav][era]

            print(f"  Bkg:  sum={B.sum():.3f}, min={B.min():.3f}, max={B.max():.3f}")
            print(f"  Fake: sum={F.sum():.3f}, min={F.min():.3f}, max={F.max():.3f}")

            # Check negative fake bins
            neg_bins = [(i, F[i]) for i in range(len(F)) if F[i] <= 0]
            if neg_bins:
                print(f"  [WARN] {len(neg_bins)} fake bins <= 0 (showing first {max_bins})")
                print("   ", neg_bins[:max_bins])
            else:
                print("  Fake all positive")

            print("  First bins:")
            for i in range(min(max_bins, len(B))):
                print(f"    bin {i}: B={B[i]:.3f}, F={F[i]:.3f}")

        # ----------------------------------
        # Signal checks
        # ----------------------------------
        print(f"\n[{flav}] SIGNAL CHECK")

        for mass in list(data["signal_combined_mass"][flav].keys())[:3]:

            S = data["signal_combined_mass"][flav][mass]

            print(f"  Mass {mass}: sum={S.sum():.4f}, min={S.min():.4e}, max={S.max():.4e}")
            
def rebin_array(arr, edges_full, edges_target):
    out = []

    for i in range(len(edges_target)-1):
        lo = edges_target[i]
        hi = edges_target[i+1]

        mask = (edges_full[:-1] >= lo) & (edges_full[:-1] < hi)
        out.append(arr[mask].sum())

    return np.array(out)

def compute_bin_Z_with_unc(S, B, E,run_z_no_unc=True):

    if run_z_no_unc:
        return compute_bin_Z(S,B)

    if S <= 0 or B <= 0:
        return 0.0

    if B <= 1e-9:
        return 0.0
    
    sigma2 = E

    if sigma2 <= 0:
        return compute_bin_Z(S, B)

    term1 = (S + B) * math.log((S + B)*(B + sigma2) / (B*B + (S + B)*sigma2))
    term2 = (B*B / sigma2) * math.log(1 + sigma2*S / (B*(B + sigma2)))

    Z2 = 2 * (term1 - term2)

    return math.sqrt(Z2) if Z2 > 0 else 0.0


def compute_bin_Z(s, b):

    if b <= 0:
        return 0.0

    if s <= 0:
        return 0.0

    # Protect against tiny s/b
    x = s / b

    if x < 1e-6:
        return s / math.sqrt(b)

    val = 2 * ((s + b) * math.log(1 + x) - s)

    if val <= 0:
        return 0.0

    return math.sqrt(val)


def fix_fake_and_bkg(F, B, FAKE_FLOOR, flavour=None, era=None, debug=False):

    if F <= 0.0:
        delta = FAKE_FLOOR - F

        if debug:
            print("[fix_fake_and_bkg] era={} flavour={}, fake_before={:.6f}, fake_floor={:.6f}, delta={:.6f}".format(
                era, flavour, F, FAKE_FLOOR, delta
            ))

        if debug and era == "Run2":
            print("[DEBUG] Before:  F =", F, "B =", B)

        F = FAKE_FLOOR
        B += delta

        if debug and era == "Run2":
            print("[DEBUG] After:   F =", F, "B =", B)

    return F, B


def compute_run2_fom_for_edges(edges, data, flav, mass, bin_lo,
                               use_fake_corr=True, run_z_no_unc=True):

    run2_Z2 = 0.0

    print("\n======================================")
    print(f"[DEBUG] Run2 FOM for {flav}, mass={mass}")
    print("Edges:", edges)
    print("======================================")

    for i in range(len(edges)-1):

        lo = edges[i]
        hi = edges[i+1]

        if i == len(edges)-2:
            mask = (bin_lo >= lo) & (bin_lo <= hi)
        else:
            mask = (bin_lo >= lo) & (bin_lo < hi)

        S_tot = 0.0
        B_tot = 0.0
        E_tot = 0.0

        print(f"\n--- Bin {i}: [{lo}, {hi}] ---")

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

            print(f"{era}: S={S:.4f}, B={B:.4f}, E={E:.4f}")

            S_tot += S
            B_tot += B
            E_tot += E

        if S_tot > 0 and B_tot > 0:
            Z = compute_bin_Z_with_unc(
                S_tot, B_tot, E_tot,
                run_z_no_unc=run_z_no_unc
            )
        else:
            Z = 0.0

        print(f"TOTAL: S={S_tot:.4f}, B={B_tot:.4f}, Z={Z:.6f}")

        run2_Z2 += Z * Z

    run2_total = math.sqrt(run2_Z2)

    print("\n======================================")
    print(f"[RESULT] Run2 FOM = {run2_total:.6f}")
    print("======================================\n")

    return run2_total

def edges_to_indices(edges, bin_lo):
    idx = []
    for e in edges[:-1]:  # ignore last edge (hi)                                                                                                                                                                                                                              
        i = np.where(bin_lo == e)[0]
        if len(i) == 0:
            raise ValueError(f"Edge {e} not found in bin_lo")
        idx.append(int(i[0]))
    idx.append(len(bin_lo))  # final N                                                                                                                                                                                                                                         
    return idx
def debug_check_path(dp, prv, edges, bin_lo):

    idx = edges_to_indices(edges, bin_lo)

    print("\n======================================")
    print("[DEBUG] Checking DP path for edges:", edges)
    print("Indices:", idx)
    print("======================================")

    for j in range(1, len(idx)):

        i = idx[j]
        p = idx[j-1]

        val = dp[j, i]
        prev_p = prv[j, i]

        print(f"\nStep {j}: p={p} -> i={i}")
        print(f"dp[{j},{i}] = {val}")
        print(f"prv[{j},{i}] = {prev_p}")
        if prev_p != p:
            print(" NOT chosen by DP")
        else:
            print(" matches DP choice")
