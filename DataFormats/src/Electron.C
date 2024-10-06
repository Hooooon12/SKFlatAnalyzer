#include "Electron.h"
#include <iostream>
#include <sstream>
#include <string>

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
  j_sigmaIetaIeta = -999.;
  j_dEtaSeed = -999.;
  j_dPhiIn = -999.;
  j_dEtaIn = -999.;
  j_HoverE  = -999.;
  j_PhiWidth = -999.;
  j_EtaWidth = -999.;
  
  j_fbrem = -999;
  j_eoverp = -999;
  j_InvEminusInvP = -999.;
  j_e2x5OverE5x5 = -999.;
  j_e1x5OverE5x5 = -999.;
  j_e15 = -999.;
  j_e25 = -999.;
  j_e55 = -999.;
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
  j_isGsfScPixChargeConsistent = false;
  j_isGsfCtfChargeConsistent = false;
  j_psEoverEraw=0;
  this->SetLeptonFlavour(ELECTRON);

}


Electron::~Electron(){

}




/*
  ELECTRON Set Functions
  
 */
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
  //j_trkiso = trackIso;
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


void Electron::SetFBrem(double d){
  j_fbrem= d;
}


void Electron::SetEOverP(double d){
  j_eoverp= d;
}


void Electron::SetEtaWidth(double d){
  j_EtaWidth= d;
}

void Electron::SetPhiWidth(double d){
  j_PhiWidth= d;
}

void Electron::SetDetaIn(double d){
  j_dEtaIn= d;
}

void Electron::SetSigmaIEtaIE(double d){
  j_sigmaIetaIeta= d;
}

void Electron::SetE15(double d){
  j_e15= d;
}

void Electron::SetE25(double d){
  j_e25= d;
}

