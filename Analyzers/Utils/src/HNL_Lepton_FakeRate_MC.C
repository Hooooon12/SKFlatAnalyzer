#include "HNL_Lepton_FakeRate_MC.h"

void HNL_Lepton_FakeRate_MC::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

  TString AnalyzerPath=getenv("SKFlat_WD"), SKFlatV = getenv("SKFlatV");

}

void HNL_Lepton_FakeRate_MC::executeEvent(){

  Event ev = GetEvent();

  //************************************************///
  // setup list of IDs and jobs

  vector<AnalyzerParameter> VParameters;
  
  if(HasFlag("MCClosure")){

    vector<TString> LIDs = {"HNL_ULID_FO_v1_a",
                            "HNL_ULID_FO_v2_a",
                            "HNL_ULID_FO_v3_a",
                            "HNL_ULID_FO_v4_a",
                            "HNL_ULID_FO_v5_a",
                            "HNL_ULID_FO_v6_a",
                            "HNL_ULID_FO_v7_a",
                            "HNL_ULID_FO_v8_a",
                            "HNL_ULID_FO_v9_a",
                            "HNL_ULID_FO_v1_b",
                            "HNL_ULID_FO_v2_b",
                            "HNL_ULID_FO_v3_b",
                            "HNL_ULID_FO_v4_b",
                            "HNL_ULID_FO_v5_b",
                            "HNL_ULID_FO_v6_b",
                            "HNL_ULID_FO_v7_b",
                            "HNL_ULID_FO_v8_b",
                            "HNL_ULID_FO_v9_b",
                            "HNL_ULID_FO_v1_c",
                            "HNL_ULID_FO_v2_c",
                            "HNL_ULID_FO_v3_c",
                            "HNL_ULID_FO_v4_c",
                            "HNL_ULID_FO_v5_c",
                            "HNL_ULID_FO_v6_c",
                            "HNL_ULID_FO_v7_c",
                            "HNL_ULID_FO_v8_c",
                            "HNL_ULID_FO_v9_c",
                            "HNL_ULID_FO_v0"};

    vector<TString> NIDs;
    for(auto i : LIDs){
      TString aname = i;
      aname = aname.ReplaceAll("HNL_ULID_FO","HNL_LooseID_FO");
      NIDs.push_back(i);
    }

    for(unsigned int l=0 ; l < LIDs.size(); l++)   VParameters.push_back(SetupFakeParameter(AnalyzerParameter::Central,MuMu, HNL_LeptonCore::NormToXsec, {"MCClosure"},NIDs[l]+"_MC","HNL_ULID_"+GetYearString(),LIDs[l]));
    for(unsigned int l=0 ; l < LIDs.size(); l++)   VParameters.push_back(SetupFakeParameter(AnalyzerParameter::Central,EE, HNL_LeptonCore::NormToXsec, {"MCClosure"},NIDs[l]+"_MC","HNL_ULID_"+GetYearString(),LIDs[l]));

    goto RunJobs;
  }


  if(HasFlag("Standard")){
    //// RUN MISCJOBS
    vector<TString> LIDs = {"HNL_ULID_FO", "HNL_ULID_FOv3"};
    vector<TString> NIDs = {"HNL_LooseID_","HNL_LooseID_v3_" };
    for(unsigned int l=0 ; l < LIDs.size(); l++)   VParameters.push_back(SetupFakeParameter(AnalyzerParameter::Central,MuMu, HNL_LeptonCore::NormToXsec, {"MCProfile"},NIDs[l]+"_MC","HNL_ULID_"+GetYearString(),LIDs[l]));


    goto RunJobs;
  }

  if(HasFlag("CheckProfile")){
    //// Check MVA Profile  for PtParton

    vector<TString> LIDs = {"HNL_ULID_FO_v1_a",
                            "HNL_ULID_FO_v2_a",
                            "HNL_ULID_FO_v3_a",
                            "HNL_ULID_FO_v4_a",
                            "HNL_ULID_FO_v5_a",
                            "HNL_ULID_FO_v6_a",
                            "HNL_ULID_FO_v7_a",
                            "HNL_ULID_FO_v8_a",
                            "HNL_ULID_FO_v9_a",
                            "HNL_ULID_FO_v1_b",
                            "HNL_ULID_FO_v2_b",
                            "HNL_ULID_FO_v3_b",
                            "HNL_ULID_FO_v4_b",
                            "HNL_ULID_FO_v5_b",
                            "HNL_ULID_FO_v6_b",
                            "HNL_ULID_FO_v7_b",
                            "HNL_ULID_FO_v8_b",
                            "HNL_ULID_FO_v9_b",
                            "HNL_ULID_FO_v1_c",
                            "HNL_ULID_FO_v2_c",
                            "HNL_ULID_FO_v3_c",
                            "HNL_ULID_FO_v4_c",
                            "HNL_ULID_FO_v5_c",
                            "HNL_ULID_FO_v6_c",
                            "HNL_ULID_FO_v7_c",
                            "HNL_ULID_FO_v8_c",
                            "HNL_ULID_FO_v9_c",
                            "HNL_ULID_FO_v0"};

    vector<TString> NIDs;
    for(auto i : LIDs){
      TString aname = i;
      aname = aname.ReplaceAll("HNL_ULID_FO_","HNL_LooseID_FO");
      NIDs.push_back(i);
    }

    for(unsigned int l=0 ; l < LIDs.size(); l++)   VParameters.push_back(SetupFakeParameter(AnalyzerParameter::Central,MuMu, HNL_LeptonCore::NormToXsec, {"MCProfile"},NIDs[l]+"_MC","HNL_ULID_"+GetYearString(),LIDs[l]));  
    
    goto RunJobs;
    
  }
  if(HasFlag("CheckProfileEE")){
    /// Check MVA/Pt Profile for ptparton in Electrons

    vector<TString> LIDs = {"HNL_ULID_FO_v1_a",
                            "HNL_ULID_FO_v2_a",
                            "HNL_ULID_FO_v3_a",
                            "HNL_ULID_FO_v4_a",
                            "HNL_ULID_FO_v5_a",
                            "HNL_ULID_FO_v6_a",
                            "HNL_ULID_FO_v7_a",
                            "HNL_ULID_FO_v8_a",
                            "HNL_ULID_FO_v9_a",
                            "HNL_ULID_FO_v1_b",
                            "HNL_ULID_FO_v2_b",
                            "HNL_ULID_FO_v3_b",
                            "HNL_ULID_FO_v4_b",
                            "HNL_ULID_FO_v5_b",
                            "HNL_ULID_FO_v6_b",
                            "HNL_ULID_FO_v7_b",
                            "HNL_ULID_FO_v8_b",
                            "HNL_ULID_FO_v9_b",
                            "HNL_ULID_FO_v1_c",
                            "HNL_ULID_FO_v2_c",
                            "HNL_ULID_FO_v3_c",
                            "HNL_ULID_FO_v4_c",
                            "HNL_ULID_FO_v5_c",
                            "HNL_ULID_FO_v6_c",
                            "HNL_ULID_FO_v7_c",
                            "HNL_ULID_FO_v8_c",
                            "HNL_ULID_FO_v9_c",
                            "HNL_ULID_FO_v0"};


    vector<TString> NIDs;
    for(auto i : LIDs){
      TString aname = i;
      aname = aname.ReplaceAll("HNL_ULID_FO_","HNL_LooseID_FO");
      NIDs.push_back(i);
    }

    for(unsigned int l=0 ; l < LIDs.size(); l++)  VParameters.push_back(SetupFakeParameter(AnalyzerParameter::Central,EE, HNL_LeptonCore::NormToXsec, {"MCProfile"},NIDs[l]+"_MC","HNL_ULID_"+GetYearString(),LIDs[l]));
    
    goto RunJobs;
  }

  if(HasFlag("MCFakes")) {


    vector<TString> LIDs = {"HNL_ULID_FO_v1_a",
                            "HNL_ULID_FO_v2_a",
                            "HNL_ULID_FO_v3_a",
                            "HNL_ULID_FO_v4_a",
                            "HNL_ULID_FO_v5_a",
                            "HNL_ULID_FO_v6_a",
                            "HNL_ULID_FO_v7_a",
                            "HNL_ULID_FO_v8_a",
                            "HNL_ULID_FO_v9_a",
                            "HNL_ULID_FO_v1_b",
                            "HNL_ULID_FO_v2_b",
                            "HNL_ULID_FO_v3_b",
                            "HNL_ULID_FO_v4_b",
                            "HNL_ULID_FO_v5_b",
                            "HNL_ULID_FO_v6_b",
                            "HNL_ULID_FO_v7_b",
                            "HNL_ULID_FO_v8_b",
                            "HNL_ULID_FO_v9_b",
                            "HNL_ULID_FO_v1_c",
                            "HNL_ULID_FO_v2_c",
                            "HNL_ULID_FO_v3_c",
                            "HNL_ULID_FO_v4_c",
                            "HNL_ULID_FO_v5_c",
                            "HNL_ULID_FO_v6_c",
                            "HNL_ULID_FO_v7_c",
                            "HNL_ULID_FO_v8_c",
                            "HNL_ULID_FO_v9_c",
                            "HNL_ULID_FO_v0"};


    vector<TString> NIDs;
    for(auto i : LIDs){
      TString aname = i;
      aname = aname.ReplaceAll("HNL_ULID_FO_","HNL_LooseID_FO");
      NIDs.push_back(i);
    }

    for(unsigned int l=0 ; l < LIDs.size(); l++)     VParameters.push_back(SetupFakeParameter(AnalyzerParameter::Central,MuMu,HNL_LeptonCore::NoNorm,        {"MCFakes"},NIDs[l]+"_MC","HNL_ULID_"+GetYearString(),LIDs[l]));
    for(unsigned int l=0 ; l < LIDs.size(); l++)     VParameters.push_back(SetupFakeParameter(AnalyzerParameter::Central,EE,HNL_LeptonCore::NoNorm,        {"MCFakes"},NIDs[l]+"_MC","HNL_ULID_"+GetYearString(),LIDs[l]));



    goto RunJobs;
  }

  
 RunJobs:
  
  for(auto param : VParameters)  executeEventFromParameter(param);
  
}

