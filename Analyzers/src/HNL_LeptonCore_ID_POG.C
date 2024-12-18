#include "HNL_LeptonCore.h"

void HNL_LeptonCore::GetSetup_POGTight(AnalyzerParameter& param_default ){

  param_default.Apply_Weight_IDSF     = true;
  param_default.Apply_Weight_TriggerSF= true;

  param_default.FakeMethod = "DATA";
  param_default.CFMethod   = "DATA";
  param_default.ConvMethod = "MC";

  param_default.Muon_Tight_ID      = "POGTightWithTightIso";
  param_default.Electron_Tight_ID  = "passPOGTight";
  param_default.Muon_FR_ID     = "HNLooseV1";
  param_default.Electron_FR_ID = "HNLoosePOG";

  param_default.k.Muon_ID_SF       = "NUM_TightID_DEN_TrackerMuons";
  param_default.k.Muon_ISO_SF      = "NUM_TightRelIso_DEN_TightIDandIPCut";
  param_default.k.Electron_ID_SF   = "passTightID";
  param_default.FakeRateParam       = "PtCone";
  param_default.FakeRateMethod       = "Standard";

  param_default.k.Muon_FR            = "AwayJetPt40";
  param_default.k.Electron_FR        = "AwayJetPt40";
  param_default.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_POGTight";

  /// Just Cross check so no need to apply trigger SF                                                                                                                                                                                                                                                     
  param_default.Apply_Weight_TriggerSF = false;
  param_default.TriggerSelection = "Dilep";

  return;
}

void HNL_LeptonCore::GetSetup_HighPt(AnalyzerParameter& param_default ){

  param_default.Apply_Weight_IDSF     = false;
  param_default.Apply_Weight_TriggerSF= false;

  param_default.FakeMethod = "DATA";
  param_default.CFMethod   = "DATA";
  param_default.ConvMethod = "MC";
  param_default.Muon_Veto_ID     = "HNVeto_17028";
  param_default.Electron_Veto_ID = "HNVeto";
  /// Tight ID                                                                                                                                                      
  param_default.Muon_Tight_ID     = "POGHighPtWithLooseTrkIso";
  param_default.Electron_Tight_ID = "passHEEPID_v3";

  /////// Fake Bkg                                                                                                                                                  
  param_default.FakeRateMethod       = "Standard";
  param_default.FakeRateParam        = "PtCone";
  param_default.k.Muon_FR            = "FR_cent";
  param_default.k.Electron_FR        = "AwayJetPt40";
  param_default.Muon_FR_ID           = "POGHighPt";
  param_default.Electron_FR_ID       = "passHEEPLoose_v3";

  //// Lepton Corr [not needed as set false]                                                                                                                        
  param_default.k.Muon_RECO_SF       = "HighPtMuonRecoSF";
  param_default.k.Electron_CF        = "CFRate_InvPtEta3_PBSExtrap_Central_passHEEPID_v3";

                                                                                                                                                 
  param_default.Apply_Weight_TriggerSF = false;
  param_default.TriggerSelection = "Dilep";
  //  param_default.TriggerSelection     = "HighPt";


  return;
}

void HNL_LeptonCore::GetSetup_MVAPOG(AnalyzerParameter& param_default ){


  param_default.Apply_Weight_IDSF     = false;
  param_default.Apply_Weight_TriggerSF= false;
  param_default.FakeMethod = "DATA";    param_default.CFMethod   = "DATA";    param_default.ConvMethod = "MC";
  /// ID config                                                                                                                                                                                                                                           
  param_default.Muon_Veto_ID     = "HNVeto_17028";   param_default.Electron_Veto_ID  = "HNVeto";
  param_default.Muon_Tight_ID    = "HNTightV2";     param_default.Electron_Tight_ID = "passMVAID_Iso_WP80";
  param_default.Muon_FR_ID       = "HNLooseV1";     param_default.Electron_FR_ID    = "HNLooseV4";

  ///FIX                                                                                                                                                                                                                                                  
  param_default.FakeRateMethod       = "Standard";
  param_default.FakeRateParam        = "Pt";
  param_default.k.Muon_FR            = "AwayJetPt40";
  param_default.k.Electron_FR        = "AwayJetPt40";
  param_default.k.Electron_ID_SF     = "passHNTightV2";
  param_default.k.Muon_ID_SF         = "NUM_HNTightV2";
  param_default.k.Muon_RECO_SF       = "MuonRecoSF";

  param_default.TriggerSelection = "Dilep";
  if( param_default.Channel.Contains("EE"))   param_default.k.Electron_Trigger_SF = "DiElIso_HNL_ULID";
  if( param_default.Channel.Contains("MuMu")) param_default.k.Muon_Trigger_SF = "DiMuIso_HNL_ULID";
  if( param_default.Channel.Contains("EMu"))  param_default.k.EMu_Trigger_SF = "EMuIso_HNL_ULID";

  param_default.AK8JetColl       = "HNL";

  return;

}


