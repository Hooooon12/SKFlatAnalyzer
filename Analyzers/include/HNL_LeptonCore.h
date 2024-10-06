#ifndef HNL_LeptonCore_h
#define HNL_LeptonCore_h

#include "TRandom.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

#include "AnalyzerCore.h"

class HNL_LeptonCore : public AnalyzerCore {


 public:

  enum NormMC {
    NoNorm,
    NormTo1Invpb,
    NormToXsec
  };

  enum TheRunEra{
    y2016B,y2016C,y2016D,y2016E,y2016F,y2016G,y2016H,
    y2017B,y2017C,y2017D,y2017E,y2017F,
    y2018A,y2018B,y2018C,y2018D,
    y2016MC, y2017MC, y2018MC,
    yUL2016B,yUL2016C,yUL2016D,yUL2016E,yUL2016F,yUL2016Flate,yUL2016G,yUL2016H,
    yUL2017B,yUL2017C,yUL2017D,yUL2017E,yUL2017F,
    yUL2018A,yUL2018B,yUL2018C,yUL2018D,
    yUL2016MCAPV, yUL2016MCnonAPV, yUL2017MC,  yUL2018MC
  };


  enum ChargeType
  {
    Inclusive=0,
    Plus=1,
    Minus=-1,
  };

  enum Channel
  {
    E=0,    EE=1,    EEE=2,    EEEE=3,
    Mu=10,    MuMu=11,    MuMuMu=12,    MuMuMuMu=13,
    EMu=15,     EMuL=16,    EMuLL=17,
    LL=18,NONE=19

  };
  enum SearchRegion
  {
    ChannelDepPresel,    ChannelDepSR1,    ChannelDepSR2,     ChannelDepSR3,    ChannelDepSR3HM,ChannelDepFAILSR3,    ChannelDepDilep,    ChannelDepTrigger,    ChannelDepInc,ChannelDepIncQ,
    ChannelDepCR1,    ChannelDepCR2,    ChannelDepCR3,ChannelDepCR3HM,
    MuonSRSummary,    ElectronSRSummary,    ElectronMuonSRSummary,
    
    MuonSR1,    ElectronSR1,    ElectronMuonSR1,    MuonSR2,    ElectronSR2,    ElectronMuonSR2,    MuonSR3,    ElectronSR3,    ElectronMuonSR3,
    MuonSR3BDT,    ElectronSR3BDT,    ElectronMuonSR3BDT,    

    MuonCR1,    ElectronCR1,    ElectronMuonCR1,    MuonCR2,    ElectronCR2,    ElectronMuonCR2,    MuonCR3,    ElectronCR3,    ElectronMuonCR3,
    MuonCR3BDT,    ElectronCR3BDT,    ElectronMuonCR3BDT,    

    MuonSR,    ElectronSR,    ElectronMuonSR,    MuonCR,    ElectronCR,    ElectronMuonCR,
    MuonSRBDT,    ElectronSRBDT,    ElectronMuonSRBDT,    MuonCRBDT,    ElectronCRBDT,    ElectronMuonCRBDT,
    MuonSROpt,    ElectronSROpt,    ElectronMuonSROpt,    MuonSRBDTOpt,    ElectronSRBDTOpt,    ElectronMuonSRBDTOpt,
    SR1,    SR2,    SR3,    SR3Fail,    SR3BDT,    SR4,    CR1,    CR2,    CR3,CR3BDT, 
    Presel,    PreselSS,    PreselOS,
    sigmm,    sigee,    sigem,   sigmm_17028,    sigee_17028,    sigem_17028,
    ControlRegion,    SignalRegion,  SR,  CR,
    WGCR,    ZGCR, WZCR, ZZCR, ZZVBFCR, WZBCR,WZVBFCR,WZVBFCR2,ZZCR2,HMCR1,HMCR2,HMCR3,HMBDTCR3,HMNPCR,HMBCR,HM1JCR,PreselVBF,
    WWNP1CR, WWNP2CR, WWNP3CR, WWCR1,WWCR2,ZAK8CR,ZCR,ZNPElCR,ZNPMuCR,TopCR,TopNPCR,TopAK8NPCR,TopNPCR2,

  };

  enum LeptonType
  {
    MuonLep=100,
    ElectronLep=101,
    TauLep=102
  };

  HNL_LeptonCore();
  ~HNL_LeptonCore();


  //========== MAIN ANALYZER FUNCTIONS


