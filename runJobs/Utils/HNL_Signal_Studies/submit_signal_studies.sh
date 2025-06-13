#!/bin/bash

# === Config ===
analyzer="HNL_Signal_Studies"
rundir="HNL_Signal_Studies"
mcpath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/"
sigpath="${SKFlat_WD}/runJobs/SampleLists/Signals/"
datapath="${SKFlat_WD}/runJobs/HNL/${analyzer}/DATA/"

njobs=20
njobs_data=20
nmax=400
dryrun=false
joblist=("WWJet" "Sig" "DY" "Z" "CheckMuonInEff" "Default" "--help")

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
  esac
done

# === Command Wrapper ===
run_cmd() {
  if $dryrun; then
    echo "[DryRun] $*"
  else
    eval "$@" &
  fi
}

# === Job Mode Functions ===
run_WWJet() {
  run_cmd "SKFlat.py -a $analyzer -l $sigpath/SSWW.txt -n 10 --nmax $nmax -e 2018 --skim SkimTree_HNMultiLepBDT --userflags WWJet"
}

run_Sig() {
  local era_list=("2017")
  local flags=("SignalKinematics")

  for era in "${era_list[@]}"; do
    for flag in "${flags[@]}"; do
      for sample in SSWW DY VBF; do
        run_cmd "SKFlat.py -a $analyzer -l $sigpath/Private/${sample}.txt -n $njobs --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT --userflags $flag"
        run_cmd "SKFlat.py -a $analyzer -l $sigpath/Official/${sample}Official.txt -n $njobs --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT --userflags $flag"
      done
    done
  done
}

run_DY() {
  local era_list=("2017")
  for era in "${era_list[@]}"; do
    run_cmd "SKFlat.py -a $analyzer -i DYJets -n 10 --nmax 500 -e $era --skim SkimTree_DileptonBDT"
  done
}

run_CheckMuonInEff() {
  run_cmd "SKFlat.py -a $analyzer -i DYTypeI_DF_M3000_private -n 10 --nmax 500 -e 2017 --skim SkimTree_HNMultiLepBDT --userflags CheckMuonInEff"
  run_cmd "SKFlat.py -a $analyzer -i DYTypeI_DF_M1000_private -n 10 --nmax 500 -e 2017 --skim SkimTree_HNMultiLepBDT --userflags CheckMuonInEff"
}

run_Z() {
  local era_list=("2018")
  for era in "${era_list[@]}"; do
    run_cmd "SKFlat.py -a $analyzer -i DYJets -n 200 --nmax 2000 -e $era --skim SkimTree_DileptonBDT"
  done
}

run_default() {
  local era_list=("2017")
  for era in "${era_list[@]}"; do
    run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Conv.txt -n 100 --nmax 400 -e $era --skim SkimTree_ConvEventSkim"
    run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Fake.txt -n 100 --nmax 400 -e $era --skim SkimTree_FakeEventSkimBDT"
    run_cmd "SKFlat.py -a $analyzer -i DYJets -n 500 --nmax 400 -e $era --skim SkimTree_DileptonBDT"
  done
}

# === Dispatcher ===
case "$1" in
  WWJet) run_WWJet ;;
  Sig) run_Sig ;;
  DY) run_DY ;;
  Z) run_Z ;;
  CheckMuonInEff) run_CheckMuonInEff ;;
  "" | Default) run_default ;;
  --help) echo "Available job modes:"; for j in "${joblist[@]}"; do echo " - $j"; done ;;
  *) echo "Unknown mode: $1. Use --help to list available options."; exit 1 ;;
esac

# === Wait if not dryrun ===
if ! $dryrun; then
  wait
  echo "All jobs submitted for analyzer: $analyzer"
else
  echo "Dry run complete. No jobs submitted."
fi
