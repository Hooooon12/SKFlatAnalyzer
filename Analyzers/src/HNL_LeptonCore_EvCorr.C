#include "HNL_LeptonCore.h"


double HNL_LeptonCore::GetPDFUncertainty(TString SRBin, int sys){

  if(!MCSample.Contains("Type")) return 1;
  if(sys==0) return 1;

  int SignalRange=0;
  if(MCSample.Contains("M500_")) SignalRange=0;
  if(MCSample.Contains("M600_")) SignalRange=0;
  if(MCSample.Contains("M700_")) SignalRange=0;
  if(MCSample.Contains("M800_")) SignalRange=0;
  if(MCSample.Contains("M900_")) SignalRange=0;
  if(MCSample.Contains("M1000_")) SignalRange=1;
  if(MCSample.Contains("M1100_")) SignalRange=1;
  if(MCSample.Contains("M1200_")) SignalRange=1;
  if(MCSample.Contains("M1300_")) SignalRange=1;
  if(MCSample.Contains("M1400_")) SignalRange=1;
  else SignalRange=2;

  /// Initial numbers from Jihoon 
  if(MCSample.Contains("DY")){
    
    if(SRBin.Contains("SR1")) return (1. + double(sys)* 0.07);
    if(SRBin.Contains("SR2")) return (1. + double(sys)* 0.01);
    if(SRBin.Contains("SR3")) return (1. + double(sys)* 0.025);

    return (1. + double(sys)* 0.02);

  }
  if(MCSample.Contains("VBF")){
    
    if(SRBin.Contains("SR2")) return (1. + double(sys)* 0.05);
    if(SRBin.Contains("SR3")) return (1. + double(sys)* 0.04);
    
    if(SignalRange==1){
      if(SRBin=="SR1_MNbin1") return(1. + double(sys)* 0.018);
      if(SRBin=="SR1_MNbin2") return(1. + double(sys)* 0.01);
      if(SRBin=="SR1_MNbin3") return(1. + double(sys)* 0.002);
      if(SRBin=="SR1_MNbin4") return(1. + double(sys)* 0.018);
      if(SRBin=="SR1_MNbin5") return(1. + double(sys)* 0.025);
      if(SRBin=="SR1_MNbin6") return(1. + double(sys)* 0.01);
      return(1. + double(sys)* 0.02);
    }
    if(SignalRange==2){
      if(SRBin=="SR1_MNbin1") return(1. + double(sys)* 0.1);
      if(SRBin=="SR1_MNbin2") return(1. + double(sys)* 0.03);
      if(SRBin=="SR1_MNbin3") return(1. + double(sys)* 0.03);
      if(SRBin=="SR1_MNbin4") return(1. + double(sys)* 0.03);
      if(SRBin=="SR1_MNbin5") return(1. + double(sys)* 0.03);
      if(SRBin=="SR1_MNbin6") return(1. + double(sys)* 0.01);
      
      if(SRBin=="SR3_MNbin8") return(1. + double(sys)* 0.06);
      if(SRBin=="SR3_MNbin10") return(1. + double(sys)* 0.06);

      return(1. + double(sys)* 0.02);
    }

    if(SignalRange==3){
      if(SRBin=="SR1_MNbin1") return(1. + double(sys)* 0.29);
      if(SRBin=="SR1_MNbin2") return(1. + double(sys)* 0.12);
      if(SRBin=="SR1_MNbin3") return(1. + double(sys)* 0.05);
      if(SRBin=="SR1_MNbin4") return(1. + double(sys)* 0.05);
      if(SRBin=="SR1_MNbin5") return(1. + double(sys)* 0.05);
      if(SRBin=="SR1_MNbin6") return(1. + double(sys)* 0.01);
      return(1. + double(sys)* 0.02);
    }
    return(1. + double(sys)* 0.02);
  }
  if(MCSample.Contains("SSWW")){

    if(SRBin.Contains("SR1")) return (1. + double(sys)* 0.02);
    if(SignalRange==1){
      if(SRBin=="SR2_HTLTbin1") return(1. + double(sys)* 0.01);
      if(SRBin=="SR2_HTLTbin7") return(1. + double(sys)* 0.07);
      return(1. + double(sys)* 0.03);
    }
    if(SignalRange==2){
      if(SRBin=="SR2_HTLTbin1") return(1. + double(sys)* 0.01);
      if(SRBin=="SR2_HTLTbin7") return(1. + double(sys)* 0.012);
      return(1. + double(sys)* 0.04);
    }
    if(SRBin.Contains("SR2")) return (1. + double(sys)* 0.05);
    if(SRBin.Contains("SR3")) return (1. + double(sys)* 0.04);

  }

  return(1. + double(sys)* 0.02);
}

