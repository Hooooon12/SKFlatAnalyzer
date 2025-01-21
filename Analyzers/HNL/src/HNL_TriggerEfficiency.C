#include "HNL_TriggerEfficiency.h"

void HNL_TriggerEfficiency::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();


  EE=false, MuMu=false, EMu=false, SystRun=false; 
  DiMuTrig_DZ=false, DiElTrig_DZ=false;
  EMuTrig_ElLeg=false, EMuTrig_MuLeg=false, EMuTrig_DZ=false;
  for(unsigned int i=0; i<Userflags.size(); i++){
    if(Userflags.at(i).Contains("EE")) EE = true; 
    if(Userflags.at(i).Contains("MuMu")) MuMu = true; 
    if(Userflags.at(i).Contains("EMu")) EMu = true; 
    //    if(Userflags.at(i).Contains("DiMuTrig_MuLeg")) DiMuTrig_MuLeg=true; 
    if(Userflags.at(i).Contains("DiMuTrig_DZ"))    DiMuTrig_DZ    = true; 
    if(Userflags.at(i).Contains("DiElTrig_DZ"))    DiElTrig_DZ    = true; 
    if(Userflags.at(i).Contains("EMuTrig_ElLeg"))  EMuTrig_ElLeg  = true; 
    if(Userflags.at(i).Contains("EMuTrig_MuLeg"))  EMuTrig_MuLeg  = true; 
    if(Userflags.at(i).Contains("EMuTrig_DZ"))     EMuTrig_DZ     = true; 
    if(Userflags.at(i).Contains("SystRun"))        SystRun        = true; 
  }

  if(GetEraShort()=="2016a"){
    TrigList_SglMu = {"HLT_IsoMu24_v", "HLT_IsoTkMu24_v"}; TrigList_SglEl = {"HLT_Ele27_WPTight_Gsf_v"};
    TrigList_DblMu   = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v", "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v"};
    TrigList_DblMuDZ = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v", "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v"};
    TrigList_DblEG   = {"HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v"};
    TrigList_DblEGDZ = {"HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"};
    TrigList_EMu    = {"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v", "HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v"};
  }
  else if(GetEraShort()=="2016b"){
    TrigList_SglMu = {"HLT_IsoMu24_v", "HLT_IsoTkMu24_v"}; TrigList_SglEl = {"HLT_Ele27_WPTight_Gsf_v"};
    TrigList_DblMu   = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v", "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v", "HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v"};
    TrigList_DblMuDZ = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v", "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v", "HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v"};
    TrigList_DblEG   = {"HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v"};
    TrigList_DblEGDZ = {"HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"};
    TrigList_EMu    = {"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v", "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v"};
    TrigList_EMuDZ  = {"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v", "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"};
  }
  else if(DataYear==2017){
    TrigList_SglMu = {"HLT_IsoMu27_v"}; TrigList_SglEl = {"HLT_Ele32_WPTight_Gsf_L1DoubleEG_v"};
    TrigList_DblMu    = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v"};
    TrigList_DblMuDZ  = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v"};
    TrigList_DblMuDZM = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v"};
    TrigList_DblEG    = {"HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v"};
    TrigList_EMu     = {"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v", "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v"};
    TrigList_EMuDZ   = {"HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v", "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v"};
  }
  else if(DataYear==2018){
    TrigList_SglMu = {"HLT_IsoMu24_v"}; TrigList_SglEl = {"HLT_Ele32_WPTight_Gsf_v"};
    TrigList_DblMu    = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v"};
    TrigList_DblMuDZ  = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v"};
    TrigList_DblMuDZM = {"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v"};
    TrigList_DblEG    = {"HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v"};
    TrigList_EMu     = {"HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v", "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v"};
    TrigList_EMuDZ   = {"HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v", "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v"};
  }

  if(EE){
    if(DiElTrig_DZ   ){ TrigList = TrigList_DblEG; }
  }
  if(EMu){
    if(EMuTrig_ElLeg ){ TrigList = TrigList_SglMu; }
    if(EMuTrig_MuLeg ){ TrigList = TrigList_SglEl; } 
    if(EMuTrig_DZ    ){ TrigList = TrigList_EMu; }
  }

  //Set up the tagger map only for the param settings to be used.
  vector<JetTagging::Parameters> jtps;
  jtps.push_back( JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets) );
  mcCorr->SetJetTaggingParameters(jtps);

}


