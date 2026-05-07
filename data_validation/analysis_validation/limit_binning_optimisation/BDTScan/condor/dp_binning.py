import numpy as np
import math

FAKE_FLOOR = 0.15 * 0.645
BIN_STEP = 0.005
# ---------------------------------------
# Fix fake + background
# ---------------------------------------
def fix_fake_and_bkg(F, B):
    if F <= 0:
        delta = FAKE_FLOOR - F
        F = FAKE_FLOOR
        B += delta
    if B < FAKE_FLOOR:
        B = FAKE_FLOOR
    return F, B

# ---------------------------------------
# Asimov Z^2
# ---------------------------------------
def calculate_fom(s, b):
    if b <= 0:
        return 0.0
    term = (s + b) * math.log(1 + s / b) - s
    return 2 * term if term > 0 else 0.0

# ---------------------------------------
# Build cumulative arrays (Run2 combined)
# ---------------------------------------
def build_run2_cumsum(histograms):

    centers = None
    bvals = None
    fvals = None
    svals = None

    for era, (h_bkg, h_sig, h_fake) in histograms.items():

        c = []
        b = []
        f = []
        s = []

        for i in range(1, h_bkg.GetNbinsX()+1):
            x = h_bkg.GetBinCenter(i)

            
            if not (-0.4 <= x <= 0.4):
                continue
            
            c.append(x)
            b.append(h_bkg.GetBinContent(i))
            f.append(h_fake.GetBinContent(i))
            s.append(h_sig.GetBinContent(i))

        c = np.array(c)
        b = np.array(b)
        f = np.array(f)
        s = np.array(s)

        if centers is None:
            centers = c
            bvals = b
            fvals = f
            svals = s
        else:
            bvals += b
            fvals += f
            svals += s

    cumsum_b = np.cumsum(bvals)
    cumsum_f = np.cumsum(fvals)
    cumsum_s = np.cumsum(svals)

    return centers, cumsum_b, cumsum_f, cumsum_s

# ---------------------------------------
# Fast interval evaluation
# ---------------------------------------
def get_interval(i, j, cumsum_b, cumsum_f, cumsum_s):

    b = cumsum_b[j] - (cumsum_b[i-1] if i > 0 else 0.0)
    f = cumsum_f[j] - (cumsum_f[i-1] if i > 0 else 0.0)
    s = cumsum_s[j] - (cumsum_s[i-1] if i > 0 else 0.0)

    f, b = fix_fake_and_bkg(f, b)

    if b < 1.0:
        return None

    return calculate_fom(s, b)

# ---------------------------------------
# DP optimizer
# ---------------------------------------

def dp_optimal_binning(bin_cache, nbins, x_min=-0.4, x_max=0.4, scan_era="Run2"):

    # ---------------------------------------
    # Allowed edges (ONLY 0.005 grid)
    # ---------------------------------------
    edge_grid = np.round(np.arange(x_min, x_max + BIN_STEP, BIN_STEP), 5)
    N = len(edge_grid)

    # ---------------------------------------
    # Precompute interval FOM
    # ---------------------------------------
    interval_fom = [[None]*N for _ in range(N)]

    for i in range(N):
        for j in range(i+1, N):

            x_low  = edge_grid[i]
            x_high = edge_grid[j]

            run2_sig = 0
            run2_bkg = 0
            scan_sig = 0
            scan_bkg = 0

            for era, (centers, cumsum_b, cumsum_f, cumsum_s, edge_indices) in bin_cache.items():

                idx_low  = int(round((x_low  - x_min) / BIN_STEP))
                idx_high = int(round((x_high - x_min) / BIN_STEP))

                if idx_low < 0 or idx_high >= len(edge_indices):
                    continue

                i_low  = edge_indices[idx_low]
                i_high = edge_indices[idx_high]

                bkg  = cumsum_b[i_high-1] - (cumsum_b[i_low-1] if i_low > 0 else 0.0)
                fake = cumsum_f[i_high-1] - (cumsum_f[i_low-1] if i_low > 0 else 0.0)
                sig  = cumsum_s[i_high-1] - (cumsum_s[i_low-1] if i_low > 0 else 0.0)

                fake, bkg = fix_fake_and_bkg(fake, bkg)

                if bkg < 0.15:
                    break

                run2_sig += sig
                run2_bkg += bkg

                if scan_era == "Run2" or era == scan_era:
                    scan_sig += sig
                    scan_bkg += bkg

            else:
                # only runs if no break occurred
                if run2_bkg >= 1:
                    interval_fom[i][j] = calculate_fom(scan_sig, scan_bkg)

    # ---------------------------------------
    # DP arrays
    # ---------------------------------------
    dp = np.full((nbins+1, N), -1e9)
    prev = np.full((nbins+1, N), -1, dtype=int)

    # ---------------------------------------
    # Base case (1 bin)
    # ---------------------------------------
    for j in range(N):
        if interval_fom[0][j] is not None:
            dp[1][j] = interval_fom[0][j]

    # ---------------------------------------
    # Fill DP
    # ---------------------------------------
    for k in range(2, nbins+1):
        for j in range(N):
            for i in range(k-2, j):

                if dp[k-1][i] < 0:
                    continue

                z = interval_fom[i][j]
                if z is None:
                    continue

                val = dp[k-1][i] + z

                if val > dp[k][j]:
                    dp[k][j] = val
                    prev[k][j] = i

    # ---------------------------------------
    # Backtrack
    # ---------------------------------------
    best_j = np.argmax(dp[nbins])
    best_score = dp[nbins][best_j]

    edges_idx = []
    k = nbins
    j = best_j

    while k > 1:
        i = prev[k][j]
        edges_idx.append(i)
        j = i
        k -= 1

    edges_idx = sorted(edges_idx)

    edges = [x_min]
    for idx in edges_idx:
        edges.append(edge_grid[idx])
    edges.append(x_max)

    return edges, best_score


