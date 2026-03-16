#include "AnalyzerCore.h"


TH1D* AnalyzerCore::GetHist1D(TString histname){

  TH1D *h = NULL;
  std::map<TString, TH1D*>::iterator mapit = maphist_TH1D.find(histname);
  if(mapit != maphist_TH1D.end()) return mapit->second;

  return h;  

}

TH2D* AnalyzerCore::GetHist2D(TString histname){

  TH2D *h = NULL;
  std::map<TString, TH2D*>::iterator mapit = maphist_TH2D.find(histname);
  if(mapit != maphist_TH2D.end()) return mapit->second;

  return h;

}

TH3D* AnalyzerCore::GetHist3D(TString histname){

  TH3D *h = NULL;
  std::map<TString, TH3D*>::iterator mapit = maphist_TH3D.find(histname);
  if(mapit != maphist_TH3D.end()) return mapit->second;

  return h;

}


void AnalyzerCore::SetHistBins(){

  map_hist_bins.clear();
  map_hist_nbins.clear();

  AddHistBinning("Pt",     {10.,15.,20.,30.,35., 40.,50., 60., 80., 100.,200.,500.});
  AddHistBinning("Pt5",    {5,6,7,8,9,10.,11,12,13,14,15.,17.5,20.,22.5,25,30.,35., 40.,50.,80., 200.});
  AddHistBinning("Eta4",   {0.,0.8,  1.479, 2.,  2.5});
  AddHistBinning("Pt80",   {10.,15.,20.,30.,35., 40.,50., 60., 80.});
  AddHistBinning("Pt50",   {10.,15.,20.,30.,35., 40.,50.});
  
  AddHistBinning("PR_pt",     {10.,15.,20.,30.,35., 40.,50., 60., 80., 100.,200.,500.});
  AddHistBinning("PR_ptcone", {10.,15.,23.,30.,35., 40.,50., 60., 80., 100.,200.,500.});
  AddHistBinning("FR_pt", {10., 15.,20.,25.,30.,35.,40.,50.,60.,80});

  //// Fake rate Binnings
  AddHistBinning("FR_MC_eta", {0.,0.25, 1.479, 2.5});
  AddHistBinning("FR_Eta", {0., 0.8, 1.479,2., 2.5});
  AddHistBinning("FR_Eta1", {0.,  2.5});
  AddHistBinning("FR_Etav2", {0., 1.479,2., 2.5});
  AddHistBinning("FR_Etav3", {0., 1.479,2.2, 2.5});
  AddHistBinning("FR_Eta2Bin", {0.,1.479, 2.5});
  AddHistBinning("FR_FB_Muon_pt", {6.,10.,12., 15.,20.,25.,30.,35.,40.,60.,80});
  AddHistBinning("FR_Muon_pt",    {6.,10., 15.,20.,25.,30.,35.,40.,60.,80});
  AddHistBinning("FR_Muon_pt2",    {6.,10., 12.,15.,20.,25.,30.,35.,40.,60.,80});
  AddHistBinning("FR_Muon_ptcone",{6.,10., 15.,20.,25.,30.,35.,40.,60.,80.});
  AddHistBinning("FR_Muon_ptcone2",{6.,10.,12, 15.,20.,25.,30.,35.,40.,60.,80.});
  AddHistBinning("FR_Electron_ptcone",{10., 15.,23.,30.,35., 40.,60.,80.});
  AddHistBinning("FR_Electron_ptcone2",{10., 15.,20.,30.,40., 50.,60.,80.});

  AddHistBinning("FR_Electron_pt",{10., 15.,20.,25.,30., 35.,40.,60.,80.});

  AddHistBinning("FR_FB_Electron_ptcone",{10., 15.,18.,23.,30.,35., 40.,60.,80.});
  AddHistBinning("FR_FB_Electron_pt",{10., 15.,18., 20.,25.,30., 35.,40.,60.,80.});
  AddHistBinning("FR_BDT", {-1.,-0.8, -0.6,-0.4,-0.2,0, 0.2,0.4,0.6,0.8,1});
  AddHistBinning("PrescaledTriggerPt",{0.,5., 10., 20., 30., 50., 200. });


  vector<double> mvabinsTMP;
  for(int ib =0 ; ib < 50+1; ib++) mvabinsTMP.push_back(ib*0.02);
  AddHistBinning("MVABins", mvabinsTMP);

  return;

}

