#ifndef HNL_RegionDefinitions_h
#define HNL_RegionDefinitions_h

#include "TRandom.h"

#include "HNL_LeptonCore.h"

class HNL_RegionDefinitions : public HNL_LeptonCore {

 public:

  HNL_RegionDefinitions();
  ~HNL_RegionDefinitions();

  // analyis functions to run SR
  
  void   RunAllSignalRegions(HNL_LeptonCore::ChargeType q, std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Tau> taus, std::vector<Jet> loose_jets,  std::vector<Jet> jets, std::vector<Jet> jets_vbf, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param, int nElForRunCF,  float w);


  void RunAllControlRegions(std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Jet> loose_jets, std::vector<Jet> jets,  std::vector<Jet> jets_vbf, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param,  vector<TString> CRs,int nElForRunCF, float weight);

  bool RunCR(TString CRName , vector<TString> CRs) ;


  void   RunMainRegionCode(bool isSR, HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,std::vector<Tau> TauColl, std::vector<Jet> loose_jets, std::vector<Jet> jets,  std::vector<Jet> vbf_jets,std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param,  float w);


  /// PRESEL 
  bool  PassPreselection(bool isSR,HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,std::vector<Tau> TauColl,  std::vector<Jet> jets, std::vector<Jet> vbf_jets,std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param,  float w);


  //// SIGNAL REGION DEFINITIONS

  bool  RunSignalRegionAK8(bool ApplyForSR,HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Tau> Taus, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param,   float w);
  
  TString  RunSignalRegionAK8String(bool ApplyForSR,HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Tau> Taus, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param,   float w);


  
  bool RunSignalRegionWW(bool ApplyForSR,HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,  std::vector<Tau> Taus,std::vector<Jet> loose_jets,  std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param,   float w);
  TString RunSignalRegionWWString(bool ApplyForSR,HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,  std::vector<Tau> Taus, std::vector<Jet> loose_jets,std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param,   float w);


  bool  RunSignalRegionAK4(bool ApplyForSR,HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,  std::vector<Tau> Taus, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param,   float w);

  TString  RunSignalRegionAK4String(bool ApplyForSR,HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType q, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,  std::vector<Tau> Taus, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev,  Particle METv, AnalyzerParameter param,   float w);
  

  TString RunSignalRegionAK4StringBDT(bool ApplyForSR, TString mN, TString NCut, TString NTree, HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *> LepTCol,  std::vector<Jet> jets,std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param,   float w);


  bool PassVBF(vector<Jet>  jets,std::vector<Lepton *> leps1, double mjj_cut,bool use_leadjet=false);
  bool PassVBFInitial(vector<Jet>  jets);
  

  // CR


  //void RunMuonChannelCR(std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Jet> jets, std::vector<Jet> jets_vbf, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param,  float weight_mm );
  //void RunElectronChannelCR(std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Jet> jets,std::vector<Jet> jets_vbf, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param,  float weight_mm );


  bool FillSSPreselectionPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto , std::vector<Jet> jets_vbf, std::vector<FatJet> fatjets,  std::vector< Jet> bjets,Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillSSVBFPreselectionPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto , std::vector<Jet> jets_vbf, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);


  bool FillZZCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillZZVBFCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillZZ2CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillZCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillZAK8CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillZ_MuonNPCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillZ_ElNPCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillTopNPCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps, std::vector<Lepton *> leps_veto   , std::vector<Jet> JetColl, std::vector<FatJet> AK8_JetColl, std::vector<Jet> B_JetColl,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillTopCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillTopCR2Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillTopAK8CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWWCR1Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWWCR2Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWWCRNPPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWWCRNP2Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWWCRNP3Plots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillWZCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets, Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWZVBFCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWZVBF2CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWZBCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillZGCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillWGCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillHighMassSR1CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillHighMassSR2CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jetsall,std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillHighMassSR3CRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillHighMassSR3BDTCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto,  std::vector<Jet> JetColl, std::vector<Jet> JetVBFColl, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);

  bool FillSSZPeakCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<Jet> jets_vbf, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillHighMass1JetCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillHighMassBJetCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);
  bool FillHighMassNPCRPlots(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps,std::vector<Lepton *> leps_veto, std::vector<Jet> jets, std::vector<FatJet>  fatjets, std::vector<Jet> bjets,  Event ev, Particle METv, AnalyzerParameter param, float w);




  void RunSR3BDT(HNL_LeptonCore::ChargeType qq, std::vector<Electron> electrons, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto,  std::vector<Tau> TauColl,  std::vector<Jet> JetColl, std::vector<Jet> VBF_JetColl,std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, Event ev,   Particle METv, AnalyzerParameter param,   float weight_ll);


  bool RunTopID;
  bool RunPOGID;
  bool RunHighPtID;
  bool RunPekingID;
  

};



#endif

