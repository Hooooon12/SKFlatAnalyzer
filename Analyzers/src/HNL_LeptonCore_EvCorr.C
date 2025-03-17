#include "HNL_LeptonCore.h"

double HNL_LeptonCore::GetPDFUncertainty(const TString& signalRegionBin, int sys){
  if (!MCSample.Contains("Type") || sys == 0) return 1.0;

  // Determine Signal Range
  int SignalRange = 2; // Default
  if (MCSample.Contains("M500_") || MCSample.Contains("M600_") || MCSample.Contains("M700_") ||
      MCSample.Contains("M800_") || MCSample.Contains("M900_")) {
    SignalRange = 0;
  } else if (MCSample.Contains("M1000_") || MCSample.Contains("M1100_") || MCSample.Contains("M1200_") ||
	     MCSample.Contains("M1300_") || MCSample.Contains("M1400_")) {
    SignalRange = 1;
  }

  // Handle DY samples
  if (MCSample.Contains("DY")) {
    if (signalRegionBin.Contains("SR1")) return 1.0 + static_cast<double>(sys) * 0.07;
    if (signalRegionBin.Contains("SR2")) return 1.0 + static_cast<double>(sys) * 0.01;
    if (signalRegionBin.Contains("SR3")) return 1.0 + static_cast<double>(sys) * 0.025;
    return 1.0 + static_cast<double>(sys) * 0.02;
  }

  // Handle VBF samples
  if (MCSample.Contains("VBF")) {
    if (signalRegionBin.Contains("SR2")) return 1.0 + static_cast<double>(sys) * 0.05;
    if (signalRegionBin.Contains("SR3")) return 1.0 + static_cast<double>(sys) * 0.04;

    static const std::map<int, std::map<TString, double>> VBF_Uncertainty = {
      {1, {{"SR1_MNbin1", 0.018}, {"SR1_MNbin2", 0.01}, {"SR1_MNbin3", 0.002},
	   {"SR1_MNbin4", 0.018}, {"SR1_MNbin5", 0.025}, {"SR1_MNbin6", 0.01}}},
      {2, {{"SR1_MNbin1", 0.1}, {"SR1_MNbin2", 0.03}, {"SR1_MNbin3", 0.03},
	   {"SR1_MNbin4", 0.03}, {"SR1_MNbin5", 0.03}, {"SR1_MNbin6", 0.01},
	   {"SR3_MNbin8", 0.06}, {"SR3_MNbin10", 0.06}}},
      {3, {{"SR1_MNbin1", 0.29}, {"SR1_MNbin2", 0.12}, {"SR1_MNbin3", 0.05},
	   {"SR1_MNbin4", 0.05}, {"SR1_MNbin5", 0.05}, {"SR1_MNbin6", 0.01}}}
    };

    if (VBF_Uncertainty.count(SignalRange) && VBF_Uncertainty.at(SignalRange).count(signalRegionBin)) {
      return 1.0 + static_cast<double>(sys) * VBF_Uncertainty.at(SignalRange).at(signalRegionBin);
    }
    return 1.0 + static_cast<double>(sys) * 0.02;
  }

  // Handle SSWW samples
  if (MCSample.Contains("SSWW")) {
    if (signalRegionBin.Contains("SR1")) return 1.0 + static_cast<double>(sys) * 0.02;

    static const std::map<int, std::map<TString, double>> SSWW_Uncertainty = {
      {1, {{"SR2_HTLTbin1", 0.01}, {"SR2_HTLTbin7", 0.07}}},
      {2, {{"SR2_HTLTbin1", 0.01}, {"SR2_HTLTbin7", 0.012}}}
    };

    if (SSWW_Uncertainty.count(SignalRange) && SSWW_Uncertainty.at(SignalRange).count(signalRegionBin)) {
      return 1.0 + static_cast<double>(sys) * SSWW_Uncertainty.at(SignalRange).at(signalRegionBin);
    }

    if (signalRegionBin.Contains("SR2")) return 1.0 + static_cast<double>(sys) * 0.05;
    if (signalRegionBin.Contains("SR3")) return 1.0 + static_cast<double>(sys) * 0.04;
  }

  return 1.0 + static_cast<double>(sys) * 0.02;
}

