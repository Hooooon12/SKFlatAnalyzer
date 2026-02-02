import ROOT
import csv

# ------------------------------------------------------------------------------
# 1. Batch Mode (Disable canvas popup)
# ------------------------------------------------------------------------------
ROOT.gROOT.SetBatch(True)

# ------------------------------------------------------------------------------
# Configuration
# ------------------------------------------------------------------------------
#INPUT_FILE = "higgsCombine_grid_2D_expected.MultiDimFit.mH120.root" # Check filename
INPUT_FILE = "higgsCombine_Run2_3ch_M100_syst_grid_2D_Asimov_r0f0.5.MultiDimFit.mH120.root" # Check filename
TREE_NAME  = "limit"

# Target Cut to find (e.g., 95% CL)
# The script will find the point with 2dNLL closest to this value.
TARGET_CUT = 3.84 # 5.99 for 2D, 3.84 for 1D

OUTPUT_CSV = "limit_points_by_f.csv"

# ------------------------------------------------------------------------------
# Main Logic
# ------------------------------------------------------------------------------
def scan_limit_by_f():
    print(f"Opening {INPUT_FILE}...")
    f_root = ROOT.TFile.Open(INPUT_FILE)
    if not f_root:
        print(f"Error: Cannot open {INPUT_FILE}")
        return
    
    t = f_root.Get(TREE_NAME)
    n_entries = t.GetEntries()
    
    # -------------------------------------------------------
    # 1. Data Collection and Grouping (by f value)
    # -------------------------------------------------------
    # Structure: data_map[f_value] = [ (r1, 2dnll1), (r2, 2dnll2), ... ]
    data_map = {}
    
    # Tracking Best Fit Point
    global_min_nll = 9999.0
    best_fit_point = (0, 0) # (f, r)

    print(f"Reading {n_entries} entries...")
    
    for i in range(n_entries):
        t.GetEntry(i)
        
        f_val = t.f
        r_val = t.r
        dnll2 = 2 * t.deltaNLL
        
        # Find Best Fit
        if dnll2 < global_min_nll:
            global_min_nll = dnll2
            best_fit_point = (f_val, r_val)

        # Round f to 6 decimal places to use as key, 
        # avoiding floating point precision issues
        f_key = round(f_val, 6)
        
        if f_key not in data_map:
            data_map[f_key] = []
        
        data_map[f_key].append( (r_val, dnll2) )

    # -------------------------------------------------------
    # 2. Find the point closest to Target Cut for each f
    # -------------------------------------------------------
    sorted_f_keys = sorted(data_map.keys())
    
    results = [] # Storage: (f, closest_r, closest_dnll, diff)

    print("\n" + "="*70)
    print(f" RESULTS: Points closest to 2*dNLL = {TARGET_CUT}")
    print(f" (Scanning along f axis)")
    print("="*70)
    print(f"{'f':^10} | {'Limit r':^12} | {'2*dNLL':^12} | {'Diff to Cut':^12}")
    print("-" * 70)

    for f in sorted_f_keys:
        points = data_map[f] # List of (r, dnll)
        
        # Logic: Find the point with smallest |dnll - TARGET|
        # (i.e., the Grid Point closest to the contour)
        closest_point = min(points, key=lambda p: abs(p[1] - TARGET_CUT))
        
        r_close = closest_point[0]
        dnll_close = closest_point[1]
        diff = abs(dnll_close - TARGET_CUT)
        
        results.append((f, r_close, dnll_close))
        
        # Print to screen
        print(f"{f:10.4f} | {r_close:12.4f} | {dnll_close:12.4f} | {diff:12.4f}")

    print("-" * 70)

    # -------------------------------------------------------
    # 3. Print Best Fit Point (Keep as requested)
    # -------------------------------------------------------
    print(f"\n[Best Fit Point]")
    print(f"  f = {best_fit_point[0]:.6f}")
    print(f"  r = {best_fit_point[1]:.6f}")
    print(f"  (Min 2*dNLL = {global_min_nll:.6f})")
    print("="*70)

    # -------------------------------------------------------
    # 4. Save to CSV
    # -------------------------------------------------------
    with open(OUTPUT_CSV, "w", newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["f", "limit_r", "closest_2dNLL", "Target_Cut"])
        
        for row in results:
            writer.writerow([row[0], row[1], row[2], TARGET_CUT])
            
    print(f"\nSaved extracted limit points to '{OUTPUT_CSV}'")

if __name__ == "__main__":
    scan_limit_by_f()
