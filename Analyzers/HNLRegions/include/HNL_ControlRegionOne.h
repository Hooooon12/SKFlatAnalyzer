#ifndef HNL_ControlRegionOne_h
#define HNL_ControlRegionOne_h

#include "HNL_RegionDefinitions.h"

class HNL_ControlRegionOne : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_ControlRegionOne();
  ~HNL_ControlRegionOne();

  void RunControlRegions(AnalyzerParameter param, vector<TString> CRs);

};



#endif
