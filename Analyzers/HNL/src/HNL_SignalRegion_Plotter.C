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

  if(_jentry < 100 && HasFlag("PrintGen"))PrintGen(All_Gens);

  if(HasFlag("ScanSystematic")){
    ///Only scan 100 events
    if(_jentry > 100) return;
  }

  /// Main ID only setting
  vector<TString> LepIDs = {"HNL_ULIDv2"};

  
  //// Allow ID setting by flags
  if(HasFlag("AllID")) LepIDs = {"HNL_ULID","HNTightV2", "POGTight","HNL_ULIDv2"};
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

  /// Flag specific settings for channel
  if(RunHighPtID) ChannelsToRun = {MuMu};
  //ChannelsToRun = {MuMu}; //JH : to prepare 250113 MUO POG meeting

  //// Match Channel to data stream
  if(IsDATA){
    if (this->DataStream == "DoubleMuon") ChannelsToRun = {MuMu};
    if (this->DataStream == "SingleMuon") ChannelsToRun = {MuMu};
    if (this->DataStream == "DoubleEG") ChannelsToRun = {EE};
    if (this->DataStream == "SingleElectron") ChannelsToRun = {EE};
    if (this->DataStream == "EGamma") ChannelsToRun = {EE};
    if (this->DataStream == "MuonEG") ChannelsToRun = {EMu};
  }

  //// Match Channel to signal process
  if(MCSample.Contains("Type")){
    //// Run channel based on MC Sample
    if(MCSample.Contains("SSWWTypeI_DF")) ChannelsToRun = {EMu};
    else if(MCSample.Contains("SSWWTypeI_SF")) ChannelsToRun = {EE,MuMu};
    else ChannelsToRun = {EE,MuMu,EMu};
  }

  if(HasFlag("CompareTuneP")) {
    ChannelsToRun = {MuMu};
    LepIDs = {"HNTightV2", "POGTight","HNL_ULIDv2"};
  }

  for (auto id: LepIDs){

    for(auto channel : ChannelsToRun){
      
      if(MCSample.Contains("Type")&& !SelectChannel(channel)) continue;

      AnalyzerParameter param = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
      
      if(HasFlag("HighPtTrigger")) param.TriggerSelection     = "HighPt";          
      if(HasFlag("HighPtTrigger")) param.Apply_Weight_TriggerSF = false;

      if(HasFlag("LimitOnly")) param.PlottingVerbose = -1;

      RunULAnalysis(param);

      TString param_name = param.Name;

      TString SystLabel = "";
      if(HasFlag("Syst_Theory")) SystLabel= "Theory";
      else if(HasFlag("Syst_Muon")) SystLabel= "Muon";
      else if(HasFlag("Syst_Muon_Reco")) SystLabel= "Muon_Reco";
      else if(HasFlag("Syst_Jet")) SystLabel= "Jet";
      else if(HasFlag("Syst_MET")) SystLabel= "MET";      
      else SystLabel=GetChannelString(channel);

      if(HasFlag("RunSyst")){

        if(!PassMETFilter()) return;

        Event ev = GetEvent();

        if(channel==EE){
          if(!ev.PassTrigger(TrigList_HNL_DblEG)) continue;
          std::vector<Muon>       MuonCollV     = SelectMuons    (param,param.Muon_Veto_ID,     5., 2.4);
          if(MuonCollV.size() > 0) continue;
        }
        if(channel==MuMu){
          if(!ev.PassTrigger(TrigList_HNL_DblMu)) continue;
          std::vector<Electron>   ElectronCollV = SelectElectrons(param,param.Electron_Veto_ID, 10., 2.5);
          if(ElectronCollV.size() >0) continue;
        }
        if(channel==EMu){
          if(!(ev.PassTrigger(TrigList_HNL_MuEG) || ev.PassTrigger(TrigList_HNL_EGMu) )) continue;
        }

      }

      //// Run Systematics
      for(auto isyst : GetSystList(SystLabel)){
        bool runJob = UpdateParamBySyst(id,param,AnalyzerParameter::Syst(isyst),param_name);
        if(runJob) RunULAnalysis(param);
      }
    }
  }
  //FillTimer("END_EV");

  return ;
}

