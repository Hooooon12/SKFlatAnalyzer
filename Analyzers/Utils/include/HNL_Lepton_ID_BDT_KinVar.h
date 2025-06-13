#ifndef HNL_Lepton_ID_BDT_KinVar_h
#define HNL_Lepton_ID_BDT_KinVar_h

#include "HNL_RegionDefinitions.h"

class HNL_Lepton_ID_BDT_KinVar : public HNL_RegionDefinitions {

 public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  void executeEvent();
  void SetupTrees(TTree* tree_mm, TTree* tree_ee);

  bool SS2l, TriLep, TetraLep; 
  bool DblMu, DblEG, MuEG;
  bool SystRun;
  bool RunIB,RunOB,RunEC;

  vector<TString> TrigList_DblMu, TrigList_DblEG, TrigList_MuEG;

  void MakeTreeSS2L(HNL_LeptonCore::Channel, vector<Lepton *>  LepTColl,
		    vector<Muon>Muons,
		    vector<Electron> Electrons,
                    vector<Jet>& JetAllColl,
		    float weight, TString Label);


  bool GetRatio;
  bool SeperateFakes;
  bool SeperateConv;
  bool SeperateCF;
  bool SeperatePrompt;

  TTree *tree_mm;
  TTree *tree_ee;
  //TTree *tree_mm_OB;
  //TTree *tree_ee_OB;

  //TTree *tree_mm_EC;
  //TTree *tree_ee_EC;


  void WriteHist();


  HNL_Lepton_ID_BDT_KinVar();
  ~HNL_Lepton_ID_BDT_KinVar();

};



#endif
