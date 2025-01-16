import os, sys, argparse
import commands as cmd
import argparse
import math
import numpy as np
from datetime import datetime
from ROOT import *
gROOT.SetBatch(kTRUE)

parser = argparse.ArgumentParser(description='Tool for high pt electron SF measurement', formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-n', dest='Nevents', type=int, default=-1, help='Number of events to run; < 0 : full')
parser.add_argument('-t', dest='Time', action='store_true', help='Print running times')
parser.add_argument('-nj', dest='NJob', type=int, default=0, help='Number of job')
parser.add_argument('-e' , dest='Era', default ='2017')
parser.add_argument('-wd' , dest='WorkDir', default ='./')

args = parser.parse_args()

Name_Nevents = "_N"+str(args.Nevents) if args.Nevents > 0 else ""
NJob=args.NJob
Era = args.Era
WorkDir=args.WorkDir

It_Probes    = [ 'MVALoose','MVABaseline','HNLMVA','HNLMVA_HighPt', 'HNL_ULID_Split_1','HNL_ULID_Split_2','HNL_ULID_Split_3','HNL_ULID_Split_4','HNL_ULID_Split_4b','HNL_ULID_Split_5','HNL_ULID_Split_5b','HNL_ULID_Split_6','HNL_ULID_Split_7','HNL_ULID_Split_8','passingHNLMVA_NoCF','passingHNLMVA_NoConv','passingHNLMVA_NoFake']

#### IDs applied to probe befrpre PASS/FAIL
It_ProbeID   = ['Pass',     'Pass', 'Pass', 'Pass','Pass',       'HNL_ULID_Probe_Split_2','HNL_ULID_Probe_Split_3','HNL_ULID_Probe_Split_4','HNL_ULID_Probe_Split_4','HNL_ULID_Probe_Split_5','HNL_ULID_Probe_Split_5','HNL_ULID_Probe_Split_6','HNL_ULID_Probe_Split_7','HNL_ULID_Probe_Split_8' ,'MVABaseline','MVABaseline','MVABaseline']
It_IsPasses = ['Pass','Fail']
It_EtaRegions = ['BB','EC']
#It_EtaRegions = ['BB']
#It_Charges = ["os","ss","ss_zpt","ss_tot","ss_zpt_tot"]
It_Charges = ["os","ss","ss_tot"]

#eras = [
#  "2016preVFP",
#  "2016postVFP",
#  "2017",
#  "2018",
#]

eras = [Era]

#grouped_eras = {
#                #'2016preVFP': ["2016preVFP"], 
#                #'2016postVFP': ["2016postVFP"], 
#                #'2016': ["2016preVFP","2016postVFP"], 
#                '2017': ["2017"], 
#                #'2018': ["2018"]
#}
grouped_eras = {                                                                                                                                                                                     
  Era : [Era]
}

luminosity = {
  '2016' : '36.3',
  '2016preVFP' : '19.5',
  '2016postVFP' : '16.8',
  '2017' : '41.5',
  '2018' : '59.8',
}
samples = {
  '2016' : [
      "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
      "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
     "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
     "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
     "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
     "WW_TuneCP5_13TeV-pythia8",
     "WZ_TuneCP5_13TeV-pythia8",
     "ZZ_TuneCP5_13TeV-pythia8",
     "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "SingleElectron",
  ],
  '2016preVFP' : [
      "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
      "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
     "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
     "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
     "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
     "WW_TuneCP5_13TeV-pythia8",
     "WZ_TuneCP5_13TeV-pythia8",
     "ZZ_TuneCP5_13TeV-pythia8",
     "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "SingleElectron",
  ],
  '2016postVFP' : [
      "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
      "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "WW_TuneCP5_13TeV-pythia8",
    "WZ_TuneCP5_13TeV-pythia8",
    "ZZ_TuneCP5_13TeV-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "SingleElectron",
  ],
  '2017' : [
    "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "WW_TuneCP5_13TeV-pythia8",
    "WZ_TuneCP5_13TeV-pythia8",
    "ZZ_TuneCP5_13TeV-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "SingleElectron",
  ],
  '2018' : [
 "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
      "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "WW_TuneCP5_13TeV-pythia8",
    "WZ_TuneCP5_13TeV-pythia8",
    "ZZ_TuneCP5_13TeV-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "EGamma",
  ],
}
types = {
  'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos'  : 'MC',
  'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : 'MC',
  'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : 'MC',
  'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : 'MC',
  'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'WW_TuneCP5_13TeV-pythia8' : 'MC',
  'WZ_TuneCP5_13TeV-pythia8' : 'MC',
  'ZZ_TuneCP5_13TeV-pythia8' : 'MC',
  'SingleElectron' : 'DATA',
  'EGamma' : 'DATA',
}
dates = {
  '2016preVFP':
    {

      'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },
       'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },

     'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'SingleElectron' : {
                         'periodB_ver2' : '2024_11_26_102156',
                         'periodC'      : '2024_11_26_102156',
                         'periodD'      : '2024_11_26_102156',
                         'periodE'      : '2024_11_26_102156',
                         'periodF'      : '2024_11_26_102156',
                        },
    },
  '2016postVFP':
    {

      'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },
       'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },



     'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'SingleElectron' : {
                         'periodF' : '2024_11_26_102156',
                         'periodG' : '2024_11_26_102156',
                         'periodH' : '2024_11_26_102156',
                        },
    },
  '2017':
    {

      'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },
       'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },


     'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {
                                                             '' : '2024_12_13_185927',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_12_13_185927',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'SingleElectron' : {
                         'periodB' : '2024_12_13_185927',
                         'periodC' : '2024_12_13_185927',
                         'periodD' : '2024_12_13_185927',
                         'periodE' : '2024_12_13_185927',
                         'periodF' : '2024_12_13_185927',
                        },
    },
  '2018':
    {

      'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },
       'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {
                                                             '' : '2024_12_13_185927',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_12_13_185927',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'EGamma' : {
                 'periodA' : '2024_12_13_185927',
                 'periodB' : '2024_12_13_185927',
                 'periodC' : '2024_12_13_185927',
                 'periodD' : '2024_12_13_185927',
                },
    },
}
triggers = {
            '2016'        : 'passHltEle27WPTightGsf',
            '2016preVFP'  : 'passHltEle27WPTightGsf',
            '2016postVFP' : 'passHltEle27WPTightGsf',
            '2017'        : 'passHltEle32DoubleEGWPTightGsf',
            '2018'        : 'passHltEle32WPTightGsf',
}
nameFilter = {

  'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos'  : 'DYJetsToEE',
  'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : 'DYJetsToTauTau',
  'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : 'DYJetsToTauTau',
  'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : 'DYJets',
  'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : 'WJets',
  'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : 'TTLL',
  'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : 'TTLJ',
  'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'tW_top',
  'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'tw_antitop',
  'WW_TuneCP5_13TeV-pythia8' : 'WW',
  'WZ_TuneCP5_13TeV-pythia8' : 'WZ',
  'ZZ_TuneCP5_13TeV-pythia8' : 'ZZ',
  'SingleElectron' : 'data',
  'EGamma' : 'data',
}

