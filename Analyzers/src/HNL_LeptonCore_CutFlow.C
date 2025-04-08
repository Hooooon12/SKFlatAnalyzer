#include "HNL_LeptonCore.h"


void HNL_LeptonCore::FillCutflow2D(TString cutflow_dirname,TString cutflow_histname, double weight, vector<TString> bin_lables, TString fill_label){

  char end_str = string(cutflow_dirname).back();

  if ( end_str  == '/') {
    cout << "[HNL_LeptonCore::FillCutflowDef ] ERROR in assiging Hist name, remove / from end " << endl;
    cout << cutflow_dirname << endl;
    exit(EXIT_FAILURE);

  }
  TH2D *this_hist = GetHist2D(cutflow_dirname+"/"+cutflow_histname);

  if( !this_hist ){
    TString cf_name="Cutflows";
    //SR_Summary                                                                                                                                                                             
    //    cout << "cf_name = " << cf_name << " cutflow_dirname = " << cutflow_dirname << " cutflow_histname = " << cutflow_histname << endl;                                                 
    if(cutflow_histname.Contains("SR")||cutflow_histname.Contains("MuonCR")||cutflow_histname.Contains("ElectronCR") ) cf_name="LimitBins";
    if(cutflow_histname.Contains("SR_Cut")) cf_name="SignalCutFlow";
    
    if(cutflow_histname.Contains("Limit")) cf_name="LimitBins";
    if(!cutflow_dirname.Contains("ChannelCutFlow"))  cf_name = cutflow_dirname + "/"+cf_name;
    else cf_name = cutflow_dirname;

    //cout << "cf_name+cutflow_histname = " << cf_name+"/"+cutflow_histname << endl;                                                                                                         
    if(IsSignal()){
      if(cutflow_dirname.Contains("BDT")){
	this_hist = new TH2D(cf_name+"/"+cutflow_histname, "", bin_lables.size(), 0, bin_lables.size(), 11, 0, 11);
        for (unsigned int i=0 ; i < bin_lables.size(); i++)  this_hist->GetXaxis()->SetBinLabel(i+1,bin_lables[i]);
        this_hist->GetYaxis()->SetBinLabel(1,"M85");
        this_hist->GetYaxis()->SetBinLabel(2,"M90");
        this_hist->GetYaxis()->SetBinLabel(3,"M95");
        this_hist->GetYaxis()->SetBinLabel(4,"M100");
        this_hist->GetYaxis()->SetBinLabel(5,"M125");
        this_hist->GetYaxis()->SetBinLabel(6,"M150");
        this_hist->GetYaxis()->SetBinLabel(7,"M200");
        this_hist->GetYaxis()->SetBinLabel(8,"M250");
        this_hist->GetYaxis()->SetBinLabel(9,"M300");
        this_hist->GetYaxis()->SetBinLabel(10,"M400");
        this_hist->GetYaxis()->SetBinLabel(11,"M500");

      }
      else{
	this_hist = new TH2D(cf_name+"/"+cutflow_histname, "", bin_lables.size(), 0, bin_lables.size(), 8, 0, 8);
	for (unsigned int i=0 ; i < bin_lables.size(); i++)  this_hist->GetXaxis()->SetBinLabel(i+1,bin_lables[i]);
	this_hist->GetYaxis()->SetBinLabel(1,"DY_M100_250");
	this_hist->GetYaxis()->SetBinLabel(2,"DY_M300_500");
	this_hist->GetYaxis()->SetBinLabel(3,"DY_M600_1000");
	this_hist->GetYaxis()->SetBinLabel(4,"DY_M1000_3000");
	this_hist->GetYaxis()->SetBinLabel(5,"VBF_300_700");
	this_hist->GetYaxis()->SetBinLabel(6,"VBF_800_3000");
	this_hist->GetYaxis()->SetBinLabel(7,"SSWW");
	this_hist->GetYaxis()->SetBinLabel(8,"Weinberg");
      }
    }
    else{

      this_hist = new TH2D(cf_name+"/"+cutflow_histname, "", bin_lables.size(), 0, bin_lables.size(), 8, 0, 8);
      for (unsigned int i=0 ; i < bin_lables.size(); i++)  this_hist->GetXaxis()->SetBinLabel(i+1,bin_lables[i]);
      this_hist->GetYaxis()->SetBinLabel(1,"Total");
      this_hist->GetYaxis()->SetBinLabel(2,"CF");
      this_hist->GetYaxis()->SetBinLabel(3,"Fake");
      this_hist->GetYaxis()->SetBinLabel(4,"Conv");
      this_hist->GetYaxis()->SetBinLabel(5,"WZ");
      this_hist->GetYaxis()->SetBinLabel(6,"ZZ");
      this_hist->GetYaxis()->SetBinLabel(7,"WpWp");
      this_hist->GetYaxis()->SetBinLabel(8,"Prompt");
    }

    this_hist->SetDirectory(NULL);

    maphist_TH2D[cutflow_dirname+"/"+cutflow_histname] = this_hist;
  }

  TString bkg_label = "WZ";
  if(IsSignal()){
    if(cutflow_dirname.Contains("BDT")){
      if(!MCSample.Contains("Type")) return;
      if(MCSample.Contains("M85_")) bkg_label = "M85";
      else if(MCSample.Contains("M90_")) bkg_label = "M90";
      else if(MCSample.Contains("M95_")) bkg_label = "M95";
      else if(MCSample.Contains("M100_")) bkg_label = "M100";
      else if(MCSample.Contains("M125_")) bkg_label = "M125";
      else if(MCSample.Contains("M150_")) bkg_label = "M150";
      else if(MCSample.Contains("M200_")) bkg_label = "M200";
      else if(MCSample.Contains("M250_")) bkg_label = "M250";
      else if(MCSample.Contains("M300_")) bkg_label = "M300";
      else if(MCSample.Contains("M400_")) bkg_label = "M400";
      else if(MCSample.Contains("M500_")) bkg_label = "M500";
      else return;
    }
    else{
      if(MCSample.Contains("DYType")) {
	if(MCSample.Contains("M85_")) bkg_label = "DY_M100_250";
	if(MCSample.Contains("M90_")) bkg_label = "DY_M100_250";
	if(MCSample.Contains("M95_")) bkg_label = "DY_M100_250";
	if(MCSample.Contains("M100_")) bkg_label = "DY_M100_250";
	if(MCSample.Contains("M125_")) bkg_label = "DY_M100_250";
	if(MCSample.Contains("M150_")) bkg_label = "DY_M100_250";
	if(MCSample.Contains("M200_")) bkg_label = "DY_M100_250";
	if(MCSample.Contains("M250_")) bkg_label = "DY_M100_250";
	if(MCSample.Contains("M300_")) bkg_label = "DY_M300_500";
	if(MCSample.Contains("M400_")) bkg_label = "DY_M300_500";
	if(MCSample.Contains("M500_")) bkg_label = "DY_M300_500";
	if(MCSample.Contains("M600_")) bkg_label = "DY_M600_1000";
	if(MCSample.Contains("M700_")) bkg_label = "DY_M600_1000";
	if(MCSample.Contains("M800_")) bkg_label = "DY_M600_1000";
	if(MCSample.Contains("M900_")) bkg_label = "DY_M600_1000";
	if(MCSample.Contains("M1000_")) bkg_label = "DY_M600_1000";
	if(MCSample.Contains("M1100_")) bkg_label = "DY_M1000_3000";
	if(MCSample.Contains("M1200_")) bkg_label = "DY_M1000_3000";
	if(MCSample.Contains("M1300_")) bkg_label = "DY_M1000_3000";
	if(MCSample.Contains("M1500_")) bkg_label = "DY_M1000_3000";
	if(MCSample.Contains("M1700_")) bkg_label = "DY_M1000_3000";
	if(MCSample.Contains("M2000_")) bkg_label = "DY_M1000_3000";
	if(MCSample.Contains("M2500_")) bkg_label = "DY_M1000_3000";
	if(MCSample.Contains("M3000_")) bkg_label = "DY_M1000_3000";
      }
      else  if(MCSample.Contains("VBFType")) {
	if(MCSample.Contains("M300_")) bkg_label = "VBF_300_700";
	if(MCSample.Contains("M400_")) bkg_label = "VBF_300_700";
	if(MCSample.Contains("M500_")) bkg_label = "VBF_300_700";
	if(MCSample.Contains("M600_")) bkg_label = "VBF_300_700";
	if(MCSample.Contains("M700_")) bkg_label = "VBF_300_700";
	if(MCSample.Contains("M800_")) bkg_label = "VBF_800_3000";
	if(MCSample.Contains("M900_")) bkg_label = "VBF_800_3000";
	if(MCSample.Contains("M1000_")) bkg_label = "VBF_800_3000";
	if(MCSample.Contains("M1100_")) bkg_label = "VBF_800_3000";
	if(MCSample.Contains("M1200_")) bkg_label = "VBF_800_3000";
	if(MCSample.Contains("M1300_")) bkg_label = "VBF_800_3000";
	if(MCSample.Contains("M1500_")) bkg_label = "VBF_800_3000";
	if(MCSample.Contains("M1700_")) bkg_label = "VBF_800_3000";
	if(MCSample.Contains("M2000_")) bkg_label = "VBF_800_3000";
	if(MCSample.Contains("M2500_")) bkg_label = "VBF_800_3000";
	if(MCSample.Contains("M3000_")) bkg_label = "VBF_800_3000";      
      }
      else if(MCSample.Contains("SSWWTypeI")) {
	bkg_label = "SSWW";
      }
      else{
	bkg_label = "Weinberg";
      }
    }
  }
  else{
   
    ///// Non BDT plot labels
    if(RunFake)   bkg_label = "Fake";
    else if(RunCF)   bkg_label = "CF";
    else{
      if(MCSample.Contains("WZG"))   bkg_label = "Conv"; 
      else if(MCSample.Contains("WZ"))   bkg_label = "WZ";
      else if(MCSample.Contains("ZZ"))   bkg_label = "ZZ";
      else if(MCSample.Contains("WpWp"))   bkg_label = "WpWp";
      else if(MCSample.Contains("ZG"))   bkg_label = "Conv";
      else if(MCSample.Contains("WG"))   bkg_label = "Conv";
      else if(MCSample.Contains("TG"))   bkg_label = "Conv";
      else  bkg_label = "Prompt";
    }
    this_hist->Fill(fill_label,"Total", weight);
  }


  this_hist->Fill(fill_label,bkg_label, weight);


  return;
}



