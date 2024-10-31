#ifndef SkimTree_HNFakeInclusive_h
#define SkimTree_HNFakeInclusive_h

#include "HNL_LeptonCore.h"

class SkimTree_HNFakeInclusive : public HNL_LeptonCore {

public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  void executeEvent();

  SkimTree_HNFakeInclusive();
  ~SkimTree_HNFakeInclusive();

  TTree *newtree;



  vector<TString> triggers;
  void WriteHist();

};



#endif
