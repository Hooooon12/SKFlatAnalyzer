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
#define M_CUT_LL 10.
#define M_CUT_NonSR3_LL 20.
#define M_ZWINDOW 20.
#define M_ZWINDOW_CR 15.
#define M_ZWINDOW_VETO 10.

class AnalyzerCore: public SKFlatNtuple {

public:

  AnalyzerCore();
  ~AnalyzerCore();

  virtual void initializeAnalyzer();

  virtual void executeEvent(){

  };

  enum BkgType
  {
    Fake=0,
    Conv=1,
    CF=2,
    FakeRate=3,
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

  vector<Jet>      All_Jets;
  vector<FatJet>   All_FatJets;
  vector<Muon>     All_Muons;
  vector<Electron> All_Electrons;
  Event _Event;

  vector<Gen>      All_Gens;
  vector<LHE>      All_LHES;
  LHE lhe_p0,lhe_p1,lhe_l0,lhe_l1,lhe_j0;
  Gen gen_p0,gen_p1,gen_l0,gen_l1,gen_l0_dressed,gen_l1_dressed,gen_l0_bare,gen_l1_bare;


  Event GetEvent();

  std::vector<Electron> GetAllElectrons();
  std::vector<Muon> GetAllMuons();

  //==== If TightIso is set, it calculate ptcone 
  //==== If UseMini is true, Lepton::RelIso() returns MiniRelIso  
  std::vector<Lepton *> MakeLeptonPointerVector(const std::vector<Muon>& muons,const std::vector<Electron>& electrons, double TightIso=-999, bool UseMini=false);
  std::vector<Lepton *> MakeLeptonPointerVector(const std::vector<Muon>& muons,const std::vector<Electron>& electrons, AnalyzerParameter param, double TightIso=-999, bool UseMini=false);
  std::vector<Lepton *> MakeLeptonPointerVector(const std::vector<Muon>& muons, double TightIso=-999, bool UseMini=false);
  std::vector<Lepton *> MakeLeptonPointerVector(const std::vector<Muon>& muons,AnalyzerParameter param, double TightIso=-999, bool UseMini=false);
  std::vector<Lepton *> MakeLeptonPointerVector(const std::vector<Electron>& electrons, double TightIso=-999, bool UseMini=false);
  std::vector<Lepton *> MakeLeptonPointerVector(const std::vector<Electron>& electrons, AnalyzerParameter param,double TightIso=-999, bool UseMini=false);

  std::vector<Tau>    GetAllTaus();
  std::vector<Photon> GetAllPhotons();
  std::vector<Jet>    GetAllJets(bool applyCorr=true);
  std::vector<FatJet> GetAllFatJets();

  std::vector<Gen>    GetGens();
  std::vector<LHE>    GetLHEs();

  std::vector<Muon>   UseTunePMuon(const std::vector<Muon>& muons);

  std::vector<Jet>       GetJets(TString ID, double ptmin, double fetamax);
  std::vector<FatJet>    GetFatJets(TString ID, double ptmin, double fetamax);
  std::vector<Muon>      GetMuons(TString ID, double ptmin, double fetamax);
  std::vector<Electron>  GetElectrons(TString ID, double ptmin, double fetamax, bool vetoHEM=true);
  std::vector<Tau>       GetTaus(TString ID, double ptmin, double fetamax);


  bool SelectEvent(int evnumber);
  
  //// =================  BDT Functions AnalyzerCore_IDBDT.C ================= 

  bool iSetupLeptonBDTv4;
  bool iSetupLeptonBDTv5;

  TMVA::Reader *MuonIDv5_FakeMVAReader;
  TMVA::Reader *MuonIDv4_FakeMVAReader;

  TMVA::Reader *ElectronIDv4_FakeMVAReader;
  TMVA::Reader *ElectronIDv4_ConvMVAReader;
  TMVA::Reader *ElectronIDv4_CFMVAReader;

  ///////////// Version5 
  TMVA::Reader *ElectronIDv5_MVAReader;
  TMVA::Reader *ElectronIDv5_CFMVAReader;
  TMVA::Reader *ElectronIDv5_FakeMVAReader;

  void InitializeIDTreeVars();
  void InitializeElectronIDTreeVars();
  void PrintBDTInput();
  void PrintBDTVariables(Electron el,TString label);