void AnalyzerCore::AddHistBinning(TString mkey, vector<double> vbins){
  
  int nbin = vbins.size();

  map_hist_bins[mkey]     = vbins;
  map_hist_nbins[mkey]    = nbin-1;
  
  return;
}

vector<double>   AnalyzerCore::GetHistBins(TString k){

  map<TString, vector<double> >::iterator it = map_hist_bins.find(k);
  if ( it == map_hist_bins.end() ) {
    cout << "Error in GetBins " << k << endl;
    exit(EXIT_FAILURE);
  }

  return it->second;

}

int AnalyzerCore::GetHistNBins(TString k){

  auto it = map_hist_nbins.find(k);
  if ( it == map_hist_nbins.end() ) {
    cout << "Error in GetBins" << endl;
    exit(EXIT_FAILURE);
  }
  return it->second;

}


void AnalyzerCore::FillHist(TString histname, double value, double weight, int n_bin, double x_min, double x_max , TString label){

  TH1D *this_hist = GetHist1D(histname);
  if( !this_hist ){
    this_hist = new TH1D(histname, "", n_bin, x_min, x_max);
    this_hist->SetDirectory(NULL);
    this_hist->GetXaxis()->SetTitle(label);
    maphist_TH1D[histname] = this_hist;
  }

  this_hist->Fill(value, weight);

}

void AnalyzerCore::FillHistogram(TString histname, double value, double weight, TString BinLabel,  TString label){
  
  FillHistogram(histname, value, weight, GetHistNBins(BinLabel), GetHistBins(BinLabel), label);
  return ;
}

void AnalyzerCore::FillHistogram(TString histname, double value_x,double value_y,  double weight, TString BinLabelx, TString BinLabely, TString label){

  FillHistogram(histname, value_x, value_y, weight, GetHistNBins(BinLabelx), GetHistBins(BinLabelx), GetHistNBins(BinLabely), GetHistBins(BinLabely));
  return ;
}

void AnalyzerCore::FillHistogram(TString histname, double value_x, double value_y, double weight, int n_binx, vector<double> xbins, int n_biny, vector<double> ybins, TString label){

  TH2D *this_hist = GetHist2D(histname);
  if( !this_hist ){
    
    double arrx_bins [n_binx+1];
    std::copy(xbins.begin(), xbins.end(), arrx_bins);

    double arry_bins [n_biny+1];
    std::copy(ybins.begin(), ybins.end(), arry_bins);

    this_hist = new TH2D(histname, "", n_binx, arrx_bins, n_biny, arry_bins);
    this_hist->SetDirectory(NULL);
    maphist_TH2D[histname] = this_hist;
  }

  this_hist->Fill(value_x, value_y, weight);

  return ;

}



void AnalyzerCore::FillHistogram(TString histname, double value, double weight, int n_bin, vector<double> xbins, TString label){

  TH1D *this_hist = GetHist1D(histname);
  if( !this_hist ){

    double arrx_bins [n_bin+1];
    std::copy(xbins.begin(), xbins.end(), arrx_bins);

    this_hist = new TH1D(histname, "", n_bin, arrx_bins);
    this_hist->SetDirectory(NULL);
    this_hist->GetXaxis()->SetTitle(label);

    maphist_TH1D[histname] = this_hist;
  }

  this_hist->Fill(value, weight);
  return ;

}


