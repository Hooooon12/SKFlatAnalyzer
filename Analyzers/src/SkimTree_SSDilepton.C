#include "SkimTree_SSDilepton.h"

void SkimTree_SSDilepton::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer({},false,false);

  outfile->cd();
  cout << "[SkimTree_SSDilepton::initializeAnalyzer()] gDirectory = " << gDirectory->GetName() << endl;
  newtree = fChain->CloneTree(0);

  triggers_dimu.clear();

  triggers.clear();

  if(GetEraShort()=="2016a"){
    triggers_dimu = {
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v",
      "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v",
    };
    
    triggers_emu = {
      "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v",      
      "HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v",
    };
  }

  if(GetEraShort()=="2016b"){
    triggers_dimu = {
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v",
      "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
    };

    triggers_emu = {
      "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v",
      "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"
    };
  }

  if(DataYear==2016){

    triggers_di_el = {
      "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v",
    };

    triggers = {
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v",
      "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v",
      "HLT_IsoMu24_v",
      "HLT_IsoTkMu24_v",
      "HLT_Mu50_v",
      "HLT_TkMu50_v",
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v",
      "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
      "HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v",
      "HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",              // H
      "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v",
      "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v",     // B-G
      "HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v",
      "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v",    // B-G
      "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v",  // H
      "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v",  // H
      "HLT_Ele27_WPTight_Gsf_v",
      "HLT_Ele25_eta2p1_WPTight_Gsf_v" ,
      "HLT_Ele115_CaloIdVT_GsfTrkIdT_v",
      "HLT_Photon175_v",
      "HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50_v",
      "HLT_Photon175_v",
      "HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165_v",
      "HLT_Photon175_v",
      "HLT_DoubleEle33_CaloIdL_MW_v",
      "HLT_DoubleEle37_Ele27_CaloIdL_GsfTrkIdVL_v",
      "HLT_Photon175_v",
      "HLT_DoublePhoton60_v"   
    };
  }
  else if(DataYear==2017){
    triggers_dimu = {
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v",
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v"

    };
    triggers_di_el = {
      "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v"
    };

    triggers_emu = {
      "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v",
      "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v",
    };
    triggers = {
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v",                  // B
      "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v",
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v",
      "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v",
      "HLT_Mu50_v",
      "HLT_TkMu50_v",
      "HLT_IsoMu27_v",
      "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v",      // C-F
      "HLT_Ele32_WPTight_Gsf_v",
      "HLT_Ele32_WPTight_Gsf_L1DoubleEG_v",
      "HLT_Ele115_CaloIdVT_GsfTrkIdT_v",
      "HLT_Photon200_v",
      "HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165_v",
      "HLT_Photon200_v",
      "HLT_DoubleEle33_CaloIdL_MW_v",
      "HLT_Ele35_WPTight_Gsf_v",
      "HLT_Ele32_WPTight_Gsf_v", 
      "HLT_Ele32_WPTight_Gsf_L1DoubleEG_v",
      "HLT_Photon200_v",
      "HLT_DoublePhoton70_v",
      "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v",
      "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v",

    };
  }
  else if(DataYear==2018){
    triggers_dimu = {
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v"
    };
    
    triggers_di_el = {
      "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v"
    };
    triggers_emu = {			 
      "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v",
      "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"
    };


    triggers = {
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v",
      "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v",
      "HLT_Mu50_v",
      "HLT_TkMu50_v",
      "HLT_IsoMu24_v",
      "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v",
      "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v",
      "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v",
      "HLT_Ele32_WPTight_Gsf_v",
      "HLT_DoubleEle25_CaloIdL_MW_v" ,
      "HLT_Ele115_CaloIdVT_GsfTrkIdT_v",
      "HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165_v",
      "HLT_Photon200_v", 
      "HLT_DoublePhoton70_v"
    };
  }
  else{
    cout << "[SkimTree_SSDilepton::initializeAnalyzer] DataYear is wrong : " << DataYear << endl;
  }

  cout << "[SkimTree_SSDilepton::initializeAnalyzer] triggers to skim = " << endl;
  for(unsigned int i=0; i<triggers.size(); i++){
    cout << "[SkimTree_SSDilepton::initializeAnalyzer]   " << triggers.at(i) << endl;
  }

}

void SkimTree_SSDilepton::executeEvent(){

  AnalyzerParameter param_sr = Setup_Param_HNL_ULIDv2("HNL_ULIDv2","MuMu");

  std::vector<Muon>       muonPreColl     = SelectMuons    (param_sr,param_sr.Muon_Veto_ID,     5., 2.4);
  std::vector<Electron>   electronPreColl = SelectElectrons(param_sr,param_sr.Electron_Veto_ID, 10., 2.5);
  
  std::sort(muonPreColl.begin(), muonPreColl.end(), PtComparing);
  std::sort(electronPreColl.begin(), electronPreColl.end(), PtComparing);


  std::vector<Lepton *> leps;
  for(unsigned int i=0; i<electronPreColl.size(); i++){
    Electron& el = electronPreColl.at(i);
    leps.push_back( &el );
  }
  for(unsigned int i=0; i<muonPreColl.size(); i++){
    Muon& mu = muonPreColl.at(i);
    leps.push_back( &mu );
  }

  if (! SameCharge(leps)) return;
  
  //=============================
  //==== If survived, fill tree
  //=============================

  newtree->Fill();

}

void SkimTree_SSDilepton::executeEventFromParameter(AnalyzerParameter param){

}

SkimTree_SSDilepton::SkimTree_SSDilepton(){

  newtree = NULL;

}

SkimTree_SSDilepton::~SkimTree_SSDilepton(){

}

void SkimTree_SSDilepton::WriteHist(){

  outfile->mkdir("recoTree");
  outfile->cd("recoTree");
  newtree->Write();
  outfile->cd();

}
