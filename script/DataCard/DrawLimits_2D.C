#include "TLegend.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "TMarker.h"
#include "TMath.h"
#include <vector>
#include <tuple>
#include <cmath>
#include <algorithm>
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

// Usage:
// root -l 'DrawLimits_2D.C("massf")'
// root -l 'DrawLimits_2D.C("3ch_vs_envelope")'
// root -l 'DrawLimits_2D.C("envelope_ternary",300,0.1,true)'
// root -l 'DrawLimits_2D.C("combined_ternary",300,0.1,true,"Run2_3ch_HNL_ternary_limit.txt")'

void BaryToXY(double xe, double xmu, double xtau, double &X, double &Y) {
    // Convention matching arXiv:2208.13882 Fig. 4 style:
    // bottom axis: electron fraction, 0 -> 1 from left to right
    // right axis: muon fraction, 0 -> 1 from bottom-right to top
    // left axis: tau fraction, 0 -> 1 from top to bottom-left
    //
    // vertices:
    // tau = (0, 0)
    // e   = (1, 0)
    // mu  = (0.5, sqrt(3)/2)

    double sum = xe + xmu + xtau;
    if (sum <= 0.) {
        X = 0.;
        Y = 0.;
        return;
    }

    xe   /= sum;
    xmu  /= sum;
    xtau /= sum;

    X = xe + 0.5 * xmu;
    Y = TMath::Sqrt(3.) / 2. * xmu;
}

void DrawTernaryFrame_13882Style(bool drawGrid = true) {
    const double h = TMath::Sqrt(3.) / 2.;

    // ------------------------------------------------------------
    // Optional inner grid lines
    // ------------------------------------------------------------
    if (drawGrid) {
        TLine grid;
        grid.SetLineColor(kGray + 1);
        grid.SetLineStyle(3);
        grid.SetLineWidth(1);

        for (int i = 1; i <= 9; ++i) {
            double t = i / 10.;

            double x1, y1, x2, y2;

            // constant xe = t
            BaryToXY(t, 0., 1. - t, x1, y1);
            BaryToXY(t, 1. - t, 0., x2, y2);
            grid.DrawLine(x1, y1, x2, y2);

            // constant xmu = t
            BaryToXY(0., t, 1. - t, x1, y1);
            BaryToXY(1. - t, t, 0., x2, y2);
            grid.DrawLine(x1, y1, x2, y2);

            // constant xtau = t
            BaryToXY(0., 1. - t, t, x1, y1);
            BaryToXY(1. - t, 0., t, x2, y2);
            grid.DrawLine(x1, y1, x2, y2);
        }
    }

    // ------------------------------------------------------------
    // Triangle border
    // ------------------------------------------------------------
    TPolyLine *tri = new TPolyLine(4);
    tri->SetPoint(0, 0., 0.);   // tau = 1
    tri->SetPoint(1, 1., 0.);   // e = 1
    tri->SetPoint(2, 0.5, h);   // mu = 1
    tri->SetPoint(3, 0., 0.);
    tri->SetLineColor(kBlack);
    tri->SetLineWidth(2);
    tri->Draw("same");

    // ------------------------------------------------------------
    // Tick labels
    // ------------------------------------------------------------
    TLatex tick;
    tick.SetTextFont(42);
    tick.SetTextSize(0.025);
    tick.SetTextAlign(22);

    for (int i = 0; i <= 10; ++i) {
        double t = i / 10.;

        // electron axis: bottom, left -> right
        tick.DrawLatex(t, -0.035, Form("%.1f", t));

        // muon axis: right edge, bottom-right -> top
        double xmu_x, xmu_y;
        BaryToXY(1. - t, t, 0., xmu_x, xmu_y);
        tick.DrawLatex(xmu_x + 0.045, xmu_y, Form("%.1f", t));

        // tau axis: left edge, top -> bottom-left
        double xtau_x, xtau_y;
        BaryToXY(0., 1. - t, t, xtau_x, xtau_y);
        tick.DrawLatex(xtau_x - 0.045, xtau_y, Form("%.1f", t));
    }

    // ------------------------------------------------------------
    // Axis titles
    // ------------------------------------------------------------
    TLatex title;
    title.SetTextFont(42);
    title.SetTextSize(0.035);
    title.SetTextAlign(22);

    title.SetTextAngle(0);
    title.DrawLatex(0.50, -0.095, "|U_{eN}|^{2}/U^{2}");

    title.SetTextAngle(-60);
    title.DrawLatex(0.86, 0.36, "|U_{#mu N}|^{2}/U^{2}");

    title.SetTextAngle(60);
    title.DrawLatex(0.12, 0.36, "|U_{#tau N}|^{2}/U^{2}");

    // ------------------------------------------------------------
    // Corner labels
    // ------------------------------------------------------------
    TLatex corner;
    corner.SetTextFont(42);
    corner.SetTextSize(0.04);
    corner.SetTextAlign(22);
    corner.SetTextAngle(0);

    corner.DrawLatex(1.04, -0.035, "e");
    corner.DrawLatex(0.50, h + 0.040, "#mu");
    corner.DrawLatex(-0.04, -0.035, "#tau");
}

