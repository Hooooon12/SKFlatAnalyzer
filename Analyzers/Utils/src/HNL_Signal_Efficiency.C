#include "HNL_Signal_Efficiency.h"

void HNL_Signal_Efficiency::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();
  SetupEventMVAReader("V2");

}


void HNL_Signal_Efficiency::executeEvent(){

  
  //==== Gen for genmatching
  AnalyzerParameter param_init  = InitialiseHNLParameter("");
  Event ev = GetEvent();
  double weight =  (gen_weight > 0) ? 1. : -1. ; //SetupWeight(ev,param);
  double weight_mc= SetupWeight(ev,param_init);                                                                                         
  FillHist ("NoCut", 1, weight, 2, 0., 2.,"");

  TString process="BKG";
  if(IsSignal()){
    
    process = GetProcess();
    
    vector<TString> labels ={"Inclusive","OS_ElEl", "SS_El-El-", "SS_El+El+","OS_MuMu", "SS_Mu-Mu-" , "SS_Mu+Mu+","OS_ElMu","SS_El-Mu-" ,"SS_El+Mu+", "OS_MuEl","SS_Mu-El-","SS_Mu+El+"};
    
    HNL_LeptonCore::FillCutflow("SignalProcess","SplitChannel",weight, labels, process);
    HNL_LeptonCore::FillCutflow("SignalProcess","SplitChannel",weight, labels, "Inclusive");
    
    
    if(_jentry% 10000==0){
      cout << "process = " << process << endl;
      PrintGen(All_Gens);
    }

    
    if(process.Contains("OS")) return;

    if(process == "Mu+Mu+") process  = "MuMu";
    else if(process == "Mu-Mu-") process  = "MuMu";
    else  if(process == "El+El+") process  = "EE";
    else if(process == "El-El-") process  = "EE";
    else if(process != "BKG")  process  = "EMu";
    
  }

  /// No Cut to leptons
  
  vector<Muon> _InputMuons = GetMuons    ( "NoCut", 5., 2.4);
  vector<Muon> InputMuons  = GetSignalLeptons(_InputMuons, All_Gens);

  vector<Electron> _InputElectrons = GetElectrons    ( "NoCut", 10., 2.5);
  vector<Electron> InputElectrons  = GetSignalLeptons(_InputElectrons, All_Gens);

	
  FillHist( process + "/NoCut", 1., weight, 4,0., 4);
  
  vector<Muon>     HNLMuons     = SelectMuons (InputMuons, "HNL_ULID_"+GetYearString(), 10., 2.4);
  vector<Electron> HNLElectrons = SelectElectrons (InputElectrons, "HNL_ULID_"+GetYearString(), 15., 2.4);
  
  vector<Muon>     HNMuons = SelectMuons (InputMuons, "HNTightV2", 5., 2.4);
  vector<Electron> HNElectrons = SelectElectrons (InputElectrons, "HNTightV2", 10., 2.4);
  
  if(SameCharge(HNLMuons) && (InputMuons[0].Pt() > 20) )     FillHist( process + "/SS_Muon_HNL", 1., weight, 4,0., 4);
  if(SameCharge(HNMuons)  && (InputMuons[0].Pt() > 20) )      FillHist( process + "/SS_Muon_HNTight", 1., weight, 4,0., 4);

  if(SameCharge(HNLElectrons) && (InputElectrons[0].Pt() > 25) ) FillHist( process + "/SS_Electron_HNL", 1., weight, 4,0., 4);
  if(SameCharge(HNElectrons)  && (InputElectrons[0].Pt() > 25))  FillHist( process + "/SS_Electron_HNTight", 1., weight, 4,0., 4);
  
  /// Check baseline IDs;
  if(HasFlag("Baseline_EE")){
    
    vector<Electron> BL_InputElectrons    = GetElectrons    ( "PassMVABaseLine", 15., 2.5);
    vector<Electron> BLTrk_InputElectrons = GetElectrons    ( "PassMVABaseLine_TrkIso", 15., 2.5);
    if(SameCharge(BL_InputElectrons) && (BL_InputElectrons[0].Pt() > 25) ) FillHist( process + "/SS_Baseline_Electron_", 1., weight, 4,0., 4);
    if(SameCharge(BLTrk_InputElectrons)  && (BLTrk_InputElectrons[0].Pt() > 25))  FillHist( process + "/SS_BaselineTrkIso_Electron", 1., weight, 4,0., 4);

    if(SameCharge(BL_InputElectrons) && (BL_InputElectrons[0].Pt() > 25) )FillElectronPlots(param_init,"SS_Baseline",BL_InputElectrons,weight);
    if(SameCharge(BLTrk_InputElectrons) && (BLTrk_InputElectrons[0].Pt() > 25) )FillElectronPlots(param_init,"SS_Baseline",BLTrk_InputElectrons,weight);
    
    return;
  }
  

  vector<HNL_LeptonCore::Channel> channels = {EE,MuMu};//, EMu};
  
  vector<TString> ElectronIDs = {"NoCut",


				 "HNVetoMVA",
				 "CutBasedLooseNoIso","CutBasedMediumNoIso","CutBasedTightNoIso","MVALooseNoIso","CutBasedVetoNoIso","HNTightV2",
				 "passPOGTight","passPOGMedium","passMVAID_noIso_WP80","passMVAID_noIso_WP90","passMVAID_Iso_WP80","passMVAID_Iso_WP90",
				 "MVALoose","TopHNSST","TopHNSSL_"+ GetEraShort(),
				 "HNTightV2", "passMVAID_Iso_WP80",
				 "HNL_ULID_"+GetYearString(), 
				 "HNL_HighPt_ULID_"+GetYearString(),
				 "HNL_ULID_NoConv","HNL_ULID_Conv","HNL_ULID_Baseline","HNL_ULID_BaseLineCC",
				 
				 "HNL_ULID_FO","HNL_ULID_FO_Up","HNL_ULID_FO_Down","HNL_HighPt_ULID_FO"};

  if(HasFlag("SplitID")) {
    ElectronIDs = {"NoCut",


		   "HNVetoMVA",
		   "CutBasedLooseNoIso","CutBasedMediumNoIso","CutBasedTightNoIso","MVALooseNoIso","CutBasedVetoNoIso","HNTightV2",
		   "passPOGTight","passPOGMedium","passMVAID_noIso_WP80","passMVAID_noIso_WP90","passMVAID_Iso_WP80","passMVAID_Iso_WP90",
		   "MVALoose","TopHNSST","TopHNSSL_"+ GetEraShort(),
		   "HNTightV2", "passMVAID_Iso_WP80",
		   "HNL_ULID_"+GetYearString(),
		   "HNL_HighPt_ULID_"+GetYearString(),
		   "HNL_ULID_NoConv","HNL_ULID_Conv","HNL_ULID_Baseline","HNL_ULID_BaseLineCC",
		   "HNL_ULID_Conv_HighPt",          "HNL_ULID_Defv1_FO", "HNL_ULID_Defv2_FO","HNL_ULID_Defv3_FO","HNL_ULID_Defv4_FO","HNL_ULID_Defv5_FO",
		   "HNL_ULID_FO","HNL_ULID_FO_Up","HNL_ULID_FO_Down"
		   "HNL_ULID_Defv3","HNL_ULID_Defv4","HNL_ULID_Defv5",
		   "HNL_ULID_Split_1","HNL_ULID_Split_2","HNL_ULID_Split_3","HNL_ULID_Split_4","HNL_ULID_Split_4b",
		   "HNL_ULID_Split_5","HNL_ULID_Split_6","HNL_ULID_Split_7","HNL_ULID_Split_8","HNL_ULID_Split_8b",
		   "HNL_ULID_Probe_Split_2","HNL_ULID_Probe_Split_3","HNL_ULID_Probe_Split_4","HNL_ULID_Probe_Split_5","HNL_ULID_Probe_Split_6","HNL_ULID_Probe_Split_7","HNL_ULID_Probe_Split_8",
    };
  }
  
  vector<TString> MuonIDs = {"NoCut","POGTight","POGHighPt","POGMedium","POGLoose","POGTightWithTightIso","HNLoosePOG","HNTightV2","HNTightPFIsoLoose", "HNTightPFIsoMedium","HNTightPFIsoTight","HNTightPFIsoVeryTight","HNTightPFIsoVeryVeryTight","HNL_HN3L","Peking","HNL_ULID_"+GetYearString(),"MVALoose", "TopHN", "TopHNL","HNL_ULID_FO","HNL_ULID_FO","HNL_ULID_FO_Up","HNL_ULID_FO_Down"};  
  
  vector<pair<HNL_LeptonCore::Channel , TString > > LeptonIDMap;
  for(auto el_id : ElectronIDs) LeptonIDMap.push_back(make_pair( EE,  el_id));
  for(auto mu_id : MuonIDs)     LeptonIDMap.push_back(make_pair( MuMu,  mu_id));

 
  for(auto dilep_channel : channels){
    

    if(MCSample.Contains("Type")){
      if (!SelectChannel(dilep_channel)) continue;
    }

    
    TString channel = GetChannelString(dilep_channel) ;
    FillHist( "NoCut_"+ channel, 1., weight, 2,0., 2);

    //    double ptbins[11] = { 0., 100.,200.,300., 400.,500,700,900,  1200.,1500.,3000};
    
    double ptbins[17] = { 0., 5,10.,15., 20., 30., 40.,50., 100.,150,175,200, 250.,500. ,750,1000.,2000.};
    
    for(auto imap : LeptonIDMap){

      AnalyzerParameter param  =  DefaultParam(imap.second,GetChannelString(dilep_channel) );

      //FillCutflow(imap.second, weight, "NoCut", param);

      if(imap.first != dilep_channel) continue;

      vector<Electron> vecSelectedElectrons;
      vector<Muon>     vecSelectedMuons;
      
      if(imap.first == MuMu) vecSelectedMuons     = SelectMuons (InputMuons, imap.second, 5., 2.4);
      if(imap.first == EE) {
	vecSelectedElectrons = SelectElectrons    ( InputElectrons ,  imap.second, 10., 2.5);
	vecSelectedMuons     = SelectMuons (InputMuons, "POGMedium", 5., 2.4);
	
      }
      
      std::vector<Lepton *> leps       = MakeLeptonPointerVector(vecSelectedMuons,vecSelectedElectrons ,param);
      std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(vecSelectedMuons,vecSelectedElectrons);

      //// Run SR For ID

      std::vector<Tau>        TauColl                 = SelectTaus   (leps_veto,param.Tau_Veto_ID,20., 2.3);
      std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
      std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
      std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
      std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
      std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
      std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);

      Particle METv = GetvMET("PuppiT1xyULCorr", param);

    
      for(auto ilep:  leps)  {
	double pt = (ilep->Pt() > 2000) ? 1999 : ilep->Pt();

	TString lepton_label=ilep->GetFlavour();
	TString eta_label   = ilep->GetEtaRegion();
	
	if(MCSample.Contains("Type")){
	  
	  FillHist( "SignalReco"+channel+"/"+lepton_label+"_ptbinned_"+eta_label+"_"+imap.second, pt, weight_mc, 16, ptbins);
	  FillHist( "SignalReco"+channel+"/"+lepton_label+"_ptbinned_"+imap.second, pt, weight_mc, 16, ptbins);
	  FillHist( "SignalReco"+channel+"/"+lepton_label+"_pt_"+eta_label+"_"+imap.second, pt, weight_mc, 400, 0., 2000.);
          FillHist( "SignalReco"+channel+"/"+lepton_label+"_pt_"+imap.second, pt, weight_mc,  400, 0., 2000.);
	}
      }

      
      param.CutFlowDir = "CutFlow";
      /// Used for Lepton Inclusive plots                                                                                                                                                                           
      param.NameInclusive_Channel = param.Name  + "/"+param.InclusiveChannelName();
      param.Name = param.Name  + "/EE";



      if(imap.first == EE) {
	if(vecSelectedElectrons.size() == 2 && vecSelectedMuons.size()==0){
	  if(SameCharge(leps))  {
	    
	    if(leps[0]->LeptonIsCF()  || leps[1]->LeptonIsCF() ) {
	      if( (leps[0]->LeptonIsCF() &&  leps[0]->Pt() > 150) ||  (leps[1]->LeptonIsCF() &&  leps[1]->Pt() > 150) ) {
		
	        Fill_RegionPlots(param,"HighPt" , TauColl, AK4_JetColl, AK8_JetColl, leps,METv, nPV,weight_mc);
		
	      }
	    }
	    
	    FillHist( "SS_SignalReco"+channel+"/LLMass_"+imap.second, GetLLMass(leps) , weight_mc,  400, 0., 2000.);
	    FillHist( "SS_SignalReco"+channel+"/"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
	    if(leps[0]->IsFake()  || leps[1]->IsFake() ) FillHist( "SS_SignalReco"+channel+"/Fake_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
	    else if(leps[0]->LeptonIsCF()  || leps[1]->LeptonIsCF() )  FillHist( "SS_SignalReco"+channel+"/CF_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
	    else if(leps[0]->IsConv()  || leps[1]->IsConv() )  FillHist( "SS_SignalReco"+channel+"/Conv_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
	    else FillHist( "SS_SignalReco"+channel+"/Prompt_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
	  }
	  else{
	    FillHist( "OS_SignalReco"+channel+"/LLMass_"+imap.second, GetLLMass(leps) , weight_mc,  400, 0., 2000.);
            FillHist( "OS_SignalReco"+channel+"/"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
            if(leps[0]->IsFake()  || leps[1]->IsFake() ) FillHist( "OS_SignalReco"+channel+"/Fake_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
            else if(leps[0]->LeptonIsCF()  || leps[1]->LeptonIsCF() )  FillHist( "OS_SignalReco"+channel+"/CF_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
            else if(leps[0]->IsConv()  || leps[1]->IsConv() )  FillHist( "OS_SignalReco"+channel+"/Conv_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
            else FillHist( "OS_SignalReco"+channel+"/Prompt_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
	  }
	}
	if(vecSelectedElectrons.size() == 1 && vecSelectedMuons.size()==1){
	  if(SameCharge(leps))  {
	    FillHist( "SS_SignalRecoEMu/LLMass_"+imap.second, GetLLMass(leps) , weight_mc,  400, 0., 2000.);
	    FillHist( "SS_SignalRecoEMu/"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
	    if(leps[0]->IsFake()  || leps[1]->IsFake() ) FillHist( "SS_SignalRecoEMu/Fake_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
	    else if(leps[0]->LeptonIsCF()  || leps[1]->LeptonIsCF() )  FillHist( "SS_SignalRecoEMu/CF_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
	    else if(leps[0]->IsConv()  || leps[1]->IsConv() )  FillHist( "SS_SignalRecoEMu/Conv_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
	    else FillHist( "SS_SignalRecoEMu/Prompt_"+leps[1]->GetFlavour()+"_ptbinned_"+imap.second, leps[1]->Pt(), weight_mc, 16, ptbins);
	  }
	}
      }
      
      

      if(MCSample.Contains("Type")) continue;
      
      //// Get Efficiency of IDs 
      if(imap.first == EE)  {
	for(auto iel : vecSelectedElectrons){

	  double pt = (iel.Pt() > 2000) ? 1999 : iel.Pt();

	  if (iel.IsPrompt()) {
	    FillHist( "Prompt_"+channel+"/Electron_pt_"+iel.GetEtaRegion()+"_"+imap.second, pt, weight, 16, ptbins);
            FillHist( "Prompt_"+channel+"/Electron_pt_"+imap.second, pt, weight,  16, ptbins);
	    
	  }
	  if (iel.IsFake()) {
	    FillHist( "Fake_"+channel+"/Electron_pt_"+iel.GetEtaRegion()+"_"+imap.second, pt, weight, 16, ptbins);
	    FillHist( "Fake_"+channel+"/Electron_pt_"+imap.second, pt, weight,  16, ptbins);
	  }
	  if (iel.IsConv()) {
	    FillHist( "Conv_"+channel+"/Electron_pt_"+iel.GetEtaRegion()+"_"+imap.second, pt, weight, 16, ptbins);
	    FillHist( "Conv_"+channel+"/Electron_pt_"+imap.second, pt,  weight,  16, ptbins);
	  }
	  if(iel.LeptonIsCF()){
	    FillHist( "CF_"+channel+"/Electron_pt_"+iel.GetEtaRegion()+"_"+imap.second, pt, weight, 16, ptbins);
            FillHist( "CF_"+channel+"/Electron_pt_"+imap.second, pt,  weight,  16, ptbins);
	  }
	}
      }
      else{

        for(auto imu : vecSelectedMuons){
	  
	  double pt = (imu.Pt() > 1000) ? 999 : imu.Pt();
	  
	  if (imu.IsPrompt()) {
	    FillHist( "Prompt_"+channel+"/Muon_pt_"+imu.GetEtaRegion()+"_"+imap.second, pt, weight, 16, ptbins);
	    FillHist( "Prompt_"+channel+"/Muon_pt_"+imap.second, pt, weight,  16, ptbins);
	  }
	  if (imu.IsFake()) {
	    FillHist( "Fake_"+channel+"/Muon_pt_"+imu.GetEtaRegion()+"_"+imap.second, pt, weight, 16, ptbins);
	    FillHist( "Fake_"+channel+"/Muon_pt_"+imap.second, pt, weight,  16, ptbins);
	  }
	  if (imu.IsConv()) {
	    FillHist( "Conv_"+channel+"/Muon_pt_"+imu.GetEtaRegion()+"_"+imap.second, pt, weight, 16, ptbins);
	    FillHist( "Conv_"+channel+"/Muon_pt_"+imap.second, pt,  weight,  16, ptbins);
	  }
	}
      }
      if(imap.first == EE)  {
	vector<int> RunEl ;
	if(RunCF) RunEl =  {0,1} ;
	else RunEl = {-1};
	
	if(imap.second.Contains(GetYearString())){
	  for(auto ir : RunEl){
	    RunAllSignalRegions(Inclusive,
				vecSelectedElectrons,vecSelectedElectrons,vecSelectedMuons,vecSelectedMuons,  TauColl,
				AK4_JetCollLoose,AK4_JetColl,AK4_VBF_JetColl,AK8_JetColl, AK4_BJetColl,
				ev,METv, param, ir, weight_mc);
	  }
	}
	
      }
    }
  }
  
}



      
//      std::vector<Muon> MuonCollB = SkimLepColl(MuonColl, "MBMO");
//      std::vector<Muon> MuonCollE = SkimLepColl(MuonColl, "ME");
//      std::vector<Muon> MuonCollFakeB = SkimLepColl(MuonCollFake, "MBMO");
//      std::vector<Muon> MuonCollFakeE = SkimLepColl(MuonCollFake, "ME");
//      std::vector<Muon> MuonCollConvB = SkimLepColl(MuonCollConv, "MBMO");
//      std::vector<Muon> MuonCollConvE = SkimLepColl(MuonCollConv, "ME");




HNL_Signal_Efficiency::HNL_Signal_Efficiency(){
  SetupEvMVA();


}
 
HNL_Signal_Efficiency::~HNL_Signal_Efficiency(){
  DeleteEvMVA();

}

