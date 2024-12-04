#include "Muon.h"
#include <iostream>
#include <sstream>
#include <string>

ClassImp(Muon)

Muon::Muon() : Lepton() {

  j_chi2 = 999.;
  j_PFCH04 = -999.;
  j_PFNH04 = -999.;
  j_PFPH04 = -999.;
  j_PU04 = -999.;
  this->SetLeptonFlavour(MUON);
  j_MiniAODPt = -999.;
  j_MomentumScaleUp = -999.;
  j_MomentumScaleDown = -999.;
  j_TunePPtError = -999.;
  j_MVA = -999.;
  j_lowptMVA = -999.;
  j_softMVA = -999.;
  j_validmuonhits = 0;
  j_matchedstations = 0;
  j_pixelHits = 0;
  j_trackerLayers = 0;
  j_museg_comp=0;
}


Muon::~Muon(){
}

void Muon::SetMuonSegmentCompatibility(double segcomp){
  j_museg_comp = segcomp;
}


void Muon::SetTypeBit(unsigned int typebit){
  j_TypeBit = typebit;
}

void Muon::SetIDBit(unsigned int idbit){
  j_IDBit = idbit;
}

void Muon::SetisPOGHighPt(bool b){
  j_isPOGHighPt = b;
}


void Muon::SetPOGMediumHIP(bool ismedium_hip, bool ismedium_nohip){
  j_ismedium_hip = ismedium_hip;
  j_ismedium_nohip = ismedium_nohip;
}
void Muon::SetIso(double ch04, double nh04, double ph04, double pu04){
  j_PFCH04 = ch04;
  j_PFNH04 = nh04;
  j_PFPH04 = ph04;
  j_PU04 = pu04;
  CalcPFRelIso();
}

void Muon::SetChi2(double chi2){
  j_chi2 = chi2;
}

void Muon::CalcPFRelIso(bool use_corrected_pt){
  double absiso = j_PFCH04+std::max( 0., j_PFNH04 + j_PFPH04 - 0.5*j_PU04 );

  if(use_corrected_pt)this->SetRelIso(absiso/this->Pt());
  else this->SetRelIso(absiso/this->MiniAODPt());

}


double Muon::EA(){

  double eta = fabs(this->Eta());

  if     (eta<0.8000) return 0.0566;
  else if(eta<1.3000) return 0.0562;
  else if(eta<2.0000) return 0.0363;
  else if(eta<2.2000) return 0.0119;
  else if(eta<2.4000) return 0.0064;
  else return 0.0064;

}

void Muon::SetMiniAODPt(double d){
  j_MiniAODPt = d;
}
void Muon::SetMiniAODTunePPt(double d){
  j_MiniAODTunePPt = d;
}

void Muon::SetMomentumScaleUpDown(double pt_up, double pt_down){
  j_MomentumScaleUp = pt_up;
  j_MomentumScaleDown = pt_down;
}

void Muon::SetTuneP4(double pt, double pt_err, double eta, double phi, double q){
  j_TuneP4.SetPtEtaPhiM(pt,eta,phi,M());
  j_TuneP4.SetCharge(q);
  j_TunePPtError = pt_err;
}

void Muon::SetMVA(double MVA){
  j_MVA = MVA;                                                                                                                                                                                       
}

void Muon::SetSoftMVA(double MVA){
  j_softMVA = MVA;
}




