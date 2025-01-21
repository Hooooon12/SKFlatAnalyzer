#ifndef HNL_TriggerEfficiency_h
#define HNL_TriggerEfficiency_h

#include "HNL_LeptonCore.h"

class HNL_TriggerEfficiency : public HNL_LeptonCore {

 public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  void executeEvent();

  bool EE, MuMu, EMu; 
  bool DiMuTrig_DZ, DiMuMFilter, DiElTrig_DZ, EMuTrig_ElLeg, EMuTrig_MuLeg, EMuTrig_DZ;
  bool SystRun;
  vector<TString> TrigList;
  vector<TString> TrigList_SglMu, TrigList_SglEl, TrigList_DblMu, TrigList_DblEG, TrigList_EMu;
  vector<TString> TrigList_DblMuDZ, TrigList_DblMuDZM, TrigList_DblEGDZ, TrigList_EMuDZ;
  TString SFKey_Trig;

  void MeasSiglEleTrigEff(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                          vector<Jet>& JetColl, vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label);

  void MeasEffDiMuTrig_MuLeg(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                             vector<Jet>& JetColl,  vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label);

  void MeasEffDiMuTrig_DZ(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                          vector<Jet>& JetColl, vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label);
  void MeasEffDiElTrig_DZ(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                          vector<Jet>&  JetColl, vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label);
  void MeasEffEMuTrig_ElLeg(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                            vector<Jet>& JetColl,  vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label);
  void MeasEffEMuTrig_MuLeg(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                            vector<Jet>& JetColl,  vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label);
  void MeasEffEMuTrig_DZ(vector<Muon>& MuTColl, vector<Muon>& MuLColl, vector<Electron>& ElTColl, vector<Electron>& ElLColl,
                         vector<Jet>& JetColl, vector<Jet>& BJetColl, Particle& vMET, Event& ev, float weight, TString Label);


  HNL_TriggerEfficiency();
  ~HNL_TriggerEfficiency();

};



#endif
