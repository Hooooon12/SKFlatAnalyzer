analyzer=HNL_SignalRegion_TestRun
rundir=HNL_SignalRegion_TestRun
runPATH=${SKFlat_WD}/runJobs/HNL/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=30
njobs_sig=5
njobs_data=100
nmax=400
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")


if [[ $1 == "" ]]; then

    declare  -a era_list=("2017")

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -i DYTypeI_DF_M1000_private  -n 1  --nmax 1   -e 2018 --skim SkimTree_HNMultiLepBDT  &
	
        #SKFlat.py -a $analyzer  -i TTLL_powheg    -n 400  --nmax 500  -e ${i}  --skim SkimTree_DileptonBDT &
        #SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO         -n 400  --nmax 500  -e ${i}  --skim SkimTree_DileptonBDT &

  done
fi

