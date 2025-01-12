#include "AnalyzerCore.h"


void AnalyzerCore::InitializeElectronIDTreeVars(){

  bdt_id_Pt=-1;
  bdt_id_Eta=-1;
  bdt_id_MiniIsoChHad=-1;
  bdt_id_MiniIsoNHad=-1;
  bdt_id_MiniIsoPhHad=-1;
  bdt_id_IsoChHad=-1;
  bdt_id_IsoNHad=-1 ;
  bdt_id_IsoPhHad=-1;
  bdt_id_RelMiniIsoCh=-1;
  bdt_id_RelMiniIsoN=-1;
  bdt_id_Dxy=-1;
  bdt_id_Dz=-1;
  bdt_id_DxySig=-1;
  bdt_id_DzSig=-1;
  bdt_id_RelIso=-1;
  bdt_id_IP3D=-1;
  bdt_id_PtRatio=-1;
  bdt_id_PtRatioV2=-1;
  bdt_id_PtRatioV3=-1;
  bdt_id_PtRel=-1;
  bdt_id_PtRelV2=-1;
  bdt_id_PtRelV3=-1;
  bdt_id_CEMFracCJ=-1;
  bdt_id_NEMFracCJ=-1;
  bdt_id_CHFracCJ=-1;
  bdt_id_NHFracCJ=-1;
  bdt_id_MuFracCJ=-1;
  bdt_id_JetDiscCJ=-1;
  bdt_id_JetDiscCJCvsB=-1;
  bdt_id_JetDiscCJCvsL=-1;
  bdt_id_MVA=-1;
  bdt_id_MVAIso=-1;
  bdt_id_MVARaw=-1;
  bdt_id_MVAIsoRaw=-1;
  bdt_id_Full5x5_sigmaIetaIeta=-1;
  bdt_id_dEtaSeed=-1;
  bdt_id_dPhiIn=-1;
  bdt_id_HoverE=-1;
  bdt_id_EoverP=-1;
  bdt_id_FBrem=-1;
  bdt_id_Rho=-1;
  bdt_id_R9=-1;
  bdt_id_psEoRraw=-1;

  bdt_id_TrkIso=-1;   //InvEminusInvP=-1;

  bdt_id_dr03TkSumPt=-1;
  bdt_id_dr03HcalTowerSumEt=-1;

  bdt_id_e55=-1;
  bdt_id_e2x5OverE5x5=-1;
  bdt_id_e1x5OverE5x5=-1;
  bdt_id_EtaWidth=-1;
  bdt_id_PhiWidth=-1;
  bdt_id_ecalPFClusterIso=-1;
  bdt_id_hcalPFClusterIso=-1;
  bdt_id_MissingHits=-1;
  bdt_id_isEcalDriven=-1;
  bdt_id_PassConversionVeto=-1;
  bdt_id_IsGsfCtfScPixChargeConsistent=-1;
  bdt_id_IsGsfScPixChargeConsistent=-1 ;
  bdt_id_IsGsfCtfChargeConsistent=-1;


  return;
}

void AnalyzerCore::InitializeIDTreeVars(){

  // Initialise all BDT var to default 
  // Kinematic                        
  bdt_id_Fake_Type=-999,bdt_id_Fake_Flavour=-999;
  bdt_id_Pt=-999, bdt_id_Eta=-999, bdt_id_PtBinned=-999,bdt_id_PtBinned2=-999;
  bdt_id_PtRatio=-999,  bdt_id_PtRel=-999; bdt_id_MassDrop=-999;

  bdt_id_PtRatioV2=-999,  bdt_id_PtRelV2=-999;
  bdt_id_PtRatioV3=-999,  bdt_id_PtRelV3=-999;
  /// Pileup 
  bdt_id_PileUp = -999;

  // Iso 
  bdt_id_MiniIsoChHad=-999;  bdt_id_MiniIsoNHad=-999;  bdt_id_MiniIsoPhHad=-999;  bdt_id_IsoChHad=-999;  bdt_id_IsoNHad=-999;  bdt_id_IsoPhHad=-999;
  bdt_id_Minireliso=-999.;  bdt_id_RelMiniIsoCh=-999.;  bdt_id_RelMiniIsoN=-999.; bdt_id_RelIso=-999.;


  // Closejet  
  bdt_id_CEMFracCJ=-999, bdt_id_NEMFracCJ=-999, bdt_id_CHFracCJ=-999, bdt_id_NHFracCJ=-999,bdt_id_MuFracCJ=-999;

  bdt_id_JetDiscCJ=-999,  bdt_id_JetDiscCJCvsB=-999,  bdt_id_JetDiscCJCvsL=-999;

  /// IP   
  bdt_id_Dxy=-999.;  bdt_id_DxySig=-999.;  bdt_id_Dz=-999.;  bdt_id_DzSig=-999., bdt_id_IP3D=-999.;

  // MVA 
  bdt_id_MVA=-999.;  bdt_id_MVAIso=-999.;  bdt_id_Chi2=-999.;
  bdt_id_MVARaw=-999;
  bdt_id_MVAIsoRaw=-999;

  // Hits 
  bdt_id_Validhits=-999.;  bdt_id_Matched_stations=-999.;
  bdt_id_Pixel_hits=-999.;  bdt_id_Tracker_layers=-999.;  bdt_id_MissingHits=-999.;

  // El Shower/SCL    
  bdt_id_Full5x5_sigmaIetaIeta=-999.; bdt_id_dEtaSeed=-999.;  bdt_id_dPhiIn=-999.;  bdt_id_dEtaIn=-999.;  bdt_id_EtaWidth=-999.;  bdt_id_PhiWidth=-999.;  bdt_id_HoverE=-999.;
  bdt_id_TrkIso=-999.;  bdt_id_e2x5OverE5x5=-999;  bdt_id_e1x5OverE5x5=-999;
  bdt_id_e55=-999;
  bdt_id_dr03HcalTowerSumEt=-999;  bdt_id_dr03TkSumPt=-999;
  bdt_id_InvEminusInvP=-999.;  bdt_id_ecalPFClusterIso=-999.;  bdt_id_hcalPFClusterIso=-999.;

  // IDs   
  bdt_id_POGTight=-999;  bdt_id_POGMedium = -999;  bdt_id_HNTightID = -999, bdt_id_POGMVA80ID=-999, bdt_id_POGMVA90ID=-999;

  // COnv/CF  
  bdt_id_isEcalDriven= -999;  bdt_id_EoverP=-999;  bdt_id_FBrem=-999;  bdt_id_PassConversionVeto=-999 ,bdt_id_Rho=-999;
  bdt_id_IsGsfCtfScPixChargeConsistent =-999;  bdt_id_IsGsfScPixChargeConsistent =-999;  bdt_id_IsGsfCtfChargeConsistent =-999;  bdt_id_R9=-999;
  bdt_id_psEoRraw= -999;
  bdt_id_w_id_tot=1;

  return;

}

void AnalyzerCore::PrintBDTVariables(Electron el,TString label){

  if(!el.IsPrompt()) return;
  if(!el.IsEC()) return;

  cout << "PrintBDTVariables " << label << endl;
  InitializeIDTreeVars();

  Lepton *lep = (Lepton *)(&el);
  for(auto i : el.MAPBDT()) cout << i.first << "  " << i.second  << endl;
  SetBDTIDVar(lep);
  SetBDTIDVariablesElectron(el);
  PrintBDTInput();

}