void AnalyzerCore::FillHist(TString histname, double value, double weight, int n_bin, double *xbins, TString label){
  

  TH1D *this_hist = GetHist1D(histname);
  if( !this_hist ){

    this_hist = new TH1D(histname, "", n_bin, xbins);
    this_hist->SetDirectory(NULL);
    this_hist->GetXaxis()->SetTitle(label);

    maphist_TH1D[histname] = this_hist;
  }

  this_hist->Fill(value, weight);
  return ;

}

void AnalyzerCore::FillHist(TString histname,
			    double value_x, double value_y,
			    double weight,
			    int n_binx, double x_min, double x_max,
			    int n_biny, double y_min, double y_max){

  TH2D *this_hist = GetHist2D(histname);
  if( !this_hist ){
    this_hist = new TH2D(histname, "", n_binx, x_min, x_max, n_biny, y_min, y_max);
    this_hist->SetDirectory(NULL);
    maphist_TH2D[histname] = this_hist;
  }

  this_hist->Fill(value_x, value_y, weight);
  return ;

}

void AnalyzerCore::FillHist(TString histname,
			    double value_x, double value_y,
			    double weight,
			    int n_binx, double *xbins,
			    int n_biny, double *ybins){

  TH2D *this_hist = GetHist2D(histname);
  if( !this_hist ){
    this_hist = new TH2D(histname, "", n_binx, xbins, n_biny, ybins);
    this_hist->SetDirectory(NULL);
    maphist_TH2D[histname] = this_hist;
  }

  this_hist->Fill(value_x, value_y, weight);

}

void AnalyzerCore::FillHist(TString histname,
                            double value_x, double value_y,
                            double weight,
                            int n_binx, double *xbins,
                            int n_biny, double y_min, double y_max){


  TH2D *this_hist = GetHist2D(histname);
  if( !this_hist ){
    this_hist = new TH2D(histname, "", n_binx, xbins, n_biny, y_min, y_max);
    this_hist->SetDirectory(NULL);
    maphist_TH2D[histname] = this_hist;
    //cout << "CReating TH2D " <<  histname << endl;                                                                                                                                                                                                                                                                                                                        
  }

  this_hist->Fill(value_x, value_y, weight);

}


void AnalyzerCore::FillHist(TString histname,
			    double value_x, double value_y, double value_z,
			    double weight,
			    int n_binx, double x_min, double x_max,
			    int n_biny, double y_min, double y_max,
			    int n_binz, double z_min, double z_max){

  TH3D *this_hist = GetHist3D(histname);
  if( !this_hist ){
    this_hist = new TH3D(histname, "", n_binx, x_min, x_max, n_biny, y_min, y_max, n_binz, z_min, z_max);
    this_hist->SetDirectory(NULL);
    maphist_TH3D[histname] = this_hist;
  }

  this_hist->Fill(value_x, value_y, value_z, weight);

}

void AnalyzerCore::FillHist(TString histname,
			    double value_x, double value_y, double value_z,
			    double weight,
			    int n_binx, const double *xbins,
			    int n_biny, const double *ybins,
			    int n_binz, const double *zbins){

  TH3D *this_hist = GetHist3D(histname);
  if( !this_hist ){
    this_hist = new TH3D(histname, "", n_binx, xbins, n_biny, ybins, n_binz, zbins);
    this_hist->SetDirectory(NULL);
    maphist_TH3D[histname] = this_hist;
  }

  this_hist->Fill(value_x, value_y, value_z, weight);

}

TH1D* AnalyzerCore::JSGetHist1D(TString suffix, TString histname){

  TH1D *h = NULL;

  std::map< TString, std::map<TString, TH1D*> >::iterator mapit = JSmaphist_TH1D.find(suffix);
  if(mapit==JSmaphist_TH1D.end()){
    return h;
  }
  else{

    std::map<TString, TH1D*> this_maphist = mapit->second;
    std::map<TString, TH1D*>::iterator mapitit = this_maphist.find(histname);
    if(mapitit != this_maphist.end()) return mapitit->second;

  }

  return h;

}