struct RFRow {
    double mass;
    double f;
    double obs;
    double exp;
};

bool GetRFLimitExact(const std::vector<RFRow> &rows,
                     double targetMass,
                     double targetF,
                     bool useObs,
                     double &val) {
    const double massTol = 1e-6 * std::max(1.0, targetMass);
    const double fTol = 1e-6;

    for (const auto &r : rows) {
        if (std::abs(r.mass - targetMass) < massTol &&
            std::abs(r.f - targetF) < fTol) {

            val = useObs ? r.obs : r.exp;

            if (val > 0.) return true;
            return false;
        }
    }

    return false;
}

std::vector<RFRow> LoadRFFile(TString input_file, TString label = "") {
    std::vector<RFRow> rows;

    std::ifstream infile(input_file.Data());
    if (!infile.is_open()) {
        std::cout << "[Error] Cannot open " << input_file << std::endl;
        return rows;
    }

    std::string line;
    std::getline(infile, line); // header

    double m, f, obs, twosig_left, onesig_left, exp, onesig_right, twosig_right;

    while (infile >> m >> f >> obs >> twosig_left >> onesig_left >> exp >> onesig_right >> twosig_right) {
        rows.push_back({m, f, obs, exp});
        std::getline(infile, line);
    }

    infile.close();

    if (label != "") {
        std::cout << "[Info] Loaded " << rows.size()
                  << " points from " << label
                  << " : " << input_file << std::endl;
    } else {
        std::cout << "[Info] Loaded " << rows.size()
                  << " points from " << input_file << std::endl;
    }

    return rows;
}

struct TerRow {
    double mass;
    double xe;
    double xmu;
    double xtau;
    double obs;
    double exp;
};

double DistTernary(double xe1, double xmu1, double xtau1,
                   double xe2, double xmu2, double xtau2) {
    return std::sqrt((xe1-xe2)*(xe1-xe2)
                   + (xmu1-xmu2)*(xmu1-xmu2)
                   + (xtau1-xtau2)*(xtau1-xtau2));
}

double GetTernaryLimitNearest(const std::vector<TerRow> &rows,
                              double targetMass,
                              double xe,
                              double xmu,
                              double xtau,
                              bool useObs) {
    if (rows.empty()) return -1.;

    double best = -1.;
    double bestMetric = 1e99;

    for (const auto &r : rows) {
        double dm = std::abs(std::log(r.mass / targetMass));
        double df = DistTernary(r.xe, r.xmu, r.xtau, xe, xmu, xtau);
        double metric = dm * 10. + df; // mass mismatch heavily penalized

        if (metric < bestMetric) {
            bestMetric = metric;
            best = useObs ? r.obs : r.exp;
        }
    }

    return best;
}

