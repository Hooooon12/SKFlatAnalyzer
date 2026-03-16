#include "FakeBackgroundEstimator.h"

FakeBackgroundEstimator::FakeBackgroundEstimator() : 
IgnoreNoHist(false),
HasLooseLepton(false)
{

  MissingHists.clear();
  
  histDir = TDirectoryHelper::GetTempDirectory("FakeBackgroundEstimator");
  
}

void FakeBackgroundEstimator::ReadHistograms(bool IsData, bool ScanIDs){
  
  cout << "FakeBackgroundEstimator::ReadHistograms Scan IDs=" << ScanIDs << endl;
  TString datapath = getenv("DATA_DIR");
  
  TString DataFakePath = datapath+"/"+GetEra()+"/FakeRate/DataFR/";
  TString MCFakePath = datapath+"/"+GetEra()+"/FakeRate/MCFR/";
  
  TDirectory* origDir = gDirectory;

  vector<TString> FakeHMaps = {   DataFakePath+"/ElFR/histmap_Electron.txt",
				  DataFakePath+"/MuFR/histmap_Muon.txt"};
  
  if(ScanIDs) FakeHMaps.push_back(DataFakePath+"/MuFR/scan_histmap_Muon.txt");
  if(ScanIDs) FakeHMaps.push_back(DataFakePath+"/ElFR/scan_histmap_Electron.txt");


  if(!IsData){
    cout << "Setting up MC Hists" << endl;
    FakeHMaps = {	  MCFakePath+"/ElFR/histmap_Electron.txt",
			  MCFakePath+"/MuFR/histmap_Muon.txt"};
  }
  for(auto ihmap  :  FakeHMaps){
    cout << " --- > " << ihmap << endl;
    string elline;
    ifstream in(ihmap);
    while(getline(in,elline)){
      std::istringstream is( elline );
      
      TString a,b,c,d,e;
      is >> a; // <Rate>
      is >> b; // <IDlabel>
      is >> c; // <var key>
      is >> d; // <syst key>
      is >> e; // <rootfilename>
      
      if(a.Contains("###")) continue;

      TString FFRPath = ihmap;
      FFRPath = FFRPath.ReplaceAll("histmap_Electron.txt","");
      FFRPath = FFRPath.ReplaceAll("histmap_Muon.txt","");
      FFRPath = FFRPath.ReplaceAll("scan_","");

      TFile *file = new TFile(FFRPath+"/"+e);
      TList *histlist = file->GetListOfKeys();
      for(int i=0;i<histlist->Capacity();i++){
        TString this_frname = histlist->At(i)->GetName();
        //  cout << "Check " <<  this_frname <<  " --  " << b+"_"+c << " --  " << d << endl;
        
        if (b.Contains("HighPt")) {
          if (!this_frname.Contains(c)) continue;
          if (!this_frname.Contains(d)) continue;
        }
        else if (!b.Contains("Top")) {
          if (!this_frname.Contains(b+"_"+c)) continue;
          //if (!this_frname.Contains(c)) continue;
          if (!this_frname.Contains(d)) continue;
        }
        else{
          if (!this_frname.Contains(b)) continue;
                if (!this_frname.Contains(d)) continue;
        }
        histDir->cd();
        
        if(ihmap.Contains("Electron")) map_hist_Electron[a+"_"+b+"_"+c+"_"+d] = (TH2D *)file->Get(this_frname)->Clone(a+"_"+b+"_"+c+"_"+d);
        else  map_hist_Muon[a+"_"+b+"_"+c+"_"+d] = (TH2D *)file->Get(this_frname)->Clone(a+"_"+b+"_"+c+"_"+d);
        
        origDir->cd();
        if(ihmap.Contains("Electron")) cout << ihmap << " [FakeBackgroundEstimator::FakeBackgroundEstimator] map_hist_Electron : " << a+"_"+b+"_"+c+"_"+d+ " --> "+this_frname << endl;
        else cout << ihmap << " [FakeBackgroundEstimator::FakeBackgroundEstimator] map_hist_Muon : " << a+"_"+b+"_"+c+"_"+d+ " --> "+this_frname << endl;

      }
      file->Close();
      delete file;
    }
  }


}

FakeBackgroundEstimator::~FakeBackgroundEstimator(){

  if(MissingHists.size() > 0){
    cout << "FakeBackgroundEstimator Missing Hists " << endl;
    for(auto iMissing : MissingHists) cout << "Missing : " << iMissing << endl;
  }
  
}


