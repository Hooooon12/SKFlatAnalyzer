#include "HNL_LeptonCore.h"

void HNL_LeptonCore::SetupEventMVAReader(TString version, bool ee, bool mm, bool emu){

  InitializeTreeVars();

  TString AnalyzerPath=std::getenv("SKFlat_WD");
  TString MVAPath = "/data/Run2UltraLegacy_v3/Run2/BDTClassifier/results_xml/HNL_ULID/"+version+"/";
  TString MVAPathV1 = "/data/Run2UltraLegacy_v3/Run2/BDTClassifier/results_xml/HNL_ULID/V1/";
  MNStrList = {"85", "90", "95", "100", "125", "150", "200", "250", "300", "400", "500"};
  NCutList  = {"200"};
  NTreeList = {"850"};

  if(mm){

    MVAReaderMM->AddVariable("Nvbfj", &ev_bdt_Nvbfj);
    MVAReaderMM->AddVariable("Nb", &ev_bdt_Nb);
    MVAReaderMM->AddVariable("Ptl1", &ev_bdt_Ptl1);
    MVAReaderMM->AddVariable("Ptl2", &ev_bdt_Ptl2);
    MVAReaderMM->AddVariable("Ptj1", &ev_bdt_Ptj1);
    MVAReaderMM->AddVariable("MET2ST", &ev_bdt_MET2ST);
    MVAReaderMM->AddVariable("HTLT1", &ev_bdt_HTLT1);
    MVAReaderMM->AddVariable("HTLT2", &ev_bdt_HTLT2);
    MVAReaderMM->AddVariable("Etal1", &ev_bdt_Etal1);
    MVAReaderMM->AddVariable("Etal2", &ev_bdt_Etal2);
    MVAReaderMM->AddVariable("NEMFracAJl2", &ev_bdt_NEMFracAJl2);
    MVAReaderMM->AddVariable("CHFracAJl1", &ev_bdt_CHFracAJl1);
    MVAReaderMM->AddVariable("CHFracAJl2", &ev_bdt_CHFracAJl2);
    MVAReaderMM->AddVariable("MuFracAJl1", &ev_bdt_MuFracAJl1);
    MVAReaderMM->AddVariable("MuFracAJl2", &ev_bdt_MuFracAJl2);
    MVAReaderMM->AddVariable("JetDiscAJl1", &ev_bdt_JetDiscAJl1);
    MVAReaderMM->AddVariable("JetDiscAJl2", &ev_bdt_JetDiscAJl2);
    MVAReaderMM->AddVariable("dRll", &ev_bdt_dRll);
    MVAReaderMM->AddVariable("dRlj11", &ev_bdt_dRlj11);
    MVAReaderMM->AddVariable("dRlj21", &ev_bdt_dRlj21);
    MVAReaderMM->AddVariable("dRlj22", &ev_bdt_dRlj22);
    MVAReaderMM->AddVariable("MSSSF", &ev_bdt_MSSSF);
    MVAReaderMM->AddVariable("Mlj11", &ev_bdt_Mlj11);
    MVAReaderMM->AddVariable("Mlj12", &ev_bdt_Mlj12);
    MVAReaderMM->AddVariable("Mlj21", &ev_bdt_Mlj21);
    MVAReaderMM->AddVariable("Mlj22", &ev_bdt_Mlj22);
    MVAReaderMM->AddVariable("MTvl1", &ev_bdt_MTvl1);
    MVAReaderMM->AddVariable("MTvl2", &ev_bdt_MTvl2);
    MVAReaderMM->AddVariable("Mllj1", &ev_bdt_Mllj1);
    MVAReaderMM->AddVariable("dRWjj", &ev_bdt_dRWjj);
    MVAReaderMM->AddVariable("dRlN12", &ev_bdt_dRlN12);
    MVAReaderMM->AddVariable("dRlN21", &ev_bdt_dRlN21);
    MVAReaderMM->AddVariable("M_W2_jj", &ev_bdt_M_W2_jj);
    MVAReaderMM->AddVariable("M_N1_l1jj", &ev_bdt_M_N1_l1jj);
    MVAReaderMM->AddVariable("M_N2_l2jj", &ev_bdt_M_N2_l2jj);
    MVAReaderMM->AddSpectator("w_tot", &w_tot);

    if(version=="V1"){
      MVAReaderMMFake->AddVariable("Nvbfj", &ev_bdt_Nvbfj);
      MVAReaderMMFake->AddVariable("Nb", &ev_bdt_Nb);
      MVAReaderMMFake->AddVariable("Ptl1", &ev_bdt_Ptl1);
      MVAReaderMMFake->AddVariable("Ptl2", &ev_bdt_Ptl2);
      MVAReaderMMFake->AddVariable("Ptj1", &ev_bdt_Ptj1);
      MVAReaderMMFake->AddVariable("MET2ST", &ev_bdt_MET2ST);
      MVAReaderMMFake->AddVariable("HTLT1", &ev_bdt_HTLT1);
      MVAReaderMMFake->AddVariable("HTLT2", &ev_bdt_HTLT2);
      MVAReaderMMFake->AddVariable("Etal1", &ev_bdt_Etal1);
      MVAReaderMMFake->AddVariable("Etal2", &ev_bdt_Etal2);
      MVAReaderMMFake->AddVariable("PtRatioAJl1", &ev_bdt_PtRatioAJl1);
      MVAReaderMMFake->AddVariable("NEMFracAJl2", &ev_bdt_NEMFracAJl2);
      MVAReaderMMFake->AddVariable("CHFracAJl1", &ev_bdt_CHFracAJl1);
      MVAReaderMMFake->AddVariable("CHFracAJl2", &ev_bdt_CHFracAJl2);
      MVAReaderMMFake->AddVariable("NHFracAJl1", &ev_bdt_NHFracAJl1);
      MVAReaderMMFake->AddVariable("NHFracAJl2", &ev_bdt_NHFracAJl2);
      MVAReaderMMFake->AddVariable("JetDiscAJl1", &ev_bdt_JetDiscAJl1);
      MVAReaderMMFake->AddVariable("JetDiscAJl2", &ev_bdt_JetDiscAJl2);
      MVAReaderMMFake->AddVariable("dRll", &ev_bdt_dRll);
      MVAReaderMMFake->AddVariable("dRlj11", &ev_bdt_dRlj11);
      MVAReaderMMFake->AddVariable("dRlj12", &ev_bdt_dRlj12);
      MVAReaderMMFake->AddVariable("dRlj21", &ev_bdt_dRlj21);
      MVAReaderMMFake->AddVariable("dRlj22", &ev_bdt_dRlj22);
      MVAReaderMMFake->AddVariable("MSSSF", &ev_bdt_MSSSF);
      MVAReaderMMFake->AddVariable("Mlj11", &ev_bdt_Mlj11);
      MVAReaderMMFake->AddVariable("Mlj12", &ev_bdt_Mlj12);
      MVAReaderMMFake->AddVariable("Mlj21", &ev_bdt_Mlj21);
      MVAReaderMMFake->AddVariable("Mlj22", &ev_bdt_Mlj22);
      MVAReaderMMFake->AddVariable("MTvl1", &ev_bdt_MTvl1);
      MVAReaderMMFake->AddVariable("MTvl2", &ev_bdt_MTvl2);
      MVAReaderMMFake->AddVariable("Mllj1", &ev_bdt_Mllj1);
      MVAReaderMMFake->AddVariable("dRWjj", &ev_bdt_dRWjj);
      MVAReaderMMFake->AddVariable("M_W2_jj", &ev_bdt_M_W2_jj);
      MVAReaderMMFake->AddVariable("M_N1_l1jj", &ev_bdt_M_N1_l1jj);
      MVAReaderMMFake->AddVariable("M_N2_l2jj", &ev_bdt_M_N2_l2jj);
      MVAReaderMMFake->AddSpectator("w_tot", &w_tot);
      
      MVAReaderMMNonFake->AddVariable("Nvbfj", &ev_bdt_Nvbfj);
      MVAReaderMMNonFake->AddVariable("Nb", &ev_bdt_Nb);
      MVAReaderMMNonFake->AddVariable("Ptl1", &ev_bdt_Ptl1);
      MVAReaderMMNonFake->AddVariable("Ptl2", &ev_bdt_Ptl2);
      MVAReaderMMNonFake->AddVariable("Ptj1", &ev_bdt_Ptj1);
      MVAReaderMMNonFake->AddVariable("Ptj2", &ev_bdt_Ptj2);
      MVAReaderMMNonFake->AddVariable("MET2ST", &ev_bdt_MET2ST);
      MVAReaderMMNonFake->AddVariable("HTLT1", &ev_bdt_HTLT1);
      MVAReaderMMNonFake->AddVariable("HTLT2", &ev_bdt_HTLT2);
      MVAReaderMMNonFake->AddVariable("Etal1", &ev_bdt_Etal1);
      MVAReaderMMNonFake->AddVariable("Etal2", &ev_bdt_Etal2);
      MVAReaderMMNonFake->AddVariable("PtRatioAJl1", &ev_bdt_PtRatioAJl1);
      MVAReaderMMNonFake->AddVariable("NEMFracAJl1", &ev_bdt_NEMFracAJl1);
      MVAReaderMMNonFake->AddVariable("CHFracAJl1", &ev_bdt_CHFracAJl1);
      MVAReaderMMNonFake->AddVariable("CHFracAJl2", &ev_bdt_CHFracAJl2);
      MVAReaderMMNonFake->AddVariable("MuFracAJl1", &ev_bdt_MuFracAJl1);
      MVAReaderMMNonFake->AddVariable("MuFracAJl2", &ev_bdt_MuFracAJl2);
      MVAReaderMMNonFake->AddVariable("JetDiscAJl1", &ev_bdt_JetDiscAJl1);
      MVAReaderMMNonFake->AddVariable("JetDiscAJl2", &ev_bdt_JetDiscAJl2);
      MVAReaderMMNonFake->AddVariable("dRll", &ev_bdt_dRll);
      MVAReaderMMNonFake->AddVariable("dRlj11", &ev_bdt_dRlj11);
      MVAReaderMMNonFake->AddVariable("dRlj21", &ev_bdt_dRlj21);
      MVAReaderMMNonFake->AddVariable("MSSSF", &ev_bdt_MSSSF);
      MVAReaderMMNonFake->AddVariable("Mlj11", &ev_bdt_Mlj11);
      MVAReaderMMNonFake->AddVariable("Mlj21", &ev_bdt_Mlj21);
      MVAReaderMMNonFake->AddVariable("Mlj22", &ev_bdt_Mlj22);
      MVAReaderMMNonFake->AddVariable("MTvl1", &ev_bdt_MTvl1);
      MVAReaderMMNonFake->AddVariable("MTvl2", &ev_bdt_MTvl2);
      MVAReaderMMNonFake->AddVariable("Mllj1", &ev_bdt_Mllj1);
      MVAReaderMMNonFake->AddVariable("dRWjj", &ev_bdt_dRWjj);
      MVAReaderMMNonFake->AddVariable("dRlN12", &ev_bdt_dRlN12);
      MVAReaderMMNonFake->AddVariable("dRlN21", &ev_bdt_dRlN21);
      MVAReaderMMNonFake->AddVariable("M_W2_jj", &ev_bdt_M_W2_jj);
      MVAReaderMMNonFake->AddVariable("M_N1_l1jj", &ev_bdt_M_N1_l1jj);
      MVAReaderMMNonFake->AddVariable("M_N2_l2jj", &ev_bdt_M_N2_l2jj);
      MVAReaderMMNonFake->AddSpectator("w_tot", &w_tot);
    }
  }

  if(ee){

    MVAReaderEE->AddVariable("Nvbfj", &ev_bdt_Nvbfj);
    MVAReaderEE->AddVariable("Nb", &ev_bdt_Nb);
    MVAReaderEE->AddVariable("Ptl1", &ev_bdt_Ptl1);
    MVAReaderEE->AddVariable("Ptl2", &ev_bdt_Ptl2);
    MVAReaderEE->AddVariable("Ptj1", &ev_bdt_Ptj1);
    MVAReaderEE->AddVariable("MET2ST", &ev_bdt_MET2ST);
    MVAReaderEE->AddVariable("HTLT1", &ev_bdt_HTLT1);
    MVAReaderEE->AddVariable("HTLT2", &ev_bdt_HTLT2);
    MVAReaderEE->AddVariable("Etal1", &ev_bdt_Etal1);
    MVAReaderEE->AddVariable("Etal2", &ev_bdt_Etal2);
    MVAReaderEE->AddVariable("PtRatioAJl1", &ev_bdt_PtRatioAJl1);
    MVAReaderEE->AddVariable("CEMFracAJl2", &ev_bdt_CEMFracAJl2);
    MVAReaderEE->AddVariable("NEMFracAJl1", &ev_bdt_NEMFracAJl1);
    MVAReaderEE->AddVariable("NEMFracAJl2", &ev_bdt_NEMFracAJl2);
    MVAReaderEE->AddVariable("CHFracAJl1", &ev_bdt_CHFracAJl1);
    MVAReaderEE->AddVariable("CHFracAJl2", &ev_bdt_CHFracAJl2);
    MVAReaderEE->AddVariable("JetDiscAJl1", &ev_bdt_JetDiscAJl1);
    MVAReaderEE->AddVariable("JetDiscAJl2", &ev_bdt_JetDiscAJl2);
    MVAReaderEE->AddVariable("dRll", &ev_bdt_dRll);
    MVAReaderEE->AddVariable("dRlj11", &ev_bdt_dRlj11);
    MVAReaderEE->AddVariable("dRlj21", &ev_bdt_dRlj21);
    MVAReaderEE->AddVariable("dRlj22", &ev_bdt_dRlj22);
    MVAReaderEE->AddVariable("MSSSF", &ev_bdt_MSSSF);
    MVAReaderEE->AddVariable("Mlj11", &ev_bdt_Mlj11);
    MVAReaderEE->AddVariable("Mlj12", &ev_bdt_Mlj12);
    MVAReaderEE->AddVariable("Mlj21", &ev_bdt_Mlj21);
    MVAReaderEE->AddVariable("Mlj22", &ev_bdt_Mlj22);
    MVAReaderEE->AddVariable("MTvl1", &ev_bdt_MTvl1);
    MVAReaderEE->AddVariable("MTvl2", &ev_bdt_MTvl2);
    MVAReaderEE->AddVariable("Mllj1", &ev_bdt_Mllj1);
    MVAReaderEE->AddVariable("dRWjj", &ev_bdt_dRWjj);
    MVAReaderEE->AddVariable("dRlN21", &ev_bdt_dRlN21);
    MVAReaderEE->AddVariable("M_W2_jj", &ev_bdt_M_W2_jj);
    MVAReaderEE->AddVariable("M_N1_l1jj", &ev_bdt_M_N1_l1jj);
    MVAReaderEE->AddVariable("M_N2_l2jj", &ev_bdt_M_N2_l2jj);
    MVAReaderEE->AddSpectator("w_tot", &w_tot);

    if(version=="V1"){

      MVAReaderEEFake->AddVariable("Nvbfj", &ev_bdt_Nvbfj);
      MVAReaderEEFake->AddVariable("Nb", &ev_bdt_Nb);
      MVAReaderEEFake->AddVariable("Ptl1", &ev_bdt_Ptl1);
      MVAReaderEEFake->AddVariable("Ptl2", &ev_bdt_Ptl2);
      MVAReaderEEFake->AddVariable("Ptj1", &ev_bdt_Ptj1);
      MVAReaderEEFake->AddVariable("MET2ST", &ev_bdt_MET2ST);
      MVAReaderEEFake->AddVariable("HTLT1", &ev_bdt_HTLT1);
      MVAReaderEEFake->AddVariable("HTLT2", &ev_bdt_HTLT2);
      MVAReaderEEFake->AddVariable("Etal1", &ev_bdt_Etal1);
      MVAReaderEEFake->AddVariable("Etal2", &ev_bdt_Etal2);
      MVAReaderEEFake->AddVariable("NEMFracAJl1", &ev_bdt_NEMFracAJl1);
      MVAReaderEEFake->AddVariable("NEMFracAJl2", &ev_bdt_NEMFracAJl2);
      MVAReaderEEFake->AddVariable("CHFracAJl1", &ev_bdt_CHFracAJl1);
      MVAReaderEEFake->AddVariable("CHFracAJl2", &ev_bdt_CHFracAJl2);
      MVAReaderEEFake->AddVariable("NHFracAJl1", &ev_bdt_NHFracAJl1);
      MVAReaderEEFake->AddVariable("NHFracAJl2", &ev_bdt_NHFracAJl2);
      MVAReaderEEFake->AddVariable("JetDiscAJl1", &ev_bdt_JetDiscAJl1);
      MVAReaderEEFake->AddVariable("JetDiscAJl2", &ev_bdt_JetDiscAJl2);
      MVAReaderEEFake->AddVariable("dRll", &ev_bdt_dRll);
      MVAReaderEEFake->AddVariable("dRlj11", &ev_bdt_dRlj11);
      MVAReaderEEFake->AddVariable("dRlj12", &ev_bdt_dRlj12);
      MVAReaderEEFake->AddVariable("dRlj21", &ev_bdt_dRlj21);
      MVAReaderEEFake->AddVariable("dRlj22", &ev_bdt_dRlj22);
      MVAReaderEEFake->AddVariable("MSSSF", &ev_bdt_MSSSF);
      MVAReaderEEFake->AddVariable("Mlj11", &ev_bdt_Mlj11);
      MVAReaderEEFake->AddVariable("Mlj21", &ev_bdt_Mlj21);
      MVAReaderEEFake->AddVariable("Mlj22", &ev_bdt_Mlj22);
      MVAReaderEEFake->AddVariable("MTvl1", &ev_bdt_MTvl1);
      MVAReaderEEFake->AddVariable("MTvl2", &ev_bdt_MTvl2);
      MVAReaderEEFake->AddVariable("Mllj1", &ev_bdt_Mllj1);
      MVAReaderEEFake->AddVariable("dRWjj", &ev_bdt_dRWjj);
      MVAReaderEEFake->AddVariable("dRlN21", &ev_bdt_dRlN21);
      MVAReaderEEFake->AddVariable("M_W2_jj", &ev_bdt_M_W2_jj);
      MVAReaderEEFake->AddVariable("M_N1_l1jj", &ev_bdt_M_N1_l1jj);
      MVAReaderEEFake->AddVariable("M_N2_l2jj", &ev_bdt_M_N2_l2jj);
      MVAReaderEEFake->AddSpectator("w_tot", &w_tot);
    
      
      MVAReaderEENonFake->AddVariable("Nvbfj", &ev_bdt_Nvbfj);
      MVAReaderEENonFake->AddVariable("Nb", &ev_bdt_Nb);
      MVAReaderEENonFake->AddVariable("Ptl1", &ev_bdt_Ptl1);
      MVAReaderEENonFake->AddVariable("Ptl2", &ev_bdt_Ptl2);
      MVAReaderEENonFake->AddVariable("Ptj1", &ev_bdt_Ptj1);
      MVAReaderEENonFake->AddVariable("Ptj2", &ev_bdt_Ptj2);
      MVAReaderEENonFake->AddVariable("MET2ST", &ev_bdt_MET2ST);
      MVAReaderEENonFake->AddVariable("HTLT1", &ev_bdt_HTLT1);
      MVAReaderEENonFake->AddVariable("HTLT2", &ev_bdt_HTLT2);
      MVAReaderEENonFake->AddVariable("Etal1", &ev_bdt_Etal1);
      MVAReaderEENonFake->AddVariable("Etal2", &ev_bdt_Etal2);
      MVAReaderEENonFake->AddVariable("CEMFracAJl1", &ev_bdt_CEMFracAJl1);
      MVAReaderEENonFake->AddVariable("CEMFracAJl2", &ev_bdt_CEMFracAJl2);
      MVAReaderEENonFake->AddVariable("NEMFracAJl1", &ev_bdt_NEMFracAJl1);
      MVAReaderEENonFake->AddVariable("NEMFracAJl2", &ev_bdt_NEMFracAJl2);
      MVAReaderEENonFake->AddVariable("CHFracAJl1", &ev_bdt_CHFracAJl1);
      MVAReaderEENonFake->AddVariable("CHFracAJl2", &ev_bdt_CHFracAJl2);
      MVAReaderEENonFake->AddVariable("JetDiscAJl1", &ev_bdt_JetDiscAJl1);
      MVAReaderEENonFake->AddVariable("JetDiscAJl2", &ev_bdt_JetDiscAJl2);
      MVAReaderEENonFake->AddVariable("dRll", &ev_bdt_dRll);
      MVAReaderEENonFake->AddVariable("dRlj11", &ev_bdt_dRlj11);
      MVAReaderEENonFake->AddVariable("dRlj21", &ev_bdt_dRlj21);
      MVAReaderEENonFake->AddVariable("MSSSF", &ev_bdt_MSSSF);
      MVAReaderEENonFake->AddVariable("Mlj11", &ev_bdt_Mlj11);
      MVAReaderEENonFake->AddVariable("Mlj21", &ev_bdt_Mlj21);
      MVAReaderEENonFake->AddVariable("Mlj22", &ev_bdt_Mlj22);
      MVAReaderEENonFake->AddVariable("MTvl1", &ev_bdt_MTvl1);
      MVAReaderEENonFake->AddVariable("MTvl2", &ev_bdt_MTvl2);
      MVAReaderEENonFake->AddVariable("Mllj1", &ev_bdt_Mllj1);
      MVAReaderEENonFake->AddVariable("dRWjj", &ev_bdt_dRWjj);
      MVAReaderEENonFake->AddVariable("dRlN12", &ev_bdt_dRlN12);
      MVAReaderEENonFake->AddVariable("dRlN21", &ev_bdt_dRlN21);
      MVAReaderEENonFake->AddVariable("M_W2_jj", &ev_bdt_M_W2_jj);
      MVAReaderEENonFake->AddVariable("M_N1_l1jj", &ev_bdt_M_N1_l1jj);
      MVAReaderEENonFake->AddVariable("M_N2_l2jj", &ev_bdt_M_N2_l2jj);
      MVAReaderEENonFake->AddSpectator("w_tot", &w_tot);
    }
  }

  if(emu){
    
    MVAReaderEM->AddVariable("Nvbfj", &ev_bdt_Nvbfj);
    MVAReaderEM->AddVariable("Nb", &ev_bdt_Nb);
    MVAReaderEM->AddVariable("Ptl1", &ev_bdt_Ptl1);
    MVAReaderEM->AddVariable("Ptl2", &ev_bdt_Ptl2);
    MVAReaderEM->AddVariable("Ptj1", &ev_bdt_Ptj1);
    MVAReaderEM->AddVariable("Ptj2", &ev_bdt_Ptj2);
    MVAReaderEM->AddVariable("MET2ST", &ev_bdt_MET2ST);
    MVAReaderEM->AddVariable("HTLT1", &ev_bdt_HTLT1);
    MVAReaderEM->AddVariable("HTLT2", &ev_bdt_HTLT2);
    MVAReaderEM->AddVariable("Etal1", &ev_bdt_Etal1);
    MVAReaderEM->AddVariable("Etal2", &ev_bdt_Etal2);
    MVAReaderEM->AddVariable("PtRatioAJl1", &ev_bdt_PtRatioAJl1);
    MVAReaderEM->AddVariable("NEMFracAJl1", &ev_bdt_NEMFracAJl1);
    MVAReaderEM->AddVariable("CHFracAJl1", &ev_bdt_CHFracAJl1);
    MVAReaderEM->AddVariable("CHFracAJl2", &ev_bdt_CHFracAJl2);
    MVAReaderEM->AddVariable("JetDiscAJl1", &ev_bdt_JetDiscAJl1);
    MVAReaderEM->AddVariable("JetDiscAJl2", &ev_bdt_JetDiscAJl2);
    MVAReaderEM->AddVariable("dRll", &ev_bdt_dRll);
    MVAReaderEM->AddVariable("dRlj11", &ev_bdt_dRlj11);
    MVAReaderEM->AddVariable("dRlj12", &ev_bdt_dRlj12);
    MVAReaderEM->AddVariable("dRlj21", &ev_bdt_dRlj21);
    MVAReaderEM->AddVariable("dRlj22", &ev_bdt_dRlj22);
    MVAReaderEM->AddVariable("MSSSF", &ev_bdt_MSSSF);
    MVAReaderEM->AddVariable("Mlj11", &ev_bdt_Mlj11);
    MVAReaderEM->AddVariable("Mlj12", &ev_bdt_Mlj12);
    MVAReaderEM->AddVariable("Mlj21", &ev_bdt_Mlj21);
    MVAReaderEM->AddVariable("Mlj22", &ev_bdt_Mlj22);
    MVAReaderEM->AddVariable("MTvl1", &ev_bdt_MTvl1);
    MVAReaderEM->AddVariable("MTvl2", &ev_bdt_MTvl2);
    MVAReaderEM->AddVariable("Mllj1", &ev_bdt_Mllj1);
    MVAReaderEM->AddVariable("dRWjj", &ev_bdt_dRWjj);
    MVAReaderEM->AddVariable("dRlN21", &ev_bdt_dRlN21);
    MVAReaderEM->AddVariable("M_W2_jj", &ev_bdt_M_W2_jj);
    MVAReaderEM->AddVariable("M_N1_l1jj", &ev_bdt_M_N1_l1jj);
    MVAReaderEM->AddVariable("M_N2_l2jj", &ev_bdt_M_N2_l2jj);
    MVAReaderEM->AddSpectator("w_tot", &w_tot);

    if(version=="V1"){
      MVAReaderEMFake->AddVariable("Nvbfj", &ev_bdt_Nvbfj);
      MVAReaderEMFake->AddVariable("Nb", &ev_bdt_Nb);
      MVAReaderEMFake->AddVariable("Ptl1", &ev_bdt_Ptl1);
      MVAReaderEMFake->AddVariable("Ptl2", &ev_bdt_Ptl2);
      MVAReaderEMFake->AddVariable("Ptj1", &ev_bdt_Ptj1);
      MVAReaderEMFake->AddVariable("MET2ST", &ev_bdt_MET2ST);
      MVAReaderEMFake->AddVariable("HTLT1", &ev_bdt_HTLT1);
      MVAReaderEMFake->AddVariable("HTLT2", &ev_bdt_HTLT2);
      MVAReaderEMFake->AddVariable("Etal1", &ev_bdt_Etal1);
      MVAReaderEMFake->AddVariable("Etal2", &ev_bdt_Etal2);
      MVAReaderEMFake->AddVariable("PtRatioAJl1", &ev_bdt_PtRatioAJl1);
      MVAReaderEMFake->AddVariable("NEMFracAJl1", &ev_bdt_NEMFracAJl1);
      MVAReaderEMFake->AddVariable("NEMFracAJl2", &ev_bdt_NEMFracAJl2);
      MVAReaderEMFake->AddVariable("CHFracAJl1", &ev_bdt_CHFracAJl1);
      MVAReaderEMFake->AddVariable("CHFracAJl2", &ev_bdt_CHFracAJl2);
      MVAReaderEMFake->AddVariable("NHFracAJl2", &ev_bdt_NHFracAJl2);
      MVAReaderEMFake->AddVariable("JetDiscAJl1", &ev_bdt_JetDiscAJl1);
      MVAReaderEMFake->AddVariable("JetDiscAJl2", &ev_bdt_JetDiscAJl2);
      MVAReaderEMFake->AddVariable("dRll", &ev_bdt_dRll);
      MVAReaderEMFake->AddVariable("dRlj11", &ev_bdt_dRlj11);
      MVAReaderEMFake->AddVariable("dRlj12", &ev_bdt_dRlj12);
      MVAReaderEMFake->AddVariable("dRlj21", &ev_bdt_dRlj21);
      MVAReaderEMFake->AddVariable("dRlj22", &ev_bdt_dRlj22);
      MVAReaderEMFake->AddVariable("MSSSF", &ev_bdt_MSSSF);
      MVAReaderEMFake->AddVariable("Mlj11", &ev_bdt_Mlj11);
      MVAReaderEMFake->AddVariable("Mlj21", &ev_bdt_Mlj21);
      MVAReaderEMFake->AddVariable("MTvl1", &ev_bdt_MTvl1);
      MVAReaderEMFake->AddVariable("MTvl2", &ev_bdt_MTvl2);
      MVAReaderEMFake->AddVariable("Mllj1", &ev_bdt_Mllj1);
      MVAReaderEMFake->AddVariable("dRWjj", &ev_bdt_dRWjj);
      MVAReaderEMFake->AddVariable("dRlN12", &ev_bdt_dRlN12);
      MVAReaderEMFake->AddVariable("dRlN21", &ev_bdt_dRlN21);
      MVAReaderEMFake->AddVariable("M_W2_jj", &ev_bdt_M_W2_jj);
      MVAReaderEMFake->AddVariable("M_N1_l1jj", &ev_bdt_M_N1_l1jj);
      MVAReaderEMFake->AddVariable("M_N2_l2jj", &ev_bdt_M_N2_l2jj);
      MVAReaderEMFake->AddSpectator("w_tot", &w_tot);
      
      MVAReaderEMNonFake->AddVariable("Nvbfj", &ev_bdt_Nvbfj);
      MVAReaderEMNonFake->AddVariable("Nb", &ev_bdt_Nb);
      MVAReaderEMNonFake->AddVariable("Ptl1", &ev_bdt_Ptl1);
      MVAReaderEMNonFake->AddVariable("Ptl2", &ev_bdt_Ptl2);
      MVAReaderEMNonFake->AddVariable("Ptj1", &ev_bdt_Ptj1);
      MVAReaderEMNonFake->AddVariable("Ptj2", &ev_bdt_Ptj2);
      MVAReaderEMNonFake->AddVariable("MET2ST", &ev_bdt_MET2ST);
      MVAReaderEMNonFake->AddVariable("HTLT1", &ev_bdt_HTLT1);
      MVAReaderEMNonFake->AddVariable("HTLT2", &ev_bdt_HTLT2);
      MVAReaderEMNonFake->AddVariable("Etal1", &ev_bdt_Etal1);
      MVAReaderEMNonFake->AddVariable("Etal2", &ev_bdt_Etal2);
      MVAReaderEMNonFake->AddVariable("PtRatioAJl1", &ev_bdt_PtRatioAJl1);
      MVAReaderEMNonFake->AddVariable("CEMFracAJl2", &ev_bdt_CEMFracAJl2);
      MVAReaderEMNonFake->AddVariable("NEMFracAJl1", &ev_bdt_NEMFracAJl1);
      MVAReaderEMNonFake->AddVariable("CHFracAJl1", &ev_bdt_CHFracAJl1);
      MVAReaderEMNonFake->AddVariable("CHFracAJl2", &ev_bdt_CHFracAJl2);
      MVAReaderEMNonFake->AddVariable("JetDiscAJl1", &ev_bdt_JetDiscAJl1);
      MVAReaderEMNonFake->AddVariable("JetDiscAJl2", &ev_bdt_JetDiscAJl2);
      MVAReaderEMNonFake->AddVariable("dRll", &ev_bdt_dRll);
      MVAReaderEMNonFake->AddVariable("dRlj11", &ev_bdt_dRlj11);
      MVAReaderEMNonFake->AddVariable("dRlj21", &ev_bdt_dRlj21);
      MVAReaderEMNonFake->AddVariable("dRlj22", &ev_bdt_dRlj22);
      MVAReaderEMNonFake->AddVariable("MSSSF", &ev_bdt_MSSSF);
      MVAReaderEMNonFake->AddVariable("Mlj11", &ev_bdt_Mlj11);
      MVAReaderEMNonFake->AddVariable("Mlj12", &ev_bdt_Mlj12);
      MVAReaderEMNonFake->AddVariable("Mlj21", &ev_bdt_Mlj21);
      MVAReaderEMNonFake->AddVariable("Mlj22", &ev_bdt_Mlj22);
      MVAReaderEMNonFake->AddVariable("MTvl1", &ev_bdt_MTvl1);
      MVAReaderEMNonFake->AddVariable("MTvl2", &ev_bdt_MTvl2);
      MVAReaderEMNonFake->AddVariable("Mllj1", &ev_bdt_Mllj1);
      MVAReaderEMNonFake->AddVariable("dRWjj", &ev_bdt_dRWjj);
      MVAReaderEMNonFake->AddVariable("dRlN21", &ev_bdt_dRlN21);
      MVAReaderEMNonFake->AddVariable("M_W2_jj", &ev_bdt_M_W2_jj);
      MVAReaderEMNonFake->AddVariable("M_N1_l1jj", &ev_bdt_M_N1_l1jj);
      MVAReaderEMNonFake->AddVariable("M_N2_l2jj", &ev_bdt_M_N2_l2jj);
      MVAReaderEMNonFake->AddSpectator("w_tot", &w_tot);
    }
  }

  for(unsigned int im=0; im<MNStrList.size(); im++){

    //// This can be changed after checking Hyper paramters                                                                                                                                                                                                                                                                   
    //FinalBDTHyperParamMap settings are whats used in limit for SR/CR                                                                                                                                                                                                                                                        
    FinalBDTHyperParamMap[MNStrList.at(im)] = make_pair("200","850");
 
    TString NTreeMM = "850", NCutMM = "200", NTreeEE = "850", NCutEE = "200", NTreeEM = "850", NCutEM = "200";

    if(version == "V1"){
      if(im < 4){ NTreeMM = "700", NCutMM = "150", NTreeEE = "500", NCutEE = "250", NTreeEM = "1000", NCutEM = "100"; }    // mN = 85, 90, 95, 100 GeV
      if(im == 4){ NTreeMM = "600", NCutMM = "100", NTreeEE = "500", NCutEE = "200", NTreeEM = "600", NCutEM = "200"; }    // mN = 125 GeV
      if(im == 5){ NTreeMM = "850", NCutMM = "250", NTreeEE = "600", NCutEE = "250", NTreeEM = "500", NCutEM = "100"; }    // mN = 150 GeV
      if(im == 6){ NTreeMM = "500", NCutMM = "300", NTreeEE = "500", NCutEE = "150", NTreeEM = "1000", NCutEM = "100"; }   // mN = 200 GeV
      if(im == 7){ NTreeMM = "850", NCutMM = "150", NTreeEE = "700", NCutEE = "200", NTreeEM = "850", NCutEM = "200"; }    // mN = 250 GeV
      if(im == 8){ NTreeMM = "850", NCutMM = "200", NTreeEE = "500", NCutEE = "200", NTreeEM = "850", NCutEM = "200"; }    // mN = 300 GeV
      if(im == 9){ NTreeMM = "850", NCutMM = "200", NTreeEE = "500", NCutEE = "400", NTreeEM = "850", NCutEM = "200"; }    // mN = 400 GeV
      if(im == 10){ NTreeMM = "850", NCutMM = "200", NTreeEE = "700", NCutEE = "200", NTreeEM = "850", NCutEM = "200"; }   // mN = 500 GeV
    }
    if(version == "V2"){
      if(im < 4){ NTreeMM = "700", NCutMM = "400", NTreeEE = "1000", NCutEE = "150", NTreeEM = "1000", NCutEM = "150"; }    // mN = 85, 90, 95, 100 GeV
      if(im == 4){ NTreeMM = "850", NCutMM = "150", NTreeEE = "500", NCutEE = "250", NTreeEM = "1000", NCutEM = "400"; }    // mN = 125 GeV
      if(im == 5){ NTreeMM = "850", NCutMM = "150", NTreeEE = "500", NCutEE = "300", NTreeEM = "850", NCutEM = "200"; }     // mN = 150 GeV
      if(im == 6){ NTreeMM = "1000", NCutMM = "400", NTreeEE = "500", NCutEE = "150", NTreeEM = "1000", NCutEM = "200"; }   // mN = 200 GeV
      if(im == 7){ NTreeMM = "1000", NCutMM = "300", NTreeEE = "850", NCutEE = "400", NTreeEM = "1000", NCutEM = "200"; }   // mN = 250 GeV
      if(im == 8){ NTreeMM = "1000", NCutMM = "400", NTreeEE = "500", NCutEE = "250", NTreeEM = "1000", NCutEM = "200"; }   // mN = 300 GeV
      if(im == 9){ NTreeMM = "1000", NCutMM = "100", NTreeEE = "1000", NCutEE = "300", NTreeEM = "1000", NCutEM = "100"; }  // mN = 400 GeV
      if(im == 10){ NTreeMM = "1000", NCutMM = "150", NTreeEE = "850", NCutEE = "250", NTreeEM = "1000", NCutEM = "150"; }  // mN = 500 GeV
    }

    TString FileNameMM        = "output_DY_MuMu_M"+MNStrList.at(im)+"_Incl_Run2_NTrees"+NTreeMM+"_NCuts"+NCutMM+"_MaxDepth3_BDT.weights.xml";
    TString FileNameMMFake    = "output_DY_MuMu_M"+MNStrList.at(im)+"_Fake_Run2_NTrees850_NCuts200_MaxDepth3_BDT.weights.xml";
    TString FileNameMMNonFake = "output_DY_MuMu_M"+MNStrList.at(im)+"_NonFake_Run2_NTrees850_NCuts200_MaxDepth3_BDT.weights.xml";

    TString MVATagStrMM        = "BDT_MuMu_M"+MNStrList.at(im)+"_Incl_NTrees"+NTreeMM+"_NCuts"+NCutMM+"_MaxDepth3";
    TString MVATagStrMMFake    = "BDT_MuMu_M"+MNStrList.at(im)+"_Fake_NTrees850_NCuts200_MaxDepth3";
    TString MVATagStrMMNonFake = "BDT_MuMu_M"+MNStrList.at(im)+"_NonFake_NTrees850_NCuts200_MaxDepth3";
    
    if(mm){
      MVAReaderMM->BookMVA(MVATagStrMM, AnalyzerPath+MVAPath+FileNameMM);

      if(version=="V1") MVAReaderMMFake->BookMVA(MVATagStrMMFake, AnalyzerPath+MVAPathV1+FileNameMMFake);
      if(version=="V1") MVAReaderMMNonFake->BookMVA(MVATagStrMMNonFake, AnalyzerPath+MVAPathV1+FileNameMMNonFake);
      map_bdt_booked["Incl_MuMu_M"+MNStrList.at(im)] = MVATagStrMM;
      if(version=="V1") map_bdt_booked["Fake_MuMu_M"+MNStrList.at(im)] = MVATagStrMMFake;
      if(version=="V1") map_bdt_booked["NonFake_MuMu_M"+MNStrList.at(im)] = MVATagStrMMNonFake;

    }

    TString FileNameEE        = "output_DY_EE_M"+MNStrList.at(im)+"_Incl_Run2_NTrees"+NTreeEE+"_NCuts"+NCutEE+"_MaxDepth3_BDT.weights.xml";
    TString FileNameEEFake    = "output_DY_EE_M"+MNStrList.at(im)+"_Fake_Run2_NTrees850_NCuts200_MaxDepth3_BDT.weights.xml";
    TString FileNameEENonFake = "output_DY_EE_M"+MNStrList.at(im)+"_NonFake_Run2_NTrees850_NCuts200_MaxDepth3_BDT.weights.xml";

    TString MVATagStrEE        = "BDT_EE_M"+MNStrList.at(im)+"_Incl_NTrees"+NTreeEE+"_NCuts"+NCutEE+"_MaxDepth3";
    TString MVATagStrEEFake    = "BDT_EE_M"+MNStrList.at(im)+"_Fake_NTrees850_NCuts200_MaxDepth3";
    TString MVATagStrEENonFake = "BDT_EE_M"+MNStrList.at(im)+"_NonFake_NTrees850_NCuts200_MaxDepth3";
    
    if(ee){
      MVAReaderEE->BookMVA(MVATagStrEE, AnalyzerPath+MVAPath+FileNameEE);
      if(version=="V1") MVAReaderEEFake->BookMVA(MVATagStrEEFake, AnalyzerPath+MVAPathV1+FileNameEEFake);
      if(version=="V1") MVAReaderEENonFake->BookMVA(MVATagStrEENonFake, AnalyzerPath+MVAPathV1+FileNameEENonFake);
      map_bdt_booked["Incl_EE_M"+MNStrList.at(im)] = MVATagStrEE;
      if(version=="V1") map_bdt_booked["Fake_EE_M"+MNStrList.at(im)] = MVATagStrEEFake;
      if(version=="V1") map_bdt_booked["NonFake_EE_M"+MNStrList.at(im)] = MVATagStrEENonFake;
    }

    TString FileNameEM        = "output_DY_EMu_M"+MNStrList.at(im)+"_Incl_Run2_NTrees"+NTreeEM+"_NCuts"+NCutEM+"_MaxDepth3_BDT.weights.xml";
    TString FileNameEMFake    = "output_DY_EMu_M"+MNStrList.at(im)+"_Fake_Run2_NTrees850_NCuts200_MaxDepth3_BDT.weights.xml";
    TString FileNameEMNonFake = "output_DY_EMu_M"+MNStrList.at(im)+"_NonFake_Run2_NTrees850_NCuts200_MaxDepth3_BDT.weights.xml";

    TString MVATagStrEM        = "BDT_EMu_M"+MNStrList.at(im)+"_Incl_NTrees"+NTreeEM+"_NCuts"+NCutEM+"_MaxDepth3";
    TString MVATagStrEMFake    = "BDT_EMu_M"+MNStrList.at(im)+"_Fake_NTrees850_NCuts200_MaxDepth3";
    TString MVATagStrEMNonFake = "BDT_EMu_M"+MNStrList.at(im)+"_NonFake_NTrees850_NCuts200_MaxDepth3";

    if(emu){
      MVAReaderEM->BookMVA(MVATagStrEM, AnalyzerPath+MVAPath+FileNameEM);
      if(version=="V1") MVAReaderEMFake->BookMVA(MVATagStrEMFake, AnalyzerPath+MVAPathV1+FileNameEMFake);
      if(version=="V1") MVAReaderEMNonFake->BookMVA(MVATagStrEMNonFake, AnalyzerPath+MVAPathV1+FileNameEMNonFake);
      map_bdt_booked["Incl_EMu_M"+MNStrList.at(im)] = MVATagStrEM;
      if(version=="V1") map_bdt_booked["Fake_EMu_M"+MNStrList.at(im)] = MVATagStrEMFake;
      if(version=="V1") map_bdt_booked["NonFake_EMu_M"+MNStrList.at(im)] = MVATagStrEMNonFake;

    }
  }
  return;

}





