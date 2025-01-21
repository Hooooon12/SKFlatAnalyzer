#include "HNL_RegionDefinitions.h"

/*

  Analysis REGION Functions 
  -----------------------------------                                                                                                                   a) RunPreslection
  b) PassVBFInitial : Use this IF using shape for SR2 
  c) PassVBF

  Presel
  ----------------------------
  - PassPreselection

  SR Functions
  -----------------------------------
  * RunAllSignalRegions Runs All SR files
  1- RunSignalRegionAK8 (SR1) OR RunSignalRegionAK8String To run RunSignalRegionAK8 and return Limit input string
  2- RunSignalRegionWW  (SR2) OR RunSignalRegionWWString To run RunSignalRegionWW and return Limit input string  
  3- RunSignalRegionAK4 (SR3) OR RunSignalRegionAK4String To run RunSignalRegionAK4  and return Limit input string  
  4- RunSignalRegionAK4StringBDT Run SR3 BDT
  

 */



void HNL_RegionDefinitions::RunAllSignalRegions(HNL_LeptonCore::ChargeType qq, 
            std::vector<Electron> electronsInitial, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Tau> TauColl, 
            std::vector<Jet> AK4_JetCollLoose,std::vector<Jet> JetColl, std::vector<Jet> VBF_JetColl,std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, 
            Event ev,   Particle METv, AnalyzerParameter param, int nElForRunCF,   float weight_ll){


  std::vector<Electron> electrons;
  if(RunCF) {
    /// Add code to smear individual electron for CF Bkg                                                                                                                                                                                                                                   
    for(unsigned int i=0; i<electronsInitial.size(); i++){
      Electron this_electron = electronsInitial.at(i);
      double ElEnergyShift = 1;

      if(int(i)==nElForRunCF) ElEnergyShift = GetShiftCFEl(this_electron, param.Electron_Tight_ID);

      this_electron.SetPtEtaPhiM( electronsInitial.at(i).Pt()*ElEnergyShift, electronsInitial.at(i).Eta(), electronsInitial.at(i).Phi(), electronsInitial.at(i).M() );
      electrons.push_back( this_electron);
    }
    //// Update MET For CF
    METv = UpdateMETCF(METv,electronsInitial,electrons);
  }
  else  electrons = electronsInitial;


  vector<HNL_LeptonCore::Channel> channels = {GetChannelENum(param.Channel)};

  if(GetChannelENum(param.Channel) == HNL_LeptonCore::NONE){
    cout << "CHANNEL NOT SET" << endl;
    exit(EXIT_FAILURE);
  }

  HNL_LeptonCore::SearchRegion CutFlow_Region = SignalRegion;

  for(auto dilep_channel : channels){
    
    double  weight_channel = weight_ll;

    //// Select CHannel used for Signals to check if signal is EE/MM/Emu using gen info
    if(MCSample.Contains("Type")&& !SelectChannel(dilep_channel)) continue;
  
    if(param.IsCentral()) FillHist( "AllChannel/RunAllSignalRegions_NOCUT_ALLCHANNEL_"+param.Name,  1,  weight_ll, 2,0,2);

    TString channel_string = GetChannelString(dilep_channel);
    param.CutFlowDir = "CutFlow";
    /// Used for Lepton Inclusive plots
    param.NameInclusive_Channel = param.Name  + "/"+param.InclusiveChannelName();
    param.Name = param.Name  + "/"+channel_string;


    //// Set HT over Pt for use in SR/CR VBF limit inputs
    for(long unsigned int imu =0 ; imu <  muons.size(); imu++) muons[imu].SetHTOverPt(GetHT(AK4_JetCollLoose,AK8_JetColl));
    for(long unsigned int iel =0 ; iel < electrons.size() ; iel++) electrons[iel].SetHTOverPt(GetHT(AK4_JetCollLoose,AK8_JetColl));

    std::vector<Lepton *> LepsT       = MakeLeptonPointerVector(muons,     electrons,     param);
    std::vector<Lepton *> LepsV       = MakeLeptonPointerVector(muons_veto,electrons_veto,param);

    if(param.IsCentral()){
      if(MCSample.Contains("Type"))Fill_RegionPlots(param,"Signal_NoCut" , TauColl,
						    JetColl, AK8_JetColl, LepsT,
						    METv, nPV, weight_channel);
    }

   
    //// Set METST value after shifting Electrons                                                                                                                                                                                             
    ev.SetMET2ST(GetMET2ST(LepsT, JetColl, AK8_JetColl, METv));

    FillCutflow(CutFlow_Region, weight_ll, "NoCut", param);

    if(!PassGenMatchFilter(LepsT,param)) continue;
    FillCutflow(CutFlow_Region, weight_ll, "GENMatched",param);
    
    if(param.syst_ ==AnalyzerParameter::Central){
      for(auto iel : electrons){
	FillHist( param.Name+"/Inclusive_El_HEM_EtaPhi", iel.Eta(), iel.Phi(),  fabs(weight_channel),  100, -2.5, 2.5, 70, -3.5,3.5);
	if(!IsData || IsHEMIssueRun()) {
	  FillHist( param.Name+"/HEM_Region_El_HEM_EtaPhi", iel.Eta(), iel.Phi(),  fabs(weight_channel),  100, -2.5, 2.5, 70, -3.5,3.5);
	  
	  if(FindHEMElectron(iel)) FillHist( param.Name+"/HEM_Region_Fail_El_HEM_EtaPhi", iel.Eta(), iel.Phi(),  fabs(weight_channel),  100, -2.5, 2.5, 70, -3.5,3.5);
	  
	}
      }
    }
    if(!PassHEMVeto(LepsV,weight_channel)) continue;
    FillCutflow(CutFlow_Region, weight_ll, "HEMVeto", param);

    if(!PassMETFilter()) return;

    FillCutflow(CutFlow_Region, weight_ll, "METFilter",param);
  
    FillCutflow(HNL_LeptonCore::ChannelDepInc, weight_channel, GetChannelString(dilep_channel) +"_NoCut",param);
    
    
    if(!ConversionSplitting(LepsT,RunConv,2,param)) continue;

    //    FillCutflow(CutFlow_Region, weight_ll, "ConvFilter",param);


    if(! CheckLeptonFlavourForChannel(dilep_channel, LepsT))  continue;
    FillCutflow(CutFlow_Region, weight_channel, "LeptonFlavour",param);
    

    if(param.IsCentral()) PassJetHEMVeto(JetColl,param.Name+"_Jet",weight_channel);

    if(LepsT.size() ==2)  FillCutflow(HNL_LeptonCore::ChannelDepDilep, weight_channel, GetChannelString(dilep_channel) +"_Dilep",param);
    
    if(RunCF){
      if(dilep_channel == MuMu) continue;
      
      if(IsData && SameCharge(LepsT)) continue;
      if(!IsData && !SameCharge(LepsT)) continue;
      
      if(IsData)weight_channel = GetCFWeightElectron(LepsT, param,nElForRunCF,true);
      if(IsData)FillWeightHist(param.Name+"/CFWeight", param, weight_channel);

    }
    else if(HasFlag("SSConvSplit")){
      /// used to test conv with SS cut
      if(!SameCharge(LepsT)) continue;
    }
    else if(MCSample.Contains("WGTo")){
      /// USE SS+OS / 2 using dilepton skim
      weight_channel*= 0.5;
      if(LepsT.size() != 2) continue;
    }
    else{
      if(!SameCharge(LepsT)) continue;
    }
 
    if(RunFake&& IsData){
      //if(_jentry < 100) cout << "Event " << event << " param = " << param.Name << " Running Fakes... FR=" <<  GetFakeWeight(LepsT, param) << endl;
      
      weight_channel = GetFakeWeight(LepsT, param);
      FillFakeWeightHist(param.Name+"/FakeWeight", LepsT,param, weight_channel);

      //// Fix for current High Pt Fakes, which are wrong
      if(RunHighPtID) weight_channel = weight_channel*0.5;
    }

    if (!PassTriggerSelection(dilep_channel, ev, LepsT,param.TriggerSelection)) continue;
    
    EvalTrigWeight(dilep_channel, muons,electrons,param, ev,weight_channel);

    FillCutflow(CutFlow_Region, weight_channel, "Trigger",param);
    if(LepsV.size()==2)     FillCutflow(CutFlow_Region, weight_channel, "LepVeto",param);
  
    if(LepsV.size()==2&& B_JetColl.size()==0)     FillCutflow(CutFlow_Region, weight_channel, "BJet",param);
    if(LepsV.size()==2&& B_JetColl.size()==0 && ev.MET2ST() < 15)   FillCutflow(CutFlow_Region, weight_channel, "MET",param);

    /// RunMainRegionCode runs SR1/SR2/SR3
    
    if(isnan(weight_channel)) {
      cout << "Weight for event is set to Nan..." << endl;
      exit(EXIT_FAILURE);
    }
    
    if(HasFlag("RunCR")) RunMainRegionCode(false,dilep_channel,Inclusive, LepsT, LepsV, TauColl, JetColl,  VBF_JetColl, AK8_JetColl, B_JetColl,ev, METv ,param, weight_channel);
    else     RunMainRegionCode(true,dilep_channel,Inclusive, LepsT, LepsV, TauColl, JetColl, VBF_JetColl, AK8_JetColl, B_JetColl,ev, METv ,param, weight_channel);
  }
}

