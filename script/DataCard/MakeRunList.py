# Place this at CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter
# python MakeRunList.py <directory> [-e 2017 2018] [-c EMu] [-m 100 200]

import os
import commands as cmd
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('dirNames', nargs='+') # nargs='+' force a user to feed this argument
parser.add_argument('-e', dest='eras', default=[], nargs='+')
parser.add_argument('-c', dest='channels', default=[], nargs='+') # store [] if nothing is fed
parser.add_argument('-m', dest='masses', default=[], nargs='+')
parser.add_argument('--Work', action='store_true', help='for workspace production purposes')
parser.add_argument('--Limit', action='store_true', help='for limit extraction purposes')
args = parser.parse_args()

#mylist = cmd.getoutput("ls /data6/Users/jihkim/CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter/*.root | grep HNL_UL.root")
#with open("RunList.txt",'w') as f:
#  f.write(mylist)
#channels = ["MuMu","EE"]
#for channel in channels:
#  mylist = cmd.getoutput("ls /data6/Users/jihkim/CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter/exo17028_CombinedYears_noSSWW/"+channel).split('\n')
#  mylist = '\n'.join(["/data6/Users/jihkim/CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter/exo17028_CombinedYears_noSSWW/"+channel+"/"+s for s in mylist])
#  with open("RunList_exo17028_noSSWW.txt",'a') as f:
#    f.write(mylist)
#    f.write('\n')

input_path = os.getcwd()

grepRegion = ' | grep card' if "Run2" in args.eras else ' | grep sr3_inv' # When you grep an individual era, there are many duplications with different regions, namely sr1, ww_cr, sr3_inv, etc, and even directories! Pick just one (grepping 'card' for Run2 or 'sr3_inv' for the others)

#tags = ["_sronly"]
tags = ["_syst"]
#tags = ["_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined","_syst"]
#tags = ["_sr1_syst","_sr2_syst","_sr3_syst","_sr_syst"]
#tags = ["_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined","_sr_syst_Combined","_syst","_sr1_syst","_sr2_syst","_sr3_syst","_sr_syst"]
#tags = ["_sr_syst_NoCFCR_Combined","_sr_syst_NoInv_Combined","_sr_syst_NoCFCR_NoInv_Combined"]

## Inclusive
#tags = ["_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined","_sr_syst_Combined","_syst","_sr1_syst","_sr2_syst","_sr3_syst","_sr_syst","_sr_syst_NoCFCR_Combined","_sr_syst_NoInv_Combined","_sr_syst_NoCFCR_NoInv_Combined"]

## No syst
#tags = ["_sr1_Combined","_sr2_Combined","_sr3_Combined","_sr_Combined","","_sr_NoCFCR_Combined","_sr_NoInv_Combined","_sr_NoCFCR_NoInv_Combined"]
## SR only
#tags = ["_sronly_sr1","_sronly_sr2","_sronly_sr3","_sronly_sr123","_sronly_sr","_sronly_sr1_syst","_sronly_sr2_syst","_sronly_sr3_syst","_sronly_sr123_syst","_sronly_sr_syst"]

isRun2 = ""
if "Run2" in args.eras:
  tags = ["_syst"]
  isRun2 = "Run2_"

for dirName in args.dirNames:

  greps = 'ls '+dirName+grepRegion+' | grep '*int(bool(args.eras))+' '.join(["-e "+era for era in args.eras])+' | grep '*int(bool(args.channels))+' '.join(["-e "+channel for channel in args.channels])+' | grep '*int(bool(args.masses))+' '.join(["-e M"+mass+"_" for mass in args.masses]) # if any of eras, chs, ms exists, this line greps it in order. if not, just ls the directory
  #print greps

  cards = cmd.getoutput(greps).replace('_sr3_inv.txt','').replace('_syst.txt','').split('\n')
  #print cards
  dirName = dirName.replace('/','')
  with open("RunList_"+isRun2+dirName+".txt",'w') as f: # FIXME maybe you can add "Work" or "Limit" to the file name and keep separate...
    for card in cards:
      for tag in tags:
        this = input_path+"/"+dirName+"/"+card+tag+".root\n"
        if args.Work:
          f.write(this)
        elif args.Limit:
          shortcard = this.split('/')[-1].replace(".root","").replace(".txt","").replace("card_","").strip('\n')
          this = input_path+"/"+dirName+"/"+shortcard+"/"+shortcard+".root\n"
          f.write(this)
