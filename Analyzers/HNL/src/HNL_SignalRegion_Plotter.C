#include "HNL_SignalRegion_Plotter.h"

void HNL_SignalRegion_Plotter::initializeAnalyzer(){

  // All default settings like trigger/ PD/ BJet are decalred in HNL_LeptonCore::initializeAnalyzer to make them consistent for all HNL codes

  HNL_LeptonCore::initializeAnalyzer();
  
  bool run_ee_bdt=false;
  bool run_mm_bdt=false;
  bool run_em_bdt=false;
  
  if(HasFlag("EE") || HasFlag("MuMu") || HasFlag("EMu")){
    run_ee_bdt=HasFlag("EE");
    run_mm_bdt=HasFlag("MuMu");
    run_em_bdt=HasFlag("EMu");
    SetupEventMVAReader("V2",run_ee_bdt,run_mm_bdt,run_em_bdt);
  }
  else SetupEventMVAReader("V2");

  nLog = 100000;

  RunEE   = HasFlag("EE");
  RunMuMu = HasFlag("MuMu");
  RunEMu  = HasFlag("EMu");
  RunSyst = HasFlag("RunSyst");
  RunFullSyst = HasFlag("RunFullSyst");
  RunTopID = HasFlag("RunHNTop");
  RunPOGID = HasFlag("RunPOG");
  RunHighPtID = HasFlag("RunHighPt");
  RunPekingID = HasFlag("RunPeking");
  
}


void HNL_SignalRegion_Plotter::executeEvent(){

  FillTimer("START_EV");
  
  if(_jentry == 0){
    cout << "HNL_SignalRegion_Plotter::IsData = " << IsData << endl;
  }
  vector<TString> LepIDs = {"HNL_ULID"}; //,"HNTightV2"};
  if(RunTopID) LepIDs = {"TopHN"};
  if(RunPOGID) LepIDs = {"POGTight"};
  if(RunHighPtID) LepIDs = {"HighPt"};
  if(RunPekingID) LepIDs = {"Peking"};
  //  if(strcmp(std::getenv("USER"),"jalmond")==0) LepIDs = {"HNL_ULID","POGTight","TopHN","HNTightV2","MVAPOG"};//,"HNTightV2","POGTight","TopHN","HighPt"};

  vector<HNL_LeptonCore::Channel> ChannelsToRun = {};
  if(RunEE)   ChannelsToRun.push_back(EE);
  if(RunMuMu) ChannelsToRun.push_back(MuMu);
  if(RunEMu)  ChannelsToRun.push_back(EMu);
  if(ChannelsToRun.size() == 0) ChannelsToRun = {EE,MuMu,EMu};

  for (auto id: LepIDs){
    for(auto channel : ChannelsToRun){

      AnalyzerParameter param = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
      param.PlottingVerbose = 0;
      if(id == "HNL_ULID")         param.PlottingVerbose = 1;
      if(id.Contains("HEEP"))      param.PlottingVerbose = 1;
      RunULAnalysis(param);

      RunSyst = true; //JH
      if(RunSyst||RunFullSyst){
        TString param_name = param.Name;
        //vector<AnalyzerParameter::Syst> SystList = GetSystList("All");
        vector<AnalyzerParameter::Syst> SystList = GetSystList("Quick");

        //if(RunSyst) {
        //  /// Set Individual systs    
        //  SystList = { 
        //    AnalyzerParameter::MuonEnUp,AnalyzerParameter::MuonEnDown,
        //    AnalyzerParameter::ElectronEnUp,AnalyzerParameter::ElectronEnDown,
        //  };
        //}

        for(auto isyst : SystList){
          bool runJob = UpdateParamBySyst(id,param,AnalyzerParameter::Syst(isyst),param_name);
          if(runJob) RunULAnalysis(param);
        }
      }
      
    }
  }
  FillTimer("END_EV");

  return ;
}

void HNL_SignalRegion_Plotter::RunULAnalysis(AnalyzerParameter param){

  if(run_Debug) cout << "HNL_SignalRegion_Plotter::executeEvent " << endl;

  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);
  
  // HL ID
  std::vector<Electron>   ElectronCollV = SelectElectrons(param,param.Electron_Veto_ID, 10., 2.5); 
  std::vector<Muon>       MuonCollV     = SelectMuons    (param,param.Muon_Veto_ID,     5., 2.4);
  
  TString el_ID = SetLeptonID("Electron",param);
  TString mu_ID = SetLeptonID("Muon", param);


  double Min_FakeMuon_Pt      =  5;
  double Min_FakeElectron_Pt =  10 ;
  std::vector<Muon>       MuonTightColl_Init     = SelectMuons    ( param,mu_ID,     Min_FakeMuon_Pt,     2.4,weight);
  std::vector<Electron>   ElectronTightColl_Init = SelectElectrons( param,el_ID, Min_FakeElectron_Pt, 2.5,weight);

  //// Apply Full Pt cut after pt corrected in fakes                                                                            
  double Min_Muon_Pt     =  10.;
  double Min_Electron_Pt =  15;
  std::vector<Muon>       MuonCollT  = SelectMuons(MuonTightColl_Init,mu_ID,     Min_Muon_Pt,     2.4);
  std::vector<Electron>   ElectronCollT = SelectElectrons(ElectronTightColl_Init,el_ID, Min_Electron_Pt, 2.5);

  std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(MuonCollV,ElectronCollV);
  std::vector<Tau>        TauColl        = SelectTaus   (leps_veto,param.Tau_Veto_ID,20., 2.3);

  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);
 
  Particle METv = GetvMET("PuppiT1xyULCorr", param, AK4_VBF_JetColl, AK8_JetColl, MuonCollT, ElectronCollT); // returns MET with systematic correction; run this after all object selection done; NOTE that VBF jet is used here
  
  EvalJetWeight(AK4_JetColl, AK8_JetColl, weight, param);

  FillTimer("START_SR");

  vector<int> RunEl ;
  if(RunCF) RunEl =  {0,1} ;
  else RunEl = {-1};

  for(auto ir : RunEl){
    
    RunAllSignalRegions(Inclusive,
      ElectronCollT,ElectronCollV,MuonCollT,MuonCollV,  TauColl,
      AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
      ev,METv, param, ir, weight);
  }

  FillTimer("END_SR");


}
 

HNL_SignalRegion_Plotter::HNL_SignalRegion_Plotter(){

  cout << "HNL_SignalRegion_Plotter::HNL_SignalRegion_Plotter  TMVA::Tools::Instance() " << endl;
  SetupEvMVA();
  
}
 
HNL_SignalRegion_Plotter::~HNL_SignalRegion_Plotter(){

  DeleteEvMVA();

}




