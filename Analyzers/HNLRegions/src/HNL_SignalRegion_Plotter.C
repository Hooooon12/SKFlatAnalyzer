#include "HNL_SignalRegion_Plotter.h"

void HNL_SignalRegion_Plotter::initializeAnalyzer(){

  // All default settings like trigger/ PD/ BJet are decalred in HNL_LeptonCore::initializeAnalyzer to make them consistent for all HNL codes


  /// Select BDT versions to plot                                                                                                                                         
  if(HasFlag("AllBDTVersions")) BDTVersions_to_run ={"V2","V3","V4"};
  else BDTVersions_to_run ={"V3"};
  
  HNL_LeptonCore::initializeAnalyzer(BDTVersions_to_run);

  
  if(IsDATA){
    bool run_ee_bdt=false;    bool run_mm_bdt=false;    bool run_em_bdt=false;

    if (this->DataStream.Contains("DoubleMuon")) run_mm_bdt=true;
    if (this->DataStream.Contains("MuonEG"))     run_em_bdt=true;
    if (this->DataStream.Contains("EGamma"))     run_ee_bdt=true;

    if (this->DataStream == "SingleMuon")     run_mm_bdt=true;
    if (this->DataStream == "DoubleEG")       run_ee_bdt=true;
    if (this->DataStream == "SingleElectron") run_ee_bdt=true;


    SetupEventMVAReaders(BDTVersions_to_run,run_ee_bdt,run_mm_bdt,run_em_bdt);

  }
  else SetupEventMVAReaders(BDTVersions_to_run);

  nLog = 100000;
}


void HNL_SignalRegion_Plotter::executeEvent(){

  FillTimer("START_EV");
  
  if(_jentry == 0)    cout << "HNL_SignalRegion_Plotter::IsData = " << IsData << endl;
  
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
    if (MCSample.Contains("SSWWTypeI_DF")||MCSample.Contains("EMu_private")) {
      ChannelsToRun = {EMu};
    }
    else if (MCSample.Contains("SSWWTypeI_SF")) {
      ChannelsToRun = {EE, MuMu};
    }
    else if (MCSample.Contains("MuMu_private")) {
      ChannelsToRun = {MuMu};
    }
    else if (MCSample.Contains("EE_private")) {
      ChannelsToRun = {EE};
    }
    else {
      ChannelsToRun = {EE, MuMu, EMu};
    }
  }
  
  for (auto id: LepIDs){
    for(auto channel : ChannelsToRun){
      
      if(IsSignal() && !SelectChannel(channel)) continue;

      //// Central run...
      AnalyzerParameter param_sr = Setup_Param_HNL_ULIDv2(id,GetChannelString(channel));
      RunULAnalysis(param_sr);

      /// Systematic run ...
      TString param_sr_name = param_sr.Name;
      TString param_sr_defname = param_sr.DefName;
      TString SystLabel = GetChannelString(channel);

      if(HasFlag("RunSyst")){

        if(!PassMETFilter()) return;

        Event ev = GetEvent();

        if(channel==EE){
          if(!ev.PassTrigger(TrigList_HNL_DblEG)) continue;
          std::vector<Muon>       MuonCollV     = SelectMuons    (param_sr,param_sr.Muon_Veto_ID,     5., 2.4);
          if(MuonCollV.size() > 0) continue;
        }
        if(channel==MuMu){
          if(!ev.PassTrigger(TrigList_HNL_DblMu)) continue;
          std::vector<Electron>   ElectronCollV = SelectElectrons(param_sr,param_sr.Electron_Veto_ID, 10., 2.5);
          if(ElectronCollV.size() >0) continue;
        }
        if(channel==EMu){
          if(!(ev.PassTrigger(TrigList_HNL_MuEG) || ev.PassTrigger(TrigList_HNL_EGMu) )) continue;
        }
      }

      //// Run Systematics
      for(auto isyst : GetSystList(SystLabel)){
        bool runJob = UpdateParamBySyst(id,param_sr,AnalyzerParameter::Syst(isyst),param_sr_name);
        if(runJob) RunULAnalysis(param_sr);
        /// Just in case reset param names
        param_sr.Name=param_sr_name;
        param_sr.DefName=param_sr_defname;
      }
    }
  }
  FillTimer("END_EV");

  return ;
}

void HNL_SignalRegion_Plotter::RunULAnalysis(AnalyzerParameter param_sr){

  if(_jentry< 1) cout << "HNL_SignalRegion_Plotter::executeEvent " << param_sr.Name <<  " " << param_sr.Channel << endl;
  
  Event ev = GetEvent();
  double weight =SetupWeight(ev,param_sr);
  
  // HL ID
  std::vector<Electron>   ElectronCollV = SelectElectrons(param_sr,param_sr.Electron_Veto_ID, 10., 2.5); 
  std::vector<Muon>       MuonCollV     = SelectMuons    (param_sr,param_sr.Muon_Veto_ID,     5., 2.4);
  
  TString el_ID = SetLeptonID("Electron",param_sr);
  TString mu_ID = SetLeptonID("Muon", param_sr);

  TString ScaleTag = "";

  //std::vector<FatJet> AK8_JetColl2 = GetHNLAK8Jets(param_sr.AK8JetColl,  param_sr);
  //if(AK8_JetColl2.size()==1) ScaleTag = param_sr.Name+param_sr.Channel;
  if(param_sr.syst_ == AnalyzerParameter::ScaleUp)   weight *= GetScaleUncertainty(1, ScaleTag);
  if(param_sr.syst_ == AnalyzerParameter::ScaleDown) weight *= GetScaleUncertainty(-1,ScaleTag);

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

  
  ///// PDF SCAN FOR SIGNAL 
  if(param_sr.syst_ == AnalyzerParameter::PDF) {
    TString ORIGName= param_sr.Name;
    TString ORIGDefName= param_sr.DefName;

    for(unsigned int iw=0; iw<weight_PDF->size(); iw++){
      double PDF_W=1;
      TString PNAME_PDF = GetPDFUncertainty(iw,PDF_W);

      param_sr.Name= ORIGName+PNAME_PDF;
      param_sr.DefName=ORIGDefName +PNAME_PDF;

      RunAllSignalRegions(Inclusive,
        ElectronCollT,ElectronCollV,MuonCollT,MuonCollV,  TauColl_Cleaned,
        AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl,
        ev,METv, param_sr, -1, weight*PDF_W);

      
    }
    return;
  }
  else{

    vector<TString> config_list = {""};
    TString ORIGName= param_sr.Name;
    TString ORIGDefName= param_sr.DefName;

    for(auto iconfig : config_list){

      param_sr.Name= ORIGName+iconfig;
      param_sr.DefName=ORIGDefName +iconfig;
      param_sr.SRConfig  =iconfig;
      for(auto ir : RunEl){
  
        RunAllSignalRegions(Inclusive,
          ElectronCollT,ElectronCollV,MuonCollT,MuonCollV,  TauColl_Cleaned,
          AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
          ev,METv, param_sr, ir, weight);
      }
      param_sr.Name= ORIGName;
      param_sr.DefName=ORIGDefName;

    }
  }

  FillTimer("END_SR");


}
 

HNL_SignalRegion_Plotter::HNL_SignalRegion_Plotter(){

  cout << "HNL_SignalRegion_Plotter::HNL_SignalRegion_Plotter  TMVA::Tools::Instance() " << endl;
  SetupEvMVA();
  
}
 
HNL_SignalRegion_Plotter::~HNL_SignalRegion_Plotter(){

  DeleteEvMVA();

}




