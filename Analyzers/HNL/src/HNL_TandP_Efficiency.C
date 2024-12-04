#include "HNL_TandP_Efficiency.h"

void HNL_TandP_Efficiency::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

}


void HNL_TandP_Efficiency::executeEvent(){

  
  //==== Gen for genmatching
  AnalyzerParameter param  = InitialiseHNLParameter("");
  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);

  RunHighPt(param,weight);
  RunHNL(param,weight);
  RunPeking(param,weight);

}

void HNL_TandP_Efficiency::RunHighPt(AnalyzerParameter param, double weight){

  TString ID = "POGHighPt";


  std::vector<Muon>      Muons     = SelectMuons    ( param, "Global",    53., 2.4, weight);
  
  for(unsigned int itag=0; itag < Muons.size(); itag++){

    if(!IsData) {
      if(!Muons[itag].IsPrompt()) continue;
    }

    /// Pass Tag ID
    Muon tag = Muons[itag];
    if(!tag.PassID("POGLoose")) continue;
    if(!tag.PassID("POGHighPt")) continue;
    if((tag.TrkIso()/tag.TuneP4().Pt()) > 0.05) continue;
    if(tag.TrkIso() > 30)  continue;
    if(tag.PassPath("HLT_Mu50_v")){
      
      //// Passes Tag
      for(unsigned int iprobe=0; iprobe <Muons.size(); iprobe++){
	if(iprobe==itag) continue;

	Muon probe = Muons[iprobe];
	if(!IsData){
	  if(!probe.IsPrompt()) continue;
	}
	
	if(( probe.TrkIso()/probe.TuneP4().Pt()) > 0.05) continue;
	if(probe.TrkIso() > 30)  continue;
	
	if(PassTandP_PairCriteria(Muons, itag,iprobe)){
	//// Probe pair

	  double pr_pt = (Muons[iprobe].Pt() > 1000) ? 999 : Muons[iprobe].Pt();

          FillTandP(probe.PassID(ID), TString("HighPt"), ID ,pr_pt,probe.Eta(),weight, TString("Bin1"));
          FillTandP(probe.PassID(ID), TString("HighPt"), ID ,pr_pt,probe.Eta(), weight,TString("Bin2"));

	}
      }
    }
  }
  
}



void HNL_TandP_Efficiency::RunHNL(AnalyzerParameter param, double weight){

  TString ID = "HNL_ULID";
  std::vector<Muon>      Muons     = SelectMuons    ( param, "Global",    53., 2.4, weight);

  for(unsigned int itag=0; itag < Muons.size(); itag++){

    Muon tag = Muons[itag];
    if(!IsData){
      if(!tag.IsPrompt()) continue;
    }
    if(!tag.PassID("POGTight")) continue;
    if((tag.TrkIso()/tag.TuneP4().Pt()) > 0.05) continue;
    if(tag.TrkIso() > 30)  continue;
    if(tag.RelIso() > 0.15) continue;
    if(tag.PassPath("HLT_Mu50_v")){

      //// Passes Tag                                                                                                                                                                                                                                                         
      for(unsigned int iprobe=0; iprobe <Muons.size(); iprobe++){

        if(iprobe==itag) continue;
	Muon probe = Muons[iprobe];

	if(!IsData){
	  if(!probe.IsPrompt()) continue;
	}

        if((probe.TrkIso()/probe.TuneP4().Pt()) > 0.05) continue;
        if(probe.TrkIso() > 30)  continue;


        //// Probe pair                                                                                                                                                                                            
	if(PassTandP_PairCriteria(Muons, itag,iprobe)){

          double pr_pt = (probe.Pt() > 1000) ? 999 : probe.Pt();

          FillTandP(probe.PassID(ID), "HNL_ULID", ID ,pr_pt,probe.Eta(), weight,"Bin1");
          FillTandP(probe.PassID(ID), "HNL_ULID", ID ,pr_pt,probe.Eta(), weight,"Bin2");

	}/// OS                                                                                                                                                                                                                                                               
      }
    }
  }

}

  


void HNL_TandP_Efficiency::RunPeking(AnalyzerParameter param, double weight){

  TString ID = "Peking";
  std::vector<Muon>      Muons     = SelectMuons    ( param, "Global",    53., 2.4, weight);

  for(unsigned int itag=0; itag < Muons.size(); itag++){

    Muon tag = Muons[itag];
    
    if(!IsData){
      if(!tag.IsPrompt()) continue;
    }

    // Pass Tight ID + ISO
    if(!tag.PassID("POGTight")) continue;
    if((tag.TrkIso()/tag.TuneP4().Pt()) > 0.05) continue;
    if(tag.TrkIso() > 30)  continue;
    if(tag.RelIso() > 0.15) continue;
    if(tag.PassPath("HLT_Mu50_v")){

      //// Passes Tag                                                                                                                                                                                                                                                                                                      
      for(unsigned int iprobe=0; iprobe <Muons.size(); iprobe++){
        if(iprobe==itag) continue;

	Muon probe = Muons[iprobe];

        if(!IsData){
          if(!probe.IsPrompt()) continue;
        }

        if((probe.TrkIso()/probe.TuneP4().Pt()) > 0.05) continue;
        if(probe.TrkIso() > 30)  continue;

        //// Probe pair                                                                                                                                                                                                                                                                                                    
        if(PassTandP_PairCriteria(Muons, itag,iprobe)){

	  double pr_pt = (probe.Pt() > 1000) ? 999 : probe.Pt();

	  FillTandP(probe.PassID(ID),"Peking", ID ,pr_pt,probe.Eta(), weight,"Bin1");
	  FillTandP(probe.PassID(ID),"Peking", ID ,pr_pt,probe.Eta(), weight,"Bin2");


	}
      }
    }
  }

}




HNL_TandP_Efficiency::HNL_TandP_Efficiency(){


}
 
HNL_TandP_Efficiency::~HNL_TandP_Efficiency(){

}