/// Event BDT Variables                                                                                                                                                                                                                                                                                                                                                                       

void HNL_LeptonCore::InitializeTreeVars(){

  /// Other var in BDT code (not used to train)                                                                                                                                                                                                                                                                                                                                               
  ev_bdt_Nj=-1;
  ev_bdt_LQ= -1;
  ev_bdt_LT= -1;
  ev_bdt_MET= -1;
  ev_bdt_dEtall  = -1;
  ev_bdt_dRjj12  = -1;
  ev_bdt_dRjj23  = -1;
  ev_bdt_dRjj13  = -1;
  ev_bdt_dRlj11  = -1;
  ev_bdt_dRlj12  = -1;
  ev_bdt_dRlj13  = -1;
  ev_bdt_dRlj21  = -1;
  ev_bdt_dRlj22  = -1;
  ev_bdt_dRlj23  = -1;
  ev_bdt_Mlj13   = -1;
  ev_bdt_Mlj23   = -1;
  ev_bdt_Mllj3   = -1;
  ev_bdt_Mllj4   = -1;
  ev_bdt_Mlljj12 = -1;
  ev_bdt_Mlljj13 = -1;
  ev_bdt_Mlljj14 = -1;
  ev_bdt_Mlljj23 = -1;
  ev_bdt_Mlljj24 = -1;
  ev_bdt_Mlljj34 = -1;
  ev_bdt_Mljj112 = -1;
  ev_bdt_Mljj113 = -1;
  ev_bdt_Mljj114 = -1;
  ev_bdt_Mljj123 = -1;
  ev_bdt_Mljj124 = -1;
  ev_bdt_Mljj134 = -1;
  ev_bdt_Mljj212 = -1;
  ev_bdt_Mljj213 = -1;
  ev_bdt_Mljj214 = -1;
  ev_bdt_Mljj223 = -1;
  ev_bdt_Mljj224 = -1;
  ev_bdt_Mljj234 = -1;
  ev_bdt_Mjj12   = -1;
  ev_bdt_Mjj13   = -1;
  ev_bdt_Mjj14   = -1;
  ev_bdt_Mjj23   = -1;
  ev_bdt_Mjj24   = -1;
  ev_bdt_Mjj34   = -1;
  ev_bdt_HT      = -1;
  ev_bdt_HTLT    = -1;
  ev_bdt_MET2HT = -1;


  /// Used in training                                                                                                                                                                                                                                                                                                                                                                        
  ev_bdt_Nvbfj=-1,  ev_bdt_Nb=-1;
  ev_bdt_Ptl1=-1,   ev_bdt_Ptl2=-1, ev_bdt_Ptj1=-1, ev_bdt_Ptj2=-1, ev_bdt_HTLT1=-1, ev_bdt_HTLT2=-1, ev_bdt_MET2ST=-1, ev_bdt_Etal1=-1, ev_bdt_Etal2=-1;
  ev_bdt_PtRatioAJl1=-1, ev_bdt_PtRatioAJl2=-1;
  ev_bdt_CEMFracAJl1=-1, ev_bdt_CEMFracAJl2=-1, ev_bdt_NEMFracAJl1=-1, ev_bdt_NEMFracAJl2=-1, ev_bdt_CHFracAJl1=-1, ev_bdt_CHFracAJl2=-1, ev_bdt_NHFracAJl1=-1, ev_bdt_NHFracAJl2=-1, ev_bdt_MuFracAJl1=-1, ev_bdt_MuFracAJl2=-1, ev_bdt_JetDiscAJl1=-1, ev_bdt_JetDiscAJl2=-1;
  ev_bdt_dRll=-1, ev_bdt_dRlj11=-1, ev_bdt_dRlj12=-1, ev_bdt_dRlj21=-1, ev_bdt_dRlj22=-1;
  ev_bdt_MSSSF=-1, ev_bdt_Mlj11=-1, ev_bdt_Mlj12=-1, ev_bdt_Mlj21=-1, ev_bdt_Mlj22=-1;
  ev_bdt_MTvl1=-1, ev_bdt_MTvl2=-1,   ev_bdt_Mllj1=-1, ev_bdt_Mllj2=-1;

  ev_bdt_PtWj1=-1, ev_bdt_PtWj2=-1;
  ev_bdt_dRWjj=-1, ev_bdt_dRlW12=-1, ev_bdt_dRlW22=-1, ev_bdt_dRlN12=-1, ev_bdt_dRlN21=-1;
  ev_bdt_M_W2_jj=-1, ev_bdt_M_W1_lljj=-1, ev_bdt_M_N1_l1jj=-1, ev_bdt_M_N2_l2jj=-1;

  w_tot=-1.;
}



