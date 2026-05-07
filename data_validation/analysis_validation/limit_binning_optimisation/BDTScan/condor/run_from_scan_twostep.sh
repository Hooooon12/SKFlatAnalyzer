#!/bin/bash

MASS=$1
FLAV=$2
n_trials=$3
job_id=$4

echo "[INFO] CWD: $(pwd)"
echo "[INFO] Files:"
ls -l

echo "[INFO] Running scan for MASS=${MASS}, FLAV=${FLAV}, n_trials=${n_trials}, job_id=${job_id}"

/usr/bin/env python3 scan_run2_twostep.py \
  --mass ${MASS} \
  --flavour ${FLAV} \
  --n_trials ${n_trials} \
  --job_id ${job_id} \

  


