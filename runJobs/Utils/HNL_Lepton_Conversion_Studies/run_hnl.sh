analyzer=HNL_Lepton_Conversion_Studies
rundir=HNL_Lepton_Conversion_Studies
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/DATA/
njobs=100
njobs_data=100
nmax=300
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

declare  -a era_list=("2017")


if [[ $1 == "" ]]; then
    

    for i in "${era_list[@]}"
    do
        #SKFlat.py -a $analyzer  -l runJobs/SampleLists/Bkg/PromptSS.txt   -n 400  --nmax 100   -e ${i}  --skim SkimTree_HNMultiLepBDT &
        #SKFlat.py -a $analyzer  -l runJobs/SampleLists/Bkg/PromptSS2.txt   -n 400  --nmax 100   -e ${i}  --skim SkimTree_HNMultiLepBDT &
	#SKFlat.py -a $analyzer  -l $mcpath/Conv2.txt  -n 400  --nmax 100   -e ${i}  --skim SkimTree_HNMultiLepBDT &
	#SKFlat.py -a $analyzer  -i TTLL_powheg  -n 400  --nmax 400   -e ${i}  --skim SkimTree_HNMultiLepBDT &
	#SKFlat.py -a $analyzer  -i TTLJ_powheg  -n 400  --nmax 400   -e ${i}  --skim SkimTree_HNMultiLepBDT &
	SKFlat.py -a $analyzer  -i DYJets_MG  -n 400  --nmax 400   -e ${i}  --skim SkimTree_HNMultiLepBDT &
        SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt     -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &

	SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt     -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &

    done

fi

