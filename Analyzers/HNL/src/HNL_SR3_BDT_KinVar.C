#include "HNL_SR3_BDT_KinVar.h"

//const float MZ = 91.1876, MW = 80.379;

void HNL_SR3_BDT_KinVar::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

  InitializeTreeVars();
  
  tree_mm->Branch("Nj", &Nj, "Nj/I");                              tree_ee->Branch("Nj", &Nj, "Nj/I");                              tree_em->Branch("Nj", &Nj, "Nj/I");
  tree_mm->Branch("Nvbfj", &Nvbfj, "Nvbfj/I");                     tree_ee->Branch("Nvbfj", &Nvbfj, "Nvbfj/I");                     tree_em->Branch("Nvbfj", &Nvbfj, "Nvbfj/I");
  tree_mm->Branch("Nb", &Nb, "Nb/I");                              tree_ee->Branch("Nb", &Nb, "Nb/I");                              tree_em->Branch("Nb", &Nb, "Nb/I");

  tree_mm->Branch("LQ", &LQ, "LQ/I");                              tree_ee->Branch("LQ", &LQ, "LQ/I");                              tree_em->Branch("LQ", &LQ, "LQ/I");
  tree_mm->Branch("Ptl1", &Ptl1, "Ptl1/F");                        tree_ee->Branch("Ptl1", &Ptl1, "Ptl1/F");                        tree_em->Branch("Ptl1", &Ptl1, "Ptl1/F");
  tree_mm->Branch("Ptl2", &Ptl2, "Ptl2/F");                        tree_ee->Branch("Ptl2", &Ptl2, "Ptl2/F");                        tree_em->Branch("Ptl2", &Ptl2, "Ptl2/F");
  tree_mm->Branch("Ptj1", &Ptj1, "Ptj1/F");                        tree_ee->Branch("Ptj1", &Ptj1, "Ptj1/F");                        tree_em->Branch("Ptj1", &Ptj1, "Ptj1/F");
  tree_mm->Branch("Ptj2", &Ptj2, "Ptj2/F");                        tree_ee->Branch("Ptj2", &Ptj2, "Ptj2/F");                        tree_em->Branch("Ptj2", &Ptj2, "Ptj2/F");
  tree_mm->Branch("Ptj3", &Ptj3, "Ptj3/F");                        tree_ee->Branch("Ptj3", &Ptj3, "Ptj3/F");                        tree_em->Branch("Ptj3", &Ptj3, "Ptj3/F");
  tree_mm->Branch("MET", &MET, "MET/F");                           tree_ee->Branch("MET", &MET, "MET/F");                           tree_em->Branch("MET", &MET, "MET/F");
  tree_mm->Branch("MET2ST", &MET2ST, "MET2ST/F");                  tree_ee->Branch("MET2ST", &MET2ST, "MET2ST/F");                  tree_em->Branch("MET2ST", &MET2ST, "MET2ST/F");
  tree_mm->Branch("HT", &HT, "HT/F");                              tree_ee->Branch("HT", &HT, "HT/F");                              tree_em->Branch("HT", &HT, "HT/F");
  tree_mm->Branch("LT", &LT, "LT/F");                              tree_ee->Branch("LT", &LT, "LT/F");                              tree_em->Branch("LT", &LT, "LT/F");
  tree_mm->Branch("HTLT", &HTLT, "HTLT/F");                        tree_ee->Branch("HTLT", &HTLT, "HTLT/F");                        tree_em->Branch("HTLT", &HTLT, "HTLT/F");
  tree_mm->Branch("HTLT1", &HTLT1, "HTLT1/F");                     tree_ee->Branch("HTLT1", &HTLT1, "HTLT1/F");                     tree_em->Branch("HTLT1", &HTLT1, "HTLT1/F");
  tree_mm->Branch("HTLT2", &HTLT2, "HTLT2/F");                     tree_ee->Branch("HTLT2", &HTLT2, "HTLT2/F");                     tree_em->Branch("HTLT2", &HTLT2, "HTLT2/F");
  tree_mm->Branch("MET2HT", &MET2HT, "MET2HT/F");                  tree_ee->Branch("MET2HT", &MET2HT, "MET2HT/F");                  tree_em->Branch("MET2HT", &MET2HT, "MET2HT/F");
  tree_mm->Branch("Etal1", &Etal1, "Etal1/F");                     tree_ee->Branch("Etal1", &Etal1, "Etal1/F");                     tree_em->Branch("Etal1", &Etal1, "Etal1/F");
  tree_mm->Branch("Etal2", &Etal2, "Etal2/F");                     tree_ee->Branch("Etal2", &Etal2, "Etal2/F");                     tree_em->Branch("Etal2", &Etal2, "Etal2/F");

  //tree_mm->Branch("PtRatiol1", &PtRatiol1, "PtRatiol1/F");         tree_ee->Branch("PtRatiol1", &PtRatiol1, "PtRatiol1/F");         tree_em->Branch("PtRatiol1", &PtRatiol1, "PtRatiol1/F");
  //tree_mm->Branch("PtRatiol2", &PtRatiol2, "PtRatiol2/F");         tree_ee->Branch("PtRatiol2", &PtRatiol2, "PtRatiol2/F");         tree_em->Branch("PtRatiol2", &PtRatiol2, "PtRatiol2/F");
  tree_mm->Branch("PtRatioAJl1", &PtRatioAJl1, "PtRatioAJl1/F");   tree_ee->Branch("PtRatioAJl1", &PtRatioAJl1, "PtRatioAJl1/F");   tree_em->Branch("PtRatioAJl1", &PtRatioAJl1, "PtRatioAJl1/F");
  tree_mm->Branch("PtRatioAJl2", &PtRatioAJl2, "PtRatioAJl2/F");   tree_ee->Branch("PtRatioAJl2", &PtRatioAJl2, "PtRatioAJl2/F");   tree_em->Branch("PtRatioAJl2", &PtRatioAJl2, "PtRatioAJl2/F");
  //tree_mm->Branch("PtRelv0l1", &PtRelv0l1, "PtRelv0l1/F");         tree_ee->Branch("PtRelv0l1", &PtRelv0l1, "PtRelv0l1/F");         tree_em->Branch("PtRelv0l1", &PtRelv0l1, "PtRelv0l1/F");
  //tree_mm->Branch("PtRelv0l2", &PtRelv0l2, "PtRelv0l2/F");         tree_ee->Branch("PtRelv0l2", &PtRelv0l2, "PtRelv0l2/F");         tree_em->Branch("PtRelv0l2", &PtRelv0l2, "PtRelv0l2/F");
  //tree_mm->Branch("PtRelv1l1", &PtRelv1l1, "PtRelv1l1/F");         tree_ee->Branch("PtRelv1l1", &PtRelv1l1, "PtRelv1l1/F");         tree_em->Branch("PtRelv1l1", &PtRelv1l1, "PtRelv1l1/F");
  //tree_mm->Branch("PtRelv1l2", &PtRelv1l2, "PtRelv1l2/F");         tree_ee->Branch("PtRelv1l2", &PtRelv1l2, "PtRelv1l2/F");         tree_em->Branch("PtRelv1l2", &PtRelv1l2, "PtRelv1l2/F");
  tree_mm->Branch("CEMFracAJl1", &CEMFracAJl1, "CEMFracAJl1/F");   tree_ee->Branch("CEMFracAJl1", &CEMFracAJl1, "CEMFracAJl1/F");   tree_em->Branch("CEMFracAJl1", &CEMFracAJl1, "CEMFracAJl1/F");
  tree_mm->Branch("CEMFracAJl2", &CEMFracAJl2, "CEMFracAJl2/F");   tree_ee->Branch("CEMFracAJl2", &CEMFracAJl2, "CEMFracAJl2/F");   tree_em->Branch("CEMFracAJl2", &CEMFracAJl2, "CEMFracAJl2/F");
  tree_mm->Branch("NEMFracAJl1", &NEMFracAJl1, "NEMFracAJl1/F");   tree_ee->Branch("NEMFracAJl1", &NEMFracAJl1, "NEMFracAJl1/F");   tree_em->Branch("NEMFracAJl1", &NEMFracAJl1, "NEMFracAJl1/F");
  tree_mm->Branch("NEMFracAJl2", &NEMFracAJl2, "NEMFracAJl2/F");   tree_ee->Branch("NEMFracAJl2", &NEMFracAJl2, "NEMFracAJl2/F");   tree_em->Branch("NEMFracAJl2", &NEMFracAJl2, "NEMFracAJl2/F");
  tree_mm->Branch("CHFracAJl1", &CHFracAJl1, "CHFracAJl1/F");      tree_ee->Branch("CHFracAJl1", &CHFracAJl1, "CHFracAJl1/F");      tree_em->Branch("CHFracAJl1", &CHFracAJl1, "CHFracAJl1/F");
  tree_mm->Branch("CHFracAJl2", &CHFracAJl2, "CHFracAJl2/F");      tree_ee->Branch("CHFracAJl2", &CHFracAJl2, "CHFracAJl2/F");      tree_em->Branch("CHFracAJl2", &CHFracAJl2, "CHFracAJl2/F");
  tree_mm->Branch("NHFracAJl1", &NHFracAJl1, "NHFracAJl1/F");      tree_ee->Branch("NHFracAJl1", &NHFracAJl1, "NHFracAJl1/F");      tree_em->Branch("NHFracAJl1", &NHFracAJl1, "NHFracAJl1/F");
  tree_mm->Branch("NHFracAJl2", &NHFracAJl2, "NHFracAJl2/F");      tree_ee->Branch("NHFracAJl2", &NHFracAJl2, "NHFracAJl2/F");      tree_em->Branch("NHFracAJl2", &NHFracAJl2, "NHFracAJl2/F");
  tree_mm->Branch("MuFracAJl1", &MuFracAJl1, "MuFracAJl1/F");      tree_ee->Branch("MuFracAJl1", &MuFracAJl1, "MuFracAJl1/F");      tree_em->Branch("MuFracAJl1", &MuFracAJl1, "MuFracAJl1/F");
  tree_mm->Branch("MuFracAJl2", &MuFracAJl2, "MuFracAJl2/F");      tree_ee->Branch("MuFracAJl2", &MuFracAJl2, "MuFracAJl2/F");      tree_em->Branch("MuFracAJl2", &MuFracAJl2, "MuFracAJl2/F");
  tree_mm->Branch("JetDiscAJl1", &JetDiscAJl1, "JetDiscAJl1/F");   tree_ee->Branch("JetDiscAJl1", &JetDiscAJl1, "JetDiscAJl1/F");   tree_em->Branch("JetDiscAJl1", &JetDiscAJl1, "JetDiscAJl1/F");
  tree_mm->Branch("JetDiscAJl2", &JetDiscAJl2, "JetDiscAJl2/F");   tree_ee->Branch("JetDiscAJl2", &JetDiscAJl2, "JetDiscAJl2/F");   tree_em->Branch("JetDiscAJl2", &JetDiscAJl2, "JetDiscAJl2/F");
  //tree_mm->Branch("CEMFracCJl1", &CEMFracCJl1, "CEMFracCJl1/F");   tree_ee->Branch("CEMFracCJl1", &CEMFracCJl1, "CEMFracCJl1/F");   tree_em->Branch("CEMFracCJl1", &CEMFracCJl1, "CEMFracCJl1/F");
  //tree_mm->Branch("CEMFracCJl2", &CEMFracCJl2, "CEMFracCJl2/F");   tree_ee->Branch("CEMFracCJl2", &CEMFracCJl2, "CEMFracCJl2/F");   tree_em->Branch("CEMFracCJl2", &CEMFracCJl2, "CEMFracCJl2/F");
  //tree_mm->Branch("NEMFracCJl1", &NEMFracCJl1, "NEMFracCJl1/F");   tree_ee->Branch("NEMFracCJl1", &NEMFracCJl1, "NEMFracCJl1/F");   tree_em->Branch("NEMFracCJl1", &NEMFracCJl1, "NEMFracCJl1/F");
  //tree_mm->Branch("NEMFracCJl2", &NEMFracCJl2, "NEMFracCJl2/F");   tree_ee->Branch("NEMFracCJl2", &NEMFracCJl2, "NEMFracCJl2/F");   tree_em->Branch("NEMFracCJl2", &NEMFracCJl2, "NEMFracCJl2/F");
  //tree_mm->Branch("CHFracCJl1", &CHFracCJl1, "CHFracCJl1/F");      tree_ee->Branch("CHFracCJl1", &CHFracCJl1, "CHFracCJl1/F");      tree_em->Branch("CHFracCJl1", &CHFracCJl1, "CHFracCJl1/F");
  //tree_mm->Branch("CHFracCJl2", &CHFracCJl2, "CHFracCJl2/F");      tree_ee->Branch("CHFracCJl2", &CHFracCJl2, "CHFracCJl2/F");      tree_em->Branch("CHFracCJl2", &CHFracCJl2, "CHFracCJl2/F");
  //tree_mm->Branch("NHFracCJl1", &NHFracCJl1, "NHFracCJl1/F");      tree_ee->Branch("NHFracCJl1", &NHFracCJl1, "NHFracCJl1/F");      tree_em->Branch("NHFracCJl1", &NHFracCJl1, "NHFracCJl1/F");
  //tree_mm->Branch("NHFracCJl2", &NHFracCJl2, "NHFracCJl2/F");      tree_ee->Branch("NHFracCJl2", &NHFracCJl2, "NHFracCJl2/F");      tree_em->Branch("NHFracCJl2", &NHFracCJl2, "NHFracCJl2/F");
  //tree_mm->Branch("MuFracCJl1", &MuFracCJl1, "MuFracCJl1/F");      tree_ee->Branch("MuFracCJl1", &MuFracCJl1, "MuFracCJl1/F");      tree_em->Branch("MuFracCJl1", &MuFracCJl1, "MuFracCJl1/F");
  //tree_mm->Branch("MuFracCJl2", &MuFracCJl2, "MuFracCJl2/F");      tree_ee->Branch("MuFracCJl2", &MuFracCJl2, "MuFracCJl2/F");      tree_em->Branch("MuFracCJl2", &MuFracCJl2, "MuFracCJl2/F");
  //tree_mm->Branch("JetDiscCJl1", &JetDiscCJl1, "JetDiscCJl1/F");   tree_ee->Branch("JetDiscCJl1", &JetDiscCJl1, "JetDiscCJl1/F");   tree_em->Branch("JetDiscCJl1", &JetDiscCJl1, "JetDiscCJl1/F");
  //tree_mm->Branch("JetDiscCJl2", &JetDiscCJl2, "JetDiscCJl2/F");   tree_ee->Branch("JetDiscCJl2", &JetDiscCJl2, "JetDiscCJl2/F");   tree_em->Branch("JetDiscCJl2", &JetDiscCJl2, "JetDiscCJl2/F");

  tree_mm->Branch("dEtall", &dEtall, "dEtall/F");                  tree_ee->Branch("dEtall", &dEtall, "dEtall/F");                  tree_em->Branch("dEtall", &dEtall, "dEtall/F");
  tree_mm->Branch("dRll", &dRll, "dRll/F");                        tree_ee->Branch("dRll", &dRll, "dRll/F");                        tree_em->Branch("dRll", &dRll, "dRll/F");
  tree_mm->Branch("dRjj12", &dRjj12, "dRjj12/F");                  tree_ee->Branch("dRjj12", &dRjj12, "dRjj12/F");                  tree_em->Branch("dRjj12", &dRjj12, "dRjj12/F");
  tree_mm->Branch("dRjj23", &dRjj23, "dRjj23/F");                  tree_ee->Branch("dRjj23", &dRjj23, "dRjj23/F");                  tree_em->Branch("dRjj23", &dRjj23, "dRjj23/F");
  tree_mm->Branch("dRjj13", &dRjj13, "dRjj13/F");                  tree_ee->Branch("dRjj13", &dRjj13, "dRjj13/F");                  tree_em->Branch("dRjj13", &dRjj13, "dRjj13/F");
  tree_mm->Branch("dRlj11", &dRlj11, "dRlj11/F");                  tree_ee->Branch("dRlj11", &dRlj11, "dRlj11/F");                  tree_em->Branch("dRlj11", &dRlj11, "dRlj11/F");
  tree_mm->Branch("dRlj12", &dRlj12, "dRlj12/F");                  tree_ee->Branch("dRlj12", &dRlj12, "dRlj12/F");                  tree_em->Branch("dRlj12", &dRlj12, "dRlj12/F");
  tree_mm->Branch("dRlj13", &dRlj13, "dRlj13/F");                  tree_ee->Branch("dRlj13", &dRlj13, "dRlj13/F");                  tree_em->Branch("dRlj13", &dRlj13, "dRlj13/F");
  tree_mm->Branch("dRlj21", &dRlj21, "dRlj21/F");                  tree_ee->Branch("dRlj21", &dRlj21, "dRlj21/F");                  tree_em->Branch("dRlj21", &dRlj21, "dRlj21/F");
  tree_mm->Branch("dRlj22", &dRlj22, "dRlj22/F");                  tree_ee->Branch("dRlj22", &dRlj22, "dRlj22/F");                  tree_em->Branch("dRlj22", &dRlj22, "dRlj22/F");
  tree_mm->Branch("dRlj23", &dRlj23, "dRlj23/F");                  tree_ee->Branch("dRlj23", &dRlj23, "dRlj23/F");                  tree_em->Branch("dRlj23", &dRlj23, "dRlj23/F");  
  //tree_mm->Branch("dRlv1", &dRlv1, "dRlv1/F");                     tree_ee->Branch("dRlv1", &dRlv1, "dRlv1/F");                     tree_em->Branch("dRlv1", &dRlv1, "dRlv1/F");
  //tree_mm->Branch("dRlv2", &dRlv2, "dRlv2/F");                     tree_ee->Branch("dRlv2", &dRlv2, "dRlv2/F");                     tree_em->Branch("dRlv2", &dRlv2, "dRlv2/F");
  
  tree_mm->Branch("MSSSF", &MSSSF, "MSSSF/F");                     tree_ee->Branch("MSSSF", &MSSSF, "MSSSF/F");                     tree_em->Branch("MSSSF", &MSSSF, "MSSSF/F");
  tree_mm->Branch("Mlj11", &Mlj11, "Mlj11/F");                     tree_ee->Branch("Mlj11", &Mlj11, "Mlj11/F");                     tree_em->Branch("Mlj11", &Mlj11, "Mlj11/F");
  tree_mm->Branch("Mlj12", &Mlj12, "Mlj12/F");                     tree_ee->Branch("Mlj12", &Mlj12, "Mlj12/F");                     tree_em->Branch("Mlj12", &Mlj12, "Mlj12/F");
  tree_mm->Branch("Mlj13", &Mlj13, "Mlj13/F");                     tree_ee->Branch("Mlj13", &Mlj13, "Mlj13/F");                     tree_em->Branch("Mlj13", &Mlj13, "Mlj13/F");
  tree_mm->Branch("Mlj21", &Mlj21, "Mlj21/F");                     tree_ee->Branch("Mlj21", &Mlj21, "Mlj21/F");                     tree_em->Branch("Mlj21", &Mlj21, "Mlj21/F");
  tree_mm->Branch("Mlj22", &Mlj22, "Mlj22/F");                     tree_ee->Branch("Mlj22", &Mlj22, "Mlj22/F");                     tree_em->Branch("Mlj22", &Mlj22, "Mlj22/F");
  tree_mm->Branch("Mlj23", &Mlj23, "Mlj23/F");                     tree_ee->Branch("Mlj23", &Mlj23, "Mlj23/F");                     tree_em->Branch("Mlj23", &Mlj23, "Mlj23/F");
  tree_mm->Branch("MTvl1", &MTvl1, "MTvl1/F");                     tree_ee->Branch("MTvl1", &MTvl1, "MTvl1/F");                     tree_em->Branch("MTvl1", &MTvl1, "MTvl1/F");
  tree_mm->Branch("MTvl2", &MTvl2, "MTvl2/F");                     tree_ee->Branch("MTvl2", &MTvl2, "MTvl2/F");                     tree_em->Branch("MTvl2", &MTvl2, "MTvl2/F");
  tree_mm->Branch("Mllj1", &Mllj1, "Mllj1/F");                     tree_ee->Branch("Mllj1", &Mllj1, "Mllj1/F");                     tree_em->Branch("Mllj1", &Mllj1, "Mllj1/F");
  tree_mm->Branch("Mllj2", &Mllj2, "Mllj2/F");                     tree_ee->Branch("Mllj2", &Mllj2, "Mllj2/F");                     tree_em->Branch("Mllj2", &Mllj2, "Mllj2/F");
  tree_mm->Branch("Mllj3", &Mllj3, "Mllj3/F");                     tree_ee->Branch("Mllj3", &Mllj3, "Mllj3/F");                     tree_em->Branch("Mllj3", &Mllj3, "Mllj3/F");
  tree_mm->Branch("Mllj4", &Mllj4, "Mllj4/F");                     tree_ee->Branch("Mllj4", &Mllj4, "Mllj4/F");                     tree_em->Branch("Mllj4", &Mllj4, "Mllj4/F");
  tree_mm->Branch("Mlljj12", &Mlljj12, "Mlljj12/F");               tree_ee->Branch("Mlljj12", &Mlljj12, "Mlljj12/F");               tree_em->Branch("Mlljj12", &Mlljj12, "Mlljj12/F");
  tree_mm->Branch("Mlljj13", &Mlljj13, "Mlljj13/F");               tree_ee->Branch("Mlljj13", &Mlljj13, "Mlljj13/F");               tree_em->Branch("Mlljj13", &Mlljj13, "Mlljj13/F");
  tree_mm->Branch("Mlljj14", &Mlljj14, "Mlljj14/F");               tree_ee->Branch("Mlljj14", &Mlljj14, "Mlljj14/F");               tree_em->Branch("Mlljj14", &Mlljj14, "Mlljj14/F");
  tree_mm->Branch("Mlljj23", &Mlljj23, "Mlljj23/F");               tree_ee->Branch("Mlljj23", &Mlljj23, "Mlljj23/F");               tree_em->Branch("Mlljj23", &Mlljj23, "Mlljj23/F");
  tree_mm->Branch("Mlljj24", &Mlljj24, "Mlljj24/F");               tree_ee->Branch("Mlljj24", &Mlljj24, "Mlljj24/F");               tree_em->Branch("Mlljj24", &Mlljj24, "Mlljj24/F");
  tree_mm->Branch("Mlljj34", &Mlljj34, "Mlljj34/F");               tree_ee->Branch("Mlljj34", &Mlljj34, "Mlljj34/F");               tree_em->Branch("Mlljj34", &Mlljj34, "Mlljj34/F");
  tree_mm->Branch("Mljj112", &Mljj112, "Mljj112/F");               tree_ee->Branch("Mljj112", &Mljj112, "Mljj112/F");               tree_em->Branch("Mljj112", &Mljj112, "Mljj112/F");
  tree_mm->Branch("Mljj113", &Mljj113, "Mljj113/F");               tree_ee->Branch("Mljj113", &Mljj113, "Mljj113/F");               tree_em->Branch("Mljj113", &Mljj113, "Mljj113/F");
  tree_mm->Branch("Mljj114", &Mljj114, "Mljj114/F");               tree_ee->Branch("Mljj114", &Mljj114, "Mljj114/F");               tree_em->Branch("Mljj114", &Mljj114, "Mljj114/F");
  tree_mm->Branch("Mljj123", &Mljj123, "Mljj123/F");               tree_ee->Branch("Mljj123", &Mljj123, "Mljj123/F");               tree_em->Branch("Mljj123", &Mljj123, "Mljj123/F");
  tree_mm->Branch("Mljj124", &Mljj124, "Mljj124/F");               tree_ee->Branch("Mljj124", &Mljj124, "Mljj124/F");               tree_em->Branch("Mljj124", &Mljj124, "Mljj124/F");
  tree_mm->Branch("Mljj134", &Mljj134, "Mljj134/F");               tree_ee->Branch("Mljj134", &Mljj134, "Mljj134/F");               tree_em->Branch("Mljj134", &Mljj134, "Mljj134/F");
  tree_mm->Branch("Mljj212", &Mljj212, "Mljj212/F");               tree_ee->Branch("Mljj212", &Mljj212, "Mljj212/F");               tree_em->Branch("Mljj212", &Mljj212, "Mljj212/F");
  tree_mm->Branch("Mljj213", &Mljj213, "Mljj213/F");               tree_ee->Branch("Mljj213", &Mljj213, "Mljj213/F");               tree_em->Branch("Mljj213", &Mljj213, "Mljj213/F");
  tree_mm->Branch("Mljj214", &Mljj214, "Mljj214/F");               tree_ee->Branch("Mljj214", &Mljj214, "Mljj214/F");               tree_em->Branch("Mljj214", &Mljj214, "Mljj214/F");
  tree_mm->Branch("Mljj223", &Mljj223, "Mljj223/F");               tree_ee->Branch("Mljj223", &Mljj223, "Mljj223/F");               tree_em->Branch("Mljj223", &Mljj223, "Mljj223/F");
  tree_mm->Branch("Mljj224", &Mljj224, "Mljj224/F");               tree_ee->Branch("Mljj224", &Mljj224, "Mljj224/F");               tree_em->Branch("Mljj224", &Mljj224, "Mljj224/F");
  tree_mm->Branch("Mljj234", &Mljj234, "Mljj234/F");               tree_ee->Branch("Mljj234", &Mljj234, "Mljj234/F");               tree_em->Branch("Mljj234", &Mljj234, "Mljj234/F");
  tree_mm->Branch("Mjj12", &Mjj12, "Mjj12/F");                     tree_ee->Branch("Mjj12", &Mjj12, "Mjj12/F");                     tree_em->Branch("Mjj12", &Mjj12, "Mjj12/F");
  tree_mm->Branch("Mjj13", &Mjj13, "Mjj13/F");                     tree_ee->Branch("Mjj13", &Mjj13, "Mjj13/F");                     tree_em->Branch("Mjj13", &Mjj13, "Mjj13/F");
  tree_mm->Branch("Mjj14", &Mjj14, "Mjj14/F");                     tree_ee->Branch("Mjj14", &Mjj14, "Mjj14/F");                     tree_em->Branch("Mjj14", &Mjj14, "Mjj14/F");
  tree_mm->Branch("Mjj23", &Mjj23, "Mjj23/F");                     tree_ee->Branch("Mjj23", &Mjj23, "Mjj23/F");                     tree_em->Branch("Mjj23", &Mjj23, "Mjj23/F");
  tree_mm->Branch("Mjj24", &Mjj24, "Mjj24/F");                     tree_ee->Branch("Mjj24", &Mjj24, "Mjj24/F");                     tree_em->Branch("Mjj24", &Mjj24, "Mjj24/F");
  tree_mm->Branch("Mjj34", &Mjj34, "Mjj34/F");                     tree_ee->Branch("Mjj34", &Mjj34, "Mjj34/F");                     tree_em->Branch("Mjj34", &Mjj34, "Mjj34/F");

  tree_mm->Branch("PtWj1", &PtWj1, "PtWj1/F");                     tree_ee->Branch("PtWj1", &PtWj1, "PtWj1/F");                     tree_em->Branch("PtWj1", &PtWj1, "PtWj1/F");
  tree_mm->Branch("PtWj2", &PtWj2, "PtWj2/F");                     tree_ee->Branch("PtWj2", &PtWj2, "PtWj2/F");                     tree_em->Branch("PtWj2", &PtWj2, "PtWj2/F");
  tree_mm->Branch("dRWjj", &dRWjj, "dRWjj/F");                     tree_ee->Branch("dRWjj", &dRWjj, "dRWjj/F");                     tree_em->Branch("dRWjj", &dRWjj, "dRWjj/F");
  tree_mm->Branch("dRlW12", &dRlW12, "dRlW12/F");                  tree_ee->Branch("dRlW12", &dRlW12, "dRlW12/F");                  tree_em->Branch("dRlW12", &dRlW12, "dRlW12/F");
  tree_mm->Branch("dRlW22", &dRlW22, "dRlW22/F");                  tree_ee->Branch("dRlW22", &dRlW22, "dRlW22/F");                  tree_em->Branch("dRlW22", &dRlW22, "dRlW22/F");
  tree_mm->Branch("dRlN12", &dRlN12, "dRlN12/F");                  tree_ee->Branch("dRlN12", &dRlN12, "dRlN12/F");                  tree_em->Branch("dRlN12", &dRlN12, "dRlN12/F");
  tree_mm->Branch("dRlN21", &dRlN21, "dRlN21/F");                  tree_ee->Branch("dRlN21", &dRlN21, "dRlN21/F");                  tree_em->Branch("dRlN21", &dRlN21, "dRlN21/F");
  tree_mm->Branch("M_W2_jj", &M_W2_jj, "M_W2_jj/F");               tree_ee->Branch("M_W2_jj", &M_W2_jj, "M_W2_jj/F");               tree_em->Branch("M_W2_jj", &M_W2_jj, "M_W2_jj/F");
  tree_mm->Branch("M_W1_lljj", &M_W1_lljj, "M_W1_lljj/F");         tree_ee->Branch("M_W1_lljj", &M_W1_lljj, "M_W1_lljj/F");         tree_em->Branch("M_W1_lljj", &M_W1_lljj, "M_W1_lljj/F");
  tree_mm->Branch("M_N1_l1jj", &M_N1_l1jj, "M_N1_l1jj/F");         tree_ee->Branch("M_N1_l1jj", &M_N1_l1jj, "M_N1_l1jj/F");         tree_em->Branch("M_N1_l1jj", &M_N1_l1jj, "M_N1_l1jj/F");
  tree_mm->Branch("M_N2_l2jj", &M_N2_l2jj, "M_N2_l2jj/F");         tree_ee->Branch("M_N2_l2jj", &M_N2_l2jj, "M_N2_l2jj/F");         tree_em->Branch("M_N2_l2jj", &M_N2_l2jj, "M_N2_l2jj/F");

  tree_mm->Branch("w_tot", &w_tot, "w_tot/F");                     tree_ee->Branch("w_tot", &w_tot, "w_tot/F");                     tree_em->Branch("w_tot", &w_tot, "w_tot/F");

  outfile->cd();

}


