#ifndef AnalyzerCore_h
#define AnalyzerCore_h

#include "TLorentzVector.h"
#include "TString.h"
#include "TMath.h"
#include "TH3.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include <sstream>      
#include <ctime>

#include "SKFlatNtuple.h"
#include "Event.h"
#include "Particle.h"
#include "Gen.h"
#include "LHE.h"
#include "Lepton.h"
#include "Muon.h"
#include "Electron.h"
#include "Tau.h"
#include "Photon.h"
#include "JetTaggingParameters.h"
#include "Jet.h"
#include "FatJet.h"

#include "AnalyzerParameter.h"
#include "MCCorrection.h"
#include "PuppiSoftdropMassCorr.h"
#include "FakeBackgroundEstimator.h"
#include "CFBackgroundEstimator.h"
#include "GeneralizedEndpoint.h"
#include "GEScaleSyst.h"
#include "PDFReweight.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"


#define M_Z 91.1876
#define M_W 80.379

class AnalyzerCore: public SKFlatNtuple {

public:

  AnalyzerCore();
  ~AnalyzerCore();

  virtual void initializeAnalyzer(){

  };

  virtual void executeEvent(){

  };

  enum BkgType
  {
    Fake=0,
    Conv=1,
    CF=2,
  };


  void beginEvent();



  //==================
  //==== Get objects
  //==================

  //==== When GetAllMuons, we apply Rochester correciton
  //==== Then, Pt orderding can be changed
  //==== So, in the analysis code, do e.g.,
  // vector<Muon> muons = GetMuons(~~~);
  // std::sort(muons.begin(), muons.end(), PtComparing);
  //==== ** Recommend you to do the same for other objects (Electron, Jet, FatJet, ...) **
  inline static bool PtComparing(const Particle& p1, const Particle& p2){ return (p1.Pt() > p2.Pt()); }
  inline static bool PtComparingPtr(Particle* p1, Particle* p2){ return (p1->Pt() > p2->Pt()); }

  Event GetEvent();

  // MC weight functions                                                                                                                         
  double GetLeptonSFEventWeight   (std::vector<Lepton *> leps,AnalyzerParameter param );
  double GetMuonSFEventWeight    (std::vector<Muon> muons,AnalyzerParameter param );
  double GetElectronSFEventWeight(std::vector<Electron> electrons, AnalyzerParameter param );

  // FR functions                                                                                                                                
  double GetFakeWeightMuon(std::vector<Muon> muons , AnalyzerParameter param);
  double GetFakeWeightMuon(std::vector<Muon> muons , std::vector<TString> vtrig, AnalyzerParameter param);
  double GetFakeWeightElectron(std::vector<Electron> electrons , vector<TString> trigs, AnalyzerParameter param);
  double GetFakeWeightElectron(std::vector<Electron> electrons , AnalyzerParameter param);
  double GetFakeRateEl(double eta, double pt, AnalyzerParameter param);
  double GetFakeRateM(double eta, double pt, AnalyzerParameter param);
  double GetFakeWeight(std::vector<Lepton *> leps,  AnalyzerParameter param, bool apply_r);

  double GetCFWeightElectron(std::vector<Electron> electrons ,  AnalyzerParameter param);
  double GetCFWeightElectron(vector<double> el_pt, vector<double> el_eta ,  AnalyzerParameter param);
  double GetCFWeightElectron(std::vector<Lepton* > leps ,  AnalyzerParameter param);
  double GetCFrates(TString id, double pt, double eta);
  double GetCFWeightElectron(vector<Lepton *> lepptrs, AnalyzerParameter param, bool applySF, int syst);
  std::vector<Electron> GetAllElectrons();
  std::vector<Electron> GetElectrons(TString id, double ptmin, double fetamax, bool vetoHEM = false);
  std::vector<Electron> GetElectrons(AnalyzerParameter param, TString id, double ptmin, double fetamax ,bool Run_Fake=false, bool vetoHEM=false);

  std::vector<Electron> GetElectrons(AnalyzerParameter param  ,bool Run_Fake=false, bool vetoHEM = false);
  std::vector<Muon> GetAllMuons();
  std::vector<Muon> GetMuons(TString id, double ptmin, double fetamax);
  std::vector<Muon> GetMuons(AnalyzerParameter param, TString id, double ptmin, double fetamax,bool Run_Fake=false);
  std::vector<Muon> GetMuons(AnalyzerParameter param,bool Run_Fake=false);

