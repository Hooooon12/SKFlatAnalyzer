#!/usr/bin/env python                                                                                                                                                                                               
import os, argparse
## Arguments                                                                                                                                                                                                        
parser = argparse.ArgumentParser(description='SKFlat Command')
parser.add_argument('-s', dest='Seed', default="100")
parser.add_argument('-e', dest='Era', default="2017")
parser.add_argument('-v', dest='Version', default="Version11")
parser.add_argument('-m', dest='NMax', default="125")

args = parser.parse_args()



########## Non loop variables
BDTVersion  = args.Version
Classifier  = "BDTG"
Channel     = "EE"
EtaBin      = -1
NormMode    = "EqualNumEvents"

########### Lopp variables   

SignalModes = [3] ## --> EC
VarModes    = [1] ## --> All Inputs (no issue with SF) 
Bkgs        = ["CFBkg"]
Eras        = [str(args.Era)]
Seeds       = [str(args.Seed)]

############## HYPERPARAMETER SETTINGS  
BoostLearningValues = ["0.1","0.05","0.5"]
MinNodeSizes        = ["0.5", "1.0", "2.5"]
BaggedFracs         = ["0.6","0.8"]
NTreeOptions        = [500,1000, 1500,2000]
MaxDepths           = [2,3,4,5]
NCut                = 200
############## BATCH Option 
nMAX = " --nmax "+str(args.NMax)


for Era in Eras:
    for NTREES in NTreeOptions:
        for MaxDepth in MaxDepths:
            for BoostLearningValue in BoostLearningValues:
                for BaggedFrac in BaggedFracs:
                    for Seed in Seeds:
                        for MinNodeSize in MinNodeSizes:
                            for Bkg in Bkgs:
                                for SignalMode in SignalModes:
                                    for VarMode in VarModes:
                                        if MaxDepth == 2 and not BoostLearningValue== "0.5":
                                            continue
                                        os.system("RunIDBDT.py -a runIDBDT_HNtypeIMulti -m " + str(Classifier) + " -b "+str(Bkg)+" -ns "+str(SignalMode)+  " -nv  "+str(VarMode)+ "  -nt "+ str(NTREES) + " -c " + Channel + " -MaxDepth  " + str(MaxDepth) + " -ncut " + str(NCut) + " -eta "+ str(EtaBin) +  " -Nrom " +NormMode+ " -BoostLearningRate  " + BoostLearningValue + " -BaggedFrac " + BaggedFrac +  " -s " + Seed + "  -e "+Era+" " + nMAX + " -MinNodeSize "+MinNodeSize+ " -t " + BDTVersion)