TString HNL_LeptonCore::GetPDFUncertainty(int iw, double& ev_weight ){

  /////////// MAIN CODE TO USE AFTER ANv3
  if(!MCSample.Contains("Type")) return "";

  float NormNom = h_SumW_Scale->GetBinContent(1);

  float Norm, weight_pdf;
  if (iw<weight_PDF->size()) {
    Norm = h_SumW_PDF->GetBinContent(iw+1);
    ev_weight = weight_PDF->at(iw)*(NormNom/Norm);
    
    return "_Syst_PDF"+TString::Itoa(iw,10);
  }
  
  ev_weight=1;
  return "";


}

double HNL_LeptonCore::GetScaleUncertainty(int sys){
  
  if(!MCSample.Contains("Type")) return 1;
  if(sys==0) return 1;
  if(sys == 1){
    double min_var=0;
    for(int iw=0; iw < weight_Scale->size() -2 ; iw++) {
      if(weight_Scale->at(iw) > min_var) min_var = weight_Scale->at(iw);
    }
    return min_var;
  }
  if(sys==-1){
    double max_var=1000;
    for(int iw=0; iw < weight_Scale->size() -2; iw++) {
      if(weight_Scale->at(iw) <max_var) max_var = weight_Scale->at(iw);
    }
    return max_var;
  }
  return 1;
}

double HNL_LeptonCore::GetMuonIDWeight(std::vector<Muon> muons, AnalyzerParameter param){
  double tmpW = 1.;
  EvalMuonIDWeight(muons,param, tmpW);
  return tmpW;
}
double HNL_LeptonCore::GetElectronIDWeight(std::vector<Electron> electrons,AnalyzerParameter param){
  double tmpW =1.;
  EvalElectronIDWeight(electrons,param,tmpW);
  return tmpW;
}

