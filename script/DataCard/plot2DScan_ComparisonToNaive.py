import ROOT
import array
import csv
import sys
import math

# ------------------------------------------------------------------------------
# 1. Enable Batch Mode
# ------------------------------------------------------------------------------
ROOT.gROOT.SetBatch(True)

# ------------------------------------------------------------------------------
# Configuration
# ------------------------------------------------------------------------------
INPUT_FILE = "higgsCombine_Run2_EMu_M10000_syst_grid_2D_Asimov_r0f0.5.MultiDimFit.mH120.root" 
#INPUT_FILE = "higgsCombine_grid_2D_expected.MultiDimFit.mH120.root" 
CSV_FILE   = "limit_points_interpolated.csv" # <-- Input CSV filename
TREE_NAME  = "limit"

X_TITLE = "Flavor fraction f = |V_{e}|^{2} / (|V_{e}|^{2} + |V_{#mu}|^{2})"
Y_TITLE = "Mixing strength r = |V_{e}|^{2} + |V_{#mu}|^{2}"
Z_TITLE = "-2 #Delta ln L"

DISPLAY_CUT = 10
OUT_NAME = "HNL_CompareToNaive"

# ==============================================================================
# [USER INPUT REQUIRED] Reference Point Configuration
# ==============================================================================
# Set the 'f' value you want to use as a reference anchor.
# The script will look up the corresponding 'r' from the CSV file.
REF_F_VAL = 0.5   # <--- CHANGE THIS (e.g., 0.1, 0.5, 0.01)

# ------------------------------------------------------------------------------
# Style Settings
# ------------------------------------------------------------------------------
def set_style():
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)
    ROOT.gStyle.SetLineWidth(2)
    font = 42
    ROOT.gStyle.SetTextFont(font)
    ROOT.gStyle.SetLabelFont(font, "XYZ")
    ROOT.gStyle.SetTitleFont(font, "XYZ")
    ROOT.gStyle.SetPalette(ROOT.kBird)
    ROOT.gStyle.SetPadLeftMargin(0.16)
    ROOT.gStyle.SetPadRightMargin(0.18)
    ROOT.gStyle.SetPadBottomMargin(0.14)
    ROOT.gStyle.SetPadTopMargin(0.08)

# ------------------------------------------------------------------------------
# Helper: Fix Empty Bins
# ------------------------------------------------------------------------------
def fill_empty_bins(hist, fill_value=9999.0):
    nx = hist.GetNbinsX()
    ny = hist.GetNbinsY()
    for i in range(1, nx + 1):
        for j in range(1, ny + 1):
            if hist.GetBinEntries(hist.GetBin(i, j)) == 0:
                hist.SetBinContent(i, j, fill_value)
    return hist

# ------------------------------------------------------------------------------
# Helper: Get Reference 'r' from CSV
# ------------------------------------------------------------------------------
def get_r_from_csv(csv_path, target_f):
    """
    Reads the CSV file and finds the row with 'f' closest to target_f.
    Returns the corresponding 'interpolated_limit_r'.
    """
    print(f"Reading reference limit from {csv_path}...")
    
    best_diff = 9999.0
    found_r = None
    found_f = None
    
    try:
        with open(csv_path, 'r') as f:
            reader = csv.reader(f)
            header = next(reader, None) # Skip header
            
            for row in reader:
                try:
                    # CSV Format: f, interpolated_limit_r, Target_Cut
                    f_val = float(row[0])
                    r_val = float(row[1])
                    
                    diff = abs(f_val - target_f)
                    
                    if diff < best_diff:
                        best_diff = diff
                        found_r = r_val
                        found_f = f_val
                except ValueError:
                    continue # Skip bad rows
                    
    except FileNotFoundError:
        print(f"Error: CSV file '{csv_path}' not found!")
        sys.exit(1)

    if found_r is None:
        print(f"Error: Could not extract any valid data from {csv_path}")
        sys.exit(1)
        
    print(f"  -> Requested f_ref: {target_f}")
    print(f"  -> Closest f found: {found_f} (diff: {best_diff:.6f})")
    print(f"  -> Retrieved r_ref: {found_r}")
    
    return found_r

