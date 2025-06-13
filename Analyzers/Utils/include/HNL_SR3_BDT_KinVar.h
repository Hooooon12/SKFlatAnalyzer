#ifndef HNL_SR3_BDT_KinVar_h
#define HNL_SR3_BDT_KinVar_h

#include "HNL_RegionDefinitions.h"

class HNL_SR3_BDT_KinVar : public HNL_RegionDefinitions {

 public:

  void initializeAnalyzer();
  void executeEventFromParameter(AnalyzerParameter param);
  void executeEvent();

  bool SS2l, TriLep, TetraLep; 
  bool DblMu, DblEG, MuEG;
  bool SystRun;
  vector<TString> TrigList_DblMu, TrigList_DblEG, TrigList_MuEG;

  void MakeTreeSS2L(HNL_LeptonCore::Channel, vector<Lepton *>  LepTColl,
                    vector<Jet>& JetAllColl,
                    vector<Jet>& JetColl, 
		    vector<Jet>& JetVBFColl,
                    vector<Jet>& JetBTagColl, 
		    Particle& vMET, float weight, TString Label);

  TTree *tree_mm;
  TTree *tree_ee;
  TTree *tree_em;
  void WriteHist();
  void InitializeTreeVars();

  Int_t Nj, Nvbfj, Nb, LQ;
  Float_t Ptl1, Ptl2, Ptj1, Ptj2, Ptj3, MET, MET2ST, HT, HTLT, HTLT1, HTLT2, LT, MET2HT, Etal1, Etal2;
  Float_t PtRatiol1, PtRatiol2, PtRatioAJl1, PtRatioAJl2, PtRelv0l1, PtRelv1l1, PtRelv0l2, PtRelv1l2;
  Float_t CEMFracAJl1, CEMFracAJl2, NEMFracAJl1, NEMFracAJl2, CHFracAJl1, CHFracAJl2, NHFracAJl1, NHFracAJl2, MuFracAJl1, MuFracAJl2, JetDiscAJl1, JetDiscAJl2;
  Float_t CEMFracCJl1, CEMFracCJl2, NEMFracCJl1, NEMFracCJl2, CHFracCJl1, CHFracCJl2, NHFracCJl1, NHFracCJl2, MuFracCJl1, MuFracCJl2, JetDiscCJl1, JetDiscCJl2;
  Float_t dEtall, dRll, dRjj12, dRjj23, dRjj13;
  Float_t dRlj11, dRlj12, dRlj13, dRlj21, dRlj22, dRlj23;
  Float_t dphivl1, dphivl2, dphivj1, dphivj2, dphivj3;
  Float_t MSSSF,  Mlj11, Mlj12, Mlj13, Mlj21, Mlj22, Mlj23;
  Float_t MTvl1, MTvl2, MTvll, Mllj1, Mllj2, Mllj3, Mllj4;
  Float_t Mlljj12, Mlljj13, Mlljj14, Mlljj23, Mlljj24, Mlljj34;
  Float_t Mljj112, Mljj113, Mljj114, Mljj123, Mljj124, Mljj134;
  Float_t Mljj212, Mljj213, Mljj214, Mljj223, Mljj224, Mljj234;
  Float_t Mjj12, Mjj13, Mjj14, Mjj23, Mjj24, Mjj34;

  Float_t PtWj1, PtWj2;
  Float_t dRWjj, dRlW12, dRlW22, dRlN12, dRlN21;
  Float_t M_W2_jj, M_N1_l1jj, M_N2_l2jj, M_W1_lljj;
  
  Float_t w_tot;

  HNL_SR3_BDT_KinVar();
  ~HNL_SR3_BDT_KinVar();

};



#endif