  std::vector<Tau> GetAllTaus();
  std::vector<Tau> GetTaus(vector<Lepton*> leps, TString id, double ptmin, double fetamax);
  std::vector<Tau> GetTaus(TString id, double ptmin, double fetamax);

  std::vector<Photon> GetAllPhotons();
  std::vector<Photon> GetPhotons(TString id, double ptmin, double fetamax);



  TMVA::Reader *MuonIDFakeMVAReader;
  TMVA::Reader *MuonIDFakeNoPtMVAReader;
  TMVA::Reader *ElectronIDFakeMVAReader;
  TMVA::Reader *ElectronIDCFMVAReader;
  TMVA::Reader *ElectronIDConvMVAReader;
  TMVA::Reader *ElectronIDv2FakeMVAReader;
  TMVA::Reader *ElectronIDv2ConvMVAReader;
  TMVA::Reader *ElectronIDv2CFMVAReader;
  TMVA::Reader *ElectronIDv2CFMVAReaderPt;
  TMVA::Reader *ElectronIDv3CFMVAReader;

  bool PtOrderObj;

  // ID MVA                                                                                            
  void InitializeIDTreeVars();
  void InitializeElectronIDTreeVars();
  void PrintBDTInput();

  /// For adding var to Trees
  void SetupLeptonBDTSKFlat();
  void ResetLeptonBDTSKFlat();
  void InitialiseLeptonBDTSKFlat();

  /// Use in analysis 
  void SetupLeptonBDT();
  void SetBDTIDVar(Lepton*  lep);
  void SetBDTIDVarV1(Lepton*  lep);
  void SetBDTIDVariablesElectron(Electron el);
  void SetBDTIDVariablesMuon(Muon mu);
  void SetupIDMVAReaderMuon();
  void SetupIDMVAReaderElectron(bool v1, bool v2);
  void SetupIDMVAReaderDefault();

  void CompareBDT(TString var1, double a, double b,  int ilep );


  /// Var for ID BDTs
  Float_t bdt_id_Pt,        bdt_id_Eta,      bdt_id_PtBinned; //3
  Float_t bdt_id_PtRatio,   bdt_id_PtRel   , bdt_id_MassDrop; //3
  Float_t bdt_id_CEMFracCJ, bdt_id_NEMFracCJ,bdt_id_CHFracCJ, bdt_id_NHFracCJ, bdt_id_MuFracCJ, bdt_id_JetDiscCJ; //6
  Float_t bdt_id_Dxy,       bdt_id_Dz,       bdt_id_DxySig, bdt_id_DzSig, bdt_id_RelIso,bdt_id_IP3D,bdt_id_MVA,bdt_id_MVAIso,bdt_id_Chi2, bdt_id_Minireliso; //10
  Float_t bdt_id_Full5x5_sigmaIetaIeta,bdt_id_dEtaSeed,bdt_id_dPhiIn,bdt_id_HoverE,bdt_id_Rho,bdt_id_TrkIso,bdt_id_InvEminusInvP,bdt_id_ecalPFClusterIso,bdt_id_hcalPFClusterIso; //9
  Float_t bdt_id_RelDxy,bdt_id_RelDz,bdt_id_RelIP3D,bdt_id_RelMVA,bdt_id_RelMVAIso,bdt_id_PileUp;//6
  Float_t bdt_id_R9,bdt_id_dr03TkSumPt,bdt_id_dr03HcalTowerSumEt,bdt_id_dr03HcalDepth1TowerSumEt,bdt_id_dr03EcalRecHitSumEt, bdt_id_e2x5OverE5x5,bdt_id_e1x5OverE5x5;//7
  Float_t bdt_id_e15,bdt_id_e25,bdt_id_e55,bdt_id_EtaWidth,bdt_id_PhiWidth,bdt_id_dEtaIn, bdt_id_MiniIsoChHad,bdt_id_MiniIsoNHad,bdt_id_MiniIsoPhHad,bdt_id_IsoChHad,bdt_id_IsoNHad,bdt_id_IsoPhHad;//12
  Float_t bdt_id_RelMiniIsoCh,bdt_id_RelMiniIsoN,bdt_id_EoverP,bdt_id_FBrem;//4
  //Float_t fIsEcalDriven,Pixel_hits,  fValidhits,fMatched_stations,fTracker_layers,fMissingHits;//6
  //Float_t fPassConversionVeto,fIsGsfCtfScPixChargeConsistent, fIsGsfScPixChargeConsistent, fIsGsfCtfChargeConsistent;//4
  //Float_t fPOGTight, fPOGMedium,fHNTightID;
  Float_t bdt_id_w_id_tot;

  
  Float_t bdt_id_POGTight, bdt_id_POGMedium,bdt_id_HNTightID;
  Float_t bdt_id_isEcalDriven,bdt_id_Pixel_hits,  bdt_id_Validhits,bdt_id_Matched_stations,bdt_id_Tracker_layers,bdt_id_MissingHits;//6                                                                                                    
  Float_t bdt_id_PassConversionVeto,bdt_id_IsGsfCtfScPixChargeConsistent, bdt_id_IsGsfScPixChargeConsistent, bdt_id_IsGsfCtfChargeConsistent;//4                                                                                                                                                          
  
