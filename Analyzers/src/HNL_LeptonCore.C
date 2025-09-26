#include "HNL_LeptonCore.h"

void HNL_LeptonCore::initializeAnalyzer(vector<TString> BDTVersions, bool READBKGHISTS, bool SETUPIDBDT){

  AnalyzerCore::initializeAnalyzer();

  //// Define Limit bins per channel/era
  map_BDT_bins_labels.clear();
  map_bins_labels.clear();
  map_bins_boundaries.clear();
  DefineLimitBins();
  DefineBDTLimitBins(BDTVersions);

  /// SETUP BKG OBJ
  mcCorr          = new MCCorrection();
  puppiCorr       = new PuppiSoftdropMassCorr();
  fakeEst         = new FakeBackgroundEstimator();
  cfEst           = new CFBackgroundEstimator();
  pdfReweight     = new PDFReweight();

  runCutCounts.clear(); 
  

  //=== VERBOSE                                                                                                                                        
  run_Debug = HasFlag("DEBUG");

  //=== bkg flags                                                                                                                                      
  RunPrompt = HasFlag("RunPrompt");
  RunFake   = HasFlag("RunFake");
  RunOSFake = HasFlag("RunOSFake");

  UseMET2ST = HasFlag("UseMET2ST"); 
  if(RunOSFake) RunFake = true;/// In case RunFake not flagged
  RunFakeTF = HasFlag("RunFakeTF");
  RunCF     = HasFlag("RunCF");
  RunConv   = HasFlag("RunConv");
  RunPromptTLRemoval = HasFlag("RunPromptTLRemoval");
  run_ORTrigger = HasFlag("MultiTrig");

  /// Other flags                                                                                                                                      
  RunEE   = HasFlag("EE");
  RunMuMu = HasFlag("MuMu");
  RunEMu  = HasFlag("EMu");

  HEM1516 = HasFlag("HEM1516");

  /// clear map
  map_bdt_booked.clear();

  std::vector<JetTagging::Parameters> jtps;
  jtps.push_back( JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Loose, JetTagging::incl, JetTagging::mujets));
  jtps.push_back( JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets));
  jtps.push_back( JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Tight, JetTagging::incl, JetTagging::mujets));

  SetupTriggerLists();

  if(IsDYSample) SetupZptWeight();

  //==== MCCorrection                                                                                                                                       

  ///// Set up Jet Tagger
  mcCorr->SetJetTaggingParameters(jtps);                                                                                                                             
  
  mcCorr->SetMCSample(MCSample);
  mcCorr->SetEra(GetEra());
  mcCorr->SetIsDATA(IsDATA);
  mcCorr->SetEventInfo(run, lumi, event);
  mcCorr->SetIsFastSim(IsFastSim);

  //// Read Histograms Moved from Initialise Tools
  cout << "HNL_LeptonCore::initializeAnalyzer : Analyzer = " << Analyzer << endl;
  if(!IsDATA){
    mcCorr->ReadHistograms();

    TString tagEffFile = "";
    
    if (HasFlag("Use_TT_JetEff_2L"))
      tagEffFile = "MeasureJetTaggingEfficiency_TTLL_TTLJ_2L_hadded.root";
    else if (HasFlag("Use_TT_JetEff_SS"))
      tagEffFile = "MeasureJetTaggingEfficiency_TTLL_TTLJ_SS_hadded.root";
    else if (HasFlag("Use_DY_JetEff_2L"))
      tagEffFile = "MeasureJetTaggingEfficiency_DY_2L_hadded.root";
    else if (MCSample.Contains("WZ"))
      tagEffFile = "MeasureJetTaggingEfficiency_WZ_2L_hadded.root";
    else if (MCSample.Contains("WW"))
      tagEffFile = "MeasureJetTaggingEfficiency_WW_2L_hadded.root";
    else if (MCSample.Contains("ZZ"))
      tagEffFile = "MeasureJetTaggingEfficiency_ZZ_2L_hadded.root";
    else if (MCSample.Contains("ZG"))
      tagEffFile = "MeasureJetTaggingEfficiency_ZG_2L_hadded.root";
    else if (MCSample.Contains("ttW") || MCSample.Contains("ttZ"))
      tagEffFile = "MeasureJetTaggingEfficiency_TTV_2L_hadded.root";
    else if (MCSample.Contains("WG"))
      tagEffFile = "MeasureJetTaggingEfficiency_WG_2L_hadded.root";
    else if (MCSample.Contains("DYType"))
      tagEffFile = "MeasureJetTaggingEfficiency_HNL_DY_2L_hadded.root";
    else if (MCSample.Contains("VBFType"))
      tagEffFile = "MeasureJetTaggingEfficiency_HNL_VBF_2L_hadded.root";
    else if (MCSample.Contains("SSWW"))
      tagEffFile = "MeasureJetTaggingEfficiency_HNL_SSWW_2L_hadded.root";
    else
      tagEffFile = "MeasureJetTaggingEfficiency_TTLL_TTLJ_2L_hadded.root"; // default fallback

    // Optional: log what is being loaded
    cout << "[JetEff] Using file: " << tagEffFile << endl;
    
    // Apply
    mcCorr->SetupJetTagging(tagEffFile);
    

  }

  puppiCorr->SetEra(GetEra());
  puppiCorr->ReadHistograms();

  //==== FakeBackgroundEstimator                                                                                                                                            
  fakeEst->SetEra(GetEra());
  if(RunFake&&READBKGHISTS)     fakeEst->ReadHistograms(IsDATA,Analyzer=="HNL_ControlRegion"); /// For now when checking                                                                                                                    
  else if(RunPromptTLRemoval)   fakeEst->ReadHistograms(true,Analyzer=="HNL_ControlRegion"); /// For now when checking                                                                                                                    
  else if(Analyzer.Contains("HNL_Lepton_FakeRate") && !Analyzer.Contains("SkimTree") ) fakeEst->ReadHistograms(IsDATA,false); ///
  else if(Analyzer.Contains("Fake") && !Analyzer.Contains("SkimTree") ) fakeEst->ReadHistograms(IsDATA,true); ///
  else if(Analyzer.Contains("SkimTree_EGammaTnP_HNLHighPt") && IsDATA) fakeEst->ReadHistograms(true,false); 

  //==== CFBackgroundEstimator                                                                                                                                              
  cfEst->SetEra(GetEra());
  cfEst->SetFittedBins();

  if(RunCF&&READBKGHISTS)    cfEst->ReadHistograms(1);
  else if (Analyzer.Contains("ChargeFlip"))  cfEst->ReadHistograms(2);

  
  TString datapath = getenv("DATA_DIR");

  if((RunCF&&READBKGHISTS) || Analyzer.Contains("ChargeFlip")){
    for(auto mapCFShift  : {datapath+ "/"+GetEra()+"/CFRate/Shift.txt"} ){
      cout << "Reading " << mapCFShift << endl;
      string Fline;
      ifstream in(mapCFShift);
      while(getline(in,Fline)){
        std::istringstream is( Fline );
        TString a,b,c;
        double d,e;
        is >> a; // Era                                                                                                                                           
        is >> b; // Bin
        is >> c; // ID                                                                                                                                            
        is >> d; // shiftValuechi
        is >> e; // shiftValuemean
        
        MakeCFShiftmap[a+"_"+b+"_"+c+"_Nom"] = d;
        double sysVal= fabs(e-d);
        
        double upvalue=d;
        double downvalue=d;
        if(sysVal < 0.01) sysVal = 0.01;
        if(e > d) {
          downvalue = 0.01;
          upvalue = sysVal;
        }
        else if(e < d) {
          downvalue = sysVal;
          upvalue= 0.01;
        }
        else {
          downvalue = 0.01;
          upvalue = 0.01;
        }
        MakeCFShiftmap[a+"_"+b+"_"+c+"_Up"]  = upvalue;
        MakeCFShiftmap[a+"_"+b+"_"+c+"_Down"] = downvalue;
      }
    }
    for(auto ih : MakeCFShiftmap) cout << "Adding EL Shift " << ih.first << " " << ih.second << endl;
  }



  //// Setup Fake PartonSF 

  if((RunFake&&READBKGHISTS) 
     || RunPromptTLRemoval 
     || (Analyzer.Contains("HNL_Lepton_FakeRate") && !Analyzer.Contains("SkimTree") )
     || (Analyzer.Contains("Fake") && !Analyzer.Contains("SkimTree") )){
    
    
    vector<TString> FakeHMaps = {datapath + "/"+GetEra()+"/FakeRate/MCFR/TT_PartonSF.txt",
         datapath + "/"+GetEra()+"/FakeRate/MCFR/QCD_PartonSF.txt"};
    
    if(IsDATA) FakeHMaps = {datapath + "/"+GetEra()+"/FakeRate/DataFR/Data_PartonSF.txt"};
    cout << "HNL_LeptonCore::IsData = " << IsData << endl;
    
    for(auto ihmap  :  FakeHMaps){
      string Fline;
      ifstream in(ihmap);
      while(getline(in,Fline)){
        std::istringstream is( Fline );
        if(IsDATA){
          TString a,b,c,d;
          double e;
          is >> a; // Era
          is >> b; // Eta
          is >> c; // ID 
          is >> d; // SampleType
          is >> e; // SF                                                                                                                                                                                                                                     
          MakeSFmap[a+"_"+b+"_"+c+"_"+d] = e;  
        }
        else{
          TString a,b,c,d,e;
          double f;
          is >> a; // Era
          is >> b; // Eta
          is >> c; // ID
          is >> d; // SampleType
          is >> e; // Sample
          is >> f; // SF
          MakeSFmap[a+"_"+b+"_"+c+"_"+d + "_"+e] = f;
        }
      }
    }
    for(auto ih : MakeSFmap) cout << "Adding PartonSF " << ih.first << " " << ih.second << endl;
  }
  if(SETUPIDBDT) SetupIDMVAReaderDefault(false,false);

 
  ///// Setup Theory files
  TheoryDir = TDirectoryHelper::GetTempDirectory("Theory");

  ///// Setup K factor code
  const char* skflat_wd = getenv("SKFlat_WD");
  if (!skflat_wd) {
    std::cerr << "Error: SKFlat_WD environment variable not set!" << std::endl;
    return;
  }

  TString VV_EWK_Path = TString(skflat_wd) + "/data/Run2UltraLegacy_v3/Run2/Sample/VV_NLO_LO_CMS_mjj.root";

  h_VV_KF_CMS=nullptr;
  //  h_WW_KF_CMS=nullptr;
  
  vector<TString> EWK_Corr_VV_Samples = {"WZ_EWK","WpWp_EWK"};
  if (std::find(EWK_Corr_VV_Samples.begin(), EWK_Corr_VV_Samples.end(), MCSample) != EWK_Corr_VV_Samples.end()) {

    TDirectory* origDir = gDirectory;

    TFile* VVKFactorFile= new TFile(VV_EWK_Path);

    TheoryDir->cd();
    if(MCSample=="WZ_EWK") h_VV_KF_CMS = ((TH1D*) VVKFactorFile->Get("hWZ_KF_CMS")->Clone());
    if(MCSample=="WpWp_EWK") h_VV_KF_CMS = ((TH1D*) VVKFactorFile->Get("hWW_KF_CMS")->Clone());
    
    origDir->cd();

    if(h_VV_KF_CMS == nullptr){
      std::cerr << "Error: h_VV_KF_CMS  not set!" << std::endl;
      exit(EXIT_FAILURE);
    }

    VVKFactorFile->Close();
    cout << "Close " << VV_EWK_Path <<  " file" << endl;
    delete VVKFactorFile;

  }

  TString TheoryPath = "/data9/Users/HNL_public/PDFSyst/"+GetEra()+"/Theory/GetEffLumi_SkimTree_HNMultiLepBDT_"+MCSample+".root";
  std::ifstream infile(TheoryPath);

  
  h_SumW_PDF=nullptr;
  h_SumW_Scale=nullptr;

  if(HasFlag("RunSyst")){
    if(IsSignal()){
      if(infile.good()){
  
        TDirectory* origDir = gDirectory;
        cout << "Acessing file " << TheoryPath << endl;

        TFile* GenNormFile= new TFile(TheoryPath );

        GenNormFile->ls(); 

        TIter next(GenNormFile->GetListOfKeys());
        TKey* key;
        
        while ((key = (TKey*)next())) {
          if (strcmp(key->GetClassName(), "TH1F") == 0 || 
              strcmp(key->GetClassName(), "TH1D") == 0 || 
              strcmp(key->GetClassName(), "TH1I") == 0 || 
              strcmp(key->GetClassName(), "TH1S") == 0) {

            cout << "Histogram: " << key->GetName() << endl;

            TheoryDir->cd();
            if(strcmp(key->GetName(),"sumW_PDF") == 0) {
              cout << "Initialising sumW_PDF " << endl;
              h_SumW_PDF = ((TH1D*) GenNormFile->Get("sumW_PDF")->Clone());
            }
            if(strcmp(key->GetName(),"sumW_Scale") == 0) h_SumW_Scale = ((TH1D*) GenNormFile->Get("sumW_Scale")->Clone());
            ///if(strcmp(key->GetName(),"sumW_AlphaS") == 0)h_SumW_AlphaS = ((TH1D*) GenNormFile->Get("sumW_AlphaS")->Clone());
            
            origDir->cd();

          }
        }

        GenNormFile->Close();
        cout << "Close file" << endl;
        delete GenNormFile;

        if (h_SumW_PDF!=nullptr)    cout << "h_SumW_PDF Integral = " << h_SumW_PDF->Integral() << endl;
        if (h_SumW_Scale!=nullptr ) cout << "h_SumW_Scale Integral = " << h_SumW_Scale->Integral() << endl;

      }
      else {
        cout << "[HNL_LeptonCore::AccessPathName ] Theory file " << TheoryPath << " not found.." << endl;
        exit(EXIT_FAILURE);
      }
    }
  }

}