void HNL_Lepton_FakeRate_MC::executeEventFromParameter(AnalyzerParameter param){

  if(HasFlag("MCClosure")){
    RunFake=true;
    param.FakeRateParam = "PtParton";
  }
  
  
  
  //************************************************///
  // setup event level objects
  Event ev = GetEvent();

  double weight = SetupWeight(ev, param);
  if(IsData) weight = 1;

  if(!PassMETFilter()) return;

  //  std::vector<Muon>     Test_Initial_loose_muons         = SelectMuons    ( param,"NoCut",     5,  2.4);

  param.Muon_FR_ID = param.Muon_Loose_ID;
  param.Electron_FR_ID = param.Electron_Loose_ID;

  std::vector<Electron> Initial_loose_electrons     = SelectElectrons( param,param.Electron_Loose_ID, 10, 2.5) ;
  std::vector<Muon>     Initial_loose_muons         = SelectMuons    ( param,param.Muon_Loose_ID,     5,  2.4);

  std::vector<Electron> Loose_Electrons;
  std::vector<Muon>     Loose_Muons;
  if(param.HasFlag("MCFakes") || param.HasFlag("MCProfile") ){
    for(auto ilep : Initial_loose_electrons) {
      if(ilep.IsFake()) Loose_Electrons.push_back(ilep);
    }
    for(auto ilep : Initial_loose_muons) {
      if(ilep.IsFake()) Loose_Muons.push_back(ilep);
    }
  }
  else{
    for(auto ilep : Initial_loose_electrons) Loose_Electrons.push_back(ilep);
    for(auto ilep : Initial_loose_muons)     Loose_Muons.push_back(ilep);
  }

  std::vector<Jet> jets_tmp     = SelectJets   ( param, "tight", 25., 2.7);
  std::vector<Jet> jets; 
  for(unsigned int ijet =0; ijet < jets_tmp.size(); ijet++){
    bool jetok=true;
    for(unsigned int iel=0 ; iel < Initial_loose_electrons.size(); iel++)   { if(jets_tmp[ijet].DeltaR(Initial_loose_electrons[iel]) < 0.4) jetok = false;}
    for(unsigned int iel=0 ; iel < Initial_loose_muons.size(); iel++)       { if(jets_tmp[ijet].DeltaR(Initial_loose_muons[iel]) < 0.4)     jetok = false;}
    if(jetok) jets.push_back(jets_tmp[ijet]);
  }
  if(param.Channel=="EE")   RunE(Loose_Electrons,Loose_Muons, jets,  param, weight);
  if(param.Channel=="MuMu") RunM(Loose_Electrons,Loose_Muons,  jets, param, weight);
  
}


