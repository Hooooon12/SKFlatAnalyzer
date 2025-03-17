#include "HNL_LeptonCore.h"



std::vector<Muon> HNL_LeptonCore::SelectMuons(const std::vector<Muon>& muons, const TString& id, double ptmin, double fetamax) {
  std::vector<Muon> out;

  for (const auto& muon : muons) {
    if (muon.Pt() <= ptmin || std::abs(muon.Eta()) >= fetamax || !muon.PassID(id)) {
      continue;
    }
    out.push_back(muon);
  }

  std::sort(out.begin(), out.end(), PtComparing);
  return out;
}


std::vector<Muon> HNL_LeptonCore::SelectMuons(const TString& id, double ptmin, double fetamax) {
  std::vector<Muon> out;

  for (const auto& muon : All_Muons) {
    if (muon.Pt() <= ptmin || std::abs(muon.Eta()) >= fetamax || !muon.PassID(id)) {
      continue;
    }
    out.push_back(muon);
  }

  std::sort(out.begin(), out.end(), PtComparing);
  return out;
}


std::vector<Muon> HNL_LeptonCore::SelectMuons(AnalyzerParameter& param, const TString& id, double ptmin, double fetamax, double& EvWeight) {
  std::vector<Muon> muons = SelectMuons(param, id, ptmin, fetamax);
    
  if (!IsData) {
    EvalMuonIDWeight(muons, param, EvWeight);
  }

  return muons;
}

std::vector<Muon> HNL_LeptonCore::SelectMuons(AnalyzerParameter& param, const TString& id, double ptmin, double fetamax) {

  std::vector<Muon> muons = All_Muons;

  // Apply high pt smearing and scale systematics
  HighPtMuonCorr(muons);

  if (param.syst_ == AnalyzerParameter::MuonEnUp) {
    muons = ScaleMuons(muons, +1);
  } else if (param.syst_ == AnalyzerParameter::MuonEnDown) {
    muons = ScaleMuons(muons, -1);
  }

  // Apply resolution systematics to high-pt muons
  if (param.syst_ == AnalyzerParameter::MuonResUp) {
    SmearHighPtMuonSyst(muons);
  }

  std::vector<Muon> out;
  for (auto& muon : muons) {
    if (muon.Pt() <= ptmin || std::abs(muon.Eta()) >= fetamax || !muon.PassID(id)) {
      continue;
    }

    // Remove high-pt EC muons if flag is set
    if (HasFlag("Remove_HighPtEC") && std::abs(muon.Eta()) > 1.5 && muon.Pt() > 500) {
      continue;
    }

    bool CorrPt = (RunFake || RunPromptTLRemoval) && (id == param.Muon_FR_ID || id == param.Muon_Veto_ID);
    if (RunFake && HasFlag("OS")) CorrPt=false;
    if (CorrPt) {
      Muon this_muon = muon;
      double IsolationRequirement = param.FakeRateParam.Contains("Cone") ? GetIsolationCutFromID(Lepton(muon), param.Muon_Tight_ID) : 1.0;

      if (param.FakeRateParam == "PtCone") {
	this_muon.SetPtEtaPhiM(muon.CalcPtCone(muon.RelIso(), IsolationRequirement), muon.Eta(), muon.Phi(), muon.M());
      } else if (param.FakeRateParam == "TrkPtCone") {
	this_muon.SetPtEtaPhiM(muon.CalcTrkPtCone(muon.RelIso(), IsolationRequirement), muon.Eta(), muon.Phi(), muon.M());
      } else if (param.FakeRateParam == "PtConeMini") {
	this_muon.SetPtEtaPhiM(muon.CalcPtCone(muon.MiniRelIso(), IsolationRequirement), muon.Eta(), muon.Phi(), muon.M());
      } else if (param.FakeRateParam == "PtCorr") {
	this_muon.SetPtEtaPhiM(muon.CalcMVACone(muon.MVAFakeCut(param.Muon_Tight_ID, GetYearString())), muon.Eta(), muon.Phi(), muon.M());
      } else if (param.FakeRateParam == "PtParton") {
	this_muon.SetPtEtaPhiM(muon.PtParton(GetPtPartonSF(Lepton(muon), param.Muon_FR_ID, param), muon.MVAFakeCut(param.Muon_Tight_ID, GetYearString())),
			       muon.Eta(), muon.Phi(), muon.M());
      } 

      out.push_back(this_muon);
    } else {
      out.push_back(muon);
    }
  }

  std::sort(out.begin(), out.end(), PtComparing);
  return out;
}


