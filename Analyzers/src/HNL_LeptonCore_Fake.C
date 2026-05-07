#include "HNL_LeptonCore.h"

double HNL_LeptonCore::GetPtPartonSF(Lepton  Lep, TString LooseID,AnalyzerParameter param){

  if(!LooseID.Contains("ULID")) return 1;

  /// Treat HNL_HighPt_ULID ... same as HNL_ULID
  if(LooseID.Contains("HighPt")) LooseID=LooseID.ReplaceAll("_HighPt","");

  bool IsMuon=(Lep.LeptonFlavour() == Lepton::MUON);

  double SystFactor = 1;
  if(param.syst_ == AnalyzerParameter::FRPartonSFUp) SystFactor = 1.1;
  if(param.syst_ == AnalyzerParameter::FRPartonSFDown) SystFactor = 0.9;


  // If CheckProfile flag then new IDs are being ran so function would crash 
  // if (HasFlag("CheckProfile"))   return 1;
  // if (HasFlag("CheckProfileEE")) return 1;
  if (LooseID.Contains("Veto"))  return 1;

  bool UseDataSF=true;
  if(IsDATA) UseDataSF=true;
  else UseDataSF=false;

  if(!UseDataSF){

    //// Access SF from text file 
    if(IsMuon){

      TString FSFKey = DataEra + "_"+ Lep.GetEtaRegion() +"_MuMu_"+LooseID + "_MC_QCD" ;
      
      map<TString, double>::iterator fit = MakeSFmap.find(FSFKey);
      
      if(fit == MakeSFmap.end()) {
	cout << "MakeSFmap has no key " <<  FSFKey << endl;
	exit(EXIT_FAILURE);
      }
      
      return fit->second * SystFactor ;
    }
    else{
      
      TString FSFKey = DataEra + "_"+ Lep.etaRegionString() +"_EE_"+LooseID + "_MC_QCD" ;
      map<TString, double>::iterator fit = MakeSFmap.find(FSFKey);
      
      if(fit == MakeSFmap.end()) {
	cout << "MakeSFmap has no key " <<  FSFKey << endl;
	exit(EXIT_FAILURE);
      }
      return fit->second* SystFactor ;
    }
  }

  vector<TString> MVAIDs = {"TopMVA","HNL_ULID"};
  bool isMVAID=false;
  for(auto ID : MVAIDs){
    if(LooseID.Contains("ID")) isMVAID=true;
  }


  /// Check ID needs SF applied. i.e. BDT ID only need SF
  if(!isMVAID) return 1.;
  
  ///// Some IDs are predefined by other groups and we use their values
  if(LooseID == "HNL_TopMVA_FO_MM") return 0.661* SystFactor ;
  if(LooseID == "HNL_HN3L") return 0.661* SystFactor ;
  
  
  /// MUON   //https://jalmond.web.cern.ch/jalmond/SNU/WebPlots/HNL/HNL_LeptonFakeRatesMC/HNL_LeptonFakeRatesMC/2017/HNL_GetProfileSFMC_Jan24_Muon/?match=v1_M
  
  /// DeepJet split

  /// FCO IDs are for checks regarding Conv backgrounds
  if(LooseID.Contains("HNL_ULID_FCO")){
    return GetPtPartonSF(Lep,"HNL_ULID_FO", param);
  }

  /// Scan Numbers
  if(UseDataSF){
    if(IsMuon){
      
      //// Muon Data we use Inclusive of Barrel + Endcap
      TString FSFKey = GetYearString()  + "_Inclusive_MuMu_"+LooseID + "_DATA" ;
      map<TString, double>::iterator fit = MakeSFmap.find(FSFKey);

      if(fit == MakeSFmap.end()) {
	cout << "MakeSFmap has no key " <<  FSFKey << endl;
	exit(EXIT_FAILURE);
      }

      return fit->second* SystFactor ;
    }
    else{

      if(LooseID.Contains("HEEP")) LooseID = LooseID.ReplaceAll("_HEEP","");
      TString FSFKey = GetYearString()  + "_"+Lep.GetEtaRegion()+"_EE_"+LooseID + "_DATA" ;

      map<TString, double>::iterator fit = MakeSFmap.find(FSFKey);

      if(fit == MakeSFmap.end()) {
        cout << "MakeSFmap has no key " <<  FSFKey << endl;
        exit(EXIT_FAILURE);
      }

      return fit->second* SystFactor ;

    }
  }


  cout << "ERROR in GetPtPartonSF " << IsMuon << " " << LooseID << endl;
  exit(EXIT_FAILURE);

}
  
