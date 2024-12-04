#include "HNL_HighPtMuon_Studies.h"

void HNL_HighPtMuon_Studies::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

}


void HNL_HighPtMuon_Studies::executeEvent(){

  AddTimerStamp("start_ev");
  
  FillTimer("START_EV");

  //==== Gen for genmatching
  
  Event ev = GetEvent();  
  AnalyzerParameter param = HNL_LeptonCore::InitialiseHNLParameter("HNL_ULID");
  double weight = SetupWeight(ev,param);


  std::vector<Muon>       Muons    = GetMuons    (param.Muon_Tight_ID,     10.,  2.4);
  std::vector<Electron>   Electrons= SelectElectrons(param,param.Electron_Veto_ID, 10., 2.5);

  if(Muons.size() != 2) return;
  if(Electrons.size() != 0) return;
  

  //  vector<ULong64_t> EventNumbers = {716396397,284632362,567406354 ,37964664 ,1626972627 };
  
  //if (std::find(EventNumbers.begin(), EventNumbers.end(), event) != EventNumbers.end()){
  //  cout << "Event " << event << " run = " << run <<  " Peking ..." << endl;
  //  for(auto imu : Muons)      cout << imu.Pt() << " TuneP4().Pt()=" <<  imu.TuneP4().Pt() << " ID = " << imu.PassID(param.Muon_Tight_ID) << " eta = " << imu.Eta() << endl;
  // }
  

  if(Muons.size() != 2) return;
  
  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);

  if(AK4_BJetColl.size() > 0) return;
  
  std::vector<Lepton *> LepsT       = MakeLeptonPointerVector(Muons,     Electrons,     param);

  if(SameCharge(Muons)){
    FillHist("Inclusive_SS_Muon", Muons[0].Pt() ,  1, 50, 0, 2000 );
    if(Muons[0].Pt() > 200)     FillHist("Inclusive_SS_Muon_HighPt", Muons[0].Pt() ,  1, 50, 0, 2000 );

    if(ev.PassTrigger(TrigList_HNL_DblMu))     FillHist("Inclusive_SS_Muon_PassTrigger", Muons[0].Pt() ,  1, 50, 0, 2000 );

  }


  vector<Muon> MuonsTuneP_Roch =  UseTunePMuon(Muons,200000);
  vector<Muon> MuonsTuneP =  UseTunePMuon(Muons);
  std::vector<Lepton *> LepsT_TuneP       = MakeLeptonPointerVector(MuonsTuneP,     Electrons,     param);

  if(SameCharge(MuonsTuneP)){
    FillHist("Inclusive_SS_MuonsTuneP", MuonsTuneP[0].Pt() ,  1, 50, 0, 2000 );
    if(ev.PassTrigger(TrigList_HNL_HighPtMu))     FillHist("Inclusive_SS_MuonTuneP_PassHighPtTrigger", MuonsTuneP[0].Pt() ,  1, 50, 0, 2000 );

  }
  if(SameCharge(MuonsTuneP_Roch)){
    FillHist("Inclusive_SS_MuonsTuneP_Roch", MuonsTuneP_Roch[0].Pt() ,  1, 50, 0, 2000 );
  }

  

  if(AK8_JetColl.size() == 0 && !PassVBF(AK4_VBF_JetColl,LepsT,750)){
    if(Muons[1].Pt() < 50) return;
  } 

  Particle LL;
  Particle LL_TuneP;
  bool MisMatch=false;
  double R=0;
  for(unsigned int imu = 0; imu < Muons.size(); imu++){
    if(Muons[imu].PassID(param.Muon_Tight_ID)){
      LL=LL+Muons[imu];
      LL_TuneP=LL_TuneP+Muons[imu].TuneP4();
      if(ComparePtTune(Muons[imu], 0.3)) {
	cout << " " << endl;
	cout << "Event " << event << " lumi = " << lumi << " run number = " << run <<  endl;
	cout << "Muon pt mismatch pt = " << Muons[imu].Pt() << " tune pt = "  << Muons[imu].TuneP4().Pt() << " ratio = " <<  Muons[imu].TuneP4().Pt()/ Muons[imu].Pt()  << endl;
	cout << "Muon Eta = " << Muons[imu].Eta() << "   Muon Phi = " << Muons[imu].Phi() <<  " charge = " << Muons[imu].Charge() << endl;
	if(!MisMatch) {
	  R= fabs(Muons[imu].TuneP4().Pt()-Muons[imu].Pt())/ Muons[imu].TuneP4().Pt();
	} 
	MisMatch=true;
      }
    }
  }

  if(MisMatch){
    
    cout << "LL mass = " << LL.M() << "  tune_p mass = " << LL_TuneP.M() << endl;
    Particle METv = GetvMET("PuppiT1xyULCorr",param);              
    Particle METvTuneP = UpdateMETTuneP(METv,Muons);
    cout << "METv = " << METv.Pt() << " tune p met = " << METvTuneP.Pt() << endl;
    double MET2ST = GetMET2ST(LepsT, AK4_JetColl, AK8_JetColl, METv);
    double MET2ST_TuneP = GetMET2ST(LepsT_TuneP, AK4_JetColl, AK8_JetColl, METvTuneP);
    cout << "MET2ST = " << MET2ST << " tune p MET2ST = " << MET2ST_TuneP << endl;

    if(SameCharge(LepsT)) {
      FillHist("Inclusive_SS_N", R ,  1, 200, 0, 5 );
      if(MET2ST < 10) {
	FillHist("Inclusive_SS_MET_N", R ,  1, 200, 0, 5 );
	if(AK8_JetColl.size() > 0)         FillHist("Inclusive_SS_SR1_N", R ,  1, 200, 0, 5 );
	else if(PassVBF(AK4_VBF_JetColl,LepsT,750))  FillHist("Inclusive_SS_SR2_N", R ,  1, 200, 0, 5 );
	else  FillHist("Inclusive_SS_SR3_N", R ,  1, 200, 0, 5 );
      }
      
    }
    else {
      FillHist("Inclusive_OS_N", R ,  1, 200, 0, 5 );
      if(MET2ST < 10) {
	FillHist("Inclusive_OS_MET_N", R ,  1, 200, 0, 5 );
	if(AK8_JetColl.size() > 0)         FillHist("Inclusive_OS_SR1_N", R ,  1, 200, 0, 5 );
        else if(PassVBF(AK4_VBF_JetColl,LepsT,750))  FillHist("Inclusive_OS_SR2_N", R ,  1, 200, 0, 5 );
	else  FillHist("Inclusive_OS_SR3_N", R ,  1, 200, 0, 5 );
      }
      
    }

  }


  return;
  FillTimer("END_Run");
  
  
}
  


HNL_HighPtMuon_Studies::HNL_HighPtMuon_Studies(){


}
 
HNL_HighPtMuon_Studies::~HNL_HighPtMuon_Studies(){
  

}
