#!/bin/bash

# === Configuration ===
analyzer="HNL_Signal_Efficiency"
rundir="HNL_Signal_Efficiency"
sigpath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Signals/"
mcpath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/"
datapath="${SKFlat_WD}/runJobs/SampleLists/Data/"

njobs=5
nmax=300
skim=""
dryrun=false

era_list=("2016postVFP" "2016preVFP" "2017" "2018")
joblist=("Baseline" "Default" "--help")

# === Parse Arguments ===
for arg in "$@"; do
  case "$arg" in
    --dryrun)
      dryrun=true
      ;;
    --help)
      echo "Available job modes:"
      for mode in "${joblist[@]}"; do echo " - $mode"; done
      exit 0
      ;;
  esac
done

# === Helper ===
run_cmd() {
  if $dryrun; then
    echo "[DryRun] $*"
  else
    eval "$@" &
  fi
}

# === Job Modes ===
run_default() {
  for era in "${era_list[@]}"; do
    # run_cmd "SKFlat.py -a $analyzer -l $sigpath/DY.txt -n $njobs --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT"
    # run_cmd "SKFlat.py -a $analyzer -l $sigpath/VBF.txt -n $njobs --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT"
    # run_cmd "SKFlat.py -a $analyzer -l $sigpath/SSWW.txt -n $njobs --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT"
    # run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Conv.txt -n 100 --nmax 400 -e $era --skim SkimTree_ConvEventSkim"
    run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Fake.txt -n 100 --nmax 400 -e $era --skim SkimTree_FakeEventSkimBDT"
    # run_cmd "SKFlat.py -a $analyzer -i TTLL_powheg -n 400 --nmax 400 -e $era --skim SkimTree_DileptonBDT"
    # run_cmd "SKFlat.py -a $analyzer -i DYJetsToEE_MiNNLO -n 400 --nmax 400 -e $era --skim SkimTree_DileptonBDT"
  done
}

run_baseline() {
  for era in "${era_list[@]}"; do
    # run_cmd "SKFlat.py -a $analyzer -l $sigpath/DY.txt -n $njobs --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT --userflags Baseline_EE"
    # run_cmd "SKFlat.py -a $analyzer -l $sigpath/VBF.txt -n $njobs --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT --userflags Baseline_EE"
    # run_cmd "SKFlat.py -a $analyzer -l $sigpath/SSWW.txt -n $njobs --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT --userflags Baseline_EE"
    run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_DiLepton_EE.txt -n 100 --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT --userflags Baseline_EE"
  done
}

# === Dispatcher ===
case "$1" in
  "Baseline") run_baseline ;;
  "" | "Default") run_default ;;
  "--help") echo "Available job modes:"; for j in "${joblist[@]}"; do echo " - $j"; done ;;
  *) echo "Unknown mode: $1. Use --help to list available options."; exit 1 ;;
esac

# === Wait for background jobs ===
if ! $dryrun; then
  wait
  echo "All jobs submitted for analyzer: $analyzer"
else
  echo "Dry run complete. No jobs submitted."
fi
