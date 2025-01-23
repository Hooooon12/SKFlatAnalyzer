#include "HNL_SignalRegion_Plotter.h"

void HNL_SignalRegion_Plotter::initializeAnalyzer(){

  // All default settings like trigger/ PD/ BJet are decalred in HNL_LeptonCore::initializeAnalyzer to make them consistent for all HNL codes

  HNL_LeptonCore::initializeAnalyzer();
  

  if(IsDATA){
    bool run_ee_bdt=false;
    bool run_mm_bdt=false;
    bool run_em_bdt=false;
    
    if (this->DataStream == "DoubleMuon") run_mm_bdt=true;
    if (this->DataStream == "SingleMuon") run_mm_bdt=true;
    if (this->DataStream == "DoubleEG") run_ee_bdt=true;
    if (this->DataStream == "SingleElectron") run_ee_bdt=true;
    if (this->DataStream == "EGamma") run_ee_bdt=true;
    if (this->DataStream == "MuonEG") run_em_bdt=true;
    
    SetupEventMVAReader("V2",run_ee_bdt,run_mm_bdt,run_em_bdt);
    
  }
  else SetupEventMVAReader("V2");

  nLog = 100000;

  RunTopID = HasFlag("RunHNTop");
  RunPOGID = HasFlag("RunPOG");
  RunHighPtID = HasFlag("RunHighPt");
  RunPekingID = HasFlag("RunPeking");
  
}


void HNL_SignalRegion_Plotter::executeEvent(){

  //FillTimer("START_EV");
  
  if(_jentry == 0){
    cout << "HNL_SignalRegion_Plotter::IsData = " << IsData << endl;
  }
  //vector<TString> LepIDs = {"HNL_ULID","HNL_ULIDv2"};
  //vector<TString> LepIDs = {"HNL_ULID"};
  vector<TString> LepIDs = {"HNL_ULIDv2"};
  if(HasFlag("AllID")) LepIDs = {"HNL_ULID","HNTightV2", "POGTight"};

  //// Allow ID setting by flag
  if(RunTopID) LepIDs = {"TopHN"};
  if(RunPOGID) LepIDs = {"POGTight"};
  if(RunHighPtID) LepIDs = {"HNL_ULID","HighPt"};
  if(RunPekingID) LepIDs = {"Peking"};


  //  if(strcmp(std::getenv("USER"),"jalmond")==0) LepIDs = {"HNL_ULID","POGTight","TopHN","HNTightV2","MVAPOG"};//,"HNTightV2","POGTight","TopHN","HighPt"};

  vector<HNL_LeptonCore::Channel> ChannelsToRun = {};
  if(RunEE)   ChannelsToRun.push_back(EE);
  if(RunMuMu) ChannelsToRun.push_back(MuMu);
  if(RunEMu)  ChannelsToRun.push_back(EMu);
  if(ChannelsToRun.size() == 0) ChannelsToRun = {EE,MuMu,EMu};

  if(RunHighPtID) ChannelsToRun = {MuMu};
  //ChannelsToRun = {MuMu}; //JH : to prepare 250113 MUO POG meeting


  if(IsDATA){
    if (this->DataStream == "DoubleMuon") ChannelsToRun = {MuMu};
    if (this->DataStream == "SingleMuon") ChannelsToRun = {MuMu};
    if (this->DataStream == "DoubleEG") ChannelsToRun = {EE};
    if (this->DataStream == "SingleElectron") ChannelsToRun = {EE};
    if (this->DataStream == "EGamma") ChannelsToRun = {EE};
    if (this->DataStream == "MuonEG") ChannelsToRun = {EMu};
  }


  for (auto id: LepIDs){

    for(auto channel : ChannelsToRun){
      
      AnalyzerParameter param = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
      
      param.PlottingVerbose = 0; //// Draw basic plots
      if(id.Contains("ULID"))  param.PlottingVerbose = 1; /// Draw more plots
      if(id.Contains("HEEP"))  param.PlottingVerbose = 1;
            
      if(HasFlag("HighPtTrigger")) param.TriggerSelection     = "HighPt";          
      if(HasFlag("HighPtTrigger")) param.Apply_Weight_TriggerSF = false;

      if(HasFlag("LimitOnly")) param.PlottingVerbose = -1;

      RunULAnalysis(param);

      TString param_name = param.Name;

      for(auto isyst : GetSystList("All",channel)){ //JH
      //for(auto isyst : GetSystList("",channel)){ //JH
        bool runJob = UpdateParamBySyst(id,param,AnalyzerParameter::Syst(isyst),param_name);
        if(runJob) RunULAnalysis(param);
      }
    }
  }
  //FillTimer("END_EV");

  return ;
}

