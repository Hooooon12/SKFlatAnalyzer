#include "CFBackgroundEstimator.h"

CFBackgroundEstimator::CFBackgroundEstimator()
{

  MissingHists.clear();

  IgnoreNoHist = false;
    
  histDir = TDirectoryHelper::GetTempDirectory("CFBackgroundEstimator");

  map_pt_bins.clear();
}

void CFBackgroundEstimator::ReadHistograms(int loadFormat){

  TString datapath = getenv("DATA_DIR");
  datapath = datapath+"/"+GetEra()+"/CFRate/";

  TDirectory* origDir = gDirectory;

  bool loadStudy=false;
  bool loadSyst=false;
  bool loadStandard=false;

  if(loadFormat > 0) loadStandard=true;
  if(loadFormat > 1) loadSyst=true;
  if(loadFormat > 1) loadStudy=true;
  
  if(loadStandard){
    string elline;
    ifstream in(datapath+"/histmap_Electron.txt");
    while(getline(in,elline)){
      std::istringstream is( elline );
      TString a,b,c,d,e,f;
      is >> a; // <Rate>                                                                                                                                                                                                                                                      
      is >> b; // histlabel                                                                                                                                                                                                                                                   
      is >> c; // shiftlabel                                                                                                                                                                                                                                                  
      is >> d; // syst                                                                                                                                                                                                                                                        
      is >> e; // ID                                                                                                                                                                                                                                                          
      is >> f; // <rootfilename>                                                                                                                                                                                                                                              
      TFile *file = new TFile(datapath+"/"+f);
      TList *histlist = file->GetListOfKeys();

      for(int i=0;i<histlist->Capacity();i++){
        TString this_cfname = histlist->At(i)->GetName();
        //cout << this_cfname << " " << b+"_"+c+"_"+d  << " --- " << f << endl;                                                                                                                                                                                               
        if(!this_cfname.Contains(b+"_"+c+"_"+d )) continue;
        if(!this_cfname.Contains(e)) continue;
        histDir->cd();
        map_hist_Electron[a+"_"+b+"_"+c+"_"+d+"_"+e] = (TH2D *)file->Get(this_cfname)->Clone();
        cout << "[CFBackgroundEstimator::CFBackgroundEstimator] map_hist_Electron : " << a+"_"+b+"_"+c+"_"+d+"_"+e +"  --> "+this_cfname << endl;
      }

      //// Delete  File after loop                                                                                                                                                                                                                                            
      file->Close();
      delete file;
      origDir->cd();

    }
  }
  if(loadStudy){
    string elline;
    ifstream in(datapath+"/histmap_StudyElectron.txt");
    while(getline(in,elline)){
      std::istringstream is( elline );
      TString a,b,c,d,e,f;
      is >> a; // <Rate>
      is >> b; // histlabel
      is >> c; // shiftlabel
      is >> d; // syst
      is >> e; // ID
      is >> f; // <rootfilename>
      TFile *file = new TFile(datapath+"/"+f);
      TList *histlist = file->GetListOfKeys();
      
      for(int i=0;i<histlist->Capacity();i++){
	TString this_cfname = histlist->At(i)->GetName();
	//cout << this_cfname << " " << b+"_"+c+"_"+d  << " --- " << f << endl;
	if(!this_cfname.Contains(b+"_"+c+"_"+d )) continue;
	if(!this_cfname.Contains(e)) continue;
	histDir->cd();
	map_hist_Electron[a+"_"+b+"_"+c+"_"+d+"_"+e] = (TH2D *)file->Get(this_cfname)->Clone();
	cout << "[CFBackgroundEstimator::CFBackgroundEstimator] map_hist_Electron : " << a+"_"+b+"_"+c+"_"+d+"_"+e +"  --> "+this_cfname << endl;
	}
      
      //// Delete  File after loop
      file->Close();
      delete file;
      origDir->cd();
      
    }
  }
  if(loadSyst){
    string elline;
    ifstream in(datapath+"/histmap_SystElectron.txt");
    while(getline(in,elline)){
      std::istringstream is( elline );
      TString a,b,c,d,e,f;
      is >> a; // <Rate>                                                                                                                                                                                                                              
      is >> b; // histlabel                                                                                                                                                                                                                           
      is >> c; // shiftlabel                                                                                                                                                                                                                          
      is >> d; // syst                                                                                                                                                                                                                                
      is >> e; // ID                                                                                                                                                                                                                                  
      is >> f; // <rootfilename>                                                                                                                                                                                                                      
      TFile *file = new TFile(datapath+"/"+f);
      TList *histlist = file->GetListOfKeys();
      
      for(int i=0;i<histlist->Capacity();i++){
	TString this_cfname = histlist->At(i)->GetName();
	if(!this_cfname.Contains(b+"_"+c+"_"+d )) continue;
	if(!this_cfname.Contains(e)) continue;
	histDir->cd();
	map_hist_Electron[a+"_"+b+"_"+c+"_"+d+"_"+e] = (TH2D *)file->Get(this_cfname)->Clone();
	cout << "[CFBackgroundEstimator::CFBackgroundEstimator] map_hist_Electron : " << a+"_"+b+"_"+c+"_"+d+"_"+e +"  --> "+this_cfname << endl;
      }
      
      //// Delete  File after loop                                                                                                                                                                                                                    
      file->Close();
      delete file;
      origDir->cd();
      
    }
  }

  
  cout << "Ending ReadHistograms " << endl;
  for(auto i : map_hist_Electron) cout << i.first << endl;
  return;
}

