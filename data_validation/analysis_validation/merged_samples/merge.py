import os
import sys
import argparse
import subprocess
import shutil
import logging
from pathlib import Path

from datetime import datetime


Analyzer="HNL_ControlRegionLLL"
skim="SkimTree_HNMultiLepBDT"
skim_dilep="SkimTree_DileptonBDT"


    
# A helper function to add clear breaks in logging


def log_section_start(section_name):
    logging.info("=" * 50)  # Print 50 '=' characters as a separator                                                                                                                                                                                                                                                                                                                        
    logging.info(f"STARTING SECTION: {section_name}")
    logging.info("=" * 50)

def log_subsection_start(section_name):
    logging.info("-" * 50)  # Print 50 '=' characters as a separator                                                                                                                                                                                                                                                                                                                         
    logging.info(f"STARTING SUBSECTION: {section_name}")
    logging.info("-" * 50)
    


logging.basicConfig(
    level=logging.INFO,
    format='HaddJob:  %(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler(f'process_hadd.log'),  # Log to a file                                                                                                                                      
                                                                                                                                                                                                                   
        logging.StreamHandler()  # Log to console (screen)
    ]
       
)
eras = ["2016preVFP", "2016postVFP", "2017", "2018"]

parser = argparse.ArgumentParser(description="Example script with string flag")
# Add a string flag, like --mode somevalue
parser.add_argument('--mode', type=str, default='default', help='Mode of operation (e.g., test, prod, debug)')
parser.add_argument("--analyzer", choices=["HNL_ControlRegionTwo", "HNL_ControlRegionOne", "HNL_ControlRegionLLL","HNL_ControlRegion_Plotter"], required=True, help="Select the analyzer.")

args = parser.parse_args()

if args.analyzer == "HNL_ControlRegionTwo":
    Analyzer = "HNL_ControlRegionTwo"
    skim = "SkimTree_HNMultiLepBDT"
    skim_dilep = "SkimTree_DileptonBDT"
elif args.analyzer == "HNL_ControlRegionOne":
    Analyzer = "HNL_ControlRegionOne"
    skim = "SkimTree_AK8BDT"
    skim_dilep = "SkimTree_AK8BDT"

elif args.analyzer == "HNL_ControlRegionLLL":
    Analyzer = "HNL_ControlRegionLLL"
    skim = "SkimTree_HNMultiLepBDT"
    skim_dilep = "SkimTree_DileptonBDT"

elif args.analyzer == "HNL_ControlRegion_Plotter":
    Analyzer = "HNL_ControlRegion_Plotter"
    skim = "SkimTree_HNMultiLepBDT"
    skim_dilep = "SkimTree_DileptonBDT"

    
# Create a string with the analyzer name and current date
date_str = datetime.now().strftime("%Y-%m-%d")
analyzer_with_date = f"{Analyzer}_{date_str}"

eras = ["2016preVFP", "2016postVFP", "2017", "2018"]
base_path = f"/data6/Users/jalmond/SKFlatOutput/Run2UltraLegacy_v3/{Analyzer}"


# Output the result
print(f"Analyzer: {analyzer_with_date}")
print(f"Skim: {skim}")
print(f"Skim Dilep: {skim_dilep}")

print(f"Selected mode: {args.mode}")

mode=""

if args.mode != "default":
    mode=args.mode+"__"


Plot_Version=analyzer_with_date

