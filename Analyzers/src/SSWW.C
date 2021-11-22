#include "SSWW.h"

SSWW::SSWW(){

}

void SSWW::initializeAnalyzer(){

  //==== if you use "--userflags RunSyst" with SKFlat.py, HasFlag("RunSyst") will return "true"
//  RunSyst = HasFlag("RunSyst");
//  cout << "[SSWW::initializeAnalyzer] RunSyst = " << RunSyst << endl;
  RunFake = HasFlag("RunFake");
  RunCF = HasFlag("RunCF");

  MuonTightIDs = {"SSWW_tight","HNTightV1"};
  MuonLooseIDs = {"SSWW_loose","HNLooseV1"};
  MuonVetoIDs  = {"ISRVeto","ISRVeto"};
  if(DataYear==2016){
    ElectronTightIDs = {"SSWW_tight2016","HNTightV1"};
    ElectronLooseIDs = {"SSWW_loose2016","HNLooseV1"};
  }
  else{
    ElectronTightIDs = {"SSWW_tight","HNTightV1"};
    ElectronLooseIDs = {"SSWW_loose","HNLooseV1"};
  }
  ElectronVetoIDs  = {"ISRVeto","ISRVeto"};
  FakeRateIDs = {"SSWW","HN"}; //JH : NOTE This is used in fakeEst->ReadHistograms() in m.initializeAnalyzerTools() 

  //==== At this point, sample informations (e.g., IsDATA, DataStream, MCSample, or DataYear) are all set
  //==== You can define sample-dependent or year-dependent variables here
  //==== (Example) Year-dependent variables
  //==== I defined "TString IsoMuTriggerName;" and "double TriggerSafePtCut;" in Analyzers/include/SSWW.h 
  //==== IsoMuTriggerName is a year-dependent variable, and you don't want to do "if(Dataer==~~)" for every event (let's save cpu time).
  //==== Then, do it here, which only ran once for each macro

  MuonTriggers.clear();
  MuonTriggersH.clear();
  MuonTriggersHighPt.clear();
  ElectronTriggers.clear();
  EMuTriggers.clear();
  EMuTriggersH.clear();

  if(DataYear==2016){                                                                   // Lumi values for trigger weight (/pb)
    MuonTriggersBtoG.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v");                   // 27267.591112919 
    MuonTriggersBtoG.push_back("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v");                 // 27267.591112919 //JH : NOTE these two are prescaled at 2016H -> https://its.cern.ch/jira/browse/CMSHLT-1002
    MuonTriggers.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v");                       // 35918.219492947
    MuonTriggers.push_back("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v");                     // 35918.219492947 
    //MuonTriggers.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v");                    // 35918.219492947
    //MuonTriggers.push_back("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v");                  // 35918.219492947 
    MuonTriggersH.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v");                   // 8650.628380028
    MuonTriggersH.push_back("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v");                 // 8650.628380028
    MuonTriggersHighPt.push_back("HLT_Mu50_v");                    
    MuonTriggersHighPt.push_back("HLT_TkMu50_v");                  //JH : to see how many events are cut with high pt
    ElectronTriggers.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");          // 35918.219492947
    EMuTriggersBtoG.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v");      // 27267.591112919
    EMuTriggersBtoG.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v");     // 27267.591112919
    EMuTriggers.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v");          // 35918.219492947
    EMuTriggers.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v");         // 35918.219492947
    EMuTriggers.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");       // 35918.219492947
    EMuTriggers.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");      // 35918.219492947
    EMuTriggersH.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");      // 8650.628380028
    EMuTriggersH.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");     // 8650.628380028
    MuonPtCut1 = 20., MuonPtCut2 = 10.;
    ElectronPtCut1 = 25., ElectronPtCut2 = 15.;
    EMuPtCut1 = 25., EMuPtCut2 = 15.; 
  }
  else if(DataYear==2017){
    MuonTriggers.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v");
    ElectronTriggers.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v");
    EMuTriggers.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");
    EMuTriggers.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
    MuonPtCut1 = 20., MuonPtCut2 = 10.;
    ElectronPtCut1 = 25., ElectronPtCut2 = 15.;
    EMuPtCut1 = 25., EMuPtCut2 = 15.;
  }
  else if(DataYear==2018){
    MuonTriggers.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v");
    ElectronTriggers.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v");
    EMuTriggers.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");
    EMuTriggers.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
    MuonPtCut1 = 20., MuonPtCut2 = 10.;
    ElectronPtCut1 = 25., ElectronPtCut2 = 15.;
    EMuPtCut1 = 25., EMuPtCut2 = 15.;

    ElectronTightIDs.pop_back(); ElectronTightIDs.push_back("HEEP2018_dZ"); //JH 
  }

//  cout << "[SSWW::initializeAnalyzer] IsoMuTriggerName = " << IsoMuTriggerName << endl;
//  cout << "[SSWW::initializeAnalyzer TriggerSafePtCut = " << TriggerSafePtCut << endl;

  //==== B-Tagging
  //==== add taggers and WP that you want to use in analysis
  std::vector<JetTagging::Parameters> jtps;
  //==== If you want to use 1a or 2a method,
  jtps.push_back( JetTagging::Parameters(JetTagging::DeepCSV, JetTagging::Loose, JetTagging::incl, JetTagging::comb) );
  jtps.push_back( JetTagging::Parameters(JetTagging::DeepCSV, JetTagging::Medium, JetTagging::incl, JetTagging::comb) );
  //==== set
  mcCorr->SetJetTaggingParameters(jtps); //JH : NOTE This is used in mcCorr->SetupJetTagging() in m.initializeAnalyzerTools();
}

SSWW::~SSWW(){

  //==== Destructor of this Analyzer

}

void SSWW::executeEvent(){

  //================================================================
  //====  Example 1
  //====  Dimuon Z-peak events with two muon IDs, with systematics
  //================================================================

  //==== *IMPORTANT TO SAVE CPU TIME*
  //==== Every GetMuon() funtion first collect ALL MINIAOD muons with GetAllMuons(),
  //==== and then check ID booleans.
  //==== GetAllMuons not only loops over all MINIAOD muons, but also actually CONSTRUCT muon objects for each muons.
  //==== We are now running systematics, and you don't want to do this for every systematic sources
  //==== So, I defined "vector<Muon> AllMuons;" in Analyzers/include/SSWW.h,
  //==== and save muons objects at the very beginning of executeEvent().
  //==== Later, do "SelectMuons(AllMuons, ID, pt, eta)" to get muons with ID cuts
  AllMuons = GetAllMuons();
  AllElectrons = GetAllElectrons();
  AllJets = GetAllJets();
//  AllFatJets = GetAllFatJets();
  AllFatJets = puppiCorr->Correct(GetAllFatJets()); //JH : puppiCorr = new PuppiSoftdropMassCorr(); in the constructor of AnalyzerCore.C; apply correction to fatjet.SDMass(); the total weight = gen correction * reco correction, from SKFlatAnalyzer/data/Run2Legacy_v4/DataYear/PuppiSoftdropMassCorr/puppiCorr.root

  //==== Get L1Prefire reweight
  //==== If data, 1.;
  //==== If MC && DataYear > 2017, 1.;
  //==== If MC && DataYear <= 2017, we have to reweight the event with this value
  //==== I defined "double weight_Prefire;" in Analyzers/include/SSWW.h
//  weight_Prefire = GetPrefireWeight(0);

  AnalyzerParameter param;

  for(unsigned int it_id=0; it_id<ElectronTightIDs.size(); it_id++){
    TString MuonTightID = MuonTightIDs.at(it_id);
    TString MuonLooseID = MuonLooseIDs.at(it_id); 
    TString MuonVetoID  = MuonVetoIDs.at(it_id);
    TString ElectronTightID = ElectronTightIDs.at(it_id);
    TString ElectronLooseID = ElectronLooseIDs.at(it_id);
    TString ElectronVetoID  = ElectronVetoIDs.at(it_id);
    TString FakeRateID = FakeRateIDs.at(it_id);

    param.Clear();

    param.syst_ = AnalyzerParameter::Central;

//    param.Name = MuonID+"_"+"Central";

    // Muon ID
    param.Muon_Tight_ID = MuonTightID;
    param.Muon_Loose_ID = MuonLooseID;
    param.Muon_Veto_ID  = MuonVetoID;
    param.Muon_FR_ID = FakeRateID;     // ID name in histmap_Muon.txt
    param.Muon_FR_Key = "FR_2D"; // histname
    param.Muon_ID_SF_Key = "NUM_TightID_DEN_genTracks";
    param.Muon_ISO_SF_Key = "NUM_TightRelIso_DEN_TightIDandIPCut";
    param.Muon_Trigger_SF_Key = "";
    if(param.Muon_Tight_ID.Contains("SSWW")){
      param.Muon_UsePtCone = false;
    }
    else if(param.Muon_Tight_ID.Contains("HN")){
      param.Muon_UsePtCone = true;
    }

    // Electron ID
    param.Electron_Tight_ID = ElectronTightID;
    param.Electron_Loose_ID = ElectronLooseID;
    param.Electron_Veto_ID  = ElectronVetoID;
    param.Electron_FR_ID = FakeRateID;     // ID name in histmap_Electron.txt
    param.Electron_FR_Key = "FR_2D"; // histname
    param.Electron_ID_SF_Key = "passTightID";
    param.Electron_Trigger_SF_Key = "";
    if(param.Electron_Tight_ID.Contains("SSWW")){
      param.Electron_UsePtCone = false;
    }
    else if(param.Electron_Tight_ID.Contains("HN")){
      param.Electron_UsePtCone = true;
    }

    // Jet ID
    param.Jet_ID = "HNTight"; //JH : pass POG tight + pass loose PU ID
    //param.FatJet_ID = "HNTight";
    if(DataYear==2016) param.FatJet_ID = "HNTight0p55";
    else param.FatJet_ID = "HNTight0p45"; //JH : TODO

    executeEventFromParameter(param);

/*  if(RunSyst){
      for(int it_syst=1; it_syst<AnalyzerParameter::NSyst; it_syst++){
        param.syst_ = AnalyzerParameter::Syst(it_syst);
        param.Name = MuonID+"_"+"Syst_"+param.GetSystType();
        executeEventFromParameter(param);
      }
    }*/
  }
}

