#include "Electron.h"

ClassImp(Electron)

Electron::Electron(){

  j_En_up=1.;
  j_En_down=1.;;
  j_Res_up = 1.;
  j_Res_down = 1.;

  j_scEta = -999.;
  j_scPhi = -999.;
  j_scE = -999.;
  j_mvaiso = -999.;
  j_mvanoiso = -999.;
  j_EnergyUnCorr = -999.;
  j_passConversionVeto = false;
  j_NMissingHits = 0;
  j_Full5x5_sigmaIetaIeta = -999.;
  j_dEtaSeed = -999.;
  j_dPhiIn = -999.;
  j_HoverE  = -999.;
  j_InvEminusInvP = -999.;
  j_e2x5OverE5x5 = -999.;
  j_e1x5OverE5x5 = -999.;
  j_trkiso = -999.;
  j_dr03EcalRecHitSumEt = -999.;
  j_dr03HcalDepth1TowerSumEt = -999.;
  j_dr03HcalTowerSumEt = -999.;
  j_dr03TkSumPt = -999.;
  j_ecalPFClusterIso = -999.;
  j_hcalPFClusterIso = -999.;
  j_isEcalDriven = false;
  j_IDBit = 0;
  j_IDCutBit.clear();
  j_Rho = -999.;
  j_isGsfCtfScPixChargeConsistent = false;
  this->SetLeptonFlavour(ELECTRON);
}

Electron::~Electron(){

}

void Electron::SetEnShift(double en_up, double en_down){
  j_En_up = en_up;
  j_En_down = en_down;
}

void Electron::SetResShift(double res_up, double res_down){
  j_Res_up = res_up;
  j_Res_down = res_down;
}

void Electron::SetSC(double sceta, double scphi, double sce){
  j_scEta = sceta;
  j_scPhi = scphi;
  j_scE = sce;
}

void Electron::SetMVA(double mvaiso, double mvanoiso){
  j_mvaiso = mvaiso;
  j_mvanoiso = mvanoiso;
}

void Electron::SetUncorrE(double une){
  j_EnergyUnCorr = une;
}

void Electron::SetPassConversionVeto(bool b){
  j_passConversionVeto = b;
}

void Electron::SetNMissingHits(int n){
  j_NMissingHits = n;
}

void Electron::SetCutBasedIDVariables(
    double Full5x5_sigmaIetaIeta,
    double dEtaSeed,
    double dPhiIn,
    double HoverE,
    double InvEminusInvP,
    double e2x5OverE5x5,
    double e1x5OverE5x5,
    double trackIso,
    double dr03EcalRecHitSumEt,
    double dr03HcalDepth1TowerSumEt,
    double dr03HcalTowerSumEt,
    double dr03TkSumPt,
    double ecalPFClusterIso,
    double hcalPFClusterIso,
    int ecalDriven
  ){
  j_Full5x5_sigmaIetaIeta = Full5x5_sigmaIetaIeta;
  j_dEtaSeed = dEtaSeed;
  j_dPhiIn = dPhiIn;
  j_HoverE = HoverE;
  j_InvEminusInvP = InvEminusInvP;
  j_e2x5OverE5x5 = e2x5OverE5x5;
  j_e1x5OverE5x5 = e1x5OverE5x5;
  j_trkiso = trackIso;
  j_dr03EcalRecHitSumEt = dr03EcalRecHitSumEt;
  j_dr03HcalDepth1TowerSumEt = dr03HcalDepth1TowerSumEt;
  j_dr03HcalTowerSumEt = dr03HcalTowerSumEt;
  j_dr03TkSumPt = dr03TkSumPt;
  j_ecalPFClusterIso = ecalPFClusterIso;
  j_hcalPFClusterIso = hcalPFClusterIso;

  if(ecalDriven==0) j_isEcalDriven = false;
  else j_isEcalDriven = true;
}

void Electron::SetIDBit(unsigned int idbit){
  j_IDBit = idbit;
}

void Electron::SetIDCutBit(vector<int> idcutbit){
  j_IDCutBit = idcutbit;
}

void Electron::SetRelPFIso_Rho(double r){
  j_RelPFIso_Rho = r;
  this->SetRelIso(r);
}

double Electron::EA(){

  //==== RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt
  
  double eta = fabs(this->scEta());

  if     (eta<1.000) return 0.1440;
  else if(eta<1.479) return 0.1562;
  else if(eta<2.000) return 0.1032;
  else if(eta<2.200) return 0.0859;
  else if(eta<2.300) return 0.1116;
  else if(eta<2.400) return 0.1321;
  else if(eta<2.500) return 0.1654;
  else return 0.1654;

}

bool Electron::isHEM() const{
  if(-3.0<scEta()&&scEta()<-1.3 && -1.57<scPhi()&&scPhi()<-0.87) return true;
  else return false;
}

