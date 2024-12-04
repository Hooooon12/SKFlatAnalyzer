#ifndef HNL_TandP_Efficiency_h
#define HNL_TandP_Efficiency_h

#include "HNL_RegionDefinitions.h"

class HNL_TandP_Efficiency : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_TandP_Efficiency();
  ~HNL_TandP_Efficiency();

  void RunHighPt(AnalyzerParameter param, double weight);
  void RunHNL(AnalyzerParameter param, double weight);
  void RunPeking(AnalyzerParameter param, double weight);


};



#endif
