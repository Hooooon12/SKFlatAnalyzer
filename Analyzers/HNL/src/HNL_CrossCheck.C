#include "HNL_CrossCheck.h"

void HNL_CrossCheck::initializeAnalyzer(){

HNL_LeptonCore::initializeAnalyzer();
  SetupEventMVAReader("V2");

  nLog = 100000;
}


void HNL_CrossCheck::executeEvent(){


  if(!(_jentry%10000)) run_Debug=true;
  else run_Debug=false;

  ///// LIST IDs to run
  vector<TString> LepIDs = {"HNL_ULID"};//,"TopHN", "DefaultPOGTight"};
  if(strcmp(std::getenv("USER"),"jalmond")==0) LepIDs = {"HNTightV2"};//,"HNTightV2" ,"TopHN"};//, "POGTight"};
  if(RunFakeTF) LepIDs = {"HNL_ULID"};
  
  //// List Channels to run
  vector<HNL_LeptonCore::Channel> ChannelsToRun = {MuMu,EMu,EE};
  
  vector<TString> RegionsToPlot = {"SSMultiLep","Dilepton"}; 
  
  ///// Run command 

  vector<TString> CRToRun;
  if(HasFlag("Dilepton"))        CRToRun = {"OS_VR","SS_CR","VBF_CR","LLL_VR"};
  else if(HasFlag("SSMultiLep")) CRToRun = {"SS_CR","VBF_CR","LLL_VR"};
  else CRToRun = {"SS_CR"};

  for (auto id: LepIDs){
    for(auto channel : ChannelsToRun){
      if(channel != MuMu  && id =="TopHN") continue;

      AnalyzerParameter param_signal = HNL_LeptonCore::InitialiseHNLParameter(id,channel);
      if(channel == EMu) param_signal.CFMethod   = "MC";
      if(channel == EE)  param_signal.CFMethod   = "MC";

      for(auto iCR : CRToRun) RunControlRegions(param_signal , {iCR} );
    }
  }
  return;
}