def GetMinMax(*hists):

  if not hists:
    raise ValueError("[GetMinMax] At least one histogram must be provided.")

  global_min = float('inf')
  global_max = float('-inf')

  for hist in hists:
    if not isinstance(hist, TH1):
      raise TypeError("[GetMinMax] Expected a TH1 histogram or subclass, but got " + str(type(hist)))

    # Get the minimum and maximum bin content of the histogram
    local_min_bin = hist.GetMinimumBin()
    local_min = hist.GetBinContent(local_min_bin)
    local_max_bin = hist.GetMaximumBin()
    local_max = hist.GetBinContent(local_max_bin)

    # Update global min and max
    global_min = min(global_min, local_min)
    global_max = max(global_max, local_max)

  return global_min, global_max


def add_overflow(hist):

  last_bin = hist.GetNbinsX()
  over_bin = last_bin+1

  last_bin_content = hist.GetBinContent(last_bin)
  over_bin_content = hist.GetBinContent(over_bin)
  last_bin_error = hist.GetBinError(last_bin)
  over_bin_error = hist.GetBinError(over_bin)

  hist.SetBinContent(last_bin, last_bin_content + over_bin_content)
  hist.SetBinError(last_bin, math.sqrt(last_bin_error ** 2 + over_bin_error ** 2))

  return hist

def TurnOn():
  for year, eras in grouped_eras.items():

    OutFile = TFile.Open(WorkDir+"/Out_TurnOn/TurnOn_"+year+".root","RECREATE")

    for sample in samples[year]:

      t1 = datetime.now()
      print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling",year,sample,"..."
      path_to_add = []
      this_chain = TChain("tnpEleIDs/fitter_tree")

      for era in eras:
        for period in dates[era][sample]:
          path_to_add.append("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root")
        
      for path in path_to_add:
        print "Adding",path,"..."
        this_chain.Add(path)
      t2 = datetime.now()
      print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
    
      #t1 = datetime.now()
      #print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling total entries ..."
      #print "Total",this_chain.GetEntries(),"events." # The most time consuming part
      #t2 = datetime.now()
      #print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
    
      outName = year+"_"+nameFilter[sample]

      charges = ["Inc","OS","SS","Subt"]
      chargeComms = {
                     "Inc"  : "(1==1)",
                     "OS"   : "(el_q+tag_Ele_q==0)",
                     "SS"   : "(el_q+tag_Ele_q!=0)",
                     "Subt" : "(el_q+tag_Ele_q==0)",
      }
      tags = ["","tag_"]
      tagPts = {
                "" : "el_pt_cor",
                "tag_" : "tag_Ele_pt_cor",
      }
      trigs = ["den_",outName+"_"]
      trigComms = {
                    "den_" : "",
                    outName+"_" : "&&"+triggers[year],
      }
      IDs = ["","HEEP_"]
      IDComms = {
                    "" : "",
                    "HEEP_" : "&&(passingHEEP)",
      }
      Barrels = ["","barrel_"]
      BarrelComms = {
                     "" : "",
                     "barrel_" : "&&(fabs(el_sc_eta)<=1.4442)",
      }

      denlist = []
      numlist = []

      t1 = datetime.now()
      print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Main jobs begin ..."
      for charge, tag, trig, ID, Barrel in [(charge, tag, trig, ID, Barrel) for charge in charges for tag in tags for trig in trigs for ID in IDs for Barrel in Barrels]:
        print "Running: this_chain.Draw(\""+tagPts[tag]+">>h_"+tag+trig+ID+Barrel+charge+"(200,0,200)\",\""+chargeComms[charge]+trigComms[trig]+IDComms[ID]+BarrelComms[Barrel]+"\")"
        this_chain.Draw(tagPts[tag]+">>h_"+tag+trig+ID+Barrel+charge+"(200,0,200)",chargeComms[charge]+trigComms[trig]+IDComms[ID]+BarrelComms[Barrel])
        denlist.append(gDirectory.Get("h_"+tag+trig+ID+Barrel+charge)) if "den" in trig else numlist.append(gDirectory.Get("h_"+tag+trig+ID+Barrel+charge))
        if not "data" in outName:
          this_chain.Draw(tagPts[tag]+">>h_"+tag+trig+ID+Barrel+charge+"_gen(200,0,200)",chargeComms[charge]+trigComms[trig]+IDComms[ID]+BarrelComms[Barrel])
          denlist.append(gDirectory.Get("h_"+tag+trig+ID+Barrel+charge+"_gen")) if "den" in trig else numlist.append(gDirectory.Get("h_"+tag+trig+ID+Barrel+charge+"_gen"))
      t2 = datetime.now()
      print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
      #for i in range(len(denlist)):
      #  print denlist[i].GetName()
      #for i in range(len(denlist)):
      #  print numlist[i].GetName()
      #return

      print "last bin:",denlist[0].GetBinContent(200), "overflow bin:",denlist[0].GetBinContent(201)
      for i in range(len(denlist)):
        denlist[i] = add_overflow(denlist[i])
        numlist[i] = add_overflow(numlist[i])
      print "now last bin:",denlist[0].GetBinContent(200)
    
      for i in range(len(denlist)):
        if "Subt" in denlist[i].GetName():
          if "gen" in denlist[i].GetName(): # MC
            denlist[i].Add(denlist[i-16],-1)
            numlist[i].Add(numlist[i-16],-1)
          else: # data
            denlist[i].Add(denlist[i-8],-1)
            numlist[i].Add(numlist[i-8],-1)
    
      for i in range(len(denlist)):
        numlist[i].Divide(numlist[i],denlist[i],1,1,"B")
    
      OutFile.cd()
      for i in range(len(denlist)):
        numlist[i].Write()
    
    OutFile.Close()

  return

