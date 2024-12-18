#ifndef HNL_SignalRegion_TestRun_h
#define HNL_SignalRegion_TestRun_h

#include "HNL_RegionDefinitions.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"


class HNL_SignalRegion_TestRun : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_SignalRegion_TestRun();
  ~HNL_SignalRegion_TestRun();



  void RunULAnalysis(AnalyzerParameter param);

  ///void RunMuonChannel(std::vector<Lepton *> LepsT,std::vector<Lepton *> LepsV,std::vector<Tau> TauColl,  std::vector<Jet> JetColl, std::vector<Jet> VBFJetColl,  std::vector<FatJet> FatjetColl,  std::vector<Jet> bjets, Event Ev, AnalyzerParameter param,  float _weight)   ;


};



#endif
