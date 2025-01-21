analyzer=HNL_HighPtMuon_Studies
rundir=HNL_HighPtMuon_Studies
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/
njobs=20
njobs_data=20
nmax=400
skim=' '



if [[ $1 == "DATA" ]]; then


    declare  -a era_list=( "2016preVFP" "2016postVFP" "2017" "2018")
    for i in "${era_list[@]}"
    do	
        #SKFlat.py -a $analyzer  -l ${datapath}/SL/${i}_SingleLepton_Mu.txt    -n 1  --nmax 1  -e ${i}  --skim SkimTree_HNMultiLepBDT&
	SKFlat.py -a $analyzer  -l ${datapath}/DL/${i}_DiLepton_MuMu.txt    -n 300  --nmax 600  -e ${i}  --skim SkimTree_DileptonBDT&
    done
fi

if [[ $1 == "SIG" ]]; then

    declare  -a era_list=( "2018")

    for i in "${era_list[@]}"
    do
        #SKFlat.py -a $analyzer  -l ${SKFlat_WD}/runJobs/SampleLists/Signals/Private/SampleSig.txt  -n 300  --nmax 300  -e ${i} --skim SkimTree_HNMultiLepBDT&     
	#SKFlat.py -a $analyzer  -l ${SKFlat_WD}/runJobs/SampleLists/Signals/Private/Weinberg.txt  -n 300  --nmax 300  -e ${i} --skim SkimTree_HNMultiLepBDT&     
	SKFlat.py -a $analyzer  -l ${SKFlat_WD}/runJobs/SampleLists/Signals/Private/HM_DY.txt  -n 300  --nmax 300  -e ${i} --skim SkimTree_HNMultiLepBDT&     
	SKFlat.py -a $analyzer  -l ${SKFlat_WD}/runJobs/SampleLists/Signals/Private/HM_VBF.txt  -n 300  --nmax 300  -e ${i} --skim SkimTree_HNMultiLepBDT&     
	SKFlat.py -a $analyzer  -l ${SKFlat_WD}/runJobs/SampleLists/Signals/Private/HM_SSWW.txt  -n 300  --nmax 300  -e ${i} --skim SkimTree_HNMultiLepBDT&     
    done
fi

