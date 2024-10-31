#include "HNL_SignalRegionOpt.h"

void HNL_SignalRegionOpt::initializeAnalyzer(){

  // All default settings like trigger/ PD/ BJet are decalred in HNL_LeptonCore::initializeAnalyzer to make them consistent for all HNL codes

  HNL_LeptonCore::initializeAnalyzer();

  SetupIDMVAReaderDefault(); /// Not needed for BDT skim                                                                                                                                                           
  SetupEventMVAReader("V2"); 
}


void HNL_SignalRegionOpt::executeEvent(){
  
  if((_jentry==0)){ 
    // Print out trigger info in HNL_LeptonCore::initializeAnalyzer
    TriggerPrintOut(GetEvent());
  }
  
  AnalyzerParameter param_signal = HNL_LeptonCore::InitialiseHNLParameter("HNLSROpt");
  
  ///HNLSROpt sets Fake to use data and CF to use MC ..... 
  /// HNLOpt sets all bkg to use MC
    //cout << "########################################################" << endl;

  
  /// Name for param used in hist naming
  param_signal.Name = param_signal.DefName  + "HNTightV2";

  /// SRCOnfig used to set Region Code what to vary in SR
  param_signal.SRConfig  = "";
  
  /// ID used for leptons
  param_signal.Muon_Tight_ID="HNTightV2";
  param_signal.Muon_FR_ID = "HNLooseV1";
  param_signal.Electron_Tight_ID = "HNTightV2";
  param_signal.Electron_FR_ID = "HNLooseV4";
  
  RunULAnalysis(param_signal);
  
  
  //// SYST IF USED CURRENTLY DOES NOT RUN

  return ;
}