int HNL_LeptonCore::GetFakeLepSrcType(const Lepton& Lep, vector<Jet>& JetColl){
  //Type: -1: Unmatched, 1:L, 2:C, 3:B                                                                                                                                                                                                                                                                                                                                                                                                   
  int SrcType=-1;
  bool NearB=false, NearC=false, NearL=false;
  for(unsigned int ij=0; ij<JetColl.size(); ij++){
    if(Lep.DeltaR(JetColl.at(ij))<0.4){
      if     (JetColl.at(ij).hadronFlavour()==5){ NearB=true; break; }//1)                                                                                                                                                                                                                                                                                                                                                               
      else if(JetColl.at(ij).hadronFlavour()==4){ NearC=true; }
      else if(JetColl.at(ij).hadronFlavour()==0){ NearL=true; }
    }
  }

  if     (NearB) SrcType=3;
  else if(NearC) SrcType=2;
  else if(NearL) SrcType=1;

  return SrcType;
  //1) Higher Priority to B. if there's multiple near jets, then b-jet has higher priority                                                                                                                                                                                                                                                                                                                                              

}





/*                                   
             FAKE RATE FUNCTIONS                                                                                                                                                                              
*/


double HNL_LeptonCore::GetFakeRateElectron(Electron el, AnalyzerParameter param){

  //// Access individual lepton Fake Rate                                                                                                   

  double  LepEta = el.fEta();
  double  LepPt  = el.PtMaxed(80);
  TString  fr_key = param.k.Electron_FR;

  if(param.syst_ == AnalyzerParameter::FRAJUp) fr_key = fr_key.ReplaceAll("40","30");
  if(param.syst_ == AnalyzerParameter::FRAJDown) fr_key = fr_key.ReplaceAll("40","60");

  int FRStatSyst=0;
  if(param.syst_ == AnalyzerParameter::FRElectronUp)   FRStatSyst=1;
  if(param.syst_ == AnalyzerParameter::FRElectronDown) FRStatSyst=-1;
  if(param.syst_ == AnalyzerParameter::FRElectronRateUp)   FRStatSyst=2;
  if(param.syst_ == AnalyzerParameter::FRElectronRateDown) FRStatSyst=-2;
  if(param.syst_ == AnalyzerParameter::FRElectronHighPtUp)   FRStatSyst=10;
  if(param.syst_ == AnalyzerParameter::FRElectronHighPtDown) FRStatSyst=-10;
  
  return fakeEst->GetElectronFakeRate(param.Electron_Tight_ID, fr_key,param.FakeRateMethod, param.FakeRateParam, LepEta, LepPt, el.LeptonFakeTagger() ,FRStatSyst);


}
double HNL_LeptonCore::GetFakeRateMuon(Muon mu, AnalyzerParameter param){

  double  LepEta = mu.fEta();
  double  LepPt  = mu.PtMaxed(80);
  TString  fr_key = param.k.Muon_FR;

  if(param.syst_ == AnalyzerParameter::FRAJUp) fr_key = fr_key.ReplaceAll("40","30");
  if(param.syst_ == AnalyzerParameter::FRAJDown) fr_key = fr_key.ReplaceAll("40","60");
  
  int FRStatSyst = 0;
  if(param.syst_ == AnalyzerParameter::FRMuonUp)   FRStatSyst=1;
  if(param.syst_ == AnalyzerParameter::FRMuonDown) FRStatSyst=-1;
  if(param.syst_ == AnalyzerParameter::FRMuonRateUp)   FRStatSyst=2;
  if(param.syst_ == AnalyzerParameter::FRMuonRateDown) FRStatSyst=-2;
  if(param.syst_ == AnalyzerParameter::FRMuonHighPtUp)   FRStatSyst=10;
  if(param.syst_ == AnalyzerParameter::FRMuonHighPtDown) FRStatSyst=-10;

  return fakeEst->GetMuonFakeRate(param.Muon_Tight_ID, fr_key,param.FakeRateMethod,param.FakeRateParam,LepEta, LepPt, mu.LeptonFakeTagger() ,FRStatSyst);

}

