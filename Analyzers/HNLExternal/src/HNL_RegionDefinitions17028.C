#include "HNL_RegionDefinitions17028.h"




void HNL_RegionDefinitions17028::RunEXO17028SignalRegions( std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons,  std::vector<Muon> muons_veto, std::vector<Jet> JetColl,std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev,   Particle METv, AnalyzerParameter param,   float weight_ll){
  
  vector<HNL_LeptonCore::Channel> channels = {EE,MuMu, EMu};

  for(auto dilep_channel : channels){

    if(MCSample.Contains("Type")) {
      if (!SelectChannel(dilep_channel)) continue;
    }

    //if(dilep_channel != EMu){
    //  if(MCSample.Contains("Type") &&(! MCSample.Contains("SS"))) weight_ll = weight_ll*2;
    // }
    AnalyzerParameter param_channel = param;

    TString channel_string = GetChannelString(dilep_channel);
    param_channel.Name =  "EXO17028_"+ channel_string + "/"+param.DefName;

    float weight_channel = weight_ll;


    if(!IsDATA && dilep_channel != MuMu)  weight_channel*= GetElectronIDWeight(electrons, param_channel);                                             
    if(!IsDATA && dilep_channel != EE)    weight_channel*= GetMuonIDWeight(muons, param_channel);                                                     

    FillCutflow(HNL_LeptonCore::ChannelDepInc, weight_channel, GetChannelString(dilep_channel) +"_NoCut",param);
    FillHist( GetChannelString(dilep_channel)+"_NoCut"  , 1.,  weight_channel, 2, 0.,2. , "N cut");

    std::vector<Lepton *> leps       = MakeLeptonPointerVector(muons,electrons,param_channel);
    std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(muons_veto,electrons_veto,param_channel);

    if(RunCF){

      if(dilep_channel == MuMu) continue;

      if(IsData && SameCharge(leps)) continue;
      if(!IsData && !SameCharge(leps)) continue;

      if(IsData)weight_channel = GetCFWeightElectron(leps, param_channel,-1);
      if(IsData)FillWeightHist(param.Name+"/CFWeight",weight_channel);
    }
    else{
      if(!SameCharge(leps)) continue;
    }

    if(RunFake){
      weight_channel = GetFakeWeight(leps, param_channel);
      FillWeightHist(param.Name+"/FakeWeight",weight_channel);
    }

    if(!PassPreselection(dilep_channel, Inclusive, leps, leps_veto, JetColl,  AK8_JetColl, B_JetColl,ev, METv ,param_channel,"", weight_channel)) continue;

    if(AK8_JetColl.size() > 0) RunSignalRegionAK8 (dilep_channel,Inclusive, leps, leps_veto, JetColl, AK8_JetColl, B_JetColl,ev, METv ,param_channel,"", weight_channel) ;
    else   RunSignalRegionAK4 (dilep_channel,Inclusive, leps, leps_veto, JetColl, AK8_JetColl, B_JetColl, ev, METv ,param_channel,"", weight_channel);
    
  }
  return;
    
}



