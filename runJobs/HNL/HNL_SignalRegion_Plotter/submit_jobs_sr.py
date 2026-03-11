import os
import argparse


import time
import subprocess

def get_long_running_hadd_processes(min_minutes=20):
    try:
        # Get list of 'hadd' processes and their elapsed time in seconds
        output = subprocess.check_output(
            "ps -eo etimes,cmd | grep hadd | grep -v grep", shell=True
        ).decode("utf-8")

        long_hadds = []
        for line in output.strip().split('\n'):
            if not line.strip():
                continue
            parts = line.strip().split(None, 1)
            elapsed_seconds = int(parts[0])
            if elapsed_seconds >= min_minutes * 60:
                long_hadds.append((elapsed_seconds, parts[1]))
        return long_hadds

    except subprocess.CalledProcessError:
        return []


    

# Define paths using environment variable
base_path = os.getenv("SKFlat_WD")
sigpath = os.path.join(base_path, "runJobs/SampleLists/Signals/")
mcpath = os.path.join(base_path, "runJobs/SampleLists/Bkg/")
datapath = os.path.join(base_path, "runJobs/SampleLists/Data/")

# Default parameters
analyzer = "HNL_SignalRegion_Plotter"
rundir = analyzer
njobs = 30
njobs_sig = 5
njobs_sig_syst = 20
njobs_data = 100
nmax = 700
skim = ' '

era_list = ("2016postVFP", "2016preVFP", "2017", "2018")

# Argument parsing
parser = argparse.ArgumentParser(description='option')
parser.add_argument('-nmax', dest='nmax', default=600, type=int)
parser.add_argument('-sample', dest="samplename", default="NULL")
parser.add_argument('-njob', dest='njob', default=50, type=int)
parser.add_argument('-era', dest='era', default="NULL")
parser.add_argument('-skim', dest='skim', default="SkimTree_HNMultiLepBDT")
parser.add_argument('-flags', dest='flags', default="")

parser.add_argument('--preGT36', action='store_true')
parser.add_argument('--RunSignal', action='store_true')
parser.add_argument('--RunData', action='store_true')
parser.add_argument('--RunPrompt', action='store_true')
parser.add_argument('--RunConv',   action='store_true')
parser.add_argument('--RunFake',   action='store_true')
parser.add_argument('--RunCF',     action='store_true')

parser.add_argument('--Signal',      action='store_true')
parser.add_argument('--Central',     action='store_true')
parser.add_argument('--Systematics', action='store_true')
parser.add_argument('--Cutflow',     action='store_true')

parser.add_argument('--VERBOSE', action='store_true')
parser.add_argument('--Run', action='store_true')

args = parser.parse_args()

###### Set Era based on input: else 2018
Era=args.era
if args.era == "NULL":
    Era="2018"
    args.era = "2018"
else:
    era_list = [f"{args.era}"]


#Use GT36 samples by default                                                                                                                                                                                                                  
UseGT36 = not args.preGT36

    
# Determine if individual sample is provided
IndividualSample = args.samplename != "NULL"

if not IndividualSample:
    if not args.Signal and not args.Central and not args.Systematics and not args.Cutflow:
        print ("No inputs submitted. Run either :")
        print ("add -sample X --RunPrompt to run individual sample")
        print ("add --Central to run all jobs for SR (no systematics)")
        print ("add --Systematics to run all jobs for SR (with systematics)")
        print ("add --Signal to run all signal jobs for SR (no systematics)")
        print ("add --Signal -flags RunSyst to run all signal jobs for SR (with systematics)")
        print ("or --Signal --Systematics to run all signal jobs for SR (with systematics)")
        exit()

# Update nmax and njob from arguments
nmax = args.nmax
njob = args.njob

def split_by_comma(input_string):
    # Split the string by comma and return as a list

    if input_string == "":
        return []

    if ',' in input_string:
        return input_string.split(',')
    return [input_string]

def FlagCommand(JobType, InputFlags):
    flag = "--userflags "
    # Append each flag from the list to the command
    for x in InputFlags:
        flag += f"{x},"

    flag += f"{JobType}"

    return flag

