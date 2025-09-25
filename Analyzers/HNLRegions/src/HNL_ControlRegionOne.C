#include "HNL_ControlRegionOne.h"

void HNL_ControlRegionOne::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();
  nLog = 100000;
}

void HNL_ControlRegionOne::executeEvent(){

  vector<TString> LepIDs = {"HNL_ULIDv2"};

  vector<HNL_LeptonCore::Channel> ChannelsToRun = {};

  //// Set Individual channel
  if(RunEE)   ChannelsToRun.push_back(EE);
  if(RunMuMu) ChannelsToRun.push_back(MuMu);
  if(RunEMu)  ChannelsToRun.push_back(EMu);
  //   else run all 3 channels
  if(ChannelsToRun.size() == 0)ChannelsToRun = {EE,MuMu,EMu};

  vector<TString> Run_Config = {"ANv4","AK8_Veto1","AK8_Veto2","AK8_Veto3", "presel_mod","presel","bjet_noak8","veto_id_one","veto_id_two","bjet_noak8_hnlveto", "TauVeto","TauVeto2","TauVeto3","NoOverlap","presel_bjetlepclean","presel_bjetmod","presel_ak8_tight","presel_bjetmod_ak8_tight","presel_bjetmodx"};

  //Run_Config = {"presel"};
  
  ///// Run command 

  vector<TString> CRToRun = {"SS_CR1"};

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
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
	  param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";
	  param_cr.BJetColl = "Tight";

	  RunControlRegions(param_cr , {"SS_CR1"} );

        }
	else if(iconfig == "presel_bjetmod"){
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";
          param_cr.BJetColl = "Tight_LV_NoAK8Cleaning";
          RunControlRegions(param_cr , {"SS_CR1"} );
        }
	else if(iconfig == "presel_bjetmodx"){
	  AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";
          param_cr.BJetColl = "Tight_xLV_NoAK8Cleaning";
          RunControlRegions(param_cr , {"SS_CR1"} );
        }

	else if(iconfig == "presel"){
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";
          param_cr.BJetColl = "Tight_NoAK8Cleaning";
          RunControlRegions(param_cr , {"SS_CR1"} );
        }
	else if(iconfig == "presel_bjetmod_ak8_tight"){
	  AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tight";
          param_cr.BJetColl = "Tight_LV_NoAK8Cleaning";
          RunControlRegions(param_cr , {"SS_CR1"} );
        }
        else if(iconfig == "presel_ak8_tight"){
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tight";
          param_cr.BJetColl = "Tight_NoAK8Cleaning";
          RunControlRegions(param_cr , {"SS_CR1"} );
        }
	else if(iconfig == "presel_bjetlepclean"){
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";
          param_cr.BJetColl = "TightLepCleaned";
          RunControlRegions(param_cr , {"SS_CR1"} );
        }
	 else if(iconfig == "NoOverlap"){
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";
          param_cr.BJetColl = "Tight_NoAK8Cleaning";

	  param_cr.SRConfig = "NoOverlap";
          RunControlRegions(param_cr , {"SS_CR1"} );
        }

	else if(iconfig == "veto_id_one"){
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
	  param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Electron_Veto_ID = "passProbeID";
          param_cr.Muon_Veto_ID = "POGLoose";

          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";

	  param_cr.BJetColl = "Tight_NoAK8Cleaning";

          RunControlRegions(param_cr , {"SS_CR1"} );
        }
	else if(iconfig == "veto_id_two"){
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;

	  param_cr.Electron_Veto_ID = "passVetoID";

          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";

	  param_cr.BJetColl = "Tight_NoAK8Cleaning";

          RunControlRegions(param_cr , {"SS_CR1"} );

        }
	else if(iconfig == "bjet_noak8"){
  
          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
	  param_cr.Name = param_cr.Name + "_"+iconfig;
	  param_cr.DefName = param_cr.DefName + "_"+iconfig;
	  
	  param_cr.Jet_ID = "tightLepVeto";
	  param_cr.FatJet_ID = "tightLepVeto";
	  
	  param_cr.BJetColl = "Tight_NoAK8Cleaning";
	  
          RunControlRegions(param_cr , {"SS_CR1"} );
	}

       else if(iconfig == "bjet_noak8_hnlveto"){

          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
	  
          param_cr.BJetColl = "Tight_NoAK8Cleaning";

          RunControlRegions(param_cr , {"SS_CR1"} );
        }
       else if(iconfig == "TauVeto"){

          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;

	  param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";

          param_cr.BJetColl = "Tight_NoAK8Cleaning";
	  param_cr.Tau_Tight_ID = "JetVL_MuVL_ELVL";
	  
          RunControlRegions(param_cr , {"SS_CR1"} );
        }
	else if(iconfig == "TauVeto2"){

          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;

          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";

          param_cr.BJetColl = "Tight_NoAK8Cleaning";
          param_cr.Tau_Tight_ID = "JetVL_MuVL_ELVL";

          param_cr.SRConfig = "NoTauAK8Clean";
	  
          RunControlRegions(param_cr , {"SS_CR1"} );
        }
	else if(iconfig == "TauVeto3"){

          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;

          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";

          param_cr.BJetColl = "Tight";
          param_cr.Tau_Tight_ID = "JetVL_MuVL_ELVL";

          param_cr.SRConfig = "NoTauAK8Clean";

          RunControlRegions(param_cr , {"SS_CR1"} );
        }


	else if(iconfig == "AK8_Veto1"){


          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";
          param_cr.BJetColl = "Tight";

	  param_cr.SRConfig = "AK8LooseVeto1";

          RunControlRegions(param_cr , {"SS_CR1"} );

	  
	}
	  else if(iconfig == "AK8_Veto2"){


          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";
          param_cr.BJetColl = "Tight";

          param_cr.SRConfig = "AK8LooseVeto2";

          RunControlRegions(param_cr , {"SS_CR1"} );


        }
	  else if(iconfig == "AK8_Veto3"){
    

          AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID = "tightLepVeto";
          param_cr.FatJet_ID = "tightLepVeto";
          param_cr.BJetColl = "Tight";

          param_cr.SRConfig = "AK8LooseVeto3";

          RunControlRegions(param_cr , {"SS_CR1"} );

        }

	else{
	  AnalyzerParameter param_cr= Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
	  param_cr.Name = param_cr.Name + "_"+iconfig;
          param_cr.DefName = param_cr.DefName + "_"+iconfig;
          param_cr.Jet_ID    = "tight";
          param_cr.FatJet_ID = "tight";
          param_cr.BJetColl  = "Tight";
	  param_cr.FatJet_CleaningColl = "HNL_PN";
          RunControlRegions(param_cr , {"SS_CR1"} );
	}
      }
    }
  } /// config loop
  return;
}

