#ifndef SkimTree_EGammaTnP_HNLHighPt_h
#define SkimTree_EGammaTnP_HNLHighPt_h

#include "HNL_LeptonCore.h"

class SkimTree_EGammaTnP_HNLHighPt : public HNL_LeptonCore {

 public:

  void initializeAnalyzer();
  void executeEvent();
  void WriteHist();

  bool IsTag(Electron el_tag);
  bool IsGoodTagProbe(Electron el_tag, Electron el_probe);

  static map<int,vector<pair<int,double>>> map_L1Threshold;
  double GetL1Threshold();

  SkimTree_EGammaTnP_HNLHighPt();
  ~SkimTree_EGammaTnP_HNLHighPt();

  TTree *newtree;
  TTree *weight_tree;
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
  bool passingMVABaseline;
  bool passingTriggerEmul;
  
  bool passingCutBasedMedium94XV2;
  bool passingCutBasedTight94XV2;
  bool passingHNLMVA;
  bool passingHNLMVA_HighPt;
  bool passingHNLMVA_TrkIso;
  bool passingHNLHeep;
  bool passingHNLMVACF;
  bool passingHNLMVAConv;
  bool passingHNLMVAFake;
  bool passingHNLMVA_NoConv;
  bool passingHNLMVA_NoCF;
  bool passingHNLMVA_NoFake;
  bool passingHNL_ULID_Split_1;
  bool passingHNL_ULID_Split_2;
  bool passingHNL_ULID_Split_3;
  bool passingHNL_ULID_Split_4;
  bool passingHNL_ULID_Split_4b;
  bool passingHNL_ULID_Split_5;
  bool passingHNL_ULID_Split_5b;
  bool passingHNL_ULID_Split_6;
  bool passingHNL_ULID_Split_7;
  bool passingHNL_ULID_Split_7b;
  bool passingHNL_ULID_Split_7c;
  bool passingHNL_ULID_Split_7d;
  bool passingHNL_ULID_Split_7e;
  bool passingHNL_ULID_Split_7f;
  bool passingHNL_ULID_Split_7g;
  bool passingHNL_ULID_Split_7h;
  bool passingHNL_ULID_Split_8;
  bool passingHNL_ULID_Split_8b;

  double scoreHNLMVACF;
  double scoreHNLMVAConv;
  double scoreHNLMVAFake;
  bool passEGL1SingleEGOr;
  bool passHltEle27WPTightGsf;
  bool passHltEle27eta2p1WPTightGsf;
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

  bool el_IsFake;
  bool el_IsConv;
  bool el_IsPromptConv;
  bool el_IsPrompt;
  bool el_IsCF;

  bool tag_passEGL1SingleEGOr;
  bool tag_passHltEle27WPTightGsf;
  bool tag_passHltEle27eta2p1WPTightGsf;
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

  bool passingHNL_ULID_Probe_Split_2;
  bool passingHNL_ULID_Probe_Split_3;
  bool passingHNL_ULID_Probe_Split_4;
  bool passingHNL_ULID_Probe_Split_5;
  bool passingHNL_ULID_Probe_Split_6;
  bool passingHNL_ULID_Probe_Split_7;
  bool passingHNL_ULID_Probe_Split_8;

  bool tag_IsFake;
  bool tag_IsConv;
  bool tag_IsPromptConv;
  bool tag_IsPrompt;
  bool tag_IsCF;

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

  bool mcTrue;

};



#endif
