import os
import argparse

# Define constants
analyzer = "SkimTree_SSDileptonBDT"
qrundir = "runSkims"
skflat_wd = os.getenv("SKFlat_WD", "")  # Get SKFlat_WD environment variable
mcpath = f"{skflat_wd}/SkimScripts/SampleLists/Bkg/"
datapath = f"{skflat_wd}/SkimScripts/SampleLists/Data/"
sigpath = f"{skflat_wd}/SkimScripts/SampleLists/Signals/"
njobs = 600
njobs_data = 100
nmax = 350
era_list = ["2016postVFP", "2016preVFP", "2017", "2018"]

def run_command(command):
    print(f"Executing: {command}")
    os.system(command)  # Execute the command

def run_zz():
    for era in era_list:
        cmd = f"SKFlat.py -a {analyzer} -i ZZTo4L_powheg -n {njobs} --nmax {nmax} -e {era} --skim SkimTree_SSDilepton &"
        run_command(cmd)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--ZZ", action="store_true", help="Run ZZ jobs")
    args = parser.parse_args()

    if args.ZZ:
        run_zz()
    else:
        print("No valid argument provided..")
