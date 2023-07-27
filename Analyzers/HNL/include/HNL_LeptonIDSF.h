#ifndef HNL_LeptonIDSF_h
#define HNL_LeptonIDSF_h

#include "HNL_RegionDefinitions.h"

class HNL_LeptonIDSF : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_LeptonIDSF();
  ~HNL_LeptonIDSF();


  void MeasureElectronIDSF(AnalyzerParameter param);
  void MeasureMuonIDSF(AnalyzerParameter param);
  void MeasureIDSF(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Muon > LeptonColl, TString ID, double w);
  void MeasureIDSF(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron > LeptonColl, TString ID, double w);
  void FilllHistBins(Lepton lep, bool passID,  TString Channel_string,TString _ID, double lep_weight);


  void PlotIDEfficiency(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, TString ID, double weight_ll);
  void PlotIDEfficiency(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Muon> MuonColl, TString ID, double weight_ll);

  void MeasureMuonEfficiencies(AnalyzerParameter param);
  void MeasureElectronEfficiencies(AnalyzerParameter param);


  void MakeBDTPlots(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, TString ID, double weight_ll);
};



#endif