void HNL_CrossCheck::RunControlRegions(AnalyzerParameter param, vector<TString> CRs){


  //  if(_jentry==0) param.PrintParameters();
  run_Debug = (_jentry%nLog==0);

  if(run_Debug) cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  
  Event ev = GetEvent();

  /// SetupWeight applies w_GenNorm=1., w_BR=1., w_PU  w_Pref  
  double weight =SetupWeight(ev,param);
  
  // HL ID                                                                                                                                                   
  std::vector<Electron>   ElectronVetoColl = GetElectrons(param.Electron_Veto_ID, 10.,  2.5);
  std::vector<Muon>       MuonVetoColl     = GetMuons    (param.Muon_Veto_ID,     5.,  2.4);

  /// IF ruunning fake then use FR_ID not Tight
  TString Electron_ID = SetLeptonID("Electron",param);
  TString Muon_ID     = SetLeptonID("Muon", param);

  double Min_Muon_Pt     = RunFake ? 5  : 10.;  //// Check 
  double Min_Electron_Pt = RunFake ? 10 : 15;

  std::vector<Muon>       MuonTightColl     = SelectMuons    ( param,Muon_ID,     Min_Muon_Pt,     2.4,weight); 
  std::vector<Electron>   ElectronTightColl = SelectElectrons( param,Electron_ID, Min_Electron_Pt, 2.5,weight);

  //// Change this so now Truth matching does not remove Leptons but in Definition code the GenFIlter removes events 
  //  std::vector<Muon>       MuonTightColl      =  GetLepCollByRunType    (MuonTightCollInit,    param);  
  //std::vector<Electron>   ElectronTightColl  =  GetLepCollByRunType    (ElectronTightCollInit,param);


  Particle METv = GetvMET("PuppiT1xyULCorr",param); // returns MET with systematic correction                                                                      

  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);
  
  EvalJetWeight(AK4_JetColl, AK4_VBF_JetColl,AK8_JetColl, weight, param);


  if(ElectronTightColl.size() == 2){
    if(!SameCharge(ElectronTightColl)){
      double pt1= (ElectronTightColl[0].Pt() > 2000) ? 2000. : ElectronTightColl[0].Pt();
      double pt2= (ElectronTightColl[1].Pt() > 2000) ? 2000. : ElectronTightColl[1].Pt();
      FillHist("OS/CFLep/Reco_Pt", pt1   ,  weight, 2000, 0, 2000 );
      FillHist("OS/CFLep/Reco_Pt", pt2   ,  weight, 2000, 0, 2000 );
    }
    else  {
      double pt1= (ElectronTightColl[0].Pt() > 2000) ? 2000. : ElectronTightColl[0].Pt();
      double pt2= (ElectronTightColl[1].Pt() > 2000) ? 2000. : ElectronTightColl[1].Pt();
      if(ElectronTightColl[0].LeptonIsCF())FillHist("SS/CFLep/Reco_Pt", pt1   ,  weight, 2000, 0, 2000 );
      if(ElectronTightColl[1].LeptonIsCF())FillHist("SS/CFLep/Reco_Pt", pt2   ,  weight, 2000, 0, 2000 );
    }
  }
  return;

  if(CRs.size() == 0) return;
  
  if(_jentry < 2000 ){
    if(RunFake){
      cout << "Running Fakes: Initial check for process name " + param.Name << endl;
      cout << "Muon ID = " << param.Muon_Tight_ID  << " run ID  = " << Muon_ID << endl;
      for(auto ilep : MuonTightColl)cout << "Muon isT=" << ilep.PassID(param.Muon_Tight_ID) << endl;
      
      cout << "Electron ID = " << param.Electron_Tight_ID  << " run ID  = " << Electron_ID << endl;
      for(auto ilep : ElectronTightColl)cout << "Electron isT=" << ilep.PassID(param.Electron_Tight_ID) << endl;
    }
  }


  vector<int> RunEl ;
  if(RunCF) RunEl =  {0,1} ;
  else RunEl = {-1};

  
  HNL_LeptonCore::Channel dilep_channel   =   GetChannelENum(param.Channel);

  if(!PassMETFilter()) return;

  std::vector<Lepton *> LepsT       = MakeLeptonPointerVector(MuonTightColl, ElectronTightColl,     param);
  std::vector<Lepton *> LepsV       = MakeLeptonPointerVector(MuonVetoColl,ElectronVetoColl,param);

  if(!PassHEMVeto(LepsV,weight)) return;  

  //  if (! (CheckLeptonFlavourForChannel(dilep_channel, LepsT))) return;
  //  if (!PassTriggerSelection(dilep_channel, ev, LepsT,param.TriggerSelection)) return;
  ///  EvalTrigWeight(dilep_channel, MuonTightColl, ElectronTightColl,param, ev, weight);

  param.Name = param.Name  + "/"+ GetChannelString(dilep_channel);

 
  //FillHighMassNPCRPlots(dilep_channel, LepsT, LepsV, AK4_JetColl, AK8_JetColl, AK4_BJetColl, ev, METv, param, weight)  ;
  bool RunNP=false;
  if(RunNP){
    if(LepsT.size() < 2) return ;    

    TString EvTag ="";
    Particle SumParticle;
    int nCh=0;
    for(auto ilep : LepsT) {
      if(ilep->Charge() >0) nCh++;
      
      SumParticle += *ilep;
    }
    if(ElectronTightColl.size() ==2 && MuonTightColl.size() ==0 ) EvTag ="SSEE";
    else if(ElectronTightColl.size() ==0 && MuonTightColl.size() ==2 ) EvTag ="SSMM";
    else if(ElectronTightColl.size() ==1 && MuonTightColl.size() ==1 ) EvTag ="SSEM";
    else if(ElectronTightColl.size() ==3 && MuonTightColl.size() ==0 ) EvTag ="EEE";
    else if(ElectronTightColl.size() ==0 && MuonTightColl.size() ==3 ) EvTag ="MMM";
    else if((ElectronTightColl.size() + MuonTightColl.size()) ==3 )  EvTag ="EML";
    else EvTag = "NULL";
    double LLMass =  SumParticle.M();

    bool ZWMass = (LLMass < 90 && LLMass > 75);
    for(auto ilep : LepsT){

      if(ilep->IsConv()){
	if(HasMEPhoton(*ilep)){

	  int Idx_Closest    = GenMatchedIdx(*ilep,All_Gens);
	  if(Idx_Closest > 0) FillHist(EvTag+"/ConvExt/"+ilep->GetFlavour()+"/Lep/Gen_Pt", All_Gens[Idx_Closest].Pt()   ,  weight, 200, 0, 200 );
	  FillHist(EvTag+"/ConvExt/"+ilep->GetFlavour()+"/Lep/Reco_Pt", ilep->Pt()   ,  weight, 200, 0, 200 );
	  FillHist(EvTag+"/ConvExt/"+ilep->GetFlavour()+"/Mass", LLMass   ,  weight, 200, 0, 200 );
	  
	  for(unsigned int i=2; i<All_Gens.size(); i++){
	    Gen gen = All_Gens.at(i);
	    if(Idx_Closest < 0 && ilep->DeltaR(gen) < 0.2 ) {
	      if(gen.PID() == 22 && gen.Status()==1) {
		FillHist(EvTag+"/ConvExt/"+ilep->GetFlavour()+"/Ph/Gen_Mass", gen.M()   ,  weight, 200, 0, 200 );
		FillHist(EvTag+"/ConvExt/"+ilep->GetFlavour()+"/Ph/Gen_Pt", gen.Pt()   ,  weight, 100, 0, 500 );
		if(ZWMass){
		  FillHist(EvTag+"/ConvExtZ/"+ilep->GetFlavour()+"/Ph/Gen_Mass", gen.M()   ,  weight, 200, 0, 200 );
		  FillHist(EvTag+"/ConvExtZ/"+ilep->GetFlavour()+"/Ph/Gen_Pt", gen.Pt()   ,  weight, 100, 0, 500 );
		}
	      }
	    }
	  }
	}
	else if(ilep->LeptonGenType() < 0){
	  int Idx_Closest    = GenMatchedIdx(*ilep,All_Gens);
	  
          if(Idx_Closest > 0) FillHist(EvTag+"/ConvExt2/"+ilep->GetFlavour()+"/Lep/Gen_Pt", All_Gens[Idx_Closest].Pt()   ,  weight, 200, 0, 200 );
          FillHist(EvTag+"/ConvExt2/"+ilep->GetFlavour()+"/Lep/Reco_Pt", ilep->Pt()   ,  weight, 200, 0, 200 );
          FillHist(EvTag+"/ConvExt2/"+ilep->GetFlavour()+"/Mass", LLMass   ,  weight, 200, 0, 200 );

	  for(unsigned int i=2; i<All_Gens.size(); i++){
            Gen gen = All_Gens.at(i);
            if(Idx_Closest < 0 && ilep->DeltaR(gen) < 0.2 ) {
              if(gen.PID() == 22 && gen.Status()==1) {
		FillHist(EvTag+"/ConvExt2/"+ilep->GetFlavour()+"/Ph/Gen_Mass", gen.M()   ,  weight, 200, 0, 200 );
                FillHist(EvTag+"/ConvExt2/"+ilep->GetFlavour()+"/Ph/Gen_Pt", gen.Pt()   ,  weight, 100, 0, 500 );
              }
            }
          }
	}
	else{
	  //cout << "Internal Conv" << endl;
	  unsigned int Idx_Closest    = GenMatchedIdx(*ilep,All_Gens);
	  //cout << "Idx_Closest = " << Idx_Closest << endl;
	  Gen Mother = GenGetMother(All_Gens[Idx_Closest]);
	  int nLep(0);
	  Particle  Gamma = All_Gens[Idx_Closest];
          for(unsigned int i=2; i<All_Gens.size(); i++){
	    if(i == Idx_Closest) continue;
	    if(All_Gens.at(i).Status() != 1) continue;
	    if(MCSample.Contains("MiNNL")){
	      if(nLep > 1 && GenGetMother(All_Gens.at(i)).Index() == Mother.Index()) Gamma = Gamma + All_Gens.at(i);
	      nLep++;
	    }
	    else   if(GenGetMother(All_Gens.at(i)).Index() == Mother.Index()) Gamma = Gamma + All_Gens.at(i);
	  }

          if(Idx_Closest > 0) FillHist(EvTag+"/ConvInt/"+ilep->GetFlavour()+"/Lep/Gen_Pt", All_Gens[Idx_Closest].Pt()   ,  weight, 200  , 0, 200 );
          FillHist(EvTag+"/ConvInt/"+ilep->GetFlavour()+"/Lep/Reco_Pt", ilep->Pt()   ,  weight, 200, 0, 200 );
	  FillHist(EvTag+"/ConvInt/"+ilep->GetFlavour()+"/Mass", LLMass   ,  weight, 200, 0, 200 );

	  FillHist(EvTag+"/ConvInt/"+ilep->GetFlavour()+"/Ph/Gen_Mass", Gamma.M()   ,  weight, 200, 0, 200 );
	  FillHist(EvTag+"/ConvInt/"+ilep->GetFlavour()+"/Ph/Gen_Pt", Gamma.Pt()   ,  weight, 100, 0, 500 );
	  if(ZWMass){
	    FillHist(EvTag+"/ConvIntZ/"+ilep->GetFlavour()+"/Ph/Gen_Mass", Gamma.M()   ,  weight, 200, 0, 200 );
	    FillHist(EvTag+"/ConvIntZ/"+ilep->GetFlavour()+"/Ph/Gen_Pt", Gamma.Pt()   ,  weight, 100, 0, 500 );
	  }
	}
      }
    }
    
    return;
    Particle ll =  (*LepsT[0]) + (*LepsT[1]);
    if(LepsT.size() ==2 && ll.M() < M_CUT_NonSR3_LL) return ;
    
    //    if(! ( (LepsT[0]->IsPrompt() && LepsT[1]->IsPrompt() )  || ( LepsT[0]->LeptonIsCF() || LepsT[1]->LeptonIsCF()  ))) return;
    //    if( ! ( HasMEPhoton(*LepsT[0]) && LepsT[0]->LeptonGenType() > 0) || ( HasMEPhoton(*LepsT[1]) && LepsT[1]->LeptonGenType() > 0) || ( !HasMEPhoton(*LepsT[0]) && LepsT[0]->LeptonGenType() < 0) || ( !HasMEPhoton(*LepsT[1]) && LepsT[1]->LeptonGenType() < 0) ) return;
    
    if(CheckLeptonFlavourForChannel(dilep_channel,LepsT ) && SameCharge(LepsT)){
      cout << " " << endl;
      cout << " " << endl;
      cout << " " << endl;
      cout << " " << endl;
      cout << " " << endl;
      cout << " " << endl;
      cout << " " << endl;
      cout << " " << endl;
      cout << "SS LL " << ll.M() << endl;
      int Idx_Closest1    = GenMatchedIdx(*LepsT[0],All_Gens);
      int Idx_Closest2    = GenMatchedIdx(*LepsT[1],All_Gens);
      if(Idx_Closest1 > 0 && Idx_Closest2 > 0){
	cout << "Lep 1 matched lep isPrompt = " << GenIsPrompt(All_Gens[Idx_Closest1]) << " Matched PID = " << All_Gens[Idx_Closest1].PID()  << " Type= " << LepsT[0]->LeptonGenType() << endl;
	cout << "Lep 2 matched lep isPrompt = " << GenIsPrompt(All_Gens[Idx_Closest2]) << " Matched PID = " << All_Gens[Idx_Closest1].PID()  << " Type= " << LepsT[1]->LeptonGenType() << endl;
	cout << "Lep 1 HasMEPhoton = " << HasMEPhoton(*LepsT[0]) << endl;
	cout << "Lep 2 HasMEPhoton = " << HasMEPhoton(*LepsT[1]) << endl;
        PrintGen(All_Gens);
        for(auto i : LepsT)   PrintMatchedGen( All_Gens,*i);
	for(auto i : LepsT)   cout << i->GetFlavour() << " "  << i->PrintInfo() <<  endl;
	for(auto i : LepsT)   cout << "Is CF = " << i->LeptonIsCF() << endl;
	for(auto i : LepsT)   cout << "Is Conv = " << i->IsConv() << endl;
	for(auto i : LepsT)   cout << i->GetFlavour() << " "  << i->PrintInfo() << " LeptonIsPromptConv=" << i->LeptonIsPromptConv() <<  endl;

      }
      else {
        cout << "------------------------------------------------------------" << endl;
	cout << " " << endl;
	cout << " " << endl;
	cout << " " << endl;
	cout << " " << endl;
	cout << " " << endl;
	cout << " " << endl;
	cout << " " << endl;
	cout << "SS EMu No Match" << ll.M() << endl;
	cout << "Lep 1 matched lep " << LepsT[0]->LeptonGenType() << endl;
	cout << "Lep 2 matched lep " << LepsT[1]->LeptonGenType() << endl;
	cout << "Lep 1 HasMEPhoton = " << HasMEPhoton(*LepsT[0]) << endl;
	cout << "Lep 2 HasMEPhoton = " << HasMEPhoton(*LepsT[1]) << endl;
	cout << "Idx_Closest1 = " << Idx_Closest1 << " Idx_Closest2 = " << Idx_Closest2 << endl;

	PrintGen(All_Gens);
	
        for(auto i : LepsT)   cout << i->GetFlavour() << " "  << i->PrintInfo() <<  endl;
        for(auto i : LepsT)   cout << "Is CF = " << i->LeptonIsCF() << endl;
        for(auto i : LepsT)   cout << i->GetFlavour() << " "  << i->PrintInfo() << " LeptonIsPromptConv=" << i->LeptonIsPromptConv() <<  endl;

	
	for(auto i : LepsT)PrintMatchedGen( All_Gens,*i);
	
	for(unsigned int i=2; i<All_Gens.size(); i++){
	  Gen gen = All_Gens.at(i);
	  if(Idx_Closest1 < 0 && LepsT[0]->DeltaR(gen) < 0.2) {
	    if(gen.PID() == 22) {
	      cout << "Matched Ph ["<< i <<"] GenIsPrompt = " << GenIsPrompt(gen) << endl;
	    }
	  }
          if(Idx_Closest2 < 0 && LepsT[1]->DeltaR(gen) < 0.2) {
            if(gen.PID() == 22) {
              cout << "Matched Ph ["<< i <<"] GenIsPrompt = " << GenIsPrompt(gen) << endl;
            }
          }

	}
	

	cout << "------------------------------------------------------------" << endl;

      }
      
    }     

    return;

    bool HasConv=false;
    int ConvLepType=0;
    int nPrompt =0;
    bool GENTMatched=false;
    int nFake =0;

    for(auto ilep : LepsT){
      if(ilep->LeptonGenType() == 0)  nFake++;
      if(ilep->LeptonGenType() == -1)  nFake++;
      if(ilep->LeptonGenType() == -2)  nFake++;
      if(ilep->LeptonGenType() == -3)  nFake++;
      if(ilep->LeptonGenType() == 1) nPrompt++;
      if(ilep->LeptonGenType() == 2) nPrompt++;
      if(ilep->LeptonGenType() == 3) nPrompt++;
      if(ilep->LeptonGenType() > 3) {HasConv = true; ConvLepType = ilep->LeptonGenType();}
      if(ilep->LeptonGenType() < -4) {HasConv = true;  ConvLepType = ilep->LeptonGenType();}


      for(unsigned int i=2; i<All_Gens.size(); i++){
	Gen gen = All_Gens.at(i);
	if(ilep->DeltaR(gen) < 0.2) {
	  if(gen.PID() == 22 && gen.isPromptFinalState() && gen.Pt()> 15.) {
	    for(unsigned int j=2; j<All_Gens.size(); j++){
	      if(!(fabs(All_Gens.at(j).PID()) <7 || fabs(All_Gens.at(j).PID()) == 21)) continue;
	      if(All_Gens.at(j).Status() != 23) continue;
	      if(All_Gens.at(j).DeltaR(gen) > 0.05)GENTMatched=true;
	    }
	  }
	}
      }
    }

    if(GENTMatched){
      cout << " " << endl;
      if(LepsT.size()==2)cout << "2l GENT MATCH" << endl;
      if(LepsT.size()==3)cout << "3l GENT MATCH" << endl;
      for(auto i : LepsT)   cout << i->GetFlavour() << " "  << i->PrintInfo() << endl;
      PrintGen(All_Gens);
      for(auto i : LepsT)PrintMatchedGen( All_Gens,*i);
    }
    else {
      cout << " " << endl;
      if(LepsT.size()==2)cout << "2l NOGENT MATCH" << endl;
      if(LepsT.size()==3)cout << "3l NOGENT MATCH" << endl;
      for(auto i : LepsT)   cout << i->GetFlavour() << " "  << i->PrintInfo() << endl;
      PrintGen(All_Gens);
      for(auto i : LepsT)PrintMatchedGen( All_Gens,*i);
    }
    return;
    
    if(nPrompt == 3 || (nPrompt == 2 && SameCharge(LepsT))) {
      if(LepsT.size()==2)cout << "2l All Prompt" << endl;
      if(LepsT.size()==3)cout << "3l All Prompt" << endl;
   
      cout << "ll.M()  =  " << ll.M() << endl;
      for(auto i : LepsT)   cout << "Is CF = " << i->LeptonIsCF() << endl;
      for(auto i : LepsT)   cout << i->GetFlavour() << " "  << i->PrintInfo() << " LeptonIsPromptConv=" << i->LeptonIsPromptConv() <<  endl;
      PrintGen(All_Gens);
      for(auto i : LepsT)PrintMatchedGen( All_Gens,*i);
    }

    if(_jentry < 1000) {
      if(LepsT.size() == 3 && CheckLeptonFlavourForChannel(GetTriLeptonChannel(dilep_channel),LepsT )){
	if(!HasConv){
	  cout << "3l no conv " << endl;
	  for(auto i : LepsT)   cout << i->GetFlavour() << " "  << i->PrintInfo() << endl;
	  PrintGen(All_Gens);
	  for(auto i : LepsT)PrintMatchedGen( All_Gens,*i);
	}
	else{
	  cout << "3l has conv " << endl;
	  for(auto i : LepsT)   cout << i->GetFlavour() << " "  << i->PrintInfo() << endl;
	  PrintGen(All_Gens);
	  for(auto i : LepsT)PrintMatchedGen( All_Gens,*i);
	  
	}
      }
      
      if(SameCharge(LepsT) && CheckLeptonFlavourForChannel(dilep_channel, LepsT)){
	if(!HasConv){
	  cout << "2l no conv " << endl;
	  for(auto i : LepsT)   cout << i->GetFlavour() << " "  << i->PrintInfo() << endl;
	  PrintGen(All_Gens);
	  for(auto i : LepsT)PrintMatchedGen( All_Gens,*i);
	}
	else{
	  cout << "2l conv " << endl;
	  if(GetLLMass(LepsT) < 80  || GetLLMass(LepsT) > 100) return;
	  for(auto i : LepsT)   cout << i->GetFlavour() << " "  << i->PrintInfo() << endl;
	  PrintGen(All_Gens);
	  for(auto i : LepsT)PrintMatchedGen( All_Gens,*i);
	  
	}
      }
    }
    

    if(LepsT.size() == 3){
      AnalyzerParameter paramTrilep  = param;
      paramTrilep.Channel  = GetChannelString(GetTriLeptonChannel(dilep_channel));

      if(CheckLeptonFlavourForChannel(GetTriLeptonChannel(dilep_channel),LepsT )){
	Fill_RegionPlots(paramTrilep,"HNL_SS_ThreeLep"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(nFake==0)         Fill_RegionPlots(paramTrilep,"HNL_NonFake_ThreeLep"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(nPrompt==3) Fill_RegionPlots(paramTrilep,"HNL_Prompt_ThreeLepton_CR"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(HasConv)         Fill_RegionPlots(paramTrilep,"HNL_Conv_ThreeLep"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	
	if(ConvLepType == 6)        Fill_RegionPlots(paramTrilep,"HNL_SS_ThreeLep_6"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(ConvLepType == 5)        Fill_RegionPlots(paramTrilep,"HNL_SS_ThreeLep_5"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(ConvLepType == 4)        Fill_RegionPlots(paramTrilep,"HNL_SS_ThreeLep_4"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(ConvLepType == -4)        Fill_RegionPlots(paramTrilep,"HNL_SS_ThreeLep_M4"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(ConvLepType == -5)        Fill_RegionPlots(paramTrilep,"HNL_SS_ThreeLep_M5"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(ConvLepType == -6)        Fill_RegionPlots(paramTrilep,"HNL_SS_ThreeLep_M6"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
      }
    }

    if(LepsT.size() != 2) return ;
    if(!CheckLeptonFlavourForChannel(dilep_channel,LepsT )) return ;
    if(SameCharge(LepsT)) {
      if(nPrompt==2) Fill_RegionPlots(param,"HNL_SS_Prompt_TwoLepton_CR"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
      if(nFake==0)         Fill_RegionPlots(param,"HNL_NonFake_Dilep"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);

      if(HasConv) {
	if(LepsT[0]->LeptonGenType() == 4 || LepsT[1]->LeptonGenType() == 4) Fill_RegionPlots(param,"HNL_SS_ConvP4_TwoLepton_CR"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(LepsT[0]->LeptonGenType() == 5 || LepsT[1]->LeptonGenType() == 5) Fill_RegionPlots(param,"HNL_SS_ConvP5_TwoLepton_CR"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(LepsT[0]->LeptonGenType() == 6 || LepsT[1]->LeptonGenType() == 6) Fill_RegionPlots(param,"HNL_SS_ConvP6_TwoLepton_CR"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(LepsT[0]->LeptonGenType() == -4 || LepsT[1]->LeptonGenType() == -4) Fill_RegionPlots(param,"HNL_SS_ConvM4_TwoLepton_CR"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(LepsT[0]->LeptonGenType() == -5 || LepsT[1]->LeptonGenType() == -5) Fill_RegionPlots(param,"HNL_SS_ConvM5_TwoLepton_CR"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
	if(LepsT[0]->LeptonGenType() == -6 || LepsT[1]->LeptonGenType() == -6) Fill_RegionPlots(param,"HNL_SS_ConvM6_TwoLepton_CR"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);										     
	Fill_RegionPlots(param,"HNL_Conv_HighMassNP_TwoLepton_CR"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
      }
      Fill_RegionPlots(param,"HNL_SS_HighMassNP_TwoLepton_CR"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
    }
    else Fill_RegionPlots(param,"HNLS_OS_HighMassNP_TwoLepton_CR"  ,  AK4_JetColl, AK8_JetColl ,LepsT,  METv, nPV, weight);
    
  }

}




HNL_CrossCheck::HNL_CrossCheck(){

  SetupEvMVA();
}
 
HNL_CrossCheck::~HNL_CrossCheck(){

  DeleteEvMVA();
}