void Electron::SetE55(double d){
  j_e55= d;
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


/*
  ELECTRON ID Functions
*/

bool Electron::PassID(TString ID) const{

  /*
    PassID is main ID function used in analyser. Calles other PassID* functions

  */

  // === list of IDs for analyis
  if(ID=="NoCut") return true;
  if(ID=="NOCUT") return true;
  
  //==== XXX veto Gap Always
  if(etaRegion()==GAP) return false;

  //==== Customized
  if(ID=="TEST") return Pass_TESTID();

  /// PassStandardIDs contains POG + HNL confirmed IDs
  //==== POG                                                                                                  
  //======== VETO IDs                                                                                          
  if(ID=="passVetoID") return passVetoID(); // --- VETO POG                                                    

  /// HNL   //==== VETO IDS                                                                                   
  if(ID=="HNVetoMVA") return Pass_HNVetoMVA(); // --- veto POG MVA (95%) + VVLoose ISO/IP                    
  if(ID=="HNVeto")    return Pass_HNVetoCutBased(0.6); // -- VETO POG + VVLoose ISO/IP                                
  if(ID=="HNLoosest") return Pass_HNLoosest(); // OR of VETO IDs  LOOSEST                                     

  if(PassIDLoose(ID)   >=0) return (PassIDLoose(ID)==1)    ? true : false;
  if(PassIDTight(ID)   >=0) return (PassIDTight(ID)==1)    ? true : false;

  cout << "[Electron::PassID] No id : " << ID << endl;
  exit(ENODATA);

  return false;

}

bool Electron::PassIDForOpt(TString ID) const{

  if(PassIDOptLoose(ID)>=0) return (PassIDOptLoose(ID)==1) ? true : false;
  if(PassIDOpt(ID)     >=0) return (PassIDOpt(ID)==1)      ? true : false;

  if(PassIDStudy(ID)   >=0) return (PassIDStudy(ID)==1)    ? true : false;
  cout << "[Electron::PassID] No id : " << ID << endl;
  exit(ENODATA);

  return false;

}




int  Electron::PassIDTight(TString ID) const{

  /// All HNL IDs in Electron_HNL.C
  
  if(PassHNLTight(ID) >=0) return (PassHNLTight(ID) == 1) ?  1 : 0;
  if(PassHNLTop(ID)   >=0) return (PassHNLTop(ID) == 1) ?  1 : 0;  
  
  //////////////////////////////////////////////////////////////////////////////////// 
  //// OTHER ANALYSES IDs / POG IDs
  //////////////////////////////////////////////////////////////////////////////////// 

  if(ID=="Peking"){
    if( Run_Year()== 2016) {

      if(! (passTightID()) ) return 0;
      if( IsBB()){
	if( !(fabs(dXY()) < 0.05 && fabs(dZ())< 0.1)) return 0;
	if(  (NMissingHits()>1) ) return 0;
      }
      else {
	if( !(fabs(dXY()) < 0.1 && fabs(dZ())< 0.2)) return 0;
	if(  (NMissingHits()>1) ) return 0;
      }
      if(! IsGsfCtfScPixChargeConsistent())  return 0;
      if(! (Pass_TriggerEmulation()) ) return 0;
      return 1;
    }
  
    else{
      if(!passMVAID_Iso_WP90()) return 0;
      if(! IsGsfCtfScPixChargeConsistent())  return 0;
      if(! (Pass_TriggerEmulation()) ) return 0;
      if(IsBB()){
	if(! (RelIso() < 0.0571 )) return 0;
      }
      else       if(! (RelIso() < 0.05880 )) return 0;
      return 1;
    }
  }
  if(ID=="Peking_FO") {
    if( Run_Year()== 2016) {
      if(! (passLooseID()) ) return 0;
      if( IsBB()){
	if( !(fabs(dXY()) < 0.05 && fabs(dZ())< 0.1)) return 0;
	if(  (NMissingHits()>1) ) return 0;
      }
      else {
	if( !(fabs(dXY()) < 0.1 && fabs(dZ())< 0.2)) return 0;
	if(  (NMissingHits()>1) ) return 0;
      }
      if(! IsGsfCtfScPixChargeConsistent())  return 0;
      if(! (Pass_TriggerEmulation()) ) return 0;
      return 1;
    }
    else{
      if(!passMVAID_noiso_WPLoose()) return 0;
      if(! IsGsfCtfScPixChargeConsistent())  return 0;
      if(! (Pass_TriggerEmulation()) ) return 0;
      if(IsBB()){
	if(! (RelIso() < 0.4 )) return 0;
      }
      else       if(! (RelIso() < 0.4 )) return 0;
      return 1;
    }  
  }
  
  if(ID=="HNTightV2")     return BtoI(passTightID_NoTightCharge() &&PassHNID()  &&(fabs(IP3D()/IP3Derr())<4.));
  if(ID=="HNTightV3")     return (PassHNOpt()==1)  ? 1 : 0 ;
  if(ID=="HNTight_17028") return BtoI(Pass_HNTight2016());
  
  // No Isoc
  if(ID=="CutBasedLooseNoIso")  return BtoI(Pass_CutBasedLooseNoIso());
  if(ID=="CutBasedMediumNoIso") return BtoI(Pass_CutBasedMediumNoIso());
  if(ID=="CutBasedTightNoIso")  return BtoI(Pass_CutBasedTightNoIso());
  if(ID=="MVALooseNoIso")       return BtoI(passMVAID_noiso_WPLoose());
  if(ID=="CutBasedVetoNoIso")   return BtoI(Pass_CutBasedVetoNoIso());
  
  //=== POG
  if(ID=="passPOGTight")        return BtoI(passTightID_NoTightCharge());
  if(ID=="passPOGMedium")       return BtoI(passMediumID_NoTightCharge());
  if(ID=="passMediumID")        return BtoI(passMediumID());
  if(ID=="passTightID")         return BtoI(passTightID());
  //=== HEEP IDS                                                                                                                                                                                       
  if(ID=="passHEEPID_v1")           {
    if(Run_Year() == 2018 ) return BtoI(passHEEP2018PromptHNL());
    else return BtoI(passHEEPIDHNL());
  }
  
  if(ID=="passHEEPID_v2") return BtoI(PassID("passHEEPID_v1") && PassID("passTriggerLoose") && PassID("passTightCharge"));
  if(ID=="passHEEPID_v3") return BtoI(PassID("passHEEPID_v1") && PassID("passTriggerLoose") && PassID("passMediumCharge"));

  if(ID=="passMediumCharge") return IsGsfCtfChargeConsistent();
  if(ID=="passTightCharge")  return IsGsfCtfScPixChargeConsistent();
  if(ID=="passTriggerLoose") return Pass_TriggerEmulationLoose();
  
  
  //=== MVA

  if(ID=="passMVAID_noIso_WP90Opt") return BtoI(passMVAID_noIso_WP90()) ;
  if(ID=="passMVAID_noIso_WP80Opt") return BtoI(passMVAID_noIso_WP80()) ;
  if(ID=="passMVAID_noIso_WP80")    return BtoI(passMVAID_noIso_WP80HN()) ;
  if(ID=="passMVAID_noIso_WP90")    return BtoI(passMVAID_noIso_WP90HN());
  if(ID=="passMVAID_Iso_WP80")      return BtoI(passMVAID_Iso_WP80HN());
  if(ID=="passMVAID_Iso_WP90")      return BtoI(passMVAID_Iso_WP90HN());
  
  //==== Customized                                                                                                                                                                                  
  if(ID=="SUSYTight") return BtoI(Pass_SUSYTight());
  if(ID=="SUSYLoose") return BtoI(Pass_SUSYLoose());
  

  // ===== Type-1    

  if(ID=="TightTriggerTight")                 {
    if(! passTightID_NoTightCharge()) return 0;
    if(! (Pass_TriggerEmulation()) ) return 0;
    return 1.;
  }
  if(ID=="TriggerTight")                 {
    if(! (Pass_TriggerEmulation()) ) return 0;
    return 1.;
  }
  if(ID=="TriggerLoose")                 {
    if(! (Pass_TriggerEmulationLoose()) ) return 0;
    return 1.;
  }


  if(ID=="TightTriggerLoose")                 {
    if(! passTightID_NoTightCharge()) return 0;
    if(! (Pass_TriggerEmulationLoose()) ) return 0;
    return 1.;
  }
  if(ID=="CC")                 {
    if(! passTightID_NoTightCharge()) return 0;
    if(! (Pass_TriggerEmulationLoose()) ) return 0;
    if(! IsGsfCtfScPixChargeConsistent())  return 0;
    
    return 1.;
  }
 

  return -1;

}

bool Electron::PassMVABaseLine() const{
  
  if(!PassConversionVeto() ) return false;
  if(!Pass_LepMVAID())       return false;
  return true;
}

bool Electron::PassMVABaseLineTrkIso() const{

  if(!PassConversionVeto() ) return false;
  if(!Pass_LepMVAID_TrkIso())       return false;
  return true;
}


int  Electron::PassIDLoose(TString ID) const{
    
  if(ID=="PassMVABaseLine")          return  BtoI(Pass_LepMVAID());
  if(ID=="PassMVABaseLine_TrkIso")   return  BtoI(Pass_LepMVAID_TrkIso());

  if(ID=="MVAID")        return  BtoI(Pass_LepMVAID());
  if(ID=="TopMVAID")     return  BtoI(Pass_LepMVATopID());
  if(ID=="MVAID_TrkIso") return  BtoI(Pass_LepMVAID_TrkIso());

  if(ID=="passProbeID")  return BtoI(passMVAID_noiso_WPLoose());
  //=== POG
  if(ID=="passProbeIDTight") return BtoI(passMVAID_noiso_WPLoose()&&Pass_TriggerEmulationLoose());

  if(ID=="passLooseID")  return BtoI(passLooseID());

  if(ID=="passHEEPLoose_v1") return BtoI(passLooseHEEPID());
  if(ID=="passHEEPLoose_v2") return BtoI(passLooseHEEPID() && Pass_TriggerEmulationLoose()&&PassConversionVeto() && IsGsfCtfScPixChargeConsistent());
  if(ID=="passHEEPLoose_v3") return BtoI(passLooseHEEPID() && Pass_TriggerEmulationLoose()&&PassConversionVeto() && IsGsfCtfChargeConsistent());

  //=== loose user                                                                                                                                                                                       
  if(ID=="HNLooseMVA") return BtoI(passIDHN(3,0.2, 0.2, 0.2,0.2, 10.,10., 0.6, 0.6, -999., -999.)&&passMVAID_noiso_WPLoose());  
  if(ID=="HNLooseV1")  return BtoI(Pass_HNLooseID(0.6,0.2,0.1,10));  // V POG IP/ISO   17028 IP                                                                                                
  if(ID=="HNLooseV2")  return BtoI(Pass_HNLooseID(0.6,0.2,0.1,9999));  // V POG IP/ISO                                                                                                                  
  if(ID=="HNLooseV3")  return BtoI(Pass_HNLooseID(0.6,0.02,0.1,4.));  // V POG IP/ISO                                                                                                                    
  if(ID=="HNLooseV4")   {
    if( IsBB())   return  BtoI(Pass_HNLooseID(0.6,0.05,0.1,4.));  // non POG IP/ISO     
    else  return  BtoI(Pass_HNLooseID(0.6,0.1,0.2,4.)); 
  }

  if(ID=="HNLooseV4_LooseIP")   return  BtoI(Pass_HNLooseID(0.6,0.2,0.2,10.));  // non POG IP/ISO                                        

  if(ID=="HNLoosePOG")   {
    if( IsBB()) return  BtoI(Pass_HNLooseID(0.6,0.05,0.1,999.));  // non POG IP/ISO                                                                           
    else  return  BtoI(Pass_HNLooseID(0.6,0.1,0.2,999.) ); 
  }
  
  if(ID=="HNLoosePOG_LooseIP")   return  BtoI(Pass_HNLooseID(0.6,0.2,0.2,999.));  // non POG IP/ISO                                        

  // ===== Type-1                                                                                                                                                                                     
  if(ID=="HNLoose2016")     return BtoI(passIDHN(3,0.01, 0.01, 0.04,0.04, 4.,4., 0.6, 0.6, -999., -999.)&&passMVAID_noiso_WPLoose()); 
  
  
  
  if(ID=="passLooseID")     return BtoI(passLooseIDHN() );  // non POG IP/ISO Pass_HNLooseID(0.6,0.2,0.2,10.);                                          
  return -1;
}


int  Electron::PassIDOptLoose(TString ID) const{
  
  if(ID=="passMVAID_noIso_WP80Loose")   return BtoI(passMVAID_noIso_WP80HNLoose()) ;
  if(ID=="passMVAID_noIso_WP90Loose")   return BtoI(passMVAID_noIso_WP90HNLoose());
  if(ID=="passMVAID_Iso_WP80Loose")     return BtoI(passMVAID_Iso_WP80HNLoose());
  if(ID=="passMVAID_Iso_WP90Loose")     return BtoI(passMVAID_Iso_WP90HNLoose());
  if(ID=="passMVAID_noIso_WP90LooseV2") return BtoI(Pass_HNLoose2016MVANonIso(0.6,0.02,0.05,5));
  if(ID=="HNMVALoose") return BtoI(Pass_HNMVALoose(0.6, 999., 999.)) ;

  if(ID=="HNLoose_17028")          return BtoI(Pass_HNLoose2016(0.6, 0.2, 0.1, 10.));
  if(ID=="HNLoose_17028_TightIP")  return BtoI(Pass_HNLoose2016(0.6, 0.01, 0.04, 4.)) ;
  if(ID=="HNLoose_17028_TOP")      return BtoI(Pass_HNLoose2016(0.6, 0.02, 0.04, 4.));


  return -1;



}

bool Electron::passLooseHEEPID() const{

  //  int HEEPcutbit = IDCutBit().at(11);
  //==  H/E and EM+Had_depth1 = (4096-1) - (1<<6) - (1<<8) - (1<<7) - (1<<9) = 3135
  //== ADD (1<<0) - (1<<2) - (1<<3) - (1<<4)  

  if(!isEcalDriven()) return false;
  if(! (dXY()<0.05) ) return false;
  if(NMissingHits() > 1) return false;

  //  if(! ( (HEEPcutbit&3135)==3135 ) ) return false;
  //  if(! (HoverE()<0.13) ) return false; --> In trigger emulation cuts
  if(! (TrkIso()<20.) ) return false;
  //  if(! ( dr03EcalRecHitSumEt() + dr03HcalDepth1TowerSumEt() < 8. ) ) return false;     

  return true;
  
}

bool Electron::passLooseHEEPID_NoMinPt() const{

  int HEEPcutbit = IDCutBit().at(11);


  //==  H/E and EM+Had_depth1 AND NoMinPt = (4096-1) - (1<<6) - (1<<8) - (1<<7) - (1<<9) - (1<<0) = 3134
  if(! ( (HEEPcutbit&3134)==3134 ) ) return false;
  if(! (HoverE()<0.13) ) return false;
  if(! (TrkIso()<20.) ) return false;
  if(! (dXY()<0.1) ) return false;
  if(! ( dr03EcalRecHitSumEt() + dr03HcalDepth1TowerSumEt() < 8. ) ) return false;     

  return true;
}
  
bool Electron::passHEEPIDHNL() const {
  int HEEPcutbit = IDCutBit().at(11);

  /// Remove Pt cut only
  if(! ( (HEEPcutbit&4094)==4094 ) ) return false;
  return true;
}

bool Electron::passHEEP2018PromptHNL() const {

  //==== If not endcap, use original function                                                                                                                     
  if( fabs(scEta()) < 1.566 ) return passHEEPIDHNL();

  //==== https://github.com/CMSSNU/SKFlatMaker/blob/Run2Legacy_v4/SKFlatMaker/python/SKFlatMaker_cfi.py#L37-L50                                                   
  int HEEPcutbit = IDCutBit().at(11);

  //==== https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Applying_Individual_Cuts_of_a_Se                                             
  //==== We will modify H/E (bit nr=6) and EM+Had_depth1 (bit nr=8) isolation cut for EndCap for 2018                                                             
  //==== Decimal without H/E and EM+Had_depth1 = (4096-1) - (1<<6) - (1<<8) - (1<<0))= 3774                                                                                
  if(! ( (HEEPcutbit&3774)==3774 ) ) return false;

  //==== new cutd : https://indico.cern.ch/event/831669/contributions/3485543/attachments/1871797/3084930/ApprovalSlides_EE_v3.pdf, page 9                       

  //==== new H/E cut                                                                                                                                              
  //double cutValue_HoverE =                                      5 / scE() + 0.05; // original cut                                                                 
  double cutValue_HoverE = ( -0.4 + 0.4 * fabs(scEta()) ) * Rho() / scE() + 0.05;
  if(! (HoverE()<cutValue_HoverE) ) return false;

  //==== new EM+Had_depth1 cut                                                                                                                                    
  //double cutValue_emhaddep1 = UncorrPt() > 50. ? 2.5 + 0.03 * (UncorrPt()-50.) +                        0.28 * Rho() : 2.5 +                        0.28 * Rho();
  
  // original cut                                                                                                                                                  
  
  double cutValue_emhaddep1 = UncorrPt() > 50. ? 2.5 + 0.03 * (UncorrPt()-50.) + (0.15 + 0.07*fabs(scEta())) * Rho() : 2.5 + (0.15 + 0.07*fabs(scEta())) * Rho();
  if(! ( dr03EcalRecHitSumEt() + dr03HcalDepth1TowerSumEt() < cutValue_emhaddep1 ) ) return false;

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


bool Electron::Pass_CaloIdL_TrackIdL_IsoVL(TString Option) const{
  
  bool ApplyEA = !Option.Contains("NoEA");
  if( fabs(scEta()) <=1.479 ){
    if(! (Full5x5_sigmaIetaIeta() < 0.013) ) return false;
    if(! (fabs(dEtaSeed()) < 0.01) ) return false;
    if(! (fabs(dPhiIn()) < 0.07) ) return false;
    if(! (HoverE() < 0.13) ) return false;
    if(! (max(0.,ecalPFClusterIso()-Rho()*(ApplyEA? 0.16544:0.)) < 0.5*Pt()) ) return false;
    if(! (max(0.,hcalPFClusterIso()-Rho()*(ApplyEA? 0.05956:0.)) < 0.3*Pt()) ) return false;
    if(! (dr03TkSumPt() < 0.2*Pt()) ) return false; 
  }
  else{
    if(! (Full5x5_sigmaIetaIeta() < 0.035) ) return false;
    if(! (fabs(dEtaSeed()) < 0.015) ) return false;
    if(! (fabs(dPhiIn()) < 0.1) ) return false;
    if(! (HoverE() < 0.13) ) return false;
    if(! (max(0.,ecalPFClusterIso()-Rho()*(ApplyEA? 0.13212:0.)) < 0.5*Pt()) ) return false;
    if(! (max(0.,hcalPFClusterIso()-Rho()*(ApplyEA? 0.13052:0.)) < 0.3*Pt()) ) return false;
    if(! (dr03TkSumPt() < 0.2*Pt()) ) return false; 
  }

  return true;
  //1-a) in menu of some periods, dEtaInSeed and dPhiIn cuts are not applied in EE.
  //1-b) EA is tightened from the later runs of 2016 to 2018 (i.e. higher EA)
  //- the cuts are set as tighter one among them.
  //2) cuts are a bit tighter in HctoWA, as it is higher in trig eff. but does not reduce sig eff much.
  //   but due to detector degradation, it start to hurt the sig. eff. so set the values as cuts in menu.
}

//===============================================
//==== 2016 ID
//===============================================

bool Electron::Pass_TriggerEmulationLoose() const{
  // Trigger emulation (See https://twiki.cern.ch/twiki/bin/viewauth/CMS/SUSLeptonSF#ID_IP_ISO_AN1)                                                           
  // Cuts (IdL, IdM) in single electron triggers                                                                                                              
  // No Iso cuts in the trigger with IdM                                                                                                                      

  if(! (ecalPFClusterIso()/UncorrPt() < 0.5) ) return false;    // < 0.5                                                                                     
  if(! (hcalPFClusterIso()/UncorrPt() < 0.3) ) return false;    // < 0.3                                                                                     
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2                                                                                     

  if( fabs(scEta()) <= 1.479 ){
    if(! (Full5x5_sigmaIetaIeta() < 0.013) ) return false;       // < 0.013, 0.011                                                                            
    if(! (fabs(dEtaSeed()) < 0.01) ) return false;              // < 0.01 , 0.006                                                                            
    if(! (fabs(dPhiIn()) < 0.07) ) return false;                 // < 0.07 , 0.15                                                                             
    if(! (HoverE() < 0.13) ) return false;                       // < 0.13 , 0.12                                                                             
    if(! (fabs(InvEminusInvP()) < 9999999.) ) return false;          // < 9999., 0.05                                                                             
  }
  else{
    if(! (Full5x5_sigmaIetaIeta() < 0.035) ) return false;       // < 0.035, 0.031                                                                            
    if(! (fabs(dEtaSeed()) < 0.015) ) return false;              // < 0.015, 0.0085                                                                           
    if(! (fabs(dPhiIn()) < 0.1) ) return false;                 // < 0.1  , 0.1                                                                              
    if(! (HoverE() < 0.13) ) return false;                       // < 0.13 , 0.1                                                                              
    if(! (fabs(InvEminusInvP()) < 999999.) ) return false;          // < 9999., 0.05                                                                             
  }

  return true;
}

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
    if(! (fabs(dPhiIn()) < 0.07) ) return false;                 // < 0.07 , 0.15
    if(! (HoverE() < 0.1) ) return false;                       // < 0.13 , 0.12 
    if(! (fabs(InvEminusInvP()) < 0.1) ) return false;          // < 9999., 0.05
  }
  else{
    if(! (Full5x5_sigmaIetaIeta() < 0.035) ) return false;       // < 0.035, 0.031
    if(! (fabs(dEtaSeed()) < 0.008) ) return false;              // < 0.015, 0.0085
    if(! (fabs(dPhiIn()) < 0.08) ) return false;                 // < 0.1  , 0.1
    if(! (HoverE() < 0.1) ) return false;                       // < 0.13 , 0.1
    if(! (fabs(InvEminusInvP()) < 0.1) ) return false;          // < 9999., 0.05
  }

  return true;
}