  /// ID functions MVA
  double GetBDTScoreEl(Electron el ,BkgType bkg,TString bdttag);
  double GetBDTScoreElV1(Electron el ,BkgType bkg,TString bdttag);
  double GetBDTScoreMuon(Muon mu ,BkgType bkg, TString bdttag="BDTA");
  double GetBDTScoreEl_EtaDependant(Electron el ,BkgType bkg, TString BDTTag);
  double GetBDTScoreMuon_EtaDependant(Muon el ,BkgType bkg, TString BDTTag);


  double GetIsoFromID(TString type_lep, TString id, double eta, double pt);

  //==== If TightIso is set, it calculate ptcone
  //==== If UseMini is true, Lepton::RelIso() returns MiniRelIso
  std::vector<Lepton *> MakeLeptonPointerVector(const std::vector<Muon>& muons,const std::vector<Electron>& electrons, double TightIso=-999, bool UseMini=false);
  std::vector<Lepton *> MakeLeptonPointerVector(const std::vector<Muon>& muons,const std::vector<Electron>& electrons, AnalyzerParameter param, double TightIso=-999, bool UseMini=false);

  std::vector<Lepton *> MakeLeptonPointerVector(const std::vector<Muon>& muons, double TightIso=-999, bool UseMini=false);
  std::vector<Lepton *> MakeLeptonPointerVector(const std::vector<Electron>& electrons, double TightIso=-999, bool UseMini=false);

  std::vector<Jet> GetAllJets(bool applyCorr=true);
  std::vector<Jet> GetJets(AnalyzerParameter param);
  std::vector<Jet> GetJets(AnalyzerParameter param,TString ID, double ptmin, double fetamax);
  std::vector<Jet> GetJets(TString ID, double ptmin, double fetamax);

  double GetJetPileupIDSF(vector<Jet> jets , TString WP, AnalyzerParameter param);

  std::vector<FatJet> GetAllFatJets();
  std::vector<FatJet> GetFatJets(AnalyzerParameter param);
  std::vector<FatJet> GetFatJets(AnalyzerParameter param,TString ID, double ptmin, double fetamax);
  std::vector<FatJet> GetFatJets(TString id, double ptmin, double fetamax);

  std::vector<Gen> GetGens();
  std::vector<LHE> GetLHEs();

  Jet GetCorrectedJetCloseToLepton(Lepton lep, Jet jet);
  Jet GetCorrectedJetCloseToLepton(Muon lep, Jet jet);
  Jet GetCorrectedJetCloseToLepton(Electron lep, Jet jet);

  double  JetLeptonMassDropLepAware(  Muon lep, bool removeLep,bool ApplyCorr=false);
  double  JetLeptonMassDropLepAware(  Electron lep, bool removeLep,bool ApplyCorr=false);

  double  JetLeptonPtRelLepAware(  Lepton lep, Jet jet);
  double  JetLeptonPtRelLepAware(  Lepton lep);
  double  JetLeptonPtRelLepAware(  Electron lep);
  double  JetLeptonPtRelLepAware(  Muon lep,     bool CorrLep=false);