void SSWW::executeEventFromParameter(AnalyzerParameter param){

  vector<TString> regionsTypeI = {"highSR1", "highSR1_1jet", "highSR2"}; 
  vector<TString> regionsSSWW = {"SR", "SR_inv", "btag", "WZ", "WZb"}; 
  vector<TString> channels = {"dimu", "diel", "emu"};
  vector<TString> regionsSM = {"DYmm", "DYee", "DYemu", "TTmm", "TTee", "TTemu", "WZ", "ZG", "WG", "ZZ"}; 
  vector<TString> channels3L = {"mmm", "mme", "mee", "eee"}; //JH : iterate for the number of e
  vector<TString> channels4L = {"mmmm", "mmee", "eeee"}; //JH : iterate for the number of e / 2
  TString IDsuffix = "";
  if(param.Muon_Tight_ID.Contains("SSWW")) IDsuffix = "SSWWrep";
  else if(param.Muon_Tight_ID.Contains("HN")) IDsuffix = "HN";
  TString LepCategory = "TT";
  double cutflow_max = 20.;
  int cutflow_bin = 20;
  double weight = 1.;
  double trigger_lumi = 1., dimu_trig_weight = 0., emu_trig_weight = 0.;
  double muon_miniaodP = 0.;
 
  Event ev = GetEvent();

  bool isDoubleMuon = false, isDoubleEG = false, isMuonEG = false;
  if(IsDATA){
    if(DataStream.Contains("DoubleMuon")) isDoubleMuon = true;
    if(DataStream.Contains("DoubleEG") || DataStream.Contains("EGamma")) isDoubleEG = true;
    if(DataStream.Contains("MuonEG")) isMuonEG = true;
  }

  //=============
  //==== No Cut
  //=============

  if(!IsDATA){
    weight *= weight_norm_1invpb*ev.GetTriggerLumiByYear("Full"); //JH : weight_norm_1invpb = xsec/sumW; Lumi = 35.9, 41.5, 59.7(fb-1) total 137fb-1
    weight *= ev.MCweight(); //JH : gen_weight in MiniAOD
    weight *= GetPrefireWeight(0); //JH : No issue in 2018, otherwise returns L1PrefireReweight_Central in MiniAOD
    weight *= GetPileUpWeight(nPileUp,0); //JH : mcCorr->GetPileUpWeight(N_pileup, syst); mcCorr->GetPileUpWeight2017(N_pileup, syst); NOTE 2018 not yet added.
  } //JH : total weight calculation done.

  // Cutflow 1 : No Cuts
  for(unsigned int it_rg=0; it_rg<regionsSSWW.size(); it_rg++){
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 0.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 0.5, 1., cutflow_bin, 0., cutflow_max);
  }
  for(unsigned int it_rg=0; it_rg<regionsTypeI.size(); it_rg++){
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 0.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 0.5, 1., cutflow_bin, 0., cutflow_max);
  }
  //for(unsigned int it_rg2=0; it_rg2<regionsSM.size(); it_rg2++){
  //  FillHist(regionsSM.at(it_rg2)+"/Number_Events_"+IDsuffix, 0.5, weight, cutflow_bin, 0., cutflow_max);
  //  FillHist(regionsSM.at(it_rg2)+"/Number_Events_unweighted_"+IDsuffix, 0.5, 1., cutflow_bin, 0., cutflow_max);
  //}

  //========================
  //==== MET Filter
  //========================

  if(!PassMETFilter()) return;

  // Cutflow 2 : MET filter
  for(unsigned int it_rg=0; it_rg<regionsSSWW.size(); it_rg++){
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 1.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 1.5, 1., cutflow_bin, 0., cutflow_max);
  }
  for(unsigned int it_rg=0; it_rg<regionsTypeI.size(); it_rg++){
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 1.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 1.5, 1., cutflow_bin, 0., cutflow_max);
  }
  //for(unsigned int it_rg2=0; it_rg2<regionsSM.size(); it_rg2++){
  //  FillHist(regionsSM.at(it_rg2)+"/Number_Events_"+IDsuffix, 1.5, weight, cutflow_bin, 0., cutflow_max);
  //  FillHist(regionsSM.at(it_rg2)+"/Number_Events_unweighted_"+IDsuffix, 1.5, 1., cutflow_bin, 0., cutflow_max);
  //}

  //==============
  //==== Trigger
  //==============

  //if(!(ev.PassTrigger(MuonTriggers) || ev.PassTrigger(MuonTriggersHighPt) || ev.PassTrigger(ElectronTriggers) || ev.PassTrigger(EMuTriggers))) return; //JH : I don't think this is necessary
  if(!(ev.PassTrigger(MuonTriggers))) return;

  // Cutflow 3 : Double muon trigger
  for(unsigned int it_rg=0; it_rg<regionsSSWW.size(); it_rg++){
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 2.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 2.5, 1., cutflow_bin, 0., cutflow_max);
  }
  for(unsigned int it_rg=0; it_rg<regionsTypeI.size(); it_rg++){
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 2.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 2.5, 1., cutflow_bin, 0., cutflow_max);
  }

  //======================
  //==== Copy AllObjects
  //======================

  vector<Electron> this_AllElectrons = AllElectrons;
  vector<Muon> this_AllMuons;
  if(param.Muon_Tight_ID.Contains("HighPt")) this_AllMuons = UseTunePMuon(AllMuons);
  else this_AllMuons = AllMuons;
  vector<Jet> this_AllJets = AllJets;
  FillHist("Number_AllJets_"+IDsuffix, this_AllJets.size(), weight, 10, 0, 10);
  FillHist("Number_AllJets_unweighted_"+IDsuffix, this_AllJets.size(), 1., 10, 0, 10);
  vector<FatJet> this_AllFatJets = AllFatJets;
  FillHist("Number_AllFatJets_"+IDsuffix, this_AllFatJets.size(), weight, 10, 0, 10);
  FillHist("Number_AllFatJets_unweighted_"+IDsuffix, this_AllFatJets.size(), 1., 10, 0, 10);
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
    cout << "[SSWW::executeEventFromParameter] Wrong syst" << endl;
    exit(EXIT_FAILURE);
  }*/

  //==================================================
  //==== Then, apply ID selections using this_AllXXX
  //==================================================

  TString MuonID = param.Muon_Tight_ID;
  TString ElectronID = param.Electron_Tight_ID;
  if(RunFake){
    MuonID = param.Muon_Loose_ID;
    ElectronID = param.Electron_Loose_ID;
  }

  vector<Muon> muons = SelectMuons(this_AllMuons, MuonID, 10., 2.4);
  vector<Muon> muons_veto = SelectMuons(this_AllMuons, param.Muon_Veto_ID, 5., 2.4);
  vector<Electron> electrons = SelectElectrons(this_AllElectrons, ElectronID, 10., 2.5);
  vector<Electron> electrons_veto = SelectElectrons(this_AllElectrons, param.Electron_Loose_ID, 10., 2.5); //JH : lepton selection done
  //vector<Jet> jets_nolepveto = SelectJets(this_AllJets, param.Jet_ID, 20., 2.7); //JH : to reject bjets
  vector<Jet> jets_bcand = SelectJets(this_AllJets, param.Jet_ID, 20., 2.4); //JH : to reject bjets
//  vector<FatJet> fatjets = SelectFatJets(this_AllFatJets, param.FatJet_ID, 200., 2.7);

