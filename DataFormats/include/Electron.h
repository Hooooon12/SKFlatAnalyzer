#ifndef Electron_h
#define Electron_h

#include "Lepton.h"
#include "TString.h"

class Electron: public Lepton{

public:

  Electron();
  ~Electron();

  inline int BtoI(bool b) const {
    return b? 1 : 0 ;
  }

  inline TString GetCFShift_BinLabel(double shift) const{

    double pt_shifted = this->Pt() * shift;
    TString PtBinLabel  =  "";
    if(IsBB()){
      if(pt_shifted < 35)       PtBinLabel = "_Pt_Bin1";
      else  if(pt_shifted < 50)PtBinLabel = "_Pt_Bin2";
      else  if(pt_shifted < 70)PtBinLabel = "_Pt_Bin3";
      else  if(pt_shifted < 100)PtBinLabel = "_Pt_Bin4";
      else PtBinLabel = "_Pt_Bin5";
    }
    else{

      if(pt_shifted < 20)       PtBinLabel = "_Pt_Bin1";
      else if(pt_shifted < 30)  PtBinLabel = "_Pt_Bin2";
      else if(pt_shifted < 40)  PtBinLabel = "_Pt_Bin3";
      else if(pt_shifted < 50)  PtBinLabel = "_Pt_Bin4";
      else if(pt_shifted < 60)  PtBinLabel = "_Pt_Bin5";
      else if(pt_shifted < 70)  PtBinLabel = "_Pt_Bin6";
      else if(pt_shifted < 80)  PtBinLabel = "_Pt_Bin7";
      else  if(pt_shifted < 100)PtBinLabel = "_Pt_Bin8";
      else  if(pt_shifted < 200)PtBinLabel = "_Pt_Bin9";
      else PtBinLabel = "_Pt_Bin10";
    }
    return PtBinLabel;
    
  }
  
  inline double GetDeepJetFromID(TString ID, TString prefix) const{
    double BJetDeepJetCut=999;
    if(ID.Contains(prefix+"v1")) BJetDeepJetCut = 0.025;
    if(ID.Contains(prefix+"v2")) BJetDeepJetCut = 0.05;   //// Version 2 == TOP                                                                                                                                                                                   
    if(ID.Contains(prefix+"v3")) BJetDeepJetCut = 0.1;
    if(ID.Contains(prefix+"v4")) BJetDeepJetCut = 0.2;
    if(ID.Contains(prefix+"v5")) BJetDeepJetCut = 0.3;
    if(ID.Contains(prefix+"v6")) BJetDeepJetCut = 0.4;
    if(ID.Contains(prefix+"v7")) BJetDeepJetCut = 0.5;
    if(ID.Contains(prefix+"v8")) BJetDeepJetCut = 0.75;
    if(ID.Contains(prefix+"v9")) BJetDeepJetCut = 0.9;
    
    return BJetDeepJetCut;
  }
  inline bool ApplyCvsB(TString ID) const{
    if(ID.Contains("_b")) return true;
    if(ID.Contains("_c")) return true;
    return false;
  }
  inline bool ApplyCvsL(TString ID)const{
    if(ID.Contains("_b")) return true;
    if(ID.Contains("_c")) return true;
    return false;
  }

  inline double GetCvsBCut(TString ID)const{
    if(ID.Contains("_b")) return 0.1;
    if(ID.Contains("_c")) return 0.1;
    return -999;
  }
  inline double GetCvsLCut(TString ID)const{
    if(ID.Contains("_b")) return 0.6;
    if(ID.Contains("_c")) return 0.8;
    return 999;
  }

  void SetEnShift(double en_up, double en_down);
  inline double EnShift(int s) const {
    if(s==0) return 1.;
    else if(s>0) return j_En_up;
    else return j_En_down;
  }
  void SetResShift(double res_up, double res_down);
  inline double ResShift(int s) const {
    if(s==0) return 1.;
    else if(s>0) return j_Res_up;
    else return j_Res_down;
  }


  ///// Supercluster Variables

  void SetSC(double sceta, double scphi, double sce);
  inline double scEta() const { return j_scEta; }
  inline double scPhi() const { return j_scPhi; }
  inline double scE() const { return j_scE; }

