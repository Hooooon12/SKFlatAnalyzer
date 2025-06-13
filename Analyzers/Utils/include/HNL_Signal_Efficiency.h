#ifndef HNL_Signal_Efficiency_h
#define HNL_Signal_Efficiency_h

#include "HNL_RegionDefinitions.h"

class HNL_Signal_Efficiency : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_Signal_Efficiency();
  ~HNL_Signal_Efficiency();



};



#endif
