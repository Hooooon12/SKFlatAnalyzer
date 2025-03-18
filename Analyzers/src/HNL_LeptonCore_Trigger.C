#include "HNL_LeptonCore.h"


void HNL_LeptonCore::SetupTriggerLists(){


  std::vector<std::vector<TString>*> triggerLists = {
    &TrigList_HNL_HighPtMu,
    &TrigList_HNL_DblMu,
    &TrigList_HNL_Mu,
    &TrigList_Full_Mu,
    &TrigList_POG_Mu,
    
    &TrigList_HNL_DblEG,
    &TrigList_HNL_EG,
    &TrigList_POG_EG,
    &TrigList_HNL_HighPtEG,
    &TrigList_Full_EG,
    
    &TrigList_HNL_MuEG,
    &TrigList_POG_MuEG,
    &TrigList_Full_MuEG,
    
    &TrigList_HNL_EGMu,
    &TrigList_POG_EGMu,
    &TrigList_Full_EGMu
  };

  // Clear all the trigger lists
  for (auto& triggerList : triggerLists) {
    triggerList->clear();
  }

  if(GetEraShort()=="2016a"){
    // Lumi = 19.5 fb-1                                      
    //2016a run period 278808 
    // MU  
    TrigList_HNL_DblMu = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v",     "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v"};
    TrigList_POG_MuEG  = {"HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v"};
    TrigList_POG_EGMu  = {"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v"};
    TrigList_HNL_EGMu  = {"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v"};
    TrigList_HNL_MuEG  = {"HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v"};
    
    // 278806 and 278807 should be removed frmo thsi dataset 
    // REF https://twiki.cern.ch/twiki/bin/view/CMS/PdmVDatasetsUL2016
    // Run Range Bver2 [273150] - F [278770]                                                                      
    
  }
  if(GetEraShort()=="2016b"){

    // Lumi=16.8 /fb.
    // MU 
    TrigList_HNL_DblMu = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v","HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v", "HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v"};               
    TrigList_HNL_MuEG  = {"HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"};
    TrigList_POG_MuEG  = {"HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"};
    TrigList_POG_EGMu  = {"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v"};
    TrigList_HNL_EGMu  = {"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v"};
    TrigList_HNL_MuEG  = {"HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"};

    //https://twiki.cern.ch/twiki/bin/view/CMS/MuonHLT2016                                                                                                                     
    // Do we include HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v which is only in periodH                                                                                         
    // Do we include HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v", "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v in perfiod F + G                                                              
  }
  if(DataYear==2016){

    TrigList_POG_Mu = {    "HLT_IsoMu24_v",   "HLT_IsoTkMu24_v",};
    TrigList_HNL_HighPtMu = {"HLT_Mu50_v",   "HLT_TkMu50_v"};
    TrigList_HNL_Mu = {    "HLT_IsoMu24_v",   "HLT_IsoTkMu24_v",};
    // EG
    TrigList_HNL_DblEG = {  "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"};//,"HLT_Photon175_v"};
    TrigList_HNL_EG    = {  "HLT_Ele27_WPTight_Gsf_v" ,
                            "HLT_Ele25_eta2p1_WPTight_Gsf_v" ,
                            "HLT_Ele115_CaloIdVT_GsfTrkIdT_v",
                            "HLT_Photon175_v",
                            "HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50_v",
                            "HLT_Photon175_v",
                            "HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165_v",
                            "HLT_Photon175_v",
                            "HLT_DoubleEle33_CaloIdL_MW_v",
                            "HLT_DoubleEle37_Ele27_CaloIdL_GsfTrkIdVL_v"};
    //https://twiki.cern.ch/twiki/bin/view/CMS/EgHLTRunIISummary 
    TrigList_HNL_HighPtEG = {"HLT_Photon175_v","HLT_DoublePhoton60_v"};
    TrigList_POG_EG    = {  "HLT_Ele27_WPTight_Gsf_v" ,"HLT_Photon175_v"};

    //https://twiki.cern.ch/twiki/bin/view/CMS/EgHLTPathDetails                                                                                                             
    
  }  // END OF 2016 Triggers   

  else if(DataYear==2017){


    TrigList_HNL_DblMu = { "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v",
			   "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v" };
                           //"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v" };
    TrigList_HNL_Mu = {  "HLT_IsoMu27_v"};
    TrigList_POG_Mu = {  "HLT_IsoMu27_v"};
    TrigList_HNL_HighPtMu = {"HLT_Mu50_v"};

    TrigList_HNL_DblEG = { "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v"};//,"HLT_Photon200_v"};
    TrigList_HNL_EG = { "HLT_Ele32_WPTight_Gsf_v",
                        "HLT_Ele32_WPTight_Gsf_L1DoubleEG_v",
                        "HLT_Ele115_CaloIdVT_GsfTrkIdT_v",
                        "HLT_Photon200_v",
                        "HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165_v",
                        "HLT_Photon200_v",
                        "HLT_DoubleEle33_CaloIdL_MW_v"};
    TrigList_POG_EG = {"HLT_Ele32_WPTight_Gsf_v", "HLT_Ele32_WPTight_Gsf_L1DoubleEG_v","HLT_Photon200_v"};
    TrigList_HNL_HighPtEG = {"HLT_Photon200_v","HLT_DoublePhoton70_v"};

    TrigList_HNL_MuEG  = {"HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"};
    TrigList_HNL_EGMu  = {"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v"};
    
  } // end of 2017 triggers    

  else if(DataYear==2018){

    TrigList_HNL_DblMu = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v"};
    TrigList_HNL_Mu = {   "HLT_IsoMu24_v"};
    TrigList_HNL_HighPtMu = {"HLT_Mu50_v"};
    TrigList_POG_Mu = {   "HLT_IsoMu24_v"};

    // MAIN TRIGGER                                                                                                                                                                          
    TrigList_HNL_DblEG = {"HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v"};

    TrigList_HNL_EG = { "HLT_Ele32_WPTight_Gsf_v",
                        "HLT_DoubleEle25_CaloIdL_MW_v" ,
                        "HLT_Ele115_CaloIdVT_GsfTrkIdT_v",
                        "HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165_v"};

    TrigList_POG_EG = { "HLT_Ele32_WPTight_Gsf_v","HLT_Photon200_v"};

    TrigList_HNL_HighPtEG = { "HLT_Photon200_v", "HLT_DoublePhoton70_v"};

    TrigList_HNL_MuEG  = {"HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"};
    TrigList_HNL_EGMu  = {"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v"};

  }


  for(auto itrig : TrigList_HNL_DblMu)    TrigList_Full_Mu.push_back(itrig);
  for(auto itrig : TrigList_HNL_Mu)       TrigList_Full_Mu.push_back(itrig);
  for(auto itrig : TrigList_HNL_HighPtMu) TrigList_Full_Mu.push_back(itrig);
  for(auto itrig : TrigList_HNL_DblEG)    TrigList_Full_EG.push_back(itrig);
  for(auto itrig : TrigList_HNL_EG)       TrigList_Full_EG.push_back(itrig);
  for(auto itrig : TrigList_HNL_HighPtEG) TrigList_Full_EG.push_back(itrig);
  for(auto itrig : TrigList_HNL_MuEG)     TrigList_Full_MuEG.push_back(itrig);
  for(auto itrig : TrigList_Full_Mu)      TrigList_Full_MuEG.push_back(itrig);
  for(auto itrig : TrigList_Full_EG)      TrigList_Full_MuEG.push_back(itrig);
  for(auto itrig : TrigList_HNL_EGMu)     TrigList_Full_EGMu.push_back(itrig);

  for(auto itrig : TrigList_Full_Mu)   cout << "TrigList_Full_Mu  : "<< itrig << endl;
  for(auto itrig : TrigList_Full_EG)   cout << "TrigList_Full_EG  : "<< itrig << endl;
  
  return;
  
  
}

