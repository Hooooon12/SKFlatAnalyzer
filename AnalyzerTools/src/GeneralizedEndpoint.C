#include "GeneralizedEndpoint.h"
#include <iostream>
#include "TRandom.h"
#include <math.h>
using std::cout;
using std::cerr;
using std::endl;

GeneralizedEndpoint::GeneralizedEndpoint(){
  // Corrections from 2D matrix in MuonPOG presentation in c/TeV.
  // e.g. See https://twiki.cern.ch/twiki/bin/view/CMS/MuonUL2016#High_pT_above_120_GeV
  // [-2.4, -2.1]
  _Correction["2016a"][0][0] = -0.075; _CorrectionError["2016a"][0][0] = 0.088; // [-180, -60]
  _Correction["2016a"][0][1] = -0.049; _CorrectionError["2016a"][0][1] = 0.101; // [-60, 60]
  _Correction["2016a"][0][2] = -0.007; _CorrectionError["2016a"][0][2] = 0.060; // [60, 180]
  // [-2.1, -1.2]
  _Correction["2016a"][1][0] = -0.053; _CorrectionError["2016a"][1][0] = 0.055;
  _Correction["2016a"][1][1] = -0.100; _CorrectionError["2016a"][1][1] = 0.040;
  _Correction["2016a"][1][2] =  0.003; _CorrectionError["2016a"][1][2] = 0.038;
  // [-1.2, 0]
  _Correction["2016a"][2][0] = -0.020; _CorrectionError["2016a"][2][0] = 0.029;
  _Correction["2016a"][2][1] =  0.072; _CorrectionError["2016a"][2][1] = 0.030;
  _Correction["2016a"][2][2] = -0.015; _CorrectionError["2016a"][2][2] = 0.030;
  // [0, 1.2]
  _Correction["2016a"][3][0] = -0.009; _CorrectionError["2016a"][3][0] = 0.029;
  _Correction["2016a"][3][1] =  0.057; _CorrectionError["2016a"][3][1] = 0.034;
  _Correction["2016a"][3][2] = -0.003; _CorrectionError["2016a"][3][2] = 0.036;
  // [1.2, 2.1]
  _Correction["2016a"][4][0] = -0.010; _CorrectionError["2016a"][4][0] = 0.040;
  _Correction["2016a"][4][1] = -0.036; _CorrectionError["2016a"][4][1] = 0.043;
  _Correction["2016a"][4][2] =  0.060; _CorrectionError["2016a"][4][2] = 0.040;
  // [2.1, 2.4]
  _Correction["2016a"][5][0] =  0.078; _CorrectionError["2016a"][5][0] = 0.067;
  _Correction["2016a"][5][1] = -0.073; _CorrectionError["2016a"][5][1] = 0.102;
  _Correction["2016a"][5][2] = -0.196; _CorrectionError["2016a"][5][2] = 0.084;

  // [-2.4, -2.1]
  _Correction["2016b"][0][0] =  0.002; _CorrectionError["2016b"][0][0] = 0.078; // [-180, -60]
  _Correction["2016b"][0][1] = -0.005; _CorrectionError["2016b"][0][1] = 0.089; // [-60, 60]
  _Correction["2016b"][0][2] =  0.026; _CorrectionError["2016b"][0][2] = 0.086; // [60, 180]
  // [-2.1, -1.2]
  _Correction["2016b"][1][0] =  0.049; _CorrectionError["2016b"][1][0] = 0.046;
  _Correction["2016b"][1][1] = -0.064; _CorrectionError["2016b"][1][1] = 0.039;
  _Correction["2016b"][1][2] = -0.142; _CorrectionError["2016b"][1][2] = 0.046;
  // [-1.2, 0]
  _Correction["2016b"][2][0] = -0.045; _CorrectionError["2016b"][2][0] = 0.033;
  _Correction["2016b"][2][1] =  0.023; _CorrectionError["2016b"][2][1] = 0.032;
  _Correction["2016b"][2][2] =  0.039; _CorrectionError["2016b"][2][2] = 0.031;
  // [0, 1.2]
  _Correction["2016b"][3][0] = -0.061; _CorrectionError["2016b"][3][0] = 0.033;
  _Correction["2016b"][3][1] = -0.004; _CorrectionError["2016b"][3][1] = 0.031;
  _Correction["2016b"][3][2] =  0.000; _CorrectionError["2016b"][3][2] = 0.032;
  // [1.2, 2.1]
  _Correction["2016b"][4][0] =  0.016; _CorrectionError["2016b"][4][0] = 0.043;
  _Correction["2016b"][4][1] =  0.041; _CorrectionError["2016b"][4][1] = 0.045;
  _Correction["2016b"][4][2] =  0.003; _CorrectionError["2016b"][4][2] = 0.042;
  // [2.1, 2.4]
  _Correction["2016b"][5][0] = -0.108; _CorrectionError["2016b"][5][0] = 0.109;
  _Correction["2016b"][5][1] = -0.023; _CorrectionError["2016b"][5][1] = 0.114;
  _Correction["2016b"][5][2] = -0.091; _CorrectionError["2016b"][5][2] = 0.073;

  // [-2.4, -2.1]
  _Correction["2017"][0][0] = -0.049; _CorrectionError["2017"][0][0] = 0.070; // [-180, -60]
  _Correction["2017"][0][1] = -0.048; _CorrectionError["2017"][0][1] = 0.044; // [-60, 60]
  _Correction["2017"][0][2] = -0.025; _CorrectionError["2017"][0][2] = 0.044; // [60, 180]
  // [-2.1, -1.2]
  _Correction["2017"][1][0] = -0.032; _CorrectionError["2017"][1][0] = 0.029;
  _Correction["2017"][1][1] =  0.001; _CorrectionError["2017"][1][1] = 0.031;
  _Correction["2017"][1][2] = -0.019; _CorrectionError["2017"][1][2] = 0.030;
  // [-1.2, 0]
  _Correction["2017"][2][0] =  0.027; _CorrectionError["2017"][2][0] = 0.024;
  _Correction["2017"][2][1] =  0.039; _CorrectionError["2017"][2][1] = 0.027;
  _Correction["2017"][2][2] = -0.049; _CorrectionError["2017"][2][2] = 0.021;
  // [0, 1.2]
  _Correction["2017"][3][0] = -0.026; _CorrectionError["2017"][3][0] = 0.021;
  _Correction["2017"][3][1] =  0.027; _CorrectionError["2017"][3][1] = 0.022;
  _Correction["2017"][3][2] =  0.018; _CorrectionError["2017"][3][2] = 0.025;
  // [1.2, 2.1]
  _Correction["2017"][4][0] =  0.012; _CorrectionError["2017"][4][0] = 0.025;
  _Correction["2017"][4][1] = -0.007; _CorrectionError["2017"][4][1] = 0.023;
  _Correction["2017"][4][2] =  0.000; _CorrectionError["2017"][4][2] = 0.026;
  // [2.1, 2.4]
  _Correction["2017"][5][0] = -0.151; _CorrectionError["2017"][5][0] = 0.087;
  _Correction["2017"][5][1] =  0.058; _CorrectionError["2017"][5][1] = 0.057;
  _Correction["2017"][5][2] =  0.004; _CorrectionError["2017"][5][2] = 0.063;

  // [-2.4, -2.1]
  _Correction["2018"][0][0] = -0.089; _CorrectionError["2018"][0][0] = 0.050;
  _Correction["2018"][0][1] =  0.057; _CorrectionError["2018"][0][1] = 0.052;
  _Correction["2018"][0][2] =  0.149; _CorrectionError["2018"][0][2] = 0.057;
  // [-2.1, -1.2]
  _Correction["2018"][1][0] = -0.005; _CorrectionError["2018"][1][0] = 0.025;
  _Correction["2018"][1][1] = -0.028; _CorrectionError["2018"][1][1] = 0.025;
  _Correction["2018"][1][2] =  0.021; _CorrectionError["2018"][1][2] = 0.021;
  // [-1.2, 0]
  _Correction["2018"][2][0] = -0.014; _CorrectionError["2018"][2][0] = 0.018;
  _Correction["2018"][2][1] = -0.009; _CorrectionError["2018"][2][1] = 0.017;
  _Correction["2018"][2][2] = -0.028; _CorrectionError["2018"][2][2] = 0.017;
  // [0, 1.2]
  _Correction["2018"][3][0] =  0.020; _CorrectionError["2018"][3][0] = 0.017;
  _Correction["2018"][3][1] =  0.007; _CorrectionError["2018"][3][1] = 0.019;
  _Correction["2018"][3][2] =  0.000; _CorrectionError["2018"][3][2] = 0.020;
  // [1.2, 2.1]
  _Correction["2018"][4][0] =  0.042; _CorrectionError["2018"][4][0] = 0.031;
  _Correction["2018"][4][1] = -0.068; _CorrectionError["2018"][4][1] = 0.027;
  _Correction["2018"][4][2] = -0.059; _CorrectionError["2018"][4][2] = 0.027;
  // [2.1, 2.4]
  _Correction["2018"][5][0] = -0.012; _CorrectionError["2018"][5][0] = 0.044;
  _Correction["2018"][5][1] = -0.235; _CorrectionError["2018"][5][1] = 0.054;
  _Correction["2018"][5][2] =  0.025; _CorrectionError["2018"][5][2] = 0.039;

};