void HNL_Lepton_FakeRate_MC::RunM(std::vector<Electron> loose_el,  std::vector<Muon> loose_mu, std::vector<Jet> jets,   AnalyzerParameter param,  float event_weight){

  Event ev = GetEvent();

  if(loose_mu.size() == 0) return;
  if(loose_el.size() > 0) return;
  
  std::vector<Lepton *> leps = MakeLeptonPointerVector(loose_mu,loose_el, param);
  
  std::vector<bool>    blepsT;
  for(auto ilep : loose_mu)   blepsT.push_back(ilep.PassID(param.Muon_Tight_ID));
  
  Particle METv = GetvMET("PuppiT1xyULCorr",param);

  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);

  if(AK4_BJetColl.size() > 0) return ;

  if(param.HasFlag("MCClosure")) {

    std::vector<Muon>     Initial_loose_muons         = SelectMuons    ( param, "HNL_ULID_FO_v0",     5,  2.4);
    if(Initial_loose_muons.size() != 2) return ;

    bool PassSS = SameCharge(loose_mu);
    
    ///// Require 2 Muon 20/10 GeV  
    if(loose_mu.size() == 2 && leps.size()==2 && leps[0]->Pt() > 20 && leps[1]->Pt() > 10){
      
      Particle LL = loose_mu[0] + loose_mu[1];
      if(LL.M() < 20) return;

      std::vector<Jet>    AK4_JetColl       = GetHNLJets(param.AK4JetColl,     param);
      
      TString EventString = "";

      if(!MCSample.Contains("Sherpa")){

	if(loose_mu[0].IsPrompt() && loose_mu[1].IsFake()) EventString = "PF";
	else if(loose_mu[1].IsPrompt() && loose_mu[0].IsFake()) EventString = "FP";
	else return;
      }
      else {
	bool Lep1Prompt=false;
	bool Lep2Prompt=false;
	
	for(unsigned int i=2; i<All_Gens.size(); i++){   
	  Gen gen = All_Gens.at(i);
	  
	  if(gen.Status() != 1) continue;
	  int mindex = All_Gens.at(i).MotherIndex();
	  int MotherPID = fabs(All_Gens.at(mindex).PID());
	  bool PromptLepMu = (MotherPID == 13)  || (MotherPID == 15);
	  while (MotherPID > 10 && MotherPID < 16){
	    mindex = All_Gens.at(mindex).MotherIndex();
	    MotherPID= fabs(All_Gens.at(mindex).PID());
	  }
	  
	  bool PromptLepMuFull = PromptLepMu &&  ((MotherPID == 21)  || (MotherPID < 6));

	  if(fabs(gen.PID()) == 13){
	    if(PromptLepMuFull && loose_mu[0].DeltaR(gen) < 0.1) Lep1Prompt=true;	    
	    if(PromptLepMuFull && loose_mu[1].DeltaR(gen) < 0.1) Lep2Prompt=true;	    
	  }
	}
	
	if(Lep1Prompt && !Lep2Prompt) EventString = "PF";
	else 	if(!Lep1Prompt && Lep2Prompt) EventString = "FP";
	else return;
      }
    
      int nPromptLep = (EventString == "PF") ? 0 : 1;
      int nFakeLep   = (EventString == "PF") ? 1 : 0;
      
      bool PromptPassID  = loose_mu[nPromptLep].PassLepID() ;
      bool FakePassID    = loose_mu[nFakeLep].PassLepID() ;
      
      if(!PromptPassID) return;
      
      //double PTPartonSF    = GetPtPartonSF(loose_mu[nFakeLep],param.Muon_Loose_ID,param);
      //double MVACut        = loose_mu[nFakeLep].MVAFakeCut(param.Muon_Tight_ID,GetYearString());
      //      double lep_ptparton  = (loose_mu[nFakeLep].PtParton(PTPartonSF,MVACut) < 80) ? loose_mu[nFakeLep].PtParton(PTPartonSF,MVACut) : 79;
      
	  
      double FR_30 = fakeEst->GetMuonFakeRate(param.Muon_Tight_ID, "MC_"+param.Muon_Loose_ID+"_MC_AJ30" ,"MC_"+param.FakeRateMethod,param.FakeRateParam,loose_mu[nFakeLep].fEta(), loose_mu[nFakeLep].PtMaxed(80), loose_mu[nFakeLep].LeptonFakeTagger() );
      double FR_40 = fakeEst->GetMuonFakeRate(param.Muon_Tight_ID, "MC_"+param.Muon_Loose_ID+"_MC_AJ40" ,"MC_"+param.FakeRateMethod,param.FakeRateParam,loose_mu[nFakeLep].fEta(), loose_mu[nFakeLep].PtMaxed(80), loose_mu[nFakeLep].LeptonFakeTagger() );
      double FR_60 = fakeEst->GetMuonFakeRate(param.Muon_Tight_ID, "MC_"+param.Muon_Loose_ID+"_MC_AJ60" ,"MC_"+param.FakeRateMethod,param.FakeRateParam,loose_mu[nFakeLep].fEta(), loose_mu[nFakeLep].PtMaxed(80), loose_mu[nFakeLep].LeptonFakeTagger() );
      

      map<TString,double> LepStatus;

      if(PassSS){
	LepStatus["SSLoose_30"] = FR_30*event_weight;
	LepStatus["SSLoose_40"] = FR_40*event_weight;
	LepStatus["SSLoose_60"] = FR_60*event_weight;
	if(FakePassID)       LepStatus["SSTight"] = event_weight;
	if(loose_mu[nFakeLep].fEta() < 1.5) {
	  LepStatus["SSLooseB_30"] = FR_30*event_weight;
	  if(FakePassID)   LepStatus["SSTightB"] = event_weight;
	}
	else{
	  LepStatus["SSLooseEC_30"] = FR_30*event_weight;
          if(FakePassID)   LepStatus["SSTightEC"] = event_weight;
	}
      }
    

      LepStatus["DiLepLoose_30"] = FR_30*event_weight;
      LepStatus["DiLepLoose_40"] = FR_40*event_weight;
      LepStatus["DiLepLoose_60"] = FR_60*event_weight;

      if(FakePassID)       LepStatus["DiLepTight"] = event_weight;
     

      for(auto istatus :  LepStatus){
	//cout << "FR = " << " eta = " << loose_mu[nFakeLep].fEta() << " pt = " << loose_mu[nFakeLep].PtMaxed(80) << "  FR="  << FR << endl;
	
	if(MCSample.Contains("DY"))  FillHist("MCClosure/"+param.Name+"/SampleSum_"+istatus.first,    0.1, istatus.second,3, 0, 3 ) ;
	if(MCSample.Contains("WJe")) FillHist("MCClosure/"+param.Name+"/SampleSum_"+istatus.first,   1.1, istatus.second,3, 0,  3 ) ;
	if(MCSample.Contains("TT"))  FillHist("MCClosure/"+param.Name+"/SampleSum_"+istatus.first,    2.1, istatus.second,3, 0, 3 ) ;

	FillHist("MCClosure/"+param.Name+"/TrailingPt_CloseJet_BScore_"+istatus.first,     loose_mu[nFakeLep].CloseJet_BScore(), event_weight,200, -1, 1 ) ;
	FillHist("MCClosure/"+param.Name+"/TrailingPt_CloseJet_BScore_Weighted_"+istatus.first, loose_mu[nFakeLep].CloseJet_BScore(), istatus.second,200, -1, 1 ) ;

 	FillHist("MCClosure/"+param.Name+"/TrailingPt_QCD_LFvsHF_"+ istatus.first, loose_mu[nFakeLep].HNL_MVA_Fake("QCD_LFvsHF_v5"), istatus.second,200, -1, 1 ) ;
	FillHist("MCClosure/"+param.Name+"/TrailingPt_QCD_BvsC_"+ istatus.first, loose_mu[nFakeLep].HNL_MVA_Fake("QCD_BvsC_v5"), istatus.second,200, -1, 1 ) ;

	FillHistogram("MCClosure/"+param.Name+"/FakeLeptonPt_"+ istatus.first,  loose_mu[nFakeLep].Pt(), istatus.second,"Pt80") ;
	FillHist("MCClosure/"+param.Name+"/MLL_"+ istatus.first, LL.M(), istatus.second,50, 0,400 ) ;
	FillHist("MCClosure/"+param.Name+"/MET_"+ istatus.first, METv.Pt(), istatus.second,50, 0,400 ) ;
	
	
      }
    }
    
    return;
  }

  // remove if muon              
  


  if(param.HasFlag("MCFakes")) {
    
    std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);
    std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);

    if(leps.size()==1){
      if (leps[0]->Pt() < 100){
	
	if(UseEvent(leps , jets, 25., METv, event_weight)){
          GetMuFakeRates("Pt",       leps[0], blepsT[0], param, jets, "MCFake_AJ25" ,  event_weight);
	  GetMuFakeRates("PtParton", leps[0], blepsT[0], param, jets, "MCFake_AJ25" ,  event_weight);
	}

	if(UseEvent(leps , jets, 30., METv, event_weight))      {
	  GetMuFakeRates("Pt",       leps[0], blepsT[0], param, jets, "MCFake_AJ30" ,  event_weight);
	  GetMuFakeRates("PtParton", leps[0], blepsT[0], param, jets, "MCFake_AJ30" ,  event_weight);
	}
	if(UseEvent(leps , jets, 60., METv, event_weight))       {
	  GetMuFakeRates("Pt",       leps[0], blepsT[0], param, jets, "MCFake_AJ60" ,  event_weight);
	  GetMuFakeRates("PtParton", leps[0], blepsT[0], param, jets, "MCFake_AJ60" ,  event_weight);
	}
	
	if(UseEvent(leps , jets, 40., METv, event_weight)){
	  GetMuFakeRates("Pt",       leps[0], blepsT[0], param, jets, "MCFake_AJ40" ,  event_weight);
	  GetMuFakeRates("PtParton", leps[0], blepsT[0], param, jets, "MCFake_AJ40" ,  event_weight);
	}
      }
    }

 
    return;
  }

  bool useevent40 = UseEvent(leps , jets, 40., METv, event_weight);
  
  if(param.HasFlag("MCProfile")){

    for(auto ilep : leps){
      
      double PtPartonUncorr = ilep->PtParton(1,ilep->MVAFakeCut(param.Muon_Tight_ID,GetYearString()));
      double W = event_weight;
      
      if(ilep->Pt() > 80) continue;
      
      FillHist(("MCProfile/"+param.Name + "/FakeCR"+param.GetSystType()+"_PtPartonUncorr").Data(),     PtPartonUncorr, W, 100,0., 200.);
      FillProf(("MCProfile/"+param.Name + "/FakeCR"+param.GetSystType()+"_MVA_PtPartonUncorr").Data(), ilep->HNL_MVA_Fake(), PtPartonUncorr, W, 200, -1, 1);
      /// Eta binning
      FillProf(("MCProfile/"+param.Name + "/FakeCR"+param.GetSystType()+"_MVA_"+ilep->sRegion()+"_PtPartonUncorr").Data(), ilep->HNL_MVA_Fake(), PtPartonUncorr, W, 100, -1, 1);
      FillProf(("MCProfile/"+param.Name + "/FakeCR"+param.GetSystType()+"_MVA_"+ilep->FakeTaggerStringLF()+"_"+ilep->sRegion()+"_PtPartonUncorr").Data(), ilep->HNL_MVA_Fake(), PtPartonUncorr, W, 100, -1, 1);
      FillProf(("MCProfile/"+param.Name + "/FakeCR"+param.GetSystType()+"_MVA_"+ilep->FakeTaggerStringHF()+"_"+ilep->sRegion()+"_PtPartonUncorr").Data(), ilep->HNL_MVA_Fake(), PtPartonUncorr, W, 100, -1, 1);
      FillProf(("MCProfile/"+param.Name + "/FakeCR"+param.GetSystType()+"_MVA_"+ilep->MotherJetFlavour() + "_"+ilep->sRegion()+"_PtPartonUncorr").Data(), ilep->HNL_MVA_Fake(), PtPartonUncorr, W, 100, -1, 1);

    }
  }

  if(param.HasFlag("Standard")){
    
    vector<TString> GENLABELS = {"pion","Kaon","quark","gluon","B","C","Other"};
    
    vector<TString> RegionLabel = {"MCFakeBins_","SSMCFakeBins_","FakeCRMCFakeBins_"};
    
    for(auto ir : RegionLabel){
      if(ir == "FakeCRMCFakeBins_" && !useevent40) continue;
      if(ir == "SSMCFakeBins_"){
	if( ! SameCharge(leps)) continue;
	if(leps.at(0)->Pt() < 20) continue;
	if(leps.at(1)->Pt() < 10) continue;
      }
      
      for(auto ilep : leps){
	
	TString lep_fake_tag = MatchGenDef(All_Gens, *ilep);
	if(lep_fake_tag.Contains("pi")) lep_fake_tag = "pion";
	else if(lep_fake_tag.Contains("K")) lep_fake_tag = "Kaon";
	else if(lep_fake_tag == "q") lep_fake_tag = "quark";
	else if(lep_fake_tag == "g") lep_fake_tag = "gluon";
	else if(lep_fake_tag.Contains("B")) lep_fake_tag = "B";
	else if(lep_fake_tag.Contains("C")) lep_fake_tag = "C";
	else if(lep_fake_tag.Contains("D")) lep_fake_tag = "C";
	else lep_fake_tag = "Other";
	
	
	for(int i=0; i < 2; i++){
	  
	  TString isLoT = "Loose";
	  if(i==1) {
	    isLoT = "Tight";
	    if(!ilep->PassLepID()) continue;
	  }
	  
	  FillCutflow(param, ir+"_Lep_Fake_type_"+isLoT  , event_weight, GENLABELS, lep_fake_tag);
	  
	  vector<TString> lables = GetGenList();
	  if (!(std::find(lables.begin(), lables.end(), lep_fake_tag) == lables.end()) ) FillCutflow(param, isLoT+"_All_Lep_Fake_type", event_weight, lables, MatchGenDef(All_Gens, *ilep));
	  
	  for(int j=0; j < 2; j++){
	    
	    TString Flav = "InclusiveFlavour_";
	    if(j==1) Flav = ilep->MotherJetFlavour()+"_";
	    
	    TString PtBinLabel = "";
	    if(ilep->Pt() < 20)  PtBinLabel = "PtBin1";
	    else if(ilep->Pt() < 40)  PtBinLabel = "PtBin2";
	    else if(ilep->Pt() < 60)  PtBinLabel = "PtBin3";
	    else  PtBinLabel = "PtBin4";
	    
	    FillCutflow(param, ir+"_"+Flav+"_"+PtBinLabel+"_Lep_Fake_type_" + isLoT , event_weight, GENLABELS, lep_fake_tag);
	    if(jets.size()== 0)           FillCutflow(param, ir+"_"+Flav+"_"+PtBinLabel+"_0J_Lep_Fake_type_" + isLoT  , event_weight, GENLABELS, lep_fake_tag);
	    else if(jets.size()== 1)      FillCutflow(param, ir+"_"+Flav+"_"+PtBinLabel+"_1J_Lep_Fake_type_" + isLoT  , event_weight, GENLABELS, lep_fake_tag);
	    else                          FillCutflow(param, ir+"_"+Flav+"_"+PtBinLabel+"_2pJ_Lep_Fake_type_" + isLoT  , event_weight, GENLABELS, lep_fake_tag);
	    
	    
	    for(int k=0; k < 2; k++){
	      
	      TString genlabel = "InclusiveGEN_";
	      if(k==1) genlabel = lep_fake_tag+"_";
	      
	      FillHist((ir+ genlabel+  Flav + isLoT+ "_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_MVA").Data(),    ilep->HNL_MVA_Fake(), event_weight, 100,-1., 1.);
	      FillHist((ir+ genlabel+  Flav + isLoT+ "_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_LFvsHF").Data(),    ilep->HNL_MVA_Fake("QCD_LFvsHF_v5"), event_weight, 100,-1., 1.);
	      FillHist((ir+ genlabel+  Flav + isLoT+ "_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_HFBvsHFC").Data(),    ilep->HNL_MVA_Fake("QCD_BvsC_v5"), event_weight, 100,-1., 1.);
	      FillHist((ir+ genlabel+  Flav + isLoT+ "_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_PtRatio").Data(),    ilep->CloseJet_Ptratio() ,   event_weight, 200, 0, 5 );
	      
	      if(jets.size()== 0) {
		FillHist((ir+ genlabel+  Flav + isLoT+ "_0J_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_MVA").Data(),    ilep->HNL_MVA_Fake(), event_weight, 100,-1., 1.);
		FillHist((ir+ genlabel+  Flav + isLoT+ "_0J_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_LFvsHF").Data(),    ilep->HNL_MVA_Fake("QCD_LFvsHF_v5"), event_weight, 100,-1., 1.);
		FillHist((ir+ genlabel+  Flav + isLoT+ "_0J_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_HFBvsHFC").Data(),    ilep->HNL_MVA_Fake("QCD_BvsC_v5"), event_weight, 100,-1., 1.);
		FillHist((ir+ genlabel+  Flav + isLoT+ "_0J_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_PtRatio").Data(),    ilep->CloseJet_Ptratio() ,   event_weight, 200, 0, 5 );
	      }
	      else if(jets.size()== 1) {
		FillHist((ir+ genlabel+  Flav + isLoT+ "_1J_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_MVA").Data(),    ilep->HNL_MVA_Fake(), event_weight, 100,-1., 1.);
		FillHist((ir+ genlabel+  Flav + isLoT+ "_1J_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_LFvsHF").Data(),    ilep->HNL_MVA_Fake("QCD_LFvsHF_v5"), event_weight, 100,-1., 1.);
		FillHist((ir+ genlabel+  Flav + isLoT+ "_1J_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_HFBvsHFC").Data(),    ilep->HNL_MVA_Fake("QCD_BvsC_v5"), event_weight, 100,-1., 1.);
		FillHist((ir+ genlabel+  Flav + isLoT+ "_1J_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_PtRatio").Data(),    ilep->CloseJet_Ptratio() ,   event_weight, 200, 0, 5 );
	      }
	      else{
		FillHist((ir+ genlabel+  Flav + isLoT+ "_2pJ_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_MVA").Data(),    ilep->HNL_MVA_Fake(), event_weight, 100,-1., 1.);
		FillHist((ir+ genlabel+  Flav + isLoT+ "_2pJ_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_LFvsHF").Data(),    ilep->HNL_MVA_Fake("QCD_LFvsHF_v5"), event_weight, 100,-1., 1.);
		FillHist((ir+ genlabel+  Flav + isLoT+ "_2pJ_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_HFBvsHFC").Data(),    ilep->HNL_MVA_Fake("QCD_BvsC_v5"), event_weight, 100,-1., 1.);
		FillHist((ir+ genlabel+  Flav + isLoT+ "_2pJ_"+PtBinLabel+"/FakeCR"+param.GetSystType()+"_PtRatio").Data(),    ilep->CloseJet_Ptratio() ,   event_weight, 200, 0, 5 );
	      }
	    }
	  }
	}
      }
    }
  }
  return;

} /// end function                      