  // ----- SETUP ANALYZER
  AnalyzerParameter SetupFakeParameter(AnalyzerParameter::Syst SystType,HNL_LeptonCore::Channel channel, HNL_LeptonCore::NormMC norm,vector<TString> s_jobs,TString PNAME, TString IDT, TString IDL);
  AnalyzerParameter InitialiseHNLParameter(TString s_setup);  
  AnalyzerParameter InitialiseHNLParameter(TString s_setup, HNL_LeptonCore::Channel channel);  
  AnalyzerParameter SetupHNLParameter(TString s_setup_version, TString channel_str_name);

  AnalyzerParameter DefaultParam(TString s_setup_version,TString channel_st);

  /// List of Setups
  void GetSetup_POGTight(AnalyzerParameter& paramEv);
  void GetSetup_MVAPOG(AnalyzerParameter& paramEv);
  void GetSetup_HighPt(AnalyzerParameter& paramEv);
  void GetSetup_HNTightV2(AnalyzerParameter& paramEv);
  void GetSetup_HNL16(AnalyzerParameter& paramEv);
  void GetSetup_HNLTopID(AnalyzerParameter& paramEv);
  void GetSetup_HNLID(AnalyzerParameter& paramEv);
  void GetSetup_Peking(AnalyzerParameter& paramEv);
  void GetSetup_HNLOpt(AnalyzerParameter& paramEv);
  void GetSetup_BDT(AnalyzerParameter& paramEv);

  /// Systemaic HNL ID setup
  bool UpdateParamBySyst(TString JobID, AnalyzerParameter& paramEv , AnalyzerParameter::Syst systname, TString OrigParamName);
  
  // ------ Analysis Obj   

  // HNL_LeptonCore_MET

  std::pair<double,double> METXYCorr_Met_MetPhi(double uncormet, double uncormet_phi, int runnb, TString year, bool isMC, int npv, bool isUL =false,bool ispuppi=false);
  std::map<TString, double> cfmap;

  std::map<TString, double> MakeSFmap;
  std::map<TString, double> MakeCFShiftmap;
  map<TString, Particle> METMap(AnalyzerParameter param);


  double ReturnCFWeight(vector<double> CFRates);
  double ReturnCFWeight(double CFRates);

  /// Jet Functions   HNL_LeptonCore_Jet                                                                                                                           
  JetTagging::Parameters GetParamJetTagger(AnalyzerParameter param);
  void  EvalJetWeight(std::vector<Jet>    AK4_JetColl, std::vector<FatJet> fatjets, double & w,AnalyzerParameter& param); 
  TString CloseJetFlavour(std::vector<Jet> jetColl, Lepton* lep);
  TString CloseJetFlavour(std::vector<Jet> jetColl, Muon mu);
  TString CloseJetFlavour(std::vector<Jet> jetColl, Electron el);
  TString CloseJetFlavourInt(std::vector<Jet> jetColl, Lepton* lep);
  TString CloseJetFlavourInt(std::vector<Jet> jetColl, Muon mu);
  TString CloseJetFlavourInt(std::vector<Jet> jetColl, Electron el);

  std::vector<Jet>    GetHNLJets(TString JetType, AnalyzerParameter param);
  std::vector<FatJet> GetHNLAK8Jets(TString JetType, AnalyzerParameter param);
  std::vector<Jet>    SelBJets(std::vector<Jet>& jetColl, JetTagging::Parameters jtp);

  double GetEventFatJetSF(vector<FatJet> fatjets, TString label, int dir);
  double GetFatJetSF(FatJet fatjet, TString tag,  int dir);
  double  GetBJetSF(AnalyzerParameter param,vector<Jet> jets, JetTagging::Parameters jtp);
  double GetJetPileupIDSF(vector<Jet> jets , TString WP, AnalyzerParameter param);



