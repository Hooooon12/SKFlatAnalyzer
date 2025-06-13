#include "HNL_RegionDefinitions.h"


void HNL_RegionDefinitions::RunAllControlRegions(std::vector<Electron> electronsInitial, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, vector<Tau> TauColl_Cleaned,std::vector<Jet> AK4_JetCollLoose, std::vector<Jet> JetColl, std::vector<Jet> VBF_JetColl,   std::vector<FatJet> AK8_JetColl, std::vector<Jet> B_JetColl,  Event ev, Particle METv, AnalyzerParameter param, vector<TString> CRs, int nElRun_ForCF, float weight_ll ){



  std::vector<Electron> electrons;
  if(RunCF) {
    /// Add code to smear individual electron for CF Bkg
    for(unsigned int i=0; i<electronsInitial.size(); i++){
      Electron this_electron = electronsInitial.at(i);
      double ElEnergyShift = 1;
      
      if(int(i)==nElRun_ForCF) ElEnergyShift = GetShiftCFEl(this_electron, param.Electron_Tight_ID);
      
      this_electron.SetPtEtaPhiM( electronsInitial.at(i).Pt()*ElEnergyShift, electronsInitial.at(i).Eta(), electronsInitial.at(i).Phi(), electronsInitial.at(i).M() );
      electrons.push_back( this_electron);
    }
    METv = UpdateMETCF(METv,electronsInitial,electrons);
  }
  else  electrons = electronsInitial;


  vector<HNL_LeptonCore::Channel> channels = {GetChannelENum(param.Channel)};
 
  if(GetChannelENum(param.Channel) == HNL_LeptonCore::NONE){
    cout << "CHANNEL NOT SET" << endl;
    exit(EXIT_FAILURE);
  }


  if(run_Debug) cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;  
  int nlog(0);
  
  for(unsigned int ic = 0; ic < channels.size(); ic++){

    if(RunCF){
      if(channels[ic] == MuMu) continue;
      if(electrons_veto.size() !=2 && channels[ic] == EE) continue;
      if(electrons_veto.size() !=1 && muons_veto.size() !=1 && channels[ic] == EMu) return;
      if((electrons_veto.size() +muons_veto.size()) != 2) return;
    }
   
    if(run_Debug) cout << "HNL_RegionDefinitions::RunAllControlRegions [" << GetChannelString(channels[ic])<<" ]" << endl;
    
    HNL_LeptonCore::Channel dilep_channel   = channels[ic];
    HNL_LeptonCore::Channel trilep_channel  = GetTriLeptonChannel(dilep_channel);
    HNL_LeptonCore::Channel fourlep_channel = GetQuadLeptonChannel(dilep_channel);

    // Change Name of param to include Flavour channel                                                                                                                                                           
    HNL_LeptonCore::SearchRegion CutFlow_Region = ControlRegion;

    TString channel_string = GetChannelString(dilep_channel);
    param.CutFlowDir = "CutFlow";
    
    param.Name = param.Name  + "/"+channel_string;

    /// Set SR bin variable to keep jet selection consistent                                                                                                                                                                                  
    for(long unsigned int imu =0 ; imu <  muons.size(); imu++) muons[imu].SetHTOverPt(GetHT(AK4_JetCollLoose,AK8_JetColl));
    for(long unsigned int iel =0 ; iel < electrons.size() ; iel++) electrons[iel].SetHTOverPt(GetHT(AK4_JetCollLoose,AK8_JetColl));

    std::vector<Lepton *> LepsT       = MakeLeptonPointerVector(muons,     electrons,     param);
    std::vector<Lepton *> LepsV       = MakeLeptonPointerVector(muons_veto,electrons_veto,param);

    //// Set METST value after shifting Electrons                                                                                                                                                                                             
    ev.SetMET2ST(GetMET2ST(LepsT, JetColl, AK8_JetColl, METv));

    FillCutflow(CutFlow_Region, weight_ll, "NoCut", param);
   

    double weight_channel = weight_ll;

    if(!PassHEMVeto(LepsV,weight_channel)) continue;
    FillCutflow(CutFlow_Region, weight_channel, "HEMVeto", param);

    if(!PassMETFilter()) continue;

    FillCutflow(CutFlow_Region, weight_channel, "METFilter",param);

    if(run_Debug) {cout <<"RunAllControlRegions ["<< nlog<< "] : Pass METFilters" << endl;nlog++;}
    
    /// Filters events based on Conv/CF/Fake/Prompt
    
    bool Running_OSFake = (RunOSFake || (RunCR("OS_VR",CRs) && RunFake));
    
    if(!Running_OSFake){
      /// If running OS Region then fakes use Gen
      if(!PassGenMatchFilter(LepsT,param)) continue;
    }
    
    FillCutflow(CutFlow_Region, weight_channel, "GENMatched",param);
    
    if(run_Debug) cout << "HNL_RegionDefinitions::RunAllControlRegions [" << GetChannelString(channels[ic])<<" ]" << endl;
    
    //// Make Trilep/4 lep param
    AnalyzerParameter paramTrilep  = param;
    AnalyzerParameter paramQuadlep = param;

    paramTrilep.Channel  = GetChannelString(trilep_channel);
    paramTrilep.Name     = param.DefName + "/"+GetChannelString(trilep_channel);

    paramQuadlep.Channel = GetChannelString(fourlep_channel);
    paramQuadlep.Name    = param.DefName + "/"+GetChannelString(fourlep_channel) ;
    
    param.NameInclusive_Channel = param.DefName         + "/" + param.InclusiveChannelName();
    paramTrilep.NameInclusive_Channel = param.DefName   + "/" + paramTrilep.InclusiveChannelName();
    paramQuadlep.NameInclusive_Channel = param.DefName  + "/" + paramQuadlep.InclusiveChannelName();
   
    TString label    = param.Name;
    
    if(! (CheckLeptonFlavourForChannel(dilep_channel, LepsT) 
        || CheckLeptonFlavourForChannel(trilep_channel, LepsT) 
        || CheckLeptonFlavourForChannel(fourlep_channel, LepsT))) {
      continue;

    }


    FillCutflow(CutFlow_Region, weight_channel, "LeptonFlavour",param);

    if(TauColl_Cleaned.size() > 0) continue;

    FillCutflow(CutFlow_Region, weight_channel, "TauVeto",param);
    
    if(run_Debug) {cout <<"RunAllControlRegions ["<< nlog<< "] pass Lep Flavour" << endl;nlog++;}
    
    if(dilep_channel == EMu ){
      if(param.TriggerSelection == "POGSgLep"){
        /// For Single Lep its on OR of two PD so need to add veto logic to not double ocunt in data
        if ( PassMultiDatasetTriggerSelection(dilep_channel, ev, LepsT, "POGSgMu", "POGSglEl")) continue;
      }
      else{
        if (!PassTriggerSelection(dilep_channel, ev, LepsT,param.TriggerSelection)) continue;
      }
    }
    else{
      if (!PassTriggerSelection(dilep_channel, ev, LepsT,param.TriggerSelection)) continue;
    }
    
    EvalTrigWeight(dilep_channel, muons,electrons,param, ev, weight_channel);

    FillCutflow(CutFlow_Region, weight_channel, "Trigger",param);
    double weight_OS = weight_channel;
    /// For OS Fakes use SS TT events - VV , but RunFake uses LL so need to apply Tight ID 
    bool PassTight = true;
    if(RunFake){
      if(Running_OSFake && (dilep_channel==EE || dilep_channel==MuMu) ){
  
        if(dilep_channel==EE   && !SameCharge(electrons))   continue;
        if(dilep_channel==MuMu && !SameCharge(muons))       continue;
        
        for(auto ilep : muons) {
          if(!ilep.PassID(param.Muon_Tight_ID))  PassTight=false;
        }
        for(auto ilep : electrons) {
          if(!ilep.PassID(param.Electron_Tight_ID))PassTight=false;
        }
        
        if(!IsData) weight_OS *=-1;
        else        weight_OS =1;
      }
 
      else {
        if(IsData){
          weight_channel = GetFakeWeight(LepsT, param );

          TString nT ="";
          for(auto ilep : muons) {
            if(ilep.PassID(param.Muon_Tight_ID)) nT=nT+"T";
            else nT=nT+"L";
          }
          for(auto ilep : electrons) {
            if(ilep.PassID(param.Electron_Tight_ID))  nT=nT+"T";
            else nT=nT+"L";
          }
    
          if(SameCharge(LepsT) && LepsV.size() == 2){
            if(nT=="TT")  FillHist(  "FAKES/"+param.Name+"/NTL",   1,  1,  5, 0, 5.);
            if(nT=="TL")  FillHist(  "FAKES/"+param.Name+"/NTL",   2,  weight_channel,  5, 0, 5.);
            if(nT=="LT")  FillHist(  "FAKES/"+param.Name+"/NTL",   3,  weight_channel,  5, 0, 5.);
            if(nT=="LL")  FillHist(  "FAKES/"+param.Name+"/NTL",   4,  weight_channel,  5, 0, 5.);
            if(nT=="TT")  FillHist(  "FAKES/"+param.Name+"/UnWeighted_NTL",   1,  1,  5, 0, 5.);
            if(nT=="TL")  FillHist(  "FAKES/"+param.Name+"/UnWeighted_NTL",   2,  1,  5, 0, 5.);
            if(nT=="LT")  FillHist(  "FAKES/"+param.Name+"/UnWeighted_NTL",   3,  1,  5, 0, 5.);
            if(nT=="LL")  FillHist(  "FAKES/"+param.Name+"/UnWeighted_NTL",   4,  1,  5, 0, 5.);
          }
    
          if(Running_OSFake) weight_OS = weight_channel;
          if(run_Debug) {cout <<"RunAllControlRegions ["<< nlog<< "] Fake Weight = " << weight_channel << endl;nlog++;}
        }
      }
    }
    
    if(RunCR("BDTCheck",CRs)){
      double BDTweight_channel = weight_channel;
      if( B_JetColl.size() > 0)  return;
      if (!PassTriggerSelection(dilep_channel, ev, LepsT,"POG")) return;
      if(LepsV.size() ==2){
        Particle  ll = (*LepsV[0]);
        ll+= (*LepsV[1]);
        if(!IsData && ll.Charge() != 0) return;
        if(IsData  && ll.Charge() != 0) BDTweight_channel = -1;
        if (fabs(ll.M()-M_Z) <  10) {
          for(auto ilep : electrons) {
            if(!IsData && !ilep.IsPrompt()) continue;
            if(!IsData && ilep.LeptonIsCF()) continue;
            /// dont subtract SS for high pt 
            if(IsData  && ll.Charge() != 0 && ilep.Pt() > 100) continue;
            TString CFMVA = "";
            TString ConvMVA = "";
            TString FakeMVA = "";
            if( FindHEMElectron (ilep)) FillBDTHists(ilep,"HEM",BDTweight_channel);
            if( FindHEMElectron (ilep))  continue;
            TString ptstring = "Pt1";
            if(ilep.Pt() < 30 ) ptstring = "Pt1";
            else if(ilep.Pt() < 150 ) ptstring = "Pt2";
            else ptstring = "Pt3";
            TString etastring = ilep.sEtaRegion();
            FillBDTHists(ilep,ptstring+"_"+etastring,BDTweight_channel);
          }
        }
      }      
      return;
    }
  
    ////  AN [1]  AN2019_089 WZ/WZ VBF
    ////  AN [2]  AN2021_008 WW Peking HNL
    ////  AN [3]  AN2020_045 SSWW

    vector<TString> passedMain;
    vector<TString> passedFull;
    if(RunCR("OS_VR",CRs)){
      //// OS L+L-
      
      if(!ConversionSplitting(LepsT,RunConv,2,param)) return;

      FillCutflow(CutFlow_Region, weight_OS, "OS_VR",param);
    
      if(PassTight && FillZCRPlots     (dilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_OS)) passedFull.push_back("Z_CR");
      if(PassTight && FillTopCRPlots   (dilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_OS)) passedFull.push_back("Top_CR");
      if(PassTight && FillTopCR2Plots (dilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_OS))  passedFull.push_back("Top_CR2");
      
    }
    
    if(RunCR("LLL_VR",CRs)){

      FillCutflow(CutFlow_Region, weight_channel, "VV_VR",param);

      // LLL / LLLL 
      if(ConversionSplitting(LepsT,RunConv,4,param)){
        //////  SR1+3 ZZ
        if(FillZZCRPlots (fourlep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, paramQuadlep, weight_channel)) passedMain.push_back("ZZ_CR");
      }
      
      if(ConversionSplitting(LepsT,RunConv,3,param)){
	FillCutflow(CutFlow_Region, weight_channel, "VG_VR",param);
	
	//if(FillWGCRPlots( trilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, paramTrilep, weight_channel)) passedFull.push_back("WG_CR");
	if(FillZGCRPlots( trilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, paramTrilep, weight_channel)) passedMain.push_back("ZG_CR");
	
	////// WZ CR [SR1+3]
	
	if(FillWZCRPlots (trilep_channel,  LepsT, LepsV,TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, paramTrilep, weight_channel))  {
	  if(AK8_JetColl.size() > 0) passedMain.push_back("WZ_SR1");
	  else if(!PassVBF(VBF_JetColl,LepsT,500))  passedMain.push_back("WZ_SR3");
	}
	//// AN[2] Table 13 
	if(FillWZVBFCRPlots      (trilep_channel, LepsT, LepsV,TauColl_Cleaned, VBF_JetColl, AK8_JetColl, B_JetColl, ev, METv, paramTrilep, weight_channel)) passedMain.push_back("WZ_SR2");
	/// AN[3] Table 15
	if(FillWZVBF2CRPlots      (trilep_channel, LepsT, LepsV,TauColl_Cleaned, VBF_JetColl, AK8_JetColl, B_JetColl, ev, METv, paramTrilep, weight_channel)) passedFull.push_back("WZ_SR2b");

        ///// Top CR
        ///// AN [1] :   FillWZBCRPlots is Table11 WZ SR anti-bjet AM2019_089_v7 (Jet cut 50 -> 30 ; m(lll) > 100 ->105)
        if(FillWZBCRPlots      (trilep_channel, LepsT, LepsV,TauColl_Cleaned, VBF_JetColl, AK8_JetColl, B_JetColl, ev, METv, paramTrilep, weight_channel)) passedMain.push_back("WZB_CR");

        ////// Jiwhans functions
        if(FillZ_ElNPCRPlots   (trilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, paramTrilep, weight_channel))     passedFull.push_back("Z_NP_El_CR");
        if(FillZ_MuonNPCRPlots (trilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, paramTrilep, weight_channel))     passedFull.push_back("Z_NP_Mu_CR");
        if(FillTopNPCRPlots    (trilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, paramTrilep, weight_channel))     passedFull.push_back("Top_NP_CR");
      }
    }


    bool SS_CR = RunCR("SS_CR",CRs)  || RunCR("SS_CR1",CRs) || RunCR("SS_CR2",CRs) || RunCR("SS_CR3",CRs) ;
    
    
    if(SS_CR || RunCR("VBF_CR",CRs)){
      
      if(SS_CR)  FillCutflow(CutFlow_Region, weight_channel, "SS_CR",param);
      if(RunCR("VBF_CR",CRs)) FillCutflow(CutFlow_Region, weight_channel, "VBF_CR",param);

      if(!ConversionSplitting(LepsT,RunConv,2,param))  return;

      if(RunCF&& IsData){    
        if(LepsT.size() == 2){
          if(dilep_channel == MuMu)       continue;
          if(IsData && SameCharge(LepsT)) continue;
          if(IsData){
            weight_channel = GetCFWeightElectron(LepsT, param,nElRun_ForCF, true);
            FillWeightHist(param.ChannelDir()+"/CFWeight", weight_channel);
          }
          if(!IsData && !SameCharge(LepsT)) continue;
        }
        else continue;
      }
      else if(LepsT.size() == 2){

	std::vector<TString> WGSamples = {"WGJJToLNu","WGToLNuG_MG","WGToLNuG"};
	if (std::find(WGSamples.begin(), WGSamples.end(), MCSample) != WGSamples.end()) {
	  //// Set Weight to half and not apply SS cut
	  weight_channel *= 0.5;
	}
	else {
	  if(!SameCharge(LepsT)) continue;
	}
      }      
    }
    
    if(RunCR("VBF_CR",CRs)){

      // FillWWCR2Plots   An [3] Table 13 WW SR ; HighMET
      if(FillWWCR2Plots  (dilep_channel, LepsT, LepsV,TauColl_Cleaned, VBF_JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_channel))      passedFull.push_back("WpWp_CR");
      
      /// AN [1] Table 11 :  FillWWCRNPPlots is WW SR with anti-bjet cut [jet pt 50->30mjj 500 -> 750
      if(FillWWCRNPPlots (dilep_channel, LepsT, LepsV,TauColl_Cleaned, VBF_JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_channel))      passedFull.push_back("WpWp_CR_NP");
      /// AN [2]  Table 14    150-750 MJJ 
      if(FillWWCRNP2Plots(dilep_channel, LepsT, LepsV,TauColl_Cleaned, VBF_JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_channel))      passedFull.push_back("WpWp_CR_NP2");
      /// AN [3]  Table 15    >500 MJJ 
      if(FillWWCRNP3Plots(dilep_channel, LepsT, LepsV,TauColl_Cleaned, VBF_JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_channel))      passedFull.push_back("WpWp_CR_NP3");
    }
    
    if(SS_CR){

      //cout << param.Name << " Event " << event << endl;
      //// These are looser Selections than final limit input, 
      //// i.e., FillHighMassSR1CRPlots has no mW cut, so CR plots have more stats 
      
      if(FillSSPreselectionPlots(dilep_channel,    LepsT, LepsV,TauColl_Cleaned, JetColl,     AK8_JetColl, B_JetColl, ev, METv, param, weight_channel))	{
	passedFull.push_back("SSPresel");
	
	if(RunCR("SS_CR",CRs) || RunCR("SS_CR1",CRs)){

	  // Fill High Mass SR1 and SR2 CR Plots

	  TString passSR1 = FillHighMassSR1CRPlots(dilep_channel, LepsT, LepsV, TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_channel);
	  if (passSR1 != "false"){
	    passedMain.push_back(passSR1);
	  }
	}	   
        if(RunCR("SS_CR",CRs) || RunCR("SS_CR2",CRs)){
	  TString  passSR2 = FillHighMassSR2CRPlots(dilep_channel, LepsT, LepsV, TauColl_Cleaned, VBF_JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_channel);
	  if ( passSR2.Contains("SR2")){
            passedMain.push_back(passSR2);
          }
	  /// Running SS_CR and MJJ < 500 in CR dont run CR3
	  if ( passSR2 == "NULL") return;
	}

	if(RunCR("SS_CR",CRs) || RunCR("SS_CR3",CRs)){
	  
	  // Check VBF condition and fill SR3 CR Plots
	  if (!PassVBF(VBF_JetColl, LepsT, 0)) {
	    TString  passSR3 = FillHighMassSR3CRPlots(dilep_channel, LepsT, LepsV, TauColl_Cleaned, JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_channel);
	    if ( passSR3 != "false"){
	      passedMain.push_back(passSR3);
	    }
	  }
	}     
      }
      if(RunCR("SS_CR",CRs)){
	if(FillHighMass1JetCRPlots(dilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl,    AK8_JetColl, B_JetColl, ev, METv, param, weight_channel)) passedFull.push_back("HighMass1Jet_CR");
	if(FillHighMassBJetCRPlots(dilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl,    AK8_JetColl, B_JetColl, ev, METv, param, weight_channel)) passedFull.push_back("HighMassBJet_CR");
	if(FillHighMassNPCRPlots(dilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl,      AK8_JetColl, B_JetColl, ev, METv, param, weight_channel)) passedFull.push_back("HighMassNP_CR");
	
	FillSSZPeakCRPlots(dilep_channel, LepsT, LepsV,TauColl_Cleaned, JetColl, VBF_JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_channel);
	
	//// RunMainRegionCode(false runs CR version of SR1/2/3
	if(!HasFlag("ScanFakes")) {
	  RunMainRegionCode(false, dilep_channel, Inclusive, LepsT, LepsV,TauColl_Cleaned,JetColl, VBF_JetColl, AK8_JetColl, B_JetColl, ev, METv, param, weight_channel);
	}
      }
    }
    
    
    if(HasFlag("CR_Summary")){
      
      TString ControlLabel = "";
      vector<TString> cutlabels = {};
      vector<TString> cutlabels_main = {};
      
      if(RunCR("OS_VR",CRs)){
	cutlabels.push_back("Z_CR");
	cutlabels.push_back("Top_CR");
	cutlabels.push_back("Top_CR2");
	ControlLabel+="OS_VR";
      }
      
      if(RunCR("LLL_VR",CRs)) {
	cutlabels_main.push_back("ZZ_CR");
	cutlabels_main.push_back("ZZ_SR2");
	cutlabels_main.push_back("WG_CR");
	cutlabels_main.push_back("ZG_CR");
	cutlabels_main.push_back("WZ_SR1");
	cutlabels_main.push_back("WZ_SR2");
	cutlabels_main.push_back("WZ_SR3");
	cutlabels_main.push_back("WZB_CR");
	
	
	
	cutlabels.push_back("WZ_SR2b");
	cutlabels.push_back("Z_NP_El_CR");
	cutlabels.push_back("Z_NP_Mu_CR");
	cutlabels.push_back("Top_NP_CR");
	ControlLabel+="LLL";
      }
      
      if(RunCR("SS_CR",CRs)) {
	cutlabels_main.push_back("SR1_InvMET");
	cutlabels_main.push_back("SR1_InvBJet");
	cutlabels_main.push_back("SR2_InvMET");
	cutlabels_main.push_back("SR2_InvBJet");
	cutlabels_main.push_back("SR3_InvMET");
	cutlabels_main.push_back("SR3_InvBJet");
	
	cutlabels.push_back("SSPresel");
	cutlabels.push_back("HighMass1Jet_CR");
	cutlabels.push_back("HighMassBJet_CR");
	cutlabels.push_back("HighMassNP_CR");
	
	ControlLabel+="SS";
      }
      
      if(RunCR("VBF_CR",CRs)){
	cutlabels.push_back("WpWp_CR");
	cutlabels.push_back("WpWp_CR_NP");
	cutlabels.push_back("WpWp_CR_NP2");
	cutlabels.push_back("WpWp_CR_NP3");
	ControlLabel+="SSVBF";
      }
      
      /// Add Main CR list to Full
      for(auto i : passedMain) passedFull.push_back(i);
      for(auto i : cutlabels_main) cutlabels.push_back(i);
      
      
      TString ControlLabel_ChargeDep = ControlLabel;
      bool isPositive = (LepsT[0]->Charge() > 0);
      ControlLabel_ChargeDep += isPositive ? "_Plus" : "_Minus";
      
      
      for(auto ip : passedFull) {
	FillCutflow(param.CutFlowDirChannel(),    ControlLabel+"_ControlRegions", weight_channel, cutlabels,ip);
	FillCutflow(param.CutFlowDirIncChannel(), ControlLabel+"_ControlRegions", weight_channel, cutlabels,ip);
	
	FillCutflow(param.CutFlowDirChannel(),    ControlLabel_ChargeDep+"_ControlRegions", weight_channel, cutlabels,ip);
	FillCutflow(param.CutFlowDirIncChannel(), ControlLabel_ChargeDep+"_ControlRegions", weight_channel, cutlabels,ip);
	
      FillCutflow(HNL_LeptonCore::CRFull, weight_channel, ip,param);
      }
      
      for(auto ip : passedMain) {
	FillCutflow(param.CutFlowDirChannel(), ControlLabel+"_SelectedControlRegions", weight_channel, cutlabels_main,ip);
	FillCutflow(param.CutFlowDirIncChannel(), ControlLabel+"_SelectedControlRegions", weight_channel, cutlabels_main,ip);
	
	FillCutflow(param.CutFlowDirChannel(),    ControlLabel_ChargeDep+"_SelectedControlRegions", weight_channel, cutlabels,ip);
	FillCutflow(param.CutFlowDirIncChannel(), ControlLabel_ChargeDep+"_SelectedControlRegions", weight_channel, cutlabels,ip);
	
	FillCutflow(HNL_LeptonCore::CR, weight_channel, ip,param);
      }
      
      
      
      for(unsigned int ipass =0; ipass < passedFull.size();ipass++){
	//void AnalyzerCore::FillTypeCutflow(TString histname, double weight, vector<TString> lables, TString label1, TString label2){                                                                                 
	for(unsigned int ipass2=ipass+1; ipass2 < passedFull.size(); ipass2++){
	  FillTypeCutflow(channel_string+"CR_Correlation"+param.Name, weight_channel, cutlabels, passedFull[ipass], passedFull[ipass2]);
	}
      }
    }//// summary plot
    
  }
  return;
}



