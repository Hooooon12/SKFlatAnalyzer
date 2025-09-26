#!/bin/bash

MASS=$1
FLAV=$2
ERA=$3
NBIN=$4

echo "[INFO] CWD: $(pwd)"
echo "[INFO] Files:"
ls -l

echo "[INFO] Running scan for MASS=${MASS}, FLAV=${FLAV}, ERA=${ERA}, NBIN=${NBIN}"

/usr/bin/env python3 sr2_fom_scan.py \
  --mass ${MASS} \
  --flavour ${FLAV} \
  --era ${ERA} \
  --n_bins ${NBIN} \
