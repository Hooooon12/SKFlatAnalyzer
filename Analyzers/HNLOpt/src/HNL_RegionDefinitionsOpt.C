#include "HNL_RegionDefinitionsOpt.h"


/*
  
  Mirror of HNL_RegionDefinitions but allows var and cuts to be modified

 */



void HNL_RegionDefinitionsOpt::RunAllSignalRegionsOpt(HNL_LeptonCore::ChargeType qq, 
						   std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Tau> TauColl,
						   std::vector<Jet> AllJetColl ,  std::vector<Jet> JetCollLoose,std::vector<Jet> JetColl, std::vector<Jet> VBF_JetColl,std::vector<FatJet>  AK8_JetColl, 
						   std::vector<Jet> B_JetColl,std::vector<Jet> B_JetCollSR1, 
						   Event ev,   Particle METv, AnalyzerParameter param,   float weight_ll){

  vector<HNL_LeptonCore::Channel> channels = {EE,MuMu};

  
  for(auto dilep_channel : channels){

    double  weight_channel = weight_ll;
    
    /// SelectChannel separates OS/SS samples and EE/MM since signales have all processes in sample
    if(MCSample.Contains("Type")&& !SelectChannel(dilep_channel)) continue;
 
    if (param.Name.Contains("MuOpt") && dilep_channel != MuMu) continue;
    if (param.Name.Contains("ElOpt") && dilep_channel != EE) continue;

    if(run_Debug)cout << "ID Mu  " << param.Muon_Tight_ID << " " << param.Muon_FR_ID << endl;
    if(run_Debug)cout << "ID El  " << param.Electron_Tight_ID << " " << param.Electron_FR_ID << endl;

    std::vector<Lepton *> leps       = MakeLeptonPointerVector(muons,electrons,param);
    
    TString channel_string = GetChannelString(dilep_channel);

    AnalyzerParameter param_channel = param;

    param_channel.Name =  channel_string + "/" + param_channel.Name;

    FillCutflow(HNL_LeptonCore::ChannelDepInc, weight_channel, GetChannelString(dilep_channel) +"_NoCut",param_channel);//DEf                                                                    

    HNL_LeptonCore::SearchRegion Regions = HNL_LeptonCore::MuonSRSummary;
    if (dilep_channel == EE)     Regions = HNL_LeptonCore::ElectronSRSummary;
    if (dilep_channel == EMu)    Regions = HNL_LeptonCore::ElectronMuonSRSummary;

    FillCutflow(Regions,  weight_channel, "Inclusive",param_channel);


    //PassEventTypeFilter filters out events in MC if runconv and no conversion is present and vice versa
    if(!PassGenMatchFilter(leps, param_channel)) continue;

    FillCutflow(Regions,  weight_channel, "GenMatch",param_channel);

    // CheckLeptonFlavourForChannel checks 2 leptons of type according rto dilep_channel are present in event and passes Trigger looses cuts
    if(!CheckLeptonFlavourForChannel(dilep_channel, leps)) continue;
    FillCutflow(Regions,  weight_channel, "CheckLeptonFlavourForChannel",param_channel);


    /// Weight MC based on corr
    if(!IsDATA && dilep_channel != MuMu)  EvalElectronIDWeight(electrons, param_channel,weight_channel);
    if(!IsDATA && dilep_channel != EE)    EvalMuonIDWeight(muons, param_channel,weight_channel);

    if(!PassMETFilter()) return;
    FillCutflow(Regions,  weight_channel, "METFilter",param_channel);

   
    std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(muons_veto,electrons_veto,param);
    
    if(leps.size() ==2)  FillCutflow(HNL_LeptonCore::ChannelDepDilep, weight_channel, GetChannelString(dilep_channel) +"_Dilep",param_channel);//Def
    
    if(RunCF){
      if(dilep_channel == MuMu) continue;
      if(IsData && SameCharge(leps)) continue;
      if(!IsData && !SameCharge(leps)) continue;
      if(IsData)weight_channel = GetCFWeightElectron(leps, param_channel,-1);
      if(IsData)FillWeightHist(param_channel.Name+"/CFWeight",weight_channel);
    }
    else{
      if(!SameCharge(leps)) continue;
    }

    FillCutflow(Regions,  weight_channel, "CFCut",param_channel);

    if(RunFake&& IsData){
      
      if(run_Debug)cout << "ID2 Mu  " << param_channel.Muon_Tight_ID << " " << param_channel.Muon_FR_ID << endl;
      if(run_Debug)cout << "ID2 El  " << param_channel.Electron_Tight_ID << " " << param_channel.Electron_FR_ID << endl;

      weight_channel = GetFakeWeight(leps, param_channel);

      if(run_Debug)cout << "Fake " << weight_channel << endl;
      FillWeightHist(param_channel.Name+"/FakeWeight",weight_channel);
    }
    
    // Run PRESEL + SR1-3                                                                                                                                                           
    //bool run_ORTrigger = false;


    HNL_LeptonCore::SearchRegion LimitRegions = HNL_LeptonCore::MuonSR;
    if (dilep_channel == EE) LimitRegions =HNL_LeptonCore::ElectronSR;
    if (dilep_channel == EMu) LimitRegions =HNL_LeptonCore::ElectronMuonSR;

    HNL_LeptonCore::SearchRegion LimitRegionsBDT = HNL_LeptonCore::MuonSRBDT;
    if (dilep_channel == EE) LimitRegionsBDT =HNL_LeptonCore::ElectronSRBDT;
    if (dilep_channel == EMu) LimitRegionsBDT =HNL_LeptonCore::ElectronMuonSRBDT;


    if(!PassPreselection(dilep_channel,qq, leps, leps_veto, TauColl, JetColl, VBF_JetColl, AK8_JetColl, B_JetColl,ev, METv ,param_channel,"", weight_channel)) continue;

    FillCutflow(Regions,  weight_channel, "Preselection",param_channel);

    vector<TString> Masses    = {"600", "700","800","1000", "2000" };
    vector<TString> BDTMasses = {"100","400","500"};


    ////////// SR1 

    TString  SR1_Version = "Binning_Version2";
   
    bool SR1_MassBinning_EXO17 (false);
    bool SR1_MassBinning_Version1 (false);

    if(param_channel.SRConfig.Contains("OptSR")){
      if(param_channel.SRConfig.Contains("SigR1_v1"))    SR1_Version= "Binning_Version1";
      if(param_channel.SRConfig.Contains("SigR1_v2"))    SR1_Version= "Binning_Version2";
      if(param_channel.SRConfig.Contains("SigR1_v3"))    SR1_Version= "Binning_Version3";
      if(param_channel.SRConfig.Contains("SigR1_MD_v1")) SR1_Version= "MassDep_Version1";
      if(param_channel.SRConfig.Contains("SigR1_MD_EXO17")) SR1_Version= "MassDep_EXO17028";
      SR1_MassBinning_Version1 = (param_channel.SRConfig.Contains("SigR1_MD_v1")) ? true :  false;
      SR1_MassBinning_EXO17 = (param_channel.SRConfig.Contains("SigR1_MD_EXO17")) ? true :  false;
    }


    //// SR2 
    TString SR2_Version = "Binning_Version2";
    if(param_channel.SRConfig.Contains("OptSR")){
      if(param_channel.SRConfig.Contains("SigR2_v1")) SR2_Version= "Binning_Version1";
      if(param_channel.SRConfig.Contains("SigR2_v2")) SR2_Version= "Binning_Version2";
    }

    /// SR3 
    bool SR3_MassBinning_EXO17 (false);
    TString SR3_Version = "Binning_Version5";

    if(param_channel.SRConfig.Contains("OptSR")){
      if(param_channel.SRConfig.Contains("SigR3_v1")) SR3_Version= "Binning_Version1";
      if(param_channel.SRConfig.Contains("SigR3_v2")) SR3_Version= "Binning_Version2";
      if(param_channel.SRConfig.Contains("SigR3_v3")) SR3_Version= "Binning_Version3";
      if(param_channel.SRConfig.Contains("SigR3_v4")) SR3_Version= "Binning_Version4";
      if(param_channel.SRConfig.Contains("SigR3_v5")) SR3_Version= "Binning_Version5";
      if(param_channel.SRConfig.Contains("SigR3_v6")) SR3_Version= "Binning_Version6";
      if(param_channel.SRConfig.Contains("SigR3_MD_EXO17")) SR3_Version= "MassDep_EXO17028";
      SR3_MassBinning_EXO17 = (param_channel.SRConfig.Contains("SigR3_MD_EXO17")) ? true :  false;
    }

    
    bool MassDependentBinning = (SR1_MassBinning_Version1 || SR1_MassBinning_EXO17 || SR3_MassBinning_EXO17);
    if(AK8_JetColl.size() > 0) {

      // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
      ///@@@@@@@@@@@@@@@@@@@     SR 1 [AK8] @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
      // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
      
      FillCutflow(Regions,  weight_channel, "AK8",param_channel);
      
      //// plotSR1_version3 means mN bin is done for each mN
      if(MassDependentBinning){
	for(auto mass : Masses){
	  TString SRbinMD= RunSignalRegionAK8String (mass, dilep_channel,qq, leps, leps_veto, TauColl, JetColl, AK8_JetColl, B_JetCollSR1,ev, METv ,param_channel,SR1_Version, weight_channel) ;
	  if(SRbinMD != "false")  FillLimitInput(LimitRegions, weight_channel, SRbinMD,"LimitInput/"+param_channel.Name+"/"+mass);
	  
	  if(mass == "1000") {
	    if(SRbinMD != "false")  FillCutflow(Regions,  weight_channel, "SigReg1",param_channel);
	    else FillCutflow(Regions,  weight_channel, "SigReg1Fail",param_channel);
	  }
	} // Mass loop
      } /// Mass binning
      else{

	TString SRbin= RunSignalRegionAK8String ("---", dilep_channel,qq, leps, leps_veto, TauColl, JetColl, AK8_JetColl, B_JetCollSR1,ev, METv ,param_channel,SR1_Version, weight_channel) ;

	////// Fill Limit plot                                                                                                                                          
	if(SRbin != "false"){
	  FillLimitInput(LimitRegions, weight_channel, SRbin,"LimitInput/"+param_channel.Name);
	  for(auto bdt_mass : BDTMasses) FillLimitInput(LimitRegionsBDT, weight_channel, SRbin,"LimitInputBDT/"+param_channel.Name+"/"+bdt_mass);
	}

	///// Fill Cutflow
	if(SRbin != "false")  FillCutflow(Regions,  weight_channel, "SigReg1",param_channel);
	else FillCutflow(Regions,  weight_channel, "SigReg1Fail",param_channel);
      }
    }
    else{


      // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                                 
      ///@@@@@@@@@@@@@@@@@@@     SR 2 [VBF] @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                             
      // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                                           


      
      TString SRbin = RunSignalRegionWWString( dilep_channel,qq, leps, leps_veto,  TauColl, JetCollLoose, VBF_JetColl,  AK8_JetColl, B_JetColl,ev, METv, param_channel,  SR2_Version, weight_channel);
      
      if(SRbin != "false"){

	if(MassDependentBinning){
	  for(auto mass : Masses)FillLimitInput(LimitRegions, weight_channel, SRbin,"LimitInput/"+param_channel.Name+"/"+mass);
	}
	else FillLimitInput(LimitRegions, weight_channel, SRbin,"LimitInput/"+param_channel.Name);

	for(auto bdt_mass : BDTMasses) FillLimitInput(LimitRegionsBDT, weight_channel, SRbin,"LimitInputBDT/"+param_channel.Name+"/"+bdt_mass);
	FillCutflow(Regions,  weight_channel, "SigReg2",param_channel);
      }
      else{

	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  
	///@@@@@@@@@@@@@@@@@@@     SR 3 [AK4] @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                         
	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 

	FillCutflow(Regions,  weight_channel, "SigReg3",param_channel);

	TString SRbin  = RunSignalRegionAK4String (dilep_channel,qq, leps, leps_veto, TauColl, JetColl, AK8_JetColl, B_JetColl, ev, METv ,param_channel,SR3_Version, weight_channel);

	TString SRBDT100 = RunSignalRegionAK4StringBDT("100", dilep_channel, leps,JetColl,B_JetColl, METv , ev);   
	TString SRBDT400 = RunSignalRegionAK4StringBDT("400", dilep_channel, leps,JetColl,B_JetColl, METv, ev);   
	TString SRBDT500 = RunSignalRegionAK4StringBDT("500", dilep_channel, leps,JetColl,B_JetColl, METv, ev);   
	
	if(SRBDT100 != "false") FillLimitInput(LimitRegionsBDT, weight_channel, SRBDT100,"LimitInputBDT/"+param_channel.Name+"/100");
	if(SRBDT400 != "false") FillLimitInput(LimitRegionsBDT, weight_channel, SRBDT400,"LimitInputBDT/"+param_channel.Name+"/400");
	if(SRBDT500 != "false") FillLimitInput(LimitRegionsBDT, weight_channel, SRBDT500,"LimitInputBDT/"+param_channel.Name+"/500");
	
	
	if(SRbin != "false") {

	  if(MassDependentBinning){
	    for(auto mass : Masses) {
	      if(SR3_MassBinning_EXO17) {
		if(JetColl.size() > 1) {
		  SRbin = GetEXO17028_SignalRegion1Bin( dilep_channel,  mass ,  JetColl, AK8_JetColl, leps, METv);
		  if(SRbin != "false") FillLimitInput(LimitRegions, weight_channel, SRbin,"LimitInput/"+param_channel.Name+"/"+mass);

		}
		else FillLimitInput(LimitRegions, weight_channel, SRbin,"LimitInput/"+param_channel.Name+"/"+mass);

	      } 
	      else  FillLimitInput(LimitRegions, weight_channel, SRbin,"LimitInput/"+param_channel.Name+"/"+mass);
	    }
	  }
	  else   FillLimitInput(LimitRegions, weight_channel, SRbin, "LimitInput/"+param_channel.Name);

	  
	  FillCutflow(Regions,  weight_channel, "SigReg3Pass",param_channel);
	}
	else           FillCutflow(Regions,  weight_channel, "SigReg3Fail",param_channel);

      }
      
    }
  }
  return;
}



