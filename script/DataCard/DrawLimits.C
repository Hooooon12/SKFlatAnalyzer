//Place it in /data6/Users/jihkim/HNDiLeptonWorskspace/src/LimitPlotter

#include "Macros.h"
#include "canvas_margin.h"

TString filepath = "/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/";
TString plotpath = "/data9/Users/HNL_public/SUS-24-014/LimitExtraction/plots/";

double GetDYxsec(int mass, TString channel);
double GetVBFxsec(int mass, TString channel);
double GetSSWWxsec(int mass, TString channel);
double GetDYxsec_17028(int mass, TString channel);
double GetVBFxsec_17028(int mass, TString channel);
double GetSSWWxsec_21003(int mass, TString channel);

void print_ratio_table(const vector<vector<double>>& mass_vs_nominal,
                       const vector<vector<double>>& ratio_vs_nominal,
                       const vector<double>& mass_nominal,
                       const vector<TString>& descrps,
                       const TString& filename,
                       const TString& table_title,
                       bool append=false
                       )
{
    ofstream fout(filename, append ? ios::app : ios::trunc); // append or truncate (recreate)
    if (!fout.is_open()) {
        cerr << "[ERROR] Cannot open file: " << filename << endl;
        return;
    }

    // Write channel name first
    fout << "[" << table_title << " channel]" << "\n";

    // BDTver 100 200 300 ...
    fout << left << setw(40) << "Limit setting | Mass";
    for (double m : mass_nominal)
        fout << setw(10) << fixed << setprecision(0) << m;
    fout << "\n";

    for (size_t i = 0; i < ratio_vs_nominal.size(); ++i) {
        ostringstream BDTStream;
        BDTStream << "<" << descrps[i] << ">";
        fout << left << setw(40) << BDTStream.str();
        for (double m : mass_nominal) {
            auto it = find(mass_vs_nominal[i].begin(), mass_vs_nominal[i].end(), m);
            if (it != mass_vs_nominal[i].end()) {
                int idx = distance(mass_vs_nominal[i].begin(), it);
                double ratio = ratio_vs_nominal[i][idx];
                double percent = (ratio - 1.0) * 100.0;
                ostringstream PercentStream;
                PercentStream << showpos << fixed << setprecision(1) << percent << "%";
                fout << setw(10) << PercentStream.str();
            } else {
                fout << setw(10) << "-";
            }
        }
        fout << "\n";
    }
    fout << "\n";

    fout.close();
    cout << "[INFO] Ratio table written to " << filename << endl;
}