def GetType(arg):


    if "Type" in arg.samplename:
        return 'RunSignal'
    if arg.RunSignal:
        return 'RunSignal'

    if arg.RunPrompt:
        return 'RunPrompt'
    if arg.RunConv:
        return 'RunConv'
    if arg.RunCF:
        return 'RunCF'        
    if arg.RunFake:
        return 'RunFake'

def SampleExists(args):
    
    samplename = args.samplename
    Era = args.era
    Type="RunBkg"
    if "Type" in samplename:
        Type="RunSignal"
    if args.RunSignal:
        Type="RunSignal"

    Sample_List=GetSampleList(samplename, Type, Era)
    if samplename in Sample_List:
        return True
    else:
        print(f"{samplename} does not exist. Full list of sample names are:")
        for Sample in Sample_List:
            print(Sample)
        print ("#"*50)
        for Sample in Sample_List:
            if samplename in Sample:
                print(f"close match {Sample}")

    return False
        

def GetSampleList(samplename, Type, Era):
    input_list = os.path.join(base_path, "data/Run2UltraLegacy_v3/"+Era+"/Sample/SampleSummary_MC.txt")

    if Type == "RunSignal":
        input_list = os.path.join(base_path, "data/Run2UltraLegacy_v3/"+Era+"/Sample/SampleSummary_Signal_Type1.txt")
        
    # Initialize an empty list to store the first words
    first_words = []
    
    # Open the file and loop through each line
    with open(input_list, 'r') as file:
        for line in file:
            # Split the line by whitespace and get the first word
            words = line.split()
            if words:  # Check if the line is not empty
                first_words.append(words[0])
                
    # Now you have a list of first words
    return first_words
        
def RunCommand(TestRun, command):
    print(f"{command}")

    if not TestRun:
        os.system(f"{command}")

    return
          


TestMode = not args.Run
if TestMode:
    print(f"Running in Test Mode: add --Run to submit ")

# Process flags                                                                                                                                                              
flags = split_by_comma(args.flags)


if args.Systematics:
    flags.insert(0,"RunSyst")
    
# Run command for individual sample if provided
if IndividualSample:
    if not args.RunSignal and not args.RunPrompt and not args.RunCF and not args.RunConv and not args.RunFake:
        print(f"Need to set RunType, i.e, RunPrompt, RunSignal....")
        exit()

    if SampleExists(args):
        SampleType=GetType(args)
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -i {args.samplename} -n {njob} --nmax {nmax} -e {args.era} --skim {args.skim} {FlagCommand(SampleType, flags)}&")
    exit()


    
if args.Signal:
    for era in era_list:
        # Run the signal commands
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/SSWW1.txt -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/SSWW2.txt -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/DY.txt   -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/VBF.txt  -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/Weinberg.txt  -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")

        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Official/SSWWOfficial.txt -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Official/DYOfficial.txt   -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Official/VBFOfficial.txt  -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")

        exit()

if args.RunData:

    for era in era_list:
        DATADir = "DL"
        if UseGT36 and era == "2018":
            DATADir = "DL_GT36"

        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_MuMu.txt -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunData', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EE.txt   -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunData', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EMu.txt  -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunData', flags)} &")
    exit()

