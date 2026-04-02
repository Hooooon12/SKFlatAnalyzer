pathAnaDir='/data6/Users/jalmond/HNL/SKFlatAnalyzer/Analyzers/HNL/src/'

import os

from os import listdir
from os.path import isfile,isdir, join

import argparse
parser = argparse.ArgumentParser(description='option')
parser.add_argument('-f', dest='flag', default="NULL")
parser.add_argument('--VERBOSE', action='store_true')

args = parser.parse_args()
flag=args.flag
#update_script='script/UpdateSampleForSNULatest.sh'


macro_list  = [f for f in listdir(pathAnaDir) if isfile(join(pathAnaDir,f))]
run_options = []
for _macro in macro_list:
    if not flag == "NULL":
        if flag == "CR":
            flag = "HNL_ControlRegion_Definitions"
        if flag == "SR":
            flag = "HNL_SignalRegion_Definitions"

        if not flag in _macro:
            continue
    if '.C' in _macro:
        if not 'C~' in _macro:
            print("emacs -nw  "+pathAnaDir+ _macro)
            run_options.append("emacs -nw  "+pathAnaDir+ _macro)

        
if len(run_options) == 1:
    os.system(run_options[0])
