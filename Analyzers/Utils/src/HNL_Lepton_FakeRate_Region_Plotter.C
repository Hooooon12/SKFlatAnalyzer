#include "HNL_Lepton_FakeRate_Region_Plotter.h"

void HNL_Lepton_FakeRate_Region_Plotter::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

  TString AnalyzerPath=getenv("SKFlat_WD"), SKFlatV = getenv("SKFlatV");
  TString FilePath="/data/"+SKFlatV+"/"+GetEra()+"/FakeRate/NvtxSF/";
  NvtxSFFile = new TFile(AnalyzerPath+FilePath+"FakeRateNVtx13TeV_"+GetEra()+".root");

}

void HNL_Lepton_FakeRate_Region_Plotter::executeEvent(){

  Event ev = GetEvent();

  //************************************************///
  // setup list of IDs and jobs

  vector<AnalyzerParameter> VParameters;
  
  if(HasFlag("MakeRegionPlotsL")){
    VParameters.clear();
    VParameters.push_back(SetupFakeParameter(AnalyzerParameter::Central,
					     MuMu,
					     HNL_LeptonCore::NormTo1Invpb,
					     {"MakeSingleLeptonPlots"},
					     "HNL_ID_MakeSingleLeptonPlots"     ,"HNL_ULID_"+GetYearString(), "HNL_ULID_FO"));

    VParameters.push_back(SetupFakeParameter(AnalyzerParameter::Central,
					     EE,
					     HNL_LeptonCore::NormTo1Invpb,
					     {"MakeSingleLeptonPlots"},
					     "HNL_ID_MakeSingleLeptonPlots"     ,"HNL_ULID_"+GetYearString(), "HNL_ULID_FO"));

    goto RunJobs;
  }

  if(HasFlag("MakeRegionPlotsLL")){
    VParameters.clear();
    VParameters.push_back(SetupFakeParameter(AnalyzerParameter::Central,
					     MuMu,
					     HNL_LeptonCore::NormTo1Invpb,
					     {"MakeDiLeptonPlots"}    ,
					     "HNL_ID_MakeDiLeptonPlots"         ,"HNL_ULID_"+GetYearString(), "HNL_ULID_FO"));
    VParameters.push_back(SetupFakeParameter(AnalyzerParameter::Central,
					     EE,HNL_LeptonCore::NormTo1Invpb,
					     {"MakeDiLeptonPlots"}    ,
					     "HNL_ID_MakeDiLeptonPlots"         ,"HNL_ULID_"+GetYearString(), "HNL_ULID_FO"));
    
    goto RunJobs;
  }


 RunJobs:
  
  for(auto param : VParameters)  executeEventFromParameter(param);
  
}

void HNL_Lepton_FakeRate_Region_Plotter::executeEventFromParameter(AnalyzerParameter param){
   
  //************************************************///
  // setup event level objects
  Event ev = GetEvent();

  double weight = SetupWeight(ev, param);
  if(IsData) weight = 1;
  
  if(!PassMETFilter()) return;
  
  /// check pt is not changed
  std::vector<Electron> Initial_loose_electrons     = SelectElectrons( param,param.Electron_Loose_ID, 10, 2.5) ;
  std::vector<Muon>     Initial_loose_muons         = SelectMuons    ( param,param.Muon_Loose_ID,     5,  2.4);

  if(_jentry < 10)    cout << "param.Electron_Loose_ID = " << param.Electron_Loose_ID << " nel = " << Initial_loose_electrons.size() << endl;
  if(_jentry < 10)    cout << "param.Muon_Loose_ID = "     << param.Muon_Loose_ID     << " nmu = " << Initial_loose_muons.size() << endl;
 

  std::vector<Electron> Loose_Electrons;
  std::vector<Muon>     Loose_Muons;
  for(auto ilep : Initial_loose_electrons) Loose_Electrons.push_back(ilep);
  for(auto ilep : Initial_loose_muons)   Loose_Muons.push_back(ilep);
  
  //// is 20 correct?
  std::vector<Jet> jets_tmp     = SelectJets   ( param, "tight", 20., 2.7);
  std::vector<Jet> jets; 
  for(unsigned int ijet =0; ijet < jets_tmp.size(); ijet++){
    bool jetok=true;
    for(unsigned int iel=0 ; iel < Initial_loose_electrons.size(); iel++)   { if(jets_tmp[ijet].DeltaR(Initial_loose_electrons[iel]) < 0.4) jetok = false;}
    for(unsigned int iel=0 ; iel < Initial_loose_muons.size(); iel++)       { if(jets_tmp[ijet].DeltaR(Initial_loose_muons[iel]) < 0.4)     jetok = false;}
    if(jetok) jets.push_back(jets_tmp[ijet]);
  }
  if(param.Channel=="EE")   RunE(Loose_Electrons,Loose_Muons, jets,  param, weight);
  if(param.Channel=="MuMu") RunM(Loose_Electrons,Loose_Muons,  jets, param, weight);
  
}


