#include "HNL_Lepton_ID_BDT_KinVar.h"

void HNL_Lepton_ID_BDT_KinVar::initializeAnalyzer(){

  SeperateFakes = HasFlag("SeperateFake");
  SeperateConv = HasFlag("SeperateConv");
  SeperateCF = HasFlag("SeperateCF");
  SeperatePrompt = HasFlag("SeperatePrompt");

  GetRatio = HasFlag("GetRatio");
  HNL_LeptonCore::initializeAnalyzer();

  InitializeIDTreeVars();

  SetupTrees(tree_mm,tree_ee);
  

}
void HNL_Lepton_ID_BDT_KinVar::SetupTrees(TTree* tree_mm, TTree* tree_ee){

  tree_mm->Branch("Pt", &bdt_id_Pt, "Pt/F"); 
  tree_ee->Branch("Pt", &bdt_id_Pt, "Pt/F");
  tree_mm->Branch("PtBinned", &bdt_id_PtBinned, "PtBinned/F");
  tree_ee->Branch("PtBinned", &bdt_id_PtBinned, "PtBinned/F");
  tree_mm->Branch("PtBinned2", &bdt_id_PtBinned2, "PtBinned2/F");
  tree_ee->Branch("PtBinned2", &bdt_id_PtBinned2, "PtBinned2/F");
  tree_mm->Branch("Eta", &bdt_id_Eta, "Eta/F");
  tree_ee->Branch("Eta", &bdt_id_Eta, "Eta/F");
  tree_ee->Branch("PileUp", &bdt_id_PileUp, "PileUp/F");
  tree_mm->Branch("PileUp", &bdt_id_PileUp, "PileUp/F");

  tree_mm->Branch("MiniIsoChHad", &bdt_id_MiniIsoChHad, "MiniIsoChHad/F"); 
  tree_ee->Branch("MiniIsoChHad", &bdt_id_MiniIsoChHad, "MiniIsoChHad/F");
  tree_mm->Branch("MiniIsoNHad", &bdt_id_MiniIsoNHad, "MiniIsoNHad/F"); 
  tree_ee->Branch("MiniIsoNHad", &bdt_id_MiniIsoNHad, "MiniIsoNHad/F");
  tree_mm->Branch("MiniIsoPhHad", &bdt_id_MiniIsoPhHad, "MiniIsoPhHad/F");  
  tree_ee->Branch("MiniIsoPhHad", &bdt_id_MiniIsoPhHad, "MiniIsoPhHad/F");

  tree_ee->Branch("RelMiniIsoCh", &bdt_id_RelMiniIsoCh, "RelMiniIsoCh/F");
  tree_ee->Branch("RelMiniIsoN", &bdt_id_RelMiniIsoN, "RelMiniIsoN/F");
  tree_mm->Branch("RelMiniIsoCh", &bdt_id_RelMiniIsoCh, "RelMiniIsoCh/F");
  tree_mm->Branch("RelMiniIsoN", &bdt_id_RelMiniIsoN, "RelMiniIsoN/F");


  tree_mm->Branch("IsoChHad", &bdt_id_IsoChHad, "IsoChHad/F"); 
  tree_ee->Branch("IsoChHad", &bdt_id_IsoChHad, "IsoChHad/F");
  tree_mm->Branch("IsoNHad", &bdt_id_IsoNHad, "IsoNHad/F"); 
  tree_ee->Branch("IsoNHad", &bdt_id_IsoNHad, "IsoNHad/F");
  tree_mm->Branch("IsoPhHad", &bdt_id_IsoPhHad, "IsoPhHad/F"); 
  tree_ee->Branch("IsoPhHad", &bdt_id_IsoPhHad, "IsoPhHad/F");


  tree_mm->Branch("Dxy", &bdt_id_Dxy, "Dxy/F"); 
  tree_ee->Branch("Dxy", &bdt_id_Dxy, "Dxy/F");
  tree_mm->Branch("RelDxy", &bdt_id_RelDxy, "RelDxy/F");
  tree_ee->Branch("RelDxy", &bdt_id_RelDxy, "RelDxy/F");
  tree_mm->Branch("DxySig", &bdt_id_DxySig, "DxySig/F"); 
  tree_ee->Branch("DxySig", &bdt_id_DxySig, "DxySig/F");
  tree_mm->Branch("Dz", &bdt_id_Dz, "Dz/F"); 
  tree_ee->Branch("Dz", &bdt_id_Dz, "Dz/F");
  tree_mm->Branch("RelDz", &bdt_id_RelDz, "RelDz/F");
  tree_ee->Branch("RelDz", &bdt_id_RelDz, "RelDz/F");
  tree_mm->Branch("DzSig", &bdt_id_DzSig, "DzSig/F"); 
  tree_ee->Branch("DzSig", &bdt_id_DzSig, "DzSig/F");
  tree_mm->Branch("RelIso", &bdt_id_RelIso, "RelIso/F"); 
  tree_ee->Branch("RelIso", &bdt_id_RelIso, "RelIso/F");
  tree_mm->Branch("IP3D", &bdt_id_IP3D, "IP3D/F"); 
  tree_ee->Branch("IP3D", &bdt_id_IP3D, "IP3D/F");
  tree_mm->Branch("RelIP3D", &bdt_id_RelIP3D, "RelIP3D/F");
  tree_ee->Branch("RelIP3D", &bdt_id_RelIP3D, "RelIP3D/F");

  tree_mm->Branch("MVA", &bdt_id_MVA, "MVA/F"); 
  tree_ee->Branch("MVA", &bdt_id_MVA, "MVA/F");
  tree_ee->Branch("MVARaw", &bdt_id_MVARaw, "MVARaw/F");

  tree_ee->Branch("MVAIso", &bdt_id_MVAIso, "MVAIso/F");
  tree_ee->Branch("MVAIsoRaw", &bdt_id_MVAIsoRaw, "MVAIsoRaw/F");

  tree_mm->Branch("Chi2", &bdt_id_Chi2, "Chi2/F");  
  tree_mm->Branch("Validhits", &bdt_id_Validhits, "Validhits/F");  

  tree_mm->Branch("Matched_stations", &bdt_id_Matched_stations, "Matched_stations/F");
  tree_mm->Branch("Pixel_hits", &bdt_id_Pixel_hits, "Pixel_hits/F");
  tree_mm->Branch("Minireliso", &bdt_id_Minireliso, "Minireliso/F");
  tree_mm->Branch("Tracker_layers", &bdt_id_Tracker_layers, "Tracker_layers/F");
  tree_mm->Branch("POGTight", &bdt_id_POGTight, "POGTight/F");
  tree_mm->Branch("POGMedium", &bdt_id_POGMedium, "POGMedium/F");
  tree_mm->Branch("HNTightID", &bdt_id_HNTightID, "HNTightID/F");

  tree_ee->Branch("POGTight", &bdt_id_POGTight, "POGTight/F");
  tree_ee->Branch("POGMedium", &bdt_id_POGMedium, "POGMedium/F");
  tree_ee->Branch("HNTightID", &bdt_id_HNTightID, "HNTightID/F");
  tree_ee->Branch("POGMVA80", &bdt_id_POGMVA80ID, "POGMVA80/F");
  tree_ee->Branch("POGMVA90", &bdt_id_POGMVA90ID, "POGMVA90/F");
  tree_mm->Branch("PtRatio", &bdt_id_PtRatio, "PtRatio/F");         tree_ee->Branch("PtRatio", &bdt_id_PtRatio, "PtRatio/F");         
  tree_mm->Branch("PtRatioV2", &bdt_id_PtRatioV2, "PtRatioV2/F");         tree_ee->Branch("PtRatioV2", &bdt_id_PtRatioV2, "PtRatioV2/F");         
  tree_mm->Branch("PtRatioV3", &bdt_id_PtRatioV3, "PtRatioV3/F");         tree_ee->Branch("PtRatioV3", &bdt_id_PtRatioV3, "PtRatioV3/F");         
  tree_mm->Branch("PtRel", &bdt_id_PtRel, "PtRel/F");         tree_ee->Branch("PtRel", &bdt_id_PtRel, "PtRel/F");         
  tree_mm->Branch("PtRelV2", &bdt_id_PtRelV2, "PtRelV2/F");         tree_ee->Branch("PtRelV2", &bdt_id_PtRelV2, "PtRelV2/F");         
  tree_mm->Branch("PtRelV3", &bdt_id_PtRelV3, "PtRelV3/F");         tree_ee->Branch("PtRelV3", &bdt_id_PtRelV3, "PtRelV3/F");         

  tree_mm->Branch("CEMFracCJ", &bdt_id_CEMFracCJ, "CEMFracCJ/F");   tree_ee->Branch("CEMFracCJ", &bdt_id_CEMFracCJ, "CEMFracCJ/F");   
  tree_mm->Branch("NEMFracCJ", &bdt_id_NEMFracCJ, "NEMFracCJ/F");   tree_ee->Branch("NEMFracCJ", &bdt_id_NEMFracCJ, "NEMFracCJ/F");   
  tree_mm->Branch("CHFracCJ", &bdt_id_CHFracCJ, "CHFracCJ/F");      tree_ee->Branch("CHFracCJ", &bdt_id_CHFracCJ, "CHFracCJ/F");      
  tree_mm->Branch("NHFracCJ", &bdt_id_NHFracCJ, "NHFracCJ/F");      tree_ee->Branch("NHFracCJ", &bdt_id_NHFracCJ, "NHFracCJ/F");      
  tree_mm->Branch("MuFracCJ", &bdt_id_MuFracCJ, "MuFracCJ/F");      tree_ee->Branch("MuFracCJ", &bdt_id_MuFracCJ, "MuFracCJ/F");      
  tree_mm->Branch("JetDiscCJ", &bdt_id_JetDiscCJ, "JetDiscCJ/F");   tree_ee->Branch("JetDiscCJ", &bdt_id_JetDiscCJ, "JetDiscCJ/F");   
  tree_mm->Branch("JetDiscCJCvsB", &bdt_id_JetDiscCJCvsB, "JetDiscCJCvsB/F");   tree_ee->Branch("JetDiscCJCvsB", &bdt_id_JetDiscCJCvsB, "JetDiscCJCvsB/F");
  tree_mm->Branch("JetDiscCJCvsL", &bdt_id_JetDiscCJCvsL, "JetDiscCJCvsL/F");   tree_ee->Branch("JetDiscCJCvsL", &bdt_id_JetDiscCJCvsL, "JetDiscCJCvsL/F");


  // Electron Specific variables
  tree_ee->Branch("MissingHits", &bdt_id_MissingHits, "MissingHits/F");
  tree_ee->Branch("Full5x5_sigmaIetaIeta", &bdt_id_Full5x5_sigmaIetaIeta, "Full5x5_sigmaIetaIeta/F");
  tree_ee->Branch("dEtaSeed",    &bdt_id_dEtaSeed, "dEtaSeed/F");
  tree_ee->Branch("dPhiIn", &bdt_id_dPhiIn, "dPhiIn/F");
  tree_ee->Branch("dEtaIn", &bdt_id_dEtaIn, "dEtaIn/F");
  tree_ee->Branch("HoverE", &bdt_id_HoverE, "HoverE/F");
  tree_ee->Branch("TrkIso", &bdt_id_TrkIso, "TrkIso/F");
  tree_ee->Branch("isEcalDriven", &bdt_id_isEcalDriven, "isEcalDriven/F");
  tree_ee->Branch("EoverP", &bdt_id_EoverP, "EoverP/F");
  tree_ee->Branch("FBrem", &bdt_id_FBrem, "FBrem/F");
  tree_ee->Branch("Rho", &bdt_id_Rho, "Rho/F");
  tree_ee->Branch("InvEminusInvP", &bdt_id_InvEminusInvP, "InvEminusInvP/F");
  tree_ee->Branch("PassConversionVeto", &bdt_id_PassConversionVeto, "PassConversionVeto/F");
  tree_ee->Branch("ecalPFClusterIso", &bdt_id_ecalPFClusterIso, "ecalPFClusterIso/F");
  tree_ee->Branch("hcalPFClusterIso", &bdt_id_hcalPFClusterIso, "hcalPFClusterIso/F");
  tree_ee->Branch("IsGsfCtfScPixChargeConsistent", &bdt_id_IsGsfCtfScPixChargeConsistent, "IsGsfCtfScPixChargeConsistent/F");
  tree_ee->Branch("IsGsfScPixChargeConsistent", &bdt_id_IsGsfScPixChargeConsistent, "IsGsfScPixChargeConsistent/F");
  tree_ee->Branch("IsGsfCtfChargeConsistent", &bdt_id_IsGsfCtfChargeConsistent, "IsGsfCtfChargeConsistent/F");
  
  tree_ee->Branch("EtaWidth",&bdt_id_EtaWidth,"EtaWidth/F");
  tree_ee->Branch("PhiWidth",&bdt_id_PhiWidth,"PhiWidth/F");
  tree_ee->Branch("e2x5OverE5x5",&bdt_id_e2x5OverE5x5,"e2x5OverE5x5/F");
  tree_ee->Branch("e1x5OverE5x5",&bdt_id_e1x5OverE5x5,"e1x5OverE5x5/F");
  tree_ee->Branch("e15",&bdt_id_e15,"e15/F");
  tree_ee->Branch("e25",&bdt_id_e25,"e25/F");
  tree_ee->Branch("e55",&bdt_id_e55,"e55/F");
  tree_ee->Branch("dr03EcalRecHitSumEt",&bdt_id_dr03EcalRecHitSumEt,"dr03EcalRecHitSumEt/F");
  tree_ee->Branch("dr03HcalDepth1TowerSumEt",&bdt_id_dr03HcalDepth1TowerSumEt,"dr03HcalDepth1TowerSumEt/F");
  tree_ee->Branch("dr03HcalTowerSumEt",&bdt_id_dr03HcalTowerSumEt,"dr03HcalTowerSumEt/F");
  tree_ee->Branch("dr03TkSumPt",&bdt_id_dr03TkSumPt,"dr03TkSumPt/F");
  tree_ee->Branch("R9",&bdt_id_R9,"R9/F");

  // Event weight
  tree_mm->Branch("w_id_tot", &bdt_id_w_id_tot, "w_id_tot/F"); 
  tree_ee->Branch("w_id_tot", &bdt_id_w_id_tot, "w_id_tot/F");                     

  outfile->cd();

}


