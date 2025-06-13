#!/bin/bash

analyzer="HNL_HighPtMuon_Studies"
rundir="HNL_HighPtMuon_Studies"
datapath="${SKFlat_WD}/runJobs/SampleLists/Data/"
sigpath="${SKFlat_WD}/runJobs/SampleLists/Signals/Private/"
bkgpath="${SKFlat_WD}/runJobs/SampleLists/Bkg/Prompt/"
njobs=20
njobs_data=20
nmax=400
dryrun=false

era_list=("2016preVFP" "2016postVFP" "2017" "2018")
joblist=("Default" "DATA" "SIG" "--help")

# Enable dry run
if [[ "$2" == "--dryrun" ]]; then
  dryrun=true
fi

run_cmd() {
  if $dryrun; then
    echo "[DryRun] $*"
  else
    eval "$@"
  fi
}

run_default() {
  for era in "${era_list[@]}"; do
    run_cmd "SKFlat.py -a $analyzer -l ${bkgpath}/DY_Ext.txt -n 300 --nmax 500 -e $era --skim SkimTree_DileptonBDT --userflags DYPeak &"
  done
}

run_data() {
  for era in "${era_list[@]}"; do
    # Uncomment if you need SingleLepton Mu as well
    # run_cmd "SKFlat.py -a $analyzer -l ${datapath}/SL/${era}_SingleLepton_Mu.txt -n 1 --nmax 1 -e $era --skim SkimTree_HNMultiLepBDT &"
    run_cmd "SKFlat.py -a $analyzer -l ${datapath}/DL/${era}_DiLepton_MuMu.txt -n 300 --nmax 700 -e $era --skim SkimTree_DileptonBDT &"
  done
}

run_sig() {
  local era="2018"
  for sample in HM_DY HM_VBF HM_SSWW; do
    run_cmd "SKFlat.py -a $analyzer -l ${sigpath}/${sample}.txt -n 300 --nmax 300 -e $era --skim SkimTree_HNMultiLepBDT &"
  done
}

print_help() {
  echo "Available job modes:"
  for mode in "${joblist[@]}"; do
    echo " - $mode"
  done
}

# Dispatcher
case "$1" in
  "DATA") run_data ;;
  "SIG") run_sig ;;
  "--help"|"") print_help ;;
  *) run_default ;;
esac
