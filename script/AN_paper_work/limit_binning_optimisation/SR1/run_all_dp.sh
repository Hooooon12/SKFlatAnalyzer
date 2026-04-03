#!/bin/bash

SCRIPT="scan_sr1_dp.py"

masses=(450)
#500 600 700 800 900 1000)
flavours=(EE EMu MuMu)
eras=(2016preVFP 2016postVFP 2017 2018)

n_bins=7

for era in "${eras[@]}"; do
  for flav in "${flavours[@]}"; do
    for mass in "${masses[@]}"; do
      echo "[INFO] Running: python $SCRIPT --era $era --flavour $flav --n_bins $n_bins --mass $mass"
      python $SCRIPT --era "$era" --flavour "$flav" --n_bins "$n_bins" --mass "$mass"
    done
  done
done