bool HNL_RegionDefinitions::FillTopCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , std::vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  /// OS Top CR
  
  HNL_LeptonCore::SearchRegion Reg = TopCR;
  FillCutflow(Reg, w, "Step0",param);

  int NB_JetColl = B_JetColl.size();
  Particle  ll   = (*leps[0]) + (*leps[1]);

  if (!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 2) return false;
  FillCutflow(Reg, w, "Step2",param);

  if (METv.Pt() < 50)        return false;
  FillCutflow(Reg, w, "Step3",param);

  if (NB_JetColl != 1)       return false;
  FillCutflow(Reg, w, "Step4",param);

  if (JetColl.size() < 4)    return false;
  FillCutflow(Reg, w, "Step5",param);

  if (channel != EMu){
    //if (fabs(ll.M()-M_Z) < M_ZWINDOW) return false; //JH
    if (ll.M()  < 110)       return false;
  }
  FillCutflow(Reg, w, "Step6",param);

  if(AK8_JetColl.size() > 0) return false;

  if(run_Debug){
    cout << "HNL_Top_TwoLepton_CR " << event << endl;
    for(auto ilep: leps) cout << "HNL_Top_TwoLepton_CR Type " <<  ilep->LeptonGenType() << endl;
  }

  if(RunFake) {
    if(SameCharge(leps)) Fill_RegionPlots(param,"HNL_OS_Top_TwoLepton_CR" ,  taus,JetColl  ,AK8_JetColl,  leps,  METv, nPV, w);
  }
  else {
    if(!SameCharge(leps)) Fill_RegionPlots(param,"HNL_OS_Top_TwoLepton_CR" ,  taus,JetColl  ,AK8_JetColl,  leps,  METv, nPV, w);
  }
  return true;
}



