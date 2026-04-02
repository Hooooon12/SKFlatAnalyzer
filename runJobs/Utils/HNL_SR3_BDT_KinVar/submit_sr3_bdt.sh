#!/bin/bash

analyzer="HNL_SR3_BDT_KinVar"
rundir="HNL_SR3KinVar"
sigpath="${SKFlat_WD}/runJobs/SampleLists/Signals/Private/"
mcpath="${SKFlat_WD}/runJobs/SampleLists/Bkg/Fake/"

njobs=250
nmax=250
skim="--skim SkimTree_HNMultiLepBDT"
fakeskim="--skim SkimTree_FakeEventSkimBDT"
era_list=("2016postVFP" "2016preVFP" "2017" "2018")
era_list=("2018")
dryrun=false

# Enable dry run if second argument is --dryrun
if [[ "$2" == "--dryrun" ]]; then
    dryrun=true
fi

# Command runner
run_cmd() {
    if $dryrun; then
        echo "[DryRun] $*"
    else
        eval "$@"
    fi
}

# Default job submission
run_default() {
    for era in "${era_list[@]}"; do
        run_cmd "SKFlat.py -a $analyzer -l ${sigpath}/DY1.txt -n 5 --nmax $nmax -e $era $skim&"
        run_cmd "SKFlat.py -a $analyzer -l ${sigpath}/VBF1.txt -n 5 --nmax $nmax -e $era $skim&"
        #run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/FakeOptAll.txt -n 100 --nmax $nmax --userflags RunFake  -e $era $fakeskim &"
	#run_cmd "SKFlat.py -a $analyzer -i TTLJ_powheg  -n 100 --nmax $nmax --userflags RunFake  -e $era $fakeskim &"
    done
}

# Dispatch based on argument
case "$1" in
    "") run_default ;;
    *) echo "Unknown option: $1"; exit 1 ;;
esac