bool Muon::PassID(TString ID) const {

  double MVACut = -1;
  TString Year = this->sRun_Year();
  TString Era  = this->Run_Era();
  if(Year == "2016")  MVACut= mu_mva_cut_fake_2016;
  if(Year == "2017")  MVACut= mu_mva_cut_fake_2017;
  if(Year == "2018")  MVACut= mu_mva_cut_fake_2018;

  //  cout << "Year = " << Year << " Era = " << Era << " MVACut = " << MVACut << endl;

  ////////////// BASIC IDS

  /// ALL LEPTONS
  if(ID=="NoCut" || ID=="NOCUT") return true;

  //// LOOSE & VETO                                                                                                                                                                                              
  if(ID=="HNLoosest") return (Pass_LepMVAID() || Pass_HNVeto2016());
  if(ID=="HNVetoMVA") return Pass_HNVetoUL(); /// Clone of Pass_HNVeto2016 

  bool HLTCut = (TrkIso()/this->MiniAODPt()<0.4);
  bool dXY02  = (fabs(dXY()) < 0.2); 
  bool SIP10  = (fabs(IP3D()/IP3Derr())< 10 );
  bool RISO04 = ( RelIso()< 0.4 );
  bool dZ01   = (fabs(dZ())<0.1);
  /// looser IP                                                                                                                                                                                                  
  if(ID=="HNLooseV1")  return (isPOGLoose() && dXY02 && SIP10  && RISO04 && ( Chi2()<50. ) && HLTCut&& dZ01);
  if(ID=="HNLoosePOG") return (isPOGLoose() && dXY02 && RISO04 && ( Chi2()<50. ) && HLTCut&& dZ01);
  //// Probe ID for IDSF                                                                                                                                                                                         
  if(ID=="passProbe") return (isPOGLoose() && (this->fdXY() < 0.2) && (this->fdZ() < 0.5));


  if(ID=="POGTight")  return isPOGTight();
  if(ID=="POGHighPt") return isPOGHighPt();
  if(ID=="POGMedium") return isPOGMedium();
  if(ID=="POGLoose")  return isPOGLoose();
  if(ID=="Global")  return IsType(GlobalMuon);
  if(ID=="POGTightWithTightIso")     return Pass_POGTightWithTightIso();
  if(ID=="POGHighPtWithLooseTrkIso") return Pass_POGHighPtWithLooseTrkIso();
  if(ID=="POGHighPtWithVLooseTrkIso") return Pass_POGHighPtWithVLooseTrkIso();
  if(ID=="POGHighPtTight")    return Pass_POGHighPtTight();
  if(ID=="POGHighPtLoose")    return Pass_POGHighPtLoose();
  if(ID=="POGHighPtMixTight") return Pass_POGHighPtTightOR();
  if(ID=="POGHighPtMixLoose") return Pass_POGHighPtLooseOR();
  if(ID=="HNTightPFIsoLoose") return Pass_POGTightPFIsoLoose(true);
  if(ID=="HNTightPFIsoMedium")return Pass_POGTightPFIsoMedium(true);
  if(ID=="HNTightPFIsoTight") return Pass_POGTightPFIsoTight(true);
  if(ID=="HNTightPFIsoVeryTight")     return Pass_POGTightPFIsoVeryTight(true);
  if(ID=="HNTightPFIsoVeryVeryTight") return Pass_POGTightPFIsoVeryVeryTight(true);
  if(ID=="POGTightPFIsoLoose")  return Pass_POGTightPFIsoLoose(false);
  if(ID=="POGTightPFIsoMedium") return Pass_POGTightPFIsoMedium(false);
  if(ID=="POGTightPFIsoTight")  return Pass_POGTightPFIsoTight(false);
  if(ID=="POGTightPFIsoVeryTight")     return Pass_POGTightPFIsoVeryTight(false);
  if(ID=="POGTightPFIsoVeryVeryTight") return Pass_POGTightPFIsoVeryVeryTight(false);
  if(ID=="CutBasedTightNoIP") return Pass_CutBasedTightNoIP();

  if(ID=="POGMIDTIso")   return isPOGMedium() && RelIso()<0.15;
  if(ID=="POGMIDVVLIso") return isPOGMedium() && RelIso()<0.4;
  if(ID=="POGTIDTIso")   return isPOGTight() && RelIso()<0.15;
  if(ID=="POGTIDVVLIso") return isPOGTight() && RelIso()<0.4;
  if(ID=="POGIDMPrIsoM" ) return isPOGMedium() && fabs(dXY())<0.02 && fabs(dZ())<0.1 && RelIso()<0.2;
  if(ID=="POGIDMPrIsoVL") return isPOGMedium() && fabs(dXY())<0.02 && fabs(dZ())<0.1 && RelIso()<0.4;

  if(ID=="POGMHLT") return (isPOGMedium() && HLTCut && (fabs(dZ())<0.1));

  if(ID=="HNTightV1") return Pass_HNTight(0.07, 0.02, 0.05, 3.);
  if(ID=="HNTightV2") return (HLTCut && (fabs(dZ())<0.1) && Pass_HNTight(0.07, 0.05, 0.1, 3.));
  /// OTHER GROUP IDs


  if(ID=="PekingNoIso") {
    if(!isPOGTight())       return false;

    double dxy_cut = (this->Pt() < 20) ? 0.01 : 0.02 ;
    if(fabs(dXY()) >  dxy_cut) return false;
    if(fabs(dZ()) >  0.1)      return false;
    return true;
  }

  if(ID=="Peking") {
    if(!isPOGTight())       return false;
    if(!( RelIso()<0.15 ))  return false;
    double dxy_cut = (this->Pt() < 20) ? 0.01 : 0.02 ;
    if(fabs(dXY()) >  dxy_cut) return false;
    if(fabs(dZ()) >  0.1)      return false;
    return true;
  }
  if(ID=="Peking_FO") {
    if(!isPOGLoose())       return false;
    if(!( RelIso()<0.4 ))  return false;
    double dxy_cut = (this->Pt() < 20) ? 0.01 : 0.02 ;
    if(fabs(dXY()) >  dxy_cut) return false;
    if(fabs(dZ()) >  0.1)      return false;
    return true;
  }

  ///// EXO-17028                                                                                                                                                                                                   
  if(ID=="HNVeto_17028")  return Pass_HNVeto2016();
  if(ID=="HNLoose_17028") return Pass_HNLoose2016(0.4, 0.2, 0.1, 3.);
  if(ID=="HNTight_17028") return Pass_HNTight2016();


  /////////////////////////
  //#######################
  //#######################
  //## MVA IDS
  //#######################
  //#######################
  /////////////////////////                                                                                                                                                                                         

  if(ID=="HNL_HN3L" || ID == "HNL_TopMVA_MM") return (PassID("MVALoose") && (MVA() >= 0.64));
  
  if(ID=="HNL_TopMVA_FO_MM") {
    if(!PassID("MVALoose")) return false;
    if(MVA() < 0.64) {
      if(CloseJet_Ptratio() < 0.45) return false;
      if(CloseJet_BScore() > 0.025) return false;
    }
    return true;
  }

  if(ID=="TopHN"){
    if(! isPOGMedium()        ) return false;
    if(! (MiniRelIso()<0.1)) return false;
    if(! HLTCut ) return false;
    if(! (fabs(dZ())<0.1)  ) return false;
    if(! (SIP3D()<3.) ) return false;
    return true;
  }
  if(ID=="TopHNL"){
    if(! isPOGMedium()       ) return false;
    if(! (MiniRelIso()<0.6)  ) return false;
    if(! HLTCut ) return false;
    if(! (SIP3D()<5)         ) return false;
    if(! (fabs(dZ())<0.1)    ) return false;
    return true;
  }

  /// MVAID used to train 
  if(ID.Contains("MuOpt"))    return Pass_MultiFunction_Opt(ID);


  if(ID == "MVAID")             return Pass_LepMVAID();
  if(ID == "HNL_ULID_Baseline") return Pass_LepMVAID();
  if(ID == "MVALoose")          return (Pass_LepMVAID() && isPOGMedium()); /// Medioum cut removes LF peak with 0 FR

  /// Loose ID for SR with MVA cuts
  if(ID == "MVALooseTrgSafe")       return (PassID("MVALoose") && HLTCut);

  //// Transfer Factor ID


  if(ID == "HNL_ULID_SB_"+Era ) {
    if(!PassID("HNL_ULID_FO_LOOSE"))  return false;
    if((MVA() >  MVACut))  return false;
    if((MVA() <  -0.2))    return false;
    return true;
  }

  /////////// FINAL UL HNL Type-1 ID                                                                                                                                                                               
  if(ID == "HNL_ULID_FO_LOOSE")           return (PassID("MVALooseTrgSafe")        && (fabs(IP3D()/IP3Derr()) < 7)); 
  
  
  //// Optimised Loose IDs
  if(Era=="2016a"){
    if(ID == "HNL_ULID_FO")        return PassID("HNL_ULID_FO_v1_a");
    if(ID == "HNL_ULID_FO_Up")     return PassID("HNL_ULID_FO_v1_a");
    if(ID == "HNL_ULID_FO_Down")   return PassID("HNL_ULID_FO_v2_a");
  }
  if(Era=="2016b"){
    if(ID == "HNL_ULID_FO")        return PassID("HNL_ULID_FO_v2_a");
    if(ID == "HNL_ULID_FO_Up")     return PassID("HNL_ULID_FO_v1_a");
    if(ID == "HNL_ULID_FO_Down")   return PassID("HNL_ULID_FO_v3_a");

  }
  if(Era=="2017"){
    if(ID == "HNL_ULID_FO")        return PassID("HNL_ULID_FO_v2_a");
    if(ID == "HNL_ULID_FO_Up")     return PassID("HNL_ULID_FO_v1_a");
    if(ID == "HNL_ULID_FO_Down")   return PassID("HNL_ULID_FO_v3_a");
  }
  if(Era=="2018"){
    if(ID == "HNL_ULID_FO")        return PassID("HNL_ULID_FO_v3_a");
    if(ID == "HNL_ULID_FO_Up")     return PassID("HNL_ULID_FO_v1_a");
    if(ID == "HNL_ULID_FO_Down")   return PassID("HNL_ULID_FO_v4_a");
  }

  ///// DeepJet Variations in Loose v0-9
  if(ID == "HNL_ULID_FO_v0"){
    if(!PassID("HNL_ULID_FO_LOOSE")) return false;
    if(MVA() < MVACut) {
      if(this->HasCloseJet()){
        if(CloseJet_Ptratio() < 0.45)         return false;
      }
    }
    return true;
  }
 
  if(ID.Contains("HNL_ULID_FO_v")){

    double BJetDeepJetCut = 999;
    if(ID.Contains("HNL_ULID_FO_v1")) BJetDeepJetCut = 0.025;   //// Version 1 == TOP
    if(ID.Contains("HNL_ULID_FO_v2")) BJetDeepJetCut = 0.05;
    if(ID.Contains("HNL_ULID_FO_v3")) BJetDeepJetCut = 0.1;
    if(ID.Contains("HNL_ULID_FO_v4")) BJetDeepJetCut = 0.2;
    if(ID.Contains("HNL_ULID_FO_v5")) BJetDeepJetCut = 0.3;
    if(ID.Contains("HNL_ULID_FO_v6")) BJetDeepJetCut = 0.4;
    if(ID.Contains("HNL_ULID_FO_v7")) BJetDeepJetCut = 0.5;
    if(ID.Contains("HNL_ULID_FO_v8")) BJetDeepJetCut = 0.75;
    if(ID.Contains("HNL_ULID_FO_v9")) BJetDeepJetCut = 0.9;
    bool ApplyCloseJet_CvsBScore=false;
    bool ApplyCloseJet_CvsLScore=false;
    double CBJetDeepJetCut = -999;
    double CLJetDeepJetCut = 999;
    
    if(ID.Contains("_b")){
      ApplyCloseJet_CvsBScore = true;
      ApplyCloseJet_CvsLScore= true;
      CBJetDeepJetCut=0.1;
      CLJetDeepJetCut=0.6;
    }
    if(ID.Contains("_c")){
      ApplyCloseJet_CvsBScore = true;
      ApplyCloseJet_CvsLScore= true;
      CBJetDeepJetCut=0.1;
      CLJetDeepJetCut=0.8;
    }
    
    if(!PassID("HNL_ULID_FO_LOOSE")) return false;
    if(MVA() < MVACut) {
      if(this->HasCloseJet()){
        if(CloseJet_Ptratio() < 0.45)         return false;
        if(this->CloseJet_BScore()    >  BJetDeepJetCut) return false;
        if(ApplyCloseJet_CvsBScore && this->CloseJet_CvsBScore() <  CBJetDeepJetCut ) return false;
        if(ApplyCloseJet_CvsLScore && this->CloseJet_CvsLScore() >  CLJetDeepJetCut ) return false;
      }
    }
    return true;
  }

  
  if(ID == "HNL_ULID")        return (PassID("HNL_ULID_FO_LOOSE") && (MVA() >  MVACut));
  if(ID == "HNL_ULID_"+Year)  return (PassID("HNL_ULID_FO_LOOSE") && (MVA() >  MVACut));

  //// ISDF functions to check several WP for each MVA                                                                                                                                                                                                                                                                                                     



  cout << "[Muon::PassID] No id : " << ID << endl;
  exit(ENODATA);

  return false;

}

