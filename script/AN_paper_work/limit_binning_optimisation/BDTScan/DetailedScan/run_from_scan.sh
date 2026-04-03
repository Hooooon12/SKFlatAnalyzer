#!/bin/bash

MASS=$1
FLAV=$2
ERA=$3
NBINS=$4
MAXITER=$5

echo "[INFO] CWD: $(pwd)"
echo "[INFO] Files:"
ls -l

echo "[INFO] Running scan for MASS=${MASS}, FLAV=${FLAV}, ERA=${ERA}, NBINS=${NBINS}, MAXITER=${MAXITER}"

/usr/bin/env python3 scan_fom.py \
  --mass ${MASS} \
  --flavour ${FLAV} \
  --era ${ERA} \
  --n_bins_scan ${NBINS} \
  --n_maxiter_scan ${MAXITER}