void DrawLimits(TString year="", TString channel="", bool DrawExt=false, bool AddPub=true, int SepLimit=0, bool CompareLimits=false, bool AppendLimitTable=false, bool IsXsecLimit=false, bool Logy=true){

  // SepLimit = 0: No separated limit
  // SepLimit = 1: Signal separated limit
  // SepLimit = 2: SR separated limit

  if(DrawExt && CompareLimits){
    cout << "[INFO] DrawExt and CompareLimits are not supported simultaneously. Sorry!" << endl;
    cout << "[INFO] Exiting..." << endl;
    return;
  }
  if(AddPub && SepLimit){
    cout << "[INFO] Avoid drawing published analyses and separated limits, too messy!" << endl;
    cout << "[INFO] Exiting..." << endl;
    return;
  }

  TString AddPubTxt = "";
  if(AddPub) AddPubTxt = "_AddPub";
  TString SepLimitTxt = "";
  if(SepLimit==1) SepLimitTxt = "_SigSep";
  else if(SepLimit==2) SepLimitTxt = "_SRSep";

  bool DrawObserved = false;

  setTDRStyle();

  //gStyle->SetOptStat(0);

  //TString WP_nom = "PR48_rateParam_HNL_ULID"; // nominal working point
  //TString WP_nom = "PR86_HNL_ULID_Decorr"; // nominal working point
  //TString WP_nom = "PR97_HNL_ULIDv2_NoCR_NoSyst"; // nominal working point
  //TString WP_nom = "ANv3_HNL_ULIDv2_Decorr_NoCR"; // nominal working point
  //TString WP_nom = "ANv3_HNL_ULIDv2_Decorr_TEST_NoCR"; // nominal working point
  //TString WP_nom = "ANv3_HNL_ULIDv2_Decorr"; // nominal working point
  //TString WP_nom = "ANv3_HNL_ULIDv2_Decorr_Run2"; // nominal working point
  //TString WP_nom = "ANv4_HNL_ULIDv2_RunSyst_Decorr_JetDecorr_NoCR"; // nominal working point
  //TString WP_nom = "ANv4_HNL_ULIDv2_RunSyst_Decorr_JetDecorr"; // nominal working point
  //TString WP_nom = "ANv5_HNL_ULIDv2_RunSyst_Decorr_JetDecorr"; // nominal working point
  //TString WP_nom = "ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewRP"; // nominal working point

  //TString WP_nom = "ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewRP"; // set the nominal WP
  //TString WP_nom = "ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_FixCorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"; // set the nominal WP
  //TString WP_nom = "ANv6_NewSignals_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"; // set the nominal WP
  //TString WP_nom = "ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"; // set the nominal WP
  TString WP_nom = "ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"; // set the nominal WP
  vector<TString> WP_noms;
  if(DrawExt) WP_noms = {WP_nom+"_BDT", WP_nom+"_Ext"}; // nominal working points; BDT: up to 500 GeV, Ext: from 500 GeV
  else WP_noms = {WP_nom}; // nominal working point

  // refine nominal WP name
  TString WP_name;
  if(WP_noms.size()==1) WP_name = WP_nom;
  else if(DrawExt) WP_name = WP_nom+"_Ext";

  //TString tag_nom = "_syst"; // nominal tag
  //TString tag_nom = "_sr_Combined"; // nominal tag
  //TString tag_nom = "_syst_Run2Scaled"; // nominal tag
  //TString tag_nom = "_sronly_sr123_Run2Scaled"; // nominal tag
  //TString tag_nom = "_sronly_sr123_syst_Run2Scaled"; // nominal tag
  //TString tag_nom = "_DYVBF_sronly_sr123_syst_Run2Scaled"; // nominal tag
  //TString tag_nom = "_syst_Run2Scaled"; // nominal tag
  //TString tag_nom = "_sronly_sr123_syst"; // nominal tag
  TString tag_nom = "_syst"; // default setting
  //TString tag_nom = "_DY_syst"; // nominal tag
  //TString tag_nom = "_VBF_syst"; // nominal tag
  //TString tag_nom = "_DYVBF_syst"; // nominal tag
  //TString tag_nom = "_SSWW_syst"; // nominal tag
  //TString tag_nom = "_sr1_syst_Combined"; // nominal tag
  //TString tag_nom = "_sr2_syst_Combined"; // nominal tag
  //TString tag_nom = "_sr3_syst_Combined"; // nominal tag
  //TString tag_nom = "_syst_Run23Scaled"; // nominal tag
  TString method_nom = "Asym"; // nominal limit method
  TString Name_IsXsecLimit = "_mixing";
  if(IsXsecLimit) Name_IsXsecLimit = "_xsec";

  TString this_plotpath = plotpath+WP_name;
  if( !gSystem->mkdir(plotpath+WP_name, kTRUE) ){
    cout
    << "###################################################" << endl
    << "Directoy " << this_plotpath << " is created" << endl
    << "###################################################" << endl
    << endl;
  }

  vector<TString> files; // Multiple limits of this analysis to compare each other
  vector<double> scales;
  for(int i=0; i<WP_noms.size(); i++){
    files.push_back(filepath+WP_noms[i]+"/"+year+"_"+channel+tag_nom+"_"+method_nom+"_limit.txt"); // add files systematically; Allow multiple noms.
    scales.push_back(0.01);
  }

  TString method = "Asym"; //"Full";
  vector<TString> WPs = {}; // Nothing to draw other than nominals
  //WPs.push_back("ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewRP"); // add WP you want to overlay
  //WPs.push_back("ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_FixCorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"); // add WP you want to overlay
  //WPs.push_back("ANv6_NewSignals_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"); // add WP you want to overlay
  //WPs.push_back("ANv6_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr"); // add WP you want to overlay
  //WPs.push_back("ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr"); // add WP you want to overlay
  //WPs.push_back("ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"); // add WP you want to overlay
  WPs.push_back("ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr"); // add WP you want to overlay
  if(SepLimit) WPs = {WP_nom}; // same name with the nominal, but separate each signal/SR
  vector<TString> tags = {"_syst"}; // Default setting
  //vector<TString> tags = {"_DY_syst"};
  //vector<TString> tags = {"_VBF_syst"};
  //vector<TString> tags = {"_DYVBF_syst"};
  //vector<TString> tags = {"_SSWW_syst"};
  //vector<TString> tags = {"_sr1_syst_Combined"};
  //vector<TString> tags = {"_sr2_syst_Combined"};
  //vector<TString> tags = {"_sr3_syst_Combined"};
  if(SepLimit==1) tags = {"_DY_syst","_VBF_syst","_DYVBF_syst","_SSWW_syst"};
  else if(SepLimit==2) tags = {"_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined"};
  for(int i=0; i<WPs.size(); i++){
    for(int j=0; j<tags.size(); j++){
      //files.push_back(filepath+WPs[i]+"/"+year+"_"+channel+tags[j]+"_"+method+"_limit.txt"); // add files systematically
      files.push_back(filepath+WPs[i]+"/Run2_"+channel+tags[j]+"_"+method+"_limit.txt"); // add files systematically
      scales.push_back(0.01);
    }
  }
  //if(channel=="EE"||channel=="MuMu") files.push_back(filepath+"240503_exo17028/"+channel+"_HNTightV2_Run2_Asym_limit.txt"); // add additional files
  //scales.push_back(1.); // already scaled when extracting limits...

  vector<vector<double>> masses, obss, limits, onesig_lefts, onesig_rights, twosig_lefts, twosig_rights;
  vector<int> n_centrals;

  for(int i=0; i<files.size(); i++){

    //=== 13 TeV full Run2 (this analysis)
    string elline;
    cout << "Reading "+files.at(i)+" ..." << endl;
    ifstream in;
    in.open(files.at(i));
    vector<double> mass, obs, limit, onesig_left, onesig_right, twosig_left, twosig_right;

    int dummyint=0;
    double max_obs = 0., max_obs_mass = 0.;
    double min_obs = 9999., min_obs_mass = 0.;
    while(getline(in,elline)){
      cout << elline << endl;
      double this_mass, this_obs, this_limit, this_onesig_left, this_onesig_right, this_twosig_left, this_twosig_right;
      std::istringstream is( elline );
      TString this_line = elline;
      if(this_line.Contains("#")||this_line=="") continue;
      if (is >> this_mass) mass.push_back(this_mass);
      if (is >> this_obs) obs.push_back(this_obs);
      if (is >> this_twosig_left) twosig_left.push_back(this_twosig_left);
      if (is >> this_onesig_left) onesig_left.push_back(this_onesig_left);
      if (is >> this_limit) limit.push_back(this_limit);
      if (is >> this_onesig_right) onesig_right.push_back(this_onesig_right);
      if (is >> this_twosig_right) twosig_right.push_back(this_twosig_right);

      double scale = scales.at(i);
      //if(i==0&&this_mass>3000.) scale *= 10; //NOTE Nominal only: SSWW-only region has mass-dependent scaler.. why? to see SSWW pull with narrower range
      if(this_mass>3000.) scale *= 10; //NOTE All limit sets: SSWW-only region has mass-dependent scaler.. why? to see SSWW pull with narrower range
      //double scale=1.;
      //if(mass[dummyint]<=100) scale *= 0.001; // 0.001 only for low mass (https://cms-talk.web.cern.ch/t/too-large-error-with-hybridnew/32844)
      //else scale *= 0.01; // input signal scaled as V^2 = 0.01 by default

      obs[dummyint] *= scale;
      limit[dummyint] *= scale;
      onesig_left[dummyint] *= scale;
      onesig_right[dummyint] *= scale;
      twosig_left[dummyint] *= scale;
      twosig_right[dummyint] *= scale;
      // Now limits have been obtained. These are r, or |V|^2 or |V|^2/2.

      if(IsXsecLimit){

        double this_DYxsec = GetDYxsec(this_mass, channel);
        double this_VBFxsec = GetVBFxsec(this_mass, channel);
        double this_SSWWxsec = GetSSWWxsec(this_mass, channel);
        if(this_mass==500.){
          this_SSWWxsec = 0;
          if(DrawExt&&i==1) this_SSWWxsec = GetSSWWxsec(500, channel); // FIXME Ext M500 uses SSWW, BDT M500 not. Super hard coding.. Beware!!
        }
        if(channel=="EMu"){
          if(tag_nom.Contains("DY")||tag_nom.Contains("VBF")){
            obs[dummyint]          = obs[dummyint]          * (this_DYxsec+this_VBFxsec) ;
            limit[dummyint]        = limit[dummyint]        * (this_DYxsec+this_VBFxsec) ;
            onesig_left[dummyint]  = onesig_left[dummyint]  * (this_DYxsec+this_VBFxsec) ;
            onesig_right[dummyint] = onesig_right[dummyint] * (this_DYxsec+this_VBFxsec) ;
            twosig_left[dummyint]  = twosig_left[dummyint]  * (this_DYxsec+this_VBFxsec) ;
            twosig_right[dummyint] = twosig_right[dummyint] * (this_DYxsec+this_VBFxsec) ;
          }
          else if(tag_nom.Contains("SSWW")){
            obs[dummyint]          = 4*obs[dummyint]          *obs[dummyint]          *this_SSWWxsec;
            limit[dummyint]        = 4*limit[dummyint]        *limit[dummyint]        *this_SSWWxsec;
            onesig_left[dummyint]  = 4*onesig_left[dummyint]  *onesig_left[dummyint]  *this_SSWWxsec;
            onesig_right[dummyint] = 4*onesig_right[dummyint] *onesig_right[dummyint] *this_SSWWxsec;
            twosig_left[dummyint]  = 4*twosig_left[dummyint]  *twosig_left[dummyint]  *this_SSWWxsec;
            twosig_right[dummyint] = 4*twosig_right[dummyint] *twosig_right[dummyint] *this_SSWWxsec;
          }
          else{
            obs[dummyint]          = obs[dummyint]          * (this_DYxsec+this_VBFxsec) + 4*obs[dummyint]          *obs[dummyint]          *this_SSWWxsec;
            limit[dummyint]        = limit[dummyint]        * (this_DYxsec+this_VBFxsec) + 4*limit[dummyint]        *limit[dummyint]        *this_SSWWxsec;
            onesig_left[dummyint]  = onesig_left[dummyint]  * (this_DYxsec+this_VBFxsec) + 4*onesig_left[dummyint]  *onesig_left[dummyint]  *this_SSWWxsec;
            onesig_right[dummyint] = onesig_right[dummyint] * (this_DYxsec+this_VBFxsec) + 4*onesig_right[dummyint] *onesig_right[dummyint] *this_SSWWxsec;
            twosig_left[dummyint]  = twosig_left[dummyint]  * (this_DYxsec+this_VBFxsec) + 4*twosig_left[dummyint]  *twosig_left[dummyint]  *this_SSWWxsec;
            twosig_right[dummyint] = twosig_right[dummyint] * (this_DYxsec+this_VBFxsec) + 4*twosig_right[dummyint] *twosig_right[dummyint] *this_SSWWxsec;
          }
        }
        else{
          if(tag_nom.Contains("DY")||tag_nom.Contains("VBF")){
            obs[dummyint]          = obs[dummyint]          * (this_DYxsec+this_VBFxsec) ;
            limit[dummyint]        = limit[dummyint]        * (this_DYxsec+this_VBFxsec) ;
            onesig_left[dummyint]  = onesig_left[dummyint]  * (this_DYxsec+this_VBFxsec) ;
            onesig_right[dummyint] = onesig_right[dummyint] * (this_DYxsec+this_VBFxsec) ;
            twosig_left[dummyint]  = twosig_left[dummyint]  * (this_DYxsec+this_VBFxsec) ;
            twosig_right[dummyint] = twosig_right[dummyint] * (this_DYxsec+this_VBFxsec) ;
          }
          else if(tag_nom.Contains("SSWW")){
            obs[dummyint]          = obs[dummyint]          *obs[dummyint]          *this_SSWWxsec;
            limit[dummyint]        = limit[dummyint]        *limit[dummyint]        *this_SSWWxsec;
            onesig_left[dummyint]  = onesig_left[dummyint]  *onesig_left[dummyint]  *this_SSWWxsec;
            onesig_right[dummyint] = onesig_right[dummyint] *onesig_right[dummyint] *this_SSWWxsec;
            twosig_left[dummyint]  = twosig_left[dummyint]  *twosig_left[dummyint]  *this_SSWWxsec;
            twosig_right[dummyint] = twosig_right[dummyint] *twosig_right[dummyint] *this_SSWWxsec;
          }
          else{
            obs[dummyint]          = obs[dummyint]          * (this_DYxsec+this_VBFxsec) + obs[dummyint]          *obs[dummyint]          *this_SSWWxsec;
            limit[dummyint]        = limit[dummyint]        * (this_DYxsec+this_VBFxsec) + limit[dummyint]        *limit[dummyint]        *this_SSWWxsec;
            onesig_left[dummyint]  = onesig_left[dummyint]  * (this_DYxsec+this_VBFxsec) + onesig_left[dummyint]  *onesig_left[dummyint]  *this_SSWWxsec;
            onesig_right[dummyint] = onesig_right[dummyint] * (this_DYxsec+this_VBFxsec) + onesig_right[dummyint] *onesig_right[dummyint] *this_SSWWxsec;
            twosig_left[dummyint]  = twosig_left[dummyint]  * (this_DYxsec+this_VBFxsec) + twosig_left[dummyint]  *twosig_left[dummyint]  *this_SSWWxsec;
            twosig_right[dummyint] = twosig_right[dummyint] * (this_DYxsec+this_VBFxsec) + twosig_right[dummyint] *twosig_right[dummyint] *this_SSWWxsec;
          }
        }

      }

      onesig_left[dummyint] = limit[dummyint]-onesig_left[dummyint];
      onesig_right[dummyint] = onesig_right[dummyint] - limit[dummyint];
      twosig_left[dummyint] = limit[dummyint]-twosig_left[dummyint];
      twosig_right[dummyint] = twosig_right[dummyint] - limit[dummyint];

      if(max_obs<obs[dummyint]){
        max_obs = obs[dummyint];
        max_obs_mass = mass[dummyint];
      }
      if(min_obs>obs[dummyint]){
        min_obs = obs[dummyint];
        min_obs_mass = mass[dummyint];
      }

      dummyint++;
    }

    int n_central = dummyint;
    cout << "N mass points : " << n_central << endl;
    for(unsigned int k = 0; k < n_central ; k++){
      cout << "Mass = " << mass[k] << " expected = " << limit[k] << " + 1sigma = " << onesig_right[k] << " - 1sigma = "  << onesig_left[k] << " + 2sigma = " << twosig_right[k] << " - 2sigma = " << twosig_left[k] << endl;
    }
    cout << "Max : " << max_obs_mass << "\t" << max_obs << endl;
    cout << "Min : " << min_obs_mass << "\t" << min_obs << endl;

    n_centrals.push_back(n_central);
    masses.push_back(mass);
    obss.push_back(obs);
    limits.push_back(limit);
    onesig_lefts.push_back(onesig_left);
    onesig_rights.push_back(onesig_right);
    twosig_lefts.push_back(twosig_left);
    twosig_rights.push_back(twosig_right);

  }

  // Nominal limits
  TGraphAsymmErrors *gr_obs_0 = new TGraphAsymmErrors(n_centrals[0],&masses[0][0],&obss[0][0],0,0,0,0);
  gr_obs_0->SetLineWidth(3);
  gr_obs_0->SetLineColor(kBlack);

  TGraphAsymmErrors *gr_exp_0 = new TGraphAsymmErrors(n_centrals[0],&masses[0][0],&limits[0][0],0,0,0,0);
  gr_exp_0->SetLineWidth(3);
  gr_exp_0->SetLineStyle(2);
  gr_exp_0->SetLineColor(kBlack);

  TGraphAsymmErrors *gr_band_1sigma_0 = new TGraphAsymmErrors(n_centrals[0], &masses[0][0], &limits[0][0], 0, 0, &onesig_lefts[0][0], &onesig_rights[0][0]);
  gr_band_1sigma_0->SetFillColor(kGreen+1);
  gr_band_1sigma_0->SetLineColor(kGreen+1);
  gr_band_1sigma_0->SetMarkerColor(kGreen+1);

  TGraphAsymmErrors *gr_band_2sigma_0 = new TGraphAsymmErrors(n_centrals[0], &masses[0][0], &limits[0][0], 0, 0, &twosig_lefts[0][0], &twosig_rights[0][0]);
  gr_band_2sigma_0->SetFillColor(kOrange);
  gr_band_2sigma_0->SetLineColor(kOrange);
  gr_band_2sigma_0->SetMarkerColor(kOrange);

  TGraphAsymmErrors *gr_obs_1, *gr_exp_1, *gr_band_1sigma_1, *gr_band_2sigma_1, *gr_obs_2, *gr_exp_2, *gr_band_1sigma_2, *gr_band_2sigma_2;

  if(WP_noms.size()>1){
    gr_obs_1 = new TGraphAsymmErrors(n_centrals[1],&masses[1][0],&obss[1][0],0,0,0,0);
    gr_obs_1->SetLineWidth(3);
    gr_obs_1->SetLineColor(kBlack);

    gr_exp_1 = new TGraphAsymmErrors(n_centrals[1],&masses[1][0],&limits[1][0],0,0,0,0);
    gr_exp_1->SetLineWidth(3);
    gr_exp_1->SetLineStyle(2);
    gr_exp_1->SetLineColor(kBlack);

    gr_band_1sigma_1 = new TGraphAsymmErrors(n_centrals[1], &masses[1][0], &limits[1][0], 0, 0, &onesig_lefts[1][0], &onesig_rights[1][0]);
    gr_band_1sigma_1->SetFillColor(kGreen+1);
    gr_band_1sigma_1->SetLineColor(kGreen+1);
    gr_band_1sigma_1->SetMarkerColor(kGreen+1);

    gr_band_2sigma_1 = new TGraphAsymmErrors(n_centrals[1], &masses[1][0], &limits[1][0], 0, 0, &twosig_lefts[1][0], &twosig_rights[1][0]);
    gr_band_2sigma_1->SetFillColor(kOrange);
    gr_band_2sigma_1->SetLineColor(kOrange);
    gr_band_2sigma_1->SetMarkerColor(kOrange);
  };
  if(WP_noms.size()>2){
    gr_obs_2 = new TGraphAsymmErrors(n_centrals[2],&masses[2][0],&obss[2][0],0,0,0,0);
    gr_obs_2->SetLineWidth(3);
    gr_obs_2->SetLineColor(kBlack);

    gr_exp_2 = new TGraphAsymmErrors(n_centrals[2],&masses[2][0],&limits[2][0],0,0,0,0);
    gr_exp_2->SetLineWidth(3);
    gr_exp_2->SetLineStyle(2);
    gr_exp_2->SetLineColor(kBlack);

    gr_band_1sigma_2 = new TGraphAsymmErrors(n_centrals[2], &masses[2][0], &limits[2][0], 0, 0, &onesig_lefts[2][0], &onesig_rights[2][0]);
    gr_band_1sigma_2->SetFillColor(kGreen+1);
    gr_band_1sigma_2->SetLineColor(kGreen+1);
    gr_band_1sigma_2->SetMarkerColor(kGreen+1);

    gr_band_2sigma_2 = new TGraphAsymmErrors(n_centrals[2], &masses[2][0], &limits[2][0], 0, 0, &twosig_lefts[2][0], &twosig_rights[2][0]);
    gr_band_2sigma_2->SetFillColor(kOrange);
    gr_band_2sigma_2->SetLineColor(kOrange);
    gr_band_2sigma_2->SetMarkerColor(kOrange);
  };

  // Use when there are more than two input limits to compare
  vector<TGraph*> gr_exp_list;

  vector<int> colors = {kRed, kBlue, kViolet, kPink+6, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7,};
  if(SepLimit==1) colors = {kRed+2, kOrange-2, kRed, kBlue, kCyan, kPink+6, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7, kViolet+7,};
  //vector<int> styles = {1, 2, 3, 4, 5, 6};
  //vector<TString> descrps = {"BDTV3_Strict","BDTV3_Loose","BDTV4_Strict","BDTV4_Loose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose","BDTV4_VeryLoose"};
  vector<TString> descrps;

  if(SepLimit){
    for (auto &tag : tags) { // sr-dependent descriptions (tags: _sr1_syst_Combined, _sr2_syst_Combined, ...)
      tag.Remove(0,1); // remove length 1 string starting from the 0th entry
      tag.ReplaceAll("_syst_Combined", " only");
      tag.ReplaceAll("sr", "SR");

      tag.ReplaceAll("_syst", " ");
      tag.ReplaceAll("DYVBF", "DY+W#gamma");
      tag.ReplaceAll("DY ", "DY");
      tag.ReplaceAll("VBF", "W#gamma");
      tag.ReplaceAll("SSWW", "SSWW");

      descrps.push_back(tag);
    }
  }
  else{
    for (auto &wp : WPs) { // standard descrption: BDTV3_SR1_FixRepeatBin_AltBin_V3_Strict_15
      TString tmp = wp;
      if (tmp.BeginsWith("ANv5_")) tmp.Remove(0, 5);

      Ssiz_t pos = tmp.Index("_RunSyst");
      if (pos != kNPOS) tmp.Remove(pos);

      tmp.ReplaceAll("HNL_ULIDv2_", "");

      descrps.push_back(tmp);
    }
  }

  // Extract common masses between limits and make new limits (doesn't support DrawExt)
  if (!DrawExt && limits.size() > 1){
  
    for (size_t i = 1; i < limits.size(); ++i) { // start from limits to compare (0th limit == nominal)

      std::set<double> set0(masses[0].begin(), masses[0].end());
      std::set<double> seti(masses[i].begin(), masses[i].end());
      std::vector<double> common_mass_list;
      std::set_intersection(set0.begin(), set0.end(),
                            seti.begin(), seti.end(),
                            std::back_inserter(common_mass_list));
      std::sort(common_mass_list.begin(), common_mass_list.end());

      if (common_mass_list.empty()) {
        std::cout << "[WARN] No common masses between nominal and " << i << "th input.\n";
        continue;
      }
  
      vector<double> y_limit, y_1sig_l, y_1sig_r, y_2sig_l, y_2sig_r;
  
      for (double m : common_mass_list) {
        auto it = find(masses[i].begin(), masses[i].end(), m);
        if (it != masses[i].end()) {
          int idx = distance(masses[i].begin(), it);
          y_limit .push_back(limits[i][idx]);
          y_1sig_l.push_back(onesig_lefts[i][idx]);
          y_1sig_r.push_back(onesig_rights[i][idx]);
          y_2sig_l.push_back(twosig_lefts[i][idx]);
          y_2sig_r.push_back(twosig_rights[i][idx]);
        }
      }
  
      if (y_limit.size() != common_mass_list.size()) {
        cout << "[WARNING] Limit[" << i << "] incomplete. Skipping..." << endl;
        continue;
      }
  
      //TGraphAsymmErrors* gr_band_1sigma = new TGraphAsymmErrors(
      //  common_mass_list.size(), &common_mass_list[0], &y_limit[0],
      //  0, 0, &y_1sig_l[0], &y_1sig_r[0]
      //);
      //gr_band_1sigma->SetFillColor(kGreen+1);
      //gr_band_1sigma->SetLineColor(kGreen+1);
      //gr_band_1sigma->SetMarkerColor(kGreen+1);
      //gr_band_1sigma->SetLineStyle(styles[i % styles.size()]);
      //if (i == 0) gr_band_1sigma->Draw("A3");
      //else        gr_band_1sigma->Draw("3 SAME");
  
      //TGraphAsymmErrors* gr_band_2sigma = new TGraphAsymmErrors(
      //  common_mass_list.size(), &common_mass_list[0], &y_limit[0],
      //  0, 0, &y_2sig_l[0], &y_2sig_r[0]
      //);
      //gr_band_2sigma->SetFillColor(kOrange);
      //gr_band_2sigma->SetLineColor(kOrange);
      //gr_band_2sigma->SetMarkerColor(kOrange);
      //gr_band_2sigma->SetLineStyle(styles[i % styles.size()]);
      //gr_band_2sigma->Draw("3 SAME");
  
      TGraph* gr_limit = new TGraph(common_mass_list.size(), &common_mass_list[0], &y_limit[0]);
      gr_limit->SetLineWidth(3);
      gr_limit->SetLineColor(colors[i-1]);
      //gr_limit->SetLineStyle(styles[i % styles.size()]);
      gr_exp_list.push_back(gr_limit);
      
    }
  
  }

  //=== EXO-17-028 overlay
  const int nm_17028 = 22;
  vector<double> mass_17028 = {
    85, 90,
    100, 125, 150,200,
    250, 300, 400, 500,
    600, 700, 800, 900,
    1000, 1100, 1200, 1300,
    1400, 1500, 1700, 2000,
  };

  vector<double> obs_17028(nm_17028), exp_17028(nm_17028);
  vector<double> tempvec_obs_17028, tempvec_exp_17028;
  vector<double> scales_17028;
  if(channel=="MuMu"){ // https://github.com/jedori0228/HiggsAnalysis-CombinedLimit/blob/2016Data_HNDilepton_Limit/data/2016_HNDiLepton/Outputs_Tool/MuMu_Combined/result_VBF.txt
    //tempvec_exp_17028 = {
    //  175.333, 21.5041, 32.925, 56.3397,
    //  70.8081, 99.3095, 20.4264, 42.5126,
    //  60.1695, 116.721, 15.8605, 25.8407,
    //  38.43, 64.346, 100.265, 151.699,
    //  247.709, 340.424, 1340.34
    //}; // these are the DYTypeI results
    tempvec_exp_17028 = {
      357.479, 279.381,
      175.333, 21.5041, 32.925, 56.3397,
      70.8081, 95.0624, 18.8665, 38.4947,
      42.8618, 74.4406, 8.4652, 12.594,
      16.3718, 23.4646, 32.9925, 43.0679,
      61.6472, 74.073,  135.185, 304.058
    }; // DY+VBF TypeI results
    scales_17028 = {
      0.001, 0.001,
      0.001, 0.01, 0.01,0.01,
      0.01,0.01,0.1, 0.1,
      0.1,0.1,1,1,
      1,1,1,1,
      1,1,1,1
    };
    //tempvec_obs_17028 = {
    //  215.218, 23.0424,41.8101,49.4399,
    //  57.1134,84.404,39.6932,44.5303,
    //  81.4561,195.31,16.3137,42.605,
    //  61.6358,103.589,150.295,220.286,
    //  365.037, 516.2, 1408.5
    //}; // these are the DYTypeI results
    tempvec_obs_17028 = {
      415.315, 291.969,
      215.218, 23.0424, 41.8101, 49.4399,
      57.1134, 81.2452, 36.7668, 40.1455,
      56.0452, 122.926, 8.8852,  21.2021,
      27.1287, 38.7571, 48.9324, 63.9919,
      91.9852, 116.982, 142.081, 311.967
    }; // DY+VBF TypeI results
  }
  else   if(channel=="EE"){
    //https://github.com/jedori0228/HiggsAnalysis-CombinedLimit/blob/2016Data_HNDilepton_Limit/data/2016_HNDiLepton/Outputs_Tool/ElEl_Combined/result.txt
    //tempvec_exp_17028 = {
    //  467.448, 65.4099, 90.4068, 159.838,
    //  216.957, 284.563, 59.74, 94.6793, 
    //  104.302, 183.121, 30.189, 47.1442,
    //  72.0759, 117.305, 183.214, 285.811,
    //  434.08, 644.258, 2506.94
    //}; // these are the DYTypeI only results;
    tempvec_exp_17028 = { // https://github.com/jedori0228/HiggsAnalysis-CombinedLimit/blob/2016Data_HNDilepton_Limit/data/2016_HNDiLepton/Outputs_Tool/ElEl_Combined/result_VBF.txt
      1015.18, 1030.84,
      467.448, 65.4099, 90.4068, 159.838,
      216.957, 268.406, 53.8654, 78.1765, 
      76.2341, 118.242, 17.2123, 24.2464,
      31.61,   46.3963, 64.5734, 89.4366,
      115.964, 147.772, 279.822, 632.318
    }; // DY+VBF TypeI results;
    scales_17028 = {
      0.001, 0.001,
      0.001, 0.01, 0.01,0.01,0.01,0.01,0.1, 0.1,0.1,0.1,1,1,1,1,1,1,1,1,1,1
    };
    //tempvec_obs_17028 = {
    //  368.924, 63.3389, 61.9159, 151.2,
    //  206.654, 254.261, 68.8604, 95.9664, 
    //  123.0, 274.57, 24.8148, 46.0243,
    //  95.1426, 164.011, 252.706, 379.988,
    //  419.316, 631.767, 2486.31
    //}; // these are the DYTypeI only results;
    tempvec_obs_17028 = {
      1058.35, 937.444,
      368.924, 63.3389, 61.9159, 151.2,
      206.654, 235.791, 63.261,  79.9909, 
      91.403,  174.955, 14.6124, 23.4109,
      43.3134, 64.2114, 86.8003, 117.953,
      112.79,  143.465, 276.02,  626.971
    }; // DY+VBF TypeI results
  }
  else   if(channel=="EMu"){ // https://github.com/jedori0228/HiggsAnalysis-CombinedLimit/blob/2016Data_HNDilepton_Limit/data/2016_HNDiLepton/Outputs_Tool/MuEl_Combined/result_VBF.txt
    tempvec_exp_17028 = {
      913.101, 614.114,
      410.683, 55.9408, 82.975,  136.932,
      178.214, 218.714, 39.6317, 46.813, 
      88.4598, 123.904, 14.4261, 17.8333,
      26.8884, 33.8577, 46.471,  61.5947,
      82.8097, 124.842, 257.358, 512.365
    }; // DY+VBF
    scales_17028 = {
      0.001, 0.001,
      0.001, 0.01, 0.01,0.01,0.01,0.01,0.1, 0.1,0.1,0.1,1,1,1,1,1,1,1,1,1,1
    };
    tempvec_obs_17028 = {
      742.394, 368.479,
      290.008, 33.6466, 62.3902, 187.344,
      168.976, 257.416, 51.8467, 45.2587, 
      90.0862, 134.809, 22.2963, 18.1025,
      28.0044, 34.4163, 46.4722, 61.1279,
      83.0085, 123.863, 245.571, 508.857
    }; // DY+VBF TypeI results
  }
  cout << "Channel : " << channel << endl;
  for(unsigned int j=0; j<tempvec_obs_17028.size(); j++){
    exp_17028[j] = (channel=="EMu") ? scales_17028[j]*tempvec_exp_17028.at(j)*0.01*0.5 : scales_17028[j]*tempvec_exp_17028.at(j)*0.01;
    obs_17028[j] = (channel=="EMu") ? scales_17028[j]*tempvec_obs_17028.at(j)*0.01*0.5 : scales_17028[j]*tempvec_obs_17028.at(j)*0.01;
    //cout << "mN = " << mass_17028[j] << " 17028 obs limit = " <<  scales_17028[j]*tempvec_obs_17028.at(j)*0.01 << endl;
  }

  // Calculate the xsec limits
  int this_nm_17028 = nm_17028;
  if(IsXsecLimit&&AddPub){
    vector<double> remove_mass_17028 = {125, 250, 1700, 2000}; // VBF xsec doesn't exist for these masses; 1700 and 2000 --> mixing already above 1, poorly optimized.
    this_nm_17028 = this_nm_17028-remove_mass_17028.size();

    for (int i = nm_17028-1; i >= 0; --i) {
      for (double rm : remove_mass_17028) {
        if (mass_17028[i] == rm) {
          mass_17028.erase(mass_17028.begin() + i);
          exp_17028.erase(exp_17028.begin() + i);
          obs_17028.erase(obs_17028.begin() + i);
          break;
        }
      }
    }

    double this_DYxsec;
    double this_VBFxsec;
    for(unsigned int j=0; j<obs_17028.size(); j++){
      this_DYxsec  = GetDYxsec_17028 (mass_17028[j], channel);
      this_VBFxsec = GetVBFxsec_17028(mass_17028[j], channel);
      exp_17028[j] = exp_17028[j] * (this_DYxsec+this_VBFxsec);
      obs_17028[j] = obs_17028[j] * (this_DYxsec+this_VBFxsec);
    }
  }

  TGraph *gr_17028_exp = new TGraph(this_nm_17028, &mass_17028[0], &exp_17028[0]);
  gr_17028_exp->SetLineColor(kRed);
  gr_17028_exp->SetLineWidth(3);
  TGraph *gr_17028_obs = new TGraph(this_nm_17028, &mass_17028[0], &obs_17028[0]);
  gr_17028_obs->SetLineColor(kRed);
  gr_17028_obs->SetLineWidth(3);


  //=== trilep overlay
  int temp_n_mass_trilep = 31;
  vector<double> temp_mass_trilep = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 20, 30, 40, 50, 60, 70, 75, 85, 90, 95, 100, 130, 150, 200, 400, 600, 800, 1000, 1200,
  };
  vector<double> tmpvec_obs_trilep = {
    0.0163199, 0.00182936, 0.00059415, 0.000280499, 0.000149561, 0.000103852, 6.81653E-05, 4.97358E-05, 3.52154E-05, 3.12335E-05, 2.46796E-05, 2.13769E-05, 1.48506E-05, 1.76593E-05, 1.87597E-05, 1.80473E-05, 2.77739E-05, 0.000156184, 0.000742155, 0.00241651, 0.00289491, 0.00345643, 0.00431929, 0.00716543, 0.00796569, 0.00888232, 0.0301448, 0.0835229, 0.20645, 0.441043, 0.847844,
  };

  if(channel=="EE"){
    temp_n_mass_trilep = 30;
    temp_mass_trilep = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 20, 30, 40, 50, 60, 70, 75, 85, 90, 100, 130, 150, 200, 400, 600, 800, 1000, 1200,
    };
    tmpvec_obs_trilep = {
      0.0135784, 0.00151879, 0.000447416, 0.000223742, 0.00011303, 7.23346E-05, 5.10346E-05, 3.77547E-05, 2.72229E-05, 2.33201E-05, 1.8688E-05, 1.75216E-05, 1.20661E-05, 1.60735E-05, 2.19737E-05, 3.32274E-05, 6.70456E-05, 0.000564676, 0.00186338, 0.00629086, 0.00622036, 0.00652451, 0.010974, 0.014264, 0.0135633, 0.0523753, 0.167425, 0.428148, 0.949388, 1.83977,
    };
  }

  const int n_mass_trilep = temp_n_mass_trilep;
  double mass_trilep[n_mass_trilep];
  double obs_trilep[n_mass_trilep];
  for(unsigned int i=0; i<tmpvec_obs_trilep.size(); i++){
    mass_trilep[i] = temp_mass_trilep.at(i);
    obs_trilep[i] = tmpvec_obs_trilep.at(i);
    //cout << i << "\t" << temp_mass_trilep[i] << "\t" << obs_trilep[i] << endl;
  }
  TGraph *gr_trilepLimit = new TGraph(n_mass_trilep, mass_trilep, obs_trilep);
  gr_trilepLimit->SetLineWidth(3);
  gr_trilepLimit->SetLineStyle(4);
  gr_trilepLimit->SetLineColor(kRed);


  //=== EXO-21-003 overlay
  const int n_mass_21003 = 19;
  vector<double> mass_21003 = {50,150,300,450,600,750,900,1000,1250,1500,1750,2000,2500,5000,7500,10000,15000,20000,25000};
  vector<double> obs_21003 = {0.0632,0.0125,0.0070,0.0061,0.0060,0.0066,0.0067,0.0075,0.0086,0.0098,0.0117,0.0136,0.0189,0.0539,0.1081,0.1908,0.4021,0.7433,1.1322};
  vector<double> obs_21003_sqrt = {0.2514, 0.1118, 0.0837, 0.0781, 0.0775, 0.0812, 0.0819, 0.0866, 0.0927, 0.099, 0.1082, 0.1166, 0.1375, 0.2322, 0.3288, 0.4368, 0.6341, 0.8621, 1.064}; //xcheck with https://www.hepdata.net/record/131287
  vector<double> twolow_21003 = {0.0487,0.0100,0.0060,0.0048,0.0048,0.0050,0.0053,0.0057,0.0071,0.0081,0.0092,0.0111,0.0154,0.0438,0.0877,0.1552,0.3264,0.6055,0.9196};
  vector<double> onelow_21003 = {0.0668,0.0141,0.0076,0.0072,0.0068,0.0075,0.0071,0.0086,0.0102,0.0113,0.0132,0.0152,0.0213,0.0609,0.1220,0.2158,0.4540,0.8421,1.2790};
  vector<double> exp_21003 = {0.0981,0.0200,0.0112,0.0103,0.0103,0.0107,0.0112,0.0122,0.0142,0.0161,0.0190,0.0229,0.0317,0.0903,0.1812,0.3203,0.6738,1.2500,1.8984};
  vector<double> exp_21003_sqrt = { 0.31320920, 0.14142136, 0.10583005, 0.10148892, 0.10148892, 0.10344080, 0.10583005, 0.11045361, 0.11916375, 0.12688578, 0.13784049, 0.15132746, 0.17804494, 0.30049958, 0.42567593, 0.56595053, 0.82085321, 1.1180340, 1.3778244 };
  vector<double> onehigh_21003 = {0.1459,0.0306,0.0173,0.0152,0.0152,0.0166,0.0166,0.0188,0.0218,0.0251,0.0297,0.0343,0.0477,0.1368,0.2757,0.4863,1.0257,1.9027,2.8822};
  vector<double> twohigh_21003 = {0.2096,0.0443,0.0255,0.0224,0.0224,0.0239,0.0243,0.0274,0.0321,0.0367,0.0433,0.0501,0.0702,0.2014,0.4058,0.7168,1.5094,2.8000,4.2485};

  vector<double> this_exp_21003 = exp_21003_sqrt;
  vector<double> this_obs_21003 = obs_21003_sqrt; // mixing limits

  // Calculate the xsec limits
  int this_nm_21003 = n_mass_21003;
  if(IsXsecLimit&&AddPub){
    vector<double> remove_mass_21003 = {50, 150, 300, 450, 600, 750, 25000}; // skip the low masses; 25000 --> SSWW xsec doesn't exist for these masses
    this_nm_21003 = this_nm_21003-remove_mass_21003.size();

    for (int i = n_mass_21003-1; i >= 0; --i) {
      for (double rm : remove_mass_21003) {
        if (mass_21003[i] == rm) {
          mass_21003.erase(mass_21003.begin() + i);
          exp_21003.erase(exp_21003.begin() + i);
          obs_21003.erase(obs_21003.begin() + i);
          break;
        }
      }
    }

    double this_SSWWxsec;
    for(unsigned int j=0; j<obs_21003.size(); j++){
      this_SSWWxsec = GetSSWWxsec_21003(mass_21003[j], channel);
      exp_21003[j] = exp_21003[j] * this_SSWWxsec;
      obs_21003[j] = obs_21003[j] * this_SSWWxsec;
    }

    this_exp_21003 = exp_21003;
    this_obs_21003 = obs_21003;

  }

  TGraph *gr_21003_exp = new TGraph(this_nm_21003, &mass_21003[0], &this_exp_21003[0]);
  gr_21003_exp->SetLineWidth(3);
  gr_21003_exp->SetLineColor(kBlue);
  TGraph *gr_21003_obs = new TGraph(this_nm_21003, &mass_21003[0], &this_obs_21003[0]);
  gr_21003_obs->SetLineWidth(3);
  gr_21003_obs->SetLineColor(kBlue);

  //=== EXOT-2020-06 overlay
  const int n_mass_202006 = 19;
  double mass_202006[n_mass_202006] = {50,100,250,400,500,750,900,1000,1250,1500,1750,2000,2500,3000,5000,7500,10000,15000,20000};
  double obs_202006[n_mass_202006] = {0.342, 0.199, 0.117, 0.104, 0.101, 0.102, 0.11, 0.109, 0.121, 0.127, 0.14, 0.15, 0.173, 0.199, 0.296, 0.43, 0.564, 0.809, 1.107}; // https://www.hepdata.net/record/ins2662303
  double twolow_202006[n_mass_202006] = {0.201, 0.116, 0.07, 0.062, 0.061, 0.064, 0.069, 0.068, 0.076, 0.08, 0.089, 0.095, 0.11, 0.127, 0.189, 0.273, 0.362, 0.521, 0.712};
  double onelow_202006[n_mass_202006] = {0.233, 0.134, 0.082, 0.072, 0.071, 0.074, 0.08, 0.079, 0.088, 0.093, 0.103, 0.11, 0.127, 0.147, 0.219, 0.317, 0.419, 0.604, 0.825};
  double exp_202006[n_mass_202006] = {0.274, 0.158, 0.096, 0.085, 0.084, 0.087, 0.094, 0.093, 0.104, 0.109, 0.121, 0.13, 0.15, 0.173, 0.258, 0.373, 0.494, 0.711, 0.972};
  double onehigh_202006[n_mass_202006] = {0.329, 0.19, 0.116, 0.103, 0.102, 0.106, 0.114, 0.113, 0.126, 0.132, 0.147, 0.158, 0.182, 0.21, 0.313, 0.453, 0.6, 0.863, 1.181};
  double twohigh_202006[n_mass_202006] = {0.394, 0.229, 0.141, 0.125, 0.124, 0.129, 0.139, 0.138, 0.154, 0.161, 0.179, 0.192, 0.222, 0.256, 0.382, 0.552, 0.731, 1.052, 1.442};

  TGraph *gr_202006_exp = new TGraph(n_mass_202006, mass_202006, exp_202006);
  gr_202006_exp->SetLineWidth(3);
  gr_202006_exp->SetLineColor(kBlue-9);
  TGraph *gr_202006_obs = new TGraph(n_mass_202006, mass_202006, obs_202006);
  gr_202006_obs->SetLineWidth(3);
  gr_202006_obs->SetLineColor(kBlue-9);

  //=== EXOT-2023-16 overlay
  int n_mass_202316;
  vector<double> mass_202316, obs_202316_orig, twolow_202316_orig, onelow_202316_orig, exp_202316_orig, onehigh_202316_orig, twohigh_202316_orig, obs_202316, twolow_202316, onelow_202316, exp_202316, onehigh_202316, twohigh_202316;

  if(channel=="EE"){
    n_mass_202316 = 17;
    mass_202316 = {50,100,250,400,500,750,900,1000,1250,1500,1750,2000,2500,3000,5000,7500,10000};
    obs_202316 = {0.414, 0.228, 0.133, 0.117, 0.114, 0.117, 0.124, 0.124, 0.139, 0.143, 0.161, 0.172, 0.202, 0.226, 0.339, 0.49, 0.659}; // https://www.hepdata.net/record/ins2778961
    twolow_202316 = {0.299, 0.166, 0.097, 0.085, 0.083, 0.084, 0.09, 0.09, 0.101, 0.104, 0.117, 0.125, 0.146, 0.164, 0.246, 0.354, 0.476};
    onelow_202316 = {0.35, 0.194, 0.114, 0.1, 0.097, 0.099, 0.106, 0.106, 0.119, 0.122, 0.138, 0.147, 0.171, 0.193, 0.29, 0.417, 0.561};
    exp_202316 = {0.418, 0.233, 0.137, 0.121, 0.117, 0.12, 0.128, 0.128, 0.143, 0.147, 0.166, 0.177, 0.207, 0.232, 0.349, 0.503, 0.677};
    onehigh_202316 = {0.502, 0.282, 0.167, 0.147, 0.142, 0.146, 0.156, 0.156, 0.174, 0.179, 0.202, 0.216, 0.253, 0.282, 0.425, 0.613, 0.827};
    twohigh_202316 = {0.594, 0.335, 0.2, 0.177, 0.171, 0.176, 0.188, 0.187, 0.21, 0.216, 0.244, 0.26, 0.305, 0.339, 0.511, 0.74, 0.998};
  }
  else if(channel=="EMu"){
    n_mass_202316 = 19;
    mass_202316 = {50,100,250,400,500,750,900,1000,1250,1500,1750,2000,2500,3000,5000,7500,10000,15000,20000};
    obs_202316_orig = {0.253, 0.147, 0.087, 0.076, 0.075, 0.076, 0.079, 0.081, 0.089, 0.099, 0.103, 0.112, 0.131, 0.145, 0.223, 0.324, 0.424, 0.636, 0.829}; // https://www.hepdata.net/record/ins2778961
    twolow_202316_orig = {0.198, 0.113, 0.065, 0.056, 0.055, 0.055, 0.057, 0.059, 0.065, 0.071, 0.075, 0.082, 0.095, 0.105, 0.161, 0.234, 0.305, 0.458, 0.597};
    onelow_202316_orig = {0.232, 0.133, 0.077, 0.066, 0.065, 0.065, 0.067, 0.069, 0.077, 0.084, 0.088, 0.096, 0.111, 0.123, 0.189, 0.275, 0.359, 0.538, 0.701};
    exp_202316_orig = {0.277, 0.159, 0.092, 0.079, 0.078, 0.078, 0.081, 0.083, 0.092, 0.101, 0.106, 0.115, 0.134, 0.148, 0.227, 0.33, 0.431, 0.648, 0.844};
    onehigh_202316_orig = {0.334, 0.192, 0.111, 0.096, 0.095, 0.095, 0.098, 0.101, 0.112, 0.123, 0.128, 0.139, 0.163, 0.18, 0.275, 0.4, 0.523, 0.788, 1.026};
    twohigh_202316_orig = {0.397, 0.229, 0.133, 0.115, 0.113, 0.114, 0.118, 0.121, 0.134, 0.147, 0.154, 0.167, 0.195, 0.215, 0.329, 0.479, 0.626, 0.947, 1.232};
    // convert EMu ATLAS to ours by /2.
    obs_202316 = {0.127, 0.073, 0.043, 0.038, 0.037, 0.038, 0.04, 0.041, 0.044, 0.05, 0.051, 0.056, 0.066, 0.072, 0.112, 0.162, 0.212, 0.318, 0.414};
    twolow_202316 = {0.099, 0.057, 0.033, 0.028, 0.028, 0.028, 0.029, 0.03, 0.033, 0.036, 0.037, 0.041, 0.048, 0.052, 0.081, 0.117, 0.152, 0.229, 0.298};
    onelow_202316 = {0.116, 0.067, 0.038, 0.033, 0.033, 0.033, 0.034, 0.035, 0.038, 0.042, 0.044, 0.048, 0.056, 0.061, 0.095, 0.138, 0.179, 0.269, 0.35};
    exp_202316 = {0.139, 0.08, 0.046, 0.04, 0.039, 0.039, 0.041, 0.042, 0.046, 0.051, 0.053, 0.058, 0.067, 0.074, 0.114, 0.165, 0.215, 0.324, 0.422};
    onehigh_202316 = {0.167, 0.096, 0.056, 0.048, 0.048, 0.048, 0.049, 0.051, 0.056, 0.061, 0.064, 0.07, 0.082, 0.09, 0.138, 0.2, 0.262, 0.394, 0.513};
    twohigh_202316 = {0.199, 0.115, 0.067, 0.057, 0.057, 0.057, 0.059, 0.06, 0.067, 0.074, 0.077, 0.084, 0.098, 0.107, 0.165, 0.239, 0.313, 0.474, 0.616};
  }

  TGraph *gr_202316_exp = new TGraph(n_mass_202316, &mass_202316[0], &exp_202316[0]);
  gr_202316_exp->SetLineWidth(3);
  gr_202316_exp->SetLineColor(kBlue-9);
  TGraph *gr_202316_obs = new TGraph(n_mass_202316, &mass_202316[0], &obs_202316[0]);
  gr_202316_obs->SetLineWidth(3);
  gr_202316_obs->SetLineColor(kBlue-9);




