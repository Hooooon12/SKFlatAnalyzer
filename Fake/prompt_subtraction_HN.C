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

void prompt_subtraction_HN(){

  // Declare files and histos needed for making plots 
  TFile *f_Data[3], *f_Fake[3], *f_MC[MCNumber][3];
  TH1D *h_MC_LooseMuPtCone[MCNumber][3], *h_MC_LooseMuPtCone_Norm[MCNumber][3], *h_MC_TightMuPtCone[MCNumber][3], *h_MC_TightMuPtCone_Norm[MCNumber][3];
  TH1D *h_Data_LooseMuPtCone[3], *h_Data_TightMuPtCone[3], *h_Data_LooseMuPtCone_Subt[3], *h_Data_TightMuPtCone_Subt[3];
  TH1D *h_Data_LooseMuPtCone_Rebin[3], *h_Data_TightMuPtCone_Rebin[3], *h_Data_LooseMuPtCone_Subt_Rebin[3], *h_Data_TightMuPtCone_Subt_Rebin[3];
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
    h_Data_LooseMuPtCone[it_y] = (TH1D*)f_Data[it_y]->Get("Muon/HN/Muon_Loose_PtCone_HN");
    h_Data_TightMuPtCone[it_y] = (TH1D*)f_Data[it_y]->Get("Muon/HN/Muon_Tight_PtCone_HN");
    h_Data_LooseMu[it_y] = (TH2D*)f_Data[it_y]->Get("Muon/HN/Muon_Loose_HN");
    h_Data_TightMu[it_y] = (TH2D*)f_Data[it_y]->Get("Muon/HN/Muon_Tight_HN");
    // MC
    for(int it_mc=0; it_mc<MCNumber; it_mc++){
      h_MC_LooseMuPtCone[it_mc][it_y] = (TH1D*)f_MC[it_mc][it_y]->Get("Muon/HN/Muon_Loose_PtCone_HN");
      h_MC_TightMuPtCone[it_mc][it_y] = (TH1D*)f_MC[it_mc][it_y]->Get("Muon/HN/Muon_Tight_PtCone_HN");
      h_MC_LooseMu[it_mc][it_y] = (TH2D*)f_MC[it_mc][it_y]->Get("Muon/HN/Muon_Loose_HN");
      h_MC_TightMu[it_mc][it_y] = (TH2D*)f_MC[it_mc][it_y]->Get("Muon/HN/Muon_Tight_HN");
    }

    h_Data_LooseMuPtCone[it_y]->SetDirectory(0);
    h_Data_TightMuPtCone[it_y]->SetDirectory(0);
    h_Data_LooseMu[it_y]->SetDirectory(0);
    h_Data_TightMu[it_y]->SetDirectory(0);
    for(int it_mc=0; it_mc<MCNumber; it_mc++){
      if(h_MC_LooseMuPtCone[it_mc][it_y]) h_MC_LooseMuPtCone[it_mc][it_y]->SetDirectory(0);
      if(h_MC_TightMuPtCone[it_mc][it_y]) h_MC_TightMuPtCone[it_mc][it_y]->SetDirectory(0);
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

    // 1D, vs PtCone
    h_Data_LooseMuPtCone_Subt[it_y] = (TH1D*)h_Data_LooseMuPtCone[it_y]->Clone();
    h_Data_TightMuPtCone_Subt[it_y] = (TH1D*)h_Data_TightMuPtCone[it_y]->Clone();
    double this_content_loose, this_content_tight;
    for(int it_mc=0; it_mc<MCNumber; it_mc++){
      h_MC_LooseMuPtCone_Norm[it_mc][it_y] = (TH1D*)h_MC_LooseMuPtCone[it_mc][it_y]->Clone();
      h_MC_TightMuPtCone_Norm[it_mc][it_y] = (TH1D*)h_MC_TightMuPtCone[it_mc][it_y]->Clone();
      for(int j=1; j<=20; j++){
        //this_content_loose = h_MC_LooseMuPtCone[it_mc][it_y]->GetBinContent(j)*0.68615499;
        //this_content_tight = h_MC_TightMuPtCone[it_mc][it_y]->GetBinContent(j)*0.68615499;
        this_content_loose = h_MC_LooseMuPtCone[it_mc][it_y]->GetBinContent(j)*0.68299395;
        this_content_tight = h_MC_TightMuPtCone[it_mc][it_y]->GetBinContent(j)*0.68299395;
        h_MC_LooseMuPtCone_Norm[it_mc][it_y]->SetBinContent(j, this_content_loose);
        h_MC_TightMuPtCone_Norm[it_mc][it_y]->SetBinContent(j, this_content_tight);
      }
      for(int j=21; j<=30; j++){
        //this_content_loose = h_MC_LooseMuPtCone[it_mc][it_y]->GetBinContent(j)*1.2586214;
        //this_content_tight = h_MC_TightMuPtCone[it_mc][it_y]->GetBinContent(j)*1.2586214;
        this_content_loose = h_MC_LooseMuPtCone[it_mc][it_y]->GetBinContent(j)*1.2583334;
        this_content_tight = h_MC_TightMuPtCone[it_mc][it_y]->GetBinContent(j)*1.2583334;
        h_MC_LooseMuPtCone_Norm[it_mc][it_y]->SetBinContent(j, this_content_loose);
        h_MC_TightMuPtCone_Norm[it_mc][it_y]->SetBinContent(j, this_content_tight);
      }
      for(int j=31; j<=1000; j++){
        //this_content_loose = h_MC_LooseMuPtCone[it_mc][it_y]->GetBinContent(j)*0.93185204;
        //this_content_tight = h_MC_TightMuPtCone[it_mc][it_y]->GetBinContent(j)*0.93185204;
        this_content_loose = h_MC_LooseMuPtCone[it_mc][it_y]->GetBinContent(j)*0.93185908;
        this_content_tight = h_MC_TightMuPtCone[it_mc][it_y]->GetBinContent(j)*0.93185908;
        h_MC_LooseMuPtCone_Norm[it_mc][it_y]->SetBinContent(j, this_content_loose);
        h_MC_TightMuPtCone_Norm[it_mc][it_y]->SetBinContent(j, this_content_tight);
      }
      h_Data_LooseMuPtCone_Subt[it_y]->Add(h_MC_LooseMuPtCone_Norm[it_mc][it_y],-1);
      h_Data_TightMuPtCone_Subt[it_y]->Add(h_MC_TightMuPtCone_Norm[it_mc][it_y],-1);
    }

    //for(int j=10; j<=30; j++){
    //  cout << "=============================" << endl;
    //  cout << "before : " << h_Data_LooseMuPtCone[it_y]->GetBinContent(j) << endl;
    //  cout << "1st MC : " << h_MC_LooseMuPtCone[0][it_y]->GetBinContent(j) << endl;
    //  cout << "1st MC_Norm : " << h_MC_LooseMuPtCone_Norm[0][it_y]->GetBinContent(j) << endl;
    //  //cout << "MC_Norm : " << h_MC_LooseMuPtCone_Norm[1][it_y]->GetBinContent(j) << endl;
    //  //cout << "MC_Norm : " << h_MC_LooseMuPtCone_Norm[2][it_y]->GetBinContent(j) << endl;
    //  //cout << "MC_Norm : " << h_MC_LooseMuPtCone_Norm[3][it_y]->GetBinContent(j) << endl;
    //  //cout << "MC_Norm : " << h_MC_LooseMuPtCone_Norm[4][it_y]->GetBinContent(j) << endl;
    //  //cout << "MC_Norm : " << h_MC_LooseMuPtCone_Norm[5][it_y]->GetBinContent(j) << endl;
    //  //cout << "MC_Norm : " << h_MC_LooseMuPtCone_Norm[6][it_y]->GetBinContent(j) << endl;
    //  //cout << "MC_Norm : " << h_MC_LooseMuPtCone_Norm[7][it_y]->GetBinContent(j) << endl;
    //  //cout << "MC_Norm : " << h_MC_LooseMuPtCone_Norm[8][it_y]->GetBinContent(j) << endl;
    //  //cout << "MC_Norm : " << h_MC_LooseMuPtCone_Norm[9][it_y]->GetBinContent(j) << endl;
    //  //cout << "MC_Norm : " << h_MC_LooseMuPtCone_Norm[10][it_y]->GetBinContent(j) << endl;
    //  //cout << "MC_Norm : " << h_MC_LooseMuPtCone_Norm[11][it_y]->GetBinContent(j) << endl;
    //  cout << "after : " <<  h_Data_LooseMuPtCone_Subt[it_y]->GetBinContent(j) << endl;
    //}

    TCanvas *c1 = new TCanvas();

    int Nbin = 6;
    double xbins[7] = {10., 15., 20., 30., 40., 50., 60.};
    h_Data_TightMuPtCone_Rebin[it_y] = (TH1D*)h_Data_TightMuPtCone[it_y]->Rebin(Nbin,"",xbins);
    h_Data_LooseMuPtCone_Rebin[it_y] = (TH1D*)h_Data_LooseMuPtCone[it_y]->Rebin(Nbin,"",xbins);
    h_Data_TightMuPtCone_Rebin[it_y]->Divide(h_Data_LooseMuPtCone_Rebin[it_y]);
    h_Data_TightMuPtCone_Rebin[it_y]->GetXaxis()->SetRangeUser(10.,60.);
    h_Data_TightMuPtCone_Rebin[it_y]->GetYaxis()->SetRangeUser(0.,0.4);
    h_Data_TightMuPtCone_Rebin[it_y]->GetXaxis()->SetTitle("p_{T}^{cone} (GeV)");
    h_Data_TightMuPtCone_Rebin[it_y]->GetYaxis()->SetTitle("Fake rate");
    h_Data_TightMuPtCone_Rebin[it_y]->SetLineColor(kRed);
    h_Data_TightMuPtCone_Rebin[it_y]->SetLineWidth(2);
    h_Data_TightMuPtCone_Rebin[it_y]->SetStats(0);
    h_Data_TightMuPtCone_Rebin[it_y]->Draw();
    h_Data_TightMuPtCone_Subt_Rebin[it_y] = (TH1D*)h_Data_TightMuPtCone_Subt[it_y]->Rebin(Nbin,"",xbins);
    h_Data_LooseMuPtCone_Subt_Rebin[it_y] = (TH1D*)h_Data_LooseMuPtCone_Subt[it_y]->Rebin(Nbin,"",xbins);
    h_Data_TightMuPtCone_Subt_Rebin[it_y]->Divide(h_Data_LooseMuPtCone_Subt_Rebin[it_y]);
    h_Data_TightMuPtCone_Subt_Rebin[it_y]->SetLineColor(kBlack);
    h_Data_TightMuPtCone_Subt_Rebin[it_y]->SetLineWidth(2);
    h_Data_TightMuPtCone_Subt_Rebin[it_y]->SetStats(0);
    h_Data_TightMuPtCone_Subt_Rebin[it_y]->Draw("same");

    // Save the 1D plot
    gSystem->Exec("mkdir -p plots_Subt/");
    c1->SaveAs("./plots_Subt/FakeRate_PtCone_HNTightV1_"+year.at(it_y)+".png");


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
          //this_content_loose = h_MC_LooseMu[it_mc][it_y]->GetBinContent(i,j)*0.68615499;
          //this_content_tight = h_MC_TightMu[it_mc][it_y]->GetBinContent(i,j)*0.68615499;
          this_content_loose = h_MC_LooseMu[it_mc][it_y]->GetBinContent(i,j)*0.68299395;
          this_content_tight = h_MC_TightMu[it_mc][it_y]->GetBinContent(i,j)*0.68299395;
          h_MC_LooseMu_Norm[it_mc][it_y]->SetBinContent(i, j, this_content_loose);
          h_MC_TightMu_Norm[it_mc][it_y]->SetBinContent(i, j, this_content_tight);
        }
      }
      for(int i=3; i<=4; i++){
        for(int j=1; j<=5; j++){
          //this_content_loose = h_MC_LooseMu[it_mc][it_y]->GetBinContent(i,j)*1.2586214;
          //this_content_tight = h_MC_TightMu[it_mc][it_y]->GetBinContent(i,j)*1.2586214;
          this_content_loose = h_MC_LooseMu[it_mc][it_y]->GetBinContent(i,j)*1.2583334;
          this_content_tight = h_MC_TightMu[it_mc][it_y]->GetBinContent(i,j)*1.2583334;
          h_MC_LooseMu_Norm[it_mc][it_y]->SetBinContent(i, j, this_content_loose);
          h_MC_TightMu_Norm[it_mc][it_y]->SetBinContent(i, j, this_content_tight);
        }
      }
      for(int i=5; i<=10; i++){
        for(int j=1; j<=5; j++){
          //this_content_loose = h_MC_LooseMu[it_mc][it_y]->GetBinContent(i,j)*0.93185204;
          //this_content_tight = h_MC_TightMu[it_mc][it_y]->GetBinContent(i,j)*0.93185204;
          this_content_loose = h_MC_LooseMu[it_mc][it_y]->GetBinContent(i,j)*0.93185908;
          this_content_tight = h_MC_TightMu[it_mc][it_y]->GetBinContent(i,j)*0.93185908;
          h_MC_LooseMu_Norm[it_mc][it_y]->SetBinContent(i, j, this_content_loose);
          h_MC_TightMu_Norm[it_mc][it_y]->SetBinContent(i, j, this_content_tight);
        }
      }
      h_Data_LooseMu_Subt[it_y]->Add(h_MC_LooseMu_Norm[it_mc][it_y],-1);
      h_Data_TightMu_Subt[it_y]->Add(h_MC_TightMu_Norm[it_mc][it_y],-1);
    }

    for(int i=1; i<=4; i++){
      for(int j=1; j<=5; j++){
        cout << "==================================" << endl;
        cout << "in ptcone " << h_Data_LooseMu[it_y]->GetXaxis()->GetBinLowEdge(i) << "~" << h_Data_LooseMu[it_y]->GetXaxis()->GetBinLowEdge(i+1) << "GeV, abseta " << h_Data_LooseMu[it_y]->GetYaxis()->GetBinLowEdge(j) << "~" << h_Data_LooseMu[it_y]->GetYaxis()->GetBinLowEdge(j+1) << ";" << endl;
        cout << "//Loose//" << endl;
        cout << "before : " << h_Data_LooseMu[it_y]->GetBinContent(i,j) << endl;
        cout << "1st MC : " << h_MC_LooseMu[0][it_y]->GetBinContent(i,j) << endl;
        cout << "1st MC_Norm : " << h_MC_LooseMu_Norm[0][it_y]->GetBinContent(i,j) << endl;
        cout << "after : " <<  h_Data_LooseMu_Subt[it_y]->GetBinContent(i,j) << endl;
        cout << "//Tight//" << endl;
        cout << "before : " << h_Data_TightMu[it_y]->GetBinContent(i,j) << endl;
        cout << "1st MC : " << h_MC_TightMu[0][it_y]->GetBinContent(i,j) << endl;
        cout << "1st MC_Norm : " << h_MC_TightMu_Norm[0][it_y]->GetBinContent(i,j) << endl;
        cout << "after : " <<  h_Data_TightMu_Subt[it_y]->GetBinContent(i,j) << endl;
      }
    }

    h_Data_TightMu_Subt[it_y]->Divide(h_Data_LooseMu_Subt[it_y]);

    // Save 2D histo
    TFile *f_out = new TFile("Muon_HN_FakeRates_HNTightV1_2016.root","RECREATE");
    h_Data_TightMu_Subt[it_y]->Write("FR_2D");

  }  // year
}