bool Muon::Pass_LepMVAID() const {

  if(this->MiniAODPt() < 5)     return false;
  if(this->fEta() > 2.4)  return false;
  if(MiniRelIso() > 0.4)  return false;
  if(SIP3D() > 8)         return false;
  if(this->fdXY() > 0.05) return false;
  if(this->fdZ() > 0.1)   return false;
  if(!( isPOGLoose() ))   return false;
  return true;
}

bool Muon::PassSoftMVA(double mva1, double mva2, double mva3) const {

  if( fabs(this->Eta()) <= 0.8 ){
    if(! (softMVA()>mva1) ) return false;
  }
  else if( fabs(this->Eta()) > 0.8 && fabs(this->Eta()) <= 1.479 ){
    if(! (softMVA()>mva2) ) return false;
  }
  else{
    if(! (softMVA()>mva3) ) return false;
  }

  return true;
}



bool Muon::Pass_POGTightWithTightIso() const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.15 ))  return false;
  return true;
}

bool Muon::Pass_POGHighPtLooseOR() const {
  if(Pass_HNLoose(0.4, 0.2, 0.5, 10.)) return true;
  if(Pass_POGHighPtLoose()) return true;
  return false;
}

bool Muon::Pass_POGHighPtTightOR() const {
  if(Pass_POGTightPFIsoVeryTight(true)) return true;
  if(Pass_POGHighPtTight())  return true;
  return false;
}