/*
  //==== 8 TeV overlay
  const int nm = 16;
  double mass_8TeV[nm] = {
    40, 50, 60, 70, 80,
    90, 100, 125, 150, 175,
    200, 250, 300, 350, 400, 500
  };
  double obs_8TeV[nm], exp_8TeV[nm];

  vector<double> tempvec_obs_8TeV, tempvec_exp_8TeV;
  if(channel=="EE"){
    tempvec_obs_8TeV = {
      0.000153022, 0.000158927, 0.000330048, 0.00240212, 0.00732196,
      0.0219599, 0.0109256, 0.00787703, 0.00797621, 0.0113701,
      0.0158563, 0.0417892, 0.08102, 0.139453, 0.237897, 0.722283,
    };
    tempvec_exp_8TeV = {
      0.000117471, 0.000119376, 0.00025116, 0.00185047, 0.00558829,
      0.0164049, 0.00765044, 0.0076666, 0.0100238, 0.0143538,
      0.0202162, 0.0414845, 0.0692952, 0.111788, 0.192697, 0.58519,
    };
  }
  if(channel=="MuMu"){
    tempvec_obs_8TeV = {
      2.09584e-05, 2.58344e-05, 6.25064e-05, 0.000393122, 0.0010261,
      0.00513292, 0.00240843, 0.00182402, 0.00346005, 0.00833979,
      0.0125927, 0.0205839, 0.0403864, 0.0754627, 0.151591, 0.526906, 
    };
    tempvec_exp_8TeV = {
      2.82635E-05, 3.69206E-05, 8.52678E-05, 0.000618594, 0.00148793,
      0.00583953, 0.00257802, 0.00290533, 0.00449771, 0.00722055,
      0.0108741, 0.0194395, 0.0400305, 0.0708397, 0.149737, 0.497138
    };
  }
  if(channel=="MuEl"){
    tempvec_obs_8TeV = {
      6.64339e-05, 7.96033e-05, 0.0001717, 0.00135852, 0.00335107,
      0.00860876, 0.00485913, 0.0048582, 0.00523195, 0.00763538,
      0.00929436, 0.0205983, 0.0367042, 0.0716093, 0.132311, 0.455058, 
    };
    tempvec_exp_8TeV = {
      6.66795e-05, 8.00154e-05, 0.000171178, 0.00135744, 0.00288853,
      0.00703366, 0.0034261, 0.00332376, 0.00448255, 0.00653659,
      0.00795478, 0.0162876, 0.0247572, 0.0466464, 0.0847838, 0.285325,
    };
  }

  for(unsigned int j=0; j<tempvec_obs_8TeV.size(); j++){
    obs_8TeV[j] = tempvec_obs_8TeV.at(j);
    exp_8TeV[j] = tempvec_exp_8TeV.at(j);
  }

  TGraph *gr_8TeV_exp = new TGraph(nm, mass_8TeV, obs_8TeV);
  gr_8TeV_exp->SetLineColor(kRed);
  //gr_8TeV_exp->SetLineStyle(10);
  gr_8TeV_exp->SetLineWidth(3);

  //==== 8 and 13 TeV Combined
  //==== 8 TeV overlay

  const int nm8and13 = 28;
  double mass_8and13TeV[nm8and13] = {
    20, 30, 40, 50, 60, 70, 80, 
    90, 100, 125, 150, 200, 
    250, 300, 400, 500,
    600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1700, 2000,
  };
  double obs_8and13TeV[nm8and13], exp_8and13TeV[nm8and13];

  vector<double> tempvec_obs_8and13TeV, tempvec_exp_8and13TeV;
  if(channel=="EE"){
    tempvec_obs_8and13TeV = {
      0.00010476, 8.244e-05, 8.16e-05, 0.00010183, 0.000225, 0.0016379, 0.0043771, 0.0083341, 0.0039405, 0.004687, 0.004088, 0.008342, 0.019262, 0.022621, 0.03489, 0.0912, 0.08728, 0.16916, 0.1243, 0.177, 0.4212, 0.6272, 0.8493, 1.1492, 0.8259, 1.0344, 2.169, 4.3526, 
    };
    tempvec_exp_8and13TeV = {

    };
  }
  if(channel=="MuMu"){
    tempvec_obs_8and13TeV = {
      3.783e-05, 2.8e-05, 1.553e-05, 1.775e-05, 4.376e-05, 0.0003474, 0.0008412, 0.0023593, 0.0015521, 0.001285, 0.002622, 0.004085, 0.005141, 0.00714, 0.03481, 0.03547, 0.06339, 0.11797, 0.0802, 0.204, 0.2644, 0.3766, 0.4724, 0.6004, 0.878, 1.0681, 1.0893, 2.1158,
    };
    tempvec_exp_8and13TeV = {

    };
  }
  if(channel=="MuEl"){
    tempvec_obs_8and13TeV = {
      4.2565e-05, 3.68e-05, 2.524e-05, 3.843e-05, 0.000102995, 0.0007555, 0.0019562, 0.00209275, 0.00159395, 0.001789, 0.002613, 0.0071055, 0.0083965, 0.0128815, 0.02217, 0.022245, 0.06913, 0.0654, 0.1086, 0.0763, 0.10995, 0.13505, 0.1744, 0.2324, 0.31395, 0.4463, 1.1073, 1.77645, 
    };
    tempvec_exp_8and13TeV = {

    };
  }

  for(unsigned int j=0; j<tempvec_obs_8and13TeV.size(); j++){
    obs_8and13TeV[j] = tempvec_obs_8and13TeV.at(j);
    //exp_8and13TeV[j] = tempvec_exp_8and13TeV.at(j);
  }

  TGraph *gr_8and13TeV_obs = new TGraph(nm8and13, mass_8and13TeV, obs_8and13TeV);
  gr_8and13TeV_obs->SetLineColor(kRed);
  gr_8and13TeV_obs->SetLineWidth(2);

//======================================
  //==== LEP (L3)

  float L3[2][38];
  float L3_2[2][224];
  float DEL[2][39];
  float CMS[2][8];
 
  L3[0][0]  = 20.75; L3[1][0]  = 1.53e-4;
  L3[0][1]  = 23.75; L3[1][1]  = 1.56e-4;
  L3[0][2]  = 26.37; L3[1][2]  = 1.59e-4;
  L3[0][3]  = 29.30; L3[1][3]  = 1.64e-4;
  L3[0][4]  = 32.54; L3[1][4]  = 1.69e-4;
  L3[0][5]  = 35.61; L3[1][5]  = 1.75e-4;
  L3[0][6]  = 39.55; L3[1][6]  = 1.84e-4;
  L3[0][7]  = 43.61; L3[1][7]  = 1.98e-4;
  L3[0][8]  = 48.45; L3[1][8]  = 2.20e-4;
  L3[0][9]  = 52.23; L3[1][9]  = 2.44e-4;
  L3[0][10] = 55.90; L3[1][10] = 2.76e-4;
  L3[0][11] = 59.82; L3[1][11] = 3.13e-4;
  L3[0][12] = 62.12; L3[1][12] = 3.50e-4;
  L3[0][13] = 64.03; L3[1][13] = 4.02e-4;
  L3[0][14] = 66.49; L3[1][14] = 4.56e-4;
  L3[0][15] = 69.05; L3[1][15] = 5.28e-4;
  L3[0][16] = 71.19; L3[1][16] = 6.49e-4;
  L3[0][17] = 72.28; L3[1][17] = 7.25e-4;
  L3[0][18] = 74.51; L3[1][18] = 8.78e-4;
  L3[0][19] = 76.24; L3[1][19] = 1.06e-3;
  L3[0][20] = 78.02; L3[1][20] = 1.35e-3;
  L3[0][21] = 79.20; L3[1][21] = 1.63e-3;
  L3[0][22] = 80.46; L3[1][22] = 2.03e-3;
  L3[0][23] = 81.71; L3[1][23] = 2.46e-3;
  L3[0][24] = 83.00; L3[1][24] = 3.23e-3;
  L3[0][25] = 84.30; L3[1][25] = 4.18e-3;
  L3[0][26] = 84.98; L3[1][26] = 5.49e-3;
  L3[0][27] = 85.67; L3[1][27] = 6.95e-3;
  L3[0][28] = 86.36; L3[1][28] = 8.80e-3;
  L3[0][29] = 87.40; L3[1][29] = 1.23e-2;
  L3[0][30] = 88.44; L3[1][30] = 1.62e-2;
  L3[0][31] = 89.17; L3[1][31] = 2.28e-2;
  L3[0][32] = 89.90; L3[1][32] = 3.03e-2;
  L3[0][33] = 90.20; L3[1][33] = 3.76e-2;
  L3[0][34] = 90.68; L3[1][34] = 4.75e-2;
  L3[0][35] = 90.72; L3[1][35] = 6.38e-2;
  L3[0][36] = 90.78; L3[1][36] = 8.69e-2;
  L3[0][37] = 90.81; L3[1][37] = 9.93e-2;

  L3_2[0][0] = 8.07360406091370e+001;   L3_2[1][0] =    1.64216866767426e-003;
  L3_2[0][1] = 8.47563451776650e+001;   L3_2[1][1] =   2.48527439290502e-003;
  L3_2[0][2] = 8.53654822335025e+001;   L3_2[1][2] =   2.73626821837528e-003;
  L3_2[0][3] = 8.58527918781726e+001;   L3_2[1][3] =   2.75651933220772e-003;
  L3_2[0][4] = 8.63401015228426e+001;   L3_2[1][4] =   2.73610368475080e-003;
  L3_2[0][5] = 8.68274111675127e+001;   L3_2[1][5] =   2.65618155791328e-003;
  L3_2[0][6] = 8.85329949238579e+001;   L3_2[1][6] =   2.29035801398706e-003;
  L3_2[0][7] = 8.91421319796954e+001;   L3_2[1][7] =   2.20703835870127e-003;
  L3_2[0][8] = 8.96294416243655e+001;   L3_2[1][8] =   2.23989516299269e-003;
  L3_2[0][9] = 9.02385786802030e+001;   L3_2[1][9] =   2.37648371479660e-003;
  L3_2[0][10] = 9.06040609137056e+001;   L3_2[1][10] =   2.41188122901001e-003;
  L3_2[0][11] = 9.10913705583756e+001;   L3_2[1][11] =   2.34142970388094e-003;
  L3_2[0][12] = 9.25532994923858e+001;   L3_2[1][12] =   2.04910464264994e-003;
  L3_2[0][13] = 9.43807106598985e+001;   L3_2[1][13] =   1.74090720011271e-003;
  L3_2[0][14] = 9.46243654822335e+001;   L3_2[1][14] =   1.74088102904615e-003;
  L3_2[0][15] = 9.49898477157360e+001;   L3_2[1][15] =   1.76681129761304e-003;
  L3_2[0][16] = 9.64517766497462e+001;   L3_2[1][16] =   1.90241532453007e-003;
  L3_2[0][17] = 9.69390862944162e+001;   L3_2[1][17] =   2.07911009477572e-003;
  L3_2[0][18] = 9.75482233502538e+001;   L3_2[1][18] =   2.34049711644839e-003;
  L3_2[0][19] = 9.81573604060914e+001;   L3_2[1][19] =   2.41075748993192e-003;
  L3_2[0][20] = 9.94974619289340e+001;   L3_2[1][20] =   2.39277676176929e-003;
  L3_2[0][21] = 1.00228426395939e+002;   L3_2[1][21] =   2.22191916533212e-003;
  L3_2[0][22] = 1.00472081218274e+002;   L3_2[1][22] =   2.18922731423832e-003;
  L3_2[0][23] = 1.00837563451777e+002;   L3_2[1][23] =   2.23834675845749e-003;
  L3_2[0][24] = 1.01446700507614e+002;   L3_2[1][24] =   2.46440276951126e-003;
  L3_2[0][25] = 1.02421319796954e+002;   L3_2[1][25] =   2.65362722269690e-003;
  L3_2[0][26] = 1.03274111675127e+002;   L3_2[1][26] =   2.73324649183704e-003;
  L3_2[0][27] = 1.04005076142132e+002;   L3_2[1][27] =   2.89989775412225e-003;
  L3_2[0][28] = 1.04614213197970e+002;   L3_2[1][28] =   3.00914790101252e-003;
  L3_2[0][29] = 1.05223350253807e+002;   L3_2[1][29] =   2.87829038910166e-003;
  L3_2[0][30] = 1.05710659898477e+002;   L3_2[1][30] =   2.75314411524667e-003;
  L3_2[0][31] = 1.06197969543147e+002;   L3_2[1][31] =   2.69258601376476e-003;
  L3_2[0][32] = 1.06563451776650e+002;   L3_2[1][32] =   2.57553340959042e-003;
  L3_2[0][33] = 1.06928934010152e+002;   L3_2[1][33] =   2.28775993140507e-003;
  L3_2[0][34] = 1.07172588832487e+002;   L3_2[1][34] =   1.94385725892692e-003;
  L3_2[0][35] = 1.07659898477157e+002;   L3_2[1][35] =   1.72665021784635e-003;
  L3_2[0][36] = 1.08390862944162e+002;   L3_2[1][36] =   1.75232900484380e-003;
  L3_2[0][37] = 1.08878172588832e+002;   L3_2[1][37] =   1.75227631969861e-003;
  L3_2[0][38] = 1.09365482233503e+002;   L3_2[1][38] =   1.70109198371827e-003;
  L3_2[0][39] = 1.09730964467005e+002;   L3_2[1][39] =   1.75218412450604e-003;
  L3_2[0][40] = 1.10096446700508e+002;   L3_2[1][40] =   1.87287526805698e-003;
  L3_2[0][41] = 1.10218274111675e+002;   L3_2[1][41] =   2.00190976058399e-003;
  L3_2[0][42] = 1.10705583756345e+002;   L3_2[1][42] =   2.13978602197581e-003;
  L3_2[0][43] = 1.11314720812183e+002;   L3_2[1][43] =   2.04673407369398e-003;
  L3_2[0][44] = 1.11680203045685e+002;   L3_2[1][44] =   2.04668792102224e-003;
  L3_2[0][45] = 1.12167512690355e+002;   L3_2[1][45] =   2.15549295363718e-003;
  L3_2[0][46] = 1.12654822335025e+002;   L3_2[1][46] =   2.30394685288646e-003;
  L3_2[0][47] = 1.13873096446701e+002;   L3_2[1][47] =   2.33814091031860e-003;
  L3_2[0][48] = 1.14604060913706e+002;   L3_2[1][48] =   2.42620930950398e-003;
  L3_2[0][49] = 1.14969543147208e+002;   L3_2[1][49] =   2.59332757747740e-003;
  L3_2[0][50] = 1.15456852791878e+002;   L3_2[1][50] =   2.79253523521921e-003;
  L3_2[0][51] = 1.15822335025381e+002;   L3_2[1][51] =   2.73113121610214e-003;
  L3_2[0][52] = 1.16187817258883e+002;   L3_2[1][52] =   2.59313265580647e-003;
  L3_2[0][53] = 1.16553299492386e+002;   L3_2[1][53] =   2.51740566300458e-003;
  L3_2[0][54] = 1.16918781725888e+002;   L3_2[1][54] =   2.63169779218229e-003;
  L3_2[0][55] = 1.17162436548223e+002;   L3_2[1][55] =   2.83389547768448e-003;
  L3_2[0][56] = 1.17406091370558e+002;   L3_2[1][56] =   3.12016774116052e-003;
  L3_2[0][57] = 1.18015228426396e+002;   L3_2[1][57] =   3.64490099465068e-003;
  L3_2[0][58] = 1.18502538071066e+002;   L3_2[1][58] =   4.25791310694239e-003;
  L3_2[0][59] = 1.19111675126904e+002;   L3_2[1][59] =   5.04818725777358e-003;
  L3_2[0][60] = 1.19355329949239e+002;   L3_2[1][60] =   5.47644464689688e-003;
  L3_2[0][61] = 1.19598984771574e+002;   L3_2[1][61] =   5.51705873718600e-003;
  L3_2[0][62] = 1.19964467005076e+002;   L3_2[1][62] =   5.31643666005577e-003;
  L3_2[0][63] = 1.20208121827411e+002;   L3_2[1][63] =   4.97364986461281e-003;
  L3_2[0][64] = 1.20451776649746e+002;   L3_2[1][64] =   4.61864240777426e-003;
  L3_2[0][65] = 1.21182741116751e+002;   L3_2[1][65] =   4.35282572742742e-003;
  L3_2[0][66] = 1.22035532994924e+002;   L3_2[1][66] =   4.22558354508754e-003;
  L3_2[0][67] = 1.23010152284264e+002;   L3_2[1][67] =   4.35233498102418e-003;
  L3_2[0][68] = 1.23375634517767e+002;   L3_2[1][68] =   4.54993429494731e-003;
  L3_2[0][69] = 1.24106598984772e+002;   L3_2[1][69] =   4.65191575277598e-003;
  L3_2[0][70] = 1.24715736040609e+002;   L3_2[1][70] =   4.54955811215536e-003;
  L3_2[0][71] = 1.25081218274112e+002;   L3_2[1][71] =   4.31967810281806e-003;
  L3_2[0][72] = 1.25568527918782e+002;   L3_2[1][72] =   4.16256623322266e-003;
  L3_2[0][73] = 1.26055837563452e+002;   L3_2[1][73] =   4.35151719329958e-003;
  L3_2[0][74] = 1.26421319796954e+002;   L3_2[1][74] =   4.58288489149382e-003;
  L3_2[0][75] = 1.26664974619289e+002;   L3_2[1][75] =   4.61687221589291e-003;
  L3_2[0][76] = 1.27030456852792e+002;   L3_2[1][76] =   4.44898448115588e-003;
  L3_2[0][77] = 1.27517766497462e+002;   L3_2[1][77] =   4.10088941611898e-003;
  L3_2[0][78] = 1.28005076142132e+002;   L3_2[1][78] =   3.58911345708994e-003;
  L3_2[0][79] = 1.28248730964467e+002;   L3_2[1][79] =   3.16459603062618e-003;
  L3_2[0][80] = 1.28614213197970e+002;   L3_2[1][80] =   2.87413949588912e-003;
  L3_2[0][81] = 1.29223350253807e+002;   L3_2[1][81] =   2.68876356203647e-003;
  L3_2[0][82] = 1.29954314720812e+002;   L3_2[1][82] =   2.66880961425097e-003;
  L3_2[0][83] = 1.30685279187817e+002;   L3_2[1][83] =   2.81064532612032e-003;
  L3_2[0][84] = 1.31294416243655e+002;   L3_2[1][84] =   3.07167266322708e-003;
  L3_2[0][85] = 1.32147208121827e+002;   L3_2[1][85] =   3.18734630632389e-003;
  L3_2[0][86] = 1.32634517766497e+002;   L3_2[1][86] =   3.18725047625861e-003;
  L3_2[0][87] = 1.33487309644670e+002;   L3_2[1][87] =   3.43200320482984e-003;
  L3_2[0][88] = 1.34218274111675e+002;   L3_2[1][88] =   3.80666076160848e-003;
  L3_2[0][89] = 1.34827411167513e+002;   L3_2[1][89] =   4.06880391065743e-003;
  L3_2[0][90] = 1.35680203045685e+002;   L3_2[1][90] =   4.28501106805965e-003;
  L3_2[0][91] = 1.37385786802030e+002;   L3_2[1][91] =   4.75250216740549e-003;
  L3_2[0][92] = 1.38238578680203e+002;   L3_2[1][92] =   5.97830309169873e-003;
  L3_2[0][93] = 1.38604060913706e+002;   L3_2[1][93] =   6.43757759797191e-003;
  L3_2[0][94] = 1.38969543147208e+002;   L3_2[1][94] =   6.29602409744375e-003;
  L3_2[0][95] = 1.39335025380711e+002;   L3_2[1][95] =   5.89003237089811e-003;
  L3_2[0][96] = 1.39822335025381e+002;   L3_2[1][96] =   5.67580483455332e-003;
  L3_2[0][97] = 1.40553299492386e+002;   L3_2[1][97] =   5.84614527893421e-003;
  L3_2[0][98] = 1.41162436548223e+002;   L3_2[1][98] =   6.15688894579588e-003;
  L3_2[0][99] = 1.41893401015228e+002;   L3_2[1][99] =   6.29488841340989e-003;
  L3_2[0][100] = 1.42868020304569e+002;   L3_2[1][100] =   6.15624107922065e-003;
  L3_2[0][101] = 1.43598984771574e+002;   L3_2[1][101] =   5.71665115562675e-003;
  L3_2[0][102] = 1.44208121827411e+002;   L3_2[1][102] =   4.96629052236900e-003;
  L3_2[0][103] = 1.44573604060914e+002;   L3_2[1][103] =   4.64604510518223e-003;
  L3_2[0][104] = 1.44939086294416e+002;   L3_2[1][104] =   4.92943451049419e-003;
  L3_2[0][105] = 1.45304568527919e+002;   L3_2[1][105] =   5.38731682636837e-003;
  L3_2[0][106] = 1.45548223350254e+002;   L3_2[1][106] =   5.71596368423831e-003;
  L3_2[0][107] = 1.46035532994924e+002;   L3_2[1][107] =   5.63177810597449e-003;
  L3_2[0][108] = 1.46522842639594e+002;   L3_2[1][108] =   5.34717550725343e-003;
  L3_2[0][109] = 1.47010152284264e+002;   L3_2[1][109] =   4.67986761483261e-003;
  L3_2[0][110] = 1.47984771573604e+002;   L3_2[1][110] =   4.03551318835173e-003;
  L3_2[0][111] = 1.48593908629442e+002;   L3_2[1][111] =   3.94671845719036e-003;
  L3_2[0][112] = 1.49446700507614e+002;   L3_2[1][112] =   4.06513555676476e-003;
  L3_2[0][113] = 1.50055837563452e+002;   L3_2[1][113] =   4.74878848299695e-003;
  L3_2[0][114] = 1.50543147208122e+002;   L3_2[1][114] =   5.42559761962259e-003;
  L3_2[0][115] = 1.51395939086294e+002;   L3_2[1][115] =   5.11329981946695e-003;
  L3_2[0][116] = 1.51883248730964e+002;   L3_2[1][116] =   5.03799036581785e-003;
  L3_2[0][117] = 1.52614213197970e+002;   L3_2[1][117] =   5.26660019352337e-003;
  L3_2[0][118] = 1.53345177664975e+002;   L3_2[1][118] =   5.34492520873051e-003;
  L3_2[0][119] = 1.53954314720812e+002;   L3_2[1][119] =   5.71299991078399e-003;
  L3_2[0][120] = 1.54563451776650e+002;   L3_2[1][120] =   6.10642183118031e-003;
  L3_2[0][121] = 1.55294416243655e+002;   L3_2[1][121] =   5.79774589462011e-003;
  L3_2[0][122] = 1.55659898477157e+002;   L3_2[1][122] =   5.75484922508963e-003;
  L3_2[0][123] = 1.56025380710660e+002;   L3_2[1][123] =   6.47844986010003e-003;
  L3_2[0][124] = 1.56512690355330e+002;   L3_2[1][124] =   7.56801806481790e-003;
  L3_2[0][125] = 1.56878172588832e+002;   L3_2[1][125] =   8.97278684252325e-003;
  L3_2[0][126] = 1.57487309644670e+002;   L3_2[1][126] =   1.19760322830161e-002;
  L3_2[0][127] = 1.58340101522843e+002;   L3_2[1][127] =   1.34814638831120e-002;
  L3_2[0][128] = 1.59192893401015e+002;   L3_2[1][128] =   1.33813141819938e-002;
  L3_2[0][129] = 1.59680203045685e+002;   L3_2[1][129] =   1.29904433625631e-002;
  L3_2[0][130] = 1.60289340101523e+002;   L3_2[1][130] =   1.28941351413757e-002;
  L3_2[0][131] = 1.60898477157360e+002;   L3_2[1][131] =   1.31832411980912e-002;
  L3_2[0][132] = 1.61507614213198e+002;   L3_2[1][132] =   1.35789944950263e-002;
  L3_2[0][133] = 1.62116751269036e+002;   L3_2[1][133] =   1.34783228883051e-002;
  L3_2[0][134] = 1.62604060913706e+002;   L3_2[1][134] =   1.32798121088354e-002;
  L3_2[0][135] = 1.63335025380711e+002;   L3_2[1][135] =   1.34773098202910e-002;
  L3_2[0][136] = 1.63822335025381e+002;   L3_2[1][136] =   1.50598853570680e-002;
  L3_2[0][137] = 1.64187817258883e+002;   L3_2[1][137] =   1.67042872576638e-002;
  L3_2[0][138] = 1.64675126903553e+002;   L3_2[1][138] =   1.79874348120820e-002;
  L3_2[0][139] = 1.65406091370558e+002;   L3_2[1][139] =   1.73329496803663e-002;
  L3_2[0][140] = 1.66258883248731e+002;   L3_2[1][140] =   1.59764340313924e-002;
  L3_2[0][141] = 1.66989847715736e+002;   L3_2[1][141] =   1.41910104138897e-002;
  L3_2[0][142] = 1.67964467005076e+002;   L3_2[1][142] =   1.16190463265593e-002;
  L3_2[0][143] = 1.68451776649746e+002;   L3_2[1][143] =   1.06309534546901e-002;
  L3_2[0][144] = 1.68695431472081e+002;   L3_2[1][144] =   1.05523759102246e-002;
  L3_2[0][145] = 1.69182741116751e+002;   L3_2[1][145] =   1.17045109798434e-002;
  L3_2[0][146] = 1.69791878172589e+002;   L3_2[1][146] =   1.25105343053991e-002;
  L3_2[0][147] = 1.70401015228426e+002;   L3_2[1][147] =   1.22352608820488e-002;
  L3_2[0][148] = 1.70888324873096e+002;   L3_2[1][148] =   1.29814632719994e-002;
  L3_2[0][149] = 1.71131979695431e+002;   L3_2[1][149] =   1.47224300504785e-002;
  L3_2[0][150] = 1.71497461928934e+002;   L3_2[1][150] =   1.73264366974011e-002;
  L3_2[0][151] = 1.71862944162437e+002;   L3_2[1][151] =   1.83832771887633e-002;
  L3_2[0][152] = 1.72593908629442e+002;   L3_2[1][152] =   2.02396963375339e-002;
  L3_2[0][153] = 1.73324873096447e+002;   L3_2[1][153] =   2.34689190852153e-002;
  L3_2[0][154] = 1.73690355329949e+002;   L3_2[1][154] =   2.38184865540433e-002;
  L3_2[0][155] = 1.74299492385787e+002;   L3_2[1][155] =   2.14724580737784e-002;
  L3_2[0][156] = 1.74543147208122e+002;   L3_2[1][156] =   2.11565265075367e-002;
  L3_2[0][157] = 1.74908629441624e+002;   L3_2[1][157] =   2.19539031349350e-002;
  L3_2[0][158] = 1.75274111675127e+002;   L3_2[1][158] =   2.31211796982749e-002;
  L3_2[0][159] = 1.75761421319797e+002;   L3_2[1][159] =   2.56456085290872e-002;
  L3_2[0][160] = 1.76126903553299e+002;   L3_2[1][160] =   2.76157953889048e-002;
  L3_2[0][161] = 1.76370558375635e+002;   L3_2[1][161] =   3.01811814592575e-002;
  L3_2[0][162] = 1.76736040609137e+002;   L3_2[1][162] =   3.08583531944697e-002;
  L3_2[0][163] = 1.77345177664975e+002;   L3_2[1][163] =   2.97357743944966e-002;
  L3_2[0][164] = 1.77588832487310e+002;   L3_2[1][164] =   2.97353273764529e-002;
  L3_2[0][165] = 1.78197969543147e+002;   L3_2[1][165] =   3.20192196315258e-002;
  L3_2[0][166] = 1.78441624365482e+002;   L3_2[1][166] =   3.49936644689640e-002;
  L3_2[0][167] = 1.78685279187817e+002;   L3_2[1][167] =   3.57790822726035e-002;
  L3_2[0][168] = 1.79050761421320e+002;   L3_2[1][168] =   3.57782754749544e-002;
  L3_2[0][169] = 1.79538071065990e+002;   L3_2[1][169] =   3.52513276186468e-002;
  L3_2[0][170] = 1.80390862944162e+002;   L3_2[1][170] =   3.52494728866610e-002;
  L3_2[0][171] = 1.80756345177665e+002;   L3_2[1][171] =   3.34683880084021e-002;
  L3_2[0][172] = 1.81243654822335e+002;   L3_2[1][172] =   2.99495448280725e-002;
  L3_2[0][173] = 1.81609137055838e+002;   L3_2[1][173] =   2.86475725983867e-002;
  L3_2[0][174] = 1.82218274111675e+002;   L3_2[1][174] =   3.20112783628315e-002;
  L3_2[0][175] = 1.82705583756345e+002;   L3_2[1][175] =   3.39635777579971e-002;
  L3_2[0][176] = 1.82949238578680e+002;   L3_2[1][176] =   3.42154564680990e-002;
  L3_2[0][177] = 1.83436548223350e+002;   L3_2[1][177] =   3.37115260466601e-002;
  L3_2[0][178] = 1.84411167512690e+002;   L3_2[1][178] =   3.17708493064151e-002;
  L3_2[0][179] = 1.85873096446701e+002;   L3_2[1][179] =   2.90672832608436e-002;
  L3_2[0][180] = 1.86360406091371e+002;   L3_2[1][180] =   2.82182221991073e-002;
  L3_2[0][181] = 1.86604060913706e+002;   L3_2[1][181] =   2.82177979944333e-002;
  L3_2[0][182] = 1.86969543147208e+002;   L3_2[1][182] =   3.08388756590325e-002;
  L3_2[0][183] = 1.87335025380711e+002;   L3_2[1][183] =   3.37034183922107e-002;
  L3_2[0][184] = 1.87578680203046e+002;   L3_2[1][184] =   3.57594553603469e-002;
  L3_2[0][185] = 1.87944162436548e+002;   L3_2[1][185] =   3.57586490052733e-002;
  L3_2[0][186] = 1.88553299492386e+002;   L3_2[1][186] =   3.49718396264112e-002;
  L3_2[0][187] = 1.88796954314721e+002;   L3_2[1][187] =   3.57567675808119e-002;
  L3_2[0][188] = 1.89406091370558e+002;   L3_2[1][188] =   4.11561912958174e-002;
  L3_2[0][189] = 1.89771573604061e+002;   L3_2[1][189] =   4.23923137043059e-002;
  L3_2[0][190] = 1.90380710659898e+002;   L3_2[1][190] =   4.20780266387354e-002;
  L3_2[0][191] = 1.90624365482233e+002;   L3_2[1][191] =   4.30224499155085e-002;
  L3_2[0][192] = 1.91355329949239e+002;   L3_2[1][192] =   4.98866375797527e-002;
  L3_2[0][193] = 1.91842639593909e+002;   L3_2[1][193] =   5.13845919001389e-002;
  L3_2[0][194] = 1.92208121827411e+002;   L3_2[1][194] =   4.98840128178321e-002;
  L3_2[0][195] = 1.93426395939086e+002;   L3_2[1][195] =   4.39811274985747e-002;
  L3_2[0][196] = 1.93791878172589e+002;   L3_2[1][196] =   4.39801357495409e-002;
  L3_2[0][197] = 1.94644670050761e+002;   L3_2[1][197] =   5.37094165943240e-002;
  L3_2[0][198] = 1.95375634517767e+002;   L3_2[1][198] =   7.71946351078141e-002;
  L3_2[0][199] = 1.95741116751269e+002;   L3_2[1][199] =   8.56235840458290e-002;
  L3_2[0][200] = 1.96228426395939e+002;   L3_2[1][200] =   8.75440526286489e-002;
  L3_2[0][201] = 1.96715736040609e+002;   L3_2[1][201] =   9.01727523811539e-002;
  L3_2[0][202] = 1.97203045685279e+002;   L3_2[1][202] =   9.78209876472586e-002;
  L3_2[0][203] = 1.98177664974619e+002;   L3_2[1][203] =   1.14269425493516e-001;
  L3_2[0][204] = 1.98664974619289e+002;   L3_2[1][204] =   1.32502986799492e-001;
  L3_2[0][205] = 1.99274111675127e+002;   L3_2[1][205] =   1.59439233584269e-001;
  L3_2[0][206] = 1.99639593908629e+002;   L3_2[1][206] =   1.86255677804246e-001;
  L3_2[0][207] = 2.00248730964467e+002;   L3_2[1][207] =   2.30855798324400e-001;
  L3_2[0][208] = 2.00614213197970e+002;   L3_2[1][208] =   2.61814305867800e-001;
  L3_2[0][209] = 2.00857868020305e+002;   L3_2[1][209] =   2.73702931509858e-001;
  L3_2[0][210] = 2.01223350253807e+002;   L3_2[1][210] =   2.77779712405957e-001;
  L3_2[0][211] = 2.01588832487310e+002;   L3_2[1][211] =   2.86122800440038e-001;
  L3_2[0][212] = 2.01832487309645e+002;   L3_2[1][212] =   3.24495230396064e-001;
  L3_2[0][213] = 2.01954314720812e+002;   L3_2[1][213] =   3.46851805922331e-001;
  L3_2[0][214] = 2.02197969543147e+002;   L3_2[1][214] =   3.96291944782074e-001;
  L3_2[0][215] = 2.02441624365482e+002;   L3_2[1][215] =   4.59533733493136e-001;
  L3_2[0][216] = 2.02563451776650e+002;   L3_2[1][216] =   5.13506144309501e-001;
  L3_2[0][217] = 2.02685279187817e+002;   L3_2[1][217] =   5.48884904374619e-001;
  L3_2[0][218] = 2.02807106598985e+002;   L3_2[1][218] =   5.95453424610927e-001;
  L3_2[0][219] = 2.03050761421320e+002;   L3_2[1][219] =   7.00778579465140e-001;
  L3_2[0][220] = 2.03172588832487e+002;   L3_2[1][220] =   8.12617621404863e-001;
  L3_2[0][221] = 2.03294416243655e+002;   L3_2[1][221] =   8.81561765878811e-001;
  L3_2[0][222] = 2.03416243654822e+002;   L3_2[1][222] =   8.94706000334829e-001;
  L3_2[0][223] = 2.03416243654822e+002; L3_2[1][223] = 9.70621986828742e-001;

  DEL[0][0] = 10.1; DEL[1][0]   = 1.79e-5;
  DEL[0][1] = 11.2; DEL[1][1]   = 1.82e-5;
  DEL[0][2] = 12.7; DEL[1][2]   = 1.85e-5;
  DEL[0][3] = 15.1; DEL[1][3]   = 1.91e-5;
  DEL[0][4] = 17.8; DEL[1][4]   = 2.01e-5;
  DEL[0][5] = 19.8; DEL[1][5]   = 2.08e-5;
  DEL[0][6] = 21.8; DEL[1][6]   = 2.19e-5;
  DEL[0][7] = 23.5; DEL[1][7]   = 2.26e-5;
  DEL[0][8] = 25.5; DEL[1][8]   = 2.30e-5;
  DEL[0][9] = 27.4; DEL[1][9]   = 2.34e-5;
  DEL[0][10] = 29.6; DEL[1][10] = 2.37e-5;
  DEL[0][11] = 33.0; DEL[1][11] = 2.37e-5;
  DEL[0][12] = 36.0; DEL[1][12] = 2.41e-5;
  DEL[0][13] = 40.3; DEL[1][13] = 2.45e-5;
  DEL[0][14] = 44.5; DEL[1][14] = 2.53e-5;
  DEL[0][15] = 47.2; DEL[1][15] = 2.76e-5;
  DEL[0][16] = 50.7; DEL[1][16] = 3.05e-5;
  DEL[0][17] = 53.6; DEL[1][17] = 3.49e-5;
  DEL[0][18] = 56.0; DEL[1][18] = 4.07e-5;
  DEL[0][19] = 57.8; DEL[1][19] = 4.58e-5;
  DEL[0][20] = 59.4; DEL[1][20] = 5.25e-5;
  DEL[0][21] = 61.1; DEL[1][21] = 6.12e-5;
  DEL[0][22] = 62.8; DEL[1][22] = 7.25e-5;
  DEL[0][23] = 64.1; DEL[1][23] = 8.60e-5;
  DEL[0][24] = 65.6; DEL[1][24] = 1.00e-4;
  DEL[0][25] = 66.7; DEL[1][25] = 1.19e-4;
  DEL[0][26] = 67.8; DEL[1][26] = 1.41e-4;
  DEL[0][27] = 69.6; DEL[1][27] = 1.85e-4;
  DEL[0][28] = 70.8; DEL[1][28] = 2.30e-4;
  DEL[0][29] = 72.0; DEL[1][29] = 3.03e-4;
  DEL[0][30] = 73.2; DEL[1][30] = 3.84e-4;
  DEL[0][31] = 74.0; DEL[1][31] = 4.63e-4;
  DEL[0][32] = 75.2; DEL[1][32] = 5.77e-4;
  DEL[0][33] = 76.5; DEL[1][33] = 7.58e-4;
  DEL[0][34] = 76.9; DEL[1][34] = 8.98e-4;
  DEL[0][35] = 77.8; DEL[1][35] = 1.12e-3;
  DEL[0][36] = 78.2; DEL[1][36] = 1.33e-3;
  DEL[0][37] = 79.1; DEL[1][37] = 1.66e-3;
  DEL[0][38] = 79.9; DEL[1][38] = 2.03e-3;


  double massPointsAtlas_MuMu[25] = {100.237932 ,110.246164 ,119.409396 ,140.781448 ,160.210482 ,179.085100 ,199.904443 ,221.280479 ,239.880451 ,257.926575 ,282.358730 ,298.739614 ,324.006808 ,345.941813 ,365.100470 ,386.202713 ,409.248828 ,427.297514 ,442.847060 ,457.563558 ,471.447010 ,483.942430 ,491.994837 ,499.769752 ,499.769752};
  double limitsAtlas_MuMu[25] = {0.003474 ,0.002831 ,0.003084 ,0.004687 ,0.006876 ,0.009806 ,0.013886 ,0.019115 ,0.025217 ,0.031881 ,0.043279 ,0.052810 ,0.068711 ,0.086890 ,0.106042 ,0.132205 ,0.167192 ,0.198336 ,0.230308 ,0.265536 ,0.303980 ,0.340654 ,0.368393 ,0.395574 ,0.395574};

  double massPointsAtlas_EE[20] = {1.00455861406709e+002, 1.09809766214851e+002, 1.38752503310975e+002, 1.78574156906282e+002, 1.98797339797022e+002, 2.20886448743258e+002, 2.38308779169597e+002, 2.57287535385483e+002, 2.79378072245287e+002, 3.02092250356086e+002, 3.21384433600234e+002, 3.43787683531516e+002, 3.63702080113091e+002, 3.82682978199330e+002, 3.99174308979077e+002, 4.19400704675346e+002, 4.35581821232361e+002, 4.49584298662402e+002, 4.64520988544563e+002, 4.80079534785759e+002};
  double limitsAtlas_EE[20] = {4.04226688202133e-002, 2.87321017020558e-002, 3.33225809095133e-002, 5.51508368004247e-002, 6.92830498039127e-002, 9.05397993095326e-002, 1.11977313431971e-001, 1.38473609575320e-001, 1.75314576403352e-001, 2.16733719311297e-001, 2.55570350392377e-001, 3.13465143657095e-001, 3.72556547861380e-001, 4.39325988221587e-001, 5.10019175442642e-001, 5.96620239359251e-001, 6.76374403247621e-001, 7.60870825023857e-001, 8.49106552700366e-001, 9.62659703648251e-001};

  TGraph *gr_L3Limit = new TGraph(38, L3[0], L3[1]);
  gr_L3Limit->SetLineWidth(2);
  //gr_L3Limit->SetLineStyle(3);
  gr_L3Limit->SetLineColor(kViolet+1);

  TGraph *gr_L3_2Limit = new TGraph(224, L3_2[0], L3_2[1]);
  gr_L3_2Limit->SetLineWidth(2);
  //gr_L3_2Limit->SetLineStyle(3);
  gr_L3_2Limit->SetLineColor(kViolet);

  TGraph *gr_DELPHILimit = new TGraph(39, DEL[0], DEL[1]);
  gr_DELPHILimit->SetLineWidth(2);
  //gr_DELPHILimit->SetLineStyle(3);
  gr_DELPHILimit->SetLineColor(kMagenta);

  TGraph *gr_ATLAS_MuMu = new TGraph(25, massPointsAtlas_MuMu, limitsAtlas_MuMu);
  gr_ATLAS_MuMu->SetLineWidth(3);
  //gr_ATLAS_MuMu->SetLineStyle(3);
  gr_ATLAS_MuMu->SetLineColor(kBlue);

  TGraph *gr_ATLAS_EE = new TGraph(20, massPointsAtlas_EE, limitsAtlas_EE);
  gr_ATLAS_EE->SetLineWidth(3);
  //gr_ATLAS_EE->SetLineStyle(3);
  gr_ATLAS_EE->SetLineColor(kBlue);

//=======================================
*/

