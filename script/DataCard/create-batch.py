# Place it at CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter
# python create-batch.py -l RunList1.txt [RunList2.txt] --Asymptotic[--Full][--Q*]
# RunList.txt contains paths of results from text2workspace.py e.g. /data6/Users/jihkim/CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter/Workspace/card_2017_MuMu_M500_HNL_UL.root

import os, sys
import commands as cmd
import argparse
import datetime

parser = argparse.ArgumentParser(description='option')
parser.add_argument('--pdf', action='store_true', help='do pdfseparate')
parser.add_argument('-i', dest='Input', help='take a single argument. [NOTE] feed realpath of a card (or workspace) !!')
parser.add_argument('-l', dest='RunLists', nargs='+', help='take args as a list, return error when there is no arg')
parser.add_argument('--Full', action='store_true')
parser.add_argument('--Q1', action='store_true')
parser.add_argument('--Q2', action='store_true')
parser.add_argument('--Q3', action='store_true')
parser.add_argument('--Q4', action='store_true')
parser.add_argument('--Q5', action='store_true')
parser.add_argument('-t', dest='Ntoy', default='1000', help='N of toys when running full CLs')
parser.add_argument('--Asymptotic', action='store_true')
parser.add_argument('--Work', action='store_true', help='running workspace')
parser.add_argument('--Nuis', action='store_true', help='check nuisance fit')
args = parser.parse_args()

pwd = os.getcwd()
CMSSW_BASE = os.environ['CMSSW_BASE']
SCRAM_ARCH = os.environ['SCRAM_ARCH']

failure, result = cmd.getstatusoutput('combine --help')
if failure:
  print "[!!ERROR!!] cannot run combine."
  print "Please set proper cmsenv first."
  print "Exiting ..."
  sys.exit(1)

if args.Input is not None:
  args.RunLists = []
  args.RunLists.append(args.Input)