  inline TString CFEtaLabel() const {
    if(fabs(j_scEta) < 1.4442) return "BB2";
    else  if(fabs(j_scEta) < 2.2) return "EC1";
    else return "EC2";


  }


  ///////// ELECTRON MVA FUNCTIONS 

  void SetMVA(double mvaiso, double mvanoiso);
  inline double MVAIso() const { return j_mvaiso; }
  inline double MVANoIso() const { return j_mvanoiso; }


  /////// --> Response functions used for BDT training 
  inline bool PassMVANoIsoResponse(double A, double B, double C){
    double mva_resp = MVANoIsoResponse();
    double cut = A - std::exp(-Pt() / B) * C;
    if (mva_resp < cut) return true;
    return false;
  }

  inline bool PassMVAIsoResponse(double A, double B, double C){
    double mva_resp = MVAIsoResponse();
    double cut = A - std::exp(-Pt() / B) * C;
    if (mva_resp < cut)return true;
    return false;
  }
  
  inline double MVANoIsoResponseV1() const {
    if (j_mvanoiso == 1.) return 8;
    if(MVANoIsoResponseRaw() > 8) return 8;
    if(MVANoIsoResponseRaw() < -8) return -8;
    return MVANoIsoResponseRaw();
  }

  inline double MVANoIsoResponse() const {
    
    if (j_mvanoiso == 1.) return 7;

    if(MVANoIsoResponseRaw() > 7) return 7;
    if(MVANoIsoResponseRaw() < -7) return -7;
    return MVANoIsoResponseRaw();
  }

  inline double MVAIsoResponseV1() const {

    if (j_mvaiso == 1.) return 8;

    if(MVAIsoResponseRaw() > 8) return 8;
    if(MVAIsoResponseRaw() < -8) return -8;
    return MVAIsoResponseRaw();
  }

  inline double MVAIsoResponse() const {

    if (j_mvaiso == 1.) return 7;

    if(MVAIsoResponseRaw() > 7) return 7;
    if(MVAIsoResponseRaw() < -7) return -7;
    return MVAIsoResponseRaw();
  }

    

  inline double MVAIsoResponseRaw() const {

    return -1.0 * std::log( (2./ (j_mvaiso + 1.)) -1.)/2.;

  }
  
  inline double MVANoIsoResponseRaw() const {
    //https://github.com/cms-sw/cmssw/blob/master/CondFormats/GBRForest/interface/GBRForest.h
    //  return 2.0 / (1.0 + std::exp(-2.0 * response)) - 1;  //MVA output between -1 and 1
    
    
    return -1.0 * std::log( (2./ (j_mvanoiso + 1.)) -1.)/2.;

  }

  bool PassMVABaseLine() const;
  bool PassMVABaseLineTrkIso() const;

  bool PassMVA_POGUL_BB(double mva1, double mva2, double mva3) const ;
  bool PassMVA_POGUL_EB(double mva1, double mva2, double mva3) const ;
  bool PassMVA_POGUL_EE(double mva1, double mva2, double mva3) const ;
  bool PassMVA_2016(double mva1, double mva2, double mva3) const;


  //// Uncorrected Energy variables
  void SetUncorrE(double une);
  inline double UncorrE() const { return j_EnergyUnCorr; }
  inline double UncorrPt() const { return Pt() * j_EnergyUnCorr/E(); }


  double StringToDouble(TString st,TString subSt) const;


  bool PassHNID()const ;
  int PassHNOpt()const ;
  void SetPassConversionVeto(bool b);
  inline int PassConversionVeto() const { return j_passConversionVeto; }
  void SetNMissingHits(int n);
  inline int NMissingHits() const { return j_NMissingHits; }

  void SetEtaWidth(double d);
  void SetPhiWidth(double d);
  void SetDetaIn(double d);
  void SetSigmaIEtaIE(double d);
  void SetE15(double d);
  void SetE25(double d);
  void SetE55(double d);
  void SetFBrem(double d);
  void SetEOverP(double d);

  
  void SetCutBasedIDVariables(
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
  );
  inline double Full5x5_sigmaIetaIeta() const { return j_Full5x5_sigmaIetaIeta; }
  inline double sigmaIetaIeta() const { return j_sigmaIetaIeta; }
  