void HNL_LeptonCore::SetupEvMVA(){

  TMVA::Tools::Instance();
  cout << "Create Reader class " << endl;
  MVAReaderMM = new TMVA::Reader();
  MVAReaderEE = new TMVA::Reader();
  MVAReaderEM = new TMVA::Reader();
  MVAReaderMMFake = new TMVA::Reader();
  MVAReaderEEFake = new TMVA::Reader();
  MVAReaderEMFake = new TMVA::Reader();
  MVAReaderMMNonFake = new TMVA::Reader();
  MVAReaderEENonFake = new TMVA::Reader();
  MVAReaderEMNonFake = new TMVA::Reader();
  cout << MVAReaderMM << "  " << MVAReaderEE <<" " << MVAReaderEM << endl;
  cout << MVAReaderMMFake << "  " << MVAReaderEEFake <<" " << MVAReaderEMFake << endl;
  cout << MVAReaderMMNonFake << "  " << MVAReaderEENonFake <<" " << MVAReaderEMNonFake << endl;

}

void HNL_LeptonCore::DeleteEvMVA(){
  
  cout << MVAReaderMM << "  " << MVAReaderEE << " " << MVAReaderEM << endl;
  cout << MVAReaderMMFake << "  " << MVAReaderEEFake << " " << MVAReaderEMFake << endl;
  cout << MVAReaderMMNonFake << "  " << MVAReaderEENonFake << " " << MVAReaderEMNonFake << endl;
  delete MVAReaderMM;
  delete MVAReaderEE;
  delete MVAReaderEM;
  delete MVAReaderMMFake;
  delete MVAReaderEEFake;
  delete MVAReaderEMFake;
  delete MVAReaderMMNonFake;
  delete MVAReaderEENonFake;
  delete MVAReaderEMNonFake;

}

bool HNL_LeptonCore::IsExists(TString filepath){
  ifstream fcheck(filepath);
  return fcheck.good();
}

void HNL_LeptonCore::OutCutFlow(TString lab, double w){
  
  
  std::map<TString, double>::iterator mapit = cfmap.find(lab);
  if(mapit != cfmap.end()) {
    cfmap[mapit->first] = mapit->second+w;
  }
  else     cfmap[lab]= w;

  
  return;

}

vector<TString> HNL_LeptonCore::ConvertCutFlowLabels(vector<TString> SRlabels, TString SRName, TString CRName){

  vector<TString> CRlabels;

  for(auto i : SRlabels) {
    TString CRlabel = i;
    CRlabel=CRlabel.ReplaceAll(SRName,CRName);
    CRlabels.push_back(CRlabel);
  }
  
  return CRlabels;
}


TString HNL_LeptonCore::SetLeptonID(const TString& lep, AnalyzerParameter& p){
  if (lep == "Electron") {
    TString ID = (RunFake || RunPromptTLRemoval) ? p.Electron_FR_ID : p.Electron_Tight_ID;
    if (p.FakeMethod == "MC") ID = p.Electron_Tight_ID;
    return ID;
  }
    
  if (lep == "Muon") {
    TString ID = (RunFake || RunPromptTLRemoval) ? p.Muon_FR_ID : p.Muon_Tight_ID;
    if (p.FakeMethod == "MC") ID = p.Muon_Tight_ID;
    return ID;
  }

  std::cerr << "[HNL_LeptonCore::SetLeptonID] Error: Invalid lepton type '" << lep << "'" << std::endl;
  exit(EXIT_FAILURE);  // Exiting program due to invalid input

  return ""; // This return is redundant but ensures function consistency
}


double HNL_LeptonCore::MergeMultiMC(vector<TString> vec, TString Method){

  // This function can be used to merge MC bkg with multiple samples
  
  if(IsData) return 1;
  if(IsSignal()) return 1;

  if (std::count(vec.begin(), vec.end(), "WJet")) {
    /// Merge sherpa and HT binned
    if (MCSample.Contains("WJet")){
      return 0.5;
    }
  }

  if (std::count(vec.begin(), vec.end(), "DY")) {
    if (MCSample.Contains("DYJets")){
      return 0.25;
    }
  }


  if (std::count(vec.begin(), vec.end(), "DYMG")) {
    if (MCSample.Contains("DYJets_MG")){
      return 0.5;
    }
  }
  if (std::count(vec.begin(), vec.end(), "WG")) {
    
    if (MCSample.Contains("WGToLNuG")){
      
      int NearPhotonIdx=-1;
      
      for(unsigned int i=2; i<All_Gens.size(); i++){
  Gen gen = All_Gens[i];
  if( All_Gens.at(i).MotherIndex()<0   ) continue;
  if( !(All_Gens.at(i).PID()==22 && (All_Gens.at(i).Status()==23)) ) continue;
  NearPhotonIdx=i;
      }
      if(NearPhotonIdx < 0) {
  double maxpt=0;
  for(unsigned int i=2; i<All_Gens.size(); i++){
    Gen gen = All_Gens[i];
    if( All_Gens.at(i).MotherIndex()<0   ) continue;
    if( !(All_Gens.at(i).PID()==22 && (All_Gens.at(i).Status()==1)) ) continue;
    if(gen.isPromptFinalState() && gen.Pt() > maxpt) {
      NearPhotonIdx=i;
      maxpt = gen.Pt();
    }
  }
      }

      if(NearPhotonIdx < 0) return 0;
      
      double phPt= All_Gens[NearPhotonIdx].Pt();
    
      if(Method == "Split"){
  if (MCSample == "WGToLNuG_01J_PtG_130"){
    if (phPt < 300) return 1.;
  }
  else if (MCSample == "WGToLNuG_01J_PtG_300"){
    if (phPt < 500) return 1.;
  }
  else if (MCSample == "WGToLNuG_01J_PtG_500"){
    return 1.;
      }
  else {
    if (phPt <= 130)  return 1.;
    else return 0.;
  }
      }
      
      if(Method.Contains("Combine")){
  double nWG = (Method == "CombineAll") ? 2. : 1.;
  if (phPt < 130 ) return (1./nWG);
  if (phPt < 300 ) return (1./(nWG+1.));
  if (phPt < 500 ) return (1./(nWG+2.));
  return (1./(nWG+3.));
      
      }
    }
  }
  
  return 1.;
}


