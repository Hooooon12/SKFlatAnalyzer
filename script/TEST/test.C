R__LOAD_LIBRARY(/cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/lhapdf/6.2.3/lib/libLHAPDF.so)
R__LOAD_LIBRARY(/cvmfs/cms.cern.ch/slc7_amd64_gcc900/cms/cmssw/CMSSW_11_2_5/external/slc7_amd64_gcc900/lib/libTMVA.so)


void test(TString era, bool isdata, TString sample, TString skim, TString flag){ //echo -e ".L test.C\n test(\"2017\",false,\"DYTypeI_M500\",\"\",\"\");" | root -l 2>&1 | tee my.log

  //HNL_LeptonFakeRate m;
  //HNL_ControlRegionPlotter m;
  //HNL_SignalRegionPlotter m;
  HNL_SignalRegion_Plotter m;
  //HNL_ControlRegion_Plotter m;

  m.SetTreeName("recoTree/SKFlat");

  m.LogEvery = 1000;
  m.IsDATA = isdata;
  if(isdata) m.DataStream = sample;
  else{
    m.MCSample = sample;
    m.xsec = 6077.22;
    m.sumSign = 132089877.0;
    m.sumW = 3.336547209017e+12;
    m.IsFastSim = false;
  }
  m.SetEra(era);
  TObjArray* flags = flag.Tokenize(",");
  TString flagOutname = "";
  for(int i=0; i<flags->GetEntries(); i++){
    m.Userflags.push_back(((TObjString*)flags->At(i))->String()); //JH : https://root-forum.cern.ch/t/split-tstring-by-delimeter-in-root-c/18228
    flagOutname += "_";
    flagOutname += ((TObjString*)flags->At(i))->String();
  }
  if(era=="2017" && isdata && sample=="SingleMuon" && skim=="HNFakeBDT"){ //JH : test fake rate measurement
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_0.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_1.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_2.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_3.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_4.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_5.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_6.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_7.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_8.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_9.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_10.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_11.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_12.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_13.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_14.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_15.root")) exit(EIO);
  }
  else if(era=="2017" && isdata && sample=="DoubleMuon" && skim=="HNFakeBDT"){ //JH : test fake rate measurement
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_0.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_1.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_2.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_3.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_4.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_5.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_6.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_7.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_8.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_9.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_10.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_11.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_12.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_13.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_14.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/DoubleMuon/periodB/2023_09_01_174012/SKFlatNtuple_2017_DATA_15.root")) exit(EIO);
  }
  else if(era=="2017" && isdata && sample=="DoubleMuon" && skim=="HNMultiLepBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleMuon/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_0.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleMuon/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_1.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleMuon/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_2.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleMuon/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_3.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleMuon/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_4.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleMuon/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_5.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleMuon/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_6.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleMuon/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_7.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleMuon/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_8.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleMuon/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_9.root")) exit(EIO);
  }
  else if(era=="2017" && isdata && sample=="DoubleEG" && skim=="HNMultiLepBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_0.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_1.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_2.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_3.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_4.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_5.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_6.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_7.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/DoubleEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_8.root")) exit(EIO);
  }
  else if(era=="2017" && isdata && sample=="DoubleEG" && skim=="DileptonBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_DileptonBDT/DoubleEG/periodB/2023_09_02_114749/SKFlatNtuple_2017_DATA_0.root")) exit(EIO);
  }
  else if(era=="2017" && isdata && sample=="MuonEG" && skim=="HNMultiLepBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_0.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_1.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_2.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_3.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_4.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_5.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_6.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_7.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_8.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_9.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_10.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_11.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_12.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNMultiLepBDT/MuonEG/periodB/2023_09_04_143241/SKFlatNtuple_2017_DATA_13.root")) exit(EIO);
  }
  else if(era=="2017" && isdata && sample=="SingleElectron"){ //JH : test fake rate measurement
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/DATA_SkimTree_HNFakeBDT/SingleElectron/periodC/2023_05_18_102358/SKFlatNtuple_2017_DATA_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="DYJets" && skim=="HNFakeBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNFakeBDT/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/2023_05_18_102358/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="DYJets" && skim=="HNMultiLepBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/2023_09_04_165251/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/2023_09_04_165251/SKFlatNtuple_2017_MC_1.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/2023_09_04_165251/SKFlatNtuple_2017_MC_2.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/2023_09_04_165251/SKFlatNtuple_2017_MC_3.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/2023_09_04_165251/SKFlatNtuple_2017_MC_4.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="TTLJ" && skim=="HNFakeBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNFakeBDT/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/2023_09_01_174012/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNFakeBDT/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/2023_05_18_102358/SKFlatNtuple_2017_MC_1.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNFakeBDT/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/2023_05_18_102358/SKFlatNtuple_2017_MC_2.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNFakeBDT/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/2023_05_18_102358/SKFlatNtuple_2017_MC_3.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNFakeBDT/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/2023_05_18_102358/SKFlatNtuple_2017_MC_4.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNFakeBDT/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/2023_05_18_102358/SKFlatNtuple_2017_MC_5.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNFakeBDT/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/2023_05_18_102358/SKFlatNtuple_2017_MC_6.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNFakeBDT/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/2023_05_18_102358/SKFlatNtuple_2017_MC_7.root")) exit(EIO);
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNFakeBDT/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/2023_05_18_102358/SKFlatNtuple_2017_MC_8.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="TTLJ" && skim=="FakeEventSkimBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_FakeEventSkimBDT/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/2023_05_19_153135/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="TTLJ" && skim=="HNMultiLepBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/2023_09_04_165251/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="QCD_Mu_300" && skim=="FakeEventSkimBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_FakeEventSkimBDT/QCD_Pt-300To470_MuEnrichedPt5_TuneCP5_13TeV-pythia8/2023_09_01_042228/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="WJets" && skim=="HNFakeBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNFakeBDT/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/2023_05_18_102358/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="DYJets"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/2023_05_03_230947/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="TTLL" && skim=="HNMultiLepBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/2023_05_16_104322/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="TTLL"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/2023_05_03_230947/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="TTJJ" && skim=="HNMultiLepBDT"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/2023_05_16_033011/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="WZ"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/WZTo3LNu_TuneCP5_13TeV-amcatnloFXFX-pythia8/2023_03_29_184218/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="ZZ"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/ZZTo4L_TuneCP5_13TeV_powheg_pythia8/2023_09_04_165251/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2018" && !isdata && sample=="ZG"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2018/MC_SkimTree_HNMultiLepBDT/ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8/2023_09_04_165251/SKFlatNtuple_2018_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="DYTypeI_M100"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/DYTypeI_NLO_DF_M100/2023_03_28_191053/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="DYTypeI_M500"){
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC/DYTypeIHeavyN-DFmix_Dilepton_MN500_TuneCP5_13TeV-amcatnlo-pythia8/220801_130040/0000/SKFlatNtuple_2017_MC_1.root")) exit(EIO); //JH : this is an old one
    //if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/DYTypeI_NLO_DF_M500/2023_03_28_191053/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/DYTypeI_NLO_DF_M500/2023_09_04_164552/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="DYTypeI_DF_M1000"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/DYTypeI_NLO_DF_M1000/2023_09_04_164552/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="VBFTypeI_DF_M1000"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/VBFTypeI_NLO_DF_M1000/2023_09_04_164552/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="VBFTypeI_M500" && skim==""){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC/VBFTypeI_NLO_DF_M500/220712_175932/0000/SKFlatNtuple_2017_MC_1.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="SSWWTypeI_SF_M1000"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/SSWWTypeI_NLO_SF_M1000/2023_09_04_164552/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  else if(era=="2017" && !isdata && sample=="SSWWTypeI_DF_M1000"){
    if(!m.AddFile("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2017/MC_SkimTree_HNMultiLepBDT/SSWWTypeI_NLO_DF_M1000/2023_09_04_164552/SKFlatNtuple_2017_MC_0.root")) exit(EIO);
  }
  if(flag.Length()>0) m.SetOutfilePath("hists_"+era+"_"+sample+flagOutname+".root");
  else m.SetOutfilePath("hists_"+era+"_"+sample+".root");
  m.Init();
  m.initializeAnalyzer();
  m.initializeAnalyzerTools();
  cout << "initializeAnalyzerTools();" << endl;
  m.SwitchToTempDir();
  cout << "SwitchToTempDir();" << endl;
  m.Loop();

  m.WriteHist();

}
