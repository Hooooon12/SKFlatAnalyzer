analyzer=SkimTree_HNFake
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/SampleLists/Bkg/
datapath=${SKFlat_WD}/SkimScripts/SampleLists/Data/
njobs=400
njobs_data=400
nmax=500
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")



if [[ $1 == "MAIN" ]]; then
    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_FakeMu.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i} &
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_FakeEl.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i} &
	SKFlat.py -a $analyzer  -l $mcpath/FakeRateMainMCAlt.txt      -n ${njobs}       --nmax ${nmax}   -e ${i} &
	SKFlat.py -a $analyzer  -i WJets_amcatnlo   -n ${njobs}       --nmax ${nmax}   -e ${i} &    
	SKFlat.py -a $analyzer  -i WJets_Sherpa  -n ${njobs}       --nmax ${nmax}   -e ${i} &    
	SKFlat.py -a $analyzer  -i WJets_MG  -n ${njobs}       --nmax ${nmax}   -e ${i} &    

    done    
fi
