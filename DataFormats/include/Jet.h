#ifndef Jet_h
#define Jet_h

#include "Particle.h"
#include "JetTaggingParameters.h"

class Jet: public Particle {
public:

  Jet();
  ~Jet();
  void  PrintObject(TString label);


  void SetArea(double area);
  void SetGenFlavours(int pf, int hf);
  inline int partonFlavour() const { return j_partonFlavour; }
  inline int hadronFlavour() const { return j_hadronFlavour; }
  
  inline void SetIsSmeared(bool b)  {j_issmeared = b; }
  inline bool IsSmeared() const {return j_issmeared;}

  void SetGenHFHadronMatcher(int flavour, int origin);
  inline int GenHFHadronMatcherFlavour() const { return j_GenHFHadronMatcher_flavour; }
  inline int GenHFHadronMatcherOrigin() const { return j_GenHFHadronMatcher_origin; }
  inline bool IsGenMatched() const { return j_GenHFHadronMatcher_flavour!=-999; }
  void SetTaggerResults(std::vector<double> ds);
  void SetEnergyFractions(double cH, double nH, double nEM, double cEM, double muE);
  inline double ChargedEmEnergyFraction() const { return j_chargedEmEnergyFraction;}
  inline double ChargedHadEnergyFraction() const { return j_chargedHadronEnergyFraction;}
  inline double NeutralEmEnergyFraction() const { return j_neutralEmEnergyFraction;}
  inline double NeutralHadEnergyFraction() const { return j_neutralHadronEnergyFraction;}
  inline double MuonEnergyFraction() const { return j_muonEnergyFraction;}

  inline double NVtxTracks() const { return j_nvtx_tracks;}

  void SetNTracks(double v);

  void SetJEC(double l1, double full);
  inline double JEC_L1() const {return j_jec_l1;}
  inline double JEC_Full() const {return j_jec_full;}
  
  
  void SetMultiplicities(double cM, double nM);
  void SetPileupJetId(double v);
  inline double PileupJetId() const { return j_PileupJetId; }

  void SetEnShift(double en_up, double en_down);
  inline double EnShift(int s) const {
    if(s==0) return 1.;
    else if(s>0) return j_En_up;
    else return j_En_down;
  }
  void SetResShift(double res_up, double res_down);

  void SetRes(double res);
  void SetPxUnSmeared(double px);
  void SetPyUnSmeared(double py);

  inline double JECfactor(){
    double RAWE = sqrt(j_PxUnSmeared*j_PxUnSmeared + j_PyUnSmeared *j_PyUnSmeared) / j_jec_full ;
    
    return RAWE/this->Pt();
  }

  inline bool IsHEMJet(TString era){
    if(era != "2018") return false;
    
    if (this->Eta() < -1.3){
      if((this->Phi() < -0.87) && (this->Phi() > -1.57)) return true;
    }
    return false;
  }

  inline double ResShift(int s) const {
    if(s==0) return 1.;
    else if(s>0) return j_Res_up;
    else return j_Res_down;
  }
  void SetBJetNNCorrection(double bJetNN_corr, double bJetNN_res);
  inline double BJetNNCorrection() const { return j_bJetNN_corr; }
  inline double BJetNNResolution() const { return j_bJetNN_res; }
  void SetCJetNNCorrection(double cJetNN_corr, double cJetNN_res);
  inline double CJetNNCorrection() const { return j_cJetNN_corr; }
  inline double CJetNNResolution() const { return j_cJetNN_res; }

  void SetTightJetID(double b);
  void SetTightLepVetoJetID(double b);
  inline bool Pass_tightJetID() const { return j_tightJetID; }
  inline bool Pass_tightLepVetoJetID() const { return j_tightLepVetoJetID; }

  inline double Res() const { return j_Res;}
  inline double PxUnSmeared()  const { return j_PxUnSmeared;}
  inline double PyUnSmeared()  const { return j_PyUnSmeared;}

  inline int NMult() const { return j_neutralMultiplicity;}
  inline int CHMult() const { return j_chargedMultiplicity;}

  bool Pass_HNTight() const;

  bool PassID(TString ID) const;
  bool PassPileupMVA(TString WP, TString era) const;


  double GetTaggerResult(JetTagging::Tagger tg) const;

private:

  double  j_area;
  bool j_issmeared;
  int j_partonFlavour;
  int j_hadronFlavour;
  double  j_PxUnSmeared;
  double  j_PyUnSmeared;
  int j_GenHFHadronMatcher_flavour;
  int j_GenHFHadronMatcher_origin;
  double  j_DeepCSV;
  double  j_DeepCSV_CvsL;
  double  j_DeepCSV_CvsB;
  double  j_DeepJet;
  double  j_DeepJet_CvsL;
  double  j_DeepJet_CvsB;
  double  j_chargedHadronEnergyFraction;
  double  j_neutralHadronEnergyFraction;
  double  j_neutralEmEnergyFraction;
  double  j_chargedEmEnergyFraction;
  double j_muonEnergyFraction;
  int j_chargedMultiplicity;
  int j_neutralMultiplicity;
  double j_PileupJetId;
  double j_En_up;
  double j_En_down;
  double j_Res;
  double j_Res_up;
  double j_Res_down;
  double j_bJetNN_corr;
  double j_bJetNN_res;
  double j_cJetNN_corr;
  double j_cJetNN_res;
  bool j_tightJetID, j_tightLepVetoJetID;
  double j_nvtx_tracks;
  double j_jec_l1;
  double j_jec_full;


  ClassDef(Jet,1)
};

#endif
