#include "HNL_LeptonCore.h"


AnalyzerParameter  HNL_LeptonCore::DefaultParam(const TString& s_setup_version, const TString& channel_st){


  AnalyzerParameter param_default;
  
  param_default.SetChannel(channel_st);
  param_default.Name     = s_setup_version;
  param_default.DefName  = s_setup_version;
  param_default.CutFlowDir = "CutFlowDir";
  param_default.hprefix  = "";
  param_default.hpostfix = "";
  param_default.PlottingVerbose = 1;

  /// Default BKG                                                                                                                                                                                                   
  param_default.ApplyPR=false;

  param_default.Apply_Weight_Norm1Ipb  = true;
  param_default.Apply_Weight_LumiNorm = true;
  param_default.Apply_Weight_SumQ     = true;
  param_default.Apply_Weight_PileUp   = true;
  param_default.Apply_Weight_PreFire  = true;
  param_default.Apply_Weight_kFactor  = true;
  param_default.Apply_Weight_RECOSF   = true;
  param_default.Apply_Weight_MuonTrackerSF = true;
  param_default.Apply_Weight_BJetSF   = true;
  param_default.Apply_Weight_PNETSF   = true;
  param_default.Apply_Weight_JetPUID   = true;

  param_default.Apply_Weight_PDF  = false; // FIXME later


  //// By default dont apply ID/Trigger SF                                                                                                                            
  param_default.Apply_Weight_IDSF     = false;
  param_default.Apply_Weight_TriggerSF= false;

  // Default settings if NOT s_setup_version is set                                                                                                                   

  param_default.syst_ = AnalyzerParameter::Central;
  param_default.MCCorrrectionIgnoreNoHist = false;

  /// Lepton ID DEFAULT                                                                                                                                               
  param_default.k.Electron_RECO_SF   = "RECO_SF";  // RECO_SF_AFB is alternative SF                                                                                           
  param_default.k.Muon_RECO_SF   = "HighPtMuonRecoSF";
  param_default.Muon_Veto_ID     = "NoCut";  param_default.Muon_Tight_ID     = "NoCut";
  param_default.Electron_Veto_ID = "NoCut";  param_default.Electron_Tight_ID = "NoCut";

  param_default.Tau_Veto_ID      = "JetVLElVLMuVL";

  /// ---------                                                                                                                                                       
  param_default.Muon_MinPt = 5.;       param_default.Muon_MaxEta = 2.4;
  param_default.Electron_MinPt = 10.;  param_default.Electron_MaxEta = 2.5;
  /// JET ID DEFAULT                                                                                                                                                  
  param_default.Jet_ID                     = "tight";
  param_default.Jet_MinPt                  = 10.;
  param_default.Jet_MaxEta                 = 5.;
  param_default.BJet_Method                = "1a";
  /// ---------                                                                                                                                                       
  param_default.FatJet_ID                  = "tight";
  param_default.FatJet_MinPt = 200.;  param_default.FatJet_MaxEta = 5.;

  /// Default Trigger                                                                                                                                                 
  param_default.TriggerSelection = "Dilep";

  param_default.BTagger = "DeepJet";
  param_default.BWP ="M";
  param_default.JetPUID = "Loose";

  param_default.AK4JetColl       = "TightPUL";
  param_default.AK4VBFJetColl    = "VBFTightPUL";
  param_default.AK8JetColl       = "HNL_PN";
  param_default.BJetColl         = "Tight";

  //// Weights                                                                                                                                                        
  param_default.w.lumiweight= 1;
  param_default.w.PUweight=1;
  param_default.w.PUweight_up=1;
  param_default.w.PUweight_down=1;
  param_default.w.prefireweight=1;
  param_default.w.prefireweight_up=1;
  param_default.w.prefireweight_down=1;
  param_default.w.z0weight=1;
  param_default.w.zptweight=1;
  param_default.w.zptweight_g=1;
  param_default.w.zptweight_gy=1;
  param_default.w.zptweight_gym=1;
  param_default.w.weakweight=1;
  param_default.w.topptweight=1;
  param_default.w.muonRECOSF=1;
  param_default.w.electronRECOSF=1;
  param_default.w.electronIDSF=1;
  param_default.w.muonIDSF=1;
  param_default.w.muonISOSF=1;
  param_default.w.triggerSF=1;
  param_default.w.CFSF=1;
  param_default.w.btagSF=1;
  param_default.w.PNETSF=1;
  param_default.w.JetPU = 1;
  param_default.w.EventSetupWeight=1;

  if(!IsDATA){
    param_default.w.lumiweight*= MCweight()*_Event.GetTriggerLumi("Full");
    param_default.w.PUweight      = GetPileUpWeight(nPileUp,0);
    param_default.w.PUweight_up   = GetPileUpWeight(nPileUp,1);
    param_default.w.PUweight_down = GetPileUpWeight(nPileUp,-1);
    param_default.w.prefireweight =  GetPrefireWeight(0);
    param_default.w.prefireweight_up= GetPrefireWeight(1);
    param_default.w.prefireweight_down =GetPrefireWeight(-1);
    param_default.w.z0weight=GetZ0Weight(vertex_Z);

    if(IsDYSample){
      if((abs(lhe_l0.ID())==11||abs(lhe_l0.ID())==13)) {
        TLorentzVector genZ=(gen_l0+gen_l1);
        param_default.w.zptweight = GetZptWeight(genZ.Pt(),genZ.Rapidity());
        param_default.w.zptweight_g =GetZptWeight(genZ.Pt());
        param_default.w.zptweight_gy =GetZptWeight(genZ.Pt(),genZ.Rapidity());
        param_default.w.zptweight_gym =GetZptWeight(genZ.Pt(),genZ.Rapidity(),genZ.M());
        param_default.w.weakweight=GetDYWeakWeight(genZ.M());

      } 
      else {
        if(HasFlag("SplitDYTau"))param_default.hprefix+="tau_";
      }
    }

    if(IsTTSample)   param_default.w.topptweight=mcCorr->GetTopPtReweight(All_Gens);
  }
  return param_default;  
}

