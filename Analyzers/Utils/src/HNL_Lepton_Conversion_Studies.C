#include "HNL_Lepton_Conversion_Studies.h"

void HNL_Lepton_Conversion_Studies::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

}

void HNL_Lepton_Conversion_Studies::ProcessLeptonCategory(const std::string& s_labelPrefix,
							  std::vector<Lepton*>& leptons,
							  std::vector<Tau>& TauColl_Uncleaned,
							  std::vector<Jet>& AK4_JetColl,
							  std::vector<FatJet>& AK8_JetColl,
							  Particle& METv,
							  int nPV,
							  AnalyzerParameter& param,
							  double PhotonPt,
							  double weight) {

  TString labelPrefix = TString(s_labelPrefix);
  
  if (leptons.size() < 2) return;
  if (leptons[1]->Pt() < 15) return;
  if (leptons[0]->Pt() < 25) return;

  if (leptons.size() == 2)
    Fill_RegionPlots(param, labelPrefix + "/Conv2L", TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, leptons, METv, nPV, weight);

  if (leptons.size() == 3)
    Fill_RegionPlots(param, labelPrefix + "/Conv3L", TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, leptons, METv, nPV, weight);

  if (leptons.size() == 2) {
    if (SameCharge(leptons)) {
      Fill_RegionPlots(param, labelPrefix + "/ConvSS", TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, leptons, METv, nPV, weight);
      FillHist(labelPrefix + "/SS_Photon_Status23", PhotonPt, weight, 500, 0, 1000);
      //      FillHist(labelPrefix + "/SS_Photon_Status23_Mass", PhotonPt, weight, 500, 0, 1000);
	    
    } else {
      FillHist(labelPrefix + "/OS_Photon_Status23", PhotonPt, weight, 500, 0, 1000);
      Fill_RegionPlots(param, labelPrefix + "/ConvOS", TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, leptons, METv, nPV, weight);
    }
  }
}