bool Muon::Pass_POGHighPtLoose() const {
  //if(this->Pt() < 60) return false;
  if(!( isPOGLoose() )) return false;
  if(!( IsType(GlobalMuon) )) return false;
  if(!( TrkIso()/TuneP4().Pt()<0.3 )) return false;
  return true;
}

bool Muon::Pass_POGHighPtTight() const {
  //if(this->Pt() < 60) return false;
  if(!( isPOGHighPt() )) return false;
  if(!( TrkIso()/TuneP4().Pt()<0.1 )) return false;
  return true;
}


bool Muon::Pass_POGHighPtWithLooseTrkIso() const {
  if(!( isPOGHighPt() )) return false;
  if(!( TrkIso()/TuneP4().Pt()<0.1 )) return false;
  return true;
}


bool Muon::Pass_POGHighPtWithVLooseTrkIso() const {
  if(!( isPOGHighPt() )) return false;
  if(!( TrkIso()/TuneP4().Pt()<0.45 )) return false;
  return true;
}


bool Muon::Pass_HNVeto2016() const {
  //  if(Pass_POGHighPtTight()) return true;
  if(!( isPOGLoose() )) return false;
  if(!( fabs(dXY())<0.2 && fabs(dZ())<0.5) ) return false;
  if(MiniRelIso() > 0.4) return false; /// chnage to be consistent with loose mva 
  //if(!( RelIso()<0.6 ))  return false;
  if(!( Chi2()<50. )) return false;
  return true;
}

