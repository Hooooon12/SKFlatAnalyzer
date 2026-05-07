#ifndef mylib_h
#define mylib_h

#include "TROOT.h"
#include  "canvas_margin.h"
#include  "base_functions.h"

TH1D* GetSignalHistBasics(TString current_sample, TString filepath, TString fullhistname){
   TH1D* hist_temp;
  TFile* file = new TFile(filepath);
  if( !file ){
    cout << "No file : " << filepath << endl;
    return hist_temp;
  }


  hist_temp = (TH1D*)file->Get(fullhistname);
  return hist_temp;
}



TString GetXTitle(map<TString,TString> sigmap,TString n_sr_hist){
  for (map<TString,TString>::iterator it = sigmap.begin(); it != sigmap.end(); it++){
    TH1D* sig =GetSignalHistBasics(it->first,it->second,n_sr_hist);
    if(!sig) continue;
    return sig->GetXaxis()->GetTitle();
  }
  return "";
  
}

TString GetYTitle(map<TString,TString> sigmap,TString n_sr_hist){
  for (map<TString,TString>::iterator it = sigmap.begin(); it != sigmap.end(); it++){
    TH1D* sig =GetSignalHistBasics(it->first,it->second,n_sr_hist);
    if(!sig) continue;
    return sig->GetYaxis()->GetTitle();
  }
  return "";

}

TLegend* MakeRatioLegend( TH1D* h1, TH1D* h2){
  
  double x1 = 0.7;
  double y1 = 0.22;
  double x2 = 0.95;
  double y2 = 0.25;
  
  TLegend* legendH = new TLegend(x1,y1,x2,y2);
  legendH->SetFillColor(kWhite);
  legendH->SetTextFont(42);

  legendH->SetBorderSize(0);
  legendH->SetTextSize(0.02);
  legendH->SetNColumns(2);

  legendH->AddEntry(h1,"Stat.","f");
  legendH->AddEntry(h2,"Syst.+Stat.","f");
  
  return legendH;
}


double GetMax(vector<double> vec){

  double val=-99999999.;

  for (auto i: vec){
    if(i > val) val = i;
  }
  
  return val;
}

void    FormatHistLine(TH1D* &hist_data, int rbin,  Color_t col,int lineStyle){

  if(rbin != 1) hist_data->Rebin(rbin);
  hist_data->SetLineColor(col);
  hist_data->SetLineWidth(3);
  hist_data->SetLineStyle(lineStyle);
  hist_data->SetMarkerStyle(20);
  hist_data->SetMarkerSize(1.2);
  hist_data->SetMarkerColor(col);
  return;
}


void    FormatHist(TH1D* &hist_data, int rbin, double _max, double xmin, double xmax, Color_t col,int lineStyle, TString xaxis_title, TString yaxis_title){

  if(rbin != 1) hist_data->Rebin(rbin);
  hist_data->SetLineColor(col);
  hist_data->SetLineWidth(3);
  hist_data->SetLineStyle(lineStyle);
  hist_data->GetYaxis()->SetTitleOffset(1.4);
  hist_data->SetMarkerStyle(20);
  hist_data->SetMarkerSize(1.2);
  hist_data->SetMarkerColor(col);
  hist_data->GetXaxis()->SetRangeUser(xmin, xmax);
  if(_max > 0 )hist_data->GetYaxis()->SetRangeUser(0.1, _max*1.4);
  if(xaxis_title != "")hist_data->GetXaxis()->SetTitle(xaxis_title);
  cout << "Setting hist title to " << yaxis_title << endl;
  hist_data->GetYaxis()->SetTitle(yaxis_title);
  return;
}

void    FormatHist(TH1D* &hist_data, int rbin, double _max,  Color_t col,TString xaxis_title, TString yaxis_title=""){

  if(rbin != 1) hist_data->Rebin(rbin);
  hist_data->SetLineColor(col);
  //hist_data->SetLineWidth(4);
  hist_data->GetYaxis()->SetTitleOffset(1.4);
  hist_data->SetMarkerStyle(20);
  hist_data->SetMarkerSize(1.2);
  hist_data->SetMarkerColor(col);
  if(_max > 0 )hist_data->GetYaxis()->SetRangeUser(0.1, _max*1.4);
  if(xaxis_title != "")hist_data->GetXaxis()->SetTitle(xaxis_title);
  if(yaxis_title != "")hist_data->GetYaxis()->SetTitle(yaxis_title);
}

void    FormatHistogram(TH1D* hist, TString var, int col, double ymax){

  double max = (ymax > hist->GetMaximum()) ? ymax : hist->GetMaximum();
  if(var.Contains("DeltaPhi")) FormatHist (hist,5, max, GetHistColor(col), "#Delta#phi");
  else if(var.Contains("Lep_Mu_FromW_pt")) FormatHist (hist,2, max, GetHistColor(col), "Lep_Mu_FromW_pt");
  else if(var.Contains("_eta")) FormatHist (hist,2, max, GetHistColor(col), "#eta");
  else if(var.Contains("DeltaR")) FormatHist (hist,2, max, GetHistColor(col), "#Delta(R)");
  else if(var.Contains("_pt")) FormatHist (hist,2, max, GetHistColor(col), "p_{T} (GeV)");

}


void    SetupHist(TH1D* hist_data, int rbin, double _max){
  hist_data->Rebin(rbin);
  hist_data->SetLineColor(kBlack);
  hist_data->GetYaxis()->SetTitleOffset(1.4);
  hist_data->SetMarkerStyle(20);
  hist_data->SetMarkerSize(1.2);
  hist_data->SetMarkerColor(kBlack);
  hist_data->SetLineColor(kBlack);
  hist_data->GetYaxis()->SetRangeUser(0.1, _max*1.4);

}

double GetBinValue(double value, TH1* h, TString mode){

  if(mode =="RL"){
    for (Int_t i=h->GetNbinsX(); i > 0 ; i--)  {
      if(h->GetBinContent(i) < value) return  h->GetBinLowEdge(i);
    }
  }
  if(mode =="LR"){
    for (Int_t i=1; i < h->GetNbinsX(); i++)  {
      if(h->GetBinContent(i) < value) return  h->GetBinLowEdge(i);
    }
  }

  return -999.;
}

void GetEfficiencyHist(TH1* h, TH1* h2, TString mode){

  if(mode =="RL"){

    double integral_hist = h->Integral();
    h->GetYaxis()->SetRangeUser(0.01, 1.1);
    for (Int_t i=1;i<=h->GetNbinsX()+1;i++)       h->SetBinContent(i, h2->Integral(0, i) / integral_hist);
    
  }
  if(mode =="LR"){

    double integral_hist = h->Integral();
    h->GetYaxis()->SetRangeUser(0.01, 1.1);
    for (Int_t i=1;i<=h->GetNbinsX()+1;i++) {
      h->SetBinContent(i, h2->Integral(i, h2->GetNbinsX()) / integral_hist);
    }

  }
}

TH1D* GetDataError(TH1D* h_data, TH1D* h_nominal , TH1D* h_up, TH1D* h_down){

  TH1D* hdev_err_stat = (TH1D*)h_data->Clone("hdev_err");
    
  for (Int_t i=1;i<=h_data->GetNbinsX()+1;i++) {
    hdev_err_stat->SetBinContent(i, 1.0);
    if(h_nominal->GetBinContent(i) > 0 &&  h_data->GetBinContent(i) > 0){
      hdev_err_stat->SetBinError(i, (h_up->GetBinContent(i)-h_nominal->GetBinContent(i))/h_nominal->GetBinContent(i) );
    }
    else{
      hdev_err_stat->SetBinError(i, 0.0);
    }
  }
  return hdev_err_stat;
}

TGraphAsymmErrors* GetratioGraph(TH1D* h_data,TH1D* h_nominal, vector<double> err_up_tmp, vector<double> err_down_tmp){

  TGraphAsymmErrors * gratio = new TGraphAsymmErrors(h_data);
  
  for (int i = 0; i < gratio->GetN(); ++i) {
    
    if(err_down_tmp.at(i)  !=0.) {

      gratio->SetPointEYlow(i, err_down_tmp.at(i) / h_nominal->GetBinContent(i+1) );
      gratio->SetPointEXlow(i, 0);
      gratio->SetPointEYhigh(i, err_up_tmp.at(i) /h_nominal->GetBinContent(i+1));
      gratio->SetPointEXhigh(i, 0);
    }
    else{
      gratio->SetPointEYlow(i, 0);
      gratio->SetPointEXlow(i, 0);
      gratio->SetPointEYhigh(i, 1.8 / h_nominal->GetBinContent(i+1));
      gratio->SetPointEXhigh(i, 0);
    }
  }
  return gratio;
  
}


TH1D* GetRatioHist(TH1D* h1, TH1D* h2,TString tag){

  TH1D* hdev = (TH1D*)h1->Clone("hdev"+tag);
  
  hdev->Divide(h1,h2,1.,1.,"cl=0.683 b(1,1) mode");

  return hdev;
}

TPad* GetPad(double SUBFIGURE_MARGIN, double FIGURE2_RATIO){

   TPad *p = new TPad( "p_test", "", 0, 0, 1, 1.0 - SUBFIGURE_MARGIN, 0, 0, 0);  // create new pad, fullsize to have equal font-sizes in both plots
   p->SetTopMargin(1-FIGURE2_RATIO);   // top-boundary (should be 1 - thePad->GetBottomMargin() )
   p->SetFillStyle(0);     // needs to be transparent
   return p;
}
void SetBinLabels(TH1D* hist, std::vector<TString> list){

  for(unsigned int i = 0 ; i < list.size(); ++i){
    
    TString im = list.at(i);
    hist->GetXaxis()->SetBinLabel(i+1,im);
    
  }
  
  return;
           
}
TGraphAsymmErrors * GetDataGraph(TH1D* hist,vector<double>& err_up_tmp , vector<double>& err_down_tmp){

  const double alpha = 1 - 0.6827;

  TGraphAsymmErrors * g = new TGraphAsymmErrors(hist);
  for (int i = 0; i < g->GetN(); ++i) {
    int N = g->GetY()[i];
    double L =  (N==0) ? 0  : (ROOT::Math::gamma_quantile(alpha/2,N,1.));
    double U =  (N==0) ?  ( ROOT::Math::gamma_quantile_c(alpha,N+1,1) ) :
      ( ROOT::Math::gamma_quantile_c(alpha/2,N+1,1) );
      if ( N!=0 ) {
	g->SetPointEYlow(i, N-L );
	g->SetPointEXlow(i, 0);
	g->SetPointEYhigh(i, U-N );
	g->SetPointEXhigh(i, 0);
	err_down_tmp.push_back(N-L);
        err_up_tmp.push_back(U-N);
	
      }
      else {
	g->SetPointEYlow(i, 0.1);
	g->SetPointEXlow(i, 0.);
	g->SetPointEYhigh(i, 1.8);
	g->SetPointEXhigh(i, 0.);
	err_down_tmp.push_back(0.);
        err_up_tmp.push_back(1.8);
      }
  }

  g->SetLineWidth(2.0);
  g->SetMarkerSize(0.);
  return g;
  
}

