#include "Fake.h"

Fake::Fake(){

}

void Fake::initializeAnalyzer(){

  MuonTightIDs = {"SSWW_tight","HNTight2016"};
  MuonLooseIDs = {"SSWW_loose","HNLoose2016"};
  MuonVetoIDs = {"SSWW_loose","HNVeto2016"};
  if(DataYear==2016){
    ElectronTightIDs = {"SSWW_tight2016","HNTight2016"};
    ElectronLooseIDs = {"SSWW_loose2016","HNLoose2016"};
    ElectronVetoIDs = {"SSWW_loose2016","HNVeto2016"};
  }
  else{
    ElectronTightIDs = {"SSWW_tight","HNTight2016"};
    ElectronLooseIDs = {"SSWW_loose","HNLoose2016"};
    ElectronVetoIDs = {"SSWW_loose","HNVeto2016"};
  }

  MuonTriggers.clear();
  ElectronTriggers.clear();

  if(DataYear==2016){ // Lumi values for trigger weight (pb^-1)
    MuonTrigger3 = "HLT_Mu3_PFJet40_v"; // 7.408 (HN AN)
    MuonTrigger8 = "HLT_Mu8_TrkIsoVVL_v"; // 7.80
    MuonTrigger17 = "HLT_Mu17_TrkIsoVVL_v"; // 216.75
    ElectronTrigger8 = "HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v"; // 6.992
    ElectronTrigger12 = "HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v"; // 14.85
    ElectronTrigger17 = "HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30_v"; // 58.896
    ElectronTrigger23 = "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v"; // 62.81
    MuonTriggers.push_back(MuonTrigger3);
    MuonTriggers.push_back(MuonTrigger8);
    MuonTriggers.push_back(MuonTrigger17);
    ElectronTriggers.push_back(ElectronTrigger8);
    ElectronTriggers.push_back(ElectronTrigger12);
    ElectronTriggers.push_back(ElectronTrigger17);
    ElectronTriggers.push_back(ElectronTrigger23);
    MuonPtCut1 = 20.;
    MuonPtCut2 = 10.;
    ElectronPtCut1 = 25.;
    ElectronPtCut2 = 15.;
  }
  else if(DataYear==2017){
    MuonTrigger8 = "HLT_Mu8_TrkIsoVVL_v"; // 2.90
    MuonTrigger17 = "HLT_Mu17_TrkIsoVVL_v"; // 65.94
    ElectronTrigger8 = "HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v"; // 6.992
    ElectronTrigger12 = "HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v"; // 14.85
    ElectronTrigger17 = "HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v"; // 35.578856488
    ElectronTrigger23 = "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v"; // 62.81
    MuonTriggers.push_back(MuonTrigger3);
    MuonTriggers.push_back(MuonTrigger8);
    MuonTriggers.push_back(MuonTrigger17);
    ElectronTriggers.push_back(ElectronTrigger8);
    ElectronTriggers.push_back(ElectronTrigger12);
    ElectronTriggers.push_back(ElectronTrigger17);
    ElectronTriggers.push_back(ElectronTrigger23);
    MuonPtCut1 = 20.;
    MuonPtCut2 = 10.;
    ElectronPtCut1 = 25.;
    ElectronPtCut2 = 15.;
  }
  else if(DataYear==2018){
    MuonTrigger8 = "HLT_Mu8_TrkIsoVVL_v"; // 8.56
    MuonTrigger17 = "HLT_Mu17_TrkIsoVVL_v"; // 45.78
    ElectronTrigger8 = "HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v"; // 6.992
    ElectronTrigger12 = "HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v"; // 14.85
    ElectronTrigger17 = "HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v"; // 38.929415911(2018)
    ElectronTrigger23 = "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v"; // 62.81
    MuonTriggers.push_back(MuonTrigger3);
    MuonTriggers.push_back(MuonTrigger8);
    MuonTriggers.push_back(MuonTrigger17);
    ElectronTriggers.push_back(ElectronTrigger8);
    ElectronTriggers.push_back(ElectronTrigger12);
    ElectronTriggers.push_back(ElectronTrigger17);
    ElectronTriggers.push_back(ElectronTrigger23);
    MuonPtCut1 = 20.;
    MuonPtCut2 = 10.;
    ElectronPtCut1 = 25.;
    ElectronPtCut2 = 15.;
  }

  //==== B-Tagging
  //==== add taggers and WP that you want to use in analysis
  std::vector<JetTagging::Parameters> jtps;
  //==== If you want to use 1a or 2a method,
  jtps.push_back( JetTagging::Parameters(JetTagging::DeepCSV, JetTagging::Loose, JetTagging::incl, JetTagging::comb) );
  jtps.push_back( JetTagging::Parameters(JetTagging::DeepCSV, JetTagging::Medium, JetTagging::incl, JetTagging::comb) );
  //==== set
  mcCorr->SetJetTaggingParameters(jtps); //JH : NOTE This is used in mcCorr->SetupJetTagging() in m.initializeAnalyzerTools();
}

Fake::~Fake(){

  //==== Destructor of this Analyzer

}

