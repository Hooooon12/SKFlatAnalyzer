#include "HNL_ControlRegionTwo.h"

void HNL_ControlRegionTwo::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

  if(IsDATA){
    bool run_ee_bdt=false;
    bool run_mm_bdt=false;
    bool run_em_bdt=false;

    if (this->DataStream.Contains("DoubleMuon")) run_mm_bdt=true;
    if (this->DataStream.Contains("MuonEG")) run_em_bdt=true;
    if (this->DataStream.Contains("EGamma")) run_ee_bdt=true;
    
    if (this->DataStream == "SingleMuon") run_mm_bdt=true;
    if (this->DataStream == "DoubleEG") run_ee_bdt=true;
    if (this->DataStream == "SingleElectron") run_ee_bdt=true;


    SetupEventMVAReader("V2",run_ee_bdt,run_mm_bdt,run_em_bdt);

  }
  else SetupEventMVAReader("V2");

  nLog = 100000;
}


void HNL_ControlRegionTwo::executeEvent(){

  vector<TString> LepIDs = {"HNL_ULIDv2"};

  vector<HNL_LeptonCore::Channel> ChannelsToRun = {};

  //// Set Individual channel
  if(RunEE)   ChannelsToRun.push_back(EE);
  if(RunMuMu) ChannelsToRun.push_back(MuMu);
  if(RunEMu)  ChannelsToRun.push_back(EMu);
  //   else run all 3 channels
  if(ChannelsToRun.size() == 0)ChannelsToRun = {EE,MuMu,EMu};

  vector<TString> Run_Config = {"ANv4","presel_cleaned_bjet","presel_mod","presel","veto_id_one","veto_id_two","pileup_medium","TauVeto","Pt30"};
  
  ///// Run command 

  vector<TString> CRToRun = {"SS_CR2"};

  if(IsDATA){
    if (this->DataStream.Contains("DoubleMuon")) ChannelsToRun = {MuMu};
    if (this->DataStream.Contains("MuonEG"))     ChannelsToRun = {EMu};
    if (this->DataStream.Contains("EGamma"))    ChannelsToRun = {EE};
    
    if (this->DataStream == "SingleMuon") ChannelsToRun = {MuMu};
    if (this->DataStream == "SingleElectron") ChannelsToRun = {EE};
    if (this->DataStream == "DoubleEG") ChannelsToRun = {EE};
  }

  for (auto id: LepIDs){
    for(auto iconfig : Run_Config){
      for(auto channel : ChannelsToRun){
	
	if(iconfig == "presel_mod"){

	  /// Removes Z peak 
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
	  param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.FatJet_ID = "tightLepVeto";
	  param_cr.Jet_ID = "tightLepVeto";
          param_cr.SRConfig = "PreselectionV2";

          param_cr.BJetColl = "Tight_NoAK8Cleaning";

	  RunControlRegions(param_cr , {"SS_CR2"} );
	  
        }
	else if(iconfig == "presel"){
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.FatJet_ID = "tightLepVeto";
          param_cr.Jet_ID = "tightLepVeto";

          param_cr.BJetColl = "Tight_NoAK8Cleaning";

          RunControlRegions(param_cr , {"SS_CR2"} );
        }
	 else if(iconfig == "presel_cleaned_bjet"){
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.FatJet_ID = "tightLepVeto";
          param_cr.Jet_ID = "tightLepVeto";

	  param_cr.BJetColl = "Tight";

          RunControlRegions(param_cr , {"SS_CR2"} );
        }

	else if(iconfig == "veto_id_one"){
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
	  param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;

	  param_cr.Electron_Veto_ID = "passProbeID";
          param_cr.Muon_Veto_ID = "POGLoose";

          param_cr.FatJet_ID = "tightLepVeto";
          param_cr.Jet_ID = "tightLepVeto";

	  param_cr.BJetColl = "Tight_NoAK8Cleaning";

          RunControlRegions(param_cr , {"SS_CR2"} );
        } 
	else if(iconfig == "veto_id_two"){
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;

          param_cr.Electron_Veto_ID = "passProbeID";
          param_cr.Muon_Veto_ID = "HNLoosePOG";
	  
	  param_cr.FatJet_ID = "tightLepVeto";
          param_cr.Jet_ID = "tightLepVeto";

          param_cr.BJetColl = "Tight_NoAK8Cleaning";
	  RunControlRegions(param_cr , {"SS_CR2"} );

        }

	 else if(iconfig == "pileup_medium"){

          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;

          param_cr.FatJet_ID = "tightLepVeto";
	  param_cr.AK4VBFJetColl = "VBFTightPUM";
          param_cr.Jet_ID = "tightLepVeto";
	  param_cr.JetPUID = "Medium";

	  param_cr.BJetColl = "Tight_NoAK8Cleaning";

          RunControlRegions(param_cr , {"SS_CR2"} );

        }
	
	else if(iconfig == "TauVeto"){

          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;

          param_cr.FatJet_ID     = "tightLepVeto";
          param_cr.Jet_ID = "tightLepVeto";

	  param_cr.Tau_Tight_ID = "JetVL_MuVL_ELVL";
          param_cr.BJetColl = "Tight_NoAK8Cleaning";

          RunControlRegions(param_cr , {"SS_CR2"} );

        }
	else if(iconfig == "Pt30"){
	  AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;

          param_cr.FatJet_ID     = "tightLepVeto";
          param_cr.Jet_ID = "tightLepVeto";

          param_cr.Tau_Tight_ID = "JetVL_MuVL_ELVL";
          param_cr.BJetColl = "Tight_NoAK8Cleaning";

	  param_cr.SRConfig = "SR2_Pt30";
          RunControlRegions(param_cr , {"SS_CR2"} );
        }

	else{
	  AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
	  param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          RunControlRegions(param_cr , {"SS_CR2"} );

	}
      }
    }
  } /// config loop
  return;
}

