#include "HNL_GenericObject_Studies.h"

void HNL_GenericObject_Studies::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();
  
  // Overwite JECSources in AnalyserCore
  JECSources = {"AbsoluteStat"};
  
  for(auto jec_source : JECSources){
    SetupJECUncertainty(jec_source);
  }
  
  //SetupIDMVAReader(false);
  // SetupIDMVAReader(true);

}


void HNL_GenericObject_Studies::executeEvent(){

  //==== Gen for genmatching
  AnalyzerParameter param  = InitialiseHNLParameter("SignalStudy");
  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);

  FillHist ("NoCut", 1, weight, 2, 0., 2.,"");
  FillHist ("nPV" , nPV, weight, 100., 0., 100.,"");
  TString process="";
  if(!IsData){
    
    process = GetProcess();

    vector<TString> labels ={"Inclusive","OS_ElEl", "SS_El-El-", "SS_El+El+","OS_MuMu", "SS_Mu-Mu-" , "SS_Mu+Mu+","OS_ElMu","SS_El-Mu-" ,"SS_El+Mu+", "OS_MuEl","SS_Mu-El-","SS_Mu+El+"};

    HNL_LeptonCore::FillCutflow("SignalProcess","SplitChannel",weight, labels, process);
    HNL_LeptonCore::FillCutflow("SignalProcess","SplitChannel",weight, labels, "Inclusive");


    if(_jentry% 10000==0){
      cout << "process = " << process << endl;
      
      PrintGen(All_Gens);
    }
  }
  if(process.Contains("OS")) return;


  FillHist ("nPV_"+process , nPV, weight, 100., 0., 100.,"");

  vector<HNL_LeptonCore::Channel> channels = {EE,MuMu};

  for(auto dilep_channel : channels){

    if(MCSample.Contains("Type")){
      if (!SelectChannel(dilep_channel)) continue;
    }

    TString channel = GetChannelString(dilep_channel) ;

    //double ptbins[11] = { 0., 10.,15., 20., 30., 40.,50., 100.,500. ,1000., 2000.};

    std::vector<Jet>      jetsTmp     = GetJets   ( "NoID",    10., 5.);
    //for(auto ijet : jetsTmp ) ijet.PrintObject("Jet ");

    FillHist ("NoCut_"+channel, 1, weight, 2, 0., 2.,"");
    
    if(MCSample.Contains("DYType") && MCSample.Contains("private"))  MakeType1SignalPlots(channel, false);
    if(MCSample.Contains("VBFType") && MCSample.Contains("private"))  MakeType1VBFSignalPlots(channel, false);
    if(MCSample.Contains("SSWWType") && MCSample.Contains("private"))  MakeType1SSWWSignalPlots(channel, false);
    
    vector<Gen> gen_lep= GetGenLepronsSignal();
        
    std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, 10., 2.5);
    std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID, 5., 2.4);

  

    FillHist ("NVetoMuon_"+channel, MuonCollV.size(), weight, 4, 0., 4.,"");
    FillHist ("NVetoElectron_"+channel, ElectronCollV.size(), weight, 4, 0., 4.,"");
    
    std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(MuonCollV,ElectronCollV);

    //cout << "param.Electron_Veto_ID = " << param.Electron_Veto_ID << endl;

    if(leps_veto.size()==2)    FillHist ("DiLepton_Veto_"+channel, 1, weight, 2, 0., 2.,"");


    std::vector<Electron>   ElectronCollAll = GetElectrons( "MVAID", 10., 2.5);
    std::vector<Muon>       MuonCollAll     = GetMuons    (  "MVAID", 10., 2.4);

    for(auto ilep : ElectronCollAll){
      double a = ilep.HNL_MVA_CF("v2") ;
      double b = GetBDTScoreElV1(ilep,AnalyzerCore::CF,  "BDTGv1");
      if(a != b)  cout << "v1 = " << a  << " new v1 = " << b  << " eta = " << ilep.Eta() << endl;
    }
    
    //for(auto iel : ElectronCollAll){
    //  cout << "Electron : " << endl;
    //  iel.PrintMVAMap() ;
    //}
    

    std::vector<Lepton *> LepsMVAID  = (dilep_channel==EE) ? MakeLeptonPointerVector(ElectronCollAll) : MakeLeptonPointerVector(MuonCollAll);

    if(LepsMVAID.size() == 2)     FillHist ("DiLepton_MVA_"+channel, 1, weight, 2, 0., 2.,"");
    
    std::vector<Electron>   ElectronColl;
    std::vector<Muon>       MuonColl;

   
    if(MCSample.Contains("Type")){
      
      for(auto iel: ElectronCollAll){ 
	bool matched_lep=false;
	for(auto igen : gen_lep){
	  if(iel.DeltaR(igen) < 0.2) matched_lep=true;
	}
	if(matched_lep) ElectronColl.push_back(iel);
	
      }
      
      for(auto iel: MuonCollAll){
	bool matched_lep=false;
	for(auto igen :gen_lep){
	  if(iel.DeltaR(igen) <0.2) matched_lep=true;
	}
	if(matched_lep)MuonColl.push_back(iel);
      } 
      
    }
    else{

      //for(auto iel: ElectronCollAll)if(iel.PassID("HNTightV3")) ElectronColl.push_back(iel);
      //for(auto iel: MuonCollAll) if(iel.PassID("HNL_ULID_"+GetYearString())) MuonColl.push_back(iel);

      for(auto iel: ElectronCollAll) ElectronColl.push_back(iel);                                                                                                                    
      for(auto iel: MuonCollAll) MuonColl.push_back(iel);                                                                                                           
    }


    if(dilep_channel==EE) MuonColl.clear();
    else ElectronColl.clear();

    std::vector<Electron>   ElectronCollFake;
    std::vector<Muon>       MuonCollFake;
    std::vector<Electron>   ElectronCollConv;
    std::vector<Electron>   ElectronCollCF;
    std::vector<Muon>       MuonCollConv;
    std::vector<Electron>   ElectronCollPrompt;
    std::vector<Muon>       MuonCollPrompt;

    if(HasFlag("LeptonTypes")){
      // This function splits leptons into Fake/Conv/CF/Prompt and plots ALl plots splitting pt/eta

      if(!IsData){
	if (!MCSample.Contains("Type")){
	  
	  ElectronCollFake = SkimLepColl(ElectronColl, param, "HFake");
	  ElectronCollConv = SkimLepColl(ElectronColl, param, "NHConv");
	  ElectronCollCF = SkimLepColl(ElectronColl, param, "CF");
	  ElectronCollPrompt = SkimLepColl(ElectronColl, param, "PromptNoCF");
	  
	  FillAllElectronPlots(param,"Prompt_Electrons"  , ElectronCollPrompt , weight);
	  FillAllElectronPlots(param,"Fake_Electrons"  , ElectronCollFake , weight);
	  FillAllElectronPlots(param,"Conv_Electrons"  , ElectronCollConv , weight);
	  FillAllElectronPlots(param,"CF_Electrons"  , ElectronCollCF , weight);
	  
	  MuonCollFake = SkimLepColl(MuonColl, param, "HFake");
	  MuonCollConv = SkimLepColl(MuonColl, param, "NHConv");
	  MuonCollPrompt = SkimLepColl(MuonColl, param, "Prompt");
	  FillMuonCollPlots(param,"Prompt_Muons"  , MuonCollPrompt , weight);
	  FillMuonCollPlots(param,"Fake_Muons"  , MuonCollFake , weight);
	  FillMuonCollPlots(param,"Conv_Muons"  , MuonCollConv , weight);
	  
	}
	else{
	  FillMuonCollPlots(param,"Signal_Muons"  , MuonColl , weight);
	  FillAllElectronPlots(param,"Signal_Electrons"  , ElectronColl , weight);
	}      
      }
      continue;
    }

    return;


    std::vector<Lepton *> LepsAll  = (dilep_channel==EE) ? MakeLeptonPointerVector(ElectronColl) : MakeLeptonPointerVector(MuonColl);

    /// Collect All jets 
    std::vector<Jet>    AllAK4Jets       = SelectJets   ( param, param.Jet_ID, 10., 5.);

    std::vector<FatJet> AllAK8Jets       = SelectFatJets(param, param.FatJet_ID, 200., 5.);
    std::vector<FatJet> AK8_JetColl      = SelectAK8Jets(AllAK8Jets, 200., 5., true,  1., false, -999, false, 0., 20000.,"", ElectronCollV, MuonCollV);
    std::vector<Jet>    JetCollLoose     = SelectAK4Jets(AllAK4Jets,     15., 4.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, AK8_JetColl);

    TString PUIDWP="";
    std::vector<Jet> JetColl             = SelectAK4Jets(AllAK4Jets,     20., 2.7, true,  0.4,0.8, PUIDWP,   ElectronCollV,MuonCollV, AK8_JetColl);
    std::vector<Jet> VBF_JetColl         = SelectAK4Jets(AllAK4Jets,     30., 4.7, true,  0.4,0.8, PUIDWP,  ElectronCollV,MuonCollV, AK8_JetColl);
    std::vector<Jet> B_JetColl           = SelectAK4Jets(AllAK4Jets,     20., 2.4, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, AK8_JetColl);
    JetTagging::Parameters param_jets = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets);


    // Get BJets  and EV weight to corr BTag Eff                                                                                                                                                                                                                       
    std::vector<Jet> BJetColl    = SelectBJets(param, BJetColl, param_jets);
    double sf_btag               = GetBJetSF(param, BJetColl, param_jets);
    weight*= sf_btag;


    if(HasFlag("SR")){
      
      if (PassTriggerSelection(dilep_channel, ev, LepsAll,"Full",false)) {
	if(!PassMETFilter()) continue;
      }

      FillHist ("Trigger_"+GetChannelString(dilep_channel), 1, weight, 2, 0., 2.,"");

      FillHist ("LowMassCut_"+GetChannelString(dilep_channel), 1, weight, 2, 0., 2.,"");
    }
    
    if(HasFlag("FakeLepton")){
    
      vector<Muon> MuonTopColl;
      vector<Muon> MuonHNLColl;
      vector<Muon> MuonV2Coll;

      int nFakeMu(0);
      for(auto imu: MuonColl) {
	
	// Fill vector with tighter IDs
	if(imu.PassID("HNL_TopMVA_MM")) MuonTopColl.push_back(imu);                                                                     
	if(imu.PassID("HNL_ULID_"+GetYearString())) MuonHNLColl.push_back(imu);                                                                     
	if(imu.PassID("HNTightV2")) MuonV2Coll.push_back(imu);
	if(imu.IsFake()) nFakeMu++;

      }
      

      if (nFakeMu==0 && dilep_channel == MuMu) continue;
      
      for(auto imu: MuonColl){

	int MulepType = imu.LeptonGenType();
	FillHist("LepType/AllMuon", MulepType,   weight,12, -6., 6.);
	
	TString JetFlavour = imu.CloseJet_Flavour();

	FillHist("JetType/AllMuon", imu.CloseJet_FlavourInt(),   weight,6, 0., 6.);

        if(imu.IsFake()){

	  if (imu.Pt() < 500.)  FillHist("FakeMuon/Pt", imu.Pt(),   weight,100, 0., 500.);
	  else  FillHist("FakeMuon/Pt", 499.,   weight,100, 0., 500.);


	  if(JetFlavour == "LF")  {
            if (imu.Pt() < 500.)  FillHist("FakeMuon/LF_Pt", imu.Pt(),   weight,100, 0., 500.);
            else FillHist("FakeMuon/LF_Pt", 499.,   weight,100, 0., 500.);
          }
          if(JetFlavour == "HF")  {
            if (imu.Pt() < 500.)  FillHist("FakeMuon/HF_Pt", imu.Pt(),   weight,100, 0., 500.);
            else FillHist("FakeMuon/HF_Pt", 499.,   weight,100, 0., 500.);
          }


	  FillHist("JetType/FakeMuon", imu.CloseJet_FlavourInt(),   weight,6, 0., 6.);
	  FillHist("TypeVsFlavour/FakeMuon", MulepType,   weight,10, -5., 5.);
          if(JetFlavour == "LF")       FillHist("TypeVsFlavour/FakeMuonLF", MulepType,   weight,10, -5., 5.);
          else  if(JetFlavour == "HF") FillHist("TypeVsFlavour/FakeMuonHF", MulepType,   weight,10, -5., 5.);

          if(SameCharge(MuonColl)) {

	    if (imu.Pt() < 500.)  FillHist("FakeMuon/SSPt", imu.Pt(),   weight,100, 0., 500.);
	    else FillHist("FakeMuon/SSPt", 499.,   weight,100, 0., 500.);

	    if(JetFlavour == "LF")  {
	      if (imu.Pt() < 500.)  FillHist("FakeMuon/SSLF_Pt", imu.Pt(),   weight,100, 0., 500.);
	      else FillHist("FakeMuon/SSLF_Pt", 499.,   weight,100, 0., 500.);
	    }
	    if(JetFlavour == "HF")  {
	      if (imu.Pt() < 500.)  FillHist("FakeMuon/SSHF_Pt", imu.Pt(),   weight,100, 0., 500.);
	      else FillHist("FakeMuon/SSHF_Pt", 499.,   weight,100, 0., 500.);
	    }



	    FillHist("TypeVsFlavour/AllSSFakeMuon", MulepType,   weight,10, -5., 5.);
            if(JetFlavour == "LF")       FillHist("TypeVsFlavour/SSFakeMuonLF", MulepType,   weight,10, -5., 5.);
            else  if(JetFlavour == "HF") FillHist("TypeVsFlavour/SSFakeMuonHF", MulepType,   weight,10, -5., 5.);
	  }
	}
      } /// Loop over MVA Loose Muon coll
      
      
      if (SameCharge(MuonColl))  {

	FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 0, weight, 5, 0., 5.,"");
	if (SameCharge(MuonTopColl))  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 1, weight, 5, 0., 5.,"");
	if (SameCharge(MuonHNLColl))  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 2, weight, 5, 0., 5.,"");
	if (SameCharge(MuonV2Coll)) FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 3, weight, 5, 0., 5.,"");
	
	TString Jet1Flavour = MuonColl[0].CloseJet_Flavour();
	TString Jet2Flavour = MuonColl[1].CloseJet_Flavour();
	
	if(MuonColl[0].IsFake() && MuonColl[1].IsPrompt()){
	  
	  FillHist("TypeVsFlavour/Muon", MuonColl[0].LeptonGenType(),   weight, 10, -5., 5.);
	  if(Jet1Flavour == "LF")      FillHist("TypeVsFlavour/MuonLF", MuonColl[0].LeptonGenType(),   weight, 10, -5., 5.);
	  else if(Jet1Flavour == "HF") FillHist("TypeVsFlavour/MuonHF", MuonColl[0].LeptonGenType(),   weight,10, -5., 5.);
	  
	  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel)+"_"+Jet1Flavour+"_Fake", 0, weight, 5, 0., 5.,"");
	  if (SameCharge(MuonTopColl))  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel)+"_"+Jet1Flavour+"_Fake", 1, weight, 5, 0., 5.,"");
	  if (SameCharge(MuonHNLColl))  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel)+"_"+Jet1Flavour+"_Fake", 2, weight, 5, 0., 5.,"");
	  if (SameCharge(MuonV2Coll)) FillHist ("SameSignDilep_"+GetChannelString(dilep_channel)+"_"+Jet1Flavour+"_Fake", 3, weight, 5, 0., 5.,"");
	}
	if(MuonColl[1].IsFake() && MuonColl[0].IsPrompt()){

	  FillHist("TypeVsFlavour/Muon", MuonColl[1].LeptonGenType(),   weight,10, -5., 5.);
          if(Jet2Flavour == "LF")      FillHist("TypeVsFlavour/MuonLF", MuonColl[1].LeptonGenType(),   weight,10, -5., 5.);
          else if(Jet2Flavour == "HF") FillHist("TypeVsFlavour/MuonHF", MuonColl[1].LeptonGenType(),   weight,10, -5., 5.);
	  
	  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 0, weight, 5, 0., 5.,"");
	  if (SameCharge(MuonTopColl))  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 1, weight, 5, 0., 5.,"");
	  if (SameCharge(MuonHNLColl))  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 2, weight, 5, 0., 5.,"");
	  if (SameCharge(MuonV2Coll)) FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 3, weight, 5, 0., 5.,"");
	}
	
        if(MuonColl[0].IsFake() && MuonColl[1].IsFake()){

	  FillHist("TypeVsFlavour/MuonFF", MuonColl[0].LeptonGenType(),   weight,10, -5., 5.);
	  FillHist("TypeVsFlavour/MuonFF", MuonColl[1].LeptonGenType(),   weight,10, -5., 5.);
          if(Jet1Flavour == "LF")      FillHist("TypeVsFlavour/MuonFF_LF", MuonColl[0].LeptonGenType(),   weight,10, -5., 5.);
          else if(Jet1Flavour == "HF") FillHist("TypeVsFlavour/MuonFF_HF", MuonColl[0].LeptonGenType(),   weight,10, -5., 5.);
	  
          if(Jet2Flavour == "LF")      FillHist("TypeVsFlavour/MuonFF_LF", MuonColl[1].LeptonGenType(),   weight,10, -5., 5.);
          else if(Jet2Flavour == "HF") FillHist("TypeVsFlavour/MuonFF_HF", MuonColl[1].LeptonGenType(),   weight,10, -5., 5.);

	  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 0, weight, 5, 0., 5.,"");
	  if (SameCharge(MuonTopColl))  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 1, weight, 5, 0., 5.,"");
          if (SameCharge(MuonHNLColl))  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 2, weight, 5, 0., 5.,"");
          if (SameCharge(MuonV2Coll)) FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 3, weight, 5, 0., 5.,"");
        }


	if(BJetColl.size() == 0) {
	  FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 0, weight, 5, 0., 5.,"");
	  if (SameCharge(MuonTopColl))  FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 1, weight, 5, 0., 5.,"");
	  if (SameCharge(MuonHNLColl))  FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 2, weight, 5, 0., 5.,"");
	  if (SameCharge(MuonV2Coll)) FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 3, weight, 5, 0., 5.,"");
	  
	  
	  if(MuonColl[0].IsFake() && MuonColl[1].IsPrompt()){
	    if (SameCharge(MuonTopColl))  FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 1, weight, 5, 0., 5.,"");
	    if (SameCharge(MuonHNLColl))  FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 2, weight, 5, 0., 5.,"");
	    if (SameCharge(MuonV2Coll)) FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 3, weight, 5, 0., 5.,"");
	  }
	  if(MuonColl[1].IsFake() && MuonColl[0].IsPrompt()){
	    if (SameCharge(MuonTopColl))  FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 1, weight, 5, 0., 5.,"");
	    if (SameCharge(MuonHNLColl))  FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 2, weight, 5, 0., 5.,"");
	    if (SameCharge(MuonV2Coll)) FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 3, weight, 5, 0., 5.,"");
	  }
	  if(MuonColl[0].IsFake() &&MuonColl[1].IsFake()){
	    if (SameCharge(MuonTopColl))  FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 1, weight, 5, 0., 5.,"");
            if (SameCharge(MuonHNLColl))  FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 2, weight, 5, 0., 5.,"");
            if (SameCharge(MuonV2Coll)) FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 3, weight, 5, 0., 5.,"");
	  }
	}
      }
      
      
      vector<Electron> ElectronHNLColl;
      vector<Electron> ElectronHNLN1Coll;
      vector<Electron> ElectronHNLN2Coll;
      vector<Electron> ElectronHNLN3Coll;
      vector<Electron> ElectronHNLN4Coll;
      vector<Electron> ElectronHNLN5Coll;
      vector<Electron> ElectronHNLN6Coll;
      vector<Electron> ElectronV2Coll;

      int nFakeEl(0);
      for(auto iel: ElectronColl){
	if(iel.PassID("HNTightV2")) ElectronV2Coll.push_back(iel);                                                                              
	if(iel.PassID("HNL_ULID_"+GetYearString()))  ElectronHNLColl.push_back(iel);                                                                              
	if(iel.PassID("HNL_ULID_N1_"+GetYearString()))  ElectronHNLN1Coll.push_back(iel);                                                                              
	if(iel.PassID("HNL_ULID_N2_"+GetYearString()))  ElectronHNLN2Coll.push_back(iel);                                                                              
	if(iel.PassID("HNL_ULID_N3_"+GetYearString()))  ElectronHNLN3Coll.push_back(iel);                                                                              
	if(iel.PassID("HNL_ULID_N4_"+GetYearString()))  ElectronHNLN4Coll.push_back(iel);                                                                              
	if(iel.PassID("HNL_ULID_N5_"+GetYearString()))  ElectronHNLN5Coll.push_back(iel);                                                                              
	if(iel.PassID("HNL_ULID_N6_"+GetYearString()))  ElectronHNLN6Coll.push_back(iel);                                                                              
        if(iel.IsFake()) nFakeEl++;

      }

      
      if (nFakeEl==0 && dilep_channel == EE) continue;

      for(auto iel: ElectronColl){
	int El_lepType = iel.LeptonGenType();
	FillHist("LepType/AllElectron", El_lepType,   weight,12, -6.,6.);

        TString JetFlavour = iel.CloseJet_Flavour();

        FillHist("JetType/AllElectron", iel.CloseJet_FlavourInt(),   weight,6, 0., 6.);

	if(iel.IsFake()){
	  
	  if (iel.Pt() < 500.)  FillHist("FakeEl/Pt", iel.Pt(),   weight,100, 0., 500.);
	  else FillHist("FakeEl/Pt", 499.,   weight,100, 0., 500.);

	  if(JetFlavour == "LF")  {
	    if (iel.Pt() < 500.)  FillHist("FakeEl/LF_Pt", iel.Pt(),   weight,100, 0., 500.);
	    else FillHist("FakeEl/LF_Pt", 499.,   weight,100, 0., 500.);
	  }
	  if(JetFlavour == "HF")  {
            if (iel.Pt() < 500.)  FillHist("FakeEl/HF_Pt", iel.Pt(),   weight,100, 0., 500.);
            else FillHist("FakeEl/HF_Pt", 499.,   weight,100, 0., 500.);
          }

	  FillHist("JetType/FakeElectron", iel.CloseJet_FlavourInt(),   weight,6, 0., 6.);

	  if(JetFlavour == "LF")      FillHist("TypeVsFlavour/FakeElectronLF", El_lepType,   weight,10, -5., 5.);
	  else if(JetFlavour == "HF") FillHist("TypeVsFlavour/FakeElectronHF", El_lepType,   weight,10, -5., 5.);

	  if(SameCharge(ElectronColl)) {
	    if (iel.Pt() < 500.)  FillHist("FakeEl/SSPt", iel.Pt(),   weight,100, 0., 500.);
	    else FillHist("FakeEl/SSPt", 499.,   weight,100, 0., 500.);


	    if(JetFlavour == "LF")  {
	      if (iel.Pt() < 500.)  FillHist("FakeEl/SSLF_Pt", iel.Pt(),   weight,100, 0., 500.);
	      else FillHist("FakeEl/SSLF_Pt", 499.,   weight,100, 0., 500.);
	    }
	    if(JetFlavour == "HF")  {
	      if (iel.Pt() < 500.)  FillHist("FakeEl/SSHF_Pt", iel.Pt(),   weight,100, 0., 500.);
	      else FillHist("FakeEl/SSHF_Pt", 499.,   weight,100, 0., 500.);
	    }

	    if(JetFlavour == "LF")      FillHist("TypeVsFlavour/SSFakeElectronLF", El_lepType,   weight,10, -5., 5.);
	    else if(JetFlavour == "HF") FillHist("TypeVsFlavour/SSFakeElectronHF", El_lepType,   weight,10, -5., 5.);
	  }
	}
      }
      

      if (SameCharge(ElectronColl)) {

        Particle ll =  (ElectronColl[0]) + (ElectronColl[1]);
	
        if (! (fabs(ll.M()-M_Z) < 15)) {
	  
	  FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 0, weight, 10, 0., 10.,"");
	  if(SameCharge(ElectronV2Coll))                     FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 1, weight, 10, 0., 10.,"");
	  if(SameCharge(ElectronHNLColl))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 2, weight, 10, 0., 10.,"");
	  if(SameCharge(ElectronHNLN1Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 3, weight, 10, 0., 10.,"");
	  if(SameCharge(ElectronHNLN2Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 4, weight, 10, 0., 10.,"");
	  if(SameCharge(ElectronHNLN3Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 5, weight, 10, 0., 10.,"");
	  if(SameCharge(ElectronHNLN4Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 6, weight, 10, 0., 10.,"");
	  if(SameCharge(ElectronHNLN5Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 7, weight, 10, 0., 10.,"");
	  if(SameCharge(ElectronHNLN6Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel), 8, weight, 10, 0., 10.,"");

	  
	  TString Jet1Flavour = CloseJetFlavour(AllAK4Jets, ElectronColl[0]) ;
	  TString Jet2Flavour = CloseJetFlavour(AllAK4Jets, ElectronColl[1]) ;
	  
	  if(ElectronColl[0].IsFake()  && ElectronColl[1].IsPrompt()){

	    if(Jet1Flavour == "LF")       FillHist("TypeVsFlavour/ElectronLF", ElectronColl[0].LeptonGenType(),   weight,10, -5., 5.);
	    else  if(Jet1Flavour == "HF") FillHist("TypeVsFlavour/ElectronHF", ElectronColl[0].LeptonGenType(),   weight,10, -5., 5.);
	    

	    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 0, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronV2Coll))                     FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 1, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLColl))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 2, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN1Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 3, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN2Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 4, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN3Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 5, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN4Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 6, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN5Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 7, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN6Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 8, weight, 10, 0., 10.,"");
	  }
	  if(ElectronColl[1].IsFake() && ElectronColl[0].IsPrompt()){

            if(Jet2Flavour == "LF")      FillHist("TypeVsFlavour/ElectronLF", ElectronColl[1].LeptonGenType(),   weight,10, -5., 5.);
            else if(Jet2Flavour == "HF") FillHist("TypeVsFlavour/ElectronHF", ElectronColl[1].LeptonGenType(),   weight,10, -5., 5.);

	    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 0, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronV2Coll))                     FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 1, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLColl))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 2, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN1Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 3, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN2Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 4, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN3Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 5, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN4Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 6, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN5Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 7, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN6Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 8, weight, 10, 0., 10.,"");
	  }
	  if(ElectronColl[0].IsFake()  &&  ElectronColl[1].IsFake()){
	    
            if(Jet1Flavour == "LF")      FillHist("TypeVsFlavour/ElectronFF_LF", ElectronColl[0].LeptonGenType(),   weight,10, -5., 5.);
            else if(Jet1Flavour == "HF") FillHist("TypeVsFlavour/ElectronFF_HF", ElectronColl[0].LeptonGenType(),   weight,10, -5., 5.);
	    
	    if(Jet2Flavour == "LF")      FillHist("TypeVsFlavour/ElectronFF_LF", ElectronColl[1].LeptonGenType(),   weight,10, -5., 5.);
            else if(Jet2Flavour == "HF") FillHist("TypeVsFlavour/ElectronFF_HF", ElectronColl[1].LeptonGenType(),   weight,10, -5., 5.);

            FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 0, weight, 10, 0., 10.,"");
            if(SameCharge(ElectronV2Coll))                     FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 1, weight, 10, 0., 10.,"");
            if(SameCharge(ElectronHNLColl))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 2, weight, 10, 0., 10.,"");
            if(SameCharge(ElectronHNLN1Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 3, weight, 10, 0., 10.,"");
            if(SameCharge(ElectronHNLN2Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 4, weight, 10, 0., 10.,"");
            if(SameCharge(ElectronHNLN3Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 5, weight, 10, 0., 10.,"");
            if(SameCharge(ElectronHNLN4Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 6, weight, 10, 0., 10.,"");
            if(SameCharge(ElectronHNLN5Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 7, weight, 10, 0., 10.,"");
            if(SameCharge(ElectronHNLN6Coll))                    FillHist ("SameSignDilep_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 8, weight, 10, 0., 10.,"");

	  }
	  if(BJetColl.size() == 0) {
	    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 0, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronV2Coll))                     FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 1, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLColl))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 2, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN1Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 3, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN2Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 4, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN3Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 5, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN4Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 6, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN5Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 7, weight, 10, 0., 10.,"");
	    if(SameCharge(ElectronHNLN6Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel), 8, weight, 10, 0., 10.,"");
	    
	    if(ElectronColl[0].IsFake()  && ElectronColl[1].IsPrompt()){
	      FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 0, weight, 10, 0., 10.,"");
	      if(SameCharge(ElectronV2Coll))                     FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 1, weight, 10, 0., 10.,"");
	      if(SameCharge(ElectronHNLColl))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 2, weight, 10, 0., 10.,"");

	      if(SameCharge(ElectronHNLN1Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 3, weight, 10, 0., 10.,"");
	      if(SameCharge(ElectronHNLN2Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 4, weight, 10, 0., 10.,"");
	      if(SameCharge(ElectronHNLN3Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 5, weight, 10, 0., 10.,"");
	      if(SameCharge(ElectronHNLN4Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 6, weight, 10, 0., 10.,"");
	      if(SameCharge(ElectronHNLN5Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 7, weight, 10, 0., 10.,"");
	      if(SameCharge(ElectronHNLN6Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_Fake", 8, weight, 10, 0., 10.,"");
	    }
	    if(ElectronColl[1].IsFake() && ElectronColl[0].IsPrompt()){
              FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 0, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronV2Coll))                     FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 1, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLColl))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 2, weight, 10, 0., 10.,"");

              if(SameCharge(ElectronHNLN1Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 3, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLN2Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 4, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLN3Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 5, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLN4Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 6, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLN5Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 7, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLN6Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet2Flavour+"_Fake", 8, weight, 10, 0., 10.,"");
            }

	    if(ElectronColl[0].IsFake()  &&  ElectronColl[1].IsFake()){
	      FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 0, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronV2Coll))                     FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 1, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLColl))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 2, weight, 10, 0., 10.,"");
	      
	      if(SameCharge(ElectronHNLN1Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 3, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLN2Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 4, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLN3Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 5, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLN4Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 6, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLN5Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 7, weight, 10, 0., 10.,"");
              if(SameCharge(ElectronHNLN6Coll))                    FillHist ("SameSignDilep_0b_"+GetChannelString(dilep_channel) +"_"+Jet1Flavour+"_"+Jet2Flavour+"_Fake", 8, weight, 10, 0., 10.,"");
	      
	    }

	  }
	} 
      }
      continue;
    }
      
    continue;
  
    
    if(SameCharge(LepsAll)) {

      
      FillMuonCollPlots(param,"SS_Muons"  , MuonColl , weight);                                                                                                                                                  
      FillAllElectronPlots(param,"SS_Electrons"  , ElectronColl , weight);                                                                                                                                 


      for(auto ilep : LepsAll){
        int  lepType = ilep->LeptonGenType();

        if(ilep->LeptonFlavour() == Lepton::ELECTRON)       FillHist( "LepType/SSElectron", lepType ,weight, 14., -7., 7);
        else  FillHist( "LepType/SSMuon", lepType, weight, 14., -7., 7);
      }
    }


    std::vector<Muon> MuonCollMVALoose;
    std::vector<Muon> MuonCollMVAMedium;
    std::vector<Muon> MuonCollMVATight;
    std::vector<Muon> MuonCollHNL;
    std::vector<Muon> MuonCollMVAVeryTight;
    std::vector<Muon> MuonCollMVALooseT;
    std::vector<Muon> MuonCollMVAMediumT;
    std::vector<Muon> MuonCollMVATightT;
    std::vector<Muon> MuonCollHNTL;
    std::vector<Muon> MuonCollMVAVeryTightT;
    std::vector<Muon> MuonCollPOGTight;

    std::vector<Muon> MuonCollPOGMVA;
    std::vector<Muon> MuonCollPOGMVAT;

    std::vector<Electron> ElectronCollHNL;
    std::vector<Electron> ElectronCollMVALoose;
    std::vector<Electron> ElectronCollMVAMedium;
    std::vector<Electron> ElectronCollMVATight;
    std::vector<Electron> ElectronCollMVAVeryTight;
    std::vector<Electron> ElectronCollMVALooseT;
    std::vector<Electron> ElectronCollMVAMediumT;
    std::vector<Electron> ElectronCollMVATightT;
    std::vector<Electron> ElectronCollMVAVeryTightT;

    std::vector<Electron> ElectronCollPOGTight;
    std::vector<Electron> ElectronCollHNTight;

    std::vector<Muon> MuonCollMVAMaxLoose;
    std::vector<Muon> MuonCollMVAMaxMedium;
    std::vector<Muon> MuonCollMVAMaxTight;


    std::vector<Muon> MuonCollTypeM6;
    std::vector<Muon> MuonCollTypeM5;
    std::vector<Muon> MuonCollTypeM4;
    std::vector<Muon> MuonCollTypeM3;
    std::vector<Muon> MuonCollTypeM2;
    std::vector<Muon> MuonCollTypeM1;
    std::vector<Muon> MuonCollType0;
    std::vector<Muon> MuonCollType1;
    std::vector<Muon> MuonCollType2;
    std::vector<Muon> MuonCollType3;
    std::vector<Muon> MuonCollType4;
    std::vector<Muon> MuonCollType5;
    std::vector<Muon> MuonCollType6;

    for(auto iel :  MuonColl ) {

      int  lepType = iel.LeptonGenType();

    
      if(lepType == -6) MuonCollTypeM6.push_back(iel);
      if(lepType == -5) MuonCollTypeM5.push_back(iel);
      if(lepType == -4) MuonCollTypeM4.push_back(iel);
      if(lepType == -3) MuonCollTypeM3.push_back(iel);
      if(lepType == -2) MuonCollTypeM2.push_back(iel);
      if(lepType == -1) MuonCollTypeM1.push_back(iel);
      if(lepType == 0) MuonCollType0.push_back(iel);
      if(lepType == 1) MuonCollType1.push_back(iel);
      if(lepType == 2) MuonCollType2.push_back(iel);
      if(lepType == 3) MuonCollType3.push_back(iel);
      if(lepType == 4) MuonCollType4.push_back(iel);
      if(lepType == 5) MuonCollType5.push_back(iel);
      if(lepType == 6) MuonCollType6.push_back(iel);
      if(iel.PassID("HNL_ULID_v1"))   {
	MuonCollHNL.push_back(iel);

 	if(iel.MVA() > 0.5)  MuonCollPOGMVAT.push_back(iel);   
      }

      if(iel.MVA() > 0.5)  MuonCollPOGMVA.push_back(iel);

      if(iel.PassID("HNTightV2"))     MuonCollPOGTight.push_back(iel);                                                                                                                                                                                                                        

    }


    std::vector<Electron> ElectronCollTypeM6;
    std::vector<Electron> ElectronCollTypeM5;
    std::vector<Electron> ElectronCollTypeM4;
    std::vector<Electron> ElectronCollTypeM3;
    std::vector<Electron> ElectronCollTypeM2;
    std::vector<Electron> ElectronCollTypeM1;
    std::vector<Electron> ElectronCollType0;
    std::vector<Electron> ElectronCollType1;
    std::vector<Electron> ElectronCollType2;
    std::vector<Electron> ElectronCollType3;
    std::vector<Electron> ElectronCollType4;
    std::vector<Electron> ElectronCollType5;
    std::vector<Electron> ElectronCollType6;

    for(auto iel :  ElectronColl ) {

      int  lepType = iel.LeptonGenType();


      if(lepType == -6) ElectronCollTypeM6.push_back(iel);
      if(lepType == -5) ElectronCollTypeM5.push_back(iel);
      if(lepType == -4) ElectronCollTypeM4.push_back(iel);
      if(lepType == -3) ElectronCollTypeM3.push_back(iel);
      if(lepType == -2) ElectronCollTypeM2.push_back(iel);
      if(lepType == -1) ElectronCollTypeM1.push_back(iel);
      if(lepType == 0) ElectronCollType0.push_back(iel);
      if(lepType == 1) ElectronCollType1.push_back(iel);
      if(lepType == 2) ElectronCollType2.push_back(iel);
      if(lepType == 3) ElectronCollType3.push_back(iel);
      if(lepType == 4) ElectronCollType4.push_back(iel);
      if(lepType == 5) ElectronCollType5.push_back(iel);
      if(lepType == 6) ElectronCollType6.push_back(iel);


    }
    

    
    if(!IsData){
      
      FillMuonCollPlots(param,"TypeM6_Muons"  , MuonCollTypeM6 , weight);
      FillMuonCollPlots(param,"TypeM5_Muons"  , MuonCollTypeM5 , weight);
      FillMuonCollPlots(param,"TypeM4_Muons"  , MuonCollTypeM4 , weight);
      FillMuonCollPlots(param,"TypeM3_Muons"  , MuonCollTypeM3 , weight);
      FillMuonCollPlots(param,"TypeM2_Muons"  , MuonCollTypeM2 , weight);
      FillMuonCollPlots(param,"TypeM1_Muons"  , MuonCollTypeM1 , weight);
      FillMuonCollPlots(param,"Type0_Muons"  , MuonCollType0 , weight);
      FillMuonCollPlots(param,"Type1_Muons"  , MuonCollType1 , weight);
      FillMuonCollPlots(param,"Type2_Muons"  , MuonCollType2 , weight);
      FillMuonCollPlots(param,"Type3_Muons"  , MuonCollType3 , weight);
      FillMuonCollPlots(param,"Type4_Muons"  , MuonCollType4 , weight);
      FillMuonCollPlots(param,"Type5_Muons"  , MuonCollType5 , weight);
      FillMuonCollPlots(param,"Type6_Muons"  , MuonCollType6 , weight);
      FillAllElectronPlots(param,"TypeM6_Electrons"  , ElectronCollTypeM6 , weight);
      FillAllElectronPlots(param,"TypeM5_Electrons"  , ElectronCollTypeM5 , weight);
      FillAllElectronPlots(param,"TypeM4_Electrons"  , ElectronCollTypeM4 , weight);
      FillAllElectronPlots(param,"TypeM3_Electrons"  , ElectronCollTypeM3 , weight);
      FillAllElectronPlots(param,"TypeM2_Electrons"  , ElectronCollTypeM2 , weight);
      FillAllElectronPlots(param,"TypeM1_Electrons"  , ElectronCollTypeM1 , weight);
      FillAllElectronPlots(param,"Type0_Electrons"  , ElectronCollType0 , weight);
      FillAllElectronPlots(param,"Type1_Electrons"  , ElectronCollType1 , weight);
      FillAllElectronPlots(param,"Type2_Electrons"  , ElectronCollType2 , weight);
      FillAllElectronPlots(param,"Type3_Electrons"  , ElectronCollType3 , weight);
      FillAllElectronPlots(param,"Type4_Electrons"  , ElectronCollType4 , weight);
      FillAllElectronPlots(param,"Type5_Electrons"  , ElectronCollType5 , weight);
      FillAllElectronPlots(param,"Type6_Electrons"  , ElectronCollType6 , weight);
    }
    if(SameCharge(ElectronCollHNL))FillAllElectronPlots(param,"HNL_Electrons"  , ElectronCollHNL , weight);
    
    map<double , std::vector<Muon> > MuonVecMap;
    MuonVecMap[0.] = MuonColl;
    MuonVecMap[1.] = MuonCollHNL;
    MuonVecMap[2.] = MuonCollMVALoose;
    MuonVecMap[3.] = MuonCollMVAMedium;
    MuonVecMap[4.] = MuonCollMVATight;
    MuonVecMap[5.] = MuonCollMVAVeryTight;
    MuonVecMap[6.] = MuonCollPOGMVA;
    MuonVecMap[7.] = MuonCollPOGTight;
    MuonVecMap[8.] = MuonCollMVALooseT;
    MuonVecMap[9.] = MuonCollMVAMediumT;
    MuonVecMap[10.] = MuonCollMVATightT;
    MuonVecMap[11.] = MuonCollMVAVeryTightT;
    MuonVecMap[12.] = MuonCollPOGMVAT;



    for(auto imap : MuonVecMap)   {
      if(SameCharge(imap.second))FillHist( "Vec/MuonColl", imap.first,  weight,15, 0., 15,"HNTight");
    }
    

    map<double , std::vector<Electron> > ElectronVecMap;
    ElectronVecMap[0.] = ElectronColl;
    ElectronVecMap[1.] = ElectronCollHNL;
    ElectronVecMap[2.] = ElectronCollMVALoose;
    ElectronVecMap[3.] = ElectronCollMVAMedium;
    ElectronVecMap[4.] = ElectronCollMVATight;
    ElectronVecMap[5.] = ElectronCollMVAVeryTight;
    ElectronVecMap[6.] = ElectronCollMVALooseT;
    ElectronVecMap[7.] = ElectronCollMVAMediumT;
    ElectronVecMap[8.] = ElectronCollMVATightT;
    ElectronVecMap[9.] = ElectronCollMVAVeryTightT;
    ElectronVecMap[10.] = ElectronCollPOGTight;


    for(auto imap : ElectronVecMap)   {
      if(SameCharge(imap.second))FillHist( "Vec/ElectronColl", imap.first,  weight,15, 0., 15,"HNTight");
    }


    continue;



    return;




    if(dilep_channel==MuMu){
      
      vector<Muon> LMuons;
      vector<Muon> HMuons;
      
      for(auto imuon : MuonColl){
	Lepton *l = (Lepton *)(&imuon);
	
	int lepSrc = GetFakeLepSrcType(*l, jetsTmp);
	if(lepSrc == 1)   LMuons.push_back(imuon);
	if(lepSrc == 2 || lepSrc == 3)  HMuons.push_back(imuon);
      }

      
      FillMuonCollPlots(param,"LightFlavour_Muons"  , LMuons , weight);
      FillMuonCollPlots(param,"HeavyFlavour_Muons"  , HMuons , weight);
      
    }
    else{

      vector<Electron> LElectrons;
      vector<Electron> HElectrons;

      for(auto iel : ElectronColl){
        Lepton *l = (Lepton *)(&iel);

        int lepSrc = GetFakeLepSrcType(*l, jetsTmp);
        if(lepSrc == 1)   LElectrons.push_back(iel);
        if(lepSrc == 2 || lepSrc == 3)  HElectrons.push_back(iel);
      }


      FillAllElectronPlots(param,"LightFlavour_Electrons"  , LElectrons , weight);
      FillAllElectronPlots(param,"HeavyFlavour_Electrons"  , HElectrons , weight);


      
    }
    continue;

  }
}

