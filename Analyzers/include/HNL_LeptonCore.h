#ifndef HNL_LeptonCore_h
#define HNL_LeptonCore_h

#include "TRandom.h"

#include "AnalyzerCore.h"

class HNL_LeptonCore : public AnalyzerCore {

 public:

  enum ChargeType
  {
    Inclusive=0,
    SS=1,
    OS=-1,
  };

  enum Channel
  {
    El=0,
    EE=1,
    EEE=2,
    EEMu=4,
    EEEE=5,
    Mu=10,
    MuMu=11,
    MuMuMu=13,
    MuMuE=14,
    MuMuMuMu=15,
    EMu=16,
    MuE=17,
    

  };
  enum Region
  {
    SR1=20,
    SR2=21,
    SR3=22,
    SR4=23,
    Presel=24,
    PreselSS=25,
    PreselOS=26,
    SR=27,
    sigmm=28,
    sigee=29,
    sigem=30,
    sigmm_17028=31,
    sigee_17028=32,
    sigem_17028=33,


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

  void initializeAnalyzer();

  AnalyzerParameter InitialiseHNLParameters( TString analysis_tag, vector<vector<TString> >  param_vec);
  AnalyzerParameter InitialiseHNLParameter(TString s_setup);  



  //===== TRIGGER

  bool PassTriggerSelection(HNL_LeptonCore::Channel channel,Event ev, std::vector<Lepton *> leps, TString selection);
  // ============= HELPER FUNCTIONS
  void PrintParam(AnalyzerParameter param);
  TString QToString(HNL_LeptonCore::ChargeType q);
 
  bool CheckStream(Event ev,vector<TString> triglist);
  TString  DoubleElectronPD();
  TString  SingleElectronPD();
  TString  MuonEGPD();
  TString  DoubleMuonPD();
  bool HasLowMassMeson(std::vector<Lepton *> leps);

  ///=============== GET/SELECT OBJECT
  vector<Gen> GetGenLepronsSignal();
  TString GetProcess();
  TString GetChannelString(HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType  q=HNL_LeptonCore::ChargeType::Inclusive);
  std::vector<Jet> SelBJets(std::vector<Jet>& jetColl, JetTagging::Parameters jtp);
  TLorentzVector GetvMET(TString METType, TString Option="");



  //================== KINEMATIC HELPER
  double M_T(Particle a, Particle b);
  float GetLT(std::vector<Lepton *> leps);
  double GetHT( std::vector<Jet> jets, std::vector<FatJet> fatjets);
  double GetST( std::vector<Electron> electrons, std::vector<Muon> muons, std::vector<Jet> jets, std::vector<FatJet> fatjets,  Event ev);
  double GetST( std::vector<Electron> electrons, std::vector<Muon> muons, std::vector<Jet> jets, std::vector<FatJet> fatjets, Particle met);
  double GetST( std::vector<Lepton *> leps, std::vector<Jet> jets, std::vector<FatJet> fatjets,  Event ev);
  double GetST( std::vector<Lepton *> leps, std::vector<Jet> jets, std::vector<FatJet> fatjets, Particle met);

  bool  ZmasslllWindowCheck(std::vector<Lepton *> leps);
  bool  ZmassOSWindowCheck(std::vector<Lepton *> leps);
  bool  ZmassOSSFWindowCheck(std::vector<Lepton *> leps, double mass_diff);


  float GetMassMinOSSF(std::vector<Lepton *> leps);
  float GetMassMinSSSF(std::vector<Lepton *> leps);
  int GetIndexNonMinOSSF(std::vector<Lepton *> leps);
  int GetIndexNonMinSSSF(std::vector<Lepton *> leps);
  int GetIndexNonBestZ(std::vector<Lepton *> leps,double mass_diff);

  float GetLLMass(std::vector<Lepton *> leps);
  float GetLLMass(std::vector<Muon> leps);
  float GetLLMass(std::vector<Electron> leps);

  float GetMassBestZ(std::vector<Lepton *> leps,  bool bestZ);
  double  GetMass(TString type , std::vector<Jet> jets, std::vector<FatJet> fatjets);

  //================                                                                                                                                                                                        
  //==== Functions                                                                                                                                                                                          
  //================                                                                                                                                                                                        
  TString GetPtLabel(Muon mu);
  TString GetEtaLabel(Muon mu);

  TString GetElType(Electron el, const std::vector<Gen>& gens);
  TString GetElTypeString(Electron el, const std::vector<Gen>& gens);
  TString GetElTypeTString(Electron el, const std::vector<Gen>& gens);
  TString GetLepTypeTString(const Lepton& lep, const std::vector<Gen>& gens);

  double GetIsoFromID(HNL_LeptonCore::LeptonType lep, TString id, double eta, double pt);

  //==== LEPTON CHARGE                                                                                                                                                                                           
  bool SameCharge(vector<Electron> els, int ch=0);
  bool SameCharge(vector<Electron> els, vector<Muon> mus,  int ch=0);
  bool SameCharge(vector<Muon> mus, int ch=0);
  bool SameCharge(std::vector<Lepton *> leps, int ch=0);

  /// global var for user flags
 bool RunFake, RunCF,  RunSyst;
 bool IsSkimmed;
 bool Signal;
 bool IncludeFakeLepton;
 bool HEM1516 ,BeforeRun319077;
 bool DEBUG;

 
 bool DblEG_Channel;
 bool DblMuon_Channel;
 bool SglMuon_Channel;
 bool SglEG_Channel;
 bool MuonEG_Channel;
 