void   HNL_RegionDefinitions::RunMainRegionCode(bool IsSR,HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType qq, std::vector<Lepton *> LepsT,std::vector<Lepton *> LepsV, vector<Tau> TauColl,  std::vector<Jet> JetColl,       std::vector<Jet> VBF_JetColl,std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param,  float weight_reg){
  

  TString channel_string = GetChannelString(channel);

  HNL_LeptonCore::SearchRegion LimitRegionR1,LimitRegionR2,LimitRegionR3,LimitRegionsBDTR3;
  if(IsSR){
  
    /// Remove now LimitRegions and LimitRegionsBDT which merged SR1/2/3 into one plot as SRs can have different bin numbers per mass/era 
    LimitRegionR1  = HNL_LeptonCore::MuonSR1;
    LimitRegionR2  = HNL_LeptonCore::MuonSR2;    
    LimitRegionR3  = HNL_LeptonCore::MuonSR3;    
    LimitRegionsBDTR3 = HNL_LeptonCore::MuonSR3BDT;
    
    if (channel == EE) {
      LimitRegionR1  = HNL_LeptonCore::ElectronSR1;
      LimitRegionR2  = HNL_LeptonCore::ElectronSR2;
      LimitRegionR3  = HNL_LeptonCore::ElectronSR3;
      LimitRegionsBDTR3 = HNL_LeptonCore::ElectronSR3BDT;
    }
    if (channel == EMu) {
      LimitRegionR1  = HNL_LeptonCore::ElectronMuonSR1;
      LimitRegionR2  = HNL_LeptonCore::ElectronMuonSR2;
      LimitRegionR3  = HNL_LeptonCore::ElectronMuonSR3;
      LimitRegionsBDTR3 = HNL_LeptonCore::ElectronMuonSR3BDT;
    }
  }
  else{    
    

    LimitRegionR1  = HNL_LeptonCore::MuonCR1;
    LimitRegionR2  = HNL_LeptonCore::MuonCR2;
    LimitRegionR3  = HNL_LeptonCore::MuonCR3;
    LimitRegionsBDTR3 = HNL_LeptonCore::MuonCR3BDT;

    if (channel == EE) {

      LimitRegionR1  = HNL_LeptonCore::ElectronCR1;
      LimitRegionR2  = HNL_LeptonCore::ElectronCR2;
      LimitRegionR3  = HNL_LeptonCore::ElectronCR3;
      LimitRegionsBDTR3 = HNL_LeptonCore::ElectronCR3BDT;
    }
    if (channel == EMu) {

      LimitRegionR1  = HNL_LeptonCore::ElectronMuonCR1;
      LimitRegionR2  = HNL_LeptonCore::ElectronMuonCR2;
      LimitRegionR3  = HNL_LeptonCore::ElectronMuonCR3;
      LimitRegionsBDTR3 = HNL_LeptonCore::ElectronMuonCR3BDT;
    }
  }
  
  if(!PassPreselection(IsSR,channel,Inclusive, 
               LepsT, LepsV, TauColl, 
               JetColl, VBF_JetColl, AK8_JetColl, B_JetColl,
               ev, METv ,param, weight_reg)) return;
  

  if(AK8_JetColl.size() > 0) {
    
    TString RegionBin= RunSignalRegionAK8String (IsSR,channel,qq, LepsT, LepsV, TauColl, 
                         JetColl, AK8_JetColl,B_JetColl,
                         ev, METv ,param,weight_reg) ;
    
    if(RegionBin != "false") {

      /// Region 1+2+3
      //FillLimitInput(LimitRegions, weight_reg,   RegionBin,  "LimitExtraction/"+param.Name,"SR1_"+channel_string,channel_string);

      if(param.syst_ == AnalyzerParameter::PDFUp)   weight_reg*=GetPDFUncertainty("SR1",1);
      if(param.syst_ == AnalyzerParameter::PDFDown) weight_reg*=GetPDFUncertainty("SR1",-1);


      if(IsSR&&param.IsCentral()) Fill_RegionPlots(param,"AllSR" , TauColl, 
				JetColl, AK8_JetColl, LepsT, 
				METv, nPV, weight_reg);
      ///Update

      //      if(IsSR) FillCutflow(HNL_LeptonCore::SR, weight_reg, "SR1",param);

      if(IsSR) FillCutflow(HNL_LeptonCore::ChannelDepSR1, weight_reg, channel_string +"_SR1",param);
      else FillCutflow(HNL_LeptonCore::ChannelDepCR1, weight_reg, channel_string +"_CR1",param);
      
      //// Region1 only limit
      FillLimitInput(LimitRegionR1, weight_reg,   RegionBin,  "LimitExtraction/"+param.Name,"SR1_"+channel_string,channel_string);
    }

    return;
  }//// END OF AK8 Loop
  else{

    //// Fail AK8 Req

    TString RegionBin = RunSignalRegionWWString(IsSR, channel,qq, LepsT, LepsV,  TauColl, VBF_JetColl,  AK8_JetColl, B_JetColl,ev, METv, param,  weight_reg);
    
    if(RegionBin != "false") {

      if(param.syst_ == AnalyzerParameter::PDFUp)   weight_reg*=GetPDFUncertainty("SR2",1);
      if(param.syst_ == AnalyzerParameter::PDFDown) weight_reg*=GetPDFUncertainty("SR2",-1);

     
      if(IsSR&&param.IsCentral()) Fill_RegionPlots(param,"AllSR" , TauColl, JetColl, AK8_JetColl, LepsT,  METv, nPV, weight_reg);

      if(IsSR)FillCutflow(HNL_LeptonCore::ChannelDepSR2, weight_reg, channel_string +"_SR2",param);
      else FillCutflow(HNL_LeptonCore::ChannelDepCR2, weight_reg, channel_string +"_CR2",param);
      
      /// Region 1+2+3
      //      FillLimitInput(LimitRegions, weight_reg, RegionBin,"LimitExtraction/"+param.Name);

      /// Region 2 only Limit
      if(IsSR) FillLimitInput(LimitRegionR2, weight_reg, RegionBin,  "LimitExtraction/"+param.Name,"SR2",channel_string);
      else {
	FillLimitInput(LimitRegionR2, weight_reg, RegionBin,  "LimitExtraction/"+param.Name,"CR2",channel_string);
      }
      //// Low Mass BDT Binned R1+2+3 only limit                                                                                                                          
      //      for(unsigned int im=0; im<MNStrList.size(); im++) FillLimitInput(LimitRegionsBDT, weight_reg, RegionBin,"LimitExtractionBDT/"+param.Name+"/M"+MNStrList[im],"SR3_"+channel_string+"_"+GetYearString()+"_"+MNStrList[im],channel_string);
	
    }
    else{

      //// Fail VBF Req



      if(param.syst_ == AnalyzerParameter::PDFUp)   weight_reg*=GetPDFUncertainty("SR3",1);
      if(param.syst_ == AnalyzerParameter::PDFDown) weight_reg*=GetPDFUncertainty("SR3",-1);
      
      /// RunBDT checks if signal mc and mass <= 500
      if(RunBDT()){
	for(auto imapHP :FinalBDTHyperParamMap){
	  TString RegBDT = RunSignalRegionAK4StringBDT(IsSR,imapHP.first , imapHP.second.first, imapHP.second.second, channel,qq, LepsT, JetColl,  B_JetColl, ev, METv ,param,weight_reg);
	  
	  if(RegBDT != "false"){
	    
	    //// Low Mass BDT Binned R1+2+3 only limit input
	    //	  FillLimitInput(LimitRegionsBDT, weight_reg, RegBDT,"LimitExtractionBDT/"+param.Name+"/M"+imapHP.first,"SR3_"+channel_string+"_"+GetYearString()+"_"+imapHP.first);
	    /// R3 LowMass BDt Only limit input
	    FillLimitInput(LimitRegionsBDTR3, weight_reg, RegBDT,"LimitExtractionBDT/"+param.Name+"/M"+imapHP.first,"SR3BDT_"+channel_string+"_"+DataEra+"_"+imapHP.first,channel_string);
	    if(IsSR&&imapHP.first == "100") FillCutflow(HNL_LeptonCore::SRLowMass, weight_reg, "SR3_LowMass",param);
	  }
	}
      }
      
      RegionBin  = RunSignalRegionAK4String (IsSR,channel,qq, LepsT, LepsV, TauColl, JetColl, AK8_JetColl, B_JetColl, ev, METv ,param,weight_reg);
      if(RegionBin != "false") {
	if(IsSR&&param.IsCentral()) Fill_RegionPlots(param,"AllSR" , TauColl, JetColl, AK8_JetColl, LepsT,  METv, nPV, weight_reg);
	if(IsSR) FillCutflow(HNL_LeptonCore::ChannelDepSR3HM, weight_reg, channel_string +"_SR3",param);
	else FillCutflow(HNL_LeptonCore::ChannelDepCR3HM, weight_reg, channel_string +"_CR3",param);
	
	if(IsSR) FillCutflow(HNL_LeptonCore::SRHighMass, weight_reg, "SR3_HighMass",param);

	//// Binned R1+2+3 only limit input
	//	FillLimitInput(LimitRegions, weight_reg,   RegionBin,"LimitExtraction/"+param.Name);
	///  R3 HighMass only limit input
	FillLimitInput(LimitRegionR3, weight_reg, RegionBin,  "LimitExtraction/"+param.Name,"SR3",channel_string);
	
      }
    }
  }

  return;
}