void HNL_SignalRegion_Plotter::RunULAnalysis(AnalyzerParameter param){

  if(_jentry< 10) cout << "HNL_SignalRegion_Plotter::executeEvent " << param.Name << endl;
  
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


  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);

  if(HasFlag("TestAK8")){
    vector<TString> AK8Tag = {"Loose","HNL","HNL_NoMass","HNL_PN","HNL_PN_NoMass","EXO17028"};
    for (auto akg_tag : AK8Tag){
      std::vector<FatJet> ak8_jetcoll         = GetHNLAK8Jets(akg_tag,param);
      for(const auto& ijet : ak8_jetcoll)            FillHist( "AK8_Plots_"+akg_tag+"/AK8J_Eta",     ijet.Eta()       , weight, 100, -5., 5.   , "AK8 Jet #eta");
      double weight_jets=weight;
      EvalJetWeight(AK4_JetColl,AK4_VBF_JetColl, AK8_JetColl, weight_jets, param);
      for(const auto& ijet : ak8_jetcoll)            FillHist( "AK8_Plots_"+akg_tag+"/AK8J_Eta_weighted",     ijet.Eta()       , weight_jets, 100, -5., 5.   , "AK8 Jet #eta");
      for(const auto& ijet : ak8_jetcoll)            FillHist(  "AK8_Plots_"+akg_tag+"/AK8J_SDMass",      ijet.SDMass()    , weight_jets, 100, 0., 500.  , "Mass_{softdrop} GeV");
      for(const auto& ijet : ak8_jetcoll)    {
  if(ijet.SDMass()    < 40) FillHist( "AK8_Plots_"+akg_tag+"/AK8J_SB1_Eta_weighted",     ijet.Eta()       , weight_jets, 100, -5., 5.   , "AK8 Jet #eta");
  if(ijet.SDMass()    > 130) FillHist( "AK8_Plots_"+akg_tag+"/AK8J_SB2_Eta_weighted",     ijet.Eta()       , weight_jets, 100, -5., 5.   , "AK8 Jet #eta");
      }
    }
  }
  
  Particle METv = GetvMET("PuppiT1xyULCorr", param, AK4_VBF_JetColl, AK8_JetColl, MuonCollT, ElectronCollT); // returns MET with systematic correction; run this after all object selection done; NOTE that VBF jet is used here
  
  EvalJetWeight(AK4_JetColl,AK4_VBF_JetColl, AK8_JetColl, weight, param);

  //FillTimer("START_SR");

  vector<int> RunEl ;
  if(RunCF) RunEl =  {0,1} ;
  else RunEl = {-1};

  /////////// RUN ANALYSE LOOP
  
  ///// Scan Tau ID

  //// Add check for Taus                                                                                                                                                                                         
  std::vector<Tau>    TauColl_Cleaned;

  if(HasFlag("TauScan")){
    vector<TString> TauIDs = {"NoCut","Default"};
    vector<TString> TauJetIDs={"","JetVVL","JetVL"};
    vector<TString> TauElIDs={"","ElVVL","ElVL"};
    vector<TString> TauMuIDs={"","MuVL","MuL"};

    for(auto ij : TauJetIDs){
      for(auto ie: TauElIDs){
  for(auto im: TauMuIDs){
    TauIDs.push_back(ij+"_"+ie+"_"+im);
  }
      }
    }

    TString ORIGName= param.Name;
    TString ORIGDefName= param.DefName;

    for(auto id_tau : TauIDs){

      param.Name= ORIGName+id_tau;
      param.DefName=ORIGDefName +id_tau;

      std::vector<Tau>   TauColl_Uncleaned  = SelectTaus   (leps_veto,id_tau,20., 2.3);
      TauColl_Cleaned.clear();
     
      for(auto ilep : TauColl_Uncleaned) {
  if(id_tau == "Default") continue;
  bool matched=false;
  for(auto ilep2 : leps_veto) {
    if(ilep.DeltaR(*ilep2) < 0.4) matched=true;
  }
  if(matched) continue;
  TauColl_Cleaned.push_back(ilep);
      }

      /// Run Analyser with Tau ID cleaned 
      RunAllSignalRegions(Inclusive,
                          ElectronCollT,ElectronCollV,MuonCollT,MuonCollV,  TauColl_Cleaned,
                          AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl,
                          ev,METv, param, -1, weight);

      
    }
    return;
  }
  
  ///// PDF SCAN FOR SIGNAL 
  if(param.syst_ == AnalyzerParameter::PDF) {
    TString ORIGName= param.Name;
    TString ORIGDefName= param.DefName;

    for(unsigned int iw=0; iw<weight_PDF->size()+1; iw++){
      double PDF_W=1;
      TString PNAME_PDF = GetPDFUncertainty(iw,PDF_W);

      param.Name= ORIGName+PNAME_PDF;
      param.DefName=ORIGDefName +PNAME_PDF;

      RunAllSignalRegions(Inclusive,
        ElectronCollT,ElectronCollV,MuonCollT,MuonCollV,  TauColl_Cleaned,
        AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl,
        ev,METv, param, -1, weight*PDF_W);
      
    }
    return;
  }
  else{
    for(auto ir : RunEl){
      
      RunAllSignalRegions(Inclusive,
        ElectronCollT,ElectronCollV,MuonCollT,MuonCollV,  TauColl_Cleaned,
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