double HNL_LeptonCore::GetFakeWeight(std::vector<Lepton *> leps, AnalyzerParameter _param){

  //// Access event weight based on Lepton collection                                                               
  if(!IsDATA) return 1.;

  fakeEst->IgnoreNoHist=false;

  ///// SINGLE LEPTON EVENTS 
  double this_weight = -1.;

  if(leps.size() == 1){

    TString fr_key = (leps[0]->LeptonFlavour() == Lepton::ELECTRON) ?  _param.k.Electron_FR : _param.k.Muon_FR;
    TString pr_key = (leps[0]->LeptonFlavour() == Lepton::ELECTRON) ?  _param.k.Electron_PR : _param.k.Muon_PR;

    bool IsMuon    = (leps[0]->LeptonFlavour() != Lepton::ELECTRON);
    TString ID     = (leps[0]->LeptonFlavour() == Lepton::ELECTRON) ?  _param.Electron_Tight_ID : _param.Muon_Tight_ID;

    if(_param.syst_ == AnalyzerParameter::FRAJUp) fr_key = fr_key.ReplaceAll("40","30");
    if(_param.syst_ == AnalyzerParameter::FRAJDown) fr_key = fr_key.ReplaceAll("40","60");
   
    if(!leps[0]->LepIDSet()) {      cout << "Lepton ID not set" << endl;      exit(EXIT_FAILURE);    }

    int FRStatSyst = 0;
    if((_param.syst_ == AnalyzerParameter::FRElectronUp)||(_param.syst_ == AnalyzerParameter::FRMuonUp)) FRStatSyst=1;
    if((_param.syst_ == AnalyzerParameter::FRElectronDown)||(_param.syst_ == AnalyzerParameter::FRMuonDown)) FRStatSyst=-1;
    if((_param.syst_ == AnalyzerParameter::FRElectronRateUp)||(_param.syst_ == AnalyzerParameter::FRMuonRateUp)) FRStatSyst=2;
    if((_param.syst_ == AnalyzerParameter::FRElectronRateDown)||(_param.syst_ == AnalyzerParameter::FRMuonRateDown)) FRStatSyst=-2;
    if((_param.syst_ == AnalyzerParameter::FRElectronHighPtUp)||(_param.syst_ == AnalyzerParameter::FRMuonHighPtUp)) FRStatSyst=10;
    if((_param.syst_ == AnalyzerParameter::FRElectronHighPtDown)||(_param.syst_ == AnalyzerParameter::FRMuonHighPtDown)) FRStatSyst=-10;



    double this_fr = fakeEst->GetFakeRate  (IsMuon, ID,  fr_key, _param.FakeRateMethod, _param.FakeRateParam, leps[0]->fEta(), leps[0]->Pt(),leps[0]->LeptonFakeTagger(),FRStatSyst);
    double this_pr = fakeEst->GetPromptRate(_param.ApplyPR, IsMuon, ID, pr_key, leps[0]->fEta(), leps[0]->UncorrectedPt());

    this_weight=  fakeEst->CalculateLepWeight(this_pr, this_fr, leps[0]->PassLepID() );
    
    if(isnan(this_weight)) {
      cout << "Single Lepton Fake rate is NaN" << endl;
      cout << "IsMuon = " << IsMuon << " fr_key = " << fr_key << " ID = " << ID << " this_fr = " << this_fr << " this_pr = " << this_pr << endl;
      exit(EXIT_FAILURE);
    }
    return this_weight;
  }

  ////// DILEPTON EVENTS 
  if (leps.size() == 2){

    if(!(leps[0]->LepIDSet() || leps[1]->LepIDSet())) {
      cout << "Lepton ID not set" << endl;
      exit(EXIT_FAILURE);
    }

    TString fr_key1 = (leps[0]->LeptonFlavour() == Lepton::ELECTRON) ?  _param.k.Electron_FR : _param.k.Muon_FR;
    TString fr_key2 = (leps[1]->LeptonFlavour() == Lepton::ELECTRON) ?  _param.k.Electron_FR : _param.k.Muon_FR;
    TString pr_key = (leps[0]->LeptonFlavour() == Lepton::ELECTRON) ?  _param.k.Electron_PR : _param.k.Muon_PR;

    /// Apply Syst
    if(_param.syst_ == AnalyzerParameter::FRAJUp) fr_key1 = fr_key1.ReplaceAll("40","30");
    if(_param.syst_ == AnalyzerParameter::FRAJDown) fr_key1 = fr_key1.ReplaceAll("40","60");
    if(_param.syst_ == AnalyzerParameter::FRAJUp) fr_key2 = fr_key2.ReplaceAll("40","30");
    if(_param.syst_ == AnalyzerParameter::FRAJDown) fr_key2 = fr_key2.ReplaceAll("40","60");

    if(run_Debug){
      if(leps[0]->LeptonFlavour() == Lepton::ELECTRON) cout << "_param.Electron_Tight_ID = " << _param.Electron_Tight_ID <<  " fr_key1 = " << fr_key1 << endl;
      else cout << "_param.Muon_Tight_ID  = " <<  _param.Muon_Tight_ID <<  " fr_key1 = " << fr_key1 <<endl;
    }

    
    TString ID1   =  (leps[0]->LeptonFlavour() == Lepton::ELECTRON) ?  _param.Electron_Tight_ID : _param.Muon_Tight_ID;
    TString ID2   =  (leps[1]->LeptonFlavour() == Lepton::ELECTRON) ?  _param.Electron_Tight_ID : _param.Muon_Tight_ID;

    bool IsMuon1  = (leps[0]->LeptonFlavour() != Lepton::ELECTRON);
    bool IsMuon2  = (leps[1]->LeptonFlavour() != Lepton::ELECTRON);

    int FRStatSystLep1 = 0;
    int FRStatSystLep2 = 0;

    if(IsMuon1){
      if(_param.syst_ == AnalyzerParameter::FRMuonUp) FRStatSystLep1=1;
      if(_param.syst_ == AnalyzerParameter::FRMuonDown) FRStatSystLep1=-1;
      if(_param.syst_ == AnalyzerParameter::FRMuonRateUp)FRStatSystLep1=2;
      if(_param.syst_ == AnalyzerParameter::FRMuonRateDown) FRStatSystLep1=-2;
      if(_param.syst_ ==AnalyzerParameter::FRMuonHighPtUp)      FRStatSystLep1=10;
      if(_param.syst_ ==AnalyzerParameter::FRMuonHighPtDown)    FRStatSystLep1=-10;
    }
    else{
      if(_param.syst_ == AnalyzerParameter::FRElectronUp) FRStatSystLep1=1;
      if(_param.syst_ == AnalyzerParameter::FRElectronDown) FRStatSystLep1=-1;
      if(_param.syst_ == AnalyzerParameter::FRElectronRateUp)FRStatSystLep1=2;
      if(_param.syst_ == AnalyzerParameter::FRElectronRateDown) FRStatSystLep1=-2;
      if(_param.syst_ ==AnalyzerParameter::FRElectronHighPtUp)      FRStatSystLep1=10;
      if(_param.syst_ ==AnalyzerParameter::FRElectronHighPtDown)    FRStatSystLep1=-10;
    }
    if(IsMuon2){
       if(_param.syst_ == AnalyzerParameter::FRMuonUp) FRStatSystLep2=1;
       if(_param.syst_ == AnalyzerParameter::FRMuonDown) FRStatSystLep2=-1;
       if(_param.syst_ == AnalyzerParameter::FRMuonRateUp)FRStatSystLep2=2;
       if(_param.syst_ == AnalyzerParameter::FRMuonRateDown) FRStatSystLep2=-2;
      if(_param.syst_ ==AnalyzerParameter::FRMuonHighPtUp)      FRStatSystLep2=10;
      if(_param.syst_ ==AnalyzerParameter::FRMuonHighPtDown)    FRStatSystLep2=-10;
    }
    else{
      if(_param.syst_ == AnalyzerParameter::FRElectronUp) FRStatSystLep2=1;
      if(_param.syst_ == AnalyzerParameter::FRElectronDown) FRStatSystLep2=-1;
      if(_param.syst_ == AnalyzerParameter::FRElectronRateUp)FRStatSystLep2=2;
      if(_param.syst_ == AnalyzerParameter::FRElectronRateDown) FRStatSystLep2=-2;
      if(_param.syst_ ==AnalyzerParameter::FRElectronHighPtUp)      FRStatSystLep2=10;
      if(_param.syst_ ==AnalyzerParameter::FRElectronHighPtDown)    FRStatSystLep2=-10;
    }



    double this_fr1 =  fakeEst->GetFakeRate(IsMuon1, ID1,  fr_key1, _param.FakeRateMethod, _param.FakeRateParam, leps[0]->fEta(), leps[0]->Pt(),leps[0]->LeptonFakeTagger(),FRStatSystLep1);
    double this_fr2 =  fakeEst->GetFakeRate(IsMuon2, ID2,  fr_key2, _param.FakeRateMethod, _param.FakeRateParam, leps[1]->fEta(), leps[1]->Pt(),leps[1]->LeptonFakeTagger(),FRStatSystLep2);

    double this_pr1 =  1;//fakeEst->GetPromptRate(_param.ApplyPR, IsMuon1, ID1, pr_key, leps[0]->fEta(), leps[0]->UncorrectedPt());
    double this_pr2 =  1;//fakeEst->GetPromptRate(_param.ApplyPR, IsMuon2, ID2, pr_key, leps[1]->fEta(), leps[1]->UncorrectedPt());

    if (HasFlag("PromptR1")){
      this_pr1 =  0.95;
      this_pr2 = 0.95;
    }
    if (HasFlag("PromptR2")){
      if(leps[0]->Pt() > 40) {
	if(fabs(leps[0]->Eta()) < 1.5)   this_pr1 =  0.99;
	else   this_pr1 =  0.98;
      }
      else  if(leps[0]->Pt() > 30) {
	if(fabs(leps[0]->Eta()) < 1.5)   this_pr1 =  0.98;
        else   this_pr1 =  0.965;
      }
      else  if(leps[0]->Pt() > 20) {
	if(fabs(leps[0]->Eta()) < 1.5)   this_pr1 =  0.95;
        else   this_pr1 =  0.93;
      }
      
      else {
	if(fabs(leps[0]->Eta()) < 1.5)   this_pr1 =  0.87;
	else   this_pr1 =  0.82;
      }
    }

    
    if(this_fr1 == 1){
      cout << _param.Electron_Tight_ID << " KEY  = "  << fr_key1 << " _param.FakeRateParam = " << _param.FakeRateParam << " _param.FakeRateMethod = " << _param.FakeRateMethod << endl;
      cout << "this_fr1 = " << this_fr1 << " this_fr2 = " << this_fr2 << " this_pr1 = " << this_pr1 << " this_pr2 = " << this_pr2 << endl;
    }

    this_weight = fakeEst->CalculateDilepWeight(this_pr1,this_fr1, this_pr2, this_fr2, leps[0]->PassLepID(),leps[1]->PassLepID(),0);
    

    if(isnan(this_weight)) {
      cout << "Dilepton Fake rate is NaN"<< endl;
      cout << "IsMuon1 = " << IsMuon1 << " fr_key = " << fr_key1 << " ID1 = " << ID1<< " this_fr1 = " << this_fr1 << " pt = " << leps[0]->Pt() << " eta = " << leps[0]->fEta() << endl;
      cout << "IsMuon2 = " << IsMuon2 << " fr_key = " << fr_key2 << " ID2 = " << ID2<< " this_fr2 = " << this_fr2 << " pt = " << leps[1]->Pt() << " eta = " << leps[1]->fEta() << endl;
      exit(EXIT_FAILURE);
    }

    if(run_Debug) cout << ID1 << " " << ID2 << "fr_key11 = " << fr_key1 <<  " w= " << this_weight << " 2L"<<endl;                                                    
    return this_weight;
  }


  else{


    double this_weight = -1.;
    vector<double> FRs;

    double this_fr = -999.;
    for(auto lep : leps){

      if( lep->PassLepID() ) continue;

      TString fr_key = (lep->LeptonFlavour() == Lepton::ELECTRON) ?  _param.k.Electron_FR : _param.k.Muon_FR;
      TString ID     = (lep->LeptonFlavour() == Lepton::ELECTRON) ?  _param.Electron_Tight_ID : _param.Muon_Tight_ID;
      bool IsMuon    = (lep->LeptonFlavour() != Lepton::ELECTRON);

      if(_param.syst_ == AnalyzerParameter::FRAJUp) fr_key = fr_key.ReplaceAll("40","30");
      if(_param.syst_ == AnalyzerParameter::FRAJDown) fr_key = fr_key.ReplaceAll("40","60");
      
      int FRStatSyst = 0;
      if(IsMuon){
	if(_param.syst_ == AnalyzerParameter::FRMuonUp) FRStatSyst=1;
	if(_param.syst_ == AnalyzerParameter::FRMuonDown) FRStatSyst=-1;
	if(_param.syst_ == AnalyzerParameter::FRMuonRateUp)FRStatSyst=2;
	if(_param.syst_ == AnalyzerParameter::FRMuonRateDown) FRStatSyst=-2;
	if(_param.syst_ ==AnalyzerParameter::FRMuonHighPtUp)      FRStatSyst=10;
	if(_param.syst_ ==AnalyzerParameter::FRMuonHighPtDown)    FRStatSyst=-10;
      }
      else{
	if(_param.syst_ == AnalyzerParameter::FRElectronUp) FRStatSyst=1;
	if(_param.syst_ == AnalyzerParameter::FRElectronDown) FRStatSyst=-1;
	if(_param.syst_ == AnalyzerParameter::FRElectronRateUp)FRStatSyst=2;
	if(_param.syst_ == AnalyzerParameter::FRElectronRateDown) FRStatSyst=-2;
	if(_param.syst_ ==AnalyzerParameter::FRElectronHighPtUp)      FRStatSyst=10;
	if(_param.syst_ ==AnalyzerParameter::FRElectronHighPtDown)    FRStatSyst=-10;
      }
      
      
      this_fr = fakeEst->GetFakeRate(IsMuon, ID,  fr_key, _param.FakeRateMethod, _param.FakeRateParam, lep->fEta(), lep->Pt(),lep->LeptonFakeTagger(),FRStatSyst);
      this_weight *= -1.*this_fr/(1.-this_fr);
      FRs.push_back(this_fr);

    }

    if(FRs.size()==0){
      return 0.;
    }
    else{
      //cout << this_weight<< " ML"<<endl;                                                                                                                                                                                                                                                                                                                                                                                               

      return this_weight;
    }
  }

  return -999.;
}


