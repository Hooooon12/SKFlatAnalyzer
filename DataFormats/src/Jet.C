#include "Jet.h"

ClassImp(Jet)

Jet::Jet() : Particle() {
  j_area=-999.;
  j_issmeared=false;
  j_partonFlavour=-999;
  j_hadronFlavour=-999;
  j_GenHFHadronMatcher_flavour=-999;
  j_GenHFHadronMatcher_origin=-999;
  j_PxUnSmeared=-999.;
  j_PyUnSmeared=-999.;
  j_DeepCSV=-999.;
  j_DeepCSV_CvsL=-999.;
  j_DeepCSV_CvsB=-999.;
  j_DeepJet=-999;
  j_DeepJet_CvsL=-999;
  j_DeepJet_CvsB=-999;
  j_chargedHadronEnergyFraction=-999.;
  j_neutralHadronEnergyFraction=-999.;
  j_neutralEmEnergyFraction=-999.;
  j_chargedEmEnergyFraction=-999.;
  j_muonEnergyFraction=-999.;
  j_chargedMultiplicity=-999;
  j_neutralMultiplicity=-999;
  j_PileupJetId=-999.;
  j_jec_l1=1;
  j_jec_full=1;
  j_En_up=1.;
  j_En_down=1.;;
  j_Res  = 1.;
  j_Res_up = 1.;
  j_Res_down = 1.;
  j_bJetNN_corr=1.;
  j_bJetNN_res=-999.;
  j_cJetNN_corr=1.;
  j_cJetNN_res=-999.;
  j_nvtx_tracks=-999;  
  j_tightJetID=false;
  j_tightLepVetoJetID=false;
}

void Jet::PrintObject(TString label){
  
  cout << "Jet::PrintObject  " << label << endl;
  cout << "j_area " << j_area << endl;
  cout << "j_partonFlavour " <<  j_partonFlavour << endl;
  cout << "j_hadronFlavour " << j_hadronFlavour <<  endl;
  cout << "j_GenHFHadronMatcher_flavour " << j_GenHFHadronMatcher_flavour << endl;
  cout << "j_GenHFHadronMatcher_origin " <<  j_GenHFHadronMatcher_origin << endl;
  cout << "j_PxUnSmeared " << j_PxUnSmeared  << endl;
  cout << " j_PyUnSmeared " << j_PyUnSmeared << endl;
  cout << "j_DeepCSV " << j_DeepCSV << endl;
  cout << "j_DeepCSV_CvsL " << j_DeepCSV_CvsL<< endl;
  cout << "j_DeepCSV_CvsB " << j_DeepCSV_CvsB<< endl;
  cout << "j_DeepJet " << j_DeepJet<< endl;
  cout << "j_DeepJet_CvsL " << j_DeepJet_CvsL<< endl;
  cout << "j_DeepJet_CvsB " << j_DeepJet_CvsB<< endl;
  cout << "j_chargedHadronEnergyFraction " << j_chargedHadronEnergyFraction<< endl;
  cout << "j_neutralHadronEnergyFraction " << j_neutralHadronEnergyFraction<< endl;
  cout << "j_neutralEmEnergyFraction " << j_neutralEmEnergyFraction<< endl;
  cout << "j_chargedEmEnergyFraction " << j_chargedEmEnergyFraction<< endl;
  cout << "j_muonEnergyFraction " << j_muonEnergyFraction<< endl;
  cout << "j_chargedMultiplicity " << j_chargedMultiplicity<< endl;
  cout << "j_neutralMultiplicity " << j_neutralMultiplicity<< endl;
  cout << "j_PileupJetId " << j_PileupJetId<< endl;
  cout << "j_jec_l1 " << j_jec_l1<< endl;
  cout << "j_jec_full " << j_jec_full<< endl;
  cout << "j_En_up " << j_En_up<< endl;
  cout << "j_En_down " << j_En_down<< endl;
  cout << "j_Res " << j_Res  << endl;
  cout << "j_Res_up " << j_Res_up << endl;
  cout << "j_Res_down " << j_Res_down << endl;
  cout << "j_bJetNN_corr " << j_bJetNN_corr<< endl;
  cout << "j_bJetNN_res " << j_bJetNN_res<< endl;
  cout << "j_cJetNN_corr " << j_cJetNN_corr<< endl;
  cout << "j_cJetNN_res " << j_cJetNN_res<< endl;
  cout << "j_nvtx_tracks " << j_nvtx_tracks<< endl;
}

Jet::~Jet(){

}

void Jet::SetArea(double area){
  j_area = area;
}

