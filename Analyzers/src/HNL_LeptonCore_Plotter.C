#include "HNL_LeptonCore.h"

/*------------------------------------------------------------------------------------------------------------                                           
------------------------------------------------------------------------------------------------------------  
//                                                                                                          
//                                                                                                          
// FUNCTIONS NOW ARE FOR PLOTTING OF SR/CR/GENERAL                                                         
//                                                                                                         
------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------*/

bool HNL_LeptonCore::DrawSyst(AnalyzerParameter& param_sys){
  
  vector<AnalyzerParameter::Syst> SystToPlot;
  if(IsData){
    if(RunFake) SystToPlot= {AnalyzerParameter::Syst::FRUp,AnalyzerParameter::Syst::FRDown};
    
    if(RunCF)   SystToPlot= {AnalyzerParameter::Syst::CFRateUp,AnalyzerParameter::Syst::CFRateDown};
    
  }
  else {
    SystToPlot= {AnalyzerParameter::Syst::JetEnUp, AnalyzerParameter::Syst::JetEnDown,AnalyzerParameter::ScaleUp,AnalyzerParameter::ScaleDown, AnalyzerParameter::JetResUp,AnalyzerParameter::JetResDown,AnalyzerParameter::JetPNETUp,AnalyzerParameter::JetPNETDown,AnalyzerParameter::MuonResUp,AnalyzerParameter::MuonResDown,AnalyzerParameter::MuonEnUp,AnalyzerParameter::MuonEnDown,AnalyzerParameter::ElectronEnUp,AnalyzerParameter::ElectronEnDown,AnalyzerParameter::ElectronResUp,AnalyzerParameter::ElectronResDown,AnalyzerParameter::RenScaleUp,AnalyzerParameter::RenScaleDown,AnalyzerParameter::FacScaleUp,AnalyzerParameter::FacScaleDown};
  }
  
  SystToPlot.push_back(AnalyzerParameter::Syst::Central);
  if(std::find(SystToPlot.begin(), SystToPlot.end(), param_sys.syst_) != SystToPlot.end()) return true;
  else return false;
}