void HNL_GenericObject_Studies::RunLeptonChannel(HNL_LeptonCore::Channel channel_ID, std::vector<Lepton *> LepsT,std::vector<Lepton *> LepsV, std::vector<Tau> TauColl,  std::vector<Jet> JetCollLoose, std::vector<Jet> JetColl, std::vector<Jet> VBFJetColl,  std::vector<FatJet> FatjetColl,std::vector<Jet> BJetColl,   Event Ev, AnalyzerParameter param,  double _weight)   {
  

  FillHist (GetChannelString(channel_ID)+"_NoCut", 1, _weight, 2, 0., 2.,"");

  HNL_LeptonCore::SearchRegion Region1 = sigmm;
  //  HNL_LeptonCore::SearchRegion Region2 = sigmm_17028;

  if(channel_ID == EE) {  Region1 = sigee; };//  Region2 = sigee_17028;}
  if(channel_ID == EMu){ Region1 = sigem; }//Region2 = sigem_17028;}
  
  Particle METv = GetMiniAODvMET("PuppiT1xyULCorr");

  //CheckHEMIssue(channel_ID, LepsT, LepsV,  JetColl, METv, _weight, param.Name, "");


  int njets_old =  (JetColl.size() + FatjetColl.size()) ;
  int njets_vbf = (VBFJetColl.size() + FatjetColl.size());
  int njets = ( njets_vbf > njets_old) ? njets_vbf : njets_old ;

  bool dijet = (FatjetColl.size() > 0) || (JetColl.size() > 1);
  //bool PassBJetMVeto = (BJetColl.size()==0);
  int NBJetLV = 0;
  for(auto ib : BJetColl){
    bool hasCloseLep=false;
    for(auto il : LepsV) {
      if(il->DeltaR(ib) < 0.4) hasCloseLep=true;
    }
    if(!hasCloseLep) NBJetLV++;
  }
  
  bool PassBJetMVetoSR1 = (NBJetLV==0);

  

  // Update NAME for Muon channel                                                                                               

  TString region1 = GetChannelString(channel_ID)+"_"+param.Name;                                                       
  
  FillCutflow(Region1,_weight, "SSNoCut",param);

  // UL ANALYSIS                                                                                                                
  
  Fill_RegionPlots(param,"SSInclusive" ,  VBFJetColl, FatjetColl, LepsT,  METv, nPV, _weight);


  // UL ANALYSIS
  
  if(MCSample.Contains("Type") || MCSample.Contains("private")){
    
    vector<Gen> gen_lep= GetGenLepronsSignal();
    std::sort(gen_lep.begin(),   gen_lep.end(),        PtComparing);

    
    if(gen_lep.size() == 2){
      
      if(fabs(gen_lep[0].PID()) == 11 && fabs(gen_lep[1].PID()) ==11 ){
	
	if( (fabs(gen_lep[0].Eta()) < 2.5 && gen_lep[0].Pt() > 25) &&
	    (fabs(gen_lep[1].Eta()) < 2.5 && gen_lep[1].Pt() > 15)){
	  FillCutflow(Region1,_weight, "SSGen",  param);
	  FillCutflow(Region1,_weight, "SSGen2", param);
	  
	}
	else       if( (fabs(gen_lep[0].Eta()) < 2.5 && gen_lep[0].Pt() > 30) &&
		       (fabs(gen_lep[1].Eta()) < 2.5 && gen_lep[1].Pt() > 10)){
	  FillCutflow(Region1,_weight, "SSGen",  param);
	}
	
	else return;
      }
      
      else       if(fabs(gen_lep[0].PID()) == 13 && fabs(gen_lep[1].PID()) ==13 ){

        if( (fabs(gen_lep[0].Eta()) < 2.4 && gen_lep[0].Pt() > 20) &&
            (fabs(gen_lep[1].Eta()) < 2.4 && gen_lep[1].Pt() > 10)){
          FillCutflow(Region1,_weight, "SSGen", param);
          FillCutflow(Region1,_weight, "SSGen2", param);

        }
        else       if( (fabs(gen_lep[0].Eta()) < 2.4 && gen_lep[0].Pt() > 30) &&
                       (fabs(gen_lep[1].Eta()) < 2.4 && gen_lep[1].Pt() > 5)){
          FillCutflow(Region1,_weight, "SSGen", param);
        }

        else return;
      }
      else       if(fabs(gen_lep[0].PID()) == 13 && fabs(gen_lep[1].PID()) ==11 ){

        if( (fabs(gen_lep[0].Eta()) < 2.4 && gen_lep[0].Pt() > 25) &&
            (fabs(gen_lep[1].Eta()) < 2.5 && gen_lep[1].Pt() > 15)){
          FillCutflow(Region1,_weight, "SSGen", param);
          FillCutflow(Region1,_weight, "SSGen2", param);

        }
        else       if( (fabs(gen_lep[0].Eta()) < 2.4 && gen_lep[0].Pt() > 30) &&
                       (fabs(gen_lep[1].Eta()) < 2.5 && gen_lep[1].Pt() > 10)){
          FillCutflow(Region1,_weight, "SSGen", param);

        }

        else return;
      }

      else       if(fabs(gen_lep[0].PID()) == 11 && fabs(gen_lep[1].PID()) ==13 ){

        if( (fabs(gen_lep[0].Eta()) < 2.5 && gen_lep[0].Pt() > 25) &&
            (fabs(gen_lep[1].Eta()) < 2.4 && gen_lep[1].Pt() > 15)){
          FillCutflow(Region1,_weight, "SSGen", param);
          FillCutflow(Region1,_weight, "SSGen2", param);

        }
        else       if( (fabs(gen_lep[0].Eta()) < 2.5 && gen_lep[0].Pt() > 30) &&
                       (fabs(gen_lep[1].Eta()) < 2.4 && gen_lep[1].Pt() > 5)){
          FillCutflow(Region1,_weight, "SSGen", param);

	}

	else return;
      }
    }      
  }
  
  
  if (!PassTriggerSelection(channel_ID, Ev, LepsT,"Full",false)) return;


  FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"Trig",param);

  if (!(PassTriggerSelection(channel_ID, Ev, LepsT,"Dilep",false) || PassTriggerSelection(channel_ID, Ev, LepsT,"HighPt",false) || PassTriggerSelection(channel_ID, Ev, LepsT,"Lep",false))) return;

  FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"Trig2",param);

  if (LepsT.size() != 2) return;

  if(!SameCharge(LepsT)) return;

  FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID),param);

  // PASS DIMUON TRIGGER                                                                                                                                             
  if (!PassTriggerSelection(channel_ID, Ev, LepsT,"Dilep",false)) return;
  FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"Trig2L",param);


  if (!CheckLeptonFlavourForChannel(channel_ID, LepsV)) return;

  FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_Pt",param);

  if (LepsV.size() != 2) return;
  FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_LepVeto",param);
  
  if(!PassHEMVeto(LepsT,_weight)) return;
  FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_HEMVeto",param);
  

  if (channel_ID==EE  && (fabs(GetLLMass(LepsT)-M_Z) < 15)) return;

  if(GetLLMass(LepsT) > 10.) {
    
    param.Name = GetChannelString(channel_ID)+"_"+param.DefName;
    
    FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_LLMass",param);

    if(njets > 0 ){
      FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_Jet",param);
      
      if(PassBJetMVetoSR1) {
	FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_BJet",param);
	if(dijet)        FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_DiJet",param);
	
	
	if(FatjetColl.size() > 0){
	  
	  if(RunSignalRegionAK8(true, channel_ID, Inclusive, LepsT, LepsV, TauColl,JetColl, FatjetColl, BJetColl, Ev, METv, param,   _weight ))  
	    {
	      FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_SR1",param);
	      
	    }
	  
	  else FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_SR1Fail",param);
	}
	else{
	  
	  if(PassVBFInitial(VBFJetColl)&&RunSignalRegionWW(true,channel_ID, Inclusive, LepsT, LepsV, TauColl, VBFJetColl, FatjetColl, BJetColl, Ev,  METv, param,  _weight )){
	    FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_SR2",param);
	    
	    
	  }
	  else{
	    
	    if(RunSignalRegionAK4(true,channel_ID, Inclusive, LepsT, LepsV ,TauColl, JetColl, FatjetColl, BJetColl, Ev, METv, param, _weight ))             FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_SR3",param);
	    
	    
	    else if(JetColl.size() < 2 && LepsT[1]->Pt() >80.)             FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_SR4",param);
	    
	    else  FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_SR3Fail",param);
	    
	  } // FAIL SR2                                                                                                       
	}// Fail SR1                                                                                                          
      }// Fail Tau                                                                                                            
      }// NJET                                                                                                                  
    else if (LepsT[1]->Pt() >80.)   FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_SR5",param);
  } // LLMASS      
  
  
  return;

  
}


  