void HNL_SignalRegionOpt::RunULAnalysis(AnalyzerParameter param){

  if(run_Debug) cout << "HNL_SignalRegionOpt::executeEvent " << endl;

  Event ev = GetEvent();
  double weight =SetupWeight(ev,param);

  if (IsSignal()){
    /// For optimisation best to remove xsec so that weights are based on N events
    weight*=1/xsec;
  }

  
  //// VETO LEPTON IDS set up in param
  std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, 10., 2.5); 
  std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID, 5., 2.4);

  //// ID set based on if running Fake or normal setup
  TString el_ID = (RunFake) ?  param.Electron_FR_ID : param.Electron_Tight_ID ;
  TString mu_ID = (RunFake) ?  param.Muon_FR_ID :  param.Muon_Tight_ID ;
  
  
  ///  Set pt lower if using fakes due to ptcone 
  double Min_Muon_Pt     = (RunFake) ? 7. : 10.;
  double Min_Electron_Pt = (RunFake) ? 7. : 10.;
  
  
  //// Set initial lepton vectors : Passes RunFake to set pt == ptcone
  std::vector<Muon>       MuonCollTInit     = SelectMuons    ( param,mu_ID, Min_Muon_Pt, 2.4);
  std::vector<Electron>   ElectronCollTInit = SelectElectrons( param,el_ID, Min_Electron_Pt, 2.5)  ;
  
  //// Select leptons that pass gen cuts based on fakes/cf....
  std::vector<Muon>       MuonCollT     = GetLepCollByRunType    ( MuonCollTInit,param,weight);
  std::vector<Electron>   ElectronCollT  =  GetLepCollByRunType   ( ElectronCollTInit,param,weight);
  
  //cout << "---------------------- " << endl;
  //cout << "ElectronCollT " << ElectronCollT.size() << " ID = " << el_ID << endl;
  //cout << "MuonCollT " << MuonCollT.size() << " ID = " << mu_ID << endl;
  
  
  /// Create a single vector of Lepton Veto objects
  std::vector<Lepton *> leps_veto  = MakeLeptonPointerVector(MuonCollV,ElectronCollV);

  /// Get Taus 
  std::vector<Tau>        TauColl        = SelectTaus    (leps_veto,param.Tau_Veto_ID,25., 2.3);


  
  /// Get MET for PUPPPI TYPE 1
  Particle METv = GetMiniAODvMET("PuppiT1xyCorr"); // returns MET with systematic correction

  
  
  // JET COLLECTION

  if(HasFlag("CheckJetOpt")){
    std::vector<FatJet> fatjets_tmp  = SelectFatJets(param, param.FatJet_ID, 200., 5.);
    
    // 
    std::vector<FatJet> FatjetColl_1                  = SelectAK8Jets(fatjets_tmp, 200., 2.7, true,  1., true, -999, true, 40., 130.,"", ElectronCollV, MuonCollV);

    vector<double> etabins = {};//{2.7,4.7};
    vector<bool> LepVeto = {true, false};
    vector<double> Tau21Cut = {-1., 0, 1};
    vector<bool> SDMassCut = {true, false};
    vector<double> WQCDTagger = {-999, 0.6,0.7,0.8}; 
    vector<TString> s_etabins = {"etabin1_","etabin2_"};
    vector<TString> s_LepVeto = {"LV_","NoLV_"};
    vector<TString> s_Tau21Cut = {"tau21_HP_","","tau21_LP_"};
    vector<TString> s_SDMassCut = {"SDMassCut_",""};
    vector<TString> s_WQCDTagger = {"","PNTagger06", "PNTagger07","PNTagger08"};
    map<TString,   std::vector<FatJet> > fatjet_map;

    for(unsigned int ietabin=0 ; ietabin < etabins.size(); ietabin++){
      for(unsigned int iLepVeto=0 ; iLepVeto < LepVeto.size(); iLepVeto++){
	for(unsigned int iTau21Cut=0 ; iTau21Cut < Tau21Cut.size(); iTau21Cut++){
	  for(unsigned int iSDMassCut=0 ; iSDMassCut < SDMassCut.size(); iSDMassCut++){
	    for(unsigned int iWQCDTagger=0 ; iWQCDTagger < WQCDTagger.size(); iWQCDTagger++){
	    
	      if(iTau21Cut!= 1 && iWQCDTagger > 0) continue;
	      
	      TString idtag = "ak8_"+ s_etabins[ietabin] +  s_LepVeto[iLepVeto] + s_Tau21Cut[iTau21Cut] + s_SDMassCut[iSDMassCut] + s_WQCDTagger[iWQCDTagger] ;
	      
	      //bool apply_tau_21 = (Tau21Cut[iTau21Cut] == 0.) ?  false : true;
	      //fatjet_map[idtag]   = SelectAK8Jetsv2(fatjets_tmp, 200., etabins[ietabin], s_LepVeto[iLepVeto],  1., apply_tau_21, Tau21Cut[iTau21Cut], SDMassCut[iSDMassCut], 40., 130., WQCDTagger[iWQCDTagger], ElectronCollV, MuonCollV);
	    }
	  }
	}
      }
    }

    //HNL_ULak8_etabin2_NoLV_SDMassCut_PNTagger06_ak4_type1PuL_ak4_vbf_type1_ak4_b_type2_M                                                          
    //HNL_ULak8_etabin2_NoLV_tau21_LP_SDMassCut__ak4_type1PuL_ak4_vbf_type1_ak4_b_type2_M
    // AK4 JET
    std::vector<Jet> jets_tmp     = SelectJets   ( param, param.Jet_ID, 15., 5.);
    
    std::vector<Jet> AllJetColl                      = SelectJets   (param, "NoID",10, 3.);

    std::vector<Jet> JetCollLoose                    = SelectAK4Jets(jets_tmp,     15., 4.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, FatjetColl_1);

    std::vector<Jet> BJetColltmp                    = SelectAK4Jets(jets_tmp,     20., 2.4, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, FatjetColl_1);
    std::vector<Jet> BJetCollNLV                    = SelectAK4Jets(jets_tmp,     20., 2.4, false,  0.4,0.8, "",   ElectronCollV,MuonCollV, FatjetColl_1);

    map<TString,   std::vector<Jet> > jet_map;

    
    std::vector<Jet> JetColl                        = SelectAK4Jets(jets_tmp,     20., 2.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, FatjetColl_1);
    
    jet_map["ak4_type1"]   = JetColl;
    jet_map["ak4_type1PuL"]                          = SelectAK4Jets(jets_tmp,     20., 2.7, true,  0.4,0.8, "loose",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type1PuM"]                          = SelectAK4Jets(jets_tmp,     20., 2.7, true,  0.4,0.8, "medium",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type1PuT"]                          = SelectAK4Jets(jets_tmp,     20., 2.7, true,  0.4,0.8, "tight",   ElectronCollV,MuonCollV, FatjetColl_1);
    
    jet_map["ak4_type2"]                          = SelectAK4Jets(jets_tmp,     15., 2.5, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type2PuL"]                          = SelectAK4Jets(jets_tmp,     15., 2.5, true,  0.4,0.8, "loose",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type2PuM"]                          = SelectAK4Jets(jets_tmp,     15., 2.5, true,  0.4,0.8, "medium",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type2PuT"]                          = SelectAK4Jets(jets_tmp,     15., 2.5, true,  0.4,0.8, "tight",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type5"]                          = SelectAK4Jets(jets_tmp,     15., 2.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type5PuL"]                          = SelectAK4Jets(jets_tmp,     15., 2.7, true,  0.4,0.8, "loose",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type5PuM"]                          = SelectAK4Jets(jets_tmp,     15., 2.7, true,  0.4,0.8, "medium",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type5PuT"]                          = SelectAK4Jets(jets_tmp,     15., 2.7, true,  0.4,0.8, "tight",   ElectronCollV,MuonCollV, FatjetColl_1);


    jet_map["ak4_type3"]                          = SelectAK4Jets(jets_tmp,     20., 4.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type3PuL"]                          = SelectAK4Jets(jets_tmp,     20., 4.7, true,  0.4,0.8, "loose",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type3PuM"]                          = SelectAK4Jets(jets_tmp,     20., 4.7, true,  0.4,0.8, "medium",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type3PuT"]                          = SelectAK4Jets(jets_tmp,     20., 4.7, true,  0.4,0.8, "tight",   ElectronCollV,MuonCollV, FatjetColl_1);

    jet_map["ak4_type4"]                          = SelectAK4Jets(jets_tmp,     30., 2.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type4PuL"]                          = SelectAK4Jets(jets_tmp,     30., 2.7, true,  0.4,0.8, "loose",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type4PuM"]                          = SelectAK4Jets(jets_tmp,     30., 2.7, true,  0.4,0.8, "medium",   ElectronCollV,MuonCollV, FatjetColl_1);
    jet_map["ak4_type4PuT"]                          = SelectAK4Jets(jets_tmp,     30., 2.7, true,  0.4,0.8, "tight",   ElectronCollV,MuonCollV, FatjetColl_1);


    map<TString,   std::vector<Jet> > vbfjet_map;
    
    std::vector<Jet> VBF_JetColl1                    = SelectAK4Jets(jets_tmp,     30., 4.7, true,  0.4,0.8, "",  ElectronCollV,MuonCollV, FatjetColl_1);    // High ETa jets 

    vbfjet_map["ak4_vbf_type1"]   =  VBF_JetColl1;
    vbfjet_map["ak4_vbf_type1PuL"]                    = SelectAK4Jets(jets_tmp,     30., 4.7, true,  0.4,0.8, "loose",  ElectronCollV,MuonCollV, FatjetColl_1);    // High ETa jets 
    vbfjet_map["ak4_vbf_type1PuM"]                    = SelectAK4Jets(jets_tmp,     30., 4.7, true,  0.4,0.8, "medium",  ElectronCollV,MuonCollV, FatjetColl_1);    // High ETa jets 
    vbfjet_map["ak4_vbf_type1PuT"]                    = SelectAK4Jets(jets_tmp,     30., 4.7, true,  0.4,0.8, "tight",  ElectronCollV,MuonCollV, FatjetColl_1);    // High ETa jets 
    vbfjet_map["ak4_vbf_type2PuL"]                    = SelectAK4Jets(jets_tmp,     25., 4.7, true,  0.4,0.8, "loose",  ElectronCollV,MuonCollV, FatjetColl_1);    // High ETa jets                   
    vbfjet_map["ak4_vbf_type3PuL"]                    = SelectAK4Jets(jets_tmp,     20., 4.7, true,  0.4,0.8, "loose",  ElectronCollV,MuonCollV, FatjetColl_1);    // High ETa jets              
    //vbfjet_map["ak4_vbf_type2PuM"]                    = SelectAK4Jets(jets_tmp,     25., 4.7, true,  0.4,0.8, "medium",  ElectronCollV,MuonCollV, FatjetColl_1);    // High ETa jets              
    ////vbfjet_map["ak4_vbf_type2PuT"]                    = SelectAK4Jets(jets_tmp,     25., 4.7, true,  0.4,0.8, "tight",  ElectronCollV,MuonCollV, FatjetColl_1);    // High ETa jets              

    //    double PJet_PUID_weightL = GetJetPileupIDSF(JetColl_PuL,"loose" , param);

    
    // select B jets
    JetTagging::Parameters param_jetsL = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Loose, JetTagging::incl, JetTagging::mujets);
    JetTagging::Parameters param_jetsM = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets);
    JetTagging::Parameters param_jetsT = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Tight, JetTagging::incl, JetTagging::mujets);
    
    // Get BJets  and EV weight to corr BTag Eff                                                                                                                       
    std::vector<Jet> BJetCollL    = SelectBJets(param,  BJetColltmp, param_jetsL);
    std::vector<Jet> BJetCollM    = SelectBJets(param,  BJetColltmp, param_jetsM);
    std::vector<Jet> BJetCollT    = SelectBJets(param,  BJetColltmp, param_jetsT);

    double sf_btagL               = GetBJetSF(param, BJetColltmp, param_jetsL);
    double sf_btagM               = GetBJetSF(param, BJetColltmp, param_jetsM);
    double sf_btagT               = GetBJetSF(param, BJetColltmp, param_jetsT);

    std::vector<Jet> BJetCollNLVL    = SelectBJets(param,  BJetCollNLV, param_jetsL);
    std::vector<Jet> BJetCollNLVM    = SelectBJets(param,  BJetCollNLV, param_jetsM);
    std::vector<Jet> BJetCollNLVT    = SelectBJets(param,  BJetCollNLV, param_jetsT);

    double sf_btagL_NLV               = GetBJetSF(param, BJetCollNLV, param_jetsL);
    double sf_btagM_NLV               = GetBJetSF(param, BJetCollNLV, param_jetsM);
    double sf_btagT_NLV               = GetBJetSF(param, BJetCollNLV, param_jetsT);


    map<TString,   std::vector<Jet> > bjet_map;
    //bjet_map["ak4_b_type1_L"]   = BJetCollL;
    //bjet_map["ak4_b_type1_M"]   = BJetCollM;
    //bjet_map["ak4_b_type1_T"]   = BJetCollT;
    //bjet_map["ak4_b_type2_L"]   = BJetCollNLVL;
    bjet_map["ak4_b_type2_M"]   = BJetCollNLVM;
    //bjet_map["ak4_b_type2_T"]   = BJetCollNLVT;


    for(std::map<TString, std::vector<FatJet> >::iterator ak8_it = fatjet_map.begin(); ak8_it != fatjet_map.end(); ak8_it++){
      for(std::map<TString, std::vector<Jet> >::iterator ak4_it =  jet_map.begin(); ak4_it != jet_map.end(); ak4_it++){
	for(std::map<TString, std::vector<Jet> >::iterator ak4_vbf_it = vbfjet_map.begin(); ak4_vbf_it != vbfjet_map.end(); ak4_vbf_it++){
	  for(std::map<TString, std::vector<Jet> >::iterator ak4_b_it =  bjet_map.begin(); ak4_b_it != bjet_map.end(); ak4_b_it++){
	    
	    param.WriteOutVerbose=1; // Does not Fill Cutflow OR Region Plotter
	    
	    param.Name = param.DefName + ak8_it->first + "_"+ak4_it->first + "_"+ak4_vbf_it->first + "_"+ak4_b_it->first;
	    double weight_optjet = weight;
	    if (ak4_b_it->first == "ak4_b_type1_L") weight_optjet*= sf_btagL;
	    if (ak4_b_it->first == "ak4_b_type1_M") weight_optjet*= sf_btagM;
	    if (ak4_b_it->first == "ak4_b_type1_T") weight_optjet*= sf_btagT;
	    if (ak4_b_it->first == "ak4_b_type2_L") weight_optjet*= sf_btagL_NLV;
            if (ak4_b_it->first == "ak4_b_type2_M") weight_optjet*= sf_btagM_NLV;
            if (ak4_b_it->first == "ak4_b_type2_T") weight_optjet*= sf_btagT_NLV;
	    
	    if (ak4_it->first.Contains("PuL")) weight_optjet*=   GetJetPileupIDSF(ak4_it->second,"loose" , param);
	    if (ak4_it->first.Contains("PuM")) weight_optjet*=   GetJetPileupIDSF(ak4_it->second,"medium" , param);
	    if (ak4_it->first.Contains("PuT")) weight_optjet*=   GetJetPileupIDSF(ak4_it->second,"tight" , param);
	    if (ak4_vbf_it->first.Contains("PuL")) weight_optjet*=   GetJetPileupIDSF(ak4_vbf_it->second,"loose" , param);
            if (ak4_vbf_it->first.Contains("PuM")) weight_optjet*=   GetJetPileupIDSF(ak4_vbf_it->second,"medium" , param);
            if (ak4_vbf_it->first.Contains("PuT")) weight_optjet*=   GetJetPileupIDSF(ak4_vbf_it->second,"tight" , param);

	    
	    if(ak8_it->first.Contains("tau21_HP_")) weight_optjet*=  GetEventFatJetSF(ak8_it->second,"HP", 0);
	    if(ak8_it->first.Contains("tau21_LP_")) weight_optjet*=  GetEventFatJetSF(ak8_it->second,"LP", 0);
	    
	    
	    //cout << param.Name << " " << ak8_it->second.size() << " " << ak4_it->second.size() << " " << weight << endl;
	    RunAllSignalRegionsOpt(Inclusive, 
				ElectronCollT,ElectronCollV,MuonCollT,MuonCollV, TauColl, 
				AllJetColl,JetCollLoose, ak4_it->second, ak4_vbf_it->second, ak8_it->second, ak4_b_it->second, ak4_b_it->second, 
				ev,METv, param,weight_optjet);
	    
	    param.Name = param.DefName;
	  }
	}
      }
    }
    
    return;
  }


  //// NOW make some jet selection to use in other studies

  std::vector<FatJet> AK8Jet_TMP  = SelectFatJets(param, param.FatJet_ID, 200., 5.);
  std::vector<FatJet> AK8Jet                  = SelectAK8Jets(AK8Jet_TMP, 200., 2.7, true,  1., true, -999, true, 40., 130.,"", ElectronCollV, MuonCollV);
  std::vector<Jet> AK4Jet_TMP     = SelectJets   ( param, param.Jet_ID, 20., 5.);

  std::vector<Jet> AllJetColl                      = SelectJets   (param, "NoID",10, 3.);
  std::vector<Jet> JetCollLoose                    = SelectAK4Jets(AK4Jet_TMP,    15., 4.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, AK8Jet);
  std::vector<Jet> BJetCollNLV                    = SelectAK4Jets(AK4Jet_TMP,     20., 2.4, false,  0.4,0.8, "",   ElectronCollV,MuonCollV, AK8Jet);
  std::vector<Jet> JetColl                        = SelectAK4Jets(AK4Jet_TMP,     20., 2.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, AK8Jet);
  std::vector<Jet> VBF_JetColl                    = SelectAK4Jets(AK4Jet_TMP,     30., 4.7, true,  0.4,0.8, "",  ElectronCollV,MuonCollV, AK8Jet);  
  

  /// B JET 
  JetTagging::Parameters param_jetsM = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets);
  //  JetTagging::Parameters param_jetsL = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Loose, JetTagging::incl, JetTagging::mujets);
  JetTagging::Parameters param_jetsT = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Tight, JetTagging::incl, JetTagging::mujets);

  std::vector<Jet> BJetColl    = SelectBJets(param,  BJetCollNLV, param_jetsM);
  double sf_btagM_NLV          = GetBJetSF(param, BJetCollNLV, param_jetsM);

  std::vector<Jet> BJetCollSR1    = SelectBJets(param,  BJetCollNLV, param_jetsT);
  //  double sf_btagSR1_NLV           = GetBJetSF(param, BJetCollNLV, param_jetsT);
  

  /// param.WriteOutVerbose used to set code how many historgams to draw
  /// param.WriteOutVerbose = 0 draws ALL hists
  
  if(param.Muon_Tight_ID == "HNTightV2") param.WriteOutVerbose=0;
  else   param.WriteOutVerbose=1; // Does not Fill Cutflow OR Region Plotter  
  
  

  if(HasFlag("opt_trig")){

    if(!IsData )weight = weight*sf_btagM_NLV;

    param.Name = param.DefName + "Nom";
    param.SRConfig  =  "";
    RunAllSignalRegionsOpt(Inclusive, 
			ElectronCollT,ElectronCollV,MuonCollT,MuonCollV, TauColl, 
			AllJetColl, JetCollLoose, JetColl, VBF_JetColl,AK8Jet , BJetColl,BJetColl, 
			ev,METv, param,weight);

    param.Name = param.DefName + "Trig1OR";
    param.SRConfig = "Trig1OR";
    RunAllSignalRegionsOpt(Inclusive, 
			ElectronCollT,ElectronCollV,MuonCollT,MuonCollV, TauColl, 
			AllJetColl, JetCollLoose, JetColl, VBF_JetColl,AK8Jet , BJetColl,BJetColl, 
			ev,METv, param,weight);
    
    param.Name = param.DefName + "Trig2OR";
    param.SRConfig = "Trig2OR";
    RunAllSignalRegionsOpt(Inclusive, 
			ElectronCollT,ElectronCollV,MuonCollT,MuonCollV, TauColl, 
			AllJetColl, JetCollLoose, JetColl, VBF_JetColl,AK8Jet , BJetColl,BJetColl, 
			ev,METv, param,weight);

    param.Name = param.DefName + "Trig3OR";
    param.SRConfig = "Trig3OR";
    RunAllSignalRegionsOpt(Inclusive, 
			ElectronCollT,ElectronCollV,MuonCollT,MuonCollV, TauColl, 
			AllJetColl, JetCollLoose, JetColl, VBF_JetColl,AK8Jet , BJetColl,BJetColl, 
			ev,METv, param,weight);
    
    param.Name = param.DefName;
    param.SRConfig  =  "";

  }


  if(HasFlag("opt_sr1")){

    vector<TString> SR1Opt = {};
    vector<TString> METcut = {"SR1MET_10_","SR1MET_12_","SR1MET_15_","SR1MET_17_","SR1MET_20_"};
    vector<TString> MJJSel = {"SR1MLLJJ_500_","SR1MLLJJ_600_","SR1MLLJJ_700_","SR1MLLJJ_800_","SR1MLLJJ_900_","SR1MLLJJ_1000_"};
    vector<TString> PtCut = {"SR1PT_80_","SR1PT_90_","SR1PT_100_","SR1PT_110_","SR1PT_120_"};

    for(auto iht : METcut){
      SR1Opt.push_back(iht);
    }
    
    for(auto ij : MJJSel){
      SR1Opt.push_back( ij);
    }
    for(auto im : PtCut){
      SR1Opt.push_back(im);
    }
    
    if(!IsData )weight = weight*sf_btagM_NLV;

    for (auto is1 : SR1Opt){

      param.Name = param.DefName + is1;
      param.SRConfig  = is1;
      RunAllSignalRegionsOpt(Inclusive, 
			  ElectronCollT,ElectronCollV,MuonCollT,MuonCollV, TauColl, 
			  AllJetColl,JetCollLoose, JetColl, VBF_JetColl,AK8Jet , BJetColl,BJetColl, 
			  ev,METv, param,weight);
      // reset param                                                                                                                                                                                                                        
      param.Name = param.DefName;
      param.SRConfig  = "";
    }
  }



  if(HasFlag("opt_sr3")){

    vector<TString> SR3Opt = {};
    vector<TString> PT1cut = {"SR3_bin1pt_60_","SR3_bin1pt_70_","SR3_bin1pt_80_","SR3_bin1pt_90_","SR3_bin1pt_100_","SR3_bin1pt_120_"};
    vector<TString> PT2cut = {"SR3_bin2pt_60_","SR3_bin2pt_70_","SR3_bin2pt_80_","SR3_bin2pt_90_","SR3_bin2pt_100_","SR3_bin2pt_120_"};

    for(auto iht : PT1cut){
      SR3Opt.push_back(iht);
    }
    for(auto iht : PT2cut){
      SR3Opt.push_back(iht);
    }


    if(!IsData )weight = weight*sf_btagM_NLV;

    for (auto is1 : SR3Opt){

      param.Name = param.DefName + is1;
      param.SRConfig  = is1;
      RunAllSignalRegionsOpt(Inclusive, 
			  ElectronCollT,ElectronCollV,MuonCollT,MuonCollV, TauColl, 
			  AllJetColl,JetCollLoose, JetColl, VBF_JetColl,AK8Jet , BJetColl,BJetColl, 
			  ev,METv, param,weight);
      param.Name = param.DefName;
      param.SRConfig  = "";
    }
  }


 
  return;

}



 


HNL_SignalRegionOpt::HNL_SignalRegionOpt(){

  TMVA::Tools::Instance();

  MVAReaderMM = new TMVA::Reader();
  MVAReaderEE = new TMVA::Reader();
  MVAReaderEM = new TMVA::Reader();

}
 
HNL_SignalRegionOpt::~HNL_SignalRegionOpt(){
  delete MVAReaderMM;
  delete MVAReaderEE;
  delete MVAReaderEM;

}


