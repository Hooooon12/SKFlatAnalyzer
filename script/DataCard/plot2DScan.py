import ROOT
import array

# ------------------------------------------------------------------------------
# 1. Enable Batch Mode
#    - Prevents the canvas window from popping up during execution.
#    - Essential for running on remote clusters or via SSH.
# ------------------------------------------------------------------------------
ROOT.gROOT.SetBatch(True)

# ------------------------------------------------------------------------------
# Configuration
# ------------------------------------------------------------------------------
#INPUT_FILE = "higgsCombine_grid_2D.MultiDimFit.mH120.root" # <-- Update with your actual filename
INPUT_FILE = "higgsCombine_grid_2D_expected.MultiDimFit.mH120.root" # <-- Update with your actual filename
TREE_NAME  = "limit"

# Axis Titles (LaTeX format supported)
X_TITLE = "Flavor fraction f = |V_{e}|^{2} / (|V_{e}|^{2} + |V_{#mu}|^{2})"
Y_TITLE = "Mixing strength r = |V_{e}|^{2} + |V_{#mu}|^{2}"
Z_TITLE = "-2 #Delta ln L"

# ** Visualization Cut **
# Regions where 2*deltaNLL > DISPLAY_CUT will appear WHITE (empty).
# This provides a clean background for excluded regions.
DISPLAY_CUT = 10

# Output filename prefix
OUT_NAME = "HNL_2D_Scan_Result"

# ------------------------------------------------------------------------------
# Style Settings
# ------------------------------------------------------------------------------
def set_style():
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)
    ROOT.gStyle.SetLineWidth(2)
    
    # Font settings (Helvetica = 42 is standard for CMS)
    font = 42
    ROOT.gStyle.SetTextFont(font)
    ROOT.gStyle.SetLabelFont(font, "XYZ")
    ROOT.gStyle.SetTitleFont(font, "XYZ")
    
    # Palette: kBird (Blue to Yellow) - Modern and color-blind friendly
    ROOT.gStyle.SetPalette(ROOT.kBird)

    # Margins (tuned to fit axis labels and color bar)
    ROOT.gStyle.SetPadLeftMargin(0.16)
    ROOT.gStyle.SetPadRightMargin(0.18)
    ROOT.gStyle.SetPadBottomMargin(0.14)
    ROOT.gStyle.SetPadTopMargin(0.08)

# ------------------------------------------------------------------------------
# Helper Function: Fix Empty Bins
# ------------------------------------------------------------------------------
def fill_empty_bins(hist, fill_value=9999.0):
    """
    Iterates through all bins in the histogram.
    If a bin has 0 entries (meaning it was not scanned), fills it with a 
    high value (fill_value) instead of the default 0.
    
    Why this is needed:
        - Unscanned regions (0 entries) default to 0.
        - 0 implies 'Best Fit' in NLL scans.
        - This causes contours to wrap around the scan boundaries.
        - Filling with 9999 ensures contours remain open at the boundary.
    """
    nx = hist.GetNbinsX()
    ny = hist.GetNbinsY()
    
    for i in range(1, nx + 1):
        for j in range(1, ny + 1):
            if hist.GetBinEntries(hist.GetBin(i, j)) == 0:
                hist.SetBinContent(i, j, fill_value)
    return hist