bool Muon::Pass_HNVetoUL() const {
  if(!( isPOGLoose() )) return false;
  if(!( fabs(dXY())<0.2 && fabs(dZ())<0.5) ) return false;
  if(MiniRelIso() > 0.4) return false; /// chnage to be consistent with loose mva                                                                                                                                 
  if(!( Chi2()<50. )) return false;
  return true;
}


bool Muon::Pass_HNLoose2016(double relisoCut, double dxyCut, double dzCut, double sipCut) const {

  if(!( isPOGLoose() )) return false;
  if(!( fabs(dXY())<dxyCut && fabs(dZ())<dzCut && fabs(IP3D()/IP3Derr())<sipCut) ) return false;
  if(!( RelIso()<relisoCut ))  return false;
  if(!( Chi2()<50. )) return false;
  return true;
}

bool Muon::Pass_HNTight2016() const {
  if(!( isPOGTight() )) return false;
  if(!( fabs(dXY())<0.005 && fabs(dZ())<0.04 && fabs(IP3D()/IP3Derr())<3.) ) return false;
  if(!( RelIso()<0.07 ))  return false;
  if(!( Chi2()<10. )) return false;
  return true;
}


bool Muon::Pass_HNLoose(double relisoCut, double dxyCut, double dzCut,double sipCut) const {
  // Individual cuts of the POG cut-based ID
  if(!( isPOGLoose() )) return false;
  //if(!( IsType(GlobalMuon) )) return false;
  if(!( fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
  if(!(fabs(IP3D()/IP3Derr())<sipCut )) return false;
  if(!( RelIso()<relisoCut )) return false;
  if(!( Chi2()<50. )) return false;
  
  return true;
}

bool Muon::Pass_HNTight(double relisoCut, double dxyCut, double dzCut,double sipCut) const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<relisoCut )) return false;
  if(!( fabs(dXY())<dxyCut && fabs(dZ())<dzCut &&fabs(IP3D()/IP3Derr())<sipCut )) return false;
  return true;
}

bool Muon::Pass_ISRLoose(double relisoCut) const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<relisoCut )) return false;
  return true;
}

bool Muon::Pass_ISRTight() const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.15 )) return false;
  return true;
}

//==== TEST ID

bool Muon::Pass_TESTID() const {
  return true;
}

bool Muon::Pass_POGTightRelIso25() const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.25 )) return false;
  return true;
}

bool Muon::Pass_POGTightRelIso20() const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.2 )) return false;
  return true;
}

bool Muon::Pass_POGTightRelIso15() const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.15 )) return false;
  return true;
}

bool Muon::Pass_POGTightRelIso10() const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.1 )) return false;
  return true;
}

bool Muon::Pass_POGTightPFIsoLoose(bool applyIP) const {
  if(!( isPOGTight() )) return false;
  if(!( PassSelector(PFIsoLoose) )) return false;
  if(applyIP) {
    if(!( fabs(dXY())< 0.02 && fabs(dZ())<0.05)) return false;

  }
  return true;
}

bool Muon::Pass_POGTightPFIsoMedium(bool applyIP) const {
  if(!( isPOGTight() )) return false;
  if(!( PassSelector(PFIsoMedium) )) return false;
  if(applyIP) {
    if(!( fabs(dXY())< 0.02 && fabs(dZ())<0.05 )) return false;

  }
  return true;
}

bool Muon::Pass_POGTightPFIsoTight(bool applyIP) const {
  if(!( isPOGTight() )) return false;
  if(!( PassSelector(PFIsoTight) )) return false;
  if(applyIP) {
    if(!( fabs(dXY())< 0.02 && fabs(dZ())<0.05)) return false;

  }
  return true;
}

