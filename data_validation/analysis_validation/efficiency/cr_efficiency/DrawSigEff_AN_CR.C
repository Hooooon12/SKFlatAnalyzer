#include "base_functions.h"
#include "Macros.h"
#include "mylib.h"
#include "canvas_margin.h"
#include "HNL_Efficiency_Plotter.cc"

void DrawSigEff_AN_CR();

void DrawConfig(TString DateTag, TString Flavour, TString era, TString ID, TString mode, TString input_dir);

vector<TString> GetMassList(TString mode){
  if(mode == "VBF") return { "300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000"};
  if(mode == "DY")  return { "90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000"};
  if(mode == "WW")  return { "500","600","700","800","900","1000","1100","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"};
  return {};
}

TString GetFilePattern(TString mode, TString Flavour, TString mass){
  if(mode == "VBF")
    return "HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_VBFTypeI_DF_M"+mass+"_private.root";

  if(mode == "DY")
    return "HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DYTypeI_DF_M"+mass+"_private.root";

  if(mode == "WW"){
    if(Flavour == "EMu")
      return "HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_SSWWTypeI_DF_M"+mass+"_private.root";
    else
      return "HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_SSWWTypeI_SF_M"+mass+"_private.root";
  }

  return "";
}

TString GetLabel(TString mode){
  if(mode == "VBF") return "W#gamma";
  if(mode == "DY")  return "CCDY";
  if(mode == "WW")  return "WW";
  return "";
}

void DrawSigEff_AN_CR(){

  vector<TString> flavours = {"MuMu","EE","EMu"};
  vector<TString> modes = {"VBF","DY","WW"};

  vector<TString> input_dirs = {
    "MultiLepton__RunSignal__",
    "BJet_WPT__MultiLepton__RunSignal__",
    "BJet_PUID__MultiLepton__RunSignal__",
    "BJet_AK8Clean__MultiLepton__RunSignal__"
  };

  for(auto flavour : flavours){
    for(auto mode : modes){
      for(auto indir : input_dirs){
        DrawConfig("April1", flavour, "2018", "HNL_ULIDv2", mode, indir);
      }
    }
  }
}

void DrawConfig(TString DateTag, TString Flavour, TString era, TString ID, TString mode, TString input_dir){

  HNL_Efficiency_Plotter Plotter("SignalIDEfficiency");
  Plotter.SetupPlotter(era,"",Flavour+"_ID_Efficiency");
  Plotter.CopyToWebsite = false;

  TString year = era;
  if(year.Contains("2016")) year = "2016";

  TString basepath = "/data6/Users/jalmond/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter/";
  TString path = basepath + year + "/" + input_dir + "/";

  vector<TString> masses = GetMassList(mode);

  TH1D *h_main = new TH1D("","",masses.size(),0,masses.size());
  h_main->GetXaxis()->SetTitle("m_{N} (GeV)");
  h_main->GetYaxis()->SetRangeUser(0,0.2);

  if(Flavour == "MuMu") h_main->GetYaxis()->SetTitle("Selection efficiency for mumu");
  else if(Flavour == "EE") h_main->GetYaxis()->SetTitle("Selection efficiency for ee");
  else h_main->GetYaxis()->SetTitle("Selection efficiency for emu");

  vector<TH1D*> hists;
  for(int i=0;i<11;i++) hists.push_back(new TH1D("","",masses.size(),0,masses.size()));

  for(unsigned int i=0;i<masses.size();i++){

    h_main->GetXaxis()->SetBinLabel(i+1,masses[i]);

    TString file = path + GetFilePattern(mode,Flavour,masses[i]);

    TH1D *hist_NoCut  = Plotter.ConstructHist(file,"ChannelCutFlow/"+ID+"/"+Flavour+"/ChannelDependant_Inclusive");
    TH1D *hist_Presel = Plotter.ConstructHist(file,"ChannelCutFlow/"+ID+"/"+Flavour+"/ChannelDependant_Presel");
    TH1D *hist_CR     = Plotter.ConstructHist(file,"ChannelCutFlow/"+ID+"/"+Flavour+"/CR_Summary");

    double denom = (hist_NoCut && hist_NoCut->Integral()>0) ? hist_NoCut->Integral() : 1.0;

    double EffPre = (hist_Presel) ? 0.1*hist_Presel->Integral()/denom : 0;
    h_main->SetBinContent(i+1,EffPre);

    int bins[] = {2,4,6,3,5,7,8,9,10,11,12};

    for(int j=0;j<11;j++){
      double val = (hist_CR) ? hist_CR->GetBinContent(bins[j])/denom : 0;
      hists[j]->SetBinContent(i+1,val);
    }

    cout << mode << " " << masses[i] << " Eff = " << EffPre << endl;
  }

  TCanvas* c = new TCanvas(Flavour+"_"+mode,"",1200,800);
  canvas_margin(c);
  c->SetRightMargin(0.11);

  hist_axis(h_main);

  h_main->SetLineWidth(2);
  h_main->SetLineStyle(2);
  h_main->Draw("l");

  int colors[] = {kRed,kRed+1,kRed+2,kBlue,kBlue+1,kBlue+2,kGreen+2,kGreen+3,kGreen+4,kMagenta,kOrange+1};

  for(unsigned int i=0;i<hists.size();i++){
    hists[i]->SetLineColor(colors[i]);
    hists[i]->SetLineWidth(2);
    if(i<3) hists[i]->SetLineStyle(1);
    else if(i<6) hists[i]->SetLineStyle(4);
    else hists[i]->SetLineStyle(5);
    hists[i]->Draw("lsame");
  }

  TLegend *lg = new TLegend(0.3,0.7,0.5,0.93);
  lg->SetFillStyle(0); lg->SetBorderSize(0); lg->SetTextSize(0.03);

  lg->AddEntry(h_main,"SS2l [*0.1]","l");
  lg->AddEntry(hists[0],"SS2l CR1 IB","l");
  lg->AddEntry(hists[1],"SS2l CR2 IB","l");
  lg->AddEntry(hists[2],"SS2l CR3 IB","l");
  lg->AddEntry(hists[3],"SS2l CR1 IM","l");
  lg->AddEntry(hists[4],"SS2l CR2 IM","l");
  lg->AddEntry(hists[5],"SS2l CR3 IM","l");

  TLegend *lg2 = new TLegend(0.6,0.7,0.93,0.93);
  lg2->SetFillStyle(0); lg2->SetBorderSize(0); lg2->SetTextSize(0.03);

  lg2->AddEntry(hists[6],"3l WZ CR1","l");
  lg2->AddEntry(hists[7],"3l WZ CR2","l");
  lg2->AddEntry(hists[8],"3l WZ CR3","l");
  lg2->AddEntry(hists[10],"3l Zgamma CR","l");
  lg2->AddEntry(hists[9],"4l ZZ CR","l");

  lg->Draw();
  lg2->Draw();

  TLatex t;
  t.SetNDC();
  t.SetTextSize(0.03);
  t.DrawLatex(0.2,0.88,GetLabel(mode));

  Plotter.mkdir(Plotter.plotpath);

  TString tag = input_dir;
  tag.ReplaceAll("MultiLepton__RunSignal__","");
  if(tag == "") tag = "Nominal";

  TString HNAME = DateTag+"_ControlRegion_SignalEff_"+mode+"_"+Flavour+"_"+ID+"_"+tag;

  c->SaveAs(Plotter.plotpath+"/"+HNAME+".pdf");
  c->SaveAs(Plotter.plotpath+"/"+HNAME+".png");
}