//=======================================

  //==== Double beta decay
  double allxrange[2]= {0.,999999.};
  double dbeta_ee[2];
  for(int i=0; i<2; i++){
    dbeta_ee[i] = 5e-8*allxrange[i];
  }
  TGraph *gr_dbeta = new TGraph(2, allxrange, dbeta_ee);
  gr_dbeta->SetLineColor(kViolet);

  //==== EWPD
  double EWPD_ee[2], EWPD_mm[2];
  for(int i=0; i<2; i++){
    //==== https://journals.aps.org/prd/pdf/10.1103/PhysRevD.78.013010
    //==== 90% CL
    EWPD_ee[i] = 0.003;
    EWPD_mm[i] = 0.003;
    //==== https://www.epj-conferences.org/articles/epjconf/pdf/2013/21/epjconf_lhcp2013_19008.pdf
    //==== 95% CL
    EWPD_ee[i] = 0.041*0.041;
    EWPD_mm[i] = 0.030*0.030;
  }
  TGraph *gr_EWPD_ee = new TGraph(2, allxrange, EWPD_ee);
  TGraph *gr_EWPD_mm = new TGraph(2, allxrange, EWPD_mm);
  gr_EWPD_ee->SetLineColor(kCyan);
  gr_EWPD_mm->SetLineColor(kCyan);
  gr_EWPD_ee->SetLineStyle(2);
  gr_EWPD_mm->SetLineStyle(2);
  gr_EWPD_ee->SetLineWidth(3);
  gr_EWPD_mm->SetLineWidth(3);


  //======================
  //==== Dilepton full Run2 limit
  //======================

  //=== Legend
  cout << "Drawing Dilepton "+year+" limit ..." << endl;
  TLegend *lg = 0;
  if(IsXsecLimit){
    if(CompareLimits) lg = new TLegend(0.5, 0.45, 0.94, 0.8);
    else lg = new TLegend(0.5, 0.55, 0.94, 0.8);
  }
  else if(CompareLimits){
    if(Logy) lg = new TLegend(0.48, 0.1, 0.9, 0.55);
    else lg = new TLegend(0.18, 0.3, 0.6, 0.75);
  }
  else lg = new TLegend(0.48, 0.2, 0.9, 0.55);
  lg->SetBorderSize(0);
  lg->SetFillStyle(0);
  //TH1D *hist_emptylegend = new TH1D("hist_emptylegend","",1,0.,1.);
  //hist_emptylegend->SetLineColor(0);

  lg->SetTextSize(0.03);
  lg->SetEntrySeparation(0.02); 
  if(DrawObserved) lg->AddEntry(gr_obs_0,"Observed", "l");
  lg->AddEntry(gr_exp_0,"Expected", "l");
  if(!SepLimit) lg->AddEntry(gr_band_1sigma_0,"68% expected", "f");
  if(!SepLimit) lg->AddEntry(gr_band_2sigma_0,"95% expected", "f");
  //lg->AddEntry(hist_emptylegend,"","l");
  if(AddPub){
    TLegendEntry* e1 = lg->AddEntry(gr_17028_exp, "#splitline{CMS 2016 DY+W#gamma SS2l}{#it{JHEP} 01 (2019) 122 (exp)}", "l"); // EXO-17-028
    if(CompareLimits) e1->SetTextSize(0.028);
    else e1->SetTextSize(0.025);
  }
  if(!DrawExt){
    for (size_t i = 0; i < gr_exp_list.size(); ++i) {
      if (gr_exp_list[i]) lg->AddEntry(gr_exp_list[i], descrps[i], "l");
    }
  }

  //TLegend *lg_Alt = new TLegend(0.65, 0.15, 0.93, 0.48);
  //lg_Alt->SetBorderSize(0);
  //lg_Alt->SetFillStyle(0);
  //if(!IsXsecLimit){
  //  lg_Alt->AddEntry(gr_17028_exp, "EXO-17-028 2016 (exp)", "l"); // EXO-17-028
  //  if(CompareLimits){
  //    for (size_t i = 0; i < gr_exp_list.size(); ++i) {
  //      if (gr_exp_list[i]) lg_Alt->AddEntry(gr_exp_list[i], descrps[i], "l");
  //    }
  //  }
  //}
  
  if(channel=="MuMu"){
    //lg_Alt->AddEntry(gr_DELPHILimit, "DELPHI prompt", "l");
    //lg_Alt->AddEntry(gr_L3Limit, "L3", "l");
    //lg_Alt->AddEntry(gr_EWPD_mm, "EWPD (90% CL)", "l");
    //lg_Alt->AddEntry(gr_ATLAS_MuMu, "ATLAS 8 TeV", "l");
    //lg_Alt->AddEntry(gr_17028_obs, "CMS 13 TeV dilepton", "l");
    //lg_Alt->AddEntry(gr_trilepLimit, "CMS 13 TeV trilepton", "l");
    //lg_Alt->AddEntry(gr_21003_obs, "CMS 13 TeV SSWW", "l");
    //lg_Alt->AddEntry(gr_EWPD_mm, "EWPD", "l");
    //if(!IsXsecLimit) lg_Alt->AddEntry(gr_21003_exp, "EXO-21-003 Run2 (exp)", "l"); // EXO-21-003
    if(!IsXsecLimit){
      if(AddPub){
        TLegendEntry* e1 = lg->AddEntry(gr_21003_exp, "#splitline{CMS Run2 SSWW SS2l}{#it{PRL} 131 (2023) 011803 (exp)}", "l"); // EXO-21-003
        TLegendEntry* e2 = lg->AddEntry(gr_202006_exp, "#splitline{ATLAS Run2 SSWW SS2l}{#it{Eur. Phys. J. C} 83 (2023) 824 (exp)}", "l"); // EXOT-2020-06
        if(CompareLimits){
          e1->SetTextSize(0.028);
          e2->SetTextSize(0.028);
        }
        else{
          e1->SetTextSize(0.025);
          e2->SetTextSize(0.023);
        }
      }
    }
    else{
      if(AddPub){
        TLegendEntry* e1 = lg->AddEntry(gr_21003_exp, "#splitline{CMS Run2 SSWW SS2l}{#it{PRL} 131 (2023) 011803 (exp)}", "l"); // EXO-21-003
        if(CompareLimits){
          e1->SetTextSize(0.028);
        }
        else{
          e1->SetTextSize(0.025);
        }
      }
    }
  }
  if(channel=="EE"){
    //lg_Alt->AddEntry(gr_DELPHILimit, "DELPHI prompt", "l");
    //lg_Alt->AddEntry(gr_L3_2Limit, "L3", "l");
    //lg_Alt->AddEntry(gr_EWPD_ee, "EWPD (90% CL)", "l");
    //lg_Alt->AddEntry(gr_ATLAS_EE, "ATLAS 8 TeV", "l");
    //lg_Alt->AddEntry(gr_17028_exp, "CMS 13 TeV dilepton", "l");
    //lg_Alt->AddEntry(gr_dbeta, "Neutrino-less double beta dacay", "l");
    //lg_Alt->AddEntry(gr_17028_obs, "CMS 13 TeV dilepton", "l");
    //lg_Alt->AddEntry(gr_trilepLimit, "CMS 13 TeV trilepton", "l");
    //lg_Alt->AddEntry(gr_EWPD_ee, "EWPD", "l");
    if(!IsXsecLimit){
      if(AddPub){
        TLegendEntry* e1 = lg->AddEntry(gr_202316_exp, "#splitline{ATLAS Run2 SSWW SS2l}{#it{Phys. Lett. B} 856 (2024) 138865 (exp)}", "l"); // EXOT-2023-16
        if(CompareLimits) e1->SetTextSize(0.028);
        else e1->SetTextSize(0.022);
      }
    }
  }
  if(channel=="EMu"){
    //lg_Alt->AddEntry(gr_8TeV_exp, "CMS 8 TeV", "l");
    //lg_Alt->AddEntry(hist_emptylegend,"#color[0]{CMS 13 TeV trilepton}","l");
    //lg_Alt->AddEntry(gr_17028_exp, "CMS 13 TeV dilepton 2016 (exp)", "l");
    //lg_Alt->AddEntry(gr_17028_obs, "CMS 13 TeV dilepton", "l");
    if(!IsXsecLimit){
      if(AddPub){
        TLegendEntry* e1 = lg->AddEntry(gr_202316_exp, "#splitline{ATLAS Run2 SSWW SS2l}{#it{Phys. Lett. B} 856 (2024) 138865 (exp)}", "l"); // EXOT-2023-16
        if(CompareLimits) e1->SetTextSize(0.028);
        else e1->SetTextSize(0.022);
      }
    }

    //==== EMu mixing theoretical limit
    double EMu_ceil[2];
    for(int i=0; i<2; i++){
      EMu_ceil[i] = 0.25;
    }
    TGraph *gr_EMu_ceil = new TGraph(2, allxrange, EMu_ceil);
    gr_EMu_ceil->SetLineColor(kCyan);
    gr_EMu_ceil->SetLineStyle(2);
    gr_EMu_ceil->SetLineWidth(3);
  }

  TCanvas *c_Dilep = new TCanvas("c_Dilep", "", 1000, 1000);
  canvas_margin(c_Dilep);

  //==== PADs : draw limit (up) / ratio (down)
  TPad *c_up = new TPad("c_up", "", 0, 0.25, 1, 1);
  TPad *c_down = new TPad("c_down", "", 0, 0, 1, 0.25);
  if(CompareLimits){
    c_up->SetTopMargin(0.08);
    c_up->SetBottomMargin(0.017);
    c_up->SetLeftMargin(0.14);
    c_up->SetRightMargin(0.04);
    c_up->SetLogx();
    if(Logy) c_up->SetLogy();
    c_up->Draw();
    c_up->cd();
  }
  else{
    c_Dilep->cd();
    c_Dilep->Draw();
    c_Dilep->SetLogx();
    if(Logy) c_Dilep->SetLogy();
  }

  TH1D *dummy = new TH1D("hist", "", 100000, 0., 100000.);
  hist_axis(dummy);
  if(CompareLimits){
    dummy->GetYaxis()->SetTitleSize(0.06);
    dummy->GetYaxis()->SetLabelSize(0.05);
  }
  else{
    dummy->GetXaxis()->SetTitleOffset(0.9);
    dummy->GetXaxis()->SetLabelOffset(0.001);
    dummy->GetXaxis()->SetLabelSize(0.04);
    dummy->GetYaxis()->SetTitleSize(0.05);
    dummy->GetYaxis()->SetLabelSize(0.04);
  }
  if(channel=="EE") dummy->GetYaxis()->SetTitle("#||{V_{eN}}^{2}");
  if(channel=="MuMu") dummy->GetYaxis()->SetTitle("#||{V_{#muN}}^{2}");
  if(channel=="EMu"){
    dummy->GetYaxis()->SetTitle("#scale[0.8]{#frac{#||{ V_{eN}V_{#muN}^{*}}^{2}}{#||{ V_{eN} }^{2} + #||{ V_{#muN} }^{2}}}");
    dummy->GetYaxis()->SetTitleOffset(1.4);
    dummy->GetYaxis()->SetTitleSize(0.04);
    dummy->GetYaxis()->SetLabelSize(0.04);
    if(!CompareLimits){
      dummy->GetYaxis()->SetTitleOffset(1.6);
      dummy->GetYaxis()->SetTitleSize(0.04);
      dummy->GetYaxis()->SetLabelSize(0.03);
    }
  }
  if(IsXsecLimit){
    dummy->GetYaxis()->SetTitle("#sigma(pp #rightarrow l^{#pm}l'^{#pm}jj) (pb)");
    dummy->GetYaxis()->SetTitleSize(0.05);
    dummy->GetYaxis()->SetTitleOffset(1.4);
    dummy->GetYaxis()->SetLabelSize(0.04);
    dummy->GetXaxis()->SetLabelSize(0.04);
  }
  dummy->GetXaxis()->SetTitle("m_{N} (GeV)");
  if(CompareLimits) dummy->GetXaxis()->SetLabelSize(0);
  if(channel=="EMu") dummy->GetXaxis()->SetRangeUser(80., 60000); //FIXME
  else dummy->GetXaxis()->SetRangeUser(80., 30000); //FIXME
  //if(tag_nom.Contains("DYVBF")) dummy->GetXaxis()->SetRangeUser(80., 3000);
  //else if(tag_nom.Contains("SSWW")) dummy->GetXaxis()->SetRangeUser(400., 25000);
  //else dummy->GetXaxis()->SetRangeUser(80., 25000); //FIXME
  if(IsXsecLimit) dummy->GetYaxis()->SetRangeUser(1e-5, 0.1); //FIXME
  else dummy->GetYaxis()->SetRangeUser(5e-5, 1.); //FIXME
  //dummy->GetXaxis()->SetRangeUser(1000, 30000); //FIXME
  dummy->SetTitle("");
  dummy->Draw("hist");

  // Now draw limits
  if(!SepLimit) gr_band_2sigma_0->Draw("3same");
  if(!SepLimit) gr_band_1sigma_0->Draw("3same");
  gr_exp_0->Draw("lsame");
  if(WP_noms.size()>1){
    if(!SepLimit) gr_band_2sigma_1->Draw("3same");
    if(!SepLimit) gr_band_1sigma_1->Draw("3same");
    gr_exp_1->Draw("lsame");
  }
  if(WP_noms.size()>2){
    if(!SepLimit) gr_band_2sigma_2->Draw("3same");
    if(!SepLimit) gr_band_1sigma_2->Draw("3same");
    gr_exp_2->Draw("lsame");
  }
  if(AddPub) gr_17028_exp->Draw("lsame"); // EXO-17-028
  for (size_t i = 0; i < gr_exp_list.size(); ++i) {
    if (gr_exp_list[i]) gr_exp_list[i]->Draw("lsame");
  }
  //gr_17028_obs->Draw("lsame");
  //gr_8and13TeV_obs->Draw("lsame");
  if(channel=="MuMu"){
    //gr_L3Limit->Draw("lsame");
    //gr_DELPHILimit->Draw("lsame");
    //gr_21003_obs->Draw("lsame");
    //gr_trilepLimit->Draw("lsame");
    //gr_EWPD_mm->Draw("lsame");
    //gr_ATLAS_MuMu->Draw("lsame");
    if(AddPub){
      gr_21003_exp->Draw("lsame"); // EXO-21-003
      if(!IsXsecLimit) gr_202006_exp->Draw("lsame"); // EXOT-2020-06
    }
  }
  else if(channel=="EE"){
    //gr_L3_2Limit->Draw("lsame");
    //gr_DELPHILimit->Draw("lsame");
    //gr_trilepLimit->Draw("lsame");
    //gr_EWPD_ee->Draw("lsame");
    //gr_dbeta->Draw("lsame");
    //gr_ATLAS_EE->Draw("lsame");
    if(!IsXsecLimit){
      if(AddPub) gr_202316_exp->Draw("lsame"); // EXOT-2023-16
    }
  }
  else if(channel=="EMu"){
    if(!IsXsecLimit){
      if(AddPub) gr_202316_exp->Draw("lsame"); // EXOT-2023-16
    }
  }

  if(DrawObserved){
    gr_obs_0->Draw("lsame");
    if(WP_noms.size()>1){
      gr_obs_1->Draw("lsame");
    }
    if(WP_noms.size()>2){
      gr_obs_2->Draw("lsame");
    }
  }

  lg->Draw();
  //lg_Alt->Draw();
  
  TLatex latex_CMSPreliminary, latex_Lumi, latex_title;
  latex_CMSPreliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_title.SetNDC();

  latex_Lumi.SetTextSize(0.035);
  latex_Lumi.SetTextFont(42);
  TString lumi;
  if(year=="2016") lumi = "36.5";
  else if(year=="2016preVFP") lumi = "19.5";
  else if(year=="2016postVFP") lumi = "16.8";
  else if(year=="2017") lumi = "41.5";
  else if(year=="2018") lumi = "59.8";
  else if(year=="Run2") lumi = "137.6";
  if(tag_nom.Contains("Run2")) lumi = "137.6";
  if(tag_nom.Contains("Run23")) lumi = "440";

  latex_title.SetTextSize(0.04);
  latex_title.SetLineWidth(2);
  if(CompareLimits){
    latex_CMSPreliminary.DrawLatex(0.14, 0.93, "#scale[0.8]{CMS #bf{#it{Preliminary}}}");
    latex_Lumi.DrawLatex(0.76, 0.93, lumi+" fb^{-1} (13 TeV)");
    latex_title.DrawLatex(0.19, 0.83, "#scale[1.2]{#font[62]{CMS}}");
    latex_title.DrawLatex(0.19, 0.79, "#scale[1.0]{#font[41]{95% CL upper limit}}");
  }
  else{
    latex_CMSPreliminary.DrawLatex(0.16, 0.96, "#scale[0.8]{CMS #bf{#it{Preliminary}}}");
    if(year.Contains("Run2")||tag_nom.Contains("Run2")) latex_Lumi.DrawLatex(0.69, 0.96, lumi+" fb^{-1} (13 TeV)"); // Run2
    else latex_Lumi.DrawLatex(0.736, 0.96, lumi+" fb^{-1} (13 TeV)");
    latex_title.DrawLatex(0.21, 0.88, "#font[62]{CMS}");
    latex_title.DrawLatex(0.21, 0.84, "#font[41]{95% CL upper limit}");
    if(IsXsecLimit){
      if(channel=="MuMu") latex_title.DrawLatex(0.21, 0.80, "#font[62]{#mu#mu}");
      if(channel=="EE")   latex_title.DrawLatex(0.21, 0.80, "#font[62]{ee}");
      if(channel=="EMu")  latex_title.DrawLatex(0.21, 0.80, "#font[62]{e#mu}");
    }
    //latex_title.SetTextSize(0.05);
    //latex_title.DrawLatex(0.25, 0.88, "#font[62]{CMS}");
  }
  //if(tag_nom.Contains("Run23")) latex_Lumi.DrawLatex(0.734, 0.96, lumi+" fb^{-1} (13.6 TeV)");

  dummy->Draw("axissame");

  if(CompareLimits){
    c_Dilep->cd();
    c_down->SetTopMargin(0.03);
    c_down->SetBottomMargin(0.2);
    c_down->SetLeftMargin(0.14);
    c_down->SetRightMargin(0.04);
    c_down->SetLogx();
    if(Logy) c_down->SetLogy();
    c_down->SetGridx();
    c_down->SetGridy();
    c_down->Draw();
    c_down->cd();

    // ratio with different conditions
    vector<vector<double>> ratio_vs_nominal;
    vector<vector<double>> mass_vs_nominal;
    
    vector<double> mass_nominal = masses[0];
    vector<double> limit_nominal = limits[0];
    
    for (size_t i = 1; i < limits.size(); ++i) {
      vector<double> this_mass = masses[i];
      vector<double> this_limit = limits[i];
      vector<double> ratio_i, mass_i;
    
      for (size_t j = 0; j < this_mass.size(); ++j) {
        double m = this_mass[j];
        auto it = find(mass_nominal.begin(), mass_nominal.end(), m);
        if (it != mass_nominal.end()) {
          int idx_nom = distance(mass_nominal.begin(), it);
          double r = this_limit[j] / limit_nominal[idx_nom];
          ratio_i.push_back(r);
          mass_i.push_back(m);
        } else {
          cout << "[INFO] Mass " << m << " in limit[" << i << "] not found in nominal. Skipping..." << endl;
        }
      }
    
      ratio_vs_nominal.push_back(ratio_i);
      mass_vs_nominal.push_back(mass_i);
    }

    // ratio with EXO-17-028 expected
    vector<double> ratio_17028;
    vector<double> mass_comp_17028;
    
    for (int i = 0; i < this_nm_17028; ++i) {
      double m = mass_17028[i];
      auto it = find(mass_nominal.begin(), mass_nominal.end(), m);
      if (it != mass_nominal.end()) {
        int idx = distance(mass_nominal.begin(), it);
        double ratio = exp_17028[i] / limit_nominal[idx];
        ratio_17028.push_back(ratio);
        mass_comp_17028.push_back(m);
      } else {
        cout << "[INFO] Mass " << m << " not found in my limit list. Skipping..." << endl;
      }
    }

    // ratio with EXO-21-003 expected
    vector<double> ratio_21003;
    vector<double> mass_comp_21003;

    for (int i = 0; i < this_nm_21003; ++i) {
      double m = mass_21003[i];
      auto it = find(mass_nominal.begin(), mass_nominal.end(), m);
      if (it != mass_nominal.end()) {
        int idx = distance(mass_nominal.begin(), it);
        double ratio = this_exp_21003[i] / limit_nominal[idx];
        ratio_21003.push_back(ratio);
        mass_comp_21003.push_back(m);
      } else {
        cout << "[INFO] Mass " << m << " not found in my limit list, skipping..." << endl;
      }
    }

    // ratio with EXOT-2020-06 expected
    vector<double> ratio_202006;
    vector<double> mass_comp_202006;

    for (int i = 0; i < n_mass_202006; ++i) {
      double m = mass_202006[i];
      auto it = find(mass_nominal.begin(), mass_nominal.end(), m);
      if (it != mass_nominal.end()) {
        int idx = distance(mass_nominal.begin(), it);
        double ratio = exp_202006[i] / limit_nominal[idx];
        ratio_202006.push_back(ratio);
        mass_comp_202006.push_back(m);
      } else {
        cout << "[INFO] Mass " << m << " not found in my limit list, skipping..." << endl;
      }
    }

    // ratio with EXOT-2023-16 expected
    vector<double> ratio_202316;
    vector<double> mass_comp_202316;

    for (int i = 0; i < n_mass_202316; ++i) {
      double m = mass_202316[i];
      auto it = find(mass_nominal.begin(), mass_nominal.end(), m);
      if (it != mass_nominal.end()) {
        int idx = distance(mass_nominal.begin(), it);
        double ratio = exp_202316[i] / limit_nominal[idx];
        ratio_202316.push_back(ratio);
        mass_comp_202316.push_back(m);
      } else {
        cout << "[INFO] Mass " << m << " not found in my limit list, skipping..." << endl;
      }
    }


    TH1D *dummy2 = new TH1D("hist2", "", 100000, 0., 100000.);
    dummy2->GetYaxis()->SetTitleSize(0.1);
    dummy2->GetYaxis()->SetTitleOffset(0.5);
    if(channel=="MuMu") dummy2->GetYaxis()->SetTitle("#frac{Compared}{Nominal}");
    else if(channel=="EE") dummy2->GetYaxis()->SetTitle("#frac{Compared}{Nominal}");
    else if(channel=="EMu") dummy2->GetYaxis()->SetTitle("#frac{Compared}{Nominal}");
    if(SepLimit==1) dummy2->GetYaxis()->SetTitle("#frac{Signal sep.}{Combined}");
    else if(SepLimit==2) dummy2->GetYaxis()->SetTitle("#frac{SR sep.}{Combined}");
    dummy2->GetYaxis()->SetLabelSize(0.12);
    if(CompareLimits){
      dummy2->GetXaxis()->SetTitleSize(0.13);
      dummy2->GetXaxis()->SetTitleOffset(0.5);
    }
    else{
      dummy2->GetXaxis()->SetTitleSize(0.1);
      dummy2->GetXaxis()->SetTitleOffset(0.4);
    }
    dummy2->GetXaxis()->SetTitle("m_{N} (GeV)");
    dummy2->GetXaxis()->SetLabelSize(0.12);
    dummy2->GetXaxis()->SetRangeUser(80., 30000);
    if(channel=="EMu")dummy2->GetXaxis()->SetRangeUser(80., 60000);
    if(Logy) dummy2->GetYaxis()->SetRangeUser(0.5, 20);
    //if(Logy) dummy2->GetYaxis()->SetRangeUser(0.5, 3);
    //else dummy2->GetYaxis()->SetRangeUser(0.85, 2);
    //else dummy2->GetYaxis()->SetRangeUser(0.94, 1.06);
    else dummy2->GetYaxis()->SetRangeUser(0.5, 1.5);
    dummy2->SetTitle("");
    dummy2->Draw("hist");

    vector<TGraph*> gr_ratios;
    
    for (size_t i = 0; i < ratio_vs_nominal.size(); ++i) {
      TGraph* gr = new TGraph(
        mass_vs_nominal[i].size(),
        &mass_vs_nominal[i][0],
        &ratio_vs_nominal[i][0]
      );
      gr->SetLineWidth(2);
      gr->SetLineColor(colors[i]);
      gr->SetMarkerColor(colors[i]);
      gr_ratios.push_back(gr);
    }

    for (TGraph* g : gr_ratios) {
      if (g) g->Draw("lpsame");
    }

    // limit ratios to EXO-17-028
    TGraph *gr_ratio_17028 = new TGraph(mass_comp_17028.size(), &mass_comp_17028[0], &ratio_17028[0]);
    gr_ratio_17028->SetMarkerColor(kRed);
    gr_ratio_17028->SetLineColor(kRed);
    gr_ratio_17028->SetLineWidth(2);
    if(AddPub) gr_ratio_17028->Draw("lpsame"); // EXO-17-028

    // limit ratios to EXO-21-003
    if(channel=="MuMu"){
      TGraph *gr_ratio_21003 = new TGraph(mass_comp_21003.size(), &mass_comp_21003[0], &ratio_21003[0]);
      gr_ratio_21003->SetMarkerColor(kBlue);
      gr_ratio_21003->SetLineColor(kBlue);
      gr_ratio_21003->SetLineWidth(2);
      if(AddPub) gr_ratio_21003->Draw("lpsame"); // EXO-21-003
      TGraph *gr_ratio_202006 = new TGraph(mass_comp_202006.size(), &mass_comp_202006[0], &ratio_202006[0]);
      gr_ratio_202006->SetMarkerColor(kBlue-9);
      gr_ratio_202006->SetLineColor(kBlue-9);
      gr_ratio_202006->SetLineWidth(2);
      if(AddPub&&!IsXsecLimit) gr_ratio_202006->Draw("lpsame"); // EXOT-2020-06
    }
    else if(channel=="EE"||channel=="EMu"){
      TGraph *gr_ratio_202316 = new TGraph(mass_comp_202316.size(), &mass_comp_202316[0], &ratio_202316[0]);
      gr_ratio_202316->SetMarkerColor(kBlue-9);
      gr_ratio_202316->SetLineColor(kBlue-9);
      gr_ratio_202316->SetLineWidth(2);
      if(AddPub&&!IsXsecLimit) gr_ratio_202316->Draw("lpsame"); // EXOT-2023-16
    }

    if(Logy) c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+"_comp_Logy.pdf");
    else c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+"_comp.pdf");
    if(Logy) c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+"_comp_Logy.png");
    else c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+"_comp.png");
    print_ratio_table(mass_vs_nominal,ratio_vs_nominal,mass_nominal,descrps,this_plotpath+"/"+year+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+"_comp.txt",channel,AppendLimitTable);
  }
  else if(DrawExt){
    if(Logy) c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+"_Logy.pdf");
    else c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+".pdf");
    if(Logy) c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+"_Logy.png");
    else c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+".png");
  }
  else{
    if(Logy) c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+"_Logy.pdf");
    else c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+".pdf");
    if(Logy) c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+"_Logy.png");
    else c_Dilep->SaveAs(this_plotpath+"/"+year+"_"+channel+"_13TeV_"+WP_name+tag_nom+Name_IsXsecLimit+AddPubTxt+SepLimitTxt+".png");
  }

  return;
}

