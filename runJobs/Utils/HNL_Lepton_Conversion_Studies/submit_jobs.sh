#!/bin/bash

# === Configuration ===
analyzer="HNL_Lepton_Conversion_Studies"
rundir="HNL_Lepton_Conversion_Studies"
mcpath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/"
datapath="${SKFlat_WD}/runJobs/HNL/${analyzer}/DATA/"
njobs=100
njobs_data=100
nmax=300
skim="SkimTree_DileptonBDT"

dryrun=false
userflags=""
mode=""
era_list=("2017")
joblist=("MC")  # You can add more modes here in future

# === Parse Arguments ===
while [[ $# -gt 0 ]]; do
  case "$1" in
    --mode)
      mode="$2"
      shift 2
      ;;
    --userflags)
      userflags="--userflags $2"
      shift 2
      ;;
    --dryrun)
      dryrun=true
      shift
      ;;
    --help)
      echo "Usage: ./run_lepton_conversion_studies.sh --mode <MODE> [--userflags AltMC] [--dryrun]"
      echo "Available modes:"
      for m in "${joblist[@]}"; do echo " - $m"; done
      exit 0
      ;;
    *)
      echo "[Error] Unknown option: $1. Use --help to see available options."
      exit 1
      ;;
  esac
done

# === Require mode ===
if [[ -z "$mode" ]]; then
  echo "[Error] --mode is required. Use --help to list available modes."
  exit 1
fi

# === Command Wrapper ===
run_cmd() {
  if $dryrun; then
    echo "[DryRun] $*"
  else
    eval "$@" &
  fi
}

# === Mode Dispatcher ===
run_mc() {
  for era in "${era_list[@]}"; do
    echo "[Info] Submitting MC jobs for era: $era"
    run_cmd "SKFlat.py -a $analyzer -i ZGToLLG -n $njobs --nmax $nmax -e $era --skim $skim $userflags"
    run_cmd "SKFlat.py -a $analyzer -i WGToLNuG -n $njobs --nmax $nmax -e $era --skim $skim $userflags"
    run_cmd "SKFlat.py -a $analyzer -l runJobs/SampleLists/Bkg/Conv/ConvExt.txt -n $njobs --nmax $nmax -e $era --skim $skim $userflags"
  done
}

# === Main Dispatch ===
case "$mode" in
  MC) run_mc ;;
  *)
    echo "[Error] Unsupported mode: $mode"
    exit 1
    ;;
esac

# === Finalize ===
if ! $dryrun; then
  wait
  echo "[Done] All jobs submitted for analyzer: $analyzer"
else
  echo "[DryRun] No jobs were actually submitted."
fi


