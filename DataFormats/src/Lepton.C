#include "Lepton.h"

ClassImp(Lepton)

Lepton::Lepton() : Particle() {
  
  j_run_era=""; /// SetEra
  j_run_year=0; /// SetEra

  j_MiniAODPt = -999.;

  
  mu_mva_cut_fake_2016 = 0.72;
  mu_mva_cut_fake_2017 = 0.64;
  mu_mva_cut_fake_2018 = 0.64;

  /// EL BARREL
  el_mva_cut_fake_2016_B = 0.25;
  el_mva_cut_fake_2017_B = 0.4;
  el_mva_cut_fake_2018_B = 0.4;
  /// EL EC
  el_mva_cut_fake_2016_EC = 0.4;
  el_mva_cut_fake_2017_EC = 0.45;
  el_mva_cut_fake_2018_EC = 0.5;

  el_mva_cut_cf_2016_B = 0.6;
  el_mva_cut_cf_2017_B = 0.6;
  el_mva_cut_cf_2018_B = 0.6;
  el_mva_cut_cf_2016_EC = 0.6;
  el_mva_cut_cf_2017_EC = 0.6;
  el_mva_cut_cf_2018_EC = 0.6;

  TopMVA_cut = 0.64;
  
  j_dXY = -999.;
  j_dXYerr = -999.;
  j_dZ = -999.;
  j_dZerr = -999.;
  j_IP3D = -999.;
  j_IP3Derr = -999.;
  j_unCorrPt = -999;
  j_elSCeta= -999;
  j_RelIso = -999.;
  j_MiniRelIso = -999.;
  j_HTOverPt = -999;
  j_ptcone = -999.;
  j_lep_jetptrel=-999.;
  j_lep_jetptratio=-999.;
  j_lep_jetbscore=-999.;
  j_lep_jetcvsbscore=-999.;
  j_lep_jetcvslscore=-999.;
  j_lep_jetflavour=-999;
  j_LeptonFlavour = NONE;
  j_MiniIso_ChHad = -999.;
  j_MiniIso_NHad = -999.;
  j_MiniIso_PhHad = -999.;
  j_Iso_ChHad = -999.;
  j_Iso_NHad = -999.;
  j_Iso_PhHad = -999.;
  j_jetntracks = -1;
  j_jetntracks_mva = -1;
  j_lep_mva=-999;
  j_LeptonType = -999;
  j_LeptonIsCF = false;
  j_LeptonIsPromptConv = false;
  j_passID = false;
  j_IDSet= false;
  j_TIDName="";

  j_trkiso = -999.;

  j_lep_map_mva_hnl.clear();
  j_lep_mva_hnl_fake_v4=-999;
  j_lep_mva_hnl_fake_v4_hf=-999;
  j_lep_mva_hnl_fake_v4_hfb=-999;
  j_lep_mva_hnl_fake_v4_hfc=-999;
  j_lep_mva_hnl_fake_v4_lf=-999;
  j_lep_mva_hnl_fake_v4_top=-999;

  j_lep_mva_hnl_fake_ed_v4=-999;
  j_lep_mva_hnl_fake_ed_v4_hf=-999;
  j_lep_mva_hnl_fake_ed_v4_hfb=-999;
  j_lep_mva_hnl_fake_ed_v4_hfc=-999;
  j_lep_mva_hnl_fake_ed_v4_lf=-999;
  j_lep_mva_hnl_fake_ed_v4_top=-999;

  j_lep_mva_hnl_fake_ed_v5=-999;
  j_lep_mva_hnl_fake_v5_hfb=-999;
  j_lep_mva_hnl_fake_v5_hfc=-999;
  j_lep_mva_hnl_fake_v5_lf=-999;

  j_lep_mva_hnl_fake_QCD_LFvsHF_v5=-999;
  j_lep_mva_hnl_fake_QCD_BvsC_v5=-999;
  j_lep_mva_hnl_fake_LF1_v5=-999;
  j_lep_mva_hnl_fake_LF2_v5=-999;

  
  j_lep_mva_hnl_conv_v4=-999;
  j_lep_mva_hnl_conv_ed_v4=-999;
  j_lep_mva_hnl_conv_ed_v5=-999;

  j_lep_mva_hnl_cf_v4=-999;
  j_lep_mva_hnl_ed_cf_v4=-999;

  j_lep_mva_hnl_ed_cf_v5=-999;
  j_lep_mva_hnl_ed_cf_v5Pt=-999;

}

void Lepton::SetMiniAODPt(double d){
  j_MiniAODPt = d;
}