  double  JetLeptonPtRatioLepAware( Lepton lep, Jet jet);
  double  JetLeptonPtRatioLepAware( Lepton lep);
  double  JetLeptonPtRatioLepAware( Muon lep, bool CorrLep=false);
  double  JetLeptonPtRatioLepAware( Electron lep);

  std::vector<Lepton *> LeptonUsePtParton(std::vector<Lepton *> leps);

  bool ConversionSplitting(std::vector<Lepton *> leps);
  bool ConversionVeto(std::vector<Lepton *> leps,const std::vector<Gen>& gens);
  bool ConversionVeto_Backup(std::vector<Lepton *> leps,const std::vector<Gen>& gens);
  bool IsCF(Electron el, std::vector<Gen> gens);
  bool IsCF(Muon mu, std::vector<Gen> gens);


  //===================================================
  //==== Get objects METHOD 2
  //==== Get AllObject in the begining, and apply cut
  //==================================================+

  std::vector<Electron> SelectElectrons(const std::vector<Electron>& electrons, TString id, double ptmin, double fetamax, bool vetoHEM = false);
  std::vector<Electron> SelectElectrons(const std::vector<Electron>& electrons, TString id, double ptmin, double fetamax, bool cc, double dx_b, double dx_e,double dz_b,double dz_e, double iso_b, double iso_e);


  std::vector<Muon> UseTunePMuon(const std::vector<Muon>& muons);
  std::vector<Muon> SelectMuons(const std::vector<Muon>& muons, TString id, double ptmin, double fetamax);

  std::vector<Tau> SelectTaus(const std::vector<Tau>& taus, TString id, double ptmin, double fetamax);


  std::vector<Jet> SelectJets(const std::vector<Jet>& jets, TString id, double ptmin, double fetamax);

  std::vector<FatJet> SelectFatJets(const std::vector<FatJet>& jets, TString id, double ptmin, double fetamax);

  //==== BJets                                                                                                                                   
  //pair<int,double> GetNBJets(vector<Jet>       jets,  TString WP="Medium", TString method="2a");
  //pair<int,double> GetNBJets(vector<Jet>       jets,  AnalyzerParameter param, TString WP="Medium");
  //pair<int,double> GetNBJets(TString ID,              TString WP="Medium", TString method="2a");
  //pair<int,double> GetNBJets(AnalyzerParameter param, TString WP="Medium");
  // int GetNBJets2a(TString ID, TString WP="Medium");
  //int GetNBJets2a( vector<Jet> jets, TString WP="Medium");
  //int GetNBJets2a( vector<Jet> jets, AnalyzerParameter param,TString WP="Medium");

  //===== AK8 Jet Syst + weights                                                                                                                 

  double GetEventFatJetSF(vector<FatJet> fatjets, TString label, int dir);
  double GetFatJetSF(FatJet fatjet, TString tag,  int dir);


  //===== Detailed jet selection                                                                                                                 
  vector<Jet>   SelectAK4Jets(vector<Jet> jets, double pt_cut ,  double eta_cut, bool lepton_cleaning  , double dr_lep_clean, double dr_ak8_clean,   TString pu_tag,std::vector<Lepton *> leps_veto,  vector<FatJet> fatjets);

  double  GetBJetSF(AnalyzerParameter param,vector<Jet> jets, JetTagging::Parameters jtp);
  vector<Jet>   SelectBJets(AnalyzerParameter param, vector<Jet> jets, JetTagging::Parameters jtp);
  vector<Jet>   SelectLJets(AnalyzerParameter param, vector<Jet> jets, JetTagging::Parameters jtp);


  vector<Jet>  SelectAK4Jets(vector<Jet> jets, double pt_cut ,  double eta_cut, bool lepton_cleaning  , double dr_lep_clean, double dr_ak8_clean, TString pu_tag, vector<Electron>  veto_electrons, vector<Muon>  veto_muons, vector<FatJet> fatjets);
  
  vector<FatJet> SelectAK8Jetsv2(vector<FatJet> fatjets, double pt_cut ,  double eta_cut, bool lepton_cleaning  , double dr_lep_clean , bool apply_tau21, double tau21_cut , bool apply_masscut, double sdmass_lower_cut,  double sdmass_upper_cut, TString  WQCDTagger,  vector<Electron>  veto_electrons, vector<Muon>  veto_muons);

