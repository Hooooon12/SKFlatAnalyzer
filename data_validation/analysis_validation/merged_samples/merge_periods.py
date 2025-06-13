import os
import argparse
import subprocess

import logging


# A helper function to add clear breaks in logging                                                                                                                                                                  
def log_section_start(section_name):
    logging.info("-" * 50)  # Print 50 '=' characters as a separator                                                                                                                                                
    logging.info(f"STARTING SECTION: {section_name}")
    logging.info("-" * 50)

eras = ["2016preVFP", "2016postVFP", "2017", "2018"]
base_path = "/data6/Users/jalmond/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter"

# Periods for merging (data-specific)
merge_periods = {
    "2016preVFP": ["B_ver2", "C", "D", "E", "F"],
    "2016postVFP": ["F", "G", "H"],
    "2017": ["B", "C", "D", "E", "F"],
    "2018": ["A", "B", "C", "D"]
}

for era in eras:

    log_section_start(f"Merging {era}")
    
    # Make the era directory if it doesn't exist
    era_path = os.path.join(base_path, era)
    os.makedirs(era, exist_ok=True)

    # Get the periods to merge for this era
    periods_to_merge = merge_periods.get(era, [])

    # Loop over each period in the era
    for period in periods_to_merge:
        log_section_start(f"Merging {era}:{period}")
        
        # Construct the input file pattern
        input_files = os.path.join(base_path, era, f"MultiLepton__/DATA/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_*_{period}.root")

        # Construct the output file name
        output_file = f"{era}/HNL_ControlRegion_Plotter_{period}_Data.root"

        # Construct and run the hadd command
        cmd = f"hadd {output_file} {input_files}"
        print(f"Running: {cmd}")
        subprocess.run(cmd, shell=True, check=True)