double FakeBackgroundEstimator::GetFakeRate(bool IsMuon,TString ID, TString key, TString BinningMethod, TString BinningParam, double eta, double pt, TString FakeTagger, int sys){

  if(ID.Contains("HEEP")) return 0.4; //// THIS IS HACK AND JUST FOR CHECK

  if(IsMuon) return GetMuonFakeRate(ID, key, BinningMethod, BinningParam, eta, pt, FakeTagger, sys );
  else return GetElectronFakeRate(ID, key, BinningMethod, BinningParam, eta, pt, FakeTagger, sys );

}
 
double FakeBackgroundEstimator::HighPtCorr(TString ID, double eta, double pt, int sys){

  /// Function uses MC to correct high pt Fake rates
  if(pt < 150) return 1;


  double ApplyHighPtCorr = 1;

  // Function uses MC to correct high pt Fake rates
  if (pt < 150) return 1;

  // Initialize correction factor for each era and sys value
  double correctionFactor = 1.0;
  bool isEtaLessThan1_5 = fabs(eta) < 1.5;

  if (sys == -1 || sys == 0 || sys == 1) {
    if (GetEra().Contains("2016") || GetEra() == "2017" || GetEra() == "2018") {
      if (isEtaLessThan1_5) {
	if (pt > 200) {
	  if (sys == -1) correctionFactor = 1.0;
	  else if (sys == 0) correctionFactor = (GetEra() == "2016" ? 1.25 : (GetEra() == "2017" ? 1.2 : 1.1));
	  else if (sys == 1) correctionFactor = (GetEra() == "2016" ? 1.5 : (GetEra() == "2017" ? 1.4 : 1.2));
	} else if (pt > 150) {
	  if (sys == -1) correctionFactor = 1.0;
	  else if (sys == 0) correctionFactor = (GetEra() == "2016" ? 1.1 : (GetEra() == "2017" ? 1.1 : 1.1));
	  else if (sys == 1) correctionFactor = (GetEra() == "2016" ? 1.2 : (GetEra() == "2017" ? 1.2 : 1.2));
	}
      } else {
	if (pt > 250) {
	  correctionFactor = (sys == -1 ? 0.5 : (sys == 0 ? 0.75 : 1.0));
	}
      }
    }
  }

  ApplyHighPtCorr = correctionFactor;


  return ApplyHighPtCorr;

}

double FakeBackgroundEstimator::GetElectronFakeRate(TString ID, TString key, TString BinningMethod, TString BinningParam,double eta, double pt, TString FakeTagger, int sys){

  double ApplyHighPtCorr;

  if(sys==10)    {
    ApplyHighPtCorr = HighPtCorr(ID,eta,pt,1);
    sys=0;
  }
  else if(sys==-10){
    ApplyHighPtCorr = HighPtCorr(ID,eta,pt,-1);
    sys=0;
  }
  else ApplyHighPtCorr = HighPtCorr(ID,eta,pt,0);


  if(ID.Contains("HighPt")) ID=ID.ReplaceAll("_HighPt","");

  TString PtType = "pt_eta_";
  if(key.Contains("MC")){
    key=key.ReplaceAll("MC_","");
    if(BinningParam.Contains("PtCone" ))  PtType = "ptcone_eta_";
    if(BinningParam == "PtParton") PtType= "ptparton_eta_";
    key=key.ReplaceAll("_AJ","_MC_AJ");

    key =  "MC_"+PtType + key;
  }
  else{
    if(BinningParam.Contains("PtCone" ))  PtType = "ptcone_eta_";
    if(BinningParam == "PtParton") PtType= "ptparton_eta_";
    key =  PtType + key;
  }

  //// Use Fabs Eta
  eta = fabs(eta);
  
  //// Assign Limit on Bin Input
  double value = 1.,  error = 0.;
  
  /// Default
  double ptmin=10.1, ptmax=79., etamin=0.01, etamax=2.49;

  if(ID.Contains("Top")) {
    ptmin=10.1, ptmax=49., etamin=0.01, etamax=2.49;
  }

  pt = min(max(pt,ptmin),ptmax);
  eta = min(max(eta,etamin),etamax);

  
  std::map< TString, TH2D* >::const_iterator mapit;
  //  cout << "EL KEY FakeRate_"+ID+"_"+key << " ---" <<  endl;
  mapit = map_hist_Electron.find("FakeRate_"+ID+"_"+key);
  
  if(mapit==map_hist_Electron.end()){

    if(IgnoreNoHist){      

      TString MapK = "FakeRate_"+ID+"_"+key;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);
      return 1.;
    }
    else{

      cout << "[FakeBackgroundEstimator::GetElectronFakeRate] No "<< ID+"_"+key <<endl;
      for(auto i : map_hist_Electron ) cout << i.first << endl;
      exit(ENODATA);
    }
  }

  int this_bin = (mapit->second)->FindBin(pt,eta);
  value = (mapit->second)->GetBinContent(this_bin);
  error = (mapit->second)->GetBinError(this_bin);

  if(FakeTagger == "HFB" || FakeTagger == "HFC"){
    if(eta > 2.4) value *=1.5;
  }
  if(value > 0.5) value= 0.5;

  if(sys == 2)   return ApplyHighPtCorr*(value*1.3);
  if(sys == -2)   return ApplyHighPtCorr*(value/1.3);
  
  return ApplyHighPtCorr*(value+double(sys)*error);

}

