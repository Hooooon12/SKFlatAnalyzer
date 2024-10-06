#include "AnalyzerParameter.h"

void AnalyzerParameter::Clear(){

  Name    = "Default";
  DefName = "Default";
  Channel = "Default";
  CutFlowDir="CutFlow";
  NameInclusive_Channel="Default";

  hprefix = "";
  hpostfix = "";
  SplitPlot = "NULL";
  MCCorrrectionIgnoreNoHist = false;
  DEBUG = false;
  isocut = -1;
  
  PlottingVerbose = 1;
  WriteOutVerbose = 1;
  SRConfig="";
  Jobs.clear();

  // IDS
  Electron_Tight_ID = "Default";
  Electron_Loose_ID = "Default";
  Electron_Veto_ID  = "Default";
  Electron_FR_ID    = "Default";
  Tau_Tight_ID      = "Default";
  Tau_Loose_ID      = "Default";
  Tau_Veto_ID       = "Default";
  Muon_Tight_ID     = "Default";
  Muon_Loose_ID     = "Default";
  Muon_Veto_ID      = "Default";
  Muon_FR_ID        = "Default";
  Muon_CF_ID        = "Default";
  TriggerSelection  = "Default";
  JetPUID           = "Default";
  Jet_ID            = "Default";
  FatJet_ID         = "Default";
  AK4JetColl        = "Default";
  AK4VBFJetColl     = "Default";
  AK8JetColl        = "Default";
  BJetColl          = "Default";
  BTagger           = "Default";
  BWP               = "Default";

  /// Keys
  k.Electron_ID_SF = "Default";
  k.Electron_Trigger_SF = "Default";
  k.Electron_FR = "Default";
  k.Electron_PR = "Default";
  k.Electron_CF = "Default";
  k.Electron_RECO_SF = "Default";
  k.Muon_RECO_SF = "Default";
  k.Muon_Trigger_SF = "Default";
  k.Muon_ID_SF = "Default";
  k.Muon_ISO_SF = "Default";
  k.Muon_Trigger_SF = "Default";
  k.Muon_FR = "Default";
  k.Muon_BB_FR = "Default";
  k.Muon_EC_FR = "Default";
  k.Muon_PR = "Default";
  k.Muon_CF = "Default";
  k.EMu_Trigger_SF = "Default";


  Electron_Tight_RelIso = 999.;
  Electron_Loose_RelIso = 999.;
  Electron_Veto_RelIso = 999.;
  Electron_UseMini = false;
  Electron_UsePtCone = false;
  Electron_MinPt = 10.;
  Muon_Tight_RelIso = 999.;
  Muon_Loose_RelIso = 999.;
  Muon_Veto_RelIso = 999.;
  Muon_UseMini = false;
  Muon_UsePtCone = false;
  Muon_UseTuneP = false;
  Muon_MinPt = 10.;

  SystDir_PU=0;
  SystDir_BTag="central";
  BJet_Method="1a";
  FakeRateMethod="BDTFlavour";
  FakeRateParam="pt_eta";
  FakeMethod="MC";
  CFMethod="MC";
  ConvMethod="MC";


  ApplyPR=false;

  Apply_Weight_Norm1Ipb   = true;
  Apply_Weight_MCSign    = true;
  Apply_Weight_LumiNorm  = true;
  Apply_Weight_SumQ      = true;
  Apply_Weight_PileUp    = true;
  Apply_Weight_PreFire   = true;
  Apply_Weight_kFactor   = true;
  Apply_Weight_TriggerSF = true;
  Apply_Weight_IDSF      = true;
  Apply_Weight_MuonTrackerSF = true;
  Apply_Weight_RECOSF    = true;
  Apply_Weight_Z0        = true;
  Apply_Weight_TopCorr   = true;
  Apply_Weight_DYCorr    = true;
  Apply_Weight_BJetSF    = true;
  Apply_Weight_PNETSF    = true;
  Apply_Weight_JetPUID   = true;
  Apply_Weight_PDF       = false; //JH

  Set_ElIDW=false;
  Set_MuIDW=false;
  Set_ElTrigW=false;
  Set_MuTrigW=false;



  syst_ = Central;

}