  /// For adding var to Trees
  void SetupLeptonBDTSKFlat();
  void ResetLeptonBDTSKFlat();
  void InitialiseLeptonBDTSKFlat();

  void SetupLeptonBDTSKFlatV5();
  void ResetLeptonBDTSKFlatV5();
  //  void InitialiseLeptonBDTSKFlatV5();


  /// Use in analysis 
  void SetupLeptonBDT( bool version4, bool version5);
  void SetupLeptonBDTv4();
  void SetupLeptonBDTv5();
  void SetBDTIDVarV1(Lepton*  lep);
  void SetBDTIDVarOLD(Lepton*  lep);
  void SetBDTIDVar(Lepton*  lep);
  void SetBDTIDVariablesElectronOLD(Electron el);
  void SetBDTIDVariablesElectron(Electron el);
  void SetBDTIDVariablesMuon(Muon mu);
  void SetupIDMVAReaderMuon();
  void SetupIDMVAReaderElectron();
  void SetupIDMVAReaderElectronUpdate();
  /////// MASTER FUNCTION
  void SetupIDMVAReaderDefault(bool v4=false, bool v5=false);
  void CompareBDT(TString var1, double a, double b,  int ilep );

  /// Var for ID BDTs
  Float_t bdt_id_Pt,        bdt_id_Eta,      bdt_id_PtBinned,bdt_id_PtBinned2,bdt_id_Fake_Type,bdt_id_Fake_Flavour; //3
  Float_t bdt_id_PtRatio,   bdt_id_PtRel   , bdt_id_MassDrop; //3
  Float_t bdt_id_PtRatioV2,   bdt_id_PtRelV2;
  Float_t bdt_id_PtRatioV3,   bdt_id_PtRelV3;
  Float_t bdt_id_CEMFracCJ, bdt_id_NEMFracCJ,bdt_id_CHFracCJ, bdt_id_NHFracCJ, bdt_id_MuFracCJ, bdt_id_JetDiscCJ,bdt_id_JetDiscCJCvsB,bdt_id_JetDiscCJCvsL; //6
  Float_t bdt_id_Dxy,       bdt_id_Dz,       bdt_id_DxySig, bdt_id_DzSig, bdt_id_RelIso,bdt_id_IP3D,bdt_id_MVA,bdt_id_MVAIso,bdt_id_MVARaw,bdt_id_MVAIsoRaw,bdt_id_Chi2, bdt_id_Minireliso; //10
  Float_t bdt_id_Full5x5_sigmaIetaIeta,bdt_id_dEtaSeed,bdt_id_dPhiIn,bdt_id_HoverE,bdt_id_Rho,bdt_id_TrkIso,bdt_id_InvEminusInvP,bdt_id_ecalPFClusterIso,bdt_id_hcalPFClusterIso; //9
  Float_t bdt_id_RelDxy,bdt_id_RelDz,bdt_id_RelIP3D,bdt_id_RelMVA,bdt_id_RelMVAIso,bdt_id_PileUp;//6
  Float_t bdt_id_R9,bdt_id_dr03TkSumPt,bdt_id_dr03HcalTowerSumEt,bdt_id_dr03HcalDepth1TowerSumEt,bdt_id_dr03EcalRecHitSumEt, bdt_id_e2x5OverE5x5,bdt_id_e1x5OverE5x5,bdt_id_psEoRraw;//7
  Float_t bdt_id_e15,bdt_id_e25,bdt_id_e55,bdt_id_EtaWidth,bdt_id_PhiWidth,bdt_id_dEtaIn, bdt_id_MiniIsoChHad,bdt_id_MiniIsoNHad,bdt_id_MiniIsoPhHad,bdt_id_IsoChHad,bdt_id_IsoNHad,bdt_id_IsoPhHad;//12
  Float_t bdt_id_RelMiniIsoCh,bdt_id_RelMiniIsoN,bdt_id_EoverP,bdt_id_FBrem;//4
  //Float_t fIsEcalDriven,Pixel_hits,  fValidhits,fMatched_stations,fTracker_layers,fMissingHits;//6
  //Float_t fPassConversionVeto,fIsGsfCtfScPixChargeConsistent, fIsGsfScPixChargeConsistent, fIsGsfCtfChargeConsistent;//4
  //Float_t fPOGTight, fPOGMedium,fHNTightID;
  Float_t bdt_id_w_id_tot;

  
  Float_t bdt_id_POGTight, bdt_id_POGMedium,bdt_id_HNTightID,bdt_id_POGMVA80ID,bdt_id_POGMVA90ID;
  Float_t bdt_id_isEcalDriven,bdt_id_Pixel_hits,  bdt_id_Validhits,bdt_id_Matched_stations,bdt_id_Tracker_layers,bdt_id_MissingHits;//6                                                                                                    
  Float_t bdt_id_PassConversionVeto,bdt_id_IsGsfCtfScPixChargeConsistent, bdt_id_IsGsfScPixChargeConsistent, bdt_id_IsGsfCtfChargeConsistent;//4                                                                                                                                                          
  
