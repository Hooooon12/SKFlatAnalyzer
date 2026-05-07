#include "base_functions.h"
#include "Macros.h"
#include "mylib.h"
#include "canvas_margin.h"
#include "HNL_Efficiency_Plotter.cc"                                                                       


//vector<double> GetMassListDY = {"85","90", "95","100", "125", "150","200", "250" ,"1000",  "1100", "1200","1300", "1500", "1700", "2000",  "2500", "3000", "300", "400", "500", "600", "700", "800", "900"};
//vector<double> GetMassListVBF = {"1000",  "1100", "1200","1300", "1500", "1700", "2000",  "2500", "3000", "300", "400", "500", "600", "700", "800", "900"};
//vector<double> GetMassListWW = {"1000",  "1100", "1300", "1500", "2000",  "2500", "500"};


void DrawConfig(TString DatTag,TString flavour, TString year, TString filepath, TString ID);

void DrawSigEff_AN_VBF(){
  
  DrawConfig("Sep8","MuMu","2018","HNL_SR","HNL_ULIDv2");
  DrawConfig("Sep8","EE",  "2018","HNL_SR","HNL_ULIDv2");
  DrawConfig("Sep8","EMu", "2018","HNL_SR","HNL_ULIDv2");

  //  DrawConfig("Nov8","MuMu","2017","HNL_SR","HNTightV2");
  //  DrawConfig("Nov8","EE","2017","HNL_SR","HNTightV2");

}