  vector<FatJet> SelectAK8Jets(vector<FatJet> fatjets, double pt_cut ,  double eta_cut, bool lepton_cleaning  , double dr_lep_clean , bool apply_tau21, double tau21_cut , bool apply_masscut, double sdmass_lower_cut,  double sdmass_upper_cut,   vector<Electron>  veto_electrons, vector<Muon>  veto_muons);

  //==================
  //==== Systematics
  //==================

  std::vector<Electron> ScaleElectrons(const std::vector<Electron>& electrons, int sys);
  std::vector<Electron> SmearElectrons(const std::vector<Electron>& electrons, int sys);

  std::vector<Muon> ScaleMuons(const std::vector<Muon>& muons, int sys);

  std::vector<Jet> ScaleJets(const std::vector<Jet>& jets, int sys);
  std::vector<Jet> ScaleJetsIndividualSource(const std::vector<Jet>& jets, int sys, TString source);
  std::vector<Jet> SmearJets(const std::vector<Jet>& jets, int sys);

  std::vector<FatJet> ScaleFatJets(const std::vector<FatJet>& jets, int sys);
  std::vector<FatJet> SmearFatJets(const std::vector<FatJet>& jets, int sys);
  std::vector<FatJet> ScaleSDMassFatJets(const std::vector<FatJet>& jets, int sys);
  std::vector<FatJet> SmearSDMassFatJets(const std::vector<FatJet>& jets, int sys);

  //====================
  //==== Event Filters
  //====================

  bool PassMETFilter();

  //============
  //==== Tools
  //============

  //===== Estimators

  MCCorrection *mcCorr=NULL;
  PuppiSoftdropMassCorr *puppiCorr=NULL;
  FakeBackgroundEstimator *fakeEst=NULL;
  CFBackgroundEstimator *cfEst=NULL;
  void initializeAnalyzerTools();

  //==== MCweight
  double MCweight(bool usesign=true, bool norm_1invpb=true) const;

  //==== Kfactors
  double GetKFactor();


  //==== Prefire
  double GetPrefireWeight(int sys);

  //==== PU Reweight
  double GetPileUpWeight(int N_pileup, int syst);

  //==== Muon GeneralizedEngpoint momentum scaling
  GeneralizedEndpoint *muonGE=NULL;
  GEScaleSyst *muonGEScaleSyst=NULL;

  //==== Using new PDF set
  PDFReweight *pdfReweight=NULL;
  double GetPDFWeight(LHAPDF::PDF* pdf_);
  //==== NewCentral/ProdCentral
  double GetPDFReweight();
  //==== NewErrorSet/ProdCentral
  double GetPDFReweight(int member);

  //================
  //==== Functions
  //================
 
  bool IsOnZ(double m, double width);
  double MT(TLorentzVector a, TLorentzVector b);
  double MT2(TLorentzVector a, TLorentzVector b, Particle METv, double METgap);
  double projectedMET(TLorentzVector a, TLorentzVector b, Particle METv);
  bool HasFlag(TString flag);

  bool AnalyserRunsFullBkg();
  

  std::vector<Muon> MuonWithoutGap(const std::vector<Muon>& muons);
  std::vector<Muon> MuonPromptOnly(const std::vector<Muon>& muons, const std::vector<Gen>& gens,AnalyzerParameter param);
  std::vector<Muon> MuonPromptOnly(const std::vector<Muon>& muons, const std::vector<Gen>& gens);
  TString PromptStatus(Muon mu, const std::vector<Gen>& gens);
  
  std::vector<Muon> MuonUsePtCone(const std::vector<Muon>& muons);
  Muon MuonUsePtCone(const Muon& muon);
  Particle UpdateMET(const Particle& METv, const std::vector<Muon>& muons);
  Particle UpdateMETSmearedJet(const Particle& METv, const std::vector<Jet>& jets);
  Particle UpdateMETSyst(AnalyzerParameter param, const Particle& METv, std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Muon> muons, std::vector<Electron> electrons);
  Particle UpdateMETSyst(double met_pt, double met_phi, double met_shift_pt, double met_shift_phi, const Particle& METv);