void DrawCombinedTernary(TString ternary_file,
                         double targetMass,
                         double targetU2,
                         bool useObs) {
    std::ifstream infile(ternary_file);
    if (!infile.is_open()) {
        std::cout << "[Error] Cannot open " << ternary_file << std::endl;
        return;
    }

    std::vector<TerRow> rows;

    std::string line;
    std::getline(infile, line); // header

    double m, xe, xmu, xtau;
    double obs, m2, m1, exp, p1, p2;

    while (infile >> m >> xe >> xmu >> xtau >> obs >> m2 >> m1 >> exp >> p1 >> p2) {
        rows.push_back({m, xe, xmu, xtau, obs, exp});
        std::getline(infile, line);
    }

    infile.close();

    std::cout << "[Combined ternary] Loaded " << rows.size() << " points." << std::endl;

    TGraph2D *gTer = new TGraph2D();
    gTer->SetName("gCombinedTernary");
    gTer->SetTitle(";ternary X;ternary Y;U^{2}/U^{2}_{95}");

    int ip = 0;
    const int N = 120;

    for (int ie = 0; ie <= N; ++ie) {
        for (int imu = 0; imu <= N - ie; ++imu) {
            double xe0 = double(ie) / N;
            double xmu0 = double(imu) / N;
            double xtau0 = 1. - xe0 - xmu0;

            double X, Y;
            BaryToXY(xe0, xmu0, xtau0, X, Y);

            double U2lim = GetTernaryLimitNearest(rows, targetMass, xe0, xmu0, xtau0, useObs);

            double score = 0.;
            if (U2lim > 0.) score = targetU2 / U2lim;

            gTer->SetPoint(ip, X, Y, score);
            ip++;
        }
    }

    TCanvas *c = new TCanvas("c_comb", "Combined ternary", 800, 700);
    c->SetRightMargin(0.16);
    gTer->SetNpx(400);
    gTer->SetNpy(400);
    gTer->Draw("colz");

    TH2D *h = gTer->GetHistogram();
    h->GetXaxis()->SetTitle("");
    h->GetYaxis()->SetTitle("");
    h->GetZaxis()->SetTitle("S_{comb} = U^{2}/U^{2}_{95}");

    const double htri = TMath::Sqrt(3.) / 2.;
    
    h->GetXaxis()->SetRangeUser(-0.08, 1.08);
    h->GetYaxis()->SetRangeUser(-0.08, htri + 0.08);
    
    h->GetXaxis()->SetLabelSize(0.);
    h->GetYaxis()->SetLabelSize(0.);
    h->GetXaxis()->SetTitleSize(0.);
    h->GetYaxis()->SetTitleSize(0.);
    h->GetXaxis()->SetTickLength(0.);
    h->GetYaxis()->SetTickLength(0.);

    TGraph2D *gCont = (TGraph2D*)gTer->Clone("gCombinedContour");
    double lev[] = {1.0};
    gCont->GetHistogram()->SetContour(1, lev);
    gCont->SetLineColor(kBlack);
    gCont->SetLineWidth(3);
    gCont->Draw("same cont3");

    DrawTernaryFrame_13882Style(true);

    TLatex lat;
    lat.SetNDC();
    lat.SetTextSize(0.035);
    lat.DrawLatex(0.15, 0.92, Form("Combined ternary, m_{N}=%.0f GeV, U^{2}=%.3g", targetMass, targetU2));

    c->SaveAs(Form("CombinedTernary_m%.0f_U2%.3g_%s.pdf", targetMass, targetU2, useObs ? "obs" : "exp"));
    c->SaveAs(Form("CombinedTernary_m%.0f_U2%.3g_%s.png", targetMass, targetU2, useObs ? "obs" : "exp"));
}

