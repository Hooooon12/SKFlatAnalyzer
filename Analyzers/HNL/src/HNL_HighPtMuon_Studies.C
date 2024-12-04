#include "HNL_HighPtMuon_Studies.h"

void HNL_HighPtMuon_Studies::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();
  
}


void HNL_HighPtMuon_Studies::executeEvent(){

  AddTimerStamp("start_ev");
  
  FillTimer("START_EV");

  //==== Gen for genmatching
  
  Event ev = GetEvent();  
  AnalyzerParameter param = HNL_LeptonCore::InitialiseHNLParameter("HNL_ULID");
  double weight = SetupWeight(ev,param);

  std::vector<Muon>       Muons    = GetMuons    (param.Muon_Veto_ID,     30.,  2.4);
  if(!SameCharge(Muons)) return;

  for(unsigned int imu = 0; imu < Muons.size(); imu++){
    
    if(Muons[imu].PassID(param.Muon_Tight_ID)){
      if(ComparePtTune(Muons[imu], 0.1)) {
	cout << "Event " << event << " Muon pt mismatch "  << " pt = " << Muons[imu].Pt() << " tune pt = "  << Muons[imu].TuneP4().Pt() << " ratio = " <<  Muons[imu].TuneP4().Pt()/ Muons[imu].Pt()  << endl;
	
      }

    }
  }


  return;
  FillTimer("END_Run");
  
  
}
  


HNL_HighPtMuon_Studies::HNL_HighPtMuon_Studies(){


}
 
HNL_HighPtMuon_Studies::~HNL_HighPtMuon_Studies(){
  

}