void HNL_Lepton_FakeRate_Region_Plotter::RunM(std::vector<Electron> loose_el,  std::vector<Muon> loose_mu, std::vector<Jet> jets,   AnalyzerParameter param,  float event_weight){

  if(IsData){
    if(this->DataStream == "DoubleEG") return;
    if(this->DataStream == "SingleElectron") return;
    if(this->DataStream == "EGamma") return;
  }

  Event ev = GetEvent();

  if(loose_mu.size() == 0) return;

  // remove if muon                                                                                                                          
  if(loose_el.size() > 0) return;

  std::vector<Lepton *> leps = MakeLeptonPointerVector(loose_mu,param);

  std::vector<bool>    blepsT;
  for(auto ilep : loose_mu)   blepsT.push_back(ilep.PassID(param.Muon_Tight_ID));
  
  Particle METv = GetvMET("PuppiT1xyULCorr",param);

  if(param.HasFlag("MakeDiLeptonPlots"))  MakeDiLepPlots(MuMu,param, ev, leps,blepsT,param.Name+param.Channel,event_weight);
  
  if(!param.HasFlag("MakeSingleLeptonPlots")) return;

  if(loose_mu.size() != 1) return;
  
  bool has_away_jet=false;
  for(unsigned int ij=0; ij < jets.size(); ij++){
    if(jets.at(ij).Pt() < 40.) continue;
    float dphi =fabs(TVector2::Phi_mpi_pi(loose_mu[0].Phi()- jets.at(ij).Phi()));
    if(dphi > 2.5)      has_away_jet=true;
  }

  /// Check Single lepon

  TString triggerslist_8="HLT_Mu8_TrkIsoVVL_v";
  TString triggerslist_17="HLT_Mu17_TrkIsoVVL_v";
  TString trigger_unprescale = TrigList_POG_Mu[0];

  bool Mu8PD  = (!IsDATA || (IsDATA&& ev.IsPDForTrigger(triggerslist_8,     this->DataStream) ));
  bool Mu17PD = (!IsDATA || (IsDATA&& ev.IsPDForTrigger(triggerslist_17,    this->DataStream) ));
  bool IsoMuPD= (!IsDATA || (IsDATA&& ev.IsPDForTrigger(trigger_unprescale, this->DataStream) ));

  bool pass_8     = ev.PassTrigger(triggerslist_8)     && Mu8PD;
  bool pass_17    = ev.PassTrigger(triggerslist_17)    && Mu17PD;
  bool pass_IsoMu = ev.PassTrigger(trigger_unprescale) && IsoMuPD;

  
  double PFWeight = GetPrefireWeight(0);
  
  double NVxt_Mu8 =  ApplyNvtxReweight(nPV,triggerslist_8);
  double NVxt_Mu17 =  ApplyNvtxReweight(nPV,triggerslist_17);
  double NVtx_UnPre = GetPileUpWeight(nPileUp,0);


  bool truth_match= false;
  if(!IsDATA) {
    if(loose_mu.at(0).IsPrompt()) truth_match=true;
  }
  else truth_match=true;

  bool isTight =  blepsT[0]; 
 
  bool IsPromptLike=false;
  for(unsigned int ij=0; ij < jets.size(); ij++){
    if(jets.at(ij).Pt() < 40.) continue;
    float dphi =fabs(TVector2::Phi_mpi_pi(loose_mu[0].Phi()- jets.at(ij).Phi()));
    if(dphi > 2.5){
      if((jets.at(ij).Pt() /  loose_mu[0].Pt() ) < 1.) IsPromptLike = true;
    }
  }

  Double_t MT=0;
  Double_t METdphi=0;
  for(unsigned int imu = 0; imu < loose_mu.size();  imu++){
    METdphi=  TVector2::Phi_mpi_pi((loose_mu.at(imu).Phi()- METv.Phi()));
    MT = sqrt(2.* loose_mu.at(imu).Et()*METv.Pt() * (1 - cos( METdphi)));
  }

  bool PromptRegion = ((60. < MT)  &&(MT < 150.) && IsPromptLike); 

  if(pass_8){
    if(loose_mu.at(0).Pt() >= 10){
      double prescale_trigger = (IsDATA) ? 1. : ev.GetTriggerLumi(triggerslist_8)*NVxt_Mu8;
      FillRegionPlots("MuMu","Mu8/All_SingleLooseMuon_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);
      if(isTight) FillRegionPlots("MuMu","Mu8/All_SingleLooseMuon_TightMu_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);
      if(PromptRegion){
	FillRegionPlots("MuMu","Mu8/PromptCR_SingleLooseMuon_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);
	if(isTight) FillRegionPlots("MuMu","Mu8/PromptCR_SingleLooseMuon_TightMu_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);
      }
    }
  }
  if(pass_17){
    if(loose_mu.at(0).Pt() >= 20){
      double prescale_trigger = (IsDATA) ? 1. : ev.GetTriggerLumi(triggerslist_17)*NVxt_Mu17;
      FillRegionPlots("MuMu","Mu20/All_SingleLooseMuon_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);
      if(isTight) FillRegionPlots("MuMu","Mu8/All_SingleLooseMuon_TightMu_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);
      if(PromptRegion){
        FillRegionPlots("MuMu","Mu20/PromptCR_SingleLooseMuon_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);
        if(isTight) FillRegionPlots("MuMu","Mu8/PromptCR_SingleLooseMuon_TightMu_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);
      } 
    }
  }
  if(pass_IsoMu){
    if(loose_mu.at(0).Pt() >= 30){
      double prescale_trigger = (IsDATA) ? 1. : ev.GetTriggerLumi(trigger_unprescale)*NVtx_UnPre;
      FillRegionPlots("MuMu","IsoMu/All_SingleLooseMuon_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);
      if(isTight) FillRegionPlots("MuMu","Mu8/All_SingleLooseMuon_TightMu_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);

      if(PromptRegion){
        FillRegionPlots("MuMu","IsoMu/PromptCR_SingleLooseMuon_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);
        if(isTight) FillRegionPlots("MuMu","Mu8/PromptCR_SingleLooseMuon_TightMu_"+param.Name, jets,   loose_el,  loose_mu,  METv, event_weight*PFWeight*prescale_trigger);
      } 
    }
  }

 
  bool useevent40 = UseEvent(leps , jets, 40., METv, event_weight*PFWeight);
  if(useevent40&&has_away_jet){
    float prescale_trigger =  1;//GetPrescale(leps);

    if (blepsT[0] && jets.size() >= 1)FillRegionPlots("MuMu","MeasureRegion/TightMu40_notm_"+param.Name, jets,  loose_el,loose_mu, METv, event_weight*PFWeight*prescale_trigger);
    if (jets.size() >= 1)FillRegionPlots("MuMu","MeasureRegion/LooseMu40_notm_"+param.Name, jets,  loose_el,loose_mu, METv, event_weight*PFWeight*prescale_trigger);
    
    if(truth_match){
      if (blepsT[0] && jets.size() >= 1)FillRegionPlots("MuMu","MeasureRegion/TightMu40_"+param.Name , jets,  loose_el,loose_mu, METv, event_weight*PFWeight*prescale_trigger);
      if (jets.size() >= 1)FillRegionPlots("MuMu","MeasureRegion/LooseMu40_"+param.Name, jets,  loose_el,loose_mu, METv, event_weight*PFWeight*prescale_trigger);
    }    
  }
  
  return;

} /// end function                      





void HNL_Lepton_FakeRate_Region_Plotter::RunE( std::vector<Electron> loose_el, std::vector<Muon> loose_mu, std::vector<Jet> jets, AnalyzerParameter param,  float event_weight){

  if(IsData){
    if(this->DataStream == "DoubleMuon") return;
    if(this->DataStream == "SingleMuon") return;
  }

  Event ev = GetEvent();

  if(loose_el.size() == 0) return;
 
  // remove if muon 
  if(loose_mu.size() > 0) return;

  Particle METv = GetvMET("PuppiT1xyULCorr",param);

  std::vector<Lepton *> leps = MakeLeptonPointerVector(loose_el,param);

  std::vector<bool> blepsT;
  for(auto ilep : loose_el)   blepsT.push_back(ilep.PassID(param.Electron_Tight_ID));
  
  if(param.HasFlag("MakeDiLeptonPlots"))   MakeDiLepPlots(EE,param, ev, leps,blepsT,param.Name+param.Channel,event_weight);
  
  if(!param.HasFlag("MakeSingleLeptonPlots")) return;

  if(loose_el.size() != 1) return;

  bool has_away_jet=false;
  for(unsigned int ij=0; ij < jets.size(); ij++){
    if(jets.at(ij).Pt() < 40.) continue;
    float dphi =fabs(TVector2::Phi_mpi_pi(loose_el[0].Phi()- jets.at(ij).Phi()));
    if(dphi > 2.5){
      has_away_jet=true;
    }
  } 


  float prescale_trigger =  1;//GetPrescale(leps);
  if(prescale_trigger == 0.) return;
  event_weight*=prescale_trigger;


  if(loose_el[0].Pt() < 20) return;

  bool truth_match= false;
  if(!IsDATA) {
    if(loose_el.at(0).LeptonGenType() > 0 ) truth_match=true;
  }
  else truth_match=true;

  if(has_away_jet){
    FillRegionPlots("EE","SingleLooseElAwayJet_"+param.Name , jets,   loose_el,loose_mu,  METv, event_weight);
    if(blepsT[0]) FillRegionPlots("EE","SingleTightElAwayJet_"+param.Name, jets,  loose_el, loose_mu,  METv, event_weight);
  }
  
  Double_t MT=0;
  Double_t METdphi=0;
  for(unsigned int iel = 0; iel < loose_el.size();  iel++){
    METdphi = TVector2::Phi_mpi_pi((loose_el.at(iel).Phi()- METv.Phi()));
    MT = sqrt(2.* loose_el.at(iel).Et()*METv.Pt() * (1 - cos( METdphi)));
  }
  if((60. < MT)  &&(MT < 100.)){
    if(loose_el[0].Pt() > 25){
      if(has_away_jet && blepsT[0]){
	
	FillRegionPlots("EE","SingleTightElAwayJet_promptCR_"+param.Name, jets,  loose_el,loose_mu, METv, event_weight);
	
	bool isPrompt=false;
	for(unsigned int ij=0; ij < jets.size(); ij++){
	  if(jets.at(ij).Pt() < 40.) continue;
	  float dphi =fabs(TVector2::Phi_mpi_pi(loose_el[0].Phi()- jets.at(ij).Phi()));
	  if(dphi > 2.5){
	    if((jets.at(ij).Pt() /  loose_el[0].Pt() ) < 1.) isPrompt = true;
	  }
	}
	if (isPrompt ) FillRegionPlots("EE","SingleTightElAwayJet_promptCR2_"+param.Name, jets,  loose_el,loose_mu, METv, event_weight);
      }
    }
  }


  bool useevent40 = UseEvent(leps , jets, 40., METv,event_weight);
  if(useevent40){
    if (blepsT[0] && jets.size() >= 1)FillRegionPlots("EE","MeasureRegion_TightEl40_notm_"+param.Name, jets,  loose_el,loose_mu, METv, event_weight);
    if (jets.size() >= 1)FillRegionPlots("EE","MeasureRegion_LooseEl40_notm_"+param.Name, jets,  loose_el,loose_mu, METv, event_weight);
    
    
    if(truth_match){
      if (blepsT[0] && jets.size() >= 1)FillRegionPlots("EE","MeasureRegion_TightEl40_"+param.Name , jets,  loose_el,loose_mu, METv, event_weight);
      if (jets.size() >= 1)FillRegionPlots("EE","MeasureRegion_LooseEl40_"+param.Name, jets,  loose_el,loose_mu, METv, event_weight);
    }
  }
  
  
  return;
  


} /// end function RunE

double HNL_Lepton_FakeRate_Region_Plotter::ApplyNvtxReweight(int NPV, TString Key){

  if(IsData) return 1.;
  if(!NvtxSFFile->GetListOfKeys()->Contains(Key)){ printf("[Error] No %s in NvtxSF File.\n", Key.Data()); return -1.; }

  std::map<TString, TH1D*>::iterator mapit = maphist_NvtxSF.find(Key);
  if(mapit == maphist_NvtxSF.end()){ //first usage
    maphist_NvtxSF[Key] = (TH1D*) ((TH1*) NvtxSFFile->Get(Key))->Clone();
  }
  mapit = maphist_NvtxSF.find(Key);
  
  int this_bin = mapit->second->FindBin(NPV);
  double NvtxSF = mapit->second->GetBinContent(this_bin);
  if(NvtxSF>10) NvtxSF=1.;

  return NvtxSF;

}


void HNL_Lepton_FakeRate_Region_Plotter::MakeDiLepPlots(HNL_LeptonCore::Channel channel, AnalyzerParameter param, Event ev, std::vector<Lepton *> leps,std::vector<bool> blepsT,  TString label, float event_weight){

  double d_event_weight = event_weight;
  if(leps.size() != 2) return;
  if(!blepsT[0] || !blepsT[1]) return;

  // now we have 2 Tight leptons                                                                                                                             

  TString plot_dir = GetChannelString(channel);

  // Make Z peak                                                                                                                                            
  Particle Z = (*leps[0]) + (*leps[1]);
  if(leps[0]->Charge() == leps[1]->Charge()) return;
  bool PassZMass = (fabs(M_Z - Z.M()) < 10.) ? true : false;

  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param); 
  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);

  EvalJetWeight(AK4_JetColl, AK4_JetColl,AK8_JetColl, d_event_weight, param);

  if(AK4_BJetColl.size() > 0) return;


  if(channel == MuMu){

    TString triggerslist_8="HLT_Mu8_TrkIsoVVL_v";
    TString triggerslist_17="HLT_Mu17_TrkIsoVVL_v";

    double NVxt_Mu8 =  ApplyNvtxReweight(nPV,triggerslist_8);
    double NVxt_Mu17 =  ApplyNvtxReweight(nPV,triggerslist_17);

    bool Mu8PD= (!IsDATA || (IsDATA&& ev.IsPDForTrigger(triggerslist_8, this->DataStream) ));
    bool Mu17PD= (!IsDATA || (IsDATA&& ev.IsPDForTrigger(triggerslist_17, this->DataStream) ));

    bool pass_8 = ev.PassTrigger(triggerslist_8) && Mu8PD;
    bool pass_17 = ev.PassTrigger(triggerslist_17) && Mu17PD;

    if(pass_8){
      if(leps[1]->Pt() > 10){
        double prescale_weight  =  (IsDATA) ? 1. : ev.GetTriggerLumi(triggerslist_8)*NVxt_Mu8;

        if(PassZMass){
	  for(auto ilep : leps)FillHistogram(( plot_dir +  "/RegionPlots_Dilep/"+triggerslist_8 + "_LepPt").Data(), ilep->Pt() , d_event_weight*prescale_weight,"PrescaledTriggerPt");
        }
        FillHist(( plot_dir +  "/RegionPlots_Dilep/"+triggerslist_8 + "_LLMass").Data(), Z.M() , d_event_weight*prescale_weight, 100., 0., 200);

      }
    }
    if(pass_17){
      if(leps[1]->Pt() > 20){
        double prescale_weight  =  (IsDATA) ? 1. : ev.GetTriggerLumi(triggerslist_17)*NVxt_Mu17;
        if(PassZMass){
          for(auto ilep : leps)FillHistogram(( plot_dir +  "/RegionPlots_Dilep/"+triggerslist_17 + "_LepPt").Data(), ilep->Pt() , d_event_weight*prescale_weight,"PrescaledTriggerPt");
        }
        FillHist(( plot_dir +  "/RegionPlots_Dilep/"+triggerslist_17 + "_LLMass").Data(), Z.M() , d_event_weight*prescale_weight, 100., 0., 200);
      }
    }
  }
  
  if(channel == EE){

    TString triggerslist_12="HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v";
    TString triggerslist_23="HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v";

    bool EL12PD= (!IsDATA || (IsDATA&& ev.IsPDForTrigger(triggerslist_12, this->DataStream) ));
    bool EL23PD= (!IsDATA || (IsDATA&& ev.IsPDForTrigger(triggerslist_23, this->DataStream) ));

    bool pass_12 = ev.PassTrigger(triggerslist_12) && EL12PD ;
    bool pass_23 = ev.PassTrigger(triggerslist_23) && EL23PD;

    double NVxt_El12 =  ApplyNvtxReweight(nPV,triggerslist_12);
    double NVxt_El23 =  ApplyNvtxReweight(nPV,triggerslist_23);


    if(pass_12){
      if(leps[1]->Pt() > 14){
        double prescale_weight  =  (IsDATA) ? 1. : ev.GetTriggerLumi(triggerslist_12)*NVxt_El12;
        if(PassZMass){
          for(auto ilep : leps)FillHistogram(( plot_dir +  "/RegionPlots_Dilep/"+triggerslist_12 + "_LepPt").Data(), ilep->Pt() , d_event_weight*prescale_weight,"PrescaledTriggerPt");
        }
        FillHist(( plot_dir +  "/RegionPlots_Dilep/"+triggerslist_12 + "_LLMass").Data(), Z.M() , d_event_weight*prescale_weight, 100., 0., 200);
      }
    }

    if(pass_23){
      if(leps[1]->Pt() > 25){
        double prescale_weight  =  (IsDATA) ? 1. : ev.GetTriggerLumi(triggerslist_23)*NVxt_El23;
        if(PassZMass){
          for(auto ilep : leps)FillHistogram(( plot_dir +  "/RegionPlots_Dilep/"+triggerslist_23 + "_LepPt").Data(), ilep->Pt() , d_event_weight*prescale_weight,"PrescaledTriggerPt");
        }
        FillHist(( plot_dir +  "/RegionPlots_Dilep/"+triggerslist_23 + "_LLMass").Data(), Z.M() , d_event_weight*prescale_weight, 100., 0., 200);
      }
    }
  }

  return;


}




bool HNL_Lepton_FakeRate_Region_Plotter::UseEvent(std::vector<Lepton *> leps ,  std::vector< Jet> jets, float awayjetcut, Particle MET, float wt){

  bool useevent = false;
  if(leps.size() != 1) return false;
  
  Double_t MT=0;
  Double_t METdphi=0;
  for(unsigned int w = 0; w < leps.size();  w++){
    METdphi = TVector2::Phi_mpi_pi(leps.at(w)->Phi()-MET.Phi());
    MT = sqrt(2.* leps.at(w)->Et()*MET.Pt() * (1 - cos( METdphi)));
    if(( (MET.Pt() < 40) && (MT < 25.)) ) {

      for (unsigned int ielT=0; ielT < leps.size(); ielT++){
        for(unsigned int ij=0; ij < jets.size(); ij++){
          if(jets.at(ij).Pt() < awayjetcut) continue;
          float dphi =fabs(TVector2::Phi_mpi_pi(leps.at(ielT)->Phi()- jets.at(ij).Phi()));
	  if( (jets.at(ij).ChargedEmEnergyFraction()) > 0.65)  continue;

          if(dphi > 2.5){
            if((jets.at(ij).Pt() /  leps.at(ielT)->Pt() ) < 1.2) continue;
            useevent = true;
          }
	}
      }
    }
  }

  return useevent;
}


HNL_Lepton_FakeRate_Region_Plotter::HNL_Lepton_FakeRate_Region_Plotter(){
  
}
 
HNL_Lepton_FakeRate_Region_Plotter::~HNL_Lepton_FakeRate_Region_Plotter(){

  //  delete NvtxSFFile;
  //for(std::map< TString, TH1D* >::iterator mapit = maphist_NvtxSF.begin(); mapit!=maphist_NvtxSF.end(); mapit++){
  //  delete mapit->second;
  // }
  //maphist_NvtxSF.clear();

}

void HNL_Lepton_FakeRate_Region_Plotter::FillRegionPlots( TString plot_dir, TString region,  std::vector<Jet> jets,  std::vector<Electron> els, std::vector<Muon> mus, Particle  met,  double w){
  
  if((els.size()+ mus.size()) != 1) return;
  Lepton lep1;
  if(els.size()==1) lep1=els[0];
  else lep1 = mus[0];

  FillHistogram( plot_dir +  "/RegionPlots_"+ region+ "/Lep1_binned_pt", lep1.Pt()  , w, "FR_pt","p_{T} GeV");
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/Lep1_pt", lep1.Pt()  , w, 400, 0., 2000.,"p_{T} GeV");
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/Lep1_eta", lep1.Eta()  , w, 30, -3., 3,"#eta");
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/Lep1_CloseJet_Ptrel", lep1.CloseJet_Ptrel(), w, 50., 0., 2.,"PtRel");
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/Lep1_CloseJet_Ptratio", lep1.CloseJet_Ptratio(), w, 50., 0., 2.,"PtRel");

  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/Met", met.Pt()  , w, 200, 0., 400.,"MET GeV");
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/MetPhi", met.Phi()  , w, 100, -5, 5,"MET GeV");
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/njets" , jets.size() , w, 5, 0., 5., "N_{jets}");

  float lep_reliso  = lep1.RelIso();
  float lep_minireliso  = lep1.MiniRelIso();
  float lep_ip3d    = fabs(lep1.IP3D()/lep1.IP3Derr());
  float lep_mva     =  lep1.LepMVA();
  float lep_dxy     = fabs(lep1.dXY());

  if(els.size() > 0)   FillHist( plot_dir +  "/RegionPlots_"+ region+ "/NMissingHits", els[0].NMissingHits(), w, 5, 0., 5.);


  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/Reliso", lep_reliso, w, 50, 0., 1.);
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/MiniReliso", lep_minireliso, w, 50, 0., 1.);
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/dXY",    lep_dxy, w, 100, 0., 0.5);
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/IP3D",   lep_ip3d, w, 50, 0., 10.);
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/Mva",    lep_mva, w, 50, -1., 1.);



  double lep_jet_ratio= 0.;
  double jet_CEEF = 0.;  double jet_CHEF = 0.;
  for(unsigned int ij=0; ij < jets.size(); ij++){
    if(jets.at(ij).Pt() < 40.) continue;
    float dphi =fabs(TVector2::Phi_mpi_pi(lep1.Phi()- jets.at(ij).Phi()));
    if(dphi > 2.5){
      lep_jet_ratio = jets.at(ij).Pt() /  lep1.Pt();
      jet_CEEF = jets.at(ij).ChargedEmEnergyFraction();
      jet_CHEF = jets.at(ij).ChargedHadEnergyFraction();
    }
  }
  
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/JetLepPtRatio" , lep_jet_ratio  , w, 50, 0., 5.,"P^{jet}_{T} / P^{lepton}_{T}");
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/AwayJetChargedEMEnergyFraction" , jet_CEEF  , w, 50, 0., 2.,"Jet charged EMF");
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/AwayJetChargedHadEnergyFraction" , jet_CHEF  , w, 50, 0., 2.,"Jet charged EMF");
  


  double METdphi = TVector2::Phi_mpi_pi(lep1.Phi()-met.Phi());
  double MT = sqrt(2.* lep1.Et()*met.Pt() * (1 - cos( METdphi)));
  
  FillHist( plot_dir +  "/RegionPlots_"+ region+ "/Mt" , MT  , w, 40, 0., 200.,"MET GeV");
  
}


void HNL_Lepton_FakeRate_Region_Plotter::FillEventCutflow(int charge_i, int cf,float wt, TString cut,   TString label){


  return;

}

void HNL_Lepton_FakeRate_Region_Plotter::FillWeightHist(TString label, double _weight){
  FillHist( "weights/"+ label , _weight ,1., 200, 0., 5,"ev weight");
}
void HNL_Lepton_FakeRate_Region_Plotter::FillCutFlow(bool IsCentral, TString suffix, TString histname, double weight){

  if(IsCentral){

    FillHist(suffix+"/"+histname, 0., weight, 1, 0., 1.);

  }

}

