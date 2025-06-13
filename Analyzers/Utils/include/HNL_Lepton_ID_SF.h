#ifndef HNL_Lepton_ID_SF_h
#define HNL_Lepton_ID_SF_h

#include "HNL_RegionDefinitions.h"

class HNL_Lepton_ID_SF : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_Lepton_ID_SF();
  ~HNL_Lepton_ID_SF();


  void MeasureElectronIDSF(AnalyzerParameter param);
  void MeasureMuonIDSF(AnalyzerParameter param);
  void MeasureIDSF(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Muon > LeptonColl, TString ID, double w);
  void MeasureIDSF(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron > LeptonColl, TString ID, double w);

  void MeasureIDSF(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, vector<bool> PassIDs, TString ID, double weight_ll);

  void FilllHistBins(Lepton lep, bool passID,  TString Channel_string,TString _ID, double lep_weight);


  void PlotIDEfficiency(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, TString ID, double weight_ll);

  void PlotIDEfficiency(AnalyzerParameter param, TString dilep_channel, vector<Electron> ElectronColl, TString ID, double weight_ll);

  void PlotIDEfficiency(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Muon> MuonColl, TString ID, double weight_ll);

  void MeasureMuonEfficiencies(AnalyzerParameter param);
  void MeasureElectronEfficiencies(AnalyzerParameter param);

  void MakeBDTPlots(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, TString ID, double weight_ll);
};



#endif
