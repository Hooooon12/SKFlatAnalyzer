R__LOAD_LIBRARY(/cvmfs/cms.cern.ch/slc7_amd64_gcc700/external/lhapdf/6.2.1-gnimlf3/lib/libLHAPDF.so)


void test(int year, TString isdata, TString stream){

  //Fake m;
  //SSWW m;
  //Signal m;
  Control m;

  m.SetTreeName("recoTree/SKFlat");

  m.LogEvery = 1000;
  if(isdata == "data"){
    m.IsDATA = true;
    if(stream == "SM") m.DataStream = "SingleMuon";
    else if(stream == "DM") m.DataStream = "DoubleMuon";
    else if(year != 2018){
      if(stream == "SE") m.DataStream = "SingleElectron";
      else if(stream == "DE") m.DataStream = "DoubleEG";
    }
    else if(stream == "SE"||stream == "DE") m.DataStream = "EGamma";
    else exit(EXIT_FAILURE);
  }
  else if(isdata == "mc") m.IsDATA = false;
  m.DataYear = year;
  m.Userflags = {
    //"FR", //Fake
    //"Norm", //Fake
    "RunFake", //SSWW, Signal, Control
    //"FR_ex", //Signal, Control
  };
  if(year==2016 && isdata=="data" && stream=="DM"){
    m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_195.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_104.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_105.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_106.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_107.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_108.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_109.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_110.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_111.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_112.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_113.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_114.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_115.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_116.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_117.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_118.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_119.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_120.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_121.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_122.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_123.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_124.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_125.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_126.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_127.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_128.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_129.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_130.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_131.root");
    //m.AddFile("/gv0//DATA/SKFlat/Run2Legacy_v4/2016/DATA/DoubleMuon/periodB_ver2/191231_024317/0000/SKFlatNtuple_2016_DATA_132.root");
  }
  else if(year==2016 && isdata=="mc" && stream=="DY"){
    m.AddFile("/gv0/DATA/SKFlat/Run2Legacy_v4/2016/MC/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/191229_214240/0000/SKFlatNtuple_2016_MC_1.root");
    //m.AddFile("/gv0/DATA/SKFlat/Run2Legacy_v4/2016/MC/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/191229_214240/0000/SKFlatNtuple_2016_MC_2.root");
    //m.AddFile("/gv0/DATA/SKFlat/Run2Legacy_v4/2016/MC/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/191229_214240/0000/SKFlatNtuple_2016_MC_3.root");
    //m.AddFile("/gv0/DATA/SKFlat/Run2Legacy_v4/2016/MC/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/191229_214240/0000/SKFlatNtuple_2016_MC_4.root");
    //m.AddFile("/gv0/DATA/SKFlat/Run2Legacy_v4/2016/MC/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/191229_214240/0000/SKFlatNtuple_2016_MC_5.root");
    //m.AddFile("/gv0/DATA/SKFlat/Run2Legacy_v4/2016/MC/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/191229_214240/0000/SKFlatNtuple_2016_MC_6.root");
    //m.AddFile("/gv0/DATA/SKFlat/Run2Legacy_v4/2016/MC/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/191229_214240/0000/SKFlatNtuple_2016_MC_7.root");
  }
  else if(year==2016 && isdata=="mc" && stream=="ttZToQQ"){
    m.AddFile("/gv0/DATA/SKFlat/Run2Legacy_v4/2016/MC/TTZToQQ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/200131_162717/0000/SKFlatNtuple_2016_MC_1.root");
  }
  m.SetOutfilePath("hists_"+isdata+"_"+stream+".root");
  m.Init();
  m.initializeAnalyzer();
  m.initializeAnalyzerTools();
  m.SwitchToTempDir();
  m.Loop();

  m.WriteHist();

}