AnalyzerParameter::AnalyzerParameter(){

  Name    = "Default";
  DefName = "Default";
  Channel = "Default";
  CutFlowDir="CutFlow";
  hprefix = "";

  Jobs.clear();
  
  Electron_Tight_ID = "Default";
  Electron_Loose_ID = "Default";
  Electron_Veto_ID  = "Default";
  Electron_FR_ID    = "Default";
  Tau_Tight_ID      = "Default";
  Tau_Loose_ID      = "Default";
  Tau_Veto_ID       = "Default";
  Muon_Tight_ID     = "Default";
  Muon_Loose_ID     = "Default";
  Muon_Veto_ID      = "Default";
  Muon_FR_ID        = "Default";
  Muon_CF_ID        = "Default";

  TriggerSelection = "Dilep";
  JetPUID          = "Default";  
  Jet_ID           = "tight";  
  FatJet_ID        = "tight";  
  BTagger           = "Default";
  BWP               = "Default";
  AK4VBFJetColl     = "Default";
  AK4JetColl        = "Default";
  AK8JetColl        = "Default";
  BJetColl          = "Default";

  /// Keys                                                                                                                                                                                                                             
  k.Electron_ID_SF = "Default";
  k.Electron_Trigger_SF = "Default";
  k.Electron_FR = "Default";
  k.Electron_PR = "Default";
  k.Electron_CF = "Default";
  k.Electron_RECO_SF = "Default";
  k.Muon_RECO_SF = "MuonRecoSF"; 
  k.Muon_Trigger_SF = "Default";
  k.Muon_ID_SF = "Default";
  k.Muon_ISO_SF = "Default";
  k.Muon_Trigger_SF = "Default";
  k.Muon_FR = "Default";
  k.Muon_BB_FR = "Default";
  k.Muon_EC_FR = "Default";
  k.Muon_PR = "Default";
  k.Muon_CF = "Default";
  k.EMu_Trigger_SF = "Default";


  MCCorrrectionIgnoreNoHist = false;
  DEBUG = false;

  PlottingVerbose=0;
  WriteOutVerbose = 0;
  SRConfig="NULL";


  SystDir_PU=0;
  SystDir_BTag="Default";
  FakeRateMethod="BDTFlavour";  FakeMethod="MC";   CFMethod="MC";   ConvMethod="MC";   BJet_Method="Default";
  FakeRateParam="pt_eta";

  Set_ElIDW=false;
  Set_MuIDW=false;
  Set_ElTrigW=false;
  Set_MuTrigW=false;

  ApplyPR=false;


  /// Set weights to true by default
  Apply_Weight_Norm1Ipb   = true;
  Apply_Weight_LumiNorm  = true;
  Apply_Weight_SumQ      = true;
  Apply_Weight_PileUp    = true;
  Apply_Weight_PreFire   = true;
  Apply_Weight_kFactor   = true;
  Apply_Weight_TriggerSF = true;
  Apply_Weight_IDSF      = true;
  Apply_Weight_MuonTrackerSF = true;
  Apply_Weight_RECOSF    = true;
  Apply_Weight_Z0        = true;
  Apply_Weight_TopCorr   = true;
  Apply_Weight_DYCorr    = true;
  Apply_Weight_BJetSF    = true;
  Apply_Weight_PNETSF    = true;
  Apply_Weight_JetPUID   = true;
  Apply_Weight_PDF       = false; //JH


  syst_ = Central;
  
  /////////// JETS
  Jet_MinPt = 10.;  Jet_MaxEta = 5.;  BJet_Method                = "1a";
  /// ---------                                                                                                                                                                                
  FatJet_MinPt = 200.;  FatJet_MaxEta = 5.;
  /// ---------                                                                                                                                                                                
  Muon_MinPt = 10.; Muon_MaxEta = 2.4;
  /// ---------                                                                                                                                                                                
  Electron_MinPt = 10.;  Electron_MaxEta = 2.5;

}