 bool IsCentral, RunFullAnalysis;
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

  
  
  // Lepton ID
  vector<TString> MuonVetoIDs;
  vector<TString> MuonLooseIDs;
  vector<TString> MuonTightIDs;
  vector<TString> ElectronVetoIDs;
  vector<TString> ElectronLooseIDs;
  // Fake rate file
  vector<TString> FakeRateIDs;
  vector<TString> ElectronTightIDs;


  bool run_Debug;
  // Lepton pT cut

  // obj vectors
  vector<Gen> gens;
  vector<Electron> AllElectrons;
  vector<Muon> AllMuons, AllTriggerSafePt_HNL_ElTunePMuons;
  vector<Jet> AllJets;
  vector<Tau> AllTaus;
  vector<FatJet> AllFatJets;

  double weight_Prefire, weight_Prefire_Up, weight_Prefire_Down;


  //==== My tool 

  // PLOTS

  void FillAllMuonPlots(TString label , TString cut,  Muon mu, float w);
  void FillAllMuonPlots(TString label , TString cut,  std::vector<Muon> muons, float w);
  void FillAllElectronPlots(TString label , TString cut,  std::vector<Electron> els, float w);


  // === Cut flow                                                                                                                                                                                      
  void FillTypeCutflow(TString histname, double weight, vector<TString> lables, TString label1, TString label2);
  void FillFullTypeCutflow(TString histname, double weight, vector<TString> lables, TString label1, TString label2);


  void FillEventCutflowDef(TString dirname, TString histname, double weight, vector<TString> lables, TString label);
  void FillEventCutflowAll(TString dirname, TString histname, double weight, vector<TString> lables, TString label);


  void FillCutFlow(bool IsCentral, TString suffix, TString histname, double weight);
  void FillEventCutflow(HNL_LeptonCore::Region sr,float wt,TString cut,    TString label);
  void FillEventCutflowSR(TString analysis_dir_name,HNL_LeptonCore::Region sr, float event_weight, TString label);
  void FillEventCutflowPerMass(TString dirname,Region sr, float event_weight, TString region_name,   TString label);




  //// ===============================  TOOLS =============================== ////                                                                                                                 
		   

 

  //// ===============================  SR PLOTS =============================== ////

  void Fill_RegionPlots(HNL_LeptonCore::Channel channel, bool plotCR,TString label_1, TString label_2,  std::vector<Jet> jets, std::vector<FatJet> fatjets,  std::vector<Electron> els, std::vector<Muon> mus, Particle  met, double nvtx, double w);
  void Fill_RegionPlots(HNL_LeptonCore::Channel channel, bool plotCR,TString label_1, TString label_2,  std::vector<Jet> jets, std::vector<Jet> jets_vbf, std::vector<FatJet> fatjets,  std::vector<Electron> els, std::vector<Muon> mus, Particle  met, double nvtx, double w);


  void Fill_SigRegionPlots1(HNL_LeptonCore::Channel channel,TString label_1, TString label_2, TString label_3,  std::vector<Jet> jets, std::vector<FatJet> fatjets,  std::vector<Electron> els, std::vector<Muon> mus, Particle  met, double nvtx, double w, double var1,  double var2, double var3, double var4, double var5, double var6, double var7);


  void Fill_SigRegionPlots3(HNL_LeptonCore::Channel channel, TString label_1, TString label_2, TString label_3,  std::vector<Jet> jets, std::vector<FatJet> fatjets,  std::vector<Electron> els, std::vector<Muon> mus, Particle  met, double nvtx, double w, double var1,  double var2, double var3, double var4, double var5, double var6, double var7, double var8, double var9, double var10, double var11);


  void Fill_SigRegionPlots4(HNL_LeptonCore::Channel channel,TString label_1,std::vector<Jet> jets, std::vector<FatJet> fatjets,  std::vector<Electron> els, std::vector<Muon> mus, Particle  met, double nvtx, double w );


  bool RunHighMassSR(ChargeType charge_i, TString channel_s,HNL_LeptonCore::Channel channel, TString charge_s,Particle llCand, TString label, std::vector<Jet> jets, std::vector<FatJet> fatjets,  std::vector<Electron> , std::vector<Muon> muons, Particle METv, int mPV, double w, Event ev, AnalyzerParameter param, bool FullAnalysis);



  bool  Fill_DefSigRegionPlots3(HNL_LeptonCore::Channel channel, TString label_sr, TString label_mass, TString label_anid,  std::vector<Jet> jets, std::vector<FatJet> fatjets,  std::vector<Electron> els, std::vector<Muon> mus, Particle  met, double nvtx,  double w,  double var1,  double var2, double var3, double var4, double var5, double var6, double var7, double var8, double var9, double var10, double var11);
					       
					       
  void Fill_All_SignalRegion3(HNL_LeptonCore::Channel channel, TString signal_region, bool isdata, TString charge_s, TString label, std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Electron> electrons, std::vector<Muon> muons,  Particle _met,int _npv , double w , bool full);
					
  void Fill_All_SignalRegion1(HNL_LeptonCore::Channel channel, TString signal_region, bool isdata, TString charge_s, TString label, std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Electron> electrons, std::vector<Muon> muons,  Particle _met,int _npv , double w , bool full);

  void MakeSignalPlots();
  

  //// ===============================  GLOBAL VAR =============================== ////


  int SignalLeptonChannel;


  TH1D *hist_PUReweight;
  TH1D *hist_PUReweight_Up;
  TH1D *hist_PUReweight_Down;

  double weight_PU, weight_PU_Up, weight_PU_Down;



};



#endif

