import os
import argparse

# Define constants
analyzer = "SkimTree_HNMultiLepBDT"
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

def run_gt36():
    for era in ["2018"]:
        for dataset in [
            "EGamma_GT36:D"
        ]:
            cmd = f"SKFlat.py -a {analyzer} -i {dataset} -n 10000 --nmax 600 -e {era} --skim  SkimTree_HNMultiLep&"
            run_command(cmd)

def run_signal():
    for era in era_list:
        for signal_file, jobs in [
            ("Signal.txt", 100),
            ("DYAdditionalBDT.txt", 20),
            ("DYOfficial.txt", 20),
            ("VBFOfficial.txt", 20),
            ("SSWWOfficial.txt", 20)
        ]:
            cmd = f"SKFlat.py -a {analyzer} -l {sigpath}/{signal_file} -n {jobs} --nmax {nmax} -e {era}&"
            run_command(cmd)

def run_weinberg():
    for era in era_list:
        cmd = f"SKFlat.py -a {analyzer} -l {sigpath}/Weinberg.txt -n {njobs} --nmax {nmax} -e {era} &"
        run_command(cmd)

def run_all():
    for era in era_list:
        for dataset, jobs in [
            (f"DATA_{era}.txt", njobs_data),
            (f"DATA_l_{era}.txt", njobs_data),
            ("MCXSmall.txt", 2),
            ("MCSmall.txt", 5),
            ("MCMedium.txt", 20),
            ("MCLarge.txt", 50),
            ("MCXLarge.txt", 200),
            (f"QCD_{era}El.txt", 2),
            (f"QCD_{era}Mu.txt", 2)
        ]:
            cmd = f"SKFlat.py -a {analyzer} -l {mcpath}/{dataset} -n {jobs} --nmax {nmax} -e {era} --skim  SkimTree_HNMultiLep&"
            run_command(cmd)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--Signal", action="store_true", help="Run Signal jobs")
    parser.add_argument("--GT36", action="store_true", help="Run GT36 jobs")
    parser.add_argument("--Weinberg", action="store_true", help="Run Weinberg jobs")
    parser.add_argument("--All", action="store_true", help="Run All jobs")
    args = parser.parse_args()

    if args.Signal:
        run_signal()
    elif args.GT36:
        run_gt36()
    elif args.Weinberg:
        run_weinberg()
    elif args.All:
        run_all()
    else:
        print("No valid argument provided. Use --Signal, --GT36, --Weinberg, or --All.")
