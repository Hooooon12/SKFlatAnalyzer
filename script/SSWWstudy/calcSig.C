void calcSig(TString signal, TString mass){
  /////////////////////////////////////MC////////////////////////////////////////////
  string fileline;
  ifstream in("fileList.txt");
  double MC_SR = 0., MC_SR_inv = 0., MC_highSR1 = 0., MC_highSR1_mass = 0., MC_highSR1_1jet = 0., MC_highSR1_1jet_mass = 0., MC_highSR2 = 0., MC_highSR2_mass = 0.;
  double signal_SR = 0., signal_SR_inv = 0., signal_highSR1 = 0., signal_highSR1_mass = 0., signal_highSR1_1jet = 0., signal_highSR1_1jet_mass = 0., signal_highSR2 = 0., signal_highSR2_mass = 0.;
  double fake_SR = 0., fake_SR_inv = 0., fake_highSR1 = 0., fake_highSR1_mass = 0., fake_highSR1_1jet = 0., fake_highSR1_1jet_mass = 0., fake_highSR2 = 0., fake_highSR2_mass = 0.;
  // Line loop
  while(getline(in, fileline)){
    std::istringstream is(fileline);
    TString this_line = fileline;
    if(this_line.Contains("#")||this_line=="") continue;
    TString category, process, isSkim;
    is >> category;
    is >> process;
    is >> isSkim;
    //cout << "process : " << process << endl;
    //cout << "isSkim : " << isSkim << endl;

    if(category=="signal"){
      if(process!=signal) continue;
    }

    TFile *f_MC;
    TH1D *h_MC_SR, *h_MC_SR_inv, *h_MC_highSR1, *h_MC_highSR1_mass, *h_MC_highSR1_1jet, *h_MC_highSR1_1jet_mass, *h_MC_highSR2, *h_MC_highSR2_mass;
    if(isSkim=="skimmed") f_MC = new TFile("SSWW_SkimTree_Dilepton_"+process+".root");
    else f_MC = new TFile("SSWW_"+process+".root");
    //cout << "f_MC : " << f_MC << endl;
    h_MC_SR = (TH1D*)f_MC->Get("SR/HToverPt1_HN");
    h_MC_SR_inv = (TH1D*)f_MC->Get("SR_inv/HToverPt1_HN");
    h_MC_highSR1 = (TH1D*)f_MC->Get("highSR1/HToverPt1_HN");
    h_MC_highSR1_mass = (TH1D*)f_MC->Get("highSR1/"+mass+"/HToverPt1_HN");
    h_MC_highSR1_1jet = (TH1D*)f_MC->Get("highSR1_1jet/HToverPt1_HN");
    h_MC_highSR1_1jet_mass = (TH1D*)f_MC->Get("highSR1_1jet/"+mass+"/HToverPt1_HN");
    h_MC_highSR2 = (TH1D*)f_MC->Get("highSR2/HToverPt1_HN");
    h_MC_highSR2_mass = (TH1D*)f_MC->Get("highSR2/"+mass+"/HToverPt1_HN");
    int Nbin = 10; // Nbin of HToverPt1
    cout.flags(ios::left); // left align; see https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=tipsware&logNo=221115657586
    cout << "========================================================" << endl;
    cout << "!!process : " << process << "!!" << endl;
    if(h_MC_SR){
                                cout << "SR                                     : " << h_MC_SR->Integral(0,Nbin+1) << endl;
                                if(category=="MC")     MC_SR     += h_MC_SR->Integral(0,Nbin+1);
                                if(category=="signal") signal_SR += h_MC_SR->Integral(0,Nbin+1);
    }
    else                        cout << "SR                                     : NULL;" << endl;
    if(h_MC_SR_inv){
                                cout << "SR_inv                                 : " << h_MC_SR_inv->Integral(0,Nbin+1) << endl;
                                if(category=="MC")         MC_SR_inv += h_MC_SR_inv->Integral(0,Nbin+1);
                                if(category=="signal") signal_SR_inv += h_MC_SR_inv->Integral(0,Nbin+1);
    }
    else                        cout << "SR_inv                                 : NULL;" << endl;
    if(h_MC_highSR1){
                                cout << "highSR1                                : " << h_MC_highSR1->Integral(0,Nbin+1) << endl;
                                if(category=="MC")         MC_highSR1 += h_MC_highSR1->Integral(0,Nbin+1);
                                if(category=="signal") signal_highSR1 += h_MC_highSR1->Integral(0,Nbin+1);
    }
    else                        cout << "highSR1                                : NULL;" << endl;
    if(h_MC_highSR1_mass){
                                cout << "highSR1, with "; cout.width(6); cout << mass; cout << " optimization      : " << h_MC_highSR1_mass->Integral(0,Nbin+1) << endl;
                                if(category=="MC")         MC_highSR1_mass += h_MC_highSR1_mass->Integral(0,Nbin+1);
                                if(category=="signal") signal_highSR1_mass += h_MC_highSR1_mass->Integral(0,Nbin+1);
    }
    else                        {cout << "highSR1, with "; cout.width(6); cout << mass; cout << " optimization      : NULL;" << endl;}
    if(h_MC_highSR1_1jet){
                                cout << "highSR1_1jet                           : " << h_MC_highSR1_1jet->Integral(0,Nbin+1) << endl;
                                if(category=="MC")         MC_highSR1_1jet += h_MC_highSR1_1jet->Integral(0,Nbin+1);
                                if(category=="signal") signal_highSR1_1jet += h_MC_highSR1_1jet->Integral(0,Nbin+1);
    }
    else                        cout << "highSR1_1jet                           : NULL;" << endl;
    if(h_MC_highSR1_1jet_mass){
                                cout << "highSR1_1jet, with "; cout.width(6); cout << mass; cout << " optimization : " << h_MC_highSR1_1jet_mass->Integral(0,Nbin+1) << endl;
                                if(category=="MC")         MC_highSR1_1jet_mass += h_MC_highSR1_1jet_mass->Integral(0,Nbin+1);
                                if(category=="signal") signal_highSR1_1jet_mass += h_MC_highSR1_1jet_mass->Integral(0,Nbin+1);
    }
    else                        {cout << "highSR1_1jet, with "; cout.width(6); cout << mass; cout << " optimization : NULL;" << endl;}
    if(h_MC_highSR2){
                                cout << "highSR2                                : " << h_MC_highSR2->Integral(0,Nbin+1) << endl;
                                if(category=="MC")         MC_highSR2 += h_MC_highSR2->Integral(0,Nbin+1);
                                if(category=="signal") signal_highSR2 += h_MC_highSR2->Integral(0,Nbin+1);
    }
    else                        cout << "highSR2                                : NULL;" << endl;
    if(h_MC_highSR2_mass){
                                cout << "highSR2, with "; cout.width(6); cout << mass; cout << " optimization      : " << h_MC_highSR2_mass->Integral(0,Nbin+1) << endl;
                                if(category=="MC")         MC_highSR2_mass += h_MC_highSR2_mass->Integral(0,Nbin+1);
                                if(category=="signal") signal_highSR2_mass += h_MC_highSR2_mass->Integral(0,Nbin+1);
    }
    else                        {cout << "highSR2, with "; cout.width(6); cout << mass; cout << " optmization       : NULL;" << endl;}
  }
  cout << "===================================================================================================" << endl;
  cout << "total MC in SR                                    : " << MC_SR << endl;
  cout << "total MC in SR_inv                                : " << MC_SR_inv << endl;
  cout << "total MC in highSR1                               : " << MC_highSR1 << endl;
  cout << "total MC in highSR1, with "; cout.width(6); cout << mass; cout << " optimization     : " << MC_highSR1_mass << endl;
  cout << "total MC in highSR1_1jet                          : " << MC_highSR1_1jet << endl;
  cout << "total MC in highSR1_1jet, with "; cout.width(6); cout << mass; cout << "optimization : " << MC_highSR1_1jet_mass << endl;
  cout << "total MC in highSR2                               : " << MC_highSR2 << endl;
  cout << "total MC in highSR2, with "; cout.width(6); cout << mass; cout << "optimization      : " << MC_highSR2_mass << endl;

  /////////////////////////////////////Fake////////////////////////////////////////////
  TFile *f_Fake = new TFile("/data6/Users/jihkim/SKFlatOutput/Run2Legacy_v4/SSWW/2016/jcln_inv__fatjet_veto__RunFake__/DATA/SSWW_SkimTree_Dilepton_DoubleMuon.root");
  TH1D *h_Fake_SR, *h_Fake_SR_inv, *h_Fake_highSR1, *h_Fake_highSR1_mass, *h_Fake_highSR1_1jet, *h_Fake_highSR1_1jet_mass, *h_Fake_highSR2, *h_Fake_highSR2_mass;
  h_Fake_SR = (TH1D*)f_Fake->Get("SR/HToverPt1_HN");
  h_Fake_SR_inv = (TH1D*)f_Fake->Get("SR_inv/HToverPt1_HN");
  h_Fake_highSR1 = (TH1D*)f_Fake->Get("highSR1/HToverPt1_HN");
  h_Fake_highSR1_mass = (TH1D*)f_Fake->Get("highSR1/"+mass+"/HToverPt1_HN");
  h_Fake_highSR1_1jet = (TH1D*)f_Fake->Get("highSR1_1jet/HToverPt1_HN");
  h_Fake_highSR1_1jet_mass = (TH1D*)f_Fake->Get("highSR1_1jet/"+mass+"/HToverPt1_HN");
  h_Fake_highSR2 = (TH1D*)f_Fake->Get("highSR2/HToverPt1_HN");
  h_Fake_highSR2_mass = (TH1D*)f_Fake->Get("highSR2/"+mass+"/HToverPt1_HN");
  int Nbin = 10; // Nbin of HToverPt1
  cout << "===================================================================================================" << endl;
  if(h_Fake_SR){
                              cout << "fake in SR                                        : " << h_Fake_SR->Integral(0,Nbin+1) << endl;
                              fake_SR += h_Fake_SR->Integral(0,Nbin+1);
  }
  else                        cout << "fake in SR                                        : NULL;" << endl;
  if(h_Fake_SR_inv){
                              cout << "fake in SR_inv                                    : " << h_Fake_SR_inv->Integral(0,Nbin+1) << endl;
                              fake_SR_inv += h_Fake_SR_inv->Integral(0,Nbin+1);
  }
  else                        cout << "fake in SR_inv                                    : NULL;" << endl;
  if(h_Fake_highSR1){
                              cout << "fake in highSR1                                   : " << h_Fake_highSR1->Integral(0,Nbin+1) << endl;
                              fake_highSR1 += h_Fake_highSR1->Integral(0,Nbin+1);
  }
  else                        cout << "fake in highSR1                                   : NULL;" << endl;
  if(h_Fake_highSR1_mass){
                              cout << "fake in highSR1, with "; cout.width(6); cout << mass; cout << " optimization         : " << h_Fake_highSR1_mass->Integral(0,Nbin+1) << endl;
                              fake_highSR1_mass += h_Fake_highSR1_mass->Integral(0,Nbin+1);
  }
  else                        {cout << "fake in highSR1, with "; cout.width(6); cout << mass; cout << " optimization         : NULL;" << endl;}
  if(h_Fake_highSR1_1jet){
                              cout << "fake in highSR1_1jet                              : " << h_Fake_highSR1_1jet->Integral(0,Nbin+1) << endl;
                              fake_highSR1_1jet += h_Fake_highSR1_1jet->Integral(0,Nbin+1);
  }
  else                        cout << "fake in highSR1_1jet                              : NULL;" << endl;
  if(h_Fake_highSR1_1jet_mass){
                              cout << "fake in highSR1_1jet, with "; cout.width(6); cout << mass; cout << " optimization    : " << h_Fake_highSR1_1jet_mass->Integral(0,Nbin+1) << endl;
                              fake_highSR1_1jet_mass += h_Fake_highSR1_1jet_mass->Integral(0,Nbin+1);
  }
  else                        {cout << "fake in highSR1_1jet, with "; cout.width(6); cout << mass; cout << " optimization    : NULL;" << endl;}
  if(h_Fake_highSR2){
                              cout << "fake in highSR2                                   : " << h_Fake_highSR2->Integral(0,Nbin+1) << endl;
                              fake_highSR2 += h_Fake_highSR2->Integral(0,Nbin+1);
  }
  else                        cout << "fake in highSR2                                   : NULL;" << endl;
  if(h_Fake_highSR2_mass){
                              cout << "fake in highSR2, with "; cout.width(6); cout << mass; cout << " optimization         : " << h_Fake_highSR2_mass->Integral(0,Nbin+1) << endl;
                              fake_highSR2_mass += h_Fake_highSR2_mass->Integral(0,Nbin+1);
  }
  else                        {cout << "fake in highSR2, with "; cout.width(6); cout << mass; cout << " optimization         : NULL;" << endl;}

  ///////////////////////////////////signal//////////////////////////////////////////
  if(signal.Contains("DY")||signal.Contains("VBF")){
    signal_SR                   *= 10000.;
    signal_SR_inv               *= 10000.;
    signal_highSR1              *= 10000.;
    signal_highSR1_mass      *= 10000.;
    signal_highSR1_1jet         *= 10000.;
    signal_highSR1_1jet_mass *= 10000.;
    signal_highSR2              *= 10000.;
    signal_highSR2_mass      *= 10000.;
  }
  cout << "===================================================================================================" << endl;
  cout << "signal " << signal << " in SR                  : ";
  cout.width(12); cout << signal_SR << "--> S/sqrt(B) = ";
  cout.width(12); cout << signal_SR/sqrt(fake_SR+MC_SR);
  cout << ", full expansion = " << sqrt( 2.*((signal_SR+fake_SR+MC_SR)*log(1+(signal_SR/(fake_SR+MC_SR))) - signal_SR) ) << endl;
  cout << "signal " << signal << " in SR_inv              : ";
  cout.width(12); cout << signal_SR_inv << "--> S/sqrt(B) = ";
  cout.width(12); cout << signal_SR_inv/sqrt(fake_SR_inv+MC_SR_inv);
  cout << ", full expansion = " << sqrt( 2.*((signal_SR_inv+fake_SR_inv+MC_SR_inv)*log(1+(signal_SR_inv/(fake_SR_inv+MC_SR_inv))) - signal_SR_inv) ) << endl;
  cout << "signal " << signal << " in highSR1             : ";
  cout.width(12); cout << signal_highSR1 << "--> S/sqrt(B) = ";
  cout.width(12); cout << signal_highSR1/sqrt(fake_highSR1+MC_highSR1);
  cout << ", full expansion = " << sqrt( 2.*((signal_highSR1+fake_highSR1+MC_highSR1)*log(1+(signal_highSR1/(fake_highSR1+MC_highSR1))) - signal_highSR1) ) << endl;
  cout << "signal " << signal << " in highSR1_"; cout.width(6); cout << mass; cout << "      : ";
  cout.width(12); cout << signal_highSR1_mass << "--> S/sqrt(B) = ";
  cout.width(12); cout << signal_highSR1_mass/sqrt(fake_highSR1_mass+MC_highSR1_mass);
  cout << ", full expansion = " << sqrt( 2.*((signal_highSR1_mass+fake_highSR1_mass+MC_highSR1_mass)*log(1+(signal_highSR1_mass/(fake_highSR1_mass+MC_highSR1_mass))) - signal_highSR1_mass) ) << endl;
  cout << "signal " << signal << " in highSR1_1jet        : ";
  cout.width(12); cout << signal_highSR1_1jet << "--> S/sqrt(B) = ";
  cout.width(12); cout << signal_highSR1_1jet/sqrt(fake_highSR1_1jet+MC_highSR1_1jet);
  cout << ", full expansion = " << sqrt( 2.*((signal_highSR1_1jet+fake_highSR1_1jet+MC_highSR1_1jet)*log(1+(signal_highSR1_1jet/(fake_highSR1_1jet+MC_highSR1_1jet))) - signal_highSR1_1jet) ) << endl;
  cout << "signal " << signal << " in highSR1_1jet_"; cout.width(6); cout << mass; cout << " : ";
  cout.width(12); cout << signal_highSR1_1jet_mass << "--> S/sqrt(B) = ";
  cout.width(12); cout << signal_highSR1_1jet_mass/sqrt(fake_highSR1_1jet_mass+MC_highSR1_1jet_mass);
  cout << ", full expansion = " << sqrt( 2.*((signal_highSR1_1jet_mass+fake_highSR1_1jet_mass+MC_highSR1_1jet_mass)*log(1+(signal_highSR1_1jet_mass/(fake_highSR1_1jet_mass+MC_highSR1_1jet_mass))) - signal_highSR1_1jet_mass) ) << endl;
  cout << "signal " << signal << " in highSR2             : ";
  cout.width(12); cout << signal_highSR2 << "--> S/sqrt(B) = ";
  cout.width(12); cout << signal_highSR2/sqrt(fake_highSR2+MC_highSR2);
  cout << ", full expansion = " << sqrt( 2.*((signal_highSR2+fake_highSR2+MC_highSR2)*log(1+(signal_highSR2/(fake_highSR2+MC_highSR2))) - signal_highSR2) ) << endl;
  cout << "signal " << signal << " in highSR2_"; cout.width(6); cout << mass; cout << "      : ";
  cout.width(12); cout << signal_highSR2_mass << "--> S/sqrt(B) = ";
  cout.width(12); cout << signal_highSR2_mass/sqrt(fake_highSR2_mass+MC_highSR2_mass);
  cout << ", full expansion = " << sqrt( 2.*((signal_highSR2_mass+fake_highSR2_mass+MC_highSR2_mass)*log(1+(signal_highSR2_mass/(fake_highSR2_mass+MC_highSR2_mass))) - signal_highSR2_mass) ) << endl;
}