vector<AnalyzerParameter::Syst> HNL_LeptonCore::GetSystList(TString SystType){

  vector<AnalyzerParameter::Syst> SystList = {};

  if(SystType=="Theory"){
    //SystList.push_back(AnalyzerParameter::PDFUp);
    //SystList.push_back(AnalyzerParameter::PDFDown);
    SystList.push_back(AnalyzerParameter::PDF);
    SystList.push_back(AnalyzerParameter::ScaleUp);
    SystList.push_back(AnalyzerParameter::ScaleDown);
    return SystList;
  }

  if(SystType=="Muon_Reco"){
    SystList.push_back(AnalyzerParameter::MuonRecoSFUp);
    SystList.push_back(AnalyzerParameter::MuonRecoSFDown);
    return SystList;
  }
  if(SystType=="Muon"){
    SystList.push_back(AnalyzerParameter::MuonRecoSFUp);
    SystList.push_back(AnalyzerParameter::MuonRecoSFDown);
    SystList.push_back(AnalyzerParameter::MuonResUp);
    SystList.push_back(AnalyzerParameter::MuonResDown);
    SystList.push_back(AnalyzerParameter::MuonEnUp);
    SystList.push_back(AnalyzerParameter::MuonEnDown);
    SystList.push_back(AnalyzerParameter::MuonIDSFUp);
    SystList.push_back(AnalyzerParameter::MuonIDSFDown);
    SystList.push_back(AnalyzerParameter::MuonTriggerSFUp);
    SystList.push_back(AnalyzerParameter::MuonTriggerSFDown);
    return SystList;
  }
  if(SystType=="Jet"){
    SystList.push_back(AnalyzerParameter::JetResUp);
    SystList.push_back(AnalyzerParameter::JetResDown);
    SystList.push_back(AnalyzerParameter::JetEnUp);
    SystList.push_back(AnalyzerParameter::JetEnDown);

    return SystList;
  }
  if(SystType == "MET"){
    SystList = {AnalyzerParameter::METUnclUp,AnalyzerParameter::METUnclDown};
    return SystList;
  }

  if(!HasFlag("RunSyst")) return SystList;

  if(RunCF){
    SystList = {
      AnalyzerParameter::CFRateUp,
      AnalyzerParameter::CFRateDown,
      //AnalyzerParameter::CFSFUp, // Apply 15% in Datacard JH: we decided to use this single lnN rather than running multiple sources
      //AnalyzerParameter::CFSFDown
    };
  }
  
  else if(RunFake){
    SystList.push_back(AnalyzerParameter::FRUp); //// Add 
    SystList.push_back(AnalyzerParameter::FRDown);
    //SystList.push_back(AnalyzerParameter::FRAJUp); /// Apply 20%? in DataCard
    //SystList.push_back(AnalyzerParameter::FRAJDown); 
    //SystList.push_back(AnalyzerParameter::FRPartonSFUp); /// Apply 20%? in DataCard (See the AN)
    //SystList.push_back(AnalyzerParameter::FRPartonSFDown);
    if(SystType!="MuMu"){
      SystList.push_back(AnalyzerParameter::FRHighPtUp);
      SystList.push_back(AnalyzerParameter::FRHighPtDown);
    }
  }
  else {
    
    if(IsData) return {};
    
    if(HasFlag("ScanSystematic")) {
      SystList = {AnalyzerParameter::METUnclUp,AnalyzerParameter::METUnclDown,
	AnalyzerParameter::JetResUp,AnalyzerParameter::JetResDown,
	AnalyzerParameter::JetEnUp, AnalyzerParameter::JetEnDown};
      
      return SystList;
    }
    if(HasFlag("RunSyst")){
      
      SystList = {AnalyzerParameter::JetResUp,AnalyzerParameter::JetResDown,
                  AnalyzerParameter::JetEnUp, AnalyzerParameter::JetEnDown,
                  AnalyzerParameter::JetPUIDUp,AnalyzerParameter::JetPUIDDown,
                  AnalyzerParameter::JetPNETUp,AnalyzerParameter::JetPNETDown,
                  AnalyzerParameter::HEMJet,
                  AnalyzerParameter::BTagSFHTagCorrUp,AnalyzerParameter::BTagSFHTagCorrDown,
                  AnalyzerParameter::BTagSFLTagCorrUp,AnalyzerParameter::BTagSFLTagCorrDown,
                  AnalyzerParameter::BTagSFHTagUnCorrUp,AnalyzerParameter::BTagSFHTagUnCorrDown,
                  AnalyzerParameter::BTagSFLTagUnCorrUp,AnalyzerParameter::BTagSFLTagUnCorrDown,
                  AnalyzerParameter::METUnclUp,AnalyzerParameter::METUnclDown,
                  AnalyzerParameter::PrefireUp,AnalyzerParameter::PrefireDown,
                  AnalyzerParameter::PUUp,AnalyzerParameter::PUDown};

      if(SystType=="MuMu" || SystType=="EMu"){
        SystList.push_back(AnalyzerParameter::MuonRecoSFUp);
        SystList.push_back(AnalyzerParameter::MuonRecoSFDown);
        SystList.push_back(AnalyzerParameter::MuonEnUp);
        SystList.push_back(AnalyzerParameter::MuonEnDown);
        SystList.push_back(AnalyzerParameter::MuonResUp);
        SystList.push_back(AnalyzerParameter::MuonResDown);
        SystList.push_back(AnalyzerParameter::MuonIDSFUp);
        SystList.push_back(AnalyzerParameter::MuonIDSFDown);
        SystList.push_back(AnalyzerParameter::MuonTriggerSFUp);
        SystList.push_back(AnalyzerParameter::MuonTriggerSFDown);
      }
      if(SystType=="EE" || SystType=="EMu"){
        SystList.push_back(AnalyzerParameter::ElectronRecoSFUp);
        SystList.push_back(AnalyzerParameter::ElectronRecoSFDown);
        SystList.push_back(AnalyzerParameter::ElectronResUp);
        SystList.push_back(AnalyzerParameter::ElectronResDown);
        SystList.push_back(AnalyzerParameter::ElectronEnUp);
        SystList.push_back(AnalyzerParameter::ElectronEnDown);
        SystList.push_back(AnalyzerParameter::ElectronIDSFUp);
        SystList.push_back(AnalyzerParameter::ElectronIDSFDown);
        SystList.push_back(AnalyzerParameter::ElectronTriggerSFUp);
        SystList.push_back(AnalyzerParameter::ElectronTriggerSFDown);
      }

      bool RunFullJEC=false;
      if(RunFullJEC){
        SystList.push_back(AnalyzerParameter::JetAbsoluteStatUp);
        SystList.push_back(AnalyzerParameter::JetAbsoluteStatDown);
        SystList.push_back(AnalyzerParameter::JetAbsoluteScaleUp);
        SystList.push_back(AnalyzerParameter::JetAbsoluteScaleDown);
        SystList.push_back(AnalyzerParameter::JetAbsoluteMPFBiasUp);
        SystList.push_back(AnalyzerParameter::JetAbsoluteMPFBiasDown);
        SystList.push_back(AnalyzerParameter::JetFragmentationUp);
        SystList.push_back(AnalyzerParameter::JetFragmentationDown);
        SystList.push_back(AnalyzerParameter::JetSinglePionECALUp);
        SystList.push_back(AnalyzerParameter::JetSinglePionECALDown);
        SystList.push_back(AnalyzerParameter::JetSinglePionHCALUp);
        SystList.push_back(AnalyzerParameter::JetSinglePionHCALDown);
        SystList.push_back(AnalyzerParameter::JetFlavorQCDUp);
        SystList.push_back(AnalyzerParameter::JetFlavorQCDDown);
        SystList.push_back(AnalyzerParameter::JetTimePtEtaUp);
        SystList.push_back(AnalyzerParameter::JetTimePtEtaDown);
        SystList.push_back(AnalyzerParameter::JetRelativeJEREC1Up);
        SystList.push_back(AnalyzerParameter::JetRelativeJEREC1Down);
        SystList.push_back(AnalyzerParameter::JetRelativeJEREC2Up);
        SystList.push_back(AnalyzerParameter::JetRelativeJEREC2Down);
        SystList.push_back(AnalyzerParameter::JetRelativePtHFUp);
        SystList.push_back(AnalyzerParameter::JetRelativePtHFDown);
        SystList.push_back(AnalyzerParameter::JetRelativePtBBUp);
        SystList.push_back(AnalyzerParameter::JetRelativePtBBDown);
        SystList.push_back(AnalyzerParameter::JetRelativePtEC1Up);
        SystList.push_back(AnalyzerParameter::JetRelativePtEC1Down);
        SystList.push_back(AnalyzerParameter::JetRelativePtEC2Up);
        SystList.push_back(AnalyzerParameter::JetRelativePtEC2Down);
        SystList.push_back(AnalyzerParameter::JetRelativeBalUp);
        SystList.push_back(AnalyzerParameter::JetRelativeBalDown);
        SystList.push_back(AnalyzerParameter::JetRelativeSampleUp);
        SystList.push_back(AnalyzerParameter::JetRelativeSampleDown);
        SystList.push_back(AnalyzerParameter::JetRelativeFSRUp);
        SystList.push_back(AnalyzerParameter::JetRelativeFSRDown);
        SystList.push_back(AnalyzerParameter::JetRelativeStatFSRUp);
        SystList.push_back(AnalyzerParameter::JetRelativeStatFSRDown);
        SystList.push_back(AnalyzerParameter::JetRelativeStatECUp);
        SystList.push_back(AnalyzerParameter::JetRelativeStatECDown);
        SystList.push_back(AnalyzerParameter::JetRelativeStatHFUp);
        SystList.push_back(AnalyzerParameter::JetRelativeStatHFDown);
        SystList.push_back(AnalyzerParameter::JetPileUpDataMCUp);
        SystList.push_back(AnalyzerParameter::JetPileUpDataMCDown);
        SystList.push_back(AnalyzerParameter::JetPileUpPtRefUp);
        SystList.push_back(AnalyzerParameter::JetPileUpPtRefDown);
        SystList.push_back(AnalyzerParameter::JetPileUpPtBBUp);
        SystList.push_back(AnalyzerParameter::JetPileUpPtBBDown);
        SystList.push_back(AnalyzerParameter::JetPileUpPtEC1Up);
        SystList.push_back(AnalyzerParameter::JetPileUpPtEC1Down);
        SystList.push_back(AnalyzerParameter::JetPileUpPtEC2Up);
        SystList.push_back(AnalyzerParameter::JetPileUpPtEC2Down);
        SystList.push_back(AnalyzerParameter::JetPileUpPtHFUp);
        SystList.push_back(AnalyzerParameter::JetPileUpPtHFDown);
      }
    }
  }
  
  if(IsSignal()){
    SystList.push_back(AnalyzerParameter::PDF);
    SystList.push_back(AnalyzerParameter::PDFUp);
    SystList.push_back(AnalyzerParameter::PDFDown);
    SystList.push_back(AnalyzerParameter::ScaleUp);
    SystList.push_back(AnalyzerParameter::ScaleDown);
  }
  
  return SystList;

}

//====================================================/====================================================
//====================================================/====================================================
//====================================================/====================================================

AnalyzerParameter HNL_LeptonCore::InitialiseHNLParameter(const TString& s_setup){
  return SetupHNLParameter(s_setup,"Default");
}

AnalyzerParameter HNL_LeptonCore::InitialiseHNLParameter(const TString& s_setup, HNL_LeptonCore::Channel channel){
  
  AnalyzerParameter p = SetupHNLParameter(s_setup,GetChannelString(channel));
  //if(_jentry== 1 )   cout << "HNL_LeptonCore::InitialiseHNLParameter SetupHNLParameter Event " << event << endl;
  //p.PrintParameters();
  //}
  return p;  
}


AnalyzerParameter HNL_LeptonCore::SetupFakeParameter(AnalyzerParameter::Syst SystType, HNL_LeptonCore::Channel channel,
                 HNL_LeptonCore::NormMC norm, const std::vector<TString>& s_jobs,
                 const TString& PNAME, const TString& IDT, const TString& IDL){

  AnalyzerParameter param  ;
  param.Clear();
  param.SetChannel(GetChannelString(channel));
  param.Name     = GetChannelString(channel)+"_"+PNAME;
  param.DefName  = PNAME;
  param.CutFlowDir = "CutFlowDir";
  param.hprefix  = "";
  param.hpostfix = "";
  param.Jobs = s_jobs;
  param.Apply_Weight_Norm1Ipb  = true;
  param.Apply_Weight_LumiNorm = true;
  param.Apply_Weight_SumQ     = true;
  param.Apply_Weight_PileUp   = true;
  param.Apply_Weight_PreFire  = true;
  param.Apply_Weight_kFactor  = true;
  param.Apply_Weight_RECOSF   = true;
  param.Apply_Weight_MuonTrackerSF = true;
  param.Apply_Weight_BJetSF   = true;
  param.Apply_Weight_PNETSF   = false;
  param.Apply_Weight_JetPUID   = false;

  //// By default dont apply ID/Trigger SF                                                                           
  param.Apply_Weight_IDSF     = true;
  param.Apply_Weight_TriggerSF= false;
  
  if(norm==HNL_LeptonCore::NoNorm){
    param.Apply_Weight_LumiNorm = false;
    param.Apply_Weight_Norm1Ipb  = false;
  }
  if(norm==HNL_LeptonCore::NormTo1Invpb)    {
    param.Apply_Weight_LumiNorm = false;
    param.Apply_Weight_Norm1Ipb  = true;
  }
  if(norm==HNL_LeptonCore::NormToXsec)  {
    param.Apply_Weight_Norm1Ipb  = true;
    param.Apply_Weight_LumiNorm = true;
  }

  // Default settings if NOT s_setup_version is set                                                                                                                                                       

  param.syst_ = SystType;
  param.MCCorrrectionIgnoreNoHist = false;

  /// Lepton ID DEFAULT                                                                                                                                                                                   
  param.k.Electron_RECO_SF   = "RECO_SF";  // RECO_SF_AFB is alternative SF                                                                                                                               
  param.k.Muon_RECO_SF   = "HighPtMuonRecoSF";
  param.Muon_Veto_ID     = "HNVetoMVA"; 
  param.Electron_Veto_ID = "HNVetoMVA"; 
  if(channel == MuMu)  {
    param.Electron_Loose_ID = "HNVetoMVA";
    param.Electron_Tight_ID = "HNVetoMVA";
    param.Muon_Loose_ID = IDL;
    param.Muon_Tight_ID = IDT;
  }
  else  {
    param.Muon_Tight_ID = "HNVetoMVA";
    param.Muon_Loose_ID = "HNVetoMVA";
    param.Electron_Loose_ID = IDL;
    param.Electron_Tight_ID = IDT;
  }

  param.Tau_Veto_ID      = "JetVLElVLMuVL";
  /// ---------                                                                                                                                                                                           
  param.Muon_MinPt = 5.;       param.Muon_MaxEta = 2.4;
  param.Electron_MinPt = 10.;  param.Electron_MaxEta = 2.5;
  /// JET ID DEFAULT                                                                                                                                                                                      
  param.Jet_ID                     = "tight";
  param.Jet_MinPt                  = 30.;
  param.Jet_MaxEta                 = 5.;
  /// ---------                                                                                                                                                                                           
  param.FatJet_ID                  = "tight";
  param.FatJet_MinPt = 200.;  param.FatJet_MaxEta = 5.;

  param.TriggerSelection = "Dilep";
  param.BTagger = "DeepJet";  param.BWP ="M";
  param.JetPUID = "Loose";

  param.AK4JetColl       = "TightPUL";
  param.AK4VBFJetColl    = "VBFTightPUL";
  param.AK8JetColl       = "HNL_ParticleNet";
  param.BJetColl         = "Tight";

  //// Weights                                                                                                                                              
  param.w.lumiweight= 1;  param.w.PUweight=1;  param.w.PUweight_up=1;  param.w.PUweight_down=1;  param.w.prefireweight=1;
  param.w.prefireweight_up=1;  param.w.prefireweight_down=1;  param.w.z0weight=1;  param.w.zptweight=1;param.w.weakweight=1;
  param.w.topptweight=1;  param.w.muonRECOSF=1;  param.w.electronRECOSF=1;  param.w.electronIDSF=1;  param.w.muonIDSF=1;
  param.w.muonISOSF=1;  param.w.triggerSF=1;  param.w.CFSF=1;param.w.btagSF=1;  param.w.PNETSF=1;param.w.JetPU = 1;
  param.w.EventSetupWeight=1;
  if(!IsDATA){
    param.w.lumiweight*= MCweight()*_Event.GetTriggerLumi("Full");
    param.w.PUweight      = GetPileUpWeight(nPileUp,0);
    param.w.PUweight_up   = GetPileUpWeight(nPileUp,1);
    param.w.PUweight_down = GetPileUpWeight(nPileUp,-1);
    param.w.prefireweight =  GetPrefireWeight(0);
    param.w.prefireweight_up= GetPrefireWeight(1);
    param.w.prefireweight_down =GetPrefireWeight(-1);
    param.w.z0weight=GetZ0Weight(vertex_Z);
  }

  param.FakeMethod = "DATA";
  param.CFMethod   = "DATA";
  if (PNAME.Contains("MCFakes"))    param.FakeMethod = "MC";
  
  return param;

}