bool HNL_RegionDefinitions::FillTopCR2Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){
  
  /// OS Top CR

  HNL_LeptonCore::SearchRegion Reg = TopNPCR2;
  FillCutflow(Reg, w, "Step0",param);

  int NB_JetColl = B_JetColl.size();
  Particle  ll   = (*leps[0]) + (*leps[1]);
  
  if (!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 2) return false;
  FillCutflow(Reg, w, "Step2",param);

  if (METv.Pt()  < 40)       return false;
  FillCutflow(Reg, w, "Step3",param);

  if (NB_JetColl < 2)        return false;
  FillCutflow(Reg, w, "Step4",param);

  if (JetColl.size() < 4)    return false;
  FillCutflow(Reg, w, "Step5",param);


  if (channel != EMu){
    //if (fabs(ll.M()-M_Z) < M_ZWINDOW) return false; //JH
    if (ll.M()  < 110)       return false;
  }
  FillCutflow(Reg, w, "Step6",param);
  
  if(AK8_JetColl.size() > 0) return false;

  if(run_Debug){
    cout << "HNL_Top_TwoLepton_CR " << event << endl;
    for(auto ilep: leps) cout << "HNL_Top_TwoLepton_CR Type " <<  ilep->LeptonGenType() << endl;
    //PrintGen(gens);                                                                                                                                                                                              
  }
    
  if(RunFake) {
    if(SameCharge(leps)) Fill_RegionPlots(param,"HNL_OS_Top2b_TwoLepton_CR" ,  JetColl  ,AK8_JetColl,  leps,  METv, nPV, w);
  }
  else {
    if(!SameCharge(leps)) Fill_RegionPlots(param,"HNL_OS_Top2b_TwoLepton_CR" ,  JetColl  ,AK8_JetColl,  leps,  METv, nPV, w);
  }
  return true;
}


bool HNL_RegionDefinitions::FillTopAK8CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , vector<Tau>& taus,std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = TopAK8NPCR;
  FillCutflow(Reg, w, "Step0",param);

  if (!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 2) return false;
  FillCutflow(Reg, w, "Step2",param);

  if (METv.Pt() < 50) return false;
  FillCutflow(Reg, w, "Step3",param);

  int NB_JetColl = B_JetColl.size();
  if(NB_JetColl != 1) return false;
  FillCutflow(Reg, w, "Step4",param);

  if(AK8_JetColl.size() == 0) return false;
  FillCutflow(Reg, w, "Step5",param);

  Particle  ll   = (*leps[0]) + (*leps[1]);
  if (channel != EMu){
    //if (fabs(ll.M()-M_Z) < M_ZWINDOW) return false; //JH
    if (ll.M()  < 110)      return false;
  }
  FillCutflow(Reg, w, "Step6",param);
  
  if(run_Debug){
    cout << "HNL_TopAK8_TwoLepton_CR " << event << endl;
    for(auto ilep: leps) cout << "HNL_TopAK8_TwoLepton_CR Type " <<  ilep->LeptonGenType() << endl;
    //PrintGen(gens);                                                                                                                                                                                                                                    
  }
  if(RunFake) {
    if(SameCharge(leps)) Fill_RegionPlots(param,"HNL_OS_TopAK8_TwoLepton_CR" ,  JetColl  ,AK8_JetColl,  leps,  METv, nPV, w);
  }
  else{
    if(!SameCharge(leps))Fill_RegionPlots(param,"HNL_OS_TopAK8_TwoLepton_CR" ,  JetColl  ,AK8_JetColl,  leps,  METv, nPV, w);
  }
  return true;
}


bool HNL_RegionDefinitions::FillTopNPCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = TopNPCR;
  FillCutflow(Reg, w, "Step0",param);

  if (leps_veto.size() != 3) return false;
  FillCutflow(Reg, w, "Step1",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step2",param);

  if(!HasOSSFPair(leps)) return false;
  FillCutflow(Reg, w, "Step3",param);

  if(fabs(GetMassBestZ(leps,true) -M_Z) < 15)  return false;
  FillCutflow(Reg, w, "Step4",param);

  Particle lll = (*leps[0]) + (*leps[1])+ (*leps[2]);
  bool InZmass_lll_Window = (fabs(lll.M() - M_Z) < 15);
  if(InZmass_lll_Window) return false;
  FillCutflow(Reg, w, "Step5",param);
  
  if(B_JetColl.size() == 0) return false;
  FillCutflow(Reg, w, "Step6",param);

  if(HasLowMassOSSF(leps,M_CUT_LL))  return false;
  FillCutflow(Reg, w, "Step7",param);


  if(AK8_JetColl.size() > 0) return false;

  Fill_RegionPlots(param,"HNL_TopNP_ThreeLepton_CR" ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);

  return true;

}


bool HNL_RegionDefinitions::FillZ_MuonNPCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , vector<Tau>& taus,std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = ZNPMuCR;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  if (leps_veto.size() != 3) return false;
  FillCutflow(Reg, w, "Step1",param);


  double metcut  = (UseMET2ST) ? 5 : 30.;
  double mtcut  = 30.;

  if(UseMET2ST  && ev.MET2ST() > metcut)  return false;
  if(!UseMET2ST && METv.Pt()   > metcut) return false;

  FillCutflow(Reg, w, "Step2",param);

  if(!ZmassOSSFWindowCheck(leps,M_ZWINDOW_CR)) return false;
  FillCutflow(Reg, w, "Step3",param);
  
  if(GetIndexNonBestZ(leps,M_ZWINDOW_CR) < 0) return false;
  FillCutflow(Reg, w, "Step4",param);

  if(M_T((*leps[GetIndexNonBestZ(leps,M_ZWINDOW_CR)]), METv) > mtcut) return false;
  FillCutflow(Reg, w, "Step5",param);
  
  Lepton* noZLep = leps[GetIndexNonBestZ(leps,M_ZWINDOW_CR)];
  if(noZLep->LeptonFlavour() != Lepton::MUON) return false;
  FillCutflow(Reg, w, "Step6",param);

  if(run_Debug){
    cout << "HNL_ZNP_ThreeLepton_CR " << param.Name << " " << event  << endl;
    for(auto ilep: leps) cout << "HNL_ZNP_ThreeLepton_CR Type " <<  ilep->LeptonGenType() << endl;
  }

  if(AK8_JetColl.size() == 1)   Fill_RegionPlots(param,"HNL_ZNPMu_AK8_ThreeLepton_CR" ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);
  else Fill_RegionPlots(param,"HNL_ZNPMu_ThreeLepton_CR" ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);

  return true;

}

bool HNL_RegionDefinitions::FillZ_ElNPCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , vector<Tau>& taus,std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = ZNPElCR;
  FillCutflow(Reg, w, "Step0",param);

  if (leps_veto.size() != 3) return false;

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step1",param);

  double metcut  = (UseMET2ST) ? 5 : 30.;
  double mtcut  = 30.;


  if(UseMET2ST  && ev.MET2ST() > metcut)  return false;
  if(!UseMET2ST && METv.Pt()   > metcut) return false;
  FillCutflow(Reg, w, "Step2",param);

  if(!ZmassOSSFWindowCheck(leps,15)) return false;
  FillCutflow(Reg, w, "Step3",param);

  if(GetIndexNonBestZ(leps,M_ZWINDOW_CR) < 0) return false;
  FillCutflow(Reg, w, "Step4",param);

  if(M_T((*leps[GetIndexNonBestZ(leps,M_ZWINDOW_CR)]), METv) > mtcut) return false;
  FillCutflow(Reg, w, "Step5",param);


  Lepton* noZLep = leps[GetIndexNonBestZ(leps,M_ZWINDOW_CR)];
  if(noZLep->LeptonFlavour() == Lepton::MUON) return false;
  FillCutflow(Reg, w, "Step6",param);

  if(run_Debug){
    cout << "HNL_ZNP_ThreeLepton_CR " << param.Name << " " << event  << endl;
    for(auto ilep: leps) cout << "HNL_ZNP_ThreeLepton_CR Type " <<  ilep->LeptonGenType() << endl;
  }

  if(AK8_JetColl.size() == 1) 	  Fill_RegionPlots(param,"HNL_ZNPEl_AK8_ThreeLepton_CR", taus ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);
  
  else Fill_RegionPlots(param,"HNL_ZNPEl_ThreeLepton_CR", taus ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);

  return true;

}

