#ifndef HNL_CrossCheck_h
#define HNL_CrossCheck_h

#include "HNL_RegionDefinitions.h"

class HNL_CrossCheck : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_CrossCheck();
  ~HNL_CrossCheck();

  void RunControlRegions(AnalyzerParameter param, vector<TString> CRs);

};



#endif
