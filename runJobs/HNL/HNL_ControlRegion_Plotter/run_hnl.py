import os
import argparse

# Define paths using environment variable
base_path = os.getenv("SKFlat_WD")

mcpath = os.path.join(base_path, "runJobs/SampleLists/Bkg/")
datapath = os.path.join(base_path, "runJobs/SampleLists/Data/")

# Default parameters
analyzer = "HNL_ControlRegion_Plotter"
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

parser.add_argument('--RunPrompt', action='store_true')
parser.add_argument('--RunConv',   action='store_true')
parser.add_argument('--RunFake',   action='store_true')
parser.add_argument('--RunCF',     action='store_true')

parser.add_argument('--Central',     action='store_true')
parser.add_argument('--Systematics', action='store_true')
parser.add_argument('--LLL', action='store_true')
parser.add_argument('--SSMultiLep', action='store_true')

parser.add_argument('--Run', action='store_true')

args = parser.parse_args()

###### Set Era based on input: else 2018
Era=args.era
if args.era == "NULL":
    Era="2018"
    args.era = "2018"
else:
    era_list = [f"{args.era}"]


# Determine if individual sample is provided
IndividualSample = args.samplename != "NULL"

if not IndividualSample:
    if not args.Central and not args.Systematics:
        print ("No inputs submitted. Run either :")
        print ("add -sample X --RunPrompt to run individual sample")
        print ("add --Central to run all jobs for SR (no systematics)")
        print ("add --Systematics to run all jobs for SR (with systematics)")
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

    if JobType  == "":
        flag = flag[:-1]
        return flag

    flag += f"{JobType}"

    return flag

def GetType(arg):

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

if args.SSMultiLep:
    flags.append("SSMultiLep")   
elif args.LLL:
    flags.append("LLL")     
else:
    print("No Flag set")
    exit()
if args.Systematics:
    flags.append("RunSyst")

flags_alt=[]
flags_alt = split_by_comma(args.flags)
if args.SSMultiLep:
    flags_alt.append("SSMultiLep")
elif args.LLL:
    flags_alt.append("LLL")


# Run command for individual sample if provided
if IndividualSample:
    if not args.RunPrompt and not args.RunCF and not args.RunConv and not args.RunFake:
        print(f"Need to set RunType, i.e, RunPrompt,...")
        exit()

    if SampleExists(args):
        SampleType=GetType(args)
        RunCommand(TestMode,f"SKFlat.py -a {analyzer} -i {args.samplename} -n {njob} --nmax {nmax} -e {args.era} --skim {args.skim} {FlagCommand(SampleType, flags)}&")
    exit()



if args.Central or args.Systematics:
    for era in era_list:

        # Running background and fake data commands


        RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {datapath}/DL/{era}_DiLepton_MuMu.txt     -n 100  --nmax {nmax}   -e {era} --skim SkimTree_HNMultiLepBDT  {FlagCommand('', flags)} &")   
        RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {datapath}/DL/{era}_DiLepton_EMu.txt      -n 100  --nmax {nmax}   -e {era} --skim SkimTree_HNMultiLepBDT  {FlagCommand('', flags)} &")   
        RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {datapath}/DL/{era}_DiLepton_EE.txt       -n 100  --nmax {nmax}   -e {era} --skim SkimTree_HNMultiLepBDT  {FlagCommand('', flags)} &")   

        #### Prompt                                                                                                                                                                          
        RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {mcpath}/Prompt/PromptSS.txt             -n 20        --nmax {nmax}   -e {era}  --skim SkimTree_HNMultiLepBDT   {FlagCommand('RunPrompt', flags)}  &")   
        RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {mcpath}/Prompt/PromptSS2.txt            -n 200       --nmax {nmax}   -e {era}  --skim SkimTree_HNMultiLepBDT   {FlagCommand('RunPrompt', flags)}  &")   
        RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {mcpath}/Prompt/PromptSSAlt.txt          -n 200       --nmax {nmax}   -e {era}  --skim SkimTree_HNMultiLepBDT   {FlagCommand('RunPrompt', flags_alt)}  ")   

        ### Conv                                                                                                                                                                             
        if args.SSMultiLep:
            RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {mcpath}/Conv/ConvWG.txt         -n 50        --nmax {nmax}   -e {era} --skim SkimTree_DileptonBDT  {FlagCommand('RunConv', flags)}  &")
            RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {mcpath}/Conv/Conv.txt          -n 50        --nmax {nmax}   -e {era} --skim SkimTree_HNMultiLepBDT  {FlagCommand('RunConv', flags)}  &")
        elif args.LLL:
            RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {mcpath}/Conv/Conv.txt          -n 50        --nmax {nmax}   -e {era} --skim SkimTree_HNMultiLepBDT  {FlagCommand('RunConv', flags)}  &")
            RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {mcpath}/Conv/Conv2.txt         -n 100        --nmax {nmax}   -e {era} --skim SkimTree_HNMultiLepBDT  {FlagCommand('RunConv', flags)}  &")

        ### Fakes                                                                                                                                                                            
        RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {datapath}/DL/{era}_DiLepton_EMu.txt     -n 100  --nmax {nmax}   -e {era}  --skim SkimTree_HNMultiLepBDT   {FlagCommand('RunFake', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {datapath}/DL/{era}_DiLepton_MuMu.txt    -n 100  --nmax {nmax}   -e {era}  --skim SkimTree_HNMultiLepBDT  {FlagCommand('RunFake', flags)} &")
        RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {datapath}/DL/{era}_DiLepton_EE.txt      -n 100    --nmax {nmax}   -e {era}  --skim SkimTree_HNMultiLepBDT  {FlagCommand('RunFake', flags)} ")

        ### CF                                                                                                                                                                               
        if args.SSMultiLep:
            RunCommand(TestMode,f"SKFlat.py -a {analyzer}  -l {datapath}/DL/{era}_DiLepton_EE.txt      -n 100    --nmax {nmax}   -e {era} --skim SkimTree_DileptonBDT  {FlagCommand('RunCF', flags)} ") 