void AnalyzerCore::FillBDTHists(Muon mu,TString cut, double w, bool SplitFakes){


  map<TString, double> mapBDT = mu.MAPBDT();
  for(auto imap : mapBDT )  FillHist("BDTVariables_"+cut+"/"+ mu.GetFlavour()+ "/"+imap.first, imap.second  , w, 200, -1., 1);

  InitializeIDTreeVars();
  Lepton *lep = (Lepton *)(&mu);
  SetBDTIDVar(lep);
  SetBDTIDVariablesMuon(mu);

  if(SplitFakes){
    TString lep_fake_tag=MatchGenDef(All_Gens, mu);
    vector<TString> Lists = {"pi+","q","K+","__","g","B0","B+","D0","D+"};
    if (std::find(Lists.begin(), Lists.end(), lep_fake_tag) == Lists.end()) return;
    cut=cut+"_"+lep_fake_tag;
  }

  FillHist( cut+ "/Lepton_bdt_id_Pt",        bdt_id_Pt, w, 100, 0., 500.,"");
  FillHist( cut+ "/Lepton_bdt_id_Eta",       bdt_id_Eta, w, 60, -3., 3.,"");
  FillHist( cut+ "/Lepton_bdt_id_PtRatio",   bdt_id_PtRatio, w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRel",     bdt_id_PtRel, w, 500, 0., 500., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRatioV2",   bdt_id_PtRatioV2, w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRelV2",     bdt_id_PtRelV2, w, 500, 0., 500., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRatioV3",   bdt_id_PtRatioV3, w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRelV3",     bdt_id_PtRelV3, w, 500, 0., 500., "");

  FillHist( cut+ "/Lepton_bdt_id_MiniIsoChHad",  bdt_id_MiniIsoChHad, w, 200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_MiniIsoNHad",   bdt_id_MiniIsoNHad, w,  200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_MiniIsoPhHad",  bdt_id_MiniIsoPhHad, w, 200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_IsoChHad",      bdt_id_IsoChHad, w,     200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_IsoNHad",       bdt_id_IsoNHad, w,      200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_IsoPhHad" ,     bdt_id_IsoPhHad, w,     200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_RelMiniIsoCh",  bdt_id_RelMiniIsoCh, w, 200, 0, 0.4);
  FillHist( cut+ "/Lepton_bdt_id_RelMiniIsoN",   bdt_id_RelMiniIsoN, w,  200, 0, 0.4);
  FillHist( cut+ "/Lepton_bdt_id_RelIso",        bdt_id_RelIso, w,       200, 0, 6);
  FillHist( cut+ "/Lepton_bdt_id_Minireliso",    bdt_id_Minireliso, w,   200, 0, 2);

  FillHist( cut+ "/Lepton_bdt_id_CEMFracCJ",  bdt_id_CEMFracCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_NEMFracCJ",  bdt_id_NEMFracCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_EMFracCJ",  bdt_id_CEMFracCJ+bdt_id_NEMFracCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_CHFracCJ",   bdt_id_CHFracCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_NHFracCJ",   bdt_id_NHFracCJ, w,200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_HFracCJ",   bdt_id_CHFracCJ+bdt_id_NHFracCJ, w,200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_MuFracCJ",   bdt_id_MuFracCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJ",  bdt_id_JetDiscCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJCvsB",  bdt_id_JetDiscCJCvsB, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJCvsL",  bdt_id_JetDiscCJCvsL, w, 200, 0, 2);

  double JetDiscBins[6] = { 0.,0.1, 0.2, 0.3,  0.75, 1.};

  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJ2",  bdt_id_JetDiscCJ, w, 5, JetDiscBins);
  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJCvsB2",  bdt_id_JetDiscCJCvsB, w, 5, JetDiscBins);
  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJCvsL2",  bdt_id_JetDiscCJCvsL, w, 5, JetDiscBins);



  /// IP   
  FillHist( cut+ "/Lepton_bdt_id_Dxy",     bdt_id_Dxy, w, 200 , -30, 10);
  FillHist( cut+ "/Lepton_bdt_id_DxySig",  bdt_id_DxySig, w, 200 , -30, 10);
  FillHist( cut+ "/Lepton_bdt_id_Dz" ,     bdt_id_Dz, w, 200 , -30, 10);
  FillHist( cut+ "/Lepton_bdt_id_DzSig",   bdt_id_DzSig, w,200 , -30, 10);
  FillHist( cut+ "/Lepton_bdt_id_IP3D",    bdt_id_IP3D, w, 100, 0, 8);

  FillHist( cut+ "/Lepton_bdt_id_MVA",     bdt_id_MVA, w, 220 , -1 ,  10);
  FillHist( cut+ "/Lepton_bdt_id_MVAIso",  bdt_id_MVAIso, w,220 , -1 ,10);

  FillHist( cut+ "/Lepton_bdt_id_MVARaw",     bdt_id_MVARaw, w,   500 , -1 ,1);
  FillHist( cut+ "/Lepton_bdt_id_MVAIsoRaw",  bdt_id_MVAIsoRaw, w,500 , -1 ,1);

  return;

}





void AnalyzerCore::FillBDTHists(Electron el,TString cut, double w, bool SplitFakes){


  map<TString, double> mapBDT = el.MAPBDT();
  for(auto imap : mapBDT )  FillHist("BDTVariables_"+cut+"/"+ el.GetFlavour()+ "/"+imap.first, imap.second  , w, 200, -1., 1);


  InitializeIDTreeVars();
  Lepton *lep = (Lepton *)(&el);
  SetBDTIDVar(lep);
  SetBDTIDVariablesElectron(el);

  FillHist( cut+ "/Lepton_bdt_id_Pt",        bdt_id_Pt, w, 100, 0., 500.,"");
  FillHist( cut+ "/Lepton_bdt_id_Eta",       bdt_id_Eta, w, 60, -3., 3.,"");
  FillHist( cut+ "/Lepton_bdt_id_PtBinned",  bdt_id_PtBinned, w, 5, 0,5 );
  FillHist( cut+ "/Lepton_bdt_id_PtRatio",   bdt_id_PtRatio, w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRel",     bdt_id_PtRel, w, 500, 0., 500., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRatioV2",   bdt_id_PtRatioV2, w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRelV2",     bdt_id_PtRelV2, w, 500, 0., 500., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRatioV3",   bdt_id_PtRatioV3, w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRelV3",     bdt_id_PtRelV3, w, 500, 0., 500., "");


  FillHist( cut+ "/Lepton_bdt_id_PtRatio_CorrJet", JetLeptonPtRatioLepAwareMuon(el,true,false,false) , w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRatio_CorrJetMu", JetLeptonPtRatioLepAwareMuon(el,true,true,false) , w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRatio_UncorrLep_CorrJet", JetLeptonPtRatioLepAwareMuon(el,true,false,true) , w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRatio_UncorrLep_CorrJetMu", JetLeptonPtRatioLepAwareMuon(el,true, true, true) , w,  500, 0., 2., "");


  FillHist( cut+ "/Lepton_bdt_id_PtRel_CorrJet", JetLeptonPtRelLepAwareMuon(el,true,false,false) , w,  500, 0., 500., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRel_CorrJetMu", JetLeptonPtRelLepAwareMuon(el,true,true,false) , w,  500, 0., 500., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRel_UncorrLep_CorrJet", JetLeptonPtRelLepAwareMuon(el,true,false,true) , w,  500, 0., 500., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRel_UncorrLep_CorrJetMu", JetLeptonPtRelLepAwareMuon(el,true, true, true) , w,  500, 0., 500., "");

  FillHist( cut+ "/Lepton_bdt_id_PtRatio_UnCorrJet", JetLeptonPtRatioLepAwareMuon(el,false,false,false) , w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRatio_UnCorrJetMu", JetLeptonPtRatioLepAwareMuon(el,false,true,false) , w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRatio_UncorrLep_UnCorrJet", JetLeptonPtRatioLepAwareMuon(el,false,false,true) , w,  500, 0., 2., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRatio_UncorrLep_UnCorrJetMu", JetLeptonPtRatioLepAwareMuon(el,false, true, true) , w,  500, 0., 2., "");


  FillHist( cut+ "/Lepton_bdt_id_PtRel_UnCorrJet", JetLeptonPtRelLepAwareMuon(el,false,false,false) , w,  500, 0., 500., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRel_UnCorrJetMu", JetLeptonPtRelLepAwareMuon(el,false,true,false) , w,  500, 0., 500., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRel_UncorrLep_UnCorrJet", JetLeptonPtRelLepAwareMuon(el,false,false,true) , w,  500, 0., 500., "");
  FillHist( cut+ "/Lepton_bdt_id_PtRel_UncorrLep_UnCorrJetMu", JetLeptonPtRelLepAwareMuon(el,false, true, true) , w,  500, 0., 500., "");


  if(1){

    std::vector<Jet> jets = GetAllJets();

    double  mindR=0.4;

    Jet closejet;
    Jet UnCorrJet;
    for(auto jet : jets){
      if(jet.Pt() < 10) continue; //// Make consistent with data cut 

      if (lep->DeltaR(jet) < mindR) {
        UnCorrJet=jet;
        closejet = GetCorrectedJetCloseToLepton(*lep,jet);
        mindR = lep->DeltaR(jet);
      }
    }

    if (mindR == 0.4)  FillHist( cut+ "/Lepton_bdt_id_PtRatio_noLepCorr",  std::min(1./ (1. + el.RelIso()), 1.5) ,  w,  500, 0., 2., "");
    else FillHist( cut+ "/Lepton_bdt_id_PtRatio_noLepCorr",    std::min(el.Pt() / UnCorrJet.Pt(),1.5)  ,  w,  500, 0., 2., "");


    FillHist( cut+ "/Lepton_bdt_id_CHMultCJ",   closejet.CHMult() , w, 30, 0., 30., "");
    FillHist( cut+ "/Lepton_bdt_id_NMultCJ",   closejet.NMult() , w, 30, 0., 30, "");

    /////////// Now test corrected Energy Fractions   
    /// Since Jet Energy is updated the fraction in MC needs updating  
    //// values stored are those from MiniAOD                          
    ///https://github.com/cms-sw/cmssw/blob/master/DataFormats/PatCandidates/interface/Jet.h#L399   

  }





  FillHist( cut+ "/Lepton_bdt_id_MiniIsoChHad",  bdt_id_MiniIsoChHad, w, 200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_MiniIsoNHad",   bdt_id_MiniIsoNHad, w,  200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_MiniIsoPhHad",  bdt_id_MiniIsoPhHad, w, 200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_IsoChHad",      bdt_id_IsoChHad, w,     200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_IsoNHad",       bdt_id_IsoNHad, w,      200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_IsoPhHad" ,     bdt_id_IsoPhHad, w,     200, 0, 100);
  FillHist( cut+ "/Lepton_bdt_id_RelMiniIsoCh",  bdt_id_RelMiniIsoCh, w, 200, 0, 0.4);
  FillHist( cut+ "/Lepton_bdt_id_RelMiniIsoN",   bdt_id_RelMiniIsoN, w,  200, 0, 0.4);
  FillHist( cut+ "/Lepton_bdt_id_RelIso",        bdt_id_RelIso, w,       200, 0, 6);
  FillHist( cut+ "/Lepton_bdt_id_Minireliso",    bdt_id_Minireliso, w,   200, 0, 2);

  FillHist( cut+ "/Lepton_bdt_id_CEMFracCJ",  bdt_id_CEMFracCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_NEMFracCJ",  bdt_id_NEMFracCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_EMFracCJ",  bdt_id_CEMFracCJ+bdt_id_NEMFracCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_CHFracCJ",   bdt_id_CHFracCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_NHFracCJ",   bdt_id_NHFracCJ, w,200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_HFracCJ",   bdt_id_CHFracCJ+bdt_id_NHFracCJ, w,200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_MuFracCJ",   bdt_id_MuFracCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJ",  bdt_id_JetDiscCJ, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJCvsB",  bdt_id_JetDiscCJCvsB, w, 200, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJCvsL",  bdt_id_JetDiscCJCvsL, w, 200, 0, 2);

  double JetDiscBins[6] = { 0.,0.1, 0.2, 0.3,  0.75, 1.};

  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJ2",  bdt_id_JetDiscCJ, w, 5, JetDiscBins);
  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJCvsB2",  bdt_id_JetDiscCJCvsB, w, 5, JetDiscBins);
  FillHist( cut+ "/Lepton_bdt_id_JetDiscCJCvsL2",  bdt_id_JetDiscCJCvsL, w, 5, JetDiscBins);

  /// IP  
  FillHist( cut+ "/Lepton_bdt_id_Dxy",     bdt_id_Dxy, w, 200 , -30, 10);
  FillHist( cut+ "/Lepton_bdt_id_DxySig",  bdt_id_DxySig, w, 200 , -30, 10);
  FillHist( cut+ "/Lepton_bdt_id_Dz" ,     bdt_id_Dz, w, 200 , -30, 10);
  FillHist( cut+ "/Lepton_bdt_id_DzSig",   bdt_id_DzSig, w,200 , -30, 10);
  FillHist( cut+ "/Lepton_bdt_id_IP3D",    bdt_id_IP3D, w, 100, 0, 8);
  FillHist( cut+ "/Lepton_bdt_id_MVA",     bdt_id_MVA, w, 220 , -1 ,  10);
  FillHist( cut+ "/Lepton_bdt_id_MVAIso",  bdt_id_MVAIso, w,220 , -1 ,10);
  FillHist( cut+ "/Lepton_bdt_id_MVARaw",     bdt_id_MVARaw, w,   500 , -1 ,1);
  FillHist( cut+ "/Lepton_bdt_id_MVAIsoRaw",  bdt_id_MVAIsoRaw, w,500 , -1 ,1);
  FillHist( cut+ "/Lepton_bdt_id_MissingHits",  bdt_id_MissingHits, w, 5 ,0 ,5);

  // El Shower/SCL  
  FillHist( cut+ "/Lepton_bdt_id_Full5x5_sigmaIetaIeta",  bdt_id_Full5x5_sigmaIetaIeta, w, 200 , 0 , 0.04);
  FillHist( cut+ "/Lepton_bdt_id_dEtaSeed",  bdt_id_dEtaSeed, w, 200, 0., 0.05);
  FillHist( cut+ "/Lepton_bdt_id_dPhiIn",    bdt_id_dPhiIn, w, 200, 0. , 0.2);
  FillHist( cut+ "/Lepton_bdt_id_dEtaIn",  bdt_id_dEtaIn, w, 200, 0., 0.1);
  FillHist( cut+ "/Lepton_bdt_id_EtaWidth",  bdt_id_EtaWidth, w, 200 , 0 , 1);
  FillHist( cut+ "/Lepton_bdt_id_PhiWidth",  bdt_id_PhiWidth, w, 400, 0, 2);
  FillHist( cut+ "/Lepton_bdt_id_HoverE "  , bdt_id_HoverE, w, 150 , 0 , 1.5);
  FillHist( cut+ "/Lepton_bdt_id_TrkIso",  bdt_id_TrkIso, w, 200, 0, 5);
  FillHist( cut+ "/Lepton_bdt_id_e2x5OverE5x5",  bdt_id_e2x5OverE5x5, w, 220, 0, 1.1);
  FillHist( cut+ "/Lepton_bdt_id_e1x5OverE5x5",  bdt_id_e1x5OverE5x5, w, 350, -1, 2.5);
  FillHist( cut+ "/Lepton_bdt_id_e55",  bdt_id_e55, w, 400, 0 , 8000);
  FillHist( cut+ "/Lepton_bdt_id_dr03TkSumPt",  bdt_id_dr03TkSumPt, w,  250, 0 , 0.25);
  FillHist( cut+ "/Lepton_bdt_id_InvEminusInvP",  bdt_id_InvEminusInvP, w, 200, 0 ,2);
  FillHist( cut+ "/Lepton_bdt_id_ecalPFClusterIso",  bdt_id_ecalPFClusterIso, w,200,0 , 0.5);
  FillHist( cut+ "/Lepton_bdt_id_hcalPFClusterIso",  bdt_id_hcalPFClusterIso, w,200,0 , 0.5);

  // IDs    
  FillHist( cut+ "/Lepton_bdt_id_EoverP",  bdt_id_EoverP, w, 200, -10 ,10);
  FillHist( cut+ "/Lepton_bdt_id_FBrem",  bdt_id_FBrem, w,  200, -2 , 2);
  FillHist( cut+ "/Lepton_bdt_id_Rho",  bdt_id_Rho, w,  200, 0 , 20);
  FillHist( cut+ "/Lepton_bdt_id_IsGsfCtfScPixChargeConsistent",  bdt_id_IsGsfCtfScPixChargeConsistent , w, 3 ,0 ,3);
  FillHist( cut+ "/Lepton_bdt_id_IsGsfScPixChargeConsistent",  bdt_id_IsGsfScPixChargeConsistent , w, 3 ,0 ,3);
  FillHist( cut+ "/Lepton_bdt_id_IsGsfCtfChargeConsistent",  bdt_id_IsGsfCtfChargeConsistent , w,3 ,0 ,3);
  FillHist( cut+ "/Lepton_bdt_id_R9",  bdt_id_R9, w, 200, 0., 5);
  FillHist( cut+ "/Lepton_bdt_id_PSEoverRawE", bdt_id_psEoRraw, w, 1000, 0., 1);

  return;

}





void AnalyzerCore::PrintBDTInput(){


  cout << "bdt_id_Pt " << bdt_id_Pt<< endl;
  cout << "bdt_id_PtBinned " << bdt_id_PtBinned<< endl;
  cout << "bdt_id_Eta " << bdt_id_Eta<< endl;
  cout << "bdt_id_PtRatio " << bdt_id_PtRatio<< endl;
  cout << "bdt_id_PtRel " << bdt_id_PtRel<< endl;
  cout << "bdt_id_PileUp " << bdt_id_PileUp << endl;

  cout << "bdt_id_MiniIsoChHad " << bdt_id_MiniIsoChHad<< endl;;
  cout << "bdt_id_MiniIsoNHad " << bdt_id_MiniIsoNHad<< endl;;
  cout << "bdt_id_MiniIsoPhHad " << bdt_id_MiniIsoPhHad<< endl;;

  cout << "bdt_id_IsoChHad " << bdt_id_IsoChHad<< endl;;
  cout << "bdt_id_IsoNHad " << bdt_id_IsoNHad<< endl;;
  cout << "bdt_id_IsoPhHad" << bdt_id_IsoPhHad<< endl;;
  cout << "bdt_id_Minireliso " << bdt_id_Minireliso<< endl;
  cout << "bdt_id_RelMiniIsoCh " << bdt_id_RelMiniIsoCh<< endl;
  cout << "bdt_id_RelMiniIsoN " << bdt_id_RelMiniIsoN<< endl;
  cout << "bdt_id_RelIso " << bdt_id_RelIso<< endl;;

  cout << "bdt_id_CEMFracCJ " << bdt_id_CEMFracCJ<< endl;
  cout << "bdt_id_NEMFracCJ " << bdt_id_NEMFracCJ<< endl;
  cout << "bdt_id_CHFracCJ " << bdt_id_CHFracCJ<< endl;
  cout << "bdt_id_NHFracCJ " << bdt_id_NHFracCJ<< endl;
  cout << "bdt_id_MuFracCJ " << bdt_id_MuFracCJ<< endl;
  cout << "bdt_id_JetDiscCJ " << bdt_id_JetDiscCJ<< endl;

  /// IP   
  cout << "bdt_id_Dxy " << bdt_id_Dxy<< endl;
  cout << "bdt_id_DxySig " << bdt_id_DxySig<< endl;
  cout << "bdt_id_Dz" << bdt_id_Dz<< endl;
  cout << "bdt_id_DzSig " << bdt_id_DzSig<< endl;
  cout << "bdt_id_IP3D " << bdt_id_IP3D<< endl;
  cout << "bdt_id_MVA " << bdt_id_MVA<< endl;
  cout << "bdt_id_MVAIso " << bdt_id_MVAIso<< endl;
  cout << "bdt_id_MissingHits " << bdt_id_MissingHits<< endl;

  // El Shower/SCL 
  cout << "bdt_id_Full5x5_sigmaIetaIeta " << bdt_id_Full5x5_sigmaIetaIeta<< endl;
  cout << "bdt_id_dEtaSeed " << bdt_id_dEtaSeed<< endl;
  cout << "bdt_id_dPhiIn " << bdt_id_dPhiIn<< endl;
  cout << "bdt_id_dEtaIn " << bdt_id_dEtaIn<< endl;
  cout << "bdt_id_EtaWidth " << bdt_id_EtaWidth<< endl;
  cout << "bdt_id_PhiWidth " << bdt_id_PhiWidth<< endl;
  cout << "bdt_id_HoverE "  << bdt_id_HoverE<< endl;
  cout << "bdt_id_TrkIso " << bdt_id_TrkIso<< endl;
  cout << "bdt_id_e2x5OverE5x5 " << bdt_id_e2x5OverE5x5<< endl;;
  cout << "bdt_id_e1x5OverE5x5 " << bdt_id_e1x5OverE5x5<< endl;;
  cout << "bdt_id_e55 " << bdt_id_e55<< endl;;
  cout << "bdt_id_dr03HcalTowerSumEt " << bdt_id_dr03HcalTowerSumEt<< endl;;
  cout << "bdt_id_dr03TkSumPt " << bdt_id_dr03TkSumPt<< endl;;
  cout << "bdt_id_InvEminusInvP " << bdt_id_InvEminusInvP<< endl;
  cout << "bdt_id_ecalPFClusterIso " << bdt_id_ecalPFClusterIso<< endl;
  cout << "bdt_id_hcalPFClusterIso " << bdt_id_hcalPFClusterIso<< endl;

  // IDs   
  cout << "bdt_id_isEcalDriven " << bdt_id_isEcalDriven << endl;
  cout << "bdt_id_EoverP " << bdt_id_EoverP<< endl;;
  cout << "bdt_id_FBrem " << bdt_id_FBrem<< endl;;
  cout << "bdt_id_PassConversionVeto " << bdt_id_PassConversionVeto<< endl; ;
  cout << "bdt_id_IsGsfCtfScPixChargeConsistent " << bdt_id_IsGsfCtfScPixChargeConsistent << endl;;
  cout << "bdt_id_IsGsfScPixChargeConsistent " << bdt_id_IsGsfScPixChargeConsistent << endl;;
  cout << "bdt_id_IsGsfCtfChargeConsistent " << bdt_id_IsGsfCtfChargeConsistent << endl;;
  cout << "bdt_id_R9 " << bdt_id_R9<< endl;;
  cout << "bdt_id_psEoRraw " << bdt_id_psEoRraw << endl;

  return;

}



double AnalyzerCore::GetBDTScoreMuon(Muon mu ,BkgType bkg, TString BDTTag){
  
  TString MVATagStr = BDTTag;

  /// return def val if iSetupLeptonBDTv is not set
  if(MVATagStr.Contains("v4")) {
    if(!iSetupLeptonBDTv4) return -1;
  }
  if(MVATagStr.Contains("v5")) {
    if(!iSetupLeptonBDTv5) return -1;
  }

  InitializeIDTreeVars();

  Lepton *lep = (Lepton *)(&mu);
  SetBDTIDVar(lep);
  SetBDTIDVariablesMuon(mu);

  if (bkg == BkgType::Fake) MVATagStr += "_Fake";
  if (bkg == BkgType::FakeRate) MVATagStr += "_Fake";

  //// Version 4 is NoPt Reader  
  if(iSetupLeptonBDTv4){
    if(MVATagStr.Contains("v4"))  return  MuonIDv4_FakeMVAReader->EvaluateMVA(MVATagStr);
  }
  if(iSetupLeptonBDTv5){

    if(MVATagStr.Contains("v5"))  return  MuonIDv5_FakeMVAReader->EvaluateMVA(MVATagStr);
  }

  return -1;
}

void AnalyzerCore::SetBDTIDVarV1(Lepton*  lep){

  bdt_id_Pt    = (lep->Pt() > 500)  ? 499 : lep->Pt();
  bdt_id_Eta   = fabs(lep->Eta());
  bdt_id_MiniIsoChHad = lep->MiniIsoChHad();
  bdt_id_MiniIsoNHad = lep->MiniIsoNHad();
  bdt_id_MiniIsoPhHad = lep->MiniIsoPhHad();
  bdt_id_IsoChHad = lep->IsoChHad();
  bdt_id_IsoNHad = lep->IsoNHad();
  bdt_id_IsoPhHad = lep->IsoPhHad();
  bdt_id_Dxy   = lep->LogdXY();
  bdt_id_DxySig   = lep->LogdXYSig();
  bdt_id_Dz   = lep->LogdZ();
  bdt_id_DzSig   = lep->LogdZSig();
  bdt_id_IP3D    = lep->SIP3D();
  bdt_id_RelIso    =  lep->RelIso();
  bdt_id_Minireliso   = lep->MiniRelIso();
  bdt_id_RelMiniIsoCh = lep->MiniRelIsoCharged();
  bdt_id_RelMiniIsoN = lep->MiniRelIsoNeutral();

  //==== Vars for non-prompt lepton bkg                                                                      

  int IdxMatchJet=-1;
  float mindR1=999.;
  std::vector<Jet>   JetAllColl = GetJets("NoID", 10., 5.0);

  for(unsigned int ij=0; ij<JetAllColl.size(); ij++){
    float dR1=lep->DeltaR(JetAllColl.at(ij));
    if(dR1>0.4) continue;
    if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
  }

  if(IdxMatchJet!=-1){
    bdt_id_CEMFracCJ = JetAllColl.at(IdxMatchJet).ChargedEmEnergyFraction();
    bdt_id_NEMFracCJ = JetAllColl.at(IdxMatchJet).NeutralEmEnergyFraction();
    bdt_id_CHFracCJ  = JetAllColl.at(IdxMatchJet).ChargedHadEnergyFraction();
    bdt_id_NHFracCJ  = JetAllColl.at(IdxMatchJet).NeutralHadEnergyFraction();
    bdt_id_MuFracCJ  = JetAllColl.at(IdxMatchJet).MuonEnergyFraction();
    bdt_id_JetDiscCJ = JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet);
  }

  else{
    bdt_id_CEMFracCJ=0, bdt_id_NEMFracCJ=0., bdt_id_CHFracCJ=0., bdt_id_NHFracCJ=0., bdt_id_MuFracCJ=0., bdt_id_JetDiscCJ=0.;
  }

  return;
}


void AnalyzerCore::SetBDTIDVarOLD(Lepton*  lep){

  bdt_id_PileUp = nPV;

  bdt_id_Pt    = (lep->Pt() > 500)  ? 499 : lep->Pt();

  if (lep->Pt() < 50.) bdt_id_PtBinned= 0;
  else     if (lep->Pt() < 100.) bdt_id_PtBinned= 1;
  else     if (lep->Pt() < 200.) bdt_id_PtBinned= 2;
  else bdt_id_PtBinned =3;

  bdt_id_Eta   = fabs(lep->Eta());

  if (lep->MiniIsoChHad() > 150) bdt_id_MiniIsoChHad = 150;
  else   bdt_id_MiniIsoChHad = lep->MiniIsoChHad();

  if (lep->MiniIsoNHad() > 150) bdt_id_MiniIsoNHad = 150.;
  else   bdt_id_MiniIsoNHad = lep->MiniIsoNHad();

  if(lep->MiniIsoPhHad() > 150.) bdt_id_MiniIsoPhHad = 150.;
  else bdt_id_MiniIsoPhHad = lep->MiniIsoPhHad();

  if(lep->IsoChHad() > 150) bdt_id_IsoChHad = 150;
  else bdt_id_IsoChHad = lep->IsoChHad();
  if(lep->IsoNHad()  > 150)  bdt_id_IsoNHad  = 150;
  else   bdt_id_IsoNHad = lep->IsoNHad();
  if(lep->IsoPhHad() > 150) bdt_id_IsoPhHad  = 150;
  else bdt_id_IsoPhHad = lep->IsoPhHad();

  bdt_id_Dxy      = lep->LogdXY();
  bdt_id_RelDxy   = lep->LogdXY()/lep->Pt();
  bdt_id_DxySig   = lep->LogdXYSig();
  bdt_id_Dz       = lep->LogdZ();
  bdt_id_RelDz    = lep->LogdZ()/lep->Pt();
  bdt_id_DzSig    = lep->LogdZSig();
  bdt_id_IP3D     = lep->SIP3D();
  bdt_id_RelIP3D    = lep->SIP3D()/lep->Pt();
  if (lep->RelIso() > 1)  bdt_id_RelIso = 1;
  else  bdt_id_RelIso     =  lep->RelIso();

  bdt_id_Minireliso   = lep->MiniRelIso();
  bdt_id_RelMiniIsoCh = lep->MiniRelIsoCharged();
  bdt_id_RelMiniIsoN = lep->MiniRelIsoNeutral();

  //==== Vars for non-prompt lepton bkg 

  int IdxMatchJet=-1;
  float mindR1=999.;
  std::vector<Jet>   JetAllColl = GetAllJets();

  for(unsigned int ij=0; ij<JetAllColl.size(); ij++){
    float dR1=lep->DeltaR(JetAllColl.at(ij));
    if(dR1>0.4) continue;
    if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
  }

  if(IdxMatchJet!=-1){

    bdt_id_CEMFracCJ = JetAllColl.at(IdxMatchJet).ChargedEmEnergyFraction();
    bdt_id_NEMFracCJ = JetAllColl.at(IdxMatchJet).NeutralEmEnergyFraction();
    bdt_id_CHFracCJ  = JetAllColl.at(IdxMatchJet).ChargedHadEnergyFraction();
    bdt_id_NHFracCJ  = JetAllColl.at(IdxMatchJet).NeutralHadEnergyFraction();
    bdt_id_MuFracCJ  = JetAllColl.at(IdxMatchJet).MuonEnergyFraction();
    bdt_id_JetDiscCJ = JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet);
  }

  else{
    bdt_id_CEMFracCJ=1.5, bdt_id_NEMFracCJ=1.5, bdt_id_CHFracCJ=1.5, bdt_id_NHFracCJ=1.5, bdt_id_MuFracCJ=1.5, bdt_id_JetDiscCJ=1.5;
  }

  return;
}





void AnalyzerCore::SetBDTIDVar(Lepton*  lep){

  bdt_id_PileUp = nPV;

  bdt_id_Pt    = (lep->Pt() > 500)  ? 499 : lep->Pt();


  if (lep->Pt() < 50.) bdt_id_PtBinned= 0;
  else     if (lep->Pt() < 100.) bdt_id_PtBinned= 1;
  else     if (lep->Pt() < 200.) bdt_id_PtBinned= 2;
  else bdt_id_PtBinned =3;

  if (lep->Pt() < 20.) bdt_id_PtBinned2= 0;
  else     if (lep->Pt() < 30.) bdt_id_PtBinned2= 1;
  else     if (lep->Pt() < 50.) bdt_id_PtBinned2= 2;
  else     if (lep->Pt() < 60.) bdt_id_PtBinned2= 3;
  else     if (lep->Pt() < 70.) bdt_id_PtBinned2= 4;
  else     if (lep->Pt() < 80.) bdt_id_PtBinned2= 5;
  else     if (lep->Pt() < 90.) bdt_id_PtBinned2= 6;
  else     if (lep->Pt() < 100.) bdt_id_PtBinned2= 7;
  else     if (lep->Pt() < 150.) bdt_id_PtBinned2= 8;
  else     if (lep->Pt() < 200.) bdt_id_PtBinned2= 9;
  else bdt_id_PtBinned2 =10;

  bdt_id_Eta   = fabs(lep->Eta());

  /////// ISOLATION    
  /// MINI ISO         
  if (lep->MiniIsoChHad() > 20) bdt_id_MiniIsoChHad = 20;
  else   bdt_id_MiniIsoChHad = lep->MiniIsoChHad();
  if (lep->MiniIsoNHad() > 20) bdt_id_MiniIsoNHad = 20.;
  else   bdt_id_MiniIsoNHad = lep->MiniIsoNHad();
  if(lep->MiniIsoPhHad() > 20.) bdt_id_MiniIsoPhHad = 20.;
  else bdt_id_MiniIsoPhHad = lep->MiniIsoPhHad();

  //// pfIsolationVariables 
  if(lep->IsoChHad() > 20) bdt_id_IsoChHad = 20;
  else bdt_id_IsoChHad = lep->IsoChHad();
  if(lep->IsoNHad()  > 20)  bdt_id_IsoNHad  = 20;
  else   bdt_id_IsoNHad = lep->IsoNHad();
  if(lep->IsoPhHad() > 20) bdt_id_IsoPhHad  = 20;
  else bdt_id_IsoPhHad = lep->IsoPhHad();

  bdt_id_Dxy      = lep->LogdXY();
  bdt_id_DxySig   = lep->LogdXYSig();
  bdt_id_Dz       = lep->LogdZ();
  bdt_id_DzSig    = lep->LogdZSig();
  bdt_id_IP3D     = lep->SIP3D();


  if (lep->RelIso() > 1)  bdt_id_RelIso = 1;
  else  bdt_id_RelIso     =  lep->RelIso();

  bdt_id_Minireliso   = lep->MiniRelIso();
  bdt_id_RelMiniIsoCh = lep->MiniRelIsoCharged();
  bdt_id_RelMiniIsoN  = lep->MiniRelIsoNeutral();

  //==== Vars for non-prompt lepton bkg  
  int IdxMatchJet=-1;
  float mindR1=999.;
  std::vector<Jet>   JetAllColl = GetAllJets();

  for(unsigned int ij=0; ij<JetAllColl.size(); ij++){
    float dR1=lep->DeltaR(JetAllColl.at(ij));
    if(dR1>0.4) continue;
    if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
  }

  if(IdxMatchJet!=-1){

    bdt_id_CEMFracCJ = JetAllColl.at(IdxMatchJet).ChargedEmEnergyFraction();
    bdt_id_NEMFracCJ = JetAllColl.at(IdxMatchJet).NeutralEmEnergyFraction();
    bdt_id_CHFracCJ  = JetAllColl.at(IdxMatchJet).ChargedHadEnergyFraction();
    bdt_id_NHFracCJ  = JetAllColl.at(IdxMatchJet).NeutralHadEnergyFraction();
    bdt_id_MuFracCJ  = JetAllColl.at(IdxMatchJet).MuonEnergyFraction();
    bdt_id_JetDiscCJ = JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet);
    bdt_id_JetDiscCJCvsB = JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet_CvsB);
    bdt_id_JetDiscCJCvsL = JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet_CvsL);
  }

  else{
    bdt_id_CEMFracCJ=1., bdt_id_NEMFracCJ=0., bdt_id_CHFracCJ=0., bdt_id_NHFracCJ=0, bdt_id_MuFracCJ=1.5, bdt_id_JetDiscCJ=0.1;
    bdt_id_JetDiscCJCvsB=0.4;
    bdt_id_JetDiscCJCvsL=0.1;

  }

  return;
}



void AnalyzerCore::SetupIDMVAReaderDefault(bool ForceSetupV4, bool ForceSetupV5){

  //  bool SkimContainsVersion4 = (fChain->GetBranch("electron_mva_fake_ed_v4"));
  bool SkimContainsVersion5 = (fChain->GetBranch("electron_mva_fake_ed_v5"));

  bool SetupVersion4 = ForceSetupV4;
  bool SetupVersion5 = (!SkimContainsVersion5 || ForceSetupV5);

  if(iSetupLeptonBDTv4) SetupVersion4=false;
  if(iSetupLeptonBDTv5) SetupVersion5=false;


  SetupLeptonBDT(SetupVersion4,SetupVersion5); /// This initialises MVAReaders and sets iSetupLeptonBDTvX bools   

  if(SetupVersion4){
    // Muon ID Setup  Books weight files   
    if(fChain->GetBranch("muon_mva_fake_v4")){
      cout << "WARNING SetupIDMVAReaderDefault set tp force V4 setup when running on a skimmed sample with V4 variables in ntup" << endl;
    }
    if(fChain->GetBranch("electron_mva_fake_ed_v4")) {
      cout << "WARNING SetupIDMVAReaderDefault set tp force V4 setup when running on a skimmed sample with V4 variables in ntup" << endl;
    }
    SetupIDMVAReaderElectron();
  }

  if(SetupVersion5){

    if(fChain->GetBranch("electron_mva_fake_ed_v5"))         cout << "WARNING SetupIDMVAReaderDefault set tp force V5 setup when running on a skimmed sample with V4 variables in ntup" << endl;
    SetupIDMVAReaderElectronUpdate();
  }

  if(SetupVersion5) SetupIDMVAReaderMuon();
  return ;
}





void AnalyzerCore::SetupIDMVAReaderMuon(){

  if(! ( iSetupLeptonBDTv4 || iSetupLeptonBDTv5)) return;

  InitializeIDTreeVars();

  //// IF branch for var exists in ntuple by default dont setup reader  
  // Fake                                                               
  if(iSetupLeptonBDTv5){
    MuonIDv5_FakeMVAReader->AddVariable("Eta", &bdt_id_Eta);
    MuonIDv5_FakeMVAReader->AddVariable("MiniIsoChHad", &bdt_id_MiniIsoChHad);
    MuonIDv5_FakeMVAReader->AddVariable("MiniIsoPhHad",&bdt_id_MiniIsoPhHad);
    MuonIDv5_FakeMVAReader->AddVariable("MiniIsoNHad",&bdt_id_MiniIsoNHad);
    MuonIDv5_FakeMVAReader->AddVariable("RelMiniIsoCh", &bdt_id_RelMiniIsoCh);
    MuonIDv5_FakeMVAReader->AddVariable("RelMiniIsoN",&bdt_id_RelMiniIsoN);
    MuonIDv5_FakeMVAReader->AddVariable("RelIso",&bdt_id_RelIso);
    MuonIDv5_FakeMVAReader->AddVariable("Dxy",  &bdt_id_Dxy);
    MuonIDv5_FakeMVAReader->AddVariable("DxySig",  &bdt_id_DxySig);
    MuonIDv5_FakeMVAReader->AddVariable("Dz",  &bdt_id_Dz);
    MuonIDv5_FakeMVAReader->AddVariable("DzSig",  &bdt_id_DzSig);
    MuonIDv5_FakeMVAReader->AddVariable("IP3D", &bdt_id_IP3D);
    MuonIDv5_FakeMVAReader->AddVariable("PtRatio",  &bdt_id_PtRatio);
    MuonIDv5_FakeMVAReader->AddVariable("PtRel",  &bdt_id_PtRel);
    MuonIDv5_FakeMVAReader->AddVariable("JetDiscCJ"    ,&bdt_id_JetDiscCJ);
    MuonIDv5_FakeMVAReader->AddVariable("JetDiscCJCvsB",&bdt_id_JetDiscCJCvsB);
    MuonIDv5_FakeMVAReader->AddVariable("JetDiscCJCvsL",&bdt_id_JetDiscCJCvsL);
    MuonIDv5_FakeMVAReader->AddVariable("NEMFracCJ",&bdt_id_NEMFracCJ);
    MuonIDv5_FakeMVAReader->AddVariable("CHFracCJ",&bdt_id_CHFracCJ);
    MuonIDv5_FakeMVAReader->AddVariable("NHFracCJ",&bdt_id_NHFracCJ);
    MuonIDv5_FakeMVAReader->AddVariable("MuFracCJ",&bdt_id_MuFracCJ);
    MuonIDv5_FakeMVAReader->AddVariable("MVA",  &bdt_id_MVA);
    MuonIDv5_FakeMVAReader->AddVariable("Chi2",  &bdt_id_Chi2);
    MuonIDv5_FakeMVAReader->AddVariable("Validhits",  &bdt_id_Validhits);
    MuonIDv5_FakeMVAReader->AddVariable("Matched_stations",  &bdt_id_Matched_stations);
    MuonIDv5_FakeMVAReader->AddSpectator("w_id_tot", &bdt_id_w_id_tot);
  }

  if(iSetupLeptonBDTv4){
    MuonIDv4_FakeMVAReader->AddVariable("Eta", &bdt_id_Eta);
    MuonIDv4_FakeMVAReader->AddVariable("MiniIsoChHad", &bdt_id_MiniIsoChHad);
    MuonIDv4_FakeMVAReader->AddVariable("MiniIsoPhHad",&bdt_id_MiniIsoPhHad);
    MuonIDv4_FakeMVAReader->AddVariable("MiniIsoNHad",&bdt_id_MiniIsoNHad);
    MuonIDv4_FakeMVAReader->AddVariable("RelMiniIsoCh", &bdt_id_RelMiniIsoCh);
    MuonIDv4_FakeMVAReader->AddVariable("RelMiniIsoN",&bdt_id_RelMiniIsoN);
    MuonIDv4_FakeMVAReader->AddVariable("Dxy",  &bdt_id_Dxy);
    MuonIDv4_FakeMVAReader->AddVariable("DxySig",  &bdt_id_DxySig);
    MuonIDv4_FakeMVAReader->AddVariable("Dz",  &bdt_id_Dz);
    MuonIDv4_FakeMVAReader->AddVariable("DzSig",  &bdt_id_DzSig);
    MuonIDv4_FakeMVAReader->AddVariable("RelIso", &bdt_id_RelIso);
    MuonIDv4_FakeMVAReader->AddVariable("IP3D", &bdt_id_IP3D);
    MuonIDv4_FakeMVAReader->AddVariable("PtRatio",  &bdt_id_PtRatio);
    MuonIDv4_FakeMVAReader->AddVariable("PtRel",  &bdt_id_PtRel);
    MuonIDv4_FakeMVAReader->AddVariable("NEMFracCJ",&bdt_id_NEMFracCJ);
    MuonIDv4_FakeMVAReader->AddVariable("CHFracCJ",&bdt_id_CHFracCJ);
    MuonIDv4_FakeMVAReader->AddVariable("JetDiscCJ",&bdt_id_JetDiscCJ);
    MuonIDv4_FakeMVAReader->AddVariable("NHFracCJ",&bdt_id_NHFracCJ);
    MuonIDv4_FakeMVAReader->AddVariable("MuFracCJ",&bdt_id_MuFracCJ);
    MuonIDv4_FakeMVAReader->AddVariable("MVA",  &bdt_id_MVA);
    MuonIDv4_FakeMVAReader->AddVariable("Chi2",  &bdt_id_Chi2);
    MuonIDv4_FakeMVAReader->AddVariable("Validhits",  &bdt_id_Validhits);
    MuonIDv4_FakeMVAReader->AddVariable("Matched_stations",  &bdt_id_Matched_stations);
    MuonIDv4_FakeMVAReader->AddSpectator("w_id_tot", &bdt_id_w_id_tot);
  }



  TString AnalyzerPath=std::getenv("SKFlat_WD");

  TString MVAPathMuonFakeV4="/gv0/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeIMuonFake/Version6/"+GetYearString()+"/dataset/weights/";
  TString MVAPathMuonFakeV5="/gv0/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeIMuonFake/Version12/"+GetYearString()+"/dataset/weights/";

  TString xmlpf = "_TMVAClassification_BDTG.weights.xml";


  TString BDTGv4_Fake = "";
  if(GetYear() == 2016) BDTGv4_Fake = "BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPt_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf;
  if(GetYear() == 2017) BDTGv4_Fake = "BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPt_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf;
  if(GetYear() == 2018) BDTGv4_Fake = "BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPt_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf;

  //----> 0.988444335798: ['0.135', '0.129', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPt_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
  //----> 0.983863690644: ['0.72', '0.151', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPt_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
  //----> 0.987515887664: ['0.805', '0.28', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPt_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 

  TString BDTGv4_BB_Fake = "";
  if(GetYear() == 2016) BDTGv4_BB_Fake = "BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtBB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf;
  if(GetYear() == 2017) BDTGv4_BB_Fake = "BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtBB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf;
  if(GetYear() == 2018) BDTGv4_BB_Fake = "BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtBB_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+xmlpf;

  TString BDTGv4_EC_Fake = "";
  if(GetYear() == 2016) BDTGv4_EC_Fake = "BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtEC_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf;
  if(GetYear() == 2017) BDTGv4_EC_Fake = "BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtEC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+xmlpf;
  if(GetYear() == 2018) BDTGv4_EC_Fake = "BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtEC_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT"+xmlpf;




  if(iSetupLeptonBDTv4){
    MuonIDv4_FakeMVAReader->BookMVA("BDTGv4_Fake",    MVAPathMuonFakeV4+BDTGv4_Fake);
    MuonIDv4_FakeMVAReader->BookMVA("BDTGv4_BB_Fake", MVAPathMuonFakeV4+BDTGv4_BB_Fake);
    MuonIDv4_FakeMVAReader->BookMVA("BDTGv4_EC_Fake", MVAPathMuonFakeV4+BDTGv4_EC_Fake);
  }
  // ----> 0.990249335302: ['0.478', '0.195', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtBB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
  // ----> 0.983556816291: ['0.167', '0.551', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtEC_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
  //----> 0.988444335798: ['0.135', '0.129', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPt_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
  //----> 0.988732999523: ['0.348', '0.105', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtBB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
  // ----> 0.981577613688: ['0.479', '0.184', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtEC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
  //----> 0.983863690644: ['0.72', '0.151', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPt_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
  // ----> 0.98889260451: ['0.742', '0.172', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtBB_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
  // ----> 0.982613570019: ['0.639', '0.217', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPtEC_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT.root'] 
  //----> 0.987515887664: ['0.805', '0.28', 'BDTG_version6_MuonFakeBkg_LF_TypeI_MuMu_SignalMuonFakeNoPt_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 

  if(iSetupLeptonBDTv5){

    if(GetYear() == 2016){
      //MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_LFvsHF_Fake",                 MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_LFvsHF_2016_NTrees300_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);                 
      //MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_HFBvsHFC_Fake",     MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_HFBvsHFC_2016_NTrees300_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);                   
      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_LFvsHF_Fake",
                                      MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFvsHF_2016_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);

      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_HFBvsHFC_Fake",
                                      MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_HFBvsHFC_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);

      //----> 0.812412822248: ['0.81', '0.118', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_HFBvsHFC_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
      //----> 0.857725962924: ['0.131', '0.191', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_HFBvsHFC_2016_NTrees300_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
      //----> 0.909569086244: ['0.121', '0.299', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFvsHF_2016_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
      //----> 0.954268329212: ['0.112', '0.293', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_LFvsHF_2016_NTrees300_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT.root']      

      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_LF1_Fake",MVAPathMuonFakeV5 +"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother1_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf);
      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_LF2_Fake",MVAPathMuonFakeV5 +"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother2_2016_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_300_BDT"+ xmlpf);


      //----> 0.854619468881: ['0.12', '0.443', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother1_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT.root']  
      //----> 0.82287088718: ['0.254', '0.115', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother2_2016_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_300_BDT.root'] 
    }


    if(GetYear() == 2017){
      ///MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_LFvsHF_Fake",                        MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_LFvsHF_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
      ///MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_HFBvsHFC_Fake",     MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_HFBvsHFC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);              
      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_LFvsHF_Fake",
                                      MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFvsHF_2017_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);

      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_HFBvsHFC_Fake",
                                      MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_HFBvsHFC_2017_NTrees300_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);

      //----> 0.819748332215: ['0.386', '0.102', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_HFBvsHFC_2017_NTrees300_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT.root']       
      //----> 0.872141270213: ['0.436', '0.11', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_HFBvsHFC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root']
      //----> 0.918010546249: ['0.233', '0.39', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFvsHF_2017_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
      //----> 0.956586987068: ['0.172', '0.135', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_LFvsHF_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT.root'] 
      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_LF1_Fake",MVAPathMuonFakeV5 +"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother1_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_100_BDT"+ xmlpf);
      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_LF2_Fake",MVAPathMuonFakeV5 +"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother2_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_100_BDT"+ xmlpf);


      //----> 0.860615816119: ['0.139', '0.885', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother1_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_100_BDT.root']                             
      //----> 0.824113180693: ['0.174', '0.107', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother2_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_100_BDT.root']                            
    }

    if(GetYear() == 2018){
      //MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_LFvsHF_Fake",        MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_LFvsHF_2018_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);                           
      //MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_HFBvsHFC_Fake",      MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_HFBvsHFC_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);                 
      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_LFvsHF_Fake",    MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFvsHF_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_HFBvsHFC_Fake",  MVAPathMuonFakeV5+"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_HFBvsHFC_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);

      //----> 0.823428671706: ['0.283', '0.125', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_HFBvsHFC_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root']         
      //----> 0.874450108543: ['0.813', '0.104', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_HFBvsHFC_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_100_BDT.root'] 
      //----> 0.920109543703: ['0.106', '0.101', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFvsHF_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
      //----> 0.956505115356: ['0.106', '0.673', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_LFvsHF_2018_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_LF1_Fake",MVAPathMuonFakeV5 +"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother1_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+ xmlpf);
      MuonIDv5_FakeMVAReader->BookMVA("BDTGv5_LF2_Fake",MVAPathMuonFakeV5 +"BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother2_2018_NTrees700_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+ xmlpf);


      //----> 0.866496726781: ['0.161', '0.263', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother1_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
      //----> 0.817553405836: ['0.176', '0.141', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother2_2018_NTrees700_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
      //----> 0.719608872018: ['0.112', '0.147', 'BDTG_version12_MuonFakeBkg_TypeI_MuMu_SignalMuonFake_QCD_LFMother3_2018_NTrees300_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    }
  }

  return;

}