void Jet::SetJEC(double l1, double full){
  j_jec_l1 = l1;
  j_jec_full = full;
}
void Jet::SetGenFlavours(int pf, int hf){
  j_partonFlavour = pf;
  j_hadronFlavour = hf;
}
void Jet::SetGenHFHadronMatcher(int flavour, int origin){
  j_GenHFHadronMatcher_flavour = flavour;
  j_GenHFHadronMatcher_origin = origin; 
}
void Jet::SetTaggerResults(std::vector<double> ds){
  j_DeepCSV           = ds.at(0);
  j_DeepCSV_CvsL      = ds.at(1);
  j_DeepCSV_CvsB      = ds.at(2);
  j_DeepJet       = ds.at(3);
  j_DeepJet_CvsL  = ds.at(4);
  j_DeepJet_CvsB  = ds.at(5);
}
void Jet::SetEnergyFractions(double cH, double nH, double nEM, double cEM, double muE){
  j_chargedHadronEnergyFraction = cH;
  j_neutralHadronEnergyFraction = nH;
  j_neutralEmEnergyFraction = nEM;
  j_chargedEmEnergyFraction = cEM;
  j_muonEnergyFraction = muE;
}
void Jet::SetMultiplicities(double cM, double nM){
  j_chargedMultiplicity = cM;
  j_neutralMultiplicity = nM;
}
void Jet::SetPileupJetId(double v){
  j_PileupJetId = v;
}

void Jet::SetNTracks(double d){
  j_nvtx_tracks = d;
}


void Jet::SetEnShift(double en_up, double en_down){
  j_En_up = en_up;
  j_En_down = en_down;
}

void Jet::SetPxUnSmeared(double px){
  j_PxUnSmeared=px;
}

void Jet::SetPyUnSmeared(double py){
  j_PyUnSmeared=py;
}

void Jet::SetRes(double res){
  j_Res = res;

}


void Jet::SetResShift(double res_up, double res_down){
  j_Res_up = res_up;
  j_Res_down = res_down;
}
void Jet::SetBJetNNCorrection(double bJetNN_corr, double bJetNN_res){
  j_bJetNN_corr = bJetNN_corr;
  j_bJetNN_res = bJetNN_res;
}
void Jet::SetCJetNNCorrection(double cJetNN_corr, double cJetNN_res){
  j_cJetNN_corr = cJetNN_corr;
  j_cJetNN_res = cJetNN_res;
}
void Jet::SetTightJetID(double b){
  j_tightJetID = b;
}
void Jet::SetTightLepVetoJetID(double b){
  j_tightLepVetoJetID = b;
}

bool Jet::PassID(TString ID) const {


  if(ID=="NoID") return true;
  if(ID=="tight") return Pass_tightJetID();
  if(ID=="tightLepVeto") return Pass_tightLepVetoJetID();
  if(ID=="HNTight") return Pass_HNTight();
  if(ID=="Tight") return Pass_tightJetID();



  cout << "[Jet::PassID] No id : " << ID << endl;
  exit(ENODATA);

  return false;

}


bool Jet::Pass_HNTight() const {
  if(!Pass_tightJetID()) return false;

  return true;
}

double Jet::GetTaggerResult(JetTagging::Tagger tg) const {

  if(tg==JetTagging::DeepCSV) return j_DeepCSV;
  else if(tg==JetTagging::DeepCSV_CvsL) return j_DeepCSV_CvsL;
  else if(tg==JetTagging::DeepCSV_CvsB) return j_DeepCSV_CvsB;
  else if(tg==JetTagging::DeepJet) return j_DeepJet;
  else if(tg==JetTagging::DeepJet_CvsL) return j_DeepJet_CvsL;
  else if(tg==JetTagging::DeepJet_CvsB) return j_DeepJet_CvsB;
  else{
    cout << "[Jet::GetTaggerResult] ERROR; Wrong tagger : " << tg << endl;
    return -999;
  }
}

