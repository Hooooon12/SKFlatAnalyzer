#include "HNL_Signal_Efficiency.h"

void HNL_Signal_Efficiency::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

}


void HNL_Signal_Efficiency::executeEvent(){

  
  //==== Gen for genmatching
  AnalyzerParameter param  = InitialiseHNLParameter("");
  Event ev = GetEvent();
  double weight =  (gen_weight > 0) ? 1. : -1. ; //SetupWeight(ev,param);
  
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

  }
  if(process.Contains("OS")) return;

  if(process == "Mu+Mu+") process  = "MuMu";
  else if(process == "Mu-Mu-") process  = "MuMu";
  else  if(process == "El+El+") process  = "EE";
  else if(process == "El-El-") process  = "EE";
  else if(process != "BKG")  process  = "EMu";
    

  /// No Cut to leptons
  
  vector<Muon> _InputMuons = GetMuons    ( "NoCut", 10., 2.4);
  vector<Muon> InputMuons  = GetSignalLeptons(_InputMuons, All_Gens);

  vector<Electron> _InputElectrons = GetElectrons    ( "NoCut", 10., 2.5);
  vector<Electron> InputElectrons  = GetSignalLeptons(_InputElectrons, All_Gens);

	
  FillHist( process + "/NoCut", 1., weight, 4,0., 4);
  
  vector<Muon>     HNLMuons     = SelectMuons (InputMuons, "HNL_ULID_"+GetYearString(), 10., 2.4);
  vector<Electron> HNLElectrons = SelectElectrons (InputElectrons, "HNL_ULID_"+GetYearString(), 15., 2.4);
  
  vector<Muon>     HNMuons = SelectMuons (InputMuons, "HNTightV2", 10., 2.4);
  vector<Electron> HNElectrons = SelectElectrons (InputElectrons, "HNTightV2", 15., 2.4);
  
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

    if(SameCharge(BL_InputElectrons) && (BL_InputElectrons[0].Pt() > 25) )FillElectronPlots(param,"SS_Baseline",BL_InputElectrons,weight);
    if(SameCharge(BLTrk_InputElectrons) && (BLTrk_InputElectrons[0].Pt() > 25) )FillElectronPlots(param,"SS_Baseline",BLTrk_InputElectrons,weight);
    
    return;
  }
  

  vector<HNL_LeptonCore::Channel> channels = {EE};//,MuMu};//, EMu};
  
  vector<TString> ElectronIDs = {"NoCut","HNVetoMVA",
				 "CutBasedLooseNoIso","CutBasedMediumNoIso","CutBasedTightNoIso","MVALooseNoIso","CutBasedVetoNoIso","HNTightV2",
				 "passPOGTight","passPOGMedium","passMVAID_noIso_WP80","passMVAID_noIso_WP90","passMVAID_Iso_WP80","passMVAID_Iso_WP90",
				 "HNL_ULID_"+GetYearString(),"MVALoose","TopHNSST",
				 "HNL_ULID_NoConv","HNL_ULID_ConvLowPt","HNL_ULID_Conv_Lone","HNL_ULID_Baseline",
				 "HNL_ULID_Conv_LowPt",	  "HNL_ULID_Defv1_FO", "HNL_ULID_Defv2_FO","HNL_ULID_Defv3_FO","HNL_ULID_Defv4_FO","HNL_ULID_Defv5_FO",
				 "HNL_ULID_Defv3","HNL_ULID_Defv4","HNL_ULID_Defv5",
				 "HNL_ULID_Split_1","HNL_ULID_Split_2","HNL_ULID_Split_3","HNL_ULID_Split_4","HNL_ULID_Split_4b",
				 "HNL_ULID_Split_5","HNL_ULID_Split_6","HNL_ULID_Split_7","HNL_ULID_Split_8","HNL_ULID_Split_8b",
				 "HNL_ULID_Probe_Split_2","HNL_ULID_Probe_Split_3","HNL_ULID_Probe_Split_4","HNL_ULID_Probe_Split_5","HNL_ULID_Probe_Split_6","HNL_ULID_Probe_Split_7","HNL_ULID_Probe_Split_8"};
  
  vector<TString> MuonIDs = {"NoCut","POGTight","POGHighPt","POGMedium","POGLoose","POGTightWithTightIso","HNLoosePOG","HNTightV2","HNTightPFIsoLoose", "HNTightPFIsoMedium","HNTightPFIsoTight","HNTightPFIsoVeryTight","HNTightPFIsoVeryVeryTight","HNL_HN3L","Peking","HNL_ULID_"+GetYearString(),"MVALoose", "TopHN"};  
  
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
    
    double ptbins[14] = { 0., 10.,15., 20., 30., 40.,50., 100.,250.,500. ,750,1000.,1500,2000.};
    
    for(auto imap : LeptonIDMap){

      //FillCutflow(imap.second, weight, "NoCut", param);

      if(imap.first != dilep_channel) continue;

      vector<Electron> vecSelectedElectrons;
      vector<Muon>     vecSelectedMuons;
      
      if(imap.first == MuMu) vecSelectedMuons     = SelectMuons (InputMuons, imap.second, 10., 2.4);
      if(imap.first == EE)   vecSelectedElectrons = SelectElectrons    ( InputElectrons ,  imap.second, 10., 2.5);
      
      std::vector<Lepton *> leps       = MakeLeptonPointerVector(vecSelectedMuons,vecSelectedElectrons ,param);
      
      for(auto ilep:  leps)  {
	double pt = (ilep->Pt() > 2000) ? 1999 : ilep->Pt();

	TString lepton_label=ilep->GetFlavour();
	TString eta_label   = ilep->GetEtaRegion();
	
	if(MCSample.Contains("Type")){
	  
	  FillHist( "SignalReco"+channel+"/"+lepton_label+"_ptbinned_"+eta_label+"_"+imap.second, pt, weight, 11, ptbins);
	  FillHist( "SignalReco"+channel+"/"+lepton_label+"_ptbinned_"+imap.second, pt, weight, 11, ptbins);
	  FillHist( "SignalReco"+channel+"/"+lepton_label+"_pt_"+eta_label+"_"+imap.second, pt, weight, 400, 0., 2000.);
          FillHist( "SignalReco"+channel+"/"+lepton_label+"_pt_"+imap.second, pt, weight,  400, 0., 2000.);
	}
      }
      if(SameCharge(leps))           FillHist( "SS_SignalReco"+channel+"/LLMass_"+imap.second, GetLLMass(leps) , weight,  400, 0., 2000.);


      if(MCSample.Contains("Type")) continue;
      
      if(imap.first == EE)  {
	for(auto iel : vecSelectedElectrons){

	  double pt = (iel.Pt() > 2000) ? 1999 : iel.Pt();

	  if (iel.IsPrompt()) {
	    FillHist( "Prompt_"+channel+"/Electron_pt_"+iel.GetEtaRegion()+"_"+imap.second, pt, weight, 13, ptbins);
            FillHist( "Prompt_"+channel+"/Electron_pt_"+imap.second, pt, weight,  13, ptbins);
	    
	  }
	  if (iel.IsFake()) {
	    FillHist( "Fake_"+channel+"/Electron_pt_"+iel.GetEtaRegion()+"_"+imap.second, pt, weight, 13, ptbins);
	    FillHist( "Fake_"+channel+"/Electron_pt_"+imap.second, pt, weight,  13, ptbins);
	  }
	  if (iel.IsConv()) {
	    FillHist( "Conv_"+channel+"/Electron_pt_"+iel.GetEtaRegion()+"_"+imap.second, pt, weight, 13, ptbins);
	    FillHist( "Conv_"+channel+"/Electron_pt_"+imap.second, pt,  weight,  13, ptbins);
	  }
	  if(iel.LeptonIsCF()){
	    FillHist( "CF_"+channel+"/Electron_pt_"+iel.GetEtaRegion()+"_"+imap.second, pt, weight, 13, ptbins);
            FillHist( "CF_"+channel+"/Electron_pt_"+imap.second, pt,  weight,  13, ptbins);
	  }
	}
      }
      else{

        for(auto imu : vecSelectedMuons){
	  
	  double pt = (imu.Pt() > 1000) ? 999 : imu.Pt();
	  
	  if (imu.IsPrompt()) {
	    FillHist( "Prompt_"+channel+"/Muon_pt_"+imu.GetEtaRegion()+"_"+imap.second, pt, weight, 13, ptbins);
	    FillHist( "Prompt_"+channel+"/Muon_pt_"+imap.second, pt, weight,  13, ptbins);
	  }
	  if (imu.IsFake()) {
	    FillHist( "Fake_"+channel+"/Muon_pt_"+imu.GetEtaRegion()+"_"+imap.second, pt, weight, 13, ptbins);
	    FillHist( "Fake_"+channel+"/Muon_pt_"+imap.second, pt, weight,  13, ptbins);
	  }
	  if (imu.IsConv()) {
	    FillHist( "Conv_"+channel+"/Muon_pt_"+imu.GetEtaRegion()+"_"+imap.second, pt, weight, 13, ptbins);
	    FillHist( "Conv_"+channel+"/Muon_pt_"+imap.second, pt,  weight,  13, ptbins);
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


}
 
HNL_Signal_Efficiency::~HNL_Signal_Efficiency(){

}

