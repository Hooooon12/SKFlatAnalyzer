#include "HNL_Lepton_ID_SF.h"

void HNL_Lepton_ID_SF::initializeAnalyzer(){

  // All default settings like trigger/ PD/ BJet are decalred in HNL_LeptonCore::initializeAnalyzer to make them consistent for all HNL codes

  HNL_LeptonCore::initializeAnalyzer();

  cout << "HNL_Lepton_ID_SF Code is used to measure and study lepton ID efficinecy and SF (temp SF)" << endl;

}


void HNL_Lepton_ID_SF::executeEvent(){
  
  if((_jentry==0)){ 
    // Print out trigger info in HNL_LeptonCore::initializeAnalyzer
    TriggerPrintOut(GetEvent());
  }
 
  AnalyzerParameter param =  HNL_LeptonCore::InitialiseHNLParameter("MVAUL");

  //  MeasureElectronEfficiencies(param);
  // MeasureMuonEfficiencies(param);

  //MeasureElectronIDSF(param);
  //  cout << "------------------------------------------------------------------------------------------------------------------------------" << endl;
  MeasureMuonIDSF(param);
  return ;
}

void HNL_Lepton_ID_SF::MeasureMuonEfficiencies(AnalyzerParameter param){

  if(IsData) return;
  Event ev = GetEvent();
  double weight = SetupWeight(ev,param)/MCweight(true, true);
  weight *= MCweight(true, false);


  HNL_LeptonCore::Channel dilep_channel = MuMu;
  std::vector<Muon>       MuonCollProbe     = GetMuons    ("passProbe",    10., 2.4);
  vector<Muon> MuonCollProbePrompt;
  for(auto im : MuonCollProbe) {
    if(im.IsPrompt()) MuonCollProbePrompt.push_back(im);
  }

  std::vector<Lepton *> LeptonColl      = MakeLeptonPointerVector(MuonCollProbePrompt);

  if (!PassTriggerSelection(dilep_channel, ev, LeptonColl,"POG")) return;


  FillMuonCollPlots(param,"MuonEff", MuonCollProbePrompt, weight);

  vector<TString> IDs = {"HNL_Peking", "HNTightV2","HNL_HN3L","MVAID","MVALoose","HNL_ULID_2016","HNL_ULID_2017","HNTightPFIsoTight",};
  for(auto ID : IDs) PlotIDEfficiency(param,dilep_channel, MuonCollProbePrompt ,  ID,  weight);
  

}

void HNL_Lepton_ID_SF::MeasureElectronEfficiencies(AnalyzerParameter param){
  if(IsData) return;

  Event ev = GetEvent();

  double weight = SetupWeight(ev,param)/(ev.GetTriggerLumi("Full") *MCweight(true, true));
  weight *= MCweight(true, false);
  FillHist("MeasureElectronEfficiencies/Weight",  weight,   1, 100, -10, 10);

  HNL_LeptonCore::Channel dilep_channel= EE;

  std::vector<Electron> ProbeEl = GetElectrons( "passProbeID", 10., 2.5);


  vector<Electron> ElectronCollProbePrompt;
  vector<Electron> ElectronCollProbeConv;
  vector<Electron> ElectronCollProbeCF;
  vector<Electron> ElectronCollProbeFake;
  for(auto iel : ProbeEl) {
    if(iel.IsPrompt()) ElectronCollProbePrompt.push_back(iel);
    if(iel.LeptonIsCF())     ElectronCollProbeCF.push_back(iel);
    if(iel.IsConv())   ElectronCollProbeConv.push_back(iel);
    if(iel.IsFake())   ElectronCollProbeFake.push_back(iel);
  }


  std::vector<Lepton *> LeptonColl      = MakeLeptonPointerVector(ProbeEl);
  
  FillElectronPlots(param,"ElEff" ,  ElectronCollProbePrompt, weight);

  vector<TString> IDs = {"HNL_ULID_Baseline",
			 "HNTightV2", 
			 "HNL_Peking_2016","HNL_Peking_2017",
			 "HNL_ULID_OPT_POG1",
			 "HNL_ULID_OPT_POG2",
			 "HNL_ULID_OPT_POG3",
			 "HNL_ULID_OPT_POG4",
			 "HNL_ULID_OPT_POG5",
			 "HNL_ULID_Run2_CF",
			 "HNL_ULID_Run2_Conv",
			 "HNL_ULID_Run2_Fake",
			 "HNL_ULID_Run2_CFPt",
			 "HN2016MVA",
			 "HN2016MVA2",
			 "HN2016MVA2CC",
			 "HN2016POG",
			 "passHEEPID","passMediumID","passTightID","passMVAID_Iso_WP80","passMVAID_Iso_WP90","passMVAID_noIso_WP80","passMVAID_noIso_WP90",
			 "MVAID","passProbeID","passProbeIDTight","MVALoose",};

  if (PassTriggerSelection(dilep_channel, ev, LeptonColl,"Dilep")) {
    for(auto ID : IDs)PlotIDEfficiency(param,"EE_Prompt", ElectronCollProbePrompt ,  ID,  weight);
  }

  for(auto ID : IDs)PlotIDEfficiency(param,"EE_Fake", ElectronCollProbeFake ,   ID,  weight);
  for(auto ID : IDs)PlotIDEfficiency(param,"EE_CF", ElectronCollProbeCF  ,    ID,  weight);
  for(auto ID : IDs)PlotIDEfficiency(param,"EE_Conv", ElectronCollProbeConv ,   ID,  weight);

  return;

}


