#!/usr/bin/env python

import os,sys,time
import argparse
import datetime
import random
import subprocess



samples = {
  '2016' : [
     "DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8",
     "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
     "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
     "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
     "WW_TuneCP5_13TeV-pythia8",
     "WZ_TuneCP5_13TeV-pythia8",
     "ZZ_TuneCP5_13TeV-pythia8",
     "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "SingleElectron",
  ],
  '2016preVFP' : [
     "DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8",
     "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
     "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
     "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
     "WW_TuneCP5_13TeV-pythia8",
     "WZ_TuneCP5_13TeV-pythia8",
     "ZZ_TuneCP5_13TeV-pythia8",
     "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "SingleElectron",
  ],
  '2016postVFP' : [
    "DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8",
    "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "WW_TuneCP5_13TeV-pythia8",
    "WZ_TuneCP5_13TeV-pythia8",
    "ZZ_TuneCP5_13TeV-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "SingleElectron",
  ],
  '2017' : [
    "DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8",
    "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "WW_TuneCP5_13TeV-pythia8",
    "WZ_TuneCP5_13TeV-pythia8",
    "ZZ_TuneCP5_13TeV-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "SingleElectron",
  ],
  '2018' : [
    "DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8",
    "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "WW_TuneCP5_13TeV-pythia8",
    "WZ_TuneCP5_13TeV-pythia8",
    "ZZ_TuneCP5_13TeV-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "EGamma",
  ],
}

types = {
  'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : 'MC',
  'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : 'MC',
  'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'WW_TuneCP5_13TeV-pythia8' : 'MC',
  'WZ_TuneCP5_13TeV-pythia8' : 'MC',
  'ZZ_TuneCP5_13TeV-pythia8' : 'MC',
  'SingleElectron' : 'DATA',
  'EGamma' : 'DATA',
}



dates = {
  '2016preVFP':
    {
     'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'SingleElectron' : {
                         'periodB_ver2' : '2024_11_26_102156',
                         'periodC'      : '2024_11_26_102156',
                         'periodD'      : '2024_11_26_102156',
         'periodE'      : '2024_11_26_102156',
         'periodF'      : '2024_11_26_102156',
                        },
    },
 '2016postVFP':
    {
     'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'SingleElectron' : {
                         'periodF' : '2024_11_26_102156',
                         'periodG' : '2024_11_26_102156',
                         'periodH' : '2024_11_26_102156',
                        },
    },
'2017':
    {
     'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {
                                                             '' : '2024_12_13_185927',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_12_13_185927',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'SingleElectron' : {
                         'periodB' : '2024_12_13_185927',
                         'periodC' : '2024_12_13_185927',
                         'periodD' : '2024_12_13_185927',
                         'periodE' : '2024_12_13_185927',
                         'periodF' : '2024_12_13_185927',
                        },
    },
'2018':
    {
     'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {
                                                             '' : '2024_12_13_185927',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_12_13_185927',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_13_185927',
     },
     'EGamma' : {
                 'periodA' : '2024_12_13_185927',
                 'periodB' : '2024_12_13_185927',
                 'periodC' : '2024_12_13_185927',
                 'periodD' : '2024_12_13_185927',
                },
    },
}
triggers = {
            '2016'        : 'passHltEle27WPTightGsf',
            '2016preVFP'  : 'passHltEle27WPTightGsf',
            '2016postVFP' : 'passHltEle27WPTightGsf',
            '2017'        : 'passHltEle32DoubleEGWPTightGsf',
            '2018'        : 'passHltEle32WPTightGsf',
}
nameFilter = {
  'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : 'DYJets',
  'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : 'WJets',
  'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : 'TTLL',
  'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : 'TTLJ',
  'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'tW_top',
  'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'tw_antitop',
  'WW_TuneCP5_13TeV-pythia8' : 'WW',
  'WZ_TuneCP5_13TeV-pythia8' : 'WZ',
  'ZZ_TuneCP5_13TeV-pythia8' : 'ZZ',
  'SingleElectron' : 'data',
  'EGamma' : 'data',
}


def MakeInputFiles(_type, _era,_sample,_path,_list,NJOBS):

    if os.path.exists("batch_input/"+_era+"_"+_sample+"_1.txt"):
        os.system("rm " + "batch_input/"+_era+"_"+_sample+"_*.txt")
    nj=1
    for x in _list:
        FilePath = _path+"/"+x +"\n"

        fileName="batch_input/"+_era+"_"+_sample+"_"+str(nj-1)+".txt"
        file1= open(fileName, "a")
        file1.write(FilePath)
        nj=nj+1
        if nj > NJOBS:
            nj=1

        file1.close()