double GetRFLimitNearestMass(const std::vector<RFRow> &rows,
                             double targetMass,
                             double targetF,
                             bool useObs) {
    if (rows.empty()) return -1.;

    double bestMass = rows[0].mass;
    double bestDist = std::abs(std::log(rows[0].mass / targetMass));

    for (const auto &r : rows) {
        double d = std::abs(std::log(r.mass / targetMass));
        if (d < bestDist) {
            bestDist = d;
            bestMass = r.mass;
        }
    }

    std::vector<RFRow> sameMass;
    for (const auto &r : rows) {
        if (std::abs(r.mass - bestMass) < 1e-6) sameMass.push_back(r);
    }

    std::sort(sameMass.begin(), sameMass.end(),
              [](const RFRow &a, const RFRow &b) { return a.f < b.f; });

    if (targetF <= sameMass.front().f) {
        return useObs ? sameMass.front().obs : sameMass.front().exp;
    }
    if (targetF >= sameMass.back().f) {
        return useObs ? sameMass.back().obs : sameMass.back().exp;
    }

    for (size_t i = 0; i + 1 < sameMass.size(); ++i) {
        double f1 = sameMass[i].f;
        double f2 = sameMass[i+1].f;

        if (targetF >= f1 && targetF <= f2) {
            double z1 = useObs ? sameMass[i].obs : sameMass[i].exp;
            double z2 = useObs ? sameMass[i+1].obs : sameMass[i+1].exp;
            double t = (targetF - f1) / (f2 - f1);
            return z1 + t * (z2 - z1);
        }
    }

    return -1.;
}

void DrawEnvelopeTernary(const std::vector<RFRow> &rows,
                         double targetMass,
                         double targetU2,
                         bool useObs) {
    double Lmumu = GetRFLimitNearestMass(rows, targetMass, 0.0, useObs);
    double Lee   = GetRFLimitNearestMass(rows, targetMass, 1.0, useObs);
    double Rhalf = GetRFLimitNearestMass(rows, targetMass, 0.5, useObs);

    // Your convention check: R(f=0.5) * f(1-f) reproduces the e-mu limit.
    double Lemu_eff = 0.25 * Rhalf;

    std::cout << "[Envelope] mass = " << targetMass
              << ", U2 = " << targetU2
              << ", Lmumu = " << Lmumu
              << ", Lee = " << Lee
              << ", Lemu_eff = " << Lemu_eff
              << std::endl;

    TGraph2D *gTer = new TGraph2D();
    gTer->SetName("gEnvelopeTernary");
    gTer->SetTitle(";ternary X;ternary Y;max single-channel score");

    int ip = 0;
    const int N = 120;

    for (int ie = 0; ie <= N; ++ie) {
        for (int imu = 0; imu <= N - ie; ++imu) {
            double xe = double(ie) / N;
            double xmu = double(imu) / N;
            double xtau = 1. - xe - xmu;

            double X, Y;
            BaryToXY(xe, xmu, xtau, X, Y);

            double s_mumu = (Lmumu > 0.) ? targetU2 * xmu * xmu / Lmumu : 0.;
            double s_ee   = (Lee   > 0.) ? targetU2 * xe   * xe   / Lee   : 0.;
            double s_emu  = (Lemu_eff > 0.) ? targetU2 * xe * xmu / Lemu_eff : 0.;

            double score = std::max(s_mumu, std::max(s_ee, s_emu));

            gTer->SetPoint(ip, X, Y, score);
            ip++;
        }
    }

    TCanvas *c = new TCanvas("c_env", "Envelope ternary", 800, 700);
    c->SetRightMargin(0.16);
    gTer->SetNpx(400);
    gTer->SetNpy(400);
    gTer->Draw("colz");

    TH2D *h = gTer->GetHistogram();
    h->GetXaxis()->SetTitle("");
    h->GetYaxis()->SetTitle("");
    h->GetZaxis()->SetTitle("S_{env} = max(channel score)");

    const double htri = TMath::Sqrt(3.) / 2.;

    h->GetXaxis()->SetRangeUser(-0.08, 1.08);
    h->GetYaxis()->SetRangeUser(-0.08, htri + 0.08);
    
    h->GetXaxis()->SetLabelSize(0.);
    h->GetYaxis()->SetLabelSize(0.);
    h->GetXaxis()->SetTitleSize(0.);
    h->GetYaxis()->SetTitleSize(0.);
    h->GetXaxis()->SetTickLength(0.);
    h->GetYaxis()->SetTickLength(0.);

    // Exclusion contour: S_env = 1
    TGraph2D *gCont = (TGraph2D*)gTer->Clone("gEnvelopeContour");
    double lev[] = {1.0};
    gCont->GetHistogram()->SetContour(1, lev);
    gCont->SetLineColor(kBlack);
    gCont->SetLineWidth(3);
    gCont->Draw("same cont3");

    DrawTernaryFrame_13882Style(true);		

    TLatex lat;
    lat.SetNDC();
    lat.SetTextSize(0.035);
    lat.DrawLatex(0.15, 0.92, Form("Envelope ternary, m_{N}=%.0f GeV, U^{2}=%.3g", targetMass, targetU2));

    c->SaveAs(Form("EnvelopeTernary_m%.0f_U2%.3g_%s.pdf", targetMass, targetU2, useObs ? "obs" : "exp"));
    c->SaveAs(Form("EnvelopeTernary_m%.0f_U2%.3g_%s.png", targetMass, targetU2, useObs ? "obs" : "exp"));
}