std::vector<Electron> HNL_LeptonCore::SelectElectrons(const std::vector<Electron>& electrons, const TString& id, 
                                                      double ptmin, double fetamax, bool vetoHEM) {
  std::vector<Electron> out;

  for (const auto& electron : electrons) {
    if (electron.Pt() <= ptmin || std::abs(electron.scEta()) >= fetamax || !electron.PassID(id)) {
      continue;
    }
    if (vetoHEM && FindHEMElectron(electron)) {
      continue;
    }
    out.push_back(electron);
  }

  std::sort(out.begin(), out.end(), PtComparing);
  return out;
}

std::vector<Electron> HNL_LeptonCore::SelectElectrons(const TString& id, double ptmin, double fetamax, bool vetoHEM) {

  std::vector<Electron> out;

  for (const auto& electron : All_Electrons) {
    if (electron.Pt() <= ptmin || std::abs(electron.scEta()) >= fetamax || !electron.PassID(id)) {
      continue;
    }
    if (vetoHEM && FindHEMElectron(electron)) {
      continue;
    }
    out.push_back(electron);
  }

  std::sort(out.begin(), out.end(), PtComparing);
  return out;
}


std::vector<Electron> HNL_LeptonCore::SelectElectrons(AnalyzerParameter& param, const TString& id, 
                                                      double ptmin, double fetamax, double& EvWeight, bool vetoHEM) {
  std::vector<Electron> electrons = SelectElectrons(param, id, ptmin, fetamax, vetoHEM);
    
  if (!IsData) {
    EvalElectronIDWeight(electrons, param, EvWeight);
  }

  return electrons;
}