void HNL_LeptonCore::Fill_PlotsAK8(AnalyzerParameter& param, TString  region, TString plot_dir, 
           std::vector<Tau>& TauColl, std::vector<Jet>& jets,   std::vector<FatJet>& fatjets, std::vector<Lepton *>& leps , 
           Particle&  met, double nvtx,  double w){

  // Early exit if conditions are not met
  if(param.syst_ != AnalyzerParameter::Syst::Central || fatjets.empty() || leps.size() < 2) return;

  // Check if the lepton count matches the channel type
  std::map<int, std::string> channelTypeMap = {{2, "Dilepton"}, {3, "Trilepton"}, {4, "Quadlepton"}};
  if(param.ChannelType() != "Lepton"){
    if(param.ChannelType() != channelTypeMap[leps.size()]) return;
  }

  //  FillLeptonPlots(param, leps, plot_dir + region, w);
  
  // Main plots for AK8 Jets
  FillHist(plot_dir + region + "/AK8J_N", fatjets.size(), w, 5, 0.0, 5.0, "N_{AK8 jets}");

  int NAK8B = 0;
  for (const auto& fatjet : fatjets) {
    // Plot AK8 jet information
    FillHist(plot_dir + region + "/AK8J_Eta", fatjet.Eta(), w, 100, -5.0, 5.0, "AK8 Jet #eta");
    FillHist(plot_dir + region + "/AK8J_Pt", fatjet.Pt(), w, 100, 0.0, 2000.0, "AK8 Jet p_{T} GeV");
    FillHist(plot_dir + region + "/AK8J_Tagger_particleNet_WvsQCD", fatjet.GetTaggerResult(JetTagging::particleNet_WvsQCD), w, 50, 0, 1.0, "JetTagging::particleNet_WvsQCD");

    // Count the number of medium b-tagged jets
    if (fatjet.GetTaggerResult(JetTagging::DeepCSV) > mcCorr->GetJetTaggingCutValue(JetTagging::DeepCSV, JetTagging::Medium)) {
      NAK8B++;
    }
  }

  FillHist(plot_dir + region + "/AK8J_NB", NAK8B, w, 5, 0.0, 5.0, "N_{AK8 jets}");

  // Create Particle combinations for mass calculations
  Particle N1Cand = fatjets[0] + *leps[0];
  Particle N2Cand = fatjets[0] + *leps[1];
  Particle llJCand = *leps[0] + *leps[1] + fatjets[0];

  int nAk8bins=3;
  double AK8bins[nAk8bins+1] = { 0.0,300.0,500.0,2000.0};

  // Fill mass histograms [binned for python plotter, Main plots for unbinned]
  FillHist(plot_dir + region + "/AK8J_Mass/l1J", N1Cand.M(), w, nAk8bins,AK8bins, "Reco M_{l1J}");
  FillHist(plot_dir + region + "/AK8J_Mass/l2J", N2Cand.M(), w,nAk8bins,AK8bins , "Reco M_{l2J}");
  FillHist(plot_dir + region + "/AK8J_Mass/llJ", llJCand.M(), w, nAk8bins,AK8bins, "Reco M_{llJ}");
  
  //// Now Add detailed plots by adding userflag
  if(!User("jalmond")) return;

  FillHist(plot_dir + region + "/AK8J_Unbinned_Mass/l1J", N1Cand.M(), w, 1000,0.0,5000.0, "Reco M_{l1J}");
  FillHist(plot_dir + region + "/AK8J_Unbinned_Mass/l2J", N2Cand.M(), w,  1000,0.0,5000.0, "Reco M_{l2J}");
  FillHist(plot_dir + region + "/AK8J_Unbinned_Mass/llJ", llJCand.M(), w, 1000,0.0,5000.0, "Reco M_{llJ}");

  
  // Loop over FatJets and perform necessary calculations
  for (auto& fatjet : fatjets) {

    for (auto& ilep : leps) {
      FillHist(plot_dir + region + "/AK8Jet_dR_" + ilep->GetFlavour(), fatjet.DeltaR(*ilep), w, 50, 0.0, 10.0, "#DeltaR (WAK8," + ilep->GetFlavour() + ")");
    }

    FillHist(plot_dir + region + "/AK8Jet_dR_lep1" , fatjet.DeltaR(*leps[0]), w, 50, 0.0, 10.0, "#DeltaR (WAK8,lep1)");
    FillHist(plot_dir + region + "/AK8Jet_dR_lep2" , fatjet.DeltaR(*leps[1]), w, 50, 0.0, 10.0, "#DeltaR (WAK8,lep2)");
    
    double J_l1_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (fatjet).Phi() )) );
    double J_l2_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[1]).Phi() - (fatjet).Phi() )) );
    FillHist(plot_dir + region + "/AK8Jet_dPhi_lep1", J_l1_dphi , w, 50, -5.5, 100, "");
    FillHist(plot_dir + region + "/AK8Jet_dPhi_lep2", J_l2_dphi , w, 50, -5.5, 100, "");
    
    
    // Additional Histograms for FatJets
    FillHist(plot_dir + region + "/AK8J_SDMass", fatjet.SDMass(), w, 100, 0.0, 500.0, "Mass_{softdrop} GeV");
    FillHist(plot_dir + region + "/AK8J_tau21", fatjet.PuppiTau21(), w, 200, 0.0, 1.0, "#tau_{21}");
  }

    // Other calculations for particles, DeltaR, and DeltaPhi
  Particle WCand = fatjets[0];
  Particle NCand = (leps[1]->Pt() < leps[0]->Pt()) ? N2Cand : N1Cand;
  Lepton Nlep = (leps[1]->Pt() < 100.) ? *leps[1] : *leps[0];
  Lepton Wlep = (leps[1]->Pt() > 100.) ? *leps[1] : *leps[0];
  
  for(unsigned int ij =0; ij < jets.size(); ij++){
   FillHist( plot_dir+region+ "/AK8J_DeltaR/AK8J_AK4J",   fatjets[0].DeltaR(jets[ij]),  w, 50, 0, 5, "#DeltaR (WAK8,j)");    
  }
  
  
  for(auto ijet : jets){
    FillHist( plot_dir+ region + "/AK8Jets/CHFracCJ"      , ijet.ChargedHadEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( plot_dir+ region + "/AK8Jets/NEMFracCJ"     , ijet.NeutralEmEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( plot_dir+ region + "/AK8Jets/CEMFracCJ"     , ijet.ChargedEmEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( plot_dir+ region + "/AK8Jets/NFracCJ"       , ijet.NeutralHadEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( plot_dir+ region + "/AK8Jets/MuonEnergyFraction", ijet.MuonEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( plot_dir+ region + "/AK8Jets/NVtxTracks", ijet.NVtxTracks(), w, 50, 0.0, 50, "");
    FillHist( plot_dir+ region + "/AK8Jets/Multiplicity", ijet.NMult() + ijet.CHMult(),w, 50, 0.0, 50, "");
    if(fabs(ijet.Eta()) < 2.4) FillHist( plot_dir+ region + "/AK8Jets/PileupJetId_Central",ijet.PileupJetId() , w, 100, 0.0, 1.0, "");
    else FillHist( plot_dir+ region + "/AK8Jets/PileupJetId_Endcap",ijet.PileupJetId() , w, 100, 0.0, 1.0, "");
    
  }


  
  return;
}

bool  HNL_LeptonCore::RunPlotter(AnalyzerParameter& param,TString label ){

  if (Analyzer == "HNL_SignalRegion_TestRun")  return false;
  
  if (label == "Standard") {

    /// Only run main nuisance but not if running runsyst
    if (HasFlag("RunSyst")) return false;
    //  if(!DrawSyst(param)) return false;
    return true;
  }
  if (label == "Main") {
    //if(!DrawSyst(param)) return false;
    return true;
  }

  if (HasFlag("RunSyst")) return false;
  return false;

}

void HNL_LeptonCore::Fill_RegionPlots(AnalyzerParameter& param, TString plot_dir, vector<Tau>& taus,   std::vector<Jet>& jets,    std::vector<FatJet>& fatjets, std::vector<Lepton *>& leps , Particle&  met, double nvtx,  double w){

  if(!param.runPlotter)  return;
  
  // Initial region plots
  Fill_RegionPlotsFull(param, plot_dir, taus, jets, fatjets, leps, met, nvtx, w);
  
  // Only do charge-separated plots for certain user/configs
  if (!User("jalmond")) return;
  return;

  bool doChargeSplit =
      (RunCF && leps.size() == 2 && !SameCharge(leps)) ||
      (!RunCF && SameCharge(leps));

  if (!doChargeSplit) return;

  // Backup param.Name and mutate safely
  TString baseName = param.Name;

  param.Name = baseName + (leps[0]->Charge() > 0 ? "_q_plus" : "_q_minus");
  Fill_RegionPlotsFull(param, plot_dir, taus, jets, fatjets, leps, met, nvtx, w);

  // Restore original name
  param.Name = baseName;
}
 

void HNL_LeptonCore::Fill_RegionPlots(AnalyzerParameter& param, TString plot_dir,   std::vector<Jet>& jets,    std::vector<FatJet>& fatjets, std::vector<Lepton *>& leps , Particle&  met, double nvtx,  double w){


  if(!param.runPlotter)  return;

  std::vector<Tau> emptyTaus;
  Fill_RegionPlotsFull(param, plot_dir, emptyTaus, jets, fatjets, leps, met, nvtx, w);

  if (!User("jalmond")) return;

  return;
  
  bool doChargeSplit =
      (RunCF && leps.size() == 2 && !SameCharge(leps)) ||
      (!RunCF && SameCharge(leps));

  if (!doChargeSplit) return;

  TString baseName = param.Name;
  param.Name = baseName + (leps[0]->Charge() > 0 ? "_q_plus" : "_q_minus");

  Fill_RegionPlotsFull(param, plot_dir, emptyTaus, jets, fatjets, leps, met, nvtx, w);

  param.Name = baseName;
}


void HNL_LeptonCore::Fill_RegionPlotsFull(AnalyzerParameter& param, TString plot_dir, vector<Tau>& Taus,  std::vector<Jet>& jets,    std::vector<FatJet>& fatjets, std::vector<Lepton *>& leps , Particle&  met, double nvtx,  double w, int DrawConfig){

  TString region = "/" + param.Name + param.hprefix;
  TString regionL = "/" + param.NameInclusive_Channel + param.hprefix;

  if (_jentry < 1) {
    std::cout << "[Plotting] plot_dir: " << plot_dir
              << ", region: " << region
              << ", regionL: " << regionL << std::endl;
  }

  std::vector<TString> regions;
  regions.push_back(region);

  //// If name has _q_ then plots are for charge split, so do not draw Inclusivce lepton as it double counts
  if(!param.Name.Contains("_q_")){
    if (!param.NameInclusive_Channel.IsNull() && !param.NameInclusive_Channel.IsWhitespace())
      regions.push_back(regionL);
  }
    
  for (const TString& r : regions) {
    Fill_Main_Plots(param, r, plot_dir, Taus, jets, fatjets, leps, met, nvtx, w);
    Fill_Standard_Plots(param, r, plot_dir, Taus, jets, fatjets, leps, met, nvtx, w);
    if (param.syst_ == AnalyzerParameter::Syst::Central) Fill_Plots(param, r, plot_dir, Taus, jets, fatjets, leps, met, nvtx, w);
  }
}
  

void HNL_LeptonCore::Fill_Standard_Plots(AnalyzerParameter& param, TString  region,  TString plot_dir,
             vector<Tau>& TauColl,  std::vector<Jet>& jets, std::vector<FatJet>& fatjets, std::vector<Lepton *>& leps ,
             Particle&  met, double nvtx,  double w){

  //// Draw stndard plots, not Limit setting  variables, so only default Systs ran
  if (!RunPlotter(param,"Standard")) return;
  
  if(leps.size() < 2) return;

  FillHist( plot_dir+ region+ "/Standard/N_AK4J",    jets.size() , w, 10,  0.0, 10.0, "N_{AK4 jets}");
  FillHist( plot_dir+ region+ "/Standard/Ev_MET",    met.Pt()    , w, 200, 0.0, 400.0,"MET GeV");

  Particle llCand = *leps[0] + *leps[1];
  FillHist( plot_dir+ region+ "/Standard/M_ll", llCand.M() , w, 400, 0.0, 2000.0, "M_{ll} GeV");


  /// Add User specific plots
  
  if(!User("jalmond")) return;

  //// Binned plots , these are duplicate of other plots but used to check without need to rebin in macro

  int nPtbins=15;
  double Ptbins[nPtbins+1] = { 0,20.0,25.0,30.0, 40.0,50.0, 70.0, 100.0,  150.0,  200.0,350,500.0, 750,1000,1500,2000};

  for(auto il : leps){
    FillHist( plot_dir+ region+ "/Standard/Lepton_pt", il->Pt(), w, nPtbins,Ptbins,"1_{2} p_{T} GeV");
    FillHist( plot_dir+ region+ "/Standard/Lepton_eta",il->Eta()  , w, 50, -2.5, 2.5,"l_{1} #eta");
    FillHist( plot_dir+ region+ "/Standard/Lepton_phi",il->Phi()  , w, 50, -2.5, 2.5,"l_{1} #phi");
  }

  if(jets.size() > 1){

    double dijetmass_tmp=999.;
    double dijetmass=9990000.;
    int m=-999;
    int n=-999;

    for(UInt_t emme=0; emme<jets.size(); emme++){
      for(UInt_t enne=1; enne<jets.size(); enne++) {
        if(emme == enne) continue;
        dijetmass_tmp = (jets[emme]+jets[enne]).M();

        if ( fabs(dijetmass_tmp-80.4) < fabs(dijetmass-80.4) ) {
          dijetmass = dijetmass_tmp;
          m = emme;
          n = enne;
        }
      }
    }
    Particle N1Cand  = jets[m]+jets[n]+ *leps[0] ;
    Particle N2Cand  = jets[m]+jets[n]+ *leps[1] ;
    int nSRbins=12;
    double mljbins[nSRbins] = { 0.0, 100.0,200.0, 400.0,450,500,550,600,700,800,  1000.0,3000.};
    double MN1  = (N1Cand.M() > 2500.) ? 2499. : N1Cand.M();
    double MN2  = (N2Cand.M() > 2500.) ? 2499. : N2Cand.M();

    FillHist( plot_dir+ region+ "/Standard/DiJet_M_l1W",      MN1,        w, 11, mljbins , "Reco M_{l1jj}");
    FillHist( plot_dir+ region+ "/Standard/DiJet_M_l2W",      MN2,        w, 11, mljbins , "Reco M_{l2jj} ");

  }

  return;

}

void HNL_LeptonCore::Fill_Main_Plots(AnalyzerParameter& param, TString  region,  TString plot_dir,
                                vector<Tau>& TauColl,  std::vector<Jet>& jets, std::vector<FatJet>& fatjets, std::vector<Lepton *>& leps ,
                                Particle&  met, double nvtx,  double w){

  if (!RunPlotter(param,"Main")) return;
  
  if(leps.size() < 2) return;

  double ST      = GetST(leps, jets, fatjets, met);
  double met2_st = pow(met.Pt(),2.)/ ST;

  FillHist( plot_dir+ region+ "/MainPlots/Ev_MET2_ST", met2_st  , w, 1000, 0.0, 100.0,"MET2/ST GeV");
  
  if(fatjets.size() > 0){
    Particle N1Cand  = fatjets[0] + *leps[0] ;
    FillHist( plot_dir+ region+ "/MainPlots/M_l1J",          N1Cand.M(),       w, 9999, 0, 9999, "M_{Jl_{1}} GeV" );
  }
  else{

    FillHist( plot_dir+ region+ "/MainPlots/HT_PT1",     leps[0]->HTOverPt(),     w, 100, 0, 10, "H_{T}/p_{T}");
    double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );

    //    if(User("jalmond"))  {
    //   FillHist( plot_dir+ region+ "/MainPlots/SR2_Scan",  min(9.0,leps[0]->HTOverPt()), ll_dphi, min(199.0,(*leps[1]).Pt()), w, 100, 0, 10,50, 0, 5.0, 20, 0, 200);
    // }
    
    
    if(ll_dphi > 2.)     FillHist( plot_dir+ region+ "/MainPlots/HT_PT1_HighDphi",     leps[0]->HTOverPt(),     w, 100, 0, 10, "H_{T}/p_{T}");
    else     FillHist( plot_dir+ region+ "/MainPlots/HT_PT1_LowDPhi",     leps[0]->HTOverPt(),     w, 100, 0, 10, "H_{T}/p_{T}");

  }

  double PTLep1  = leps[0]->Pt();
  double PTLep2  = leps[1]->Pt();
  double LT = PTLep1 + PTLep2;
  
  FillHist( plot_dir + region + "/MainPlots/Lepton_1_pt", PTLep1, w, 9999, 0, 9999, "l_{1} p_{T} GeV");
  FillHist( plot_dir + region + "/MainPlots/Lepton_2_pt", PTLep2, w, 9999, 0, 9999, "l_{2} p_{T} GeV");
  if(leps.size() > 2) FillHist( plot_dir + region + "/MainPlots/Lepton_3_pt", leps[2]->Pt(), w, 9999, 0, 9999, "l_{3} p_{T} GeV");
  if(leps.size() > 3) FillHist( plot_dir + region + "/MainPlots/Lepton_4_pt", leps[3]->Pt(), w, 9999, 0, 9999, "l_{4} p_{T} GeV");
  
  FillHist( plot_dir + region + "/MainPlots/Lepton_pt", PTLep1, w, 9999, 0, 9999, "l p_{T} GeV");
  FillHist( plot_dir + region + "/MainPlots/Lepton_pt", PTLep2, w, 9999, 0, 9999, "l p_{T} GeV");
  if(leps.size() > 2) FillHist( plot_dir + region + "/MainPlots/Lepton_pt", leps[2]->Pt(), w, 9999, 0, 9999, "l p_{T} GeV");
  if(leps.size() > 3) FillHist( plot_dir + region + "/MainPlots/Lepton_pt", leps[3]->Pt(), w, 9999, 0, 9999, "l p_{T} GeV");

  if(leps.size() > 2)  LT+= leps[2]->Pt();
  if(leps.size() > 3)  LT+= leps[3]->Pt();
  FillHist( plot_dir + region + "/MainPlots/L_T", LT, w, 9999, 0, 9999, "l_{T} p_{T} GeV");

  if(User("jalmond"))  {
    double NJ = 0.;
    if(jets.size() > 1) NJ = 1.;
    FillHist( plot_dir+ region+ "/MainPlots/SR3_Scan",  min(1199.0,LT), min(9.0,met2_st), NJ, w, 120, 0, 1200,18, 0, 9.0, 2, 0, 2);
  }
  return;

}

void HNL_LeptonCore::Fill_Plots(AnalyzerParameter& param, TString  region,  TString plot_dir, 
        vector<Tau>& TauColl,  std::vector<Jet>& jets, std::vector<FatJet>& fatjets, std::vector<Lepton *>& leps , 
        Particle&  met, double nvtx,  double w){

  if (!RunPlotter(param,"Fill_Plots")) return;

  
  TString regionAK8 = region + "/AK8";
  TString lepregion = region + "/LeptonMVA";

  // Check if the lepton count matches the channel type                                                                                                                                                             
  // Check if the lepton count matches the channel type
  std::map<int, std::string> channelTypeMap = {{2, "Dilepton"}, {3, "Trilepton"}, {4, "Quadlepton"}};
  if(param.ChannelType() != "Lepton"){
    if(param.ChannelType() != channelTypeMap[leps.size()]) return;
  }
  bool threelep = (leps.size()  == 3);
  bool fourlep  = (leps.size()  == 4);
  
  int nel(0), nmu(0);
  int sumQ=0;
  for(auto ilep: leps) {
    sumQ += ilep->Charge();
    if(ilep->LeptonFlavour() == Lepton::ELECTRON) nel++;
    if(ilep->LeptonFlavour() == Lepton::MUON) nmu++;
  }
    
  //// Run AK8 Plots
  if(fatjets.size() > 0) Fill_PlotsAK8(param, regionAK8, plot_dir,TauColl,jets , fatjets, leps, met, nvtx,w);

  vector<Photon> phs = GetAllPhotons();
  vector<Photon> phs_loose;
  int n_medium_ph =0;
  int n_loose_ph =0;
  for(auto iph : phs) {
    if(iph.Pt() < 10) continue;
    if(fabs(iph.Eta()) > 2.4) continue;
  
    if(iph.passLooseID()) n_loose_ph++;
    if(iph.passMVAID_WP80()){
      n_medium_ph++;
      phs_loose.push_back(iph);
    }
  }
  
  
  ///// Run resolved pliots
  FillHist( plot_dir+ region+ "/NObj/N_El", nel,  w, 5, 0, 5, "El size");
  FillHist( plot_dir+ region+ "/NObj/N_Mu", nmu,  w, 5, 0, 5, "Mu size");
  FillHist( plot_dir+ region+ "/NObj/N_tau", TauColl.size(),  w, 5, 0, 5, "Tau size");
  FillHist( plot_dir+ region+ "/NObj/N_ak4jet", jets.size(),  w, 10, 0, 10, "AK4 size");
  FillHist( plot_dir+ region+ "/NObj/N_ak8jet", fatjets.size(),  w, 4, 0, 4, "AK8 size");

  //// Check Loose AK8 Count
  std::vector<FatJet>   AK8_Loose_JetColl  = SelectFatJets(param, param.FatJet_ID, param.FatJet_MinPt, 2.7);
  FillHist( plot_dir+ region+ "/NObj/N_ak8_loose_jet", AK8_Loose_JetColl.size(),  w, 4, 0, 4, "AK8 size");
  for(auto iak8_loose : AK8_Loose_JetColl){
    for(auto ilep : leps)   FillHist( plot_dir+region+ "/AK8J_DeltaR/AK8LooseJ_lep",  ilep->DeltaR(iak8_loose) ,  w, 50, 0, 5, "#DeltaR (WAK8,j)");

    for(auto iak8 : fatjets){
      if(iak8.Pt() == iak8_loose.Pt()) continue;
      FillHist( plot_dir+region+ "/AK8J_DeltaR/AK8J_AK8LooseJ",   iak8.DeltaR(iak8_loose),  w, 50, 0, 5, "#DeltaR (WAK8,j)");
    }     
  }
  
  if(leps.size() < 2) return;

  //  bool PlotZZ = MCSample.Contains("ZZ_");
  if(MCSample.Contains("SSWWType")){
    Gen Lep1;
    Gen Lep2;
    Gen j1,j2;
    bool j1IsSet(false);
    bool l1IsSet(false);
    
    TString LepFl_l1, LepFl_l2;
    int Lep_Mother_ind(-1);
    
    for(unsigned int i=2; i<All_Gens.size(); i++){
      Gen gen = All_Gens.at(i);
      if( ! ( ( fabs(gen.PID()) == 13)  || (fabs(gen.PID()) == 11) )) continue;
      if (gen.Status() == 23){
  TString LepFl = (fabs(gen.PID()) == 13) ? "Mu" : "El";
  
  if(!l1IsSet) {  Lep1= gen; l1IsSet=true;LepFl_l1= LepFl;}
  else { Lep2 = gen; LepFl_l2= LepFl;}
  Lep_Mother_ind = gen.MotherIndex();
      }
    }
    
    for(unsigned int i=2; i<All_Gens.size(); i++){
      Gen gen = All_Gens.at(i);
      
      if (gen.MotherIndex() == Lep_Mother_ind){
  if(fabs(gen.PID()) > 6) continue;
  if(!j1IsSet) {  j1= gen; j1IsSet=true;}
  else j2 = gen;
      }
    }
    FillHist( plot_dir+ region+ "/SignalProcess/Dijet_mass",  (j1+j2).M(),  1.0, 50, 0.0, 5000.0,"m(jj) GeV");

    std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets("VBFLoose",  param);
    Particle LL =  *leps[0] + *leps[1] ;
    double HT_VBF = GetHT(AK4_VBF_JetColl,{});
    Particle PuppiMETvULPhiCorr = GetMiniAODvMET("PuppiT1xyULCorr");
    if((j1+j2).M() < 700){
      FillHist( plot_dir+ region+ "/SignalProcess_LowMass/MET", PuppiMETvULPhiCorr.Pt() ,  1.0, 100, 0.0, 200.0,"");
      FillHist( plot_dir+ region+ "/SignalProcess_LowMass/HT", HT_VBF ,  1.0, 200, 0.0, 5000.0,"");
      if(AK4_VBF_JetColl.size() == 1) {
  FillHist( plot_dir+ region+ "/SignalProcess_LowMass/DR_rj_l1", AK4_VBF_JetColl[0].DeltaR(*leps[0]) ,  1.0, 100, 0.0, 10.0,"");
  FillHist( plot_dir+ region+ "/SignalProcess_LowMass/DR_rj_l2", AK4_VBF_JetColl[0].DeltaR(*leps[1]) ,  1.0, 100, 0.0, 10.0,"");
      }
      else  if(AK4_VBF_JetColl.size() > 1) {
        FillHist( plot_dir+ region+ "/SignalProcess_LowMass/DR_rj_l1", AK4_VBF_JetColl[0].DeltaR(*leps[0]) ,  1.0, 100, 0.0, 10.0,"");
        FillHist( plot_dir+ region+ "/SignalProcess_LowMass/DR_rj_l2", AK4_VBF_JetColl[0].DeltaR(*leps[1]) ,  1.0, 100, 0.0, 10.0,"");
  FillHist( plot_dir+ region+ "/SignalProcess_LowMass/DR_j1_j2", AK4_VBF_JetColl[0].DeltaR(AK4_VBF_JetColl[1]) ,  1.0, 100, 0.0, 10.0,"");
      }
      FillHist( plot_dir+ region+ "/SignalProcess_LowMass/drll", leps[0]->DeltaR(*leps[1]) ,  1.0, 100, 0.0, 10,"");

      FillHist( plot_dir+ region+ "/SignalProcess_LowMass/Dijet_mass",  (j1+j2).M(),  1.0, 50, 0.0, 5000.0,"m(jj) GeV");
      FillHist( plot_dir+ region+ "/SignalProcess_LowMass/DEta", fabs(j1.Eta() - j2.Eta()) ,  1.0, 100, -20.0, 20.0,"");
      FillHist( plot_dir+ region+ "/SignalProcess_LowMass/DR", j1.DeltaR(j2) ,  1.0, 100, 0.0, 10.0,"");
      FillHist( plot_dir+ region+ "/SignalProcess_LowMass/J1_Pt", j1.Pt() ,  1.0, 500, 0.0, 2000.0,"");
      FillHist( plot_dir+ region+ "/SignalProcess_LowMass/J2_Pt", j2.Pt() ,  1.0, 500, 0.0, 2000.0,"");
      FillHist( plot_dir+ region+ "/SignalProcess_LowMass/MLL", LL.M() ,  1.0, 100, 0.0, 1000.0,"");
      //      FillHist( plot_dir+ region+ "/SignalProcess_LowMass/", LL.M() ,  1.0, 100, 0.0, 1000.0,"");

      //cout << "Low Mass  (j1+j2).M() = " << (j1+j2).M()   << " LL mass =  " << LL.M() << " dR ll = " << leps[0]->DeltaR(*leps[1]) << endl;
      
  //for(auto i : AK4_VBF_JetColl ) {
  //  for(auto ilep : leps) cout << "dr = " << ilep->DeltaR(i) << endl;
  //  }
  
  /*cout << "Low Mass pt j1 = " ;
  j1.Print()  ;
  cout << "Low Mass pt j2 = " ;
  j2.Print() ;
  cout << "Low Mass deta = " << fabs(j1.Eta() - j2.Eta()) << endl;
  cout << "LL mass =  " << LL.M() << " dR ll = " << leps[0]->DeltaR(*leps[1]) << endl;*/
    }
    else{

      FillHist( plot_dir+ region+ "/SignalProcess_HighMass/MET", PuppiMETvULPhiCorr.Pt() ,  1.0, 100, 0.0, 200.0,"");
      FillHist( plot_dir+ region+ "/SignalProcess_HighMass/HT", HT_VBF ,  1.0, 200, 0.0, 5000.0,"");

      if(AK4_VBF_JetColl.size() == 1) {
        FillHist( plot_dir+ region+ "/SignalProcess_HighMass/DR_rj_l1", AK4_VBF_JetColl[0].DeltaR(*leps[0]) ,  1.0, 100, 0.0, 10.0,"");
        FillHist( plot_dir+ region+ "/SignalProcess_HighMass/DR_rj_l2", AK4_VBF_JetColl[0].DeltaR(*leps[1]) ,  1.0, 100, 0.0, 10.0,"");
      }
      else  if(AK4_VBF_JetColl.size() > 1) {
        FillHist( plot_dir+ region+ "/SignalProcess_HighMass/DR_rj_l1", AK4_VBF_JetColl[0].DeltaR(*leps[0]) ,  1.0, 100, 0.0, 10.0,"");
        FillHist( plot_dir+ region+ "/SignalProcess_HighMass/DR_rj_l2", AK4_VBF_JetColl[0].DeltaR(*leps[1]) ,  1.0, 100, 0.0, 10.0,"");
  FillHist( plot_dir+ region+ "/SignalProcess_HighMass/DR_j1_j2", AK4_VBF_JetColl[0].DeltaR(AK4_VBF_JetColl[1]) ,  1.0, 100, 0.0, 10.0,"");
      }

      FillHist( plot_dir+ region+ "/SignalProcess_HighMass/drll", leps[0]->DeltaR(*leps[1]) ,  1.0, 100, 0.0, 10.0,"");

      FillHist( plot_dir+ region+ "/SignalProcess_HighMass/Dijet_mass",  (j1+j2).M(),  1.0, 50, 0.0, 5000.0,"m(jj) GeV");
      FillHist( plot_dir+ region+ "/SignalProcess_HighMass/DEta", fabs(j1.Eta() - j2.Eta()) ,  1.0, 100, -20.0, 20.0,"");
      FillHist( plot_dir+ region+ "/SignalProcess_HighMass/DR", j1.DeltaR(j2) ,  1.0, 100, 0.0, 10.0,"");
      FillHist( plot_dir+ region+ "/SignalProcess_HighMass/J1_Pt", j1.Pt() ,  1.0, 500, 0.0, 2000.0,"");
      FillHist( plot_dir+ region+ "/SignalProcess_HighMass/J2_Pt", j2.Pt() ,  1.0, 500, 0.0, 2000.0,"");
      FillHist( plot_dir+ region+ "/SignalProcess_HighMass/MLL", LL.M() ,  1.0, 100, 0.0, 1000.0,"");
      
      
      //cout << "High Mass  (j1+j2).M() = " << (j1+j2).M()   << " LL mass =  " << LL.M() << " dR ll = " << leps[0]->DeltaR(*leps[1]) << endl;
      
      //for(auto i : AK4_VBF_JetColl ) {
      //    for(auto ilep : leps) cout << "HM dr = " << ilep->DeltaR(i) << endl;
      //        }
      
      /*  cout << "High Mass pt j1 = " ;
    j1.Print() ;
    cout << "High Mass pt j2 = " ;
    j2.Print();
  cout << "High Mass deta = " << fabs(j1.Eta() - j2.Eta()) << endl;
  cout <<  "LL mass =  " << LL.M() << " dR ll = " << leps[0]->DeltaR(*leps[1]) << endl;*/
      

    }
  
      
    if(AK4_VBF_JetColl.size() < 2)     FillHist( plot_dir+ region+ "/SignalProcess/Dijet_mass_01J",  (j1+j2).M(),  1.0, 50, 0.0, 5000.0,"m(jj) GeV");
    
    
  }
  if(HasFlag("PlotZZ")){
    Particle ZZ;

    vector<int> MotherPID;
    for (auto i : leps) {
      int Idx_Closest = GenMatchedIdx(*i, All_Gens);
    
      // Ensure the index is valid
      if (Idx_Closest < 0 || static_cast<size_t>(Idx_Closest) >= All_Gens.size()) continue;

      Gen MatchedGen = All_Gens.at(Idx_Closest);
      //      std::cout << "MatchedGen " << Idx_Closest << " gen.Status = " << MatchedGen.Status()//
      //    << " PID = " << MatchedGen.PID() << std::endl;

      int Idx_Closest_Mother = Idx_Closest;

      // Check that MotherIndex is within bounds before accessing
      while (Idx_Closest_Mother >= 0 &&
       ( std::abs(All_Gens.at(Idx_Closest_Mother).PID()) == 11 || 
         std::abs(All_Gens.at(Idx_Closest_Mother).PID()) == 13 ) && 
       static_cast<size_t>(Idx_Closest_Mother) < All_Gens.size() &&
       All_Gens.at(Idx_Closest_Mother).MotherIndex() >= 0 &&
       static_cast<size_t>(All_Gens.at(Idx_Closest_Mother).MotherIndex()) < All_Gens.size()) {
        

        Idx_Closest_Mother = All_Gens.at(Idx_Closest_Mother).MotherIndex();
        int motherPID = std::abs(All_Gens.at(Idx_Closest_Mother).PID());
  //  std::cout << "fabs(All_Gens[All_Gens[" << Idx_Closest_Mother
  //        << "].MotherIndex()].PID()) = " << motherPID << std::endl;

        if (!( motherPID == 11 || motherPID == 13)) break;
        
      }
      
      //cout << "Mother PID = " << std::abs(All_Gens.at(Idx_Closest_Mother).PID())  << endl;
      if (std::find(MotherPID.begin(), MotherPID.end(), Idx_Closest_Mother) == MotherPID.end()) {
  MotherPID.push_back(All_Gens.at(Idx_Closest_Mother).PID());
      }      
    }
    if(MotherPID.size() != 2 || 
       ( abs(MotherPID[0]) != 23 || abs(MotherPID[1]) != 23)) {
      //      PrintGen(All_Gens);

      //      for (auto i : MotherPID) cout << "PID " <<  i << endl;
      //      for (auto i : leps)  PrintMatchedGen(All_Gens,*i);
    }
    for(auto ip : MotherPID) {
      FillHist( plot_dir+ region+ "/GenZZ/ZMass", All_Gens[ip].M(),  w, 500, 0, 500, "M_{ZZ} GeV");
    }

    
    for(int i=2; i<int(All_Gens.size()); i++){
      Gen gen = All_Gens.at(i);
      if(gen.Status()==1){
  if(abs(gen.PID()) == 13 || abs(gen.PID()) == 11){
    ZZ=ZZ+gen;
  }
      }
    }
    FillHist( plot_dir+ region+ "/GenZZ/ZZMass", ZZ.M(),  w, 500, 0, 1000, "M_{ZZ} GeV");
    FillHist( plot_dir+ region+ "/GenZZ/ZZPt", ZZ.Pt(),  w, 500, 0, 1000, "M_{ZZ} GeV");
    FillHist( plot_dir+ region+ "/GenZZ/ZZMassV2", GetGenZZMass(),  w, 500, 0, 1000, "M_{ZZ} GeV");
    FillHist( plot_dir+ region+ "/GenZZ/ZZPtV2", GetGenZZPt(),  w, 500, 0, 1000, "M_{ZZ} GeV");

  }

  //// Plot properties of extra lepton

  if(leps.size() > 1){
    
    int n_nonmatched_mu=0;
    for(auto imuon : All_Muons){
      if(!imuon.isPOGLoose()) continue;
      if(imuon.Pt() < 5.0) continue;
      
      bool matched=false;
      for(auto i : leps){
  if(i->DeltaR(imuon) <  0.1) {
    matched=true;
  }
      }
      if(!matched) n_nonmatched_mu++;
    }
    
    FillHist( plot_dir+ region + "/ExtraLep/All_Muon_NonMatched"    , n_nonmatched_mu, w, 10, 0.0, 10.0, "");
    
    int n_nonmatched_el_mva=0;
    int n_nonmatched_el_cbveto=0;
    
    for(auto iel : All_Electrons){
      if(iel.Pt() < 10)  continue;
      bool matched=false;
      
      for(auto i : leps){
  if(i->DeltaR(iel) <  0.1) {
    matched=true;
  }
      }
      if(!matched){
  if(iel.passMVAID_noiso_WPLoose()) n_nonmatched_el_mva++;
  if(iel.passVetoID()) n_nonmatched_el_cbveto++;
  
      }
    }   
    FillHist( plot_dir+ region + "/ExtraLep/All_El_NonMatched_MVA"  , n_nonmatched_el_mva, w, 10, 0.0, 10.0, "");
    FillHist( plot_dir+ region + "/ExtraLep/All_El_NonMatched_CBVeto" ,n_nonmatched_el_cbveto , w, 10, 0.0, 10.0, "");
    
    int n_nonnmatched_tau=0;
    for(auto itau : GetAllTaus()){
      if(!itau.PassID("JetVL_MuVL_ELVL"))continue;
      if(itau.Pt() < 20) continue;
      if(fabs(itau.Eta()) > 2.3) continue;
      
      bool matched=false;
      for(auto i : leps){
  if(i->DeltaR(itau) <  0.1) {
    matched=true;
  }
      }
      if(!matched) n_nonnmatched_tau++;           
    }
    FillHist( plot_dir+ region + "/ExtraLep/All_Tau_NonMatched"    , n_nonnmatched_tau, w, 10, 0.0, 10.0, "");
  }
  
  for(auto ijet : jets){
    FillHist( plot_dir+ region + "/Jets/CHFracCJ"      , ijet.ChargedHadEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( plot_dir+ region + "/Jets/NEMFracCJ"     , ijet.NeutralEmEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( plot_dir+ region + "/Jets/CEMFracCJ"     , ijet.ChargedEmEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( plot_dir+ region + "/Jets/NFracCJ"       , ijet.NeutralHadEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( plot_dir+ region + "/Jets/MuonEnergyFraction", ijet.MuonEnergyFraction(), w, 100, 0.0, 1.0, "");
    //FillHist( plot_dir+ region + "/Jets/NVtxTracks", ijet.NVtxTracks(), w, 50, 0.0, 50, "");
    FillHist( plot_dir+ region + "/Jets/Multiplicity", ijet.NMult() + ijet.CHMult(),w, 50, 0.0, 50, "");
    FillHist( plot_dir+ region + "/Jets/NMult", ijet.NMult(),w, 35, 0.0, 35, "");
    FillHist( plot_dir+ region + "/Jets/CHMult", ijet.CHMult(),w, 35, 0.0, 35, "");
    if(fabs(ijet.Eta()) < 2.4) FillHist( plot_dir+ region + "/Jets/PileupJetId_Central",ijet.PileupJetId() , w, 100, -1.0, 1.0, "");
    else if(fabs(ijet.Eta()) < 3.) FillHist( plot_dir+ region + "/Jets/PileupJetId_Endcap",ijet.PileupJetId() , w, 100, -1.0, 1.0, "");
    else FillHist( plot_dir+ region + "/Jets/PileupJetId_Forward",ijet.PileupJetId() , w, 100, -1.0, 1.0, "");
  }
    
  FillHist( plot_dir+ region+ "/Leptons/SumQ", sumQ,  w, 10, -5, 5, "Q size");

  //// Lepton plots 
  Particle llCand = *leps[0] + *leps[1];
  FillHist( plot_dir+ region+ "/DiLepton/ll_Pt",  llCand.Pt()  , w, 2000, 0.0, 2000.0, "Z pt GeV");
  double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );
  double ll_deta = fabs((*leps[0]).Eta() - (*leps[1]).Eta());
  FillHist( plot_dir+ region+ "/DeltaPhi/dPhi_lep1_lep2", ll_dphi  , w, 63, 0, 3.15, "#Delta #Phi(l1,l2)") ;
  FillHist( plot_dir+ region+ "/DeltaEta/dEta_lep1_lep2", ll_deta  , w, 200, -5.0, 5.0, "#Delta #Phi(l1,l2)") ;
  FillHist( plot_dir+ region+"/DeltaR/dR_ll", leps[0]->DeltaR(*leps[1] ) ,w, 100,  0.0, 10,"#DeltaR(l,l)");
  
  FillHist( plot_dir+ region+ "/Leptons/Lep_r_pt",  leps[1]->Pt()/leps[0]->Pt()  ,  w, 200, 0, 1,"l_{1} p_{T} GeV");
  FillHist( plot_dir+ region+ "/Leptons/Lep_1_pt",  leps[0]->Pt()  ,  w, 200, 0, 2000,"l_{1} p_{T} GeV");
  FillHist( plot_dir+ region+ "/Leptons/Lep_2_pt",  leps[1]->Pt()  ,  w, 200, 0, 1000,"1_{2} p_{T} GeV");
  FillHist( plot_dir+ region+ "/Leptons/Lep_1_eta", leps[0]->Eta()  , w, 60, -3.0, 3,"l_{1} #eta");
  FillHist( plot_dir+ region+ "/Leptons/Lep_2_eta", leps[1]->Eta()  , w, 60, -3.0, 3.0,"l_{2} #eta");
  FillHist( plot_dir+ region+ "/Leptons/Lep_1_phi", leps[0]->Phi()  , w, 200, -10, 10.0,"l_{3} #phi");
  FillHist( plot_dir+ region+ "/Leptons/Lep_2_phi", leps[1]->Phi()  , w, 200, -10, 10.0,"l_{3} #phi");

  int nHTPTbins=5;
  double HTPTbins[nHTPTbins+1] = {0,1, 2,4,6,10};

  //// Event plots
  //FillHist( plot_dir+ region+ "/SKEvent/HToLepPt1", leps[0]->HTOverPt()  , w, nHTPTbins,HTPTbins,"HT/PT(1)");
  FillHist( plot_dir+ region+ "/SKEvent/HToLepPt1", leps[0]->HTOverPt()  , w, 20,0,10,"H_{T}/p_{T}(l_{1})");
  FillHist( plot_dir+ region+ "/SKEvent/Mt_lep1", MT(*leps[0] ,met)  , w, 200, 0.0, 400.0,"MT GeV");
  
  
  //// 3l + 4l plots
  
  if(threelep) {
    Particle lllCand = *leps[0] + *leps[1] + *leps[2] ;

    FillHist( plot_dir+ region+ "/Leptons/Lep_3_eta", leps[2]->Eta()  , w, 60, -3.0, 3.0,"l_{3} #eta");
    FillHist( plot_dir+ region+ "/Leptons/Lep_3_phi", leps[2]->Phi()  , w, 200, -10, 10.0,"l_{3} #phi");
    FillHist( plot_dir+ region+ "/Leptons/Lep_3_pt",  leps[2]->Pt()  ,  w, 200, 0, 1000,"1_{2} p_{T} GeV");

    FillHist( plot_dir+ region+ "/Mass/M_lll", lllCand.M() , w, 200, 0.0, 800.0,"M(lll) GeV");
    if(GetIndexNonMinOSSF(leps) > 0 )FillHist( plot_dir+ region+ "/SKEvent/Mt_minOSSF", MT(*leps[GetIndexNonMinOSSF(leps)], met) , w, 200, 0.0, 800.0,"M(T) non MinOS GeV");
    FillHist( plot_dir+ region+ "/Mass/M_minOSSF", GetMassMinOSSF(leps), w, 200, 0.0, 800.0,"M non MinOS GeV");
    FillHist( plot_dir+ region+ "/Mass/M_minSSSF", GetMassMinSSSF(leps), w, 200, 0.0, 800.0,"M(T) non MinSS GeV");
    if(GetIndexNonMinSSSF(leps) > 0 )FillHist( plot_dir+ region+ "/Mass/Mt_minSSSF", MT(*leps[GetIndexNonMinSSSF(leps)], met) , w, 200, 0.0, 800.0,"M non MinSS GeV");
  }
  
  if(fourlep) {
    Particle llllCand = *leps[0] + *leps[1] + *leps[2] + *leps[3] ;
    if(llllCand.M() > 70 && llllCand.M() < 170)     FillHist( plot_dir+ region+ "/Mass/M_ZZ", llllCand.M() , w, 100, 70., 170.,"M_{4l} (GeV)");
    //FillHist( plot_dir+ region+ "/Mass/M_llll_gen", GetGenZZMass(), w, 200, 0., 800.,"Pt(llll) GeV");
    //FillHist( plot_dir+ region+ "/Pt/M_llll_gen", GetGenZZPt(), w, 200, 0., 800.,"M(llll) GeV");
    FillHist( plot_dir+ region+ "/Mass/M_llll", llllCand.M() , w, 200, 0., 800.,"M(llll) GeV");
    FillHist( plot_dir+ region+ "/Mass/M_BestZ", LeptonMassBestZ(leps,LeptonPairBestZCand(leps)) , w, 200, 0., 800.,"M(Z1) GeV");
    FillHist( plot_dir+ region+ "/Mass/M_OtherZ", LeptonMassNonZ(leps,LeptonPairBestZCand(leps)) , w, 200, 0., 800.,"M(Z1) GeV");
    FillHist( plot_dir+ region+ "/Mass/M_BestZAlt", GetMassBestZ(leps,true) , w, 200, 0., 800.,"M(Z1) GeV");

    FillHist( plot_dir+ region+ "/Leptons/Lep_3_pt", leps[2]->PtMaxed(1000.)  , w, 1000, 0., 1000.,"l_{3} p_{T} GeV");
    FillHist( plot_dir+ region+ "/Leptons/Lep_3_eta", leps[2]->Eta()  , w, 60, -3., 3.,"l_{3} #eta");
    FillHist( plot_dir+ region+ "/Leptons/Lep_3_phi", leps[2]->Phi()  , w, 200, -10, 10.,"l_{3} #phi");
    FillHist( plot_dir+ region+ "/Leptons/Lep_4_pt", leps[3]->PtMaxed(1000.)  , w, 1000, 0., 1000.,"l_{4} p_{T} GeV");
    FillHist( plot_dir+ region+ "/Leptons/Lep_4_eta", leps[3]->Eta()  , w, 60, -3., 3.,"l_{4} #eta");
    FillHist( plot_dir+ region+ "/Leptons/Lep_4_phi", leps[3]->Phi()  , w, 200, -10, 10.,"l_{4} #phi");
  }
  
  //// Jet plots                                                                                                                                                                                                  
  int nBJet=0;
  JetTagging::Parameters JPForPlots = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets);
  for(unsigned int i=0; i < jets.size(); i++){
    if(fabs(jets.at(i).Eta()) > 2.4) continue;
    if( jets[i].GetTaggerResult(JPForPlots.j_Tagger) > mcCorr->GetJetTaggingCutValue(JPForPlots.j_Tagger, JPForPlots.j_WP) ) nBJet++;
   
    FillHist( plot_dir+ region+ "/AK4Jets/Jet_pt",  jets[i].Pt() , w, 400, 0.0, 2000.0, "AK4 Jet p_{T} GeV");
    FillHist( plot_dir+ region+ "/AK4Jets/Jet_eta",  jets[i].Eta() , w, 100, -5.0, 5.0, "AK4 Jet #eta ");
  }
  FillHist( plot_dir+ region+ "/NObj/N_BJet",  nBJet , w, 5, 0., 5., "NBjet");
  FillHist( plot_dir+ region+ "/NObj/N_Jet",  jets.size() , w, 10, 0., 10., "NJet"); //JH
  
  
  if(jets.size() > 1){
    //// Check for Jet pair from N->Wl->jjl
    double dijetmass_tmp=999.;
    double dijetmass=9990000.;
    int m=-999;
    int n=-999;
    
    for(UInt_t emme=0; emme<jets.size(); emme++){
      for(UInt_t enne=1; enne<jets.size(); enne++) {
        if(emme == enne) continue;
        dijetmass_tmp = (jets[emme]+jets[enne]).M();
  
        if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
          dijetmass = dijetmass_tmp;
          m = emme;
          n = enne;
        }
      }
    }
    Particle W1Cand  = *leps[0]  + *leps[1]  + jets[m]+jets[n];
    Particle WCand   = jets[m]+jets[n];
    Particle N1Cand  = jets[m]+jets[n]+ *leps[0] ;
    Particle N2Cand  = jets[m]+jets[n]+ *leps[1] ;
    
    
    double MN1 = (N1Cand.M() > 5000.) ? 4999. : N1Cand.M();
    double MN2 = (N2Cand.M() > 5000.) ? 4999. : N2Cand.M();
    double MllW = (W1Cand.M() > 5000.) ? 4999. : W1Cand.M();

    /// lep+jet plots                                                                                                                                                                                               
    FillHist( plot_dir+ region+ "/Mass/DiJet_M_l1jj",   MN1,        w, 2500, 0, 5000, "Reco M_{l1jj}");
    FillHist( plot_dir+ region+ "/Mass/DiJet_M_l2jj",   MN2,        w, 2500, 0, 5000, "Reco M_{l2jj}");
    FillHist( plot_dir+ region+ "/Mass/DiJet_M_lljj",   MllW,        w, 2500, 0, 5000, "Reco M_{lljj}");
    //// Jet plots                                                                                                                                                                                                  
    FillHist(plot_dir+ region+ "/Mass/DiJet_M_W",   WCand.M(),  w, 50, 0, 500, "Reco M_{jj}");
  }
  

  if(jets.size() > 1){
    double maxDiJetDeta=0.;
    int ijet1(-1), ijet2(-1);
    for(unsigned int ij = 0; ij < jets.size()-1; ij++){
      for(unsigned int ij2 = ij+1; ij2 < jets.size(); ij2++){
  
        double deta = fabs(jets[ij].Eta() - jets[ij2].Eta());
        if(deta > maxDiJetDeta) {
          maxDiJetDeta=deta;
          ijet1=ij;
          ijet2=ij2;
        }
      }
    }

    int nVBFMJJbins=6;
    double MJJbins[nVBFMJJbins+1] = {0,300, 500,750,1200,1700,3000};

    FillHist( plot_dir+ region+ "/VBF/MaxDEta_jet1_jet2", maxDiJetDeta  , w, 200, 0.0, 10.0, "Max DEta");
    Particle JJMEta = jets[ijet1] + jets[ijet2];
    //FillHist( plot_dir+ region+ "/VBF/MaxDEtaJets_MJJ",JJMEta.M()   , w, nVBFMJJbins, MJJbins, "MaxDEta MJJ");
    FillHist( plot_dir+ region+ "/VBF/MaxDEtaJets_MJJ",JJMEta.M()   , w, 15, 0, 3000, "MaxDEta MJJ");
    double Av_JetEta= 0.5*(jets[ijet1].Eta()+ jets[ijet2].Eta());
    
    double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);
    FillHist( plot_dir+ region+ "/VBF/MaxDEta_zeppenfeld", zeppenfeld  , w, 200, 0.0, 10.0, "zeppenfeld");
  } 
  
  if(jets.size()>1){
    int nVBFMJJbins=6;
    double MJJbins[nVBFMJJbins+1] = {0,300, 500,750,1200,1700,3000};
    
    Particle JJLead = jets[0] + jets[1];
    double deta = fabs(jets[0].Eta() - jets[1].Eta());
    //FillHist( plot_dir+ region+ "/VBF/Lead_MJJ",JJLead.M()   , w, nVBFMJJbins, MJJbins, "Lead MJJ");
    FillHist( plot_dir+ region+ "/VBF/Lead_MJJ",JJLead.M()   , w, 15, 0, 3000, "Lead MJJ");
    FillHist( plot_dir+ region+ "/VBF/Lead_DEta", deta  , w, 200, 0.0, 10.0, "DEta JJ");
    double maxDiJetDeta=fabs(jets[0].Eta() - jets[1].Eta());
    double Av_JetEta= 0.5*(jets[0].Eta()+ jets[1].Eta());
    double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);
    FillHist( plot_dir+ region+ "/VBF/Lead_zeppenfeld", zeppenfeld  , w, 200, 0.0, 10.0, "zeppenfeld");
  }
  
  if(jets.size()>1){
    FillHist( plot_dir+ region+ "/Jets/Jet_1_pt",  jets[0].Pt() , w, 400, 0.0, 2000.0, "Jet_{1} p_{T} GeV");
    FillHist( plot_dir+ region+ "/Jets/Jet_1_eta",  jets[0].Eta() , w, 100, -5.0, 5.0, "Jet_{1} #eta ");
    FillHist( plot_dir+ region+ "/Jets/Jet_2_pt",  jets[1].Pt() , w, 400, 0.0, 2000.0, "Jet_{2} p_{T} GeV");
    FillHist( plot_dir+ region+ "/Jets/Jet_2_eta",  jets[1].Eta() , w, 100, -5.0, 5.0, "Jet_{2} #eta ");
  }
  
  //// Now draw detailed plots using flag
  //if(!HasFlag("Plots")) return;
  
  double HT = GetHT(jets, fatjets);
  FillHist( plot_dir+ region+ "/SKEvent/Ev_HT", HT, w, 3000, 0.0, 3000.0, "H_{T} GeV");
  
  FillHist( plot_dir+ region+ "/SKEvent/nPV",  nvtx , w, 120, 0.0, 120.);
  FillHist( plot_dir+ region+ "/SKEvent/nPileUp",  nPileUp, w, 120, 0.0, 120.);

  double ST = GetST(leps, jets, fatjets, met);
  double XT = ST - met.Pt();
  
  FillHist( plot_dir+ region+ "/SKEvent/Ev_ST", ST, w, 5000, 0.0, 5000.0, "S_{T} GeV");
  FillHist( plot_dir+ region+ "/SKEvent/Ev_XT", XT, w, 5000, 0.0, 5000.0, "X_{T} GeV");
  FillHist( plot_dir+ region+ "/SKEvent/XT_PT1", XT/leps[0]->Pt(), w, 100, 0.0, 10.0, "XT/PT(1)");

  //FillHist( plot_dir+ region+ "/SKEvent/Ev_MET", met.Pt()  , w, 100, 0, 100,"MET GeV"); //JH

  //double ST = GetST(leps, jets, fatjets, met);
  //
  //FillHist( plot_dir+ region+ "/SKEvent/Ev_ST", ST  , w, 250, 0.0, 5000.0,"ST GeV");

  //Particle METv           = GetMiniAODvMET("T1xyCorr");
  //Particle PuppiMETvULPhiCorr = GetMiniAODvMET("PuppiT1xyULCorr");

  //FillHist( plot_dir+ region+ "/SKEvent/Ev_PuppiMET_T1ULxyCorr", PuppiMETvULPhiCorr.Pt()  , w, 200, 0.0, 400.0,"MET GeV");
  //FillHist( plot_dir+ region+ "/SKEvent/Ev_pfMETphi_T1xyCorr", METv.Phi()  , w, 200, -5.0, 5.0,"MET #phi");

  //
  //FillHist( plot_dir+ region+ "/SKEvent/Mt_lep2", MT(*leps[1] ,met)  , w, 200, 0.0, 400.0,"MT GeV");
  //if(threelep) {
  //  if(GetIndexNonBestZ(leps,M_ZWINDOW)> 0 )FillHist( plot_dir+ region+ "/SKEvent/Mt_nonZLep", MT(*leps[GetIndexNonBestZ(leps,M_ZWINDOW)], met) , w, 200, 0.0, 800.0,"M(T) non Z GeV");
  //} //JH

  return;
}



void HNL_LeptonCore::FillTandP(bool passProbe, TString  Tag, TString  ID, double pr_pt, double eta,  double weight, TString  BinLabel){

  int n_bin    =8;
  vector<double> bins = { 0,120,200,300,400,500,750,1000,2000};

  if(BinLabel == "Bin2"){
    n_bin    =6;
    bins ={ 0,60,120,200,300,1000,2000};
  }

  double arrx_bins [n_bin+1];
  std::copy(bins.begin(), bins.end(), arrx_bins);

  FillHist( ID+"/"+Tag+"_"+BinLabel+"/Denom",  pr_pt,  weight, n_bin,arrx_bins);
  if(passProbe)       FillHist( ID+"/"+Tag+"_"+BinLabel+"/Num",  pr_pt,  weight, n_bin,arrx_bins);

  if(fabs(eta) < 0.9){
    FillHist( ID+"/"+Tag+"_"+BinLabel+"_Eta1/Denom",  pr_pt,  weight, n_bin,arrx_bins);
    if(passProbe)       FillHist( ID+"/"+Tag+"_"+BinLabel+"_Eta1/Num",  pr_pt,  weight, n_bin,arrx_bins);
  }
  else  if(fabs(eta) < 1.2){
    FillHist( ID+"/"+Tag+"_"+BinLabel+"_Eta2/Denom",  pr_pt,  weight, n_bin,arrx_bins);
    if(passProbe)       FillHist( ID+"/"+Tag+"_"+BinLabel+"_Eta2/Num",  pr_pt,  weight, n_bin,arrx_bins);
  }
  else     if(fabs(eta) < 2.1){

    FillHist( ID+"/"+Tag+"_"+BinLabel+"_Eta3/Denom",  pr_pt,  weight, n_bin,arrx_bins);
    if(passProbe)       FillHist( ID+"/"+Tag+"_"+BinLabel+"_Eta3/Num",  pr_pt,  weight, n_bin,arrx_bins);
    
  }
  else{
    FillHist( ID+"/"+Tag+"_"+BinLabel+"_Eta4/Denom",  pr_pt,  weight, n_bin,arrx_bins);
    if(passProbe)       FillHist( ID+"/"+Tag+"_"+BinLabel+"_Eta4/Num",  pr_pt,  weight, n_bin,arrx_bins);
  }
  
  return;

}


/// FillMuonPlots Fills all kinamatics                                                                                                                                                                                                                                          
void HNL_LeptonCore::FillMuonCollPlots(bool passSel, TString  SelSt, AnalyzerParameter& param, TString  cut,  std::vector<Muon>& muons, double w){

  if(!DrawSyst(param)) return;

  for(auto imu: muons)   FillMuonPlots(passSel, SelSt, param, cut, imu, w);

  return;
}
void HNL_LeptonCore::FillMuonCollPlots(AnalyzerParameter& param, TString  cut,  std::vector<Muon>& muons, double w){

  if(!DrawSyst(param)) return;

  TString myString = "";  // Initialize with an empty TString

  for(auto imu: muons)   FillMuonPlots(true, myString, param, cut, imu, w);
  return;
}

/// FillAllMuonPlots Fills all kinamatics in pt/eta bins                                                                              
void HNL_LeptonCore::FillMuonPlots(AnalyzerParameter& param, TString  cut,  Muon& muon, double w){
  if(!DrawSyst(param)) return;
  TString myString = "";  // Initialize with an empty TString                                                                                                                                                      

  FillMuonPlots(true, myString, param, cut, muon, w);
}                                                                                                                                      
    

void HNL_LeptonCore::FillMuonPlots(bool passSel, TString  SelSt, AnalyzerParameter& param, TString  cut,  Muon& muon, double w){
  
  if(!DrawSyst(param)) return;

  if(!passSel) return;

  cut = "MuonPlots/"+cut;

  if(SelSt != "") {
    cut = cut + "_"+SelSt;
    TString Option = "";
    if(param.SplitPlot == "PtBin_Type_") Option = cut +"/"+muon.GetPtLabel() +"_"+muon.sLepGenType();
    if(param.SplitPlot.Contains("PtBin_")) Option =cut +"/"+muon.GetPtLabel();
    if(param.SplitPlot.Contains("Type_"))   Option = cut +"/"+muon.sLepGenType();
    FillMuonKinematicPlots(param,Option, muon, w);

  }
    
  else FillMuonKinematicPlots(param, cut, muon, w);
  
  return;
}



void HNL_LeptonCore::FillLeptonKinematicPlots(AnalyzerParameter& param, TString  cut,  Lepton& lep, double w){

  if(!DrawSyst(param)) return;

  vector<Jet> JetAllColl = All_Jets;

  int IdxMatchJet=-1;
  int IdxMatchAwayJet=-1;

  double mindR(999.);
  double maxDphi=-999;
  double PtRatioAwayJet(-999.);
  double Jet_Disc(-1);
  
  TString label = param.hpostfix;

  for(unsigned int ij=0; ij<JetAllColl.size(); ij++){

    double dR=lep.DeltaR(JetAllColl.at(ij));
    if(dR<mindR){       mindR=dR; IdxMatchJet=ij;     }
    double dphi =fabs(TVector2::Phi_mpi_pi(lep.Phi()- JetAllColl.at(ij).Phi()));
    if(dphi > maxDphi) {maxDphi= dphi; IdxMatchAwayJet=ij;}
  }
  if(IdxMatchJet!=-1)     Jet_Disc       = JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet);
  if(IdxMatchAwayJet!=-1) PtRatioAwayJet = JetAllColl.at(IdxMatchAwayJet).Pt()/ lep.Pt();

  TString label_lep = (lep.LeptonFlavour()==Lepton::ELECTRON) ? "Electron": "Muon";
  double Pt_Lep =  lep.Pt();

  label += label_lep;

  /// Kinematics                                                                                                                                                                                                                                                                
  FillHist( cut+ "/Lepton_Pt_"+label   , lep.Pt() , w, 5000, 0.0, 10000.0, label_lep+"p_{T} GeV");
  FillHist( cut+ "/Lepton_Eta_"+label  , lep.Eta() , w, 60, -3.0, 3.0,label_lep+"#eta");
  FillHist( cut+ "/Lepton_Phi_"+label  , lep.Phi() , w, 200, -10.0, 10.0,label_lep+"#phi");
  double ptbins[14] = { 0.0, 10.0,15.0, 20.0, 30.0, 40.0,50.0, 100.0,200.0,300.0,400.0,500. ,1000.0,2000.};
  FillHist( cut+ "/Lepton_PtBinned_"+label   , Pt_Lep, w, 11, ptbins);

  // IP                                                                                                                                                                                                                                                                         
  FillHist( cut+ "/Lepton_Dxy_"+label           , lep.fdXY() , w, 250,  0, 0.2, "dXY");
  FillHist( cut+ "/Lepton_Dz_"+label            , lep.fdZ() , w, 250, 0, 0.5, "dZ");
  FillHist( cut+ "/Lepton_LogDxy_"+label        , lep.LogdXY() , w, 300, -20,  10, "dXY");

  FillHist( cut+ "/Lepton_dXYSig_"+ label       , lep.LogdXYSig(), w, 100, -10,10);
  FillHist( cut+ "/Lepton_dZSig_"+ label        , lep.LogdZSig(),w, 100,-10,10);
  FillHist( cut+ "/Lepton_SIP3D_"+ label        , lep.SIP3D(),w, 200,0,20);
  FillHist( cut+ "/Lepton_IP3D_"+ label        , lep.IP3D(),w, 100,0,1);


  // Lep/Jet                                                                                                                                                                                                                                                                    
  FillHist( cut+ "/Lepton_PtRel_"+label         , JetLeptonPtRelLepAware( lep), w, 500, 0.0, 100.0, "");
  FillHist( cut+ "/Lepton_PtRatio_"+label       , JetLeptonPtRatioLepAware(lep), w, 500, 0.0, 2.0, "");

  double MotherPt = lep.Pt() /  lep.CloseJet_Ptratio();
  if(MotherPt > 200) MotherPt=200;
  FillHist( cut+ "/Lepton_MotherJetPt_"+label,  MotherPt, w, 100, 0.0, 200);
  FillHist( cut+ "/Lepton_MotherFlavour_"+label , lep.CloseJet_FlavourInt(), w, 10, 0.0, 10);

  if(IdxMatchJet != -1 ){
    FillHist( cut+ "/Lepton_CHFracCJ_"+label      , JetAllColl.at(IdxMatchJet).ChargedHadEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( cut+ "/Lepton_NEMFracCJ_"+label     , JetAllColl.at(IdxMatchJet).NeutralEmEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( cut+ "/Lepton_CEMFracCJ_"+label     , JetAllColl.at(IdxMatchJet).ChargedEmEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( cut+ "/Lepton_NFracCJ_"+label       , JetAllColl.at(IdxMatchJet).NeutralHadEnergyFraction(), w, 100, 0.0, 1.0, "");
    FillHist( cut+ "/Lepton_JetDiscCJ_"+label     , Jet_Disc , w, 400, -2.0, 2.0, "");
    FillHist( cut+ "/Lepton_PtRatioAwayJet_"+label, PtRatioAwayJet , w, 100, 0.0, 5.0, "");
  }

  map<TString, double> lep_bdt_map = lep.MAPBDT();
  for(auto i : lep_bdt_map){
    FillHist( cut+ "/Lepton_mva_"+i.first+label  , i.second, w, 100, -1.0, 1.0, "MVA");
    FillHist( cut+ "/Lepton_MotherJetPtVsMVA_"+label,  lep.CloseJet_Ptratio(), i.second, w, 100, 0.0, 2.0, 100, -1.0, 1.);
    FillHist( cut+ "/Lepton_RelIsoVsMVA_"+label,  lep.RelIso() ,  i.second, w, 200, 0.0, 0.4, 100, -1.0, 1);
  }

  // Isolation                                                                                                                                                                                                                                                              
  FillHist( cut+ "/Lepton_MotherJetPtVRelIso_"+label,  lep.CloseJet_Ptratio(), lep.RelIso(),  w, 100, 0.0, 2.0, 200, 0.0, 0.4);
  FillHist( cut+ "/Lepton_MiniReliso_"+label , lep.MiniRelIso() , w, 500, 0.0, 2.0, "R_{ISO} GeV");
  FillHist( cut+ "/Lepton_Reliso_el_"+label , lep.RelIso() , w, 200, 0.0, 0.4, "R_{ISO} GeV");

}


void HNL_LeptonCore::FillMuonKinematicPlots(AnalyzerParameter& param, TString  cut,  Muon& lep, double w){

  if(!DrawSyst(param)) return;
  Lepton lepObj = Lepton(lep);
  FillLeptonKinematicPlots(param, cut, lepObj,w);

  TString label = param.hpostfix;
  
  FillHist( cut+ "/Muon/Mva_"+label    , lep.MVA(), w, 600, -1.0, 1.0, "MVA");
  FillHist( cut+ "/Muon/Chi2_"+label   , lep.Chi2(), w, 200,0.0, 20.0, "chi2");
  FillHist( cut+ "/Muon/Validhits_"+label  , lep.ValidMuonHits(), w, 100,0.0, 100.0, "");
  FillHist( cut+ "/Muon/Matched_stations_"+label  , lep.MatchedStations(), w, 10,0.0, 10.0, "");
  FillHist( cut+ "/Muon/Pixel_hits_"+label  , lep.PixelHits(), w, 10,0.0, 10.0, "");
  FillHist( cut+ "/Muon/Tracker_layers_"+label  , lep.TrackerLayers(), w, 50,0.0, 50.0, "");

  vector<TString> IDs ={"HNTightV2","POGLoose","POGMedium","POGTight"};
  for (auto ID : IDs){
    if(lep.PassID(ID)) FillHist( cut+ "/Muon/Pass_"+ID+label  , 1, w, 4, 0.0, 4.0, "Pass " + ID);
    else FillHist( cut+ "/Muon/Pass_"+ID+label  , 0, w, 4, 0.0, 4.0, "Pass " + ID);

    if(lep.PassID(ID)) FillHist( cut+ "/Muon/Pass_"+ID+label  , 1, w, 4, 0.0, 4.0, "Pass " + ID);
    else FillHist( cut+ "/Muon/Pass_"+ID+label  , 0, w, 4, 0.0, 4.0, "Pass " + ID);

  }

  return;
}



void HNL_LeptonCore::FillElectronPlots(AnalyzerParameter& param , TString  cut,  std::vector<Electron>& ElectronColl, double w){

  if(!DrawSyst(param)) return;

  for(auto iel : ElectronColl)   FillElectronKinematicPlots(param, cut, iel, w);

  return;
}

void HNL_LeptonCore::FillAllElectronPlots(AnalyzerParameter& param , TString  cut,  std::vector<Electron>& ElectronColl, double w){

  if(!DrawSyst(param)) return;

  TString label  = param.hpostfix;


  FillHist( cut+ "/NElectrons"+label , size(ElectronColl) , w, 5, 0.0, 5.0, "n_{el}");

  for(unsigned int i=0; i < ElectronColl.size(); i++){

    TString eta_label="";
    if(fabs(ElectronColl.at(i).Eta()) < 1.5) eta_label = "_BB";
    else eta_label = "_EC";

    TString pt_label=GetPtBin(false,ElectronColl.at(i).Pt());

    int LepType= ElectronColl.at(i).LeptonGenType();
    TString gen_label ="";
    if (LepType >= 0) gen_label = to_string(LepType);
    else gen_label = "Minus_"+to_string(fabs(LepType));

    if(Analyzer.Contains("BDTStudies")){
      
      if(GenTypeMatched(MatchGenDef(All_Gens,ElectronColl.at(i)))){
        TString myString = "Electron_"+label+"_"+ ElectronColl.at(i).CloseJet_Flavour()+"_"+MatchGenDef(All_Gens,ElectronColl[i])+"_"+ cut;
        if(label.Contains("Fake")) FillElectronKinematicPlots( param,myString , ElectronColl.at(i), w);
      }
    }
    TString myString = "Electron_"+label+"_"+ ElectronColl.at(i).CloseJet_Flavour() + cut;
    FillElectronKinematicPlots( param,myString , ElectronColl.at(i), w);
 
  }
  return;
}

void HNL_LeptonCore::FillElectronKinematicPlots(AnalyzerParameter& param , TString  cut,  Electron& lep, double w){

  if(!DrawSyst(param)) return;
  
  Lepton lepObj = Lepton(lep);

  FillLeptonKinematicPlots(param, cut, lepObj,w);

  TString label = param.hpostfix;

  FillHist( cut+ "/Electron/SCEta_"+label  , lep.scEta() , w, 60, -3.0,  3.0, "electron #eta");
  FillHist( cut+ "/Electron/SCPhi_"+label  , lep.scPhi() , w, 70, -3.5, 3.5,"electron #phi");

  /// POG MVA                                                                                                                                                                                                                                                                   
  FillHist( cut+ "/Electron/NoIsoMva_"           +label  , lep.MVANoIso(),                  w, 600, -1.0, 1.0, "MVA");
  FillHist( cut+ "/Electron/RelNoIsoMvaResponse_"+label  , lep.MVANoIsoResponse()/lep.Pt(), w, 160, -1.0,1.0, "MVA");
  FillHist( cut+ "/Electron/NoIsoMvaResponse_"   +label  , lep.MVANoIsoResponse(),          w, 160, -8.0,8.0, "MVA");
  FillHist( cut+ "/Electron/MvaIso_"             +label  , lep.MVAIso(),                    w, 600, -1.0, 1.0, "MVA");
  FillHist( cut+ "/Electron/RelMvaIsoResponse_"  +label  , lep.MVAIsoResponse()/lep.Pt(),   w, 600, -1.0, 1.0, "MVA");
  FillHist( cut+ "/Electron/MvaIsoResponse_"     +label  , lep.MVAIsoResponse(),            w, 600, -8.0, 8.0, "MVA");

  /// CF MVA                                                                                                                                                                                                                                                                    
  FillHist( cut+ "/Electron/EoverP_"       +label  , lep.EOverP() , w, 400, -10, 100);
  FillHist( cut+ "/Electron/LogEoverP_"    +label  , log(lep.EOverP()) , w, 400, -1, 10);

  FillHist( cut+ "/Electron/FBrem_"        +label  , std::max(lep.FBrem(),-2.),  w, 200, -1.0, 1);
  FillHist( cut+ "/Electron/R9_"           +label  , lep.R9(), w, 100, 0.0, 1);
  FillHist( cut+ "/Electron/dr03TkSumPt_"  +label  , lep.dr03TkSumPt()/lep.UncorrPt(), w, 200, 0.0, 0.5, "");
  FillHist( cut+ "/Electron/E15_"          +label  , lep.e15(), w, 1000, 0.0, 1000);
  FillHist( cut+ "/Electron/E25_"          +label  , lep.e25(), w, 1000, 0.0, 1000);
  FillHist( cut+ "/Electron/E55_"          +label  , lep.e55(), w, 1000, 0.0, 1000);
  FillHist( cut+ "/Electron/e2x5OverE5x5_" +label  , lep.e2x5OverE5x5(), w, 100, 0.0, 1);
  FillHist( cut+ "/Electron/e1x5OverE5x5_" +label  , lep.e1x5OverE5x5(), w, 100, 0.0, 1);
  FillHist( cut+ "/Electron/EtaWidth_"+label  , lep.EtaWidth(), w, 100, 0.0, 0.2);
  FillHist( cut+ "/Electron/PhiWidth_"+label  , lep.PhiWidth(), w, 200, 0.0, 0.2);

  FillHist( cut+ "/Electron/MissingHits_"+label  , lep.NMissingHits(), w, 8, 0.0, 8.0, "Missing Hits");
  FillHist( cut+ "/Electron/Full5x5_sigmaIetaIeta_"+label  , lep.Full5x5_sigmaIetaIeta(), w, 200, 0.0, 0.05, "");
  FillHist( cut+ "/Electron/PassConversionVeto_"+label  , lep.PassConversionVeto(), w, 2, 0.0, 2.0, "");
  FillHist( cut+ "/Electron/IsGsfCtfScPixChargeConsistent_"+label  , lep.IsGsfCtfScPixChargeConsistent(), w, 2, 0.0, 2.0, "");
  FillHist( cut+ "/Electron/IsGsfScPixChargeConsistent_"+label  , lep.IsGsfScPixChargeConsistent(), w, 2, 0.0, 2.0, "");
  FillHist( cut+ "/Electron/IsGsfCtfChargeConsistent_"+label  , lep.IsGsfCtfChargeConsistent(), w, 2, 0.0, 2.0, "");
  FillHist( cut+ "/Electron/dEtaSeed_"+label  , lep.dEtaSeed(), w, 200, -0.1, 0.1, "");
  FillHist( cut+ "/Electron/dPhiIn_"+label  , lep.dPhiIn(), w, 400, -0.5, 0.5, "");
  FillHist( cut+ "/Electron/dEtaIn_"+label  , lep.dEtaIn(), w, 400, -0.2, 0.2, "");
  FillHist( cut+ "/Electron/HoverE_"+label  , lep.HoverE(), w, 500, 0.0, 1.0, "");
  FillHist( cut+ "/Electron/TrkIso_"+label  , lep.TrkIso(), w, 1000, 0.0, 100, "");
  FillHist( cut+ "/Electron/isEcalDriven_"+label  , lep.isEcalDriven(), w, 2, 0.0, 2.0, "");
  FillHist( cut+ "/Electron/InvEminusInvP_"+label  , fabs(lep.InvEminusInvP()), w, 100, 0.0, 0.2);


  // COnv MVA                                                                                                                                                                                                                                                                   
  FillHist( cut+ "/Electron/dr03HcalTowerSumEt_"+label  , lep.dr03HcalTowerSumEt(), w, 200, 0.0, 10.0, "");
  /// Extra                                                                                                                                                                                                                                                                     
  FillHist( cut+ "/Electron/dr03EcalRecHitSumEt_"+label  , lep.dr03EcalRecHitSumEt(), w, 2000, 0.0, 100.0, "");
  FillHist( cut+ "/Electron/dr03HcalDepth1TowerSumEt_"+label  , lep.dr03HcalDepth1TowerSumEt(), w, 200, 0.0, 10.0, "");
  FillHist( cut+ "/Electron/ecalPFClusterIso_"+label  , lep.ecalPFClusterIso()/lep.UncorrPt(), w, 100, 0.0, 1.0, "");
  FillHist( cut+ "/Electron/hcalPFClusterIso_"+label  , lep.hcalPFClusterIso()/lep.UncorrPt(), w, 100, 0.0, 1.0, "");

  vector<TString> IDs ={"HNVetoMVA","HNVeto","HNL_ULID_Split_1","HNL_ULID_Split_2"};
  for (auto ID : IDs){
    if(lep.PassID(ID)) FillHist( cut+ "/Electron/Pass_"+ID+label  , 1, w, 4, 0.0, 4.0, "Pass " + ID);
    else FillHist( cut+ "/Electron/Pass_"+ID+label  , 0, w, 4, 0.0, 4.0, "Pass " + ID);
  }

  return;


}


void HNL_LeptonCore::FillLeptonPlots(AnalyzerParameter& param,std::vector<Lepton *>& leps, TString  this_region, double weight){
  
  if(!DrawSyst(param)) return;

  for(unsigned int i=0; i<leps.size(); i++){

    TString this_itoa = TString::Itoa(i,10);

    Lepton *lep = leps[i];

    FillHist(this_region+"/Lepton/"+this_itoa+"_Pt_"+this_region, lep->Pt(), weight, 1000, 0.0, 1000.);
    FillHist(this_region+"/Lepton/"+this_itoa+"_Eta_"+this_region, lep->Eta(), weight, 60, -3.0, 3.);
    FillHist(this_region+"/Lepton/"+this_itoa+"_RelIso_"+this_region, lep->RelIso(), weight, 100, 0.0, 1.);
    FillHist(this_region+"/Lepton/"+this_itoa+"_MiniRelIso_"+this_region, lep->MiniRelIso(), weight, 100, 0.0, 1.);

    FillHist(this_region+"/Lepton/"+this_itoa+"_dXY_"+this_region, fabs(lep->dXY()), weight, 500, 0.0, 0.05);
    FillHist(this_region+"/Lepton/"+this_itoa+"_dXYSig_"+this_region, fabs(lep->dXY()/lep->dXYerr()), weight, 100, 0.0, 10);
    FillHist(this_region+"/Lepton/"+this_itoa+"_dZ_"+this_region, fabs(lep->dZ()), weight, 500, 0.0, 0.5);
    FillHist(this_region+"/Lepton/"+this_itoa+"_dZSig_"+this_region, fabs(lep->dZ()/lep->dZerr()), weight, 100, 0.0, 10);
    FillHist(this_region+"/Lepton/"+this_itoa+"_IP3D_"+this_region, fabs(lep->IP3D()), weight, 500, 0.0, 0.5);
    FillHist(this_region+"/Lepton/"+this_itoa+"_IP3DSig_"+this_region, fabs(lep->IP3D()/lep->IP3Derr()), weight, 100, 0.0, 10);

    if(lep->LeptonFlavour()==Lepton::ELECTRON){
      Electron *el = (Electron *)lep;
      FillHist(this_region+"/Lepton/"+this_itoa+"_MVANoIso_"+this_region, el->MVANoIso(), weight, 200, -1.0, 1.);
    }
    else if(lep->LeptonFlavour()==Lepton::MUON){
      Muon *mu = (Muon *)lep;
      FillHist(this_region+"/Lepton/"+this_itoa+"_Chi2_"+this_region, mu->Chi2(), weight, 500, 0.0, 50.);
      FillHist(this_region+"/Lepton/"+this_itoa+"_TrkRelIso_"+this_region, mu->TrkIso()/mu->TuneP4().Pt(), weight, 100, 0.0, 1.);
    }
    else{
      cout << "[HNL_LeptonCore::FillLeptonPlots] lepton flavour wrong.." << endl;
      exit(EXIT_FAILURE);
    }


  }
  return;
}

void HNL_LeptonCore::FillJetPlots(AnalyzerParameter& param,std::vector<Jet>& jets, std::vector<FatJet>& fatjets, TString  this_region, double weight){

  if(!DrawSyst(param)) return;

  for(unsigned int i=0; i<jets.size(); i++){

    TString this_itoa = TString::Itoa(i,10);
    FillHist(this_region+"/Jet_"+this_itoa+"_Pt_"+this_region, jets.at(i).Pt(), weight, 1000, 0.0, 1000.);
    FillHist(this_region+"/Jet_"+this_itoa+"_Eta_"+this_region, jets.at(i).Eta(), weight, 60, -3.0, 3.);

  }

  for(unsigned int i=0; i<fatjets.size(); i++){

    TString this_itoa = TString::Itoa(i,10);
    FillHist(this_region+"/FatJet_"+this_itoa+"_Pt_"+this_region, fatjets.at(i).Pt(), weight, 1000, 0.0, 1000.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_Eta_"+this_region, fatjets.at(i).Eta(), weight, 60, -3.0, 3.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_Mass_"+this_region, fatjets.at(i).M(), weight, 3000, 0.0, 3000.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_SDMass_"+this_region, fatjets.at(i).SDMass(), weight, 3000, 0.0, 3000.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_LSF_"+this_region, fatjets.at(i).LSF(), weight, 100, 0.0, 1.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_PuppiTau21_"+this_region, fatjets.at(i).PuppiTau2()/fatjets.at(i).PuppiTau1(), weight, 100, 0.0, 1.);
  }

}

double HNL_LeptonCore::FillWeightHist(TString label, double _weight){

  int szst = 50 - std::string(label).size();
  TString empty_st = "";
  for(int i = 0 ; i < szst; i++) empty_st+= " ";
  if(run_Debug) cout << "HNL_LeptonCore::FillWeightHist ["+label+"] " <<  empty_st<< "  correction =" <<   _weight << endl;

  double max_x_range = 5.;
  if(label.Contains("Lumi")) max_x_range = 100000; 

  if(!label.Contains("Syst_"))   FillHist( "weights/"+ label , _weight ,1.0, 200, -1.*max_x_range, max_x_range,"ev weight");

  return _weight;
}

double HNL_LeptonCore::FillFakeWeightHist(TString  label, vector<Lepton *>& Leps,AnalyzerParameter& param,  double _weight){

  if(param.PlottingVerbose == -1) return 0.;

  if(run_Debug) cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
  if(run_Debug) {
    for(auto i: Leps) {
      if(i->LeptonFlavour()==Lepton::MUON) cout << "Muon " << endl;
      else cout<< "Electron " << endl;
    }
  }

  if(run_Debug) cout << "nLep = " << Leps.size() << endl;
  if(run_Debug) cout << "Weight = " << _weight << endl;

  TString TLType="";
  for(auto i: Leps) {
    if(i->PassLepID())TLType+="T";
    else TLType+="L";
  }
  if(run_Debug) cout << "TLType = " << TLType << endl;

  //  if(Leps.size() > 0) cout << "LepTightIDName = " << Leps[0]->LepTightIDName() << endl;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
  // cout << "TLType = " << TLType <<  " Weight = " << _weight << endl;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      

  FillHist( "FakeWeights/"+ label , _weight ,1.0, 200, -5.0, 5,"ev weight");

  if(Leps.size() == 2){

    bool T1 = Leps[0]->PassLepID();
    bool T2 = Leps[1]->PassLepID();
    if(T1&&T2)         FillHist( "FakeStudy/TLSplit/TightLoose"+ label , 1, _weight , 5, 0.0, 5.0);
    if(T1&&!T2)        FillHist( "FakeStudy/TLSplit/TightLoose"+ label , 2, _weight , 5, 0.0, 5.0);
    if(!T1&&T2)        FillHist( "FakeStudy/TLSplit/TightLoose"+ label , 3, _weight , 5, 0.0, 5.0);
    if(!T1&&!T2)       FillHist( "FakeStudy/TLSplit/TightLoose"+ label , 4, _weight , 5, 0.0, 5.0);
    if(run_Debug) cout << "FillFakeWeightHist Lepton Types T1T2 = " << T1 << " "  << T2 << endl;
  }

  for(auto ilep : Leps) {
    FillHist( "FakeStudy/MVAPlots/QCD_LFvsHF_v5_"+label, ilep->HNL_MVA_Fake("QCD_LFvsHF_v5"), 1, 100, -1.0, 1.);

    if(ilep->HNL_MVA_Fake("QCD_LFvsHF_v5") > 0) FillHist( "FakeStudy/MVAPlots/QCD_BvsC_v5_LF_"+label, ilep->HNL_MVA_Fake("QCD_BvsC_v5"), 1, 100, -1.0, 1);
    else  FillHist( "FakeStudy/MVAPlots/QCD_BvsC_v5_HF_"+label, ilep->HNL_MVA_Fake("QCD_BvsC_v5"), 1, 100, -1.0, 1.);


    if(!ilep->PassLepID())  {

      
      //FillHist( "FakeStudy/Rates/"+ilep->LepTightIDName()+"/Loose_"+ label , FR, 1 , 1000, 0.0, 5);
      //FillHist( "FakeStudy/Rates/"+ilep->LepTightIDName()+"/Loose_VsPt"+ label , ilep->PtMaxed(60.), FR, 1 , 60, 0, 60, 1000, 0.0, 5);
      //FillHist( "FakeStudy/Rates/"+ilep->LepTightIDName()+"/Loose_"+ilep->sEtaRegion()+"_"+ label , FR, 1 , 1000, 0.0, 5);

    }

  }


  if(Leps.size() == 2){
    bool T1 = Leps[0]->PassLepID();
    bool T2 = Leps[1]->PassLepID();
    double FW = GetFakeWeight(Leps, param);
    if(T1&&T2)FillHist( "FakeStudy/EventWeight/TT_"+ label , FW, _weight , 100, 0.0, 5.0);
    if(T1&&!T2)FillHist( "FakeStudy/EventWeight/TL_"+ label , FW, _weight , 100, 0.0, 5.0);
    if(!T1&&T2)FillHist( "FakeStudy/EventWeight/LT_"+ label , FW, _weight , 100, 0.0, 5.0);
    if(!T1&&!T2)FillHist( "FakeStudy/EventWeight/LL_"+ label , FW, _weight , 100, 0.0, 5.0);
    if(run_Debug) cout << "EventWeight = " << FW << endl;
  }

  return _weight;
}