double HNL_Lepton_FakeRate_MC::GetMuonMCFakeRate(Muon mu, TString Version){

  double feta = mu.fEta();
  double lpt = mu.Pt();

  if(GetEraShort() == "2018"){
    if(Version == "AJ40"){
      if(feta < 1.5){
        if(lpt < 15) return 0.104;
        if(lpt < 20) return 0.205;
        if(lpt < 25) return 0.198;
        if(lpt < 30) return 0.197;
        if(lpt < 35) return 0.225;
        if(lpt < 40) return 0.205;
        if(lpt < 60) return 0.21;
        return 0.175;
      }
      else{
        if(lpt < 15) return 0.075;
        if(lpt < 20) return 0.187;
        if(lpt < 25) return 0.175;
        if(lpt < 30) return 0.174;
        if(lpt < 35) return 0.206;
        if(lpt < 40) return 0.171;
        if(lpt < 60) return 0.17;
        return 0.142;

      }
    }
    else if(Version == "AJ40_Weighted"){
      if(feta < 1.5){
        if(lpt < 15) return 0.104;
        if(lpt < 20) return 0.195;
        if(lpt < 25) return 0.178;
        if(lpt < 30) return 0.165;
        if(lpt < 35) return 0.183;
        if(lpt < 40) return 0.16;
        if(lpt < 60) return 0.154;
        return 0.148;
      }
      else{
        if(lpt < 15) return 0.076;
        if(lpt < 20) return 0.180;
        if(lpt < 25) return 0.158;
        if(lpt < 30) return 0.145;
        if(lpt < 35) return 0.159;
        if(lpt < 40) return 0.127;
        if(lpt < 60) return 0.105;
        return 0.1;

      }
    }
  }


  return 0;
}


