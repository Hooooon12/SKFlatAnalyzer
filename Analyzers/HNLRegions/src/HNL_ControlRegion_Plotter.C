#include "HNL_ControlRegion_Plotter.h"

void HNL_ControlRegion_Plotter::initializeAnalyzer(){

  
  /// Select BDT versions to plot                                                                                                       
  if(HasFlag("AllBDTVersions"))BDTVersions_to_run ={"V2","V3","V4"};
  else BDTVersions_to_run ={"V3"};


  HNL_LeptonCore::initializeAnalyzer(BDTVersions_to_run);
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
    if(runSyst)CRToRun = {"SS_CR","LLL_VR"};
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


      if(IsSignal() && !SelectChannel(channel)) continue;

      AnalyzerParameter param_cr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
      if(runSyst){
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
      
      /// set runplotter true for non syst and if runsyst only for RunSystPlotter runs
      if(HasFlag("RunSyst")&& !HasFlag("RunSystPlotter"))  param_cr.runPlotter = false;
      else if(HasFlag("RunSyst")&&HasFlag("RunSystPlotter")) param_cr.runPlotter = true;
      else  param_cr.runPlotter = true;
      
      for(auto iCR : CRToRun)	  RunControlRegions(param_cr , {iCR} );


      //// Run plotter for specific systematics                                                                                                                                                                    
      vector<AnalyzerParameter::Syst> SystToPlot = {AnalyzerParameter::Syst::Central};
      if(HasFlag("RunSyst") && HasFlag("RunSystPlotter")){
        if(IsData){
          if(RunFake) {
	    if(channel!=EE) {
              SystToPlot= {
                AnalyzerParameter::Syst::FRMuonRateUp,AnalyzerParameter::Syst::FRMuonRateDown,
                AnalyzerParameter::Syst::FRMuonHighPtUp,AnalyzerParameter::Syst::FRMuonHighPtDown};
            }
            if(channel!=MuMu) {
              SystToPlot= {
                AnalyzerParameter::Syst::FRElectronRateUp,AnalyzerParameter::Syst::FRElectronRateDown,
                AnalyzerParameter::Syst::FRElectronHighPtUp,AnalyzerParameter::Syst::FRElectronHighPtDown};
            }
	  }
	  if(RunCF)   SystToPlot= {AnalyzerParameter::Syst::CFRateUp,AnalyzerParameter::Syst::CFRateDown};
        }
        else {
	  SystToPlot= {AnalyzerParameter::Syst::JetEnUp, AnalyzerParameter::Syst::JetEnDown, AnalyzerParameter::JetResUp,AnalyzerParameter::JetResDown,AnalyzerParameter::JetPNETUp,AnalyzerParameter::JetPNETDown,AnalyzerParameter::MuonResUp,AnalyzerParameter::MuonResDown,AnalyzerParameter::MuonEnUp,AnalyzerParameter::MuonEnDown,AnalyzerParameter::ElectronEnUp,AnalyzerParameter::ElectronEnDown,AnalyzerParameter::ElectronResUp,AnalyzerParameter::ElectronResDown,AnalyzerParameter::RenScaleUp,AnalyzerParameter::RenScaleDown,AnalyzerParameter::FacScaleUp,AnalyzerParameter::FacScaleDown};
        }
      } 
      
      for(auto iCR : CRToRun){

	/// grab name for central job
	TString param_name = param_cr.Name;
	TString param_defname = param_cr.DefName;
	
	TString SystString=GetChannelString(channel);
	
	for(auto isyst : GetSystList(SystString)){

	  if(std::find(SystToPlot.begin(), SystToPlot.end(), isyst) != SystToPlot.end()) param_cr.runPlotter = true;
	  else  param_cr.runPlotter = false;

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

  if(HasFlag("ApplySRSF")){
    if(MCSample == "WZTo3LNu_amcatnlo"){
      if(DataEra == "2016preVFP") weight *= 0.83822;
      if(DataEra == "2016postVFP") weight *=  0.90653;
      if(DataEra == "2017") weight *= 0.92787;
      if(DataEra == "2018") weight *= 0.89930;
    }
  }
  
  std::vector<Electron>   ElectronVetoColl = GetElectrons(param_cr.Electron_Veto_ID, 10.,  2.5);
  std::vector<Muon>       MuonVetoColl     = GetMuons    (param_cr.Muon_Veto_ID,     5.,  2.4);

  /// IF ruunning fake then use FR_ID not Tight
  TString Electron_ID = SetLeptonID("Electron",param_cr);
  TString Muon_ID     = SetLeptonID("Muon", param_cr);

  TString ScaleTag = "";

  //if(param_cr.syst_ == AnalyzerParameter::ScaleUp)   weight *= GetScaleUncertainty(1, ScaleTag);
  //if(param_cr.syst_ == AnalyzerParameter::ScaleDown) weight *= GetScaleUncertainty(-1,ScaleTag);
  if(param_cr.syst_ == AnalyzerParameter::RenScaleUp)   weight *= GetRenScaleUncertainty(1, ScaleTag);
  if(param_cr.syst_ == AnalyzerParameter::RenScaleDown) weight *= GetRenScaleUncertainty(-1,ScaleTag);
  if(param_cr.syst_ == AnalyzerParameter::FacScaleUp)   weight *= GetFacScaleUncertainty(1, ScaleTag);
  if(param_cr.syst_ == AnalyzerParameter::FacScaleDown) weight *= GetFacScaleUncertainty(-1,ScaleTag);

  
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

  TString bjet_tag = "BJet";
  if(HasFlag("BJet_PUID")) bjet_tag = "BJet_v6";
  if(HasFlag("BJet_WPT")) bjet_tag = "BJet_v8";
  if(HasFlag("BJet_AK8Clean")) bjet_tag = "BJet_v7";

  std::vector<Jet>    AK4_BJetColl                = GetHNLJets(bjet_tag, param_cr);


  // Evaluate jet weights
  EvalJetWeight(
		AK4_JetColl,
		AK4_VBF_JetColl,
		AK8_JetColl,
		weight,
		param_cr
		);
  
  
  // MET
  Particle METv = GetvMET(
			  "PuppiT1xyULCorr",
			  param_cr,
			  MuonTightColl,
			  ElectronTightColl
			  );
  
  
  // Nothing to do if no CRs
  if (CRs.empty()) return;
  
  
  // --------------------------------------------------
  // Charge flip electron running setup (run twice for RunCF)
  // --------------------------------------------------
  
  vector<int> RunEl;
  
  if (RunCF) {
    
    if (param_cr.Channel == "EE")
      RunEl = {0, 1};
    else
      RunEl = {0};
    
  }
  else {
    
    RunEl = {-1};
    
  }
  
  
  // --------------------------------------------------
  // Tau cleaning
  // --------------------------------------------------
  
  vector<Lepton*> leps_veto =    MakeLeptonPointerVector(
							 MuonVetoColl,
							 ElectronVetoColl
							 );
  
  
  vector<Tau> TauColl_Cleaned =    SelectTaus(
					      leps_veto,
					      AK8_JetColl,
					      param_cr.Tau_Tight_ID,
					      20.,
					      2.3,
					      false
					      );
  
  
  // --------------------------------------------------
  // PDF systematics
  // --------------------------------------------------
  
  if (param_cr.syst_ == AnalyzerParameter::PDF) {
    
    TString origName    = param_cr.Name;
    TString origDefName = param_cr.DefName;
    
    
    for (auto ir : RunEl) {
      
      for (unsigned int iw = 0; iw < weight_PDF->size(); iw++) {
	
	double pdfWeight = 1.0;
	
	TString pdfName =
	  GetPDFUncertainty(iw, pdfWeight);
	
	
	param_cr.Name =	  origName + pdfName;
	
	param_cr.DefName =	  origDefName + pdfName;
	
	
	RunAllControlRegions(

			     ElectronTightColl,
			     ElectronVetoColl,
			     
			     MuonTightColl,
			     MuonVetoColl,
			     
			     TauColl_Cleaned,
			     
			     AK4_JetCollLoose,
			     AK4_JetColl,
			     AK4_VBF_JetColl,
			     AK8_JetColl,
			     AK4_BJetColl,
			     
			     ev,
			     METv,
			     
			     param_cr,
			     CRs,
			     ir,
			     
			     weight * pdfWeight
			     );
      }
    }
    
    return;
  }
  
  
  // --------------------------------------------------
  // Nominal case
  // --------------------------------------------------
  
  for (auto ir : RunEl) {
    
    RunAllControlRegions(
			 
			 ElectronTightColl,
			 ElectronVetoColl,
			 
			 MuonTightColl,
			 MuonVetoColl,
			 
			 TauColl_Cleaned,
			 
			 AK4_JetCollLoose,
			 AK4_JetColl,
			 AK4_VBF_JetColl,
			 AK8_JetColl,
			 AK4_BJetColl,
			 
			 ev,
			 METv,
			 
			 param_cr,
			 CRs,
			 ir,
			 
			 weight
			 );
  }
  
  
}

  



HNL_ControlRegion_Plotter::HNL_ControlRegion_Plotter(){

  SetupEvMVA();
}
 
HNL_ControlRegion_Plotter::~HNL_ControlRegion_Plotter(){

  DeleteEvMVA();
}
