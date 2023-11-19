#include "HNL_SignalRegionPlotter.h"

void HNL_SignalRegionPlotter::initializeAnalyzer(){

  // All default settings like trigger/ PD/ BJet are decalred in HNL_LeptonCore::initializeAnalyzer to make them consistent for all HNL codes

  HNL_LeptonCore::initializeAnalyzer();
  SetupEventMVAReader();
  nLog = 100000;

}


void HNL_SignalRegionPlotter::executeEvent(){

  FillTimer("START_EV");
  
  vector<TString> LepIDs = {"HNL_ULID","HNTightV2"};//,"TopHN", "DefaultPOGTight"};
  //vector<HNL_LeptonCore::Channel> ChannelsToRun = {EE,MuMu,EMu,MuE};
  vector<HNL_LeptonCore::Channel> ChannelsToRun = {MuMu};

  for (auto id: LepIDs){
    for(auto channel : ChannelsToRun){
      AnalyzerParameter param = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
      param.PlottingVerbose = -1; /// ONLY DRAW BASIC PLOTS
      RunULAnalysis(param);
      
      if(!IsData) RunSyst=true;
      if(RunSyst){
  TString param_name = param.Name;
  vector<AnalyzerParameter::Syst> SystList;// = GetSystList("Initial");
  
  for(auto isyst : SystList){
    param.syst_ = AnalyzerParameter::Syst(isyst);
    
    param.Name = "Syst_"+param.GetSystType()+param_name;
    param.DefName = "Syst_"+param.GetSystType()+param_name;
    RunULAnalysis(param);
  }
      }    
      
    }
  }
  FillTimer("END_EV");

  return ;
}

void HNL_SignalRegionPlotter::RunULAnalysis(AnalyzerParameter param){

  if(run_Debug) cout << "HNL_SignalRegionPlotter::executeEvent " << endl;

  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);
  
  // veto leptons
  std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, 10., 2.5); 
  std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID,     5., 2.4);
  
  TString el_ID = SetLeptonID("Electron",param);
  TString mu_ID = SetLeptonID("Muon", param);

  double Min_Muon_Pt     = (RunFake) ? 3. : 5.;
  double Min_Electron_Pt = (RunFake) ? 7. : 10.;

  std::vector<Muon>       MuonCollTInit     = SelectMuons    ( param,mu_ID, Min_Muon_Pt,     2.4, weight);
  std::vector<Electron>   ElectronCollTInit = SelectElectrons( param,el_ID, Min_Electron_Pt, 2.5, weight);
 
  std::vector<Muon>       MuonCollT      =  GetLepCollByRunType   ( MuonCollTInit ,   param);
  std::vector<Electron>   ElectronCollT  =  GetLepCollByRunType   ( ElectronCollTInit,param);

  std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(MuonCollV,ElectronCollV);
  std::vector<Tau>        TauColl        = SelectTaus   (leps_veto,param.Tau_Veto_ID,20., 2.3);

  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets("VBFTight",  param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);
 
  Particle METv = GetvMET("PuppiT1xyULCorr",param); // returns MET with systematic correction; run this after all object selection done; NOTE that VBF jet is used here
  
  EvalJetWeight(AK4_JetColl, AK8_JetColl, weight, param);

  FillTimer("START_SR");

  RunAllSignalRegions(Inclusive,
          ElectronCollT,ElectronCollV,MuonCollT,MuonCollV,  TauColl,
          AK4_JetCollLoose, AK4_JetAllColl, AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
          ev,METv, param, weight);

  FillTimer("END_SR");


}



 


HNL_SignalRegionPlotter::HNL_SignalRegionPlotter(){

  cout << "HNL_SignalRegionPlotter::HNL_SignalRegionPlotter  TMVA::Tools::Instance() " << endl;
  SetupEvMVA();
  
}
 
HNL_SignalRegionPlotter::~HNL_SignalRegionPlotter(){

  DeleteEvMVA();

}