bool  HNL_RegionDefinitionsOpt::PassPreselection(HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType qq, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, vector<Tau> TauColl,  std::vector<Jet> JetColl, std::vector<Jet> VBF_JetColl,std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param, TString TriggerOption ,  float w){

  // ONLY CODE SS PRESLECTION 
  
  // APPLY PT CUTS
  /*
    ee channel 25/10
    emu   25/5
    mm    20/5
    
    THIS IS APPLIED IN PassTriggerSelection
    
  */
  
  if(run_Debug) cout << "HNL_RegionDefinitionsOpt::PassPreselection " << GetChannelString(channel) <<  endl;

  // Make sure events contain 2 leps                                                                                                       
  // Make sure correct leptons are used                                                                                                                                      

  if (! (  PassMultiDatasetTriggerSelection(channel, ev, leps,"Dilep", "HighPt") || PassMultiDatasetTriggerSelection(channel, ev, leps,"Dilep", "Lep"))) return false;
  
  FillCutflow(HNL_LeptonCore::ChannelDepTrigger, w, GetChannelString(channel) +"_MultiTrigger", param);//Def
  
  if(param.SRConfig.Contains("Trig1OR")){
    if( !(PassMultiDatasetTriggerSelection(channel, ev, leps,"Dilep","HighPt")))  return false;
  }
  else if(param.SRConfig.Contains("Trig2OR")){
    if(!(PassMultiDatasetTriggerSelection(channel, ev, leps,"Dilep","Lep")))  return false;
  }
  else if(param.SRConfig.Contains("Trig3OR")){
    if( !((PassMultiDatasetTriggerSelection(channel, ev, leps,"Dilep","Lep")) || PassMultiDatasetTriggerSelection(channel, ev, leps,"Dilep", "HighPt"))) return false;
  }
  else      if (!PassTriggerSelection(channel, ev, leps,"Dilep")) return false;
  
  w *= HNLZvtxSF(channel);

  
  FillCutflow(HNL_LeptonCore::ChannelDepTrigger, w, GetChannelString(channel) +"_Trigger", param);//Def


  if(run_Debug) cout << "HNL_RegionDefinitionsOpt::PassTriggerSelection " << GetChannelString(channel) <<  endl;

  
  int nel_hem(0);
  if (channel==EE){
    if (DataEra=="2018"){
      for(auto iel : leps){
	if (iel->Eta() < -1.25){
          if((iel->Phi() < -0.82) && (iel->Phi() > -1.62)) nel_hem++;
	}
      }
    }
  }

  if(nel_hem > 0) return false;


  // Make sure events contain 2 leps
  if (leps_veto.size() != 2) return false;

  
  // CUT ON MASS OF LL PAIR

  Particle ll =  (*leps[0]) + (*leps[1]);
  
  // VETO Z PEAK IN EE CHANNEL

  double mz_cut = 10.;
  if (param.SRConfig.Contains("PreSel_MZ_20_"))   mz_cut = 20.;
  if (param.SRConfig.Contains("PreSel_MZ_15_"))   mz_cut = 15.;
  if (param.SRConfig.Contains("PreSel_MZ_10_"))    mz_cut = 10.;
  if (param.SRConfig.Contains("PreSel_MZ_5_"))    mz_cut = 5.;
  if (param.SRConfig.Contains("PreSel_MZ_0_"))    mz_cut = 0.;

  if (channel==EE  && (fabs(ll.M()-M_Z) < mz_cut)) return false;

  if(ll.M() < 10) return false; // TO_CHECK: IS 20 BEST OPTION

  // REMOVE 0 Jet EVENTS
  //int njets     = JetColl.size() + AK8_JetColl.size() + VBF_JetColl.size();
 

  Fill_RegionPlots(param,"Preselection" , TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);

  FillCutflow(HNL_LeptonCore::ChannelDepPresel, w, GetChannelString(channel) +"_Presel", param);//Def
  
  return true;
}


bool  HNL_RegionDefinitionsOpt::RunSignalRegionAK8(HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq , std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto , std::vector<Tau> TauColl, std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param, TString PostLabel ,  float w){


  TString SR1String = RunSignalRegionAK8String("",channel, qq, leps, leps_veto, TauColl,JetColl,AK8_JetColl,B_JetColl,ev, METv, param, PostLabel,w);
  
  if(SR1String == "false") return false;
  else return true;
  
}


/// Return TString for Limit bin