bool Electron::PassID(TString ID) const{

  //==== XXX veto Gap Always
  if(etaRegion()==GAP) return false;

  //==== POG
  if(ID=="passVetoID") return passVetoID();
  if(ID=="passLooseID") return passLooseID();
  if(ID=="passMediumID") return passMediumID();
  if(ID=="passTightID") return passTightID();
  if(ID=="passHEEPID") return passHEEPID();
  if(ID=="passHEEPID2018Prompt") return passHEEP2018Prompt();
  if(ID=="passMVAID_noIso_WP80") return passMVAID_noIso_WP80();
  if(ID=="passMVAID_noIso_WP90") return passMVAID_noIso_WP90();
  if(ID=="passMVAID_iso_WP80") return passMVAID_iso_WP80();
  if(ID=="passMVAID_iso_WP90") return passMVAID_iso_WP90();
  //==== Customized
  if(ID=="NewVetoID") return Pass_NewVetoID();
  if(ID=="SUSYTight") return Pass_SUSYTight();
  if(ID=="SUSYLoose") return Pass_SUSYLoose();
  if(ID=="NOCUT") return true;
  if(ID=="TEST") return Pass_TESTID();
  if(ID=="TightWithIPcut") return Pass_CutBasedTightWithIPcut();
  if(ID=="HEEP_dZ_CF") return Pass_HEEP_dZ_CF();
  if(ID=="HEEP2018_dZ_CF") return Pass_HEEP2018_dZ_CF();
  if(ID=="HEEP_dZ") return Pass_HEEP_dZ();
  if(ID=="HEEP2018_dZ") return Pass_HEEP2018_dZ();
  if(ID=="CutBasedLooseNoIso") return Pass_CutBasedLooseNoIso();
  if(ID=="HNVeto2016") return Pass_HNVeto2016();
  if(ID=="HNLoose2016") return Pass_HNLoose2016();
  if(ID=="HNTight2016") return Pass_HNTight2016();
  if(ID=="HNLooseV1") return Pass_HNLoose(0.6, 0.05, 0.1, 4., true);
  if(ID=="HNLooseV21") return Pass_HNLooseV21();
  if(ID=="HNLooseV22") return Pass_HNLooseV22();
  if(ID=="HNLooseV23") return Pass_HNLooseV23();
  if(ID=="HNTightV1_250") return Pass_HNTight(250, 0.05, 0.1, 4., true);
  if(ID=="HNTightV1_200") return Pass_HNTight(200, 0.05, 0.1, 4., true);
  if(ID=="HNTightV1") return Pass_HNTight(0.05, 0.1, 4., true);
  if(ID=="HNTightV2") return Pass_HNTightV2();
  if(ID=="SSWW_tight") return SSWW_tight();
  if(ID=="SSWW_tight2016") return SSWW_tight2016();
  if(ID=="SSWW_loose") return SSWW_loose();
  if(ID=="SSWW_loose2016") return SSWW_loose2016();
  if(ID=="HNMVALoose") return Pass_HNMVALoose();
  if(ID=="HNMVALooseV2") return Pass_HNMVALooseV2();
  if(ID=="HNMVATightV2") return Pass_HNMVATightV2();
  if(ID=="HNMVATight") return Pass_HNMVATight(0.08, 0.05, 0.1, true);
  if(ID=="ISRVeto") return Pass_ISRVeto(0.6);
  if(ID=="ISRLoose") return Pass_ISRLoose();
  if(ID=="ISRTight") return Pass_ISRTight();
  cout << "[Electron::PassID] No id : " << ID << endl;
  exit(EXIT_FAILURE);

  return false;
}

bool Electron::Pass_SUSYMVAWP(TString wp) const{

  double sceta = fabs(scEta());

    double cutA = 0.77;
    double cutB = 0.52;

  if(wp=="Tight"){
    if     (sceta<0.8)  { cutA = 0.77; cutB = 0.52; }
    else if(sceta<1.479){ cutA = 0.56; cutB = 0.11; } 
    else                { cutA = 0.48; cutB =-0.01; }
  }
  else if(wp=="Loose"){
    if     (sceta<0.8)  { cutA =-0.48; cutB =-0.85; }
    else if(sceta<1.479){ cutA =-0.67; cutB =-0.91; }
    else                { cutA =-0.49; cutB =-0.83; }
  }
  else{}

  double cutSlope = (cutB-cutA) / 10.;
  double cutFinal = std::min( cutA, std::max(cutB , cutA + cutSlope*(this->Pt()-15.) ) );

  //==== Using NoIso MVA, because we apply MiniIso later
  if(MVANoIso()>cutFinal) return true;
  else return false;

}

bool Electron::Pass_SUSYTight() const{
  if(! Pass_SUSYMVAWP("Tight") ) return false;
  if(! (MiniRelIso()<0.1) ) return false;
  if(! (fabs(dXY())<0.05 && fabs(dZ())<0.1 && fabs(IP3D()/IP3Derr())<8.) ) return false;
  if(! PassConversionVeto() ) return false;
  if(! (NMissingHits()==0) ) return false;

  return true;
}

bool Electron::Pass_SUSYLoose() const{
  if(! Pass_SUSYMVAWP("Loose") ) return false;
  if(! (MiniRelIso()<0.4) ) return false;
  if(! (fabs(dXY())<0.05 && fabs(dZ())<0.1 && fabs(IP3D()/IP3Derr())<8.) ) return false;
  if(! PassConversionVeto() ) return false;
  if(! (NMissingHits()==0) ) return false;

  return true;
}

bool Electron::Pass_CutBasedTightWithIPcut() const{
  if(! passTightID() ) return false;
  if( fabs(scEta()) <= 1.479 ){
    if(! (fabs(dXY())<0.05 && fabs(dZ())<0.1) ) return false;
  }
  else{
    if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
  }
  return true;
}