  inline double FBrem() const { return j_fbrem; }
  inline double EOverP() const { return j_eoverp; }

  inline double dEtaSeed() const { return j_dEtaSeed; }
  inline double dPhiIn() const { return j_dPhiIn; }
  inline double dEtaIn() const { return j_dEtaIn; }
  inline double EtaWidth() const { return j_EtaWidth; }
  inline double PhiWidth() const { return j_PhiWidth; }
  inline double HoverE() const { return j_HoverE; }
  inline double InvEminusInvP() const { return j_InvEminusInvP; }
  inline double e2x5OverE5x5() const { return j_e2x5OverE5x5; }
  inline double e1x5OverE5x5() const { return j_e1x5OverE5x5; }
  inline double e15() const { return j_e15; }
  inline double e25() const { return j_e25; }
  inline double e55() const { return j_e55; }
  inline double dr03EcalRecHitSumEt() const { return j_dr03EcalRecHitSumEt; }
  inline double dr03HcalDepth1TowerSumEt() const { return j_dr03HcalDepth1TowerSumEt; }
  inline double dr03HcalTowerSumEt() const { return j_dr03HcalTowerSumEt; }
  inline double dr03TkSumPt() const { return j_dr03TkSumPt; }
  inline double ecalPFClusterIso() const { return j_ecalPFClusterIso; }
  inline double hcalPFClusterIso() const { return j_hcalPFClusterIso; }
  inline bool isEcalDriven() const { return j_isEcalDriven; }

  void SetIDBit(unsigned int idbit);

  void SetIDCutBit(vector<int> idcutbit);
  inline vector<int> IDCutBit() const { return j_IDCutBit; }

  static const int N_SELECTOR = 12;
  enum Selector {
    POG_CB_VETO = 1<< 0,
    POG_CB_LOOSE = 1<< 1,
    POG_CB_MEDIUM = 1<< 2,
    POG_CB_TIGHT = 1<< 3,
    POG_MVA_ISO_WP80 = 1<< 4,
    POG_MVA_ISO_WP90 = 1<< 5,
    POG_MVA_ISO_WPHZZ = 1<< 6,
    POG_MVA_ISO_WPLOOSE = 1<< 7,
    POG_MVA_NOISO_WP80 = 1<< 8,
    POG_MVA_NOISO_WP90 = 1<< 9,
    POG_MVA_NOISO_WPLOOSE = 1<< 10,
    POG_HEEP = 1<< 11
  };

  inline bool PassSelector( unsigned int s ) const { return (j_IDBit & s)==s; }
  inline bool passVetoID()   const {return PassSelector(POG_CB_VETO); }
  inline bool passLooseID()  const {return PassSelector(POG_CB_LOOSE); }
  inline bool passMediumID() const {return PassSelector(POG_CB_MEDIUM); }
  inline bool passTightID()  const {return PassSelector(POG_CB_TIGHT); }

  inline bool passMVAID_noIso_WP80() const {return PassSelector(POG_MVA_NOISO_WP80); }
  inline bool passMVAID_noIso_WP90() const {return PassSelector(POG_MVA_NOISO_WP90); }
  inline bool passMVAID_Iso_WP80() const {return PassSelector(POG_MVA_ISO_WP80); }
  inline bool passMVAID_Iso_WP90() const {return PassSelector(POG_MVA_ISO_WP90); }
  inline bool passMVAID_noiso_WPLoose() const {return PassSelector(POG_MVA_NOISO_WPLOOSE); }
  inline bool passHEEPID() const {return PassSelector(POG_HEEP); }
  inline bool passHEEPID_NoMinPt() const {
					int HEEPcutbit = IDCutBit().at(11);

					//cout << "HEEPcutbit : " << HEEPcutbit << endl;
					//cout << "Pt : " << Pt() << endl;
					//if((HEEPcutbit&4094)==4094){
					//cout << "HEEP w/o MinPt passed;" << endl;
					//cout << "===========================" << endl;
					//cout << "HEEPcutbit : " << HEEPcutbit << endl;
					//cout << "Pt : " << Pt() << endl;
					//cout << "===========================" << endl;
					//}
					//if(PassSelector(POG_HEEP)){
					//cout << "HEEP passed;" << endl;
					//cout << "===========================" << endl;
					//cout << "HEEPcutbit : " << HEEPcutbit << endl;
					//cout << "Pt : " << Pt() << endl;
					//cout << "===========================" << endl;
					//}
					
					return (HEEPcutbit&4094)==4094; } //JH

