#include "Muon.h"

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
}

Muon::~Muon(){
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

void Muon::CalcPFRelIso(){
  double absiso = j_PFCH04+std::max( 0., j_PFNH04 + j_PFPH04 - 0.5*j_PU04 );
  //cout << "[Muon::CalcPFRelIso] j_PFCH04 = " << j_PFCH04 << endl;
  //cout << "[Muon::CalcPFRelIso] j_PFNH04 = " << j_PFNH04 << endl;
  //cout << "[Muon::CalcPFRelIso] j_PFPH04 = " << j_PFPH04 << endl;
  //cout << "[Muon::CalcPFRelIso] j_PU04 = " << j_PU04 << endl;
  //cout << "[Muon::CalcPFRelIso] --> absiso = " << absiso << endl;
  //this->SetRelIso(absiso/this->Pt()); //JH
  this->SetRelIso(absiso/this->MiniAODPt()); //TODO This is same as IDBit
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
  //j_lowptMVA = lowptMVA;
  //j_softMVA = softMVA;
}

bool Muon::PassID(TString ID) const {
  //==== POG
  if(ID=="POGTight") return isPOGTight();
  if(ID=="POGHighPt") return isPOGHighPt();
  if(ID=="POGMedium") return isPOGMedium();
  if(ID=="POGLoose") return isPOGLoose();
  if(ID=="POGTightWithTightIso") return Pass_POGTightWithTightIso();
  if(ID=="POGTightWithLooseIso") return Pass_POGTightWithLooseIso();
  if(ID=="POGTightWithVetoIso") return Pass_POGTightWithVetoIso();
  if(ID=="POGHighPtWithLooseTrkIso") return Pass_POGHighPtWithLooseTrkIso();
  if(ID=="POGHighPtWithVeryLooseTrkIso") return Pass_POGHighPtWithVeryLooseTrkIso();
  if(ID=="POGHighPtWithVetoTrkIso") return Pass_POGHighPtWithVetoTrkIso();
  //==== Customized
  if(ID=="TEST") return Pass_TESTID();
  if(ID=="HNVeto2016") return Pass_HNVeto2016();
  if(ID=="HNLoose2016") return Pass_HNLoose2016();
  if(ID=="HNTight2016") return Pass_HNTight2016();
  if(ID=="HNLoose") return Pass_HNLoose();
  if(ID=="HNLooseV3") return Pass_HNLoose(0.4, 0.2, 0.5, 10.); // RelIso, d0, dz, SIP
  if(ID=="HNTight") return Pass_HNTight();
  if(ID=="HNTightV1") return Pass_HNTight(0.07, 0.05, 0.1, 3.); // RelIso, d0, dz, SIP
  if(ID=="HNTightV2") return Pass_HNTightV2();
  if(ID=="SSWW_tight") return SSWW_tight();
  if(ID=="SSWW_loose") return SSWW_loose();

  if(ID=="ISRVeto") return Pass_ISRVeto(0.6);

  //==== No cut
  if(ID=="NOCUT") return true;

  cout << "[Muon::PassID] No id : " << ID << endl;
  exit(EXIT_FAILURE);

  return false;

}
bool Muon::Pass_POGTightWithTightIso() const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.15 ))  return false;
  return true;
}
bool Muon::Pass_POGTightWithLooseIso() const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.4 ))  return false;
  return true;
}
bool Muon::Pass_POGTightWithVetoIso() const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.6 ))  return false;
  return true;
}
bool Muon::Pass_POGHighPtWithLooseTrkIso() const {
  if(!( isPOGHighPt() )) return false;
  if(!( TrkIso()/TuneP4().Pt()<0.1 )) return false;
  return true;
}
bool Muon::Pass_POGHighPtWithVeryLooseTrkIso() const {
  if(!( isPOGHighPt() )) return false;
  if(!( TrkIso()/TuneP4().Pt()<0.3 )) return false;
  return true;
}
bool Muon::Pass_POGHighPtWithVetoTrkIso() const {
  if(!( isPOGHighPt() )) return false;
  if(!( TrkIso()/TuneP4().Pt()<0.4 )) return false;
  return true;
}

bool Muon::Pass_HNVeto2016() const{
  if(!( isPOGLoose() )) return false;
  if(!( fabs(dXY())<0.2 && fabs(dZ())<0.5) ) return false;
  if(!( RelIso()<0.6 ))  return false;
  if(!( Chi2()<50. )) return false;
  return true;
}

bool Muon::Pass_HNLoose2016() const{
  if(!( isPOGLoose() )) return false;
  if(!( fabs(dXY())<0.2 && fabs(dZ())<0.1 && fabs(IP3D()/IP3Derr())<3.) ) return false;
  if(!( RelIso()<0.4 ))  return false;
  if(!( Chi2()<50. )) return false;
  return true;
}

bool Muon::Pass_HNTight2016() const{
  if(!( isPOGTight() )) return false;
  if(!( fabs(dXY())<0.005 && fabs(dZ())<0.04 && fabs(IP3D()/IP3Derr())<3.) ) return false;
  if(!( RelIso()<0.07 ))  return false;
  if(!( Chi2()<10. )) return false;
  return true;
}

bool Muon::Pass_HNLoose() const{
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.4 )) return false;
  return true;
}

bool Muon::Pass_HNLoose(double relisoCut, double dxyCut, double dzCut, double sipCut) const {
  // Individual cuts of the POG cut-based tight ID
  if(!( isPOGLoose() )) return false;
  if(!( IsType(GlobalMuon) )) return false;
  if(!( Chi2()<10. )) return false;
  if(!( ValidMuonHits()>0 )) return false;
  if(!( MatchedStations()>1 )) return false;
  if(!( fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
  if(!( PixelHits()>0 )) return false;
  if(!( TrackerLayers()>5 )) return false;
  //if(!( isPOGTight() )) return false;
  //if(!( fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
  // RelPFIso
  if(!( RelIso()<relisoCut )) return false;
  if(!( fabs(IP3D()/IP3Derr())<sipCut )) return false;
  return true;
}

bool Muon::Pass_HNTight(double relisoCut, double dxyCut, double dzCut, double sipCut) const {
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<relisoCut )) return false;
  if(!( fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
  if(!( fabs(IP3D()/IP3Derr())<sipCut )) return false;
  return true;
}

bool Muon::Pass_ISRVeto(double relisoCut) const {
  if(!( isPOGLoose() )) return false;
  if(!( RelIso()<relisoCut )) return false;
  return true;
}

bool Muon::Pass_HNTight() const{
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.15 )) return false;
  return true;
}

bool Muon::Pass_HNTightV2() const{
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.1 )) return false;
  if(!( fabs(dXY())<0.01 && fabs(dZ())<0.04 )) return false;
  return true;
}

bool Muon::SSWW_tight() const{
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.15 )) return false;
  if(this->Pt()<20.){
    if(!( fabs(dXY())<0.01 )) return false;
  }
  else if(this->Pt()>=20.){
    if(!( fabs(dXY())<0.02 )) return false;
  }
  if(!( fabs(dZ())<0.1 )) return false;
  return true;
}

bool Muon::SSWW_loose() const{
  if(!( isPOGTight() )) return false;
  if(!( RelIso()<0.4 )) return false;
  if(this->Pt()<20.){
    if(!( fabs(dXY())<0.01 )) return false;
  }
  else if(this->Pt()>=20.){
    if(!( fabs(dXY())<0.02 )) return false;
  }
  if(!( fabs(dZ())<0.1 )) return false;
  return true;
}


//==== TEST ID

bool Muon::Pass_TESTID() const {
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
