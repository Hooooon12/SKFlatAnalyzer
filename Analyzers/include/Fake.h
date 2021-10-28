#ifndef Fake_h
#define Fake_h

#include "AnalyzerCore.h"

class Fake : public AnalyzerCore {

public:

  void initializeAnalyzer();

  void executeEventFromParameter(AnalyzerParameter param, Long64_t Nentry);
  void executeEvent(Long64_t Nentry);

  bool RunSyst, RunFake, RunCF;
  bool RunNewPDF;
  bool RunXSecSyst;

  // Trigger
  TString MuonTrigger3, MuonTrigger8, MuonTrigger17, ElectronTrigger8, ElectronTrigger12, ElectronTrigger17, ElectronTrigger23;
  vector<TString> MuonTriggers;
  vector<TString> MuonTriggersBtoG;
  vector<TString> MuonTriggersH;
  vector<TString> MuonTriggersHighPt;
  vector<TString> ElectronTriggers;
  vector<TString> EMuTriggers;
  vector<TString> EMuTriggersBtoG;
  vector<TString> EMuTriggersH;

  // Lepton ID
  vector<TString> MuonVetoIDs;
  vector<TString> MuonLooseIDs;
  vector<TString> MuonTightIDs;
  vector<TString> ElectronVetoIDs;
  vector<TString> ElectronLooseIDs;
  vector<TString> ElectronTightIDs;

  // Fake rate file
  vector<TString> FakeRateIDs;

  // Lepton pT cut
  double MuonPtCut;
  double MuonPtCut1;
  double MuonPtCut2;
  double ElectronPtCut;
  double ElectronPtCut1;
  double ElectronPtCut2;
  double EMuPtCut1;
  double EMuPtCut2;

//  vector<TString> EleIDs, EleIDSFKeys, MuonIDs, MuonIDSFKeys;
  vector<Electron> AllElectrons;
  vector<Muon> AllMuons;
  vector<Jet> AllJets;
  vector<FatJet> AllFatJets;

//  double weight_Prefire;

  Fake();
  ~Fake();

};

#endif
