#include "HNL_Signal_Studies.h"

void HNL_Signal_Studies::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();
  
}


void HNL_Signal_Studies::executeEvent(){

  AddTimerStamp("start_ev");
  
  FillTimer("START_EV");

  //==== Gen for genmatching
  
  Event ev = GetEvent();
  double weight = 1;//SetupWeight(ev,param);



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

  FillHist ("ObjectCount/SS_NoCut", 1, weight, 2, 0., 2.,"");

  FillTimer("SS_EV");

  AnalyzerParameter param1  = InitialiseHNLParameter("HNTightV2");
  
  std::vector<Electron>   ElectronCollV1 = GetElectrons(param1.Electron_Veto_ID, 10., 2.5);
  std::vector<Muon>       MuonCollV1     = GetMuons    (param1.Muon_Veto_ID, 5., 2.4);
  

  vector<HNL_LeptonCore::Channel> channels = {EE,MuMu};
  
  for(auto dilep_channel : channels){
    
    if(MCSample.Contains("Type")){
      if (!SelectChannel(dilep_channel)) continue;
    }
    AnalyzerParameter param  = InitialiseHNLParameter("HNL_ULID",dilep_channel);
    
    param.Channel  = GetChannelString(dilep_channel);
    
    weight = SetupWeight(ev,param);                                                                                                                                                             
    if(MCSample.Contains("WGToLNuG")) weight *= 0.5;
    
    
    TString channel = GetChannelString(dilep_channel) ;
    
    if(HasFlag("PlotBDT")){
      if(dilep_channel == MuMu) PlotBDTVariablesMuon(param);
      if(dilep_channel == EE)   PlotBDTVariablesElectron(param);
      return;
    }

    
    FillTimer("START_RUN_"+channel);
    
    
    //double ptbins[11] = { 0., 10.,15., 20., 30., 40.,50., 100.,500. ,1000., 2000.};

    if(HasFlag("SignalKinematics")){
      
      if(MCSample.Contains("DYType")   && MCSample.Contains("private"))  MakeType1SignalPlots(channel, false);
      if(MCSample.Contains("VBFType")  && MCSample.Contains("private"))  MakeType1VBFSignalPlots(channel, false);
      if(MCSample.Contains("SSWWType") && MCSample.Contains("private"))  MakeType1SSWWSignalPlots(channel, false);
      
    }
    
    std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, 10., 2.5);
    std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID, 5., 2.4);
    
    std::vector<Electron>   ElectronCollT = GetElectrons( param.Electron_Tight_ID, 10., 2.5);
    std::vector<Muon>       MuonCollT     = GetMuons    ( param.Muon_Tight_ID, 5., 2.4);

    std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
    std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
    std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
    std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
    std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets("VBFTight",  param);
    std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);

    for(long unsigned int imu =0 ; imu <  MuonCollT.size(); imu++) MuonCollT[imu].SetHTOverPt(GetHT(AK4_JetCollLoose,AK8_JetColl));
    for(long unsigned int iel =0 ; iel <  ElectronCollT.size() ; iel++) ElectronCollT[iel].SetHTOverPt(GetHT(AK4_JetCollLoose,AK8_JetColl));


    std::vector<Lepton *> LepsT       = MakeLeptonPointerVector(MuonCollT,ElectronCollT);
    std::vector<Lepton *> LepsV  = MakeLeptonPointerVector(MuonCollV,ElectronCollV);


    std::vector<Tau> TauColl                        = SelectTaus(LepsV,param.Tau_Veto_ID,20., 2.3);
    
    Particle METv = GetvMET("PuppiT1xyULCorr", param, AK4_VBF_JetColl, AK8_JetColl, MuonCollT, ElectronCollT);

    // returns MET with systematic correction; run this after all object selection done; NOTE that VBF jet is used here

    if(HasFlag("CheckMuonInEff")){
      //      cout << "------------------------------" << endl;
      if (IsData || ( process.Contains("Mu+Mu+") or   process.Contains("Mu-Mu-")  or process.Contains("MuMu"))){
	vector<Gen> gen_lep= GetGenLepronsSignal();
	for(auto i : gen_lep) {
	  //	  cout << "Gen muon Pt=" << i.Pt() << " eta = " << i.Eta() << " phi = " << i.Phi() << endl;
	  FillHist("GenMuon/All_Eta", i.Eta() , weight, 100, -5., 5);
	  FillHist("GenMuon/All_Phi", i.Phi() , weight, 100, -5., 5);
	  FillHist("GenMuon/All_Pt", i.Pt() , weight, 100, 0, 2000);

	  bool Matched=false;
	  for(auto imu : MuonCollT){
	    if(i.DeltaR(imu) < 0.1)     Matched=true;
	  }
	  if(Matched){
	    
	    FillHist("GenMuon/Matched_Eta", i.Eta() , weight, 100, -5., 5);
	    FillHist("GenMuon/Matched_Phi", i.Phi() , weight, 100, -5., 5);
	    FillHist("GenMuon/Matched_Pt", i.Pt() , weight, 100, 0, 2000);
	  }
	  else{
	    FillHist("GenMuon/NonMatched_Eta", i.Eta() , weight, 100, -5., 5);
	    FillHist("GenMuon/NonMatched_Phi", i.Phi() , weight, 100, -5., 5);
	    FillHist("GenMuon/NonMatched_Pt", i.Pt() , weight, 100, 0, 2000);
	  }
	}
      }
      return;
    }

    if(HasFlag("WWJet")){
      
      if(AK4_VBF_JetColl.size() < 2) return ;

      double maxDiJetDeta=0.;
      int ijet1(-1), ijet2(-1);
      for(unsigned int ij = 0; ij < AK4_VBF_JetColl.size()-1; ij++){
	for(unsigned int ij2 = ij+1; ij2 < AK4_VBF_JetColl.size(); ij2++){

	  double deta = fabs(AK4_VBF_JetColl[ij].Eta() - AK4_VBF_JetColl[ij2].Eta());
	  if(deta > maxDiJetDeta) {
	    maxDiJetDeta=deta;
	    ijet1=ij;
	    ijet2=ij2;
	  }
	}
      }
      
      /// CHeck GEN Matched
      vector<Particle> GenWWJets = GetJetWW();
      if(GenWWJets.size() != 2) return;
      bool Method_PtOrder_Match=false;
      bool Method_dEta_Match=false;
      
      Particle J1 = GenWWJets[0];
      Particle J2 = GenWWJets[1];
      
      if(J1.DeltaR(AK4_VBF_JetColl[0]) < 0.2){
	if(J2.DeltaR(AK4_VBF_JetColl[1]) < 0.2){
	  Method_PtOrder_Match=true;
	}
      }
      else   if(J2.DeltaR(AK4_VBF_JetColl[0]) < 0.2){
	if(J1.DeltaR(AK4_VBF_JetColl[1]) < 0.2){
          Method_PtOrder_Match=true;
        } 
      } 

      
      if(J1.DeltaR(AK4_VBF_JetColl[ijet1]) < 0.2){
	if(J2.DeltaR(AK4_VBF_JetColl[ijet2]) < 0.2){
          Method_dEta_Match=true;
        } 
      } 
      else   if(J2.DeltaR(AK4_VBF_JetColl[ijet1]) < 0.2){
	if(J1.DeltaR(AK4_VBF_JetColl[ijet2]) < 0.2){
          Method_dEta_Match=true;
        }
      }

      
      FillHist("SSWWJetMatch/JetMatchEff", 0 , weight, 5, 0., 5);
      if(Method_PtOrder_Match)FillHist("SSWWJetMatch/JetMatchEff", 1 , weight, 5, 0., 5);
      if(Method_dEta_Match)FillHist("SSWWJetMatch/JetMatchEff", 2 , weight, 5, 0., 5);


      return;
    }

    
    TString def_paramName =param.Name;
    param.DefName=param.Name;

    param.NameInclusive_Channel = param.Name  + "/"+param.InclusiveChannelName();

    
    if (IsData || ( process.Contains("Mu+Mu+") or   process.Contains("Mu-Mu-") )){
      RunLeptonChannel(MuMu,LepsT, LepsV,  TauColl, AK4_JetCollLoose, AK4_JetColl, AK4_VBF_JetColl, AK8_JetColl,AK4_BJetColl, ev, param, weight );
    }
    
    param.Name=def_paramName;
    param.DefName=param.Name;

    if (IsData || ( process.Contains("El+El+") or   process.Contains("El-El-") )){
      RunLeptonChannel(EE,LepsT, LepsV,  TauColl, AK4_JetCollLoose, AK4_JetColl, AK4_VBF_JetColl, AK8_JetColl,AK4_BJetColl, ev, param, weight );
    }    
           
  }
  
  return;
  FillTimer("END_Run");
  
  
}


