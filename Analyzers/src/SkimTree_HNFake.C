#include "SkimTree_HNFake.h"

void SkimTree_HNFake::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer(false,false);

  outfile->cd();
  cout << "[SkimTree_HNFake::initializeAnalyzer()] gDirectory = " << gDirectory->GetName() << endl;
  newtree = fChain->CloneTree(0);

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
      
      if (this->DataStream == "SingleMuon"){

        triggers = {
	  "HLT_IsoMu24_v",
	  "HLT_IsoTkMu24_v"
	};
      }
      if (this->DataStream == "SingleElectron"){
	triggers = {
	  "HLT_Ele27_WPTight_Gsf_v",
	};
      }

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
        "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v",     // DoubleEG
	"HLT_IsoMu24_v",
	"HLT_IsoTkMu24_v",
	"HLT_Ele27_WPTight_Gsf_v"
      };

    } // 2016 MC
    
  } // 2016


  else if(DataYear==2017){
    

    if(IsDATA){
      
      if (this->DataStream == "SingleMuon"){
	triggers = { "HLT_Mu3_PFJet40_v","HLT_Mu50_v" ,  "HLT_IsoMu24_v", "HLT_IsoMu27_v"};
	
      } // 2017 DATA SMu

      if (this->DataStream == "SingleElectron"){
	
	triggers = {
	  "HLT_Ele27_WPTight_Gsf_v",
	  "HLT_Ele28_WPTight_Gsf_v",
	  "HLT_Ele32_WPTight_Gsf_v",
	  "HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v",     // SingleElectron
	  "HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v",    // SingleElectron
	  //"HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v",        // SingleElectron
	  "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v",     // SingleElectron
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
	"HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v",     // SingleElectron
	"HLT_IsoMu24_v", 
	"HLT_IsoMu27_v",
	"HLT_Ele27_WPTight_Gsf_v",
	"HLT_Ele28_WPTight_Gsf_v",
	"HLT_Ele32_WPTight_Gsf_v"
	
      };
      cout << "Filling 2017 trigger " << endl;

    } // 2017 MC
    
  }
  else if(DataYear==2018){
    if(IsDATA){
   
      if (this->DataStream == "SingleMuon"){
      
	triggers = {"HLT_Mu3_PFJet40_v","HLT_Mu50_v", "HLT_IsoMu24_v"};
	
      }
      if (this->DataStream == "EGamma"){

        triggers = {
	  "HLT_Ele27_WPTight_Gsf_v",
	  "HLT_Ele28_WPTight_Gsf_v",
	  "HLT_Ele32_WPTight_Gsf_v",
          "HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v", 
          "HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v",
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
	"HLT_IsoMu24_v",
        "HLT_Mu50_v",
	"HLT_Mu3_PFJet40_v",                             // SingleMuon
	"HLT_Mu8_TrkIsoVVL_v",                           // DoubleMuon
	"HLT_Mu17_TrkIsoVVL_v",                          // DoubleMuon
	"HLT_Ele27_WPTight_Gsf_v",
	"HLT_Ele28_WPTight_Gsf_v",
	"HLT_Ele32_WPTight_Gsf_v",
	"HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v",     // EGamma
	"HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v",    // EGamma
	//"HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v",        // EGamma
	"HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v"     // EGamma
      };

    }

  }
  else{
    cout<<"[SkimTree_HNFake::initializeAnalyzer] DataYear is wrong : " << DataYear << endl;
  }


  cout << "[SkimTree_HNFake::initializeAnalyzer] triggers to skim = " << endl;
  for(unsigned int i=0; i<triggers.size(); i++){
    cout << "[SkimTree_HNFake::initializeAnalyzer]   " << triggers.at(i) << endl;
  }

}

void SkimTree_HNFake::executeEvent(){

  Event ev;
  ev.SetTrigger(*HLT_TriggerName);

  if(!(ev.PassTrigger(triggers))) return;

  double mu_pt = (IsData) ? 4: 4;
  double el_pt = (IsData) ? 8: 8;

  vector<Muon> allmuons  = GetMuons("HNLoosest",     mu_pt, 2.4);
  vector<Electron> allel = GetElectrons("HNLoosest", el_pt, 2.5);
  
  int NLep = allmuons.size() + allel.size();

  if( NLep == 0 ) return;  

  vector<Lepton*> Leptons = MakeLeptonPointerVector(allmuons,allel);
  bool HasOSZCand = (ZmassOSSFWindowCheck(Leptons,20)); 

  Particle METv = GetMiniAODvMET("PuppiT1xyULCorr"); // returns MET with systematic correction                                                                      
  bool HighMET = false;

  vector<Jet> alljet = GetJets("tight", 20., 2.7);
  bool dphi_lj(false);
  bool dR_lj(false);
  bool PassPt(false);

  for(unsigned int imu=0; imu < allmuons.size(); imu++){
    if(allmuons[imu].Pt() > 30) PassPt = true;
    if((METv.Pt() > 25) && MT(allmuons[imu], METv) > 60) HighMET = true;
    else{
      for(unsigned int ij=0; ij <alljet.size(); ij++){
	float dphi =fabs(TVector2::Phi_mpi_pi(allmuons[imu].Phi()- alljet.at(ij).Phi()));
	float dR = allmuons[imu].DeltaR(alljet.at(ij));
	if(dphi > 2.5) dphi_lj=true;
	if(dR > 0.7) dR_lj=true;
      }
    }
  }
  
  for(unsigned int iel=0; iel <allel.size(); iel++){
    if(allel[iel].Pt() > 30) PassPt = true;
    if((METv.Pt() > 25) && MT(allel[iel], METv) > 60) HighMET = true;
    else{ 
      for(unsigned int ij=0; ij <alljet.size(); ij++){
	float dphi =fabs(TVector2::Phi_mpi_pi(allel[iel].Phi()- alljet.at(ij).Phi()));
	float dR = allel[iel].DeltaR(alljet.at(ij));
	if(dR > 0.7) dR_lj=true;
	if(dphi >2.5) dphi_lj=true;
      }
    }
  }

  /// Fill If it is either QCD Back-to-back , Z peak, or Prompt Norm cand
  if( dphi_lj )   newtree->Fill();
  else if(dR_lj)  newtree->Fill();
  else if(HasOSZCand) newtree->Fill();
  else if(HighMET) newtree->Fill();
  else if(NLep==1&&PassPt) newtree->Fill();
  return;





}

void SkimTree_HNFake::executeEventFromParameter(AnalyzerParameter param){

}

SkimTree_HNFake::SkimTree_HNFake(){
  newtree=NULL;
}

SkimTree_HNFake::~SkimTree_HNFake(){

}

void SkimTree_HNFake::WriteHist(){

  outfile->mkdir("recoTree");
  outfile->cd("recoTree");
  newtree->Write();
  outfile->cd();

}