TGraph* GetExp17028(TString channel){

  const int nm_17028 = 19;
  double mass_17028[nm_17028] = {
				 100, 125, 150,200,
				 250, 300, 400, 500,
				 600, 700, 800, 900,
				 1000, 1100, 1200, 1300,
				 1400, 1500, 1700
  };

   double obs_17028[nm_17028], exp_17028[nm_17028];
  vector<double> tempvec_obs_17028, tempvec_exp_17028;
  vector<double> scales_17028;

  if(channel=="MuMu"){
    tempvec_exp_17028 = {
      175.333, 21.5041, 32.925, 56.3397,
      70.8081, 99.3095, 20.4264, 42.5126,
      60.1695, 116.721, 15.8605, 25.8407,
      38.43, 64.346, 100.265, 151.699,
      247.709, 340.424, 1340.34
    } ;
    scales_17028 = {
      0.001, 0.01, 0.01,0.01,0.01,0.01,0.1, 0.1,0.1,0.1,1,1,1,1,1,1,1,1,1,1
    };
    tempvec_obs_17028 = {
      215.218, 23.0424,41.8101,49.4399,57.1134,84.404,39.6932,
      44.5303,81.4561,195.31,16.3137,42.605,61.6358,103.589,150.295,
      220.286, 365.037, 516.2, 1408.5

    };
      }
  else   if(channel=="EE"){
    //https://github.com/jedori0228/HiggsAnalysis-CombinedLimit/blob/2016Data_HNDilepton_Limit/data/2016_HNDiLepton/Outputs_Tool/EE_Combined/result.txt      \
                                                                                                                                                              
    tempvec_exp_17028 = {
      467.448, 65.4099, 90.4068, 159.838,216.957, 284.563, 59.74, 94.6793,
      104.302, 183.121, 30.189, 47.1442, 72.0759, 117.305, 183.214,
      285.811, 434.08, 644.258, 2506.94
    };
    scales_17028 = {
      0.001, 0.01, 0.01,0.01,0.01,0.01,0.1, 0.1,0.1,0.1,1,1,1,1,1,1,1,1,1,1
    };
    tempvec_obs_17028 = {
      368.924, 63.3389, 61.9159, 151.2,
      206.654, 254.261, 68.8604, 95.9664,
      123.0, 274.57, 24.8148, 46.0243,
      95.1426, 164.011, 252.706, 379.988,
      419.316, 631.767, 2486.31

    };
  }

   for(unsigned int j=0; j<tempvec_obs_17028.size(); j++){

    obs_17028[j] = scales_17028[j]*tempvec_obs_17028.at(j)*0.01;
    exp_17028[j] = scales_17028[j]*tempvec_exp_17028.at(j)*0.01;
  }

  TGraph *gr_17028_exp = new TGraph(nm_17028, mass_17028, exp_17028);
  gr_17028_exp->SetLineColor(kRed);
  //gr_8TeV_exp->SetLineStyle(10);                                                                                                                           \
                                                                                                                                                              
  gr_17028_exp->SetLineWidth(3);

  gr_17028_exp->SetLineStyle(2);


  return gr_17028_exp;

}

void SetNomBinError(TH1D* hnom, TH1D* hup, TH1D* hdown){

  for(int i=1; i < hnom->GetNbinsX()+1; i++){

    float err1 = fabs(hnom->GetBinContent(i)- hup->GetBinContent(i));
    float err2 = fabs(hnom->GetBinContent(i)- hdown->GetBinContent(i));

    if(err1 > err2 ) hnom->SetBinError(i, err1);
    if(err2 > err1 ) hnom->SetBinError(i, err2);
  }
  return;
}

TH1D* MakeSumHist2(THStack* thestack){

  TH1D* hsum=0;
  TList* list = thestack->GetHists();
  TIter it(list, true);
  TObject* obj=0;
  while( (obj = it.Next()) ) {
    TH1D* h = dynamic_cast<TH1D*>(obj);
    if(!hsum) hsum = (TH1D*)h->Clone( (string(h->GetName()) + "_sum").c_str() );
    else {
      hsum->Add(h, 1.0);
    }
  }//hist loop

  return hsum;
}

TH1D* MakeErrorBand(TH1D* hnom, TH1D* hup, TH1D* hdown){

  TH1D* errorband = (TH1D*)hnom->Clone("aa");
  for(int i=1; i < errorband->GetNbinsX()+1; i++){

    float bin_content = (hup->GetBinContent(i)+ hdown->GetBinContent(i))/2.;
    float bin_error = (hup->GetBinContent(i)- hdown->GetBinContent(i))/2.;

    errorband->SetBinContent(i,bin_content);
    errorband->SetBinError(i,bin_error);
  }

  errorband->SetFillStyle(3004);
  errorband->SetFillColor(kBlue-2);
  errorband->SetMarkerSize(0);
  errorband->SetMarkerStyle(0);
  errorband->SetLineColor(kWhite);
  errorband->Draw("E2Same");

  return errorband;

}

map<TString,int> ConfigMapInt(TString config_file){

  map<TString,int> conf_map;

  ifstream config_file_name (config_file);

  if(!config_file_name){
    cerr << "Did not find "+config_file+", exiting ..." << endl;
    return conf_map;
  }


  while(!config_file_name.eof()) {
    string tmp;
    string tmp1;
    int tmppath;
    config_file_name >> tmp;
    if(tmp=="config_int") {
      config_file_name >> tmp1;
      config_file_name >> tmppath;
      cout << "ConfigMapInt : " << tmp1 << " " << tmppath << endl;
      conf_map[tmp1] = tmppath;
    }
  }

  return conf_map;
}

map<TString,double> ConfigMapDouble(TString config_file){

  map<TString,double> conf_map;

  ifstream config_file_name (config_file);

  if(!config_file_name){
    cerr << "Did not find "+config_file+", exiting ..." << endl;
    return conf_map;
  }


  while(!config_file_name.eof()) {
    string tmp;
    string tmp1;
    double tmppath;
    config_file_name >> tmp;
    if(tmp=="config_double") {
      config_file_name >> tmp1;
      config_file_name >> tmppath;

      conf_map[tmp1] = tmppath;
    }
  }

  return conf_map;
}


map<TString,TString> ConfigMap(TString config_file){

  map<TString,TString> conf_map;

  ifstream config_file_name (config_file);

  if(!config_file_name){
    cerr << "Did not find "+config_file+", exiting ..." << endl;
    return conf_map;
  }


  while(!config_file_name.eof()) {
    string tmp;
    string tmp1;
    string tmppath;
    config_file_name >> tmp;
    if(tmp=="config") {
      config_file_name >> tmp1;
      config_file_name >> tmppath;

      conf_map[tmp1] = tmppath;
    }
  }

  return conf_map;
}


map<TString,Color_t> SigColorMap(TString config_file){


  map<TString,Color_t> conf_map;

  ifstream config_file_name (config_file);

  if(!config_file_name){
    cerr << "Did not find "+config_file+", exiting ..." << endl;
    return conf_map;
  }


  while(!config_file_name.eof()) {
    string tmp;
    string tmp1;
    string tmppath;
    Color_t col;
    config_file_name >> tmp;

    if(tmp=="signal") {
      config_file_name >> tmp1;
      config_file_name >> tmppath;
      config_file_name >> col;
      conf_map["mn"+tmp1] = col;
    }
  }
  return conf_map;
}


map<TString,Color_t> BkgColorMap(TString config_file){

  
  map<TString,Color_t> conf_map;

  ifstream config_file_name (config_file);

  if(!config_file_name){
    cerr << "Did not find "+config_file+", exiting ..." << endl;
    return conf_map;
  }


  while(!config_file_name.eof()) {
    string tmp;
    string tmp1;
    string tmppath;
    Color_t col;
    config_file_name >> tmp;

    if(tmp=="sample") {
      config_file_name >> tmp1;
      config_file_name >> tmppath;
      config_file_name >> col;
      conf_map[tmp1] = col;
    }
  }
  return conf_map;
}

map<TString,vector<TString> > BkgConfigMap(TString config_file){

  map<TString,vector<TString > > conf_map;

  ifstream config_file_name (config_file);

  if(!config_file_name){
    cerr << "Did not find "+config_file+", exiting ..." << endl;
    return conf_map;
  }

  
  while(!config_file_name.eof()) {
    string tmp;
    string tmp1;
    string tmppath;
    config_file_name >> tmp;

    if(tmp=="sample") {
      config_file_name >> tmp1;
      config_file_name >> tmppath;
      map<TString,vector<TString > > ::iterator it = conf_map.find(tmp1);
      if(it != conf_map.end()) conf_map[tmp1].push_back(tmppath);
      else {
	vector<TString> samples = {tmppath};
	conf_map[tmp1] = samples;
      }
    }
  }
  return conf_map;
}

map<TString,TString> SigConfigMap(TString config_file){

  map<TString,TString> conf_map;

  ifstream config_file_name (config_file);

  if(!config_file_name){
    cerr << "Did not find "+config_file+", exiting ..." << endl;
    return conf_map;
  }


  while(!config_file_name.eof()) {
    string tmp;
    string tmp1;
    string tmppath;
    config_file_name >> tmp;

    if(tmp=="signal") {
      config_file_name >> tmp1;
      config_file_name >> tmppath;
      conf_map["mn"+tmp1] = tmppath;
    }
  }
  return conf_map;
}


void SetXaxisRange(TH1D* hist){

  double this_x_min = hist->GetXaxis()->GetBinLowEdge(1);
  double this_x_max = hist->GetXaxis()->GetBinUpEdge( hist->GetXaxis()->GetNbins() );


  hist->GetXaxis()->SetRangeUser(this_x_min, this_x_max);
}


double GetMinimumBinValue(TH1D* hist){

  double minValue = 100000000;
  for(unsigned int i=1; i<=hist->GetNbinsX()+1; i++){
    if(hist->GetBinContent(i) < minValue) minValue = hist->GetBinContent(i);
  }
  return minValue;
}

TH1D* MakeOverflowBin(TH1D* hist, TString ref=""){

 
  int n_bin_origin = hist->GetXaxis()->GetNbins();

  //==== Changed NBins                                                                                                                                                                                                                        
  int bin_first = hist->GetXaxis()->GetFirst();
  int bin_last = hist->GetXaxis()->GetLast();
  int n_bin_inrange = bin_last-bin_first+1;
  

  double x_first_lowedge = hist->GetXaxis()->GetBinLowEdge(bin_first);
  double x_last_upedge = hist->GetXaxis()->GetBinUpEdge(bin_last);
  
  double Allunderflows = hist->Integral(0, bin_first-1);
  double Allunderflows_error = hist->GetBinError(0);


  Allunderflows_error = Allunderflows_error*Allunderflows_error;
  
  for(unsigned int i=1; i<=bin_first-1; i++){
    Allunderflows_error += (hist->GetBinError(i))*(hist->GetBinError(i));
  }
  Allunderflows_error = sqrt(Allunderflows_error);

  double Alloverflows = hist->Integral(bin_last+1, n_bin_origin+1);
  double Alloverflows_error = hist->GetBinError(n_bin_origin+1);
  Alloverflows_error = Alloverflows_error*Alloverflows_error;
  for(unsigned int i=bin_last+1; i<=n_bin_origin; i++){
    Alloverflows_error += (hist->GetBinError(i))*(hist->GetBinError(i));
  }
  Alloverflows_error = sqrt(Alloverflows_error);

  //==== Make X-bin array                                                                                                                                                                                                                     
  Double_t temp_xbins[n_bin_inrange+1];
  int counter=0;
  for(int i=bin_first;i<=bin_last;i++){
    temp_xbins[counter] = hist->GetXaxis()->GetBinLowEdge(i);
    counter++;
  }
  temp_xbins[n_bin_inrange+1-1] = hist->GetXaxis()->GetBinUpEdge(bin_last);


  const Double_t *xcopy=temp_xbins;
  TH1D *hist_out = new TH1D(hist->GetName()+ref, hist->GetTitle(), n_bin_inrange, xcopy);
  for(unsigned int i=1; i<=n_bin_inrange; i++){
    double this_content = hist->GetBinContent(bin_first-1+i);
    double this_error = hist->GetBinError(bin_first-1+i);
    //cout << "["<<hist_out->GetXaxis()->GetBinLowEdge(i)<<", "<<hist_out->GetXaxis()->GetBinUpEdge(i)<<"] : "<<this_content<<endl;                                                                                                           

    //==== underflows                                                                                                                                                                                                                         
    if(i==1){
      this_content += Allunderflows;
      this_error = TMath::Sqrt( this_error*this_error + Allunderflows_error*Allunderflows_error );
    }

    //==== overflows                                                                                                                                                                                                                          
    if(i==n_bin_inrange){
      this_content += Alloverflows;
      this_error = TMath::Sqrt( this_error*this_error + Alloverflows_error*Alloverflows_error );
    }

    hist_out->SetBinContent(i, this_content);
    hist_out->SetBinError(i, this_error);

  }

  return hist_out;

}

			  
TH1D* GetSignalHist(TLegend* legend_g,TString current_sample, TString filepath, TString fullhistname,pair<Color_t,int> col, bool addleg){

  TH1D* hist_temp;
  TFile* file = new TFile(filepath);
  if( !file ){
    cout << "No file : " << filepath << endl;
    return hist_temp;
  }
  
    
  hist_temp = (TH1D*)file->Get(fullhistname);
  
  hist_temp->SetLineColor(col.first);
  hist_temp->SetLineStyle(col.second);
  hist_temp->SetLineWidth(2.);
    
  if(addleg)legend_g->AddEntry(hist_temp,current_sample+" GeV","l");

  return hist_temp;
}