bool  HNL_RegionDefinitions17028::PassPreselection(HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType qq, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,  std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param, TString PostLabel ,  float w){

  // ONLY CODE SS PRESLECTION 

  // APPLY PT CUTS
  /*
    ee channel 25/10
    emu   25/10
    mm    20/10

    THIS IS APPLIED IN PassTriggerSelection

   */

  if(run_Debug) cout << "HNL_RegionDefinitions17028::PassPreselection " << GetChannelString(channel) <<  endl;


  w *= HNLZvtxSF(channel);

  if(run_Debug) cout << "HNL_RegionDefinitions17028::PassTriggerSelection " << GetChannelString(channel) <<  endl;


  int nel_hem(0);
  if (channel==EE){
    if (DataEra=="2018"){
      for(auto iel : leps){
	if (iel->Eta() < -1.25){
          if((iel->Phi() < -0.82) && (iel->Phi() > -1.62)) nel_hem++;
	}
      }
    }
  }

  if(nel_hem > 0) return false;


  // Make sure events contain 2 leps
  if (leps.size() != 2 ) return false;
  if (leps_veto.size() != 2) return false;


  FillCutflow(HNL_LeptonCore::ChannelDepDilep, w, GetChannelString(channel) +"_Dilepton",param);

  if (!PassTriggerSelection(channel, ev, leps,"Dilep")) return false;

  FillCutflow(HNL_LeptonCore::ChannelDepTrigger, w, GetChannelString(channel) +"_Trigger",param);


  // Make sure correct leptons are used
  if (channel==EE     && !(leps[0]->LeptonFlavour() == Lepton::ELECTRON && leps[1]->LeptonFlavour() == Lepton::ELECTRON)) return false;
  if (channel==MuMu   && !(leps[0]->LeptonFlavour() == Lepton::MUON     && leps[1]->LeptonFlavour() == Lepton::MUON))    return false;
  if (channel==EMu  &&
      !( (leps[0]->LeptonFlavour() == Lepton::ELECTRON && leps[1]->LeptonFlavour() == Lepton::MUON) ||
         (leps[0]->LeptonFlavour() == Lepton::MUON && leps[1]->LeptonFlavour() == Lepton::ELECTRON) ))  return false;  

  

  // IF RUN CF THEN  MAKE SURE WE RUN ON OS TO WEIGHT


  // CUT ON MASS OF LL PAIR

  Particle ll =  (*leps[0]) + (*leps[1]);
  
  // VETO Z PEAK IN EE CHANNEL
  if (channel==EE  && (fabs(ll.M()-M_Z) < 15)) return false;

  if(ll.M() < 10) return false; // TO_CHECK: IS 20 BEST OPTION


  // REMOVE 0 Jet EVENTS
  int njets     = JetColl.size() + AK8_JetColl.size();
  
  if(njets == 0 )  return false;
  FillCutflow(HNL_LeptonCore::ChannelDepPresel, w, GetChannelString(channel) +"_Presel",param);

  
  return true;
}




bool  HNL_RegionDefinitions17028::RunSignalRegionAK8(HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq , std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto , std::vector<Jet> JetColl, std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param, TString PostLabel ,  float w){


  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;

  if (leps_veto.size() != 2) return false;
  if (AK8_JetColl.size() == 0) return false;

  
  if(qq==Plus && leps[0]->Charge() < 0) return false;
  if(qq==Minus && leps[0]->Charge() > 0) return false;


  double ST = GetST(leps, JetColl, AK8_JetColl, METv);
  double met2_st = pow(METv.Pt(),2.)/ ST;
  bool PassHMMet    = (met2_st < 15);

  bool PassBJetMVeto = (B_JetColl.size()==0);
 
  double UpperMassSR1WmassCut = 150.;

  TString signal_region1 = "HNL_SR2_17028";

  //  Run High Mass with main jet collections                                                                                                                                                                                        
  double FatJetTau21_SF = GetEventFatJetSF(AK8_JetColl,"ak8_type1", 0);
  FillWeightHist(param.Name+"/fatjet_ak8_type1_sf_"+param.Name,FatJetTau21_SF);
  if(!IsData)w*=  FatJetTau21_SF;
  
  if (GetRecoObjMass(signal_region1,JetColl, AK8_JetColl,  leps) < UpperMassSR1WmassCut) {
    
    
    
    if(PassHMMet&&PassBJetMVeto) {
      //FillCutflow(HNL_LeptonCore::ChannelDepSR2, w, GetChannelString(channel) +"_SR2",param);
      Fill_All_SignalRegion2(channel, signal_region1, IsData, "SS",  param.Name,  JetColl, AK8_JetColl, leps, METv , nPV , w   );

      return true;
    }
  }
  return false;
}



bool  HNL_RegionDefinitions17028::RunSignalRegionAK4(HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto ,  std::vector<Jet> JetColl,std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param, TString PostLabel ,  float w){
	

					
  if(!CheckLeptonFlavourForChannel(channel, leps)) return false;

  if (leps_veto.size() != 2) return false;


  if(qq==Plus && leps[0]->Charge() < 0) return false;
  if(qq==Minus && leps[0]->Charge() > 0) return false;



  Particle ll =  (*leps[0]) + (*leps[1]);
  if (channel==EE  && (fabs(ll.M()-M_Z) < 15)) return false;

  double ST = GetST(leps, JetColl, AK8_JetColl, METv);
  double met2_st = pow(METv.Pt(),2.)/ ST;
  bool PassHMMet    = (met2_st < 15);

  double LowerMassSR3WmassCut = 30.;
 
  double UpperMassSR3WmassCut = 150.;
  
  int  NB_JetColl      =  B_JetColl.size();


  if(JetColl.size() < 2) return false;

  if(!(GetRecoObjMass("HNL_SR3", JetColl, AK8_JetColl,leps) < UpperMassSR3WmassCut && GetRecoObjMass("HNL_SR3", JetColl, AK8_JetColl,leps) > LowerMassSR3WmassCut)) {
    return false;
  }
							   

  if(JetColl[0].Pt() < 25.)  return false;

  if(!PassHMMet) {
    return false;
  }


  if(NB_JetColl>  0){
    return false;
  }


  
  FillCutflow(HNL_LeptonCore::ChannelDepSR1, w, GetChannelString(channel) +"_SR1",param);

  TString signal_region = "HNL_SR1_17028";

  Fill_All_SignalRegion1(channel, signal_region, IsData, "SS",  param.Name,  JetColl, AK8_JetColl, leps, METv , nPV , w   );

  return true;
  
}