TString HNL_LeptonCore::GetPDFUncertainty(int weightIndex, double& ev_weight) {
  // Ensure MCSample contains "Type", otherwise return an empty string
  if (!MCSample.Contains("Type")) {
    return "";
  }

  float NormNom = h_SumW_Scale->GetBinContent(1);

  // Check if weightIndex is within the bounds of weight_PDF
  if (static_cast<size_t>(weightIndex) < weight_PDF->size()) {
    float Norm = h_SumW_PDF->GetBinContent(weightIndex + 1);
    ev_weight = weight_PDF->at(weightIndex) * (NormNom / Norm);

    return "_Syst_PDF" + TString::Itoa(weightIndex, 10);
  }

  // Default case when weightIndex is out of range
  ev_weight = 1;
  return "";
}


double HNL_LeptonCore::GetScaleUncertainty(int sys) {
  // Return 1 if the sample type is not valid or if weight_Scale is empty
  if (!MCSample.Contains("Type") || sys == 0 || weight_Scale->empty()) {
    return 1.0;
  }

  // Ensure weight_Scale has enough elements
  if (weight_Scale->size() < 2) {
    return 1.0;
  }

  if (sys == 1) {
    double min_var = 0.0;
    for (size_t weightIndex = 0; weightIndex < weight_Scale->size() - 2; ++weightIndex) {
      min_var = std::max(min_var, static_cast<double>(weight_Scale->at(weightIndex)));
    }
    return min_var;
  }

  if (sys == -1) {
    double max_var = 1000.0;
    for (size_t weightIndex = 0; weightIndex < weight_Scale->size() - 2; ++weightIndex) {
      max_var = std::min(max_var, static_cast<double>(weight_Scale->at(weightIndex)));
    }
    return max_var;
  }

  return 1.0;
}


double HNL_LeptonCore::GetMuonIDWeight(const std::vector<Muon>& muons, AnalyzerParameter& param) {
  double weight = 1.0;
  EvalMuonIDWeight(muons, param, weight);
  return weight;
}

double HNL_LeptonCore::GetElectronIDWeight(const std::vector<Electron>& electrons, AnalyzerParameter& param) {
  double weight = 1.0;
  EvalElectronIDWeight(electrons, param, weight);
  return weight;
}