void HNL_TriggerEfficiency::executeEvent(){


  Event ev = GetEvent();
  float weight=1., w_GenNorm=1., w_BR=1., w_PU=1.;
  if(!IsDATA){
    w_GenNorm = MCweight()*GetKFactor()*ev.GetTriggerLumi("Full");
    w_PU      = GetPileUpWeight(nPileUp, 0);
    weight *= w_GenNorm * w_BR * w_PU;
  }

  bool PassTrig=false;
  if     (DiMuTrig_DZ) PassTrig = ev.PassTrigger(TrigList_DblMuDZM) or ev.PassTrigger(TrigList_DblMuDZ) or ev.PassTrigger(TrigList_DblMu);
  else if(DiElTrig_DZ) PassTrig = ev.PassTrigger(TrigList_DblEG) or ev.PassTrigger(TrigList_DblEGDZ);
  else                 PassTrig = ev.PassTrigger(TrigList);
  if(!PassTrig) return;
  if(!PassMETFilter()) return;

  bool PreCutPass=false;
  vector<Muon>     muonPreColl     = GetMuons("NOCUT", 5., 2.4);
  vector<Electron> electronPreColl = GetElectrons("NOCUT", 5., 2.5);
  sort(muonPreColl.begin(), muonPreColl.end(), PtComparing);
  sort(electronPreColl.begin(), electronPreColl.end(), PtComparing);
  if(EE and electronPreColl.size()>1) PreCutPass=true;
  if(MuMu and muonPreColl.size()>1    ) PreCutPass=true;
  if(EMu and electronPreColl.size()>0 && muonPreColl.size()>0) PreCutPass=true;
  if(!PreCutPass) return;


  TString MuTID = "HNL_ULID_"+GetYearString(), MuLID = "HNVetoMVA", MuVID = "HNVetoMVA";
  TString ElTID = "HNL_HighPt_ULID_"+GetYearString(), ElLID = "HNVetoMVA", ElVID = "HNVetoMVA";
  float PTminEl = ElTID.Contains("SS")? 15.:10.;
  vector<Muon>     muonTightColl     = SelectMuons(muonPreColl, MuTID, 10., 2.4);
  vector<Electron> electronTightColl = SelectElectrons(electronPreColl, ElTID, PTminEl, 2.5);
  vector<Muon>     muonLooseColl     = SelectMuons(muonPreColl, MuLID, 10., 2.4);
  vector<Muon>     muonVetoColl      = SelectMuons(muonPreColl, MuVID, 5., 2.4);
  vector<Electron> electronLooseColl = SelectElectrons(electronPreColl, ElLID, PTminEl, 2.5);
  vector<Electron> electronVetoColl  = SelectElectrons(electronPreColl, ElVID, 10., 2.5);


  JetTagging::Parameters param_jets = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets);
  vector<Jet> jetNoVetoColl  = GetJets("tight", 25., 2.4);
  sort(jetNoVetoColl.begin(), jetNoVetoColl.end(), PtComparing);
  vector<Jet> bjetNoVetoColl = SelBJets(jetNoVetoColl, param_jets);
  vector<Jet> jetColl  = JetsVetoLeptonInside(jetNoVetoColl, electronVetoColl, muonVetoColl, 0.4);
  vector<Jet> bjetColl = SelBJets(jetColl, param_jets);


  Particle vMET = ev.GetMETVector();
  Particle vMET_xyCorr(pfMET_Type1_PhiCor_pt*TMath::Cos(pfMET_Type1_PhiCor_phi), pfMET_Type1_PhiCor_pt*TMath::Sin(pfMET_Type1_PhiCor_phi), 0., pfMET_Type1_PhiCor_pt);


  vector<Gen> truthColl;


  bool EventCand = false;
  if(MuMu){ EventCand = muonLooseColl.size()>1; }
  if(EE){ EventCand = electronLooseColl.size()>1; }
  if(EMu){ EventCand = electronLooseColl.size()>0 && muonLooseColl.size()>0; }

  float w_topptrw = 1., w_prefire = 1., sf_trig = 1.;
  float sf_mutk = 1., sf_muid = 1., sf_muiso = 1., sf_elreco = 1., sf_elid = 1., sf_btag = 1.;
  if((!IsDATA) and EventCand){
    if(MCSample.Contains("TT") and MCSample.Contains("powheg")) truthColl = GetGens();
    //w_topptrw = mcCorr->GetTopPtReweight(truthColl);
    w_prefire = GetPrefireWeight(0);
    //sf_muid   = GetMuonSF(muonTightColl, "TopHNID_TkMu", "ID");
    //sf_muiso  = GetMuonSF(muonTightColl, "POGTIso_POGTID", "Iso");
    //sf_elreco = GetElectronSF(electronTightColl, "", "Reco");
    //sf_elid   = GetElectronSF(electronTightColl, "POGMVAIsoWP90", "ID");
    if(EMu) sf_btag = mcCorr->GetBTaggingReweight_1a(jetColl, param_jets);
    //sf_trig   = mcCorr->GetTriggerSF(electronTightColl, muonTightColl, SFKey_Trig, "");
    //cout<<" w_prefire:"<<w_prefire<<" sf_trig:"<<sf_trig<<endl;
    //cout<<"sf_mutk"<<sf_mutk<<" sf_muid:"<<sf_muid<<" sf_muiso:"<<sf_muiso<<" sf_elreco:"<<sf_elreco<<" sf_elid:"<<sf_elid<<" sf_btag:"<<sf_btag<<endl;
  }
  weight *= w_topptrw * w_prefire * sf_trig;
  weight *= sf_mutk * sf_muid * sf_muiso * sf_elreco * sf_elid * sf_btag;

 
  if(MuMu){
    if(DiMuTrig_DZ   ) MeasEffDiMuTrig_DZ(muonTightColl, muonVetoColl, electronTightColl, electronVetoColl, jetColl, bjetColl, vMET, ev, weight, "");
    //    if(DiMuTrig_MuLeg) MeasEffDiMuTrig_MuLeg(muonTightColl, muonLooseColl, electronTightColl, electronLooseColl, jetColl, bjetColl, vMET, ev, weight, "");

  }
  if(EE){
    if(DiElTrig_DZ) MeasEffDiElTrig_DZ(muonTightColl, muonVetoColl, electronTightColl, electronVetoColl, jetColl, bjetColl, vMET, ev, weight, "");
  }
  if(EMu){
    if(EMuTrig_ElLeg){
      MeasEffEMuTrig_ElLeg(muonTightColl, muonVetoColl, electronTightColl, electronVetoColl, jetColl, bjetColl, vMET, ev, weight, "");
    }
    if(EMuTrig_MuLeg){
      MeasEffEMuTrig_MuLeg(muonTightColl, muonVetoColl, electronTightColl, electronVetoColl, jetColl, bjetColl, vMET, ev, weight, "");
    }
    if(EMuTrig_DZ){
      MeasEffEMuTrig_DZ(muonTightColl, muonVetoColl, electronTightColl, electronVetoColl, jetColl, bjetColl, vMET, ev, weight, "");
    }
  }


}