bool HNL_LeptonCore::PassMultiDatasetTriggerSelection(HNL_LeptonCore::Channel channel,Event ev, std::vector<Lepton *> leps, TString selectionMain, TString selectionOR){

  bool PassTrigger       = PassTriggerSelection(channel, ev,leps, selectionMain, true);
  bool PassTrigger2      = PassTriggerSelection(channel, ev,leps, selectionOR, true);

  // For non-data, we only need to return the OR of the first two triggers
  if (!IsData) {
    return (PassTrigger || PassTrigger2);
  }

  if(PassTrigger) return true;

  bool PassTriggerAllPD  = PassTriggerSelection(channel, ev,leps, selectionMain, false);
  if(PassTriggerAllPD) return false;
  return PassTrigger2;

}



bool HNL_LeptonCore::PassTriggerSelection(HNL_LeptonCore::Channel channel,Event ev, std::vector<Lepton *> leps, TString selection, bool check_pd ){

  bool PassTrigger(false);
  bool apply_ptcut(true);

  std::vector<Lepton *> leps_eg;
  std::vector<Lepton *> leps_muon;

  if (channel == MuMu){
    for(auto ilep : leps) {
      if(ilep->LeptonFlavour() == Lepton::MUON) leps_muon.push_back(ilep);
    }

    if(selection == "Dilep")            return PassTriggerAndCheckStream(apply_ptcut,leps_muon,ev,TrigList_HNL_DblMu,check_pd);
    else  if(selection == "Lep")        return PassTriggerAndCheckStream(apply_ptcut,leps_muon,ev,TrigList_HNL_Mu,check_pd);
    else  if(selection == "POGSglLep")  return PassTriggerAndCheckStream(apply_ptcut,leps_muon,ev,TrigList_POG_Mu,check_pd);
    else  if(selection == "HighPt")     return PassTriggerAndCheckStream(apply_ptcut,leps_muon,ev,TrigList_HNL_HighPtMu,check_pd);
    else  if(selection == "Full")       return PassTriggerAndCheckStream(apply_ptcut,leps_muon,ev,TrigList_Full_Mu,check_pd);
    else {
      cout << "[HNL_LeptonCore::PassTriggerSelection] selection " << selection <<  "not found.." << endl;
      exit(EXIT_FAILURE);
    }
  }

  if (channel == EE){
    for(auto ilep : leps) {
      if(ilep->LeptonFlavour() == Lepton::ELECTRON) leps_eg.push_back(ilep);
    }
    if(selection == "Dilep")          return  PassTriggerAndCheckStream(apply_ptcut,leps_eg,ev,TrigList_HNL_DblEG,check_pd);
    else if(selection == "POGSglLep") return  PassTriggerAndCheckStream(apply_ptcut,leps_eg,ev,TrigList_POG_EG,check_pd);
    else if(selection == "Lep")       return  PassTriggerAndCheckStream(apply_ptcut,leps_eg,ev,TrigList_HNL_EG,check_pd);
    else if(selection == "HighPt")    return  PassTriggerAndCheckStream(apply_ptcut,leps_eg,ev,TrigList_HNL_HighPtEG,check_pd);
    else if(selection == "Full")      return  PassTriggerAndCheckStream(apply_ptcut,leps_eg,ev,TrigList_Full_EG,check_pd);
    else {
      cout << "[HNL_LeptonCore::PassTriggerSelection] selection " << selection << "  not found.." << endl;
      exit(EXIT_FAILURE);
    }
  }
  

  if (channel == EMu){

    for(auto ilep : leps) {
      if(ilep->LeptonFlavour() == Lepton::MUON)     leps_muon.push_back(ilep);
      if(ilep->LeptonFlavour() == Lepton::ELECTRON) leps_eg.push_back(ilep);
    }

    if(selection == "Dilep")     return  PassTriggerAndCheckStream(apply_ptcut,leps,     ev,TrigList_HNL_EGMu,check_pd) || PassTriggerAndCheckStream(apply_ptcut,leps,     ev,TrigList_HNL_MuEG,check_pd);
    if(selection == "POGSglEl")  return  PassTriggerAndCheckStream(apply_ptcut,leps_eg,  ev,TrigList_POG_EG,check_pd);
    if(selection == "POGSglMu")  return  PassTriggerAndCheckStream(apply_ptcut,leps_muon,ev,TrigList_POG_Mu,check_pd);
    if(selection == "Full")      return  PassTriggerAndCheckStream(apply_ptcut,leps,     ev,TrigList_Full_EGMu,check_pd) || PassTriggerAndCheckStream(apply_ptcut,leps,     ev,TrigList_Full_MuEG,check_pd);

    cout << "[HNL_LeptonCore::PassTriggerSelection] selection " <<  selection << " not found.." << endl;
    exit(EXIT_FAILURE);
  
  }

  return PassTrigger;
}