bool HNL_RegionDefinitions::FillZCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , std::vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl,  std::vector< Jet>& B_JetColl,Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = ZCR;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 2)        return false;
  FillCutflow(Reg, w, "Step2",param);
  
  int NB_JetColl = B_JetColl.size();
  Particle  ll = (*leps[0]) + (*leps[1]);

  if(NB_JetColl > 0)                return false;
  FillCutflow(Reg, w, "Step3",param);

  
  if (ll.M() < 50 ) return false;


  //// Plot No DY corr...                                                                                                                                                                                                                                                                                                  
  //// No Trigger SF                                                                                                                                                                                                                                                                                                       
  //// Trigger Only                                                                                                                                                                                                                                                                                                        
  //// Trigger + Reco                                                                                                                                                                                                                                                                                                      
  //// Trigger + reco + ID                                                                                                                                                                                                                                                                                                 

  // Define selection labels                                                                                                                                                                                                                                                                                               
  TString Label_sel[] = {
    "_NoCorr_DefaultWeight",
    "_NoCorr_TriggerSF",
    "_NoCorr_TriggerSF_RecoSF",
    "_NoCorr_TriggerSF_RecoSF_IDSF",
    "_NoCorr_TriggerSF_RecoSF_IDSF_3bin",
    "_Corr1_DefaultWeight",
    "_Corr1_TriggerSF",
    "_Corr1_TriggerSF_RecoSF",
    "_Corr1_TriggerSF_RecoSF_IDSF",
    "_Corr1_TriggerSF_RecoSF_IDSF_3bin",
    "_Corr2_DefaultWeight",
    "_Corr2_TriggerSF",
    "_Corr2_TriggerSF_RecoSF",
    "_Corr2_TriggerSF_RecoSF_IDSF",
    "_Corr2_TriggerSF_RecoSF_IDSF_3bin",
    "_Corr3_DefaultWeight",
    "_Corr3_TriggerSF",
    "_Corr3_TriggerSF_RecoSF",
    "_Corr3_TriggerSF_RecoSF_IDSF",
    "_Corr3_TriggerSF_RecoSF_IDSF_3bin",
    "_Corr4_DefaultWeight",
    "_Corr4_TriggerSF",
    "_Corr4_TriggerSF_RecoSF",
    "_Corr4_TriggerSF_RecoSF_IDSF",
    "_Corr4_TriggerSF_RecoSF_IDSF_3bin"
  };

  double weight_corr = param.w.z0weight*param.w.zptweight*param.w.weakweight;
  double weight_SF   = param.w.muonIDSF*param.w.muonRECOSF*param.w.muonTrackerSF*param.w.triggerSF;
  double IDSF2 = 1;

  for(auto lep : leps){
    if(!IsData)IDSF2*= mcCorr->MuonID_SF( "NUM_2bins_HNL_ULID_"+GetYearString(), abs(lep->Eta()), lep->Pt(), 0);
  }
  // Define weights                                                                                                                                                                                                                                                                                                        
  double def_weight_level1       = w/(weight_corr*weight_SF); /// No SF No Z Weight
  double def_weight_level2       = w/(param.w.zptweight*weight_SF); // No SF  No ZPt weight
  double def_weight_level3       = def_weight_level2 * param.w.zptweight_g;
  double def_weight_level4       = def_weight_level2 * param.w.zptweight_gy;
  double def_weight_level5       = def_weight_level2 * param.w.zptweight_gym;

  double os_weight[] = {def_weight_level1,
      def_weight_level1*param.w.triggerSF,
      def_weight_level1*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF,
      def_weight_level1*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF*param.w.muonIDSF,
      def_weight_level1*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF*IDSF2,

      def_weight_level2,
      def_weight_level2*param.w.triggerSF,
      def_weight_level2*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF,
      def_weight_level2*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF*param.w.muonIDSF,
      def_weight_level2*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF*IDSF2,

      def_weight_level3,
      def_weight_level3*param.w.triggerSF,
      def_weight_level3*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF,
      def_weight_level3*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF*param.w.muonIDSF,
      def_weight_level3*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF*IDSF2,

      def_weight_level4,
      def_weight_level4*param.w.triggerSF,
      def_weight_level4*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF,
      def_weight_level4*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF*param.w.muonIDSF,
      def_weight_level4*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF*IDSF2,

                        def_weight_level5,
                        def_weight_level5*param.w.triggerSF,
                        def_weight_level5*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF,
                        def_weight_level5*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF*param.w.muonIDSF,
                        def_weight_level5*param.w.triggerSF*param.w.muonRECOSF*param.w.muonTrackerSF*IDSF2,

  };

  

  if (RunFake) {
    if (SameCharge(leps)) {
      for (int i = 0; i < 25; ++i) {
        Fill_RegionPlots(param, "HNL_OS_FullMass_" + Label_sel[i] + "TwoLepton_CR", taus,
                         JetColl, AK8_JetColl, leps, METv, nPV, os_weight[i]);
      }
    }
  }

  else if(!SameCharge(leps)){

    for (int i = 0; i < 25; ++i) {
      Fill_RegionPlots(param, "HNL_OS_FullMass_" + Label_sel[i] + "TwoLepton_CR", taus,
                       JetColl, AK8_JetColl, leps, METv, nPV, os_weight[i]);
    }
  }
  
  if (fabs(ll.M()-M_Z) > M_ZWINDOW) return false;
  FillCutflow(Reg, w, "Step4",param);

  if(AK8_JetColl.size() > 0) return false;

  if(run_Debug){
    cout << "HNL_Z_TwoLepton_CR " << param.Name << " " << event  << endl;
    for(auto ilep: leps) cout << "HNL_Z_TwoLepton_CR Type " <<  ilep->LeptonGenType() << endl;
  }

  if (RunFake) {
    if (SameCharge(leps)) {
      for (int i = 0; i < 25; ++i) {
  Fill_RegionPlots(param, "HNL_OS_Z_" + Label_sel[i] + "TwoLepton_CR", taus,
       JetColl, AK8_JetColl, leps, METv, nPV, os_weight[i]);
      }
    }
  }
  
  else if(!SameCharge(leps)){

    for (int i = 0; i < 25; ++i) {
      Fill_RegionPlots(param, "HNL_OS_Z_" + Label_sel[i] + "TwoLepton_CR", taus,
           JetColl, AK8_JetColl, leps, METv, nPV, os_weight[i]);
    }
  }

  
  return true;

}


bool HNL_RegionDefinitions::FillZAK8CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto, std::vector<Tau>& taus,  std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl,  std::vector< Jet>& B_JetColl,Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = ZAK8CR;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 2) return false;
  FillCutflow(Reg, w, "Step2",param);

  if(METv.Pt() > 30) return false;
  FillCutflow(Reg, w, "Step3",param);

  int NB_JetColl = B_JetColl.size();
  if(NB_JetColl > 0) return false;
  FillCutflow(Reg, w, "Step4",param);

  if(AK8_JetColl.size() == 0) return false;
  FillCutflow(Reg, w, "Step5",param);

  Particle  ll = (*leps[0]) + (*leps[1]);

  if (fabs(ll.M()-M_Z) > M_ZWINDOW) return false;
  FillCutflow(Reg, w, "Step6",param);

  if(run_Debug){
    cout << "HNL_ZAK8_TwoLepton_CR " << param.Name << " " << event  << endl;
    for(auto ilep: leps) cout << "HNL_ZAK8_TwoLepton_CR Type " <<  ilep->LeptonGenType() << endl;
  }

  if(RunFake){
    if(SameCharge(leps))Fill_RegionPlots(param,"HNL_OS_ZAK8_TwoLepton_CR", taus ,  JetColl , AK8_JetColl,  leps,  METv, nPV, w);
  }
  else{
    if(!SameCharge(leps))Fill_RegionPlots(param,"HNL_OS_ZAK8_TwoLepton_CR", taus ,  JetColl , AK8_JetColl,  leps,  METv, nPV, w);
  }
  return true;

}

bool HNL_RegionDefinitions::FillWWCR2Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   ,vector<Tau>& taus, std::vector<Jet>& jets_eta5, std::vector<FatJet>& AK8_JetColl,  std::vector< Jet>& B_JetColl,Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = WWCR;
  FillCutflow(Reg, w, "Step0",param);
  // This CRis same as AN2021_008 Table 10                                                                                                                                                                         
  // Same as SR but High MET

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  if(leps[1]->Pt() < 25) return false;

  FillCutflow(Reg, w, "Step1",param);

  if(leps_veto.size() != 2) return false;
  FillCutflow(Reg, w, "Step2",param);

  if(jets_eta5.size() < 2) return false;
  FillCutflow(Reg, w, "Step3",param);

  int NB_JetColl = B_JetColl.size();
  Particle ll =  *leps[0] + *leps[1];

  // two highest pt jets with                                                                                                                                                                                      
  Jet j1 = jets_eta5[0] ;
  Jet j2 = jets_eta5[1];

  bool passJetPt = ((j1.Pt() > 30.) && (j2.Pt() > 30.));
  if(!passJetPt)  return false;

  if ( ll.M() < M_CUT_NonSR3_LL) return false;

  double met2_st = ev.MET2ST();//MET2ST(leps, jets_eta5, AK8_JetColl, METv);
  if(met2_st < 10) return false;
  FillCutflow(Reg, w, "Step4",param);

  if (NB_JetColl>0) return false;
  FillCutflow(Reg, w, "Step5",param);

  if ((j1+j2).M() < 500.) return false;
  FillCutflow(Reg, w, "Step6",param);

  double DiJetDeta = fabs(j1.Eta() - j2.Eta());
  if (DiJetDeta  <2.5) return false;
  FillCutflow(Reg, w, "Step7",param);

  double Av_JetEta= 0.5*(j1.Eta()+ j2.Eta());
  double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,DiJetDeta);
  if (zeppenfeld > 0.75) return false;
  FillCutflow(Reg, w, "Step8",param);

  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return false;
  FillCutflow(Reg, w, "Step9",param);


  if(run_Debug){
    cout << "HNL_WpWp_TwoLepton_CR " << param.Name << " " << event  << endl;
    for(auto ilep: leps) cout << "HNL_WpWp_TwoLepton_CR Type " <<  ilep->LeptonGenType() << endl;
  }

  if(AK8_JetColl.size() > 0) return false;

  Fill_RegionPlots(param,"HNL_WpWp_TwoLepton_CR2" , taus, jets_eta5,  AK8_JetColl,  leps,  METv, nPV, w);

  return true;

}


