#!/usr/bin/env python3

# Place it at CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter
# python create-batch.py -l RunList1.txt [RunList2.txt][RunList*.txt] --Asymptotic[--Full][--Q*][--Work][--Nuis][--pdf]
# RunList.txt contains paths of results from text2workspace.py e.g. /data6/Users/jihkim/CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter/Workspace/card_2017_MuMu_M500_HNL_UL.root

import os, sys
import subprocess as cmd
import argparse
import datetime

parser = argparse.ArgumentParser(description='option')
parser.add_argument('--pdf', action='store_true', help='do pdfseparate; run this after getting all impacts')
parser.add_argument('-i', dest='Input', help='take a single argument. [NOTE] feed realpath of a card (or workspace) !!')
parser.add_argument('-l', dest='RunLists', nargs='+', help='take args as a list, return error when there is no arg')
parser.add_argument('--Full', action='store_true')
parser.add_argument('--Q1', action='store_true')
parser.add_argument('--Q2', action='store_true')
parser.add_argument('--Q3', action='store_true')
parser.add_argument('--Q4', action='store_true')
parser.add_argument('--Q5', action='store_true')
parser.add_argument('-t', dest='Ntoy', default='1000', help='N of toys')
parser.add_argument('--Asymptotic', action='store_true')
parser.add_argument('--Work', action='store_true', help='create workspace')
parser.add_argument('--FitDiag', action='store_true', help='check nuisance fit')
parser.add_argument('--Impact', action='store_true', help='check impacts')
parser.add_argument('--FastScan', action='store_true', help='fast scan')
parser.add_argument('--MDfit', action='store_true', help='multidimension fits')
parser.add_argument('--Breakdown', action='store_true', help='uncertainty breakdown')
parser.add_argument('--GOF', action='store_true', help='goodness of fit test')
parser.add_argument('--InjectSignal', default='0', help='inject signals to asimov')
args = parser.parse_args()

IsNuis = False # Limit extraction setting
Ncheck = 0
for check in ['FitDiag','Impact','Breakdown','FastScan','MDfit','GOF']:
  if vars(args)[check] is True:
    this_check = check
    IsNuis = True # Statistical tests
    Ncheck += 1
if Ncheck > 1:
  print("More than 1 Nuisance flag activated; This is not supported.")
  print("Exiting ...")
  sys.exit(1)

AsimovSetting = " -t -1 --expectSignal="+args.InjectSignal
AsimovName = "s"+args.InjectSignal

pwd = os.getcwd()
CMSSW_BASE = os.environ['CMSSW_BASE']
SCRAM_ARCH = os.environ['SCRAM_ARCH']

failure, result = cmd.getstatusoutput('combine --help')
if failure:
  print("[!!ERROR!!] cannot run combine.")
  print("Please set proper cmsenv first.")
  print("Exiting ...")
  sys.exit(1)

if args.Input is not None:
  args.RunLists = []
  args.RunLists.append(args.Input)

