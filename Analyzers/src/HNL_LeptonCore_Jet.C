#include "HNL_LeptonCore.h"

JetTagging::Parameters HNL_LeptonCore::GetParamJetTagger(AnalyzerParameter& param){
  
  if(param.BTagger == "DeepJet" && param.BWP == "T") return JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Tight, JetTagging::incl, JetTagging::mujets);
  if(param.BTagger == "DeepJet" && param.BWP == "M") return JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets);
  if(param.BTagger == "DeepJet" && param.BWP == "L") return JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Loose, JetTagging::incl, JetTagging::mujets);
  
  if(param.BTagger == "DeepCVS" && param.BWP == "L") return JetTagging::Parameters(JetTagging::DeepCSV, JetTagging::Loose, JetTagging::incl, JetTagging::comb);
  if(param.BTagger == "DeepCVS" && param.BWP == "M") return JetTagging::Parameters(JetTagging::DeepCSV, JetTagging::Medium, JetTagging::incl, JetTagging::comb);
  if(param.BTagger == "DeepCVS" && param.BWP == "T") return JetTagging::Parameters(JetTagging::DeepCSV, JetTagging::Tight, JetTagging::incl, JetTagging::comb);

  cout << "[HNL_LeptonCore::GetParamJetTagger ] Tagger not set.." << param.BTagger<< endl;
  exit(EXIT_FAILURE);
  return JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets);

}

void  HNL_LeptonCore::EvalJetWeight(const std::vector<Jet>&    AK4_JetColl, const std::vector<Jet>&    AK4_VBFJetColl, const std::vector<FatJet>&   AK8_JetColl,  double & w,AnalyzerParameter& param){
  
  if(IsData) return;
    
  /// BJET SF
 
  if (param.Apply_Weight_BJetSF && param.BTagger != "Default") {
    // Get jet tagging parameters
    JetTagging::Parameters param_jets = GetParamJetTagger(param);

    // Get and filter jets
    std::vector<Jet> BJetColl_Tmp = GetHNLJets(param.BJetColl, param);
    std::vector<Jet> BJetColl;
    
    // Only push jets with |Eta| < 2.4
    for (const auto& jet : BJetColl_Tmp) {
      if (std::abs(jet.Eta()) < 2.4) {
	BJetColl.push_back(jet);
      }
    }

    // Apply b-tagging SF and update weights
    double sf_btag = GetBJetSF(param, BJetColl, param_jets);
    w *= sf_btag;

    // Store the SF and fill the histogram
    param.w.btagSF = sf_btag;
    FillWeightHist(param.ChannelDir() + "/" + param.BTagger + "SF" + param.BWP, sf_btag);
  }


  /// PNET SF
  if (param.Apply_Weight_PNETSF) {
    double AK8PNETSF = 1.0;
    double PNETSyst = 0;

    // Determine systematics for PNET
    if (param.syst_ == AnalyzerParameter::JetPNETUp) {
      PNETSyst = 1;
    } else if (param.syst_ == AnalyzerParameter::JetPNETDown) {
      PNETSyst = -1;
    }

    // Apply PNET SF for each jet in AK8_JetColl
    for (const auto& jet : AK8_JetColl) {
      AK8PNETSF *= jet.GetTaggerSF(JetTagging::particleNet_WvsQCD, DataEra, PNETSyst);
    }

    // Update weight and parameter
    w *= AK8PNETSF;
    param.w.PNETSF = AK8PNETSF;

    // Fill histogram
    FillWeightHist(param.ChannelDir() + "/PNET_JETTagger", AK8PNETSF);
  }

  // Jet PUID SF
  if (param.Apply_Weight_JetPUID) {

    vector<Jet> Merged_Jets = AK4_VBFJetColl; 
    for(const auto& jet : AK4_JetColl) {
      if(jet.Pt() < 30) Merged_Jets.push_back(jet);
    }
    double jPUID = GetJetPileupIDSF(Merged_Jets, param.JetPUID, param);

    // Apply weight and update parameter
    w *= jPUID;
    param.w.JetPU = jPUID;

    // Fill histogram
    FillWeightHist(param.ChannelDir() + "/PJet_PUID" + param.JetPUID + "_weight_", jPUID);
  }

  return;
}

