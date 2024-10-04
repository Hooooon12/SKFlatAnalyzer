#include "HNL_LeptonCore.h"


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
    if(cutflow_histname.Contains("SR")||cutflow_histname.Contains("MuonCR")||cutflow_histname.Contains("ElectronCR")) cf_name="LimitBins";
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

void HNL_LeptonCore::FillLimitInput(HNL_LeptonCore::SearchRegion sr, double event_weight, TString label,  TString hist_path){

  vector<TString> lables = GetLabelsFromRegion(sr);
  TString histname = GetCutFlowNameFromRegion(sr);

  FillCutflowDef(hist_path,   histname, event_weight,lables, label);

  /// Fill SingleBinned                                                                                                                                                                                           
  vector<HNL_LeptonCore::SearchRegion> SingleBinned = {MuonCR1,MuonCR2,MuonCR3,MuonCRBDT,MuonCR3BDT,
                                                       ElectronCR1,ElectronCR2,ElectronCR3,ElectronCRBDT,ElectronCR3BDT,
                                                       ElectronMuonCR1,ElectronMuonCR2,ElectronMuonCR3,ElectronMuonCRBDT,ElectronMuonCR3BDT,
                                                       MuonSR1,MuonSR2,MuonSR3,MuonSRBDT,MuonSR3BDT,
                                                       ElectronSR1,ElectronSR2,ElectronSR3,ElectronSRBDT,ElectronSR3BDT,
                                                       ElectronMuonSR1,ElectronMuonSR2,ElectronMuonSR3,ElectronMuonSRBDT,ElectronMuonSR3BDT,
                                                       MuonCR,ElectronCR,ElectronMuonCR,
                                                       MuonSR,ElectronSR,ElectronMuonSR};

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
  if(sr==WZCR)   EVHistName= "WZCR";
  if(sr==WZBCR)   EVHistName= "WZBCR";
  if(sr==ZZCR)   EVHistName= "ZZCR";
  if(sr==ZZVBFCR)   EVHistName= "ZZVBFCR";
  if(sr==ZZCR2)  EVHistName= "ZZCR2";
  if(sr==WGCR)   EVHistName= "WGCR";
  if(sr==ZGCR)   EVHistName= "ZGCR";
  if(sr==WZBCR)   EVHistName= "WZBCR";
  if(sr==WZVBFCR)   EVHistName= "WZVBFCR";
  if(sr==WZVBFCR2)   EVHistName= "WZVBFCR2";
  if(sr==HMCR1)   EVHistName= "HMCR1";
  if(sr==HMCR2)   EVHistName= "HMCR2";
  if(sr==HMCR3)   EVHistName= "HMCR3";
  if(sr==HMNPCR)   EVHistName= "HMNPCR";
  if(sr==HMBCR)   EVHistName= "HMBCR";
  if(sr==HM1JCR)   EVHistName= "HM1JCR";
  if(sr==PreselVBF)   EVHistName= "PreselVBF";
  if(sr==Presel)   EVHistName= "Presel";
  if(sr==WWNP1CR)   EVHistName= "WWNP1CR";
  if(sr==WWNP2CR)   EVHistName= "WWNP2CR";
  if(sr==WWNP3CR)   EVHistName= "WWNP3CR";
  if(sr==WWCR1)   EVHistName= "WWCR1";
  if(sr==WWCR2)   EVHistName= "WWCR2";
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
  if(sr==MuonSR         || sr==MuonSROpt         )     EVHistName ="MuonSR";
  if(sr==ElectronSR     || sr==ElectronSROpt     )     EVHistName ="ElectronSR";
  if(sr==ElectronMuonSR || sr==ElectronMuonSROpt )     EVHistName ="ElectronMuonSR";
  if(sr==MuonSRBDT          || sr==MuonSRBDTOpt)         EVHistName ="MuonSRBDT";
  if(sr==ElectronSRBDT      || sr==ElectronSRBDTOpt)     EVHistName ="ElectronSRBDT";
  if(sr==ElectronMuonSRBDT  || sr==ElectronMuonSRBDTOpt) EVHistName ="ElectronMuonSRBDT";

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

  if(sr==MuonCR          ) EVHistName ="MuonCR";
  if(sr==ElectronCR      ) EVHistName ="ElectronCR";
  if(sr==ElectronMuonCR  ) EVHistName ="ElectronMuonCR";
  if(sr==MuonCRBDT          ) EVHistName ="MuonCRBDT";
  if(sr==ElectronCRBDT      ) EVHistName ="ElectronCRBDT";
  if(sr==ElectronMuonCRBDT  ) EVHistName ="ElectronMuonCRBDT";
  
  
  /// Channel Summary plots
  if(sr==ChannelDepInc )  EVHistName ="ChannelDependant_Inclusive";
  if(sr==ChannelDepDilep)EVHistName ="ChannelDependant_Dilep";
  if(sr==ChannelDepTrigger) EVHistName ="ChannelDependant_Trigger";
  if(sr==ChannelDepPresel)  EVHistName ="ChannelDependant_Presel";
  if(sr==ChannelDepSR1)   EVHistName ="ChannelDependant_SR1";
  if(sr==ChannelDepSR2)   EVHistName ="ChannelDependant_SR2";
  if(sr==ChannelDepSR3)   EVHistName ="ChannelDependant_SR3_BDT";
  if(sr==ChannelDepSR3HM)   EVHistName ="ChannelDependant_SR3_HighMass";

  if(sr==ChannelDepFAILSR3) EVHistName ="ChannelDependantSR3Fail";
  if(sr==SR) EVHistName ="SR_Summary";

  if(sr==ChannelDepCR1)   EVHistName ="ChannelDependant_CR1";
  if(sr==ChannelDepCR2)   EVHistName ="ChannelDependant_CR2";
  if(sr==ChannelDepCR3)   EVHistName ="ChannelDependant_CR3_BDT";
  if(sr==ChannelDepCR3HM)   EVHistName ="ChannelDependant_CR3_HighMass";
  
  //// Region plots
  if(sr == ControlRegion) EVHistName = "ValidationRegionFlow";
  if(sr == SignalRegion) EVHistName = "SignalRegionFlow";

    
  if(sr==CR)    EVHistName ="ControlRegions";
  if(sr==sigmm) EVHistName ="SR_CutFlow";
  if(sr==sigee) EVHistName ="SR_CutFlow";
  if(sr==sigem) EVHistName ="SR_CutFlow";
  if(sr==sigmm_17028) EVHistName ="SR_CutFlow_17028";
  if(sr==sigee_17028) EVHistName ="SR_CutFlow_17028";

  return EVHistName;
}


