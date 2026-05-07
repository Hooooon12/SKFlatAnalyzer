#include "base_functions.h"
#include "Macros.h"
#include "mylib.h"
#include "canvas_margin.h"
#include "HNL_Efficiency_Plotter.cc"

///// Code to make AN plots for SR fractions

void DrawConfig(TString DatTag,TString flavour, TString year, TString filepath, TString ID);

void DrawSigEff_AN_DY(){
  
  DrawConfig("April1","MuMu","2018","HNL_SR","HNL_ULIDv2");
  DrawConfig("April1","EE",  "2018","HNL_SR","HNL_ULIDv2");
  DrawConfig("April1","EMu", "2018","HNL_SR","HNL_ULIDv2");
}

void DrawConfig(TString DateTag,TString Flavour, TString era, TString filepath, TString ID){

  vector<TString> eras = {era};

  for (auto year : eras){
    
    cout << "Running Era = " << year << endl;

    HNL_Efficiency_Plotter Plotter("SignalIDEfficiency");
    Plotter.SetupPlotter(year,"",Flavour+"_ID_Efficiency");
    Plotter.CopyToWebsite = false;

    if(year.Contains("2016")) year = "2016";

    TString path = "/data6/Users/jalmond/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter/" + year + "/MultiLepton__RunSignal__/";

    vector<TString> GetMassListDY = {
      "90","95","100","125","150","200","250","300","400","500","600",
      "700","800","900","1000","1100","1200","1300","1500","1700",
      "2000","2500","3000"
    };

    // Main histogram
    TH1D *h_eff_DY = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());
    h_eff_DY->GetXaxis()->SetTitle("m_{N} (GeV)");
    h_eff_DY->GetYaxis()->SetRangeUser(0,0.2);

    if(Flavour == "MuMu") h_eff_DY->GetYaxis()->SetTitle("Selection efficiency for mumu");
    else if(Flavour == "EE") h_eff_DY->GetYaxis()->SetTitle("Selection efficiency for ee");
    else h_eff_DY->GetYaxis()->SetTitle("Selection efficiency for emu");

    // CR histograms
    TH1D *h_eff_DY_CR1_IB = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());
    TH1D *h_eff_DY_CR2_IB = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());
    TH1D *h_eff_DY_CR3_IB = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());

    TH1D *h_eff_DY_CR1_IM = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());
    TH1D *h_eff_DY_CR2_IM = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());
    TH1D *h_eff_DY_CR3_IM = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());

    TH1D *h_eff_DY_CR1_WZ = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());
    TH1D *h_eff_DY_CR2_WZ = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());
    TH1D *h_eff_DY_CR3_WZ = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());

    TH1D *h_eff_DY_CR_ZZ = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());
    TH1D *h_eff_DY_CR_ZG = new TH1D("","",GetMassListDY.size(),0,GetMassListDY.size());

    // Bin labels
    for(unsigned int i = 0 ; i < GetMassListDY.size() ; i++){
      h_eff_DY->GetXaxis()->SetBinLabel(i+1,GetMassListDY[i]);
    }

    // Loop over masses
    for(unsigned int i = 0 ; i < GetMassListDY.size(); i++){

      TString path_mass = path + "HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DYTypeI_DF_M" + GetMassListDY[i] + "_private.root";

      TH1D *hist_NoCut     = Plotter.ConstructHist(path_mass,"ChannelCutFlow/"+ID+"/"+Flavour+"/ChannelDependant_Inclusive");
      TH1D *hist_Presel    = Plotter.ConstructHist(path_mass,"ChannelCutFlow/"+ID+"/"+Flavour+"/ChannelDependant_Presel");
      TH1D *hist_CR        = Plotter.ConstructHist(path_mass,"ChannelCutFlow/"+ID+"/"+Flavour+"/CR_Summary");

      double denom = (hist_NoCut && hist_NoCut->Integral() > 0) ? hist_NoCut->Integral()/3 : 1.0;

      double EffPre     = (hist_Presel) ?0.1*hist_Presel->Integral()/denom : 0;

      double EffCR1_IB  = (hist_CR) ? hist_CR->GetBinContent(2)/denom : 0;
      double EffCR2_IB  = (hist_CR) ? hist_CR->GetBinContent(4)/denom : 0;
      double EffCR3_IB  = (hist_CR) ? hist_CR->GetBinContent(6)/denom : 0;

      double EffCR1_IM  = (hist_CR) ? hist_CR->GetBinContent(3)/denom : 0;
      double EffCR2_IM  = (hist_CR) ? hist_CR->GetBinContent(5)/denom : 0;
      double EffCR3_IM  = (hist_CR) ? hist_CR->GetBinContent(7)/denom : 0;

      double EffCR1_WZ  = (hist_CR) ? hist_CR->GetBinContent(8)/denom : 0;
      double EffCR2_WZ  = (hist_CR) ? hist_CR->GetBinContent(9)/denom : 0;
      double EffCR3_WZ  = (hist_CR) ? hist_CR->GetBinContent(10)/denom : 0;

      double EffCR_ZZ   = (hist_CR) ? hist_CR->GetBinContent(11)/denom : 0;
      double EffCR_ZG   = (hist_CR) ? hist_CR->GetBinContent(12)/denom : 0;

      // Fill histograms
      h_eff_DY->SetBinContent(i+1,EffPre);

      h_eff_DY_CR1_IB->SetBinContent(i+1,EffCR1_IB);
      h_eff_DY_CR2_IB->SetBinContent(i+1,EffCR2_IB);
      h_eff_DY_CR3_IB->SetBinContent(i+1,EffCR3_IB);

      h_eff_DY_CR1_IM->SetBinContent(i+1,EffCR1_IM);
      h_eff_DY_CR2_IM->SetBinContent(i+1,EffCR2_IM);
      h_eff_DY_CR3_IM->SetBinContent(i+1,EffCR3_IM);

      h_eff_DY_CR1_WZ->SetBinContent(i+1,EffCR1_WZ);
      h_eff_DY_CR2_WZ->SetBinContent(i+1,EffCR2_WZ);
      h_eff_DY_CR3_WZ->SetBinContent(i+1,EffCR3_WZ);

      h_eff_DY_CR_ZZ->SetBinContent(i+1,EffCR_ZZ);
      h_eff_DY_CR_ZG->SetBinContent(i+1,EffCR_ZG);

      cout << "DY " << GetMassListDY[i] << " Eff = " << EffPre << endl;
    }

    // Canvas
    TCanvas* c1 = new TCanvas(Flavour+"_DYEff","",1200,800);
    canvas_margin(c1);
    c1->SetRightMargin(0.11);

    hist_axis(h_eff_DY);

    h_eff_DY->SetLineWidth(2);
    h_eff_DY->SetLineStyle(2);
    h_eff_DY->Draw("l");

    // Style + draw all
    vector<TH1D*> hists = {
      h_eff_DY_CR1_IB,h_eff_DY_CR2_IB,h_eff_DY_CR3_IB,
      h_eff_DY_CR1_IM,h_eff_DY_CR2_IM,h_eff_DY_CR3_IM,
      h_eff_DY_CR1_WZ,h_eff_DY_CR2_WZ,h_eff_DY_CR3_WZ,
      h_eff_DY_CR_ZZ,h_eff_DY_CR_ZG
    };

    int colors[] = {
      kRed,kRed+1,kRed+2,
      kBlue,kBlue+1,kBlue+2,
      kGreen+2,kGreen+3,kGreen+4,
      kMagenta,kOrange+1
    };

    for(unsigned int i=0;i<hists.size();i++){
      hists[i]->SetLineColor(colors[i]);
      hists[i]->SetLineWidth(2);
      if(i < 3)    hists[i]->SetLineStyle(1);
      else if(i < 6)    hists[i]->SetLineStyle(4);
      else  hists[i]->SetLineStyle(5);
      hists[i]->Draw("lsame");
    }

    // Legend
    TLegend *lg = new TLegend(0.3,0.7,0.5,0.93);
    lg->SetFillStyle(0);
    lg->SetBorderSize(0);
    lg->SetTextSize(0.03);

    lg->AddEntry(h_eff_DY,"SS2l [*0.1]","l");

    lg->AddEntry(h_eff_DY_CR1_IB,"SS2l CR1 IB","l");
    lg->AddEntry(h_eff_DY_CR2_IB,"SS2l CR2 IB","l");
    lg->AddEntry(h_eff_DY_CR3_IB,"SS2l CR3 IB","l");

    lg->AddEntry(h_eff_DY_CR1_IM,"SS2l CR1 IM","l");
    lg->AddEntry(h_eff_DY_CR2_IM,"SS2l CR2 IM","l");
    lg->AddEntry(h_eff_DY_CR3_IM,"SS2l CR3 IM","l");
    
    TLegend *lg2 = new TLegend(0.6,0.7,0.93,0.93);
    lg2->SetFillStyle(0);
    lg2->SetBorderSize(0);
    lg2->SetTextSize(0.03);

    lg2->AddEntry(h_eff_DY_CR1_WZ,"3l WZ CR1","l");
    lg2->AddEntry(h_eff_DY_CR2_WZ,"3l WZ CR2","l");
    lg2->AddEntry(h_eff_DY_CR3_WZ,"3l WZ CR3","l");

    lg2->AddEntry(h_eff_DY_CR_ZG,"3l Zgamma CR","l");
    lg2->AddEntry(h_eff_DY_CR_ZZ,"4l ZZ CR","l");

    lg->Draw();
    lg2->Draw();

    TLatex channelname;
    channelname.SetNDC();
    channelname.SetTextSize(0.03);
    channelname.DrawLatex(0.2,0.88,"CCDY");

    Plotter.mkdir(Plotter.plotpath);

    TString HNAME = DateTag+"_ControlRegion_SignalEff_DY_"+Flavour+"_"+ID;
    c1->SaveAs(Plotter.plotpath+"/"+HNAME+".pdf");
    c1->SaveAs(Plotter.plotpath+"/"+HNAME+".png");
  }
}
