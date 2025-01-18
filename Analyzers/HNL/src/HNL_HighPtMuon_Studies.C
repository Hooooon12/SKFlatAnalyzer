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

  //// Access muon and highpt muon collections with rc corr applied as default
  //// Need following muon collections
  //// v1) rochester corrected muons
  //// v2) rochester corr for pt < 200 ; GE corrected for pt > 200
  ////

  std::vector<Muon>       Muons_HNL_v1     = GetHighPtMuons("PF_Nom", param.Muon_Tight_ID,     10.,  2.4);
  std::vector<Muon>       Muons_HNL_v2     = GetHighPtMuons("PF_Roch",param.Muon_Tight_ID,    10.,  2.4);
  std::vector<Muon>       Muons_HNL_v3     = GetHighPtMuons("PF_GE",  param.Muon_Tight_ID,      10.,  2.4);
  std::vector<Muon>       Muons_HNL_v4     = GetHighPtMuons("PF_POG", param.Muon_Tight_ID,     10.,  2.4);
  
  std::vector<Muon>       Muons_HNL_v5     = GetHighPtMuons("TuneP_Nom",param.Muon_Tight_ID,     10.,  2.4);
  std::vector<Muon>       Muons_HNL_v6     = GetHighPtMuons("TuneP_Roch",param.Muon_Tight_ID,    10.,  2.4);
  std::vector<Muon>       Muons_HNL_v7     = GetHighPtMuons("TuneP_GE",param.Muon_Tight_ID,      10.,  2.4);
  std::vector<Muon>       Muons_HNL_v8     = GetHighPtMuons("TuneP_POG",param.Muon_Tight_ID,     10.,  2.4);

  std::vector<Muon> Muons_HNL_Tune_GEUp = ScaleMuons(Muons_HNL_v7,1);
  std::vector<Muon> Muons_HNL_Tune_GEDown = ScaleMuons(Muons_HNL_v7,-1);

  std::vector<Muon>       Muons_HPT_v1     = GetHighPtMuons("PF_Nom","POGHighPtTightWithIP",     10.,  2.4);
  std::vector<Muon>       Muons_HPT_v2     = GetHighPtMuons("PF_Roch","POGHighPtTightWithIP",    10.,  2.4);
  std::vector<Muon>       Muons_HPT_v3     = GetHighPtMuons("PF_GE","POGHighPtTightWithIP",      10.,  2.4);
  std::vector<Muon>       Muons_HPT_v4     = GetHighPtMuons("PF_POG","POGHighPtTightWithIP",     10.,  2.4);

  std::vector<Muon>       Muons_HPT_v5     = GetHighPtMuons("TuneP_Nom","POGHighPtTightWithIP",     10.,  2.4);
  std::vector<Muon>       Muons_HPT_v6     = GetHighPtMuons("TuneP_Roch","POGHighPtTightWithIP",    10.,  2.4);
  std::vector<Muon>       Muons_HPT_v7    = GetHighPtMuons("TuneP_GE","POGHighPtTightWithIP",      10.,  2.4);
  std::vector<Muon>       Muons_HPT_v8     = GetHighPtMuons("TuneP_POG","POGHighPtTightWithIP",     10.,  2.4);


  /// N -1
  std::vector<Muon>       Muons_HNL_C1     = UseTunePMuonUL(GetMuons    ("HNL_ULID_Baseline",     10.,  2.4));
  std::vector<Muon>       Muons_HNL_C2     = UseTunePMuonUL(GetMuons    ("MVALoose",     10.,  2.4));
  std::vector<Muon>       Muons_HNL_C3     = UseTunePMuonUL(GetMuons    ("MVALooseTrgSafe",     10.,  2.4));
  std::vector<Muon>       Muons_HNL_C4     = UseTunePMuonUL(GetMuons    ("HNL_ULID_FO_LOOSE",     10.,  2.4));

  std::vector<Electron>   Electrons= SelectElectrons(param,param.Electron_Veto_ID, 10., 2.5);
  //  std::vector<Muon>  AllMuons= SelectMuons(param,param.Muon_Veto_ID, 10., 2.5);


  if(Electrons.size() != 0) return;
  

  //  vector<ULong64_t> EventNumbers = {716396397,284632362,567406354 ,37964664 ,1626972627 };
  
  //if (std::find(EventNumbers.begin(), EventNumbers.end(), event) != EventNumbers.end()){
  //  cout << "Event " << event << " run = " << run <<  " Peking ..." << endl;
  //  for(auto imu : Muons)      cout << imu.Pt() << " TuneP4().Pt()=" <<  imu.TuneP4().Pt() << " ID = " << imu.PassID(param.Muon_Tight_ID) << " eta = " << imu.Eta() << endl;
  // }
  

  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);


  if(AK4_BJetColl.size() > 0) return;

  DrawMuonPlotsHighPt(Muons_HNL_v1, ev,"Muon_ULID_v1",   Electrons,param);
  DrawMuonPlotsHighPt(Muons_HNL_v2, ev,"Muon_ULID_v2",   Electrons,param);
  DrawMuonPlotsHighPt(Muons_HNL_v3, ev,"Muon_ULID_v3",   Electrons,param);
  DrawMuonPlotsHighPt(Muons_HNL_v4, ev,"Muon_ULID_v4",   Electrons,param);
  DrawMuonPlotsHighPt(Muons_HNL_v5, ev,"Muon_ULID_v5",   Electrons,param);
  DrawMuonPlotsHighPt(Muons_HNL_v6, ev,"Muon_ULID_v6",   Electrons,param);
  DrawMuonPlotsHighPt(Muons_HNL_v7, ev,"Muon_ULID_v7",   Electrons,param);
  DrawMuonPlotsHighPt(Muons_HNL_v8, ev,"Muon_ULID_v8",   Electrons,param);

  DrawMuonPlotsHighPt(Muons_HPT_v1, ev,"Muon_HighPt_v1", Electrons,param);
  DrawMuonPlotsHighPt(Muons_HPT_v2, ev,"Muon_HighPt_v2", Electrons,param);
  DrawMuonPlotsHighPt(Muons_HPT_v3, ev,"Muon_HighPt_v3", Electrons,param);
  DrawMuonPlotsHighPt(Muons_HPT_v4, ev,"Muon_HighPt_v4", Electrons,param);
  DrawMuonPlotsHighPt(Muons_HPT_v5, ev,"Muon_HighPt_v5", Electrons,param);
  DrawMuonPlotsHighPt(Muons_HPT_v6, ev,"Muon_HighPt_v6", Electrons,param);
  DrawMuonPlotsHighPt(Muons_HPT_v7, ev,"Muon_HighPt_v7", Electrons,param);
  DrawMuonPlotsHighPt(Muons_HPT_v8, ev,"Muon_HighPt_v8", Electrons,param);

  DrawMuonPlotsHighPt(Muons_HNL_Tune_GEUp, ev, "Muon_HighPt_Tune_GEUp",Electrons,param);
  DrawMuonPlotsHighPt(Muons_HNL_Tune_GEDown, ev, "Muon_HighPt_Tune_GEDown",Electrons,param);

  DrawMuonPlotsHighPt(Muons_HNL_C1, ev,"Muon_ULID_Nm1_C1",   Electrons,param);
  DrawMuonPlotsHighPt(Muons_HNL_C2, ev,"Muon_ULID_Nm1_C2",   Electrons,param);
  DrawMuonPlotsHighPt(Muons_HNL_C3, ev,"Muon_ULID_Nm1_C3",   Electrons,param);
  DrawMuonPlotsHighPt(Muons_HNL_C4, ev,"Muon_ULID_Nm1_C4",   Electrons,param);


  Particle METv = GetvMET("PuppiT1xyULCorr",param);
  std::vector<Lepton *> LepsT       = MakeLeptonPointerVector(Muons_HNL_v1,     Electrons,     param);
  double MET2ST = GetMET2ST(LepsT, AK4_JetColl, AK8_JetColl, METv);
 
  if(SameCharge(Muons_HNL_v3) && (MET2ST < 15) && ev.PassTrigger(TrigList_HNL_DblMu) && Muons_HNL_v3[0].Pt() > 20 && Muons_HNL_v3[1].Pt() > 10){
    DrawMuonPlotsHighPt(Muons_HNL_v3, ev,"SR_Muon_ULID_v3",   Electrons,param);
  }
  if(SameCharge(Muons_HPT_v3) && (MET2ST < 15) && ev.PassTrigger(TrigList_HNL_HighPtMu) && Muons_HPT_v3[0].Pt() > 55 && Muons_HPT_v3[1].Pt() > 10){
    DrawMuonPlotsHighPt(Muons_HPT_v3, ev,"SR_Muon_HighPt_v3",   Electrons,param);
  }

  //// Check Pt ratio

  if(Muons_HNL_v1.size() < 2) return;

  Particle LL;
  Particle LL_TuneP;
  bool MisMatch=false;
  double R=0;
  double MisMatchedPt=0;
  for(unsigned int imu = 0; imu < Muons_HNL_v1.size(); imu++){
    if(Muons_HNL_v1[imu].PassID(param.Muon_Tight_ID)){
      LL=LL+Muons_HNL_v1[imu];
      LL_TuneP=LL_TuneP+Muons_HNL_v1[imu].TuneP4();

      FillHist("Inclusive_DiLep_N", ((Muons_HNL_v1[imu].TuneP4().Pt()-Muons_HNL_v1[imu].Pt())/ Muons_HNL_v1[imu].TuneP4().Pt()) ,  1, 200, -1, 1 );
      if(ComparePtTune(Muons_HNL_v1[imu], 0.3)) {
	//cout << " " << endl;
	//cout << "Event " << event << " lumi = " << lumi << " run number = " << run <<  endl;
	//cout << "Muon pt mismatch pt = " << Muons_HNL_v1[imu].Pt() << " tune pt = "  << Muons_HNL_v1[imu].TuneP4().Pt() << " ratio = " <<  Muons_HNL_v1[imu].TuneP4().Pt()/ Muons_HNL_v1[imu].Pt()  << endl;
	//cout << "Muon Eta = " << Muons_HNL_v1[imu].Eta() << "   Muon Phi = " << Muons_HNL_v1[imu].Phi() <<  " charge = " << Muons_HNL_v1[imu].Charge() << endl;
	if(!MisMatch) {
	  R= (Muons_HNL_v1[imu].TuneP4().Pt()-Muons_HNL_v1[imu].Pt())/ Muons_HNL_v1[imu].TuneP4().Pt();
	  MisMatchedPt=Muons_HNL_v1[imu].Pt();
	} 
	MisMatch=true;
      }
    }
  }
  
  if(Muons_HNL_v1.size() != 2) return;
  Particle LL2 = Muons_HNL_v1[0] + Muons_HNL_v1[1];
  if(LL2.M() < 20) return;
  if(MET2ST > 10)  return;
  if(Muons_HNL_v1.at(1).Pt()  < 20) return;


  int nPtbins=12;
  double Pt1bins[nPtbins+1] = { 20.,25., 30., 40., 50., 70., 100.,  150.,  200.,400.,600,1000,2000};

  if(MisMatch){
    
    if(SameCharge(LepsT)) {

      FillHist("Mismatched_SS_N",    R ,  1, 200, -1, 1 );
      FillHist("Mismatched_SS_R_Pt", MisMatchedPt,R ,  1,  nPtbins, Pt1bins, 200, -1, 1 );

      if(AK8_JetColl.size() > 0)         FillHist("Mismatched_SS_SR1_N", R ,  1, 200, -1, 1 );
      else if(PassVBF(AK4_VBF_JetColl,LepsT,750))  FillHist("Mismatched_SS_SR2_N", R ,  1, 200, -1, 1 );
      else  FillHist("Mismatched_SS_SR3_N", R ,  1, 200, -1, 1 );
    }
    else {
      FillHist("Mismatched_OS_N", R ,  1, 200, -1, 1 );
      if(AK8_JetColl.size() > 0)         FillHist("Mismatched_OS_SR1_N", R ,  1, 200, -1, 1 );
      else if(PassVBF(AK4_VBF_JetColl,LepsT,750))  FillHist("Mismatched_OS_SR2_N", R ,  1, 200, -1, 1 );
      else if(AK4_JetColl.size() < 2 && LepsT[0]->Pt() > 200) FillHist("Mismatched_OS_SR3_N", R ,  1, 200, -1, 1 );
      else if(AK4_JetColl.size() > 1)  FillHist("Mismatched_OS_SR3_N", R ,  1, 200, -1, 1 );
    }
  }
  
  if(SameCharge(LepsT)) {
    
    for(unsigned int imu = 0; imu < Muons_HNL_v1.size(); imu++){
      if(Muons_HNL_v1[imu].PassID(param.Muon_Tight_ID)){
	FillHist("SS_AllR_N", ((Muons_HNL_v1[imu].TuneP4().Pt()-Muons_HNL_v1[imu].Pt())/ Muons_HNL_v1[imu].TuneP4().Pt()) ,  1, 200, -1, 1 );
      }
    }

    FillHist("R_SS_N", R ,  1, 200, -1, 1 );
    if(AK8_JetColl.size() > 0)         FillHist("R_SS_SR1_N", R ,  1, 200, -1, 1 );
    else if(PassVBF(AK4_VBF_JetColl,LepsT,750))  FillHist("R_SS_SR2_N", R ,  1, 200, -1, 1 );
    else  FillHist("R_SS_SR3_N", R ,  1, 200, -1, 1 );
  }
  else {
    FillHist("R_OS_N", R ,  1, 200, -1, 1 );
    if(AK8_JetColl.size() > 0)         FillHist("R_OS_SR1_N", R ,  1, 200, -1, 1 );
    else if(PassVBF(AK4_VBF_JetColl,LepsT,750))  FillHist("R_OS_SR2_N", R ,  1, 200, -1, 1 );
    else if(AK4_JetColl.size() < 2 && LepsT[0]->Pt() > 200) FillHist("R_OS_SR3_N", R ,  1, 200, -1, 1 );
    else if(AK4_JetColl.size() > 1)  FillHist("R_OS_SR3_N", R ,  1, 200, -1, 1 );

  }

  return;
  FillTimer("END_Run");
  
  
}
  