TString HNL_RegionDefinitionsOpt::RunSignalRegionAK8String(TString mass, HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq , std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto , std::vector<Tau> TauColl, std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param, TString Modifier ,  float w){

  
  bool HasTightTau=false;
  for(auto itau: TauColl){
    bool HasTightTau_n=true;
    for (auto ilep : leps) if (ilep->DeltaR(itau) < 0.4) HasTightTau_n=false;
    for (auto ijet : AK8_JetColl) if (ijet.DeltaR(itau) < 1.)  HasTightTau_n=false;
    for (auto ijet : JetColl) if (ijet.DeltaR(itau) < 0.5)  HasTightTau_n=false;
    if(HasTightTau_n) HasTightTau=true;
  }
  
  TString PNMass = param.Name+"_"+mass;
  if (mass == "---") PNMass = param.Name;

  FillCutflow(HNL_LeptonCore::SR1, w, "SR1_Init",PNMass);
  
  if (leps_veto.size() != 2) return "false";
  FillCutflow(HNL_LeptonCore::SR1,w, "SR1_lep_pt",PNMass);
  if(AK8_JetColl.size() == 0) return "false";
  
  if(qq==Plus && leps[0]->Charge() < 0) return "false";
  if(qq==Minus && leps[0]->Charge() > 0) return "false";

  FillCutflow(HNL_LeptonCore::ChannelDepSR1, w, GetChannelString(channel) +"_SR1", "ChannelCutFlow/"+PNMass);//Def                                          

  FillCutflow(HNL_LeptonCore::SR1, w, "SR1_lep_charge",PNMass);
  FillCutflow(HNL_LeptonCore::SR1, w, "SR1_dilep_mass",PNMass);

  double ST = GetST(leps, JetColl, AK8_JetColl, METv);
  double HT = GetHT(JetColl, AK8_JetColl);
  double met2_st = pow(METv.Pt(),2.)/ ST;

  double met_cut = 10.;
  if (param.SRConfig.Contains("SR1MET_20"))   met_cut = 20.;
  if (param.SRConfig.Contains("SR1MET_18"))   met_cut = 18.;
  if (param.SRConfig.Contains("SR1MET_17"))   met_cut = 17.;
  if (param.SRConfig.Contains("SR1MET_15"))   met_cut = 15.;
  if (param.SRConfig.Contains("SR1MET_12"))   met_cut = 12.;
  if (param.SRConfig.Contains("SR1MET_10"))   met_cut = 10.;
  if (param.SRConfig.Contains("SR1MET_9"))   met_cut = 9.;
  if (param.SRConfig.Contains("SR1MET_8"))   met_cut = 8.;
  if (param.SRConfig.Contains("SR1MET_7"))   met_cut = 7.;
  if (param.SRConfig.Contains("SR1MET_6"))   met_cut = 6.;
    
  bool PassHMMet    = (met2_st < met_cut);
  bool PassBJetMVeto = (B_JetColl.size()==0);

  double mass_cut = 450.;

  if (param.SRConfig.Contains("SR1MLLJJ_0p3"))   mass_cut = 300.;
  if (param.SRConfig.Contains("SR1MLLJJ_0p4"))   mass_cut = 400.;
  if (param.SRConfig.Contains("SR1MLLJJ_0p45"))   mass_cut = 450.;
  if (param.SRConfig.Contains("SR1MLLJJ_0p5"))   mass_cut = 500.;
  if (param.SRConfig.Contains("SR1MLLJJ_0p6"))   mass_cut = 600.;
  if (param.SRConfig.Contains("SR1MLLJJ_0p7"))   mass_cut = 700.;
  if (param.SRConfig.Contains("SR1MLLJJ_0p8"))   mass_cut = 800.;
  if (param.SRConfig.Contains("SR1MLLJJ_0p9"))   mass_cut = 900.;
  if (param.SRConfig.Contains("SR1MLLJJ_1"))   mass_cut = 1000.;
 
  double LowerMassSR1WmassCut = mass_cut;
  
  TString signal_region1 = "HNL_SR1";
  
  Fill_RegionPlots(param,"InclusiveR1" , TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w); ///// MASS DEP????
  
  double dPhil2_l = fabs(TVector2::Phi_mpi_pi( ( (AK8_JetColl[0] + *leps[1]).Phi() - (leps[0]->Phi() ))));
  
  double pt_cut = 100.;
  if(DataEra=="2018")  pt_cut = 200.;
  if (param.SRConfig.Contains("SR1PT_80"))   pt_cut = 80.;
  if (param.SRConfig.Contains("SR1PT_90"))   pt_cut = 90.;
  if (param.SRConfig.Contains("SR1PT_100"))   pt_cut = 100.;
  if (param.SRConfig.Contains("SR1PT_110"))   pt_cut = 110.;
  if (param.SRConfig.Contains("SR1PT_120"))   pt_cut = 120.;
  if (param.SRConfig.Contains("SR1PT_140"))   pt_cut = 140.;
  if (param.SRConfig.Contains("SR1PT_160"))   pt_cut = 160.;
  if (param.SRConfig.Contains("SR1PT_180"))   pt_cut = 180.;
  if (param.SRConfig.Contains("SR1PT_200"))   pt_cut = 200.;

  FillCutflow(HNL_LeptonCore::SR1, w, "SR1_1AK8",PNMass);

  float dijetmass_tmp=999.;
  float dijetmass=9990000.;

  int m=-999;
  for(UInt_t emme=0; emme<AK8_JetColl.size(); emme++){
    dijetmass_tmp = AK8_JetColl[emme].SDMass();
    if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
      dijetmass = dijetmass_tmp;
      m = emme;
    }
  }

  Particle N1cand = AK8_JetColl[m] + *leps[0];
  Particle Wcand = AK8_JetColl[m] + *leps[0]+*leps[1];
  double MN1 = (N1cand.M() > 2000.) ? 1999. : N1cand.M();
  
  if(!PassHMMet) return "false";
  FillCutflow(HNL_LeptonCore::SR1, w, "SR1_MET",PNMass);


  if(Wcand.M()  < LowerMassSR1WmassCut ) return "false";
  FillCutflow(HNL_LeptonCore::SR1, w, "SR1_Wmass",PNMass);


  if(!PassBJetMVeto) return "false";
  
  FillCutflow(HNL_LeptonCore::SR1, w, "SR1_bveto",PNMass);
  
  
  if(Modifier.Contains("Binning_Version1")){
    FillCutflow(HNL_LeptonCore::SR1, w, "SR1_PNTagger",PNMass);
    double dRl2_l2_cut = 2.;

    if(dPhil2_l < dRl2_l2_cut) return "false";

    FillCutflow(HNL_LeptonCore::SR1, w, "SR1_DphiN_Wlep",PNMass);

    if(HasTightTau) return "false";

    FillCutflow(HNL_LeptonCore::SR1, w, "SR1_TauVeto",PNMass);

    if( ( HT/leps[0]->Pt()) > 4.) return "false";

  }
  else{

    FillCutflow(HNL_LeptonCore::SR1, w, "SR1_PNTagger",PNMass);
    FillCutflow(HNL_LeptonCore::SR1, w, "SR1_DphiN_Wlep",PNMass);
    FillCutflow(HNL_LeptonCore::SR1, w, "SR1_TauVeto",PNMass);
  }
    
  
  FillCutflow(HNL_LeptonCore::SR1, w, "SR1_HTPt",PNMass);
  Fill_RegionPlots(param,"InclusiveSRHM1" , TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
  

  if(Modifier.Contains("Binning_Version3")){

    if(leps[0]->Pt() < pt_cut)  return "SR1_MNbin1";
    FillCutflow(HNL_LeptonCore::SR1, w, "SR1_LepPt",PNMass);

    if(leps[1]->Pt() < 20)      return "SR1_MNbin1";
    FillCutflow(HNL_LeptonCore::SR1, w, "SR1_Lep2Pt",PNMass);
      
    int nSRbins=7;
    double ml1jbins[nSRbins] = { 0., 500., 600, 750.,900, 1500, 2000.};
    for(int ibin=1; ibin < nSRbins; ibin++){
      if(MN1 < ml1jbins[ibin]) return "SR1_MNbin"+to_string(ibin+1);
    }
    
    cout << "Error in SR1 binning " << Modifier << endl;
    exit(EXIT_FAILURE); 
    
  }
  
  else if (Modifier.Contains("Binning_Version2")){

    TString SR1Bin="SR1_MNbin1";

    if(MN1 > 225){
      if(leps[0]->Pt() < pt_cut)  return "SR1_MNbin1";
      FillCutflow(HNL_LeptonCore::SR1, w, "SR1_LepPt",PNMass);
    }
    else   if(MN1 > 150){
      if(leps[0]->Pt() < pt_cut-50)  return "SR1_MNbin1";
      FillCutflow(HNL_LeptonCore::SR1, w, "SR1_LepPt",PNMass);
    }
    

    int nSRbins=8;
    double ml1jbins[nSRbins] = { 0., 500, 600.,700.,850, 1000.,1400, 2000.};
    for(int ibin=1; ibin < nSRbins; ibin++){
      if(MN1 < ml1jbins[ibin]) return "SR1_MNbin"+to_string(ibin+1);
    }
    cout << "Error in SR1 binning " << Modifier << endl;
    exit(EXIT_FAILURE);

  }

  else  if (Modifier.Contains("Binning_Version1")){

    if(channel==MuMu){
      if(MN1 > 350){
	if(leps[0]->Pt() < 140)   return "false";
	if(leps[1]->Pt() < 65)    return "false";
      }
      else   if(MN1 > 280){
	if(leps[0]->Pt() < 140)   return "false";
	if(leps[1]->Pt() < 40)    return "false";
      }   
      else if(MN1 > 225){
	if(leps[0]->Pt() < 140)   return "false";
	if(leps[1]->Pt() < 25)    return "false";
      }   
      else   if(MN1 > 150){
	if(leps[0]->Pt() < 70)   return "false";
	if(leps[1]->Pt() < 15)    return "false";
      }   
    }
    if(channel==EE || channel==EMu){
      if(MN1 > 635){
	if(leps[0]->Pt() < 140)   return "false";
	if(leps[1]->Pt() < 50)    return "false";
      }
      else   if(MN1 > 550){
	if(leps[0]->Pt() < 120)   return "false";
	if(leps[1]->Pt() < 50)    return "false";
      }
      else if(MN1 > 440){
	if(leps[0]->Pt() < 120)   return "false";
	if(leps[1]->Pt() < 35)    return "false";
      }
      else   if(MN1 > 270){
	if(leps[0]->Pt() < 100)   return "false";
	if(leps[1]->Pt() < 25)    return "false";
      }
    }   
    int nSRbins=8;
    double ml1jbins[nSRbins] = { 0., 200., 400.,500., 600.,700., 1000., 2000.};
    for(int ibin=1; ibin < nSRbins; ibin++){
      if(MN1 < ml1jbins[ibin]) return "SR1_MNbin"+to_string(ibin);
    }
    

    cout << "Error in SR1 binning " << Modifier << endl;
    exit(EXIT_FAILURE);
  }

  if(Modifier.Contains("MassDep_Version1")){

    TString SR1Bin="SR1_MNbin1";

    if(leps[0]->Pt() < pt_cut)  return "SR1_MNbin1";
    FillCutflow(HNL_LeptonCore::SR1, w, "SR1_LepPt",PNMass);

    if(MN1 < 300) return SR1Bin;

    FillCutflow(HNL_LeptonCore::SR1, w, "SR1_masscuts",PNMass);

    vector<double> vMassBins = MassBinsSR1(channel,mass);

    int nSRbins=  vMassBins.size();

    double ml1jbins[nSRbins] = { vMassBins[0], vMassBins[1],vMassBins[2],vMassBins[3],vMassBins[4]}; 

    if(param.WriteOutVerbose == 0)FillHist( "LimitSR1/"+PNMass+"/N1Mass_Central",  MN1,  w, nSRbins-1, ml1jbins, "Reco M_{l1jj}");

    for(int ibin=1; ibin < nSRbins; ibin++){
      if(MN1 < ml1jbins[ibin]) return "SR1_MNbin"+to_string(ibin);
    }

    cout << "Error in SR1 binning " << Modifier << endl;
    exit(EXIT_FAILURE);

  }
  
  if(Modifier.Contains("MassDep_EXO17028")){
    FillCutflow(HNL_LeptonCore::SR1, w, "SR1_LepPt",PNMass);
    if(AK8_JetColl[0].SDMass() < 150)  return GetEXO17028_SignalRegion2Bin(channel,mass, JetColl, AK8_JetColl, leps,  METv);
    else return "SR1_MNbin3";
  }

  
  cout << "Error in SR1 binning " << Modifier << endl;
  exit(EXIT_FAILURE);


}