  ///// MET FUNCTIONS  HNL_LeptonCore_MET
  Particle GetvMET(TString METType, AnalyzerParameter param, std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Muon> muons, std::vector<Electron> electrons, bool propsmear=true);
  Particle GetvMET(TString METType, AnalyzerParameter param, bool propsmear=true);
  Particle GetMiniAODvMET(TString METType);
  Particle GetvCorrMET(TString METType, AnalyzerParameter param, Particle METUncorr);

   
  //------ BKG
  double GetPtPartonSF(Lepton Lep, TString ID, AnalyzerParameter param);
  double GetFakeWeightMuon(std::vector<Muon> muons , AnalyzerParameter param);
  double GetFakeWeightMuon(std::vector<Muon> muons , std::vector<TString> vtrig, AnalyzerParameter param);
  double GetFakeWeightElectron(std::vector<Electron> electrons , vector<TString> trigs, AnalyzerParameter param);
  double GetFakeWeightElectron(std::vector<Electron> electrons , AnalyzerParameter param);
  double GetFakeRateElectron(Electron el , AnalyzerParameter param);
  double GetFakeRateMuon(Muon mu, AnalyzerParameter param);
  double GetFakeWeight(std::vector<Lepton *> leps,  AnalyzerParameter param);
  double GetIsoFromID(Lepton lep, TString id);
  double GetCFSF(AnalyzerParameter param, Lepton*  lep, bool ApplySF=true) ;
  double GetCFSF(AnalyzerParameter param, Lepton  lep, bool ApplySF=true) ;
  double GetCFSF(AnalyzerParameter param, TString EtaReg, bool ApplySF=true) ;
  double GetCFWeightElectron(std::vector<Electron> electrons ,  AnalyzerParameter param,   bool ApplySF=true);
  double GetCFWeightElectron(std::vector<Lepton* > leps ,  AnalyzerParameter param,int nEl,bool ApplySF=true);
  double CheckShiftRange(double val, double shift);
  double GetShiftPtMedian(double pt1, double pt2, double shiftVal);

  double GetCFShift(TString ID, TString bintag);
  double GetCFShiftSyst(TString ID, TString bintag,int sys);
  double GetShiftCFEl(Electron el , TString ID, bool ApplyCorr=false, int sys=0) ;
  double GetShiftCFEl(double elpt, bool isbb, TString ID, bool ApplyCorr=false, int sys=0) ;
  double PtExtrap(double val, double x1, double x2, double y1, double y2);
  double GetZMassShift(vector<Electron> Electrons) ;

  int  GetFakeLepSrcType(const Lepton& Lep, vector<Jet>& JetColl);


  //---- BDT Related    HNL_LeptonCore_BDT                                                                                                                                                                                           
  void initializeAnalyzer(bool READBKGHISTS=true, bool SETUPIDBDT=true);
  void SetupEventMVAReader(TString version, bool ee=true, bool mm=true, bool emu=true);
  void SetupEvMVA();
  void DeleteEvMVA();
  
  /// ------ Corrections  HNL_LeptonCore_Corrections                                                                                                                                                
  double HNLZvtxSF(HNL_LeptonCore::Channel ch);
  double HNLZvtxSF(TString  ch);
  bool PassHEMVeto(std::vector<Lepton *> leps);
  
  double GetDYWeakWeight(double mass);

  //  ================= MC weight functions              HNL_LeptonCore_EvCorr.C   =================                           
  TString SetLeptonID(TString lep, AnalyzerParameter p);                                                                                                                                                                                       
  void EvalLeptonIDWeight(std::vector<Lepton *> leps,      AnalyzerParameter& param , double& d);
  void EvalMuonIDWeight     (std::vector<Muon> muons,         AnalyzerParameter& param, double& d );
  void EvalElectronIDWeight (std::vector<Electron> electrons, AnalyzerParameter& param, double& d );
  double GetElectronIDWeight(std::vector<Electron> electrons,AnalyzerParameter param);
  double GetMuonIDWeight(std::vector<Muon> muons,AnalyzerParameter param);

  double GetPileUpWeight(int N_pileup, int syst);
  double GetZ0Weight(double valx);


  //  ================= MC weight functions              HNL_LeptonCore_Lepton.C   =================                                                                                                                                          

  std::vector<Muon> SelectMuons(const std::vector<Muon>& muons,    TString id, double ptmin, double fetamax);
  std::vector<Muon> SelectMuons(TString id, double ptmin, double fetamax);
  std::vector<Muon> SelectMuons(AnalyzerParameter& param, TString id, double ptmin, double fetamax, double& EvWeight);
  std::vector<Muon> SelectMuons(AnalyzerParameter& param, TString id, double ptmin, double fetamax);

  std::vector<Electron> SelectElectrons(const std::vector<Electron>& electrons,
					TString id, double ptmin, double fetamax, bool vetoHEM = false);

  std::vector<Electron> SelectElectrons(TString id, double ptmin, double fetamax, bool vetoHEM = false);
  std::vector<Electron> SelectElectrons(AnalyzerParameter& param, TString id, double ptmin, double fetamax, double& EvWeight, bool vetoHEM=false);
  std::vector<Electron> SelectElectrons(AnalyzerParameter& param, TString id, double ptmin, double fetamax,bool vetoHEM=false);


  std::vector<Tau>    SelectTaus(const std::vector<Tau>& taus,       TString id, double ptmin, double fetamax);
  std::vector<Tau>    SelectTaus(vector<Lepton*> leps, TString id, double ptmin, double fetamax);
  std::vector<Tau>    SelectTaus(TString id, double ptmin, double fetamax);