commands_template = [

    
    ("Conv", [
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_ZGToLLG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_ZGToLLG_PtG_130.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_DYJets10to50_MG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_DYJets_MG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGToLNuG_MG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGToLNuG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGToLNuG_01J_PtG_130.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGToLNuG_01J_PtG_300.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGToLNuG_01J_PtG_500.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGJJToLNu.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_TTG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_WWG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_WZG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_TG.root",
    ]),

    ("ZG", [
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_ZGToLLG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_ZGToLLG_PtG_130.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_DYJets10to50_MG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_DYJets_MG.root",
        ]),
    ("CF", f"{mode}MultiLepton__RunCF__/DATA/{Analyzer}_{skim_dilep}_*"),
    ("Fake", f"{mode}MultiLepton__RunFake__/DATA/{Analyzer}_{skim}_*"),
    ("Data", f"{mode}MultiLepton__/DATA/{Analyzer}_{skim}_*"),

    ("WG", [
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGToLNuG_MG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGToLNuG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGToLNuG_01J_PtG_130.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGToLNuG_01J_PtG_300.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGToLNuG_01J_PtG_500.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim_dilep}_WGJJToLNu.root",
    ]),
    ("Other_Conv", [
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_TTG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_WWG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_WZG.root",
        f"{mode}MultiLepton__RunConv__/{Analyzer}_{skim}_TG.root",
    ]),

    ("ZZ", [
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto4e.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto2e2tau.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto2e2mu.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto4mu.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto2mu2tau.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluHToZZTo4L.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_VBF_HToZZTo4L.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto4tau.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_ZZTo4L_powheg.root",
     ]),

    ("ggZZ", [
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto4e.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto2e2tau.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto2e2mu.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto4mu.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto2mu2tau.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluHToZZTo4L.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_VBF_HToZZTo4L.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_GluGluToZZto4tau.root"
    ]),
    ("qqZZ", [f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_ZZTo4L_powheg.root"]),
    ("TTV", [
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_ttZToLLNuNu.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_ttWToLNu.root"
    ]),
    ("WW", [
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_WpWp_EWK.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_WpWp_QCD.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_WWTo2L2Nu_DS.root"
    ]),
    
    ("Other", [
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_tZq.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_TTTT.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_ttHToNonbb.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_VHToNonbb.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_TTZZ.root"
    ]),
    ("Extra", [
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_tHq.root",
    ]),
    ("VVV", [
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_WZZ.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_ZZZ.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_WWZ.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_WWW.root"
    ]),
    ("WZ", [
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_WZ_EWK.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_WZTo3LNu_mllmin4p0_powheg.root"
    ]),
    ("WZAlt", [
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_WZ_EWK.root",
        f"{mode}MultiLepton__RunPrompt__/{Analyzer}_{skim}_WZTo3LNu_amcatnlo.root"
    ])

]

if Analyzer=="HNL_ControlRegionLLL":
    commands_template.remove(("CF", f"{mode}MultiLepton__RunCF__/DATA/{Analyzer}_{skim_dilep}_*"))

for era in eras:
    era_path = os.path.join(base_path, era)

    if os.path.exists(era):
        if os.path.isdir(era):  # Check if it's a directory
            logging.info(f"Removing old directory {era}")
            shutil.rmtree(era)  # Removes a directory and all its contents

    os.makedirs(era, exist_ok=True)  # Ensure output directory exists


    log_section_start(f"Checking {era}")
    for tag, input_files in commands_template:
        
        output_file = f"{era}/{Analyzer}_{tag}.root"
        log_subsection_start(f"Checking inputs to {output_file}")
        
        # If the input is a wildcard pattern (string), expand with full path                                                                                                                                        
        if not isinstance(input_files, str):
            # Check if files exist if input_files is not a wildcard                                                                                                                                                 
            input_paths = [os.path.join(era_path, f) for f in input_files]
            # Ensure all files exist                                                                                                                                                                                
            missing_files = [f for f in input_paths if not os.path.isfile(f)]
            if missing_files:
                logging.warning(f"Error: The following files do not exist: {', '.join(missing_files)}")
                sys.exit(1)

    
    log_section_start(f"Merging {era}")
   
    for tag, input_files in commands_template:
       
        output_file = f"{era}/{Analyzer}_{tag}.root"
        log_subsection_start(f"Merging -->{output_file}")
        
        # If the input is a wildcard pattern (string), expand with full path
        if isinstance(input_files, str):
            input_path = os.path.join(base_path, era, input_files)
            cmd = f"hadd {output_file} {input_path}"
            # If the tag is 'Data', copy the files as well
            if tag == "Data":
                cmd_cp = f"cp {input_path} {era}/"
                logging.info(f"Running: {cmd_cp}")
                subprocess.run(cmd_cp, shell=True, check=True)
        else:

            # Check if files exist if input_files is not a wildcard
            input_paths = [os.path.join(era_path, f) for f in input_files]

            cmd = f"hadd {output_file} " + " ".join(input_paths)
            # If the tag is 'Data', copy the files as well
            if tag == "Data":
                cmd_cp = f"cp " + " ".join(input_paths) + f" {era}/"
                logging.info(f"Running: {cmd_cp}")
                subprocess.run(cmd_cp, shell=True, check=True)

        logging.info(f"Running: {cmd}")
        subprocess.run(cmd, shell=True, check=True)


# move outfiles to plot version directory

# Define target directory
target_dir = Path(Plot_Version)

# Delete if it already exists
if target_dir.exists() and target_dir.is_dir():
    shutil.rmtree(target_dir)

# Create the directory
target_dir.mkdir(parents=True, exist_ok=True)


# Move all directories starting with "20" into the new directory
for entry in os.listdir():
    if entry.startswith("20") and os.path.isdir(entry):
        shutil.move(entry, target_dir / entry)



        