double HNL_LeptonCore::GetIsolationCutFromID(Lepton lep, TString id){

  double pt = lep.Pt();
  double eta = lep.Eta();

  if (lep.IsMuon()) {

    if (id == "Peking")        return 0.15;
    if (id == "TopHNT")        return 0.1;
    if (id == "TopHN")         return 0.1;
    if (id == "HNTight_17028") return 0.07;
    if (id == "HNTightV1")     return 0.07;
    if (id == "HNTightV2")     return 0.07;
    if (id == "POGTightPFIsoVeryVeryTight") return 0.05;
    if (id.Contains("TightPFIsoVeryVeryTight")) return 0.05;
    if (id.Contains("TightPFIsoVeryTight")) return 0.1;
    if (id.Contains("TightPFIsoTight")) return 0.15;
    if (id.Contains("TightWithTightIso")) return 0.15;
    if (id.Contains("TightStandardPFIsoTight")) return 0.15;
    if (id.Contains("PFIsoMedium")) return 0.2;
    if (id.Contains("PFIsoLoose")) return 0.25;
    if (id.Contains("PFIsoVeto")) return 0.4;
    if (id == "POGHighPtWithLooseTrkIso") return 0.1;
    if (id == "POGHighPtWithVLooseTrkIso") return 0.45;
    if (id == "POGHighPtTight") return 0.1;
    if (id == "POGHighPtMixTight") return 0.1;
    if (id.Contains("HNMVA_")) return 0.1;
  }
  else {

    if (id == "Peking_2016")      {
      if(fabs(eta) < 1.479) return (0.0287 + (0.506/pt));
      else  return (0.0445 + (0.963/pt));
    }
    if (id == "Peking_2017" || id == "Peking_2018")      {
      if(fabs(eta) < 1.479) return 0.0571;
      else  return 0.05880;
    }

    if( id.Contains("passHEEPID" ))         return 5;
    if( id == "CutBasedLooseNoIso" ) return 0.6;

    if( id == "TopHNSST" ) return 0.1;
    if( id == "HNTight_17028") return 0.08;
    if( id.Contains("HNTightV")) {
      if(fabs(eta) < 1.479) return (0.0287 + (0.506/pt));
      else  return (0.0445 + (0.963/pt));
    }
    if( id == "HN2016") {
      if(fabs(eta) < 1.479) return 0.1;
      else  return (0.06);
    }
    if( id == "HN2017") {
      if(fabs(eta) < 1.479) return 0.085;
      else  return (0.05);
    }
    if( id == "HN2018") {
      if(fabs(eta) < 1.479) return 0.095;
      else  return (0.07);
    }
    if( id == "HNRelaxedIP2016") {
      if(fabs(eta) < 1.479) return 0.1;
      else  return (0.05);
    }
    if( id == "HNRelaxedIP2017") {
      if(fabs(eta) < 1.479) return 0.1;
      else  return (0.05);
    }
    if( id == "HNRelaxedIP2018") {
      if(fabs(eta) < 1.479) return 0.095;
      else  return (0.07);
    }
    if( id == "passTightID_nocc") {
      if(fabs(eta) < 1.479) return (0.0287 + (0.506/pt));
      else  return (0.0445 + (0.963/pt));
    }
    if( id.Contains("passPOGTight")){
      if(fabs(eta) < 1.479) return (0.0287 + (0.506/pt));
      else  return (0.0445 + (0.963/pt));

    }
    if( id.Contains("passPOGMedium")){
      if(fabs(eta) < 1.479) return (0.0478 + (0.506/pt));
      else  return (0.0658 + (0.963/pt));
    }
    if( id == "passTightID") {
      if(fabs(eta) < 1.479) return (0.0287 + (0.506/pt));
      else  return (0.0445 + (0.963/pt));
    }
    if( id.Contains("HNMediumV")) {
      if(fabs(eta) < 1.479) return (0.0478 + (0.506/pt));
      else  return (0.0658 + (0.963/pt));
    }
    if( id == "passMediumID") {
      if(fabs(eta) < 1.479) return (0.0478 + (0.506/pt));
      else  return (0.0658 + (0.963/pt));
    }
    if( id == "HN2016POG") {
      if(fabs(eta) < 1.479) return (0.0287 + (0.506/pt));
      else  return (0.0445 + (0.963/pt));
    }

    if( id == "Iso1") {
      if(fabs(eta) < 1.479) return  0.08;
      else  return 0.08;
    }
    if( id == "Iso2") {
      if(fabs(eta) < 1.479) return  0.09;
      else  return 0.08;
    }
    if( id == "Iso3") {
      if(fabs(eta) < 1.479) return  0.1;
      else  return 0.08;
    }
    if( id == "Iso4") {
      if(fabs(eta) < 1.479) return  0.12;
      else  return 0.08;
    }
    if( id == "Iso5") {
      if(fabs(eta) < 1.479) return  0.09;
      else  return 0.09;
    }
    if( id == "Iso6") {
      if(fabs(eta) < 1.479) return  0.1;
      else  return 0.1;
    }
    if( id == "Iso7") {
      if(fabs(eta) < 1.479) return  0.12;
      else  return 0.12;
    }

    if( id.Contains("HNTight_Opt")) return 0.08;

    if( id.Contains("HN2016MVA")) return 0.08;
    if( id.Contains("HN2016POG")) return 0.08;
    if( id == "passMVAID_noIso_WP90V16") return 0.05;
    if( id == "passMVAID_noIso_WP80") return 0.08;
    if( id == "passMVAID_noIso_WP90") return 0.08;
    if( id == "passMVAID_Iso_WP80") return 999.0;
    if( id == "passMVAID_Iso_WP90") return 999.0;

    if (id.Contains("HNMVA_")) return 0.1;


  }
  cout << "[AnalyzerCore::GetIsoFromID ] ID not found.." << id<< endl;
  exit(EXIT_FAILURE);

  return -999999999.;

}