bool Muon::Pass_POGTightPFIsoVeryTight(bool applyIP) const {
  if(!( isPOGTight() )) return false;
  if(!( PassSelector(PFIsoVeryTight) )) return false;
  if(applyIP) {
    if(!( fabs(dXY())< 0.02 && fabs(dZ())<0.05)) return false;
  }
  return true;
}

bool Muon::Pass_POGTightPFIsoVeryVeryTight(bool applyIP) const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.05 )) return false;
  if(applyIP) {
    if(!( fabs(dXY())< 0.02 && fabs(dZ())<0.05)) return false;
  }
  return true;
}

bool Muon::Pass_POGTightCutsWithTightIso() const {  // This gives the same result with the result using POGTightWithTightIso
  if(!( isPOGLoose() )) return false;
  if(!( IsType(GlobalMuon) )) return false;
  if(!( Chi2()<10. )) return false;
  if(!( ValidMuonHits()>0 )) return false;
  if(!( MatchedStations()>1 )) return false;
  if(!( fabs(dXY())<0.2 && fabs(dZ())<0.5) ) return false;
  if(!( PixelHits()>0 )) return false;
  if(!( TrackerLayers()>5 )) return false;
  if(!( RelIso()<0.15 )) return false;
  return true;
}

//==== N-1 POG Tight
bool Muon::Pass_CutBasedTightNoIP() const {

  bool debug=false;
  if(!( isPOGLoose() )) {if(debug)cout << "Fail isPOGLoose" << endl; return false;}
  if(!( IsType(GlobalMuon) )) {if(debug)cout << "Fail GlobalMuon" << endl; return false;}
  if(!( Chi2()<10. )) {if(debug)cout << "Fail Chi2" << endl; return false;}
  if(!( ValidMuonHits()>0 )) {if(debug)cout << isPOGTight() << "  Fail ValidMuonHits" << endl; return false;}
  if(!( MatchedStations()>1 )) {if(debug)cout << "Fail MatchedStations" << endl; return false;}
  if(!( PixelHits()>0 )) {if(debug)cout << "Fail PixelHits" << endl; return false;}
  if(!( TrackerLayers()>5 )) {if(debug)cout << "Fail TrackerLayers" << endl; return false;}
  return true;
}


void Muon::SetValidMuonHits(int n){
  j_validmuonhits = n;
}

void Muon::SetMatchedStations(int n){
  j_matchedstations = n;
}

void Muon::SetPixelHits(int n){
  j_pixelHits = n;
}

void Muon::SetTrackerLayers(int n){
  j_trackerLayers = n;
}