bool HNL_RegionDefinitions::FillWWCRNPPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , vector<Tau>& taus ,std::vector<Jet>& jets_eta5, std::vector<FatJet>& AK8_JetColl,std::vector< Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  // This is the same as AN2019_089 Table 11 Nonprompt

  HNL_LeptonCore::SearchRegion Reg = WWNP1CR;
  FillCutflow(Reg, w, "Step0",param);
  
  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  if(leps[0]->Pt() < 25.) return false;
  if(leps[1]->Pt() < 20.) return false;

  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 2) return false;
  Particle ll =  *leps[0] + *leps[1];
  if ( ll.M() < M_CUT_NonSR3_LL) return false;
  FillCutflow(Reg, w, "Step2",param);

  if(jets_eta5.size() < 2) return false;

  FillCutflow(Reg, w, "Step3",param);
  
  
  std::vector<Jet> JetColl                  = SelectAK4Jets(jets_eta5, 20., 2.5, true,  0.4,0.8,"",    leps_veto,AK8_JetColl);
  int NB_JetColl = B_JetColl.size();
  //file:///Users/john/Downloads/AN2019_089_v7.pdf SSWW +WZ  + AN2020_045
  //https://arxiv.org/pdf/2005.01173.pdf 
  // two highest pt jets with   
  
  Jet j1 = jets_eta5[0] ;
  Jet j2 = jets_eta5[1];
  bool passJetPt = ((j1.Pt() > 50.) && (j2.Pt() > 50.));
  if(!passJetPt)  return false;

  FillCutflow(Reg, w, "Step4",param);
  
  if ( METv.Pt() < 30.) return false;
  FillCutflow(Reg, w, "Step5",param);

  if (NB_JetColl!=1) return false;
  FillCutflow(Reg, w, "Step6",param);

  if ((j1+j2).M() < 500.) return false;
  FillCutflow(Reg, w, "Step7",param);
  
  double DiJetDeta = fabs(j1.Eta() - j2.Eta());
  if (DiJetDeta  <2.5) return false;
  FillCutflow(Reg, w, "Step8",param);

  double Av_JetEta= 0.5*(j1.Eta()+ j2.Eta());
  double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,DiJetDeta);
  if (zeppenfeld > 0.75) return false;
  FillCutflow(Reg, w, "Step9",param);

  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return false;
  FillCutflow(Reg, w, "Step10",param);

  if(run_Debug){
    cout << "HNL_WpWpNP_TwoLepton_CR " << param.Name << " " << event  << endl;
    for(auto ilep: leps) cout << "HNL_WpWpNP_TwoLepton_CR Type " <<  ilep->LeptonGenType() << endl;
  }

  if(AK8_JetColl.size() > 0) return false;

  Fill_RegionPlots(param,"HNL_WpWpNP_TwoLepton_CR" ,  taus,jets_eta5,  AK8_JetColl,  leps, METv, nPV, w);
  
  return true;

}

bool HNL_RegionDefinitions::FillWWCRNP2Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   ,vector<Tau>& taus , std::vector<Jet>& jets_eta5, std::vector<FatJet>& AK8_JetColl,std::vector< Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = WWNP2CR;
  FillCutflow(Reg, w, "Step0",param);

  // This is the same as AN2020_045 Table 15 Nonprompt                                                                                                                                                             

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  if(leps[1]->Pt() < 30)  return false;
  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 2) return false;

  FillCutflow(Reg, w, "Step2",param);

  Particle ll =  *leps[0] + *leps[1];
  if ( ll.M() < M_CUT_NonSR3_LL) return false;
  
  FillCutflow(Reg, w, "Step3",param);
  
  if(jets_eta5.size() < 2) return false;
  FillCutflow(Reg, w, "Step4",param);

  std::vector<Jet> JetColl                  = SelectAK4Jets(jets_eta5, 20., 2.5, true,  0.4,0.8,"",    leps_veto,AK8_JetColl);
  int NB_JetColl = B_JetColl.size();

  // two highest pt jets with                                                                                                                                                                                      
  Jet j1 = jets_eta5[0] ;
  Jet j2 = jets_eta5[1];
  bool passJetPt = ((j1.Pt() > 30.) && (j2.Pt() > 30.));
  if(!passJetPt)  return false;

  FillCutflow(Reg, w, "Step5",param);

  if (NB_JetColl!=1) return false;
  FillCutflow(Reg, w, "Step6",param);

  if ((j1+j2).M() < 150.) return false;
  if ((j1+j2).M() > 500.) return false;
  FillCutflow(Reg, w, "Step7",param);

  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return false;
  FillCutflow(Reg, w, "Step8",param);

  if(run_Debug){
    cout << "HNL_WpWpNP2_TwoLepton_CR " << param.Name << " " << event  << endl;
    for(auto ilep: leps) cout << "HNL_WpWpNP2_TwoLepton_CR Type " <<  ilep->LeptonGenType() << endl;
  }

  if(AK8_JetColl.size() > 0) return false;

  Fill_RegionPlots(param,"HNL_WpWpNP2_TwoLepton_CR" ,  taus,jets_eta5,  AK8_JetColl,  leps,  METv, nPV, w);

  return true;

}

bool HNL_RegionDefinitions::FillWWCRNP3Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto,vector<Tau>& taus, std::vector<Jet>& jets_eta5, std::vector<FatJet>& AK8_JetColl,std::vector< Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = WWNP3CR;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  if(leps[1]->Pt() < 30)  return false;

  FillCutflow(Reg, w, "Step1",param);
  if (leps_veto.size() != 2) return false;

  FillCutflow(Reg, w, "Step2",param);

  if(jets_eta5.size() < 2) return false;
  FillCutflow(Reg, w, "Step3",param);

  std::vector<Jet> JetColl                  = SelectAK4Jets(jets_eta5, 20., 2.5, true,  0.4,0.8,"",    leps_veto,AK8_JetColl);
  int NB_JetColl = B_JetColl.size();

  Particle ll =  *leps[0] + *leps[1];

  // two highest pt jets with                                                                                                                                                                                      
  Jet j1 = jets_eta5[0] ;
  Jet j2 = jets_eta5[1];
  bool passJetPt = ((j1.Pt() > 30.) && (j2.Pt() > 30.));
  if(!passJetPt)  return false;

  FillCutflow(Reg, w, "Step4",param);

  if ( ll.M() < M_CUT_NonSR3_LL) return false;
  FillCutflow(Reg, w, "Step5",param);

  if ( METv.Pt() < 30.) return false;
  if (NB_JetColl>0) return false;
  FillCutflow(Reg, w, "Step6",param);

  if ((j1+j2).M() < 500.) return false;
  FillCutflow(Reg, w, "Step7",param);

  double DiJetDeta = fabs(j1.Eta() - j2.Eta());
  if (DiJetDeta  <2.5) return false;
  FillCutflow(Reg, w, "Step8",param);

  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return false;
  FillCutflow(Reg, w, "Step9",param);

  if(run_Debug){
    cout << "HNL_WpWpNP3_TwoLepton_CR " << param.Name << " " << event  << endl;
    for(auto ilep: leps) cout << "HNL_WpWpNP3_TwoLepton_CR Type " <<  ilep->LeptonGenType() << endl;
  }

  if(AK8_JetColl.size() > 0) return false;

  Fill_RegionPlots(param,"HNL_WpWpNP3_TwoLepton_CR" ,  taus,jets_eta5,  AK8_JetColl,  leps,  METv, nPV, w);

  return true;
  

}

bool HNL_RegionDefinitions::FillSSPreselectionPlots(HNL_LeptonCore::Channel channel,std::vector<Lepton *>& leps,std::vector<Lepton *>& leps_veto,vector<Tau>& taus,std::vector<Jet>& JetColl,std::vector<FatJet>& AK8_JetColl,std::vector<Jet>& B_JetColl,Event& ev,  Particle& METv,AnalyzerParameter& param,float w){
  HNL_LeptonCore::SearchRegion Reg = Presel;
  FillCutflow(Reg, w, "Step0",param);
  if(!RunCF && !SameCharge(leps))  return false;
  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;

  FillCutflow(Reg, w, "Step1",param);
  if (leps_veto.size() != 2) return false;

  FillCutflow(Reg, w, "Step2",param);

  Particle ll =  (*leps[0]) + (*leps[1]);
  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return false;
  if (param.SRConfig == "PreselectionV2") {
    if ((fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return false;
  }
  FillCutflow(Reg, w, "Step3",param);

  if(ll.M() < M_CUT_LL) return false;
  FillCutflow(Reg, w, "Step4",param);

  Fill_RegionPlots(param,"HNL_SSPresel_TwoLepton"  ,  taus,JetColl,  AK8_JetColl,  leps, METv, nPV, w);


  return true;

}

bool HNL_RegionDefinitions::RunCR(TString CRName , vector<TString> CRs) {
  
  if (std::find(CRs.begin(), CRs.end(), CRName) != CRs.end())    return true;
  else return false;
  
}




TString HNL_RegionDefinitions::FillHighMassSR1CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = HMCR1;
  FillCutflow(Reg, w, "Step0",param);
  if(!CheckLeptonFlavourForChannel(channel, leps)) return "false";

  if((leps[0]->Pt()+ leps[1]->Pt()) < 50) return "false"; 

  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 2) return "false";
  FillCutflow(Reg, w, "Step2",param);

  if (leps.size() != 2) return "false";
  FillCutflow(Reg, w, "Step3",param);
  

  int NB_JetColl = B_JetColl.size();

  Particle ll =  (*leps[0]) + (*leps[1]);
  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return "false";
  
  if(ll.M() < M_CUT_LL) return "false";
  FillCutflow(Reg, w, "Step4",param);

  double met2_st = ev.MET2ST(); 
  bool PassHMMet    = (met2_st < 15);
  if(channel==MuMu) PassHMMet    = (met2_st < 10);

  if(AK8_JetColl.size()!=1)    return "false";

  FillCutflow(Reg, w, "Step5",param);

  /////////// NOTES 
  ///////
  //////    Particle Wcand = AK8_JetColl[m] + *leps[0]+*leps[1];   CUT is not applied here (could add based on stats )                                                                              
  /////    if(Wcand.M()  < LowerMassSR1WmassCut ) return ""false"";                                                                                                                                      
  //////                                                                                                                                                                                                          
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
  Particle Wcand = AK8_JetColl[m] + *leps[0]+*leps[1];

  //// Additional CR

  Fill_RegionPlots(param,"HNL_HighMassSR1_Inclusive"  ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);

  runCutCounts[run].Cut("HNL_HighMassSR1_Inclusive");

  if(NB_JetColl==1 && false){

    runCutCounts[run].Cut("HNL_HighMassSR1_BJet");

    cout << " -=-------------------------------- " << endl;
    cout << "SR1 BJET CR: Ev Number= " << event << " run = " << run << endl;
    cout << "NJet = " << JetColl.size() << endl;

    bool closelep=false;
    for(auto ilep : leps) {
      if(ilep->DeltaR(AK8_JetColl[0])  < 0.2) {

	cout << "dr (AK8,lep) " << ilep->GetFlavour() <<  " " << ilep->DeltaR(AK8_JetColl[0]) << " -------> close " <<  endl;
	closelep=true;
      }
      else cout << "dr (AK8,lep) " << ilep->GetFlavour() <<  " " << ilep->DeltaR(AK8_JetColl[0]) << endl;

      for(auto ib : B_JetColl) cout << "dr (BJet,lep) " << ilep->GetFlavour() <<  " " << ilep->DeltaR(ib) << endl;

      if(leps[0]->DeltaR(*leps[1]) < 0.1 || leps[0]->DeltaR(*leps[1]) > 4)cout << "dR leps  = " << leps[0]->DeltaR(*leps[1]) << endl;
    }
    if(closelep)   runCutCounts[run].Cut("HNL_HighMassSR1_NJet_AK8_CloseLep");

  }
  if(!PassHMMet)     runCutCounts[run].Cut("HNL_HighMassSR1_MET");


  if(PassHMMet && NB_JetColl==0) return "false"; /// SR
  if(NB_JetColl >1) return "false"; /// Dont consider Multi BJet events

  FillCutflow(Reg, w, "Step6",param);
  
  /// Check if Z peak is needed to be vetoed

  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return "false";

  Fill_RegionPlots(param,"HNL_HighMassSR1_TwoLepton_CR"  ,  taus,JetColl,  AK8_JetColl,  leps,   METv, nPV, w);

  if(NB_JetColl==1) {
    Fill_RegionPlots(param,"HNL_HighMassSR1_InvBJet_TwoLepton_CR"  ,  taus,JetColl,  AK8_JetColl,  leps,   METv, nPV, w);
    return "SR1_InvBJet";
  }
  Fill_RegionPlots(param,"HNL_HighMassSR1_InvMET_TwoLepton_CR"  ,  taus,JetColl,  AK8_JetColl,  leps,   METv, nPV, w);

  return "SR1_InvMET";
}

