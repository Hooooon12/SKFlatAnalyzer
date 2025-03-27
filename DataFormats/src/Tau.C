#include "Tau.h"

ClassImp(Tau)

Tau::Tau(){

  j_IDBit = 0;
  j_decaymode=-1;
  j_idDecayModeNewDMs=false;

  this->SetLeptonFlavour(TAU);
}

void Tau::PrintObject(TString label){

}

Tau::~Tau(){

}

void Tau::SetIDBit(unsigned int idbit){
  j_IDBit = idbit;
}


void Tau::SetDecayMode(int decaymode){

  j_decaymode= decaymode;
}


void Tau::SetDecayModeNewDM(bool DecayModeNewDMs){

  j_idDecayModeNewDMs= DecayModeNewDMs;
}


   

bool Tau::PassID(TString ID) const{

  if(ID=="Default")     return false;


  // === list of IDs for analyis
  if(ID=="NoCut") {
    if(j_decaymode == 0 || j_decaymode == 1 || j_decaymode == 10 || j_decaymode ==11) {
      if (DecayModeNewDM() )return true;
      return false;
    }
  }

  if(ID=="HNVeto)") {
    if(j_decaymode == 0 || j_decaymode == 1 || j_decaymode == 10 || j_decaymode ==11) {
      if (DecayModeNewDM() && passTIDvJet() && passTIDvEl()  && passTIDvMu() ) return true;
    }
    return false;
  }

  if(j_decaymode == 0 || j_decaymode == 1 || j_decaymode == 10 || j_decaymode ==11) {
    if (DecayModeNewDM()){
      if(ID.Contains("JetVVL") && !passVVLIDvJet()) return false;
      if(ID.Contains("JetVL")  && !passVLIDvJet())  return false;
      if(ID.Contains("JetL")   && !passLIDvJet())   return false;
      if(ID.Contains("JetM")   && !passMIDvJet())   return false;
      if(ID.Contains("JetT")   && !passTIDvJet())   return false;
      if(ID.Contains("JetVT")  && !passVTIDvJet())  return false;
      if(ID.Contains("JetVVT") && !passVVTIDvJet()) return false;
      
      // El
      if(ID.Contains("ElVVL") && !passVVLIDvEl())return false;
      if(ID.Contains("ElVL") && !passVLIDvEl())return false;
      if(ID.Contains("ElL") && !passLIDvEl())return false;
      if(ID.Contains("ElM") && !passMIDvEl())return false;
      if(ID.Contains("ElT") && !passTIDvEl())return false;
      if(ID.Contains("ElVT") && !passVTIDvEl())return false;
      if(ID.Contains("ElVVT") && !passVVTIDvEl())return false;

      if(ID.Contains("MuVL") && !passVLIDvMu())return false;
      if(ID.Contains("MuL") && !passLIDvMu())return false;
      if(ID.Contains("MuM") && !passMIDvMu())return false;
      if(ID.Contains("MuT") && !passTIDvMu())return false;
      return true;
    }
  }

  return false;
}