bool Muon::PassFilter(TString filter) const{
  if( filter=="hltDiMu9Ele9CaloIdLTrackIdLMuonlegL3Filtered9" ) return j_filterbits&(ULong64_t(1)<<0);
  else if( filter=="hltDiMuon178Mass3p8Filtered" ) return j_filterbits&(ULong64_t(1)<<1);
  else if( filter=="hltDiMuon178Mass8Filtered" ) return j_filterbits&(ULong64_t(1)<<2);
  else if( filter=="hltDiMuon178RelTrkIsoFiltered0p4" ) return j_filterbits&(ULong64_t(1)<<3);
  else if( filter=="hltDiMuon178RelTrkIsoFiltered0p4DzFiltered0p2" ) return j_filterbits&(ULong64_t(1)<<4);
  else if( filter=="hltDiMuonGlb17Glb8DzFiltered0p2" ) return j_filterbits&(ULong64_t(1)<<5);
  else if( filter=="hltDiMuonGlb17Glb8RelTrkIsoFiltered0p4" ) return j_filterbits&(ULong64_t(1)<<6);
  else if( filter=="hltDiMuonGlb17Glb8RelTrkIsoFiltered0p4DzFiltered0p2" ) return j_filterbits&(ULong64_t(1)<<7);
  else if( filter=="hltDiMuonGlb17Trk8RelTrkIsoFiltered0p4" ) return j_filterbits&(ULong64_t(1)<<8);
  else if( filter=="hltDiMuonGlb17Trk8RelTrkIsoFiltered0p4DzFiltered0p2" ) return j_filterbits&(ULong64_t(1)<<9);
  else if( filter=="hltDiMuonGlbFiltered17TrkFiltered8" ) return j_filterbits&(ULong64_t(1)<<10);
  else if( filter=="hltDiMuonTrk17Trk8RelTrkIsoFiltered0p4" ) return j_filterbits&(ULong64_t(1)<<11);
  else if( filter=="hltDiMuonTrk17Trk8RelTrkIsoFiltered0p4DzFiltered0p2" ) return j_filterbits&(ULong64_t(1)<<12);
  else if( filter=="hltDiTkMuonTkFiltered17TkFiltered8" ) return j_filterbits&(ULong64_t(1)<<13);
  else if( filter=="hltL1TripleMu553L2TriMuFiltered3L3TriMuFiltered10105" ) return j_filterbits&(ULong64_t(1)<<14);
  else if( filter=="hltL1TripleMu553L2TriMuFiltered3L3TriMuFiltered5" ) return j_filterbits&(ULong64_t(1)<<15);
  else if( filter=="hltL3crIsoL1sMu22L1f0L2f10QL3f24QL3trkIsoFiltered0p09" ) return j_filterbits&(ULong64_t(1)<<16);
  else if( filter=="hltL3crIsoL1sMu22Or25L1f0L2f10QL3f27QL3trkIsoFiltered0p07" ) return j_filterbits&(ULong64_t(1)<<17);
  else if( filter=="hltL3crIsoL1sSingleMu22L1f0L2f10QL3f24QL3trkIsoFiltered0p07" ) return j_filterbits&(ULong64_t(1)<<18);
  else if( filter=="hltL3fL1DoubleMu155fFiltered17" ) return j_filterbits&(ULong64_t(1)<<19);
  else if( filter=="hltL3fL1DoubleMu155fPreFiltered8" ) return j_filterbits&(ULong64_t(1)<<20);
  else if( filter=="hltL3fL1DoubleMu157fFiltered18" ) return j_filterbits&(ULong64_t(1)<<21);
  else if( filter=="hltL3fL1DoubleMu157fFiltered9" ) return j_filterbits&(ULong64_t(1)<<22);
  else if( filter=="hltL3fL1DoubleMu7EG7f0Filtered9" ) return j_filterbits&(ULong64_t(1)<<23);
  else if( filter=="hltL3fL1Mu6DoubleEG10f0Filtered8" ) return j_filterbits&(ULong64_t(1)<<24);
  else if( filter=="hltL3fL1TripleMu553f0Filtered10105" ) return j_filterbits&(ULong64_t(1)<<25);
  else if( filter=="hltL3fL1TripleMu553f0Filtered1055" ) return j_filterbits&(ULong64_t(1)<<26);
  else if( filter=="hltL3fL1TripleMu553f0PreFiltered555" ) return j_filterbits&(ULong64_t(1)<<27);
  else if( filter=="hltL3fL1sDoubleMu114L1f0L2f10L3Filtered17" ) return j_filterbits&(ULong64_t(1)<<28);
  else if( filter=="hltL3fL1sDoubleMu114L1f0L2f10OneMuL3Filtered17" ) return j_filterbits&(ULong64_t(1)<<29);
  else if( filter=="hltL3fL1sDoubleMu114TkFiltered17Q" ) return j_filterbits&(ULong64_t(1)<<30);
  else if( filter=="hltL3fL1sMu10lqL1f0L2f10L3Filtered17" ) return j_filterbits&(ULong64_t(1)<<31);
  else if( filter=="hltL3fL1sMu15DQlqL1f0L2f10L3Filtered17" ) return j_filterbits&(ULong64_t(1)<<32);
  else if( filter=="hltL3fL1sMu1lqL1f0L2f10L3Filtered17TkIsoFiltered0p4" ) return j_filterbits&(ULong64_t(1)<<33);
  else if( filter=="hltL3fL1sMu22L1f0Tkf24QL3trkIsoFiltered0p09" ) return j_filterbits&(ULong64_t(1)<<34);
  else if( filter=="hltL3fL1sMu5L1f0L2f5L3Filtered8" ) return j_filterbits&(ULong64_t(1)<<35);
  else if( filter=="hltL3fL1sMu5L1f0L2f5L3Filtered8TkIsoFiltered0p4" ) return j_filterbits&(ULong64_t(1)<<36);
  else if( filter=="hltL3pfL1sDoubleMu114L1f0L2pf0L3PreFiltered8" ) return j_filterbits&(ULong64_t(1)<<37);
  else if( filter=="hltL3pfL1sDoubleMu114ORDoubleMu125L1f0L2pf0L3PreFiltered8" ) return j_filterbits&(ULong64_t(1)<<38);
  else if( filter=="hltMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLDZFilter" ) return j_filterbits&(ULong64_t(1)<<39);
  else if( filter=="hltMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLMuonlegL3IsoFiltered23" ) return j_filterbits&(ULong64_t(1)<<40);
  else if( filter=="hltMu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVLMuonlegL3IsoFiltered23" ) return j_filterbits&(ULong64_t(1)<<41);
  else if( filter=="hltMu8DiEle12CaloIdLTrackIdLMuonlegL3Filtered8" ) return j_filterbits&(ULong64_t(1)<<42);
  else if( filter=="hltMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLDZFilter" ) return j_filterbits&(ULong64_t(1)<<43);
  else if( filter=="hltMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLMuonlegL3IsoFiltered8" ) return j_filterbits&(ULong64_t(1)<<44);
  else if( filter=="hltMu9Ele9DZFilter" ) return j_filterbits&(ULong64_t(1)<<45);
  else if( filter=="hltTripleTrkMuFiltered5NoVtx" ) return j_filterbits&(ULong64_t(1)<<46);
  else{
    cout<<"[Muon::PassFilter] unknown filter "<<filter<<endl;
    exit(ENODATA);
  }
  return false;
}