TString  HNL_RegionDefinitionsOpt::GetEXO17028_SignalRegion1Bin(HNL_LeptonCore::Channel channel, TString mass ,  std::vector<Jet> jets, std::vector<FatJet> fatjets,    std::vector<Lepton *> leps, Particle _MET) {

  vector<double> VarCuts;
  if(channel == EE){
    if(mass == "85") VarCuts = {4, 25., 3.1,  25., 15., 50.,120.,  110., 45., 95, 6. };
    if(mass == "95") VarCuts = {4, 25., 3.1,  25., 15., 50.,120.,  110., 50., 100, 6.};
    if(mass == "100") VarCuts = {4, 25., 3.1, 25., 15., 50.,120., 120., 50., 110., 6.};
    if(mass == "150") VarCuts = {4, 25., 3.1, 40, 25, 50.,120., 180, 130, 160, 6};
    if(mass== "200") VarCuts =  {4, 25., 3.1, 55., 40., 50.,120., 220.,160., 225.,  6.};
    if(mass== "250") VarCuts =  {4, 25., 999, 70., 60., 50.,120., 310.,220., 270.,  6.};
    if(mass== "300") VarCuts =  {4, 25., 999., 80., 60., 50.,120., 370.,235., 335.,  6.};
    if(mass== "400") VarCuts =  {4, 25., 999., 100., 65., 50.,120., 450.,335., 450.,  6.};
    if(mass== "500") VarCuts =  {4, 25., 9999., 125., 65., 50.,120., 560.,400., 555., 6.};
    if(mass== "600") VarCuts =  {4, 25., 9999., 125., 0., 50.,120., 760.,400., 690., 6.};
    if(mass== "700") VarCuts =  {4, 25., 9999., 125., 0., 50.,120., 760.,400., 966., 6.};
    if(mass== "800") VarCuts =  {4, 25., 9999., 125., 0., 50.,120., 760.,400., 1130., 6.};
    if(mass== "900") VarCuts =  {4, 25., 9999., 125., 0., 50.,120., 760.,400., 1300., 6.};
    if(mass== "1000") VarCuts = {4, 25., 9999., 125., 0., 50.,120., 760.,400., 1490., 6.};
    if(mass== "1100") VarCuts = {4, 25., 9999., 125., 0., 50.,120., 760.,400., 1490., 6.};
    if(mass== "1200") VarCuts = {4, 25., 9999., 125., 0., 50.,120., 760.,400., 1600., 6.};
    if(mass== "1300") VarCuts = {4, 25., 9999., 125., 0., 50.,120., 760.,400., 1930., 6.};
    if(mass== "1400") VarCuts = {4, 25., 9999., 125., 0., 50.,120., 760.,400., 1930., 6.};
    if(mass== "1500") VarCuts = {4, 25., 9999., 125., 0., 50.,120., 760.,400., 1930., 6.};
    if(mass== "1700") VarCuts = {4, 25., 9999., 125., 0., 50.,120., 760.,400., 2130., 6.};
    if(mass== "2000") VarCuts = {4, 25., 9999., 125., 0., 50.,120., 760.,400., 2530., 6.};
  }
  if(channel ==MuMu){
    if(mass == "85") VarCuts = {4, 25., 3.1,  25., 10., 50.,120.,  90., 40., 100, 9. };
    if(mass == "95") VarCuts = {4, 25., 3.1,  25., 10., 50.,120.,  90., 45., 105, 9.};
    if(mass == "100") VarCuts = {4, 25., 3.1, 25., 15., 50.,120., 110., 55., 115., 9.};
    if(mass == "150") VarCuts = {4, 25., 3.1, 35,  35, 50.,120., 150, 110, 170, 7};
    if(mass== "200") VarCuts =  {4, 25., 3.1, 50., 40., 50.,120., 250.,160., 215.,  7.};
    if(mass== "250") VarCuts =  {4, 25., 3.1, 85., 45., 50.,120., 310.,215., 270.,  7.};
    if(mass== "300") VarCuts =  {4, 25., 999., 100., 50., 50.,120., 370.,225., 340.,  7.};
    if(mass== "400") VarCuts =  {4, 25., 999., 110., 60., 50.,120., 490.,296., 490.,  7.};
    if(mass== "500") VarCuts =  {4, 25., 9999., 110., 60., 50.,120., 610.,370., 550., 7.};
    if(mass== "600") VarCuts =  {4, 25., 999., 110., 0., 50.,120., 680.,370., 630.,  7.};
    if(mass== "700") VarCuts =  {4, 25., 999., 110., 0., 50.,120., 800.,370., 885.,  7.};
    if(mass== "800") VarCuts =  {4, 25., 999., 110., 0., 50.,120., 800.,370., 890.,  7.};
    if(mass== "900") VarCuts =  {4, 25., 999., 110., 0., 50.,120., 800.,370., 1225.,  7.};
    if(mass== "1000") VarCuts = {4, 25., 999., 110., 0., 50.,120., 800.,370., 1230.,  7.};
    if(mass== "1100") VarCuts = {4, 25., 9999., 110., 0., 50.,120., 800.,370., 1245., 6.};
    if(mass== "1200") VarCuts = {4, 25., 999., 110., 0., 50.,120., 800.,370., 1690.,  7.};
    if(mass== "1300") VarCuts = {4, 25., 999., 110., 0., 50.,120., 800.,370., 1890.,  7.};
    if(mass== "1400") VarCuts = {4.,25., 999., 110., 0., 50.,120., 800.,370., 1940.,  7.};
    if(mass== "1500") VarCuts = {4, 25., 9999., 110., 0., 50.,120., 800.,370., 2220., 6.};
    if(mass== "1700") VarCuts = {4, 25., 9999., 110., 0., 50.,120., 800.,370., 2520., 6.};
    if(mass== "2000") VarCuts = {4, 25., 9999., 110., 0., 50.,120., 800.,370., 2720., 6.};
  }

  if(VarCuts.size() != 11) {
    cout << "Error in SR1 VarCuts size " << endl;
    exit(EXIT_FAILURE);
  }

  return PassSigRegion1(jets, fatjets ,leps,_MET,VarCuts);

  

}

