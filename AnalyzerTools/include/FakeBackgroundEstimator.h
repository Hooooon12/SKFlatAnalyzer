#ifndef FakeBackgroundEstimator_h
#define FakeBackgroundEstimator_h

#include <map>
#include <vector>
#include <fstream>
#include <sstream>

#include "TFile.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"

#include "Lepton.h"
#include "Muon.h"
#include "Electron.h"

#include "AnalyzerParameter.h"
#include "TDirectoryHelper.h"

class FakeBackgroundEstimator{

public:

  FakeBackgroundEstimator();
  ~FakeBackgroundEstimator();

  TDirectory *histDir;

  void ReadHistograms(bool IsData, bool ScanIDs);

  bool IgnoreNoHist;

  int DataYear;
  TString DataEra;
  void SetEra(TString era){
    DataEra=era;
    DataYear=TString(era(0,4)).Atoi();
  }
  TString GetEra() const { return DataEra; }
  int GetYear() const { return DataYear; }

  std::map< TString, TH2D* > map_hist_Electron;
  std::map< TString, TH2D* > map_hist_Muon;

  double GetFakeRate(bool isMuon,TString ID, TString key, TString BinningMethod,TString BinningParam,    double eta, double pt, TString FakeTagger, int sys=0);
  double GetElectronFakeRate(TString ID, TString key, TString BinningMethod,TString BinningParam, double eta, double pt, TString FakeTagger, int sys=0);
  double GetMuonFakeRate    (TString ID, TString key, TString BinningMethod,TString BinningParam,    double eta, double pt, TString FakeTagger, int sys=0);


  double HighPtCorr(TString ID,double eta, double pt, int sys);

  double GetPromptRate(bool ApplyR,bool isMuon, TString ID, TString key, double eta, double pt, int sys=0);
  double GetElectronPromptRate(TString ID, TString key, double eta, double pt, int sys=0);
  double GetMuonPromptRate(TString ID, TString key, double eta, double pt, int sys=0);


  bool HasLooseLepton;

  float CalculateLepWeight(float r1, float fr1, bool lep1Tight);
  float CalculateDilepWeight(float r1, float fr1, float r2, float fr2, bool lep1Tight, bool lep2Tight, int eventtype);
  
  vector<TString> MissingHists;


};

#endif
