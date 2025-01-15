# Place it at /data6/Users/jihkim/HNDiLeptonWorskspace/Limits/ReadLimits/Shape
# python ReadLimitFromTree.py --Full[--Asymptotic]

from ROOT import *
import os, argparse

parser = argparse.ArgumentParser(description='option')
parser.add_argument('--Asymptotic', action='store_true')
parser.add_argument('--Full', action='store_true')
args = parser.parse_args()

#workdir = "/data6/Users/jihkim/CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter/Batch/"
#workdir = "/data6/Users/jihkim/NewCombine/CMSSW_14_1_0_pre4/src/DilepHN/Batch/"
workdir = "/data6/Users/jihkim/LatestCombine/CMSSW_14_1_0_pre4/src/DilepHN/Batch/"

years = ["2016","2017","2018"]
years = ["Run2"]
years = ["2016preVFP","2016postVFP","2017","2018","Run2"]
#years = ["Run2"]
#years = ["2017"]
years = ["2018"]
channels = ["MuMu","EE","EMu"]
#channels = ["MuMu","EE"]
#channels = ["EE"]
#channels = ["MuMu"]
#masses = ["100","200","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000"]
#masses = ["90","100","150","200","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["90","100","150","200","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
masses = ["125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["100","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["100","200","300","400","1000","10000"]
IDs = [""] #["_ID"]
#tags = ["_sronly_syst"] #["_DYVBF"]
#tags = ["_sronly"] #["_DYVBF"]
#tags = ["_syst"] #["_DYVBF"]
#tags = ["_sronly_sr123_syst"] #["_DYVBF"]
tags = ["_sronly_sr123"] #["_DYVBF"]
#tags = [""] #["_DYVBF"]
#tags = ["_sr_Combined"] #["_DYVBF"]
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
myWPs = ["PR97_HNL_ULIDv2_NoCR"]

for WP in myWPs:
  this_workdir = workdir+WP
  os.system("mkdir -p out/"+WP)
  for year, channel, ID, tag in [[year, channel, ID, tag] for year in years for channel in channels for ID in IDs for tag in tags]:
    
    if args.Asymptotic:
      #with open("out/"+WP+"/"+year+"_"+channel+ID+tag+"_Asym_limit.txt", 'w') as f:
      with open("out/"+WP+"/"+year+"_"+channel+ID+tag+"_Run2Scaled_Asym_limit.txt", 'w') as f:
      #with open("out/"+WP+"/"+year+"_"+channel+ID+tag+"_Run23Scaled_Asym_limit.txt", 'w') as f:
  
        for mass in masses:
          this_name = year+"_"+channel+"_M"+mass+ID+tag
          print this_name
          path = this_workdir+"/Asymptotic/"+this_name+"/output/"+this_name+"_Asymptotic.root"
  
          f_Asym = TFile.Open(path)
          tree_Asym = f_Asym.Get("limit")
  
          tree_Asym.GetEntry(2) # substitute for obs. limit for now
          #f.write(mass+"\t"+str(round(tree_Asym.limit,3))+"\t")
          #f.write(mass+"\t"+str(round(tree_Asym.limit/1.82,3))+"\t") # FIXME estimating full Run2 from 2017
          f.write(mass+"\t"+str(round(tree_Asym.limit/1.52,3))+"\t") # FIXME estimating full Run2 from 2018
          #f.write(mass+"\t"+str(round(tree_Asym.limit/3.16,3))+"\t") # FIXME estimating full Run2+3 from 2017
  
          for i in range(5): # expected limits
            tree_Asym.GetEntry(i)
            #f.write(str(round(tree_Asym.limit,3))+"\t")
            #f.write(str(round(tree_Asym.limit/1.82,3))+"\t") # FIXME estimating full Run2 from 2017
            f.write(str(round(tree_Asym.limit/1.52,3))+"\t") # FIXME estimating full Run2 from 2018
            #f.write(str(round(tree_Asym.limit/3.16,3))+"\t") # FIXME estimating full Run2+3 from 2017
          f.write("\n")
          print "done."
  
    if args.Full:
      with open("out/"+WP+"/"+year+"_"+channel+ID+tag+"_Full_limit.txt", 'w') as f:
  
        for mass in masses:
          this_name = year+"_"+channel+"_M"+mass+ID+tag
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
