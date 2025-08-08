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
parser.add_argument('-t', dest='Ntoy', default='1000', help='N of toys when running full CLs')
parser.add_argument('--Asymptotic', action='store_true')
parser.add_argument('--Work', action='store_true', help='create workspace')
parser.add_argument('--FitDiag', action='store_true', help='check nuisance fit')
parser.add_argument('--Impact', action='store_true', help='check impacts')
parser.add_argument('--Breakdown', action='store_true', help='uncertainty breakdown')
args = parser.parse_args()

IsNuis = False
Ncheck = 0
for check in ['FitDiag','Impact','Breakdown']:
  if vars(args)[check] is True:
    this_check = check
    IsNuis = True
    Ncheck += 1
if Ncheck > 1:
  print("More than 1 Nuisance flag activated; This is not supported.")
  print("Exiting ...")
  sys.exit(1)

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
    os.system('mkdir -p Impacts/'+WP)
  elif args.Work or IsNuis:
    with open(WP+'/submit_skeleton.sh','w') as skel:
      skel.write("universe = vanilla\n")
      skel.write("+SingularityImage = \"/cvmfs/singularity.opensciencegrid.org/opensciencegrid/osgvo-el9:latest\"\n")
      skel.write("should_transfer_files = YES\n")
      skel.write("when_to_transfer_output = ON_EXIT\n")
      skel.write("request_memory = 24000\n")
      skel.write("request_cpus = 4\n")
  else:
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
    shortcard = card.split('/')[-1].replace(".root","").replace(".txt","").replace("card_","")
 
    if args.pdf:
      os.chdir(pwd+"/"+WP+"/"+shortcard)
      if "Weinberg" not in shortcard.split('_')[2] and float(shortcard.split('_')[2].replace("M","")) <= 3000.:
        if "SSWW" in shortcard:
          os.system("pdfseparate "+shortcard+"_DefMod.pdf -f 1 -l 1 "+shortcard+"_DefMod_1.pdf")
          os.system("cp "+shortcard+"_DefMod_1.pdf "+pwd+"/Impacts/"+WP+"/Impact_"+shortcard+".pdf")
        else:
          os.system("pdfseparate "+shortcard+".pdf -f 1 -l 1 "+shortcard+"_1.pdf")
          os.system("cp "+shortcard+"_1.pdf "+pwd+"/Impacts/"+WP+"/Impact_"+shortcard+".pdf")
      else: # mass is above 3000 GeV or Weinberg so that it only contains SSWW --> get impact with default physics model
        os.system("pdfseparate "+shortcard+"_DefMod.pdf -f 1 -l 1 "+shortcard+"_DefMod_1.pdf")
        os.system("cp "+shortcard+"_DefMod_1.pdf "+pwd+"/Impacts/"+WP+"/Impact_"+shortcard+".pdf")
      os.chdir(pwd)
      continue
    elif args.Work:
      os.system('mkdir -p '+WP+'/'+shortcard)
      os.system('cp '+WP+'/submit_skeleton.sh '+WP+'/'+shortcard+'/submit_Workspace.sh')
    elif IsNuis:
      os.system('mkdir -p '+WP+'/'+shortcard)
      os.system('cp '+WP+'/submit_skeleton.sh '+WP+'/'+shortcard+'/submit_'+this_check+'.sh')
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
      with open("Batch/"+WP+"/Asymptotic/"+shortcard+"/submit_Asymptotic.sh",'a') as submitfile:
        submitfile.write("executable = run_Asymptotic.sh\n")
        submitfile.write("log = "+shortcard+"_Asymptotic.log\n")
        submitfile.write("output = "+shortcard+"_Asymptotic.out\n")
        submitfile.write("error = "+shortcard+"_Asymptotic.err\n")
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
        if "Weinberg" in shortcard:
          runfile.write("text2workspace.py "+card+" -o "+shortcard+".root\n") # For Weinberg, run with default physics model
        else:
          if "EMu" in shortcard:
            runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_EMu "+card+" -o "+shortcard+".root\n")
          else:
            runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel "+card+" -o "+shortcard+".root\n")
          if float(shortcard.split('_')[2].replace("M","")) > 3000. or "SSWW" in shortcard: # mass is above 3000 GeV so it only contains SSWW, or SSWW only --> add DefMod for impact check
            runfile.write("text2workspace.py "+card+" -o "+shortcard+"_DefMod.root\n") # impact with default physics model with SSWW: see https://cms-talk.web.cern.ch/t/0-impact-on-poi-negative-bin-issue/42793
      with open(WP+"/"+shortcard+"/submit_Workspace.sh",'a') as submitfile:
        submitfile.write("executable = MakeWorkspace.sh\n")
        submitfile.write("log = "+shortcard+"_Workspace.log\n")
        submitfile.write("output = "+shortcard+"_Workspace.out\n")
        submitfile.write("error = "+shortcard+"_Workspace.out\n")
        #submitfile.write("error = "+shortcard+"_Workspace.err\n")
        submitfile.write("queue\n")
      os.chdir(WP+"/"+shortcard)
      os.system('condor_submit -a "priority = -15" submit_Workspace.sh -batch-name '+shortcard+'_'+WP+'_Workspace')
      os.chdir(pwd)

    if IsNuis:
      this_mass = shortcard.split('_M')[-1].split('_')[0]
      with open(WP+"/"+shortcard+"/Run"+this_check+".sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("pushd "+pwd+"/"+WP+"/"+shortcard+"\n")
        runfile.write("echo Setting cmsenv environment...\n")
        runfile.write("cmsenv\n")
        if "Weinberg" not in shortcard.split('_')[2] and float(shortcard.split('_')[2].replace("M","")) <= 3000.:
          if "SSWW" in shortcard:
            if args.FitDiag:
              runfile.write("echo Running FitDiagnostics...\n") # Asimov set as default; FIXME later to choose Asimov or not
              runfile.write("combine -M FitDiagnostics "+shortcard+".root --rMin -10 --rMax 10 --saveShapes --saveWithUncertainties -n _"+shortcard+" --plots -t -1\n") # FIXME maybe shortcard+"_DefMod.root? No?
            elif args.Impact:
              runfile.write("combineTool.py -M Impacts -d "+shortcard+"_DefMod.root -m "+this_mass+" --rMin -100 --rMax 100 --robustFit 1 --doInitialFit --name "+shortcard+"_DefMod -t -1\n")
              runfile.write("combineTool.py -M Impacts -d "+shortcard+"_DefMod.root -m "+this_mass+" --rMin -100 --rMax 100 --robustFit 1 --doFits --name "+shortcard+"_DefMod -t -1\n")
              runfile.write("combineTool.py -M Impacts -d "+shortcard+"_DefMod.root -m "+this_mass+" --rMin -100 --rMax 100 --robustFit 1 --output "+shortcard+"_DefMod_impacts.json --name "+shortcard+"_DefMod\n")
              runfile.write("plotImpacts.py -i "+shortcard+"_DefMod_impacts.json -o "+shortcard+"_DefMod\n")
              #runfile.write("pdfseparate "+shortcard+"_DefMod.pdf -f 1 -l 1 "+shortcard+"_DefMod_1.pdf\n")
              runfile.write("combineTool.py -M FastScan -w "+shortcard+"_DefMod.root:w -o "+shortcard+"_DefMod_Asimov_nll -t -1\n")
              runfile.write("combineTool.py -M FastScan -w "+shortcard+"_DefMod.root:w -o "+shortcard+"_DefMod_nll\n")
              runfile.write("combineTool.py -M MultiDimFit "+shortcard+"_DefMod.root --algo grid --points=41 --rMin -10 --rMax 10 --alignEdges 1 -t -1 --name ."+shortcard+"_DefMod\n")
              runfile.write("combineTool.py -M MultiDimFit "+shortcard+"_DefMod.root --algo grid --points=401 --rMin -100 --rMax 100 --alignEdges 1 -t -1 --name ."+shortcard+"_DefMod_rRange100\n")
              runfile.write("plot1DScan.py higgsCombine."+shortcard+"_DefMod.MultiDimFit.mH120.root -o "+shortcard+"_DefMod_MDfit\n")
              runfile.write("plot1DScan.py higgsCombine."+shortcard+"_DefMod_rRange100.MultiDimFit.mH120.root -o "+shortcard+"_DefMod_rRange100_MDfit\n")
            elif args.Breakdown:
              runfile.write("combine -M MultiDimFit "+shortcard+"_DefMod.root --points=21 --rMin -5 --rMax 5 --alignEdges 1 -t -1 --saveWorkspace -n ."+shortcard+"_saveWorkspace\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 -t -1 -n ."+shortcard+"_total\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy -t -1 -n ."+shortcard+"_freeze_jet\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory -t -1 -n ."+shortcard+"_freeze_jet_theory\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake -t -1 -n ."+shortcard+"_freeze_jet_theory_fake\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec\n")
              if "EE" in shortcard: runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,cf -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec_cf\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeParameters allConstrainedNuisances -t -1 -n ."+shortcard+"_freeze_all\n")
              if "EE" not in shortcard:
                runfile.write("plot1DScan.py higgsCombine."+shortcard+"_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine."+shortcard+"_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine."+shortcard+"_freeze_jet_theory.MultiDimFit.mH120.root:\"jet+theory\":5 higgsCombine."+shortcard+"_freeze_jet_theory_fake.MultiDimFit.mH120.root:\"jet+theory+fake\":6 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep.MultiDimFit.mH120.root:\"jet+theory+fake+lep\":7 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup\":8 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi\":9 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag\":10 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire\":11 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met\":12 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec\":13 higgsCombine."+shortcard+"_freeze_all.MultiDimFit.mH120.root:\"stat\":14 --output "+shortcard+"_breakdown --y-max 10 --y-cut 40 --breakdown \"jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,rest,stat\"\n")
              else:
                runfile.write("plot1DScan.py higgsCombine."+shortcard+"_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine."+shortcard+"_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine."+shortcard+"_freeze_jet_theory.MultiDimFit.mH120.root:\"jet+theory\":5 higgsCombine."+shortcard+"_freeze_jet_theory_fake.MultiDimFit.mH120.root:\"jet+theory+fake\":6 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep.MultiDimFit.mH120.root:\"jet+theory+fake+lep\":7 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup\":8 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi\":9 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag\":10 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire\":11 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met\":12 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec\":13 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec_cf.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec+cf\":14 higgsCombine."+shortcard+"_freeze_all.MultiDimFit.mH120.root:\"stat\":15 --output "+shortcard+"_breakdown --y-max 10 --y-cut 40 --breakdown \"jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,cf,rest,stat\"\n")

          else:
            if args.FitDiag:
              runfile.write("echo Running FitDiagnostics...\n") # Asimov set as default; FIXME later to choose Asimov or not
              runfile.write("combine -M FitDiagnostics "+shortcard+".root --rMin -10 --rMax 10 --saveShapes --saveWithUncertainties -n _"+shortcard+" --plots -t -1\n")
            elif args.Impact:
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
              runfile.write("combineTool.py -M MultiDimFit "+shortcard+".root --algo grid --points=41 --rMin -10 --rMax 10 --alignEdges 1 -t -1 --name ."+shortcard+"\n")
              runfile.write("combineTool.py -M MultiDimFit "+shortcard+".root --algo grid --points=401 --rMin -100 --rMax 100 --alignEdges 1 -t -1 --name ."+shortcard+"_rRange100\n") # original code
              runfile.write("echo Summarizing the result into 1D...\n")
              runfile.write("plot1DScan.py higgsCombine."+shortcard+".MultiDimFit.mH120.root -o "+shortcard+"_MDfit\n")
              runfile.write("plot1DScan.py higgsCombine."+shortcard+"_rRange100.MultiDimFit.mH120.root -o "+shortcard+"_rRange100_MDfit\n")
            elif args.Breakdown:
              runfile.write("combine -M MultiDimFit "+shortcard+".root --points=21 --rMin -5 --rMax 5 --alignEdges 1 -t -1 --saveWorkspace -n ."+shortcard+"_saveWorkspace\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 -t -1 -n ."+shortcard+"_total\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy -t -1 -n ."+shortcard+"_freeze_jet\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory -t -1 -n ."+shortcard+"_freeze_jet_theory\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake -t -1 -n ."+shortcard+"_freeze_jet_theory_fake\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec\n")
              if "EE" in shortcard: runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,cf -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec_cf\n")
              runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeParameters allConstrainedNuisances -t -1 -n ."+shortcard+"_freeze_all\n")
              if "EE" not in shortcard:
                runfile.write("plot1DScan.py higgsCombine."+shortcard+"_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine."+shortcard+"_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine."+shortcard+"_freeze_jet_theory.MultiDimFit.mH120.root:\"jet+theory\":5 higgsCombine."+shortcard+"_freeze_jet_theory_fake.MultiDimFit.mH120.root:\"jet+theory+fake\":6 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep.MultiDimFit.mH120.root:\"jet+theory+fake+lep\":7 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup\":8 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi\":9 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag\":10 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire\":11 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met\":12 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec\":13 higgsCombine."+shortcard+"_freeze_all.MultiDimFit.mH120.root:\"stat\":14 --output "+shortcard+"_breakdown --y-max 10 --y-cut 40 --breakdown \"jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,rest,stat\"\n")
              else:
                runfile.write("plot1DScan.py higgsCombine."+shortcard+"_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine."+shortcard+"_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine."+shortcard+"_freeze_jet_theory.MultiDimFit.mH120.root:\"jet+theory\":5 higgsCombine."+shortcard+"_freeze_jet_theory_fake.MultiDimFit.mH120.root:\"jet+theory+fake\":6 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep.MultiDimFit.mH120.root:\"jet+theory+fake+lep\":7 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup\":8 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi\":9 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag\":10 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire\":11 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met\":12 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec\":13 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec_cf.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec+cf\":14 higgsCombine."+shortcard+"_freeze_all.MultiDimFit.mH120.root:\"stat\":15 --output "+shortcard+"_breakdown --y-max 10 --y-cut 40 --breakdown \"jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,cf,rest,stat\"\n")

        else: # mass is above 3000 GeV so that it only contains SSWW, or Weinberg --> get impact with default physics model
          if args.FitDiag:
            runfile.write("echo Running FitDiagnostics...\n") # Asimov set as default; FIXME later to choose Asimov or not
            runfile.write("combine -M FitDiagnostics "+shortcard+".root --rMin -10 --rMax 10 --saveShapes --saveWithUncertainties -n _"+shortcard+" --plots -t -1\n") # FIXME maybe shortcard+"_DefMod.root? No?
          elif args.Impact:
            runfile.write("combineTool.py -M Impacts -d "+shortcard+"_DefMod.root -m "+this_mass+" --rMin -100 --rMax 100 --robustFit 1 --doInitialFit --name "+shortcard+"_DefMod -t -1\n")
            runfile.write("combineTool.py -M Impacts -d "+shortcard+"_DefMod.root -m "+this_mass+" --rMin -100 --rMax 100 --robustFit 1 --doFits --name "+shortcard+"_DefMod -t -1\n")
            runfile.write("combineTool.py -M Impacts -d "+shortcard+"_DefMod.root -m "+this_mass+" --rMin -100 --rMax 100 --robustFit 1 --output "+shortcard+"_DefMod_impacts.json --name "+shortcard+"_DefMod\n")
            runfile.write("plotImpacts.py -i "+shortcard+"_DefMod_impacts.json -o "+shortcard+"_DefMod\n")
            #runfile.write("pdfseparate "+shortcard+"_DefMod.pdf -f 1 -l 1 "+shortcard+"_DefMod_1.pdf\n")
            runfile.write("combineTool.py -M FastScan -w "+shortcard+"_DefMod.root:w -o "+shortcard+"_DefMod_Asimov_nll -t -1\n")
            runfile.write("combineTool.py -M FastScan -w "+shortcard+"_DefMod.root:w -o "+shortcard+"_DefMod_nll\n")
            runfile.write("combineTool.py -M MultiDimFit "+shortcard+"_DefMod.root --algo grid --points=41 --rMin -10 --rMax 10 --alignEdges 1 -t -1 --name ."+shortcard+"_DefMod\n")
            runfile.write("combineTool.py -M MultiDimFit "+shortcard+"_DefMod.root --algo grid --points=401 --rMin -100 --rMax 100 --alignEdges 1 -t -1 --name ."+shortcard+"_DefMod_rRange100\n")
            runfile.write("plot1DScan.py higgsCombine."+shortcard+"_DefMod.MultiDimFit.mH120.root -o "+shortcard+"_DefMod_MDfit\n")
            runfile.write("plot1DScan.py higgsCombine."+shortcard+"_DefMod_rRange100.MultiDimFit.mH120.root -o "+shortcard+"_DefMod_rRange100_MDfit\n")
          elif args.Breakdown:
            runfile.write("combine -M MultiDimFit "+shortcard+"_DefMod.root --points=21 --rMin -5 --rMax 5 --alignEdges 1 -t -1 --saveWorkspace -n ."+shortcard+"_saveWorkspace\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 -t -1 -n ."+shortcard+"_total\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy -t -1 -n ."+shortcard+"_freeze_jet\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory -t -1 -n ."+shortcard+"_freeze_jet_theory\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake -t -1 -n ."+shortcard+"_freeze_jet_theory_fake\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec\n")
            if "EE" in shortcard: runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeNuisanceGroups jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,cf -t -1 -n ."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec_cf\n")
            runfile.write("combine -M MultiDimFit higgsCombine."+shortcard+"_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r=-5,5 --freezeParameters allConstrainedNuisances -t -1 -n ."+shortcard+"_freeze_all\n")
            if "EE" not in shortcard:
              runfile.write("plot1DScan.py higgsCombine."+shortcard+"_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine."+shortcard+"_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine."+shortcard+"_freeze_jet_theory.MultiDimFit.mH120.root:\"jet+theory\":5 higgsCombine."+shortcard+"_freeze_jet_theory_fake.MultiDimFit.mH120.root:\"jet+theory+fake\":6 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep.MultiDimFit.mH120.root:\"jet+theory+fake+lep\":7 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup\":8 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi\":9 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag\":10 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire\":11 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met\":12 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec\":13 higgsCombine."+shortcard+"_freeze_all.MultiDimFit.mH120.root:\"stat\":14 --output "+shortcard+"_breakdown --y-max 10 --y-cut 40 --breakdown \"jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,rest,stat\"\n")
            else:
              runfile.write("plot1DScan.py higgsCombine."+shortcard+"_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine."+shortcard+"_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine."+shortcard+"_freeze_jet_theory.MultiDimFit.mH120.root:\"jet+theory\":5 higgsCombine."+shortcard+"_freeze_jet_theory_fake.MultiDimFit.mH120.root:\"jet+theory+fake\":6 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep.MultiDimFit.mH120.root:\"jet+theory+fake+lep\":7 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup\":8 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi\":9 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag\":10 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire\":11 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met\":12 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec\":13 higgsCombine."+shortcard+"_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec_cf.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec+cf\":14 higgsCombine."+shortcard+"_freeze_all.MultiDimFit.mH120.root:\"stat\":15 --output "+shortcard+"_breakdown --y-max 10 --y-cut 40 --breakdown \"jet_energy,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,cf,rest,stat\"\n")

        runfile.write("echo Done.\n")

      with open(WP+"/"+shortcard+"/submit_"+this_check+".sh",'a') as submitfile:
        submitfile.write("executable = Run"+this_check+".sh\n")
        submitfile.write("log = "+shortcard+"_Run"+this_check+".log\n")
        submitfile.write("output = "+shortcard+"_Run"+this_check+".out\n")
        submitfile.write("error = "+shortcard+"_Run"+this_check+".out\n")
        submitfile.write("queue\n")
      os.chdir(WP+"/"+shortcard)
      os.system('condor_submit -a "priority = -15" submit_'+this_check+'.sh -batch-name '+shortcard+'_'+WP+'_'+this_check)
      os.chdir(pwd)