void HNL_LeptonCore::FillCutflowDef(TString cutflow_dirname,TString cutflow_histname, double weight, vector<TString> bin_lables, TString fill_label){

  //if(cutflow_dirname.Contains("MuMu") && cutflow_histname.Contains("ChannelDep") ) return;
  //if(cutflow_dirname.Contains("EE") && cutflow_histname.Contains("ChannelDep") ) return;
  //if(cutflow_dirname.Contains("EMu") && cutflow_histname.Contains("ChannelDep") ) return;
  
  char end_str = string(cutflow_dirname).back();

  if ( end_str  == '/') {
    cout << "[HNL_LeptonCore::FillCutflowDef ] ERROR in assiging Hist name, remove / from end " << endl;
    cout << cutflow_dirname << endl;
    exit(EXIT_FAILURE);

  }
  TH1D *this_hist = GetHist1D(cutflow_dirname+"/"+cutflow_histname);

  if( !this_hist ){
    TString cf_name="Cutflows";
    //SR_Summary
    //    cout << "cf_name = " << cf_name << " cutflow_dirname = " << cutflow_dirname << " cutflow_histname = " << cutflow_histname << endl;
    if(cutflow_histname.Contains("SR")||
       cutflow_histname.Contains("MuonCR")||
       cutflow_histname.Contains("ElectronCR")) cf_name="LimitBins";
    if(cutflow_histname.Contains("SR_Cut")) cf_name="SignalCutFlow";
  
    if(cutflow_histname.Contains("Limit")) cf_name="LimitBins";
    if(!cutflow_dirname.Contains("ChannelCutFlow"))  cf_name = cutflow_dirname + "/"+cf_name;
    else cf_name = cutflow_dirname;

    //cout << "cf_name+cutflow_histname = " << cf_name+"/"+cutflow_histname << endl;
    this_hist = new TH1D(cf_name+"/"+cutflow_histname, "", bin_lables.size(), 0, bin_lables.size());
    for (unsigned int i=0 ; i < bin_lables.size(); i++)  this_hist->GetXaxis()->SetBinLabel(i+1,bin_lables[i]);
    this_hist->SetDirectory(NULL);

    maphist_TH1D[cutflow_dirname+"/"+cutflow_histname] = this_hist;
  }
  this_hist->Fill(fill_label, weight);

}