bool Electron::Pass_TriggerEmulationN(int cut) const{
  // Trigger emulation (See https://twiki.cern.ch/twiki/bin/viewauth/CMS/SUSLeptonSF#ID_IP_ISO_AN1)                                                                             
  // Cuts (IdL, IdM) in single electron triggers                                                                                                                                
  // No Iso cuts in the trigger with IdM                                                                                                                                        
  if(cut==1){
    if(! (ecalPFClusterIso()/UncorrPt() < 0.5) ) return false;    // < 0.5                                                                                                        
  }
  
  if(cut==2){
    if(! (hcalPFClusterIso()/UncorrPt() < 0.3) ) return false;    // < 0.3                                                                                                       
  }
  
  if(cut==3){
    
  if(! (dr03TkSumPt()/UncorrPt() < 0.2) ) return false;          // < 0.2                                                                                                       
  }


  if( fabs(scEta()) <= 1.479 ){

    if(cut==4){
      
    if(! (Full5x5_sigmaIetaIeta() < 0.013) ) return false;       // < 0.013, 0.011                                                                                              

    }
    
    if(cut==5){
      
    if(! (fabs(dEtaSeed()) < 0.01) ) return false;              // < 0.01 , 0.006                                                                                               
    }
    if(cut==6){

    if(! (fabs(dPhiIn()) < 0.07) ) return false;                 // < 0.07 , 0.15                                                                                               
    }
    if(cut==7){

    if(! (HoverE() < 0.13) ) return false;                       // < 0.13 , 0.12                                                                                               
    }
    if(cut==8){

    if(! (fabs(InvEminusInvP()) < 0.1) ) return false;          // < 9999., 0.05                                                                                                
    }

  }
  else{

    if(cut==9){

    if(! (Full5x5_sigmaIetaIeta() < 0.035) ) return false;       // < 0.035, 0.031                                                                                              
    }
    if(cut==10){

    if(! (fabs(dEtaSeed()) < 0.015) ) return false;              // < 0.015, 0.0085                                                                                             
    }
    if(cut==11){

    if(! (fabs(dPhiIn()) < 0.1) ) return false;                 // < 0.1  , 0.1                                                                                                 
    }
    if(cut==12){

    if(! (HoverE() < 0.13) ) return false;                       // < 0.13 , 0.1                                                                                                
    }
    if(cut==13){

    if(! (fabs(InvEminusInvP()) < 0.1) ) return false;          // < 9999., 0.05                                                                                                
    }
    
  }
  
  return true;
}