void HNL_Signal_Studies::PlotBDTVariablesMuon(AnalyzerParameter param){

  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);

  std::vector<Electron>   ElectronCollProbe = GetElectrons("passPOGTight", 10., 2.5);
  std::vector<Muon>       MuonCollProbe     = GetMuons    ("passProbe", 10., 2.4);

  HNL_LeptonCore::Channel dilep_channel = MuMu;

  TString channel_string = GetChannelString(dilep_channel) ;
  vector<TString>  IDList = {"HNTightV2",
                             "MVAID",
                             "HNL_ULID_2017"};


  if(SameCharge(MuonCollProbe)) {
    if(MuonCollProbe[0].Pt() > 20 &&  MuonCollProbe[1].Pt() > 10) {

      for (auto ID: IDList){
	if(MuonCollProbe[0].PassID(ID) && MuonCollProbe[1].PassID(ID))     FillHist("ID/"+ID+"_"+channel_string+"_nev" , 1 , weight, 2, 0., 2);
      }
    }
  }
  else return ;


  for(auto imu : MuonCollProbe){



    if(!imu.PassID("MVALoose")) continue;

    TString tag= imu.LepGenTypeString();
    
    //cout << "Muon Pt = " << imu.Pt() << " eta = " << imu.Eta() << endl;
    
    TString etabin = (fabs(imu.Eta()) < 1.5) ? "BB_" : "EC_";
    TString ptbin = "Ptlt20_";
    if(imu.Pt() < 15) ptbin = "Ptlt15_";
    else if(imu.Pt() < 20) ptbin = "Ptlt20_";
    else if(imu.Pt() < 50) ptbin = "Ptlt50_";
    else  ptbin = "Ptgt50_";


    vector<TString> Tags = {tag+"/Lep_MVA_",
                            tag+imu.CloseJet_Flavour()+"/Lep_MVA_",
                            tag+etabin+"/Lep_MVA_",
                            tag+etabin+imu.CloseJet_Flavour()+"/Lep_MVA_",
                            tag+etabin+ptbin+"/Lep_MVA_",
                            tag+etabin+ptbin+imu.CloseJet_Flavour()+"/Lep_MVA_",
    };
    
    for(auto i  : Tags){

      if(i.Contains("Pileup")) continue;
      if(i.Contains("IsEWtau")) continue;
      if(i.Contains("LF") && !i.Contains("IsFake")) continue;
      if(i.Contains("HF") && !i.Contains("IsFake")) continue;

      map<TString, double> mapBDT = imu.MAPBDT();

      for(auto imap : mapBDT )	{

	FillHist("PlotBDTVariables/"+channel_string+i+"_"+imap.first, imap.second , weight, 200, -1., 1);
	if(!imu.PassID("HNL_ULID_2017") )     FillHist("PlotBDTVariablesFail/"+channel_string+i+"_"+imap.first+"_"+channel_string, imap.second , weight, 200, -1., 1);

	if(SameCharge(MuonCollProbe)) {
	  FillHist("PlotBDTVariables/SS_"+channel_string+i+"_"+imap.first, imap.second , weight, 200, -1., 1);
	}
      }

      if(SameCharge(MuonCollProbe)) FillHist("PlotBDTVariables/SS_"+channel_string+i+"Fake_"+channel_string, imu.MVA() , weight, 200, -1., 1);
   

      
    }
  }
}