void HNL_LeptonCore::FillCutflow(TString analysis_dir_name,TString histname, double weight, vector<TString> lables, TString label){
  FillCutflowDef(analysis_dir_name,histname, weight,lables, label);
  //  FillCutflowDef(analysis_dir_name,histname+"_unweighted", 1,lables, label);
}


void HNL_LeptonCore::FillCutflow(AnalyzerParameter param,TString histname, double weight, vector<TString> lables, TString label){
  
  if(param.Channel!="Default")FillCutflowDef(param.CutFlowDirIncChannel(),histname, weight,lables, label);
  FillCutflowDef(param.CutFlowDirChannel(),   histname, weight,lables, label);

  //FillCutflowDef(param.CutFlowDirChannel(),   histname+"_unweighted",1,lables, label);
  //if(param.Channel!="Default")FillCutflowDef(param.CutFlowDirIncChannel(),histname+"_unweighted", 1,lables, label);

}

void HNL_LeptonCore::FillLimitInput(HNL_LeptonCore::SearchRegion sr, double event_weight, TString label,  TString hist_path,TString bin_key, TString channel){

  vector<TString> lables = GetLimitLabelsFromRegion(sr,bin_key,channel);

  TString histname = GetCutFlowNameFromRegion(sr);

  FillCutflowDef(hist_path,   histname, event_weight,lables, label);
  FillCutflow2D (hist_path+"_2D",   histname, event_weight,lables, label);
  
  TString hist_pathLL = hist_path;
  hist_pathLL=hist_pathLL.ReplaceAll("EE","LL");
  hist_pathLL=hist_pathLL.ReplaceAll("MuMu","LL");
  hist_pathLL=hist_pathLL.ReplaceAll("EMu","LL");
  FillCutflowDef(hist_pathLL,   histname, event_weight,lables, label);


  /// Fill SingleBinned                                                                                                                                                                                           
  vector<HNL_LeptonCore::SearchRegion> SingleBinned = {MuonCR1,        MuonCR2,        MuonCR3,        MuonCR3BDT, 

						       MuonInvBJetCR1, MuonInvMETCR1,MuonInvBJetCR2, MuonInvMETCR2,MuonInvBJetCR3, MuonInvMETCR3,MuonInvBJetCR3BDT, MuonInvMETCR3BDT,
                                                       ElectronCR1,    ElectronCR2,    ElectronCR3,    ElectronCR3BDT, 
						       ElectronInvBJetCR1, ElectronInvMETCR1,ElectronInvBJetCR2, ElectronInvMETCR2,ElectronInvBJetCR3, ElectronInvMETCR3,ElectronInvBJetCR3BDT, ElectronInvMETCR3BDT,
						       ElectronMuonCR1,ElectronMuonCR2,ElectronMuonCR3,ElectronMuonCR3BDT, 
						       ElectronMuonInvBJetCR1, ElectronMuonInvMETCR1,ElectronMuonInvBJetCR2, ElectronMuonInvMETCR2,ElectronMuonInvBJetCR3, ElectronMuonInvMETCR3,ElectronMuonInvBJetCR3BDT, ElectronMuonInvMETCR3BDT,
						       MuonSR1,        MuonSR2,        MuonSR3,        MuonSR3BDT,
                                                       ElectronSR1,    ElectronSR2,    ElectronSR3,    ElectronSR3BDT,
                                                       ElectronMuonSR1,ElectronMuonSR2,ElectronMuonSR3,ElectronMuonSR3BDT};

  if(std::find(SingleBinned.begin(), SingleBinned.end(), sr) != SingleBinned.end()){
    FillCutflowDef(hist_path,   histname+"_SingleBin", event_weight,{"SingleBin"}, "SingleBin");
  }
  else{
    cout << "Limit plot error" << endl;
    exit(EXIT_FAILURE);
  }
  return;
}
void HNL_LeptonCore::FillCutflow(HNL_LeptonCore::SearchRegion sr, double event_weight, TString label,  TString hist_path){

  vector<TString> lables = GetLabelsFromRegion(sr);
  TString histname = GetCutFlowNameFromRegion(sr);
  
  FillCutflowDef(hist_path,   histname, event_weight,lables, label);

  if(sr==HNL_LeptonCore::SRLowMass && label.Contains("SR")) {
    /// If filling summary SR then fill incl.
    FillCutflowDef(hist_path,   histname, event_weight,lables, "Inclusive_SR");
  }
  if(sr==HNL_LeptonCore::SRHighMass && label.Contains("SR")) {
    /// If filling summary SR then fill incl.                                                                                                                                                                       
    FillCutflowDef(hist_path,   histname, event_weight,lables, "Inclusive_SR");
  }
  //FillCutflowDef(hist_path,   histname+"_unweighted", 1,lables, label);

}