  vector<Muon>     SkimLepColl(const vector<Muon>& MuColl,     AnalyzerParameter param, TString Option);
  vector<Electron> SkimLepColl(const vector<Electron>& ElColl, AnalyzerParameter param,TString Option);
  vector<Electron> SkimLepColl(const vector<Electron>& ElColl, TString Option);
  vector<Muon>     SkimLepColl(const vector<Muon>& MuColl,     TString Option);

  std::vector<Jet>    SelectJets(AnalyzerParameter param);
  std::vector<Jet>    SelectJets(AnalyzerParameter param,TString ID, double ptmin, double fetamax);
  std::vector<Jet>    SelectJets(TString ID, double ptmin, double fetamax);
  std::vector<Jet>    SelectJets(const std::vector<Jet>& jets,       TString id, double ptmin, double fetamax);

  std::vector<FatJet> SelectFatJets(AnalyzerParameter param);
  std::vector<FatJet> SelectFatJets(AnalyzerParameter param,TString ID, double ptmin, double fetamax);
  std::vector<FatJet> SelectFatJets(TString id, double ptmin, double fetamax);
  std::vector<FatJet> SelectFatJets(const std::vector<FatJet>& jets, TString id, double ptmin, double fetamax);
  vector<Jet>      SkimJetColl(const vector<Jet>& JetColl,     vector<Gen>& TruthColl, AnalyzerParameter param,TString Option);


  //===== Detailed jet selection

  vector<Jet>    SelectAK4Jets(vector<Jet> jets,
                               double pt_cut ,  double eta_cut,
                               bool lepton_cleaning  , double dr_lep_clean, double dr_ak8_clean,   TString pu_tag,
                               std::vector<Lepton *> leps_veto,  vector<FatJet> fatjets);
  vector<Jet>    SelectBJets(AnalyzerParameter param, vector<Jet> jets, JetTagging::Parameters jtp);
  vector<Jet>    SelectLJets(AnalyzerParameter param, vector<Jet> jets, JetTagging::Parameters jtp);
  vector<Jet>    SelectAK4Jets(vector<Jet> jets,
			       double pt_cut ,  double eta_cut,
			       bool lepton_cleaning  , double dr_lep_clean, double dr_ak8_clean, TString pu_tag,
			       vector<Electron>  veto_electrons, vector<Muon>  veto_muons, vector<FatJet> fatjets);

  vector<FatJet> SelectAK8Jets(vector<FatJet> fatjets,
                                 double pt_cut ,  double eta_cut,
                                 bool lepton_cleaning  , double dr_lep_clean , bool apply_tau21, double tau21_cut , bool apply_masscut, double sdmass_lower_cut,  double sdmass_upper_cut, TString  WQCDTagger,
                                 vector<Electron>veto_electrons, vector<Muon>  veto_muons);



  // ZptWeight
  void SetupZptWeight();
  double GetZptWeight(double mass,double rapidity,double pt,TString opt="GYM");
  void DeleteZptWeight();
  TF1* fZptWeightG=NULL;
  vector<TF1*> fZptWeightY;
  TAxis* fZptWeightYaxis=NULL;
  vector<TF1*> fZptWeightM;
  TAxis* fZptWeightMaxis=NULL;



  //=====================                                                                                                                                                                                           
  //==== Tools                                                                                                                                                                                                      
  //====================                                                                                                                                                                                            

  //===== Estimators                                                                                                                                                                                               

  MCCorrection *mcCorr=NULL;
  PuppiSoftdropMassCorr *puppiCorr=NULL;
  FakeBackgroundEstimator *fakeEst=NULL;
  CFBackgroundEstimator *cfEst=NULL;


  // HNL PLOTS  HNL_LeptonCore_Plotter                                                                                                                                     

  /// Function to check which hists to plot whening running systematics
  bool DrawSyst(AnalyzerParameter param_sys);

  void FillMuonCollPlots     (bool passSel, TString sel, AnalyzerParameter param, TString cut,  std::vector<Muon> muons, double w);
  void FillMuonCollPlots     (AnalyzerParameter param, TString cut,  std::vector<Muon> muons, double w);
  void FillMuonPlots         (bool passSel, TString sel, AnalyzerParameter param, TString cut,  Muon muon, double w);
  void FillMuonPlots         (AnalyzerParameter param, TString cut,  Muon muon, double w);
  void FillMuonKinematicPlots(AnalyzerParameter param, TString cut,  Muon mu, double w);
  void FillAllElectronPlots  (AnalyzerParameter param, TString cut,  std::vector<Electron> els, double w);
  void FillElectronPlots     (AnalyzerParameter param, TString cut,  std::vector<Electron> els, double w);
  void FillElectronKinematicPlots(AnalyzerParameter param, TString cut,  Electron el, double w);
  void FillLeptonKinematicPlots  (AnalyzerParameter param, TString cut,  Lepton lep, double w);