std::vector<FatJet> HNL_LeptonCore::GetHNLAK8Jets(const TString& JetType, AnalyzerParameter& param){
  
  std::vector<FatJet>   AK8JetColl  = SelectFatJets(param, param.FatJet_ID, param.FatJet_MinPt, param.FatJet_MaxEta);
  if(JetType=="Loose")  return AK8JetColl;

  std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, param.Electron_MinPt, param.Electron_MaxEta);
  std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID,     param.Muon_MinPt,  param.Muon_MaxEta);

  if(JetType=="HNL")            return SelectAK8Jets  (AK8JetColl,  200., 2.7,  true,   1., false, -999, true,  40., 130.,    "",  ElectronCollV, MuonCollV);
  if(JetType=="HNL_NoMass")     return SelectAK8Jets  (AK8JetColl,  200., 2.7,  true,   1., false, -999, false, 0.,  200000., "",  ElectronCollV, MuonCollV);
  if(JetType=="BDT")            return SelectAK8Jets  (AK8JetColl,  200., 2.7,  true,   1., false, -999, false, 0.,  200000., "",  ElectronCollV, MuonCollV);

  //if(JetType=="HNL_PN")         return SelectAK8Jets  (AK8JetColl,  200., 2.7,  true,   1., false, -999, true,  40., 130.,    "particleNet_WvsQCD", ElectronCollV, MuonCollV);
  if(JetType=="HNL_PN")          return SelectAK8Jets  (AK8JetColl,  200., 2.4,  true,   1., false, -999, false,  0., 200000.,    "particleNet_WvsQCD", ElectronCollV, MuonCollV);

  if(JetType=="EXO17028")        return SelectAK8Jets  (AK8JetColl,  200., 2.7,  true,   1., true, -999, true,  60., 130.,    "",  ElectronCollV, MuonCollV);


  return SelectAK8Jets  (AK8JetColl,  200., 5.,   true,   1., false, -999, false, 0., 20000., "", ElectronCollV, MuonCollV);

}

std::vector<Jet> HNL_LeptonCore::GetHNLJets(const TString& JetType, AnalyzerParameter& param){
  /// AK4                                                                                                                                                                    
  if(JetType=="All")          return SelectJets   ( param, "NoID",      param.Jet_MinPt,  param.Jet_MaxEta);
  if(JetType=="NoCut_Eta3")   return SelectJets   ( param, "NoID",      param.Jet_MinPt,  3.); 
  if(JetType=="SmearCorr")    return SelectJets   ( param, param.Jet_ID,15.,  2.5); /// Used in MET syst

  std::vector<Jet> AK4_Loose      =  SelectJets   ( param, param.Jet_ID, param.Jet_MinPt,  param.Jet_MaxEta);
  // AK8  
  std::vector<FatJet> AK8_JetColl  = GetHNLAK8Jets(param.AK8JetColl, param);

  /// Lepotns for cleaning                                                                                                                                                 
  std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, param.Electron_MinPt, param.Electron_MaxEta);
  std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID,     param.Muon_MinPt,  param.Muon_MaxEta);
  
  if(JetType=="Loose")    return SelectAK4Jets(AK4_Loose,     15., 4.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, AK8_JetColl);
  if(JetType=="Tight")    return SelectAK4Jets(AK4_Loose,     20., 2.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, AK8_JetColl);
  if(JetType=="VBFTight") return SelectAK4Jets(AK4_Loose,     30., 4.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, AK8_JetColl);
  if(JetType=="TightPUL") return SelectAK4Jets(AK4_Loose,     20., 2.7, true,  0.4,0.8, "Loose",   ElectronCollV,MuonCollV, AK8_JetColl);
  if(JetType=="VBFTightPUL") return SelectAK4Jets(AK4_Loose,     30., 4.7, true,  0.4,0.8, "Loose",   ElectronCollV,MuonCollV, AK8_JetColl);

  /// Jets with no AK8 cleaning 
  std::vector<FatJet> DUMMY_AK8_JetColl;
  if(JetType=="Tight_NoAk8Cleaning")    return SelectAK4Jets(AK4_Loose,     20., 2.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, DUMMY_AK8_JetColl);

  /// BJET                                                                                                                                                                 
  JetTagging::Parameters param_jets = GetParamJetTagger(param);
  if(JetType=="BJet"){
    std::vector<Jet> BJetCollLoose    = GetHNLJets(param.BJetColl,     param);
    return SelectBJets(param, BJetCollLoose, param_jets);
  }

  if(JetType=="BJet_NoAk8Cleaning"){
    std::vector<Jet> BJetCollLoose    = GetHNLJets("Tight_NoAk8Cleaning",     param);
    return SelectBJets(param, BJetCollLoose, param_jets);
  }
  
  // Else just return Standard Jet coll for HNL                                                                                                                             
  return SelectAK4Jets(AK4_Loose,     20., 2.7, true,  0.4,0.8, "",   ElectronCollV,MuonCollV, AK8_JetColl);

}