bool Jet::PassPileupMVA(TString WP, TString Era) const {

  //https://twiki.cern.ch/twiki/bin/view/CMS/PileupJetIDUL                                                                                                                                                                                                                                                                                   

  bool pass=false;

  if (Era.Contains("2016")){
    if(WP=="Tight"){
      if(fabs(this->Eta()) < 2.5){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > 0.71) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > 0.87) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > 0.94) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > 0.97) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 2.75){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.32) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.08) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > 0.24) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > 0.48) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 3.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.30) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.16) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() >  0.05) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() >  0.26) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 5.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.22) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.12) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() >  0.10) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() >  0.29) pass=true; }
        else pass=true;
      }
      else pass=true;
    }
    else if(WP=="Medium"){
      if(fabs(this->Eta()) < 2.5){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > 0.20) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > 0.62) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > 0.86) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > 0.93) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 2.75){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.56) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.39) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.10) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() >  0.19) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 3.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.43) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.32) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.15) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() >  0.04) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 5.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.38) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.29) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.08) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() >  0.12) pass=true; }
        else pass=true;
      }
      else pass=true;
    }
    else if(WP=="Loose"){
      if(fabs(this->Eta()) < 2.5){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.95) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.90) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.71) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > -0.42) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 2.75){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.70) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.57) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.36) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > -0.09) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 3.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.52) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.43) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.29) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > -0.14) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 5.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.49) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.42) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.23) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > -0.02) pass=true; }
        else pass=true;
      }
      else pass=true;
    }
    else{
      cout << "[Jet::PassPileupID] No wp : " << WP << endl;
      exit(EXIT_FAILURE);
    }
  }

  //2017                                                                                                                                                                                                                                                                                                                                      

  else if (Era.Contains("2017")){

    if(WP=="Tight"){
      if(fabs(this->Eta()) < 2.5){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > 0.77) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > 0.90) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > 0.96) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > 0.98) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 2.75){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > 0.38) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > 0.60) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > 0.82) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > 0.92) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 3.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.31) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.12) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() >  0.20) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() >  0.47) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 5.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.21) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.13) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() >  0.09) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() >  0.29) pass=true; }
        else pass=true;
      }
      else pass=true;
    }
    else if(WP=="Medium"){
      if(fabs(this->Eta()) < 2.5){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > 0.26) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > 0.68) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > 0.90) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > 0.96) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 2.75){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.33) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.04) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() >  0.36) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() >  0.61) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 3.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.54) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.43) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.16) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() >  0.14) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 5.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.37) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.30) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.09) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() >  0.12) pass=true; }
        else pass=true;
      }
      else pass=true;
    }
    else if(WP=="Loose"){
      if(fabs(this->Eta()) < 2.5){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.95) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.88) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.63) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > -0.19) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 2.75){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.72) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.55) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.18) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() >  0.22) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 3.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.68) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.60) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.43) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > -0.13) pass=true; }
        else pass=true;
      }
      else if(fabs(this->Eta()) < 5.0){
        if(this->Pt() < 10.){ pass=true;}
        else if(this->Pt() < 20.){ if(PileupJetId() > -0.47) pass=true; }
        else if(this->Pt() < 30.){ if(PileupJetId() > -0.43) pass=true; }
        else if(this->Pt() < 40.){ if(PileupJetId() > -0.24) pass=true; }
        else if(this->Pt() < 50.){ if(PileupJetId() > -0.03) pass=true; }
        else pass=true;
      }
      else pass=true;
    }
    else{
      cout << "[Jet::PassPileupID] No wp : " << WP << endl;
      exit(EXIT_FAILURE);
    }
  }
  
  else if (Era.Contains("2018")){
    if(WP=="Tight"){
      if(fabs(this->Eta()) < 2.5){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > 0.77) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > 0.90) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() > 0.96) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() > 0.98) pass=true; }
	else pass=true;
      }
      else if(fabs(this->Eta()) < 2.75){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > 0.38) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > 0.60) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() > 0.82) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() > 0.92) pass=true; }
	else pass=true;
      }
      else if(fabs(this->Eta()) < 3.0){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > -0.31) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > -0.12) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() >  0.20) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() >  0.47) pass=true; }
	else pass=true;
      }
      else if(fabs(this->Eta()) < 5.0){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > -0.21) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > -0.13) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() >  0.09) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() >  0.29) pass=true; }
	else pass=true;
      }
      else pass=true;
    }
    else if(WP=="Medium"){
      if(fabs(this->Eta()) < 2.5){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > 0.26) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > 0.68) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() > 0.90) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() > 0.96) pass=true; }
	else pass=true;
      }
      else if(fabs(this->Eta()) < 2.75){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > -0.33) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > -0.04) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() >  0.36) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() >  0.61) pass=true; }
	else pass=true;
      }
      else if(fabs(this->Eta()) < 3.0){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > -0.54) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > -0.43) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() > -0.16) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() >  0.14) pass=true; }
	else pass=true;
      }
      else if(fabs(this->Eta()) < 5.0){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > -0.37) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > -0.30) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() > -0.09) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() >  0.12) pass=true; }
	else pass=true;
      }
      else pass=true;
    }
    else if(WP=="Loose"){
      if(fabs(this->Eta()) < 2.5){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > -0.95) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > -0.88) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() > -0.63) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() > -0.19) pass=true; }
	else pass=true;
      }
      else if(fabs(this->Eta()) < 2.75){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > -0.72) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > -0.55) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() > -0.18) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() >  0.22) pass=true; }
	else pass=true;
      }
      else if(fabs(this->Eta()) < 3.0){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > -0.68) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > -0.60) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() > -0.43) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() > -0.13) pass=true; }
	else pass=true;
      }
      else if(fabs(this->Eta()) < 5.0){
	if(this->Pt() < 10.){ pass=true;}
	else if(this->Pt() < 20.){ if(PileupJetId() > -0.47) pass=true; }
	else if(this->Pt() < 30.){ if(PileupJetId() > -0.43) pass=true; }
	else if(this->Pt() < 40.){ if(PileupJetId() > -0.24) pass=true; }
	else if(this->Pt() < 50.){ if(PileupJetId() > -0.03) pass=true; }
	else pass=true;
      }
      else pass=true;
    }
    else{
      cout << "[Jet::PassPileupID] No wp : " << WP << endl;
      exit(EXIT_FAILURE);
    }
  }
  
  
  return pass;
}

