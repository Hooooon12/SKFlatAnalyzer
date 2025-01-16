analyzer=SkimTree_HNLTriggerSF
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/SampleLists/Bkg/
datapath=${SKFlat_WD}/SkimScripts/SampleLists/Data/
njobs=600
njobs_data=600
nmax=300
declare  -a era_list=("2017")

if [[ $1 == "All" ]]; then

    for i in "${era_list[@]}"
    do
        #SKFlat.py -a $analyzer  -i DYJetsToMuMu_MiNNLO     -n ${njobs_data}  --nmax ${nmax}   -e ${i} &
        #SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO       -n ${njobs_data}  --nmax ${nmax}   -e ${i} &
        #SKFlat.py -a $analyzer  -i TTLL_powheg             -n ${njobs_data}  --nmax ${nmax}   -e ${i} &
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_SingleLepton.txt   -n ${njobs_data}  --nmax ${nmax}   -e ${i} &
        #SKFlat.py -a $analyzer  -l $datapath/DATA_EMu_${i}.txt       -n ${njobs_data}  --nmax ${nmax}   -e ${i} 

    done
fi