for RunList in args.RunLists:
  cards = open(RunList).readlines() if args.Input is None else [args.Input]
  NCARD = len(cards)
  WP = RunList.split('.')[-2].replace('RunList_','').replace('Run2_','') if args.Input is None else args.Input.split('/')[-2] # Currently, RunList is splitted into Run2 and normal setting (code structure issue -- it doesn't change WP)

  if args.pdf:
    os.system('mkdir -p '+this_check+'/'+WP+'/'+AsimovName)
  elif args.Work or IsNuis: # Make workspace or perform statistical tests
    with open(WP+'/submit_skeleton.sh','w') as skel:
      skel.write("universe = vanilla\n")
      skel.write("+SingularityImage = \"/cvmfs/singularity.opensciencegrid.org/opensciencegrid/osgvo-el9:latest\"\n")
      skel.write("should_transfer_files = YES\n")
      skel.write("when_to_transfer_output = ON_EXIT\n")
      skel.write("request_memory = 24000\n")
      skel.write("request_cpus = 4\n")
  else: # Extract limits
    os.system('mkdir -p Batch/'+WP)
    with open('Batch/submit_skeleton.sh','w') as skel:
      skel.write("universe = vanilla\n")
      skel.write("getenv   = True\n")
      skel.write("should_transfer_files = YES\n")
      skel.write("when_to_transfer_output = ON_EXIT\n")
      skel.write("request_memory = 24000\n")
      skel.write("request_cpus = 4\n")
  
  for i in range(0,NCARD):
  
    card = cards[i].strip('\n')
    if '#' in card: continue
    shortcard = card.split('/')[-1].replace(".root","").replace(".txt","").replace("card_","") # Run2_EE_Ext_M500_syst
    this_mass = "0" if "Weinberg" in shortcard else shortcard.split('_M')[-1].split('_')[0]
 
    if args.pdf:
      this_shortcard = shortcard+"_DefMod" if ((float(this_mass) > 3000.) or "SSWW" in shortcard) else shortcard
      if args.Impact:
        os.chdir(pwd+"/"+WP+"/"+shortcard+'/'+this_check+'/'+AsimovName)
        os.system("pdfseparate Impact_"+this_shortcard+"_"+AsimovName+".pdf -f 1 -l 1 Impact_"+this_shortcard+"_"+AsimovName+"_1.pdf")
        os.system("cp Impact_"+this_shortcard+"_"+AsimovName+"_1.pdf "+pwd+"/"+this_check+"/"+WP+"/"+AsimovName+"/Impact_"+this_shortcard+"_"+AsimovName+".pdf")
        os.chdir(pwd+"/"+this_check+"/"+WP+"/"+AsimovName)
        os.system("pdftoppm -png -singlefile Impact_"+this_shortcard+"_"+AsimovName+".pdf Impact_"+this_shortcard+"_"+AsimovName)
        os.chdir(pwd)
      if args.MDfit:
        os.chdir(pwd+"/"+WP+"/"+shortcard+'/'+this_check+'/'+AsimovName)
        os.system("cp MDfit_"+this_shortcard+"_"+AsimovName+".pdf MDfit_"+this_shortcard+"_"+AsimovName+".png "+pwd+"/"+this_check+"/"+WP+"/"+AsimovName)
        os.chdir(pwd)
      if args.FitDiag:
        os.chdir(pwd+"/"+WP+"/"+shortcard+'/'+this_check+'/'+AsimovName)
        os.system("cp pulls_"+this_shortcard+"_"+AsimovName+".txt "+pwd+"/"+this_check+"/"+WP+"/"+AsimovName)
        os.chdir(pwd)
      if args.Breakdown:
        os.chdir(pwd+"/"+WP+"/"+shortcard+'/'+this_check+'/'+AsimovName)
        os.system("cp "+this_shortcard+"_"+AsimovName+"_breakdown.pdf "+this_shortcard+"_"+AsimovName+"_breakdown.png "+pwd+"/"+this_check+"/"+WP+"/"+AsimovName)
        os.chdir(pwd)
      continue
    elif args.Work:
      os.system('mkdir -p '+WP+'/'+shortcard)
      os.system('cp '+WP+'/submit_skeleton.sh '+WP+'/'+shortcard+'/submit_Workspace.sh')
    elif IsNuis:
      os.system('mkdir -p '+WP+'/'+shortcard+'/'+this_check+'/'+AsimovName)
      os.system('cp '+WP+'/submit_skeleton.sh '+WP+'/'+shortcard+'/'+this_check+'/'+AsimovName+'/submit_'+this_check+'_'+AsimovName+'.sh')
      os.system('cp '+WP+'/'+shortcard+'/'+shortcard+'.root '+WP+'/'+shortcard+'/'+this_check+'/'+AsimovName)
      os.system('cp '+WP+'/'+shortcard+'/'+shortcard+'_DefMod.root '+WP+'/'+shortcard+'/'+this_check+'/'+AsimovName)
    else:
      os.system('mkdir -p Batch/'+WP+'/full_CLs/'+shortcard+'/output/')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/full_CLs/'+shortcard+'/submit_Q1.sh')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/full_CLs/'+shortcard+'/submit_Q2.sh')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/full_CLs/'+shortcard+'/submit_Q3.sh')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/full_CLs/'+shortcard+'/submit_Q4.sh')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/full_CLs/'+shortcard+'/submit_Q5.sh')
      os.system('mkdir -p Batch/'+WP+'/Asymptotic/'+shortcard+'/output/')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/Asymptotic/'+shortcard+'/submit_Asymptotic.sh')
  
    if args.Full or args.Q1:
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/run_Q1.sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("combine -M HybridNew --LHCmode LHC-limits "+card+" -n "+shortcard+" --saveHybridResult --expectedFromGrid 0.025 -T "+args.Ntoy+"\n")
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/submit_Q1.sh",'a') as submitfile:
        submitfile.write("executable = run_Q1.sh\n")
        submitfile.write("log = "+shortcard+"_Q1.log\n")
        submitfile.write("output = "+shortcard+"_Q1.out\n")
        submitfile.write("error = "+shortcard+"_Q1.out\n")
        submitfile.write("transfer_output_files = higgsCombine"+shortcard+".HybridNew.mH120.quant0.025.root\n")
        submitfile.write("transfer_output_remaps = \"higgsCombine"+shortcard+".HybridNew.mH120.quant0.025.root = output/"+shortcard+"_Q1.root\"\n")
        submitfile.write("queue\n")
      os.chdir('Batch/'+WP+'/full_CLs/'+shortcard)
      os.system('condor_submit submit_Q1.sh -batch-name '+shortcard+'_'+WP+'_Q1')
      os.chdir(pwd)
    if args.Full or args.Q2:
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/run_Q2.sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("combine -M HybridNew --LHCmode LHC-limits "+card+" -n "+shortcard+" --saveHybridResult --expectedFromGrid 0.160 -T "+args.Ntoy+"\n")
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/submit_Q2.sh",'a') as submitfile:
        submitfile.write("executable = run_Q2.sh\n")
        submitfile.write("log = "+shortcard+"_Q2.log\n")
        submitfile.write("output = "+shortcard+"_Q2.out\n")
        submitfile.write("error = "+shortcard+"_Q2.out\n")
        submitfile.write("transfer_output_files = higgsCombine"+shortcard+".HybridNew.mH120.quant0.160.root\n")
        submitfile.write("transfer_output_remaps = \"higgsCombine"+shortcard+".HybridNew.mH120.quant0.160.root = output/"+shortcard+"_Q2.root\"\n")
        submitfile.write("queue\n")
      os.chdir('Batch/'+WP+'/full_CLs/'+shortcard)
      os.system('condor_submit submit_Q2.sh -batch-name '+shortcard+'_'+WP+'_Q2')
      os.chdir(pwd)
    if args.Full or args.Q3:
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/run_Q3.sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("combine -M HybridNew --LHCmode LHC-limits "+card+" -n "+shortcard+" --saveHybridResult --expectedFromGrid 0.500 -T "+args.Ntoy+"\n")
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/submit_Q3.sh",'a') as submitfile:
        submitfile.write("executable = run_Q3.sh\n")
        submitfile.write("log = "+shortcard+"_Q3.log\n")
        submitfile.write("output = "+shortcard+"_Q3.out\n")
        submitfile.write("error = "+shortcard+"_Q3.out\n")
        submitfile.write("transfer_output_files = higgsCombine"+shortcard+".HybridNew.mH120.quant0.500.root\n")
        submitfile.write("transfer_output_remaps = \"higgsCombine"+shortcard+".HybridNew.mH120.quant0.500.root = output/"+shortcard+"_Q3.root\"\n")
        submitfile.write("queue\n")
      os.chdir('Batch/'+WP+'/full_CLs/'+shortcard)
      os.system('condor_submit submit_Q3.sh -batch-name '+shortcard+'_'+WP+'_Q3')
      os.chdir(pwd)
    if args.Full or args.Q4:
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/run_Q4.sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("combine -M HybridNew --LHCmode LHC-limits "+card+" -n "+shortcard+" --saveHybridResult --expectedFromGrid 0.840 -T "+args.Ntoy+"\n")
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/submit_Q4.sh",'a') as submitfile:
        submitfile.write("executable = run_Q4.sh\n")
        submitfile.write("log = "+shortcard+"_Q4.log\n")
        submitfile.write("output = "+shortcard+"_Q4.out\n")
        submitfile.write("error = "+shortcard+"_Q4.out\n")
        submitfile.write("transfer_output_files = higgsCombine"+shortcard+".HybridNew.mH120.quant0.840.root\n")
        submitfile.write("transfer_output_remaps = \"higgsCombine"+shortcard+".HybridNew.mH120.quant0.840.root = output/"+shortcard+"_Q4.root\"\n")
        submitfile.write("queue\n")
      os.chdir('Batch/'+WP+'/full_CLs/'+shortcard)
      os.system('condor_submit submit_Q4.sh -batch-name '+shortcard+'_'+WP+'_Q4')
      os.chdir(pwd)
    if args.Full or args.Q5:
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/run_Q5.sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("combine -M HybridNew --LHCmode LHC-limits "+card+" -n "+shortcard+" --saveHybridResult --expectedFromGrid 0.975 -T "+args.Ntoy+"\n")
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/submit_Q5.sh",'a') as submitfile:
        submitfile.write("executable = run_Q5.sh\n")
        submitfile.write("log = "+shortcard+"_Q5.log\n")
        submitfile.write("output = "+shortcard+"_Q5.out\n")
        submitfile.write("error = "+shortcard+"_Q5.out\n")
        submitfile.write("transfer_output_files = higgsCombine"+shortcard+".HybridNew.mH120.quant0.975.root\n")
        submitfile.write("transfer_output_remaps = \"higgsCombine"+shortcard+".HybridNew.mH120.quant0.975.root = output/"+shortcard+"_Q5.root\"\n")
        submitfile.write("queue\n")
      os.chdir('Batch/'+WP+'/full_CLs/'+shortcard)
      os.system('condor_submit submit_Q5.sh -batch-name '+shortcard+'_'+WP+'_Q5')
      os.chdir(pwd)
  
    if args.Asymptotic:
      with open("Batch/"+WP+"/Asymptotic/"+shortcard+"/run_Asymptotic.sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("combine -M AsymptoticLimits "+card+" --run blind\n")
      with open("Batch/"+WP+"/Asymptotic/"+shortcard+"/submit_Asymptotic.sh",'a') as submitfile:
        submitfile.write("executable = run_Asymptotic.sh\n")
        submitfile.write("log = "+shortcard+"_Asymptotic.log\n")
        submitfile.write("output = "+shortcard+"_Asymptotic.out\n")
        submitfile.write("error = "+shortcard+"_Asymptotic.out\n")
        submitfile.write("transfer_output_files = higgsCombineTest.AsymptoticLimits.mH120.root\n")
        submitfile.write("transfer_output_remaps = \"higgsCombineTest.AsymptoticLimits.mH120.root = output/"+shortcard+"_Asymptotic.root\"\n")
        submitfile.write("queue\n")
      os.chdir('Batch/'+WP+'/Asymptotic/'+shortcard)
      os.system('condor_submit -a "priority = -15" submit_Asymptotic.sh -batch-name '+shortcard+'_'+WP+'_Asymptotic')
      os.chdir(pwd)

    if args.Work:
      with open(WP+"/"+shortcard+"/MakeWorkspace.sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("pushd "+pwd+"/"+WP+"/"+shortcard+"\n")
        runfile.write("echo Setting cmsenv environment...\n")
        runfile.write("cmsenv\n")
        card = card.replace(".root",".txt") # The Runlist contains card_name.root by default.
        if "EMu" in shortcard:
          runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_EMu "+card+" --channel-masks -o "+shortcard+".root\n")
        else:
          runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel "+card+" --channel-masks -o "+shortcard+".root\n")
        if (float(this_mass) > 3000.) or "SSWW" in shortcard: # mass is above 3000 GeV so it only contains SSWW, or SSWW only --> add DefMod for impact check
          runfile.write("text2workspace.py "+card+" --channel-masks -o "+shortcard+"_DefMod.root\n") # impact with default physics model with SSWW: see https://cms-talk.web.cern.ch/t/0-impact-on-poi-negative-bin-issue/42793
      with open(WP+"/"+shortcard+"/submit_Workspace.sh",'a') as submitfile:
        submitfile.write("executable = MakeWorkspace.sh\n")
        submitfile.write("log = "+shortcard+"_Workspace.log\n")
        submitfile.write("output = "+shortcard+"_Workspace.out\n")
        submitfile.write("error = "+shortcard+"_Workspace.out\n")
        submitfile.write("queue\n")
      os.chdir(WP+"/"+shortcard)
      os.system('condor_submit -a "priority = -15" submit_Workspace.sh -batch-name '+shortcard+'_'+WP+'_Workspace')
      os.chdir(pwd)

    if IsNuis:
      list_shortcard = [shortcard, shortcard+"_DefMod"] if ((float(this_mass) > 3000.) or "SSWW" in shortcard) else [shortcard]
      with open(WP+"/"+shortcard+"/"+this_check+"/"+AsimovName+"/Run"+this_check+"_"+AsimovName+".sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("pushd "+pwd+"/"+WP+"/"+shortcard+"/"+this_check+"/"+AsimovName+"\n")
        runfile.write("echo Setting cmsenv environment...\n")
        runfile.write("cmsenv\n")

        for this_shortcard in list_shortcard:
          if args.FitDiag:
            if "DefMod" in this_shortcard: continue # Must use the actual physics model
            runfile.write("echo Running FitDiagnostics...\n") # Asimov set as default; FIXME later to choose whether Asimov or not
            runfile.write("combine -M FitDiagnostics "+this_shortcard+".root --rMin -10 --rMax 10 --saveShapes --saveWithUncertainties --saveNormalizations --saveWorkspace -n _"+this_shortcard+" --plots"+AsimovSetting+"\n")
            runfile.write("python3 $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit/test/diffNuisances.py -a fitDiagnostics_"+this_shortcard+".root > pulls_"+this_shortcard+".txt\n")
          elif args.GOF:
            if "DefMod" in this_shortcard: continue # Must use the actual physics model
            runfile.write("echo Running the goodness of fit test...\n")
            runfile.write("combine -M GoodnessOfFit "+this_shortcard+".root -t -1 --algo saturated -n gof_Asimov_"+this_shortcard+"\n")
            runfile.write("combine -M GoodnessOfFit "+this_shortcard+".root -t "+args.Ntoy+" --algo saturated -n gof_Ntoy"+args.Ntoy+"_"+this_shortcard+"\n")
            runfile.write("combine -M GoodnessOfFit "+this_shortcard+".root --algo saturated --setParameters mask_year16a_sr1=1,mask_year16a_sr2=1,mask_year16a_sr3=1,mask_year16b_sr1=1,mask_year16b_sr2=1,mask_year16b_sr3=1,mask_year17_sr1=1,mask_year17_sr2=1,mask_year17_sr3=1,mask_year18_sr1=1,mask_year18_sr2=1,mask_year18_sr3=1,r=0 --freezeParameters mask_year16a_sr1,mask_year16a_sr2,mask_year16a_sr3,mask_year16b_sr1,mask_year16b_sr2,mask_year16b_sr3,mask_year17_sr1,mask_year17_sr2,mask_year17_sr3,mask_year18_sr1,mask_year18_sr2,mask_year18_sr3,r -n gof_CRonly_obs_"+this_shortcard+"\n")
            runfile.write("combine -M GoodnessOfFit "+this_shortcard+".root -t "+args.Ntoy+" --algo saturated --setParameters mask_year16a_sr1=1,mask_year16a_sr2=1,mask_year16a_sr3=1,mask_year16b_sr1=1,mask_year16b_sr2=1,mask_year16b_sr3=1,mask_year17_sr1=1,mask_year17_sr2=1,mask_year17_sr3=1,mask_year18_sr1=1,mask_year18_sr2=1,mask_year18_sr3=1,r=0 --freezeParameters mask_year16a_sr1,mask_year16a_sr2,mask_year16a_sr3,mask_year16b_sr1,mask_year16b_sr2,mask_year16b_sr3,mask_year17_sr1,mask_year17_sr2,mask_year17_sr3,mask_year18_sr1,mask_year18_sr2,mask_year18_sr3,r -n gof_CRonly_toys_Ntoy"+args.Ntoy+"_"+this_shortcard+"\n")
            runfile.write("combine -M GoodnessOfFit "+this_shortcard+".root -t "+args.Ntoy+" --algo saturated --setParameters mask_year16a_sr1=1,mask_year16a_sr2=1,mask_year16a_sr3=1,mask_year16b_sr1=1,mask_year16b_sr2=1,mask_year16b_sr3=1,mask_year17_sr1=1,mask_year17_sr2=1,mask_year17_sr3=1,mask_year18_sr1=1,mask_year18_sr2=1,mask_year18_sr3=1,r=0 --freezeParameters mask_year16a_sr1,mask_year16a_sr2,mask_year16a_sr3,mask_year16b_sr1,mask_year16b_sr2,mask_year16b_sr3,mask_year17_sr1,mask_year17_sr2,mask_year17_sr3,mask_year18_sr1,mask_year18_sr2,mask_year18_sr3,r --toysFrequentist -n gof_CRonly_toysFreq_Ntoy"+args.Ntoy+"_"+this_shortcard+"\n")
          elif args.Impact:
            if (float(this_mass) > 3000.):
              runfile.write("combineTool.py -M Impacts -d "+this_shortcard+".root -m "+this_mass+" --rMin -100 --rMax 100 --robustFit 1 --doInitialFit --name Impact_"+this_shortcard+"_"+AsimovName+AsimovSetting+"\n")
              runfile.write("combineTool.py -M Impacts -d "+this_shortcard+".root -m "+this_mass+" --rMin -100 --rMax 100 --robustFit 1 --doFits --name Impact_"+this_shortcard+"_"+AsimovName+AsimovSetting+"\n")
            else:
              runfile.write("combineTool.py -M Impacts -d "+this_shortcard+".root -m "+this_mass+" --rMin -10 --rMax 10 --robustFit 1 --doInitialFit --name Impact_"+this_shortcard+"_"+AsimovName+AsimovSetting+"\n")
              runfile.write("combineTool.py -M Impacts -d "+this_shortcard+".root -m "+this_mass+" --rMin -10 --rMax 10 --robustFit 1 --doFits --name Impact_"+this_shortcard+"_"+AsimovName+AsimovSetting+"\n")
            runfile.write("combineTool.py -M Impacts -d "+this_shortcard+".root -m "+this_mass+" --output "+this_shortcard+"_"+AsimovName+"_impacts.json --name Impact_"+this_shortcard+"_"+AsimovName+"\n")
            runfile.write("plotImpacts.py -i "+this_shortcard+"_"+AsimovName+"_impacts.json -o Impact_"+this_shortcard+"_"+AsimovName+"\n")
          elif args.FastScan:
            runfile.write("combineTool.py -M FastScan -w "+this_shortcard+".root:w -o "+this_shortcard+"_Asimov_nll"+AsimovSetting+"\n")
            runfile.write("combineTool.py -M FastScan -w "+this_shortcard+".root:w -o "+this_shortcard+"_nll\n")
          elif args.MDfit:
            runfile.write("combineTool.py -M MultiDimFit "+this_shortcard+".root --algo grid --points=41 --rMin -1 --rMax 1 --alignEdges 1"+AsimovSetting+" --name ."+this_shortcard+"_"+AsimovName+"_rRange1\n")
            runfile.write("combineTool.py -M MultiDimFit "+this_shortcard+".root --algo grid --points=41 --rMin -10 --rMax 10 --alignEdges 1"+AsimovSetting+" --name ."+this_shortcard+"_"+AsimovName+"_rRange10\n")
            runfile.write("combineTool.py -M MultiDimFit "+this_shortcard+".root --algo grid --points=401 --rMin -100 --rMax 100 --alignEdges 1"+AsimovSetting+" --name ."+this_shortcard+"_"+AsimovName+"_rRange100\n")
            runfile.write("plot1DScan.py higgsCombine."+this_shortcard+"_"+AsimovName+"_rRange1.MultiDimFit.mH120.root -o MDfit_rRange1_"+this_shortcard+"_"+AsimovName+"\n")
            runfile.write("plot1DScan.py higgsCombine."+this_shortcard+"_"+AsimovName+"_rRange10.MultiDimFit.mH120.root -o MDfit_rRange10_"+this_shortcard+"_"+AsimovName+"\n")
            runfile.write("plot1DScan.py higgsCombine."+this_shortcard+"_"+AsimovName+"_rRange100.MultiDimFit.mH120.root -o MDfit_rRange100_"+this_shortcard+"_"+AsimovName+"\n")
          elif args.Breakdown:
            runfile.write("combine -M MultiDimFit "+this_shortcard+"_"+AsimovName+".root --points=21 --rMin -5 --rMax 5 --alignEdges 1"+AsimovSetting+" --saveWorkspace -n ."+this_shortcard+"_"+AsimovName+"_saveWorkspace\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_total\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_jet\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec\n")
            if "EE" in this_shortcard+"_"+AsimovName: runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,cf"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec_cf\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+this_shortcard+"_"+AsimovName+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeParameters allConstrainedNuisances"+AsimovSetting+" -n ."+this_shortcard+"_"+AsimovName+"_freeze_all\n")
            if "EE" not in this_shortcard+"_"+AsimovName:
              runfile.write("plot1DScan.py higgsCombine."+this_shortcard+"_"+AsimovName+"_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory.MultiDimFit.mH120.root:\"jet+theory\":5 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake.MultiDimFit.mH120.root:\"jet+theory+fake\":6 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep.MultiDimFit.mH120.root:\"jet+theory+fake+lep\":7 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup\":8 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi\":9 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag\":10 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire\":11 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met\":12 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec\":13 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_all.MultiDimFit.mH120.root:\"stat\":14 --output "+this_shortcard+"_"+AsimovName+"_breakdown --y-max 10 --y-cut 40 --breakdown \"jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,rest,stat\"\n")
            else:
              runfile.write("plot1DScan.py higgsCombine."+this_shortcard+"_"+AsimovName+"_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory.MultiDimFit.mH120.root:\"jet+theory\":5 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake.MultiDimFit.mH120.root:\"jet+theory+fake\":6 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep.MultiDimFit.mH120.root:\"jet+theory+fake+lep\":7 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup\":8 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi\":9 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag\":10 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire\":11 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met\":12 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec\":13 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec_cf.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec+cf\":14 higgsCombine."+this_shortcard+"_"+AsimovName+"_freeze_all.MultiDimFit.mH120.root:\"stat\":15 --output "+this_shortcard+"_"+AsimovName+"_breakdown --y-max 10 --y-cut 40 --breakdown \"jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,cf,rest,stat\"\n")

        runfile.write("echo Done.\n")

      with open(WP+"/"+shortcard+"/"+this_check+"/"+AsimovName+"/submit_"+this_check+"_"+AsimovName+".sh",'a') as submitfile:
        submitfile.write("executable = Run"+this_check+"_"+AsimovName+".sh\n")
        submitfile.write("log = "+shortcard+"_Run"+this_check+"_"+AsimovName+".log\n")
        submitfile.write("output = "+shortcard+"_Run"+this_check+"_"+AsimovName+".out\n")
        submitfile.write("error = "+shortcard+"_Run"+this_check+"_"+AsimovName+".out\n")
        submitfile.write("queue\n")
      os.chdir(WP+"/"+shortcard+"/"+this_check+"/"+AsimovName)
      os.system('condor_submit -a "priority = -15" submit_'+this_check+'_'+AsimovName+'.sh -batch-name '+shortcard+'_'+WP+'_'+this_check+'_'+AsimovName)
      os.chdir(pwd)