TString HNL_RegionDefinitionsOpt::PassSigRegion1(std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Lepton *> leps, Particle  met,  vector<double> VarCuts){

  double var1 = VarCuts[0];
  double var2 = VarCuts[1];
  double var3 = VarCuts[2];
  double var4 = VarCuts[3];
  double var5 = VarCuts[4];
  double var6 = VarCuts[5];
  double var7 = VarCuts[6];
  double var8 = VarCuts[7];
  double var9 = VarCuts[8];
  double var10 = VarCuts[9];
  double var11 = VarCuts[10];


  if(jets.size() < 2) return "false";
  float dijetmass_tmp=999.;
  float dijetmass=9990000.;
  int m=-999;
  int n=-999;
  double ST(0.);
  for(UInt_t emme=0; emme<jets.size(); emme++){
    ST += jets[emme].Pt();
    for(UInt_t enne=1; enne<jets.size(); enne++) {

      dijetmass_tmp = (jets[emme]+jets[enne]).M();
      if(emme == enne) continue;

      if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
        dijetmass = dijetmass_tmp;
        m = emme;
        n = enne;
      }
    }
  }


  ST += leps[0]->Pt() + leps[1]->Pt() + met.Pt();
  double met2 = pow(met.Pt(),2.);
  Particle Wcand = jets[m] + jets[n];
  Particle W1cand = jets[m] + jets[n]+ *leps[0] + *leps[1];
  Particle N1cand = jets[m] + jets[n]+ *leps[0];
  Particle N2cand = jets[m] + jets[n]+ *leps[1];


  if(jets.size() >= var1) return "SR3_bin3";
  if(jets[0].Pt() < var2) return "SR3_bin3";
  if(Wcand.DeltaR(*leps[1]) > var3) return "SR3_bin3";
  if(leps[0]->Pt() < var4) return "SR3_bin3";
  if(leps[1]->Pt() < var5) return "SR3_bin3";
  if(Wcand.M() < var6) return "SR3_bin3";
  if(Wcand.M() > var7) return "SR3_bin3";
  if(W1cand.M() < var8) return "SR3_bin3";
  if(N1cand.M() < var9 && N2cand.M()  < var9) return "SR3_bin3";
  if(N1cand.M()  > var10 && N2cand.M()  > var10) return "SR3_bin3";
  if(met2/ST > var11) return "SR3_bin3";

  return "SR3_bin4";

}


TString  HNL_RegionDefinitionsOpt::GetEXO17028_SignalRegion2Bin(HNL_LeptonCore::Channel channel, TString mass ,  std::vector<Jet> jets, std::vector<FatJet> fatjets,    std::vector<Lepton *> leps, Particle _MET) {

  vector<double> VarCuts;
  if(channel == EE){
    if(mass == "100") VarCuts = {25.,  15., 40., 130., 100.,  220.,  15.};
    if(mass == "150") VarCuts = {90,   15,  40., 130., 125,   185,   15};
    if(mass== "200") VarCuts =  {100., 20., 40., 130., 173.,  220.,  15.};
    if(mass== "250") VarCuts =  {100., 25., 40., 130., 220.,  305.,  15.};
    if(mass== "300") VarCuts =  {100., 30., 40., 130., 270,   330.,  15.};
    if(mass== "400") VarCuts =  {100., 35., 40., 130., 330.,  440.,  15.};
    if(mass== "500") VarCuts =  {120., 35., 40., 130., 440.,  565.,  15.};
    if(mass== "600") VarCuts =  {120., 0.,  40., 130., 565.,  675.,  15.};
    if(mass== "700") VarCuts =  {140., 0.,  40., 130., 635.,  775.,  15.};
    if(mass== "800") VarCuts =  {140., 0.,  40., 130., 740.,  1005., 15.};
    if(mass== "900") VarCuts =  {140., 0.,  40., 130., 865.,  1030., 15.};
    if(mass== "1000") VarCuts = {140., 0.,  40., 130., 890.,  1185., 15.};
    if(mass== "1100") VarCuts = {140., 0.,  40., 130., 1035., 1395., 15};
    if(mass== "1200") VarCuts = {140., 0.,  40., 130., 1085., 1460., 15.};
    if(mass== "1300") VarCuts = {140., 0.,  40., 130., 1140., 1590., 15};
    if(mass== "1400") VarCuts = {140., 0.,  40., 130., 1245., 1700., 15};
    if(mass== "1500") VarCuts = {140., 0.,  40., 130., 1300., 1800., 15.};
    if(mass== "1700") VarCuts = {140., 0.,  40., 130., 1300., 2000., 15.};
    if(mass== "2000") VarCuts = {140., 0.,  40., 130., 1300., 2800., 15};
  }
  if(channel ==MuMu){
    if(mass == "100") VarCuts = {25.,  15., 40.,130.,  98.,  145., 15.};
    if(mass == "150") VarCuts = {70,   15,  40.,130.,  145,  175, 15};
    if(mass== "200") VarCuts =  {100., 20., 40., 130., 175., 235., 15.};
    if(mass== "250") VarCuts =  {140., 25., 40., 130., 226., 280., 15.};
    if(mass== "300") VarCuts =  {140., 40., 40., 130., 280., 340., 15.};
    if(mass== "400") VarCuts =  {140., 65., 40., 130., 340., 445., 15.};
    if(mass== "500") VarCuts =  {140., 65., 40., 130., 445., 560., 15.};
    if(mass== "600") VarCuts =  {140., 0.,  40., 130., 560., 685., 15};
    if(mass== "700") VarCuts =  {140., 0.,  40., 130., 635., 825., 15.};
    if(mass== "800") VarCuts =  {140., 0.,  40., 130., 755., 960., 15.};
    if(mass== "900") VarCuts =  {140., 0.,  40., 130., 840., 1055., 15};
    if(mass== "1000") VarCuts = {140., 0.,  40., 130., 900., 1205., 15};
    if(mass== "1100") VarCuts = {140., 0.,  40., 130., 990., 1250., 15.};
    if(mass== "1200") VarCuts = {140., 0.,  40., 130., 1035., 1430., 15.};
    if(mass== "1300") VarCuts = {140., 0.,  40., 130., 1110., 1595., 15};
    if(mass== "1400") VarCuts = {140., 0.,  40., 130., 1285., 1700., 15};
    if(mass== "1500") VarCuts = {140., 0.,  40., 130., 1330., 1800., 15};
    if(mass== "1700") VarCuts = {140., 0.,  40., 130., 1330., 2200., 15};
    if(mass== "2000") VarCuts = {140., 0.,  40., 130., 1330., 99999., 15.};
  }

  if(VarCuts.size() != 7) {
    cout << "Error in SR2 VarCuts size " << endl;
    exit(EXIT_FAILURE);
  }

  return PassSigRegion2(jets, fatjets ,leps,_MET,VarCuts);
}

TString HNL_RegionDefinitionsOpt::PassSigRegion2(std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Lepton *> leps, Particle  met,  vector<double> VarCuts){

  double var1 = VarCuts[0];
  double var2 = VarCuts[1];
  double var3 = VarCuts[2];
  double var4 = VarCuts[3];
  double var5 = VarCuts[4];
  double var6 = VarCuts[5];
  double var7 = VarCuts[6];

  if(fatjets.size() == 0) return "SR1_MNbin2";

  float dijetmass_tmp=999.;
  float dijetmass=9990000.;
  int m=-999;
  double ST(0.);
  for(UInt_t emme=0; emme<jets.size(); emme++){
    ST += jets[emme].Pt();
  }
  for(UInt_t emme=0; emme<fatjets.size(); emme++){
    ST += fatjets[emme].Pt();
    dijetmass_tmp = fatjets[emme].SDMass();
    if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
      dijetmass = dijetmass_tmp;
      m = emme;
    }
  }
  ST += leps[0]->Pt() + leps[1]->Pt() + met.Pt();

  double met2 = met.Pt()*met.Pt();
  Particle Wcand  = fatjets[m];
  Particle W1cand = fatjets[m] + *leps[0] + *leps[1];
  Particle N1cand = fatjets[m] + *leps[0];
  Particle N2cand = fatjets[m] + *leps[1];

  if(leps[0]->Pt() < var1) return "SR1_MNbin2";
  if(leps[1]->Pt() < var2) return "SR1_MNbin2";
  if(fatjets[m].SDMass() < var3) return "SR1_MNbin2";
  if(fatjets[m].SDMass() > var4) return "SR1_MNbin2";
  if(N1cand.M()  < var5 && N2cand.M()  < var5) return "SR1_MNbin2";

  if(N1cand.M()  > var6 && N2cand.M()  > var6) return "SR1_MNbin2";

  if(met2/ST > var7) return "SR1_MNbin2";

  return "SR1_MNbin1";


}