bool  HNL_RegionDefinitions::PassPreselection(bool ApplyForSR,HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType qq, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, vector<Tau> TauColl,  std::vector<Jet> JetColl, std::vector<Jet> VBF_JetColl,std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param,  float w){

  // ONLY CODE SS PRESLECTION 
  
  // APPLY PT CUTS
  /*
    ee channel 25/10
    emu   25/5
    mm    20/5
    
    
   */
  
  if(run_Debug) cout << "HNL_RegionDefinitions::PassPreselection " << GetChannelString(channel) <<  endl;

  FillCutflow(HNL_LeptonCore::ChannelDepTrigger, w, GetChannelString(channel) +"_MultiTrigger",param);
  FillCutflow(HNL_LeptonCore::ChannelDepTrigger, w, GetChannelString(channel) +"_Trigger",param);

  // Make sure events contain 2 leps
  if (leps_veto.size() != 2) return false;
  
  // CUT ON MASS OF LL PAIR

  Particle ll =  (*leps[0]) + (*leps[1]);
  
  // VETO Z PEAK IN EE CHANNEL
  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return false;

  if(ll.M() < M_CUT_LL) return false; // TO_CHECK: IS 20 BEST OPTION


  if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRLowMass, w, "Preselection",param);
  if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRHighMass, w, "Preselection",param);

  if(ApplyForSR&&param.IsCentral())Fill_RegionPlots(param,"Preselection" , TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
  
  FillCutflow(HNL_LeptonCore::ChannelDepPresel, w, GetChannelString(channel) +"_Presel",param);
  
  return true;
}




