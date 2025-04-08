analyzer=HNL_LeptonIDSF
rundir=HNL_LeptonIDSF
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Data/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
njobs=250
nmax=200
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")



if [[ $1 == "" ]]; then

    declare  -a era_list=("2017")

    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton_E.txt    -n 600  --nmax 300   -e ${i}  --skim SkimTree_DileptonBDT&
        #SKFlat.py -a $analyzer  -l ${mcpath}/Conv.txt    -n 100  --nmax 400  -e ${i}  --skim SkimTree_ConvEventSkim&
        #SKFlat.py -a $analyzer  -l ${mcpath}/Fake.txt    -n 100  --nmax 400  -e ${i}  --skim SkimTree_FakeEventSkimBDT&

    done
fi



if [[ $1 == "IDSF" ]]; then

    declare  -a era_list=("2017")

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton_Mu.txt   -n 600  --nmax 300   -e ${i}  --skim SkimTree_DileptonBDT&
	#SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton_E.txt    -n 600  --nmax 300   -e ${i}  --skim SkimTree_DileptonBDT&
	SKFlat.py -a $analyzer  -i DYJets                    -n 600  --nmax 300   -e ${i}  --skim SkimTree_DileptonBDT&
    done
fi