void HNL_LeptonCore::EvalMuonIDWeight(std::vector<Muon> muons,AnalyzerParameter& param , double& ev_weight ){

  if(param.Set_MuIDW) return;
  if(IsDATA) return ;
  param.Set_MuIDW=true;

  double this_weight(1.);

  mcCorr->IgnoreNoHist = param.MCCorrrectionIgnoreNoHist;

  for (auto mu: muons){
    double MiniAODP = sqrt( mu.MiniAODPt() * mu.MiniAODPt() + mu.Pz() * mu.Pz() );
    double this_pt  = mu.MiniAODPt();
    double this_eta = mu.Eta();

    int SystDir_MuonIDSF(0), SystDir_MuonISOSF(0), SystDir_MuonRecoSF (0);
    if(param.syst_ == AnalyzerParameter::MuonRecoSFUp)        SystDir_MuonRecoSF = +1;
    else if(param.syst_ == AnalyzerParameter::MuonRecoSFDown) SystDir_MuonRecoSF = -1;
    else if(param.syst_ == AnalyzerParameter::MuonIDSFUp)     SystDir_MuonIDSF   = +1;
    else if(param.syst_ == AnalyzerParameter::MuonIDSFDown)   SystDir_MuonIDSF   = -1;
    else if(param.syst_ == AnalyzerParameter::MuonISOSFUp)    SystDir_MuonISOSF  = +1;
    else if(param.syst_ == AnalyzerParameter::MuonISOSFDown)  SystDir_MuonISOSF  = -1;

    double reco_pt = (param.k.Muon_RECO_SF  == "HighPtMuonRecoSF") ?  MiniAODP : this_pt; ////                                                                                                                       
    /// [1]   RECO ID SF   --- SHOULD BE USED ON ALL MUONS
    if(param.k.Muon_RECO_SF  == "MuonRecoSF" && this_pt > 200)  reco_pt=MiniAODP;
    double this_recosf = (param.Apply_Weight_RECOSF) ?  mcCorr->MuonReco_SF(param.k.Muon_RECO_SF, this_eta, reco_pt,SystDir_MuonRecoSF) : 1. ;
    this_weight *= this_recosf;
    FillWeightHist(param.ChannelDir()+"/RecoMuWeight_"+param.Name,this_recosf);
    param.w.muonRECOSF=this_recosf;
    
    /// [2]  TRACKING ID SF  (Taken from https://github.com/sansan9401/SKFlatAnalyzer/blob/Run2UltraLegacy_asym/ branch Oct 14 23)
    double this_trackersf = (param.Apply_Weight_MuonTrackerSF) ? mcCorr->MuonTracker_SF("NUM_GlobalMuons", this_eta, reco_pt,0) : 1. ;
    this_weight *= this_trackersf;
    FillWeightHist(param.ChannelDir()+"/TrackerMuWeight_"+param.Name,this_trackersf);
    param.w.muonTrackerSF =this_trackersf;

    /// [3] ID SF needs KEY input 
    double this_idsf   = (param.Apply_Weight_IDSF) ?  mcCorr->MuonID_SF (param.k.Muon_ID_SF,  this_eta, this_pt,SystDir_MuonIDSF) : 1.;
    double this_isosf  = (param.Apply_Weight_IDSF) ?  mcCorr->MuonISO_SF(param.k.Muon_ISO_SF, this_eta, this_pt,SystDir_MuonISOSF) : 1. ;
    param.w.muonIDSF  = this_idsf;
    param.w.muonISOSF = this_isosf;
    FillWeightHist(param.ChannelDir()+"/IDMuWeight_"+param.Name,this_idsf);
    FillWeightHist(param.ChannelDir()+"/ISOMuWeight_"+param.Name,this_isosf);
    this_weight *= this_idsf*this_isosf;
    if(param.DEBUG) cout << "GetMuonSFEventWeight this_idsf=" << this_idsf << " this_isosf=" << this_isosf  << endl;
    
  }// end of muon loop                                                                                                                                                                                                                                               
  FillWeightHist(param.ChannelDir()+"/FullMuWeight_"+param.Name,this_weight);
  
  // Update ev weight using combined corr weight
  if(param.DEBUG) cout << "EvalMuonIDWeight  = " <<  this_weight << endl;
  ev_weight*=this_weight;
  return;
}