void HNL_LeptonCore::FillCutflow(HNL_LeptonCore::SearchRegion sr, double event_weight, TString label,  AnalyzerParameter param){
  
  TString OutputDir= param.CutFlowDirChannel();
  
  vector<TString> lables = GetLabelsFromRegion(sr);
  TString histname = GetCutFlowNameFromRegion(sr);
  FillCutflowDef(OutputDir,   histname, event_weight,lables, label);
  if(param.Channel!="Default") FillCutflowDef(param.CutFlowDirIncChannel(),   histname, event_weight,lables, label);

  //  FillCutflowDef(OutputDir,   histname+"_unweighted", 1,lables, label);
  //  if(param.Channel!="Default") FillCutflowDef(param.CutFlowDirIncChannel(),   histname+"_unweighted", 1,lables, label);


  return;
}


void HNL_LeptonCore::FillFullTypeCutflow(TString histname, double weight, vector<TString> lables, TString label1, TString label2){

  TH2D *this_hist = GetHist2D(histname);
  if( !this_hist ){
    this_hist = new TH2D("FillEventFullType/"+histname, "", lables.size(), 0, lables.size(),  lables.size(), 0, lables.size());
    this_hist->SetDirectory(NULL);
    for (unsigned int i=0 ; i < lables.size(); i++)  this_hist->GetXaxis()->SetBinLabel(i+1,lables[i]);
    for (unsigned int i=0 ; i < lables.size(); i++)  this_hist->GetYaxis()->SetBinLabel(i+1,lables[i]);

    maphist_TH2D[histname] = this_hist;
  }

  this_hist->Fill(label1, label2, fabs(weight));

}


void HNL_LeptonCore::FillTypeCutflow(TString histname, double weight, vector<TString> lables, TString label1, TString label2){

  TH2D *this_hist = GetHist2D(histname);
  if( !this_hist ){
    this_hist = new TH2D("CR2DCutflow/"+histname, "", lables.size(), 0, lables.size(),  lables.size(), 0, lables.size());
    this_hist->SetDirectory(NULL);
    for (unsigned int i=0 ; i < lables.size(); i++)  this_hist->GetXaxis()->SetBinLabel(i+1,lables[i]);
    for (unsigned int i=0 ; i < lables.size(); i++)  this_hist->GetYaxis()->SetBinLabel(i+1,lables[i]);

    maphist_TH2D[histname] = this_hist;
  }

  this_hist->Fill(label1, label2, fabs(weight));

}


void HNL_LeptonCore::FillCutFlow(bool IsCentral, TString suffix, TString histname, double ev_weight){

  if(IsCentral)   FillHist(suffix+"/"+histname, 0., ev_weight, 1, 0., 1.);

  return ;
}



