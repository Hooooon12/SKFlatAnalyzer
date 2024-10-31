#include "HNL_LeptonCore.h"

double HNL_LeptonCore::GetLimitBin(TString region, vector<Lepton*> leps, vector<Jet> AK4Jets, vector<FatJet> AK8_JetColl , Event ev, double & nbins_reg){


  double Binvalue=0;
  
  
  if(region=="CR_SR1_Inv"){

    Particle l1J =  *leps[0] +  AK8_JetColl[0];
    if(l1J.M() < 200) Binvalue=0.5;
    else if(l1J.M() < 500) Binvalue=1.5;
    else if(l1J.M() < 750) Binvalue= 2.5;
    else if(l1J.M() < 1000) Binvalue= 3.5;
    else Binvalue= 4.5;
    //// 3 bins                                                                                                                                                  
    nbins_reg=5;
    return Binvalue;
  }
  if(region=="CR_SR2_Inv"){
    double SR2BinValue = leps[0]->HTOverPt();
    if(SR2BinValue < 1) Binvalue = 0.5;
    else if(SR2BinValue < 2) Binvalue = 1.5;
    else   if(SR2BinValue < 5) Binvalue = 2.5;
    else   Binvalue = 3.5;

    nbins_reg=4;
    return Binvalue;
    
  }


  if(region.Contains("CR_SR1")){

    Particle l1J =  *leps[0] +  AK8_JetColl[0];
    if(l1J.M() < 200) Binvalue=0.5;
    else if(l1J.M() < 750) Binvalue= 1.5;
    else Binvalue= 2.5;    
    //// 3 bins
    nbins_reg=3;
    return Binvalue; 
  }

  if(region.Contains("CR_SR2")){

    double SR2BinValue = leps[0]->HTOverPt();
    if(SR2BinValue < 2) Binvalue = 0.5;
    else   if(SR2BinValue < 5) Binvalue = 1.5;
    else   Binvalue = 2.5;

    nbins_reg=3;
    return Binvalue;

  }

  if(region.Contains("CR_SR3")){
    if((AK4Jets.size()<2) && (leps[1]->Pt()  < 50)) Binvalue=0.5;
    else   if((AK4Jets.size()<2) && (leps[1]->Pt()  < 150)) Binvalue=1.5;
    else   if((AK4Jets.size()<2)) Binvalue=2.5;
    else   {
      Particle llJJ =  *leps[0] + *leps[1]+AK4Jets[0]+AK4Jets[1];
      if(llJJ.M() < 200) Binvalue= 3.5;
      else     if(llJJ.M() < 400) Binvalue= 4.5;
      else Binvalue= 5.5;
    }
    nbins_reg=6;
    return Binvalue;
    
  }

  if(region.Contains("CR_CF_SR1")){

    Particle l1JJ =  *leps[0] + AK8_JetColl[0];

    if(l1JJ.M() < 200) Binvalue= 0.5;
    else     if(l1JJ.M() < 750) Binvalue= 1.5;
    else Binvalue= 2.5;
    nbins_reg=3;
    return Binvalue;
    
  }

  if(region.Contains("CR_CF_SR2")){

    double SR2BinValue = leps[0]->HTOverPt();
    if(SR2BinValue < 2) Binvalue=0.5;
    else if(SR2BinValue < 5) Binvalue=1.5;
    else if(SR2BinValue < 10) Binvalue=2.5;
    nbins_reg=3;
    return Binvalue;
   

  }
  
  if(region.Contains("CR_CF_SR3")){

    if(leps[0]->Pt() < 50) Binvalue = 0.5;
    else if(leps[0]->Pt() < 200) Binvalue = 1.5;
    else Binvalue = 2.5;
    nbins_reg=3;
    return Binvalue;

  }

  if(region.Contains("CR_WW_SR2")){
    double VBFBinValue = leps[0]->HTOverPt();
    if(VBFBinValue < 1) Binvalue= 0.5;
    else     if(VBFBinValue < 2) Binvalue= 1.5;
    else     if(VBFBinValue < 5) Binvalue= 2.5;
    else Binvalue= 3.5;
    nbins_reg=4;

    return Binvalue;
  }

  cout << "No Limit Region set" << endl;
  exit(EXIT_FAILURE);

}