void HNL_Lepton_ID_SF::PlotIDEfficiency(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, TString ID, double weight_ll){

  TString channel_string = GetChannelString(dilep_channel) ;
  
  for(auto iel : ElectronColl) {
    FilllHistBins(iel, iel.PassID(ID),  channel_string,ID, weight_ll);
  }

}

void HNL_Lepton_ID_SF::PlotIDEfficiency(AnalyzerParameter param, TString channel_string, vector<Electron> ElectronColl, TString IDs, double weight_ll){

  for(auto iel : ElectronColl) {
    FilllHistBins(iel, iel.PassID(IDs),  channel_string,IDs, weight_ll);
  }

}



void HNL_Lepton_ID_SF::PlotIDEfficiency(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Muon> MuonColl, TString ID, double weight_ll){

  TString channel_string = GetChannelString(dilep_channel) ;

  for(auto imu : MuonColl) {
    FilllHistBins(imu, imu.PassID(ID),  channel_string,ID, weight_ll);
  }

}



void HNL_Lepton_ID_SF::MeasureMuonIDSF(AnalyzerParameter param){

  ///// Functiom to measure PRELIMINARY SFs for muon ID

  Event ev      = GetEvent();
  double weight = SetupWeight(ev,param);

  HNL_LeptonCore::Channel dilep_channel = MuMu;
  std::vector<Muon>       MuonCollProbe = GetMuons    ("passProbe",    10., 2.4);

  std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, 10., 2.5);
  if(ElectronCollV.size() != 0) return;                                                                                                                                                                                               

  //// BASIC event cuts
  
  std::vector<Jet> BJetColl  = GetHNLJets("BJetL", param);
  Particle METv              = GetMiniAODvMET("PuppiT1xyCorr"); // reyturns MET with systematic correction                                                           
  if(BJetColl.size() > 0 ) return;
  if(!PassMETFilter()) return;

  std::vector<Lepton *> LeptonColl      = MakeLeptonPointerVector(MuonCollProbe);

  // Chose Typ1 Phi corr MET + smear jets 
  if(!CheckLeptonFlavourForChannel(dilep_channel, LeptonColl)) return;
  if (!PassTriggerSelection(dilep_channel, ev, LeptonColl,"POG")) return;

  // Require 2 Probe muons in Z mass
  if(MuonCollProbe.size() != 2) return;
  //  if(70 <  fabs(GetLLMass(MuonCollProbe)- M_Z) > 20) return;
  if(70 > GetLLMass(MuonCollProbe))   return;
  if(130 < GetLLMass(MuonCollProbe))   return;

  if(MuonCollProbe[0].DeltaR(MuonCollProbe[1]) < 0.3) return;
  if(!IsData){
    if(!MuonCollProbe[0].IsPrompt()) return;
    if(!MuonCollProbe[1].IsPrompt()) return;
  }


  vector<TString> IDs;
  IDs = {  "HNL_ULID_"+GetYearString(), "HNL_ULID_NoHLT_"+GetYearString(),"TopHNT","HNTightV2"};

  /// Make cuts to remove fakes
  
  for(auto id : IDs)    MeasureIDSF(param,dilep_channel, MuonCollProbe, id, weight);
    
  return;
}



