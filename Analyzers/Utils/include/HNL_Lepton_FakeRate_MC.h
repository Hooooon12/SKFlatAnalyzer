#ifndef HNL_Lepton_FakeRate_MC_h
#define HNL_Lepton_FakeRate_MC_h

#include "HNL_LeptonCore.h"


class HNL_Lepton_FakeRate_MC  : public HNL_LeptonCore {

 public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  void RunE(std::vector<Electron> electrons, std::vector<Muon> muons, std::vector<Jet> jets,AnalyzerParameter param,  float w);
  void RunM(std::vector<Electron> electrons, std::vector<Muon> muons, std::vector<Jet> jets, AnalyzerParameter param,  float w);

  
  void executeEvent();

  HNL_Lepton_FakeRate_MC();
  ~HNL_Lepton_FakeRate_MC();


  void GetMuFakeRates(TString Method, Lepton * lep, bool blepsT,  AnalyzerParameter param,  std::vector<Jet> jets,  TString tag,float event_weight);
  void GetElFakeRates(TString Method, Lepton * lep, bool blepsT,  AnalyzerParameter param,  std::vector<Jet> jets,  TString tag,float event_weight);

				   
  bool UseEvent(std::vector<Lepton *> leps,   std::vector< Jet> jets, float awayjetcut, Particle MET, float wt);

  double GetMuonMCFakeRate(Muon mu, TString version);
  double GetElectronMCFakeRate(Electron el, TString version);


  //==== pileup

  void FillCutFlow(bool IsCentral, TString suffix, TString histname, double weight);
  void FillEventCutflow(int charge_s,int version,float wt,TString cut,    TString label);

  TFile* NvtxSFFile;
  map< TString, TH1D* > maphist_NvtxSF;



};



#endif