void AnalyzerCore::SetupIDMVAReaderElectronUpdate(){

  InitializeIDTreeVars();


  ////// Version5 has merged to have just one reader for Fake/Conv                                      
  /////// Barrel                                                                                      
  ElectronIDv5_MVAReader->AddVariable("Pt",           &bdt_id_Pt);
  ElectronIDv5_MVAReader->AddVariable("Eta",          &bdt_id_Eta);
  ElectronIDv5_MVAReader->AddVariable("MiniIsoChHad", &bdt_id_MiniIsoChHad);
  ElectronIDv5_MVAReader->AddVariable("MiniIsoPhHad", &bdt_id_MiniIsoPhHad);
  ElectronIDv5_MVAReader->AddVariable("MiniIsoNHad",  &bdt_id_MiniIsoNHad);
  ElectronIDv5_MVAReader->AddVariable("RelMiniIsoCh", &bdt_id_RelMiniIsoCh);
  ElectronIDv5_MVAReader->AddVariable("RelMiniIsoN",  &bdt_id_RelMiniIsoN);
  ElectronIDv5_MVAReader->AddVariable("RelIso",       &bdt_id_RelIso);
  if(DataYear == 2016){
    ElectronIDv5_MVAReader->AddVariable("Dxy",        &bdt_id_Dxy);
    ElectronIDv5_MVAReader->AddVariable("Dz",         &bdt_id_Dz);
  }
  else{
    ElectronIDv5_MVAReader->AddVariable("DxySig",     &bdt_id_DxySig);
    ElectronIDv5_MVAReader->AddVariable("DzSig",     &bdt_id_DzSig);
  }
  ElectronIDv5_MVAReader->AddVariable("IP3D",  &bdt_id_IP3D);
  ElectronIDv5_MVAReader->AddVariable("PtRatioV3",    &bdt_id_PtRatioV3);
  ElectronIDv5_MVAReader->AddVariable("PtRelV2",      &bdt_id_PtRelV2);
  ElectronIDv5_MVAReader->AddVariable("JetDiscCJ"    ,&bdt_id_JetDiscCJ);
  ElectronIDv5_MVAReader->AddVariable("JetDiscCJCvsB",&bdt_id_JetDiscCJCvsB);
  ElectronIDv5_MVAReader->AddVariable("JetDiscCJCvsL",&bdt_id_JetDiscCJCvsL);

  ElectronIDv5_MVAReader->AddVariable("MVA",          &bdt_id_MVA);
  ElectronIDv5_MVAReader->AddVariable("MVAIso",       &bdt_id_MVAIso);
  ElectronIDv5_MVAReader->AddVariable("dPhiIn",       &bdt_id_dPhiIn);
  ElectronIDv5_MVAReader->AddVariable("EoverP",       &bdt_id_EoverP);
  ElectronIDv5_MVAReader->AddVariable("FBrem",        &bdt_id_FBrem);
  ElectronIDv5_MVAReader->AddVariable("R9" ,          &bdt_id_R9);
  ElectronIDv5_MVAReader->AddVariable("e55",          &bdt_id_e55);
  ElectronIDv5_MVAReader->AddVariable("PhiWidth",     &bdt_id_PhiWidth);
  ElectronIDv5_MVAReader->AddVariable("EtaWidth",     &bdt_id_EtaWidth);
  ElectronIDv5_MVAReader->AddVariable("MissingHits",  &bdt_id_MissingHits);
  ElectronIDv5_MVAReader->AddVariable("PassConversionVeto",  &bdt_id_PassConversionVeto);
  ElectronIDv5_MVAReader->AddVariable("IsGsfCtfScPixChargeConsistent",  &bdt_id_IsGsfCtfScPixChargeConsistent);
  ElectronIDv5_MVAReader->AddVariable("IsGsfScPixChargeConsistent",  &bdt_id_IsGsfScPixChargeConsistent);
  ElectronIDv5_MVAReader->AddVariable("IsGsfCtfChargeConsistent",  &bdt_id_IsGsfCtfChargeConsistent);
  ElectronIDv5_MVAReader->AddVariable("InvEminusInvP",  &bdt_id_InvEminusInvP);
  ElectronIDv5_MVAReader->AddSpectator("w_id_tot", &bdt_id_w_id_tot);


  /////// Barrel                                                                                           
  ElectronIDv5_CFMVAReader->AddVariable("PtBinned2",    &bdt_id_PtBinned2);
  ElectronIDv5_CFMVAReader->AddVariable("Eta",          &bdt_id_Eta);
  ElectronIDv5_CFMVAReader->AddVariable("MiniIsoChHad", &bdt_id_MiniIsoChHad);
  ElectronIDv5_CFMVAReader->AddVariable("MiniIsoPhHad", &bdt_id_MiniIsoPhHad);
  ElectronIDv5_CFMVAReader->AddVariable("MiniIsoNHad",  &bdt_id_MiniIsoNHad);
  ElectronIDv5_CFMVAReader->AddVariable("RelMiniIsoCh", &bdt_id_RelMiniIsoCh);
  ElectronIDv5_CFMVAReader->AddVariable("RelMiniIsoN",  &bdt_id_RelMiniIsoN);
  ElectronIDv5_CFMVAReader->AddVariable("RelIso",       &bdt_id_RelIso);
  if(DataYear == 2016){
    ElectronIDv5_CFMVAReader->AddVariable("Dxy",        &bdt_id_Dxy);
    ElectronIDv5_CFMVAReader->AddVariable("Dz",         &bdt_id_Dz);
  }
  else{
    ElectronIDv5_CFMVAReader->AddVariable("DxySig",     &bdt_id_DxySig);
    ElectronIDv5_CFMVAReader->AddVariable("DzSig",     &bdt_id_DzSig);
  }
  ElectronIDv5_CFMVAReader->AddVariable("IP3D",  &bdt_id_IP3D);
  ElectronIDv5_CFMVAReader->AddVariable("PtRatioV3",    &bdt_id_PtRatioV3);
  ElectronIDv5_CFMVAReader->AddVariable("PtRelV2",      &bdt_id_PtRelV2);
  ElectronIDv5_CFMVAReader->AddVariable("JetDiscCJ"    ,&bdt_id_JetDiscCJ);
  ElectronIDv5_CFMVAReader->AddVariable("JetDiscCJCvsB",&bdt_id_JetDiscCJCvsB);
  ElectronIDv5_CFMVAReader->AddVariable("JetDiscCJCvsL",&bdt_id_JetDiscCJCvsL);

  ElectronIDv5_CFMVAReader->AddVariable("MVA",          &bdt_id_MVA);
  ElectronIDv5_CFMVAReader->AddVariable("MVAIso",       &bdt_id_MVAIso);
  ElectronIDv5_CFMVAReader->AddVariable("dPhiIn",       &bdt_id_dPhiIn);
  ElectronIDv5_CFMVAReader->AddVariable("EoverP",       &bdt_id_EoverP);
  ElectronIDv5_CFMVAReader->AddVariable("FBrem",        &bdt_id_FBrem);
  ElectronIDv5_CFMVAReader->AddVariable("R9" ,          &bdt_id_R9);
  ElectronIDv5_CFMVAReader->AddVariable("e55",          &bdt_id_e55);
  ElectronIDv5_CFMVAReader->AddVariable("PhiWidth",     &bdt_id_PhiWidth);
  ElectronIDv5_CFMVAReader->AddVariable("EtaWidth",     &bdt_id_EtaWidth);
  ElectronIDv5_CFMVAReader->AddVariable("MissingHits",  &bdt_id_MissingHits);
  ElectronIDv5_CFMVAReader->AddVariable("PassConversionVeto",  &bdt_id_PassConversionVeto);
  ElectronIDv5_CFMVAReader->AddVariable("IsGsfCtfScPixChargeConsistent",  &bdt_id_IsGsfCtfScPixChargeConsistent);
  ElectronIDv5_CFMVAReader->AddVariable("IsGsfScPixChargeConsistent",  &bdt_id_IsGsfScPixChargeConsistent);
  ElectronIDv5_CFMVAReader->AddVariable("IsGsfCtfChargeConsistent",  &bdt_id_IsGsfCtfChargeConsistent);
  ElectronIDv5_CFMVAReader->AddVariable("InvEminusInvP",  &bdt_id_InvEminusInvP);
  ElectronIDv5_CFMVAReader->AddSpectator("w_id_tot", &bdt_id_w_id_tot);




  ////////// FAKE MVA READER HF vs LF  
  //////// IP Reader                   
  ElectronIDv5_FakeMVAReader->AddVariable("Eta",          &bdt_id_Eta);
  ElectronIDv5_FakeMVAReader->AddVariable("MiniIsoChHad", &bdt_id_MiniIsoChHad);
  ElectronIDv5_FakeMVAReader->AddVariable("MiniIsoPhHad", &bdt_id_MiniIsoPhHad);
  ElectronIDv5_FakeMVAReader->AddVariable("MiniIsoNHad",  &bdt_id_MiniIsoNHad);
  ElectronIDv5_FakeMVAReader->AddVariable("RelMiniIsoCh", &bdt_id_RelMiniIsoCh);
  ElectronIDv5_FakeMVAReader->AddVariable("RelMiniIsoN",  &bdt_id_RelMiniIsoN);
  ElectronIDv5_FakeMVAReader->AddVariable("RelIso",       &bdt_id_RelIso);
  if(DataYear == 2016){
    ElectronIDv5_FakeMVAReader->AddVariable("Dxy",        &bdt_id_Dxy);
    ElectronIDv5_FakeMVAReader->AddVariable("Dz",         &bdt_id_Dz);
  }
  else{
    ElectronIDv5_FakeMVAReader->AddVariable("DxySig",     &bdt_id_DxySig);
    ElectronIDv5_FakeMVAReader->AddVariable("DzSig",  &bdt_id_DzSig);
  }
  ElectronIDv5_FakeMVAReader->AddVariable("IP3D",  &bdt_id_IP3D);
  ElectronIDv5_FakeMVAReader->AddVariable("PtRatioV3",    &bdt_id_PtRatioV3);
  ElectronIDv5_FakeMVAReader->AddVariable("PtRelV2",      &bdt_id_PtRelV2);
  ElectronIDv5_FakeMVAReader->AddVariable("JetDiscCJ"    ,&bdt_id_JetDiscCJ);
  ElectronIDv5_FakeMVAReader->AddVariable("JetDiscCJCvsB",&bdt_id_JetDiscCJCvsB);
  ElectronIDv5_FakeMVAReader->AddVariable("JetDiscCJCvsL",&bdt_id_JetDiscCJCvsL);
  ElectronIDv5_FakeMVAReader->AddVariable("MVA",          &bdt_id_MVA);
  ElectronIDv5_FakeMVAReader->AddVariable("MVAIso",       &bdt_id_MVAIso);
  ElectronIDv5_FakeMVAReader->AddVariable("dPhiIn",       &bdt_id_dPhiIn);
  ElectronIDv5_FakeMVAReader->AddVariable("EoverP",       &bdt_id_EoverP);
  ElectronIDv5_FakeMVAReader->AddVariable("FBrem",        &bdt_id_FBrem);
  ElectronIDv5_FakeMVAReader->AddVariable("R9" ,          &bdt_id_R9);
  ElectronIDv5_FakeMVAReader->AddVariable("CEMFracCJ",&bdt_id_CEMFracCJ);
  ElectronIDv5_FakeMVAReader->AddVariable("NEMFracCJ",&bdt_id_NEMFracCJ);
  ElectronIDv5_FakeMVAReader->AddVariable("CHFracCJ",&bdt_id_CHFracCJ);
  ElectronIDv5_FakeMVAReader->AddVariable("NHFracCJ",&bdt_id_NHFracCJ);
  ElectronIDv5_FakeMVAReader->AddVariable("e55",          &bdt_id_e55);
  ElectronIDv5_FakeMVAReader->AddVariable("PhiWidth",     &bdt_id_PhiWidth);
  ElectronIDv5_FakeMVAReader->AddVariable("EtaWidth",     &bdt_id_EtaWidth);
  ElectronIDv5_FakeMVAReader->AddVariable("MissingHits",  &bdt_id_MissingHits);
  ElectronIDv5_FakeMVAReader->AddVariable("PassConversionVeto",  &bdt_id_PassConversionVeto);
  ElectronIDv5_FakeMVAReader->AddVariable("IsGsfCtfScPixChargeConsistent",  &bdt_id_IsGsfCtfScPixChargeConsistent);
  ElectronIDv5_FakeMVAReader->AddVariable("IsGsfScPixChargeConsistent",  &bdt_id_IsGsfScPixChargeConsistent);
  ElectronIDv5_FakeMVAReader->AddVariable("IsGsfCtfChargeConsistent",  &bdt_id_IsGsfCtfChargeConsistent);
  ElectronIDv5_FakeMVAReader->AddVariable("InvEminusInvP",  &bdt_id_InvEminusInvP);
  ElectronIDv5_FakeMVAReader->AddSpectator("w_id_tot", &bdt_id_w_id_tot);


  TString MVAPathMulti ="/data6/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeIMulti/Version11/"   +GetYearString()+"/dataset/weights/";
  TString MVAPathMultiV10 ="/data6/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeIMulti/Version10/"   +GetYearString()+"/dataset/weights/";

  TString MVAPathFake ="/data6/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeIElectronFake/Version12/"        +GetYearString()+"/dataset/weights/";
  TString xmlpf = "_TMVAClassification_BDTG.weights.xml";


  if(DataYear == 2016){
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_LF_Fake",   MVAPathMulti+ "BDTG_version11_FakeBkg_LF_TypeI_EE_SignalFakeBkg_LF_IP_Inc_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_HFB_Fake",  MVAPathMulti+ "BDTG_version11_FakeBkg_HFB_TypeI_EE_SignalFakeBkg_HFB_IP_Inc_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_HFC_Fake",  MVAPathMulti+ "BDTG_version11_FakeBkg_HFC_TypeI_EE_SignalFakeBkg_HFC_IP_Inc_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_BB_Fake",   MVAPathMulti+ "BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_Inc_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_EC_Fake",   MVAPathMulti+ "BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_EC_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_BB_Conv",   MVAPathMulti+ "BDTG_version11_ConvBkg_TypeI_EE_SignalConvBkg_IP_Inc_2016_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.6_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_EC_Conv",   MVAPathMulti+ "BDTG_version11_ConvBkg_TypeI_EE_SignalConvBkg_IP_EC_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_BB_CF",     MVAPathMulti+ "BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_CFPt_Inc_2016_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_EC_CF",     MVAPathMulti+ "BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_CFPt_EC_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.6_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_CFMVAReader->BookMVA("BDTGv5Pt_BB_CF",   MVAPathMulti+ "BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_IP_Inc_2016_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_CFMVAReader->BookMVA("BDTGv5Pt_EC_CF",   MVAPathMulti+ "BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_IP_EC_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);

    //----> 0.991062884939: ['0.149', '0.185', 'BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_IP_Inc_2016_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.990798401177: ['0.181', '0.673', 'BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_CFPt_Inc_2016_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.97059833623:  ['0.661', '0.15',  'BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_IP_EC_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.97117516843:  ['0.568', '0.126', 'BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_CFPt_EC_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.6_Seed_200_BDT.root']    
    //----> 0.957969456743: ['0.668', '0.106', 'BDTG_version11_ConvBkg_TypeI_EE_SignalConvBkg_IP_Inc_2016_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.6_Seed_200_BDT.root'] 
    //----> 0.931905703359: ['0.4',   '0.149', 'BDTG_version11_ConvBkg_TypeI_EE_SignalConvBkg_IP_EC_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_200_BDT.root'] 
    //----> 0.976175150802: ['0.116', '0.164', 'BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_Inc_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.987073656244: ['0.144', '0.126', 'BDTG_version11_FakeBkg_HFB_TypeI_EE_SignalFakeBkg_HFB_IP_Inc_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.97930107624:  ['0.349', '0.133', 'BDTG_version11_FakeBkg_HFC_TypeI_EE_SignalFakeBkg_HFC_IP_Inc_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.979841755577: ['0.125', '0.128', 'BDTG_version11_FakeBkg_LF_TypeI_EE_SignalFakeBkg_LF_IP_Inc_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //----> 0.959983970767: ['0.327', '0.105', 'BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_EC_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //// Falvour Fake                                                                                                                                                                                                                                                             
    //ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LFvsHF_Fake",         MVAPathFake+ "BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_LFvsHF_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf); 
    //ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_HFBvsHFC_Fake",                               MVAPathFake+ "BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_HFBvsHFC_2016_NTrees300_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    
    

    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_LFvsHF_Fake",
                                        MVAPathFake +"BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFvsHF_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT"+ xmlpf);

    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_HFBvsHFC_Fake",
                                        MVAPathFake+ "BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_HFBvsHFC_2016_NTrees700_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf);


    //----> 0.820222557836: ['0.45', '0.108', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_HFBvsHFC_2016_NTrees700_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.845329887104: ['0.508', '0.114', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_HFBvsHFC_2016_NTrees300_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_200_BDT.root'] 
    ///----> 0.940261336547: ['0.108', '0.844', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_LFvsHF_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.956875330221: ['0.107', '0.468', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFvsHF_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT.root'] 
    //ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LF1_Fake",MVAPathFake +"BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFMother1_2016_NTrees300_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_1_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_600_BDT"+ xmlpf); 

    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LF1_Fake",MVAPathFake +"BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFMother2_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_400_BDT"+ xmlpf);
    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LF2_Fake",MVAPathFake +"BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFMother3_2016_NTrees300_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);


  }

  if(DataYear == 2017){

    ElectronIDv5_MVAReader->BookMVA("BDTGv5_LF_Fake",  MVAPathMulti+ "BDTG_version11_FakeBkg_LF_TypeI_EE_SignalFakeBkg_LF_IP_Inc_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_HFB_Fake", MVAPathMulti+ "BDTG_version11_FakeBkg_HFB_TypeI_EE_SignalFakeBkg_HFB_IP_Inc_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_HFC_Fake", MVAPathMulti+ "BDTG_version11_FakeBkg_HFC_TypeI_EE_SignalFakeBkg_HFC_IP_Inc_2017_NTrees300_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_BB_Fake",  MVAPathMulti+ "BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_Inc_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_BB_Conv",  MVAPathMultiV10+ "BDTG_version10_ConvBkg_TypeI_EE_SignalConvBkg_IP_Inc_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_EC_Fake",  MVAPathMulti+ "BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_EC_2017_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_EC_Conv",  MVAPathMulti+ "BDTG_version11_ConvBkg_TypeI_EE_SignalConvBkg_IP_EC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_BB_CF",    MVAPathMultiV10+ "BDTG_version10_CFBkg_TypeI_EE_SignalCFBkg_IP_Inc_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_EC_CF",    MVAPathMultiV10+ "BDTG_version10_CFBkg_TypeI_EE_SignalCFBkg_IP_EC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_CFMVAReader->BookMVA("BDTGv5Pt_BB_CF",   MVAPathMulti+ "BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_IP_Inc_2017_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_CFMVAReader->BookMVA("BDTGv5Pt_EC_CF",   MVAPathMulti+ "BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_IP_EC_2017_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);

    //----> 0.990828228762: ['0.128', '0.387', 'BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_IP_Inc_2017_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //----> 0.978197862817: ['0.601', '0.116', 'BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_IP_EC_2017_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.991111776595: ['0.137', '0.305', 'BDTG_version10_CFBkg_TypeI_EE_SignalCFBkg_IP_Inc_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //----> 0.978149831063: ['0.715', '0.139', 'BDTG_version10_CFBkg_TypeI_EE_SignalCFBkg_IP_EC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //----> 0.95638347114: ['0.478', '0.107', 'BDTG_version10_ConvBkg_TypeI_EE_SignalConvBkg_IP_Inc_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //----> 0.936974932769: ['0.808', '0.106', 'BDTG_version11_ConvBkg_TypeI_EE_SignalConvBkg_IP_EC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //----> 0.978882992815: ['0.145', '0.188', 'BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_Inc_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //----> 0.983114923384: ['0.225', '0.103', 'BDTG_version11_FakeBkg_LF_TypeI_EE_SignalFakeBkg_LF_IP_Inc_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //----> 0.988083459142: ['0.222', '0.182', 'BDTG_version11_FakeBkg_HFB_TypeI_EE_SignalFakeBkg_HFB_IP_Inc_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //----> 0.967189103974: ['0.566', '0.145', 'BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_EC_2017_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 

    //// Reader for LF/HF tagging  
    //ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LFvsHF_Fake",         MVAPathFake+ "BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_LFvsHF_2017_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf); 
    //ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_HFBvsHFC_Fake",       MVAPathFake+ "BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_HFBvsHFC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf); 
    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_LFvsHF_Fake",      MVAPathFake+ "BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFvsHF_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_HFBvsHFC_Fake",    MVAPathFake+ "BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_HFBvsHFC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf);

    //----> 0.832200821965: ['0.316', '0.162', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_HFBvsHFC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.860209742143: ['0.115', '0.542', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_HFBvsHFC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_2_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT.root']  
    //----> 0.946179015603: ['0.114', '0.693', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_LFvsHF_2017_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT.root']  
    //----> 0.954029739705: ['0.884', '0.139', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFvsHF_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LF1_Fake",MVAPathFake +"BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFMother1_2017_NTrees300_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT"+ xmlpf);                                   
    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LF1_Fake",MVAPathFake +"BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFMother2_2017_NTrees300_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_300_BDT"+ xmlpf);
    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LF2_Fake",MVAPathFake +"BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFMother3_2017_NTrees300_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);

  }

  if(DataYear == 2018){
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_LF_Fake",  MVAPathMulti+ "BDTG_version11_FakeBkg_LF_TypeI_EE_SignalFakeBkg_LF_IP_Inc_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_HFB_Fake", MVAPathMulti+ "BDTG_version11_FakeBkg_HFB_TypeI_EE_SignalFakeBkg_HFB_IP_Inc_2018_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_HFC_Fake", MVAPathMulti+ "BDTG_version11_FakeBkg_HFC_TypeI_EE_SignalFakeBkg_HFC_IP_Inc_2018_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_BB_Fake",  MVAPathMulti+ "BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_Inc_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_BB_Conv",  MVAPathMulti+ "BDTG_version11_ConvBkg_TypeI_EE_SignalConvBkg_IP_Inc_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.6_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_EC_Fake",  MVAPathMulti+ "BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_EC_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_EC_Conv",  MVAPathMulti+ "BDTG_version11_ConvBkg_TypeI_EE_SignalConvBkg_IP_EC_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_0.1_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_BB_CF",    MVAPathMulti+ "BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_CFPt_Inc_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_MVAReader->BookMVA("BDTGv5_EC_CF",    MVAPathMulti+ "BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_CFPt_EC_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.6_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_CFMVAReader->BookMVA("BDTGv5Pt_BB_CF",   MVAPathMulti+ "BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_IP_Inc_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_CFMVAReader->BookMVA("BDTGv5Pt_EC_CF",   MVAPathMulti+ "BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_IP_EC_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+ xmlpf);


    //ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LFvsHF_Fake",         MVAPathFake+ "BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_LFvsHF_2018_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf); 
    //ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_HFBvsHFC_Fake",       MVAPathFake+ "BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_HFBvsHFC_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf); 
    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_LFvsHF_Fake",      MVAPathFake+ "BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFvsHF_2018_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_QCD_HFBvsHFC_Fake",    MVAPathFake+ "BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_HFBvsHFC_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf);


    //----> 0.828152956177: ['0.107', '0.117', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_HFBvsHFC_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root']  
    //----> 0.862270057625: ['0.876', '0.158', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_HFBvsHFC_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.946927590863: ['0.152', '0.143', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_LFvsHF_2018_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.960522118948: ['0.183', '0.118', 'BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFvsHF_2018_NTrees700_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 

    //----> 0.983071530511: ['0.41', '0.138', 'BDTG_version11_FakeBkg_LF_TypeI_EE_SignalFakeBkg_LF_IP_Inc_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //----> 0.988276867484: ['0.332', '0.196', 'BDTG_version11_FakeBkg_HFB_TypeI_EE_SignalFakeBkg_HFB_IP_Inc_2018_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_200_Shrinkage_0.1_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.982666976387: ['0.333', '0.102', 'BDTG_version11_FakeBkg_HFC_TypeI_EE_SignalFakeBkg_HFC_IP_Inc_2018_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_3_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.979829589516: ['0.873', '0.1', 'BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_Inc_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.969013599405: ['0.339', '0.118', 'BDTG_version11_FakeBkg_TypeI_EE_SignalFakeBkg_IP_EC_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.95722221247: ['0.11', '0.101', 'BDTG_version11_ConvBkg_TypeI_EE_SignalConvBkg_IP_Inc_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_4_nCuts_200_Shrinkage_0.05_BaggedFrac_0.6_Seed_200_BDT.root'] 
    //----> 0.938911043985: ['0.117', '0.108', 'BDTG_version11_ConvBkg_TypeI_EE_SignalConvBkg_IP_EC_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_0.1_MaxDepth_3_nCuts_200_Shrinkage_0.1_BaggedFrac_0.6_Seed_100_BDT.root'] 
    //----> 0.991307165103: ['0.12', '0.243', 'BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_IP_Inc_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
    //----> 0.990921894748: ['0.101', '0.78', 'BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_CFPt_Inc_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root']
    //----> 0.978420600256: ['0.406', '0.11', 'BDTG_version11_CFBkg_TypeI_EE_SignalCFBkg_CFPt_EC_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_0.5_MaxDepth_5_nCuts_200_Shrinkage_0.05_BaggedFrac_0.6_Seed_200_BDT.root'] 

    //ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LF1_Fake",MVAPathFake +"BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFMother1_2018_NTrees300_NormMode_EqualNumEvents_MinNodeSize_1.0_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT"+ xmlpf);
    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LF1_Fake",MVAPathFake +"BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFMother2_2018_NTrees300_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf);
    ElectronIDv5_FakeMVAReader->BookMVA("BDTGv5_LF2_Fake",MVAPathFake +"BDTG_version12_FakeBkg_TypeI_EE_SignalElectronFake_IP_QCD_LFMother3_2018_NTrees300_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_200_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+ xmlpf);

  }


  return;

}