vector<TString>  HNL_LeptonCore::GetLabelsFromRegion(HNL_LeptonCore::SearchRegion sr){
  
  vector<TString> labels;
  
  int nstep(0);
  if(sr==WGCR)  nstep = 10;
  if(sr==ZGCR)  nstep = 11;
  if(sr==WZCR)  nstep = 11;
  if(sr==ZZCR)  nstep = 6;
  if(sr==ZZVBFCR)  nstep = 9;
  if(sr==ZZCR2)  nstep = 9;
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
  if(sr==PreselVBF)   nstep = 5;
  if(sr==Presel)   nstep = 5; 
  if(sr==WWNP1CR)   nstep = 11; 
  if(sr==WWNP2CR)   nstep = 10; 
  if(sr==WWNP3CR)   nstep = 10; 
  if(sr==WWCR1)   nstep = 8; 
  if(sr==WWCR2)   nstep = 10;
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
 
  if(sr==SR1 || sr==CR1 )   {
    labels = {  "SR1_Init","SR1_lep_charge","SR1_lep_pt","SR1_dilep_mass" ,"SR1_1AK8" ,"SR1_MET" ,"SR1_bveto","SR1_Wmass"};
    if(sr==CR1) labels = ConvertCutFlowLabels(labels);
  }
  
  if(sr==SR2  || sr==CR2) {
    
    labels = {  "SR2_lep_charge",  "SR2_lep_pt",  "SR2_DPhi",  "SR2_LLMass", "SR2_DiJet", "SR2_DiJetEta", "SR2_DiJetMass", "SR2_Zep", "SR2_VBF","SR2_met","SR2_bveto", "SR2_ht_lt1"};
    
    if(sr==CR2) labels = ConvertCutFlowLabels(labels);
  }
  
  if( sr==SR3 || sr==CR3) {
    labels = { "SR3_lep_charge" ,"SR3_lep_pt", "SR3_dilep_mass","SR3_MET","SR3_bveto","SR3_0JetBinHighPT","SR3_0JetBinLowPT","SR3_1JetBinHighPT", "SR3_1JetBinLowPT","SR3_dijet", "SR3_J1Pt"};
    if(sr==CR3) labels = ConvertCutFlowLabels(labels);
  }

  if(sr==MuonSRSummary) labels = {"Inclusive", "GenMatch", "CheckLeptonFlavourForChannel","METFilter", "CFCut", "Preselection", "AK8","SigReg1", "SigReg1Fail",  "SigReg2",  "SigReg3", "SigReg3Pass", "SigReg3Fail"};
  
  if( sr==SR3BDT || sr==CR3BDT)  {
    labels = {  "SR3_lep_charge" , "SR3_lep_pt",  "SR3_dilep_mass", "SR3_MET", "SR3_bveto"};//, "SR3_jet", "SR3_dijet","SR3_Wmass",  "SR3_J1Pt", "SR3_MET", "SR3_bveto"};
    if( sr==CR3BDT) labels= ConvertCutFlowLabels(labels);
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
  
  if(sr==ChannelDepFAILSR3) labels = {"MuMu_SR3Fail_0j","MuMu_SR3Fail_1j","MuMu_SR3Fail_2j", "EE_SR3Fail_0j","EE_SR3Fail_1j","EE_SR3Fail_2j","EMu_SR3Fail_0j","EMu_SR3Fail_1j","EMu_SR3Fail_2j"};
  
  if(sr==ChannelDepCR1)      labels = {"MuMu_CR1","EE_CR1","EMu_CR1"};
  if(sr==ChannelDepCR2)      labels = {"MuMu_CR2","EE_CR2","EMu_CR2"};
  if(sr==ChannelDepCR3)      labels = {"MuMu_CR3","EE_CR3","EMu_CR3"};
  if(sr==ChannelDepCR3HM)    labels = {"MuMu_CR3_HM","EE_CR3_HM","EMu_CR3_HM"};

  //// SR Labels
  if(sr==SR)   labels = {"SR1","SR2","SR3_LowMass","SR3_HighMass"};
  if(sr == SignalRegion)     labels = {"NoCut","GENMatched","HEMVeto","METFilter","ConvFilter","LeptonFlavour","Trigger","BJet"};
  /// CR lanels 
  if(sr == ControlRegion)    labels = {"NoCut","HEMVeto","METFilter","GENMatched","LeptonFlavour","Trigger","OS_VR","VV_VR","VG_VR","SS_CR","VBF_CR"};

  vector<TString> SR1labels = {"SR1_MNbin1","SR1_MNbin2","SR1_MNbin3","SR1_MNbin4","SR1_MNbin5", "SR1_MNbin6"};   
  vector<TString> SR2labels = {"SR2_HTLTbin1", "SR2_HTLTbin2", "SR2_HTLTbin3"};
  vector<TString> SR3labels = {"SR3_bin1","SR3_bin2","SR3_bin3","SR3_bin4","SR3_bin5","SR3_bin6","SR3_bin7","SR3_bin8", "SR3_bin9","SR3_bin10","SR3_bin11","SR3_bin12","SR3_bin13","SR3_bin14"};
  vector<TString> SR3BDTlabels = {"SR3_BDTbin1","SR3_BDTbin2","SR3_BDTbin3","SR3_BDTbin4","SR3_BDTbin5","SR3_BDTbin6","SR3_BDTbin7","SR3_BDTbin8","SR3_BDTbin9","SR3_BDTbin10","SR3_BDTbin11","SR3_BDTbin12","SR3_BDTbin13","SR3_BDTbin14","SR3_BDTbin15","SR3_BDTbin16","SR3_BDTbin17"};

  vector<TString> CR1labels = ConvertCutFlowLabels(SR1labels);
  vector<TString> CR2labels = ConvertCutFlowLabels(SR2labels);
  vector<TString> CR3labels = ConvertCutFlowLabels(SR3labels);
  vector<TString> CR3BDTlabels= ConvertCutFlowLabels(SR3BDTlabels);
  
  vector<TString> SRlabels ;
  for(auto i : SR1labels) SRlabels.push_back(i);
  for(auto i : SR2labels) SRlabels.push_back(i);
  for(auto i : SR3labels) SRlabels.push_back(i);
 

  vector<TString> CRlabels =ConvertCutFlowLabels(SRlabels);

  vector<TString> SRBDTlabels;
  for(auto i : SR1labels) SRBDTlabels.push_back(i);
  for(auto i : SR2labels) SRBDTlabels.push_back(i);
  for(auto i : SR3BDTlabels) SRBDTlabels.push_back(i);

  vector<TString> CRBDTlabels = ConvertCutFlowLabels(SRBDTlabels);
  
  
  vector<TString> SRlabelsOpt = {"SR1_MNbin1","SR1_MNbin2","SR1_MNbin3","SR1_MNbin4",
				 "SR2_HTLTbin1", "SR2_HTLTbin2",
				 "SR3_bin1","SR3_bin2","SR3_bin3","SR3_bin4","SR3_bin5","SR3_bin6","SR3_bin7","SR3_bin8", "SR3_bin9","SR3_bin10","SR3_bin11"};
  vector<TString> SRBDTlabelsOpt=  {"SR1_MNbin1","SR1_MNbin2","SR1_MNbin3","SR1_MNbin4",
				    "SR2_HTLTbin1", "SR2_HTLTbin2",
				    "SR3_BDTbin1","SR3_BDTbin2","SR3_BDTbin3","SR3_BDTbin4","SR3_BDTbin5","SR3_BDTbin6","SR3_BDTbin7"};
  
  ///// STANDARD ANALYSIS LIMIT LABELS                                                                                                                                                                                                                                                                                                                                                                                                                                                     
  if(sr==MuonSR1    || sr==ElectronSR1   || sr==ElectronMuonSR1)      labels = SR1labels;
  if(sr==MuonSR2    || sr==ElectronSR2   || sr==ElectronMuonSR2)      labels = SR2labels;
  if(sr==MuonSR3    || sr==ElectronSR3   || sr==ElectronMuonSR3)      labels = SR3labels;

  if(sr==MuonCR1    || sr==ElectronCR1   || sr==ElectronMuonCR1)      labels = CR1labels;
  if(sr==MuonCR2    || sr==ElectronCR2   || sr==ElectronMuonCR2)      labels = CR2labels;
  if(sr==MuonCR3    || sr==ElectronCR3   || sr==ElectronMuonCR3)      labels = CR3labels;


  if(sr==MuonSR    || sr==ElectronSR    || sr==ElectronMuonSR)       labels = SRlabels;
  if(sr==MuonCR    || sr==ElectronCR    || sr==ElectronMuonCR)       labels = CRlabels;
  ///// STANDARD ANALYSIS LIMIT LABELS WITH CHARGE SPLIT                                                                                                                                                                                                                              
  if(sr==MuonSRBDT || sr==ElectronSRBDT || sr==ElectronMuonSRBDT)    labels = SRBDTlabels ;
  if(sr==MuonCRBDT || sr==ElectronCRBDT || sr==ElectronMuonCRBDT)    labels = CRBDTlabels ;
  
  if(sr==MuonSR3BDT || sr==ElectronSR3BDT || sr==ElectronMuonSR3BDT)    labels = SR3BDTlabels ;
  if(sr==MuonCR3BDT || sr==ElectronCR3BDT || sr==ElectronMuonCR3BDT)    labels = CR3BDTlabels ;

  //// OPTIMISING LIMIT PLOTS                                                                                                                                                                                                                                                                                                                                                                                                                                                              
  if(sr==MuonSRBDTOpt || sr==ElectronSRBDTOpt || sr==ElectronMuonSRBDTOpt)    labels = SRBDTlabelsOpt ;
  if(sr==MuonSROpt    || sr==ElectronSROpt    || sr==ElectronMuonSROpt)       labels = SRlabelsOpt;

  //                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
  if(sr==CR)   labels = { "Z_CR","Top_CR","Top_CR2", "TopAK8_CR","ZAK8_CR",
			  "WpWp_CR1","WpWp_CR2","WpWp_CRNP","WpWp_CRNP2","WpWp_CRNP3",
			  "WZ_CR", "ZZ_CR","ZZVBF_CCR","ZZOffShell_CR","ZG_CR","WG_CR","WZVBF_CR","WZVBF_CR2","WZB_CR", 
			  "ZNPEl_CR", "ZNPMu_CR", "TopNP_CR",
			  "HighMassSR1_CR","HighMassSR2_CR","HighMassSR3_CR" ,"HighMass1Jet_CR",
			  "HighMassBJet_CR","HighMassNP_CR", "SSPresel"};
  
  
  if(sr==sigmm)  labels = {"SSNoCut", "SSGen", "SSGen2", "SSMuMuTrig", "SSMuMuTrig2", "SSMuMuTrig2L", "SSMuMu", "SSMuMu_Pt", "SSMuMu_HEMVeto","SSMuMu_LepVeto", "SSMuMu_LLMass",  "SSMuMu_vTau","SSMuMu_Jet","SSMuMu_BJet", "SSMuMu_DiJet",  "SSMuMu_SR1", "SSMuMu_SR2","SSMuMu_SR3Inclusive","SSMuMu_SR3","SSMuMu_SR4","SSMuMu_SRFail"};
  
  if(sr==sigmm_17028) labels = {"SSNoCut", "SSGen","SSGen2", "SSMuMuTrig", "SSMuMuTrig2","SSMuMuTrig2L" , "SSMuMu","SSMuMu_Pt", "SSMuMu_HEMVeto","SSMuMu_LepVeto", "SSMuMu_LLMass", "SSMuMu_Jet", "SSMuMu_BJet", "SSMuMu_DiJet",  "SSMuMu_SR1","SSMuMu_SR3", "SSMuMu_SRFail"};

  if(sr==sigee) labels = {"SSNoCut", "SSGen", "SSGen2", "SSEETrig", "SSEETrig2", "SSEETrig2L","SSEE", "SSEE_Pt","SSEE_LepVeto", "SSEE_HEMVeto","SSEE_LLMass",  "SSEE_vTau","SSEE_Jet","SSEE_BJet", "SSEE_DiJet",  "SSEE_SR1","SSEE_SR2","SSEE_SR3Inclusive","SSEE_SR3","SSEE_SR4", "SSEE_SRFail"};
  
  if(sr==sigee_17028) labels = {"SSNoCut", "SSGen","SSGen2", "SSEETrig", "SSEETrig2", "SSEETrig2L", "SSEE","SSEE_Pt", "SSEE_HEMVeto", "SSEE_LepVeto", "SSEE_LLMass", "SSEE_Jet", "SSEE_BJet", "SSEE_DiJet",  "SSEE_SR1","SSEE_SR3","SSEE_SRFail"};
    
  if(sr==sigem)   labels = {"SSNoCut", "SSGen", "SSGen2", "SSEMuTrig", "SSEMuTrig2","SSEMuTrig2L","SSEMu", "SSEMu_Pt","SSEMu_LepVeto", "SSEMu_LLMass",  "SSEMu_vTau","SSEMu_Jet","SSEMu_BJet", "SSEMu_DiJet",  "SSEMu_SR1","SSEMu_SR1Fail", "SSEMu_SR2","SSEMu_SR3","SSEMu_SR4","SSEMu_SR5","SSEMu_SR3Fail"};
  
  if(sr==sigem_17028)  labels = {"SSNoCut", "SSGen","SSGen2", "SSEMuTrig", "SSEMuTrig2", "SSEMuTrig2L", "SSEMu","SSEMu_Pt", "SSEMu_LepVeto", "SSEMu_LLMass", "SSEMu_Jet", "SSEMu_BJet", "SSEMu_DiJet",  "SSEMu_SR1","SSEMu_SR1Fail","SSEMu_SR3","SSEMu_SR3Fail"};
  
  return labels;
  
}




