#include "HNL_SignalRegionPlotter21003.h"

void HNL_SignalRegionPlotter21003::initializeAnalyzer(){

  // All default settings like trigger/ PD/ BJet are decalred in HNL_LeptonCore::initializeAnalyzer to make them consistent for all HNL codes

  HNL_LeptonCore::initializeAnalyzer();

}


void HNL_SignalRegionPlotter21003::executeEvent(){
  
  if((_jentry==0)){ 
    // Print out trigger info in HNL_LeptonCore::initializeAnalyzer
    TriggerPrintOut(GetEvent());
  }
  
  AnalyzerParameter param_signal = HNL_LeptonCore::InitialiseHNLParameter("Peking");
  RunULAnalysis(param_signal);


  return ;
}

void HNL_SignalRegionPlotter21003::RunULAnalysis(AnalyzerParameter param){

  if(run_Debug) cout << "HNL_SignalRegionPlotter21003::executeEvent " << endl;

  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);

  
  // HL ID
  std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, 10., 2.5); 
  std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID, 5., 2.4);

  TString el_ID = (RunFake) ?  param.Electron_FR_ID : param.Electron_Tight_ID ;
  TString mu_ID = (RunFake) ?  param.Muon_FR_ID :  param.Muon_Tight_ID ;

  double Min_Muon_Pt     = (RunFake) ? 30. : 30.;
  double Min_Electron_Pt = (RunFake) ? 30  : 30.;

  std::vector<Muon>       MuonCollTInit     = SelectMuons    ( param,mu_ID, Min_Muon_Pt,     2.4);
  std::vector<Electron>   ElectronCollTInit = SelectElectrons( param,el_ID, Min_Electron_Pt, 2.5)  ;

  std::vector<Muon>       MuonCollT      =  GetLepCollByRunType   ( MuonCollTInit,param);
  std::vector<Electron>   ElectronCollT  =  GetLepCollByRunType   ( ElectronCollTInit,param);

  std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(MuonCollV,ElectronCollV);

  std::vector<Tau>        TauColl        = SelectTaus     (leps_veto,param.Tau_Veto_ID,20., 2.3);

  // Creat Lepton vector to have lepton blind codes 

  Particle METv = GetMiniAODvMET("T1xyCorr"); // returns MET with systematic correction

  
  std::vector<FatJet> AK8_JetColl;
  
  // AK4 JET                                                                                                                                                                              
  std::vector<Jet> jets_tmp     = SelectJets   ( param, param.Jet_ID, 15., 5.);

  
  std::vector<Jet> JetCollLoose                      = SelectAK4Jets(jets_tmp,     15., 4.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, AK8_JetColl);
  
  TString PUIDWP="loose";
  std::vector<Jet> VBF_JetColl                       = SelectAK4Jets(jets_tmp,     30., 4.7, true,  0.4,0.8, PUIDWP,  ElectronCollV,MuonCollV, AK8_JetColl);   

  std::vector<Jet> BJetColltmp                       = SelectAK4Jets(jets_tmp,     20., 2.4, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, AK8_JetColl);

  double PJet_PUID_weight = GetJetPileupIDSF(VBF_JetColl, PUIDWP, param);
  weight*= PJet_PUID_weight;
  FillWeightHist("PJet_PUID_weight_" ,PJet_PUID_weight);

  
  // select B jets                                                                                                                                                                        
  JetTagging::Parameters param_jets = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets);
  
  // Get BJets  and EV weight to corr BTag Eff                                                                                                                       
  std::vector<Jet> BJetColl    = SelectBJets(param, BJetColltmp, param_jets);
  double sf_btag               = GetBJetSF(param, BJetColltmp, param_jets);
  if(!IsData )weight*= sf_btag;

  vector<HNL_LeptonCore::Channel> channels = {EE,MuMu, EMu};

  for(auto dilep_channel : channels){
    
    if(run_Debug) cout << "HNL_RegionDefinitions::RunAllSignalRegions " << GetChannelString(dilep_channel) <<  endl;
    
    double  weight_channel = weight;
    if(MCSample.Contains("Type")) {
      if (!SelectChannel(dilep_channel)) continue;
    }

    std::vector<Lepton *> leps       = MakeLeptonPointerVector(MuonCollT,ElectronCollT,param);
    if(!PassGenMatchFilter(leps,param)) continue;

    AnalyzerParameter param_channel = param;

    if(!IsDATA && dilep_channel != MuMu)  EvalElectronIDWeight(ElectronCollT, param_channel,weight_channel);
    if(!IsDATA && dilep_channel != EE)    EvalMuonIDWeight(MuonCollT, param_channel,weight_channel);

    FillCutflow(HNL_LeptonCore::ChannelDepInc, weight_channel, GetChannelString(dilep_channel) +"_NoCut", param);

    if(!PassMETFilter()) continue;
    
    TString channel_string = GetChannelString(dilep_channel);
    param_channel.Name =  channel_string + "/" + param_channel.DefName;

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

    
    if(!CheckLeptonFlavourForChannel(dilep_channel, leps)) continue;
    if (leps_veto.size() != 2)  continue;

    if (!PassTriggerSelection(dilep_channel, ev, leps,"Dilep")) continue;
								  
    weight_channel *= HNLZvtxSF(dilep_channel);
    
    if(!PassHEMVeto( leps,weight_channel)) continue;
    
    Particle ll =  (*leps[0]) + (*leps[1]);
    if(ll.M() < 20)  continue;
    FillCutflow(HNL_LeptonCore::SR2,weight_channel, "Presel",param);
    
    
    RunSignalRegionWW (dilep_channel,Inclusive, leps, leps_veto, TauColl, JetCollLoose, VBF_JetColl, BJetColl, ev, METv ,param_channel,weight_channel);
    
    
  }// channel loop 
 

}



 


HNL_SignalRegionPlotter21003::HNL_SignalRegionPlotter21003(){


}
 
HNL_SignalRegionPlotter21003::~HNL_SignalRegionPlotter21003(){

}