void AnalyzerCore::SetupIDMVAReaderElectron(){

  if(!( iSetupLeptonBDTv4)) return;
  
  /* 
     electron_v1:                                                                                                                                             
     - sets up for legacy purposes the first of the BDT ID. This was in Dec 2022. This tuned out buggy since 3 variables were set as ints/floats incorrectly.      This also had more variables in the training.      
     electron_v2plus:                                                                                                                                                                                                                                                                                                        

     - This is all versions after and including version2.                                                                                                                                                        
     - These all share the training variables, hense why they can be grouped together and use the same Reader class variable. 
     -  Version 2 of the training had issues in regards to ptbinned variable being set wrong and so was scrpped            
     -  Version 3 had issues in miniiso and so Fakes were scrapped                                                        
     -  Version 4 saw poor training in LF and so the LF was retrained removiong pi+                                          
  */


  InitializeIDTreeVars();

  ElectronIDv4_FakeMVAReader->AddVariable("Pt", &bdt_id_Pt);
  ElectronIDv4_FakeMVAReader->AddVariable("Eta", &bdt_id_Eta);
  ElectronIDv4_FakeMVAReader->AddVariable("MiniIsoChHad", &bdt_id_MiniIsoChHad);
  ElectronIDv4_FakeMVAReader->AddVariable("MiniIsoPhHad", &bdt_id_MiniIsoPhHad);
  ElectronIDv4_FakeMVAReader->AddVariable("MiniIsoNHad", &bdt_id_MiniIsoNHad);
  ElectronIDv4_FakeMVAReader->AddVariable("RelMiniIsoCh", &bdt_id_RelMiniIsoCh);
  ElectronIDv4_FakeMVAReader->AddVariable("RelMiniIsoN", &bdt_id_RelMiniIsoN);
  ElectronIDv4_FakeMVAReader->AddVariable("Dxy",  &bdt_id_Dxy);
  ElectronIDv4_FakeMVAReader->AddVariable("Dz",  &bdt_id_Dz);
  ElectronIDv4_FakeMVAReader->AddVariable("RelIso", &bdt_id_RelIso);
  ElectronIDv4_FakeMVAReader->AddVariable("IP3D", &bdt_id_IP3D);
  ElectronIDv4_FakeMVAReader->AddVariable("PtRatio",  &bdt_id_PtRatio);
  ElectronIDv4_FakeMVAReader->AddVariable("PtRel",  &bdt_id_PtRel);
  ElectronIDv4_FakeMVAReader->AddVariable("NEMFracCJ",&bdt_id_NEMFracCJ);
  ElectronIDv4_FakeMVAReader->AddVariable("CHFracCJ",&bdt_id_CHFracCJ);
  ElectronIDv4_FakeMVAReader->AddVariable("JetDiscCJ",&bdt_id_JetDiscCJ);
  ElectronIDv4_FakeMVAReader->AddVariable("NHFracCJ",&bdt_id_NHFracCJ);
  ElectronIDv4_FakeMVAReader->AddVariable("CEMFracCJ",&bdt_id_CEMFracCJ);
  ElectronIDv4_FakeMVAReader->AddVariable("MVA",  &bdt_id_MVA);
  ElectronIDv4_FakeMVAReader->AddVariable("MVAIso",  &bdt_id_MVAIso);
  ElectronIDv4_FakeMVAReader->AddSpectator("w_id_tot", &bdt_id_w_id_tot);

  ElectronIDv4_CFMVAReader->AddVariable("PtBinned", &bdt_id_PtBinned);
  ElectronIDv4_CFMVAReader->AddVariable("Eta", &bdt_id_Eta);
  ElectronIDv4_CFMVAReader->AddVariable("Dxy",  &bdt_id_Dxy);
  ElectronIDv4_CFMVAReader->AddVariable("DxySig",  &bdt_id_DxySig);
  ElectronIDv4_CFMVAReader->AddVariable("Dz",  &bdt_id_Dz);
  ElectronIDv4_CFMVAReader->AddVariable("DzSig",  &bdt_id_DzSig);
  ElectronIDv4_CFMVAReader->AddVariable("IP3D", &bdt_id_IP3D);
  ElectronIDv4_CFMVAReader->AddVariable("MVA",  &bdt_id_MVA);
  ElectronIDv4_CFMVAReader->AddVariable("MVAIso",  &bdt_id_MVAIso);
  ElectronIDv4_CFMVAReader->AddVariable("Full5x5_sigmaIetaIeta",  &bdt_id_Full5x5_sigmaIetaIeta);
  ElectronIDv4_CFMVAReader->AddVariable("dPhiIn",  &bdt_id_dPhiIn);
  ElectronIDv4_CFMVAReader->AddVariable("EoverP",  &bdt_id_EoverP);
  ElectronIDv4_CFMVAReader->AddVariable("FBrem",  &bdt_id_FBrem);
  ElectronIDv4_CFMVAReader->AddVariable("R9",  &bdt_id_R9);
  ElectronIDv4_CFMVAReader->AddVariable("e55",  &bdt_id_e55);
  ElectronIDv4_CFMVAReader->AddVariable("EtaWidth",  &bdt_id_EtaWidth);
  ElectronIDv4_CFMVAReader->AddVariable("PhiWidth",  &bdt_id_PhiWidth);
  ElectronIDv4_CFMVAReader->AddVariable("PassConversionVeto",  &bdt_id_PassConversionVeto);
  ElectronIDv4_CFMVAReader->AddVariable("IsGsfCtfScPixChargeConsistent",  &bdt_id_IsGsfCtfScPixChargeConsistent);
  ElectronIDv4_CFMVAReader->AddVariable("IsGsfScPixChargeConsistent",  &bdt_id_IsGsfScPixChargeConsistent);
  ElectronIDv4_CFMVAReader->AddVariable("IsGsfCtfChargeConsistent",  &bdt_id_IsGsfCtfChargeConsistent);
  ElectronIDv4_CFMVAReader->AddVariable("InvEminusInvP", &bdt_id_InvEminusInvP);
  ElectronIDv4_CFMVAReader->AddVariable("hcalPFClusterIso",  &bdt_id_hcalPFClusterIso);
  ElectronIDv4_CFMVAReader->AddVariable("ecalPFClusterIso",  &bdt_id_ecalPFClusterIso);
  ElectronIDv4_CFMVAReader->AddVariable("dr03TkSumPt",  &bdt_id_dr03TkSumPt);
  ElectronIDv4_CFMVAReader->AddVariable("HoverE",  &bdt_id_HoverE);
  ElectronIDv4_CFMVAReader->AddVariable("MissingHits",  &bdt_id_MissingHits);
  ElectronIDv4_CFMVAReader->AddSpectator("w_id_tot", &bdt_id_w_id_tot);


  ElectronIDv4_ConvMVAReader->AddVariable("Pt", &bdt_id_Pt);
  ElectronIDv4_ConvMVAReader->AddVariable("Eta", &bdt_id_Eta);
  ElectronIDv4_ConvMVAReader->AddVariable("MiniIsoChHad", &bdt_id_MiniIsoChHad);
  ElectronIDv4_ConvMVAReader->AddVariable("RelMiniIsoCh", &bdt_id_RelMiniIsoCh);
  ElectronIDv4_ConvMVAReader->AddVariable("IsoChHad", &bdt_id_IsoChHad);
  ElectronIDv4_ConvMVAReader->AddVariable("IsoNHad", &bdt_id_IsoNHad);
  ElectronIDv4_ConvMVAReader->AddVariable("IsoPhHad", &bdt_id_IsoPhHad);
  ElectronIDv4_ConvMVAReader->AddVariable("Dxy",  &bdt_id_Dxy);
  ElectronIDv4_ConvMVAReader->AddVariable("DxySig",  &bdt_id_DxySig);
  ElectronIDv4_ConvMVAReader->AddVariable("Dz",  &bdt_id_Dz);
  ElectronIDv4_ConvMVAReader->AddVariable("DzSig",  &bdt_id_DzSig);
  ElectronIDv4_ConvMVAReader->AddVariable("RelIso", &bdt_id_RelIso);
  ElectronIDv4_ConvMVAReader->AddVariable("IP3D", &bdt_id_IP3D);
  ElectronIDv4_ConvMVAReader->AddVariable("PtRatio",  &bdt_id_PtRatio);
  ElectronIDv4_ConvMVAReader->AddVariable("PtRel",  &bdt_id_PtRel);
  ElectronIDv4_ConvMVAReader->AddVariable("CEMFracCJ",&bdt_id_CEMFracCJ);
  ElectronIDv4_ConvMVAReader->AddVariable("NEMFracCJ",&bdt_id_NEMFracCJ);
  ElectronIDv4_ConvMVAReader->AddVariable("CHFracCJ",&bdt_id_CHFracCJ);
  ElectronIDv4_ConvMVAReader->AddVariable("JetDiscCJ",&bdt_id_JetDiscCJ);
  ElectronIDv4_ConvMVAReader->AddVariable("NHFracCJ",&bdt_id_NHFracCJ);
  ElectronIDv4_ConvMVAReader->AddVariable("MVA",  &bdt_id_MVA);
  ElectronIDv4_ConvMVAReader->AddVariable("MVAIso",  &bdt_id_MVAIso);
  ElectronIDv4_ConvMVAReader->AddVariable("Full5x5_sigmaIetaIeta",  &bdt_id_Full5x5_sigmaIetaIeta);
  ElectronIDv4_ConvMVAReader->AddVariable("dEtaSeed",  &bdt_id_dEtaSeed);
  ElectronIDv4_ConvMVAReader->AddVariable("dPhiIn",  &bdt_id_dPhiIn);
  ElectronIDv4_ConvMVAReader->AddVariable("EoverP",  &bdt_id_EoverP);
  ElectronIDv4_ConvMVAReader->AddVariable("FBrem",  &bdt_id_FBrem);
  ElectronIDv4_ConvMVAReader->AddVariable("R9",  &bdt_id_R9);
  ElectronIDv4_ConvMVAReader->AddVariable("TrkIso",  &bdt_id_TrkIso);
  ElectronIDv4_ConvMVAReader->AddVariable("EtaWidth",  &bdt_id_EtaWidth);
  ElectronIDv4_ConvMVAReader->AddVariable("PhiWidth",  &bdt_id_PhiWidth);
  ElectronIDv4_ConvMVAReader->AddVariable("InvEminusInvP", &bdt_id_InvEminusInvP);
  ElectronIDv4_ConvMVAReader->AddVariable("ecalPFClusterIso",  &bdt_id_ecalPFClusterIso);
  ElectronIDv4_ConvMVAReader->AddVariable("MissingHits",  &bdt_id_MissingHits);
  ElectronIDv4_ConvMVAReader->AddVariable("PassConversionVeto",  &bdt_id_PassConversionVeto);
  ElectronIDv4_ConvMVAReader->AddVariable("IsGsfCtfScPixChargeConsistent",  &bdt_id_IsGsfCtfScPixChargeConsistent);
  ElectronIDv4_ConvMVAReader->AddVariable("IsGsfScPixChargeConsistent",  &bdt_id_IsGsfScPixChargeConsistent);
  ElectronIDv4_ConvMVAReader->AddVariable("IsGsfCtfChargeConsistent",  &bdt_id_IsGsfCtfChargeConsistent);
  ElectronIDv4_ConvMVAReader->AddSpectator("w_id_tot", &bdt_id_w_id_tot);

  TString AnalyzerPath=std::getenv("SKFlat_WD");
  /// Version 2 paths                                                                                      
  TString MVAPathConvv2 ="/data6/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeIConv/Version3/"        +GetYearString()+"/dataset/weights/";
  TString MVAPathCFv2   ="/data6/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeICF/Version3/"          +GetYearString()+"/dataset/weights/";

  /// Other    

  TString MVAPathFakev3 ="/data6/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeIElectronFake/Version4/"+GetYearString()+"/dataset/weights/";

  TString MVAPathFakev4 ="/data6/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeIElectronFake/Version5/"+GetYearString()+"/dataset/weights/";
  TString MVAPathFakev5a ="/data6/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeIElectronFake/Version6/"+GetYearString()+"/dataset/weights/";
  TString MVAPathFakev5b ="/data6/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeIElectronFakeHF/Version6/"+GetYearString()+"/dataset/weights/";

  TString MVAPathConvv3 ="/data6/Users/jalmond/BDTOutput/Run2UltraLegacy_v3/runIDBDT_HNtypeIConv/Version6/"        +GetYearString()+"/dataset/weights/";
  TString xmlpf = "_TMVAClassification_BDTG.weights.xml";


  ///////////////////////////////////////////////////////////////////////////////// 
  ///////////////////////////////////////////////////////////////////////////////// 
  ///                                                                               
  ///    VERSION 1 ELECTRON MVA (Dec 22)                                           
  ///                                                                              
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  /// Version1 : Original training with some minor bugs                            
  /// Version2 : new training with bug fix and some variables changed + ptbinned   
  /// Version3 : is new with bug fix plus All variables checked,                   
  /// Version4 : fixed rho bug ub version3                                         
  /// Version5 : is same as 4 but with pi+ fakes suppressed                        
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                              
  ///    VERSION 3 ELECTRON MVA (Feb 20)                                           
  ///                                                                              
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////


  cout << "Lepton BDT ID details: Training done for CF/Conversion/Fake backgrounds." << endl;

  if(GetYear() == 2016) {
    cout << "Setting up 2016 CF READER: ElectronIDv4_CFMVAReader/ElectronIDv4_CFMVAReaderPt/ElectronIDv3CFMVAReader" <<endl;

    ElectronIDv4_CFMVAReader->BookMVA("BDTGv4_BB_CF",     MVAPathCFv2+ "BDTG_version3_CF_TypeI_EE_SignalCFPtBinned_BB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT" +xmlpf);
    ElectronIDv4_CFMVAReader->BookMVA("BDTGv4_EC_CF",     MVAPathCFv2+ "BDTG_version3_CF_TypeI_EE_SignalCFPtBinned_EC_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT" +xmlpf);
    ElectronIDv4_CFMVAReader->BookMVA("BDTGv4_CF",        MVAPathCFv2+ "BDTG_version3_CF_TypeI_EE_SignalCFPtBinned_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.5_BaggedFrac_0.8_Seed_100_BDT" +xmlpf);

  }
  if(GetYear() == 2017) {
    cout << "ElectronIDv4_CFMVAReader 2017 ElectronIDv4_CFMVAReader/ElectronIDv4_CFMVAReaderPt/ElectronIDv3CFMVAReader" << endl;
    ElectronIDv4_CFMVAReader->BookMVA("BDTGv4_BB_CF",     MVAPathCFv2+ "BDTG_version3_CF_TypeI_EE_SignalCFPtBinned_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT" +xmlpf);
    ElectronIDv4_CFMVAReader->BookMVA("BDTGv4_EC_CF",     MVAPathCFv2+ "BDTG_version3_CF_TypeI_EE_SignalCFPtBinned_EC_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT" +xmlpf);
    ElectronIDv4_CFMVAReader->BookMVA("BDTGv4_CF",        MVAPathCFv2+ "BDTG_version3_CF_TypeI_EE_SignalCFPtBinned_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.5_BaggedFrac_0.8_Seed_100_BDT" +xmlpf);

  }

  if(GetYear() == 2018) {
    cout << "ElectronIDv4_CFMVAReader 2018 ElectronIDv4_CFMVAReader/ElectronIDv4_CFMVAReaderPt/ElectronIDv3CFMVAReader" << endl;

    ElectronIDv4_CFMVAReader->BookMVA("BDTGv4_BB_CF",     MVAPathCFv2+ "BDTG_version3_CF_TypeI_EE_SignalCFPtBinned_BB_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_CFMVAReader->BookMVA("BDTGv4_EC_CF",     MVAPathCFv2+ "BDTG_version3_CF_TypeI_EE_SignalCFPtBinned_EC_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT" +xmlpf);
    ElectronIDv4_CFMVAReader->BookMVA("BDTGv4_CF",        MVAPathCFv2+ "BDTG_version3_CF_TypeI_EE_SignalCFPtBinned_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.5_BaggedFrac_0.8_Seed_100_BDT" +xmlpf);
  }

  /// FAKE ID BDT 

  if(GetYear() == 2016) {

    ////// Version 5 training--> v4 variable 
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Fake"     ,MVAPathFakev4+"BDTG_version5_FakeBkg_TypeI_EE_SignalElectronFake_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Top_Fake" ,MVAPathFakev4+"BDTG_version5_FakeBkgTop_TypeI_EE_SignalElectronFake_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_LF_Fake"  ,MVAPathFakev4+"BDTG_version5_FakeBkg_LF_TypeI_EE_SignalElectronFake_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HF_Fake"  ,MVAPathFakev3+"BDTG_version4_FakeBkg_HF_TypeI_EE_SignalElectronFake_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFB_Fake" ,MVAPathFakev3+"BDTG_version4_FakeBkg_HFB_TypeI_EE_SignalElectronFake_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFC_Fake" ,MVAPathFakev3+"BDTG_version4_FakeBkg_HFC_TypeI_EE_SignalElectronFake_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    // 0.971327007236: ['0.202', '0.815', 'BDTG_version5_FakeBkg_TypeI_EE_SignalElectronFake_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    // 0.976957626476: ['0.372', '0.332', 'BDTG_version5_FakeBkgTop_TypeI_EE_SignalElectronFake_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    // 0.975798366843: ['0.504', '0.11',  'BDTG_version5_FakeBkg_LF_TypeI_EE_SignalElectronFake_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 

    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_BB_Fake"  ,MVAPathFakev5a+"BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_BB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Top_BB_Fake" ,MVAPathFakev5a+"BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_BB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_LF_BB_Fake"  ,MVAPathFakev5a+"BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_BB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HF_BB_Fake"  ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_BB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFB_BB_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_BB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFC_BB_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_BB_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);

    //----> 0.975949420253: ['0.446', '0.205', 'BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_BB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.979959343427: ['0.73', '0.103', 'BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_BB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.979043755334: ['0.33', '0.191', 'BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_BB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.985552179145: ['0.848', '0.159', 'BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_BB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.98730828959: ['0.751', '0.405', 'BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_BB_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.976667335878: ['0.683', '0.162', 'BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_BB_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 

    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_EC_Fake"  ,   MVAPathFakev5a+"BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_EC_2016_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Top_EC_Fake" ,MVAPathFakev5a+"BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_EC_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_LF_EC_Fake"  ,MVAPathFakev5a+"BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_EC_2016_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HF_EC_Fake"  ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_EC_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFB_EC_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_EC_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFC_EC_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_EC_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT"+xmlpf);
    //----> 0.95022859816: ['0.256', '0.136', 'BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_EC_2016_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT.root']     
    //----> 0.961784246695: ['0.134', '0.15', 'BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_EC_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root']
    //----> 0.956937361582: ['0.173', '0.303', 'BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_EC_2016_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT.root'] 
    // ----> 0.971113356591: ['0.705', '0.241', 'BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_EC_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    // ----> 0.974627102644: ['0.54', '0.168', 'BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_EC_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.960022052178: ['0.491', '0.155', 'BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_EC_2016_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT.root'] 
  }
  if(GetYear() == 2017) {

    ////// Version 5 training--> v4 variable (HF same as v3/version4)                                                                                                                                                       
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Fake",    MVAPathFakev4+"BDTG_version5_FakeBkg_TypeI_EE_SignalElectronFake_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Top_Fake",MVAPathFakev4+"BDTG_version5_FakeBkgTop_TypeI_EE_SignalElectronFake_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_LF_Fake", MVAPathFakev4+"BDTG_version5_FakeBkg_LF_TypeI_EE_SignalElectronFake_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HF_Fake", MVAPathFakev3+"BDTG_version4_FakeBkg_HF_TypeI_EE_SignalElectronFake_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFB_Fake",MVAPathFakev3+"BDTG_version4_FakeBkg_HFB_TypeI_EE_SignalElectronFake_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFC_Fake",MVAPathFakev3+"BDTG_version4_FakeBkg_HFC_TypeI_EE_SignalElectronFake_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_200_BDT"+xmlpf);

    //  0.975586771441: ['0.223', '0.123', 'BDTG_version5_FakeBkg_TypeI_EE_SignalElectronFake_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //  0.979394122213: ['0.577', '0.367', 'BDTG_version5_FakeBkgTop_TypeI_EE_SignalElectronFake_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //  0.980371549497: ['0.164', '0.111', 'BDTG_version5_FakeBkg_LF_TypeI_EE_SignalElectronFake_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 

    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_BB_Fake"  ,MVAPathFakev5a+"BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Top_BB_Fake" ,MVAPathFakev5a+"BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_LF_BB_Fake"  ,MVAPathFakev5a+"BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HF_BB_Fake"  ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFB_BB_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFC_BB_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);

    //----> 0.978938844852: ['0.751', '0.729', 'BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.981902876321: ['0.629', '0.165', 'BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.981820420981: ['0.563', '0.275', 'BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.987001382377: ['0.267', '0.196', 'BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    // ----> 0.988386810659: ['0.388', '0.232', 'BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.979082329986: ['0.667', '0.227', 'BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_BB_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 

    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_EC_Fake"  ,MVAPathFakev5a+"BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_EC_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Top_EC_Fake" ,MVAPathFakev5a+"BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_EC_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_LF_EC_Fake"  ,MVAPathFakev5a+"BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_EC_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HF_EC_Fake"  ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_EC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFB_EC_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_EC_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFC_EC_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_EC_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);

    //----> 0.962165822548: ['0.248', '0.176', 'BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_EC_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root']    
    //----> 0.968933049054: ['0.231', '0.169', 'BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_EC_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.970866620109: ['0.639', '0.264', 'BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_EC_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.974173905908: ['0.476', '0.111', 'BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_EC_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.97665037703: ['0.778', '0.113', 'BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_EC_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.969668869156: ['0.508', '0.231', 'BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_EC_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 

  }


  if(GetYear() == 2018) {

    ////// Version 5 training--> v4 variable  
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Fake",MVAPathFakev4+"BDTG_version5_FakeBkg_TypeI_EE_SignalElectronFake_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Top_Fake",MVAPathFakev4+"BDTG_version5_FakeBkgTop_TypeI_EE_SignalElectronFake_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_LF_Fake",MVAPathFakev4+"BDTG_version5_FakeBkg_LF_TypeI_EE_SignalElectronFake_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HF_Fake",MVAPathFakev3+"BDTG_version4_FakeBkg_HF_TypeI_EE_SignalElectronFake_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFB_Fake",MVAPathFakev3+"BDTG_version4_FakeBkg_HFB_TypeI_EE_SignalElectronFake_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFC_Fake",MVAPathFakev3+"BDTG_version4_FakeBkg_HFC_TypeI_EE_SignalElectronFake_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);

    //  0.976461138531: ['0.125', '0.139', 'BDTG_version5_FakeBkg_TypeI_EE_SignalElectronFake_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //  0.979605314702: ['0.254', '0.209', 'BDTG_version5_FakeBkgTop_TypeI_EE_SignalElectronFake_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //  0.980518631458: ['0.339', '0.15',  'BDTG_version5_FakeBkg_LF_TypeI_EE_SignalElectronFake_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 


    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_BB_Fake"  ,MVAPathFakev5a+"BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_BB_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Top_BB_Fake" ,MVAPathFakev5a+"BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_BB_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_LF_BB_Fake"  ,MVAPathFakev5a+"BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_BB_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HF_BB_Fake"  ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_BB_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFB_BB_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_BB_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFC_BB_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_BB_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);

    //----> 0.979606996988: ['0.779', '0.551', 'BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_BB_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.982087022796: ['0.741', '0.236', 'BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_BB_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.982137700661: ['0.22', '0.444', 'BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_BB_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.987488943063: ['0.597', '0.445', 'BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_BB_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.988714985006: ['0.582', '0.123', 'BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_BB_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.979900644334: ['0.595', '0.163', 'BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_BB_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 


    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_EC_Fake"  ,MVAPathFakev5a+"BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_EC_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_Top_EC_Fake" ,MVAPathFakev5a+"BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_EC_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_LF_EC_Fake"  ,MVAPathFakev5a+"BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_EC_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HF_EC_Fake"  ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_EC_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFB_EC_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_EC_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.5_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
    ElectronIDv4_FakeMVAReader->BookMVA("BDTGv4_HFC_EC_Fake" ,MVAPathFakev5b+"BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_EC_2018_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT"+xmlpf);

    //----> 0.963927708661: ['0.133', '0.318', 'BDTG_version6_FakeBkg_TypeI_EE_SignalElectronFake_EC_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
    //----> 0.969841629299: ['0.136', '0.281', 'BDTG_version6_FakeBkgTop_TypeI_EE_SignalElectronFake_EC_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root']
    //----> 0.971429977072: ['0.137', '0.46',  'BDTG_version6_FakeBkg_LF_TypeI_EE_SignalElectronFake_EC_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.97553488121: ['0.145', '0.593', 'BDTG_version6_FakeBkg_HF_TypeI_EE_SignalElectronFake_EC_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_3_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.977575778164: ['0.132', '0.569', 'BDTG_version6_FakeBkg_HFB_TypeI_EE_SignalElectronFake_EC_2018_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.5_BaggedFrac_0.8_Seed_100_BDT.root'] 
    //----> 0.969351834758: ['0.714', '0.131', 'BDTG_version6_FakeBkg_HFC_TypeI_EE_SignalElectronFake_EC_2018_NTrees1500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_200_BDT.root'] 
  }

  /// CONV ID BDT    
  if(GetYear() == 2016)   ElectronIDv4_ConvMVAReader->BookMVA("BDTGv4_Conv",MVAPathConvv2+"BDTG_version3_Conv_TypeI_EE_SignalConv_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
  if(GetYear() == 2017)   ElectronIDv4_ConvMVAReader->BookMVA("BDTGv4_Conv",MVAPathConvv2+"BDTG_version3_Conv_TypeI_EE_SignalConv_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.5_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
  if(GetYear() == 2018)   ElectronIDv4_ConvMVAReader->BookMVA("BDTGv4_Conv",MVAPathConvv2+"BDTG_version3_Conv_TypeI_EE_SignalConv_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);

  // AUC              KS_signal   KS_bkg      File    
  // 0.964866659147: ['0.503', '0.127', 'BDTG_version3_Conv_TypeI_EE_SignalConv_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
  // 0.96530088056:  ['0.504', '0.107', 'BDTG_version3_Conv_TypeI_EE_SignalConv_2017_NTrees500_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.5_BaggedFrac_0.8_Seed_100_BDT.root'] 
  // 0.966752119434: ['0.162', '0.214', 'BDTG_version3_Conv_TypeI_EE_SignalConv_2018_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 


  if(GetYear() == 2016)   ElectronIDv4_ConvMVAReader->BookMVA("BDTGv4_BB_Conv",MVAPathConvv3+"BDTG_version6_Conv_TypeI_EE_SignalConv_BB_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
  if(GetYear() == 2017)   ElectronIDv4_ConvMVAReader->BookMVA("BDTGv4_BB_Conv",MVAPathConvv3+"BDTG_version6_Conv_TypeI_EE_SignalConv_BB_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);
  if(GetYear() == 2018)   ElectronIDv4_ConvMVAReader->BookMVA("BDTGv4_BB_Conv",MVAPathConvv3+"BDTG_version6_Conv_TypeI_EE_SignalConv_BB_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT"+xmlpf);


  //0.967854971214: ['0.275', '0.15', 'BDTG_version6_Conv_TypeI_EE_SignalConv_BB_2016_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
  //0.966308164128: ['0.507', '0.14', 'BDTG_version6_Conv_TypeI_EE_SignalConv_BB_2017_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 
  //0.961272507148: ['0.373', '0.118', 'BDTG_version6_Conv_TypeI_EE_SignalConv_BB_2018_NTrees1000_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.5_Seed_100_BDT.root'] 

  if(GetYear() == 2016)   ElectronIDv4_ConvMVAReader->BookMVA("BDTGv4_EC_Conv",MVAPathConvv3+"BDTG_version6_Conv_TypeI_EE_SignalConv_EC_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
  if(GetYear() == 2017)   ElectronIDv4_ConvMVAReader->BookMVA("BDTGv4_EC_Conv",MVAPathConvv3+"BDTG_version6_Conv_TypeI_EE_SignalConv_EC_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);
  if(GetYear() == 2018)   ElectronIDv4_ConvMVAReader->BookMVA("BDTGv4_EC_Conv",MVAPathConvv3+"BDTG_version6_Conv_TypeI_EE_SignalConv_EC_2018_NTrees300_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.5_BaggedFrac_0.8_Seed_100_BDT"+xmlpf);

  //0.939642286898: ['0.395', '0.119', 'BDTG_version6_Conv_TypeI_EE_SignalConv_EC_2016_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_5_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
  //0.956053445237: ['0.789', '0.117', 'BDTG_version6_Conv_TypeI_EE_SignalConv_EC_2017_NTrees2000_NormMode_EqualNumEvents_MinNodeSize_5.0_MaxDepth_4_nCuts_300_Shrinkage_0.05_BaggedFrac_0.8_Seed_100_BDT.root'] 
  //0.951211013251: ['0.112', '0.452', 'BDTG_version6_Conv_TypeI_EE_SignalConv_EC_2018_NTrees300_NormMode_EqualNumEvents_MinNodeSize_2.5_MaxDepth_2_nCuts_300_Shrinkage_0.5_BaggedFrac_0.8_Seed_100_BDT.root'] 
  return;


}





void AnalyzerCore::SetBDTIDVariablesMuon(Muon mu){

  bdt_id_MVA = mu.MVA();

  if (JetLeptonPtRelLepAware(mu) >600) bdt_id_PtRel = 600;
  else bdt_id_PtRel=JetLeptonPtRelLepAware(mu);
  bdt_id_PtRatio=JetLeptonPtRatioLepAware(mu);

  if(mu.Chi2() > 100) bdt_id_Chi2=100;
  else bdt_id_Chi2 = mu.Chi2();
  bdt_id_Validhits = float(mu.ValidMuonHits());
  bdt_id_Matched_stations = float(mu.MatchedStations());
  bdt_id_Pixel_hits  = float(mu.PixelHits());
  bdt_id_Tracker_layers = float(mu.TrackerLayers());

  bdt_id_POGTight = mu.isPOGTight() ?  1.: 0.;
  bdt_id_POGMedium = mu.isPOGMedium() ?  1.: 0.;
  bdt_id_HNTightID = mu.PassID("HNTightV2") ?  1.: 0.;

  return;

}




void AnalyzerCore::SetBDTIDVariablesElectronOLD(Electron el){

  bdt_id_MVA    = el.MVANoIsoResponse();
  bdt_id_MVAIso    = el.MVAIsoResponse();
  bdt_id_RelMVA    = el.MVANoIsoResponse()/el.Pt();
  bdt_id_RelMVAIso    = el.MVAIsoResponse()/el.Pt();
  bdt_id_MissingHits = el.NMissingHits();
  bdt_id_Full5x5_sigmaIetaIeta  = el.Full5x5_sigmaIetaIeta();
  bdt_id_dEtaSeed  = el.dEtaSeed();
  if(bdt_id_dEtaSeed > 0.2) bdt_id_dEtaSeed = 0.2;
  if(bdt_id_dEtaSeed < -0.2) bdt_id_dEtaSeed= -0.2;

  bdt_id_dPhiIn  = el.dPhiIn();
  if(bdt_id_dPhiIn > 0.6) bdt_id_dPhiIn = 0.6;
  if(bdt_id_dPhiIn < -0.6) bdt_id_dPhiIn = -0.6;

  bdt_id_dEtaIn  = el.dEtaIn();
  if(bdt_id_dEtaIn > 0.1) bdt_id_dEtaIn=  0.1;
  if(bdt_id_dEtaIn < -0.1) bdt_id_dEtaIn=  -0.1;

  bdt_id_EtaWidth = el.EtaWidth();
  bdt_id_PhiWidth = el.PhiWidth();
  bdt_id_e2x5OverE5x5 = el.e2x5OverE5x5();
  bdt_id_e1x5OverE5x5= el.e1x5OverE5x5();
  bdt_id_e15= el.e15();
  bdt_id_e25= el.e25();
  bdt_id_e55= el.e55();
  bdt_id_dr03EcalRecHitSumEt= el.dr03EcalRecHitSumEt()/el.UncorrPt();
  bdt_id_dr03HcalDepth1TowerSumEt= el.dr03HcalDepth1TowerSumEt()/el.UncorrPt();
  bdt_id_dr03HcalTowerSumEt= el.dr03HcalTowerSumEt()/el.UncorrPt();
  bdt_id_dr03TkSumPt= el.dr03TkSumPt()/el.UncorrPt();
  bdt_id_R9= el.R9();
  if(bdt_id_R9 > 5) bdt_id_R9 = 5;

  bdt_id_HoverE  = el.HoverE();
  if (el.TrkIso()/el.UncorrPt() > 10) bdt_id_TrkIso = 10;
  else bdt_id_TrkIso  = el.TrkIso()/el.UncorrPt();
  if(isinf(el.InvEminusInvP())) bdt_id_InvEminusInvP = 0.;
  else bdt_id_InvEminusInvP = fabs(el.InvEminusInvP());

  bdt_id_ecalPFClusterIso = el.ecalPFClusterIso()/el.UncorrPt();
  bdt_id_hcalPFClusterIso = el.hcalPFClusterIso()/el.UncorrPt();
  bdt_id_isEcalDriven = (el.isEcalDriven()) ? 1. : 0.;
  bdt_id_EoverP = log(el.EOverP());
  if(bdt_id_EoverP > 20) bdt_id_EoverP = 20;
  if(bdt_id_EoverP < -20) bdt_id_EoverP = -20;

  if (el.FBrem() < -1) bdt_id_FBrem = -1;
  else   bdt_id_FBrem = el.FBrem();

  bdt_id_PassConversionVeto  = (el.PassConversionVeto()) ? 1. : 0.;
  bdt_id_IsGsfCtfScPixChargeConsistent  = (el.IsGsfCtfScPixChargeConsistent()) ? 1. : 0.;
  bdt_id_IsGsfScPixChargeConsistent  = (el.IsGsfScPixChargeConsistent()) ? 1. : 0.;
  bdt_id_IsGsfCtfChargeConsistent  = (el.IsGsfCtfChargeConsistent()) ? 1. : 0.;

  bdt_id_PtRatio=JetLeptonPtRatioLepAware(el);

  if (JetLeptonPtRelLepAware(el) > 600)bdt_id_PtRel = 600;
  else bdt_id_PtRel=JetLeptonPtRelLepAware(el);

  bdt_id_POGTight = el.PassID("passPOGTight") ?  1.: 0.;
  bdt_id_POGMedium =el.PassID("passPOGMedium") ?  1.: 0.;
  bdt_id_HNTightID = el.PassID("HNTightV2") ?  1.: 0.;


}





void AnalyzerCore::SetBDTIDVariablesElectron(Electron el){

  //// MVA Values  Response used as POG cut uses rrsponse not value                                                                                                                                             
  bdt_id_MVA       = el.MVANoIsoResponse();
  bdt_id_MVAIso    = el.MVAIsoResponse();

  bdt_id_MVARaw    = el.MVANoIso();
  bdt_id_MVAIsoRaw = el.MVAIso();

  ///// https://github.com/cms-sw/cmssw/blob/CMSSW_10_6_25/RecoEgamma/ElectronIdentification/data/ElectronMVAEstimatorRun2Variables.txt  

  bdt_id_MissingHits           = el.NMissingHits();
  bdt_id_Full5x5_sigmaIetaIeta = el.Full5x5_sigmaIetaIeta();

  bdt_id_dEtaSeed          = fabs(el.dEtaSeed());
  if(bdt_id_dEtaSeed > 0.2) bdt_id_dEtaSeed = 0.2;

  bdt_id_dPhiIn  = fabs(el.dPhiIn());
  if(bdt_id_dPhiIn > 0.6) bdt_id_dPhiIn = 0.6;

  bdt_id_dEtaIn  = fabs(el.dEtaIn());
  if(bdt_id_dEtaIn > 0.06) bdt_id_dEtaIn=  0.06;   /// 0.06 

  bdt_id_EtaWidth = el.EtaWidth();
  bdt_id_PhiWidth = el.PhiWidth();

  bdt_id_e2x5OverE5x5 = el.e2x5OverE5x5();
  bdt_id_e1x5OverE5x5= (el.e55()) !=0. ? 1.-(el.e15()/el.e55()) : -1. ;
  if(bdt_id_e1x5OverE5x5 < -1) bdt_id_e1x5OverE5x5=-1;
  if(bdt_id_e1x5OverE5x5 > 2) bdt_id_e1x5OverE5x5=2;

  bdt_id_e55= el.e55();

  bdt_id_dr03HcalTowerSumEt= el.dr03HcalTowerSumEt()/el.UncorrPt();
  bdt_id_dr03TkSumPt= el.dr03TkSumPt()/el.UncorrPt();
  bdt_id_R9= el.R9();
  if(bdt_id_R9 > 5) bdt_id_R9 = 5;


  bdt_id_psEoRraw= el.psEoverEraw();
  if (bdt_id_psEoRraw > 1) bdt_id_psEoRraw=1;

  bdt_id_HoverE  = el.HoverE();

  if (el.TrkIso()/el.UncorrPt() > 10) bdt_id_TrkIso = 10;
  else bdt_id_TrkIso  = el.TrkIso()/el.UncorrPt();

  if(isinf(el.InvEminusInvP())) bdt_id_InvEminusInvP = 0.;
  else bdt_id_InvEminusInvP = fabs(el.InvEminusInvP());

  bdt_id_ecalPFClusterIso = el.ecalPFClusterIso()/el.UncorrPt();
  bdt_id_hcalPFClusterIso = el.hcalPFClusterIso()/el.UncorrPt();

  bdt_id_isEcalDriven = (el.isEcalDriven()) ? 1. : 0.;

  if(el.EOverP() > 20) bdt_id_EoverP = log(20);
  else bdt_id_EoverP = log(el.EOverP());  //// 20 then log?  

  if (el.FBrem() < -1) bdt_id_FBrem = -1;
  else   bdt_id_FBrem = el.FBrem();

  bdt_id_Rho=el.Rho();
  bdt_id_PassConversionVeto  = (el.PassConversionVeto()) ? 1. : 0.;

  bdt_id_IsGsfCtfScPixChargeConsistent  = (el.IsGsfCtfScPixChargeConsistent()) ? 1. : 0.;
  bdt_id_IsGsfScPixChargeConsistent  = (el.IsGsfScPixChargeConsistent()) ? 1. : 0.;
  bdt_id_IsGsfCtfChargeConsistent  = (el.IsGsfCtfChargeConsistent()) ? 1. : 0.;

  bdt_id_PtRatio=JetLeptonPtRatioLepAware(el);
  bdt_id_PtRatioV2=JetLeptonPtRatioLepAwareV2(el);
  bdt_id_PtRatioV3 = (el.IsBB()) ?  bdt_id_PtRatio : bdt_id_PtRatioV2;

  if (JetLeptonPtRelLepAware(el) > 600)bdt_id_PtRel = 600;
  else bdt_id_PtRel=JetLeptonPtRelLepAware(el);

  if (JetLeptonPtRelLepAwareV2(el) > 600)bdt_id_PtRelV2 = 600;
  else bdt_id_PtRelV2=JetLeptonPtRelLepAwareV2(el);

  bdt_id_PtRelV3 = (el.IsBB()) ? bdt_id_PtRel : bdt_id_PtRelV2;

  bdt_id_POGTight = el.PassID("passPOGTight") ?  1.: 0.;
  bdt_id_POGMedium =el.PassID("passPOGMedium") ?  1.: 0.;
  bdt_id_HNTightID = el.PassID("HNTightV2") ?  1.: 0.;
  bdt_id_POGMVA80ID = el.passMVAID_noIso_WP80() ? 1.: 0.;
  bdt_id_POGMVA90ID = el.passMVAID_noIso_WP90() ? 1.: 0.;
  return;
}



double AnalyzerCore::GetBDTScoreMuon_EtaDependant(Muon mu ,BkgType bkg, TString BDTTag){

  TString  MVATagStr = BDTTag;
  if(MVATagStr.Contains("v4"))  {
    if(!iSetupLeptonBDTv4) return -9999;
  }
  if(MVATagStr.Contains("v5"))  {
    if(!iSetupLeptonBDTv5) return -9999;
  }
  
  int Version=1;
  if(MVATagStr.Contains("BDTGv4"))Version=4;
  if(MVATagStr.Contains("BDTGv5"))Version=5;

  InitializeIDTreeVars();

  if(bkg != BkgType::Fake) return 1.;

  Lepton *lep = (Lepton *)(&mu);
  if(Version < 5) {
    SetBDTIDVarOLD(lep);
    SetBDTIDVariablesMuon(mu);
    if(mu.IsBB())    MVATagStr += "_BB";
    else  MVATagStr += "_EC";

  }
  else{
    SetBDTIDVar(lep);
    SetBDTIDVariablesMuon(mu);

    if(mu.IsEC() )  MVATagStr += "_EC";
  }

  if (bkg == BkgType::Fake)   MVATagStr += "_Fake";

  if(MVATagStr.Contains("v4"))   return  MuonIDv4_FakeMVAReader->EvaluateMVA(MVATagStr);
  if(MVATagStr.Contains("v5"))   return  MuonIDv5_FakeMVAReader->EvaluateMVA(MVATagStr);

  return -9999.;
}



double AnalyzerCore::GetBDTScoreEl_EtaDependant(Electron el ,BkgType bkg, TString BDTTag){


  TString  MVATagStr = BDTTag;

  if(MVATagStr.Contains("BDTGv4")){
    if(!iSetupLeptonBDTv4) return -9999;
  }
  if(MVATagStr.Contains("BDTGv5")){
    if(!iSetupLeptonBDTv5) return -9999;
  }

  int Version=1;
  if(MVATagStr.Contains("BDTGv4"))Version=4;
  if(MVATagStr.Contains("BDTGv5"))Version=5;

  /// Version 5 now sets BB to inclusive and EC to EC  

  InitializeIDTreeVars();

  Lepton *lep = (Lepton *)(&el);
  if(Version < 5) {
    SetBDTIDVarOLD(lep);
    SetBDTIDVariablesElectronOLD(el);

    if(el.IsBB())    MVATagStr += "_BB";
    else  MVATagStr += "_EC";
  }
  if(Version == 5) {
    SetBDTIDVar(lep);
    SetBDTIDVariablesElectron(el);

    if(el.IsBB())    MVATagStr += "_BB";
    if(el.IsEC())  MVATagStr += "_EC";
  }

  if (bkg == BkgType::CF)     MVATagStr += "_CF";
  if (bkg == BkgType::Conv)   MVATagStr += "_Conv";
  if (bkg == BkgType::Fake)   MVATagStr += "_Fake";


  if(MVATagStr.Contains("BDTGv4")){
    
    //{   cout << "iSetupLeptonBDTv2 not set.." << MVATagStr<< endl;     exit(EXIT_FAILURE);}
    
    if (bkg == BkgType::CF)    return  ElectronIDv4_CFMVAReader->EvaluateMVA(MVATagStr);

    if (bkg == BkgType::Conv) return  ElectronIDv4_ConvMVAReader->EvaluateMVA(MVATagStr);

    if (bkg == BkgType::Fake){
      TString MVATagStrTMP = MVATagStr;
      return  ElectronIDv4_FakeMVAReader->EvaluateMVA(MVATagStrTMP);
    }
  }

  if(MVATagStr.Contains("BDTGv5")){
    if(!iSetupLeptonBDTv5) {   cout << "iSetupLeptonBDTv5 not set.." << MVATagStr<< endl;     exit(EXIT_FAILURE);}

    if(MVATagStr.Contains("BDTGv5_"))      return  ElectronIDv5_MVAReader->EvaluateMVA(MVATagStr);

    if (bkg == BkgType::CF) {
      if(MVATagStr.Contains("Pt"))  return  ElectronIDv5_CFMVAReader->EvaluateMVA(MVATagStr);
    }
  }

  return -9999.;
}




/// Original BDT functions 
double AnalyzerCore::GetBDTScoreElV1(Electron el ,BkgType bkg, TString BDTTag){

  InitializeElectronIDTreeVars();

  std::vector<Jet>   JetAllColl = GetJets("NoID", 10., 5.0);

  Lepton *lep = (Lepton *)(&el);
  SetBDTIDVarV1(lep);

  bdt_id_MVA    = el.MVANoIsoResponseV1();
  bdt_id_MVAIso    = el.MVAIsoResponseV1();
  bdt_id_MissingHits = el.NMissingHits();
  bdt_id_Full5x5_sigmaIetaIeta  = el.Full5x5_sigmaIetaIeta();  //sigmaIetaIeta  = el.sigmaIetaIeta();
  bdt_id_dEtaSeed  = el.dEtaSeed();
  bdt_id_dPhiIn  = el.dPhiIn();//  dEtaIn  = el.dEtaIn();    
  bdt_id_EtaWidth = el.EtaWidth();
  bdt_id_PhiWidth = el.PhiWidth();
  bdt_id_e2x5OverE5x5 = el.e2x5OverE5x5();
  bdt_id_e55= el.e55();

  bdt_id_dr03HcalTowerSumEt= el.dr03HcalTowerSumEt()/el.UncorrPt();
  bdt_id_dr03TkSumPt= el.dr03TkSumPt()/el.UncorrPt();
  bdt_id_R9= el.R9();
  bdt_id_HoverE  = el.HoverE();
  bdt_id_TrkIso  = el.TrkIso()/el.UncorrPt();  //InvEminusInvP = fabs(el.InvEminusInvP());
  bdt_id_ecalPFClusterIso = el.ecalPFClusterIso()/el.UncorrPt();
  bdt_id_hcalPFClusterIso = el.hcalPFClusterIso()/el.UncorrPt();

  bdt_id_isEcalDriven = el.isEcalDriven();

  bdt_id_EoverP = log(el.EOverP());

  bdt_id_FBrem = el.FBrem();

  bdt_id_PassConversionVeto  = (el.PassConversionVeto()) ? 1 : 0;
  bdt_id_IsGsfCtfScPixChargeConsistent  = (el.IsGsfCtfScPixChargeConsistent()) ? 1 : 0;
  bdt_id_IsGsfScPixChargeConsistent  = (el.IsGsfScPixChargeConsistent()) ? 1 : 0;
  bdt_id_IsGsfCtfChargeConsistent  = (el.IsGsfCtfChargeConsistent()) ? 1 : 0;


  //==== Vars for non-prompt lepton bkg   
  int IdxMatchJet=-1;
  float mindR1=999.;

  for(unsigned int ij=0; ij<JetAllColl.size(); ij++){
    float dR1=lep->DeltaR(JetAllColl.at(ij));
    if(dR1>0.4) continue;
    if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
  }

  if(IdxMatchJet!=-1){
    bdt_id_PtRatio=JetLeptonPtRatioLepAware(el);
    bdt_id_PtRel=JetLeptonPtRelLepAware(el);
  }

  else{
    bdt_id_PtRatio = min(1/(1.+lep->RelIso()), 1.5);
    bdt_id_PtRel=0;
  }

  TString MVATagStr="";
  if(BDTTag.Contains("v2")){

    MVATagStr = BDTTag;
    MVATagStr=MVATagStr.ReplaceAll("v2","");

    if(el.IsBB())    MVATagStr += "_BB";
    else  MVATagStr += "_EC";

    if (bkg == BkgType::Fake) MVATagStr += "_Fake";
    if (bkg == BkgType::Conv) MVATagStr += "_Conv";

  }
  else{
    MVATagStr = BDTTag;
    if (bkg == BkgType::Fake) MVATagStr += "_Fake";
    if (bkg == BkgType::Conv) MVATagStr += "_Conv";
    if (bkg == BkgType::CF){
      if(el.IsBB())   MVATagStr += "_BB";
      else     MVATagStr += "_EC";
      MVATagStr += "_CF";
    }
  }


  return -999;
}




double AnalyzerCore::GetBDTScoreEl(Electron el ,BkgType bkg, TString BDTTag){

  TString  MVATagStr = BDTTag;

  if(MVATagStr.Contains("BDTGv4")){
    if(!iSetupLeptonBDTv4) return -999;
  }
  if(MVATagStr.Contains("BDTGv5")){
    if(!iSetupLeptonBDTv5) return -999;
  }
  int Version=1;
  if(MVATagStr.Contains("BDTGv4"))Version=4;
  if(MVATagStr.Contains("BDTGv5"))Version=5;

  InitializeIDTreeVars();

  Lepton *lep = (Lepton *)(&el);
  if(Version < 5) {
    SetBDTIDVarOLD(lep);
    SetBDTIDVariablesElectronOLD(el);
  }
  if(Version == 5) {
    SetBDTIDVar(lep);
    SetBDTIDVariablesElectron(el);
  }

  if (bkg == BkgType::Fake) MVATagStr += "_Fake";
  if (bkg == BkgType::FakeRate) MVATagStr += "_Fake";
  if (bkg == BkgType::Conv) MVATagStr += "_Conv";
  if (bkg == BkgType::CF)   MVATagStr += "_CF";

  if(MVATagStr.Contains("BDTGv4")){

    if (bkg == BkgType::CF){

      if(MVATagStr.Contains("BDTGv4_"))      return  ElectronIDv4_CFMVAReader->EvaluateMVA(MVATagStr);
    }

    if (bkg == BkgType::Fake) {
      TString MVATagStrTMP = MVATagStr;
      //      MVATagStrTMP=MVATagStrTMP.ReplaceAll("BDTGv4_HF","BDTGv3_HF"); //// TEST 
      return  ElectronIDv4_FakeMVAReader->EvaluateMVA(MVATagStrTMP);
    }

    if (bkg == BkgType::Conv) return  ElectronIDv4_ConvMVAReader->EvaluateMVA(MVATagStr);
  }


  if(MVATagStr.Contains("BDTGv5")){

    if(!iSetupLeptonBDTv5) {   cout << "iSetupLeptonBDTv5 not set.." << MVATagStr<< endl;     exit(EXIT_FAILURE);}

    if (bkg == BkgType::FakeRate) return  ElectronIDv5_FakeMVAReader->EvaluateMVA(MVATagStr);

    if(MVATagStr.Contains("BDTGv5_"))  return  ElectronIDv5_MVAReader->EvaluateMVA(MVATagStr);
    if(MVATagStr.Contains("Pt"))       return  ElectronIDv5_CFMVAReader->EvaluateMVA(MVATagStr);


    cout << "--> ElectronIDv5_FakeMVAReader " << MVATagStr << endl;
  }


  return -9999.;
}



void AnalyzerCore::CompareBDT(TString var1, double a, double b,  int ilep ){

  if( (a/b) != 1 ) cout << "Electron " << ilep << " " << var1 << " " <<  a << "   : " << b <<  "  a/b = " << a/b << endl;


  return;
}




void AnalyzerCore::SetupLeptonBDTSKFlatV5(){

  std::vector<Muon>     AllmuonColl     = All_Muons;
  std::vector<Electron> AllelectronColl = All_Electrons;
  std::vector<Jet>      AK4_JetAllColl  = All_Jets;


  for(auto i: AllmuonColl)   {

    float  JetDiscCJ_CvsB = -999;
    float  JetDiscCJ_CvsL = -999;
    float  JetDiscCJ = -999;

    int JetFlavourCJ=-999;
    int IdxMatchJet=-1;
    float mindR1=999.;

    for(unsigned int ij=0; ij<AK4_JetAllColl.size(); ij++){
      float dR1=i.DeltaR(AK4_JetAllColl.at(ij));
      if(dR1>0.4) continue;
      if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
    }
    if(IdxMatchJet!=-1) {
      JetDiscCJ_CvsB   = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet_CvsB);
      JetDiscCJ_CvsL    = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet_CvsL);
      JetDiscCJ = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet);
      JetFlavourCJ = AK4_JetAllColl.at(IdxMatchJet).hadronFlavour();

    }
    else {
      JetDiscCJ_CvsB=0.4;
      JetDiscCJ_CvsL=0.1;
      JetDiscCJ=0.1;
      JetFlavourCJ=0;
    }
    vmuon_v2_cj_cvsbjetdisc->push_back(JetDiscCJ_CvsB);
    vmuon_v2_cj_cvsljetdisc->push_back(JetDiscCJ_CvsL);
    vmuon_v2_cj_bjetdisc->push_back(JetDiscCJ);
    vmuon_v2_cj_flavour->push_back(JetFlavourCJ);

    vmuon_mva_fake_QCD_LFvsHF_v5->push_back(GetBDTScoreMuon(i,AnalyzerCore::FakeRate,     "BDTGv5_QCD_LFvsHF"));
    vmuon_mva_fake_QCD_HFBvsHFC_v5->push_back(GetBDTScoreMuon(i,AnalyzerCore::FakeRate,   "BDTGv5_QCD_HFBvsHFC"));
    vmuon_mva_fake_QCD_LF1_v5->push_back(GetBDTScoreMuon(i,AnalyzerCore::FakeRate,  "BDTGv5_LF1"));
    vmuon_mva_fake_QCD_LF2_v5->push_back(GetBDTScoreMuon(i,AnalyzerCore::FakeRate,  "BDTGv5_LF2"));

  }




  for(auto i: AllelectronColl){

    velectron_mva_fake_ed_v5->push_back(GetBDTScoreEl_EtaDependant(i,AnalyzerCore::Fake,  "BDTGv5"));
    velectron_mva_fakeHFB_v5->push_back(GetBDTScoreEl(i,AnalyzerCore::Fake,  "BDTGv5_HFB"));
    velectron_mva_fakeHFC_v5->push_back(GetBDTScoreEl(i,AnalyzerCore::Fake,  "BDTGv5_HFC"));
    velectron_mva_fakeLF_v5->push_back(GetBDTScoreEl(i,AnalyzerCore::Fake,  "BDTGv5_LF"));

    velectron_mva_conv_ed_v5->push_back(GetBDTScoreEl_EtaDependant(i,AnalyzerCore::Conv,  "BDTGv5"));

    velectron_mva_cf_ed_v5->push_back(GetBDTScoreEl_EtaDependant(i,AnalyzerCore::CF,  "BDTGv5"));
    velectron_mva_cf_ed_v5pt->push_back(GetBDTScoreEl_EtaDependant(i,AnalyzerCore::CF,  "BDTGv5Pt"));

    velectron_mva_fake_QCD_LFvsHF_v5->push_back(GetBDTScoreEl(i,AnalyzerCore::FakeRate,    "BDTGv5_QCD_LFvsHF"));
    velectron_mva_fake_QCD_HFBvsHFC_v5->push_back(GetBDTScoreEl(i,AnalyzerCore::FakeRate,  "BDTGv5_QCD_HFBvsHFC"));
    velectron_mva_fake_QCD_LF1_v5->push_back(GetBDTScoreEl(i,AnalyzerCore::FakeRate,  "BDTGv5_LF1"));
    velectron_mva_fake_QCD_LF2_v5->push_back(GetBDTScoreEl(i,AnalyzerCore::FakeRate,  "BDTGv5_LF2"));


    float  JetDiscCJ_CvsB = -999;
    float  JetDiscCJ_CvsL = -999;
    float  JetDiscCJ = -999;

    int JetFlavourCJ=-999;
    int IdxMatchJet=-1;
    float mindR1=999.;

    for(unsigned int ij=0; ij<AK4_JetAllColl.size(); ij++){
      float dR1=i.DeltaR(AK4_JetAllColl.at(ij));
      if(dR1>0.4) continue;
      if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
    }
    if(IdxMatchJet!=-1) {
      JetDiscCJ_CvsB   = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet_CvsB);
      JetDiscCJ_CvsL    = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet_CvsL);
      JetDiscCJ = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet);
      JetFlavourCJ = AK4_JetAllColl.at(IdxMatchJet).hadronFlavour();

    }
    else {
      JetDiscCJ_CvsB=0.4;
      JetDiscCJ_CvsL=0.1;
      JetDiscCJ=0.1;
      JetFlavourCJ=0;
    }
    velectron_v2_cj_cvsbjetdisc->push_back(JetDiscCJ_CvsB);
    velectron_v2_cj_cvsljetdisc->push_back(JetDiscCJ_CvsL);
    velectron_v2_cj_bjetdisc->push_back(JetDiscCJ);
    velectron_v2_cj_flavour->push_back(JetFlavourCJ);
  }

  return;
}



