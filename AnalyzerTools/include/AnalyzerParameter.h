#ifndef AnalyzerParameter_h
#define AnalyzerParameter_h

#include "TString.h"
#include <iostream>

using namespace std;

class AnalyzerParameter{

 public:
  TString Name;
  TString NameInclusive_Channel;
  TString DefName; 
  TString Channel;
  TString SplitPlot;
  TString CutFlowDir;
  TString hprefix;
  TString hpostfix;


  bool MCCorrrectionIgnoreNoHist,DEBUG;

  ///// Obj Parameters
  TString Electron_Tight_ID, Electron_Loose_ID, Electron_Veto_ID;
  TString Electron_FR_ID, Electron_CF_ID ;
  TString Muon_Tight_ID, Muon_Loose_ID, Muon_Veto_ID;
  TString Muon_FR_ID, Muon_CF_ID;
  TString JetPUID,TriggerSelection;
  TString Tau_Tight_ID, Tau_Loose_ID, Tau_Veto_ID;
  TString Jet_ID, FatJet_ID,FatJet_CleaningColl;

  bool   Electron_UseMini, Electron_UsePtCone;
  bool   Muon_UseMini, Muon_UsePtCone, Muon_UseTuneP;

  double isocut;
  double Electron_Tight_RelIso, Electron_Loose_RelIso, Electron_Veto_RelIso;
  double Electron_MinPt, Electron_MaxEta ;
  double Muon_Tight_RelIso, Muon_Loose_RelIso, Muon_Veto_RelIso;
  double Muon_MinPt, Muon_MaxEta ;
  double Jet_MinPt, Jet_MaxEta ;
  double FatJet_MinPt, FatJet_MaxEta ;

  bool ApplyPR;
  /// Weights
  bool   Apply_Weight_LumiNorm, Apply_Weight_MCSign,Apply_Weight_Norm1Ipb, Apply_Weight_SumQ,  Apply_Weight_PileUp, Apply_Weight_PreFire ,Apply_Weight_kFactor, Apply_Weight_IDSF , Apply_Weight_TriggerSF, Apply_Weight_RECOSF, Apply_Weight_Z0, Apply_Weight_TopCorr,Apply_Weight_DYCorr, Apply_Weight_BJetSF, Apply_Weight_PNETSF,Apply_Weight_MuonTrackerSF,Apply_Weight_JetPUID;


  /// Other
  TString SRConfig;
  int SystDir_PU,WriteOutVerbose, PlottingVerbose;

  std::string SystDir_BTag,  BJet_Method,   FakeMethod,CFMethod,  ConvMethod;

  vector<TString> Jobs;
  TString FakeRateMethod;
  TString FakeRateParam;
  TString BTagger;
  TString BWP;
  TString AK4JetColl;
  TString AK4VBFJetColl;
  TString AK8JetColl;
  TString BJetColl;

  struct Weight{
    double lumiweight=1;
    double PUweight=1,PUweight_up=1,PUweight_down=1;
    double prefireweight=1,prefireweight_up=1,prefireweight_down=1;
    double z0weight=1;
    double zptweight=1;
    double zptweight_g=1;
    double zptweight_gy=1;
    double zptweight_gym=1;
    double topptweight=1;
    double weakweight=1;
    double muonRECOSF=1;
    double electronRECOSF=1;
    vector<vector<double>> electronRECOSF_sys;
    double electronIDSF=1;
    vector<vector<double>> electronIDSF_sys;
    double muonIDSF=1;
    vector<vector<double>> muonIDSF_sys;
    double muonISOSF=1;
    vector<vector<double>> muonISOSF_sys;
    double muonTrackerSF=1;
    double triggerSF=1,triggerSF_up=1,triggerSF_down=1;
    vector<vector<double>> triggerSF_sys;
    double CFSF=1,CFSF_up=1,CFSF_down=1;
    double btagSF=1,btagSF_hup=1,btagSF_hdown=1,btagSF_lup=1,btagSF_ldown=1;
    double JetPU=1;
    double PNETSF=1;
    double EventSetupWeight=1;
  };
  struct Key{
    TString Muon_RECO_SF,Electron_RECO_SF,Electron_ID_SF,Muon_ID_SF, Muon_ISO_SF,Muon_FR,Muon_PR, Electron_FR,Electron_PR, Electron_CF,EMu_Trigger_SF,Electron_Trigger_SF,Muon_Trigger_SF,Muon_CF,Muon_EC_FR,Muon_BB_FR;
    vector<TString> triggerSF;
  };

