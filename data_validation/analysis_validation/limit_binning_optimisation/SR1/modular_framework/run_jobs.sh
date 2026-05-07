#!/bin/bash

python sr1_fom_scan_Run2_Paper.py --config config.full_scan > logs/runlogs/logs_full_scan.txt &
python sr1_fom_scan_Run2_Paper.py --config config.fom_unc > logs/runlogs/logs_fom_unc.txt &

wait

python sr1_fom_scan_Run2_Paper.py --config config.full_scan_loose_stat > logs/runlogs/logs_loose.txt &
python sr1_fom_scan_Run2_Paper.py --config config.full_scan_9bin > logs/runlogs/logs_9bin.txt &

wait