void HNL_Lepton_ID_SF::MeasureElectronIDSF(AnalyzerParameter param){

  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);

  HNL_LeptonCore::Channel dilep_channel= EE;

  /// Event cuts
  std::vector<Jet> BJetColl  = GetHNLJets("BJetL", param);
  Particle METv = GetMiniAODvMET("PuppiT1xyCorr"); // reyturns MET with systematic correction                                                                                                                            

  if(BJetColl.size() > 0 ) return;
  if(!PassMETFilter()) return;

  /// Obj cuts
  std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID, 5., 2.4);
  if(MuonCollV.size() != 0) return;

  //  return; /// JOHN
  std::vector<Electron> ProbeEl = GetElectrons( "passProbeID", 10., 2.5);

  bool HasHEM(false);
  for(auto i: ProbeEl) {
    if(FindHEMElectron(i)) HasHEM=true;
  }
  if(HasHEM) return;


  if(ProbeEl.size() != 2) return;
  if(!IsData){
    if(!ProbeEl[0].IsPrompt()) return;
    if(!ProbeEl[1].IsPrompt()) return;
  }

  if(fabs(GetLLMass(ProbeEl)- M_Z) > 20) return;

  std::vector<Lepton *> LeptonColl      = MakeLeptonPointerVector(ProbeEl);
  if(!PassTriggerSelection(dilep_channel, ev, LeptonColl,"POG")) return;
  if(!CheckLeptonFlavourForChannel(dilep_channel, LeptonColl)) return; // Check if EE cahnnel has 2 el...                                                                                                         
  
  vector<TString> IDs = {};
  IDs.push_back("passProbeIDTight");
  IDs.push_back("MVALoose");
  IDs.push_back("passMVAID_noIso_WP90Opt");
  IDs.push_back("passMVAID_noIso_WP90");

  for(auto id : IDs)    MeasureIDSF(param ,dilep_channel, ProbeEl, id, weight);
  
  vector<TString> MVAIDs = {"MVALoose",
			    "HNL_ULID_ScanCFVT_BDTG",
			    "HNL_ULID_ScanCFT_BDTG",
			    "HNL_ULID_ScanCFM_BDTG",
			    "HNL_ULID_ScanCFL_BDTG",
			    "HNL_ULID_ScanCFVL_BDTG",
			    
			    "HNL_ULID_ScanCFVTPt_BDTG",
			    "HNL_ULID_ScanCFTPt_BDTG",
			    "HNL_ULID_ScanCFMPt_BDTG",
			    "HNL_ULID_ScanCFLPt_BDTG",
			    "HNL_ULID_ScanCFVLPt_BDTG",

			    "HNL_ULID_ScanFAKEVVT_BDTG",
			    "HNL_ULID_ScanFAKEVT_BDTG",
			    "HNL_ULID_ScanFAKET_BDTG",
			    "HNL_ULID_ScanFAKEM_BDTG",
			    "HNL_ULID_ScanFAKEL_BDTG",
			    "HNL_ULID_ScanFAKEVL_BDTG",
			    "HNL_ULID_ScanFAKEVVL_BDTG",

			    "HNL_ULID_ScanCONVVT_BDTG",
			    "HNL_ULID_ScanCONVT_BDTG",
			    "HNL_ULID_ScanCONVM_BDTG",
			    "HNL_ULID_ScanCONVL_BDTG",

			    "HNL_ULID_Run2T",
			    "HNL_ULID_Run2L",
			    "HNL_ULID_Run2_CF",
			    "HNL_ULID_Run2_CFPt",
			    "HNL_ULID_Run2_Conv",
			    "HNL_ULID_Run2_Fake",
			    "HNL_ULID_2016",
			    "HNL_ULID_2017",
			    "HNL_ULID_2018",
			    "HNTightV2",
			    "TopHNSST",
			    "HNL_Peking_2016","HNL_Peking_2017",
			    "passHEEPID","passMediumID","passTightID","passMVAID_Iso_WP80","passMVAID_Iso_WP90","passMVAID_noIso_WP80","passMVAID_noIso_WP90"};


  for(auto mvaid : MVAIDs){

    bool passID1 = ProbeEl[0].PassID(mvaid);                                                                                  
    bool passID2 = ProbeEl[1].PassID(mvaid);                                                                                  

    vector<bool> PassedIDs = {passID1,passID2};
    MeasureIDSF(param ,dilep_channel, ProbeEl, PassedIDs, mvaid,weight);                                                                                                                                                                                                
  }
  return;
  
}
     