void Fake::executeEvent(Long64_t Nentry){

  AllMuons = GetAllMuons();
  AllElectrons = GetAllElectrons();
  AllJets = GetAllJets();
  AllFatJets = puppiCorr->Correct(GetAllFatJets()); //JH : puppiCorr = new PuppiSoftdropMassCorr(); in the constructor of AnalyzerCore.C; apply correction to fatjet.SDMass(); the total weight = gen correction * reco correction, from SKFlatAnalyzer/data/Run2Legacy_v4/DataYear/PuppiSoftdropMassCorr/puppiCorr.root

  //==== Get L1Prefire reweight
  //==== If data, 1.;
  //==== If MC && DataYear > 2017, 1.;
  //==== If MC && DataYear <= 2017, we have to reweight the event with this value
  //==== I defined "double weight_Prefire;" in Analyzers/include/Fake.h
//  weight_Prefire = GetPrefireWeight(0);

  AnalyzerParameter param;

  for(unsigned int it_id=0; it_id<ElectronTightIDs.size(); it_id++){
    TString MuonTightID = MuonTightIDs.at(it_id);
    TString MuonLooseID = MuonLooseIDs.at(it_id); 
    TString MuonVetoID = MuonVetoIDs.at(it_id); 
    TString ElectronTightID = ElectronTightIDs.at(it_id);
    TString ElectronLooseID = ElectronLooseIDs.at(it_id);
    TString ElectronVetoID = ElectronVetoIDs.at(it_id);

    param.Clear();

    param.syst_ = AnalyzerParameter::Central;

    // Muon ID
    param.Muon_Tight_ID = MuonTightID;
    param.Muon_Loose_ID = MuonLooseID;
    param.Muon_Veto_ID = MuonVetoID;

    // Electron ID
    param.Electron_Tight_ID = ElectronTightID;
    param.Electron_Loose_ID = ElectronLooseID;
    param.Electron_Veto_ID = ElectronVetoID;

    // Jet ID
    param.Jet_ID = "HNTight"; //JH : pass POG tight + pass loose PU ID
    if(DataYear==2016) param.FatJet_ID = "HNTight0p55";
    else param.FatJet_ID = "HNTight0p45"; //JH : TODO

    executeEventFromParameter(param, Nentry);

/*  if(RunSyst){
      for(int it_syst=1; it_syst<AnalyzerParameter::NSyst; it_syst++){
        param.syst_ = AnalyzerParameter::Syst(it_syst);
        param.Name = MuonID+"_"+"Syst_"+param.GetSystType();
        executeEventFromParameter(param, Nentry);
      }
    }*/
  }
}

