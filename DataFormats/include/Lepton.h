#ifndef Lepton_h
#define Lepton_h

#include "Particle.h"

class Lepton: public Particle {
public:

  Lepton();
  ~Lepton();

  void SetdXY(double dXY, double dXYerr);
  inline double dXY() const {return j_dXY;}
  inline double dXYerr() const {return j_dXYerr;}

  void SetLepMVA(double mva);
  inline double lep_mva() const {return j_lep_mva;}


  void SetJetPtRel(double ptrel);
  inline double lep_jet_ptrel() const {return j_lep_jetptrel;}
  void SetJetPtRatio(double ptr);
  inline double lep_jet_ptratio() const {return j_lep_jetptratio;}



  void SetdZ(double dZ, double dZerr);
  inline double dZ() const {return j_dZ;}
  inline double dZerr() const {return j_dZerr;}

  void SetIP3D(double IP3D, double IP3Derr);
  inline double IP3D() const {return j_IP3D;}
  inline double IP3Derr() const {return j_IP3Derr;}

  //==== AbsIso will be set in Muon/Electron,
  //==== and use SetRelIso to save calculated RelIso
  void SetRelIso(double r);
  inline double RelIso() const {return j_RelIso;}

  //==== SUSY mini Iso has same formula for Muon and Electron
  void SetMiniIso(double ch, double nh, double ph, double pu, double rho, double EA);
  inline double MiniRelIso() const {return j_MiniRelIso;}

  enum Flavour{
    NONE, ELECTRON, MUON, TAU
  };

  inline bool PassLepID()  const {return j_passID;}
  inline bool LepIDSet()  const {return j_IDSet;}


  inline int LeptonType() const {return j_LeptonType;}
  void SetLeptonType(int t);


  inline Flavour LeptonFlavour() const {return j_LeptonFlavour;}
  void SetLeptonFlavour(Flavour f);

  void SetPassID(bool p);
  void SetID();

  inline bool IsElectron() const {return j_LeptonFlavour==ELECTRON;}
  inline bool IsMuon() const {return j_LeptonFlavour==MUON;}

  inline TString GetFlavour() const {
    if (j_LeptonFlavour==ELECTRON) return "Electron";
    else return "Muon";
  }

  inline float miniIsoDr() const {
    float mindr = 0.05;
    float maxdr = 0.2;
    float kt_scale = 10.0;
    return std::max(mindr, std::min(maxdr, float(kt_scale/this->Pt())));
  }

  void SetPtCone(double f){ j_ptcone = f; }
  inline bool IsPtConeAvailable() const {
    if(j_ptcone<0) return false;
    else return true;
  }
  inline double PtCone() const {
    if(j_ptcone<0){
      cout << "[Lepton::PtCone] ptcone not set" << endl;
      exit(EXIT_FAILURE);
    }
    return j_ptcone;
  }
  inline double CalcPtCone(double this_reliso, double Tight_reliso){
    return ( this->Pt() ) * ( 1. + max(0., (this_reliso-Tight_reliso)) );
  }

  virtual void Print();

private:
  double j_dXY, j_dXYerr;
  double j_lep_mva;
  double j_dZ, j_dZerr;
  double j_IP3D, j_IP3Derr;

  double j_RelIso, j_MiniRelIso;
  double j_ptcone;
  double j_lep_jetptrel,j_lep_jetptratio;
  Flavour j_LeptonFlavour;

  int j_LeptonType;

  bool j_passID;
  
  bool j_IDSet;

  ClassDef(Lepton,1)

};

#endif