void HNL_LeptonCore::EvalMuonIDWeight(const std::vector<Muon>& muons, AnalyzerParameter& param, double& ev_weight) {
  if (param.Set_MuIDW || IsDATA) return;
  param.Set_MuIDW = true;

  double this_weight = 1.0;
  mcCorr->IgnoreNoHist = param.MCCorrrectionIgnoreNoHist;

  // Determine systematic direction for Muon scale factors
  int SystDir_MuonIDSF = 0, SystDir_MuonISOSF = 0, SystDir_MuonRecoSF = 0;

  switch (param.syst_) {
  case AnalyzerParameter::MuonRecoSFUp:   SystDir_MuonRecoSF = +1; break;
  case AnalyzerParameter::MuonRecoSFDown: SystDir_MuonRecoSF = -1; break;
  case AnalyzerParameter::MuonIDSFUp:     SystDir_MuonIDSF = +1; break;
  case AnalyzerParameter::MuonIDSFDown:   SystDir_MuonIDSF = -1; break;
  case AnalyzerParameter::MuonISOSFUp:    SystDir_MuonISOSF = +1; break;
  case AnalyzerParameter::MuonISOSFDown:  SystDir_MuonISOSF = -1; break;
  default: break;
  }

  for (const auto& mu : muons) {
    double MiniAODP = std::sqrt(mu.MiniAODPt() * mu.MiniAODPt() + mu.Pz() * mu.Pz());
    double this_pt = mu.MiniAODPt();
    double this_eta = mu.Eta();

    // Determine the appropriate reco_pt
    double reco_pt = (param.k.Muon_RECO_SF == "HighPtMuonRecoSF" || 
		      (param.k.Muon_RECO_SF == "MuonRecoSF" && this_pt > 200)) ? MiniAODP : this_pt;

    // [1] Reco ID SF - Should be applied to all Muons
    double this_recosf = (param.Apply_Weight_RECOSF) ? 
      mcCorr->MuonReco_SF(param.k.Muon_RECO_SF, this_eta, reco_pt, SystDir_MuonRecoSF) : 1.0;

    this_weight *= this_recosf;
    param.w.muonRECOSF *= this_recosf;
    FillWeightHist(param.ChannelDir() + "/RecoMuWeight_" + param.Name, this_recosf);

    // [2] Tracking ID SF
    double this_trackersf = (param.Apply_Weight_MuonTrackerSF) ? 
      mcCorr->MuonTracker_SF("NUM_GlobalMuons", this_eta, reco_pt, 0) : 1.0;

    this_weight *= this_trackersf;
    param.w.muonTrackerSF *= this_trackersf;
    FillWeightHist(param.ChannelDir() + "/TrackerMuWeight_" + param.Name, this_trackersf);

    // [3] ID and ISO SF
    double this_idsf = (param.Apply_Weight_IDSF) ? 
      mcCorr->MuonID_SF(param.k.Muon_ID_SF, this_eta, this_pt, SystDir_MuonIDSF) : 1.0;

    double this_isosf = (param.Apply_Weight_IDSF) ? 
      mcCorr->MuonISO_SF(param.k.Muon_ISO_SF, this_eta, this_pt, SystDir_MuonISOSF) : 1.0;

    this_weight *= this_idsf * this_isosf;
    param.w.muonIDSF *= this_idsf;
    param.w.muonISOSF *= this_isosf;

    FillWeightHist(param.ChannelDir() + "/IDMuWeight_" + param.Name, this_idsf);
    FillWeightHist(param.ChannelDir() + "/ISOMuWeight_" + param.Name, this_isosf);

    if (param.DEBUG) {
      std::cout << "GetMuonSFEventWeight this_idsf=" << this_idsf 
		<< " this_isosf=" << this_isosf << std::endl;
    }
  }

  FillWeightHist(param.ChannelDir() + "/FullMuWeight_" + param.Name, this_weight);

  // Update event weight using the combined correction weight
  if (param.DEBUG) {
    std::cout << "EvalMuonIDWeight  = " << this_weight << std::endl;
  }
  ev_weight *= this_weight;
}



