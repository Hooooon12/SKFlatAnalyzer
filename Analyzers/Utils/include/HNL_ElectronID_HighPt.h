#ifndef HNL_ElectronID_HighPt_h
#define HNL_ElectronID_HighPt_h

#include "HNL_RegionDefinitions.h"

class HNL_ElectronID_HighPt : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_ElectronID_HighPt();
  ~HNL_ElectronID_HighPt();



};



#endif