void AnalyzerCore::SetupLeptonBDTSKFlat(){

  /// Originally used GetALl* which orders pt,                                                                                                                                                                                                                                                                                                                                                                                           
  /// Then if order changes the pushed vector has a mis match in lepton index                                                                                                                                                                                                                                                                                                                                                    

  std::vector<Muon>     AllmuonColl     = All_Muons;
  std::vector<Electron> AllelectronColl = All_Electrons;
  std::vector<Jet>      AK4_JetAllColl  = All_Jets;

  for(auto i: AllmuonColl){
    vmuon_ptratio->push_back(JetLeptonPtRatioLepAware(i));
    vmuon_ptrel->push_back(JetLeptonPtRelLepAware(i));
    vmuon_lepton_type->push_back(GetLeptonType_JH(i, All_Gens));
  }

  for(auto i: AllelectronColl){
    velectron_ptratio->push_back(JetLeptonPtRatioLepAware(i));
    velectron_ptrel->push_back(JetLeptonPtRelLepAware(i));
    velectron_lepton_type->push_back(GetLeptonType_JH(i, All_Gens));
  }

  return;


}

void AnalyzerCore::ResetLeptonBDTSKFlatV5(){

  ///// Reset V5 variables to add to BDT skim                                                                                                                                                                                                                                                                                                                                                                                            
  ///// No overlap with V4                                                                                                                                                                                                                                                                                                                                                                                                               
  //cout << "V5 variables (some removed from original BDT skim code     "<< endl;                                                                                                                                                                                                                                                                                                                                                        

  velectron_v2_cj_bjetdisc->clear();
  velectron_v2_cj_cvsbjetdisc->clear();
  velectron_v2_cj_cvsljetdisc->clear();
  velectron_v2_cj_flavour->clear();

  velectron_mva_cf_ed_v5->clear();
  velectron_mva_cf_ed_v5pt->clear();
  velectron_mva_conv_ed_v5->clear();
  velectron_mva_fake_ed_v5->clear();
  velectron_mva_fakeHFB_v5->clear();
  velectron_mva_fakeHFC_v5->clear();
  velectron_mva_fakeLF_v5->clear();
  velectron_mva_fake_QCD_LFvsHF_v5->clear();
  velectron_mva_fake_QCD_HFBvsHFC_v5->clear();
  velectron_mva_fake_QCD_LF1_v5->clear();
  velectron_mva_fake_QCD_LF2_v5->clear();

  vmuon_v2_cj_bjetdisc->clear();
  vmuon_v2_cj_cvsbjetdisc->clear();
  vmuon_v2_cj_cvsljetdisc->clear();
  vmuon_v2_cj_flavour->clear();

  vmuon_mva_fake_QCD_LFvsHF_v5->clear();
  vmuon_mva_fake_QCD_HFBvsHFC_v5->clear();
  vmuon_mva_fake_QCD_LF1_v5->clear();
  vmuon_mva_fake_QCD_LF2_v5->clear();

  //cout << "END V5 variables (some removed from original BDT skim code     "<< endl;                                                                                                                                                                                                                                                                                                                                                    

  return;
}




