TFile *f1 = new TFile("/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegionPlotter_TestOldMuID/LimitInputs/HNL_ULID/2017/M100_MuMu_card_input.root");
//TFile *f1 = new TFile("/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegionPlotter/LimitInputs/HNL_ULID/2017/M100_MuMu_card_input.root");
TH1D *h_sig = (TH1D*)f1->Get("signalDYVBF");
TH1D *h_bkg = (TH1D*)f1->Get("data_obs");
double FOM = 0.;
double this_sig = 0.;
double this_bkg = 0.;
double this_FOM = 0.;
for(int i=1; i<20; i++){
  this_sig = h_sig->GetBinContent(i);
  this_bkg = h_bkg->GetBinContent(i);
  if(this_bkg<=0) this_bkg = 0.001;
  this_FOM = sqrt( 2.*((this_sig+this_bkg)*log(1+(this_sig/(this_bkg))) - this_sig) );
  FOM+=this_FOM;
};
cout << "total FOM : " << FOM << endl;


FOM = 0;
this_FOM = sqrt( 2.*((67.+18.57)*log(1+(67./(18.57))) - 67.) );
FOM+=this_FOM;
this_FOM = sqrt( 2.*((0.3+0.99)*log(1+(0.3/(0.99))) - 0.3) );
FOM+=this_FOM;
cout << "17028 FOM : " << FOM << endl;
