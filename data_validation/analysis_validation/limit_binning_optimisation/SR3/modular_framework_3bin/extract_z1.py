#!/usr/bin/env python3

import re
import sys

logfile = sys.argv[1]

header_re = re.compile(r'\|\s*(\w+)\s*\|\s*mass=(\d+)\s*\|\s*MET=(\d+)')
region_name_re = re.compile(r'REGION:\s*(\S+)')
region_z_re = re.compile(r'Region Z = ([0-9.]+)')
bin_line_re = re.compile(r'^\s*\d+\s+\|')  # lines starting with bin index

current = None
current_region = None
bin_count = 0

with open(logfile) as f:
    for line in f:

        # Header
        h = header_re.search(line)
        if h:
            flavour = h.group(1)
            mass = h.group(2)
            met = h.group(3)
            current = (mass, flavour, met)
            continue

        # New region
        rname = region_name_re.search(line)
        if rname:
            current_region = rname.group(1)
            bin_count = 0   # reset counter
            continue

        # Count bins
        if bin_line_re.search(line):
            bin_count += 1
            continue

        # Region Z
        rz = region_z_re.search(line)
        if rz and current and current_region:
            z = rz.group(1)
            print(f"{bin_count} bin {current[0]} {current[1]} MET{current[2]} {current_region} Z={z}")