void HNL_Lepton_ID_SF::MeasureIDSF(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Muon> MuonColl, TString ID, double weight_ll){

  TString channel_string = GetChannelString(dilep_channel) ;

  bool TightWindowCut = ((GetLLMass(MuonColl)- M_Z) < 10) ;

  std::vector<Lepton *> LeptonColl      = MakeLeptonPointerVector(MuonColl);

  ///// Set SS events to -1 * weight to remove Fake leptons from T&P
  if(IsData){
    if(SameCharge(MuonColl)) weight_ll = -1;
    else weight_ll=1;
  }  
  else if(SameCharge(MuonColl)) return;
  
  TString TrigMatchedPath = (GetYearString() == "2017") ? "HLT_IsoMu27_v" : "HLT_IsoMu24_v";

  //  if(MuonColl[0].PassID("POGTightWithTightIso") && MuonColl[0].Pt() > 30 && MuonColl[0].PassPath(TrigMatchedPath) )cout << "Muon 1  " <<  "  trigger1 = " << MuonColl[0].PassPath(TrigMatchedPath) << " POGTightWithTightIso=" << MuonColl[0].PassID("POGTightWithTightIso")  << " IDPass2 = " << MuonColl[1].PassID(ID) << endl;
  //  if(MuonColl[1].PassID("POGTightWithTightIso") && MuonColl[1].Pt() > 31 && MuonColl[0].PassPath(TrigMatchedPath)) cout << "Muon 2  " <<  "  trigger2 = " << MuonColl[1].PassPath(TrigMatchedPath) << " POGTightWithTightIso=" << MuonColl[1].PassID("POGTightWithTightIso")  << " IDPass1 = " << MuonColl[0].PassID(ID) << endl;

  if(MuonColl[0].PassID("POGTightWithTightIso") && MuonColl[0].Pt() > 30 && MuonColl[0].PassPath(TrigMatchedPath) ) {
    
    double SF=1;
    if(!IsData){
      double lep1_pt  = MuonColl[0].MiniAODPt();
      double lep1_eta = MuonColl[0].Eta();
      double lep2_pt  = MuonColl[1].MiniAODPt();
      double lep2_eta = MuonColl[1].Eta();
      
      double lep1_idsf   = mcCorr->MuonID_SF ("NUM_TightID_DEN_TrackerMuons",  lep1_eta, lep1_pt,0);
      double lep1_isosf  = mcCorr->MuonISO_SF("NUM_TightRelIso_DEN_TightIDandIPCut", lep1_eta, lep1_pt,0);
      double lep1_recosf = mcCorr->MuonReco_SF("MuonRecoSF", lep1_eta, lep1_pt,0);
      double lep2_recosf = mcCorr->MuonReco_SF("MuonRecoSF", lep2_eta, lep2_pt,0);

      SF = lep1_idsf*lep1_isosf*lep1_recosf*lep2_recosf;
    }
    if( !(MuonColl[1].Pt() < 20 && MT(MuonColl[1],GetMiniAODvMET("PuppiT1xyCorr")) > 30) ) {
    
      if(SameCharge(MuonColl)) FilllHistBins(Lepton(MuonColl[1]), MuonColl[1].PassID(ID),  channel_string,ID+"_SS", SF*weight_ll);
      else  FilllHistBins(Lepton(MuonColl[1]), MuonColl[1].PassID(ID),  channel_string,ID, SF*weight_ll);
    }
    
    if(SameCharge(MuonColl)) {
      if(MuonColl[1].Pt() > 100) FilllHistBins(Lepton(MuonColl[1]), MuonColl[1].PassID(ID),  channel_string,ID+"_SSTZ", SF*weight_ll);
      else if(TightWindowCut)    FilllHistBins(Lepton(MuonColl[1]), MuonColl[1].PassID(ID),  channel_string,ID+"_SSTZ", SF*weight_ll);
    }
    else{
      if(MuonColl[1].Pt() > 100) FilllHistBins(Lepton(MuonColl[1]), MuonColl[1].PassID(ID),  channel_string,ID+"_TZ", SF*weight_ll);
      else if(TightWindowCut)    FilllHistBins(Lepton(MuonColl[1]), MuonColl[1].PassID(ID),  channel_string,ID+"_TZ", SF*weight_ll);
    }

  }
  if(MuonColl[1].PassID("POGTightWithTightIso")&& MuonColl[1].Pt() > 30 && MuonColl[1].PassPath(TrigMatchedPath)) {

    double SF=1;
    if(!IsData){
      double lep1_pt  = MuonColl[1].MiniAODPt();
      double lep1_eta = MuonColl[1].Eta();
      double lep2_pt  = MuonColl[0].MiniAODPt();
      double lep2_eta = MuonColl[0].Eta();
      
      double lep1_idsf   = mcCorr->MuonID_SF ("NUM_TightID_DEN_TrackerMuons",  lep1_eta, lep1_pt,0);
      double lep1_isosf  = mcCorr->MuonISO_SF("NUM_TightRelIso_DEN_TightIDandIPCut", lep1_eta, lep1_pt,0);
      double lep1_recosf = mcCorr->MuonReco_SF("MuonRecoSF", lep1_eta, lep1_pt,0);
      double lep2_recosf = mcCorr->MuonReco_SF("MuonRecoSF", lep2_eta, lep2_pt,0);

      SF = lep1_idsf*lep1_isosf*lep1_recosf*lep2_recosf;
    }

    if( !(MuonColl[0].Pt() < 20 && MT(MuonColl[0],GetMiniAODvMET("PuppiT1xyCorr")) > 30) ) {
      
      if(SameCharge(MuonColl)) FilllHistBins(Lepton(MuonColl[0]), MuonColl[0].PassID(ID),  channel_string,ID+"_SS", SF*weight_ll);
      else  FilllHistBins(Lepton(MuonColl[0]), MuonColl[0].PassID(ID),  channel_string,ID, SF*weight_ll);
    }

    if(SameCharge(MuonColl)) {
      if(MuonColl[0].Pt() > 100) FilllHistBins(Lepton(MuonColl[0]), MuonColl[0].PassID(ID),  channel_string,ID+"_SSnZ", SF*weight_ll);
      else if(TightWindowCut)    FilllHistBins(Lepton(MuonColl[0]), MuonColl[0].PassID(ID),  channel_string,ID+"_SSTZ", SF*weight_ll);
    }
    else{
      if(MuonColl[0].Pt() > 100) FilllHistBins(Lepton(MuonColl[0]), MuonColl[0].PassID(ID),  channel_string,ID+"_TZ", SF*weight_ll);
      else if(TightWindowCut)    FilllHistBins(Lepton(MuonColl[0]), MuonColl[0].PassID(ID),  channel_string,ID+"_TZ", SF*weight_ll);
    }
  }
  
  return;
  
}