void Draw3chVsEnvelope(const std::vector<RFRow> &combRows,
                       TString mumu_file,
                       TString ee_file,
                       TString emu_file,
                       bool useObs) {

    std::vector<RFRow> mumuRows = LoadRFFile(mumu_file, "MuMu envelope");
    std::vector<RFRow> eeRows   = LoadRFFile(ee_file,   "EE envelope");
    std::vector<RFRow> emuRows  = LoadRFFile(emu_file,  "EMu envelope");

    if (combRows.empty()) {
        std::cout << "[Error] combined 3ch rows are empty." << std::endl;
        return;
    }

    if (mumuRows.empty() || eeRows.empty() || emuRows.empty()) {
        std::cout << "[Error] one or more envelope input files are empty." << std::endl;
        return;
    }

    TGraph2D *gRatio = new TGraph2D();
    gRatio->SetName("g_3ch_vs_envelope");
    gRatio->SetTitle(";m_{N} (GeV);f_{e};R = r_{env}/r_{comb}");

    int ip = 0;

    for (const auto &c : combRows) {

        const double mass = c.mass;
        const double fval = c.f;
        const double r_comb = useObs ? c.obs : c.exp;

        if (r_comb <= 0.) continue;

        double r_mumu = -1.;
        double r_ee   = -1.;
        double r_emu  = -1.;

        bool has_mumu = GetRFLimitExact(mumuRows, mass, fval, useObs, r_mumu);
        bool has_ee   = GetRFLimitExact(eeRows,   mass, fval, useObs, r_ee);
        bool has_emu  = GetRFLimitExact(emuRows,  mass, fval, useObs, r_emu);

        const bool is_f0 = std::abs(fval - 0.0) < 1e-6;
        const bool is_f1 = std::abs(fval - 1.0) < 1e-6;

        double r_env = -1.;

        if (is_f0) {
            // At f = 0, EE and EMu are not physically meaningful for the 3ch-model
            // single-card envelope study. Only MuMu is expected to exist.
            if (!has_mumu) {
                std::cout << "[WARN] Missing MuMu envelope point at m="
                          << mass << ", f=" << fval << std::endl;
                continue;
            }

            r_env = r_mumu;
        }
        else if (is_f1) {
            // At f = 1, MuMu and EMu are not physically meaningful.
            // Only EE is expected to exist.
            if (!has_ee) {
                std::cout << "[WARN] Missing EE envelope point at m="
                          << mass << ", f=" << fval << std::endl;
                continue;
            }

            r_env = r_ee;
        }
        else {
            // For 0 < f < 1, all three single-channel envelope inputs
            // should exist.
            if (!has_mumu || !has_ee || !has_emu) {
                std::cout << "[WARN] Missing envelope point at m="
                          << mass << ", f=" << fval
                          << "  has MuMu/EE/EMu = "
                          << has_mumu << "/"
                          << has_ee << "/"
                          << has_emu << std::endl;
                continue;
            }

            r_env = std::min(r_mumu, std::min(r_ee, r_emu));
        }

        if (r_env <= 0.) continue;

        double R = r_env / r_comb;

        gRatio->SetPoint(ip, mass, fval, R);
        ip++;
    }

    std::cout << "[3ch_vs_envelope] Filled " << ip << " ratio points." << std::endl;

    if (ip == 0) {
        std::cout << "[Error] No valid R points were filled." << std::endl;
        return;
    }

    TCanvas *c = new TCanvas("c_3ch_vs_envelope", "3ch vs envelope", 800, 700);
    c->SetTicks(1, 1);
    c->SetLogx();
    c->SetRightMargin(0.16);

    gRatio->SetNpx(3000);
    gRatio->SetNpy(200);

    gRatio->Draw("colz");

    TH2D *hist = gRatio->GetHistogram();

    hist->GetXaxis()->SetTitle("m_{N} (GeV)");
    hist->GetXaxis()->SetTitleSize(0.045);
    hist->GetXaxis()->SetLabelSize(0.04);

    hist->GetYaxis()->SetTitle("f_{e} = |V_{e}|^{2} / (|V_{e}|^{2} + |V_{#mu}|^{2})");
    hist->GetYaxis()->SetTitleSize(0.045);
    hist->GetYaxis()->SetLabelSize(0.04);
    hist->GetYaxis()->SetRangeUser(0.0, 1.0);

    hist->GetZaxis()->SetTitle("R = r_{env} / r_{comb}");
    hist->GetZaxis()->SetTitleSize(0.045);
    hist->GetZaxis()->SetLabelSize(0.035);

    // Optional: uncomment this if you want to force the visual range.
    // hist->GetZaxis()->SetRangeUser(1.0, 1.5);

    // R = 1.00 contour
    TGraph2D *g_cont1 = (TGraph2D*)gRatio->Clone("g_R_1p00");
    double level1[] = {1.00};
    g_cont1->GetHistogram()->SetContour(1, level1);
    g_cont1->SetLineColor(kBlack);
    g_cont1->SetLineWidth(2);
    g_cont1->SetLineStyle(1);
    g_cont1->Draw("same cont3");

    // R = 1.10 contour: 10% improvement
    TGraph2D *g_cont2 = (TGraph2D*)gRatio->Clone("g_R_1p10");
    double level2[] = {1.10};
    g_cont2->GetHistogram()->SetContour(1, level2);
    g_cont2->SetLineColor(kRed);
    g_cont2->SetLineWidth(2);
    g_cont2->SetLineStyle(2);
    g_cont2->Draw("same cont3");

    // R = 1.25 contour: 25% improvement
    TGraph2D *g_cont3 = (TGraph2D*)gRatio->Clone("g_R_1p25");
    double level3[] = {1.25};
    g_cont3->GetHistogram()->SetContour(1, level3);
    g_cont3->SetLineColor(kBlue + 1);
    g_cont3->SetLineWidth(2);
    g_cont3->SetLineStyle(3);
    g_cont3->Draw("same cont3");

    TLatex latex;
    latex.SetNDC();

    latex.SetTextFont(61);
    latex.SetTextSize(0.05);
    latex.DrawLatex(0.12, 0.91, "CMS");

    latex.SetTextFont(52);
    latex.SetTextSize(0.04);
    latex.DrawLatex(0.23, 0.91, "Work in Progress");

    latex.SetTextFont(42);
    latex.SetTextSize(0.035);
    latex.SetTextAlign(31);
    latex.DrawLatex(0.84, 0.91, "137.6 fb^{-1} (13 TeV)");

    TLegend *leg = new TLegend(0.15, 0.15, 0.48, 0.28);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.03);
    leg->SetTextFont(42);

    leg->AddEntry(g_cont1, "R = 1.00", "l");
    leg->AddEntry(g_cont2, "R = 1.10, 10% stronger", "l");
    leg->AddEntry(g_cont3, "R = 1.25, 25% stronger", "l");

    leg->Draw();

    c->SaveAs(Form("Limit_3ch_vs_envelope_%s.pdf", useObs ? "obs" : "exp"));
    c->SaveAs(Form("Limit_3ch_vs_envelope_%s.png", useObs ? "obs" : "exp"));
}