  void FillLeptonPlots(AnalyzerParameter param,std::vector<Lepton *> leps, TString this_region, double weight);
  void FillJetPlots   (AnalyzerParameter param,std::vector<Jet> jets, std::vector<FatJet> fatjets, TString this_region, double weight);
 
  double FillWeightHist(TString label, double _weight);
  double FillFakeWeightHist(TString label, vector<Lepton *> Leps,AnalyzerParameter param,  double _weight);
  void FillFakeHistograms(AnalyzerParameter param, TString Label, vector<Lepton* > Leptons, vector<Jet> JetColl, vector<FatJet> FatJetColl,vector<Jet> BJetColl, Particle MET, double w);

  

  //// ===============================  SR PLOTS =============================== ////                                                                                                                                                                                                                                                                                                       
  void Fill_RegionPlots(AnalyzerParameter param, TString plot_dir,  std::vector<Jet> jets,    std::vector<FatJet> fatjets, std::vector<Lepton *> leps , Particle  met, double nvtx,  double w); 
  void Fill_RegionPlots(AnalyzerParameter param, TString plot_dir, vector<Tau> Taus,  std::vector<Jet> jets,    std::vector<FatJet> fatjets, std::vector<Lepton *> leps , Particle  met, double nvtx,  double w); 

  void Fill_Plots(AnalyzerParameter param, TString region, TString plot_dir, vector<Tau> Taus,  std::vector<Jet> jets,    std::vector<FatJet> fatjets, std::vector<Lepton *> leps , Particle  met, double 
nvtx,  double w); 
  void Fill_PlotsAK8(AnalyzerParameter param, TString region,TString plot_dir,  std::vector<Tau> Taus, std::vector<Jet> jets,  std::vector<FatJet> fatjets,  std::vector<Lepton *> leps, Particle  met, double nvtx, double w);

  void Fill_SigRegionPlots1(HNL_LeptonCore::Channel channel,TString label_1, TString label_2, TString label_3,  std::vector<Jet> jets, std::vector<FatJet> fatjets,  std::vector<Lepton *> leps, Particle  met, double nvtx, double w, double var1,  double var2, double var3, double var4, double var5, double var6, double var7);
  void Fill_SigRegionPlots3(HNL_LeptonCore::Channel channel, TString label_1, TString label_2, TString label_3,  std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Lepton *> leps , Particle  met, double nvtx, double w, double var1,  double var2, double var3, double var4, double var5, double var6, double var7, double var8, double var9, double var10, double var11);
  void Fill_SigRegionPlots4(HNL_LeptonCore::Channel channel,TString label_1,std::vector<Jet> jets, std::vector<FatJet> fatjets,  std::vector<Lepton *> leps, Particle  met, double nvtx, double w );
  bool  Fill_DefSigRegionPlots3(HNL_LeptonCore::Channel channel, TString label_sr, TString label_mass, TString label_anid,  std::vector<Jet> jets, std::vector<FatJet> fatjets,  std::vector<Lepton *> leps, Particle  met, double nvtx,  double w,  double var1,  double var2, double var3, double var4, double var5, double var6, double var7, double var8, double var9, double var10, double var11);
  void Fill_All_SignalRegion3(HNL_LeptonCore::Channel channel, TString signal_region, bool isdata, TString charge_s, TString label, std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Lepton *> leps,  Particle _met,int _npv , double w , bool full);
  void Fill_All_SignalRegion1(HNL_LeptonCore::Channel channel, TString signal_region, bool isdata, TString charge_s, TString label, std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Lepton *> leps,  Particle _met,int _npv , double w , bool full);


  // === Cut flow  HNL_LeptonCore_CutFlow                                                                                                                                                                         
  void FillTypeCutflow(TString histname, double weight, vector<TString> lables, TString label1, TString label2);
  void FillFullTypeCutflow(TString histname, double weight, vector<TString> lables, TString label1, TString label2);
  
  void FillCutflowDef(TString dirname, TString histname, double weight, vector<TString> lables, TString label);
  void FillCutflow(TString analysis_dir_name,TString histname, double weight, vector<TString> lables, TString label);