void HNL_Lepton_ID_SF::MeasureIDSF(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, TString ID, double weight_ll){
  
  TString channel_string = GetChannelString(dilep_channel) ;

  bool TightWindowCut = ((GetLLMass(ElectronColl)- M_Z) < 10) ;

  std::vector<Lepton *> LeptonColl      = MakeLeptonPointerVector(ElectronColl);

  if(!IsData){
    if(!SameCharge(ElectronColl)) {
      if( ElectronColl[0].LeptonIsCF() || ElectronColl[1].LeptonIsCF()) return;
    }
  }
  else{
    if(SameCharge(ElectronColl)) weight_ll = -1;
    else weight_ll=1;
  }

  if(ElectronColl[0].PassID("passPOGTight")&& ElectronColl[0].Pt() > 35) {

    double SF=1;
    if(!IsData){
      double this_recosf  = mcCorr->ElectronReco_SF(ElectronColl[0].scEta(),ElectronColl[0].Pt(), 0);
      double this_idsf    = mcCorr->ElectronID_SF("passTightID", ElectronColl[0].scEta(), ElectronColl[0].Pt(), 0);
      
      double lep2_recosf  = mcCorr->ElectronReco_SF(ElectronColl[1].scEta(),ElectronColl[1].Pt(), 0);
      SF = this_recosf*this_idsf*lep2_recosf;
    }


    if(ElectronColl[1].Pt() > 100 ) FilllHistBins(Lepton(ElectronColl[1]), ElectronColl[1].PassID(ID),  channel_string,ID, SF*weight_ll);
    else if(TightWindowCut) FilllHistBins(Lepton(ElectronColl[1]), ElectronColl[1].PassID(ID),  channel_string,ID, SF*weight_ll);


  }
  if(ElectronColl[1].PassID("passPOGTight")&& ElectronColl[1].Pt() > 35) {

    double SF=1;
    if(!IsData){
      double this_recosf  = mcCorr->ElectronReco_SF(ElectronColl[1].scEta(),ElectronColl[1].Pt(), 0);
      double this_idsf    = mcCorr->ElectronID_SF("passTightID", ElectronColl[1].scEta(), ElectronColl[1].Pt(), 0);
      
      double lep2_recosf  = mcCorr->ElectronReco_SF(ElectronColl[0].scEta(),ElectronColl[0].Pt(), 0);
      SF = this_recosf*this_idsf*lep2_recosf;
    }

    if(ElectronColl[0].Pt() > 100 ) FilllHistBins(Lepton(ElectronColl[0]), ElectronColl[0].PassID(ID),  channel_string,ID, SF*weight_ll);
    else if(TightWindowCut) FilllHistBins(Lepton(ElectronColl[0]), ElectronColl[0].PassID(ID),  channel_string,ID, SF*weight_ll);

  }

  return;

}



