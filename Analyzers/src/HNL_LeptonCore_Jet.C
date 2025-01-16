#include "HNL_LeptonCore.h"

JetTagging::Parameters HNL_LeptonCore::GetParamJetTagger(AnalyzerParameter param){
  
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

void  HNL_LeptonCore::EvalJetWeight(std::vector<Jet>    AK4_JetColl, std::vector<FatJet>   AK8_JetColl,  double & w,AnalyzerParameter& param){
  
  if(IsData) return;
    
  /// BJET SF
 
  if(param.Apply_Weight_BJetSF && param.BTagger != "Default"){
    JetTagging::Parameters param_jets = GetParamJetTagger(param);
    std::vector<Jet>    BJetColl_Tmp      = GetHNLJets(param.BJetColl,     param);
    std::vector<Jet>    BJetColl;
    for(int ij =0 ; ij < BJetColl_Tmp.size(); ij++) {
      if(fabs(BJetColl_Tmp[ij].Eta()) < 2.4) BJetColl.push_back(BJetColl_Tmp[ij]);
    }
    double sf_btag                    = GetBJetSF(param, BJetColl, param_jets);
    w*= sf_btag;

    param.w.btagSF=sf_btag;
    FillWeightHist(param.ChannelDir()+"/"+param.BTagger+"SF"+param.BWP, sf_btag);
  }

  /// PNET SF
  if(param.Apply_Weight_PNETSF){
    double AK8PNETSF(1.);
    
    double PNETSyst = 0;
    if(param.syst_ == AnalyzerParameter::JetPNETUp) PNETSyst = 1;
    if(param.syst_ == AnalyzerParameter::JetPNETDown) PNETSyst = -1;
    
    for(auto iJ : AK8_JetColl) AK8PNETSF*= iJ.GetTaggerSF(JetTagging::particleNet_WvsQCD, DataEra, PNETSyst);
    w*= AK8PNETSF;
    param.w.PNETSF=AK8PNETSF;

    FillWeightHist(param.ChannelDir()+"/PNET_JETTagger", AK8PNETSF);
  }
  if(param.Apply_Weight_JetPUID){
    double jPUID = GetJetPileupIDSF(AK4_JetColl, param.JetPUID, param);
    w*= FillWeightHist(param.ChannelDir()+"/PJet_PUID"+param.JetPUID+"_weight_" , jPUID);
    param.w.JetPU=jPUID;
  }
  return;
}

std::vector<FatJet> HNL_LeptonCore::GetHNLAK8Jets(TString JetType, AnalyzerParameter param){
  
  std::vector<FatJet>   AK8JetColl  = SelectFatJets(param, param.FatJet_ID, param.FatJet_MinPt, param.FatJet_MaxEta);
  if(JetType=="Loose")  return AK8JetColl;

  std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, param.Electron_MinPt, param.Electron_MaxEta);
  std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID,     param.Muon_MinPt,  param.Muon_MaxEta);

  if(JetType=="HNL")            return SelectAK8Jets  (AK8JetColl,  200., 2.7,  true,   1., false, -999, true,  40., 130.,    "",  ElectronCollV, MuonCollV);
  if(JetType=="HNL_NoMass")     return SelectAK8Jets  (AK8JetColl,  200., 2.7,  true,   1., false, -999, false, 0.,  200000., "",  ElectronCollV, MuonCollV);
  if(JetType=="BDT")            return SelectAK8Jets  (AK8JetColl,  200., 2.7,  true,   1., false, -999, false, 0.,  200000., "",  ElectronCollV, MuonCollV);
  if(JetType=="HNL_PN")         return SelectAK8Jets  (AK8JetColl,  200., 2.7,  true,   1., false, -999, true,  40., 130.,    "particleNet_WvsQCD", ElectronCollV, MuonCollV);
  if(JetType=="HNL_PN_NoMass")  return SelectAK8Jets  (AK8JetColl,  200., 2.7,  true,   1., false, -999, true,  0., 200000.,    "particleNet_WvsQCD", ElectronCollV, MuonCollV);
  if(JetType=="EXO17028")        return SelectAK8Jets  (AK8JetColl,  200., 2.7,  true,   1., true, -999, true,  60., 130.,    "",  ElectronCollV, MuonCollV);


  return SelectAK8Jets  (AK8JetColl,  200., 5.,   true,   1., false, -999, false, 0., 20000., "", ElectronCollV, MuonCollV);

}

