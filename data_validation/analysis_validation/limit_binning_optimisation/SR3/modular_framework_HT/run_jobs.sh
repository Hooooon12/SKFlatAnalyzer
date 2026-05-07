#!/bin/bash

python sr3_fom_scan_Run2_Paper.py --config config.full_scan > logs/runlogs/logs_full_scan.txt

python sr3_fom_scan_Run2_Paper.py --config config.full_scan_loose_stat > logs/runlogs/logs_loose.txt

python sr3_fom_scan_Run2_Paper.py --config config.full_scan_nbin > logs/runlogs/logs_nbin.txt
