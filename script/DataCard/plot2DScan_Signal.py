import ROOT
import array
import sys

# ------------------------------------------------------------------------------
# 1. Environment & Style Setup (Publication Quality)
# ------------------------------------------------------------------------------
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetPalette(ROOT.kBird)

# [FIX] Thicker Lines & Ticks on all sides
ROOT.gStyle.SetLineWidth(2)        # General line thickness
ROOT.gStyle.SetFrameLineWidth(3)   # Thick frame border
ROOT.gStyle.SetPadTickX(1)         # Ticks on Top
ROOT.gStyle.SetPadTickY(1)         # Ticks on Right

# ------------------------------------------------------------------------------
# [User Configuration]
# ------------------------------------------------------------------------------
#INPUT_FILE = "higgsCombine_Run2_3ch_M1000_syst_grid_2D_Asimov_r1f0.5.MultiDimFit.mH120.root"
#INPUT_FILE = "higgsCombine_grid_2D.MultiDimFit.mH120.root"
INPUT_FILE = "higgsCombine_Run2_EMu_M100_syst_grid_2D_Asimov_r1f0.5.MultiDimFit.mH120.root"

X_TITLE = "Flavor fraction f_{e} = |V_{e}|^{2} / (|V_{e}|^{2} + |V_{#mu}|^{2})"
Y_TITLE = "Mixing strength r = |V_{e}|^{2} + |V_{#mu}|^{2}"
Z_TITLE = "-2 #Delta ln L"

# Visualization Settings
DISPLAY_CUT = 30     # Values > 10 will be white
X_MIN, X_MAX = 0.0, 1.0
Y_MIN, Y_MAX = 0.0, 2.0 
NBINS = 50

# Contour Levels
CONTOUR_LEVELS = [2.30, 5.99] 
LINE_STYLES    = [1,    2]    
LINE_WIDTHS    = [2,    2]

# ------------------------------------------------------------------------------
# 2. Load Data & Find Best Fit
# ------------------------------------------------------------------------------
f_in = ROOT.TFile.Open(INPUT_FILE)
if not f_in or f_in.IsZombie():
    print(f"ERROR: Cannot open file {INPUT_FILE}")
    sys.exit(1)

tree = f_in.Get("limit")
if not tree:
    print("ERROR: Cannot find tree 'limit'")
    sys.exit(1)

# Initialize Graph
g2d = ROOT.TGraph2D()
n_points = tree.GetEntries()
print(f"Processing {n_points} points...")

# Variables to track Best Fit Point
min_nll = 99999.0
best_r  = 0.0
best_f  = 0.0

# Step 1: Fill Data & Find Best Fit
for i, ev in enumerate(tree):
    dnll = 2 * ev.deltaNLL
    if dnll < 0: dnll = 0
    
    if dnll < min_nll:
        min_nll = dnll
        best_r  = ev.r
        best_f  = ev.f
        
    g2d.SetPoint(i, ev.f, ev.r, dnll)

print(f"Best Fit Point found at: f={best_f:.3f}, r={best_r:.4f} (2dNLL={min_nll:.4f})")

# Step 2: Set Histogram Frame
h_frame = ROOT.TH2D("h_frame", "", NBINS, X_MIN, X_MAX, NBINS, Y_MIN, Y_MAX)
g2d.SetHistogram(h_frame)

# Step 3: Get Raw Histogram
h_raw = g2d.GetHistogram()
h_raw.SetTitle("")
h_raw.GetXaxis().SetTitle(X_TITLE)
h_raw.GetXaxis().SetTitleOffset(1.2)
h_raw.GetYaxis().SetTitle(Y_TITLE)
h_raw.GetZaxis().SetTitle(Z_TITLE)

# Step 4: Create Masked Histogram
h_draw = h_raw.Clone("h_draw")
for ix in range(1, h_draw.GetNbinsX() + 1):
    for iy in range(1, h_draw.GetNbinsY() + 1):
        content = h_draw.GetBinContent(ix, iy)
        if content > DISPLAY_CUT:
            h_draw.SetBinContent(ix, iy, -1.0) 

h_draw.SetMinimum(0.0)
h_draw.SetMaximum(DISPLAY_CUT)

# ------------------------------------------------------------------------------
# 3. Extract Clean Contours
# ------------------------------------------------------------------------------
print("Extracting clean contours...")

