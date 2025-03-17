#include "HNL_LeptonCore.h"


double HNL_LeptonCore::ReturnCFWeight(double CFRates){

  double W = CFRates/(1-CFRates);

  return W;
}


double HNL_LeptonCore::ReturnCFWeight(const std::vector<double>& CFRates){

  double W = 0.;
  for(auto i : CFRates) W += (i/(1-i));
  return W;
}


double HNL_LeptonCore::GetCFSF(AnalyzerParameter param, Lepton*  lep, bool ApplySF){
  if(lep->IsMuon()) return 1.;

  
  return GetCFSF(param,lep->GetEtaRegion(), ApplySF);

}
double HNL_LeptonCore::GetCFSF(AnalyzerParameter param, Lepton  lep, bool ApplySF){
  if(lep.IsMuon()) return 1.;
  return GetCFSF(param,lep.GetEtaRegion(), ApplySF);

}
double HNL_LeptonCore::GetCFSF(AnalyzerParameter param, TString EraReg, bool ApplySF){

  if(!ApplySF) return 1;
  /*
    Function to get DATA/MC SF for CF Bkg
    Depends on ID/Eta
   */

 

  double _SF=1.;

  map<TString,vector<double> > CFSFValues;

  //// A second correction is applied based on Integral in Data Z peak fit, where an axtra shift is applied to miniise Data chi2

  CFSFValues["HNL_HighPt_ULID_BB"]      =  {0.9170, 1.044,1.552,1.430};
  CFSFValues["HNL_HighPt_ULID_EC"]      =  {0.873,0.914,1.329,1.293};

  CFSFValues["HNL_ULID_BB"]      =  {0.9704, 1.1178,1.5525,1.4446};
  CFSFValues["HNL_ULID_EC"]      =  {0.925,0.999,1.361,1.308};

  CFSFValues["passPOGTight_BB"]  =  {1.097, 1.04, 1.49, 1.54};
  CFSFValues["passPOGTight_EC"]  =  {1.01,1, 1.30, 1.35};
  
  CFSFValues["HNTightV2_BB"]     =  {0.842,0.967, 1.34, 1.44};
  CFSFValues["HNTightV2_EC"]     =  {0.84,0.90, 1.37, 1.40};
  
  CFSFValues["TopHNSST_BB"]      =  {0.80,     0.79,     1.162, 1.47};
  CFSFValues["TopHNSST_EC"]      =  {0.78,     0.83,     1.31,  1.28};
  

  /// HEEP Values are copied for HNTight
  CFSFValues["passHEEPID_v1_BB"]     =  {0.842,0.967, 1.34, 1.44};
  CFSFValues["passHEEPID_v1_EC"]     =  {0.84,0.90, 1.37, 1.40};

  CFSFValues["passHEEPID_v3_BB"]     =  {1.136,0.991,1.314,1.323};
  CFSFValues["passHEEPID_v3_EC"]     =  {0.904,0.902,1.199,1.235};

  ///// Pt Binned SF
  
  //  CFSFValues["HNL_HighPt_ULID_Pt2_BB"]      =  {0.88, 1.044,1.511,1.430};
  //  CFSFValues["HNL_HighPt_ULID_Pt2_BB"]      =  {0.9170, 1.044,1.329,1.430};



  map<TString,vector<double> >::iterator CFMapIter ;

  TString CFKey = param.Electron_Tight_ID+"_"+EraReg;
  CFKey=CFKey.ReplaceAll("_"+GetYearString(),"");
  CFMapIter = CFSFValues.find(CFKey);
  if(CFMapIter == CFSFValues.end()) {  cout << "[HNL_LeptonCore::GetCFSF ] ERROR in CFSF.. " << CFKey  << endl;   exit(EXIT_FAILURE);}
  _SF= CFMapIter->second[GetEraNum()-1];
  

  if(param.syst_ == AnalyzerParameter::CFSFUp) {
    if(EraReg== "BB") _SF *= 1.09;
    else  _SF *= 1.07;
  }
  if(param.syst_ == AnalyzerParameter::CFSFDown) {
    if(EraReg== "BB") _SF *= 1/1.09;
    else  _SF *= 1/1.07;
  }


  return _SF;

}


double HNL_LeptonCore::GetCFWeightElectron(std::vector<Electron> El ,  AnalyzerParameter param, bool ApplySF){

  std::vector<Lepton *> Leptons = MakeLeptonPointerVector(El,param);
  return GetCFWeightElectron(Leptons, param, -1, ApplySF);

}