vector<double> HNL_RegionDefinitionsOpt::MassBinsSR1(HNL_LeptonCore::Channel channel, TString mass ){
  
  if(channel == MuMu){
    if(mass == "100") return {0., 95,  150,200 , 2000.};
    if(mass == "150") return {0., 100, 170, 250 , 2000.};
    if(mass== "200") return  {0., 175, 250 ,300, 2000.};
    if(mass== "300") return  {0., 275, 350 ,400, 2000.};
    if(mass== "400") return  {0., 340, 400 ,500, 2000.};
    if(mass== "500") return  {0., 440, 560, 600 , 2000.};
    if(mass== "600") return  {0., 560, 700, 750 , 2000.};
    if(mass== "700") return  {0., 600, 850,1000 , 2000.};
    if(mass== "800") return  {0., 750 ,1000,1200 , 2000.};
    if(mass== "900") return  {0., 750, 840,1200 , 2000.};
    if(mass== "1000") return  {0., 750, 900, 1250 , 2000.};
    if(mass== "1100") return  {0., 750, 990, 1300, 2000.};
    if(mass== "1200") return  {0., 750, 1000, 1450, 2000.};
    if(mass== "1300") return  {0., 750,1000, 1600 , 2000.};
    if(mass== "1400") return  {0., 750,1000, 1700, 2000.};
    if(mass== "1500") return  {0., 750,1000, 1800, 2000.};
    if(mass== "2000") return  {0., 750,1000, 1900, 2000.};
  }
  if(channel == EE){
    if(mass == "100") return {0., 95,  150,200 , 2000.};
    if(mass == "150") return {0., 100, 170, 250 , 2000.};
    if(mass== "200") return  {0., 175, 250 ,300, 2000.};
    if(mass== "300") return  {0., 275, 350 ,400, 2000.};
    if(mass== "400") return  {0., 340, 400 ,500, 2000.};
    if(mass== "500") return  {0., 440, 560, 600 , 2000.};
    if(mass== "600") return  {0., 560, 700, 750 , 2000.};
    if(mass== "700") return  {0., 600, 850,1000 , 2000.};
    if(mass== "800") return  {0., 750 ,1000,1200 , 2000.};
    if(mass== "900") return  {0., 750, 840,1200 , 2000.};
    if(mass== "1000") return  {0., 750, 900, 1250 , 2000.};
    if(mass== "1100") return  {0., 750, 990, 1300, 2000.};
    if(mass== "1200") return  {0., 750, 1000, 1450, 2000.};
    if(mass== "1300") return  {0., 750,1000, 1600 , 2000.};
    if(mass== "1400") return  {0., 750,1000, 1700, 2000.};
    if(mass== "1500") return  {0., 750,1000, 1800, 2000.};
    if(mass== "2000") return  {0., 750,1000, 1900, 2000.};

  }

  return {0., 600, 850,1000 , 2000.};
}

TString HNL_RegionDefinitionsOpt::RunSignalRegionAK4StringBDT(TString mN, HNL_LeptonCore::Channel channel, 
							      std::vector<Lepton *> LepTColl,  
							      std::vector<Jet> JetColl,std::vector<Jet> B_JetColl,Particle METv, Event  ev){

  if (LepTColl.size() != 2) return "false";
  Particle ll =  (*LepTColl[0]) + (*LepTColl[1]);
  if (channel==EE  && (fabs(ll.M()-90.) < 10)) return "false";

  float MVAvalue = ev.HNL_MVA_Event(GetChannelString(channel)+"_"+mN);

  std::vector<FatJet> FatJetColl;
  double ST = GetST( LepTColl, JetColl, FatJetColl, METv);
  double met2_st = pow(METv.Pt(),2.)/ ST;
  bool PassHMMet    = (met2_st < 15);


  TString LabelPrefix =  "SR3";
  if(!PassHMMet) return "false";
  if(B_JetColl.size() > 0)    return "false";
  
  if(DataYear==2016){
    if(MVAvalue< 0.0) return LabelPrefix+"_BDTbin1";
    else if(MVAvalue< 0.10) return LabelPrefix+"_BDTbin2";
    else if(MVAvalue< 0.125) return LabelPrefix+"_BDTbin3";
    else if(MVAvalue< 0.15) return LabelPrefix+"_BDTbin4";
    else if(MVAvalue< 0.2) return LabelPrefix+"_BDTbin5";
    else if(MVAvalue< 0.225) return LabelPrefix+"_BDTbin6";
    else  return LabelPrefix+"_BDTbin7";

  }
  
  if(DataYear==2017){
    if(MVAvalue< 0.0) return LabelPrefix+"_BDTbin1";
    else if(MVAvalue< 0.10) return LabelPrefix+"_BDTbin2";
    else if(MVAvalue< 0.125) return LabelPrefix+"_BDTbin3";
    else if(MVAvalue< 0.15) return LabelPrefix+"_BDTbin4";
    else if(MVAvalue< 0.2) return LabelPrefix+"_BDTbin5";
    else if(MVAvalue< 0.225) return LabelPrefix+"_BDTbin6";
    else  return LabelPrefix+"_BDTbin7";
  }

  if(DataYear==2018){
    if(MVAvalue< 0.0) return LabelPrefix+"_BDTbin1";
    else if(MVAvalue< 0.10) return LabelPrefix+"_BDTbin2";
    else if(MVAvalue< 0.125) return LabelPrefix+"_BDTbin3";
    else if(MVAvalue< 0.15) return LabelPrefix+"_BDTbin4";
    else if(MVAvalue< 0.2) return LabelPrefix+"_BDTbin5";
    else if(MVAvalue< 0.225) return LabelPrefix+"_BDTbin6";
    else  return LabelPrefix+"_BDTbin7";

  }
  return "true";
}



bool  HNL_RegionDefinitionsOpt::RunSignalRegionWW(HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,
						  std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto , std::vector<Tau> TauColl, 
						  std::vector<Jet> JetCollLoose, std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, 
						  Event ev, Particle METv, AnalyzerParameter param, TString PostLabel ,  float w){
  
  TString SR2String = RunSignalRegionWWString(channel, qq, leps, leps_veto, TauColl,JetCollLoose, JetColl,AK8_JetColl,B_JetColl,ev, METv, param, PostLabel,w);
  if(SR2String == "false") return false;
  else return true;
  
}



TString HNL_RegionDefinitionsOpt::RunSignalRegionWWString(HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,
							  std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto , std::vector<Tau> TauColl, 
							  std::vector<Jet> JetCollLoose, std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, 
							  Event ev, Particle METv, AnalyzerParameter param, TString Modifier ,  float w){
  
  
  if (leps_veto.size() != 2) return "false";


  if(qq==Plus && leps[0]->Charge() < 0) return "false";
  if(qq==Minus && leps[0]->Charge() > 0) return "false";

  FillCutflow(HNL_LeptonCore::SR2,w, "SR2_lep_charge",param);

  FillCutflow(HNL_LeptonCore::SR2, w, "SR2_lep_pt",param);

  Fill_RegionPlots(param,"InclusiveSR2" ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);

  if (param.SRConfig.Contains("VBFLeadJet")){

    if (param.SRConfig.Contains("MJJ0p45")){
      if(!PassVBF(JetColl,leps,450., param, w, true)) return "false";
    }
    else if (param.SRConfig.Contains("MJJ0p5")){
      if(!PassVBF(JetColl,leps,500., param, w, true)) return "false";
    }   
    else if (param.SRConfig.Contains("MJJ0p6")){
      if(!PassVBF(JetColl,leps,600., param, w, true)) return "false";
    }
    else if (param.SRConfig.Contains("MJJ0p75")){
      if(!PassVBF(JetColl,leps,750., param, w, true)) return "false";
    }
    else if (param.SRConfig.Contains("MJJ0p8")){
      if(!PassVBF(JetColl,leps,800., param, w, true)) return "false";
    }
    else if (param.SRConfig.Contains("MJJ0p85")){
      if(!PassVBF(JetColl,leps,850., param, w, true)) return "false";
    }
    else if (param.SRConfig.Contains("MJJ0p9")){
      if(!PassVBF(JetColl,leps,900., param, w, true)) return "false";
    }
    else if (param.SRConfig.Contains("MJJ1")){
      if(!PassVBF(JetColl,leps,1000., param, w, true)) return "false";
    }
    else if(!PassVBF(JetColl,leps,750., param, w, true)) return "false";

  }
  else   if (param.SRConfig.Contains("VBFAwayJet")){

    if (param.SRConfig.Contains("MJJ0p45")){
      if(!PassVBF(JetColl,leps,450., param, w, false)) return "false";
    }   
    else if (param.SRConfig.Contains("MJJ0p5")){
      if(!PassVBF(JetColl,leps,500., param, w, false)) return "false";
    }
    else if (param.SRConfig.Contains("MJJ0p6")){
      if(!PassVBF(JetColl,leps,600., param, w, false)) return "false";
    }
    else if (param.SRConfig.Contains("MJJ0p75")){
      if(!PassVBF(JetColl,leps,750., param, w, false)) return "false";
    }
    else if (param.SRConfig.Contains("MJJ0p8")){
      if(!PassVBF(JetColl,leps,800., param, w, false)) return "false";
    }
    else if (param.SRConfig.Contains("MJJ0p85")){
      if(!PassVBF(JetColl,leps,850., param, w, false)) return "false";
    }

    else if (param.SRConfig.Contains("MJJ0p9")){
      if(!PassVBF(JetColl,leps,900., param, w, false)) return "false";
    }
    else if (param.SRConfig.Contains("MJJ1")){
      if(!PassVBF(JetColl,leps,1000., param, w, false)) return "false";
    }

    else if(!PassVBF(JetColl,leps,750., param, w, false)) return "false";
    
  }

  else       if(!PassVBF(JetColl,leps,750., param, w, true)) return "false";


  FillCutflow(HNL_LeptonCore::SR2, w, "SR2_VBF",param);

  bool PassBJetMVeto = (B_JetColl.size()==0);

  double HT(0.);

  if (param.SRConfig.Contains("LooseJet")){
    for(auto ij : JetCollLoose) HT += ij.Pt();
  }
  else   if (param.SRConfig.Contains("TightJet")){
    for(auto ij : JetColl) HT += ij.Pt();
  }
  else{
    for(auto ij : JetColl) HT += ij.Pt();
  }


  double htltcut = 1.;
  
  if (param.SRConfig.Contains("HTLT_10")) htltcut = 1.;
  if (param.SRConfig.Contains("HTLT_15")) htltcut = 1.5;
  if (param.SRConfig.Contains("HTLT_20")) htltcut = 2;
  if (param.SRConfig.Contains("HTLT_25")) htltcut = 2.5;
  if (param.SRConfig.Contains("HTLT_30")) htltcut = 35;
  if (param.SRConfig.Contains("HTLT_50")) htltcut = 5;  


  double ST = GetST( leps, JetColl, AK8_JetColl, METv);
  double met2_st = pow(METv.Pt(),2.)/ ST;

  double met_cut = 10.;
  if (param.SRConfig.Contains("SR2MET_20"))   met_cut = 20.;
  if (param.SRConfig.Contains("SR2MET_18"))   met_cut = 18.;
  if (param.SRConfig.Contains("SR2MET_15"))   met_cut = 15.;
  if (param.SRConfig.Contains("SR2MET_12"))   met_cut = 12.;
  if (param.SRConfig.Contains("SR2MET_10"))   met_cut = 10.;
  if (param.SRConfig.Contains("SR2MET_9"))   met_cut = 9.;
  if (param.SRConfig.Contains("SR2MET_8"))   met_cut = 8.;
  if (param.SRConfig.Contains("SR2MET_7"))   met_cut = 7.;
  if (param.SRConfig.Contains("SR2MET_6"))   met_cut = 6.;

  bool PassHMMet    = (met2_st < met_cut); 
  
  if(!PassHMMet) return "false";
  FillCutflow(HNL_LeptonCore::SR2, w, "SR2_met",param);
  if(!PassBJetMVeto)return"false";
  FillCutflow(HNL_LeptonCore::SR2, w, "SR2_bveto",param);    
  if ( (HT/leps[0]->Pt()) > htltcut) return"false";
  FillCutflow(HNL_LeptonCore::SR2, w, "SR2_ht_lt1",param);

  FillCutflow(HNL_LeptonCore::SR2, w, "SR2_Final",param);

  if(Modifier == "Binning_Version1"){
    
    double HTLT[3] = { 0.,1., 2};
    double QHTLT[5] = { -2.,-1.,0.,1., 2};
    if(param.WriteOutVerbose == 0)FillHist( "LimitSR2/"+param.Name+"/HT_LT1_Central",  HT/leps[0]->Pt(),  w, 2, HTLT, "Reco HT/LT1");
    if(param.WriteOutVerbose == 0)FillHist( "LimitSR2/"+param.Name+"/Q_HT_LT1_Central",  (leps[0]->Charge()*HT)/leps[0]->Pt(),  w, 4 , QHTLT, "Reco HT/LT1 * Q");
    
    FillCutflow(HNL_LeptonCore::ChannelDepSR2, w, GetChannelString(channel) +"_SR2", param);//Def
    
    if (HT/leps[0]->Pt() < 1)      return "SR2_HTLTbin1";
    return "SR2_HTLTbin2";
    
  }
  else if(Modifier == "Binning_Version2"){
    
    double PTBin[5] = { 0.,50, 100., 120., 200};
    double L2Pt = (leps[1]->Pt() > 200) ? 199. : leps[1]->Pt();
    if(param.WriteOutVerbose == 0)FillHist( "LimitSR2/"+param.Name+"/PT2_LT1_Central",  L2Pt,  w, 4, PTBin, "Reco PTLep2");
    
    
    if (L2Pt > 120)      return "SR2_HTLTbin1";
    return "SR2_HTLTbin2";
  }
  return "false";
}



