#include <iostream>
#include <vector>

#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TPad.h"
#include "TGraphAsymmErrors.h"
#include "TGraph.h"
#include "TLatex.h"

using namespace std;

// ===============================
// Style
// ===============================
void setTDRStyle() {
  TStyle *tdrStyle = new TStyle("tdrStyle","Style");
  tdrStyle->SetOptStat(0);
  tdrStyle->SetCanvasBorderMode(0);
  tdrStyle->SetCanvasColor(kWhite);
  tdrStyle->SetPadBorderMode(0);
  tdrStyle->SetPadColor(kWhite);
  tdrStyle->SetHistLineWidth(2);
  tdrStyle->SetMarkerStyle(20);
  tdrStyle->SetPadTopMargin(0.05);
  tdrStyle->SetPadBottomMargin(0.13);
  tdrStyle->SetPadLeftMargin(0.16);
  tdrStyle->SetPadRightMargin(0.05);
  tdrStyle->cd();
}

// ===============================
void canvas_margin(TCanvas *c) {
  c->SetTopMargin(0.05);
  c->SetBottomMargin(0.13);
  c->SetLeftMargin(0.16);
  c->SetRightMargin(0.05);
}

// ===============================
void hist_axis(TH1D *hist){
  hist->SetTitle("");
  hist->GetYaxis()->SetTitleSize(0.04);
  hist->GetXaxis()->SetTitleSize(0.04);
}

// ===============================
TH1D* GetHist(TString filepath, TString histname){
  TFile* f = TFile::Open(filepath);
  if(!f || f->IsZombie()){
    cout << "Cannot open file: " << filepath << endl;
    return nullptr;
  }
  TH1D* h = (TH1D*)f->Get(histname);
  if(!h){
    cout << "Missing hist: " << histname << endl;
    return nullptr;
  }
  return (TH1D*)h->Clone();
}