double GetDYxsec(int mass, TString channel){ // /data9/Users/jihkim_public/Type1/Type1_xsecs/DYTypeI_NLO_XsecEE_BRmultiplied_SS.txt * 10000 (scale to V=1); xsec in pb

  double this_xsec;
  if(mass==15)    this_xsec = 10000*0.4644284577;
  if(mass==20)    this_xsec = 10000*0.4451020715;
  if(mass==30)    this_xsec = 10000*0.3878409444;
  if(mass==40)    this_xsec = 10000*0.3106031805;
  if(mass==50)    this_xsec = 10000*0.21907503667;
  if(mass==60)    this_xsec = 10000*0.12336928471;
  if(mass==70)    this_xsec = 10000*0.0420040158;
  if(mass==75)    this_xsec = 10000*0.01526706468;
  if(mass==85)    this_xsec = 10000*0.0024159997584;
  if(mass==90)    this_xsec = 10000*0.001643333169;
  if(mass==95)    this_xsec = 10000*0.00116134864166;
  if(mass==100)   this_xsec = 10000*0.000826350052365;
  if(mass==125)   this_xsec = 10000*0.000259371513063;
  if(mass==150)   this_xsec = 10000*0.000116597780507;
  if(mass==200)   this_xsec = 10000*3.57786272755e-05;
  if(mass==250)   this_xsec = 10000*1.49950878991e-05;
  if(mass==300)   this_xsec = 10000*7.43681976965e-06;
  if(mass==350)   this_xsec = 10000*4.112418747999959e-06;
  if(mass==400)   this_xsec = 10000*2.45309287603e-06;
  if(mass==450)   this_xsec = 10000*1.5461080519999844e-06;
  if(mass==500)   this_xsec = 10000*1.01479289272e-06;
  if(mass==600)   this_xsec = 10000*4.83348624065e-07;
  if(mass==700)   this_xsec = 10000*2.52158749717e-07;
  if(mass==800)   this_xsec = 10000*1.40763360024e-07;
  if(mass==900)   this_xsec = 10000*8.25462040455e-08;
  if(mass==1000)  this_xsec = 10000*5.04281513305e-08;
  if(mass==1100)  this_xsec = 10000*3.16719142728e-08;
  if(mass==1200)  this_xsec = 10000*2.04280405905e-08;
  if(mass==1300)  this_xsec = 10000*1.34709703562e-08;
  if(mass==1400)  this_xsec = 10000*9.0345813159e-09;
  if(mass==1500)  this_xsec = 10000*6.153183296e-09;
  if(mass==1600)  this_xsec = 10000*4.25307743069e-09;
  if(mass==1700)  this_xsec = 10000*2.96975882769e-09;
  if(mass==1800)  this_xsec = 10000*2.09509353849e-09;
  if(mass==1900)  this_xsec = 10000*1.49392749967e-09;
  if(mass==2000)  this_xsec = 10000*1.07244692035e-09;
  if(mass==2500)  this_xsec = 10000*2.2953977199999996e-10;
  if(mass==3000)  this_xsec = 10000*5.7583824586666664e-11;
  if(mass==5000)  this_xsec = 0.;
  if(mass==7500)  this_xsec = 0.;
  if(mass==10000) this_xsec = 0.;
  if(mass==15000) this_xsec = 0.;
  if(mass==20000) this_xsec = 0.;
  if(mass==25000) this_xsec = 0.;
  if(mass==30000) this_xsec = 0.;
  if(mass==40000) this_xsec = 0.;
  if(mass==50000) this_xsec = 0.;
  if(mass==60000) this_xsec = 0.;
  if(mass==70000) this_xsec = 0.;
  if(mass==80000) this_xsec = 0.;
  if(mass==90000) this_xsec = 0.;
  if(mass==100000)this_xsec = 0.;

  if(channel=="EE"||channel=="MuMu") return this_xsec;
  else if(channel=="EMu") return this_xsec*2;
  return 0;

}