# ------------------------------------------------------------------------------
# Main Comparison Function
# ------------------------------------------------------------------------------
def draw_comparison():
    set_style()
    
    # 1. Get Reference R automatically
    ref_r_val = get_r_from_csv(CSV_FILE, REF_F_VAL)
    
    # 2. Open ROOT File
    f = ROOT.TFile.Open(INPUT_FILE)
    if not f:
        print(f"Error: Cannot open {INPUT_FILE}")
        return
    t = f.Get(TREE_NAME)

    c = ROOT.TCanvas("c", "c", 800, 700)
    
    # 3. Background Heatmap
    t.Draw(f"2*deltaNLL:r:f>>h_color(50,0,1,50,0,2)", f"2*deltaNLL < {DISPLAY_CUT}", "prof goff")
    h_color = ROOT.gDirectory.Get("h_color")
    
    h_color.SetTitle("")
    h_color.GetXaxis().SetTitle(X_TITLE)
    h_color.GetYaxis().SetTitle(Y_TITLE)
    h_color.GetZaxis().SetTitle(Z_TITLE)
    h_color.GetXaxis().SetTitleOffset(1.1)
    h_color.GetYaxis().SetTitleOffset(1.3)
    h_color.GetZaxis().SetTitleOffset(1.1)
    h_color.SetMaximum(DISPLAY_CUT)
    h_color.SetMinimum(0.0)
    h_color.Draw("COLZ")

    # 4. Draw Real Contours (1D and 2D definitions)
    t.Draw("2*deltaNLL:r:f>>h_cont(50,0,1,50,0,2)", "", "prof goff")
    h_cont = ROOT.gDirectory.Get("h_cont")
    fill_empty_bins(h_cont, 9999.0)
    
    # A. Draw 1D 95% CL (2dNLL = 3.84) if you want
    #h_1d = h_cont.Clone("h_1d")
    #h_1d.SetContour(1, array.array('d', [3.84]))
    #h_1d.SetLineColor(ROOT.kGray+2)
    #h_1d.SetLineWidth(2)
    #h_1d.SetLineStyle(2) # Dashed
    #h_1d.Draw("CONT3 SAME")

    # B. Draw 2D 95% CL (2dNLL = 5.99) -> Main Result
    h_2d = h_cont.Clone("h_2d")
    h_2d.SetContour(1, array.array('d', [5.99]))
    h_2d.SetLineColor(ROOT.kBlack)
    h_2d.SetLineWidth(3)
    h_2d.SetLineStyle(1) # Solid
    h_2d.Draw("CONT3 SAME")

    # 5. Draw Naive Scaling Curve (Auto-Calculated)
    # Logic: constant K = r_ref * f_ref * (1 - f_ref)
    DYVBF_scaling_const = ref_r_val * REF_F_VAL * (1.0 - REF_F_VAL)
    
    DYVBF_naive_func = ROOT.TF1("DYVBF_naive_func", f"{DYVBF_scaling_const} / (x * (1.0 - x))", 0.05, 0.95)
    
    # Style: Red Long-Dash
    DYVBF_naive_func.SetLineColor(ROOT.kRed)
    DYVBF_naive_func.SetLineWidth(3)
    DYVBF_naive_func.SetLineStyle(7) 
    DYVBF_naive_func.Draw("SAME")

    # Logic: constant K = r_ref * sqrt(f_ref * (1 - f_ref))
    SSWW_scaling_const = ref_r_val * math.sqrt(REF_F_VAL * (1.0 - REF_F_VAL))

    SSWW_naive_func = ROOT.TF1("SSWW_naive_func", f"{SSWW_scaling_const} / sqrt( (x * (1.0 - x)) )", 0.05, 0.95)
    
    # Style: Blue Dash
    SSWW_naive_func.SetLineColor(ROOT.kBlue)
    SSWW_naive_func.SetLineWidth(2)
    SSWW_naive_func.SetLineStyle(7) 
    SSWW_naive_func.Draw("SAME") # Decided not to draw, not to add confusion to the audience.

    # 6. Legend
    leg = ROOT.TLegend(0.40, 0.75, 0.82, 0.88)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg.SetTextSize(0.028)
    
    l_2d = ROOT.TLine(); l_2d.SetLineColor(ROOT.kBlack); l_2d.SetLineWidth(3); l_2d.SetLineStyle(1)
    #l_1d = ROOT.TLine(); l_1d.SetLineColor(ROOT.kGray+2); l_1d.SetLineWidth(2); l_1d.SetLineStyle(2)
    DYVBF_l_naive = ROOT.TLine(); DYVBF_l_naive.SetLineColor(ROOT.kRed); DYVBF_l_naive.SetLineWidth(3); DYVBF_l_naive.SetLineStyle(7)
    #SSWW_l_naive = ROOT.TLine(); SSWW_l_naive.SetLineColor(ROOT.kBlue); SSWW_l_naive.SetLineWidth(2); SSWW_l_naive.SetLineStyle(7)
    
    leg.AddEntry(l_2d, "2D Scan (95% CL, 5.99)", "l")
    #leg.AddEntry(l_1d, "Real 2D Scan (1D-def 95%, 3.84)", "l")
    # Legend still shows user's REF_F_VAL, even if actual used point was slightly different
    leg.AddEntry(DYVBF_l_naive, f"Naive Scaling (Ref: f={REF_F_VAL})", "l")
    #leg.AddEntry(DYVBF_l_naive, f"DYVBF Scaling (Ref: f={REF_F_VAL})", "l")
    #leg.AddEntry(SSWW_l_naive, f"SSWW Scaling (Ref: f={REF_F_VAL})", "l")
    leg.Draw()

    # 7. Labels
    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextFont(61); latex.SetTextSize(0.05)
    latex.DrawLatex(0.16, 0.93, "CMS")
    
    latex.SetTextFont(52); latex.SetTextSize(0.04)
    latex.DrawLatex(0.26, 0.93, "Simulation Preliminary")
    
    latex.SetTextFont(42); latex.SetTextSize(0.035)
    latex.DrawLatex(0.65, 0.93, "138 fb^{-1} (13 TeV)")

    c.SaveAs(f"{OUT_NAME}.pdf")
    c.SaveAs(f"{OUT_NAME}.png")
    print(f"Comparison plot saved to {OUT_NAME}.pdf")

if __name__ == "__main__":
    draw_comparison()
