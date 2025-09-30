# Place it at /data6/Users/jihkim/HNDiLeptonWorskspace/Limits/ReadLimits/Shape
# python ReadLimitFromTree.py --Full[--Asymptotic]

from ROOT import *
import os, argparse

parser = argparse.ArgumentParser(description='option')
parser.add_argument('--BDT', action='store_true')
parser.add_argument('--Ext', action='store_true')
parser.add_argument('--Asymptotic', action='store_true')
parser.add_argument('--Full', action='store_true')
args = parser.parse_args()

#workdir = "/data6/Users/jihkim/CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter/Batch/"
#workdir = "/data6/Users/jihkim/NewCombine/CMSSW_14_1_0_pre4/src/DilepHN/Batch/"
workdir = "/data6/Users/jihkim/LatestCombine/CMSSW_14_1_0_pre4/src/DilepHN/Batch/"

years = ["2016","2017","2018"]
years = ["Run2"]
years = ["2016preVFP","2016postVFP","2017","2018","Run2"]
years = ["Run2"]
#years = ["2017"]
#years = ["2018"]
channels = ["MuMu","EE","EMu"]
#channels = ["MuMu","EE"]
#channels = ["EE"]
#channels = ["MuMu"]
#channels = ["EMu"]
#masses = ["100","200","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000"]
#masses = ["90","100","150","200","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["100","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["100","200","300","400","1000","10000"]
masses = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000","25000","30000"]
masses_EMu = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000","25000","30000","40000","50000","60000"]
#masses = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses_EMu = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000"]
#masses_EMu = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000"]

IDs = [""] #["_ID"]

#myWPs = ["240422_HNL_ULID"]
#myWPs = ["240501_1704_HNL_ULID","240501_1704_HNTightV2"]
#myWPs = ["240504_PR44_HNL_ULID"]
#myWPs = ["240505_PR45_HNL_ULID"]
#myWPs = ["240505_PR46_HNL_ULID","240505_PR46_HNTightV2"]
#myWPs = ["rateParam_HNL_ULID"]
#myWPs = ["PR48_rateParam_HNL_ULID"]
#myWPs = ["PR55_HNL_ULID","PR55_HighPt"]
#myWPs = ["PR55_NoMinPt_HighPt"]
#myWPs = ["PR86_HNL_ULID_Decorr"]
#myWPs = ["PR95_HNL_ULID_Decorr"]
#myWPs = ["PR95_HNL_ULID_Decorr","PR95_HNL_ULID_Decorr_NoMuSyst","PR95_HNL_ULID_MuSystOnly"] # WP of input limit file
#myWPs = ["PR95_HNL_ULID_MuSystOnly"] # WP of input limit file
#myWPs = ["PR95_HNL_ULID"] # WP of input limit file
#myWPs = ["PR95_HNL_ULID_NoCR_Decorr"]
#myWPs = ["PR89_HNL_ULID_HighPtIDComp_lnNsyst_Decorr","PR89_HighPt_HighPtIDComp_lnNsyst_Decorr"]
#myWPs = ["PR97_HNL_ULIDv2_NoCR_NoSyst"]
#myWPs = ["ANv3_HNL_ULIDv2_Decorr_NoCR"]
#myWPs = ["ANv3_HNL_ULIDv2_Decorr_TEST_NoCR"]
#myWPs = ["ANv3_HNL_ULIDv2_Decorr"]
#myWPs = ["ANv3_HNL_ULIDv2_Decorr_Run2"]
#myWPs = ["HEMJet_HNL_ULIDv2_RemoveHEMJet_NoCR_NoSyst","HEMJet_HNL_ULIDv2_ScaleHEMJet_NoCR_NoSyst","TuneP_HNL_ULIDv2_CompareTuneP_NoCR_NoSyst","TuneP_HNTightV2_CompareTuneP_NoCR_NoSyst","TuneP_POGTight_CompareTuneP_NoCR_NoSyst"]
#myWPs = ["ANv3_HNL_ULIDv2_Decorr_NoCR_NoSyst"]
#myWPs = ["ANv4_HNL_ULIDv2_RunSyst_Decorr_JetDecorr_NoCR"]
#myWPs = ["ANv4_HNL_ULIDv2_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv5_HNL_ULIDv2_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv5_HNL_ULIDv2_RunSyst_BeforeJetIDLepPt_Decorr_JetDecorr","ANv5_HNL_ULIDv2_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv5_HNL_ULIDv2_RunSyst_Decorr_JetDecorr","ANv5_BDTV3_StrictBinning_HNL_ULIDv2_RunSyst_Decorr_JetDecorr","ANv5_BDTV3_LooseBinning_HNL_ULIDv2_RunSyst_Decorr_JetDecorr","ANv5_BDTV4_StrictBinning_HNL_ULIDv2_RunSyst_Decorr_JetDecorr","ANv5_BDTV4_LooseBinning_HNL_ULIDv2_RunSyst_Decorr_JetDecorr","ANv5_BDTV4_VeryLooseBinning_HNL_ULIDv2_RunSyst_Decorr_JetDecorr",]
#myWPs = ["ANv5_BDTV4_BugFix_HNL_ULIDv2_V4_LooseBin_RunSyst_Decorr_JetDecorr","ANv5_BDTV4_BugFix_HNL_ULIDv2_V4_StrictBin_RunSyst_Decorr_JetDecorr",]
#myWPs = ["ANv5_BDTV4_BugFix_HNL_ULIDv2_V3_LooseBin_RunSyst_Decorr_JetDecorr","ANv5_BDTV4_BugFix_HNL_ULIDv2_V3_StrictBin_RunSyst_Decorr_JetDecorr"]
#myWPs = [
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V2_StrictBin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V3_StrictBin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V3_Strict_10_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V3_Strict_20_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V4_Strict_10_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V4_Strict_15_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V4_Strict_20_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V2_StrictBin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V3_StrictBin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V3_Strict_10_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V3_Strict_20_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V4_Strict_10_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V4_Strict_15_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V4_Strict_20_Bin_RunSyst_Decorr_JetDecorr",
#]
#myWPs = ["ANv5_BDTV3_UpdateSRBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr","ANv5_BDTV3_UpdateSRBinning_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv5_BDTV3_AltSR1_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr","ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"]
myWPs = ["ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewRP"]