void HNL_Signal_Studies::PlotBDTVariablesElectron(AnalyzerParameter param){

  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);
  
  std::vector<Electron>   ElectronCollProbe = GetElectrons("passProbeID", 10., 2.5);

  HNL_LeptonCore::Channel dilep_channel= EE;

  vector<TString>  IDList = {"HNTightV2",
                             "MVAID",
                             "HNL_ULID_2017"};
  
  
  TString channel_string = GetChannelString(dilep_channel) ;
  if(SameCharge(ElectronCollProbe)) {
    if(ElectronCollProbe[0].Pt() > 25 &&  ElectronCollProbe[1].Pt() > 15) {

      
      for (auto ID: IDList){
        if(ElectronCollProbe[0].PassID(ID) && ElectronCollProbe[1].PassID(ID))     FillHist("ID/"+ID+"_"+channel_string+"_nev" , 1 , weight, 2, 0., 2);
      }
    }
  }
  else return;


  for(auto iel : ElectronCollProbe){

    if(!iel.PassID("MVALoose")) continue;

    TString tag= iel.LepGenTypeString();
    if(iel.LeptonIsCF()) tag = tag +"_IsCF";
    
    TString etabin = (fabs(iel.Eta()) < 1.5) ? "BB_" : "EC_";
    TString ptbin = "Ptlt20_";
    if(iel.Pt() < 15) ptbin = "Ptlt15_";
    else if(iel.Pt() < 20) ptbin = "Ptlt20_";
    else if(iel.Pt() < 50) ptbin = "Ptlt50_";
    else  ptbin = "Ptgt50_";
    
    vector<TString> Tags = {tag+"/El_MVA_",
			    tag+iel.CloseJet_Flavour()+"/El_MVA_",
			    tag+etabin+"/El_MVA_",
                            tag+etabin+iel.CloseJet_Flavour()+"/El_MVA_",
			    tag+etabin+ptbin+"/El_MVA_",
                            //tag+etabin+ptbin+iel.CloseJet_Flavour()+"/El_MVA_",
		    
    };

    for(auto i  : Tags){

      if(i.Contains("Pileup")) continue;
      if(i.Contains("IsEWtau")) continue;
      if(i.Contains("IsCF") && !i.Contains("IsPrompt")) continue;
      if(i.Contains("LF") && !i.Contains("IsFake")) continue;
      if(i.Contains("HF") && !i.Contains("IsFake")) continue;

      map<TString, double> mapBDT = iel.MAPBDT();

      for(auto imap : mapBDT ){

	FillHist("PlotBDTVariables/"+channel_string+i+"_"+imap.first+"_"+channel_string, imap.second , weight, 200, -1., 1);

	if(iel.PassID("HNL_ULID_2017") ) 
	  FillHist("PlotBDTVariables/HNL_ULID_2017/"+channel_string+i+"_"+imap.first+"_"+channel_string, imap.second , weight, 200, -1., 1);

	else           FillHist("PlotBDTVariablesl/Fail_HNL_ULID_2017/"+channel_string+i+"_"+imap.first+"_"+channel_string, imap.second , weight, 200, -1., 1);
	
	
	if(SameCharge(ElectronCollProbe)){
	    FillHist("PlotBDTVariables/SS_"+channel_string+i+"_"+imap.first+"_"+channel_string, imap.second , weight, 200, -1., 1);
	    
	}
      }
    } 
  }
  
  return;
}
void HNL_Signal_Studies::RunLeptonChannel(HNL_LeptonCore::Channel channel_ID, std::vector<Lepton *> LepsT,std::vector<Lepton *> LepsV, std::vector<Tau> TauColl,  std::vector<Jet> JetCollLoose, std::vector<Jet> JetColl, std::vector<Jet> VBFJetColl,  std::vector<FatJet> FatjetColl,std::vector<Jet> BJetColl,   Event Ev, AnalyzerParameter param,  double _weight)   {


  FillHist (GetChannelString(channel_ID)+"_NoCut", 1, _weight, 2, 0., 2.,"");

  HNL_LeptonCore::SearchRegion Region1 = sigmm;


  if(channel_ID == EE) {  Region1 = sigee;}
  if(channel_ID == EMu){ Region1 = sigem;}
  
  Particle METv =GetMiniAODvMET("PuppiT1xyULCorr");


  int NBJetLV = 0;
  for(auto ib : BJetColl){
    bool hasCloseLep=false;
    for(auto il : LepsV) {
      if(il->DeltaR(ib) < 0.4) hasCloseLep=true;
    }
    if(!hasCloseLep) NBJetLV++;
  }
  bool PassBJetMVeto = (BJetColl.size()==0);

  // Update NAME for Muon channel                                                                                               

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
	  FillCutflow(Region1,_weight, "SSGen", param);
	  FillCutflow(Region1,_weight, "SSGen2",param);
	  

	}
	else       if( (fabs(gen_lep[0].Eta()) < 2.5 && gen_lep[0].Pt() > 30) &&
		       (fabs(gen_lep[1].Eta()) < 2.5 && gen_lep[1].Pt() > 10)){
	  FillCutflow(Region1,_weight, "SSGen", param);
	}
	
	else return;
      }
      
      else       if(fabs(gen_lep[0].PID()) == 13 && fabs(gen_lep[1].PID()) ==13 ){

        if( (fabs(gen_lep[0].Eta()) < 2.4 && gen_lep[0].Pt() > 20) &&
            (fabs(gen_lep[1].Eta()) < 2.4 && gen_lep[1].Pt() > 10)){
          FillCutflow(Region1,_weight, "SSGen",param);
          FillCutflow(Region1,_weight, "SSGen2",param);

        }
        else       if( (fabs(gen_lep[0].Eta()) < 2.4 && gen_lep[0].Pt() > 30) &&
                       (fabs(gen_lep[1].Eta()) < 2.4 && gen_lep[1].Pt() > 5)){
          FillCutflow(Region1,_weight, "SSGen",param);
        }

        else return;
      }
      else       if(fabs(gen_lep[0].PID()) == 13 && fabs(gen_lep[1].PID()) ==11 ){

        if( (fabs(gen_lep[0].Eta()) < 2.4 && gen_lep[0].Pt() > 25) &&
            (fabs(gen_lep[1].Eta()) < 2.5 && gen_lep[1].Pt() > 15)){
          FillCutflow(Region1,_weight, "SSGen",param);
          FillCutflow(Region1,_weight, "SSGen2",param);
        }
        else       if( (fabs(gen_lep[0].Eta()) < 2.4 && gen_lep[0].Pt() > 30) &&
                       (fabs(gen_lep[1].Eta()) < 2.5 && gen_lep[1].Pt() > 10)){
          FillCutflow(Region1,_weight, "SSGen",param);
        }

        else return;
      }

      else       if(fabs(gen_lep[0].PID()) == 11 && fabs(gen_lep[1].PID()) ==13 ){

        if( (fabs(gen_lep[0].Eta()) < 2.5 && gen_lep[0].Pt() > 25) &&
            (fabs(gen_lep[1].Eta()) < 2.4 && gen_lep[1].Pt() > 15)){
          FillCutflow(Region1,_weight, "SSGen",param);
          FillCutflow(Region1,_weight, "SSGen2",param);

        }
        else       if( (fabs(gen_lep[0].Eta()) < 2.5 && gen_lep[0].Pt() > 30) &&
                       (fabs(gen_lep[1].Eta()) < 2.4 && gen_lep[1].Pt() > 5)){
          FillCutflow(Region1,_weight, "SSGen",param);

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


  Fill_RegionPlots(param,"HNL_Presel" ,  JetColl, FatjetColl, LepsT,  METv, nPV, _weight);

  if(GetLLMass(LepsT) > 10.) {
    
    param.Name = param.DefName + "/"+ GetChannelString(channel_ID);
    
    FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_LLMass",param);
    FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_Jet",param);
    
    if(PassBJetMVeto) {
      FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_BJet",param);

      if(FatjetColl.size() > 0){
	if(HNL_RegionDefinitions::RunSignalRegionAK8(true, channel_ID, Inclusive, LepsT, LepsV, TauColl,JetColl, FatjetColl, BJetColl, Ev, METv, param,  _weight )){
	  FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_SR1",param);
	}
      }
      else{
	
	bool passSR2 = HNL_RegionDefinitions::RunSignalRegionWW(true, channel_ID, Inclusive, LepsT, LepsV, TauColl,JetColl, FatjetColl, BJetColl, Ev, METv, param,  _weight );

	if(passSR2){
	  FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_SR2",param);
	}
	else{
	 
	  FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_SR3Inclusive",param);
	  
	  if(HNL_RegionDefinitions::RunSignalRegionAK4(true,channel_ID, Inclusive, LepsT, LepsV ,TauColl, JetColl, FatjetColl, BJetColl, Ev, METv, param,  _weight )) {
	    FillCutflow(Region1,_weight, "SS"+GetChannelString(channel_ID)+"_SR3",param);
	  }
	} // FAIL SR2                                                                                                       
      }// Fail SR1                                                                                                          
    }// Fail Tau                                                                                                            
  } // LLMASS      
  param.Name=param.DefName;


  return;

  
}


  


HNL_Signal_Studies::HNL_Signal_Studies(){


}
 
HNL_Signal_Studies::~HNL_Signal_Studies(){
  

  

}


vector<Particle> HNL_Signal_Studies::GetJetWW(){
  for(unsigned int i=2; i< All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);

    if( (fabs(gen.PID()) == 13 )|| (fabs(gen.PID()) == 11 )){
      if(gen.Pt() < 10. || fabs(gen.Eta()) > 2.4) return {};
      
    }
  }

  Gen j1,j2;
  int Lep_Mother_ind(-1);
  bool j1IsSet(false);

  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);
    if( ! ( ( fabs(gen.PID()) == 13)  || (fabs(gen.PID()) == 11) )) continue;
    if (gen.Status() == 23){
      TString LepFl = (fabs(gen.PID()) == 13) ? "Mu" : "El";
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
  
  return {j1,j2};
  
}

