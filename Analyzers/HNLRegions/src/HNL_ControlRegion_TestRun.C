#include "HNL_ControlRegion_TestRun.h"

void HNL_ControlRegion_TestRun::initializeAnalyzer(){

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


    SetupEventMVAReader("V3",run_ee_bdt,run_mm_bdt,run_em_bdt);

  }
  else SetupEventMVAReader("V3");


  RunTopID = HasFlag("RunHNTop");
  RunPOGID = HasFlag("RunPOG");
  RunHighPtID = HasFlag("RunHighPt");
  RunPekingID = HasFlag("RunPeking");

  nLog = 100000;
}


void HNL_ControlRegion_TestRun::executeEvent(){

  //if(!(_jentry%10000)) run_Debug=true;
  //else run_Debug=false;
  run_Debug=false; //JH

  //vector<TString> LepIDs = {"HNL_ULIDv2","POGTight","HNTightV2"};
  vector<TString> LepIDs = {"HNL_ULIDv2"};

  vector<HNL_LeptonCore::Channel> ChannelsToRun = {};

  //// Set Individual channel
  if(RunEE)   ChannelsToRun.push_back(EE);
  if(RunMuMu) ChannelsToRun.push_back(MuMu);
  if(RunEMu)  ChannelsToRun.push_back(EMu);
  //   else run all 3 channels
  if(ChannelsToRun.size() == 0)ChannelsToRun = {EE,MuMu,EMu};

  vector<TString> Run_Config = {""};
  
  ///// Run command 

  vector<TString> CRToRun;
  if(HasFlag("OS"))  {
    LepIDs = {"HNL_ULIDv2"};
    CRToRun = {"OS_VR"};
    ChannelsToRun = {MuMu};
  }
  else {
    /// Run All CRs for AN
    if(HasFlag("MultiLepton")) {
      if(HasFlag("RunSyst"))CRToRun = {"SS_CR","LLL_VR"};
      else CRToRun = {"SS_CR","VBF_CR","LLL_VR"};
    }
    
    /// Run selected CRs
    if(HasFlag("Dilepton"))    CRToRun = {"OS_VR","SS_CR","VBF_CR"};
    if(HasFlag("SSMultiLep"))  CRToRun = {"SS_CR","VBF_CR"};
    if(HasFlag("LLL")) CRToRun.push_back("LLL_VR");
  }
  
  if(IsDATA){
    if (this->DataStream.Contains("DoubleMuon")) ChannelsToRun = {MuMu};
    if (this->DataStream.Contains("MuonEG"))     ChannelsToRun = {EMu};
    if (this->DataStream.Contains("EGamma"))    ChannelsToRun = {EE};
    
    if (this->DataStream == "SingleMuon") ChannelsToRun = {MuMu};
    if (this->DataStream == "SingleElectron") ChannelsToRun = {EE};
    if (this->DataStream == "DoubleEG") ChannelsToRun = {EE};
  }

  for(auto iconfig : Run_Config){
    for (auto id: LepIDs){
      for(auto channel : ChannelsToRun){
	if(channel != MuMu  && id =="TopHN") continue;

	AnalyzerParameter param_cr;
	if(id=="HNL_ULIDv2") param_cr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
	else param_cr = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
	
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

	//// Config different settings
	
	if(iconfig == "alt_fake"){
	  AnalyzerParameter param_loose = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
	  param_loose.Name = param_loose.Name + "_AltID";
	  param_loose.DefName = param_loose.DefName + "_AltID";
	  param_loose.k.Electron_FR        = "HNL_ULID_FO_v0_AJ40_El12";
	  param_loose.Electron_FR_ID    = "HNL_HighPt_ULID_FO_v0";
	  
	  for(auto iCR : CRToRun) RunControlRegions(param_loose , {iCR} );
	}

	else{
	  
	  //// Make it clearer the param used in CR/SR for HNL ID                                                       
	  
	  //// Main Jobs for analysis
	  
	  for(auto iCR : CRToRun)	  RunControlRegions(param_cr , {iCR} );
	  
	  for(auto iCR : CRToRun){
	    /// grab name for central job
	    TString param_name = param_cr.Name;
	    TString param_defname = param_cr.DefName;
	    
	    TString SystString = "";
	    if(HasFlag("OS")) SystString = "Muon";
	    else SystString=GetChannelString(channel);
	    
	    for(auto isyst : GetSystList(SystString)){
	      bool runJob = UpdateParamBySyst(id,param_cr,AnalyzerParameter::Syst(isyst),param_name);
	      if(runJob)         RunControlRegions(param_cr , {iCR} );
	      /// Reset 
	      param_cr.Name=param_name;
	      param_cr.DefName=param_defname;
	    } // Systematics	  
	  } /// CRs
	}/// Main config
      } // Channels
    } /// Lepton ID
  } /// config loop
  return;
}