//===============================================
//==== HEEP ID
//===============================================

bool Electron::Pass_HEEP_dZ_CF() const{
  if(!passHEEPID() ) return false;

  if( fabs(scEta()) <= 1.479 ){
    if(!( fabs(dZ()) <0.10 )) return false;
  }
  else{
    if(!( fabs(dZ()) <0.20 )) return false;
  }

  if(! (PassConversionVeto()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  if(! (Pass_TriggerEmulation()) ) return false;

  return true;
}

bool Electron::Pass_HEEP2018_dZ_CF() const{
  if(!passHEEP2018Prompt() ) return false;

  if( fabs(scEta()) <= 1.479 ){
    if(!( fabs(dZ()) <0.10 )) return false;
  }
  else{
    if(!( fabs(dZ()) <0.20 )) return false;
  }

  if(! (PassConversionVeto()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  if(! (Pass_TriggerEmulation()) ) return false;

  return true;
}

bool Electron::Pass_HEEP_dZ() const{
  if(!passHEEPID() ) return false;

  if( fabs(scEta()) <= 1.479 ){
    if(!( fabs(dZ()) <0.10 )) return false;
  }
  else{
    if(!( fabs(dZ()) <0.20 )) return false;
  }

  return true;
}

bool Electron::Pass_HEEP2018_dZ() const{
  if(!passHEEP2018Prompt() ) return false;

  if( fabs(scEta()) <= 1.479 ){
    if(!( fabs(dZ()) <0.10 )) return false;
  }
  else{
    if(!( fabs(dZ()) <0.20 )) return false;
  }

  return true;
}

bool Electron::Pass_NewVetoID() const{
  if(!passVetoID() ) return false;
  if(!( fabs(dXY()) <0.2 )) return false;
  if(!( fabs(dZ()) <0.5 )) return false;
  if(! (RelIso()<0.6) ) return false;

  return true;
}

bool Electron::passHEEP2018Prompt() const {

  //==== If not endcap, use original function
  if( fabs(scEta()) < 1.566 ) return passHEEPID();

  //==== https://github.com/CMSSNU/SKFlatMaker/blob/Run2Legacy_v4/SKFlatMaker/python/SKFlatMaker_cfi.py#L37-L50
  int HEEPcutbit = IDCutBit().at(11);

  //==== https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Applying_Individual_Cuts_of_a_Se
  //==== We will modify H/E (bit nr=6) and EM+Had_depth1 (bit nr=8) isolation cut for EndCap for 2018
  //==== Decimal without H/E and EM+Had_depth1 = (4096-1) - (1<<6) - (1<<8) = 3775
  if(! ( (HEEPcutbit&3775)==3775 ) ) return false;

  //==== new cutd : https://indico.cern.ch/event/831669/contributions/3485543/attachments/1871797/3084930/ApprovalSlides_EE_v3.pdf, page 9

  //==== new H/E cut
//double cutValue_HoverE =                                      5 / scE() + 0.05; // original cut
  double cutValue_HoverE = ( -0.4 + 0.4 * fabs(scEta()) ) * Rho() / scE() + 0.05;
  if(! (HoverE()<cutValue_HoverE) ) return false;

  //==== new EM+Had_depth1 cut
//double cutValue_emhaddep1 = UncorrPt() > 50. ? 2.5 + 0.03 * (UncorrPt()-50.) +                        0.28 * Rho() : 2.5 +                        0.28 * Rho(); // original cut
  double cutValue_emhaddep1 = UncorrPt() > 50. ? 2.5 + 0.03 * (UncorrPt()-50.) + (0.15 + 0.07*fabs(scEta())) * Rho() : 2.5 + (0.15 + 0.07*fabs(scEta())) * Rho();
  if(! ( dr03EcalRecHitSumEt() + dr03HcalDepth1TowerSumEt() < cutValue_emhaddep1 ) ) return false;

  return true;
}

//===============================================
//==== 2016 ID
//===============================================

bool Electron::Pass_TriggerEmulation() const{
  // Trigger emulation (See https://twiki.cern.ch/twiki/bin/viewauth/CMS/SUSLeptonSF#ID_IP_ISO_AN1)
  // Cuts (IdL, IdM) in single electron triggers
  // No Iso cuts in the trigger with IdM

  if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;    // < 0.5
  if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;    // < 0.3
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2

  if( fabs(scEta()) <= 1.479 ){
    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;       // < 0.013, 0.011
    if(! (fabs(dEtaSeed()) < 0.005) ) return false;              // < 0.01 , 0.006
    if(! (fabs(dPhiIn()) < 0.04) ) return false;                 // < 0.07 , 0.15
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.12 
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }
  else{
    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;       // < 0.035, 0.031
    if(! (fabs(dEtaSeed()) < 0.007) ) return false;              // < 0.015, 0.0085
    if(! (fabs(dPhiIn()) < 0.04) ) return false;                 // < 0.1  , 0.05
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.1
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }

  return true;
}

bool Electron::Pass_HNVeto2016() const{
  if( fabs(scEta()) <= 0.8 ){
    if(! (MVANoIso()>-0.1) ) return false;
  }
  else if( fabs(scEta()) > 0.8 && fabs(scEta()) <= 1.479 ){
    if(! (MVANoIso()>0.1) ) return false;
  }
  else{
    if(! (MVANoIso()>-0.1) ) return false;
  }
  if(! (fabs(dXY())<0.2 && fabs(dZ())<0.5) ) return false;
  if(! (RelIso()<0.6) ) return false;

  return true;
}

bool Electron::Pass_HNLoose2016() const{
  if( fabs(scEta()) <= 0.8 ){
    if(! (MVANoIso()>-0.1) ) return false;
  }
  else if( fabs(scEta()) > 0.8 && fabs(scEta()) <= 1.479 ){
    if(! (MVANoIso()>0.1) ) return false;
  }
  else{
    if(! (MVANoIso()>-0.1) ) return false;
  }
  if(! (fabs(dXY())<0.2 && fabs(dZ())<0.1 && fabs(IP3D()/IP3Derr())<10.) ) return false;
  if(! (RelIso()<0.6) ) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;

  // Trigger emulation
  if(! (dr03TkSumPt()/UncorrPt() < 0.18) ) return false;      
  if( fabs(scEta()) <= 1.479 ){
    if(! (Full5x5_sigmaIetaIeta() < 0.012) ) return false;   
    if(! (fabs(dEtaSeed()) < 0.0095) ) return false;          
    if(! (fabs(dPhiIn()) < 0.065) ) return false;             
    if(! (HoverE() < 0.09) ) return false;                   
    if(! (ecalPFClusterIso()/UncorrPt() < 0.37) ) return false;
    if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;
  }
  else{
    if(! (Full5x5_sigmaIetaIeta() < 0.033) ) return false;   
    if(! (HoverE() < 0.09) ) return false;                   
    if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;
    if(! (hcalPFClusterIso()/UncorrPt() < 0.28) ) return false;
  }

  return true;
}

bool Electron::Pass_HNTight2016() const{
  if( fabs(scEta()) <= 0.8 ){
    if(! (MVANoIso()>0.9) ) return false;
  }
  else if( fabs(scEta()) > 0.8 && fabs(scEta()) <= 1.479 ){
    if(! (MVANoIso()>0.825) ) return false;
  }
  else{
    if(! (MVANoIso()>0.5) ) return false;
  }
  if(! (fabs(dXY())<0.01 && fabs(dZ())<0.04 && fabs(IP3D()/IP3Derr())<4.) ) return false;
  if(! (RelIso()<0.08) ) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;

  // Trigger emulation
  if(! (dr03TkSumPt()/UncorrPt() < 0.18) ) return false;      
  if( fabs(scEta()) <= 1.479 ){
    if(! (Full5x5_sigmaIetaIeta() < 0.012) ) return false;   
    if(! (fabs(dEtaSeed()) < 0.0095) ) return false;          
    if(! (fabs(dPhiIn()) < 0.065) ) return false;             
    if(! (HoverE() < 0.09) ) return false;                   
    if(! (ecalPFClusterIso()/UncorrPt() < 0.37) ) return false;
    if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;
  }
  else{
    if(! (Full5x5_sigmaIetaIeta() < 0.033) ) return false;   
    if(! (HoverE() < 0.09) ) return false;                   
    if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;
    if(! (hcalPFClusterIso()/UncorrPt() < 0.28) ) return false;
  }

  return true;
}

//===============================================
//==== Run2 ID
//===============================================
bool Electron::Pass_HNLoose(double relisoCut, double dxyCut, double dzCut, double sipCut, bool isPOGIP) const{
  if(! (Pass_CutBasedLooseNoIso()) ) return false;
  if(! (RelIso()<relisoCut) ) return false;
  if( fabs(scEta()) <= 1.479 ){
    if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
  }
  else{
    if(isPOGIP){
      if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
    }
    else{
      if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
    }
  }
  if(! (fabs(IP3D()/IP3Derr())<sipCut) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  if(! (Pass_TriggerEmulation()) ) return false;
  return true;
}

bool Electron::Pass_HNLooseV21() const{
  if(! (RelIso()<0.6) ) return false;
  if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;    // < 0.5
  if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;    // < 0.3
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2
  if( fabs(scEta()) <= 1.479 ){                                                   // original values in the POG Loose ID
    if(! (fabs(dXY())<0.05 && fabs(dZ())<0.1) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;                        // 0.0112
    if(! (fabs(dEtaSeed()) < 0.00377) ) return false;                              
    if(! (fabs(dPhiIn()) < 0.04) ) return false;                                  // 0.0884
    if(! (HoverE() < 0.05 + 1.16/scE() + 0.0324*Rho()/scE()) ) return false;
    if(! (HoverE() < 0.08) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;                           // 0.193
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;
  }
  else{
    if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;                        // 0.0425
    if(! (fabs(dEtaSeed()) < 0.00674) ) return false;
    if(! (fabs(dPhiIn()) <  0.08) ) return false;                                 // 0.169
    if(! (HoverE() < 0.0441 + 2.54/scE() + 0.183*Rho()/scE()) ) return false;
    if(! (HoverE() < 0.08) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;                           // 0.111
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;
  }
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  return true;
}

bool Electron::Pass_HNLooseV22() const{
  if(! (RelIso()<0.6) ) return false;
  if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;    // < 0.5
  if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;    // < 0.3
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2
  if(! (fabs(dXY())<0.2 && fabs(dZ())<0.2) ) return false;
  if( fabs(scEta()) <= 1.479 ){                                                   // original values in the POG Loose ID
//    if(! (fabs(dXY())<0.05 && fabs(dZ())<0.1) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;                        // 0.0112
    if(! (fabs(dEtaSeed()) < 0.00377) ) return false;
    if(! (fabs(dPhiIn()) < 0.04) ) return false;                                  // 0.0884
    if(! (HoverE() < 0.05 + 1.16/scE() + 0.0324*Rho()/scE()) ) return false;
    if(! (HoverE() < 0.08) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;                           // 0.193
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;
  }
  else{
//    if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;                        // 0.0425
    if(! (fabs(dEtaSeed()) < 0.00674) ) return false;
    if(! (fabs(dPhiIn()) <  0.08) ) return false;                                 // 0.169
    if(! (HoverE() < 0.0441 + 2.54/scE() + 0.183*Rho()/scE()) ) return false;
    if(! (HoverE() < 0.08) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;                           // 0.111
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;
  }
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  return true;
}

bool Electron::Pass_HNLooseV23() const{
  if(! (RelIso()<0.6) ) return false;
  if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;    // < 0.5
  if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;    // < 0.3
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2
  if( fabs(scEta()) <= 1.479 ){                                                   // original values in the POG Loose ID
//    if(! (fabs(dXY())<0.05 && fabs(dZ())<0.1) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;                        // 0.0112
    if(! (fabs(dEtaSeed()) < 0.00377) ) return false;
    if(! (fabs(dPhiIn()) < 0.04) ) return false;                                  // 0.0884
    if(! (HoverE() < 0.05 + 1.16/scE() + 0.0324*Rho()/scE()) ) return false;
    if(! (HoverE() < 0.08) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;                           // 0.193
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;
  }
  else{
//    if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;                        // 0.0425
    if(! (fabs(dEtaSeed()) < 0.00674) ) return false;
    if(! (fabs(dPhiIn()) <  0.08) ) return false;                                 // 0.169
    if(! (HoverE() < 0.0441 + 2.54/scE() + 0.183*Rho()/scE()) ) return false;
    if(! (HoverE() < 0.08) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;                           // 0.111
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;
  }
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  return true;
}

bool Electron::Pass_HNTight(double chargeCut, double dxyCut, double dzCut, double sipCut, bool isPOGIP) const{
  if(! (passTightID()) ) return false;
  //if(! (RelIso()<relisoCut) ) return false;
  if( fabs(scEta()) <= 1.479 ){
    if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
  }
  else{
    if(isPOGIP){
      if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
    }
    else{
      if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
    }
  }
  if(! (fabs(IP3D()/IP3Derr())<sipCut) ) return false;
  if(UncorrPt() < chargeCut){
    if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  }
  if(! (Pass_TriggerEmulation()) ) return false;
  return true;
}

bool Electron::Pass_HNTight(double dxyCut, double dzCut, double sipCut, bool isPOGIP) const{
  if(! (passTightID()) ) return false;
  //if(! (RelIso()<relisoCut) ) return false;
  if( fabs(scEta()) <= 1.479 ){
    if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
  }
  else{
    if(isPOGIP){
      if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
    }
    else{
      if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
    }
  }
  if(! (fabs(IP3D()/IP3Derr())<sipCut) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  if(! (Pass_TriggerEmulation()) ) return false;
  return true;
}

bool Electron::SSWW_loose2016() const{

  if( fabs(scEta()) <= 1.479 ){
    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;       // < 0.013, 0.011
    if(! (fabs(dEtaSeed()) < 0.004) ) return false;              // < 0.01 , 0.006
    if(! (fabs(dPhiIn()) < 0.02) ) return false;                 // < 0.07 , 0.15
    if(! (HoverE() < 0.06) ) return false;                       // < 0.13 , 0.12 
    if(! (fabs(InvEminusInvP()) < 0.013) ) return false;          // < 9999., 0.05
    if(! (ecalPFClusterIso()/UncorrPt() < 0.16) ) return false;    // < 0.5
    if(! (hcalPFClusterIso()/UncorrPt() < 0.12) ) return false;    // < 0.3
    if(! (dr03TkSumPt()/UncorrPt() < 0.08) ) return false;          // < 0.2
    if(! (fabs(dXY()) < 0.05) ) return false;
    if(! (fabs(dZ()) < 0.1) ) return false;
  }
  else{
    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;       // < 0.035, 0.031
    if(! (HoverE() < 0.065) ) return false;                       // < 0.13 , 0.1
    if(! (fabs(InvEminusInvP()) < 0.013) ) return false;          // < 9999., 0.05
    if(! (ecalPFClusterIso()/UncorrPt() < 0.12) ) return false;    // < 0.5
    if(! (hcalPFClusterIso()/UncorrPt() < 0.12) ) return false;    // < 0.3
    if(! (dr03TkSumPt()/UncorrPt() < 0.08) ) return false;          // < 0.2
    if(! (fabs(dXY()) < 0.1) ) return false;
    if(! (fabs(dZ()) < 0.2) ) return false;
  } // JH : we don't have gsf chi2() and ndof() in SKFlat

  if(! (NMissingHits()<=1) ) return false;

  return true;
}

bool Electron::SSWW_tight2016() const{
  if(! (passTightID()) ) return false;
  if(! (NMissingHits()<=1) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;

  if( fabs(scEta()) <= 1.479 ){
    if(! (fabs(dXY()) < 0.05) ) return false;
    if(! (fabs(dZ()) < 0.1) ) return false;
  }
  else{
    if(! (fabs(dXY()) < 0.1) ) return false;
    if(! (fabs(dZ()) < 0.2) ) return false;
  }

  return true;
}

bool Electron::SSWW_loose() const{
  if(! (passMediumID()) ) return false;

  if( fabs(scEta()) <= 1.479 ){
    if(! (fabs(dXY()) < 0.05) ) return false;
    if(! (fabs(dZ()) < 0.1) ) return false;
  }
  else{
    if(! (Full5x5_sigmaIetaIeta() < 0.03) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.014) ) return false;
    if(! (fabs(dXY()) < 0.1) ) return false;
    if(! (fabs(dZ()) < 0.2) ) return false;
  }

  return true;
}

bool Electron::SSWW_tight() const{
  if(! (passMVAID_noIso_WP90()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;

  if( fabs(scEta()) <= 1.479 ){
    if(! (RelIso() < 0.0571) ) return false;
  }
  else{
    if(! (RelIso() < 0.05880) ) return false;
  }

  return true;
}

bool Electron::Pass_HNMVATight(double relisoCut, double dxyCut, double dzCut, bool isPOGIP) const{
  if(!( passMVAID_noIso_WP80() )) return false;
  if(! (RelIso()<relisoCut) ) return false;
  if( fabs(scEta()) <= 1.479 ){
    if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
  }
  else{
    if(isPOGIP){
      if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
    }
    else{
      if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
    }
  }
  if(! (PassConversionVeto()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  if(! (Pass_TriggerEmulation()) ) return false;
  return true;
}

bool Electron::Pass_HNTightV2() const{
  if(!( passTightID() )) return false;
  if(! (RelIso()<0.08) ) return false;
  if(! (fabs(dXY())<0.01 && fabs(dZ())<0.04) ) return false;
  if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;    // < 0.5
  if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;    // < 0.3
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2
  if( fabs(scEta()) <= 1.479 ){
    if(! (RelIso() < 0.0287+0.506/UncorrPt()) ) return false;    // When UncorrPt < 9.864, RelIso < 0.08
    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;       // < 0.013, 0.011
    if(! (fabs(dEtaSeed()) < 0.005) ) return false;              // < 0.01 , 0.006
    if(! (fabs(dPhiIn()) < 0.04) ) return false;                 // < 0.07 , 0.15
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.12 
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }
  else{
    if(! (RelIso() < 0.0445+0.963/UncorrPt()) ) return false;    // When UncorrPt < 27.127, RelIso < 0.08
    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;       // < 0.035, 0.031
    if(! (fabs(dEtaSeed()) < 0.007) ) return false;              // < 0.015, 0.0085
    if(! (fabs(dPhiIn()) < 0.08) ) return false;                 // < 0.1  , 0.1
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.1
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  return true;
}

bool Electron::Pass_HNMVALoose() const{
  if(!( passMVAID_noIso_WP90() )) return false;
  if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;    // < 0.5
  if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;    // < 0.3
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2
  if( fabs(scEta()) <= 1.479 ){
    if(! (RelIso() < 0.112+0.506/UncorrPt()) ) return false;
    if(! (fabs(dXY())<0.05 && fabs(dZ())<0.1) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;       // < 0.013, 0.011
    if(! (fabs(dEtaSeed()) < 0.005) ) return false;              // < 0.01 , 0.006
    if(! (fabs(dPhiIn()) < 0.04) ) return false;                 // < 0.07 , 0.15
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.12 
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }
  else{
    if(! (RelIso() < 0.108+0.963/UncorrPt()) ) return false;
    if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;       // < 0.035, 0.031
    if(! (fabs(dEtaSeed()) < 0.007) ) return false;              // < 0.015, 0.0085
    if(! (fabs(dPhiIn()) < 0.08) ) return false;                 // < 0.1  , 0.1
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.1
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }
  if(! (PassConversionVeto()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  return true;
}

bool Electron::Pass_HNMVALooseV2() const{
  if(!( passMVAID_noIso_WP90() )) return false;
  if(! (RelIso()<0.6) ) return false;
  if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;    // < 0.5
  if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;    // < 0.3
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2
  if( fabs(scEta()) <= 1.479 ){
//    if(! (RelIso() < 0.112+0.506/UncorrPt()) ) return false;
//    if(! (fabs(dXY())<0.05 && fabs(dZ())<0.1) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;       // < 0.013, 0.011
    if(! (fabs(dEtaSeed()) < 0.005) ) return false;              // < 0.01 , 0.006
    if(! (fabs(dPhiIn()) < 0.04) ) return false;                 // < 0.07 , 0.15
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.12 
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }
  else{
//    if(! (RelIso() < 0.108+0.963/UncorrPt()) ) return false;
//    if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;       // < 0.035, 0.031
    if(! (fabs(dEtaSeed()) < 0.007) ) return false;              // < 0.015, 0.0085
    if(! (fabs(dPhiIn()) < 0.08) ) return false;                 // < 0.1  , 0.1
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.1
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }
  if(! (PassConversionVeto()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  return true;
}

//bool Electron::Pass_HNMVATight() const{
//  if(!( passMVAID_noIso_WP80() )) return false;
//  if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;    // < 0.5
//  if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;    // < 0.3
//  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2
//  if( fabs(scEta()) <= 1.479 ){
//    if(! (RelIso() < 0.0287+0.506/UncorrPt()) ) return false;
//    if(! (fabs(dXY())<0.05 && fabs(dZ())<0.1) ) return false;
//    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;       // < 0.013, 0.011
//    if(! (fabs(dEtaSeed()) < 0.005) ) return false;              // < 0.01 , 0.006
//    if(! (fabs(dPhiIn()) < 0.04) ) return false;                 // < 0.07 , 0.15
//    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.12 
//    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
//  }
//  else{
//    if(! (RelIso() < 0.0445+0.963/UncorrPt()) ) return false;
//    if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
//    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;       // < 0.035, 0.031
//    if(! (fabs(dEtaSeed()) < 0.007) ) return false;              // < 0.015, 0.0085
//    if(! (fabs(dPhiIn()) < 0.08) ) return false;                 // < 0.1  , 0.1
//    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.1
//    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
//  }
//  if(! (PassConversionVeto()) ) return false;
//  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
//  return true;
//}

bool Electron::Pass_HNMVATightV2() const{
  if(!( passMVAID_noIso_WP80() )) return false;
  if(! (RelIso()<0.08) ) return false;
  if(! (fabs(dXY())<0.01 && fabs(dZ())<0.04) ) return false;
  if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;    // < 0.5
  if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;    // < 0.3
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2
  if( fabs(scEta()) <= 1.479 ){
    if(! (RelIso() < 0.0287+0.506/UncorrPt()) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;       // < 0.013, 0.011
    if(! (fabs(dEtaSeed()) < 0.005) ) return false;              // < 0.01 , 0.006
    if(! (fabs(dPhiIn()) < 0.04) ) return false;                 // < 0.07 , 0.15
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.12 
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }
  else{
    if(! (RelIso() < 0.0445+0.963/UncorrPt()) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;       // < 0.035, 0.031
    if(! (fabs(dEtaSeed()) < 0.007) ) return false;              // < 0.015, 0.0085
    if(! (fabs(dPhiIn()) < 0.08) ) return false;                 // < 0.1  , 0.1
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.1
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }
  if(! (PassConversionVeto()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  return true;
}

bool Electron::Pass_ISRVeto(double relisoCut) const{
  if(! (Pass_CutBasedVetoNoIso()) ) return false;
  if(! (RelIso()<relisoCut) ) return false;
  return true;
}

bool Electron::Pass_ISRLoose() const{
  if(! (RelIso()<0.6) ) return false;
  if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;    // < 0.5
  if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;    // < 0.3
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2
  if( fabs(scEta()) <= 1.479 ){                                                   // original values
    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;                        // 0.0112
    if(! (fabs(dEtaSeed()) < 0.00377) ) return false;
    if(! (fabs(dPhiIn()) < 0.04) ) return false;                                  // 0.0884
    if(! (HoverE() < 0.05 + 1.16/scE() + 0.0324*Rho()/scE()) ) return false;
    if(! (HoverE() < 0.08) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;                           // 0.193
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;
  }
  else{
    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;                        // 0.0425
    if(! (fabs(dEtaSeed()) < 0.00674) ) return false;
    if(! (fabs(dPhiIn()) <  0.08) ) return false;                                 // 0.169
    if(! (HoverE() < 0.0441 + 2.54/scE() + 0.183*Rho()/scE()) ) return false;
    if(! (HoverE() < 0.08) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;                           // 0.111
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;
  }
  return true;
}

/*bool Electron::Pass_ISRLoose() const{
  if(!( passLooseID() )) return false;
  if( fabs(scEta()) <= 1.479 ){
//    if(! (fabs(dXY())<0.05 && fabs(dZ())<0.1) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.012) ) return false;
    if(! (fabs(dEtaSeed()) < 0.0095) ) return false;
    if(! (fabs(dPhiIn()) < 0.065) ) return false;
    if(! (HoverE() < 0.12) ) return false;
  }
  else{
//    if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.034) ) return false;
    if(! (fabs(dEtaSeed()) < 0.0145) ) return false;
    if(! (fabs(dPhiIn()) < 0.0095) ) return false;
    if(! (HoverE() < 0.12) ) return false;
  }
//  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  return true;
}*/

bool Electron::Pass_ISRTight() const{
  if(!( passMediumID() )) return false;
  if(! (ecalPFClusterIso()/UncorrPt() < 0.45) ) return false;    // < 0.5
  if(! (hcalPFClusterIso()/UncorrPt() < 0.25) ) return false;    // < 0.3
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2
  if( fabs(scEta()) <= 1.479 ){
//    if(! (fabs(dXY())<0.05 && fabs(dZ())<0.1) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.011) ) return false;       // < 0.013, 0.011
    if(! (fabs(dEtaSeed()) < 0.005) ) return false;              // < 0.01 , 0.006
    if(! (fabs(dPhiIn()) < 0.04) ) return false;                 // < 0.07 , 0.15
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.12 
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }
  else{
//    if(! (fabs(dXY())<0.1 && fabs(dZ())<0.2) ) return false;
    if(! (Full5x5_sigmaIetaIeta() < 0.031) ) return false;       // < 0.035, 0.031
    if(! (fabs(dEtaSeed()) < 0.007) ) return false;              // < 0.015, 0.0085
    if(! (fabs(dPhiIn()) < 0.08) ) return false;                 // < 0.1  , 0.1
    if(! (HoverE() < 0.08) ) return false;                       // < 0.13 , 0.1
    if(! (fabs(InvEminusInvP()) < 0.01) ) return false;          // < 9999., 0.05
  }
//  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  return true;
}

//==== TEST ID

bool Electron::Pass_TESTID() const{
  return true;
}



bool Electron::Pass_CutBasedLooseNoIso() const{

  if( fabs(scEta()) <= 1.479 ){

    if(! (Full5x5_sigmaIetaIeta() < 0.0112) ) return false;
    if(! (fabs(dEtaSeed()) < 0.00377) ) return false;
    if(! (fabs(dPhiIn()) < 0.0884) ) return false;
    if(! (HoverE() < 0.05 + 1.16/scE() + 0.0324*Rho()/scE()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.193) ) return false;
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;

    return true;

  }
  else{

    if(! (Full5x5_sigmaIetaIeta() < 0.0425) ) return false;
    if(! (fabs(dEtaSeed()) < 0.00674) ) return false;
    if(! (fabs(dPhiIn()) <  0.169 ) ) return false;
    if(! (HoverE() < 0.0441 + 2.54/scE() + 0.183*Rho()/scE()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.111) ) return false;
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;

    return true;

  }

}

bool Electron::Pass_CutBasedVetoNoIso() const{
  
  if( fabs(scEta()) <= 1.479 ){
    
    if(! (Full5x5_sigmaIetaIeta() < 0.0126 ) ) return false;
    if(! (fabs(dEtaSeed()) < 0.00463 ) ) return false;
    if(! (fabs(dPhiIn()) < 0.148) ) return false;
    if(! (HoverE() < 0.05 + 1.16/scE() + 0.0324*Rho()/scE()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.209) ) return false;
    if(! (NMissingHits() <= 2) ) return false;
    if(! (PassConversionVeto()) ) return false;
    
    return true;
  
  }
  else{
    
    if(! (Full5x5_sigmaIetaIeta() < 0.0457) ) return false;
    if(! (fabs(dEtaSeed()) < 0.00814) ) return false;
    if(! (fabs(dPhiIn()) < 0.19) ) return false;
    if(! (HoverE() < 0.05 + 2.54/scE() + 0.183*Rho()/scE()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.132) ) return false;
    if(! (NMissingHits() <= 3) ) return false;
    if(! (PassConversionVeto()) ) return false;
    
    return true;
  
  }

}

bool Electron::Pass_CutBasedLoose() const{

  if( fabs(scEta()) <= 1.479 ){

    if(! (Full5x5_sigmaIetaIeta() < 0.0112) ) return false;
    if(! (fabs(dEtaSeed()) < 0.00377) ) return false;
    if(! (fabs(dPhiIn()) < 0.0884) ) return false;
    if(! (HoverE() < 0.05 + 1.16/scE() + 0.0324*Rho()/scE()) ) return false;
    if(! (RelIso() < 0.112+0.506/UncorrPt()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.193) ) return false;
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;

    return true;

  }
  else{

    if(! (Full5x5_sigmaIetaIeta() < 0.0425) ) return false;
    if(! (fabs(dEtaSeed()) < 0.00674) ) return false;
    if(! (fabs(dPhiIn()) <  0.169 ) ) return false;
    if(! (HoverE() < 0.0441 + 2.54/scE() + 0.183*Rho()/scE()) ) return false;
    if(! (RelIso() < 0.108+0.963/UncorrPt()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.111) ) return false;
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;

    return true;

  }

}

bool Electron::Pass_CutBasedVeto() const{

  if( fabs(scEta()) <= 1.479 ){

    if(! (Full5x5_sigmaIetaIeta() < 0.0126 ) ) return false;
    if(! (fabs(dEtaSeed()) < 0.00463 ) ) return false;
    if(! (fabs(dPhiIn()) < 0.148) ) return false;
    if(! (HoverE() < 0.05 + 1.16/scE() + 0.0324*Rho()/scE()) ) return false;
    if(! (RelIso() < 0.198+0.506/UncorrPt()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.209) ) return false;
    if(! (NMissingHits() <= 2) ) return false;
    if(! (PassConversionVeto()) ) return false;

    return true;

  }
  else{

    if(! (Full5x5_sigmaIetaIeta() < 0.0457) ) return false;
    if(! (fabs(dEtaSeed()) < 0.00814) ) return false;
    if(! (fabs(dPhiIn()) < 0.19) ) return false;
    if(! (HoverE() < 0.05 + 2.54/scE() + 0.183*Rho()/scE()) ) return false;
    if(! (RelIso() < 0.203+0.963/UncorrPt()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.132) ) return false;
    if(! (NMissingHits() <= 3) ) return false;
    if(! (PassConversionVeto()) ) return false;

    return true;

  }

}

void Electron::SetRho(double r){
  j_Rho = r;
}

void Electron::SetIsGsfCtfScPixChargeConsistent(bool b){
  j_isGsfCtfScPixChargeConsistent = b;
}
