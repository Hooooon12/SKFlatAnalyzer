#include "HNL_Lepton_ChargeFlip.h"

void HNL_Lepton_ChargeFlip::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

  //// Declare  binnings
  
  /// 5 MODES
  ///

  // HasFlag("ShiftEnergy")  ---> Step 1 measure Pt response of CF vs Prompt Electron to Get Pt/Eta dep shift  
  // HasFlag("ElCFRates")    ---> Measure rates in DY/Top MC (using Pt CF response)set
  // HasFlag("ClosureTest") ---> Check mc closure  (Using only DY)
  // HasFlag("ScaleFactor")  ---> Compare Data SS vs OS weighted in BB / EC (using CF Rates) 
  
  //  cfEst->IgnoreNoHist = true;

  //  if(HasFlag("ShiftEnergy"))cfEst->IgnoreNoHist = true;
  
}

void HNL_Lepton_ChargeFlip::executeEvent(){
  
  vector<TString> LepIDs = {"HNL_ULID","TopHN", "POGTight", "HNTightV2", "passHEEPID_v1","passHEEPID_v3", "HNL_HighPt_ULID"};

  if(HasFlag("ClosureTest")) LepIDs = {"HNL_ULID" , "HNL_HighPt_ULID",  "passHEEPID_v1",  "passHEEPID_v3" };
  if(HasFlag("ScaleFactor")) LepIDs = {"HNL_ULID" , "HNL_HighPt_ULID", "passHEEPID_v1","passHEEPID_v3" };
  if(HasFlag("ScaleFactorTop"))  LepIDs = {"TopHN","HNL_HighPt_ULID"};

  //  if(HasFlag("ShiftEnergyZ")) LepIDs = {"HNL_ULID" , "POGTight","passHEEPID_v3", "TpHN", };


  for (auto id: LepIDs){
    AnalyzerParameter param = InitialiseHNLParameter("MCBkg");
    param.Name    =  id;
    param.DefName =  id;
    
    //// Do not include Xsec norm since looking to Add TT/DY samples in Eff
    param.Apply_Weight_Norm1Ipb = false;
    param.Apply_Weight_LumiNorm = false;
    
    if(HasFlag("ScaleFactor")){
      param.Apply_Weight_LumiNorm = true;
      param.Apply_Weight_Norm1Ipb = true;
    }
    if(HasFlag("ScaleFactorTop")){
      param.Apply_Weight_LumiNorm = true;
      param.Apply_Weight_Norm1Ipb = true;
    }
    if(HasFlag("ClosureTest")){
      param.Apply_Weight_LumiNorm = true;
      param.Apply_Weight_Norm1Ipb = true;
    }

    if(HasFlag("shiftEnergyZ")){
      param.Apply_Weight_LumiNorm = true;
      param.Apply_Weight_Norm1Ipb = true;
    }
    
    param.Apply_Weight_SumQ     = true;

    param.Apply_Weight_PileUp   = true;
    param.Apply_Weight_PreFire  = true;
    param.Apply_Weight_kFactor  = false;
    param.Apply_Weight_IDSF     = false;
    param.Apply_Weight_RECOSF     = true;
    param.Apply_Weight_TriggerSF= false;
      
    param.FakeMethod   = "DATA";
    param.CFMethod     = "DATA";
    param.ConvMethod   = "MC";
    
    param.Electron_Tight_ID = (id == "TopHN") ? "TopHNSST" :  id;
    param.Muon_Tight_ID   =    "POGTightWithTightIso";
    if(id =="POGTight")   param.Electron_Tight_ID  = "passPOGTight";
    
    if(id =="HNL_ULID")      param.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_HNL_ULID";
    if(id =="HNL_HighPt_ULID")      param.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_HNL_HighPt_ULID";
    if(id =="TopHN")         param.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_TopHNSST";
    if(id =="HNTightV2")     param.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_HNTightV2";
    if(id =="POGTight")      param.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_POGTight";
    if(id =="passHEEPID_v1")   param.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_passHEEPID_v1";
    if(id =="passHEEPID_v3")   param.k.Electron_CF  = "CFRate_InvPtEta3_PBSExtrap_Central_passHEEPID_v3";


    executeEventFromParameter(param);
  }
}

// check if fitting is useless
/*
 execute under --userflag CFfitval
1. pick up some processes with large xsec (ttbar DY)
2. choose OS ee events
3-1. use CF rate fit function to estimate SS ee from OS ee events
3-2. use CF rate bin values to estimate SS ee from OS ee events

and compare 3-1 and 3-2

*/

