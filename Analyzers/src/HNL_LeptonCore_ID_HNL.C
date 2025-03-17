#include "HNL_LeptonCore.h"

void HNL_LeptonCore::GetSetup_HNL16(AnalyzerParameter& param_default ){

  param_default.Apply_Weight_PNETSF   = false;

  //// Trun on SF weights 
                                                                                                                                                                      
  param_default.Apply_Weight_IDSF     = true;
  param_default.Apply_Weight_TriggerSF= true;

  /// Set Bkg config      
                                                                                                                                                                      
  param_default.FakeMethod = "DATA";    param_default.CFMethod   = "DATA";    param_default.ConvMethod = "MC";
  /// ID config           
                                                                                                                                                                      
  param_default.Muon_Veto_ID     = "HNVeto_17028";   param_default.Electron_Veto_ID  = "HNVeto";
  param_default.Muon_Tight_ID    = "HNTightV2";     param_default.Electron_Tight_ID = "HNTightV2";
  param_default.Muon_FR_ID       = "HNLooseV1";     param_default.Electron_FR_ID    = "HNLooseV4";

  param_default.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_" + param_default.Electron_Tight_ID;

  param_default.FakeRateMethod       = "Standard";
  param_default.FakeRateParam        = "PtCone";
  param_default.k.Muon_FR            = "AwayJetPt40";
  param_default.k.Electron_FR        = "AwayJetPt40";
  param_default.k.Electron_ID_SF     = "passHNTightV2";
  param_default.k.Muon_ID_SF         = "NUM_HNTightV2";
  param_default.k.Muon_RECO_SF       = "MuonRecoSF";

  param_default.TriggerSelection = "Dilep";
  if(param_default.Channel.Contains("EE"))   param_default.k.Electron_Trigger_SF = "DiElIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("MuMu")) param_default.k.Muon_Trigger_SF = "DiMuIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("EMu"))  param_default.k.EMu_Trigger_SF = "EMuIso_HNL_ULIDv2";

  return;
}




void HNL_LeptonCore::GetSetup_HNTightV2(AnalyzerParameter& param_default ){
  
  //// Trun on SF weights                                                                                                                                           
  param_default.Apply_Weight_IDSF     = true;
  param_default.Apply_Weight_TriggerSF= true;

  /// Set Bkg config                                                                                                                                                
  param_default.FakeMethod = "DATA";    param_default.CFMethod   = "DATA";    param_default.ConvMethod = "MC";
  /// ID config                                                                                                                                                     
  param_default.Muon_Veto_ID     = "HNVeto_17028";   param_default.Electron_Veto_ID  = "HNVeto";
  param_default.Muon_Tight_ID    = "HNTightV2";     param_default.Electron_Tight_ID = "HNTightV2";
  param_default.Muon_FR_ID       = "HNLooseV1";     param_default.Electron_FR_ID    = "HNLooseV4";

  param_default.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_" + param_default.Electron_Tight_ID;

  param_default.FakeRateMethod       = "Standard";
  param_default.FakeRateParam        = "PtCone";
  param_default.k.Muon_FR            = "AwayJetPt40";
  param_default.k.Electron_FR        = "AwayJetPt40";
  param_default.k.Electron_ID_SF     = "passHNTightV2";
  param_default.k.Muon_ID_SF         = "NUM_HNTightV2";
  param_default.k.Muon_RECO_SF       = "MuonRecoSF";

  param_default.TriggerSelection = "Dilep";
  if(param_default.Channel.Contains("EE"))   param_default.k.Electron_Trigger_SF = "DiElIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("MuMu")) param_default.k.Muon_Trigger_SF = "DiMuIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("EMu"))  param_default.k.EMu_Trigger_SF = "EMuIso_HNL_ULIDv2";

  param_default.AK8JetColl       = "HNL";

  return;
}


void HNL_LeptonCore::GetSetup_HNLTopID(AnalyzerParameter& param_default ){

  param_default.Apply_Weight_IDSF     = true;
  param_default.Apply_Weight_TriggerSF= true;

  param_default.FakeMethod = "DATA";
  param_default.CFMethod   = "DATA";
  param_default.ConvMethod = "MC";
  param_default.Muon_Veto_ID     = "HNVeto_17028";  param_default.Muon_Tight_ID     = "TopHN";
  param_default.Electron_Veto_ID = "HNVeto";        param_default.Electron_Tight_ID = "TopHNSST";
  param_default.Muon_FR_ID        = "TopHNL";
  param_default.Electron_FR_ID    = "TopHNSSL_"+GetEraShort();

  param_default.FakeRateMethod       = "Standard";
  param_default.FakeRateParam        = "PtConeMini";

  param_default.k.Muon_FR            = "FR_cent";
  param_default.k.Electron_FR        = "FR_cent";

  param_default.k.Electron_ID_SF     = "passTopHNSST";
  param_default.k.Muon_ID_SF         = "NUM_TopHNT";
  param_default.k.Muon_RECO_SF       = "MuonRecoSF";

  param_default.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_TopHNSST";

  param_default.TriggerSelection = "Dilep";
  if(param_default.Channel.Contains("EE"))   param_default.k.Electron_Trigger_SF = "DiElIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("MuMu")) param_default.k.Muon_Trigger_SF = "DiMuIso_HNL_ULIDv2";
  if(param_default.Channel.Contains("EMu"))  param_default.k.EMu_Trigger_SF = "EMuIso_HNL_ULIDv2";

  return;
}