//  FillHist("Njet_"+IDsuffix, jets_nolepveto.size(), weight, 8, 0., 8.);


  // Jet, FatJet selection to avoid double counting due to jets matched geometrically with a lepton
  vector<Jet> jets;
  vector<FatJet> fatjets;
  jets.clear();
  fatjets.clear();
  int lepton_count1 = 0, lepton_count2 = 0, fatjet_count = 0, jet_count = 0; 

  // Fatjet selection in CATanalyzer (see the links)
  // https://github.com/jedori0228/LQanalyzer/blob/CatAnalyzer_13TeV_v8-0-7.36_HNAnalyzer/CATConfig/SelectionConfig/user_fatjets.sel
  // https://github.com/jedori0228/LQanalyzer/blob/CatAnalyzer_13TeV_v8-0-7.36_HNAnalyzer/LQCore/Selection/src/FatJetSelection.cc#L113-L124

  if(HasFlag("jcln_inv")){
    for(unsigned int i=0; i<this_AllFatJets.size(); i++){
      lepton_count1 = 0;
      if(!(this_AllFatJets.at(i).PassID(param.FatJet_ID))) continue; //JH : "HNTight"
      if(!(this_AllFatJets.at(i).Pt() > 200.)) continue;
      if(!(fabs(this_AllFatJets.at(i).Eta()) < 2.7)) continue;
      for(unsigned int j=0; j<muons.size(); j++){
        if(this_AllFatJets.at(i).DeltaR(muons.at(j)) < 0.8) lepton_count1++; //JH : tight muon cleaning
      }
      for(unsigned int j=0; j<electrons.size(); j++){
        if(this_AllFatJets.at(i).DeltaR(electrons.at(j)) < 0.8) lepton_count1++; //JH : tight electron cleaning
      } 
      if(lepton_count1 > 0) continue;
      fatjets.push_back(this_AllFatJets.at(i));
    }
    for(unsigned int i=0; i<this_AllJets.size(); i++){
      lepton_count2 = 0, fatjet_count = 0;
      if(!(this_AllJets.at(i).PassID(param.Jet_ID))) continue; //JH :"HNTight"
      if(!(this_AllJets.at(i).Pt() > 15.)) continue;
      if(!(fabs(this_AllJets.at(i).Eta()) < 4.7)) continue;
      for(unsigned int j=0; j<muons.size(); j++){
        if(this_AllJets.at(i).DeltaR(muons.at(j)) < 0.3) lepton_count2++; //JH : tight muon cleaning
      }
      for(unsigned int j=0; j<electrons.size(); j++){
        if(this_AllJets.at(i).DeltaR(electrons.at(j)) < 0.3) lepton_count2++; //JH : tight electron cleaning
      }
      for(unsigned int j=0; j<fatjets.size(); j++){
        if(this_AllJets.at(i).DeltaR(fatjets.at(j)) < 0.8) fatjet_count++; //JH : fatjet cleaning
      }
      if(lepton_count2 > 0) continue;
      if(fatjet_count > 0) continue;
      jets.push_back(this_AllJets.at(i));
    }
  }
  else{
    for(unsigned int i=0; i<this_AllJets.size(); i++){
      lepton_count2 = 0, fatjet_count = 0;
      if(!(this_AllJets.at(i).PassID(param.Jet_ID))) continue; //JH :"HNTight"
      if(!(this_AllJets.at(i).Pt() > 15.)) continue;
      if(!(fabs(this_AllJets.at(i).Eta()) < 4.7)) continue;
      for(unsigned int j=0; j<muons.size(); j++){
        if(this_AllJets.at(i).DeltaR(muons.at(j)) < 0.3) lepton_count2++; //JH : tight muon cleaning
      }
      for(unsigned int j=0; j<electrons.size(); j++){
        if(this_AllJets.at(i).DeltaR(electrons.at(j)) < 0.3) lepton_count2++; //JH : tight electron cleaning
      }
      //for(unsigned int j=0; j<fatjets.size(); j++){
      //  if(this_AllJets.at(i).DeltaR(fatjets.at(j)) < 0.8) fatjet_count++; //JH : fatjet cleaning
      //}
      if(lepton_count2 > 0) continue;
      //if(fatjet_count > 0) continue;
      jets.push_back(this_AllJets.at(i));
    }
    for(unsigned int i=0; i<this_AllFatJets.size(); i++){
      lepton_count1 = 0, jet_count = 0;
      if(!(this_AllFatJets.at(i).PassID(param.FatJet_ID))) continue; //JH : "HNTight"
      if(!(this_AllFatJets.at(i).Pt() > 200.)) continue;
      if(!(fabs(this_AllFatJets.at(i).Eta()) < 2.7)) continue;
      for(unsigned int j=0; j<muons.size(); j++){
        if(this_AllFatJets.at(i).DeltaR(muons.at(j)) < 0.8) lepton_count1++; //JH : tight muon cleaning
      }
      for(unsigned int j=0; j<electrons.size(); j++){
        if(this_AllFatJets.at(i).DeltaR(electrons.at(j)) < 0.8) lepton_count1++; //JH : tight electron cleaning
      } 
      for(unsigned int j=0; j<jets.size(); j++){
        if(this_AllFatJets.at(i).DeltaR(jets.at(j)) < 0.8) jet_count++; //JH : jet cleaning
      }
      if(lepton_count1 > 0) continue;
      if(jet_count > 0) continue;
      fatjets.push_back(this_AllFatJets.at(i));
    }
  }
  FillHist("Number_JetsSel_"+IDsuffix, jets.size(), weight, 10, 0, 10);
  FillHist("Number_JetsSel_unweighted_"+IDsuffix, jets.size(), 1., 10, 0, 10);
  FillHist("Number_FatJetsSel_"+IDsuffix, fatjets.size(), weight, 10, 0, 10);
  FillHist("Number_FatJetsSel_unweighted_"+IDsuffix, fatjets.size(), 1., 10, 0, 10);
//JH : jet, fatjet selection done.

//  FillHist("Nfatjet_hn_"+IDsuffix, fatjets.size(), weight, 5, 0., 5.);
//  FillHist("Njet_hn_"+IDsuffix, jets.size(), weight, 8, 0., 8.); 

  std::vector<Lepton*> leptons, leptons_minus, leptons_plus, leptons_veto;

  //=======================
  //==== Sort in pt-order
  //=======================

  std::sort(muons.begin(), muons.end(), PtComparing);
  std::sort(muons_veto.begin(), muons_veto.end(), PtComparing);
  std::sort(electrons.begin(), electrons.end(), PtComparing);
  std::sort(electrons_veto.begin(), electrons_veto.end(), PtComparing);
  std::sort(jets.begin(), jets.end(), PtComparing);
  //std::sort(jets_nolepveto.begin(), jets_nolepveto.end(), PtComparing);
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
  METv = UpdateMETMuon(METv, muons);
  METv = UpdateMETElectron(METv, electrons);
  double MET = METv.Pt(); // JH : MET propagated

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
  Particle llj, l1j, l2j,  lljj, l1jj, l2jj, l1J, l2J;
  Particle WtagLep, DiLep, DiJet, TriLep, ZtagLep1, ZtagLep2, Ztemp, Ztemp1, Ztemp2, Ztemp3, Ztemp4, ZCand1, ZCand2, GammaCand, GammaLep1, GammaLep2;
  int ossf_mass10 = 0;
  
  // Set tight_iso cut & calculate pTcone
  double mu_tight_iso = 0.07;
  //if(IDsuffix == "HNV2") mu_tight_iso = 0.1;
  if(IDsuffix == "HN16") mu_tight_iso = 0.07;

  double el_tight_iso = 0.;
  double this_ptcone_muon = 0., this_ptcone_electron = 0.;

  // Set pTcone
  for(unsigned int i=0; i<muons.size(); i++){
    this_ptcone_muon = muons.at(i).CalcPtCone(muons.at(i).RelIso(), mu_tight_iso); //JH : CalcPtCone() in Lepton.h; this returns (i) pt for more tightly isolated leptons than the tight_iso, or (ii) pt + pt*(RelIso-tight_iso) which is the proxy for the mother parton's pt -> used for fake estimation
    muons.at(i).SetPtCone(this_ptcone_muon);
  }
   
  for(unsigned int i=0; i<electrons.size(); i++){
    //el_tight_iso = 0.0287+0.506/electrons.at(i).UncorrPt(); //JH : TODO electron uses UncorrPt() but I don't understand the meaning yet
    //if(fabs(electrons.at(i).scEta()) > 1.479) el_tight_iso = 0.0445+0.963/electrons.at(i).UncorrPt();
    //if(IDsuffix == "HNV2"){
    //  el_tight_iso = std::min(0.08, 0.0287+0.506/electrons.at(i).UncorrPt());
    //  if(fabs(electrons.at(i).scEta()) > 1.479) el_tight_iso = std::min(0.08, 0.0445+0.963/electrons.at(i).UncorrPt());
    //} 
    if(IDsuffix == "HN16") el_tight_iso = 0.08;

    if(param.Electron_Tight_ID.Contains("HNTight")){ // POG cut-based tight WP
      el_tight_iso = 0.0287+0.506/electrons.at(i).UncorrPt();
      if(fabs(electrons.at(i).scEta()) > 1.479) el_tight_iso = 0.0445+0.963/electrons.at(i).UncorrPt();
    }

    this_ptcone_electron = electrons.at(i).CalcPtCone(electrons.at(i).RelIso(), el_tight_iso);
    electrons.at(i).SetPtCone(this_ptcone_electron);
  }

  //if(muons.size()==2 && electrons.size()==0){
  //  FillHist("Pt_muon1", muons.at(0).Pt(), weight, 1000, 0., 1000.);
  //  FillHist("Pt_muon2", muons.at(1).Pt(), weight, 1000, 0., 1000.);
  //  FillHist("PtCone_muon1", muons.at(0).PtCone(), weight, 1000, 0., 1000.);
  //  FillHist("PtCone_muon2", muons.at(1).PtCone(), weight, 1000, 0., 1000.);
  //}
  //if(muons.size()==0 && electrons.size()==2){
  //  FillHist("Pt_electron1", electrons.at(0).Pt(), weight, 1000, 0., 1000.);
  //  FillHist("Pt_electron2", electrons.at(1).Pt(), weight, 1000, 0., 1000.);
  //  FillHist("PtCone_electron1", electrons.at(0).PtCone(), weight, 1000, 0., 1000.);
  //  FillHist("PtCone_electron2", electrons.at(1).PtCone(), weight, 1000, 0., 1000.);
  //} //JH : Draw lepton pt and ptcone

