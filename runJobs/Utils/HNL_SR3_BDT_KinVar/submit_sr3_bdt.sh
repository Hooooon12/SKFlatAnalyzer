#!/bin/bash

analyzer="HNL_SR3KinVar"
rundir="HNL_SR3KinVar"
sigpath="${SKFlat_WD}/runJobs/${analyzer}/Signals/"
mcpath="${SKFlat_WD}/runJobs/${analyzer}/Bkg/"
njobs=250
nmax=250
skim="--skim SkimTree_HNMultiLep"
era_list=("2016postVFP" "2016preVFP" "2017" "2018")
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
        run_cmd "SKFlat.py -a $analyzer -l ${sigpath}/DY.txt -n 5 --nmax $nmax -e $era &"
        run_cmd "SKFlat.py -a $analyzer -l ${sigpath}/VBF.txt -n 5 --nmax $nmax -e $era &"
        run_cmd "SKFlat.py -a $analyzer -l ${sigpath}/SSWW.txt -n 5 --nmax $nmax -e $era &"
        run_cmd "SKFlat.py -a $analyzer -l ${mcpath}/${era}/MC.txt -n 25 --nmax $nmax -e $era $skim &"
    done
}

# Dispatch based on argument
case "$1" in
    "") run_default ;;
    *) echo "Unknown option: $1"; exit 1 ;;
esac

