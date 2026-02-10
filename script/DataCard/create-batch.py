#!/usr/bin/env python3

# Place it at CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter
# python create-batch.py -l [RunList*.txt] --Asymptotic[--Full][--Q*][--Work][--Nuis][--pdf]
# python create-batch.py -l [RunList*.txt] --Full -t 500 -n 10 --rRange 0.1:5.0:0.5
# RunList.txt contains paths of results from text2workspace.py e.g. /data6/Users/jihkim/CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter/Workspace/card_2017_MuMu_M500_HNL_UL.root

import os, sys
import subprocess as cmd
import argparse
import math
import numpy as np
import random

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
# Parallelization & Grid Options for CLs
parser.add_argument('-t', dest='Ntoy', default='1000', help='Total N of toys per r-point')
parser.add_argument('-n', dest='Split', default=1, type=int, help='Number of parallel jobs to split the toys into')
parser.add_argument('--rRange', dest='rRange', help='Format: min:max:step (e.g. 0.1:5.0:0.2). REQUIRED for parallel CLs.', default=None)
parser.add_argument('--Diagnostic', action='store_true', help='Save toys and verbosity for debugging')
parser.add_argument('--tag', dest='UserTag', default="", help='Unique tag to prevent overwrite DAGMan (e.g. v2, run2)')
# Other works
parser.add_argument('--Asymptotic', action='store_true')
parser.add_argument('--Work', action='store_true', help='create workspace')
parser.add_argument('--FitDiag', action='store_true', help='check nuisance fit')
parser.add_argument('--Impact', action='store_true', help='check impacts')
parser.add_argument('--FastScan', action='store_true', help='fast scan')
parser.add_argument('--MDfit', action='store_true', help='multidimension fits')
parser.add_argument('--r', default='0', help='(3ch, EMuFull only) inject r')
parser.add_argument('--f', default='0.5', help='(3ch, EMuFull only) inject f')
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

AsimovSetting = "-t -1 --expectSignal="+args.InjectSignal
AsimovName = "s"+args.InjectSignal

pwd = os.getcwd()
CMSSW_BASE = os.environ['CMSSW_BASE']

failure, result = cmd.getstatusoutput('combine --help')
if failure:
  print("[!!ERROR!!] cannot run combine.")
  print("Please set proper cmsenv first.")
  print("Exiting ...")
  sys.exit(1)

if args.Input is not None:
  args.RunLists = []
  args.RunLists.append(args.Input)

# --- Helper Functions ---

def parse_r_range(r_str):
    """
    Accepts:
      - "" / None -> []
      - "min:max" -> [min, max]
      - "min:max:step" -> list of grid points from min to max (inclusive-ish)
    """
    if not r_str:
        return []

    parts = r_str.split(':')
    try:
        if len(parts) == 2:
            start, end = map(float, parts)
            return [round(start, 4), round(end, 4)]

        if len(parts) == 3:
            start, end, step = map(float, parts)
            if step == 0:
                raise ValueError("step must be non-zero")

            # Create grid points (including end point if close)
            points = np.arange(start, end + step/1000.0, step).tolist()
            return [round(p, 4) for p in points]

        raise ValueError("expected 2 or 3 fields")
    except Exception:
        print(f"[ERROR] rRange format '{r_str}' is invalid. Use min:max or min:max:step")
        sys.exit(1)