void AnalyzerCore::JSFillHist(TString suffix, TString histname, double value, double weight, int n_bin, double x_min, double x_max){

  TH1D *this_hist = JSGetHist1D(suffix, histname);
  if( !this_hist ){

    this_hist = new TH1D(histname, "", n_bin, x_min, x_max);
    (JSmaphist_TH1D[suffix])[histname] = this_hist;

  }

  this_hist->Fill(value, weight);

}

TH2D* AnalyzerCore::JSGetHist2D(TString suffix, TString histname){

  TH2D *h = NULL;

  std::map< TString, std::map<TString, TH2D*> >::iterator mapit = JSmaphist_TH2D.find(suffix);
  if(mapit==JSmaphist_TH2D.end()){
    return h;
  }
  else{

    std::map<TString, TH2D*> this_maphist = mapit->second;
    std::map<TString, TH2D*>::iterator mapitit = this_maphist.find(histname);
    if(mapitit != this_maphist.end()) return mapitit->second;

  }

  return h;

}

void AnalyzerCore::JSFillHist(TString suffix, TString histname,
			      double value_x, double value_y,
			      double weight,
			      int n_binx, double x_min, double x_max,
			      int n_biny, double y_min, double y_max){

  TH2D *this_hist = JSGetHist2D(suffix, histname);
  if( !this_hist ){

    this_hist = new TH2D(histname, "", n_binx, x_min, x_max, n_biny, y_min, y_max);
    (JSmaphist_TH2D[suffix])[histname] = this_hist;

  }

  this_hist->Fill(value_x, value_y, weight);

}

void AnalyzerCore::JSFillHist(TString suffix, TString histname,
			      double value_x, double value_y,
			      double weight,
			      int n_binx, double *xbins,
			      int n_biny, double *ybins){

  TH2D *this_hist = JSGetHist2D(suffix, histname);
  if( !this_hist ){

    this_hist = new TH2D(histname, "", n_binx, xbins, n_biny, ybins);
    (JSmaphist_TH2D[suffix])[histname] = this_hist;

  }

  this_hist->Fill(value_x, value_y, weight);

}


void AnalyzerCore::DeleteHistMaps(){

  for(std::map< TString, TH1D* >::iterator mapit = maphist_TH1D.begin(); mapit!=maphist_TH1D.end(); mapit++)   delete mapit->second;
  maphist_TH1D.clear();

  for(std::map< TString, TH2D* >::iterator mapit = maphist_TH2D.begin(); mapit!=maphist_TH2D.end(); mapit++)  delete mapit->second;
  maphist_TH2D.clear();

  for(std::map< TString, TH3D* >::iterator mapit = maphist_TH3D.begin(); mapit!=maphist_TH3D.end(); mapit++)  delete mapit->second;
  maphist_TH3D.clear();

  return;
}



