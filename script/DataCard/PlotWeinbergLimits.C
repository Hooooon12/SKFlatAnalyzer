#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TBox.h>
#include <TLine.h>
#include <TLatex.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TROOT.h>

#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <cstdlib>
#include <iostream>

//---------------------- Data structure ----------------------
struct Entry {
  double obs{0};
  double q025{0}, q16{0}, q50{0}, q84{0}, q975{0};
};

struct ExternalLimit {
  bool hasCMS{false};
  double cmsExp{0}, cmsObs{0};
  bool hasATLAS{false};
  double atlasExp{0}, atlasObs{0};
};

//---------------------- Small helpers -----------------------
static inline std::string ltrim(std::string s){
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
          [](unsigned char ch){ return !std::isspace(ch); }));
  return s;
}

static inline std::string rtrim(std::string s){
  s.erase(std::find_if(s.rbegin(), s.rend(),
          [](unsigned char ch){ return !std::isspace(ch); }).base(), s.end());
  return s;
}

static inline std::string trim(std::string s){ return rtrim(ltrim(s)); }

//---------------------- Parser for new mass-limit txt format ----------------------
// Expected format:
// Channel  Obs  Exp_m2s  Exp_m1s  Exp  Exp_p1s  Exp_p2s
// EE       ...  ...      ...      ...  ...      ...
static Entry ParseMassLimitFile(const std::string& path, const std::string& expectedChannel){
  std::ifstream fin(path);
  if(!fin.is_open()){
    throw std::runtime_error("Cannot open file: " + path);
  }

  std::string line;
  std::string header;
  while(std::getline(fin, line)){
    line = trim(line);
    if(line.empty()) continue;
    header = line;
    break;
  }

  if(header.empty()){
    throw std::runtime_error("Empty limit file: " + path);
  }

  std::string dataLine;
  while(std::getline(fin, line)){
    line = trim(line);
    if(line.empty()) continue;
    dataLine = line;
    break;
  }
  fin.close();

  if(dataLine.empty()){
    throw std::runtime_error("No data row found in: " + path);
  }

  std::istringstream iss(dataLine);
  std::string ch;
  Entry e;
  if(!(iss >> ch >> e.obs >> e.q025 >> e.q16 >> e.q50 >> e.q84 >> e.q975)){
    throw std::runtime_error("Could not parse data row in: " + path + "\nRow: " + dataLine);
  }

  if(ch != expectedChannel){
    throw std::runtime_error("Channel mismatch in " + path + ": expected " + expectedChannel + ", got " + ch);
  }

  return e;
}

static void SetTDRStyle() {
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFont(42, "XYZ");
  gStyle->SetLabelFont(42, "XYZ");
  gStyle->SetTextFont(42);
  gStyle->SetTitleSize(0.05, "XYZ");
  gStyle->SetLabelSize(0.04, "XYZ");
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetEndErrorSize(0);
}

static void DrawCMSHeader(TPad* pad,
                          const std::string& lumiText = "138 fb^{-1} (13 TeV)",
                          const std::string& extraText = "Preliminary",
                          int cmsPos = 11) {
  pad->cd();

  float l = pad->GetLeftMargin();
  float t = pad->GetTopMargin();
  float r = pad->GetRightMargin();

  const int cmsFont = 61;
  const int extraFont = 52;
  const int lumiFont = 42;

  // Keep the luminosity aligned with the right edge of the plotting frame.
  const float headerY = 1.0 - t + 0.012;

  TLatex lumi;
  lumi.SetNDC(true);
  lumi.SetTextFont(lumiFont);
  lumi.SetTextSize(0.036);
  lumi.SetTextAlign(31);
  lumi.DrawLatex(1.0 - r, headerY, lumiText.c_str());

  float cmsX = l;
  int align = 11;
  if(cmsPos==33){ cmsX = 1.0 - r; align = 31; }

  TLatex cms;
  cms.SetNDC(true);
  cms.SetTextFont(cmsFont);
  cms.SetTextSize(0.055);
  cms.SetTextAlign(align);
  cms.DrawLatex(cmsX, headerY, "CMS");

  if(!extraText.empty()){
    TLatex extra;
    extra.SetNDC(true);
    extra.SetTextFont(extraFont);
    extra.SetTextSize(0.040);
    extra.SetTextAlign(11);
    // 'Preliminary' is on the same baseline, immediately to the right of CMS.
    extra.DrawLatex(cmsX + 0.075, headerY, extraText.c_str());
  }
}

