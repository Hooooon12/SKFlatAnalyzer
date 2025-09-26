#include "HNL_SignalRegion_TestRun.h"

void HNL_SignalRegion_TestRun::initializeAnalyzer(){

  // All default settings like trigger/ PD/ BJet are decalred in HNL_LeptonCore::initializeAnalyzer to make them consistent for all HNL codes

  BDTVersions_to_run ={"V3","V4"};

  HNL_LeptonCore::initializeAnalyzer(BDTVersions_to_run);

  
  if(IsDATA){
    bool run_ee_bdt=false;    bool run_mm_bdt=false;    bool run_em_bdt=false;

    if (this->DataStream.Contains("DoubleMuon")) run_mm_bdt=true;
    if (this->DataStream.Contains("MuonEG"))     run_em_bdt=true;
    if (this->DataStream.Contains("EGamma"))     run_ee_bdt=true;

    if (this->DataStream == "SingleMuon")     run_mm_bdt=true;
    if (this->DataStream == "DoubleEG")       run_ee_bdt=true;
    if (this->DataStream == "SingleElectron") run_ee_bdt=true;
    
    SetupEventMVAReader("V3",run_ee_bdt,run_mm_bdt,run_em_bdt);

  }
  else SetupEventMVAReader("V3");

  nLog = 100000;
}


void HNL_SignalRegion_TestRun::executeEvent(){

  FillTimer("START_EV");
  
  if(_jentry == 0)    cout << "HNL_SignalRegion_TestRun::IsData = " << IsData << endl;

  vector<TString> Run_Config = {"Default","SR1_Pt15","SR1_Pt20","SR2_Pt15","SR2_Pt20","SR3_Cut20","SR3_Cu0t30","SR3_Cut40","SR3_Cut50"};
  
  // Default ID setting
  vector<TString> LepIDs = {"HNL_ULIDv2"};
  
  vector<HNL_LeptonCore::Channel> ChannelsToRun= {EE, MuMu, EMu};

  
  /// Match Channel to data stream
  if (IsDATA) {
    const auto& ds = this->DataStream;
    
    if (ds.Contains("DoubleMuon") || ds == "SingleMuon") {
      ChannelsToRun = {MuMu};
    }
    else if (ds.Contains("EGamma") || ds == "DoubleEG" || ds == "SingleElectron") {
      ChannelsToRun = {EE};
    }
    else if (ds.Contains("MuonEG")) {
      ChannelsToRun = {EMu};
    }
  }
  
  // Match Channel to signal process based on MC sample
  if (IsSignal()){
    if (MCSample.Contains("SSWWTypeI_DF")) {
      ChannelsToRun = {EMu};
    }
    else if (MCSample.Contains("SSWWTypeI_SF")) {
      ChannelsToRun = {EE, MuMu};
    }
    else {
      ChannelsToRun = {EE, MuMu, EMu};
    }
  }
  
  for (auto id: LepIDs){
    for(auto iconfig : Run_Config){
      for(auto channel : ChannelsToRun){
	
	if(IsSignal() && !SelectChannel(channel)) continue;


	if(iconfig=="SR1_Pt15"){

          AnalyzerParameter param_sr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_sr.Name = param_sr.Name + "_"+iconfig;
          param_sr.DefName = param_sr.DefName + "_"+iconfig;

	  param_sr.SRConfig = "SR1_Pt15";

	  RunULAnalysis(param_sr);

	}
	else if(iconfig=="SR1_Pt20"){
	  
          AnalyzerParameter param_sr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_sr.Name = param_sr.Name + "_"+iconfig;
          param_sr.DefName = param_sr.DefName + "_"+iconfig;

          param_sr.SRConfig = "SR1_Pt20";

          RunULAnalysis(param_sr);

        }
	 else if(iconfig=="SR2_Pt15"){

          AnalyzerParameter param_sr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_sr.Name = param_sr.Name + "_"+iconfig;
          param_sr.DefName = param_sr.DefName + "_"+iconfig;

          param_sr.SRConfig = "SR2_P15";

          RunULAnalysis(param_sr);

        }
	 else if(iconfig=="SR2_Pt20"){
	   
          AnalyzerParameter param_sr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_sr.Name = param_sr.Name + "_"+iconfig;
          param_sr.DefName = param_sr.DefName + "_"+iconfig;

          param_sr.SRConfig = "SR2_P20";

          RunULAnalysis(param_sr);

        }
	 else if(iconfig=="SR3_Cut20"){
	      
          AnalyzerParameter param_sr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_sr.Name = param_sr.Name + "_"+iconfig;
          param_sr.DefName = param_sr.DefName + "_"+iconfig;

          param_sr.SRConfig = "SR3_Cut20";

          RunULAnalysis(param_sr);

        }
	 else if(iconfig=="SR3_Cut30"){

	   AnalyzerParameter param_sr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
	   param_sr.Name = param_sr.Name + "_"+iconfig;
	   param_sr.DefName = param_sr.DefName + "_"+iconfig;
	   
	   param_sr.SRConfig = "SR3_Cut30";
	   
	   RunULAnalysis(param_sr);
	   
        }
	 else if(iconfig=="SR3_Cut40"){

          AnalyzerParameter param_sr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_sr.Name = param_sr.Name + "_"+iconfig;
          param_sr.DefName = param_sr.DefName + "_"+iconfig;

          param_sr.SRConfig = "SR3_Cut40";

          RunULAnalysis(param_sr);

        }

	 else if(iconfig=="SR3_Cut50"){

          AnalyzerParameter param_sr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_sr.Name = param_sr.Name + "_"+iconfig;
          param_sr.DefName = param_sr.DefName + "_"+iconfig;

          param_sr.SRConfig = "SR3_Cut50";

          RunULAnalysis(param_sr);

        }



	else{
	  //// Central run...
	  AnalyzerParameter param_sr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
          param_sr.Name = param_sr.Name + "_"+iconfig;
          param_sr.DefName = param_sr.DefName + "_"+iconfig;
	  RunULAnalysis(param_sr);

	}
      }
    }
  }
  FillTimer("END_EV");

  return ;
}