TString HNL_LeptonCore::GetPtBin(bool muon, double pt){
  TString pt_label="";
  
  if(pt< 50) pt_label = "_ptbin1"; 
  else if(pt < 100) pt_label = "_ptbin2"; 
  else if(pt < 400) pt_label = "_ptbin3";
  else pt_label = "_ptbin4";                                                               
  return pt_label;

}

bool  HNL_LeptonCore::UpdateParamBySyst(TString JobID, AnalyzerParameter& paramEv , AnalyzerParameter::Syst systname, const TString& OrigParamName){

  //// Update Name of param based on systematic settings
  paramEv.syst_   = systname;
  paramEv.Name    = "Syst_"+paramEv.GetSystType()+OrigParamName;
  paramEv.DefName = "Syst_"+paramEv.GetSystType()+OrigParamName;
 
  if(!RunFake) return true;

  //// This function updates the ID/Keys for Fakes based on systematic settings

  /// If not HNL_ULID setting return 
  
  if((JobID != "HNL_ULID")&&(JobID != "HNL_ULIDv2")) return false;

  //// Setup FR ID
  if(paramEv.syst_ == AnalyzerParameter::FRLooseIDDJUp){
    paramEv.Muon_FR_ID        = "HNL_ULID_FO_Up";
    paramEv.Electron_FR_ID    = "HNL_HighPt_ULID_FO_Up";
  }
  else if(paramEv.syst_ == AnalyzerParameter::FRLooseIDDJDown){
    paramEv.Muon_FR_ID        = "HNL_ULID_FO_Down";
    paramEv.Electron_FR_ID    = "HNL_HighPt_ULID_FO_Down";
  }
  else{
    paramEv.Muon_FR_ID        = "HNL_ULID_FO";
    paramEv.Electron_FR_ID    = "HNL_HighPt_ULID_FO";
  }

  TString MuFRBin = (paramEv.syst_ ==AnalyzerParameter::FRAltBinning) ? "_Binv2" : "";
  TString ElFRBin = (paramEv.syst_ ==AnalyzerParameter::FRAltBinning) ? "" : "_El12";

  TString JFRJetPt = "_AJ40";
  if(paramEv.syst_ ==AnalyzerParameter::FRAJUp) JFRJetPt = "_AJ30";
  if(paramEv.syst_ ==AnalyzerParameter::FRAJDown) JFRJetPt = "_AJ60";

  if(GetEra() == "2016preVFP"){

    if(paramEv.syst_ == AnalyzerParameter::FRLooseIDDJUp){
      paramEv.k.Muon_FR            = "HNL_ULID_FO_v1_a"+JFRJetPt+MuFRBin;
      paramEv.k.Electron_FR        = "HNL_ULID_FO_v8_a"+JFRJetPt+ElFRBin;
    }
    else  if(paramEv.syst_ == AnalyzerParameter::FRLooseIDDJDown){
      paramEv.k.Muon_FR            = "HNL_ULID_FO_v2_a"+JFRJetPt+MuFRBin;
      paramEv.k.Electron_FR        = "HNL_ULID_FO_v0"+JFRJetPt+ElFRBin;
    }
    else{
      paramEv.k.Muon_FR            = "HNL_ULID_FO_v1_a"+JFRJetPt+MuFRBin;
      paramEv.k.Electron_FR        = "HNL_ULID_FO_v9_a"+JFRJetPt+ElFRBin;
    }
  }
  if(GetEra() == "2016postVFP"){

    if(paramEv.syst_ == AnalyzerParameter::FRLooseIDDJUp){
      paramEv.k.Muon_FR            = "HNL_ULID_FO_v1_a"+JFRJetPt+MuFRBin;
      paramEv.k.Electron_FR        = "HNL_ULID_FO_v8_a"+JFRJetPt+ElFRBin;
    }
    else  if(paramEv.syst_ == AnalyzerParameter::FRLooseIDDJDown){
      paramEv.k.Muon_FR            = "HNL_ULID_FO_v3_a"+JFRJetPt+MuFRBin;
      paramEv.k.Electron_FR        = "HNL_ULID_FO_v0"+JFRJetPt+ElFRBin;
    }
    else {
      paramEv.k.Muon_FR            = "HNL_ULID_FO_v2_a"+JFRJetPt+MuFRBin;
      paramEv.k.Electron_FR        = "HNL_ULID_FO_v9_a"+JFRJetPt+ElFRBin;
    }
  }
  if(GetYearString() == "2017"){
    if(paramEv.syst_ == AnalyzerParameter::FRLooseIDDJUp){
      paramEv.k.Muon_FR            = "HNL_ULID_FO_v1_a"+JFRJetPt+MuFRBin;
      paramEv.k.Electron_FR        = "HNL_ULID_FO_v8_a"+JFRJetPt+ElFRBin;

    }
    else  if(paramEv.syst_ == AnalyzerParameter::FRLooseIDDJDown){
      paramEv.k.Muon_FR            = "HNL_ULID_FO_v3_a"+JFRJetPt+MuFRBin;;
      paramEv.k.Electron_FR        = "HNL_ULID_FO_v0"+JFRJetPt+ElFRBin;
    }
    else {
      paramEv.k.Muon_FR            = "HNL_ULID_FO_v2_a"+JFRJetPt+MuFRBin;;
      paramEv.k.Electron_FR        = "HNL_ULID_FO_v9_a"+JFRJetPt+ElFRBin;
    }
  }
  if(GetYearString() == "2018"){
    if(paramEv.syst_ == AnalyzerParameter::FRLooseIDDJUp){
      paramEv.k.Muon_FR            = "HNL_ULID_FO_v1_a"+JFRJetPt+MuFRBin;
      paramEv.k.Electron_FR        = "HNL_ULID_FO_v8_a"+JFRJetPt+ElFRBin;
    }
    else  if(paramEv.syst_ == AnalyzerParameter::FRLooseIDDJDown){
      paramEv.k.Muon_FR            = "HNL_ULID_FO_v4_a"+JFRJetPt+MuFRBin;
      paramEv.k.Electron_FR        = "HNL_ULID_FO_v0"+JFRJetPt+ElFRBin;
    }
    else {
      paramEv.k.Muon_FR         = "HNL_ULID_FO_v3_a"+JFRJetPt+MuFRBin;;
      paramEv.k.Electron_FR     = "HNL_ULID_FO_v9_a"+JFRJetPt+ElFRBin;
    }
  }


  return true;
}


AnalyzerParameter HNL_LeptonCore::SetupHNLParameter(const TString& s_setup_version, const TString& channel_str_name){

  /// Main IDs
  if (s_setup_version=="HNL_ULID")   return Setup_Param_HNL_ULID(s_setup_version,channel_str_name);
  if (s_setup_version=="HNL_ULIDv2") return Setup_Param_HNL_ULIDv2(s_setup_version,channel_str_name);
  if (s_setup_version=="POGTight")   return Setup_Param_POGTight(s_setup_version,channel_str_name);
  if (s_setup_version=="HNTightV2")  return Setup_Param_HNTightV2(s_setup_version,channel_str_name);

  /// Other configurations
  if (s_setup_version=="MVAPOG")     return Setup_Param_MVAPOG(s_setup_version,channel_str_name);
  if (s_setup_version=="HighPt")     return Setup_Param_HighPt(s_setup_version,channel_str_name);
  if (s_setup_version=="EXO17028")   return Setup_Param_HNL16(s_setup_version,channel_str_name);
  if (s_setup_version=="TopHN")      return Setup_Param_HNLTopID(s_setup_version,channel_str_name);
  if (s_setup_version=="Peking")     return Setup_Param_Peking(s_setup_version,channel_str_name);
  if (s_setup_version=="HNL_Opt")    return Setup_Param_HNLOpt(s_setup_version,channel_str_name);
  if (s_setup_version=="BDT")        return Setup_Param_BDT(s_setup_version,channel_str_name);
  
  /// Other non defined setups
  AnalyzerParameter param  =  DefaultParam(s_setup_version, channel_str_name);
  if (s_setup_version=="")      return param;
  if (s_setup_version=="Basic") return param;

  if (s_setup_version == "FakeRate" ){
    param.Apply_Weight_LumiNorm = false;
    return param;
  }
  
  if (s_setup_version=="SignalStudy" || s_setup_version=="MCBkg"){
    param.FakeMethod = "MC";
    param.CFMethod   = "MC";
    param.ConvMethod = "MC";
    return param;
  }
  if (s_setup_version=="MCStudy"){
    param.FakeMethod = "MC";
    param.CFMethod   = "MC";
    param.ConvMethod = "MC";
    param.Muon_Tight_ID     = "HNL_ULID_"+GetYearString();
    param.Electron_Tight_ID = "HNL_ULID_"+GetYearString();
    return param;
  }

  return param;

  cout << "[HNL_LeptonCore::InitialiseHNLParameters ] ID not found.." << endl;
  exit(EXIT_FAILURE);
  
}

  