void HNL_LeptonCore::SetupEventBDTVariables(std::vector<Lepton *> LepTColl,
                                            std::vector<Jet> JetAllColl,std::vector<Jet> JetColl, std::vector<Jet> JetVBFColl, std::vector<Jet> B_JetColl,
                                            Event  ev, Particle METv, AnalyzerParameter param){

  /// Ensure obj def is correct                                                                                                                                                                                                                                                                                                                                                               
  // All Jets = 10 GeV, noID and eta 3                                                                                                                                                                                                                                                                                                                                                        

  InitializeTreeVars();

  float Mll = GetLLMass(LepTColl);
  ev_bdt_Nb      = B_JetColl.size();
  ev_bdt_Nj      = JetColl.size();
  ev_bdt_Nvbfj   = JetVBFColl.size();
  ev_bdt_Ptl1    = LepTColl[0]->Pt();
  ev_bdt_Ptl2    = LepTColl.at(1)->Pt();
  ev_bdt_LT      = GetLT(LepTColl);
  ev_bdt_Ptj1    = JetColl.size()<1? -1.: JetColl.at(0).Pt();
  ev_bdt_Ptj2    = JetColl.size()<2? -1.:JetColl.at(1).Pt();
  ev_bdt_Ptj3    = JetColl.size()<3? -1.:JetColl.at(2).Pt();
  ev_bdt_MET     = METv.Pt();
  ev_bdt_Etal1   = fabs(LepTColl.at(0)->Eta());
  ev_bdt_Etal2   = fabs(LepTColl.at(1)->Eta());
  ev_bdt_dEtall  = abs(LepTColl.at(0)->Eta()-LepTColl.at(1)->Eta());
  ev_bdt_dRll    = LepTColl.at(0)->DeltaR(*LepTColl.at(1));
  ev_bdt_dRjj12  = JetColl.size()<2? -1.:JetColl.at(0).DeltaR(JetColl.at(1));
  ev_bdt_dRjj23  = JetColl.size()<3? -1.:JetColl.at(1).DeltaR(JetColl.at(2));
  ev_bdt_dRjj13  = JetColl.size()<3? -1.:JetColl.at(0).DeltaR(JetColl.at(2));
  ev_bdt_dRlj11  = JetColl.size()<1? -1.:LepTColl.at(0)->DeltaR(JetColl.at(0));
  ev_bdt_dRlj12  = JetColl.size()<2? -1.:LepTColl.at(0)->DeltaR(JetColl.at(1));
  ev_bdt_dRlj13  = JetColl.size()<3? -1.:LepTColl.at(0)->DeltaR(JetColl.at(2));
  ev_bdt_dRlj21  = JetColl.size()<1? -1.:LepTColl.at(1)->DeltaR(JetColl.at(0));
  ev_bdt_dRlj22  = JetColl.size()<2? -1.:LepTColl.at(1)->DeltaR(JetColl.at(1));
  ev_bdt_dRlj23  = JetColl.size()<3? -1.:LepTColl.at(1)->DeltaR(JetColl.at(2));
  ev_bdt_MSSSF   = Mll;
  ev_bdt_Mlj11   = JetColl.size()<1? -1.:(*LepTColl.at(0)+JetColl.at(0)).M();
  ev_bdt_Mlj12   = JetColl.size()<2? -1.:(*LepTColl.at(0)+JetColl.at(1)).M();
  ev_bdt_Mlj13   = JetColl.size()<3? -1.:(*LepTColl.at(0)+JetColl.at(2)).M();
  ev_bdt_Mlj21   = JetColl.size()<1? -1.:(*LepTColl.at(1)+JetColl.at(0)).M();
  ev_bdt_Mlj22   = JetColl.size()<2? -1.:(*LepTColl.at(1)+JetColl.at(1)).M();
  ev_bdt_Mlj23   = JetColl.size()<3? -1.:(*LepTColl.at(1)+JetColl.at(2)).M();
  ev_bdt_MTvl1   = MT(*LepTColl.at(0),METv);
  ev_bdt_MTvl2   = MT(*LepTColl.at(1),METv);
  ev_bdt_Mllj1   = JetColl.size()<1? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(0)).M();
  ev_bdt_Mllj2   = JetColl.size()<2? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(1)).M();
  ev_bdt_Mllj3   = JetColl.size()<3? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(2)).M();
  ev_bdt_Mllj4   = JetColl.size()<4? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(3)).M();
  ev_bdt_Mlljj12 = JetColl.size()<2? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(0)+JetColl.at(1)).M();
  ev_bdt_Mlljj13 = JetColl.size()<3? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(0)+JetColl.at(2)).M();
  ev_bdt_Mlljj14 = JetColl.size()<4? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(0)+JetColl.at(3)).M();
  ev_bdt_Mlljj23 = JetColl.size()<3? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(1)+JetColl.at(2)).M();
  ev_bdt_Mlljj24 = JetColl.size()<4? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(1)+JetColl.at(3)).M();
  ev_bdt_Mlljj34 = JetColl.size()<4? -1.:(*LepTColl.at(0)+*LepTColl.at(1)+JetColl.at(2)+JetColl.at(3)).M();
  ev_bdt_Mljj112 = JetColl.size()<2? -1.:(*LepTColl.at(0)+JetColl.at(0)+JetColl.at(1)).M();
  ev_bdt_Mljj113 = JetColl.size()<3? -1.:(*LepTColl.at(0)+JetColl.at(0)+JetColl.at(2)).M();
  ev_bdt_Mljj114 = JetColl.size()<4? -1.:(*LepTColl.at(0)+JetColl.at(0)+JetColl.at(3)).M();
  ev_bdt_Mljj123 = JetColl.size()<3? -1.:(*LepTColl.at(0)+JetColl.at(1)+JetColl.at(2)).M();
  ev_bdt_Mljj124 = JetColl.size()<4? -1.:(*LepTColl.at(0)+JetColl.at(1)+JetColl.at(3)).M();
  ev_bdt_Mljj134 = JetColl.size()<4? -1.:(*LepTColl.at(0)+JetColl.at(2)+JetColl.at(3)).M();
  ev_bdt_Mljj212 = JetColl.size()<2? -1.:(*LepTColl.at(1)+JetColl.at(0)+JetColl.at(1)).M();
  ev_bdt_Mljj213 = JetColl.size()<3? -1.:(*LepTColl.at(1)+JetColl.at(0)+JetColl.at(2)).M();
  ev_bdt_Mljj214 = JetColl.size()<4? -1.:(*LepTColl.at(1)+JetColl.at(0)+JetColl.at(3)).M();
  ev_bdt_Mljj223 = JetColl.size()<3? -1.:(*LepTColl.at(1)+JetColl.at(1)+JetColl.at(2)).M();
  ev_bdt_Mljj224 = JetColl.size()<4? -1.:(*LepTColl.at(1)+JetColl.at(1)+JetColl.at(3)).M();
  ev_bdt_Mljj234 = JetColl.size()<4? -1.:(*LepTColl.at(1)+JetColl.at(2)+JetColl.at(3)).M();
  ev_bdt_Mjj12   = JetColl.size()<2? -1.:(JetColl.at(0)+JetColl.at(1)).M();
  ev_bdt_Mjj13   = JetColl.size()<3? -1.:(JetColl.at(0)+JetColl.at(2)).M();
  ev_bdt_Mjj14   = JetColl.size()<4? -1.:(JetColl.at(0)+JetColl.at(3)).M();
  ev_bdt_Mjj23   = JetColl.size()<3? -1.:(JetColl.at(1)+JetColl.at(2)).M();
  ev_bdt_Mjj24   = JetColl.size()<4? -1.:(JetColl.at(1)+JetColl.at(3)).M();
  ev_bdt_Mjj34   = JetColl.size()<4? -1.:(JetColl.at(2)+JetColl.at(3)).M();




  //==== Vars for non-prompt lepton bkg                                                                                                                                                                                                                                                                                                                                                       
  int IdxMatchAwayJetl1=-1, IdxMatchAwayJetl2=-1;
  float maxdPhi1=-999., maxdPhi2=-999.;

  for(unsigned int ij=0; ij<JetAllColl.size(); ij++){

    float dPhi1 = fabs(TVector2::Phi_mpi_pi(LepTColl.at(0)->Phi() - JetAllColl.at(ij).Phi()));
    if(dPhi1>maxdPhi1){ maxdPhi1=dPhi1; IdxMatchAwayJetl1=ij; }

  }

  for(unsigned int ij=0; ij<JetAllColl.size(); ij++){

    float dPhi2 = fabs(TVector2::Phi_mpi_pi(LepTColl.at(1)->Phi() - JetAllColl.at(ij).Phi()));
    if(dPhi2>maxdPhi2){ maxdPhi2=dPhi2; IdxMatchAwayJetl2=ij; }

  }

  if(IdxMatchAwayJetl1!=-1){

    ev_bdt_PtRatioAJl1 = JetAllColl.at(IdxMatchAwayJetl1).Pt()/LepTColl.at(0)->Pt();
    ev_bdt_CEMFracAJl1 = JetAllColl.at(IdxMatchAwayJetl1).ChargedEmEnergyFraction();
    ev_bdt_NEMFracAJl1 = JetAllColl.at(IdxMatchAwayJetl1).NeutralEmEnergyFraction();
    ev_bdt_CHFracAJl1  = JetAllColl.at(IdxMatchAwayJetl1).ChargedHadEnergyFraction();
    ev_bdt_NHFracAJl1  = JetAllColl.at(IdxMatchAwayJetl1).NeutralHadEnergyFraction();
    ev_bdt_MuFracAJl1  = JetAllColl.at(IdxMatchAwayJetl1).MuonEnergyFraction();
    ev_bdt_JetDiscAJl1 = JetAllColl.at(IdxMatchAwayJetl1).GetTaggerResult(JetTagging::DeepJet);

  }
  else{

    ev_bdt_PtRatioAJl1=-1., ev_bdt_CEMFracAJl1=-1., ev_bdt_NEMFracAJl1=-1., ev_bdt_CHFracAJl1=-1., ev_bdt_NHFracAJl1=-1., ev_bdt_MuFracAJl1=-1., ev_bdt_JetDiscAJl1=-1.;

  }

  if(IdxMatchAwayJetl2!=-1){

    ev_bdt_PtRatioAJl2 = JetAllColl.at(IdxMatchAwayJetl2).Pt()/LepTColl.at(1)->Pt();
    ev_bdt_CEMFracAJl2 = JetAllColl.at(IdxMatchAwayJetl2).ChargedEmEnergyFraction();
    ev_bdt_NEMFracAJl2 = JetAllColl.at(IdxMatchAwayJetl2).NeutralEmEnergyFraction();
    ev_bdt_CHFracAJl2  = JetAllColl.at(IdxMatchAwayJetl2).ChargedHadEnergyFraction();
    ev_bdt_NHFracAJl2  = JetAllColl.at(IdxMatchAwayJetl2).NeutralHadEnergyFraction();
    ev_bdt_MuFracAJl2  = JetAllColl.at(IdxMatchAwayJetl2).MuonEnergyFraction();
    ev_bdt_JetDiscAJl2 = JetAllColl.at(IdxMatchAwayJetl2).GetTaggerResult(JetTagging::DeepJet);

  }
  else{

    ev_bdt_PtRatioAJl2=-1., ev_bdt_CEMFracAJl2=-1., ev_bdt_NEMFracAJl2=-1., ev_bdt_CHFracAJl2=-1., ev_bdt_NHFracAJl2=-1., ev_bdt_MuFracAJl2=-1., ev_bdt_JetDiscAJl2=-1.;

  }

  ev_bdt_HT      = 0;
  for(unsigned int itj=0; itj<JetColl.size(); itj++){ ev_bdt_HT+=JetColl.at(itj).Pt(); }
  ev_bdt_HTLT=ev_bdt_HT/ev_bdt_LT;
  ev_bdt_HTLT1=ev_bdt_HT/LepTColl.at(0)->Pt();
  ev_bdt_HTLT2=ev_bdt_HT/LepTColl.at(1)->Pt();

  std::vector<FatJet> FatJetColl;
  double ST = GetST( LepTColl, JetColl, FatJetColl, METv);
  ev_bdt_MET2HT  = JetColl.size()<1? -1.:pow(ev_bdt_MET,2.)/ev_bdt_HT;
  ev_bdt_MET2ST  = pow(ev_bdt_MET,2.)/ST;

  float dijetmass_tmp=9999.;
  float dijetmass=99990000.;
  int m=-999;
  int n=-999;

  for(UInt_t emme=0; emme<JetColl.size(); emme++){
    for(UInt_t enne=emme+1; enne<JetColl.size(); enne++) {

      dijetmass_tmp = (JetColl[emme]+JetColl[enne]).M();
      if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
        dijetmass = dijetmass_tmp;
        m = emme;
        n = enne;
      }
    }
  }

  ev_bdt_PtWj1     = JetColl.size() > 1 ? JetColl[m].Pt() : -1.;
  ev_bdt_PtWj2     = JetColl.size() > 1 ? JetColl[n].Pt() : -1.;
  ev_bdt_dRWjj     = JetColl.size() > 1 ? JetColl[m].DeltaR(JetColl[n]) : -1.;
  ev_bdt_dRlW12    = JetColl.size() > 1 ? LepTColl.at(0)->DeltaR(JetColl[m] + JetColl[n]) : -1.;
  ev_bdt_dRlW22    = JetColl.size() > 1 ? LepTColl.at(1)->DeltaR(JetColl[m] + JetColl[n]) : -1.;
  ev_bdt_dRlN12    = JetColl.size() > 1 ? LepTColl.at(0)->DeltaR(*LepTColl.at(1) + JetColl[m] + JetColl[n]) : -1.;
  ev_bdt_dRlN21    = JetColl.size() > 1 ? LepTColl.at(1)->DeltaR(*LepTColl.at(0) + JetColl[m] + JetColl[n]) : -1.;
  ev_bdt_M_W2_jj   = JetColl.size() > 1 ? (JetColl[m] + JetColl[n]).M() : -1.;
  ev_bdt_M_W1_lljj = JetColl.size() > 1 ? (JetColl[m] + JetColl[n] + *LepTColl.at(0) + *LepTColl.at(1)).M() : -1.;
  ev_bdt_M_N1_l1jj = JetColl.size() > 1 ? (JetColl[m] + JetColl[n] + *LepTColl.at(0)).M() : -1.;
  ev_bdt_M_N2_l2jj = JetColl.size() > 1 ? (JetColl[m] + JetColl[n] + *LepTColl.at(1)).M() : -1.;

  return;
}


