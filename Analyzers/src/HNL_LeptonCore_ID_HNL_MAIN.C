#include "HNL_LeptonCore.h"


void HNL_LeptonCore::GetSetup_HNLHPTID(AnalyzerParameter& param_default ){
  GetSetup_HNLID(param_default);

  param_default.Electron_Tight_ID = "HNL_HighPt_ULID_"+GetYearString();
  param_default.Electron_FR_ID    = "HNL_HighPt_ULID_FO";
 
  param_default.k.Electron_ID_SF     = "passHNL_ULIDv2_"+GetYearString();
  param_default.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_HNL_HighPt_ULID";
  
  param_default.TriggerSelection = "Dilep";

  if(param_default.Channel.Contains("EE"))   param_default.k.Electron_Trigger_SF = "DiElIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("MuMu")) param_default.k.Muon_Trigger_SF = "DiMuIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("EMu"))  param_default.k.EMu_Trigger_SF = "EMuIso_HNL_ULIDv2";



  return;
}

void HNL_LeptonCore::GetSetup_HNLID(AnalyzerParameter& param_default ){

  /// MAIN SETUP FOR ANALYSIS                                                                                                                                                                                                                                                                             
  param_default.Apply_Weight_IDSF     = true;
  param_default.Apply_Weight_TriggerSF= true;

  param_default.FakeMethod = "DATA";
  param_default.CFMethod   = "DATA";
  param_default.ConvMethod = "MC";

  param_default.Muon_Veto_ID      = "HNVetoMVA";
  param_default.Muon_Tight_ID     = "HNL_ULID_"+GetYearString();
  param_default.Electron_Veto_ID  = "HNVetoMVA";
  param_default.Electron_Tight_ID = "HNL_ULID_"+GetYearString();


  ///Fakes                                                                                                                                                                                                                                                                                                
  param_default.FakeRateMethod    = "Standard";
  param_default.FakeRateParam     = "PtParton";

  param_default.Muon_FR_ID        = "HNL_ULID_FO";
  param_default.Electron_FR_ID    = "HNL_ULID_FO";

  TString JFRJetPt = "_AJ40";
  TString MuFRBin =  "";
  TString ElFRBin = "_El12";

  if(GetEra() == "2016preVFP"){
    param_default.k.Muon_FR            = "HNL_ULID_FO_v1_a"+JFRJetPt+MuFRBin;
    param_default.k.Electron_FR        = "HNL_ULID_FO_v9_a"+JFRJetPt+ElFRBin;
  }
  if(GetEra() == "2016postVFP"){
    param_default.k.Muon_FR            = "HNL_ULID_FO_v2_a"+JFRJetPt+MuFRBin;
    param_default.k.Electron_FR        = "HNL_ULID_FO_v9_a"+JFRJetPt+ElFRBin;
  }
  if(GetYearString() == "2017"){
    param_default.k.Muon_FR            = "HNL_ULID_FO_v2_a"+JFRJetPt+MuFRBin;;
    param_default.k.Electron_FR        = "HNL_ULID_FO_v9_a"+JFRJetPt+ElFRBin;
  }
  if(GetYearString() == "2018"){
    param_default.k.Muon_FR         = "HNL_ULID_FO_v3_a"+JFRJetPt+MuFRBin;;
    param_default.k.Electron_FR     = "HNL_ULID_FO_v9_a"+JFRJetPt+ElFRBin;
  }

  param_default.k.Muon_ID_SF         = "NUM_HNL_ULID_"+GetYearString();
  param_default.k.Muon_ISO_SF        = "Default";
  param_default.k.Electron_ID_SF     = "passHNL_ULID_"+GetYearString();

  param_default.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_HNL_HighPt_ULID";
  param_default.TriggerSelection = "Dilep";
  if(param_default.Channel.Contains("EE"))   param_default.k.Electron_Trigger_SF = "DiElIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("MuMu")) param_default.k.Muon_Trigger_SF = "DiMuIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("EMu"))  param_default.k.EMu_Trigger_SF = "EMuIso_HNL_ULIDv2";

  return;
}