void HNL_LeptonCore::EvalLeptonIDWeight(std::vector<Lepton *> leps, AnalyzerParameter& param , double& ev_weight ){

  if(IsDATA) return;
  
  double this_weight(1.);
  if(!IsDATA){

    mcCorr->IgnoreNoHist = param.MCCorrrectionIgnoreNoHist;

    int SystDir_MuonIDSF(0), SystDir_MuonISOSF(0), SystDir_MuonRecoSF (0);
    if(param.syst_ == AnalyzerParameter::MuonRecoSFUp)SystDir_MuonRecoSF = +1;
    else if(param.syst_ == AnalyzerParameter::MuonRecoSFDown)SystDir_MuonRecoSF = -1;
    else if(param.syst_ == AnalyzerParameter::MuonIDSFUp)  SystDir_MuonIDSF = +1;
    else if(param.syst_ == AnalyzerParameter::MuonIDSFDown)  SystDir_MuonIDSF = -1;
    else if(param.syst_ == AnalyzerParameter::MuonISOSFUp) SystDir_MuonISOSF  = +1;
    else if(param.syst_ == AnalyzerParameter::MuonISOSFDown) SystDir_MuonISOSF  = -1;

    int SystDir_ElectronIDSF(0),SystDir_ElectronRecoSF (0);
    if(param.syst_ == AnalyzerParameter::ElectronRecoSFUp)SystDir_ElectronRecoSF = +1;
    else if(param.syst_ == AnalyzerParameter::ElectronRecoSFDown)SystDir_ElectronRecoSF = -1;
    else if(param.syst_ == AnalyzerParameter::ElectronIDSFUp)  SystDir_ElectronIDSF = +1;
    else if(param.syst_ == AnalyzerParameter::ElectronIDSFDown)  SystDir_ElectronIDSF = -1;

    for (auto lep: leps){
      if(lep->LeptonFlavour() == Lepton::ELECTRON){
        double this_recosf  = mcCorr->ElectronReco_SF(param.k.Electron_RECO_SF,lep->defEta(),lep->Pt(), SystDir_ElectronRecoSF);
        double this_idsf    = mcCorr->ElectronID_SF(param.k.Electron_ID_SF, lep->defEta(), lep->Pt(), SystDir_ElectronIDSF);

	this_weight *= this_recosf*this_idsf;
      }
      if(lep->LeptonFlavour() == Lepton::MUON){

	double this_pt  = lep->Pt();
        double this_eta = lep->Eta();

	double this_idsf   = mcCorr->MuonID_SF (param.k.Muon_ID_SF,  this_eta, this_pt,SystDir_MuonIDSF);
        double this_isosf  = mcCorr->MuonISO_SF(param.k.Muon_ISO_SF, this_eta, this_pt,SystDir_MuonISOSF);

	this_weight *= this_idsf*this_isosf;

	//double reco_pt = (param.Muon_RECO_SF_Key  == "HighPtMuonRecoSF") ?  MiniAODP : this_pt;                                                                                                                                                                                                                                                                                                                                                                                                                                     
        double reco_pt =this_pt;
        double this_recosf = mcCorr->MuonReco_SF(param.k.Muon_RECO_SF, this_eta, reco_pt,SystDir_MuonRecoSF);

	this_weight *= this_recosf;

      } 
    }
  }
  
  ev_weight*=this_weight;
  return;


}
void  HNL_LeptonCore::EvalElectronIDWeight(std::vector<Electron> electrons, AnalyzerParameter& param , double& ev_weight ){

  if(param.Set_ElIDW) return;
  if(IsDATA) return ;
  param.Set_ElIDW=true;
  
  mcCorr->IgnoreNoHist = param.MCCorrrectionIgnoreNoHist;
  double this_weight(1.);

  int SystDir_ElectronIDSF(0), SystDir_ElectronRecoSF (0);
  if(param.syst_ == AnalyzerParameter::ElectronRecoSFUp)        SystDir_ElectronRecoSF = +1;
  else if(param.syst_ == AnalyzerParameter::ElectronRecoSFDown) SystDir_ElectronRecoSF = -1;
  else if(param.syst_ == AnalyzerParameter::ElectronIDSFUp)     SystDir_ElectronIDSF = +1;
  else if(param.syst_ == AnalyzerParameter::ElectronIDSFDown)   SystDir_ElectronIDSF = -1;

  for (auto el: electrons){
    
    double pt = (el.Pt() < 15) ? 16 : el.Pt();

    /// [1]   RECO ID SF   --- SHOULD BE USED ON ALL Electrons
    double this_recosf  = (param.Apply_Weight_RECOSF) ?  mcCorr->ElectronReco_SF(param.k.Electron_RECO_SF,el.scEta(),pt, SystDir_ElectronRecoSF) : 1.;

    this_weight *= this_recosf;
    param.w.electronRECOSF = this_recosf;

    /// [2]   ID SF   --- Needs Key dep on ID
    double this_idsf    = (param.Apply_Weight_IDSF) ?  mcCorr->ElectronID_SF(param.k.Electron_ID_SF, el.scEta(), pt, SystDir_ElectronIDSF) : 1.;    
    this_weight *= this_idsf;
    param.w.electronIDSF =this_idsf;
    FillWeightHist(param.ChannelDir()+"/el_reco_sf_"+param.Name, this_recosf);
    FillWeightHist(param.ChannelDir()+"/el_id_sf_"+param.Name, this_idsf);
  }
  if(param.DEBUG)  cout << "EvalElIDWeight  = " <<  this_weight << endl;

  ev_weight*=this_weight;
  
  return;
}