double HNL_LeptonCore::EvaluateEventMVA(TString mN, TString bkgType, TString NCut, TString NTree, HNL_LeptonCore::Channel channel,
                                        std::vector<Lepton *> LepTColl, Event ev, Particle METv, AnalyzerParameter param, double weight, bool isVarPlots){

  std::vector<FatJet> FatjetColl                  = GetHNLAK8Jets("HNL_PN",param);
  std::vector<Jet> All_JetColl                    = GetHNLJets("TightPUL",param);
  std::vector<Jet> B_JetColl                      = GetHNLJets("BJet",param);
  std::vector<Jet> JetColl                        = GetHNLJets("TightPUL",param);
  std::vector<Jet> VBF_JetColl                    = GetHNLJets("VBFTightPUL",param);

  SetupEventBDTVariables(LepTColl,
                         All_JetColl, JetColl,VBF_JetColl,B_JetColl,
                         ev,METv,param);

  if(isVarPlots && bkgType=="Incl"){

    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Nvbfj", ev_bdt_Nvbfj, weight, 10, 0., 10.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Nb", ev_bdt_Nb, weight, 10, 0., 10.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Ptl1", ev_bdt_Ptl1, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Ptl2", ev_bdt_Ptl2, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Ptj1", ev_bdt_Ptj1, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Ptj2", ev_bdt_Ptj2, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/MET2ST", ev_bdt_MET2ST, weight, 100, 0., 50.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/HTLT1", ev_bdt_HTLT1, weight, 100, 0., 50.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/HTLT2", ev_bdt_HTLT2, weight, 100, 0., 50.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Etal1", ev_bdt_Etal1, weight, 25, 0., 2.5);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Etal2", ev_bdt_Etal2, weight, 25, 0., 2.5);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/PtRatioAJl1", ev_bdt_PtRatioAJl1, weight, 100, 0., 5.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/CEMFracAJl2", ev_bdt_CEMFracAJl2, weight, 100, 0., 1.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/NEMFracAJl1", ev_bdt_NEMFracAJl1, weight, 100, 0., 1.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/NEMFracAJl2", ev_bdt_NEMFracAJl2, weight, 100, 0., 1.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/CHFracAJl1", ev_bdt_CHFracAJl1, weight, 100, 0., 1.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/CHFracAJl2", ev_bdt_CHFracAJl2, weight, 100, 0., 1.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/MuFracAJl1", ev_bdt_MuFracAJl1, weight, 100, 0., 1.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/MuFracAJl2", ev_bdt_MuFracAJl2, weight, 100, 0., 1.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/JetDiscAJl1", ev_bdt_JetDiscAJl1, weight, 100, 0., 1.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/JetDiscAJl2", ev_bdt_JetDiscAJl2, weight, 100, 0., 1.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/dRll", ev_bdt_dRll, weight, 100, 0., 10.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/dRlj11", ev_bdt_dRlj11, weight, 100, 0., 10.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/dRlj12", ev_bdt_dRlj12, weight, 100, 0., 10.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/dRlj21", ev_bdt_dRlj21, weight, 100, 0., 10.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/dRlj22", ev_bdt_dRlj22, weight, 100, 0., 10.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/MSSSF", ev_bdt_MSSSF, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Mlj11", ev_bdt_Mlj11, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Mlj12", ev_bdt_Mlj12, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Mlj21", ev_bdt_Mlj21, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Mlj22", ev_bdt_Mlj22, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/MTvl1", ev_bdt_MTvl1, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/MTvl2", ev_bdt_MTvl2, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/Mllj1", ev_bdt_Mllj1, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/dRWjj", ev_bdt_dRWjj, weight, 100, 0., 10.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/dRlN12", ev_bdt_dRlN12, weight, 100, 0., 10.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/dRlN21", ev_bdt_dRlN21, weight, 100, 0., 10.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/M_W2_jj", ev_bdt_M_W2_jj, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/M_N1_l1jj", ev_bdt_M_N1_l1jj, weight, 1000, 0., 1000.);
    FillHist("HNL_HighMassSR3_TwoLepton_CR/"+param.Name+"/BDT/M_N2_l2jj", ev_bdt_M_N2_l2jj, weight, 1000, 0., 1000.);

  }

  map<TString,TString>::iterator mapit = map_bdt_booked.find(bkgType+"_"+GetChannelString(channel)+"_M"+mN);
  if(mapit == map_bdt_booked.end()) {
    cout << "[EvaluateEventMVA] : Wrong configuration of map_bdt_booked " << bkgType+"_"+GetChannelString(channel)+"_M"+mN << " was not found as key...." << endl;
    for(auto i : map_bdt_booked) cout << i.first << " : "  << i.second << endl;
    exit(EXIT_FAILURE);
  }
  TString MVATagStr =  mapit->second;
  float MVAvalue = -99999.;

  if(bkgType == "Incl"){
    if(GetChannelString(channel) == "MuMu") MVAvalue = MVAReaderMM->EvaluateMVA(MVATagStr);
    if(GetChannelString(channel) == "EE")   MVAvalue = MVAReaderEE->EvaluateMVA(MVATagStr);
    if(GetChannelString(channel) == "EMu")  MVAvalue = MVAReaderEM->EvaluateMVA(MVATagStr);
  }
  if(bkgType == "Fake"){
    if(GetChannelString(channel) == "MuMu") MVAvalue = MVAReaderMMFake->EvaluateMVA(MVATagStr);
    if(GetChannelString(channel) == "EE")   MVAvalue = MVAReaderEEFake->EvaluateMVA(MVATagStr);
    if(GetChannelString(channel) == "EMu")  MVAvalue = MVAReaderEMFake->EvaluateMVA(MVATagStr);
  }
  if(bkgType == "NonFake"){
    if(GetChannelString(channel) == "MuMu") MVAvalue = MVAReaderMMNonFake->EvaluateMVA(MVATagStr);
    if(GetChannelString(channel) == "EE")   MVAvalue = MVAReaderEENonFake->EvaluateMVA(MVATagStr);
    if(GetChannelString(channel) == "EMu")  MVAvalue = MVAReaderEMNonFake->EvaluateMVA(MVATagStr);
  }

  return MVAvalue;
}

void HNL_LeptonCore::TriggerPrintOut(Event ev){

  vector<TString> PDs = {"DoubleEG","DoubleMuon","MuonEG","SingleElectron","SingleMuon", "EGamma"};

  cout << "TriggerPrintOut: MuMu" << endl;
  for(auto itrig : TrigList_HNL_DblMu){
    for(auto ipd : PDs){
      if(ev.IsPDForTrigger(itrig, ipd)) cout << "Trigger " << itrig << " In dataset " << ipd << endl;
    }
  }

  cout << " " << endl;

  cout << "TriggerPrintOut: EE" << endl;
  for(auto itrig : TrigList_HNL_DblEG){
    for(auto ipd : PDs){
      if(ev.IsPDForTrigger(itrig, ipd)) cout << "Trigger " << itrig << " In dataset " << ipd << endl;
    }
  }

  cout << " " << endl;


  cout << "TriggerPrintOut: EMu" << endl;
  for(auto itrig : TrigList_HNL_MuEG){
    for(auto ipd : PDs){
      if(ev.IsPDForTrigger(itrig, ipd)) cout << "Trigger " << itrig << " In dataset " << ipd << endl;
    }
  }

  cout << " " << endl;

  return;

}

