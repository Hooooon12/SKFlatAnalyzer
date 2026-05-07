#!/bin/bash

> fom_scan_list.txt  # overwrite file

MASSES=(85 90 95 100 125 150 200 250 300 350 400 450  500)
FLAVOURS=(MuMu EE EMu)
NJOBS=1
NTRIALS=1000000

for mass in "${MASSES[@]}"; do
    for flav in "${FLAVOURS[@]}"; do
	for ((job=0; job<${NJOBS}; job++)); do
	    echo "$mass $flav $NTRIALS $job" >> fom_scan_list.txt
	done
    done
    done