void HNL_Lepton_ID_BDT_KinVar::executeEvent(){

  if(MCSample.Contains("TTLJ") && SeperatePrompt) {
    double r = ((double) rand() / (RAND_MAX));

    if(!HasFlag("NoBJet") && (r > 0.12)) return;
  }
  
  AnalyzerParameter param_bdt = HNL_LeptonCore::InitialiseHNLParameter("BDT");
  
  Event ev = GetEvent();

  if(!PassMETFilter()) return;

  /// Ignore PU and Prefireweight
  double weight = MCweight(true, true)*ev.GetTriggerLumi("Full") ;//SetupWeight(ev,param_bdt);

  vector<TString> MCMergeList = {"DY","WG"};
  if(SeperateConv || SeperateCF)weight *= MergeMultiMC(MCMergeList,"CombineAll");

  if(weight <= 0) {
    std::vector<Electron>   ElectronCollTMP =  SelectElectrons( param_bdt,"MVAID", 10., 2.5);
    for(auto ilep : ElectronCollTMP)   FillHist( "fEtaElZeroW", fabs(ilep.Eta()) ,weight, 300., 0., 3);
    ////  Remove -v2 weights for training 
    return;
  }
  
  if(SeperateFakes && !HasFlag("ByType")){
    weight  = 1.;//GetPrefireWeight(0) * GetPileUpWeight(nPileUp,0); 
    if(MCSample.Contains("TTL")) weight*= 0.5;
  }
  if(SeperatePrompt) {
    weight  = 1.;//GetPrefireWeight(0) * GetPileUpWeight(nPileUp,0);
  }
  
  if(MCSample.Contains("Type")) weight = 1;//GetPrefireWeight(0) * GetPileUpWeight(nPileUp,0);
  
  FillHist("CutFlow", 0, weight,  20, 0., 20.);
  FillHist( "nPV", nPV ,weight, 100., 0., 100.);
 
  vector<HNL_LeptonCore::Channel> channels = {EE,MuMu};
  
  for(auto dilep_channel : channels){

    std::vector<Muon>       MuonCollTAll     =  SelectMuons    ( param_bdt,"MVAID", 10., 2.4);
    std::vector<Electron>   ElectronCollTAll =  SelectElectrons( param_bdt,"MVAID", 10., 2.5);
 
    bool HasHEM(false);
    for(auto i: ElectronCollTAll) {
      if(FindHEMElectron(i)) HasHEM=true;
    }
    if(HasHEM) continue;
    
    FillHist( "nPVHEM", nPV ,weight, 100., 0., 100.);


    if(dilep_channel == EE)  MuonCollTAll.clear();
    if(dilep_channel == MuMu) ElectronCollTAll.clear();

    std::vector<Electron>   ElectronCollT;
    std::vector<Muon>       MuonCollT;

    vector<Gen> gen_lep= GetGenLepronsSignal();
    if(IsSignal()){

      for(auto iel: ElectronCollTAll){
        bool matched_lep=false;
        for(auto igen : gen_lep){
          if(iel.DeltaR(igen) < 0.2) matched_lep=true;
        }
        if(matched_lep) ElectronCollT.push_back(iel);
      }

      for(auto iel: MuonCollTAll){
        bool matched_lep=false;
        for(auto igen :gen_lep){
          if(iel.DeltaR(igen) <0.2) matched_lep=true;
        }
        if(matched_lep)MuonCollT.push_back(iel);
      }

    }
    else{
      ElectronCollT = ElectronCollTAll;
      MuonCollT=MuonCollTAll;
    }


    //    TString OptionEl="CFHFakeNHConv";
    TString OptionEl="";
    TString OptionMu="";
    if(SeperateFakes) {
      OptionEl="HFake";
      OptionMu="HFake";
    }
    if(SeperateConv) {
      OptionEl = "NHConv";
      OptionMu="NHConv";
    }
    if(SeperateCF) {
      OptionEl = "CF";
      if(dilep_channel==MuMu) continue;
    }
    if(SeperatePrompt) {
      OptionEl="PromptNoCF";
      OptionMu="Prompt";
    }


    /// Select SS dilepotn events, [assing trigger and MVALoose ID
    std::vector<Lepton *> LepsAll  = (dilep_channel==EE) ? MakeLeptonPointerVector(ElectronCollT) : MakeLeptonPointerVector(MuonCollT);
    
    //if (!PassTriggerSelection(dilep_channel, ev, LepsAll, "Dilep")) continue;


    std::vector<Electron>   ElectronCollTSkim  = (IsSignal()) ? ElectronCollT :  SkimLepColl(ElectronCollT, param_bdt, OptionEl);
    std::vector<Muon>       MuonCollTSkim      = (IsSignal()) ? MuonCollT :  SkimLepColl(MuonCollT, param_bdt, OptionMu);


    std::vector<Lepton *> LepsT  = MakeLeptonPointerVector(MuonCollTSkim,ElectronCollTSkim);

    if(SameCharge(LepsAll)) {
      
      for(auto ilep : LepsT){
	int  lepType = ilep->LeptonGenType();

	if(ilep->LeptonFlavour() == Lepton::ELECTRON)       FillHist( "LepType/SSElectron", lepType ,weight, 14., -7., 7);
	else  FillHist( "LepType/SSMuon", lepType, weight, 14., -7., 7);
      }
    }

    for(auto ilep : LepsT) {
      int  lepType = ilep->LeptonGenType();
      double neg_w = (weight > 0) ? weight : 0.;
      if(ilep->LeptonFlavour() == Lepton::ELECTRON)       FillHist( "LepType/Electron", lepType ,neg_w, 14., -7., 7);
      else  FillHist( "LepType/Muon", lepType, neg_w, 14., -7., 7);
      
    }

    if(GetRatio) continue;

    //if(!CheckLeptonFlavourForChannel(dilep_channel,LepsAll)) return;

    std::vector<Jet>    AK4_JetAllColl = GetAllJets();

    if(weight < 0) continue;

    if(HasFlag("NoBJet") && SeperatePrompt){
      std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet",     param_bdt);
      if(AK4_BJetColl.size() > 0) return;
    }
    
    MakeTreeSS2L(dilep_channel, LepsT,MuonCollTSkim,ElectronCollTSkim, AK4_JetAllColl,  weight, "");

  }
  
}