void AnalyzerCore::ResetLeptonBDTSKFlat(){

  velectron_ptrel->clear();
  velectron_ptratio->clear();
  vmuon_ptrel->clear();
  vmuon_ptratio->clear();
  velectron_lepton_type->clear();
  vmuon_lepton_type->clear();

  //  cout << "END V4 variables (some removed from original BDT skim code     "<< endl;                                                                                                                                                                                                                                                                                                                                            

  return;
}

void AnalyzerCore::InitialiseLeptonBDTSKFlat(){

  vSKWeight=0;

  velectron_ptratio = 0;
  velectron_ptrel  = 0;
  velectron_v2_cj_bjetdisc = 0;
  velectron_v2_cj_cvsbjetdisc = 0;
  velectron_v2_cj_cvsljetdisc = 0;
  velectron_v2_cj_flavour = 0;
  velectron_lepton_type=0;

  /// ELECTRON 
  ///// BDT V5                                                                                                                                                                                                                                                                                                                                                                                                                        
  velectron_mva_fake_ed_v5 = 0 ;
  velectron_mva_fakeHFB_v5 = 0 ;
  velectron_mva_fakeHFC_v5 = 0 ;
  velectron_mva_fakeLF_v5 = 0 ;
  velectron_mva_fake_QCD_LFvsHF_v5 = 0 ;
  velectron_mva_fake_QCD_HFBvsHFC_v5 = 0 ;
  velectron_mva_conv_ed_v5 = 0;
  velectron_mva_cf_ed_v5 = 0;
  velectron_mva_cf_ed_v5pt = 0;
  velectron_mva_fake_QCD_LF1_v5 = 0 ;
  velectron_mva_fake_QCD_LF2_v5 = 0 ;



  /// MUON                                                                                                                                                                                                           ///// BDT V5                                                                                                                                                                                                     

  vmuon_mva_fake_QCD_LFvsHF_v5 = 0 ;
  vmuon_mva_fake_QCD_HFBvsHFC_v5 = 0 ;
  vmuon_mva_fake_QCD_LF1_v5 = 0 ;
  vmuon_mva_fake_QCD_LF2_v5 = 0 ;

  vmuon_ptratio = 0;
  vmuon_ptrel  = 0;
  vmuon_v2_cj_bjetdisc = 0;
  vmuon_v2_cj_cvsbjetdisc = 0;
  vmuon_v2_cj_cvsljetdisc = 0;
  vmuon_v2_cj_flavour = 0;
  vmuon_lepton_type=0;

  return;
}



