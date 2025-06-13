#ifndef HNL_ControlRegion_Plotter_h
#define HNL_ControlRegion_Plotter_h

#include "HNL_RegionDefinitions.h"

class HNL_ControlRegion_Plotter : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_ControlRegion_Plotter();
  ~HNL_ControlRegion_Plotter();

  void RunControlRegions(AnalyzerParameter param, vector<TString> CRs);

};



#endif