std::vector<Electron> HNL_LeptonCore::SelectElectrons(AnalyzerParameter& param, const TString& id, 
                                                      double ptmin, double fetamax, bool vetoHEM) {
  std::vector<Electron> electrons = All_Electrons;

  // Apply smearing or scaling based on systematic variations
  if (param.syst_ == AnalyzerParameter::ElectronResUp) {
    electrons = SmearElectrons(electrons, +1);
  } else if (param.syst_ == AnalyzerParameter::ElectronResDown) {
    electrons = SmearElectrons(electrons, -1);
  } else if (param.syst_ == AnalyzerParameter::ElectronEnUp) {
    electrons = ScaleElectrons(electrons, +1);
  } else if (param.syst_ == AnalyzerParameter::ElectronEnDown) {
    electrons = ScaleElectrons(electrons, -1);
  }

  std::vector<Electron> out;

  for (auto& electron : electrons) {
    if (electron.Pt() <= ptmin || std::abs(electron.scEta()) >= fetamax || !electron.PassID(id)) {
      continue;
    }

    // Apply HEM veto if required
    if (vetoHEM && FindHEMElectron(electron)) {
      continue;
    }

    // Remove high-pt electrons in the EC region if flag is set
    if (HasFlag("Remove_HighPtEC") && std::abs(electron.Eta()) > 1.5 && electron.Pt() > 500) {
      continue;
    }

    bool CorrPt = (RunFake || RunPromptTLRemoval) && 
      (id == param.Electron_FR_ID || id == param.Electron_Veto_ID);

    if (RunFake&& HasFlag("OS")) CorrPt=false;

    if (CorrPt) {
      Electron this_electron = electron;
      double IsolationRequirement = param.FakeRateParam.Contains("Cone") ?  GetIsolationCutFromID(Lepton(electron), param.Electron_Tight_ID) : 1.;

      if (param.FakeRateParam == "PtCone") {
	this_electron.SetPtEtaPhiM(electron.CalcPtCone(electron.RelIso(), IsolationRequirement), 
				   electron.Eta(), electron.Phi(), electron.M());
      } else if (param.FakeRateParam == "TrkPtCone") {
	this_electron.SetPtEtaPhiM(electron.CalcTrkPtCone(electron.RelIso(), IsolationRequirement), 
				   electron.Eta(), electron.Phi(), electron.M());
      } else if (param.FakeRateParam == "PtConeMini") {
	this_electron.SetPtEtaPhiM(electron.CalcPtCone(electron.MiniRelIso(), IsolationRequirement), 
				   electron.Eta(), electron.Phi(), electron.M());
      } else if (param.FakeRateParam == "PtCorr") {
	this_electron.SetPtEtaPhiM(electron.CalcMVACone(electron.MVAFakeCut(param.Electron_Tight_ID, GetYearString())), 
				   electron.Eta(), electron.Phi(), electron.M());
      } else if (param.FakeRateParam == "PtParton") {
	this_electron.SetPtEtaPhiM(electron.PtParton(GetPtPartonSF(Lepton(electron), param.Electron_FR_ID, param), 
						     electron.MVAFakeCut(param.Electron_Tight_ID, GetYearString())), 
				   electron.Eta(), electron.Phi(), electron.M());
      } 

      out.push_back(this_electron);
    } else {
      out.push_back(electron);
    }
  }

  std::sort(out.begin(), out.end(), PtComparing);
  return out;
}



std::vector<Tau> HNL_LeptonCore::SelectTaus(const std::vector<Tau>& taus, const TString& id, double ptmin, double fetamax) {
  std::vector<Tau> out;

  for (const auto& tau : taus) {
    if (tau.Pt() <= ptmin || std::abs(tau.Eta()) >= fetamax || !tau.PassID(id)) {
      continue;
    }
    out.push_back(tau);
  }

  std::sort(out.begin(), out.end(), PtComparing);
  return out;
}


std::vector<Tau> HNL_LeptonCore::SelectTaus(std::vector<Lepton*>& leps, const TString& id, double ptmin, double fetamax) {
  std::vector<Tau> Taus = SelectTaus(id, ptmin, fetamax);
  std::vector<Tau> out;

  for (auto& tau : Taus) {
    bool pass = true;
    for (auto* lep : leps) {
      if (lep->DeltaR(tau) <= 0.4) {
	pass = false;
	break;
      }
    }

    if (pass) {
      out.push_back(tau);
    }
  }

  std::sort(out.begin(), out.end(), PtComparing);
  return out;
}


std::vector<Tau> HNL_LeptonCore::SelectTaus(const TString& id, double ptmin, double fetamax) {
  std::vector<Tau> taus = GetAllTaus();
  std::vector<Tau> out;

  for (const auto& tau : taus) {
    if (tau.Pt() <= ptmin || std::abs(tau.Eta()) >= fetamax || !tau.PassID(id)) {
      continue;
    }
    out.push_back(tau);
  }

  std::sort(out.begin(), out.end(), PtComparing);
  return out;
}


vector<Muon> HNL_LeptonCore::SkimLepColl(const vector<Muon>& MuColl,  AnalyzerParameter& param, const TString& Option){

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


vector<Electron> HNL_LeptonCore::SkimLepColl(const vector<Electron>& ElColl, AnalyzerParameter& param,const TString& Option){

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


vector<Electron> HNL_LeptonCore::SkimLepColl(const vector<Electron>& ElColl, const TString& Option){

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


vector<Muon> HNL_LeptonCore::SkimLepColl(const vector<Muon>& MuColl,  const TString& Option){

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