  std::vector<Muon> MuonApplyPtCut(const std::vector<Muon>& muons, double ptcut);
  std::vector<Electron> ElectronPromptOnly(const std::vector<Electron>& electrons, const std::vector<Gen>& gens,AnalyzerParameter param);
  std::vector<Electron> ElectronPromptOnly(const std::vector<Electron>& electrons, const std::vector<Gen>& gens);
  std::vector<Electron> ElectronUsePtCone(const std::vector<Electron>& electrons);
  Electron ElectronUsePtCone(const Electron& electron);
  std::vector<Electron> ElectronApplyPtCut(const std::vector<Electron>& electrons, double ptcut);
  std::vector<Jet> JetsAwayFromFatJet(const std::vector<Jet>& jets, const std::vector<FatJet>& fatjets, double mindr=1.0);
  std::vector<Jet> JetsVetoLeptonInside(const std::vector<Jet>& jets, const std::vector<Electron>& els, const std::vector<Muon>& mus, double dR=0.4);
  std::vector<FatJet> FatJetsVetoLeptonInside(const std::vector<FatJet>& jets, const std::vector<Electron>& els, const std::vector<Muon>& mus, double dR=0.8);
  std::vector<Jet> JetsAwayFromPhoton(const std::vector<Jet>& jets, const std::vector<Photon>& photons, double mindr);
  Particle AddFatJetAndLepton(const FatJet& fatjet, const Lepton& lep);



  //--------- JA                                                                                                                                 

  void PrintEvent(AnalyzerParameter param,TString selection,double w);
  void FillEventComparisonFile(AnalyzerParameter param, TString label,string time, double w);

  //==== GenMatching

  vector<TString> GetGenList();
  vector<TString> GetGenListPlot();

  void PrintGen(const std::vector<Gen>& gens);
  void PrintMatchedGen(std::vector<Gen>& gens,const Lepton& Lep);

  bool GenTypeMatched(TString gen_string);
  TString MatchGenPID(int PID, vector<Gen> gens, Gen gen);

  TString MatchGenDef(std::vector<Gen>& gens,const Lepton& Lep);
  static Gen GetGenMatchedLepton(const Lepton& lep, const std::vector<Gen>& gens);
  static Gen GetGenMatchedPhoton(const Lepton& lep, const std::vector<Gen>& gens);
  static vector<int> TrackGenSelfHistory(const Gen& me, const std::vector<Gen>& gens);
  bool IsFromHadron(const Gen& me, const std::vector<Gen>& gens);
  int GetLeptonType(const Lepton& lep, const std::vector<Gen>& gens);
  int GetLeptonType_Public(int TruthIdx, const std::vector<Gen>& TruthColl);
  int GetGenPhotonType(const Gen& genph, const std::vector<Gen>& gens);
  static bool IsFinalPhotonSt23_Public(const std::vector<Gen>& TruthColl);
  int  GetPrElType_InSameSCRange_Public(int TruthIdx, const std::vector<Gen>& TruthColl);

  int  GenMatchedIdx(const Lepton& Lep, std::vector<Gen>& truthColl);
  int  GetNearPhotonIdx(const Lepton& Lep, std::vector<Gen>& TruthColl);
  int  FirstNonSelfMotherIdx(int TruthIdx, std::vector<Gen>& TruthColl);
  int  LastSelfMotherIdx(int TruthIdx,std::vector<Gen>& TruthColl);
  bool HasHadronicAncestor(int TruthIdx, std::vector<Gen>& TruthColl);
  bool IsFinalPhotonSt23(std::vector<Gen>& TruthColl);
  int  GetPrElType_InSameSCRange(int TruthIdx, std::vector<Gen>& TruthColl, TString Option="");

  bool IsSignalPID(int pid);

  

  // HEM code
  bool FindHEMElectron(Electron electron);

  //============ JEC Uncertainty
  double GetJECUncertainty(TString source, TString JetType,  double eta, double pt, int sys);
  void  SetupJECUncertainty(TString source , TString JetType="AK4PFchs");

  // ==== TIMING MAP

  std::map< TString, double > TimingMap;
  std::map< TString, double > TimerMap;
  std::map< TString, TString> TimeTagMatcher;
  void AddTimerStamp(TString tag);
  void FillTimer(TString tag);
  //==== Plotting

  std::map< TString, TProfile* >   maphist_TProfile;
  std::map< TString, TProfile2D* > maphist_TProfile2D;
  std::map< TString, TProfile3D* > maphist_TProfile3D;
  std::map< TString, TH1D* > maphist_TH1D;
  std::map< TString, TH2D* > maphist_TH2D;
  std::map< TString, TH3D* > maphist_TH3D;

