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

  double weight_noZWeight = weight / param.w.zptweight;
  
  
  //// Access muon and highpt muon collections with rc corr applied as default
  //// Need following muon collections
  //// v1) rochester corrected muons
  //// v2) rochester corr for pt < 200 ; GE corrected for pt > 200
  ////


  if (HasFlag("DYPeak")) {
    std::vector<Muon> ZMuons = SelectMuons(param, param.Muon_Tight_ID, 10.0, 2.5);
    if (ZMuons.size() != 2 || SameCharge(ZMuons)) return;

    Lepton l1(ZMuons[0]);
    Lepton l2(ZMuons[1]);

    int Idx1_Closest = GenMatchedIdx(l1, All_Gens);
    int Idx2_Closest = GenMatchedIdx(l2, All_Gens);

    if (std::abs(All_Gens[Idx1_Closest].PID()) != 13 || std::abs(All_Gens[Idx2_Closest].PID()) != 13) return;

    Particle Z = All_Gens[Idx1_Closest] + All_Gens[Idx2_Closest];
    Particle ZReco = ZMuons[0] + ZMuons[1];

    // Fill histograms for Gen-level
    
    std::vector<std::pair<std::string, double>> weights = {
      {"", weight},                // Default weight
      {"_noZWeight", weight_noZWeight}  // Alternative weight
    };
    
    for (const auto& [suffix, w] : weights) {
      // Fill histograms for Gen-level
      FillHist("Gen/Z_Mass" + suffix, Z.M(), w, 2000, 0, 2000);
      FillHist("Gen/Z_Pt" + suffix, Z.Pt(), w, 2000, 0, 2000);
      FillHist("Gen/Mu1_Pt" + suffix, All_Gens[Idx1_Closest].Pt(), w, 2000, 0, 2000);
      FillHist("Gen/Mu2_Pt" + suffix, All_Gens[Idx2_Closest].Pt(), w, 2000, 0, 2000);
      
      // Fill histograms for Reco-level
      FillHist("Reco/Z_Mass" + suffix, ZReco.M(), w, 2000, 0, 2000);
      FillHist("Reco/Z_Pt" + suffix, ZReco.Pt(), w, 2000, 0, 2000);
      FillHist("Reco/Mu1_Pt" + suffix, ZMuons[0].Pt(), w, 2000, 0, 2000);
      FillHist("Reco/Mu2_Pt" + suffix, ZMuons[1].Pt(), w, 2000, 0, 2000);
    }
    return;
  }
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
  std::vector<Lepton *> LepsTTune       = MakeLeptonPointerVector(Muons_HNL_v5,     Electrons,     param);
  double MET2ST = GetMET2ST(LepsT, AK4_JetColl, AK8_JetColl, METv);
  double MET2ST_TuneP = GetMET2ST(LepsTTune, AK4_JetColl, AK8_JetColl, METv);
 

  if(SameCharge(Muons_HNL_v3) && (MET2ST < 15) && ev.PassTrigger(TrigList_HNL_DblMu) && Muons_HNL_v3[0].Pt() > 20 && Muons_HNL_v3[1].Pt() > 10){
    DrawMuonPlotsHighPt(Muons_HNL_v3, ev,"SR_Muon_ULID_v3",   Electrons,param);
  }
  if(SameCharge(Muons_HPT_v3) && (MET2ST < 15) && ev.PassTrigger(TrigList_HNL_HighPtMu) && Muons_HPT_v3[0].Pt() > 55 && Muons_HPT_v3[1].Pt() > 10){
    DrawMuonPlotsHighPt(Muons_HPT_v3, ev,"SR_Muon_HighPt_v3",   Electrons,param);
  }

  //// Check Pt ratio

  if(Muons_HNL_v1.size() < 2) return;

  Particle LL, LL_TuneP;
  bool MisMatch = false;
  double MisMatchedPt = 0.0;
  double R = 0.0;

  if (SameCharge(Muons_HNL_v1)) {
    for (size_t imu = 0; imu < Muons_HNL_v1.size(); ++imu) {
      const auto& muon = Muons_HNL_v1[imu];

      if (muon.PassID(param.Muon_Tight_ID)) {
	LL += muon;
	LL_TuneP += muon.TuneP4();

	double tuneP_ratio = (muon.TuneP4().Pt() - muon.Pt()) / muon.TuneP4().Pt();
	FillHist("Inclusive_DiLep_N", tuneP_ratio, 1, 200, -1, 1);

	if (ComparePtTune(muon, 0.3)) {
	  std::cout << "\n"
		    << "Event " << event << " lumi = " << lumi 
		    << " run number = " << run << "\n"
		    << "index = " << imu << " MET2ST = " << MET2ST 
		    << " MET2ST_TuneP = " << MET2ST_TuneP << "\n"
		    << "Muon pt mismatch: pt = " << muon.Pt() 
		    << " tune pt = " << muon.TuneP4().Pt()
		    << " ratio = " << muon.TuneP4().Pt() / muon.Pt() << "\n"
		    << "Muon Eta = " << muon.Eta()
		    << " Muon Phi = " << muon.Phi() 
		    << " charge = " << muon.Charge() << "\n"
		    << "LL mass = " << LL.M() 
		    << " LL_TuneP mass = " << LL_TuneP.M() 
		    << std::endl;

	  if (!MisMatch) {
	    R = tuneP_ratio;
	    MisMatchedPt = muon.Pt();
	  }
	  MisMatch = true;
	}
      }
    }
  }
  if(R>0.3) std::cout <<  "MisMatchedPt = " << MisMatchedPt << endl;
  
  if(Muons_HNL_v1.size() != 2) return;
  Particle LL2 = Muons_HNL_v1[0] + Muons_HNL_v1[1];
  if(LL2.M() < 20) return;
  if(MET2ST > 10)  return;
  if(Muons_HNL_v1.at(1).Pt()  < 100) return;


  for(unsigned int imu = 0; imu < Muons_HNL_v1.size(); imu++){
    FillHist("Pull_Inclusive_DiLep_N", ((Muons_HNL_v1[imu].TuneP4().Pt()-Muons_HNL_v1[imu].Pt())/ Muons_HNL_v1[imu].TuneP4().Pt()) ,  1, 500, -1, 1 );

    if(AK8_JetColl.size() > 0)         FillHist("Pull_OS_SR1_N",((Muons_HNL_v1[imu].TuneP4().Pt()-Muons_HNL_v1[imu].Pt())/ Muons_HNL_v1[imu].TuneP4().Pt()) ,  1, 500, -1, 1 );
    else if(PassVBF(AK4_VBF_JetColl,LepsT,750))  FillHist("Pull_OS_SR2_N",((Muons_HNL_v1[imu].TuneP4().Pt()-Muons_HNL_v1[imu].Pt())/ Muons_HNL_v1[imu].TuneP4().Pt()) ,  1, 500, -1, 1 );
    else if(AK4_JetColl.size() < 2 && LepsT[0]->Pt() > 200) FillHist("Pull_OS_SR3_N",((Muons_HNL_v1[imu].TuneP4().Pt()-Muons_HNL_v1[imu].Pt())/ Muons_HNL_v1[imu].TuneP4().Pt()) ,  1, 500, -1, 1 );
    else if(AK4_JetColl.size() > 1)  FillHist("Pull_OS_SR3_N",((Muons_HNL_v1[imu].TuneP4().Pt()-Muons_HNL_v1[imu].Pt())/ Muons_HNL_v1[imu].TuneP4().Pt()) ,  1, 500, -1, 1 );
    
  }


  //  int nPtbins=12;
  //double Pt1bins[nPtbins+1] = { 20.,25., 30., 40., 50., 70., 100.,  150.,  200.,400.,600,1000,2000};

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