void AnalyzerCore::SetupLeptonBDT( bool version4, bool version5){

  cout << "AnalyzerCore::SetupLeptonBDT version4 = " << version4 << " version5 = " << version5 << endl;

  iSetupLeptonBDTv4=version4;
  iSetupLeptonBDTv5=version5;
  TMVA::Tools::Instance();
  //ElectronIDv3ConvMVAReader = new TMVA::Reader();

  if(version4) SetupLeptonBDTv4();
  if(version5) SetupLeptonBDTv5();
  cout << "AnalyzerCore::SetupLeptonBDT Done " << endl;
  return;
}

void AnalyzerCore::SetupLeptonBDTv5(){

  ElectronIDv5_MVAReader = new TMVA::Reader();
  ElectronIDv5_CFMVAReader = new TMVA::Reader();
  ElectronIDv5_FakeMVAReader  = new TMVA::Reader();
  MuonIDv5_FakeMVAReader = new TMVA::Reader();
}

void AnalyzerCore::SetupLeptonBDTv4(){

  /// Version 2                                                                                                                                                                                                                                                                                                                                                                                                                         

  ElectronIDv4_FakeMVAReader = new TMVA::Reader();
  ElectronIDv4_CFMVAReader   = new TMVA::Reader();
  ElectronIDv4_ConvMVAReader = new TMVA::Reader();
  MuonIDv4_FakeMVAReader = new TMVA::Reader();

  return;
}