bool  HNL_RegionDefinitionsOpt::RunSignalRegionAK4(HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto ,  std::vector<Tau> TauColl,std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param, TString PostLabel ,  float w){



  TString SR3String = RunSignalRegionAK4String(channel, qq, leps, leps_veto, TauColl,JetColl,AK8_JetColl,B_JetColl,ev, METv, param, PostLabel,w);
  if(SR3String == "false") return false;
  else return true;

}


TString HNL_RegionDefinitionsOpt::RunSignalRegionAK4String(HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto ,  std::vector<Tau> TauColl,std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param, TString Modifier ,  float w){
                                       
  FillCutflow(HNL_LeptonCore::SR3, w, "SR3_lep_pt",param);

  if (leps_veto.size() != 2) {
    return "false";
  }

  if(qq==Plus && leps[0]->Charge() < 0)  return "false";
  if(qq==Minus && leps[0]->Charge() > 0) return "false";

  FillCutflow(HNL_LeptonCore::SR3, w, "SR3_lep_charge",param);

  Particle ll =  (*leps[0]) + (*leps[1]);
  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) {
    return "false";
  }

  FillCutflow(HNL_LeptonCore::SR3, w, "SR3_dilep_mass",param);

  double ST      = GetST(leps, JetColl, AK8_JetColl, METv);
  double met2_st = pow(METv.Pt(),2.)/ ST;

  double met_cut = 15;
  if (param.SRConfig.Contains("SR3MET_20"))   met_cut = 20.;
  if (param.SRConfig.Contains("SR3MET_18"))   met_cut = 18.;
  if (param.SRConfig.Contains("SR3MET_15"))   met_cut = 15.;
  if (param.SRConfig.Contains("SR3MET_12"))   met_cut = 12.;
  if (param.SRConfig.Contains("SR3MET_10"))   met_cut = 10.;
  if (param.SRConfig.Contains("SR3MET_9"))   met_cut = 9.;
  if (param.SRConfig.Contains("SR3MET_8"))   met_cut = 8.;
  if (param.SRConfig.Contains("SR3MET_7"))   met_cut = 7.;
  if (param.SRConfig.Contains("SR3MET_6"))   met_cut = 6.;

  bool PassHMMet  = (met2_st < met_cut);

  int  NB_JetColl      =  B_JetColl.size();

  double pt_bin1= 140.;
  if (param.SRConfig.Contains("SR3_bin1pt_60")) pt_bin1= 60.;
  if (param.SRConfig.Contains("SR3_bin1pt_70")) pt_bin1= 70.;
  if (param.SRConfig.Contains("SR3_bin1pt_80")) pt_bin1= 80.;
  if (param.SRConfig.Contains("SR3_bin1pt_90")) pt_bin1= 90.;
  if (param.SRConfig.Contains("SR3_bin1pt_100")) pt_bin1= 100.;
  if (param.SRConfig.Contains("SR3_bin1pt_120")) pt_bin1= 120.;
  if (param.SRConfig.Contains("SR3_bin1pt_140")) pt_bin1= 140.;
  if (param.SRConfig.Contains("SR3_bin1pt_160")) pt_bin1= 160.;


  double pt_bin2= 140.;
  if (param.SRConfig.Contains("SR3_bin2pt_60")) pt_bin2= 60.;
  if (param.SRConfig.Contains("SR3_bin2pt_70")) pt_bin2= 70.;
  if (param.SRConfig.Contains("SR3_bin2pt_80")) pt_bin2= 80.;
  if (param.SRConfig.Contains("SR3_bin2pt_90")) pt_bin2= 90.;
  if (param.SRConfig.Contains("SR3_bin2pt_100")) pt_bin2= 100.;
  if (param.SRConfig.Contains("SR3_bin2pt_120")) pt_bin2= 120.;
  if (param.SRConfig.Contains("SR3_bin2pt_140")) pt_bin2= 140.;
  if (param.SRConfig.Contains("SR3_bin2pt_160")) pt_bin2= 160.;
  
  
  if(NB_JetColl>  0)  {
    return "false";
  }
  FillCutflow(HNL_LeptonCore::SR3, w, "SR3_bveto",param);

  if(!PassHMMet) {
    return "false";
  }
  FillCutflow(HNL_LeptonCore::SR3, w, "SR3_MET",param);  


  //////// SR1  PASS MET and BJet veto

  if(JetColl.size() == 0 && leps[1]->Pt() > pt_bin1) {
    if(param.WriteOutVerbose == 0)FillHist( "LimitSR3/"+param.Name+"/SignalBins",   0.5, w, 16, 0, 16.,  "Signalbins");
    if(param.WriteOutVerbose == 0)    FillHist( "LimitSR3/"+param.Name+"/SignalBinsQ",   0.5*leps[0]->Charge(), w, 32, -16, 16., "Signalbins");                                       
    FillCutflow(HNL_LeptonCore::SR3, w, "SR3_0JetBin",param);

    return "SR3_bin1";
  }
  if(JetColl.size() == 1 && leps[1]->Pt() > pt_bin1) {
    if(param.WriteOutVerbose == 0)FillHist( "LimitSR3/"+param.Name+"/SignalBins",   1.5, w, 16, 0, 16., "Signalbins");
    if(param.WriteOutVerbose == 0)FillHist( "LimitSR3/"+param.Name+"/SignalBinsQ",   1.5*leps[0]->Charge(), w, 32, -16, 16., "Signalbins");                                       
    FillCutflow(HNL_LeptonCore::SR3, w, "SR3_1JetBin",param);

    return "SR3_bin2";
  }

  if(JetColl.size() >0)FillCutflow(HNL_LeptonCore::SR3, w, "SR3_jet",param);

  if(JetColl.size() < 2) {
    return "false";
  }
  FillCutflow(HNL_LeptonCore::SR3, w, "SR3_dijet",param);

  if(Modifier == "MassDep_EXO17028")      return "SR3_bin3";

  if(JetColl[0].Pt() < 25.)  {
    return "false";
  }
  FillCutflow(HNL_LeptonCore::SR3, w, "SR3_J1Pt",param);
 
  float dijetmass_tmp=999.;
  float dijetmass=9990000.;
  int m=-999;
  int n=-999;
  for(UInt_t emme=0; emme<JetColl.size(); emme++){
    for(UInt_t enne=1; enne<JetColl.size(); enne++) {

      if(emme == enne) continue;      
      dijetmass_tmp = (JetColl[emme]+JetColl[enne]).M();

      
      if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
        dijetmass = dijetmass_tmp;
        m = emme;
        n = enne;
      }
    }
  }


  Particle N1cand = JetColl[m] + JetColl[n]+ *leps[0];
  Particle N2cand = JetColl[m] + JetColl[n]+ *leps[1];
  Particle Wcand = JetColl[m] + JetColl[n]+ *leps[0]+ *leps[1];
  Particle NCand = (leps[1]->Pt() < leps[0]->Pt()) ? N2cand : N1cand;
  Lepton Nlep = (leps[1]->Pt() < 100.) ?  *leps[1] : *leps[0];
  Lepton Wlep = (leps[1]->Pt() > 100.) ? *leps[1] : *leps[0];

  if(Wcand.M() < 400) return "SR3_bin3";
  FillCutflow(HNL_LeptonCore::SR3, w, "SR3_W1Mass",param);

  if(leps[1]->Pt() < 20) return "SR3_bin3";
  if(leps[0]->Pt() < pt_bin2) return "SR3_bin3";
  FillCutflow(HNL_LeptonCore::SR3, w, "SR3_LepPt",param);

  Fill_RegionPlots(param,"InclusiveSRHM3" , TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);

  //double PhiComp = fabs(TVector2::Phi_mpi_pi( ( (NCand.Phi() - Wlep.Phi() ))));
  /// < 1.5 ? > 1.5
  //NCand.DeltaR(Wlep)

  FillCutflow(HNL_LeptonCore::ChannelDepSR3, w, GetChannelString(channel) +"_SR3", param);//Def          
  

  double Lep_LT = (leps[0]->Pt()+leps[1]->Pt());
  
  if(Modifier == "Binning_Version6"){

    if(Lep_LT > 400){
      if(N1cand.M() < 500.)   return "SR3_bin4";
      if(N1cand.M() < 750.)   return "SR3_bin5";
      return "SR3_bin6";
    }
    else{ 
      if(met2_st < 3){
	if(N1cand.M() < 500.)   return "SR3_bin7";
	if(N1cand.M() < 750.)   return "SR3_bin8";
	return "SR3_bin9";
      }
      else{
	if(N1cand.M() < 500.)   return "SR3_bin10";
	return "SR3_bin11";
      }
    }
  }



  if(Modifier == "Binning_Version5"){

    if(met2_st < 2){
      if(N1cand.M() < 500.)   return "SR3_bin4";
      if(N1cand.M() < 750.)   return "SR3_bin5";
      return "SR3_bin6";
    }
    else   if(met2_st < 7){
      if(N1cand.M() < 500.)   return "SR3_bin7";
      if(N1cand.M() < 750.)   return "SR3_bin8";
      return "SR3_bin9";
    }
    else{
      if(N1cand.M() < 750.)   return "SR3_bin10";
    }
    return "SR3_bin11";
  }


  if(Modifier == "Binning_Version1"){

    double L2T =  leps[1]->Pt();
    TString sbin="";
  
    if(L2T > 140)  sbin="4";
    else if(L2T > 80){
      if(N1cand.M() > 350.) {
	if(Wcand.M() > 600)  sbin="5";
	else  sbin="6";
      }
      else sbin="7";
    }
    else {
      if(N1cand.M() > 250.) {
	sbin="8";
      }  
      else {
	sbin="9";
      }
    }
    
    TString signal_region = "HNL_SR3";

    return "SR3_bin"+sbin;
  }
  if(Modifier == "Binning_Version2"){
    double LT = leps[0]->Pt() + leps[1]->Pt();
    TString sbin="";

    if(LT > 150){
      if(N1cand.M() > 350.) {
	if(Wcand.M() > 600)  {
	  sbin="4";
	}
	else  {
	  sbin="5";
	}
      }
      else {
	sbin="6";
      }
    }
    else {
      if(N1cand.M() > 250.) {
	sbin="7";
      }
      else {
	sbin="8";
      }
    }

    TString signal_region = "HNL_SR3";

    return "SR3_bin"+sbin;

  }

  if(Modifier == "Binning_Version3"){
    
    if(Wcand.M() < 300)  {
      return "false";
    }
    
        
    int nSRbins=5;
    
    //std::string::size_type sz;
    //double central_mass = std::stod(std::string(mass),&sz);
    double ml1jbins[nSRbins] = { 300., 400, 550, 700, 2000.};
    
    if(param.WriteOutVerbose == 0)FillHist( "LimitSR3/"+param.Name+"/N1Mass_Central",  N1cand.M(),  w, nSRbins-1, ml1jbins, "Reco M_{l1jj}");
    
    if(leps[0]->Pt() < pt_bin2  || leps[1]->Pt() < 20)  {
      
      return "false";
    }

    
    if(Wcand.M() < 400) {
      return "false";
    }
    if(N1cand.M() < 300)  {
      
      return "false";
    }
    
    if(channel==MuMu || channel == EE){
      
      if(met2_st > 7 ) {
	if(Wcand.M() > 600){
	  if(N1cand.M() < 500) return "SR3_bin4"; 
	  else return "SR3_bin5";
	}
	else return "SR3_bin6";
      }
      
      if(N1cand.M() < 370 || Wcand.M() < 600) return "SR3_bin7";
      if(Wcand.M() > 800) return "SR3_bin8";
      
      if(N1cand.M() < 500 ) return "SR3_bin9";
      if(N1cand.M() < 700 ) return "SR3_bin10";
      else return "SR3_bin11";
    }
    
  }

  if(Modifier == "Binning_Version4"){

    if(leps[1]->Pt() < 30) return "SR3_bin3";
    if(Wcand.M() < 300) return "SR3_bin3";
    if(Wcand.M() < 400) return "SR3_bin4";
    if(Wcand.M() < 800) {
      if(N1cand.M() < 500) return "SR3_bin5";
      if(N1cand.M() < 700) return "SR3_bin6";
      else return "SR3_bin7";
    }
    else{
      if(Wcand.M() < 700) return "SR3_bin8";
      if(Wcand.M() < 1000) return "SR3_bin9";
      return "SR3_bin10";
           
    }
  }

  return "false";
  
}