double HNL_LeptonCore::GetCFWeightElectron(std::vector<Lepton* > leps ,  AnalyzerParameter param,int nEl,  bool ApplySF){

  if(leps.size()  != 2) return 1.;
  int nElIt = 0;

  double cf_weight = 0;
  
  int sysR  = 0;
  if(param.syst_ == AnalyzerParameter::CFRateUp) sysR = 1;
  if(param.syst_ == AnalyzerParameter::CFRateDown) sysR = -1;
  
  TString TightID = param.Electron_Tight_ID;
  if(TightID.Contains("HNL_ULID")) TightID=TightID.ReplaceAll("_"+GetYearString(),"");

  for(auto ilep : leps){
    if(ilep->LeptonFlavour()==Lepton::ELECTRON){
      
      if(nElIt==nEl) {
	double el_cf_rate =   cfEst->GetElectronCFRateFitted(TightID,"InvPtBB1", param.k.Electron_CF,ilep->defEta(),ilep->Pt(), sysR);
	el_cf_rate *= GetCFSF(param,ilep,ApplySF);
	cf_weight += (el_cf_rate / (1.-el_cf_rate));
      }
      else if(nEl == -1){
	double el_cf_rate =   cfEst->GetElectronCFRateFitted(TightID,"InvPtBB1", param.k.Electron_CF,ilep->defEta(),ilep->Pt(), sysR);
	el_cf_rate *= GetCFSF(param,ilep,ApplySF);
        cf_weight += (el_cf_rate / (1.-el_cf_rate));
      }
    }
    nElIt++;
  }
  

  if(run_Debug) cout << "GetCFWeightElectron weight=" << cf_weight << endl;


  //// Appply Mass Dependant Corr for 2017 for now
  if(IsData && DataEra == "2017"){
    if(param.syst_ == AnalyzerParameter::CFSFUp) {
      if(GetLLMass(leps) < 90.) cf_weight *= 1.2;
    }
  }

  return cf_weight;
}




double HNL_LeptonCore::GetZMassShift(vector<Electron> Electrons) {

  if(Electrons.size() != 2) return 1;

  if(Electrons[0].Charge() == Electrons[1].Charge()) return 1;

  if(DataEra=="2016preVFP"){
    if(Electrons[0].IsBB() && Electrons[1].IsBB())  return 0.993;
    if(Electrons[0].IsEC() && Electrons[1].IsEC())  return 0.996;
    return (0.993+0.996)/2.;

  }
  else  if(DataEra=="2016postVFP"){
    if(Electrons[0].IsBB() && Electrons[1].IsBB())  return 0.992;
    if(Electrons[0].IsEC() && Electrons[1].IsEC())  return 0.995;
    return (0.992+0.995)/2.;

  }
  if(DataEra=="2017"){
    if(Electrons[0].IsBB() && Electrons[1].IsBB())  return 0.987;
    if(Electrons[0].IsEC() && Electrons[1].IsEC())  return 0.993;
    return (0.987+0.993)/2.;
  }
  else if(DataEra=="2018"){
    /// 2018/16 Need updating
    if(Electrons[0].IsBB() && Electrons[1].IsBB())  return 0.988;
    if(Electrons[0].IsEC() && Electrons[1].IsEC())  return 0.993;
    return (0.988+0.993)/2.;
  }
  
  return 1;

}

double HNL_LeptonCore::PtExtrap(double val, double x1, double x2, double y1, double y2){

  double ptdirr = val - x1;
  double corr = ((y2-y1) / (x2-x1)) *ptdirr;
  return y1 + corr;

}
double HNL_LeptonCore::GetShiftPtMedian(double pt1, double pt2, double shiftVal){

  return (pt2+pt1)/(2.*shiftVal); 

}

double HNL_LeptonCore::CheckShiftRange(double val, double shift){
  if(val*shift > 1) return 1;
  return val*shift;
}