def classify_hist(this_year, this_chain):

  if not this_chain.mcTrue:
    return None

  # EtaRegion
  if abs(this_chain.el_sc_eta) < 1.4442:
    EtaRegion = 'BB'
  elif 1.566 < abs(this_chain.el_sc_eta) < 2.5:
    EtaRegion = 'EC'
  else:
    return None

  if EtaRegion not in It_EtaRegions:
    return None

  # Charge
  Charges = []
  if (this_chain.el_q+this_chain.tag_Ele_q)==0:
    Charges.append('os')
  elif (this_chain.el_q+this_chain.tag_Ele_q)!=0:
    #Charges.append('ss_OSTurnOn') # add this if necessary
    if getattr(this_chain, "tag_"+triggers[this_year]):
      Charges.append('ss')
      #Charges.append('ss_zpt')

  # Probe type
  Probes = {}
  nProbe=0
  for this_probe in It_Probes:
    ProbeID=It_ProbeID[nProbe]
    nProbe=nProbe+1

    ### Apply probe ID before Pass/Fail check
    if ProbeID != "Pass":
      if not getattr(this_chain, "passing"+ProbeID):
        continue

    Probes[this_probe] = 'Pass' if getattr(this_chain, "passing"+this_probe) else 'Fail'
    #for attr in dir(this_chain):
    #  if attr.startswith('passing'):
    #    ID = attr.replace('passing','')
    #    Probes[ID] = 'Pass' if getattr(this_chain, attr) else 'Fail' # use this to include every IDs

  return EtaRegion, Charges, Probes

def makePlots(Data_OS, Stack, Bundle, Error, Era, Name,n_job):

  TS=Name+"_"+Era + "_Name_"+ str(n_job)
  c1 = TCanvas("c1_"+TS,"",1000,1000)
  c1.cd()

  c_up = TPad("c_up", "", 0, 0.25, 1, 1)
  c_up.SetTopMargin(0.08)
  c_up.SetBottomMargin(0.017)
  c_up.SetLeftMargin(0.14)
  c_up.SetRightMargin(0.04)
  c_up.SetLogx()
  c_up.SetLogy()
  c_up.Draw()
  c_up.cd()

  Stack.Draw("hist")
  Stack.SetTitle("")
  Stack.GetXaxis().SetLabelSize(0)
  Stack.GetYaxis().SetLabelSize(0.045)
  Stack.GetYaxis().SetTitle("Events")
  Stack.GetYaxis().SetTitleSize(0.075)
  Stack.GetYaxis().SetTitleOffset(0.8)

  Error.SetMarkerSize(0)
  Error.SetLineWidth(0)
  Error.SetFillStyle(3144)
  Error.SetFillColor(kBlack)
  Error.Draw("e2 same")

  Data_OS.SetMarkerStyle(20)
  Data_OS.SetMarkerColor(kBlack)
  Data_OS.Draw("ep same")

  lg = TLegend(0.6, 0.45, 0.8, 0.85)
  lg.AddEntry(Error, "Stat. Uncertainty", "f")
  lg.AddEntry(Data_OS, "Data_OS", "lep")
  lg.AddEntry(Bundle[0], "DY", "f")
  lg.AddEntry(Bundle[1], "W", "f")
  lg.AddEntry(Bundle[2], "t#bar{t}", "f")
  lg.AddEntry(Bundle[3], "Diboson", "f")
  lg.AddEntry(Bundle[4], "SingleTop", "f")
  lg.AddEntry(Bundle[5], "Fake", "f")
  lg.SetBorderSize(0)
  lg.SetTextSize(0.03)
  lg.SetFillStyle(1001)
  lg.SetShadowColor(0)
  lg.Draw("same")
 
  txt_lumi = TLatex()
  txt_lumi.SetNDC()
  txt_lumi.SetTextSize(0.05)
  txt_lumi.SetTextAlign(32)
  txt_lumi.SetTextFont(42)
  txt_lumi.DrawLatex(.95,.96, luminosity[Era]+" fb^{-1} (13 TeV)")

  c1.cd()

  c_down = TPad("c_down", "", 0, 0, 1, 0.25)
  c_down.SetTopMargin(0.03)
  c_down.SetBottomMargin(0.35)
  c_down.SetLeftMargin(0.14)
  c_down.SetRightMargin(0.04)
  c_down.SetGridx()
  c_down.SetGridy()
  c_down.SetLogx()
  c_down.Draw()
  c_down.cd()

  this_nBins = Data_OS.GetNbinsX()

  Error_Stat = Error.Clone()
  for iBin in range(this_nBins):
    binContent = Error_Stat.GetBinContent(iBin+1)
    binError = Error_Stat.GetBinError(iBin+1)
    if binContent != 0.:
      binError = binError/binContent
    else:
      binError = 0.
    Error_Stat.SetBinContent(iBin+1, 1.)
    Error_Stat.SetBinError(iBin+1, binError)

  #print "Error of ratio:"
  #for iBin in range(this_nBins):
  #  print Error_Stat.GetBinError(iBin+1)

  Error_Stat.SetTitle("")
  Error_Stat.SetStats(0)
  Error_Stat.GetXaxis().SetTitle("p_{T} [GeV]")
  Error_Stat.GetYaxis().SetTitle("#frac{Obs.}{Pred.}")
  #Error_Stat.GetXaxis().SetRange(minBinNumber, maxBinNumber)
  Error_Stat.GetYaxis().SetRangeUser(0.5, 1.5)
  Error_Stat.GetXaxis().SetLabelSize(0.12)
  Error_Stat.GetYaxis().SetLabelSize(0.08)
  Error_Stat.GetXaxis().SetTitleSize(0.16)
  Error_Stat.GetYaxis().SetTitleSize(0.14)
  Error_Stat.GetXaxis().SetTitleOffset(0.9)
  Error_Stat.GetYaxis().SetTitleOffset(0.4)

  Error_Stat.SetMarkerSize(0)
  Error_Stat.SetLineWidth(0)
  Error_Stat.SetFillStyle(1001)
  Error_Stat.SetFillColor(kGray)
  Error_Stat.Draw("e2")

  Ratio = Data_OS.Clone()
  Ratio.Divide(Error)
  Ratio.SetLineColor(1)
  Ratio.SetMarkerColor(1)
  Ratio.SetMarkerStyle(20)
  Ratio.Draw("ep same")

  lg2 = TLegend(0.75, 0.88, 0.9, 0.95)
  lg2.SetNColumns(2)
  lg2.AddEntry(Error_Stat, "Stat. Uncert.", "f")
  lg2.SetBorderSize(1)
  lg2.SetTextSize(0.06)
  lg2.SetFillStyle(1001)
  lg2.SetShadowColor(0)
  lg2.Draw("same")

  minRange = Data_OS.GetBinLowEdge(1)
  maxRange = Data_OS.GetBinLowEdge(this_nBins) + Data_OS.GetBinWidth(this_nBins)

  line = TLine(minRange, 1., maxRange, 1.)
  line.SetLineWidth(1)
  line.SetLineColor(2)
  line.Draw()

  c1.SaveAs(WorkDir+"/Out_TurnOn/Pt_"+Era+Name+"_"+str(n_job)+".png")
  del c1

  return