void HNL_Lepton_ID_BDT_KinVar::MakeTreeSS2L(HNL_LeptonCore::Channel lep_channel,vector<Lepton *>  LepTColl, 
				   vector<Muon> Muons, 
				   vector<Electron> Electrons,
				   vector<Jet>& JetAllColl,
				   float weight, TString Label)
{
  

  int imu(0), iel(0);

  for(auto lep : LepTColl){
    
    InitializeIDTreeVars();
    
    float weight_lep = weight;


    TString lep_fake_tag= "";
    if(SeperateFakes) lep_fake_tag=  MatchGenDef(All_Gens, *lep);

    
    if(HasFlag("ByType")){
      if(SeperateFakes  && HasFlag("LF")){
	
	std::vector<Jet>    AK4_JetAllColl = GetAllJets();
	
	int JetHadFlavour = -999;
	int IdxMatchJet=-1;
	float mindR1=999.;
	
	for(unsigned int ij=0; ij<AK4_JetAllColl.size(); ij++){
	  float dR1=lep->DeltaR(AK4_JetAllColl.at(ij));
	  if(dR1>0.4) continue;
	  if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
	}
	
	if(IdxMatchJet!=-1)     JetHadFlavour = AK4_JetAllColl.at(IdxMatchJet).hadronFlavour();
	else JetHadFlavour=-1;
	
	
	if (JetHadFlavour != 0)  return;
	if(lep->LeptonFlavour() == Lepton::MUON) {
	  if (HasFlag("LFMother_1")  && lep_fake_tag!="q") return;
          if (HasFlag("LFMother_not1")  && lep_fake_tag=="q") return;
	  if (HasFlag("LFMother_2")  && !(lep_fake_tag=="K+" || lep_fake_tag=="pi+")) return;
	  if (HasFlag("LFMother_not2")  && (lep_fake_tag=="K+" || lep_fake_tag=="pi+")) return;
	  if (HasFlag("LFMother_3")  && !(lep_fake_tag=="g" )) return;
	  if (HasFlag("LFMother_not3")  && (lep_fake_tag=="g" )) return;
	}
	
	if(lep->LeptonFlavour() == Lepton::ELECTRON) {
	  if (HasFlag("LFMother_1")  && !(lep_fake_tag=="q" || lep_fake_tag.Contains("pi0")) ) return;
	  if (HasFlag("LFMother_not1")  && (lep_fake_tag=="q" || lep_fake_tag.Contains("pi0")) ) return;
	  if (HasFlag("LFMother_2")  && lep_fake_tag!="ph") return;
	  if (HasFlag("LFMother_not2")  && lep_fake_tag=="ph") return;
          if (HasFlag("LFMother_3")  && lep_fake_tag!="g") return;
          if (HasFlag("LFMother_not3")  && lep_fake_tag=="g") return;

	}
      }
      
      if(SeperateFakes && HasFlag("HFB")){
	std::vector<Jet>    AK4_JetAllColl = GetAllJets();
	
	int JetHadFlavour = -999;
	int IdxMatchJet=-1;
	float mindR1=999.;
	
	for(unsigned int ij=0; ij<AK4_JetAllColl.size(); ij++){
	  float dR1=lep->DeltaR(AK4_JetAllColl.at(ij));
	  if(dR1>0.4) continue;
	  if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
	}
	
	if(IdxMatchJet!=-1)     JetHadFlavour = AK4_JetAllColl.at(IdxMatchJet).hadronFlavour();
	else JetHadFlavour=-1;
	
	if (JetHadFlavour != 5)  return;
      }
      
      if(SeperateFakes && HasFlag("ByType") && HasFlag("HFC")){
	std::vector<Jet>    AK4_JetAllColl = GetAllJets();
	
	int JetHadFlavour = -999;
	int IdxMatchJet=-1;
	float mindR1=999.;
	
	for(unsigned int ij=0; ij<AK4_JetAllColl.size(); ij++){
	  float dR1=lep->DeltaR(AK4_JetAllColl.at(ij));
	  if(dR1>0.4) continue;
	  if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
	}
	
	if(IdxMatchJet!=-1)     JetHadFlavour = AK4_JetAllColl.at(IdxMatchJet).hadronFlavour();
	else JetHadFlavour=-1;
	
	if (JetHadFlavour != 4)  return;
	
	if (HasFlag("HFMother_D0") && lep_fake_tag!="D0") return;
	if (HasFlag("HFMother_Dplus") && lep_fake_tag!="D+") return;
	if (HasFlag("HFMother_D") && lep_fake_tag!="D") return;
      }
    }
    else {
      if(SeperateFakes && HasFlag("HFB")){
	std::vector<Jet>    AK4_JetAllColl = GetAllJets();
	
	int JetHadFlavour = -999;
	int IdxMatchJet=-1;
	float mindR1=999.;
	
	for(unsigned int ij=0; ij<AK4_JetAllColl.size(); ij++){
	  float dR1=lep->DeltaR(AK4_JetAllColl.at(ij));
	  if(dR1>0.4) continue;
	  if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
	}
	if(IdxMatchJet!=-1)     JetHadFlavour = AK4_JetAllColl.at(IdxMatchJet).hadronFlavour();
	else JetHadFlavour=-1;
	
	if (JetHadFlavour != 5)  return;
      }
      
      if(SeperateFakes && HasFlag("HFC")){
	std::vector<Jet>    AK4_JetAllColl = GetAllJets();
	
	int JetHadFlavour = -999;
	int IdxMatchJet=-1;
	float mindR1=999.;
	
	for(unsigned int ij=0; ij<AK4_JetAllColl.size(); ij++){
	  float dR1=lep->DeltaR(AK4_JetAllColl.at(ij));
	  if(dR1>0.4) continue;
	  if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
	}
	if(IdxMatchJet!=-1)     JetHadFlavour = AK4_JetAllColl.at(IdxMatchJet).hadronFlavour();
	else JetHadFlavour=-1;
	
	if (JetHadFlavour != 4)  return;
      }
      
      if(SeperateFakes && HasFlag("LF")){
	
	/// Remove some types of fakes based on SS  study
	
	if(lep->LeptonFlavour() == Lepton::MUON) {
	  if(MCSample.Contains("TT")){
	    
	    if(MatchGenDef(All_Gens, *lep) == "pi+"){
	      double r = ((double) rand() / (RAND_MAX));
	      if(r > 0.1) return;
	    }
	    if(MatchGenDef(All_Gens, *lep) == "electron"){
	      double r = ((double) rand() / (RAND_MAX));
	      if(r > 0.1) return;
	    }
	    if(MatchGenDef(All_Gens, *lep) == "K+"){
	      double r = ((double) rand() / (RAND_MAX));
	      if(r > 0.1) return;
	    }
	  }
	  else{
	    if(MatchGenDef(All_Gens, *lep) == "pi+"){
	      double r = ((double) rand() / (RAND_MAX));
	      if(r > 0.05) return;
	    }
	    if(MatchGenDef(All_Gens, *lep) == "electron") return;
	    if(MatchGenDef(All_Gens, *lep) == "K+") return;	
	  }
	}
	else{
	  
	  if(MCSample.Contains("TT")){
	    
	    if(MatchGenDef(All_Gens, *lep) == "pi+"){
	      double r = ((double) rand() / (RAND_MAX));
	      if(r > 0.1) return;
	    }
	    
	    if(MatchGenDef(All_Gens, *lep) == "tau"){
	      double r = ((double) rand() / (RAND_MAX));
	      if(r > 0.1) return;
	    }
	  }
	  else{
	    if(MatchGenDef(All_Gens, *lep) == "pi+"){
	      double r = ((double) rand() / (RAND_MAX));
	      if(r > 0.05) return;
	    }
	    
	    if(MatchGenDef(All_Gens, *lep) == "tau"){
	      double r = ((double) rand() / (RAND_MAX));
	      if(r > 0.2) return;
	    }
	    if(MatchGenDef(All_Gens, *lep) == "__"){
	      double r = ((double) rand() / (RAND_MAX));
	      if(r > 0.2) return;
	    }
	  }
	}
	
	
	std::vector<Jet>    AK4_JetAllColl = GetAllJets();
	
	int JetHadFlavour = -999;
	int IdxMatchJet=-1;
	float mindR1=999.;
	
	for(unsigned int ij=0; ij<AK4_JetAllColl.size(); ij++){
	  float dR1=lep->DeltaR(AK4_JetAllColl.at(ij));
	  if(dR1>0.4) continue;
	  if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
	}
	if(IdxMatchJet!=-1)     JetHadFlavour = AK4_JetAllColl.at(IdxMatchJet).hadronFlavour();
	else JetHadFlavour=-1;
	
	if (JetHadFlavour != 0)  return;
      }
    
      if(SeperateFakes && HasFlag("LFAll")){
	std::vector<Jet>    AK4_JetAllColl = GetAllJets();
	
	int JetHadFlavour = -999;
	int IdxMatchJet=-1;
	float mindR1=999.;
	
	for(unsigned int ij=0; ij<AK4_JetAllColl.size(); ij++){
	  float dR1=lep->DeltaR(AK4_JetAllColl.at(ij));
	  if(dR1>0.4) continue;
	  if(dR1<mindR1){ mindR1=dR1; IdxMatchJet=ij; }
	}
	if(IdxMatchJet!=-1)     JetHadFlavour = AK4_JetAllColl.at(IdxMatchJet).hadronFlavour();
	else JetHadFlavour=-1;
	
	if (JetHadFlavour != 0)  return;
      }
    }
    
    if(lep->LeptonFlavour() == Lepton::MUON) FillHist( "nPV_Muon", nPV ,weight, 100., 0., 100.);
    else FillHist( "nPV_Electron", nPV ,weight, 100., 0., 100.);


    SetBDTIDVar(lep);

    if(lep->LeptonFlavour() == Lepton::ELECTRON)  SetBDTIDVariablesElectron(Electrons[iel]);
    else SetBDTIDVariablesMuon(Muons[imu]);

    bdt_id_w_id_tot     = !IsDATA? weight_lep: 1.;

    if(lep->LeptonFlavour() == Lepton::MUON)     tree_mm->Fill();
    if(lep->LeptonFlavour() == Lepton::ELECTRON) tree_ee->Fill();

    if(lep->LeptonFlavour() == Lepton::ELECTRON) iel++;
    else imu++;

  }
  
}



