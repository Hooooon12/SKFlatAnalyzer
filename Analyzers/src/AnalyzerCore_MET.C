#include "AnalyzerCore.h"


Particle AnalyzerCore::UpdateMETSyst(double met_pt, double met_phi, double met_shift_pt, double met_shift_phi, const Particle& METv){

  double met_px = met_pt*TMath::Cos(met_phi);
  double met_py = met_pt*TMath::Sin(met_phi);
  double met_shift_px = met_shift_pt*TMath::Cos(met_shift_phi);
  double met_shift_py = met_shift_pt*TMath::Sin(met_shift_phi);

  double met_x = METv.Px();
  double met_y = METv.Py();

  met_x = met_x + met_shift_px - met_px;
  met_y = met_y + met_shift_py - met_py;

  Particle METout;
  METout.SetPxPyPzE(met_x,met_y,0,sqrt(met_x*met_x+met_y*met_y));
  return METout;


}

Particle AnalyzerCore::UpdateMETCF(const Particle METv, const std::vector<Electron> electrons, const std::vector<Electron> electrons_shifted){

  double met_x = METv.Px();
  double met_y = METv.Py();

  double px_orig(0.), py_orig(0.),px_corrected(0.), py_corrected(0.);
  for(auto iel : electrons){
    px_orig+= iel.Px();
    py_orig+= iel.Py();
  }
  for(auto iel : electrons_shifted){
    px_corrected += iel.Px();
    py_corrected += iel.Py();
  }

  met_x = met_x + px_orig - px_corrected;
  met_y = met_y + py_orig - py_corrected;

  Particle METout;
  METout.SetPxPyPzE(met_x,met_y,0,sqrt(met_x*met_x+met_y*met_y));
  return METout;

}



Particle AnalyzerCore::UpdateMET(const Particle& METv, const std::vector<Muon>& muons){

  double met_x = METv.Px();
  double met_y = METv.Py();

  double px_orig(0.), py_orig(0.),px_corrected(0.), py_corrected(0.);
  for(unsigned int i=0; i<muons.size(); i++){

    px_orig+= muons.at(i).MiniAODPt()*TMath::Cos(muons.at(i).Phi());
    py_orig+= muons.at(i).MiniAODPt()*TMath::Sin(muons.at(i).Phi());

    px_corrected += muons.at(i).Px();
    py_corrected += muons.at(i).Py();

  }

  met_x = met_x + px_orig - px_corrected;
  met_y = met_y + py_orig - py_corrected;

  Particle METout;
  METout.SetPxPyPzE(met_x,met_y,0,sqrt(met_x*met_x+met_y*met_y));
  return METout;

}

Particle AnalyzerCore::UpdateMETSmearedJet(const Particle& METv, const std::vector<Jet>& jets){

  double met_x = METv.Px();
  double met_y = METv.Py();

  double px_orig(0.), py_orig(0.),px_corrected(0.), py_corrected(0.);
  for(auto jet : jets){
    px_orig+= jet.PxUnSmeared();
    py_orig+= jet.PyUnSmeared();

    px_corrected += jet.Px();
    py_corrected += jet.Py();
  }

  met_x = met_x + px_orig - px_corrected;
  met_y = met_y + py_orig - py_corrected;

  Particle METout;
  METout.SetPxPyPzE(met_x,met_y,0,sqrt(met_x*met_x+met_y*met_y));
  return METout;

}


Particle AnalyzerCore::UpdateMETSyst(AnalyzerParameter param, const Particle& METv, std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Muon> muons, std::vector<Electron> electrons){

  double met_x = METv.Px();
  double met_y = METv.Py();

  double px_orig(0.), py_orig(0.),px_corrected(0.), py_corrected(0.);

  if(param.syst_ == AnalyzerParameter::JetResUp ||
     param.syst_ == AnalyzerParameter::JetResDown ||
     param.syst_ == AnalyzerParameter::JetEnUp ||
     param.syst_ == AnalyzerParameter::JetEnDown){

    for(unsigned int i=0; i<jets.size(); i++){
      px_orig += jets.at(i).PxUnSmeared();
      py_orig += jets.at(i).PyUnSmeared();
      px_corrected += jets.at(i).Px();
      py_corrected += jets.at(i).Py();
    }
    for(unsigned int i=0; i<fatjets.size(); i++){
      px_orig += fatjets.at(i).PxUnSmeared();
      py_orig += fatjets.at(i).PyUnSmeared();
      px_corrected += fatjets.at(i).Px();
      py_corrected += fatjets.at(i).Py();
    }
  }

  if(param.syst_ == AnalyzerParameter::JetMassUp ||
     param.syst_ == AnalyzerParameter::JetMassDown ||
     param.syst_ == AnalyzerParameter::JetMassSmearUp ||
     param.syst_ == AnalyzerParameter::JetMassSmearDown){

    for(unsigned int i=0; i<fatjets.size(); i++){
      px_orig += fatjets.at(i).PxUnSmeared();
      py_orig += fatjets.at(i).PyUnSmeared();
      px_corrected += fatjets.at(i).Px();
      py_corrected += fatjets.at(i).Py();
    }
  }

  if(param.syst_ ==AnalyzerParameter::MuonEnUp ||
     param.syst_ ==AnalyzerParameter::MuonEnDown){
    for(unsigned int i=0; i<muons.size(); i++){
      px_orig += muons.at(i).UncorrectedPt() * TMath::Cos(muons.at(i).Phi());
      py_orig += muons.at(i).UncorrectedPt() * TMath::Sin(muons.at(i).Phi());
      px_corrected += muons.at(i).Px();
      py_corrected += muons.at(i).Py();
    }
  }
  if(param.syst_ ==AnalyzerParameter::ElectronEnUp ||
     param.syst_ ==AnalyzerParameter::ElectronEnDown ||
     param.syst_ ==AnalyzerParameter::ElectronResUp ||
     param.syst_ ==AnalyzerParameter::ElectronResDown){
    for(unsigned int i=0; i<electrons.size(); i++){
      px_orig += electrons.at(i).UncorrectedPt() * TMath::Cos(electrons.at(i).Phi());
      py_orig += electrons.at(i).UncorrectedPt() * TMath::Sin(electrons.at(i).Phi());
      px_corrected += electrons.at(i).Px();
      py_corrected += electrons.at(i).Py();
    }
  }

  met_x = met_x + px_orig - px_corrected;
  met_y = met_y + py_orig - py_corrected;

  Particle METout;
  METout.SetPxPyPzE(met_x,met_y,0,sqrt(met_x*met_x+met_y*met_y));
  return METout;

}