void HNL_LeptonCore::EvalLeptonIDWeight(const std::vector<Lepton*>& leps, AnalyzerParameter& param, double& ev_weight) {

  if(param.Set_ElIDW && param.Set_MuIDW) return;

  if (IsDATA) return;

  param.Set_ElIDW=true;
  param.Set_MuIDW=true;


  double this_weight = 1.0;

  mcCorr->IgnoreNoHist = param.MCCorrrectionIgnoreNoHist;

  // Determine systematic directions for Muon and Electron scale factors
  int SystDir_MuonIDSF = 0, SystDir_MuonISOSF = 0, SystDir_MuonRecoSF = 0;
  int SystDir_ElectronIDSF = 0, SystDir_ElectronRecoSF = 0;

  switch (param.syst_) {
  case AnalyzerParameter::MuonRecoSFUp:     SystDir_MuonRecoSF = +1; break;
  case AnalyzerParameter::MuonRecoSFDown:   SystDir_MuonRecoSF = -1; break;
  case AnalyzerParameter::MuonIDSFUp:       SystDir_MuonIDSF = +1; break;
  case AnalyzerParameter::MuonIDSFDown:     SystDir_MuonIDSF = -1; break;
  case AnalyzerParameter::MuonISOSFUp:      SystDir_MuonISOSF = +1; break;
  case AnalyzerParameter::MuonISOSFDown:    SystDir_MuonISOSF = -1; break;
  case AnalyzerParameter::ElectronRecoSFUp: SystDir_ElectronRecoSF = +1; break;
  case AnalyzerParameter::ElectronRecoSFDown: SystDir_ElectronRecoSF = -1; break;
  case AnalyzerParameter::ElectronIDSFUp:   SystDir_ElectronIDSF = +1; break;
  case AnalyzerParameter::ElectronIDSFDown: SystDir_ElectronIDSF = -1; break;
  default: break;
  }

  for (const auto& lep : leps) {
    double this_pt = lep->Pt();
    double this_eta = lep->Eta();

    if (lep->LeptonFlavour() == Lepton::ELECTRON) {
      double this_recosf = mcCorr->ElectronReco_SF(param.k.Electron_RECO_SF, lep->defEta(), this_pt, SystDir_ElectronRecoSF);
      double this_idsf = mcCorr->ElectronID_SF(param.k.Electron_ID_SF, lep->defEta(), this_pt, SystDir_ElectronIDSF);
      this_weight *= this_recosf * this_idsf;
    } 
    else if (lep->LeptonFlavour() == Lepton::MUON) {
      double this_idsf = mcCorr->MuonID_SF(param.k.Muon_ID_SF, this_eta, this_pt, SystDir_MuonIDSF);
      double this_isosf = mcCorr->MuonISO_SF(param.k.Muon_ISO_SF, this_eta, this_pt, SystDir_MuonISOSF);
      double this_recosf = mcCorr->MuonReco_SF(param.k.Muon_RECO_SF, this_eta, this_pt, SystDir_MuonRecoSF);
            
      this_weight *= this_idsf * this_isosf * this_recosf;
    }
  }

  ev_weight *= this_weight;
}


void HNL_LeptonCore::EvalElectronIDWeight(const std::vector<Electron>& electrons, AnalyzerParameter& param, double& ev_weight) {
  if (param.Set_ElIDW || IsDATA) return;
  param.Set_ElIDW = true;

  mcCorr->IgnoreNoHist = param.MCCorrrectionIgnoreNoHist;
  double this_weight = 1.0;

  // Determine systematic direction for Electron ID and Reco scale factors
  int SystDir_ElectronIDSF = 0, SystDir_ElectronRecoSF = 0;

  switch (param.syst_) {
  case AnalyzerParameter::ElectronRecoSFUp:   SystDir_ElectronRecoSF = +1; break;
  case AnalyzerParameter::ElectronRecoSFDown: SystDir_ElectronRecoSF = -1; break;
  case AnalyzerParameter::ElectronIDSFUp:     SystDir_ElectronIDSF = +1; break;
  case AnalyzerParameter::ElectronIDSFDown:   SystDir_ElectronIDSF = -1; break;
  default: break;
  }

  for (const auto& el : electrons) {
    double pt = (el.Pt() < 15) ? 16 : el.Pt();

    // Electron Reco SF - Applied to all electrons
    double this_recosf = (param.Apply_Weight_RECOSF) ? 
      mcCorr->ElectronReco_SF(param.k.Electron_RECO_SF, el.scEta(), pt, SystDir_ElectronRecoSF) : 1.0;

    this_weight *= this_recosf;
    param.w.electronRECOSF = this_recosf;

    // Electron ID SF - Needs key dependent on ID
    double this_idsf = (param.Apply_Weight_IDSF) ? 
      mcCorr->ElectronID_SF(param.k.Electron_ID_SF, el.scEta(), pt, SystDir_ElectronIDSF) : 1.0;

    this_weight *= this_idsf;
    param.w.electronIDSF = this_idsf;

    // Fill weight histograms
    FillWeightHist(param.ChannelDir() + "/el_reco_sf_" + param.Name, this_recosf);
    FillWeightHist(param.ChannelDir() + "/el_id_sf_" + param.Name, this_idsf);
  }

  if (param.DEBUG) std::cout << "EvalElIDWeight  = " << this_weight << std::endl;

  ev_weight *= this_weight;
}


