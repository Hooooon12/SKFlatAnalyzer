import os 


Eras = ["2016a","2016b","2017","2018"]



for Era in Eras:
    EraFull=Era
    if Era== "2016a":
        EraFull="2016preVFP"
    if Era== "2016b":
        EraFull="2016postVFP"

    ScriptDir="/data6/Users/jalmond/HNL/HNDiLeptonWorskspace/src/BkgPlots/HNL_Lepton_Fakes/AnalyzerInputFile/"

    AnalyzerFileDir="/data6/Users/jalmond/HNL/SKFlatAnalyzer/data/Run2UltraLegacy_v3/"+EraFull+"/FakeRate/DataFR/ElFR/"

    os.system("mv  "+ScriptDir + "MakeScanMapFile/scan_histmap_Electron_"+Era+".txt " + AnalyzerFileDir + "/scan_histmap_Electron.txt")
    os.system("mv  "+ScriptDir + "EGamma/FR_EGamma_"+Era+"_*  " + AnalyzerFileDir )