void HNL_Signal_Studies::MakeType1SSWWSignalPlots(TString process, bool apply_reco_cut){

  if(apply_reco_cut) process=process+"_PtEtaReq";

  for(unsigned int i=2; i< All_Gens.size(); i++){
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
  
  FillHist( "SignalProcess/SignalGen"+process+"/Jet1_eta", j1.Eta(),  1.,100, -5., 5,"#eta");
  FillHist( "SignalProcess/SignalGen"+process+"/Jet2_eta", j2.Eta(),  1.,100, -5., 5,"#eta");
  FillHist( "SignalProcess/SignalGen"+process+"/Jet_pt",  j1.Pt(), 1., 200, 0., 1000.,"pt (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Jet_pt",  j2.Pt(), 1., 200, 0., 1000.,"pt (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Dijet_mass",  (j1+j2).M(),  1., 50, 0., 2000.,"m(jj) GeV");


  FillHist( "SignalProcess/SignalGen"+process+"/Lep_"+LepFl_l1+"_pt",  Lep1.Pt(), 1., 200, 0., 1000.,"pt (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Lep_"+LepFl_l1+"_eta", Lep1.Eta(),  1.,100, -5., 5,"#eta");
  FillHist( "SignalProcess/SignalGen"+process+"/Lep_DeltaR_JJ", (j1+j2).DeltaR(Lep1), 1., 100, 0., 10.,"#DeltaR (jj,l1)");
  FillHist( "SignalProcess/SignalGen"+process+"/Lep_DeltaR_JJ", (j1+j2).DeltaR(Lep2), 1., 100, 0., 10.,"#DeltaR (jj,l2)");
  FillHist( "SignalProcess/SignalGen"+process+"/Mass_lep1_JJ", (j1+j2 + Lep1).M(), 1., 100, 0., 5000.,"M(l1jj) GeV");
  FillHist( "SignalProcess/SignalGen"+process+"/Mass_lep2_JJ", (j1+j2 + Lep2).M(), 1., 100, 0., 5000.,"M(l2jj) GeV");
  FillHist( "SignalProcess/SignalGen"+process+"/Mass_llJJ", (j1+j2 +Lep1+ Lep2).M(), 1., 100, 0., 5000.,"M(lljj) GeV ");

  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep1_J1",  fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep1.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (l1,j1)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep2_J1",  fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (l2,j1)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep1_J2",  fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep1.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (l1,j2)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep2_J2",  fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (l2,j2)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep1_Lep2",  fabs(TVector2::Phi_mpi_pi( ( (Lep1.Phi() - Lep2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (l,l)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_J1_J2",  fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - j2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi (j,j)");

  double max_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep1.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep2.Phi() )))) > max_dpiphi)  max_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep2.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep1.Phi() )))) > max_dpiphi)  max_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep1.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep2.Phi() )))) > max_dpiphi)  max_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep2.Phi() ))));
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_J_Max",  max_dpiphi,  1., 200, 0., 10.,"#Delta#phi max(l,j)");

  double min_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep1.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep2.Phi() )))) < min_dpiphi)  min_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j1.Phi() - Lep2.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep1.Phi() )))) < min_dpiphi)  min_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep1.Phi() ))));
  if (fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep2.Phi() )))) < min_dpiphi)  min_dpiphi = fabs(TVector2::Phi_mpi_pi( ( (j2.Phi() - Lep2.Phi() ))));
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_J_Min",  min_dpiphi,  1., 200, 0., 5.,"#Delta#phi min(l,j)");


  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep1_J1",  j1.DeltaR(Lep1), 1., 100, 0., 10.,"#DeltaR (j1,l1)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep2_J1",   j1.DeltaR(Lep2), 1., 100, 0., 10.,"#DeltaR (j1,l2)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep1_J2", j2.DeltaR(Lep1), 1., 100, 0., 10.,"#DeltaR (j2,l1)"); 
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep2_J2",  j2.DeltaR(Lep2), 1., 100, 0., 10.,"#DeltaR (j2,l2)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep1_Lep2",Lep2.DeltaR(Lep1), 1., 100, 0., 10.,"#DeltaR (l,l)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_J1_J2",  j1.DeltaR(j2), 1., 100, 0., 10.,"#DeltaR (j1,j2)");

  double max_dpiR = j1.DeltaR(Lep1);
  if (j1.DeltaR(Lep2) > max_dpiR) max_dpiR = j1.DeltaR(Lep2);
  if (j2.DeltaR(Lep1) > max_dpiR) max_dpiR = j2.DeltaR(Lep1);
  if (j2.DeltaR(Lep2) > max_dpiR) max_dpiR = j2.DeltaR(Lep2);

  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_J_Max",  max_dpiR,  1., 200, 0., 5.,"#DeltaR max(L,J");

  double min_dpiR = j1.DeltaR(Lep1);
  if (j1.DeltaR(Lep2) < min_dpiR) min_dpiR = j1.DeltaR(Lep2);
  if (j2.DeltaR(Lep1) < min_dpiR) min_dpiR = j2.DeltaR(Lep1);
  if (j2.DeltaR(Lep2) < min_dpiR) min_dpiR = j2.DeltaR(Lep2);

  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_J_Min",  min_dpiR,  1., 200, 0., 5.,"#DeltaR min(L,J)");



  Particle JJMEta = j1+j2;

  FillHist("SignalProcess/SignalGen"+process+"/MaxDEtaVBF_Jets_MJJ",JJMEta.M()   , 1, 100, 0., 4000., "MaxDEta MJJ");
  
  double maxDiJetDeta=fabs(j1.Eta() - j2.Eta());
  double Av_JetEta= 0.5*(j1.Eta()+ j2.Eta());
  double zeppenfeld = TMath::Max(Lep1.Eta()  - Av_JetEta , Lep2.Eta()  - Av_JetEta ) /maxDiJetDeta;
  FillHist( "SignalProcess/SignalGen"+process+"/MaxDEtaVBF_Jets_zeppenfeld", zeppenfeld  , 1, 200, 0., 2., "zeppenfeld");




  
  return ;
}




