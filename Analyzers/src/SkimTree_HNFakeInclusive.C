#include "SkimTree_HNFakeInclusive.h"

void SkimTree_HNFakeInclusive::initializeAnalyzer(){

  outfile->cd();
  cout << "[SkimTree_HNFakeInclusive::initializeAnalyzer()] gDirectory = " << gDirectory->GetName() << endl;
  newtree = fChain->CloneTree(0);

  HNL_LeptonCore::initializeAnalyzer(false,true);
  InitialiseLeptonBDTSKFlat();

  ///// ELECTRON BRANCHES                                                                                                                                                                                                                                                                                                     
  if(!fChain->GetBranch("electron_mva_cf_v2")){
    newtree->Branch("electron_ptrel",      &velectron_ptrel);
    newtree->Branch("electron_ptratio",    &velectron_ptratio);
    newtree->Branch("electron_lepton_type",&velectron_lepton_type);
  }

  newtree->Branch("electron_mva_fake_ed_v5",&velectron_mva_fake_ed_v5);
  newtree->Branch("electron_mva_fakeHFB_v5",&velectron_mva_fakeHFB_v5);
  newtree->Branch("electron_mva_fakeHFC_v5",&velectron_mva_fakeHFC_v5);
  newtree->Branch("electron_mva_fakeLF_v5", &velectron_mva_fakeLF_v5);
  newtree->Branch("electron_mva_conv_ed_v5",&velectron_mva_conv_ed_v5);
  newtree->Branch("electron_mva_cf_ed_v5",  &velectron_mva_cf_ed_v5 );
  newtree->Branch("electron_mva_cf_ed_v5pt",&velectron_mva_cf_ed_v5pt );

  newtree->Branch("electron_mva_fake_QCD_LFvsHF_v5",  &velectron_mva_fake_QCD_LFvsHF_v5);
  newtree->Branch("electron_mva_fake_QCD_HFBvsHFC_v5",&velectron_mva_fake_QCD_HFBvsHFC_v5);
  newtree->Branch("electron_mva_fake_QCD_LF1_v5",     &velectron_mva_fake_QCD_LF1_v5);
  newtree->Branch("electron_mva_fake_QCD_LF2_v5",     &velectron_mva_fake_QCD_LF2_v5);

  newtree->Branch("electron_v2_cj_bjetdisc",&velectron_v2_cj_bjetdisc);
  newtree->Branch("electron_v2_cj_cvsbjetdisc",&velectron_v2_cj_cvsbjetdisc);
  newtree->Branch("electron_v2_cj_cvsljetdisc",&velectron_v2_cj_cvsljetdisc);
  newtree->Branch("electron_v2_cj_flavour", &velectron_v2_cj_flavour);
  //// MUON BRANCH                                                                                                                                                                                                                                                                                                                                                          

  if(!fChain->GetBranch("muon_mva_fake_ed_v4")){
    newtree->Branch("muon_ptrel",         &vmuon_ptrel);
    newtree->Branch("muon_ptratio",       &vmuon_ptratio);
    newtree->Branch("muon_lepton_type",   &vmuon_lepton_type);
    newtree->Branch("SKWeight", &vSKWeight);
  }

  newtree->Branch("muon_mva_fake_QCD_LFvsHF_v5",  &vmuon_mva_fake_QCD_LFvsHF_v5);
  newtree->Branch("muon_mva_fake_QCD_HFBvsHFC_v5",&vmuon_mva_fake_QCD_HFBvsHFC_v5);
  newtree->Branch("muon_mva_fake_QCD_LF1_v5",     &vmuon_mva_fake_QCD_LF1_v5);
  newtree->Branch("muon_mva_fake_QCD_LF2_v5",     &vmuon_mva_fake_QCD_LF2_v5);

  newtree->Branch("muon_v2_cj_bjetdisc",   &vmuon_v2_cj_bjetdisc);
  newtree->Branch("muon_v2_cj_cvsbjetdisc",&vmuon_v2_cj_cvsbjetdisc);
  newtree->Branch("muon_v2_cj_cvsljetdisc"   ,&vmuon_v2_cj_cvsljetdisc);
  newtree->Branch("muon_v2_cj_flavour",    &vmuon_v2_cj_flavour);


  triggers.clear();


  if(DataYear==2016){

    if(IsDATA){
      
      if (this->DataStream == "DoubleMuon"){    
	
	triggers = {
	  "HLT_Mu3_PFJet40_v",                             // DoubleMuon
	  "HLT_Mu8_TrkIsoVVL_v",                           // DoubleMuon
	  "HLT_Mu17_TrkIsoVVL_v",                          // DoubleMuon
	};
      } // // 2016 DMu DATA    
      
      if (this->DataStream == "DoubleEG"){
        triggers = {
	  "HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v",     // DoubleEG
	  "HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v",    // DoubleEG
	  "HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30_v",    // DoubleEG
          //"HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v",        // DoubleEG
	  "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v"     // DoubleEG
	};
      } // 2016EGMu DATA    
    } // 2016 DATA

    else{


      triggers = {
	"HLT_Mu50_v",
        "HLT_Mu3_PFJet40_v",                             // DoubleMuon
        "HLT_Mu8_TrkIsoVVL_v",                           // DoubleMuon
        "HLT_Mu17_TrkIsoVVL_v",                          // DoubleMuon
        "HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v",     // DoubleEG
        "HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v",    // DoubleEG
        "HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30_v",    // DoubleEG
        //"HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v",        // DoubleEG
        "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v"     // DoubleEG
      };

    } // 2016 MC
    
  } // 2016


  else if(DataYear==2017){
    

    if(IsDATA){
      
      if (this->DataStream == "SingleMuon"){
	triggers = { "HLT_Mu3_PFJet40_v","HLT_Mu50_v"};
	
      } // 2017 DATA SMu

      if (this->DataStream == "SingleElectron"){
	
	triggers = {
	  "HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v",     // SingleElectron
	  "HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v",    // SingleElectron
	  //"HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v",        // SingleElectron
	  "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v"     // SingleElectron
	};
      }// 2017 DATA SEl       

      if (this->DataStream == "DoubleMuon"){

	triggers = {
          "HLT_Mu8_TrkIsoVVL_v",                           // DoubleMuon                                                                
          "HLT_Mu17_TrkIsoVVL_v",                          // DoubleMuon                                                                
        };

      } // 2017 DATA DMu       
    } // 2017
    
    else {
      
      
      triggers = {
	"HLT_Mu50_v",
	"HLT_Mu3_PFJet40_v",                             // SingleMuon
	"HLT_Mu8_TrkIsoVVL_v",                           // DoubleMuon
	"HLT_Mu17_TrkIsoVVL_v",                          // DoubleMuon
	"HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v",     // SingleElectron
	"HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v",    // SingleElectron
	//"HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v",        // SingleElectron
	"HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v"     // SingleElectron
      };
      cout << "Filling 2017 trigger " << endl;

    } // 2017 MC
    
  }
  else if(DataYear==2018){
    if(IsDATA){
   
      if (this->DataStream == "SingleMuon"){
      
	triggers = {"HLT_Mu3_PFJet40_v","HLT_Mu50_v"};
	
      }
      if (this->DataStream == "EGamma"){

        triggers = {
          "HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v", 
          "HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v",
          //"HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v",      
          "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v" 
        };
      }
      if (this->DataStream == "DoubleMuon"){
	triggers = {
	  "HLT_Mu8_TrkIsoVVL_v", 
	  "HLT_Mu17_TrkIsoVVL_v",
	};
	
      }
    } // 2018 DATA
    else {
      

      triggers = {
        "HLT_Mu50_v",
	"HLT_Mu3_PFJet40_v",                             // SingleMuon
	"HLT_Mu8_TrkIsoVVL_v",                           // DoubleMuon
	"HLT_Mu17_TrkIsoVVL_v",                          // DoubleMuon
	"HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v",     // EGamma
	"HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v",    // EGamma
	//"HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v",        // EGamma
	"HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v"     // EGamma
      };

    }

  }
  else{
    cout<<"[SkimTree_HNFakeInclusive::initializeAnalyzer] DataYear is wrong : " << DataYear << endl;
  }


  cout << "[SkimTree_HNFakeInclusive::initializeAnalyzer] triggers to skim = " << endl;
  for(unsigned int i=0; i<triggers.size(); i++){
    cout << "[SkimTree_HNFakeInclusive::initializeAnalyzer]   " << triggers.at(i) << endl;
  }

}

void SkimTree_HNFakeInclusive::executeEvent(){


  if(!fChain->GetBranch("electron_mva_cf_v2"))  ResetLeptonBDTSKFlat();
  ResetLeptonBDTSKFlatV5();

  vSKWeight=MCweight(true,true);

  if(!fChain->GetBranch("electron_mva_cf_v2"))SetupLeptonBDTSKFlat();
  SetupLeptonBDTSKFlatV5();

  newtree->Fill();

  
  return;

}

void SkimTree_HNFakeInclusive::executeEventFromParameter(AnalyzerParameter param){

}

SkimTree_HNFakeInclusive::SkimTree_HNFakeInclusive(){
  newtree=NULL;
}

SkimTree_HNFakeInclusive::~SkimTree_HNFakeInclusive(){

}

void SkimTree_HNFakeInclusive::WriteHist(){

  outfile->mkdir("recoTree");
  outfile->cd("recoTree");
  newtree->Write();
  outfile->cd();

}
