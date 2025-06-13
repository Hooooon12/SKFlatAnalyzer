#!/bin/bash

# === Config ===
analyzer="MeasureJetTaggingEfficiency"
rundir="MeasureJetTaggingEfficiency"

mcpath="${SKFlat_WD}/runJobs/SampleLists/Bkg"
sigpath="${SKFlat_WD}/runJobs/SampleLists/Signals"

njobs=200
njobs_data=400
nmax=800
skim="SkimTree_DileptonBDT"
dryrun=false

era_list=("2018" "2017" "2016postVFP" "2016preVFP")
joblist=("MC" "Sig" "--help")

# === Parse Arguments ===
mode="${1:-MC}"
if [[ "$2" == "--dryrun" ]]; then
  dryrun=true
fi

# === Helpers ===
run_cmd() {
  if $dryrun; then
    echo "[DryRun] $*"
  else
    eval "$@" &
  fi
}

print_help() {
  echo "Usage: ./run_measure_jet_tagging_eff.sh <mode> [--dryrun]"
  echo ""
  echo "Available modes:"
  for j in "${joblist[@]}"; do echo " - $j"; done
}

# === Job Modes ===
run_mc() {
  echo "Running MC background submissions..."
  for era in "${era_list[@]}"; do
      #run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/JetEff/MC_dilep_Jet.txt -n $njobs_data --nmax $nmax -e $era --skim $skim --userflags DiLeptonReq"
      run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/JetEff/MC_multilep_Jet.txt -n $njobs_data --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT --userflags DiLeptonReq"
  done
}

run_sig_and_extended() {
  echo "Running Signal + Extended MC submissions..."

  sig_list=(
    "${sigpath}/Private/SSWW.txt"
    "${sigpath}/Private/VBF.txt"
    "${sigpath}/Private/DY.txt"
  )

  file_list=(
    "${mcpath}/JetEff/MC_dilep_Jet.txt"
  )
  file_multilep_list=(
    "${mcpath}/JetEff/MC_multilep_Jet.txt"
  )

  for era in "${era_list[@]}"; do
    for file in "${file_list[@]}"; do
	run_cmd "SKFlat.py -a $analyzer -l $file -n $njobs --nmax $nmax -e $era --skim $skim --userflags DiLeptonReq"
    done
    for file in "${file_multilep_list[@]}"; do
	run_cmd "SKFlat.py -a $analyzer -l $file -n $njobs --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT --userflags DiLeptonReq"
    done

    for file in "${sig_list[@]}"; do
      run_cmd "SKFlat.py -a $analyzer -l $file -n $njobs --nmax $nmax -e $era --skim SkimTree_HNMultiLepBDT --userflags DiLeptonReq"
    done
  done
}

# === Dispatcher ===
case "$mode" in
  "MC") run_mc ;;
  "Sig") run_sig_and_extended ;;
  "--help") print_help; exit 0 ;;
  *) echo "Unknown mode: '$mode'. Use --help to list options."; exit 1 ;;
esac

# === Finalization ===
if ! $dryrun; then
  wait
  echo "All jobs submitted for analyzer: $analyzer"
else
  echo "Dry run complete. No jobs submitted."
fi
