// dumpFloatPars.C
// Usage:
//   root -l -q 'dumpFloatPars.C("/data6/Users/jihkim/LatestCombine/CMSSW_14_1_0_pre4/src/DilepHN/ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewRP/Run2_EE_M95_syst/fitDiagnostics_Run2_EE_M95_syst.root","fit_s")'
//   root -l -q 'dumpFloatPars.C("/data6/Users/jihkim/LatestCombine/CMSSW_14_1_0_pre4/src/DilepHN/ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewRP/Run2_EE_M95_syst/fitDiagnostics_Run2_EE_M95_syst.root","fit_b")'

#include "TFile.h"
#include "TSystem.h"
#include "RooFitResult.h"
#include "RooArgList.h"
#include "RooRealVar.h"
#include <cstdio>
#include <cmath>

void dumpFloatPars(const char* fname,
                   const char* fitname="fit_s",
                   double ratio_if_init0 = NAN)  // printed when initErr==0
{
  TFile f(fname);
  auto fr = (RooFitResult*)f.Get(fitname);
  if(!fr){
    printf("Cannot find %s in %s\n", fitname, fname);
    gSystem->Exit(1);
  }

  auto& init = fr->floatParsInit();
  auto& fin  = fr->floatParsFinal();

  printf("RooFitResult: %s  (file: %s)\n", fitname, fname);
  printf("%-60s  %-26s  %-26s  %s\n",
         "name", "init (val +/- err)", "final (val +/- err)", "final/init err");
  printf("%s\n", std::string(60+2+26+2+26+2+14, '-').c_str());

  for(int i=0;i<fin.getSize();++i){
    auto vF = (RooRealVar*)fin.at(i);
    auto vI = (RooRealVar*)init.find(vF->GetName()); // match by name
    if(!vI) continue;

    const double initVal = vI->getVal();
    const double initErr = vI->getError();
    const double finVal  = vF->getVal();
    const double finErr  = vF->getError();

    double ratio = ratio_if_init0;
    if(initErr != 0.0) ratio = finErr / initErr;

    if(std::isnan(ratio))
      printf("%-60s  % .6f +/- %-10.6f  % .6f +/- %-10.6f  %s\n",
             vF->GetName(), initVal, initErr, finVal, finErr, "n/a");
    else
      printf("%-60s  % .6f +/- %-10.6f  % .6f +/- %-10.6f  % .6f\n",
             vF->GetName(), initVal, initErr, finVal, finErr, ratio);
  }
}

