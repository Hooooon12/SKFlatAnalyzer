#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraph2D.h"
#include "TStyle.h"
#include "TAxis.h"
#include "TLatex.h"
#include "TH2D.h"
#include <fstream>
#include <iostream>

void DrawLimits_2D() {
    // =========================================================================
    // 1. Style Configuration (CMS Publication Style)
    // =========================================================================
    // Set general style options for a clean look
    gStyle->SetOptStat(0);              // Remove statistics box
    gStyle->SetOptTitle(0);             // Remove default title (we will use TLatex)
    gStyle->SetPalette(kBird);          // Color palette (kBird, kRainBow, kTemperatureMap, etc.)
    gStyle->SetNumberContours(255);     // Smooth color gradients
    gStyle->SetPadRightMargin(0.16);    // Make room for the Z-axis color bar
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetTitleOffset(1.1, "X");
    gStyle->SetTitleOffset(1.2, "Y");
    gStyle->SetTitleOffset(1.1, "Z");

    // =========================================================================
    // 2. Load Data
    // =========================================================================
    // Create TGraph2D to hold the data: X=Mass, Y=f, Z=Limit(r)
    TGraph2D *g = new TGraph2D();
    g->SetName("gLimitParams");
    g->SetTitle(";Mass [GeV];Electron Flavor Fraction (f_{e});95% CL Limit on r");

    // Open the text file (Format: Mass  f  Obs  Exp ...)
    // Make sure 'limit_results.txt' exists in the same directory
		TString input_file = "/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval/Run2_3ch_HNL_syst_Asym_limit.txt";
    std::ifstream infile(input_file);
    if (!infile.is_open()) {
        std::cout << "[Error] " << input_file << " not found!" << std::endl;
        return;
    }

    double m, f, obs, twosig_left, onesig_left, exp, onesig_right, twosig_right; 
    // Adjust these variables based on your actual file columns.
    // Assuming columns: Mass, f, Obs, Exp ... 
    int point_idx = 0;
    
    // Skip header line if exists
    std::string line;
    std::getline(infile, line); 

    while (infile >> m >> f >> obs >> twosig_left >> onesig_left >> exp >> onesig_right >> twosig_right) {
        // We use 'exp' (Expected Limit) for the landscape. 
        // Change to 'obs' if you want Observed Limit.
        g->SetPoint(point_idx, m, f, obs); 
        point_idx++;
        
        // Skip the rest of the line if there are more columns (like +/- 1sigma)
        std::string dummy;
        std::getline(infile, dummy); 
    }
    infile.close();

    std::cout << "[Info] Loaded " << point_idx << " points." << std::endl;

    // =========================================================================
    // 3. Draw the Canvas
    // =========================================================================
    TCanvas *c1 = new TCanvas("c1", "Lepton combined limits", 800, 700);
    c1->SetTicks(1, 1); // Ticks on all sides
		c1->SetLogx(); // log X-axis

    //g->SetMinimum(0.0005);
		g->SetNpx(3000); // high resolution X-axis (so that color can be well split)
    g->SetNpy(200);

    // Draw the 2D Color Map
    // "colz": Color map + Z-axis scale bar
    g->Draw("colz");

    // =========================================================================
    // 4. Axis Tuning (Crucial for Paper)
    // =========================================================================
    // TGraph2D generates a histogram internally for drawing axes
    TH2D* hist = g->GetHistogram();
    
    // X-Axis (Mass)
    //hist->GetXaxis()->SetTitle("Heavy Neutrino Mass [GeV]");
    hist->GetXaxis()->SetTitle("m_{N} (GeV)");
    hist->GetXaxis()->SetTitleSize(0.045);
    hist->GetXaxis()->SetLabelSize(0.04);
    hist->GetYaxis()->SetRangeUser(50., 30000.);
    
    // Y-Axis (f)
    //hist->GetYaxis()->SetTitle("Electron Flavor Fraction f_{e} = |V_{e}|^{2} / (|V_{e}|^{2} + |V_{#mu}|^{2})");
    hist->GetYaxis()->SetTitle("f_{e} = |V_{e}|^{2} / (|V_{e}|^{2} + |V_{#mu}|^{2})");
    hist->GetYaxis()->SetTitleSize(0.045);
    hist->GetYaxis()->SetLabelSize(0.04);
    hist->GetYaxis()->SetRangeUser(0.0, 1.0); // Ensure Y-axis is 0 to 1

    // Z-Axis (Limit r)
    hist->GetZaxis()->SetTitle("95% CL Upper Limit on r = |V_{e}|^{2} + |V_{#mu}|^{2}");
    hist->GetZaxis()->SetTitleSize(0.045);
    hist->GetZaxis()->SetLabelSize(0.035);
    // Log scale for Z is often useful if limits span orders of magnitude
    c1->SetLogz();
		//hist->GetZaxis()->SetRangeUser(0.001, 2.);

    // Improve Z-axis label visibility in log scale
    //hist->GetZaxis()->SetMoreLogLabels(); // Show more intermediate labels
    //hist->GetZaxis()->SetNoExponent();    // Use decimal format (0.1) instead of power (10^-1)

    // =========================================================================
    // 5. Add Contours (Optional but Recommended)
    // =========================================================================
    // Add contour lines for specific r values (e.g., r=1.0, r=0.1)
    // This helps readers visualize the sensitivity boundary.
    
    TGraph2D *g_cont1 = (TGraph2D*)g->Clone("g_cont1");
    double level1[] = {1.}; 
    g_cont1->GetHistogram()->SetContour(1, level1);
    
    g_cont1->SetLineColor(kRed);
    g_cont1->SetLineWidth(2);
    g_cont1->SetLineStyle(1);         // 1: solid
    g_cont1->Draw("same cont3");      // overlay

    TGraph2D *g_cont2 = (TGraph2D*)g->Clone("g_cont2");
    double level2[] = {0.1}; 
    g_cont2->GetHistogram()->SetContour(1, level2);
    
    g_cont2->SetLineColor(kBlack);
    g_cont2->SetLineWidth(2);
    g_cont2->SetLineStyle(2);         // 2: dashed
    g_cont2->Draw("same cont3");

    TGraph2D *g_cont3 = (TGraph2D*)g->Clone("g_cont3");
    double level3[] = {0.002}; 
    g_cont3->GetHistogram()->SetContour(1, level3);
    g_cont3->SetLineColor(kSpring);
    g_cont3->SetLineWidth(2);
    g_cont3->SetLineStyle(3);      // 3: dotted
    g_cont3->Draw("same cont3");

    // =========================================================================
    // 6. CMS Labels and Legends
    // =========================================================================
    TLatex latex;
    latex.SetNDC(); // Normalized coordinates (0-1)
    
    // CMS Label
    latex.SetTextFont(61); // Helvetica Bold
    latex.SetTextSize(0.05);
    latex.DrawLatex(0.12, 0.91, "CMS");
    
    // Preliminary / Work in Progress
    latex.SetTextFont(52); // Helvetica Italic
    latex.SetTextSize(0.04);
    latex.DrawLatex(0.23, 0.91, "Work in Progress");

    // Luminosity info (example)
    latex.SetTextFont(42); // Helvetica Regular
    latex.SetTextSize(0.035);
    latex.SetTextAlign(31); // Align Right
    latex.DrawLatex(0.84, 0.91, "137.6 fb^{-1} (13 TeV)");

    // TLegend(x1, y1, x2, y2) in NDC coordinates
    TLegend *leg = new TLegend(0.15, 0.15, 0.45, 0.25);
    leg->SetBorderSize(0);      // Remove border
    leg->SetFillStyle(0);       // Transparent background
    leg->SetTextSize(0.03);
    leg->SetTextFont(42);
    
    // "l" option means it will draw a line using the object's line attributes
    leg->AddEntry(g_cont1, "r = 1.0 (Theory Exclusion)", "l");
    leg->AddEntry(g_cont2, "r = 0.1", "l");
    leg->AddEntry(g_cont3, "r = 0.002", "l");
    
    leg->Draw();

    // Save
    c1->SaveAs("Limit_3ch_Mass_vs_f.pdf");
    c1->SaveAs("Limit_3ch_Mass_vs_f.png");
}