/*
void HNL_TriggerEfficiency::MeasEffDiMuTrig_MuLeg(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                                        vector<Jet>& JetColl,  vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label){

  if( !(MuTColl.size()==2 && ElTColl.size()==0) ) return;
  if( !(MuLColl.size()==2 && ElLColl.size()==0) ) return;
  if( MuTColl.at(0).Pt()<(DataYear!=2017? 26:29) ) return;
  if( MuTColl.at(0).Charge()==MuTColl.at(1).Charge() ) return;
  if( MuTColl.at(0).DeltaR(MuTColl.at(1))<0.4 ) return;

  const int NPtBinEdges1=10, NPtBinEdges2=10, NfEtaBinEdges=9;
  double PtBinEdges1[NPtBinEdges1], PtBinEdges2[NPtBinEdges2];
  double PtBinEdges1_17[NPtBinEdges1] = {0.,15.,17.,20.,25.,30.,40.,50.,100.,200.};
  double PtBinEdges2_17[NPtBinEdges2] = {0.,5.,8.,10.,15.,20.,30.,50.,100.,200.};
  std::copy(PtBinEdges1_17, PtBinEdges1_17+NPtBinEdges1, PtBinEdges1); 
  std::copy(PtBinEdges2_17, PtBinEdges2_17+NPtBinEdges2, PtBinEdges2); 

  double fEtaBinEdges[NfEtaBinEdges] = {-2.4,-2.1,-1.2,-0.9,0.,0.9,1.2,2.1,2.4};
  double PTMu   = MuTColl.at(0).Pt();
  //double fEtaMu = fabs(MuTColl.at(0).Eta());
  double fEtaMu = MuTColl.at(0).Eta();

  vector<TString> TrigListToMeas1={"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v"};
  vector<TString> TrigListToMeas2={"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v"};
  bool PassLeg1=false, PassLeg2=false;
  if(MCSample.Contains("Trig") or DataStream.Contains("Trig")){
    for(unsigned int it_obj=0; it_obj<HLTObject_eta->size(); it_obj++){
      float dR = sqrt(pow(MuTColl.at(0).Eta()-HLTObject_eta->at(it_obj),2)+pow(MuTColl.at(0).Phi()-HLTObject_phi->at(it_obj),2));
      if(dR>0.2) continue;
      if(HLTObject_FiredPaths->at(it_obj).find(TrigListToMeas1.at(0))!=std::string::npos ){ PassLeg1=true; }
      if(HLTObject_FiredPaths->at(it_obj).find(TrigListToMeas2.at(0))!=std::string::npos ){ PassLeg2=true; }
      if(PassLeg1 && PassLeg2) break;
    }
  }
  else{
    PassLeg1=ev.PassTrigger(TrigListToMeas1); PassLeg2=ev.PassTrigger(TrigListToMeas2);
  }


  FillHist("NMu1_ALL_Pt_1D", PTMu, weight, NPtBinEdges1-1, PtBinEdges1);
  FillHist("NMu1_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
  FillHist("NMu2_ALL_Pt_1D", PTMu, weight, NPtBinEdges2-1, PtBinEdges2);
  FillHist("NMu2_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
  if(PassLeg1){
    FillHist("NMu1Trig_ALL_Pt_1D", PTMu, weight, NPtBinEdges1-1, PtBinEdges1);
    FillHist("NMu1Trig_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
  }
  if(PassLeg2){
    FillHist("NMu2Trig_ALL_Pt_1D", PTMu, weight, NPtBinEdges2-1, PtBinEdges2);
    FillHist("NMu2Trig_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
  }

  //Syst:QCD contamination 
  if(ElTColl.at(0).Pt()>40){
    FillHist("NMu1_AltTag_ALL_Pt_1D", PTMu, weight, NPtBinEdges1-1, PtBinEdges1);
    FillHist("NMu1_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
    FillHist("NMu2_AltTag_ALL_Pt_1D", PTMu, weight, NPtBinEdges2-1, PtBinEdges2);
    FillHist("NMu2_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
    if(PassLeg1){
      FillHist("NMu1Trig_AltTag_ALL_Pt_1D", PTMu, weight, NPtBinEdges1-1, PtBinEdges1);
      FillHist("NMu1Trig_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
    }
    if(PassLeg2){
      FillHist("NMu2Trig_AltTag_ALL_Pt_1D", PTMu, weight, NPtBinEdges2-1, PtBinEdges2);
      FillHist("NMu2Trig_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
    }
  }

}
*/