double GetVBFxsec(int mass, TString channel){ // /data6/Users/jihkim/SKFlatAnalyzer/data/Run2UltraLegacy_v3/2018/Sample/CommonSampleInfo/VBFTypeI_DF_M400_private.txt divided by 8; /data9/Users/jihkim_public/Type1/Type1_xsecs/VBFTypeI_NLO_XsecEE_BRmultiplied_SS.txt * 10000; xsec in pb (V=1)

  double this_xsec;
  if(mass==85)    this_xsec = 0.;
  if(mass==90)    this_xsec = 0.;
  if(mass==95)    this_xsec = 0.;
  if(mass==100)   this_xsec = 0.;
  if(mass==125)   this_xsec = 0.;
  if(mass==150)   this_xsec = 0.;
  if(mass==200)   this_xsec = 0.;
  if(mass==250)   this_xsec = 0.;
  if(mass==300)   this_xsec = 10000*6.510161879333267e-07;
  if(mass==350)   this_xsec = 10000*5.208354403999949e-07;
  if(mass==400)   this_xsec = 10000*4.2478900486666243e-07;
  if(mass==450)   this_xsec = 10000*3.5008180079999645e-07;
  if(mass==500)   this_xsec = 10000*2.91733616626e-07;
  if(mass==600)   this_xsec = 10000*2.07271062539e-07;
  if(mass==700)   this_xsec = 10000*1.51244445515e-07;
  if(mass==800)   this_xsec = 10000*1.13025739998e-07;
  if(mass==900)   this_xsec = 10000*8.5171526743e-08;
  if(mass==1000)  this_xsec = 10000*6.5193755934e-08;
  if(mass==1100)  this_xsec = 10000*5.06951932905e-08;
  if(mass==1200)  this_xsec = 10000*3.93646163636e-08;
  if(mass==1300)  this_xsec = 10000*3.09606267106e-08;
  if(mass==1400)  this_xsec = 10000*2.44815567052e-08;
  if(mass==1500)  this_xsec = 10000*1.95349771398e-08;
  if(mass==1600)  this_xsec = 10000*1.55611811025e-08;
  if(mass==1700)  this_xsec = 10000*1.24673049266e-08;
  if(mass==1800)  this_xsec = 10000*1.00778343095e-08;
  if(mass==1900)  this_xsec = 10000*8.12941093575e-09;
  if(mass==2000)  this_xsec = 10000*6.59633344035e-09;
  if(mass==2500)  this_xsec = 10000*2.36798936e-09;
  if(mass==3000)  this_xsec = 10000*8.757678999999999e-10;
  if(mass==5000)  this_xsec = 0.;
  if(mass==7500)  this_xsec = 0.;
  if(mass==10000) this_xsec = 0.;
  if(mass==15000) this_xsec = 0.;
  if(mass==20000) this_xsec = 0.;
  if(mass==25000) this_xsec = 0.;
  if(mass==30000) this_xsec = 0.;
  if(mass==40000) this_xsec = 0.;
  if(mass==50000) this_xsec = 0.;
  if(mass==60000) this_xsec = 0.;
  if(mass==70000) this_xsec = 0.;
  if(mass==80000) this_xsec = 0.;
  if(mass==90000) this_xsec = 0.;
  if(mass==100000)this_xsec = 0.;

  if(channel=="EE"||channel=="MuMu") return this_xsec;
  else if(channel=="EMu") return this_xsec*2;
  return 0;

}