//  if(electrons.size() > 0) cout << electrons.at(0).PtCone() << endl;

  // Shift electron energy when RunCF=true
  vector<Electron> electrons_beforeShift;
  vector<Electron> electrons_afterShift;
  electrons_beforeShift.clear();
  electrons_afterShift.clear();
  if(RunCF){
    if(muons.size()==0 && electrons.size()==2){
      electrons_beforeShift.push_back(electrons.at(0));
      electrons_beforeShift.push_back(electrons.at(1));
      electrons = ShiftElectronEnergy(electrons, param, true);
      electrons_afterShift.push_back(electrons.at(0));
      electrons_afterShift.push_back(electrons.at(1));
      METv = UpdateMETElectronCF(METv, electrons_beforeShift, electrons_afterShift);
    }
  }

  // Define leptons (pT order)
  for(unsigned int i=0; i<muons.size(); i++) leptons.push_back(&muons.at(i));
  for(unsigned int i=0; i<electrons.size(); i++) leptons.push_back(&electrons.at(i));
  std::sort(leptons.begin(), leptons.end(), PtComparingPtr);

  // Define leptons passing veto IDs
  for(unsigned int i=0; i<muons_veto.size(); i++) leptons_veto.push_back(&muons_veto.at(i));
  for(unsigned int i=0; i<electrons_veto.size(); i++) leptons_veto.push_back(&electrons_veto.at(i));

  // leptons (minus, plus charge)
  for(unsigned int i=0; i<muons.size(); i++){
    if(muons.at(i).Charge() < 0) leptons_minus.push_back(&muons.at(i));
    if(muons.at(i).Charge() > 0) leptons_plus.push_back(&muons.at(i));
  }
  for(unsigned int i=0; i<electrons.size(); i++){
    if(electrons.at(i).Charge() < 0) leptons_minus.push_back(&electrons.at(i));
    if(electrons.at(i).Charge() > 0) leptons_plus.push_back(&electrons.at(i));
  }

  lepton_veto_size = leptons_veto.size() - leptons.size();

  // Define HT, ST, MET^2/ST
  for(unsigned int i=0; i<jets.size(); i++) HT += jets.at(i).Pt();
  for(unsigned int i=0; i<jets.size(); i++) ST += jets.at(i).Pt();
  for(unsigned int i=0; i<fatjets.size(); i++) ST += fatjets.at(i).Pt();
  for(unsigned int i=0; i<leptons.size(); i++) ST += leptons.at(i)->Pt();
  ST += MET;
  MET2ST = MET*MET/ST;

  //=========================
  //==== Event selections..
  //========================= 

  //=====================================================================================
  //=====================================================================================
  //==== SSWW reproduction (SR, Inverted SR, b-tagged CR, WZ CR, WZb CR)
  //=====================================================================================
  //=====================================================================================

  // jet pt>15, |eta|<4.7, tight lepton cleaned with dR<0.3, pass loose PU, 2a-bjet pt>20, |eta|<2.4, electron pt>10, |eta|<2.5, muon pt>10, |eta|<2.4 

  for(unsigned int it_rg=0; it_rg<regionsSSWW.size(); it_rg++){

    if(electrons_veto.size() > 0) continue;
    if(RunFake) weight *= fakeEst->GetWeight(leptons, param);
    // Cutflow 4 : loose electron veto
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 3.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 3.5, 1., cutflow_bin, 0., cutflow_max);
  
    if(! (muons.size()>=2) ) continue;
    // Cutflow 5 : at least 2 tight muons
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 4.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 4.5, 1., cutflow_bin, 0., cutflow_max);
  
    if(! (jets.size()>=2) ) continue;
    // Cutflow 6 : at least 2 AK4 jets
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 5.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 5.5, 1., cutflow_bin, 0., cutflow_max);

    DiJet = jets.at(0)+jets.at(1);
    double avgEta = 0.5*(jets.at(0).Eta()+jets.at(1).Eta());
    double dEta = fabs(jets.at(0).Eta()-jets.at(1).Eta());
    double max_zep = std::max(fabs(muons.at(0).Eta()-avgEta),fabs(muons.at(1).Eta()-avgEta))/dEta;

    if(it_rg<=2){ // SR, SR_inv, b-tagged CR

      if(! (muons.size()==2 && muons.at(0).Charge()*muons.at(1).Charge()==1) ) continue;
      // SR Cutflow 7 : 2 same sign tight muons
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 6.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 6.5, 1., cutflow_bin, 0., cutflow_max);

      if(muons.at(0).PassID(param.Muon_Tight_ID) && !(muons.at(1).PassID(param.Muon_Tight_ID))) LepCategory = "TL";
      if(!(muons.at(0).PassID(param.Muon_Tight_ID)) && muons.at(1).PassID(param.Muon_Tight_ID)) LepCategory = "LT";
      if(!(muons.at(1).PassID(param.Muon_Tight_ID)) && !(muons.at(0).PassID(param.Muon_Tight_ID))) LepCategory = "LL";

      if(! (muons.at(0).Pt()>30. && muons.at(1).Pt()>30.) ) continue;
      // SR Cutflow 8 : muon pt > 30/30 GeV
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 7.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 7.5, 1., cutflow_bin, 0., cutflow_max);

      DiLep = muons.at(0)+muons.at(1);

      if(! (DiLep.M()>20.) ) continue;
      // SR Cutflow 9 : m(ll) > 20 GeV
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 8.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 8.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (max_zep<0.75) ) continue;
      // SR Cutflow 10 : max zeppenfeld < 0.75
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 9.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 9.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (jets.at(1).Pt()>30.) ) continue;
      // SR Cutflow 11 : jet pt > 30 GeV
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 10.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 10.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (dEta>2.5) ) continue;
      // SR Cutflow 12 : dEta(jj) > 2.5
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 11.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 11.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (DiJet.M()>750.) ) continue;
      // SR Cutflow 13 : m(jj) > 750 GeV
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 12.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 12.5, 1., cutflow_bin, 0., cutflow_max);

      // Skip the tau veto for now;

      if(it_rg<2){ // SRs
        if(! (Nbjet_medium==0) ) continue;
        // SR Cutflow 14 : b-veto
        FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 13.5, weight, cutflow_bin, 0., cutflow_max);
        FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 13.5, 1., cutflow_bin, 0., cutflow_max);

        double dPhi = fabs(muons.at(0).DeltaPhi(muons.at(1)));
        if(it_rg==0){
          if(dPhi>2.){
            // SR Cutflow 15 : dPhi(ll) > 2
            FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 14.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 14.5, 1., cutflow_bin, 0., cutflow_max);
            if(HasFlag("fatjet_veto")){
              if(! (fatjets.size()==0) ) continue; // no fatjet
              // SR Cutflow 16 : no fatjet
              FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 15.5, weight, cutflow_bin, 0., cutflow_max);
              FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 15.5, 1., cutflow_bin, 0., cutflow_max);
            }

            FillHist(regionsSSWW.at(it_rg)+"/Number_Jets_"+IDsuffix, jets.size(), weight, 10, 0., 10.);
            FillHist(regionsSSWW.at(it_rg)+"/Number_BJets_Loose_"+IDsuffix, Nbjet_loose, weight, 10, 0., 10.);
            FillHist(regionsSSWW.at(it_rg)+"/Number_BJets_Medium_"+IDsuffix, Nbjet_medium, weight, 10, 0., 10.);
            FillHist(regionsSSWW.at(it_rg)+"/Number_FatJets_"+IDsuffix, fatjets.size(), weight, 10, 0., 10.);
            if(fatjets.size()>0){
              FillHist(regionsTypeI.at(it_rg)+"/FatJet_Pt_"+IDsuffix, fatjets.at(0).Pt(), weight, 1500, 0., 1500.);
              FillHist(regionsTypeI.at(it_rg)+"/FatJet_Eta_"+IDsuffix, fatjets.at(0).Eta(), weight, 100, -5, 5);
              FillHist(regionsTypeI.at(it_rg)+"/FatJet_Mass_"+IDsuffix, fatjets.at(0).SDMass(), weight, 1500, 0., 1500.);
            }
            FillHist(regionsSSWW.at(it_rg)+"/DiLep_Mass_"+IDsuffix, DiLep.M(), weight, 1500, 0., 1500.);
            FillHist(regionsSSWW.at(it_rg)+"/DiJet_Mass_"+IDsuffix, DiJet.M(), weight, 3000, 0., 3000.);
            FillHist(regionsSSWW.at(it_rg)+"/Mu1_Pt_"+IDsuffix, muons.at(0).Pt(), weight, 1500, 0., 1500.);
            FillHist(regionsSSWW.at(it_rg)+"/Mu2_Pt_"+IDsuffix, muons.at(1).Pt(), weight, 1500, 0., 1500.);
            FillHist(regionsSSWW.at(it_rg)+"/Mu1_Eta_"+IDsuffix, muons.at(0).Eta(), weight, 50, -2.5, 2.5);
            FillHist(regionsSSWW.at(it_rg)+"/Mu2_Eta_"+IDsuffix, muons.at(1).Eta(), weight, 50, -2.5, 2.5);
            FillHist(regionsSSWW.at(it_rg)+"/Jet1_Pt_"+IDsuffix, jets.at(0).Pt(), weight, 1500, 0., 1500.);
            FillHist(regionsSSWW.at(it_rg)+"/Jet2_Pt_"+IDsuffix, jets.at(1).Pt(), weight, 1500, 0., 1500.);
            FillHist(regionsSSWW.at(it_rg)+"/Jet1_Eta_"+IDsuffix, jets.at(0).Eta(), weight, 100, -5, 5);
            FillHist(regionsSSWW.at(it_rg)+"/Jet2_Eta_"+IDsuffix, jets.at(1).Eta(), weight, 100, -5, 5);
            FillHist(regionsSSWW.at(it_rg)+"/MET_"+IDsuffix, MET, weight, 1000, 0., 1000.);
            FillHist(regionsSSWW.at(it_rg)+"/Zep_"+IDsuffix, max_zep, weight, 150, 0., 1.5);
            FillHist(regionsSSWW.at(it_rg)+"/dEtajj_"+IDsuffix, dEta, weight, 100, 0., 10.);
            FillHist(regionsSSWW.at(it_rg)+"/dPhill_"+IDsuffix, dPhi, weight, 32, 0., 3.2);
            FillHist(regionsSSWW.at(it_rg)+"/HToverPt1_"+IDsuffix, HT/muons.at(0).Pt(), weight, 10, 0., 10.);
          }
        }
        else if(it_rg==1){
          if(dPhi<=2.){
            // Inverted SR Cutflow 15 : dPhi(ll) <= 2
            FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 14.5, weight, cutflow_bin, 0., cutflow_max);
            FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 14.5, 1., cutflow_bin, 0., cutflow_max);
            if(HasFlag("fatjet_veto")){
              if(! (fatjets.size()==0) ) continue; // no fatjet
              // SR Cutflow 16 : no fatjet
              FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 15.5, weight, cutflow_bin, 0., cutflow_max);
              FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 15.5, 1., cutflow_bin, 0., cutflow_max);
            }

            FillHist(regionsSSWW.at(it_rg)+"/Number_Jets_"+IDsuffix, jets.size(), weight, 10, 0., 10.);
            FillHist(regionsSSWW.at(it_rg)+"/Number_BJets_Loose_"+IDsuffix, Nbjet_loose, weight, 10, 0., 10.);
            FillHist(regionsSSWW.at(it_rg)+"/Number_BJets_Medium_"+IDsuffix, Nbjet_medium, weight, 10, 0., 10.);
            FillHist(regionsSSWW.at(it_rg)+"/Number_FatJets_"+IDsuffix, fatjets.size(), weight, 10, 0., 10.);
            if(fatjets.size()>0){
              FillHist(regionsTypeI.at(it_rg)+"/FatJet_Pt_"+IDsuffix, fatjets.at(0).Pt(), weight, 1500, 0., 1500.);
              FillHist(regionsTypeI.at(it_rg)+"/FatJet_Eta_"+IDsuffix, fatjets.at(0).Eta(), weight, 100, -5, 5);
              FillHist(regionsTypeI.at(it_rg)+"/FatJet_Mass_"+IDsuffix, fatjets.at(0).SDMass(), weight, 1500, 0., 1500.);
            }
            FillHist(regionsSSWW.at(it_rg)+"/DiLep_Mass_"+IDsuffix, DiLep.M(), weight, 1500, 0., 1500.);
            FillHist(regionsSSWW.at(it_rg)+"/DiJet_Mass_"+IDsuffix, DiJet.M(), weight, 3000, 0., 3000.);
            FillHist(regionsSSWW.at(it_rg)+"/Mu1_Pt_"+IDsuffix, muons.at(0).Pt(), weight, 1500, 0., 1500.);
            FillHist(regionsSSWW.at(it_rg)+"/Mu2_Pt_"+IDsuffix, muons.at(1).Pt(), weight, 1500, 0., 1500.);
            FillHist(regionsSSWW.at(it_rg)+"/Mu1_Eta_"+IDsuffix, muons.at(0).Eta(), weight, 50, -2.5, 2.5);
            FillHist(regionsSSWW.at(it_rg)+"/Mu2_Eta_"+IDsuffix, muons.at(1).Eta(), weight, 50, -2.5, 2.5);
            FillHist(regionsSSWW.at(it_rg)+"/Jet1_Pt_"+IDsuffix, jets.at(0).Pt(), weight, 1500, 0., 1500.);
            FillHist(regionsSSWW.at(it_rg)+"/Jet2_Pt_"+IDsuffix, jets.at(1).Pt(), weight, 1500, 0., 1500.);
            FillHist(regionsSSWW.at(it_rg)+"/Jet1_Eta_"+IDsuffix, jets.at(0).Eta(), weight, 100, -5, 5);
            FillHist(regionsSSWW.at(it_rg)+"/Jet2_Eta_"+IDsuffix, jets.at(1).Eta(), weight, 100, -5, 5);
            FillHist(regionsSSWW.at(it_rg)+"/MET_"+IDsuffix, MET, weight, 1000, 0., 1000.);
            FillHist(regionsSSWW.at(it_rg)+"/Zep_"+IDsuffix, max_zep, weight, 150, 0., 1.5);
            FillHist(regionsSSWW.at(it_rg)+"/dEtajj_"+IDsuffix, dEta, weight, 100, 0., 10.);
            FillHist(regionsSSWW.at(it_rg)+"/dPhill_"+IDsuffix, dPhi, weight, 32, 0., 3.2);
            FillHist(regionsSSWW.at(it_rg)+"/HToverPt1_"+IDsuffix, HT/muons.at(0).Pt(), weight, 10, 0., 10.);
          }
        }
      }
      else if(it_rg==2){ // b-tagged CR
        if(Nbjet_medium==0) continue;
        // b-tagged CR Cutflow 14 : b-veto inversion
        FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 13.5, weight, cutflow_bin, 0., cutflow_max);
        FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 13.5, 1., cutflow_bin, 0., cutflow_max);

        FillHist(regionsSSWW.at(it_rg)+"/Number_Jets_"+IDsuffix, jets.size(), weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/Number_BJets_Loose_"+IDsuffix, Nbjet_loose, weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/Number_BJets_Medium_"+IDsuffix, Nbjet_medium, weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/Number_FatJets_"+IDsuffix, fatjets.size(), weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/DiLep_Mass_"+IDsuffix, DiLep.M(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/DiJet_Mass_"+IDsuffix, DiJet.M(), weight, 3000, 0., 3000.);
        FillHist(regionsSSWW.at(it_rg)+"/Mu1_Pt_"+IDsuffix, muons.at(0).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Mu2_Pt_"+IDsuffix, muons.at(1).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Mu1_Eta_"+IDsuffix, muons.at(0).Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/Mu2_Eta_"+IDsuffix, muons.at(1).Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/Jet1_Pt_"+IDsuffix, jets.at(0).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Jet2_Pt_"+IDsuffix, jets.at(1).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Jet1_Eta_"+IDsuffix, jets.at(0).Eta(), weight, 100, -5, 5);
        FillHist(regionsSSWW.at(it_rg)+"/Jet2_Eta_"+IDsuffix, jets.at(1).Eta(), weight, 100, -5, 5);
        FillHist(regionsSSWW.at(it_rg)+"/MET_"+IDsuffix, MET, weight, 1000, 0., 1000.);
        FillHist(regionsSSWW.at(it_rg)+"/Zep_"+IDsuffix, max_zep, weight, 150, 0., 1.5);
        FillHist(regionsSSWW.at(it_rg)+"/dEtajj_"+IDsuffix, dEta, weight, 100, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/HToverPt1_"+IDsuffix, HT/muons.at(0).Pt(), weight, 10, 0., 10.);

        if(RunFake){
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Number_Jets_"+IDsuffix, jets.size(), weight, 10, 0., 10.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Number_BJets_Loose_"+IDsuffix, Nbjet_loose, weight, 10, 0., 10.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Number_BJets_Medium_"+IDsuffix, Nbjet_medium, weight, 10, 0., 10.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Number_FatJets_"+IDsuffix, fatjets.size(), weight, 10, 0., 10.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/DiLep_Mass_"+IDsuffix, DiLep.M(), weight, 1500, 0., 1500.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/DiJet_Mass_"+IDsuffix, DiJet.M(), weight, 3000, 0., 3000.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Mu1_Pt_"+IDsuffix, muons.at(0).Pt(), weight, 1500, 0., 1500.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Mu2_Pt_"+IDsuffix, muons.at(1).Pt(), weight, 1500, 0., 1500.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Mu1_Eta_"+IDsuffix, muons.at(0).Eta(), weight, 50, -2.5, 2.5);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Mu2_Eta_"+IDsuffix, muons.at(1).Eta(), weight, 50, -2.5, 2.5);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Jet1_Pt_"+IDsuffix, jets.at(0).Pt(), weight, 1500, 0., 1500.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Jet2_Pt_"+IDsuffix, jets.at(1).Pt(), weight, 1500, 0., 1500.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Jet1_Eta_"+IDsuffix, jets.at(0).Eta(), weight, 100, -5, 5);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Jet2_Eta_"+IDsuffix, jets.at(1).Eta(), weight, 100, -5, 5);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/MET_"+IDsuffix, MET, weight, 1000, 0., 1000.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Zep_"+IDsuffix, max_zep, weight, 150, 0., 1.5);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/dEtajj_"+IDsuffix, dEta, weight, 100, 0., 10.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/HToverPt1_"+IDsuffix, HT/muons.at(0).Pt(), weight, 10, 0., 10.);

          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Number_Jets_unweighted_"+IDsuffix, jets.size(), 1., 10, 0., 10.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Number_BJets_Loose_unweighted_"+IDsuffix, Nbjet_loose, 1., 10, 0., 10.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Number_BJets_Medium_unweighted_"+IDsuffix, Nbjet_medium, 1., 10, 0., 10.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Number_FatJets_unweighted_"+IDsuffix, fatjets.size(), 1., 10, 0., 10.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/DiLep_Mass_unweighted_"+IDsuffix, DiLep.M(), 1., 1500, 0., 1500.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/DiJet_Mass_unweighted_"+IDsuffix, DiJet.M(), 1., 3000, 0., 3000.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Mu1_Pt_unweighted_"+IDsuffix, muons.at(0).Pt(), 1., 1500, 0., 1500.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Mu2_Pt_unweighted_"+IDsuffix, muons.at(1).Pt(), 1., 1500, 0., 1500.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Mu1_Eta_unweighted_"+IDsuffix, muons.at(0).Eta(), 1., 50, -2.5, 2.5);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Mu2_Eta_unweighted_"+IDsuffix, muons.at(1).Eta(), 1., 50, -2.5, 2.5);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Jet1_Pt_unweighted_"+IDsuffix, jets.at(0).Pt(), 1., 1500, 0., 1500.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Jet2_Pt_unweighted_"+IDsuffix, jets.at(1).Pt(), 1., 1500, 0., 1500.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Jet1_Eta_unweighted_"+IDsuffix, jets.at(0).Eta(), 1., 100, -5, 5);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Jet2_Eta_unweighted_"+IDsuffix, jets.at(1).Eta(), 1., 100, -5, 5);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/MET_unweighted_"+IDsuffix, MET, 1., 1000, 0., 1000.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/Zep_unweighted_"+IDsuffix, max_zep, 1., 150, 0., 1.5);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/dEtajj_unweighted_"+IDsuffix, dEta, 1., 100, 0., 10.);
          FillHist(regionsSSWW.at(it_rg)+"/"+LepCategory+"/HToverPt1_unweighted_"+IDsuffix, HT/muons.at(0).Pt(), 1., 10, 0., 10.);
        }

      }

    }
    else{ // WZ, WZb CR

      if(! (muons.size()==3 && abs(muons.at(0).Charge()+muons.at(1).Charge()+muons.at(2).Charge())==1) ) continue;
      // CR Cutflow 7 : 1 OSSF muon pair + 1 muon
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 6.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 6.5, 1., cutflow_bin, 0., cutflow_max);

      TriLep = muons.at(0)+muons.at(1)+muons.at(2);

      // select ZCand, ZtagLep1, ZtagLep2, WtagLep
      int l1 = -999, l2 = -999;
      double tmpMassDiff = 1000000.; 
      int dilep_mass20 = 0;
      for(int ilep1=0; ilep1<2; ilep1++){
        for(int ilep2=ilep1+1; ilep2<3; ilep2++){ // for each pair (01, 02, 12)
          DiLep = muons.at(ilep1) + muons.at(ilep2);
          if(DiLep.M()>20.) dilep_mass20++; // count m(ll) > 20 GeV
          if(muons.at(ilep1).Charge()*muons.at(ilep2).Charge()>0) continue; // now skip the same sign
          Ztemp = muons.at(ilep1) + muons.at(ilep2);
          if(fabs(Ztemp.M() - MZ) < tmpMassDiff){
            tmpMassDiff = fabs(Ztemp.M() - MZ);
            ZCand = Ztemp; l1 = ilep1; l2 = ilep2; //JH : l1, l2 are the closest to m(Z)
          }
        }
      }
      ZtagLep1 = muons.at(l1);
      ZtagLep2 = muons.at(l2);
      for(int ilep3=0; ilep3<3; ilep3++){
        if(ilep3!=l1 && ilep3!=l2) WtagLep = muons.at(ilep3);
      }
      if(! (WtagLep.Pt()>0.) ){
        cout << "In SSWW WZ CR :: WtagLep pt is not positive. Something's wrong!!" << endl;
        cout << "WtagLep pt : " << WtagLep.Pt() << endl;
        exit(EXIT_FAILURE);
      }

      if(! (ZtagLep1.Pt()>25. && ZtagLep2.Pt()>10. && WtagLep.Pt()>25.) ) continue;
      // CR Cutflow 8 : muon pt > 25/10/25 GeV
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 7.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 7.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (dilep_mass20==3) ) continue;
      // CR Cutflow 9 : m(ll) > 20 GeV
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 8.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 8.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (max_zep<1.) ) continue;
      // CR Cutflow 10 : max zeppenfeld < 1.
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 9.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 9.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (jets.at(1).Pt()>30.) ) continue;
      // CR Cutflow 11 : jet pt > 30 GeV
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 10.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 10.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (dEta>2.5) ) continue;
      // CR Cutflow 12 : dEta(jj) > 2.5
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 11.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 11.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (DiJet.M()>750.) ) continue;
      // CR Cutflow 13 : m(jj) > 750 GeV
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 12.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 12.5, 1., cutflow_bin, 0., cutflow_max);

      // Skip the tau veto for now;

      if(! (MET>30.) ) continue;
      // CR Cutflow 14 : MET > 30 GeV
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 13.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 13.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (IsOnZ(ZCand.M(), 15.)) ) continue;
      // CR Cutflow 15 : |m(ll)-m(Z)| < 15 GeV
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 14.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 14.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (TriLep.M()>100.) ) continue;
      // CR Cutflow 16 : m(lll) > 100 GeV
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 15.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 15.5, 1., cutflow_bin, 0., cutflow_max);

      if(it_rg==3){ // WZ CR
        if(! (Nbjet_medium==0) ) continue;
        // WZ CR Cutflow 17 : b-veto
        FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 16.5, weight, cutflow_bin, 0., cutflow_max);
        FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 16.5, 1., cutflow_bin, 0., cutflow_max);

        FillHist(regionsSSWW.at(it_rg)+"/Number_Jets_"+IDsuffix, jets.size(), weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/Number_BJets_Loose_"+IDsuffix, Nbjet_loose, weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/Number_BJets_Medium_"+IDsuffix, Nbjet_medium, weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/Number_FatJets_"+IDsuffix, fatjets.size(), weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/ZCand_Mass_"+IDsuffix, ZCand.M(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/TriLep_Mass_"+IDsuffix, TriLep.M(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/DiJet_Mass_"+IDsuffix, DiJet.M(), weight, 3000, 0., 3000.);
        FillHist(regionsSSWW.at(it_rg)+"/Mu1_Pt_"+IDsuffix, muons.at(0).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Mu2_Pt_"+IDsuffix, muons.at(1).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Mu3_Pt_"+IDsuffix, muons.at(2).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/ZtagLep1_Pt_"+IDsuffix, ZtagLep1.Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/ZtagLep2_Pt_"+IDsuffix, ZtagLep2.Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/WtagLep_Pt_"+IDsuffix, WtagLep.Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Mu1_Eta_"+IDsuffix, muons.at(0).Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/Mu2_Eta_"+IDsuffix, muons.at(1).Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/Mu3_Eta_"+IDsuffix, muons.at(2).Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/ZtagLep1_Eta_"+IDsuffix, ZtagLep1.Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/ZtagLep2_Eta_"+IDsuffix, ZtagLep2.Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/WtagLep_Eta_"+IDsuffix, WtagLep.Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/Jet1_Pt_"+IDsuffix, jets.at(0).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Jet2_Pt_"+IDsuffix, jets.at(1).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Jet1_Eta_"+IDsuffix, jets.at(0).Eta(), weight, 100, -5, 5);
        FillHist(regionsSSWW.at(it_rg)+"/Jet2_Eta_"+IDsuffix, jets.at(1).Eta(), weight, 100, -5, 5);
        FillHist(regionsSSWW.at(it_rg)+"/MET_"+IDsuffix, MET, weight, 1000, 0., 1000.);
        FillHist(regionsSSWW.at(it_rg)+"/Zep_"+IDsuffix, max_zep, weight, 150, 0., 1.5);
        FillHist(regionsSSWW.at(it_rg)+"/dEtajj_"+IDsuffix, dEta, weight, 100, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/HToverPt1_"+IDsuffix, HT/muons.at(0).Pt(), weight, 10, 0., 10.);
      }
      else if(it_rg==4){ // WZb CR
        if(Nbjet_medium==0) continue;
        // WZb CR Cutflow 17 : b-veto inversion
        FillHist(regionsSSWW.at(it_rg)+"/Number_Events_"+IDsuffix, 16.5, weight, cutflow_bin, 0., cutflow_max);
        FillHist(regionsSSWW.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 16.5, 1., cutflow_bin, 0., cutflow_max);

        FillHist(regionsSSWW.at(it_rg)+"/Number_Jets_"+IDsuffix, jets.size(), weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/Number_BJets_Loose_"+IDsuffix, Nbjet_loose, weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/Number_BJets_Medium_"+IDsuffix, Nbjet_medium, weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/Number_FatJets_"+IDsuffix, fatjets.size(), weight, 10, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/ZCand_Mass_"+IDsuffix, ZCand.M(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/TriLep_Mass_"+IDsuffix, TriLep.M(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/DiJet_Mass_"+IDsuffix, DiJet.M(), weight, 3000, 0., 3000.);
        FillHist(regionsSSWW.at(it_rg)+"/Mu1_Pt_"+IDsuffix, muons.at(0).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Mu2_Pt_"+IDsuffix, muons.at(1).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Mu3_Pt_"+IDsuffix, muons.at(2).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/ZtagLep1_Pt_"+IDsuffix, ZtagLep1.Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/ZtagLep2_Pt_"+IDsuffix, ZtagLep2.Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/WtagLep_Pt_"+IDsuffix, WtagLep.Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Mu1_Eta_"+IDsuffix, muons.at(0).Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/Mu2_Eta_"+IDsuffix, muons.at(1).Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/Mu3_Eta_"+IDsuffix, muons.at(2).Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/ZtagLep1_Eta_"+IDsuffix, ZtagLep1.Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/ZtagLep2_Eta_"+IDsuffix, ZtagLep2.Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/WtagLep_Eta_"+IDsuffix, WtagLep.Eta(), weight, 50, -2.5, 2.5);
        FillHist(regionsSSWW.at(it_rg)+"/Jet1_Pt_"+IDsuffix, jets.at(0).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Jet2_Pt_"+IDsuffix, jets.at(1).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsSSWW.at(it_rg)+"/Jet1_Eta_"+IDsuffix, jets.at(0).Eta(), weight, 100, -5, 5);
        FillHist(regionsSSWW.at(it_rg)+"/Jet2_Eta_"+IDsuffix, jets.at(1).Eta(), weight, 100, -5, 5);
        FillHist(regionsSSWW.at(it_rg)+"/MET_"+IDsuffix, MET, weight, 1000, 0., 1000.);
        FillHist(regionsSSWW.at(it_rg)+"/Zep_"+IDsuffix, max_zep, weight, 150, 0., 1.5);
        FillHist(regionsSSWW.at(it_rg)+"/dEtajj_"+IDsuffix, dEta, weight, 100, 0., 10.);
        FillHist(regionsSSWW.at(it_rg)+"/HToverPt1_"+IDsuffix, HT/muons.at(0).Pt(), weight, 10, 0., 10.);
      }

    }

  } //SSWW SR, CR done

  // Now estimate if there are any SSWWTypeI signals falling into HNTypeI signal region
  for(unsigned int it_rg=0; it_rg<regionsTypeI.size(); it_rg++){

    if(! (muons.size()==2 && electrons.size()==0 && muons.at(0).Charge()*muons.at(1).Charge()==1) ) continue;
    if(RunFake) weight *= fakeEst->GetWeight(leptons, param);
    // Cutflow 4 : 2 same-sign tight muons
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 3.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 3.5, 1., cutflow_bin, 0., cutflow_max);

    if(! (muons.at(0).Pt()>20. && muons.at(1).Pt()>15.) ) continue; // Note I use pt > 15 GeV, due to WG, ZG sample
    // Cutflow 5 : muon pt > 20/15 GeV
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 4.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 4.5, 1., cutflow_bin, 0., cutflow_max);

    if(lepton_veto_size > 0) continue; // 3rd lepton veto
    // Cutflow 6 : 3rd lepton veto
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 5.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 5.5, 1., cutflow_bin, 0., cutflow_max);

    DiLep = muons.at(0)+muons.at(1);
    double dPhi = fabs(muons.at(0).DeltaPhi(muons.at(1)));

    if(! (DiLep.M() > 10.) ) continue; // m(ll) > 10 GeV
    // Cutflow 7 : m(ll) > 10 GeV
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 6.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 6.5, 1., cutflow_bin, 0., cutflow_max);

    if(! (Nbjet_medium == 0) ) continue;
    // Cutflow 8 : b-veto
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 7.5, weight, cutflow_bin, 0., cutflow_max);
    FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 7.5, 1., cutflow_bin, 0., cutflow_max);

    if(it_rg==0){ // high SR1
      if(! (jets.size()>=2) ) continue;
      // Cutflow 9 : at least 2 jets
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 8.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 8.5, 1., cutflow_bin, 0., cutflow_max);

      // Now let's invert the SSWW cut
      DiJet = jets.at(0)+jets.at(1);
      double avgEta = 0.5*(jets.at(0).Eta()+jets.at(1).Eta());
      double dEta = fabs(jets.at(0).Eta()-jets.at(1).Eta());
      double max_zep = std::max(fabs(muons.at(0).Eta()-avgEta),fabs(muons.at(1).Eta()-avgEta))/dEta;

      if( (electrons_veto.size()==0)&&(muons.at(0).Pt()>30. && muons.at(1).Pt()>30.)&&(DiLep.M()>20.)&&(max_zep<0.75)&&(jets.at(1).Pt()>30.)&&(dEta>2.5)&&(DiJet.M()>750.)&&(Nbjet_medium==0) ) continue;
      // Cutflow 10 : SSWW signal failure
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 9.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 9.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (fatjets.size()==0) ) continue; // no fatjet
      // Cutflow 11 : no fatjet
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 10.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 10.5, 1., cutflow_bin, 0., cutflow_max);

      // Select two jets that makes m(jj) closest to m(W)
      double tmpMassDiff = 10000.;
      int j1 = 0, j2 = 0;
      for(unsigned int k=0; k<jets.size(); k++){
        for(unsigned int l=k+1; l<jets.size(); l++){
          Wtemp = jets.at(k) + jets.at(l);
          if(fabs(Wtemp.M() - MW) < tmpMassDiff){
            tmpMassDiff = fabs(Wtemp.M() - MW);
            j1 = k; j2 = l; //JH : this saves (k,l) tuple if that combination gives a smaller difference than the former combination
          }
        }
      }
      WCand = jets.at(j1) + jets.at(j1);

      if(! (WCand.M() < 150.) ) continue;
      // Cutflow 12 : m(WCand) < 150 GeV 
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 11.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 11.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (MET2ST < 15.) ) continue;
      // Cutflow 13 : MET2ST < 15 GeV
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 12.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 12.5, 1., cutflow_bin, 0., cutflow_max);

      FillHist(regionsTypeI.at(it_rg)+"/Number_Jets_"+IDsuffix, jets.size(), weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/Number_BJets_Loose_"+IDsuffix, Nbjet_loose, weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/Number_BJets_Medium_"+IDsuffix, Nbjet_medium, weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/Number_FatJets_"+IDsuffix, fatjets.size(), weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/DiLep_Mass_"+IDsuffix, DiLep.M(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/DiJet_Mass_"+IDsuffix, DiJet.M(), weight, 3000, 0., 3000.);
      FillHist(regionsTypeI.at(it_rg)+"/Mu1_Pt_"+IDsuffix, muons.at(0).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/Mu2_Pt_"+IDsuffix, muons.at(1).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/Mu1_Eta_"+IDsuffix, muons.at(0).Eta(), weight, 50, -2.5, 2.5);
      FillHist(regionsTypeI.at(it_rg)+"/Mu2_Eta_"+IDsuffix, muons.at(1).Eta(), weight, 50, -2.5, 2.5);
      FillHist(regionsTypeI.at(it_rg)+"/Jet1_Pt_"+IDsuffix, jets.at(0).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/Jet2_Pt_"+IDsuffix, jets.at(1).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/Jet1_Eta_"+IDsuffix, jets.at(0).Eta(), weight, 100, -5, 5);
      FillHist(regionsTypeI.at(it_rg)+"/Jet2_Eta_"+IDsuffix, jets.at(1).Eta(), weight, 100, -5, 5);
      FillHist(regionsTypeI.at(it_rg)+"/MET_"+IDsuffix, MET, weight, 1000, 0., 1000.);
      FillHist(regionsTypeI.at(it_rg)+"/Zep_"+IDsuffix, max_zep, weight, 150, 0., 1.5);
      FillHist(regionsTypeI.at(it_rg)+"/dEtajj_"+IDsuffix, dEta, weight, 100, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/dPhill_"+IDsuffix, dPhi, weight, 32, 0., 3.2);
      FillHist(regionsTypeI.at(it_rg)+"/HToverPt1_"+IDsuffix, HT/muons.at(0).Pt(), weight, 10, 0., 10.);

      if(! (fabs(jets.at(0).Eta())<2.7 && fabs(jets.at(1).Eta())<2.7) ) continue;
      // Cutflow 14 : jet eta < 2.7
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 13.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 13.5, 1., cutflow_bin, 0., cutflow_max);

      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Number_Jets_"+IDsuffix, jets.size(), weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Number_BJets_Loose_"+IDsuffix, Nbjet_loose, weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Number_BJets_Medium_"+IDsuffix, Nbjet_medium, weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Number_FatJets_"+IDsuffix, fatjets.size(), weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/DiLep_Mass_"+IDsuffix, DiLep.M(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/DiJet_Mass_"+IDsuffix, DiJet.M(), weight, 3000, 0., 3000.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Mu1_Pt_"+IDsuffix, muons.at(0).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Mu2_Pt_"+IDsuffix, muons.at(1).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Mu1_Eta_"+IDsuffix, muons.at(0).Eta(), weight, 50, -2.5, 2.5);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Mu2_Eta_"+IDsuffix, muons.at(1).Eta(), weight, 50, -2.5, 2.5);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Jet1_Pt_"+IDsuffix, jets.at(0).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Jet2_Pt_"+IDsuffix, jets.at(1).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Jet1_Eta_"+IDsuffix, jets.at(0).Eta(), weight, 100, -5, 5);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Jet2_Eta_"+IDsuffix, jets.at(1).Eta(), weight, 100, -5, 5);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/MET_"+IDsuffix, MET, weight, 1000, 0., 1000.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Zep_"+IDsuffix, max_zep, weight, 150, 0., 1.5);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/dEtajj_"+IDsuffix, dEta, weight, 100, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/dPhill_"+IDsuffix, dPhi, weight, 32, 0., 3.2);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/HToverPt1_"+IDsuffix, HT/muons.at(0).Pt(), weight, 10, 0., 10.);
    }
    else if(it_rg==1){ // high SR1 w/ 1 jet
      if(! (jets.size()==1 && fatjets.size()==0) ) continue;
      // Cutflow 9 : only 1 jet, no fatjet
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 8.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 8.5, 1., cutflow_bin, 0., cutflow_max);

      FillHist(regionsTypeI.at(it_rg)+"/Number_Jets_"+IDsuffix, jets.size(), weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/Number_BJets_Loose_"+IDsuffix, Nbjet_loose, weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/Number_BJets_Medium_"+IDsuffix, Nbjet_medium, weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/Number_FatJets_"+IDsuffix, fatjets.size(), weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/DiLep_Mass_"+IDsuffix, DiLep.M(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/Mu1_Pt_"+IDsuffix, muons.at(0).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/Mu2_Pt_"+IDsuffix, muons.at(1).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/Mu1_Eta_"+IDsuffix, muons.at(0).Eta(), weight, 50, -2.5, 2.5);
      FillHist(regionsTypeI.at(it_rg)+"/Mu2_Eta_"+IDsuffix, muons.at(1).Eta(), weight, 50, -2.5, 2.5);
      FillHist(regionsTypeI.at(it_rg)+"/Jet1_Pt_"+IDsuffix, jets.at(0).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/Jet1_Eta_"+IDsuffix, jets.at(0).Eta(), weight, 100, -5, 5);
      FillHist(regionsTypeI.at(it_rg)+"/MET_"+IDsuffix, MET, weight, 1000, 0., 1000.);
      FillHist(regionsTypeI.at(it_rg)+"/dPhill_"+IDsuffix, dPhi, weight, 32, 0., 3.2);
      FillHist(regionsTypeI.at(it_rg)+"/HToverPt1_"+IDsuffix, HT/muons.at(0).Pt(), weight, 10, 0., 10.);

      if(! (fabs(jets.at(0).Eta())<2.7) ) continue;
      // Cutflow 10 : jet eta < 2.7
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 9.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 9.5, 1., cutflow_bin, 0., cutflow_max);

      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Number_Jets_"+IDsuffix, jets.size(), weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Number_BJets_Loose_"+IDsuffix, Nbjet_loose, weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Number_BJets_Medium_"+IDsuffix, Nbjet_medium, weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Number_FatJets_"+IDsuffix, fatjets.size(), weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/DiLep_Mass_"+IDsuffix, DiLep.M(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Mu1_Pt_"+IDsuffix, muons.at(0).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Mu2_Pt_"+IDsuffix, muons.at(1).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Mu1_Eta_"+IDsuffix, muons.at(0).Eta(), weight, 50, -2.5, 2.5);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Mu2_Eta_"+IDsuffix, muons.at(1).Eta(), weight, 50, -2.5, 2.5);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Jet1_Pt_"+IDsuffix, jets.at(0).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/Jet1_Eta_"+IDsuffix, jets.at(0).Eta(), weight, 100, -5, 5);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/MET_"+IDsuffix, MET, weight, 1000, 0., 1000.);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/dPhill_"+IDsuffix, dPhi, weight, 32, 0., 3.2);
      FillHist(regionsTypeI.at(it_rg)+"/jeta2p7/HToverPt1_"+IDsuffix, HT/muons.at(0).Pt(), weight, 10, 0., 10.);
    }
    else if(it_rg==2){ // high SR2
      if(HasFlag("fatjet_veto")){
        if(! (fatjets.size()>0) ) continue; // at least 1 fatjet
      }
      else{
        if(! (jets.size()<2 && fatjets.size()>0) ) continue; // at best 1 jet && at least 1 fatjet
      }
      // Cutflow 9 : (at best 1 jet), at least 1 fatjet
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 8.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 8.5, 1., cutflow_bin, 0., cutflow_max);

      double tmpMassDiff = 10000.;
      int j3 = 0;
      for(unsigned int k=0; k<fatjets.size(); k++){
        if(fabs(fatjets.at(k).M() - MW) < tmpMassDiff){
          tmpMassDiff = fabs(fatjets.at(k).SDMass() - MW);
          j3 = k;
        }
      }

      if(! (fatjets.at(j3).SDMass() < 150.) ) continue;
      // Cutflow 10 : m(WCand) < 150 GeV 
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 9.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 9.5, 1., cutflow_bin, 0., cutflow_max);

      if(! (MET2ST < 15.) ) continue;
      // Cutflow 11 : MET2ST < 15 GeV
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_"+IDsuffix, 10.5, weight, cutflow_bin, 0., cutflow_max);
      FillHist(regionsTypeI.at(it_rg)+"/Number_Events_unweighted_"+IDsuffix, 10.5, 1., cutflow_bin, 0., cutflow_max);

      FillHist(regionsTypeI.at(it_rg)+"/Number_Jets_"+IDsuffix, jets.size(), weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/Number_BJets_Loose_"+IDsuffix, Nbjet_loose, weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/Number_BJets_Medium_"+IDsuffix, Nbjet_medium, weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/Number_FatJets_"+IDsuffix, fatjets.size(), weight, 10, 0., 10.);
      FillHist(regionsTypeI.at(it_rg)+"/DiLep_Mass_"+IDsuffix, DiLep.M(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/Mu1_Pt_"+IDsuffix, muons.at(0).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/Mu2_Pt_"+IDsuffix, muons.at(1).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/Mu1_Eta_"+IDsuffix, muons.at(0).Eta(), weight, 50, -2.5, 2.5);
      FillHist(regionsTypeI.at(it_rg)+"/Mu2_Eta_"+IDsuffix, muons.at(1).Eta(), weight, 50, -2.5, 2.5);
      FillHist(regionsTypeI.at(it_rg)+"/FatJet_Pt_"+IDsuffix, fatjets.at(0).Pt(), weight, 1500, 0., 1500.);
      FillHist(regionsTypeI.at(it_rg)+"/FatJet_Eta_"+IDsuffix, fatjets.at(0).Eta(), weight, 100, -5, 5);
      FillHist(regionsTypeI.at(it_rg)+"/FatJet_Mass_"+IDsuffix, fatjets.at(0).SDMass(), weight, 1500, 0., 1500.);
      if(jets.size()>0){
        FillHist(regionsTypeI.at(it_rg)+"/Jet1_Pt_"+IDsuffix, jets.at(0).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsTypeI.at(it_rg)+"/Jet1_Eta_"+IDsuffix, jets.at(0).Eta(), weight, 100, -5, 5);
      }
      if(jets.size()>1){
        FillHist(regionsTypeI.at(it_rg)+"/Jet2_Pt_"+IDsuffix, jets.at(1).Pt(), weight, 1500, 0., 1500.);
        FillHist(regionsTypeI.at(it_rg)+"/Jet2_Eta_"+IDsuffix, jets.at(1).Eta(), weight, 100, -5, 5);
        FillHist(regionsTypeI.at(it_rg)+"/DiJet_Mass_"+IDsuffix, DiJet.M(), weight, 3000, 0., 3000.);
      }
      FillHist(regionsTypeI.at(it_rg)+"/MET_"+IDsuffix, MET, weight, 1000, 0., 1000.);
      FillHist(regionsTypeI.at(it_rg)+"/dPhill_"+IDsuffix, dPhi, weight, 32, 0., 3.2);
      FillHist(regionsTypeI.at(it_rg)+"/HToverPt1_"+IDsuffix, HT/muons.at(0).Pt(), weight, 10, 0., 10.);
    }

  }

}