def CheckList(input_samples, types_sample,NJobs,Era,SkimName):
    from os import listdir
    from os.path import isfile, isdir,join
    NFewestFiles=1000000

    for x in input_samples[Era]:
        path_to_files="/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+Era+"/"+types[x]+"_"+SkimName+"/" + x
        print path_to_files

        if types[x] == "MC":
            datedir = [f for f in listdir(path_to_files) if isdir(join(path_to_files, f))]
            if len(datedir ) > 1:
                exit
            new_path_to_files = path_to_files + "/"+datedir[0]
            infiles = [f for f in listdir(new_path_to_files) if isfile(join(new_path_to_files, f))]
            if len(infiles) < NFewestFiles:
                NFewestFiles = len(infiles)
            MakeInputFiles(types[x],Era,x, new_path_to_files,infiles,NJobs)
        else:
            perioddir = [f for f in listdir(path_to_files) if isdir(join(path_to_files, f))]
            if len(perioddir ) > 1:
                exit

            for period in perioddir:
                period_path_to_files  = path_to_files + "/"+period
                datedir = [f for f in listdir(period_path_to_files) if isdir(join(period_path_to_files, f))]
                if len(datedir ) > 1:
                    exit

                new_path_to_files = period_path_to_files + "/"+datedir[0]
                infiles = [f for f in listdir(new_path_to_files) if isfile(join(new_path_to_files, f))]
                if len(infiles) < NFewestFiles:
                    NFewestFiles = len(infiles)
                MakeInputFiles(types[x],Era,x+"_"+period, new_path_to_files,infiles,NJobs)

    return NFewestFiles


def MakeInputList(input_samples, types_sample, NJobs,Era,SkimName):

    from os import listdir
    from os.path import isfile, isdir,join

    print "Making MakeInputList for following samples"

    for x in input_samples[Era]:
        path_to_files="/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+Era+"/"+types[x]+"_"+SkimName+"/" + x
        print path_to_files

        if types[x] == "MC":
            datedir = [f for f in listdir(path_to_files) if isdir(join(path_to_files, f))]
            if len(datedir ) > 1:
                exit
            new_path_to_files = path_to_files + "/"+datedir[0]
            infiles = [f for f in listdir(new_path_to_files) if isfile(join(new_path_to_files, f))]

            MakeInputFiles(types[x],Era,x, new_path_to_files,infiles,NJobs)
        else:
            perioddir = [f for f in listdir(path_to_files) if isdir(join(path_to_files, f))]
            if len(perioddir ) > 1:
                exit

            for period in perioddir:
                period_path_to_files  = path_to_files + "/"+period
                datedir = [f for f in listdir(period_path_to_files) if isdir(join(period_path_to_files, f))]
                if len(datedir ) > 1:
                    exit

                new_path_to_files = period_path_to_files + "/"+datedir[0]
                infiles = [f for f in listdir(new_path_to_files) if isfile(join(new_path_to_files, f))]
                MakeInputFiles(types[x],Era,x+"_"+period, new_path_to_files,infiles,NJobs)




## Arguments

parser = argparse.ArgumentParser(description='SKFlat Command')
parser.add_argument('-e', dest='Era', default="2017",help="2016preVFP(2016a), 2016postVFP(2016b), 2017, 2018, Run2")
parser.add_argument('-o', dest='Outputdir', default="",help="")
parser.add_argument('-v', dest='Version', default="Version1",help="")
parser.add_argument('-s', dest='Script', default="NULL",help="pick config/*.py")
parser.add_argument('-n', dest='NJob',type=int, default=15,help="")
parser.add_argument('-max', dest='NMax',type=int, default=500,help="")

args = parser.parse_args()

Version = args.Version
NJobs=args.NJob
Era = args.Era

if args.Script == "NULL":
    print ("-s command not set")
    exit()

print ("Running MakeInputList:")
print ("Njobs = " + str(args.NJob))
print ("Era = " + str(args.Era))

if not os.path.exists("batch_input"):
    os.mkdir("batch_input")
    print ("making dir... batch_input/")

#os.system("python MakeInputList.py -e " + Era +  " -n " + str(NJobs))

SkimName="SkimTree_EGammaTnP_HNLHighPt"
NMaxAllowed=CheckList(samples,types,NJobs,Era,SkimName)

if NJobs > NMaxAllowed:
  NJobs=NMaxAllowed
  print("Updating Njobs since some input has fewer files: NJobs --> " + str(NJobs))

MakeInputList(samples,types,NJobs,Era,SkimName)

## Add Abosolute path for outputdir

Outputdir = os.getcwd()+'/'+args.Outputdir

## TimeStamp

# 1) dir/file name style
JobStartTime = datetime.datetime.now()
timestamp =  JobStartTime.strftime('%Y_%m_%d_%H%M%S')


# 2) log style
string_JobStartTime =  JobStartTime.strftime('%Y-%m-%d %H:%M:%S')