bool  HNL_RegionDefinitions::RunSignalRegionAK8(bool ApplyForSR,HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq , std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto , std::vector<Tau> TauColl, std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param, float w){


  TString SR1String = RunSignalRegionAK8String(ApplyForSR,channel, qq, leps, leps_veto, TauColl,JetColl,AK8_JetColl,B_JetColl,ev, METv, param, w);
  
  if(SR1String == "false") return false;
  else return true;
  
}


/// Return TString for Limit bin

TString HNL_RegionDefinitions::RunSignalRegionAK8String(bool ApplyForSR, 
                            HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq , 
                            std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto , std::vector<Tau> TauColl,
                            std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, 
                            Event ev, Particle METv, AnalyzerParameter param,  float w){
   
  double met_cut     = 15;
  double met2_st     = ev.MET2ST(); 
  bool PassHMMet     = ApplyForSR ? (met2_st < met_cut) :  (met2_st > met_cut);
  bool PassBJetMVeto = ApplyForSR ? (B_JetColl.size()==0) : (B_JetColl.size()== 1);
  bool PassRegionReq = ApplyForSR ? (PassHMMet && PassBJetMVeto) :  (PassHMMet ||  PassBJetMVeto) ;


  TString RegionTag                = ApplyForSR ? "SR1" : "CR1";
  HNL_LeptonCore::SearchRegion Reg = ApplyForSR ? HNL_LeptonCore::SR1 : HNL_LeptonCore::CR1;

  FillCutflow(Reg, w, RegionTag+"_Init",param);

  if(qq==Plus && leps[0]->Charge() < 0) return "false";
  if(qq==Minus && leps[0]->Charge() > 0) return "false";
  FillCutflow(Reg, w, RegionTag+"_lep_charge",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return "false";  
  if (leps_veto.size() != 2) return "false";

  if(leps[1]->Pt() < 20 ) return "false";

  FillCutflow(Reg, w, RegionTag+"_lep_pt",param);
  
  Particle ll =  (*leps[0]) + (*leps[1]);
  if(ll.M() < M_CUT_LL) return false;

  FillCutflow(Reg, w, RegionTag+"_dilep_mass",param);
    
  if(AK8_JetColl.size() != 1)  return "false";
  FillCutflow(Reg, w, RegionTag+"_1AK8",param);

  if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRLowMass,  w, "SR1",param);
  if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRHighMass, w, "SR1",param);
  if(ApplyForSR&&param.IsCentral()) Fill_RegionPlots(param,"Inclusive"+RegionTag , TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);

  if(PassHMMet)FillCutflow(Reg, w, RegionTag+"_MET",param);
  if(!ApplyForSR&&PassBJetMVeto) FillCutflow(Reg, w, RegionTag+"_bveto",param);
  
  if(PassRegionReq&&ApplyForSR) FillCutflow(Reg, w, RegionTag+"_bveto",param);    

  if(!PassRegionReq) return "false";

  //Fill Limit plot
  
  //// Fill Plots before All SR cuts for better stats 
  if(ApplyForSR&&param.IsCentral())   Fill_RegionPlots(param,"Pass"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);

  //// WMass cut removed for now to increase bkg 
  // Particle Wcand = AK8_JetColl[m] + *leps[0]+*leps[1];
  // FillCutflow(Reg, w, RegionTag+"_Wmass",param);

  Particle N1cand = AK8_JetColl[0] + *leps[0];
  double MN1 = (N1cand.M() > 2000.) ? 1999. : N1cand.M();

 
  double nbin_reg;
  double binvalue = GetLimitBin("CR_SR1_Inv",leps,JetColl,AK8_JetColl,ev,nbin_reg);    
  FillHist(  "LimitExtraction/"+ param.Name+"/LimitShape_"+RegionTag+"/N1Mass_Central", binvalue,  w, int(nbin_reg),0,nbin_reg ,"CR Binned");
  
  //// Return SR bin
  /// Bins defined in  HNL_LeptonCore::DefineLimitBins() in HNL_LeptonCore_LimitBins.C 
  vector<double> ml1jbins;
  if(ApplyForSR) ml1jbins = GetLimitBinBoundary("SR1",GetChannelString(channel));
  else  ml1jbins = GetLimitBinBoundary(RegionTag);
  
  for(unsigned int ibin=1; ibin < ml1jbins.size(); ibin++){
    if(MN1 < ml1jbins[ibin]) return RegionTag+"_MNbin"+to_string(ibin);
  }
  
  return "true";

}