CFBackgroundEstimator::~CFBackgroundEstimator(){

  if(MissingHists.size() > 0){
    cout << "CFBackgroundEstimator Missing Hists " << endl;
    for(auto iMissing : MissingHists) cout << "Missing : " << iMissing << endl;
  }
}


void CFBackgroundEstimator::SetFittedBins(){
  
  vector<double> InputBins_EtaBB1 = {  0.,0.8,      1.4442,1.556,      2.5};
  map_pt_bins["InputBins_Eta_BB1"] = InputBins_EtaBB1;
  

  vector<double> InputBins_V1 = {  0.,
				   0.0015,
				   0.002,
				   0.00225,
				   0.0025,
				   0.00275,
				   0.003,
				   0.004,
				   0.005,
				   0.006,
				   0.007,
				   0.008,
				   0.010,
				   0.0125,
				   0.015,
				   0.0175,
				   0.020,
				   0.0225,
				   0.025,
				   0.030,
				   0.035,
				   0.04,
				   0.07};

  map_pt_bins["InputBins_Version1"] = InputBins_V1;

  vector<double> InputBins_V2 = {0.,
				 0.002,
				 0.0025,
				 0.003,
				 0.004,
				 0.005,
				 0.006,
				 0.007,
				 0.008,
				 0.010,
				 0.0125,
				 0.015,
				 0.020,
				 0.025,
				 0.030,
				 0.04,
				 0.07};
  
  map_pt_bins["InputBins_Version2"] = InputBins_V2;
  
  
  vector<double> InputBins_EC1 =  { 0.,
				    0.002,
                                    0.00225,
                                    0.0025,
                                    0.00275,
                                    0.003,
                                    0.0035,
                                    0.004,
                                    0.0045,
                                    0.005,
                                    0.0055,
                                    0.006,
                                    0.007,
                                    0.008,
                                    0.009,
                                    0.010,
                                    0.0111,
                                    0.0125,
                                    0.0135,
                                    0.015,
                                    0.0175,
                                    0.020,
                                    0.0225,
                                    0.025,
                                    0.030,
                                    0.035,
                                    0.04,
                                    0.07};

  map_pt_bins["InputBins_EC1"] = InputBins_EC1;


  vector<double> InputBins_EC2 =  { 0.,
                                    0.00225,
                                    0.0025,
                                    0.00275,
                                    0.003,
                                    0.0035,
                                    0.004,
                                    0.0045,
                                    0.005,
                                    0.0055,
                                    0.006,
                                    0.007,
                                    0.008,
                                    0.009,
                                    0.010,
                                    0.0111,
                                    0.0125,
                                    0.0135,
                                    0.015,
                                    0.0175,
                                    0.020,
                                    0.0225,
                                    0.025,
                                    0.030,
                                    0.035,
                                    0.04,
                                    0.07};

  map_pt_bins["InputBins_EC2"] = InputBins_EC2;

  vector<double> InputBins_Pt    = { 15.,
                                     25.,
                                     40.,
                                     45.,
                                     50.,
                                     55.,
                                     60.,
                                     65.,
                                     70.,
                                     80.,
                                     90.,
                                     100.,
                                     125.,
                                     150.,
                                     175.,
                                     200.,
                                     300.,
                                     400.,
                                     500.,
                                     1000.};

  
  map_pt_bins["InputBins_Pt"] = InputBins_Pt;


  //map_pt_bins = ptbin_map;
  

}