  /// ID functions MVA
  double GetBDTScoreEl(Electron el ,BkgType bkg,TString bdttag);
  double GetBDTScoreElV1(Electron el ,BkgType bkg,TString bdttag);
  double GetBDTScoreMuon(Muon mu ,BkgType bkg, TString bdttag="BDTA");
  double GetBDTScoreEl_EtaDependant(Electron el ,BkgType bkg, TString BDTTag);
  double GetBDTScoreMuon_EtaDependant(Muon el ,BkgType bkg, TString BDTTag);

  void FillBDTHists(Electron el,TString cut, double w, bool SplitFakes=false);
  void FillBDTHists(Muon mu,TString cut, double w,bool SplitFakes=false);



  // =================   FR functions   AnalyzerCore_Jet.C  ======================  
  
  Jet GetCorrectedJetCloseToLepton(Lepton lep, Jet jet);
  Jet GetCorrectedJetCloseToLepton(Muon lep, Jet jet);
  Jet GetCorrectedJetCloseToLepton(Electron lep, Jet jet);
  Jet GetCorrectedJetCloseToLepton(vector<Particle> leps, Jet jet);
  double  JetLeptonMassDropLepAware(  Muon lep, bool removeLep,bool ApplyCorr=false);
  double  JetLeptonMassDropLepAware(  Electron lep, bool removeLep,bool ApplyCorr=false);
  double  JetLeptonPtRelLepAware(  Lepton lep, Jet jet);
  double  JetLeptonPtRelLepAware(  Lepton lep);
  double  JetLeptonPtRelLepAwareV2(  Lepton lep);
  double  JetLeptonPtRelLepAware(  Electron lep);
  double  JetLeptonPtRelLepAware(  Muon lep,     bool CorrLep=false);
  double  JetLeptonPtRatioLepAwareMuon(Lepton lep, bool smearjet, bool corrMu, bool uncorrLepE);
  double  JetLeptonPtRelLepAwareMuon( Lepton lep, bool smearjet, bool corrMu, bool uncorrLepE);
  double  JetLeptonPtRatioLepAware( Lepton lep, Jet jet);
  double  JetLeptonPtRatioLepAware( Lepton lep);
  double  JetLeptonPtRatioLepAwareV2( Lepton lep);
  double  JetLeptonPtRatioLepAware( Muon lep, bool CorrLep=false);
  double  JetLeptonPtRatioLepAware( Electron lep);

  // =================   FR functions   AnalyzerCore_GEN.C  ======================                                                                                                                                                                                                         
  bool ConversionSplitting(std::vector<Lepton *> leps, bool RunConvMode, int nlep,AnalyzerParameter param);
  bool ConversionVeto(std::vector<Lepton *> leps,const std::vector<Gen>& gens);
  bool IsCF(Electron el, std::vector<Gen> gens, bool checcloseel=true);
  bool IsCF(Muon mu, std::vector<Gen> gens);
  bool HasPromptConv(Electron el);

  bool HasMEPhoton(Lepton lep);
  bool GenIsPrompt(Gen gen);
  Gen GenGetMother(Gen gen);

  void GetAFBGenParticles(const vector<Gen>& gens,Gen& parton0,Gen& parton1,Gen& l0,Gen& l1,int mode);
  void GetAFBLHEParticles(const vector<LHE>& lhes,LHE& p0,LHE& p1,LHE& l0,LHE& l1,LHE& j0);