  void FillCutflow(AnalyzerParameter param,TString histname, double weight, vector<TString> lables, TString label);
  void FillCutflow(HNL_LeptonCore::SearchRegion sr, double event_weight, TString label,  AnalyzerParameter param);
  void FillCutflow(HNL_LeptonCore::SearchRegion sr, double event_weight, TString label,  TString hist_path);
  void FillLimitInput(HNL_LeptonCore::SearchRegion sr, double event_weight, TString label,  TString hist_path);

  vector<TString>  GetLabelsFromRegion(HNL_LeptonCore::SearchRegion sr);
  TString GetCutFlowNameFromRegion(HNL_LeptonCore::SearchRegion sr);
  void FillCutFlow(bool IsCentral, TString suffix, TString histname, double weight);

  /// BDT Variables and Functions HNL_LeptonCore_BDT                                                                                                                                                                                                                                                                                                                                                                                                                                
  vector<TString> MNStrList, NCutList, NTreeList;
  map<TString, std::pair<TString,TString> > FinalBDTHyperParamMap;
  TMVA::Reader *MVAReader;
  TMVA::Reader *MVAReaderMM, *MVAReaderEE, *MVAReaderEM;
  TMVA::Reader *MVAReaderMMFake, *MVAReaderEEFake, *MVAReaderEMFake, *MVAReaderMMNonFake, *MVAReaderEENonFake, *MVAReaderEMNonFake;

  /// Map to store booked settings
  map<TString, TString > map_bdt_booked;


  /// Event BDT var 
  void InitializeTreeVars();
  void SetupEventBDTVariables(std::vector<Lepton *> LepTColl,    std::vector<Jet> JetAllColl,std::vector<Jet> JetColl, std::vector<Jet> JetVBFColl, std::vector<Jet> B_JetColl, Event  ev, Particle METv, AnalyzerParameter param);

  double EvaluateEventMVA(TString mN, TString bkgType, TString NCut, TString NTree, HNL_LeptonCore::Channel channel,
                          std::vector<Lepton *> LepTColl,
                          Event ev, Particle METv, AnalyzerParameter param, double weight=1., bool isVarPlots=false);

  Float_t ev_bdt_Nj, ev_bdt_Nvbfj, ev_bdt_Nb, ev_bdt_LQ;
  Float_t ev_bdt_Ptl1, ev_bdt_Ptl2, ev_bdt_Ptj1, ev_bdt_Ptj2, ev_bdt_Ptj3, ev_bdt_MET, ev_bdt_HT, ev_bdt_LT, ev_bdt_HTLT, ev_bdt_HTLT1, ev_bdt_HTLT2, ev_bdt_MET2ST, ev_bdt_MET2HT, ev_bdt_Etal1, ev_bdt_Etal2;
  Float_t ev_bdt_PtRatioAJl1, ev_bdt_PtRatioAJl2;
  Float_t ev_bdt_CEMFracAJl1, ev_bdt_CEMFracAJl2, ev_bdt_NEMFracAJl1, ev_bdt_NEMFracAJl2, ev_bdt_CHFracAJl1, ev_bdt_CHFracAJl2, ev_bdt_NHFracAJl1, ev_bdt_NHFracAJl2, ev_bdt_MuFracAJl1, ev_bdt_MuFracAJl2, ev_bdt_JetDiscAJl1, ev_bdt_JetDiscAJl2;
  Float_t ev_bdt_dEtall, ev_bdt_dRll, ev_bdt_dRjj12, ev_bdt_dRjj23, ev_bdt_dRjj13;
  Float_t ev_bdt_dRlj11, ev_bdt_dRlj12, ev_bdt_dRlj13, ev_bdt_dRlj21, ev_bdt_dRlj22, ev_bdt_dRlj23;
  Float_t ev_bdt_dphivl1, ev_bdt_dphivl2, ev_bdt_dphivj1, ev_bdt_dphivj2, ev_bdt_dphivj3;
  Float_t ev_bdt_MSSSF, ev_bdt_Mlj11, ev_bdt_Mlj12, ev_bdt_Mlj13, ev_bdt_Mlj21, ev_bdt_Mlj22, ev_bdt_Mlj23;
  Float_t ev_bdt_MTvl1, ev_bdt_MTvl2, ev_bdt_MTvll, ev_bdt_Mllj1, ev_bdt_Mllj2, ev_bdt_Mllj3, ev_bdt_Mllj4;
  Float_t ev_bdt_Mlljj12, ev_bdt_Mlljj13, ev_bdt_Mlljj14, ev_bdt_Mlljj23, ev_bdt_Mlljj24, ev_bdt_Mlljj34;
  Float_t ev_bdt_Mljj112, ev_bdt_Mljj113, ev_bdt_Mljj114, ev_bdt_Mljj123, ev_bdt_Mljj124, ev_bdt_Mljj134;
  Float_t ev_bdt_Mljj212, ev_bdt_Mljj213, ev_bdt_Mljj214, ev_bdt_Mljj223, ev_bdt_Mljj224, ev_bdt_Mljj234;
  Float_t ev_bdt_Mjj12, ev_bdt_Mjj13, ev_bdt_Mjj14, ev_bdt_Mjj23, ev_bdt_Mjj24, ev_bdt_Mjj34;
  Float_t ev_bdt_PtWj1, ev_bdt_PtWj2;
  Float_t ev_bdt_dRWjj, ev_bdt_dRlW12, ev_bdt_dRlW22, ev_bdt_dRlN12, ev_bdt_dRlN21;
  Float_t ev_bdt_M_W2_jj, ev_bdt_M_N1_l1jj, ev_bdt_M_N2_l2jj, ev_bdt_M_W1_lljj;
  Float_t w_tot;