double HNL_LeptonCore::GetShiftCFEl(Electron el,TString ID, bool ApplyDataCorr, int Sys) {
  
  //// By default if HNL ID just use extraoplated Shift based on pt binned MC Truth
  
  if(ID.Contains("HNL_ULID")) ID=ID.ReplaceAll("_"+GetYearString(),"");//// Remove Year for CF inputs 

  //// Set the shift for HighPt same as Old ID since diff is < 1%
  if(ID.Contains("HighPt")) ID="HNL_ULID";

  double DataCorr = 1.;
  if(ApplyDataCorr){
    if(DataEra=="2016preVFP"  && el.IsBB()) DataCorr=0.985;
    if(DataEra=="2016postVFP" && el.IsBB()) DataCorr=0.99;
    if(DataEra=="2017"        && el.IsBB()) DataCorr=0.985;
    if(DataEra=="2018"        && el.IsBB()) DataCorr=0.985;
    
    if(DataEra=="2016preVFP"  && !el.IsBB()) DataCorr=1;
    if(DataEra=="2016postVFP" && !el.IsBB()) DataCorr=1.;
    if(DataEra=="2017"        && !el.IsBB()) DataCorr=0.995;
    if(DataEra=="2018"        && !el.IsBB()) DataCorr=0.985;
  }
  
  return  DataCorr*GetShiftCFEl(el.Pt(),el.IsBB(), ID, ApplyDataCorr, Sys );
  
}

double HNL_LeptonCore::GetCFShiftSyst(TString ID, TString bintag, int direction){

  if(direction==0) return 1.;

  TString key = GetEraShort() + "_"+bintag+"_"+ID+"_Nom";

  if(direction>0)  key = GetEraShort() + "_"+bintag+"_"+ID+"_Up";
  if(direction<0)  key = GetEraShort() + "_"+bintag+"_"+ID+"_Down";
  
  map<TString,double >::iterator CFMapIter ;

  CFMapIter = MakeCFShiftmap.find(key);

  if(CFMapIter == MakeCFShiftmap.end()) {  cout << "[HNL_LeptonCore::GGetCFShiftSyst] ERROR in GetCFShiftSyst.. " << key  << endl;   exit(EXIT_FAILURE);}

  return 1 + double(direction)*CFMapIter->second;


}

double HNL_LeptonCore::GetCFShift(TString ID, TString bintag){

  TString key = GetEraShort() + "_"+bintag+"_"+ID+"_Nom";
    
  map<TString,double >::iterator CFMapIter ;
  
  CFMapIter = MakeCFShiftmap.find(key);
  if(CFMapIter == MakeCFShiftmap.end()) {  cout << "[HNL_LeptonCore::GGetCFShift] ERROR in GetCFShift.. " << key  << endl;   exit(EXIT_FAILURE);}

  return CFMapIter->second;

}

