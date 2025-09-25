#include "HNL_ControlRegion_Plotter.h"

void HNL_ControlRegion_Plotter::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();
  
  /// Select BDT versions to plot                                                                                                       
  BDTVersions_to_run ={"V3","V4"};


  if(IsDATA){
    bool run_ee_bdt=false;  bool run_mm_bdt=false;  bool run_em_bdt=false;

    if (this->DataStream.Contains("DoubleMuon")) run_mm_bdt=true;
    if (this->DataStream.Contains("MuonEG")) run_em_bdt=true;
    if (this->DataStream.Contains("EGamma")) run_ee_bdt=true;
    
    if (this->DataStream == "SingleMuon") run_mm_bdt=true;
    if (this->DataStream == "DoubleEG") run_ee_bdt=true;
    if (this->DataStream == "SingleElectron") run_ee_bdt=true;


    SetupEventMVAReaders(BDTVersions_to_run,run_ee_bdt,run_mm_bdt,run_em_bdt);

  }
  else SetupEventMVAReaders(BDTVersions_to_run);
}


void HNL_ControlRegion_Plotter::executeEvent(){

  if(!(_jentry%10000)) run_Debug=true;
  else run_Debug=false;

  vector<TString> LepIDs = {"HNL_ULIDv2"};

  vector<HNL_LeptonCore::Channel> ChannelsToRun = {EE,MuMu,EMu};

  vector<TString> Run_Config = {""};

  vector<TString> CRToRun;

  /// Run All CRs for AN
  if(HasFlag("MultiLepton")) {
    if(HasFlag("RunSyst"))CRToRun = {"SS_CR","LLL_VR"};
    else CRToRun = {"SS_CR","VBF_CR","LLL_VR"};
  }
  
  if(HasFlag("SSMultiLep"))  CRToRun = {"SS_CR","VBF_CR"};
  if(HasFlag("LLL")) CRToRun.push_back("LLL_VR");
  if(HasFlag("SSDilepBDT")) CRToRun = {"SS_CR"};

  if(IsDATA){
    if (this->DataStream.Contains("DoubleMuon")) ChannelsToRun = {MuMu};
    if (this->DataStream.Contains("MuonEG"))     ChannelsToRun = {EMu};
    if (this->DataStream.Contains("EGamma"))    ChannelsToRun = {EE};
    
    if (this->DataStream == "SingleMuon") ChannelsToRun = {MuMu};
    if (this->DataStream == "SingleElectron") ChannelsToRun = {EE};
    if (this->DataStream == "DoubleEG") ChannelsToRun = {EE};
  }
  
  for (auto id: LepIDs){
    for(auto channel : ChannelsToRun){
      if(channel != MuMu  && id =="TopHN") continue;
      
      AnalyzerParameter param_cr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
      if(HasFlag("RunSyst")){
	/// Some code to remove unnecessary Syst runs                                                                                                                                                              
	if(!PassMETFilter()) return;
	
	Event ev = GetEvent();
	
	if(channel==EE){
	  if(!ev.PassTrigger(TrigList_HNL_DblEG)) continue;
	  std::vector<Muon>       MuonCollV     = SelectMuons    (param_cr,param_cr.Muon_Veto_ID,     5., 2.4);  
	  if(MuonCollV.size() > 0) continue;
	}
	if(channel==MuMu){
	  if(!ev.PassTrigger(TrigList_HNL_DblMu)) continue;
	  std::vector<Electron>   ElectronCollV = SelectElectrons(param_cr,param_cr.Electron_Veto_ID, 10., 2.5);	    
	  if(ElectronCollV.size() >0) continue;
	}
	if(channel==EMu){
	  if(!(ev.PassTrigger(TrigList_HNL_MuEG) || ev.PassTrigger(TrigList_HNL_EGMu) )) continue;
	}
      }
      
      
      for(auto iCR : CRToRun)	  RunControlRegions(param_cr , {iCR} );
      
      for(auto iCR : CRToRun){

	/// grab name for central job
	TString param_name = param_cr.Name;
	TString param_defname = param_cr.DefName;
	
	TString SystString=GetChannelString(channel);
	
	for(auto isyst : GetSystList(SystString)){
	  bool runJob = UpdateParamBySyst(id,param_cr,AnalyzerParameter::Syst(isyst),param_name);
	  if(runJob)         RunControlRegions(param_cr , {iCR} );
	  /// Reset 
	  param_cr.Name=param_name;
	  param_cr.DefName=param_defname;
	} // Systematics	  
      } /// CRs
    } // Channels
  } /// Lepton ID

  return;
}

void HNL_ControlRegion_Plotter::RunControlRegions(AnalyzerParameter param_cr, vector<TString> CRs){

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

  Particle METv = GetvMET("PuppiT1xyULCorr", param_cr, MuonTightColl,ElectronTightColl);

  if(CRs.size() == 0) return;
  
  vector<int> RunEl ;
  if(RunCF) RunEl =  {0,1} ;
  else RunEl = {-1};


  ///// Scan Tau ID                                              

  std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(MuonVetoColl,ElectronVetoColl);
  
  //// Add check for Taus                                                                                                                                                                                                                                                     
  std::vector<Tau>  TauColl_Cleaned= SelectTaus   (leps_veto, AK8_JetColl, param_cr.Tau_Tight_ID,20., 2.3,false);
  
  for(auto ir : RunEl){
    RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl, TauColl_Cleaned,
			 
			 AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
			 ev,METv, param_cr, CRs,ir,weight);
  }

}




HNL_ControlRegion_Plotter::HNL_ControlRegion_Plotter(){

  SetupEvMVA();
}
 
HNL_ControlRegion_Plotter::~HNL_ControlRegion_Plotter(){

  DeleteEvMVA();
}