def measureSFs(Data_OS, Bundle, Era, EtaRegion, Probe, Tag, Save, n_job, OutFile):

  return 
  OutName = "SF_Pt_"+Era+"_"+EtaRegion+"_"+Probe+Tag

  this_nBins = Data_OS['Pass'].GetNbinsX()
  this_Bins = Data_OS['Pass'].GetXaxis().GetXbins().GetArray()

  #Bundle : DY, WJets, ttbar, Diboson, SingleTop, Fakes

  for i in range(len(Bundle['Pass'])):
    if i==0 : continue # skip DY
    Data_OS['Pass'].Add(Bundle['Pass'][i],-1)
    Data_OS['Fail'].Add(Bundle['Fail'][i],-1)

  Data_Eff = Data_OS['Pass'].Clone()
  Data_Tot = Data_OS['Pass'].Clone()
  Data_Tot.Add(Data_OS['Fail'])

  combined_data_pass = Data_Eff.Integral()
  combined_data_tot = Data_Tot.Integral()
  combined_data_eff = combined_data_pass/combined_data_tot

  #print "Data eff check:"
  #for i in range(this_nBins):
  #  if Data_Eff.GetBinContent(i+1)>0:
  #    print i+1,Data_Eff.GetBinContent(i+1)
  #    print i+1,Data_Tot.GetBinContent(i+1)
  #    print i+1,Data_Eff.GetBinContent(i+1)/Data_Tot.GetBinContent(i+1)

  # handling exceptions
  print "Checking Data eff bins ..."
  for i in range(Data_Eff.GetNbinsX()):
    print i+1, "th bin num:", Data_Eff.GetBinContent(i+1), "den:", Data_Tot.GetBinContent(i+1)
    if Data_Eff.GetBinContent(i+1) <= 0:
      Data_Eff.SetBinContent(i+1, 1)
      Data_Tot.SetBinContent(i+1, 0.01) # eff = 100 so out of range
  Data_Eff.Divide(Data_Eff,Data_Tot,1,1,"B")

  #for i in range(this_nBins):
  #  print i+1,Data_Eff.GetBinContent(i+1)

  MC_Eff = Bundle['Pass'][0].Clone()
  MC_Tot = Bundle['Pass'][0].Clone()
  MC_Tot.Add(Bundle['Fail'][0])

  combined_mc_pass = MC_Eff.Integral()
  combined_mc_tot = MC_Tot.Integral()
  combined_mc_eff = combined_mc_pass/combined_mc_tot

  combined_sf = combined_data_eff/combined_mc_eff

  print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  print "Running", OutName, "..."
  print "Combined data efficiency:", combined_data_eff
  print "Combined mc efficiency:", combined_mc_eff
  print "Combined scale factor:", combined_sf # FIXME add error later
  print "Saving into", OutFile.GetName(), "..." # FIXME add error later
  print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

  # Fill Gap
  Save['Data_Eff'].SetBinContent(2, 0)
  Save['MC_Eff']  .SetBinContent(2, 0)
  Save['SF']      .SetBinContent(2, 0)
  if EtaRegion == "BB":
    Save['Data_Eff'].SetBinContent(1, combined_data_eff)
    Save['MC_Eff']  .SetBinContent(1, combined_mc_eff)
    Save['SF']      .SetBinContent(1, combined_sf)
  elif EtaRegion == "EC":
    Save['Data_Eff'].SetBinContent(3, combined_data_eff)
    Save['MC_Eff']  .SetBinContent(3, combined_mc_eff)
    Save['SF']      .SetBinContent(3, combined_sf)
  else:
    raise ValueError("Unknown EtaRegion: "+EtaRegion)

  #print "MC eff check:"
  #for i in range(this_nBins):
  #  if MC_Eff.GetBinContent(i+1)>0:
  #    print i+1,MC_Eff.GetBinContent(i+1),"+-",MC_Eff.GetBinError(i+1)
  #    print i+1,MC_Tot.GetBinContent(i+1),"+-",MC_Tot.GetBinError(i+1)
  #    print i+1,MC_Eff.GetBinContent(i+1)/MC_Tot.GetBinContent(i+1)

  # handling exceptions
  print "Checking MC eff bins ..."
  for i in range(MC_Eff.GetNbinsX()):
    print i+1, "th bin num:", MC_Eff.GetBinContent(i+1), "den:", MC_Tot.GetBinContent(i+1)
    if MC_Eff.GetBinContent(i+1) <= 0:
      MC_Eff.SetBinContent(i+1, 1)
      MC_Tot.SetBinContent(i+1, 0.1) # eff = 10 so out of range
  MC_Eff.Divide(MC_Eff,MC_Tot,1,1,"B")

  #for i in range(this_nBins):
  #  print i+1,MC_Eff.GetBinContent(i+1),"+-",MC_Eff.GetBinError(i+1)

  TS=OutName + "_"+ str(n_job)
  
  c1 = TCanvas("c1_"+TS,"",1000,1000)
  c1.cd()

  c_up = TPad("c_up", "", 0, 0.25, 1, 1)
  c_up.SetTopMargin(0.08)
  c_up.SetBottomMargin(0.017)
  c_up.SetLeftMargin(0.14)
  c_up.SetRightMargin(0.04)
  c_up.SetLogx()
  c_up.Draw()
  c_up.cd()

  c_up_min, c_up_max = GetMinMax(Data_Eff, MC_Eff)
  c_up_min *= 0.9

  Data_Eff.SetTitle("")
  Data_Eff.SetStats(0)
  Data_Eff.GetXaxis().SetLabelSize(0)
  Data_Eff.GetYaxis().SetLabelSize(0.045)
  Data_Eff.GetYaxis().SetTitle("Efficiency")
  Data_Eff.GetYaxis().SetTitleSize(0.075)
  Data_Eff.GetYaxis().SetTitleOffset(0.7)
  Data_Eff.GetYaxis().SetRangeUser(c_up_min, 1.1)
  Data_Eff.SetMarkerStyle(20)
  Data_Eff.SetMarkerColor(kBlack)
  Data_Eff.SetLineColor(kBlack)
  Data_Eff.Draw("ep")
  MC_Eff.SetMarkerStyle(20)
  MC_Eff.SetMarkerColor(kRed)
  MC_Eff.SetLineWidth(1)
  MC_Eff.SetLineColor(kRed)
  MC_Eff.Draw("ep same")

  lg = TLegend(0.3, 0.72, 0.5, 0.87)
  lg.AddEntry(Data_Eff, "Data", "lep")
  lg.AddEntry(MC_Eff, "MC", "lep")
  lg.SetBorderSize(0)
  lg.SetTextSize(0.03)
  lg.SetFillStyle(1001)
  lg.SetShadowColor(0)
  lg.Draw("same")
 
  txt_lumi = TLatex()
  txt_lumi.SetNDC()
  txt_lumi.SetTextSize(0.05)
  txt_lumi.SetTextAlign(32)
  txt_lumi.SetTextFont(42)
  txt_lumi.DrawLatex(.95,.96, luminosity[Era]+" fb^{-1} (13 TeV)")

  IDnames = {

    'HNL_ULID_Split_1' : 'HNL_ULID_Split_1',
    'HNL_ULID_Split_2' : 'HNL_ULID_Split_2',
    'HNL_ULID_Split_3' : 'HNL_ULID_Split_3',
    'HNL_ULID_Split_4' : 'HNL_ULID_Split_4',
    'HNL_ULID_Split_4b' : 'HNL_ULID_Split_4b',
    'HNL_ULID_Split_5' : 'HNL_ULID_Split_5',
    'HNL_ULID_Split_5b' : 'HNL_ULID_Split_5b',
    'HNL_ULID_Split_6' : 'HNL_ULID_Split_6',
    'HNL_ULID_Split_7' : 'HNL_ULID_Split_7',
    'HNL_ULID_Split_7b' : 'HNL_ULID_Split_7b',
    'HNL_ULID_Split_7c' : 'HNL_ULID_Split_7c',
    'HNL_ULID_Split_7d' : 'HNL_ULID_Split_7d',
    'HNL_ULID_Split_7e' : 'HNL_ULID_Split_7e',
    'HNL_ULID_Split_7f' : 'HNL_ULID_Split_7f',
    'HNL_ULID_Split_7g' : 'HNL_ULID_Split_7g',
    'HNL_ULID_Split_7h' : 'HNL_ULID_Split_7h',
    'HNL_ULID_Split_8' : 'HNL_ULID_Split_8',
    'HNL_ULID_Split_8b' : 'HNL_ULID_Split_8b',
             'HEEP'               : 'HEEP',
             'HNLMVA'             : 'MVA ID',
             'CutBasedTight94XV2' : 'POG Tight',
             'HNLMVA_TrkIso'      : 'MVA ID w/ TrkIso',
             'HNLHeep'            : 'MVA + HEEP combi.',
             'HNLMVAFake'         : 'MVA Fake',
             'HNLMVACF'           : 'MVA CF',
             'HNLMVAConv'         : 'MVA Conv',
             'MVALoose'           : 'Basic sel. for MVA',
  }
  txt_id = TLatex()
  txt_id.SetNDC()
  txt_id.SetTextSize(0.06)
  txt_id.SetTextAlign(12)
  txt_id.SetTextFont(42)
  txt_id.DrawLatex(.58,.86, IDnames[Probe])

  txt_eta = TLatex()
  txt_eta.SetNDC()
  txt_eta.SetTextSize(0.06)
  txt_eta.SetTextAlign(12)
  txt_eta.SetTextFont(42)
  txt_eta.DrawLatex(.58,.78, "|#eta| < 1.4442") if EtaRegion=='BB' else txt_eta.DrawLatex(.58,.78, "1.566 < |#eta| < 2.5")

  c1.cd()

  c_down = TPad("c_down", "", 0, 0, 1, 0.25)
  c_down.SetTopMargin(0.03)
  c_down.SetBottomMargin(0.35)
  c_down.SetLeftMargin(0.14)
  c_down.SetRightMargin(0.04)
  c_down.SetGridx()
  c_down.SetGridy()
  c_down.SetLogx()
  c_down.Draw()
  c_down.cd()

  Ratio = Data_Eff.Clone()
  Ratio.Divide(MC_Eff)

  print "Checking SF bins ..."
  for i in range(Ratio.GetNbinsX()):
    print i+1,"th bin:",Ratio.GetBinContent(i+1)

  c_down_min, c_down_max = GetMinMax(Ratio)
  c_down_min = max(0.9, c_down_min*0.95)
  c_down_max = 1.+(1.-c_down_min)

  Ratio.SetTitle("")
  Ratio.SetStats(0)
  Ratio.GetXaxis().SetTitle("p_{T} [GeV]")
  Ratio.GetYaxis().SetTitle("#frac{Data}{MC}")
  Ratio.GetYaxis().SetRangeUser(c_down_min, c_down_max)
  Ratio.GetXaxis().SetLabelSize(0.12)
  Ratio.GetYaxis().SetLabelSize(0.08)
  Ratio.GetXaxis().SetTitleSize(0.16)
  Ratio.GetYaxis().SetTitleSize(0.14)
  Ratio.GetXaxis().SetTitleOffset(0.9)
  Ratio.GetYaxis().SetTitleOffset(0.4)
  Ratio.SetLineColor(1)
  Ratio.SetMarkerColor(1)
  Ratio.SetMarkerStyle(20)
  Ratio.Draw("ep")

  #lg2 = TLegend(0.75, 0.88, 0.9, 0.95)
  #lg2.AddEntry(Ratio, "Scale factor", "lep")
  #lg2.SetBorderSize(1)
  #lg2.SetTextSize(0.06)
  #lg2.SetFillStyle(1001)
  #lg2.SetShadowColor(0)
  #lg2.Draw("same")

  minRange = Data_Eff.GetBinLowEdge(1)
  maxRange = Data_Eff.GetBinLowEdge(this_nBins) + Data_Eff.GetBinWidth(this_nBins)

  line = TLine(minRange, 1., maxRange, 1.)
  line.SetLineWidth(1)
  line.SetLineColor(2)
  line.Draw()

  c1.SaveAs(WorkDir+"/Out_Eff/"+OutName+"_"+str(n_job)+".png")
  del c1

  return

