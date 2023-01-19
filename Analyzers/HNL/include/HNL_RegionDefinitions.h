#ifndef HNL_RegionDefinitions_h
#define HNL_RegionDefinitions_h

#include "TRandom.h"

#include "HNL_LeptonCore.h"

class HNL_RegionDefinitions : public HNL_LeptonCore {

 public:

  HNL_RegionDefinitions();
  ~HNL_RegionDefinitions();

  // analyis functions to run SR
  
			
  void   RunAllSignalRegions(HNL_LeptonCore::ChargeType q, std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Tau> taus,  std::vector<Jet> loose_jets,  std::vector<Jet> jets_all, std::vector<Jet> jets, std::vector<Jet> jets_vbf, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param,  float w);


  void RunAllControlRegions(std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Jet> jets, std::vector<Jet> jets_vbf, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param,  float weight_mm );


  /// PRESEL 
  bool  PassPreselection(HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,std::vector<Tau> TauColl,  std::vector<Jet> jets, std::vector<Jet> vbf_jets,std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, TString channel_string ,  float w);


  //// SIGNAL REGION DEFINITIONS

  bool  RunSignalRegionAK8(HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Tau> Taus, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param, TString channel_string ,  float w);
  
  TString  RunSignalRegionAK8String(HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Tau> Taus, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param, TString channel_string ,  float w);


  
  bool RunSignalRegionWW(HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,  std::vector<Tau> Taus, std::vector<Jet> loose_jets, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param, TString channel_string ,  float w);
  TString RunSignalRegionWWString(HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,  std::vector<Tau> Taus, std::vector<Jet> loose_jets, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param, TString channel_string ,  float w);


  bool  RunSignalRegionAK4(HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,  std::vector<Tau> Taus, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param, TString channel_string ,  float w);

  TString  RunSignalRegionAK4String(HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,  std::vector<Tau> Taus, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param, TString channel_string ,  float w);
  
  TString RunSignalRegionAK4StringBDT(TString mN, TString NCut, TString NTree, HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *> LepTCol,   std::vector<Lepton*> leps_veto, std::vector<Tau> TauColl, std::vector<Jet> JetAllColl, std::vector<Jet> JetColl, std::vector<Jet> JetVBFColl, std::vector<Jet> B_JetColl, Event ev, Particle METv, AnalyzerParameter param, TString PostLabel ,  float w);


  bool RunSignalRegionTrilepton(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param, TString channel_string ,  float w);

 // bool RunElectronChannelSR4(HNL_LeptonCore::Channel channel ,std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param,  TString channel_string ,float w);

  bool PassVBF(vector<Jet>  jets,std::vector<Lepton *> leps1, double mjj_cut,bool use_leadjet=false);
  bool PassVBFInitial(vector<Jet>  jets);
  

  // CR



  //void RunMuonChannelCR(std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Jet> jets, std::vector<Jet> jets_vbf, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param,  float weight_mm );
  //void RunElectronChannelCR(std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Jet> jets,std::vector<Jet> jets_vbf, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param,  float weight_mm );


  bool FillOSPreselectionPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto , std::vector<Jet> jets_vbf, std::vector<FatJet> fatjets, std::vector< Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillSSPreselectionPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto , std::vector<Jet> jets_vbf, std::vector<FatJet> fatjets,  std::vector< Jet> bjets,Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillSSVBFPreselectionPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto , std::vector<Jet> jets_vbf, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);


  bool FillZZCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillZZ2CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillZCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillZNPCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillTopCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWWCR1Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWWCR2Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWWCRNPPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWWCRNP2Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWWCRNP3Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillWZCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWZ2CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWZBCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillZGCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWGCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillHighMassSR1CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillHighMassSR2CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillHighMassSR3CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillHighMass1JetCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillHighMassBJetCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillHighMassNPCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);







};



#endif