  double MergeMultiMC(vector<TString> vec, TString Method);
  void OutCutFlow(TString lab, double w);
  TString QToString(HNL_LeptonCore::ChargeType q);
  bool CheckLeptonFlavourForChannel(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps);
  bool PassGenMatchFilter(vector<Lepton *> leps,AnalyzerParameter param);
  bool HasLowMassMeson(std::vector<Lepton *> leps);
  double SetupWeight(Event ev, AnalyzerParameter& param);
  TString GetPtBin(bool mu, double pt);
  TString DoubleToString(double d);

  HNL_LeptonCore::Channel GetTriLeptonChannel(HNL_LeptonCore::Channel channel);
  HNL_LeptonCore::Channel GetQuadLeptonChannel(HNL_LeptonCore::Channel channel);
  HNL_LeptonCore::Channel GetChannelENum(TString ch);



  /// ------ SYSTEMATICS
  vector<AnalyzerParameter::Syst> GetSystList(TString SystType="");

  /// ---- TRIGGER HNL_LeptonCore_Trigger
  void    SetupTriggerLists();  
  void    TriggerPrintOut(Event ev);
  double  GetPtCutTrigger(TString trigname, int nlep, TString flavour);
  TString Category(Electron el);
  bool    PassPtTrigger(Event ev, vector<TString> triglist,std::vector<Lepton *> leps, bool check_pd=true);
  bool    PassTriggerSelection(HNL_LeptonCore::Channel channel,Event ev, std::vector<Lepton *> leps,  TString selection, bool check_pd=true);
  bool    PassMultiDatasetTriggerSelection(HNL_LeptonCore::Channel channel,Event ev, std::vector<Lepton *> leps, TString selectionMain, TString selectionOR);
  bool    PassTriggerAndCheckStream(bool apply_ptcut,vector<Lepton*> leps, Event ev, vector<TString> triglist, bool check_pd=true);
  void    EvalTrigWeight(HNL_LeptonCore::Channel channel, vector<Muon> muons, vector<Electron> electrons, AnalyzerParameter& p, Event ev,double& w) ;


  ///=============== GET/SELECT OBJECT
  vector<Gen> GetGenLepronsSignal();
  bool SelectChannel(HNL_LeptonCore::Channel channel);
  TString GetProcess();
  TString GetChannelString(HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType  q=HNL_LeptonCore::ChargeType::Inclusive);
  Particle GetSignalObject(TString obj, TString Sig);

  vector<Muon> GetSignalLeptons(const std::vector<Muon>& MuColl, vector<Gen>& TruthColl);
  vector<Electron> GetSignalLeptons(const std::vector<Electron>& ElColl, vector<Gen>& TruthColl);
  vector<Muon> GetLepCollByRunType(const vector<Muon>& MuColl,  AnalyzerParameter& param,  TString Option="");
  vector<Electron> GetLepCollByRunType(const vector<Electron>& ElColl,AnalyzerParameter& param,  TString Option="");

  //================== KINEMATIC HELPER
  double M_T(Particle a, Particle b);
  double GetLT(std::vector<Lepton *> leps);
  double GetHT( std::vector<Jet> jets, std::vector<FatJet> fatjets);
  double GetST( std::vector<Electron> electrons, std::vector<Muon> muons, std::vector<Jet> jets, std::vector<FatJet> fatjets, Particle met);
  double GetST( std::vector<Lepton *> leps, std::vector<Jet> jets, std::vector<FatJet> fatjets, Particle met);
  double GetMET2ST( std::vector<Lepton *> leps, std::vector<Jet> jets, std::vector<FatJet> fatjets, Particle met);

