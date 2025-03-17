#include "HNL_SignalRegionPlotter17028.h"

void HNL_SignalRegionPlotter17028::initializeAnalyzer(){

  // All default settings like trigger/ PD/ BJet are decalred in HNL_LeptonCore::initializeAnalyzer to make them consistent for all HNL codes

  HNL_LeptonCore::initializeAnalyzer();

}


void HNL_SignalRegionPlotter17028::executeEvent(){
  
  if((_jentry==0)){ 
    // Print out trigger info in HNL_LeptonCore::initializeAnalyzer
    TriggerPrintOut(GetEvent());
  }
  
  AnalyzerParameter param_signal = HNL_LeptonCore::InitialiseHNLParameter("EXO17028");
  RunEXO17028Analysis(param_signal);

  return ;
}

void HNL_SignalRegionPlotter17028::RunEXO17028Analysis(AnalyzerParameter param){


  if(run_Debug) cout << "HNL_SignalRegionPlotter17028::executeEvent " << endl;

  
  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);

  
  // HL ID
  std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, 10., 2.5); 
  std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID, 5., 2.4);

  TString el_ID = (RunFake) ?  param.Electron_FR_ID : param.Electron_Tight_ID ;
  TString mu_ID = (RunFake) ?  param.Muon_FR_ID :  param.Muon_Tight_ID ;

  double Min_Muon_Pt     = (RunFake) ? 8. : 10.;
  double Min_Electron_Pt = (RunFake) ? 8. : 10.;



  std::vector<Muon>       MuonCollT     = SelectMuons    ( param,mu_ID, Min_Muon_Pt,     2.4, weight);
  std::vector<Electron>   ElectronCollT = SelectElectrons( param,el_ID, Min_Electron_Pt, 2.5, weight);

  std::vector<Tau>        mytaus        = GetTaus     (param.Tau_Veto_ID,20., 2.3); 

  // Creat Lepton vector to have lepton blind codes 

  
  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets("EXO17028",param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets("Tight",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);


  Particle METv = GetvMET("PuppiT1xyULCorr", param);

  EvalJetWeight(AK4_JetColl, AK4_JetColl, AK8_JetColl, weight, param);

  RunEXO17028SignalRegions(ElectronCollT,ElectronCollV,MuonCollT,MuonCollV,  AK4_JetColl,AK8_JetColl, AK4_BJetColl, ev,METv, param, weight);
  
}


 


HNL_SignalRegionPlotter17028::HNL_SignalRegionPlotter17028(){


}
 
HNL_SignalRegionPlotter17028::~HNL_SignalRegionPlotter17028(){

}




