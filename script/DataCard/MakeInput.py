# Place it in SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter
# It does hadd first, and then create inputs for Combine.
# Which to hadd is controlled by the Merge* flags.
# If you want to do hadd only for some reason, then set regions as blank.
# Run python MakeInput.py --Syst\ python MakeInput.py --CR --Syst. 

import os, sys
import commands as cmd
import argparse
from ROOT import *
import array
gROOT.SetBatch(kTRUE)

parser = argparse.ArgumentParser(description='script for creating input root file.',formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('--Scan', action='store_true', help='scan the bin content')
parser.add_argument('--CnC', action='store_true', help='1bin cut and count setting')
parser.add_argument('--CR', action='store_true', help='Make HNL_ControlRegion_Plotter input (default : HNL_SignalRegion_Plotter)')
parser.add_argument('--Syst', action='store_true', help='Add systematics')
parser.add_argument('--Merge', action='store_true', help='hadd the needed histograms')
args = parser.parse_args()


eras = ["2016preVFP", "2016postVFP", "2017", "2018"]
#eras = ["2016preVFP"]
#eras = ["2016postVFP"]
#eras = ["2017"]
#eras = ["2018"]
#eras = ["Run2"] # Let's merge Run2 after running all eras first
#masses = ["M90","M100","M150","M200","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000"]
#masses = ["M100","M1000","M10000"]
masses = ["M85","M90","M95","M100","M125","M150","M200","M250","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000"]
#masses = ["M85","M90","M95","M100","M125","M150","M200","M250"]
#masses = ["M1000"]
#masses = ["M100"]
#masses = ["M85","M90","M95","M100","M125","M150","M200","M250","M300","M400","M500","M1000"]
#masses = ["M3000","M5000","M7500","M10000","M15000","M20000"]
#channels = ["MuMu","EE","EMu"]
channels = ["MuMu","EE"]
#channels = ["EMu"]
HistChannelMap = {'MuMu':'Muon', 'EE':'Electron', 'EMu':'ElectronMuon'}
## Ugly region maps ##
RegionToCRFlagMap = {}
RegionToChannelMap = {}
RegionToHistSuffixMap = {}

#tags = ["HNL_ULID","HNTightV2"] # HNLParameter Name
tags = ["HNL_ULID"] # HNLParameter Name, used to call the histogram
#tags = ["HNL_ULID","HighPt"] # HNLParameter Name, used to call the histogram
#tags = ["HighPt"] # HNLParameter Name, used to call the histogram
#outputTag = "240501_1704_" # tag the output directory name as you wish
#outputTag = "rateParam_" # tag the output directory name as you wish
#outputTag = "PR48_rateParam_" # tag the output directory name as you wish
#outputTag = "PR51_" # tag the output directory name as you wish
#outputTag = "PR51_rescale_" # tag the output directory name as you wish
#outputTag = "PR52_" # tag the output directory name as you wish
#outputTag = "PR52_New" # tag the output directory name as you wish
#outputTag = "PR52_10TeVrescale_" # tag the output directory name as you wish
#outputTag = "PR55_" # tag the output directory name as you wish
#outputTag = "PR55_NoMinPt_" # tag the output directory name as you wish
#outputTag = "PR52_TestScan_" # tag the output directory name as you wish
outputTag = "PR52_SSWWrescale_" # tag the output directory name as you wish

if args.CnC:
  outputTag += 'CnC_'

# Skim
DataSkim = "_SkimTree_HNMultiLepBDT_"
FakeSkim = "_SkimTree_HNMultiLepBDT_"
#CFSkim = "_SkimTree_HNMultiLepBDT_" #FIXME MC CF
CFSkim = "_SkimTree_DileptonBDT_" #FIXME Data CF
ConvSkim = "_SkimTree_HNMultiLepBDT_"
MCSkim = "_SkimTree_HNMultiLepBDT_"
SignalSkim = "_SkimTree_HNMultiLepBDT_"

# This will do necessary hadd for you.
MergeData   = True if args.Merge else False
MergeFake   = True if args.Merge else False  # RunFake
MergeCF     = True if args.Merge else False  # RunCF
MergeConv   = True if args.Merge else False  # RunConv
MergeMC     = True if args.Merge else False  # RunPrompt
MergeSignal = True if args.Merge else False
#MergeDYVBF = True if args.Merge else False
#MergeSSWW  = True if args.Merge else False

if args.CR:
  Blinded = False # Blinded --> the total background will be used as data_obs
  CRflags = ["SS_CR__","VBF_CR__","LLL_VR__"]
  Analyzer = "HNL_ControlRegion_Plotter"

  #regions = ["sr_inv","sr1_inv","sr2_inv","sr3_inv","cf_cr","ww_cr","zg_cr","zg_cr1","zg_cr3","wz_cr","wz_cr1","wz_cr2","wz_cr3","zz_cr","zz_cr1","zz_cr3"] # for CRs
  regions = ["sr_inv","sr1_inv","sr2_inv","sr3_inv","cf_cr","ww_cr","zg_cr","wz_cr","zz_cr"] if not args.Merge else "" # for CRs
  #regions = "" # Use this when merging only

  RegionToCRFlagMap['sr_inv'] = "SS_CR__"
  RegionToCRFlagMap['sr1_inv'] = "SS_CR__"
  RegionToCRFlagMap['sr2_inv'] = "SS_CR__"
  RegionToCRFlagMap['sr3_inv'] = "SS_CR__"
  RegionToCRFlagMap['cf_cr']  = "SS_CR__"
  RegionToCRFlagMap['ww_cr']  = "VBF_CR__"
  RegionToCRFlagMap['zg_cr']  = "LLL_VR__"
  RegionToCRFlagMap['zg_cr1']  = "LLL_VR__"
  RegionToCRFlagMap['zg_cr3']  = "LLL_VR__"
  RegionToCRFlagMap['wz_cr']  = "LLL_VR__"
  RegionToCRFlagMap['wz_cr1']  = "LLL_VR__"
  RegionToCRFlagMap['wz_cr2']  = "LLL_VR__"
  RegionToCRFlagMap['wz_cr3']  = "LLL_VR__"
  RegionToCRFlagMap['zz_cr']  = "LLL_VR__"
  RegionToCRFlagMap['zz_cr1']  = "LLL_VR__"
  RegionToCRFlagMap['zz_cr3']  = "LLL_VR__"

  RegionToChannelMap['sr_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr1_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr2_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr3_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['cf_cr']  = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['ww_cr']  = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['zg_cr']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['zg_cr1']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['zg_cr3']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr1']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr2']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr3']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['zz_cr']  = {'MuMu':'MuMuMuMu', 'EE':'EEEE', 'EMu':'EMuLL'}
  RegionToChannelMap['zz_cr1']  = {'MuMu':'MuMuMuMu', 'EE':'EEEE', 'EMu':'EMuLL'}
  RegionToChannelMap['zz_cr3']  = {'MuMu':'MuMuMuMu', 'EE':'EEEE', 'EMu':'EMuLL'}

  RegionToHistSuffixMap['sr_inv']  = {'MuMu':'LimitBins/MuonCR',  'EE':'LimitBins/ElectronCR',  'EMu':'LimitBins/ElectronMuonCR'}
  RegionToHistSuffixMap['sr1_inv'] = {'MuMu':'LimitBins/MuonCR1', 'EE':'LimitBins/ElectronCR1', 'EMu':'LimitBins/ElectronMuonCR1'}
  RegionToHistSuffixMap['sr2_inv'] = {'MuMu':'LimitBins/MuonCR2', 'EE':'LimitBins/ElectronCR2', 'EMu':'LimitBins/ElectronMuonCR2'}
  RegionToHistSuffixMap['sr3_inv'] = {'MuMu':'LimitBins/MuonCR3', 'EE':'LimitBins/ElectronCR3', 'EMu':'LimitBins/ElectronMuonCR3'}
  RegionToHistSuffixMap['cf_cr']   = {'MuMu':'LimitShape_CF/Binned', 'EE':'LimitShape_CF/Binned', 'EMu':'LimitShape_CF/Binned'}
  RegionToHistSuffixMap['ww_cr']   = {'MuMu':'LimitShape_WW/Binned', 'EE':'LimitShape_WW/Binned', 'EMu':'LimitShape_WW/Binned'}
  RegionToHistSuffixMap['zg_cr']   = {'MuMu':'LimitShape_ZG/Binned', 'EE':'LimitShape_ZG/Binned', 'EMu':'LimitShape_ZG/Binned'}
  RegionToHistSuffixMap['zg_cr1']  = {'MuMu':'LimitShape_ZG_SR1/Binned', 'EE':'LimitShape_ZG_SR1/Binned', 'EMu':'LimitShape_ZG_SR1/Binned'}
  RegionToHistSuffixMap['zg_cr3']  = {'MuMu':'LimitShape_ZG_SR3/Binned', 'EE':'LimitShape_ZG_SR3/Binned', 'EMu':'LimitShape_ZG_SR3/Binned'}
  RegionToHistSuffixMap['wz_cr']   = {'MuMu':'LimitShape_WZ/Binned', 'EE':'LimitShape_WZ/Binned', 'EMu':'LimitShape_WZ/Binned'}
  RegionToHistSuffixMap['wz_cr1']  = {'MuMu':'LimitShape_WZ_SR1/Binned', 'EE':'LimitShape_WZ_SR1/Binned', 'EMu':'LimitShape_WZ_SR1/Binned'}
  RegionToHistSuffixMap['wz_cr2']  = {'MuMu':'LimitShape_WZ_SR2/Binned', 'EE':'LimitShape_WZ_SR2/Binned', 'EMu':'LimitShape_WZ_SR2/Binned'}
  RegionToHistSuffixMap['wz_cr3']  = {'MuMu':'LimitShape_WZ_SR3/Binned', 'EE':'LimitShape_WZ_SR3/Binned', 'EMu':'LimitShape_WZ_SR3/Binned'}
  RegionToHistSuffixMap['zz_cr']   = {'MuMu':'LimitShape_ZZ/Binned', 'EE':'LimitShape_ZZ/Binned', 'EMu':'LimitShape_ZZ/Binned'}
  RegionToHistSuffixMap['zz_cr1']  = {'MuMu':'LimitShape_ZZ_SR1/Binned', 'EE':'LimitShape_ZZ_SR1/Binned', 'EMu':'LimitShape_ZZ_SR1/Binned'}
  RegionToHistSuffixMap['zz_cr3']  = {'MuMu':'LimitShape_ZZ_SR3/Binned', 'EE':'LimitShape_ZZ_SR3/Binned', 'EMu':'LimitShape_ZZ_SR3/Binned'}