void HNL_SignalRegion_Plotter::RunULAnalysis(AnalyzerParameter param){

  if(run_Debug) cout << "HNL_SignalRegion_Plotter::executeEvent " << endl;

  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);
  
  // HL ID
  std::vector<Electron>   ElectronCollV = SelectElectrons(param,param.Electron_Veto_ID, 10., 2.5); 
  std::vector<Muon>       MuonCollV     = SelectMuons    (param,param.Muon_Veto_ID,     5., 2.4);
  
  TString el_ID = SetLeptonID("Electron",param);
  TString mu_ID = SetLeptonID("Muon", param);
  
  if(param.syst_ == AnalyzerParameter::ScaleUp) weight *= GetScaleUncertainty(1);
  if(param.syst_ == AnalyzerParameter::ScaleDown) weight *= GetScaleUncertainty(-1);


  double Min_FakeMuon_Pt      =  5;
  double Min_FakeElectron_Pt =  10 ;
  std::vector<Muon>       MuonTightColl_Init     = SelectMuons    ( param,mu_ID,     Min_FakeMuon_Pt,     2.4,weight);
  std::vector<Electron>   ElectronTightColl_Init = SelectElectrons( param,el_ID, Min_FakeElectron_Pt, 2.5,weight);

  if(HasFlag("CompareTuneP")){
    MuonTightColl_Init = GetHighPtMuons("TuneP_POG",mu_ID, Min_FakeMuon_Pt,     2.4);
  }

  //// Apply Full Pt cut after pt corrected in fakes                                                                            
  double Min_Muon_Pt     =  10.;
  double Min_Electron_Pt =  15;
  std::vector<Muon>       MuonCollT  = SelectMuons(MuonTightColl_Init,mu_ID,     Min_Muon_Pt,     2.4);
  std::vector<Electron>   ElectronCollT = SelectElectrons(ElectronTightColl_Init,el_ID, Min_Electron_Pt, 2.5);

  std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(MuonCollV,ElectronCollV);
  std::vector<Tau>        TauColl        = SelectTaus   (leps_veto,param.Tau_Veto_ID,20., 2.3);

  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);
 
  Particle METv = GetvMET("PuppiT1xyULCorr", param, AK4_VBF_JetColl, AK8_JetColl, MuonCollT, ElectronCollT); // returns MET with systematic correction; run this after all object selection done; NOTE that VBF jet is used here
  
  EvalJetWeight(AK4_JetColl, AK8_JetColl, weight, param);

  //FillTimer("START_SR");

  vector<int> RunEl ;
  if(RunCF) RunEl =  {0,1} ;
  else RunEl = {-1};

  if(param.syst_ == AnalyzerParameter::PDF) {
    TString ORIGName= param.Name;
    TString ORIGDefName= param.DefName;

    for(unsigned int iw=0; iw<weight_PDF->size()+1; iw++){
      double PDF_W=1;
      TString PNAME_PDF = GetPDFUncertainty(iw,PDF_W);

      param.Name= ORIGName+PNAME_PDF;
      param.DefName=ORIGDefName +PNAME_PDF;

      RunAllSignalRegions(Inclusive,
			  ElectronCollT,ElectronCollV,MuonCollT,MuonCollV,  TauColl,
			  AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl,
			  ev,METv, param, -1, weight*PDF_W);

      
    }
  }
  else{
					  
    for(auto ir : RunEl){
      
      RunAllSignalRegions(Inclusive,
			  ElectronCollT,ElectronCollV,MuonCollT,MuonCollV,  TauColl,
			  AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
			ev,METv, param, ir, weight);
    }
  }

  //FillTimer("END_SR"); //JH


}
 

HNL_SignalRegion_Plotter::HNL_SignalRegion_Plotter(){

  cout << "HNL_SignalRegion_Plotter::HNL_SignalRegion_Plotter  TMVA::Tools::Instance() " << endl;
  SetupEvMVA();
  
}
 
HNL_SignalRegion_Plotter::~HNL_SignalRegion_Plotter(){

  DeleteEvMVA();

}