void HNL_Lepton_ID_BDT_KinVar::executeEventFromParameter(AnalyzerParameter param){

  
}




HNL_Lepton_ID_BDT_KinVar::HNL_Lepton_ID_BDT_KinVar(){


  tree_mm = new TTree("Tree_mm", "Tree_mm");
  tree_ee = new TTree("Tree_ee", "Tree_ee");


  //tree_mm_IB = new TTree("Tree_mm_IB", "Tree_mm_IB");
  //t/r/ee_ee_IB = new TTree("Tree_ee_IB", "Tree_ee_IB");

  //  tree_mm_OB = new TTree("Tree_mm_OB", "Tree_mm_OB");
  //tree_ee_OB = new TTree("Tree_ee_OB", "Tree_ee_OB");


  //t//ree_mm_EC = new TTree("Tree_mm_EC", "Tree_mm_EC");
  //tree_ee_EC = new TTree("Tree_ee_EC", "Tree_ee_EC");


}


HNL_Lepton_ID_BDT_KinVar::~HNL_Lepton_ID_BDT_KinVar(){

  delete tree_mm;
  delete tree_ee;


  //delete tree_mm_IB;
  //delete tree_ee_IB;

  // delete tree_mm_OB;
  // delete tree_ee_OB;

  //delete tree_mm_EC;
  //delete tree_ee_EC;
}


void HNL_Lepton_ID_BDT_KinVar::WriteHist(){

  AnalyzerCore::WriteHist();

  outfile->cd();

  tree_mm->Write();
  tree_ee->Write();

  outfile->cd();

}