void HNL_Lepton_ID_SF::MeasureIDSF(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, vector<bool> PassIDs, TString ID, double weight_ll){

  TString channel_string = GetChannelString(dilep_channel) ;
  
  bool TightWindowCut = ((GetLLMass(ElectronColl)- M_Z) < 10) ;
  

  std::vector<Lepton *> LeptonColl      = MakeLeptonPointerVector(ElectronColl);

  if(!IsData){
    if(!SameCharge(ElectronColl)) {
      if( ElectronColl[0].LeptonIsCF() || ElectronColl[1].LeptonIsCF()) return;
    }
  }
  else{
    if(SameCharge(ElectronColl)) weight_ll = -1;
    else weight_ll=1;
  }
  if(ElectronColl[0].PassID("passPOGTight")&& ElectronColl[0].Pt() > 35) {

    double SF=1;
    if(!IsData){
      double this_recosf  = mcCorr->ElectronReco_SF(ElectronColl[0].scEta(),ElectronColl[0].Pt(), 0);
      double this_idsf    = mcCorr->ElectronID_SF("passTightID", ElectronColl[0].scEta(), ElectronColl[0].Pt(), 0);

      double lep2_recosf  = mcCorr->ElectronReco_SF(ElectronColl[1].scEta(),ElectronColl[1].Pt(), 0);
      SF = this_recosf*this_idsf*lep2_recosf;
    }
    
    if(ElectronColl[1].Pt() > 100 )FilllHistBins(Lepton(ElectronColl[1]), PassIDs[1],  channel_string,ID, SF*weight_ll);
    else if(TightWindowCut) FilllHistBins(Lepton(ElectronColl[1]), PassIDs[1],  channel_string,ID, SF*weight_ll);

  }
  if(ElectronColl[1].PassID("passPOGTight")&& ElectronColl[1].Pt() > 35) {

    double SF=1;
    if(!IsData){
      double this_recosf  = mcCorr->ElectronReco_SF(ElectronColl[1].scEta(),ElectronColl[1].Pt(), 0);
      double this_idsf    = mcCorr->ElectronID_SF("passTightID", ElectronColl[1].scEta(), ElectronColl[1].Pt(), 0);

      double lep2_recosf  = mcCorr->ElectronReco_SF(ElectronColl[0].scEta(),ElectronColl[0].Pt(), 0);
      SF = this_recosf*this_idsf*lep2_recosf;
    }

    if(ElectronColl[0].Pt() > 100 ) FilllHistBins(Lepton(ElectronColl[0]), PassIDs[0],  channel_string,ID, SF*weight_ll);
    else if(TightWindowCut) FilllHistBins(Lepton(ElectronColl[0]), PassIDs[0],  channel_string,ID, SF*weight_ll);

  }

  return;

}