double HNL_LeptonCore::HNLZvtxSF(const TString& ch) {
  if (IsData || ch.Contains("MuMu") || DataYear == 2016 || DataYear == 2018) {
    return 1.0;
  }
  if (DataYear == 2017) {
    return 0.991;
  }
  return 1.0;
}

double HNL_LeptonCore::HNLZvtxSF(HNL_LeptonCore::Channel ch) {
  // See https://indico.cern.ch/event/697573/contributions/2887077/attachments/1596611/2530584/hltZVtxInefficiency.pdf
  if (ch == MuMu || IsData || DataYear == 2016 || DataYear == 2018) {
    return 1.0;
  }
  if (DataYear == 2017) {
    return 0.991;
  }
  return 1.0;
}



bool HNL_LeptonCore::PassHEMVeto(const std::vector<Lepton*>& leps, double& weight_hem) {
  if (DataEra != "2018") return true;

  int nel_hem = 0;
  for (const auto& ilep : leps) {
    if (ilep->IsMuon()) continue;

    if (ilep->Eta() < -1.25 && ilep->Phi() > -1.62 && ilep->Phi() < -0.82) {
      ++nel_hem;
    }
  }

  return nel_hem == 0;
}


void HNL_LeptonCore::PassJetHEMVeto(const std::vector<Jet>& jets, const TString& Flag, double& weight_hem) {
  // Check status of Jets in HEM region
  int nJets_orig = jets.size();
  int nJets_HEMVeto = 0;

  for (const auto& ijet : jets) {
    bool PassHEM = !(ijet.Eta() < -1.3 && ijet.Phi() > -1.57 && ijet.Phi() < -0.87);

    FillHist(Flag + "/Jet_HEM_EtaPhi", ijet.Eta(), ijet.Phi(), fabs(weight_hem), 100, -5.0, 5.0, 100, -5.0, 5.0);

    if (PassHEM) {
      FillHist(Flag + "/PassVeto_Jet_HEM_EtaPhi", ijet.Eta(), ijet.Phi(), fabs(weight_hem), 100, -5.0, 5.0, 100, -5.0, 5.0);
      ++nJets_HEMVeto;
    } else {
      FillHist(Flag + "/FailVeto_Jet_HEM_EtaPhi", ijet.Eta(), ijet.Phi(), fabs(weight_hem), 100, -5.0, 5.0, 100, -5.0, 5.0);
    }
  }

  FillHist(Flag + "/PassVeto_NJet", nJets_orig, nJets_HEMVeto, fabs(weight_hem), 10, 0.0, 10.0, 10, 0.0, 10.0);
}




double HNL_LeptonCore::GetDYWeakWeight(double mass) {
  if (IsDATA || !IsDYSample) return 1.0;

  static const std::vector<std::pair<double, double>> mass_weight_table = {
    {55,  0.988939}, {60,  0.992556}, {65,  0.996362}, {70,  1.00086},  {75,  1.00593}, 
    {80,  1.00989},  {85,  1.01263},  {90,  1.01373},  {95,  1.01338},  {100, 1.01242}, 
    {110, 1.01078},  {120, 1.00839},  {130, 1.00628},  {140, 1.00461},  {150, 1.0033},  
    {170, 1.00201},  {200, 0.999256}, {250, 0.995825}, {300, 0.992451}, {400, 0.986289}, 
    {500, 0.979024}, {600, 0.972292}, {700, 0.967596}, {800, 0.959725}, {1000, 0.953025}, 
    {1500, 0.935142}, {2000, 0.909548}, {3000, 0.8895}
  };

  for (const auto& [mass_threshold, weight] : mass_weight_table) {
    if (mass < mass_threshold) return weight;
  }

  return 0.900657;
}