for RunList in args.RunLists:
  cards = open(RunList).readlines() if args.Input is None else [args.Input]
  NCARD = len(cards)
  WP = RunList.split('.')[-2].replace('RunList_','').replace('Run2_','') if args.Input is None else args.Input.split('/')[-2] # Currently, RunList is splitted into Run2 and normal setting (code structure issue -- it doesn't change WP)

  if args.pdf:
    os.system('mkdir -p Impacts/'+WP)
  elif args.Work or args.Nuis:
    with open(WP+'/submit_skeleton.sh','w') as skel: # for Nuisance check
      skel.write("universe = vanilla\n")
      skel.write("+SingularityImage = \"/cvmfs/singularity.opensciencegrid.org/opensciencegrid/osgvo-el9:latest\"\n")
      skel.write("should_transfer_files = YES\n")
      skel.write("when_to_transfer_output = ON_EXIT\n")
      skel.write("request_memory = 24000\n")
  else:
    os.system('mkdir -p Batch/'+WP)
    with open('Batch/submit_skeleton.sh','w') as skel:
      skel.write("universe = vanilla\n")
      skel.write("getenv   = True\n")
      skel.write("should_transfer_files = YES\n")
      skel.write("when_to_transfer_output = ON_EXIT\n")
      skel.write("request_memory = 24000\n")
  
  for i in range(0,NCARD):
  
    card = cards[i].strip('\n')
    if '#' in card: continue
    shortcard = card.split('/')[-1].replace(".root","").replace(".txt","").replace("card_","")
 
    if args.pdf:
      os.chdir(pwd+"/"+WP+"/"+shortcard)
      os.system("pdfseparate "+shortcard+".pdf -f 1 -l 1 "+shortcard+"_1.pdf") # this doesn't work in singularity environment.................
      os.system("cp "+shortcard+"_1.pdf "+pwd+"/Impacts/"+WP+"/Impact_"+shortcard+".pdf")
      if float(shortcard.split('_')[2].replace("M","")) > 3000.: # mass is above 3000 GeV so that it only contains SSWW --> get impact with default physics model
        os.system("pdfseparate "+shortcard+"_DefMod.pdf -f 1 -l 1 "+shortcard+"_DefMod_1.pdf")
        os.system("cp "+shortcard+"_DefMod_1.pdf "+pwd+"/Impacts/"+WP+"/Impact_"+shortcard+".pdf")
      os.chdir(pwd)
      continue
    elif args.Work:
      os.system('mkdir -p '+WP+'/'+shortcard)
      os.system('cp '+WP+'/submit_skeleton.sh '+WP+'/'+shortcard+'/submit_Workspace.sh')
    elif args.Nuis:
      os.system('mkdir -p '+WP+'/'+shortcard)
      os.system('cp '+WP+'/submit_skeleton.sh '+WP+'/'+shortcard+'/submit_Nuisance.sh')
    else:
      os.system('mkdir -p Batch/'+WP+'/full_CLs/'+shortcard+'/output/')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/full_CLs/'+shortcard+'/submit_Q1.sh')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/full_CLs/'+shortcard+'/submit_Q2.sh')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/full_CLs/'+shortcard+'/submit_Q3.sh')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/full_CLs/'+shortcard+'/submit_Q4.sh')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/full_CLs/'+shortcard+'/submit_Q5.sh')
      os.system('mkdir -p Batch/'+WP+'/Asymptotic/'+shortcard+'/output/')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/Asymptotic/'+shortcard+'/submit_Asympt.sh')
  
    if args.Full or args.Q1:
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/run_Q1.sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("combine -M HybridNew --LHCmode LHC-limits "+card+" -n "+shortcard+" --saveHybridResult --expectedFromGrid 0.025 -T "+args.Ntoy+"\n")
      with open("Batch/"+WP+"/full_CLs/"+shortcard+"/submit_Q1.sh",'a') as submitfile:
        submitfile.write("executable = run_Q1.sh\n")
        submitfile.write("log = "+shortcard+"_Q1.log\n")
        submitfile.write("output = "+shortcard+"_Q1.out\n")
        submitfile.write("error = "+shortcard+"_Q1.err\n")
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
        submitfile.write("error = "+shortcard+"_Q2.err\n")
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
        submitfile.write("error = "+shortcard+"_Q3.err\n")
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
        submitfile.write("error = "+shortcard+"_Q4.err\n")
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
        submitfile.write("error = "+shortcard+"_Q5.err\n")
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
      with open("Batch/"+WP+"/Asymptotic/"+shortcard+"/submit_Asympt.sh",'a') as submitfile:
        submitfile.write("executable = run_Asymptotic.sh\n")
        submitfile.write("log = "+shortcard+"_Asymptotic.log\n")
        submitfile.write("output = "+shortcard+"_Asymptotic.out\n")
        submitfile.write("error = "+shortcard+"_Asymptotic.err\n")
        submitfile.write("transfer_output_files = higgsCombineTest.AsymptoticLimits.mH120.root\n")
        submitfile.write("transfer_output_remaps = \"higgsCombineTest.AsymptoticLimits.mH120.root = output/"+shortcard+"_Asymptotic.root\"\n")
        submitfile.write("queue\n")
      os.chdir('Batch/'+WP+'/Asymptotic/'+shortcard)
      os.system('condor_submit submit_Asympt.sh -batch-name '+shortcard+'_'+WP+'_Asymptotic')
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
          runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_EMu "+card+" -o "+shortcard+".root\n")
        else:
          runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel "+card+" -o "+shortcard+".root\n")
        if float(shortcard.split('_')[2].replace("M","")) > 3000.: # mass is above 3000 GeV so that it only contains SSWW
          runfile.write("text2workspace.py "+card+" -o "+shortcard+"_DefMod.root\n") # check the impact with default physics model with SSWW: see https://cms-talk.web.cern.ch/t/0-impact-on-poi-negative-bin-issue/42793
      with open(WP+"/"+shortcard+"/submit_Workspace.sh",'a') as submitfile:
        submitfile.write("executable = MakeWorkspace.sh\n")
        submitfile.write("log = "+shortcard+"_Workspace.log\n")
        submitfile.write("output = "+shortcard+"_Workspace.out\n")
        submitfile.write("error = "+shortcard+"_Workspace.out\n")
        #submitfile.write("error = "+shortcard+"_Workspace.err\n")
        submitfile.write("queue\n")
      os.chdir(WP+"/"+shortcard)
      os.system('condor_submit submit_Workspace.sh -batch-name '+shortcard+'_'+WP+'_Workspace')
      os.chdir(pwd)

    if args.Nuis:
      this_mass = shortcard.split('_M')[-1].split('_')[0]
      with open(WP+"/"+shortcard+"/CheckNuisance.sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("pushd "+pwd+"/"+WP+"/"+shortcard+"\n")
        runfile.write("echo Setting cmsenv environment...\n")
        runfile.write("cmsenv\n")
        runfile.write("echo Running FitDiagnostics...\n") # Asimov set as default; FIXME later to choose Asimov or not
        runfile.write("combine -M FitDiagnostics "+shortcard+".root --rMin -10 --rMax 10 --saveShapes --saveWithUncertainties -n "+shortcard+" --plots -t -1\n")
        runfile.write("echo Running Initial fit...\n")
        runfile.write("combineTool.py -M Impacts -d "+shortcard+".root -m "+this_mass+" --rMin -10 --rMax 10 --robustFit 1 --doInitialFit --name "+shortcard+" -t -1\n")
        runfile.write("echo Running Actual fit...\n")
        runfile.write("combineTool.py -M Impacts -d "+shortcard+".root -m "+this_mass+" --rMin -10 --rMax 10 --robustFit 1 --doFits --name "+shortcard+" -t -1\n")
        runfile.write("echo Making impact json...\n")
        runfile.write("combineTool.py -M Impacts -d "+shortcard+".root -m "+this_mass+" --rMin -10 --rMax 10 --robustFit 1 --output "+shortcard+"_impacts.json --name "+shortcard+"\n")
        runfile.write("echo Making impact plots...\n")
        runfile.write("plotImpacts.py -i "+shortcard+"_impacts.json -o "+shortcard+"\n")
        #runfile.write("pdfseparate "+shortcard+".pdf -f 1 -l 1 "+shortcard+"_1.pdf\n") # this doesn't work in singularity environment.................
        runfile.write("echo Running fast scan...\n")
        runfile.write("combineTool.py -M FastScan -w "+shortcard+".root:w -o "+shortcard+"_Asimov_nll -t -1\n")
        runfile.write("combineTool.py -M FastScan -w "+shortcard+".root:w -o "+shortcard+"_nll\n")
        runfile.write("echo Running multi-dimensional fit...\n")
        runfile.write("combineTool.py -M MultiDimFit "+shortcard+".root --algo grid --points=200 --rMin -10 --rMax 10 --alignEdges 1 -t -1 --name ."+shortcard+"\n")
        runfile.write("combineTool.py -M MultiDimFit "+shortcard+".root --algo grid --points=1000 --rMin -100 --rMax 100 --alignEdges 1 -t -1 --name ."+shortcard+"_rRange100\n")
        runfile.write("echo Summarizing the result into 1D...\n")
        runfile.write("plot1DScan.py higgsCombine."+shortcard+".MultiDimFit.mH120.root -o "+shortcard+"_MDfit\n")
        runfile.write("plot1DScan.py higgsCombine."+shortcard+"_rRange100.MultiDimFit.mH120.root -o "+shortcard+"_rRange100_MDfit\n")
        if float(shortcard.split('_')[2].replace("M","")) > 3000.: # mass is above 3000 GeV so that it only contains SSWW --> get impact with default physics model
          runfile.write("combineTool.py -M Impacts -d "+shortcard+"_DefMod.root -m "+this_mass+" --rMin -100 --rMax 100 --robustFit 1 --doInitialFit --name "+shortcard+"_DefMod -t -1\n")
          runfile.write("combineTool.py -M Impacts -d "+shortcard+"_DefMod.root -m "+this_mass+" --rMin -100 --rMax 100 --robustFit 1 --doFits --name "+shortcard+"_DefMod -t -1\n")
          runfile.write("combineTool.py -M Impacts -d "+shortcard+"_DefMod.root -m "+this_mass+" --rMin -100 --rMax 100 --robustFit 1 --output "+shortcard+"_DefMod_impacts.json --name "+shortcard+"_DefMod\n")
          runfile.write("plotImpacts.py -i "+shortcard+"_DefMod_impacts.json -o "+shortcard+"_DefMod\n")
          #runfile.write("pdfseparate "+shortcard+"_DefMod.pdf -f 1 -l 1 "+shortcard+"_DefMod_1.pdf\n")
          runfile.write("combineTool.py -M FastScan -w "+shortcard+"_DefMod.root:w -o "+shortcard+"_DefMod_Asimov_nll -t -1\n")
          runfile.write("combineTool.py -M FastScan -w "+shortcard+"_DefMod.root:w -o "+shortcard+"_DefMod_nll\n")
          runfile.write("combineTool.py -M MultiDimFit "+shortcard+"_DefMod.root --algo grid --points=200 --rMin -10 --rMax 10 --alignEdges 1 -t -1 --name ."+shortcard+"_DefMod\n")
          runfile.write("combineTool.py -M MultiDimFit "+shortcard+"_DefMod.root --algo grid --points=1000 --rMin -100 --rMax 100 --alignEdges 1 -t -1 --name ."+shortcard+"_DefMod_rRange100\n")
          runfile.write("plot1DScan.py higgsCombine."+shortcard+"_DefMod.MultiDimFit.mH120.root -o "+shortcard+"_DefMod_MDfit\n")
          runfile.write("plot1DScan.py higgsCombine."+shortcard+"_DefMod_rRange100.MultiDimFit.mH120.root -o "+shortcard+"_DefMod_rRange100_MDfit\n")
        runfile.write("echo Done.\n")
      with open(WP+"/"+shortcard+"/submit_Nuisance.sh",'a') as submitfile:
        submitfile.write("executable = CheckNuisance.sh\n")
        submitfile.write("log = "+shortcard+"_CheckNuisance.log\n")
        submitfile.write("output = "+shortcard+"_CheckNuisance.out\n")
        submitfile.write("error = "+shortcard+"_CheckNuisance.out\n")
        #submitfile.write("error = "+shortcard+"_CheckNuisance.err\n")
        submitfile.write("queue\n")
      os.chdir(WP+"/"+shortcard)
      os.system('condor_submit submit_Nuisance.sh -batch-name '+shortcard+'_'+WP+'_Nuisance')
      os.chdir(pwd)
