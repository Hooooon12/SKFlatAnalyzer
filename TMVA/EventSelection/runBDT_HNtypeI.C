void runBDT_HNtypeI(TString channel, int mass, TString bkgType, TString era, TString BDTMethod, int NTrees, int NCuts, int MaxDepth){
//void runBDT_HNtypeI(TString channel, int mass, TString era, bool isVBF, int MET_mode, int Nb_mode, int Nj_mode=0){

  TString analyzer = "HNL_SR3_BDT_KinVar";
  TString mass_txt = TString::Itoa(mass, 10);
  TString treeName = "";
  TString NTrees_txt = TString::Itoa(NTrees, 10);
  TString NCuts_txt = TString::Itoa(NCuts, 10);
  TString MaxDepth_txt = TString::Itoa(MaxDepth, 10);
  //TString MET_mode_txt = TString::Itoa(MET_mode, 10);
  //TString Nb_mode_txt = TString::Itoa(Nb_mode, 10);
  //TString Nj_mode_txt = TString::Itoa(Nj_mode, 10);
  //TString signalName = "DY";
  //if(mass==500 && isVBF) signalName = "DYVBF";

  TString bkgName = "", outBkgName = "";
  if(bkgType == "Fake"){
    outBkgName = "Fake";
    bkgName = "MCFake_SSOS"; // MCFake_SSOS, MCFake_SS
  }
  if(bkgType == "NonFake"){
    outBkgName = "NonFake";
    if(channel == "EE") bkgName = "NonFake_EE";
    else bkgName = "NonFake_MuMu";
  }
  if(bkgType == "Incl"){
    outBkgName = "Inclusive";
    if(channel == "EE") bkgName = "AllBkg_EE";
    else bkgName = "AllBkg_MuMu";
  }


  //TMVA::gConfig().GetVariablePlotting().fNbinsMVAoutput = 200;
  TMVA::gConfig().GetVariablePlotting().fNbins1D = 500; 

  const TString path = "/data6/Users/helee/working_HN_Plotter/Run2UltraLegacy_v3/"+analyzer+"/"+era+"/";

  TFile* fsin = TFile::Open(path+analyzer+"_DYTypeI_M"+mass_txt+".root");
  TFile* fbin = TFile::Open(path+analyzer+"_"+bkgName+".root");

  if(channel == "MuMu") treeName = "Tree_mm";
  if(channel == "EE") treeName = "Tree_ee";
  if(channel == "EMu") treeName = "Tree_em";

  TTree* tree_signal = (TTree*)fsin->Get(treeName);
  TTree* tree_bkg    = (TTree*)fbin->Get(treeName);

  //TString fout_name = ("output_"+signalName+"TypeI_"+channel+"_M"+mass_txt+"_Mode"+MET_mode_txt+Nb_mode_txt+Nj_mode_txt+"_"+era+"_BDT.root");
  TString fout_name = ("output_DYTypeI_"+channel+"_M"+mass_txt+"_"+outBkgName+"_"+era+"_NTrees"+NTrees_txt+"_NCuts"+NCuts_txt+"_MaxDepth"+MaxDepth_txt+"_"+BDTMethod+".root");

  TFile* fout = TFile::Open(fout_name, "RECREATE");

  TMVA::Factory* factory = new TMVA::Factory("TMVAClassification", fout,
                                             "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification");

  //TString name_data_loader;
  //if(!chk_pre_kin) name_data_loader = Form("%dJets", n_jet);
  //else name_data_loader = Form("Pre_Kin_%dJets", n_jet);

  TMVA::DataLoader* data_loader = new TMVA::DataLoader("dataset");
  //data_loader->AddVariable( "weight",       "weight",       "units", 'F');
  //data_loader->AddVariable( "n_jets",       "n_jets",       "units", 'I');

  //(usejets){
  //==== Variables : Number of jets (NVar = 2)
  //////data_loader->AddVariable("Nj", "Nj", "units", 'I'); 
  //data_loader->AddVariable("Nvbfj", "Nvbfj", "units", 'I');
  //data_loader->AddVariable("Nb", "Nb", "units", 'I');

  //==== Variables : Pt(l), HT, MET, Eta(l) (NVar = 9)
  //data_loader->AddVariable("Ptl1", "Ptl1", "units", 'F');
  //data_loader->AddVariable("Ptl2", "Ptl2", "units", 'F');
  //data_loader->AddVariable("Ptj1", "Ptj1", "units", 'F');
  //data_loader->AddVariable("Ptj2", "Ptj2", "units", 'F');
  //////data_loader->AddVariable("Ptj3", "Ptj3", "units", 'F');
  //////data_loader->AddVariable("MET", "MET", "units", 'F');
  //data_loader->AddVariable("MET2ST", "MET2ST", "units", 'F');
  //////data_loader->AddVariable("HT", "HT", "units", 'F');          
  //////data_loader->AddVariable("LT", "LT", "units", 'F');
  //////data_loader->AddVariable("HTLT", "HTLT", "units", 'F');
  //data_loader->AddVariable("HTLT1", "HTLT1", "units", 'F');
  //data_loader->AddVariable("HTLT2", "HTLT2", "units", 'F');
  //////data_loader->AddVariable("MET2HT", "MET2HT", "units", 'F');
  //data_loader->AddVariable("Etal1", "Etal1", "units", 'F');
  //data_loader->AddVariable("Etal2", "Etal2", "units", 'F');

  //==== Variables regarding away jets (NVar = 14)
  //////data_loader->AddVariable("PtRatiol1", "PtRatiol1", "units", 'F');
  //////data_loader->AddVariable("PtRatiol2", "PtRatiol2", "units", 'F');
  //data_loader->AddVariable("PtRatioAJl1", "PtRatioAJl1", "units", 'F');
  //data_loader->AddVariable("PtRatioAJl2", "PtRatioAJl2", "units", 'F');
  //////data_loader->AddVariable("PtRelv0l1", "PtRelv0l1", "units", 'F');
  //////data_loader->AddVariable("PtRelv0l2", "PtRelv0l2", "units", 'F');
  //////data_loader->AddVariable("PtRelv1l1", "PtRelv1l1", "units", 'F');
  //////data_loader->AddVariable("PtRelv1l2", "PtRelv1l2", "units", 'F');
  /*data_loader->AddVariable("CEMFracAJl1", "CEMFracAJl1", "units", 'F');
  data_loader->AddVariable("CEMFracAJl2", "CEMFracAJl2", "units", 'F');
  data_loader->AddVariable("NEMFracAJl1", "NEMFracAJl1", "units", 'F');
  data_loader->AddVariable("NEMFracAJl2", "NEMFracAJl2", "units", 'F');
  data_loader->AddVariable("CHFracAJl1", "CHFracAJl1", "units", 'F');
  data_loader->AddVariable("CHFracAJl2", "CHFracAJl2", "units", 'F');
  data_loader->AddVariable("NHFracAJl1", "NHFracAJl1", "units", 'F');
  data_loader->AddVariable("NHFracAJl2", "NHFracAJl2", "units", 'F');
  data_loader->AddVariable("MuFracAJl1", "MuFracAJl1", "units", 'F');
  data_loader->AddVariable("MuFracAJl2", "MuFracAJl2", "units", 'F');
  data_loader->AddVariable("JetDiscAJl1", "JetDiscAJl1", "units", 'F');
  data_loader->AddVariable("JetDiscAJl2", "JetDiscAJl2", "units", 'F');*/
  /*data_loader->AddVariable("CEMFracCJl1", "CEMFracCJl1", "units", 'F');
  data_loader->AddVariable("CEMFracCJl2", "CEMFracCJl2", "units", 'F');
  data_loader->AddVariable("NEMFracCJl1", "NEMFracCJl1", "units", 'F');
  data_loader->AddVariable("NEMFracCJl2", "NEMFracCJl2", "units", 'F');
  data_loader->AddVariable("CHFracCJl1", "CHFracCJl1", "units", 'F');
  data_loader->AddVariable("CHFracCJl2", "CHFracCJl2", "units", 'F');
  data_loader->AddVariable("NHFracCJl1", "NHFracCJl1", "units", 'F');
  data_loader->AddVariable("NHFracCJl2", "NHFracCJl2", "units", 'F');
  data_loader->AddVariable("MuFracCJl1", "MuFracCJl1", "units", 'F');
  data_loader->AddVariable("MuFracCJl2", "MuFracCJl2", "units", 'F');
  data_loader->AddVariable("JetDiscCJl1", "JetDiscCJl1", "units", 'F');
  data_loader->AddVariable("JetDiscCJl2", "JetDiscCJl2", "units", 'F');*/

  //==== Variables : dEta, dR, invariant masses, MT (NVar = 14)
  //////data_loader->AddVariable("dEtall", "dEtall", "units", 'F');
  //data_loader->AddVariable("dRll", "dRll", "units", 'F');
  //////data_loader->AddVariable("dRjj12", "dRjj12", "units", 'F');
  //////data_loader->AddVariable("dRjj23", "dRjj23", "units", 'F');
  //////data_loader->AddVariable("dRjj13", "dRjj13", "units", 'F');
  //data_loader->AddVariable("dRlj11", "dRlj11", "units", 'F');
  //data_loader->AddVariable("dRlj12", "dRlj12", "units", 'F');
  //////data_loader->AddVariable("dRlj13", "dRlj13", "units", 'F');
  //data_loader->AddVariable("dRlj21", "dRlj21", "units", 'F');
  //data_loader->AddVariable("dRlj22", "dRlj22", "units", 'F');
  //////data_loader->AddVariable("dRlj23", "dRlj23", "units", 'F');
  //data_loader->AddVariable("MSSSF", "MSSSF", "units", 'F');
  //data_loader->AddVariable("Mlj11", "Mlj11", "units", 'F');
  //data_loader->AddVariable("Mlj12", "Mlj12", "units", 'F');
  //////data_loader->AddVariable("Mlj13", "Mlj13", "units", 'F');
  //data_loader->AddVariable("Mlj21", "Mlj21", "units", 'F');
  //data_loader->AddVariable("Mlj22", "Mlj22", "units", 'F');
  //////data_loader->AddVariable("Mlj23", "Mlj23", "units", 'F');
  //data_loader->AddVariable("MTvl1", "MTvl1", "units", 'F');
  //data_loader->AddVariable("MTvl2", "MTvl2", "units", 'F');
  //data_loader->AddVariable("Mllj1", "Mllj1", "units", 'F');
  //data_loader->AddVariable("Mllj2", "Mllj2", "units", 'F');
  /*data_loader->AddVariable("Mllj3", "Mllj3", "units", 'F');
  data_loader->AddVariable("Mllj4", "Mllj4", "units", 'F');
  data_loader->AddVariable("Mlljj12", "Mlljj12", "units", 'F');
  data_loader->AddVariable("Mlljj13", "Mlljj13", "units", 'F');
  data_loader->AddVariable("Mlljj14", "Mlljj14", "units", 'F');
  data_loader->AddVariable("Mlljj23", "Mlljj23", "units", 'F');
  data_loader->AddVariable("Mlljj24", "Mlljj24", "units", 'F');
  data_loader->AddVariable("Mlljj34", "Mlljj34", "units", 'F');
  data_loader->AddVariable("Mljj112", "Mljj112", "units", 'F');
  data_loader->AddVariable("Mljj113", "Mljj113", "units", 'F');
  data_loader->AddVariable("Mljj114", "Mljj114", "units", 'F');
  data_loader->AddVariable("Mljj123", "Mljj123", "units", 'F');
  data_loader->AddVariable("Mljj124", "Mljj124", "units", 'F');
  data_loader->AddVariable("Mljj134", "Mljj134", "units", 'F');
  data_loader->AddVariable("Mljj212", "Mljj212", "units", 'F');
  data_loader->AddVariable("Mljj213", "Mljj213", "units", 'F');
  data_loader->AddVariable("Mljj214", "Mljj214", "units", 'F');
  data_loader->AddVariable("Mljj223", "Mljj223", "units", 'F');
  data_loader->AddVariable("Mljj224", "Mljj224", "units", 'F');
  data_loader->AddVariable("Mljj234", "Mljj234", "units", 'F');
  data_loader->AddVariable("Mjj12", "Mjj12", "units", 'F');
  data_loader->AddVariable("Mjj13", "Mjj13", "units", 'F');
  data_loader->AddVariable("Mjj14", "Mjj14", "units", 'F');
  data_loader->AddVariable("Mjj23", "Mjj23", "units", 'F');
  data_loader->AddVariable("Mjj24", "Mjj24", "units", 'F');
  data_loader->AddVariable("Mjj34", "Mjj34", "units", 'F');*/ 

  //==== Variables using jets whose mass is closest to mW (NVar = 11)
  //if(Nj_mode != 1){
    /*data_loader->AddVariable("PtWj1", "PtWj1", "units", 'F');
    data_loader->AddVariable("PtWj2", "PtWj2", "units", 'F');
    data_loader->AddVariable("dRWjj", "dRWjj", "units", 'F');
    data_loader->AddVariable("dRlW12", "dRlW12", "units", 'F');
    data_loader->AddVariable("dRlW22", "dRlW22", "units", 'F');
    data_loader->AddVariable("dRlN12", "dRlN12", "units", 'F');
    data_loader->AddVariable("dRlN21", "dRlN21", "units", 'F');
    data_loader->AddVariable("M_W2_jj", "M_W2_jj", "units", 'F');
    data_loader->AddVariable("M_W1_lljj", "M_W1_lljj", "units", 'F');
    data_loader->AddVariable("M_N1_l1jj", "M_N1_l1jj", "units", 'F');
    data_loader->AddVariable("M_N2_l2jj", "M_N2_l2jj", "units", 'F');*/
  //}

  //==== NVar = 50

  /*data_loader->AddVariable("Nvbfj", "Nvbfj", "units", 'I');
  data_loader->AddVariable("Nb", "Nb", "units", 'I');
  data_loader->AddVariable("Ptl1", "Ptl1", "units", 'F');
  data_loader->AddVariable("Ptl2", "Ptl2", "units", 'F');
  data_loader->AddVariable("Ptj1", "Ptj1", "units", 'F');
  data_loader->AddVariable("Ptj2", "Ptj2", "units", 'F');
  data_loader->AddVariable("MET2ST", "MET2ST", "units", 'F');
  data_loader->AddVariable("HTLT1", "HTLT1", "units", 'F');
  data_loader->AddVariable("HTLT2", "HTLT2", "units", 'F');
  data_loader->AddVariable("Etal1", "Etal1", "units", 'F');
  data_loader->AddVariable("Etal2", "Etal2", "units", 'F');
  data_loader->AddVariable("PtRatioAJl1", "PtRatioAJl1", "units", 'F');
  data_loader->AddVariable("PtRatioAJl2", "PtRatioAJl2", "units", 'F');
  data_loader->AddVariable("CEMFracAJl1", "CEMFracAJl1", "units", 'F');
  data_loader->AddVariable("CEMFracAJl2", "CEMFracAJl2", "units", 'F');
  data_loader->AddVariable("NEMFracAJl1", "NEMFracAJl1", "units", 'F');
  data_loader->AddVariable("NEMFracAJl2", "NEMFracAJl2", "units", 'F');
  data_loader->AddVariable("CHFracAJl1", "CHFracAJl1", "units", 'F');
  data_loader->AddVariable("CHFracAJl2", "CHFracAJl2", "units", 'F');
  data_loader->AddVariable("NHFracAJl1", "NHFracAJl1", "units", 'F');
  data_loader->AddVariable("NHFracAJl2", "NHFracAJl2", "units", 'F');
  data_loader->AddVariable("MuFracAJl1", "MuFracAJl1", "units", 'F');
  data_loader->AddVariable("MuFracAJl2", "MuFracAJl2", "units", 'F');
  data_loader->AddVariable("JetDiscAJl1", "JetDiscAJl1", "units", 'F');
  data_loader->AddVariable("JetDiscAJl2", "JetDiscAJl2", "units", 'F');
  data_loader->AddVariable("dRll", "dRll", "units", 'F');
  data_loader->AddVariable("dRlj11", "dRlj11", "units", 'F');
  data_loader->AddVariable("dRlj12", "dRlj12", "units", 'F');
  data_loader->AddVariable("dRlj21", "dRlj21", "units", 'F');
  data_loader->AddVariable("dRlj22", "dRlj22", "units", 'F');
  data_loader->AddVariable("MSSSF", "MSSSF", "units", 'F');
  data_loader->AddVariable("Mlj11", "Mlj11", "units", 'F');
  data_loader->AddVariable("Mlj12", "Mlj12", "units", 'F');
  data_loader->AddVariable("Mlj21", "Mlj21", "units", 'F');
  data_loader->AddVariable("Mlj22", "Mlj22", "units", 'F');
  data_loader->AddVariable("MTvl1", "MTvl1", "units", 'F');
  data_loader->AddVariable("MTvl2", "MTvl2", "units", 'F');
  data_loader->AddVariable("Mllj1", "Mllj1", "units", 'F');
  data_loader->AddVariable("Mllj2", "Mllj2", "units", 'F');
  data_loader->AddVariable("PtWj1", "PtWj1", "units", 'F');
  data_loader->AddVariable("PtWj2", "PtWj2", "units", 'F');
  data_loader->AddVariable("dRWjj", "dRWjj", "units", 'F');
  data_loader->AddVariable("dRlW12", "dRlW12", "units", 'F');
  data_loader->AddVariable("dRlW22", "dRlW22", "units", 'F');
  data_loader->AddVariable("dRlN12", "dRlN12", "units", 'F');
  data_loader->AddVariable("dRlN21", "dRlN21", "units", 'F');
  data_loader->AddVariable("M_W2_jj", "M_W2_jj", "units", 'F');
  data_loader->AddVariable("M_W1_lljj", "M_W1_lljj", "units", 'F');
  data_loader->AddVariable("M_N1_l1jj", "M_N1_l1jj", "units", 'F');
  data_loader->AddVariable("M_N2_l2jj", "M_N2_l2jj", "units", 'F');*/

  //==== NVar = 35

  if(channel == "MuMu"){

    if(bkgType == "Fake"){

      data_loader->AddVariable("Nvbfj", "Nvbfj", "units", 'I');
      data_loader->AddVariable("Nb", "Nb", "units", 'I');
      data_loader->AddVariable("Ptl1", "Ptl1", "units", 'F');
      data_loader->AddVariable("Ptl2", "Ptl2", "units", 'F');
      data_loader->AddVariable("Ptj1", "Ptj1", "units", 'F');
      //data_loader->AddVariable("Ptj2", "Ptj2", "units", 'F');
      data_loader->AddVariable("MET2ST", "MET2ST", "units", 'F');
      data_loader->AddVariable("HTLT1", "HTLT1", "units", 'F');
      data_loader->AddVariable("HTLT2", "HTLT2", "units", 'F');
      data_loader->AddVariable("Etal1", "Etal1", "units", 'F');
      data_loader->AddVariable("Etal2", "Etal2", "units", 'F');
      data_loader->AddVariable("PtRatioAJl1", "PtRatioAJl1", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl2", "PtRatioAJl2", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl1", "CEMFracAJl1", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl2", "CEMFracAJl2", "units", 'F');
      //data_loader->AddVariable("NEMFracAJl1", "NEMFracAJl1", "units", 'F');
      data_loader->AddVariable("NEMFracAJl2", "NEMFracAJl2", "units", 'F');
      data_loader->AddVariable("CHFracAJl1", "CHFracAJl1", "units", 'F');
      data_loader->AddVariable("CHFracAJl2", "CHFracAJl2", "units", 'F');
      data_loader->AddVariable("NHFracAJl1", "NHFracAJl1", "units", 'F');
      data_loader->AddVariable("NHFracAJl2", "NHFracAJl2", "units", 'F');
      //data_loader->AddVariable("MuFracAJl1", "MuFracAJl1", "units", 'F');
      //data_loader->AddVariable("MuFracAJl2", "MuFracAJl2", "units", 'F');
      data_loader->AddVariable("JetDiscAJl1", "JetDiscAJl1", "units", 'F');
      data_loader->AddVariable("JetDiscAJl2", "JetDiscAJl2", "units", 'F');
      data_loader->AddVariable("dRll", "dRll", "units", 'F');
      data_loader->AddVariable("dRlj11", "dRlj11", "units", 'F');
      data_loader->AddVariable("dRlj12", "dRlj12", "units", 'F');
      data_loader->AddVariable("dRlj21", "dRlj21", "units", 'F');
      data_loader->AddVariable("dRlj22", "dRlj22", "units", 'F');
      data_loader->AddVariable("MSSSF", "MSSSF", "units", 'F');
      data_loader->AddVariable("Mlj11", "Mlj11", "units", 'F');
      data_loader->AddVariable("Mlj12", "Mlj12", "units", 'F');
      data_loader->AddVariable("Mlj21", "Mlj21", "units", 'F');
      data_loader->AddVariable("Mlj22", "Mlj22", "units", 'F');
      data_loader->AddVariable("MTvl1", "MTvl1", "units", 'F');
      data_loader->AddVariable("MTvl2", "MTvl2", "units", 'F');
      data_loader->AddVariable("Mllj1", "Mllj1", "units", 'F');
      //data_loader->AddVariable("Mllj2", "Mllj2", "units", 'F');
      //data_loader->AddVariable("PtWj1", "PtWj1", "units", 'F');
      //data_loader->AddVariable("PtWj2", "PtWj2", "units", 'F');
      data_loader->AddVariable("dRWjj", "dRWjj", "units", 'F');
      //data_loader->AddVariable("dRlW12", "dRlW12", "units", 'F');
      //data_loader->AddVariable("dRlW22", "dRlW22", "units", 'F');
      //data_loader->AddVariable("dRlN12", "dRlN12", "units", 'F');
      //data_loader->AddVariable("dRlN21", "dRlN21", "units", 'F');
      data_loader->AddVariable("M_W2_jj", "M_W2_jj", "units", 'F');
      //data_loader->AddVariable("M_W1_lljj", "M_W1_lljj", "units", 'F');
      data_loader->AddVariable("M_N1_l1jj", "M_N1_l1jj", "units", 'F');
      data_loader->AddVariable("M_N2_l2jj", "M_N2_l2jj", "units", 'F');

    }
    else if(bkgType == "NonFake"){

      data_loader->AddVariable("Nvbfj", "Nvbfj", "units", 'I');
      data_loader->AddVariable("Nb", "Nb", "units", 'I');
      data_loader->AddVariable("Ptl1", "Ptl1", "units", 'F');
      data_loader->AddVariable("Ptl2", "Ptl2", "units", 'F');
      data_loader->AddVariable("Ptj1", "Ptj1", "units", 'F');
      data_loader->AddVariable("Ptj2", "Ptj2", "units", 'F');
      data_loader->AddVariable("MET2ST", "MET2ST", "units", 'F');
      data_loader->AddVariable("HTLT1", "HTLT1", "units", 'F');
      data_loader->AddVariable("HTLT2", "HTLT2", "units", 'F');
      data_loader->AddVariable("Etal1", "Etal1", "units", 'F');
      data_loader->AddVariable("Etal2", "Etal2", "units", 'F');
      data_loader->AddVariable("PtRatioAJl1", "PtRatioAJl1", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl2", "PtRatioAJl2", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl1", "CEMFracAJl1", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl2", "CEMFracAJl2", "units", 'F');
      data_loader->AddVariable("NEMFracAJl1", "NEMFracAJl1", "units", 'F');
      //data_loader->AddVariable("NEMFracAJl2", "NEMFracAJl2", "units", 'F');
      data_loader->AddVariable("CHFracAJl1", "CHFracAJl1", "units", 'F');
      data_loader->AddVariable("CHFracAJl2", "CHFracAJl2", "units", 'F');
      //data_loader->AddVariable("NHFracAJl1", "NHFracAJl1", "units", 'F');
      //data_loader->AddVariable("NHFracAJl2", "NHFracAJl2", "units", 'F');
      data_loader->AddVariable("MuFracAJl1", "MuFracAJl1", "units", 'F');
      data_loader->AddVariable("MuFracAJl2", "MuFracAJl2", "units", 'F');
      data_loader->AddVariable("JetDiscAJl1", "JetDiscAJl1", "units", 'F');
      data_loader->AddVariable("JetDiscAJl2", "JetDiscAJl2", "units", 'F');
      data_loader->AddVariable("dRll", "dRll", "units", 'F');
      data_loader->AddVariable("dRlj11", "dRlj11", "units", 'F');
      //data_loader->AddVariable("dRlj12", "dRlj12", "units", 'F');
      data_loader->AddVariable("dRlj21", "dRlj21", "units", 'F');
      //data_loader->AddVariable("dRlj22", "dRlj22", "units", 'F');
      data_loader->AddVariable("MSSSF", "MSSSF", "units", 'F');
      data_loader->AddVariable("Mlj11", "Mlj11", "units", 'F');
      //data_loader->AddVariable("Mlj12", "Mlj12", "units", 'F');
      data_loader->AddVariable("Mlj21", "Mlj21", "units", 'F');
      data_loader->AddVariable("Mlj22", "Mlj22", "units", 'F');
      data_loader->AddVariable("MTvl1", "MTvl1", "units", 'F');
      data_loader->AddVariable("MTvl2", "MTvl2", "units", 'F');
      data_loader->AddVariable("Mllj1", "Mllj1", "units", 'F');
      //data_loader->AddVariable("Mllj2", "Mllj2", "units", 'F');
      //data_loader->AddVariable("PtWj1", "PtWj1", "units", 'F');
      //data_loader->AddVariable("PtWj2", "PtWj2", "units", 'F');
      data_loader->AddVariable("dRWjj", "dRWjj", "units", 'F');
      //data_loader->AddVariable("dRlW12", "dRlW12", "units", 'F');
      //data_loader->AddVariable("dRlW22", "dRlW22", "units", 'F');
      data_loader->AddVariable("dRlN12", "dRlN12", "units", 'F');
      data_loader->AddVariable("dRlN21", "dRlN21", "units", 'F');
      data_loader->AddVariable("M_W2_jj", "M_W2_jj", "units", 'F');
      //data_loader->AddVariable("M_W1_lljj", "M_W1_lljj", "units", 'F');
      data_loader->AddVariable("M_N1_l1jj", "M_N1_l1jj", "units", 'F');
      data_loader->AddVariable("M_N2_l2jj", "M_N2_l2jj", "units", 'F');

    }
    else{

      data_loader->AddVariable("Nvbfj", "Nvbfj", "units", 'I');
      data_loader->AddVariable("Nb", "Nb", "units", 'I');
      data_loader->AddVariable("Ptl1", "Ptl1", "units", 'F');
      data_loader->AddVariable("Ptl2", "Ptl2", "units", 'F');
      data_loader->AddVariable("Ptj1", "Ptj1", "units", 'F');
      //data_loader->AddVariable("Ptj2", "Ptj2", "units", 'F');
      data_loader->AddVariable("MET2ST", "MET2ST", "units", 'F');
      data_loader->AddVariable("HTLT1", "HTLT1", "units", 'F');
      data_loader->AddVariable("HTLT2", "HTLT2", "units", 'F');
      data_loader->AddVariable("Etal1", "Etal1", "units", 'F');
      data_loader->AddVariable("Etal2", "Etal2", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl1", "PtRatioAJl1", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl2", "PtRatioAJl2", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl1", "CEMFracAJl1", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl2", "CEMFracAJl2", "units", 'F');
      //data_loader->AddVariable("NEMFracAJl1", "NEMFracAJl1", "units", 'F');
      data_loader->AddVariable("NEMFracAJl2", "NEMFracAJl2", "units", 'F');
      data_loader->AddVariable("CHFracAJl1", "CHFracAJl1", "units", 'F');
      data_loader->AddVariable("CHFracAJl2", "CHFracAJl2", "units", 'F');
      //data_loader->AddVariable("NHFracAJl1", "NHFracAJl1", "units", 'F');
      //data_loader->AddVariable("NHFracAJl2", "NHFracAJl2", "units", 'F');
      data_loader->AddVariable("MuFracAJl1", "MuFracAJl1", "units", 'F');
      data_loader->AddVariable("MuFracAJl2", "MuFracAJl2", "units", 'F');
      data_loader->AddVariable("JetDiscAJl1", "JetDiscAJl1", "units", 'F');
      data_loader->AddVariable("JetDiscAJl2", "JetDiscAJl2", "units", 'F');
      data_loader->AddVariable("dRll", "dRll", "units", 'F');
      data_loader->AddVariable("dRlj11", "dRlj11", "units", 'F');
      //data_loader->AddVariable("dRlj12", "dRlj12", "units", 'F');
      data_loader->AddVariable("dRlj21", "dRlj21", "units", 'F');
      data_loader->AddVariable("dRlj22", "dRlj22", "units", 'F');
      data_loader->AddVariable("MSSSF", "MSSSF", "units", 'F');
      data_loader->AddVariable("Mlj11", "Mlj11", "units", 'F');
      data_loader->AddVariable("Mlj12", "Mlj12", "units", 'F');
      data_loader->AddVariable("Mlj21", "Mlj21", "units", 'F');
      data_loader->AddVariable("Mlj22", "Mlj22", "units", 'F');
      data_loader->AddVariable("MTvl1", "MTvl1", "units", 'F');
      data_loader->AddVariable("MTvl2", "MTvl2", "units", 'F');
      data_loader->AddVariable("Mllj1", "Mllj1", "units", 'F');
      //data_loader->AddVariable("Mllj2", "Mllj2", "units", 'F');
      //data_loader->AddVariable("PtWj1", "PtWj1", "units", 'F');
      //data_loader->AddVariable("PtWj2", "PtWj2", "units", 'F');
      data_loader->AddVariable("dRWjj", "dRWjj", "units", 'F');
      //data_loader->AddVariable("dRlW12", "dRlW12", "units", 'F');
      //data_loader->AddVariable("dRlW22", "dRlW22", "units", 'F');
      data_loader->AddVariable("dRlN12", "dRlN12", "units", 'F');
      data_loader->AddVariable("dRlN21", "dRlN21", "units", 'F');
      data_loader->AddVariable("M_W2_jj", "M_W2_jj", "units", 'F');
      //data_loader->AddVariable("M_W1_lljj", "M_W1_lljj", "units", 'F');
      data_loader->AddVariable("M_N1_l1jj", "M_N1_l1jj", "units", 'F');
      data_loader->AddVariable("M_N2_l2jj", "M_N2_l2jj", "units", 'F');

    }


  }

  if(channel == "EE"){

    if(bkgType == "Fake"){

      data_loader->AddVariable("Nvbfj", "Nvbfj", "units", 'I');
      data_loader->AddVariable("Nb", "Nb", "units", 'I');
      data_loader->AddVariable("Ptl1", "Ptl1", "units", 'F');
      data_loader->AddVariable("Ptl2", "Ptl2", "units", 'F');
      data_loader->AddVariable("Ptj1", "Ptj1", "units", 'F');
      //data_loader->AddVariable("Ptj2", "Ptj2", "units", 'F');
      data_loader->AddVariable("MET2ST", "MET2ST", "units", 'F');
      data_loader->AddVariable("HTLT1", "HTLT1", "units", 'F');
      data_loader->AddVariable("HTLT2", "HTLT2", "units", 'F');
      data_loader->AddVariable("Etal1", "Etal1", "units", 'F');
      data_loader->AddVariable("Etal2", "Etal2", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl1", "PtRatioAJl1", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl2", "PtRatioAJl2", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl1", "CEMFracAJl1", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl2", "CEMFracAJl2", "units", 'F');
      data_loader->AddVariable("NEMFracAJl1", "NEMFracAJl1", "units", 'F');
      data_loader->AddVariable("NEMFracAJl2", "NEMFracAJl2", "units", 'F');
      data_loader->AddVariable("CHFracAJl1", "CHFracAJl1", "units", 'F');
      data_loader->AddVariable("CHFracAJl2", "CHFracAJl2", "units", 'F');
      data_loader->AddVariable("NHFracAJl1", "NHFracAJl1", "units", 'F');
      data_loader->AddVariable("NHFracAJl2", "NHFracAJl2", "units", 'F');
      //data_loader->AddVariable("MuFracAJl1", "MuFracAJl1", "units", 'F');
      //data_loader->AddVariable("MuFracAJl2", "MuFracAJl2", "units", 'F');
      data_loader->AddVariable("JetDiscAJl1", "JetDiscAJl1", "units", 'F');
      data_loader->AddVariable("JetDiscAJl2", "JetDiscAJl2", "units", 'F');
      data_loader->AddVariable("dRll", "dRll", "units", 'F');
      data_loader->AddVariable("dRlj11", "dRlj11", "units", 'F');
      data_loader->AddVariable("dRlj12", "dRlj12", "units", 'F');
      data_loader->AddVariable("dRlj21", "dRlj21", "units", 'F');
      data_loader->AddVariable("dRlj22", "dRlj22", "units", 'F');
      data_loader->AddVariable("MSSSF", "MSSSF", "units", 'F');
      data_loader->AddVariable("Mlj11", "Mlj11", "units", 'F');
      //data_loader->AddVariable("Mlj12", "Mlj12", "units", 'F');
      data_loader->AddVariable("Mlj21", "Mlj21", "units", 'F');
      data_loader->AddVariable("Mlj22", "Mlj22", "units", 'F');
      data_loader->AddVariable("MTvl1", "MTvl1", "units", 'F');
      data_loader->AddVariable("MTvl2", "MTvl2", "units", 'F');
      data_loader->AddVariable("Mllj1", "Mllj1", "units", 'F');
      //data_loader->AddVariable("Mllj2", "Mllj2", "units", 'F');
      //data_loader->AddVariable("PtWj1", "PtWj1", "units", 'F');
      //data_loader->AddVariable("PtWj2", "PtWj2", "units", 'F');
      data_loader->AddVariable("dRWjj", "dRWjj", "units", 'F');
      //data_loader->AddVariable("dRlW12", "dRlW12", "units", 'F');
      //data_loader->AddVariable("dRlW22", "dRlW22", "units", 'F');
      //data_loader->AddVariable("dRlN12", "dRlN12", "units", 'F');
      data_loader->AddVariable("dRlN21", "dRlN21", "units", 'F');
      data_loader->AddVariable("M_W2_jj", "M_W2_jj", "units", 'F');
      //data_loader->AddVariable("M_W1_lljj", "M_W1_lljj", "units", 'F');
      data_loader->AddVariable("M_N1_l1jj", "M_N1_l1jj", "units", 'F');
      data_loader->AddVariable("M_N2_l2jj", "M_N2_l2jj", "units", 'F');

    }
    else if(bkgType == "NonFake"){

      data_loader->AddVariable("Nvbfj", "Nvbfj", "units", 'I');
      data_loader->AddVariable("Nb", "Nb", "units", 'I');
      data_loader->AddVariable("Ptl1", "Ptl1", "units", 'F');
      data_loader->AddVariable("Ptl2", "Ptl2", "units", 'F');
      data_loader->AddVariable("Ptj1", "Ptj1", "units", 'F');
      data_loader->AddVariable("Ptj2", "Ptj2", "units", 'F');
      data_loader->AddVariable("MET2ST", "MET2ST", "units", 'F');
      data_loader->AddVariable("HTLT1", "HTLT1", "units", 'F');
      data_loader->AddVariable("HTLT2", "HTLT2", "units", 'F');
      data_loader->AddVariable("Etal1", "Etal1", "units", 'F');
      data_loader->AddVariable("Etal2", "Etal2", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl1", "PtRatioAJl1", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl2", "PtRatioAJl2", "units", 'F');
      data_loader->AddVariable("CEMFracAJl1", "CEMFracAJl1", "units", 'F');
      data_loader->AddVariable("CEMFracAJl2", "CEMFracAJl2", "units", 'F');
      data_loader->AddVariable("NEMFracAJl1", "NEMFracAJl1", "units", 'F');
      data_loader->AddVariable("NEMFracAJl2", "NEMFracAJl2", "units", 'F');
      data_loader->AddVariable("CHFracAJl1", "CHFracAJl1", "units", 'F');
      data_loader->AddVariable("CHFracAJl2", "CHFracAJl2", "units", 'F');
      //data_loader->AddVariable("NHFracAJl1", "NHFracAJl1", "units", 'F');
      //data_loader->AddVariable("NHFracAJl2", "NHFracAJl2", "units", 'F');
      //data_loader->AddVariable("MuFracAJl1", "MuFracAJl1", "units", 'F');
      //data_loader->AddVariable("MuFracAJl2", "MuFracAJl2", "units", 'F');
      data_loader->AddVariable("JetDiscAJl1", "JetDiscAJl1", "units", 'F');
      data_loader->AddVariable("JetDiscAJl2", "JetDiscAJl2", "units", 'F');
      data_loader->AddVariable("dRll", "dRll", "units", 'F');
      data_loader->AddVariable("dRlj11", "dRlj11", "units", 'F');
      //data_loader->AddVariable("dRlj12", "dRlj12", "units", 'F');
      data_loader->AddVariable("dRlj21", "dRlj21", "units", 'F');
      //data_loader->AddVariable("dRlj22", "dRlj22", "units", 'F');
      data_loader->AddVariable("MSSSF", "MSSSF", "units", 'F');
      data_loader->AddVariable("Mlj11", "Mlj11", "units", 'F');
      //data_loader->AddVariable("Mlj12", "Mlj12", "units", 'F');
      data_loader->AddVariable("Mlj21", "Mlj21", "units", 'F');
      data_loader->AddVariable("Mlj22", "Mlj22", "units", 'F');
      data_loader->AddVariable("MTvl1", "MTvl1", "units", 'F');
      data_loader->AddVariable("MTvl2", "MTvl2", "units", 'F');
      data_loader->AddVariable("Mllj1", "Mllj1", "units", 'F');
      //data_loader->AddVariable("Mllj2", "Mllj2", "units", 'F');
      //data_loader->AddVariable("PtWj1", "PtWj1", "units", 'F');
      //data_loader->AddVariable("PtWj2", "PtWj2", "units", 'F');
      data_loader->AddVariable("dRWjj", "dRWjj", "units", 'F');
      //data_loader->AddVariable("dRlW12", "dRlW12", "units", 'F');
      //data_loader->AddVariable("dRlW22", "dRlW22", "units", 'F');
      data_loader->AddVariable("dRlN12", "dRlN12", "units", 'F');
      data_loader->AddVariable("dRlN21", "dRlN21", "units", 'F');
      data_loader->AddVariable("M_W2_jj", "M_W2_jj", "units", 'F');
      //data_loader->AddVariable("M_W1_lljj", "M_W1_lljj", "units", 'F');
      data_loader->AddVariable("M_N1_l1jj", "M_N1_l1jj", "units", 'F');
      data_loader->AddVariable("M_N2_l2jj", "M_N2_l2jj", "units", 'F');

    }
    else{

      data_loader->AddVariable("Nvbfj", "Nvbfj", "units", 'I');
      data_loader->AddVariable("Nb", "Nb", "units", 'I');
      data_loader->AddVariable("Ptl1", "Ptl1", "units", 'F');
      data_loader->AddVariable("Ptl2", "Ptl2", "units", 'F');
      data_loader->AddVariable("Ptj1", "Ptj1", "units", 'F');
      //data_loader->AddVariable("Ptj2", "Ptj2", "units", 'F');
      data_loader->AddVariable("MET2ST", "MET2ST", "units", 'F');
      data_loader->AddVariable("HTLT1", "HTLT1", "units", 'F');
      data_loader->AddVariable("HTLT2", "HTLT2", "units", 'F');
      data_loader->AddVariable("Etal1", "Etal1", "units", 'F');
      data_loader->AddVariable("Etal2", "Etal2", "units", 'F');
      data_loader->AddVariable("PtRatioAJl1", "PtRatioAJl1", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl2", "PtRatioAJl2", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl1", "CEMFracAJl1", "units", 'F');
      data_loader->AddVariable("CEMFracAJl2", "CEMFracAJl2", "units", 'F');
      data_loader->AddVariable("NEMFracAJl1", "NEMFracAJl1", "units", 'F');
      data_loader->AddVariable("NEMFracAJl2", "NEMFracAJl2", "units", 'F');
      data_loader->AddVariable("CHFracAJl1", "CHFracAJl1", "units", 'F');
      data_loader->AddVariable("CHFracAJl2", "CHFracAJl2", "units", 'F');
      //data_loader->AddVariable("NHFracAJl1", "NHFracAJl1", "units", 'F');
      //data_loader->AddVariable("NHFracAJl2", "NHFracAJl2", "units", 'F');
      //data_loader->AddVariable("MuFracAJl1", "MuFracAJl1", "units", 'F');
      //data_loader->AddVariable("MuFracAJl2", "MuFracAJl2", "units", 'F');
      data_loader->AddVariable("JetDiscAJl1", "JetDiscAJl1", "units", 'F');
      data_loader->AddVariable("JetDiscAJl2", "JetDiscAJl2", "units", 'F');
      data_loader->AddVariable("dRll", "dRll", "units", 'F');
      data_loader->AddVariable("dRlj11", "dRlj11", "units", 'F');
      //data_loader->AddVariable("dRlj12", "dRlj12", "units", 'F');
      data_loader->AddVariable("dRlj21", "dRlj21", "units", 'F');
      data_loader->AddVariable("dRlj22", "dRlj22", "units", 'F');
      data_loader->AddVariable("MSSSF", "MSSSF", "units", 'F');
      data_loader->AddVariable("Mlj11", "Mlj11", "units", 'F');
      data_loader->AddVariable("Mlj12", "Mlj12", "units", 'F');
      data_loader->AddVariable("Mlj21", "Mlj21", "units", 'F');
      data_loader->AddVariable("Mlj22", "Mlj22", "units", 'F');
      data_loader->AddVariable("MTvl1", "MTvl1", "units", 'F');
      data_loader->AddVariable("MTvl2", "MTvl2", "units", 'F');
      data_loader->AddVariable("Mllj1", "Mllj1", "units", 'F');
      //data_loader->AddVariable("Mllj2", "Mllj2", "units", 'F');
      //data_loader->AddVariable("PtWj1", "PtWj1", "units", 'F');
      //data_loader->AddVariable("PtWj2", "PtWj2", "units", 'F');
      data_loader->AddVariable("dRWjj", "dRWjj", "units", 'F');
      //data_loader->AddVariable("dRlW12", "dRlW12", "units", 'F');
      //data_loader->AddVariable("dRlW22", "dRlW22", "units", 'F');
      //data_loader->AddVariable("dRlN12", "dRlN12", "units", 'F');
      data_loader->AddVariable("dRlN21", "dRlN21", "units", 'F');
      data_loader->AddVariable("M_W2_jj", "M_W2_jj", "units", 'F');
      //data_loader->AddVariable("M_W1_lljj", "M_W1_lljj", "units", 'F');
      data_loader->AddVariable("M_N1_l1jj", "M_N1_l1jj", "units", 'F');
      data_loader->AddVariable("M_N2_l2jj", "M_N2_l2jj", "units", 'F');

    }

  }

  if(channel == "EMu"){

    if(bkgType == "Fake"){

      data_loader->AddVariable("Nvbfj", "Nvbfj", "units", 'I');
      data_loader->AddVariable("Nb", "Nb", "units", 'I');
      data_loader->AddVariable("Ptl1", "Ptl1", "units", 'F');
      data_loader->AddVariable("Ptl2", "Ptl2", "units", 'F');
      data_loader->AddVariable("Ptj1", "Ptj1", "units", 'F');
      //data_loader->AddVariable("Ptj2", "Ptj2", "units", 'F');
      data_loader->AddVariable("MET2ST", "MET2ST", "units", 'F');
      data_loader->AddVariable("HTLT1", "HTLT1", "units", 'F');
      data_loader->AddVariable("HTLT2", "HTLT2", "units", 'F');
      data_loader->AddVariable("Etal1", "Etal1", "units", 'F');
      data_loader->AddVariable("Etal2", "Etal2", "units", 'F');
      data_loader->AddVariable("PtRatioAJl1", "PtRatioAJl1", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl2", "PtRatioAJl2", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl1", "CEMFracAJl1", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl2", "CEMFracAJl2", "units", 'F');
      data_loader->AddVariable("NEMFracAJl1", "NEMFracAJl1", "units", 'F');
      data_loader->AddVariable("NEMFracAJl2", "NEMFracAJl2", "units", 'F');
      data_loader->AddVariable("CHFracAJl1", "CHFracAJl1", "units", 'F');
      data_loader->AddVariable("CHFracAJl2", "CHFracAJl2", "units", 'F');
      //data_loader->AddVariable("NHFracAJl1", "NHFracAJl1", "units", 'F');
      data_loader->AddVariable("NHFracAJl2", "NHFracAJl2", "units", 'F');
      //data_loader->AddVariable("MuFracAJl1", "MuFracAJl1", "units", 'F');
      //data_loader->AddVariable("MuFracAJl2", "MuFracAJl2", "units", 'F');
      data_loader->AddVariable("JetDiscAJl1", "JetDiscAJl1", "units", 'F');
      data_loader->AddVariable("JetDiscAJl2", "JetDiscAJl2", "units", 'F');
      data_loader->AddVariable("dRll", "dRll", "units", 'F');
      data_loader->AddVariable("dRlj11", "dRlj11", "units", 'F');
      data_loader->AddVariable("dRlj12", "dRlj12", "units", 'F');
      data_loader->AddVariable("dRlj21", "dRlj21", "units", 'F');
      data_loader->AddVariable("dRlj22", "dRlj22", "units", 'F');
      data_loader->AddVariable("MSSSF", "MSSSF", "units", 'F');
      data_loader->AddVariable("Mlj11", "Mlj11", "units", 'F');
      //data_loader->AddVariable("Mlj12", "Mlj12", "units", 'F');
      data_loader->AddVariable("Mlj21", "Mlj21", "units", 'F');
      //data_loader->AddVariable("Mlj22", "Mlj22", "units", 'F');
      data_loader->AddVariable("MTvl1", "MTvl1", "units", 'F');
      data_loader->AddVariable("MTvl2", "MTvl2", "units", 'F');
      data_loader->AddVariable("Mllj1", "Mllj1", "units", 'F');
      //data_loader->AddVariable("Mllj2", "Mllj2", "units", 'F');
      //data_loader->AddVariable("PtWj1", "PtWj1", "units", 'F');
      //data_loader->AddVariable("PtWj2", "PtWj2", "units", 'F');
      data_loader->AddVariable("dRWjj", "dRWjj", "units", 'F');
      //data_loader->AddVariable("dRlW12", "dRlW12", "units", 'F');
      //data_loader->AddVariable("dRlW22", "dRlW22", "units", 'F');
      data_loader->AddVariable("dRlN12", "dRlN12", "units", 'F');
      data_loader->AddVariable("dRlN21", "dRlN21", "units", 'F');
      data_loader->AddVariable("M_W2_jj", "M_W2_jj", "units", 'F');
      //data_loader->AddVariable("M_W1_lljj", "M_W1_lljj", "units", 'F');
      data_loader->AddVariable("M_N1_l1jj", "M_N1_l1jj", "units", 'F');
      data_loader->AddVariable("M_N2_l2jj", "M_N2_l2jj", "units", 'F');

    }
    else if(bkgType == "NonFake"){

      data_loader->AddVariable("Nvbfj", "Nvbfj", "units", 'I');
      data_loader->AddVariable("Nb", "Nb", "units", 'I');
      data_loader->AddVariable("Ptl1", "Ptl1", "units", 'F');
      data_loader->AddVariable("Ptl2", "Ptl2", "units", 'F');
      data_loader->AddVariable("Ptj1", "Ptj1", "units", 'F');
      data_loader->AddVariable("Ptj2", "Ptj2", "units", 'F');
      data_loader->AddVariable("MET2ST", "MET2ST", "units", 'F');
      data_loader->AddVariable("HTLT1", "HTLT1", "units", 'F');
      data_loader->AddVariable("HTLT2", "HTLT2", "units", 'F');
      data_loader->AddVariable("Etal1", "Etal1", "units", 'F');
      data_loader->AddVariable("Etal2", "Etal2", "units", 'F');
      data_loader->AddVariable("PtRatioAJl1", "PtRatioAJl1", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl2", "PtRatioAJl2", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl1", "CEMFracAJl1", "units", 'F');
      data_loader->AddVariable("CEMFracAJl2", "CEMFracAJl2", "units", 'F');
      data_loader->AddVariable("NEMFracAJl1", "NEMFracAJl1", "units", 'F');
      //data_loader->AddVariable("NEMFracAJl2", "NEMFracAJl2", "units", 'F');
      data_loader->AddVariable("CHFracAJl1", "CHFracAJl1", "units", 'F');
      data_loader->AddVariable("CHFracAJl2", "CHFracAJl2", "units", 'F');
      //data_loader->AddVariable("NHFracAJl1", "NHFracAJl1", "units", 'F');
      //data_loader->AddVariable("NHFracAJl2", "NHFracAJl2", "units", 'F');
      //data_loader->AddVariable("MuFracAJl1", "MuFracAJl1", "units", 'F');
      //data_loader->AddVariable("MuFracAJl2", "MuFracAJl2", "units", 'F');
      data_loader->AddVariable("JetDiscAJl1", "JetDiscAJl1", "units", 'F');
      data_loader->AddVariable("JetDiscAJl2", "JetDiscAJl2", "units", 'F');
      data_loader->AddVariable("dRll", "dRll", "units", 'F');
      data_loader->AddVariable("dRlj11", "dRlj11", "units", 'F');
      //data_loader->AddVariable("dRlj12", "dRlj12", "units", 'F');
      data_loader->AddVariable("dRlj21", "dRlj21", "units", 'F');
      data_loader->AddVariable("dRlj22", "dRlj22", "units", 'F');
      data_loader->AddVariable("MSSSF", "MSSSF", "units", 'F');
      data_loader->AddVariable("Mlj11", "Mlj11", "units", 'F');
      data_loader->AddVariable("Mlj12", "Mlj12", "units", 'F');
      data_loader->AddVariable("Mlj21", "Mlj21", "units", 'F');
      data_loader->AddVariable("Mlj22", "Mlj22", "units", 'F');
      data_loader->AddVariable("MTvl1", "MTvl1", "units", 'F');
      data_loader->AddVariable("MTvl2", "MTvl2", "units", 'F');
      data_loader->AddVariable("Mllj1", "Mllj1", "units", 'F');
      //data_loader->AddVariable("Mllj2", "Mllj2", "units", 'F');
      //data_loader->AddVariable("PtWj1", "PtWj1", "units", 'F');
      //data_loader->AddVariable("PtWj2", "PtWj2", "units", 'F');
      data_loader->AddVariable("dRWjj", "dRWjj", "units", 'F');
      //data_loader->AddVariable("dRlW12", "dRlW12", "units", 'F');
      //data_loader->AddVariable("dRlW22", "dRlW22", "units", 'F');
      //data_loader->AddVariable("dRlN12", "dRlN12", "units", 'F');
      data_loader->AddVariable("dRlN21", "dRlN21", "units", 'F');
      data_loader->AddVariable("M_W2_jj", "M_W2_jj", "units", 'F');
      //data_loader->AddVariable("M_W1_lljj", "M_W1_lljj", "units", 'F');
      data_loader->AddVariable("M_N1_l1jj", "M_N1_l1jj", "units", 'F');
      data_loader->AddVariable("M_N2_l2jj", "M_N2_l2jj", "units", 'F');

    }
    else{

      data_loader->AddVariable("Nvbfj", "Nvbfj", "units", 'I');
      data_loader->AddVariable("Nb", "Nb", "units", 'I');
      data_loader->AddVariable("Ptl1", "Ptl1", "units", 'F');
      data_loader->AddVariable("Ptl2", "Ptl2", "units", 'F');
      data_loader->AddVariable("Ptj1", "Ptj1", "units", 'F');
      data_loader->AddVariable("Ptj2", "Ptj2", "units", 'F');
      data_loader->AddVariable("MET2ST", "MET2ST", "units", 'F');
      data_loader->AddVariable("HTLT1", "HTLT1", "units", 'F');
      data_loader->AddVariable("HTLT2", "HTLT2", "units", 'F');
      data_loader->AddVariable("Etal1", "Etal1", "units", 'F');
      data_loader->AddVariable("Etal2", "Etal2", "units", 'F');
      data_loader->AddVariable("PtRatioAJl1", "PtRatioAJl1", "units", 'F');
      //data_loader->AddVariable("PtRatioAJl2", "PtRatioAJl2", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl1", "CEMFracAJl1", "units", 'F');
      //data_loader->AddVariable("CEMFracAJl2", "CEMFracAJl2", "units", 'F');
      data_loader->AddVariable("NEMFracAJl1", "NEMFracAJl1", "units", 'F');
      //data_loader->AddVariable("NEMFracAJl2", "NEMFracAJl2", "units", 'F');
      data_loader->AddVariable("CHFracAJl1", "CHFracAJl1", "units", 'F');
      data_loader->AddVariable("CHFracAJl2", "CHFracAJl2", "units", 'F');
      //data_loader->AddVariable("NHFracAJl1", "NHFracAJl1", "units", 'F');
      //data_loader->AddVariable("NHFracAJl2", "NHFracAJl2", "units", 'F');
      //data_loader->AddVariable("MuFracAJl1", "MuFracAJl1", "units", 'F');
      //data_loader->AddVariable("MuFracAJl2", "MuFracAJl2", "units", 'F');
      data_loader->AddVariable("JetDiscAJl1", "JetDiscAJl1", "units", 'F');
      data_loader->AddVariable("JetDiscAJl2", "JetDiscAJl2", "units", 'F');
      data_loader->AddVariable("dRll", "dRll", "units", 'F');
      data_loader->AddVariable("dRlj11", "dRlj11", "units", 'F');
      data_loader->AddVariable("dRlj12", "dRlj12", "units", 'F');
      data_loader->AddVariable("dRlj21", "dRlj21", "units", 'F');
      data_loader->AddVariable("dRlj22", "dRlj22", "units", 'F');
      data_loader->AddVariable("MSSSF", "MSSSF", "units", 'F');
      data_loader->AddVariable("Mlj11", "Mlj11", "units", 'F');
      data_loader->AddVariable("Mlj12", "Mlj12", "units", 'F');
      data_loader->AddVariable("Mlj21", "Mlj21", "units", 'F');
      data_loader->AddVariable("Mlj22", "Mlj22", "units", 'F');
      data_loader->AddVariable("MTvl1", "MTvl1", "units", 'F');
      data_loader->AddVariable("MTvl2", "MTvl2", "units", 'F');
      data_loader->AddVariable("Mllj1", "Mllj1", "units", 'F');
      //data_loader->AddVariable("Mllj2", "Mllj2", "units", 'F');
      //data_loader->AddVariable("PtWj1", "PtWj1", "units", 'F');
      //data_loader->AddVariable("PtWj2", "PtWj2", "units", 'F');
      data_loader->AddVariable("dRWjj", "dRWjj", "units", 'F');
      //data_loader->AddVariable("dRlW12", "dRlW12", "units", 'F');
      //data_loader->AddVariable("dRlW22", "dRlW22", "units", 'F');
      //data_loader->AddVariable("dRlN12", "dRlN12", "units", 'F');
      data_loader->AddVariable("dRlN21", "dRlN21", "units", 'F');
      data_loader->AddVariable("M_W2_jj", "M_W2_jj", "units", 'F');
      //data_loader->AddVariable("M_W1_lljj", "M_W1_lljj", "units", 'F');
      data_loader->AddVariable("M_N1_l1jj", "M_N1_l1jj", "units", 'F');
      data_loader->AddVariable("M_N2_l2jj", "M_N2_l2jj", "units", 'F');

    }

  }

  data_loader->AddSpectator("w_tot", "w_tot", "units", 'F');          

  data_loader->AddSignalTree(tree_signal, 1.);
  data_loader->AddBackgroundTree(tree_bkg, 1.0);
  data_loader->SetWeightExpression("w_tot");  

  //==== Nj, Nb cut
  TCut cut_s = "";
  TCut cut_b = "";

  /*if(Nb_mode == 0){

    if(Nj_mode == 1){
      cut_s = "Nj<2";
      cut_b = "Nj<2";
    }
    if(Nj_mode == 2){
      cut_s = "Nj>=2";
      cut_b = "Nj>=2";
    }

  }

  if(Nb_mode == 1){

    if(Nj_mode == 1){
      cut_s = "(Nb<1)&&(Nj<2)";
      cut_b = "(Nb<1)&&(Nj<2)";
    }
    if(Nj_mode == 2){
      cut_s = "(Nb<1)&&(Nj<2)";
      cut_b = "(Nb<1)&&(Nj<2)";
    }

  }*/

  int n_train_signal = tree_signal->GetEntries(cut_s)/2;
  int n_train_back = tree_bkg->GetEntries(cut_b)/2;


  data_loader->PrepareTrainingAndTestTree(cut_s, cut_b,
                                          Form("nTrain_Signal=%d:nTrain_Background=%d:nTest_Signal=%d:nTest_Background=%d:SplitMode=Random:NormMode=NumEvents:V", n_train_signal, n_train_back, n_train_signal, n_train_back));

  //==== Adaptive Boost
  // factory->BookMethod(data_loader, TMVA::Types::kBDT, "BDT",
  //       "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=100"); // UseBaggedBoost -> only for GradBoost

  //==== Boosted Decision Trees with adaptive boosting
//  factory->BookMethod( data_loader,TMVA::Types::kBDT, "BDT",
//		       "!H:!V:NTrees=850:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=100:PruneMethod=NoPruning" ); 

  //factory->BookMethod( data_loader,TMVA::Types::kBDT, "BDT", "!H:!V:NTrees="+NTrees_txt+":nEventsMin=400:MaxDepth=3:nCuts="+NCuts_txt+":BoostType=AdaBoost:SeparationType=GiniIndex:PruneMethod=NoPruning" );
  if(BDTMethod == "BDT"){
    factory->BookMethod( data_loader,TMVA::Types::kBDT, "BDT", "!H:!V:NTrees="+NTrees_txt+":MinNodeSize=1.0%:MaxDepth="+MaxDepth_txt+":nCuts="+NCuts_txt+":BoostType=AdaBoost:SeparationType=GiniIndex:PruneMethod=NoPruning:NegWeightTreatment=IgnoreNegWeightsInTraining" ); // Without negative weights
    //factory->BookMethod( data_loader,TMVA::Types::kBDT, "BDT", "!H:!V:NTrees="+NTrees_txt+":MinNodeSize=0.5%:MaxDepth="+MaxDepth_txt+":nCuts="+NCuts_txt+":BoostType=AdaBoost:SeparationType=GiniIndex:PruneMethod=NoPruning:NegWeightTreatment=IgnoreNegWeightsInTraining" );
  }

  //==== Gradient Boost
  //factory->BookMethod(data_loader, TMVA::Types::kBDT, "BDTG",//
  //	      "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=100:MaxDepth=2" );
  if(BDTMethod == "BDTG"){
    factory->BookMethod(data_loader, TMVA::Types::kBDT, "BDTG", "!H:!V:NTrees="+NTrees_txt+":MinNodeSize=1.0%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts="+NCuts_txt+":MaxDepth="+MaxDepth_txt+":NegWeightTreatment=IgnoreNegWeightsInTraining" );
  }

  if(BDTMethod == "DNN"){

    bool useDLGPU = false;
    useDLGPU = true;

    // Define DNN layout
    TString inputLayoutString = "InputLayout=1|1|35";
    TString batchLayoutString= "BatchLayout=1|128|35";
    //TString layoutString ("Layout=DENSE|64|TANH,DENSE|64|TANH,DENSE|64|TANH,DENSE|64|TANH,DENSE|1|LINEAR");
    TString layoutString ("Layout=DENSE|64|RELU,DENSE|64|RELU,DENSE|64|RELU,DENSE|64|RELU,DENSE|1|LINEAR");

    // Define Training strategies
    // one can catenate several training strategies
    TString training1("LearningRate=1e-3,Momentum=0.9,"
                      "ConvergenceSteps=10,BatchSize=128,TestRepetitions=1,"
                      "MaxEpochs=30,WeightDecay=1e-4,Regularization=None,"
                      "Optimizer=ADAM,ADAM_beta1=0.9,ADAM_beta2=0.999,ADAM_eps=1.E-7," // ADAM default parameters
                      "DropConfig=0.0+0.0+0.0+0.");
    //TString training2("LearningRate=1e-3,Momentum=0.9"
    //                  "ConvergenceSteps=10,BatchSize=128,TestRepetitions=1,"
    //                  "MaxEpochs=20,WeightDecay=1e-4,Regularization=None,"
    //                  "Optimizer=SGD,DropConfig=0.0+0.0+0.0+0.");

    TString trainingStrategyString ("TrainingStrategy=");
    trainingStrategyString += training1; // + "|" + training2;

    // General Options.

    TString dnnOptions("!H:V:ErrorStrategy=CROSSENTROPY:VarTransform=G:"
                       "WeightInitialization=XAVIER");
    dnnOptions.Append (":"); dnnOptions.Append (inputLayoutString);
    dnnOptions.Append (":"); dnnOptions.Append (batchLayoutString);
    dnnOptions.Append (":"); dnnOptions.Append (layoutString);
    dnnOptions.Append (":"); dnnOptions.Append (trainingStrategyString);

    TString dnnMethodName = "DNN_CPU";
    if(useDLGPU){
      dnnOptions += ":Architecture=GPU";
      dnnMethodName = "DNN_GPU";
    }
    else{
      dnnOptions += ":Architecture=CPU";
    }

    factory->BookMethod(data_loader, TMVA::Types::kDL, dnnMethodName, dnnOptions);

  }

  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();

  fout->Close();

  delete factory;
  delete data_loader;

  //TMVA::TMVAGui(fout_name);

  return;
}
