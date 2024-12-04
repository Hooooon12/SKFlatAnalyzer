#include "AnalyzerCore.h"

AnalyzerCore::AnalyzerCore(){

  outfile = NULL;

  //// MUON SCALE OBJ
  muonGE          = new GeneralizedEndpoint();
  muonGEScaleSyst = new GEScaleSyst();

  /// GLABAL VARIABLES
  iSetupLeptonBDTv5=false;
  iSetupLeptonBDTv4=false;
  IsDYSample=false;
  IsTTSample=false;

  /// SETUP PREDEFINED HIST VARIABLE BINNINGS
  SetHistBins();
}

AnalyzerCore::~AnalyzerCore(){

  //=== hist maps
  
  DeleteHistMaps();
  DeleteProfileMaps();

  //==== output rootfile

  if(outfile){
    outfile->Close();    delete outfile;
  }

  //==== Tools

  if(muonGE) delete muonGE;
  if(muonGEScaleSyst) delete muonGEScaleSyst;

  AK4CHSJECUncMap.clear();
  AK4PUPPIJECUncMap.clear();
  AK8CHSJECUncMap.clear();
  AK8PUPPIJECUncMap.clear();
  
  if(iSetupLeptonBDTv5){

    ////// NEW MVA 
    delete ElectronIDv5_MVAReader;
    delete ElectronIDv5_CFMVAReader;
    delete ElectronIDv5_FakeMVAReader;
    delete MuonIDv5_FakeMVAReader;

  }
  if(iSetupLeptonBDTv4){
    delete ElectronIDv4_FakeMVAReader;
    delete ElectronIDv4_CFMVAReader;
    delete ElectronIDv4_ConvMVAReader;
    delete MuonIDv4_FakeMVAReader;
  }

}

void AnalyzerCore::initializeAnalyzer(){

  IsDYSample=false;
  IsTTSample=false;
  if(MCSample.Contains("DYJets")||MCSample.Contains("ZToEE")||MCSample.Contains("ZToMuMu")||MCSample.Contains(TRegexp("DY[0-9]Jets"))) IsDYSample=true;
  if(MCSample.Contains(TRegexp("TT[LJ][LJ]"))) IsTTSample=true;
  if(IsSignal()) IsDYSample=false;

  cout << "AnalyzerCore::initializeAnalyzer IsDYSample=" << IsDYSample << endl;
  cout << "AnalyzerCore::initializeAnalyzer IsTTSample=" << IsTTSample << endl;

  run_Debug=false;  TESTBDT=false;  nLog=50000;

  TimeTagMatcher.clear();  TimerMap.clear();
  TimerMap["LATEST"] = std::clock();
  TimingMap.clear();
  TimingMap["start"] = std::clock();

  JECSources = {"AbsoluteStat","AbsoluteScale","AbsoluteFlavMap","AbsoluteMPFBias","Fragmentation","SinglePionECAL","SinglePionHCAL","FlavorQCD","TimePtEta","RelativeJEREC1","RelativeJEREC2","RelativeJERHF","RelativePtBB","RelativePtEC1","RelativePtEC2","RelativePtHF","RelativeBal","RelativeSample","RelativeFSR","RelativeStatFSR","RelativeStatEC","RelativeStatHF","PileUpDataMC","PileUpPtRef","PileUpPtBB","PileUpPtEC1","PileUpPtEC2","PileUpPtHF","FlavorZJet","FlavorPhotonJet","FlavorPureGluon","FlavorPureQuark","FlavorPureCharm","FlavorPureBottom","Total"};
  

};



//==== Attach the historams to ai different direcotry, not outfile
//==== We will write these histograms in WriteHist() to outfile
void AnalyzerCore::SwitchToTempDir(){

  gROOT->cd();
  TDirectory *tempDir = NULL;
  int counter = 0;
  while (!tempDir) {
    //==== First, let's find a directory name that doesn't exist yet
    std::stringstream dirname;
    dirname << "AnalyzerCore" << counter;
    if (gROOT->GetDirectory((dirname.str()).c_str())) {
      ++counter;
      continue;
    }
    //==== Let's try to make this directory
    tempDir = gROOT->mkdir((dirname.str()).c_str());
  }
  tempDir->cd();

}

void AnalyzerCore::SetOutfilePath(TString outname){
  outfile = new TFile(outname,"RECREATE");
};

Event AnalyzerCore::GetEvent(){

  Event ev;
  ev.SetTrigger(*HLT_TriggerName);
  ev.SetMET(pfMET_Type1_pt,pfMET_Type1_phi);
  ev.SetnPV(nPV);
  ev.SetEra(GetEra());

  return ev;

}


std::vector<Muon> AnalyzerCore::GetAllMuons(){

  std::vector<Muon> out;
  if(!muon_pt) return out;
  for(unsigned int i=0; i<muon_pt->size(); i++){

    Muon mu;
    mu.SetRunYear(DataYear);
    mu.SetRunEra(GetEraShort());
    mu.SetCharge(muon_charge->at(i));
    mu.SetUncorrectedPt(muon_pt->at(i));
    Muon mu_tmp;
    mu_tmp.SetPtEtaPhiM(muon_pt->at(i), muon_eta->at(i), muon_phi->at(i), muon_mass->at(i));
    mu.SetMiniAODPt(muon_pt->at(i));
    mu.SetMiniAODTunePPt(muon_TuneP_pt->at(i));

    double rc = muon_roch_sf->at(i);
    double rc_err = muon_roch_sf_up->at(i)-rc;
    //==== For the Rochester corection, up and down err are the same
    mu.SetMomentumScaleUpDown( muon_pt->at(i) * (rc+rc_err), muon_pt->at(i) * (rc-rc_err)  );
    mu.SetPtEtaPhiM(muon_pt->at(i)*rc, muon_eta->at(i), muon_phi->at(i), muon_mass->at(i));

    //==== TuneP
    //==== Apply scailing later with AnalyzerCore::UseTunePMuon()
    mu.SetTuneP4(muon_TuneP_pt->at(i), muon_TuneP_ptError->at(i), muon_TuneP_eta->at(i), muon_TuneP_phi->at(i), muon_TuneP_charge->at(i));

    mu.SetSoftMVA(muon_softMVA->at(i));
    mu.SetMVA(muon_MVA->at(i));
    mu.SetLepMVA( muon_MVA->at(i));
    
    mu.SetdXY(muon_dxyVTX->at(i), muon_dxyerrVTX->at(i));
    mu.SetdZ(muon_dzVTX->at(i), muon_dzerrVTX->at(i));
    mu.SetIP3D(muon_3DIPVTX->at(i), muon_3DIPerrVTX->at(i));
    mu.SetTypeBit(muon_TypeBit->at(i));
    mu.SetIDBit(muon_IDBit->at(i));
    mu.SetisPOGHighPt(muon_ishighpt->at(i));
    mu.SetPOGMediumHIP(muon_ismedium_hip->at(i),muon_ismedium_nohip->at(i));
    mu.SetChi2(muon_normchi->at(i));
    mu.SetIso(muon_PfChargedHadronIsoR04->at(i),muon_PfNeutralHadronIsoR04->at(i),muon_PfGammaIsoR04->at(i),muon_PFSumPUIsoR04->at(i));
    mu.SetTrkIso(muon_trkiso->at(i));
    mu.SetLepIso(muon_PfChargedHadronIsoR04->at(i),muon_PfNeutralHadronIsoR04->at(i),muon_PfGammaIsoR04->at(i));
   
    mu.SetTrackerLayers(muon_trackerLayers->at(i));

    mu.SetMatchedStations(muon_matchedstations->at(i));
    mu.SetPixelHits(muon_pixelHits->at(i));
    mu.SetValidMuonHits(muon_validmuonhits->at(i));

    //==== Should be set after Eta is set

    mu.SetMiniIso(
      muon_PfChargedHadronMiniIso->at(i), 
      muon_PfNeutralHadronMiniIso->at(i), 
      muon_PfGammaMiniIso->at(i), 
      muon_PFSumPUMiniIso->at(i),
      Rho,
      mu.EA()
    );

    mu.SetFilterBits(muon_filterbits->at(i));
    mu.SetPathBits(muon_pathbits->at(i));
    
    
    ////// FILL MVA VARIABLES
    if(fChain->GetBranch("muon_mva_fake_v4"))     mu.SetHNL_FakeLepMVAMuonV4(muon_mva_fake_v4->at(i));
    if(fChain->GetBranch("muon_mva_fake_ed_v4"))  mu.SetHNL_FakeLepMVAMuon_EtaDependantV4(muon_mva_fake_ed_v4->at(i));
    if(fChain->GetBranch("muon_mva_fake_QCD_LFvsHF_v5")) {
      mu.SetHNL_FakeFlavourLepMVA_V5( muon_mva_fake_QCD_LFvsHF_v5->at(i),
              muon_mva_fake_QCD_HFBvsHFC_v5->at(i),
              muon_mva_fake_QCD_LF1_v5->at(i),
              muon_mva_fake_QCD_LF2_v5->at(i));
    }

    if(iSetupLeptonBDTv4) {
      if(!fChain->GetBranch("muon_mva_fake_v4")) mu.SetHNL_FakeLepMVAMuonV4( GetBDTScoreMuon(mu,AnalyzerCore::Fake,  "BDTGv4"));
      if(!fChain->GetBranch("muon_mva_fake_ed_v4")) mu.SetHNL_FakeLepMVAMuon_EtaDependantV4( GetBDTScoreMuon_EtaDependant(mu,AnalyzerCore::Fake,  "BDTGv4"));
    }

    if(iSetupLeptonBDTv5) {
      
      if(!fChain->GetBranch("muon_mva_fake_QCD_LFvsHF_v5")){
  
  mu.SetHNL_FakeFlavourLepMVA_V5( GetBDTScoreMuon(mu,AnalyzerCore::FakeRate,  "BDTGv5_QCD_LFvsHF"),
          GetBDTScoreMuon(mu,AnalyzerCore::FakeRate,  "BDTGv5_QCD_HFBvsHFC"),
          GetBDTScoreMuon(mu,AnalyzerCore::FakeRate,  "BDTGv5_LF1"),
          GetBDTScoreMuon(mu,AnalyzerCore::FakeRate,  "BDTGv5_LF2"));
  
      }
    }
    

    if(fChain->GetBranch("muon_ptrel"))            mu.SetJetPtRel(muon_ptrel->at(i));
    if(fChain->GetBranch("muon_ptratio"))          mu.SetJetPtRatio(muon_ptratio->at(i));
    if(fChain->GetBranch("muon_v2_cj_bjetdisc"))   mu.SetCloseJetBScore(muon_v2_cj_bjetdisc->at(i));
    if(fChain->GetBranch("muon_v2_cj_cvsbjetdisc"))mu.SetCloseJetCvsBScore(muon_v2_cj_cvsbjetdisc->at(i));
    if(fChain->GetBranch("muon_v2_cj_cvsljetdisc"))mu.SetCloseJetCvsLScore(muon_v2_cj_cvsljetdisc->at(i));
    if(fChain->GetBranch("muon_v2_cj_flavour"))    mu.SetCloseJetFlavour(muon_v2_cj_flavour->at(i));
    

    bool FillCloseJetVar=!fChain->GetBranch("muon_v2_cj_cvsljetdisc");
    if(Analyzer=="HNL_LeptonID_BDT_KinVar") FillCloseJetVar=false;

    if(FillCloseJetVar){
      
      std::vector<Jet>    AK4_JetAllColl = All_Jets;

      float  JetDiscCJ = -999;
      float  JetDiscCJ_CvsB = -999;
      float  JetDiscCJ_CvsL = -999;

      int JetHadFlavour = -999;
      int IdxMatchJet=-1;
      float mindR1=999.;

      for(unsigned int ij=0; ij<AK4_JetAllColl.size(); ij++){
        float dR1=mu.DeltaR(AK4_JetAllColl.at(ij));
        if(dR1>0.4) continue;
        if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
      }
      if(IdxMatchJet!=-1)    {
  JetDiscCJ     = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet);
  JetDiscCJ_CvsB     = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet_CvsB);
  JetDiscCJ_CvsL     = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet_CvsL);

  JetHadFlavour = AK4_JetAllColl.at(IdxMatchJet).hadronFlavour();
      }
      else {
  JetDiscCJ=0.1; //// These are low pt ; value chosen to match peak in prompt leptons
  JetDiscCJ_CvsB=0.4;
  JetDiscCJ_CvsL=0.1;
  JetHadFlavour=0;
      }

      mu.SetJetPtRel(JetLeptonPtRelLepAware(mu));
      mu.SetJetPtRatio(JetLeptonPtRatioLepAware(mu));
      mu.SetCloseJetBScore(JetDiscCJ);
      mu.SetCloseJetCvsBScore(JetDiscCJ_CvsB);
      mu.SetCloseJetCvsLScore(JetDiscCJ_CvsL);
      mu.SetCloseJetFlavour(JetHadFlavour);
    }

    if(!fChain->GetBranch("muon_lepton_type") ||  !fChain->GetBranch("muon_is_cf"))  {
      int lep_type = GetLeptonType_JH(mu, All_Gens);
      mu.SetLeptonType(lep_type);
      mu.SetLeptonIsCF(IsCF(mu, All_Gens));

    }
    else {
      if(fChain->GetBranch("muon_lepton_type")) mu.SetLeptonType(muon_lepton_type->at(i));
      if(fChain->GetBranch("muon_is_cf")) mu.SetLeptonIsCF(muon_is_cf->at(i));
    }
    
    int UpdatedLeptonType = -999;
    if(MCSample.Contains("Sherpa")) UpdatedLeptonType = HotFixLeptonType(Lepton(mu));
    if(MCSample.Contains("MiNNLO")) UpdatedLeptonType = HotFixLeptonType(Lepton(mu));
    if(UpdatedLeptonType != -999) mu.SetLeptonType(UpdatedLeptonType);
    out.push_back(mu);

  }

  //if(PtOrderObj) std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}


