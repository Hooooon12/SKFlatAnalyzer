#include "HNL_ControlRegion_Plotter.h"

void HNL_ControlRegion_Plotter::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();


  if(IsDATA){
    bool run_ee_bdt=false;
    bool run_mm_bdt=false;
    bool run_em_bdt=false;

    if (this->DataStream.Contains("DoubleMuon")) run_mm_bdt=true;
    if (this->DataStream == "SingleMuon") run_mm_bdt=true;
    if (this->DataStream == "DoubleEG") run_ee_bdt=true;
    if (this->DataStream == "SingleElectron") run_ee_bdt=true;
    if (this->DataStream == "EGamma") run_ee_bdt=true;
    if (this->DataStream.Contains("MuonEG")) run_em_bdt=true;

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

  //if(!(_jentry%10000)) run_Debug=true;
  //else run_Debug=false;
  run_Debug=false; //JH

  //vector<TString> LepIDs = {"HNL_ULID","HNL_ULIDv2","HNTightV2"};
  //vector<TString> LepIDs = {"HNL_ULID"};
  //vector<TString> LepIDs = {"HNL_ULID","HNL_ULIDv2"};
  vector<TString> LepIDs = {"HNL_ULIDv2"};

  /// Set ID by flag
  if(RunTopID) LepIDs = {"TopHN"};
  if(RunPOGID) LepIDs = {"POGTight"};
  //if(RunHighPtID) LepIDs = {"HighPt"};
  if(RunHighPtID) LepIDs = {"HNL_ULID","HighPt"};
  if(RunPekingID) LepIDs = {"Peking"};

  if(strcmp(std::getenv("USER"),"jalmond")==0) LepIDs = {"HNL_ULIDv2","POGTight","HNTightV2"};


  vector<HNL_LeptonCore::Channel> ChannelsToRun = {};
  if(RunEE)   ChannelsToRun.push_back(EE);
  if(RunMuMu) ChannelsToRun.push_back(MuMu);
  if(RunEMu)  ChannelsToRun.push_back(EMu);
  if(ChannelsToRun.size() == 0)ChannelsToRun = {EE,MuMu,EMu};

  if(RunHighPtID) ChannelsToRun = {MuMu};
  //ChannelsToRun = {MuMu}; //JH : to prepare 250113 MUO POG meeting

  ///// Run command 

  vector<TString> CRToRun;
  if(HasFlag("Dilepton"))        CRToRun = {"OS_VR","SS_CR","VBF_CR","LLL_VR"};
  else if(HasFlag("OS_VR"))      CRToRun = {"OS_VR"}; //JH
  else if(HasFlag("SS_CR"))      CRToRun = {"SS_CR"}; //JH
  else if(HasFlag("VBF_CR"))     CRToRun = {"VBF_CR"}; //JH
  else if(HasFlag("LLL_VR"))     CRToRun = {"LLL_VR"}; //JH
  else if(HasFlag("SSMultiLep")) CRToRun = {"SS_CR","VBF_CR"};

  if(IsDATA){
    if (this->DataStream.Contains("DoubleMuon")) ChannelsToRun = {MuMu};
    if (this->DataStream == "SingleMuon") ChannelsToRun = {MuMu};
    if (this->DataStream == "DoubleEG") ChannelsToRun = {EE};
    if (this->DataStream == "SingleElectron") ChannelsToRun = {EE};
    if (this->DataStream == "EGamma") ChannelsToRun = {EE};
    if (this->DataStream.Contains("MuonEG")) ChannelsToRun = {EMu};
  }

  for (auto id: LepIDs){

    for(auto channel : ChannelsToRun){
      if(channel != MuMu  && id =="TopHN") continue;
      if(channel != EE  && id =="HEEP") continue;

      AnalyzerParameter param_signal = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
      if(channel == EMu) param_signal.CFMethod   = "MC";

      param_signal.PlottingVerbose = 0;
      if(HasFlag("LimitOnly")) param_signal.PlottingVerbose = -1;

      for(auto iCR : CRToRun) {
        RunControlRegions(param_signal , {iCR} );

        TString param_name = param_signal.Name;

        TString SystString = "";
        SystString=GetChannelString(channel);


        if(HasFlag("RunSyst")){
          /// Some code to remove unnecessary Syst runs                                                                                                                                                              
          if(!PassMETFilter()) return;
        
          Event ev = GetEvent();

          if(channel==EE){
            if(!ev.PassTrigger(TrigList_HNL_DblEG)) continue;
            std::vector<Muon>       MuonCollV     = SelectMuons    (param_signal,param_signal.Muon_Veto_ID,     5., 2.4);  
            if(MuonCollV.size() > 0) continue;
          }
          if(channel==MuMu){
                  if(!ev.PassTrigger(TrigList_HNL_DblMu)) continue;
            std::vector<Electron>   ElectronCollV = SelectElectrons(param_signal,param_signal.Electron_Veto_ID, 10., 2.5);      
            if(ElectronCollV.size() >0) continue;
          }
          if(channel==EMu){
            if(!(ev.PassTrigger(TrigList_HNL_MuEG) || ev.PassTrigger(TrigList_HNL_EGMu) )) continue;
          }
        }
        

        for(auto isyst : GetSystList(SystString)){
          bool runJob = UpdateParamBySyst(id,param_signal,AnalyzerParameter::Syst(isyst),param_name);
          if(runJob)         RunControlRegions(param_signal , {iCR} );
        }
      }
    }
  }
  return;
}