void HNL_SR3_BDT_KinVar::executeEvent(){
  
  AnalyzerParameter param_bdt = HNL_LeptonCore::InitialiseHNLParameter("BDT");

  Event ev = GetEvent();
  double weight = SetupWeight(ev, param_bdt);
  
  //FillHist("CutFlow", 0., weight, 20, 0., 20.);
  FillHist("CutFlow_MuMu", 0.5, 1., 10, 0., 10.);
  FillHist("CutFlow_EE", 0.5, 1., 10, 0., 10.);
  FillHist("CutFlow_EMu", 0.5, 1., 10, 0., 10.);

  if(!PassMETFilter()) return;

  FillHist("CutFlow_MuMu", 1.5, 1., 10, 0., 10.);
  FillHist("CutFlow_EE", 1.5, 1., 10, 0., 10.);
  FillHist("CutFlow_EMu", 1.5, 1., 10, 0., 10.);

  vector<HNL_LeptonCore::Channel> channels = {EE, MuMu, EMu};

  std::vector<Muon>       MuonCollT     = GetLepCollByRunType    ( GetMuons    (param_bdt.Muon_Tight_ID, 10., 2.4)    , param_bdt, "");
  std::vector<Electron>   ElectronCollT = GetLepCollByRunType    ( GetElectrons(param_bdt.Electron_Tight_ID, 10., 2.5), param_bdt, "");

  std::vector<Electron>   ElectronCollV = GetElectrons(param_bdt.Electron_Veto_ID, 10., 2.5);
  std::vector<Muon>       MuonCollV     = GetMuons    (param_bdt.Muon_Veto_ID, 5., 2.4);

  std::vector<Lepton *> LepsT  = MakeLeptonPointerVector(MuonCollT,ElectronCollT);
  std::vector<Lepton *> LepsV  = MakeLeptonPointerVector(MuonCollV,ElectronCollV);

  if(!PassHEMVeto(LepsV,weight)) return;

  FillHist("CutFlow_MuMu", 2.5, 1., 10, 0., 10.);
  FillHist("CutFlow_EE", 2.5, 1., 10, 0., 10.);
  FillHist("CutFlow_EMu", 2.5, 1., 10, 0., 10.);

  if(!(LepsT.size() == 2)) return;

  if(!(LepsV.size() == 2)) return;

  FillHist("CutFlow_MuMu", 3.5, 1., 10, 0., 10.);
  FillHist("CutFlow_EE", 3.5, 1., 10, 0., 10.);
  FillHist("CutFlow_EMu", 3.5, 1., 10, 0., 10.);

  for(auto dilep_channel : channels){

    //std::vector<Muon>       MuonCollT     = GetLepCollByRunType    ( GetMuons    ( param_bdt,param_bdt.Muon_Tight_ID, 10., 2.4, RunFake)    , param_bdt, "NoSel");
    //std::vector<Electron>   ElectronCollT = GetLepCollByRunType    ( GetElectrons( param_bdt,param_bdt.Electron_Tight_ID, 10., 2.5, RunFake), param_bdt, "NoSel");
    
    //std::vector<Electron>   ElectronCollV = GetElectrons(param_bdt.Electron_Veto_ID, 10., 2.5);
    //std::vector<Muon>       MuonCollV     = GetMuons    (param_bdt.Muon_Veto_ID, 5., 2.4);
    
    //std::vector<Lepton *> LepsT  = MakeLeptonPointerVector(MuonCollT,ElectronCollT);
    //std::vector<Lepton *> LepsV  = MakeLeptonPointerVector(MuonCollV,ElectronCollV);

    if(!CheckLeptonFlavourForChannel(dilep_channel, LepsT)) continue;

    if(dilep_channel == MuMu) FillHist("CutFlow_MuMu", 4.5, 1., 10, 0., 10.);
    if(dilep_channel == EE) FillHist("CutFlow_EE", 4.5, 1., 10, 0., 10.);
    if(dilep_channel == EMu) FillHist("CutFlow_EMu", 4.5, 1., 10, 0., 10.);

    if(!PassGenMatchFilter(LepsT, param_bdt)) continue;

    if(!ConversionSplitting(LepsT, RunConv, 2, param_bdt)) continue;

    if(!PassTriggerSelection(dilep_channel, ev, LepsT, "Dilep")) continue;

    if(dilep_channel == MuMu){

      if(DataEra=="2016postVFP"){

        if(!ev.PassTrigger("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v")){
          if(!ev.PassTrigger("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v")) {
            if(ev.PassTrigger("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v")) weight = weight*ev.GetTriggerLumi("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v")/ev.GetTriggerLumi("Full");
          }
        }

      }
      if(DataEra=="2017"){

        //if(!ev.PassTrigger("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v")){
        //  if(ev.PassTrigger("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v")) weight = weight*4803.366325775/ev.GetTriggerLumi("Full");
        //}
        if(!ev.PassTrigger("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v")){
           if(ev.PassTrigger("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v")) weight = weight*ev.GetTriggerLumi("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v")/ev.GetTriggerLumi("Full");
        }

      }

    }

    if(dilep_channel == MuMu) FillHist("CutFlow_MuMu", 5.5, 1., 10, 0., 10.);
    if(dilep_channel == EE) FillHist("CutFlow_EE", 5.5, 1., 10, 0., 10.);
    if(dilep_channel == EMu) FillHist("CutFlow_EMu", 5.5, 1., 10, 0., 10.);

    //std::vector<Tau>    mytaus         = GetTaus("HNVeto",20., 2.3);
 
    std::vector<FatJet> fatjets_tmp    = GetFatJets("tight", 200., 5);
    std::vector<Jet>    jets_tmp       = GetJets("tight", 15., 5);

    //std::vector<Jet>    AK4_JetAllColl = GetJets("NoID", 10., 3.0);
    //std::vector<FatJet> AK8_JetColl    = SelectAK8Jets(fatjets_tmp, 200., 2.7, true, 1., false, -999, false, 40., 130., ElectronCollV, MuonCollV);
    //std::vector<Jet>    AK4_JetColl    = SelectAK4Jets(jets_tmp,     20., 2.7, true, 0.4, 0.8, "", ElectronCollV, MuonCollV, AK8_JetColl);
    //std::vector<Jet>    AK4_JetVBFColl = SelectAK4Jets(jets_tmp,     30., 4.7, true, 0.4, 0.8, "", ElectronCollV, MuonCollV, AK8_JetColl);

    std::vector<FatJet> AK8_JetColl    = SelectAK8Jets(fatjets_tmp,   200., 2.7, true, 1., false, -999, true, 40., 130., "particleNet_WvsQCD", ElectronCollV, MuonCollV);
    std::vector<Jet>    AK4_JetColl    = SelectAK4Jets(jets_tmp,      20., 2.7, true, 0.4, 0.8, "Loose",  ElectronCollV, MuonCollV, AK8_JetColl);
    std::vector<Jet>    AK4_JetVBFColl = SelectAK4Jets(jets_tmp,      30., 4.7, true, 0.4, 0.8, "Loose",  ElectronCollV, MuonCollV, AK8_JetColl);
    std::vector<Jet>    AK4_JetAllColl = SelectAK4Jets(jets_tmp,      20., 2.7, true, 0.4, 0.8, "Loose",  ElectronCollV, MuonCollV, AK8_JetColl);

    //Particle METv = GetMiniAODvMET("PuppiT1xyULCorr");
    Particle METv = GetvMET("PuppiT1xyULCorr", param_bdt); 

    std::vector<Jet> bjets_tmp         = SelectAK4Jets(jets_tmp,      20., 2.4, true, 0.4, 0.8, "", ElectronCollV, MuonCollV, AK8_JetColl);

    JetTagging::Parameters param_jets  = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets);
    std::vector<Jet> BJetColl          = SelectBJets(param_bdt, bjets_tmp, param_jets);
    double sf_btag                     = GetBJetSF(param_bdt, bjets_tmp, param_jets);
    if(!IsData) weight *= sf_btag;

    //==== Removing Z peak events in ee channel
    bool passNonZ = true;
    if(dilep_channel == EE){
      if(fabs(GetLLMass(LepsT)-M_Z) < M_ZWINDOW_VETO) passNonZ = false;
    }

    //==== Merging OS+SS for signal    
    bool passCharge = false;
    if(MCSample.Contains("TypeI")) passCharge = true;
    if(RunFake) passCharge = true;
    else{
      if(SameCharge(LepsT)) passCharge = true;
    }

    //==== Merging multiple datasets for signal and DY Z
    double weightCharge = 1., weightDYZ = 1.;

    if(RunFake || MCSample.Contains("DYTypeI")) weightCharge = 0.5;
    weight *= weightCharge;

    if(MCSample.Contains("DYJets")){
      if(RunConv) weightDYZ = 1.; // DYJets_MG only
      if(RunFake || RunCF){
        if(MCSample.Contains("MiNNLO")) weightDYZ = 1.;
        else weightDYZ = 0.5; // DYJets_MG, DYJets
      }
    }
    weight *= weightDYZ;

    //==== Applying CF SF
    double avgSF = 1.;

    if(RunCF){
      avgSF = (GetCFSF(param_bdt, LepsT[0], true) + GetCFSF(param_bdt, LepsT[1], true))*0.5;
    }

    weight *= avgSF;

    //==== Applying Conv SF

    //==== Event selection : SR3
    bool EventCand = false;

    if(passCharge){

      if(dilep_channel == MuMu) FillHist("CutFlow_MuMu", 6.5, 1., 10, 0., 10.);
      if(dilep_channel == EE) FillHist("CutFlow_EE", 6.5, 1., 10, 0., 10.);
      if(dilep_channel == EMu) FillHist("CutFlow_EMu", 6.5, 1., 10, 0., 10.);

      if(GetLLMass(LepsT) > M_CUT_LL){
        if(passNonZ){

          if(dilep_channel == MuMu) FillHist("CutFlow_MuMu", 7.5, 1., 10, 0., 10.);
          if(dilep_channel == EE) FillHist("CutFlow_EE", 7.5, 1., 10, 0., 10.);
          if(dilep_channel == EMu) FillHist("CutFlow_EMu", 7.5, 1., 10, 0., 10.);

          if(AK8_JetColl.size() == 0){

            if(dilep_channel == MuMu) FillHist("CutFlow_MuMu", 8.5, 1., 10, 0., 10.);
            if(dilep_channel == EE) FillHist("CutFlow_EE", 8.5, 1., 10, 0., 10.);
            if(dilep_channel == EMu) FillHist("CutFlow_EMu", 8.5, 1., 10, 0., 10.);

            //if(!PassVBFInitial(AK4_JetVBFColl)){
            if(!PassVBF(AK4_JetVBFColl, LepsT, 750., true)){

              if(dilep_channel == MuMu) FillHist("CutFlow_MuMu", 9.5, 1., 10, 0., 10.);
              if(dilep_channel == EE) FillHist("CutFlow_EE", 9.5, 1., 10, 0., 10.);
              if(dilep_channel == EMu) FillHist("CutFlow_EMu", 9.5, 1., 10, 0., 10.);

              EventCand=true;

	    }
          }
	}
      }
    }

    //if(!EventCand) return;

    if(EventCand) MakeTreeSS2L(dilep_channel, LepsT, AK4_JetAllColl, AK4_JetColl, AK4_JetVBFColl, BJetColl, METv, weight, "");

  }

}

