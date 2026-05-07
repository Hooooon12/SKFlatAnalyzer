#include "base_functions.h"
#include "Macros.h"
#include "mylib.h"
#include "canvas_margin.h"
#include "HNL_Efficiency_Plotter.cc"                                                                       


///// Code to make AN plots for SR fractions

void DrawConfig(TString DatTag,TString flavour, TString year, TString filepath, TString ID);

void DrawSigEff_AN_DY_Xsec(){
  
  DrawConfig("Feb14","MuMu","2018","HNL_SR","HNL_ULIDv2");
  DrawConfig("Feb14","EE",  "2018","HNL_SR","HNL_ULIDv2");
  DrawConfig("Feb14","EMu", "2018","HNL_SR","HNL_ULIDv2");

  //  DrawConfig("Oct9","MuMu","2017","HNL_SR","HNTightV2");
  //DrawConfig("Oct9","EE","2017","HNL_SR","HNTightV2");

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
    
    
    vector<TString> GetMassListDY = {"85", "90","95","100", "125", "150","200", "250",  "300", "400", "500"};

   
    TH1D * h_eff_DY = new TH1D ("","",GetMassListDY.size(),0,GetMassListDY.size());
    h_eff_DY->GetXaxis()->SetTitle("m_{N} (GeV)");
    h_eff_DY->GetYaxis()->SetRangeUser(0,1);
    h_eff_DY->GetYaxis()->SetTitle("A.U.");

    TH1D * h_eff_DY_SR3 = new TH1D ("","",GetMassListDY.size(),0,GetMassListDY.size());
    
    TH1D * h_eff_DY_SR = new TH1D ("","",GetMassListDY.size(),0,GetMassListDY.size());
    
    // Define the data points (mass, cross-section)
    double mass[] = {1, 2, 3, 4, 5, 6,7, 8,9, 10, 11};
    double xsec2[] = {2.57E-01, 1.54E-01, 1.09E-01, 7.77E-02, 2.45E-02, 1.10E-02, 3.40E-03, 1.43E-03, 7.13E-04, 2.37E-04, 9.86E-05, 4.72E-05};
    double xsec3[] = {193.2799807, 131.4666535,92.90789133,66.10800419,20.74972105,9.327822441,2.862290182,1.199607032,0.5949455816,0.1962474301,0.08118343142};

    
    int n = sizeof(mass) / sizeof(mass[0]); // Number of points
    double xsec[n];
    for(int x=0; x < n; x++){
      xsec[x] = xsec3[x] * 0.25/1000;
    }
    
    // Create a TGraph
    TGraph* graph = new TGraph(n, mass, xsec);
    graph->SetTitle("Drell-Yan Cross Section vs Mass;Mass (GeV);Cross Section (pb)");
    graph->SetMarkerStyle(20);
    graph->SetMarkerSize(1);
    graph->SetMarkerColor(kBlue);
    graph->SetLineColor(kBlue);
    
    

    for(unsigned int i = 0 ; i < GetMassListDY.size() ; i++) h_eff_DY->GetXaxis()->SetBinLabel(i+1,GetMassListDY[i]);

    for(unsigned int i = 0 ; i < GetMassListDY.size(); i++){
      TString path_mass = path + "HNL_SignalRegion_Plotter_SkimTree_HNMultiLepBDT_DYTypeI_DF_M"+GetMassListDY[i] + "_private.root";
      
      double  NoCutValue (0);
      double  Presel (0);
      double  SR3 (0);

      
      TH1D *hist_NoCut        = Plotter.ConstructHist(path_mass,"ChannelCutFlow/"+ID+"/"+Flavour +"/ChannelDependant_Inclusive");
      TH1D *hist_NumPresel    = Plotter.ConstructHist(path_mass,"ChannelCutFlow/"+ID+"/"+Flavour+"/ChannelDependant_Presel");
      
      
      TH1D *hist_SR ;
      if(i < 10) hist_SR = Plotter.ConstructHist(path_mass,"ChannelCutFlow/"+ID+"/"+Flavour+"/SR_SummaryBDT");
      else hist_SR = Plotter.ConstructHist(path_mass,"ChannelCutFlow/"+ID+"/"+Flavour+"/SR_Summary");

      cout << path_mass << endl;
      cout << "DY " << GetMassListDY[i] << endl;
      cout << hist_NoCut << " " << hist_NumPresel << endl;
      
      cout << "ChannelCutFlow/"+ID+"/"+Flavour+"/ChannelDependant_Presel" << endl;
      cout << "Presel Yield = " << hist_NumPresel->Integral() << endl;

      double EffPre = (hist_NumPresel) ?  hist_NumPresel->Integral() / hist_NoCut->Integral() : 0;
      double EffSR3 = (hist_SR) ?  hist_SR->GetBinContent(5) / hist_NoCut->Integral() : 0;
      
      cout << "DY " << GetMassListDY[i] << " Eff = " << EffPre << endl;

      h_eff_DY_SR3->SetBinContent(i+1,  EffSR3);
      h_eff_DY_SR->SetBinContent(i+1,   10*xsec[i]*EffSR3);
    }
    
    
    TCanvas* c1 = new TCanvas(Flavour+"_DYEff", "", 1200, 800);
    c1->Draw();
    c1->cd();
    canvas_margin(c1);
    c1->SetRightMargin( 0.11 );
    
    //    h_eff_DY->GetYaxis()->SetTitle("#epsilon_{Signal}");
    
    hist_axis(h_eff_DY_SR3);
    
    h_eff_DY->GetYaxis()->SetRangeUser(0.001,10);
    h_eff_DY->Draw("l");

    h_eff_DY_SR3->GetXaxis()->SetRangeUser(80,200);
    h_eff_DY_SR3->SetLineWidth(2.0);
    h_eff_DY_SR3->SetLineStyle(23);
    h_eff_DY_SR3->Draw("lsame");
    
    h_eff_DY_SR->SetLineColor(kBlack);
    h_eff_DY_SR->SetLineStyle(4);
    h_eff_DY_SR->SetLineWidth(2);

    h_eff_DY_SR3->Draw("lsame");
    h_eff_DY_SR->Draw("lsame");

    // Create a Canvas to draw the graph                                                                                                                                                       
    graph->Draw("SAME P L"); // A=Axis, P=Points, L=Line                                                                                                                                          

    c1->SetLogy();
    // Update and show the canvas                                                                                                                                                            
    c1->Update();
    

    TLegend *lg = new TLegend(0.55, 0.8, 0.93, 0.93);
    lg->SetFillStyle(0);
    lg->SetBorderSize(0);
    lg->SetTextSize(0.03);
    lg->AddEntry(graph,"HNL Xsec","p");
    lg->AddEntry(h_eff_DY_SR3,"SS2l : SR3 Eff.","l");
    lg->AddEntry(h_eff_DY_SR,"SS2l  : XSEC*SR3 Eff.","l");
    lg->Draw();

    TLatex channelname;
    channelname.SetNDC();
    channelname.SetTextSize(0.03);
    channelname.DrawLatex(0.2, 0.88,"CCDY");

    cout << Plotter.plotpath << endl;
    Plotter.mkdir(Plotter.plotpath);

    TString HNAME = DateTag+"_SignalEff_DY_XSEC_"+Flavour + "_"+ID ;
    cout << Plotter.plotpath+"/"+HNAME+".pdf" << endl;
    c1->SaveAs(Plotter.plotpath+"/"+HNAME+".pdf");
    
  }
  
  return;
}



