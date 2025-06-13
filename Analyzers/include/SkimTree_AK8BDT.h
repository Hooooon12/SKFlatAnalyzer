#ifndef SkimTree_AK8BDT_h
#define SkimTree_AK8BDT_h

#include "HNL_LeptonCore.h"

class SkimTree_AK8BDT : public HNL_LeptonCore {

 public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  void executeEvent();

  SkimTree_AK8BDT();
  ~SkimTree_AK8BDT();

  TTree *newtree;

  bool isSingleMu;

  vector<TString> triggers;
  vector<TString> triggers_dimu;
  vector<TString> triggers_di_el;
  vector<TString> triggers_emu;
  void WriteHist();

  double LeptonPtCut, AK4JetPtCut, AK8JetPtCut;

};



#endif
