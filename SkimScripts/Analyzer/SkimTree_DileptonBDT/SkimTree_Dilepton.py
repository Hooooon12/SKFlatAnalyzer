import os
import argparse

# Define constants
analyzer = "SkimTree_DileptonBDT"

qrundir = "runSkims"

skflat_wd = os.getenv("SKFlat_WD", "")  # Get SKFlat_WD environment variable

mcpath   = f"{skflat_wd}/SkimScripts/SampleLists/Bkg/"
datapath = f"{skflat_wd}/SkimScripts/SampleLists/Data/"
sigpath  = f"{skflat_wd}/SkimScripts/SampleLists/Signals/"
njobs = 1000
njobs_data = 2000
nmax = 400
era_list = ["2016postVFP", "2016preVFP", "2017", "2018"]

def run_command(command):
    print(f"Executing: {command}")
    os.system(command)  # Execute the command

def run_gt36():
    for era in ["2018"]:
        for dataset in [
                #"DoubleMuon_GT36:A", "DoubleMuon_GT36:B", "DoubleMuon_GT36:C", "DoubleMuon_GT36:D",
                #"MuonEG_GT36:A", "MuonEG_GT36:B", "MuonEG_GT36:C", "MuonEG_GT36:D",
                #"EGamma_GT36:A", "EGamma_GT36:B", "EGamma_GT36:C",
                "EGamma_GT36:D"
        ]:
            cmd = f"SKFlat.py -a {analyzer} -i {dataset} -n 10000 --nmax 600 -e {era} &"
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
            cmd = f"SKFlat.py -a {analyzer} -l {mcpath}/{dataset} -n {jobs} --nmax {nmax} -e {era} &"
            run_command(cmd)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--GT36", action="store_true", help="Run GT36 jobs")
    parser.add_argument("--All", action="store_true", help="Run All jobs")
    args = parser.parse_args()

    if args.GT36:
        run_gt36()
    elif args.All:
        run_all()
    else:
        print("No valid argument provided. Use --Signal, --GT36, --Weinberg, or --All.")
