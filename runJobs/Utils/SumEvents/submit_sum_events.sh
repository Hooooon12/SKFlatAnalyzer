#!/bin/bash

# === Config ===
analyzer=SumEvents
rundir=SumEvents
nmax=600
declare -a era_list=("2018" "2016postVFP" "2016preVFP" "2017")

# === Paths ===
sigpath=${SKFlat_WD}/runJobs/HNL/SumEvents/Signals/
mcpath_bdt=${SKFlat_WD}/runJobs/HNL/SumEvents/Bkg/SkimTree_HNMultiLepBDT/Bkg/
datapath_bdt=${SKFlat_WD}/runJobs/HNL/SumEvents/DATA/

mcpath_dilep=${SKFlat_WD}/runJobs/HNL/SumEvents/Bkg/SkimTree_Dilepton/
datapath_dilep=${SKFlat_WD}/runJobs/HNL/SumEvents/data_lists_multilep/SkimTree_Dilepton/

mcpath_fake=${SKFlat_WD}/runJobs/HNL/SumEvents/HNFakeBkg/
datapath_fake=${SKFlat_WD}/runJobs/HNL/SumEvents/data_lists_multilep/

# === Parse Arguments ===
run_bdt=false
run_dilepton=false
run_fake=false

for arg in "$@"; do
  case $arg in
    --bdt) run_bdt=true ;;
    --dilepton) run_dilepton=true ;;
    --fake) run_fake=true ;;
    --all) run_bdt=true; run_dilepton=true; run_fake=true ;;
    *) echo "Unknown option: $arg"; exit 1 ;;
  esac
done

if ! $run_bdt && ! $run_dilepton && ! $run_fake; then
  echo "No section selected. Use --bdt, --dilepton, --fake, or --all"
  exit 1
fi

# === Main Loop ===
for era in "${era_list[@]}"; do
  echo "Processing era: ${era}"

  if $run_bdt; then
    echo "[Running BDT section]"
    SKFlat.py -a $analyzer -l ${sigpath}/Signal.txt -n 1 --nmax ${nmax} -e ${era} &

    # Example MC (uncomment what you need)
    # SKFlat.py -a $analyzer -l ${mcpath_bdt}/MCSmall.txt -n 1 --nmax ${nmax} -e ${era} --skim SkimTree_HNMultiLepBDT &

    # Example Data (optional)
    # SKFlat.py -a $analyzer -l ${datapath_bdt}/DATA_${era}.txt -n 1 --nmax ${nmax} -e ${era} --skim SkimTree_HNMultiLepBDT &
  fi

  if $run_dilepton; then
    echo "[Running DiLepton section]"
    SKFlat.py -a $analyzer -l ${mcpath_dilep}/MC2.txt -n 1 --nmax ${nmax} -e ${era} --skim SkimTree_Dilepton &
    SKFlat.py -a $analyzer -l ${mcpath_dilep}/MC2.txt -n 1 --nmax ${nmax} -e ${era} --skim SkimTree_DileptonBDT &
  fi

  if $run_fake; then
    echo "[Running Fake section]"
    SKFlat.py -a $analyzer -l ${datapath_fake}/DATA_${era}_mu.txt -n 1 --nmax ${nmax} -e ${era} --skim SkimTree_HNFake &
    SKFlat.py -a $analyzer -l ${datapath_fake}/DATA_${era}_el.txt -n 1 --nmax ${nmax} -e ${era} --skim SkimTree_HNFake &
    SKFlat.py -a $analyzer -l ${mcpath_fake}/MC2.txt              -n 1 --nmax ${nmax} -e ${era} --skim SkimTree_HNFake

    SKFlat.py -a $analyzer -l ${datapath_fake}/DATA_${era}_mu.txt -n 1 --nmax ${nmax} -e ${era} --skim SkimTree_HNFakeBDT &
    SKFlat.py -a $analyzer -l ${datapath_fake}/DATA_${era}_el.txt -n 1 --nmax ${nmax} -e ${era} --skim SkimTree_HNFakeBDT &
    SKFlat.py -a $analyzer -l ${mcpath_fake}/MC2.txt              -n 1 --nmax ${nmax} -e ${era} --skim SkimTree_HNFakeBDT
  fi

done

wait
echo "All selected jobs submitted."