def SplitChain(_era,_type,_sample,nj):
  List = WorkDir+"/batch_input/"+_era+"_"+_sample+"_"+str(nj)+".txt"
  file1 = open(List, "r")  
  JobFiles=[]
  for x in file1:
    filex=x.split()[0]
    print filex+";"
    JobFiles.append(filex)
  file1.close()
  return JobFiles
  
def makeKinComparison(NthJob):

  pt_bins = np.array([35, 40, 45, 50, 60, 70, 80, 100, 150,200, 300, 400, 1000], dtype=np.float64)
  nBins = len(pt_bins)-1

  HEEP_eta_bins = np.array([0, 1.4442, 1.566, 2.5], dtype='d')
  nBins_HEEP_eta = len(HEEP_eta_bins)-1

  #for era in eras:
  for year, eras in grouped_eras.items():
 
    # Call necessary files and hists first
    TurnOnFiles = {}
    TurnOnHists = {}
    for era in eras:
      TurnOnFiles[era] = TFile.Open(WorkDir+"/Out_TurnOn/TurnOn_"+era+".root")
      TurnOnHists[era] = TurnOnFiles[era].Get("h_"+era+"_data_HEEP_barrel_Subt")

    OutFile = TFile.Open(WorkDir+"/Out_Eff/SF_"+year+"_"+str(NthJob)+".root","RECREATE")

    nMC = len(samples[year])-1
    mc_chains = [TChain("tnpEleIDs/fitter_tree") for _ in range(nMC)] # Don't use [] * nMC <-- this makes all the items share the same reference
  
    # Initialize h_mc
    h_mc = {}

    for EtaRegion in It_EtaRegions:
      h_mc[EtaRegion] = {}
      for Charge in It_Charges:
        h_mc[EtaRegion][Charge] = {}
        # ID iteration
        for Probe in It_Probes:
          h_mc[EtaRegion][Charge][Probe] = {}
          for IsPass in It_IsPasses:
            h_mc[EtaRegion][Charge][Probe][IsPass] = TH1D("pt_"+year+"_"+Charge+"_"+EtaRegion+"_"+Probe+"_"+IsPass,"pt_"+year+"_"+Charge+"_"+EtaRegion+"_"+Probe+"_"+IsPass,nBins,pt_bins) if "tot" in Charge else [TH1D("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass,"pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass,nBins,pt_bins) for sample in samples[year][:-1]]
        # All probes
        h_mc[EtaRegion][Charge]['All'] = TH1D("pt_"+year+"_"+Charge+"_"+EtaRegion,"pt_"+year+"_"+Charge+"_"+EtaRegion,nBins,pt_bins) if "tot" in Charge else [TH1D("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge,"pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge,nBins,pt_bins) for sample in samples[year][:-1]] 

    for i, sample in enumerate(samples[year]):
  
      if i == len(samples[year])-1: continue # skip the data
  
      for era in eras:
        t1 = datetime.now()
        print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Adding","/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+dates[era][sample]['']+"/*.root","..."
        for mc_file in SplitChain(era,types[sample],sample,NthJob):
          print ("Adding " + mc_file)
          mc_chains[i].Add(mc_file)
          #"/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+dates[era][sample]['']+"/*.root")
        t2 = datetime.now()
        print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."

      t1 = datetime.now()
      print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling total entries ..."
      #Nevents = args.Nevents if args.Nevents > 0 else mc_chains[i].GetEntriesFast() # The most time consuming part
      Nevents = args.Nevents if args.Nevents > 0 else mc_chains[i].GetEntries() # The most time consuming part
      t2 = datetime.now()
      print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."

      print "Running",Nevents,"events ..."
      
      t1 = datetime.now()
      print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Filling MC OS, SS events ..."
      for entry in range(Nevents):
        mc_chains[i].GetEntry(entry)
       
        # apply era-based turn on, even though the results can be merged
        this_era = mc_chains[i].GetCurrentFile().GetName().split('/')[-1].split('_')[1] # /gv0/DATA/SKFlat/Run2UltraLegacy_v3/2016preVFP/MC_SkimTree_EGammaTnP_HighPt/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/2024_12_03_013251/SKFlatNtuple_2016preVFP_MC_0.root

        weight_pt = mc_chains[i].tag_Ele_pt_cor
        if weight_pt >= 200: weight_pt = 199.5
        TurnOn_Weight = TurnOnHists[this_era].GetBinContent(TurnOnHists[this_era].FindBin(weight_pt)) # let's simulate as if MC tag passed the trigger

        #if entry%10000==0:
        #  print "weight_pt:",mc_chains[i].tag_Ele_pt_cor,"TurnOn:",TurnOn_Weight
        #  print "probe_pt:",mc_chains[i].el_pt_cor

        histinfo = classify_hist(year, mc_chains[i])

        if histinfo is not None:
          EtaRegion, Charges, Probes = histinfo

          this_weight = mc_chains[i].totWeight # Basic

          for Charge in Charges:
            if 'os' in Charge or 'zpt' in Charge:
              this_weight *= mc_chains[i].zptweight # Zpt
            if 'os' in Charge:
              this_weight *= TurnOn_Weight # OS TurnOn
            
            # ID iteration
            for ID, isPass in Probes.items():
              h_mc[EtaRegion][Charge][ID][isPass][i].Fill(mc_chains[i].el_pt_cor,this_weight)
              if 'ss' in Charge: h_mc[EtaRegion][Charge+'_tot'][ID][isPass].Fill(mc_chains[i].el_pt_cor,this_weight)
            # All probes
            h_mc[EtaRegion][Charge]['All'][i].Fill(mc_chains[i].el_pt_cor,this_weight)
            if 'ss' in Charge: h_mc[EtaRegion][Charge+'_tot']['All'].Fill(mc_chains[i].el_pt_cor,this_weight)
      t2 = datetime.now()
      print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
  
      OutFile.cd()
      t1 = datetime.now()
      print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Writing hists ..."
      for EtaRegion in It_EtaRegions:
        for Charge in It_Charges:
          if "tot" not in Charge:
            h_mc[EtaRegion][Charge]['All'][i] = add_overflow(h_mc[EtaRegion][Charge]['All'][i])
            h_mc[EtaRegion][Charge]['All'][i].Write()
            for Probe in It_Probes:
              for IsPass in It_IsPasses:
                  h_mc[EtaRegion][Charge][Probe][IsPass][i] = add_overflow(h_mc[EtaRegion][Charge][Probe][IsPass][i])
                  h_mc[EtaRegion][Charge][Probe][IsPass][i].Write()
      t2 = datetime.now()
      print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."

    #### Sample iteration done.

    OutFile.cd()
    print "before add_overflow:", h_mc['BB']['ss_tot']['All'].GetBinContent(nBins), h_mc['BB']['ss_tot']['All'].GetBinContent(nBins+1)
    for EtaRegion in It_EtaRegions:
      for Charge in It_Charges:
        if "tot" in Charge:
          h_mc[EtaRegion][Charge]['All'] = add_overflow(h_mc[EtaRegion][Charge]['All'])
          h_mc[EtaRegion][Charge]['All'].Write()
          for Probe in It_Probes:
            for IsPass in It_IsPasses:
                h_mc[EtaRegion][Charge][Probe][IsPass] = add_overflow(h_mc[EtaRegion][Charge][Probe][IsPass])
                h_mc[EtaRegion][Charge][Probe][IsPass].Write()
    print "now:",h_mc['BB']['ss_tot']['All'].GetBinContent(nBins)

    # Now collect OS MC samples into bundles
    h_Bundle = {}
    for EtaRegion in It_EtaRegions:
      h_Bundle[EtaRegion] = {}
      # ID iteration
      for Probe in It_Probes:
        h_Bundle[EtaRegion][Probe] = {}
        for IsPass in It_IsPasses:
          h_Bundle[EtaRegion][Probe][IsPass] = []
      # All probes
      h_Bundle[EtaRegion]['All'] = []
    
    for EtaRegion in It_EtaRegions:
      for Probe in It_Probes:
        for IsPass in It_IsPasses:
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][0].Clone()) # DY
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][1].Clone()) # WJets
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][2].Clone()) # ttbar
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][4].Clone()) # Diboson
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][7].Clone()) # SingleTop
          h_Bundle[EtaRegion][Probe][IsPass][2].Add(h_mc[EtaRegion]['os'][Probe][IsPass][3])
          h_Bundle[EtaRegion][Probe][IsPass][3].Add(h_mc[EtaRegion]['os'][Probe][IsPass][5])
          h_Bundle[EtaRegion][Probe][IsPass][3].Add(h_mc[EtaRegion]['os'][Probe][IsPass][6])
          h_Bundle[EtaRegion][Probe][IsPass][4].Add(h_mc[EtaRegion]['os'][Probe][IsPass][8])
  
          h_Bundle[EtaRegion][Probe][IsPass][0].SetFillColor(kSpring+10)
          h_Bundle[EtaRegion][Probe][IsPass][1].SetFillColor(kBlue)
          h_Bundle[EtaRegion][Probe][IsPass][2].SetFillColor(kYellow)
          h_Bundle[EtaRegion][Probe][IsPass][3].SetFillColor(kRed)
          h_Bundle[EtaRegion][Probe][IsPass][4].SetFillColor(kViolet)

      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][0].Clone()) # DY
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][1].Clone()) # WJets
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][2].Clone()) # ttbar
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][4].Clone()) # Diboson
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][7].Clone()) # SingleTop
      h_Bundle[EtaRegion]['All'][2].Add(h_mc[EtaRegion]['os']['All'][3])
      h_Bundle[EtaRegion]['All'][3].Add(h_mc[EtaRegion]['os']['All'][5])
      h_Bundle[EtaRegion]['All'][3].Add(h_mc[EtaRegion]['os']['All'][6])
      h_Bundle[EtaRegion]['All'][4].Add(h_mc[EtaRegion]['os']['All'][8])
  
      h_Bundle[EtaRegion]['All'][0].SetFillColor(kSpring+10)
      h_Bundle[EtaRegion]['All'][1].SetFillColor(kBlue)
      h_Bundle[EtaRegion]['All'][2].SetFillColor(kYellow)
      h_Bundle[EtaRegion]['All'][3].SetFillColor(kRed)
      h_Bundle[EtaRegion]['All'][4].SetFillColor(kViolet)
  
    # Data
    data_chain = TChain("tnpEleIDs/fitter_tree")
  
    # now 'sample' is data...
    t1 = datetime.now()
    print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling",year,"data ..."
    for era in eras:
      for period in dates[era][sample]:
        print "Adding","/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root","..."

        for data_file in SplitChain(era,types[sample],sample+"_"+period,NthJob):
          data_chain.Add(data_file)

          #data_chain.Add("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root")
    t2 = datetime.now()
    print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
  
    #t1 = datetime.now()
    #print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling total entries ..."
    #print "data entries:",data_chain.GetEntries() # The most time consuming part
    #t2 = datetime.now()
    #print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."

    h_data = {}

    for EtaRegion in It_EtaRegions:
      h_data[EtaRegion] = {}
      for Charge in ["os","ss"]:
        h_data[EtaRegion][Charge] = {}
        # ID iteration
        for Probe in It_Probes:
          h_data[EtaRegion][Charge][Probe] = {}
          for IsPass in It_IsPasses:
            h_data[EtaRegion][Charge][Probe][IsPass] = TH1D("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass,"pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass,nBins,pt_bins)
        # All probes
        h_data[EtaRegion][Charge]['All'] = TH1D("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge,"pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge,nBins,pt_bins)

    # pre-processing for main jobs
    this_draw_command = "el_pt_cor>>pt_"+year+"_"+nameFilter[sample]
    this_cuts = {
                 "BB" : "(fabs(el_sc_eta)<1.4442)",
                 "EC" : "(1.566<fabs(el_sc_eta))&&(fabs(el_sc_eta)<2.5)",
                 "os" : "(el_q+tag_Ele_q==0)",
                 "ss" : "(el_q+tag_Ele_q!=0)",
    }


    #### add ID of N-1 to probe Fail 
    nProbe_data=0
    for Probe in It_Probes:
      ProbeID_data=It_ProbeID[nProbe_data]
      if nProbe_data < 5:
        this_cuts[Probe] = {
          'Pass':"(passing"+Probe+"==1)",
          'Fail':"(passing"+Probe+"==0)",
        }
      else:
        this_cuts[Probe] = {
          'Pass':"(passing"+Probe+"==1)",
          'Fail':"(passing"+Probe+"==0 && passing"+ProbeID_data+"==1)",
        }

      nProbe_data=nProbe_data+1

    #### create hists w/ cuts applied
    t1 = datetime.now()
    print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Creating hists ..."

    for EtaRegion in It_EtaRegions:
      for Charge in ["os","ss"]:
        # ID iteration

        nProbe_data=0 
        for Probe in It_Probes:

          for IsPass in It_IsPasses:
            draw_command = this_draw_command+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass
            cut_condition = this_cuts[EtaRegion]+"&&"+this_cuts[Charge]+"&&"+this_cuts[Probe][IsPass]
            if args.Nevents > 0:
              cut_condition += "&&Entry$<"+str(args.Nevents)
            print "Now processing: Draw(\""+draw_command+"\",\""+cut_condition+"\")"
            data_chain.Draw(draw_command, cut_condition)
        # All probes
        draw_command = this_draw_command+"_"+EtaRegion+"_"+Charge
        cut_condition = this_cuts[EtaRegion]+"&&"+this_cuts[Charge]
        if args.Nevents > 0:
          cut_condition += "&&Entry$<"+str(args.Nevents)
        print "Now processing:","Draw(\""+draw_command+"\",\""+cut_condition+"\")"
        data_chain.Draw(draw_command, cut_condition)

    t2 = datetime.now()
    print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
  
    h_Stack = {}
    h_Error = {}
    for EtaRegion in It_EtaRegions:
      h_Stack[EtaRegion] = {}
      h_Error[EtaRegion] = {}
      # ID iteration
      for Probe in It_Probes:
        h_Stack[EtaRegion][Probe] = {}
        h_Error[EtaRegion][Probe] = {}
        for IsPass in It_IsPasses:
          h_Stack[EtaRegion][Probe][IsPass] = THStack("hs_pt_"+year+"_"+EtaRegion+"_"+Probe+"_"+IsPass,"hs_pt_"+year+"_"+EtaRegion+"_"+Probe+"_"+IsPass)
          h_Error[EtaRegion][Probe][IsPass] = h_Bundle[EtaRegion][Probe][IsPass][0].Clone()
          h_Error[EtaRegion][Probe][IsPass].Reset()
      # All probes
      h_Stack[EtaRegion]['All'] = THStack("hs_pt_"+year+"_"+EtaRegion,"hs_pt_"+year+"_"+EtaRegion)
      h_Error[EtaRegion]['All'] = h_Bundle[EtaRegion]['All'][0].Clone()
      h_Error[EtaRegion]['All'].Reset()

    OutFile.cd()

    # Declare Eff, SF hists
    h_SFs = {}

    #### make plots and measure SFs
    t1 = datetime.now()
    print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Measuring SFs ..."

    #print "before add_overflow:",h_data['BB']['os']['All'].GetBinContent(nBins), h_data['BB']['os']['All'].GetBinContent(nBins+1)

    for EtaRegion in It_EtaRegions:
      # ID iteration
      for Probe in It_Probes:
        if Probe not in h_SFs:
          h_SFs[Probe] = {
                          'Data_Eff' : TH1D("DataEff_"+Probe, "DataEff_"+Probe, nBins_HEEP_eta, HEEP_eta_bins),
                          'MC_Eff'   : TH1D("MCEff_"+Probe, "MCEff_"+Probe, nBins_HEEP_eta, HEEP_eta_bins),
                          'SF'       : TH1D("SF_"+Probe, "SF_"+Probe, nBins_HEEP_eta, HEEP_eta_bins),
          }
        for IsPass in It_IsPasses:
          for Charge in ["os","ss"]:
            # Add overflow
            h_data[EtaRegion][Charge][Probe][IsPass] = add_overflow(h_data[EtaRegion][Charge][Probe][IsPass])
            h_data[EtaRegion][Charge][Probe][IsPass].Write()

          # SS data - SS prompt = OS fake
          h_data[EtaRegion]['ss'][Probe][IsPass].Add(h_mc[EtaRegion]['ss_tot'][Probe][IsPass],-1)
          h_Bundle[EtaRegion][Probe][IsPass].append(h_data[EtaRegion]['ss'][Probe][IsPass].Clone()) # Fake
          h_Bundle[EtaRegion][Probe][IsPass][5].SetFillColor(kAzure+1)

          # Now Sum up all bkgs to estimate combined error, and collect bundles into one stack
          for iBundle in reversed(range(len(h_Bundle[EtaRegion][Probe][IsPass]))):
            h_Bundle[EtaRegion][Probe][IsPass][iBundle].SetLineWidth(0)
            h_Error[EtaRegion][Probe][IsPass].Add(h_Bundle[EtaRegion][Probe][IsPass][iBundle])
            h_Stack[EtaRegion][Probe][IsPass].Add(h_Bundle[EtaRegion][Probe][IsPass][iBundle])
  
          print "Making pass/fail plots ..."
          makePlots(h_data[EtaRegion]['os'][Probe][IsPass], h_Stack[EtaRegion][Probe][IsPass], h_Bundle[EtaRegion][Probe][IsPass], h_Error[EtaRegion][Probe][IsPass], year, "_"+EtaRegion+"_"+Probe+"_"+IsPass+Name_Nevents,NthJob)
        print "Calculating SFs ..."
        measureSFs(h_data[EtaRegion]['os'][Probe], h_Bundle[EtaRegion][Probe], year, EtaRegion, Probe, Name_Nevents, h_SFs[Probe], NthJob,OutFile)

      # All probes
      for Charge in ["os","ss"]:
        # Add overflow
        h_data[EtaRegion][Charge]['All'] = add_overflow(h_data[EtaRegion][Charge]['All'])
        h_data[EtaRegion][Charge]['All'].Write()

      # SS data - SS prompt = OS fake
      h_data[EtaRegion]['ss']['All'].Add(h_mc[EtaRegion]['ss_tot']['All'],-1)
      h_Bundle[EtaRegion]['All'].append(h_data[EtaRegion]['ss']['All'].Clone()) # Fake
      h_Bundle[EtaRegion]['All'][5].SetFillColor(kAzure+1)

      # Now Sum up all bkgs to estimate combined error, and collect bundles into one stack
      print h_Error[EtaRegion]['All'].GetBinContent(1), h_Error[EtaRegion]['All'].GetBinError(1) # to check h_Error was reset successfully
      for iBundle in reversed(range(len(h_Bundle[EtaRegion]['All']))):
        h_Bundle[EtaRegion]['All'][iBundle].SetLineWidth(0)
        h_Error[EtaRegion]['All'].Add(h_Bundle[EtaRegion]['All'][iBundle])
        h_Stack[EtaRegion]['All'].Add(h_Bundle[EtaRegion]['All'][iBundle])
      print h_Error[EtaRegion]['All'].GetBinContent(1), h_Error[EtaRegion]['All'].GetBinError(1)
  
      print "Making all probes plots ..."
      makePlots(h_data[EtaRegion]['os']['All'], h_Stack[EtaRegion]['All'], h_Bundle[EtaRegion]['All'], h_Error[EtaRegion]['All'], year, "_"+EtaRegion+"_AllProbes"+Name_Nevents,NthJob)
    #### EtaRegion done.

    #print "now:",h_data['BB']['os']['All'].GetBinContent(nBins)

    t2 = datetime.now()
    print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
    
    #for Probe in h_SFs.keys():
    #  for SF in h_SFs[Probe].keys():
    #    h_SFs[Probe][SF].Write()

    OutFile.Close()

  return

if __name__ == '__main__':
  beginTime = datetime.now()
  #TurnOn()
  makeKinComparison(NJob)
  endTime = datetime.now()
  print "["+endTime.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Total done in",endTime-beginTime,"."
