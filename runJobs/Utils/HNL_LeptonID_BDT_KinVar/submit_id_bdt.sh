#!/bin/bash

analyzer="HNL_LeptonID_BDT_KinVar"
rundir="HNL_LeptonID_BDT_KinVar"
sigpath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Signals/"
mcpath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/"
njobs=300
nmax=300
dryrun=false

era_list=("2017" "2018" "2016postVFP" "2016preVFP")
joblist=("TT" "All" "--help")

# Enable dry run if second argument is --dryrun
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
  nmax=300
  for era in "${era_list[@]}"; do
    for suffix in 1 2 3 not1 not2 not3; do
      run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/QCD_${era}.txt -n 10 --nmax $nmax -e $era --userflags SeperateFake,ByType,LF,LFMother_${suffix} --skim SkimTree_FakeEventSkim &"
    done
    # Uncomment the base samples if needed:
    # run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/QCD_${era}.txt -n 10 --nmax $nmax -e $era --userflags SeperateFake,ByType,HFB --skim SkimTree_FakeEventSkim &"
    # run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/QCD_${era}.txt -n 10 --nmax $nmax -e $era --userflags SeperateFake,ByType,HFC --skim SkimTree_FakeEventSkim &"
    # run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/QCD_${era}.txt -n 10 --nmax $nmax -e $era --userflags SeperateFake,ByType,LF --skim SkimTree_FakeEventSkim &"
  done
}

run_tt() {
  nmax=400
  for era in "${era_list[@]}"; do
    run_cmd "SKFlat.py -a $analyzer -i TTLJ_powheg -n 600 --nmax $nmax -e $era --userflags SeperatePrompt,NoBJet &"
  done
}

run_all() {
  nmax=400
  for era in "${era_list[@]}"; do
    # Uncomment other samples as needed
    # run_cmd "SKFlat.py -a $analyzer -l ${sigpath}/DY.txt -n 10 --nmax $nmax -e $era &"
    # run_cmd "SKFlat.py -a $analyzer -l ${sigpath}/VBF.txt -n 10 --nmax $nmax -e $era &"
    # run_cmd "SKFlat.py -a $analyzer -l ${sigpath}/SSWW.txt -n 10 --nmax $nmax -e $era &"
    # run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Prompt.txt -n 600 --nmax $nmax -e $era --userflags SeperatePrompt &"

    run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Fake.txt -n 300 --nmax $nmax -e $era --userflags SeperateFake,HFB --skim SkimTree_FakeEventSkim &"
    run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Fake.txt -n 300 --nmax $nmax -e $era --userflags SeperateFake,LFAll --skim SkimTree_FakeEventSkim &"

    # run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Fake.txt -n 300 --nmax $nmax -e $era --userflags SeperateFake --skim SkimTree_FakeEventSkim &"
    # run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Conv.txt -n 500 --nmax $nmax -e $era --userflags SeperateConv --skim SkimTree_ConvEventSkim &"
    # run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/CF.txt -n 1000 --nmax $nmax -e $era --userflags SeperateCF --skim SkimTree_CFEventSkim &"
  done
}

print_help() {
  echo "Available job modes:"
  for job in "${joblist[@]}"; do
    echo " - $job"
  done
}

# Dispatcher
case "$1" in
  "TT") run_tt ;;
  "All") run_all ;;
  "--help"|"") print_help ;;
  *) echo "Unknown job mode: '$1'. Use --help to list options." ;;
esac