# ------------------------------------------------------------------------------
# Main Plotting Function
# ------------------------------------------------------------------------------
def draw_final_scan():
    set_style()
    
    # Load File
    f = ROOT.TFile.Open(INPUT_FILE)
    if not f:
        print(f"Error: Cannot open {INPUT_FILE}")
        return
    t = f.Get(TREE_NAME)

    c = ROOT.TCanvas("c", "c", 800, 700)
    
    # ==========================================================================
    # 1. Create Heatmap Histogram (Background)
    #    - Apply cut (2*deltaNLL < DISPLAY_CUT).
    #    - High NLL regions become empty (White background).
    # ==========================================================================
    cmd_color = "2*deltaNLL:r:f>>h_color(50,0,1,50,0,2)"
    cut_color = f"2*deltaNLL < {DISPLAY_CUT}" 
    
    # 'prof' option creates a TProfile2D to average bin contents
    t.Draw(cmd_color, cut_color, "prof goff")
    h_color = ROOT.gDirectory.Get("h_color")
    
    # Style the Heatmap
    h_color.SetTitle("")
    h_color.GetXaxis().SetTitle(X_TITLE)
    h_color.GetYaxis().SetTitle(Y_TITLE)
    h_color.GetZaxis().SetTitle(Z_TITLE)
    
    h_color.GetXaxis().SetTitleOffset(1.1)
    h_color.GetYaxis().SetTitleOffset(1.3)
    h_color.GetZaxis().SetTitleOffset(1.1)
    
    # Set Color Range (Matches the Cut)
    h_color.SetMaximum(DISPLAY_CUT)
    h_color.SetMinimum(0.0)
    
    # Draw Heatmap (Base Layer)
    h_color.Draw("COLZ") 

    # ==========================================================================
    # 2. Create Contour Histogram (Calculation)
    #    - NO CUT is applied here (""). We need the full NLL landscape.
    #    - This ensures contours don't close artificially at the cut boundary.
    # ==========================================================================
    t.Draw("2*deltaNLL:r:f>>h_cont(50,0,1,50,0,2)", "", "prof goff")
    h_cont = ROOT.gDirectory.Get("h_cont")
    
    # Fix empty bins (unscanned areas) to prevent ghost contours
    fill_empty_bins(h_cont, 9999.0)
    
    # --------------------------------------------------------------------------
    # 3. Draw Contours
    #    - Calculated from 'h_cont' (full data)
    #    - Drawn on top of 'h_color' (white background)
    # --------------------------------------------------------------------------
    
    # Line Color: Dark Grey or Black (Visible on both Blue and White)
    line_color = ROOT.kGray+3 
    
    # 1 sigma (68% CL)
    h_1sig = h_cont.Clone("h_1sig")
    h_1sig.SetContour(1, array.array('d', [2.30]))
    h_1sig.SetLineColor(line_color)
    h_1sig.SetLineWidth(2)
    h_1sig.SetLineStyle(1) # Solid
    h_1sig.Draw("CONT3 SAME")
    
    # 2 sigma (95% CL)
    h_2sig = h_cont.Clone("h_2sig")
    h_2sig.SetContour(1, array.array('d', [5.99]))
    h_2sig.SetLineColor(line_color)
    h_2sig.SetLineWidth(2)
    h_2sig.SetLineStyle(2) # Dashed
    h_2sig.Draw("CONT3 SAME")

    # ==========================================================================
    # 4. Legend & CMS Labels
    # ==========================================================================
    # Create dummy line objects for the legend
    l1 = ROOT.TLine()
    l1.SetLineColor(line_color)
    l1.SetLineWidth(2)
    l1.SetLineStyle(1)
    
    l2 = ROOT.TLine()
    l2.SetLineColor(line_color)
    l2.SetLineWidth(2)
    l2.SetLineStyle(2)
    
    # Legend Position
    leg = ROOT.TLegend(0.55, 0.78, 0.80, 0.88) 
    leg.SetBorderSize(0)
    leg.SetFillStyle(0) # Transparent
    leg.SetTextSize(0.03)
    leg.SetTextColor(ROOT.kBlack) # Black text ensures visibility on white bg
    
    leg.AddEntry(l1, "68% CL (1#sigma)", "l")
    leg.AddEntry(l2, "95% CL (2#sigma)", "l")
    leg.Draw()

    # Standard CMS Labels
    latex = ROOT.TLatex()
    latex.SetNDC()
    
    # CMS Logo
    latex.SetTextFont(61)
    latex.SetTextSize(0.05)
    latex.DrawLatex(0.16, 0.93, "CMS")
    
    # Status Label
    latex.SetTextFont(52)
    latex.SetTextSize(0.04)
    latex.DrawLatex(0.26, 0.93, "Work in Progress")
    
    # Luminosity Label
    latex.SetTextFont(42)
    latex.SetTextSize(0.035)
    latex.DrawLatex(0.65, 0.93, "138 fb^{-1} (13 TeV)")

    # Save Output
    c.SaveAs(f"{OUT_NAME}.pdf")
    c.SaveAs(f"{OUT_NAME}.png")
    print(f"Done. Plots saved to {OUT_NAME}.pdf / .png")

if __name__ == "__main__":
    draw_final_scan()
