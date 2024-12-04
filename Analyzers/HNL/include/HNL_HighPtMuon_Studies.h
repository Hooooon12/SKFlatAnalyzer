#ifndef HNL_HighPtMuon_Studies_h
#define HNL_HighPtMuon_Studies_h



#include "HNL_RegionDefinitions.h"

class HNL_HighPtMuon_Studies : public HNL_RegionDefinitions {


 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_HighPtMuon_Studies();
  ~HNL_HighPtMuon_Studies();



};



#endif