  bool passHEEPIDHNL() const;
  bool passHEEP2018PromptHNL() const;

  int PassHNLTight(TString ID) const;
  int PassHNLTop(TString ID) const;

  bool Pass_MultiFunction_Opt(TString ID) const;
  bool Pass_LepMVAID() const ;
  bool Pass_LepMVAID_TrkIso() const ;
  bool Pass_LepMVAID_split1() const;
  bool Pass_LepMVAID_split2() const;
  bool Pass_LepMVAID_split3() const;
  bool Pass_LepMVAID_split4() const;
  bool Pass_LepMVATopID() const ;
  bool passLooseHEEPID() const;
  bool passLooseHEEPID_NoMinPt() const; //JH

  bool passHEEP2018Prompt() const; // HEEP

  bool Pass_SUSYMVAWP(TString wp) const;
  bool Pass_SUSYTight() const;
  bool passMVAID_noIso_WP80HN() const;
  bool passMVAID_noIso_WP90HN() const;
  bool passMVAID_noIso_WP90XHN(double relisoCut, double dxyCut, double dzCut, double sipCut) const;
  bool passMVAID_noIso_XHN(double relisoCut, double dxyCut, double dzCut, double sipCut) const;
  bool passPOGMVAID_noIso_WP80HN() const;
  bool passPOGMVAID_noIso_WP90HN() const;



  bool passMVAID_noIso_WP80HNLoose() const;
  bool passMVAID_noIso_WP90HNLoose() const;
  
  bool passMVAID_Iso_WP80HN() const;
  bool passMVAID_Iso_WP90HN() const;
  bool passPOGMVAID_Iso_WP80HN() const;
  bool passPOGMVAID_Iso_WP90HN() const;

  bool passMVAID_Iso_WP80HNLoose() const;
  bool passMVAID_Iso_WP90HNLoose() const;


  bool Pass_SUSYLoose() const;
  bool passTightIDHN() const;
  bool passMediumIDHN() const;
  bool passLooseIDHN() const;
  bool passMediumID_NoTightCharge() const;
  bool passTightID_NoTightChargeB() const;
  bool passTightID_NoTightCharge() const;
  bool passLooseID_NoTightChargeB() const;
  bool passLooseID_NoTightCharge() const;
  bool Pass_CutBasedTightWithIPcut() const;

  //==== HN ID
  bool Pass_TriggerEmulation() const;
  bool Pass_TriggerEmulationLoose() const;
  bool Pass_TriggerEmulationN(int cut) const;

  bool Pass_HNVetoMVA() const;
  bool Pass_HNLoose2016(double relisoCut, double dxyCut, double dzCut, double sipCut) const;

  bool Pass_HNLoose2016MVAISO( double dxyCut, double dzCut, double sipCut) const;
  bool Pass_HNLoose2016MVANonIso( double relisoCut,double dxyCut, double dzCut, double sipCut) const;
  bool Pass_HNTight2016() const;
  bool Pass_HNTightUL() const;

  bool Pass_HNVetoCutBased(double relisoCut) const;
  bool Pass_HNLoose(double relisoCut, double dxyCut, double dzCut) const;
  bool Pass_HNLooseID(double relisoCut, double dxyCut, double dzCut, double sipCut) const;
  bool Pass_HNLoosest() const;
  bool Pass_HNTight(double relisoCut,double dxyCut1, double dxyCut2, double dzCut, double sipCut1, double sipCut2) const;
  bool Pass_HNMedium(double relisoCut,double dxyCut1, double dxyCut2, double dzCut, double sipCut1, double sipCut2) const;

  bool Pass_ISRLoose(double relisoCut) const;
  bool Pass_ISRTight() const;

  bool Pass_HNMVALoose(double relisoCut, double dxyCut, double dzCut) const;
  bool Pass_HNMVATight(double relisoCut, double dxyCut, double dzCut) const;