std::vector<Electron> AnalyzerCore::GetAllElectrons(){

  std::vector<Electron> out;
  if(!electron_Energy) return out;


  for(unsigned int i=0; i<electron_Energy->size(); i++){

    Electron el;
    el.SetRunYear(DataYear);
    el.SetRunEra(GetEraShort());
    el.SetEnShift(  electron_Energy_Scale_Up->at(i)/electron_Energy->at(i), electron_Energy_Scale_Down->at(i)/electron_Energy->at(i) );
    el.SetResShift( electron_Energy_Smear_Up->at(i)/electron_Energy->at(i), electron_Energy_Smear_Down->at(i)/electron_Energy->at(i) );

    el.SetPtEtaPhiE(1., electron_eta->at(i), electron_phi->at(i), electron_Energy->at(i));
    double el_theta = el.Theta();
    double el_pt = electron_Energy->at(i) * TMath::Sin( el_theta );
    el.SetPtEtaPhiE( el_pt, electron_eta->at(i), electron_phi->at(i), electron_Energy->at(i));
    el.SetUncorrectedPt(electron_EnergyUnCorr->at(i) * TMath::Sin( el_theta ));

    el.SetUncorrE(electron_EnergyUnCorr->at(i));
    el.SetSC(electron_scEta->at(i), electron_scPhi->at(i), electron_scEnergy->at(i));
    el.SetElSCEta(electron_scEta->at(i));
    el.SetCharge(electron_charge->at(i));
    el.SetdXY(electron_dxyVTX->at(i), electron_dxyerrVTX->at(i));
    el.SetdZ(electron_dzVTX->at(i), electron_dzerrVTX->at(i));
    el.SetIP3D(electron_3DIPVTX->at(i), electron_3DIPerrVTX->at(i));
    el.SetMVA(electron_MVAIso->at(i), electron_MVANoIso->at(i));
    el.SetLepMVA( electron_MVANoIso->at(i));
    el.SetPassConversionVeto(electron_passConversionVeto->at(i));
    el.SetNMissingHits(electron_mHits->at(i));
    el.SetRho(Rho);
    el.SetIsGsfCtfScPixChargeConsistent(electron_isGsfCtfScPixChargeConsistent->at(i),   electron_isGsfScPixChargeConsistent->at(i),electron_isGsfCtfChargeConsistent->at(i));

    el.SetR9(electron_r9->at(i));
    el.SetL1Et(electron_l1et->at(i));
    el.SetpsEoverEraw(electron_scPreEnergy->at(i)/electron_scRawEnergy->at(i));
    
    el.SetCutBasedIDVariables(
      electron_Full5x5_SigmaIEtaIEta->at(i),
      electron_dEtaInSeed->at(i),
      electron_dPhiIn->at(i),
      electron_HoverE->at(i),
      electron_InvEminusInvP->at(i),
      electron_e2x5OverE5x5->at(i),
      electron_e1x5OverE5x5->at(i),
      electron_trackIso->at(i),
      electron_dr03EcalRecHitSumEt->at(i),
      electron_dr03HcalDepth1TowerSumEt->at(i),
      electron_dr03HcalTowerSumEt->at(i),
      electron_dr03TkSumPt->at(i),
      electron_ecalPFClusterIso->at(i),
      electron_hcalPFClusterIso->at(i),
      electron_ecalDriven->at(i)
    );
    el.SetTrkIso(electron_trackIso->at(i));


    el.SetIDBit(electron_IDBit->at(i));
    vector<int> temp_idcutbit;
    for(unsigned int j=0; j<Electron::N_SELECTOR; j++){
      temp_idcutbit.push_back( electron_IDCutBit->at( i*Electron::N_SELECTOR + j ) );
    }
    el.SetIDCutBit(temp_idcutbit);
    el.SetRelPFIso_Rho(electron_RelPFIso_Rho->at(i));

    //==== Should be ran after SCeta is set
    el.SetMiniIso(
      electron_chMiniIso->at(i),
      electron_nhMiniIso->at(i),
      electron_phMiniIso->at(i),
      electron_puChMiniIso->at(i),
      Rho,
      el.EA()
    );
    el.SetEtaWidth(electron_etaWidth->at(i));
    el.SetPhiWidth(electron_phiWidth->at(i));
    el.SetFBrem(electron_fbrem->at(i));
    el.SetEOverP(electron_eOverP->at(i));
    el.SetDetaIn(electron_dEtaIn->at(i));
    el.SetSigmaIEtaIE(electron_sigmaIEtaIEta->at(i));
    el.SetLepIso(electron_chIso03->at(i),electron_nhIso03->at(i),electron_phIso03->at(i));
    
    el.SetE15(electron_E15->at(i));
    el.SetE25(electron_E25->at(i));
    el.SetE55(electron_E55->at(i));


    el.SetFilterBits(electron_filterbits->at(i));
    el.SetPathBits(electron_pathbits->at(i));


    /////// USE NEW Branches to set lepton variables
    if(fChain->GetBranch("electron_ptrel"))         el.SetJetPtRel(electron_ptrel->at(i));
    if(fChain->GetBranch("electron_ptratio"))       el.SetJetPtRatio(electron_ptratio->at(i));
    if(fChain->GetBranch("electron_v2_cj_flavour")) el.SetCloseJetFlavour(electron_v2_cj_flavour->at(i));
    if(fChain->GetBranch("electron_v2_cj_bjetdisc"))    el.SetCloseJetBScore(electron_v2_cj_bjetdisc->at(i));
    if(fChain->GetBranch("electron_v2_cj_cvsbjetdisc")) el.SetCloseJetCvsBScore(electron_v2_cj_cvsbjetdisc->at(i));
    if(fChain->GetBranch("electron_v2_cj_cvsljetdisc")) el.SetCloseJetCvsLScore(electron_v2_cj_cvsljetdisc->at(i));
        
    //////// MVA Branches
    ////*************** FAKE MVA
    if(fChain->GetBranch("electron_mva_fake_v4"))  el.SetHNL_FakeLepMVAV4(electron_mva_fake_v4->at(i), 
									  electron_mva_fakeHF_v4->at(i), 
									  electron_mva_fakeHFB_v4->at(i),
									  electron_mva_fakeHFC_v4->at(i), 
									  electron_mva_fakeLF_v4->at(i),
									  electron_mva_fakeTop_v4->at(i));

    if(fChain->GetBranch("electron_mva_fake_ed_v4"))  el.SetHNL_FakeLepMVA_EtaDependantV4(electron_mva_fake_ed_v4->at(i),
											  electron_mva_fakeHF_ed_v4->at(i),
											  electron_mva_fakeHFB_ed_v4->at(i),
											  electron_mva_fakeHFC_ed_v4->at(i),
											  electron_mva_fakeLF_ed_v4->at(i),
											  electron_mva_fakeTop_ed_v4->at(i));
    
    ////*************** CONV MVA
    if(fChain->GetBranch("electron_mva_conv_v2"))    el.SetHNL_ConvLepMVAV4(electron_mva_conv_v2->at(i));
    if(fChain->GetBranch("electron_mva_conv_ed_v2")) el.SetHNL_ConvLepMVA_EtaDependantV4(electron_mva_conv_ed_v2->at(i));

    ////*************** CF MVA
    if(fChain->GetBranch("electron_mva_cf_v2"))     el.SetHNL_CFLepMVAV4(electron_mva_cf_v2->at(i));

    if(fChain->GetBranch("electron_mva_cf_ed_v2"))  el.SetHNL_CFLepMVA_EtaDependantV4(electron_mva_cf_ed_v2->at(i));

    

    if(fChain->GetBranch("electron_mva_conv_ed_v5")) el.SetHNL_ConvLepMVA_EtaDependantV5(electron_mva_conv_ed_v5->at(i));
											 

    if(fChain->GetBranch("electron_mva_cf_ed_v5"))  el.SetHNL_CFLepMVA_EtaDependantV5(electron_mva_cf_ed_v5->at(i), 
										      electron_mva_cf_ed_v5pt->at(i));

    if(fChain->GetBranch("electron_mva_fake_ed_v5")) {
      
      el.SetHNL_FakeLepMVA_EtaDependantV5(electron_mva_fake_ed_v5->at(i),
					  electron_mva_fakeHFB_v5->at(i),
					  electron_mva_fakeHFC_v5->at(i),
					  electron_mva_fakeLF_v5->at(i));
      
      el.SetHNL_FakeFlavourLepMVA_V5(electron_mva_fake_QCD_LFvsHF_v5->at(i),
				     electron_mva_fake_QCD_HFBvsHFC_v5->at(i),
				     electron_mva_fake_QCD_LF1_v5->at(i),
				     electron_mva_fake_QCD_LF2_v5->at(i));





    }
    
    
    if(iSetupLeptonBDTv5){


      if(!fChain->GetBranch("electron_mva_conv_ed_v5"))      el.SetHNL_ConvLepMVA_EtaDependantV5( GetBDTScoreEl_EtaDependant(el,AnalyzerCore::Conv,  "BDTGv5"));
      if(!fChain->GetBranch("electron_mva_fake_ed_v5"))      el.SetHNL_FakeLepMVA_EtaDependantV5(GetBDTScoreEl_EtaDependant(el,AnalyzerCore::Fake,  "BDTGv5" ), 
												 GetBDTScoreEl(el,AnalyzerCore::Fake,  "BDTGv5_HFB" ) , 
												 GetBDTScoreEl(el,AnalyzerCore::Fake,  "BDTGv5_HFC" ),  
												 GetBDTScoreEl(el,AnalyzerCore::Fake,  "BDTGv5_LF" ));
      
      
      if(!fChain->GetBranch("electron_mva_cf_ed_v5"))      el.SetHNL_CFLepMVA_EtaDependantV5  (GetBDTScoreEl_EtaDependant(el,AnalyzerCore::CF, "BDTGv5" ), 
											       GetBDTScoreEl_EtaDependant(el,AnalyzerCore::CF, "BDTGv5Pt" ));
      
      
      
      if(!fChain->GetBranch("electron_mva_fake_QCD_LFvsHF_v5")) {
	
        el.SetHNL_FakeFlavourLepMVA_V5(	GetBDTScoreEl(el,AnalyzerCore::FakeRate,  "BDTGv5_QCD_LFvsHF"),
					GetBDTScoreEl(el,AnalyzerCore::FakeRate,  "BDTGv5_QCD_HFBvsHFC"),
					GetBDTScoreEl(el,AnalyzerCore::FakeRate,  "BDTGv5_LF1"),
					GetBDTScoreEl(el,AnalyzerCore::FakeRate,  "BDTGv5_LF2"));
      }
    }


    if(iSetupLeptonBDTv4){

      if(!fChain->GetBranch("electron_mva_fake_v4"))  el.SetHNL_FakeLepMVAV4(GetBDTScoreEl(el,  AnalyzerCore::Fake,  "BDTGv4" ), 
									     GetBDTScoreEl(el,AnalyzerCore::Fake,  "BDTGv4_HF"), 
									     GetBDTScoreEl(el,AnalyzerCore::Fake,  "BDTGv4_HFB"), 
									     GetBDTScoreEl(el,AnalyzerCore::Fake,  "BDTGv4_HFC"), 
									     GetBDTScoreEl(el,AnalyzerCore::Fake,  "BDTGv4_LF"),
									     GetBDTScoreEl(el,AnalyzerCore::Fake,  "BDTGv4_Top"));
      

      if(!fChain->GetBranch("electron_mva_fake_ed_v4"))  el.SetHNL_FakeLepMVA_EtaDependantV4(GetBDTScoreEl_EtaDependant(el,AnalyzerCore::Fake,  "BDTGv4" ),
											     GetBDTScoreEl_EtaDependant(el,AnalyzerCore::Fake,  "BDTGv4_HF"),
											     GetBDTScoreEl_EtaDependant(el,AnalyzerCore::Fake,  "BDTGv4_HFB"),
											     GetBDTScoreEl_EtaDependant(el,AnalyzerCore::Fake,  "BDTGv4_HFC"),
											     GetBDTScoreEl_EtaDependant(el,AnalyzerCore::Fake,  "BDTGv4_LF"),
											     GetBDTScoreEl_EtaDependant(el,AnalyzerCore::Fake,  "BDTGv4_Top"));
      



      if(!fChain->GetBranch("electron_mva_conv_v2"))    el.SetHNL_ConvLepMVAV4(GetBDTScoreEl(el,AnalyzerCore::Conv,  "BDTGv4"));
      if(!fChain->GetBranch("electron_mva_conv_ed_v2")) el.SetHNL_ConvLepMVA_EtaDependantV4(GetBDTScoreEl_EtaDependant(el,AnalyzerCore::Conv,  "BDTGv4"));
      
      
      if(!fChain->GetBranch("electron_mva_cf_v4"))     el.SetHNL_CFLepMVAV4(GetBDTScoreEl(el,AnalyzerCore::CF,  "BDTGv4"));
      if(!fChain->GetBranch("electron_mva_cf_ed_v4"))  el.SetHNL_CFLepMVA_EtaDependantV4(GetBDTScoreEl_EtaDependant(el,AnalyzerCore::CF,  "BDTGv4"));
      
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FillCloseJetVar=!fChain->GetBranch("electron_v2_cj_cvsbjetdisc");
    if(Analyzer=="HNL_LeptonID_BDT_KinVar") FillCloseJetVar=false;
    if(FillCloseJetVar){
      
      std::vector<Jet>    AK4_JetAllColl = All_Jets;
      
      float  JetDiscCJ = -999;
      float  JetDiscCJ_CvsB = -999;
      float  JetDiscCJ_CvsL = -999;

      float mindR1=999.;
      int IdxMatchJet=-1;
      int JetHadFlavour = -999;
    
      for(unsigned int ij=0; ij<AK4_JetAllColl.size(); ij++){

	float dR1=el.DeltaR(AK4_JetAllColl.at(ij));
	if(dR1>0.4) continue;
	if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
      }
      if(IdxMatchJet!=-1) {
	JetDiscCJ        = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet);
        JetDiscCJ_CvsB   = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet_CvsB);
        JetDiscCJ_CvsL    = AK4_JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet_CvsL);

	JetHadFlavour  = AK4_JetAllColl.at(IdxMatchJet).hadronFlavour();
      }
      else {
        JetDiscCJ=0.1; //// These are low pt ; value chosen to match peak in prompt leptons                                                                                
        JetDiscCJ_CvsB=0.4;
        JetDiscCJ_CvsL=0.1;
	JetHadFlavour = 0;
      }

      el.SetJetPtRel(JetLeptonPtRelLepAware(el));
      el.SetJetPtRatio(JetLeptonPtRatioLepAware(el));
      el.SetCloseJetBScore(JetDiscCJ);
      el.SetCloseJetCvsBScore(JetDiscCJ_CvsB);
      el.SetCloseJetCvsLScore(JetDiscCJ_CvsL);
      el.SetCloseJetFlavour(JetHadFlavour);
      
    }
    
    if(!fChain->GetBranch("electron_lepton_type") ||  !fChain->GetBranch("electron_is_cf"))  {
      int lep_type = GetLeptonType_JH(el, All_Gens);
      el.SetLeptonType(lep_type);
    }
    else {
      if(fChain->GetBranch("electron_lepton_type")) el.SetLeptonType(electron_lepton_type->at(i));
    }

    el.SetLeptonIsPromptConv(HasPromptConv(el));
    el.SetLeptonIsCF(IsCF(el, All_Gens,true) );                                                                                                               

    
    int UpdatedLeptonType = -999;
    if(MCSample.Contains("Sherpa")) UpdatedLeptonType = HotFixLeptonType(Lepton(el));
    if(MCSample.Contains("MiNNLO")) UpdatedLeptonType = HotFixLeptonType(Lepton(el));
    if(UpdatedLeptonType != -999) el.SetLeptonType(UpdatedLeptonType);

    out.push_back(el);
  }
  
  //if(PtOrderObj) std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}