vector<double> CFBackgroundEstimator::FindBin(TString key){

  map<TString, vector<double> >::iterator mapit;                                                                                                                                                     

  mapit = map_pt_bins.find(key);
  if(mapit  == map_pt_bins.end()){
    cout << "FindBin ERROR " << key << endl;
    exit(ENODATA);
  }
  
  return mapit->second;
}

double CFBackgroundEstimator::GetElectronCFRateFitted(TString ID, TString EtaBinTag, TString key, double eta, double pt, int sys){

  vector<double> HistBins;
  if(EtaBinTag == "InvPtBB1")      HistBins = FindBin("InputBins_Version1");
  else if(EtaBinTag == "InvPtBB2") HistBins = FindBin("InputBins_Version2");
  else if(EtaBinTag == "InvPtEC1") HistBins = FindBin("InputBins_EC1");
  else if(EtaBinTag == "InvPtEC2") HistBins = FindBin("InputBins_EC2");
  else if(EtaBinTag == "InvPt")    HistBins = FindBin("InputBins_Version1");
  else if(EtaBinTag == "Pt")       HistBins = FindBin("InputBins_Pt");
  else HistBins = FindBin(EtaBinTag);

  eta = fabs(eta);
  if(pt > 1000) pt = 999;

  if(eta>=2.5) eta = 2.49;
  

  //// Lowest pt value is 20;                                                                                                                                                                
  if(pt < 15) pt = 15;

  std::map< TString, TH2D* >::const_iterator mapit;
  mapit = map_hist_Electron.find(key );

  if(mapit==map_hist_Electron.end()){
    cout << "[CFBackgroundEstimator::GetElectronCFRateFitted] No"<< key  <<endl;
    if(IgnoreNoHist) {
      TString MapK = key;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);
      return 1.;
    }
    exit(ENODATA);
  }

  /// Based on Z Mass closure apply some correction to CFRates                                                                                                                                                    
  double MC_Closure_Corr = 1;

  if(ID.Contains("HNL_ULID")){
    if(GetEra() == "2016postVFP"){
      if(pt < 30) MC_Closure_Corr = 0.95;
      else if(pt < 100) MC_Closure_Corr = 0.9;
      else if(pt < 300) MC_Closure_Corr = 0.95;
      else if(pt < 400) MC_Closure_Corr = 0.8;
      else MC_Closure_Corr = 0.7;
      if(pt  < 400&& pt > 370)   MC_Closure_Corr*= (0.037);
    }
    if(GetEra() == "2016preVFP"){
      if(pt < 100) MC_Closure_Corr = 0.95;
      if(pt > 300) MC_Closure_Corr = 0.85;
    }
    
    if(GetEra() == "2017"){
      if(pt > 200) MC_Closure_Corr = 0.8;
      else if(pt > 100) MC_Closure_Corr = 0.9;
    }
    if(GetEra() == "2018"){
      if(pt < 30) MC_Closure_Corr = 0.95;
      if(pt < 90 && pt > 60) MC_Closure_Corr = 0.95;
      
    }
  }

  /// 
  /*
    2016post 40-50 0.9 70-100 0.9
    2016pre 200 + SF = 1.2 ; 
    2017  150+ SF = 0.8 
    // 2018 200 + SF = 0.8; 100-200 1.2
    
   */



  int this_bin = (key.Contains("Inv")) ?  (mapit->second)->FindBin(1/pt,eta) :  (mapit->second)->FindBin(pt,eta);

  if(GetEra() == "2016postVFP"){
    if(ID.Contains("HNL_ULID")){
      if( key.Contains("InvPtEta3")  && this_bin == (mapit->second)->FindBin(0.0023,2.3)) this_bin = (mapit->second)->FindBin(0.0026,2.3)  ;
      if( key.Contains("InvPtEta3")  && this_bin == (mapit->second)->FindBin(0.0023,1.9)) this_bin = (mapit->second)->FindBin(0.0026,1.9)  ;
    }
  }


  int this_bin_m1 = this_bin-1;
  int this_bin_p1 = this_bin+1;

  double value_MainBin = (mapit->second)->GetBinContent(this_bin)*MC_Closure_Corr;
  double error_MainBin = (mapit->second)->GetBinError(this_bin);

  
  double IPt = (key.Contains("Inv")) ?   (1/pt) : pt;  

  if(IPt >= (HistBins.at(HistBins.size()-2) + HistBins.at(HistBins.size()-1))/2)   return value_MainBin + double(sys)*error_MainBin; 
  if(IPt <= 0.5*HistBins.at(1)) return value_MainBin + double(sys)*error_MainBin;
  

  double value_mBin = (mapit->second)->GetBinContent(this_bin_m1)*MC_Closure_Corr;
  double value_pBin = (mapit->second)->GetBinContent(this_bin_p1)*MC_Closure_Corr;

  vector <double> InputBins = HistBins;
  
  vector <double> BinCentres;
  double Center_MainBin(-1.), Center_mBin(-1.), Center_pBin(-1.);
  bool corrBinFound=false;
  for(int ib =1; ib < int(InputBins.size()); ib++){
    if(!corrBinFound && (IPt < InputBins[ib]) && (IPt >=InputBins[ib-1]) ) {
      corrBinFound=true;
      Center_MainBin = (InputBins[ib] + InputBins[ib-1]) /2.;
      if(ib>1) Center_mBin = (InputBins[ib-1] + InputBins[ib-2]) /2.;
      if(ib == int(InputBins.size())-1) Center_pBin = 0.; 
      else       Center_pBin    = (InputBins[ib+1] + InputBins[ib]) /2.;

    }
  }


  //cout << "  "<< endl;
  //cout << "Pt = " <<  pt << " IPt = " << IPt << " this_bin = " << this_bin << " this_bin_m1 = " << this_bin_m1 << "  = " << this_bin_p1 << endl;
  //cout << "value_MainBin = " << value_MainBin << " value_mBin " << value_mBin << " value_pBin = " << value_pBin << endl;
  //cout << "Center_MainBin = " << Center_MainBin << " Center_mBin = " << Center_mBin << " Center_pBin = " << Center_pBin << endl;

  
  ////    0         0.002     0.003                      0.04     0.07
  ////    |         |         |       |    |      |     |    |

  //cout << "GetElectronCFRateFitted EXTRAP " << endl;

  if(IPt < Center_MainBin) {

    double dX = Center_MainBin - Center_mBin;
    double dY = value_mBin - value_MainBin;
    
    double CFRate = value_MainBin + (dY/dX) * (Center_MainBin-IPt);
    
    
    //cout << "Left of centre : IPt = " << IPt << " dX = " << dX << " dY = " << dY << " (Center_MainBin-IPt) = " << (Center_MainBin-IPt) << endl;
    //cout << "Left of centre : CFRate : " << value_MainBin << " --> "<<  CFRate << endl;
    if(isnan(CFRate)) {
      cout << "CF Fit returns nan. " << endl;
      
      exit(ENODATA);
    }
    return CFRate+ double(sys)*error_MainBin;
  }

  else{
    double dX = Center_pBin - Center_MainBin;
    double dY = value_MainBin - value_pBin;
    
    //cout << "Right of centre : IPt = " << IPt << " dX = "<< dX << " dY = " << dY<< " (Center_MainBin-IPt) = " << (Center_MainBin-IPt) << endl;
    double CFRate = value_MainBin + (dY/dX) * (Center_MainBin-IPt);

    //cout << "Right of centre : CFRate : "<< value_MainBin << " --> "<<  CFRate << endl;

    if(isnan(CFRate)) {
      cout << "CF Fit returns nan. " <<endl;
      
      exit(ENODATA);
    }
    
    return CFRate+ double(sys)*error_MainBin;
  }

  
}