double HNL_Lepton_FakeRate_MC::GetElectronMCFakeRate(Electron el, TString Version){
  
  double feta = el.fEta();
  double lpt = el.Pt();
  
  if(GetEraShort() == "2018"){
    if(Version == "AJ40"){
      if(feta < 1.5){
	if(lpt < 15) return 0.144;
	if(lpt < 23) return 0.145;
	if(lpt < 30) return 0.125;
	if(lpt < 35) return 0.115;
	if(lpt < 40) return 0.1;
	if(lpt < 60) return 0.095;
	return 0.08;
      }
      else{
	if(lpt < 15) return 0.178;
        if(lpt < 23) return 0.175;
        if(lpt < 30) return 0.145;
        if(lpt < 35) return 0.13;
        if(lpt < 40) return 0.11;
        if(lpt < 60) return 0.125;
        return 0.085;
      }
    }
    else if(Version == "AJ40_Weighted"){
      if(feta < 1.5){
        if(lpt < 15) return 0.144;
        if(lpt < 23) return 0.128;
        if(lpt < 30) return 0.116;
        if(lpt < 35) return 0.085;
        if(lpt < 40) return 0.08;
        if(lpt < 60) return 0.09;
        return 0.13;
      }
      else{
	if(lpt < 15) return 0.153;
        if(lpt < 23) return 0.185;
        if(lpt < 30) return 0.125;
        if(lpt < 35) return 0.125;
        if(lpt < 40) return 0.1;
        if(lpt < 60) return 0.125;
        return 0.09;
      }
    }
  }
  

  return 0;
}



void HNL_Lepton_FakeRate_MC::RunE( std::vector<Electron> loose_el, std::vector<Muon> loose_mu, std::vector<Jet> jets, AnalyzerParameter param,  float event_weight){

  if(IsData){
    if(this->DataStream == "DoubleMuon") return;
    if(this->DataStream == "SingleMuon") return;
  }

  Event ev = GetEvent();

  if(loose_el.size() == 0) return;
 
  // remove if muon 
  if(loose_mu.size() > 0) return;

  Particle METv = GetvMET("PuppiT1xyULCorr",param);

  std::vector<Lepton *> leps = MakeLeptonPointerVector(loose_mu,loose_el, param);

  std::vector<bool>    blepsT;
  for(auto ilep : loose_el)   blepsT.push_back(ilep.PassID(param.Electron_Tight_ID));

  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);

  if(AK4_BJetColl.size() > 0) return ;

  if(param.HasFlag("MCClosure")) {

    std::vector<Electron> Initial_loose_electrons     = SelectElectrons( param, "HNL_ULID_FO_v0", 10, 2.5) ;


    if(Initial_loose_electrons.size() != 2) return ;

    bool PassSS = SameCharge(loose_el);

    ///// Require 2 El 20/10 GeV                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
    if(loose_el.size() == 2 && leps.size()==2 && leps[0]->Pt() > 25 && leps[1]->Pt() > 15){

      Particle LL = loose_el[0] + loose_el[1];
      if(LL.M() < 20) return;

      std::vector<Jet>    AK4_JetColl       = GetHNLJets(param.AK4JetColl,     param);

      TString EventString = "";

      if(!MCSample.Contains("Sherpa")){

        if(loose_el[0].IsPrompt() && loose_el[1].IsFake()) EventString = "PF";
        else if(loose_el[1].IsPrompt() && loose_el[0].IsFake()) EventString = "FP";
        else return;
      }
      else {
        bool Lep1Prompt=false;
        bool Lep2Prompt=false;

        for(unsigned int i=2; i<All_Gens.size(); i++){
          Gen gen = All_Gens.at(i);

          if(gen.Status() != 1) continue;
          int mindex = All_Gens.at(i).MotherIndex();
          int MotherPID = fabs(All_Gens.at(mindex).PID());
          bool PromptLepEl = (MotherPID == 11) || (MotherPID == 15);

          while (MotherPID > 10 && MotherPID < 16){
            mindex = All_Gens.at(mindex).MotherIndex();
            MotherPID= fabs(All_Gens.at(mindex).PID());
          }

          bool PromptLepElFull = PromptLepEl &&  ((MotherPID == 21)  || (MotherPID < 6));

          if(fabs(gen.PID()) == 11){
            if(PromptLepElFull && loose_el[0].DeltaR(gen) < 0.1) Lep1Prompt=true;
            if(PromptLepElFull && loose_el[1].DeltaR(gen) < 0.1) Lep2Prompt=true;
          }
        }

        if(Lep1Prompt && !Lep2Prompt) EventString = "PF";
        else    if(!Lep1Prompt && Lep2Prompt) EventString = "FP";
        else return;
      }

      int nPromptLep = (EventString == "PF") ? 0 : 1;
      int nFakeLep   = (EventString == "PF") ? 1 : 0;

      bool PromptPassID  = loose_el[nPromptLep].PassLepID() ;
      bool FakePassID    = loose_el[nFakeLep].PassLepID() ;

      if(!PromptPassID) return;

      //double PTPartonSF    = GetPtPartonSF(loose_el[nFakeLep],param.Electron_Loose_ID,param);
      //double MVACut        = loose_el[nFakeLep].MVAFakeCut(param.Electron_Tight_ID,GetYearString());
      //double PtCorr        = (loose_el[nFakeLep].CalcMVACone(MVACut)  < 80 )       ? loose_el[nFakeLep].CalcMVACone( MVACut)  : 79;
      //      double lep_ptparton  = (loose_el[nFakeLep].PtParton(PTPartonSF,MVACut) < 80) ? loose_el[nFakeLep].PtParton(PTPartonSF,MVACut) : 79;


      double FR_30 = fakeEst->GetElectronFakeRate(param.Electron_Tight_ID, "MC_"+param.Electron_Loose_ID+"_MC_AJ30" ,"MC_"+param.FakeRateMethod,param.FakeRateParam,loose_el[nFakeLep].fEta(), loose_el[nFakeLep].PtMaxed(80), loose_el[nFakeLep].LeptonFakeTagger() );
      double FR_40 = fakeEst->GetElectronFakeRate(param.Electron_Tight_ID, "MC_"+param.Electron_Loose_ID+"_MC_AJ40" ,"MC_"+param.FakeRateMethod,param.FakeRateParam,loose_el[nFakeLep].fEta(), loose_el[nFakeLep].PtMaxed(80), loose_el[nFakeLep].LeptonFakeTagger() );
      double FR_60 = fakeEst->GetElectronFakeRate(param.Electron_Tight_ID, "MC_"+param.Electron_Loose_ID+"_MC_AJ60" ,"MC_"+param.FakeRateMethod,param.FakeRateParam,loose_el[nFakeLep].fEta(), loose_el[nFakeLep].PtMaxed(80), loose_el[nFakeLep].LeptonFakeTagger() );

      map<TString,double> LepStatus;
      
      if(PassSS){
	LepStatus["SSLoose_30"] = FR_30*event_weight;
        LepStatus["SSLoose_40"] = FR_40*event_weight;
        LepStatus["SSLoose_60"] = FR_60*event_weight;
        if(FakePassID)       LepStatus["SSTight"] = event_weight;

	if(loose_el[nFakeLep].fEta() < 1.5) {
	  LepStatus["SSLooseB_30"] = FR_30*event_weight;
          if(FakePassID)   LepStatus["SSTightB"] = event_weight;
	}
	else{
          LepStatus["SSLooseEC_30"] = FR_30*event_weight;
          if(FakePassID)   LepStatus["SSTightEC"] = event_weight;
	}
      }
      LepStatus["DiLepLoose_30"] = FR_30*event_weight;
      LepStatus["DiLepLoose_40"] = FR_40*event_weight;
      LepStatus["DiLepLoose_60"] = FR_60*event_weight;
      if(FakePassID)       LepStatus["DiLepTight"] = event_weight;


      for(auto istatus :  LepStatus){
        if(MCSample.Contains("DY")) FillHist("MCClosure/"+param.Name+"/SampleSum_"+istatus.first,    0.1, istatus.second,3, 0, 3 ) ;
        if(MCSample.Contains("WJe")) FillHist("MCClosure/"+param.Name+"/SampleSum_"+istatus.first,   1.1, istatus.second,3, 0, 3 ) ;
        if(MCSample.Contains("TT")) FillHist("MCClosure/"+param.Name+"/SampleSum_"+istatus.first,    2.1, istatus.second,3, 0, 3 ) ;

        FillHist("MCClosure/"+param.Name+"/TrailingPt_CloseJet_BScore_"+istatus.first,     loose_el[nFakeLep].CloseJet_BScore(), event_weight,200, -1, 1 ) ;
        FillHist("MCClosure/"+param.Name+"/TrailingPt_CloseJet_BScore_Weighted_"+istatus.first, loose_el[nFakeLep].CloseJet_BScore(), istatus.second,200, -1, 1 ) ;

        FillHist("MCClosure/"+param.Name+"/TrailingPt_QCD_LFvsHF_"+ istatus.first, loose_el[nFakeLep].HNL_MVA_Fake("QCD_LFvsHF_v5"), istatus.second,200, -1, 1 ) ;
        FillHist("MCClosure/"+param.Name+"/TrailingPt_QCD_BvsC_"+ istatus.first, loose_el[nFakeLep].HNL_MVA_Fake("QCD_BvsC_v5"), istatus.second,200, -1, 1 ) ;

   	FillHistogram("MCClosure/"+param.Name+"/FakeLeptonPt_"+ istatus.first,  loose_el[nFakeLep].Pt(), istatus.second,"Pt80") ;
	FillHist("MCClosure/"+param.Name+"/MLL_"+ istatus.first, LL.M(), istatus.second,50, 0,200 ) ;
        FillHist("MCClosure/"+param.Name+"/MET_"+ istatus.first, METv.Pt(), istatus.second,50, 0,200 ) ;

	
      }
    }

    return;
  }

  // remove if muon                                                                                                                                                                                                                                                                       


  if(param.HasFlag("MCFakes")) {
    
    std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);
    std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);

    if(leps.size()==1){
      if (leps[0]->Pt() < 100){

        if(UseEvent(leps , jets, 25., METv, event_weight)){
          GetElFakeRates("Pt",       leps[0], blepsT[0], param, jets, "MCFake_AJ25" ,  event_weight);
          GetElFakeRates("PtParton", leps[0], blepsT[0], param, jets, "MCFake_AJ25" ,  event_weight);
          GetElFakeRates("PtCorr",   leps[0], blepsT[0], param, jets, "MCFake_AJ25" ,  event_weight);
        }

        if(UseEvent(leps , jets, 30., METv, event_weight))      {
          GetElFakeRates("Pt",       leps[0], blepsT[0], param, jets, "MCFake_AJ30" ,  event_weight);
          GetElFakeRates("PtParton", leps[0], blepsT[0], param, jets, "MCFake_AJ30" ,  event_weight);
          GetElFakeRates("PtCorr",   leps[0], blepsT[0], param, jets, "MCFake_AJ30" ,  event_weight);
        }
        if(UseEvent(leps , jets, 60., METv, event_weight))       {
          GetElFakeRates("Pt",       leps[0], blepsT[0], param, jets, "MCFake_AJ60" ,  event_weight);
          GetElFakeRates("PtParton", leps[0], blepsT[0], param, jets, "MCFake_AJ60" ,  event_weight);
          GetElFakeRates("PtCorr",   leps[0], blepsT[0], param, jets, "MCFake_AJ60" ,  event_weight);
        }

        if(UseEvent(leps , jets, 40., METv, event_weight)){
          GetElFakeRates("Pt",       leps[0], blepsT[0], param, jets, "MCFake_AJ40" ,  event_weight);
          GetElFakeRates("PtParton", leps[0], blepsT[0], param, jets, "MCFake_AJ40" ,  event_weight);
          GetElFakeRates("PtCorr",   leps[0], blepsT[0], param, jets, "MCFake_AJ40" ,  event_weight);
        }
      }
    }
  }
  

  if(param.HasFlag("MCProfile")){
    
    for(auto ilep : leps){
      if(ilep->Pt() > 80) continue;

      double PtPartonUncorr = ilep->PtParton(1,ilep->MVAFakeCut(param.Electron_Tight_ID,GetYearString()));
      double W = event_weight;
      
      FillHist(("MCProfile/"+param.Name + "/FakeCR"+param.GetSystType()+"_PtPartonUncorr").Data(),PtPartonUncorr, W, 40,0., 200.);
      FillProf(("MCProfile/"+param.Name + "/FakeCR"+param.GetSystType()+"_MVA_PtPartonUncorr").Data(), ilep->HNL_MVA_Fake("HNL"), PtPartonUncorr, W, 40, -1, 1);
      FillProf(("MCProfile/"+param.Name + "/FakeCR"+param.GetSystType()+"_MVA_"+ilep->etaRegionString()+"_PtPartonUncorr").Data(), ilep->HNL_MVA_Fake("HNL"), PtPartonUncorr, W, 40, -1, 1);
      FillProf(("MCProfile/"+param.Name + "/FakeCR"+param.GetSystType()+"_MVA_3bin_"+ilep->GetEtaRegion("3bin")+"_PtPartonUncorr").Data(), ilep->HNL_MVA_Fake("HNL"), PtPartonUncorr, W, 40, -1, 1);
    }
    return;
  }

  
  return;
  


} /// end function RunE


