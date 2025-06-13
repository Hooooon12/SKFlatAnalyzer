#include "HNL_TandP_Efficiency.h"

void HNL_TandP_Efficiency::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

}


void HNL_TandP_Efficiency::executeEvent(){

  
  //==== Gen for genmatching
  AnalyzerParameter param  = InitialiseHNLParameter("");
  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);

  //RunHighPt(param,weight);
  
  RunTaPMuon("HNL_HighPt_ULID",param,weight);
  //  RunTaPMuon("POGHighPtTightWithIP",param,weight);
  //RunTaPMuon("POGTightWithTightIso",param,weight);
  RunTaPMuon("passTightID",param,weight);
  RunTaPMuon("passHEEPID_v1",param,weight);
  RunTaPMuon("Peking",param,weight);
  //  RunTaPMuon("TopHN",param,weight);
  RunTaPMuon("passMVAID_Iso_WP90",param,weight);
  RunTaPMuon("passMVAID_noIso_WP90",param,weight);
  //RunPeking(param,weight);

}

void HNL_TandP_Efficiency::RunHighPt(AnalyzerParameter param, double weight){

  TString ID = "POGHighPt";

  AnalyzerParameter p = HNL_LeptonCore::InitialiseHNLParameter("Basic");
  Event ev = GetEvent();
  vector<Electron> electrons= GetAllElectrons();
  
  if(DataYear == 2016){
    if(! (ev.PassTrigger("HLT_Ele27_WPTight_Gsf_v")))return;
  }
  if(DataYear == 2017){
    if(! (ev.PassTrigger("HLT_Ele32_WPTight_Gsf_L1DoubleEG_v"))) return;
  }
  if(DataYear == 2018){
    if(! (ev.PassTrigger("HLT_Ele32_WPTight_Gsf_v"))) return;
  }

  double EvWeight=p.w.lumiweight*p.w.PUweight*p.w.prefireweight*p.w.zptweight*p.w.z0weight*p.w.weakweight;
  
  if(electrons.size() == 2){
    
    int nPtbins=14;
    double Ptbins[nPtbins+1] = { 20.,25.,30., 40.,50., 70., 100.,  150.,  200.,350,500., 750,1000,1500,2000};

    if(1){
      Electron el_tag = electrons[0];
      Electron el_probe = electrons[1];
      if(fabs(el_tag.scEta())<1.4442 && el_tag.passHEEPID() && el_tag.PassPath("HLT_Ele32_WPTight_Gsf_L1DoubleEG_v") &&el_tag.Pt() > 35 && el_tag.IsPrompt()){
	if(el_probe.IsFake() && el_probe.Pt() > 10){
	  FillHist("Fake_QCD_All",el_probe.Pt() ,  EvWeight, nPtbins,Ptbins);
	  if(el_probe.PassID("HNL_HighPt_ULID"))           FillHist("Fake_QCD_Pass",el_probe.Pt() ,  EvWeight, nPtbins,Ptbins);
	  else   FillHist("Fake_QCD_Fail",el_probe.Pt() ,  EvWeight, nPtbins,Ptbins);
	}
      }
    }
    if(1){
      Electron el_tag = electrons[1];
      Electron el_probe = electrons[0];
      if(fabs(el_tag.scEta())<1.4442 && el_tag.passHEEPID() && el_tag.PassPath("HLT_Ele32_WPTight_Gsf_L1DoubleEG_v") &&el_tag.Pt() > 35 && el_tag.IsPrompt()){
	if(el_probe.IsFake() && el_probe.Pt() > 10){
          FillHist("Fake_QCD_All",el_probe.Pt() ,  EvWeight, nPtbins,Ptbins);
	  if(el_probe.PassID("HNL_HighPt_ULID"))           FillHist("Fake_QCD_Pass",el_probe.Pt() ,  EvWeight, nPtbins,Ptbins);
          else FillHist("Fake_QCD_Fail",el_probe.Pt() ,  EvWeight, nPtbins,Ptbins);
        }
      }
    }
  }
  return;

  if(!PassMETFilter()) return;
  
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



bool HNL_TandP_Efficiency::IsGoodTagProbe(Electron el_tag, Electron el_probe){

  // https://indico.cern.ch/event/1255216/contributions/5273071/attachments/2594851/4478919/HEEP%20ID%202016UL%20for%20EGamma.pdf                                           
  //if(el_probe.Pt() < 35) return false;                                                                                                                                    
  if(fabs(el_probe.scEta()) >2.5) return false;
  if(el_probe.etaRegion()==Electron::GAP) return false;

  //  if((el_tag+el_probe).M()<70) return false;                                                                                                                            
  //  if((el_tag+el_probe).M()>110) return false;                                                                                                                           
  //if((el_tag.Charge() + el_probe.Charge()) != 0)  return false;                                                                                                           
  return true;
}



bool HNL_TandP_Efficiency::IsTag(Electron el_tag){
  // https://indico.cern.ch/event/1255216/contributions/5273071/attachments/2594851/4478919/HEEP%20ID%202016UL%20for%20EGamma.pdf                                           

  // Within barrel                                                                                                                                                          
  if(fabs(el_tag.scEta())>1.4442) return false;
  // Pass HEEP                                                                                                                                                              
  if(DataYear == 2018){
    if(!el_tag.passHEEP2018Prompt()) return false;
  }
  else{
    if(!el_tag.passHEEPID()) return false;
  }

  // Match trigger in data                                                                                                                                                  
  //  if(IsDATA){                                                                                                                                                           
  if(DataYear == 2016){
    //if(!el_tag.PassPath("HLT_Ele27_eta2p1_WPTight_Gsf_v")) return false;                                                                                                  
    if(!el_tag.PassPath("HLT_Ele27_WPTight_Gsf_v")) return false;
  }
  else if(DataYear == 2017){
    if(!el_tag.PassPath("HLT_Ele32_WPTight_Gsf_L1DoubleEG_v")) return false;
  }
  else if(DataYear == 2018){
    if(!el_tag.PassPath("HLT_Ele32_WPTight_Gsf_v")) return false;
  }

  return true;
}


void HNL_TandP_Efficiency::RunTaPMuon(TString ID, AnalyzerParameter param, double weight){

  Event ev = GetEvent();


  vector<Electron> electrons= GetAllElectrons();
  AnalyzerParameter p = HNL_LeptonCore::InitialiseHNLParameter("HNL_ULIDv2");

  if(DataYear == 2016){
    if(! (ev.PassTrigger("HLT_Ele27_WPTight_Gsf_v")))return;
  }
  if(DataYear == 2017){
    if(! (ev.PassTrigger("HLT_Ele32_WPTight_Gsf_L1DoubleEG_v"))) return;
  }
  if(DataYear == 2018){
    if(! (ev.PassTrigger("HLT_Ele32_WPTight_Gsf_v"))) return;
  }


  if(!PassMETFilter()) return;
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", p);
  if(AK4_BJetColl.size() > 0) return;

  double EvWeight=1.;
  if(!IsDATA)   EvWeight=p.w.lumiweight*p.w.PUweight*p.w.prefireweight*p.w.z0weight*p.w.weakweight;




  int nTagPair(0);
  for(Electron& tag:electrons){

    if(!IsTag(tag)) continue;

    bool HasPair=false;
    for(Electron& probe:electrons){
      if(&tag==&probe) continue;
      if(!IsGoodTagProbe(tag,probe)) continue;
      HasPair=true;
    }
    if(HasPair) nTagPair++;
  }
  
  vector<pair<Electron,Electron> > matched_pair_electrons;
  if(nTagPair==0) return;
  else if(nTagPair==1){

    double pt_probe_highest = 0;
    for(Electron& tag:electrons){
      if(!IsTag(tag)) continue;
      //// Loop over probe candidates                                                                                                                                     
      for(Electron& probe:electrons){

	if(&tag==&probe) continue;
	if(!IsGoodTagProbe(tag,probe)) continue;

	/// Check highest pt                                                                                                                                              
	if(probe.Pt() > pt_probe_highest) {
	  pt_probe_highest = probe.Pt();

	  if(matched_pair_electrons.size() == 2){
	    /// Reset to update Highest Pt                                                                                                                                
	    matched_pair_electrons.pop_back();
	  }
	  matched_pair_electrons.push_back(make_pair(tag,probe));
	}
      }
    }
  }
  else if(nTagPair>1){

    //// Since there are more than one pair with different tags  need to check first if one pair has two tags                                                             

    for(Electron& tag:electrons){
      double pt_probe_highest_pt = 0;
      if(!IsTag(tag)) continue;
      Electron probe_assigned;
      for(Electron& probe:electrons){
	if(&tag==&probe) continue;

	if(!IsGoodTagProbe(tag,probe)) continue;

	if(probe.Pt() > pt_probe_highest_pt) {
	  pt_probe_highest_pt = probe.Pt();
	  probe_assigned = probe;
	}
      }// probe loop                                                                                                                                                      
      matched_pair_electrons.push_back(make_pair(tag,probe_assigned));
    } // tag loop                                                                                                                                                         
  } // multi Tag pairs loop                                                                                                                                               

  int nPairs_counter=-1; /// Needed to access Gen Info                                                                                                                    
  for(auto t_p_pair : matched_pair_electrons){
    nPairs_counter++;

    vector<Electron> vProbe = {t_p_pair.second};  // fill vector to keep structure of code same                                                                           
    Electron tag = t_p_pair.first;
    if(!IsTag(tag)) continue;
    double totWeight = EvWeight;
    
    if(!IsDATA) totWeight = totWeight * mcCorr->ElectronID_SF("HEEP",tag.scEta(), tag.Pt(), 0);
    
    if(!tag.IsPrompt()) continue;
    for(Electron& probe:vProbe){

      if(&tag==&probe) continue;
      if(!IsGoodTagProbe(tag,probe)) continue;
      if(!probe.IsPrompt())continue;
      totWeight=totWeight* mcCorr->ElectronReco_SF("RECO_SF",tag.defEta(),tag.Pt(),0);
      totWeight=totWeight* mcCorr->ElectronReco_SF("RECO_SF",probe.defEta(),probe.Pt(),0);
      TString PtEtaBin = probe.GetEtaLabel() + "_"+ probe.GetPtLabel();
      Particle Z = probe+tag;
      if(probe.PassID(ID))FillHist(ID+"/Z_Mass_Pass_TandP_"+PtEtaBin,Z.M() , totWeight, 200, 0, 200);
      else                FillHist(ID+"/Z_Mass_Fail_TandP_"+PtEtaBin,Z.M() , totWeight, 200, 0, 200);
      
    }
  }
 
  return;

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
    
    //// Passes Tag                                                                                                                                                                                                                                                         
    for(unsigned int iprobe=0; iprobe <Muons.size(); iprobe++){
      
      if(iprobe==itag) continue;
      Muon probe = Muons[iprobe];
      
      if(!IsData){
	if(!probe.IsPrompt()) continue;
      }
      


      if((probe.TrkIso()/probe.TuneP4().Pt()) > 0.05) continue;
      if(probe.TrkIso() > 30)  continue;

      if(PassTandP_PairCriteria(Muons, itag,iprobe)){

        double pr_pt = probe.Pt();

        FillTandP(probe.PassID(ID), ID, ID ,pr_pt,probe.Eta(), 1,"Bin1");
        FillTandP(probe.PassID(ID), ID, ID ,pr_pt,probe.Eta(), 1,"Bin2");

      }/// OS   


      //// Probe pair                                                                                                                                                                                            
      if(PassTandP_PairCriteriaHighMass(Muons, itag,iprobe)){
	
	double pr_pt = probe.Pt();
	
	FillTandP(probe.PassID(ID), "HighMass_"+ID, ID ,pr_pt,probe.Eta(), 1,"Bin1");
	FillTandP(probe.PassID(ID), "HighMass_"+ID, ID ,pr_pt,probe.Eta(), 1,"Bin2");
	
      }/// OS                                                                                                                                                                                                                                                               
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
	
	double pr_pt =  probe.Pt();
	
	FillTandP(probe.PassID(ID),"Peking", ID ,pr_pt,probe.Eta(), 1,"Bin1");
	FillTandP(probe.PassID(ID),"Peking", ID ,pr_pt,probe.Eta(), 1,"Bin2");
	      
      }
    }
  }

}




HNL_TandP_Efficiency::HNL_TandP_Efficiency(){


}
 
HNL_TandP_Efficiency::~HNL_TandP_Efficiency(){

}