/*
AnalyzerParameter& AnalyzerParameter::operator=(const AnalyzerParameter& p){
  if(this != &p){

    Name   = p.Name;
    DefName= p.DefName;
    Channel= p.Channel;
    CutFlowDir= p.CutFlowDir;
    hprefix= p.hprefix;
    hpostfix= p.hpostfix;
    MCCorrrectionIgnoreNoHist= p.MCCorrrectionIgnoreNoHist;
    DEBUG= p.DEBUG;
    Electron_Tight_ID= p.Electron_Tight_ID;
    Electron_Loose_ID= p.Electron_Loose_ID;
    Electron_Veto_ID= p.Electron_Veto_ID;
    Electron_FR_ID= p.Electron_FR_ID;
    Electron_CF_ID= p.Electron_CF_ID;
    Muon_Tight_ID= p.Muon_Tight_ID;
    Muon_Loose_ID= p.Muon_Loose_ID;
    Muon_Veto_ID= p.Muon_Veto_ID;
    Muon_FR_ID= p.Muon_FR_ID;
    Muon_CF_ID= p.Muon_CF_ID;
    JetPUID= p.JetPUID;
    TriggerSelection= p.TriggerSelection;
    Tau_Tight_ID= p.Tau_Tight_ID;
    Tau_Loose_ID= p.Tau_Loose_ID;
    Tau_Veto_ID= p.Tau_Veto_ID;
    Jet_ID= p.Jet_ID;
    FatJet_ID= p.FatJet_ID;
    
    Electron_UseMini= p.Electron_UseMini;
    Electron_UsePtCone= p.Electron_UsePtCone;
    Muon_UseMini= p.Muon_UseMini;
    Muon_UsePtCone= p.Muon_UsePtCone;
    Muon_UseTuneP= p.Muon_UseTuneP;

    Electron_Tight_RelIso= p.Electron_Tight_RelIso;
    Electron_Loose_RelIso= p.Electron_Loose_RelIso;
    Electron_Veto_RelIso= p.Electron_Veto_RelIso;
    Electron_MinPt= p.Electron_MinPt;
    Electron_MaxEta= p.Electron_MaxEta;
    Muon_Tight_RelIso= p.Muon_Tight_RelIso;
    Muon_Loose_RelIso= p.Muon_Loose_RelIso;
    Muon_Veto_RelIso= p.Muon_Veto_RelIso;
    Muon_MinPt= p.Muon_MinPt;
    Muon_MaxEta= p.Muon_MaxEta;
    Jet_MinPt= p.Jet_MinPt;
    Jet_MaxEta= p.Jet_MaxEta;
    FatJet_MinPt= p.FatJet_MinPt;
    FatJet_MaxEta= p.FatJet_MaxEta;

    Apply_Weight_LumiNorm= p.Apply_Weight_LumiNorm;
    Apply_Weight_SumW= p.Apply_Weight_SumW;
    Apply_Weight_PileUp= p.Apply_Weight_PileUp;
    Apply_Weight_PreFire= p.Apply_Weight_PreFire;
    Apply_Weight_kFactor= p.Apply_Weight_kFactor;
    Apply_Weight_IDSF= p.Apply_Weight_IDSF;
    Apply_Weight_TriggerSF= p.Apply_Weight_TriggerSF;
    Apply_Weight_RECOSF= p.Apply_Weight_RECOSF;
    Apply_Weight_Z0= p.Apply_Weight_Z0;
    Apply_Weight_TopCorr= p.Apply_Weight_TopCorr;
    Apply_Weight_DYCorr= p.Apply_Weight_DYCorr;
    Apply_Weight_BJetSF= p.Apply_Weight_BJetSF; 
    Apply_Weight_PNETSF= p.Apply_Weight_PNETSF;
    Apply_Weight_MuonTrackerSF= p.Apply_Weight_MuonTrackerSF;

    SRConfig= p.SRConfig;
    WriteOutVerbose= p.WriteOutVerbose;

    SystDir_PU= p.SystDir_PU;
    SystDir_BTag= p.SystDir_BTag;
    BJet_Method= p.BJet_Method;
    FakeMethod= p.FakeMethod;
    CFMethod= p.CFMethod;
    ConvMethod= p.ConvMethod;
    FakeRateMethod= p.FakeRateMethod;
    
    w.lumiweight= p.w.lumiweight;
    w.PUweight= p.w.PUweight;
    w.PUweight_up= p.w.PUweight_up;
    w.PUweight_down= p.w.PUweight_down;
    w.prefireweight= p.w.prefireweight;
    w.prefireweight_up= p.w.prefireweight_up;
    w.prefireweight_down= p.w.prefireweight_down;
    w.z0weight= p.w.z0weight;
    w.zptweight= p.w.zptweight;
    w.topptweight= p.w.topptweight;
    w.weakweight= p.w.weakweight;
    w.muonRECOSF= p.w.muonRECOSF;
    w.electronRECOSF= p.w.electronRECOSF;
    w.electronRECOSF_sys= p.w.electronRECOSF_sys;
    w.electronIDSF= p.w.electronIDSF;
    w.electronIDSF_sys= p.w.electronIDSF_sys;
    w.muonIDSF= p.w.muonIDSF;
    w.muonIDSF_sys= p.w.muonIDSF_sys;
    w.muonISOSF= p.w.muonISOSF;
    w.muonISOSF_sys= p.w.muonISOSF_sys;
    w.muonTrackerSF= p.w.muonTrackerSF;
    w.triggerSF= p.w.triggerSF;
    w.triggerSF_up= p.w.triggerSF_up;
    w.triggerSF_down= p.w.triggerSF_down;
    w.triggerSF_sys= p.w.triggerSF_sys;
    w.CFSF= p.w.CFSF;
    w.CFSF_up= p.w.CFSF_up;
    w.CFSF_down= p.w.CFSF_down;
    w.btagSF= p.w.btagSF;
    w.btagSF_hup= p.w.btagSF_hup;
    w.btagSF_hdown= p.w.btagSF_hdown;
    w.btagSF_lup= p.w.btagSF_lup;
    w.btagSF_ldown= p.w.btagSF_ldown;
    w.PNETSF= p.w.PNETSF;
    
    k.Muon_RECO_SF=p.k.Muon_RECO_SF;
    k.Electron_RECO_SF=p.k.Electron_RECO_SF;
    k.Electron_ID_SF=p.k.Electron_ID_SF;
    k.Muon_ID_SF=p.k.Muon_ID_SF;
    k.Muon_ISO_SF=p.k.Muon_ISO_SF;
    k.Muon_FR=p.k.Muon_FR;
    k.Muon_PR=p.k.Muon_PR;
    k.Electron_FR=p.k.Electron_FR;
    k.Electron_PR=p.k.Electron_PR; 
    k.Electron_CF=p.k.Electron_CF;
    k.EMu_Trigger_SF=p.k.EMu_Trigger_SF;
    k.Electron_Trigger_SF=p.k.Electron_Trigger_SF;
    k.Muon_Trigger_SF=p.k.Muon_Trigger_SF;
    k.Muon_CF=p.k.Muon_CF;
    k.triggerSF=p.k.triggerSF;
    syst_=p.syst_;

    Set_ElIDW=p.Set_ElIDW;
    Set_MuIDW=p.Set_MuIDW;
    Set_ElTrigW=p.Set_ElTrigW;
    Set_MuTrigW=p.Set_MuTrigW;
  }

  return *this;
}

*/