double GetIntegral(map<TString,vector<TString> > _map, TString fullhistname){

  double total(0.);
  
  for (map<TString,vector<TString> >::iterator it = _map.begin(); it != _map.end(); it++){
    
    TString current_sample = it->first;
    TString filepath       = it->second[0];
    
    TFile* file = new TFile(filepath);
    if( !file ){
      cout << "No file : " << filepath << endl;
      continue;
    }

    TH1D* hist_temp = (TH1D*)file->Get(fullhistname);
    if(!hist_temp || hist_temp->GetEntries() == 0){
      cout << "No histogram : " << current_sample << " " << fullhistname << " " <<  filepath << endl;
      file->Close();
      delete file;
      continue;
    }
    for(unsigned int i=1; i < it->second.size(); i++){
      TString _filepath       = it->second[i];
    
      TFile* _file = new TFile(_filepath);
      if( !_file ){
	cout << "No file : " << _filepath << endl;
	continue;
      }
      TH1D* _hist_temp = (TH1D*)_file->Get(fullhistname);
      
      hist_temp->Add(_hist_temp);
    }
    
    total+= hist_temp->Integral();
    file->Close();

    /// delete pointer
    delete file;
  }

  return total;
}
THStack*  MakeStack(TLegend* legend_g,map<TString,vector<TString> > _map, TString fullhistname, map<TString,Color_t> _colmap, int rbin, int sys){

  THStack* MC_stacked = new THStack("MC_stacked", "");

  for (map<TString,vector<TString> >::iterator it = _map.begin(); it != _map.end(); it++){

    TString current_sample = it->first;
    TString filepath       = it->second[0];

    TFile* file = new TFile(filepath);
    if( !file ){
      cout << "No file : " << filepath << endl;
      continue;
    }
    
    TH1D* hist_temp = (TH1D*)file->Get(fullhistname);

    if(!hist_temp || hist_temp->GetEntries() == 0){
      cout << "No histogram : " << current_sample << " " << fullhistname << " " <<  filepath << endl;
      file->Close();
      delete file;
      continue;
    }

    for(unsigned int i=1; i < it->second.size(); i++){
      TString _filepath       = it->second[i];

      TFile* _file = new TFile(_filepath);
      if( !_file ){
	cout << "No file : " << _filepath << endl;
	continue;
      }
      TH1D* _hist_temp = (TH1D*)file->Get(fullhistname);
      hist_temp->Add(_hist_temp);
    }
    hist_temp->SetName(fullhistname+"_"+current_sample);

    SetXaxisRange(hist_temp);
    TH1D *hist_final = MakeOverflowBin(hist_temp);
    hist_final->GetXaxis()->SetTitle(hist_temp->GetXaxis()->GetTitle());
    hist_final->GetYaxis()->SetTitle(hist_temp->GetYaxis()->GetTitle());
    //==== Remove Negative bins                                                                                                                                               
    TAxis *xaxis = hist_final->GetXaxis();
    for(int ccc=1; ccc<=xaxis->GetNbins(); ccc++){
      //if(DoDebug) cout << current_sample << "\t["<<xaxis->GetBinLowEdge(ccc) <<", "<<xaxis->GetBinUpEdge(ccc) << "] : " << hist_final->GetBinContent(ccc) << endl;
      if(hist_final->GetBinContent(ccc)<0){
	hist_final->SetBinContent(ccc, 0.);
	hist_final->SetBinError(ccc, 0.);
      }
    }
    
    map<TString,Color_t>::iterator color_it;
    color_it= _colmap.find(current_sample);
    hist_final->SetFillColor(color_it->second);
    hist_final->SetLineColor(color_it->second);
    if(sys==0)legend_g->AddEntry(hist_final,current_sample,"f");
    if(filepath.Contains("Fake") && sys>0) hist_final->Scale(1.3) ;
    if(filepath.Contains("Fake") && sys<0) hist_final->Scale(0.7) ;
    if(filepath.Contains("CF") && sys>0) hist_final->Scale(1.25) ;
    if(filepath.Contains("CF") && sys<0) hist_final->Scale(0.75) ;
    if(filepath.Contains("Prompt") && sys>0) hist_final->Scale(1.2) ;
    if(filepath.Contains("Prompt") && sys<0) hist_final->Scale(0.8) ;

    if(fabs(sys) == 2){
      for(int ccc=1; ccc<=xaxis->GetNbins(); ccc++){
	if(sys > 0)hist_final->SetBinContent(ccc, hist_final->GetBinContent(ccc) + hist_final->GetBinError(ccc));
	if(sys < 0)hist_final->SetBinContent(ccc, hist_final->GetBinContent(ccc) - hist_final->GetBinError(ccc));
      }
    }
    hist_final->Rebin(rbin);
    MC_stacked->Add(hist_final);


    /// delete pointer
                                                                                                                                      
    delete file;

  }


  return MC_stacked;

}

vector<TString> GetListFromKeys(TFile* _file, TString dirname, TString _type){

  vector<TString> vlist;
  
  //if(CheckFile(_file) > 0)  return vlist;
  TDirectory* _dir = _file->GetDirectory(dirname);
  TList* list = _dir->GetListOfKeys() ;
  TIter next(list) ;
  TKey* key ;
  TObject* obj ;

  while ( (key = (TKey*)next()) ) {
    obj = key->ReadObj() ;
    TString hname = obj->GetName();
    TString objname= obj->ClassName();
    
    if(!(objname == _type)) continue;

    hname = hname.ReplaceAll(dirname+"/", "");
    vlist.push_back(hname);

  }

  return vlist;
}

vector<TString> GetHistNames(TString file, TString dirname, TString analyzername,TString flavour, TString ID){

  cout << "GetHistNames[] "<< endl;
  vector<TString> vlist;

  TFile * _file = new TFile(file);
  if(CheckFile(_file) > 0)  return vlist;
  TDirectory* _dir = _file->GetDirectory(dirname);
  TList* list = _dir->GetListOfKeys() ;
  TIter next(list) ;
  TKey* key ;
  TObject* obj ;

  while ( (key = (TKey*)next()) ) {
    obj = key->ReadObj() ;
    TString hname = obj->GetName();
    TString objname= obj->ClassName();
    
    if(!(hname.Contains(ID+"_"+flavour+"_"+dirname))) continue;

    //nPV_HNtypeI_JA_EE_HEEP2018
    //    //POG_SingleMuon_OnZ_OS/NEvent_POG_SingleMuon_OnZ_OS                                                                                                                                                                            
    cout << ID+"_"+flavour+"_"+dirname << endl;
    hname = hname.ReplaceAll(ID+"_"+flavour+"_"+dirname,"");
    hname = hname.ReplaceAll("/","");
    //hname = hname.ReplaceAll("_","");
    vlist.push_back(hname);
  }

  /// delete pointer                                                                                                                   
  delete file;

  return vlist;

}
vector<TString> GetIDNames(TString file, TString dirname, TString flavour, TString region){

  
  vector<TString> vlist;

  TFile * _file = new TFile(file);
  if(CheckFile(_file) > 0)  return vlist;
  TDirectory* _dir = _file->GetDirectory(dirname);
  TList* list = _dir->GetListOfKeys() ;
  TIter next(list) ;
  TKey* key ;
  TObject* obj ;

  while ( (key = (TKey*)next()) ) {
    obj = key->ReadObj() ;
    TString hname = obj->GetName();
    TString objname= obj->ClassName();
    //cout << hname << " " << dirname+"_njets_"+analyzername+"_"+flavour << " JOHN" <<  endl;
    if(!hname.Contains("NEvent_")) continue;
    //POG_SingleMuon_OnZ_OS/NEvent_POG_SingleMuon_OnZ_OS
    cout << flavour << " " << region << endl;
    hname = hname.ReplaceAll("_"+flavour+"_"+region,"");
    hname = hname.ReplaceAll("/NEvent_","");
    //hname = hname.ReplaceAll("_","");

    std::string s = std::string(hname);
    std::string half = s.substr(0, s.length()/2);

    cout << "---> " << half << endl;
    vlist.push_back(TString(half));
  }
  /// delete pointer                                                                                                                   
  delete _file;

  return vlist;
   
}
vector<TString> GetDirName(TString file){

  vector<TString> vlist;
  
  TFile * _file = new TFile(file);
  if(CheckFile(_file) > 0)  return vlist;

  TList* list = _file->GetListOfKeys() ;
  TIter next(list) ;
  TKey* key ;
  TObject* obj ;
  
  while ( (key = (TKey*)next()) ) {
    obj = key->ReadObj() ;
    TString hname = obj->GetName();
    TString objname= obj->ClassName();
    if(objname.Contains("Dir")) vlist.push_back(hname);
  }

  /// delete pointer                                                                                                                   
  delete _file;

  return vlist;
  
}

bool CheckInput(vector<TString> list, TString _var ,TString tag){
  if(std::find(list.begin(), list.end(), _var) != list.end()) cout << "Running with " << tag  << " :  "  <<_var << "" << endl;
  else {cout << "Error in input of " + tag+ ": " << _var<< "" << endl; for (auto i: list)   std::cout << i <<endl; return false; }

  return true;
}
bool CheckFileInput(TString fname){

  TFile * file_ = new TFile((fname).Data());

  if(CheckFile(file_) > 0)  {
    cout << "CheckInputFile::ERROR missing file " << fname << endl;
    /// delete pointer                                                                                                                   
    delete file_;

    return false;
  }
  /// delete pointer                                                                                                                   
  delete file_;

  return true;


}

double GetLumi(TString year){

  if  (year == "2016")  return 35600;
  if  (year == "2017")  return 41540;
  if  (year == "2018")  return 59960;
  
  return 35600+41540+59960;
}

double GetXsec(int mass){

    if(mass==100) return 7.77E-02;
  if(mass==125) return 2.45E-02;
  if(mass==150) return 1.10E-02;
  if(mass==200) return 3.40E-03;
  if(mass==250) return 1.43E-03;
  if(mass==300) return 7.13E-04;
  if(mass==400) return 2.37E-04;
  if(mass==500) return 9.86E-05;
  if(mass==600) return 4.72E-05;
  if(mass==700) return 2.47E-05;
  if(mass==800) return 1.38E-05;
  if(mass==900) return 8.11E-06;
  if(mass==1000) return 4.96E-06;
  if(mass==1100) return 3.12E-06;
  if(mass==1200) return 2.02E-06;
  if(mass==1300) return 1.33E-06;
  if(mass==1400) return 8.91E-07;
  if(mass==1500) return 6.05E-07;
  if(mass==1700) return 2.90E-07;
  if(mass==2000) return 1.02E-07;

  cout << "[GetXsec] Mass Wrong : " << mass << endl;
  return 0.;

}

