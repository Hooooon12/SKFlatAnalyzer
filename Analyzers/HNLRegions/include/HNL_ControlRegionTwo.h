#ifndef HNL_ControlRegionTwo_h
#define HNL_ControlRegionTwo_h

#include "HNL_RegionDefinitions.h"

class HNL_ControlRegionTwo : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_ControlRegionTwo();
  ~HNL_ControlRegionTwo();

  void RunControlRegions(AnalyzerParameter param, vector<TString> CRs);

};



#endif