// ===============================
void DrawConfig(TString Era, TString Type,
                TString sNum, TString sDen,
                TString lNum, TString lDen){

  TString path="data/";
  TString filePath = path + "Fake_" + Era + ".root";

  cout << "Running Era = " << Era << endl;
  cout << "Accessing " << filePath << endl;

  TH1D *hist_num = GetHist(filePath, Type+"/"+sNum);
  TH1D *hist_denom = GetHist(filePath, Type+"/"+sDen);

  if(!hist_num || !hist_denom) return;

  hist_num->GetXaxis()->SetRangeUser(20,500);
  hist_denom->GetXaxis()->SetRangeUser(20,500);

  TCanvas* c1 = new TCanvas("c1","",800,800);
  canvas_margin(c1);

  // ===============================
  // Ratio
  // ===============================
  TH1D *ratio_point = (TH1D *)hist_num->Clone("ratio");
  ratio_point->Divide(hist_denom);

  ratio_point->GetYaxis()->SetRangeUser(0.01,0.5);
  ratio_point->GetYaxis()->SetTitle("#epsilon_{Fake}");
  ratio_point->GetXaxis()->SetTitle("p_{T} (GeV)");

  hist_axis(ratio_point);
  ratio_point->Draw("hist");

  // ===============================
  // Extract FR at 80 GeV
  // ===============================
  double FR_80 = 0.0;

  for(int x = 1; x <= ratio_point->GetNbinsX(); x++){
    double low = ratio_point->GetBinLowEdge(x);
    double val = ratio_point->GetBinContent(x);

    if(x == ratio_point->FindBin(79)) FR_80 = val;

    cout << "Bin " << x
         << " lowEdge=" << low
         << " value=" << val << endl;
  }

  // ===============================
  // Build uncertainty band (grfr)
  // ===============================
  double frx[2] = {80, 500};
  double fry[2] = {FR_80, FR_80};

  double frx_err_low[2]  = {0.1, 0.1};
  double frx_err_high[2] = {0.1, 0.1};

  double fry_err_low[2];
  double fry_err_high[2];

  if(sNum.Contains("Muon")){
    fry_err_low[0]  = 0.2 * FR_80;
    fry_err_low[1]  = 0.2 * FR_80;
    fry_err_high[0] = 0.2 * FR_80;
    fry_err_high[1] = 0.2 * FR_80;
  }
  else{
    fry_err_low[0]  = 0.4 * FR_80;
    fry_err_low[1]  = 0.4 * FR_80;
    fry_err_high[0] = 0.8 * FR_80;
    fry_err_high[1] = 0.8 * FR_80;
  }

  TGraphAsymmErrors* grfr = new TGraphAsymmErrors(
    2, frx, fry,
    frx_err_low, frx_err_high,
    fry_err_low, fry_err_high
  );

  grfr->SetFillColor(kCyan);
  grfr->SetFillStyle(3002);
  grfr->Draw("E3 same");

  // redraw ratio on top
  ratio_point->Draw("same");

  // ===============================
  // Extrapolation line
  // ===============================
  double xline[2] = {80, 500};
  double yline[2] = {FR_80, FR_80};

  TGraph *g1 = new TGraph(2, xline, yline);
  g1->SetLineStyle(4);
  g1->Draw("same");

  // ===============================
  // Legend
  // ===============================
  TLegend *lg = new TLegend(0.6, 0.75, 0.93, 0.9);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.025);

  if(sNum.Contains("Muon")){
    lg->AddEntry(g1, "FR(extrap. 80 GeV)","l");
    lg->AddEntry(grfr, "FR(80 GeV) +/- 20%","f");
  }
  else{
    lg->AddEntry(g1, "FR(80 GeV)","l");
    lg->AddEntry(grfr, "FR(80 GeV) +80% -40%","f");
  }

  lg->Draw();

  // ===============================
  // Labels
  // ===============================
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.035);

  TString Flavour = (sNum.Contains("Muon")) ? "Muon" : "Electron";

  latex.DrawLatex(0.2, 0.88, Flavour);
  latex.DrawLatex(0.2, 0.83, Era);

  if(sNum.Contains("BB")) latex.DrawLatex(0.35, 0.88,"Barrel");
  if(sNum.Contains("EC")) latex.DrawLatex(0.35, 0.88,"Endcap");

  // ===============================
  // Save
  // ===============================
  TString outdir = "plots_pdf/";
  gSystem->mkdir(outdir, true);  // create if it doesn't exist
  
  TString outname = outdir + "FakeRate_" + Type + "_" + sNum + "_" + Era;
  c1->SaveAs(outname + ".pdf");

  TString outdir_png = "plots_png/";
  gSystem->mkdir(outdir_png, true);  // create if it doesn't exist                                                                                                                                                     

  TString outname_png = outdir_png + "FakeRate_" + Type + "_" + sNum + "_" + Era;
  c1->SaveAs(outname_png + ".png");
  
  cout << "Saved: " << outname << ".pdf" << endl;
}

// ===============================
void Plot_MCFake_Rate_HighPt_STANDALONE(){

  setTDRStyle();

  vector<TString> Etas = {"BB","EC"};

  for (auto ieta : Etas){

    // Electrons
    DrawConfig("2016","Fake_EE",
      "Electron_pt_"+ieta+"_HNL_HighPt_ULID_2016",
      "Electron_pt_"+ieta+"_HNL_HighPt_ULID_FO",
      "","");

    DrawConfig("2017","Fake_EE",
      "Electron_pt_"+ieta+"_HNL_HighPt_ULID_2017",
      "Electron_pt_"+ieta+"_HNL_HighPt_ULID_FO",
      "","");

    DrawConfig("2018","Fake_EE",
      "Electron_pt_"+ieta+"_HNL_HighPt_ULID_2018",
      "Electron_pt_"+ieta+"_HNL_HighPt_ULID_FO",
      "","");

    // Muons
    DrawConfig("2016","Fake_MuMu",
      "Muon_pt_"+ieta+"_HNL_ULID_2016",
      "Muon_pt_"+ieta+"_HNL_ULID_FO",
      "","");

    DrawConfig("2017","Fake_MuMu",
      "Muon_pt_"+ieta+"_HNL_ULID_2017",
      "Muon_pt_"+ieta+"_HNL_ULID_FO",
      "","");

    DrawConfig("2018","Fake_MuMu",
      "Muon_pt_"+ieta+"_HNL_ULID_2018",
      "Muon_pt_"+ieta+"_HNL_ULID_FO",
      "","");
  }
}