bool Electron::passMediumID_NoTightCharge() const {
  if(! (passMediumID()) ) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()) ) return false;
  if( fabs(scEta())<= 1.479 ){
    if( !(fabs(dXY()) < 0.05 && fabs(dZ())< 0.1)) return false;
  }
  else   if( !(fabs(dXY()) < 0.1 && fabs(dZ())< 0.2)) return false;


  return true;
}

bool Electron::passTightID_NoTightCharge() const {
  if(! (passTightID()) ) return false;
  if( fabs(scEta())<= 1.479 ){
    if( !(fabs(dXY()) < 0.05 && fabs(dZ())< 0.1)) return false;
  }
    else   if( !(fabs(dXY()) < 0.1 && fabs(dZ())< 0.2)) return false;

  
  return true;
}

bool Electron::passTightID_NoTightChargeB() const {

  if(! (passTightID()) ) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()) ) return false;
  if(! IsGsfCtfScPixChargeConsistent()) return false;
  if( fabs(scEta())<= 1.479 ){
    if( !(fabs(dXY()) < 0.05 && fabs(dZ())< 0.1)) return false;
  }
  else   if( !(fabs(dXY()) < 0.1 && fabs(dZ())< 0.2 )) return false;

  return true;
}
bool Electron::passLooseID_NoTightCharge() const {

  if(! (Pass_CutBasedVetoNoIso()) ) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()) ) return false;
  if( !(fabs(dXY()) < 0.2 && fabs(dZ())< 0.2 )) return false;
  if(! (RelIso()<0.6) ) return false;

  return true;
}