TString HNL_LeptonCore::GetCutFlowNameFromRegion(HNL_LeptonCore::SearchRegion sr){
  
  TString EVHistName = "";
  if(sr==WZCR)    EVHistName= "WZCR";
  if(sr==WZBCR)   EVHistName= "WZBCR";
  if(sr==ZZCR)    EVHistName= "ZZCR";
  if(sr==WGCR)    EVHistName= "WGCR";
  if(sr==ZGCR)    EVHistName= "ZGCR";
  if(sr==WZBCR)   EVHistName= "WZBCR";
  if(sr==WZVBFCR)   EVHistName= "WZVBFCR";
  if(sr==WZVBFCR2)  EVHistName= "WZVBFCR2";
  if(sr==HMCR1)   EVHistName= "HMCR1";
  if(sr==HMCR2)   EVHistName= "HMCR2";
  if(sr==HMCR3)   EVHistName= "HMCR3";
  if(sr==HMNPCR)  EVHistName= "HMNPCR";
  if(sr==HMBCR)   EVHistName= "HMBCR";
  if(sr==HM1JCR)   EVHistName= "HM1JCR";
  if(sr==Presel)   EVHistName= "Presel";
  if(sr==WWNP1CR)   EVHistName= "WWNP1CR";
  if(sr==WWNP2CR)   EVHistName= "WWNP2CR";
  if(sr==WWNP3CR)   EVHistName= "WWNP3CR";
  if(sr==WWCR)   EVHistName= "WWCR";
  if(sr==ZAK8CR)   EVHistName= "ZAK8CR";
  if(sr==ZCR)   EVHistName= "ZCR";
  if(sr==ZNPElCR)   EVHistName= "ZNPElCR";
  if(sr==ZNPMuCR)   EVHistName= "ZNPMuCR";
  if(sr==TopCR)   EVHistName= "TopCR";
  if(sr==TopNPCR)   EVHistName= "TopNPCR";
  if(sr==TopAK8NPCR)   EVHistName= "TopAK8NPCR";
  if(sr==TopNPCR2)   EVHistName= "TopNPCR2";


  ///// SR Names

  if(sr==SR1)    EVHistName= "Cutflow_SR1";
  if(sr==SR2)    EVHistName= "Cutflow_SR2";
  if(sr==SR3)    EVHistName= "Cutflow_SR3";
  if(sr==SR3BDT) EVHistName= "Cutflow_SR3BDT";

  if(sr==CR1)    EVHistName= "Cutflow_CR1";
  if(sr==CR2)    EVHistName= "Cutflow_CR2";
  if(sr==CR3)    EVHistName= "Cutflow_CR3";
  if(sr==CR3BDT) EVHistName= "Cutflow_CR3BDT";

  if(sr==MuonSR1)   EVHistName ="MuonSR1";
  if(sr==MuonSR2)   EVHistName ="MuonSR2";
  if(sr==MuonSR3)   EVHistName ="MuonSR3";
  if(sr==ElectronSR1 )   EVHistName ="ElectronSR1";
  if(sr==ElectronSR2 )   EVHistName ="ElectronSR2";
  if(sr==ElectronSR3 )   EVHistName ="ElectronSR3";
  if(sr==ElectronMuonSR1)   EVHistName ="ElectronMuonSR1";
  if(sr==ElectronMuonSR2)   EVHistName ="ElectronMuonSR2";
  if(sr==ElectronMuonSR3)   EVHistName ="ElectronMuonSR3";

  if(sr==MuonSR3BDT)   EVHistName ="MuonSR3BDT";
  if(sr==ElectronSR3BDT)   EVHistName ="ElectronSR3BDT";
  if(sr==ElectronMuonSR3BDT)   EVHistName ="ElectronMuonSR3BDT";


  ///// CR Names                                                                                     
  if(sr==MuonCR1)   EVHistName ="MuonCR1";
  if(sr==MuonCR2)   EVHistName ="MuonCR2";
  if(sr==MuonCR3)   EVHistName ="MuonCR3";
  if(sr==ElectronCR1 )   EVHistName ="ElectronCR1";
  if(sr==ElectronCR2 )   EVHistName ="ElectronCR2";
  if(sr==ElectronCR3 )   EVHistName ="ElectronCR3";
  if(sr==ElectronMuonCR1)   EVHistName ="ElectronMuonCR1";
  if(sr==ElectronMuonCR2)   EVHistName ="ElectronMuonCR2";
  if(sr==ElectronMuonCR3)   EVHistName ="ElectronMuonCR3";
  if(sr==MuonCR3BDT)   EVHistName ="MuonCR3BDT";
  if(sr==ElectronCR3BDT)   EVHistName ="ElectronCR3BDT";
  if(sr==ElectronMuonCR3BDT)   EVHistName ="ElectronMuonCR3BDT";

  if(sr==MuonInvBJetCR1)   EVHistName ="MuonInvBJetCR1";
  if(sr==ElectronInvBJetCR1)   EVHistName ="ElectronInvBJetCR1";
  if(sr==ElectronMuonInvBJetCR1)   EVHistName ="ElectronMuonInvBJetCR1";

  if(sr==MuonInvMETCR1)   EVHistName ="MuonInvMETCR1";
  if(sr==ElectronInvMETCR1)   EVHistName ="ElectronInvMETCR1";
  if(sr==ElectronMuonInvMETCR1)   EVHistName ="ElectronMuonInvMETCR1";

  if(sr==MuonInvBJetCR2)   EVHistName ="MuonInvBJetCR2";
  if(sr==ElectronInvBJetCR2)   EVHistName ="ElectronInvBJetCR2";
  if(sr==ElectronMuonInvBJetCR2)   EVHistName ="ElectronMuonInvBJetCR2";

  if(sr==MuonInvMETCR2)   EVHistName ="MuonInvMETCR2";
  if(sr==ElectronInvMETCR2)   EVHistName ="ElectronInvMETCR2";
  if(sr==ElectronMuonInvMETCR2)   EVHistName ="ElectronMuonInvMETCR2";

  if(sr==MuonInvBJetCR3)   EVHistName ="MuonInvBJetCR3";
  if(sr==ElectronInvBJetCR3)   EVHistName ="ElectronInvBJetCR3";
  if(sr==ElectronMuonInvBJetCR3)   EVHistName ="ElectronMuonInvBJetCR3";

  if(sr==MuonInvMETCR3)   EVHistName ="MuonInvMETCR3";
  if(sr==ElectronInvMETCR3)   EVHistName ="ElectronInvMETCR3";
  if(sr==ElectronMuonInvMETCR3)   EVHistName ="ElectronMuonInvMETCR3";


  if(sr==MuonInvBJetCR3BDT)   EVHistName ="MuonInvBJetCR3BDT";
  if(sr==ElectronInvBJetCR3BDT)   EVHistName ="ElectronInvBJetCR3BDT";
  if(sr==ElectronMuonInvBJetCR3BDT)   EVHistName ="ElectronMuonInvBJetCR3BDT";

  if(sr==MuonInvMETCR3BDT)   EVHistName ="MuonInvMETCR3BDT";
  if(sr==ElectronInvMETCR3BDT)   EVHistName ="ElectronInvMETCR3BDT";
  if(sr==ElectronMuonInvMETCR3BDT)   EVHistName ="ElectronMuonInvMETCR3BDT";


  /// Channel Summary plots
  if(sr==ChannelDepInc )    EVHistName ="ChannelDependant_Inclusive";
  if(sr==ChannelDepDilep)   EVHistName ="ChannelDependant_Dilep";
  if(sr==ChannelDepTrigger) EVHistName ="ChannelDependant_Trigger";
  if(sr==ChannelDepPresel)  EVHistName ="ChannelDependant_Presel";
  if(sr==ChannelDepSR1)     EVHistName ="ChannelDependant_SR1";
  if(sr==ChannelDepSR2)     EVHistName ="ChannelDependant_SR2";
  if(sr==ChannelDepSR3)     EVHistName ="ChannelDependant_SR3_BDT";
  if(sr==ChannelDepSR3HM)   EVHistName ="ChannelDependant_SR3_HighMass";

  if(sr==SRLowMass)         EVHistName  ="SR_SummaryBDT";
  if(sr==SRHighMass)        EVHistName ="SR_Summary";

  if(sr==ChannelDepCR1)     EVHistName ="ChannelDependant_CR1";
  if(sr==ChannelDepCR2)     EVHistName ="ChannelDependant_CR2";
  if(sr==ChannelDepCR3)     EVHistName ="ChannelDependant_CR3_BDT";
  if(sr==ChannelDepCR3HM)   EVHistName ="ChannelDependant_CR3_HighMass";
  
  //// Region plots
  if(sr == ControlRegion)   EVHistName = "ValidationRegionFlow";
  if(sr == SignalRegion)    EVHistName = "SignalRegionFlow";

    
  if(sr==CR)    EVHistName ="ControlRegions";
  if(sr==CRFull)    EVHistName ="ControlRegions";
  if(sr==sigmm) EVHistName ="SR_CutFlow";
  if(sr==sigee) EVHistName ="SR_CutFlow";
  if(sr==sigem) EVHistName ="SR_CutFlow";
  if(sr==sigmm_17028) EVHistName ="SR_CutFlow_17028";
  if(sr==sigee_17028) EVHistName ="SR_CutFlow_17028";

  return EVHistName;
}



