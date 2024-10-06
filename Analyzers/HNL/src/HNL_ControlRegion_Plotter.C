#include "HNL_ControlRegion_Plotter.h"

void HNL_ControlRegion_Plotter::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

  bool run_ee_bdt=false;
  bool run_mm_bdt=false;
  bool run_em_bdt=false;

  if(HasFlag("EE") || HasFlag("MuMu") || HasFlag("EMu")){
    run_ee_bdt=HasFlag("EE");
    run_mm_bdt=HasFlag("MuMu");
    run_em_bdt=HasFlag("EMu");
    SetupEventMVAReader("V2",run_ee_bdt,run_mm_bdt,run_em_bdt);
  }
  else SetupEventMVAReader("V2");

  RunTopID = HasFlag("RunHNTop");
  RunPOGID = HasFlag("RunPOG");
  RunHighPtID = HasFlag("RunHighPt");
  RunPekingID = HasFlag("RunPeking");

  nLog = 100000;
}


void HNL_ControlRegion_Plotter::executeEvent(){

  if(!(_jentry%10000)) run_Debug=true;
  else run_Debug=false;

  vector<TString> LepIDs = {"HNL_ULID"};//,"HNTightV2"};
  if(RunTopID) LepIDs = {"TopHN"};
  if(RunPOGID) LepIDs = {"POGTight"};
  if(RunHighPtID) LepIDs = {"HighPt"};
  if(RunPekingID) LepIDs = {"Peking"};
  //  if(strcmp(std::getenv("USER"),"jalmond")==0) LepIDs = {"HNL_ULID","POGTight","TopHN","HNTightV2","MVAPOG"};//,"HNTightV2","POGTight","TopHN","HighPt"};                                

  vector<HNL_LeptonCore::Channel> ChannelsToRun = {};
  if(RunEE)   ChannelsToRun.push_back(EE);
  if(RunMuMu) ChannelsToRun.push_back(MuMu);
  if(RunEMu)  ChannelsToRun.push_back(EMu);
  if(ChannelsToRun.size() == 0)ChannelsToRun = {EE,MuMu,EMu};

  ///// Run command 

  vector<TString> CRToRun;
  if(HasFlag("Dilepton"))        CRToRun = {"OS_VR","SS_CR","VBF_CR","LLL_VR"};
  else if(HasFlag("OS_VR"))      CRToRun = {"OS_VR"}; //JH
  else if(HasFlag("SS_CR"))      CRToRun = {"SS_CR"}; //JH
  else if(HasFlag("VBF_CR"))     CRToRun = {"VBF_CR"}; //JH
  else if(HasFlag("LLL_VR"))     CRToRun = {"LLL_VR"}; //JH
  else if(HasFlag("SSMultiLep")) CRToRun = {"SS_CR","VBF_CR","LLL_VR"};

  for (auto id: LepIDs){
    for(auto channel : ChannelsToRun){
      if(channel != MuMu  && id =="TopHN") continue;
      if(channel != EE  && id =="HighPt") continue;

      AnalyzerParameter param_signal = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
      if(channel == EMu) param_signal.CFMethod   = "MC";

      param_signal.PlottingVerbose = 0;
      if(id == "HNL_ULID")        param_signal.PlottingVerbose = 1;
      if(id.Contains("HEEP"))     param_signal.PlottingVerbose = 1;
      if(RunSyst)  param_signal.PlottingVerbose = -1;



      for(auto iCR : CRToRun) {
        RunControlRegions(param_signal , {iCR} );

        TString param_name = param_signal.Name;
        for(auto isyst : GetSystList()){
          bool runJob = UpdateParamBySyst(id,param_signal,AnalyzerParameter::Syst(isyst),param_name);
          if(runJob)         RunControlRegions(param_signal , {iCR} );
        }
      }
    }
  }
  return;
}

void HNL_ControlRegion_Plotter::RunControlRegions(AnalyzerParameter param, vector<TString> CRs){

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

  if(RunFakeTF){
    // Muon_ID = "HNL_ULID_SB_"+GetYearString();
    // Electron_ID = "HNL_ULID_SB_"+GetYearString();

    Muon_ID = "HNL_ULID_FO"; 
    Electron_ID = "HNL_ULID_Def_FO_"+GetYearString();
  }

  double Min_FakeMuon_Pt     =  5;
  double Min_FakeElectron_Pt =  10 ;
  std::vector<Muon>       MuonTightColl_Init     = SelectMuons    ( param,Muon_ID,     Min_FakeMuon_Pt,     2.4,weight); 
  std::vector<Electron>   ElectronTightColl_Init = SelectElectrons( param,Electron_ID, Min_FakeElectron_Pt, 2.5,weight);

  //// Apply Full Pt cut after pt corrected in fakes  
  double Min_Muon_Pt     =  10.;
  double Min_Electron_Pt =  15;
  std::vector<Muon>       MuonTightColl  = SelectMuons(MuonTightColl_Init,Muon_ID,     Min_Muon_Pt,     2.4);
  std::vector<Electron>   ElectronTightColl = SelectElectrons(ElectronTightColl_Init,Electron_ID, Min_Electron_Pt, 2.5);


  //// Change this so now Truth matching does not remove Leptons but in Definition code the GenFIlter removes events 
  //  std::vector<Muon>       MuonTightColl      =  GetLepCollByRunType    (MuonTightCollInit,    param);  
  //std::vector<Electron>   ElectronTightColl  =  GetLepCollByRunType    (ElectronTightCollInit,param);


  Particle METv = GetvMET("PuppiT1xyULCorr",param); // returns MET with systematic correction                                                                      

  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);
  
  EvalJetWeight(AK4_JetColl, AK8_JetColl, weight, param);

  if(CRs.size() == 0) return;
  

  if(_jentry < 500 ){
    if(RunFake){
      cout << "Running Fakes: Initial check for process name " + param.Name << endl;
      cout << "Muon ID = " << param.Muon_Tight_ID  << " run ID  = " << Muon_ID << endl;
      for(auto ilep : MuonTightColl)cout << "Muon isT=" << ilep.PassID(param.Muon_Tight_ID) << endl;
      
      cout << "Electron ID = " << param.Electron_Tight_ID  << " run ID  = " << Electron_ID << endl;
      for(auto ilep : ElectronTightColl)cout << "Electron isT=" << ilep.PassID(param.Electron_Tight_ID) << endl;
    }
  }


  vector<int> RunEl ;
  if(RunCF) RunEl =  {0,1} ;
  else RunEl = {-1};

  for(auto ir : RunEl){
    RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl, 
       AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
       ev,METv, param, CRs,ir,weight);
  }

}




HNL_ControlRegion_Plotter::HNL_ControlRegion_Plotter(){

  SetupEvMVA();
}
 
HNL_ControlRegion_Plotter::~HNL_ControlRegion_Plotter(){

  DeleteEvMVA();
}
