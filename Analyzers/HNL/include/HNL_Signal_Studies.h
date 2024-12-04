#ifndef HNL_Signal_Studies_h
#define HNL_Signal_Studies_h

#include "HNL_RegionDefinitions.h"

class HNL_Signal_Studies : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_Signal_Studies();
  ~HNL_Signal_Studies();


  vector<Particle> GetJetWW();

  void RunLeptonChannel(HNL_LeptonCore::Channel channel_ID, std::vector<Lepton *> LepsT,std::vector<Lepton *> LepsV,std::vector<Tau> TauColl,  std::vector<Jet> JetCollLoose, std::vector<Jet> JetColl, std::vector<Jet> VBFJetColl,  std::vector<FatJet> FatjetColl,  std::vector<Jet> bjets, Event Ev, AnalyzerParameter param,  double _weight)   ;

  void CheckHEMIssue(HNL_LeptonCore::Channel channel,std::vector<Lepton *> LepsT,std::vector<Lepton *> LepsV,       vector<Jet>& JetColl, Particle& vMET, float weight, TString Label, TString Option);

  void PlotBDTVariablesMuon(AnalyzerParameter param);
  void PlotBDTVariablesElectron(AnalyzerParameter param);


  void MakeType1SignalPlots(TString a, bool b);
  void MakeType1VBFSignalPlots(TString process, bool apply_reco_cut);
  void MakeType1SSWWSignalPlots(TString process, bool apply_reco_cut);

};



#endif
