#!/bin/bash

analyzer="HNL_Lepton_ID_BDT_Studies"
rundir="HNL_Lepton_ID_BDT_Studies"
datapath="${SKFlat_WD}/runJobs/HNL/${analyzer}/Data/"
mcpath="${SKFlat_WD}/runJobs/SampleLists/Bkg/Fake/"
njobs=250
nmax=800
skim="SkimTree_Dilepton"
dryrun=false

era_list=("2018" "2016postVFP" "2016preVFP" "2017")

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

run_BDT() {
    local era_list=("2018")
    for era in "${era_list[@]}"; do
        run_cmd "SKFlat.py -a $analyzer -i DYJetsToMuMu_MiNNLO -n 400 --nmax 400 -e $era --skim SkimTree_DileptonBDT --userflags BDTPerType,BDTPlots,BDT &"
        run_cmd "SKFlat.py -a $analyzer -i DYJetsToEE_MiNNLO   -n 400 --nmax 400 -e $era --skim SkimTree_DileptonBDT --userflags BDTPerType,BDTPlots,BDT &"
        # run_cmd "SKFlat.py -a $analyzer -i TTLJ_powheg -n 400 --nmax 400 -e $era --skim SkimTree_DileptonBDT --userflags BDTPerType,BDTPlots,BDT &"
        # run_cmd "SKFlat.py -a $analyzer -l ${datapath}/DATA_${era}.txt -n 100 --nmax 400 --userflag BDTPerType,BDTPlots,BDT -e $era --skim SkimTree_DileptonBDT &"
    done
}

run_BDTType() {
    local era_list=("2018")
    for era in "${era_list[@]}"; do
        run_cmd "SKFlat.py -a $analyzer -i DYJetsToMuMu_MiNNLO -n 500 --nmax 500 -e $era --skim SkimTree_DileptonBDT --userflags BDTPerType &"
        run_cmd "SKFlat.py -a $analyzer -i DYJetsToEE_MiNNLO   -n 500 --nmax 500 -e $era --skim SkimTree_DileptonBDT --userflags BDTPerType &"
        run_cmd "SKFlat.py -a $analyzer -i TTLJ_powheg         -n 200 --nmax 500 -e $era --skim SkimTree_DileptonBDT --userflags BDTPerType &"
    done
}

run_default() {
    local era_list=("2017")
    for era in "${era_list[@]}"; do
        run_cmd "SKFlat.py -a $analyzer -i TTLJ_powheg -n 300 --nmax 300 -e $era --userflag SSBreakdown --skim SkimTree_FakeEventSkimBDT &"
        # run_cmd "SKFlat.py -a $analyzer -i DYJets -n 200 --nmax 300 -e $era --userflag SSBreakdown --skim SkimTree_FakeEventSkimBDT &"
        # run_cmd "SKFlat.py -a $analyzer -i WJets_MG -n 200 --nmax 300 -e $era --userflag SSBreakdown --skim SkimTree_FakeEventSkimBDT &"
        run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/QCD_${era}.txt -n 10 --nmax 300 --userflag SSBreakdown -e $era --skim SkimTree_FakeEventSkimBDT &"
    done
}

run_OS() {
    local era_list=("2017")
    for era in "${era_list[@]}"; do
        run_cmd "SKFlat.py -a $analyzer -i DYJetsToEE_MiNNLO -n 200 --nmax 300 -e $era --userflag BDT --skim SkimTree_DileptonBDT &"
    done
}

run_All() {
    local era_list=("2016postVFP" "2016preVFP" "2017" "2018")
    for era in "${era_list[@]}"; do
        run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Fake.txt -n 250 --nmax 400 -e $era --skim SkimTree_FakeEventSkimBDT --userflags FakeSplit&"
        #run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/CF.txt   -n 100 --nmax 400 -e $era --skim SkimTree_CFEventSkim &"
        #run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Conv.txt -n 100 --nmax 400 -e $era --skim SkimTree_ConvEventSkim &"
        #run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/Sig.txt  -n 100 --nmax 400 -e $era --skim SkimTree_HNMultiLepBDT &"
    done
}

# Argument dispatch
case "$1" in
    "BDT") run_BDT ;;
    "BDTType") run_BDTType ;;
    "OS") run_OS ;;
    "All") run_All ;;
    "") run_default ;;
    *) echo "Unknown option: $1"; exit 1 ;;
esac