double HNL_LeptonCore::GetKFactor(){

  if(IsDATA) return 1.;

  double weight = 1.;

  if(MCSample.Contains("WZTo3LNu_powheg") or MCSample.Contains("WZTo3LNu_mllmin4p0_powheg") or MCSample.Contains("WZTo2L2Q")){
    //Physics Letters B 761 (2016) 197                                                                                                                                                                                                                                          
    //http://dx.doi.org/10.1016/j.physletb.2016.08.017                                                                                                                                                                                                                          
    weight = 1.109;
  }
  else if(MCSample.Contains("ZZTo4L_powheg") or MCSample.Contains("ZZTo2L2Nu") or MCSample.Contains("ZZTo2L2Q")){
    // Physics Letters B 735 (2014) 311-313                                                                                                                                                                                                                                    
    // https://doi.org/10.1016/j.physletb.2014.06.056                                                                                                                                                                                                                           
    weight = ZZKfactor("ZZMass");
    //    weight =  1.16;
    if(HasFlag("ZZKFMass"))  weight = ZZKfactor("ZZMass");
    if(HasFlag("ZZKFPt"))    weight = ZZKfactor("ZZPt");
    //    if(_jentry % 1000) cout << "K-Factor ZZMass = " << ZZKfactor("ZZMass") << " K-Factor ZZPt " << ZZKfactor("ZZPt") << " " << endl;
    
    return weight;
  }
  else if(MCSample.Contains("GluGluToZZto")){
    //  2.3 brings gg->ZZ from LO to NNLO (https://www.arxiv.org/pdf/1504.02388)
    return 2.2;

  }
  else if(MCSample.Contains("GluGluHToZZ")){

    return 1.67;
    //AN2016_359                                                                                                                                                                                                                                                                
  }
  else if(MCSample.Contains("ttZ") && !MCSample.Contains("To")){
    weight = 839.3/780.;
  }
  else if(MCSample.Contains("ttW") && !MCSample.Contains("To")){
    weight = 600.8/610.;
  }
  else if(MCSample.Contains("WJet") && MCSample.Contains("HT")){
    return 1.21;
  }

  if(MCSample.Contains("WZTo3LNu_mllmin0p1_powheg"))     weight = 0.632; //// This is done from WZ CR Norm                                                                                                                                                                      
  vector<TString> EWK_Corr_VV_Samples = {"WZ_EWK","WpWp_EWK"};
  if (std::find(EWK_Corr_VV_Samples.begin(), EWK_Corr_VV_Samples.end(), MCSample) != EWK_Corr_VV_Samples.end()) {

    double  mymjj_EW = GetGenJJMass();
    if(mymjj_EW < 525) mymjj_EW=525;
    if(mymjj_EW > 1975) mymjj_EW=1900;

    float nominal_EW_correction = h_VV_KF_CMS->GetBinContent(h_VV_KF_CMS->GetXaxis()->FindFixBin(mymjj_EW));                                                            
    return nominal_EW_correction;
  }


  return weight;

}

double HNL_LeptonCore::ZZKfactor(TString method){

  // finalState=1 : 4e/4mu/4tau
  // finalState=2 : 2e2mu/2mutau/2e2tau

  int finalState = GetZZFinalState();
  float k=0.0;
  //  cout << "GetGenZZPt() = " << GetGenZZPt() << "  " << GetGenZZMass() << endl;
  if(method=="ZZPt"){
    double GENpTZZ = GetGenZZPt();
    if (finalState==1) {
      k+=0.64155491983*(GENpTZZ>0.0&&GENpTZZ<=5.0);
      k+=1.09985240531*(GENpTZZ>5.0&&GENpTZZ<=10.0);
      k+=1.29390628654*(GENpTZZ>10.0&&GENpTZZ<=15.0);
      k+=1.37859998571*(GENpTZZ>15.0&&GENpTZZ<=20.0);
      k+=1.42430263312*(GENpTZZ>20.0&&GENpTZZ<=25.0);
      k+=1.45038493266*(GENpTZZ>25.0&&GENpTZZ<=30.0);
      k+=1.47015377651*(GENpTZZ>30.0&&GENpTZZ<=35.0);
      k+=1.48828685748*(GENpTZZ>35.0&&GENpTZZ<=40.0);
      k+=1.50573440448*(GENpTZZ>40.0&&GENpTZZ<=45.0);
      k+=1.50211655928*(GENpTZZ>45.0&&GENpTZZ<=50.0);
      k+=1.50918720827*(GENpTZZ>50.0&&GENpTZZ<=55.0);
      k+=1.52463089491*(GENpTZZ>55.0&&GENpTZZ<=60.0);
      k+=1.52400838378*(GENpTZZ>60.0&&GENpTZZ<=65.0);
      k+=1.52418067701*(GENpTZZ>65.0&&GENpTZZ<=70.0);
      k+=1.55424382578*(GENpTZZ>70.0&&GENpTZZ<=75.0);
      k+=1.52544284222*(GENpTZZ>75.0&&GENpTZZ<=80.0);
      k+=1.57896384602*(GENpTZZ>80.0&&GENpTZZ<=85.0);
      k+=1.53034682567*(GENpTZZ>85.0&&GENpTZZ<=90.0);
      k+=1.56147329708*(GENpTZZ>90.0&&GENpTZZ<=95.0);
      k+=1.54468169268*(GENpTZZ>95.0&&GENpTZZ<=100.0);
      k+=1.57222952415*(GENpTZZ>100.0);
    }
    
    if (finalState==2) {
      k+=0.743602533303*(GENpTZZ>0.0&&GENpTZZ<=5.0);
      k+=1.14789453219*(GENpTZZ>5.0&&GENpTZZ<=10.0);
      k+=1.33815867892*(GENpTZZ>10.0&&GENpTZZ<=15.0);
      k+=1.41420044104*(GENpTZZ>15.0&&GENpTZZ<=20.0);
      k+=1.45511318916*(GENpTZZ>20.0&&GENpTZZ<=25.0);
      k+=1.47569225244*(GENpTZZ>25.0&&GENpTZZ<=30.0);
      k+=1.49053003693*(GENpTZZ>30.0&&GENpTZZ<=35.0);
      k+=1.50622827695*(GENpTZZ>35.0&&GENpTZZ<=40.0);
      k+=1.50328889799*(GENpTZZ>40.0&&GENpTZZ<=45.0);
      k+=1.52186945281*(GENpTZZ>45.0&&GENpTZZ<=50.0);
      k+=1.52043468754*(GENpTZZ>50.0&&GENpTZZ<=55.0);
      k+=1.53977869986*(GENpTZZ>55.0&&GENpTZZ<=60.0);
      k+=1.53491994434*(GENpTZZ>60.0&&GENpTZZ<=65.0);
      k+=1.51772882172*(GENpTZZ>65.0&&GENpTZZ<=70.0);
      k+=1.54494489131*(GENpTZZ>70.0&&GENpTZZ<=75.0);
      k+=1.57762411697*(GENpTZZ>75.0&&GENpTZZ<=80.0);
      k+=1.55078339014*(GENpTZZ>80.0&&GENpTZZ<=85.0);
      k+=1.57078191891*(GENpTZZ>85.0&&GENpTZZ<=90.0);
      k+=1.56162666568*(GENpTZZ>90.0&&GENpTZZ<=95.0);
      k+=1.54183774627*(GENpTZZ>95.0&&GENpTZZ<=100.0);
      k+=1.58485762205*(GENpTZZ>100.0);
    }

    if (k==0.0) return 1.1;
    else return k; // if something goes wrong return inclusive k-factor
    
  }

  if(method=="ZZMass") {
    
    // finalState=1 : 4e/4mu/4tau
    // finalState=2 : 2e2mu/2mutau/2e2tau
    
    double GENmassZZ = GetGenZZMass();

    float k=0.0;
    
    if (finalState==1) {
      k+=1.23613311013*(GENmassZZ>0.0&&GENmassZZ<=25.0);
      k+=1.17550314639*(GENmassZZ>25.0&&GENmassZZ<=50.0);
      k+=1.17044565911*(GENmassZZ>50.0&&GENmassZZ<=75.0);
      k+=1.03141209689*(GENmassZZ>75.0&&GENmassZZ<=100.0);
      k+=1.05285574912*(GENmassZZ>100.0&&GENmassZZ<=125.0);
      k+=1.11287217794*(GENmassZZ>125.0&&GENmassZZ<=150.0);
      k+=1.13361441158*(GENmassZZ>150.0&&GENmassZZ<=175.0);
      k+=1.10355603327*(GENmassZZ>175.0&&GENmassZZ<=200.0);
      k+=1.10053981637*(GENmassZZ>200.0&&GENmassZZ<=225.0);
      k+=1.10972676811*(GENmassZZ>225.0&&GENmassZZ<=250.0);
      k+=1.12069120525*(GENmassZZ>250.0&&GENmassZZ<=275.0);
      k+=1.11589101635*(GENmassZZ>275.0&&GENmassZZ<=300.0);
      k+=1.13906170314*(GENmassZZ>300.0&&GENmassZZ<=325.0);
      k+=1.14854594271*(GENmassZZ>325.0&&GENmassZZ<=350.0);
      k+=1.14616229031*(GENmassZZ>350.0&&GENmassZZ<=375.0);
      k+=1.14573157789*(GENmassZZ>375.0&&GENmassZZ<=400.0);
      k+=1.13829430515*(GENmassZZ>400.0&&GENmassZZ<=425.0);
      k+=1.15521193686*(GENmassZZ>425.0&&GENmassZZ<=450.0);
      k+=1.13679822698*(GENmassZZ>450.0&&GENmassZZ<=475.0);
      k+=1.13223956942*(GENmassZZ>475.0);
    }

    if (finalState==2) {
      k+=1.25094466582*(GENmassZZ>0.0&&GENmassZZ<=25.0);
      k+=1.22459455362*(GENmassZZ>25.0&&GENmassZZ<=50.0);
      k+=1.19287368979*(GENmassZZ>50.0&&GENmassZZ<=75.0);
      k+=1.04597506451*(GENmassZZ>75.0&&GENmassZZ<=100.0);
      k+=1.08323413771*(GENmassZZ>100.0&&GENmassZZ<=125.0);
      k+=1.09994968030*(GENmassZZ>125.0&&GENmassZZ<=150.0);
      k+=1.16698455800*(GENmassZZ>150.0&&GENmassZZ<=175.0);
      k+=1.10399053155*(GENmassZZ>175.0&&GENmassZZ<=200.0);
      k+=1.10592664340*(GENmassZZ>200.0&&GENmassZZ<=225.0);
      k+=1.10690381480*(GENmassZZ>225.0&&GENmassZZ<=250.0);
      k+=1.11194928918*(GENmassZZ>250.0&&GENmassZZ<=275.0);
      k+=1.13522586553*(GENmassZZ>275.0&&GENmassZZ<=300.0);
      k+=1.11895090244*(GENmassZZ>300.0&&GENmassZZ<=325.0);
      k+=1.13898508615*(GENmassZZ>325.0&&GENmassZZ<=350.0);
      k+=1.15463977506*(GENmassZZ>350.0&&GENmassZZ<=375.0);
      k+=1.17341664594*(GENmassZZ>375.0&&GENmassZZ<=400.0);
      k+=1.20093349763*(GENmassZZ>400.0&&GENmassZZ<=425.0);
      k+=1.18915554919*(GENmassZZ>425.0&&GENmassZZ<=450.0);
      k+=1.18546007375*(GENmassZZ>450.0&&GENmassZZ<=475.0);
      k+=1.12864505708*(GENmassZZ>475.0);
    }

    if (k==0.0) return 1.1;
    else return k; // if something goes wrong return inclusive k-factor
    
  }

  cout << "[HNL_LeptonCore::ZZKFatcor ] method not found.." << endl;
  exit(EXIT_FAILURE);



  return -99999999999;
}