std::vector<Tau> AnalyzerCore::GetAllTaus(){

  std::vector<Tau> out;
  if(!tau_pt) return out;
  for(unsigned int i=0; i<tau_pt->size(); i++){

    Tau tau;
    tau.SetCharge(tau_charge->at(i));
    tau.SetPtEtaPhiM(tau_pt->at(i), tau_eta->at(i), tau_phi->at(i), tau_mass->at(i));
    tau.SetDecayMode(tau_decaymode->at(i));
    tau.SetdXY(tau_dxy->at(i),0.);
    tau.SetdZ(tau_dz->at(i),0.);
    tau.SetDecayModeNewDM(tau_idDecayModeNewDMs->at(i));
    tau.SetIDBit(tau_IDBit->at(i));

    out.push_back(tau);

  }


  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}

std::vector<Lepton *> AnalyzerCore::MakeLeptonPointerVector(const std::vector<Muon>& muons,AnalyzerParameter param_, double TightIso, bool UseMini){

  std::vector<Lepton *> out;
  for(unsigned int i=0; i<muons.size(); i++){
    Lepton *l = (Lepton *)(&muons.at(i));
    if( !(l->LeptonFlavour() == Lepton::MUON) ){
      cout << "[AnalyzerCore::MakeLeptonPointerVector(std::vector<Muon>& muons)] Not muon.." << endl;
      exit(EXIT_FAILURE);
    }
    if(TightIso>0){

      double this_RelIso = l->RelIso();
      if(UseMini) this_RelIso = l->MiniRelIso();
      double ptcone = l->CalcPtCone(this_RelIso, TightIso);
      l->SetPtCone( ptcone );

    }
    l->SetPassID(muons[i].PassID(param_.Muon_Tight_ID), param_.Muon_Tight_ID);

    out.push_back(l);
  }
  std::sort(out.begin(),     out.end(),     PtComparingPtr);

  return out;

}
std::vector<Lepton *> AnalyzerCore::MakeLeptonPointerVector(const std::vector<Muon>& muons, double TightIso, bool UseMini){

  std::vector<Lepton *> out;
  for(unsigned int i=0; i<muons.size(); i++){
    Lepton *l = (Lepton *)(&muons.at(i));
    if( !(l->LeptonFlavour() == Lepton::MUON) ){
      cout << "[AnalyzerCore::MakeLeptonPointerVector(std::vector<Muon>& muons)] Not muon.." << endl;
      exit(EXIT_FAILURE);
    }
    if(TightIso>0){

      double this_RelIso = l->RelIso();
      if(UseMini) this_RelIso = l->MiniRelIso();
      double ptcone = l->CalcPtCone(this_RelIso, TightIso);
      l->SetPtCone( ptcone );

    }

    out.push_back(l);
  }
  std::sort(out.begin(),     out.end(),     PtComparingPtr);

  return out;

}
std::vector<Lepton *> AnalyzerCore::MakeLeptonPointerVector(const std::vector<Electron>& electrons, AnalyzerParameter param_,double TightIso, bool UseMini){

  std::vector<Lepton *> out;
  for(unsigned int i=0; i<electrons.size(); i++){
    Lepton *l = (Lepton *)(&electrons.at(i));
    if( !(l->LeptonFlavour() == Lepton::ELECTRON) ){
      cout << "[AnalyzerCore::MakeLeptonPointerVector(std::vector<ELECTRON>& electrons)] Not electron.." << endl;
      exit(EXIT_FAILURE);
    }
    if(TightIso>0){

      double this_RelIso = l->RelIso();
      if(UseMini) this_RelIso = l->MiniRelIso();
      double ptcone = l->CalcPtCone(this_RelIso, TightIso);
      l->SetPtCone( ptcone );

    }
    l->SetPassID(electrons[i].PassID(param_.Electron_Tight_ID), param_.Electron_Tight_ID);

    out.push_back(l);
  }

  std::sort(out.begin(),     out.end(),     PtComparingPtr);

  return out;

}
std::vector<Lepton *> AnalyzerCore::MakeLeptonPointerVector(const std::vector<Electron>& electrons, double TightIso, bool UseMini){

  std::vector<Lepton *> out;
  for(unsigned int i=0; i<electrons.size(); i++){
    Lepton *l = (Lepton *)(&electrons.at(i));
    if( !(l->LeptonFlavour() == Lepton::ELECTRON) ){
      cout << "[AnalyzerCore::MakeLeptonPointerVector(std::vector<ELECTRON>& electrons)] Not electron.." << endl;
      exit(EXIT_FAILURE);
    }
    if(TightIso>0){

      double this_RelIso = l->RelIso();
      if(UseMini) this_RelIso = l->MiniRelIso();
      double ptcone = l->CalcPtCone(this_RelIso, TightIso);
      l->SetPtCone( ptcone );

    }

    out.push_back(l);
  }

  std::sort(out.begin(),     out.end(),     PtComparingPtr);

  return out;

}
std::vector<Lepton *> AnalyzerCore::MakeLeptonPointerVector(const std::vector<Muon>& muons, const std::vector<Electron>& electrons,double TightIso, bool UseMini){

  std::vector<Lepton *> out;
  for(unsigned int i=0; i<muons.size(); i++){
    Lepton *l = (Lepton *)(&muons.at(i));
    if( !(l->LeptonFlavour() == Lepton::MUON) ){
      cout << "[AnalyzerCore::MakeLeptonPointerVector(std::vector<Muon>& muons)] Not muon.." << endl;
      exit(EXIT_FAILURE);
    }
    if(TightIso>0){

      double this_RelIso = l->RelIso();
      if(UseMini) this_RelIso = l->MiniRelIso();
      double ptcone = l->CalcPtCone(this_RelIso, TightIso);
      l->SetPtCone( ptcone );

    }
    out.push_back(l);
  }

  for(unsigned int i=0; i<electrons.size(); i++){
    Lepton *l = (Lepton *)(&electrons.at(i));
    if( !(l->LeptonFlavour() == Lepton::ELECTRON) ){
      cout << "[AnalyzerCore::MakeLeptonPointerVector(std::vector<ELECTRON>& electrons)] Not electron.." << endl;
      exit(EXIT_FAILURE);
    }
    if(TightIso>0){

      double this_RelIso = l->RelIso();
      if(UseMini) this_RelIso = l->MiniRelIso();
      double ptcone = l->CalcPtCone(this_RelIso, TightIso);
      l->SetPtCone( ptcone );

    }

    out.push_back(l);
  }

  std::sort(out.begin(),     out.end(),     PtComparingPtr);

  return out;
}


std::vector<Lepton *> AnalyzerCore::MakeLeptonPointerVector(const std::vector<Muon>& muons, const std::vector<Electron>& electrons, AnalyzerParameter param_,  double TightIso, bool UseMini){

  std::vector<Lepton *> out;
  for(unsigned int i=0; i<muons.size(); i++){
    Lepton *l = (Lepton *)(&muons.at(i));
    if( !(l->LeptonFlavour() == Lepton::MUON) ){
      cout << "[AnalyzerCore::MakeLeptonPointerVector(std::vector<Muon>& muons)] Not muon.." << endl;
      exit(EXIT_FAILURE);
    }
    if(TightIso>0){

      double this_RelIso = l->RelIso();
      if(UseMini) this_RelIso = l->MiniRelIso();
      double ptcone = l->CalcPtCone(this_RelIso, TightIso);
      l->SetPtCone( ptcone );

    }
    l->SetPassID(muons[i].PassID(param_.Muon_Tight_ID), param_.Muon_Tight_ID);
    //cout << "MakeLeptonPointerVector Muon " << param_.Muon_Tight_ID << " " <<  l->Pt() << endl;
    out.push_back(l);
  }

  for(unsigned int i=0; i<electrons.size(); i++){
    Lepton *l = (Lepton *)(&electrons.at(i));
    if( !(l->LeptonFlavour() == Lepton::ELECTRON) ){
      cout << "[AnalyzerCore::MakeLeptonPointerVector(std::vector<ELECTRON>& electrons)] Not electron.." << endl;
      exit(EXIT_FAILURE);
    }
    if(TightIso>0){

      double this_RelIso = l->RelIso();
      if(UseMini) this_RelIso = l->MiniRelIso();
      double ptcone = l->CalcPtCone(this_RelIso, TightIso);
      l->SetPtCone( ptcone );

    }
    l->SetPassID(electrons[i].PassID(param_.Electron_Tight_ID), param_.Electron_Tight_ID);

    //cout << "MakeLeptonPointerVector Electron " << param_.Electron_Tight_ID <<" " <<  l->Pt() << endl;

    out.push_back(l);
  }

  std::sort(out.begin(),     out.end(),     PtComparingPtr);

  return out;
}

std::vector<Photon> AnalyzerCore::GetAllPhotons(){

  std::vector<Photon> out;
  for(unsigned int i=0; i<photon_Energy->size(); i++){
    
    Photon pho;

    pho.SetPtEtaPhiE(1., photon_eta->at(i), photon_phi->at(i), photon_Energy->at(i));
    double pho_theta = pho.Theta();
    double pho_pt = photon_Energy->at(i) * TMath::Sin( pho_theta );
    pho.SetPtEtaPhiE( pho_pt, photon_eta->at(i), photon_phi->at(i), photon_Energy->at(i));

    pho.SetSC(photon_scEta->at(i), photon_scPhi->at(i));
    pho.SetRho(Rho);

    pho.SetCutBasedIDVariables(
      photon_Full5x5_SigmaIEtaIEta->at(i),
      photon_HoverE->at(i),
      photon_ChIsoWithEA->at(i),
      photon_NhIsoWithEA->at(i),
      photon_PhIsoWithEA->at(i)
    );
    
    std::vector<bool> ids = {
      photon_passLooseID->at(i),
      photon_passMediumID->at(i),
      photon_passTightID->at(i),
      photon_passMVAID_WP80->at(i),
      photon_passMVAID_WP90->at(i)
     };
    pho.SetPOGIDs(ids);
        
    out.push_back(pho);
    
  }
  return out;
  
}