double FakeBackgroundEstimator::GetMuonFakeRate(TString ID, TString key, TString BinningMethod, TString BinningParam,  double eta, double pt, TString FakeTagger, int sys){

  if(fabs(sys)==10)  sys=0;
    
  TString PtType = "pt_eta_";
  if(key.Contains("MC")){
    key=key.ReplaceAll("MC_","");
    if(BinningParam.Contains("PtCone" ))  PtType = "ptcone_eta_";
    if(BinningParam == "PtParton") PtType= "ptparton_eta_";
    if(BinningParam == "PtCorr")   PtType= "ptcorr_eta_";

    key=key.ReplaceAll("_AJ","_MC_AJ");

    key =  "MC_"+PtType + key;

  }
  else{
    if(BinningParam.Contains("PtCone" ))  PtType = "ptcone_eta_";
    if(BinningParam == "PtParton") PtType= "ptparton_eta_";
    if(BinningParam == "PtCorr")   PtType= "ptcorr_eta_";
    if(BinningParam == "MotherPt") PtType= "mjpt_eta_";
    
    key =  PtType + key;
  }
  


  double value = 1.;
  double error = 0.;

  eta = fabs(eta);

  /// Make Sure pt is not out of bin range                                                                                                                                                                                                                                     


  double ptmin=10.1, ptmax=79., etamin=0.01, etamax=2.49;

  if(ID.Contains("Top")) {
    ptmin=10.1, ptmax=49., etamin=0.01, etamax=2.49;
  }

  pt = min(max(pt,ptmin),ptmax);
  eta = min(max(eta,etamin),etamax);


  /// For Flvour bins binning is differen                                                                                                                                                   

  std::map< TString, TH2D* >::const_iterator mapit;
  //cout << "KEY FakeRate_"+ID+" _ "+key << endl;
  mapit = map_hist_Muon.find("FakeRate_"+ID+"_"+key);

  if(mapit==map_hist_Muon.end()){
    cout << "Missing  " << "FakeRate_"+ID+"_"+key << endl;
    for(auto i : map_hist_Muon) cout << i.first << endl;
    if(IgnoreNoHist){
      TString MapK = "FakeRate_"+ID+"_"+key;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);
      return 1.;
    }
    else{
      cout << "[FakeBackgroundEstimator::GetMuonFakeRate] No "<< ID+"_"+key <<endl;
      for(auto im : map_hist_Muon) cout << im.first << endl;

      exit(ENODATA);
    }
  }

  int this_bin (0);
  this_bin = (mapit->second)->FindBin(pt,eta);

  value = (mapit->second)->GetBinContent(this_bin);
  error = (mapit->second)->GetBinError(this_bin);

  if(value > 0.5) value = 0.5;

  if(sys == 2)   return  value*1.2;
  if(sys == -2)   return value/1.2;

  return value+double(sys)*error;

}


 
double FakeBackgroundEstimator::GetPromptRate(bool ApplyR,bool isMuon, TString ID, TString key, double eta, double pt, int sys){
  
  if(!ApplyR) return 1;

  if(isMuon)  return GetMuonPromptRate(ID, key, eta, pt, sys);
  else return GetElectronPromptRate(ID, key, eta, pt, sys);
}

double FakeBackgroundEstimator::GetElectronPromptRate(TString ID, TString key, double eta, double pt, int sys){

  double value = 1.;
  double error = 0.;
  
  eta = fabs(eta);
  if(pt>=500) pt = 499;
  if(pt < 15) pt = 15;


  std::map< TString, TH2D* >::const_iterator mapit;
  mapit = map_hist_Electron.find("PromptRate_"+ID+"_"+key);

  if(mapit==map_hist_Electron.end()){
    if(IgnoreNoHist){
      TString MapK = "PromptRate_"+ID+"_"+key;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);

      return 1.;
    }
    else{
      cout << "[FakeBackgroundEstimator::GetElectronPromptRate] No "<< ID+"_"+key <<endl;
      exit(ENODATA);
    }
  }

  int this_bin = (mapit->second)->FindBin(pt,eta);
  value = (mapit->second)->GetBinContent(this_bin);
  error = (mapit->second)->GetBinError(this_bin);

  return value+double(sys)*error;

}