HNL_RegionDefinitionsOpt::HNL_RegionDefinitionsOpt(){
      


}
 
HNL_RegionDefinitionsOpt::~HNL_RegionDefinitionsOpt(){

}




bool HNL_RegionDefinitionsOpt::PassVBFInitial(vector<Jet>  JetColl){

  if(JetColl.size() < 2) return false;

  double maxDiJetDeta=0.;
  int ijet1(-1), ijet2(-1);
  for(unsigned int ij = 0; ij < JetColl.size()-1; ij++){
    for(unsigned int ij2 = ij+1; ij2 < JetColl.size(); ij2++){

      double deta = fabs(JetColl[ij].Eta() - JetColl[ij2].Eta());
      if(deta > maxDiJetDeta) {
        maxDiJetDeta=deta;
        ijet1=ij;
        ijet2=ij2;
      }
    }
  }

  if(maxDiJetDeta < 2.5) return false;
  Particle JJ = JetColl[ijet1] + JetColl[ijet2];
  if(JJ.M() < 450.) return false;

  return true;


}

bool HNL_RegionDefinitionsOpt::PassVBF(vector<Jet>  JetColl,std::vector<Lepton *> leps, double mjj_cut ,AnalyzerParameter param,double w , bool use_leadjets){

  if(leps.size() != 2) return false;
  double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );
  if(ll_dphi < 2.) return false;
  FillCutflow(HNL_LeptonCore::SR2, w, "SR2_DPhi",param);

  if( ( (*leps[0]) + (*leps[1]) ).M() < 20.) return false;
  FillCutflow(HNL_LeptonCore::SR2, w, "SR2_LLMass",param);

  if(JetColl.size() < 2) return false;
  FillCutflow(HNL_LeptonCore::SR2, w, "SR2_DiJet",param);


  double maxDiJetDeta=0.;
  int ijet1(-1), ijet2(-1);
  for(unsigned int ij = 0; ij < JetColl.size()-1; ij++){
    for(unsigned int ij2 = ij+1; ij2 < JetColl.size(); ij2++){

      double deta = fabs(JetColl[ij].Eta() - JetColl[ij2].Eta());
      if(deta > maxDiJetDeta) {
        maxDiJetDeta=deta;
        ijet1=ij;
        ijet2=ij2;
      }
    }
  }
  if(use_leadjets){ijet1=0;ijet2=1;}
  
  if(maxDiJetDeta < 2.5) return false;
  FillCutflow(HNL_LeptonCore::SR2, w, "SR2_DiJetEta",param);

  Particle JJ = JetColl[ijet1] + JetColl[ijet2];
  if(JJ.M() < mjj_cut) return false;
  FillCutflow(HNL_LeptonCore::SR2, w, "SR2_DiJetMass",param);

  double Av_JetEta= 0.5*(JetColl[ijet1].Eta()+ JetColl[ijet2].Eta());
  double zeppenfeld = TMath::Max((*leps[0]).Eta()  - Av_JetEta , (*leps[1]).Eta()  - Av_JetEta ) /maxDiJetDeta;

  if(zeppenfeld > 0.75) return false;

  return true;
}