double HNL_LeptonCore::GetPtCutTrigger(TString trigname, int nlep, TString flavour){

  if(flavour == "Muon"){
    if(trigname.Contains("HLT_Mu17") ||  trigname.Contains("HLT_TkMu17")) {
      if(trigname.Contains("Mu8") || trigname.Contains("TkMu8")) {
        if(nlep==0) return 20;
        if(nlep==1) return 10;
        if(nlep>1) return 0.;
      }
    }
    else if(trigname.Contains("HLT_Mu23") ||  trigname.Contains("HLT_Mu23")) {
      if(trigname.Contains("Mu8") || trigname.Contains("TkMu8")) {
        if(nlep==0) return 25;
        if(nlep==1) return 10;
        if(nlep>1) return 0.;
      }
    }

    else if(trigname.Contains("HLT_Mu50") && trigname.Contains("HLT_TkMu50")) {
      if(nlep==0) return 55;
      if(nlep>0) return 0.;
    }

    else if(trigname.Contains("HLT_Iso") && trigname.Contains("Mu24")) {
      if(nlep==0) return 26;
      if(nlep>0) return 0.;
    }

    else if(trigname.Contains("HLT_Iso") && trigname.Contains("Mu27")) {
      if(nlep==0) return 30;
      if(nlep>0) return 0.;
    }

  }


  if(flavour =="Electron"){

    if(trigname.Contains("HLT_Ele23_Ele12")){
      if(nlep==0) return 25;
      if(nlep==1) return 15.;
      if(nlep>1) return 0.;
    }

    else if(trigname.Contains("HLT_Ele27_WPTight")){
      if(nlep==0) return 30;
      if(nlep>0) return 0.;
    }

    else if(trigname.Contains("HLT_Ele25_eta2p1")){
      if(nlep==0) return 27;
      if(nlep>0) return 0.;
    }

    else if(trigname.Contains("HLT_Ele115")){
      if(nlep==0) return 120;
      if(nlep>0) return 0.;
    }

    else if(trigname.Contains("HLT_Photon175")){
      if(nlep==0) return 180;
      if(nlep>0) return 0.;
    }

    else if(trigname.Contains("HLT_Ele45_CaloIdVT")){
      if(nlep==0) return 50;
      if(nlep>0) return 0.;
    }

    else if(trigname.Contains("HLT_Ele50_CaloIdVT")){
      if(nlep==0) return 55;
      if(nlep>0) return 0.;
    }

    else if(trigname.Contains("HLT_DoubleEle33")){
      if(nlep==0) return 35;
      if(nlep==1) return 35;
      if(nlep>0) return 0.;
    }

    else if(trigname.Contains("HLT_DoubleEle37_Ele27")){
      if(nlep==0) return 40;
      if(nlep==1) return 30;
      if(nlep>0) return 0.;
    }
    else if(trigname.Contains("HLT_DoublePhoton60")){
      if(nlep==0) return 65;
      if(nlep>0) return 0.;
    }
  }
  if(flavour =="Muon" || flavour =="Electron"){

    if(trigname.Contains("HLT_Mu8_TrkIsoVVL_Ele23")){
      if(flavour =="Electron" && nlep==0) return 25;
      if(flavour =="Muon"&& nlep==0) return 10;
      if(nlep>0) return 0.;
    }

    else if(trigname.Contains("HLT_Mu23_TrkIsoVVL_Ele8")){
      if(flavour =="Electron" && nlep==0) return 10;
      if(flavour =="Muon"&& nlep==0) return 25;
      if(nlep>0) return 0.;

    }
    else if(trigname.Contains("HLT_Mu23_TrkIsoVVL_Ele12")){
      if(flavour =="Electron" && nlep==0) return 15;
      if(flavour =="Muon"&& nlep==0) return 25;
      if(nlep>0) return 0.;
    }

  }
  if(flavour =="Electron"){

    if(trigname.Contains("HLT_Ele27_")){
      if(nlep==0) return 30;
      if(nlep>0) return 0.;
    }
    else if(trigname.Contains("HLT_Ele32_")){
      if(nlep==0) return 35;
      if(nlep>0) return 0.;
    }
    else if(trigname.Contains("HLT_Ele35")){
      if(nlep==0) return 37;
      if(nlep>0) return 0.;
    }
    else if(trigname.Contains("HLT_Photon200")){
      if(nlep==0) return 210;
      if(nlep>0) return 0.;
    }
    else if(trigname.Contains("HLT_DoublePhoton70")){
      if(nlep==0) return 80;
      if(nlep==1) return 80;
      if(nlep>1) return 0.;
    }

    else if(trigname.Contains("HLT_DoubleEle25")){
      if(nlep==0) return 28;
      if(nlep==1) return 28;
      if(nlep>1) return 0.;
    }
  }
  return 99999.;
}