HNL_GenericObject_Studies::HNL_GenericObject_Studies(){


}
 
HNL_GenericObject_Studies::~HNL_GenericObject_Studies(){
  

  

}


void HNL_GenericObject_Studies::MakeType1SSWWSignalPlots(TString process, bool apply_reco_cut){

  if(apply_reco_cut) process=process+"_PtEtaReq";

  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);
    
    if(apply_reco_cut){
      if( (fabs(gen.PID()) == 13 )|| (fabs(gen.PID()) == 11 )){
	if(gen.Pt() < 10. || fabs(gen.Eta()) > 2.4) return;
	
      }
    }																													
  }
  

  /////////////

  Gen Lep1;
  Gen Lep2;
  Gen j1,j2;
  bool j1IsSet(false);
  bool l1IsSet(false);

  TString LepFl_l1, LepFl_l2;
  int Lep_Mother_ind(-1);

  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);
    if( ! ( ( fabs(gen.PID()) == 13)  || (fabs(gen.PID()) == 11) )) continue;
    if (gen.Status() == 23){
      TString LepFl = (fabs(gen.PID()) == 13) ? "Mu" : "El";

      if(!l1IsSet) {  Lep1= gen; l1IsSet=true;LepFl_l1= LepFl;}
      else { Lep2 = gen; LepFl_l2= LepFl;}
      Lep_Mother_ind = gen.MotherIndex();
    }
  }

  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);

    if (gen.MotherIndex() == Lep_Mother_ind){
      if(fabs(gen.PID()) > 6) continue;
      if(!j1IsSet) {  j1= gen; j1IsSet=true;}
      else j2 = gen;
    }
  }
  double ZeroCheck = Lep1.Pt() * Lep2.Pt()*j1.Pt() *j2.Pt() ;
  if(ZeroCheck == 0.){
    cout << "Lep1 " << Lep1.Pt() << endl;
    cout << "Lep2 " << Lep2.Pt() << endl;
    cout <<  "j1 " << j1.Pt() << endl;
    cout <<  "j2 " << j2.Pt() << endl;
    PrintGen(All_Gens);
    exit(EXIT_FAILURE);
  }
  
  FillHist( "SignalGen"+process+"/Jet1_eta", j1.Eta(),  1.,100, -5., 5,"#eta");
  FillHist( "SignalGen"+process+"/Jet2_eta", j2.Eta(),  1.,100, -5., 5,"#eta");
  FillHist( "SignalGen"+process+"/Jet_pt",  j1.Pt(), 1., 200, 0., 1000.,"pt (GeV)");
  FillHist( "SignalGen"+process+"/Jet_pt",  j2.Pt(), 1., 200, 0., 1000.,"pt (GeV)");
  FillHist( "SignalGen"+process+"/Dijet_mass",  (j1+j2).M(),  1., 50, 0., 2000.,"m(jj) GeV");


  FillHist( "SignalGen"+process+"/Lep_"+LepFl_l1+"_pt",  Lep1.Pt(), 1., 200, 0., 1000.,"pt (GeV)");
  FillHist( "SignalGen"+process+"/Lep_"+LepFl_l1+"_eta", Lep1.Eta(),  1.,100, -5., 5,"#eta");
  FillHist( "SignalGen"+process+"/Lep_DeltaR_JJ", (j1+j2).DeltaR(Lep1), 1., 100, 0., 10.,"#DeltaR (jj,l1)");
  FillHist( "SignalGen"+process+"/Lep_DeltaR_JJ", (j1+j2).DeltaR(Lep2), 1., 100, 0., 10.,"#DeltaR (jj,l2)");
  FillHist( "SignalGen"+process+"/Mass_lep1_JJ", (j1+j2 + Lep1).M(), 1., 100, 0., 5000.,"M(l1jj) GeV");
  FillHist( "SignalGen"+process+"/Mass_lep2_JJ", (j1+j2 + Lep2).M(), 1., 100, 0., 5000.,"M(l2jj) GeV");
  FillHist( "SignalGen"+process+"/Mass_llJJ", (j1+j2 +Lep1+ Lep2).M(), 1., 100, 0., 5000.,"M(lljj) GeV ");

  FillHist("SignalGen"+process+"/DeltaPhi_Lep1_J1",  fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep1.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (l1,j1)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep2_J1",  fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (l2,j1)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep1_J2",  fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep1.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (l1,j2)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep2_J2",  fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (l2,j2)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep1_Lep2",  fabs(TVector2::Phi_mpi_pi( ( (Lep1.Phi() - Lep2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (l,l)");
  FillHist("SignalGen"+process+"/DeltaPhi_J1_J2",  fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - j2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (j,j)");

  double max_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep1.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep2.Phi() )))) > max_dpiphi)  max_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep2.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep1.Phi() )))) > max_dpiphi)  max_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep1.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep2.Phi() )))) > max_dpiphi)  max_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep2.Phi() ))));
  FillHist("SignalGen"+process+"/DeltaPhi_Lep_J_Max",  max_dpiphi,  1., 200, 0., 10.,"#Delta#phi max(l,j)");

  double min_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep1.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep2.Phi() )))) < min_dpiphi)  min_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep2.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep1.Phi() )))) < min_dpiphi)  min_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep1.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep2.Phi() )))) < min_dpiphi)  min_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep2.Phi() ))));
  FillHist("SignalGen"+process+"/DeltaPhi_Lep_J_Min",  min_dpiphi,  1., 200, 0., 5.,"#Delta#phi min(l,j)");


  FillHist("SignalGen"+process+"/DeltaR_Lep1_J1",  j1.DeltaR(Lep1), 1., 100, 0., 10.,"#DeltaR (j1,l1)");
  FillHist("SignalGen"+process+"/DeltaR_Lep2_J1",   j1.DeltaR(Lep2), 1., 100, 0., 10.,"#DeltaR (j1,l2)");
  FillHist("SignalGen"+process+"/DeltaR_Lep1_J2", j2.DeltaR(Lep1), 1., 100, 0., 10.,"#DeltaR (j2,l1)"); 
  FillHist("SignalGen"+process+"/DeltaR_Lep2_J2",  j2.DeltaR(Lep2), 1., 100, 0., 10.,"#DeltaR (j2,l2)");
  FillHist("SignalGen"+process+"/DeltaR_Lep1_Lep2",Lep2.DeltaR(Lep1), 1., 100, 0., 10.,"#DeltaR (l,l)");
  FillHist("SignalGen"+process+"/DeltaR_J1_J2",  j1.DeltaR(j2), 1., 100, 0., 10.,"#DeltaR (j1,j2)");

  double max_dpiR = j1.DeltaR(Lep1);
  if (j1.DeltaR(Lep2) > max_dpiR) max_dpiR = j1.DeltaR(Lep2);
  if (j2.DeltaR(Lep1) > max_dpiR) max_dpiR = j2.DeltaR(Lep1);
  if (j2.DeltaR(Lep2) > max_dpiR) max_dpiR = j2.DeltaR(Lep2);

  FillHist("SignalGen"+process+"/DeltaR_Lep_J_Max",  max_dpiR,  1., 200, 0., 5.,"#DeltaR max(L,J");

  double min_dpiR = j1.DeltaR(Lep1);
  if (j1.DeltaR(Lep2) < min_dpiR) min_dpiR = j1.DeltaR(Lep2);
  if (j2.DeltaR(Lep1) < min_dpiR) min_dpiR = j2.DeltaR(Lep1);
  if (j2.DeltaR(Lep2) < min_dpiR) min_dpiR = j2.DeltaR(Lep2);

  FillHist("SignalGen"+process+"/DeltaR_Lep_J_Min",  min_dpiR,  1., 200, 0., 5.,"#DeltaR min(L,J)");



  Particle JJMEta = j1+j2;

  FillHist("SignalGen"+process+"/MaxDEtaVBF_Jets_MJJ",JJMEta.M()   , 1, 100, 0., 4000., "MaxDEta MJJ");
  
  double maxDiJetDeta=fabs(j1.Eta() - j2.Eta());
  double Av_JetEta= 0.5*(j1.Eta()+ j2.Eta());
  double zeppenfeld = TMath::Max(Lep1.Eta()  - Av_JetEta , Lep2.Eta()  - Av_JetEta ) /maxDiJetDeta;
  FillHist( "SignalGen"+process+"/MaxDEtaVBF_Jets_zeppenfeld", zeppenfeld  , 1, 200, 0., 2., "zeppenfeld");




  
  return ;
}