double GetSSWWxsec(int mass, TString channel){ // /data9/Users/jihkim_public/Type1/Type1_xsecs/SSWWTypeI_NLO_Xsec_EMu.txt; xsec in pb (V=1)

  double this_xsec;
  if(mass==85)      this_xsec = 0.;
  if(mass==90)      this_xsec = 0.;
  if(mass==95)      this_xsec = 0.;
  if(mass==100)     this_xsec = 0.;
  if(mass==125)     this_xsec = 0.;
  if(mass==150)     this_xsec = 0.;
  if(mass==200)     this_xsec = 0.;
  if(mass==250)     this_xsec = 0.;
  if(mass==300)     this_xsec = 0.;
  if(mass==400)     this_xsec = 0.;
  if(mass==500)     this_xsec = 0.03605;
  if(mass==600)     this_xsec = 0.03391;
  if(mass==700)     this_xsec = 0.03193;
  if(mass==750)     this_xsec = 0.03084;
  if(mass==800)     this_xsec = 0.02989;
  if(mass==900)     this_xsec = 0.02791;
  if(mass==1000)    this_xsec = 0.02587;
  if(mass==1100)    this_xsec = 0.02398;
  if(mass==1200)    this_xsec = 0.02239;
  if(mass==1250)    this_xsec = 0.02151;
  if(mass==1300)    this_xsec = 0.02087;
  if(mass==1500)    this_xsec = 0.01811;
  if(mass==1700)    this_xsec = 0.01591;
  if(mass==1750)    this_xsec = 0.01524;
  if(mass==2000)    this_xsec = 0.01306;
  if(mass==2500)    this_xsec = 0.009716;
  if(mass==3000)    this_xsec = 0.00745;
  if(mass==5000)    this_xsec = 0.003251;
  if(mass==7500)    this_xsec = 0.00156;
  if(mass==10000)   this_xsec = 0.000914;
  if(mass==15000)   this_xsec = 0.0004146;
  if(mass==20000)   this_xsec = 0.0002345;
  if(mass==25000)   this_xsec = 0.000152;
  if(mass==30000)   this_xsec = 0.0001051;
  if(mass==40000)   this_xsec = 5.897e-05;
  if(mass==50000)   this_xsec = 3.788e-05;
  if(mass==60000)   this_xsec = 2.613e-05;
  if(mass==70000)   this_xsec = 1.938e-05;
  if(mass==80000)   this_xsec = 1.485e-05;
  if(mass==90000)   this_xsec = 1.168e-05;
  if(mass==100000)  this_xsec = 9.495e-06;

  if(channel=="EE"||channel=="MuMu") return this_xsec/2.;
  else if(channel=="EMu") return this_xsec;
  return 0;

}