void HNL_ControlRegion_Plotter::RunControlRegions(AnalyzerParameter param, vector<TString> CRs){

  //run_Debug = (_jentry%nLog==0);
  run_Debug = false; //JH

  //if(run_Debug) cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  
  Event ev = GetEvent();

  /// SetupWeight applies w_GenNorm=1., w_BR=1., w_PU  w_Pref  
  double weight =SetupWeight(ev,param);
  
  // HL ID                                                                                                                                                   
  std::vector<Electron>   ElectronVetoColl = GetElectrons(param.Electron_Veto_ID, 10.,  2.5);
  std::vector<Muon>       MuonVetoColl     = GetMuons    (param.Muon_Veto_ID,     5.,  2.4);

  /// IF ruunning fake then use FR_ID not Tight
  TString Electron_ID = SetLeptonID("Electron",param);
  TString Muon_ID     = SetLeptonID("Muon", param);

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
  //  std::vector<Electron>   ElectronTightColl  =  GetLepCollByRunType    (ElectronTightCollInit,param);

  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);
  
  EvalJetWeight(AK4_JetColl,AK4_VBF_JetColl, AK8_JetColl, weight, param);

  Particle METv = GetvMET("PuppiT1xyULCorr", param, AK4_VBF_JetColl, AK8_JetColl, MuonTightColl,ElectronTightColl);

  if(CRs.size() == 0) return;
  
  vector<int> RunEl ;
  if(RunCF) RunEl =  {0,1} ;
  else RunEl = {-1};


  ///// Scan Tau ID                                              

  std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(MuonVetoColl,ElectronVetoColl);
                                                                                                                                                                                                             

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
      RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl, TauColl_Cleaned,
                           AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl,
                           ev,METv, param, CRs,-1,weight);



    }
    return;
  }




  if(HasFlag("OS"))  {
    
    
    if(!MCSample.Contains("DYJets_Pt")){

      /// Run MiNNLO only
      RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl,TauColl_Cleaned,
         AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl,
         ev,METv, param, CRs,-1,weight);
      
    }
    
    
    
    //param.Name=param.Name+"_PtBinnedDY";
    //param.DefName=param.DefName+"_PtBinnedDY";

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


    //RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl,TauColl_Cleaned,
    //   AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl,
    //   ev,METv, param, CRs,-1,weight); //JH : this is to combine DY nominal and pt-binned samples in Z CR. Only for checking high-pt muon purposes. skip this

  }
  else{
    
    for(auto ir : RunEl){
      RunAllControlRegions(ElectronTightColl,ElectronVetoColl,MuonTightColl,MuonVetoColl, TauColl_Cleaned,

         AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl, 
         ev,METv, param, CRs,ir,weight);
    }
  }
}




HNL_ControlRegion_Plotter::HNL_ControlRegion_Plotter(){

  SetupEvMVA();
}
 
HNL_ControlRegion_Plotter::~HNL_ControlRegion_Plotter(){

  DeleteEvMVA();
}
