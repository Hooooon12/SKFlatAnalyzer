import ROOT
import csv

# ------------------------------------------------------------------------------
# 1. Batch Mode
# ------------------------------------------------------------------------------
ROOT.gROOT.SetBatch(True)

# ------------------------------------------------------------------------------
# Configuration
# ------------------------------------------------------------------------------
#INPUT_FILE = "higgsCombine_grid_2D_expected.MultiDimFit.mH120.root"
INPUT_FILE = "higgsCombine_Run2_EMu_M10000_syst_grid_2D_Asimov_r0f0.5.MultiDimFit.mH120.root"
TREE_NAME  = "limit"

# Target Cut (2D 95% CL)
TARGET_CUT = 5.99 
#TARGET_CUT = 3.84 # 1D 95% CL

OUTPUT_CSV = "limit_points_interpolated.csv"

# ------------------------------------------------------------------------------
# Main Logic
# ------------------------------------------------------------------------------
def scan_limit_interpolated():
    print(f"Opening {INPUT_FILE}...")
    f_root = ROOT.TFile.Open(INPUT_FILE)
    if not f_root:
        print(f"Error: Cannot open {INPUT_FILE}")
        return
    
    t = f_root.Get(TREE_NAME)
    n_entries = t.GetEntries()
    
    # -------------------------------------------------------
    # 1. Group Data by f
    # -------------------------------------------------------
    data_map = {} # data_map[f] = [ (r, dnll), ... ]
    
    print(f"Reading {n_entries} entries...")
    for i in range(n_entries):
        t.GetEntry(i)
        
        # Rounding f to handle float precision issues
        f_key = round(t.f, 6)
        r_val = t.r
        dnll2 = 2 * t.deltaNLL
        
        if f_key not in data_map:
            data_map[f_key] = []
        
        data_map[f_key].append( (r_val, dnll2) )

    # -------------------------------------------------------
    # 2. Linear Interpolation for each f
    # -------------------------------------------------------
    sorted_f_keys = sorted(data_map.keys())
    
    results = [] # (f, interp_r)

    print("\n" + "="*50)
    print(f" RESULTS: Interpolated Limit at 2*dNLL = {TARGET_CUT}")
    print("="*50)
    print(f"{'f':^10} | {'Interp Limit r':^15}")
    print("-" * 50)

    for f in sorted_f_keys:
        points = data_map[f]
        # Sort points by r to find the crossing interval
        points.sort(key=lambda p: p[0]) 
        
        found_crossing = False
        interp_r = -1.0
        
        # Iterate to find where 2*dNLL crosses TARGET_CUT (5.99)
        # We look for a pair of points (i, i+1) where one is below and one is above 5.99
        for i in range(len(points) - 1):
            r1, nll1 = points[i]
            r2, nll2 = points[i+1]
            
            # Check for crossing (one below, one above)
            # Case A: Rising NLL (Typical limit) -> nll1 < 5.99 < nll2
            # Case B: Falling NLL (Should not happen for simple limit) -> nll1 > 5.99 > nll2
            if (nll1 < TARGET_CUT < nll2) or (nll1 > TARGET_CUT > nll2):
                # Linear Interpolation Formula:
                # r = r1 + (r2 - r1) * (Target - nll1) / (nll2 - nll1)
                slope = (r2 - r1) / (nll2 - nll1)
                interp_r = r1 + slope * (TARGET_CUT - nll1)
                
                results.append((f, interp_r))
                found_crossing = True
                print(f"{f:10.4f} | {interp_r:15.4f}")
                
                # We typically take the first crossing as the limit.
                # If there are multiple (islands), logic needs to be more complex.
                break 
        
        if not found_crossing:
            # If no crossing found (e.g., all points are below limit), skip or flag
            pass

    # -------------------------------------------------------
    # 3. Save to CSV
    # -------------------------------------------------------
    with open(OUTPUT_CSV, "w", newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["f", "interpolated_limit_r", "Target_Cut"])
        
        for row in results:
            writer.writerow([row[0], row[1], TARGET_CUT])
            
    print("-" * 50)
    print(f"\nSaved interpolated limits to '{OUTPUT_CSV}'")

if __name__ == "__main__":
    scan_limit_interpolated()