void DrawLimits_2D(
    TString mode = "massf",
    double targetMass = 300.,
    double targetU2 = 0.1,
    bool useObs = false,
    TString ternary_file = "",
    TString input_file = "",
    TString envelope_dir = ""
) {
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

    std::vector<RFRow> rfRows;

    // Open the text file (Format: Mass  f  Obs  Exp ...)
    // Make sure 'limit_results.txt' exists in the same directory
		//TString input_file = "/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval/Run2_3ch_HNL_syst_Asym_limit.txt";
    if (input_file == "") {
        std::cout << "[Error] Please provide input_file, e.g. "
                  << "\"/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/<YOUR_3CH_WP>/Run2Sum_3ch_HNL_syst_Asym_limit.txt\""
                  << std::endl;
        return;
    }
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
        // Set useObs if you want Observed Limit.
				double zval = useObs ? obs : exp;
        g->SetPoint(point_idx, m, f, zval);
        rfRows.push_back({m, f, obs, exp});
        point_idx++;
        
        // Skip the rest of the line if there are more columns (like +/- 1sigma)
        std::string dummy;
        std::getline(infile, dummy); 
    }
    infile.close();

    std::cout << "[Info] Loaded " << point_idx << " points." << std::endl;

    if (mode == "3ch_vs_envelope") {
    
        //TString envelope_dir =
        //    "/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval_StudyEnvelope";
    
        //TString mumu_file = envelope_dir + "/Run2_MuMu_HNL_syst_Asym_limit.txt";
        //TString ee_file   = envelope_dir + "/Run2_EE_HNL_syst_Asym_limit.txt";
        //TString emu_file  = envelope_dir + "/Run2_EMu_HNL_syst_Asym_limit.txt";

        if (envelope_dir == "") {
            std::cout << "[Error] mode=3ch_vs_envelope requires envelope_dir." << std::endl;
            return;
        }

        TString mumu_file = envelope_dir + "/Run2Sum_MuMu_HNL_syst_Asym_limit.txt";
        TString ee_file   = envelope_dir + "/Run2Sum_EE_HNL_syst_Asym_limit.txt";
        TString emu_file  = envelope_dir + "/Run2Sum_EMu_HNL_syst_Asym_limit.txt";
    
        Draw3chVsEnvelope(rfRows, mumu_file, ee_file, emu_file, useObs);
        return;
    }

    if (mode == "envelope_ternary") {
        DrawEnvelopeTernary(rfRows, targetMass, targetU2, useObs);
        return;
    }
    
    if (mode == "combined_ternary") {
        if (ternary_file == "") {
            std::cout << "[Error] combined_ternary mode requires ternary_file." << std::endl;
            return;
        }
        DrawCombinedTernary(ternary_file, targetMass, targetU2, useObs);
        return;
    }

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
    //hist->GetXaxis()->SetRangeUser(50., 30000.);
    
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
    c1->SaveAs(Form("Limit_3ch_Mass_vs_f_%s.pdf", useObs ? "obs" : "exp"));
    c1->SaveAs(Form("Limit_3ch_Mass_vs_f_%s.png", useObs ? "obs" : "exp"));
}