double FakeBackgroundEstimator::GetMuonPromptRate(TString ID, TString key, double eta, double pt, int sys){

  double value = 1.;
  double error = 0.;

  /// Fix range of input vs Binned
  eta = fabs(eta);
  if(pt>=500) pt = 499;
  if(pt < 10) pt = 10;

  key=key.ReplaceAll("FO_2016","FO");
  key=key.ReplaceAll("FO_2017","FO");
  key=key.ReplaceAll("FO_2018","FO");

  std::map< TString, TH2D* >::const_iterator mapit;
  mapit = map_hist_Muon.find("PromptRate_"+ID+"_"+key);

  if(mapit==map_hist_Muon.end()){
    if(IgnoreNoHist) {

      TString MapK = "PromptRate_"+ID+"_"+key;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);

      return 1.;
    }
    else{
      cout << "[FakeBackgroundEstimator::GetMuonPromptRate] No "<< ID+"_"+key <<endl;
      exit(ENODATA);
    }
  }

  int this_bin = (mapit->second)->FindBin(pt,eta);
  value = (mapit->second)->GetBinContent(this_bin);
  error = (mapit->second)->GetBinError(this_bin);

  return value+double(sys)*error;

}



float FakeBackgroundEstimator::CalculateLepWeight(float r1, float fr1, bool lep1Tight){

  //https://cds.cern.ch/record/1951336/files/ATLAS-CONF-2014-058.pdf
  double w_l =0.;
  double alpha = (fr1/(r1-fr1)) * (r1);
  if(lep1Tight) w_l = - alpha  ;
  else w_l = alpha * r1;

  
  return w_l;

}

float FakeBackgroundEstimator::CalculateDilepWeight(float r1, float fr1, float r2, float fr2, bool lep1Tight, bool lep2Tight, int eventtype){ // eventtype = 0 as def

  // Function calculates event weight given r/f of two lepons in the event
  double alpha = 1./((r1- fr1)*(r2- fr2));

  // Initialise weight
  float w_ll=-999.;

  // Terms for RF and FR events from MM
  double termTT_sf = alpha*(r1*fr2*(fr1-1.)*(1.-r2) + (fr1*r2*(r1-1.)*(1.-fr2)));
  double termTL_sf = alpha*(r1*fr2*((1-fr1)*r2) + (fr1*r2*(1.-r1)*fr2));
  double termLT_sf = alpha*(r1*fr2*(fr1*(1.-r2))+ (fr1*r2*(1.-fr2)*r1));
  double termLL_sf = -2.*alpha*(r1*fr2*(fr1*r2));

  
  // Term for FF events
  double termTT_df = alpha*fr1*fr2*((1.-r1)*(1.-r2));
  double termTL_df = alpha*fr1*fr2*r2*(r1-1.);
  double termLT_df = alpha*fr1*fr2*r1*(r2-1.);
  double termLL_df = alpha*r1*r2*(fr1*fr2);

  double termTT(0.);
  double termTL(0.);
  double termLT(0.);
  double termLL(0.);
  
  if(eventtype==0) {
    termTT = termTT_sf + termTT_df;
    termTL = termTL_sf + termTL_df;
    termLT = termLT_sf + termLT_df;
    termLL = termLL_sf + termLL_df;    
  }
  else   if(eventtype==1) {
    termTT = termTT_sf;
    termTL = termTL_sf;
    termLT = termLT_sf;
    termLL = termLL_sf;
  }
  else if(eventtype==2) {
    termTT = termTT_df;
    termTL = termTL_df;
    termLT = termLT_df;
    termLL = termLL_df;
  }

  // decide which type of event we have so to return correct weight
  bool isLL = (!lep1Tight&&!lep2Tight);
  bool isLT = (!lep1Tight&&lep2Tight);
  bool isTL = (lep1Tight && !lep2Tight);
  bool isTT = (lep1Tight && lep2Tight);

  if(isLL)w_ll = termLL;
  if(isTL)w_ll = termTL;
  if(isLT)w_ll = termLT;
  if(isTT)w_ll=  termTT;


  return w_ll;
}