bool  HNL_RegionDefinitions::RunSignalRegionWW(bool ApplyForSR,HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto , std::vector<Tau> TauColl,  std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param,   float w){

  TString SR2String = RunSignalRegionWWString(ApplyForSR,channel, qq, leps, leps_veto, TauColl,JetColl,AK8_JetColl,B_JetColl,ev, METv, param, w);
  if(SR2String == "false") return false;
  else return true;
  
}



TString HNL_RegionDefinitions::RunSignalRegionWWString(bool ApplyForSR,HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto , std::vector<Tau> TauColl,   std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param,  float w){

  double met_cut     = 15;
  double met2_st     = ev.MET2ST();

  TString RegionTag                = ApplyForSR ? "SR2" : "CR2";
  HNL_LeptonCore::SearchRegion Reg = ApplyForSR ? HNL_LeptonCore::SR2 : HNL_LeptonCore::CR2;


  bool PassHMMet     = ApplyForSR ? (met2_st < met_cut) :  (met2_st > met_cut);
  bool PassBJetMVeto = ApplyForSR ? (B_JetColl.size()==0) : (B_JetColl.size() ==1);

  bool PassRegionReq = ApplyForSR ? (PassHMMet && PassBJetMVeto) :  (PassHMMet ||  PassBJetMVeto) ;


  if (leps_veto.size() != 2) return "false";

  if(qq==Plus  && leps[0]->Charge() < 0) return "false";
  if(qq==Minus && leps[0]->Charge() > 0) return "false";

  FillCutflow(Reg, w, RegionTag+"_lep_charge",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return "false";

  if(leps[1]->Pt() < 15.) return "false";
  FillCutflow(Reg, w, RegionTag+"_lep_pt",param);

  if(ApplyForSR) Fill_RegionPlots(param,"Initial"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);

  bool use_leadjets=true;
  //  double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );
  //  if(ll_dphi < 2.) return "false";
  FillCutflow(Reg, w, RegionTag+"_DPhi",param);
  
  if( ( (*leps[0]) + (*leps[1]) ).M() < 20.) return "false";
  FillCutflow(Reg, w, RegionTag+"_LLMass",param);

  if(JetColl.size() < 2) return "false";
  FillCutflow(Reg, w, RegionTag+"_DiJet",param);

  int ijet1(0), ijet2(1);

  if(use_leadjets){ijet1=0;ijet2=1;}

  double maxDiJetDeta =fabs(JetColl[ijet1].Eta() - JetColl[ijet2].Eta());

  if(maxDiJetDeta < 2.5) return "false";
  FillCutflow(Reg, w, RegionTag+"_DiJetEta",param);

  Particle JJ = JetColl[ijet1] + JetColl[ijet2];
  if(JJ.M() < 750) return "false";
  FillCutflow(Reg, w, RegionTag+"_DiJetMass",param);
  
  double Av_JetEta= 0.5*(JetColl[ijet1].Eta()+ JetColl[ijet2].Eta());
  double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);
  
  if(zeppenfeld > 0.75) return "false";
  FillCutflow(Reg, w, RegionTag+"_Zep",param);
  
  if(!PassVBF(JetColl,leps,750., true)) return "false";
    
  if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRLowMass, w, "SR2",param);
  if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRHighMass, w, "SR2",param);
  if(ApplyForSR&&param.IsCentral()) Fill_RegionPlots(param,"Inclusive"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
  
  FillCutflow(Reg, w, RegionTag+"_VBF",param);
  
  if(PassRegionReq){

    if(PassHMMet) FillCutflow(Reg, w, RegionTag+"_met",param);
    if(PassBJetMVeto) FillCutflow(Reg, w, RegionTag+"_bveto",param);

    if(ApplyForSR&&param.IsCentral())Fill_RegionPlots(param,"Pass"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);      

    double HTOverPT = leps[0]->HTOverPt();
    
    double nbin_reg;
    double binvalue = GetLimitBin("CR_SR2_Inv",leps,JetColl,AK8_JetColl,ev,nbin_reg);
    FillHist(  "LimitExtraction/"+ param.Name+"/LimitShape_"+RegionTag+"/HT_PT_Central", binvalue,  w, int(nbin_reg),0,nbin_reg ,"Reco H_{T}/P_{T}^{lep1}");

    FillCutflow(Reg, w, RegionTag+"_ht_lt1",param);
    
    double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );
    
    
    if(!ApplyForSR){
      if(DataYear == 2016){
	if(HTOverPT < 3.) return  RegionTag+"_HTLTbin1";
	else if(HTOverPT < 5.) return  RegionTag+"_HTLTbin2";
	else return  RegionTag+"_HTLTbin3";
      }
      if(DataYear == 2017){
	if(HTOverPT < 3.) return  RegionTag+"_HTLTbin1";
	else if(HTOverPT < 5.) return  RegionTag+"_HTLTbin2";
	else return  RegionTag+"_HTLTbin3";
      }
      if(DataYear == 2018){
	if(HTOverPT < 3.) return  RegionTag+"_HTLTbin1";
	else if(HTOverPT < 5.) return  RegionTag+"_HTLTbin2";
	else return  RegionTag+"_HTLTbin3";
      }
    }
    else{

      if(DataYear == 2016){
	if(ll_dphi > 2.) {
	  if(HTOverPT < 2.) return RegionTag+"_HTLTbin1";
	  return RegionTag+"_HTLTbin2";
	}
	else{
	  if(HTOverPT < 3.)  return RegionTag+"_HTLTbin3";
	  else if(HTOverPT < 5.)  return RegionTag+"_HTLTbin4";
	  else return RegionTag+"_HTLTbin5";
	}
      }

      if(DataYear == 2017){
	if(ll_dphi > 2.) {
	  if(HTOverPT < 3.){
	    if (leps[1]->Pt() > 80.)      return RegionTag+"_HTLTbin1";
	    else return RegionTag+"_HTLTbin2";
	  }
	  else return RegionTag+"_HTLTbin3";
	}
	else{
	  if(HTOverPT < 3.)  return RegionTag+"_HTLTbin4";
	  else return RegionTag+"_HTLTbin5";
	}
      }
      if(DataYear== 2018){
	if(ll_dphi > 2.) {
	  if(HTOverPT < 3.){
	    if (leps[1]->Pt() > 80.)      return RegionTag+"_HTLTbin1";
	    else return RegionTag+"_HTLTbin2";
	  }
	  else return RegionTag+"_HTLTbin3";
	}
	else{
	  if(HTOverPT < 3.)  return RegionTag+"_HTLTbin4";
	  else return RegionTag+"_HTLTbin5";
	}
      }
      
    }
  }
  return "false";
}



