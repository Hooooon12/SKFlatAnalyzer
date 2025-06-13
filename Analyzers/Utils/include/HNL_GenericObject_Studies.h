#ifndef HNL_GenericObject_Studies_h
#define HNL_GenericObject_Studies_h

#include "HNL_RegionDefinitions.h"

class HNL_GenericObject_Studies : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_GenericObject_Studies();
  ~HNL_GenericObject_Studies();



  void RunLeptonChannel(HNL_LeptonCore::Channel channel_ID, std::vector<Lepton *> LepsT,std::vector<Lepton *> LepsV,std::vector<Tau> TauColl,  std::vector<Jet> JetCollLoose, std::vector<Jet> JetColl, std::vector<Jet> VBFJetColl,  std::vector<FatJet> FatjetColl,  std::vector<Jet> bjets, Event Ev, AnalyzerParameter param,  double  _weight)   ;

  void CheckHEMIssue(HNL_LeptonCore::Channel channel,std::vector<Lepton *> LepsT,std::vector<Lepton *> LepsV,       vector<Jet>& JetColl, Particle& vMET, float weight, TString Label, TString Option);


  void MakeType1SignalPlots(TString a, bool b);
  void MakeType1VBFSignalPlots(TString process, bool apply_reco_cut);
  void MakeType1SSWWSignalPlots(TString process, bool apply_reco_cut);

};



#endif
