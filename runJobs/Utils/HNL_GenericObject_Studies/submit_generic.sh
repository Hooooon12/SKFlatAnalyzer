#!/bin/bash

analyzer="HNL_GenericObject_Studies"
rundir="HNL_GenericObject_Studies"
mcpath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Signals/"
bkgpath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/"
datapath="${SKFlat_WD}/runJobs/HNL/${analyzer}/DATA/"
njobs=20
njobs_data=20
nmax=300
dryrun=false

era_list=("2016postVFP" "2016preVFP" "2017" "2018")
joblist=("Default" "--help")

# Dry run mode
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
  local era="2018"

  # Uncomment this line if you want to run the Data EMu list
  # run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_DiLepton_EMu.txt -n $njobs_data --nmax $nmax -e $era --skim SkimTree_ZZ --userflags CheckEMu &"

  # This command is currently being used
  run_cmd "SKFlat.py -a $analyzer -i MuonEG:C -n 1 --nmax $nmax -e 2017 --skim SkimTree_ZZ --userflags CheckEMu &"
}

print_help() {
  echo "Available job modes:"
  for mode in "${joblist[@]}"; do
    echo " - $mode"
  done
}

# Dispatcher
case "$1" in
  "") run_default ;;
  "--help") print_help ;;
  *) echo "Unknown job mode: '$1'. Use --help to list available options." ;;
esac
