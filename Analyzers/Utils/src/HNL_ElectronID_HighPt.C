#include "HNL_ElectronID_HighPt.h"

void HNL_ElectronID_HighPt::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

}


void HNL_ElectronID_HighPt::executeEvent(){

  
  //==== Gen for genmatching
  AnalyzerParameter param  = InitialiseHNLParameter("");

  Event ev = GetEvent();

  double weight =  SetupWeight(ev,param);
  
  FillHist ("NoCut", 1, weight, 2, 0., 2.,"");

  vector<Electron> InputElectrons = GetElectrons    ( "NoCut", 10., 2.5);

  vector<TString> IDs = {"HNL_ULID_HEEP","passHEEPID_v3","HNL_ULID","HNTightV2", "PassMVABaseLine","PassMVABaseLine_TrkIso"};
  
  for(auto id : IDs){
    
    vector<Electron> HNLElectrons = SelectElectrons (InputElectrons, id, 15., 2.5);
  
    for(auto ielectron : HNLElectrons){
      if(IsData) FillElectronKinematicPlots(param , id+"_Data",  ielectron, weight);
      else if (ielectron.IsFake())    FillElectronKinematicPlots(param , id+"_Fake",  ielectron, weight);
      else if (ielectron.IsConv())    FillElectronKinematicPlots(param , id+"_Conv",  ielectron, weight);
      else if(ielectron.LeptonIsCF()) FillElectronKinematicPlots(param , id+"_CF",  ielectron, weight);
      else if(ielectron.IsPrompt())   FillElectronKinematicPlots(param , id+"_Prompt",  ielectron, weight);

    }

    if(SameCharge(HNLElectrons)  && (HNLElectrons[0].Pt() > 25) && (HNLElectrons[1].Pt() > 15))  {

      FillHist( id + "/SS_Electron", 1., weight, 4,0., 4);

      FillElectronPlots(param,"SS_"+id,HNLElectrons,weight);
    
    }
  }
  return;
  
}



HNL_ElectronID_HighPt::HNL_ElectronID_HighPt(){


}
 
HNL_ElectronID_HighPt::~HNL_ElectronID_HighPt(){

}