def create_hybrid_grid_dag(WP, shortcard, card, quantile, quant_val, total_toys, n_split, r_points, pwd, is_diagnostic, user_tag):
    """
    Creates a DAG that:
    1. Runs N jobs in parallel. Each job iterates over the full r_grid but runs (Total/N) toys.
    2. Merges the outputs (Hadd).
    3. Reads the result to extract limits.
    """
    base_dir = f'Batch/{WP}/full_CLs/{shortcard}'
    
    r_tag = f"{r_points[0]}_{r_points[-1]}"
    full_tag = f"{r_tag}_{user_tag}" if user_tag else r_tag

    dag_filename = f'{shortcard}_{quantile}_{full_tag}.dag'
    toys_per_job = math.ceil(int(total_toys) / n_split)
    
    # Diagnostic flags: Save everything if requested
    extra_flags = "--saveToys --saveHybridResult -v 1" if is_diagnostic else ""

    dag_content = ""
    grid_output_files = []

    # 1. Parallel Jobs (Split by Toys)
    for k in range(n_split):
        job_name = f"{quantile}_part{k}_{full_tag}"
        part_suffix = f"{shortcard}_{quantile}_part{k}_{full_tag}"
        
        # Script generation: Loop over r_points INSIDE the shell script
        # This avoids creating thousands of condor jobs for each point.
        with open(f"{base_dir}/run_{job_name}.sh", 'w') as runfile:
            runfile.write("#!/bin/bash\n")

            # Setup CMSSW Environment
            runfile.write(f"pushd {CMSSW_BASE}/src\n")
            runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
            runfile.write("eval `scramv1 runtime -sh`\n")
            runfile.write("popd\n") # Return to working directory

            runfile.write("ulimit -s unlimited\n")
            
            # Loop over r values
            for r in r_points:
                # Generate a random seed here in Python
                # This allows us to predict the output filename exactly.
                this_seed = random.randint(100000, 999999)
                
                # Command explanation:
                # --singlePoint {r}: Fix r, don't auto-search.
                # --clsAcc 0: Calculate CLs exactly (no approximations).
                # -s {this_seed}: We enforce the seed.
                cmd = (f"combine -M HybridNew --LHCmode LHC-limits {card} "
                       f"-n {part_suffix}_r{r} "
                       f"--expectedFromGrid {quant_val} "
                       f"-T {toys_per_job} --singlePoint {r} --clsAcc 0 -s {this_seed} {extra_flags}")
                
                runfile.write(f"echo 'Running Point r={r} with seed {this_seed}'\n")
                runfile.write(f"{cmd}\n")
                
                # Construct the exact filename that combine will produce with this seed
                # Format: higgsCombine{Name}.HybridNew.mH120.{Seed}.quant{Quant}.root
                expected_file = f"higgsCombine{part_suffix}_r{r}.HybridNew.mH120.{this_seed}.quant{quant_val}.root"
                grid_output_files.append(f"{pwd}/{base_dir}/{expected_file}") # Add to global merge list
            
            runfile.write("echo 'All points done for this chunk.'\n")

        with open(f"{base_dir}/submit_{job_name}.sub", 'w') as subfile:
            subfile.write("universe = vanilla\n")
            subfile.write("getenv = True\n")
            subfile.write("request_memory = 4000\n")
            subfile.write("request_cpus = 1\n")
            subfile.write(f"executable = run_{job_name}.sh\n")
            subfile.write(f"log = logs/{job_name}.log\n")
            subfile.write(f"output = logs/{job_name}.out\n")
            subfile.write(f"error = logs/{job_name}.out\n")
            subfile.write("should_transfer_files = YES\n")
            subfile.write("when_to_transfer_output = ON_EXIT\n")
            subfile.write("queue\n")

        dag_content += f"JOB {job_name} submit_{job_name}.sub\n"
        
    # 2. Merge Job (Hadd)
    # We produce ONE big merged file containing all toys for all r points
    merged_filename = f"higgsCombine{shortcard}_{quantile}_merged_{full_tag}.HybridNew.mH120.quant{quant_val}.root"
    merge_job_name = f"{quantile}_merge_{full_tag}"
    
    with open(f"{base_dir}/run_{merge_job_name}.sh", 'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write(f"pushd {CMSSW_BASE}/src\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("eval `scramv1 runtime -sh`\n")
        runfile.write("popd\n")
        # Hadd allows merging files with same tree structures (HybridResult)
        # Note: Argument list might be too long for bash, so we use xargs or a response file if needed.
        # For ~10 splits * ~20 points = 200 files, it's usually fine.
        files_str = " ".join(grid_output_files)
        runfile.write(f"hadd -f {merged_filename} {files_str}\n")

    with open(f"{base_dir}/submit_{merge_job_name}.sub", 'w') as subfile:
        subfile.write("universe = vanilla\n")
        subfile.write("getenv = True\n")
        subfile.write(f"executable = run_{merge_job_name}.sh\n")
        subfile.write(f"log = logs/{merge_job_name}.log\n")
        subfile.write(f"output = logs/{merge_job_name}.out\n")
        subfile.write(f"error = logs/{merge_job_name}.out\n")
        subfile.write("should_transfer_files = YES\n")
        subfile.write("when_to_transfer_output = ON_EXIT\n")
        subfile.write("queue\n")

    dag_content += f"JOB {merge_job_name} submit_{merge_job_name}.sub\n"
    parent_line = "PARENT " + " ".join([f"{quantile}_part{k}_{full_tag}" for k in range(n_split)])
    dag_content += f"{parent_line} CHILD {merge_job_name}\n"

    # scripts to merge multiple runs' outputs

    final_merged_filename = f"higgsCombine{shortcard}_{quantile}_merged_final.HybridNew.mH120.quant{quant_val}.root"
    final_merge_job_name = f"{quantile}_merge_final"
    
    with open(f"{base_dir}/run_{final_merge_job_name}.sh", 'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write(f"pushd {CMSSW_BASE}/src\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("eval `scramv1 runtime -sh`\n")
        runfile.write("popd\n")
        runfile.write(f"hadd -f {final_merged_filename} $(find {base_dir} -maxdepth 1 -type f -name '*merged*.root' ! -name '*merged_final*' -exec realpath {{}} +)\n")

    with open(f"{base_dir}/submit_{final_merge_job_name}.sub", 'w') as subfile:
        subfile.write("universe = vanilla\n")
        subfile.write("getenv = True\n")
        subfile.write(f"executable = run_{final_merge_job_name}.sh\n")
        subfile.write(f"log = logs/{final_merge_job_name}.log\n")
        subfile.write(f"output = logs/{final_merge_job_name}.out\n")
        subfile.write(f"error = logs/{final_merge_job_name}.out\n")
        subfile.write("should_transfer_files = YES\n")
        subfile.write("when_to_transfer_output = ON_EXIT\n")
        subfile.write(f"batch_name = {shortcard}_{WP}_{quantile}_merge_final\n")
        subfile.write("queue\n")

    # 3. Read/Limit Job
    read_job_name = f"{quantile}_read_{full_tag}"
    this_output = f"{shortcard}_{quantile}_{full_tag}.root"
    plot_output_name = f"limit_scan_{shortcard}_{quantile}_{full_tag}"
    
    with open(f"{base_dir}/run_{read_job_name}.sh", 'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write(f"pushd {CMSSW_BASE}/src\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("eval `scramv1 runtime -sh`\n")
        runfile.write("popd\n")
        # --readHybridResults: reads the merged grid
        # --grid: input file
        # It will interpolate between the r points we scanned.
        runfile.write(f"combine -M HybridNew --LHCmode LHC-limits {card} -n {shortcard}_{full_tag} --readHybridResults --grid={pwd}/{base_dir}/{merged_filename} --expectedFromGrid {quant_val} --plot={plot_output_name}.png\n")
        # Optional: Plotting command hint (can't run easily without X11, but script is ready)
        # runfile.write(f"plotLimitGrid.py {this_output} ... \n")
        runfile.write("echo 'Limit calculation and plotting done.'\n")

    with open(f"{base_dir}/submit_{read_job_name}.sub", 'w') as subfile:
        subfile.write("universe = vanilla\n")
        subfile.write("getenv = True\n")
        subfile.write(f"executable = run_{read_job_name}.sh\n")
        subfile.write(f"log = logs/{read_job_name}.log\n")
        subfile.write(f"output = logs/{read_job_name}.out\n")
        subfile.write(f"error = logs/{read_job_name}.out\n")
        subfile.write("should_transfer_files = YES\n")
        subfile.write("when_to_transfer_output = ON_EXIT\n")

        expected_this = f"higgsCombine{shortcard}_{full_tag}.HybridNew.mH120.quant{quant_val}.root"
        expected_plot = f"{plot_output_name}.png"

        # Transfer both ROOT file and PNG
        subfile.write(f"transfer_output_files = {expected_this},{expected_plot}\n")
        subfile.write(f"transfer_output_remaps = \"{expected_this} = output/{this_output}; {expected_plot} = output/{expected_plot}\"\n")
        subfile.write("queue\n")

    dag_content += f"JOB {read_job_name} submit_{read_job_name}.sub\n"
    dag_content += f"PARENT {merge_job_name} CHILD {read_job_name}\n"

    # scripts to run CLs using final merged grids

    final_read_job_name = f"{quantile}_read_final"
    final_output = f"{shortcard}_{quantile}_final.root"
    final_plot_output_name = f"limit_scan_{shortcard}_{quantile}_final"
    
    with open(f"{base_dir}/run_{final_read_job_name}.sh", 'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write(f"pushd {CMSSW_BASE}/src\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("eval `scramv1 runtime -sh`\n")
        runfile.write("popd\n")
        runfile.write(f"combine -M HybridNew --LHCmode LHC-limits {card} -n {shortcard}_final --readHybridResults --grid={pwd}/{base_dir}/{merged_filename} --expectedFromGrid {quant_val} --plot={final_plot_output_name}.png\n")
        runfile.write("echo 'Limit calculation and plotting done.'\n")

    with open(f"{base_dir}/submit_{final_read_job_name}.sub", 'w') as subfile:
        subfile.write("universe = vanilla\n")
        subfile.write("getenv = True\n")
        subfile.write(f"executable = run_{final_read_job_name}.sh\n")
        subfile.write(f"log = logs/{final_read_job_name}.log\n")
        subfile.write(f"output = logs/{final_read_job_name}.out\n")
        subfile.write(f"error = logs/{final_read_job_name}.out\n")
        subfile.write("should_transfer_files = YES\n")
        subfile.write("when_to_transfer_output = ON_EXIT\n")
        subfile.write(f"batch_name = {shortcard}_{WP}_{quantile}_read_final\n")

        expected_this = f"higgsCombine{shortcard}_final.HybridNew.mH120.quant{quant_val}.root"
        expected_plot = f"{final_plot_output_name}.png"

        # Transfer both ROOT file and PNG
        subfile.write(f"transfer_output_files = {expected_this},{expected_plot}\n")
        subfile.write(f"transfer_output_remaps = \"{expected_this} = output/{final_output}; {expected_plot} = output/{expected_plot}\"\n")
        subfile.write("queue\n")

    with open(f"{base_dir}/{dag_filename}", 'w') as f:
        f.write(dag_content)

    return dag_filename

# --- Main Logic ---

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
      skel.write("request_memory = 8000\n")
      skel.write("request_cpus = 1\n")
  else: # Extract limits
    os.system('mkdir -p Batch/'+WP)
    with open('Batch/submit_skeleton.sh','w') as skel:
      skel.write("universe = vanilla\n")
      skel.write("getenv   = True\n")
      skel.write("should_transfer_files = YES\n")
      skel.write("when_to_transfer_output = ON_EXIT\n")
      skel.write("request_memory = 4000\n")
      skel.write("request_cpus = 1\n")
  
  for i in range(0,NCARD):
    card = cards[i].strip('\n')
    if '#' in card: continue
    shortcard = card.split('/')[-1].replace(".root","").replace(".txt","").replace("card_","") # Run2_EE_Ext_M500_syst
    this_mass = "0" if "Weinberg" in shortcard else shortcard.split('_M')[-1].split('_')[0]

    if "EMuFull" in WP or "3ch" in shortcard: AsimovName = f"r{args.r}f{args.f}"
 
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
        os.system("cp "+this_shortcard+"_"+AsimovName+"_Breakdown.pdf "+this_shortcard+"_"+AsimovName+"_Breakdown.png "+pwd+"/"+this_check+"/"+WP+"/"+AsimovName)
        os.chdir(pwd)
      continue
    elif args.Work:
      os.system('mkdir -p '+WP+'/'+shortcard)
      os.system('cp '+WP+'/submit_skeleton.sh '+WP+'/'+shortcard+'/submit_Workspace.sh')
    elif IsNuis:
      os.system(f'mkdir -p {WP}/{shortcard}/{this_check}/{AsimovName}')
      os.system(f'cp {WP}/submit_skeleton.sh {WP}/{shortcard}/{this_check}/{AsimovName}/submit_{this_check}_{AsimovName}.sh')
      os.system(f'cp {WP}/{shortcard}/{shortcard}.root {WP}/{shortcard}/{this_check}/{AsimovName}')
      os.system(f'cp {WP}/{shortcard}/{shortcard}_DefMod.root {WP}/{shortcard}/{this_check}/{AsimovName}')
    else:
      # CLs extraction
      os.system('mkdir -p Batch/'+WP+'/full_CLs/'+shortcard+'/output/')
      os.system('mkdir -p Batch/'+WP+'/full_CLs/'+shortcard+'/logs/')
      
      quantiles_to_run = []
      if args.Full or args.Q1: quantiles_to_run.append(('Q1', '0.025'))
      if args.Full or args.Q2: quantiles_to_run.append(('Q2', '0.160'))
      if args.Full or args.Q3: quantiles_to_run.append(('Q3', '0.500'))
      if args.Full or args.Q4: quantiles_to_run.append(('Q4', '0.840'))
      if args.Full or args.Q5: quantiles_to_run.append(('Q5', '0.975'))

      for q_name, q_val in quantiles_to_run:
          
          # [Refactored] Parallel + Grid Logic
          if args.Split > 1:
              if not args.rRange:
                  print(f"[ERROR] You requested split jobs (-n {args.Split}) but provided no rRange.")
                  print("For HybridNew parallelization, you MUST provide --rRange min:max:step (e.g. 0.1:5.0:0.5)")
                  print("This is to ensure all jobs simulate the same r-values for merging.")
                  sys.exit(1)
              
              r_points = parse_r_range(args.rRange)
              print(f"[{shortcard}] Grid Scan Mode: {len(r_points)} points from {r_points[0]} to {r_points[-1]}")
              
              dag_file = create_hybrid_grid_dag(WP, shortcard, card, q_name, q_val, args.Ntoy, args.Split, r_points, pwd, args.Diagnostic, args.UserTag)
              
              os.chdir('Batch/'+WP+'/full_CLs/'+shortcard)

              # [Modified] Submit and Show Cluster ID
              batch_name = f"{shortcard}_{WP}_{q_name}_Ntoy{args.Ntoy}"
              if args.rRange: batch_name += f"_{args.rRange}"
              
              command = f'condor_submit_dag -batch-name {batch_name} {dag_file}'
              status, output = cmd.getstatusoutput(command)
              
              if status == 0:
                  print(f"\n[Submission Success] Job submitted: {batch_name}")
                  print(output)
                  import re
                  match = re.search(r'cluster (\d+)', output)
                  if match:
                      print(f"To remove this job: condor_rm {match.group(1)}  OR  condor_rm -constraint 'JobBatchName == \"{batch_name}\"'")
                  print("="*60 + "\n")
              else:
                  print(f"[Submission Failed] Error executing: {command}")
                  print(output)

              os.chdir(pwd)
              
          else:
              # [Fallback] Single Job (Existing Logic with Diagnostic capability)
              os.system(f'cp Batch/submit_skeleton.sh Batch/{WP}/full_CLs/{shortcard}/submit_{q_name}.sh')
              extra_flags = f"--saveToys --saveHybridResult -v 1 --plot=limit_scan_{shortcard}_{q_name}.png" if args.Diagnostic else ""
              
              with open(f"Batch/{WP}/full_CLs/{shortcard}/run_{q_name}.sh",'w') as runfile:
                runfile.write("#!/bin/bash\n")
                # Even in single job, if user gives rRange, we use it for better stability
                if args.rRange:
                    # Not fully implemented for single job here to keep it simple, 
                    # but typically single job uses auto-search.
                    runfile.write(f"combine -M HybridNew --LHCmode LHC-limits {card} -n {shortcard} --expectedFromGrid {q_val} -T {args.Ntoy} {extra_flags}\n")
                else:
                    runfile.write(f"combine -M HybridNew --LHCmode LHC-limits {card} -n {shortcard} --expectedFromGrid {q_val} -T {args.Ntoy} {extra_flags}\n")
              
              with open(f"Batch/{WP}/full_CLs/{shortcard}/submit_{q_name}.sh",'a') as submitfile:
                submitfile.write(f"executable = run_{q_name}.sh\n")
                submitfile.write(f"log = {shortcard}_{q_name}.log\n")
                submitfile.write(f"output = {shortcard}_{q_name}.out\n")
                submitfile.write(f"error = {shortcard}_{q_name}.out\n")
                if args.Diagnostic:
                  submitfile.write(f"transfer_output_files = higgsCombine{shortcard}.HybridNew.mH120.123456.quant{q_val}.root,limit_scan_{shortcard}_{q_name}.png\n") # NOTE that --saveToys change the output file name to include the seed.
                  submitfile.write(f"transfer_output_remaps = \"higgsCombine{shortcard}.HybridNew.mH120.123456.quant{q_val}.root = output/{shortcard}_{q_name}.root\"\n")
                else:
                  submitfile.write(f"transfer_output_files = higgsCombine{shortcard}.HybridNew.mH120.quant{q_val}.root,limit_scan_{shortcard}_{q_name}.png\n") # NOTE that --saveToys change the output file name to include the seed.
                  submitfile.write(f"transfer_output_remaps = \"higgsCombine{shortcard}.HybridNew.mH120.quant{q_val}.root = output/{shortcard}_{q_name}.root\"\n")
                submitfile.write("queue\n")
              
              os.chdir('Batch/'+WP+'/full_CLs/'+shortcard)
              os.system(f'condor_submit submit_{q_name}.sh -batch-name {shortcard}_{WP}_{q_name}')
              os.chdir(pwd)

    if args.Asymptotic:
      os.system('mkdir -p Batch/'+WP+'/Asymptotic/'+shortcard+'/output/')
      os.system('cp Batch/submit_skeleton.sh Batch/'+WP+'/Asymptotic/'+shortcard+'/submit_Asymptotic.sh')

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
        if "3ch" in shortcard:
            if (float(this_mass) > 3000.):
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_3ch "+card+" --PO r0=0.1 --channel-masks -o "+shortcard+".root\n")
            elif (float(this_mass) <= 100.):
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_3ch "+card+" --PO r0=1 --channel-masks -o "+shortcard+".root\n")
            else:
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_3ch "+card+" --PO r0=0.01 --channel-masks -o "+shortcard+".root\n")
        elif "EMu" in shortcard:
          if "EMuFull" in WP:
            if (float(this_mass) > 3000.):
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_EMu_Full "+card+" --PO r0=0.1 --channel-masks -o "+shortcard+".root\n")
            elif (float(this_mass) <= 100.):
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_EMu_Full "+card+" --PO r0=1 --channel-masks -o "+shortcard+".root\n")
            else:
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_EMu_Full "+card+" --PO r0=0.01 --channel-masks -o "+shortcard+".root\n")
          else:
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
      with open(f"{WP}/{shortcard}/{this_check}/{AsimovName}/Run{this_check}_{AsimovName}.sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("pushd "+pwd+"/"+WP+"/"+shortcard+"/"+this_check+"/"+AsimovName+"\n")
        runfile.write("echo Setting cmsenv environment...\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("cmsenv\n")
        runfile.write("echo Done.\n")
        runfile.write("popd\n")

        for this_shortcard in list_shortcard:
          if args.FitDiag:
            if "DefMod" in this_shortcard: continue # Must use the actual physics model
            runfile.write("echo Running FitDiagnostics...\n") # Asimov set as default; FIXME later to choose whether Asimov or not
            runfile.write(f"combine -M FitDiagnostics {pwd}/{WP}/{shortcard}/{this_shortcard}.root --rMin -10 --rMax 10 --saveShapes --saveWithUncertainties --saveNormalizations --saveWorkspace -n _{this_shortcard} --plots {AsimovSetting}\n")
            runfile.write(f"python3 $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit/test/diffNuisances.py -a fitDiagnostics_{this_shortcard}.root > pulls_{this_shortcard}.txt\n")
          elif args.GOF:
            if "DefMod" in this_shortcard: continue # Must use the actual physics model
            runfile.write("echo Running the goodness of fit test...\n")
            runfile.write(f"combine -M GoodnessOfFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root -t -1 --algo saturated -n gof_Asimov_{this_shortcard}\n")
            runfile.write(f"combine -M GoodnessOfFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root -t {args.Ntoy} --algo saturated -n gof_Ntoy{args.Ntoy}_{this_shortcard}\n")
            runfile.write(f"combine -M GoodnessOfFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root --algo saturated --setParameters mask_year16a_sr1=1,mask_year16a_sr2=1,mask_year16a_sr3=1,mask_year16b_sr1=1,mask_year16b_sr2=1,mask_year16b_sr3=1,mask_year17_sr1=1,mask_year17_sr2=1,mask_year17_sr3=1,mask_year18_sr1=1,mask_year18_sr2=1,mask_year18_sr3=1,r=0 --freezeParameters mask_year16a_sr1,mask_year16a_sr2,mask_year16a_sr3,mask_year16b_sr1,mask_year16b_sr2,mask_year16b_sr3,mask_year17_sr1,mask_year17_sr2,mask_year17_sr3,mask_year18_sr1,mask_year18_sr2,mask_year18_sr3,r -n gof_CRonly_obs_{this_shortcard}\n")
            runfile.write(f"combine -M GoodnessOfFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root -t {args.Ntoy} --algo saturated --setParameters mask_year16a_sr1=1,mask_year16a_sr2=1,mask_year16a_sr3=1,mask_year16b_sr1=1,mask_year16b_sr2=1,mask_year16b_sr3=1,mask_year17_sr1=1,mask_year17_sr2=1,mask_year17_sr3=1,mask_year18_sr1=1,mask_year18_sr2=1,mask_year18_sr3=1,r=0 --freezeParameters mask_year16a_sr1,mask_year16a_sr2,mask_year16a_sr3,mask_year16b_sr1,mask_year16b_sr2,mask_year16b_sr3,mask_year17_sr1,mask_year17_sr2,mask_year17_sr3,mask_year18_sr1,mask_year18_sr2,mask_year18_sr3,r -n gof_CRonly_toys_Ntoy{args.Ntoy}_{this_shortcard}\n")
            runfile.write(f"combine -M GoodnessOfFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root -t {args.Ntoy} --algo saturated --setParameters mask_year16a_sr1=1,mask_year16a_sr2=1,mask_year16a_sr3=1,mask_year16b_sr1=1,mask_year16b_sr2=1,mask_year16b_sr3=1,mask_year17_sr1=1,mask_year17_sr2=1,mask_year17_sr3=1,mask_year18_sr1=1,mask_year18_sr2=1,mask_year18_sr3=1,r=0 --freezeParameters mask_year16a_sr1,mask_year16a_sr2,mask_year16a_sr3,mask_year16b_sr1,mask_year16b_sr2,mask_year16b_sr3,mask_year17_sr1,mask_year17_sr2,mask_year17_sr3,mask_year18_sr1,mask_year18_sr2,mask_year18_sr3,r --toysFrequentist -n gof_CRonly_toysFreq_Ntoy{args.Ntoy}_{this_shortcard}\n")
          elif args.Impact:
            if (float(this_mass) > 3000.):
              runfile.write(f"combineTool.py -M Impacts -d {pwd}/{WP}/{shortcard}/{this_shortcard}.root -m {this_mass} --rMin -100 --rMax 100 --robustFit 1 --doInitialFit --name Impact_{this_shortcard}_{AsimovName} {AsimovSetting}\n")
              runfile.write(f"combineTool.py -M Impacts -d {pwd}/{WP}/{shortcard}/{this_shortcard}.root -m {this_mass} --rMin -100 --rMax 100 --robustFit 1 --doFits --name Impact_{this_shortcard}_{AsimovName} {AsimovSetting}\n")
            else:
              runfile.write(f"combineTool.py -M Impacts -d {pwd}/{WP}/{shortcard}/{this_shortcard}.root -m {this_mass} --rMin -10 --rMax 10 --robustFit 1 --doInitialFit --name Impact_{this_shortcard}_{AsimovName} {AsimovSetting}\n")
              runfile.write(f"combineTool.py -M Impacts -d {pwd}/{WP}/{shortcard}/{this_shortcard}.root -m {this_mass} --rMin -10 --rMax 10 --robustFit 1 --doFits --name Impact_{this_shortcard}_{AsimovName} {AsimovSetting}\n")
            runfile.write(f"combineTool.py -M Impacts -d {pwd}/{WP}/{shortcard}/{this_shortcard}.root -m {this_mass} --output {this_shortcard}_{AsimovName}_impacts.json --name Impact_{this_shortcard}_{AsimovName}\n")
            runfile.write(f"plotImpacts.py -i {this_shortcard}_{AsimovName}_impacts.json -o Impact_{this_shortcard}_{AsimovName}\n")
          elif args.FastScan:
            runfile.write(f"combineTool.py -M FastScan -w {pwd}/{WP}/{shortcard}/{this_shortcard}.root:w -o {this_shortcard}_Asimov_nll {AsimovSetting}\n")
            runfile.write(f"combineTool.py -M FastScan -w {pwd}/{WP}/{shortcard}/{this_shortcard}.root:w -o {this_shortcard}_nll\n")
          elif args.MDfit:
            if "EMuFull" in WP or "3ch" in shortcard:
              if "DefMod" in this_shortcard: continue # Must use the actual physics model
              runfile.write(f"combineTool.py -M MultiDimFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root -t -1 --setParameters r={args.r},f={args.f} --setParameterRanges r=0,2:f=0,1 --algo grid --points=2601 --robustFit 1 --saveNLL --name _{this_shortcard}_grid_2D_Asimov_r{args.r}f{args.f}\n")
            else:
              runfile.write(f"combineTool.py -M MultiDimFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root --algo grid --points=41 --rMin -1 --rMax 1 --alignEdges 1 {AsimovSetting} --name .{this_shortcard}_{AsimovName}_rRange1\n")
              runfile.write(f"combineTool.py -M MultiDimFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root --algo grid --points=41 --rMin -10 --rMax 10 --alignEdges 1 {AsimovSetting} --name .{this_shortcard}_{AsimovName}_rRange10\n")
              runfile.write(f"combineTool.py -M MultiDimFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root --algo grid --points=401 --rMin -100 --rMax 100 --alignEdges 1 {AsimovSetting} --name .{this_shortcard}_{AsimovName}_rRange100\n")
              runfile.write(f"plot1DScan.py higgsCombine.{this_shortcard}_{AsimovName}_rRange1.MultiDimFit.mH120.root -o MDfit_rRange1_{this_shortcard}_{AsimovName}\n")
              runfile.write(f"plot1DScan.py higgsCombine.{this_shortcard}_{AsimovName}_rRange10.MultiDimFit.mH120.root -o MDfit_rRange10_{this_shortcard}_{AsimovName}\n")
              runfile.write(f"plot1DScan.py higgsCombine.{this_shortcard}_{AsimovName}_rRange100.MultiDimFit.mH120.root -o MDfit_rRange100_{this_shortcard}_{AsimovName}\n")
          elif args.Breakdown:
            rRange = {}
            if (float(this_mass) <= 100.):
              rRange['rMin'] = -0.05
              rRange['rMax'] = 0.05
              rRange['points'] = 41
            else:
              rRange['rMin'] = -5
              rRange['rMax'] = 5
              rRange['points'] = 21
            runfile.write(f"combine -M MultiDimFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root --points={rRange['points']} --rMin {rRange['rMin']} --rMax {rRange['rMax']} --alignEdges 1 {AsimovSetting} --saveWorkspace -n .{this_shortcard}_{AsimovName}_saveWorkspace\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} {AsimovSetting} -n .{this_shortcard}_{AsimovName}_total\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_jet\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,theory {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_jet_theory\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,theory,fake {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_jet_theory_fake\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,theory,fake,lep_uncert {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,theory,fake,lep_uncert,pileup {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,theory,fake,lep_uncert,pileup,lumi {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,theory,fake,lep_uncert,pileup,lumi,btag_sf {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec\n")
            if "EE" in this_shortcard: runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,cf {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec_cf\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeParameters allConstrainedNuisances {AsimovSetting} -n .{this_shortcard}_{AsimovName}_freeze_all\n")
            if "EE" not in this_shortcard:
              runfile.write(f"plot1DScan.py higgsCombine.{this_shortcard}_{AsimovName}_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory.MultiDimFit.mH120.root:\"jet+theory\":5 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake.MultiDimFit.mH120.root:\"jet+theory+fake\":6 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep.MultiDimFit.mH120.root:\"jet+theory+fake+lep\":7 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup\":8 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi\":9 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag\":10 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire\":11 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met\":12 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec\":13 higgsCombine.{this_shortcard}_{AsimovName}_freeze_all.MultiDimFit.mH120.root:\"stat\":14 --output {this_shortcard}_{AsimovName}_Breakdown --y-max 10 --y-cut 40 --breakdown \"jet_uncert,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,rest,stat\"\n")
            else:
              runfile.write(f"plot1DScan.py higgsCombine.{this_shortcard}_{AsimovName}_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory.MultiDimFit.mH120.root:\"jet+theory\":5 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake.MultiDimFit.mH120.root:\"jet+theory+fake\":6 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep.MultiDimFit.mH120.root:\"jet+theory+fake+lep\":7 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup\":8 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi\":9 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag\":10 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire\":11 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met\":12 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec\":13 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_theory_fake_lep_pileup_lumi_btag_prefire_met_xsec_cf.MultiDimFit.mH120.root:\"jet+theory+fake+lep+pileup+lumi+btag+prefire+met+xsec+cf\":14 higgsCombine.{this_shortcard}_{AsimovName}_freeze_all.MultiDimFit.mH120.root:\"stat\":15 --output {this_shortcard}_{AsimovName}_Breakdown --y-max 10 --y-cut 40 --breakdown \"jet_uncert,theory,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,cf,rest,stat\"\n")

        runfile.write("echo Done.\n")

      with open(WP+"/"+shortcard+"/"+this_check+"/"+AsimovName+"/submit_"+this_check+"_"+AsimovName+".sh",'a') as submitfile:
        submitfile.write("executable = Run"+this_check+"_"+AsimovName+".sh\n")
        submitfile.write("log = "+shortcard+"_Run"+this_check+"_"+AsimovName+".log\n")
        submitfile.write("output = "+shortcard+"_Run"+this_check+"_"+AsimovName+".out\n")
        submitfile.write("error = "+shortcard+"_Run"+this_check+"_"+AsimovName+".out\n")
        submitfile.write("should_transfer_files = YES\n")
        submitfile.write("when_to_transfer_output = ON_EXIT\n")
        submitfile.write("queue\n")
      os.chdir(WP+"/"+shortcard+"/"+this_check+"/"+AsimovName)
      if args.MDfit and ("EMuFull" in WP or "3ch" in shortcard):
        os.system(f'condor_submit -a "priority = -15" submit_{this_check}_{AsimovName}.sh -batch-name {shortcard}_{WP}_{this_check}_grid_2D_Asimov_r{args.r}f{args.f}')
      else:
        os.system(f'condor_submit -a "priority = -15" submit_{this_check}_{AsimovName}.sh -batch-name {shortcard}_{WP}_{this_check}_{AsimovName}')
      os.chdir(pwd)