else:
  Blinded = True # Blinded --> the total background will be used as data_obs
  CRflags = [""]
  Analyzer = "HNL_SignalRegion_Plotter"

  regions = ["sr","sr1","sr2","sr3"] if not args.Merge else "" # for SRs
  #regions = ["sr"] # for SRs
  #regions = "" # Use this when merging only

  RegionToCRFlagMap['sr'] = ""
  RegionToCRFlagMap['sr1'] = ""
  RegionToCRFlagMap['sr2'] = ""
  RegionToCRFlagMap['sr3'] = ""

  RegionToChannelMap['sr'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr1'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr2'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr3'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}

  RegionToHistSuffixMap['sr'] = {'MuMu':'LimitBins/MuonSR', 'EE':'LimitBins/ElectronSR', 'EMu':'LimitBins/ElectronMuonSR'}
  RegionToHistSuffixMap['sr1'] = {'MuMu':'LimitBins/MuonSR1', 'EE':'LimitBins/ElectronSR1', 'EMu':'LimitBins/ElectronMuonSR1'}
  RegionToHistSuffixMap['sr2'] = {'MuMu':'LimitBins/MuonSR2', 'EE':'LimitBins/ElectronSR2', 'EMu':'LimitBins/ElectronMuonSR2'}
  RegionToHistSuffixMap['sr3'] = {'MuMu':'LimitBins/MuonSR3', 'EE':'LimitBins/ElectronSR3', 'EMu':'LimitBins/ElectronMuonSR3'}

## ChargeSplit has been deprecated due to insignificant improvement. Just legacy ##
ChargeSplit = False
if ChargeSplit:
  ChargeSplit = "ChargeSplit"
else:
  ChargeSplit = ""

InputPath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/"+Analyzer+"_PR52/"
#InputPath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/"+Analyzer

##### Start merging #####
MergeList = {}
MergeList['Conv_inc']      = ["TG","TTG","WZG","WWG","WGToLNuG","WGToLNuG_MG","WGJJToLNu","ZGToLLG","DYJets_MG"] #FIXME time to time
MergeList['Conv_others']   = ["TG","TTG","WZG","WWG","WGToLNuG","WGToLNuG_MG","WGJJToLNu"] #FIXME time to time
MergeList['ZG_norm']       = ["DYJets_MG","ZGToLLG"] #FIXME add this after applying ConversionSpltting
MergeList['ZZ_norm']       = ["ZZTo4L_powheg","GluGluToZZto4e","GluGluToZZto4mu","GluGluToZZto2e2mu"] #FIXME time to time
MergeList['WZ_norm']       = ["WZTo3LNu_mllmin4p0_powheg","WZ_EWK"] #FIXME time to time
MergeList['WW_norm']       = ["WpWp_QCD","WpWp_EWK"] #FIXME time to time
MergeList['Prompt_others'] = [
                              #VVV
                              'WWW','WWZ','WZZ','ZZZ',
                              #SingleTop : 0.1 level events
                              #'SingleTop_sch_Lep','SingleTop_tch_antitop_Incl','SingleTop_tch_top_Incl','SingleTop_tW_antitop_NoFullyHad','SingleTop_tW_top_NoFullyHad',
                              #ttV
                              'ttWToLNu','ttZToLLNuNu', #'ttZToQQ_ll', 'ttWToQQ' : no entry
                              #TTXX
                              'TTTT','TTZZ',
                              #tZq
                              'tZq',
                              #Higgs
                              'ttHToNonbb','tHq','VHToNonbb',
                              #VBFHiggs
                              'VBF_HToZZTo4L', #'VBFHToTauTau_M125', 'VBFHToWWTo2L2Nu', : no entry
                              #ggH
                              'GluGluHToZZTo4L', #'GluGluHToTauTau_M125', 'GluGluHToWWTo2L2Nu', : no entry
                              #minor WWs
                              'WWTo2L2Nu_DS','WWTo2L2Nu_powheg',
                             ] #FIXME time to time
MergeList['Prompt_inc']    = [
                              #VVV
                              'WWW','WWZ','WZZ','ZZZ',
                              #SingleTop : 0.1 level events
                              #'SingleTop_sch_Lep','SingleTop_tch_antitop_Incl','SingleTop_tch_top_Incl','SingleTop_tW_antitop_NoFullyHad','SingleTop_tW_top_NoFullyHad',
                              #ttV
                              'ttWToLNu','ttZToLLNuNu', #'ttZToQQ_ll', 'ttWToQQ' : no entry
                              #TTXX
                              'TTTT','TTZZ',
                              #tZq
                              'tZq',
                              #Higgs
                              'ttHToNonbb','tHq','VHToNonbb',
                              #VBFHiggs
                              'VBF_HToZZTo4L', #'VBFHToTauTau_M125', 'VBFHToWWTo2L2Nu', : no entry
                              #ggH
                              'GluGluHToZZTo4L', #'GluGluHToTauTau_M125', 'GluGluHToWWTo2L2Nu', : no entry
                              #minor WWs
                              'WWTo2L2Nu_DS','WWTo2L2Nu_powheg',
                              #WW
                              'WpWp_QCD','WpWp_EWK',
                              #ZZ
                              'ZZTo4L_powheg','GluGluToZZto4e','GluGluToZZto4mu','GluGluToZZto2e2mu',
                              #WZ
                              'WZTo3LNu_mllmin4p0_powheg','WZ_EWK',
                             ] #FIXME time to time

if MergeData:

  if Blinded:
    print "[MergeData] Data blinded. skipping..."
  else:
    print "[MergeData] Data unblinded. merging..."
    for era in eras:
      if era=="Run2":
        for CRflag in CRflags:
          os.system("mkdir -p "+InputPath+"/Run2/"+CRflag+"/DATA/")
          OutFile=InputPath + "/Run2/" + CRflag + "/DATA/"+Analyzer+DataSkim+"DATA.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + InputPath + "/2016preVFP/" + CRflag + "/DATA/*DATA.root"\
                                      + " " + InputPath + "/2016postVFP/" + CRflag + "/DATA/*DATA.root"\
                                      + " " + InputPath + "/2017/" + CRflag + "/DATA/*DATA.root"\
                                      + " " + InputPath + "/2018/" + CRflag + "/DATA/*DATA.root"\
                   )
      else:
        for CRflag in CRflags:
          OutFile=InputPath + "/" + era + "/" + CRflag + "/DATA/"+Analyzer+DataSkim+"DATA.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + InputPath + "/"+era+"/" + CRflag + "/DATA/*")

if MergeFake:

  for era in eras:
    if era=="Run2":
      for CRflag in CRflags:
        os.system("mkdir -p "+InputPath+"/Run2/"+CRflag+"RunFake__/DATA/")
        OutFile=InputPath + "/Run2/" + CRflag + "RunFake__/DATA/"+Analyzer+FakeSkim+"Fake.root"
        if os.path.exists(OutFile):
          os.system("rm " + OutFile)
        os.system("hadd " + OutFile + " " + InputPath + "/2016preVFP/" + CRflag + "RunFake__/DATA/*Fake.root"\
                                    + " " + InputPath + "/2016postVFP/" + CRflag + "RunFake__/DATA/*Fake.root"\
                                    + " " + InputPath + "/2017/" + CRflag + "RunFake__/DATA/*Fake.root"\
                                    + " " + InputPath + "/2018/" + CRflag + "RunFake__/DATA/*Fake.root"\
                 )
    else:
      for CRflag in CRflags:
        OutFile=InputPath + "/" + era + "/" + CRflag + "RunFake__/DATA/"+Analyzer+FakeSkim+"Fake.root"
        if os.path.exists(OutFile):
          os.system("rm " + OutFile)
        os.system("hadd " + OutFile + " " + InputPath + "/"+era+"/" + CRflag + "RunFake__/DATA/*")

if MergeCF:

  for era in eras:
    if era=="Run2":
      for CRflag in CRflags:
        os.system("mkdir -p "+InputPath+"/Run2/"+CRflag+"RunCF__/DATA/")
        OutFile=InputPath + "/Run2/" + CRflag + "RunCF__/DATA/"+Analyzer+CFSkim+"CF.root"
        if os.path.exists(OutFile):
          os.system("rm " + OutFile)
        os.system("hadd " + OutFile + " " + InputPath + "/2016preVFP/" + CRflag + "RunCF__/DATA/*CF.root"\
                                    + " " + InputPath + "/2016postVFP/" + CRflag + "RunCF__/DATA/*CF.root"\
                                    + " " + InputPath + "/2017/" + CRflag + "RunCF__/DATA/*CF.root"\
                                    + " " + InputPath + "/2018/" + CRflag + "RunCF__/DATA/*CF.root"\
                 )
    else:
      for CRflag in CRflags:
        OutFile=InputPath + "/" + era + "/" + CRflag + "RunCF__/DATA/"+Analyzer+CFSkim+"CF.root" #FIXME Data CF
        #OutFile=InputPath + "/" + era + "/" + CRflag + "RunCF__/"+Analyzer+CFSkim+"CF.root" #FIXME MC CF
        if os.path.exists(OutFile):
          os.system("rm " + OutFile)
        os.system("hadd " + OutFile + " " + InputPath + "/"+era+"/" + CRflag + "RunCF__/DATA/*") #FIXME Data CF
        #os.system("hadd " + OutFile + " " + InputPath + "/"+era+"/" + CRflag + "RunCF__/*") #FIXME MC CF

if MergeConv:

  for era in eras:
    if era=="Run2":
      for CRflag in CRflags:
        os.system("mkdir -p "+InputPath+"/Run2/"+CRflag+"RunConv__/")
        for OutProc in MergeList.keys():
          OutFile=InputPath + "/Run2/" + CRflag + "RunConv__/"+Analyzer+ConvSkim+OutProc+".root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          if os.system("hadd " + OutFile + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2016preVFP/")+ThisProc+".root" for ThisProc in MergeList[OutProc]])\
                                         + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2016postVFP/")+ThisProc+".root" for ThisProc in MergeList[OutProc]])\
                                         + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2017/")+ThisProc+".root" for ThisProc in MergeList[OutProc]])\
                                         + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2018/")+ThisProc+".root" for ThisProc in MergeList[OutProc]])\
                      ) != 0:
            os.system("rm " + OutFile) # remove the output if there is any unmatched process
    else:
      for CRflag in CRflags:
        for OutProc in MergeList.keys():
          OutFile=InputPath + "/" + era + "/" + CRflag + "RunConv__/"+Analyzer+ConvSkim+OutProc+".root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          if os.system("hadd " + OutFile + " " + ' '.join([OutFile.split(OutProc+".root")[0]+ThisProc+".root" for ThisProc in MergeList[OutProc]])) != 0:
            os.system("rm " + OutFile) # remove the output if there is any unmatched process

if MergeMC:

  for era in eras:
    if era=="Run2":
      for CRflag in CRflags:
        os.system("mkdir -p "+InputPath+"/Run2/"+CRflag+"RunPrompt__/")
        for OutProc in MergeList.keys():
          OutFile=InputPath + "/" + era + "/" + CRflag + "RunPrompt__/"+Analyzer+MCSkim+OutProc+".root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          if os.system("hadd " + OutFile + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2016preVFP/")+ThisProc+".root" for ThisProc in MergeList[OutProc]])\
                                         + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2016postVFP/")+ThisProc+".root" for ThisProc in MergeList[OutProc]])\
                                         + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2017/")+ThisProc+".root" for ThisProc in MergeList[OutProc]])\
                                         + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2018/")+ThisProc+".root" for ThisProc in MergeList[OutProc]])\
                      ) != 0:
            os.system("rm " + OutFile) # remove the output if there is any unmatched process
    else:
      for CRflag in CRflags:
        for OutProc in MergeList.keys():
          OutFile=InputPath + "/" + era + "/" + CRflag + "RunPrompt__/"+Analyzer+MCSkim+OutProc+".root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          if os.system("hadd " + OutFile + " " + ' '.join([OutFile.split(OutProc+".root")[0]+ThisProc+".root" for ThisProc in MergeList[OutProc]])) != 0:
            os.system("rm " + OutFile) # remove the output if there is any unmatched process


if MergeSignal:

  if args.CR:
    print "##### This is CR setting."
    print "##### Skipping signal merging ..."
    pass
  else:
    for era in eras:
      for mass in masses:
        if era=="Run2":
          os.system("mkdir -p "+InputPath+"/Run2/")
          OutFileDY    = InputPath + "/Run2/"+Analyzer+"_signalDY_"+mass+".root"
          OutFileVBF   = InputPath + "/Run2/"+Analyzer+"_signalVBF_"+mass+".root"
          OutFileDYVBF = InputPath + "/Run2/"+Analyzer+"_signalDYVBF_"+mass+".root"
          OutFileSSWW  = InputPath + "/Run2/"+Analyzer+"_signalSSWW_"+mass+".root"
          # First, create DY, VBF, SSWWTypeI seperately
          if os.system("hadd -f " + OutFileDY + " " + InputPath+"/2016preVFP/*DYTypeI*"+mass+"_private.root"\
                                              + " " + InputPath+"/2016postVFP/*DYTypeI*"+mass+"_private.root"\
                                              + " " + InputPath+"/2017/*DYTypeI*"+mass+"_private.root"\
                                              + " " + InputPath+"/2018/*DYTypeI*"+mass+"_private.root"\
                     ) != 0:
            os.system("rm " + OutFileDY) # remove the output if there is any unmatched process
          if os.system("hadd -f " + OutFileVBF + " " + InputPath+"/2016preVFP/*VBFTypeI*"+mass+"_private.root"\
                                               + " " + InputPath+"/2016postVFP/*VBFTypeI*"+mass+"_private.root"\
                                               + " " + InputPath+"/2017/*VBFTypeI*"+mass+"_private.root"\
                                               + " " + InputPath+"/2018/*VBFTypeI*"+mass+"_private.root"\
                     ) != 0:
            os.system("rm " + OutFileVBF) # remove the output if there is any unmatched process
          if os.system("hadd -f " + OutFileSSWW + " " + InputPath+"/2016preVFP/*SSWWTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2016postVFP/*SSWWTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2017/*SSWWTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2018/*SSWWTypeI*"+mass+"_private.root"\
                     ) != 0:
            os.system("rm " + OutFileSSWW) # remove the output if there is any unmatched process
          # Now treat DYVBF depending on the mass
          if int(mass.replace("M","")) < 300: # DY only
            os.system("hadd -f " + OutFileDYVBF + " " + InputPath+"/2016preVFP/*DYTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2016postVFP/*DYTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2017/*DYTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2018/*DYTypeI*"+mass+"_private.root")
          else: # DY+VBF
            os.system("hadd -f " + OutFileDYVBF + " " + InputPath+"/2016preVFP/*DYTypeI*"+mass+"_private.root" + " " + InputPath+"/2016preVFP/*VBFTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2016postVFP/*DYTypeI*"+mass+"_private.root" + " " + InputPath+"/2016postVFP/*VBFTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2017/*DYTypeI*"+mass+"_private.root" + " " + InputPath+"/2017/*VBFTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2018/*DYTypeI*"+mass+"_private.root" + " " + InputPath+"/2018/*VBFTypeI*"+mass+"_private.root")
        else:
          OutFileDY    = InputPath + "/" + era + "/"+Analyzer+"_signalDY_"+mass+".root"
          OutFileVBF   = InputPath + "/" + era + "/"+Analyzer+"_signalVBF_"+mass+".root"
          OutFileDYVBF = InputPath + "/" + era + "/"+Analyzer+"_signalDYVBF_"+mass+".root"
          OutFileSSWW  = InputPath + "/" + era + "/"+Analyzer+"_signalSSWW_"+mass+".root"
          # First, create DY, VBF, SSWWTypeI seperately
          os.system("cp " + InputPath+"/"+era+"/*DYTypeI*"+mass+"_private.root " + OutFileDY)
          os.system("cp " + InputPath+"/"+era+"/*VBFTypeI*"+mass+"_private.root " + OutFileVBF)
          os.system("hadd -f " + OutFileSSWW + " " + InputPath+"/"+era+"/*SSWWTypeI*"+mass+"_private.root")
          # Now treat DYVBF depending on the mass
          if int(mass.replace("M","")) < 300: # DY only
            os.system("hadd -f " + OutFileDYVBF + " " + InputPath+"/"+era+"/*DYTypeI*"+mass+"_private.root")
          else: # DY+VBF
            os.system("hadd -f " + OutFileDYVBF + " " + InputPath+"/"+era+"/*DYTypeI*"+mass+"_private.root" + " " + InputPath+"/"+era+"/*VBFTypeI*"+mass+"_private.root")



##### Useful functions #####
def FillScan(outScan, inScan, procName):
  #print "[FillScan] Initiate",procName,"..."
  #print "[FillScan] Currently",outScan.GetNbinsY(),"soures are contained."
  FillBin = 0
  for i in range(outScan.GetNbinsY()+5):
    #print type(outScan.GetYaxis().GetBinLabel(i+1))
    if procName == outScan.GetYaxis().GetBinLabel(i+1):
      print "[FillScan] procName duplicated:",procName
      print "[FillScan] Please check. skipping..."
      return
    if outScan.GetYaxis().GetBinLabel(i+1) == "":
      FillBin = i+1
      #print "[FillScan] FillBin =",FillBin
      outScan.GetYaxis().SetBinLabel(FillBin, procName)
      break

  try:
    inScan.GetNbinsX()
  except AttributeError:
    print "[FillScan] There is no hist named",procName
    print "[FillScan] Filling zeros..."
    for j in range(outScan.GetNbinsX()):
      outScan.SetBinContent(j+1,FillBin,0)
  else:
    for j in range(outScan.GetNbinsX()):
      outScan.SetBinContent(j+1,FillBin,inScan.GetBinContent(j+1))
  
  ### Sanity check ###
  #print "Label of ybin:",outScan.GetYaxis().GetBinLabel(FillBin)
  #print "Contents :",outScan.Integral(0,outScan.GetNbinsX(),FillBin,FillBin)
  #print "Original contents :",inScan.Integral()
  #print "[FillScan] Done."
  #print "[FillScan] Now",outScan.GetNbinsY(),"soures are contained."

  return

##### Main job starts #####
for tag in tags:
  for era in eras:
    for region in regions: # ...and even each region to control!!
      print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",region
      OutputPath = InputPath+'/LimitExtraction/'+outputTag+tag+'/'
      os.system('mkdir -p '+OutputPath + era + '/' + region)
  
      f_path_data          = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "/DATA/"+Analyzer+DataSkim+"DATA.root"
      f_path_fake          = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "RunFake__/DATA/"+Analyzer+FakeSkim+"Fake.root"
      f_path_cf            = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "RunCF__/DATA/"+Analyzer+CFSkim+"CF.root"
      #f_path_cf            = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "RunCF__/"+Analyzer+CFSkim+"CF.root" #FIXME MC CF
      f_path_zg            = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "RunConv__/"+Analyzer+ConvSkim+"ZG_norm.root"
      f_path_conv_others   = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "RunConv__/"+Analyzer+ConvSkim+"Conv_others.root"
      f_path_conv_inc      = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "RunConv__/"+Analyzer+ConvSkim+"Conv_inc.root"
      f_path_wz            = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "RunPrompt__/"+Analyzer+MCSkim+"WZ_norm.root"
      f_path_zz            = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "RunPrompt__/"+Analyzer+MCSkim+"ZZ_norm.root"
      f_path_ww            = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "RunPrompt__/"+Analyzer+MCSkim+"WW_norm.root"
      f_path_prompt_others = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "RunPrompt__/"+Analyzer+MCSkim+"Prompt_others.root"
      f_path_prompt_inc    = InputPath + "/" + era + "/" + RegionToCRFlagMap[region] + "RunPrompt__/"+Analyzer+MCSkim+"Prompt_inc.root"
      
      if not Blinded: f_data = TFile.Open(f_path_data)
      f_fake          = TFile.Open(f_path_fake)
      f_cf            = TFile.Open(f_path_cf)
      f_zg            = TFile.Open(f_path_zg)
      f_conv_others   = TFile.Open(f_path_conv_others)
      f_conv_inc      = TFile.Open(f_path_conv_inc)
      f_wz            = TFile.Open(f_path_wz)
      f_zz            = TFile.Open(f_path_zz)
      f_ww            = TFile.Open(f_path_ww)
      f_prompt_others = TFile.Open(f_path_prompt_others)
      f_prompt_inc    = TFile.Open(f_path_prompt_inc)

      for mass in masses: # iterate for each mass ...
        for channel in channels: # ...and each channel

          if (region == "sr" or "sr3" in region) and int(mass.replace("M","")) <= 500:
            LimitDir = "LimitExtractionBDT"
            InputHistMass = mass+"/"
            if not 'BDT' in RegionToHistSuffixMap[region][channel]:
              RegionToHistSuffixMap[region][channel] += 'BDT'
            else: pass
          else:
            LimitDir = "LimitExtraction"
            InputHistMass = ""
            RegionToHistSuffixMap[region][channel] = RegionToHistSuffixMap[region][channel].replace('BDT','') # FIXME make sure SR and CR share the same naming convention

          # Set channel dependent scaler first
          DYVBFscaler = 0.01 # Set the signalDYVBF scaler
          if int(mass.replace("M","")) > 3000: DYVBFscaler = 0.1 # relax the scale for SSWW impact
          if "EMu" in channel:
            SSWWscaler = 4.*DYVBFscaler*DYVBFscaler # Set the EMu signalSSWW scaler
          else:
            SSWWscaler = DYVBFscaler*DYVBFscaler # Set the signalSSWW scaler

          # Mass dependent scaler, if necessary ...
          #if int(mass.replace("M","")) <= 500: DYVBFscaler = 0.01
          #elif int(mass.replace("M","")) <= 3000: DYVBFscaler = 0.1
          ##else: DYVBFscaler = 1.
          #else: DYVBFscaler = 0.3
          #if "EMu" in channel:
          #  SSWWscaler = 4.*DYVBFscaler*DYVBFscaler # Set the EMu signalSSWW scaler
          #else:
          #  SSWWscaler = DYVBFscaler*DYVBFscaler # Set the signalSSWW scaler

          print "input_hist :", LimitDir+"/"+tag+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel]
          input_hist = LimitDir+"/"+tag+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel]
          
          print "##### Initiating",region,mass,channel,"..."
          if not Blinded: h_data        = f_data.Get(input_hist)
          h_fake          = f_fake.Get(input_hist)
          h_cf            = f_cf.Get(input_hist)
          h_zg            = f_zg.Get(input_hist)
          h_conv_others   = f_conv_others.Get(input_hist)
          h_conv_inc      = f_conv_inc.Get(input_hist)
          h_wz            = f_wz.Get(input_hist)
          h_zz            = f_zz.Get(input_hist)
          h_ww            = f_ww.Get(input_hist)
          h_prompt_others = f_prompt_others.Get(input_hist)
          h_prompt_inc    = f_prompt_inc.Get(input_hist)
          print "##### histo done."
 
          # Make list of [file path, histogram, histo name]
          input_list = [
                        [f_path_fake, h_fake, "fake"],
                        [f_path_cf, h_cf, "cf"],
                        [f_path_zg, h_zg, "zg"],
                        [f_path_conv_others, h_conv_others, "conv_others"],
                        [f_path_conv_inc, h_conv_inc, "conv_inc"],
                        [f_path_wz, h_wz, "wz"],
                        [f_path_zz, h_zz, "zz"],
                        [f_path_ww, h_ww, "ww"],
                        [f_path_prompt_others, h_prompt_others, "prompt_others"],
                        [f_path_prompt_inc, h_prompt_inc, "prompt_inc"],
                       ]

          if args.Scan:
            print "##### Scan initiated. #####"
            h_scan = TH2D("Nominal","Nominal",h_fake.GetNbinsX(),0,h_fake.GetNbinsX(),len(input_list)+2,0,len(input_list)+2) # There is no automatic merging from many TH1s... see https://root-forum.cern.ch/t/filling-a-th2-from-two-existing-th1/14575; +2 is to secure space for 2 signals. I was going to extend the axis, but... (below)
            #h_scan.GetYaxis().SetCanExtend(1) # This seems not resolved... https://root-forum.cern.ch/t/extending-axis-for-th1-vs-th2/20964
            print "h_scan for Nominal created; this should be empty:",h_scan.Integral(0,h_fake.GetNbinsX(),1,1)
            if h_scan.Integral(0,h_fake.GetNbinsX(),1,1)!=0.: sys.exit()
            h_scan.SetDirectory(0)
            scan_list = []

            for i in range(len(input_list)):
              print "##### Making 2D hist for",input_list[i][2],"#####"
              FillScan(h_scan,input_list[i][1],input_list[i][2]) # out, in, name
          
          if Blinded:
            print "##### This analysis is blinded."
            print "##### Creating Asimov data..."
            print "Adding prompt..."
            h_data = h_prompt_inc.Clone()

            bkg_list = [ #bkg except prompt (already added above)
                        [f_path_fake, h_fake, "fake"],
                        [f_path_cf, h_cf, "cf"],
                        [f_path_conv_inc, h_conv_inc, "conv_inc"],
                       ]
          
            total_number = 0 # to cross check
            total_number += h_prompt_inc.GetEntries()
          
            for bkg in bkg_list:
              try:
                bkg[1].GetEntries()
              except AttributeError:
                print("[!!WARNING!!] There is no hist named "+input_hist+" in "+bkg[0]+" .")
                print "Skipping "+bkg[2]+"..."
                continue
              print "Adding "+bkg[2]+"..."
              h_data.Add(bkg[1]) # Add each bkg while iterating
              total_number += bkg[1].GetEntries()
          
            #print h_data.GetEntries(), total_number
            if h_data.GetEntries() == total_number: pass #NOTE Only valid when blinded.
            else:
              print "[!!ERROR!!] Cross check failed. Exiting..."
              sys.exit()
          
            input_list.append(["fake_data_path", h_data, "data_obs"]) # fake data = total bkg. There is no hist path of it.
          else:
            input_list.append([f_path_data, h_data, "data_obs"])
          print "##### Data done."
  
          # Now list has bkg, (pseudo) data. Finally let's add signals
          if args.CR:
            print "##### This is CR setting."
            print "##### Skipping signal ..."
          else:
            f_path_signalDYVBF = InputPath + "/" + era + "/"+Analyzer+"_signalDYVBF_"+mass+".root"
            f_path_signalSSWW  = InputPath + "/" + era + "/"+Analyzer+"_signalSSWW_"+mass+".root"
  
            print "opening",f_path_signalDYVBF,"..."
            f_signalDYVBF = TFile.Open(f_path_signalDYVBF)
            print "opening",f_path_signalSSWW,"..."
            f_signalSSWW  = TFile.Open(f_path_signalSSWW)
  
            #if int(mass.replace("M","")) <= 100: DYVBFscaler = 0.001 # if you want to use HybridNew without additional options, see https://cms-talk.web.cern.ch/t/too-large-error-with-hybridnew/32844
            try:
              h_signalDYVBF = f_signalDYVBF.Get(input_hist)
            except ReferenceError:
              print("[!!WARNING!!] There is no signal file "+f_path_signalDYVBF+" .")
              print "Skipping..."
              if args.Scan:
                print "##### Making 2D hist for","signalDYVBF","#####"
                FillScan(h_scan,h_signalDYVBF,"signalDYVBF") # out, in, name
              #print "Making an empty hist..."
              #h_signalDYVBF = h_data.Clone() #NOTE This might be not working after unblinding, when there is no real data.
              #for i in range(h_signalDYVBF.GetNbinsX()):
              #  h_signalDYVBF.SetBinContent(i+1,0)
              #  h_signalDYVBF.SetBinError(i+1,0)
              #input_list.append([f_path_signalDYVBF, h_signalDYVBF, "signalDYVBF"])
              #if h_signalDYVBF.Integral() != 0:
              #  print "[!!ERROR!!] this must be empty. But having :",h_signalDYVBF.Integral()
              #  print "Exiting..."
              #  sys.exit()
            else:
              if args.Scan:
                print "##### Making 2D hist for","signalDYVBF","#####"
                FillScan(h_scan,h_signalDYVBF,"signalDYVBF") # out, in, name
              try:
                if "PR52" in InputPath and "EMu" in channel: h_signalDYVBF.Scale(2) #XXX FIXME MY SKFlatOutput EMu signal had a half of events
                h_signalDYVBF.Scale(DYVBFscaler) # Scaling the signal due to Combine fitting
              except AttributeError:
                print("[!!WARNING!!] There is no hist named "+input_hist+" in "+f_path_signalDYVBF+" .")
                print "Skipping..."
                #print "Making an empty hist..."
                #h_signalDYVBF = h_data.Clone() #NOTE This might be not working after unblinding, when there is no real data.
                #for i in range(h_signalDYVBF.GetNbinsX()):
                #  h_signalDYVBF.SetBinContent(i+1,0)
                #  h_signalDYVBF.SetBinError(i+1,0)
                #input_list.append([f_path_signalDYVBF, h_signalDYVBF, "signalDYVBF"])
                #if h_signalDYVBF.Integral() != 0:
                #  print "[!!ERROR!!] this must be empty. But having :",h_signalDYVBF.Integral()
                #  print "Exiting..."
                #  sys.exit()
              else:
                input_list.append([f_path_signalDYVBF, h_signalDYVBF, "signalDYVBF"])
                print "Scaled signalDYVBF :", h_signalDYVBF.Integral()
  
            try:
              h_signalSSWW  = f_signalSSWW.Get(input_hist)
            except ReferenceError:
              print("[!!WARNING!!] There is no signal file "+f_path_signalSSWW+" .")
              print "Skipping..."
              if args.Scan:
                print "##### Making 2D hist for","signalSSWW","#####"
                FillScan(h_scan,h_signalDYVBF,"signalSSWW") # out, in, name
              #print "Making an empty hist..."
              #h_signalSSWW = h_data.Clone() #NOTE This might be not working after unblinding, when there is no real data.
              #for i in range(h_signalSSWW.GetNbinsX()):
              #  h_signalSSWW.SetBinContent(i+1,0)
              #  h_signalSSWW.SetBinError(i+1,0)
              #input_list.append([f_path_signalSSWW, h_signalSSWW, "signalSSWW"])
              #if h_signalSSWW.Integral() != 0:
              #  print "[!!ERROR!!] this must be empty. But having :",h_signalSSWW.Integral()
              #  print "Exiting..."
              #  sys.exit()
            else:
              if args.Scan:
                print "##### Making 2D hist for","signalSSWW","#####"
                FillScan(h_scan,h_signalSSWW,"signalSSWW") # out, in, name
              try:
                h_signalSSWW.Scale(SSWWscaler) # To be consistent when calculating mixing limits
              except AttributeError:
                print("[!!WARNING!!] There is no hist named "+input_hist+" in "+f_path_signalSSWW+" .")
                print "Skipping..."
                #print "Making an empty hist..."
                #h_signalSSWW = h_data.Clone() #NOTE This might be not working after unblinding, when there is no real data.
                #for i in range(h_signalSSWW.GetNbinsX()):
                #  h_signalSSWW.SetBinContent(i+1,0)
                #  h_signalSSWW.SetBinError(i+1,0)
                #input_list.append([f_path_signalSSWW, h_signalSSWW, "signalSSWW"])
                #if h_signalSSWW.Integral() != 0:
                #  print "[!!ERROR!!] this must be empty. But having :",h_signalSSWW.Integral()
                #  print "Exiting..."
                #  sys.exit()
              else:
                print "Scaled signalSSWW :", h_signalSSWW.Integral()
                input_list.append([f_path_signalSSWW, h_signalSSWW, "signalSSWW"])
  
            if args.Scan:
              #print h_scan
              #print h_scan.GetTitle()
              #for i in range(h_scan.GetNbinsX()): print h_scan.GetYaxis().GetBinLabel(3), h_scan.GetBinContent(i+1,3)
              #h_scan.SetDirectory(0)
              scan_list.append(h_scan)
              #print scan_list

            print "##### Signal done."
  
  
          if args.Syst:
            if args.CR:
              print "##### This is CR setting."
              print "##### Skipping systematics ..."
            else:
              print "##### Systematics activated."
              syst_list = [
                           "JetResUp","JetResDown",
                           "JetEnUp","JetEnDown",
                           "JetMassUp","JetMassDown",
                           "JetMassSmearUp","JetMassSmearDown",
                           "MuonEnUp","MuonEnDown",
                           "ElectronEnUp","ElectronEnDown",
                           "ElectronResUp","ElectronResDown",
                           #"MuonRecoSFUp","MuonRecoSFDown",
                           #"MuonIDSFUp","MuonIDSFDown",
                           #"MuonISOSFUp","MuonISOSFDown",
                           #"ElectronRecoSFUp","ElectronRecoSFDown",
                           #"ElectronIDSFUp","ElectronIDSFDown",
                           #"ElectronTriggerSFUp","ElectronTriggerSFDown",
                           "BTagSFHTagUp","BTagSFHTagDown",
                           "BTagSFLTagUp","BTagSFLTagDown",
                           "METUnclUp","METUnclDown",
                           "PrefireUp","PrefireDown",
                           "PUUp","PUDown"
                           #"CFUp","CFDown",
                           #"FRUp","FRDown",
                          ]
  
              Nproc = len(input_list) # The number of processes = the length of the input list before adding systematics
  
              for i in range(Nproc):
                if args.Scan:
                  h_scan = TH2D(input_list[i][2],input_list[i][2],h_fake.GetNbinsX(),0,h_fake.GetNbinsX(),len(syst_list),0,len(syst_list))
                  print "h_scan for",input_list[i][2],"syst created; this should be empty:",h_scan.Integral(0,h_fake.GetNbinsX(),1,1)
                  if h_scan.Integral(0,h_fake.GetNbinsX(),1,1)!=0.: sys.exit()
                  h_scan.SetDirectory(0)

                for this_syst in syst_list: # Define new input_hist with each syst name
                  input_hist = LimitDir+"/Syst_"+this_syst+tag+"/"+channel+"/"+InputHistMass+RegionToHistSuffixMap[region][channel]

                  if not "fake_data_path" in input_list[i][0]: # There is no file like "fake_data_path" ...
                    name_syst = input_list[i][2]+"_"+this_syst # Define syst histo name
                    f_syst = TFile.Open(input_list[i][0]) # Get each process's file
                    try:
                      h_syst = f_syst.Get(input_hist)
                    except ReferenceError:
                      print("[!!WARNING!!] There is no file "+input_list[i][0]+" .")
                      if args.Scan:
                        print "##### Making 2D hist for",name_syst,"#####"
                        FillScan(h_scan,h_syst,name_syst) # out, in, name
                      if "signal" in input_list[i][2]:
                        print "Skipping..."
                      else:
                        print "Making an empty hist..."
                        if args.CR:
                          h_syst = h_prompt_inc.Clone() # I could use data here, but data could have no entry due to stats so just use prompt_inc here.
                        else:
                          h_syst = h_data.Clone() # SR --> data = total bkg, lowest possibility of no stats
                        for j in range(h_syst.GetNbinsX()):
                          h_syst.SetBinContent(j+1,0)
                          h_syst.SetBinError(j+1,0)
                        h_syst.SetBinContent(1,0.001) # to avoid Combine complaining for empty hist.
                        h_syst.SetBinError(1,0.000001)
                        h_syst.SetDirectory(0) # Store h_syst in memory so that it cannot be deleted during the iteration
                    else:
                      if args.Scan:
                        print "##### Making 2D hist for",name_syst,"#####"
                        FillScan(h_scan,h_syst,name_syst) # out, in, name
                      try:
                        h_syst.SetDirectory(0) # Store h_syst in memory so that it cannot be deleted during the iteration
                      except AttributeError:
                        print("[!!WARNING!!] There is no hist named "+input_hist+" in "+input_list[i][0]+" .")
                        if "signal" in input_list[i][2]:
                          print "Skipping..."
                        else:
                          print "Making an empty hist..."
                          if args.CR:
                            h_syst = h_prompt_inc.Clone()
                          else:
                            h_syst = h_data.Clone()
                          for j in range(h_syst.GetNbinsX()):
                            h_syst.SetBinContent(j+1,0)
                            h_syst.SetBinError(j+1,0)
                          h_syst.SetBinContent(1,0.001) # to avoid Combine complaining for empty hist.
                          h_syst.SetBinError(1,0.000001)
                          h_syst.SetDirectory(0) # Store h_syst in memory so that it cannot be deleted during the iteration

                    try:
                      if "DYVBF" in input_list[i][2]: # Scale the syst variated signals
                        if "PR52" in InputPath and "EMu" in channel: h_syst.Scale(2) #XXX FIXME MY SKFlatOutput EMu signal had a half of events
                        h_syst.Scale(DYVBFscaler)
                      elif "SSWW" in input_list[i][2]:
                        h_syst.Scale(SSWWscaler)
                    except AttributeError:
                      print("[!!WARNING!!] There is no hist named "+input_hist+" in "+input_list[i][0]+" .")
                      if "signal" in input_list[i][2]:
                        print "Skipping..."
                        continue
                    print "Appending "+name_syst+"..."
                    input_list.append([input_list[i][0], h_syst, name_syst]) # Append each syst histogram while iterating
  
                if args.Scan:
                  #for i in range(h_scan.GetNbinsX()): print h_scan.GetYaxis().GetBinLabel(3), h_scan.GetBinContent(i+1,3)
                  h_scan.SetDirectory(0)
                  scan_list.append(h_scan)
                  #print scan_list

              print "##### Systematics done."
  
  
          print "##### Now creating a limit input root file..."
          outName = OutputPath+era+"/"+region+"/"+mass+"_"+channel
          outfile = TFile.Open(outName+"_card_input.root","RECREATE")
          
          outfile.cd() # Move into it
          for item in input_list: # Remember, item = [path,hist,name]
            try:
              item[1].SetName(item[2])
            except AttributeError:
              print("[!!WARNING!!] There is no hist corresponding to "+item[2]+" in "+item[0]+" .") # Final scan
              if "signal" in item[2]:
                print "Skipping..."
                continue
              else:
                print "Making an empty hist..."
                if args.CnC:
                   h_missing = TH1D(item[2],item[2],1,0,1)
                else:
                  if args.CR:
                    h_missing = h_prompt_inc.Clone()
                  else:
                    h_missing = h_data.Clone()
                for i in range(h_missing.GetNbinsX()):
                  h_missing.SetBinContent(i+1,0)
                  h_missing.SetBinError(i+1,0)
                h_missing.SetBinContent(1,0.001) # to avoid Combine complaining for empty hist.
                h_missing.SetBinError(1,0.000001)
                h_missing.SetName(item[2])
                print "Writing "+item[2]+"..."
                h_missing.Write() # Write empty histogram
                continue

            if item[1].Integral() <=0 : # treat -ve bins
              print("[!!WARNING!!] Negative events "+str(item[1].Integral())+" in "+item[2]+" ------------------------------------")
              print "Creating a makeup hist..."
              for i in range(item[1].GetNbinsX()):
                item[1].SetBinContent(i+1,0)
                item[1].SetBinError(i+1,0)
              item[1].SetBinContent(1,0.001) # to avoid Combine complaining for empty hist.
              item[1].SetBinError(1,0.000001)

            if args.CnC:
              print "!!Cut and count option activated!!"
              print "!!Merging all into 1 bin...!!"
              #print item[1].GetXaxis().GetBinLowEdge(1)
              #print item[1].GetXaxis().GetBinUpEdge(item[1].GetNbinsX())
              #newbin = array.array('d',[0,item[1].GetNbinsX()])
              #print newbin
              
              item[1].Rebin(item[1].GetNbinsX())
              CnChist = TH1D(item[2],item[2],1,0,1)
              CnChist.SetBinContent(1,item[1].GetBinContent(1))
              CnChist.SetBinError(1,item[1].GetBinError(1))
              #CnChist = item[1].Rebin(item[1].GetNbinsX())
              #CnChist = item[1].Rebin(1,"test",newbin)

            print "Writing "+item[2]+"..."
            if args.CnC: CnChist.Write()
            else: item[1].Write() # Write each histogram while iterating
            #item[1].Write() # Write each histogram while iterating
          
          outfile.Close()
          print outName+"_card_input.root has been created."

          if args.Scan:
            colors = array.array('i',[632,417,860])
            levels = array.array('d',[-1.e308,-0.00001,0.00001,1.e308]) # Even if I set the color with zero bins, it won't be drawn if the minimum is zero (not negative). See https://root-forum.cern.ch/t/not-plotting-zero-bins-in-th2-with-negative-entries/19633/4
            #print "len(scan_list):",len(scan_list)
            canvas = TCanvas("canvas","canvas",900,1000)
            #canvas.SetFrameFillColor(417) # https://root-forum.cern.ch/t/how-to-set-a-color-for-zero-entry-bins-in-cont-histograms/5411; This doesn't look pretty.
            #pad = TPad("pad","pad",0.1,0,1,1)
            #pad.Draw()
            #pad.cd()
            for i in range(len(scan_list)):
              this_h_scan = scan_list[i]
              this_proc = this_h_scan.GetTitle()
              #print "this_proc:",this_proc
              #Final check
              #for j in range(this_h_scan.GetNbinsY()): print j,this_h_scan.GetYaxis().GetBinLabel(j+1),this_h_scan.GetBinContent(1,j+1)
              this_h_scan.SetStats(0)
              this_h_scan.SetContour(3,levels)
              #this_h_scan.GetYaxis().SetLabelSize(0.01)
              this_h_scan.Draw("COLZTEXT")
              this_h_scan.GetZaxis().SetRangeUser(this_h_scan.GetMinimum(),1.)
              gStyle.SetPalette(3,colors)
              gStyle.SetPaintTextFormat("4.1f") # https://root-forum.cern.ch/t/decimal-precision-with-text-drawing-option/15923
              #gPad.SetLogz()
              if i==0: canvas.Print(outName+"_card_scan.pdf(", "Title: "+this_proc)
              elif i==(len(scan_list)-1): canvas.Print(outName+"_card_scan.pdf)", "Title: "+this_proc)
              else: canvas.Print(outName+"_card_scan.pdf", "Title: "+this_proc) # https://root-forum.cern.ch/t/problem-with-saving-multiple-canvases-to-a-single-pdf/57145/3
            print outName+"_card_scan.pdf has been created."