TGraphAsymmErrors*  Get2016SoverSB(bool massbinned, TString year ,TString sr, TString channel, vector<double> masses){
   map<double, double>  tempvec_exo17028_ee_schannel_sr1_Eff;
  tempvec_exo17028_ee_schannel_sr1_Eff[100]= 1.1;
  tempvec_exo17028_ee_schannel_sr1_Eff[125]= 2.6;
  tempvec_exo17028_ee_schannel_sr1_Eff[150]= 3.1;
  tempvec_exo17028_ee_schannel_sr1_Eff[200]= 4.9;
  tempvec_exo17028_ee_schannel_sr1_Eff[250]= 5.9;
  tempvec_exo17028_ee_schannel_sr1_Eff[300]= 7.6;
  tempvec_exo17028_ee_schannel_sr1_Eff[400]= 6.6;
  tempvec_exo17028_ee_schannel_sr1_Eff[500]= 5.5;
  tempvec_exo17028_ee_schannel_sr1_Eff[600]= 3.8;
  tempvec_exo17028_ee_schannel_sr1_Eff[700]= 4.0;
  tempvec_exo17028_ee_schannel_sr1_Eff[800]= 3.6;
  tempvec_exo17028_ee_schannel_sr1_Eff[900]= 3.2 ;
  tempvec_exo17028_ee_schannel_sr1_Eff[1000]= 2.6;
  tempvec_exo17028_ee_schannel_sr1_Eff[1100]= 2.2;
  tempvec_exo17028_ee_schannel_sr1_Eff[1200]= 2.0;
  tempvec_exo17028_ee_schannel_sr1_Eff[1300]= 1.8;
  tempvec_exo17028_ee_schannel_sr1_Eff[1400]= 1.5;
  tempvec_exo17028_ee_schannel_sr1_Eff[1500]= 1.3;

   map<double, double>  tempvec_exo17028_ee_schannel_sr2_Eff;
  tempvec_exo17028_ee_schannel_sr2_Eff[100]= 0.005;
  tempvec_exo17028_ee_schannel_sr2_Eff[125]= 0.04;
  tempvec_exo17028_ee_schannel_sr2_Eff[150]= 0.19;
  tempvec_exo17028_ee_schannel_sr2_Eff[200]= 0.6;
  tempvec_exo17028_ee_schannel_sr2_Eff[250]= 2.2;
  tempvec_exo17028_ee_schannel_sr2_Eff[300]= 3.5;
  tempvec_exo17028_ee_schannel_sr2_Eff[400]= 9.1;
  tempvec_exo17028_ee_schannel_sr2_Eff[500]= 14.3;
  tempvec_exo17028_ee_schannel_sr2_Eff[600]= 17.4;
  tempvec_exo17028_ee_schannel_sr2_Eff[700]= 19.4;
  tempvec_exo17028_ee_schannel_sr2_Eff[800]= 20.8;
  tempvec_exo17028_ee_schannel_sr2_Eff[900]= 19.2;
  tempvec_exo17028_ee_schannel_sr2_Eff[1000]= 21.5;
  tempvec_exo17028_ee_schannel_sr2_Eff[1100]= 20.3;
  tempvec_exo17028_ee_schannel_sr2_Eff[1200]= 20.8;
  tempvec_exo17028_ee_schannel_sr2_Eff[1300]= 20.5;
  tempvec_exo17028_ee_schannel_sr2_Eff[1400]= 19.6;
  tempvec_exo17028_ee_schannel_sr2_Eff[1500]= 19.5;

  map<double, double>  tempvec_exo17028_mm_schannel_sr1_Eff;
  tempvec_exo17028_mm_schannel_sr1_Eff[100]= 2.6;
  tempvec_exo17028_mm_schannel_sr1_Eff[125]= 5.1;
  tempvec_exo17028_mm_schannel_sr1_Eff[150]= 6.6;
  tempvec_exo17028_mm_schannel_sr1_Eff[200]= 8.1;
  tempvec_exo17028_mm_schannel_sr1_Eff[250]= 11;
  tempvec_exo17028_mm_schannel_sr1_Eff[300]= 13.2;
  tempvec_exo17028_mm_schannel_sr1_Eff[400]= 11.7;
  tempvec_exo17028_mm_schannel_sr1_Eff[500]= 8.6;
  tempvec_exo17028_mm_schannel_sr1_Eff[600]= 7.4;
  tempvec_exo17028_mm_schannel_sr1_Eff[700]= 6.7;
  tempvec_exo17028_mm_schannel_sr1_Eff[800]= 6.0;
  tempvec_exo17028_mm_schannel_sr1_Eff[900]= 5.4;
  tempvec_exo17028_mm_schannel_sr1_Eff[1000]= 4.6;
  tempvec_exo17028_mm_schannel_sr1_Eff[1100]= 4.1;
  tempvec_exo17028_mm_schannel_sr1_Eff[1200]= 3.6;
  tempvec_exo17028_mm_schannel_sr1_Eff[1300]= 3.2;
  tempvec_exo17028_mm_schannel_sr1_Eff[1400]= 2.7;
  tempvec_exo17028_mm_schannel_sr1_Eff[1500]= 2.5;

  map<double, double>  tempvec_exo17028_mm_schannel_sr2_Eff;
  tempvec_exo17028_mm_schannel_sr2_Eff[100]= 0.006;
  tempvec_exo17028_mm_schannel_sr2_Eff[125]= 0.08;
  tempvec_exo17028_mm_schannel_sr2_Eff[150]= 0.28;
  tempvec_exo17028_mm_schannel_sr2_Eff[200]= 1.4;
  tempvec_exo17028_mm_schannel_sr2_Eff[250]= 3;
  tempvec_exo17028_mm_schannel_sr2_Eff[300]= 5.4;
  tempvec_exo17028_mm_schannel_sr2_Eff[400]= 13.3;
  tempvec_exo17028_mm_schannel_sr2_Eff[500]= 22.4;
  tempvec_exo17028_mm_schannel_sr2_Eff[600]= 30.2;
  tempvec_exo17028_mm_schannel_sr2_Eff[700]= 34.6;
  tempvec_exo17028_mm_schannel_sr2_Eff[800]= 34.8;
  tempvec_exo17028_mm_schannel_sr2_Eff[900]= 35.8;
  tempvec_exo17028_mm_schannel_sr2_Eff[1000]= 38.4;
  tempvec_exo17028_mm_schannel_sr2_Eff[1100]= 36.7;
  tempvec_exo17028_mm_schannel_sr2_Eff[1200]= 38.5;
  tempvec_exo17028_mm_schannel_sr2_Eff[1300]= 38.5;
  tempvec_exo17028_mm_schannel_sr2_Eff[1400]= 35.9;
  tempvec_exo17028_mm_schannel_sr2_Eff[1500]= 36.4;


  map<double, double>  tempvec_exo17028_ee_tchannel_sr1_Eff;
  tempvec_exo17028_ee_tchannel_sr1_Eff[300]= 3.0;
  tempvec_exo17028_ee_tchannel_sr1_Eff[400]= 3.0;
  tempvec_exo17028_ee_tchannel_sr1_Eff[500]= 2.7;
  tempvec_exo17028_ee_tchannel_sr1_Eff[600]= 1.7;
  tempvec_exo17028_ee_tchannel_sr1_Eff[700]= 2.8;
  tempvec_exo17028_ee_tchannel_sr1_Eff[800]= 3.0;
  tempvec_exo17028_ee_tchannel_sr1_Eff[900]= 2.9;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1000]= 2.4;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1100]= 2.0;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1200]= 1.8;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1300]= 1.6;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1400]= 1.3;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1500]= 1.2;

  map<double, double>  tempvec_exo17028_ee_tchannel_sr2_Eff;
  tempvec_exo17028_ee_tchannel_sr2_Eff[300]= 0.6;
  tempvec_exo17028_ee_tchannel_sr2_Eff[400]= 2.9;
  tempvec_exo17028_ee_tchannel_sr2_Eff[500]= 6.1;
  tempvec_exo17028_ee_tchannel_sr2_Eff[600]= 11;
  tempvec_exo17028_ee_tchannel_sr2_Eff[700]= 13.1;
  tempvec_exo17028_ee_tchannel_sr2_Eff[800]= 14;
  tempvec_exo17028_ee_tchannel_sr2_Eff[900]= 13.2;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1000]= 15.3;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1100]= 14.7;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1200]= 15.3;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1300]= 15.5;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1400]= 15.1;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1500]= 15.2;

  map<double, double>  tempvec_exo17028_mm_tchannel_sr1_Eff;
  tempvec_exo17028_mm_tchannel_sr1_Eff[300]= 5.2;
  tempvec_exo17028_mm_tchannel_sr1_Eff[400]= 5.1;
  tempvec_exo17028_mm_tchannel_sr1_Eff[500]= 4.1;
  tempvec_exo17028_mm_tchannel_sr1_Eff[600]= 4.1;
  tempvec_exo17028_mm_tchannel_sr1_Eff[700]= 3.9;
  tempvec_exo17028_mm_tchannel_sr1_Eff[800]= 5.4;
  tempvec_exo17028_mm_tchannel_sr1_Eff[900]= 5.0;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1000]= 4.2;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1100]= 3.8;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1200]= 3.4;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1300]= 3.0;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1400]= 2.7;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1500]= 2.3;

  map<double, double>  tempvec_exo17028_mm_tchannel_sr2_Eff;
  tempvec_exo17028_mm_tchannel_sr2_Eff[300]= 0.7;
  tempvec_exo17028_mm_tchannel_sr2_Eff[400]= 2.7;
  tempvec_exo17028_mm_tchannel_sr2_Eff[500]= 6.8;
  tempvec_exo17028_mm_tchannel_sr2_Eff[600]= 20.4;
  tempvec_exo17028_mm_tchannel_sr2_Eff[700]= 24.7;
  tempvec_exo17028_mm_tchannel_sr2_Eff[800]= 24.9;
  tempvec_exo17028_mm_tchannel_sr2_Eff[900]= 26.9;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1000]= 28.9;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1100]= 29.2;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1200]= 30.1;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1300]= 30.7;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1400]= 29.4;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1500]= 30.0;


  ///////
  map<double, double>  tempvec_exo17028_ee_schannel_sr1_Bkg;
  tempvec_exo17028_ee_schannel_sr1_Bkg[100]= 20.3;
  tempvec_exo17028_ee_schannel_sr1_Bkg[125]= 17.7;
  tempvec_exo17028_ee_schannel_sr1_Bkg[150]= 14.7;
  tempvec_exo17028_ee_schannel_sr1_Bkg[200]= 12.4;
  tempvec_exo17028_ee_schannel_sr1_Bkg[250]= 6.0;
  tempvec_exo17028_ee_schannel_sr1_Bkg[300]= 8.2;
  tempvec_exo17028_ee_schannel_sr1_Bkg[400]= 2.5;
  tempvec_exo17028_ee_schannel_sr1_Bkg[500]= 1.5;
  tempvec_exo17028_ee_schannel_sr1_Bkg[600]= 0.9;
  tempvec_exo17028_ee_schannel_sr1_Bkg[700]= 1.7;
  tempvec_exo17028_ee_schannel_sr1_Bkg[800]= 1.7;
  tempvec_exo17028_ee_schannel_sr1_Bkg[900]= 1.7;
  tempvec_exo17028_ee_schannel_sr1_Bkg[1000]=1.7;
  tempvec_exo17028_ee_schannel_sr1_Bkg[1100]=1.7;
  tempvec_exo17028_ee_schannel_sr1_Bkg[1200]=1.7;
  tempvec_exo17028_ee_schannel_sr1_Bkg[1300]=1.7;
  tempvec_exo17028_ee_schannel_sr1_Bkg[1400]=1.7;
  tempvec_exo17028_ee_schannel_sr1_Bkg[1500]=1.7;

  map<double, double>  tempvec_exo17028_ee_schannel_sr2_Bkg;
  tempvec_exo17028_ee_schannel_sr2_Bkg[100]= 3.4;
  tempvec_exo17028_ee_schannel_sr2_Bkg[125]= 0.2;
  tempvec_exo17028_ee_schannel_sr2_Bkg[150]= 1.3;
  tempvec_exo17028_ee_schannel_sr2_Bkg[200]= 0.8;
  tempvec_exo17028_ee_schannel_sr2_Bkg[250]= 2.1;
  tempvec_exo17028_ee_schannel_sr2_Bkg[300]= 1.3;
  tempvec_exo17028_ee_schannel_sr2_Bkg[400]= 3.1;
  tempvec_exo17028_ee_schannel_sr2_Bkg[500]= 2.8;
  tempvec_exo17028_ee_schannel_sr2_Bkg[600]= 0.8;
  tempvec_exo17028_ee_schannel_sr2_Bkg[700]= 0.8;
  tempvec_exo17028_ee_schannel_sr2_Bkg[800]= 0.9;
  tempvec_exo17028_ee_schannel_sr2_Bkg[900]= 0.2;
  tempvec_exo17028_ee_schannel_sr2_Bkg[1000]= 0.3;
  tempvec_exo17028_ee_schannel_sr2_Bkg[1100]=0.1;
  tempvec_exo17028_ee_schannel_sr2_Bkg[1200]=0.1;
  tempvec_exo17028_ee_schannel_sr2_Bkg[1300]=0.1;
  tempvec_exo17028_ee_schannel_sr2_Bkg[1400]=0.1;
  tempvec_exo17028_ee_schannel_sr2_Bkg[1500]=0.1;

  map<double, double>  tempvec_exo17028_mm_schannel_sr1_Bkg;
  tempvec_exo17028_mm_schannel_sr1_Bkg[100]= 18.6;
  tempvec_exo17028_mm_schannel_sr1_Bkg[125]= 11.7;
  tempvec_exo17028_mm_schannel_sr1_Bkg[150]= 8.9;
  tempvec_exo17028_mm_schannel_sr1_Bkg[200]= 4.6;
  tempvec_exo17028_mm_schannel_sr1_Bkg[250]= 3.0;
  tempvec_exo17028_mm_schannel_sr1_Bkg[300]= 2.6;
  tempvec_exo17028_mm_schannel_sr1_Bkg[400]= 0.9;
  tempvec_exo17028_mm_schannel_sr1_Bkg[500]= 0.4;
  tempvec_exo17028_mm_schannel_sr1_Bkg[600]= 0.3;
  tempvec_exo17028_mm_schannel_sr1_Bkg[700]= 0.2;
  tempvec_exo17028_mm_schannel_sr1_Bkg[800]= 0.2;
  tempvec_exo17028_mm_schannel_sr1_Bkg[900]= 0.2;
  tempvec_exo17028_mm_schannel_sr1_Bkg[1000]=0.2;
  tempvec_exo17028_mm_schannel_sr1_Bkg[1100]=0.2;
  tempvec_exo17028_mm_schannel_sr1_Bkg[1200]=0.2;
  tempvec_exo17028_mm_schannel_sr1_Bkg[1300]=0.2;
  tempvec_exo17028_mm_schannel_sr1_Bkg[1400]=0.2;
  tempvec_exo17028_mm_schannel_sr1_Bkg[1500]=0.2;

  map<double, double>  tempvec_exo17028_mm_schannel_sr2_Bkg;
  tempvec_exo17028_mm_schannel_sr2_Bkg[100]= 1.0;
  tempvec_exo17028_mm_schannel_sr2_Bkg[125]= 0.8;
  tempvec_exo17028_mm_schannel_sr2_Bkg[150]= 1.0;
  tempvec_exo17028_mm_schannel_sr2_Bkg[200]= 1.3;
  tempvec_exo17028_mm_schannel_sr2_Bkg[250]= 0.3;
  tempvec_exo17028_mm_schannel_sr2_Bkg[300]= 0.4;
  tempvec_exo17028_mm_schannel_sr2_Bkg[400]= 0.5;
  tempvec_exo17028_mm_schannel_sr2_Bkg[500]= 0.8;
  tempvec_exo17028_mm_schannel_sr2_Bkg[600]= 0.7;
  tempvec_exo17028_mm_schannel_sr2_Bkg[700]= 0.8;
  tempvec_exo17028_mm_schannel_sr2_Bkg[800]= 0.4;
  tempvec_exo17028_mm_schannel_sr2_Bkg[900]= 0.2;
  tempvec_exo17028_mm_schannel_sr2_Bkg[1000]=0.1;
  tempvec_exo17028_mm_schannel_sr2_Bkg[1100]=0.1;
  tempvec_exo17028_mm_schannel_sr2_Bkg[1200]=0.2;
  tempvec_exo17028_mm_schannel_sr2_Bkg[1300]=0.3;
  tempvec_exo17028_mm_schannel_sr2_Bkg[1400]=0.1;
  tempvec_exo17028_mm_schannel_sr2_Bkg[1500]=0.1;



  
  map<double, double> tmpM ;
  if(sr == "SR1" && channel == "MuMu") tmpM = tempvec_exo17028_mm_schannel_sr1_Eff;
  if(sr == "SR2" && channel == "MuMu") tmpM = tempvec_exo17028_mm_schannel_sr2_Eff;
  if(sr == "SR1" && channel == "EE") tmpM = tempvec_exo17028_ee_schannel_sr1_Eff;
  if(sr == "SR2" && channel == "EE") tmpM = tempvec_exo17028_ee_schannel_sr2_Eff;

  map<double, double> tmpMBkg ;
  if(sr == "SR1" && channel == "MuMu") tmpMBkg = tempvec_exo17028_mm_schannel_sr1_Bkg;
  if(sr == "SR2" && channel == "MuMu") tmpMBkg = tempvec_exo17028_mm_schannel_sr2_Bkg;
  if(sr == "SR1" && channel == "EE") tmpMBkg = tempvec_exo17028_ee_schannel_sr1_Bkg;
  if(sr == "SR2" && channel == "EE") tmpMBkg = tempvec_exo17028_ee_schannel_sr2_Bkg;

  int Nbins = masses.size();
  double x[Nbins], y[Nbins], xlow[Nbins], xup[Nbins], ylow[Nbins], yup[Nbins];

  for(Int_t i=0; i<Nbins; i++){

    map<double, double>::iterator its = tmpM.find(masses[i]);
    map<double, double>::iterator itb = tmpMBkg.find(masses[i]);
    x[i] = masses[i];
    if(massbinned){
      y[i] = 0.01*its->second / sqrt(itb->second);
      cout << "EXO 17-028 " << masses[i] << " " << its->second  << " " << itb->second <<  " s/sb = " << 0.01*its->second / sqrt(itb->second) <<endl;
    }
    else{
      float bkg = 502.;
      if(sr == "SR1" && channel == "MuMu")bkg = 502.;
      if(sr == "SR1" && channel == "EE")bkg = 382.;
      if(sr == "SR2" && channel == "MuMu")bkg = 13.;
      if(sr == "SR2" && channel == "EE")bkg = 10.;

      y[i] = 0.;//0.01*its->second / sqrt(itb->second);
      cout << "EXO 17-028 " << masses[i] << " " << its->second  << " " << itb->second <<  " s/sb = " << 0.01*its->second / sqrt(bkg) <<endl;
      
    }
    xlow[i]=  0.;
    xup[i] = 0.;
    ylow[i] = 0.;
    yup[i] = 0.;
  }


  TGraphAsymmErrors *out = new TGraphAsymmErrors(Nbins, x, y, xlow, xup, ylow, yup);
  //out->SetLineWidth(2.0);                                                                                                                                                                                                                   
  //out->SetMarkerSize(0.);                                                                                                                                                                                                                   
  out->GetHistogram()->GetXaxis()->SetTitle("m_{N} (GeV)");
  out->GetHistogram()->GetYaxis()->SetTitle("#epsilon/sqrt(B)");
  out->SetTitle("");
  return out;


}