if args.Central:

    ### DATA JOBS NOT CODED YET SINCE BLINDED

    for era in era_list:
        DATADir = "DL"
        if UseGT36 and era == "2018":
            DATADir = "DL_GT36"

        # Running background and fake data commands
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/SSWW1.txt -n 20    --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/SSWW2.txt -n 20    --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/DY.txt   -n 20    --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/VBF.txt  -n 20    --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/Weinberg.txt  -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")

        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_MuMu.txt -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunFake', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EE.txt   -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunFake', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EMu.txt  -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunFake', flags)} &")

        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS1.txt  -n 50  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunPrompt', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS3.txt  -n 50  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunPrompt', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS4.txt  -n 50  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunPrompt', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS5.txt  -n 50  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunPrompt', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS2.txt -n 200 --nmax {nmax} -e {era} --skim SkimTree_SSDileptonBDT {FlagCommand('RunPrompt', flags)} &")

        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Conv/Conv.txt   -n 50 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunConv', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Conv/ConvWG.txt -n 50 --nmax {nmax} -e {era} --skim SkimTree_DileptonBDT  {FlagCommand('RunConv', flags)} &")

        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EE.txt -n 200 --nmax {nmax} -e {era} --skim SkimTree_DileptonBDT {FlagCommand('RunCF', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EMu.txt -n 200 --nmax {nmax} -e {era} --skim SkimTree_DileptonBDT {FlagCommand('RunCF', flags)} &")


if args.Systematics:

    ### DATA JOBS NOT CODED YET SINCE BLINDED                                                                                                                                                  

    for era in era_list:
        DATADir = "DL"
        if UseGT36 and era == "2018":
            DATADir = "DL_GT36"

        # Running background and fake data commands                                                                                                                                            
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/SSWW1.txt -n 20    --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/SSWW2.txt -n 20    --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/DY.txt   -n 20    --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/VBF.txt  -n 20    --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/Weinberg.txt  -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_MuMu.txt -n 20 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunFake', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EE.txt   -n 20 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunFake', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EMu.txt  -n 20 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunFake', flags)} &")
        
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS1.txt  -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunPrompt', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS3.txt  -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunPrompt', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS4.txt  -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunPrompt', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS5.txt  -n 20  --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunPrompt', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS2.txt -n 40 --nmax {nmax} -e {era} --skim SkimTree_SSDileptonBDT {FlagCommand('RunPrompt', flags)} &")

        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Conv/Conv.txt   -n 20 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunConv', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Conv/ConvWG.txt -n 20 --nmax {nmax} -e {era} --skim SkimTree_DileptonBDT  {FlagCommand('RunConv', flags)} &")

        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EE.txt -n 100 --nmax {nmax} -e {era} --skim SkimTree_DileptonBDT {FlagCommand('RunCF', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EMu.txt -n 200 --nmax {nmax} -e {era} --skim SkimTree_DileptonBDT {FlagCommand('RunCF', flags)} &")

        # -- Wait + Monitor 'hadd' after job submission for this era --
        if not TestMode:
            print(f"\n[INFO] Waiting 1 hour after launching all jobs for era {era}...\n")
            time.sleep(7200)  # Wait 1 hour before monitoring

            while True:
                long_hadds = get_long_running_hadd_processes(min_minutes=20)
                if not long_hadds:
                    print(f"[INFO] No long-running hadd jobs. Proceeding to next era: {era}\n")
                    break
                else:
                    print(f"[INFO] Found {len(long_hadds)} hadd job(s) running > 20 min:")
                    for elapsed, cmd in long_hadds:
                        mins = elapsed // 60
                        print(f"    {mins:>3} min - {cmd}")
                    print(f"[INFO] Sleeping 10 more minutes before rechecking...\n")
                    time.sleep(600)  # Wait another 10 minutes
        else:
            print(f"[TEST MODE] Skipping wait/monitor block for era {era}")
        
        
if args.Cutflow:
    nmax = 750
    flags = []
    for era in era_list:
        # Run for cutflow analysis
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/SSWW.txt -n 20 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/DY.txt -n 20 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {sigpath}/Private/VBF.txt -n 20 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunSignal', flags)} &")

        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_MuMu.txt -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunFake', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EE.txt -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunFake', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EMu.txt -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunFake', flags)} &")

        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_MuMu.txt -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunOS', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EE.txt -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunOS', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EMu.txt -n 100 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunOS', flags)} &")

        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS.txt -n 50 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunPrompt', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Prompt/PromptSS2.txt -n 200 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunPrompt', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Conv/Conv.txt -n 50 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunConv', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {mcpath}/Conv/ConvWG.txt -n 50 --nmax {nmax} -e {era} --skim SkimTree_HNMultiLepBDT {FlagCommand('RunConv', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -l {datapath}/{DATADir}/{era}_DiLepton_EE.txt -n 200 --nmax {nmax} -e {era} --skim SkimTree_DileptonBDT {FlagCommand('RunCF', flags)} &")
