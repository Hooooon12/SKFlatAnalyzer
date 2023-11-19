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
  j_trkiso = -999.;
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

void Muon::PrintObject(TString label){
  
  cout << "PrintObject " << label << endl;
  Lepton::PrintObject(label);
  
  cout << "Muon ------ " << endl;
  cout << "Chi2 " << j_chi2  << endl;
  cout << "PFCH04 " << j_PFCH04 << endl;
  cout << "PFNH04 " << j_PFNH04 << endl;
  cout << "PFPH04 " << j_PFPH04 << endl;
  cout << "PU04 " << j_PU04  << endl;
  cout << "TrikIso " << j_trkiso << endl;
  cout << "MiniAODPt " << j_MiniAODPt << endl;
  cout << "MomentumScaleUp " << j_MomentumScaleUp << endl;
  cout << "MomentumScaleDown " << j_MomentumScaleDown << endl;
  cout << "TunePPtError " << j_TunePPtError << endl;
  cout << "MVA " << j_MVA << endl;
  cout << "lowptMVA " << j_lowptMVA << endl;
  cout << "softMVA " << j_softMVA << endl;
  cout << "validmuonhits " << j_validmuonhits << endl;
  cout << "matchedstations " << j_matchedstations << endl;
  cout << "pixelHits " << j_pixelHits << endl;
  cout << "trackerLayers " << j_trackerLayers << endl;
  cout << "museg_comp " << j_museg_comp<< endl;

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
void Muon::SetIso(double ch04, double nh04, double ph04, double pu04, double trkiso){
  j_PFCH04 = ch04;
  j_PFNH04 = nh04;
  j_PFPH04 = ph04;
  j_PU04 = pu04;
  j_trkiso = trkiso;
  CalcPFRelIso();
}

void Muon::SetChi2(double chi2){
  j_chi2 = chi2;
}

void Muon::CalcPFRelIso(bool use_corrected_pt){
  double absiso = j_PFCH04+std::max( 0., j_PFNH04 + j_PFPH04 - 0.5*j_PU04 );
  //cout << "[Muon::CalcPFRelIso] j_PFCH04 = " << j_PFCH04 << endl;                                                                              
  //cout << "[Muon::CalcPFRelIso] j_PFNH04 = " << j_PFNH04 << endl;                                                                              
  //cout << "[Muon::CalcPFRelIso] j_PFPH04 = " << j_PFPH04 << endl;                                                                              
  //cout << "[Muon::CalcPFRelIso] j_PU04 = " << j_PU04 << endl;                                                                                  
  //cout << "[Muon::CalcPFRelIso] --> absiso = " << absiso << endl;                                                                              

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
  if(ID.Contains("2016")) MVACut= mu_mva_cut_fake_2016;
  if(ID.Contains("2017")) MVACut= mu_mva_cut_fake_2017;
  if(ID.Contains("2018")) MVACut= mu_mva_cut_fake_2018;

  ////////////// BASIC IDS

  /// ALL LEPTONS
  if(ID=="NoCut") return true;
  if(ID=="NOCUT") return true;


  //// LOOSE & VETO                                                                                                                                                                                               
  if(ID=="HNLoosest") return (Pass_LepMVAID() || Pass_HNVeto2016());
  if(ID=="HNVetoMVA") return Pass_HNVetoUL(); /// Clone of Pass_HNVeto2016 

  bool HLTCut = (TrkIso()/Pt()<0.4);
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
  if(ID=="POGTightWithTightIso")     return Pass_POGTightWithTightIso();
  if(ID=="POGHighPtWithLooseTrkIso") return Pass_POGHighPtWithLooseTrkIso();
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

  if(ID=="POGMIDTIso") return isPOGMedium() && RelIso()<0.15;
  if(ID=="POGMIDVVLIso") return isPOGMedium() && RelIso()<0.4;
  if(ID=="POGTIDTIso") return isPOGTight() && RelIso()<0.15;
  if(ID=="POGTIDVVLIso") return isPOGTight() && RelIso()<0.4;
  if(ID=="POGIDMPrIsoM" ) return isPOGMedium() && fabs(dXY())<0.02 && fabs(dZ())<0.1 && RelIso()<0.2;
  if(ID=="POGIDMPrIsoVL") return isPOGMedium() && fabs(dXY())<0.02 && fabs(dZ())<0.1 && RelIso()<0.4;


  if(ID=="POGMHLT") return (isPOGMedium() && HLTCut && (fabs(dZ())<0.1));

  if(ID=="HNTightV1") return Pass_HNTight(0.07, 0.02, 0.05, 3.);
  if(ID=="HNTightV2") return (HLTCut && (fabs(dZ())<0.1) && Pass_HNTight(0.07, 0.05, 0.1, 3.));
  /// OTHER GROUP IDs

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

  if(ID=="HNL_HN3L" || ID == "HNL_TopMVA_MM") return (PassID("MVALoose") && (MVA() >= 0.64));
  
  if(ID=="HNL_TopMVA_FO_MM") {
    if(!PassID("MVALoose")) return false;
    if(MVA() < 0.64) {
      if(CloseJet_Ptratio() < 0.45) return false;
      if(CloseJet_BScore() > 0.025) return false;
    }
    return true;
  }

  if(ID=="TopHNT"){
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

  ///// EXO-17028                                                                                                                                                                                                 
  if(ID=="HNVeto_17028")  return Pass_HNVeto2016();
  if(ID=="HNLoose_17028") return Pass_HNLoose2016(0.4, 0.2, 0.1, 3.);
  if(ID=="HNTight_17028") return Pass_HNTight2016();
  /////////////////   MVA ID FUNCTIONS
  
  /// MVAID used to train 
  if(ID=="MVAID") return Pass_LepMVAID();
  if(ID.Contains("MuOpt")) return Pass_MultiFunction_Opt(ID);
  if(ID=="HNL_ULID_Baseline") return Pass_LepMVAID();
  if(ID == "MVALoose")       return (Pass_LepMVAID() && isPOGMedium());
  if(ID == "MVALooseNoPOGM") return Pass_LepMVAID();
  /// Loose ID for SR with MVA cuts
  if(ID == "MVALooseTrgSafe")       return (PassID("MVALoose") && HLTCut);
  if(ID == "MVALooseNoPOGMTrgSafe") return (PassID("MVALooseNoPOGM") && HLTCut);

  /////////// FINAL UL HNL Type-1 ID                                                                                                                                                                               
  if(ID == "HNL_ULID_POGM_FO") return (PassID("MVALooseTrgSafe") && (fabs(IP3D()/IP3Derr()) < 7)); 
  if(ID == "HNL_ULID_FO")      return (PassID("MVALooseNoPOGMTrgSafe")       && (fabs(IP3D()/IP3Derr()) < 7));
  if(ID.Contains("HNL_ULID_FOv2_")) {
    if(!PassID("HNL_ULID_FO")) return false;
    if(MVA() < MVACut) {
      if(this->HasCloseJet() && CloseJet_BScore() > 0.025) return false;
    }
    return true;
  }
  if(ID.Contains("HNL_ULID_FOv3_")){
    if(!PassID("HNL_ULID_FO")) return false;
    if(MVA() < MVACut) {
      if(this->HasCloseJet()){
  if(CloseJet_Ptratio() < 0.45) return false;
  if(CloseJet_BScore() > 0.025) return false;
      }
    }
    return true;
  }

  if(ID.Contains("HNL_ULID_FOv4_"))  return (PassID("HNL_ULID_FO") && (MVA() > -0.9));
  if(ID.Contains("HNL_ULID_FOv5_")) {
    if(!PassID("HNL_ULID_FO")) return false;
    if(MVA() < MVACut) {
      if(CloseJet_BScore() > 0.2) return false;
      if(HNL_MVA_Fake("QCD_LFvsHF_v5") > 0.9)  return false;
    }
    return true;

  }

  if(ID.Contains("HNL_ULID_FOv6_")) {
    if(!PassID("HNL_ULID_FO")) return false;
    if(MVA() < MVACut) {
      if( fabs(this->Eta())<= 1.479 ){
        if(CloseJet_BScore() > 0.27) return false;
      }
      else         if(CloseJet_BScore() > 0.05) return false;
    }
    return true;
  }

  if(ID.Contains("HNL_ULID_FOv7_")) {
    if(!PassID("HNL_ULID_FO")) return false;
    if(MVA() < MVACut) {
      if(CloseJet_BScore() > 0.2) return false;
      if(HNL_MVA_Fake("QCD_BvsC_v5") < -0.9)  return false;
      if(HNL_MVA_Fake("QCD_BvsC_v5") > 0.9)  return false;
      if(HNL_MVA_Fake("QCD_LFvsHF_v5") < -0.9)  return false;
      if(HNL_MVA_Fake("QCD_LFvsHF_v5") > 0.9)  return false;
    }
    return true;
 }
  
  if(ID == "HNL_ULID_2016")   return (PassID("MVALooseTrgSafe") && (MVA() >  MVACut) && (fabs(IP3D()/IP3Derr()) < 7) );
  //if(ID == "HNL_ULID_2017")   return (PassID("MVALooseTrgSafe") && (MVA() >  MVACut) && (fabs(IP3D()/IP3Derr()) < 7) );
  if(ID == "HNL_ULID_2018")   return (PassID("MVALooseTrgSafe") && (MVA() >  MVACut) && (fabs(IP3D()/IP3Derr()) < 7) );
  if(ID == "HNL_ULID_2017"){
    if(!PassID("MVALooseTrgSafe")) return false;
    if( fabs(this->Eta()) <= 1.479 ){
      if(MVA() < 0.77)  return false;
    }
    else{
      if(MVA() < 0.6)  return false;
    }
    if(fabs(IP3D()/IP3Derr()) > 7) return false;
    return true;
  } //JH test old MVA cut


  //// Following are functions to test UL IDs
  
  if(ID=="HNLIPv1") {
    double dxy_cut = 0.01 ;
    if(this->Pt() > 10 && this->Pt()  < 60.) dxy_cut -= (this->Pt() - 10.) * 0.005/ 50.;
    if(this->Pt() > 60.) dxy_cut = 0.005;
    if(fabs(dXY()) >  dxy_cut)   return false;
    if(fabs(dZ()) >  0.05)   return false;
    return true;
  }

  if(ID=="HNLIPv2") {
    double dxy_cut = 0.01 ;
    if(fabs(dXY()) >  dxy_cut)   return false;
    if(fabs(dZ()) >  0.05)   return false;
    return true;
  }
  if(ID=="HNLIPv3") {
    double dxy_cut = 0.02 ;
    if(fabs(dXY()) >  dxy_cut)   return false;
    if(fabs(dZ()) >  0.05)   return false;
    return true;
  }
  if(ID=="HNLIPv4") {
    double dxy_cut = 0.01 ;
    if(fabs(dXY()) >  dxy_cut)   return false;
    if(fabs(dZ()) >  0.05)   return false;
    if(fabs(IP3D()/IP3Derr()) > 3.) return false;
    return true;
  }
  if(ID=="HNLIPv5") {
    double dxy_cut = 0.02 ;
    if(fabs(dXY()) >  dxy_cut)   return false;
    if(fabs(dZ()) >  0.05)   return false;
    if(fabs(IP3D()/IP3Derr()) >3.) return false;
    return true;
  }
  
  if(ID=="HNLIPv6") {
    double dxy_cut = 0.02 ;
    if(fabs(dXY()) >  dxy_cut)   return false;
    if(fabs(dZ()) >  0.05)   return false;
    if(fabs(IP3D()/IP3Derr()) >4.) return false;
    return true;
  }


  if(ID=="HNLIPv7") {
    double dxy_cut = 0.02 ;
    if(fabs(dXY()) >  dxy_cut)   return false;
    if(fabs(dZ()) >  0.05)   return false;
    if(fabs(IP3D()/IP3Derr()) >5.) return false;
    return true;
  }


  if(ID=="HNLIPv8") {
    double dxy_cut = 0.02 ;
    if(fabs(dXY()) >  dxy_cut)   return false;
    if(fabs(dZ()) >  0.05)   return false;
    if(fabs(IP3D()/IP3Derr()) >6.) return false;
    return true;
  }
  if(ID=="HNLIPv9") {
    double dxy_cut = 0.01 ;
    if(fabs(dXY()) >  dxy_cut)   return false;
    if(fabs(dZ()) >  0.05)   return false;
    if(fabs(IP3D()/IP3Derr()) >4.) return false;
    return true;
  }
 
  //// FUNCTIONS for MVA ID                                                                                                                                                                                                                                                                                                   

  if(ID.Contains("HNL_ULIDv2") ) {

    if(HNL_MVA_Fake("EDv4") < 0.)  return false;
    if(!PassID("HNL_ULID_2016")) return false;

    return true;
  }

  if(ID.Contains("HNL_ULIDv3") ) {
    if(HNL_MVA_Fake("EDv4") < 0.2)  return false;
    if(!PassID("HNL_ULID_2016")) return false;
    return true;
  }
  if(ID.Contains("HNL_ULIDv4") ) {
    if(HNL_MVA_Fake("EDv4") < 0.4)  return false;
    if(!PassID("HNL_ULID_2016")) return false;
    return true;
  }


  /// Breakdown of MVA ID cuts                                                                                                                                                                                                                                                                                                                                              
  if(ID == "MVALooseCut1") {
    if(!( isPOGLoose() )) return false;
    if(MiniRelIso() > 0.4) return false;
    return true;
  }
  if(ID == "MVALooseCut2") {
    if(!( isPOGLoose() )) return false;
    if(SIP3D() > 8) return false;
    return true;
  }
  if(ID == "MVALooseCut3") {
    if(!( isPOGLoose() )) return false;
    if(this->fdXY() > 0.05) return false;
    return true;
  }
  if(ID == "MVALooseCut4") {
    if(!( isPOGLoose() )) return false;
    if(this->fdZ() > 0.1) return false;
    return true;
  }


  //// ISDF functions to check several WP for each MVA                                                                                                                                                                                                                                                                                                                      

  if(ID.Contains("HNL_ULID_FAKE")) {
    if(!PassID("MVALoose")) return false;
    double _cut=1;
    if(ID.Contains("_FAKET_")) _cut=0.7;
    if(ID.Contains("_FAKEM_")) _cut=0.5;
    if(ID.Contains("_FAKEL_")) _cut=0.2;
    if(ID.Contains("_FAKEVL_")) _cut=0.;
    if(ID.Contains("_FAKEVVL_")) _cut=-0.2;


    TString IDTmp = ID;
    IDTmp=IDTmp.ReplaceAll("_FAKET_","_FAKE_");
    IDTmp=IDTmp.ReplaceAll("_FAKEM_","_FAKE_");
    IDTmp=IDTmp.ReplaceAll("_FAKEL_","_FAKE_");
    IDTmp=IDTmp.ReplaceAll("_FAKEVL_","_FAKE_");
    IDTmp=IDTmp.ReplaceAll("_FAKEVVL_","_FAKE_");

    if(IDTmp == "HNL_ULID_FAKE_BDTG"  && !Pass_MVA(HNL_MVA_Fake("EDv4"),_cut,"EDv4"))            return false;
    return true;
  }


  if(ID.Contains("HNL_ULID_TopFAKE")) {
    if(!PassID("MVALoose")) return false;
    double _cut=1;
    if(ID.Contains("_TopFAKET_")) _cut=0.7;
    if(ID.Contains("_TopFAKEM_")) _cut=0.5;
    if(ID.Contains("_TopFAKEL_")) _cut=0.2;
    if(ID.Contains("_TopFAKEVL_")) _cut=0.;
    if(ID.Contains("_TopFAKEVVL_")) _cut=-0.2;

    TString IDTmp = ID;
    IDTmp=IDTmp.ReplaceAll("_TopFAKET_","_TopFAKE_");
    IDTmp=IDTmp.ReplaceAll("_TopFAKEM_","_TopFAKE_");
    IDTmp=IDTmp.ReplaceAll("_TopFAKEL_","_TopFAKE_");
    IDTmp=IDTmp.ReplaceAll("_TopFAKEVL_","_TopFAKE_");
    IDTmp=IDTmp.ReplaceAll("_TopFAKEVVL_","_TopFAKE_");

    if(IDTmp == "HNL_ULID_TopFAKE_BDTG"  && !Pass_MVA(j_MVA,_cut,"j_MVA"))            return false;
    return true;
  }


 
  
  ///. Simple ISO/IP sels
  if(ID=="HNTight_Iso05_dxy_01_ip_3") return Pass_HNTight(0.05,0.01,0.05,3.);
  if(ID=="HNTight_Iso05_dxy_01_ip_4") return Pass_HNTight(0.05,0.01,0.05,4.);
  if(ID=="HNTight_Iso05_dxy_01_ip_999") return Pass_HNTight(0.05,0.01,0.05,999.);
  if(ID=="HNTight_Iso05_dxy_02_ip_3") return Pass_HNTight(0.05,0.02,0.05,3.);
  if(ID=="HNTight_Iso05_dxy_02_ip_4") return Pass_HNTight(0.05,0.02,0.05,4.);
  if(ID=="HNTight_Iso05_dxy_02_ip_999") return Pass_HNTight(0.05,0.02,0.05,999.);
  if(ID=="HNTight_Iso05_dxy_05_ip_3") return Pass_HNTight(0.05,0.05, 0.05,3.);
  if(ID=="HNTight_Iso05_dxy_05_ip_4") return Pass_HNTight(0.05,0.05, 0.05,4.);
  if(ID=="HNTight_Iso05_dxy_05_ip_999") return Pass_HNTight(0.05,0.05, 0.05,999.);
  if(ID=="HNTight_Iso06_dxy_01_ip_3") return Pass_HNTight(0.06,0.01,0.05,3.);
  if(ID=="HNTight_Iso06_dxy_01_ip_4") return Pass_HNTight(0.06,0.01,0.05,4.);
  if(ID=="HNTight_Iso06_dxy_01_ip_999") return Pass_HNTight(0.06,0.01,0.05,999.);
  if(ID=="HNTight_Iso06_dxy_02_ip_3") return Pass_HNTight(0.06,0.02,0.05,3.);
  if(ID=="HNTight_Iso06_dxy_02_ip_4") return Pass_HNTight(0.06,0.02,0.05,4.);
  if(ID=="HNTight_Iso06_dxy_02_ip_999") return Pass_HNTight(0.06,0.02,0.05,999.);
  if(ID=="HNTight_Iso06_dxy_05_ip_3") return Pass_HNTight(0.06,0.05, 0.05,3.);
  if(ID=="HNTight_Iso06_dxy_05_ip_4") return Pass_HNTight(0.06,0.05, 0.05,4.);
  if(ID=="HNTight_Iso06_dxy_05_ip_999") return Pass_HNTight(0.06,0.05, 0.05,999.);
  if(ID=="HNTight_Iso07_dxy_01_ip_3") return Pass_HNTight(0.07,0.01,0.05,3.);
  if(ID=="HNTight_Iso07_dxy_01_ip_4") return Pass_HNTight(0.07,0.01,0.05,4.);
  if(ID=="HNTight_Iso07_dxy_01_ip_999") return Pass_HNTight(0.07,0.01,0.05,999.);
  if(ID=="HNTight_Iso07_dxy_02_ip_3") return Pass_HNTight(0.07,0.02,0.05,3.);
  if(ID=="HNTight_Iso07_dxy_02_ip_4") return Pass_HNTight(0.07,0.02,0.05,4.);
  if(ID=="HNTight_Iso07_dxy_02_ip_999") return Pass_HNTight(0.07,0.02,0.05,999.);
  if(ID=="HNTight_Iso07_dxy_05_ip_3") return Pass_HNTight(0.07,0.05, 0.05,3.);
  if(ID=="HNTight_Iso07_dxy_05_ip_4") return Pass_HNTight(0.07,0.05, 0.05,4.);
  if(ID=="HNTight_Iso07_dxy_05_ip_999") return Pass_HNTight(0.07,0.05, 0.05,999.);
  if(ID=="HNTight_Iso08_dxy_01_ip_3") return Pass_HNTight(0.08,0.01,0.05,3.);
  if(ID=="HNTight_Iso08_dxy_01_ip_4") return Pass_HNTight(0.08,0.01,0.05,4.);
  if(ID=="HNTight_Iso08_dxy_01_ip_999") return Pass_HNTight(0.08,0.01,0.05,999.);
  if(ID=="HNTight_Iso08_dxy_02_ip_3") return Pass_HNTight(0.08,0.02,0.05,3.);
  if(ID=="HNTight_Iso08_dxy_02_ip_4") return Pass_HNTight(0.08,0.02,0.05,4.);
  if(ID=="HNTight_Iso08_dxy_02_ip_999") return Pass_HNTight(0.08,0.02,0.05,999.);
  if(ID=="HNTight_Iso08_dxy_05_ip_3") return Pass_HNTight(0.08,0.05, 0.05,3.);
  if(ID=="HNTight_Iso08_dxy_05_ip_4") return Pass_HNTight(0.08,0.05, 0.05,4.);
  if(ID=="HNTight_Iso08_dxy_05_ip_999") return Pass_HNTight(0.08,0.05, 0.05,999.);
  if(ID=="HNTight_Iso09_dxy_01_ip_3") return Pass_HNTight(0.09,0.01,0.05,3.);
  if(ID=="HNTight_Iso09_dxy_01_ip_4") return Pass_HNTight(0.09,0.01,0.05,4.);
  if(ID=="HNTight_Iso09_dxy_01_ip_999") return Pass_HNTight(0.09,0.01,0.05,999.);
  if(ID=="HNTight_Iso09_dxy_02_ip_3") return Pass_HNTight(0.09,0.02,0.05,3.);
  if(ID=="HNTight_Iso09_dxy_02_ip_4") return Pass_HNTight(0.09,0.02,0.05,4.);
  if(ID=="HNTight_Iso09_dxy_02_ip_999") return Pass_HNTight(0.09,0.02,0.05,999.);
  if(ID=="HNTight_Iso09_dxy_05_ip_3") return Pass_HNTight(0.09,0.05, 0.05,3.);
  if(ID=="HNTight_Iso09_dxy_05_ip_4") return Pass_HNTight(0.09,0.05, 0.05,4.);
  if(ID=="HNTight_Iso09_dxy_05_ip_999") return Pass_HNTight(0.09,0.05, 0.05,999.);
  if(ID=="HNTight_Iso10_dxy_01_ip_3") return Pass_HNTight(0.10,0.01,0.05,3.);
  if(ID=="HNTight_Iso10_dxy_01_ip_4") return Pass_HNTight(0.10,0.01,0.05,4.);
  if(ID=="HNTight_Iso10_dxy_01_ip_999") return Pass_HNTight(0.10,0.01,0.05,999.);
  if(ID=="HNTight_Iso10_dxy_02_ip_3") return Pass_HNTight(0.10,0.02,0.05,3.);
  if(ID=="HNTight_Iso10_dxy_02_ip_4") return Pass_HNTight(0.10,0.02,0.05,4.);
  if(ID=="HNTight_Iso10_dxy_02_ip_999") return Pass_HNTight(0.10,0.02,0.05,999.);
  if(ID=="HNTight_Iso10_dxy_05_ip_3") return Pass_HNTight(0.10,0.05, 0.05,3.);
  if(ID=="HNTight_Iso10_dxy_05_ip_4") return Pass_HNTight(0.10,0.05, 0.05,4.);
  if(ID=="HNTight_Iso10_dxy_05_ip_999") return Pass_HNTight(0.10,0.05, 0.05,999.);

  cout << "[Muon::PassID] No id : " << ID << endl;
  exit(ENODATA);

  return false;

}

bool Muon::Pass_MultiFunction_Opt(TString ID) const {
  
  //
  //cout << "ID = " << ID << "  " << this->Pt() << " " << this->Eta()<< endl;

  if (ID.Contains("HNL2016") && !PassID("HNL_ULID_2016")) return false;
  if (ID.Contains("HNL2017") && !PassID("HNL_ULID_2017")) return false;
  if (ID.Contains("HNL2018") && !PassID("HNL_ULID_2018")) return false;

 
  TString ID_sub = ID;
  ID_sub = ID_sub.ReplaceAll("_"," ");
  string sID_sub = string(ID_sub);

  vector<TString> subStrings;
  istringstream ID_subs(sID_sub);
  do {
    string subs;
    ID_subs >> subs;
    subStrings.push_back(TString(subs));
  } while (ID_subs);

  //for (auto is : subStrings) cout << " --> " << is << endl;


  TString dxy_method = "";
  TString dz_method = "";
  TString sip_method = "";
  TString iso_methodB="";
  TString iso_methodEC="";
  TString pog_methodB="";
  TString pog_methodEC="";
  

  TString mva_methodBB="";
  TString mva_methodEC="";

  TString mva_methodLFBB="";
  TString mva_methodLFEC="";

  TString NPMVAString="";

  // If MVA ID then need Loose MVA and pog medium
  if(ID.Contains("MVA")&& !Pass_LepMVAID()) return false;

  /// Apply pog medium as loose sel
  if(!isPOGMedium()) return false;

  for(unsigned int i=0; i < subStrings.size(); i++){
    
    if (subStrings[i].Contains("NPMM")){
      NPMVAString =subStrings[i];
      NPMVAString = NPMVAString.ReplaceAll("NPMM","");
    }

    if (subStrings[i].Contains("MVAHFBB")) mva_methodBB=subStrings[i];
    if (subStrings[i].Contains("MVAHFEC")) mva_methodEC=subStrings[i];

    if (subStrings[i].Contains("MVALFBB")) mva_methodLFBB=subStrings[i];
    if (subStrings[i].Contains("MVALFEC")) mva_methodLFEC=subStrings[i];

    if (subStrings[i].Contains("POGB")) pog_methodB=subStrings[i];
    if (subStrings[i].Contains("POGEC")) pog_methodEC=subStrings[i];

    if (subStrings[i].Contains("ISOB")) iso_methodB=subStrings[i];
    if (subStrings[i].Contains("ISOEC")) iso_methodEC=subStrings[i];
    
    if (subStrings[i].Contains("DXY")) dxy_method=subStrings[i];
    if (subStrings[i].Contains("DZ"))  dz_method=subStrings[i];
    if (subStrings[i].Contains("SIP")) sip_method=subStrings[i];

  }

  
  /*cout << "mva_methodBB = " << mva_methodBB << endl;
  cout << "mva_methodEC = " << mva_methodEC << endl;
  cout << "mva_methodLFBB = " << mva_methodLFBB << endl;
  cout << "mva_methodLFEC = " << mva_methodLFEC << endl;
  
  cout << "pog_methodB " <<  pog_methodB << endl;  
  cout << "pog_methodEC " <<  pog_methodEC << endl;  
  cout << "iso_methodB " <<  iso_methodB << endl;  
  cout << "iso_methodEC " <<  iso_methodEC << endl;  
  cout << "dxy_method " <<  dxy_method << endl;  
  cout << "NPMVAString " << NPMVAString << endl;
  */

  if(dxy_method.Contains("DXYv")){
    
    if(!PassID(dxy_method.ReplaceAll("DXY","HNLIP"))) return false;
    
  }  
  else  if(dxy_method.Contains("DXY")){
    double dxy_cut = StringToDouble(dxy_method,"DXY");
    if(fabs(dXY())  > dxy_cut)  return false; 
  }
  if(dz_method.Contains("DZ")){
    double dz_cut = StringToDouble(dz_method,"DZ");
    if(fabs(dZ())  > dz_cut)  return false;
  }
  if(sip_method.Contains("SIP")){
    double sip_cut = StringToDouble(sip_method,"SIP");
    if(fabs(SIP3D())  > sip_cut)  return false;
  }

  return   PassIDOptMulti(NPMVAString,pog_methodB,pog_methodEC,mva_methodBB,mva_methodEC ,mva_methodLFBB,mva_methodLFEC,  iso_methodB,iso_methodEC);
 
}

bool Muon::passIDHN(int ID, double dxy_b, double dxy_e, double dz_b,double dz_e, double sip_b, double sip_e, double iso_b,double iso_e, double miso_b, double miso_e) const{

  if( fabs(this->Eta())<= 1.479 ){
    if(! (fabs(dXY())< dxy_b && fabs(dZ())<dz_b && fabs(IP3D()/IP3Derr())< sip_b) ) return false;
  }
  else{
    if(! (fabs(dXY())< dxy_e && fabs(dZ())<dz_e && fabs(IP3D()/IP3Derr())< sip_e) ) return false;
  }
  if( ID==1){
   

    if(! (isPOGTight()) ) return false;
    if( miso_e > 0.){
      if(fabs(this->Eta())> 1.479  &&  (MiniRelIso()>fabs(miso_e)) ) return false;
    }
    else if( iso_e > 0.){
      if(fabs(this->Eta())> 1.479  &&  (RelIso()>fabs(iso_e)) ) return false;
    }
    else if(fabs(this->Eta())> 1.479  &&  ! (isPOGTight()))  return false;
    
    if( miso_b > 0.){
      if(fabs(this->Eta()) <= 1.479  &&  (MiniRelIso()>fabs(miso_b)) ) return false;
    }
    else if( iso_b > 0.){
      if(fabs(this->Eta()) <= 1.479  &&  (RelIso()>fabs(iso_b)) ) return false;
    }
    else if(fabs(this->Eta()) <= 1.479  &&! (isPOGTight()))  return false;
    
  }
  else if( ID==2){

    if(! (isPOGMedium()) ) return false;
    if( miso_e > 0.){
      if(fabs(this->Eta())> 1.479  &&  (MiniRelIso()>fabs(miso_e)) ) return false;
    }
    else if( iso_e > 0.){
      if(fabs(this->Eta())> 1.479  &&  (RelIso()>fabs(iso_e)) ) return false;
    }
    else if(fabs(this->Eta())> 1.479  &&  ! (isPOGMedium()))  return false;
    if( miso_b > 0.){
      if(fabs(this->Eta()) <= 1.479  &&  (MiniRelIso()>fabs(miso_b)) ) return false;
    }
    else if( iso_b > 0.){
      if(fabs(this->Eta()) <= 1.479  &&  (RelIso()>fabs(iso_b)) ) return false;
    }
    else if(fabs(this->Eta()) <= 1.479  &&! (isPOGMedium()))  return false;
    
  }

  else if( ID==3){
   
    if(fabs(this->Eta())> 1.479  &&  (RelIso()>fabs(iso_e)) ) return false;
    if(fabs(this->Eta())< 1.479  &&  (RelIso()>fabs(iso_b)) ) return false;

  }
  else if( ID==4){
    if(fabs(this->Eta())> 1.479  &&  (MiniRelIso()>fabs(miso_e)) ) return false;
    if(fabs(this->Eta())< 1.479  &&  (MiniRelIso()>fabs(miso_b)) ) return false;

  }

  return true;
}




bool Muon::Pass_LepMVAID() const {

  if(this->Pt() < 5)     return false;
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


int  Muon::PassIDOptMulti(TString mva_method,TString sel_methodB,TString sel_methodEC,  TString mva_methodBB, TString mva_methodEC,TString mva_methodLFBB, TString mva_methodLFEC, TString iso_methodB,TString iso_methodEC ) const{

  bool DEBUG=false;

  //  ///NPMVABB1neg1 NPMVABB2neg1 NPMVABB3neg1 NPMVABB40p73POGECT ISOEC0p15 DXYv1/FillEventCutflow/MuonSR 3.28117668762                                                                                                                                         
  //cout << "PassIDOptMulti " << endl;
  if( fabs(this->Eta())<= 1.479 ){
    
    if(mva_method != ""){
      //cout << "LF  " << mva_methodLFBB << "  mva_method " <<  mva_method << " mva_methodLFBB = " << mva_methodLFBB << endl;
      if(mva_methodLFBB.Contains("MVALFBB") && !Pass_MVA(HNL_MVA_Fake(mva_method),StringToDouble(mva_methodLFBB,"MVALFBB"),  mva_methodLFBB )) return 0;   
    }
    
    if(mva_methodBB.Contains("MVAHFBB")) {
      //cout << "mva_methodBB = " << mva_methodBB << endl;
      if( !Pass_MVA(MVA(),StringToDouble(mva_methodBB,"MVAHFBB"), mva_methodBB )) return 0;
    }
    if(sel_methodB == "POGBT"){
      if(! (isPOGTight()) ) return 0;
    }
    if(sel_methodB == "POGBM"){
      if(! (isPOGMedium()) ) return 0;
    }
    
    if(iso_methodB != ""){
      
      double  iso_cut_B = StringToDouble(iso_methodB,"ISOB");

      if(DEBUG) cout << "RelIso " << iso_cut_B << " val= " <<  RelIso() << endl;
      if(! (RelIso()<iso_cut_B) ) return 0;
    }
  }
  else{
    if(DEBUG) cout << "PassIDOpt ENDCAP " << endl;

    if(mva_method != ""){
      //cout << "LF  " <<mva_methodLFEC << "  mva_method " <<  mva_method << " mva_methodLFEC = " << mva_methodLFEC << endl;

      if(mva_methodLFEC.Contains("MVALFEC") && !Pass_MVA(HNL_MVA_Fake(mva_method),StringToDouble(mva_methodLFEC,"MVALFEC"),  mva_methodLFEC )) return 0;
    }
    if(mva_methodEC.Contains("MVAHFEC")){
      //cout << "mva_methodEC = " << mva_methodEC<< endl;
      if(!Pass_MVA(MVA(),StringToDouble(mva_methodEC,"MVAHFEC"), mva_methodEC )) return 0;
    }
    
    if(iso_methodEC != ""){

      double  iso_cut_EC = StringToDouble(iso_methodEC,"ISOEC");

      if(DEBUG) cout << "RelIso " << iso_cut_EC << " val = " << RelIso() <<endl;

      if(! (RelIso()<iso_cut_EC) ) return 0;
    }

    if(sel_methodEC == "POGECT"){
      if(! (isPOGTight()) ) return 0;
    }
    if(sel_methodEC == "POGECM"){
      if(! (isPOGMedium()) ) return 0;
    }
  }

  return 1;

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