void HNL_LeptonCore::ZptCorrectionReset(){
  if(fZptWeightG){
    delete fZptWeightG;
    fZptWeightG=NULL;
  }
  if(fZptWeightYaxis){
    delete fZptWeightYaxis;
    fZptWeightYaxis=NULL;
  }
  for(auto f:fZptWeightY){
    if(f) delete f;
  }
  fZptWeightY.clear();
  if(fZptWeightMaxis){
    delete fZptWeightMaxis;
    fZptWeightMaxis=NULL;
  }
  for(auto f:fZptWeightM){
    if(f) delete f;
  }
  fZptWeightM.clear();
}

double HNL_LeptonCore::GetZptWeight(double pt) const {
  if(!fZptWeightG) return 1.;
  if(pt<0) pt=0;
  if(pt>=650) pt=649.9;
  double sf=fZptWeightG->Eval(pt);
  return sf;
}


double HNL_LeptonCore::GetZptWeight(double pt,double rapidity,double mass) const {
  double sf=GetZptWeight(pt,rapidity);
  if(!fZptWeightM.size()) return sf;
  if(mass==0) return sf;
  if(pt<0) pt=0;
  if(pt>=650) pt=649.9;
  double y=fabs(rapidity);
  double m=mass;
  if(m<fZptWeightMaxis->GetXmin()) m=fZptWeightMaxis->GetXmin();
  if(m>=fZptWeightMaxis->GetXmax()) m=fZptWeightMaxis->GetXmax()-1e-6;

  double mmin=fZptWeightMaxis->GetBinCenter(1);
  double mmax=fZptWeightMaxis->GetBinCenter(fZptWeightMaxis->GetNbins());
  int binm1,binm2;
  if(m<mmin){
    binm1=1;
    binm2=2;
  }else if(m>=mmax){
    binm1=fZptWeightMaxis->GetNbins()-1;
    binm2=fZptWeightMaxis->GetNbins();
  }else{
    int binm=fZptWeightMaxis->FindBin(m);
    if(m>=fZptWeightMaxis->GetBinCenter(binm)){
      binm1=binm;
      binm2=binm+1;
    }else{
      binm1=binm-1;
      binm2=binm;
    }
  }
  double m1=fZptWeightMaxis->GetBinCenter(binm1);
  double m2=fZptWeightMaxis->GetBinCenter(binm2);
  sf*=( (m2-m)*fZptWeightM[binm1]->Eval(pt) + (m-m1)*fZptWeightM[binm2]->Eval(pt) )/(m2-m1);
  return sf;
}

double HNL_LeptonCore::GetZptWeight(double pt,double rapidity) const {
  double sf=GetZptWeight(pt);

  if(!fZptWeightY.size()) return sf;
  if(std::isnan(rapidity)) return sf;
  if(pt<0) pt=0;
  if(pt>=650) pt=649.9;
  double y=fabs(rapidity);
  if(y>=fZptWeightYaxis->GetXmax()) y=fZptWeightYaxis->GetXmax()-1e-6;  

  double ymin=fZptWeightYaxis->GetBinCenter(1);
  double ymax=fZptWeightYaxis->GetBinCenter(fZptWeightYaxis->GetNbins());
  int biny1,biny2;
  if(y<ymin){
    biny1=1;
    biny2=2;
  }else if(y>=ymax){
    biny1=fZptWeightYaxis->GetNbins()-1;
    biny2=fZptWeightYaxis->GetNbins();
  }else{
    int biny=fZptWeightYaxis->FindBin(y);
    if(y>=fZptWeightYaxis->GetBinCenter(biny)){
      biny1=biny;
      biny2=biny+1;
    }else{
      biny1=biny-1;
      biny2=biny;
    }
  }
  double y1=fZptWeightYaxis->GetBinCenter(biny1);
  double y2=fZptWeightYaxis->GetBinCenter(biny2);
  sf*=( (y2-y)*fZptWeightY[biny1]->Eval(pt) + (y-y1)*fZptWeightY[biny2]->Eval(pt) )/(y2-y1);
  return sf;
}



