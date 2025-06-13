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
  param_default.Electron_Tight_ID = "HNL_ULID_"+GetYearString();

  param_default.k.Electron_ID_SF  = "TmpHNL_ULID_"+GetYearString();
  param_default.k.Muon_ID_SF      = "TmpHNL_ULID_"+GetYearString();
  param_default.Muon_FR_ID        = "HNL_ULID_FO";
  param_default.Electron_FR_ID    = "HNL_ULID_FO";

  return param_default;

}