void HNL_GenericObject_Studies::MakeType1VBFSignalPlots(TString process, bool apply_reco_cut){

  int N_Mother_ind(-1); // Index of N                                                                                                                                                                             
  int W2_ind(0); // Index of W2 : i.e W from N decay                                                                                                                                                     
  
  TString  mu_ch="";
  
  if(apply_reco_cut) process=process+"_PtEtaReq";


  // First loop over gen collection to get index of W/N and apply pt/eta cuts if needed                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);

    if(All_Gens.at(gen.MotherIndex()).PID() == 9900012 || All_Gens.at(gen.MotherIndex()).PID() == 9900014){

      if(apply_reco_cut){
        if( (fabs(gen.PID()) == 13 )|| (fabs(gen.PID()) == 11 )){
          if(gen.Pt() < 10. || fabs(gen.Eta()) > 2.4) return;
        }
      }
    }


    if(fabs(gen.PID()) == 24 && (All_Gens.at(gen.MotherIndex()).PID() == 9900012 || All_Gens.at(gen.MotherIndex()).PID() == 9900014)){
      W2_ind= i;
      // If W decays to W in gen then get daughter                                                                                                                                                                                                                                                                                                                                                                                    
      for(unsigned int i2=2; i2<All_Gens.size(); i2++){
        Gen gen2 = All_Gens.at(i2);
        if(gen2.MotherIndex() == W2_ind){
          if (fabs(gen2.PID()) == 24 ) W2_ind = i2;
        }
      }
    }

    if((gen.PID() == 9900012 || gen.PID() == 9900014) &&  gen.Status() == 22) {
      N_Mother_ind= gen.MotherIndex();
    }
  }

  // Loop over again to cut on Lep from W1                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);

    if(gen.MotherIndex() == N_Mother_ind){
      if(apply_reco_cut){
        if( (fabs(gen.PID()) == 13 )|| (fabs(gen.PID()) == 11 )){
          if(gen.Pt() < 10. || fabs(gen.Eta()) > 2.4) return;
        }
      }
    }
  }


  Gen LepFromN;
  Gen LepFromW;
  Gen N;
  Gen W2;
  Gen J, j1,j2;
  bool j1IsSet(false);

  TString LepFl_l1, LepFl_l2;

  // Loop over Gen collection and Natch N/W/l/j objects                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);

    if(int(gen.MotherIndex()) == W2_ind && gen.Status() == 23) {
      W2 = All_Gens.at(gen.MotherIndex());
      if(!j1IsSet) {  j1= gen; j1IsSet=true;}
      else j2 = gen;
    }
    if(gen.PID() == 9900012 || gen.PID() == 9900014){
      N= All_Gens.at(i);
    }

    if( fabs(gen.PID()) < 7 && gen.Status() == 23){
      if(gen.MotherIndex() == N_Mother_ind) J=gen;
    }
    
    if( ! ( ( fabs(gen.PID()) == 13)  || (fabs(gen.PID()) == 11) )) continue;

    TString LepFl = (fabs(gen.PID()) == 13) ? "Mu" : "El";

    if(All_Gens.at(gen.MotherIndex()).PID() == 9900012|| All_Gens.at(gen.MotherIndex()).PID() == 9900014) {
      LepFromN = gen;
      LepFl_l1=LepFl;
    }
    else if(gen.MotherIndex() == N_Mother_ind){
      LepFromW=gen;
      LepFl_l2=LepFl;
    }
  }
  
  
  FillHist( "SignalGen"+process+"/Jet_FromW_eta", j1.Eta(),  1.,100, -5., 5,"J #eta");
  FillHist( "SignalGen"+process+"/Jet_FromW_eta", j2.Eta(),  1.,100, -5., 5,"J #eta");
  FillHist( "SignalGen"+process+"/Jet_FromW_pt",  j1.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
  FillHist( "SignalGen"+process+"/Jet_FromW_pt",  j2.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
  FillHist( "SignalGen"+process+"/W_Dijet_mass",  W2.M(),  1., 200, 0., 200.,"W(jj) Mass (GeV)");

  FillHist( "SignalGen"+process+"/InitialQ_Jet_eta", J.Eta(),  1.,100, -5., 5,"VBF_J #eta");
  FillHist( "SignalGen"+process+"/InitialQ_Jet_pt",  J.Pt(), 1., 200, 0., 400.,"VBF_J p_{T} (GeV)");

  FillHist( "SignalGen"+process+"/Neutrino_pt", N.Pt(), 1., 200, 0., 400.,"N p_{T} (GeV)");
  FillHist( "SignalGen"+process+"/Neutrino_mass", N.M(), 1., 200, 0., 2000.,"m_N (GeV)");
  FillHist( "SignalGen"+process+"/Neutrino_eta", N.Eta(), 1.,100, -5., 5,"N #eta");

  FillHist( "SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_pt",  LepFromN.Pt(), 1., 200, 0., 400.,"#ell_N p_{T} (GeV)");
  FillHist( "SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_eta", LepFromN.Eta(),  1.,100, -5., 5,"#ell_N #eta");
  FillHist( "SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_DeltaR_W_From_Neutrino", W2.DeltaR(LepFromN), 1., 100, 0., 10.,"#Delta R(ln,W2)");
  FillHist( "SignalGen"+process+"/Mass_"+LepFl_l1+"_From_Neutrino_AND_W_FromNeutrino", (W2 + LepFromN).M(), 1., 150, 0., 1500.,"M(ln+W2) GeV");

  if (LepFromN.PID() < 0){

    FillHist( "SignalGenPlusQ"+process+"/Jet_FromW_eta", j1.Eta(),  1.,100, -5., 5,"J #eta");
    FillHist( "SignalGenPlusQ"+process+"/Jet_FromW_eta", j2.Eta(),  1.,100, -5., 5,"J #eta");
    FillHist( "SignalGenPlusQ"+process+"/Jet_FromW_pt",  j1.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
    FillHist( "SignalGenPlusQ"+process+"/Jet_FromW_pt",  j2.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
    FillHist( "SignalGenPlusQ"+process+"/W_Dijet_mass",  W2.M(),  1., 200, 0., 200.,"W(jj) Mass (GeV)");

    FillHist( "SignalGenPlusQ"+process+"/InitialQ_Jet_eta", J.Eta(),  1.,100, -5., 5,"VBF_J #eta");
    FillHist( "SignalGenPlusQ"+process+"/InitialQ_Jet_pt",  J.Pt(), 1., 200, 0., 400.,"VBF_J p_{T} (GeV)");

    FillHist( "SignalGenPlusQ"+process+"/Neutrino_pt", N.Pt(), 1., 200, 0., 400.,"N p_{T} (GeV)");
    FillHist( "SignalGenPlusQ"+process+"/Neutrino_mass", N.M(), 1., 200, 0., 2000.,"m_N (GeV)");
    FillHist( "SignalGenPlusQ"+process+"/Neutrino_eta", N.Eta(), 1.,100, -5., 5,"N #eta");

    FillHist( "SignalGenPlusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_pt",  LepFromN.Pt(), 1., 200, 0., 400.,"#ell_N p_{T} (GeV)");
    FillHist( "SignalGenPlusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_eta", LepFromN.Eta(),  1.,100, -5., 5,"#ell_N #eta");
    FillHist( "SignalGenPlusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_DelaR_W_From_Neutrino", W2.DeltaR(LepFromN), 1., 100, 0., 10.,"#Delta R(ln,W)");
    FillHist( "SignalGenPlusQ"+process+"/Mass_"+LepFl_l1+"_From_Neutrino_AND_W_FromNeutrino", (W2 + LepFromN).M(), 1., 150, 0., 1500.,"M(ln+W2) GeV");
   
  }
  else{
    FillHist( "SignalGenMinusQ"+process+"/Jet_FromW_eta", j1.Eta(),  1.,100, -5., 5,"J #eta");
    FillHist( "SignalGenMinusQ"+process+"/Jet_FromW_eta", j2.Eta(),  1.,100, -5., 5,"J #eta");
    FillHist( "SignalGenMinusQ"+process+"/Jet_FromW_pt",  j1.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
    FillHist( "SignalGenMinusQ"+process+"/Jet_FromW_pt",  j2.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
    FillHist( "SignalGenMinusQ"+process+"/W_Dijet_mass",  W2.M(),  1., 200, 0., 200.,"W(jj) Mass (GeV)");

    FillHist( "SignalGenMinusQ"+process+"/InitialQ_Jet_eta", J.Eta(),  1.,100, -5., 5,"VBF_J #eta");
    FillHist( "SignalGenMinusQ"+process+"/InitialQ_Jet_pt",  J.Pt(), 1., 200, 0., 400.,"VBF_J p_{T} (GeV)");

    FillHist( "SignalGenMinusQ"+process+"/Neutrino_pt", N.Pt(), 1., 200, 0., 400.,"N p_{T} (GeV)");
    FillHist( "SignalGenMinusQ"+process+"/Neutrino_mass", N.M(), 1., 200, 0., 2000.,"m_N (GeV)");
    FillHist( "SignalGenMinusQ"+process+"/Neutrino_eta", N.Eta(), 1.,100, -5., 5,"N #eta");

    FillHist( "SignalGenMinusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_pt",  LepFromN.Pt(), 1., 200, 0., 1000.,"#ell_N p_{T} (GeV)");
    FillHist( "SignalGenMinusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_eta", LepFromN.Eta(),  1.,100, -5., 5,"#ell_N #eta");
    FillHist( "SignalGenMinusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_DelaR_W_From_Neutrino", W2.DeltaR(LepFromN), 1., 100, 0., 10.,"#Delta R(ln,W)");
    FillHist( "SignalGenMinusQ"+process+"/Mass_"+LepFl_l1+"_From_Neutrino_AND_W_FromNeutrino", (W2 + LepFromN).M(), 1., 150, 0., 1500.,"M(ln+W2) GeV");
  }

  FillHist( "SignalGen"+process+"/" + mu_ch+"Lep_"+LepFl_l2+"_FromW_pt", LepFromW.Pt(), 1., 200, 0., 400.,"#ell_w p_{T} (GeV)");
  FillHist( "SignalGen"+process+"/" + mu_ch+"Lep_"+LepFl_l2+"_FromW_eta", LepFromW.Eta(),  1.,100, -5., 5,"#ell_w #eta");

  FillHist("SignalGen"+process+"/Nlep_Wlep_pt", LepFromN.Pt(), LepFromW.Pt(), 1., 500,0., 2000.,500, 0.,2000.);

  FillHist("SignalGen"+process+"/DeltaR_Lep_FromN_N",  N.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(N,ln)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromN_W",  W2.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(W2,ln)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromW_N",  N.DeltaR(LepFromW), 1., 200, 0., 5.,"#DeltaR(N,lw)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromW_W",  W2.DeltaR(LepFromW), 1., 200, 0., 5.,"#DeltaR(W2,lw)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromN_J1",  j1.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(j1,ln)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromN_J2",  j2.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(j2,ln)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromW_LepFromN",  LepFromW.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(lw,ln)");
  FillHist("SignalGen"+process+"/DeltaR_W_N",  W2.DeltaR(N), 1., 200, 0., 5.,"#DeltaR(N,W2)");


  FillHist("SignalGen"+process+"/DeltaPhi_Lep_FromN_N",  fabs(TVector2::Phi_mpi_pi( ( (N.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,ln)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep_FromN_W",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W2,ln)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep_FromW_N",  fabs(TVector2::Phi_mpi_pi( ( (N.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,lw)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep_FromW_W",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W2,lw)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep_FromW_LepFromN",  fabs(TVector2::Phi_mpi_pi( ( (LepFromN.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(lw,ln)");
  FillHist("SignalGen"+process+"/DeltaPhi_W_N",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - N.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,W2)");

  Particle WrongN = LepFromW + j1+j2;
  FillHist("SignalGen"+process+"/DeltaPhiWr_Lep_FromN_N",  fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(wN,ln)");
  FillHist("SignalGen"+process+"/DeltaPhiWr_Lep_FromW_N",  fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(wN,lw)");

  Particle W1 = (N+LepFromW);
  FillHist("SignalGen"+process+"/DeltaPhi_W1_Lep_FromN",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,ln)");
  FillHist("SignalGen"+process+"/DeltaPhi_W1_Lep_FromW",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,ln)");
  FillHist("SignalGen"+process+"/DeltaPhi_W1_N",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - N.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,W1)");
  FillHist("SignalGen"+process+"/DeltaPhi_W1_W2",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - W2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,W2)");
  FillHist("SignalGen"+process+"/Mass_NpWl",  (N+LepFromW).M(), 1., 200, 0., 2000.,"M(N,Wl) GeV");


  
  vector<Particle> JetsColl = {J,j1,j2};
  double maxDiJetDeta=0.;
  int ijet1(-1), ijet2(-1);
  for(unsigned int ij = 0; ij < JetsColl.size()-1; ij++){
    for(unsigned int ij2 = ij+1; ij2 < JetsColl.size(); ij2++){

      double deta = fabs(JetsColl[ij].Eta() - JetsColl[ij2].Eta());
      if(deta > maxDiJetDeta) {
        maxDiJetDeta=deta;
        ijet1=ij;
        ijet2=ij2;
      }
    }
  }
  

  Particle JJMEta = JetsColl[ijet1] + JetsColl[ijet2];

  FillHist("SignalGen"+process+"/MaxDEtaVBF_Jets_MJJ",JJMEta.M()   , 1, 100, 0., 4000., "MaxDEta MJJ (GeV)");

  double Av_JetEta= 0.5*(JetsColl[ijet1].Eta()+ JetsColl[ijet2].Eta());
  double zeppenfeld = TMath::Max(LepFromW.Eta()  - Av_JetEta , LepFromN.Eta()  - Av_JetEta ) /maxDiJetDeta;
  FillHist( "SignalGen"+process+"/MaxDEtaVBF_Jets_zeppenfeld", zeppenfeld  , 1, 200, 0., 2., "zeppenfeld");


  double ZeroCheck = LepFromN.Pt()*LepFromW.Pt()*N.Pt()*W2.Pt()*J.Pt() * j1.Pt()*j1.Pt();
  if(ZeroCheck == 0.){
    cout << "LepFromN.Pt()  " << LepFromN.Pt() << endl;
    cout << "LepFromW.Pt()  " << LepFromW.Pt() << endl;
    cout << "N.Pt()  " << N.Pt() << endl;
    cout << "W2.Pt()  " << W2.Pt() << endl;
    cout << "Init Q Pt()  " << J.Pt() << endl;
    cout << "j1.Pt()  " << j1.Pt() << endl;
    cout << "j2.Pt()  " << j2.Pt() << endl;
    
    PrintGen(All_Gens);
    exit(EXIT_FAILURE);
  }
  if(LepFromN.Pt() == 0 || LepFromW.Pt() == 0.) {
    cout << "LepFromN pt = " << LepFromN.Pt() << "  LepFromW pt = " << LepFromW.Pt() << endl;
    PrintGen(All_Gens);
    exit(EXIT_FAILURE);
  }

  return ;
}



void HNL_GenericObject_Studies::MakeType1SignalPlots(TString process, bool apply_reco_cut){
  
  int N_Mother_ind(-1); // Index of N
  int W2_ind(0); // Index of W2 : i.e W from N decay 
  TString  mu_ch="";

  if(apply_reco_cut) process=process+"_PtEtaReq";
  

  // First loop over gen collection to get index of W/N and apply pt/eta cuts if needed
  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);
    
    if(All_Gens.at(gen.MotherIndex()).PID() == 9900012 || All_Gens.at(gen.MotherIndex()).PID() == 9900014){

      if(apply_reco_cut){
	if( (fabs(gen.PID()) == 13 )|| (fabs(gen.PID()) == 11 )){
	  if(gen.Pt() < 10. || fabs(gen.Eta()) > 2.4) return;
	}
      }
    }
   
    
    if(fabs(gen.PID()) == 24 && (All_Gens.at(gen.MotherIndex()).PID() == 9900012 || All_Gens.at(gen.MotherIndex()).PID() == 9900014)){
      W2_ind= i;
      // If W decays to W in gen then get daughter
      
      for(unsigned int i2=2; i2<All_Gens.size(); i2++){
	Gen gen2 = All_Gens.at(i2);
	if(gen2.MotherIndex() == W2_ind){
	  if (fabs(gen2.PID()) == 24 ) W2_ind = i2;
	}
      }
    }
    
    if((gen.PID() == 9900012 || gen.PID() == 9900014) &&  gen.Status() == 22) {
      N_Mother_ind= gen.MotherIndex();
    }
  }

  // Loop over again to cut on Lep from W1
  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);
    
    if(gen.MotherIndex() == N_Mother_ind){
      if(apply_reco_cut){
        if( (fabs(gen.PID()) == 13 )|| (fabs(gen.PID()) == 11 )){
          if(gen.Pt() < 10. || fabs(gen.Eta()) > 2.4) return;
        }
      }
    }
  }

  
  Gen LepFromN;
  Gen LepFromW;
  Gen N;
  Gen W2;
  Gen j1,j2;
  bool j1IsSet(false);

  TString LepFl_l1, LepFl_l2;
  
  // Loop over Gen collection and Natch N/W/l/j objects
  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);
    
    if(int(gen.MotherIndex()) == W2_ind && gen.Status() == 23) {
      W2 = All_Gens.at(gen.MotherIndex());
      if(!j1IsSet) {  j1= gen; j1IsSet=true;}
      else j2 = gen;
    }
    if(gen.PID() == 9900012 || gen.PID() == 9900014){
      N= All_Gens.at(i);
    }
    
    if( ! ( ( fabs(gen.PID()) == 13)  || (fabs(gen.PID()) == 11) )) continue;
    
    TString LepFl = (fabs(gen.PID()) == 13) ? "Mu" : "El";

    if(All_Gens.at(gen.MotherIndex()).PID() == 9900012|| All_Gens.at(gen.MotherIndex()).PID() == 9900014) {
      LepFromN = gen;
      LepFl_l1=LepFl;
    }
    else if(gen.MotherIndex() == N_Mother_ind){
      LepFromW=gen;
      LepFl_l2=LepFl;
    }
  }
  
  if(LepFromN.Pt() == 0 || LepFromW.Pt() == 0.) {
    cout << "LepFromN pt = " << LepFromN.Pt() << "  LepFromW pt = " << LepFromW.Pt() << endl;
    PrintGen(All_Gens);
    exit(EXIT_FAILURE);
  }
  

  
  // PLOTS


  FillHist( "SignalGen"+process+"/Jet_FromW_eta", j1.Eta(),  1.,100, -5., 5,"J #eta");
  FillHist( "SignalGen"+process+"/Jet_FromW_eta", j2.Eta(),  1.,100, -5., 5,"J #eta");
  FillHist( "SignalGen"+process+"/Jet_FromW_pt",  j1.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
  FillHist( "SignalGen"+process+"/Jet_FromW_pt",  j2.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
  FillHist( "SignalGen"+process+"/W_Dijet_mass",  W2.M(),  1., 200, 0., 200.,"W(jj) Mass (GeV)");


  FillHist( "SignalGen"+process+"/Neutrino_pt", N.Pt(), 1., 200, 0., 400.,"N p_{T} (GeV)");
  FillHist( "SignalGen"+process+"/Neutrino_mass", N.M(), 1., 200, 0., 2000.,"m_N (GeV)");
  FillHist( "SignalGen"+process+"/Neutrino_eta", N.Eta(), 1.,100, -5., 5,"N #eta");

  FillHist( "SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_pt",  LepFromN.Pt(), 1., 200, 0., 400.,"#ell_N p_{T} (GeV)");
  FillHist( "SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_eta", LepFromN.Eta(),  1.,100, -5., 5,"#ell_N #eta");
  FillHist( "SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_DeltaR_W_From_Neutrino", W2.DeltaR(LepFromN), 1., 100, 0., 10.,"#Delta R(ln,W2)");
  FillHist( "SignalGen"+process+"/Mass_"+LepFl_l1+"_From_Neutrino_AND_W_FromNeutrino", (W2 + LepFromN).M(), 1., 150, 0., 1500.,"M(ln+W2) GeV");

  
  FillHist( "SignalGen"+process+"/Lep_"+LepFl_l2+"_FromW_pt", LepFromW.Pt(), 1., 200, 0., 400.,"#ell_w p_{T} (GeV)");
  FillHist( "SignalGen"+process+"/Lep_"+LepFl_l2+"_FromW_eta", LepFromW.Eta(),  1.,100, -5., 5,"#ell_w #eta ");
  
  
  FillHist("SignalGen"+process+"/Nlep_Wlep_pt", LepFromN.Pt(), LepFromW.Pt(), 1., 500,0., 2000.,500, 0.,2000.);

  if( (LepFromN.Pt() < 50) || ( LepFromW.Pt() < 50)){
    if(LepFromN.Pt() < LepFromW.Pt()) FillHist("SignalGen"+process+"/Pt_LT50_NOrderPt", 1, 1, 2, 0, 2);
    else FillHist("SignalGen"+process+"/Pt_LT50_NOrderPt",0, 1, 2, 0, 2);
  }
  if( (LepFromN.Pt() < 100) || ( LepFromW.Pt() <100)){
    if(LepFromN.Pt() < LepFromW.Pt()) FillHist("SignalGen"+process+"/Pt_LT100_NOrderPt",1, 1, 2, 0, 2);
    else FillHist("SignalGen"+process+"/Pt_LT100_NOrderPt",0, 1, 2, 0, 2);
  }
  
  if(LepFromN.Pt() < LepFromW.Pt()) FillHist("SignalGen"+process+"/Pt_NOrderPt",1, 1, 2, 0, 2);
  else FillHist("SignalGen"+process+"/Pt_NOrderPt",0, 1, 2, 0, 2);
  
  if(j1.Pt() == 0  || j2.Pt() == 0.)      {
    cout << "J1 pt = " << j1.Pt() << " J2 pt = " << j2.Pt() << endl;   
    PrintGen(All_Gens);
    exit(EXIT_FAILURE);
  }


  //cout << "LepFromN pt = " << LepFromN.Pt() << "  LepFromW pt = " << LepFromW.Pt() << endl;

  double dphi1 = fabs(TVector2::Phi_mpi_pi( ( ((LepFromN + j1+j2).Phi() - LepFromW.Phi() ))));
  double dphi2 = fabs(TVector2::Phi_mpi_pi( ( ((LepFromW + j1+j2).Phi() - LepFromN.Phi() ))));
  //double Nphi1 =  N.Phi();
  //double Nphi2 =  (LepFromN + j1+j2).Phi(); 
  
  // if( Nphi1 !=  Nphi2) {
  //  
  //  cout << Nphi1 << " " << Nphi2 << endl;
  //  cout << (LepFromN + j1+j2).Phi() << " " << N.Phi() << endl;
  //  cout << (LepFromN + j1+j2).M() << " " << N.M() << endl;
  //  cout << (j1+j2).M() << " " << W2.M() << endl;
  //  PrintGen(All_Gens);
  //  exit(EXIT_FAILURE);
  // }


  if(dphi1 > dphi2) FillHist("SignalGen"+process+"/DPhi_NOrder_",1, 1, 2, 0, 2);
  else FillHist("SignalGen"+process+"/DPhi_NOrder_",0, 1, 2, 0, 2);
  
  if(W2.DeltaR(LepFromW) > W2.DeltaR(LepFromN)) FillHist("SignalGen"+process+"/DPhiW_NOrder_",1, 1, 2, 0, 2);
  else FillHist("SignalGen"+process+"/DPhiW_NOrder_",0, 1, 2, 0, 2);

  


  FillHist("SignalGen"+process+"/DeltaR_Lep_FromN_N",  N.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(N,ln)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromN_W",  W2.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(W2,ln)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromW_N",  N.DeltaR(LepFromW), 1., 200, 0., 5.,"#DeltaR(N,lw)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromW_W",  W2.DeltaR(LepFromW), 1., 200, 0., 5.,"#DeltaR(W2,lw)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromN_J1",  j1.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(j1,ln)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromN_J2",  j2.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(j2,ln)");
  FillHist("SignalGen"+process+"/DeltaR_Lep_FromW_LepFromN",  LepFromW.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(lw,ln)");
  FillHist("SignalGen"+process+"/DeltaR_W_N",  W2.DeltaR(N), 1., 200, 0., 5.,"#DeltaR(N,W2)");


  FillHist("SignalGen"+process+"/DeltaPhi_Lep_FromN_N",  fabs(TVector2::Phi_mpi_pi( ( (N.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,ln)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep_FromN_W",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W2,ln)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep_FromW_N",  fabs(TVector2::Phi_mpi_pi( ( (N.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,lw)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep_FromW_W",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W2,lw)");
  FillHist("SignalGen"+process+"/DeltaPhi_Lep_FromW_LepFromN",  fabs(TVector2::Phi_mpi_pi( ( (LepFromN.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(lw,ln)");
  FillHist("SignalGen"+process+"/DeltaPhi_W_N",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - N.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,W2)");

  Particle WrongN = LepFromW + j1+j2;
  FillHist("SignalGen"+process+"/DeltaPhiWr_Lep_FromN_N",  fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(wN,ln)");
  FillHist("SignalGen"+process+"/DeltaPhiWr_Lep_FromW_N",  fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(wN,lw)");

  Particle W1 = (N+LepFromW);
  FillHist("SignalGen"+process+"/DeltaPhi_W1_Lep_FromN",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,ln)");
  FillHist("SignalGen"+process+"/DeltaPhi_W1_Lep_FromW",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,ln)");
  FillHist("SignalGen"+process+"/DeltaPhi_W1_N",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - N.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,W1)");
  FillHist("SignalGen"+process+"/DeltaPhi_W1_W2",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - W2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,W2)");
  FillHist("SignalGen"+process+"/Mass_NpWl",  (N+LepFromW).M(), 1., 200, 0., 2000.,"M(N,Wl) GeV");


  if(fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - LepFromN.Phi() ))))  >  N.DeltaR(LepFromN) ) FillHist("SignalGen"+process+"/DPhiWrN_NOrder_",1, 1, 2, 0, 2);
  else FillHist("SignalGen"+process+"/DPhiWrN_NOrder_",0, 1, 2, 0, 2);

  return;

}


void HNL_GenericObject_Studies::CheckHEMIssue(HNL_LeptonCore::Channel channel,std::vector<Lepton *> LepsT,std::vector<Lepton *> LepsV,       vector<Jet>& JetColl, Particle& vMET, float weight, TString Label, TString Option)
{

  if(DataYear!=2018) return;

  if( !(LepsT.size()==2)) return;
  if( !(LepsV.size()==2)) return;
  
  /* 
     FIX
  if( DataYear==2016 or DataYear==2018 ){ if(LepsT.at(0).Pt()<26) return; }
  else{                                   if(LepsT.at(0).Pt()<29) return; }
  
  const int NEtaEdges=5, NPhiEdges=9;
  double EtaEdges_p[NEtaEdges]={0., 0.6, 1.3, 1.9, 2.5};
  double PhiEdges[NPhiEdges]={-3.15, -2.4, -1.6, -0.8, 0., 0.8, 1.6, 2.4, 3.15};
  float mu2_feta=fabs(LepsT.at(1).Eta()), mu2_phi = LepsT.at(1).Phi();
  
  if(mu2_feta>0){ FillHist(LepsT[1]->GetFlavour()+"2_fEtaPhi_p"+Label, mu2_feta, mu2_phi, weight, NEtaEdges-1, EtaEdges_p, NPhiEdges-1, PhiEdges); }
  else{           FillHist(LepsT[1]->GetFlavour()"2_fEtaPhi_m"+Label, mu2_feta, mu2_phi, weight, NEtaEdges-1, EtaEdges_p, NPhiEdges-1, PhiEdges); }
  
  if(IsDATA && IsHEMIssueRun()){
    if(mu2_feta>0){ FillHist(LepsT[1]->GetFlavour()"2_fEtaPhi_p_HEMRun"+Label, mu2_feta, mu2_phi, weight, NEtaEdges-1, EtaEdges_p, NPhiEdges-1, PhiEdges); }
    else{           FillHist(LepsT[1]->GetFlavour()"2_fEtaPhi_m_HEMRun"+Label, mu2_feta, mu2_phi, weight, NEtaEdges-1, EtaEdges_p, NPhiEdges-1, PhiEdges); }
  }
  
  */
  return;

}