bool Muon::PassPath(TString path) const{
  if( path=="HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<0);
  else if( path=="HLT_DiMu9_Ele9_CaloIdL_TrackIdL_v" ) return j_pathbits&(ULong64_t(1)<<1);
  else if( path=="HLT_IsoMu24_v" ) return j_pathbits&(ULong64_t(1)<<2);
  else if( path=="HLT_IsoMu27_v" ) return j_pathbits&(ULong64_t(1)<<3);
  else if( path=="HLT_IsoTkMu24_v" ) return j_pathbits&(ULong64_t(1)<<4);
  else if( path=="HLT_Mu17_Mu8_SameSign_DZ_v" ) return j_pathbits&(ULong64_t(1)<<5);
  else if( path=="HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v" ) return j_pathbits&(ULong64_t(1)<<6);
  else if( path=="HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v" ) return j_pathbits&(ULong64_t(1)<<7);
  else if( path=="HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<8);
  else if( path=="HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v" ) return j_pathbits&(ULong64_t(1)<<9);
  else if( path=="HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<10);
  else if( path=="HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v" ) return j_pathbits&(ULong64_t(1)<<11);
  else if( path=="HLT_Mu17_TrkIsoVVL_v" ) return j_pathbits&(ULong64_t(1)<<12);
  else if( path=="HLT_Mu17_v" ) return j_pathbits&(ULong64_t(1)<<13);
  else if( path=="HLT_Mu18_Mu9_SameSign_v" ) return j_pathbits&(ULong64_t(1)<<14);
  else if( path=="HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<15);
  else if( path=="HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v" ) return j_pathbits&(ULong64_t(1)<<16);
  else if( path=="HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<17);
  else if( path=="HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v" ) return j_pathbits&(ULong64_t(1)<<18);
  else if( path=="HLT_Mu50_v" ) return j_pathbits&(ULong64_t(1)<<19);
  else if( path=="HLT_Mu8_DiEle12_CaloIdL_TrackIdL_v" ) return j_pathbits&(ULong64_t(1)<<20);
  else if( path=="HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<21);
  else if( path=="HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v" ) return j_pathbits&(ULong64_t(1)<<22);
  else if( path=="HLT_Mu8_TrkIsoVVL_v" ) return j_pathbits&(ULong64_t(1)<<23);
  else if( path=="HLT_Mu8_v" ) return j_pathbits&(ULong64_t(1)<<24);
  else if( path=="HLT_OldMu100_v" ) return j_pathbits&(ULong64_t(1)<<25);
  else if( path=="HLT_TkMu100_v" ) return j_pathbits&(ULong64_t(1)<<26);
  else if( path=="HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<27);
  else if( path=="HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v" ) return j_pathbits&(ULong64_t(1)<<28);
  else if( path=="HLT_TkMu50_v" ) return j_pathbits&(ULong64_t(1)<<29);
  else if( path=="HLT_TripleMu_10_5_5_DZ_v" ) return j_pathbits&(ULong64_t(1)<<30);
  else if( path=="HLT_TripleMu_12_10_5_v" ) return j_pathbits&(ULong64_t(1)<<31);
  else if( path=="HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx_v" ) return j_pathbits&(ULong64_t(1)<<32);
  else if( path=="HLT_TrkMu15_DoubleTrkMu5NoFiltersNoVtx_v" ) return j_pathbits&(ULong64_t(1)<<33);
  else{
    cout<<"[Muon::PassPath] unknown path "<<path<<endl;
    exit(ENODATA);
  }
  return false;
}



double  Muon::StringToDouble(TString st,TString subSt) const{

  //cout << st << " " << subSt << endl;
  st = st.ReplaceAll(subSt,"");
  st = st.ReplaceAll("p",".");
  st = st.ReplaceAll("neg","-");

  std::string _str = std::string(st);
  std::string::size_type sz;                                                                                                                                                                                         
  //cout << _str << " " << std::stod (_str,&sz) << endl;
  double _d = std::stod (_str,&sz);

  return _d;


}
