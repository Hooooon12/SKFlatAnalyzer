#ifndef Particle_h
#define Particle_h

#include "TLorentzVector.h"
#include <TString.h>
#include <iostream>

using namespace std;

class Particle: public TLorentzVector{

public:


  inline TString GetCFShift_BinLabel(double gen_dummy_pt) const{

    TString PtBinLabel  =  "";
    if(fabs(this->Eta()) < 1.5){
      if(gen_dummy_pt < 35)       PtBinLabel = "_Pt_Bin1";
      else  if(gen_dummy_pt < 50)PtBinLabel = "_Pt_Bin2";
      else  if(gen_dummy_pt < 70)PtBinLabel = "_Pt_Bin3";
      else  if(gen_dummy_pt < 100)PtBinLabel = "_Pt_Bin4";
      else PtBinLabel = "_Pt_Bin5";
    }
    else{

      if(gen_dummy_pt < 20)       PtBinLabel = "_Pt_Bin1";
      else if(gen_dummy_pt < 30)  PtBinLabel = "_Pt_Bin2";
      else if(gen_dummy_pt < 40)  PtBinLabel = "_Pt_Bin3";
      else if(gen_dummy_pt < 50)  PtBinLabel = "_Pt_Bin4";
      else if(gen_dummy_pt < 60)  PtBinLabel = "_Pt_Bin5";
      else if(gen_dummy_pt < 70)  PtBinLabel = "_Pt_Bin6";
      else if(gen_dummy_pt < 80)  PtBinLabel = "_Pt_Bin7";
      else  if(gen_dummy_pt < 100)PtBinLabel = "_Pt_Bin8";
      else  if(gen_dummy_pt < 200)PtBinLabel = "_Pt_Bin9";
      else PtBinLabel = "_Pt_Bin10";
    }
    return PtBinLabel;
  }



  void  PrintObject(TString label);


  //==== Default Constructor with p4
  Particle();
  //==== TLorentzVector copy constructor.
  Particle(const TLorentzVector& p);
  //==== Copy constructor.
  Particle(const Particle& p);
  //==== Construct from 4-vector components
  Particle(double px, double py, double pz, double e);

  //==== Add Charge
  Particle& operator+=(const Particle& p);

  //==== Assignment operator uses KParticle copy constructor
  Particle& operator=(const Particle& p);

  ~Particle();

  void SetCharge(double q);
  inline double Charge() const {return j_Charge;}

  inline TString PIDToString(int pid) const {

    /////// https://pdg.lbl.gov/2008/reviews/rpp2008-rev-monte-carlo-numbering.pdf
    
    if(fabs(pid)  < 7){
      TString sign="";
      if (pid < 0) sign="bar";
      
      if(fabs(pid) < 4) return "q";
      if(fabs(pid) == 4) return "c";
      if(fabs(pid) == 5) return "b";
      if(fabs(pid) == 6) return "t";

    }
 
    if(pid == -11) return "e+";
    if(pid == 11)  return "e-";

    if(pid == -13) return "mu+";
    if(pid == 13)  return "mu-";

    if(fabs(pid) == 11) return "electron";
    if(fabs(pid) == 12) return "nue";
    if(fabs(pid) == 13) return "muon";
    if(fabs(pid) == 14) return "numu";
    if(fabs(pid) == 15) return "tau";
    if(fabs(pid) == 16) return "nutau";

    if(fabs(pid) == 21) return "g";
    if(fabs(pid) == 22) return "ph";
    if(fabs(pid) == 23) return "Z";
    if(fabs(pid) == 24) return "W";

    if(fabs(pid) == 2212) return "proton";

    ///// Most Common in Fake
    if(fabs(pid) == 111) return "pi0";
    if(fabs(pid) == 211) return "pi+";
    
    if(fabs(pid) == 411) return "D+";
    if(fabs(pid) == 421) return "D0";
    vector<int> VCMeson = {10411,10421,414,423,10413,10423,415,425,431,10431,433,10433,20433,435};
    if (std::find(VCMeson.begin(), VCMeson.end(), fabs(pid)) != VCMeson.end()) return "D";

    if(fabs(pid) == 321) return "K+";
    if(fabs(pid) == 313) return "K*";
    if(fabs(pid) == 323) return "K*";
    vector<int> VSMeson = {130,310,311,10313};
    if (std::find(VSMeson.begin(), VSMeson.end(), fabs(pid)) != VSMeson.end()) return "K";
    
    if(fabs(pid) == 511) return "B0";
    if(fabs(pid) == 521) return "B+";
    vector<int> VBMeson = {513,523,531,533};
    if (std::find(VBMeson.begin(), VBMeson.end(), fabs(pid)) != VBMeson.end()) return"B";
    
 
    vector<int> VLMeson= {223,331,221,333,223,213};
    if (std::find(VLMeson.begin(), VLMeson.end(), fabs(pid)) != VLMeson.end()) return "LightMeson";

    if(fabs(pid) > 4000 && fabs(pid) < 5000) return "CBaryon";
    if(fabs(pid) > 5000 && fabs(pid) < 6000) return "BBaryon";

    vector<int> VccMeson = {441,10441,100441,443,10443,20443,100443,30443,9000443,9010443,9010443,9020443,445,100445};
    if (std::find(VccMeson.begin(), VccMeson.end(), fabs(pid)) != VccMeson.end()) return "ccMeson";
    vector<int> VbbMeson = {551,10551,100551,110551,200551,210551,553,10553,20553,30553,100553,110553,120553,130553,200553,210553,220553,300553,9000553,9010553,555,10555,110555,20555,100555,110555,120555,200555,557,100557};
    if (std::find(VbbMeson.begin(), VbbMeson.end(), fabs(pid)) != VbbMeson.end()) return "bbMeson";


    return "NULL";
    
  }

  bool k_debug;


  //==== Print four vector
  virtual void Print();

  

private:
  double j_Charge;


  ClassDef(Particle,1)

};

#endif
