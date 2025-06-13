import os
import argparse

# Define constants
analyzer = "SkimTree_AK8BDT"
qrundir = "runSkims"
skflat_wd = os.getenv("SKFlat_WD", "")  # Get SKFlat_WD environment variable
mcpath = f"{skflat_wd}/SkimScripts/SampleLists/Bkg/"
datapath = f"{skflat_wd}/SkimScripts/SampleLists/Data/"
sigpath = f"{skflat_wd}/SkimScripts/SampleLists/Signals/"
njobs = 600
njobs_data = 100
nmax = 400
era_list = ["2016postVFP", "2016preVFP", "2017", "2018"]

def run_command(command):
    print(f"Executing: {command}")
    os.system(command)  # Execute the command

def run_data():

   
    era_list = ["2018"]
    for era in era_list:
        for dataset, jobs, skimcmd in [
                (f"DATA_EE_{era}.txt", njobs_data," --skim SkimTree_AK8 " ),
                (f"DATA_EMu_{era}.txt", njobs_data," --skim SkimTree_AK8 "),
        ]:
            cmd = f"SKFlat.py -a {analyzer} -l {datapath}/{dataset} -n {jobs} --nmax {nmax} -e {era} {skimcmd} &"
            run_command(cmd)


def run_new():


    era_list = ["2016postVFP"]
    for era in era_list:

        cmd = f"SKFlat.py -a {analyzer} -i DYJets10to50_MG -n 200 --nmax {nmax} -e {era}  --skim SkimTree_AK8&"
        run_command(cmd)




            
def run_all():

    
    for era in era_list:
        for dataset, jobs in [
                (f"DATA_{era}.txt", njobs_data)
        ]:
            cmd = f"SKFlat.py -a {analyzer} -l {datapath}/{dataset} -n {jobs} --nmax {nmax} -e {era} --skim SkimTree_AK8 &"
            run_command(cmd)

        for dataset, jobs in [
                ("MCXSmall.txt", 40),
                ("MCSmall.txt", 40),
                ("MCMedium.txt", 50),
                ("MCLarge.txt", 75),
                ("MCXLarge.txt", 400)
        ]:
            cmd = f"SKFlat.py -a {analyzer} -l {mcpath}/{dataset} -n {jobs} --nmax {nmax} -e {era}  --skim SkimTree_AK8&"
            run_command(cmd)



            
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--All", action="store_true", help="Run All jobs")
    parser.add_argument("--New", action="store_true", help="Run All jobs")
    parser.add_argument("--Data", action="store_true", help="Run All jobs")
    args = parser.parse_args()

    if args.All:
        run_all()
    if args.New:
        run_new()
        
    if args.Data:
        run_data()        
    else:
        print("No valid argument provided. Use --Signal, --GT36, --Weinberg, or --All.")