//---------------------- Main drawing function ----------------------
void PlotWeinbergLimits(
    const char* WP="ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195",
    bool observed=false,
    const char* lumi_text="138 fb^{-1} (13 TeV)",
    const char* extra_text="Preliminary") {

  SetTDRStyle();

  const std::string limitBase = "/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/";
  const std::string wp(WP);
  const std::string prefix = "Run2Sum_";
  const std::string suffix = "_Weinberg_syst_Asym_mass_limit.txt";

  // Channel order on the plot
  const std::vector<std::string> order = {"EMu", "EE", "MuMu"};

  // Read our limits from one file per channel
  std::map<std::string, Entry> data;
  for(const auto& ch : order){
    const std::string path = limitBase + wp + "/" + prefix + ch + suffix;
    std::cout << "[INPUT] " << path << std::endl;
    data[ch] = ParseMassLimitFile(path, ch);
  }

  // External Weinberg-operator limits, hard-coded as Exp(Obs) [GeV]
  // MuMu: CMS 12.8 (10.8), ATLAS 13.1 (16.7)
  // EE:   ATLAS 24 (24)
  // EMu:  ATLAS 14 (12)
  std::map<std::string, ExternalLimit> ext;

  ext["MuMu"].hasCMS = true;
  ext["MuMu"].cmsExp = 12.8;
  ext["MuMu"].cmsObs = 10.8;
  ext["MuMu"].hasATLAS = true;
  ext["MuMu"].atlasExp = 13.1;
  ext["MuMu"].atlasObs = 16.7;

  ext["EE"].hasATLAS = true;
  ext["EE"].atlasExp = 24.0;
  ext["EE"].atlasObs = 24.0;

  ext["EMu"].hasATLAS = true;
  ext["EMu"].atlasExp = 14.0;
  ext["EMu"].atlasObs = 12.0;

  const int N = (int)order.size();
  std::vector<double> y(N);
  for(int j=0; j<N; ++j) y[j] = j;

  // X-range: include our expected bands, our observed values in obs mode,
  // and whichever external values are being drawn.
  double xmin = 1e9, xmax = -1e9;
  auto widen = [&](double v){ xmin = std::min(xmin,v); xmax = std::max(xmax,v); };

  for(const auto& ch : order){
    const Entry& e = data.at(ch);
    widen(e.q025); widen(e.q16); widen(e.q50); widen(e.q84); widen(e.q975);
    if(observed) widen(e.obs);

    const ExternalLimit& x = ext.at(ch);
    if(x.hasCMS)  widen(observed ? x.cmsObs   : x.cmsExp);
    if(x.hasATLAS) widen(observed ? x.atlasObs : x.atlasExp);
  }

  double span = xmax - xmin;
  if(span <= 0){ xmin -= 1; xmax += 1; span = xmax - xmin; }
  xmin -= 0.05 * span;
  xmax += 0.15 * span;

  // Canvas: make it wider and reserve a dedicated right-side gutter for the legend.
  const int H = 480 + 120*N;
  TCanvas* c = new TCanvas("c", observed ? "Weinberg Observed Limits" : "Weinberg Expected Limits", 1300, H);
  c->SetLeftMargin(0.12);
  c->SetRightMargin(0.28);
  c->SetBottomMargin(0.13);
  c->SetTopMargin(0.10);

  // Frame
  TH2F* frame = new TH2F("frame", "", 10, xmin, xmax, 10, -0.5, N-0.5);
  frame->SetTitle("");
  frame->GetXaxis()->SetTitle("m_{ll} [GeV]");
  frame->GetYaxis()->SetNdivisions(0);
  frame->GetXaxis()->SetTitleSize(0.05);
  frame->GetXaxis()->SetLabelSize(0.042);
  frame->Draw();

  // Bands and lines
  const double h2s = 0.28;
  const double h1s = 0.16;

  // Legend prototypes
  TBox* box2s = new TBox(0,0,1,1);
  box2s->SetFillColorAlpha(kYellow, 0.7);
  box2s->SetLineColor(0);

  TBox* box1s = new TBox(0,0,1,1);
  box1s->SetFillColorAlpha(kGreen+1, 0.8);
  box1s->SetLineColor(0);

  // Our expected median is now dashed in both modes.
  TLine* lmed = new TLine(0,0,1,0);
  lmed->SetLineStyle(2);
  lmed->SetLineWidth(3);
  lmed->SetLineColor(kBlack);

  // Our observed limit is overlaid as a solid black line only in observed mode.
  TLine* lobs = new TLine(0,0,1,0);
  lobs->SetLineStyle(1);
  lobs->SetLineWidth(4);
  lobs->SetLineColor(kBlack);

  // External reference styles retained from the original macro.
  TLine* lcms = new TLine(0,0,0,1);
  lcms->SetLineStyle(2);
  lcms->SetLineWidth(3);
  lcms->SetLineColor(kBlue+1);

  TLine* latl = new TLine(0,0,0,1);
  latl->SetLineStyle(7);
  latl->SetLineWidth(3);
  latl->SetLineColor(kRed+1);

  TLine* latl2 = new TLine(0,0,0,1);
  latl2->SetLineStyle(7);
  latl2->SetLineWidth(3);
  latl2->SetLineColor(kMagenta+2);

  bool needAtlasEPJC = false; // MuMu
  bool needAtlasPLB  = false; // EE, EMu

  // Draw per-channel objects
  for(int j=0; j<N; ++j){
    const std::string& ch = order[j];
    const Entry& e = data.at(ch);
    const ExternalLimit& x = ext.at(ch);
    const double yc = y[j];

    // Our expected +/-2 sigma band
    TBox* b2 = new TBox(e.q025, yc - h2s, e.q975, yc + h2s);
    b2->SetFillColorAlpha(kYellow, 0.7);
    b2->SetLineColor(0);
    b2->Draw("same");

    // Our expected +/-1 sigma band
    TBox* b1 = new TBox(e.q16, yc - h1s, e.q84, yc + h1s);
    b1->SetFillColorAlpha(kGreen+1, 0.8);
    b1->SetLineColor(0);
    b1->Draw("same");

    // Our expected median: dashed black
    TLine* l50 = new TLine(e.q50, yc - h2s, e.q50, yc + h2s);
    l50->SetLineStyle(2);
    l50->SetLineWidth(3);
    l50->SetLineColor(kBlack);
    l50->Draw("same");

    // In observed mode, overlay our observed limit as solid black.
    if(observed){
      TLine* lo = new TLine(e.obs, yc - 0.34, e.obs, yc + 0.34);
      lo->SetLineStyle(1);
      lo->SetLineWidth(4);
      lo->SetLineColor(kBlack);
      lo->Draw("same");
    }

    // External CMS reference: expected in exp mode, observed in obs mode.
    if(x.hasCMS){
      const double v = observed ? x.cmsObs : x.cmsExp;
      TLine* lc = new TLine(v, yc - 0.34, v, yc + 0.34);
      lc->SetLineStyle(2);
      lc->SetLineWidth(4);
      lc->SetLineColor(kBlue+1);
      lc->Draw("same");
    }

    // External ATLAS reference: expected in exp mode, observed in obs mode.
    if(x.hasATLAS){
      const double v = observed ? x.atlasObs : x.atlasExp;
      if(ch == "MuMu"){
        TLine* la = new TLine(v, yc - 0.34, v, yc + 0.34);
        la->SetLineStyle(7);
        la->SetLineWidth(4);
        la->SetLineColor(kRed+1);
        la->Draw("same");
        needAtlasEPJC = true;
      } else if(ch == "EE" || ch == "EMu"){
        TLine* la2 = new TLine(v, yc - 0.34, v, yc + 0.34);
        la2->SetLineStyle(7);
        la2->SetLineWidth(4);
        la2->SetLineColor(kMagenta+2);
        la2->Draw("same");
        needAtlasPLB = true;
      }
    }
  }

  // Y-axis labels: place each label at the exact vertical center of its limit band.
  TLatex lab;
  lab.SetTextFont(42);
  lab.SetTextSize(0.040);
  lab.SetTextAlign(32);

  const double yNdcMin = c->GetBottomMargin();
  const double yNdcMax = 1.0 - c->GetTopMargin();
  const double plotNdcH = yNdcMax - yNdcMin;
  const double labelX = c->GetLeftMargin() - 0.012;

  for(int j=0; j<N; ++j){
    std::string label = order[j];
    if(label == "MuMu") label = "#mu#mu";
    if(label == "EMu")  label = "e#mu";
    if(label == "EE")   label = "ee";

    // Frame spans y=[-0.5, N-0.5], so band j is centered at (j+0.5)/N in the frame.
    const double labelY = yNdcMin + plotNdcH * (j + 0.5) / N;
    lab.DrawLatexNDC(labelX, labelY, label.c_str());
  }

  DrawCMSHeader(c, lumi_text, extra_text, 11);

  // Legend: use the dedicated right margin, shifted down and away from all bands.
  const double frameRight = 1.0 - c->GetRightMargin();
  const double x1 = frameRight + 0.018;
  const double x2 = 0.985;
  const double y1 = 0.14;
  const double y2 = observed ? 0.46 : 0.43;

  TLegend* leg = new TLegend(x1, y1, x2, y2);
  leg->SetTextSize(0.021);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetMargin(0.15);

  leg->AddEntry(box2s, "Expected #pm2#sigma", "f");
  leg->AddEntry(box1s, "Expected #pm1#sigma", "f");
  leg->AddEntry(lmed,  "Expected median", "l");
  if(observed){
    leg->AddEntry(lobs, "Observed", "l");
  }

  leg->AddEntry(lcms,
                observed ? "CMS, PRL 131 (2023) 011803 (obs.)"
                         : "CMS, PRL 131 (2023) 011803 (exp.)",
                "l");

  if(needAtlasEPJC){
    leg->AddEntry(latl,
                  observed ? "ATLAS, EPJC (2023) 83 824 (obs.)"
                           : "ATLAS, EPJC (2023) 83 824 (exp.)",
                  "l");
  }

  if(needAtlasPLB){
    leg->AddEntry(latl2,
                  observed ? "ATLAS, PLB 856 (2024) 138865 (obs.)"
                           : "ATLAS, PLB 856 (2024) 138865 (exp.)",
                  "l");
  }

  leg->Draw();
  c->RedrawAxis();

  std::string base("plots/" + std::string(WP) + "/" + std::string(WP) + "_Weinberg");
  base += observed ? "_obs" : "_exp";
  c->SaveAs((base + ".pdf").c_str());
  c->SaveAs((base + ".png").c_str());
}