bool  HNL_RegionDefinitions::RunSignalRegionAK4(bool ApplyForSR,HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto ,  std::vector<Tau> TauColl,std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param,   float w){

  TString SR3String = RunSignalRegionAK4String(ApplyForSR,channel, qq, leps, leps_veto, TauColl,JetColl,AK8_JetColl,B_JetColl,ev, METv, param,w);
  if(SR3String == "false") return false;
  else return true;

}




TString HNL_RegionDefinitions::RunSignalRegionAK4StringBDT(bool ApplyForSR, TString mN, TString NCut, TString NTree, HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *> LepTColl, std::vector<Jet> JetColl, std::vector<Jet> B_JetColl,Event ev, Particle METv, AnalyzerParameter param,  float w){
  
  //// Only fill Cutflow/plots for 1 mass point
  bool FillCutFlow = (mN=="100") ;

  TString BDTLabel = "M"+mN+"_NTrees"+NTree+"_NCuts"+NCut+"_MaxDepth3";
  
  double met_cut     = 15; /// Apply buffer between SR and CR MET cuts
  vector<FatJet> AK8_JetColl;

  HNL_LeptonCore::SearchRegion Reg = ApplyForSR ? HNL_LeptonCore::SR3BDT : HNL_LeptonCore::CR3BDT;


  double met2_st     = ev.MET2ST();

  bool PassHMMet     = ApplyForSR ? (met2_st < met_cut) :  (met2_st > met_cut);
  bool PassBJetMVeto = ApplyForSR ? (B_JetColl.size()==0) : (B_JetColl.size() ==1);
  bool PassRegionReq = ApplyForSR ? (PassHMMet && PassBJetMVeto) :  (PassHMMet ||  PassBJetMVeto) ;

  TString RegionTag                = ApplyForSR ? "SR3BDT" : "CR3BDT";



  if(qq==Plus && LepTColl[0]->Charge() < 0) return "false";
  if(qq==Minus && LepTColl[0]->Charge() > 0) return "false";
  if(FillCutFlow)FillCutflow(Reg, w, RegionTag+"_lep_charge",param);

  if(!CheckLeptonFlavourForChannel(channel, LepTColl)) return "false";
  if(FillCutFlow)FillCutflow(Reg, w, RegionTag+"_lep_pt",param);

  if (LepTColl.size() != 2) return "false";

  Particle ll =  (*LepTColl[0]) + (*LepTColl[1]);
  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return "false";

  if(FillCutFlow)  FillCutflow(Reg, w, RegionTag+"_dilep_mass",param);

  //float MVAvalue = EvaluateEventMVA(mN, NCut,NTree, channel, LepTColl, ev, METv, param, w);
  //cout << "Summary of BDT " << mN << endl;
  //cout << "Predetermined value = " << MVAvalue << " " <<  ev.HNL_MVA_Event(GetChannelString(channel)+"_"+mN) << endl;


  if(!PassRegionReq)  return "false";
  if(FillCutFlow)  FillCutflow(Reg, w, RegionTag+"_MET",param);
  if(FillCutFlow) FillCutflow(Reg, w, RegionTag+"_bveto",param);

  float MVAvalueIncl    = EvaluateEventMVA(mN, "Incl", NCut, NTree, channel, LepTColl, ev, METv, param, w, false); // true : fill MVA variables
  //float MVAvalueFake    = EvaluateEventMVA(mN, "Fake", NCut, NTree, channel, LepTColl, ev, METv, param, w);
  //float MVAvalueNonFake = EvaluateEventMVA(mN, "NonFake", NCut, NTree, channel, LepTColl, ev, METv, param, w);
  
  if(!ApplyForSR|| HasFlag("PlotBDT")){
    FillHist("LimitExtraction/"+param.Name+"/LimitShape_"+RegionTag+"BDT/MVA1D_Incl_AllJets_"+BDTLabel, MVAvalueIncl, w, 400, -1., 1.);
  }

  if(ApplyForSR && param.syst_ ==AnalyzerParameter::Central)     FillHist("LimitExtraction/"+param.Name+"/LimitShape_"+RegionTag+"BDT/MVA1D_Incl_AllJets_"+BDTLabel, MVAvalueIncl, w, 400, -1., 1.);

  vector<Tau> TauColl;
  if(FillCutFlow&&ApplyForSR&&param.IsCentral()) Fill_RegionPlots(param,"Pass"+RegionTag+"BDT" ,TauColl,  JetColl, AK8_JetColl, LepTColl,  METv, nPV, w);

  if(FillCutFlow){
    if(ApplyForSR)FillCutflow(HNL_LeptonCore::ChannelDepSR3, w, GetChannelString(channel) +"_"+RegionTag,param);
    else FillCutflow(HNL_LeptonCore::ChannelDepCR3, w, GetChannelString(channel) +"_"+RegionTag,param);
  }

  ///////// BDT BINS NEED OPTIMISING

  vector<pair<TString, double > > BDTLimitBinsMu;
  vector<pair<TString, double > > BDTLimitBinsEl;    
  vector<pair<TString, double > > BDTLimitBinsEMu;                                                     
  
  /// Change binning so no end bins have < 0.5 bkg
    
  //// Check Binnings in HNL_LeptonCore_BDT.C 
  if(ApplyForSR) {
    if(channel == MuMu) SetBinningBDT(GetChannelString(channel), mN,RegionTag,  RegionTag+"_"+GetChannelString(channel)+"_"+ DataEra + "_"+mN ,  BDTLimitBinsMu);
    if(channel == EE)   SetBinningBDT(GetChannelString(channel), mN,RegionTag,  RegionTag+ "_"+GetChannelString(channel)+"_"+ DataEra + "_"+mN ,  BDTLimitBinsEl);
    if(channel == EMu)  SetBinningBDT(GetChannelString(channel), mN,RegionTag,  RegionTag+ "_"+GetChannelString(channel)+"_"+ DataEra + "_"+mN ,  BDTLimitBinsEMu);
  }
  else {

    if(channel == MuMu) SetBinningBDT(GetChannelString(channel), mN,RegionTag, RegionTag+"_"+GetChannelString(channel) ,  BDTLimitBinsMu);
    if(channel == EE)   SetBinningBDT(GetChannelString(channel), mN,RegionTag, RegionTag+"_"+GetChannelString(channel) ,  BDTLimitBinsEl);
    if(channel == EMu)  SetBinningBDT(GetChannelString(channel), mN,RegionTag, RegionTag+"_"+GetChannelString(channel) ,  BDTLimitBinsEMu);
  }
  
  /// Need to Add EMu Binning as current EMu uses EE
  
  double  bdtbin = 0.5;
  if(channel == MuMu){ 
    for(auto imap : BDTLimitBinsMu){
      if(MVAvalueIncl< imap.second) {
        FillHist( "LimitExtraction/"+param.Name+"/LimitShape_"+RegionTag+"BDT/MVA_"+BDTLabel+"_Binned",  bdtbin,  w,  BDTLimitBinsMu.size(), 0, double(BDTLimitBinsMu.size()),  RegionTag);
        return imap.first;
      }
      bdtbin=bdtbin+1.;
    }  
  }
  if(channel == EE){
        
    for(auto imap : BDTLimitBinsEl){
      if(MVAvalueIncl< imap.second) {
        FillHist( "LimitExtraction/"+param.Name+"/LimitShape_"+RegionTag+"BDT/MVA_"+BDTLabel+"_Binned",  bdtbin,  w, BDTLimitBinsEl.size(), 0, double(BDTLimitBinsEl.size()),  RegionTag);
        return imap.first;
      }
      bdtbin=bdtbin+1.;
    }
  }
  if(channel == EMu){

    for(auto imap : BDTLimitBinsEMu){
      if(MVAvalueIncl< imap.second) {
        FillHist( "LimitExtraction/"+param.Name+"/LimitShape_"+RegionTag+"BDT/MVA_"+BDTLabel+"_Binned",  bdtbin,  w, BDTLimitBinsEMu.size(), 0, double(BDTLimitBinsEMu.size()),  RegionTag);
        return imap.first;
      }
      bdtbin=bdtbin+1.;
    }
  }


  return "true";
}