bool AnalyzerParameter::HasFlag(TString JobName) {

  if (std::find(Jobs.begin(), Jobs.end(), JobName) != Jobs.end())    return true;
  else return false;

}


void  AnalyzerParameter::SetChannel(TString inch){

  Channel = inch;
  return;
}

TString  AnalyzerParameter::GetChannelFlavour(){
  if(Channel == "EE"  ||Channel == "EEE" || Channel == "EEEE") return "Electron";
  if(Channel == "MuMu"  ||Channel == "MuMuMu" || Channel == "MuMuMuMu") return "Muon";
  return "ElectronMuon";

}
TString  AnalyzerParameter::ChannelType(){

  if(Channel == "EE"   || Channel =="MuMu"       || Channel == "EMu"   || Channel == "MuE" ) return "Dilepton";
  if(Channel == "EEE"  || Channel =="MuMuMu"     || Channel == "EMuL"  || Channel == "MuEL" ) return "Trilepton";
  if(Channel == "EEEE" || Channel =="MuMuMuMu"   || Channel == "EMuLL" || Channel == "MuELL" ) return "Quadlepton";

  return "Lepton";

}

TString  AnalyzerParameter::ChannelDir(){

  if(Channel == "Default")  return Name;
  return Name + "/"+ Channel;
}


