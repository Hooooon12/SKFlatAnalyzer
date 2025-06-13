#ifndef HNL_Validation_h
#define HNL_Validation_h

#include "HNL_RegionDefinitions.h"

class HNL_Validation : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_Validation();
  ~HNL_Validation();




};



#endif