  void SetRelPFIso_Rho(double r);
  double EA();


  bool PassStandardIDs(TString ID) const;

  int  PassLooseIDOpt(TString  trigger, TString dxy_method, TString sel_methodB,TString sel_methodEC, TString conv_method, TString chg_method, TString iso_methodB,TString iso_methodEC ) const;


  bool passIDHN(int ID, double dxy_b, double dxy_e, double dz_b,double dz_e, double sip_b, double sip_e, double iso_b,double iso_e, double miso_b, double miso_e) const;
  bool PassIDOpt(TString ID, bool cc, double dx_b ,double dx_e,double dz_b,double dz_e, double iso_b, double iso_e) const;
  //==== ID

  bool PassID(TString ID) const;
  bool PassIDForOpt(TString ID) const;

  int  PassIDOptLoose(TString ID) const;
  int  PassIDOpt(TString ID) const;
  int  PassIDLoose(TString ID) const;
  int  PassIDTight(TString ID) const;
  int  PassIDStudy(TString ID) const;

  // Jiwhan functio
  bool Pass_CaloIdL_TrackIdL_IsoVL(TString Option="") const;

  bool Pass_TESTID() const;

  bool Pass_CutBasedTightNoIso() const;
  bool Pass_CutBasedLooseNoIso() const;
  bool Pass_CutBasedMediumNoIso() const;
  bool Pass_CutBasedVetoNoIso() const;
  bool Pass_CutBasedLoose() const;
  bool Pass_CutBasedVeto() const;
  void SetRho(double r);
  inline double Rho() const { return j_Rho; }
  void SetIsGsfCtfScPixChargeConsistent(bool b, bool c, bool d);
  inline bool IsGsfCtfScPixChargeConsistent() const { return j_isGsfCtfScPixChargeConsistent; }
  inline bool IsGsfScPixChargeConsistent() const { return j_isGsfScPixChargeConsistent; }
  inline bool IsGsfCtfChargeConsistent() const { return j_isGsfCtfChargeConsistent; }

  inline void SetR9(double r9) { j_r9=r9; }
  inline double R9() const { return j_r9; }

  inline void  SetpsEoverEraw(double psE) {j_psEoverEraw=psE;}
  inline double psEoverEraw() const { return j_psEoverEraw;}

  inline void SetL1Et(double l1et) { j_L1Et=l1et; }
  inline double L1Et() const { return j_L1Et; }

  void SetFilterBits(ULong64_t filterbits){ j_filterbits=filterbits; }
  void SetPathBits(ULong64_t pathbits){ j_pathbits=pathbits; }
  bool PassFilter(TString filter) const;
  bool PassPath(TString path) const;

    

private:

  double j_En_up;
  double j_En_down;
  double j_Res_up;
  double j_Res_down;
  double j_scEta,j_scPhi, j_scE;
  double j_mvaiso, j_mvanoiso;
  double j_EnergyUnCorr;
  bool j_passConversionVeto;

  double j_Full5x5_sigmaIetaIeta, j_sigmaIetaIeta,j_dEtaSeed, j_dPhiIn,j_dEtaIn,j_PhiWidth, j_EtaWidth, j_HoverE, j_InvEminusInvP, j_e2x5OverE5x5, j_e1x5OverE5x5, j_dr03EcalRecHitSumEt, j_dr03HcalDepth1TowerSumEt, j_e15, j_e25, j_e55,j_fbrem,j_eoverp;
  double j_dr03HcalTowerSumEt, j_dr03TkSumPt, j_ecalPFClusterIso, j_hcalPFClusterIso;
  bool j_isEcalDriven;
  double j_L1Et; 
  unsigned int j_IDBit;
  vector<int> j_IDCutBit;
  double j_RelPFIso_Rho;

  double j_Rho ,j_r9, j_psEoverEraw;
  

  int j_isGsfCtfScPixChargeConsistent,j_isGsfScPixChargeConsistent,j_isGsfCtfChargeConsistent;
  int j_NMissingHits;

  ULong64_t j_filterbits;
  ULong64_t j_pathbits;

  ClassDef(Electron,1)

};

#endif