TString  AnalyzerParameter::CutFlowDirChannel(){

  if(Channel == "Default")  return CutFlowDir+"/"+DefName;
  return "Channel"+CutFlowDir+"/"+DefName+"/"+Channel;
}

TString  AnalyzerParameter::CutFlowDirIncChannel(){

  if(Channel =="Default")  return CutFlowDir+"/"+DefName;
  return "Channel"+CutFlowDir+"/"+DefName+"/"+InclusiveChannelName();
}


TString  AnalyzerParameter::InclusiveChannelName(){
  
  if (Channel == "MuMu" ||  Channel == "EE" || Channel == "EMu" || Channel == "MuE") return "LL";
  if (Channel == "MuMuMu"||  Channel == "EEE" || Channel == "EMuL"|| Channel == "MuEL") return "LLL";
  return "LLLL";
  
}

double AnalyzerParameter::Weight(TString wkey){

  if(wkey == "Full") return w.lumiweight*w.PUweight* w.prefireweight*w.electronRECOSF*w.muonRECOSF*w.muonIDSF*w.muonISOSF*w.electronIDSF*w.triggerSF;

  if(wkey == "NoLumi") return w.PUweight* w.prefireweight*w.electronRECOSF*w.muonRECOSF*w.muonIDSF*w.muonISOSF*w.electronIDSF*w.triggerSF;
  
  return 1.;
  
}

double AnalyzerParameter::EventWeight(){
  
  double EvWeight = 1;

  if(Apply_Weight_LumiNorm) EvWeight*= w.lumiweight;
  if(Apply_Weight_PileUp)   EvWeight*= w.PUweight;
  if(Apply_Weight_PreFire)  EvWeight*= w.prefireweight;
  if(Apply_Weight_Z0)       EvWeight*= w.z0weight;
  if(Apply_Weight_TopCorr)  EvWeight*= w.topptweight;
  if(Apply_Weight_DYCorr)   EvWeight*= w.weakweight;
  if(Apply_Weight_DYCorr)   EvWeight*= w.zptweight;
  if(Apply_Weight_RECOSF)   EvWeight*= w.electronRECOSF*w.muonRECOSF;
  if(Apply_Weight_IDSF)     EvWeight*= w.muonIDSF*w.muonISOSF*w.electronIDSF;
  if(Apply_Weight_TriggerSF)EvWeight*= w.triggerSF;
  if(Apply_Weight_BJetSF)   EvWeight*= w.btagSF;
  if(Apply_Weight_PNETSF)   EvWeight*= w.PNETSF;
  if(Apply_Weight_MuonTrackerSF)  EvWeight*= w.muonTrackerSF;

  return EvWeight;

}