HNL_RegionDefinitions17028::HNL_RegionDefinitions17028(){
      


}
 
HNL_RegionDefinitions17028::~HNL_RegionDefinitions17028(){

}



void HNL_RegionDefinitions17028::Fill_All_SignalRegion1(HNL_LeptonCore::Channel channel, TString signal_region1, bool isdata, TString charge_s, TString label, std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Lepton *> leps,   Particle _MET,int _npv  , double w   ){


  if (channel == EE){

    // EE SR1/3                                                                                                                                                                                                                         

    Fill_SigRegionPlots1(channel,signal_region1 , "_mn100", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 3.1, 25., 15., 50.,120., 120., 50., 110., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn125", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 3.1, 30., 25., 50.,120., 120., 90., 140., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn200", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 3.1, 55., 40., 50.,120., 220.,160., 225.,  6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn250", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999, 70., 60., 50.,120., 310.,220., 270.,  6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn300", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 80., 60., 50.,120., 370.,235., 335.,  6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn400", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 100., 65., 50.,120., 450.,335., 450.,  6.);

    Fill_SigRegionPlots1(channel, signal_region1 , "_mn500", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 65., 50.,120., 560.,400., 555., 6.);
    Fill_SigRegionPlots1(channel, signal_region1 , "_mn600", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 690., 6.);
    Fill_SigRegionPlots1(channel, signal_region1 , "_mn700", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 966., 6.);
    Fill_SigRegionPlots1(channel, signal_region1 , "_mn800", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 1130., 6.);
    Fill_SigRegionPlots1(channel, signal_region1 , "_mn900", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 1300., 6.);
    Fill_SigRegionPlots1(channel, signal_region1 , "_mn1000", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 1490., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn1100", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 1490., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn1200", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 1600., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn1300", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 1930., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn1400", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 1930., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn1500", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 1930., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn1700", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 2130., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn2000", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 2530., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn2500", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 9999., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn5000", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 9999., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "_mn20000", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 125., 0., 50.,120., 760.,400., 999999., 6.);


  }
  if (channel == MuMu){

    Fill_SigRegionPlots1(channel,signal_region1 , "mn100", label, jets,  fatjets ,  leps,  _MET, _npv, w, 4, 25., 3.1, 25., 15., 50.,120., 110., 55., 115., 9.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn125", label, jets,  fatjets ,  leps,  _MET, _npv, w, 4, 25., 3.1, 25., 25., 50.,120., 140., 85., 140., 7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn200", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 3.1, 50., 40., 50.,120., 250.,160., 215.,  7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn250", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 3.1, 85., 45., 50.,120., 310.,215., 270.,  7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn300", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 100., 50., 50.,120., 370.,225., 340.,  7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn400", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 110., 60., 50.,120., 490.,296., 490.,  7.);
    Fill_SigRegionPlots1(channel, signal_region1 , "mn500", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 110., 60., 50.,120., 610.,370., 550., 7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn600", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 110., 0., 50.,120., 680.,370., 630.,  7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn700", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 110., 0., 50.,120., 800.,370., 885.,  7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn800", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 110., 0., 50.,120., 800.,370., 890.,  7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn900", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 110., 0., 50.,120., 800.,370., 1225.,  7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn1000", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 110., 0., 50.,120., 800.,370., 1230.,  7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn1100", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 110., 0., 50.,120., 800.,370., 1245., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn1200", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 110., 0., 50.,120., 800.,370., 1690.,  7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn1300", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 110., 0., 50.,120., 800.,370., 1890.,  7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn1400", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 999., 110., 0., 50.,120., 800.,370., 1940.,  7.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn1500", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 110., 0., 50.,120., 800.,370., 2220., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn1700", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 110., 0., 50.,120., 800.,370., 2520., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn2000", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 110., 0., 50.,120., 800.,370., 2720., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn2500", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 110., 0., 50.,120., 800.,370., 3220., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn5000", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 110., 0., 50.,120., 800.,370., 99999., 6.);
    Fill_SigRegionPlots1(channel,signal_region1 , "mn20000", label, jets,  fatjets,  leps,  _MET, _npv, w, 4, 25., 9999., 110., 0., 50.,120., 800.,370., 9999999., 6.);

  } // MM                                                                                                                                                                                                                               


  return;
}




void HNL_RegionDefinitions17028::Fill_All_SignalRegion2(HNL_LeptonCore::Channel channel, TString signal_region2, bool isdata, TString charge_s, TString label, std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Lepton *> leps, Particle _MET,int _npv  , double w){

  // get loop of systs + one nominal                                                                                                                                                                                                    
  
  
  if(channel == EE){
    // EE SR2/4                                                                                                                                                                                                                         
    Fill_SigRegionPlots2(channel,signal_region2 , "mn100", label, jets,  fatjets,  leps,  _MET, _npv, w, 25.,15., 40.,130., 90., 220., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn125", label, jets,  fatjets,  leps,  _MET, _npv, w, 60.,15., 40.,130., 123., 145., 15.);////////////////                                                                           
    Fill_SigRegionPlots2(channel,signal_region2 , "mn200", label, jets,  fatjets,  leps,  _MET, _npv, w, 100., 20., 40., 130., 173., 220., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn250", label, jets,  fatjets,  leps,  _MET, _npv, w, 100., 25., 40., 130., 220., 305., 15.);//////////////                                                                          
    Fill_SigRegionPlots2(channel,signal_region2 , "mn300", label, jets,  fatjets,  leps,  _MET, _npv, w, 100., 30., 40., 130., 270, 330., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn400", label, jets,  fatjets,  leps,  _MET, _npv, w, 100., 35., 40., 130., 330., 440., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn500", label, jets,  fatjets,  leps,  _MET, _npv, w, 120., 35., 40., 130., 440., 565., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn600", label, jets,  fatjets,  leps,  _MET, _npv, w, 120., 0., 40., 130., 565., 675., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn700", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0., 40., 130., 635., 775., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn800", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0., 40., 130., 740., 1005., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn900", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0., 40., 130., 865., 1030., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1000", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0., 40., 130., 890., 1185., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1100", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1035., 1395., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1200", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1085., 1460., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1300", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1140., 1590., 15.);///////////                                                                            
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1400", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1245., 1700., 15.);///////                                                                                
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1500", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1300., 1800., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1700", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1300., 2000., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn2000", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1300., 2800., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn2500", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1300., 3800., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn5000", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1300., 5800., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn20000", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1300., 99999., 15.);

  }
  if(channel == MuMu){

    Fill_SigRegionPlots2(channel,signal_region2 , "mn100", label, jets,  fatjets,  leps,  _MET, _npv, w, 25.,  15., 40.,130., 98., 145., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn125", label, jets,  fatjets,  leps,  _MET, _npv, w, 60.,  15., 40.,130., 110., 150., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn200", label, jets,  fatjets,  leps,  _MET, _npv, w, 100., 20., 40., 130., 175., 235., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn250", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 25., 40., 130., 226., 280., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn300", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 20., 40., 130., 280., 340., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn400", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 65., 40., 130., 340., 445., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn500", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 65., 40., 130., 445., 560., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn600", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0., 40., 130., 560., 685., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn700", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0., 40., 130., 635., 825., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn800", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0., 40., 130., 755., 960., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn900", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0., 40., 130., 840., 1055., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1000", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0., 40., 130., 900., 1205., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1100", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 990., 1250., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1200", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1035., 1430., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1300", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1110., 1595., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1400", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1285., 1700., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1500", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1330., 1800., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn1700", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1330., 2200., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn2000", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1330., 99999., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn2500", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1330., 99999., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn5000", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1330., 9999., 15.);
    Fill_SigRegionPlots2(channel,signal_region2 , "mn20000", label, jets,  fatjets,  leps,  _MET, _npv, w, 140., 0.,40., 130., 1330., 999999., 15.);


  }
  if(channel == EMu){

  }

  return;
}




void HNL_RegionDefinitions17028::Fill_SigRegionPlots2(HNL_LeptonCore::Channel channel,TString label_sr, TString label_mass, TString label_anid,  std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Lepton *> leps, Particle  met, double nvtx,  double w,  double var1,  double var2, double var3, double var4, double var5, double var6, double var7){

  if(!CheckLeptonFlavourForChannel(channel, leps)) return;

  if(fatjets.size() == 0) return;


  float dijetmass_tmp=999.;
  float dijetmass=9990000.;
  int m=-999;
  double ST(0.);
  for(UInt_t emme=0; emme<jets.size(); emme++){
    ST += jets[emme].Pt();
  }
  for(UInt_t emme=0; emme<fatjets.size(); emme++){
    ST += fatjets[emme].Pt();
    dijetmass_tmp = fatjets[emme].SDMass();
    if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
      dijetmass = dijetmass_tmp;
      m = emme;
    }
  }
  ST += leps[0]->Pt() + leps[1]->Pt() + met.Pt();

  double met2 = met.Pt()*met.Pt();
  Particle Wcand  = fatjets[m];
  Particle W1cand = fatjets[m] + *leps[0] + *leps[1];
  Particle N1cand = fatjets[m] + *leps[0];
  Particle N2cand = fatjets[m] + *leps[1];

  //  HNL_LeptonCore::SearchRegion  cutflow_SR_index = SR2;
  TString label_mass_tmp=label_mass;
  //if  (label_anid.Contains("OS")) label_mass_tmp+="OS";                                                                                                                                                                             

  if(leps[0]->Pt() < var1) return;
  if(leps[1]->Pt() < var2) return;
  if(fatjets[m].SDMass() < var3) return;
  if(fatjets[m].SDMass() > var4) return;
  if(N1cand.M()  < var5 && N2cand.M()  < var5) return;

  if(N1cand.M()  > var6 && N2cand.M()  > var6) return;

  if(met2/ST > var7) return;

  FillHist( label_anid+"/"+label_sr+ "/"+ label_mass +  "_nevent"  , 1.,  w, 2, 0.,2. , "N cut");
  //  FillCutflowSR(label_anid,cutflow_SR_index,w, label_sr+"_"+label_mass);

  return;

}


void   HNL_RegionDefinitions17028::Fill_SigRegionPlots1(HNL_LeptonCore::Channel channel, TString label_sr, TString label_mass, TString label_anid,  std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Lepton *> leps, Particle  met , double nvtx,  double w,  double var1,  double var2, double var3, double var4, double var5, double var6, double var7, double var8, double var9, double var10, double var11){


  if(!CheckLeptonFlavourForChannel(channel, leps)) return;

  if(jets.size() < 2) return;

  
  float dijetmass_tmp=999.;
  float dijetmass=9990000.;
  int m=-999;
  int n=-999;
  double ST(0.);
  for(UInt_t emme=0; emme<jets.size(); emme++){
    ST += jets[emme].Pt();
    for(UInt_t enne=1; enne<jets.size(); enne++) {

      dijetmass_tmp = (jets[emme]+jets[enne]).M();
      if(emme == enne) continue;

      if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
        dijetmass = dijetmass_tmp;
        m = emme;
        n = enne;
      }
    }
  }


  ST += leps[0]->Pt() + leps[1]->Pt() + met.Pt();
  double met2 = pow(met.Pt(),2.);
  Particle Wcand = jets[m] + jets[n];
  Particle W1cand = jets[m] + jets[n]+ *leps[0] + *leps[1];
  Particle N1cand = jets[m] + jets[n]+ *leps[0];
  Particle N2cand = jets[m] + jets[n]+ *leps[1];



  //  HNL_LeptonCore::SearchRegion cutflow_SR_index = SR1;
  if(jets.size() >= var1) return;
  if(jets[0].Pt() < var2)return;
  if(Wcand.DeltaR(*leps[1]) > var3) return;
  if(leps[0]->Pt() < var4) return;
  if(leps[1]->Pt() < var5) return;
  if(Wcand.M() < var6) return;
  if(Wcand.M() > var7) return;
  if(W1cand.M() < var8) return;
  if(N1cand.M() < var9 && N2cand.M()  < var9) return;
  if(N1cand.M()  > var10 && N2cand.M()  > var10) return;
  if(met2/ST > var11) return;
  
  //  FillCutflowSR(label_anid,cutflow_SR_index,w, label_sr+"_"+label_mass);
  
  FillHist( label_anid+"/"+label_sr+ "/"+label_mass +  "_nevent" ,  1.,  w, 2, 0.,2. );
  return;
  
}
