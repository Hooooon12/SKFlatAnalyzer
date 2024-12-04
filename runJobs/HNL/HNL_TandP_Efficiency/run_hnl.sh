analyzer=HNL_TandP_Efficiency
rundir=HNL_TandP_Efficiency
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=5
nmax=300
skim=' '

declare  -a era_list=("2018")


if [[ $1 == "" ]]; then

    for i in "${era_list[@]}"
    do
	
        SKFlat.py -a $analyzer  -l $datapath/SL/${i}_SingleLepton_Mu.txt   -n 200    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT &
	#SKFlat.py -a $analyzer  -i DYJetsToMuMu_MiNNLO   -n 400    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT &
	#SKFlat.py -a $analyzer  -i DYJets   -n 400    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT &
	
    done

fi