void HNL_SR3_BDT_KinVar::MakeTreeSS2L(HNL_LeptonCore::Channel lep_channel,vector<Lepton *>  LepTColl, 
                                 vector<Jet>& JetAllColl,
				 vector<Jet>& JetColl, 
				 vector<Jet>& JetVBFColl,
                                 vector<Jet>& JetBTagColl, 
				 Particle& vMET, float weight, TString Label)
{
  
  
  //if(!SameCharge(LepTColl)) return;
  
  // CorrectChannelStream checks EE channel has only EE events
  //if(!CheckLeptonFlavourForChannel(lep_channel, LepTColl)) return;
  //if(CheckLeptonFlavourForChannel(lep_channel, LepTColl)){

  // Pt cut is set in CheckLeptonFlavourForChannel
  //if(!(LepTColl[0]->Pt()>20 && LepTColl[1]->Pt()>10)) return;

  //float Mll = GetLLMass(LepTColl);
  //if(lep_channel==EE && (fabs(Mll-M_Z)<M_ZWINDOW_VETO)) return;

  //if(lep_channel == MuMu) FillHist("CutFlow_MuMu", 7.5, 1., 10, 0., 10.);
  //if(lep_channel == EE) FillHist("CutFlow_EE", 7.5, 1., 10, 0., 10.);
  //if(lep_channel == EMu) FillHist("CutFlow_EMu", 7.5, 1., 10, 0., 10.);

  InitializeTreeVars();
  
  Nj      = JetColl.size();
  Nvbfj   = JetVBFColl.size();
  Nb      = JetBTagColl.size();

  LQ      = LepTColl[0]->Charge();
  Ptl1    = LepTColl[0]->Pt();
  Ptl2    = LepTColl.at(1)->Pt();
  LT      = GetLT(LepTColl);
  Ptj1    = JetColl.size()<1? -1.:JetColl.at(0).Pt();
  Ptj2    = JetColl.size()<2? -1.:JetColl.at(1).Pt();
  Ptj3    = JetColl.size()<3? -1.:JetColl.at(2).Pt();
  MET     = vMET.Pt();
  Etal1   = fabs(LepTColl.at(0)->Eta());
  Etal2   = fabs(LepTColl.at(1)->Eta());

 
  dEtall  = abs(LepTColl.at(0)->Eta()-LepTColl.at(1)->Eta());
  dRll    = LepTColl.at(0)->DeltaR(*LepTColl.at(1));
  dRjj12  = JetColl.size()<2? -1.:JetColl.at(0).DeltaR(JetColl.at(1));
  dRjj23  = JetColl.size()<3? -1.:JetColl.at(1).DeltaR(JetColl.at(2));
  dRjj13  = JetColl.size()<3? -1.:JetColl.at(0).DeltaR(JetColl.at(2));
  dRlj11  = JetColl.size()<1? -1.:LepTColl.at(0)->DeltaR(JetColl.at(0));
  dRlj12  = JetColl.size()<2? -1.:LepTColl.at(0)->DeltaR(JetColl.at(1));
  dRlj13  = JetColl.size()<3? -1.:LepTColl.at(0)->DeltaR(JetColl.at(2));
  dRlj21  = JetColl.size()<1? -1.:LepTColl.at(1)->DeltaR(JetColl.at(0));
  dRlj22  = JetColl.size()<2? -1.:LepTColl.at(1)->DeltaR(JetColl.at(1));
  dRlj23  = JetColl.size()<3? -1.:LepTColl.at(1)->DeltaR(JetColl.at(2));
  
  MSSSF   = GetLLMass(LepTColl);
  Mlj11   = JetColl.size()<1? -1.:(*LepTColl.at(0)+JetColl.at(0)).M();
  Mlj12   = JetColl.size()<2? -1.:(*LepTColl.at(0)+JetColl.at(1)).M();
  Mlj13   = JetColl.size()<3? -1.:(*LepTColl.at(0)+JetColl.at(2)).M();
  Mlj21   = JetColl.size()<1? -1.:(*LepTColl.at(1)+JetColl.at(0)).M();
  Mlj22   = JetColl.size()<2? -1.:(*LepTColl.at(1)+JetColl.at(1)).M();
  Mlj23   = JetColl.size()<3? -1.:(*LepTColl.at(1)+JetColl.at(2)).M();
  MTvl1   = MT(*LepTColl.at(0),vMET);
  MTvl2   = MT(*LepTColl.at(1),vMET);
  Mllj1   = JetColl.size()<1? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(0)).M();
  Mllj2   = JetColl.size()<2? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(1)).M();
  Mllj3   = JetColl.size()<3? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(2)).M();
  Mllj4   = JetColl.size()<4? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(3)).M();
  Mlljj12 = JetColl.size()<2? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(0)+JetColl.at(1)).M();
  Mlljj13 = JetColl.size()<3? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(0)+JetColl.at(2)).M();
  Mlljj14 = JetColl.size()<4? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(0)+JetColl.at(3)).M();
  Mlljj23 = JetColl.size()<3? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(1)+JetColl.at(2)).M();
  Mlljj24 = JetColl.size()<4? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(1)+JetColl.at(3)).M();
  Mlljj34 = JetColl.size()<4? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(2)+JetColl.at(3)).M();
  Mljj112 = JetColl.size()<2? -1.:(*LepTColl.at(0)+JetColl.at(0)+JetColl.at(1)).M();
  Mljj113 = JetColl.size()<3? -1.:(*LepTColl.at(0)+JetColl.at(0)+JetColl.at(2)).M();
  Mljj114 = JetColl.size()<4? -1.:(*LepTColl.at(0)+JetColl.at(0)+JetColl.at(3)).M();
  //return;
  Mljj123 = JetColl.size()<3? -1.:(*LepTColl.at(0)+JetColl.at(1)+JetColl.at(2)).M();
  Mljj124 = JetColl.size()<4? -1.:(*LepTColl.at(0)+JetColl.at(1)+JetColl.at(3)).M();
  Mljj134 = JetColl.size()<4? -1.:(*LepTColl.at(0)+JetColl.at(2)+JetColl.at(3)).M();
  Mljj212 = JetColl.size()<2? -1.:(*LepTColl.at(1)+JetColl.at(0)+JetColl.at(1)).M();
  Mljj213 = JetColl.size()<3? -1.:(*LepTColl.at(1)+JetColl.at(0)+JetColl.at(2)).M();
  Mljj214 = JetColl.size()<4? -1.:(*LepTColl.at(1)+JetColl.at(0)+JetColl.at(3)).M();
  Mljj223 = JetColl.size()<3? -1.:(*LepTColl.at(1)+JetColl.at(1)+JetColl.at(2)).M();
  Mljj224 = JetColl.size()<4? -1.:(*LepTColl.at(1)+JetColl.at(1)+JetColl.at(3)).M();
  Mljj234 = JetColl.size()<4? -1.:(*LepTColl.at(1)+JetColl.at(2)+JetColl.at(3)).M();
  Mjj12   = JetColl.size()<2? -1.:(JetColl.at(0)+JetColl.at(1)).M();
  Mjj13   = JetColl.size()<3? -1.:(JetColl.at(0)+JetColl.at(2)).M();
  Mjj14   = JetColl.size()<4? -1.:(JetColl.at(0)+JetColl.at(3)).M();
  Mjj23   = JetColl.size()<3? -1.:(JetColl.at(1)+JetColl.at(2)).M();
  Mjj24   = JetColl.size()<4? -1.:(JetColl.at(1)+JetColl.at(3)).M();
  Mjj34   = JetColl.size()<4? -1.:(JetColl.at(2)+JetColl.at(3)).M();

  //==== Vars for non-prompt lepton bkg
  //int IdxMatchJetl1=-1, IdxMatchAwayJetl1=-1, IdxMatchJetl2=-1, IdxMatchAwayJetl2=-1;
  //float mindR1=999., maxdPhi1=-999., mindR2=999., maxdPhi2=-999.;
  int IdxMatchAwayJetl1=-1, IdxMatchAwayJetl2=-1;
  float maxdPhi1=-999., maxdPhi2=-999.;

  for(unsigned int ij=0; ij<JetAllColl.size(); ij++){

    float dPhi1 = fabs(TVector2::Phi_mpi_pi(LepTColl.at(0)->Phi() - JetAllColl.at(ij).Phi()));
    if(dPhi1>maxdPhi1){ maxdPhi1=dPhi1; IdxMatchAwayJetl1=ij; }
    //float dR1=LepTColl.at(0)->DeltaR(JetAllColl.at(ij));
    //if(dR1>0.4) continue;
    //if(dR1<mindR1){ mindR1=dR1; IdxMatchJetl1=ij; }

  }

  for(unsigned int ij=0; ij<JetAllColl.size(); ij++){

    float dPhi2 = fabs(TVector2::Phi_mpi_pi(LepTColl.at(1)->Phi() - JetAllColl.at(ij).Phi()));
    if(dPhi2>maxdPhi2){ maxdPhi2=dPhi2; IdxMatchAwayJetl2=ij; }
    //float dR2=LepTColl.at(1)->DeltaR(JetAllColl.at(ij));
    //if(dR2>0.4) continue;
    //if(dR2<mindR2){ mindR2=dR2; IdxMatchJetl2=ij; }

  }

  /*if(IdxMatchJetl1!=-1){

    PtRatiol1   = LepTColl.at(0)->Pt()/JetAllColl.at(IdxMatchJetl1).Pt();
    TLorentzVector JetNoLep1(JetAllColl.at(IdxMatchJetl1));
    JetNoLep1  -= *LepTColl.at(0);
    PtRelv0l1   = LepTColl.at(0)->Perp(JetAllColl.at(IdxMatchJetl1).Vect());
    PtRelv1l1   = LepTColl.at(0)->Perp(JetNoLep1.Vect());
    CEMFracCJl1 = JetAllColl.at(IdxMatchJetl1).ChargedEmEnergyFraction();
    NEMFracCJl1 = JetAllColl.at(IdxMatchJetl1).NeutralEmEnergyFraction();
    CHFracCJl1  = JetAllColl.at(IdxMatchJetl1).ChargedHadEnergyFraction();
    NHFracCJl1  = JetAllColl.at(IdxMatchJetl1).NeutralHadEnergyFraction();
    MuFracCJl1  = JetAllColl.at(IdxMatchJetl1).MuonEnergyFraction();
    JetDiscCJl1 = JetAllColl.at(IdxMatchJetl1).GetTaggerResult(JetTagging::DeepJet);

  }
  else{

    PtRatiol1=-1., PtRelv0l1=-1., PtRelv1l1=-1., CEMFracCJl1=-1., NEMFracCJl1=-1., CHFracCJl1=-1., NHFracCJl1=-1., MuFracCJl1=-1., JetDiscCJl1=-1.;

  }

  if(IdxMatchJetl2!=-1){

    PtRatiol2   = LepTColl.at(1)->Pt()/JetAllColl.at(IdxMatchJetl2).Pt();
    TLorentzVector JetNoLep2(JetAllColl.at(IdxMatchJetl2));
    JetNoLep2  -= *LepTColl.at(1);
    PtRelv0l2   = LepTColl.at(1)->Perp(JetAllColl.at(IdxMatchJetl2).Vect());
    PtRelv1l2   = LepTColl.at(1)->Perp(JetNoLep2.Vect());
    CEMFracCJl2 = JetAllColl.at(IdxMatchJetl2).ChargedEmEnergyFraction();
    NEMFracCJl2 = JetAllColl.at(IdxMatchJetl2).NeutralEmEnergyFraction();
    CHFracCJl2  = JetAllColl.at(IdxMatchJetl2).ChargedHadEnergyFraction();
    NHFracCJl2  = JetAllColl.at(IdxMatchJetl2).NeutralHadEnergyFraction();
    MuFracCJl2  = JetAllColl.at(IdxMatchJetl2).MuonEnergyFraction();
    JetDiscCJl2 = JetAllColl.at(IdxMatchJetl2).GetTaggerResult(JetTagging::DeepJet);

  }
  else{

    PtRatiol2=-1., PtRelv0l2=-1., PtRelv1l2=-1., CEMFracCJl2=-1., NEMFracCJl2=-1., CHFracCJl2=-1., NHFracCJl2=-1., MuFracCJl2=-1., JetDiscCJl2=-1.;

  }*/

  if(IdxMatchAwayJetl1!=-1){

    PtRatioAJl1 = JetAllColl.at(IdxMatchAwayJetl1).Pt()/LepTColl.at(0)->Pt();
    CEMFracAJl1 = JetAllColl.at(IdxMatchAwayJetl1).ChargedEmEnergyFraction();
    NEMFracAJl1 = JetAllColl.at(IdxMatchAwayJetl1).NeutralEmEnergyFraction();
    CHFracAJl1  = JetAllColl.at(IdxMatchAwayJetl1).ChargedHadEnergyFraction();
    NHFracAJl1  = JetAllColl.at(IdxMatchAwayJetl1).NeutralHadEnergyFraction();
    MuFracAJl1  = JetAllColl.at(IdxMatchAwayJetl1).MuonEnergyFraction();
    JetDiscAJl1 = JetAllColl.at(IdxMatchAwayJetl1).GetTaggerResult(JetTagging::DeepJet);

  }
  else{

    PtRatioAJl1=-1., CEMFracAJl1=-1., NEMFracAJl1=-1., CHFracAJl1=-1., NHFracAJl1=-1., MuFracAJl1=-1., JetDiscAJl1=-1.;

  }

  if(IdxMatchAwayJetl2!=-1){

    PtRatioAJl2 = JetAllColl.at(IdxMatchAwayJetl2).Pt()/LepTColl.at(1)->Pt();
    CEMFracAJl2 = JetAllColl.at(IdxMatchAwayJetl2).ChargedEmEnergyFraction();
    NEMFracAJl2 = JetAllColl.at(IdxMatchAwayJetl2).NeutralEmEnergyFraction();
    CHFracAJl2  = JetAllColl.at(IdxMatchAwayJetl2).ChargedHadEnergyFraction();
    NHFracAJl2  = JetAllColl.at(IdxMatchAwayJetl2).NeutralHadEnergyFraction();
    MuFracAJl2  = JetAllColl.at(IdxMatchAwayJetl2).MuonEnergyFraction();
    JetDiscAJl2 = JetAllColl.at(IdxMatchAwayJetl2).GetTaggerResult(JetTagging::DeepJet);

  }
  else{

    PtRatioAJl2=-1., CEMFracAJl2=-1., NEMFracAJl2=-1., CHFracAJl2=-1., NHFracAJl2=-1., MuFracAJl2=-1., JetDiscAJl2=-1.;

  }

  //==== Vars requiring complex algo.
  HT      = 0;
  for(unsigned int itj=0; itj<JetColl.size(); itj++){ HT+=JetColl.at(itj).Pt(); }
  
  HTLT=HT/LT;
  HTLT1=HT/LepTColl.at(0)->Pt();
  HTLT2=HT/LepTColl.at(1)->Pt();
  
  std::vector<FatJet> FatJetColl;
  
  double ST = GetST( LepTColl, JetColl, FatJetColl, vMET);
  
  MET2HT = JetColl.size()<1? -1.:pow(MET,2.)/HT;
  MET2ST = pow(MET,2.)/ST;
  
  float dijetmass_tmp=9999.;
  float dijetmass=99990000.;
  int m=-999;
  int n=-999;
  
  for(UInt_t emme=0; emme<JetColl.size(); emme++){
    for(UInt_t enne=emme+1; enne<JetColl.size(); enne++) {
      
      dijetmass_tmp = (JetColl[emme]+JetColl[enne]).M();
      //if(emme == enne) continue;
      
      if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
	dijetmass = dijetmass_tmp;
	m = emme;
	n = enne;
      }
    }
  }

  PtWj1     = JetColl.size() > 1 ? JetColl[m].Pt() : -1.;
  PtWj2     = JetColl.size() > 1 ? JetColl[n].Pt() : -1.;
  dRWjj     = JetColl.size() > 1 ? JetColl[m].DeltaR(JetColl[n]) : -1.;
  dRlW12    = JetColl.size() > 1 ? LepTColl.at(0)->DeltaR(JetColl[m] + JetColl[n]) : -1.;
  dRlW22    = JetColl.size() > 1 ? LepTColl.at(1)->DeltaR(JetColl[m] + JetColl[n]) : -1.;
  dRlN12    = JetColl.size() > 1 ? LepTColl.at(0)->DeltaR(*LepTColl.at(1) + JetColl[m] + JetColl[n]) : -1.;
  dRlN21    = JetColl.size() > 1 ? LepTColl.at(1)->DeltaR(*LepTColl.at(0) + JetColl[m] + JetColl[n]) : -1.;
  M_W2_jj   = JetColl.size() > 1 ? (JetColl[m] + JetColl[n]).M() : -1.;
  M_W1_lljj = JetColl.size() > 1 ? (JetColl[m] + JetColl[n] + *LepTColl.at(0) + *LepTColl.at(1)).M() : -1.;
  M_N1_l1jj = JetColl.size() > 1 ? (JetColl[m] + JetColl[n] + *LepTColl.at(0)).M() : -1.;
  M_N2_l2jj = JetColl.size() > 1 ? (JetColl[m] + JetColl[n] + *LepTColl.at(1)).M() : -1.;
  
  w_tot     = !IsDATA? weight:-1.;
  //if(lep_channel == MuMu) tree_mm->Fill();
  //if(lep_channel == EE)   tree_ee->Fill();
  //if(lep_channel == EMu)  tree_em->Fill();

  if(lep_channel == MuMu){
    tree_mm->Fill();
    FillHist("sumw_MuMu", 0.5, w_tot, 2, 0., 2.);
  }
  if(lep_channel == EE){
    tree_ee->Fill();
    FillHist("sumw_EE", 0.5, w_tot, 2, 0., 2.);
  }
  if(lep_channel == EMu){
    tree_em->Fill();
    FillHist("sumw_EMu", 0.5, w_tot, 2, 0., 2.);
  }
  //}
}
  