## Environment Variables

SCRAM_ARCH = os.environ['SCRAM_ARCH']
cmsswrel = os.environ['cmsswrel']

USER = os.environ['USER']

TandPRunlogDir = "/data6/Users/"+USER+"/TandPRunlog/"

os.system("mkdir -p " +TandPRunlogDir)

if Version != "":
  TandPRunlogDir=TandPRunlogDir+Version+"/"
  os.system("mkdir -p " +TandPRunlogDir)

TandPRunlogDir=TandPRunlogDir+Era+"/"
os.system("mkdir -p " +TandPRunlogDir)

UID = str(os.getuid())
HOSTNAME = os.environ['HOSTNAME']
SampleHOSTNAME = HOSTNAME

## Check joblog email

StringForHash=""
random.seed(hash(StringForHash+timestamp+args.Era))

RandomNumber = int(random.random()*1000000)
str_RandomNumber = str(RandomNumber)

webdirname = timestamp+"_"+str_RandomNumber

## Define MasterJobDir

MasterJobDir = TandPRunlogDir+'/TS_'+timestamp+'__'+str_RandomNumber+"__"
MasterJobDir += '__'+HOSTNAME+'/'
os.mkdir(MasterJobDir)

WorkDirs = [os.environ['SKFlat_WD'] + '/Tag_Probe/']

scriptname=""
for WorkDir in WorkDirs:

  scriptname = "RunEff.py"
  if args.Script != "":
    scriptname = args.Script
    
  print ("Running job for output " + MasterJobDir)
    
  os.system('cp ' + WorkDir + '/'+ scriptname+' ' + MasterJobDir)
  os.system('mv batch_input ' + MasterJobDir)
  os.system('cp -r Out_TurnOn/ ' + MasterJobDir)
  os.system('mkdir -p ' +  MasterJobDir+"/Out_Eff/")
  
  scriptname=scriptname.replace('Config/','')

  commandsfilename = 'ElectronIDSF_'+Era+"_"+Version
  run_commands = open(MasterJobDir+'/'+commandsfilename+'.sh','w')

  print>>run_commands,'''#!/bin/bash
SECTION=`printf $1`
WORKDIR=`pwd`

Trial=0

#### make sure use C locale
export LC_ALL=C

#### Don't make root history
export ROOT_HIST=0

#### use cvmfs for root ####
export CMS_PATH=/cvmfs/cms.cern.ch
source $CMS_PATH/cmsset_default.sh
export SCRAM_ARCH={2}
export cmsswrel={3}
cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/$cmsswrel/src
echo "@@@@ SCRAM_ARCH = "$SCRAM_ARCH
echo "@@@@ cmsswrel = "$cmsswrel
echo "@@@@ scram..."
eval `scramv1 runtime -sh`
cd -


source /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/$cmsswrel/external/$SCRAM_ARCH/bin/thisroot.sh

while [ "$Trial" -lt 3 ]; do
  echo "#### running ####"


  python {0}/{1} -nj ${{SECTION}} -e {4} -wd {0} 2> err.log

  EXITCODE=$?
  if [ "$EXITCODE" -eq 5 ]; then
    echo "IO error occured.. running again in 300 seconds.."
    Trial=$((Trial+=1))
    sleep 300
  else
    break
  fi
done


if [ "$EXITCODE" -ne 0 ]; then
  echo "ERROR errno=$EXITCODE" >> err.log
fi

cat err.log >&2
exit $EXITCODE
'''.format(MasterJobDir,scriptname, SCRAM_ARCH, cmsswrel, Era )
  run_commands.close()

  submit_command = open(MasterJobDir+'/submit.jds','w')
  if args.NMax:
    concurrency_limits='concurrency_limits = n'+str(args.NMax)+'.'+os.getenv("USER")
    print>>submit_command,'''executable = {1}.sh
jobbatchname = {1}
universe   = vanilla
arguments  = $(Process)
log = condor.log
getenv     = True
should_transfer_files = YES
when_to_transfer_output = ON_EXIT
output = job_$(Process).log
error = job_$(Process).err
{2}
queue {0}
'''.format(str(NJobs), commandsfilename,concurrency_limits)
    submit_command.close()


    cwd = os.getcwd()
    os.chdir(MasterJobDir)
    condorOptions = ''
    os.system('condor_submit submit.jds '+condorOptions)
    os.chdir(cwd)


print '##################################################'
print("RunDir: " + MasterJobDir)
print 'Submission Finished'
print '- JobID = '+str_RandomNumber
print '- Run Script = ' + scriptname
print '- Njobs = ' + str(NJobs)
print '- Era = '+str(args.Era)
print '- output will be send to : '+MasterJobDir+"/Out_Eff/"

print '##################################################'
