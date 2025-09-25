#ifndef HNL_ControlRegionLLL_h
#define HNL_ControlRegionLLL_h

#include "HNL_RegionDefinitions.h"

class HNL_ControlRegionLLL : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_ControlRegionLLL();
  ~HNL_ControlRegionLLL();

  void RunControlRegions(AnalyzerParameter param, vector<TString> CRs);

};



#endif