std::vector<Jet> AnalyzerCore::GetAllJets(bool applySmear){

  std::vector<Jet> out;
  for(unsigned int i=0; i<jet_pt->size(); i++){
    Jet jet;
    jet.SetPtEtaPhiM(jet_pt->at(i), jet_eta->at(i), jet_phi->at(i), jet_m->at(i));

    jet.SetPxUnSmeared(jet.Px());
    jet.SetPyUnSmeared(jet.Py());

    jet.SetJEC(jet_JECL1FastJet->at(i), jet_JECFull->at(i));
    //jet.SetNTracks(jet_vtxNtracks->at(i));
    //==== Jet energy up and down are 1.xx or 0.99, not energy
    jet.SetEnShift( jet_shiftedEnUp->at(i), jet_shiftedEnDown->at(i) );
    if(!IsDATA){
      if(applySmear){
	jet *= jet_smearedRes->at(i);
	jet.SetIsSmeared(true);
      }
      jet.SetResShift( jet_smearedResUp->at(i)/jet_smearedRes->at(i), jet_smearedResDown->at(i)/jet_smearedRes->at(i) );
      jet.SetRes(jet_smearedRes->at(i));
      jet.SetGenFlavours(jet_partonFlavour->at(i), jet_hadronFlavour->at(i));
      jet.SetGenHFHadronMatcher(jet_GenHFHadronMatcher_flavour->at(i),jet_GenHFHadronMatcher_origin->at(i));
    }
    jet.SetBJetNNCorrection(jet_bJetNN_corr->at(i),jet_bJetNN_res->at(i));
    jet.SetCJetNNCorrection(jet_cJetNN_corr->at(i),jet_cJetNN_res->at(i));
    jet.SetCharge(jet_charge->at(i));

    jet.SetArea(jet_area->at(i));
    std::vector<double> tvs = {
      jet_DeepCSV->at(i),
      jet_DeepCSV_CvsL->at(i),
      jet_DeepCSV_CvsB->at(i),
      jet_DeepJet->at(i),
      jet_DeepJet_CvsL->at(i),
      jet_DeepJet_CvsB->at(i),
    };
    jet.SetTaggerResults(tvs);
    jet.SetEnergyFractions(jet_chargedHadronEnergyFraction->at(i), jet_neutralHadronEnergyFraction->at(i), jet_neutralEmEnergyFraction->at(i), jet_chargedEmEnergyFraction->at(i), jet_muonEnergyFraction->at(i));
    jet.SetMultiplicities(jet_chargedMultiplicity->at(i), jet_neutralMultiplicity->at(i));
    jet.SetPileupJetId(jet_PileupJetId->at(i));
    jet.SetTightJetID(jet_tightJetID->at(i));
    jet.SetTightLepVetoJetID(jet_tightLepVetoJetID->at(i));
    
    out.push_back(jet);
  }

  //if(PtOrderObj)  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}

std::vector<FatJet> AnalyzerCore::GetAllFatJets(){

  std::vector<FatJet> out;
  for(unsigned int i=0; i<fatjet_pt->size(); i++){
    FatJet jet;
    jet.SetPtEtaPhiM(fatjet_pt->at(i), fatjet_eta->at(i), fatjet_phi->at(i), fatjet_m->at(i));
    jet.SetPxUnSmeared(jet.Px());
    jet.SetPyUnSmeared(jet.Py());
    jet.SetEnShift( fatjet_shiftedEnUp->at(i), fatjet_shiftedEnDown->at(i) );
    if(!IsDATA){
      jet *= fatjet_smearedRes->at(i);
      jet.SetResShift( fatjet_smearedResUp->at(i)/fatjet_smearedRes->at(i), fatjet_smearedResDown->at(i)/fatjet_smearedRes->at(i) );
    }
    jet.SetCharge(fatjet_charge->at(i));

    jet.SetArea(fatjet_area->at(i));
    jet.SetGenFlavours(fatjet_partonFlavour->at(i), fatjet_hadronFlavour->at(i));
    std::vector<double> tvs = {
      fatjet_DeepCSV->at(i),
      fatjet_DeepCSV_CvsL->at(i),
      fatjet_DeepCSV_CvsB->at(i),
      fatjet_particleNet_TvsQCD->at(i),
      fatjet_particleNet_WvsQCD->at(i),
      fatjet_particleNet_ZvsQCD->at(i),
      fatjet_particleNet_HbbvsQCD->at(i),
      fatjet_particleNet_HccvsQCD->at(i),
      fatjet_particleNet_H4qvsQCD->at(i),
      fatjet_particleNet_QCD->at(i),
      fatjet_particleNetMD_Xbb->at(i),
      fatjet_particleNetMD_Xcc->at(i),
      fatjet_particleNetMD_Xqq->at(i),
      fatjet_particleNetMD_QCD->at(i),
    };
    jet.SetTaggerResults(tvs);
    jet.SetEnergyFractions(fatjet_chargedHadronEnergyFraction->at(i), fatjet_neutralHadronEnergyFraction->at(i), fatjet_neutralEmEnergyFraction->at(i), fatjet_chargedEmEnergyFraction->at(i), fatjet_muonEnergyFraction->at(i));
    jet.SetMultiplicities(fatjet_chargedMultiplicity->at(i), fatjet_neutralMultiplicity->at(i));
    jet.SetLSF(fatjet_LSF->at(i), fatjet_LSFlep_PID->at(i));
    jet.SetTightJetID(fatjet_tightJetID->at(i));
    jet.SetTightLepVetoJetID(fatjet_tightLepVetoJetID->at(i));
    jet.SetPuppiTaus(fatjet_puppi_tau1->at(i), fatjet_puppi_tau2->at(i), fatjet_puppi_tau3->at(i), fatjet_puppi_tau4->at(i));
    jet.SetSDMass(fatjet_softdropmass->at(i));

    out.push_back(jet);
  }

  //if(PtOrderObj)  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}


std::vector<Gen> AnalyzerCore::GetGens(){

  std::vector<Gen> out;
  if(IsDATA) return out;

  for(unsigned int i=0; i<gen_pt->size(); i++){

    Gen gen;

    gen.SetIsEmpty(false);
    gen.SetPtEtaPhiM(gen_pt->at(i), gen_eta->at(i), gen_phi->at(i), gen_mass->at(i));
    gen.SetCharge(gen_charge->at(i));
    gen.SetIndexPIDStatus(i, gen_PID->at(i), gen_status->at(i));
    gen.SetMother(gen_mother_index->at(i));
    gen.SetGenStatusFlag_isPrompt( gen_isPrompt->at(i) );
    gen.SetGenStatusFlag_isPromptFinalState( gen_isPromptFinalState->at(i) );
    gen.SetGenStatusFlag_isTauDecayProduct( gen_isTauDecayProduct->at(i) );
    gen.SetGenStatusFlag_isPromptTauDecayProduct( gen_isPromptTauDecayProduct->at(i) );
    gen.SetGenStatusFlag_isDirectPromptTauDecayProductFinalState( gen_isDirectPromptTauDecayProductFinalState->at(i) );
    gen.SetGenStatusFlag_isHardProcess( gen_isHardProcess->at(i) );
    gen.SetGenStatusFlag_isLastCopy( gen_isLastCopy->at(i) );
    gen.SetGenStatusFlag_isLastCopyBeforeFSR( gen_isLastCopyBeforeFSR->at(i) );
    gen.SetGenStatusFlag_isPromptDecayed( gen_isPromptDecayed->at(i) );
    gen.SetGenStatusFlag_isDecayedLeptonHadron( gen_isDecayedLeptonHadron->at(i) );
    gen.SetGenStatusFlag_fromHardProcessBeforeFSR( gen_fromHardProcessBeforeFSR->at(i) );
    gen.SetGenStatusFlag_fromHardProcessDecayed( gen_fromHardProcessDecayed->at(i) );
    gen.SetGenStatusFlag_fromHardProcessFinalState( gen_fromHardProcessFinalState->at(i) );
    gen.SetGenStatusFlag_isMostlyLikePythia6Status3( gen_isMostlyLikePythia6Status3->at(i) );

    out.push_back(gen);

  }

  return out;

}

std::vector<LHE> AnalyzerCore::GetLHEs(){

  std::vector<LHE> out;
  if(IsDATA) return out;

  for(unsigned int i=0; i<LHE_Px->size(); i++){

    LHE lhe;

    lhe.SetPxPyPzE(LHE_Px->at(i), LHE_Py->at(i), LHE_Pz->at(i), LHE_E->at(i));
    lhe.SetIndexIDStatus(i, LHE_ID->at(i), LHE_Status->at(i));

    out.push_back(lhe);

  }

  return out;

}

std::vector<Muon> AnalyzerCore::UseTunePMuon(const std::vector<Muon>& muons, double ptboundary){

  std::vector<Muon> out;
  for(unsigned int i=0; i<muons.size(); i++){
    //==== muons is a const vector. So in this function, we have to copy the elements like below
    Muon this_muon=muons.at(i);

    Particle this_tunep4 = this_muon.TuneP4();

    //==== Momentum scaling
    //==== 1) if tuneP Pt < 200 -> Rochester
    //==== 2) if tuneP pt >= 200 -> Generalized Endpoint

    double new_pt( this_tunep4.Pt() ), new_pt_up( this_tunep4.Pt() ), new_pt_down( this_tunep4.Pt() );
    if(this_tunep4.Pt()<ptboundary){

      //==== 19/03/24 (jskim) : For 99% of the muons, MiniAODPt and TunePPt are same
      //==== we can just use MiniAODPt * RochesterCorrection, multiplied by (TuneP Pt)/(MiniAODPt)
      double TunePOverPt = this_tunep4.Pt() / this_muon.MiniAODPt();
      new_pt      = TunePOverPt * this_muon.Pt(); // this_muon.Pt() = MiniAODPt * RochesterCorrection
      new_pt_up   = TunePOverPt * this_muon.MomentumShift(+1);
      new_pt_down = TunePOverPt * this_muon.MomentumShift(-1);

/*
      cout << "## Rochester ##" << endl;
      cout << "this_muon.MiniAODPt() = " << this_muon.MiniAODPt() << endl;
      cout << "this_muon.MiniAODTunePPt() = " << this_muon.MiniAODTunePPt() << endl;
      cout << "new_pt = " << new_pt << endl;
      cout << "new_pt_up = " << new_pt_up << endl;
      cout << "new_pt_down = " << new_pt_down << endl;
*/

    }
    else{

      //==== Unlike rochester, GE method should be only applied to MC

      if(!IsDATA){

        //==== ScaledPts defined in GeneralizedEndpointPt.h ..

        ScaledPts ptvalues;
        //==== TODO FIXME
        //==== 19/09/02 : There is no GEScaleSyst map for 2016
        //if(DataYear==2016) ptvalues = muonGE->GeneralizedEndpointPt(this_tunep4.Pt(), this_tunep4.Charge(), this_tunep4.Eta(), this_tunep4.Phi()*180./M_PI, event);
        //else ptvalues = muonGEScaleSyst->GEPt(DataYear, this_tunep4.Pt(), this_tunep4.Eta(), this_tunep4.Phi(), this_tunep4.Charge());
        //==== 24/10/23
        ptvalues = muonGE->GeneralizedEndpointPt(GetEraShort(), this_tunep4.Pt(), this_tunep4.Charge(), this_tunep4.Eta(), this_tunep4.Phi()*180./M_PI, event);

        new_pt = ptvalues.ScaledPt;
        //==== Mode == 1 : Kappa up
        //==== Mode == 2 : Kappa down
        new_pt_up = ptvalues.ScaeldPt_Up;
        new_pt_down = ptvalues.ScaeldPt_Down;

/*
        cout << "## GeneralizedEndpointPt ##" << endl;
        cout << "old_pt = " << this_tunep4.Pt() << endl;
        cout << "new_pt = " << new_pt << endl;
        cout << "new_pt_up = " << new_pt_up << endl;
        cout << "new_pt_down = " << new_pt_down << endl;
*/

      }

    }

    //==== Scale the pt
    this_muon.SetPtEtaPhiM( new_pt, this_tunep4.Eta(), this_tunep4.Phi(), this_tunep4.M() );
    this_muon.SetMomentumScaleUpDown(new_pt_up,new_pt_down);
    this_muon.SetCharge( this_tunep4.Charge() );
    this_muon.SetMiniAODPt( this_muon.MiniAODTunePPt() );


/*
    cout << "@@@@ TuneP @@@@" << endl;
    cout << "this_muon.Pt() = " << this_muon.Pt() << endl;
    cout << "this_muon.MiniAODPt() = " << this_muon.MiniAODPt() << endl;
    cout << "this_muon.MiniAODTunePPt() = " << this_muon.MiniAODTunePPt() << endl;
    cout << "this_muon.MomentumShift(0) = " << this_muon.MomentumShift(0) << endl;
    cout << "this_muon.MomentumShift(+1) = " << this_muon.MomentumShift(+1) << endl;
    cout << "this_muon.MomentumShift(-1) = " << this_muon.MomentumShift(-1) << endl;
*/

    out.push_back(this_muon);
  }

  std::sort(out.begin(),       out.end(),        PtComparing);
 
  return out;

}



void AnalyzerCore::beginEvent(){
  
  // fill jets first as they are independant
  if(Analyzer.Contains("BDT") && Analyzer.Contains("SkimTree")) PtOrderObj=false;
  else PtOrderObj=true;

  if(!IsData)  {
    All_Gens = GetGens();  
    All_LHES = GetLHEs();
    if(!IsSignal() && (IsDYSample||MCSample.Contains("TTLL"))){
      GetAFBLHEParticles(All_LHES,lhe_p0,lhe_p1,lhe_l0,lhe_l1,lhe_j0);
      GetAFBGenParticles(All_Gens,gen_p0,gen_p1,gen_l0,gen_l1,3);
      GetAFBGenParticles(All_Gens,gen_p0,gen_p1,gen_l0_dressed,gen_l1_dressed,1);
      GetAFBGenParticles(All_Gens,gen_p0,gen_p1,gen_l0_bare,gen_l1_bare,0);

    }
  }
  All_Jets      = GetAllJets();
  All_FatJets   = GetAllFatJets();
  All_Muons     = GetAllMuons();
  All_Electrons = GetAllElectrons();
  _Event = GetEvent();
  

  return;
}
void AnalyzerCore::initializeAnalyzerTools(){

  /*                                                                                                                                                                                                                                                                         
                                                                                                                                                                                                                                                                              
    // In your analyser code add this line to constructor to fill map with JEC source values.                                                                                                                                                                                 
    for(auto jec_source : JECSources)   SetupJECUncertainty(jec_source, "AK4PFchs");                                                                                                                                                                                          
                                                                                                                                                                                                                                                                              
  }                                                                                                                                                                                                                                                                           
    // Then you can get vector of jets with shift calling                                                                                                                                                                                                                     
    std::vector<Jet> AnalyzerCore::ScaleJetsIndividualSource(const std::vector<Jet>& jets, int sys, TString source);                                                                                                                                                          
    vector<Jet> jets_AbsoluteStatUp = ScaleJetsIndividualSource(jets, 1, "AbsoluteStat");                                                                                                                                                                                     
  */


}


double AnalyzerCore::GetKFactor(){

  if(IsDATA) return 1.;

  double weight = 1.;

  if(MCSample.Contains("WZTo3LNu_powheg") or MCSample.Contains("WZTo3LNu_mllmin4p0_powheg") or MCSample.Contains("WZTo2L2Q")){
    //Physics Letters B 761 (2016) 197 
    //http://dx.doi.org/10.1016/j.physletb.2016.08.017 
    weight = 1.109;
  }
  else if(MCSample.Contains("ZZTo4L_powheg") or MCSample.Contains("ZZTo2L2Nu") or MCSample.Contains("ZZTo2L2Q")){
    // Physics Letters B 735 (2014) 311-313
    // https://doi.org/10.1016/j.physletb.2014.06.056
    weight = 1.16; 
  }
  else if(MCSample.Contains("GluGluToZZto")){
    //  1.67 brings gg->ZZ from LO to NLO (http://arxiv.org/abs/1509.06734)
    return 1.67;
  }
  else if(MCSample.Contains("GluGluHToZZ")){
    return 1.67;
    //AN2016_359
  }
  else if(MCSample.Contains("ttZ") && !MCSample.Contains("To")){
    weight = 839.3/780.;
  }
  else if(MCSample.Contains("ttW") && !MCSample.Contains("To")){
    weight = 600.8/610.;
  }
  else if(MCSample.Contains("WJet") && MCSample.Contains("HT")){
    return 1.21;
  }

  if(MCSample.Contains("WZTo3LNu_mllmin0p1_powheg"))     weight = 0.632; //// This is done from WZ CR Norm 
  
  
  return weight;

}


double AnalyzerCore::MCweight(bool usesign, bool norm_1invpb) const {

  if(IsDATA) return 1.;
  double weight=gen_weight;
  
  //MiNNLO sample has some events with unphysically large weight
  if(MCSample.Contains("DYJets")&&MCSample.Contains("MiNNLO")){
    double maxweight=2358.0700*5.;
    if(abs(weight)>maxweight){
      weight=weight>0. ? maxweight : -1.0*maxweight;
    }
  }
  //Sherpa sample needs weighted events
  if(MCSample.Contains("WJets") && MCSample.Contains("Sherpa")){
    usesign = false;
  }

  if(usesign){
    if(weight>0) weight=1.0;
    else if(weight<0) weight=-1.0;
    else weight=0.0;
  }
  if(norm_1invpb){
    if(usesign) weight*=xsec/sumSign;
    else weight*=xsec/sumW;
  }
  return weight;
}


bool AnalyzerCore::IsOnZ(double m, double width){
  if( fabs(m-M_Z) < width ) return true;
  else return false;
}

double AnalyzerCore::MT(TLorentzVector a, TLorentzVector b){
  double dphi = a.DeltaPhi(b);
  return TMath::Sqrt( 2.*a.Pt()*b.Pt()*(1.- TMath::Cos(dphi) ) );
}

double AnalyzerCore::MT2(TLorentzVector a, TLorentzVector b, Particle METv, double METgap){

  TLorentzVector METa, METb;
  METa.SetPxPyPzE( 0., 0., 0., 0.);
  double MTa, MTb;
  double tempMETa =0., tempMT2 = TMath::Max(MT(a, METv), MT(b, METv));

  while(tempMETa < METv.Pt()){

    METa.SetPxPyPzE(tempMETa*TMath::Cos(METv.Phi()), tempMETa*TMath::Sin(METv.Phi()), 0., tempMETa);
    METb = METv - METa;

    MTa = MT(METa, a);
    MTb = MT(METb, b);

    tempMT2 = TMath::Min(tempMT2, TMath::Max(MTa, MTb));

    tempMETa = tempMETa + METgap;
  }  

  return tempMT2;

}

double AnalyzerCore::projectedMET(TLorentzVector a, TLorentzVector b, Particle METv){

  if( fabs(a.DeltaPhi(METv)) < fabs(b.DeltaPhi(METv)) ){
    return (METv.Pt() * TMath::Sin(fabs(a.DeltaPhi(METv))) );
  }
  else return (METv.Pt() * TMath::Sin(fabs(b.DeltaPhi(METv))) );

}

bool AnalyzerCore::HasFlag(TString flag){

  //cout << "[AnalyzerCore::HasFlag] Userflags.size() = " << Userflags.size() << endl;
  //for(unsigned int i=0; i<Userflags.size(); i++){
  //  cout << "[AnalyzerCore::HasFlag] " << Userflags.at(i) << endl;
  //}
  
  return std::find(Userflags.begin(), Userflags.end(), flag) != Userflags.end();

}

std::vector<Muon> AnalyzerCore::MuonWithoutGap(const std::vector<Muon>& muons){

  std::vector<Muon> out;
  for(unsigned int i=0; i<muons.size(); i++){
    double this_eta = fabs( muons.at(i).Eta() );
    if( 1.444 <= this_eta && this_eta < 1.566 ) continue;

    out.push_back( muons.at(i) );
  }
  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}




bool AnalyzerCore::HasEWLepInJet(Jet Jet, vector<Gen>& TruthColl, TString Option){

  int HasEWLep=false;
  bool InclEWTau=Option.Contains("InclTau");
  for(unsigned int i=2; i<TruthColl.size(); i++){
    if(TruthColl.at(i).MotherIndex()<0 )  continue;
    bool IsCand=false;
    int abspid=fabs(TruthColl.at(i).PID());
    int absmpid=fabs(TruthColl.at(TruthColl.at(i).MotherIndex()).PID());
    if( TruthColl.at(i).Status()==1 ){
      if( abspid==11 || abspid==13 ){
        int LepType=GetLeptonType_JH(i, TruthColl);
        if( LepType==1 || LepType==2 || LepType==3 ) IsCand=true; 
      }
    }
    if( InclEWTau && abspid==15 && TruthColl.at(i).Status()>20 && TruthColl.at(i).Status()<30 ) IsCand=true;
    else if( InclEWTau && abspid==15 && (absmpid==23 || absmpid==24) && TruthColl.at(i).Status()==2 ) IsCand=true;

    if( IsCand && TruthColl.at(i).DeltaR(Jet)<0.4) HasEWLep=true;
    if( HasEWLep ) break;
  }

  return HasEWLep;
}


std::vector<Muon> AnalyzerCore::MuonPromptOnly(const std::vector<Muon>& muons, const std::vector<Gen>& gens){

  if(IsDATA) return muons;

  std::vector<Muon> out;

  for(unsigned int i=0; i<muons.size(); i++){
    if(muons.at(i).LeptonGenType() <= 0) continue;
    out.push_back( muons.at(i) );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}

std::vector<Muon> AnalyzerCore::MuonPromptOnly(const std::vector<Muon>& muons, const std::vector<Gen>& gens, AnalyzerParameter param){

  if(IsDATA) return muons;

  std::vector<Muon> out;

  for(unsigned int i=0; i<muons.size(); i++){
    if(muons.at(i).LeptonGenType() <= 0) continue;
    
    out.push_back( muons.at(i) );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}


TString AnalyzerCore::PromptStatus(Muon mu, const std::vector<Gen>& gens){

  if(GetLeptonType(mu, gens)>0) return "Prompt";
  else return "Fake";

}


std::vector<Muon> AnalyzerCore::MuonUsePtCone(const std::vector<Muon>& muons){

  std::vector<Muon> out;

  for(unsigned int i=0; i<muons.size(); i++){
    //==== muons is a const vector. So in this function, we have to copy the elements like below
    Muon this_muon = muons.at(i);
    this_muon.SetPtEtaPhiM( muons.at(i).PtCone(), muons.at(i).Eta(), muons.at(i).Phi(), muons.at(i).M() );
    out.push_back( this_muon );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}

Muon AnalyzerCore::MuonUsePtCone(const Muon& muon){

  //==== muon is a const object. So in this function, we have to copy the object like below
  Muon this_muon = muon;
  this_muon.SetPtEtaPhiM( muon.PtCone(), muon.Eta(), muon.Phi(), muon.M() );
  return this_muon;

}



std::vector<Muon> AnalyzerCore::MuonApplyPtCut(const std::vector<Muon>& muons, double ptcut){

  std::vector<Muon> out;

  for(unsigned int i=0; i<muons.size(); i++){
    if(!( muons.at(i).Pt() > ptcut )) continue;
    out.push_back( muons.at(i) );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}

std::vector<Electron> AnalyzerCore::ElectronPromptOnly(const std::vector<Electron>& electrons, const std::vector<Gen>& gens){

  if(IsDATA) return electrons;

  std::vector<Electron> out;
  for(unsigned int i=0; i<electrons.size(); i++){
    bool pass=true;
    if(electrons.at(i).IsFake()) pass=false;
    //    if(electrons.at(i).LeptonGenType() <= 0) pass=false;
    //    if(electrons.at(i).LeptonGenType() >= 4) pass=false;
    if(pass)out.push_back( electrons.at(i) );
  }
  
  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;
  
}

std::vector<Electron> AnalyzerCore::ElectronPromptOnly(const std::vector<Electron>& electrons, const std::vector<Gen>& gens, AnalyzerParameter param){

  if(IsDATA) return electrons;

  std::vector<Electron> out;
  for(unsigned int i=0; i<electrons.size(); i++){
    bool pass=true;
    if(electrons.at(i).LeptonGenType() <= 0) pass=false;
    if(pass) out.push_back(electrons.at(i));
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}

std::vector<Electron> AnalyzerCore::ElectronUsePtCone(const std::vector<Electron>& electrons){

  std::vector<Electron> out;

  for(unsigned int i=0; i<electrons.size(); i++){
    //==== electrons is a const vector. So in this function, we have to copy the elements like below
    Electron this_electron = electrons.at(i);
    this_electron.SetPtEtaPhiM( electrons.at(i).PtCone(), electrons.at(i).Eta(), electrons.at(i).Phi(), electrons.at(i).M() );
    out.push_back( this_electron );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}

Electron AnalyzerCore::ElectronUsePtCone(const Electron& electron){

  //==== electron is a const object. So in this function, we have to copy the object like below
  Electron this_electron = electron;
  this_electron.SetPtEtaPhiM( electron.PtCone(), electron.Eta(), electron.Phi(), electron.M() );
  return this_electron;

}

std::vector<Electron> AnalyzerCore::ElectronApplyPtCut(const std::vector<Electron>& electrons, double ptcut){

  std::vector<Electron> out;

  for(unsigned int i=0; i<electrons.size(); i++){
    if(!( electrons.at(i).Pt() > ptcut )) continue;
    out.push_back( electrons.at(i) );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}



std::vector<Jet> AnalyzerCore::JetsVetoLeptonInside(const std::vector<Jet>& jets, const std::vector<Electron>& els, const std::vector<Muon>& mus, double dR){

  std::vector<Jet> out;
  for(unsigned int i=0; i<jets.size(); i++){

    bool HasLeptonInside = false;

    for(unsigned int j=0; j<els.size(); j++){
      if( jets.at(i).DeltaR( els.at(j) ) < dR ){
        HasLeptonInside = true;
        break;
      }
    }
    if(HasLeptonInside) continue;

    for(unsigned int j=0; j<mus.size(); j++){
      if( jets.at(i).DeltaR( mus.at(j) ) < dR ){
        HasLeptonInside = true;
        break;
      }
    }
    if(HasLeptonInside) continue;

    //==== if all fine,
    out.push_back( jets.at(i) );

  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}

std::vector<FatJet> AnalyzerCore::FatJetsVetoLeptonInside(const std::vector<FatJet>& jets, const std::vector<Electron>& els, const std::vector<Muon>& mus, double dR){

  std::vector<FatJet> out;
  for(unsigned int i=0; i<jets.size(); i++){
    FatJet this_jet = jets.at(i);

    bool HasLeptonInside = false;

    for(unsigned int j=0; j<els.size(); j++){
      if( this_jet.DeltaR( els.at(j) ) < dR ){
        HasLeptonInside = true;
        break;
      }
    }
    if(HasLeptonInside) continue;

    for(unsigned int j=0; j<mus.size(); j++){
      if( this_jet.DeltaR( mus.at(j) ) < dR ){
        HasLeptonInside = true;
        break;
      }
    }
    if(HasLeptonInside) continue;

    //==== if all fine,
    out.push_back( this_jet );

  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}

std::vector<Jet> AnalyzerCore::JetsAwayFromPhoton(const std::vector<Jet>& jets, const std::vector<Photon>& photons, double mindr){
  
  std::vector<Jet> out;
  for(unsigned int i=0; i<jets.size(); i++){
    
    bool Overlap = false;
    for(unsigned int j=0; j<photons.size(); j++){
      if( ( jets.at(i) ).DeltaR( photons.at(j) ) < mindr ){
        Overlap = true;
        break;
      }
    }
    if(!Overlap) out.push_back( jets.at(i) );

  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}


Particle AnalyzerCore::AddFatJetAndLepton(const FatJet& fatjet, const Lepton& lep){

  if(fatjet.DeltaR( lep )<0.8){
    return fatjet;
  }
  else{
    return fatjet+lep;
  }

}


bool AnalyzerCore::SelectEvent(int evnumber){

  
  if(int(event) == evnumber) return true;
  else return false;
			
}

void AnalyzerCore::PrintEvent(AnalyzerParameter param,TString selection,double w){

  // if run_timestamp is not set dont run                                                                                                        
  if (timestamp == "-999999") return;

  FillEventComparisonFile(param,selection,string(timestamp), w);

  cout <<  "selection = " << selection << " RunNumber:EventNumber = " << run << ":" << event << endl;

}



Gen AnalyzerCore::GetGenMatchedLepton(const Lepton& lep, const std::vector<Gen>& gens){

  //==== find status 1 lepton

  int reco_PID = -999;
  if(lep.LeptonFlavour()==Lepton::ELECTRON) reco_PID = 11;
  else if(lep.LeptonFlavour()==Lepton::MUON) reco_PID = 13;
  else{
    cout << "[AnalyzerCore::GetGenMatchedLepton] input lepton flavour not set" << endl;
    exit(EXIT_FAILURE);
  }

  double min_dR = 0.1;
  Gen gen_closest;
  for(unsigned int i=2; i<gens.size(); i++){

    Gen gen = gens.at(i);

    //==== Status 1
    if( gen.Status() != 1 ) continue;
    //==== PID
    if( abs( gen.PID() ) != reco_PID ) continue;
    //==== reject ISR?
    if( gen.MotherIndex() < 0 ) continue;
    //==== dR matching
    if( gen.DeltaR( lep ) < min_dR ){
      min_dR = gen.DeltaR( lep ) ;
      gen_closest = gen;
    }

  }

  return gen_closest;

}

Gen AnalyzerCore::GetGenMatchedPhoton(const Lepton& lep, const std::vector<Gen>& gens){
//Find if there is a photon candidate for a source of external conversion (similar pt, direction of lepton)

  double min_dR = 0.2;//1)
  Gen gen_closest;
  double pt_min = 10., dPtRelmax=0.5, dRmax=0.2;//1)
  for(unsigned int i=2; i<gens.size(); i++){

    Gen gen = gens.at(i);

    if( gen.MotherIndex() < 0 ) continue;
    if( !( abs(gen.PID())==22 && (gen.Status()==1 || gen.Status()==23) ) ) continue;//2)
    if( gen.Pt() < pt_min ) continue;
    if( !(lep.Pt()/gen.Pt()>(1.-dPtRelmax) && lep.Pt()/gen.Pt()<(1.+dPtRelmax)) ) continue;
    if( gen.DeltaR( lep ) > dRmax ) continue;
    if( gen.Status()==23 && !IsFinalPhotonSt23_Public(gens) ) continue;//2)

    if( gen.DeltaR( lep ) < min_dR ){
      min_dR = gen.DeltaR( lep ) ;
      gen_closest = gen;
    }

  }

  return gen_closest;

//1) Validity of cuts are checked for electron with PT>10. Note that the previous dPtRel<0.2 cut in the CatNtuple analysis was optimized for PT(e)>25.
//   External conversion rate proportionate to M^{-2}, thus muon external conversion rate is negligible.
//2) As the algorithm targets stable photon near lepton, only status-1 photon should be investigated theoretically. 
//   However, in some events, last status in photon history is 23. This is believed to be due to skimming of GEN history between pythia & MiniAOD. 
//   This case is also covered here. Note that IsFinalPhotonSt23 has to analyze whole history, i.e. computationally heavier part in this function.
//   Therefore it is put at last part of criteria for efficient functioning.
//For more details about the optimization, ask J.B..
}

vector<int> AnalyzerCore::TrackGenSelfHistory(const Gen& me, const std::vector<Gen>& gens){
//returns {index of the first history of the gen, index of the last history of the gen's mother}

  int myindex = me.Index();

  if(myindex<2){
    vector<int> out = {myindex, -1};
    return out;
  }

  int mypid = gens.at(myindex).PID();

  int currentidx = myindex;
  int motherindex = me.MotherIndex();

  while(gens.at(motherindex).PID() == mypid){

    //==== Go one generation up
    currentidx = motherindex;
    motherindex = gens.at(motherindex).MotherIndex();

    if(motherindex<0) break;
  }

  vector<int> out = {currentidx, motherindex};

  return out;

}

bool AnalyzerCore::IsFromHadron(const Gen& me, const std::vector<Gen>& gens){

  bool out = false;

  int myindex = me.Index();
  if(myindex<2) return true;

  vector<int> my_history = TrackGenSelfHistory(me, gens);
  Gen          Start = gens.at( my_history[0] );
  Gen MotherOf_Start = gens.at( my_history[1] );

  //==== Status 21~29 are from hardprocess
  //==== Means it's lepton from hardprocess
  //==== e.g., leptons from Z start their lives with status 23
  if( 20 < Start.Status() && Start.Status() < 30 ) return false;

  Gen current_me = Start; // me will always be Start
  Gen current_mother = Start; // initializing
  while( current_mother.Index() >= 2 ){

    vector<int> current_history = TrackGenSelfHistory(current_me, gens);

    //==== Go one generation up
    //==== not being used after this line
    //==== not a bug!!
    current_me = gens.at(current_history[1]);

    //==== Now look at mother of previous "me"
    current_mother = gens.at(current_history[1]);

    vector<int> current_mother_history = TrackGenSelfHistory(current_mother, gens);
    Gen StartOf_current_mother = gens.at(current_mother_history[0]);
    int current_mother_PID = current_mother.PID();

    //==== From Z,W,H,t
    if( current_mother_PID==23 || current_mother_PID==24 || current_mother_PID==25 || current_mother_PID==6 ){
      out = false;
      break;
    }
    //==== From Signal
    if( IsSignalPID(current_mother_PID) ){
      out = false;
    }
    if( (current_mother_PID==11 || current_mother_PID==13 || current_mother_PID==15 || current_mother_PID==22) && (StartOf_current_mother.Status()>20 && StartOf_current_mother.Status()<30)){
      out = false;
      break;
    }
    if( current_mother_PID>50 ){
      out=true;
      break;
    }
    if( (current_mother_PID>=1 && current_mother_PID<=5) || current_mother_PID==21 ){
      out=true; 
      break;
    }

  }

  return out;

}


int AnalyzerCore::GetPrElType_InSameSCRange_Public(int TruthIdx, const std::vector<Gen>& TruthColl){
//Abbreviation: Get Prompt Electron Type In Same Supercluster Range (Public (shared) version of J.B.'s original gen-matching code)
//Prompt e>eee (int. conv.) case, collimated electrons can be merged in one SC & track is selected among them, and reconstructed as single electron. 
//In this case, still there will be a nearby prompt electron of LepType 1-3 within supercluster merging range.
//Supercluster merging range: |dphi|<0.3/0.3 (EB/EE), |deta|<0/0.07 (EB/EE) (additionally put 0.03 for marginal difference from size of crystals)
//Ref:JINST 10 (2015) P06005, arXiv:1502.02701
//Return Value: 1/2/3:LeptonType-1/2/3 electron found in same SC range
//             -1: No LeptonType-1/2/3 electron found in same SC range
//
//Note: Hadronic origins are not included in this algo. as I am not sure of effect of many nearby particles in jets on the ele-reco performance.
//      + it is not of my interest yet.

  if(TruthIdx<2) return false;
  if(abs(TruthColl.at(TruthIdx).PID())!=11) return false;
  if(TruthColl.at(TruthIdx).Status()!=1) return false;

  double dPhiMax=0.3, dEtaMax=0.1;
  int NearbyElType=-1;

  for(unsigned int it_gen=2; it_gen<TruthColl.size(); it_gen++){
    if(TruthColl.at(it_gen).Status()!=1) continue;
    if(abs(TruthColl.at(it_gen).PID())!=11) continue;
    if(fabs(TruthColl.at(TruthIdx).Eta()-TruthColl.at(it_gen).Eta())>dEtaMax) continue;
    if(fabs(TruthColl.at(TruthIdx).DeltaPhi(TruthColl.at(it_gen)))>dPhiMax) continue;

    int LepType = GetLeptonType_Public(it_gen, TruthColl);
    if(!(LepType>=1 && LepType<=3)) continue;
    NearbyElType  = LepType;
  }

  return NearbyElType;
}


int AnalyzerCore::GetLeptonType_Public(int TruthIdx, const std::vector<Gen>& TruthColl){
//Type : 1:EW-Prompt / 2: BSM-Prompt / 3:EW/BSM-Prompt-Tau Daughter 
//       4:Internal Conversion from Soft QED Radiation (PS-level) / 5:Internal Conversion from Hard Process Photon (ME-level)
//      -1:Unmatched & not EW Conversion candidate (mis-reco. or external conversion within jets)
//      -2:Hadron Daughter / -3:Daughter of Tau from Hadron or Parton / -4:Internal Conversion Daughter having hadronic origin
//      -5:External Conversion from Hard process photon / -6:External conversion from t/EWV/EWlep
//      (-4:Daughter of Non-hard scattered photon & has parton or hadron ancestor OR implicit Conv from quark)
//       0:Error / >0: Non-fake: Non-hadronic origin / <0 : Fakes: Hadronic origin or external conversion
//Note: There is no physical distinction between type 4 vs. 5 and type -5 vs. -6. The distinction is only intended for straightforward debugging.


  //Only consider Status 1 lepton
  if(TruthIdx<2) return 0;
  if(TruthColl.at(TruthIdx).Status()!=1) return 0;
  if( !(fabs(TruthColl.at(TruthIdx).PID())==11 || fabs(TruthColl.at(TruthIdx).PID())==13) ) return 0;


  int LeptonType=0;
  int MPID=0, GrMPID=0;
  vector<int> my_history, mom_history, grmom_history;
  int LastSelfIdx=-1, MotherIdx=-1, LastSelfMIdx=-1, GrMotherIdx=-1, LastSelfGrMIdx=-1;
  int Status_orig=0, MStatus_orig=0, MStatus_last=0, GrMStatus_orig=0, GrMStatus_last=0;
  bool HadronicOrigin = false;

  my_history      = TrackGenSelfHistory(TruthColl.at(TruthIdx), TruthColl);
  LastSelfIdx     = my_history[0];
  MotherIdx       = my_history[1];
  Status_orig     = TruthColl.at(LastSelfIdx).Status();
  HadronicOrigin  = IsFromHadron(TruthColl.at(TruthIdx), TruthColl);
  
  if(   MotherIdx!=-1   ){ mom_history  = TrackGenSelfHistory(TruthColl.at(MotherIdx), TruthColl);
                           LastSelfMIdx = mom_history[0];
                           GrMotherIdx  = mom_history[1];
                           MPID         = TruthColl.at(MotherIdx).PID();
                           MStatus_orig = TruthColl.at(LastSelfMIdx).Status();
                           MStatus_last = TruthColl.at(MotherIdx).Status();
                         }
  if(  GrMotherIdx!=-1  ){ grmom_history  = TrackGenSelfHistory(TruthColl.at(GrMotherIdx), TruthColl);
                           LastSelfGrMIdx = grmom_history[0];
                           GrMPID         = TruthColl.at(GrMotherIdx).PID();
                           GrMStatus_orig = TruthColl.at(LastSelfGrMIdx).Status();
                           GrMStatus_last = TruthColl.at(GrMotherIdx).Status();
                         }

  if     ( TruthIdx==-1 )                                       LeptonType= 0;
  else if( fabs(MPID)==23 || fabs(MPID)==24 || fabs(MPID)==25 ) LeptonType= 1;
  else if( IsSignalPID(MPID) )                                  LeptonType= 2;
  else if( Status_orig>20 && Status_orig<30 )                   LeptonType= 1;//1)
  else if( fabs(MPID)>50 )                                      LeptonType=-2;
  else if( fabs(MPID)==15 && MStatus_last==2 ){
           if     ( fabs(GrMPID)==23 || fabs(GrMPID)==24 || fabs(GrMPID)==25 ) LeptonType= 3;
           else if( IsSignalPID(GrMPID) )                                      LeptonType= 3;
           else if( MStatus_orig>20  && MStatus_orig<30  )                     LeptonType= 3;//1)
           else if( HadronicOrigin )                                           LeptonType=-3;//2-a)
           else if( fabs(GrMPID)==22  && GrMStatus_orig>20 && GrMStatus_orig<30 )                     LeptonType= 5;//2-b)
           else if( fabs(GrMPID)==22 )                                                                LeptonType= 4;//2-c)
           else if( (fabs(GrMPID)==11 || fabs(GrMPID)==13 || fabs(GrMPID)==15) && GrMStatus_last!=2 ) LeptonType= 4;//2-d)
           else                                                                                       LeptonType= 0;
         }
  else if( fabs(MPID)==22 ){
           if( MStatus_orig>20 && MStatus_orig<30 )                            LeptonType= 5;//3-a)
           else if( HadronicOrigin )                                           LeptonType=-4;//3-b)
           else if( fabs(GrMPID)==24 || fabs(GrMPID)==23 || fabs(GrMPID)==6  ) LeptonType= 4;//3-c)
           else if( fabs(GrMPID)==11 || fabs(GrMPID)==13 || fabs(GrMPID)==15 ) LeptonType= 4;//3-d)
           else                                                                LeptonType= 0;
         }
  else if( (fabs(MPID)==11 || fabs(MPID)==13 || fabs(MPID)==15) && MStatus_last!=2 && !HadronicOrigin ) LeptonType= 4;//4-a)
  else if( ((fabs(MPID)>=1 && fabs(MPID)<=5) || fabs(MPID)==21) && MStatus_last!=2 )                    LeptonType=-4;//4-b)
  else if( fabs(MPID)==6 ) LeptonType=4;//4-c)
  else LeptonType=0;


  return LeptonType;

//**footnote
//These are based on observation in DY,ZG,TT sample(DY,ZG:amcnlo+pythia, TT:powheg+pythia) for other PS generator, convention may differ.
//1) In MG5aMC@NLO generator, output of ME level generation does not have specific guage field mother if it is not on-shell specific generation.
//   e.g. u u~ > l+ l- -> fabs(MID)=1 .
//   This line is put after checking prompt mother PID checking, in order to distinguish type 1 and 2.
//2-a) e.g. a)Had > ta+X, ta>l+2nu b) q>ta+X in jet fragmentation (ta is not from hard process (ME), as it was already checked in prev. step)
//2-b) e.g. gamma>ta(+)+ta(-)+X, ta>lnu (St=2)
//2-c) e.g. " " " " " " " " " " " " " " " " " ", but soft gamma case. this is not observed in test sample but put here just in case.
//          (hadronic origin was already checked before, so no need to worry about contamination from hadronic origins at this line)
//2-d) e.g. l>tata..+l.. , ta>l+2nu (tau pair produced from internal conversion), photon radiation is not decay: GenStatus!=2
//3-a) e.g. hard process gamma>ll
//3-b) e.g. a) Had>gamma+X, gamma>ll+X (in PS+Had stage intermediate process is omitted you see just Had>Nphoton+Mhadrons+..)
//          b) q>gamma+q, gamma>ll+X in jet fragmentation or radiations of tops.
//          c) gluon>Ngamma+Mhadrons in jet fragmentation (actually observed in samples)
//3-c) e.g. W+>W+ gamma, or t>t+gamma, gamma>ll+X, not yet observed in test sample but possible (upto radiation is observed so far)
//3-d) e.g. ta>ta+gamma, gamma>ll+X, tau not from hadron(e.g. pp>tata)
//4-a) e.g. EW lep l, l>lll... just implicit conversion. 
//4-b) e.g. q or g> Nlepton +MHadrons... in parton shower history
//4-c) e.g. t>t+ll.. implicit conversion
}



int AnalyzerCore::GetLeptonType(const Lepton& lep, const std::vector<Gen>& gens){

  //==== [Type]
  //====  1 : EWPrompt
  //====  2 : Signal Daughter
  //====  3 : EWtau daughter
  //====  4 : Internal Conversion daughter from t/EWV/EWlep(Implicit,Explicit)
  //====  5 : Internal Conversion daughter from HardScatterPhoton
  //==== -1 : Unmatched & not EW Conversion candidate
  //==== -2 : Hadron daughter
  //==== -3 : Daughter of tau from hadron or parton
  //==== -4 : Internal conversion daughter(implicit,explicit) having hadronic origin 
  //==== -5 : External conversion candidate(Hard scattered photon)
  //==== -6 : External conversion from t/EWV/EWlep
  //==== (-4: Daughter of Non-hard scattered photon & has parton or hadron ancestor OR implicit Conv from quark)
  //==== -------------------------------------------------------------
  //====  0 : Error
  //==== >0 : Non-fake; Non-hadronic origin
  //==== <0 : Fakes   ; Hadronic origin or external conversion

  Gen gen_closest = GetGenMatchedLepton(lep, gens); // if gen_closest.IsEmpty(), we should then look for a near photon
  //cout << "[AnalyzerCore::GetLeptonType] [Reco] pt = " << lep.Pt() << "\t, eta = " << lep.Eta() << endl;
  //cout << "[AnalyzerCore::GetLeptonType] [Gen] Index = " << gen_closest.Index() << endl;

/*
  //==== 1) Using Simple Flags
  //==== Prompt
  if( gen_closest.isPromptFinalState() ){
    return 1;
  }
  //=== From Tau
  else if( gen_closest.isDirectPromptTauDecayProductFinalState() ){
    return 2;
  }
  else{
    //=== Fake
    return -1;
  }
*/  

  //==== 2) Use Gen history

  int LeptonType=0, MatchedTruthIdx=-1;
  //==== No matched gen lepton
  if( gen_closest.IsEmpty() ){
    Gen gen_photon_closest = GetGenMatchedPhoton(lep, gens);
    int NearPhotonType = GetGenPhotonType(gen_photon_closest,gens);
    if     ( NearPhotonType<=0 ) LeptonType=-1;//1)
    else if( NearPhotonType==1 ) LeptonType=-5;//2)
    else if( NearPhotonType==2 ) LeptonType=-6;//3)
  }
  //==== Has macthed gen lepton
  else{
    MatchedTruthIdx = gen_closest.Index();
    LeptonType = GetLeptonType_Public(MatchedTruthIdx, gens);//4)
    if(LeptonType>=4 && LeptonType<=5){//5)
      int NearbyPrElType = GetPrElType_InSameSCRange_Public(MatchedTruthIdx, gens);
      if(NearbyPrElType>0) LeptonType = NearbyPrElType;
    }
  }

  return LeptonType;

//**footnote
//1) matched to no gen-lepton nor gen-photon -> mis-reco.
//   matched to no gen-lepton, but to photon with hadronic origin -> mis-reco. (e.g. pions->e) or external conversion from photon in jets
//2) matched to no gen-lepton, but to photon with non-hadronic origin (hard process) -> external conversion from photon with non-hadronic source (ME-level)
//3) matched to no gen-lepton, but to photon with non-hadronic origin (soft QED radiation) -> external conversion from photon with non-hadronic source (PS-level)
//4) matched to gen-lepton, categorize based on the truth categorization algo.: AnalyzerCore::GetLeptonType_Public(int TruthIdx, std::vector<Gen>& TruthColl)
//5) collimated e/gm objects are merged in SC, hence if there is prompt electron within SC-merging range, reco-electron's properties actually represent pre-QED-FSR prompt-electron,
//   rather than the closest internal conversion electron. Therefore shift the type to the prompt lepton's type.
//- Note: distinction between type 4 vs. 5 and type -5 vs. -6 is unphysical. it is only for debugging.
}

int AnalyzerCore::GetGenPhotonType(const Gen& genph, const std::vector<Gen>& gens){

  //==== [Type]
  //====  0 : Invalid input or Error or HardScatter is input when hardscatter is not final state
  //====  1 : HardScatter / 2: Else prompt daughter(l,V,t)
  //==== -1 : Reserved for unmatched(Not used now) / -2: Hadronic origin

  if(genph.IsEmpty()) return 0;

  int genph_index = genph.Index();

  if( genph_index<2 ) return 0;
  if( !(gens.at(genph_index).PID()==22 && (gens.at(genph_index).Status()==1 || gens.at(genph_index).Status()==23)) ) return 0;

  if(gens.at(genph_index).Status()==23){
    if(IsFinalPhotonSt23_Public(gens)) return 1;
    else                               return 0;
  }//From this pt, only St1 Photon is treated.

  vector<int> phhist = TrackGenSelfHistory(genph, gens);

  Gen          Start = gens.at(phhist[0]);
  Gen MotherOf_Start = gens.at(phhist[1]);
  int MotherOf_Start_PID = abs(MotherOf_Start.PID()); // |PID|

  bool fromhadron = IsFromHadron(genph, gens);

  if     ( 20 < Start.Status() && Start.Status() < 30 ) return 1;//1)
  else if( MotherOf_Start_PID==23 || MotherOf_Start_PID==25) return 1;//2)
  else if( MotherOf_Start_PID==24 || MotherOf_Start_PID==6 || IsSignalPID(MotherOf_Start_PID) ) return 2;//3)
  else if( fromhadron ) return -2;//4)
  else if( MotherOf_Start_PID==11 || MotherOf_Start_PID==13 || MotherOf_Start_PID==15 ) return 2;//5)
  else return 0;

//*footnote
//1) Hard process photon's original status is 20's, unless the history is skimmed in MiniAOD.
//2) If part of intermediate history is skimmed at MinoAOD, then last photon status is 1 without any preceding history lines.
//   In that case, intermediate mother is written in history, and this line catches the case.
//3) top and charged bosons can radiate photons.
// - Note that distinction between 1 and 2 is not physical. You should use both 1 & 2 for prompt photons.
//4) this category does not include top, but photons from hadrons and quarks. Predominantly, in most of the cases they are daughter of pi0.
//   Rarer, but other mesons as eta, B, or even some quarks can also radiate energetic photons.
//5) Photons radiated from lepton FSR. Sometimes they are quite energetic.
}


bool AnalyzerCore::IsFinalPhotonSt23_Public(const std::vector<Gen>& TruthColl){
//In Some XG proc events, it is observed that some of photons' last status is 23. Presumably due to skimming of generator history between pythia and MiniAOD.
//The function returns if this is the case.
//And this is designed only for 1 hard photon case as W+G or Z+G or TT+G

  bool IsFinalGammaStatus23 = false;
  bool HasStatus23Photon    = false;
  for(unsigned int i=2; i<TruthColl.size(); i++){
    int fpid  = fabs(TruthColl.at(i).PID());
    int GenSt = TruthColl.at(i).Status();
    int MPID_direct= TruthColl.at(TruthColl.at(i).MotherIndex()).PID();
    if( !((fpid!=22 && MPID_direct==22) || (fpid==22 && (GenSt==23||GenSt==1))) ) continue;

    vector<int> my_history, mom_history;
    my_history  = TrackGenSelfHistory(TruthColl.at(i), TruthColl);
    int LastSelfIdx     = my_history[0];
    int MotherIdx       = my_history[1];
    int LastSelfSt      = TruthColl.at(LastSelfIdx).Status();
    int LastSelfMIdx=-1, MStatus_orig=-1;
    if(MotherIdx!=-1){
      mom_history  = TrackGenSelfHistory(TruthColl.at(MotherIdx), TruthColl);
      LastSelfMIdx = mom_history[0];
      MStatus_orig = TruthColl.at(LastSelfMIdx).Status();
    }

    if(fpid==22){
      if(GenSt==23) {HasStatus23Photon=true; IsFinalGammaStatus23=true;}
      else if(GenSt==1 && LastSelfSt==23) {IsFinalGammaStatus23=false; break;}//a)
    }
    else if( MPID_direct==22 && MStatus_orig==23 ){ IsFinalGammaStatus23=false; break;}//b)
  }

  if(!HasStatus23Photon) return false;

  return IsFinalGammaStatus23;

//**footnotes
//a) Status-23 photon's last is 1. Thus status-23 photon is not the last history.
//b) Daughter particle of status-23 photon is found. Thus status-23 photon is not the last history.
}


bool AnalyzerCore::IsSignalPID(int pid){

  pid = abs(pid);

  //==== HeavyNeutrino
  if(pid>=9900000) return true;
  //==== ChargedHiggs
  if(pid==36 || pid==37) return true;

  return false;

}

//==== END Gen Matching Tools
//==============================================================

int AnalyzerCore::GetPartonType_JH(int TruthIdx, std::vector<Gen>& TruthColl){
  //Type : 1:W decay product (LO sample)
  //       Currently have interest only in parton from W decay
  //       0:Error/Non classified 

  //Only consider Status 1 lepton
  if(TruthIdx<2) return 0;
  if( !(fabs(TruthColl.at(TruthIdx).PID())>0 && fabs(TruthColl.at(TruthIdx).PID())<10) ) return 0;

  int PartonType=0;
  int MotherIdx       = TruthColl.at(TruthIdx).MotherIndex();

  int MPID=0;
  //  int Status_now=0;
  //    if(    TruthIdx!=-1   ){ Status_now    = TruthColl.at(TruthIdx).Status();
  //                           }                           
  if(   MotherIdx!=-1   ){ MPID         = TruthColl.at(MotherIdx).PID();
  }
 
  if     ( TruthIdx==-1 )                                       PartonType= 0;
  else if( fabs(MPID)==23 || fabs(MPID)==24 || fabs(MPID)==25 ) PartonType= 1;
  else PartonType=0;

  return PartonType;
}


int AnalyzerCore::GetPrElType_InSameSCRange(int TruthIdx, std::vector<Gen>& TruthColl, TString Option){
  //Abbreviation: Get Prompt Electron Type In Same Supercluster Range
  //Pr. e>eee (int. conv.) case, collimated electrons can be merged in one SC & track is selected among them, and reconstructed as single electron. 
  //In this case, still there will be a nearby prompt electron of LepType 1-3 within supercluster merging range.
  //Supercluster merging range: |dphi|<0.3/0.3 (EB/EE), |deta|<0/0.07 (EB/EE) (+marginal 0.03 from 2 crystal size of 5x5 clusters)
  //Ref:JINST 10 (2015) P06005, arXiv:1502.02701
  //Return Value: 1/2/3:LeptonType-1/2/3 electron found in same SC range
  //             -1: No LeptonType-1/2/3 electron found in same SC range
  //
  //Note: Hadronic origins are not included in this algo. as I am not sure of effect of many nearby particles in jets on the ele-reco performance.
  //      + it is not of my interest yet.

  if(TruthIdx<2) return 0;
  if(abs(TruthColl.at(TruthIdx).PID())!=11) return 0;
  if(TruthColl.at(TruthIdx).Status()!=1) return 0;

  double dPhiMax=0.3, dEtaMax=0.1;
  int NearbyElType=-1, NearbyPrElIdx=-1;

  for(unsigned int it_gen=2; it_gen<TruthColl.size(); it_gen++){
    if(TruthColl.at(it_gen).Status()!=1) continue;
    if(abs(TruthColl.at(it_gen).PID())!=11) continue;
    if(fabs(TruthColl.at(TruthIdx).Eta()-TruthColl.at(it_gen).Eta())>dEtaMax) continue;
    if(fabs(TruthColl.at(TruthIdx).DeltaPhi(TruthColl.at(it_gen)))>dPhiMax) continue;

    int LepType = GetLeptonType_JH(it_gen, TruthColl);
    if(!(LepType>=1 && LepType<=3)) continue;
    NearbyElType  = LepType;
    NearbyPrElIdx = it_gen;
  }

  if     (Option.Contains("IdxType")){ return (NearbyElType>0? NearbyPrElIdx*10+NearbyElType:-1); }
  else if(Option.Contains("Idx"))    { return NearbyPrElIdx; }
  
  return NearbyElType;
  
}


int AnalyzerCore::GetLeptonType_JH(int TruthIdx, std::vector<Gen>& TruthColl){
  //Type : 1:EWPrompt  /  2:Signal Daughter /  3:EW/Sig-tau daughter / 4:Internal Conversion daughter from t/EWV/EWlep(Implicit,Explicit) / 5:Internal Conversion daughter from HardScatterPhoton
  //      -1:Unmatched & not EW Conversion candidate / -2:Hadron daughter / -3:Daughter of tau from hadron or parton / -4:Internal conversion daughter(implicit,explicit) having hadronic origin / -5:External conversion candidate(Hard scattered photon) / -6:External conversion from t/EWV/EWlep
  //      (-4:Daughter of Non-hard scattered photon & has parton or hadron ancestor OR implicit Conv from quark)
  //       0:Error / >0: Non-fake: Non-hadronic origin / <0 : Fakes: Hadronic origin or external conversion


  //Only consider Status 1 lepton
  if(TruthColl.size()==0) return 0;
  if(TruthIdx<2) return 0;
  if(TruthColl.at(TruthIdx).Status()!=1) return 0;
  if( !(fabs(TruthColl.at(TruthIdx).PID())==11 || fabs(TruthColl.at(TruthIdx).PID())==13) ) return 0;

  int LeptonType=0;
  int LastSelfIdx     = LastSelfMotherIdx(TruthIdx,TruthColl);
  int MotherIdx       = FirstNonSelfMotherIdx(TruthIdx,TruthColl);
  int LastSelfMIdx    = LastSelfMotherIdx(MotherIdx,TruthColl);
  int GrMotherIdx     = FirstNonSelfMotherIdx(MotherIdx,TruthColl);
  int LastSelfGrMIdx  = LastSelfMotherIdx(GrMotherIdx,TruthColl);

  int MPID=0, GrMPID=0;
  int Status_orig=0, MStatus_orig=0, MStatus_last=0, GrMStatus_orig=0, GrMStatus_last=0;
  bool HadronicOrigin = false;
  if(    TruthIdx!=-1   ){ Status_orig    = TruthColl.at(LastSelfIdx).Status();
    HadronicOrigin = HasHadronicAncestor(TruthIdx, TruthColl);
  }                           
  if(   MotherIdx!=-1   ){ MPID         = TruthColl.at(MotherIdx).PID();
    MStatus_orig = TruthColl.at(LastSelfMIdx).Status();
    MStatus_last = TruthColl.at(MotherIdx).Status();
  }
  if(  GrMotherIdx!=-1  ){ GrMPID         = TruthColl.at(GrMotherIdx).PID();
    GrMStatus_orig = TruthColl.at(LastSelfGrMIdx).Status();
    GrMStatus_last = TruthColl.at(GrMotherIdx).Status();
  }
 
  if     ( TruthIdx==-1 )                                       LeptonType= 0;
  else if( fabs(MPID)==23 || fabs(MPID)==24 || fabs(MPID)==25 ) LeptonType= 1;
  else if( IsSignalPID(MPID) )                                  LeptonType= 2;
  else if( Status_orig>20 && Status_orig<30 )                   LeptonType= 1;//1)
  else if( fabs(MPID)>50 )                                      LeptonType=-2;
  else if( fabs(MPID)==15 && MStatus_last==2 ){
    if     ( fabs(GrMPID)==23 || fabs(GrMPID)==24 || fabs(GrMPID)==25 ) LeptonType= 3;
    else if( IsSignalPID(GrMPID) )                                      LeptonType= 3;
    else if( MStatus_orig>20  && MStatus_orig<30  )                     LeptonType= 3;//1)
    else if( HadronicOrigin )                                           LeptonType=-3;//2-a)
    else if( fabs(GrMPID)==22  && GrMStatus_orig>20 && GrMStatus_orig<30 )                     LeptonType= 5;//2-b)
    else if( fabs(GrMPID)==22 )                                                                LeptonType= 4;//2-c)
    else if( (fabs(GrMPID)==11 || fabs(GrMPID)==13 || fabs(GrMPID)==15) && GrMStatus_last!=2 ) LeptonType= 4;//2-d)
    else                                                                                       LeptonType= 0;
  }
  else if( fabs(MPID)==22 ){
    if( MStatus_orig>20 && MStatus_orig<30 )                            LeptonType= 5;//3-a)
    else if( HadronicOrigin )                                           LeptonType=-4;//3-b)
    else if( fabs(GrMPID)==24 || fabs(GrMPID)==23 || fabs(GrMPID)==6  ) LeptonType= 4;//3-c)
    else if( fabs(GrMPID)==11 || fabs(GrMPID)==13 || fabs(GrMPID)==15 ) LeptonType= 4;//3-d)
    else                                                                LeptonType= 0;
  }
  else if( (fabs(MPID)==11 || fabs(MPID)==13 || fabs(MPID)==15) && MStatus_last!=2 && !HadronicOrigin ) LeptonType= 4;//4-a)
  else if( ((fabs(MPID)>=1 && fabs(MPID)<=5) || fabs(MPID)==21) && MStatus_last!=2 )                    LeptonType=-4;//4-b)
  else if( fabs(MPID)==6 ) LeptonType=4;//4-c)
  else LeptonType=0;


  return LeptonType;

  //**footnote
  //These are based on observation in DY,ZG,TT sample(DY,ZG:amcnlo+pythia, TT:powheg+pythia) for other PS generator, convention may differ.
  //1) In amcnlo generator, output of ME level generation does not have specific guage field mother. e.g. u u~ > l+ l- -> fabs(MID)=1
  //   This perhaps due to multiple field can interplay in production, and apparently it is not possible to distinguish them in any logic.
  //   e.g. think about previous example. you cannot say whether this is from gamma or Z or H...
  //   But in PS procedure, corrections on ME proc is done sometimes. In that case it seems mother is set Z for OS ll prod. W for lnu prod.
  //   e.g. If pythia applies ISR process on input u u~, than it should affect momentum of all the consequent processes, or in case of lnu, W radiating gamma can be added.
  //   You may think lnu case is obvious, but it may not like in case of pp > lllnu(You never know which one is from W>lnu and Z>ll)
  //2-a) e.g. a)Had > ta+X, ta>l+2nu b) q>ta+X in jet fragmentation (ta is not hardscattered, since it is already considered prev. step)
  //2-b) e.g. gamma>ta(+)+ta(-)+X, ta>lnu (St=2)
  //2-c) e.g. " " " " " " " " " " " " " " " " " ", but soft gamma case. this is not observed in test sample but put here just in case.
  //          (Non hadronic origin since such case already counted before, gamma should be from non-hadronic source)
  //2-d) e.g. l>tata..+l.. , ta>l+2nu (Implicit tau conv. from non-hadronic lepton and decay) In implicit conv. GenStatus!=2
  //3-a) e.g. hard gamma>ll
  //3-b) e.g. a) Had>gamma+X, gamma>ll+X (in PS+Had stage intermediate process is omitted you see just Had>Nphoton+Mhadrons+..)
  //          b) q>gamma+q, gamms>ll+X in jet fragmentation or radiations of tops.
  //          c) gluon>Ngamma+Mhadrons in jet fragmentation (Actually observed in samples)
  //3-c) e.g. W+>W+ gamma, or t>t+gamma, gamma>ll+X, not yet observed in test sample but possible (upto radiation is observed so far)
  //3-d) e.g. ta>ta+gamma, gamma>ll+X, tau not from hadron(e.g. pp>tata)
  //4-a) e.g. EW lep l, l>lll... just implicit conversion. 
  //4-b) e.g. q or g> Nlepton +MHadrons... in parton shower history
  //4-c) e.g. t>t+ll.. implicit conversion
}


int AnalyzerCore::GetLeptonType_JH(const Lepton& Lep, std::vector<Gen>& TruthColl){
  //Type : 1:EW-Prompt / 2: BSM-Prompt / 3:EW/BSM-Prompt-Tau Daughter 
  //       4:Internal Conversion from Soft QED Radiation (PS-level) / 5:Internal Conversion from Hard Process Photon (ME-level)
  //      -1:Unmatched & not EW Conversion candidate (mis-reco. or external conversion within jets)
  //      -2:Hadron Daughter / -3:Daughter of Tau from Hadron or Parton / -4:Internal Conversion Daughter having hadronic origin
  //      -5:External Conversion Candidate (Hard scattered photon) / -6:External conversion from t/EWV/EWlep
  //      (-4:Daughter of Non-hard scattered photon & has parton or hadron ancestor OR implicit Conv from quark)
  //       0:Error / >0: Non-fake: Non-hadronic origin / <0 : Fakes: Hadronic origin or external conversion

  if(TruthColl.size()==0) return 0;

  int LeptonType=0;
  int MatchedTruthIdx = GenMatchedIdx(Lep,TruthColl);

  int NearPhotonType=0, NearPhotonIdx=-1;
  if( MatchedTruthIdx==-1 ){
    NearPhotonIdx  = GetNearPhotonIdx(Lep, TruthColl);
    NearPhotonType = GetPhotonType_JH(NearPhotonIdx, TruthColl);
    if     ( NearPhotonType<=0 ) LeptonType=-1;//1)
    else if( NearPhotonType==1 ) LeptonType=-5;//2)
    else if( NearPhotonType==2 ) LeptonType=-6;//3)
  }
  else{
    LeptonType = GetLeptonType_JH(MatchedTruthIdx, TruthColl);//4)

    if(LeptonType>=4 && LeptonType<=5){//5)
      int NearbyPrElType = GetPrElType_InSameSCRange(MatchedTruthIdx, TruthColl);
      if(NearbyPrElType>0) LeptonType = NearbyPrElType;
    }
  }


  return LeptonType;

  //**footnote
  //1) matched to no gen-lepton nor gen-photon -> mis-reco.
  //   matched to no gen-lepton, but to photon with hadronic origin -> mis-reco. (e.g. pions->e) or external conversion from photon in jets
  //2) matched to no gen-lepton, but to photon with non-hadronic origin (hard process) -> external conversion from photon with non-hadronic source (ME-level)
  //3) matched to no gen-lepton, but to photon with non-hadronic origin (soft QED radiation) -> external conversion from photon with non-hadronic source (PS-level)
  //4) matched to gen-lepton, categorize based on the truth categorization algo.: AnalyzerCore::GetLeptonType_JH(int TruthIdx, std::vector<Gen>& TruthColl)
  //5) collimated e/gm objects are merged in SC, hence if there is prompt electron within SC-merging range, reco-electron's properties actually represent pre-QED-FSR prompt-electron,
  //   rather than the closest internal conversion electron. Therefore shift the type to the prompt lepton's type.
}



int AnalyzerCore::GetPhotonType_JH(int PhotonIdx, std::vector<Gen>& TruthColl){
  //Type : 
  // 0: Invalid input or Error or Hard process photon is input when it is not final line of its history
  // 1: prompt photon (hard process (ME-level)) / 2: prompt Else prompt daughter(l,V,t)
  //-1: Reserved for unmatched(Not used now) / -2: Hadronic origin

  if( PhotonIdx<2 ) return 0;
  if( !(TruthColl.at(PhotonIdx).PID()==22 && (TruthColl.at(PhotonIdx).Status()==1 || TruthColl.at(PhotonIdx).Status()==23)) ) return 0;

  if(TruthColl.at(PhotonIdx).Status()==23){
    if(IsFinalPhotonSt23(TruthColl)) return 1;
    else                             return 0;
  }//From this pt, only St1 Photon is treated.

  int PhotonType=0;
  int LastSelfIdx    = LastSelfMotherIdx(PhotonIdx,TruthColl);
  int MotherIdx      = FirstNonSelfMotherIdx(PhotonIdx,TruthColl);
  int fMPID=0, Status_orig=0;
  bool HadronicOrigin = false;
  if( PhotonIdx!=-1 ){ Status_orig    = TruthColl.at(LastSelfIdx).Status();
    HadronicOrigin = HasHadronicAncestor(PhotonIdx, TruthColl);
  }                           
  if( MotherIdx!=-1 ){ fMPID          = fabs(TruthColl.at(MotherIdx).PID()); }


  if     (    Status_orig>20 && Status_orig<30          ) PhotonType= 1;//1)
  else if(         fMPID==23 || fMPID==25               ) PhotonType= 1;//2)
  else if( fMPID==24 || fMPID==6  || IsSignalPID(fMPID) ) PhotonType= 2;//3)
  else if(            HadronicOrigin                    ) PhotonType=-2;//4)
  else if( fMPID==11 || fMPID==13 || fMPID==15          ) PhotonType= 2;//5)
  else                                                    PhotonType= 0;
  
  return PhotonType;
  //**footnote
  //1) In case of ME-level photon, they have history; GenSt=23>...>1.
  //   this is better than checking mother because intermediate-mother is not written in some cases, where generation is not explicitly on-shell.
  //   e.g. qq>llG instead of qq>Z>llG
  //2) Sometimes, photon's final state is 1 before any history. (Presumably skimmed between pythia & MiniAOD).
  //   e.g. G;St=1, Mother=Z ; algorithm 1) cannot catch this.
  //   but PS step also treats photon radiation (same kind of history), therefore distinction between type 1 and 2 is not physical.
  //3) top and charged bosons radiate photons, and some case the photon is very energetic.
  //4) This category does not include tops. Photons from hadrons and quarks. But predominantly, in most of the cases they are daughter of pi0.
  //   But rarely other mesons as eta, B, or even some quarks can also radiate energetic photons.
  //5) Photons radiated from lepton FSR. Sometimes they are quite energetic.
}

//------------------------------------------------------------------------------------------//



//--Gen-Matching Tools----------------------------------------------------------------------//
int AnalyzerCore::GenMatchedIdx(const Lepton& Lep, std::vector<Gen>& truthColl){
  //Find Matched Index within dR01; if ambiguous closest dR one chosen (Resolution way better than dPtRel)
  //Seed from RecoLepton

  int MatchedIdx=-1, PIDLep=0;
  double dR=999., dRmax=0.1;

  if(Lep.LeptonFlavour()==Lepton::ELECTRON) PIDLep = 11;
  else if(Lep.LeptonFlavour()==Lepton::MUON) PIDLep = 13;
  else{
    cout << "[AnalyzerCore::GetGenMatchedLepton] input lepton flavour not set" << endl;
    exit(EXIT_FAILURE);
  }
  
  for(unsigned int i=2; i<truthColl.size(); i++){
    if( truthColl.at(i).MotherIndex()<0     ) continue;
    if( truthColl.at(i).Status()!=1         ) continue;
    if( fabs(truthColl.at(i).PID())!=PIDLep ) continue;
    if( truthColl.at(i).DeltaR(Lep)>dRmax   ) continue;

    if( truthColl.at(i).DeltaR(Lep)<dR ){ dR=truthColl.at(i).DeltaR(Lep); MatchedIdx=i; }
  }


  //  cout << "truthColl.size() = " << truthColl.size() << " MatchedIdx = " << MatchedIdx << endl;
  return MatchedIdx;
}


int AnalyzerCore::GetNearPhotonIdx(const Lepton& Lep, std::vector<Gen>& TruthColl){
  // Return gen-photon index, which are candidate for source of external conversion. 1)

  int NearPhotonIdx=-1;
  double PTthreshold=10., dPtRelmax=0.5;//2)
  double dRmax=0.2;//3)
  double dRmin=999.;
  for(unsigned int i=2; i<TruthColl.size(); i++){
    if( TruthColl.at(i).MotherIndex()<0   ) continue;
    if( !(TruthColl.at(i).PID()==22 && (TruthColl.at(i).Status()==1 || TruthColl.at(i).Status()==23)) ) continue;
    if( TruthColl.at(i).Pt()<PTthreshold  ) continue;
    if( !(Lep.Pt()/TruthColl.at(i).Pt()>(1.-dPtRelmax) && Lep.Pt()/TruthColl.at(i).Pt()<(1.+dPtRelmax)) ) continue;
    if( Lep.DeltaR(TruthColl.at(i))>dRmax ) continue;

    if( TruthColl.at(i).Status()==23 && !IsFinalPhotonSt23(TruthColl) ) continue;//4)
    if( Lep.DeltaR(TruthColl.at(i))<dRmin ){ dRmin=Lep.DeltaR(TruthColl.at(i)); NearPhotonIdx=i; }
  }


  return NearPhotonIdx;
  //footnote
  //1) External conversion is only meaningful for electron, as external conversion rate is theoretically ~M^{-2} in asymmetric limit (ref: arXiv:1110.1368)
  //   In the conversion study, I also observed that muon conversion rate is less than ~1/3000 of electron conversion rate (unobserved).
  //2) In 16' analyses using Cat-Ntuples, I used dPtRel<0.2 cut, which was optimized for PT>25 electrons. It is updated to 0.5 as I observe that PT(e)/PT(g) varies between 0.5 and 1.3 for 10<PT(e)<25. Thus this cut is valid for PT(e)>10 GeV.
  //3) dReg is larger for low Pt electrons, but 0.2 cut is still enough for matching for all PT(e)>10.
  //4) In some MC events, hard scattered photon(GenSt23) is the last history of the photon and do not have daughter in the history. Presumably because generator history is partly skimmed in MiniAOD.
}


int AnalyzerCore::FirstNonSelfMotherIdx(int TruthIdx, std::vector<Gen>& TruthColl){

  if(TruthIdx<2) return -1;

  int pid=TruthColl.at(TruthIdx).PID(), midx=TruthIdx;
  while(TruthColl.at(midx).PID()==pid){
    midx=TruthColl.at(midx).MotherIndex();  
    if(midx<0) break;
  }

  return midx;
}


int AnalyzerCore::LastAbsSelfMotherIdx(int TruthIdx,std::vector<Gen>& TruthColl){

  if(TruthIdx<2) return TruthIdx;

  int pid=fabs(TruthColl.at(TruthIdx).PID()), midx=TruthIdx, currentidx=TruthIdx;
  while(fabs(TruthColl.at(midx).PID())==pid){
    currentidx=midx;
    midx=TruthColl.at(midx).MotherIndex();
    if(midx<0) break;
  }

  return currentidx;
}


int AnalyzerCore::LastSelfMotherIdx(int TruthIdx,std::vector<Gen>& TruthColl){

  if(TruthIdx<2) return TruthIdx;

  int pid=TruthColl.at(TruthIdx).PID(), midx=TruthIdx, currentidx=TruthIdx;
  while(TruthColl.at(midx).PID()==pid){
    currentidx=midx;
    midx=TruthColl.at(midx).MotherIndex();  
    if(midx<0) break;
  }

  return currentidx;
}


bool AnalyzerCore::HasHadronicAncestor(int TruthIdx, std::vector<Gen>& TruthColl){
  //Returns true  if 1)has hadron mother, 2)has quark mother(!top) 3)Incident protons
  //        false if 1)is hardscattered truth, 2)EW/H/BSM/t daughter, 3)not above, 4)invalid input(e.g. unmatched case)
  
  if(TruthIdx<0) return false;
  if(TruthIdx<2) return true;

  bool HasPartonHadronAncestor=false;
  int  midx=TruthIdx, fmid=fabs(TruthColl.at(midx).PID()), MSt_orig=-1;
  int  St_orig=TruthColl.at(LastSelfMotherIdx(TruthIdx, TruthColl)).Status();
  if( St_orig>20 && St_orig<30) return false;

  while( midx>=2 ){
    midx=FirstNonSelfMotherIdx(midx,TruthColl);
    MSt_orig=TruthColl.at(LastSelfMotherIdx(midx,TruthColl)).Status();
    fmid=fabs(TruthColl.at(midx).PID());
    if(  fmid==23 || fmid==24 || fmid==25 || fmid==6 || IsSignalPID(fmid) ){ HasPartonHadronAncestor=false; break; }
    if( (fmid==11 || fmid==13 || fmid==15 || fmid==22) && (MSt_orig>20 && MSt_orig<30)){ HasPartonHadronAncestor=false; break; }
    if( fmid>50 ) { HasPartonHadronAncestor=true; break; }
    if( (fmid>=1 && fmid<=5) || fmid==21 ){ HasPartonHadronAncestor=true; break; }
  }

  return HasPartonHadronAncestor;
}




bool AnalyzerCore::IsFinalPhotonSt23(std::vector<Gen>& TruthColl){
  //In Some XG proc events, it is observed that some of photons' last status is 23. Presumably due to skimming of generator history between pythia and MiniAOD.
  //The function returns if this is the case.
  //And this is designed only for 1 hard photon case as W+G or Z+G or TT+G

  bool IsFinalGammaStatus23 = false;
  bool HasStatus23Photon    = false;
  for(unsigned int i=2; i<TruthColl.size(); i++){
    int fpid  = fabs(TruthColl.at(i).PID());
    int GenSt = TruthColl.at(i).Status();
    int MPID_direct= TruthColl.at(TruthColl.at(i).MotherIndex()).PID();
    if( !((fpid!=22 && MPID_direct==22) || (fpid==22 && (GenSt==23||GenSt==1))) ) continue;

    int LastSelfIdx  = LastSelfMotherIdx(i,TruthColl);
    int LastSelfSt   = TruthColl.at(LastSelfIdx).Status();
    int MotherIdx    = FirstNonSelfMotherIdx(i,TruthColl);
    int LastSelfMIdx=-1, MStatus_orig=-1;
    if(MotherIdx!=-1){
      LastSelfMIdx = LastSelfMotherIdx(MotherIdx,TruthColl);
      MStatus_orig = TruthColl.at(LastSelfMIdx).Status();
    }

    if(fpid==22){
      if(GenSt==23) {HasStatus23Photon=true; IsFinalGammaStatus23=true;}
      else if(GenSt==1 && LastSelfSt==23) {IsFinalGammaStatus23=false; break;}//a)
    }
    else if( MPID_direct==22 && MStatus_orig==23 ){ IsFinalGammaStatus23=false; break;}//b)
  }

  if(!HasStatus23Photon) return false;
  
  return IsFinalGammaStatus23;

  //**footnotes
  //a) Status-23 photon's last is 1. Thus status-23 photon is not the last history.
  //b) Daughter particle of status-23 photon is found. Thus status-23 photon is not the last history.
}






