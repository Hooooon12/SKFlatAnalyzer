#ifndef HNL_Lepton_ChargeFlip_h
#define HNL_Lepton_ChargeFlip_h

#include "HNL_RegionDefinitions.h"

class HNL_Lepton_ChargeFlip : public HNL_RegionDefinitions {

 public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  void executeEvent();

  HNL_Lepton_ChargeFlip();
  ~HNL_Lepton_ChargeFlip();

};



#endif