void AnalyzerCore::WriteHist() {
  // Set max compression
  outfile->SetCompressionLevel(9);
  outfile->cd();

  WriteProfile();

  // Determine if in memory-saving mode
  bool doOptimize = runSyst;

  // Lambda to write THxD maps (TH1D/TH2D/TH3D)
  auto write_hist_map = [&](auto& hist_map) {
    for (auto& [fullpath, hist] : hist_map) {
      if (doOptimize) {
        if (hist->GetEntries() == 0 || hist->GetSumOfWeights() == 0) continue;
      }

      TString name = hist->GetName();
      TString hist_name = name(name.Last('/') + 1, name.Length());
      TString dir_path  = name(0, name.Last('/'));

      TDirectory* dir = outfile->GetDirectory(dir_path);
      if (!dir) dir = outfile->mkdir(dir_path);
      outfile->cd(dir_path);

      hist->SetName(hist_name);
      hist->Write();
    }
    outfile->cd();
  };

  // TH1D, TH2D, TH3D
  write_hist_map(maphist_TH1D);
  write_hist_map(maphist_TH2D);
  write_hist_map(maphist_TH3D);

  // JS maps
  auto write_js_map = [&](auto& js_map) {
    for (auto& [dir_path, hist_map] : js_map) {
      TDirectory* dir = outfile->GetDirectory(dir_path);
      if (!dir) dir = outfile->mkdir(dir_path);
      outfile->cd(dir_path);

      for (auto& [_, hist] : hist_map) {
        if (doOptimize) {
          if (hist->GetEntries() == 0 || hist->GetSumOfWeights() == 0) continue;
        }

        hist->Write();
      }

      outfile->cd();
    }
  };

  write_js_map(JSmaphist_TH1D);
  write_js_map(JSmaphist_TH2D);

  //==== Timer Hist ====
  if (TimerMap.size() > 1) {
    std::vector<TString> TimerLabels;
    for (auto& i : TimeTagMatcher) TimerLabels.push_back(i.second);
    std::sort(TimerLabels.begin(), TimerLabels.end());

    TH1* timer_hist = new TH1D("TimeHist", "", TimerLabels.size(), 0, TimerLabels.size());
    for (unsigned int i = 0; i < TimerLabels.size(); i++)
      timer_hist->GetXaxis()->SetBinLabel(i + 1, TimerLabels[i]);

    timer_hist->SetDirectory(nullptr);

    for (auto& i : TimerMap) {
      if (i.first != "LATEST") {
        //std::cout << i.first << " processing time = " << i.second << std::endl;
        timer_hist->Fill(i.first, i.second);
      }
    }

    TDirectory* dir = outfile->GetDirectory("Timer");
    if (!dir) dir = outfile->mkdir("Timer");
    outfile->cd("Timer");
    timer_hist->Write();
    outfile->cd();
    delete timer_hist;
  }
  
}






void AnalyzerCore::FillEventComparisonFile(AnalyzerParameter param,TString label,string date, double w){

  //// Make TEX file  

  ofstream ofile_tex;
  string lqdir = getenv("SKFlat_WD");

  if(!IsData)label = label  +"_" +MCSample;
  else label = label  +"_" +GetEra();

  string compfile = string(getenv("SKFlatEvCompDir")) +  "/"+date + "/"+string(param.Name)+string(label)+".txt";

  cout << "FillEventComparisonFile  " << label << " " << compfile << endl;
  ofile_tex.open(compfile.c_str(),ios::out | ios::app);

  ofile_tex.setf(ios::fixed,ios::floatfield);
  ofile_tex << "[ "<<getenv("USER") << " : " << run << " : " << event << " : " << param.Muon_Tight_ID << " : " << param.Electron_Tight_ID << " :  " << w <<   "]" << endl;

  ofile_tex.close();

  return;
}

void  AnalyzerCore::FillTimer(TString inittag){

  TString tag = "";
  if (TimeTagMatcher.find(inittag) == TimeTagMatcher.end()) {
    tag = TString(std::to_string(TimeTagMatcher.size())) +"_"+inittag;
    TimeTagMatcher[inittag] = tag;
  }
  else{
    auto itr = TimeTagMatcher.find(inittag);
    tag = itr->second;
  }

  if (TimerMap.find(tag) == TimerMap.end()) {
    auto itr = TimerMap.find("LATEST");
    double last_time = itr->second;
    TimerMap[tag] = (std::clock() - last_time)/ CLOCKS_PER_SEC;
  }
  else{
    auto itr = TimerMap.find("LATEST");
    auto itr2= TimerMap.find(tag);
    TimerMap[tag]= itr2->second + ( (std::clock() - itr->second)/ CLOCKS_PER_SEC);
  }
  TimerMap["LATEST"] =std::clock() ;

  if(_jentry==0) return;

  if(_jentry%10000==0){
    vector<TString> TimerLabels;
    for(auto i: TimeTagMatcher) TimerLabels.push_back(i.first);
    for(auto i : TimerMap) {
      if(i.first != "LATEST")    cout << i.first << " processing time = " << i.second << endl;
    }
  }

  return;
}
void  AnalyzerCore::AddTimerStamp(TString tag){

  if (TimingMap.find(tag) == TimingMap.end()) TimingMap[tag] = std::clock();

  return;

}