bool HNL_Lepton_FakeRate_MC::UseEvent(std::vector<Lepton *> leps ,  std::vector< Jet> jets, float awayjetcut, Particle MET, float wt){

  bool useevent = false;
  if(leps.size() != 1) return false;
  
  Double_t MT=0;
  Double_t METdphi=0;
  for(unsigned int w = 0; w < leps.size();  w++){
    METdphi = TVector2::Phi_mpi_pi(leps.at(w)->Phi()-MET.Phi());
    //    MT = sqrt(2.* leps.at(w)->Et()*MET.Pt() * (1 - cos( METdphi)));
    MT = sqrt(2.* 35*MET.Pt() * (1 - cos( METdphi)));
    if(( (MET.Pt() < 40) && (MT < 25.)) ) {

      for (unsigned int ielT=0; ielT < leps.size(); ielT++){
        for(unsigned int ij=0; ij < jets.size(); ij++){
          if(jets.at(ij).Pt() < awayjetcut) continue;
	  //       float dphi =fabs(TVector2::Phi_mpi_pi(leps.at(ielT)->Phi()- jets.at(ij).Phi()));
	  if( (jets.at(ij).ChargedEmEnergyFraction()) > 0.65)  continue;

          if(leps.at(ielT)->DeltaR(jets.at(ij))> 0.7){
            //if((jets.at(ij).Pt() /  leps.at(ielT)->Pt() ) < 1.2) continue;
            useevent = true;
          }
	}
      }
    }
  }

  return useevent;
}