double CFBackgroundEstimator::GetElectronCFRate(TString ID, TString key, double eta, double pt, int sys){

  //cout << "[CFBackgroundEstimator::GetElectronCFRate] ID = " << ID << ", key = " << key << endl;
  //cout << "[CFBackgroundEstimator::GetElectronCFRate] eta = " << eta << ", pt = " << pt << endl;

  double value = 1.;
  double error = 0.;

  eta = fabs(eta);
  if(eta>=2.5) eta = 2.49;
  
  //// Lowest pt value is 20;

  if(pt < 15) pt = 15;
  
  //if(pt < 25) {
  //// Temp fix until new Rates are added with low pt bin
  //key = key.ReplaceAll("Inv","");
  //}
  
  if(pt > 1000) pt = 999;
 
 
  std::map< TString, TH2D* >::const_iterator mapit;

  mapit = map_hist_Electron.find(key );

  if(mapit==map_hist_Electron.end()){
    cout << "[CFBackgroundEstimator::GetElectronCFRate] No"<< key  <<endl;
    if(IgnoreNoHist) {
      TString MapK = key;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);
      return 1.;
    }

    exit(ENODATA);
  }

  int this_bin = (key.Contains("Inv")) ?  (mapit->second)->FindBin(1/pt,eta) :  (mapit->second)->FindBin(pt,eta);
  value = (mapit->second)->GetBinContent(this_bin);
  error = (mapit->second)->GetBinError(this_bin);

  if(pt > 500) error = value *0.5; ///// FIX Later

  return value+double(sys)*error;

}