void HNL_SignalRegion_TestRun::RunULAnalysis(AnalyzerParameter param_sr){

  if(_jentry< 1) cout << "HNL_SignalRegion_TestRun::executeEvent " << param_sr.Name <<  " " << param_sr.Channel << endl;
  
  Event ev = GetEvent();
  double weight =SetupWeight(ev,param_sr);
  
  // HL ID
  std::vector<Electron>   ElectronCollV = SelectElectrons(param_sr,param_sr.Electron_Veto_ID, 10., 2.5); 
  std::vector<Muon>       MuonCollV     = SelectMuons    (param_sr,param_sr.Muon_Veto_ID,     5., 2.4);
  
  TString el_ID = SetLeptonID("Electron",param_sr);
  TString mu_ID = SetLeptonID("Muon", param_sr);
  
  if(param_sr.syst_ == AnalyzerParameter::ScaleUp)   weight *= GetScaleUncertainty(1);
  if(param_sr.syst_ == AnalyzerParameter::ScaleDown) weight *= GetScaleUncertainty(-1);

  double Min_FakeMuon_Pt     =  5;   double Min_FakeElectron_Pt =  10 ;
  std::vector<Muon>       MuonTightColl_Init     = SelectMuons    ( param_sr,mu_ID,     Min_FakeMuon_Pt, 2.4, weight);
  std::vector<Electron>   ElectronTightColl_Init = SelectElectrons( param_sr,el_ID, Min_FakeElectron_Pt, 2.5, weight);

  //// Apply Full Pt cut after pt corrected in fakes                                                                            
  double Min_Muon_Pt     =  10.;   double Min_Electron_Pt =  15;
  std::vector<Muon>       MuonCollT  = SelectMuons(MuonTightColl_Init,mu_ID,     Min_Muon_Pt,     2.4);
  std::vector<Electron>   ElectronCollT = SelectElectrons(ElectronTightColl_Init,el_ID, Min_Electron_Pt, 2.5);

  // create lepton collection
  std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(MuonCollV,ElectronCollV);

  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param_sr.AK8JetColl,  param_sr);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param_sr.AK4JetColl,     param_sr);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param_sr.AK4VBFJetColl,  param_sr);

  //// Alternate jet samples 
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",param_sr);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param_sr);

  Particle METv = GetvMET("PuppiT1xyULCorr", param_sr, MuonCollT, ElectronCollT); // returns MET with systematic correction; run this after all object selection done; NOTE that VBF jet is used here
  
  EvalJetWeight(AK4_JetColl,AK4_VBF_JetColl, AK8_JetColl, weight, param_sr);

  FillTimer("START_SR");

  vector<int> RunEl ;
  if(RunCF) RunEl =  {0,1} ;
  else RunEl = {-1};


  /////////// RUN ANALYSE LOOP
  
  ///// Scan Tau ID

  //// Add check for Taus                                                                                                                                                                                         
  std::vector<Tau>  TauColl_Cleaned= SelectTaus   (leps_veto, AK8_JetColl, param_sr.Tau_Tight_ID,20., 2.3,false);

  
  
  for(auto ir : RunEl){
    
    RunAllSignalRegions(Inclusive,
			ElectronCollT,ElectronCollV,MuonCollT,MuonCollV,  TauColl_Cleaned,
			AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
			ev,METv, param_sr, ir, weight);
  }
  
  FillTimer("END_SR");


}
 

HNL_SignalRegion_TestRun::HNL_SignalRegion_TestRun(){

  cout << "HNL_SignalRegion_TestRun::HNL_SignalRegion_TestRun  TMVA::Tools::Instance() " << endl;
  SetupEvMVA();
  
}
 
HNL_SignalRegion_TestRun::~HNL_SignalRegion_TestRun(){

  DeleteEvMVA();

}




