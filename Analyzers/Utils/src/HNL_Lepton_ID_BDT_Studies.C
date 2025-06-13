#include "HNL_Lepton_ID_BDT_Studies.h"

void HNL_Lepton_ID_BDT_Studies::initializeAnalyzer(){

  // All default settings like trigger/ PD/ BJet are decalred in HNL_LeptonCore::initializeAnalyzer to make them consistent for all HNL codes

  HNL_LeptonCore::initializeAnalyzer();

}


void HNL_Lepton_ID_BDT_Studies::executeEvent(){
  
  if((_jentry==0)){ 
    // Print out trigger info in HNL_LeptonCore::initializeAnalyzer
    TriggerPrintOut(GetEvent());
  }
  
  AnalyzerParameter param  = HNL_LeptonCore::InitialiseHNLParameter("HNL_ULIDv2");
  
  Event ev = GetEvent();
  double weight = SetupWeight(ev,param);
  if(MCSample.Contains("Type")) {
    weight = 1;
  }
  //  else if(MCSample.Contains("DYJets")) weight *= 0.5;
  //else if(MCSample.Contains("WJets")) weight *= 0.5;
  //else if(MCSample.Contains("WGToLNuG")) weight *= 0.5;

  if(MCSample.Contains("WJets")){
    if (weight < 0) return;
  }


  //// weight WJet and DY by 0.5 to allow weigted sample to be consistent with TTBar when addinh WJet MG && MCatNLO

  std::vector<Electron>   ElectronCollProbe = GetElectrons("MVALoose", 10., 2.5); 
  std::vector<Electron>   ElectronColl      = GetElectrons("passPOGTight", 10., 2.5); 

  std::vector<Muon>       MuonCollProbe     = GetMuons    ("HNL_ULID_Baseline", 10., 2.4);
  std::vector<Muon>       MuonColl          = GetMuons    ("POGTightWithTightIso", 10., 2.4);


  if(!PassMETFilter()) return;

  std::vector<Jet> BJetColl  = GetHNLJets("BJetM", param);
  bool HasBjet = (BJetColl.size() > 0);

  if(HasFlag("SSBreakdown")){
    
    CheckSSFakeBreakDown(param,EE, ElectronCollProbe, MuonCollProbe, weight);
    CheckSSFakeBreakDown(param,MuMu, ElectronCollProbe, MuonCollProbe, weight);
    return;
  }

  //////////////////////////  //////////////////////////  //////////////////////////  //////////////////////////
  //////////////////////////  //////////////////////////  //////////////////////////  //////////////////////////
  //
  //  Make plots  of Bkg / Sig eff for MVA IDs
  //
  //////////////////////////  //////////////////////////  //////////////////////////  //////////////////////////
  //////////////////////////  //////////////////////////  //////////////////////////  //////////////////////////

  
  std::vector<Lepton *> LeptonCollProbe      = MakeLeptonPointerVector(MuonCollProbe,ElectronCollProbe);  
  if(LeptonCollProbe.size() == 0) return;
  
  
  if(HasFlag("FakeSplit")){

    for(auto ilep : ElectronCollProbe){
      if ( FindHEMElectron (ilep )) continue;
      if (!ilep.IsFake()) continue;
      TString LepType = "Fake"+ilep.CloseJet_Flavour();
      double bvsl_score = ilep.CloseJet_BScore();
      double cvsb_score = ilep.CloseJet_CvsBScore();
      double cvsl_score = ilep.CloseJet_CvsLScore();
      FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/"+LepType+"_BvsLscore", bvsl_score  , weight, 200, -1., 1);
      FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/"+LepType+"_BvsCscore", cvsb_score  , weight, 200, -1., 1);
      FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/"+LepType+"_CvsLscore", cvsl_score  , weight, 200, -1., 1);

      map<TString, double> mapBDT = ilep.MAPBDT();
      for(auto imap : mapBDT )  FillHist("BDTVariableFS/"+ ilep.GetFlavour()+ "/"+ LepType+"_"+imap.first, imap.second  , weight, 200, -1., 1);

      if (bvsl_score < 0.3 && cvsl_score < 0.1 )       FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/JetTaggerBin_"+LepType, 1  , weight, 5, 0., 5);
      if (bvsl_score > 0.3 && cvsb_score < 0.3 )       FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/JetTaggerBin_"+LepType, 2  , weight, 5, 0., 5);
      if (cvsl_score > 0.1 && cvsb_score > 0.3 )       FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/JetTaggerBin_"+LepType, 3  , weight, 5, 0., 5);
      
      if(ilep.HNL_MVA_Fake("QCD_LFvsHF_v5") > 0.9 ) FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/LepQCDTaggerBin_"+LepType, 1  , weight, 5, 0., 5);
      else if(ilep.HNL_MVA_Fake("QCD_LFvsHF_v5") > -0.5 ){
	
	if(ilep.HNL_MVA_Fake("QCD_BvsC_v5") > 0.2) FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/LepQCDTaggerBin_"+LepType, 2  , weight, 5, 0., 5);
      }
      else {
	//// Pure B
	FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/LepQCDTaggerBin_"+LepType, 1  , weight, 5, 0., 5);
      }
      
    }
    
    for(auto ilep : MuonCollProbe){
      if (!ilep.IsFake()) continue;
      TString LepType = "Fake"+ilep.CloseJet_Flavour();
      double bvsl_score = ilep.CloseJet_BScore();
      double cvsb_score = ilep.CloseJet_CvsBScore();
      double cvsl_score = ilep.CloseJet_CvsLScore();
      FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/"+LepType+"_BvsLscore", bvsl_score  , weight, 200, -1., 1);
      FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/"+LepType+"_BvsCscore", cvsb_score  , weight, 200, -1., 1);
      FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/"+LepType+"_CvsLscore", cvsl_score  , weight, 200, -1., 1);

      map<TString, double> mapBDT = ilep.MAPBDT();
      for(auto imap : mapBDT )  FillHist("BDTVariableFS/"+ ilep.GetFlavour()+ "/"+ LepType+"_"+imap.first, imap.second  , weight, 200, -1., 1);

      if (bvsl_score < 0.3 && cvsl_score < 0.1 )       FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/JetTaggerBin_"+LepType, 1  , weight, 5, 0., 5);
      if (bvsl_score > 0.3 && cvsb_score < 0.3 )       FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/JetTaggerBin_"+LepType, 2  , weight, 5, 0., 5);
      if (cvsl_score > 0.1 && cvsb_score > 0.3 )       FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/JetTaggerBin_"+LepType, 3  , weight, 5, 0., 5);
      
      if(ilep.HNL_MVA_Fake("QCD_LFvsHF_v5") > 0) FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/LepQCDTaggerBin_"+LepType, 1  , weight, 5, 0., 5);
      else  {
	if(ilep.HNL_MVA_Fake("QCD_BvsC_v5") > 0) FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/LepQCDTaggerBin_"+LepType, 2  , weight, 5, 0., 5);
	else FillHist("FakeSplit/"+ ilep.GetFlavour()+ "/LepQCDTaggerBin_"+LepType, 3  , weight, 5, 0., 5);
      } 
    }
    
    
    return;
  }
  

  if(HasFlag("BDTPerType")){

    int nbin_pt    =10;
    double ptbins    [nbin_pt    +1] = { 10.,15.,20.,30.,35., 40.,50., 60., 80., 100.,200.};


    for(auto ilep : ElectronCollProbe){
      double PtLep =  (ilep.Pt() > 200) ? 199 : ilep.Pt();

      TString  LepType = ilep.LepGenTypeNumberString();
      if ( FindHEMElectron (ilep )) continue;
      map<TString, double> mapBDT = ilep.MAPBDT();
      for(auto imap : mapBDT ){
	FillHist("BDTVariableLowPt/"+LepType+"/Electron/"+imap.first, imap.second  , weight, 200, -1., 1);
      }
      
      if(ilep.PassID(param.Electron_FR_ID))FillHist("LepTypeFR/Electron/"+LepType+"_Loose", PtLep,   weight, nbin_pt, ptbins,"");
      if(ilep.PassID(param.Electron_Tight_ID))FillHist("LepTypeFR/Electron/"+LepType+"_Tight", PtLep,   weight, nbin_pt, ptbins,"");
      
      
    }
    for(auto ilep : MuonCollProbe){
      double PtLep =  (ilep.Pt() > 200) ? 199 : ilep.Pt();

      TString  LepType = ilep.LepGenTypeNumberString();
      map<TString, double> mapBDT = ilep.MAPBDT();
      for(auto imap : mapBDT ) {
        FillHist("BDTVariableLowPt/"+LepType+"/Muon/"+imap.first, imap.second  , weight, 200, -1., 1);
      }
      
      if(ilep.PassID(param.Muon_FR_ID))FillHist("LepTypeFR/Muon/"+LepType+"_Loose", PtLep,   weight, nbin_pt, ptbins,"");
      if(ilep.PassID(param.Muon_Tight_ID))FillHist("LepTypeFR/Muon/"+LepType+"_Tight", PtLep,   weight, nbin_pt, ptbins,"");
    }
    return;
  }
  
  if(HasFlag("BDTPlots")){
    
    
    for(auto ilep : ElectronCollProbe){
      TString  LepType = "";
      if ( FindHEMElectron (ilep )) continue;
      
      if (ilep.IsConv())  LepType = "Conv";
      if (ilep.IsFake())  LepType = "Fake"+ilep.CloseJet_Flavour();
      if (ilep.IsFake() && ilep.CloseJet_Flavour() == "Pileup") continue;
      if (ilep.LeptonIsCF())  LepType = "CF";
      else if (ilep.IsPrompt()) LepType = "Prompt";
      if(ilep.IsEWtau()) continue;
      if (LepType == "") continue;
      
      TString ptstring = "Pt1";
      if(ilep.Pt() < 30 ) ptstring = "Pt1";
      else if(ilep.Pt() < 150 ) ptstring = "Pt2";
      else ptstring = "Pt4";
      
      TString etastring = ilep.sEtaRegion();
      
      FillBDTHists(ilep,LepType+"/"+ptstring+"_"+etastring,weight);
    }
  
    
    for(auto ilep : MuonCollProbe){
      
      TString  LepType = "";
      if (ilep.IsConv())  LepType = "Conv";
      if (ilep.IsFake())  LepType = "Fake"+ilep.CloseJet_Flavour();
      if (ilep.IsFake() && ilep.CloseJet_Flavour() == "Pileup") continue;
      if (ilep.LeptonIsCF())  LepType = "CF";
      else if (ilep.IsPrompt()) LepType = "Prompt";
      if(ilep.IsEWtau()) continue;
      if (LepType == "") continue;
      
      
      FillBDTHists(ilep,LepType+"/Muon",weight);
      
  
    }

    
    for(auto ilep : ElectronCollProbe){
      
      TString  LepType = "";
      if (ilep.IsConv())  LepType = "Conv";
      if (ilep.IsFake())  LepType = "Fake"+ilep.CloseJet_Flavour();
      if (ilep.IsFake() && ilep.CloseJet_Flavour() == "Pileup") continue;
      if (ilep.LeptonIsCF())  LepType = "CF";
      else if (ilep.IsPrompt()) LepType = "Prompt";
      if(ilep.IsEWtau()) continue;
      if (LepType == "") continue;
      
      
      FillBDTHists(ilep,LepType+"/Electron",weight);
      
    }
    return;
  }


  for(auto ilep : LeptonCollProbe){

    TString tag= ilep->LepGenTypeString();
    if(ilep->LeptonIsCF()) tag = tag +"_IsCF";
    TString etabin = (fabs(ilep->Eta()) < 1.5) ? "BB_" : "EC_";
    TString ptbin = "Ptlt20_";

    if(ilep->IsMuon()){
      if(ilep->Pt() < 15) ptbin = "Ptlt15_";
      else if(ilep->Pt() < 20) ptbin = "Ptlt20_";
      else if(ilep->Pt() < 50) ptbin = "Ptlt50_";
      else if(ilep->Pt() < 80) ptbin = "Ptlt80_";
      else if(ilep->Pt() < 100) ptbin = "Ptlt100_";
      else  ptbin = "Ptgt100_";
    }
    else{
      if(ilep->Pt() < 15) ptbin = "Ptlt15_";
      else if(ilep->Pt() < 20) ptbin = "Ptlt20_";
      else if(ilep->Pt() < 50) ptbin = "Ptlt50_";
      else if(ilep->Pt() < 80) ptbin = "Ptlt80_";
      else if(ilep->Pt() < 100) ptbin = "Ptlt100_";
      else  ptbin = "Ptgt100_";

    }
    
    vector<TString>   Tags = {tag+etabin+"/MVA_",
			      tag+etabin+ilep->CloseJet_Flavour()+"/MVA_",
			      tag+ptbin+etabin+"/MVA_"};
    
    for(auto i  : Tags){
      
      if(i.Contains("Pileup")) continue;
      if(i.Contains("IsEWtau")) continue;
      if(i.Contains("IsCF") && !i.Contains("IsPrompt")) continue;
      if(i.Contains("LF") && !i.Contains("IsFake")) continue;
      if(i.Contains("HF") && !i.Contains("IsFake")) continue;
      
      if(i.Contains("IsFake") && HasBjet) continue;
      map<TString, double> mapBDT = ilep->MAPBDT();
      for(auto imap : mapBDT )  FillHist("BDTVariable/"+ ilep->GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);

    }
  }

  for(auto imuon : MuonCollProbe){
    
    TString tag= imuon.LepGenTypeString();
    if(imuon.LeptonIsCF()) tag = tag +"_IsCF";
    TString etabin = (fabs(imuon.Eta()) < 1.5) ? "BB_" : "EC_";
    TString ptbin = "Ptlt20_";
    if(imuon.Pt() < 15) ptbin = "Ptlt15_";
    else if(imuon.Pt() < 20) ptbin = "Ptlt20_";
    else if(imuon.Pt() < 50) ptbin = "Ptlt50_";
    else if(imuon.Pt() < 80) ptbin = "Ptlt80_";
    else if(imuon.Pt() < 100) ptbin = "Ptlt100_";
    else  ptbin = "Ptgt100_";

    vector<TString> Tags = {
                            tag+etabin+"/MVA_",
                            tag+etabin+imuon.MotherJetFlavour()+"/MVA_",
                            tag+ptbin+etabin+"/MVA_",

    };
  
    for(auto i  : Tags){

      if(i.Contains("Pileup")) continue;
      if(i.Contains("IsEWtau")) continue;
      if(i.Contains("IsCF") && !i.Contains("IsPrompt")) continue;
      if(i.Contains("LF") && !i.Contains("IsFake")) continue;
      if(i.Contains("HF") && !i.Contains("IsFake")) continue;
      
      if(i.Contains("IsFake") && HasBjet) continue;

      if(imuon.PassID("HNL_HighPt_ULID_"+GetYearString()))FillMuonKinematicPlots( param, "Tight"+imuon.GetFlavour(), imuon, weight);
      
      map<TString, double> mapBDT = imuon.MAPBDT();
      for(auto imap : mapBDT )  {
        if(imuon.HNL_MVA_Fake("HFTop") > 0.)   FillHist("BDTVariablesHFNP1/"+ imuon.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
        if(imuon.HNL_MVA_Fake("EDv4")  > 0.)   FillHist("BDTVariablesNP1/"  + imuon.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
        if(imuon.HNL_MVA_Fake("HFTop") > 0.2)  FillHist("BDTVariablesHFNP2/"+ imuon.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
        if(imuon.HNL_MVA_Fake("EDv4")  > 0.2)  FillHist("BDTVariablesNP2/"  + imuon.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
        if(imuon.HNL_MVA_Fake("HFTop") > 0.4)  FillHist("BDTVariablesHFNP3/"+ imuon.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
        if(imuon.HNL_MVA_Fake("EDv4")  > 0.4)  FillHist("BDTVariablesNP3/"  + imuon.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
        if(imuon.HNL_MVA_Fake("HFTop") > 0.6)  FillHist("BDTVariablesHFNP4/"+ imuon.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
        if(imuon.HNL_MVA_Fake("EDv4")  > 0.6)  FillHist("BDTVariablesNP4/"  + imuon.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
      }

      vector<TString> IDs = {"HNTightV2","HNL_Peking","HNTight_17028","HNL_HN3L","HNL_HighPt_ULID_"+GetYearString()};

      int nbin_pt    =10;
      double ptbins    [nbin_pt    +1] = { 10.,15.,20.,30.,35., 40.,50., 60., 80., 100.,200.};
      double PtLep = (imuon.Pt() > 200) ? 199 : imuon.Pt();
      
      if(!i.Contains("Pt") )FillHist("IDEff/"+imuon.GetFlavour()+"Ptbinned_"+i+"_denominator", PtLep,   weight, nbin_pt, ptbins,"");
      
      for(auto ID : IDs){
	FillHist("TightID/"+ imuon.GetFlavour()+ "/"+ i+"_"+ID, 0  , weight, 2, 0, 2);
	if(!i.Contains("Pt") ){
	  if(imuon.PassID(ID))       FillHist("IDEff/"+imuon.GetFlavour()+"Ptbinned_"+i+ "_"+ID , PtLep,   weight, nbin_pt, ptbins,"");
	}

	if(imuon.PassID(ID))       FillHist("TightID/"+ imuon.GetFlavour()+ "/"+ i+"_"+ID, 1  , weight, 2, 0, 2);
      }
    }
  }
  
  
  for(auto iel : ElectronCollProbe){

    TString tag= iel.LepGenTypeString();
    if(iel.LeptonIsCF()) tag = tag +"_IsCF";
    TString etabin = (fabs(iel.Eta()) < 1.5) ? "BB_" : "EC_";
    TString ptbin = "Ptlt20_";
    if(iel.Pt() < 15) ptbin = "Ptlt15_";
    else if(iel.Pt() < 20) ptbin = "Ptlt20_";
    else if(iel.Pt() < 50) ptbin = "Ptlt50_";
    else if(iel.Pt() < 80) ptbin = "Ptlt80_";
    else if(iel.Pt() < 100) ptbin = "Ptlt100_";
    else  ptbin = "Ptgt100_";

    vector<TString>    Tags = {tag+etabin+"/MVA_",
			       tag+etabin+iel.MotherJetFlavour()+"/MVA_",
			       tag+ptbin+etabin+"/MVA_"
    };

    
    ///////// HNL_ULID_Baseline  HAS COnv veto on top of MVALoose

    if(iel.PassID("HNL_ULID_Baseline")) {
      Tags.push_back(tag+etabin+"/Baseline_MVA_");
      Tags.push_back(tag+etabin+iel.MotherJetFlavour()+"/Baseline_MVA_");
      Tags.push_back(tag+ptbin+etabin+"/Baseline_MVA_");
    }
    
    for(auto i  : Tags){

      if(i.Contains("Pileup")) continue;
      if(i.Contains("IsEWtau")) continue;
      if(i.Contains("IsCF") && !i.Contains("IsPrompt")) continue;
      if(i.Contains("LF") && !i.Contains("IsFake")) continue;
      if(i.Contains("HF") && !i.Contains("IsFake")) continue;
      if(i.Contains("IsFake") && HasBjet) continue;
      

      if(iel.PassID("HNL_ULID_HighPt_"+GetYearString()))FillElectronKinematicPlots(param, "Tight"+iel.GetFlavour(), iel, weight);
      FillElectronKinematicPlots(param,"LooseUL_"+iel.GetFlavour()+i , iel, weight);

      
      map<TString, double> mapBDT = iel.MAPBDT();
      for(auto imap : mapBDT )  {

	FillHist("BDTVariablesInc/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
	
	/////// BARREL ELECTRON SCAN
	TString EtaRegion = (iel.IsBB()) ? "BB" : "EC";

	if(iel.HNL_MVA_Fake("EDv5") > 0.2) {
	  FillHist("BDTVariablesNP"+EtaRegion+"1/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
	  
	  if(iel.PassID("HNL_ULID_Conv_HighPt")) FillHist("BDTVariablesConvNP"+EtaRegion+"1/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
          if(iel.HNL_MVA_Conv("EDv5")   > -0.7) FillHist("BDTVariablesConvNP"+EtaRegion+"2/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
          if(iel.HNL_MVA_Conv("EDv5")   > 0.) FillHist("BDTVariablesConvNP"+EtaRegion+"3/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
	  
	}

	if(iel.PassID("HNL_ULID_Conv_HighPt")) FillHist("BDTVariablesConv"+EtaRegion+"1/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
	if(iel.HNL_MVA_Conv("EDv5")   > -0.7) FillHist("BDTVariablesConv"+EtaRegion+"2/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
	if(iel.HNL_MVA_Conv("EDv5")   > 0.) FillHist("BDTVariablesConv"+EtaRegion+"3/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);

	
	if(iel.HNL_MVA_CF("EDv5")   >0.4) FillHist("BDTVariablesCF"+EtaRegion+"1/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
	if(iel.HNL_MVA_CF("EDv5")   >0.6) FillHist("BDTVariablesCF"+EtaRegion+"2/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
	
	if(iel.HNL_MVA_Fake("EDv5") > 0.2  && iel.HNL_MVA_CF("EDv5") >0.4)  FillHist("BDTVariablesNPCF"+EtaRegion+"1/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
        if(iel.HNL_MVA_Fake("EDv5") > 0.2  && iel.HNL_MVA_CF("EDv5") >0.6) FillHist("BDTVariablesNPCF"+EtaRegion+"2/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
	if(iel.HNL_MVA_Fake("EDv5") > 0.3  && iel.HNL_MVA_CF("EDv5") >0.65) FillHist("BDTVariablesNPCF"+EtaRegion+"3/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);

	if(iel.HNL_MVA_CF("EDv5") >0.4 &&  iel.PassID("HNL_ULID_Conv_HighPt"))      FillHist("BDTVariablesConvCF"+EtaRegion+"1/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
	
	if(iel.HNL_MVA_CF("EDv5") >0.65 &&  iel.PassID("HNL_ULID_Conv_HighPt"))      FillHist("BDTVariablesConvCF"+EtaRegion+"2/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
	if(iel.HNL_MVA_CF("EDv5") >0.65 &&  iel.HNL_MVA_Conv("EDv5")   > -0.7)      FillHist("BDTVariablesConvCF"+EtaRegion+"3/"+ iel.GetFlavour()+ "/"+ i+"_"+imap.first, imap.second  , weight, 200, -1., 1);
	
      }
      

      vector<TString> IDs = {"HNTightV2","HNL_Peking_"+GetYearString(),  "HNTight_17028","HNL_HighPt_ULID" , "HNL_ULID_Conv_HighPt"};


      int nbin_pt    =10;
      double ptbins    [nbin_pt    +1] = { 10.,15.,20.,30.,35., 40.,50., 60., 80., 100.,200.};
      double PtLep = (iel.Pt() > 200) ? 199 : iel.Pt();

      if(!i.Contains("Pt") )      FillHist("IDEff/"+iel.GetFlavour()+"Ptbinned_"+i+"_denominator", PtLep,   weight, nbin_pt, ptbins,"");
      

      for(auto ID : IDs){
        FillHist("TightID/"+ iel.GetFlavour()+ "/"+ i+"_"+ID, 0  , weight, 2, 0, 2);

	if(!i.Contains("Pt") ){
	  if(iel.PassID(ID))       FillHist("IDEff/"+iel.GetFlavour()+"Ptbinned_"+i+ "_"+ID , PtLep,   weight, nbin_pt, ptbins,"");
	}
        if(iel.PassID(ID))       FillHist("TightID/"+ iel.GetFlavour()+ "/"+ i+"_"+ID, 1  , weight, 2, 0, 2);

      } // IDs
    } // Tags
  }
  
  HNL_LeptonCore::Channel dilep_channel= EE;
  if(MuonCollProbe.size() > 0) return;
  if(HasFlag("BDT"))MakeBDTPlots(param,dilep_channel, ElectronCollProbe, param.Electron_Tight_ID, weight);
  
  return;
  
  
}


void HNL_Lepton_ID_BDT_Studies::MakeJetDiscPlots(TString label, AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl ,vector<Muon>       MuonColl, double weight_ll){

  TString channel_string = GetChannelString(dilep_channel) ;

  std::vector<Lepton *> LeptonColl      = MakeLeptonPointerVector(MuonColl, ElectronColl);

  if(label.Contains("Tight")) EvalLeptonIDWeight(LeptonColl,param, weight_ll);
  
  std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, 10., 2.5);
  std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID, 5., 2.4);
  std::vector<Lepton *>   LeptonCollV      = MakeLeptonPointerVector(MuonCollV, ElectronCollV);


  //// Select TT -> emu for Top sample
  
  Event ev = GetEvent();
  if(LeptonColl.size() != 2) return;
  if(HasFlag("SS")){
    if(!SameCharge(LeptonColl)) return;
  }
  else {
    if(SameCharge(LeptonColl)) return;
  }

  Particle METv = GetMiniAODvMET("PuppiT1xyCorr"); // reyturns MET with systematic correction                                                                                                 

  std::vector<Jet>      JetColl     = GetHNLJets("Tight", param);
  std::vector<Jet> BJetColl  = GetHNLJets("BJetM", param);                                                                                                                

  if(HasFlag("SS")){
    if(BJetColl.size() == 0) return;
  }

  double metcut = (HasFlag("SS")) ? 30. : 40;
  

  if(ElectronColl.size() == 1 && MuonColl.size()   == 1){
    if(ElectronCollV.size() == 1 && MuonCollV.size()   == 1){
      if((JetColl.size() > 3 && METv.Pt() > metcut )){
	if(dilep_channel==MuMu){
	  if(PassTriggerSelection(dilep_channel, ev, LeptonColl,"Lep")){

	    if(MuonColl[0].Pt() > 20) {
	      
	      vector<Jet> JetAllColl = GetAllJets();
	      TString cut = "TopJets_TightMu";
	      
	      FillHist(cut +"/MET_"+channel_string, GetMiniAODvMET("PuppiT1xyCorr").Pt(), weight_ll, 200, 0., 200);
	      if(JetColl.size() < 6) FillHist(cut +"/NJet_"+channel_string, JetColl.size(), weight_ll, 8, 0., 8);
	      else FillHist(cut +"/NJet_"+channel_string, 6., weight_ll, 8, 0., 8);

	      FillHist(cut +"/MuonPt_"+channel_string+"_"+label, MuonColl[0].Pt(), weight_ll, 40, 0., 200);
	      FillHist(cut +"/ElectronPt_"+channel_string+"_"+label, ElectronColl[0].Pt(), weight_ll, 40, 0., 200);
	      
              FillHist(cut +"/MET_"+channel_string+"_"+label, GetMiniAODvMET("PuppiT1xyCorr").Pt(), weight_ll, 200, 0., 200);
              if(JetColl.size() < 6) FillHist(cut +"/NJet_"+channel_string+"_"+label, JetColl.size(), weight_ll, 8, 0., 8);
              else FillHist(cut +"/NJet_"+channel_string+"_"+label, 6., weight_ll, 8, 0., 8);

	      for(auto ilep: LeptonColl){
		
		if(ilep->Pt() > 25) FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_gt25PtRatio_"+label   , JetLeptonPtRatioLepAware(*ilep) , weight_ll, 200,0,2);
		if(ilep->Pt() > 25)     FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_gt25PtRel_"+label   , JetLeptonPtRelLepAware(*ilep) , weight_ll, 200,0,200);
                FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_PtRel_"+label   , JetLeptonPtRelLepAware(*ilep) , weight_ll, 200,0,200);
                FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_PtRatio_"+label   , JetLeptonPtRatioLepAware(*ilep) , weight_ll, 200,0,2);
		if(ilep->IsPrompt( )){
		  FillHist( cut+ "/"+ilep->GetFlavour() + "PromptLep_Jet_PtRel_"+label   , JetLeptonPtRelLepAware(*ilep) , weight_ll, 200,0,200);

		  FillHist( cut+ "/"+ilep->GetFlavour() + "PromptLep_Jet_PtRatio_"+label   , JetLeptonPtRatioLepAware(*ilep) , weight_ll, 200,0,2);

		}

              }

	      for(unsigned int ij=0; ij<JetAllColl.size(); ij++){
		
		bool closelep(false);
		TString lepflav="";
		TString ptlabel="";
		bool isprompt(true);

		for(auto ilep : LeptonCollV) {
		  if(ilep->DeltaR(JetAllColl.at(ij)) < 0.4) {
		    closelep=true;
		    lepflav=ilep->GetFlavour();
		    if(ilep->Pt() < 20) ptlabel="lt20_";
		    else if(ilep->Pt() < 35)ptlabel="lt35_";
		    else ptlabel="gt35_";
		    if(!ilep->IsPrompt( )) isprompt=false;
		  }
		}
		
		if(!closelep){
		  if(!JetAllColl.at(ij).PassID("tight")) continue;
		  if(JetAllColl.at(ij).Pt() < 20) continue;
		}
		
		FillHist( cut+ "/JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		
		if(closelep) {

		  double mc_cj_disc_corr  = GetCorrCloseJetDisc(true, JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet)) ;
		  
		  FillHist( cut+ "/CJ_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  FillHist( cut+ "/CJ_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  
		  FillHist( cut+ "/ModCJ_JetDisc_DeepJet_"+lepflav+"_"+label   , mc_cj_disc_corr , weight_ll, 1000,0,1);
		  FillHist( cut+ "/ModCJ_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , mc_cj_disc_corr , weight_ll, 1000,0,1);

		  if(isprompt){
		    FillHist( cut+ "/CJ_PromptLep_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		    FillHist( cut+ "/CJ_PromptLep_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  }
		  else{
		    FillHist( cut+ "/CJ_NonPromptLep_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
                    FillHist( cut+ "/CJ_NonPromptLep_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  }
		}
		else {
		  FillHist( cut+ "/AJ_JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  if(JetAllColl.at(ij).Pt() > 30) FillHist( cut+ "/AJ_JetDisc_DeepJet30_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		}
		
		if(JetAllColl.at(ij).PassID("tight")) {
		  if(closelep)    FillHist( cut+ "/TightCJ_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  else {
		    FillHist( cut+ "/TightAJ_JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		    if(JetAllColl.at(ij).Pt() > 30) FillHist( cut+ "/TightAJ_JetDisc_DeepJet30_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  }
		}
	      }
	      
	      //FillElectronPlots("ElectronMuon","Top",ElectronColl,weight_ll);
	      //if(BJetColl.size() > 1)FillElectronPlots("ElectronMuon","BTop",ElectronColl,weight_ll);
	    }
	  }
	}
	else if(dilep_channel==EE){

	  if(PassTriggerSelection(dilep_channel, ev, LeptonColl,"Lep")){
	    
	    if(ElectronColl[0].Pt() > 20) {
	      
	      vector<Jet> JetAllColl = GetAllJets();
	      TString cut = "TopJets_TightEl";

	      FillHist(cut +"/MET_"+channel_string, GetMiniAODvMET("PuppiT1xyCorr").Pt(), weight_ll, 200, 0., 200);
	      if(JetColl.size() < 6) FillHist(cut +"/NJet_"+channel_string, JetColl.size(), weight_ll, 8, 0., 8);
	      else FillHist(cut +"/NJet_"+channel_string, 6., weight_ll, 8, 0., 8);

	      FillHist(cut +"/MuonPt_"+channel_string+"_"+label, MuonColl[0].Pt(), weight_ll, 40, 0., 200);
	      FillHist(cut +"/ElectronPt_"+channel_string+"_"+label, ElectronColl[0].Pt(), weight_ll, 40, 0., 200);
	      FillHist(cut +"/MET_"+channel_string+"_"+label, GetMiniAODvMET("PuppiT1xyCorr").Pt(), weight_ll, 200, 0., 200);
              if(JetColl.size() < 6) FillHist(cut +"/NJet_"+channel_string+"_"+label, JetColl.size(), weight_ll, 8, 0., 8);
              else FillHist(cut +"/NJet_"+channel_string+"_"+label, 6., weight_ll, 8, 0., 8);


	      for(auto ilep: LeptonColl){
		if(ilep->Pt() > 25) FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_gt25PtRatio_"+label   , JetLeptonPtRatioLepAware(*ilep) , weight_ll, 200,0,2);
		if(ilep->Pt() > 25)     FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_gt25PtRel_"+label   , JetLeptonPtRelLepAware(*ilep) , weight_ll, 200,0,200);
                FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_PtRel_"+label   , JetLeptonPtRelLepAware(*ilep) , weight_ll, 200,0,200);
                FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_PtRatio_"+label   , JetLeptonPtRatioLepAware(*ilep) , weight_ll, 200,0,2);

		if(ilep->IsPrompt()){
                  FillHist( cut+ "/"+ilep->GetFlavour() + "PromptLep_Jet_PtRel_"+label   , JetLeptonPtRelLepAware(*ilep) , weight_ll, 200,0,200);

                  FillHist( cut+ "/"+ilep->GetFlavour() + "PromptLep_Jet_PtRatio_"+label   , JetLeptonPtRatioLepAware(*ilep) , weight_ll, 200,0,2);

                }


	      }

	      for(unsigned int ij=0; ij<JetAllColl.size(); ij++){
		
		bool closelep(false);
		TString lepflav="";
		TString ptlabel="";
		bool isprompt=true;
		for(auto ilep : LeptonCollV) {
		  if(ilep->DeltaR(JetAllColl.at(ij)) < 0.4) {
		    closelep=true;
		    lepflav=ilep->GetFlavour();
		    if(ilep->Pt() < 20)ptlabel="lt20_";
                    else if(ilep->Pt() < 35)ptlabel="lt35_";
                    else ptlabel="gt35_";
                    if(!ilep->IsPrompt()) isprompt=false;

		  }
		}
		
		if(!closelep){
		  if(!JetAllColl.at(ij).PassID("tight")) continue;
		  if(JetAllColl.at(ij).Pt() < 20) continue;
		}
		
		FillHist( cut+ "/JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		
		if(closelep) {
                  double mc_cj_disc_corr  = GetCorrCloseJetDisc(false, JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet)) ;

		  FillHist( cut+ "/CJ_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  FillHist( cut+ "/CJ_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  
                  FillHist( cut+ "/ModCJ_JetDisc_DeepJet_"+lepflav+"_"+label   , mc_cj_disc_corr , weight_ll, 1000,0,1);

                  FillHist( cut+ "/ModCJ_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , mc_cj_disc_corr , weight_ll, 1000,0,1);


		  if(isprompt){
                    FillHist( cut+ "/CJ_PromptLep_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
                    FillHist( cut+ "/CJ_PromptLep_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
                  }
		  else{
                    FillHist( cut+ "/CJ_NonPromptLep_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
                    FillHist( cut+ "/CJ_NonPromptLep_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
                  }

		}
		else {
		  FillHist( cut+ "/AJ_JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
                  if(JetAllColl.at(ij).Pt() > 30) FillHist( cut+ "/AJ_JetDisc_DeepJet30_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		}
		
		if(JetAllColl.at(ij).PassID("tight")) {
		  if(closelep)    FillHist( cut+ "/TightCJ_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  else FillHist( cut+ "/TightAJ_JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		}
		if(BJetColl.size() > 1){
		  FillHist( cut+ "BJetEv/JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  
		  if(closelep)    FillHist( cut+ "BJetEv/CJ_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  else FillHist( cut+ "BJetEv/AJ_JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  
		  if(JetAllColl.at(ij).PassID("tight")) {
		    if(closelep)    FillHist( cut+ "BJetEv/TightCJ_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		    else FillHist( cut+ "BJetEv/TightAJ_JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
		  }
		  
		}
	      }
	    }
	  }
	}
      }
    }
  }
  else{
  

    if(!PassTriggerSelection(dilep_channel, ev, LeptonColl,"Lep")) return;

    if(dilep_channel==EE && ElectronCollV.size() != 2) return;
    if(dilep_channel==EE &&  MuonCollV.size() > 0) return;
    if(dilep_channel==MuMu && MuonCollV.size() != 2) return;
    if(dilep_channel==MuMu && ElectronCollV.size()  > 0) return;
       
    if(dilep_channel==EE && ElectronColl.size() != 2) return;
    if(dilep_channel==EE &&  MuonColl.size() > 0) return;
    if(dilep_channel==MuMu && MuonColl.size() != 2) return;
    if(dilep_channel==MuMu && ElectronColl.size()  > 0) return;

    if(LeptonColl.size() != 2) return;

    if(dilep_channel==EE && ElectronCollV[0].Pt() < 35) return;
    if(dilep_channel==MuMu && MuonCollV[0].Pt() < 35) return;
    if(SameCharge(LeptonColl)) return;

    // Z peak 
    

    if(fabs(GetLLMass(LeptonColl)- M_Z) < 10 && METv.Pt() < 30) {
     
      vector<Jet> JetAllColl = GetAllJets();
      TString cut = "ZJets_"+channel_string;

      if(dilep_channel==MuMu)FillHist(cut +"/MuonPt_"+channel_string+"_"+label, MuonColl[1].Pt(), weight_ll, 40, 0., 200);
      if(dilep_channel==EE)  FillHist(cut +"/ElectronPt_"+channel_string+"_"+label, ElectronColl[1].Pt(), weight_ll, 40, 0., 200);

      FillHist(cut +"/MET_"+channel_string, GetMiniAODvMET("PuppiT1xyCorr").Pt(), weight_ll, 200, 0., 200);
      if(JetColl.size() < 6) FillHist(cut +"/NJet_"+channel_string, JetColl.size(), weight_ll, 8, 0., 8);
      else FillHist(cut +"/NJet_"+channel_string, 6., weight_ll, 8, 0., 8);


      FillHist(cut +"/MET_"+channel_string+"_"+label, GetMiniAODvMET("PuppiT1xyCorr").Pt(), weight_ll, 200, 0., 200);
      if(JetColl.size() < 6) FillHist(cut +"/NJet_"+channel_string+"_"+label, JetColl.size(), weight_ll, 8, 0., 8);
      else FillHist(cut +"/NJet_"+channel_string+"_"+label, 6., weight_ll, 8, 0., 8);


      for(unsigned int ij=0; ij<JetAllColl.size(); ij++){

	bool closelep(false);
	TString lepflav="";
	TString ptlabel="";
	bool isprompt=true;
	for(auto ilep : LeptonCollV) {
	  if(ilep->DeltaR(JetAllColl.at(ij)) < 0.4) {
	    closelep=true;
	    lepflav=ilep->GetFlavour();
	    if(ilep->Pt() < 20)ptlabel="lt20_";
	    else if(ilep->Pt() < 35)ptlabel="lt35_";
	    else ptlabel="gt35_";
	    if(!ilep->IsPrompt()) isprompt=false;
	  }
	}
	
	if(!closelep){
	  if(!JetAllColl.at(ij).PassID("tight")) continue;
	  if(JetAllColl.at(ij).Pt() < 20) continue;
	}

	FillHist( cut+ "/JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);

	if(closelep)    {
	  double mc_cj_disc_corr  = (lepflav=="Electron") ? GetCorrCloseJetDisc(false, JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet)):  GetCorrCloseJetDisc(true, JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet)) ;

	  FillHist( cut+ "/CJ_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
	  FillHist( cut+ "/CJ_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
	  FillHist( cut+ "/ModCJ_JetDisc_DeepJet_"+lepflav+"_"+label   , mc_cj_disc_corr , weight_ll, 1000,0,1);
	  FillHist( cut+ "/ModCJ_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , mc_cj_disc_corr , weight_ll, 1000,0,1);

	  if(isprompt){
	    FillHist( cut+ "/CJ_PromptLep_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
	    FillHist( cut+ "/CJ_PromptLep_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
	  }
	  else{
	    FillHist( cut+ "/CJ_NonPromptLep_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
	    FillHist( cut+ "/CJ_NonPromptLep_JetDisc_DeepJet_"+ptlabel+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
	  }

	}
	else FillHist( cut+ "/AJ_JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);

	if(JetAllColl.at(ij).PassID("tight")) {
	  if(closelep)    FillHist( cut+ "/TightCJ_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
	  else FillHist( cut+ "/TightAJ_JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
	}
	if(BJetColl.size() > 1){
	  FillHist( cut+ "BJetEv/JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);

	  if(closelep)    FillHist( cut+ "BJetEv/CJ_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
	  else FillHist( cut+ "BJetEv/AJ_JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);

	  if(JetAllColl.at(ij).PassID("tight")) {
	    if(closelep)    FillHist( cut+ "BJetEv/TightCJ_JetDisc_DeepJet_"+lepflav+"_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
	    else FillHist( cut+ "BJetEv/TightAJ_JetDisc_DeepJet_"+label   , JetAllColl.at(ij).GetTaggerResult(JetTagging::DeepJet) , weight_ll, 1000,0,1);
	  }
	  
	}
      }
      
      for(auto ilep : LeptonColl){

	if(ilep->Pt() > 25) FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_gt25PtRatio_"+label   , JetLeptonPtRatioLepAware(*ilep) , weight_ll, 200,0,2);
	if(ilep->Pt() > 25) 	FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_gt25PtRel_"+label   , JetLeptonPtRelLepAware(*ilep) , weight_ll, 200,0,200);
	
	FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_PtRel_"+label   , JetLeptonPtRelLepAware(*ilep) , weight_ll, 200,0,200);

	FillHist( cut+ "/"+ilep->GetFlavour() + "Jet_PtRatio_"+label   , JetLeptonPtRatioLepAware(*ilep) , weight_ll, 200,0,2);


	if(ilep->IsPrompt()){
	  FillHist( cut+ "/"+ilep->GetFlavour() + "PromptLep_Jet_PtRel_"+label   , JetLeptonPtRelLepAware(*ilep) , weight_ll, 200,0,200);

	  FillHist( cut+ "/"+ilep->GetFlavour() + "PromptLep_Jet_PtRatio_"+label   , JetLeptonPtRatioLepAware(*ilep) , weight_ll, 200,0,2);

	}

      }
    }
  }
  
  return;

}



void HNL_Lepton_ID_BDT_Studies::CheckSSFakeBreakDown(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, vector<Muon> MuonColl , double weight_ll){

  //// Check out SS Fake breakdown
  // How many By Type
  // Split by HF/LF

  if(IsData) return;

  TString channel_string = GetChannelString(dilep_channel) ;
  
  std::vector<Lepton *> LeptonColl      = MakeLeptonPointerVector(MuonColl, ElectronColl);

  vector<TString> lables = GetGenList();
  std::vector<Jet>      JetColl     = GetHNLJets("Tight", param);

  if(dilep_channel==EE){

    if(MuonColl.size()>0) return;

    vector<Electron> ElectronCollFake     = SkimLepColl(ElectronColl, param, "HFake");
    vector<Electron> ElectronCollPrompt   = SkimLepColl(ElectronColl, param, "PromptNoCF");
    
    if(ElectronCollFake.size()==0) return;
    
    for(auto iel : ElectronCollFake) {
      
      TString lep_fake_tag=MatchGenDef(All_Gens, iel); 


      vector<TString>  Types = {"q","pi+","g","pi0","ph"};

      if (std::find(Types.begin(), Types.end(), lep_fake_tag) != Types.end()){
	
        cout << "Types  Reco " << iel.MotherJetFlavour() << " pt = " << iel.Pt() << " eta = " << iel.Eta() << " phi = " << iel.Phi() <<  endl;
        cout << "CloseJet_Flavour = " << iel.CloseJet_Flavour() << endl;
        cout <<"lep_fake_tag = " << lep_fake_tag << " missing "<< endl;

        PrintMatchedGen(All_Gens, iel);

	map<TString, double> mapbdt = iel.MAPBDT();
	for(auto i : mapbdt) cout << i.first << " " << i.second << endl;

      }



      if (std::find(lables.begin(), lables.end(), lep_fake_tag) == lables.end()){

        cout << "El Reco " << iel.MotherJetFlavour() << " pt = " << iel.Pt() << " eta = " << iel.Eta() << " phi = " << iel.Phi() << endl;                                        
        cout << "CloseJet_Flavour = " << iel.CloseJet_Flavour() << endl;                                                                             
	
	cout << "lep_fake_tag = " << lep_fake_tag << " missing "<< endl;
        PrintMatchedGen(All_Gens, iel);                                                                                                                                
	
      }
      else {
	if(iel.CloseJet_FlavourInt()==0) FillCutflow("Electron", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
	if(iel.CloseJet_FlavourInt()==4) FillCutflow("Electron", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
	if(iel.CloseJet_FlavourInt()==5) FillCutflow("Electron", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
	if(iel.CloseJet_FlavourInt()==4) FillCutflow("Electron", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
        if(iel.CloseJet_FlavourInt()==5) FillCutflow("Electron", "HF_Fake_type", weight_ll, lables,lep_fake_tag);

       	
	if(ElectronCollFake.size()==1){

	  bool useevent=false;
	  for(auto ijet : JetColl) {
	    float dphi =fabs(TVector2::Phi_mpi_pi(ElectronCollFake[0].Phi()- ijet.Phi()));
	    if(dphi > 2.5 && ijet.Pt() > 30) useevent=true;
	  }
	  if(useevent) {

	    FillBDTHists(iel, "FakeCR_El_Fake_"+iel.MotherJetFlavour()+"_"+lep_fake_tag,weight_ll);

	    map<TString, double> mapbdt = iel.MAPBDT();
            for(auto i : mapbdt){
              if(!i.first.Contains("Fake_v4")) continue;
              if(i.first.Contains("LF")) continue;
              if(i.first.Contains("HF")) continue;
	      FillHist( "ElectronCR/"+iel.MotherJetFlavour()+"/"+i.first+"_"+channel_string+"_"+lep_fake_tag  , i.second, weight_ll, 100, -1., 1., "MVA");
	      if(ElectronCollFake[0].IsBB() && ElectronCollFake[0].Pt() < 20)      FillHist( "ElectronCR/"+iel.MotherJetFlavour()+"/BB_Pt1_"+i.first+"_"+channel_string+"_"+lep_fake_tag  , i.second, weight_ll, 100, -1., 1., "MVA");
	      else if(ElectronCollFake[0].IsBB() && ElectronCollFake[0].Pt() < 30)      FillHist( "ElectronCR/"+iel.MotherJetFlavour()+"/BB_Pt2_"+i.first+"_"+channel_string+"_"+lep_fake_tag  , i.second, weight_ll, 100, -1., 1., "MVA");
	      else if(ElectronCollFake[0].IsBB() && ElectronCollFake[0].Pt() < 50)      FillHist( "ElectronCR/"+iel.MotherJetFlavour()+"/BB_Pt3_"+i.first+"_"+channel_string+"_"+lep_fake_tag  , i.second, weight_ll, 100, -1., 1., "MVA");
	      else  FillHist( "ElectronCR/"+iel.MotherJetFlavour()+"/BB_Pt4_"+i.first+"_"+channel_string+"_"+lep_fake_tag  , i.second, weight_ll, 100, -1., 1., "MVA");

              if(ElectronCollFake[0].IsEC() && ElectronCollFake[0].Pt() < 20)      FillHist( "ElectronCR/"+iel.MotherJetFlavour()+"/EC_Pt1_"+i.first+"_"+channel_string+"_"+lep_fake_tag  , i.second, weight_ll, 100, -1., 1., "MVA");
              else if(ElectronCollFake[0].IsEC() && ElectronCollFake[0].Pt() < 30)      FillHist( "ElectronCR/"+iel.MotherJetFlavour()+"/EC_Pt2_"+i.first+"_"+channel_string+"_"+lep_fake_tag  , i.second, weight_ll, 100, -1., 1., "MVA");
              else if(ElectronCollFake[0].IsEC() && ElectronCollFake[0].Pt() < 50)      FillHist( "ElectronCR/"+iel.MotherJetFlavour()+"/EC_Pt3_"+i.first+"_"+channel_string+"_"+lep_fake_tag  , i.second, weight_ll, 100, -1., 1., "MVA");
	      else FillHist( "ElectronCR/"+iel.MotherJetFlavour()+"/EC_Pt4_"+i.first+"_"+channel_string+"_"+lep_fake_tag  , i.second, weight_ll, 100, -1., 1., "MVA");

            }
	    if(iel.CloseJet_FlavourInt()==0) FillCutflow("CRElectron", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(iel.CloseJet_FlavourInt()==4) FillCutflow("CRElectron", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(iel.CloseJet_FlavourInt()==5) FillCutflow("CRElectron", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(iel.CloseJet_FlavourInt()==4) FillCutflow("CRElectron", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(iel.CloseJet_FlavourInt()==5) FillCutflow("CRElectron", "HF_Fake_type", weight_ll, lables,lep_fake_tag);

	  }
	}
	
	if(LeptonColl.size()==2){
	  if(SameCharge(LeptonColl)){
	    
	    FillBDTHists(iel, "SSEl_Fake/"+iel.MotherJetFlavour()+"/"+lep_fake_tag,weight_ll);
	    
	    if(iel.CloseJet_FlavourInt()==0) FillCutflow("SSElectron", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(iel.CloseJet_FlavourInt()==4) FillCutflow("SSElectron", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);	  
	    if(iel.CloseJet_FlavourInt()==5) FillCutflow("SSElectron", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(iel.CloseJet_FlavourInt()==4) FillCutflow("SSElectron", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(iel.CloseJet_FlavourInt()==5) FillCutflow("SSElectron", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(JetColl.size() == 0){
	      if(iel.CloseJet_FlavourInt()==0) FillCutflow("SSElectron_0j", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==4) FillCutflow("SSElectron_0j", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==5) FillCutflow("SSElectron_0j", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==4) FillCutflow("SSElectron_0j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==5) FillCutflow("SSElectron_0j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	    }
	    else     if(JetColl.size() == 1){
	      if(iel.CloseJet_FlavourInt()==0) FillCutflow("SSElectron_1j", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==4) FillCutflow("SSElectron_1j", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==5) FillCutflow("SSElectron_1j", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==4) FillCutflow("SSElectron_1j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==5) FillCutflow("SSElectron_1j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	    }
	    else{
	      if(iel.CloseJet_FlavourInt()==0) FillCutflow("SSElectron_2j", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==4) FillCutflow("SSElectron_2j", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==5) FillCutflow("SSElectron_2j", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==4) FillCutflow("SSElectron_2j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	      if(iel.CloseJet_FlavourInt()==5) FillCutflow("SSElectron_2j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	      
	    }
	  }
	  else{
	    if(iel.CloseJet_FlavourInt()==0) FillCutflow("OSElectron", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
            if(iel.CloseJet_FlavourInt()==4) FillCutflow("OSElectron", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
            if(iel.CloseJet_FlavourInt()==5) FillCutflow("OSElectron", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
            if(iel.CloseJet_FlavourInt()==4) FillCutflow("OSElectron", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
            if(iel.CloseJet_FlavourInt()==5) FillCutflow("OSElectron", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
            if(JetColl.size() == 0){
              if(iel.CloseJet_FlavourInt()==0) FillCutflow("OSElectron_0j", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==4) FillCutflow("OSElectron_0j", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==5) FillCutflow("OSElectron_0j", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==4) FillCutflow("OSElectron_0j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==5) FillCutflow("OSElectron_0j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
            }
            else     if(JetColl.size() == 1){
              if(iel.CloseJet_FlavourInt()==0) FillCutflow("OSElectron_1j", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==4) FillCutflow("OSElectron_1j", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==5) FillCutflow("OSElectron_1j", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==4) FillCutflow("OSElectron_1j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==5) FillCutflow("OSElectron_1j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
            }
            else{
              if(iel.CloseJet_FlavourInt()==0) FillCutflow("OSElectron_2j", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==4) FillCutflow("OSElectron_2j", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==5) FillCutflow("OSElectron_2j", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==4) FillCutflow("OSElectron_2j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
              if(iel.CloseJet_FlavourInt()==5) FillCutflow("OSElectron_2j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	      
            }

	  }
	}
      }
    }
    
    //// Now events are SS dilepton with 1/2 fakes
    
    //FillAllElectronPlots("FakeElectron","MC"+channel_string,ElectronCollFake,weight_ll);
    //FillAllElectronPlots("PromptElectron","MC"+channel_string,ElectronCollPrompt,weight_ll);

    //   if(!CheckLeptonFlavourForChannel(dilep_channel, LeptonColl)) return; // Check if EE cahnnel has 2 el                                                                                                                                                                                         
    //if(!SameCharge(LeptonColl)) return;

    //FillAllElectronPlots("SS_FakeElectron","MC"+channel_string,ElectronCollFake,weight_ll);
    //FillAllElectronPlots("SS_PromptElectron","MC"+channel_string,ElectronCollPrompt,weight_ll);

    
  }

  if(dilep_channel==MuMu){

    /// JOHN 

    if(ElectronColl.size()>0) return;

    vector<Muon> MuonCollFake     = SkimLepColl(MuonColl, param, "HFake");
    vector<Muon> MuonCollPrompt   = SkimLepColl(MuonColl, param, "Prompt");
    
    if(MuonCollFake.size()==0) return;

    //// Now events are SS dilepton with 1/2 fakes                                                                                                                                            
    //    FillAllMuonPlots("PromptMuon","MC"+channel_string,MuonCollPrompt,weight_ll);                                                                                                        
    //FillAllMuonPlots("FakeMuon"  ,"MC"+channel_string,MuonCollFake,weight_ll);
                                                 
    for(auto imu : MuonCollFake) {

      TString lep_fake_tag=MatchGenDef(All_Gens, imu);

      vector<TString>  Types = {"q","pi+","g","pi0"};

      if (std::find(Types.begin(), Types.end(), lep_fake_tag) != Types.end()){

        cout << "Types  Reco " << imu.MotherJetFlavour() << " pt = " << imu.Pt() << " eta = " << imu.Eta() << " phi = " << imu.Phi() <<  endl;
        cout << "CloseJet_Flavour = " << imu.CloseJet_Flavour() << endl;
        cout <<"lep_fake_tag = " << lep_fake_tag << " missing "<< endl;

        PrintMatchedGen(All_Gens, imu);

        map<TString, double> mapbdt = imu.MAPBDT();
	for(auto i : mapbdt) cout << i.first <<" " << i.second<< endl;

      }


      if (std::find(lables.begin(), lables.end(), lep_fake_tag) == lables.end()){
	cout << "MUON MISSING Reco " << imu.MotherJetFlavour() << " pt = " << imu.Pt() << " eta = " << imu.Eta() << " phi = " << imu.Phi() << endl;
	cout << "CloseJet_Flavour = " << imu.CloseJet_Flavour() << endl;
	cout <<"lep_fake_tag = " << lep_fake_tag << " missing "<< endl;
        PrintMatchedGen(All_Gens, imu);

      }
      //else if(lep_fake_tag=="__"){
      //	cout << "Mu Reco " << imu.MotherJetFlavour() << " pt = " << imu.Pt() << " eta = " << imu.Eta() << " phi = " << imu.Phi() << endl;
      // cout << "Mu CloseJet_Flavour = " << imu.CloseJet_Flavour() << endl;
      // cout <<"lep_fake_tag = " << lep_fake_tag << " missing "<< endl;
      //	TString lep_fake_tag2=MatchGenDef(All_Gens, imu,true);

      //        PrintMatchedGen(All_Gens, imu);

      //      }
      else{
	if(imu.CloseJet_FlavourInt()==0) FillCutflow("Muon", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
	if(imu.CloseJet_FlavourInt()==4) FillCutflow("Muon", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
	if(imu.CloseJet_FlavourInt()==5) FillCutflow("Muon", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
        if(imu.CloseJet_FlavourInt()==4) FillCutflow("Muon", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
        if(imu.CloseJet_FlavourInt()==5) FillCutflow("Muon", "HF_Fake_type", weight_ll, lables,lep_fake_tag);


	if(MuonCollFake.size()==1){

          bool useevent=false;
          for(auto ijet : JetColl) {
            float dphi =fabs(TVector2::Phi_mpi_pi(MuonCollFake[0].Phi()- ijet.Phi()));
            if(dphi > 2.5) useevent=true;
          }
          if(useevent) {

            FillBDTHists(imu, "FakeCR_Mu_Fake/"+imu.MotherJetFlavour()+"/"+lep_fake_tag,weight_ll);

	    map<TString, double> mapbdt = imu.MAPBDT();
	    for(auto i : mapbdt){
	      if(i.first.Contains("LF")) continue;

	      FillHist( "MuonCR/"+imu.MotherJetFlavour()+"/"+i.first+"_"+channel_string +"_"+lep_fake_tag , i.second, weight_ll, 100, -1., 1., "MVA");
	      if(MuonCollFake[0].IsBB() && MuonCollFake[0].Pt() < 20) 	      FillHist( "MuonCR/"+imu.MotherJetFlavour()+"/BB_Pt1_"+i.first+"_"+channel_string +"_"+lep_fake_tag , i.second, weight_ll, 100, -1., 1., "MVA");
	      else if(MuonCollFake[0].IsBB() && MuonCollFake[0].Pt() < 30) 	      FillHist( "MuonCR/"+imu.MotherJetFlavour()+"/BB_Pt2_"+i.first+"_"+channel_string +"_"+lep_fake_tag , i.second, weight_ll, 100, -1., 1., "MVA");
	      else if(MuonCollFake[0].IsBB() && MuonCollFake[0].Pt() < 50) 	      FillHist( "MuonCR/"+imu.MotherJetFlavour()+"/BB_Pt3_"+i.first+"_"+channel_string +"_"+lep_fake_tag , i.second, weight_ll, 100, -1., 1., "MVA");
	      else FillHist( "MuonCR/"+imu.MotherJetFlavour()+"/BB_Pt4_"+i.first+"_"+channel_string +"_"+lep_fake_tag , i.second, weight_ll, 100, -1., 1., "MVA");

	      if(MuonCollFake[0].IsEC() && MuonCollFake[0].Pt() < 20)         FillHist( "MuonCR/"+imu.MotherJetFlavour()+"/EC_Pt1_"+i.first+"_"+channel_string +"_"+lep_fake_tag , i.second, weight_ll, 100, -1., 1., "MVA");
	      else if(MuonCollFake[0].IsEC() && MuonCollFake[0].Pt() < 30)         FillHist( "MuonCR/"+imu.MotherJetFlavour()+"/EC_Pt2_"+i.first+"_"+channel_string +"_"+lep_fake_tag , i.second, weight_ll, 100, -1., 1., "MVA");
	      else if(MuonCollFake[0].IsEC() && MuonCollFake[0].Pt() < 50)         FillHist( "MuonCR/"+imu.MotherJetFlavour()+"/EC_Pt3_"+i.first+"_"+channel_string +"_"+lep_fake_tag , i.second, weight_ll, 100, -1., 1., "MVA");
	      else FillHist( "MuonCR/"+imu.MotherJetFlavour()+"/EC_Pt4_"+i.first+"_"+channel_string +"_"+lep_fake_tag , i.second, weight_ll, 100, -1., 1., "MVA");

	    }
	    
            if(imu.CloseJet_FlavourInt()==0) FillCutflow("CRMuon", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==4) FillCutflow("CRMuon", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==5) FillCutflow("CRMuon", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==4) FillCutflow("CRMuon", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==5) FillCutflow("CRMuon", "HF_Fake_type", weight_ll, lables,lep_fake_tag);

          }
        }
	if(SameCharge(LeptonColl))  {

	  if(imu.CloseJet_FlavourInt()==0) FillCutflow("SSMuon", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
	  if(imu.CloseJet_FlavourInt()==4) FillCutflow("SSMuon", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);	  
	  if(imu.CloseJet_FlavourInt()==5) FillCutflow("SSMuon", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
	  if(imu.CloseJet_FlavourInt()==4) FillCutflow("SSMuon", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
          if(imu.CloseJet_FlavourInt()==5) FillCutflow("SSMuon", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	  if(JetColl.size() == 0){
	    if(imu.CloseJet_FlavourInt()==0) FillCutflow("SSMuon_0j", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(imu.CloseJet_FlavourInt()==4) FillCutflow("SSMuon_0j", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(imu.CloseJet_FlavourInt()==5) FillCutflow("SSMuon_0j", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(imu.CloseJet_FlavourInt()==4) FillCutflow("SSMuon_0j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	    if(imu.CloseJet_FlavourInt()==5) FillCutflow("SSMuon_0j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
    	  }
	  else           if(JetColl.size() == 1){
            if(imu.CloseJet_FlavourInt()==0) FillCutflow("SSMuon_1j", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==4) FillCutflow("SSMuon_1j", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==5) FillCutflow("SSMuon_1j", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==4) FillCutflow("SSMuon_1j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==5) FillCutflow("SSMuon_1j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
          }

	  else{
	    if(imu.CloseJet_FlavourInt()==0) FillCutflow("SSMuon_2j", "LF_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==4) FillCutflow("SSMuon_2j", "HFC_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==5) FillCutflow("SSMuon_2j", "HFB_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==4) FillCutflow("SSMuon_2j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
            if(imu.CloseJet_FlavourInt()==5) FillCutflow("SSMuon_2j", "HF_Fake_type", weight_ll, lables,lep_fake_tag);
	  }
	}
      }
      
    }
    return;
    if(!SameCharge(LeptonColl))return;

    FillMuonCollPlots(param,"SS_FakeMuon_"+channel_string,MuonCollFake,weight_ll);
    FillMuonCollPlots(param,"SS_PromptMuon_"+channel_string,MuonCollPrompt,weight_ll);
        
  }
  
    

  return; 

}

void HNL_Lepton_ID_BDT_Studies::MakeBDTPlots(AnalyzerParameter param,HNL_LeptonCore::Channel dilep_channel, vector<Electron> ElectronColl, TString ID, double weight_ll){

  TString channel_string = GetChannelString(dilep_channel) ;

  std::vector<Lepton *> LeptonColl      = MakeLeptonPointerVector(ElectronColl);
  
  if(!IsData){

    vector<Electron> ElectronCollFake     = SkimLepColl(ElectronColl, param, "HFake");
    FillAllElectronPlots(param,"FakeElectron_"+channel_string,ElectronCollFake,weight_ll);

    vector<Electron> ElectronCollCF     = SkimLepColl(ElectronColl,  param, "CF");
    FillAllElectronPlots(param,"CFElectron_"+channel_string,ElectronCollCF,weight_ll);

    vector<Electron> ElectronCollConv     = SkimLepColl(ElectronColl, param, "NHConv");
    FillAllElectronPlots(param,"ConvElectron_"+channel_string,ElectronCollConv,weight_ll);

    vector<Electron> ElectronCollPrompt   = SkimLepColl(ElectronColl, param, "PromptNoCF");
    FillAllElectronPlots(param,"PromptElectron_"+channel_string,ElectronCollPrompt,weight_ll);
  }
  

  if(!CheckLeptonFlavourForChannel(dilep_channel, LeptonColl)) return; // Check if EE cahnnel has 2 el                            
  Event ev = GetEvent();
  
  if(!PassTriggerSelection(dilep_channel, ev, LeptonColl,"Dilep")) return;

  if(LeptonColl.size() != 2) return;
 
  Particle METv = GetMiniAODvMET("PuppiT1xyCorr"); // reyturns MET with systematic correction                                            

  std::vector<Jet> BJetColl  = GetHNLJets("BJetM", param);
  // use Z peak                                                                                                                              
  if(fabs(GetLLMass(LeptonColl)- M_Z) < 10 && METv.Pt() < 30&& BJetColl.size()==0) {
    
    if(ElectronColl[0].PassID("passPOGTight") && ElectronColl[1].PassID("passPOGTight") ){
      AnalyzerParameter paramPOG=param;
      paramPOG.k.Electron_ID_SF = "passTightID";
      double POGSF = 1.;
      EvalElectronIDWeight(ElectronColl,paramPOG, POGSF);

      FillHist(channel_string+"/"+ID+"/llmass_"+channel_string, GetLLMass(LeptonColl), weight_ll*POGSF, 70, 50., 120);
      FillHist(channel_string+"/"+ID+"/NVTX_"+channel_string, nPileUp, weight_ll*POGSF, 100, 0., 100);
      FillHist(channel_string+"/"+ID+"/MET_"+channel_string, GetMiniAODvMET("PuppiT1xyCorr").Pt(), weight_ll*POGSF, 100, 0., 100);
    }
    
    for(auto iel : ElectronColl) {
      if(iel.PassID("HNL_ULID_Baseline")){
	
	map<TString, double> mapbdt = iel.MAPBDT();
	for(auto i : mapbdt){
	  if(SameCharge(ElectronColl))FillHist( channel_string+"_SS/"+i.first+"_"+channel_string  , i.second, weight_ll, 100, -1., 1., "MVA");
	  else FillHist( channel_string+"_OS/"+i.first+"_"+channel_string  , i.second, weight_ll, 100, -1., 1., "MVA");
	}
      }
    }
    

    std::vector<FatJet>   AK8_JetColl = GetHNLAK8Jets("", param);
    std::vector<Jet>      JetColl     = GetHNLJets("Tight", param);

    if(BJetColl.size() > 1) return;
    if(METv.Pt() > 30) return;
    if(!ZmassOSSFWindowCheck(LeptonColl, 10.)) return;
    
    FillAllElectronPlots(param,"OSElectronZ",ElectronColl,weight_ll);

    vector<Electron> ElectronCollDATATight;
    vector<Electron> ElectronCollDATAFake;
    for(auto iel : ElectronColl){
      if(iel.IsGsfCtfScPixChargeConsistent() && iel.SIP3D() < 3) ElectronCollDATATight.push_back(iel);
      if( iel.SIP3D() > 5) ElectronCollDATAFake.push_back(iel);
    }
    FillAllElectronPlots(param,"OSElectron_ZTight",ElectronCollDATATight,weight_ll);
    FillAllElectronPlots(param,"OSElectron_ZFake" ,ElectronCollDATAFake,weight_ll);

    
    std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, 10., 2.5);
    std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID, 5., 2.4);
    
    std::vector<Lepton *> LeptonCollV      = MakeLeptonPointerVector(ElectronCollV);
    
    std::vector<Tau> TauColl;

    Particle MET = GetMiniAODvMET("PuppiT1xyCorr");
    Fill_RegionPlots(param,"ZPeak" ,  TauColl, JetColl, AK8_JetColl, LeptonColl, MET , nPV, weight_ll);
  }

  return;

}



HNL_Lepton_ID_BDT_Studies::HNL_Lepton_ID_BDT_Studies(){


}
 
HNL_Lepton_ID_BDT_Studies::~HNL_Lepton_ID_BDT_Studies(){

}