bool Electron::passMVAID_noIso_WP80HN() const {
  if(!(passMVAID_noIso_WP80())) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()))return false;
  if(! (RelIso()<0.08) ) return false;
  if( fabs(scEta())<= 1.479 ){
    if( !(fabs(dXY()) < 0.05 && fabs(dZ())< 0.1 && fabs(IP3D()/IP3Derr()) < 4.)) return false;
  }
  else   if( !(fabs(dXY()) < 0.1 && fabs(dZ())< 0.2 && fabs(IP3D()/IP3Derr()) < 4.)) return false;
  if(! IsGsfCtfScPixChargeConsistent()) return false;

  return true;
  
}

bool Electron::passPOGMVAID_noIso_WP80HN() const {
  if(!(passMVAID_noIso_WP80())) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()))return false;
  if(! IsGsfCtfScPixChargeConsistent()) return false;
  return true;

}
bool Electron::passPOGMVAID_noIso_WP90HN() const {
  if(!(passMVAID_noIso_WP90())) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()))return false;
  if(! IsGsfCtfScPixChargeConsistent()) return false;
  return true;

}

bool Electron::passPOGMVAID_Iso_WP90HN() const {
  if(!(passMVAID_Iso_WP90())) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()))return false;
  if(! IsGsfCtfScPixChargeConsistent())  return false;
  return true;
}

bool Electron::passPOGMVAID_Iso_WP80HN() const {
  if(!(passMVAID_Iso_WP80())) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()))return false;
  if(! IsGsfCtfScPixChargeConsistent())  return false;
  return true;
}


bool Electron::passMVAID_noIso_WP80HNLoose() const {
  if(!(Pass_HNLoose2016MVANonIso(0.6, 0.2, 0.1, 10.))) return false;

  return true;
}


bool Electron::passMVAID_Iso_WP90HN() const {
  if(!(passMVAID_Iso_WP90())) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()))return false;
  if( fabs(scEta())<= 1.479 ){
    if( !(fabs(dXY()) < 0.05 && fabs(dZ())< 0.1 && fabs(IP3D()/IP3Derr()) < 4.)) return false;
  }
  else   if( !(fabs(dXY()) < 0.1 && fabs(dZ())< 0.2 && fabs(IP3D()/IP3Derr()) < 4.)) return false;

  if(! IsGsfCtfScPixChargeConsistent())  return false;
  return true;
}

