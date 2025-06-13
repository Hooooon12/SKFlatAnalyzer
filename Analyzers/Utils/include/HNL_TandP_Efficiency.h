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
  void RunTaPMuon(TString ID,AnalyzerParameter param, double weight);
  void RunPeking(AnalyzerParameter param, double weight);

  bool IsTag(Electron el_tag);
  bool IsGoodTagProbe(Electron el_tag, Electron el_probe);

};



#endif