void HNL_Lepton_Conversion_Studies::executeEvent(){


 
  Event ev = GetEvent();

  AnalyzerParameter param = HNL_LeptonCore::InitialiseHNLParameter("HNL_ULIDv2");
  double weight =SetupWeight(ev,param);

  if(_jentry < 100) PrintGen(All_Gens);

  double PhotonPt=0;
  double PhotonMass=0;
  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens.at(i);
    if(gen.PID() == 22 && gen.Status()==23)   PhotonPt=gen.Pt();
    if(gen.PID() == 22 && gen.Status()==23)  FillHist( ("Photon_Status23"), gen.Pt(), weight, 500, 0, 1000);
    if(gen.PID() == 22 && gen.Status()==1)  FillHist( ("Photon_Status1"), gen.Pt(), weight, 500, 0, 1000);

    if(gen.PID() == 22 && gen.Status()==23)   PhotonMass=gen.M();
    if(gen.PID() == 22 && gen.Status()==23)  FillHist( ("Photon_Status23_mass"), gen.M(), weight, 500, 0, 1000);
    if(gen.PID() == 22 && gen.Status()==1)  FillHist( ("Photon_Status1_mass"), gen.M(), weight, 500, 0, 1000);

    
  }

  double PhotonPt_with_Status1=0;
    
  /// Check Photon code
  bool checkPhoton = true;
  
  if (checkPhoton) {
    int NearPhotonIdx = -1;
    
    // Step 1: Look for status 23 photon
    for (unsigned int i = 2; i < All_Gens.size(); ++i) {
      const Gen& gen = All_Gens[i];
      if (gen.MotherIndex() < 0) continue;
      if (gen.PID() == 22 && gen.Status() == 23) {
	NearPhotonIdx = static_cast<int>(i);
	break; // Found status 23 photon, use first one
      }
    }
    
    // Step 2: Fallback to highest-pt status 1 prompt photon
    if (NearPhotonIdx < 0) {
      double maxPt = 0;
      for (unsigned int i = 2; i < All_Gens.size(); ++i) {
	const Gen& gen = All_Gens[i];
	if (gen.MotherIndex() < 0) continue;
	if (gen.PID() != 22 || gen.Status() != 1) continue;
	if (gen.isPromptFinalState() && gen.Pt() > maxPt) {
	  NearPhotonIdx = static_cast<int>(i);
	  maxPt = gen.Pt();
	}
      }
    }

    // Step 3: Comparison
    if (NearPhotonIdx >= 0) {
      double phPt = All_Gens[NearPhotonIdx].Pt();

      PhotonPt_with_Status1=phPt;
      
      if (std::abs(phPt - PhotonPt) > 1e-3) { // Use epsilon for float comparison
	std::cout << "[PhotonCheck] Photon mismatch: PhotonPt = " << PhotonPt
		  << ", FoundGenPt = " << phPt << std::endl;
	PrintGen(All_Gens);
      }
    } else {
      std::cout << "[PhotonCheck] No suitable photon found in gen collection." << std::endl;
    }
  }
  

  if (HasFlag("AltMC")) {
    if (MCSample.Contains("ZG")) {
      if (MCSample == "ZGToLLG_PtG_130") {
	if (PhotonPt < 130) return;
      } else {
	if (PhotonPt > 130) return;
      }
    } 
    else if (MCSample.Contains("WG")) {
      if (MCSample == "WGToLNuG_01J_PtG_500") {
	if (PhotonPt < 500) return;
      } 
      else if (MCSample == "WGToLNuG_01J_PtG_300") {
	if (PhotonPt < 300 || PhotonPt > 500) return;
      } 
      else if (MCSample == "WGToLNuG_01J_PtG_130") {
	if (PhotonPt < 130 || PhotonPt > 300) return;
      } 
      else {
	if (PhotonPt > 130) return;
      }
    }
  } 
  else {
    if (MCSample.Contains("PtG")) return;
  }
  
  std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, 15., 2.5);
  std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID, 10., 2.4);

  
  std::vector<Electron>   ElectronCollT = GetElectrons(param.Electron_Tight_ID, 15., 2.5);
  std::vector<Muon>       MuonCollT     = GetMuons    (param.Muon_Tight_ID, 10., 2.4);
  
  TString PlotDir="Inclusive";

  std::vector<Lepton *> Veto_Leptons  = MakeLeptonPointerVector(MuonCollV,ElectronCollV,param);
  std::vector<Lepton *> Tight_Leptons   = MakeLeptonPointerVector(MuonCollT,ElectronCollT,param);
  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);

  std::vector<Tau>   TauColl_Uncleaned  = SelectTaus   (Veto_Leptons,"JetT_MuT_ELT",20., 2.3);

  Particle METv = GetvMET("PuppiT1xyULCorr", param, MuonCollT, ElectronCollT); // returns MET with systematic correction; run this afte 
  ProcessLeptonCategory("VetoID", Veto_Leptons, TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, METv, nPV,  param, PhotonPt, weight);
  ProcessLeptonCategory("TightID", Tight_Leptons, TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, METv, nPV, param, PhotonPt, weight);
  if(Veto_Leptons.size()==2) ProcessLeptonCategory("TightID_Cleaned", Tight_Leptons, TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, METv, nPV, param, PhotonPt, weight);
  if(Veto_Leptons.size()==2) ProcessLeptonCategory("TightID_Cleaned_PhStatus1", Tight_Leptons, TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, METv, nPV, param, PhotonPt_with_Status1, weight);
  
  return;
  
  std::map< TString, bool > map_Region_to_Bool;
  map_Region_to_Bool.clear();

  
  int nT=0;
  int nL_Conv=0;
  for(unsigned int ilep=0; ilep < ElectronCollV.size(); ilep++) {
    
    if(ElectronCollV[ilep].PassID(param.Electron_Tight_ID))nT++;
    else if( (ElectronCollV.at(ilep).MVAKey("Conv_EDv5") < 0) && (ElectronCollV.at(ilep).MVAKey("Fake_EDv5") > 0) && (ElectronCollV.at(ilep).MVAKey("CF_EDv5") > 0.6)) nL_Conv++;
    
  }
  for(unsigned int ilep=0; ilep < MuonCollV.size(); ilep++) {
    if(MuonCollV[ilep].PassID(param.Muon_Tight_ID))nT++;

  }

  TString Type = "";
  for(unsigned int ilep=0; ilep < Tight_Leptons.size(); ilep++) {
    if(Tight_Leptons[ilep]->LeptonIsCF()) Type+="CF";
    else if(Tight_Leptons[ilep]->IsFake())  Type+="Fake";
    else if(Tight_Leptons[ilep]->IsConv()) Type+="Conv";
    else if(Tight_Leptons[ilep]->IsPrompt())  Type+="Prompt";
  }

  if(!SameCharge(Veto_Leptons))  return;

  TString EvType = "";
  if(Type=="PromptPrompt") EvType = "Prompt";
  else if(Type.Contains("Fake")) EvType = "Fake";
  else if(Type.Contains("Conv")) EvType = "Conv";
  else EvType = "CF";

  if(IsData) EvType = "Data";

  map_Region_to_Bool.clear();
  map_Region_to_Bool["Inclusive"] = true;
  map_Region_to_Bool["ConvSB"] = (nT==1 && nL_Conv ==1 && ElectronCollV.size()==2) ;
  map_Region_to_Bool["ConvSBEMu"] = (nT==1 && nL_Conv ==1 && ElectronCollV.size()==1) ;
  map_Region_to_Bool["TT"] = (nT==2 && ElectronCollV.size()==2);
  map_Region_to_Bool["TTEMu"] = (nT==2 && ElectronCollV.size()==1);

  for(std::map< TString, bool >::iterator it = map_Region_to_Bool.begin(); it != map_Region_to_Bool.end(); it++){
    
    if(it->second){
    
      Particle LL = *Tight_Leptons[0] + *Tight_Leptons[1];
      
      PlotDir= it->first + "/"+EvType;
      
      FillHist( (PlotDir + "/LL/mass_ll").Data(), LL.M(),weight, 250, 0, 500);

      double ptbins[8] = { 0., 10.,15., 20.,25.,30., 40., 100.};
      
      for(unsigned int ilep=0; ilep < Tight_Leptons.size(); ilep++){
	
	double lpt = (Tight_Leptons.at(ilep)->Pt() > 100.) ? 99.: Tight_Leptons.at(ilep)->Pt();
	TString SFlav = Tight_Leptons.at(ilep)->sLepGenType();
	
	if(Tight_Leptons.at(ilep)->GetFlavour() == "Electron") FillHist( (PlotDir + "/EE_LL/lep_conv").Data(), Tight_Leptons.at(ilep)->MVAKey("Conv_EDv5"),weight, 100, -1, 1);
	if(Tight_Leptons.at(ilep)->GetFlavour() == "Electron")FillHist( (PlotDir + "/EE_LL/lep_cf").Data(), Tight_Leptons.at(ilep)->MVAKey("CF_EDv5"), weight,100,-1, 1);
	if(Tight_Leptons.at(ilep)->GetFlavour() == "Electron")FillHist( (PlotDir + "/EE_LL/lep_fake").Data(), Tight_Leptons.at(ilep)->MVAKey("Fake_EDv5"), weight,100,-1, 1);
	
	FillHist( (PlotDir + "/EE_LL/lep_pt").Data(),lpt, weight, 7, ptbins);
	if(nT>=1)         FillHist( (PlotDir + "/EE_TL/lep_pt").Data(),lpt, weight, 7, ptbins);
	if(nT==2)         FillHist( (PlotDir + "/EE_TT/lep_pt").Data(),lpt, weight, 7, ptbins);

      }
    }
  }
  
}




HNL_Lepton_Conversion_Studies::HNL_Lepton_Conversion_Studies(){


}
 
HNL_Lepton_Conversion_Studies::~HNL_Lepton_Conversion_Studies(){

}