void Fake::executeEventFromParameter(AnalyzerParameter param, Long64_t Nentry){

  vector<TString> regionsFake = {"SSWW","HN"};
  vector<TString> regionsNorm = {"Z_Muon","Z_Electron","W_Muon","W_Electron"};
  TString IDsuffix;
  if(param.Muon_Tight_ID.Contains("SSWW")) IDsuffix = "SSWW";
  else if(param.Muon_Tight_ID.Contains("HN")) IDsuffix = "HN";
  double cutflow_max = 10.;
  int cutflow_bin = 10;
  double weight = 1.;
  double weight_common = 1.;
 
  Event ev = GetEvent();

  bool isSingleMuon = false, isDoubleMuon = false, isSingleElectron = false, isDoubleEG = false; // see https://github.com/jalmond/SKFlatAnalyzer/blob/HNtypeI_UltraLegacy/DataFormats/src/Event.C#L1062
  if(IsDATA){
    if(DataStream.Contains("SingleMuon")) isSingleMuon = true;
    if(DataStream.Contains("DoubleMuon")) isDoubleMuon = true;
    if(DataStream.Contains("SingleElectron")||DataStream.Contains("EGamma")) isSingleElectron = true;
    if(DataStream.Contains("DoubleEG")||DataStream.Contains("EGamma")) isDoubleEG = true;
  }
  if(!IsDATA){
    weight_common *= weight_norm_1invpb; //JH : weight_norm_1invpb = xsec/sumW; Lumi = 35.9, 41.5, 59.7(fb-1) total 137fb-1
    weight_common *= ev.MCweight(); //JH : gen_weight in MiniAOD
    weight_common *= GetPrefireWeight(0); //JH : No issue in 2018, otherwise returns L1PrefireReweight_Central in MiniAOD
    weight_common *= GetPileUpWeight(nPileUp,0); //JH : mcCorr->GetPileUpWeight(N_pileup, syst); mcCorr->GetPileUpWeight2017(N_pileup, syst); NOTE 2018 not yet added.
  } //JH : weight except the trigger lumi

  //========================
  //==== MET Filter
  //========================

  if(!PassMETFilter()) return;

  //==============
  //==== Trigger
  //==============

  //if(ev.PassTrigger(MuonTriggers)||ev.PassTrigger(ElectronTriggers)) PrintTrigger();
  //PrintTrigger();
  //If(ev.PassTrigger(ElectronTriggers)){
  //  cout << "!!Electron Trigger passed!!" << endl;
  //  PrintTrigger();
  //}
  //if(Nentry == 0) PrintTrigger();

  //======================
  //==== Copy AllObjects
  //======================

  vector<Electron> this_AllElectrons = AllElectrons;
  vector<Muon> this_AllMuons;
  if(param.Muon_Tight_ID.Contains("HighPt")) this_AllMuons = UseTunePMuon(AllMuons);
  else this_AllMuons = AllMuons;
  vector<Jet> this_AllJets = AllJets;
  vector<FatJet> this_AllFatJets = AllFatJets;
  //FillHist("Number_AllJets_"+IDsuffix, this_AllJets.size(), weight, 10, 0, 10);
  //FillHist("Number_AllJets_unweighted_"+IDsuffix, this_AllJets.size(), 1., 10, 0, 10);
  //FillHist("Number_AllFatJets_"+IDsuffix, this_AllFatJets.size(), weight, 10, 0, 10);
  //FillHist("Number_AllFatJets_unweighted_"+IDsuffix, this_AllFatJets.size(), 1., 10, 0, 10);
  vector<Gen> gens = GetGens();

  //==== Then, for each systematic sources
  //==== 1) Smear or scale them
  //==== 2) Then apply ID selections
  //==== This order should be explicitly followed
  //==== Below are all variables for available systematic sources

/*  if(param.syst_ == AnalyzerParameter::Central){

  }
  else if(param.syst_ == AnalyzerParameter::JetResUp){
    this_AllJets = SmearJets( this_AllJets, +1 );
    //this_AllFatJets = SmearFatJets( this_AllFatJets, +1 );
  }
  else if(param.syst_ == AnalyzerParameter::JetResDown){
    this_AllJets = SmearJets( this_AllJets, -1 );
    //this_AllFatJets = SmearFatJets( this_AllFatJets, -1 );
  }
  else if(param.syst_ == AnalyzerParameter::JetEnUp){
    this_AllJets = ScaleJets( this_AllJets, +1 );
    //this_AllFatJets = ScaleFatJets( this_AllFatJets, +1 );
  }
  else if(param.syst_ == AnalyzerParameter::JetEnDown){
    this_AllJets = ScaleJets( this_AllJets, -1 );
    //this_AllFatJets = ScaleFatJets( this_AllFatJets, -1 );
  }
  else if(param.syst_ == AnalyzerParameter::MuonEnUp){
    this_AllMuons = ScaleMuons( this_AllMuons, +1 );
  }
  else if(param.syst_ == AnalyzerParameter::MuonEnDown){
    this_AllMuons = ScaleMuons( this_AllMuons, -1 );
  }
  else if(param.syst_ == AnalyzerParameter::ElectronResUp){
    //this_AllElectrons = SmearElectrons( this_AllElectrons, +1 );
  }
  else if(param.syst_ == AnalyzerParameter::ElectronResDown){
    //this_AllElectrons = SmearElectrons( this_AllElectrons, -1 );
  }
  else if(param.syst_ == AnalyzerParameter::ElectronEnUp){
    //this_AllElectrons = ScaleElectrons( this_AllElectrons, +1 );
  }
  else if(param.syst_ == AnalyzerParameter::ElectronEnDown){
    //this_AllElectrons = ScaleElectrons( this_AllElectrons, -1 );
  }
  else{
    cout << "[Fake::executeEventFromParameter] Wrong syst" << endl;
    exit(EXIT_FAILURE);
  }*/

  //==================================================
  //==== Then, apply ID selections using this_AllXXX
  //==================================================

  TString MuonTightID = param.Muon_Tight_ID;
  TString MuonLooseID = param.Muon_Loose_ID;
  TString MuonVetoID = param.Muon_Veto_ID;
  TString ElectronTightID = param.Electron_Tight_ID;
  TString ElectronLooseID = param.Electron_Loose_ID;
  TString ElectronVetoID = param.Electron_Veto_ID;

  vector<Muon> muons_tight = SelectMuons(this_AllMuons, MuonTightID, 10., 2.4);
  vector<Muon> muons_loose = SelectMuons(this_AllMuons, MuonLooseID, 10., 2.4);
  vector<Muon> muons_veto = SelectMuons(this_AllMuons, MuonVetoID, 5., 2.4);
  vector<Electron> electrons_tight = SelectElectrons(this_AllElectrons, ElectronTightID, 10., 2.5);
  vector<Electron> electrons_loose = SelectElectrons(this_AllElectrons, ElectronLooseID, 10., 2.5);
  vector<Electron> electrons_veto = SelectElectrons(this_AllElectrons, ElectronVetoID, 10., 2.5);
  vector<Jet> jets_bcand = SelectJets(this_AllJets, param.Jet_ID, 20., 2.4); //JH : to reject bjets

  // Jet, FatJet selection to avoid double counting due to jets matched geometrically with a lepton
  vector<Jet> jets;
  vector<FatJet> fatjets;
  jets.clear();
  fatjets.clear();
  int lepton_count1 = 0, lepton_count2 = 0, fatjet_count = 0, jet_count = 0; 

  // Fatjet selection in CATanalyzer (see the links)
  // https://github.com/jedori0228/LQanalyzer/blob/CatAnalyzer_13TeV_v8-0-7.36_HNAnalyzer/CATConfig/SelectionConfig/user_fatjets.sel
  // https://github.com/jedori0228/LQanalyzer/blob/CatAnalyzer_13TeV_v8-0-7.36_HNAnalyzer/LQCore/Selection/src/FatJetSelection.cc#L113-L124

  for(unsigned int i=0; i<this_AllFatJets.size(); i++){
    lepton_count1 = 0;
    if(!(this_AllFatJets.at(i).PassID(param.FatJet_ID))) continue; //JH : "HNTight"
    if(!(this_AllFatJets.at(i).Pt() > 200.)) continue;
    if(!(fabs(this_AllFatJets.at(i).Eta()) < 2.7)) continue;
    for(unsigned int j=0; j<muons_veto.size(); j++){
      if(this_AllFatJets.at(i).DeltaR(muons_veto.at(j)) < 0.8) lepton_count1++; //JH : veto muon cleaning
    }
    for(unsigned int j=0; j<electrons_veto.size(); j++){
      if(this_AllFatJets.at(i).DeltaR(electrons_veto.at(j)) < 0.8) lepton_count1++; //JH : veto electron cleaning
    } 
    if(lepton_count1 > 0) continue;
    fatjets.push_back(this_AllFatJets.at(i));
  }
  for(unsigned int i=0; i<this_AllJets.size(); i++){
    lepton_count2 = 0, fatjet_count = 0;
    if(!(this_AllJets.at(i).PassID(param.Jet_ID))) continue; //JH :"HNTight"
    if(!(this_AllJets.at(i).Pt() > 20.)) continue;
    if(!(fabs(this_AllJets.at(i).Eta()) < 2.7)) continue;
    for(unsigned int j=0; j<muons_veto.size(); j++){
      if(this_AllJets.at(i).DeltaR(muons_veto.at(j)) < 0.4) lepton_count2++; //JH : veto muon cleaning
    }
    for(unsigned int j=0; j<electrons_veto.size(); j++){
      if(this_AllJets.at(i).DeltaR(electrons_veto.at(j)) < 0.4) lepton_count2++; //JH : veto electron cleaning
    }
    //for(unsigned int j=0; j<fatjets.size(); j++){
    //  if(this_AllJets.at(i).DeltaR(fatjets.at(j)) < 0.8) fatjet_count++; //JH : fatjet cleaning
    //}
    if(lepton_count2 > 0) continue;
    //if(fatjet_count > 0) continue;
    jets.push_back(this_AllJets.at(i));
  }
  //FillHist("Number_JetsSel_"+IDsuffix, jets.size(), weight, 10, 0, 10);
  //FillHist("Number_JetsSel_unweighted_"+IDsuffix, jets.size(), 1., 10, 0, 10);
  //FillHist("Number_FatJetsSel_"+IDsuffix, fatjets.size(), weight, 10, 0, 10);
  //FillHist("Number_FatJetsSel_unweighted_"+IDsuffix, fatjets.size(), 1., 10, 0, 10);
//JH : jet, fatjet selection done.

  //=======================
  //==== Sort in pt-order
  //=======================

  std::sort(muons_tight.begin(), muons_tight.end(), PtComparing);
  std::sort(muons_loose.begin(), muons_loose.end(), PtComparing);
  std::sort(muons_veto.begin(), muons_veto.end(), PtComparing);
  std::sort(electrons_tight.begin(), electrons_tight.end(), PtComparing);
  std::sort(electrons_loose.begin(), electrons_loose.end(), PtComparing);
  std::sort(electrons_veto.begin(), electrons_veto.end(), PtComparing);
  std::sort(jets.begin(), jets.end(), PtComparing);
  std::sort(jets_bcand.begin(), jets_bcand.end(), PtComparing);
  std::sort(fatjets.begin(), fatjets.end(), PtComparing);

  //==== B-Tagging 
  int Nbjet_loose = 0, Nbjet_medium = 0;
  JetTagging::Parameters jtp_DeepCSV_Loose = JetTagging::Parameters(JetTagging::DeepCSV,
                                                                     JetTagging::Loose,
                                                                     JetTagging::incl, JetTagging::comb);
  JetTagging::Parameters jtp_DeepCSV_Medium = JetTagging::Parameters(JetTagging::DeepCSV,
                                                                     JetTagging::Medium,
                                                                     JetTagging::incl, JetTagging::comb); //JH : Set b-tagging parameters

  //==== method 1a)
  //==== multiply "btagWeight" to the event weight
//  double btagWeight = mcCorr->GetBTaggingReweight_1a(jets, jtp_DeepCSV_Medium);

  //==== method 2a)
  for(unsigned int ij=0; ij<jets_bcand.size(); ij++){ 
//    double this_discr = jets_bcand.at(ij).GetTaggerResult(JetTagging::DeepCSV);
      //==== No SF
//      if( this_discr > mcCorr->GetJetTaggingCutValue(JetTagging::DeepCSV, JetTagging::Medium) ) NBJets_NoSF++;
    if(mcCorr->IsBTagged_2a(jtp_DeepCSV_Loose, jets_bcand.at(ij))) Nbjet_loose++; 
    if(mcCorr->IsBTagged_2a(jtp_DeepCSV_Medium, jets_bcand.at(ij))) Nbjet_medium++; //JH : count Nbjet. NOTE : AN says they used CVSv2 and medium WP.
  } 

//  FillHist("Nbjet_loose_"+IDsuffix, Nbjet_loose, weight, 5, 0., 5.);
//  FillHist("Nbjet_medium_"+IDsuffix, Nbjet_medium, weight, 5, 0., 5.);

  //===================================
  //==== Set up pTcone, lepton vector
  //===================================

  Particle METv = ev.GetMETVector();

  double Mt = 0.;
  double Mt3l = 0.;
  double ST = 0.;
  double HT = 0.;
  double MET2ST = 0.;
  double MZ = 91.1876;
  double MW = 80.379;
  double muon_recosf = 1.;
  double muon_idsf = 1.;
  double muon_isosf = 1.;
  double muon_trigsf = 1.;
  double ele_idsf = 1.;
  double ele_recosf = 1.;
  int lepton_veto_size = 0;
  Particle ZCand, WCand, Wtemp;
  Particle llj, l1j, l2j, lljj, l1jj, l2jj, l1J, l2J;
  Particle WtagLep, DiLep, DiJet, TriLep, ZtagLep1, ZtagLep2, Ztemp, Ztemp1, Ztemp2, Ztemp3, Ztemp4, ZCand1, ZCand2, GammaCand, GammaLep1, GammaLep2;
  int ossf_mass10 = 0;
  
  // Set tight_iso cut & calculate pTcone
  double mu_tight_iso = 0.07;
  double el_tight_iso = 0.;
  double this_ptcone_muon = 0., this_ptcone_electron = 0.;

  // Set pTcone
  for(unsigned int i=0; i<muons_loose.size(); i++){
    this_ptcone_muon = muons_loose.at(i).CalcPtCone(muons_loose.at(i).RelIso(), mu_tight_iso); //JH : CalcPtCone() in Lepton.h; this returns (i) pt for more tightly isolated leptons than the tight_iso, or (ii) pt + pt*(RelIso-tight_iso) which is the proxy for the mother parton's pt -> used for fake estimation
    muons_loose.at(i).SetPtCone(this_ptcone_muon);
  }
   
  for(unsigned int i=0; i<electrons_loose.size(); i++){
    el_tight_iso = 0.0287+0.506/electrons_loose.at(i).UncorrPt(); // https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Working_points_for_2016_data_for
    if(fabs(electrons_loose.at(i).scEta()) > 1.479) el_tight_iso = 0.0445+0.963/electrons_loose.at(i).UncorrPt();

    this_ptcone_electron = electrons_loose.at(i).CalcPtCone(electrons_loose.at(i).RelIso(), el_tight_iso);
    electrons_loose.at(i).SetPtCone(this_ptcone_electron);
  }

  // Define leptons (pT order)
  std::vector<Lepton*> leptons_loose;
  for(unsigned int i=0; i<muons_loose.size(); i++) leptons_loose.push_back(&muons_loose.at(i));
  for(unsigned int i=0; i<electrons_loose.size(); i++) leptons_loose.push_back(&electrons_loose.at(i));
  std::sort(leptons_loose.begin(), leptons_loose.end(), PtComparingPtr);
  std::vector<Lepton*> leptons_tight;
  for(unsigned int i=0; i<muons_tight.size(); i++) leptons_tight.push_back(&muons_tight.at(i));
  for(unsigned int i=0; i<electrons_tight.size(); i++) leptons_tight.push_back(&electrons_tight.at(i));
  std::sort(leptons_tight.begin(), leptons_tight.end(), PtComparingPtr);

  // Define HT, ST, MET^2/ST
  //for(unsigned int i=0; i<jets.size(); i++) HT += jets.at(i).Pt();
  //for(unsigned int i=0; i<jets.size(); i++) ST += jets.at(i).Pt();
  //for(unsigned int i=0; i<fatjets.size(); i++) ST += fatjets.at(i).Pt();
  //for(unsigned int i=0; i<leptons_loose.size(); i++) ST += leptons_loose.at(i)->Pt();
  //ST += MET;
  //MET2ST = MET*MET/ST;

  //=========================
  //==== Event selections..
  //========================= 

  //=====================================================================================
  //=====================================================================================
  //==== Fake rate meaurement
  //=====================================================================================
  //=====================================================================================

  if(HasFlag("FR")){

    if(! (jets.size()==0||leptons_loose.size()!=1) ){ // only 1 loose lepton and at least 1 jet
      Jet j1 = jets.at(0);
      METv = UpdateMETMuon(METv, muons_loose);
      METv = UpdateMETElectron(METv, electrons_loose);
      double MET = METv.Pt(); // MET propagated
      
      for(int it_rg=0; it_rg<regionsFake.size(); it_rg++){

        if(regionsFake.at(it_rg)=="SSWW"){ // SSWW selection

          continue; // I don't need SSWW selection for now
          if(IDsuffix=="HN") continue;

          if(muons_loose.size()==1){ // muon FR
            Muon mu1 = muons_loose.at(0);
            if(!IsDATA){
              if(-4<=GetLeptonType(mu1, gens)&&GetLeptonType(mu1, gens)<=0) continue; // Note here the prompt lepton includes the internal/external conversions from the EW process
            }
            // Cutflow 1 : only 1 muon and at least 1 jet
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 0.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 0.5, 1., cutflow_bin, 0., cutflow_max);

            // Cutflow 2 : pt-dependent trigger requirement
            if(IsDATA){
              if(!isDoubleMuon) continue;
            }

            if(mu1.Pt() < 20.){
              if(!( ev.PassTrigger(MuonTrigger8) )) continue;
              if(!IsDATA) weight = weight_common*ev.GetTriggerLumiByYear(MuonTrigger8);
            }
            else if(20 <= mu1.Pt() && mu1.Pt() < 50.){
              if(!( ev.PassTrigger(MuonTrigger17) )) continue;
              if(!IsDATA) weight = weight_common*ev.GetTriggerLumiByYear(MuonTrigger17);
            }
            else continue;
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 1.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 1.5, 1., cutflow_bin, 0., cutflow_max);

            // event selection
            if(!( mu1.DeltaR(j1) > 1. )) continue;
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 2.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 2.5, 1., cutflow_bin, 0., cutflow_max);
            if(!( j1.Pt() > 25. )) continue;
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 3.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 3.5, 1., cutflow_bin, 0., cutflow_max);
            if(!( MET < 20. )) continue;
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 4.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 4.5, 1., cutflow_bin, 0., cutflow_max);
            Mt = MT(mu1, METv);
            if(!( Mt < 20. )) continue;
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 5.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 5.5, 1., cutflow_bin, 0., cutflow_max);

            // fill histos
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Jet1_Pt_"+IDsuffix, j1.Pt(), weight, 8, 10., 50.);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Jet1_Eta_"+IDsuffix, j1.Eta(), weight, 20, -5., 5.);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Loose_"+IDsuffix, mu1.Pt(), fabs(mu1.Eta()), weight, 8, 10., 50., 5, 0., 2.5);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Loose_Pt_"+IDsuffix, mu1.Pt(), weight, 8, 10., 50.);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Loose_Eta_"+IDsuffix, fabs(mu1.Eta()), weight, 5, 0., 2.5);
            if(muons_tight.size() > 0){
              if( (muons_loose.at(0).Pt() != muons_tight.at(0).Pt()) || (muons_loose.at(0).Eta() != muons_tight.at(0).Eta()) ){
                cout << "!!ERROR!!" << endl;
                cout << "loose lepton pt, eta : " << muons_loose.at(0).Pt() << ", " << muons_loose.at(0).Eta() << endl;
                cout << "tight lepton pt, eta : " << muons_tight.at(0).Pt() << ", " << muons_tight.at(0).Eta() << endl;
              }
              FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 6.5, weight, cutflow_bin, 0., cutflow_max);
              FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 6.5, 1., cutflow_bin, 0., cutflow_max);
              FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Tight_"+IDsuffix, mu1.Pt(), fabs(mu1.Eta()), weight, 8, 10., 50., 5, 0., 2.5);
              FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Tight_Pt_"+IDsuffix, mu1.Pt(), weight, 8, 10., 50.);
              FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Tight_Eta_"+IDsuffix, fabs(mu1.Eta()), weight, 5, 0., 2.5);
            }
          }
          else if(electrons_loose.size()==1){
            Electron ele1 = electrons_loose.at(0);
            if(!IsDATA){
              if(-4<=GetLeptonType(ele1, gens)&&GetLeptonType(ele1, gens)<=0) continue; // Note here the prompt lepton includes the internal/external conversions from the EW process
            }
            // Cutflow 1 : only 1 electron and at least 1 jet
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 0.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 0.5, 1., cutflow_bin, 0., cutflow_max);

            // pt-dependent trigger requirement
            if(IsDATA){
              if(DataYear==2016&&!isDoubleEG) continue;
              else if((DataYear==2017||DataYear==2018)&&!isSingleElectron) continue;
            }

            if(ele1.Pt() < 25.){
              if(!( ev.PassTrigger(ElectronTrigger12) )) continue;
              if(!IsDATA) weight = weight_common*ev.GetTriggerLumiByYear(ElectronTrigger12);
            }
            else if(25 <= ele1.Pt() && ele1.Pt() < 50.){
              if(!( ev.PassTrigger(ElectronTrigger23) )) continue;
              if(!IsDATA) weight = weight_common*ev.GetTriggerLumiByYear(ElectronTrigger23);
            }
            else continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 1.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 1.5, 1., cutflow_bin, 0., cutflow_max);

            // event selection
            if(!( ele1.DeltaR(j1) > 1. )) continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 2.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 2.5, 1., cutflow_bin, 0., cutflow_max);
            if(!( j1.Pt() > 35. )) continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 3.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 3.5, 1., cutflow_bin, 0., cutflow_max);
            if(!( MET < 20. )) continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 4.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 4.5, 1., cutflow_bin, 0., cutflow_max);
            Mt = MT(ele1, METv);
            if(!( Mt < 20. )) continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 5.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 5.5, 1., cutflow_bin, 0., cutflow_max);

            // fill histos
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Jet1_Pt_"+IDsuffix, j1.Pt(), weight, 8, 10., 50.);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Jet1_Eta_"+IDsuffix, j1.Eta(), weight, 20, -5., 5.);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Loose_"+IDsuffix, ele1.Pt(), fabs(ele1.Eta()), weight, 8, 10., 50., 5, 0., 2.5);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Loose_Pt_"+IDsuffix, ele1.Pt(), weight, 8, 10., 50.);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Loose_Eta_"+IDsuffix, fabs(ele1.Eta()), weight, 5, 0., 2.5);
            if(electrons_tight.size() > 0){
              if( (electrons_loose.at(0).Pt() != electrons_tight.at(0).Pt()) || (electrons_loose.at(0).Eta() != electrons_tight.at(0).Eta()) ){
                cout << "!!ERROR!!" << endl;
                cout << "loose lepton pt, eta : " << electrons_loose.at(0).Pt() << ", " << electrons_loose.at(0).Eta() << endl;
                cout << "tight lepton pt, eta : " << electrons_tight.at(0).Pt() << ", " << electrons_tight.at(0).Eta() << endl;
              }
              FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 6.5, weight, cutflow_bin, 0., cutflow_max);
              FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 6.5, 1., cutflow_bin, 0., cutflow_max);
              FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Tight_"+IDsuffix, ele1.Pt(), fabs(ele1.Eta()), weight, 8, 10., 50., 5, 0., 2.5);
              FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Tight_Pt_"+IDsuffix, ele1.Pt(), weight, 8, 10., 50.);
              FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Tight_Eta_"+IDsuffix, fabs(ele1.Eta()), weight, 5, 0., 2.5);
            }
          }
        }
        else if(regionsFake.at(it_rg).Contains("HN")){ // EXO-17-028 selection

          if(IDsuffix=="SSWW") continue;

          if(muons_loose.size()==1){ // muon FR
            Muon mu1 = muons_loose.at(0);
            if(!IsDATA){
              if(-4<=GetLeptonType(mu1, gens)&&GetLeptonType(mu1, gens)<=0) continue; // Note here the prompt lepton includes the internal/external conversions from the EW process
            }
            // Cutflow 1 : only 1 loose muon and at least 1 jets
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 0.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 0.5, 1., cutflow_bin, 0., cutflow_max);

            // pt-dependent trigger requirement
            if(IsDATA){
              if(mu1.PtCone() < 20.){
                if(DataYear==2016){
                  if(!( isDoubleMuon )) continue;
                }
                if(DataYear==2017||DataYear==2018){
                  if(!( isSingleMuon )) continue;
                }
              }
              else{
                if(!( isDoubleMuon )) continue;
              }
            }

            if(mu1.PtCone() < 20.){
              if(!( ev.PassTrigger(MuonTrigger3) )) continue;
              if(!IsDATA) weight = weight_common*ev.GetTriggerLumiByYear(MuonTrigger3);
            }
            else if(20 <= mu1.PtCone() && mu1.PtCone() < 30.){
              if(!( ev.PassTrigger(MuonTrigger8) )) continue;
              if(!IsDATA) weight = weight_common*ev.GetTriggerLumiByYear(MuonTrigger8);
            }
            else if(30 <= mu1.PtCone() && mu1.PtCone() < 9999.){
              if(!( ev.PassTrigger(MuonTrigger17) )) continue;
              if(!IsDATA) weight = weight_common*ev.GetTriggerLumiByYear(MuonTrigger17);
            }
            else continue;
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 1.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 1.5, 1., cutflow_bin, 0., cutflow_max);

            // event selection
            vector<Jet> awayjets;
            for(int i=0; i<jets.size(); i++){
              if(mu1.DeltaPhi(jets.at(i))>2.5) awayjets.push_back(jets.at(i));
            }
            if(!( awayjets.size() > 0 )) continue;
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 2.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 2.5, 1., cutflow_bin, 0., cutflow_max);
            j1 = awayjets.at(0);
            if(!( j1.Pt() > 40. )) continue;
            if(DataYear==2016&&mu1.PtCone() < 20.){
              if(!( j1.Pt() > 50. )) continue;
            }
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 3.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 3.5, 1., cutflow_bin, 0., cutflow_max);
            if(!( MET < 80. )) continue;
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 4.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 4.5, 1., cutflow_bin, 0., cutflow_max);
            Mt = MT(mu1, METv);
            if(!( Mt < 25. )) continue;
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 5.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 5.5, 1., cutflow_bin, 0., cutflow_max);
            if(!( j1.Pt()/mu1.Pt() > 1. )) continue;
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 6.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 6.5, 1., cutflow_bin, 0., cutflow_max);
            if(!( j1.ChargedEmEnergyFraction() < 0.65 )) continue;
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 7.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 7.5, 1., cutflow_bin, 0., cutflow_max);

            // fill histos
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Jet1_Pt_"+IDsuffix, j1.Pt(), weight, 18, 10., 100.);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Jet1_Eta_"+IDsuffix, j1.Eta(), weight, 20, -5., 5.);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Loose_"+IDsuffix, mu1.PtCone(), fabs(mu1.Eta()), weight, 10, 10., 60., 5, 0., 2.5);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Loose_PtCone_"+IDsuffix, mu1.PtCone(), weight, 10, 10., 60.);
            FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Loose_Eta_"+IDsuffix, fabs(mu1.Eta()), weight, 5, 0., 2.5);
            if(muons_tight.size() > 0){
              if( (muons_loose.at(0).Pt() != muons_tight.at(0).Pt()) || (muons_loose.at(0).Eta() != muons_tight.at(0).Eta()) ){
                cout << "!!ERROR!!" << endl;
                cout << "loose lepton pt, eta : " << muons_loose.at(0).Pt() << ", " << muons_loose.at(0).Eta() << endl;
                cout << "tight lepton pt, eta : " << muons_tight.at(0).Pt() << ", " << muons_tight.at(0).Eta() << endl;
              }
              FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 8.5, weight, cutflow_bin, 0., cutflow_max);
              FillHist("Muon/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 8.5, 1., cutflow_bin, 0., cutflow_max);
              FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Tight_"+IDsuffix, mu1.PtCone(), fabs(mu1.Eta()), weight, 10, 10., 60., 5, 0., 2.5);
              FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Tight_PtCone_"+IDsuffix, mu1.PtCone(), weight, 10, 10., 60.);
              FillHist("Muon/"+regionsFake.at(it_rg)+"/Muon_Tight_Eta_"+IDsuffix, fabs(mu1.Eta()), weight, 5, 0., 2.5);
            }
          }
          if(electrons_loose.size()==1){ // electron FR
            Electron ele1 = electrons_loose.at(0);
            if(!IsDATA){
              if(-4<=GetLeptonType(ele1, gens)&&GetLeptonType(ele1, gens)<=0) continue; // Note here the prompt lepton includes the internal/external conversions from the EW process
            }
            // Cutflow 1 : only 1 loose electron and at least 1 jet
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 0.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 0.5, 1., cutflow_bin, 0., cutflow_max);

            // pt-dependent trigger requirement
            if(IsDATA){
              if(DataYear==2016&&!isDoubleEG) continue;
              else if((DataYear==2017||DataYear==2018)&&!isSingleElectron) continue;
            }

            if(ele1.PtCone() < 23.){
              if(!( ev.PassTrigger(ElectronTrigger8) )) continue;
              if(!IsDATA) weight = weight_common*ev.GetTriggerLumiByYear(ElectronTrigger8);
            }
            else if(23. <= ele1.PtCone() && ele1.PtCone() < 35.){
              if(!( ev.PassTrigger(ElectronTrigger12) )) continue;
              if(!IsDATA) weight = weight_common*ev.GetTriggerLumiByYear(ElectronTrigger12);
            }
            else if(35. <= ele1.PtCone() && ele1.PtCone() < 45.){
              if(!( ev.PassTrigger(ElectronTrigger17) )) continue;
              if(!IsDATA) weight = weight_common*ev.GetTriggerLumiByYear(ElectronTrigger17);
            }
            else if(45. <= ele1.PtCone() && ele1.PtCone() < 9999.){
              if(!( ev.PassTrigger(ElectronTrigger23) )) continue;
              if(!IsDATA) weight = weight_common*ev.GetTriggerLumiByYear(ElectronTrigger23);
            }
            else continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 1.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 1.5, 1., cutflow_bin, 0., cutflow_max);

            // event selection
            vector<Jet> awayjets;
            for(int i=0; i<jets.size(); i++){
              if(ele1.DeltaPhi(jets.at(i))>2.5) awayjets.push_back(jets.at(i));
            }
            if(!( awayjets.size() > 0 )) continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 2.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 2.5, 1., cutflow_bin, 0., cutflow_max);
            j1 = awayjets.at(0);
            if(!( j1.Pt() > 40. )) continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 3.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 3.5, 1., cutflow_bin, 0., cutflow_max);
            if(!( MET < 80. )) continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 4.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 4.5, 1., cutflow_bin, 0., cutflow_max);
            Mt = MT(ele1, METv);
            if(!( Mt < 25. )) continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 5.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 5.5, 1., cutflow_bin, 0., cutflow_max);
            if(!( j1.Pt()/ele1.Pt() > 1. )) continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 6.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 6.5, 1., cutflow_bin, 0., cutflow_max);
            if(!( j1.ChargedEmEnergyFraction() < 0.65 )) continue;
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 7.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 7.5, 1., cutflow_bin, 0., cutflow_max);

            // fill histos
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Jet1_Pt_"+IDsuffix, j1.Pt(), weight, 18, 10., 100.);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Jet1_Eta_"+IDsuffix, j1.Eta(), weight, 20, -5., 5.);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Loose_"+IDsuffix, ele1.PtCone(), fabs(ele1.Eta()), weight, 10, 10., 60., 5, 0., 2.5);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Loose_PtCone_"+IDsuffix, ele1.PtCone(), weight, 10, 10., 60.);
            FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Loose_Eta_"+IDsuffix, fabs(ele1.Eta()), weight, 5, 0., 2.5);
            if(electrons_tight.size() > 0){
              if( (electrons_loose.at(0).Pt() != electrons_tight.at(0).Pt()) || (electrons_loose.at(0).Eta() != electrons_tight.at(0).Eta()) ){
                cout << "!!ERROR!!" << endl;
                cout << "loose lepton pt, eta : " << electrons_loose.at(0).Pt() << ", " << electrons_loose.at(0).Eta() << endl;
                cout << "tight lepton pt, eta : " << electrons_tight.at(0).Pt() << ", " << electrons_tight.at(0).Eta() << endl;
              }
              FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_"+IDsuffix, 8.5, weight, cutflow_bin, 0., cutflow_max);
              FillHist("Electron/"+regionsFake.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 8.5, 1., cutflow_bin, 0., cutflow_max);
              FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Tight_"+IDsuffix, ele1.PtCone(), fabs(ele1.Eta()), weight, 10, 10., 60., 5, 0., 2.5);
              FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Tight_PtCone_"+IDsuffix, ele1.PtCone(), weight, 10, 10., 60.);
              FillHist("Electron/"+regionsFake.at(it_rg)+"/Electron_Tight_Eta_"+IDsuffix, fabs(ele1.Eta()), weight, 5, 0., 2.5);
            }
          }
        }
      }
    }
  }

  if(HasFlag("Norm")){

    METv = UpdateMETMuon(METv, muons_tight);
    METv = UpdateMETElectron(METv, electrons_tight);
    double MET = METv.Pt(); // MET propagated

    //if(IDsuffix=="HN"&&Nentry%1000==0) cout << "N tight muons : " << muons_tight.size() << ", N tight electrons : " << electrons_tight.size() << endl;

    for(unsigned int it_rg=0; it_rg<regionsNorm.size(); it_rg++){

      if(IDsuffix=="SSWW") continue; // I don't use SSWW for now

      if(it_rg==0){ // Z_Muon
        if(!( muons_tight.size()==2&&electrons_tight.size()==0 )) continue;
        ZCand = muons_tight.at(0)+muons_tight.at(1);
        if(!( IsOnZ(ZCand.M(),20.) )) continue;
        if(!( (muons_tight.at(0).Pt()>MuonPtCut1 && muons_tight.at(1).Pt()>MuonPtCut2) )) continue;
        for(unsigned int i=0; i<MuonTriggers.size(); i++){
          if(IsDATA){
            //if(Nentry%1000==0) cout << "Trigger name : " << MuonTriggers.at(i) << ", DataStream : " << DataStream << ", IsPDForTrigger : " << ev.IsPDForTrigger(MuonTriggers.at(i),DataStream) << endl;
            if(ev.IsPDForTrigger(MuonTriggers.at(i),DataStream)){
              if(ev.PassTrigger(MuonTriggers.at(i))) FillLeptonPlots(leptons_tight, regionsNorm.at(it_rg)+"_"+MuonTriggers.at(i)+"_"+IDsuffix, weight);
            }
            else continue;
          }
          else{
            if(ev.PassTrigger(MuonTriggers.at(i))){
              //if(Nentry%1000==0) cout << "Trigger name : " << MuonTriggers.at(i) << ", Lumi : " << ev.GetTriggerLumiByYear(MuonTriggers.at(i)) << endl;
              weight = weight_common*ev.GetTriggerLumiByYear(MuonTriggers.at(i));
              FillLeptonPlots(leptons_tight, regionsNorm.at(it_rg)+"_"+MuonTriggers.at(i)+"_"+IDsuffix, weight);
            }
          }
        }
      }
      else if(it_rg==1){ // Z_Electron
        if(!( muons_tight.size()==0&&electrons_tight.size()==2 )) continue;
        ZCand = electrons_tight.at(0)+electrons_tight.at(1);
        if(!( IsOnZ(ZCand.M(),20.) )) continue;
        if(!( (electrons_tight.at(0).Pt()>ElectronPtCut1 && electrons_tight.at(1).Pt()>ElectronPtCut2) )) continue;
        for(unsigned int i=0; i<ElectronTriggers.size(); i++){
          if(IsDATA){
            if(ev.IsPDForTrigger(ElectronTriggers.at(i),DataStream)){
              if(ev.PassTrigger(ElectronTriggers.at(i))) FillLeptonPlots(leptons_tight, regionsNorm.at(it_rg)+"_"+ElectronTriggers.at(i)+"_"+IDsuffix, weight);
            }
            else continue;
          }
          else{
            if(ev.PassTrigger(ElectronTriggers.at(i))){
              weight = weight_common*ev.GetTriggerLumiByYear(ElectronTriggers.at(i));
              FillLeptonPlots(leptons_tight, regionsNorm.at(it_rg)+"_"+ElectronTriggers.at(i)+"_"+IDsuffix, weight);
            }
          }
        }
      }
      else if(it_rg==2){ // W_Muon
        if(!( muons_tight.size()==1&&electrons_tight.size()==0 )) continue;
        Mt = MT(muons_tight.at(0), METv);
        if(!( MET > 40. )) continue;
        if(!( (60.<Mt && Mt<100.) )) continue;
        if(!( muons_tight.at(0).Pt()>MuonPtCut1 )) continue;
        for(unsigned int i=0; i<MuonTriggers.size(); i++){
          if(IsDATA){
            if(ev.IsPDForTrigger(MuonTriggers.at(i),DataStream)){
              if(ev.PassTrigger(MuonTriggers.at(i))) FillLeptonPlots(leptons_tight, regionsNorm.at(it_rg)+"_"+MuonTriggers.at(i)+"_"+IDsuffix, weight);
            }
            else continue;
          }
          else{
            if(ev.PassTrigger(MuonTriggers.at(i))){
              weight = weight_common*ev.GetTriggerLumiByYear(MuonTriggers.at(i));
              FillLeptonPlots(leptons_tight, regionsNorm.at(it_rg)+"_"+MuonTriggers.at(i)+"_"+IDsuffix, weight);
            }
          }
        }
      }
      else if(it_rg==3){ // W_Electron
        if(!( muons_tight.size()==0&&electrons_tight.size()==1 )) continue;
        Mt = MT(electrons_tight.at(0), METv);
        if(!( MET > 40. )) continue;
        if(!( (60.<Mt && Mt<100.) )) continue;
        if(!( electrons_tight.at(0).Pt()>ElectronPtCut1 )) continue;
        for(unsigned int i=0; i<ElectronTriggers.size(); i++){
          if(IsDATA){
            if(ev.IsPDForTrigger(ElectronTriggers.at(i),DataStream)){
              if(ev.PassTrigger(ElectronTriggers.at(i))) FillLeptonPlots(leptons_tight, regionsNorm.at(it_rg)+"_"+ElectronTriggers.at(i)+"_"+IDsuffix, weight);
            }
            else continue;
          }
          else{
            if(ev.PassTrigger(ElectronTriggers.at(i))){
              weight = weight_common*ev.GetTriggerLumiByYear(ElectronTriggers.at(i));
              FillLeptonPlots(leptons_tight, regionsNorm.at(it_rg)+"_"+ElectronTriggers.at(i)+"_"+IDsuffix, weight);
            }
          }
        }
      }
    }
  }

  if(HasFlag("PR")){


  }


}
