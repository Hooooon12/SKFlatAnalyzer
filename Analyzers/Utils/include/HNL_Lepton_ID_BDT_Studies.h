#ifndef HNL_Lepton_ID_BDT_Studies_h
#define HNL_Lepton_ID_BDT_Studies_h

#include "HNL_RegionDefinitions.h"

class HNL_Lepton_ID_BDT_Studies : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_Lepton_ID_BDT_Studies();
  ~HNL_Lepton_ID_BDT_Studies();

  double  GetCorrCloseJetDisc(bool ismuon, double jetdisc);
  void CheckSSFakeBreakDown(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, vector<Muon> MuonColl, double weight_ll);
  void MakeBDTPlots(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, TString ID, double weight_ll);
  void MakeJetDiscPlots(TString label, AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl ,vector<Muon> MuonColl, double weight_ll);
};



#endif