void AnalyzerParameter::PrintParameters(){

  cout << "\n PrintParameters:" <<endl;
  cout << "--------------------------------------" << endl;
  cout << "Name     = " << Name << endl;
  cout << "DefName  = " << DefName << endl;
  cout << "Channel  = " << Channel << endl;
  cout << "hprefix  = " << hprefix << endl;
  cout << "SRConfig = " << SRConfig << endl;

  cout << "\n IDs:" <<endl;
  cout << "--------------------------------------" << endl;
  cout << "FakeRateMethod = " << FakeRateMethod << endl;
  cout << "FakeRateParam  = " << FakeRateParam << endl;
  cout << "FakeRateName   = " << FakeRateName() << endl;

  cout << "--------------------------------------" << endl;

  cout << "Electron_Tight_ID = " << Electron_Tight_ID << endl;
  cout << "Electron_Loose_ID = " << Electron_Loose_ID << endl;
  cout << "Electron_Veto_ID  = " << Electron_Veto_ID << endl;
  cout << "Electron_FR_ID    = " << Electron_FR_ID << endl;
  cout << "Tau_Tight_ID      = " << Tau_Tight_ID << endl;
  cout << "Tau_Loose_ID      = " << Tau_Loose_ID << endl;
  cout << "Tau_Veto_ID       = " << Tau_Veto_ID << endl;
  cout << "Muon_Tight_ID     = " << Muon_Tight_ID << endl;
  cout << "Muon_Loose_ID     = " << Muon_Loose_ID << endl;
  cout << "Muon_Veto_ID      = " << Muon_Veto_ID << endl;
  cout << "Muon_FR_ID        = " << Muon_FR_ID << endl;
  cout << "Muon_CF_ID        = " << Muon_CF_ID << endl;
  cout << "TriggerSelection  = " << TriggerSelection << endl;
  cout << "JetPUID           = " << JetPUID << endl;
  cout << "Jet_ID            = " << Jet_ID << endl;
  cout << "FatJet_ID         = " << FatJet_ID << endl;
  cout << "AK4JetColl        = " << AK4JetColl << endl;
  cout << "AK4VBFJetColl     = " << AK4VBFJetColl << endl;
  cout << "AK8JetColl        = " << AK8JetColl << endl;
  cout << "BJetColl          = " << BJetColl << endl;
  cout << "BTagger           = " << BTagger << endl;
  cout << "BWP               = " << BWP << endl;

  cout << "\n Keys:" <<endl;
  cout << "--------------------------------------" << endl;
  cout << "k.Electron_ID_SF      = " << k.Electron_ID_SF << endl;
  cout << "k.Electron_Trigger_SF = " << k.Electron_Trigger_SF << endl;
  cout << "k.Electron_FR         = " << k.Electron_FR << endl;
  cout << "k.Electron_PR         = " << k.Electron_PR << endl;
  cout << "k.Electron_CF         = " << k.Electron_CF << endl;
  cout << "k.Electron_RECO_SF    = " << k.Electron_RECO_SF << endl;
  cout << "k.Muon_RECO_SF        = " << k.Muon_RECO_SF << endl;
  cout << "k.Muon_Trigger_SF     = " << k.Muon_Trigger_SF << endl;
  cout << "k.Muon_ID_SF          = " << k.Muon_ID_SF << endl;
  cout << "k.Muon_ISO_SF         = " << k.Muon_ISO_SF << endl;
  cout << "k.Muon_Trigger_SF     = " << k.Muon_Trigger_SF << endl;
  cout << "k.Muon_FR             = " << k.Muon_FR << endl;
  cout << "k.Muon_PR             = " << k.Muon_PR << endl;
  cout << "k.Muon_CF             = " << k.Muon_CF << endl;
  cout << "k.EMu_Trigger_SF      = " << k.EMu_Trigger_SF << endl;
  cout << "\n Weights:" << endl;
  cout << "--------------------------------------" << endl;
  cout << "w.lumiweight          = " << w.lumiweight << endl;
  cout << "w.PUweight            = " << w.PUweight << endl;
  cout << "w.PUweight_up         = " << w.PUweight_up<< endl;
  cout << "w.PUweight_down       = " << w.PUweight_down << endl;
  cout << "w.prefireweight       = "<< w.prefireweight << endl;  
  cout << "w.prefireweight_up    = " << w.prefireweight_up << endl; 
  cout << "w.prefireweight_down  = " << w.prefireweight_down<< endl;
  cout << "w.z0weight            = " << w.z0weight<< endl;
  cout << "w.weakweight          = " << w.weakweight<< endl;
  cout << "w.zptweight           = " << w.zptweight<< endl;
  cout << "w.topptweight         = " << w.topptweight<< endl;
  cout << "w.muonRECOSF          = " << w.muonRECOSF << endl;
  cout << "w.electronRECOSF      = " << w.electronRECOSF << endl;
  cout << "w.electronIDSF        = " << w.electronIDSF << endl;
  cout << "w.muonIDSF            = " << w.muonIDSF << endl;
  cout << "w.muontrackerSF       = " << w.muonTrackerSF << endl;
  cout << "w.triggerSF           = " << w.triggerSF << endl;
  cout << "w.CFSF                = " << w.CFSF << endl;
  cout << "w.btagSF              = " << w.btagSF << endl;
  cout << "w.PNETSF              = " << w.PNETSF << endl;
  cout << "w.JetPU               = " << w.JetPU << endl;
  cout << "w.EventSetupWeight     = " << w.EventSetupWeight << endl;

  cout << "Apply_Weight_Norm1pb   = " <<  Apply_Weight_Norm1Ipb  << endl;
  cout << "Apply_Weight_LumiNorm  = " <<  Apply_Weight_LumiNorm  << endl;
  cout << "Apply_Weight_SumQ      = " << Apply_Weight_SumQ      << endl;
  cout << "Apply_Weight_PileUp    = " << Apply_Weight_PileUp    << endl;
  cout << "Apply_Weight_PreFire   = " << Apply_Weight_PreFire   << endl;
  cout << "Apply_Weight_kFactor   = " << Apply_Weight_kFactor   << endl;
  cout << "Apply_Weight_TriggerSF = " << Apply_Weight_TriggerSF << endl;
  cout << "Apply_Weight_IDSF      = " << Apply_Weight_IDSF      << endl;
  cout << "Apply_Weight_MuonTrackerSF = " << Apply_Weight_MuonTrackerSF << endl;
  cout << "Apply_Weight_RECOSF    = " << Apply_Weight_RECOSF    << endl;
  cout << "Apply_Weight_Z0        = " << Apply_Weight_Z0        << endl;
  cout << "Apply_Weight_TopCorr   = " << Apply_Weight_TopCorr   << endl;
  cout << "Apply_Weight_DYCorr    = " << Apply_Weight_DYCorr    << endl;
  cout << "Apply_Weight_BJetSF    = " << Apply_Weight_BJetSF    << endl;
  cout << "Apply_Weight_PNETSF    = " << Apply_Weight_PNETSF    << endl;
  cout << "Apply_Weight_JetPUID   = " << Apply_Weight_JetPUID   << endl;
  cout << "Apply_Weight_PDF       = " << Apply_Weight_PDF       << endl; //JH
  return;
}

