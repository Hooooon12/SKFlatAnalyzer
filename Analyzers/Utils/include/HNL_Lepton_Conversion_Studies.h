#ifndef HNL_Lepton_Conversion_Studies_h
#define HNL_Lepton_Conversion_Studies_h

#include "HNL_RegionDefinitions.h"

class HNL_Lepton_Conversion_Studies : public HNL_RegionDefinitions {

 public:


  void initializeAnalyzer();
  void executeEvent();

  HNL_Lepton_Conversion_Studies();
  ~HNL_Lepton_Conversion_Studies();

  void ProcessLeptonCategory( const std::string& labelPrefix,
			      std::vector<Lepton*>& leptons,
			      std::vector<Tau>& TauColl_Uncleaned,
			      std::vector<Jet>& AK4_JetColl,
			      std::vector<FatJet>& AK8_JetColl,
			      Particle& METv,
			      int nPV,
			      AnalyzerParameter& param,
			      double PhotonPt,
			      double weight) ;




};



#endif
