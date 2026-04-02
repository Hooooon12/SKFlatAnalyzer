#!/bin/bash

MASS=$1
FLAV=$2
ERA=$3
NBINS=$4
NBINMINS=$5

echo "[INFO] CWD: $(pwd)"
echo "[INFO] Files:"
ls -l

echo "[INFO] Running scan for MASS=${MASS}, FLAV=${FLAV}, ERA=${ERA}, NBINS=${NBINS}, NBINS=${NBINMINS}"

/usr/bin/env python3 merge_strict_fom.py \
  --mass ${MASS} \
  --flavour ${FLAV} \
  --era ${ERA} \
  --n_bins_final ${NBINS} \
  --n_bins_min ${NBINMINS}
