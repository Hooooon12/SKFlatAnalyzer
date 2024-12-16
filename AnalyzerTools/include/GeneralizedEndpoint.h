#ifndef ScaledPts_h
#define ScaledPts_h

struct ScaledPts {
  double ScaledPt, ScaledPt_Up, ScaledPt_Down;
};

#endif


#ifndef GeneralizedEndpoint_h
#define GeneralizedEndpoint_h

#include "TString.h"
#include <iostream>
#include <map>

//==== https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonReferenceSelectionAndCalibrationsRun2
//==== https://github.com/cms-analysis/SUSYBSMAnalysis-Zprime2muAnalysis/blob/mini-AOD/src/GeneralizedEndpoint.cc

using namespace std;

class GeneralizedEndpoint{

public:
  GeneralizedEndpoint();
  virtual ~GeneralizedEndpoint();

  ScaledPts GeneralizedEndpointPt(TString Era, float MuonPt, int MuonCharge, float MuonEta, float MuonPhi, int seed);

private:
  std::map<TString,std::map<int,std::map<int,float> > > _Correction;
  std::map<TString,std::map<int,std::map<int,float> > > _CorrectionError;   

};

#endif