bool HNL_RegionDefinitions::FillSSZPeakCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<Jet>& VBF_JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){
  
  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  if (leps_veto.size() != 2) return false;
  if (!RunCF && !SameCharge(leps)) return false;

  if(channel != EE) return false;

  Particle ll =  (*leps[0]) + (*leps[1]);

  // Kepp only Z peak events with many CF
  if (fabs(ll.M()-M_Z) > M_ZWINDOW_VETO) return false;

  //// Remove BJet events
  int NB_JetColl = B_JetColl.size();
  if(NB_JetColl  > 0) return false;

  double met2_st = ev.MET2ST();//  double met2_st = GetMET2ST(leps, JetColl, AK8_JetColl, METv);
  bool PassHMMet    = (met2_st < 10);
  if(!PassHMMet) return false;

  //Fill_RegionPlots(param,"HNL_HighMassSSZPeak_TwoLepton_CR"  ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);

  if(AK8_JetColl.size() > 0){
    Fill_RegionPlots(param,"HNL_HighMassSSZPeak_AK8_TwoLepton_CR"  , taus, JetColl,  AK8_JetColl,  leps, METv, nPV, w);
  }
  else if(PassVBF(VBF_JetColl,leps,450)){
    Fill_RegionPlots(param,"HNL_HighMassSSZPeak_VBF_TwoLepton_CR"  , taus, VBF_JetColl,  AK8_JetColl,  leps, METv, nPV, w);
  }
  else{
    Fill_RegionPlots(param,"HNL_HighMassSSZPeak_AK4_TwoLepton_CR"  , taus, JetColl,  AK8_JetColl,  leps,  METv, nPV, w);
  }
  return true;
}


bool HNL_RegionDefinitions::FillHighMass1JetCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   ,vector<Tau>& taus,  std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = HM1JCR;
  FillCutflow(Reg, w, "Step0",param);

  if (channel!=EE ) return false;
  FillCutflow(Reg, w, "Step1",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  if (leps_veto.size() != 2) return false;

  /// Remove signals 
  if(leps[1]->Pt() > 50) return false; 
  
  FillCutflow(Reg, w, "Step2",param);

  Particle ll =  (*leps[0]) + (*leps[1]);
  // Keep only Z peak events with many CF
  if (fabs(ll.M()-M_Z) > M_ZWINDOW_VETO) return false;
  FillCutflow(Reg, w, "Step3",param);

  if(JetColl.size() != 1) return false;
  FillCutflow(Reg, w, "Step4",param);

  if(AK8_JetColl.size() > 0) return false;

  Fill_RegionPlots(param,"HNL_HighMass1Jet_TwoLepton_CR"  , taus,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);

  return true;

}


bool HNL_RegionDefinitions::FillHighMassBJetCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = HMBCR;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  if (leps_veto.size() != 2) return false;
  FillCutflow(Reg, w, "Step1",param);


  Particle ll =  (*leps[0]) + (*leps[1]);
  //  if (channel==EE  && (fabs(ll.M()-M_Z) > M_ZWINDOW_VETO)) return false;
  FillCutflow(Reg, w, "Step2",param);

  if(ll.M() < M_CUT_NonSR3_LL) return false;
  FillCutflow(Reg, w, "Step3",param);

  int NB_JetColl = B_JetColl.size();

  if(NB_JetColl  != 1) return false;
  FillCutflow(Reg, w, "Step4",param);

  if(AK8_JetColl.size() >  0)  {
    return false;
  }

  Fill_RegionPlots(param,"HNL_HighMassBJet_TwoLepton_CR"  ,  taus,JetColl,  AK8_JetColl,  leps, METv, nPV, w);

  return true;

}


bool HNL_RegionDefinitions::FillHighMassNPCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   ,vector<Tau>& taus,  std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = HMNPCR;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step1",param);

  Particle ll =  (*leps[0]) + (*leps[1]);
  if(ll.M() < M_CUT_NonSR3_LL) return false;

  FillCutflow(Reg, w, "Step2",param);

  if (leps_veto.size() != 2) return false;
  FillCutflow(Reg, w, "Step3",param);
  
  if(JetColl.size() > 0) return false;
  FillCutflow(Reg, w, "Step4",param);

  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return false;
  if (channel==EMu  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return false;

  FillCutflow(Reg, w, "Step5",param);

  double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );

  if(ll_dphi < 2.5)    return false;
  FillCutflow(Reg, w, "Step6",param);

  Fill_RegionPlots(param,"HNL_HighMassNP_TwoLepton_CR", taus  ,  JetColl,  AK8_JetColl,  leps, METv, nPV, w);
  
  return true;

}

TString  HNL_RegionDefinitions::FillHighMassSR3CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto   , std::vector<Tau>& taus,  std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = HMCR3;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return "false";
  if (leps_veto.size() != 2) return "false";
  FillCutflow(Reg, w, "Step1",param);

  int NB_JetColl = B_JetColl.size();

  Particle ll =  (*leps[0]) + (*leps[1]);
  
  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return "false";
  FillCutflow(Reg, w, "Step2",param);

  if(ll.M() < 20) return "false";
  FillCutflow(Reg, w, "Step3",param);

  double met2_st = ev.MET2ST(); //  double met2_st = GetMET2ST(leps, JetColl, AK8_JetColl, METv);
  bool PassHMMet = (met2_st < 15);

  if(PassHMMet && NB_JetColl==0) return "false";
  if(NB_JetColl >1) return "false";
  FillCutflow(Reg, w, "Step4",param);

  if(AK8_JetColl.size() > 0) return "false";
  FillCutflow(Reg, w, "Step5",param);
    
  Fill_RegionPlots(param,"HNL_HighMassSR3_TwoLepton_CR", taus  ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);
  
  if (JetColl.size() < 2  && leps[1]->Pt() > 80.) {
    if(leps[1]->Pt() > 140.)return true;
    else return "false";
  }

  if (JetColl.size() < 2) return "false";
  FillCutflow(Reg, w, "Step6",param);


  double LowerMassSR3WmassCut = 50.;
  double UpperMassSR3WmassCut = 150.;

  if(!(GetRecoObjMass("HNL_SR3", JetColl, AK8_JetColl,leps) < UpperMassSR3WmassCut && GetRecoObjMass("HNL_SR3", JetColl, AK8_JetColl,leps) > LowerMassSR3WmassCut)) return "false";
  FillCutflow(Reg, w, "Step7",param);

  Fill_RegionPlots(param,"HNL_HighMassSR3_2J_TwoLepton_CR", taus  ,  JetColl,  AK8_JetColl,  leps, METv, nPV, w);

  
  if(NB_JetColl>0) return "SR3_InvBJet";
  return "SR3_InvMET";  
}



TString  HNL_RegionDefinitions::FillHighMassSR2CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps,  std::vector<Lepton *>& leps_veto , std::vector<Tau>& taus  , std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = HMCR2;
  FillCutflow(Reg, w, "Step0",param);

  
  if(!CheckLeptonFlavourForChannel(channel, leps)) return "false";
  if(leps[1]->Pt() < 15) return "false"; 
  
  if (param.SRConfig == "SR2_Pt20") {
    if(leps[1]->Pt() < 20) return "false";
  }
  if (param.SRConfig == "SR2_Pt25") {
    if(leps[1]->Pt() < 25) return "false";
  }
  if (param.SRConfig == "SR2_Pt30") {
    if(leps[1]->Pt() < 30) return "false";
  }
  
  
  if (leps_veto.size() != 2) return "false";
  if (leps.size() != 2) return "false";
  FillCutflow(Reg, w, "Step1",param);

  int NB_JetColl = B_JetColl.size();

  Particle ll    =  (*leps[0]) + (*leps[1]);
  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return "false";
  FillCutflow(Reg, w, "Step2",param);

  if (ll.M() < M_CUT_NonSR3_LL) return "false";  ///// CHECK  10 GeV or 20 GeV
  FillCutflow(Reg, w, "Step3",param);

  double met2_st = ev.MET2ST();//  double met2_st = GetMET2ST(leps, JetColl, AK8_JetColl, METv);
  bool PassHMMet = (met2_st < 15);   ///// USe SR MET + 2 GeV

  if(AK8_JetColl.size() > 0) return "false";

  if(PassVBFInitial(JetColl)){
    
    if(PassHMMet && NB_JetColl==0) return "false"; /// SR                                                                                                                                       
    if(NB_JetColl >1) return "false"; /// Dont consider Multi BJet events                                                                                                                       
    Fill_RegionPlots(param,"HNL_HighMassSR2_Inclusive_CR", taus  ,  JetColl,  AK8_JetColl,  leps,METv, nPV, w);
    
    if(PassVBF(JetColl, leps,500)){
      
      if(NB_JetColl==1) Fill_RegionPlots(param,"HNL_HighMassSR2_InvBJet_CR", taus  ,  JetColl,  AK8_JetColl,  leps,METv, nPV, w);
      else Fill_RegionPlots(param,"HNL_HighMassSR2_InvMET_CR", taus  ,  JetColl,  AK8_JetColl,  leps,METv, nPV, w);
        
      FillCutflow(Reg, w, "Step6",param);
    
      if(NB_JetColl>0) return "SR2_InvBJet";
      return "SR2_InvMET";
    }
    else return "NULL"; ///  Fails Mass Cut
  }
  return "false";
}


bool HNL_RegionDefinitions::FillWZVBFCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton*>& leps, std::vector<Lepton*>& leps_veto , std::vector<Tau>& taus  , std::vector<Jet>& jets_eta5, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = WZVBFCR;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  if(!HasOSSFPair(leps)) return false;

  if(jets_eta5.size() < 2) return false;
  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 3) return false;
  FillCutflow(Reg, w, "Step2",param);

  double metcut  = (UseMET2ST) ? 5 : 30.;

  double trilep_masscut=105.;

  std::vector<Jet> JetColl                  = SelectAK4Jets(jets_eta5, 20., 2.5, true,  0.4,0.8,"",    leps_veto,AK8_JetColl);
  int NB_JetColl = B_JetColl.size();

  int ijet1(0), ijet2(1);

  if(GetIndexNonBestZ(leps,M_ZWINDOW_CR) < 0) return false;
  FillCutflow(Reg, w, "Step3",param);

  if(GetMassMinOSSF(leps)  < 20.) return false;
  FillCutflow(Reg, w, "Step4",param);

  if(((*leps[0])+ (*leps[1]) + (*leps[2])).M() <  trilep_masscut) return false;
  FillCutflow(Reg, w, "Step5",param);


  if(jets_eta5.size() < 2) return false;
  Jet j1 = jets_eta5[ijet1] ;
  Jet j2 = jets_eta5[ijet2];
  double maxDiJetDeta = fabs(j1.Eta() - j2.Eta());

  bool passJetPt = ((j1.Pt() > 30.) && (j2.Pt() > 30.));
  if(!passJetPt)  return false;

  if (NB_JetColl>0) return false;
  if ((j1+j2).M() < 400.) return false;
  if (maxDiJetDeta<2.5) return false;

  double Av_JetEta= 0.5*(jets_eta5[ijet1].Eta()+ jets_eta5[ijet2].Eta());
  double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);

  if (zeppenfeld > 1.00) return false;
  FillCutflow(Reg, w, "Step6",param);
  if(AK8_JetColl.size() > 0) return false;
  if(UseMET2ST  && ev.MET2ST() < metcut)  return false;
  if(!UseMET2ST && METv.Pt()   < metcut) return false;

  double nbin_reg;
  double binvalue = GetLimitBin("CR_SR2_WZ",leps,JetColl,AK8_JetColl,ev,nbin_reg);

  FillHist(  "LimitExtraction/"+ param.Name+"/LimitShape_WZ_SR2/Binned",  binvalue,  w, int(nbin_reg),0,nbin_reg ,"CR Binned");  
  Fill_RegionPlots(param,"HNL_WZVBF_ThreeLepton_CR", taus ,  jets_eta5,  AK8_JetColl,  leps,  METv, nPV, w);

  OutCutFlow("HNL_WZVBF_ThreeLepton_CR",w);

  return true;

}