  vector<TString> GetGenList();
  vector<TString> GetGenListPlot();

  void PrintGen(const std::vector<Gen>& gens);
  void PrintMatchedGen(std::vector<Gen>& gens,const Lepton& Lep);
  int  HotFixLeptonType(Lepton lep);

  bool GenTypeMatched(TString gen_string);
  bool IsFromHadron(const Gen& me, const std::vector<Gen>& gens);
  bool HasHadronicAncestor(int TruthIdx, std::vector<Gen>& TruthColl);
  bool IsFinalPhotonSt23(std::vector<Gen>& TruthColl);
  bool IsSignalPID(int pid);


  inline int GetEraNum() const {
    if(DataEra=="2016preVFP") return 1;
    if(DataEra=="2016postVFP") return 2;
    if(DataEra=="2017") return 3;
    if(DataEra=="2018") return 4;
    return -1;
  }
  
  
  // ------ General HNL functions (HNL_LeptonCore.C)                                                                                              
  inline bool IsSignal() const {
    if(MCSample.Contains("TypeI")) return true;
    return false;
  }

  TString MatchGenPID(int PID, vector<Gen> gens, Gen gen);
  TString MatchGenDef(std::vector<Gen>& gens,const Lepton& Lep,bool DEBUG=false);

  static Gen GetGenMatchedLepton(const Lepton& lep, const std::vector<Gen>& gens);
  static Gen GetGenMatchedPhoton(const Lepton& lep, const std::vector<Gen>& gens);
  static vector<int> TrackGenSelfHistory(const Gen& me, const std::vector<Gen>& gens);
  static bool IsFinalPhotonSt23_Public(const std::vector<Gen>& TruthColl);

  int GetLeptonType(const Lepton& lep, const std::vector<Gen>& gens);
  int GetLeptonType_Public(int TruthIdx, const std::vector<Gen>& TruthColl);
  int GetGenPhotonType(const Gen& genph, const std::vector<Gen>& gens);
  int GetPrElType_InSameSCRange_Public(int TruthIdx, const std::vector<Gen>& TruthColl);
  int GenMatchedIdx(const Lepton& Lep, std::vector<Gen>& truthColl);
  int GetNearPhotonIdx(const Lepton& Lep, std::vector<Gen>& TruthColl);
  int FirstNonSelfMotherIdx(int TruthIdx, std::vector<Gen>& TruthColl);
  int LastAbsSelfMotherIdx(int TruthIdx,std::vector<Gen>& TruthColl);
  int LastSelfMotherIdx(int TruthIdx,std::vector<Gen>& TruthColl);
  int GetPrElType_InSameSCRange(int TruthIdx, std::vector<Gen>& TruthColl, TString Option="");

  int  GetPartonType_JH(int TruthIdx, std::vector<Gen>& TruthColl);
  int  GetLeptonType_JH(int TruthIdx, std::vector<Gen>& TruthColl);
  int  GetLeptonType_JH(const Lepton& Lep, std::vector<Gen>& TruthColl);
  int  GetPhotonType_JH(int PhotonIdx, std::vector<Gen>& TruthColl);


  // ================= MET Functions AnalyzerCore_MET.C  ===================

  Particle UpdateMET(const Particle& METv, const std::vector<Muon>& muons);
  Particle UpdateMETSmearedJet(const Particle& METv, const std::vector<Jet>& jets);
  Particle UpdateMETSyst(AnalyzerParameter param, const Particle& METv, std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Muon> muons, std::vector<Electron> electrons);
  Particle UpdateMETSyst(double met_pt, double met_phi, double met_shift_pt, double met_shift_phi, const Particle& METv);
  Particle UpdateMETCF(const Particle METv, const std::vector<Electron> electrons, const std::vector<Electron> electrons_shifted);

  // =================== HISTOGRAM  AnalyzerCore_HIST.C  ===================  
  std::map< TString, TH1D* > maphist_TH1D;
  std::map< TString, TH2D* > maphist_TH2D;
  std::map< TString, TH3D* > maphist_TH3D;

  std::map< TString, TProfile* >   maphist_TProfile;
  std::map< TString, TProfile2D* > maphist_TProfile2D;
  std::map< TString, TProfile3D* > maphist_TProfile3D;