void Lepton::PrintObject(TString label){
  cout << "------------------------------" << endl;
  cout << "Pt = " << this->Pt() << endl;
  cout << "Eta = " << this->fEta() << endl;
  cout << "j_dXY = " << j_dXY << endl;
  cout << "j_dXYerr = " << j_dXYerr << endl;
  cout << "j_dZ = " << j_dZ  << endl;
  cout << "j_dZerr = " << j_dZerr  << endl;
  cout << "j_IP3D = " << j_IP3D  << endl;
  cout << "j_IP3Derr = " << j_IP3Derr  << endl;
  cout << "j_RelIso = " << j_RelIso  << endl;
  cout << "j_MiniRelIso = " << j_MiniRelIso << endl;
  cout << "j_ptcone = " << j_ptcone  << endl;
  cout << "j_lep_jetptrel= " << j_lep_jetptrel << endl;
  cout << "j_lep_jetptratio= " << j_lep_jetptratio << endl;
  cout << "j_lep_jetbscore = " << j_lep_jetbscore << endl;
  cout << "j_MiniIso_ChHad = " << j_MiniIso_ChHad   << endl;
  cout << "j_MiniIso_NHad = " << j_MiniIso_NHad  << endl;
  cout << "j_MiniIso_PhHad = " << j_MiniIso_PhHad  << endl;
  cout << "j_Iso_ChHad = " << j_Iso_ChHad  << endl;
  cout << "j_Iso_NHad = " << j_Iso_NHad  << endl;
  cout << "j_Iso_PhHad = " << j_Iso_PhHad  << endl;
  cout << "j_jetntracks = " << j_jetntracks  << endl;
  cout << "j_jetntracks_mva = " << j_jetntracks_mva  << endl;
  cout << "j_lep_mva= " << j_lep_mva  << endl;
  cout << "j_LeptonType = " << j_LeptonType  << endl;

  
  return;
}

Lepton::~Lepton(){

}

bool Lepton::Pass_MVA_BBEC(TString MVALabel , double cut_b, double cut_ec, TString s_helper) const {
  
  double mva_value = MVAKey(MVALabel);
    
  if( IsBB()){
    if(!Pass_MVA(mva_value, cut_b,  s_helper))   return false;
    
  }
  else{
    if(!Pass_MVA(mva_value, cut_ec, s_helper))   return false;
  }
  return true;

}

bool Lepton::Pass_MVA(double mva, double cut, TString s_mva) const {
  
  if(mva == -999){

    cout<<"[Electron::PassMVA] mva value not set "<< s_mva<<endl;
    exit(ENODATA);
  }

  //  cout << "PassULMVA " << s_mva << " " << mva << " " << cut <<  " ( mva < cut) = " << ( mva > cut)  <<endl;
  if ( mva < cut)  return false;
  return true;
}


void Lepton::SetTrkIso(double trkiso){
  j_trkiso=trkiso;
}




void Lepton::SetHNL_FakeLepMVAV4(double mvafake,double mvafake_hf, double mvafake_hfb,double mvafake_hfc, double mvafake_lf, double mvafake_top){

  j_lep_mva_hnl_fake_v4=mvafake;
  j_lep_mva_hnl_fake_v4_hf=mvafake_hf;
  j_lep_mva_hnl_fake_v4_hfb=mvafake_hfb;
  j_lep_mva_hnl_fake_v4_hfc=mvafake_hfc;
  j_lep_mva_hnl_fake_v4_lf=mvafake_lf;
  j_lep_mva_hnl_fake_v4_top=mvafake_top;

}

void Lepton::SetHNL_FakeLepMVA_EtaDependantV4(double mvafake,double mvafake_hf, double mvafake_hfb,double mvafake_hfc, double mvafake_lf, double mvafake_top){

  j_lep_mva_hnl_fake_ed_v4=mvafake;
  j_lep_mva_hnl_fake_ed_v4_hf=mvafake_hf;
  j_lep_mva_hnl_fake_ed_v4_hfb=mvafake_hfb;
  j_lep_mva_hnl_fake_ed_v4_hfc=mvafake_hfc;
  j_lep_mva_hnl_fake_ed_v4_lf=mvafake_lf;
  j_lep_mva_hnl_fake_ed_v4_top=mvafake_top;

}

void Lepton::SetHNL_FakeLepMVA_EtaDependantV5(double mvafake,  double mvafake_hfb,double mvafake_hfc, double mvafake_lf){

  j_lep_mva_hnl_fake_ed_v5=mvafake;
  j_lep_mva_hnl_fake_v5_hfb=mvafake_hfb;
  j_lep_mva_hnl_fake_v5_hfc=mvafake_hfc;
  j_lep_mva_hnl_fake_v5_lf=mvafake_lf;

}



void Lepton::SetHNL_FakeLepMVAMuonV4(double mvafake){
  j_lep_mva_hnl_fake_v4=mvafake;
}

void Lepton::SetHNL_FakeLepMVAMuon_EtaDependantV4(double mvafake){
  j_lep_mva_hnl_fake_ed_v4=mvafake;
}