bool Electron::passMVAID_Iso_WP80HN() const {
  if(!(passMVAID_Iso_WP80())) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()))return false;
  if( fabs(scEta())<= 1.479 ){
    if( !(fabs(dXY()) < 0.05 && fabs(dZ())< 0.1 && fabs(IP3D()/IP3Derr()) < 4.)) return false;
  }
  else   if( !(fabs(dXY()) < 0.1 && fabs(dZ())< 0.2 && fabs(IP3D()/IP3Derr()) < 4.)) return false;

  if(! IsGsfCtfScPixChargeConsistent())  return false;
  return true;
}

bool Electron::passMVAID_Iso_WP80HNLoose() const {
  if(!(Pass_HNLoose2016MVAISO( 0.2, 0.1, 10.))) return false;
  return true;
}

bool Electron::passMVAID_Iso_WP90HNLoose() const {
  if(!(Pass_HNLoose2016MVAISO( 0.2, 0.1, 10.))) return false;
  return true;
}


bool Electron::passMVAID_noIso_XHN(double relisoCut, double dxyCut, double dzCut, double sipCut) const {
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()))return false;
  if(relisoCut > 0){
    if(! (RelIso()<relisoCut) ) return false;
  }
  else{
    if(! (MiniRelIso()<fabs(relisoCut)) ) return false;
  }
  if( fabs(scEta())<= 1.479 ){
    if( !(fabs(dXY()) < dxyCut && fabs(dZ())< dzCut && fabs(IP3D()/IP3Derr()) < sipCut)) return false;
  }
  else   if( !(fabs(dXY()) < dxyCut && fabs(dZ())< dzCut && fabs(IP3D()/IP3Derr()) < sipCut)) return false;
  if(! IsGsfCtfScPixChargeConsistent()) return false;
  return true;

}
bool Electron::passMVAID_noIso_WP90XHN(double relisoCut, double dxyCut, double dzCut, double sipCut) const {
  if(!(passMVAID_noIso_WP90())) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()))return false;
  if(relisoCut > 0){
    if(! (RelIso()<relisoCut) ) return false;
  }
  else{
    if(! (MiniRelIso()<fabs(relisoCut)) ) return false;
  }
  if( fabs(scEta())<= 1.479 ){
    if( !(fabs(dXY()) < dxyCut && fabs(dZ())< dzCut && fabs(IP3D()/IP3Derr()) < sipCut)) return false;
  }
  else   if( !(fabs(dXY()) < dxyCut && fabs(dZ())< dzCut && fabs(IP3D()/IP3Derr()) < sipCut)) return false;
  if(! IsGsfCtfScPixChargeConsistent()) return false;
  return true;

}
bool Electron::passMVAID_noIso_WP90HN() const {
  if(!(passMVAID_noIso_WP90())) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()))return false;
  if(! (RelIso()<0.1) ) return false;
  if( fabs(scEta())<= 1.479 ){
    if( !(fabs(dXY()) < 0.02 && fabs(dZ())< 0.1 && fabs(IP3D()/IP3Derr()) < 6.)) return false;
  }
  else   if( !(fabs(dXY()) < 0.04 && fabs(dZ())< 0.2 && fabs(IP3D()/IP3Derr()) < 6.)) return false;
  if(! IsGsfCtfScPixChargeConsistent()) return false;
  return true;
  
}
bool Electron::passMVAID_noIso_WP90HNLoose() const {
  if(!(Pass_HNLoose2016MVANonIso(0.6, 0.2, 0.1, 10.))) return false;

  return true;
}


bool Electron::passLooseIDHN() const {

  if(! (Pass_CutBasedVetoNoIso()) ) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()) ) return false;
  if( !(fabs(dXY()) < 0.2 && fabs(dZ())< 0.2 && fabs(IP3D()/IP3Derr())<10.)) return false;

  if(! (IsGsfCtfScPixChargeConsistent()) )return false;

  if(! (RelIso()<0.6) ) return false;
  
  return true;
}


bool Electron::passLooseID_NoTightChargeB() const {
  return true;
}


bool Electron::PassMVA_POGUL_BB(double c, double tau, double A) const {
  
  // cut on response not log
  // https://github.com/cms-sw/cmssw/blob/master/RecoEgamma/ElectronIdentification/python/Identification/mvaElectronID_Fall17_iso_V2_cff.py
  if(c == -999) c = 6.12931925263; 
  if(tau == -999) tau = 13.281753835;
  if(A == -999) A = 8.71138432196;
  //  cout << "c = " << c  << "  tau = " << tau << " A = " << A << endl;
  double cutCategory3 =  c - exp(- this->Pt() / tau) * A;

  if( fabs(scEta()) <= 0.8 ){
    if(! (MVAIsoResponse()> cutCategory3 )) {
      //cout << "FaIL BB cutCategory3 =" << cutCategory3 << "  MVANoIsoResponse()=" << MVANoIsoResponse()   << endl;
      return false;
    }
  }
  //cout << "Pass  BB cutCategory3 =" << cutCategory3 << endl;
  return true;
}

bool Electron::PassMVA_POGUL_EB(double c, double tau, double A) const {

  // cut on response not log                                                                                                                                                                                      
  if(c == -999) c = 5.26289004857;
  if(tau == -999) tau = 13.2154971491;
  if(A == -999) A = 8.0997882835;


  double cutCategory4 = c -exp(- this->Pt() / tau) * A;

  if( fabs(scEta()) > 0.8 && fabs(scEta()) <= 1.479 ){
    if(! (MVAIsoResponse()> cutCategory4 )) {
      //cout << "FaIL EB cutCategory4 =" << cutCategory4 << " MVANoIsoResponse() = " << MVANoIsoResponse() << endl;
      return false;
    }
  }

  return true;
}