bool HNL_LeptonCore::PassPtTrigger(Event ev, vector<TString> triglist,std::vector<Lepton *> leps, bool check_pd){

  /// Code to check if event passes trigger/ Trigger is in PD running on for DAta and pt cuts are applied                                                                                                  
  TString itrig = triglist[0];  

  bool pass_trig_pt=true;

  if(check_pd && IsData&&!ev.IsPDForTrigger(itrig, this->DataStream)) return false;
    
  int nel(0),  nmu(0);
  
  for(unsigned int ilep = 0 ; ilep < leps.size(); ilep++){
    
    if(leps[ilep]->LeptonFlavour() == Lepton::ELECTRON) nel++;
    else nmu++;
    
    double lep_ptcut = (leps[ilep]->LeptonFlavour() == Lepton::ELECTRON) ? GetPtCutTrigger(itrig, nel-1, "Electron") : GetPtCutTrigger(itrig, nmu-1, "Muon") ;
    
    if(leps[ilep]->Pt() < lep_ptcut) pass_trig_pt=false;
    //cout << "leps[ilep]->Pt() = " << leps[ilep]->Pt() << " lep_ptcut = " << lep_ptcut << endl;
  }
  //cout << "GetPtCutTrigger = " << pass_trig_pt << endl;
  return pass_trig_pt;
}