void HNL_Signal_Studies::MakeType1VBFSignalPlots(TString process, bool apply_reco_cut){

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
  
  
  FillHist( "SignalProcess/SignalGen"+process+"/Jet_FromW_eta", j1.Eta(),  1.,100, -5., 5,"J #eta");
  FillHist( "SignalProcess/SignalGen"+process+"/Jet_FromW_eta", j2.Eta(),  1.,100, -5., 5,"J #eta");
  FillHist( "SignalProcess/SignalGen"+process+"/Jet_FromW_pt",  j1.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Jet_FromW_pt",  j2.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/W_Dijet_mass",  W2.M(),  1., 200, 0., 200.,"W(jj) Mass (GeV)");

  FillHist( "SignalProcess/SignalGen"+process+"/InitialQ_Jet_eta", J.Eta(),  1.,100, -5., 5,"VBF_J #eta");
  FillHist( "SignalProcess/SignalGen"+process+"/InitialQ_Jet_pt",  J.Pt(), 1., 200, 0., 400.,"VBF_J p_{T} (GeV)");

  FillHist( "SignalProcess/SignalGen"+process+"/Neutrino_pt", N.Pt(), 1., 200, 0., 400.,"N p_{T} (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Neutrino_mass", N.M(), 1., 200, 0., 2000.,"m_N (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Neutrino_eta", N.Eta(), 1.,100, -5., 5,"N #eta");

  FillHist( "SignalProcess/SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_pt",  LepFromN.Pt(), 1., 200, 0., 400.,"#ell_N p_{T} (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_eta", LepFromN.Eta(),  1.,100, -5., 5,"#ell_N #eta");
  FillHist( "SignalProcess/SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_DeltaR_W_From_Neutrino", W2.DeltaR(LepFromN), 1., 100, 0., 10.,"#Delta R(ln,W2)");
  FillHist( "SignalProcess/SignalGen"+process+"/Mass_"+LepFl_l1+"_From_Neutrino_AND_W_FromNeutrino", (W2 + LepFromN).M(), 1., 150, 0., 1500.,"M(ln+W2) GeV");

  if (LepFromN.PID() < 0){

    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/Jet_FromW_eta", j1.Eta(),  1.,100, -5., 5,"J #eta");
    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/Jet_FromW_eta", j2.Eta(),  1.,100, -5., 5,"J #eta");
    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/Jet_FromW_pt",  j1.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/Jet_FromW_pt",  j2.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/W_Dijet_mass",  W2.M(),  1., 200, 0., 200.,"W(jj) Mass (GeV)");

    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/InitialQ_Jet_eta", J.Eta(),  1.,100, -5., 5,"VBF_J #eta");
    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/InitialQ_Jet_pt",  J.Pt(), 1., 200, 0., 400.,"VBF_J p_{T} (GeV)");

    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/Neutrino_pt", N.Pt(), 1., 200, 0., 400.,"N p_{T} (GeV)");
    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/Neutrino_mass", N.M(), 1., 200, 0., 2000.,"m_N (GeV)");
    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/Neutrino_eta", N.Eta(), 1.,100, -5., 5,"N #eta");

    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_pt",  LepFromN.Pt(), 1., 200, 0., 400.,"#ell_N p_{T} (GeV)");
    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_eta", LepFromN.Eta(),  1.,100, -5., 5,"#ell_N #eta");
    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_DelaR_W_From_Neutrino", W2.DeltaR(LepFromN), 1., 100, 0., 10.,"#Delta R(ln,W)");
    FillHist( "SignalProcess/SignalGenPlusQ"+process+"/Mass_"+LepFl_l1+"_From_Neutrino_AND_W_FromNeutrino", (W2 + LepFromN).M(), 1., 150, 0., 1500.,"M(ln+W2) GeV");
   
  }
  else{
    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/Jet_FromW_eta", j1.Eta(),  1.,100, -5., 5,"J #eta");
    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/Jet_FromW_eta", j2.Eta(),  1.,100, -5., 5,"J #eta");
    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/Jet_FromW_pt",  j1.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/Jet_FromW_pt",  j2.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/W_Dijet_mass",  W2.M(),  1., 200, 0., 200.,"W(jj) Mass (GeV)");

    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/InitialQ_Jet_eta", J.Eta(),  1.,100, -5., 5,"VBF_J #eta");
    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/InitialQ_Jet_pt",  J.Pt(), 1., 200, 0., 400.,"VBF_J p_{T} (GeV)");

    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/Neutrino_pt", N.Pt(), 1., 200, 0., 400.,"N p_{T} (GeV)");
    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/Neutrino_mass", N.M(), 1., 200, 0., 2000.,"m_N (GeV)");
    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/Neutrino_eta", N.Eta(), 1.,100, -5., 5,"N #eta");

    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_pt",  LepFromN.Pt(), 1., 200, 0., 1000.,"#ell_N p_{T} (GeV)");
    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_eta", LepFromN.Eta(),  1.,100, -5., 5,"#ell_N #eta");
    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_DelaR_W_From_Neutrino", W2.DeltaR(LepFromN), 1., 100, 0., 10.,"#Delta R(ln,W)");
    FillHist( "SignalProcess/SignalGenMinusQ"+process+"/Mass_"+LepFl_l1+"_From_Neutrino_AND_W_FromNeutrino", (W2 + LepFromN).M(), 1., 150, 0., 1500.,"M(ln+W2) GeV");
  }

  FillHist( "SignalProcess/SignalGen"+process+"/" + mu_ch+"Lep_"+LepFl_l2+"_FromW_pt", LepFromW.Pt(), 1., 200, 0., 400.,"#ell_w p_{T} (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/" + mu_ch+"Lep_"+LepFl_l2+"_FromW_eta", LepFromW.Eta(),  1.,100, -5., 5,"#ell_w #eta");

  FillHist("SignalProcess/SignalGen"+process+"/Nlep_Wlep_pt", LepFromN.Pt(), LepFromW.Pt(), 1., 500,0., 2000.,500, 0.,2000.);

  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromN_N",  N.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(N,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromN_W",  W2.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(W2,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromW_N",  N.DeltaR(LepFromW), 1., 200, 0., 5.,"#DeltaR(N,lw)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromW_W",  W2.DeltaR(LepFromW), 1., 200, 0., 5.,"#DeltaR(W2,lw)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromN_J1",  j1.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(j1,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromN_J2",  j2.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(j2,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromW_LepFromN",  LepFromW.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(lw,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_W_N",  W2.DeltaR(N), 1., 200, 0., 5.,"#DeltaR(N,W2)");


  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_FromN_N",  fabs(TVector2::Phi_mpi_pi( ( (N.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_FromN_W",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W2,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_FromW_N",  fabs(TVector2::Phi_mpi_pi( ( (N.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,lw)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_FromW_W",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W2,lw)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_FromW_LepFromN",  fabs(TVector2::Phi_mpi_pi( ( (LepFromN.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(lw,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_W_N",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - N.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,W2)");

  Particle WrongN = LepFromW + j1+j2;
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhiWr_Lep_FromN_N",  fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(wN,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhiWr_Lep_FromW_N",  fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(wN,lw)");

  Particle W1 = (N+LepFromW);
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_W1_Lep_FromN",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_W1_Lep_FromW",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_W1_N",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - N.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,W1)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_W1_W2",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - W2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,W2)");
  FillHist("SignalProcess/SignalGen"+process+"/Mass_NpWl",  (N+LepFromW).M(), 1., 200, 0., 2000.,"M(N,Wl) GeV");


  
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

  FillHist("SignalProcess/SignalGen"+process+"/MaxDEtaVBF_Jets_MJJ",JJMEta.M()   , 1, 100, 0., 4000., "MaxDEta MJJ (GeV)");

  double Av_JetEta= 0.5*(JetsColl[ijet1].Eta()+ JetsColl[ijet2].Eta());
  double zeppenfeld = TMath::Max(LepFromW.Eta()  - Av_JetEta , LepFromN.Eta()  - Av_JetEta ) /maxDiJetDeta;
  FillHist( "SignalProcess/SignalGen"+process+"/MaxDEtaVBF_Jets_zeppenfeld", zeppenfeld  , 1, 200, 0., 2., "zeppenfeld");


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