bool Electron::PassMVA_POGUL_EE(double c, double tau, double A) const {

  if(c == -999) c = 4.37338792902;
  if(tau == -999) tau= 14.0776094696;
  if(A == -999) A = 8.48513324496;

  double cutCategory5 = c -exp(- this->Pt() / tau) * A;
  
  if( fabs(scEta()) >= 1.479 ){
    if(! (MVANoIsoResponse()> cutCategory5 )) {
      //cout << "FaIL EB cutCategory5 =" << cutCategory5 << " MVANoIsoResponse() = " << MVANoIsoResponse() << endl;
      return false;
    }
  }
  return true;
}




bool Electron::PassMVA_2016(double mva1, double mva2, double mva3) const {

  if( fabs(scEta()) <= 0.8 ){
    if(! (MVANoIso()>mva1) ) return false;
  }
  else if( fabs(scEta()) > 0.8 && fabs(scEta()) <= 1.479 ){
    if(! (MVANoIso()>mva2) ) return false;
  }
  else{
    if(! (MVANoIso()>mva3) ) return false;
  }

  return true;
}



//==== TEST ID

bool Electron::Pass_TESTID() const{
  return true;
}


bool Electron::Pass_CutBasedTightNoIso() const{

  if( fabs(scEta()) <= 1.479 ){

    if(! (Full5x5_sigmaIetaIeta() < 0.0104) ) return false;
    if(! (fabs(dEtaSeed()) < 0.00255) ) return false;
    if(! (fabs(dPhiIn()) < 0.022) ) return false;
    if(! (HoverE() < 0.026 + 1.15/scE() + 0.0324*Rho()/scE()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.159) ) return false;
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;

    return true;

  }
  else{

    if(! (Full5x5_sigmaIetaIeta() < 0.0353) ) return false;
    if(! (fabs(dEtaSeed()) < 0.00501) ) return false;
    if(! (fabs(dPhiIn()) <  0.236 ) ) return false;
    if(! (HoverE() < 0.0188 + 2.06/scE() + 0.183*Rho()/scE()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.0197) ) return false;
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;

    return true;

  }
}
bool Electron::Pass_CutBasedMediumNoIso() const{

  if( fabs(scEta()) <= 1.479 ){

    if(! (Full5x5_sigmaIetaIeta() < 0.0106) ) return false;
    if(! (fabs(dEtaSeed()) < 0.0032) ) return false;
    if(! (fabs(dPhiIn()) < 0.0547) ) return false;
    if(! (HoverE() < 0.046 + 1.16/scE() + 0.0324*Rho()/scE()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.184) ) return false;
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;

    return true;

  }
  else{

    if(! (Full5x5_sigmaIetaIeta() < 0.0387) ) return false;
    if(! (fabs(dEtaSeed()) < 0.00632) ) return false;
    if(! (fabs(dPhiIn()) <  0.394 ) ) return false;
    if(! (HoverE() < 0.0275 + 2.52/scE() + 0.183*Rho()/scE()) ) return false;
    if(! (fabs(InvEminusInvP()) < 0.0721) ) return false;
    if(! (NMissingHits() <= 1) ) return false;
    if(! (PassConversionVeto()) ) return false;

    return true;

  }

}