void HNL_SR3_BDT_KinVar::executeEventFromParameter(AnalyzerParameter param){

  
}


void HNL_SR3_BDT_KinVar::InitializeTreeVars(){

  Nj=-1, Nvbfj=-1, Nb=-1, LQ=-999;
  Ptl1=-1, Ptl2=-1, Ptj1=-1, Ptj2=-1, Ptj3=-1, MET=-1, HTLT=-1, HTLT1=-1, HTLT2=-1, LT=-1,  HT=-1, MET2HT=-1, MET2ST=-1, Etal1=-1, Etal2=-1;
  PtRatiol1=-1, PtRatiol2=-1, PtRatioAJl1=-1, PtRatioAJl2=-1, PtRelv0l1=-1, PtRelv1l1=-1, PtRelv0l2=-1, PtRelv1l2=-1;
  CEMFracAJl1=-1, CEMFracAJl2=-1, NEMFracAJl1=-1, NEMFracAJl2=-1, CHFracAJl1=-1, CHFracAJl2=-1, NHFracAJl1=-1, NHFracAJl2=-1, MuFracAJl1=-1, MuFracAJl2=-1, JetDiscAJl1=-1, JetDiscAJl2=-1;
  CEMFracCJl1=-1, CEMFracCJl2=-1, NEMFracCJl1=-1, NEMFracCJl2=-1, CHFracCJl1=-1, CHFracCJl2=-1, NHFracCJl1=-1, NHFracCJl2=-1, MuFracCJl1=-1, MuFracCJl2=-1, JetDiscCJl1=-1, JetDiscCJl2=-1;
  dEtall=-1, dRll=-1, dRjj12=-1, dRjj23=-1, dRjj13=-1;
  dRlj11=-1, dRlj12=-1, dRlj13=-1, dRlj21=-1, dRlj22=-1, dRlj23=-1;
  MSSSF=-1, Mlj11=-1, Mlj12=-1, Mlj13=-1, Mlj21=-1, Mlj22=-1, Mlj23=-1;
  MTvl1=-1, MTvl2=-1, Mllj1=-1, Mllj2=-1, Mllj3=-1, Mllj4=-1;
  Mlljj12=-1, Mlljj13=-1, Mlljj14=-1, Mlljj23=-1, Mlljj24=-1, Mlljj34=-1;
  Mljj112=-1, Mljj113=-1, Mljj114=-1, Mljj123=-1, Mljj124=-1, Mljj134=-1;
  Mljj212=-1, Mljj213=-1, Mljj214=-1, Mljj223=-1, Mljj224=-1, Mljj234=-1;
  Mjj12=-1, Mjj13=-1, Mjj14=-1, Mjj23=-1, Mjj24=-1, Mjj34=-1;

  PtWj1=-1, PtWj2=-1;
  dRWjj=-1, dRlW12=-1, dRlW22=-1, dRlN12=-1, dRlN21=-1;
  M_W2_jj=-1, M_W1_lljj=-1, M_N1_l1jj=-1, M_N2_l2jj=-1;

  w_tot=-1;

}



HNL_SR3_BDT_KinVar::HNL_SR3_BDT_KinVar(){

  tree_mm = new TTree("Tree_mm", "Tree_mm");
  tree_ee = new TTree("Tree_ee", "Tree_ee");
  tree_em = new TTree("Tree_em", "Tree_em");

}


HNL_SR3_BDT_KinVar::~HNL_SR3_BDT_KinVar(){

  delete tree_mm;
  delete tree_ee;
  delete tree_em;

}


void HNL_SR3_BDT_KinVar::WriteHist(){

  AnalyzerCore::WriteHist();

  outfile->cd();
  tree_mm->Write();
  tree_ee->Write();
  tree_em->Write();
  outfile->cd();

}
