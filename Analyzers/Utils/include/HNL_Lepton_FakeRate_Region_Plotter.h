#ifndef HNL_Lepton_FakeRate_Region_Plotter_h
#define HNL_Lepton_FakeRate_Region_Plotter_h

#include "HNL_LeptonCore.h"


class HNL_Lepton_FakeRate_Region_Plotter  : public HNL_LeptonCore {

 public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  void RunE(std::vector<Electron> electrons, std::vector<Muon> muons, std::vector<Jet> jets,AnalyzerParameter param,  float w);
  void RunM(std::vector<Electron> electrons, std::vector<Muon> muons, std::vector<Jet> jets, AnalyzerParameter param,  float w);

  
  void executeEvent();

  HNL_Lepton_FakeRate_Region_Plotter();
  ~HNL_Lepton_FakeRate_Region_Plotter();


  void FillRegionPlots( TString plot_dir, TString region,    std::vector<Jet> jets,  std::vector<Electron> els, std::vector<Muon> mus, Particle  met,  double w);

  double ApplyNvtxReweight(int Nvtx, TString Key);
  
  void MakeDiLepPlots(HNL_LeptonCore::Channel channel, AnalyzerParameter param, Event ev, std::vector<Lepton *> leps,std::vector<bool> blepsT,  TString label, float event_weight);
				    
  bool UseEvent(std::vector<Lepton *> leps,   std::vector< Jet> jets, float awayjetcut, Particle MET, float wt);


  //==== pileup

  void FillWeightHist(TString label, double _weight);
  void FillCutFlow(bool IsCentral, TString suffix, TString histname, double weight);
  void FillEventCutflow(int charge_s,int version,float wt,TString cut,    TString label);

  TFile* NvtxSFFile;
  map< TString, TH1D* > maphist_NvtxSF;



};



#endif