TString HNL_LeptonCore::CloseJetFlavour(std::vector<Jet> jetColl, Muon mu){

  int IdxMatchJet=-1;
  float mindR1=999.;

  for(unsigned int ij=0; ij<jetColl.size(); ij++){
    float dR1=mu.DeltaR(jetColl.at(ij));
    if(dR1>0.4) continue;
    if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
  }

  if(IdxMatchJet < 0)   return "NULL";

  int JetFlavour = jetColl.at(IdxMatchJet).hadronFlavour();

  if(JetFlavour == 5) return "HF";
  if(JetFlavour == 4) return "HF";
  if(JetFlavour == 0) return "LF";
  return "NULL";
}


TString HNL_LeptonCore::CloseJetFlavour(std::vector<Jet> jetColl, Electron el){

  int IdxMatchJet=-1;
  float mindR1=999.;

  for(unsigned int ij=0; ij<jetColl.size(); ij++){
    float dR1=el.DeltaR(jetColl.at(ij));
    if(dR1>0.4) continue;
    if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
  }
  if(IdxMatchJet < 0)   return "NULL";

  int JetFlavour = jetColl.at(IdxMatchJet).hadronFlavour();

  if(JetFlavour == 5) return "HF";
  if(JetFlavour == 4) return "HF";
  if(JetFlavour == 0) return "LF";
  return "NULL";
}


TString HNL_LeptonCore::CloseJetFlavour(std::vector<Jet> jetColl, Lepton* lep){

  int IdxMatchJet=-1;
  float mindR1=999.;

  for(unsigned int ij=0; ij<jetColl.size(); ij++){
    float dR1=lep->DeltaR(jetColl.at(ij));
    if(dR1>0.4) continue;
    if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
  }
  if(IdxMatchJet < 0)   return "NULL";

  int JetFlavour = jetColl.at(IdxMatchJet).hadronFlavour();

  if(JetFlavour == 5) return "HF";
  if(JetFlavour == 4) return "HF";
  if(JetFlavour == 0) return "LF";
  return "NULL";
}



std::vector<Jet> HNL_LeptonCore::SelBJets(std::vector<Jet>& jetColl, JetTagging::Parameters jtp){

  std::vector<Jet> bjetColl;
  for(unsigned int i=0; i<jetColl.size(); i++){
    if( jetColl.at(i).GetTaggerResult(jtp.j_Tagger) <= mcCorr->GetJetTaggingCutValue(jtp.j_Tagger, jtp.j_WP) ) continue;
    bjetColl.push_back( jetColl.at(i) );
  }
  return bjetColl;
}



std::vector<Jet> HNL_LeptonCore::SelectJets(const std::vector<Jet>& jets, TString id, double ptmin, double fetamax){

  std::vector<Jet> out;
  for(unsigned int i=0; i<jets.size(); i++){
    if(!( jets.at(i).Pt()>ptmin ))            continue;
    if(!( fabs(jets.at(i).Eta())<fetamax ))   continue;
    if(!( jets.at(i).PassID(id) ))            continue;

    if(HasFlag("RemoveHEMJet")){
      if(DataEra=="2018"){
	if (jets.at(i).Eta() < -1.3){
          if((jets.at(i).Phi() < -0.87) && (jets.at(i).Phi() > -1.57)) continue;
        }
      } 
    }
    if(HasFlag("ScaleHEMJet")){
      Jet this_jet = jets.at(i);

      if((jets.at(i).Phi() < -0.87) && (jets.at(i).Phi() > -1.57)){
        if (jets.at(i).Eta() < -1.3){
          if (jets.at(i).Eta() > - 2.5)   this_jet *= 0.8;
          else  this_jet *= 0.65;
        }
      }
      out.push_back(this_jet);
    }
    else     out.push_back( jets.at(i) );

  }
  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;
}

