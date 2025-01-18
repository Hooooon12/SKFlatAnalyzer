#include "HNL_LeptonCore.h"

std::vector<Muon> HNL_LeptonCore::SelectMuons(const std::vector<Muon>& muons, TString id, double ptmin, double fetamax){

  std::vector<Muon> out;
  for(unsigned int i=0; i<muons.size(); i++){
    if(!( muons.at(i).Pt()>ptmin ))           continue;
    if(!( fabs(muons.at(i).Eta())<fetamax ))  continue;
    if(!( muons.at(i).PassID(id) ))           continue;
    out.push_back( muons.at(i) );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;
}

std::vector<Muon> HNL_LeptonCore::SelectMuons(TString id, double ptmin, double fetamax){

  std::vector<Muon> muons = All_Muons;

  std::vector<Muon> out;
  for(unsigned int i=0; i<muons.size(); i++){
    if(!( muons.at(i).Pt()>ptmin ))             continue;
    if(!( fabs(muons.at(i).Eta())<fetamax ))  continue;
    if(!( muons.at(i).PassID(id) ))             continue;
    out.push_back( muons.at(i) );
  }
  std::sort(out.begin(),       out.end(),        PtComparing);
  return out;
}


std::vector<Muon> HNL_LeptonCore::SelectMuons(AnalyzerParameter& param, TString id, double ptmin, double fetamax, double& EvWeight){
  
  std::vector<Muon> muons = SelectMuons(param, id, ptmin, fetamax);
  if(!IsData) EvalMuonIDWeight(muons  , param, EvWeight);
  return muons;
}

std::vector<Muon> HNL_LeptonCore::SelectMuons(AnalyzerParameter& param, TString id, double ptmin, double fetamax){

  std::vector<Muon> this_AllMuons =  All_Muons;
  std::vector<Muon> muons ;

  //// smeares high pt and sets scale syst

  HighPtMuonCorr(this_AllMuons);


  if(param.syst_ == AnalyzerParameter::MuonEnUp)         muons = ScaleMuons( this_AllMuons, +1 );
  else if(param.syst_ == AnalyzerParameter::MuonEnDown)  muons = ScaleMuons( this_AllMuons, -1 );
  else muons = this_AllMuons;


  //// High Pt have resolution syst 
  if(param.syst_ == AnalyzerParameter::MuonResUp)    SmearHighPtMuonSyst(muons);


  std::vector<Muon> out;
  for(unsigned int i=0; i<muons.size(); i++){
    if(!( muons.at(i).Pt()> ptmin ))          continue;
    if(!( fabs(muons.at(i).Eta())< fetamax )) continue;
    if(!( muons.at(i).PassID(id) ))           continue;
    
    bool CorrPt = (RunFake||RunPromptTLRemoval) && (id == param.Muon_FR_ID || id == param.Muon_Veto_ID);

    if(CorrPt){

      Muon this_muon = muons.at(i);
      if(param.FakeRateParam == "PtCone"){
        double Isocut = GetIsoFromID(Lepton(muons[i]), param.Muon_Tight_ID );
        this_muon.SetPtEtaPhiM( muons.at(i).CalcPtCone(muons.at(i).RelIso(), Isocut), muons.at(i).Eta(), muons.at(i).Phi(), muons.at(i).M() );
      }
      if(param.FakeRateParam == "TrkPtCone"){
        double Isocut = GetIsoFromID(Lepton(muons[i]), param.Muon_Tight_ID );
	this_muon.SetPtEtaPhiM( muons.at(i).CalcTrkPtCone(muons.at(i).RelIso(), Isocut), muons.at(i).Eta(), muons.at(i).Phi(), muons.at(i).M() );
      }
      if(param.FakeRateParam == "PtConeMini"){
        double Isocut  = GetIsoFromID(Lepton(muons[i]), param.Muon_Tight_ID);
        this_muon.SetPtEtaPhiM( muons.at(i).CalcPtCone(muons.at(i).MiniRelIso(), Isocut), muons.at(i).Eta(), muons.at(i).Phi(), muons.at(i).M() );
      }
      if(param.FakeRateParam == "PtCorr"){
        this_muon.SetPtEtaPhiM( muons.at(i).CalcMVACone( muons.at(i).MVAFakeCut(param.Muon_Tight_ID,GetYearString())) , muons.at(i).Eta(), muons.at(i).Phi(), muons.at(i).M() );
      }
      if(param.FakeRateParam == "PtParton"){
        this_muon.SetPtEtaPhiM(muons.at(i).PtParton(GetPtPartonSF(Lepton(muons.at(i)), param.Muon_FR_ID, param), muons.at(i).MVAFakeCut(param.Muon_Tight_ID,GetYearString())), muons.at(i).Eta(), muons.at(i).Phi(), muons.at(i).M() );
      }
      if(param.FakeRateParam == "MotherJetPt"){
        this_muon.SetPtEtaPhiM( muons.at(i).MotherJetPt(),muons.at(i).Eta(), muons.at(i).Phi(), muons.at(i).M() );
      } 
      out.push_back( this_muon);
      
    }
    else   out.push_back( muons.at(i) );
  }
  
  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;
}


std::vector<Electron> HNL_LeptonCore::SelectElectrons(const std::vector<Electron>& electrons, TString id, double ptmin, double fetamax, bool vetoHEM){

  std::vector<Electron> out;
  for(unsigned int i=0; i<electrons.size(); i++){
    if(!( electrons.at(i).Pt()>ptmin ))             continue;
    if(!( fabs(electrons.at(i).scEta())<fetamax ))  continue;
    if(!( electrons.at(i).PassID(id) ))             continue;
    if(vetoHEM){
      if ( FindHEMElectron (electrons.at(i)) ){
        continue;
      }
    }
    out.push_back(electrons.at(i));
  }
  std::sort(out.begin(),       out.end(),        PtComparing);
  return out;
}


std::vector<Electron> HNL_LeptonCore::SelectElectrons(TString id, double ptmin, double fetamax, bool vetoHEM){

  std::vector<Electron> electrons = All_Electrons;

  std::vector<Electron> out;
  for(unsigned int i=0; i<electrons.size(); i++){
    if(!( electrons.at(i).Pt()>ptmin ))             continue;
    if(!( fabs(electrons.at(i).scEta())<fetamax ))  continue;
    if(!( electrons.at(i).PassID(id) ))             continue;
    if(vetoHEM){
      if ( FindHEMElectron (electrons.at(i)) ){
        continue;
      }
    }
    out.push_back( electrons.at(i) );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}

std::vector<Electron> HNL_LeptonCore::SelectElectrons(AnalyzerParameter& param, TString id, double ptmin, double fetamax, double& EvWeight,bool vetoHEM){

  std::vector<Electron> electrons = SelectElectrons(param, id, ptmin, fetamax, vetoHEM);
  if(!IsData) EvalElectronIDWeight(electrons, param, EvWeight);
  return electrons;
}

std::vector<Electron> HNL_LeptonCore::SelectElectrons(AnalyzerParameter& param, TString id, double ptmin, double fetamax, bool vetoHEM){

  std::vector<Electron> this_AllElectrons = All_Electrons;
  std::vector<Electron> electrons ;

  if(param.syst_ == AnalyzerParameter::ElectronResUp)        electrons = SmearElectrons( this_AllElectrons, +1 );
  else if(param.syst_ == AnalyzerParameter::ElectronResDown) electrons = SmearElectrons( this_AllElectrons, -1 );
  else if(param.syst_ == AnalyzerParameter::ElectronEnUp)    electrons = ScaleElectrons( this_AllElectrons, +1 );
  else if(param.syst_ == AnalyzerParameter::ElectronEnDown)  electrons = ScaleElectrons( this_AllElectrons, -1 );
  else electrons = this_AllElectrons;

  //  double ElEnergyShift=1;
  // if(RunCF ) ElEnergyShift = GetZMassShift(electrons);

  std::vector<Electron> out;
  for(unsigned int i=0; i<electrons.size(); i++){

    if(!( electrons.at(i).Pt()> ptmin ))            continue;
    if(!( fabs(electrons.at(i).scEta())< fetamax )) continue;
    if(!( electrons.at(i).PassID(id) ))             continue;
    if(vetoHEM){
      if ( FindHEMElectron (electrons.at(i)) ){
        continue;
      }
    }
    
    bool CorrPt = (RunFake||RunPromptTLRemoval) && (id == param.Electron_FR_ID || id == param.Electron_Veto_ID);

    if(CorrPt){
      Electron this_electron = electrons.at(i);
      if(param.FakeRateParam == "PtCone"){
        double Isocut = GetIsoFromID(Lepton(electrons.at(i)),param.Electron_Tight_ID);
        this_electron.SetPtEtaPhiM( electrons.at(i).CalcPtCone(electrons.at(i).RelIso(), Isocut), electrons.at(i).Eta(), electrons.at(i).Phi(), electrons.at(i).M() );
      }
      if(param.FakeRateParam == "TrkPtCone"){
	double Isocut = GetIsoFromID(Lepton(electrons.at(i)),param.Electron_Tight_ID);
        this_electron.SetPtEtaPhiM( electrons.at(i).CalcTrkPtCone(electrons.at(i).RelIso(), Isocut), electrons.at(i).Eta(), electrons.at(i).Phi(), electrons.at(i).M() );
      }
      if(param.FakeRateParam == "PtConeMini"){
        double Isocut = GetIsoFromID(Lepton(electrons.at(i)) ,param.Electron_Tight_ID);
        this_electron.SetPtEtaPhiM( electrons.at(i).CalcPtCone(electrons.at(i).MiniRelIso(), Isocut), electrons.at(i).Eta(), electrons.at(i).Phi(), electrons.at(i).M() );
      }

      if(param.FakeRateParam == "PtCorr"){
        this_electron.SetPtEtaPhiM( electrons.at(i).CalcMVACone( electrons.at(i).MVAFakeCut(param.Electron_Tight_ID,GetYearString())) , electrons.at(i).Eta(), electrons.at(i).Phi(), electrons.at(i).M() );
      }
      if(param.FakeRateParam == "PtParton"){
        this_electron.SetPtEtaPhiM(electrons.at(i).PtParton(GetPtPartonSF(Lepton(electrons.at(i)), param.Electron_FR_ID,param), electrons.at(i).MVAFakeCut(param.Electron_Tight_ID,GetYearString())), electrons.at(i).Eta(), electrons.at(i).Phi(), electrons.at(i).M() );
      }
      if(param.FakeRateParam == "MotherJetPt"){
        this_electron.SetPtEtaPhiM( electrons.at(i).MotherJetPt(),electrons.at(i).Eta(), electrons.at(i).Phi(), electrons.at(i).M() );
      }
      
      out.push_back( this_electron);
    }
    else   out.push_back( electrons.at(i) );
  }
  
  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;
  
}

std::vector<Tau> HNL_LeptonCore::SelectTaus(const std::vector<Tau>& taus, TString id, double ptmin, double fetamax){

  std::vector<Tau> out;
  for(unsigned int i=0; i<taus.size(); i++){
    if(!( taus.at(i).Pt()>ptmin ))            continue;
    if(!( fabs(taus.at(i).Eta())<fetamax ))   continue;
    if(!( taus.at(i).PassID(id) ))            continue;
    out.push_back( taus.at(i) );
  }
  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}

std::vector<Tau> HNL_LeptonCore::SelectTaus(std::vector<Lepton* > leps,TString id, double ptmin, double fetamax){

  std::vector<Tau> Taus = SelectTaus(id, ptmin, fetamax);
  std::vector<Tau> out;
  for(auto tau : Taus){
    for(auto lep : leps){
      if(lep->DeltaR(tau) > 0.4)  out.push_back(tau);
    }
  }
  std::sort(out.begin(),       out.end(),        PtComparing);
  
  return out;
}

std::vector<Tau> HNL_LeptonCore::SelectTaus(TString id, double ptmin, double fetamax){

  std::vector<Tau> taus = GetAllTaus();
  std::vector<Tau> out;

  for(unsigned int i=0; i<taus.size(); i++){
    if(!( taus.at(i).Pt()>ptmin ))           continue;
    if(!( fabs(taus.at(i).Eta())<fetamax ))  continue;
    if(!( taus.at(i).PassID(id) ))           continue;
    out.push_back( taus.at(i) );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}


vector<Muon> HNL_LeptonCore::SkimLepColl(const vector<Muon>& MuColl,  AnalyzerParameter param, TString Option){

  bool GetPrompt=false, GetHadFake=false, GetEWtau=false, GetNHIntConv=false, GetNHExtConv=false;

  if(Option.Contains("Prompt"))          GetPrompt    =true;
  if(Option.Contains("HFake"))           GetHadFake   =true;
  if(Option.Contains("EWtau"))           GetEWtau     =true;
  if(Option.Contains("NHConv"))         {GetNHIntConv =true; GetNHExtConv=true;}
  else{
    if(Option.Contains("NHIntConv")) GetNHIntConv =true;
    if(Option.Contains("NHExtConv")) GetNHExtConv =true;
  }
  if(     Option=="Fake"     )          {GetHadFake   =true; GetNHExtConv=true;}



  vector<Muon> ReturnVec;
  for(unsigned int i=0; i<MuColl.size(); i++){

    if(IsData) ReturnVec.push_back(MuColl.at(i));
    else {
      int LepType= MuColl.at(i).LeptonGenType();
      bool PassSel=false;

      if( GetPrompt    && (LepType==1 || LepType==2) ) PassSel=true;
      if( GetHadFake   && (LepType<0 && LepType>=-4) ) PassSel=true;
      if( GetEWtau     &&         LepType==3         ) PassSel=true;
      if( GetNHIntConv &&         LepType>=4         ) PassSel=true;
      if( GetNHExtConv &&         LepType<-4         ) PassSel=true;
      if( PassSel ) ReturnVec.push_back(MuColl.at(i));
    }
  }

  return ReturnVec;
}


vector<Electron> HNL_LeptonCore::SkimLepColl(const vector<Electron>& ElColl, AnalyzerParameter param,TString Option){

  bool GetPrompt=false, GetHadFake=false, GetEWtau=false, GetNHIntConv=false, GetNHExtConv=false, GetCF=false;
  //CFHFakeNHConv                                                                                                                                                                                                                                                                                  
  if(Option.Contains("Prompt"))          GetPrompt    =true;
  if(Option.Contains("CF"))              GetCF        =true;
  if(Option.Contains("HFake"))           GetHadFake   =true;
  if(Option.Contains("EWtau"))           GetEWtau     =true;
  if(Option.Contains("NHConv"))         {GetNHIntConv =true; GetNHExtConv=true;}
  else{ if(Option.Contains("NHIntConv")) GetNHIntConv =true;
    if(Option.Contains("NHExtConv")) GetNHExtConv =true; }
  if(     Option=="Fake"     )          {GetHadFake   =true; GetNHExtConv=true;}


  vector<Electron> ReturnVec;
  for(unsigned int i=0; i<ElColl.size(); i++){
    if(IsData) ReturnVec.push_back(ElColl.at(i));
    else {
      int LepType= ElColl.at(i).LeptonGenType();
      bool PassSel=false;

      if( GetPrompt    && (LepType==1 || LepType==2) ) PassSel=true;
      if( GetHadFake   && (LepType<0 && LepType>=-4) ) PassSel=true;
      if( GetEWtau     &&         LepType==3         ) PassSel=true;
      if( GetNHIntConv &&         LepType>=4         ) PassSel=true;
      if( GetNHExtConv &&         LepType<-4         ) PassSel=true;
      if( GetCF        &&         ElColl.at(i).LeptonIsCF() ) PassSel=true;
      if( Option.Contains("NoCF") && ElColl.at(i).LeptonIsCF()) PassSel=false;
      if( PassSel ) ReturnVec.push_back(ElColl.at(i));
    }
  }

  return ReturnVec;
}


vector<Electron> HNL_LeptonCore::SkimLepColl(const vector<Electron>& ElColl, TString Option){

  vector<Electron> ReturnColl;

  bool Barrel1=false, Barrel2=false, Endcap=false;
  if(Option.Contains("B1")) Barrel1=true;
  if(Option.Contains("B2")) Barrel2=true;
  if(Option.Contains("E"))  Endcap =true;

  for(unsigned int i=0; i<ElColl.size(); i++){
    bool PassSel=false; double fEta=fabs(ElColl.at(i).Eta());
    if( Barrel1 && fEta <0.8               ) PassSel=true;
    if( Barrel2 && fEta>=0.8 && fEta<1.479 ) PassSel=true;
    if( Endcap  && fEta>=1.479 && fEta<2.5 ) PassSel=true;
    if( PassSel ) ReturnColl.push_back(ElColl.at(i));
  }

  return ReturnColl;
}


vector<Muon> HNL_LeptonCore::SkimLepColl(const vector<Muon>& MuColl,  TString Option){

  vector<Muon> ReturnColl;
  bool Barrel=false, Overlap=false, Endcap=false;
  if(Option.Contains("MB")) Barrel =true;
  if(Option.Contains("MO")) Overlap=true;
  if(Option.Contains("ME")) Endcap =true;

  for(unsigned int i=0; i<MuColl.size(); i++){
    bool PassSel=false; double fEta=fabs(MuColl.at(i).Eta());
    if( Barrel  && fEta <0.9               ) PassSel=true;
    if( Overlap && fEta>=0.9 && fEta<1.6   ) PassSel=true;
    if( Endcap  && fEta>=1.6 && fEta<2.4   ) PassSel=true;
    if( PassSel ) ReturnColl.push_back(MuColl.at(i));
  }

  return ReturnColl;
}


bool  HNL_LeptonCore::ComparePtTune(Muon mu, double ratio){
  
  double pt = mu.Pt();

  Particle this_tunep4 = mu.TuneP4();
  double tunep= this_tunep4.Pt();

  if(this_tunep4.Pt()<200){

    double TunePOverPt = this_tunep4.Pt() / mu.MiniAODPt();
    tunep      = TunePOverPt * mu.Pt(); 
    
  }
  else{
    if(!IsDATA){

      ScaledPts ptvalues;
      ptvalues = muonGE->GeneralizedEndpointPt(GetEraShort(), this_tunep4.Pt(), this_tunep4.Charge(), this_tunep4.Eta(), this_tunep4.Phi()*180./M_PI, event);
      tunep = ptvalues.ScaledPt;
    }
  }
  
  if(fabs( (tunep- pt) / tunep) > ratio){
    cout << "ComparePtTune : tunep" << tunep << " pt=" <<  pt << " event = " << event << " run = " << run <<  endl;
    return true;
  }
  return false;

}
