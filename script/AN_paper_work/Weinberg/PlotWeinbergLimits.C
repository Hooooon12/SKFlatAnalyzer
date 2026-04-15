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
#include <cstdlib> // for std::strtod

//---------------------- Data structure ----------------------
struct Entry {
  double q025{0}, q16{0}, q50{0}, q84{0}, q975{0};
  bool hasCMS{false};   double cms{0};
  bool hasATLAS{false}; double atlas{0};
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

static bool parseDouble(const std::string& s, double& out){
  char* end=nullptr;
  out = std::strtod(s.c_str(), &end);
  return end && end!=s.c_str();
}
static bool isChannel(const std::string& s){
  return (s=="MuMu" || s=="EE" || s=="EMu");
}

//---------------------- Parser for the given format ----------------------
static std::map<std::string, Entry> ParseLimitsFile(const std::string& path){
  std::ifstream fin(path);
  if(!fin.is_open()){
    throw std::runtime_error("Cannot open file: " + path);
  }

  std::vector<std::string> rawLines;
  std::string line;
  while(std::getline(fin, line)){
    rawLines.push_back(trim(line));
  }
  fin.close();

  std::map<std::string, Entry> data;

  for (size_t i = 0; i < rawLines.size(); ){
    std::string tok = rawLines[i];
    if(tok.empty()){ ++i; continue; }

    if(isChannel(tok)){
      std::string ch = tok;
      ++i;

      // read 5 quantiles (skip empty lines)
      std::vector<double> q; q.reserve(5);
      while(i < rawLines.size() && q.size() < 5){
        if(rawLines[i].empty()){ ++i; continue; }
        double v;
        if(!parseDouble(rawLines[i], v)){
          throw std::runtime_error("Expected numeric quantile for "+ch+", got '"+rawLines[i]+"'");
        }
        q.push_back(v);
        ++i;
      }
      if(q.size()!=5){
        throw std::runtime_error("Found only "+std::to_string(q.size())+" quantiles for "+ch+" (need 5).");
      }

      Entry e;
      e.q025=q[0]; e.q16=q[1]; e.q50=q[2]; e.q84=q[3]; e.q975=q[4];

      // After quantiles: 0+ lines like "CMS 12.8" or "ATLAS 13.1" until next channel or blank+channel
      while(i < rawLines.size()){
        if(rawLines[i].empty()){
          size_t j=i+1;
          while(j<rawLines.size() && rawLines[j].empty()) ++j;
          if(j<rawLines.size() && isChannel(rawLines[j])){ i=j; break; }
          i = j;
          continue;
        }
        if(isChannel(rawLines[i])) break;

        std::istringstream iss(rawLines[i]);
        std::string label, valstr;
        if(!(iss >> label)) { ++i; continue; }
        if(!(iss >> valstr)) { ++i; continue; }

        double v;
        if(parseDouble(valstr, v)){
          if(label=="CMS"){ e.hasCMS = true; e.cms = v; }
          else if(label=="ATLAS"){ e.hasATLAS = true; e.atlas = v; }
        }
        ++i;
      }

      data[ch] = e;
    } else {
      ++i; // ignore stray text
    }
  }

  if(data.empty()) throw std::runtime_error("No channel data parsed; check file format.");
  return data;
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
                          int cmsPos = 11 /* 11=left, 33=right */) {
  pad->cd();

  float l = pad->GetLeftMargin();
  float t = pad->GetTopMargin();
  float r = pad->GetRightMargin();

  const int cmsFont = 61;
  const int extraFont = 52;
  const int lumiFont = 42;

  TLatex lumi;
  lumi.SetNDC(true);
  lumi.SetTextFont(lumiFont);
  lumi.SetTextSize(0.04);
  lumi.SetTextAlign(31);
  lumi.DrawLatex(1.0 - r, 1.0 - t + 0.01, lumiText.c_str());

  float cmsX = l + 0.01;
  float cmsY = 1.0 - t + 0.01;
  int align = 11;
  if(cmsPos==33){ cmsX = 1.0 - r - 0.01; align = 31; }

  TLatex cms;
  cms.SetNDC(true);
  cms.SetTextFont(cmsFont);
  cms.SetTextSize(0.06);
  cms.SetTextAlign(align);
  cms.DrawLatex(cmsX, cmsY, "CMS");

  if(!extraText.empty()){
    TLatex extra;
    extra.SetNDC(true);
    extra.SetTextFont(extraFont);
    extra.SetTextSize(0.045);
    extra.SetTextAlign(align);
    extra.DrawLatex(cmsX, cmsY - 0.055, extraText.c_str());
  }
}

//---------------------- Main drawing function ----------------------
void PlotWeinbergLimits(const char* limits_txt="Limits.txt",
                        const char* out_base="Weinberg_Limits",
                        const char* plot_title="Weinberg Expected Limits (Full Run 2)",
                        const char* lumi_text="138 fb^{-1} (13 TeV)",
                        const char* extra_text="Preliminary") {

  SetTDRStyle();

  // Parse data
  std::map<std::string, Entry> data = ParseLimitsFile(limits_txt);

  // Channel order
  std::vector<std::string> order = {"MuMu","EE","EMu"};
  for(const auto& kv: data){
    if(std::find(order.begin(), order.end(), kv.first)==order.end()){
      order.push_back(kv.first);
    }
  }

  const int N = (int)order.size();
  std::vector<double> y(N);
  for(int j=0;j<N;++j) y[j]=j;

  // X-range
  double xmin=1e9, xmax=0;
  auto widen = [&](double v){ xmin = std::min(xmin,v); xmax = std::max(xmax,v); };
  for(int j=0;j<N;++j){
    const Entry& e = data.at(order[j]);
    widen(e.q025); widen(e.q16); widen(e.q50); widen(e.q84); widen(e.q975);
    if(e.hasCMS) widen(e.cms);
    if(e.hasATLAS) widen(e.atlas);
  }
  double span = xmax - xmin;
  if(span<=0){ xmin -= 1; xmax += 1; span = xmax-xmin; }
  xmin -= 0.05*span;
  xmax += 0.15*span;

  // Canvas
  const int H = 480 + 120*N;
  TCanvas* c = new TCanvas("c","Weinberg Expected Limits", 900, H);
  c->SetLeftMargin(0.14);
  c->SetRightMargin(0.04);
  c->SetBottomMargin(0.13);
  c->SetTopMargin(0.10);

  // Frame
  TH2F* frame = new TH2F("frame","", 10, xmin, xmax, 10, -0.5, N-0.5);
  frame->SetTitle("");
  frame->GetXaxis()->SetTitle("m_{ll} [GeV]");
  frame->GetYaxis()->SetNdivisions(0);
  frame->GetXaxis()->SetTitleSize(0.05);
  frame->GetXaxis()->SetLabelSize(0.042);
  frame->Draw();

  // Bands and lines
  const double h2s = 0.28;
  const double h1s = 0.16;

  // Legend prototypes (objects used for legend keys)
  TBox* box2s = new TBox(0,0,1,1);  box2s->SetFillColorAlpha(kYellow, 0.7); box2s->SetLineColor(0);
  TBox* box1s = new TBox(0,0,1,1);  box1s->SetFillColorAlpha(kGreen+1, 0.8); box1s->SetLineColor(0);
  TLine* lmed  = new TLine(0,0,1,0); lmed->SetLineWidth(3); lmed->SetLineColor(kBlack);

  // CMS: dashed blue
  TLine* lcms  = new TLine(0,0,0,1); lcms->SetLineStyle(2); lcms->SetLineWidth(3); lcms->SetLineColor(kBlue+1);
  // ATLAS EPJC (MuMu): solid red
  TLine* latl  = new TLine(0,0,0,1); latl->SetLineStyle(7); latl->SetLineWidth(3); latl->SetLineColor(kRed+1);
  // ATLAS PLB (EE/EMu): dash-dot magenta
  TLine* latl2 = new TLine(0,0,0,1); latl2->SetLineStyle(7); latl2->SetLineWidth(3); latl2->SetLineColor(kMagenta+2);

  // Flags for which ATLAS legend entries to include
  bool needAtlasEPJC = false; // MuMu
  bool needAtlasPLB  = false; // EE, EMu

  // Draw per-channel objects
  for(int j=0;j<N;++j){
    const std::string& ch = order[j];
    const Entry& e = data.at(ch);
    double yc = y[j];

    // 2 sigma band
    TBox* b2 = new TBox(e.q025, yc - h2s, e.q975, yc + h2s);
    b2->SetFillColorAlpha(kYellow, 0.7);
    b2->SetLineColor(0);
    b2->Draw("same");

    // 1 sigma band
    TBox* b1 = new TBox(e.q16, yc - h1s, e.q84, yc + h1s);
    b1->SetFillColorAlpha(kGreen+1, 0.8);
    b1->SetLineColor(0);
    b1->Draw("same");

    // Median
    TLine* l50 = new TLine(e.q50, yc - h2s, e.q50, yc + h2s);
    l50->SetLineWidth(3);
    l50->SetLineColor(kBlack);
    l50->Draw("same");

    // CMS reference (optional)
    if(e.hasCMS){
      TLine* lc = new TLine(e.cms, yc - 0.34, e.cms, yc + 0.34);
      lc->SetLineStyle(2);      // dashed
      lc->SetLineWidth(4);
      lc->SetLineColor(kBlue+1);
      lc->Draw("same");
    }

    // ATLAS reference (optional) with channel-dependent style
    if(e.hasATLAS){
      if(ch == "MuMu"){
        // EPJC: solid red
        TLine* la = new TLine(e.atlas, yc - 0.34, e.atlas, yc + 0.34);
        la->SetLineStyle(7);
        la->SetLineWidth(4);
        la->SetLineColor(kRed+1);
        la->Draw("same");
        needAtlasEPJC = true;
      } else if(ch == "EE" || ch == "EMu"){
        // PLB: dash-dot magenta
        TLine* la2 = new TLine(e.atlas, yc - 0.34, e.atlas, yc + 0.34);
        la2->SetLineStyle(7);
        la2->SetLineWidth(4);
        la2->SetLineColor(kMagenta+2);
        la2->Draw("same");
        needAtlasPLB = true;
      }
    }
  }

  // Y-axis labels (channel names)
  TLatex lab;
  lab.SetTextFont(42);
  lab.SetTextSize(0.04);
  lab.SetTextAlign(32); // right-justified
  for(int j=0;j<N;++j){
    std::string label = order[j];
    if (label == "MuMu") label = "#mu#mu";
    if (label == "EMu")  label = "e#mu";
    if (label == "EE")   label = "ee";
    lab.DrawLatexNDC(0.135, 0.16 + (0.70/(N>1? (N-1):1))*j, label.c_str());
  }

  // CMS header / lumi
  DrawCMSHeader(c, lumi_text, extra_text, 11);

  // Legend anchored to the right margin (never crosses)
  double rmar = c->GetRightMargin();
  double legW = 0.33;  // width in NDC
  double legH = 0.26;  // height in NDC
  double x2 = 1.0 - rmar - 0.04;  // small padding from right edge
  double x1 = x2 - legW;
  double y1 = 0.18;
  double y2 = y1 + legH;

  TLegend* leg = new TLegend(x1, y1, x2, y2);
  leg->SetTextSize(0.025);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetMargin(0.15);

  leg->AddEntry(box2s, "Expected #pm2#sigma", "f");
  leg->AddEntry(box1s, "Expected #pm1#sigma", "f");
  leg->AddEntry(lmed,  "Expected median", "l");
  leg->AddEntry(lcms,  "CMS, PRL 131 (2023) 011803", "l");
  if(needAtlasEPJC){
    leg->AddEntry(latl,  "ATLAS, EPJC (2023) 83 824", "l");
  }
  if(needAtlasPLB){
    leg->AddEntry(latl2, "ATLAS, PLB 856 (2024) 138865", "l");
  }
  leg->Draw();

  c->RedrawAxis();

  std::string base(out_base);
  c->SaveAs((base + ".pdf").c_str());
  c->SaveAs((base + ".png").c_str());
}