void HNL_Lepton_FakeRate_MC::GetElFakeRates(TString Method, Lepton* lep,bool blepsT,  AnalyzerParameter param,  std::vector<Jet> jets,  TString tag,float event_weight){

  bool IsMuon=(lep->LeptonFlavour() == Lepton::MUON);
  if(IsMuon) return;

  if(param.Name.Contains("HNL_")){
    if(Method.Contains("PtCone")) return;
  }
  else{
    if(!Method.Contains("PtCone")) return;
  }

  double   isocut  = (Method == "PtCone") ? GetIsolationCutFromID(*lep, param.Electron_Tight_ID) : 0. ;
  TString  LooseID =  param.Electron_Loose_ID ;
  TString  TightID =  param.Electron_Tight_ID ;
  double MVACut = lep->MVAFakeCut(TightID,GetYearString());


  Event ev = GetEvent();
  TString triggerslist_8="HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v";   
  TString triggerslist_12="HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v";
  TString triggerslist_23="HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v";

  bool El8PD= true;
  bool EL12PD= true;
  bool EL23PD= true;

  bool pass_8  = ev.PassTrigger(triggerslist_8) && El8PD;
  bool pass_12 = ev.PassTrigger(triggerslist_12) && EL12PD ;
  bool pass_23 = ev.PassTrigger(triggerslist_23) && EL23PD;

  int For_HLT_PFJet30_v=0;
  for(auto ij : jets){
    if(ij.Pt() > 40.) For_HLT_PFJet30_v++;
  }
  if(!For_HLT_PFJet30_v) return;

  float lep_pt (0);
  
  float weight_ptcorr=event_weight;

  TString ptname = "pt_eta";
  TString PtHist = "pt"; /// Used to set binning                                                                                                                                                                  

  if(Method == "Pt"){
    lep_pt      =  (lep->Pt() < 80) ?  lep->Pt() : 79;

    if(lep_pt > 25){
      if(!pass_23) return;
    }
    else  if(lep_pt > 15){
      if(!pass_12) return;
    }
    else  if(lep_pt > 10){
      if(!pass_8) return;
    }
  }
  if(Method == "PtCone"){
    PtHist ="ptcone";
    lep_pt =  ( lep->CalcPtCone(lep->RelIso(), isocut) < 80) ?  lep->Pt() : 79;
    ptname = "ptcone_eta";
    if(lep_pt > 40){
      if(lep->Pt() < 25) return;
      if(!pass_23) return;
    }
    else if(lep_pt > 23){
      if(lep->Pt() < 15) return;
      if(!pass_12) return;
    }
    else if(lep_pt > 10){
      if(lep->Pt() < 9.5) return;
      if(!pass_8) return;

    }
    else return ;
  }
  else if(Method == "PtParton"){
    PtHist ="ptcone";
    ptname = "ptparton_eta";
    double PTPartonSF = GetPtPartonSF(*lep, LooseID,param);

    lep_pt      =  (lep->PtParton(PTPartonSF,MVACut) < 80) ?  lep->PtParton(PTPartonSF, MVACut) : 79;

    FillProf((param.Name + "_FakeCR40_MVA_PtParton").Data(), lep->HNL_MVA_Fake(), lep_pt, event_weight, 100, -1, 1);

    if(lep_pt > 40){
      if(lep->Pt() < 25) return;
      if(!pass_23) return;
    }
    else if(lep_pt > 23){
      if(lep->Pt() < 10) return;
      if(!pass_12) return;
    }
    else if(lep_pt > 10){
      if(lep->Pt() < 9.5) return;
      if(!pass_8) return;

    }
    else return ;
    
  }
  if(Method == "PtCorr"){

    PtHist ="ptcone";
    ptname = "ptcorr_eta";
    lep_pt      = (lep->CalcMVACone(MVACut)  < 80) ? lep->CalcMVACone(MVACut)  : 79;
    

    if(lep_pt > 40){
      if(lep->Pt() < 25) return;
      if(!pass_23) return;
    }
    else if(lep_pt > 23){
      if(lep->Pt() < 10) return;
      if(!pass_12) return;
    }
    else if(lep_pt > 10){
      if(lep->Pt() < 9.5) return;
      if(!pass_8) return;

    }
    else return ;

  }
  if(Method == "MotherPt"){
    PtHist ="ptcone";
    ptname = "MJpt_eta";
    lep_pt      = lep->MotherJetPt();


    if(lep_pt > 40){
      if(lep->Pt() < 25) return;
      if(!pass_23) return;
    }
    else if(lep_pt > 23){
      if(lep->Pt() < 10) return;
      if(!pass_12) return;
    }
    else if(lep_pt > 10){
      if(lep->Pt() < 9.5) return;
      if(!pass_8) return;

    }
    else return ;

  }

  float lep_eta     = lep->fEta(); /// returns |eta| OR for el |scEta|
  float lep_mva_lfvshf  = lep->HNL_MVA_Fake("QCD_LFvsHF_v5");
  float lep_mva_bvsc    = lep->HNL_MVA_Fake("QCD_BvsC_v5");
  float lep_blscore     = lep->CloseJet_BScore();
  float lep_cbscore     = lep->CloseJet_CvsBScore();
  float lep_clscore     = lep->CloseJet_CvsLScore();
  TString lepEtaRegion  = lep->etaRegionString();
  TString lepRegion     = (lep->IsBB()) ? "BB" : "EC";

  TString L_prefix = "Fake_"+Method+"_Loose"+tag;
  TString T_prefix = "Fake_"+Method+"_Tight"+tag;

  for(int ilep = 0 ; ilep < 2; ilep++)  {

    TString prefix = (ilep==0) ? L_prefix : T_prefix;
    if((ilep==1) && !(blepsT)) continue;
    
    prefix = param.Name + "/"+Method + "/"+prefix;

    TString Ptlab = "p_{T} (GeV)";

    if(lep_pt >10){
      FillHistogram((prefix + "_"+ptname).Data(),            lep_pt, lep_eta,  weight_ptcorr, "FR_"+lep->GetFlavour()+"_"+ PtHist , "FR_Eta", Ptlab);
      FillHistogram((prefix + "_FinerBins_"+ptname).Data(),            lep_pt, lep_eta,  weight_ptcorr, "FR_FB_"+lep->GetFlavour()+"_"+ PtHist , "FR_Eta", Ptlab);
      FillHistogram((prefix + "_pt").Data(),                 lep_pt,  weight_ptcorr, "FR_"+lep->GetFlavour()+"_"+ PtHist, Ptlab);
      FillHistogram((prefix + "_pt_"+ lepEtaRegion).Data(),  lep_pt,  weight_ptcorr, "FR_"+lep->GetFlavour()+"_"+ PtHist, Ptlab);
      FillHistogram((prefix + "_pt_"+ lepRegion).Data(),     lep_pt,  weight_ptcorr, "FR_"+lep->GetFlavour()+"_"+ PtHist, Ptlab);
      FillHistogram((prefix + "_eta").Data(),                lep_eta, weight_ptcorr ,"FR_Eta","#eta");
      FillHist((prefix + "_eta_fine").Data(),           lep_eta, weight_ptcorr , 50, 0, 2.5,"#eta");
      FillHist((prefix + "_lep_mva_lfvshf").Data(),   lep_mva_lfvshf,  weight_ptcorr, 50, -1., 1.);
      FillHist((prefix + "_lep_mva_bvsc").Data(),   lep_mva_bvsc,  weight_ptcorr, 50, -1., 1.);
      FillHist((prefix + "_blscore").Data(), lep_blscore,    weight_ptcorr, 50, -1., 1.);
      FillHist((prefix + "_cbscore").Data(), lep_cbscore,    weight_ptcorr, 50, -1., 1.);
      FillHist((prefix + "_clscore").Data(), lep_clscore,    weight_ptcorr, 50, -1., 1.);
      

    }
  }
  return;

}