  // Maps for JEC
  std::map<TString, std::vector<std::map<double, std::vector<double> > > > AK4CHSJECUncMap;
  std::map<TString, std::vector<std::map<double, std::vector<double> > > > AK4PUPPIJECUncMap;
  std::map<TString, std::vector<std::map<double, std::vector<double> > > > AK8CHSJECUncMap;
  std::map<TString, std::vector<std::map<double, std::vector<double> > > > AK8PUPPIJECUncMap;

  vector<TString> JECSources;


  TProfile*   GetHistPf(TString histname);
  TProfile2D* GetHistPf2D(TString histname);
  TProfile3D* GetHistPf3D(TString histname);
  TH1D* GetHist1D(TString histname);
  TH2D* GetHist2D(TString histname);
  TH3D* GetHist3D(TString histname);

  // === Ev weights                                                                                                                              
  void FillWeightHist(TString label, double _weight);

  // === HIST settings/filling                        

  void FillProf(TString histname, double xvalue, double yvalue, double weight, int n_bin, double x_min, double x_max, bool IsAverage=true);
  void FillProf(TString histname, double xvalue, double yvalue, double zvalue, double weight,
									                int n_binx, double x_min, double x_max,
									                int n_biny, double y_min, double y_max);
  void FillProf(TString histname, double xvalue, double yvalue, double zvalue, double wvalue, double weight,
									                int n_binx, double x_min, double x_max,
									                int n_biny, double y_min, double y_max,
									                int n_binz, double z_min, double z_max);
  void FillHist(TString histname, double value, double weight, int n_bin, double x_min, double x_max, TString label="");
  void FillHist(TString histname, double value, double weight, int n_bin, double *xbins, TString label="");
  void FillHist(TString histname, vector<TString> labels, TString label, double weight);

  void FillHist(TString histname,
                double value_x, double value_y,
                double weight,
                int n_binx, double x_min, double x_max,
                int n_biny, double y_min, double y_max);
  void FillHist(TString histname,
                double value_x, double value_y,
                double weight,
                int n_binx, double *xbins,
                int n_biny, double *ybins);
  void FillHist(TString histname,
		double value_x, double value_y, double value_z,
		double weight,
		int n_binx, double x_min, double x_max,
		int n_biny, double y_min, double y_max,
		int n_binz, double z_min, double z_max);
  void FillHist(TString histname,
		double value_x, double value_y, double value_z,
		double weight,
		int n_binx, const double *xbins,
		int n_biny, const double *ybins,
		int n_binz, const double *zbins);

  //==== JSFillHist : 1D
  std::map< TString, std::map<TString, TH1D*> > JSmaphist_TH1D;
  TH1D* JSGetHist1D(TString suffix, TString histname);
  void JSFillHist(TString suffix, TString histname, double value, double weight, int n_bin, double x_min, double x_max);
  //==== JSFillHist : 2D
  std::map< TString, std::map<TString, TH2D*> > JSmaphist_TH2D;
  TH2D* JSGetHist2D(TString suffix, TString histname);
  void JSFillHist(TString suffix, TString histname,
                  double value_x, double value_y,
                  double weight,
                  int n_binx, double x_min, double x_max,
                  int n_biny, double y_min, double y_max);
  void JSFillHist(TString suffix, TString histname,
                  double value_x, double value_y,
                  double weight,
                  int n_binx, double *xbins,
                  int n_biny, double *ybins);

  virtual void WriteHist();

  //==== Quick Plotters
  void FillLeptonPlots(std::vector<Lepton *> leps, TString this_region, double weight);
  void FillJetPlots(std::vector<Jet> jets, std::vector<FatJet> fatjets, TString this_region, double weight);

  //==== Output rootfile
  void SwitchToTempDir();
  TFile *outfile=NULL;
  void SetOutfilePath(TString outname);


  // JA Added functions 

  vector<Muon> SkimLepColl(const vector<Muon>& MuColl, AnalyzerParameter param, TString Option);
  vector<Electron> SkimLepColl(const vector<Electron>& ElColl, AnalyzerParameter param,TString Option);
  vector<Electron> SkimLepColl(const vector<Electron>& ElColl,TString Option);
  vector<Muon> SkimLepColl(const vector<Muon>& MuColl,  TString Option);
  vector<Jet> SkimJetColl(const vector<Jet>& JetColl, vector<Gen>& TruthColl, AnalyzerParameter param,TString Option);
  bool HasEWLepInJet(Jet Jet, vector<Gen>& TruthColl, TString Option);


