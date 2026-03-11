#!/bin/bash

set -e  # Optional: exit script if any command fails

analyzer="GetEffLumi"
rundir="GetEffLumi"
sigpath="${SKFlat_WD}/runJobs/SampleLists/Signals"
nmax=1000
njobs_sig=2
skim="SkimTree_HNMultiLepBDT"

declare -a era_list=("2018" "2016postVFP" "2016preVFP" "2017")

echo "[INFO] Starting SKFlat jobs for analyzer: $analyzer"
echo "[INFO] Signal path: $sigpath"
echo "[INFO] Skim: $skim"
echo ""

for era in "${era_list[@]}"; do
    echo "[INFO] Submitting jobs for era: $era"
    SKFlat.py -a "$analyzer" \
              -i "WZTo3LNu_amcatnlo" \
              -n "$njobs_sig" \
              --nmax "$nmax" \
              -e "$era" &
done

wait
echo "[INFO] All jobs submitted and background processes completed."