void HNL_TriggerEfficiency::MeasEffEMuTrig_MuLeg(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                                       vector<Jet>& JetColl,  vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label){

  if( !(ElTColl.size()==1 && MuTColl.size()==1) ) return;
  if( !(ElLColl.size()==1 && MuLColl.size()==1) ) return;
  if(!IsDATA){ if( !(MuTColl.at(0).IsPrompt() && ElTColl.at(0).IsPrompt()) ) return; }
  if( ElTColl.at(0).Pt()<(DataYear<2017? 30:35) ) return;
  if( MuTColl.at(0).Charge()==ElTColl.at(0).Charge() ) return;
  if( MuTColl.at(0).DeltaR(ElTColl.at(0))<0.4 ) return;
  if( !(JetColl.size()>1 && BJetColl.size()>0) ) return;

  const int NPtBinEdges1=11, NPtBinEdges2=11, NfEtaBinEdges=5;
  double PtBinEdges1[NPtBinEdges1], PtBinEdges2[NPtBinEdges2];
  double PtBinEdges1_17[NPtBinEdges1] = {0.,10.,20.,23.,25.,30.,40.,50.,100.,200.,1000.};
  double PtBinEdges2_17[NPtBinEdges2] = {0.,5.,8.,10.,15.,20.,30.,50.,100.,200.,1000.};
  copy(PtBinEdges1_17, PtBinEdges1_17+NPtBinEdges1, PtBinEdges1);
  copy(PtBinEdges2_17, PtBinEdges2_17+NPtBinEdges2, PtBinEdges2);
  double PtBinEdges3[NPtBinEdges1-1] = {0.,10.,20.,23.,25.,30.,40.,50.,100.,200.};
  double PtBinEdges4[NPtBinEdges2-1] = {0.,5.,8.,10.,15.,20.,30.,50.,100.,200.};

  double fEtaBinEdges[NfEtaBinEdges] = {0.,0.9,1.2,2.1,2.4};
  double PTMu   = MuTColl.at(0).MiniAODPt();
  double fEtaMu = fabs(MuTColl.at(0).Eta());

  bool PassLeg1=false, PassLeg2=false, PassTagHLT = false;
  for(unsigned int it=0; it<TrigList_SglEl.size(); it++){ if(ElTColl.at(0).PassPath(TrigList_SglEl.at(it))){ PassTagHLT=true; break; } }
  TString TestFilter1 = GetEraShort()=="2016a"? "hltMu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVLMuonlegL3IsoFiltered23":
    "hltMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLMuonlegL3IsoFiltered23";
  TString TestFilter2 = "hltMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLMuonlegL3IsoFiltered8";
  PassLeg1 = MuTColl.at(0).PassFilter(TestFilter1), PassLeg2 = MuTColl.at(0).PassFilter(TestFilter2);
  if(DataYear==2017) PassLeg2 = MuTColl.at(0).PassPath("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");
  //filter name different only in 17B in full Run-2, wasn't catched before processing, but checked eff(DZ)~1 in 17
  if(!PassTagHLT) return;

  FillHist("NMu1_ALL_Pt_1D", PTMu, weight, NPtBinEdges1-1, PtBinEdges1);
  FillHist("NMu1_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
  FillHist("LowPt/NMu1_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-2, PtBinEdges3, NfEtaBinEdges-1, fEtaBinEdges);
  FillHist("NMu2_ALL_Pt_1D", PTMu, weight, NPtBinEdges2-1, PtBinEdges2);
  FillHist("NMu2_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
  FillHist("LowPt/NMu2_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-2, PtBinEdges4, NfEtaBinEdges-1, fEtaBinEdges);
  if(PassLeg1){
    FillHist("NMu1Trig_ALL_Pt_1D", PTMu, weight, NPtBinEdges1-1, PtBinEdges1);
    FillHist("NMu1Trig_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
    FillHist("LowPt/NMu1Trig_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-2, PtBinEdges3, NfEtaBinEdges-1, fEtaBinEdges);
  }
  if(PassLeg2){
    FillHist("NMu2Trig_ALL_Pt_1D", PTMu, weight, NPtBinEdges2-1, PtBinEdges2);
    FillHist("NMu2Trig_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
    FillHist("LowPt/NMu2Trig_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-2, PtBinEdges4, NfEtaBinEdges-1, fEtaBinEdges);
  }

  //Syst:QCD contamination 
  if(ElTColl.at(0).Pt()>40){
    FillHist("NMu1_AltTag_ALL_Pt_1D", PTMu, weight, NPtBinEdges1-1, PtBinEdges1);
    FillHist("NMu1_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
    FillHist("LowPt/NMu1_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-2, PtBinEdges3, NfEtaBinEdges-1, fEtaBinEdges);
    FillHist("NMu2_AltTag_ALL_Pt_1D", PTMu, weight, NPtBinEdges2-1, PtBinEdges2);
    FillHist("NMu2_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
    FillHist("LowPt/NMu2_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-2, PtBinEdges4, NfEtaBinEdges-1, fEtaBinEdges);
    if(PassLeg1){
      FillHist("NMu1Trig_AltTag_ALL_Pt_1D", PTMu, weight, NPtBinEdges1-1, PtBinEdges1);
      FillHist("NMu1Trig_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
      FillHist("LowPt/NMu1Trig_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges1-2, PtBinEdges3, NfEtaBinEdges-1, fEtaBinEdges);
    }
    if(PassLeg2){
      FillHist("NMu2Trig_AltTag_ALL_Pt_1D", PTMu, weight, NPtBinEdges2-1, PtBinEdges2);
      FillHist("NMu2Trig_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
      FillHist("LowPt/NMu2Trig_AltTag_ALL_PtEta_2D", PTMu, fEtaMu, weight, NPtBinEdges2-2, PtBinEdges4, NfEtaBinEdges-1, fEtaBinEdges);
    }
  }

}



void HNL_TriggerEfficiency::MeasEffEMuTrig_ElLeg(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
				       vector<Jet>& JetColl,  vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label){
  if( !(MuTColl.size()==1 && ElTColl.size()==1) ) return;
  if( !(MuLColl.size()==1 && ElLColl.size()==1) ) return;
  if(!IsDATA){ if( !(MuTColl.at(0).IsPrompt() && ElTColl.at(0).IsPrompt()) ) return; }
  if( MuTColl.at(0).Pt()<(DataYear!=2017? 26:29) ) return;
  if( MuTColl.at(0).Charge()==ElTColl.at(0).Charge() ) return;
  if( MuTColl.at(0).DeltaR(ElTColl.at(0))<0.4 ) return;
  if( !(JetColl.size()>1 && BJetColl.size()>0) ) return;

  const int NPtBinEdges1=11, NPtBinEdges2=10, NfEtaBinEdges=4;
  double PtBinEdges1[NPtBinEdges1], PtBinEdges2[NPtBinEdges2];
  double PtBinEdges1_17[NPtBinEdges1] = {0.,10.,20.,23.,25.,30.,40.,50.,100.,200.,1000.};
  double PtBinEdges2_17[NPtBinEdges2] = {0.,10.,12.,15.,20.,30.,50.,100.,200.,1000.};
  copy(PtBinEdges1_17, PtBinEdges1_17+NPtBinEdges1, PtBinEdges1); 
  copy(PtBinEdges2_17, PtBinEdges2_17+NPtBinEdges2, PtBinEdges2); 
  double PtBinEdges3[NPtBinEdges1-1] = {0.,10.,20.,23.,25.,30.,40.,50.,100.,200.};
  double PtBinEdges4[NPtBinEdges2-1] = {0.,10.,12.,15.,20.,30.,50.,100.,200.};

  double fEtaBinEdges[NfEtaBinEdges]={0., 0.8, 1.479, 2.5};
  double PTEle   = ElTColl.at(0).Pt();
  double fEtaEle = fabs(ElTColl.at(0).Eta());

  bool PassLeg1=false, PassLeg2=false, PassTagHLT = false;
  for(unsigned int it=0; it<TrigList_SglMu.size(); it++){ if(MuTColl.at(0).PassPath(TrigList_SglMu.at(it))){ PassTagHLT=true; break; } }
  TString TestFilter1 = "hltMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLElectronlegTrackIsoFilter";
  TString TestFilter2 = GetEraShort()=="2016a"? "hltMu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVLElectronlegTrackIsoFilter":
    "hltMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLElectronlegTrackIsoFilter";
  PassLeg1 = ElTColl.at(0).PassFilter(TestFilter1), PassLeg2 = ElTColl.at(0).PassFilter(TestFilter2);
  if(!PassTagHLT) return;

  FillHist("NEle1_ALL_Pt_1D", PTEle, weight, NPtBinEdges1-1, PtBinEdges1);
  FillHist("NEle1_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
  FillHist("LowPt/NEle1_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges1-2, PtBinEdges3, NfEtaBinEdges-1, fEtaBinEdges);
  FillHist("NEle2_ALL_Pt_1D", PTEle, weight, NPtBinEdges2-1, PtBinEdges2);
  FillHist("NEle2_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
  FillHist("LowPt/NEle2_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges2-2, PtBinEdges4, NfEtaBinEdges-1, fEtaBinEdges);
  if(PassLeg1){
    FillHist("NEle1Trig_ALL_Pt_1D", PTEle, weight, NPtBinEdges1-1, PtBinEdges1);
    FillHist("NEle1Trig_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
    FillHist("LowPt/NEle1Trig_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges1-2, PtBinEdges3, NfEtaBinEdges-1, fEtaBinEdges);
  }
  if(PassLeg2){
    FillHist("NEle2Trig_ALL_Pt_1D", PTEle, weight, NPtBinEdges2-1, PtBinEdges2);
    FillHist("NEle2Trig_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
    FillHist("LowPt/NEle2Trig_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges2-2, PtBinEdges4, NfEtaBinEdges-1, fEtaBinEdges);
  }

  //Syst:QCD contamination 
  //if(MuTColl.at(0).Pt()>35 && MuTColl.at(0).RelIso()<0.15){
  if(MuTColl.at(0).Pt()>35){
    FillHist("NEle1_AltTag_ALL_Pt_1D", PTEle, weight, NPtBinEdges1-1, PtBinEdges1);
    FillHist("NEle1_AltTag_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
    FillHist("LowPt/NEle1_AltTag_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges1-2, PtBinEdges3, NfEtaBinEdges-1, fEtaBinEdges);
    FillHist("NEle2_AltTag_ALL_Pt_1D", PTEle, weight, NPtBinEdges2-1, PtBinEdges2);
    FillHist("NEle2_AltTag_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
    FillHist("LowPt/NEle2_AltTag_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges2-2, PtBinEdges4, NfEtaBinEdges-1, fEtaBinEdges);
    if(PassLeg1){
      FillHist("NEle1Trig_AltTag_ALL_Pt_1D", PTEle, weight, NPtBinEdges1-1, PtBinEdges1);
      FillHist("NEle1Trig_AltTag_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges1-1, PtBinEdges1, NfEtaBinEdges-1, fEtaBinEdges);
      FillHist("LowPt/NEle1Trig_AltTag_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges1-2, PtBinEdges3, NfEtaBinEdges-1, fEtaBinEdges);
    }
    if(PassLeg2){
      FillHist("NEle2Trig_AltTag_ALL_Pt_1D", PTEle, weight, NPtBinEdges2-1, PtBinEdges2);
      FillHist("NEle2Trig_AltTag_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges2-1, PtBinEdges2, NfEtaBinEdges-1, fEtaBinEdges);
      FillHist("LowPt/NEle2Trig_AltTag_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges2-2, PtBinEdges4, NfEtaBinEdges-1, fEtaBinEdges);
    }
  }

}


void HNL_TriggerEfficiency::MeasEffEMuTrig_DZ(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                                    vector<Jet>& JetColl, vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label)
{
  if(   GetEraShort()=="2016a"                  ) return;
  if( !(MuTColl.size()==1 && MuLColl.size()==1) ) return;
  if( !(ElTColl.size()==1 && ElLColl.size()==1) ) return;
  if(!IsDATA){ if( !(MuTColl.at(0).IsPrompt() && ElTColl.at(0).IsPrompt()) ) return; }
  if( !(ElTColl.at(0).Pt()>15 && MuTColl.at(0).Pt()>10) ) return;
  if( !(ElTColl.at(0).Pt()>25 || MuTColl.at(0).Pt()>25) ) return;
  if(  ElTColl.at(0).Charge() == MuTColl.at(0).Charge() ) return;
  if(  ElTColl.at(0).DeltaR(MuTColl.at(0))<0.4  ) return;
  if( !(JetColl.size()>1 && BJetColl.size()>0) ) return;

  bool FireIsoEl1Mu2=false, FireIsoEl2Mu1=false, FireIsoAndDZEl1Mu2=false, FireIsoAndDZEl2Mu1=false;
  TString IsoFilterEl1 = "hltMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLElectronlegTrackIsoFilter";
  TString IsoFilterEl2 = "hltMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLElectronlegTrackIsoFilter"; 
  TString IsoFilterMu1 = "hltMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLMuonlegL3IsoFiltered23";
  TString IsoFilterMu2 = "hltMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLMuonlegL3IsoFiltered8";
  TString PathEl1Mu2   = "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v"; 
  TString PathEl2Mu1   = "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v"; 
  TString DZPathEl1Mu2 = "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v"; 
  TString DZPathEl2Mu1 = "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"; 
  if(ev.PassTrigger(PathEl1Mu2) && ElTColl.at(0).Pt()>25 && MuTColl.at(0).Pt()>10){
    if(ElTColl.at(0).PassFilter(IsoFilterEl1) && MuTColl.at(0).PassFilter(IsoFilterMu2)) FireIsoEl1Mu2=true;
    if(FireIsoEl1Mu2 && ElTColl.at(0).PassPath(DZPathEl1Mu2) && MuTColl.at(0).PassPath(DZPathEl1Mu2)) FireIsoAndDZEl1Mu2=true;
  }
  if(ev.PassTrigger(PathEl2Mu1) && ElTColl.at(0).Pt()>15 && MuTColl.at(0).Pt()>25){
    if(ElTColl.at(0).PassFilter(IsoFilterEl2) && MuTColl.at(0).PassFilter(IsoFilterMu1)) FireIsoEl2Mu1=true;
    if(FireIsoEl2Mu1 && ElTColl.at(0).PassPath(DZPathEl2Mu1) && MuTColl.at(0).PassPath(DZPathEl2Mu1)) FireIsoAndDZEl2Mu1=true;
  }

  if(FireIsoEl1Mu2){
    FillHist("NEvtIsoEl1Mu2"+Label, 0., weight, 2, 0., 2.);
    if(FireIsoAndDZEl1Mu2){ FillHist("NEvtIsoEl1Mu2DZTrig"+Label, 0., weight, 2, 0., 2.); }
  }
  if(FireIsoEl2Mu1){
    FillHist("NEvtIsoEl2Mu1"+Label, 0., weight, 2, 0., 2.);
    if(FireIsoAndDZEl2Mu1){ FillHist("NEvtIsoEl2Mu1DZTrig"+Label, 0., weight, 2, 0., 2.); }
  }
  if(FireIsoEl1Mu2 or FireIsoEl2Mu1){
    FillHist("NEvtIsoEMu"+Label, 0., weight, 2, 0., 2.);
    if( (FireIsoEl1Mu2 && FireIsoAndDZEl1Mu2) or (FireIsoEl2Mu1 && FireIsoAndDZEl2Mu1) ){
      FillHist("NEvtIsoEMuDZTrig"+Label, 0., weight, 2, 0., 2.);
    }
  }

} 




void HNL_TriggerEfficiency::MeasEffDiMuTrig_DZ(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                                     vector<Jet>& JetColl, vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label)
{
  if( !(MuTColl.size()==2 && MuLColl.size()==2) ) return;
  if( ElLColl.size()!=0 ) return;
  if(!IsDATA){ if( !(MuTColl.at(0).IsPrompt() && MuTColl.at(1).IsPrompt()) ) return; }
  if( !(MuTColl.at(0).Pt()>20. && MuTColl.at(1).Pt()>10.) ) return;
  if( MuTColl.at(0).Charge() == MuTColl.at(1).Charge() ) return;
  
  float Mll = (MuTColl.at(0)+MuTColl.at(1)).M();
  float dRll = MuTColl.at(0).DeltaR(MuTColl.at(1));

  bool FireIso=false, FireIsoAndDZ=false, FireIsoAndDZAndM=false, FireIsoAndM=false;
  //it seems associated filter label doesn't have to be that of firing filter.
  //for filter requiring multiplicity, simply counting matching object implies firing of trigger (e.g. last MuIsofilter),
  //but it may not be filters calculating quantities, but it will not apply to path matching, as it requires successful path
  for(unsigned int itr=0; itr<TrigList_DblMu.size(); itr++){
    if(!ev.PassTrigger(TrigList_DblMu.at(itr))) continue;
    int NMatchIso=0, NMatchIsoAndDZ=0, NMatchIsoAndDZAndM=0, NMatchIsoAndM=0;
    for(unsigned int im=0; im<MuTColl.size(); im++){
      if(MuTColl.at(im).PassPath(TrigList_DblMu.at(itr))){
        NMatchIso++;
        if(MuTColl.at(im).PassPath(TrigList_DblMuDZ.at(itr))){
          NMatchIsoAndDZ++;
          if(TrigList_DblMuDZM.size()>0 && MuTColl.at(im).PassPath(TrigList_DblMuDZM.at(0))) NMatchIsoAndDZAndM++;
        }
        if(TrigList_DblMuDZM.size()>0 && MuTColl.at(im).PassPath(TrigList_DblMuDZM.at(0))) NMatchIsoAndM++;
      }
    }
    FireIso          = FireIso          || NMatchIso         ==2;
    FireIsoAndDZ     = FireIsoAndDZ     || NMatchIsoAndDZ    ==2;
    FireIsoAndDZAndM = FireIsoAndDZAndM || NMatchIsoAndDZAndM==2;
    FireIsoAndM      = FireIsoAndM      || NMatchIsoAndM     ==2;
  }

  bool FireDZ=false, FireDZAndM=false;
  for(unsigned int itr=0; itr<TrigList_DblMuDZ.size(); itr++){
    if(!ev.PassTrigger(TrigList_DblMuDZ.at(itr))) continue;
    int NMatchDZ=0, NMatchDZAndM=0;
    for(unsigned int im=0; im<MuTColl.size(); im++){
      if(MuTColl.at(im).PassPath(TrigList_DblMuDZ.at(itr))){
        NMatchDZ++;
        if(TrigList_DblMuDZM.size()>0 && MuTColl.at(im).PassPath(TrigList_DblMuDZM.at(0))) NMatchDZAndM++;
      }
    }
    FireDZ     = FireDZ     || NMatchDZ    ==2;
    FireDZAndM = FireDZAndM || NMatchDZAndM==2;
  }
  
  bool FireM=false;
  for(unsigned int itr=0; itr<TrigList_DblMuDZM.size(); itr++){
    if(!ev.PassTrigger(TrigList_DblMuDZM.at(itr))) continue;
    int NMatchM=0;
    for(unsigned int im=0; im<MuTColl.size(); im++){
      if(MuTColl.at(im).PassPath(TrigList_DblMuDZM.at(itr)))  NMatchM++;
    }
    FireM = FireM || NMatchM==2;
  }


  vector<TString> SelTagList;
  if(Mll>60. && Mll<120. && dRll>0.4) SelTagList.push_back("_ZWinW60");
  if(fabs(Mll-M_Z)<20. && dRll>0.4) SelTagList.push_back("_ZWinW40");
  if(fabs(Mll-M_Z)<15. && dRll>0.4) SelTagList.push_back("_ZWinW30");
  if(fabs(Mll-M_Z)<10. && dRll>0.4) SelTagList.push_back("_ZWinW20");

  for(unsigned int iSel=0; iSel<SelTagList.size(); iSel++){
    TString SelTag(SelTagList.at(iSel));
    if(FireIso){
      FillHist("NEvtIso"     +SelTag+Label, 0.5, weight, 2, 0., 2.);
      FillHist("NEvtIso_DZ1" +SelTag+Label, fabs(MuTColl.at(0).dZ()), weight, 40, 0., 0.4);
      FillHist("NEvtIso_DZ2" +SelTag+Label, fabs(MuTColl.at(1).dZ()), weight, 40, 0., 0.4);
      FillHist("NEvtIso_DZ12"+SelTag+Label, fabs(MuTColl.at(0).dZ()-MuTColl.at(1).dZ()), weight, 40, 0., 0.4);
    }
    if(FireIsoAndDZ){
      FillHist("NEvtIsoDZTrig"     +SelTag+Label, 0.5, weight, 2, 0., 2.);
      FillHist("NEvtIsoDZTrig_DZ1" +SelTag+Label, fabs(MuTColl.at(0).dZ()), weight, 40, 0., 0.4);
      FillHist("NEvtIsoDZTrig_DZ2" +SelTag+Label, fabs(MuTColl.at(1).dZ()), weight, 40, 0., 0.4);
      FillHist("NEvtIsoDZTrig_DZ12"+SelTag+Label, fabs(MuTColl.at(0).dZ()-MuTColl.at(1).dZ()), weight, 40, 0., 0.4);
    }
    if(FireDZ          ) FillHist("NEvtDZTrig"     +SelTag+Label, 0.5, weight, 2, 0., 2.);
    if(FireIsoAndDZAndM) FillHist("NEvtIsoDZMTrig"+SelTag+Label, 0.5, weight, 2, 0., 2.);
    if(FireIsoAndM     ){
      FillHist("NEvtIsoMTrig"     +SelTag+Label, 0.5, weight, 2, 0., 2.);
      FillHist("NEvtIsoMTrig_DZ1" +SelTag+Label, fabs(MuTColl.at(0).dZ()), weight, 40, 0., 0.4);
      FillHist("NEvtIsoMTrig_DZ2" +SelTag+Label, fabs(MuTColl.at(1).dZ()), weight, 40, 0., 0.4);
      FillHist("NEvtIsoMTrig_DZ12"+SelTag+Label, fabs(MuTColl.at(0).dZ()-MuTColl.at(1).dZ()), weight, 40, 0., 0.4);
    }
    if(FireDZAndM      ) FillHist("NEvtDZMTrig"   +SelTag+Label, 0.5, weight, 2, 0., 2.);
    if(FireM           ) FillHist("NEvtMTrig"     +SelTag+Label, 0.5, weight, 2, 0., 2.);
  }
  if(FireDZ){
    FillHist("NEvtDZTrig_MllNearCut"+Label, Mll, weight, 100, 0., 10.);
    FillHist("NEvtDZTrig_Mll"       +Label, Mll, weight, 38, 10., 200.);
    if(FireDZAndM){
      FillHist("NEvtDZMTrig_MllNearCut"+Label, Mll, weight, 100, 0., 10.);
      FillHist("NEvtDZMTrig_Mll"       +Label, Mll, weight, 38, 10., 200.);
    }
  }
} 


void HNL_TriggerEfficiency::MeasEffDiElTrig_DZ(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                                     vector<Jet>&  JetColl, vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label)
{
  if( TrigList_DblEG.size() != TrigList_DblEGDZ.size() ) return;
  if( !(ElTColl.size()==2 && ElLColl.size()==2) ) return;
  if( MuLColl.size()!=0 ) return;
  if(!IsDATA){ if( !(ElTColl.at(0).IsPrompt() && ElTColl.at(1).IsPrompt()) ) return; }
  if( !(ElTColl.at(0).Pt()>25. && ElTColl.at(1).Pt()>15.) ) return;
  if(  ElTColl.at(0).Charge() == ElTColl.at(1).Charge() ) return;
  
  float Mll  = (ElTColl.at(0)+ElTColl.at(1)).M();
  float dRll = ElTColl.at(0).DeltaR(ElTColl.at(1));

  bool FireIso=false, FireIsoAndDZ=false;
  for(unsigned int itr=0; itr<TrigList_DblEG.size(); itr++){
    if(!ev.PassTrigger(TrigList_DblEG.at(itr))) continue;
    int NMatchIso=0, NMatchIsoAndDZ=0;
    for(unsigned int ie=0; ie<ElTColl.size(); ie++){
      if(ElTColl.at(ie).PassPath(TrigList_DblEG.at(itr))){
        NMatchIso++;
        if(ElTColl.at(ie).PassPath(TrigList_DblEGDZ.at(itr))) NMatchIsoAndDZ++;
      }
    }
    FireIso      = FireIso      || NMatchIso      ==2;
    FireIsoAndDZ = FireIsoAndDZ || NMatchIsoAndDZ ==2;
  }

  bool FireDZ=false;
  for(unsigned int itr=0; itr<TrigList_DblEGDZ.size(); itr++){
    if(!ev.PassTrigger(TrigList_DblEGDZ.at(itr))) continue;
    int NMatchDZ=0;
    for(unsigned int ie=0; ie<ElTColl.size(); ie++){
      if(ElTColl.at(ie).PassPath(TrigList_DblEGDZ.at(itr))) NMatchDZ++;
    }
    FireDZ = FireDZ || NMatchDZ ==2;
  }

  vector<TString> SelTagList;
  if(Mll>60. && Mll<120. && dRll>0.3) SelTagList.push_back("_ZWinW60");
  if(fabs(Mll-M_Z)<20. && dRll>0.3) SelTagList.push_back("_ZWinW40");
  if(fabs(Mll-M_Z)<15. && dRll>0.3) SelTagList.push_back("_ZWinW30");
  if(fabs(Mll-M_Z)<10. && dRll>0.3) SelTagList.push_back("_ZWinW20");

  for(unsigned int iSel=0; iSel<SelTagList.size(); iSel++){
    TString SelTag(SelTagList.at(iSel));
    if(FireIso){
      FillHist("NEvtIso"     +SelTag+Label, 0.5, weight, 2, 0., 2.);
      FillHist("NEvtIso_DZ1" +SelTag+Label, fabs(ElTColl.at(0).dZ()), weight, 10, 0., 0.1);
      FillHist("NEvtIso_DZ2" +SelTag+Label, fabs(ElTColl.at(1).dZ()), weight, 10, 0., 0.1);
      FillHist("NEvtIso_DZ12"+SelTag+Label, fabs(ElTColl.at(0).dZ()-ElTColl.at(1).dZ()), weight, 20, 0., 0.2);
    }
    if(FireIsoAndDZ){
      FillHist("NEvtIsoDZTrig"     +SelTag+Label, 0.5, weight, 2, 0., 2.);
      FillHist("NEvtIsoDZTrig_DZ1" +SelTag+Label, fabs(ElTColl.at(0).dZ()), weight, 10, 0., 0.1);
      FillHist("NEvtIsoDZTrig_DZ2" +SelTag+Label, fabs(ElTColl.at(1).dZ()), weight, 10, 0., 0.1);
      FillHist("NEvtIsoDZTrig_DZ12"+SelTag+Label, fabs(ElTColl.at(0).dZ()-ElTColl.at(1).dZ()), weight, 20, 0., 0.2);
    }
    if(FireDZ) FillHist("NEvtDZTrig"+SelTag+Label, 0.5, weight, 2, 0., 2.);
  }
} 


void HNL_TriggerEfficiency::MeasSiglEleTrigEff(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                                     vector<Jet>& JetColl, vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label)

{
  if( !(ElTColl.size()==1 && ElLColl.size()==1) ) return;
  if( !(MuTColl.size()==1 && MuLColl.size()==1) ) return;
  if( MuTColl.at(0).Charge() == ElTColl.at(0).Charge() ) return;
  if( DataYear==2016 or DataYear==2018 ){ 
    if(MuTColl.at(0).Pt()<26) return;
  }
  else{
    if(MuTColl.at(0).Pt()<29) return;
  }
  if( ElTColl.at(0).DeltaR(MuTColl.at(0))<0.4 ) return;

  //Sanity check
  FillHist("PtMu1" +Label, MuTColl.at(0).Pt(), weight, 30, 0., 300.);
  FillHist("PtEl1" +Label, ElTColl.at(0).Pt(), weight, 30, 0., 300.);
  FillHist("EtaMu1"+Label, MuTColl.at(0).Eta(), weight, 20, -5., 5.);
  FillHist("EtaEl1"+Label, ElTColl.at(0).Eta(), weight, 20, -5., 5.);
  FillHist("MET"   +Label, vMET.Pt(), weight, 40, 0., 400.);


  vector<TString> TrigListToMeas;
  if(DataYear==2016){ TrigListToMeas.push_back("HLT_Ele27_WPTight_Gsf_v"); }
  if(DataYear==2017){ TrigListToMeas.push_back("HLT_Ele32_WPTight_Gsf_v"); TrigListToMeas.push_back("HLT_Ele32_WPTight_Gsf_L1DoubleEG_v"); }
  if(DataYear==2018){ TrigListToMeas.push_back("HLT_Ele32_WPTight_Gsf_v"); }

  const int NPtBinEdges=11, NfEtaBinEdges=4;
  double PtBinEdges[NPtBinEdges];
  double PtBinEdges_16[NPtBinEdges]   = {0., 15., 24., 27., 30., 35., 45., 70., 100., 200., 500};
  double PtBinEdges_1718[NPtBinEdges] = {0., 20., 29., 32., 35., 40., 50., 70., 100., 200., 500};
  if(DataYear==2016){ copy(PtBinEdges_16, PtBinEdges_16+NPtBinEdges, PtBinEdges); }
  else              { copy(PtBinEdges_1718, PtBinEdges_1718+NPtBinEdges, PtBinEdges); }

  double fEtaBinEdges[NfEtaBinEdges]={0., 0.8, 1.479, 2.5};
  double PTEle   = ElTColl.at(0).Pt();
  double fEtaEle = fabs(ElTColl.at(0).Eta());

  FillHist("NEle_ALL_Pt_1D", PTEle, weight, NPtBinEdges-1, PtBinEdges);
  FillHist("NEle_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges-1, PtBinEdges, NfEtaBinEdges-1, fEtaBinEdges);
  if(ev.PassTrigger(TrigListToMeas)){
    FillHist("NEleTrig_ALL_Pt_1D", PTEle, weight, NPtBinEdges-1, PtBinEdges);
    FillHist("NEleTrig_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges-1, PtBinEdges, NfEtaBinEdges-1, fEtaBinEdges);
  }

  //Syst:QCD contamination 
  if(MuTColl.at(0).Pt()>35 && MuTColl.at(0).RelIso()<0.1){
    FillHist("NEle_AltTag_ALL_Pt_1D", PTEle, weight, NPtBinEdges-1, PtBinEdges);
    FillHist("NEle_AltTag_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges-1, PtBinEdges, NfEtaBinEdges-1, fEtaBinEdges);
    if(ev.PassTrigger(TrigListToMeas)){
      FillHist("NEleTrig_AltTag_ALL_Pt_1D", PTEle, weight, NPtBinEdges-1, PtBinEdges);
      FillHist("NEleTrig_AltTag_ALL_PtEta_2D", PTEle, fEtaEle, weight, NPtBinEdges-1, PtBinEdges, NfEtaBinEdges-1, fEtaBinEdges);
    }
  }
  
}





void HNL_TriggerEfficiency::executeEventFromParameter(AnalyzerParameter param){

  if(!PassMETFilter()) return;

  Event ev = GetEvent();

}

HNL_TriggerEfficiency::HNL_TriggerEfficiency(){

}

HNL_TriggerEfficiency::~HNL_TriggerEfficiency(){

}