TString HNL_RegionDefinitions::RunSignalRegionAK4String(bool ApplyForSR,HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto ,  std::vector<Tau> TauColl,std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param,  float w){

  double met_cut     = 15. ;
  double met2_st     = ev.MET2ST();
  bool PassHMMet     = ApplyForSR ? (met2_st < met_cut) :  (met2_st > met_cut);
  bool PassBJetMVeto = ApplyForSR ? (B_JetColl.size()==0) : (B_JetColl.size() == 1);
  bool PassRegionReq = ApplyForSR ? (PassHMMet && PassBJetMVeto) :  (PassHMMet ||  PassBJetMVeto) ;

  TString RegionTag                = ApplyForSR ? "SR3" : "CR3";
  HNL_LeptonCore::SearchRegion Reg = ApplyForSR ? HNL_LeptonCore::SR3 : HNL_LeptonCore::CR3;

  if(qq==Plus  && leps[0]->Charge() < 0) return "false";
  if(qq==Minus && leps[0]->Charge() > 0) return "false";
   FillCutflow(Reg, w, RegionTag+"_lep_charge",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return "false";

  FillCutflow(Reg, w, RegionTag+"_lep_pt",param);
  if(ApplyForSR&&param.IsCentral()) Fill_RegionPlots(param,"InclusiveSR3" ,TauColl,  JetColl, AK8_JetColl, leps,  METv, nPV, w);

  if (leps_veto.size() != 2) return "false";

  if(TauColl.size() > 0) return "false";
  FillCutflow(Reg, w, RegionTag+"_tauveto",param);

  Particle ll =  (*leps[0]) + (*leps[1]);
  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) {
    if(ApplyForSR&&param.IsCentral())Fill_RegionPlots(param,RegionTag+"ZPeak" ,TauColl,  JetColl, AK8_JetColl, leps,  METv, nPV, w);
    return "false";
  }
  

  FillCutflow(Reg, w, RegionTag+"_dilep_mass",param);

  if(!PassRegionReq) return "false";

  FillCutflow(Reg, w, RegionTag+"_MET",param);
  FillCutflow(Reg, w, RegionTag+"_bveto",param);
  
  if(JetColl.size() > 0){
    if(JetColl[0].Pt() < 25.)  return "false";
  }
  FillCutflow(Reg, w, RegionTag+"_J1Pt",param);
  
  /// Remove Low Pt in High Mass SR
  if(leps[1]->Pt() < 50 ||  ll.M() < 50) {
    return "false";
  }
  FillCutflow(Reg, w, RegionTag+"_L1Pt",param);
  
  if(ApplyForSR&&param.IsCentral()) {
    if(JetColl.size() == 0)  Fill_RegionPlots(param,"ZeroJetSR3" ,TauColl,  JetColl, AK8_JetColl, leps,  METv, nPV, w);
    else if(JetColl.size() == 1)  Fill_RegionPlots(param,"OneJetSR3" ,TauColl,  JetColl, AK8_JetColl, leps,  METv, nPV, w);
    else  Fill_RegionPlots(param,"DiJetSR3" ,TauColl,  JetColl, AK8_JetColl, leps,  METv, nPV, w);
  }

  double LT = leps[0]->Pt() + leps[1]->Pt();
  double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );                                                        
  
  ////// Set Limit Binned 
  if(JetColl.size() < 2){
    FillCutflow(Reg, w, RegionTag+"_lowjet",param);
    //// These cuts are temp HL will check
     
    return GetSR3StringBin(RegionTag,GetChannelString(channel), true, met2_st,LT,ll_dphi);
  }  
  
  if(ApplyForSR&&param.IsCentral())Fill_RegionPlots(param,"PassSR3" ,TauColl,  JetColl, AK8_JetColl, leps,  METv, nPV, w);
  
  if(JetColl.size() < 2)   return "false";
  FillCutflow(Reg, w, RegionTag+"_dijet",param);
                                                           
  TString LimitBin = GetSR3StringBin(RegionTag,GetChannelString(channel), false, met2_st,LT,ll_dphi);

    
  return LimitBin;
}


