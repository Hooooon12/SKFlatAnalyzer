#ifndef Event_h
#define Event_h

#include "Particle.h"
#include "TString.h"
#include "TObject.h"

class Event : public TObject {
public:

  Event();
  ~Event();
  void  PrintObject(TString label);


  void SetnPV(double n);
  inline int nPV() const { return j_nPV; }
  inline double MET2ST() const {return j_MET2ST;}

  void SetTrigger(vector<string> HLT_TriggerName);
  bool PassTrigger(TString trig);
  bool PassTrigger(std::vector<TString> trigs);
  double GetTriggerLumi(TString trig);
  double GetTriggerLumiByYear(TString trig);
  bool IsPDForTrigger(TString trig, TString PD);

  void SetMET2ST(double metst);
  void SetMET(double pt, double phi);
  inline Particle GetMETVector() const {return j_METVector;}

  void SetEra(TString era){
    j_DataEra=era;
    j_DataYear=TString(era(0,4)).Atoi();
  }
  TString GetEra() const { return j_DataEra; }
  int GetYear() const { return j_DataYear; }

  void SetMVA(TString channel, double mass, double value);

  inline double HNL_MVA_Event(TString vers) const {
    if(vers == "MuMu_90") return j_mva_mm_90;
    if(vers == "MuMu_100") return j_mva_mm_100;
    if(vers == "MuMu_150") return j_mva_mm_150;
    if(vers == "MuMu_200") return j_mva_mm_200;
    if(vers == "MuMu_300") return j_mva_mm_300;
    if(vers == "MuMu_400") return j_mva_mm_400;
    if(vers == "MuMu_500") return j_mva_mm_500;
    if(vers == "EE_90") return j_mva_ee_90;
    if(vers == "EE_100") return j_mva_ee_100;
    if(vers == "EE_150") return j_mva_ee_150;
    if(vers == "EE_200") return j_mva_ee_200;
    if(vers == "EE_300") return j_mva_ee_300;
    if(vers == "EE_400") return j_mva_ee_400;
    if(vers == "EE_500") return j_mva_ee_500;
    if(vers == "EMu_90") return j_mva_em_90;
    if(vers == "EMu_100") return j_mva_em_100;
    if(vers == "EMu_150") return j_mva_em_150;
    if(vers == "EMu_200") return j_mva_em_200;
    if(vers == "EMu_300") return j_mva_em_300;
    if(vers == "EMu_400") return j_mva_em_400;
    if(vers == "EMu_500") return j_mva_em_500;
    
    return -999;
  }


private:
  int j_nPV;
  vector<string> j_HLT_TriggerName;
  double j_MET2ST;
  Particle j_METVector;
  int j_DataYear;
  TString j_DataEra;
  
  double j_mva_mm_90,j_mva_mm_100,j_mva_mm_150,j_mva_mm_200,j_mva_mm_300,j_mva_mm_400,j_mva_mm_500;
  double j_mva_ee_90,j_mva_ee_100,j_mva_ee_150,j_mva_ee_200,j_mva_ee_300,j_mva_ee_400,j_mva_ee_500;
  double j_mva_em_90,j_mva_em_100,j_mva_em_150,j_mva_em_200,j_mva_em_300,j_mva_em_400,j_mva_em_500;
  

  ClassDef(Event,1)
};

#endif