  bool  IsOSSF(Lepton * leps1, Lepton * leps2);
  bool  ZmasslllWindowCheck(std::vector<Lepton *> leps, double MassCut);
  bool  ZmassOSSFWindowCheck(std::vector<Lepton *> leps, double mass_diff);

  double GetMassMinOSSF(std::vector<Lepton *> leps);
  double GetMassMinSSSF(std::vector<Lepton *> leps);
  int GetIndexNonMinOSSF(std::vector<Lepton *> leps);
  int GetIndexNonMinSSSF(std::vector<Lepton *> leps);
  int GetIndexNonBestZ(std::vector<Lepton *> leps,double mass_diff);

  double GetLLMass(std::vector<Lepton *> leps);
  double GetLLMass(std::vector<Muon> leps);
  double GetLLMass(std::vector<Electron> leps);

  double GetMassBestZ(std::vector<Lepton *> leps,  bool bestZ);
  double GetRecoObjMass(TString type , std::vector<Jet> jets, std::vector<FatJet> fatjets, vector<Lepton*> leps);


  double CalulateMaxZeppenfeld(std::vector<Lepton *> leps, double Av_JetEta, double maxDiJetDeta);
  double CalulateZeppenfeld(Lepton * lep, double Av_JetEta, double maxDiJetDeta);  

  pair<int,int> LeptonPairBestZCand(std::vector<Lepton *> leps);
  double LeptonMassBestZ(std::vector<Lepton *> leps, pair<int,int> Zind);
  double LeptonMassNonZ(std::vector<Lepton *> leps, pair<int,int> Zind);

  bool HasLowMassOSSF(std::vector<Lepton *> leps, double MassCut);
  bool HasOSSFPair(std::vector<Lepton *> leps);

  //================                                                                                                                                                                                        
  //==== Functions                                                                                                                                                                                          
  //================                                                                                                                                                                                        

  vector<TString> ConvertCutFlowLabels(vector<TString> SRlabels);
  bool    IsExists(TString filepath);

  //==== LEPTON CHARGE                                                                                                                                                                                           
  bool SameCharge(vector<Electron> els, int ch=0);
  bool SameCharge(vector<Electron> els, vector<Muon> mus,  int ch=0);
  bool SameCharge(vector<Muon> mus, int ch=0);
  bool SameCharge(std::vector<Lepton *> leps, int ch=0);

  /// global var for user flags
  bool RunPrompt,RunFake,RunOSFake,RunFakeTF, RunCF,  RunConv, RunSyst,RunPromptTLRemoval,run_ORTrigger,RunFullSyst;
  bool RunEE;
  bool RunEMu;
  bool RunMuMu;
  bool IsSkimmed, Signal, HEM1516 ,BeforeRun319077;
  bool DEBUG,IsCentral, RunFullAnalysis;
  TRandom3* rand_;
  
  // Trigger vectors used to store year dependant trigger lists
  
  vector<TString> TrigList_HNL_HighPtMu;
  vector<TString> TrigList_HNL_DblMu;
  vector<TString> TrigList_HNL_Mu;
  vector<TString> TrigList_Full_Mu;
  vector<TString> TrigList_POG_Mu;
  
  // EG
  vector<TString> TrigList_HNL_DblEG;
  vector<TString> TrigList_HNL_EG;
  vector<TString> TrigList_POG_EG;
  vector<TString> TrigList_HNL_HighPtEG;

  vector<TString> TrigList_Full_EG;
  
  // MUG
  vector<TString> TrigList_HNL_MuEG;
  vector<TString> TrigList_POG_MuEG;
  vector<TString> TrigList_Full_MuEG;
  
  vector<TString> TrigList_HNL_EGMu;
  vector<TString> TrigList_POG_EGMu;
  vector<TString> TrigList_Full_EGMu;

  
  // Lepton ID
  vector<TString> MuonVetoIDs;
  vector<TString> MuonLooseIDs;
  vector<TString> MuonTightIDs;
  vector<TString> ElectronVetoIDs;
  vector<TString> ElectronLooseIDs;
  // Fake rate file
  vector<TString> FakeRateIDs;
  vector<TString> ElectronTightIDs;

  // Lepton pT cut

  // obj vectors

  double weight_Prefire, weight_Prefire_Up, weight_Prefire_Down;


    //// ===============================  TOOLS =============================== ////                                                                                                                 
		   
  double GetFilterEffType1(TString SigProcess, int mass);
  double GetXsec(TString SigProcess, int mass);
 

  TH1D *hist_PUReweight;
  TH1D *hist_PUReweight_Up;
  TH1D *hist_PUReweight_Down;

  double weight_PU, weight_PU_Up, weight_PU_Down;


};



#endif

