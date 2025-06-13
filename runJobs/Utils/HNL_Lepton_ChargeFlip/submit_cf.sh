#!/bin/bash

analyzer="HNL_Lepton_ChargeFlip"
rundir="HNL_Lepton_ChargeFlip"
datapath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Data/"
mcpath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/"
njobs=250
njobs_data=100
nmax=400
dryrun=false

era_list=("2016postVFP" "2016preVFP" "2017" "2018")
joblist=("Closure" "Rates" "SF" "Shift" "--help")

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

run_closure() {
  for era in "${era_list[@]}"; do
    run_cmd "SKFlat.py -a $analyzer -i DYJetsToEE_MiNNLO -n 200 --nmax 200 -e $era --skim SkimTree_DileptonBDT --userflags ClosureTest &"
    run_cmd "SKFlat.py -a $analyzer -i TTLL_powheg       -n 200 --nmax 200 -e $era --skim SkimTree_DileptonBDT --userflags ClosureTest &"
  done
}

run_rates() {
  for era in "${era_list[@]}"; do
    samples=(
      DYJets_MG_HT-70to100 DYJets_MG_HT-100to200 DYJets_MG_HT-200to400
      DYJets_MG_HT-400to600 DYJets_MG_HT-600to800 DYJets_MG_HT-800to1200
      DYJets_MG_HT-1200to2500 DYJets_MG_HT-2500toInf DYJets_Pt-50To100
      DYJets_Pt-100To250 DYJets_Pt-250To400 DYJets_Pt-400To650 DYJets_Pt-650ToInf
      WJets_MG DYJets DYJets_MG DYJetsToEE_MiNNLO TTLJ_powheg TTLL_powheg
    )
    for sample in "${samples[@]}"; do
      skim="SkimTree_DileptonBDT"
      [[ "$sample" == "WJets_MG" || "$sample" == "TTLJ_powheg" ]] && skim="SkimTree_BDT"
      run_cmd "SKFlat.py -a $analyzer -i $sample -n 100 --nmax 400 -e $era --skim $skim --userflags ElCFRates &"
    done
  done
}

run_sf() {
  for era in "${era_list[@]}"; do
    run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_DiLepton_EE.txt -n $njobs_data --nmax 600 -e $era --skim SkimTree_DileptonBDT --userflag ScaleFactorTop &"
    run_cmd "SKFlat.py -a $analyzer -i DYJetsToEE_MiNNLO -n 300 --nmax 600 -e $era --skim SkimTree_DileptonBDT --userflag ScaleFactorTop &"
    # Additional commented commands preserved below
    # run_cmd "SKFlat.py -a $analyzer -i ZGToLLG -n 100 --nmax 500 -e $era --skim SkimTree_HNMultiLepBDT --userflag ScaleFactor &"
    # run_cmd "SKFlat.py -a $analyzer -i WGToLNuG -n 100 --nmax 500 -e $era --skim SkimTree_HNMultiLepBDT --userflag ScaleFactor &"
  done
}

run_shift() {
  for era in "${era_list[@]}"; do
    run_cmd "SKFlat.py -a $analyzer -i DYJets -n 600 --nmax 500 -e $era --skim SkimTree_DileptonBDT --userflags ShiftEnergy &"
    run_cmd "SKFlat.py -a $analyzer -i DYJetsToEE_MiNNLO -n 600 --nmax 500 -e $era --skim SkimTree_DileptonBDT --userflags ShiftEnergy"
    run_cmd "SKFlat.py -a $analyzer -i TTLL_powheg -n 600 --nmax 500 -e $era --skim SkimTree_DileptonBDT --userflags ShiftEnergy &"
    run_cmd "SKFlat.py -a $analyzer -i TTLJ_powheg -n 600 --nmax 500 -e $era --skim SkimTree_DileptonBDT --userflags ShiftEnergy"
    run_cmd "SKFlat.py -a $analyzer -l runJobs/SampleLists/Bkg/CFSkimList.txt -n 600 --nmax 500 -e $era --skim SkimTree_CFEventSkim --userflags ShiftEnergy"
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
  "Closure") run_closure ;;
  "Rates") run_rates ;;
  "SF") run_sf ;;
  "Shift") run_shift ;;
  "--help"|"") print_help ;;
  *) echo "Unknown job mode: '$1'. Use --help to list options." ;;
esac
