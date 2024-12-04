analyzer=HNL_HighPtMuon_Studies
rundir=HNL_HighPtMuon_Studies
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/
njobs=20
njobs_data=20
nmax=400
skim=' '

declare  -a era_list=( "2018")

if [[ $1 == "" ]]; then

    for i in "${era_list[@]}"
    do
	
        SKFlat.py -a $analyzer  -l ${datapath}/SL/${i}_SingleLepton_Mu.txt    -n 1  --nmax 1  -e ${i}  --skim SkimTree_HNMultiLepBDT&
    done
fi