void HNL_LeptonCore::FillFakeHistograms(AnalyzerParameter& param, TString plot_dir,std::vector<Lepton *>& Leptons, std::vector<Jet>& jets,   std::vector<FatJet>& fatjets, vector<Jet>& BJetColl, Particle& met,  double w){

  if(!DrawSyst(param)) return;

  for(auto ilep : Leptons){

    if(!IsData && !ilep->IsFake()) continue;

    TString LorT = (ilep->PassLepID()) ? "Tight" : "LooseNotTight";
    TString MotherJetFlavour = (IsData) ? "Data" :  ilep->MotherJetFlavour();

    vector<TString> PassLabels;
    PassLabels.push_back(plot_dir+"/"+ilep->GetFlavour());
    if(ilep->Pt() < 15)           PassLabels.push_back(plot_dir+"/Pt0to15_"+ilep->GetFlavour());
    else if(ilep->Pt() < 25)      PassLabels.push_back(plot_dir+"/Pt15to25_"+ilep->GetFlavour());
    else     PassLabels.push_back(plot_dir+"/Pt25toInf_"+ilep->GetFlavour());

    if(ilep->CloseJet_BScore()  <  0.02)  PassLabels.push_back(plot_dir+"/BScore0to0p02_"+ilep->GetFlavour());
    else if(ilep->CloseJet_BScore()  <  0.05)  PassLabels.push_back(plot_dir+"/BScore0p02to0p05_"+ilep->GetFlavour());
    else if(ilep->CloseJet_BScore()  <  0.2)  PassLabels.push_back(plot_dir+"/BScore0p05to0p2_"+ilep->GetFlavour());
    else  PassLabels.push_back(plot_dir+"/BScore0p2toInf_"+ilep->GetFlavour());

    if(ilep->CloseJet_BScore()  <  0.9 && ilep->CloseJet_CvsBScore() > 0.1 && ilep->CloseJet_CvsLScore() < 0.6)  PassLabels.push_back(plot_dir+"/DeepJetCuts_"+ilep->GetFlavour());


    for(auto ilab : PassLabels){

      if(ilep->GetFlavour() == "Electron"){
        FillHist((ilab+"_HF_MVA_"+MotherJetFlavour+"_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_Fake("HF") ,  w, 200, -1, 1 );
        FillHist((ilab+"_HNL_Fake_MVA_"+MotherJetFlavour+"_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_Fake("HNL") ,  w, 200, -1, 1 );
        FillHist((ilab+"_HNL_CF_MVA_"  +MotherJetFlavour+"_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_CF("EDv5") ,  w, 200, -1, 1 );
        FillHist((ilab+"_HNL_Conv_MVA_"+MotherJetFlavour+"_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_Conv("EDv5") ,  w, 200, -1, 1 );
      }


      FillHist((ilab+"_LF_MVA_"+MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_Fake("LF") ,  w, 200, -1, 1 );
      FillHist((ilab+"_ISO_"+   MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->RelIso() ,  w, 60, 0, 0.6 );
      FillHist((ilab+"_QCD_LFvsHF_"+MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->HNL_MVA_Fake("QCD_LFvsHF_v5") ,  w, 200,-1, 1);
      FillHist((ilab+"_QCD_BvsC_"+  MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->HNL_MVA_Fake("QCD_BvsC_v5") ,  w, 200, -1, 1 );
      FillHist((ilab+"_BScore_"+    MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->CloseJet_BScore() ,  w, 200, -1, 1 );
      FillHist((ilab+"_CvsB_"+      MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->CloseJet_CvsBScore() ,  w, 200, -1, 1 );
      FillHist((ilab+"_CvsL_"+      MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->CloseJet_CvsLScore() ,  w, 200, -1, 1);
      FillHist((ilab+"_PtRatio_"+   MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->CloseJet_Ptratio() ,   w, 200, 0, 5 );
      FillHist((ilab+"_PtRel_"+     MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->CloseJet_Ptrel() ,     w, 50, 0, 200  );
      FillHist((ilab+"_Pt_"+   MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->PtMaxed(200.) ,  w, 100, 0, 200);

      FillHist((ilab+"_LF_MVA_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_Fake("LF") ,  w, 200, -1, 1 );
      FillHist((ilab+"_ISO_"+   param.Name +"_"+LorT).Data(), ilep->RelIso() ,  w, 60, 0, 0.6 );
      FillHist((ilab+"_QCD_LFvsHF_"+param.Name +"_"+LorT).Data(), ilep->HNL_MVA_Fake("QCD_LFvsHF_v5") ,  w, 200,-1, 1);
      FillHist((ilab+"_QCD_BvsC_"+  param.Name +"_"+LorT).Data(), ilep->HNL_MVA_Fake("QCD_BvsC_v5") ,  w, 200, -1, 1 );
      FillHist((ilab+"_BScore_"+    param.Name +"_"+LorT).Data(), ilep->CloseJet_BScore() ,  w, 200, -1, 1 );
      FillHist((ilab+"_CvsB_"+      param.Name +"_"+LorT).Data(), ilep->CloseJet_CvsBScore() ,  w, 200, -1, 1 );
      FillHist((ilab+"_CvsL_"+      param.Name +"_"+LorT).Data(), ilep->CloseJet_CvsLScore() ,  w, 200, -1, 1);
      FillHist((ilab+"_PtRatio_"+   param.Name +"_"+LorT).Data(), ilep->CloseJet_Ptratio() ,   w, 200, 0, 5 );
      FillHist((ilab+"_PtRel_"+     param.Name +"_"+LorT).Data(), ilep->CloseJet_Ptrel() ,     w, 50, 0, 200  );
      FillHist((ilab+"_Pt_"+   param.Name +"_"+LorT).Data(), ilep->PtMaxed(200.) ,  w, 100, 0, 200);

    }
  }

  return;
}