vector<TString>  HNL_LeptonCore::GetLimitLabelsFromRegion(HNL_LeptonCore::SearchRegion sr, TString bin_key, TString channel){

  vector<TString> labels;
  
  //// SR1 is channel dependant
  vector<TString> SR1labels = GetLimitLabels("SR1",channel);
  vector<TString> SR2labels = GetLimitLabels("SR2");
  vector<TString> SR3labels = GetLimitLabels("SR3",channel);
 
  vector<TString> CR1labels =  GetLimitLabels("CR1");
  vector<TString> CR2labels =  GetLimitLabels("CR2");
  vector<TString> CR3labels =  GetLimitLabels("CR3");

  if(sr==MuonSR1    || sr==ElectronSR1   || sr==ElectronMuonSR1)      return SR1labels;
  if(sr==MuonSR2    || sr==ElectronSR2   || sr==ElectronMuonSR2)      return SR2labels;
  if(sr==MuonSR3    || sr==ElectronSR3   || sr==ElectronMuonSR3)      return SR3labels;

  if(sr==MuonCR1    || sr==ElectronCR1   || sr==ElectronMuonCR1)      return CR1labels;
  if(sr==MuonCR2    || sr==ElectronCR2   || sr==ElectronMuonCR2)      return CR2labels;
  if(sr==MuonCR3    || sr==ElectronCR3   || sr==ElectronMuonCR3)      return CR3labels;

  if(sr==MuonInvBJetCR1 || sr==ElectronInvBJetCR1 || sr==ElectronMuonInvBJetCR1)  return  CR1labels;
  if(sr==MuonInvMETCR1 || sr==ElectronInvMETCR1 || sr==ElectronMuonInvMETCR1)  return  CR1labels;
  
  if(sr==MuonInvBJetCR2 || sr==ElectronInvBJetCR2 || sr==ElectronMuonInvBJetCR2)  return  CR2labels;
  if(sr==MuonInvMETCR2 || sr==ElectronInvMETCR2 || sr==ElectronMuonInvMETCR2)  return  CR2labels;

  if(sr==MuonInvBJetCR3 || sr==ElectronInvBJetCR3 || sr==ElectronMuonInvBJetCR3)  return  CR3labels;
  if(sr==MuonInvMETCR3 || sr==ElectronInvMETCR3 || sr==ElectronMuonInvMETCR3)  return  CR3labels;


  vector<TString> SR3BDTlabels = GetBDTLimitLabels(bin_key);
  if(sr==MuonSR3BDT || sr==ElectronSR3BDT || sr==ElectronMuonSR3BDT)  return SR3BDTlabels;


  if(sr==MuonCR3BDT || sr==ElectronCR3BDT || sr==ElectronMuonCR3BDT)  return  GetBDTLimitLabels("CR3BDT_"+channel);
  if(sr==MuonInvBJetCR3BDT || sr==ElectronInvBJetCR3BDT || sr==ElectronMuonInvBJetCR3BDT)  return  GetBDTLimitLabels("CR3BDT_"+channel);
  if(sr==MuonInvMETCR3BDT || sr==ElectronInvMETCR3BDT || sr==ElectronMuonInvMETCR3BDT)  return  GetBDTLimitLabels("CR3BDT_"+channel);

  return {""};

}