void HNL_Lepton_ChargeFlip::executeEventFromParameter(AnalyzerParameter param){

  if(_jentry==0) param.PrintParameters();
  run_Debug = (_jentry%nLog==0);

  if(!PassMETFilter()) return;

 
  Event ev = GetEvent();

  double EvWeight =SetupWeight(ev,param);
  if(HasFlag("ElCFRates")) EvWeight=1;

  if(_jentry<10) cout << "EvWeight  = " << EvWeight << endl;
  // Electrons 
  std::vector<Electron> ElectronColltmp;
  if(param.Electron_Tight_ID.Contains("HEEP")) ElectronColltmp = GetElectrons(param.Electron_Tight_ID,15.,2.5);
  else ElectronColltmp = GetElectrons(param.Electron_Tight_ID,15.,2.5);
  
  if(run_Debug) cout <<  "Number of All ID Electrons = " << ElectronColltmp.size() << endl;

  // Muons
  std::vector<Muon> MuonColl; std::vector<Muon> MuonCollVeto;
  if(param.Muon_Tight_ID.Contains("HEEP")) MuonColl = GetMuons(param.Muon_Tight_ID,25.,2.4);
  else MuonColl = GetMuons(param.Muon_Tight_ID,10.,2.4);
  MuonCollVeto = GetMuons(param.Muon_Veto_ID,10.,2.4);
  std::sort(MuonColl.begin(),MuonColl.end(),PtComparing);

  // Gen

  vector<Electron> ElectronCollPrompt = ElectronPromptOnly(ElectronColltmp,All_Gens,param);
  vector<Electron> ElectronColl;
  if(IsData) ElectronColl =ElectronCollPrompt;
  else if(MCSample.Contains("WG") || MCSample.Contains("ZG"))   ElectronColl = ElectronCollPrompt;
  else {
    for(auto iel: ElectronCollPrompt){
      if(HasPromptConv(iel)) continue;
      if(iel.LeptonGenType() >= 4 ) continue;
      if(iel.LeptonGenType() <= -4 ) continue;
      ElectronColl.push_back(iel);
    }
  }
  /// If MC then only Prompt + CF are saved (except for Conv MC)

  vector<Muon> PromptMuonColl   = MuonPromptOnly(MuonColl,All_Gens,param);
  std::sort(ElectronColl.begin(),ElectronColl.end(),PtComparing);
  std::vector<Lepton *> Leptons = MakeLeptonPointerVector(PromptMuonColl,ElectronColl,param);

  if(run_Debug)cout <<  "Number of GEN MATCHED ID Electrons = "<< ElectronColl.size() << endl;


  // Jet
  vector<Jet> JetColl = GetJets(param.Jet_ID,10.,5.);

  if(!PassHEMVeto(Leptons,EvWeight)) return ;


  if(HasFlag("ElCFRates")){
    

    if(IsDATA) return;
    
    vector<double> v_nbins_invpt = cfEst->FindBin("InputBins_Version1");

    int nbins_invpt(v_nbins_invpt.size()-1);
    double invptbins [nbins_invpt+1];
    std::copy(v_nbins_invpt.begin(), v_nbins_invpt.end(), invptbins);

    vector<double> v_nbins_invpt2 = cfEst->FindBin("InputBins_Version2");
    int nbins_invpt2(v_nbins_invpt2.size()-1);
    double invptbins2 [nbins_invpt2+1]; 
    std::copy(v_nbins_invpt2.begin(), v_nbins_invpt2.end(), invptbins2);


    vector<double> v_nbins_invpt_ec1 = cfEst->FindBin("InputBins_EC1");
    int nbins_invpt_ec1(v_nbins_invpt_ec1.size()-1);
    double invptbinsEC1 [nbins_invpt_ec1+1];
    std::copy(v_nbins_invpt_ec1.begin(), v_nbins_invpt_ec1.end(), invptbinsEC1);

    vector<double> v_nbins_invpt_ec2 = cfEst->FindBin("InputBins_EC2");
    int nbins_invpt_ec2(v_nbins_invpt_ec2.size()-1);
    double invptbinsEC2 [nbins_invpt_ec2+1];
    std::copy(v_nbins_invpt_ec2.begin(), v_nbins_invpt_ec2.end(), invptbinsEC2);


    vector<double> v_nbins_pt = cfEst->FindBin("InputBins_Pt");
    int nbins_pt(v_nbins_pt.size()-1);
    double ptbins [nbins_pt+1];
    std::copy(v_nbins_pt.begin(), v_nbins_pt.end(), ptbins);

    int nbins_eta  = 4;
    int nbins_eta2 = 3;
    int nbins_eta3 = 6 ;

    int nbins_etaBB1 = 4 ;
    int nbins_etaBB2 = 3 ;
    int nbins_etaEC1 = 2 ;
    int nbins_etaEC2 = 1 ;

    /// Full eta range
    double etabins   [nbins_eta+1]       = {  0.,0.8,      1.4442,1.556,      2.5};
    double etabins2  [nbins_eta2+1]      = {  0.,          1.4442,1.556,      2.5};
    double etabins3  [nbins_eta3+1]      = {  0.,0.8, 1.1, 1.4442,1.556, 2.2, 2.5};

    /// Eta sepcific bins for pt variation
    double etabinsBB1  [nbins_etaBB1+1]  = {  0.,    0.8, 1.1,1.25, 1.4442};
    double etabinsBB2  [nbins_etaBB2+1]  = {  0.,    0.8, 1.1, 1.4442};
    double etabinsEC1  [nbins_etaEC1+1]  = {  1.556, 1.8, 2.2};
    double etabinsEC2  [nbins_etaEC2+1]  = {  2.2,   2.5};


    for(unsigned int i=0 ; i < ElectronColl.size() ; i++){

      /// Remove Conv and Fakeelectrons
      if(ElectronColl.at(i).LeptonGenType() <= 0 || ElectronColl.at(i).LeptonGenType() >=4) continue;
      
      /////TruthIdx returns matched lepton status 1 index
      int TruthIdx  = GenMatchedIdx(ElectronColl.at(i), All_Gens);;
      if(TruthIdx < 0 || (TruthIdx > int(All_Gens.size()))) continue;

      ///// Get access to Matched status 1 gen el
      Gen truth_lep = All_Gens[TruthIdx];
      if(truth_lep.PID() ==0 ) continue;


      bool IsCF = ElectronColl.at(i).LeptonIsCF();
    
      /// CF Rate Method 2 WIth Int Conv                                                                                                                    
      
      if(HasPromptConv(ElectronColl.at(i))) continue;
   
      for(unsigned int imethod = 0; imethod < 7; imethod++){
	
	double PtShift = 1. ;
	TString Method = "";

	if(imethod==0) {
          /// PtShift for CF -> Prompt                                                                                                                                                                                                                     
          PtShift = GetShiftCFEl(ElectronColl.at(i),param.Electron_Tight_ID,false,0);
          Method = "PBSExtrap_Central";
        }
	if(imethod==1) {
	  PtShift = GetShiftCFEl(ElectronColl.at(i),param.Electron_Tight_ID,false,1);
          Method = "PBSExtrap_PSigma";
        }
	if(imethod==2) {
          PtShift = GetShiftCFEl(ElectronColl.at(i),param.Electron_Tight_ID,false,-1);
          Method = "PBSExtrap_MSigma";
        }

	if(imethod==3) {
	  PtShift = 1. ;
	  Method = "NoS";
	}
	
	if(imethod==4) {
          PtShift = 0.987 ; /// 1.3 BUG in April 12 samples
          Method = "CS_Central";
	}
        if(imethod==5) {
          PtShift = 0.975 ; /// 1.3 BUG in April 12 samples                                                                                                                                                       
          Method = "CS_PSigma";
        }
        if(imethod==6) {
          PtShift = 0.995 ; /// 1.3 BUG in April 12 samples                                                                                                                                                       
          Method = "CS_MSigma";
	}
      
		
	vector<TString> FillCFrates = {"Denom"};
	if(IsCF) FillCFrates.push_back("Num");
	
	double ShiftValue = IsCF ? 1. : PtShift;
	for(auto icf : FillCFrates){
	  if(_jentry < 5) {						
	    cout << "Method= " << Method<< endl;
	    cout << _jentry << "ElectronColl.at(i).InvPt(ShiftValue) = " << ElectronColl.at(i).InvPt(ShiftValue) << endl;
	    cout << _jentry << "ElectronColl.at(i).InvPt() = " << ElectronColl.at(i).InvPt() << endl;
	  }
	  if(abs(ElectronColl.at(i).scEta())<0.4) FillHist(param.Name+"/CFRate/"+Method+"/EtaRegionBB1_"+icf,ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");
	  else if(abs(ElectronColl.at(i).scEta())<0.8) FillHist(param.Name+"/CFRate/"+Method+"/EtaRegionBB2_"+icf,ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");
	  if(abs(ElectronColl.at(i).scEta())<0.8)       FillHist(param.Name+"/CFRate/"+Method+"/EtaRegion1_"+icf,ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");
	  else if(abs(ElectronColl.at(i).scEta())<1.1)  FillHist(param.Name+"/CFRate/"+Method+"/EtaRegion2_"+icf,ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");
	  else if(abs(ElectronColl.at(i).scEta())<1.25) FillHist(param.Name+"/CFRate/"+Method+"/EtaRegion3_"+icf,ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");
	  else if(abs(ElectronColl.at(i).scEta())<1.5)  FillHist(param.Name+"/CFRate/"+Method+"/EtaRegion4_"+icf,ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");
	  else if(abs(ElectronColl.at(i).scEta())<1.8)  FillHist(param.Name+"/CFRate/"+Method+"/EtaRegion5_"+icf,ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");
	  else if(abs(ElectronColl.at(i).scEta())<2.2)  FillHist(param.Name+"/CFRate/"+Method+"/EtaRegion6_"+icf,ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");
	  else FillHist(param.Name+"/CFRate/"+Method+"/EtaRegion7_"+icf,ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");
	  
	  	  
	  //// Pt Inclusive
	  FillHist(param.Name+"/CFRateInv/"+Method + "/" +icf, ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");
	  FillHist(param.Name+"/CFRateInv/"+Method + "/FineBinning_" +icf, ElectronColl.at(i).InvPt(ShiftValue), EvWeight, 500, 0., 1.,"#pT^{-1}");
	  FillHist(param.Name+"/CFRatePt/"+Method + "/"  +icf, ElectronColl.at(i).PtMaxed(2000,ShiftValue), EvWeight,nbins_pt, ptbins,"pT");
	  FillHist(param.Name+"/CFRatePt/"+Method + "/FineBinning_"  +icf, ElectronColl.at(i).PtMaxed(500,ShiftValue), EvWeight,250,0.,500.,"pT");
	  
	  if(ElectronColl.at(i).scEta() < 0)          FillHist(param.Name+"/CFRateInvPosEta/"+Method + "/" +icf, ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");
	  else FillHist(param.Name+"/CFRateInvNegEta/"+Method + "/" +icf, ElectronColl.at(i).InvPt(ShiftValue), EvWeight, nbins_invpt, invptbins,"#pT^{-1}");


	  FillHist(param.Name+"/CFRate/absEta_Binning1/"+Method + "/"  +icf, fabs(ElectronColl.at(i).scEta()),  EvWeight, nbins_eta,etabins ,"#eta");
	  FillHist(param.Name+"/CFRate/absEta_Binning2/"+Method + "/"  +icf, fabs(ElectronColl.at(i).scEta()),  EvWeight, nbins_eta2,etabins2 ,"#eta");
	  FillHist(param.Name+"/CFRate/absEta_Binning3/"+Method + "/"  +icf, fabs(ElectronColl.at(i).scEta()),  EvWeight, nbins_eta3,etabins3 ,"#eta");
	  FillHist(param.Name+"/CFRate/absEta_BB1/"+Method + "/"  +icf, fabs(ElectronColl.at(i).scEta()),  EvWeight, nbins_etaBB1,etabinsBB1 ,"#eta");
	  FillHist(param.Name+"/CFRate/absEta_BB2/"+Method + "/"  +icf, fabs(ElectronColl.at(i).scEta()),  EvWeight, nbins_etaBB2,etabinsBB2 ,"#eta");
	  FillHist(param.Name+"/CFRate/absEta_EC1/"+Method + "/"  +icf, fabs(ElectronColl.at(i).scEta()),  EvWeight, nbins_etaEC1,etabinsEC1 ,"#eta");
	  FillHist(param.Name+"/CFRate/absEta_EC2/"+Method + "/"  +icf, fabs(ElectronColl.at(i).scEta()),  EvWeight, nbins_etaEC2,etabinsEC2 ,"#eta");
	  
	  FillHist(param.Name+"/CFRate_Eta/"+Method + "/"+icf, ElectronColl.at(i).scEta(), EvWeight, 50, -2.5, 2.5,"scEta");
	  
	  FillHist(param.Name+"/CFRate_PtEta1/"    +Method + "/"+icf,ElectronColl.at(i).PtMaxed(2000,ShiftValue),  fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_pt,        ptbins,    nbins_eta,etabins);
	  FillHist(param.Name+"/CFRate_PtEta3/"   +Method + "/"+icf,ElectronColl.at(i).PtMaxed(2000,ShiftValue), fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_pt,        ptbins,     nbins_eta3,etabins3);

	  FillHist(param.Name+"/CFRate_InvPtEta1/" +Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue) ,     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt, invptbins, nbins_eta,etabins);
	  FillHist(param.Name+"/CFRate_InvPtEta2/" +Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue) ,     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt, invptbins, nbins_eta2,etabins2);
	  FillHist(param.Name+"/CFRate_InvPtEta3/"+Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue),     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt,   invptbins, nbins_eta3,etabins3);

          FillHist(param.Name+"/CFRate_InvPt2Eta1/" +Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue) ,     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt2, invptbins2, nbins_eta,etabins);
          FillHist(param.Name+"/CFRate_InvPt2Eta2/" +Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue) ,     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt2, invptbins2, nbins_eta2,etabins2);
          FillHist(param.Name+"/CFRate_InvPt2Eta3/"+Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue),     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt2,   invptbins2, nbins_eta3,etabins3);

	  
	  ///// ETa specific pt binnings
	  FillHist(param.Name+"/CFRate_InvPtEtaBB1/"+Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue),     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt, invptbins, nbins_etaBB1,etabinsBB1);	  
	  FillHist(param.Name+"/CFRate_InvPtEtaBB2/"+Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue),     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt, invptbins, nbins_etaBB2,etabinsBB2);	  
	  FillHist(param.Name+"/CFRate_InvPtEtaEC1/"+Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue),     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt_ec1, invptbinsEC1, nbins_etaEC1,etabinsEC1);
	  FillHist(param.Name+"/CFRate_InvPtEtaEC2/"+Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue),     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt_ec2, invptbinsEC2, nbins_etaEC2,etabinsEC2);
	  
          FillHist(param.Name+"/CFRate_InvPt2EtaBB1/"+Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue),     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt2, invptbins2, nbins_etaBB1,etabinsBB1);
          FillHist(param.Name+"/CFRate_InvPt2EtaBB2/"+Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue),     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt2, invptbins2, nbins_etaBB2,etabinsBB2);
          FillHist(param.Name+"/CFRate_InvPt2EtaEC1/"+Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue),     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt2, invptbins2, nbins_etaEC1,etabinsEC1);
          FillHist(param.Name+"/CFRate_InvPt2EtaEC2/"+Method + "/"+icf,ElectronColl.at(i).InvPt(ShiftValue),     fabs(ElectronColl.at(i).scEta()), EvWeight,  nbins_invpt2, invptbins2, nbins_etaEC2,etabinsEC2);


	}
      }
    }
    return;
  }
  
  if(HasFlag("ShiftCFStudy")){

    for(auto iel : ElectronColl){
      Gen MatchedGen = GetGenMatchedLepton(iel, All_Gens);
      
      if(iel.LeptonIsCF()) {

	FillHist(param.Name+"/CFStudy/CF_El_Eta", iel.scEta() /MatchedGen.Eta() , EvWeight ,100, 0 , 5);
	FillHist(param.Name+"/CFStudy/CF_El_Phi", iel.Phi() /MatchedGen.Phi() , EvWeight ,100, 0 , 5);
        FillHist(param.Name+"/CFStudy/CF_El_E",   iel.E() /MatchedGen.E() , EvWeight ,100, 0 , 5);
        FillHist(param.Name+"/CFStudy/CF_El_Pt",  iel.Pt() /MatchedGen.Pt() , EvWeight ,100, 0 , 5);
	if(iel.Pt() < 250){
	  FillHist(param.Name+"/CFStudy/CF_El_MC",  iel.IsGsfCtfChargeConsistent() , EvWeight ,5, 0 , 5);
	  FillHist(param.Name+"/CFStudy/CF_El_IP3D",  iel.SIP3D(), EvWeight ,100, -10 , 10);
	  FillHist(param.Name+"/CFStudy/CF_El_DXY",  fabs(iel.dXY()), EvWeight ,100, 0 , 1);
	  FillHist(param.Name+"/CFStudy/CF_El_DZ",  fabs(iel.dZ()), EvWeight ,100, 0 , 1);
	}
	else{
	  FillHist(param.Name+"/CFStudy/CF_HPT_El_MC",  iel.IsGsfCtfChargeConsistent() , EvWeight ,5, 0 , 5);
          FillHist(param.Name+"/CFStudy/CF_HPT_El_IP3D",  iel.SIP3D(), EvWeight ,100, -10 , 10);
	  FillHist(param.Name+"/CFStudy/CF_HPT_El_DXY",  fabs(iel.dXY()), EvWeight ,100, 0 , 1);
	  FillHist(param.Name+"/CFStudy/CF_HPT_El_DZ",  fabs(iel.dZ()), EvWeight ,100, 0 , 1);

	}
      }
      else{
	FillHist(param.Name+"/CFStudy/Prompt_El_Eta", iel.scEta() /MatchedGen.Eta() , EvWeight ,100, 0 , 5);
        FillHist(param.Name+"/CFStudy/Prompt_El_Phi", iel.Phi() /MatchedGen.Phi() , EvWeight ,100, 0 , 5);
        FillHist(param.Name+"/CFStudy/Prompt_El_E",   iel.E() /MatchedGen.E() , EvWeight ,100, 0 , 5);
        FillHist(param.Name+"/CFStudy/Prompt_El_Pt",  iel.Pt() /MatchedGen.Pt() , EvWeight ,100, 0 , 5);
        FillHist(param.Name+"/CFStudy/Prompt_El_MC",  iel.IsGsfCtfChargeConsistent() , EvWeight ,5, 0 , 5);
      }
    }   
    
    return;
  }


  
  if(HasFlag("ClosureTest")){
    
    if(_jentry == 1){
      TString CFKey = param.Electron_Tight_ID;
      
      for(int ipt = 3; ipt < 350; ipt++){
	double lpt = double(ipt)*5.;
	if(lpt > 700) lpt = 699;
	
	double WFittedv3_Eta1       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1", "CFRate_InvPtEta3_PBSExtrap_Central_"   + CFKey,   0.1, lpt, 0);
	double WFittedv3_Eta2       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1", "CFRate_InvPtEta3_PBSExtrap_Central_"   + CFKey,   0.7, lpt, 0);
	double WFittedv3_Eta3       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1", "CFRate_InvPtEta3_PBSExtrap_Central_"   + CFKey,   0.9, lpt, 0);
	double WFittedv3_Eta4       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1", "CFRate_InvPtEta3_PBSExtrap_Central_"   + CFKey,   1.3, lpt, 0);
	double WFittedv3_Eta5       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1", "CFRate_InvPtEta3_PBSExtrap_Central_"   + CFKey,   2.1, lpt, 0);
	double WFittedv3_Eta6       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1", "CFRate_InvPtEta3_PBSExtrap_Central_"   + CFKey,   2.35, lpt, 0);
	
	FillHist(param.Name+"/CFRatePlotsInvPt/Rate_Fittedv3_Eta1", 1/lpt  , WFittedv3_Eta1           ,1000, 0, 0., 0.1);
	FillHist(param.Name+"/CFRatePlotsInvPt/Rate_Fittedv3_Eta2", 1/lpt  , WFittedv3_Eta2           ,1000, 0, 0., 0.1);
	FillHist(param.Name+"/CFRatePlotsInvPt/Rate_Fittedv3_Eta3", 1/lpt  , WFittedv3_Eta3           ,1000, 0, 0., 0.1);
	FillHist(param.Name+"/CFRatePlotsInvPt/Rate_Fittedv3_Eta4", 1/lpt  , WFittedv3_Eta4           ,1000, 0, 0., 0.1);
	FillHist(param.Name+"/CFRatePlotsInvPt/Rate_Fittedv3_Eta5", 1/lpt  , WFittedv3_Eta5           ,1000, 0, 0., 0.1);
	FillHist(param.Name+"/CFRatePlotsInvPt/Rate_Fittedv3_Eta6", 1/lpt  , WFittedv3_Eta6           ,1000, 0, 0., 0.1);

      }
    }
    
    int nbinNeg= 19;

    double pTbinNeg[nbinNeg+1] = {-500,-200,-100,-80,-50,-45,-40,-35,-30,-15, 15.,30.,35,40.,45, 50., 80.,100.,200.,500};

    if(run_Debug) cout << "ClosureTest processing " << endl;

    for(auto iel : ElectronColl){
      /// Remove Conv and Fakeelectrons                                                                                                                                                                                              
      if(iel.LeptonGenType() <= 0 || iel.LeptonGenType() >=3) continue;

      /////TruthIdx returns matched lepton status 1 index                                                                                                                                                                            
      int TruthIdx  = GenMatchedIdx(iel, All_Gens);;
      if(TruthIdx < 0 || (TruthIdx > int(All_Gens.size()))) continue;

      ///// Get access to Matched status 1 gen el                                                                                                                                                                                    
      Gen truth_lep = All_Gens[TruthIdx];
      if(truth_lep.PID() ==0 ) continue;

      bool IsCF = iel.LeptonIsCF();
      
      if(HasPromptConv(iel)) continue;
         
      if(IsCF){
	FillHist(param.Name+"/Closure/CF_LeptonPt", iel.PtMaxed(500) ,       EvWeight        , 250, 0, 500);

	if(fabs(iel.scEta()) < 0.8)                  FillHist(param.Name+"/Closure/CF_LeptonPt_EtaBin1", iel.PtMaxed(500) ,       EvWeight        , 250, 0, 500);
	else if(fabs(iel.scEta()) < 1.4442)          FillHist(param.Name+"/Closure/CF_LeptonPt_EtaBin2", iel.PtMaxed(500) ,       EvWeight        , 250, 0, 500);
	else if(fabs(iel.scEta()) > 1.556 && fabs(iel.scEta()) < 2.)         FillHist(param.Name+"/Closure/CF_LeptonPt_EtaBin3", iel.PtMaxed(500) ,       EvWeight        , 250, 0, 500);
	else  FillHist(param.Name+"/Closure/CF_LeptonPt_EtaBin4", iel.PtMaxed(500) ,       EvWeight        , 250, 0, 500);
	
	FillHist(param.Name+"/Closure/CF_LepEta",  fabs(iel.scEta()) ,       EvWeight        , 100,0, 2.5);
      }
      else {

        map<TString,vector<double> > CFMapShift;
        CFMapShift["_w_pbs"] = {GetShiftCFEl(iel,param.Electron_Tight_ID, false,0), GetShiftCFEl(iel,param.Electron_Tight_ID, false,1),GetShiftCFEl(iel,param.Electron_Tight_ID, false,-1) };
        
	//if(param.Electron_Tight_ID.Contains("HNL_ULID")) 
	CFMapShift["_w_cs"]  = { 0.987,0.975,0.995 };
        CFMapShift["_w_nos"] = {1,1,1};

	for(auto im : CFMapShift){
  
	  TString CFKey    = param.Electron_Tight_ID;
	  if(CFKey =="passPOGTight") CFKey="POGTight";
	  
	  Electron ShiftedEl = iel;
	  Electron ShiftedElSigUp = iel;
	  Electron ShiftedElSigDown = iel;

	  ShiftedEl*=im.second.at(0);
	  ShiftedElSigUp*=im.second.at(1);
	  ShiftedElSigDown*=im.second.at(2);
	  

	  TString CFKeyClv3 = "";

	  //// Stat
          TString CFKeyClv3_StatUp = "";
          TString CFKeyClv3_StatDown = "";

	  //// Syst stat + Closure
          TString CFKeyClv3_SystUp = "";
          TString CFKeyClv3_SystDown = "";

	  /// Smear syst
          TString CFKeyClv3_PS = "";
          TString CFKeyClv3_MS = "";


	  TString DirName = "Closure";
	  if(im.first == "_w_pbs") {
	    DirName = "Closure_PBS";

            CFKeyClv3        = "CFRate_InvPtEta3_PBSExtrap_Central_";
	    CFKeyClv3_StatUp  = "CFRate_InvPtEta3_PBSExtrap_Central_";
	    CFKeyClv3_StatDown  = "CFRate_InvPtEta3_PBSExtrap_Central_";
            CFKeyClv3_SystUp      = "CFRate_InvPtEta3_PBSExtrap_SystUp_";
            CFKeyClv3_SystDown    = "CFRate_InvPtEta3_PBSExtrap_SystDown_";
            CFKeyClv3_PS      = "CFRate_InvPtEta3_PBSExtrap_PSigma_";
            CFKeyClv3_MS      = "CFRate_InvPtEta3_PBSExtrap_MSigma_";


	  }
	  if(im.first == "_w_cs"){
	    DirName = "Closure_CS";

            CFKeyClv3       = "CFRate_InvPtEta3_CS_Central_";
            CFKeyClv3_StatUp  = "CFRate_InvPtEta3_CS_Central_";
            CFKeyClv3_StatDown  = "CFRate_InvPtEta3_CS_Central_";
            CFKeyClv3_SystUp      = "CFRate_InvPtEta3_CS_SystUp_";
            CFKeyClv3_SystDown    = "CFRate_InvPtEta3_CS_SystDown_";
            CFKeyClv3_PS      = "CFRate_InvPtEta3_CS_PSigma_";
            CFKeyClv3_MS      = "CFRate_InvPtEta3_CS_MSigma_";

	  }
	  if(im.first == "_w_nos"){
	    DirName = "Closure_NoShift";
	    
            CFKeyClv3        = "CFRate_InvPtEta3_NoS_Central_";
            CFKeyClv3_StatUp  = "CFRate_InvPtEta3_NoS_Central_";
            CFKeyClv3_StatDown  = "CFRate_InvPtEta3_NoS_Central_";
            CFKeyClv3_SystUp      = "CFRate_InvPtEta3_NoS_Central_";
            CFKeyClv3_SystDown    = "CFRate_InvPtEta3_NoS_Central_";
            CFKeyClv3_PS      = "CFRate_InvPtEta3_NoS_Central_";
            CFKeyClv3_MS      = "CFRate_InvPtEta3_NoS_Central_";

	  }
  
	  double CFClosureWFittedv3             = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1",             CFKeyClv3  + CFKey,   ShiftedEl.defEta(), ShiftedEl.Pt(), 0);
          double CFClosureWFittedv3_StatUp      = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1",             CFKeyClv3_StatUp  + CFKey,  ShiftedEl.defEta(),ShiftedEl.Pt(), 1);
          double CFClosureWFittedv3_StatDown    = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1",             CFKeyClv3_StatDown  + CFKey,   ShiftedEl.defEta(), ShiftedEl.Pt(), -1);
          double CFClosureWFittedv3_SystUp      = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1",             CFKeyClv3_SystUp  + CFKey,  ShiftedEl.defEta(),ShiftedEl.Pt(), 0);
          double CFClosureWFittedv3_SystDown    = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1",             CFKeyClv3_SystDown  + CFKey,   ShiftedEl.defEta(), ShiftedEl.Pt(), 0);
          double CFClosureWFittedv3_SUp     = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1",             CFKeyClv3_PS  + CFKey,   ShiftedElSigUp.defEta(), ShiftedElSigUp.Pt(), 0);
	  double CFClosureWFittedv3_SDown   = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1",             CFKeyClv3_MS  + CFKey,   ShiftedElSigDown.defEta(), ShiftedElSigDown.Pt(), 0);

	  //// Use All MC
	  FillHist(param.Name+"/"+DirName+"/NonCF_Fitted_Binningv3_LeptonPt_"     +im.first,                         ShiftedEl.PtMaxed(500) ,          CFClosureWFittedv3*EvWeight        , 250, 0, 500);
          FillHist(param.Name+"/"+DirName+"/NonCF_Fitted_Binningv3_StatUp_LeptonPt_"     +im.first,                  ShiftedEl.PtMaxed(500) ,          CFClosureWFittedv3_StatUp*EvWeight        , 250, 0, 500);
          FillHist(param.Name+"/"+DirName+"/NonCF_Fitted_Binningv3_StatDown_LeptonPt_"     +im.first,                ShiftedEl.PtMaxed(500) ,          CFClosureWFittedv3_StatDown*EvWeight        , 250, 0, 500);
          FillHist(param.Name+"/"+DirName+"/NonCF_Fitted_Binningv3_SystUp_LeptonPt_"     +im.first,                  ShiftedEl.PtMaxed(500) ,          CFClosureWFittedv3_SystUp*EvWeight        , 250, 0, 500);
          FillHist(param.Name+"/"+DirName+"/NonCF_Fitted_Binningv3_SystDown_LeptonPt_"     +im.first,                ShiftedEl.PtMaxed(500) ,          CFClosureWFittedv3_SystDown*EvWeight        , 250, 0, 500);

          FillHist(param.Name+"/"+DirName+"/NonCF_Fitted_Binningv3_ShiftUp_LeptonPt_"     +im.first,                  ShiftedElSigUp.PtMaxed(500) ,          CFClosureWFittedv3_SUp*EvWeight        , 250, 0, 500);
          FillHist(param.Name+"/"+DirName+"/NonCF_Fitted_Binningv3_ShiftDown_LeptonPt_"     +im.first,                  ShiftedElSigDown.PtMaxed(500) ,          CFClosureWFittedv3_SDown*EvWeight        , 250, 0,500);
	  


	}
      }
    }

    if(ElectronColl.size() != 2) return;
    if(Leptons.size() != 2) return;

    if(ElectronColl.at(0).Pt() < 25) return;
    if(ElectronColl.at(1).Pt() < 15) return;

    //// Select SS events in MC that are 1 Prompt + 1 CF
    if(SameCharge(ElectronColl)){
      int nCF = 0;
      if(ElectronColl.at(0).LeptonIsCF()) nCF++;
      if(ElectronColl.at(1).LeptonIsCF()) nCF++;
      if(ElectronColl.at(0).IsPrompt() && ElectronColl.at(1).IsPrompt() && (nCF == 1)){

	/// Remove rare converstions  where e->eee share SC using HasPromptConv
	if(!HasPromptConv(ElectronColl.at(0)) && !HasPromptConv(ElectronColl.at(1))) {
	  
	  // To be safe remove LeptonGenType() > 4 which are conversions
	  if(ElectronColl.at(0).LeptonGenType() < 3 &&  ElectronColl.at(1).LeptonGenType() <3) {
	    
	    // Construct LL candidate
	    Particle ZCand = ElectronColl.at(0)+ElectronColl.at(1);
	    double Zmass = ZCand.M(); 
	    if(Zmass < 50) Zmass=50;
	    if(Zmass > 150) Zmass=150;

	    TString  EtaCat = "BE";
	    if(ElectronColl[0].IsBB() && ElectronColl[1].IsBB()) EtaCat = "BB";
	    else if(ElectronColl[0].IsEC() && ElectronColl[1].IsEC()) EtaCat = "EE";

	    FillHist(param.Name+"/ZMass/SS_"+EtaCat, Zmass , EvWeight ,100 , 50, 150);
	    FillHist(param.Name+"/Z/SS_"+EtaCat+"_LepPt",  ElectronColl[0].PtMaxed(500) ,         EvWeight , nbinNeg, pTbinNeg,"pT");
	    FillHist(param.Name+"/Z/SS_"+EtaCat+"_LepPt", -1*ElectronColl[1].PtMaxed(500) ,      EvWeight , nbinNeg, pTbinNeg,"pT");
	    
	    FillHist(param.Name+"/Z/SS_LepPt", ElectronColl[0].PtMaxed(500) ,         EvWeight , nbinNeg, pTbinNeg,"pT");
	    FillHist(param.Name+"/Z/SS_LepPt", -1*ElectronColl[1].PtMaxed(500) ,      EvWeight , nbinNeg, pTbinNeg,"pT");
	    FillHist(param.Name+"/Z/SS_LepsEta",  (ElectronColl[0].scEta()+2.5) , EvWeight, 100., -5., 5., "Eta");
	    FillHist(param.Name+"/Z/SS_LepsEta",  -1*(ElectronColl[1].scEta()+2.5) , EvWeight, 100., -5., 5., "Eta");
	    
            FillHist(param.Name+"/Z/SS_LepEta",  ElectronColl[0].scEta() , EvWeight, 100., -5., 5., "Eta");
            FillHist(param.Name+"/Z/SS_LepEta",  ElectronColl[1].scEta() , EvWeight, 100., -5., 5., "Eta");


	    FillHist(param.Name+"/ZMass/SS", Zmass , EvWeight ,100 , 50, 150);
            FillHist(param.Name+"/Z/SS_Lepton1Pt", ElectronColl[0].PtMaxed(500) ,      EvWeight , 100, 0, 500,"pT");
	    if(ElectronColl[0].IsBB())             FillHist(param.Name+"/Z/SS_BB_Lepton1Pt", ElectronColl[0].PtMaxed(500) ,      EvWeight , 100, 0, 500,"pT");
	    else  FillHist(param.Name+"/Z/SS_EC_Lepton1Pt", ElectronColl[0].PtMaxed(500) ,      EvWeight , 100, 0, 500,"pT");
            FillHist(param.Name+"/Z/SS_Lepton2Pt", ElectronColl[1].PtMaxed(500) ,      EvWeight , 40, 0, 200,"pT");
	    FillHist(param.Name+"/MET/SS",  GetvMET("PuppiT1xyULCorr",param).Pt() , EvWeight ,50 , 0, 100,"MET");
	    if(Zmass < 70)        FillHist(param.Name+"/Z/SS_Bin1_Lepton1Pt", ElectronColl[0].PtMaxed(500) ,      EvWeight , 100, 0, 500,"pT");
	    else if(Zmass < 78)        FillHist(param.Name+"/Z/SS_Bin2_Lepton1Pt", ElectronColl[0].PtMaxed(500) ,      EvWeight , 100, 0, 500,"pT");
	    else if(Zmass < 82)        FillHist(param.Name+"/Z/SS_Bin3_Lepton1Pt", ElectronColl[0].PtMaxed(500) ,      EvWeight , 100, 0, 500,"pT");
	    else if(Zmass < 85)        FillHist(param.Name+"/Z/SS_Bin4_Lepton1Pt", ElectronColl[0].PtMaxed(500) ,      EvWeight , 100, 0, 500,"pT");
	    else if(Zmass < 90)        FillHist(param.Name+"/Z/SS_Bin5_Lepton1Pt", ElectronColl[0].PtMaxed(500) ,      EvWeight , 100, 0, 500,"pT");



	  } /// Non Conv
	} 
      } //// END OF Prompt + CF
    } // END of SS
    else {

      //// Now plot OS * CF rate [Energy Shifted] to compare
      if(ElectronColl.at(0).IsPrompt() && ElectronColl.at(1).IsPrompt()){

	if(HasPromptConv(ElectronColl.at(0))  || HasPromptConv(ElectronColl.at(1))) return;
	if(ElectronColl.at(0).LeptonGenType() >= 3 || ElectronColl.at(1).LeptonGenType() >=3) return;
	

	//// Loop several Energy shift
	map<TString,TString> m_binning;
	//m_binning["Bin1"] ="InvPtEta1";
	//m_binning["Bin2"] ="InvPtEta2";
	m_binning["Bin3"] ="InvPtEta3";
	//m_binning["Bin4"] ="InvPt2Eta1";
	//m_binning["Bin5"] ="InvPt2Eta2";
	//m_binning["Bin6"] ="InvPt2Eta3";
	for(auto ib : m_binning){
	  TString BinningTag = "InputBins_Version1";
	  if(ib.second.Contains("InvPt2"))  BinningTag = "InputBins_Version2";
	  
	  for (unsigned int imethod = 0 ; imethod < 14; imethod++){
	    int nShift = 0;
	    if(imethod==2 || imethod==5 || imethod==10) nShift=1;
	    if(imethod==3 || imethod==6 || imethod==11)  nShift=-1;
	    
	    TString ShiftVersion = "NoShift";
	    if(imethod==1)  ShiftVersion = "Paired_ConstShift";
	    if(imethod==2)  ShiftVersion = "Paired_ConstShiftUp";
	    if(imethod==3)  ShiftVersion = "Paired_ConstShiftDown";
	    if(param.Electron_Tight_ID.Contains("HEEP") && ShiftVersion.Contains("Const")) continue;
	    if(imethod==4)  ShiftVersion = "Separate_BinnedShift";
	    if(imethod==5)  ShiftVersion = "Separate_BinnedShiftUp";
	    if(imethod==6)  ShiftVersion = "Separate_BinnedShiftDown";
	    if(imethod==7)  ShiftVersion = "Separate_BinnedStatUp";
	    if(imethod==8)  ShiftVersion = "Separate_BinnedStatDown";
	    if(imethod==9)  ShiftVersion = "Separate_BinnedShift";
	    if(imethod==10)  ShiftVersion = "Separate_BinnedShiftUp";
	    if(imethod==11)  ShiftVersion = "Separate_BinnedShiftDown";
	    if(imethod==12)  ShiftVersion = "Separate_BinnedStatUp";
	    if(imethod==13)  ShiftVersion = "Separate_BinnedStatDown";
	    
	    
	    TString CFKey = param.Electron_Tight_ID;
	    if(CFKey =="passPOGTight")CFKey="POGTight";
	    
	    //                  CFRate_InvPtEta1_PBSExtrap_Central_
	    TString CFString = "CFRate_"+ib.second+"_NoS_Central_" +CFKey;                                                                                                                                                                                          
	    if(imethod==4 ||  imethod==9)  CFString = "CFRate_"+ib.second+"_PBSExtrap_Central_" +CFKey;
	    if(imethod==7 ||imethod==12) CFString = "CFRate_"+ib.second+"_PBSExtrap_Central_" +CFKey;
	    if(imethod==8 ||imethod==13) CFString = "CFRate_"+ib.second+"_PBSExtrap_Central_" +CFKey;
	    
	    if(imethod==5 || imethod==10)  CFString = "CFRate_"+ib.second+"_PBSExtrap_PSigma_" +CFKey;
	    if(imethod==6 || imethod==11)  CFString = "CFRate_"+ib.second+"_PBSExtrap_MSigma_" +CFKey;
	    if(imethod==1 ) CFString = "CFRate_"+ib.second+"_CS_Central_" +CFKey;
	    if(imethod==2 ) CFString = "CFRate_"+ib.second+"_CS_PSigma_" +CFKey;
	    if(imethod==3) CFString = "CFRate_"+ib.second+"_CS_MSigma_" +CFKey;
	    
	    ShiftVersion = ShiftVersion + "_"+ib.first+"/"+CFString;
	    
	    // Uncorrected 
	    Electron unshifted_el1 = ElectronColl.at(0);
	    Electron unshifted_el2 = ElectronColl.at(1);
	    
	    vector<Electron> ShiftedEl;
	    Electron this_el1 = ElectronColl.at(0);
	    Electron this_el2 = ElectronColl.at(1);
	    
	    Particle ZCand = this_el1+this_el2;
	    double Zmass = ZCand.M();
	    
	    double shiftEl1 = GetShiftCFEl(this_el1,param.Electron_Tight_ID,false,nShift);
	    double shiftEl2 = GetShiftCFEl(this_el2,param.Electron_Tight_ID,false,nShift);
	    
	    
	    if(imethod  == 1) { this_el1*= 0.987;  this_el2*= 0.987;}
	    else if(imethod  == 2) { this_el1*= 0.975;    this_el2*= 0.975;}
	    else if(imethod  == 3) { this_el1*= 0.995;   this_el2*= 0.995;}
	    
	    else if     (imethod  == 4)   this_el1*= shiftEl1;
	    else if     (imethod  == 5)  this_el1*=shiftEl1; 
	    else if     (imethod  == 6)  this_el1*= shiftEl1; 
	    else if     (imethod  == 7)  this_el1*= shiftEl1; 
	    else if     (imethod  == 8)  this_el1*= shiftEl1; 
	    
	    else if     (imethod  == 9)   this_el2*= shiftEl2;
	    else if     (imethod  == 10)  this_el2*=shiftEl2;
	    else if     (imethod  == 11)  this_el2*= shiftEl2;
	    else if     (imethod  == 12)  this_el2*= shiftEl2;
	    else if     (imethod  == 13)  this_el2*= shiftEl2;
	    
	    Particle ZShiftedCand = this_el1+this_el2;
	    double ZShiftedmass = ZShiftedCand.M();
	    if(ZShiftedmass < 50) ZShiftedmass=50;
	    if(ZShiftedmass > 150) ZShiftedmass=150;
	    
	    if(Zmass < 50) Zmass = 50;
	    if(Zmass > 150) Zmass = 150;
	    
	    TString  OSEtaCat = "BE";
	    if(ElectronColl[0].IsBB() && ElectronColl[1].IsBB()) OSEtaCat = "BB";
	    if(ElectronColl[0].IsEC() && ElectronColl[1].IsEC()) OSEtaCat = "EC";
	    
	    
	    FillHist(param.Name+"/"+ShiftVersion+"/ZMass/OS_EnergyShift", ZShiftedmass , EvWeight ,100 , 50, 150);
	    FillHist(param.Name+"/"+ShiftVersion+"/ZMass/OS_"+OSEtaCat+"_EnergyShift", ZShiftedmass , EvWeight ,100 , 50, 150);
	    
	    ShiftedEl.clear();
	    ShiftedEl.push_back(this_el1);
	    ShiftedEl.push_back(this_el2);
	    
	    int SystRate = 0;
	    /// Now Stat error added to Syst hist
	    if(imethod == 7 ||imethod ==12) SystRate = 1;
	    if(imethod == 8 ||imethod ==13) SystRate = -1;
	    
	    double weight_shifted = 1;
	    if(imethod< 4) {
	      
	      double CFRate1   = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, BinningTag,CFString,this_el1.defEta(), this_el1.Pt(), SystRate);
	      double CFRate2   = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, BinningTag,CFString,this_el2.defEta(), this_el2.Pt(), SystRate);
	      
	      weight_shifted = (CFRate1/(1-CFRate1)) + (CFRate2/(1-CFRate2));
	    }
	    
	    else  if(imethod < 9) {
	      double CFRate1   = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, BinningTag,CFString,this_el1.defEta(), this_el1.Pt(), SystRate);
	      weight_shifted = (CFRate1/(1-CFRate1));
	    }
	    else {
	      double CFRate2   = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, BinningTag,CFString,this_el2.defEta(), this_el2.Pt(), SystRate);
	      weight_shifted = (CFRate2/(1-CFRate2));
	    }
	    
	    /// SHIFTED
	    FillHist(param.Name+"/"+ShiftVersion+"/ZMass/OS_Weighted_EnergyShift",  ZShiftedmass             , weight_shifted*EvWeight ,100 , 50, 150);
	    //if(ZShiftedmass <  70)           FillHist(param.Name+"/"+ShiftVersion+"/ZMass/OS_Bin1_Weighted_EnergyShift",  ZShiftedmass             , weight_shifted*EvWeight ,100 , 50, 150);
	    // else if(ZShiftedmass < 78)     FillHist(param.Name+"/"+ShiftVersion+"/ZMass/OS_Bin2_Weighted_EnergyShift",  ZShiftedmass             , weight_shifted*EvWeight ,100 , 50, 150);
	    //else if(ZShiftedmass < 82)     FillHist(param.Name+"/"+ShiftVersion+"/ZMass/OS_Bin3_Weighted_EnergyShift",  ZShiftedmass             , weight_shifted*EvWeight ,100 , 50, 150);
	    //else if(ZShiftedmass < 85)     FillHist(param.Name+"/"+ShiftVersion+"/ZMass/OS_Bin4_Weighted_EnergyShift",  ZShiftedmass             , weight_shifted*EvWeight ,100 , 50, 150);
	    //else if(ZShiftedmass < 90)     FillHist(param.Name+"/"+ShiftVersion+"/ZMass/OS_Bin5_Weighted_EnergyShift",  ZShiftedmass             , weight_shifted*EvWeight ,100 , 50, 150);
	    
	    
	    FillHist(param.Name+"/"+ShiftVersion+"/ZMass/OS_"+OSEtaCat+"_Weighted_EnergyShift",  ZShiftedmass             , weight_shifted*EvWeight ,100 , 50, 150);
	    
	    FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_"+OSEtaCat+"_Weighted_EnergyShift_Pt",   this_el1.PtMaxed(500)    , weight_shifted*EvWeight , nbinNeg, pTbinNeg,"pT");
	    FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_"+OSEtaCat+"_Weighted_EnergyShift_Pt",   -1*this_el2.PtMaxed(500) , weight_shifted*EvWeight , nbinNeg, pTbinNeg,"pT");
	    
	    
	    FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_Weighted_Eta",  this_el1.scEta()  , weight_shifted*EvWeight ,100., -5, 5, "Eta");
	    FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_Weighted_Eta",  this_el2.scEta()  , weight_shifted*EvWeight ,100., -5, 5, "Eta");
	    FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_Weighted_EnergyShift_Etas",  (this_el1.scEta()+2.5)   , weight_shifted*EvWeight ,100., -5, 5, "Eta");
	    FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_Weighted_EnergyShift_Etas",  -1*(this_el2.scEta()+2.5), weight_shifted*EvWeight ,100., -5, 5, "Eta");
	    FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_Weighted_EnergyShift_Lepton1Pt", this_el1.PtMaxed(500) ,            weight_shifted*EvWeight , 100, 0, 500,"pT");
	    FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_Weighted_EnergyShift_Lepton2Pt", this_el2.PtMaxed(500) ,            weight_shifted*EvWeight , 40, 0, 200,"pT");
	    
	    FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_EnergyShift_Lepton1Pt", this_el1.PtMaxed(500) ,            EvWeight , 100, 0, 500,"pT");

            if(this_el1.IsBB())FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_BB_EnergyShift_Lepton1Pt", this_el1.PtMaxed(500) ,            EvWeight , 100, 0, 500,"pT");
	    else  FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_EC_EnergyShift_Lepton1Pt", this_el1.PtMaxed(500) ,            EvWeight , 100, 0, 500,"pT");


	    FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_EnergyShift_Lepton2Pt", this_el2.PtMaxed(500) ,            EvWeight , 40, 0, 200,"pT");
	    
	    if(ZShiftedmass <  70)         FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_Bin1_Weighted_EnergyShift_Lepton1Pt", this_el1.PtMaxed(500) ,            weight_shifted*EvWeight , 100, 0, 500,"pT");
	    else if(ZShiftedmass < 78)     FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_Bin2_Weighted_EnergyShift_Lepton1Pt", this_el1.PtMaxed(500) ,            weight_shifted*EvWeight , 100, 0, 500,"pT");
	    else if(ZShiftedmass < 82)     FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_Bin3_Weighted_EnergyShift_Lepton1Pt", this_el1.PtMaxed(500) ,            weight_shifted*EvWeight , 100, 0, 500,"pT");
	    else if(ZShiftedmass < 85)     FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_Bin4_Weighted_EnergyShift_Lepton1Pt", this_el1.PtMaxed(500) ,            weight_shifted*EvWeight , 100, 0, 500,"pT");
	    else if(ZShiftedmass < 90)     FillHist(param.Name+"/"+ShiftVersion+"/Z/OS_Bin5_Weighted_EnergyShift_Lepton1Pt", this_el1.PtMaxed(500) ,            weight_shifted*EvWeight , 100, 0, 500,"pT");
	    
	    
	    
	    double met_x = GetvMET("PuppiT1xyULCorr",param).Px();
	    double met_y = GetvMET("PuppiT1xyULCorr",param).Py();
	    double px_orig(0.), py_orig(0.),px_corrected(0.), py_corrected(0.);
	    px_orig += ElectronColl.at(0).Pt() * TMath::Cos(ElectronColl.at(0).Phi());
	    px_orig += ElectronColl.at(1).Pt() * TMath::Cos(ElectronColl.at(1).Phi());
	    py_orig += ElectronColl.at(0).Pt() * TMath::Sin(ElectronColl.at(0).Phi());
	    py_orig += ElectronColl.at(1).Pt() * TMath::Sin(ElectronColl.at(1).Phi());
	    px_corrected += this_el1.Px();
	    px_corrected += this_el2.Px();
	    py_corrected += this_el1.Py();
	    py_corrected += this_el2.Py();
	    met_x = met_x + px_orig - px_corrected;
	    met_y = met_y + py_orig - py_corrected;
	    
	    Particle UpdatedMET;
	    UpdatedMET.SetPxPyPzE(met_x,met_y,0,sqrt(met_x*met_x+met_y*met_y));
	    
	    FillHist(param.Name+"/"+ShiftVersion+"/MET/OS_Corrected_EnergyShift",  UpdatedMET.Pt() , weight_shifted*EvWeight ,50 , 0, 100,"MET");
	    FillHist(param.Name+"/"+ShiftVersion+"/MET/OS_EnergyShift",  GetvMET("PuppiT1xyULCorr",param).Pt() , weight_shifted*EvWeight ,50 , 0, 100,"MET");
	    
	  }
	  
	}
      }
    }/// END OS
    return;
  }



  if(HasFlag("ShiftEnergyZ")){
    
    if(ElectronColl.size() !=2 ) return;

    Electron El1 =  ElectronColl[0];
    Electron El2 =  ElectronColl[1];
    
    bool CheckStatus=true;
    for(auto iel : ElectronColl){

      if(iel.LeptonGenType() <= 0 || iel.LeptonGenType() >=3) CheckStatus=false;
      
      /////TruthIdx returns matched lepton status 1 index                                                                                                                                                                                                                                                 
      int TruthIdx  = GenMatchedIdx(iel, All_Gens);;
      if(TruthIdx < 0 || (TruthIdx > int(All_Gens.size()))) continue;

      ///// Get access to Matched status 1 gen el                                                                                                                                                                                                                                                         
      Gen truth_lep = All_Gens[TruthIdx];
      if(truth_lep.PID() ==0 ) continue;

      bool hasOverlappingEl=false;
      int nSt1el(0);
      for(unsigned int i=2; i < All_Gens.size(); i++){
        if(TruthIdx == int(i)) continue;
        if(fabs(All_Gens.at(TruthIdx).Eta()-All_Gens.at(i).Eta())>0.1) continue;
        if(fabs(All_Gens.at(TruthIdx).DeltaPhi(All_Gens.at(i)))>0.3) continue;
        if(All_Gens[TruthIdx].MotherIndex() == All_Gens[i].MotherIndex()){
          if(All_Gens[i].Status() == 1) {

            if(fabs(All_Gens[i].PID()) == 11){
              Particle Z = All_Gens[i]  + truth_lep;
              if(Z.M() > 10) hasOverlappingEl=true;
            }
            if(fabs(All_Gens[i].PID()) == 11) nSt1el++;
          }
        }
      }

      if(hasOverlappingEl) CheckStatus=false;
      if(HasPromptConv(iel)) CheckStatus=false;

    }

    if(!CheckStatus) return;
    
    Particle ZCand  = ElectronColl.at(0)+ElectronColl.at(1);
    double MllLeft  = 50;
    double MllRight = 150;
    int NBin = 50;

    if(! (MllLeft<=ZCand.M()&&ZCand.M()<MllRight) ) return;

    double MllLeft2  = 70;
    double MllRight2 = 110;
    int NBin2 = 20;

    Gen El1Gen = All_Gens.at(GenMatchedIdx(El1, All_Gens));
    Gen El2Gen = All_Gens.at(GenMatchedIdx(El2, All_Gens));

    TString ElCat1 =  El1Gen.GetLeptonCatagory();
    TString ElCat2 = El2Gen.GetLeptonCatagory();

    TString EE_EtaBin;
    if(ElectronColl[0].IsBB() && ElectronColl[1].IsBB() ) EE_EtaBin= "BB";
    else   if(!ElectronColl[0].IsBB() && !ElectronColl[1].IsBB()) EE_EtaBin= "EE";
    else EE_EtaBin = "EB";

    if(SameCharge(ElectronColl)){

      //// Z ebent with CF in Barrel
      if(El1.LeptonIsCF() && El1.IsBB() && El2.IsEC()) {
	FillHist(param.Name+"/ZMass_BCF/"+ElCat1+"_ZMass_SS",                  ZCand.M(), EvWeight, NBin, MllLeft, MllRight);
	if((MllLeft2<=ZCand.M()&&ZCand.M()<MllRight2))         FillHist(param.Name+"/ZMass_BCF/"+ElCat1+"_ZMass_bin2_SS",  ZCand.M(), EvWeight, NBin2, MllLeft2, MllRight2);
      }
      if(El2.LeptonIsCF() && El2.IsBB() && El1.IsEC()) {
	FillHist(param.Name+"/ZMass_BCF/"+ElCat2+"_ZMass_SS",                  ZCand.M(), EvWeight, NBin, MllLeft, MllRight);
	if((MllLeft2<=ZCand.M()&&ZCand.M()<MllRight2))         FillHist(param.Name+"/ZMass_BCF/"+ElCat2+"_ZMass_bin2_SS",  ZCand.M(), EvWeight, NBin2, MllLeft2, MllRight2);
      } 
      
      if(El1.LeptonIsCF() && El1.IsEC() && El2.IsBB()) {
        FillHist(param.Name+"/ZMass_ECCF/"+ElCat1+"_ZMass_SS",                  ZCand.M(), EvWeight, NBin, MllLeft, MllRight);
	if((MllLeft2<=ZCand.M()&&ZCand.M()<MllRight2))         FillHist(param.Name+"/ZMass_ECCF/"+ElCat1+"_ZMass_bin2_SS",  ZCand.M(), EvWeight, NBin2, MllLeft2, MllRight2);

      } 
      if(El2.LeptonIsCF() && El2.IsEC() && El1.IsBB()) {
	FillHist(param.Name+"/ZMass_ECCF/"+ElCat2+"_ZMass_SS",                  ZCand.M(), EvWeight, NBin, MllLeft, MllRight);
	if((MllLeft2<=ZCand.M()&&ZCand.M()<MllRight2))         FillHist(param.Name+"/ZMass_ECCF/"+ElCat2+"_ZMass_bin2_SS",  ZCand.M(), EvWeight, NBin2, MllLeft2, MllRight2);

      }
      

      if(El1.LeptonIsCF())   FillHist(param.Name+"/ZMass/ZMass_SS",                  ZCand.M(), EvWeight, NBin, MllLeft, MllRight);
      if(El2.LeptonIsCF())   FillHist(param.Name+"/ZMass/ZMass_SS",                  ZCand.M(), EvWeight, NBin, MllLeft, MllRight);

      if(El1.LeptonIsCF())   FillHist(param.Name+"/ZMass_"+EE_EtaBin+"/ZMass_SS",                  ZCand.M(), EvWeight, NBin, MllLeft, MllRight);
      if(El2.LeptonIsCF())   FillHist(param.Name+"/ZMass_"+EE_EtaBin+"/ZMass_SS",                  ZCand.M(), EvWeight, NBin, MllLeft, MllRight);


      if(ZCand.M() < 20)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_0to20_SS", El1.Pt(), EvWeight, 100, 0, 500);
      else       if(ZCand.M() < 50)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_20to50_SS", El1.Pt(), EvWeight, 100, 0, 500);
      else       if(ZCand.M() < 70)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_50to70_SS", El1.Pt(), EvWeight, 100, 0, 500);
      else       if(ZCand.M() < 80)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_70to80_SS", El1.Pt(), EvWeight, 100, 0, 500);
      else       if(ZCand.M() < 90)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_80to90_SS", El1.Pt(), EvWeight, 100, 0, 500);
      else       if(ZCand.M() < 100)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_90to100_SS", El1.Pt(), EvWeight, 100, 0, 500);
      else       if(ZCand.M() < 200)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_100to200_SS", El1.Pt(), EvWeight, 100, 0, 500);
      else       if(ZCand.M() < 500)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_200to500_SS", El1.Pt(), EvWeight, 100, 0, 500);
      else FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_500to2000_SS", El1.Pt(), EvWeight, 100, 0, 500);

      if(ZCand.M() < 20)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_0to20_SS", El1.Eta(), EvWeight, 100, -2.5, 2.5);
      else       if(ZCand.M() < 50)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_20to50_SS", El1.Eta(), EvWeight, 100, -2.5, 2.5);
      else       if(ZCand.M() < 100)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_50to100_SS", El1.Eta(), EvWeight, 100, -2.5, 2.5);
      else       if(ZCand.M() < 200)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_100to200_SS", El1.Eta(), EvWeight, 100, -2.5, 2.5);
      else       if(ZCand.M() < 500)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_200to500_SS", El1.Eta(), EvWeight, 100, -2.5, 2.5);
      else FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_500to2000_SS", El1.Eta(), EvWeight, 100, -2.5, 2.5);


    }
    else{

      vector<double> Shifts_CL = {1,1,1, 1,
				  0.998,0.996,0.995,0.9925,0.99,0.9875,0.985,0.9825, 0.98,0.9775,0.975,0.9725,0.97,0.965, 0.96,0.955,0.95, 0.94,0.93,0.92,0.91,0.9};
      vector<TString> Shifts_CL_Tag = {"Func_0","Func_m1","Func_p1","_m0","_m1","_m2","_m3","_m4","_m5","_m6","_m7","_m8","_m9","_m10","_m11","_m12","_m13","_m14","_m15","_m16","_m17","_m18", "_m19","_m20","_m21","_m22"};
      for(long unsigned int is = 0 ; is < Shifts_CL.size(); is++){
	Electron El1_shift = El1; 
	Electron El2_shift = El2;

	if(is > 2){
          El1_shift*=Shifts_CL[is];
          El2_shift*=Shifts_CL[is];
        }
        else if(is ==2){
          double PtShift1 = GetShiftCFEl(El1,param.Electron_Tight_ID,false,1);
          double PtShift2 = GetShiftCFEl(El2,param.Electron_Tight_ID,false,1);

          El1_shift*=PtShift1;
          El2_shift*=PtShift2;

        }
        else if(is ==1){
          double PtShift1 = GetShiftCFEl(El1,param.Electron_Tight_ID,false,-1);
          double PtShift2 = GetShiftCFEl(El2,param.Electron_Tight_ID,false,-1);

          El1_shift*=PtShift1;
          El2_shift*=PtShift2;

        }
        else{

          double PtShift1 = GetShiftCFEl(El1,param.Electron_Tight_ID,false,0);
          double PtShift2 = GetShiftCFEl(El2,param.Electron_Tight_ID,false,0);

          El1_shift*=PtShift1;
          El2_shift*=PtShift2;

        }

	Particle ZCand_shift1  = El1_shift + El2;
	Particle ZCand_shift2  = El1 + El2_shift;
	
	double rate_cf1_shift= cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,"InvPtBB1", param.k.Electron_CF,El1_shift.defEta(), El1_shift.Pt(), 0);     
	double rate_cf2_shift= cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,"InvPtBB1", param.k.Electron_CF,El2_shift.defEta(), El2_shift.Pt(), 0);     

	if(El1.IsBB() && El2.IsEC()) {
	  FillHist(param.Name+"/ZMass_BCF/"+ElCat1+"_ZMass_OS_"+Shifts_CL_Tag[is],  ZCand_shift1.M(), EvWeight*rate_cf1_shift*(1-rate_cf1_shift), NBin, MllLeft, MllRight);
	  if((MllLeft2<=ZCand.M()&&ZCand.M()<MllRight2))   FillHist(param.Name+"/ZMass_BCF/"+ElCat1+"_ZMass_bin2_OS_"+Shifts_CL_Tag[is],  ZCand_shift1.M(), EvWeight*rate_cf1_shift*(1-rate_cf1_shift), NBin2, MllLeft2, MllRight2);

	}
	if(El2.IsBB() && El1.IsEC()) {
          FillHist(param.Name+"/ZMass_BCF/"+ElCat2+"_ZMass_OS_"+Shifts_CL_Tag[is],  ZCand_shift2.M(), EvWeight*rate_cf2_shift*(1-rate_cf2_shift), NBin, MllLeft, MllRight);
          if((MllLeft2<=ZCand.M()&&ZCand.M()<MllRight2))   FillHist(param.Name+"/ZMass_BCF/"+ElCat2+"_ZMass_bin2_OS_"+Shifts_CL_Tag[is],  ZCand_shift2.M(), EvWeight*rate_cf2_shift*(1-rate_cf2_shift), NBin2, MllLeft2, MllRight2);

	}

	if(El1.IsBB() && El2.IsEC()) {
          FillHist(param.Name+"/ZMass_ECCF/"+ElCat2+"_ZMass_OS_" +Shifts_CL_Tag[is],  ZCand_shift2.M(), EvWeight*rate_cf2_shift*(1-rate_cf2_shift), NBin, MllLeft, MllRight);
          if((MllLeft2<=ZCand.M()&&ZCand.M()<MllRight2))   FillHist(param.Name+"/ZMass_ECCF/"+ElCat2+"_ZMass_bin2_OS_"+Shifts_CL_Tag[is],  ZCand_shift2.M(), EvWeight*rate_cf2_shift*(1-rate_cf2_shift), NBin2, MllLeft2, MllRight2);

	}
        if(El2.IsBB() && El1.IsEC()) {
          FillHist(param.Name+"/ZMass_ECCF/"+ElCat1+"_ZMass_OS_"+Shifts_CL_Tag[is],  ZCand_shift1.M(), EvWeight*rate_cf1_shift*(1-rate_cf1_shift), NBin, MllLeft, MllRight);
          if((MllLeft2<=ZCand.M()&&ZCand.M()<MllRight2))   FillHist(param.Name+"/ZMass_ECCF/"+ElCat1+"_ZMass_bin2_OS_"+Shifts_CL_Tag[is],  ZCand_shift1.M(), EvWeight*rate_cf1_shift*(1-rate_cf1_shift), NBin2, MllLeft2, MllRight2);
	}

      }	  
      
      for(long unsigned int is = 0 ; is < Shifts_CL.size(); is++){
        Electron El1_shift = El1;
        Electron El2_shift = El2;
	
        Particle ZCand_shift  ;

	if(is > 2){
	  El1_shift*=Shifts_CL[is];
	  El2_shift*=Shifts_CL[is];
	}
	else if(is ==2){
	  double PtShift1 = GetShiftCFEl(El1,param.Electron_Tight_ID,false,1);
	  double PtShift2 = GetShiftCFEl(El2,param.Electron_Tight_ID,false,1);

          El1_shift*=PtShift1;
          El2_shift*=PtShift2;

	}
	else if(is ==1){
	  double PtShift1 = GetShiftCFEl(El1,param.Electron_Tight_ID,false,-1);
	  double PtShift2 = GetShiftCFEl(El2,param.Electron_Tight_ID,false,-1);

          El1_shift*=PtShift1;
          El2_shift*=PtShift2;

        }
	else{
	
	  double PtShift1 = GetShiftCFEl(El1,param.Electron_Tight_ID,false,0);
	  double PtShift2 = GetShiftCFEl(El2,param.Electron_Tight_ID,false,0);

	  El1_shift*=PtShift1;
          El2_shift*=PtShift2;
	  
	}

	ZCand_shift  = El1_shift + El2_shift;
	Particle ZCand_shift1  = El1_shift + El2;
	Particle ZCand_shift2  = El1 + El2_shift;
	
	
        //double rate_cf1= cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,"InvPtBB1", param.k.Electron_CF,El1.defEta(), El1.Pt(), 0);
	//double rate_cf2= cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,"InvPtBB1", param.k.Electron_CF,El2.defEta(), El2.Pt(), 0);

	double rate_cf1_shift= cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,"InvPtBB1", param.k.Electron_CF,El1_shift.defEta(), El1_shift.Pt(), 0);
        double rate_cf2_shift= cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,"InvPtBB1", param.k.Electron_CF,El2_shift.defEta(), El2_shift.Pt(), 0);


        FillHist(param.Name+"/ZMass/ZMass_OS"+Shifts_CL_Tag[is],  ZCand_shift.M(), EvWeight*rate_cf1_shift*(1-rate_cf1_shift), NBin, MllLeft, MllRight);
        FillHist(param.Name+"/ZMass/ZMass_OS"+Shifts_CL_Tag[is],  ZCand_shift.M(), EvWeight*rate_cf2_shift*(1-rate_cf2_shift), NBin, MllLeft, MllRight);

        FillHist(param.Name+"/ZMass/ZMass_sep_OS"+Shifts_CL_Tag[is],  ZCand_shift1.M(), EvWeight*rate_cf1_shift*(1-rate_cf1_shift), NBin, MllLeft, MllRight);
        FillHist(param.Name+"/ZMass/ZMass_sep_OS"+Shifts_CL_Tag[is],  ZCand_shift2.M(), EvWeight*rate_cf2_shift*(1-rate_cf2_shift), NBin, MllLeft, MllRight);

	FillHist(param.Name+"/ZMass_"+EE_EtaBin+"/ZMass_OS"+Shifts_CL_Tag[is],  ZCand_shift.M(), EvWeight*rate_cf1_shift*(1-rate_cf1_shift), NBin, MllLeft, MllRight);
        FillHist(param.Name+"/ZMass_"+EE_EtaBin+"/ZMass_OS"+Shifts_CL_Tag[is],  ZCand_shift.M(), EvWeight*rate_cf2_shift*(1-rate_cf2_shift), NBin, MllLeft, MllRight);

        FillHist(param.Name+"/ZMass_"+EE_EtaBin+"/ZMass_sep_OS"+Shifts_CL_Tag[is],  ZCand_shift1.M(), EvWeight*rate_cf1_shift*(1-rate_cf1_shift), NBin, MllLeft, MllRight);
	FillHist(param.Name+"/ZMass_"+EE_EtaBin+"/ZMass_sep_OS"+Shifts_CL_Tag[is],  ZCand_shift2.M(), EvWeight*rate_cf2_shift*(1-rate_cf2_shift), NBin, MllLeft, MllRight);

      }
      

      double rate_cf1= cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,"InvPtBB1", param.k.Electron_CF, El1.defEta(), El1.Pt(), 0);
      //      double rate_cf2= cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,"InvPtBB1", param.k.Electron_CF, El2.defEta(), El2.Pt(), 0);

      if(ZCand.M() < 20)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_0to20_OS", El1.Pt(), EvWeight*rate_cf1*(1-rate_cf1), 100, 0, 500);
      else       if(ZCand.M() < 50)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_20to50_OS", El1.Pt(), EvWeight*rate_cf1*(1-rate_cf1), 100, 0, 500);
      else       if(ZCand.M() < 70)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_50to70_OS", El1.Pt(), EvWeight*rate_cf1*(1-rate_cf1), 100, 0, 500);
      else       if(ZCand.M() < 80)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_70to80_OS", El1.Pt(), EvWeight*rate_cf1*(1-rate_cf1), 100, 0, 500);
      else       if(ZCand.M() < 90)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_80to90_OS", El1.Pt(), EvWeight*rate_cf1*(1-rate_cf1), 100, 0, 500);
      else       if(ZCand.M() < 100)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_90to100_OS", El1.Pt(), EvWeight*rate_cf1*(1-rate_cf1), 100, 0, 500);
      else       if(ZCand.M() < 200)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_100to200_OS", El1.Pt(), EvWeight*rate_cf1*(1-rate_cf1), 100, 0, 500);
      else       if(ZCand.M() < 500)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_200to500_OS", El1.Pt(), EvWeight*rate_cf1*(1-rate_cf1), 100, 0, 500);
      else FillHist(param.Name+"/Z_LepPt/Z_Lep1Pt_500to2000_OS", El1.Pt(), EvWeight*rate_cf1*(1-rate_cf1), 100, 0, 500);

      if(ZCand.M() < 20)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_0to20_SS", El1.Eta(), EvWeight*rate_cf1*(1-rate_cf1), 100, -2.5, 2.5);
      else       if(ZCand.M() < 50)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_20to50_SS", El1.Eta(), EvWeight*rate_cf1*(1-rate_cf1), 100, -2.5, 2.5);
      else       if(ZCand.M() < 100)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_50to100_SS", El1.Eta(), EvWeight*rate_cf1*(1-rate_cf1), 100, -2.5, 2.5);
      else       if(ZCand.M() < 200)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_100to200_SS", El1.Eta(), EvWeight*rate_cf1*(1-rate_cf1), 100, -2.5, 2.5);
      else       if(ZCand.M() < 500)   FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_200to500_SS", El1.Eta(), EvWeight*rate_cf1*(1-rate_cf1), 100, -2.5, 2.5);
      else FillHist(param.Name+"/Z_LepPt/Z_Lep1Eta_500to2000_SS", El1.Eta(), EvWeight*rate_cf1*(1-rate_cf1), 100, -2.5, 2.5);

      
    }
    return;
  }
  if(HasFlag("ShiftEnergy")){


    int nbins_res(33);
    double bins_res [nbins_res+1] = { -0.5, -0.25,-0.225, -0.2, -0.19, -0.18,  -0.17, -0.16, -0.15, -0.14, -0.13, -0.12, -0.11, -0.10,  -0.09,-0.08,-0.07,-0.06,-0.05,-0.04,-0.03,-0.02,-0.01,0.0, 0.01, 0.02, 0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.1};



    //// This function is used to measure the pt / eta dependance on the shift between Prompt El and CF electrons
    for(auto iel : ElectronColl){
      
      /// Remove Conv and Fakeelectrons
      if(iel.LeptonGenType() <= 0 || iel.LeptonGenType() >=3) continue;
      
      /////TruthIdx returns matched lepton status 1 index
      int TruthIdx  = GenMatchedIdx(iel, All_Gens);;
      if(TruthIdx < 0 || (TruthIdx > int(All_Gens.size()))) continue;

      ///// Get access to Matched status 1 gen el
      Gen truth_lep = All_Gens[TruthIdx];
      if(truth_lep.PID() ==0 ) continue;

      ///// Get sum of status 1 photon and electrons in dR < 0.1 to matched el 
      double PtSum = 0;
      Particle X = truth_lep;

      //// Check for 2 status 1 electrons overlaping; makes it complicated so veto for study
      bool hasOverlappingEl=false;
      
      /// check for rare CF 
      int nSt1el(0);
      
      for(unsigned int i=2; i < All_Gens.size(); i++){
	if(TruthIdx == int(i)) continue;
	if(fabs(All_Gens.at(TruthIdx).Eta()-All_Gens.at(i).Eta())>0.1) continue;
	if(fabs(All_Gens.at(TruthIdx).DeltaPhi(All_Gens.at(i)))>0.3) continue;
	if(All_Gens[TruthIdx].MotherIndex() == All_Gens[i].MotherIndex()){
	  if(All_Gens[i].Status() == 1) {

	    if(fabs(All_Gens[i].PID()) == 11){
	      Particle Z = All_Gens[i]  + truth_lep;
	      if(Z.M() > 10) hasOverlappingEl=true;
	    }
	    PtSum+= All_Gens[i].Pt() ;
	    X=X+All_Gens[i];
	    if(fabs(All_Gens[i].PID()) == 11) nSt1el++;
	  }
	}
      }
      
      if(hasOverlappingEl) {
	cout << "2 status 1 el overlapping " << endl;
	cout << "PtSum = " << PtSum << " truth_lep.Pt() = " << truth_lep.Pt() << " Type  " << iel.LeptonGenType() << endl;                                                                                  
	cout << " HasPromptConv(iel)  = " << HasPromptConv(iel) << endl;
	cout << "iel.LeptonIsCF() = " << iel.LeptonIsCF() << endl;
	return;
      }
     
      /// if nSt1el (number of status 1 electrons in dR < 0.4 from same mother , if > 1 e->eee  that passes JH Gen code
      bool bIsConv  =  HasPromptConv(iel);
      
      /// LeptonIsCF() == IsCF(iel, All_Gens, true);
      bool bIsCF     =  iel.LeptonIsCF() ;// IsCF(iel, All_Gens, true);
      
      double gen_d_pt = ((truth_lep.Pt()+PtSum) > 500) ? 500 : truth_lep.Pt() + PtSum;
      
      if(gen_d_pt > 499) continue;

      double gen_d_eta = X.Eta();
      double gen_d_phi = X.Phi();

      /// PtResponse measures how RECO pt compares to GEN pt
      double PtResponse   = (iel.Pt()    -  gen_d_pt) / gen_d_pt;
      double EtaResponse  = (iel.Eta()  -  gen_d_eta) / gen_d_eta;
      double PhiResponse  = (iel.Phi()  -  gen_d_phi) / gen_d_phi;

      
      if(bIsConv) continue;
      if(PtResponse < -0.5) {
	cout << "  " << endl;
	cout << "TruthIdx = " << TruthIdx << endl;
	cout << "Is CF = "<< iel.LeptonIsCF() << endl;
	cout << "PtResponse = " << PtResponse << endl;
	cout << "gen_d_pt = " << gen_d_pt <<  "  iel.Pt() = " << iel.Pt() << " truth_lep.Pt() = " << truth_lep.Pt() << endl;
	cout << "gen_d_eta = " << gen_d_eta <<  "  iel.Eta() = " << iel.Eta() << endl;
	cout << "gen_d_phi = " << gen_d_phi <<  "  iel.Phi() = " << iel.Phi() << endl;
 
	for(unsigned int i=2; i < All_Gens.size(); i++){
	 if(TruthIdx == int(i)) continue;
	 if(fabs(All_Gens.at(TruthIdx).Eta()-All_Gens.at(i).Eta())>0.1) continue;
	 if(All_Gens.at(TruthIdx).DeltaPhi(All_Gens.at(i))>0.3) continue;
	 if(All_Gens[TruthIdx].MotherIndex() == All_Gens[i].MotherIndex()){
	 }
	}
	PrintGen(All_Gens);
	PrintMatchedGen(All_Gens, Lepton(iel));
      }

      TString EtaBin = (iel.IsBB()) ? "BB" : "EC";

      //double rate_cf= cfEst->GetElectronCFRate(param.Electron_Tight_ID, param.Electron_CF_Key, iel.defEta(), iel.Pt(), 0);
  
    
      if(iel.IsPrompt() && !bIsCF){
	FillHist(param.Name+"/EnergyShift/"+EtaBin+"_Pt_Prompt", PtResponse ,   EvWeight , 200, -0.5,0.5);
	FillHist(param.Name+"/EnergyShift/"+EtaBin+"_Eta_Prompt", EtaResponse , EvWeight , 200, -0.1,0.1);
	FillHist(param.Name+"/EnergyShift/"+EtaBin+"_Phi_Prompt", PhiResponse , EvWeight , 200,  -0.1,0.1);
	
      }

    
      if(iel.IsPrompt() &&  bIsCF){
	FillHist(param.Name+"/EnergyShift/"+EtaBin+"_Pt_CF",  PtResponse , EvWeight , nbins_res,bins_res);
        FillHist(param.Name+"/EnergyShift/"+EtaBin+"_Eta_CF", EtaResponse , EvWeight , 200, -0.1,0.1);
	FillHist(param.Name+"/EnergyShift/"+EtaBin+"_Phi_CF", PhiResponse , EvWeight , 200,  -0.1,0.1);
      }


      TString PtBinLabel      = iel.GetCFShift_BinLabel(1.);  /// assignes bin based on BB/EC and pt 
      TString Gen_PtBinLabel  = X.GetCFShift_BinLabel(gen_d_pt);  /// assignes bin based on BB/EC and pt                                                                                                                                                                
          
      if(iel.IsPrompt() &&  bIsCF) {
        FillHist(param.Name+"/Pt/"+EtaBin+"_CF", iel.Pt() , EvWeight , 200, 0,2000);
	FillHist(param.Name+"/EnergyShift/"+EtaBin+PtBinLabel+"_CF", PtResponse , EvWeight ,nbins_res,bins_res);
	FillHist(param.Name+"/GenEnergyShift/"+EtaBin+Gen_PtBinLabel+"_CF", PtResponse , EvWeight ,nbins_res,bins_res);
	FillHist(param.Name+"/EnergyShift/"+EtaBin+"_CF", PtResponse , EvWeight ,nbins_res,bins_res);
      }

      
      /// Check Results in GetShiftCFEl

      if(iel.IsPrompt() &&  !bIsCF ) {
	
        FillHist(param.Name+"/Pt/"+EtaBin+"_Prompt", iel.Pt() , EvWeight , 200, 0,2000);

	double shiftEl = 1.;//GetShiftCFEl(iel,param.Electron_Tight_ID,IsData,0);
	double PtResponseShift_Pre = (shiftEl*iel.Pt() - gen_d_pt) / (gen_d_pt);

	/// Check performance of already measured shift values
	//        FillHist(param.Name+"/EnergyShift/"+EtaBin+"_Pt_CF",  PtResponse , EvWeight , nbins_res,bins_res);

	FillHist(param.Name+"/EnergyShift/"+EtaBin+"_PromptShifted_GetShiftCFEl", PtResponseShift_Pre , EvWeight , nbins_res,bins_res);
	FillHist(param.Name+"/EnergyShift/"+EtaBin+PtBinLabel+"_PromptShifted_GetShiftCFEl", PtResponseShift_Pre , EvWeight , nbins_res,bins_res);

	unsigned int nBins = 75;
	if(iel.Pt() < 50)  nBins = 125;
	
	for (unsigned int ishift = 0 ; ishift < nBins; ishift++){
	  double shiftEl = 1.025 - double(ishift)*0.001;
	  TString shift_string = DoubleToString(shiftEl);
	  double PtResponseShift = (shiftEl*iel.Pt() - gen_d_pt) / (gen_d_pt);
	  TString PtBinLabel_shift = iel.GetCFShift_BinLabel(shiftEl);
	  
	  FillHist(param.Name+"/EnergyShift/"+EtaBin+PtBinLabel_shift+"_PromptShifted_"+shift_string, PtResponseShift , EvWeight , nbins_res,bins_res);
	  FillHist(param.Name+"/GenEnergyShift/"+EtaBin+Gen_PtBinLabel+"_PromptShifted_"+shift_string, PtResponseShift , EvWeight , nbins_res,bins_res);
	  FillHist(param.Name+"/EnergyShift/"+EtaBin+"_PromptShifted_"+shift_string, PtResponseShift , EvWeight , nbins_res,bins_res);
	  
	}
      } /// Energy Shift
    }
    
    return;
  }

  
  if(HasFlag("ScaleFactor") || HasFlag("ScaleFactorTop") ){

    FillHist(param.Name+"/ZGSub/All",  1    , EvWeight ,2., 0, 2, "");

    if (!PassTriggerSelection(EE, ev,   Leptons,"Dilep")) return ;

    if(Leptons.size() != 2) return;
    if(ElectronColl.size() != 2) return;
    
    Particle ZCand  = ElectronColl.at(0)+ElectronColl.at(1);
    double MllLeft  = 50;
    double MllRight = 150;
    int NBin = 50;

    if(! (MllLeft<=ZCand.M()&&ZCand.M()<MllRight) ) return;

    if(!IsDATA) {
      
      if(MCSample.Contains("DY")){
	if(ElectronColl.at(0).LeptonGenType() <= 0 || ElectronColl.at(0).LeptonGenType() >=4) return;
	if(ElectronColl.at(1).LeptonGenType() <= 0 || ElectronColl.at(1).LeptonGenType() >=4) return;
	
	bool IsCF1 = ElectronColl.at(0).LeptonIsCF();
	bool IsCF2 = ElectronColl.at(1).LeptonIsCF();
	
	if(HasPromptConv(ElectronColl.at(0))) return;
	if(HasPromptConv(ElectronColl.at(1))) return;
	
	if(SameCharge(ElectronColl)){
	  if(!IsCF1 && !IsCF2) return;
	}
      }
      else{
	/// ZG Subtraction
        FillHist(param.Name+"/ZGSub/SS",  1    , EvWeight ,2., 0, 2, "");

	if(! (ElectronColl.at(0).IsConv() ||  ElectronColl.at(1).IsConv() )) return;
	EvWeight = -1*EvWeight;
        FillHist(param.Name+"/ZGSub/SS_noconv",  1    , EvWeight ,2., 0, 2, "");

      }
    }
    TString MassRange = "LT60";
    if(ZCand.M() < 60) MassRange ="LT60";
    else if(ZCand.M() < 85) MassRange ="LT85";
    else if(ZCand.M() < 100) MassRange ="LT100";
    else  MassRange ="GT100";

    if(HasFlag("RemoveLowPt"))  {
      if(ElectronColl.at(1).Pt() < 25) return;
    }
    
    Electron this_el1 = ElectronColl.at(0);
    Electron this_el2 = ElectronColl.at(1);
  
    TString EtaCat = "";
    if(abs(ElectronColl.at(0).scEta())<1.4442&&abs(ElectronColl.at(1).scEta())<1.4442) EtaCat = "BB";
    if((abs(ElectronColl.at(0).scEta())<1.4442&&abs(ElectronColl.at(1).scEta())>=1.556)||(abs(ElectronColl.at(0).scEta())>=1.556&&abs(ElectronColl.at(1).scEta())<1.4442)) EtaCat ="BE";
    if(abs(ElectronColl.at(0).scEta())>=1.556&&abs(ElectronColl.at(1).scEta())>=1.556) EtaCat = "EE";
    

    TString PtEtaCat1 = ElectronColl.at(0).PtEtaCategoryCFScaleFactor();
    TString PtEtaCat2 = ElectronColl.at(1).PtEtaCategoryCFScaleFactor();

    if(ElectronColl.at(0).Charge()*ElectronColl.at(1).Charge()>0){
      FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_ZMass_SS",                                    ZCand.M(), EvWeight, NBin, MllLeft, MllRight);
      FillHist(param.Name+"/ScaleFactor/"+PtEtaCat1+"_ZMass_SS",                                    ZCand.M(), EvWeight, NBin, MllLeft, MllRight);
      FillHist(param.Name+"/ScaleFactor/"+PtEtaCat2+"_ZMass_SS",                                    ZCand.M(), EvWeight, NBin, MllLeft, MllRight);
      FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_ZMass_Mass"+MassRange+"_SS",                  ZCand.M(), EvWeight, NBin, MllLeft, MllRight);
      FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_LepPt_SS_Lep1Pt_Mass_"+MassRange+"_weighted", ElectronColl.at(0).Pt(),        EvWeight, 40, 0, 200);
      FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_LepPt_SS_Lep2Pt_Mass_"+MassRange+"_weighted", ElectronColl.at(1).Pt(),        EvWeight, 40, 0, 200);
    }
    if(ElectronColl.at(0).Charge()*ElectronColl.at(1).Charge()<0){
      FillHist(param.Name+"/ScaleFactor/BB_ZMass_OS", ZCand.M(), EvWeight, NBin, MllLeft, MllRight);
    } 
  
    Electron unshifted_el1 = ElectronColl.at(0);
    Electron unshifted_el2 = ElectronColl.at(1);


    for(unsigned int ishift = 0 ; ishift < 21 ; ishift++){

      Electron this_El1 = ElectronColl.at(0);
      Electron this_El2 = ElectronColl.at(1);

      double shiftEl1 = GetShiftCFEl(this_El1,param.Electron_Tight_ID,true,0); /// apply corr when measuring SF
      double shiftEl2 = GetShiftCFEl(this_El2,param.Electron_Tight_ID,true,0);

      vector<double> Shifts_CL = {0.995,0.99,0.985,0.98,0.975,0.97,0.965};
      vector<TString> Shifts_CL_Tag = {"_m1","_m2","_m3","_m4","_m5","_m6","_m7"};
      TString shiftstring = "";

      unsigned int nx = 2*(Shifts_CL.size() +1);
      
      if(ishift < 2) shiftstring = "PTB";
      else if(ishift < nx){
	for(unsigned int k = 0 ; k < Shifts_CL.size(); k++){
	  if(ishift == (2+ (2*k)))  {shiftstring = "PTB"+Shifts_CL_Tag.at(k);  shiftEl1 = shiftEl1 * Shifts_CL.at(k); }
	  if(ishift == (3+ (2*k)))  {shiftstring = "PTB"+Shifts_CL_Tag.at(k);  shiftEl2 = shiftEl2 * Shifts_CL.at(k); }
	}
      }
      else if(ishift==nx)      shiftstring = "CS_0p8";
      else if(ishift==(nx+1))  shiftstring = "CS_1";
      else if(ishift==(nx+2))  shiftstring = "CS_1p2";
      else if(ishift==(nx+3))  shiftstring = "CS_1p4";
      else if(ishift==(nx+4))  shiftstring = "CS_1p6";
      
      if(ishift == 0 || ishift == 2 || ishift == 4 || ishift == 6 || ishift == 8 || ishift == 10 || ishift == 12 || ishift == 14)	this_El1*= shiftEl1; 
      else if(ishift == 1 || ishift == 3 || ishift == 5 || ishift == 7 || ishift == 9 || ishift == 11 || ishift == 13 || ishift == 15)	this_El2*= shiftEl2; 

      else if(ishift==nx) {
	this_El1*=  0.992;	this_El2*=  0.992;
      }
      else if(ishift==(nx+1)) {
	this_El1*=  0.99;	this_El2*=  0.99;
      } 
      else if(ishift==(nx+2)) {
	this_El1*=  0.988;        this_El2*=  0.988;
      }
      else if(ishift==(nx+3)) {
        this_El1*=  0.986;        this_El2*=  0.986;
      }
      else if(ishift==(nx+4)) {
        this_El1*=  0.984;        this_El2*=  0.984;
      }

      if(_jentry>=5500 && _jentry<=6000) cout << "El 1 " << unshifted_el1.Pt () << " eta  = " << unshifted_el1.Eta() << " shifted pt = " << this_El1.Pt()<< endl;
      if(_jentry>=5500 && _jentry<=6000) cout << "El 2 " << unshifted_el2.Pt () << " eta  = " << unshifted_el2.Eta() <<   " shifted pt = " << this_El2.Pt()<< endl;
      
      Particle ZCand_shifted  = this_El1 + this_El2;
      Particle ZCand_unshifted = unshifted_el1 + unshifted_el2;

      TString CFKey = param.Electron_Tight_ID;
      if(CFKey =="passPOGTight") CFKey="POGTight";

      //GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1", "CFRate_InvPtEta3_PBSExtrap_Central_"   + CFKey
      double weight_Closure=1;         double weight_Closure_SF=1;
      double weight_ClosureNoS=1;      double weight_ClosureNoS_SF=1;

      double rate_cf1       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,  "InputBins_Version1", "CFRate_InvPtEta3_PBSExtrap_Central_" +CFKey,this_El1.defEta(), this_El1.Pt(), 0);
      double rate_cf2       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID, "InputBins_Version1", "CFRate_InvPtEta3_PBSExtrap_Central_" +CFKey, this_El2.defEta(), this_El2.Pt(),0);

      double rateNoS_cf1       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,  "InputBins_Version1", "CFRate_InvPtEta3_NoS_Central_" +CFKey, unshifted_el1.defEta(), unshifted_el1.Pt(), 0);
      double rateNoS_cf2       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,  "InputBins_Version1", "CFRate_InvPtEta3_NoS_Central_" +CFKey,unshifted_el2.defEta(), unshifted_el2.Pt(),0);

      double rateCS_cf1       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,  "InputBins_Version1", "CFRate_InvPtEta3_CS_Central_" +CFKey,this_El1.defEta(), this_El1.Pt(), 0);
      double rateCS_cf2       = cfEst->GetElectronCFRateFitted(param.Electron_Tight_ID,   "InputBins_Version1", "CFRate_InvPtEta3_CS_Central_" +CFKey,this_El2.defEta(), this_El2.Pt(),0);

      if(_jentry>=5500 && _jentry<=6000) cout << ishift << " rate_cf1 = " << rate_cf1 << " rate_cf2=" << rate_cf2 << endl;
      if(_jentry>=5500 && _jentry<=6000) cout << ishift << " rateNoS_cf1 = " << rateNoS_cf1 << " rateNoS_cf2 = " << rateNoS_cf2 << endl;


      if(ishift == 0 || ishift == 2 || ishift == 4 || ishift == 6 || ishift == 8 || ishift == 10 || ishift == 12 || ishift == 14)  {
	weight_Closure    = ReturnCFWeight(rate_cf1);	   weight_Closure_SF    = ReturnCFWeight(rate_cf1 * GetCFSF(param, Lepton(this_El1)));
	weight_ClosureNoS = ReturnCFWeight(rateNoS_cf1);   weight_ClosureNoS_SF = ReturnCFWeight(rateNoS_cf1 * GetCFSF(param, Lepton(this_El1)));
      }
    else if(ishift == 1 || ishift == 3 || ishift == 5 || ishift == 7 || ishift == 9 || ishift == 11 || ishift == 13 || ishift == 15)  {

	weight_Closure    = ReturnCFWeight(rate_cf2);      weight_Closure_SF    = ReturnCFWeight(rate_cf2 * GetCFSF(param, Lepton(this_El2)));
	weight_ClosureNoS = ReturnCFWeight(rateNoS_cf2);   weight_ClosureNoS_SF = ReturnCFWeight(rateNoS_cf2 * GetCFSF(param, Lepton(this_El2)));
      }
      else{

        weight_Closure      = ReturnCFWeight({rateCS_cf1,rateCS_cf2});
        weight_Closure_SF   = ReturnCFWeight({rateCS_cf1 * GetCFSF(param, Lepton(this_El1)),rateCS_cf2 * GetCFSF(param, Lepton(this_El2))});
	
	weight_ClosureNoS      = ReturnCFWeight({rateNoS_cf1,rateNoS_cf2});
        weight_ClosureNoS_SF   = ReturnCFWeight({rateNoS_cf1 * GetCFSF(param, Lepton(this_El1)),rateNoS_cf2 * GetCFSF(param, Lepton(this_El2))});

      }
      weight_Closure    *= EvWeight;
      weight_Closure_SF *= EvWeight;

      weight_ClosureNoS    *= EvWeight;
      weight_ClosureNoS_SF *= EvWeight;

      
      if(_jentry>=7000 && _jentry<=7500) cout << ishift << " weight_Closure = " << weight_Closure << " weight_shiftedSF = " << weight_Closure_SF << endl;
      if(_jentry>=7000 && _jentry<=7500) cout << ishift << " weight_ClosureNoS = " << weight_ClosureNoS << " weight_shiftedSF = " << weight_ClosureNoS_SF << endl;
      if(_jentry>=7000 && _jentry<=7500) cout << ishift << " weight_Closure = " << weight_Closure << " weight_shiftedSF = " << weight_Closure_SF << endl;
      if(_jentry>=7000 && _jentry<=7500) cout << ishift << " weight_ClosureNoS = " << weight_ClosureNoS << " weight_shiftedSF = " << weight_ClosureNoS_SF << endl;
      
      if(HasFlag("RunFakes")){
	//weight_shifted = GetFakeWeight(EL_Leptons, param);
	weight_Closure_SF = weight_Closure;
      }
      else       if(HasFlag("RunBkg")){
        weight_Closure = EvWeight;
	weight_Closure_SF = weight_Closure;
      }
      
      TString MassRangeShift = "LT60";
      if(ZCand_shifted.M() < 60) MassRangeShift ="LT60";
      else if(ZCand_shifted.M() < 85) MassRangeShift ="LT85";
      else if(ZCand_shifted.M() < 100) MassRangeShift ="LT100";
      else  MassRangeShift ="GT100";

      if(MllLeft<=ZCand_shifted.M()&&ZCand_shifted.M()<MllRight){
	if(ElectronColl.at(0).Charge()*ElectronColl.at(1).Charge()<0){
	  
	  FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_ZMass_OS_CF_"+shiftstring+"_weighted", ZCand_shifted.M(),           weight_Closure, NBin, MllLeft, MllRight);
	  FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_ZMass_OS_CF_"+shiftstring+"SF_weighted", ZCand_shifted.M(),         weight_Closure_SF, NBin, MllLeft, MllRight);
	  
	  FillHist(param.Name+"/ScaleFactor/"+PtEtaCat1+"_ZMass_OS_CF_"+shiftstring+"_weighted", ZCand_shifted.M(),           weight_Closure, NBin, MllLeft, MllRight);
          FillHist(param.Name+"/ScaleFactor/"+PtEtaCat1+"_ZMass_OS_CF_"+shiftstring+"SF_weighted", ZCand_shifted.M(),         weight_Closure_SF, NBin, MllLeft, MllRight);

          FillHist(param.Name+"/ScaleFactor/"+PtEtaCat2+"_ZMass_OS_CF_"+shiftstring+"_weighted", ZCand_shifted.M(),           weight_Closure, NBin, MllLeft, MllRight);
          FillHist(param.Name+"/ScaleFactor/"+PtEtaCat2+"_ZMass_OS_CF_"+shiftstring+"SF_weighted", ZCand_shifted.M(),         weight_Closure_SF, NBin, MllLeft, MllRight);

	  FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_ZMass_OS_CF_"+shiftstring+"_NoS_weighted", ZCand_shifted.M(),           weight_ClosureNoS, NBin, MllLeft, MllRight);
	  FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_ZMass_OS_CF_"+shiftstring+"SF_NoS_weighted", ZCand_shifted.M(),         weight_ClosureNoS_SF, NBin, MllLeft, MllRight);

	  FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_ZMass_OS_CF_"+shiftstring+"_Mass"+MassRangeShift+"_weighted", ZCand_shifted.M(),           weight_Closure, NBin, MllLeft, MllRight);
	  FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_ZMass_OS_CF_"+shiftstring+"SF_Mass"+MassRangeShift+"_weighted", ZCand_shifted.M(),         weight_Closure_SF, NBin, MllLeft, MllRight);
	  
	  FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_LepPt_OS_CF_"+shiftstring+"_Lep1Pt_Mass_"+MassRangeShift+"_weighted", this_El1.Pt(),         weight_Closure, 40, 0, 200);
	  FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_LepPt_OS_CF_"+shiftstring+"_Lep2Pt_Mass_"+MassRangeShift+"_weighted", this_El2.Pt(),         weight_Closure, 40, 0, 200);
      
	  if(ishift < 2){
	    
	    FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_ZMass_OS_CFweighted_unshifted", ZCand_unshifted.M(),         weight_Closure, NBin, MllLeft, MllRight);
	    FillHist(param.Name+"/ScaleFactor/"+EtaCat+"_ZMass_OS_CFweighted_unshiftedSF", ZCand_unshifted.M(),       weight_Closure_SF, NBin, MllLeft, MllRight);
	  }
        } // OS
      }// unsifted
    }// shift
  }
}

HNL_Lepton_ChargeFlip::HNL_Lepton_ChargeFlip(){

}

HNL_Lepton_ChargeFlip::~HNL_Lepton_ChargeFlip(){

}