double HNL_LeptonCore::SetupWeight(Event ev, AnalyzerParameter& param){

  //=== Apply MC weight                                                                                                                                                                                    
  if(IsDATA) return 1.;

  double prefire_weight = 1.;
  if(param.Apply_Weight_PreFire){
    if(param.syst_ == AnalyzerParameter::PrefireUp) prefire_weight = GetPrefireWeight(1);
    else if(param.syst_ == AnalyzerParameter::PrefireDown)  prefire_weight = GetPrefireWeight(-1);
    else  prefire_weight = GetPrefireWeight(0);
    FillWeightHist(param.ChannelDir()+"/PrefireWeight" ,prefire_weight);
  }

  double pileup_weight(1.);
  if(param.Apply_Weight_PileUp){
    if(param.syst_ == AnalyzerParameter::PUUp) pileup_weight= GetPileUpWeight(nPileUp,1);
    else if(param.syst_ == AnalyzerParameter::PUDown) pileup_weight= GetPileUpWeight(nPileUp,-1);
    else pileup_weight= GetPileUpWeight(nPileUp,0);
    FillWeightHist(param.ChannelDir()+"/PileupWeight",pileup_weight);
  }

  double this_mc_weight =  MCweight(param.Apply_Weight_SumQ, param.Apply_Weight_Norm1Ipb);
  FillWeightHist(param.ChannelDir()+"/MCWeight",    this_mc_weight);
  
  if(param.Apply_Weight_LumiNorm) FillWeightHist(param.ChannelDir()+"/LumiWeight",  ev.GetTriggerLumi("Full"));
  if(param.Apply_Weight_kFactor)  FillWeightHist(param.ChannelDir()+"/KFactor",     GetKFactor());

  if(param.Apply_Weight_LumiNorm) this_mc_weight *= ev.GetTriggerLumi("Full");
  if(param.Apply_Weight_kFactor)  this_mc_weight *= GetKFactor();
  if(param.Apply_Weight_PileUp)   this_mc_weight *= pileup_weight;
  if(param.Apply_Weight_PreFire)  this_mc_weight *= prefire_weight;
         
  if(param.Apply_Weight_Z0)      this_mc_weight *= GetZ0Weight(vertex_Z);
  if(param.Apply_Weight_TopCorr) this_mc_weight *= mcCorr->GetTopPtReweight(All_Gens);
  if(param.Apply_Weight_DYCorr)  this_mc_weight *= param.w.zptweight;
  if(param.Apply_Weight_DYCorr)  this_mc_weight *= param.w.weakweight;
  if(param.Apply_Weight_Z0) FillWeightHist(param.ChannelDir()+"/Weight_Z0",     GetZ0Weight(vertex_Z));
  if(param.Apply_Weight_TopCorr)  FillWeightHist(param.ChannelDir()+"/TopCorr",mcCorr->GetTopPtReweight(All_Gens));
  if(param.Apply_Weight_DYCorr) {
    FillWeightHist(param.ChannelDir()+"/zptweight",param.w.zptweight);
    FillWeightHist(param.ChannelDir()+"/zptweight_g",param.w.zptweight_g);
    FillWeightHist(param.ChannelDir()+"/zptweight_gy",param.w.zptweight_gy);
    FillWeightHist(param.ChannelDir()+"/zptweight_gym",param.w.zptweight_gym);
    FillWeightHist(param.ChannelDir()+"/weakweight",param.w.weakweight);
  }

  FillWeightHist(param.ChannelDir()+"/MCFullWeight_" , this_mc_weight);
  
  param.w.EventSetupWeight = this_mc_weight;
  return this_mc_weight;
  
}

HNL_LeptonCore::Channel  HNL_LeptonCore::GetChannelENum(TString ch){
  
  if(ch == "EE")   return EE;
  if(ch == "MuMu") return MuMu;
  if(ch == "EMu")  return EMu;

  return NONE;
}

HNL_LeptonCore::Channel  HNL_LeptonCore::GetTriLeptonChannel(HNL_LeptonCore::Channel channel){

  
  if(channel == EE)   return EEE;
  if(channel == MuMu) return MuMuMu;
  if(channel == EMu)  return EMuL;

  return channel;
}

HNL_LeptonCore::Channel HNL_LeptonCore::GetQuadLeptonChannel(HNL_LeptonCore::Channel channel){

  if(channel ==EE)   return EEEE;
  if(channel == MuMu) return MuMuMuMu;
  if(channel == EMu)  return EMuLL;

  return channel;
}

bool HNL_LeptonCore::CheckLeptonFlavourForChannel(HNL_LeptonCore::Channel channel, std::vector<Lepton *> leps){
  
  int n_el(0);
  int n_mu(0);
  for(auto ilep : leps)  {
    if(ilep->LeptonFlavour() == Lepton::MUON) n_mu++;
    if(ilep->LeptonFlavour() == Lepton::ELECTRON) n_el++;
  }
  
  if(channel==MuMu  || channel==EE || channel== EMu ){
    
    if (leps.size() != 2) return false;

    if (channel==EE     && !(leps[0]->LeptonFlavour() == Lepton::ELECTRON && leps[1]->LeptonFlavour() == Lepton::ELECTRON)) return false;
    if (channel==MuMu   && !(leps[0]->LeptonFlavour() == Lepton::MUON     && leps[1]->LeptonFlavour() == Lepton::MUON))    return false;
    if (channel==EMu    && (n_el != n_mu) ) return false;
    double lep1_ptcut= (channel==EE) ?   25. : 20.;
    double lep2_ptcut= (channel==EE) ?   10. : 10.;
    
    if(!(leps[0]->Pt() > lep1_ptcut && leps[1]->Pt()  > lep2_ptcut)) return false;
    
    return true;

  }

  
  if(channel==MuMuMu  || channel==EEE || channel==EMuL){
    
    if( leps.size() != 3) return false;
    if(channel==MuMuMu && n_mu != 3) return false;
    if(channel==EEE    && n_el != 3) return false;
    if(channel==EMuL   && (n_el == 3  || n_mu == 3)) return false;
    
    double lep1_ptcut= (channel==MuMuMu) ?   20.  : 25.;
    double lep2_ptcut= (channel==MuMuMu) ?   10   : 10.;
    double lep3_ptcut= (channel==MuMuMu) ?   10.  : 10.;

    if(!(leps[0]->Pt() > lep1_ptcut && leps[1]->Pt()  > lep2_ptcut && leps[2]->Pt()  > lep3_ptcut)) return false;

    return true;
  }

  if(channel == MuMuMuMu || channel == EEEE || channel == EMuLL ) {

    if( leps.size() != 4) return false;
    if( channel == MuMuMuMu && n_mu != 4) return false;
    if( channel == EEEE     && n_el != 4) return false;
    if( channel == EMuLL    && !(n_mu == 2 && n_mu == 2)) return false;
    
    double lep1_ptcut= (channel==MuMuMuMu) ?   20. : 25.;
    double lep2_ptcut= (channel==MuMuMuMu) ?   10. : 10.;
    double lep3_ptcut= (channel==MuMuMuMu) ?   10. : 10.;
    double lep4_ptcut= (channel==MuMuMuMu) ?   10. : 10.;

    if(!(leps[0]->Pt() > lep1_ptcut && leps[1]->Pt()  > lep2_ptcut && leps[2]->Pt()  > lep3_ptcut && leps[3]->Pt()  > lep4_ptcut)) return false;

    return true;
  }

  return true;

}


TString HNL_LeptonCore::DoubleToString(double d){
  
  std::string str = std::to_string (d);
  str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
  str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
  
  TString ts_str = TString(str);
  ts_str = ts_str.ReplaceAll(".","p");
  ts_str = ts_str.ReplaceAll("-","neg");
  
  return ts_str;
  
}




HNL_LeptonCore::HNL_LeptonCore(){
      
  rand_ = new TRandom3(1234);

  RunPromptTLRemoval= false;
  RunFake = false;
  RunCF= false;
  IsCentral=true;
  RunFullAnalysis=true;
  run_Debug=false;

}
 
HNL_LeptonCore::~HNL_LeptonCore(){
  
  cout << "Cutflow results" << endl;
  for(std::map< TString, double >::iterator mapit = cfmap.begin(); mapit!=cfmap.end(); mapit++){
    cout << "Cutflow key = " <<  mapit->first << " = " << mapit->second << endl;
  }

  // Printing results for each run
  cout << "Print cut count "<< endl;
  for (const auto& entry : runCutCounts) {
    entry.second.printCounts(entry.first);  // Print counts for each run
  }

  
  //==== Tools                         
  if(mcCorr) delete mcCorr;
  if(puppiCorr) delete puppiCorr;
  if(fakeEst) delete fakeEst;
  if(cfEst) delete cfEst;
  if(pdfReweight) delete pdfReweight;

  cfmap.clear();
 
  delete rand_;
  
  if(HasFlag("RunSyst")){
    if(IsSignal()){

      if (h_SumW_PDF != nullptr) {
  delete h_SumW_PDF;
  h_SumW_PDF = nullptr; // Set pointer to nullptr after deletion
      }
      if (h_SumW_Scale != nullptr) {
  delete h_SumW_Scale;
  h_SumW_Scale = nullptr; // Set pointer to nullptr after deletion
      }
      //      if (h_SumW_AlphaS != nullptr) {
      //delete h_SumW_AlphaS;
      //h_SumW_AlphaS = nullptr; // Set pointer to nullptr after deletion
      //}
    }
  }

  if (h_VV_KF_CMS != nullptr){
    delete h_VV_KF_CMS;
    h_VV_KF_CMS = nullptr;
  }

  DeleteZptWeight();

}


Particle HNL_LeptonCore::GetSignalObject(TString obj, TString Sig){
  
  if(Sig=="DY"){


    int N_Mother_ind(-1); // Index of N                                                                                                                                                                                                                                
    int W2_ind(0); // Index of W2 : i.e W from N decay                                                                                                                

    for(unsigned int i=2; i<All_Gens.size(); i++){
      Gen gen = All_Gens.at(i);
      if(fabs(gen.PID()) == 24 && (All_Gens.at(gen.MotherIndex()).PID() == 9900012 || All_Gens.at(gen.MotherIndex()).PID() == 9900014)){
  W2_ind= i;
  for(unsigned int i2=2; i2<All_Gens.size(); i2++){
    Gen gen2 = All_Gens.at(i2);
    if(gen2.MotherIndex() == W2_ind){
      if (fabs(gen2.PID()) == 24 ) W2_ind = i2;
    }
  }
      }

      if((gen.PID() == 9900012 || gen.PID() == 9900014) &&  gen.Status() == 22) {
  N_Mother_ind= gen.MotherIndex();
      }
    }
    Gen LepFromN;
    Gen LepFromW;
    Gen N;
    Gen W2;
    Gen j1,j2;
    bool j1IsSet(false);

    for(unsigned int i=2; i<All_Gens.size(); i++){
      Gen gen = All_Gens.at(i);

      if(int(gen.MotherIndex()) == W2_ind && gen.Status() == 23) {
  W2 = All_Gens.at(gen.MotherIndex());
  if(!j1IsSet) {  j1= gen; j1IsSet=true;}
  else j2 = gen;
      }
      if(gen.PID() == 9900012 || gen.PID() == 9900014){
  N= All_Gens.at(i);
      }
      if( ! ( ( fabs(gen.PID()) == 13)  || (fabs(gen.PID()) == 11) )) continue;

      TString LepFl = (fabs(gen.PID()) == 13) ? "Mu" : "El";

      if(All_Gens.at(gen.MotherIndex()).PID() == 9900012|| All_Gens.at(gen.MotherIndex()).PID() == 9900014) {
  LepFromN = gen;
      }
      else if(gen.MotherIndex() == N_Mother_ind){
  LepFromW=gen;
      }
    }

    if(obj=="W2") return W2;
    if(obj=="j1") return j1;
    if(obj=="j2") return j2;
    if(obj=="LepFromN") return LepFromN;
    if(obj=="LepFromW") return LepFromW;
    if(obj== "N") return N;
  }
  Particle NullPtc;
  return NullPtc;
}