double CFBackgroundEstimator::GetMuonCFRate(TString ID, TString key, double eta, double pt, int sys){

  //cout << "[CFBackgroundEstimator::GetMuonCFRate] ID = " << ID << ", key = " << key << endl;
  //cout << "[CFBackgroundEstimator::GetMuonCFRate] eta = " << eta << ", pt = " << pt << endl;

  double value = 1.;
  double error = 0.;

  eta = fabs(eta);

  if(eta>=2.5) eta = 2.49;

  TString EtaRegion = "InnerBarrel";
  if(eta<0.8) EtaRegion = "InnerBarrel";
  else if(eta<1.479) EtaRegion = "OuterBarrel";
  else EtaRegion = "EndCap";

  std::map< TString, TH1D* >::const_iterator mapit;
  mapit = map_hist_Muon.find(ID+"_"+key+"_"+EtaRegion+"_InvGenPt");

  if(mapit==map_hist_Muon.end()){
    cout << "[CFBackgroundEstimator::GetMuonCFRate] No"<< ID+"_"+key+"_"+EtaRegion+"_InvGenPt" <<endl;
    exit(ENODATA);
  }

  int this_bin = (mapit->second)->FindBin(1./pt,eta);
  value = (mapit->second)->GetBinContent(this_bin);
  error = (mapit->second)->GetBinError(this_bin);

  //cout << "[CFBackgroundEstimator::CFBackgroundEstimator] value = " << value << endl;

  return value+double(sys)*error;

}

double CFBackgroundEstimator::GetWeight(vector<Lepton *> lepptrs, AnalyzerParameter param, int sys){

  double this_weight = 0.;
  for(unsigned int i=0; i<lepptrs.size(); i++){

    double this_cf = -999.;

    if(lepptrs.at(i)->LeptonFlavour()==Lepton::ELECTRON){

      Electron *el = (Electron *)( lepptrs.at(i) );

      this_cf = GetElectronCFRate(param.Electron_CF_ID, param.k.Electron_CF, fabs(el->scEta()), el->Pt(), sys);

    }
    else{

      Muon *mu = (Muon *)( lepptrs.at(i) );

      this_cf = GetMuonCFRate(param.Muon_CF_ID, param.k.Muon_CF, fabs(mu->Eta()), mu->Pt(), sys);

    }

    this_weight += this_cf/(1.-this_cf);

  }

  return this_weight;

}