double HNL_LeptonCore::HNLZvtxSF(TString  ch){
  
  if(IsData) return 1.;
  if(ch.Contains("MuMu")) return 1.;
  if(DataYear==2016) return 1.;
  if(DataYear==2018) return 1.;
  if(DataYear==2017) return 0.991;

  return 1.;
}

double HNL_LeptonCore::HNLZvtxSF(HNL_LeptonCore::Channel ch){
  // See https://indico.cern.ch/event/697573/contributions/2887077/attachments/1596611/2530584/hltZVtxInefficiency.pdf                                         

  if(ch == MuMu) return 1.;

  if(IsData) return 1.;
  if(DataYear==2016) return 1.;
  if(DataYear==2018) return 1.;
  if(DataYear==2017) return 0.991;

  return 1.;
}



bool HNL_LeptonCore::PassHEMVeto(std::vector<Lepton *> leps, double& weight_hem){
  
  if (DataEra!="2018") return true;

  int nel_hem(0);
  for(auto ilep : leps){
    if(ilep->IsMuon()) continue;
    
    if (ilep->Eta() < -1.25){
      if((ilep->Phi() < -0.82) && (ilep->Phi() > -1.62)) nel_hem++;
    }
  }

  if(nel_hem >0) return false;

  return true;

}

void  HNL_LeptonCore::PassJetHEMVeto(vector<Jet> jets, TString Flag, double& weight_hem){
 
  //// Check status of Jets in HEM region  

  std::vector<Muon> muons = SelectMuons("POGLoose", 5, 2.8);
  
  int nJets_orig = jets.size();
  int nJets_HEMVeto (0);
  for(auto  ijet : jets) {
    
    bool PassHEM=true;
    FillHist( Flag+"/Jet_HEM_EtaPhi", ijet.Eta(), ijet.Phi(),  fabs(weight_hem), 100, -5.,5., 100, -5., 5.);
    
    if (ijet.Eta() < -1.3){
      if((ijet.Phi() < -0.87) && (ijet.Phi() > -1.57)) PassHEM=false;
    }
    
    if(PassHEM)   {
      FillHist( Flag+"/PassVeto_Jet_HEM_EtaPhi", ijet.Eta(), ijet.Phi(),  fabs(weight_hem), 100, -5.,5., 100, -5., 5.);
      nJets_HEMVeto++;
    }
    else       FillHist( Flag+"/FailVeto_Jet_HEM_EtaPhi", ijet.Eta(), ijet.Phi(),  fabs(weight_hem), 100, -5.,5., 100, -5., 5.);

  }
  
  FillHist( Flag+"/PassVeto_NJet", nJets_orig, nJets_HEMVeto,  fabs(weight_hem), 10, 0.,10., 10,0., 10.);
  

  return;
}

double HNL_LeptonCore::GetDYWeakWeight(double mass){
  if(IsDATA) return 1.;
  if(!IsDYSample) return 1.;
  if(mass<55) return 0.988939;
  else if(mass<60) return 0.992556;
  else if(mass<65) return 0.996362;
  else if(mass<70) return 1.00086;
  else if(mass<75) return 1.00593;
  else if(mass<80) return 1.00989;
  else if(mass<85) return 1.01263;
  else if(mass<90) return 1.01373;
  else if(mass<95) return 1.01338;
  else if(mass<100) return 1.01242;
  else if(mass<110) return 1.01078;
  else if(mass<120) return 1.00839;
  else if(mass<130) return 1.00628;
  else if(mass<140) return 1.00461;
  else if(mass<150) return 1.0033;
  else if(mass<170) return 1.00201;
  else if(mass<200) return 0.999256;
  else if(mass<250) return 0.995825;
  else if(mass<300) return 0.992451;
  else if(mass<400) return 0.986289;
  else if(mass<500) return 0.979024;
  else if(mass<600) return 0.972292;
  else if(mass<700) return 0.967596;
  else if(mass<800) return 0.959725;
  else if(mass<1000) return 0.953025;
  else if(mass<1500) return 0.935142;
  else if(mass<2000) return 0.909548;
  else if(mass<3000) return 0.8895;
  else return 0.900657;
}

