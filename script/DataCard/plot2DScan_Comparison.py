import ROOT
import array
import math
import sys

# ------------------------------------------------------------------------------
# 1. Environment Setup
# ------------------------------------------------------------------------------
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetPalette(ROOT.kBird) # Color palette

# [FIX] Thicker Lines & Ticks on all sides
ROOT.gStyle.SetLineWidth(2)        # General line thickness
ROOT.gStyle.SetFrameLineWidth(3)   # Thick frame border
ROOT.gStyle.SetPadTickX(1)         # Ticks on Top
ROOT.gStyle.SetPadTickY(1)         # Ticks on Right

# ------------------------------------------------------------------------------
# [User Configuration] File Names and Settings
# ------------------------------------------------------------------------------
INPUT_FILE = "higgsCombine_Run2_3ch_M10000_syst_grid_2D_Asimov_r0f0.5.MultiDimFit.mH120.root" 

X_TITLE = "Flavor fraction f = |V_{e}|^{2} / (|V_{e}|^{2} + |V_{#mu}|^{2})"
Y_TITLE = "Mixing strength r = |V_{e}|^{2} + |V_{#mu}|^{2}"
Z_TITLE = "-2 #Delta ln L"

REF_F_VAL   = 0.5   # Reference point for Naive Scaling
CL_VAL      = 5.99  # 95% CL (2 DOF), Use 3.84 for 1D
DISPLAY_CUT = 10    # Values above this will be WHITE in the background

# Fixed Axis Ranges
X_MIN, X_MAX = 0.0, 1.0
Y_MIN, Y_MAX = 0.0, 2.0 
NBINS = 50

# ------------------------------------------------------------------------------
# 2. Load Data & Create 2D Histogram
# ------------------------------------------------------------------------------
f_in = ROOT.TFile.Open(INPUT_FILE)
if not f_in or f_in.IsZombie():
    print(f"ERROR: Cannot open file {INPUT_FILE}")
    sys.exit(1)

tree = f_in.Get("limit")
if not tree:
    print("ERROR: Cannot find tree 'limit'")
    sys.exit(1)

# Step 1: Initialize TGraph2D
g2d = ROOT.TGraph2D()
n_points = tree.GetEntries()
print(f"Processing {n_points} points...")

# Step 2: Fill data FIRST (Prevents SegFault)
for i, ev in enumerate(tree):
    dnll = 2 * ev.deltaNLL 
    if dnll < 0: dnll = 0
    g2d.SetPoint(i, ev.f, ev.r, dnll)

# Step 3: Define the histogram frame and set it to TGraph2D
h_frame = ROOT.TH2D("h_frame", "", NBINS, X_MIN, X_MAX, NBINS, Y_MIN, Y_MAX)
g2d.SetHistogram(h_frame) 

# Step 4: Get the interpolated histogram (Pure Physics Data)
h_raw = g2d.GetHistogram() 
h_raw.SetTitle("")
h_raw.GetXaxis().SetTitle(X_TITLE)
h_raw.GetXaxis().SetTitleOffset(1.2)
h_raw.GetYaxis().SetTitle(Y_TITLE)
h_raw.GetZaxis().SetTitle(Z_TITLE)

# Step 5: Create a separate histogram for BACKGROUND DRAWING
h_draw = h_raw.Clone("h_draw")

# Apply masking ONLY to the drawing histogram (White background for high values)
for ix in range(1, h_draw.GetNbinsX() + 1):
    for iy in range(1, h_draw.GetNbinsY() + 1):
        content = h_draw.GetBinContent(ix, iy)
        if content > DISPLAY_CUT:
            h_draw.SetBinContent(ix, iy, -1.0) # Set to negative (Underflow -> White)

h_draw.SetMinimum(0.0)
h_draw.SetMaximum(DISPLAY_CUT)

# ------------------------------------------------------------------------------
# 3. Extract Clean Contours (From h_raw)
# ------------------------------------------------------------------------------
print("Extracting clean contours...")

c_temp = ROOT.TCanvas("c_temp", "temp", 0, 0, 500, 500)
c_temp.cd()

h_contour_calc = h_raw.Clone("h_contour_calc")
h_contour_calc.SetContour(1, array.array('d', [CL_VAL])) 
h_contour_calc.Draw("CONT LIST") 
c_temp.Update()

specials = ROOT.gROOT.GetListOfSpecials()
contours = specials.FindObject("contours")

final_contours = []

if contours:
    cnt_list = contours.At(0)
    cur_graph = cnt_list.First()
    
    while cur_graph:
        # Noise filter
        if cur_graph.GetN() > 20: 
            gc = cur_graph.Clone()
            gc.SetLineColor(ROOT.kBlack)
            gc.SetLineWidth(3)
            final_contours.append(gc)
        cur_graph = cnt_list.After(cur_graph)

del c_temp 

# ------------------------------------------------------------------------------
# 4. Calculate Naive Scaling Curves (DYVBF & SSWW)
# ------------------------------------------------------------------------------
ref_r_limit = 0.0
bin_x = h_raw.GetXaxis().FindBin(REF_F_VAL)
found_ref = False