bool HNL_RegionDefinitions::FillWZVBF2CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton*>& leps, std::vector<Lepton*>& leps_veto , std::vector<Tau>& taus  , std::vector<Jet>& jets_eta5, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = WZVBFCR2;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  if(!HasOSSFPair(leps)) return false;

  if(jets_eta5.size() < 2) return false;
  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 3) return false;
  FillCutflow(Reg, w, "Step2",param);


  double metcut  = (UseMET2ST) ? 5 : 30.;
  double trilep_masscut=105.;


  std::vector<Jet> JetColl                  = SelectAK4Jets(jets_eta5, 20., 2.5, true,  0.4,0.8,"",    leps_veto,AK8_JetColl);
  int NB_JetColl = B_JetColl.size();


  int ijet1(0), ijet2(1);

  if(GetIndexNonBestZ(leps,M_ZWINDOW_CR) < 0) return false;
  FillCutflow(Reg, w, "Step3",param);

  if(GetMassMinOSSF(leps)  < 20.) return false;
  FillCutflow(Reg, w, "Step4",param);

  if(((*leps[0])+ (*leps[1]) + (*leps[2])).M() <  trilep_masscut) return false;
  FillCutflow(Reg, w, "Step5",param);


  if(jets_eta5.size() < 2) return false;
  Jet j1 = jets_eta5[ijet1] ;
  Jet j2 = jets_eta5[ijet2];
  double maxDiJetDeta =fabs(j1.Eta() - j2.Eta());

  bool passJetPt = ((j1.Pt() > 30.) && (j2.Pt() > 30.));
  if(!passJetPt)  return false;

  if (NB_JetColl>0) return false;
  if ((j1+j2).M() < 500.) return false;
  if (maxDiJetDeta<2.5) return false;

  double Av_JetEta= 0.5*(jets_eta5[ijet1].Eta()+ jets_eta5[ijet2].Eta());

  double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);

  if (zeppenfeld > 1.00) return false;
  FillCutflow(Reg, w, "Step6",param);

  if(UseMET2ST  && ev.MET2ST() < metcut)  return false;
  if(!UseMET2ST && METv.Pt()   < metcut) return false;

  if(AK8_JetColl.size() > 0) return false;


  Fill_RegionPlots(param,"HNL_WZVBF2_ThreeLepton_CR", taus ,  jets_eta5,  AK8_JetColl,  leps,  METv, nPV, w);

  return true;

}


bool HNL_RegionDefinitions::FillWZBCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton*>& leps, std::vector<Lepton*>& leps_veto , std::vector<Tau>& taus  , std::vector<Jet>& jets_eta5, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = WZBCR;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 3) return false;
  FillCutflow(Reg, w, "Step2",param);

  if(!HasOSSFPair(leps)) return false;
  FillCutflow(Reg, w, "Step3",param);

  if(jets_eta5.size() < 2) return false;
  FillCutflow(Reg, w, "Step4",param);

  double metcut  = (UseMET2ST) ? 5 : 30.;
  double trilep_masscut=105.;

  std::vector<Jet> JetColl   = SelectAK4Jets(jets_eta5, 20., 2.5, true,  0.4,0.8,"",    leps_veto,AK8_JetColl);
  int NB_JetColl = B_JetColl.size();
  //file:///Users/john/Downloads/AN2019_089_v7.pdf SSWW +WZ  + AN2020_045                                                                                                                                          

  if(jets_eta5.size() < 2) return false;
  FillCutflow(Reg, w, "Step5",param);


  int ijet1(0), ijet2(1);

  if(GetIndexNonBestZ(leps,M_ZWINDOW_CR) < 0) return false;
  FillCutflow(Reg, w, "Step6",param);

  if(((*leps[0])+ (*leps[1]) + (*leps[2])).M() <  trilep_masscut) return false;
  FillCutflow(Reg, w, "Step7",param);

  Jet j1 = jets_eta5[ijet1] ;
  Jet j2 = jets_eta5[ijet2];
  double maxDiJetDeta =fabs(j1.Eta() - j2.Eta());

  bool passJetPt = ((j1.Pt() > 30.) && (j2.Pt() > 30.));
  if(!passJetPt)  return false;

  FillCutflow(Reg, w, "Step8",param);


  FillCutflow(Reg, w, "Step9",param);

  if (NB_JetColl!=1) return false;
  FillCutflow(Reg, w, "Step10",param);

  if ((j1+j2).M() < 750.) return false;
  FillCutflow(Reg, w, "Step11",param);

  if (maxDiJetDeta<2.5) return false;
  FillCutflow(Reg, w, "Step12",param);

  if(!ZmassOSSFWindowCheck(leps,M_ZWINDOW_CR)) return false;
  FillCutflow(Reg, w, "Step13",param);


  double Av_JetEta= 0.5*(jets_eta5[ijet1].Eta()+ jets_eta5[ijet2].Eta());
  double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);
  if (zeppenfeld > 1.00) return false;
  FillCutflow(Reg, w, "Step14",param);

  if(AK8_JetColl.size() > 0) return false;

  if(UseMET2ST  && ev.MET2ST() < metcut)  return false;
  if(!UseMET2ST && METv.Pt()   < metcut) return false;


  Fill_RegionPlots(param,"HNL_WZB_ThreeLepton_CR", taus ,  jets_eta5,  AK8_JetColl,  leps,  METv, nPV, w);

  double nbin_reg;
  double binvalue = GetLimitBin("CR_SR2_WZB",leps,JetColl,AK8_JetColl,ev,nbin_reg);
  FillHist(  "LimitExtraction/"+ param.Name+"/LimitShape_WZB_SR2/Binned",  binvalue,  w, int(nbin_reg),0,nbin_reg ,"CR Binned");

  OutCutFlow("HNL_WZB_ThreeLepton_CR",w);

  return true;

}


bool HNL_RegionDefinitions::FillZZCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto , std::vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = ZZCR;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 4) return false;
  FillCutflow(Reg, w, "Step2",param);

  if(!ConversionSplitting(leps,RunConv,4,param)) return false;


  int sumQ = leps[0]->Charge() + leps[1]->Charge() +leps[2]->Charge() +leps[3]->Charge() ;
  if(sumQ != 0) return false;
  FillCutflow(Reg, w, "Step3",param);

  int NB_JetColl=B_JetColl.size();

  if(NB_JetColl  > 0 ) return false;
  FillCutflow(Reg, w, "Step4",param);

  bool z_cr_pass(false);

  Particle Z1Cand;
  Particle Z2Cand;

  for(unsigned int iel =0; iel < leps.size() ; iel++){
    for(unsigned int iel2 =iel+1; iel2 < leps.size() ; iel2++){
      if(iel== iel2) continue;
      if(IsOSSF(leps[iel], leps[iel2])){
  Z1Cand = (*leps[iel]) + (*leps[iel2]) ;
        int zel1(-9), zel2(-9);
        if(iel ==0 && iel2==1){ zel1=2; zel2=3;    Z2Cand = (*leps[2]) + (*leps[3]);}
        if(iel ==0 && iel2==2){ zel1=1; zel2=3;    Z2Cand = (*leps[1]) + (*leps[3]);}
        if(iel ==0 && iel2==3){ zel1=1; zel2=2;    Z2Cand = (*leps[1]) + (*leps[2]);}
        if(iel ==1 && iel2==2){ zel1=0; zel2=3;    Z2Cand = (*leps[0]) + (*leps[3]);}
        if(iel ==1 && iel2==3){ zel1=0; zel2=2;    Z2Cand = (*leps[0]) + (*leps[2]);}
        if(iel ==2 && iel2==3){ zel1=0; zel2=1;    Z2Cand = (*leps[0]) + (*leps[1]);}
  
  if(IsOSSF(leps[zel1], leps[zel2])){
          if(fabs(Z1Cand.M() - M_Z) < M_ZWINDOW_CR){
      if(fabs(Z2Cand.M() - M_Z) < M_ZWINDOW_CR){
        z_cr_pass=true;
      }
          }
        }
      }
    }
  }
  if(!z_cr_pass)  return false;
  FillCutflow(Reg, w, "Step5",param);

  
  if(AK8_JetColl.size() > 0) {
    Fill_RegionPlots(param,"HNL_ZZ_AK8_FourLepton_CR", taus ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);
    return false;
  }

  FillHist(  "LimitExtraction/"+ param.Name+"/LimitShape_ZZ/Binned",  0,  w, 1,0,1 ,"CR Binned");

  Fill_RegionPlots(param,"HNL_ZZ_FourLepton_CR", taus ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);
  double weight_orig_kfactor = 1.;
  if(MCSample.Contains("ZZTo4L_powheg")) weight_orig_kfactor =  1.16/ZZKfactor("ZZMass");

  if(User("jalmond"))Fill_RegionPlots(param,"HNL_ZZ_OrigKF_FourLepton_CR", taus ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w*weight_orig_kfactor);

  
  OutCutFlow("HNL_ZZ_FourLepton_CR",w);
  
  return true;
}


bool HNL_RegionDefinitions::FillZZ2CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto , std::vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = ZZCR2;
  FillCutflow(Reg, w, "Step0",param);

  if (leps_veto.size() != 4) return false;
  FillCutflow(Reg, w, "Step1",param);

  if(!ConversionSplitting(leps,RunConv,4,param)) return false;

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step2",param);

  int sumQ = leps[0]->Charge() + leps[1]->Charge() +leps[2]->Charge() +leps[3]->Charge() ;
  if(sumQ != 0) return false;
  FillCutflow(Reg, w, "Step3",param);

  if(B_JetColl.size()  > 0 ) return false;
  FillCutflow(Reg, w, "Step4",param);

  //// Select Best Z candidate indices
  pair<int,int> BestZCand = LeptonPairBestZCand(leps);
  if(BestZCand.first == -1) return false;
  FillCutflow(Reg, w, "Step5",param);

  Particle Z1Cand =  (*leps[BestZCand.first]) + (*leps[BestZCand.second]);
  vector<int> Z2ind;
  for(unsigned int iel =0; iel < leps.size() ; iel++){
    if(int(iel) == BestZCand.first) continue;
    if(int(iel) == BestZCand.second) continue;
    Z2ind.push_back(iel);
  }
  if(Z2ind.size() != 2) return false;
  FillCutflow(Reg, w, "Step6",param);

  if(!IsOSSF(leps[Z2ind[0]],leps[Z2ind[1]])) return false;
  FillCutflow(Reg, w, "Step7",param);

  Particle Z2Cand = (*leps[Z2ind[0]]) + (*leps[Z2ind[1]]);
  if(Z2Cand.M() < M_CUT_LL) return false;
  FillCutflow(Reg, w, "Step8",param);

  Fill_RegionPlots(param,"HNL_ZZLoose_FourLepton_CR", taus ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);


  OutCutFlow("HNL_ZZLoose_FourLepton_CR",w);

  return true;
}


