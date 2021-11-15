#include <iostream>

// How to run this code?
// root -b -l -q makePlots_Fake.C

TString workdir = "/data6/Users/jihkim/SKFlatOutput/";
TString SKFlatVersion = "Run2Legacy_v4";
//TString skim = "SkimTree_Dilepton";
TString flag = "FR__Norm__";
TString analyzer = "Fake";
TString file_path = "";
TString PDname = "DoubleMuon";
//vector<TString> year = {"2016", "2017", "2018"};
vector<TString> year = {"2016"};
//vector<TString> luminosity = {"35.9", "41.5", "59.7"};
vector<TString> luminosity = {"35.9"};
vector<TString> ZGname = {"ZGTo2LG", "ZGToLLG_01J", "ZGToLLG_01J"};
vector<TString> WGname = {"WGToLNuG", "WGToLNuG_01J", "WGToLNuG_01J"};

const int MCNumber = 12; //
int maxBinNumber_total = 0, maxBinNumber_temp = 0;
double minRange = 0., maxRange = 0., binContent = 0., binError = 0., binError_Stat = 0., binError_Syst = 0.;
double max_Data = 0., max_Background = 0., max_Hist = 0.;

void prompt_subtraction_SSWW(){

  // Declare files and histos needed for making plots 
  TFile *f_Data[3], *f_Fake[3], *f_MC[MCNumber][3];
  TH1D *h_MC_LooseMuPt[MCNumber][3], *h_MC_LooseMuPt_Norm[MCNumber][3], *h_MC_TightMuPt[MCNumber][3], *h_MC_TightMuPt_Norm[MCNumber][3];
  TH1D *h_Data_LooseMuPt[3], *h_Data_TightMuPt[3], *h_Data_LooseMuPt_Subt[3], *h_Data_TightMuPt_Subt[3];
  TH1D *h_MC_LooseMuEta[MCNumber][3], *h_MC_LooseMuEta_Norm[MCNumber][3], *h_MC_TightMuEta[MCNumber][3], *h_MC_TightMuEta_Norm[MCNumber][3];
  TH1D *h_Data_LooseMuEta[3], *h_Data_TightMuEta[3], *h_Data_LooseMuEta_Subt[3], *h_Data_TightMuEta_Subt[3];
  TH2D *h_Data_LooseMu[3], *h_Data_TightMu[3], *h_MC_LooseMu[MCNumber][3], *h_MC_TightMu[MCNumber][3];
  TObject *h_Data_LooseMu_Subt_tmp[3], *h_Data_TightMu_Subt_tmp[3], *h_MC_LooseMu_Norm_tmp[MCNumber][3], *h_MC_TightMu_Norm_tmp[MCNumber][3]; // TH2 has no Clone function; instead I have to call TObject::Clone() first, and then type cast TH2D* .
  TH2D *h_Data_LooseMu_Subt[3], *h_Data_TightMu_Subt[3], *h_MC_LooseMu_Norm[MCNumber][3], *h_MC_TightMu_Norm[MCNumber][3];

  // Year loop
  for(int it_y=0; it_y<year.size(); it_y++){
    file_path = SKFlatVersion+"/"+analyzer+"/"+year.at(it_y)+"/FR__Norm__/";

    // PDname in 2018 : DoubleEG -> EGamma
    //if(channel.Contains("Electron")){
    //  if(it_y == 2) PDname = "EGamma";
    //  else PDname = "DoubleEG";
    //} // TODO : make prompt_subtraction get channel as an argument. so that the code can run with ele, mu each

    //=========================================
    //==== Set input ROOT files
    //=========================================

    // DATA, Fake
    f_Data[it_y]   = new TFile(workdir+file_path+"DATA/"+analyzer+"_"+PDname+".root");
    // MC : WJets
    f_MC[0][it_y] = new TFile(workdir+file_path+analyzer+"_WJets_MG.root");
    // MC : DYJets
    f_MC[1][it_y] = new TFile(workdir+file_path+analyzer+"_DYJets.root");
    // MC : Top
    f_MC[2][it_y]  = new TFile(workdir+file_path+analyzer+"_SingleTop_tch_top_Incl.root"); //JH
    f_MC[3][it_y]  = new TFile(workdir+file_path+analyzer+"_SingleTop_tch_antitop_Incl.root"); //JH
    f_MC[4][it_y]  = new TFile(workdir+file_path+analyzer+"_TTLJ_powheg.root");
    f_MC[5][it_y] = new TFile(workdir+file_path+analyzer+"_TTLL_powheg.root");
    //MC : VV
    f_MC[6][it_y]  = new TFile(workdir+file_path+analyzer+"_WZTo3LNu_powheg.root");
    f_MC[7][it_y]  = new TFile(workdir+file_path+analyzer+"_ZZTo4L_powheg.root");
    f_MC[8][it_y]  = new TFile(workdir+file_path+analyzer+"_"+ZGname.at(it_y)+".root");
    f_MC[9][it_y]  = new TFile(workdir+file_path+analyzer+"_WWToLNuQQ_powheg.root");
    f_MC[10][it_y]  = new TFile(workdir+file_path+analyzer+"_WWTo2L2Nu_powheg.root");
    f_MC[11][it_y]  = new TFile(workdir+file_path+analyzer+"_"+WGname.at(it_y)+".root");

    cout << "called the root files..." << endl;

    //=========================================
    //==== Get histograms
    //=========================================

    //TString region_tmp = channel+"/"+region;
    //if(region=="Z"||region=="W") region_tmp = region+"_"+channel;

    // DATA, MC
    h_Data_LooseMuPt[it_y] = (TH1D*)f_Data[it_y]->Get("Muon/SSWW/Muon_Loose_Pt_SSWW");
    h_Data_TightMuPt[it_y] = (TH1D*)f_Data[it_y]->Get("Muon/SSWW/Muon_Tight_Pt_SSWW");
    h_Data_LooseMuEta[it_y] = (TH1D*)f_Data[it_y]->Get("Muon/SSWW/Muon_Loose_Eta_SSWW");
    h_Data_TightMuEta[it_y] = (TH1D*)f_Data[it_y]->Get("Muon/SSWW/Muon_Tight_Eta_SSWW");
    h_Data_LooseMu[it_y] = (TH2D*)f_Data[it_y]->Get("Muon/SSWW/Muon_Loose_SSWW");
    h_Data_TightMu[it_y] = (TH2D*)f_Data[it_y]->Get("Muon/SSWW/Muon_Tight_SSWW");
    // MC
    for(int it_mc=0; it_mc<MCNumber; it_mc++){
      h_MC_LooseMuPt[it_mc][it_y] = (TH1D*)f_MC[it_mc][it_y]->Get("Muon/SSWW/Muon_Loose_Pt_SSWW");
      h_MC_TightMuPt[it_mc][it_y] = (TH1D*)f_MC[it_mc][it_y]->Get("Muon/SSWW/Muon_Tight_Pt_SSWW");
      h_MC_LooseMuEta[it_mc][it_y] = (TH1D*)f_MC[it_mc][it_y]->Get("Muon/SSWW/Muon_Loose_Eta_SSWW");
      h_MC_TightMuEta[it_mc][it_y] = (TH1D*)f_MC[it_mc][it_y]->Get("Muon/SSWW/Muon_Tight_Eta_SSWW");
      h_MC_LooseMu[it_mc][it_y] = (TH2D*)f_MC[it_mc][it_y]->Get("Muon/SSWW/Muon_Loose_SSWW");
      h_MC_TightMu[it_mc][it_y] = (TH2D*)f_MC[it_mc][it_y]->Get("Muon/SSWW/Muon_Tight_SSWW");
    }

    h_Data_LooseMuPt[it_y]->SetDirectory(0);
    h_Data_TightMuPt[it_y]->SetDirectory(0);
    h_Data_LooseMuEta[it_y]->SetDirectory(0);
    h_Data_TightMuEta[it_y]->SetDirectory(0);
    h_Data_LooseMu[it_y]->SetDirectory(0);
    h_Data_TightMu[it_y]->SetDirectory(0);
    for(int it_mc=0; it_mc<MCNumber; it_mc++){
      if(h_MC_LooseMuPt[it_mc][it_y]) h_MC_LooseMuPt[it_mc][it_y]->SetDirectory(0);
      if(h_MC_TightMuPt[it_mc][it_y]) h_MC_TightMuPt[it_mc][it_y]->SetDirectory(0);
      if(h_MC_LooseMuEta[it_mc][it_y]) h_MC_LooseMuEta[it_mc][it_y]->SetDirectory(0);
      if(h_MC_TightMuEta[it_mc][it_y]) h_MC_TightMuEta[it_mc][it_y]->SetDirectory(0);
      if(h_MC_LooseMu[it_mc][it_y]) h_MC_LooseMu[it_mc][it_y]->SetDirectory(0);
      if(h_MC_TightMu[it_mc][it_y]) h_MC_TightMu[it_mc][it_y]->SetDirectory(0);
    } // JH : you must set the directory so that you can have the histo after closing the root file.

    f_Data[it_y]->Close();
    for(int it_mc=0; it_mc<MCNumber; it_mc++){
      f_MC[it_mc][it_y]->Close();
    }

    //=========================================
    //==== Make plots
    //=========================================

    // 1D vs Pt, abs(Eta)
    h_Data_LooseMuPt_Subt[it_y] = (TH1D*)h_Data_LooseMuPt[it_y]->Clone();
    h_Data_TightMuPt_Subt[it_y] = (TH1D*)h_Data_TightMuPt[it_y]->Clone();
    h_Data_LooseMuEta_Subt[it_y] = (TH1D*)h_Data_LooseMuEta[it_y]->Clone();
    h_Data_TightMuEta_Subt[it_y] = (TH1D*)h_Data_TightMuEta[it_y]->Clone();
    double this_content_loose, this_content_tight;
    for(int it_mc=0; it_mc<MCNumber; it_mc++){
      h_MC_LooseMuPt_Norm[it_mc][it_y] = (TH1D*)h_MC_LooseMuPt[it_mc][it_y]->Clone();
      h_MC_TightMuPt_Norm[it_mc][it_y] = (TH1D*)h_MC_TightMuPt[it_mc][it_y]->Clone();
      for(int j=1; j<=2; j++){
        this_content_loose = h_MC_LooseMuPt[it_mc][it_y]->GetBinContent(j)*1.2555318;
        this_content_tight = h_MC_TightMuPt[it_mc][it_y]->GetBinContent(j)*1.2555318;
        h_MC_LooseMuPt_Norm[it_mc][it_y]->SetBinContent(j, this_content_loose);
        h_MC_TightMuPt_Norm[it_mc][it_y]->SetBinContent(j, this_content_tight);
      }
      for(int j=3; j<=8; j++){
        this_content_loose = h_MC_LooseMuPt[it_mc][it_y]->GetBinContent(j)*0.92947641;
        this_content_tight = h_MC_TightMuPt[it_mc][it_y]->GetBinContent(j)*0.92947641;
        h_MC_LooseMuPt_Norm[it_mc][it_y]->SetBinContent(j, this_content_loose);
        h_MC_TightMuPt_Norm[it_mc][it_y]->SetBinContent(j, this_content_tight);
      }
      h_Data_LooseMuPt_Subt[it_y]->Add(h_MC_LooseMuPt_Norm[it_mc][it_y],-1);
      h_Data_TightMuPt_Subt[it_y]->Add(h_MC_TightMuPt_Norm[it_mc][it_y],-1);
      h_Data_LooseMuEta_Subt[it_y]->Add(h_MC_LooseMuEta[it_mc][it_y],-1); // SSWW didn't do the normalization actually..
      h_Data_TightMuEta_Subt[it_y]->Add(h_MC_TightMuEta[it_mc][it_y],-1);
    }

    //for(int j=1; j<=8; j++){
    //  cout << "before : " << h_Data_LooseMuPt[it_y]->GetBinContent(j) << endl;
    //  cout << "MC : " << h_MC_LooseMuPt[0][it_y]->GetBinContent(j) << endl;
    //  cout << "MC_Norm : " << h_MC_LooseMuPt_Norm[0][it_y]->GetBinContent(j) << endl;
    //  cout << "after : " <<  h_Data_LooseMuPt_Subt[it_y]->GetBinContent(j) << endl;
    //}
    //for(int j=1; j<=5; j++){
    //  cout << "before : " << h_Data_LooseMuEta[it_y]->GetBinContent(j) << endl;
    //  cout << "MC : " << h_MC_LooseMuEta[0][it_y]->GetBinContent(j) << endl;
    //  cout << "after : " <<  h_Data_LooseMuEta_Subt[it_y]->GetBinContent(j) << endl;
    //  cout << "before : " << h_Data_TightMuEta[it_y]->GetBinContent(j) << endl;
    //  cout << "MC : " << h_MC_TightMuEta[0][it_y]->GetBinContent(j) << endl;
    //  cout << "after : " <<  h_Data_TightMuEta_Subt[it_y]->GetBinContent(j) << endl;
    //}

    TCanvas *c1 = new TCanvas();

    h_Data_TightMuPt[it_y]->Divide(h_Data_LooseMuPt[it_y]);
    h_Data_TightMuPt[it_y]->GetXaxis()->SetRangeUser(10.,50.);
    h_Data_TightMuPt[it_y]->GetYaxis()->SetRangeUser(0.,1.);
    h_Data_TightMuPt[it_y]->GetXaxis()->SetTitle("p_{T} (GeV)");
    h_Data_TightMuPt[it_y]->GetYaxis()->SetTitle("Fake rate");
    h_Data_TightMuPt[it_y]->SetLineColor(kBlack);
    h_Data_TightMuPt[it_y]->SetLineWidth(2);
    h_Data_TightMuPt[it_y]->SetStats(0);
    h_Data_TightMuPt[it_y]->Draw();
    h_Data_TightMuPt_Subt[it_y]->Divide(h_Data_LooseMuPt_Subt[it_y]);
    h_Data_TightMuPt_Subt[it_y]->SetLineColor(kRed);
    h_Data_TightMuPt_Subt[it_y]->SetLineWidth(2);
    h_Data_TightMuPt_Subt[it_y]->SetStats(0);
    h_Data_TightMuPt_Subt[it_y]->Draw("same");

    // Save the 1D plot
    gSystem->Exec("mkdir -p plots_Subt/");
    c1->SaveAs("./plots_Subt/FakeRate_Pt_SSWW_"+year.at(it_y)+".png");

    h_Data_TightMuEta[it_y]->Divide(h_Data_LooseMuEta[it_y]);
    h_Data_TightMuEta[it_y]->GetXaxis()->SetRangeUser(0.,2.5);
    h_Data_TightMuEta[it_y]->GetYaxis()->SetRangeUser(0.,1.);
    h_Data_TightMuEta[it_y]->GetXaxis()->SetTitle("#||{#eta}");
    h_Data_TightMuEta[it_y]->GetYaxis()->SetTitle("Fake rate");
    h_Data_TightMuEta[it_y]->SetLineColor(kBlack);
    h_Data_TightMuEta[it_y]->SetLineWidth(2);
    h_Data_TightMuEta[it_y]->SetStats(0);
    h_Data_TightMuEta[it_y]->Draw();
    h_Data_TightMuEta_Subt[it_y]->Divide(h_Data_LooseMuEta_Subt[it_y]);
    h_Data_TightMuEta_Subt[it_y]->SetLineColor(kRed);
    h_Data_TightMuEta_Subt[it_y]->SetLineWidth(2);
    h_Data_TightMuEta_Subt[it_y]->SetStats(0);
    h_Data_TightMuEta_Subt[it_y]->Draw("same");

    // Save the 1D plot
    gSystem->Exec("mkdir -p plots_Subt/");
    c1->SaveAs("./plots_Subt/FakeRate_Eta_SSWW_"+year.at(it_y)+".png");


    //============================== 2D ===============================//
    h_Data_LooseMu_Subt_tmp[it_y] = h_Data_LooseMu[it_y]->Clone();
    h_Data_TightMu_Subt_tmp[it_y] = h_Data_TightMu[it_y]->Clone();
    h_Data_LooseMu_Subt[it_y] = (TH2D*)h_Data_LooseMu_Subt_tmp[it_y];
    h_Data_TightMu_Subt[it_y] = (TH2D*)h_Data_TightMu_Subt_tmp[it_y];
    for(int it_mc=0; it_mc<MCNumber; it_mc++){
      h_MC_LooseMu_Norm_tmp[it_mc][it_y] = h_MC_LooseMu[it_mc][it_y]->Clone();
      h_MC_TightMu_Norm_tmp[it_mc][it_y] = h_MC_TightMu[it_mc][it_y]->Clone();
      h_MC_LooseMu_Norm[it_mc][it_y] = (TH2D*)h_MC_LooseMu_Norm_tmp[it_mc][it_y];
      h_MC_TightMu_Norm[it_mc][it_y] = (TH2D*)h_MC_TightMu_Norm_tmp[it_mc][it_y];
      for(int i=1; i<=2; i++){
        for(int j=1; j<=5; j++){
          this_content_loose = h_MC_LooseMu[it_mc][it_y]->GetBinContent(i,j)*1.2555318;
          this_content_tight = h_MC_TightMu[it_mc][it_y]->GetBinContent(i,j)*1.2555318;
          h_MC_LooseMu_Norm[it_mc][it_y]->SetBinContent(i, j, this_content_loose);
          h_MC_TightMu_Norm[it_mc][it_y]->SetBinContent(i, j, this_content_tight);
        }
      }
      for(int i=3; i<=8; i++){
        for(int j=1; j<=5; j++){
          this_content_loose = h_MC_LooseMu[it_mc][it_y]->GetBinContent(i,j)*0.92947641;
          this_content_tight = h_MC_TightMu[it_mc][it_y]->GetBinContent(i,j)*0.92947641;
          h_MC_LooseMu_Norm[it_mc][it_y]->SetBinContent(i, j, this_content_loose);
          h_MC_TightMu_Norm[it_mc][it_y]->SetBinContent(i, j, this_content_tight);
        }
      }
      h_Data_LooseMu_Subt[it_y]->Add(h_MC_LooseMu_Norm[it_mc][it_y],-1);
      h_Data_TightMu_Subt[it_y]->Add(h_MC_TightMu_Norm[it_mc][it_y],-1);
    }

    //for(int i=1; i<=6; i++){
    //  for(int j=1; j<=2; j++){
    //    cout << "before : " << h_Data_LooseMu[it_y]->GetBinContent(i,j) << endl;
    //    cout << "MC : " << h_MC_LooseMu[0][it_y]->GetBinContent(i,j) << endl;
    //    cout << "MC_Norm : " << h_MC_LooseMu_Norm[0][it_y]->GetBinContent(i,j) << endl;
    //    cout << "after : " <<  h_Data_LooseMu_Subt[it_y]->GetBinContent(i,j) << endl;
    //  }
    //}

    h_Data_TightMu_Subt[it_y]->Divide(h_Data_LooseMu_Subt[it_y]);

    // Save 2D histo
    TFile *f_out = new TFile("Muon_SSWW_FakeRates_2016.root","RECREATE");
    h_Data_TightMu_Subt[it_y]->Write("FR_2D");

  }  // year
}