std::vector<Jet> HNL_LeptonCore::GetHNLJets(TString JetType, AnalyzerParameter param){
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



vector<Jet> HNL_LeptonCore::SkimJetColl(const vector<Jet>& JetColl, vector<Gen>& TruthColl, AnalyzerParameter param,TString Option){

  bool GetPrLepCleanJet=false;
  TString Criteria="";
  if(Option.Contains("NoPr"))  GetPrLepCleanJet =true;
  if(Option.Contains("NoTau")) Criteria="InclTau";

  vector<Jet> ReturnVec;
  for(unsigned int i=0; i<JetColl.size(); i++){
    bool HasEWLep=HasEWLepInJet(JetColl.at(i), TruthColl, Criteria);
    if( GetPrLepCleanJet && (!HasEWLep) ) ReturnVec.push_back(JetColl.at(i));
  }

  return ReturnVec;
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
  if(param.syst_ == AnalyzerParameter::JetEnUp)            jets    = ScaleJets( jets_uncorr, +1 );
  else if(param.syst_ == AnalyzerParameter::JetEnDown)     jets    = ScaleJets( jets_uncorr, -1 );
  else if(param.syst_ == AnalyzerParameter::JetResUp)      jets    = SmearJets(jets_uncorr, +1 );
  else if(param.syst_ == AnalyzerParameter::JetResDown)    jets    = SmearJets(jets_uncorr, -1 );
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
  int sys_ak8_mass_scale = 0;
  int sys_ak8_mass_smear = 0;

  if(param.syst_ == AnalyzerParameter::JetEnUp)    sys_ak8_scale=1;
  if(param.syst_ == AnalyzerParameter::JetEnDown)  sys_ak8_scale=-1;
  if(param.syst_ == AnalyzerParameter::JetResUp)   sys_ak8_smear=1;
  if(param.syst_ == AnalyzerParameter::JetResDown) sys_ak8_smear=-1;
  if(param.syst_ == AnalyzerParameter::JetMassUp)  sys_ak8_mass_scale=1;
  if(param.syst_ == AnalyzerParameter::JetMassDown)  sys_ak8_mass_scale=-1;
  if(param.syst_ == AnalyzerParameter::JetMassSmearUp) sys_ak8_mass_smear=1;
  if(param.syst_ == AnalyzerParameter::JetMassSmearDown) sys_ak8_mass_smear=-1;

  jets    = ScaleFatJets( jets_pc, sys_ak8_scale );
  jets    = SmearFatJets(jets, sys_ak8_smear );
  jets    = ScaleSDMassFatJets( jets, sys_ak8_mass_scale );
  jets    = SmearSDMassFatJets( jets, sys_ak8_mass_smear );

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


  vector<FatJet> output_fatjets;
  for(unsigned int ijet =0; ijet < fatjets.size(); ijet++){

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
  if(param.syst_ == AnalyzerParameter::BTagSFHTagUp)         syst="SystHTagUp";
  else if (param.syst_ == AnalyzerParameter::BTagSFHTagDown) syst="SystHTagDown";
  else if (param.syst_ == AnalyzerParameter::BTagSFLTagUp)   syst="SystLTagUp";
  else if (param.syst_ == AnalyzerParameter::BTagSFLTagDown) syst="SystLTagDown";

  return mcCorr->GetBTaggingReweight_1a(jets, jtp, syst);
}


double HNL_LeptonCore::GetJetPileupIDSF(vector<Jet> jets , TString WP, AnalyzerParameter param){

  if(IsData) return 1.;
  if(WP=="") return 1.;
  if(WP=="None") return 1.;

  double JPU_W=1.;
  for(auto ij: jets){
    if(param.syst_ == AnalyzerParameter::JetPUIDUp)   JPU_W*= mcCorr->JetPileUpSF(ij, WP,1 );
    else if(param.syst_ == AnalyzerParameter::JetPUIDDown)   JPU_W*= mcCorr->JetPileUpSF(ij, WP,-1 );
    else JPU_W*= mcCorr->JetPileUpSF(ij, WP,0 );
  }

  return JPU_W;
}


double HNL_LeptonCore::GetEventFatJetSF(vector<FatJet> fatjets, TString label, int dir){
  double FatJetTau21_SF(1);
  for (auto ifj : fatjets)   FatJetTau21_SF*=GetFatJetSF(ifj, label,dir);
  return FatJetTau21_SF;
}

double HNL_LeptonCore::GetFatJetSF(FatJet fatjet, TString tag,  int dir){

  if(IsDATA) return 1.;
  double fsys = -1;
  if(dir > 0) fsys =1;
  if(dir==0) fsys=0.;


  double loose_sf(1.);
  if(tag.Contains("HP")){
    if(DataYear==2016) loose_sf = 0.99  + fsys*0.11;
    if(DataYear==2017) loose_sf = 0.974 + fsys*0.029;
    if(DataYear==2018) loose_sf = 0.980 + fsys*0.019;
    return loose_sf;
  }
  else if(tag.Contains("LP")){

    if(DataYear==2016) loose_sf = 1.03 + fsys*0.14;

    else if(DataYear==2017) {
      if (fatjet.PuppiTau21() < 0.45) loose_sf = 0.974 + fsys*0.029;
      else if (fatjet.PuppiTau21() < 0.75)    loose_sf = 1.136 + fsys*0.162;
    }

    else if(DataYear==2018) {
      if (fatjet.PuppiTau21() <0.45) loose_sf = 0.980 + fsys*0.019;
      else if (fatjet.PuppiTau21() < 0.75)  loose_sf = 1.20 + fsys*0.194;
    }
    return loose_sf;
  }

  return 1.;

}