#tags = ["_sronly_syst"]
#tags = ["_sronly"]
#tags = ["_syst"]
#tags = ["_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined"]
#tags = ["_sronly_sr123_syst"]
#tags = ["_sronly_sr123"]
#tags = ["_DYVBF_sronly_sr123_syst"]
tags = ["_DYVBF_syst","_DY_syst","_VBF_syst","_SSWW_syst"]
#tags = ["_syst","_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined"]

BDTTag = '_BDT' if args.BDT else ''
ExtTag = '_Ext' if args.Ext else ''

for WP in myWPs:
  this_workdir = workdir+WP
  WP = WP+BDTTag+ExtTag
  os.system("mkdir -p out/"+WP)
  for year, channel, ID, tag in [[year, channel, ID, tag] for year in years for channel in channels for ID in IDs for tag in tags]:
    
    if args.Asymptotic:
      with open("out/"+WP+"/"+year+"_"+channel+ID+tag+"_Asym_limit.txt", 'w') as f:
      #with open("out/"+WP+"/"+year+"_"+channel+ID+tag+"_Run2Scaled_Asym_limit.txt", 'w') as f:
      #with open("out/"+WP+"/"+year+"_"+channel+ID+tag+"_Run23Scaled_Asym_limit.txt", 'w') as f:
  
        for mass in (masses if channel!="EMu" else masses_EMu):
          this_name = year+"_"+channel+"_M"+mass+ID+tag
          if args.BDT:
            if float(mass)>500.: continue
          elif args.Ext:
            if float(mass)<500.:
              f.write("\n")
              continue
            elif float(mass)==500: 
              this_name = year+"_"+channel+ExtTag+"_M"+mass+ID+tag
          print(this_name)
          path = this_workdir+"/Asymptotic/"+this_name+"/output/"+this_name+"_Asymptotic.root"
  
          try: f_Asym = TFile.Open(path)
          except OSError:
            f.write("\n")
            continue
          tree_Asym = f_Asym.Get("limit")
  
          try: tree_Asym.GetEntry(2) # substitute for obs. limit for now
          except AttributeError:
            f.write("\n")
            continue
          f.write(mass+"\t"+str(round(tree_Asym.limit,3))+"\t")
          #f.write(mass+"\t"+str(round(tree_Asym.limit/1.82,3))+"\t") # FIXME estimating full Run2 from 2017
          #f.write(mass+"\t"+str(round(tree_Asym.limit/1.52,3))+"\t") # FIXME estimating full Run2 from 2018
          #f.write(mass+"\t"+str(round(tree_Asym.limit/3.16,3))+"\t") # FIXME estimating full Run2+3 from 2017
          #f.write(mass+"\t"+str(round(tree_Asym.limit/1.77,3))+"\t") # FIXME estimating full Run2+3 from Run2
  
          for i in range(5): # expected limits
            tree_Asym.GetEntry(i)
            f.write(str(round(tree_Asym.limit,3))+"\t")
            #f.write(str(round(tree_Asym.limit/1.82,3))+"\t") # FIXME estimating full Run2 from 2017
            #f.write(str(round(tree_Asym.limit/1.52,3))+"\t") # FIXME estimating full Run2 from 2018
            #f.write(str(round(tree_Asym.limit/3.16,3))+"\t") # FIXME estimating full Run2+3 from 2017
            #f.write(str(round(tree_Asym.limit/1.77,3))+"\t") # FIXME estimating full Run2+3 from Run2
          f.write("\n")
          print("done.")
  
    if args.Full:
      with open("out/"+WP+"/"+year+"_"+channel+ID+tag+"_Full_limit.txt", 'w') as f:
  
        for mass in (masses if channel!="EMu" else masses_EMu):
          this_name = year+"_"+channel+"_M"+mass+ID+tag
          if args.BDT:
            if float(mass)>500.: continue
          elif args.Ext:
            if float(mass)<500.:
              f.write("\n")
              continue
            elif float(mass)==500: 
              this_name = year+"_"+channel+ExtTag+"_M"+mass+ID+tag
          print(this_name)
          paths = [
                  this_workdir+"/full_CLs/"+this_name+"/output/"+this_name+"_Q1.root",
                  this_workdir+"/full_CLs/"+this_name+"/output/"+this_name+"_Q2.root",
                  this_workdir+"/full_CLs/"+this_name+"/output/"+this_name+"_Q3.root",
                  this_workdir+"/full_CLs/"+this_name+"/output/"+this_name+"_Q4.root",
                  this_workdir+"/full_CLs/"+this_name+"/output/"+this_name+"_Q5.root",
                  ]
  
          f_Q1 = TFile.Open(paths[0])
          f_Q2 = TFile.Open(paths[1])
          f_Q3 = TFile.Open(paths[2])
          f_Q4 = TFile.Open(paths[3])
          f_Q5 = TFile.Open(paths[4])
  
          tree_Q1 = f_Q1.Get("limit")
          tree_Q2 = f_Q2.Get("limit")
          tree_Q3 = f_Q3.Get("limit")
          tree_Q4 = f_Q4.Get("limit")
          tree_Q5 = f_Q5.Get("limit")
          
          tree_Q1.GetEntry(0)
          tree_Q2.GetEntry(0)
          tree_Q3.GetEntry(0)
          tree_Q4.GetEntry(0)
          tree_Q5.GetEntry(0)
          
          f.write(mass+"\t"+str(round(tree_Q3.limit,3))+"\t"+str(round(tree_Q1.limit,3))+"\t"+str(round(tree_Q2.limit,3))+"\t"+str(round(tree_Q3.limit,3))+"\t"+str(round(tree_Q4.limit,3))+"\t"+str(round(tree_Q5.limit,3))+"\n")