  Key k;
  Weight w;
  enum Syst{
    Central,
    FRAJUp,FRAJ40,FRAJDown,
    FRHighPtUp,FRHighPtDown,
    FRLooseIDDJUp,FRLooseIDDJDown,
    FRPartonSFUp, FRPartonSFDown,PRApplied,FRAltBinning,
    JetResUp, JetResDown,    JetEnUp, JetEnDown,
    JetAbsoluteStatUp,
    JetAbsoluteStatDown,
    JetAbsoluteScaleUp,
    JetAbsoluteScaleDown,
    JetAbsoluteMPFBiasUp,
    JetAbsoluteMPFBiasDown,
    JetFragmentationUp,
    JetFragmentationDown,
    JetSinglePionECALUp,
    JetSinglePionECALDown,
    JetSinglePionHCALUp,
    JetSinglePionHCALDown,
    JetFlavorQCDUp,
    JetFlavorQCDDown,
    JetTimePtEtaUp,
    JetTimePtEtaDown,
    JetRelativeJEREC1Up,
    JetRelativeJEREC1Down,
    JetRelativeJEREC2Up,
    JetRelativeJEREC2Down,
    JetRelativePtHFUp,
    JetRelativePtHFDown,
    JetRelativePtBBUp,
    JetRelativePtBBDown,
    JetRelativePtEC1Up,
    JetRelativePtEC1Down,
    JetRelativePtEC2Up,
    JetRelativePtEC2Down,
    JetRelativeBalUp,
    JetRelativeBalDown,
    JetRelativeSampleUp,
    JetRelativeSampleDown,
    JetRelativeFSRUp,
    JetRelativeFSRDown,
    JetRelativeStatFSRUp,
    JetRelativeStatFSRDown,
    JetRelativeStatECUp,
    JetRelativeStatECDown,
    JetRelativeStatHFUp,
    JetRelativeStatHFDown,
    JetPileUpDataMCUp,
    JetPileUpDataMCDown,
    JetPileUpPtRefUp,
    JetPileUpPtRefDown,
    JetPileUpPtBBUp,
    JetPileUpPtBBDown,
    JetPileUpPtEC1Up,
    JetPileUpPtEC1Down,
    JetPileUpPtEC2Up,
    JetPileUpPtEC2Down,
    JetPileUpPtHFUp,
    JetPileUpPtHFDown,

    JetMassUp,JetMassDown,
    JetMassSmearUp,JetMassSmearDown,
    JetPNETDown,JetPNETUp,
    HEMJet,
    MuonRecoSFUp,MuonRecoSFDown,
    MuonEnUp,MuonEnDown,    MuonIDSFUp,
    MuonIDSFDown,    MuonISOSFUp,MuonISOSFDown,
    MuonResUp,MuonResDown,
    MuonIDSFStatUp,MuonIDSFStatDown,
    MuonRecoSFStatUp,MuonRecoSFStatDown,
    MuonTriggerSFUp,MuonTriggerSFDown,
    MuonTriggerSFStatUp,MuonTriggerSFStatDown,
    ElectronRecoSFUp,ElectronRecoSFDown,
    ElectronResUp,ElectronResDown,
    ElectronEnUp,ElectronEnDown,
    ElectronIDSFUp,ElectronIDSFDown,
    ElectronTriggerSFUp,ElectronTriggerSFDown,
    ElectronRecoSFStatUp,ElectronRecoSFStatDown,
    ElectronIDSFStatUp,ElectronIDSFStatDown,
    ElectronTriggerSFStatUp,ElectronTriggerSFStatDown,
    BTagSFHTagUnCorrUp,BTagSFHTagUnCorrDown,
    BTagSFLTagUnCorrUp,BTagSFLTagUnCorrDown,
    BTagSFHTagCorrUp,BTagSFHTagCorrDown,
    BTagSFLTagCorrUp,BTagSFLTagCorrDown,
    METUnclUp,METUnclDown,
    CFRateUp,CFRateDown,
    CFSFUp,CFSFDown,
    FRUp,FRDown,
    PrefireUp,PrefireDown,
    PUUp,PUDown,
    JetPUIDUp, JetPUIDDown,
    PDF,PDFUp,PDFDown,
    ScaleUp,ScaleDown,
    NSyst
  };

  Syst syst_;
  TString GetSystType();

  void Clear();

  AnalyzerParameter();
  ~AnalyzerParameter();

  //  AnalyzerParameter& operator=(const AnalyzerParameter& p);


  void PrintParameters();

  double EventWeight();
  double Weight(TString wkey);
  
  inline bool IsCentral() {
    if (syst_ == AnalyzerParameter::Central) return true;
    return false;
  }

  inline TString FakeRateName() {
    return FakeRateMethod + "_"+FakeRateParam;
  }

  TString GetChannelFlavour();

  TString ChannelType();
  void SetChannel(TString inch);
  TString InclusiveChannelName();
  TString CutFlowDirChannel();
  TString CutFlowDirIncChannel();
  TString ChannelDir();
  
  bool  HasFlag(TString JobName) ;

  bool Set_ElIDW;
  bool Set_MuIDW;
  bool Set_ElTrigW;
  bool Set_MuTrigW;

};

#endif