void HNL_Signal_Studies::MakeType1SignalPlots(TString process, bool apply_reco_cut){
  
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


  FillHist( "SignalProcess/SignalGen"+process+"/Jet_FromW_eta", j1.Eta(),  1.,100, -5., 5,"J #eta");
  FillHist( "SignalProcess/SignalGen"+process+"/Jet_FromW_eta", j2.Eta(),  1.,100, -5., 5,"J #eta");
  FillHist( "SignalProcess/SignalGen"+process+"/Jet_FromW_pt",  j1.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Jet_FromW_pt",  j2.Pt(), 1., 200, 0., 400.,"J p_{T} (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/W_Dijet_mass",  W2.M(),  1., 200, 0., 200.,"W(jj) Mass (GeV)");


  FillHist( "SignalProcess/SignalGen"+process+"/Neutrino_pt", N.Pt(), 1., 200, 0., 400.,"N p_{T} (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Neutrino_mass", N.M(), 1., 200, 0., 2000.,"m_N (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Neutrino_eta", N.Eta(), 1.,100, -5., 5,"N #eta");

  FillHist( "SignalProcess/SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_pt",  LepFromN.Pt(), 1., 200, 0., 400.,"#ell_N p_{T} (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_eta", LepFromN.Eta(),  1.,100, -5., 5,"#ell_N #eta");
  FillHist( "SignalProcess/SignalGen"+process+"/Lep_"+LepFl_l1+"_From_Neutrino_DeltaR_W_From_Neutrino", W2.DeltaR(LepFromN), 1., 100, 0., 10.,"#Delta R(ln,W2)");
  FillHist( "SignalProcess/SignalGen"+process+"/Mass_"+LepFl_l1+"_From_Neutrino_AND_W_FromNeutrino", (W2 + LepFromN).M(), 1., 150, 0., 1500.,"M(ln+W2) GeV");

  
  FillHist( "SignalProcess/SignalGen"+process+"/Lep_"+LepFl_l2+"_FromW_pt", LepFromW.Pt(), 1., 200, 0., 400.,"#ell_w p_{T} (GeV)");
  FillHist( "SignalProcess/SignalGen"+process+"/Lep_"+LepFl_l2+"_FromW_eta", LepFromW.Eta(),  1.,100, -5., 5,"#ell_w #eta ");
  
  
  FillHist("SignalProcess/SignalGen"+process+"/Nlep_Wlep_pt", LepFromN.Pt(), LepFromW.Pt(), 1., 500,0., 2000.,500, 0.,2000.);

  if( (LepFromN.Pt() < 50) || ( LepFromW.Pt() < 50)){
    if(LepFromN.Pt() < LepFromW.Pt()) FillHist("SignalProcess/SignalGen"+process+"/Pt_LT50_NOrderPt", 1, 1, 2, 0, 2);
    else FillHist("SignalProcess/SignalGen"+process+"/Pt_LT50_NOrderPt",0, 1, 2, 0, 2);
  }
  if( (LepFromN.Pt() < 100) || ( LepFromW.Pt() <100)){
    if(LepFromN.Pt() < LepFromW.Pt()) FillHist("SignalProcess/SignalGen"+process+"/Pt_LT100_NOrderPt",1, 1, 2, 0, 2);
    else FillHist("SignalProcess/SignalGen"+process+"/Pt_LT100_NOrderPt",0, 1, 2, 0, 2);
  }
  
  if(LepFromN.Pt() < LepFromW.Pt()) FillHist("SignalProcess/SignalGen"+process+"/Pt_NOrderPt",1, 1, 2, 0, 2);
  else FillHist("SignalProcess/SignalGen"+process+"/Pt_NOrderPt",0, 1, 2, 0, 2);
  
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


  if(dphi1 > dphi2) FillHist("SignalProcess/SignalGen"+process+"/DPhi_NOrder_",1, 1, 2, 0, 2);
  else FillHist("SignalProcess/SignalGen"+process+"/DPhi_NOrder_",0, 1, 2, 0, 2);
  
  if(W2.DeltaR(LepFromW) > W2.DeltaR(LepFromN)) FillHist("SignalProcess/SignalGen"+process+"/DPhiW_NOrder_",1, 1, 2, 0, 2);
  else FillHist("SignalProcess/SignalGen"+process+"/DPhiW_NOrder_",0, 1, 2, 0, 2);

  


  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromN_N",  N.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(N,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromN_W",  W2.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(W2,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromW_N",  N.DeltaR(LepFromW), 1., 200, 0., 5.,"#DeltaR(N,lw)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromW_W",  W2.DeltaR(LepFromW), 1., 200, 0., 5.,"#DeltaR(W2,lw)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromN_J1",  j1.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(j1,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromN_J2",  j2.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(j2,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_Lep_FromW_LepFromN",  LepFromW.DeltaR(LepFromN), 1., 200, 0., 5.,"#DeltaR(lw,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaR_W_N",  W2.DeltaR(N), 1., 200, 0., 5.,"#DeltaR(N,W2)");


  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_FromN_N",  fabs(TVector2::Phi_mpi_pi( ( (N.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_FromN_W",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W2,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_FromW_N",  fabs(TVector2::Phi_mpi_pi( ( (N.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,lw)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_FromW_W",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W2,lw)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_Lep_FromW_LepFromN",  fabs(TVector2::Phi_mpi_pi( ( (LepFromN.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(lw,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_W_N",  fabs(TVector2::Phi_mpi_pi( ( (W2.Phi() - N.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,W2)");

  Particle WrongN = LepFromW + j1+j2;
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhiWr_Lep_FromN_N",  fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(wN,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhiWr_Lep_FromW_N",  fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(wN,lw)");

  Particle W1 = (N+LepFromW);
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_W1_Lep_FromN",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - LepFromN.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_W1_Lep_FromW",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - LepFromW.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,ln)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_W1_N",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - N.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(N,W1)");
  FillHist("SignalProcess/SignalGen"+process+"/DeltaPhi_W1_W2",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - W2.Phi() )))),  1., 200, 0., 5.,"#Delta#phi(W1,W2)");
  FillHist("SignalProcess/SignalGen"+process+"/Mass_NpWl",  (N+LepFromW).M(), 1., 200, 0., 2000.,"M(N,Wl) GeV");


  if(fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - LepFromN.Phi() ))))  >  N.DeltaR(LepFromN) ) FillHist("SignalProcess/SignalGen"+process+"/DPhiWrN_NOrder_",1, 1, 2, 0, 2);
  else FillHist("SignalProcess/SignalGen"+process+"/DPhiWrN_NOrder_",0, 1, 2, 0, 2);

  return;

}


void HNL_Signal_Studies::CheckHEMIssue(HNL_LeptonCore::Channel channel,std::vector<Lepton *> LepsT,std::vector<Lepton *> LepsV,       vector<Jet>& JetColl, Particle& vMET, float weight, TString Label, TString Option)
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
