#ifndef SkimTree_TnP_HighPt_h
#define SkimTree_TnP_HighPt_h

#include "AnalyzerCore.h"

class SkimTree_TnP_HighPt : public AnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  void executeEvent();

  SkimTree_TnP_HighPt();
  ~SkimTree_TnP_HighPt();

  TTree *newtree;

  vector<TString> single_muon_triggers;
  vector<TString> single_electron_triggers;
  void WriteHist();

};



#endif