bool HNL_LeptonCore::PassTriggerAndCheckStream(bool apply_ptcut,vector<Lepton*> leps, Event ev, vector<TString> triglist, bool check_data_pd){

  bool trig_passed=false;
  for(auto itrig : triglist){

    if(!IsData) {

      if(ev.PassTrigger(itrig))  {
        trig_passed=true;
        if(apply_ptcut  && !PassPtTrigger(ev,  {itrig}, leps)) trig_passed=false;
	
        if(trig_passed) return true;
      }
    }
    else if(check_data_pd){
      if(ev.PassTrigger(itrig) && ev.IsPDForTrigger(itrig, this->DataStream)) {
        trig_passed=true;
        if(apply_ptcut  && !PassPtTrigger(ev, {itrig}, leps,check_data_pd)) trig_passed=false;
        if(trig_passed) return true;
      }
    }
    else if(ev.PassTrigger(itrig)){
      trig_passed=true;
      if(apply_ptcut  && !PassPtTrigger(ev,  {itrig}, leps,check_data_pd)) trig_passed=false;
      if(trig_passed) return true;
    }
  }

  return trig_passed;
}


void HNL_LeptonCore::EvalTrigWeight(HNL_LeptonCore::Channel channel, vector<Muon> muons, vector<Electron> electrons, AnalyzerParameter& p, Event ev, double& w) {
  
  if(IsData) return;

  double trigW = HNLZvtxSF(channel);
  w*=trigW;
  FillWeightHist(p.ChannelDir()+"/HNLZvtxSF", trigW);
  
  if(!p.Apply_Weight_TriggerSF) {
    p.w.triggerSF =trigW;
    return;
  }
    
  TString SFKey_Trig = "Default";
  if(p.TriggerSelection == "Dilep"){
    if(channel == MuMu)    SFKey_Trig = p.k.Muon_Trigger_SF;
    else if(channel == EE) SFKey_Trig = p.k.Electron_Trigger_SF;
    else SFKey_Trig = p.k.EMu_Trigger_SF;
  }
  else{
    if(channel == EMu) {
      SFKey_Trig = p.k.Electron_Trigger_SF;
      SFKey_Trig = p.k.Muon_Trigger_SF;
    }
  }

  TString TriggerSFSyst="";
  if(p.syst_ == AnalyzerParameter::MuonTriggerSFUp)TriggerSFSyst="SystUp";
  if(p.syst_ == AnalyzerParameter::MuonTriggerSFDown)TriggerSFSyst="SystDown";
  if(p.syst_ == AnalyzerParameter::ElectronTriggerSFUp)TriggerSFSyst="SystUp";
  if(p.syst_ == AnalyzerParameter::ElectronTriggerSFDown)TriggerSFSyst="SystDown";

  double this_trigsf =  SFKey_Trig!=""? mcCorr->GetTriggerSF(electrons, muons, SFKey_Trig, TriggerSFSyst):1.;
  //  if(_jentry < 1000) cout << "this_trigsf = " << this_trigsf << " SFKey_Trig = " << SFKey_Trig << endl;
  FillWeightHist(p.ChannelDir()+"/Trig_SF", this_trigsf);

  w*=(this_trigsf);
  p.w.triggerSF =trigW*this_trigsf;
  
  
  ///// Update wright for Cases unprescaled triggers pass but prescale triggers fail
  //// Case 1 2016b

  if (DataEra == "2016postVFP"){
    if(!ev.PassTrigger("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v")){
      if(!ev.PassTrigger("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v")) {
	if(ev.PassTrigger("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v")) w = w  * ev.GetTriggerLumi("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v")/ev.GetTriggerLumi( "Full");
      }
    }
  }
  if (DataEra == "2017"){
    if(!ev.PassTrigger("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v")){
      if(ev.PassTrigger("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v"))   w = w  * ev.GetTriggerLumi("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v") / ev.GetTriggerLumi( "Full");
    }
  }
  return;
  

}