bool Electron::Pass_CutBasedLooseNoIso() const{
  bool debug=false;
  if( fabs(scEta()) <= 1.479 ){
    if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EB" << endl;
    if(! (Full5x5_sigmaIetaIeta() < 0.0112) ) { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EB Full5x5_sigmaIetaIeta" << Full5x5_sigmaIetaIeta()   << endl; return false;}
    if(! (fabs(dEtaSeed()) < 0.00377) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EB fabs(dEtaSeed())"  << fabs(dEtaSeed()) << endl; return false;}
    if(! (fabs(dPhiIn()) < 0.0884) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EB fabs(dPhiIn())  " << fabs(dPhiIn())   << endl; return false;}
    if(! (HoverE() < 0.05 + 1.16/scE() + 0.0324*Rho()/scE()) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EB HoverE() " << HoverE() << " : " <<  0.05 + 1.16/scE() + 0.0324*Rho()/scE()  << endl; return false;}
    if(! (fabs(InvEminusInvP()) < 0.193) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EB fabs(InvEminusInvP()) " << fabs(InvEminusInvP())    << endl; return false;}
    if(! (NMissingHits() <= 1) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EB NMissingHits() " << NMissingHits()   << endl; return false;}
    if(! (PassConversionVeto()) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EB PassConversionVeto()" << PassConversionVeto()  << endl; return false;}

    return true;

  }
  else{

    if(! (Full5x5_sigmaIetaIeta() < 0.0425) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EE Full5x5_sigmaIetaIeta()" << Full5x5_sigmaIetaIeta()  << endl; return false;}
    if(! (fabs(dEtaSeed()) < 0.00674) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EE fabs(dEtaSeed())" << fabs(dEtaSeed())  << endl; return false;}
    if(! (fabs(dPhiIn()) <  0.169 ) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EE"  << endl; return false;}
    if(! (HoverE() < 0.0441 + 2.54/scE() + 0.183*Rho()/scE()) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EE HoverE() "  << HoverE()  << endl; return false;}
    if(! (fabs(InvEminusInvP()) < 0.111) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EE fabs(InvEminusInvP())"  << fabs(InvEminusInvP()) <<  endl; return false;}
    if(! (NMissingHits() <= 1) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EE NMissingHits() " << NMissingHits()   << endl; return false;}
    if(! (PassConversionVeto()) )  { if(debug) cout << "Electron::Pass_CutBasedLooseNoIso EE PassConversionVeto()" << PassConversionVeto()  << endl; return false;}

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

void Electron::SetIsGsfCtfScPixChargeConsistent(bool b, bool c, bool d){
  j_isGsfCtfScPixChargeConsistent = b;
  j_isGsfScPixChargeConsistent = c;
  j_isGsfCtfChargeConsistent = d;
}

bool Electron::PassFilter(TString filter) const{
  if( filter=="hltDiMu9Ele9CaloIdLTrackIdLElectronlegDphiFilter" ) return j_filterbits&(ULong64_t(1)<<0);
  else if( filter=="hltEGL1SingleEGOrFilter" ) return j_filterbits&(ULong64_t(1)<<1);
  else if( filter=="hltEle12CaloIdLTrackIdLIsoVLTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<2);
  else if( filter=="hltEle16Ele12Ele8CaloIdLTrackIdLDphiLeg1Filter" ) return j_filterbits&(ULong64_t(1)<<3);
  else if( filter=="hltEle16Ele12Ele8CaloIdLTrackIdLDphiLeg2Filter" ) return j_filterbits&(ULong64_t(1)<<4);
  else if( filter=="hltEle17CaloIdLTrackIdLIsoVLTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<5);
  else if( filter=="hltEle17CaloIdMGsfTrackIdMDphiFilter" ) return j_filterbits&(ULong64_t(1)<<6);
  else if( filter=="hltEle23CaloIdLTrackIdLIsoVLJet30TrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<7);
  else if( filter=="hltEle23Ele12CaloIdLTrackIdLIsoVLDZFilter" ) return j_filterbits&(ULong64_t(1)<<8);
  else if( filter=="hltEle23Ele12CaloIdLTrackIdLIsoVLTrackIsoLeg1Filter" ) return j_filterbits&(ULong64_t(1)<<9);
  else if( filter=="hltEle23Ele12CaloIdLTrackIdLIsoVLTrackIsoLeg2Filter" ) return j_filterbits&(ULong64_t(1)<<10);
  else if( filter=="hltEle27WPTightGsfTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<11);
  else if( filter=="hltEle27erWPTightGsfTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<12);
  else if( filter=="hltEle28WPTightGsfTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<13);
  else if( filter=="hltEle32L1DoubleEGWPTightGsfTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<14);
  else if( filter=="hltEle32WPTightGsfTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<15);
  else if( filter=="hltEle35noerWPTightGsfTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<16);
  else if( filter=="hltEle8CaloIdLTrackIdLIsoVLTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<17);
  else if( filter=="hltMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLDZFilter" ) return j_filterbits&(ULong64_t(1)<<18);
  else if( filter=="hltMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLElectronlegTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<19);
  else if( filter=="hltMu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVLElectronlegTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<20);
  else if( filter=="hltMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLDZFilter" ) return j_filterbits&(ULong64_t(1)<<21);
  else if( filter=="hltMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLElectronlegTrackIsoFilter" ) return j_filterbits&(ULong64_t(1)<<22);
  else if( filter=="hltMu9Ele9DZFilter" ) return j_filterbits&(ULong64_t(1)<<23);
  else{
    cout<<"[Electron::PassFilter] unknown filter "<<filter<<endl;
    exit(ENODATA);
  }
  return false;
}

bool Electron::PassPath(TString path) const{
  if( path=="HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<0);
  else if( path=="HLT_DiMu9_Ele9_CaloIdL_TrackIdL_v" ) return j_pathbits&(ULong64_t(1)<<1);
  else if( path=="HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v" ) return j_pathbits&(ULong64_t(1)<<2);
  else if( path=="HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL_v" ) return j_pathbits&(ULong64_t(1)<<3);
  else if( path=="HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30_v" ) return j_pathbits&(ULong64_t(1)<<4);
  else if( path=="HLT_Ele17_CaloIdM_TrackIdM_PFJet30_v" ) return j_pathbits&(ULong64_t(1)<<5);
  else if( path=="HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v" ) return j_pathbits&(ULong64_t(1)<<6);
  else if( path=="HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<7);
  else if( path=="HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v" ) return j_pathbits&(ULong64_t(1)<<8);
  else if( path=="HLT_Ele27_WPTight_Gsf_v" ) return j_pathbits&(ULong64_t(1)<<9);
  else if( path=="HLT_Ele27_eta2p1_WPTight_Gsf_v" ) return j_pathbits&(ULong64_t(1)<<10);
  else if( path=="HLT_Ele28_WPTight_Gsf_v" ) return j_pathbits&(ULong64_t(1)<<11);
  else if( path=="HLT_Ele32_WPTight_Gsf_L1DoubleEG_v" ) return j_pathbits&(ULong64_t(1)<<12);
  else if( path=="HLT_Ele32_WPTight_Gsf_v" ) return j_pathbits&(ULong64_t(1)<<13);
  else if( path=="HLT_Ele35_WPTight_Gsf_v" ) return j_pathbits&(ULong64_t(1)<<14);
  else if( path=="HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30_v" ) return j_pathbits&(ULong64_t(1)<<15);
  else if( path=="HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<16);
  else if( path=="HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v" ) return j_pathbits&(ULong64_t(1)<<17);
  else if( path=="HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<18);
  else if( path=="HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v" ) return j_pathbits&(ULong64_t(1)<<19);
  else if( path=="HLT_Mu8_DiEle12_CaloIdL_TrackIdL_v" ) return j_pathbits&(ULong64_t(1)<<20);
  else if( path=="HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v" ) return j_pathbits&(ULong64_t(1)<<21);
  else if( path=="HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v" ) return j_pathbits&(ULong64_t(1)<<22);
  else{
    cout<<"[Electron::PassPath] unknown path "<<path<<endl;
    exit(ENODATA);
  }
  return false;
}



double  Electron::StringToDouble(TString st,TString subSt) const{
  
  //cout << st << " " << subSt << endl;
  st = st.ReplaceAll(subSt,"");
  st = st.ReplaceAll("p",".");
  st = st.ReplaceAll("neg","-");
  //cout << st << endl;
  std::string _str = std::string(st);
  std::string::size_type sz;  
  
  double _d = std::stod (_str,&sz);

  return _d;


}




bool Electron::PassIDOpt(TString ID, bool cc, double dx_b ,double dx_e,double dz_b,double dz_e, double iso_b, double iso_e) const{

  bool pass_id= PassID(ID);
  if(!pass_id) return false;

  if( fabs(scEta()) <= 1.479 ){
    if(!( fabs(dZ()) < dz_b )) return false;
    if(!( fabs(dXY()) < dx_b )) return false;
    if(!( fabs(RelIso()) < iso_b )) return false;
  }
  else{
    if(!( fabs(dZ()) < dz_e )) return false;
    if(!( fabs(dXY()) < dx_e )) return false;
    if(!( fabs(RelIso()) < iso_e )) return false;
  }


  return true;
}
