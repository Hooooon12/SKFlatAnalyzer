#include "HNL_ControlRegion.h"

void HNL_ControlRegion::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();
  SetupEventMVAReader("V2");

  nLog = 100000;
}


void HNL_ControlRegion::executeEvent(){

  if(!(_jentry%10000)) run_Debug=true;
  else run_Debug=false;

  ///// LIST IDs to run
  vector<TString> LepIDs = {"HNL_ULID"};//"TopHN","HNL_ULID","HNTightV2"};//,"TopHN", "DefaultPOGTight"};
  //// List Channels to run
  //  vector<HNL_LeptonCore::Channel> ChannelsToRun = {MuMu,EE,EMu};
  vector<HNL_LeptonCore::Channel> ChannelsToRun = {EMu};
  if(RunPromptTLRemoval) ChannelsToRun = {EE};

  vector<TString> RegionsToPlot = {"SSMultiLep","Dilepton"}; 
  
  ///// Run command 

  vector<TString> CRToRun;
  bool StandardProcess = true;
  if(HasFlag("Dilepton"))        CRToRun = {"OS_VR","SS_CR","VBF_CR","LLL_VR"};
  else if(HasFlag("SSMultiLep")) CRToRun = {"SS_CR","VBF_CR","LLL_VR"};
  else if(HasFlag("ScanFakes"))  { StandardProcess=false ; CRToRun = {"SS_CR"};}
  else CRToRun = {"SS_CR"};

  if(StandardProcess){
    for (auto id: LepIDs){
      for(auto channel : ChannelsToRun){
	AnalyzerParameter param_signal = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
	
	for(auto iCR : CRToRun) RunControlRegions(param_signal , {iCR} );
      }
    }
    return;
  }

  if(HasFlag("ScanFakes")){
    
    for (auto id: LepIDs){
      for(auto channel : ChannelsToRun){
	      
	vector<TString> AJetPtEE = {"AJ25","AJ30","AJ40"};
	vector<TString> AJetPt = {"AJ30","AJ40"};

	if(id=="HNL_ULID"){
	  
	  if(channel == MuMu){
	    
	    vector<TString> MuFakeIDs = {"HNL_ULID_FO_v0",
					 "HNL_ULID_FO_v1_a",
					 "HNL_ULID_FO_v1_b",
					 "HNL_ULID_FO_v1_c",
					 "HNL_ULID_FO_v2_a",
                                         "HNL_ULID_FO_v2_b",
                                         "HNL_ULID_FO_v2_c",
					 "HNL_ULID_FO_v3_a",
                                         "HNL_ULID_FO_v3_b",
                                         "HNL_ULID_FO_v3_c",
					 "HNL_ULID_FO_v4_a",
                                         "HNL_ULID_FO_v4_b",
                                         "HNL_ULID_FO_v4_c",
					 "HNL_ULID_FO_v5_a",
                                         "HNL_ULID_FO_v5_b",
                                         "HNL_ULID_FO_v5_c",
					 "HNL_ULID_FO_v6_a",
                                         "HNL_ULID_FO_v6_b",
                                         "HNL_ULID_FO_v6_c",
					 "HNL_ULID_FO_v7_a",
                                         "HNL_ULID_FO_v7_b",
                                         "HNL_ULID_FO_v7_c",
					 "HNL_ULID_FO_v8_a",
                                         "HNL_ULID_FO_v8_b",
                                         "HNL_ULID_FO_v8_c",
					 "HNL_ULID_FO_v9_a",
                                         "HNL_ULID_FO_v9_b",
                                         "HNL_ULID_FO_v9_c"};
	    
	    vector<TString> FakeTag   = {"HNL_ULID_FO_v0",
                                         "HNL_ULID_FO_v1_a",
                                         "HNL_ULID_FO_v1_b",
                                         "HNL_ULID_FO_v1_c",
                                         "HNL_ULID_FO_v2_a",
                                         "HNL_ULID_FO_v2_b",
                                         "HNL_ULID_FO_v2_c",
                                         "HNL_ULID_FO_v3_a",
                                         "HNL_ULID_FO_v3_b",
                                         "HNL_ULID_FO_v3_c",
                                         "HNL_ULID_FO_v4_a",
                                         "HNL_ULID_FO_v4_b",
                                         "HNL_ULID_FO_v4_c",
                                         "HNL_ULID_FO_v5_a",
                                         "HNL_ULID_FO_v5_b",
                                         "HNL_ULID_FO_v5_c",
                                         "HNL_ULID_FO_v6_a",
                                         "HNL_ULID_FO_v6_b",
                                         "HNL_ULID_FO_v6_c",
                                         "HNL_ULID_FO_v7_a",
                                         "HNL_ULID_FO_v7_b",
                                         "HNL_ULID_FO_v7_c",
                                         "HNL_ULID_FO_v8_a",
                                         "HNL_ULID_FO_v8_b",
                                         "HNL_ULID_FO_v8_c",
                                         "HNL_ULID_FO_v9_a",
                                         "HNL_ULID_FO_v9_b",
                                         "HNL_ULID_FO_v9_c"};

	    vector<TString> FakeParam = {"PtParton","Pt"};
	    vector<TString> FakeMethod= {"Standard"};                                                              
	    
	    for(unsigned int i= 0 ; i < FakeTag.size(); i++){
	      for(unsigned int j= 0 ; j < FakeParam.size(); j++){
		for(unsigned int k= 0 ; k < FakeMethod.size(); k++){
		  for(auto iaj : AJetPt){
		    if(!RunFake){
		      if(FakeMethod[k] != "Standard") continue;
		      if(FakeParam[j]  != "PtParton") continue;
		      if(iaj !=  "AJ30") continue;
		      if(FakeTag[i] != "HNL_ULID_FO_v0") continue;
		    }
		    if(FakeMethod[k] == "BDTFlavour" && FakeTag[i] !=  "HNL_ULID_FO") continue;
		    AnalyzerParameter param_signal = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
		    
		    param_signal.PlottingVerbose = -1;
		    param_signal.FakeRateMethod = FakeMethod[k]; 
		    param_signal.FakeRateParam  = FakeParam[j]; 
		    param_signal.Muon_FR_ID     = MuFakeIDs[i] ; 
		    param_signal.k.Muon_FR      = FakeTag[i]+"_"+iaj;
		    param_signal.ApplyPR        = false;
		    param_signal.Name           = param_signal.DefName+"_"+FakeTag[i]+"_"+param_signal.FakeRateName()+"_"+iaj;
		    param_signal.DefName        = param_signal.DefName+"_"+FakeTag[i]+"_"+param_signal.FakeRateName()+"_"+iaj;
		    
		    RunControlRegions(param_signal , CRToRun );

		  }
		}
	      }
	    }
	  }
          else if(channel == EMu){
	    vector<TString> ElFakeIDs = {"HNL_ULID_FO_v0",
                                         "HNL_ULID_FO_v1_a",
                                         "HNL_ULID_FO_v1_b",
                                         "HNL_ULID_FO_v1_c",
                                         "HNL_ULID_FO_v2_a",
                                         "HNL_ULID_FO_v2_b",
                                         "HNL_ULID_FO_v2_c",
                                         "HNL_ULID_FO_v3_a",
                                         "HNL_ULID_FO_v3_b",
                                         "HNL_ULID_FO_v3_c",
                                         "HNL_ULID_FO_v4_a",
                                         "HNL_ULID_FO_v4_b",
                                         "HNL_ULID_FO_v4_c",
                                         "HNL_ULID_FO_v5_a",
                                         "HNL_ULID_FO_v5_b",
                                         "HNL_ULID_FO_v5_c",
                                         "HNL_ULID_FO_v6_a",
                                         "HNL_ULID_FO_v6_b",
                                         "HNL_ULID_FO_v6_c",
                                         "HNL_ULID_FO_v7_a",
                                         "HNL_ULID_FO_v7_b",
                                         "HNL_ULID_FO_v7_c",
                                         "HNL_ULID_FO_v8_a",
                                         "HNL_ULID_FO_v8_b",
                                         "HNL_ULID_FO_v8_c",
                                         "HNL_ULID_FO_v9_a",
                                         "HNL_ULID_FO_v9_b",
                                         "HNL_ULID_FO_v9_c"};

	    vector<TString> FakeTag   = {"HNL_ULID_FO_v0",
                                         "HNL_ULID_FO_v1_a",
                                         "HNL_ULID_FO_v1_b",
                                         "HNL_ULID_FO_v1_c",
                                         "HNL_ULID_FO_v2_a",
                                         "HNL_ULID_FO_v2_b",
                                         "HNL_ULID_FO_v2_c",
                                         "HNL_ULID_FO_v3_a",
                                         "HNL_ULID_FO_v3_b",
                                         "HNL_ULID_FO_v3_c",
                                         "HNL_ULID_FO_v4_a",
                                         "HNL_ULID_FO_v4_b",
                                         "HNL_ULID_FO_v4_c",
                                         "HNL_ULID_FO_v5_a",
                                         "HNL_ULID_FO_v5_b",
                                         "HNL_ULID_FO_v5_c",
                                         "HNL_ULID_FO_v6_a",
                                         "HNL_ULID_FO_v6_b",
                                         "HNL_ULID_FO_v6_c",
                                         "HNL_ULID_FO_v7_a",
                                         "HNL_ULID_FO_v7_b",
                                         "HNL_ULID_FO_v7_c",
                                         "HNL_ULID_FO_v8_a",
                                         "HNL_ULID_FO_v8_b",
                                         "HNL_ULID_FO_v8_c",
                                         "HNL_ULID_FO_v9_a",
                                         "HNL_ULID_FO_v9_b",
                                         "HNL_ULID_FO_v9_c"};
            vector<TString> FakeParam = {"PtParton","Pt"};
            vector<TString> FakeMethod= {"Standard"};


            for(unsigned int i= 0 ; i < FakeTag.size(); i++){
              for(unsigned int j= 0 ; j < FakeParam.size(); j++){
                for(unsigned int k= 0 ; k < FakeMethod.size(); k++){
                  if(FakeMethod[k] == "BDTFlavour" && FakeTag[i] !=  "HNL_ULID_FO") continue;
                  for(auto iaj : AJetPtEE){

                    if(!RunFake){
                      if(FakeMethod[k] != "Standard") continue;
                      if(FakeParam[j]  != "PtParton") continue;
                      if(iaj !=  "AJ30") continue;
                      if(FakeTag[i] != "HNL_ULID_FO_v0") continue;
                    }


                    AnalyzerParameter param_signal = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
                    param_signal.CFMethod   = "DATA";

                    param_signal.PlottingVerbose = -1;
                    param_signal.FakeRateMethod = FakeMethod[k];
                    param_signal.FakeRateParam  = FakeParam[j];

		    param_signal.Muon_FR_ID     = "HNL_ULID_FO" ;
		    
                    param_signal.Electron_FR_ID     = ElFakeIDs[i] ;
                    param_signal.k.Electron_FR      = FakeTag[i]+"_"+iaj;
                    param_signal.ApplyPR        = false;

                    param_signal.Name           = param_signal.DefName+"_"+FakeTag[i]+"_"+param_signal.FakeRateName()+"_"+iaj;
                    param_signal.DefName           = param_signal.DefName+"_"+FakeTag[i]+"_"+param_signal.FakeRateName()+"_"+iaj;
                    RunControlRegions(param_signal , CRToRun );
                  }

		}
	      }
	    }
	  

	  }

	  else if(channel == EE){
	    
	    
	    vector<TString> ElFakeIDs = {"HNL_ULID_FO_v0",
                                         "HNL_ULID_FO_v1_a",
                                         "HNL_ULID_FO_v1_b",
                                         "HNL_ULID_FO_v1_c",
                                         "HNL_ULID_FO_v2_a",
                                         "HNL_ULID_FO_v2_b",
                                         "HNL_ULID_FO_v2_c",
                                         "HNL_ULID_FO_v3_a",
                                         "HNL_ULID_FO_v3_b",
                                         "HNL_ULID_FO_v3_c",
                                         "HNL_ULID_FO_v4_a",
                                         "HNL_ULID_FO_v4_b",
                                         "HNL_ULID_FO_v4_c",
                                         "HNL_ULID_FO_v5_a",
                                         "HNL_ULID_FO_v5_b",
                                         "HNL_ULID_FO_v5_c",
                                         "HNL_ULID_FO_v6_a",
                                         "HNL_ULID_FO_v6_b",
                                         "HNL_ULID_FO_v6_c",
                                         "HNL_ULID_FO_v7_a",
                                         "HNL_ULID_FO_v7_b",
                                         "HNL_ULID_FO_v7_c",
                                         "HNL_ULID_FO_v8_a",
                                         "HNL_ULID_FO_v8_b",
                                         "HNL_ULID_FO_v8_c",
                                         "HNL_ULID_FO_v9_a",
                                         "HNL_ULID_FO_v9_b",
                                         "HNL_ULID_FO_v9_c"};


            vector<TString> FakeTag   = {"HNL_ULID_FO_v0",
                                         "HNL_ULID_FO_v1_a",
                                         "HNL_ULID_FO_v1_b",
                                         "HNL_ULID_FO_v1_c",
                                         "HNL_ULID_FO_v2_a",
                                         "HNL_ULID_FO_v2_b",
                                         "HNL_ULID_FO_v2_c",
                                         "HNL_ULID_FO_v3_a",
                                         "HNL_ULID_FO_v3_b",
                                         "HNL_ULID_FO_v3_c",
                                         "HNL_ULID_FO_v4_a",
                                         "HNL_ULID_FO_v4_b",
                                         "HNL_ULID_FO_v4_c",
                                         "HNL_ULID_FO_v5_a",
                                         "HNL_ULID_FO_v5_b",
                                         "HNL_ULID_FO_v5_c",
                                         "HNL_ULID_FO_v6_a",
                                         "HNL_ULID_FO_v6_b",
                                         "HNL_ULID_FO_v6_c",
                                         "HNL_ULID_FO_v7_a",
                                         "HNL_ULID_FO_v7_b",
                                         "HNL_ULID_FO_v7_c",
                                         "HNL_ULID_FO_v8_a",
                                         "HNL_ULID_FO_v8_b",
                                         "HNL_ULID_FO_v8_c",
                                         "HNL_ULID_FO_v9_a",
                                         "HNL_ULID_FO_v9_b",
                                         "HNL_ULID_FO_v9_c"};

            vector<TString> FakeParam = {"PtParton","Pt"};
            vector<TString> FakeMethod= {"Standard"};
	    
	    
	    for(unsigned int i= 0 ; i < FakeTag.size(); i++){
	      for(unsigned int j= 0 ; j < FakeParam.size(); j++){
		for(unsigned int k= 0 ; k < FakeMethod.size(); k++){
		  if(FakeMethod[k] == "BDTFlavour" && FakeTag[i] !=  "HNL_ULID_FO") continue;
		  for(auto iaj : AJetPtEE){
		    
		    if(!RunFake){
                      if(FakeMethod[k] != "Standard") continue;
                      if(FakeParam[j]  != "PtParton") continue;
                      if(iaj !=  "AJ30") continue;
                      if(FakeTag[i] != "HNL_ULID_FO_v0") continue;
                    }


		    AnalyzerParameter param_signal = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
		    param_signal.CFMethod   = "DATA";
		    
		    param_signal.PlottingVerbose = -1;
		    param_signal.FakeRateMethod = FakeMethod[k];
		    param_signal.FakeRateParam  = FakeParam[j];
		    param_signal.Electron_FR_ID     = ElFakeIDs[i] ;
		    param_signal.k.Electron_FR      = FakeTag[i]+"_"+iaj;
		    param_signal.ApplyPR        = false;
		    
		    param_signal.Name           = param_signal.DefName+"_"+FakeTag[i]+"_"+param_signal.FakeRateName()+"_"+iaj;
		    param_signal.DefName           = param_signal.DefName+"_"+FakeTag[i]+"_"+param_signal.FakeRateName()+"_"+iaj;
		    RunControlRegions(param_signal , CRToRun );
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  
  /// For BDT plots 

  if(HasFlag("BDTCheck")) {
    AnalyzerParameter param_signal = HNL_LeptonCore::InitialiseHNLParameter("SignalStudy");
    vector<TString> IDs = {"HNL_ULID_Baseline"};
    TString param_signal_name = param_signal.Name;
    for (auto id: IDs){
      param_signal.Electron_Tight_ID = id;
      param_signal.Muon_Tight_ID = id;
      param_signal.Name = param_signal_name + id;
      param_signal.DefName = param_signal_name + id;
      param_signal.k.Electron_ID_SF = "Default";
      param_signal.Muon_FR_ID = "HNL_ULID_Baseline";
      param_signal.Electron_FR_ID = "HNL_ULID_Baseline";
      RunControlRegions(param_signal , {"BDTCheck"});

      return;
    }
  }

  return;
  
}

void HNL_ControlRegion::RunControlRegions(AnalyzerParameter param, vector<TString> CRs){


  //  if(_jentry==0) param.PrintParameters();
  run_Debug = (_jentry%nLog==0);

  if(run_Debug) cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  
  Event ev = GetEvent();

  /// SetupWeight applies w_GenNorm=1., w_BR=1., w_PU  w_Pref  
  double weight =SetupWeight(ev,param);
  
  // HL ID                                                                                                                                                   
  std::vector<Electron>   ElectronVetoColl = GetElectrons(param.Electron_Veto_ID, 10.,  2.5);
  std::vector<Muon>       MuonVetoColl     = GetMuons    (param.Muon_Veto_ID,     5.,  2.4);

  /// IF ruunning fake then use FR_ID not Tight
  TString Electron_ID = SetLeptonID("Electron",param);
  TString Muon_ID     = SetLeptonID("Muon", param);

  double Min_Muon_Pt     = (RunFake) ? 5  : 10.;
  double Min_Electron_Pt = (RunFake) ? 10 : 15;

  std::vector<Muon>       MuonTightColl     = SelectMuons    ( param,Muon_ID,     Min_Muon_Pt,     2.4,weight); 
  std::vector<Electron>   ElectronTightColl = SelectElectrons( param,Electron_ID, Min_Electron_Pt, 2.5,weight);

  Particle METv = GetvMET("PuppiT1xyULCorr",param); // returns MET with systematic correction                                                                      

  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);
  
  EvalJetWeight(AK4_JetColl,AK4_VBF_JetColl, AK8_JetColl, weight, param);

  if(CRs.size() == 0) return;
  

  if(HasFlag("BB")){
    std::vector<Lepton *> LepsAll       = MakeLeptonPointerVector(MuonTightColl,ElectronTightColl);
    if(LepsAll.size() == 2){
      if(! (LepsAll[0]->IsBB() && LepsAll[1]->IsBB())) return;
    }
  }
  if(HasFlag("EC")){
    std::vector<Lepton *> LepsAll       = MakeLeptonPointerVector(MuonTightColl,ElectronTightColl);
    if(LepsAll.size() == 2){
      if(! (LepsAll[0]->IsEC() && LepsAll[1]->IsEC())) return;
    }
  }

  vector<int> RunEl ;
  if(RunCF) RunEl =  {0,1} ;
  else RunEl = {-1};

  for(auto ir : RunEl){
    RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl, 
			 AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
			 ev,METv, param, CRs, ir, weight);
  }

}




HNL_ControlRegion::HNL_ControlRegion(){

  SetupEvMVA();
}
 
HNL_ControlRegion::~HNL_ControlRegion(){

  DeleteEvMVA();
}