  TProfile*   GetHistPf(TString histname);
  TProfile2D* GetHistPf2D(TString histname);
  TProfile3D* GetHistPf3D(TString histname);

  TH1D* GetHist1D(TString histname);
  TH2D* GetHist2D(TString histname);
  TH3D* GetHist3D(TString histname);
  void DeleteHistMaps();
  void DeleteProfileMaps();

  // === Ev weights                                                                                                                                                                                                                                                                        
  double FillWeightHist(TString label, double _weight);
  double FillFakeWeightHist(TString label, vector<Lepton *> Leps,AnalyzerParameter param,  double _weight);

  // === HIST settings/filling                                                                                                             
  vector<double> GetHistBins(TString k);
  int GetHistNBins(TString k);
  void SetHistBins();
  map<TString, int> map_hist_nbins;
  map<TString, vector<double> > map_hist_bins;                                                                                                                                                
  void AddHistBinning(TString mkey, vector<double> mbins);


  void FillProf(TString histname, double xvalue, double yvalue, double weight, int n_bin, double x_min, double x_max, bool IsAverage=true);
  void FillProf(TString histname, double xvalue, double yvalue, double zvalue, double weight,
		int n_binx, double x_min, double x_max,
		int n_biny, double y_min, double y_max);
  void FillProf(TString histname, double xvalue, double yvalue, double zvalue, double wvalue, double weight,
		int n_binx, double x_min, double x_max,
		int n_biny, double y_min, double y_max,
		int n_binz, double z_min, double z_max);


  void FillHistogram(TString histname, double value, double weight, TString BinLabel,  TString label="");
  void FillHist(TString histname, double value, double weight, int n_bin, double x_min, double x_max, TString label="");
  void FillHist(TString histname, double value, double weight, int n_bin, double *xbins, TString label="");
  void FillHistogram(TString histname, double value, double weight, int n_bin, vector<double> xbins, TString label="");
  void FillHistogram(TString histname, double value_x, double value_y, double weight, int n_binx, vector<double> xbins, int n_biny, vector<double> ybins,  TString label="");

  void FillHistogram(TString histname, double value_x,double value_y, double weight, TString BinLabelx,TString BinLabely ,  TString label="");
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
                double value_x, double value_y,
                double weight,
                int n_binx, double *xbins,
                int n_biny, double y_min, double y_max);

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
  void WriteProfile();


  void PrintEvent(AnalyzerParameter param,TString selection,double w);
  void FillEventComparisonFile(AnalyzerParameter param, TString label,string time, double w);

  //==================
  //==== Systematics
  //==================

  // AnalyzerCore_Lepton.C
  std::vector<Electron> ScaleElectrons(const std::vector<Electron>& electrons, int sys);
  std::vector<Electron> SmearElectrons(const std::vector<Electron>& electrons, int sys);
  std::vector<Muon> ScaleMuons(const std::vector<Muon>& muons, int sys);

  // AnalyzerCore_Jet.C
  std::vector<Jet> ScaleJets(const std::vector<Jet>& jets, int sys);
  std::vector<Jet> ScaleJetsIndividualSource(const std::vector<Jet>& jets, int sys, TString source);
  std::vector<Jet> SmearJets(const std::vector<Jet>& jets, int sys);

  std::vector<FatJet> ScaleFatJets(const std::vector<FatJet>& jets, int sys);
  std::vector<FatJet> SmearFatJets(const std::vector<FatJet>& jets, int sys);
  std::vector<FatJet> ScaleSDMassFatJets(const std::vector<FatJet>& jets, int sys);
  std::vector<FatJet> SmearSDMassFatJets(const std::vector<FatJet>& jets, int sys);

  //============ JEC Uncertainty                                                                                                                                                                                                                                                           
  double GetJECUncertainty(TString source, TString JetType,  double eta, double pt, int sys);
  void  SetupJECUncertainty(TString source , TString JetType="AK4PFchs");
  // Maps for JEC                                                                                                                                                                                                                                                                          
  std::map<TString, std::vector<std::map<double, std::vector<double> > > > AK4CHSJECUncMap;
  std::map<TString, std::vector<std::map<double, std::vector<double> > > > AK4PUPPIJECUncMap;
  std::map<TString, std::vector<std::map<double, std::vector<double> > > > AK8CHSJECUncMap;
  std::map<TString, std::vector<std::map<double, std::vector<double> > > > AK8PUPPIJECUncMap;

