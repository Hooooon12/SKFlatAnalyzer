export SKFlat_WD=`pwd`
export SKFlat_LIB_PATH=$SKFlat_WD/lib/
mkdir -p $SKFlat_LIB_PATH
mkdir -p $SKFlat_WD/tar

export SKFlatV="Run2UltraLegacy_v3"
mkdir -p $SKFlat_WD/data/$SKFlatV
export DATA_DIR=$SKFlat_WD/data/$SKFlatV

#### use cvmfs for root ####
export CMS_PATH=/cvmfs/cms.cern.ch
source $CMS_PATH/cmsset_default.sh
#export SCRAM_ARCH=slc7_amd64_gcc900
#export cmsswrel='cmssw/CMSSW_11_2_5'
export SCRAM_ARCH=el9_amd64_gcc12
export cmsswrel='cmssw/CMSSW_14_1_0_pre4'
#export cmsswrel='cmssw/CMSSW_15_0_1'

cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/$cmsswrel/src
echo "@@@@ SCRAM_ARCH = "$SCRAM_ARCH
echo "@@@@ cmsswrel = "$cmsswrel
echo "@@@@ scram..."
eval `scramv1 runtime -sh`
cd -
source /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/$cmsswrel/external/$SCRAM_ARCH/bin/thisroot.sh

if [[ $HOSTNAME == *"ui"*".sdfarm.kr"* ]]; then

  echo "@@@@ Working on KISTI"
  export SKFlatRunlogDir="/cms/ldap_home/$USER/SKFlatRunlog/"
  export SKFlatOutputDir="/cms/ldap_home/$USER/SKFlatOutput/"

elif [[ $HOSTNAME == *"tamsa1"* ]]; then

  echo "@@@@ Working on tamsa1"
  export SKFlatRunlogDir="/data6/Users/$USER/SKFlatRunlog/"
  export SKFlatOutputDir="/data6/Users/$USER/SKFlatOutput/"
  export BDTRunlogDir="/data6/Users/$USER/BDTRunlog/"
  export BDTOutputDir="/data6/Users/$USER/BDTOutput/"
  export MVARunlogDir="/data6/Users/$USER/MVARunlog/"
  export MVAOutputDir="/data6/Users/$USER/MVAOutput/"

  export SKFlatEvCompDir="/data6/Users/$USER/SKFlatOutput/EventComparisons/"
    
elif [[ $HOSTNAME == *"tamsa2"* ]]; then

  echo "@@@@ Working on tamsa2"
  export SKFlatRunlogDir="/data6/Users/$USER/SKFlatRunlog/"
  export SKFlatOutputDir="/data6/Users/$USER/SKFlatOutput/"
  export BDTRunlogDir="/data6/Users/$USER/BDTRunlog/"
  export BDTOutputDir="/data6/Users/$USER/BDTOutput/"
  export SKFlatEvCompDir="/data6/Users/$USER/SKFlatOutput/EventComparisons/"
  

elif [[ $HOSTNAME == *"knu"* ]]; then

  echo "@@@@ Working on KNU"
  export SKFlatRunlogDir="/u/user/$USER/scratch/SKFlatRunlog/"
  export SKFlatOutputDir="/u/user/$USER/scratch/SKFlatOutput/"

fi

alias skout="cd $SKFlatOutputDir/$SKFlatV/"

export MYBIN=$SKFlat_WD/bin/
export PYTHONDIR=$SKFlat_WD/python/
export PATH=${MYBIN}:${PYTHONDIR}:${PATH}

export ROOT_INCLUDE_PATH=$ROOT_INCLUDE_PATH:$SKFlat_WD/DataFormats/include/:$SKFlat_WD/AnalyzerTools/include/:$SKFlat_WD/Analyzers/include/:$SKFlat_WD/Analyzers/HNL/include
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SKFlat_LIB_PATH
#export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/bin/python2.7:$LD_LIBRARY_PATH # IF you want to use python 2.7

source $SKFlat_WD/bin/BashColorSets.sh

## submodules ##
#source bin/CheckSubmodules.sh

if [ "$1" = "-q" ];then
    return
fi

## PYTHON ## (no more python2...)
alias python=python3

## Todo list ##
python python/PrintToDoLists.py
source $SKFlat_WD/tmp/ToDoLists.sh
rm $SKFlat_WD/tmp/ToDoLists.sh

CurrentGitBranch=`git branch | grep \* | cut -d ' ' -f2`
printf "> Current SKFlatAnalyzer branch : "${BRed}$CurrentGitBranch${Color_Off}"\n"
echo "-----------------------------------------------------------------"
## Log Dir ##
echo "* Your Log Directory Usage (ctrl+c to skip)"
#du -sh $SKFlatRunlogDir

#export LANG=en_US.UTF-8
#export LC_ALL=en_US.UTF-8
#export LANGUAGE=en_US.UTF-8

#LANG="en_US.utf-8"
##LC_COLLATE="lt_LT.utf-8"
#LC_TIME="en_DK.utf-8"