void DrawConfig(TString DateTag,TString Flavour, TString era, TString filepath,  TString ID){

  vector<TString> eras =  {era};

  for (auto year : eras){
    
    cout << "Running Era = " << year << endl;
    HNL_Efficiency_Plotter Plotter("SignalIDEfficiency");
    Plotter.SetupPlotter(year,"",Flavour+"_ID_Efficiency");
    Plotter.CopyToWebsite = false;
    
    if(year.Contains("2016")) year = "2016";

    TString path="/data6/Users/jalmond/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter/"+year+"/RunSignal__/";
    
    
    vector<TString> GetMassListDY = {"90","95","100", "125", "150","200", "250",  "300", "400", "500", "600", "700", "800", "900","1000",  "1100", "1200","1300", "1500", "1700", "2000",  "2500", "3000"};
    vector<TString> GetMassListVBF = { "300", "400", "500", "600", "700", "800", "900", "1000",  "1100", "1200","1300", "1500", "1700", "2000",  "2500", "3000"};
    vector<TString> GetMassListWW = {"500", "600","700","800","900","1000",  "1100", "1300", "1500","1700", "2000",  "2500", "3000","5000","7500","10000","15000","20000"};

   
    TH1D * h_eff_DY = new TH1D ("","",GetMassListVBF.size(),0,GetMassListVBF.size());
    h_eff_DY->GetXaxis()->SetTitle("m_{N} (GeV)");
    h_eff_DY->GetYaxis()->SetRangeUser(0,1);
    if(Flavour == "MuMu") h_eff_DY->GetYaxis()->SetTitle("m_{N} Selection efficiency for #mu#mu channel");
    else if(Flavour == "EE")h_eff_DY->GetYaxis()->SetTitle("m_{N} Selection efficiency for ee channel");
    else h_eff_DY->GetYaxis()->SetTitle("m_{N} Selection efficiency for e#mu channel");

    TH1D * h_eff_DY_SR1 = new TH1D ("","",GetMassListVBF.size(),0,GetMassListVBF.size());
    TH1D * h_eff_DY_SR2 = new TH1D ("","",GetMassListVBF.size(),0,GetMassListVBF.size());
    TH1D * h_eff_DY_SR3 = new TH1D ("","",GetMassListVBF.size(),0,GetMassListVBF.size());

    TH1D * h_eff_DY_SR = new TH1D ("","",GetMassListVBF.size(),0,GetMassListVBF.size());

    for(unsigned int i = 0 ; i < GetMassListVBF.size() ; i++) h_eff_DY->GetXaxis()->SetBinLabel(i+1,GetMassListVBF[i]);

    for(unsigned int i = 0 ; i < GetMassListVBF.size(); i++){
      TString path_mass = path + "HNL_SignalRegion_Plotter_SkimTree_HNMultiLepBDT_VBFTypeI_DF_M"+GetMassListVBF[i] + "_private.root";
      
      double  NoCutValue (0);
      double  Presel (0);
      double  SR1 (0);
      double  SR2 (0);
      double  SR3 (0);

      
      TH1D *hist_NoCut        = Plotter.ConstructHist(path_mass,"ChannelCutFlow/"+ID+"/"+Flavour +"/ChannelDependant_Inclusive");
      TH1D *hist_NumPresel    = Plotter.ConstructHist(path_mass,"ChannelCutFlow/"+ID+"/"+Flavour+"/ChannelDependant_Presel");
      TH1D *hist_SR          = Plotter.ConstructHist(path_mass,"ChannelCutFlow/"+ID+"/"+Flavour+"/SR_SummaryBDT");

      cout << path_mass << endl;
      cout << "DY " << GetMassListVBF[i] << endl;
      cout << hist_NoCut << " " << hist_NumPresel << endl;
      
      cout << "ChannelCutFlow/"+ID+"/"+Flavour+"/ChannelDependant_Presel" << endl;
      cout << "Presel Yield = " << hist_NumPresel->Integral() << endl;

      double EffPre = (hist_NumPresel) ?  hist_NumPresel->Integral() / hist_NoCut->Integral() : 0;
      double EffSR1 = (hist_SR)    ?  hist_SR->GetBinContent(3) / hist_NoCut->Integral() : 0;
      double EffSR2 = (hist_SR) ?  hist_SR->GetBinContent(4) / hist_NoCut->Integral() : 0;
      double EffSR3 = (hist_SR) ?  hist_SR->GetBinContent(5) / hist_NoCut->Integral() : 0;
      
      cout << "DY " << GetMassListVBF[i] << " Eff = " << EffPre << endl;

      h_eff_DY->SetBinContent(i+1,  EffPre);
      h_eff_DY_SR1->SetBinContent(i+1,  EffSR1);
      h_eff_DY_SR2->SetBinContent(i+1,  EffSR2);
      h_eff_DY_SR3->SetBinContent(i+1,  EffSR3);
      h_eff_DY_SR->SetBinContent(i+1,  EffSR1+EffSR2+EffSR3);

      //h_eff_DY_SR2->Fill(GetMassListVBF[i], EffSR2);
      //h_eff_DY_SR3->Fill(GetMassListVBF[i], EffSR3);

    }
    
    
    TCanvas* c1 = new TCanvas(Flavour+"_DYEff", "", 1200, 800);
    c1->Draw();
    c1->cd();
    canvas_margin(c1);
    c1->SetRightMargin( 0.11 );
    
    //    h_eff_DY->GetYaxis()->SetTitle("#epsilon_{Signal}");

    hist_axis(h_eff_DY);
  
    h_eff_DY->SetLineWidth(2.0);
    h_eff_DY->SetLineStyle(23);
    h_eff_DY->Draw("l");
    
    h_eff_DY_SR1->SetLineColor(kRed);
    //    h_eff_DY_SR1->SetLineStyle(24);
    h_eff_DY_SR1->SetLineWidth(2);
    
    h_eff_DY_SR2->SetLineColor(kOrange);
    //h_eff_DY_SR2->SetLineStyle(26);
    h_eff_DY_SR2->SetLineWidth(2);
    
    h_eff_DY_SR3->SetLineColor(kBlue);
    //    h_eff_DY_SR3->SetLineStyle(28);
    h_eff_DY_SR3->SetLineWidth(2);

    h_eff_DY_SR->SetLineColor(kBlack);
    h_eff_DY_SR->SetLineStyle(4);
    h_eff_DY_SR->SetLineWidth(2);

    h_eff_DY_SR1->Draw("lsame");
    h_eff_DY_SR2->Draw("lsame");
    h_eff_DY_SR3->Draw("lsame");
    h_eff_DY_SR->Draw("lsame");

    TLegend *lg = new TLegend(0.55, 0.8, 0.93, 0.93);
    lg->SetFillStyle(0);
    lg->SetBorderSize(0);
    lg->SetTextSize(0.03);
    lg->AddEntry(h_eff_DY,"SS2l","l");
    lg->AddEntry(h_eff_DY_SR1,"SS2l : SR1","l");
    lg->AddEntry(h_eff_DY_SR2,"SS2l : SR2","l");
    lg->AddEntry(h_eff_DY_SR3,"SS2l : SR3","l");
    lg->AddEntry(h_eff_DY_SR,"SS2l : SR1+2+3","l");
    lg->Draw();

    TLatex channelname;
    channelname.SetNDC();
    channelname.SetTextSize(0.03);
    channelname.DrawLatex(0.2, 0.88,"W#gamma");

    cout << Plotter.plotpath << endl;
    Plotter.mkdir(Plotter.plotpath);

    TString HNAME = DateTag + "_SignalEff_VBF_"+Flavour + "_"+ID ;
    cout << Plotter.plotpath+"/"+HNAME+".pdf" << endl;
    c1->SaveAs(Plotter.plotpath+"/"+HNAME+".pdf");
    
  }
  
  return;
}



