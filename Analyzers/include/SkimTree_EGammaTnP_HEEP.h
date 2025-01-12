#ifndef SkimTree_EGammaTnP_HEEP_h
#define SkimTree_EGammaTnP_HEEP_h

#include "HNL_LeptonCore.h"

class SkimTree_EGammaTnP_HEEP : public HNL_LeptonCore {

public:

  void initializeAnalyzer();
  void executeEvent();
  void WriteHist();

  bool IsTag(Electron el_tag);
  bool IsGoodTagProbe(Electron el_tag, Electron el_probe);

  static map<int,vector<pair<int,double>>> map_L1Threshold;
  double GetL1Threshold();

  SkimTree_EGammaTnP_HEEP();
  ~SkimTree_EGammaTnP_HEEP();

  TTree* newtree;
  TTree* weight_tree;
  float weight;
  float PUweight,PUweight_up,PUweight_down;
  float prefireweight,prefireweight_up,prefireweight_down;
  float zptweight;
  float z0weight;
  float totWeight;
  
  float L1ThresholdHLTEle23Ele12CaloIdLTrackIdLIsoVL;

  bool passingMVA80;
  bool passingMVA90;
  bool passingHEEP;
  bool passingMVALoose;
  bool passingTriggerEmul;
  
  bool passingCutBasedMedium94XV2;
  bool passingCutBasedTight94XV2;
  bool passingHNLMVA;
  bool passingHNLMVA_HighPt;
  bool passingHNLMVA_HighPt_Tight;
  bool passingHNLMVA_TrkIso;
  bool passingHNLHeep;
  bool passingHNLMVACF;
  bool passingHNLMVAConv;
  bool passingHNLMVAFake;
  double scoreHNLMVACF;
  double scoreHNLMVAConv;
  double scoreHNLMVAFake;
  bool passEGL1SingleEGOr;
  bool passHltEle27WPTightGsf;
  bool passHltEle28WPTightGsf;
  bool passHltEle32WPTightGsf;
  bool passHltEle32DoubleEGWPTightGsf;
  bool passHltEle35WPTightGsf;
  bool passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg1;
  bool passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg2;
  float el_sc_abseta;
  float el_e;
  float el_e_cor;
  float el_et;
  float el_et_cor;
  float el_pt;
  float el_pt_cor;
  float el_eta;
  float el_abseta;
  float el_sc_eta;
  float el_phi;
  int el_q;
  bool el_3charge;
  float el_l1et;

  bool tag_passEGL1SingleEGOr;
  bool tag_passHltEle27WPTightGsf;
  bool tag_passHltEle28WPTightGsf;
  bool tag_passHltEle32WPTightGsf;
  bool tag_passHltEle32DoubleEGWPTightGsf;
  bool tag_passHltEle35WPTightGsf;
  bool tag_passingCutBasedMedium94XV2;
  bool tag_passingCutBasedTight94XV2;
  bool tag_passtrigMVA;
  bool tag_passingMVA80;
  bool tag_passingMVA90;
  bool tag_passingHEEP;
  bool tag_passingMVALoose;
  bool tag_passingTriggerEmul;


  float tag_Ele_IsoMVA94XV2;
  float tag_Ele_e;
  float tag_Ele_e_cor;
  float tag_Ele_et;
  float tag_Ele_et_cor;
  float tag_Ele_pt;
  float tag_Ele_pt_cor;
  float tag_Ele_eta;
  float tag_Ele_abseta;
  float tag_Ele_phi;
  float tag_Ele_q;
  bool tag_Ele_3charge;
  float tag_sc_eta;

  float pair_mass;
  float pair_mass_cor;
  float pair_pt;
  float pair_pt_cor;

  float mc_probe_e;
  float mc_probe_et;
  float mc_probe_eta;
  float mc_probe_phi;
  bool mcTrue;
  float mcMass;

};



#endif

