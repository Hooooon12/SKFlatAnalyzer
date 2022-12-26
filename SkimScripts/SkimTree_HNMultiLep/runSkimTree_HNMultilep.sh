analyzer=SkimTree_HNMultiLep
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/${analyzer}/Bkg/
datapath=${SKFlat_WD}/SkimScripts/${analyzer}/data_lists_multilep/
sigpath=${SKFlat_WD}/SkimScripts/${analyzer}/Signals/
njobs=400
njobs_data=100
nmax=250
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")


if [[ $1 == "DATA" ]]; then
    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $datapath/DATA_TMP.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i} &
        #SKFlat.py -a $analyzer  -l $datapath/DATA_l_${i}.txt   -n ${njobs_data}  --nmax ${nmax}    -e ${i} 
    done
    
fi

if [[ $1 == "DATAEMu" ]]; then
    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}EMu.txt  -n ${njobs_data}  --nmax 100   -e ${i} &

    done

fi
if [[ $1 == "MC" ]]; then
    for i in "${era_list[@]}"
    do
	#SKFlat.py -a $analyzer  -i  WJets_Sherpa  -n 50  --nmax ${nmax}   -e ${i} &
	SKFlat.py -a $analyzer  -i  WJets_MG  -n 50  --nmax ${nmax}   -e ${i} &
	#SKFlat.py -a $analyzer  -i   -n 1000  --nmax ${nmax}   -e ${i} &

		
    done

fi