void HNL_ControlRegionOne::RunControlRegions(AnalyzerParameter param_cr, vector<TString> CRs){

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

  std::vector<Jet>    AK4_BJetColl2               = GetHNLJets("BJet2", param_cr);
  std::vector<Jet>    AK4_BJetColl3               = GetHNLJets("BJet3", param_cr);
  std::vector<Jet>    AK4_BJetColl4               = GetHNLJets("BJet4", param_cr);
  std::vector<Jet>    AK4_BJetColl5               = GetHNLJets("BJet5", param_cr);

  std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(MuonVetoColl,ElectronVetoColl);

  if(param_cr.SRConfig == "AK8LooseVeto1"){
    std::vector<FatJet> AK8_LooseJetColl          = GetHNLAK8Jets("HNL_Loose",param_cr);
    if(AK8_LooseJetColl.size() > 1) return;
  }
  if(param_cr.SRConfig == "AK8LooseVeto2"){
    std::vector<FatJet> AK8_LooseJetColl          = GetHNLAK8Jets("HNL",param_cr);
    if(AK8_LooseJetColl.size() > 1) return;
  }
  if(param_cr.SRConfig == "AK8LooseVeto3"){
    std::vector<FatJet> AK8_LooseJetColl          = GetHNLAK8Jets("HNL_NoMass",param_cr);
    if(AK8_LooseJetColl.size() > 1) return;
  }
  
  /*
  int sb1 = AK4_BJetColl.size();
  int sb2 = AK4_BJetColl2.size();
  int sb3 = AK4_BJetColl3.size();
  int sb4 = AK4_BJetColl4.size();
  int sb5 = AK4_BJetColl5.size();
 
  bool check_Ev=false;
  for(auto ijet : AK4_BJetColl){
    for(auto ilep : leps_veto) {
      if(ilep->DeltaR(ijet) < 0.4){
	cout << "Lep BJet dr = " << ilep->DeltaR(ijet) << endl;
	check_Ev=true;
      }
    }
  }
  
  if (check_Ev){
    //&& sb2 != sb3 || sb2 != sb4 || sb2 != sb5) {
    
    cout << event  << " : " << param_cr.Name << "  AK4_BJetColl = " << AK4_BJetColl.size() << " param.Jet_ID = " << param_cr.Jet_ID << endl;
    cout << "AK4_BJetColl1.size = " << sb1 << endl;
    cout << "AK4_BJetColl2.size = " << sb2 << endl;
    cout << "AK4_BJetColl3.size = " << sb3 << endl;
    cout << "AK4_BJetColl4.size = " << sb4 << endl;
    cout << "AK4_BJetColl5.size = " << sb5 << endl;
    std::vector<FatJet> AK8L_JetColl             = GetHNLAK8Jets("Loose",param_cr);
    cout << "AK8L_JetColl size = " << AK8L_JetColl.size() << endl;
    cout << "AK8_JetColl size = " << AK8_JetColl.size() << endl;
    for(auto ijet : AK4_BJetColl2){
      for(auto ifjet : AK8L_JetColl) {
	cout << "AK8 BJet dr = " << ifjet.DeltaR(ijet) << endl;
      }
      cout << "TightID LepV pass = " << ijet.PassID("tightLepVeto") << endl;
      for(auto ilep : leps_veto) cout << "Lep BJet dr = " << ilep->DeltaR(ijet) << endl;

    }
  }
  */
  EvalJetWeight(AK4_JetColl,AK4_VBF_JetColl, AK8_JetColl, weight, param_cr);

  Particle METv = GetvMET("PuppiT1xyULCorr", param_cr, MuonTightColl,ElectronTightColl);

  
  vector<int> RunEl ;
  if(RunCF) RunEl =  {0,1} ;
  else RunEl = {-1};


  ///// Scan Tau ID                                              

  
  //// Add check for Taus

  vector<Tau> TauColl_Cleaned;
  if(param_cr.SRConfig == "NoTauAK8Clean")     TauColl_Cleaned= SelectTaus   (leps_veto, AK8_JetColl, param_cr.Tau_Tight_ID,20., 2.3,false);
  else TauColl_Cleaned= SelectTaus   (leps_veto, AK8_JetColl, param_cr.Tau_Tight_ID,20., 2.3);


  if(param_cr.SRConfig == "NoOverlap")    {

    bool HasOverlap = false;
    for( auto ilep : leps_veto){
      for(auto ij : AK8_JetColl){
	if(ilep->DeltaR(ij) < 0.2) HasOverlap=true;
      }
      for(auto ij : AK4_VBF_JetColl){
	if(ilep->DeltaR(ij) < 0.2) HasOverlap=true;
      }
    }
    if(leps_veto.size() > 1){
      if(leps_veto[0]->DeltaR(*leps_veto[1]) < 0.2) HasOverlap=true;
    }
    if(HasOverlap) return;
  }

  for(auto ir : RunEl){
    RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl, TauColl_Cleaned,
			 
			 AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
			 ev,METv, param_cr, CRs,ir,weight);
  }
}




HNL_ControlRegionOne::HNL_ControlRegionOne(){

  SetupEvMVA();
}
 
HNL_ControlRegionOne::~HNL_ControlRegionOne(){

  DeleteEvMVA();
}