TGraphAsymmErrors* Get2016SigEff(TString sr, TString channel, vector<double> masses,TString range){

  map<double, double>  tempvec_exo17028_ee_schannel_sr1_Eff;
  tempvec_exo17028_ee_schannel_sr1_Eff[100]= 1.1;
  tempvec_exo17028_ee_schannel_sr1_Eff[125]= 2.6;
  tempvec_exo17028_ee_schannel_sr1_Eff[150]= 3.1;
  tempvec_exo17028_ee_schannel_sr1_Eff[200]= 4.9;
  tempvec_exo17028_ee_schannel_sr1_Eff[250]= 5.9;
  tempvec_exo17028_ee_schannel_sr1_Eff[300]= 7.6;
  tempvec_exo17028_ee_schannel_sr1_Eff[400]= 6.6;
  tempvec_exo17028_ee_schannel_sr1_Eff[500]= 5.5;
  tempvec_exo17028_ee_schannel_sr1_Eff[600]= 3.8;
  tempvec_exo17028_ee_schannel_sr1_Eff[700]= 4.0;
  tempvec_exo17028_ee_schannel_sr1_Eff[800]= 3.6;
  tempvec_exo17028_ee_schannel_sr1_Eff[900]= 3.2 ;
  tempvec_exo17028_ee_schannel_sr1_Eff[1000]= 2.6;
  tempvec_exo17028_ee_schannel_sr1_Eff[1100]= 2.2;
  tempvec_exo17028_ee_schannel_sr1_Eff[1200]= 2.0;
  tempvec_exo17028_ee_schannel_sr1_Eff[1300]= 1.8;
  tempvec_exo17028_ee_schannel_sr1_Eff[1400]= 1.5;
  tempvec_exo17028_ee_schannel_sr1_Eff[1500]= 1.3;

  map<double, double>  tempvec_exo17028_ee_schannel_sr2_Eff;
  tempvec_exo17028_ee_schannel_sr2_Eff[100]= 0.005;
  tempvec_exo17028_ee_schannel_sr2_Eff[125]= 0.04;
  tempvec_exo17028_ee_schannel_sr2_Eff[150]= 0.19;
  tempvec_exo17028_ee_schannel_sr2_Eff[200]= 0.6;
  tempvec_exo17028_ee_schannel_sr2_Eff[250]= 2.2;
  tempvec_exo17028_ee_schannel_sr2_Eff[300]= 3.5;
  tempvec_exo17028_ee_schannel_sr2_Eff[400]= 9.1;
  tempvec_exo17028_ee_schannel_sr2_Eff[500]= 14.3;
  tempvec_exo17028_ee_schannel_sr2_Eff[600]= 17.4;
  tempvec_exo17028_ee_schannel_sr2_Eff[700]= 19.4;
  tempvec_exo17028_ee_schannel_sr2_Eff[800]= 20.8;
  tempvec_exo17028_ee_schannel_sr2_Eff[900]= 19.2;
  tempvec_exo17028_ee_schannel_sr2_Eff[1000]= 21.5;
  tempvec_exo17028_ee_schannel_sr2_Eff[1100]= 20.3;
  tempvec_exo17028_ee_schannel_sr2_Eff[1200]= 20.8;
  tempvec_exo17028_ee_schannel_sr2_Eff[1300]= 20.5;
  tempvec_exo17028_ee_schannel_sr2_Eff[1400]= 19.6;
  tempvec_exo17028_ee_schannel_sr2_Eff[1500]= 19.5;

  map<double, double>  tempvec_exo17028_mm_schannel_sr1_Eff;
  tempvec_exo17028_mm_schannel_sr1_Eff[100]= 2.6;
  tempvec_exo17028_mm_schannel_sr1_Eff[125]= 5.1;
  tempvec_exo17028_mm_schannel_sr1_Eff[150]= 6.6;
  tempvec_exo17028_mm_schannel_sr1_Eff[200]= 8.1;
  tempvec_exo17028_mm_schannel_sr1_Eff[250]= 11;
  tempvec_exo17028_mm_schannel_sr1_Eff[300]= 13.2;
  tempvec_exo17028_mm_schannel_sr1_Eff[400]= 11.7;
  tempvec_exo17028_mm_schannel_sr1_Eff[500]= 8.6;
  tempvec_exo17028_mm_schannel_sr1_Eff[600]= 7.4;
  tempvec_exo17028_mm_schannel_sr1_Eff[700]= 6.7;
  tempvec_exo17028_mm_schannel_sr1_Eff[800]= 6.0;
  tempvec_exo17028_mm_schannel_sr1_Eff[900]= 5.4;
  tempvec_exo17028_mm_schannel_sr1_Eff[1000]= 4.6;
  tempvec_exo17028_mm_schannel_sr1_Eff[1100]= 4.1;
  tempvec_exo17028_mm_schannel_sr1_Eff[1200]= 3.6;
  tempvec_exo17028_mm_schannel_sr1_Eff[1300]= 3.2;
  tempvec_exo17028_mm_schannel_sr1_Eff[1400]= 2.7;
  tempvec_exo17028_mm_schannel_sr1_Eff[1500]= 2.5;

  map<double, double>  tempvec_exo17028_mm_schannel_sr2_Eff;
  tempvec_exo17028_mm_schannel_sr2_Eff[100]= 0.006;
  tempvec_exo17028_mm_schannel_sr2_Eff[125]= 0.08;
  tempvec_exo17028_mm_schannel_sr2_Eff[150]= 0.28;
  tempvec_exo17028_mm_schannel_sr2_Eff[200]= 1.4;
  tempvec_exo17028_mm_schannel_sr2_Eff[250]= 3;
  tempvec_exo17028_mm_schannel_sr2_Eff[300]= 5.4;
  tempvec_exo17028_mm_schannel_sr2_Eff[400]= 13.3;
  tempvec_exo17028_mm_schannel_sr2_Eff[500]= 22.4;
  tempvec_exo17028_mm_schannel_sr2_Eff[600]= 30.2;
  tempvec_exo17028_mm_schannel_sr2_Eff[700]= 34.6;
  tempvec_exo17028_mm_schannel_sr2_Eff[800]= 34.8;
  tempvec_exo17028_mm_schannel_sr2_Eff[900]= 35.8;
  tempvec_exo17028_mm_schannel_sr2_Eff[1000]= 38.4;
  tempvec_exo17028_mm_schannel_sr2_Eff[1100]= 36.7;
  tempvec_exo17028_mm_schannel_sr2_Eff[1200]= 38.5;
  tempvec_exo17028_mm_schannel_sr2_Eff[1300]= 38.5;
  tempvec_exo17028_mm_schannel_sr2_Eff[1400]= 35.9;
  tempvec_exo17028_mm_schannel_sr2_Eff[1500]= 36.4;


  map<double, double>  tempvec_exo17028_ee_tchannel_sr1_Eff;
  tempvec_exo17028_ee_tchannel_sr1_Eff[300]= 3.0;
  tempvec_exo17028_ee_tchannel_sr1_Eff[400]= 3.0;
  tempvec_exo17028_ee_tchannel_sr1_Eff[500]= 2.7;
  tempvec_exo17028_ee_tchannel_sr1_Eff[600]= 1.7;
  tempvec_exo17028_ee_tchannel_sr1_Eff[700]= 2.8;
  tempvec_exo17028_ee_tchannel_sr1_Eff[800]= 3.0;
  tempvec_exo17028_ee_tchannel_sr1_Eff[900]= 2.9;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1000]= 2.4;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1100]= 2.0;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1200]= 1.8;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1300]= 1.6;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1400]= 1.3;
  tempvec_exo17028_ee_tchannel_sr1_Eff[1500]= 1.2;

  map<double, double>  tempvec_exo17028_ee_tchannel_sr2_Eff;
  tempvec_exo17028_ee_tchannel_sr2_Eff[300]= 0.6;
  tempvec_exo17028_ee_tchannel_sr2_Eff[400]= 2.9;
  tempvec_exo17028_ee_tchannel_sr2_Eff[500]= 6.1;
  tempvec_exo17028_ee_tchannel_sr2_Eff[600]= 11;
  tempvec_exo17028_ee_tchannel_sr2_Eff[700]= 13.1;
  tempvec_exo17028_ee_tchannel_sr2_Eff[800]= 14;
  tempvec_exo17028_ee_tchannel_sr2_Eff[900]= 13.2;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1000]= 15.3;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1100]= 14.7;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1200]= 15.3;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1300]= 15.5;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1400]= 15.1;
  tempvec_exo17028_ee_tchannel_sr2_Eff[1500]= 15.2;

  map<double, double>  tempvec_exo17028_mm_tchannel_sr1_Eff;
  tempvec_exo17028_mm_tchannel_sr1_Eff[300]= 5.2;
  tempvec_exo17028_mm_tchannel_sr1_Eff[400]= 5.1;
  tempvec_exo17028_mm_tchannel_sr1_Eff[500]= 4.1;
  tempvec_exo17028_mm_tchannel_sr1_Eff[600]= 4.1;
  tempvec_exo17028_mm_tchannel_sr1_Eff[700]= 3.9;
  tempvec_exo17028_mm_tchannel_sr1_Eff[800]= 5.4;
  tempvec_exo17028_mm_tchannel_sr1_Eff[900]= 5.0;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1000]= 4.2;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1100]= 3.8;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1200]= 3.4;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1300]= 3.0;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1400]= 2.7;
  tempvec_exo17028_mm_tchannel_sr1_Eff[1500]= 2.3;

  map<double, double>  tempvec_exo17028_mm_tchannel_sr2_Eff;
  tempvec_exo17028_mm_tchannel_sr2_Eff[300]= 0.7;
  tempvec_exo17028_mm_tchannel_sr2_Eff[400]= 2.7;
  tempvec_exo17028_mm_tchannel_sr2_Eff[500]= 6.8;
  tempvec_exo17028_mm_tchannel_sr2_Eff[600]= 20.4;
  tempvec_exo17028_mm_tchannel_sr2_Eff[700]= 24.7;
  tempvec_exo17028_mm_tchannel_sr2_Eff[800]= 24.9;
  tempvec_exo17028_mm_tchannel_sr2_Eff[900]= 26.9;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1000]= 28.9;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1100]= 29.2;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1200]= 30.1;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1300]= 30.7;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1400]= 29.4;
  tempvec_exo17028_mm_tchannel_sr2_Eff[1500]= 30.0;

  map<double, double> tmpM ;
  if(sr == "SR1" && channel == "MuMu") tmpM = tempvec_exo17028_mm_schannel_sr1_Eff;
  if(sr == "SR2" && channel == "MuMu") tmpM = tempvec_exo17028_mm_schannel_sr2_Eff;
  if(sr == "SR1" && channel == "EE") tmpM = tempvec_exo17028_ee_schannel_sr1_Eff;
  if(sr == "SR2" && channel == "EE") tmpM = tempvec_exo17028_ee_schannel_sr2_Eff;

  int Nbins = masses.size();
  double x[Nbins], y[Nbins], xlow[Nbins], xup[Nbins], ylow[Nbins], yup[Nbins];

  for(Int_t i=0; i<Nbins; i++){

    map<double, double>::iterator it = tmpM.find(masses[i]);
    if(range=="low"){
      x[i] = masses[i];
      y[i] = it->second/100.;
    }
    else{
      if (i > Nbins -3) {
        x[i] =  masses[i];
        y[i] = y[Nbins -3];
      }else{
        x[i] = masses[i];
        y[i] = it->second/100.;
      }
    }

    xlow[i]=  0.;
    xup[i] = 0.;
    ylow[i] = 0.;
    yup[i] = 0.;
  }


  TGraphAsymmErrors *out = new TGraphAsymmErrors(Nbins, x, y, xlow, xup, ylow, yup);
  //out->SetLineWidth(2.0);                                                                                                                                                                                                                                                                                                                                                                                                                                                         
  out->GetHistogram()->GetXaxis()->SetTitle("m_{N} (GeV)");
  out->GetHistogram()->GetYaxis()->SetTitle("#epsilon");
  out->SetTitle("");
  return out;


}



