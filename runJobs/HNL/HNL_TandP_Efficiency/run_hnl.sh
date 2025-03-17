analyzer=HNL_TandP_Efficiency
rundir=HNL_TandP_Efficiency
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=5
nmax=800
skim=' '

#declare  -a era_list=(#"2016preVFP" "2016postVFP" "2017" "2018")
declare  -a era_list=("2018")       

if [[ $1 == "" ]]; then

    for i in "${era_list[@]}"
    do
	
        #SKFlat.py -a $analyzer  -l ${SKFlat_WD}/runJobs/SampleLists/Bkg/Prompt/DY_Ext.txt  -n 200    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT &
	SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO   -n 400    --nmax 800   -e ${i} --skim SkimTree_DileptonBDT& 
	#SKFlat.py -a $analyzer  -i TTLL_powheg         -n 400    --nmax 800   -e ${i} --skim SkimTree_DileptonBDT &
	
	
    done

fi