double HNL_LeptonCore::GetShiftCFEl(double el_pt, bool IsBB,TString ID, bool ApplyDataCorr, int sys) {

  //// Get Shift for  Prompt -> CF Pt response                                                                                                                                                                   
  if(IsBB){
    double MedianBin1 = GetShiftPtMedian(15,35,GetCFShift(ID,"BB_Pt_Bin1"));
    double MedianBin2 = GetShiftPtMedian(35,50,GetCFShift(ID,"BB_Pt_Bin2"));
    double MedianBin3 = GetShiftPtMedian(50,70,GetCFShift(ID,"BB_Pt_Bin3"));
    double MedianBin4 = GetShiftPtMedian(70,100,GetCFShift(ID,"BB_Pt_Bin4"));
    double MedianBin5 = GetShiftPtMedian(100,200,GetCFShift(ID,"BB_Pt_Bin5"));
    
    if(el_pt < MedianBin1)  return CheckShiftRange(GetCFShift(ID,"BB_Pt_Bin1"),GetCFShiftSyst(ID,"BB_Pt_Bin1",sys));
    if(el_pt < MedianBin2)  return CheckShiftRange(PtExtrap(el_pt , MedianBin1,MedianBin2, GetCFShift(ID,"BB_Pt_Bin1"),GetCFShift(ID,"BB_Pt_Bin2")),GetCFShiftSyst(ID,"BB_Pt_Bin2",sys));
    if(el_pt < MedianBin3)  return CheckShiftRange(PtExtrap(el_pt , MedianBin2,MedianBin3, GetCFShift(ID,"BB_Pt_Bin2"),GetCFShift(ID,"BB_Pt_Bin3")),GetCFShiftSyst(ID,"BB_Pt_Bin3",sys));
    if(el_pt < MedianBin4)  return CheckShiftRange(PtExtrap(el_pt , MedianBin3,MedianBin4, GetCFShift(ID,"BB_Pt_Bin3"),GetCFShift(ID,"BB_Pt_Bin4")),GetCFShiftSyst(ID,"BB_Pt_Bin4",sys));
    if(el_pt < MedianBin5)  return CheckShiftRange(PtExtrap(el_pt , MedianBin4,MedianBin5, GetCFShift(ID,"BB_Pt_Bin4"),GetCFShift(ID,"BB_Pt_Bin5")),GetCFShiftSyst(ID,"BB_Pt_Bin5",sys));
    return CheckShiftRange(GetCFShift(ID,"BB_Pt_Bin5"),GetCFShiftSyst(ID,"BB_Pt_Bin5",sys));
    
  }
  else {
    
    double MedianBin1 = GetShiftPtMedian(15,20,GetCFShift(ID,"EC_Pt_Bin1"));
    double MedianBin2 = GetShiftPtMedian(20,30,GetCFShift(ID,"EC_Pt_Bin2"));
    double MedianBin3 = GetShiftPtMedian(30,40,GetCFShift(ID,"EC_Pt_Bin3"));
    double MedianBin4 = GetShiftPtMedian(40,50,GetCFShift(ID,"EC_Pt_Bin4"));
    double MedianBin5 = GetShiftPtMedian(50,60,GetCFShift(ID,"EC_Pt_Bin5"));
    double MedianBin6 = GetShiftPtMedian(60,70,GetCFShift(ID,"EC_Pt_Bin6"));
    double MedianBin7 = GetShiftPtMedian(70,80,GetCFShift(ID,"EC_Pt_Bin7"));
    double MedianBin8 = GetShiftPtMedian(80,100,GetCFShift(ID,"EC_Pt_Bin8"));
    double MedianBin9 = GetShiftPtMedian(100,200,GetCFShift(ID,"EC_Pt_Bin9"));
    
    if(el_pt < MedianBin1)  return CheckShiftRange(GetCFShift(ID,"EC_Pt_Bin1"),GetCFShiftSyst(ID,"EC_Pt_Bin1",sys));
    if(el_pt < MedianBin2)  return CheckShiftRange(PtExtrap(el_pt , MedianBin1,MedianBin2, GetCFShift(ID,"EC_Pt_Bin1"),GetCFShift(ID,"EC_Pt_Bin2")),GetCFShiftSyst(ID,"EC_Pt_Bin2",sys));
    if(el_pt < MedianBin3)  return CheckShiftRange(PtExtrap(el_pt , MedianBin2,MedianBin3, GetCFShift(ID,"EC_Pt_Bin2"),GetCFShift(ID,"EC_Pt_Bin3")),GetCFShiftSyst(ID,"EC_Pt_Bin3",sys));
    if(el_pt < MedianBin4)  return CheckShiftRange(PtExtrap(el_pt , MedianBin3,MedianBin4, GetCFShift(ID,"EC_Pt_Bin3"),GetCFShift(ID,"EC_Pt_Bin4")),GetCFShiftSyst(ID,"EC_Pt_Bin4",sys));
    if(el_pt < MedianBin5)  return CheckShiftRange(PtExtrap(el_pt , MedianBin4,MedianBin5, GetCFShift(ID,"EC_Pt_Bin4"),GetCFShift(ID,"EC_Pt_Bin5")),GetCFShiftSyst(ID,"EC_Pt_Bin5",sys));
    if(el_pt < MedianBin6)  return CheckShiftRange(PtExtrap(el_pt , MedianBin5,MedianBin6, GetCFShift(ID,"EC_Pt_Bin5"),GetCFShift(ID,"EC_Pt_Bin6")),GetCFShiftSyst(ID,"EC_Pt_Bin6",sys));
    if(el_pt < MedianBin7)  return CheckShiftRange(PtExtrap(el_pt , MedianBin6,MedianBin7, GetCFShift(ID,"EC_Pt_Bin6"),GetCFShift(ID,"EC_Pt_Bin7")),GetCFShiftSyst(ID,"EC_Pt_Bin7",sys));
    if(el_pt < MedianBin8)  return CheckShiftRange(PtExtrap(el_pt , MedianBin7,MedianBin8, GetCFShift(ID,"EC_Pt_Bin7"),GetCFShift(ID,"EC_Pt_Bin8")),GetCFShiftSyst(ID,"EC_Pt_Bin8",sys));
    if(el_pt < MedianBin9)  return CheckShiftRange(PtExtrap(el_pt , MedianBin8,MedianBin9, GetCFShift(ID,"EC_Pt_Bin8"),GetCFShift(ID,"EC_Pt_Bin9")),GetCFShiftSyst(ID,"EC_Pt_Bin9",sys));
    return CheckShiftRange(GetCFShift(ID,"EC_Pt_Bin9"),GetCFShiftSyst(ID,"EC_Pt_Bin9",sys));
    
  }

  
  cout << "[HNL_LeptonCore::GetShiftCFEl ] ERROR in GetShiftCFEl.. " << ID <<  endl;
  exit(EXIT_FAILURE);


  return 1;
}