bool HNL_RegionDefinitions::FillZZVBFCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto , std::vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = ZZVBFCR;
  FillCutflow(Reg, w, "Step0",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(Reg, w, "Step1",param);

  if (leps_veto.size() != 4) return false;
  FillCutflow(Reg, w, "Step2",param);

  if(!ConversionSplitting(leps,RunConv,4,param)) return false;

  int sumQ = leps[0]->Charge() + leps[1]->Charge() +leps[2]->Charge() +leps[3]->Charge() ;
  if(sumQ != 0) return false;
  FillCutflow(Reg, w, "Step3",param);

  bool z_cr_pass(false);

  Particle Z1Cand;
  Particle Z2Cand;

  for(unsigned int iel =0; iel < leps.size() ; iel++){
    for(unsigned int iel2 =iel+1; iel2 < leps.size() ; iel2++){
      if(iel== iel2) continue;
      if(IsOSSF(leps[iel], leps[iel2])){
        Z1Cand = (*leps[iel]) + (*leps[iel2]) ;
        int zel1(-9), zel2(-9);
        if(iel ==0 && iel2==1){ zel1=2; zel2=3;    Z2Cand = (*leps[2]) + (*leps[3]);}
        if(iel ==0 && iel2==2){ zel1=1; zel2=3;    Z2Cand = (*leps[1]) + (*leps[3]);}
        if(iel ==0 && iel2==3){ zel1=1; zel2=2;    Z2Cand = (*leps[1]) + (*leps[2]);}
        if(iel ==1 && iel2==2){ zel1=0; zel2=3;    Z2Cand = (*leps[0]) + (*leps[3]);}
        if(iel ==1 && iel2==3){ zel1=0; zel2=2;    Z2Cand = (*leps[0]) + (*leps[2]);}
        if(iel ==2 && iel2==3){ zel1=0; zel2=1;    Z2Cand = (*leps[0]) + (*leps[1]);}

        if(IsOSSF(leps[zel1], leps[zel2])){
          if(fabs(Z1Cand.M() - M_Z) < M_ZWINDOW_CR){
            if(fabs(Z2Cand.M() - M_Z) < M_ZWINDOW_CR){
              z_cr_pass=true;
            }
          }
        }
      }
    }
  }
  if(!z_cr_pass)  return false;
  FillCutflow(Reg, w, "Step4",param);
  
  if(JetColl.size() < 2) return false;
  FillCutflow(Reg, w, "Step5",param);

  int ijet1(0), ijet2(1);

  Jet j1 = JetColl[ijet1] ;
  Jet j2 = JetColl[ijet2];
  double maxDiJetDeta =fabs(j1.Eta() - j2.Eta());

  bool passJetPt = ((j1.Pt() > 30.) && (j2.Pt() > 30.));
  if(!passJetPt)  return false;

  FillCutflow(Reg, w, "Step6",param);

  if ((j1+j2).M() < 500.) return false;
  FillCutflow(Reg, w, "Step7",param);

  if (maxDiJetDeta < 2.5) return false;
  FillCutflow(Reg, w, "Step8",param);

  double Av_JetEta= 0.5*(JetColl[ijet1].Eta()+ JetColl[ijet2].Eta());
  double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);

  if (zeppenfeld > 0.75) return false;
  FillCutflow(Reg, w, "Step9",param);

  Fill_RegionPlots(param,"HNL_ZZVBF_FourLepton_CR", taus ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);

  return true;
}



bool HNL_RegionDefinitions::FillZGCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto , std::vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = ZGCR;

  //if(MCSample.Contains("DY")) return false;
  FillCutflow(ZGCR, w, "Step0",param);
  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(ZGCR, w, "Step1",param);

  if (leps_veto.size() != 3) return false;
  FillCutflow(Reg, w, "Step2",param);
  
  if(!HasOSSFPair(leps)) return false;
  FillCutflow(ZGCR, w, "Step3",param);

  if(leps.at(2)->Pt() < 10) return false;
  FillCutflow(ZGCR, w, "Step4",param);

  double metcut  = (UseMET2ST) ? 5 : 50.;
  int NB_JetColl      = B_JetColl.size();

  if(!ZmasslllWindowCheck(leps,M_ZWINDOW_CR)) return false;
  FillCutflow(ZGCR, w, "Step5",param);

  if(HasLowMassOSSF(leps,M_CUT_LL))       return false;
  FillCutflow(ZGCR, w, "Step6",param);

  if(ZmassOSSFWindowCheck(leps,M_ZWINDOW_CR)) return false;
  FillCutflow(ZGCR, w, "Step7",param);

  if(NB_JetColl > 0)     return false;
  FillCutflow(ZGCR, w, "Step8",param);

  if(UseMET2ST  && ev.MET2ST() > metcut)  return false;
  if(!UseMET2ST && METv.Pt()   > metcut) return false;

  FillCutflow(ZGCR, w, "Step9",param);

  if(!ConversionSplitting(leps,RunConv,3,param)) return false;
  
  FillCutflow(ZGCR, w, "Step10",param);
  
  OutCutFlow("HNL_ZG_ThreeLepton_CR",w);

  FillHist(  "LimitExtraction/"+ param.Name+"/LimitShape_ZG/Binned",  0,  w, 1,0,1 ,"CR Binned");
  
  Fill_RegionPlots(param,"HNL_ZG_ThreeLepton_CR" ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);
  
  return true;
  
}



bool HNL_RegionDefinitions::FillWGCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto , std::vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){
  
  bool DEBUGWG=false;
  if(DEBUGWG)cout << "FillWGCRPlots " << param.Name<< endl;
  if(DEBUGWG)PrintGen(All_Gens);
  if(DEBUGWG)for(auto ilep : leps) cout << ilep->GetFlavour() << " pt = " << ilep->Pt() << endl;
  FillCutflow(WGCR, w, "Step0",param);
  
  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;
  FillCutflow(WGCR, w, "Step1",param);

  if (leps_veto.size() != 3) return false;
  if(leps.size() != 3) return false;
  
  FillCutflow(WGCR, w, "Step2",param);
  if(!HasOSSFPair(leps))    return false;
  FillCutflow(WGCR, w, "Step3",param);

  if(DEBUGWG)cout << "FillWGCRPlots 2" << endl;
  if(HasLowMassMeson(leps)) return false;
  FillCutflow(WGCR, w, "Step4",param);

  if(leps[2]->Pt() < 10) return false;

  FillCutflow(WGCR, w, "Step5",param);
  if(DEBUGWG)cout << "FillWGCRPlots 3" << endl;

  double metcut = 30.;
  double mt_cut = 30.;

  int NB_JetColl=B_JetColl.size();

  ///  if(!HasLowMassOSSF(leps, 4))  return false;

  double MassMinOSSF = GetMassMinOSSF(leps);
  if(MassMinOSSF > 4)  return false;
  FillCutflow(WGCR, w, "Step6",param);
  if(DEBUGWG)cout << "FillWGCRPlots 4" << endl;

  Particle lll = (*leps[0]) + (*leps[1])+ (*leps[2]);
  double MT_lll = M_T(METv,lll);

  if(MT_lll <= mt_cut)  return false;
  FillCutflow(WGCR, w, "Step7",param);
  if(DEBUGWG)cout << "FillWGCRPlots 5" << endl;

  if(NB_JetColl > 0)    return false;
  FillCutflow(WGCR, w, "Step8",param);
 
  if(METv.Pt() < metcut)return false;
  if(DEBUGWG)cout << "FillWGCRPlots 6" << endl;

  FillCutflow(WGCR, w, "Step8",param);

  if(!ConversionSplitting(leps,RunConv,3,param)) {
    if(DEBUGWG)cout << "Fail Conversion split " << endl;
    return false;
  }
  FillCutflow(WGCR, w, "Step9",param);
  if(DEBUGWG)cout << "FillWGCRPlots 7" << endl;

  Fill_RegionPlots(param,"HNL_WG_ThreeLepton_CR", taus  ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);
  OutCutFlow("HNL_WG_ThreeLepton_CR",w);

  return true;
}


bool HNL_RegionDefinitions::FillWZCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto , std::vector<Tau>& taus, std::vector<Jet>& JetColl, std::vector<FatJet>& AK8_JetColl, std::vector<Jet>& B_JetColl,  Event& ev, Particle& METv, AnalyzerParameter& param, float w){

  HNL_LeptonCore::SearchRegion Reg = WZCR;
  

  FillCutflow(Reg, w, "Step0",param);

  if(leps_veto.size() != 3) return false;

  FillCutflow(Reg, w, "Step1",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;

  FillCutflow(Reg, w, "Step2",param);

  if(!HasOSSFPair(leps)) return false;

  FillCutflow(Reg, w, "Step3",param);


  double metcut  = (UseMET2ST) ? 5 : 50.;
  double mtcut   = 20.;
  double trilep_masscut=105.;

  int NB_JetColl=B_JetColl.size();
    
  int sum_ch(0);
  for(auto ilep : leps) sum_ch += ilep->Charge();


  if(fabs(sum_ch) != 1) return false;
  FillCutflow(Reg, w, "Step4",param);
  

  if(NB_JetColl > 0) return false;
  FillCutflow(Reg, w, "Step5",param);
  
  if(GetIndexNonBestZ(leps,M_ZWINDOW_CR) < 0) return false;
  FillCutflow(Reg, w, "Step6",param);
  
  if(M_T((*leps[GetIndexNonBestZ(leps,M_ZWINDOW_CR)]), METv)  < mtcut) return false;
  FillCutflow(Reg, w, "Step7",param);

  if(HasLowMassOSSF(leps,M_CUT_LL)) return false;
  FillCutflow(Reg, w, "Step8",param);


  if(((*leps[0])+ (*leps[1]) + (*leps[2])).M() <  trilep_masscut) return false;
  FillCutflow(Reg, w, "Step9",param);
  
  if(UseMET2ST  && ev.MET2ST() < metcut)  return false;
  if(!UseMET2ST && METv.Pt()   < metcut) return false;

  FillCutflow(Reg, w, "Step10",param);

  OutCutFlow("HNL_WZ_ThreeLepton_CR",w);

  if(AK8_JetColl.size() == 0) {

    double nbin_reg;
    double binvalue = GetLimitBin("CR_SR3_WZ",leps,JetColl,AK8_JetColl,ev,nbin_reg);
    FillHist(  "LimitExtraction/"+ param.Name+"/LimitShape_WZ_SR3/Binned",  binvalue,  w, int(nbin_reg),0,nbin_reg ,"CR Binned");
   
  }
  else if(AK8_JetColl.size() == 1){

    double nbin_reg;
    double binvalue = GetLimitBin("CR_SR1_WZ",leps,JetColl,AK8_JetColl,ev,nbin_reg);
    FillHist(  "LimitExtraction/"+ param.Name+"/LimitShape_WZ_SR1/Binned",  binvalue,  w, int(nbin_reg),0,nbin_reg ,"CR Binned");
  }
  else return false;
  
  //// SR1  
  if(AK8_JetColl.size() == 1 )   Fill_RegionPlots(param,"HNL_WZ_SR1_ThreeLepton_CR", taus ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);  
  //// SR3
  if(AK8_JetColl.size() == 0)  Fill_RegionPlots(param,"HNL_WZ_SR3_ThreeLepton_CR", taus ,  JetColl,  AK8_JetColl,  leps,  METv, nPV, w);
  //// High Pt check

  return true;
}