TString AnalyzerParameter::GetSystType(){

  if(syst_==Syst::Central){
    return "Central";
  }
  else if(syst_==Syst::FRAJ30)   return "AJ30";
  else if(syst_==Syst::FRAJ40)   return "AJ40";
  else if(syst_==Syst::FRAJ60)   return "AJ60";
  else if(syst_==Syst::FRLooseIDDJUp)     return "LIDUp";
  else if(syst_==Syst::FRLooseIDDJDown)   return "LIDDown";
  else if(syst_==Syst::FRPartonSFUp)   return "PSFUp"; 
  else if(syst_==Syst::FRPartonSFDown) return "PSFDown";
  else if(syst_==Syst::FRAltBinning) return "AltBinning";
  else if(syst_==Syst::PRApplied) return "PRApplied";
  else if(syst_==Syst::JetResUp) return "JetResUp";
  else if(syst_==Syst::JetResDown) return "JetResDown";
  else if(syst_==Syst::JetEnUp)  return "JetEnUp";
  else if(syst_==Syst::JetEnDown)   return "JetEnDown";
  else if(syst_==Syst::PrefireUp)   return "PrefireUp";
  else if(syst_==Syst::PrefireDown) return "PrefireDown";
  else if(syst_==Syst::PUUp)     return "PUUp";
  else if(syst_==Syst::PUDown)   return "PUDown";
  else if(syst_==Syst::JetPUIDUp)return "JetPUIDUp";
  else if(syst_==Syst::JetPUIDDown) return "JetPUIDDown";
  else if(syst_==Syst::MuonRecoSFUp)  return "MuonRecoSFUp";
  else if(syst_==Syst::MuonRecoSFDown) return "MuonRecoSFDown";
  else if(syst_==Syst::MuonEnUp)   return "MuonEnUp";
  else if(syst_==Syst::MuonEnDown) return "MuonEnDown";
  else if(syst_==Syst::MuonIDSFUp) return "MuonIDSFUp";
  else if(syst_==Syst::MuonIDSFDown)    return "MuonIDSFDown";
  else if(syst_==Syst::MuonISOSFUp)    return "MuonISOSFUp";
  else if(syst_==Syst::MuonISOSFDown)  return "MuonISOSFDown";
  else if(syst_==Syst::MuonTriggerSFUp)  return "MuonTriggerSFUp";
  else if(syst_==Syst::MuonTriggerSFDown)  return "MuonTriggerSFDown";
  else if(syst_==Syst::ElectronRecoSFUp) return "ElectronRecoSFUp";
  else if(syst_==Syst::ElectronRecoSFDown)   return "ElectronRecoSFDown";
  else if(syst_==Syst::ElectronResUp)  return "ElectronResUp";
  else if(syst_==Syst::ElectronResDown) return "ElectronResDown";
  else if(syst_==Syst::ElectronEnUp)   return "ElectronEnUp";
  else if(syst_==Syst::ElectronEnDown)   return "ElectronEnDown";
  else if(syst_==Syst::ElectronIDSFUp)   return "ElectronIDSFUp";
  else if(syst_==Syst::ElectronIDSFDown)  return "ElectronIDSFDown";
  else if(syst_==Syst::ElectronTriggerSFUp)  return "ElectronTriggerSFUp";
  else if(syst_==Syst::ElectronTriggerSFDown)  return "ElectronTriggerSFDown";
  else if(syst_==Syst::BTagSFHTagUp)   return "BTagSFHTagUp";
  else if(syst_==Syst::BTagSFHTagDown)   return "BTagSFHTagDown";
  else if(syst_==Syst::BTagSFLTagUp)   return "BTagSFLTagUp";
  else if(syst_==Syst::BTagSFLTagDown)   return "BTagSFLTagDown";
  else if(syst_==Syst::METUnclUp)   return "METUnclUp";
  else if(syst_==Syst::METUnclDown)  return "METUnclDown";
  else if(syst_==Syst::CFRateUp)    return "CFRateUp";
  else if(syst_==Syst::CFRateDown)   return "CFRateDown";
  else if(syst_==Syst::CFSFUp)    return "CFSFUp";
  else if(syst_==Syst::CFSFDown)   return "CFSFDown";
  else if(syst_==Syst::FRUp)   return "FRUp";
  else if(syst_==Syst::FRDown)   return "FRDown";
  
  else{
    cout << "[AnalyzerParameter::GetSystType] Wrong Syst" << endl;
    exit(ENODATA);
    return "ERROR";
  }

}

AnalyzerParameter::~AnalyzerParameter(){
}