for y_bin in range(1, h_raw.GetNbinsY()):
    val_curr = h_raw.GetBinContent(bin_x, y_bin)
    val_prev = h_raw.GetBinContent(bin_x, y_bin-1)
    
    if val_curr > CL_VAL and val_prev <= CL_VAL:
        r_curr = h_raw.GetYaxis().GetBinCenter(y_bin)
        r_prev = h_raw.GetYaxis().GetBinCenter(y_bin-1)
        fraction = (CL_VAL - val_prev) / (val_curr - val_prev + 1e-9)
        ref_r_limit = r_prev + fraction * (r_curr - r_prev)
        found_ref = True
        break

if not found_ref:
    print(f"WARNING: Could not find limit at f={REF_F_VAL}. Using fallback.")
    ref_r_limit = 1.0 

print(f"Reference Limit at f={REF_F_VAL}: r = {ref_r_limit:.4f}")

g_naive_dy   = ROOT.TGraph() 
g_naive_ssww = ROOT.TGraph() 

n_naive = 0
for f_val in [i*0.01 for i in range(1, 100)]: 
    eff_ratio = (REF_F_VAL * (1 - REF_F_VAL)) / (f_val * (1 - f_val))
    
    r_dy   = ref_r_limit * eff_ratio
    r_ssww = ref_r_limit * math.sqrt(eff_ratio)
    
    # Check bounds (1.1x buffer to let line exit cleanly)
    if r_dy < Y_MAX * 1.1:
        g_naive_dy.SetPoint(g_naive_dy.GetN(), f_val, r_dy)
        
    if r_ssww < Y_MAX * 1.1:
        g_naive_ssww.SetPoint(g_naive_ssww.GetN(), f_val, r_ssww)

g_naive_dy.SetLineColor(ROOT.kRed)
g_naive_dy.SetLineWidth(3)
g_naive_dy.SetLineStyle(7) 

g_naive_ssww.SetLineColor(ROOT.kBlue)
g_naive_ssww.SetLineWidth(3)
g_naive_ssww.SetLineStyle(7) 

# ------------------------------------------------------------------------------
# 5. Final Plotting
# ------------------------------------------------------------------------------
c1 = ROOT.TCanvas("c1", "HNL 2D Scan", 800, 750) # Height increased slightly

# [FIX] Set margins explicitly to control text position
top_margin = 0.07
right_margin = 0.17
c1.SetTopMargin(top_margin)    # Top space
c1.SetBottomMargin(0.12) # X-axis title space
c1.SetLeftMargin(0.12)   # Y-axis title space
c1.SetRightMargin(right_margin)  # Z-axis (Color bar) space

c1.cd()

# (1) Draw Background
h_draw.Draw("colz")

# (2) Draw Contours
for gr in final_contours:
    gr.Draw("C SAME")

# (3) Draw Naive Curves
g_naive_dy.Draw("L SAME")
g_naive_ssww.Draw("L SAME")

# Legend
leg = ROOT.TLegend(0.45, 0.75, 0.82, 0.88)
leg.SetBorderSize(0)
leg.SetFillStyle(0)
leg.SetTextSize(0.025)

dummy_real = ROOT.TLine(); dummy_real.SetLineColor(ROOT.kBlack); dummy_real.SetLineWidth(3)
dummy_dy   = ROOT.TLine(); dummy_dy.SetLineColor(ROOT.kRed);     dummy_dy.SetLineWidth(3); dummy_dy.SetLineStyle(7)
dummy_ssww = ROOT.TLine(); dummy_ssww.SetLineColor(ROOT.kBlue);  dummy_ssww.SetLineWidth(3); dummy_ssww.SetLineStyle(7)

leg.AddEntry(dummy_real, "2D Limit (95% CL)", "l")
leg.AddEntry(dummy_dy,   f"Naive DY+W#gamma (Ref f={REF_F_VAL})", "l")
leg.AddEntry(dummy_ssww, f"Naive SSWW (Ref f={REF_F_VAL})", "l")
leg.Draw()

# [FIX] CMS Label - Bigger and closer to the frame
latex = ROOT.TLatex()
latex.SetNDC()

# 1. CMS Logo
latex.SetTextFont(61)
latex.SetTextSize(0.05) # Increased from 0.04
latex.DrawLatex(0.12, 0.94, "CMS") # Aligned with Left Margin (0.12), Just above frame

# 2. Extra Text
latex.SetTextFont(52)
latex.SetTextSize(0.04) # Increased from 0.03
latex.DrawLatex(0.23, 0.94, "Work in Progress") # Placed next to CMS

# [FIX] Lumi & Energy Info (Right aligned to the frame end)
latex.SetTextFont(42)
latex.SetTextSize(0.035)
latex.SetTextAlign(31) # Right-bottom alignment
# x-pos = 1.0 - right_margin (Right edge of the frame)
latex.DrawLatex(1.0 - right_margin, 1.0 - top_margin + 0.01, "138 fb^{-1} (13 TeV)")

# Redraw axes to avoid color overlaid on the axes
border = ROOT.TBox(X_MIN, Y_MIN, X_MAX, Y_MAX)
border.SetFillStyle(0)   # An empty box
border.SetLineWidth(3)   # Assign the same line width with the axes
border.SetLineColor(ROOT.kBlack)
border.Draw("l same")    # l: Draw line only

c1.Update()
c1.SaveAs("HNL_2D_Scan_Comp.png")
c1.SaveAs("HNL_2D_Scan_Comp.pdf")

print("Done! Check HNL_2D_Scan_Comp.png")
