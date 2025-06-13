#!/bin/bash

# === Config ===
analyzer="HNL_TandP_Efficiency"
rundir="HNL_TandP_Efficiency"
mcpath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/"
datapath="${SKFlat_WD}/runJobs/SampleLists/Data/"
njobs=5
nmax=800
skim="SkimTree_DileptonBDT"
dryrun=false
joblist=("Default" "--dryrun" "--help")

# Default era list
era_list=("2018")
# Uncomment for full era range
# era_list=("2016preVFP" "2016postVFP" "2017" "2018")

# === Parse Arguments ===
for arg in "$@"; do
  case "$arg" in
    --dryrun)
      dryrun=true
      ;;
    --help)
      echo "Available job modes:"
      for j in "${joblist[@]}"; do echo " - $j"; done
      exit 0
      ;;
    *)
      echo "Unknown option: $arg. Use --help to list options."
      exit 1
      ;;
  esac
done

# === Command Runner ===
run_cmd() {
  if $dryrun; then
    echo "[DryRun] $*"
  else
    eval "$@" &
  fi
}

# === Submission Loop ===
for era in "${era_list[@]}"; do
  echo "Preparing submission for era: $era"

  run_cmd "SKFlat.py -a $analyzer -i DYJetsToEE_MiNNLO -n 400 --nmax ${nmax} -e ${era} --skim ${skim}"
  run_cmd "SKFlat.py -a $analyzer -i TTLL_powheg -n 400 --nmax ${nmax} -e ${era} --skim ${skim}"

  # Optional:
  # run_cmd "SKFlat.py -a $analyzer -l ${SKFlat_WD}/runJobs/SampleLists/Bkg/Prompt/DY_Ext.txt -n 200 --nmax ${nmax} -e ${era} --skim ${skim}"
done

# === Wait if not dry run ===
if ! $dryrun; then
  wait
  echo "All jobs submitted for analyzer: $analyzer"
else
  echo "Dry run complete. No jobs submitted."
fi
