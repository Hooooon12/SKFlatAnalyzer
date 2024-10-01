analyzer=HNL_ControlRegion_Plotter
rundir=HNL_ControlRegion_Plotter
runPATH=${SKFlat_WD}/runJobs/HNL/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=30
njobs_sig=2
njobs_data=200
nmax=600
skim=' '

if [[ $1 == "" ]]; then

    declare  -a era_list=("2017")
    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 1    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags SSMultiLep,RunFake   &
	SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 1    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags SSMultiLep,RunFake   &

    done
    
fi