void HNL_LeptonCore::SetupZptWeight(){

  ZptCorrectionReset();

  TString _MCSample=MCSample;
  if(MCSample.Contains("MiNNLO")) _MCSample="MiNNLO";
  if(MCSample.Contains("DYJets_Pt-")) _MCSample="MiNNLO";
  TString zptpath=(TString)getenv("DATA_DIR")+"/Run2/SMP/ZptWeight_"+_MCSample+".root";

  std::ifstream fcheck(zptpath);
  if(fcheck.good()){
    std::cout<<"[ZptCorrection::Setup] using file "+zptpath<<std::endl;
  }else{
    std::cout<<"[ZptCorrection::Setup] no "+zptpath<<std::endl;
    return;
  }
  TFile f(zptpath);
  fZptWeightG=(TF1*)f.Get("zptweight_g");
  if(!fZptWeightG){
    std::cout<<"[ZptCorrection::Setup] no zptweight_g"<<std::endl;
    exit(ENODATA);
  }
  fZptWeightYaxis=(TAxis*)f.Get("yaxis");
  if(!fZptWeightYaxis){
    std::cout<<"[ZptCorrection::Setup] no yaxis"<<std::endl;
    exit(ENODATA);
  }
  fZptWeightY.resize(fZptWeightYaxis->GetNbins()+2,NULL);
  for(int i=1;i<fZptWeightYaxis->GetNbins()+1;i++){
    fZptWeightY[i]=(TF1*)f.Get(Form("zptweight_y%d",i));
    if(!fZptWeightY[i]){
      std::cout<<"[ZptCorrection::Setup] no zptweight_y"+TString(i)<<std::endl;
      exit(ENODATA);
    }
  }
  fZptWeightMaxis=(TAxis*)f.Get("maxis");
  if(!fZptWeightMaxis){
    std::cout<<"[ZptCorrection::Setup] no maxis"<<std::endl;
    exit(ENODATA);
  }
  fZptWeightM.resize(fZptWeightMaxis->GetNbins()+2,NULL);
  for(int i=1;i<fZptWeightMaxis->GetNbins()+1;i++){
    fZptWeightM[i]=(TF1*)f.Get(Form("zptweight_m%d",i));
    if(!fZptWeightM[i]){
      std::cout<<"[ZptCorrection::Setup] no zptweight_m"+TString(i)<<std::endl;
      exit(ENODATA);
    }
  }
}


void HNL_LeptonCore::DeleteZptWeight(){
  ZptCorrectionReset();

}



double HNL_LeptonCore::GetPileUpWeight(int N_pileup, int syst){
  if(IsDATA) return 1.;
  else return mcCorr->GetPileUpWeight(N_pileup, syst);
}



double HNL_LeptonCore::GetZ0Weight(double valx){
  if(IsDATA) return 1.;
  double rt=1.;
  if(GetEra()=="2016preVFP"){
    double data_val=TMath::Gaus(valx,2.46312e-01,3.50458e+00,true);
    double mc_val=TMath::Gaus(valx,9.28612e-01,3.65203e+00,true);
    rt=data_val/mc_val;
  }else if(GetEra()=="2016postVFP"){
    double data_val=TMath::Gaus(valx,2.41640e-01,3.63717e+00,true);
    double mc_val=TMath::Gaus(valx,9.30108e-01,3.65454e+00,true);
    rt=data_val/mc_val;
  }else if(GetEra()=="2017"){
    double data_val=TMath::Gaus(valx,3.81830e-01,3.67614e+00,true);
    double mc_val=TMath::Gaus(valx,8.19642e-01,3.50992e+00,true);
    rt=data_val/mc_val;
  }else if(GetEra()=="2018"){
    double data_val=TMath::Gaus(valx,-1.36030e-01,3.41464e+00,true);
    double mc_val=TMath::Gaus(valx,3.58575e-02,3.50953e+00,true);
    rt=data_val/mc_val;
  }
  if(rt>2) rt=2;
  return rt;
}

