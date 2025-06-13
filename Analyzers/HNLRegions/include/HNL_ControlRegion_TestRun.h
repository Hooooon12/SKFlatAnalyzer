#ifndef HNL_ControlRegion_TestRun_h
#define HNL_ControlRegion_TestRun_h

#include "HNL_RegionDefinitions.h"

class HNL_ControlRegion_TestRun : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_ControlRegion_TestRun();
  ~HNL_ControlRegion_TestRun();

  void RunControlRegions(AnalyzerParameter param, vector<TString> CRs);

};



#endif
