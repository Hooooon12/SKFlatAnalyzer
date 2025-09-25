#include "HNL_LeptonCore.h"

AnalyzerParameter HNL_LeptonCore::Setup_Param_Peking(const TString& s_setup_version, const TString& channel_st){

  AnalyzerParameter param_default  =  DefaultParam(s_setup_version, channel_st);

  param_default.Apply_Weight_TriggerSF = false;
  param_default.Apply_Weight_IDSF      = false;
  param_default.CFMethod     = "Data";
  param_default.FakeMethod   = "MC";
  param_default.ConvMethod   = "MC";
  param_default.Muon_Tight_ID = "Peking";
  param_default.Electron_Tight_ID = "Peking_"+GetYearString();

  param_default.FakeRateMethod       = "Standard";
  param_default.FakeRateParam        = "PtCone";
  param_default.k.Muon_FR            = "FR_cent";
  param_default.k.Electron_FR        = "FR_cent";
  param_default.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_TopHNSST"; //// FIX                                                                                                                            

  return param_default;

}



AnalyzerParameter HNL_LeptonCore::Setup_Param_HNLOpt(const TString& s_setup_version, const TString& channel_st){

  AnalyzerParameter param_default  =  DefaultParam(s_setup_version, channel_st);

  param_default.Apply_Weight_TriggerSF = false;
  param_default.Apply_Weight_IDSF      = false;
  param_default.FakeMethod = "MC";
  param_default.CFMethod   = "MC";
  param_default.ConvMethod = "MC";
  param_default.Muon_Tight_ID     = "HNTightV2";
  param_default.Electron_Tight_ID = "HNTightV2";
  param_default.k.Electron_ID_SF  = "NUM_HNTightV2";
  param_default.k.Muon_ID_SF      = "NUM_HNTightV2";
  param_default.Muon_FR_ID        = "HNLooseV1";
  param_default.Electron_FR_ID = "HNLooseV4";

  return param_default;

}

AnalyzerParameter HNL_LeptonCore::Setup_Param_BDT(const TString& s_setup_version, const TString& channel_st){

  AnalyzerParameter param_default  =  DefaultParam(s_setup_version, channel_st);

  param_default.Apply_Weight_TriggerSF = false;
  param_default.Apply_Weight_IDSF      = false;
  param_default.FakeMethod = "MC";
  param_default.CFMethod   = "MC";
  param_default.ConvMethod = "MC";

  param_default.Muon_Veto_ID      = "HNVetoMVA";
  param_default.Muon_Tight_ID     = "HNL_ULID_"+GetYearString();
  param_default.Electron_Veto_ID  = "HNVetoMVA";
  param_default.Electron_Tight_ID = "HNL_HighPt_ULID_"+GetYearString();

  param_default.k.Muon_ID_SF         = "NUM_HNL_ULID_"+GetYearString();
  param_default.k.Muon_ISO_SF        = "Default";
  param_default.k.Electron_ID_SF     = "passHNL_ULIDv2_"+GetYearString();

  param_default.Muon_FR_ID        = "HNL_ULID_FO";
  param_default.Electron_FR_ID    = "HNL_HighPt_ULID_FO";
 
  param_default.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_HNL_HighPt_ULID";
  
  param_default.TriggerSelection = "Dilep";
  if(param_default.Channel.Contains("EE"))   param_default.k.Electron_Trigger_SF = "DiElIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("MuMu")) param_default.k.Muon_Trigger_SF = "DiMuIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("EMu"))  param_default.k.EMu_Trigger_SF = "EMuIso_HNL_ULIDv2";

  return param_default;

}