  vector<TString> JECSources;

  //================================================
  //==== Event Filters    // AnalyzerCore_EvCorr.C
  //=================================================

  bool PassMETFilter();
  bool FindHEMElectron(Electron electron);
  double GetPrefireWeight(int sys);

  //=====================
  //==== Tools
  //====================

  //===== Estimators

  void initializeAnalyzerTools();

  //==== MCweight
  double MCweight(bool usesign=true, bool norm_1invpb=true) const;

  //==== Kfactors
  double GetKFactor();

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

  std::vector<Muon> MuonWithoutGap(const std::vector<Muon>& muons);
  std::vector<Muon> MuonPromptOnly(const std::vector<Muon>& muons, const std::vector<Gen>& gens,AnalyzerParameter param);
  std::vector<Muon> MuonPromptOnly(const std::vector<Muon>& muons, const std::vector<Gen>& gens);
  TString PromptStatus(Muon mu, const std::vector<Gen>& gens);
  
  std::vector<Muon> MuonUsePtCone(const std::vector<Muon>& muons);
  Muon MuonUsePtCone(const Muon& muon);
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

  // ==== TIMING MAP
  std::map< TString, double > TimingMap;
  std::map< TString, double > TimerMap;
  std::map< TString, TString> TimeTagMatcher;
  void AddTimerStamp(TString tag);
  void FillTimer(TString tag);

  //==== Output rootfile
  void SwitchToTempDir();
  TFile *outfile=NULL;
  void SetOutfilePath(TString outname);

  bool HasEWLepInJet(Jet Jet, vector<Gen>& TruthColl, TString Option);

  // HEM code                                                                                                                                                
  bool IsHEMIssueRun();
  bool IsHEMIssueReg(Particle& Particle);
  bool IsHEMCRReg(Particle& Particle, TString Option);

  /// Variables for filling MVA branches 
  float vSKWeight;
  //// v2 values for making V5 BDTSkims
  vector<float>* velectron_ptratio;
  vector<float>* velectron_ptrel;
  vector<float>* velectron_v2_cj_bjetdisc;
  vector<float>* velectron_v2_cj_cvsbjetdisc;
  vector<float>* velectron_v2_cj_cvsljetdisc;
  vector<float>* velectron_v2_cj_flavour;

  ///// MVA V5
  vector<float>* velectron_mva_cf_ed_v5;
  vector<float>* velectron_mva_cf_ed_v5pt;
  vector<float>* velectron_mva_conv_ed_v5;
  vector<float>* velectron_mva_fake_ed_v5;
  vector<float>* velectron_mva_fakeHFB_v5;
  vector<float>* velectron_mva_fakeHFC_v5;
  vector<float>* velectron_mva_fakeLF_v5;
  vector<float>* velectron_mva_fake_QCD_LFvsHF_v5;
  vector<float>* velectron_mva_fake_QCD_HFBvsHFC_v5;
  vector<float>* velectron_mva_fake_QCD_LF1_v5;
  vector<float>* velectron_mva_fake_QCD_LF2_v5;


  ///// MVA V5                                                                                                                                                                                                                                                                                                                
  vector<float>* vmuon_mva_fake_QCD_LFvsHF_v5;
  vector<float>* vmuon_mva_fake_QCD_HFBvsHFC_v5;
  vector<float>* vmuon_mva_fake_QCD_LF1_v5;
  vector<float>* vmuon_mva_fake_QCD_LF2_v5;

  vector<float>* vmuon_ptrel;
  vector<float>* vmuon_ptratio;
  vector<float>* vmuon_v2_cj_bjetdisc;
  vector<float>* vmuon_v2_cj_cvsbjetdisc;
  vector<float>* vmuon_v2_cj_cvsljetdisc;
  vector<float>* vmuon_v2_cj_flavour;
  vector<int>*   vmuon_lepton_type;
  vector<int>*   velectron_lepton_type;


  ///// MISCLILLANEOUS VARIABLES
  bool run_Debug,  PtOrderObj, TESTBDT;
  int nLog;
  string run_timestamp;

  bool IsDYSample, IsTTSample;

};



#endif