  // JH functions 

  // HEM code                                                                                                                                                

  bool IsHEMIssueRun();
  bool IsHEMIssueReg(Particle& Particle);
  bool IsHEMCRReg(Particle& Particle, TString Option);

  int  GetPartonType_JH(int TruthIdx, std::vector<Gen>& TruthColl);
  int  GetLeptonType_JH(int TruthIdx, std::vector<Gen>& TruthColl);
  int  GetLeptonType_JH(const Lepton& Lep, std::vector<Gen>& TruthColl);
  int  GetPhotonType_JH(int PhotonIdx, std::vector<Gen>& TruthColl);
  int  GetFakeLepSrcType(const Lepton& Lep, vector<Jet>& JetColl);

  bool iSetupLeptonBDT;


  /// Variables for filling MVA branches 

  float vSKWeight;
  vector<float>* velectron_ptratio;
  vector<float>* velectron_ptrel;
  vector<float>* velectron_cj_bjetdisc;
  vector<float>* velectron_cj_flavour;

  vector<float>* velectron_mva_fake_v1;
  vector<float>* velectron_mva_fake_v2;
  vector<float>* velectron_mva_fakeHF_v2;
  vector<float>* velectron_mva_fakeLF_v2;
  vector<float>* velectron_mva_fakeTop_v2;
  vector<float>* velectron_mva_fake_v3;
  vector<float>* velectron_mva_fakeHF_v3;
  vector<float>* velectron_mva_fakeHFB_v3;
  vector<float>* velectron_mva_fakeHFC_v3;
  vector<float>* velectron_mva_fakeLF_v3;
  vector<float>* velectron_mva_fakeTop_v3;
  vector<float>* velectron_mva_fake_v4;
  vector<float>* velectron_mva_fakeHF_v4;
  vector<float>* velectron_mva_fakeHFB_v4;
  vector<float>* velectron_mva_fakeHFC_v4;
  vector<float>* velectron_mva_fakeLF_v4;
  vector<float>* velectron_mva_fakeTop_v4;
  vector<float>* velectron_mva_fake_ed_v4;
  vector<float>* velectron_mva_fakeHF_ed_v4;
  vector<float>* velectron_mva_fakeHFB_ed_v4;
  vector<float>* velectron_mva_fakeHFC_ed_v4;
  vector<float>* velectron_mva_fakeLF_ed_v4;
  vector<float>* velectron_mva_fakeTop_ed_v4;

 
  vector<float>* velectron_mva_conv_v1;
  vector<float>* velectron_mva_conv_v2;
  vector<float>* velectron_mva_conv_ed_v2;

  vector<float>* velectron_mva_cf_v1;
  vector<float>* velectron_mva_cf_v2;
  vector<float>* velectron_mva_cf_v2p1;
  vector<float>* velectron_mva_cf_v2p2;
  vector<float>* velectron_mva_cf_ed_v2;
  vector<float>* velectron_mva_cf_ed_v2p1;
  vector<float>* velectron_mva_cf_ed_v2p2;



  vector<float>* vmuon_mva_fake_v1;
  vector<float>* vmuon_mva_fake_v2;
  vector<float>* vmuon_mva_fake_v3;
  vector<float>* vmuon_mva_fake_v4;
  vector<float>* vmuon_mva_fake_ed_v4;
  vector<float>* vmuon_ptrel;
  vector<float>* vmuon_ptratio;
  vector<float>* vmuon_cj_bjetdisc;
  vector<float>* vmuon_cj_flavour;

  vector<int>*   vmuon_lepton_type;
  vector<int>*   velectron_lepton_type;
  vector<bool>*  velectron_is_cf;
  vector<bool>*  vmuon_is_cf;

  vector<Jet>      All_Jets;
  vector<FatJet>   All_FatJets;
  vector<Muon>    All_Muons;
  vector<Electron> All_Electrons;
  vector<Gen> All_Gens;

  string run_timestamp;


  bool TESTBDT;

};



#endif