HNL_RegionDefinitions::HNL_RegionDefinitions(){
      
}
 
HNL_RegionDefinitions::~HNL_RegionDefinitions(){

  
}


void HNL_RegionDefinitions::RunSR3BDT(HNL_LeptonCore::ChargeType qq, std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto,  std::vector<Tau> TauColl, std::vector<Jet> JetColl, std::vector<Jet> VBF_JetColl,std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev,   Particle METv, AnalyzerParameter param,   float weight_ll){

  /// Function plots BDT for all Setup in MNStrList[im], NCutList[ic], NTreeList[]                                                                                                                                
  // The string returned can be ignored, since this is for use in Limit plot, but function make plot in LimitInputSR3BDT/param.Name                                                                               

  vector<HNL_LeptonCore::Channel> channels = {EE,MuMu, EMu};
  std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(muons_veto,electrons_veto,param);
  std::vector<Lepton *> leps       = MakeLeptonPointerVector(muons,electrons,param);
  for(auto dilep_channel : channels){

    for(unsigned int im=0; im<MNStrList.size(); im++){
      for(unsigned int ic=0; ic<NCutList.size(); ic++){
        for(unsigned int it=0; it<NTreeList.size(); it++){
          RunSignalRegionAK4StringBDT(true,MNStrList[im], NCutList[ic], NTreeList[it], dilep_channel,qq, leps, JetColl,B_JetColl, ev, METv ,param,weight_ll);

        }
      }
    }
  }


  return;

}



bool HNL_RegionDefinitions::PassVBFInitial(vector<Jet>  JetColl){

  if(JetColl.size() < 2) return false;


  int ijet1(0), ijet2(1);
  double maxDiJetDeta = fabs(JetColl[ijet1].Eta() - JetColl[ijet2].Eta());


  if(maxDiJetDeta < 2.5) return false;
  Particle JJ = JetColl[ijet1] + JetColl[ijet2];
  if(JJ.M() < 450.) return false;

  return true;


}

bool HNL_RegionDefinitions::PassVBF(vector<Jet>  JetColl,std::vector<Lepton *> leps, double mjj_cut ,bool use_leadjets){

  if(leps.size() != 2) return false;
  //  double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );
  //  if(ll_dphi < 2.) return false;
  if( ( (*leps[0]) + (*leps[1]) ).M() < 20.) return false;
  if(JetColl.size() < 2) return false;

  int ijet1(0), ijet2(1);
  double maxDiJetDeta =fabs(JetColl[ijet1].Eta() - JetColl[ijet2].Eta());
  
  if(maxDiJetDeta < 2.5) return false;
  Particle JJ = JetColl[ijet1] + JetColl[ijet2];
  if(JJ.M() < mjj_cut) return false;
  double Av_JetEta= 0.5*(JetColl[ijet1].Eta()+ JetColl[ijet2].Eta());
  double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);

  if(zeppenfeld > 0.75) return false;

  return true;
}