vector<TString>  HNL_LeptonCore::GetLabelsFromRegion(HNL_LeptonCore::SearchRegion sr){
  
  vector<TString> labels;

  //// Hard code nsteps for each CR  
  int nstep(0);
  if(sr==WGCR)  nstep = 10;
  if(sr==ZGCR)  nstep = 11;
  if(sr==WZCR)  nstep = 11;
  if(sr==ZZCR)  nstep = 6;
  if(sr==WGCR)   nstep = 10;
  if(sr==WZBCR)   nstep = 15;
  if(sr==WZVBFCR)   nstep = 7;
  if(sr==WZVBFCR2)   nstep = 7;
  if(sr==HMCR1)   nstep = 7;
  if(sr==HMCR2)   nstep = 7;
  if(sr==HMCR3)   nstep = 8;
  if(sr==HMNPCR)   nstep = 7;
  if(sr==HMBCR)   nstep = 5;
  if(sr==HM1JCR)   nstep = 5;
  if(sr==Presel)   nstep = 5; 
  if(sr==WWNP1CR)   nstep = 11; 
  if(sr==WWNP2CR)   nstep = 10; 
  if(sr==WWNP3CR)   nstep = 10; 
  if(sr==WWCR)   nstep = 10; 
  if(sr==ZAK8CR)   nstep = 7;
  if(sr==ZCR)   nstep = 5; 
  if(sr==ZNPElCR)   nstep = 7; 
  if(sr==ZNPMuCR)   nstep = 7;
  if(sr==TopNPCR)   nstep = 8; 
  if(sr==TopCR)   nstep = 7; 
  if(sr==TopAK8NPCR)   nstep = 7; 
  if(sr==TopNPCR2)   nstep = 7;

  if(nstep > 0) {
    for(int i=0; i < nstep; i++) labels.push_back("Step"+to_string(i));
    return labels;
  }
 
  if(sr==SR1 || sr== CR1  )   {
    labels = {  "SR1_Init","SR1_lep_charge","SR1_lep_pt","SR1_dilep_mass" ,"SR1_1AK8" ,"SR1_MET" ,"SR1_bveto","SR1_Wmass"};
    if( sr== CR1) labels = ConvertCutFlowLabels(labels,"SR","CR");
  }
  
  if(sr==SR2  || sr==CR2 ) {
    
    labels = {  "SR2_lep_charge",  "SR2_lep_pt",  "SR2_DPhi",  "SR2_LLMass", "SR2_DiJet", "SR2_DiJetEta", "SR2_DiJetMass", "SR2_Zep", "SR2_VBF","SR2_met","SR2_bveto", "SR2_ht_lt1"};
    
    if( sr== CR2) labels = ConvertCutFlowLabels(labels,"SR","CR");
  }
  
  if( sr==SR3 || sr==CR3 ) {
    labels = { "SR3_lep_charge" ,"SR3_lep_pt", "SR3_tauveto","SR3_dilep_mass","SR3_MET","SR3_bveto", "SR3_J1Pt", "SR3_L1Pt","SR3_lowjet","SR3_dijet"};
    if( sr== CR3) labels = ConvertCutFlowLabels(labels,"SR","CR");
  }

  if(sr==MuonSRSummary) labels = {"Inclusive", "GenMatch", "CheckLeptonFlavourForChannel","METFilter", "CFCut", "Preselection", "AK8","SigReg1", "SigReg1Fail",  "SigReg2",  "SigReg3", "SigReg3Pass"};
  
  if( sr==SR3BDT || sr==CR3BDT )  {
    labels = {  "SR3_lep_charge" , "SR3_lep_pt",  "SR3_dilep_mass", "SR3_MET", "SR3_bveto"};//, "SR3_jet", "SR3_dijet","SR3_Wmass",  "SR3_J1Pt", "SR3_MET", "SR3_bveto"};
    if( sr== CR3BDT) labels = ConvertCutFlowLabels(labels,"SR","CR");
 

  }
  
  /// Channel Summary Labels

  if(sr==ChannelDepInc )  labels = {"MuMu_NoCut","EE_NoCut","EMu_NoCut"};
  
  if(sr==ChannelDepIncQ ) labels = {"MuMu_MMQ_NoCut","MuMu_PPQ_NoCut", "EE_MMQ_NoCut",  "EE_PPQ_NoCut","EMu_MMQ_NoCut", "EMu_PPQ_NoCut"};
  
  if(sr==ChannelDepDilep)labels = {"MuMu_Dilep","EE_Dilep","EMu_Dilep"};
  
  if(sr==ChannelDepTrigger) labels = {"MuMu_Trigger","EE_Trigger","EMu_Trigger", "MuMu_MultiTrigger","EE_MultiTrigger","EMu_MultiTrigger"};
  
  if(sr==ChannelDepPresel) labels = {"MuMu_Presel","EE_Presel","EMu_Presel"};
  
  if(sr==ChannelDepSR1)   labels = {"MuMu_SR1","EE_SR1","EMu_SR1"};
  if(sr==ChannelDepSR2)   labels = {"MuMu_SR2","EE_SR2","EMu_SR2"};
  if(sr==ChannelDepSR3)   labels = {"MuMu_SR3","EE_SR3","EMu_SR3"};
  if(sr==ChannelDepSR3HM) labels = {"MuMu_SR3_HM","EE_SR3_HM","EMu_SR3_HM"};
  
  if(sr==ChannelDepCR1)      labels = {"MuMu_CR1","EE_CR1","EMu_CR1"};
  if(sr==ChannelDepCR2)      labels = {"MuMu_CR2","EE_CR2","EMu_CR2"};
  if(sr==ChannelDepCR3)      labels = {"MuMu_CR3","EE_CR3","EMu_CR3"};
  if(sr==ChannelDepCR3HM)    labels = {"MuMu_CR3_HM","EE_CR3_HM","EMu_CR3_HM"};

  //// SR Labels
  if(sr==SRLowMass)    labels = {"Preselection","Inclusive_SR","SR1","SR2","SR3_LowMass"};
  if(sr==SRHighMass)   labels = {"Preselection","Inclusive_SR","SR1","SR2","SR3_HighMass"};
  if(sr == SignalRegion)     labels = {"NoCut","Trigger", "GENMatched","HEMVeto","METFilter","LeptonFlavour","TauVeto","SSLepton","LepVeto","DiLepMass", "BJet","MET"};
  /// CR lanels 
  if(sr == ControlRegion)    labels = {"NoCut","HEMVeto","METFilter","GENMatched","LeptonFlavour","TauVeto","Trigger","OS_VR","VV_VR","VG_VR","SS_CR","VBF_CR"};

  //                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
  if(sr==CR)   labels = {  "ZZ_CR","ZG_CR","WG_CR","WZ_SR1", "WZ_SR2", "WZ_SR3", "SR1_InvMET", "SR1_InvBJet","SR2_InvMET", "SR2_InvBJet","SR3_InvMET", "SR3_InvBJet"};			 

                                                                                                                                                                                                                                                                                
  if(sr==CRFull)   labels = { "SSPresel",  "Z_CR","Top_CR","Top_CR2", "TopAK8_CR","ZAK8_CR",
                          "WpWp_CR","WpWp_CR_NP","WpWp_CR_NP2","WpWp_CR_NP3",
			  "ZZ_CR","ZG_CR","WG_CR","WZ_SR1", "WZ_SR2", "WZ_SR3","WZB_CR",
                          "ZNPEl_CR", "ZNPMu_CR", "TopNP_CR",
                          "SR1_InvMET", "SR1_InvBJet","SR2_InvMET", "SR2_InvBJet","SR3_InvMET", "SR3_InvBJet",
                          "HighMassBJet_CR","HighMassNP_CR"} ;
  
  
  if(sr==sigmm)  labels = {"SSNoCut", "SSGen", "SSGen2", "SSMuMuTrig", "SSMuMuTrig2", "SSMuMuTrig2L", "SSMuMu", "SSMuMu_Pt", "SSMuMu_HEMVeto","SSMuMu_LepVeto", "SSMuMu_LLMass",  "SSMuMu_vTau","SSMuMu_Jet","SSMuMu_BJet", "SSMuMu_DiJet",  "SSMuMu_SR1", "SSMuMu_SR2","SSMuMu_SR3Inclusive","SSMuMu_SR3","SSMuMu_SR4","SSMuMu_SRFail"};
  
  if(sr==sigmm_17028) labels = {"SSNoCut", "SSGen","SSGen2", "SSMuMuTrig", "SSMuMuTrig2","SSMuMuTrig2L" , "SSMuMu","SSMuMu_Pt", "SSMuMu_HEMVeto","SSMuMu_LepVeto", "SSMuMu_LLMass", "SSMuMu_Jet", "SSMuMu_BJet", "SSMuMu_DiJet",  "SSMuMu_SR1","SSMuMu_SR3", "SSMuMu_SRFail"};

  if(sr==sigee) labels = {"SSNoCut", "SSGen", "SSGen2", "SSEETrig", "SSEETrig2", "SSEETrig2L","SSEE", "SSEE_Pt","SSEE_LepVeto", "SSEE_HEMVeto","SSEE_LLMass",  "SSEE_vTau","SSEE_Jet","SSEE_BJet", "SSEE_DiJet",  "SSEE_SR1","SSEE_SR2","SSEE_SR3Inclusive","SSEE_SR3","SSEE_SR4", "SSEE_SRFail"};
  
  if(sr==sigee_17028) labels = {"SSNoCut", "SSGen","SSGen2", "SSEETrig", "SSEETrig2", "SSEETrig2L", "SSEE","SSEE_Pt", "SSEE_HEMVeto", "SSEE_LepVeto", "SSEE_LLMass", "SSEE_Jet", "SSEE_BJet", "SSEE_DiJet",  "SSEE_SR1","SSEE_SR3","SSEE_SRFail"};
    
  if(sr==sigem)   labels = {"SSNoCut", "SSGen", "SSGen2", "SSEMuTrig", "SSEMuTrig2","SSEMuTrig2L","SSEMu", "SSEMu_Pt","SSEMu_LepVeto", "SSEMu_LLMass",  "SSEMu_vTau","SSEMu_Jet","SSEMu_BJet", "SSEMu_DiJet",  "SSEMu_SR1","SSEMu_SR1Fail", "SSEMu_SR2","SSEMu_SR3","SSEMu_SR4","SSEMu_SR5","SSEMu_SR3Fail"};
  
  if(sr==sigem_17028)  labels = {"SSNoCut", "SSGen","SSGen2", "SSEMuTrig", "SSEMuTrig2", "SSEMuTrig2L", "SSEMu","SSEMu_Pt", "SSEMu_LepVeto", "SSEMu_LLMass", "SSEMu_Jet", "SSEMu_BJet", "SSEMu_DiJet",  "SSEMu_SR1","SSEMu_SR1Fail","SSEMu_SR3","SSEMu_SR3Fail"};
  
  return labels;
  
}