std::vector<Jet> HNL_LeptonCore::SelectJets(AnalyzerParameter param,TString id, double ptmin, double fetamax){

  std::vector<Jet> jets_uncorr = All_Jets;
  std::vector<Jet> jets;
  
  std::unordered_map<AnalyzerParameter::Syst, std::pair<int, std::string>> syst_map = {
    {AnalyzerParameter::JetAbsoluteStatUp, {1, "AbsoluteStat"}},       {AnalyzerParameter::JetAbsoluteStatDown, {-1, "AbsoluteStat"}},
    {AnalyzerParameter::JetAbsoluteScaleUp, {1, "AbsoluteScale"}},     {AnalyzerParameter::JetAbsoluteScaleDown, {-1, "AbsoluteScale"}},
    {AnalyzerParameter::JetAbsoluteMPFBiasUp, {1, "AbsoluteMPFBias"}}, {AnalyzerParameter::JetAbsoluteMPFBiasDown, {-1, "AbsoluteMPFBias"}},
    {AnalyzerParameter::JetFragmentationUp, {1, "Fragmentation"}},     {AnalyzerParameter::JetFragmentationDown, {-1, "Fragmentation"}},
    {AnalyzerParameter::JetSinglePionECALUp, {1, "SinglePionECAL"}},   {AnalyzerParameter::JetSinglePionECALDown, {-1, "SinglePionECAL"}},
    {AnalyzerParameter::JetSinglePionHCALUp, {1, "SinglePionHCAL"}},   {AnalyzerParameter::JetSinglePionHCALDown, {-1, "SinglePionHCAL"}},
    {AnalyzerParameter::JetFlavorQCDUp, {1, "FlavorQCD"}},             {AnalyzerParameter::JetFlavorQCDDown, {-1, "FlavorQCD"}},
    {AnalyzerParameter::JetTimePtEtaUp, {1, "TimePtEta"}},             {AnalyzerParameter::JetTimePtEtaDown, {-1, "TimePtEta"}},
    {AnalyzerParameter::JetRelativeJEREC1Up, {1, "RelativeJEREC1"}},   {AnalyzerParameter::JetRelativeJEREC1Down, {-1, "RelativeJEREC1"}},
    {AnalyzerParameter::JetRelativeJEREC2Up, {1, "RelativeJEREC2"}},   {AnalyzerParameter::JetRelativeJEREC2Down, {-1, "RelativeJEREC2"}},
    {AnalyzerParameter::JetRelativePtHFUp, {1, "RelativePtHF"}},       {AnalyzerParameter::JetRelativePtHFDown, {-1, "RelativePtHF"}},
    {AnalyzerParameter::JetRelativePtBBUp, {1, "RelativePtBB"}},       {AnalyzerParameter::JetRelativePtBBDown, {-1, "RelativePtBB"}},
    {AnalyzerParameter::JetRelativePtEC1Up, {1, "RelativePtEC1"}},     {AnalyzerParameter::JetRelativePtEC1Down, {-1, "RelativePtEC1"}},
    {AnalyzerParameter::JetRelativePtEC2Up, {1, "RelativePtEC2"}},     {AnalyzerParameter::JetRelativePtEC2Down, {-1, "RelativePtEC2"}},
    {AnalyzerParameter::JetRelativeBalUp, {1, "RelativeBal"}},         {AnalyzerParameter::JetRelativeBalDown, {-1, "RelativeBal"}},
    {AnalyzerParameter::JetRelativeSampleUp, {1, "RelativeSample"}},   {AnalyzerParameter::JetRelativeSampleDown, {-1, "RelativeSample"}},
    {AnalyzerParameter::JetRelativeFSRUp, {1, "RelativeFSR"}},         {AnalyzerParameter::JetRelativeFSRDown, {-1, "RelativeFSR"}},
    {AnalyzerParameter::JetRelativeStatFSRUp, {1, "RelativeStatFSR"}}, {AnalyzerParameter::JetRelativeStatFSRDown, {-1, "RelativeStatFSR"}},
    {AnalyzerParameter::JetRelativeStatECUp, {1, "RelativeStatEC"}},   {AnalyzerParameter::JetRelativeStatECDown, {-1, "RelativeStatEC"}},
    {AnalyzerParameter::JetRelativeStatHFUp, {1, "RelativeStatHF"}},   {AnalyzerParameter::JetRelativeStatHFDown, {-1, "RelativeStatHF"}},
    {AnalyzerParameter::JetPileUpDataMCUp, {1, "PileUpDataMC"}},       {AnalyzerParameter::JetPileUpDataMCDown, {-1, "PileUpDataMC"}},
    {AnalyzerParameter::JetPileUpPtRefUp, {1, "PileUpPtRef"}},         {AnalyzerParameter::JetPileUpPtRefDown, {-1, "PileUpPtRef"}},
    {AnalyzerParameter::JetPileUpPtBBUp, {1, "PileUpPtBB"}},           {AnalyzerParameter::JetPileUpPtBBDown, {-1, "PileUpPtBB"}},
    {AnalyzerParameter::JetPileUpPtEC1Up, {1, "PileUpPtEC1"}},         {AnalyzerParameter::JetPileUpPtEC1Down, {-1, "PileUpPtEC1"}},
    {AnalyzerParameter::JetPileUpPtEC2Up, {1, "PileUpPtEC2"}},         {AnalyzerParameter::JetPileUpPtEC2Down, {-1, "PileUpPtEC2"}},
    {AnalyzerParameter::JetPileUpPtHFUp, {1, "PileUpPtHF"}},           {AnalyzerParameter::JetPileUpPtHFDown, {-1, "PileUpPtHF"}}
  };
  
  if(param.syst_ == AnalyzerParameter::JetEnUp)            jets    = ScaleJets( jets_uncorr, +1 );
  else if(param.syst_ == AnalyzerParameter::JetEnDown)     jets    = ScaleJets( jets_uncorr, -1 );
  else if(param.syst_ == AnalyzerParameter::JetResUp)      jets    = SmearJets(jets_uncorr, +1 );
  else if(param.syst_ == AnalyzerParameter::JetResDown)    jets    = SmearJets(jets_uncorr, -1 );
  else if (syst_map.find(param.syst_) != syst_map.end()) {
    jets = ScaleJetsIndividualSource(jets_uncorr, syst_map[param.syst_].first, syst_map[param.syst_].second);
  }
  else jets =jets_uncorr;
  
  std::vector<Jet> out;
  for(unsigned int i=0; i<jets.size(); i++){
    if(!( jets.at(i).Pt()> ptmin ))            continue;
    if(!( fabs(jets.at(i).Eta() )< fetamax ))  continue;
    if(!( jets.at(i).PassID( id) ))            continue;

    if(HasFlag("RemoveHEMJet")){
      if(DataEra=="2018"){
        if (jets.at(i).Eta() < -1.3){
          if((jets.at(i).Phi() < -0.87) && (jets.at(i).Phi() > -1.57)) continue;
        }
      }
    }
    if(param.syst_ == AnalyzerParameter::HEMJet || HasFlag("ScaleHEMJet")){
      Jet this_jet = jets.at(i);

      if((jets.at(i).Phi() < -0.87) && (jets.at(i).Phi() > -1.57)){
        if (jets.at(i).Eta() < -1.3){
          if (jets.at(i).Eta() > - 2.5)   this_jet *= 0.8;
          else  this_jet *= 0.65;
        }
      }
      out.push_back(this_jet);
    }
    else     out.push_back( jets.at(i) );

  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;
}






std::vector<FatJet> HNL_LeptonCore::SelectFatJets(const std::vector<FatJet>& jets, TString id, double ptmin, double fetamax){

  std::vector<FatJet> out;
  for(unsigned int i=0; i<jets.size(); i++){
    if(!( jets.at(i).Pt()>ptmin ))           continue;
    if(!( fabs(jets.at(i).Eta())<fetamax ))  continue;
    if(!( jets.at(i).PassID(id) ))           continue;

    if(HasFlag("RemoveHEMJet")){
      if(DataEra=="2018"){
        if (jets.at(i).Eta() < -1.3){
          if((jets.at(i).Phi() < -0.87) && (jets.at(i).Phi() > -1.57)) continue;
        }
      }
    }
    if(HasFlag("ScaleHEMJet")){
      FatJet this_jet = jets.at(i);

      if((jets.at(i).Phi() < -0.87) && (jets.at(i).Phi() > -1.57)){
        if (jets.at(i).Eta() < -1.3){
          if (jets.at(i).Eta() > - 2.5)   this_jet *= 0.8;
          else  this_jet *= 0.65;
        }
      }
      out.push_back(this_jet);
    }
    else     out.push_back( jets.at(i) );

  }
  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;
}



std::vector<FatJet> HNL_LeptonCore::SelectFatJets(AnalyzerParameter param,TString id, double ptmin, double fetamax){

  std::vector<FatJet> jets_pc = puppiCorr->Correct(All_FatJets);
  std::vector<FatJet> jets;
  int sys_ak8_scale = 0;
  int sys_ak8_smear = 0;

  if(param.syst_ == AnalyzerParameter::JetEnUp)    sys_ak8_scale=1;
  if(param.syst_ == AnalyzerParameter::JetEnDown)  sys_ak8_scale=-1;
  if(param.syst_ == AnalyzerParameter::JetResUp)   sys_ak8_smear=1;
  if(param.syst_ == AnalyzerParameter::JetResDown) sys_ak8_smear=-1;
  jets    = ScaleFatJets( jets_pc, sys_ak8_scale );
  jets    = SmearFatJets(jets, sys_ak8_smear );

  std::vector<FatJet> out;
  for(unsigned int i=0; i<jets.size(); i++){
    if(!( jets.at(i).Pt()>  ptmin ))          continue;
    if(!( fabs(jets.at(i).Eta())< fetamax ))  continue;
    if(!( jets.at(i).PassID(id) ))            continue;

    if(HasFlag("RemoveHEMJet")){
      if(DataEra=="2018"){
        if (jets.at(i).Eta() < -1.3){
          if((jets.at(i).Phi() < -0.87) && (jets.at(i).Phi() > -1.57)) continue;
        }
      }
    }
    if(HasFlag("ScaleHEMJet")){
      FatJet this_jet = jets.at(i);

      if((jets.at(i).Phi() < -0.87) && (jets.at(i).Phi() > -1.57)){
        if (jets.at(i).Eta() < -1.3){
          if (jets.at(i).Eta() > - 2.5)   this_jet *= 0.8;
          else  this_jet *= 0.65;
        }
      }
      out.push_back(this_jet);
    }
    else     out.push_back( jets.at(i) );
    
  }

  std::sort(out.begin(),       out.end(),        PtComparing);
  return out;
}



vector<Jet>   HNL_LeptonCore::SelectBJets(AnalyzerParameter param,vector<Jet> jetColl, JetTagging::Parameters jtp){

  vector<Jet> output_jets;

  for(unsigned int ijet =0; ijet < jetColl.size(); ijet++){
    if(fabs(jetColl.at(ijet).Eta()) > 2.4) continue;
    
    if( jetColl[ijet].GetTaggerResult(jtp.j_Tagger) <= mcCorr->GetJetTaggingCutValue(jtp.j_Tagger, jtp.j_WP) ) continue;
    output_jets.push_back( jetColl.at(ijet) );
  }
  std::sort(output_jets.begin(),       output_jets.end(),        PtComparing);
  return output_jets;

}

vector<Jet>   HNL_LeptonCore::SelectLJets(AnalyzerParameter param,vector<Jet> jetColl, JetTagging::Parameters jtp){

  vector<Jet> output_jets;

  for(unsigned int ijet =0; ijet < jetColl.size(); ijet++){
    if( jetColl[ijet].GetTaggerResult(jtp.j_Tagger) > mcCorr->GetJetTaggingCutValue(jtp.j_Tagger, jtp.j_WP) ) continue;
    output_jets.push_back( jetColl.at(ijet) );
  }
  std::sort(output_jets.begin(),       output_jets.end(),        PtComparing);
  return output_jets;

}


vector<Jet>   HNL_LeptonCore::SelectAK4Jets(vector<Jet> jets, double pt_cut ,  double eta_cut, bool lepton_cleaning  , double dr_lep_clean, double dr_ak8_clean, TString pu_tag,std::vector<Lepton *> leps_veto,  vector<FatJet> fatjets){

  vector<Jet> output_jets;
  for(unsigned int ijet =0; ijet < jets.size(); ijet++){
    bool jetok=true;

    if(fabs(jets[ijet].Eta()) > eta_cut) continue;
    if(jets[ijet].Pt() < pt_cut)continue;

    for(auto ilep : leps_veto){
      if(ilep->DeltaR(jets[ijet]) < dr_lep_clean) jetok = false;
    }

    for(unsigned int ifjet =0; ifjet < fatjets.size(); ifjet++){
      if(jets[ijet].DeltaR(fatjets[ifjet]) <dr_ak8_clean) jetok = false;
    }

    if(lepton_cleaning&&!jetok) continue;
    if(pu_tag=="")output_jets.push_back(jets[ijet]);
    else {
      if(jets[ijet].PassPileupMVA(pu_tag,GetEra())) output_jets.push_back(jets[ijet]);
    }
  }

  std::sort(output_jets.begin(),       output_jets.end(),        PtComparing);

  return output_jets;

}




vector<Jet>   HNL_LeptonCore::SelectAK4Jets(vector<Jet> jets, double pt_cut ,  double eta_cut, bool lepton_cleaning  , double dr_lep_clean, double dr_ak8_clean, TString pu_tag, vector<Electron>  veto_electrons, vector<Muon>  veto_muons, vector<FatJet> fatjets){

  vector<Jet> output_jets;
  for(unsigned int ijet =0; ijet < jets.size(); ijet++){
    bool jetok=true;

    if(fabs(jets[ijet].Eta()) > eta_cut) continue;
    if(jets[ijet].Pt() < pt_cut)continue;

    for(unsigned int iel=0 ; iel < veto_electrons.size(); iel++){
      if(jets[ijet].DeltaR(veto_electrons[iel]) < dr_lep_clean) jetok = false;
    }

    for(unsigned int iel=0 ; iel < veto_muons.size(); iel++){
      if(jets[ijet].DeltaR(veto_muons[iel]) < dr_lep_clean) jetok = false;
    }
    for(unsigned int ifjet =0; ifjet < fatjets.size(); ifjet++){
      if(jets[ijet].DeltaR(fatjets[ifjet]) <dr_ak8_clean) jetok = false;
    }

    if(lepton_cleaning&&!jetok) continue;
    if(pu_tag=="")output_jets.push_back(jets[ijet]);
    else if(jets[ijet].PassPileupMVA(pu_tag,GetEra())) output_jets.push_back(jets[ijet]);
  }
  std::sort(output_jets.begin(),       output_jets.end(),        PtComparing);

  return output_jets;
}



vector<FatJet>  HNL_LeptonCore::SelectAK8Jets(vector<FatJet> fatjets, double pt_cut ,  double eta_cut, bool lepton_cleaning  , double dr_lep_clean , bool apply_tau21, double tau21_cut , bool apply_masscut, double sdmass_lower_cut,  double sdmass_upper_cut, TString  tagger,  vector<Electron>  veto_electrons, vector<Muon>  veto_muons){

  bool DEBUGMode=false;

  vector<FatJet> output_fatjets;
  for(unsigned int ijet =0; ijet < fatjets.size(); ijet++){
    
    if(abs(fatjets[ijet].Eta()) > 2.3) DEBUGMode=true;
    else DEBUGMode=false;

    bool jetok=true;

    for(unsigned int iel=0 ; iel < veto_electrons.size(); iel++){
      if(fatjets[ijet].DeltaR(veto_electrons[iel]) < dr_lep_clean) jetok = false;
    }

    for(unsigned int iel=0 ; iel < veto_muons.size(); iel++){
      if(fatjets[ijet].DeltaR(veto_muons[iel]) < dr_lep_clean) jetok = false;
    }
    
    if( tagger != ""){
      if (!fatjets[ijet].PassTagger(JetTagging::StringToTagger(string(tagger)), DataEra)) continue;
    }

    double lower_sd_mass_cut=sdmass_lower_cut;
    double upper_sd_mass_cut=sdmass_upper_cut;
    if(sdmass_lower_cut < 0.){
      lower_sd_mass_cut = 40.;
      upper_sd_mass_cut = 130.;
      if(DataYear==2017){
        lower_sd_mass_cut=65.;
        upper_sd_mass_cut=105.;
      }
    }
    // tau21 cut has SF so need to apply SD mass for 2017                                                                                                                                                                                                                                                                                                                                  
    if(apply_tau21) {
      if(DataYear==2017) {
        lower_sd_mass_cut  = 65.;
        upper_sd_mass_cut  = 105.;
      }
    }

    double tau_21_cut = tau21_cut;
    if(tau21_cut > 0.){
      if(DataYear==2016) tau_21_cut = 0.55;
      if(DataYear==2017) tau_21_cut = 0.75;
      if(DataYear==2018) tau_21_cut = 0.75;
    }
    else{
      if(DataYear==2016) tau_21_cut = 0.35;
      if(DataYear==2017) tau_21_cut = 0.45;
      if(DataYear==2018) tau_21_cut = 0.45;
    }
    if( fabs(fatjets[ijet].Eta()) > eta_cut)    continue;
    if( fatjets[ijet].Pt()  < pt_cut)           continue;

    if(lepton_cleaning && !jetok)  continue;
    if(apply_tau21 && !fatjets[ijet].PassPuppiTau21(tau_21_cut))  continue;
    if(apply_masscut && !fatjets[ijet].PassSDMassrange(lower_sd_mass_cut,upper_sd_mass_cut)) continue;


    output_fatjets.push_back(fatjets[ijet]);
  }


  std::sort(output_fatjets.begin(),       output_fatjets.end(),        PtComparing);

  return output_fatjets;
}


double  HNL_LeptonCore::GetBJetSF(AnalyzerParameter param,vector<Jet> jets, JetTagging::Parameters jtp){

  if(IsData) return 1.;
  string syst = "";
  if(param.syst_ == AnalyzerParameter::BTagSFHTagCorrUp)         syst="SystUpHTagCorr";
  else if (param.syst_ == AnalyzerParameter::BTagSFHTagCorrDown) syst="SystDownHTagCorr";
  else if (param.syst_ == AnalyzerParameter::BTagSFLTagCorrUp)   syst="SystUpLTagCorr";
  else if (param.syst_ == AnalyzerParameter::BTagSFLTagCorrDown) syst="SystDownLTagCorr";
  else if (param.syst_ == AnalyzerParameter::BTagSFHTagUnCorrUp)   syst="SystUpHTagUnCorr";
  else if (param.syst_ == AnalyzerParameter::BTagSFHTagUnCorrDown) syst="SystDownHTagUnCorr";
  else if (param.syst_ == AnalyzerParameter::BTagSFLTagUnCorrUp)   syst="SystUpLTagUnCorr";
  else if (param.syst_ == AnalyzerParameter::BTagSFLTagUnCorrDown) syst="SystDownLTagUnCorr";

  return mcCorr->GetBTaggingReweight_1a(jets, jtp, syst);
}


double HNL_LeptonCore::GetJetPileupIDSF(const std::vector<Jet>& jets, const TString& WP, const AnalyzerParameter& param) {
  if (IsData || WP.IsNull() || WP == "None") return 1.0;

  double JPU_W = 1.0;

  // Determine systematics direction
  int syst_dir = 0;
  if (param.syst_ == AnalyzerParameter::JetPUIDUp) {
    syst_dir = 1;
  } else if (param.syst_ == AnalyzerParameter::JetPUIDDown) {
    syst_dir = -1;
  }

  // Apply pileup ID SF for each jet
  for (const auto& jet : jets) {
    JPU_W *= mcCorr->JetPileUpSF(jet, WP, syst_dir);
  }

  return JPU_W;
}

double HNL_LeptonCore::GetEventFatJetSF(const std::vector<FatJet>& fatjets, const TString& label, int dir) {
  double FatJetTau21_SF = 1.0;
    
  for (const auto& fatjet : fatjets) {
    FatJetTau21_SF *= GetFatJetSF(fatjet, label, dir);
  }

  return FatJetTau21_SF;
}




double HNL_LeptonCore::GetFatJetSF(const FatJet& fatjet, const TString& tag, int dir) {
  if (IsDATA) return 1.0;

  double fsys = (dir > 0) ? 1.0 : (dir == 0 ? 0.0 : -1.0);
  double loose_sf = 1.0;

  if (tag.Contains("HP")) {
    switch (DataYear) {
    case 2016: loose_sf = 0.99 + fsys * 0.11; break;
    case 2017: loose_sf = 0.974 + fsys * 0.029; break;
    case 2018: loose_sf = 0.980 + fsys * 0.019; break;
    }
    return loose_sf;
  }
  else if (tag.Contains("LP")) {
    if (DataYear == 2016) {
      loose_sf = 1.03 + fsys * 0.14;
    }
    else if (DataYear == 2017) {
      if (fatjet.PuppiTau21() < 0.45) {
	loose_sf = 0.974 + fsys * 0.029;
      }
      else if (fatjet.PuppiTau21() < 0.75) {
	loose_sf = 1.136 + fsys * 0.162;
      }
    }
    else if (DataYear == 2018) {
      if (fatjet.PuppiTau21() < 0.45) {
	loose_sf = 0.980 + fsys * 0.019;
      }
      else if (fatjet.PuppiTau21() < 0.75) {
	loose_sf = 1.20 + fsys * 0.194;
      }
    }
    return loose_sf;
  }

  return 1.0;
}