void Lepton::SetHNL_FakeFlavourLepMVA_V5(double mvafake1,double mvafake2, double mvafake3,double mvafake4){

  j_lep_mva_hnl_fake_QCD_LFvsHF_v5=mvafake1;
  j_lep_mva_hnl_fake_QCD_BvsC_v5=mvafake2;
  j_lep_mva_hnl_fake_LF1_v5=mvafake3;
  j_lep_mva_hnl_fake_LF2_v5=mvafake4;

}




void Lepton::SetHNL_ConvLepMVAV4( double mvaconv){
  j_lep_mva_hnl_conv_v4=mvaconv;
}

void Lepton::SetHNL_ConvLepMVA_EtaDependantV4( double mvaconv){
  j_lep_mva_hnl_conv_ed_v4=mvaconv;
}
void Lepton::SetHNL_ConvLepMVA_EtaDependantV5( double mvaconv){

  j_lep_mva_hnl_conv_ed_v5=mvaconv;

}

void Lepton::SetHNL_CFLepMVAV4(double mvacfv4) {
  j_lep_mva_hnl_cf_v4=mvacfv4;
}

void Lepton::SetHNL_CFLepMVA_EtaDependantV4(double mvacfv4){
  j_lep_mva_hnl_ed_cf_v4=mvacfv4;

}

void Lepton::SetHNL_CFLepMVA_EtaDependantV5(double mvacfv5, double mvacfv5_pt) {
  j_lep_mva_hnl_ed_cf_v5=mvacfv5;
  j_lep_mva_hnl_ed_cf_v5Pt=mvacfv5_pt;
}


void Lepton::SetJetNTracks(double d){
  j_jetntracks =d;
}

void Lepton::SetJetNTracksMVA(double d){
  j_jetntracks_mva =d;
}

void Lepton::SetJetPtRel(double ptrel){

  j_lep_jetptrel = ptrel;
}

void Lepton::SetJetPtRatio(double ptr){

  j_lep_jetptratio = ptr;
}


void Lepton::SetCloseJetBScore(double bscore){

  j_lep_jetbscore = bscore;
}


void Lepton::SetCloseJetCvsBScore(double score){
  j_lep_jetcvsbscore= score;
}


void Lepton::SetCloseJetCvsLScore(double score){

  j_lep_jetcvslscore = score;
}

void Lepton::SetCloseJetFlavour(int flav){

  j_lep_jetflavour = flav;
}


void Lepton::SetLepMVA(double MVA){
  j_lep_mva = MVA;
}

void Lepton::SetUncorrectedPt(double uncorrPt){
  j_unCorrPt = uncorrPt;
}

void Lepton::SetElSCEta(double sceta){
  j_elSCeta=sceta;
}

void Lepton::SetHTOverPt(double f){

  j_HTOverPt = (f / this->Pt());

}


void Lepton::SetdXY(double dXY, double dXYerr){
  j_dXY = dXY;
  j_dXYerr = dXYerr;
}

void Lepton::SetdZ(double dZ, double dZerr){
  j_dZ = dZ;
  j_dZerr = dZerr;
}

void Lepton::SetIP3D(double IP3D, double IP3Derr){
  j_IP3D = IP3D;
  j_IP3Derr = IP3Derr;
}

void Lepton::SetRelIso(double r){
  j_RelIso = r;
}

void Lepton::SetMiniIso(double ch, double nh, double ph, double pu, double rho, double EA){

  double r_mini = miniIsoDr();

  double correction = rho * EA * (r_mini/0.3) * (r_mini/0.3);
  double correctedIso = ch + std::max(0.0, nh + ph - correction);
  
  j_MiniRelIso = correctedIso/this->Pt();

  j_MiniIso_ChHad =ch;
  j_MiniIso_NHad =nh;
  j_MiniIso_PhHad =ph;


}

void Lepton::SetLepIso(double ch, double nh, double ph){

  j_Iso_ChHad =ch;
  j_Iso_NHad =nh;
  j_Iso_PhHad =ph;

}


void Lepton::SetLeptonFlavour(Flavour f){
  j_LeptonFlavour = f;
}

void Lepton::SetLeptonType(int t){
  j_LeptonType= t;
}

void Lepton::SetLeptonIsCF(bool t){
  j_LeptonIsCF= t;
}
void Lepton::SetLeptonIsPromptConv(bool t){
  j_LeptonIsPromptConv= t;
}

void Lepton::SetID(){
  j_IDSet = true;
}

void Lepton::SetPassID(bool p, TString IDName){

  j_IDSet= true;
  j_passID = p;
  j_TIDName = IDName;
}



void Lepton::Print(){
  if(IsElectron()) cout << "Electron\t";
  else if(IsMuon()) cout << "Muon\t";
  else cout << "NoFlav\t";
  cout << "(Pt, Eta, Phi, M, Charge) = " << Pt() << "\t" << Eta() << "\t" << Phi() << "\t" << M() << "\t" << Charge() << endl;
}