void HNL_HighPtMuon_Studies::DrawMuonPlotsHighPt(vector<Muon> muons, Event ev,TString label, vector<Electron> electrons, AnalyzerParameter param){

  double ptbins[11] = { 0., 100.,200.,300., 400.,500,700,900,  1200.,1500.,3000};

  double os_ptbins[20] = { 0.,10,20,30,40,50,60,70,80, 100.,120,140,160,180,200.,250,300.,340, 400.,3000};


  if(muons.size() == 0) return;
  
  FillHist("Inclusive/Muon1_Pt_"+label, muons[0].Pt() ,  1, 10,ptbins );
  FillHist("Inclusive/Muon1_Default_Pt_"+label, muons[0].MiniAODPt()   ,  1, 10,ptbins );

  Particle LL = muons[0] + muons[1];
  if(LL.M() < 20) return;
  
  Particle METv = GetvMET("PuppiT1xyULCorr",param);
  std::vector<Lepton *> LepsT       = MakeLeptonPointerVector(muons,    electrons,     param);

  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  
  double MET2ST = GetMET2ST(LepsT, AK4_JetColl, AK8_JetColl, METv);

  if(ev.PassTrigger(TrigList_HNL_DblMu)) {
    if(muons[0].Pt() > 20 && muons[1].Pt() > 10){
      
      if(SameCharge(muons)){
	FillHist("SS/Muon1_Pt_"+label, muons[0].Pt() ,  1, 10,ptbins);
	FillHist("SS/Muon1_Default_Pt_"+label, muons[0].MiniAODPt()   ,  1, 10,ptbins );
      }
      else{
	if(muons[0].Pt() > 200 && muons[1].Pt() > 50){
	  FillHist("OS/Muon1_Pt_"+label, muons[0].Pt() ,  1, 19,os_ptbins);
	  FillHist("OS/Muon1_Default_Pt_"+label, muons[0].MiniAODPt()   ,  1, 19,os_ptbins);
	}
      }
      
      if(MET2ST > 15) return;
      
      if(SameCharge(muons)){
	FillHist("SS_MET/Muon1_Pt_"+label, muons[0].Pt() ,  1, 10,ptbins);
	FillHist("SS_MET/Muon1_Default_Pt_"+label, muons[0].MiniAODPt()   ,  1, 10,ptbins );
      }
      else{
	if(muons[0].Pt() > 200 && muons[1].Pt() > 50){
	  FillHist("OS_MET/Muon1_Pt_"+label, muons[0].Pt() ,  1,  19,os_ptbins);
	  FillHist("OS_MET/Muon1_Default_Pt_"+label, muons[0].MiniAODPt()   ,  1, 19,os_ptbins);
	}
      }
      
      
      if(SameCharge(muons)){
	Muon smearMu = muons[0];
	SmearHighPtMuon(smearMu);
	FillHist("SS_smearcorr/"+label,smearMu.Pt() ,  1, 10,ptbins);
	SmearHighPtMuonSyst(muons);    
	FillHist("SS_smearcorr_syst/"+label, muons[0].Pt() ,  1, 10,ptbins);
      }
    }
  }

  if(ev.PassTrigger(TrigList_HNL_HighPtMu)) {
    if(muons[0].Pt() > 53 && muons[1].Pt() > 10){

      if(SameCharge(muons)){
        FillHist("SS/Mu50_Muon1_Pt_"+label, muons[0].Pt() ,  1, 10,ptbins);
        FillHist("SS/Mu50_Muon1_Default_Pt_"+label, muons[0].MiniAODPt()   ,  1, 10,ptbins );
      }
      else{
        FillHist("OS/Mu50_Muon1_Pt_"+label, muons[0].Pt() ,  1, 19,os_ptbins);
        FillHist("OS/Mu50_Muon1_Default_Pt_"+label, muons[0].MiniAODPt()   ,  1, 19,os_ptbins);
      }

      if(MET2ST > 15) return;

      if(SameCharge(muons)){
        FillHist("SS_MET/Mu50_Muon1_Pt_"+label, muons[0].Pt() ,  1, 10,ptbins);
        FillHist("SS_MET/Mu50_Muon1_Default_Pt_"+label, muons[0].MiniAODPt()   ,  1, 10,ptbins );
      }
      else{
        FillHist("OS_MET/Mu50_Muon1_Pt_"+label, muons[0].Pt() ,  1,  19,os_ptbins);
        FillHist("OS_MET/Mu50_Muon1_Default_Pt_"+label, muons[0].MiniAODPt()   ,  1,  19,os_ptbins);
      }


    }
  }

  

  return;
}



HNL_HighPtMuon_Studies::HNL_HighPtMuon_Studies(){


}
 
HNL_HighPtMuon_Studies::~HNL_HighPtMuon_Studies(){
  

}