TString HNL_LeptonCore::GetChannelString(HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType q) {
  TString channel_string = "";

  switch (channel) {
  case EE:      channel_string = "EE"; break;
  case MuMu:    channel_string = "MuMu"; break;
  case EMu:     channel_string = "EMu"; break;
  case EEE:     channel_string = "EEE"; break;
  case EMuL:    channel_string = "EMuL"; break;
  case MuMuMu:  channel_string = "MuMuMu"; break;
  case EEEE:    channel_string = "EEEE"; break;
  case MuMuMuMu: channel_string = "MuMuMuMu"; break;
  case EMuLL:   channel_string = "EMuLL"; break;
  default:      return "";  // Handle unknown channel
  }

  if (q == Plus) {
    channel_string += "_+";
  } else if (q == Minus) {
    channel_string += "_-";
  }

  return channel_string;
}


TString HNL_LeptonCore::QToString(HNL_LeptonCore::ChargeType q){

  if (q==Plus) return "Plus";
  if (q==Minus) return "Minus";
  else return "";

}



vector<Gen> HNL_LeptonCore::GetGenLepronsSignal(){

  bool isDYVBF=false;

  vector<Gen> gen_lep;
  int N_Mother(0);

  for (auto i : All_Gens){ Gen gen = i; if(gen.PID() == 9900012 || gen.PID() == 9900014) isDYVBF=true; }

  if(isDYVBF){

    for(unsigned int i=2; i<All_Gens.size(); i++){

      Gen gen = All_Gens.at(i);

      if((gen.PID() == 9900012 || gen.PID() == 9900014)  && gen.Status()==22) {
        N_Mother= gen.MotherIndex();
      }
    }

    for(unsigned int i=2; i<All_Gens.size(); i++){
      Gen gen = All_Gens.at(i);     TString lep_ch="";

      if((All_Gens.at(gen.MotherIndex()).PID() == 9900012|| All_Gens.at(gen.MotherIndex()).PID() == 9900014) && !(All_Gens.at(i).PID() == 9900012 || All_Gens.at(i).PID() == 9900014)){

        if(fabs(gen.PID()) == 15)     gen_lep.push_back(gen);

        if(fabs(gen.PID()) == 13)     gen_lep.push_back(gen);
        if(fabs(gen.PID()) == 11)     gen_lep.push_back(gen);

      }
      else if(gen.MotherIndex() == N_Mother&& !(All_Gens.at(i).PID() == 9900012 || All_Gens.at(i).PID() == 9900014)){


        if(fabs(gen.PID()) == 15) gen_lep.push_back(gen);

        if(fabs(gen.PID()) == 13) gen_lep.push_back(gen);
        if(fabs(gen.PID()) == 11) gen_lep.push_back(gen);
      }
    }
  }
  else{
    for(unsigned int i=2; i<All_Gens.size(); i++){
      Gen gen = All_Gens.at(i);
      if (fabs(gen.PID()) == 13 && gen.Status() == 23) gen_lep.push_back(gen);
      if (fabs(gen.PID()) == 11 && gen.Status() == 23) gen_lep.push_back(gen);
      if (fabs(gen.PID()) == 15 && gen.Status() == 23)gen_lep.push_back(gen);
    }
  }

  return gen_lep;
}

bool HNL_LeptonCore::SelectChannel(HNL_LeptonCore::Channel channel) {
  TString process = GetProcess();
  //  if(_jentry < 10000) cout << "process = " << process << endl;
  // Define matching process strings for each channel
  switch (channel) {
  case LL:
    if (process.Contains("SS")) return true;
    break;
  case MuMu:
    if (process.Contains("SS_Mu+Mu+") || process.Contains("SS_Mu-Mu-")) return true;
    break;
  case EE:
    if (process.Contains("SS_El+El+") || process.Contains("SS_El-El-")) return true;
    break;
  case EMu:
    if (process.Contains("SS_El+Mu+") || process.Contains("SS_Mu+El+") ||
  process.Contains("SS_El-Mu-") || process.Contains("SS_Mu-El-")) return true;
    break;
  default:
    return false;
  }

  return false;
}




TString HNL_LeptonCore::GetProcess(){

  if (IsData) return "";
  if(!IsSignal()) return "";


  int N_Mother(0);

  //  cout << "index\tPID\tStatus\tMIdx\tMPID\tStart\tPt\tEta\tPhi\tM" << endl;                                                                                                                                          

  bool isDYVBF=false;

  for (auto i : All_Gens){ Gen gen = i; if(gen.PID() == 9900012 || gen.PID() == 9900014) isDYVBF=true; }

  if(isDYVBF){

    /*bool isVBF=false;                                                                                                                                                                                                 
                                                                                                                                                                                                                         
    for (auto i : All_Gens){                                                                                                                                                                                                 
      Gen gen = i;                                                                                                                                                                                                       
      if (gen.PID() == 22 && gen.Status() == 21) isVBF=true;                                                                                                                                                             
    }                                                                                                                                                                                                                    
    */

    for(unsigned int i=2; i<All_Gens.size(); i++){

      Gen gen = All_Gens.at(i);

      if((gen.PID() == 9900012 || gen.PID() == 9900014)  && gen.Status()==22) {
        N_Mother= gen.MotherIndex();
      }
    }

    int lep_1_ch=-999;
    int lep_2_ch=-999;

    TString lep1_s="", lep2_s="";
    TString lep1_ss="", lep2_ss="";

    for(unsigned int i=2; i<All_Gens.size(); i++){
      Gen gen = All_Gens.at(i);     TString lep_ch="";

      if((All_Gens.at(gen.MotherIndex()).PID() == 9900012|| All_Gens.at(gen.MotherIndex()).PID() == 9900014) && !(All_Gens.at(i).PID() == 9900012 || All_Gens.at(i).PID() == 9900014)){
        //if(fabs(gen.PID())  < 16 && fabs(gen.PID())  > 10) lep_2_ch = (gen.PID() < 0) ? 1 : -1;                                                                                                                        

        if(fabs(gen.PID()) == 15) {
          lep2_s="Tau";
          if (gen.PID() <0) lep2_ss=lep2_s+"+";
          else lep2_ss=lep2_s+"-";
          lep_2_ch = (gen.PID() < 0) ? 1 : -1;
        }
        if(fabs(gen.PID()) == 13) {
          lep2_s="Mu";
          if (gen.PID() <0) lep2_ss=lep2_s+"+";
          else lep2_ss=lep2_s+"-";
          lep_2_ch = (gen.PID() < 0) ? 1 : -1;
        }
        //if(fabs(gen.PID()) == 14) {lep2_s="Mu";lep2_s+=lep_ch;}                                                                                                                                                        
        if(fabs(gen.PID()) == 11) {
          lep2_s="El";
          if (gen.PID() <0) lep2_ss=lep2_s+"+";
          else lep2_ss=lep2_s+"-";
          lep_2_ch = (gen.PID() < 0) ? 1 : -1;
        }
      }
      else if(gen.MotherIndex() == N_Mother&& !(All_Gens.at(i).PID() == 9900012 || All_Gens.at(i).PID() == 9900014)){

        //if(fabs(gen.PID())  < 16 && fabs(gen.PID())  > 10)  lep_1_ch = (gen.PID() < 0) ? 1 : -1;                                                                                                                      
        if(fabs(gen.PID()) == 15) {
          lep1_s="Tau";
          if (gen.PID() < 0) lep1_ss=lep1_s+"+";
          else lep1_ss=lep1_s+"-";
          lep_1_ch = (gen.PID() < 0) ? 1 : -1;
        }
        if(fabs(gen.PID()) == 13) {
          lep1_s="Mu";
          if (gen.PID() < 0) lep1_ss=lep1_s+"+";
          else lep1_ss=lep1_s+"-";
          lep_1_ch = (gen.PID() < 0) ? 1 : -1;
        }
        if(fabs(gen.PID()) == 11) {
          lep1_s="El";
          if (gen.PID() < 0) lep1_ss=lep1_s+"+";
          else lep1_ss=lep1_s+"-";
          lep_1_ch = (gen.PID() < 0) ? 1 : -1;
        }
      }
    }
    if(lep_1_ch == lep_2_ch) return ("SS_"+ lep1_ss+lep2_ss);
    else return ("OS_"+ lep1_s + lep2_s);

  }

  else{


    TString pr="SS_";
    for(unsigned int i=2; i<All_Gens.size(); i++){
      Gen gen = All_Gens.at(i);
      if (fabs(gen.PID()) == 13 && gen.Status() == 23) {
        pr=pr+"Mu";
        if (gen.PID() < 0) pr = pr+"+";
        else pr = pr+"-";
      }
      if (fabs(gen.PID()) == 11 && gen.Status() == 23) {
        pr=pr+"El";
        if (gen.PID() < 0) pr = pr+"+";
        else pr = pr+"-";
      }
      if (fabs(gen.PID()) == 15 && gen.Status() == 23){
        pr=pr+"Tau";
        if (gen.PID() < 0) pr = pr+"+";
        else pr = pr+"-";
      }
    }
    return pr;
  }
  return "TMP";

}

bool HNL_LeptonCore::HasLowMassMeson(std::vector<Lepton *> leps){

  for(unsigned int i = 0; i < leps.size(); i++){
    for(unsigned int j = i+1;  j <leps.size(); j++){
      if(leps[i]->LeptonFlavour() != leps[j]->LeptonFlavour()) continue;
      if(leps[i]->Charge() == leps[j]->Charge() ) continue;
      Particle ll = (*leps[i]) + (*leps[j]);
      if(fabs(ll.M() - 3.10) < 0.08) return true;
      if(fabs(ll.M() - 3.69) < 0.08) return true;
      if(fabs(ll.M() - 0.78) < 0.08) return true;
      if(fabs(ll.M() - 1.02) < 0.08) return true;
      if(fabs(ll.M() - 3.10) < 0.08) return true;
      if(fabs(ll.M() - 9.46) < 0.08) return true;
      if(fabs(ll.M() - 10.02) < 0.08) return true;
      if(fabs(ll.M() - 10.36) < 0.08) return true;
    }
  }


  return false;

}

double HNL_LeptonCore::GetFilterEffType1(TString SigProcess, int mass) {
  // Filter efficiencies for DY process based on mass
  if (SigProcess != "DY") return -1.;

  // Use a map for efficient lookup
  std::map<int, double> effMap = {
    {85, 0.417}, {90, 0.625}, {95, 0.739}, {100, 0.856},
    {125, 0.978}, {150, 0.983}, {200, 0.994}, {250, 0.996},
    {300, 0.996}, {400, 0.998}, {500, 0.998}
  };

  // Return the efficiency if the mass exists in the map, otherwise return -1
  auto it = effMap.find(mass);
  return (it != effMap.end()) ? it->second : -1.;
}

double HNL_LeptonCore::GetXsec(TString SigProcess, int mass){

  return 0.;
}