HNL_Lepton_ID_SF::HNL_Lepton_ID_SF(){


}
 
HNL_Lepton_ID_SF::~HNL_Lepton_ID_SF(){

}



void HNL_Lepton_ID_SF::FilllHistBins(Lepton lep, bool passID,  TString Channel_string,TString _ID, double lep_weight){

  /// Electron Binning https://twiki.cern.ch/twiki/pub/CMS/EgammaUL2016To2018/egammaEffi.txt_Ele_Medium_preVFP_egammaPlots.pdf  
  int nbin_pt16  =5;
  int nbin_pt    =6;
  int nbin_eta   =10;

  double ptbins16  [nbin_pt16+1]   = { 10.,20.,35., 50.,100.,300.};
  double ptbins    [nbin_pt  +1]   = { 10.,20.,35., 50.,100.,200.,300.};
  double etabins   [nbin_eta +1]   = {-2.5, -2.0, -1.566, -1.444, -0.8, 0, 0.8, 1.444, 1.566, 2.0, 2.5};
  
  /// Muon Binning https://indico.cern.ch/event/950340/contributions/3992871/attachments/2093495/3518135/tnp_2018_UL.pdf
  int nbin_Mupt    =8;
  int nbin_Mueta   =4;

  double Mu_ptbins [nbin_Mupt  + 1]    = {10., 15.,20.,25., 30.,40.,50., 60., 120.};
  double Mu_etabins[nbin_Mueta + 1]    = {0, 0.9, 1.2, 2.1, 2.4};

  //  int lepType = (IsData) ? 1 : lep.LeptonGenType();
  TString TypeLable="";

  if(!IsData && !lep.IsPrompt()) return;

  TString Den_tag=TypeLable+"_denom";

  double PtLep     = (lep.Pt() > 300) ? 299 : lep.Pt();
  double MuonPtLep = (lep.Pt() > 120) ? 119 : lep.Pt();
  
  if(IsData && PtLep > 100 && lep_weight < 0) return;

  //  cout << _ID +Den_tag << endl;

  // 2D plots
  FillHist(Channel_string+"/Pt_Eta_"     + _ID+Den_tag,    PtLep,     lep.defEta(),  lep_weight, nbin_pt,   ptbins,   nbin_eta , etabins);
  FillHist(Channel_string+"/Pt16_Eta_"   + _ID+Den_tag,    PtLep,     lep.defEta(),  lep_weight, nbin_pt16, ptbins16, nbin_eta , etabins);
  FillHist(Channel_string+"/Eta_MuonPt_" + _ID+Den_tag,    lep.fEta(),   MuonPtLep,  lep_weight, nbin_Mueta , Mu_etabins, nbin_Mupt, Mu_ptbins);
  FillHist(Channel_string+"/Eta_Pt_"     + _ID+Den_tag,    lep.defEta(), PtLep,      lep_weight, nbin_eta , etabins,  nbin_pt,   ptbins);
  FillHist(Channel_string+"/Eta_Pt16_"   + _ID+Den_tag,    lep.defEta(), PtLep,      lep_weight, nbin_eta , etabins,  nbin_pt16, ptbins16);
  FillHist(Channel_string+"/MuonPt_Eta_" + _ID+Den_tag,    MuonPtLep, lep.fEta(), lep_weight, nbin_Mupt, Mu_ptbins, nbin_Mueta , Mu_etabins);

  /// 1D Plots
  FillHist(Channel_string+"/1D_Pt_"     + lep.sEtaRegion() +"_"+_ID+Den_tag, PtLep,       lep_weight, nbin_pt,   ptbins,"");
  FillHist(Channel_string+"/1D_MuonPt_" + lep.sEtaRegion() +"_"+_ID+Den_tag, MuonPtLep,   lep_weight, nbin_Mupt, Mu_ptbins,"");

  FillHist(Channel_string+"/1D_Eta"     + lep.sPtRegion(GetYearString())+"_"+_ID+Den_tag,lep.defEta(),    lep_weight, nbin_eta   , etabins);
  FillHist(Channel_string+"/1D_MuonEta" + lep.sPtRegion(GetYearString())+"_"+_ID+Den_tag,lep.fEta(),   lep_weight, nbin_Mueta , Mu_etabins);
    
  if(passID){
    TString Num_tag=TypeLable+"_num";

    // 2D plots                                                                                                                                             
    FillHist(Channel_string+"/Pt_Eta_"  +_ID+Num_tag,PtLep, lep.defEta(),  lep_weight, nbin_pt, ptbins, nbin_eta , etabins);
    FillHist(Channel_string+"/Pt16_Eta_"+_ID+Num_tag,PtLep, lep.defEta(),  lep_weight, nbin_pt16, ptbins16, nbin_eta , etabins);
    FillHist(Channel_string+"/Eta_MuonPt_" + _ID+Num_tag,    lep.fEta(),   MuonPtLep,  lep_weight, nbin_Mueta , Mu_etabins, nbin_Mupt, Mu_ptbins);
    FillHist(Channel_string+"/Eta_Pt_"  +_ID+Num_tag,lep.defEta(), PtLep,  lep_weight, nbin_eta , etabins,  nbin_pt, ptbins);
    FillHist(Channel_string+"/Eta_Pt16_"+_ID+Num_tag,lep.defEta(), PtLep,  lep_weight, nbin_eta , etabins,  nbin_pt16, ptbins16);
    FillHist(Channel_string+"/MuonPt_Eta_" + _ID+Num_tag,  MuonPtLep,  lep.fEta(),  lep_weight, nbin_Mupt, Mu_ptbins, nbin_Mueta , Mu_etabins);

    /// 1D Plots                                                                                                                                                                                                                                                                                                  
    FillHist(Channel_string+"/1D_Pt_"     + lep.sEtaRegion()+"_"+_ID+Num_tag, PtLep,       lep_weight, nbin_pt, ptbins,"");
    FillHist(Channel_string+"/1D_MuonPt_" + lep.sEtaRegion()+"_"+_ID+Num_tag, MuonPtLep,   lep_weight, nbin_Mupt,       Mu_ptbins,"");
    FillHist(Channel_string+"/1D_Eta"     + lep.sPtRegion(GetYearString())+"_"+_ID+Num_tag,lep.defEta(),    lep_weight, nbin_eta , etabins);
    FillHist(Channel_string+"/1D_MuonEta" + lep.sPtRegion(GetYearString())+"_"+_ID+Num_tag,lep.fEta(),   lep_weight, nbin_Mueta , Mu_etabins);


    
  }


  return;
}