TGraphAsymmErrors* Get2016SigEffHighMass(TString sr, TString channel, vector<double> masses, TString range){

  map<double, double>  tempvec_exo17028_ee_schannel_sr1_Eff;
  tempvec_exo17028_ee_schannel_sr1_Eff[100]= 3.6;
  tempvec_exo17028_ee_schannel_sr1_Eff[125]= 10;
  tempvec_exo17028_ee_schannel_sr1_Eff[150]= 14.;
  tempvec_exo17028_ee_schannel_sr1_Eff[200]= 19.;
  tempvec_exo17028_ee_schannel_sr1_Eff[250]= 22.;
  tempvec_exo17028_ee_schannel_sr1_Eff[300]= 24.;
  tempvec_exo17028_ee_schannel_sr1_Eff[400]= 27.;
  tempvec_exo17028_ee_schannel_sr1_Eff[500]= 28.;
  tempvec_exo17028_ee_schannel_sr1_Eff[600]= 30;
  tempvec_exo17028_ee_schannel_sr1_Eff[700]= 30.;
  tempvec_exo17028_ee_schannel_sr1_Eff[800]= 30.;
  tempvec_exo17028_ee_schannel_sr1_Eff[900]= 29.;
  tempvec_exo17028_ee_schannel_sr1_Eff[1000]= 29.;
  tempvec_exo17028_ee_schannel_sr1_Eff[1100]=28.;
  tempvec_exo17028_ee_schannel_sr1_Eff[1200]=28.;
  tempvec_exo17028_ee_schannel_sr1_Eff[1300]=27.;
  tempvec_exo17028_ee_schannel_sr1_Eff[1400]=26.;
  tempvec_exo17028_ee_schannel_sr1_Eff[1500]=26.;
  
  map<double, double>  tempvec_exo17028_mm_schannel_sr1_Eff;
  tempvec_exo17028_mm_schannel_sr1_Eff[100]= 7.6;
  tempvec_exo17028_mm_schannel_sr1_Eff[125]= 17.;
  tempvec_exo17028_mm_schannel_sr1_Eff[150]= 23.;
  tempvec_exo17028_mm_schannel_sr1_Eff[200]= 30.;
  tempvec_exo17028_mm_schannel_sr1_Eff[250]= 35.;
  tempvec_exo17028_mm_schannel_sr1_Eff[300]= 40.;
  tempvec_exo17028_mm_schannel_sr1_Eff[400]= 45.;
  tempvec_exo17028_mm_schannel_sr1_Eff[500]= 48.;
  tempvec_exo17028_mm_schannel_sr1_Eff[600]= 51.;
  tempvec_exo17028_mm_schannel_sr1_Eff[700]= 51.;
  tempvec_exo17028_mm_schannel_sr1_Eff[800]= 52.;
  tempvec_exo17028_mm_schannel_sr1_Eff[900]= 52.;
  tempvec_exo17028_mm_schannel_sr1_Eff[1000]=52.;
  tempvec_exo17028_mm_schannel_sr1_Eff[1100]=51.;
  tempvec_exo17028_mm_schannel_sr1_Eff[1200]=51.;
  tempvec_exo17028_mm_schannel_sr1_Eff[1300]=51.;
  tempvec_exo17028_mm_schannel_sr1_Eff[1400]=50.;
  tempvec_exo17028_mm_schannel_sr1_Eff[1500]=49;


  map<double, double> tmpM ;
  if(sr == "SR1" && channel == "MuMu") tmpM = tempvec_exo17028_mm_schannel_sr1_Eff;
  else if(sr == "SR2" && channel == "MuMu") tmpM = tempvec_exo17028_mm_schannel_sr1_Eff;
  else if(sr == "SR1" && channel == "EE") tmpM = tempvec_exo17028_ee_schannel_sr1_Eff;
  else if(sr == "SR2" && channel == "EE") tmpM = tempvec_exo17028_ee_schannel_sr1_Eff;
  else if (channel == "MuMu") tmpM = tempvec_exo17028_mm_schannel_sr1_Eff;
  else if (channel == "EE") tmpM = tempvec_exo17028_ee_schannel_sr1_Eff;

  // -2 is becuase 1700,2000 were not in EXO-17-028
  int Nbins = masses.size();
  double x[Nbins], y[Nbins], xlow[Nbins], xup[Nbins], ylow[Nbins], yup[Nbins];

  for(Int_t i=0; i<Nbins; i++){

    map<double, double>::iterator it = tmpM.find(masses[i]);
    if(range=="low"){
      x[i] = masses[i];
      y[i] = it->second/100.;
    }
    else{
      if (i > Nbins -3) {
	x[i] =  masses[i];
	y[i] = y[Nbins -3];
      }else{
	x[i] = masses[i];
	y[i] = it->second/100.;
      }
    }
    xlow[i]=  0.;
    xup[i] = 0.;
    ylow[i] = 0.;
    yup[i] = 0.;
  }
  
  
  TGraphAsymmErrors *out = new TGraphAsymmErrors(Nbins, x, y, xlow, xup, ylow, yup);
  //out->SetLineWidth(2.0);
  //out->SetMarkerSize(0.);
  out->GetHistogram()->GetXaxis()->SetTitle("m_{N} (GeV)");
  out->GetHistogram()->GetYaxis()->SetTitle("#epsilon");
  out->SetTitle("");
  return out;
  
  
}