c_temp = ROOT.TCanvas("c_temp", "temp", 0, 0, 500, 500)
c_temp.cd()

final_contours = [] 

for level, style, width in zip(CONTOUR_LEVELS, LINE_STYLES, LINE_WIDTHS):
    h_temp = h_raw.Clone(f"h_temp_{level}")
    h_temp.SetContour(1, array.array('d', [level]))
    h_temp.Draw("CONT LIST")
    c_temp.Update()
    
    specials = ROOT.gROOT.GetListOfSpecials()
    contours = specials.FindObject("contours")
    
    if contours:
        cnt_list = contours.At(0)
        cur_graph = cnt_list.First()
        while cur_graph:
            if cur_graph.GetN() > 20: 
                gc = cur_graph.Clone()
                gc.SetLineColor(ROOT.kGray+3)
                gc.SetLineStyle(style)
                gc.SetLineWidth(width)
                final_contours.append(gc)
            cur_graph = cnt_list.After(cur_graph)
    h_temp.Delete()

del c_temp

# ------------------------------------------------------------------------------
# 4. Create Best Fit Marker
# ------------------------------------------------------------------------------
g_best = ROOT.TGraph()
g_best.SetPoint(0, best_f, best_r)
g_best.SetMarkerStyle(33) 
g_best.SetMarkerSize(2.5)
g_best.SetMarkerColor(ROOT.kGray+3)

# ------------------------------------------------------------------------------
# 5. Final Plotting
# ------------------------------------------------------------------------------
c1 = ROOT.TCanvas("c1", "Signal Injection Scan", 800, 750)

# Margins
top_margin = 0.08
right_margin = 0.17
c1.SetTopMargin(top_margin)
c1.SetBottomMargin(0.12)
c1.SetLeftMargin(0.12)
c1.SetRightMargin(right_margin)

c1.cd()

# (1) Draw Background
h_draw.Draw("colz")

# (2) Draw Contours
for gc in final_contours:
    gc.Draw("C SAME")

# (3) Draw Best Fit Point
g_best.Draw("P SAME")

# (4) Legend
# [FIX] Increased text size and adjusted position slightly
leg = ROOT.TLegend(0.43, 0.76, 0.77, 0.89)
leg.SetBorderSize(0)
leg.SetFillStyle(0)
leg.SetTextSize(0.032) # Bigger text

dummy_68 = ROOT.TLine(); dummy_68.SetLineColor(ROOT.kGray+3); dummy_68.SetLineWidth(2); dummy_68.SetLineStyle(1)
dummy_95 = ROOT.TLine(); dummy_95.SetLineColor(ROOT.kGray+3); dummy_95.SetLineWidth(2); dummy_95.SetLineStyle(2)
dummy_bf = ROOT.TMarker(); dummy_bf.SetMarkerStyle(33); dummy_bf.SetMarkerColor(ROOT.kGray+3); dummy_bf.SetMarkerSize(1.5)

leg.AddEntry(dummy_bf, f"Best Fit: (r={best_r:.2f}, f_{{e}}={best_f:.2f})", "p")
leg.AddEntry(dummy_68, "68% CL (1#sigma)", "l")
leg.AddEntry(dummy_95, "95% CL (2#sigma)", "l")
leg.Draw()

# (5) CMS Labels & Lumi
latex = ROOT.TLatex()
latex.SetNDC()

# CMS Logo
latex.SetTextFont(61)
latex.SetTextSize(0.05)
latex.DrawLatex(0.12, 1.0 - top_margin + 0.01, "CMS") # Just above the frame

# Work in Progress
latex.SetTextFont(52)
latex.SetTextSize(0.04)
latex.DrawLatex(0.23, 1.0 - top_margin + 0.01, "Work in Progress")

# [FIX] Lumi & Energy Info (Right aligned to the frame end)
latex.SetTextFont(42)
latex.SetTextSize(0.035)
latex.SetTextAlign(31) # Right-bottom alignment
# x-pos = 1.0 - right_margin (Right edge of the frame)
latex.DrawLatex(1.0 - right_margin, 1.0 - top_margin + 0.01, "138 fb^{-1} (13 TeV)")

c1.Update()
c1.SaveAs("HNL_Signal_Injection.png")
c1.SaveAs("HNL_Signal_Injection.pdf")

print("Done! Check HNL_Signal_Injection.png")