void HNL_ControlRegion_TestRun::RunControlRegions(AnalyzerParameter param_cr, vector<TString> CRs){

  //run_Debug = (_jentry%nLog==0);
  run_Debug = false; //JH

  //if(run_Debug) cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  
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
  std::vector<Tau>    TauColl_Cleaned;
  
  if(HasFlag("TauScan")){
    if(HasFlag("HighJet")) {
      if(AK4_JetColl.size() < 4) return;
    }
    
    param_cr.PlottingVerbose = 0;
    vector<TString> TauIDs = {"NoCut","Default"};
    vector<TString> TauJetIDs={"JetVVL","JetVL","JetL","JetM","JetT","JetVT","JetVVT"};
    vector<TString> TauElIDs={"ElVVL","ElT"};
    vector<TString> TauMuIDs={"MuVL","MuT"};

    for(auto ij : TauJetIDs){
      for(auto ie: TauElIDs){
        for(auto im: TauMuIDs){
          TauIDs.push_back(ij+"_"+ie+"_"+im);
          TauIDs.push_back("AK8_"+ij+"_"+ie+"_"+im);
        }
      }
    }

    TString ORIGName= param_cr.Name;
    TString ORIGDefName= param_cr.DefName;

    for(auto id_tau : TauIDs){

      param_cr.Name= ORIGName+id_tau;
      param_cr.DefName=ORIGDefName +id_tau;

      std::vector<Tau>   TauColl_Uncleaned  = SelectTaus   (leps_veto,id_tau,20., 2.3);
      TauColl_Cleaned.clear();

      for(auto ilep : TauColl_Uncleaned) {
        if(id_tau == "Default") continue;
        bool matched=false;
        for(auto ilep2 : leps_veto) {
          if(ilep.DeltaR(*ilep2) < 0.4) matched=true;
        }

        if(id_tau.Contains("AK8")){
          for(auto ijet : AK8_JetColl){
            if(ijet.DeltaR(ilep) < 0.8) matched=true;
          }
        }

        if(matched) continue;

        TauColl_Cleaned.push_back(ilep);
      }
    
      /// Run Analyser with Tau ID cleaned                                                                                                                                                                                                                                    
      RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl, TauColl_Cleaned,
                           AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl,
                           ev,METv, param_cr, CRs,-1,weight);



    }
    return;
  } //// Tau Scan code


  //// Select Taus, and clean with AK8 jets and light-leptons
  
  //  TauColl_Cleaned  = SelectTaus   (leps_veto, AK8_JetColl, "JetT_MuT_ELT",20., 2.3);

  TauColl_Cleaned.clear();
  
  if(HasFlag("OS"))  {
      
      
      if(!MCSample.Contains("DYJets_Pt")){

  /// Run MiNNLO only
      RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl,TauColl_Cleaned,
			   AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl,
			   ev,METv, param_cr, CRs,-1,weight);
      
    }
    
    
    
    param_cr.Name=param_cr.Name+"_PtBinnedDY";
    param_cr.DefName=param_cr.DefName+"_PtBinnedDY";

    //
    //if(MCSample == "DYJetsToMuMu_MiNNLO" || MCSample.Contains("DYJets_Pt") ) {

    //  if(MuonTightColl.size()==2 && !(SameCharge(MuonTightColl))){
    //    Lepton l1(MuonTightColl[0]);
    //    Lepton l2(MuonTightColl[1]);

    //    int Idx1_Closest = GenMatchedIdx(l1, All_Gens);
    //    int Idx2_Closest = GenMatchedIdx(l2, All_Gens);
    //    if(Idx1_Closest >=0 && Idx2_Closest >= 0){

    //      if (std::abs(All_Gens[Idx1_Closest].PID()) != 13 || std::abs(All_Gens[Idx2_Closest].PID()) != 13) return;

    //      Particle Z = All_Gens[Idx1_Closest] + All_Gens[Idx2_Closest];

    //      if(MCSample == "DYJetsToMuMu_MiNNLO"){
    //        if(Z.Pt() > 150) return;
    //      }
    //      else if(Z.Pt() <= 150) return;
    //    }
    //    else return;
    //  } // DY loop                                                                                                                                                                                              
    //}


    RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl,TauColl_Cleaned,

			 AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl,
			 ev,METv, param_cr, CRs,-1,weight);

  }
  else{
    
    for(auto ir : RunEl){
      RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl, TauColl_Cleaned,

			   AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
			   ev,METv, param_cr, CRs,ir,weight);
    }
  }
}




HNL_ControlRegion_TestRun::HNL_ControlRegion_TestRun(){

  SetupEvMVA();
}
 
HNL_ControlRegion_TestRun::~HNL_ControlRegion_TestRun(){

  DeleteEvMVA();
}