double GetDYxsec_17028(int mass, TString channel){ // AN2017_291_v11 * 100 /2 (scale to V=1) --> AN setting was SS+OS and V^2 = 0.01 (not 100 as written); xsec in pb

  double this_xsec;

  if(mass==85)    this_xsec =  22.65;
  if(mass==90)    this_xsec =  15.4;
  if(mass==95)    this_xsec =  10.9;
  if(mass==100)   this_xsec =  7.75;
  if(mass==125)   this_xsec =  2.445;
  if(mass==150)   this_xsec =  1.10;
  if(mass==200)   this_xsec =  0.34;
  if(mass==250)   this_xsec =  0.143;
  if(mass==300)   this_xsec =  0.0715;
  if(mass==400)   this_xsec =  0.02365;
  if(mass==500)   this_xsec =  0.00985;
  if(mass==600)   this_xsec =  0.00472;
  if(mass==700)   this_xsec =  0.00247;
  if(mass==800)   this_xsec =  0.001385;
  if(mass==900)   this_xsec =  0.00081;
  if(mass==1000)  this_xsec =  0.000496;
  if(mass==1100)  this_xsec =  0.000312;
  if(mass==1200)  this_xsec =  0.000202;
  if(mass==1300)  this_xsec =  0.000133;
  if(mass==1400)  this_xsec =  0.000089;
  if(mass==1500)  this_xsec =  0.0000605;
  if(mass==1700)  this_xsec =  0.000029;
  if(mass==2000)  this_xsec =  0.00001025;

  if(channel=="EE"||channel=="MuMu") return this_xsec;
  else if(channel=="EMu") return this_xsec*2;
  return 0;

}

double GetVBFxsec_17028(int mass, TString channel){ // AN2017_291_v11 * 100 /2 (scale to V=1) --> AN setting was SS+OS and V^2 = 0.01 (not 100 as written); xsec in pb

  double this_xsec;

  if(mass==85)    this_xsec = 0.;
  if(mass==90)    this_xsec = 0.;
  if(mass==95)    this_xsec = 0.;
  if(mass==100)   this_xsec = 0.04825;
  if(mass==150)   this_xsec = 0.02455;
  if(mass==200)   this_xsec = 0.0164;
  if(mass==300)   this_xsec = 0.00915;
  if(mass==400)   this_xsec = 0.0058;
  if(mass==500)   this_xsec = 0.003905;
  if(mass==600)   this_xsec = 0.00275;
  if(mass==700)   this_xsec = 0.00199;
  if(mass==800)   this_xsec = 0.00147;
  if(mass==900)   this_xsec = 0.00111;
  if(mass==1000)  this_xsec = 0.000845;
  if(mass==1100)  this_xsec = 0.00065;
  if(mass==1200)  this_xsec = 0.00051;
  if(mass==1300)  this_xsec = 0.000397;
  if(mass==1400)  this_xsec = 0.0003135;
  if(mass==1500)  this_xsec = 0.00025;
  if(mass==1700)  this_xsec = 0.0001605;
  if(mass==2000)  this_xsec = 0.0000845;

  if(channel=="EE"||channel=="MuMu") return this_xsec;
  else if(channel=="EMu") return this_xsec*2;
  return 0;

}

double GetSSWWxsec_21003(int mass, TString channel){ // AN2021_008_v12 Table 1, lumi-weighted using 35.9, 41.5, 59.7 following Sec. 5.1; xsec in pb

  double this_xsec;

  if(mass==50)    this_xsec = 4.6372 * 0.001;
  if(mass==150)   this_xsec = 13.5936 * 0.001;
  if(mass==300)   this_xsec = 17.7465 * 0.001;
  if(mass==450)   this_xsec = 18.0586 * 0.001;
  if(mass==600)   this_xsec = 16.7398 * 0.001;
  if(mass==750)   this_xsec = 15.36 * 0.001;
  if(mass==900)   this_xsec = 13.7605 * 0.001;
  if(mass==1000)  this_xsec = 12.7143 * 0.001;
  if(mass==1250)  this_xsec = 10.526 * 0.001;
  if(mass==1500)  this_xsec = 8.8342 * 0.001;
  if(mass==1750)  this_xsec = 7.5311 * 0.001;
  if(mass==2000)  this_xsec = 6.3828 * 0.001;
  if(mass==2500)  this_xsec = 4.7563 * 0.001;
  if(mass==5000)  this_xsec = 1.5818 * 0.001;
  if(mass==7500)  this_xsec = 0.768 * 0.001;
  if(mass==10000) this_xsec = 0.4348 * 0.001;
  if(mass==15000) this_xsec = 0.2043 * 0.001;
  if(mass==20000) this_xsec = 0.1145 * 0.001;

  if(channel=="MuMu") return this_xsec;
  else return 0;

}