void FormatHist(TH1* h , TString fill, Color_t t){


  if(fill.Contains("f")){
    h->SetFillColor(t);
    h->SetLineColor(t);
    h->SetLineWidth(10);    
  }
  if(fill.Contains("l")){

    h->SetLineWidth(10);

    h->SetLineColor(t);
  }
  if(fill.Contains("p")){

  h->SetMarkerColor(t);
  h->SetMarkerStyle(20);
  h->SetMarkerSize(1.6);
  h->SetLineWidth(10);
  }
  
}

void DrawLatex(TString year){

  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);

  latex_CMSPriliminary.DrawLatex(0.2, 0.96, "#font[62]{CMS Preliminery}");

  latex_Lumi.SetTextSize(0.035);
  latex_Lumi.SetTextFont(42);
  if(year=="2016a")latex_Lumi.DrawLatex(0.72, 0.96, "19.5 fb^{-1} (13 TeV)");
  else if(year=="2016b")latex_Lumi.DrawLatex(0.72, 0.96, "16.8 fb^{-1} (13 TeV)");
  else if(year=="2017")latex_Lumi.DrawLatex(0.72, 0.96, "41.5 fb^{-1} (13 TeV)");
  else if(year=="2018")latex_Lumi.DrawLatex(0.72, 0.96, "59.8 fb^{-1} (13 TeV)");
  else latex_Lumi.DrawLatex(0.72, 0.96, "137.3 fb^{-1} (13 TeV)");

}

void DrawLatexLabel(TString label, float x, float y){

  TLatex channelname;
  channelname.SetNDC();
  channelname.SetTextSize(0.03);
  channelname.DrawLatex(x, y,label);


}
void DrawLatexWithLabel(TString year,TString label, double size, float x, float y){

  DrawLatex(year);
  
  TLatex channelname;
  channelname.SetNDC();
  channelname.SetTextSize(size);
  channelname.DrawLatex(x, y,label);

  
}
bool FileHasDir(TFile* file, TString name){

    TList* list = file->GetListOfKeys() ;
    TIter next(list) ;
    TKey* key ;
    TObject* obj ;
    
    bool dir_found=false;
    while ( (key = (TKey*)next()) ) {
      obj = key->ReadObj() ;
      TString hname = obj->GetName();
      TString objname= obj->ClassName();
      if(hname == name && objname.Contains("Dir")) dir_found=true;
    }
    if(dir_found) return true;
    //scout << "File " << file->GetName() << " missing directory " << name << endl;

    TIter next2(list) ;
    TKey* key2 ;
    TObject* obj2 ;
    
    while ( (key2 = (TKey*)next2()) ) {
        obj2 = key2->ReadObj() ;
        TString hname2 = obj2->GetName();
        TString objname2= obj2->ClassName();
        //cout << "Possible directory names are " << hname2 << endl;

      }

    return false;
}

void PrintHistList(TList* list){

  TIter next(list) ;
  TKey* key ;
  TObject* obj ;
  while ( (key = (TKey*)next()) ) {
    obj = key->ReadObj() ;
    TString hname = obj->GetName();
    TString objname= obj->ClassName();
    if(!objname.Contains("Dir"))    cout << hname << " " << objname << endl;
  }
}
void PrintHists(TFile* file){
  
  PrintOut("PrintHists");
  TKey* key ;
  TObject* obj ;
  TList* list = file->GetListOfKeys() ;
  TIter next(list) ;  
  vector<TString> DirList;
  while ( (key = (TKey*)next()) ) {
    obj = key->ReadObj() ;
    TString hname = obj->GetName();
    TString objname= obj->ClassName();
    if(!objname.Contains("Dir"))    cout << hname << " " << objname << endl;
    else {
      DirList.push_back(hname);
      TList* list2 = file->GetDirectory(hname)->GetListOfKeys() ;
      PrintHistList(list2);
    }
  }
  for(auto i : DirList) cout << "Directory : " << i << endl;
  
  return;
}


bool CheckHist(TH1* h ){
  bool hist_exist = true;
  if(!h){
    cout << "No histogram with name " << h->GetName() << endl;
    hist_exist= false;
  }
  return hist_exist;
}


bool CheckHist(TH2* h ){
  bool hist_exist = true;
  if(!h){
    cout << "No histogram with name " << h->GetName() << endl;
    hist_exist= false;
  }
  return hist_exist;
}


bool CheckHist(TFile* file, TString name ){
  
  TString name_fix = name;
  name_fix = name_fix.ReplaceAll("/"," ");
  TH1* h;
  vector<string> v{_getsplit(string(name_fix), ' ')};
  TList* list ;
  if(v.size() == 2){
    if(FileHasDir(file, TString(v[0])))list = file->GetDirectory(TString(v[0]))->GetListOfKeys() ;
    //name= v[1];                                                                                                                 
    else return false;
  }
  else{
    list = file->GetListOfKeys() ;
  }
  
  
  TIter next(list) ;
  TKey* key ;
  TObject* obj ;

  bool hist_found=false;
  while ( (key = (TKey*)next()) ) {
    obj = key->ReadObj() ;
    TString hname = obj->GetName();
    TString objname= obj->ClassName();
    if(hname == name) hist_found=true;
  }
  if(!hist_found) return false;
  return true;
}


void NormHist(TH1* hist){

  hist->Scale(1./ hist->Integral());

}


TH1D* GetHist(TFile* file, TString name , TString ref="", bool return_void=false, bool debug=false){
  
  /// This function checks the hist exists in a given file and returns the TH1D pointer

  PreLogMessage();
  cout << "GetHist " << name << " from " << file->GetName() << endl;

  TString name_fix = name;
  name_fix = name_fix.ReplaceAll("/"," ");

  vector<string> v{_getsplit(string(name_fix), ' ')};
  TList* list ;
  if(v.size() == 2){
    if(FileHasDir(file, TString(v[0])))list = file->GetDirectory(TString(v[0]))->GetListOfKeys() ;
    else {
      TH1D* this_hist = new TH1D(name+"__"+file->GetName()+"__"+ref, name+"__"+file->GetName()+"__"+ref,1,0.,1);
      return this_hist;

    }
  }
  else{
    list = file->GetListOfKeys() ;
  }

  TIter next(list) ;
  TKey* key ;
  TObject* obj ;

  bool hist_found=false;
  while ( (key = (TKey*)next()) ) {
    obj = key->ReadObj() ;
    TString hname = obj->GetName();
    TString objname= obj->ClassName();
    if(hname == name) hist_found=true;
  }
  
  
  if(!hist_found){

    TH1D* h = (TH1D*)(file->Get(name));
    if(h) return h;
    else if(return_void){
      cout << "File " << file->GetName() << " missing file " << name << endl;
      cout << "Returning Empty hist" << endl;
      TH1D* this_hist = new TH1D(name+"__"+file->GetName()+"__"+ref, name+"__"+file->GetName()+"__"+ref,1,0.,1);
      return this_hist;
    }
    else {
      PrintHists(file);
      PreLogMessage();
      cout << "File " << file->GetName() << " missing file " << name << endl;
      PreLogMessage();
      cout << "================= Exiting ================" << endl;
      gApplication->Terminate();
     
    }
  }
  
  TH1D* h = (TH1D*)(file->Get(name));
  return h;
  
  
}

void PrintBins(TString histname, TH1* h){

  cout << "############################################"<<endl;
  cout << "Print out for " << histname << endl;
  cout << "############################################"<<endl;

  TAxis *axis = h->GetXaxis();

  cout << "Number of bins = " <<  axis->GetNbins() << endl;
  double _sumerr(0.);
  for(unsigned int i=0; i < axis->GetNbins()+1 ; i++){

    Double_t xmin = axis->GetBinLowEdge(i);
    Double_t xmax = axis->GetBinLowEdge(i+1);
    if(h->GetBinContent(i) != 0.)cout << "Bin " << i << "  boundary " << xmin << " - " <<  xmax << " value = " << h->GetBinContent(i) << " error = " << h->GetBinError(i) <<  " error % = " << 100*h->GetBinError(i)/h->GetBinContent(i) << endl;
    if(h->GetBinContent(i) > 0.)_sumerr+= h->GetBinError(i)/h->GetBinContent(i);
  }
  cout << "############################################"<<endl;
  cout << "Sum error ("<< histname <<") = " << _sumerr/axis->GetNbins() << endl;
  cout << " " << endl;

}

TString GetCutDir(TString var){

  if (var.Contains("mva")) return "LR";
  
  return "RL";
}

vector<double> GetLowStatRebin(TH1* hist, bool rebinsmart){

  double threshold_bin_stat = 0.2;
  
  TAxis *axis = hist->GetXaxis();
  
  vector<double> new_xbins;

  double t_err=0.;
  double t_val=0.;
  for(Int_t i=1; i< axis->GetNbins()+1; i++){

    Double_t xmin = axis->GetBinLowEdge(i);

    if(!rebinsmart) {      new_xbins.push_back(xmin); continue;}

    if(i==1) {
      new_xbins.push_back(xmin);

      continue;
    }
    if(i==axis->GetNbins())       {
      Double_t xmax = axis->GetBinLowEdge(i+1);
      new_xbins.push_back(xmax);

      continue;
    }

    double err = hist->GetBinError(i);
    double val = hist->GetBinContent(i);
    
    if(val <= 0.) continue;

    t_err = sqrt(t_err*t_err+err*err);
    t_val = t_val+val;
    double err_val = t_err/t_val;
    if(err_val > threshold_bin_stat) continue;
    t_err=0;
    t_val=0;
    new_xbins.push_back(xmin);
  }
  
  return new_xbins;
}

TH1D* RebinStat(TH1D* hist, TString tag){

  double threshold_bin_stat = 0.1;

  TAxis *axis = hist->GetXaxis();

  Double_t new_xbins[1000];
  int k=0;
  for(Int_t i=0; i< axis->GetNbins(); i++){

    Double_t xmin = axis->GetBinLowEdge(i);

    if(i==0) {
      new_xbins[k] = xmin;
      k++;
      continue;
    }
    double err = hist->GetBinError(i);
    double val = hist->GetBinContent(i);

    
    if(err/val > threshold_bin_stat) continue;

    new_xbins[k] = xmin;
    k++;
  }

  
  TH1D *hnew = (TH1D*)hist->Rebin(k,"hnew"+tag,new_xbins);
  return hnew;
  
}


TH1D* GetHistFull(vector<double> newrbins,  TFile* file, TString name, Color_t _linecol=kBlack, Width_t _width = 999, Style_t _style = 999, Color_t _fillcol = kBlack,  bool return_void=true, bool debug=false){

  TH1D* hist_tmp =  GetHist(file, name, return_void, debug);

  Double_t array_newrbins[newrbins.size()];
  for(unsigned int i=0; i < newrbins.size() ; i++) array_newrbins[i] = newrbins[i];

  TH1D* hist;

  bool rebin=true;
  if(newrbins.size() < 3) rebin = false;
  
  if(rebin) hist = (TH1D*)hist_tmp->Rebin(newrbins.size()-1, name+"rebin", array_newrbins);
  else hist = hist_tmp;

  if(_linecol != kBlack) hist->SetLineColor(_linecol);
  if(_fillcol != kBlack) hist->SetFillColor(_fillcol);
  if(_width != 999) hist->SetLineWidth(_width);
  if(_style != 999) hist->SetLineStyle(_style);

  return hist;
}

double GetSignalIntegral( double mass, TString year, TString sigpath,  TString n_sr_hist, TString hist_all){
  
  TFile * filemm = new TFile((sigpath).Data());
  if(CheckFile(filemm) > 0)  return 0.;
  TH1*  hpass = GetHist(filemm, n_sr_hist);
  TH1*  hall = GetHist(filemm, hist_all);

  return ( hpass->Integral()/hall->Integral()) * GetLumi(year)*GetXsec(mass) ;

}
double GetIntegral( TFile * filemm,  TString n_sr_hist){

  
  if(CheckFile(filemm) > 0)  return 0.;
  if(!CheckHist(filemm, n_sr_hist)) return 0.;
  TH1*  hpass = GetHist(filemm, n_sr_hist,true);
  double tot = hpass->Integral();
  
  return tot;

}

double SignalScale(double yield){
  if(yield==0.) return 0.;
  return 5./yield; 
}