GeneralizedEndpoint::~GeneralizedEndpoint()
{
};

ScaledPts GeneralizedEndpoint::GeneralizedEndpointPt(TString Era, float MuonPt, int MuonCharge, float MuonEta, float MuonPhi, int seed){

  ScaledPts out;
  out.ScaledPt = MuonPt;
  out.ScaledPt_Up = MuonPt;
  out.ScaledPt_Down = MuonPt;

  //Check the input format.
  if (fabs(MuonEta)>2.4) {
    //cerr<<"ERROR: MuonEta = "<< MuonEta << ", outisde valide range = [-2.4,2.4] "<<endl;
    return out;
  }
  if (fabs(MuonPhi)>180) {
    //cerr<<"ERROR: MuonPhi = "<< MuonPhi << ", outisde valide range = [-180,180] "<<endl;
    return out;
  }
  if (MuonCharge != 1 && MuonCharge != -1) {
    //cerr<<"ERROR: Invalide Muon Charge = "<< MuonCharge << endl;
    return out;
  }

  // Eta Binning
  unsigned int etaBINS = 6;
  unsigned int kEtaBin = etaBINS;
  double EtaBin[etaBINS+1];
  EtaBin[0]=-2.4; EtaBin[1]=-2.1; EtaBin[2]=-1.2; EtaBin[3]=0.;
  EtaBin[4]=1.2; EtaBin[5]=2.1; EtaBin[6]=2.4;  

  // Phi Binning.
  unsigned int phiBINS =3;
  unsigned int kPhiBin = phiBINS;
  double PhiBin[phiBINS+1];
  PhiBin[0]=-180.; PhiBin[1]=-60.; PhiBin[2]=60.; PhiBin[3]=180.;
  
  for (unsigned int kbin=0; kbin<=etaBINS; ++kbin) {
    if (MuonEta<EtaBin[kbin+1]) {
      kEtaBin = kbin;
      break;
    }
  }

  for (unsigned int kbin=0; kbin<=phiBINS; ++kbin) {
    if (MuonPhi<PhiBin[kbin+1]) {
      kPhiBin = kbin;
      break;
    }
  }

  float KappaBias=_Correction[Era][kEtaBin][kPhiBin];
  float KappaBiasError=_CorrectionError[Era][kEtaBin][kPhiBin];
  
  float rnd = KappaBias+99*KappaBiasError;
  gRandom->SetSeed(seed);
  while (abs(KappaBias-rnd) > KappaBiasError)
    rnd = gRandom->Gaus(KappaBias,KappaBiasError);

  KappaBias = rnd;
  // if ((KappaBias-KappaBiasError)/KappaBias > 0.70) return MuonPt;
  //  if (abs(KappaBiasError/KappaBias) > 0.70) return MuonPt; // ignore the bias if the error on the estimation is too big... 
  
  MuonPt = MuonPt/1000.; //GeV to TeV.
  MuonPt = MuonCharge*fabs(MuonPt); //Signed Pt.
  MuonPt = 1/MuonPt; //Convert to Curvature.
  MuonPt = MuonPt + KappaBias; //Apply the bias.

  double MuonPt_Central = MuonPt + KappaBias;
  double MuonPt_Up = MuonPt + KappaBias + KappaBiasError;
  double MuonPt_Down = MuonPt + KappaBias - KappaBiasError;

  if (fabs(MuonPt_Central) < 0.14) MuonPt_Central = KappaBiasError; //To avoid a division by set the curvature to its error if after the correction the pt is larger than 7 TeV.
  MuonPt_Central = 1/MuonPt_Central;//Return to Pt.
  MuonPt_Central = fabs(MuonPt_Central);//returns unsigned Pt, any possible sign flip due to the curvature is absorbed here.
  MuonPt_Central = MuonPt_Central*1000.;//Return to Pt in GeV.

  if (fabs(MuonPt_Up) < 0.14) MuonPt_Up = KappaBiasError; //To avoid a division by set the curvature to its error if after the correction the pt is larger than 7 TeV.
  MuonPt_Up = 1/MuonPt_Up;//Return to Pt.
  MuonPt_Up = fabs(MuonPt_Up);//returns unsigned Pt, any possible sign flip due to the curvature is absorbed here.
  MuonPt_Up = MuonPt_Up*1000.;//Return to Pt in GeV.

  if (fabs(MuonPt_Down) < 0.14) MuonPt_Down = KappaBiasError; //To avoid a division by set the curvature to its error if after the correction the pt is larger than 7 TeV.
  MuonPt_Down = 1/MuonPt_Down;//Return to Pt.
  MuonPt_Down = fabs(MuonPt_Down);//returns unsigned Pt, any possible sign flip due to the curvature is absorbed here.
  MuonPt_Down = MuonPt_Down*1000.;//Return to Pt in GeV.

  out.ScaledPt = MuonPt_Central;
  out.ScaledPt_Up = MuonPt_Up;
  out.ScaledPt_Down = MuonPt_Down;

  return out;
};