void HNL_ControlRegionTwo::RunControlRegions(AnalyzerParameter param_cr, vector<TString> CRs){

  run_Debug = (_jentry%nLog==0);

  if(run_Debug) cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  
  Event ev = GetEvent();

  /// SetupWeight applies w_GenNorm=1., w_BR=1., w_PU  w_Pref  
  double weight =SetupWeight(ev,param_cr);

  std::vector<Electron>   ElectronVetoColl = GetElectrons(param_cr.Electron_Veto_ID, 10.,  2.5);
  std::vector<Muon>       MuonVetoColl     = GetMuons    (param_cr.Muon_Veto_ID,     5.,  2.4);

  /// IF ruunning fake then use FR_ID not Tight
  TString Electron_ID = SetLeptonID("Electron",param_cr);
  TString Muon_ID     = SetLeptonID("Muon", param_cr);

  double Min_FakeMuon_Pt     =  5;
  double Min_FakeElectron_Pt =  10 ;
  std::vector<Muon>       MuonTightColl_Init     = SelectMuons    ( param_cr,Muon_ID,     Min_FakeMuon_Pt,     2.4,weight); 
  std::vector<Electron>   ElectronTightColl_Init = SelectElectrons( param_cr,Electron_ID, Min_FakeElectron_Pt, 2.5,weight);

  //// Apply Full Pt cut after pt corrected in fakes  
  double Min_Muon_Pt     =  10.;
  double Min_Electron_Pt =  15;
  std::vector<Muon>       MuonTightColl  = SelectMuons(MuonTightColl_Init,Muon_ID,     Min_Muon_Pt,     2.4);
  std::vector<Electron>   ElectronTightColl = SelectElectrons(ElectronTightColl_Init,Electron_ID, Min_Electron_Pt, 2.5);


  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param_cr.AK8JetColl,param_cr);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param_cr.AK4JetColl,     param_cr);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param_cr.AK4VBFJetColl,  param_cr);

  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param_cr);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param_cr);
  
  EvalJetWeight(AK4_JetColl,AK4_VBF_JetColl, AK8_JetColl, weight, param_cr);

  Particle METv = GetvMET("PuppiT1xyULCorr", param_cr,  MuonTightColl,ElectronTightColl);

  
  vector<int> RunEl ;
  if(RunCF) RunEl =  {0,1} ;
  else RunEl = {-1};


  ///// Scan Tau ID                                              

  std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(MuonVetoColl,ElectronVetoColl);
  
  //// Add check for Taus                                                                                                                                                                                                                                                     
  std::vector<Tau>    TauColl_Cleaned;

  if(!(_jentry % 1000)){
    cout << "Entry " << _jentry << " param : " <<   param_cr.Name << endl;
    cout << "n AK4_JetColl = " << AK4_JetColl.size() << endl;
    for(auto i : AK4_JetColl) {
      i.Print();
      cout << "ChargedEmEnergyFraction = " << i.ChargedEmEnergyFraction() << " MuonEnergyFraction = " << i.MuonEnergyFraction() << endl;
    }
    cout << "n AK4_BJetColl " << AK4_BJetColl.size() <<	endl;
    for(auto i :  AK4_BJetColl ) i.Print();
    cout << "n AK8_JetColl  " << AK8_JetColl.size() << endl;
    for(auto i : AK8_JetColl) i.Print();
    cout << "n ElectronVetoColl " << ElectronVetoColl.size() << endl;
    for(auto i : ElectronVetoColl) i.Print();
    cout << "n MuonVetoColl " << MuonVetoColl.size() << endl;
    for(auto i : MuonVetoColl)i.Print();
      }
  
  
  for(auto ir : RunEl){
    RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl, TauColl_Cleaned,
			 
			 AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
			 ev,METv, param_cr, CRs,ir,weight);
  }
}




HNL_ControlRegionTwo::HNL_ControlRegionTwo(){

  SetupEvMVA();
}
 
HNL_ControlRegionTwo::~HNL_ControlRegionTwo(){

  DeleteEvMVA();
}