double  HNL_LeptonCore::GetRecoObjMass(TString METHOD , std::vector<Jet> jets, std::vector<FatJet> fatjets,vector<Lepton*> leps){

  if (METHOD=="HNL_SR3"){

    if(jets.size() < 2) return 0.;
    double dijetmass_tmp=9999.;
    double dijetmass=99990000.;
    int m=-999;
    int n=-999;
    double ST(0.);
    for(UInt_t emme=0; emme<jets.size(); emme++){
      ST += jets[emme].Pt();
      for(UInt_t enne=1; enne<jets.size(); enne++) {
        dijetmass_tmp = (jets[emme]+jets[enne]).M();
        if(emme == enne) continue;
        if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
          dijetmass = dijetmass_tmp;
          m = emme;
          n = enne;
        }
      }
    }
    Particle Wcand = jets[m] + jets[n];
    return Wcand.M();
  }

  if (METHOD=="HNL_SR3_NLL"){

    if(jets.size() < 2) return 0.;
    if(leps.size() != 2) return 0.;

    double dijetmass_tmp=9999.;
    double dijetmass=99990000.;
    int m=-999;
    int n=-999;
    double ST(0.);
    for(UInt_t emme=0; emme<jets.size(); emme++){
      ST += jets[emme].Pt();
      for(UInt_t enne=1; enne<jets.size(); enne++) {

  dijetmass_tmp = (jets[emme]+jets[enne]).M();
        if(emme == enne) continue;

        if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
          dijetmass = dijetmass_tmp;
          m = emme;
          n = enne;
  }
      }
    }
    Particle Wcand = jets[m] + jets[n]+*leps[0] + *leps[1];
    return Wcand.M();
  }
  if (METHOD=="HNL_SR3_N1L"){

    if(jets.size() < 2) return 0.;
    if(leps.size() != 2) return 0.;

    double dijetmass_tmp=9999.;
    double dijetmass=99990000.;
    int m=-999;
    int n=-999;
    double ST(0.);
    for(UInt_t emme=0; emme<jets.size(); emme++){
      ST += jets[emme].Pt();
      for(UInt_t enne=1; enne<jets.size(); enne++) {

        dijetmass_tmp = (jets[emme]+jets[enne]).M();
        if(emme == enne) continue;

        if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
          dijetmass = dijetmass_tmp;
          m = emme;
          n = enne;
        }
      }
    }
    Particle Ncand = jets[m] + jets[n]+*leps[0] ;
    return Ncand.M();
  }



  if (METHOD=="HNL_SR1"){
    if(fatjets.size() ==0 )  return 0.;
    if(leps.size() != 2) return 0.;
    double dijetmass_tmp=999.;
    double dijetmass=9990000;
    int m=-999;
    for(UInt_t emme=0; emme<fatjets.size(); emme++){
      dijetmass_tmp= fatjets[emme].SDMass();
      if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
        dijetmass = dijetmass_tmp;
        m = emme;
      }
    }
    Particle W = fatjets[m] + *leps[0] + *leps[1];
    return W.M();
  }

  if (METHOD=="HNL_SR2_17028"){
    if(fatjets.size() ==0 )  return 0.;
    if(leps.size() != 2) return 0.;
    double dijetmass_tmp=999.;
    double dijetmass=9990000;
    int m=-999;
    for(UInt_t emme=0; emme<fatjets.size(); emme++){
      dijetmass_tmp= fatjets[emme].SDMass();
      if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
        dijetmass = dijetmass_tmp;
        m = emme;
      }
    }

    return fatjets[m].SDMass();
  }


  cout << "[HNL_LeptonCore::GetMass ] ID not found.." << endl;
  exit(EXIT_FAILURE);

  return -9999.;
}



vector<Muon> HNL_LeptonCore::GetLepCollByRunType(const std::vector<Muon>& MuColl, AnalyzerParameter& param,TString Option){

  if(MCSample.Contains("Sherpa")) return MuColl;

  /// Empty Option means  param is used to configure
  if(Option == ""){
    if(param.FakeMethod == "MC") Option+="HFake";
    if(param.ConvMethod == "MC") Option+="NHConv";
  }

  ///cout << "AnalyzerCore::LepCollByRunType  Muon Option = " << Option << endl;                                                                                                                                                                                                                   

  bool GetHadFake=false,  GetNHIntConv=false, GetNHExtConv=false;

  if(Option.Contains("HFake"))           GetHadFake   =true;
  if(Option.Contains("NHConv"))         {GetNHIntConv =true; GetNHExtConv=true;}
  else{ 
    if(Option.Contains("NHIntConv")) GetNHIntConv =true;
    if(Option.Contains("NHExtConv")) GetNHExtConv =true;
  }
  if(     Option=="Fake"     )          {GetHadFake   =true; GetNHExtConv=true;}


  if(IsData)  return MuColl;
  if(IsSignal()) return MuColl;


  //cout << "========================================================" << endl;
  //cout << "Option : " << Option << endl;
  //cout << "GetHadFake : " << GetHadFake << endl;
  //cout << "GetNHIntConv : " << GetNHIntConv << endl;
  //cout << "GetNHExtConv : " << GetNHExtConv << endl;

  vector<Muon> ReturnVec;
  for(unsigned int i=0; i<MuColl.size(); i++){
    if(Option=="NoSel")  ReturnVec.push_back(MuColl.at(i));
    else {
      int LepType= MuColl.at(i).LeptonGenType();
      bool PassSel=false;

      if( LepType > 0  && LepType < 4) PassSel=true;
      if( GetHadFake   && (LepType<0 && LepType>=-4) ) PassSel=true;
      if( GetNHIntConv && LepType>=4 ) PassSel=true;
      if( GetNHExtConv && LepType<-4 ) PassSel=true;
      if( PassSel ) ReturnVec.push_back(MuColl.at(i));
    }
  }

  //cout << "Nmu : " << ReturnVec.size() << endl;
  return ReturnVec;
}


vector<Electron> HNL_LeptonCore::GetSignalLeptons(const std::vector<Electron>& ElColl, vector<Gen>& TruthColl){

  std::vector<Electron>   ElectronColl;

  if(IsSignal()){

    vector<Gen> gen_lep= GetGenLepronsSignal();

    for(auto iel: ElColl){
      bool matched_lep=false;
      for(auto igen : gen_lep){
        if(fabs(igen.PID()) != 11) continue;
        if(iel.DeltaR(igen) < 0.2 &&  ((iel.Pt()/ igen.Pt()) > 0.7 ) && ((iel.Pt()/ igen.Pt()) < 1.3 )) matched_lep=true;
      }
      if(matched_lep) ElectronColl.push_back(iel);
    }
  }
  else  return ElColl;

  return ElectronColl;
}


vector<Muon> HNL_LeptonCore::GetSignalLeptons(const std::vector<Muon>& MuColl, vector<Gen>& TruthColl){

  std::vector<Muon>   MuonColl;

  if(IsSignal()){

    vector<Gen> gen_lep= GetGenLepronsSignal();
    //for(auto igen : gen_lep) cout << "GetGenLepronsSignal " << igen.Pt() << " " << igen.Eta() << endl;                                                                                                                                                                                                   
    for(auto imu: MuColl){
      bool matched_lep=false;
      for(auto igen : gen_lep){
        if(fabs(igen.PID()) != 13) continue;
        if(imu.DeltaR(igen) < 0.2&& ((imu.Pt()/ igen.Pt()) > 0.7 ) && ((imu.Pt()/ igen.Pt()) < 1.3 )) matched_lep=true;
      }
      if(matched_lep) MuonColl.push_back(imu);
    }
  }
  else return MuColl;


  return  MuonColl;
}




vector<Electron> HNL_LeptonCore::GetLepCollByRunType(const vector<Electron>& ElColl, AnalyzerParameter& param, TString Option){

  if(MCSample.Contains("Sherpa")) return ElColl;

  if(Option == ""){
    if(param.FakeMethod == "MC") Option+="HFake";
    if(param.ConvMethod == "MC") Option+="NHConv";
    if(param.CFMethod   == "MC") Option+="CF";
  }

  if(RunPromptTLRemoval) Option == "NHConv";
  if(RunPromptTLRemoval) Option += "CF";

  //cout << "AnalyzerCore::GetLepCollByRunType Electron  Option = " << Option << endl;                                                                                                                                                                                                                     
  bool GetHadFake=false,  GetNHIntConv=false, GetNHExtConv=false, GetCF=false;

  if(Option.Contains("HFake"))           GetHadFake   = true;
  if(Option.Contains("CF"))              GetCF        = true;
  if(Option.Contains("NHConv"))         {GetNHIntConv = true; GetNHExtConv=true;}
  else{
    if(Option.Contains("NHIntConv"))   GetNHIntConv = true;
    if(Option.Contains("NHExtConv"))   GetNHExtConv = true; 
  }
  if(Option=="Fake")                   {GetHadFake  = true; GetNHExtConv=true;}
  
  if(IsData)  return ElColl;
  if(IsSignal()) return ElColl;

  vector<Electron> ReturnVec;
  for(unsigned int i=0; i<ElColl.size(); i++){
    if (Option == "NoSel") ReturnVec.push_back(ElColl.at(i));
    else {
      int LepType= ElColl.at(i).LeptonGenType();

      bool PassSel=false;
      if( LepType > 0   && LepType < 4 )                PassSel=true;

      if( GetCF         && ElColl.at(i).LeptonIsCF() )  PassSel=true;
      //if( !GetCF        && ElColl.at(i).LeptonIsCF() )  PassSel=false;
      if( GetHadFake    && (LepType<0 && LepType>=-4))  PassSel=true;
      if( GetNHIntConv  && (LepType>=4 || ElColl.at(i).LeptonIsPromptConv()) ) PassSel=true;
      if( GetNHExtConv  && (LepType<-4 || ElColl.at(i).LeptonIsPromptConv()) ) PassSel=true;
      if( PassSel ) ReturnVec.push_back(ElColl.at(i));
    }
  }
  return ReturnVec;
}

bool HNL_LeptonCore::SameCharge(vector<Muon> mus, int ch){
  if(mus.size() != 2) return false;
  int sumQ=mus[0].Charge()+mus[1].Charge();
  if(ch==0){
    if(fabs(sumQ) == 2) return true;
    return false;
  }
  else if(ch==1){
    if(sumQ == 2) return true;
    return false;
  }
  else if(ch==-1){
    if(sumQ == -2) return true;
    return false;
  }
  return false;
}

bool HNL_LeptonCore::SameCharge(vector<Electron> els, int ch){
  if(els.size() != 2) return false;
  int sumQ=els[0].Charge()+els[1].Charge();
  if(ch==0){
    if(fabs(sumQ) == 2) return true;
    return false;
  }
  else if(ch==1){
    if(sumQ == 2) return true;
    return false;
  }
  else if(ch==-1){
    if(sumQ == -2) return true;
    return false;
  }
  return false;
}


bool HNL_LeptonCore::SameCharge(vector<Electron> els, vector<Muon> mus,int ch){

  if(els.size() != 1) return false;
  if(mus.size() != 1) return false;

  int sumQ=els[0].Charge()+mus[1].Charge();
  if(ch==0){
    if(fabs(sumQ) == 2) return true;
    return false;
  }
  else if(ch==1){
    if(sumQ == 2) return true;
    return false;
  }
  else if(ch==-1){
    if(sumQ == -2) return true;
    return false;
  }
  return false;
}


bool HNL_LeptonCore::SameCharge(std::vector<Lepton *> leps, int ch){

  if(leps.size() != 2) return false;

  int sumQ=leps[0]->Charge()+leps[1]->Charge();
  if(ch==0){
    if(fabs(sumQ) == 2) return true;
    return false;
  }
  else if(ch==1){
    if(sumQ == 2) return true;
    return false;

  }
  else if(ch==-1){
    if(sumQ == -2) return true;
    return false;
  }
  return false;
}