double HNL_LeptonCore::GetZptWeight(double mass,double rapidity,double pt,TString opt){

  if(!fZptWeightG) return 1.;

  if(mass==0) return 1.;
  if(isnan(rapidity)) return 1.;
  double m=mass;
  if(m<fZptWeightMaxis->GetXmin()) m=fZptWeightMaxis->GetXmin();
  if(m>=fZptWeightMaxis->GetXmax()) m=fZptWeightMaxis->GetXmax()-1e-6;
  double y=fabs(rapidity);
  if(y>=fZptWeightYaxis->GetXmax()) y=fZptWeightYaxis->GetXmax()-1e-6;
  if(pt<0) pt=0;
  if(pt>=650) pt=649.9;
  double sf=1.;

  opt.ToUpper();
  if(opt.Contains("G")) sf*=fZptWeightG->Eval(pt);

  if(opt.Contains("Y")){
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
  }

  if(opt.Contains("M")){
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
  }


  return sf;
}


void HNL_LeptonCore::SetupZptWeight(){
  TString _MCSample=MCSample;
  if(MCSample.Contains("MiNNLO")) _MCSample="MiNNLO";
  TString zptpath=(TString)getenv("DATA_DIR")+"/"+GetEra()+"/SMP/ZptWeight_"+_MCSample+".root";
  if(IsExists(zptpath)){
    cout<<"[HNL_LeptonCore::SetupZptWeight] using file "+zptpath<<endl;
  }else{
    cout<<"[HNL_LeptonCore::SetupZptWeight] no "+zptpath<<endl;
    return;
  }
  DeleteZptWeight();
  TFile f(zptpath);
  fZptWeightG=(TF1*)f.Get("zptweight_g");
  if(!fZptWeightG){
    cout<<"[HNL_LeptonCore::SetupZptWeight] no zptweight_g"<<endl;
    exit(ENODATA);
  }
  fZptWeightYaxis=(TAxis*)f.Get("yaxis");
  if(!fZptWeightYaxis){
    cout<<"[HNL_LeptonCore::SetupZptWeight] no yaxis"<<endl;
    exit(ENODATA);
  }
  fZptWeightY.resize(fZptWeightYaxis->GetNbins()+2,NULL);
  for(int i=1;i<fZptWeightYaxis->GetNbins()+1;i++){
    fZptWeightY[i]=(TF1*)f.Get(Form("zptweight_y%d",i));
    if(!fZptWeightY[i]){
      cout<<"[HNL_LeptonCore::SetupZptWeight] no zptweight_y"+TString(i)<<endl;
      exit(ENODATA);
    }
  }
  fZptWeightMaxis=(TAxis*)f.Get("maxis");
  if(!fZptWeightMaxis){
    cout<<"[HNL_LeptonCore::SetupZptWeight] no maxis"<<endl;
    exit(ENODATA);
  }
  fZptWeightM.resize(fZptWeightMaxis->GetNbins()+2,NULL);
  for(int i=1;i<fZptWeightMaxis->GetNbins()+1;i++){
    fZptWeightM[i]=(TF1*)f.Get(Form("zptweight_m%d",i));
    if(!fZptWeightM[i]){
      cout<<"[HNL_LeptonCore::SetupZptWeight] no zptweight_m"+TString(i)<<endl;
      exit(ENODATA);
    }
  }
}


void HNL_LeptonCore::DeleteZptWeight(){
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