double SignalScale(TString year, double  mass){

  float scale = 1.;
  if(mass <= 200) scale = 0.1;
  else if(mass <= 600) scale = 1.;
  else if(mass <= 1000) scale = 10.;
  else scale = 100.;
  
  if (year == "2017") scale *= 41.54/36.47;
  if (year == "2018") scale *= 59.96/36.47;

  return scale;

  
}


double GetMaximum(TGraphAsymmErrors* g1, vector<TGraphAsymmErrors*> grs){

  double _max= g1->GetHistogram()->GetMaximum();
  for(unsigned int ig=0; ig < grs.size(); ig++){
    if (grs[ig]->GetHistogram()->GetMaximum() > _max) _max = grs[ig]->GetHistogram()->GetMaximum();
  }
  return _max;
}

TGraphAsymmErrors* v_to_graph(int Nbins,  vector<double> vx,  vector<double> vxup, vector<double> vxdown, vector<double> vy,  vector<double> vyup,  vector<double> vydown, Color_t col){

  TH1::SetDefaultSumw2(true);


  double x[Nbins], y[Nbins], xlow[Nbins], xup[Nbins], ylow[Nbins], yup[Nbins];

  for(Int_t i=0; i<Nbins; i++){
    x[i] =vx[i];
    y[i] = vy[i];
    xlow[i] = vxdown[i];
    xup[i] = vxup[i];
    ylow[i] = vydown[i];
    yup[i] = vyup[i];

  }
  TGraphAsymmErrors *out = new TGraphAsymmErrors(Nbins, x, y, xlow, xup, ylow, yup);
  out->SetLineWidth(2.0);
  //out->SetMarkerSize(0.);
  out->SetLineColor(col);
  out->SetMarkerColor(col);

  out->SetTitle("");
  return out;

}


TGraphAsymmErrors* hist_to_graph(TH1D* hist, bool YErrorZero=false){

  TH1::SetDefaultSumw2(true);

  int Nbins = hist->GetXaxis()->GetNbins();
  double x[Nbins], y[Nbins], xlow[Nbins], xup[Nbins], ylow[Nbins], yup[Nbins];
  TAxis *xaxis = hist->GetXaxis();
  for(Int_t i=0; i<Nbins; i++){
    x[i] = xaxis->GetBinCenter(i+1);
    y[i] = hist->GetBinContent(i+1);
    xlow[i] = xaxis->GetBinCenter(i+1)-xaxis->GetBinLowEdge(i+1);
    xup[i] = xaxis->GetBinUpEdge(i+1)-xaxis->GetBinCenter(i+1);
    ylow[i] = hist->GetBinError(i+1);
    yup[i] = hist->GetBinError(i+1);
    if(YErrorZero){
      ylow[i] = 0;
      yup[i] = 0;
    }
    //ylow[i] = 0;
    //yup[i] = 0;
    //cout << "x = " << x[i] << ", y = " << y[i] << ", x_low = " << xlow[i] << ", xup = " << xup[i] << ", ylow = " << ylow[i] << ", yup = " << yup[i] << endl;
  }
  TGraphAsymmErrors *out = new TGraphAsymmErrors(Nbins, x, y, xlow, xup, ylow, yup);
  out->SetTitle("");
  return out;

}


TGraphAsymmErrors* hist_to_graph(TH1D* hist, int n_skip_x_left){

  TH1::SetDefaultSumw2(true);

  int Nbins = hist->GetXaxis()->GetNbins()-n_skip_x_left;
  double x[Nbins], y[Nbins], xlow[Nbins], xup[Nbins], ylow[Nbins], yup[Nbins];
  TAxis *xaxis = hist->GetXaxis();
  for(Int_t i=1; i<=Nbins; i++){
    x[i-1] = xaxis->GetBinCenter(i+n_skip_x_left);
    y[i-1] = hist->GetBinContent(i+n_skip_x_left);
    xlow[i-1] = xaxis->GetBinCenter(i+n_skip_x_left)-xaxis->GetBinLowEdge(i+n_skip_x_left);
    xup[i-1] = xaxis->GetBinUpEdge(i+n_skip_x_left)-xaxis->GetBinCenter(i+n_skip_x_left);
    ylow[i-1] = hist->GetBinError(i+n_skip_x_left);
    yup[i-1] = hist->GetBinError(i+n_skip_x_left);
    //ylow[i-1] = 0;
    //yup[i-1] = 0;
    //cout << "x = " << x[i-1] << ", y = " << y[i-1] << ", x_low = " << xlow[i-1] << ", xup = " << xup[i-1] << ", ylow = " << ylow[i-1] << ", yup = " << yup[i-1] << endl;
  }
  TGraphAsymmErrors *out = new TGraphAsymmErrors(Nbins, x, y, xlow, xup, ylow, yup);
  out->SetTitle("");
  return out;

}

TGraphAsymmErrors* hist_to_graph(TH1D* hist, int n_skip_x_left, int n_x_shift, int i_x_shift){

  TH1::SetDefaultSumw2(true);

  int Nbins = hist->GetXaxis()->GetNbins()-n_skip_x_left;
  double x[Nbins], y[Nbins], xlow[Nbins], xup[Nbins], ylow[Nbins], yup[Nbins];
  TAxis *xaxis = hist->GetXaxis();
  for(Int_t i=1; i<=Nbins; i++){
    x[i-1] = xaxis->GetBinCenter(i+n_skip_x_left);
    y[i-1] = hist->GetBinContent(i+n_skip_x_left);
    xlow[i-1] = xaxis->GetBinCenter(i+n_skip_x_left)-xaxis->GetBinLowEdge(i+n_skip_x_left);
    xup[i-1] = xaxis->GetBinUpEdge(i+n_skip_x_left)-xaxis->GetBinCenter(i+n_skip_x_left);
    ylow[i-1] = hist->GetBinError(i+n_skip_x_left);
    yup[i-1] = hist->GetBinError(i+n_skip_x_left);

    double dx = (xaxis->GetBinUpEdge(i+n_skip_x_left)-xaxis->GetBinLowEdge(i+n_skip_x_left))/double(n_x_shift+1);
    x[i-1] = xaxis->GetBinLowEdge(i+n_skip_x_left) + double(i_x_shift+1) * dx;
    xlow[i-1] = double(i_x_shift+1) * dx;
    xup[i-1] = xaxis->GetBinUpEdge(i+n_skip_x_left)-x[i-1];

    //ylow[i-1] = 0;
    //yup[i-1] = 0;
    //cout << "x = " << x[i-1] << ", y = " << y[i-1] << ", x_low = " << xlow[i-1] << ", xup = " << xup[i-1] << ", ylow = " << ylow[i-1] << ", yup = " << yup[i-1] << endl;
  }
  TGraphAsymmErrors *out = new TGraphAsymmErrors(Nbins, x, y, xlow, xup, ylow, yup);
  out->SetTitle("");
  return out;

}


TGraphAsymmErrors* GraphSubtract(TGraphAsymmErrors *a, TGraphAsymmErrors *b, bool Rel){

  //==== do a-b

  int NX = a->GetN();
  TGraphAsymmErrors* gr_out = (TGraphAsymmErrors*)a->Clone();

  for(int i=0; i<NX; i++){

    double a_x, a_y, b_x, b_y;

    a->GetPoint(i, a_x, a_y);
    b->GetPoint(i, b_x, b_y);

    if(Rel==true){
      gr_out->SetPoint( i, a_x, (a_y-b_y)/b_y );
    }
    else{
      gr_out->SetPoint( i, a_x, a_y-b_y );
    }

  }

  return gr_out;

}

void RemoveLargeError(TGraphAsymmErrors *a){

  int NX = a->GetN();

  for(int i=0; i<NX; i++){

    double x, y, yerr_low, yerr_high;

    a->GetPoint(i, x, y);
    yerr_low  = a->GetErrorYlow(i);
    yerr_high = a->GetErrorYhigh(i);

    if(y+yerr_high==1.){
      a->SetPointEYhigh( i, yerr_low );
    }

  }

}

void ScaleGraph(TGraphAsymmErrors *a, double c){

  int NX = a->GetN();

  for(int i=0; i<NX; i++){

    double x, y, yerr_low, yerr_high;

    a->GetPoint(i, x, y);
    yerr_low  = a->GetErrorYlow(i);
    yerr_high = a->GetErrorYhigh(i);

    a->SetPoint(i, x, c*y);
    a->SetPointEYlow(i, c*yerr_low);
    a->SetPointEYhigh(i, c*yerr_high);

  }

}



double GetMaximum(TH1D* hist){

  TAxis *xaxis = hist->GetXaxis();

  double maxval(-1.);
  for(int i=1; i<=xaxis->GetNbins(); i++){
    if( hist->GetBinContent(i) + hist->GetBinError(i) > maxval ){
      maxval = hist->GetBinContent(i) + hist->GetBinError(i);
    }
  }

  return maxval;

}

double GetMaximum(TGraphAsymmErrors *a){

  int NX = a->GetN();

  double maxval(-9999.);
  for(int i=0; i<NX; i++){

    double x, y, yerr_low, yerr_high;

    a->GetPoint(i, x, y);
    yerr_low  = a->GetErrorYlow(i);
    yerr_high = a->GetErrorYhigh(i);

    if( y+yerr_high > maxval ){
      maxval = y+yerr_high;
    }

  }

  return maxval;

}

double GetYieldSystematics(TH1D *hist){

  int n_syst = hist->GetXaxis()->GetNbins();
  int n_source = (n_syst-1)/2;

  //==== Bin index
  //==== i=1 : central
  //==== i=2 : _MuonEn_up
  //==== i=3 : _MuonEn_down
  //==== -> n_syst = 3
  //==== -> n_source = (n_syst-1)/2 = (3-1)/2 = 1

  double y_central = hist->GetBinContent(1);

  double sum_syst = 0.;
  for(int i=1; i<=n_source; i++){
    double yield_up = hist->GetBinContent(i*2);
    double yield_down = hist->GetBinContent(i*2+1);

    double syst_up = fabs(yield_up-y_central);
    double syst_down = fabs(yield_down-y_central);

    sum_syst += 0.5*(syst_up*syst_up+syst_down*syst_down);

  }

  return sqrt(sum_syst);

}

TDirectory *MakeTemporaryDirectory(){

  gROOT->cd();
  TDirectory* tempDir = 0;
  int counter = 0;
  while (not tempDir) {
    // First, let's find a directory name that doesn't exist yet:
    std::stringstream dirname;
    dirname << "HNCommonLeptonFakes_%i" << counter;
    if (gROOT->GetDirectory((dirname.str()).c_str())) {
      ++counter;
      continue;
    }
    // Let's try to make this directory:
    tempDir = gROOT->mkdir((dirname.str()).c_str());

  }

  return tempDir;

}

string DToString(double a, int prec=2){
  
  std::ostringstream streamObj2;
  streamObj2 << std::fixed;
  streamObj2 << std::setprecision(prec);
  streamObj2 << a;
  std::string strObj2 = streamObj2.str();
  return strObj2;
}

void AddPhantomZero(double a, TString align, int digit_int, int digit_frac){

  if(align=="r"){
    int number_maxdigit = 0;
    for(int i=10; i>=0; i--){
      if(a/pow(10,i)>=1.){
        number_maxdigit = i;
        break;
      }
    }
    //cout << "number_maxdigit = " << number_maxdigit << endl;
    for(int i=0; i<digit_int-(number_maxdigit+1); i++) cout << "\\phantom{0}";
    cout << std::fixed<<std::setprecision(digit_frac) << a;
  }

  else if(align=="l"){
    int target_total_digit = digit_int+digit_frac;
    int number_maxdigit = 0;
    for(int i=10; i>=0; i--){
      if(a/pow(10,i)>=1.){
        number_maxdigit = i;
        break;
      }
    }
    //cout << "target_total_digit = " << target_total_digit << endl;
    //cout << "number_maxdigit = " << number_maxdigit << endl;
    //cout << "--> # of \\phantom{0} = " << target_total_digit-digit_int-(number_maxdigit+1) << endl;

    cout << std::fixed<<std::setprecision(digit_frac) << a;
    for(int i=0; i<target_total_digit-(number_maxdigit+1)-digit_frac; i++) cout << "\\phantom{0}";
  }

}




#endif