void HNL_Lepton_FakeRate_MC::GetMuFakeRates(TString Method, Lepton*  lep, bool blepsT,  AnalyzerParameter param,  std::vector<Jet> jets,  TString tag,float event_weight){
  

  bool IsMuon=(lep->LeptonFlavour() == Lepton::MUON);
  if(!IsMuon) return;

  if(param.Name.Contains("HNL_")){
    if(Method.Contains("PtCone")) return;
  }
  else{
    if(!Method.Contains("PtCone")) return;
  }
  
  FillHist("GetMuFakeRates",   1, event_weight, 1, 0,1);


  double   isocut  = (Method == "PtCone") ? GetIsolationCutFromID(*lep, param.Muon_Tight_ID) : 0.;
  TString  LooseID =  param.Muon_Loose_ID ;
  TString  TightID =  param.Muon_Tight_ID ;
  double MVACut = lep->MVAFakeCut(TightID,GetYearString());
  
  ///// TRIGGER CONDITIONS

  Event ev = GetEvent();  

  TString triggerslist_3="HLT_Mu3_PFJet40_v";  TString triggerslist_8="HLT_Mu8_TrkIsoVVL_v";  TString triggerslist_17="HLT_Mu17_TrkIsoVVL_v";
  bool Mu3PD= true;
  bool Mu8PD= true;
  bool Mu17PD= true;

  bool pass_3 = ev.PassTrigger(triggerslist_3) && Mu3PD;
  bool pass_8 = ev.PassTrigger(triggerslist_8) && Mu8PD;
  bool pass_17 = ev.PassTrigger(triggerslist_17) && Mu17PD;

  int For_HLT_Mu3_PFJet40_v=0;
  for(auto ij : jets){
    if(ij.Pt() > 50.) For_HLT_Mu3_PFJet40_v++;
  }

  TString ptname = "pt";
  TString PtHist = "pt"; /// Used to set binning
  
  float weight_ptcorr=event_weight;
  float lep_pt (0);
  if(Method == "Pt"){
    lep_pt      =  (lep->Pt() < 80) ?  lep->Pt() : 79;
    if(lep_pt > 20){
      if(!pass_17) return;
    }
    else  if(lep_pt > 10){
      if(!pass_8) return;
    }
    else  if(lep_pt > 5){
      if(!pass_3) return;
      if(For_HLT_Mu3_PFJet40_v==0) return;
    }
  }
  if(Method == "PtCone"){
    lep_pt      =  ( lep->CalcPtCone(lep->RelIso(), isocut) < 80) ?  lep->Pt() : 79;
    ptname = "ptcone";
    if(lep_pt > 30){
      if(lep->Pt() < 20) return;
      if(!pass_17) return;
    }
    else if(lep_pt > 15){
      if(lep->Pt() < 10) return;
      if(!pass_8) return;
    }
    else if(lep_pt > 5){
      if(lep->Pt() < 5) return;
      if(!pass_3) return;
      if(For_HLT_Mu3_PFJet40_v==0) return;

    }
    else return ;
  }
  else if(Method == "PtParton"){
    
    ptname = "ptparton";
    double PTPartonSF = GetPtPartonSF(*lep, LooseID,param);
    
    //cout << param.Name << "  PTPartonSF = " << PTPartonSF << " MVACut = " << MVACut << endl;
    
    lep_pt      =  (lep->PtParton(PTPartonSF,MVACut) < 80) ?  lep->PtParton(PTPartonSF, MVACut) : 79;
    
    FillProf((param.Name + "_Fake"+tag+"_MVA_PtParton").Data(), lep->HNL_MVA_Fake(), lep_pt, event_weight, 100, -1, 1);
    FillProf((param.Name + "_Fake"+tag+"_MVA_PtParton_"+lep->MotherJetFlavour() ).Data(), lep->HNL_MVA_Fake(), lep_pt, event_weight, 100, -1, 1);
    
    if(lep_pt > 30){
      if(lep->Pt() < 20) return;
      if(!pass_17) return;
    }
    else if(lep_pt > 15){
      if(lep->Pt() < 10) return;
      if(!pass_8) return;
    }
    else if(lep_pt > 5){
      if(lep->Pt() < 5) return;
      if(!pass_3) return;
      if(For_HLT_Mu3_PFJet40_v==0) return;

    }
   else return ;
    
  }
  else if(Method == "PtParton20"){

    ptname = "ptparton";
    double PTPartonSF = GetPtPartonSF(*lep, LooseID,param);

    lep_pt      =  (lep->PtParton(PTPartonSF,MVACut,2) < 80) ?  lep->PtParton(PTPartonSF, MVACut,2) : 79;

    FillProf((param.Name + "_Fake"+tag+"_MVA_PtParton").Data(), lep->HNL_MVA_Fake(), lep_pt, event_weight, 100, -1, 1);
    FillProf((param.Name + "_Fake"+tag+"_MVA_PtParton_"+lep->MotherJetFlavour() ).Data(), lep->HNL_MVA_Fake(), lep_pt, event_weight, 100, -1, 1);

    if(lep_pt > 40){
      if(lep->Pt() < 20) return;
      if(!pass_17) return;
    }
    else if(lep_pt > 20){
      if(lep->Pt() < 10) return;
      if(!pass_8) return;
    }
    else if(lep_pt > 10){
      if(lep->Pt() < 5) return;
      if(!pass_3) return;
      if(For_HLT_Mu3_PFJet40_v==0) return;

    }
    else return ;

  }

  if(Method == "PtCorr"){

    ptname = "ptcorr";
    lep_pt      = (lep->CalcMVACone( MVACut)  < 80) ? lep->CalcMVACone(MVACut)  : 79;
    
    if(lep_pt > 30){
      if(lep->Pt() < 20) return;
      if(!pass_17) return;
    }
    else if(lep_pt > 15){
      if(lep->Pt() < 10) return;
      if(!pass_8) return;
    }
    else if(lep_pt > 5){
      if(lep->Pt() < 5) return;
      if(!pass_3) return;
      if(For_HLT_Mu3_PFJet40_v==0) return;
    }
    else return ;
  }
  if(Method == "PtCorr20"){

    ptname = "ptcorr20";
    lep_pt      = (lep->CalcMVACone( MVACut,2)  < 80) ? lep->CalcMVACone(MVACut,2)  : 79;

    if(lep_pt > 40){
      if(lep->Pt() < 20) return;
      if(!pass_17) return;
    }
    else if(lep_pt > 20){
      if(lep->Pt() < 10) return;
      if(!pass_8) return;
    }
    else if(lep_pt > 10){
      if(lep->Pt() < 5) return;
      if(!pass_3) return;
      if(For_HLT_Mu3_PFJet40_v==0) return;
    }
    else return ;
  }


  if(Method == "MotherPt"){

    ptname = "MJpt";
    lep_pt      = lep->MotherJetPt();
    
    if(lep_pt > 30){
      if(lep->Pt() < 20) return;
      if(!pass_17) return;
    }
    else if(lep_pt > 15){
      if(lep->Pt() < 10) return;
      if(!pass_8) return;
    }
    else if(lep_pt > 5){
      if(lep->Pt() < 5) return;
      if(!pass_3) return;
      if(For_HLT_Mu3_PFJet40_v==0) return;
    }
    else return ;
  }

  float lep_eta     = lep->fEta(); /// returns |eta| OR for el |scEta|
  float lep_mva_lfvshf  = lep->HNL_MVA_Fake("QCD_LFvsHF_v5");
  float lep_mva_bvsc    = lep->HNL_MVA_Fake("QCD_BvsC_v5");
  float lep_blscore     = lep->CloseJet_BScore();
  float lep_cbscore     = lep->CloseJet_CvsBScore();
  float lep_clscore     = lep->CloseJet_CvsLScore();
  TString lepEtaRegion  = lep->etaRegionString();
  TString lepRegion     = (lep->IsBB()) ? "BB" : "EC";

  TString L_prefix = "Fake_"+Method+"_Loose"+tag;
  TString T_prefix = "Fake_"+Method+"_Tight"+tag;

  for(int ilep = 0 ; ilep < 2; ilep++)  {

    TString prefix = (ilep==0) ? L_prefix : T_prefix;

    if((ilep==1) && !(blepsT)) continue;
    
    prefix = param.Name + "/"+Method + "/"+prefix;

    TString Ptlab = "p_{T} (GeV)";

    if(lep_pt >10){
      FillHistogram((prefix + "_"+ptname+"_eta").Data(),            lep_pt, lep_eta,  weight_ptcorr, "FR_"+lep->GetFlavour()+"_"+PtHist , "FR_Eta", Ptlab);
      FillHistogram((prefix + "_FinerBins_"+ptname+"_eta").Data(),            lep_pt, lep_eta,  weight_ptcorr, "FR_FB_"+lep->GetFlavour()+"_"+PtHist , "FR_Eta", Ptlab);
      
      FillHistogram((prefix + "_"+ptname).Data(),                 lep_pt,  weight_ptcorr, "FR_"+lep->GetFlavour()+"_"+PtHist, Ptlab);
      FillHistogram((prefix + "_"+ptname+"_"+ lepRegion).Data(),     lep_pt,  weight_ptcorr, "FR_"+lep->GetFlavour()+"_"+PtHist, Ptlab);
      FillHistogram((prefix + "_eta").Data(),                lep_eta, weight_ptcorr ,"FR_Eta","#eta");
      FillHist((prefix + "_eta_fine").Data(),           lep_eta, weight_ptcorr , 50, 0, 2.5,"#eta");
      FillHist((prefix + "_lep_mva_lfvshf").Data(),   lep_mva_lfvshf,  weight_ptcorr, 50, -1., 1.);
      FillHist((prefix + "_lep_mva_bvsc").Data(),   lep_mva_bvsc,  weight_ptcorr, 50, -1., 1.);
      FillHist((prefix + "_blscore").Data(), lep_blscore,    weight_ptcorr, 50, -1., 1.);
      FillHist((prefix + "_cbscore").Data(), lep_cbscore,    weight_ptcorr, 50, -1., 1.);
      FillHist((prefix + "_clscore").Data(), lep_clscore,    weight_ptcorr, 50, -1., 1.);
      
    }
  }
  return;
}


HNL_Lepton_FakeRate_MC::HNL_Lepton_FakeRate_MC(){
  
}
 
HNL_Lepton_FakeRate_MC::~HNL_Lepton_FakeRate_MC(){


}


void HNL_Lepton_FakeRate_MC::FillEventCutflow(int charge_i, int cf,float wt, TString cut,   TString label){


  return;

}


void HNL_Lepton_FakeRate_MC::FillCutFlow(bool IsCentral, TString suffix, TString histname, double weight){

  if(IsCentral){

    FillHist(suffix+"/"+histname, 0., weight, 1, 0., 1.);

  }

}

