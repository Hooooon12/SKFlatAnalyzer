#ifndef SkimTree_SSDileptonBDT_h
#define SkimTree_SSDileptonBDT_h

#include "HNL_LeptonCore.h"

class SkimTree_SSDileptonBDT : public HNL_LeptonCore {

 public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  void executeEvent();

  SkimTree_SSDileptonBDT();
  ~SkimTree_SSDileptonBDT();

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
