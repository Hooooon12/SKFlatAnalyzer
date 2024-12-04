analyzer=SkimTree_HNMultiLep
qrundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/SampleLists/Bkg/
datapath=${SKFlat_WD}/SkimScripts/SampleLists/Data/
sigpath=${SKFlat_WD}/SkimScripts/SampleLists/Signals/
njobs=600
njobs_data=100
nmax=350
declare  -a era_list=("2016postVFP" "2016preVFP"  "2017" "2018")

if [[ $1 == "Signal" ]]; then
    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -l ${sigpath}/Signal.txt  -n 100  --nmax 100   -e ${i}   
	SKFlat.py -a $analyzer  -l ${sigpath}/DYAdditionalBDT.txt  -n 20  --nmax ${nmax}   -e ${i} &
        SKFlat.py -a $analyzer  -l ${sigpath}/DYOfficial.txt  -n 20  --nmax ${nmax}   -e ${i} &
        SKFlat.py -a $analyzer  -l ${sigpath}/VBFOfficial.txt  -n 20  --nmax ${nmax}   -e ${i} &
        SKFlat.py -a $analyzer  -l ${sigpath}/SSWWOfficial.txt  -n 20  --nmax ${nmax}   -e ${i} &

    done
fi

if [[ $1 == "Weinberg" ]]; then

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l ${sigpath}/Weinberg.txt    -n ${njobs}  --nmax ${nmax}   -e ${i}  &
    done
fi

if [[ $1 == "All" ]]; then
    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}.txt     -n ${njobs_data}  --nmax ${nmax}   -e ${i}    &
        SKFlat.py -a $analyzer  -l $datapath/DATA_l_${i}.txt   -n ${njobs_data}  --nmax ${nmax}    -e ${i} &
        SKFlat.py -a $analyzer  -l ${mcpath}/MCXSmall.txt      -n 2  --nmax ${nmax}    -e ${i}  &
        SKFlat.py -a $analyzer  -l ${mcpath}/MCSmall.txt       -n 5  --nmax ${nmax}    -e ${i}  &
        SKFlat.py -a $analyzer  -l ${mcpath}/MCMedium.txt      -n 20  --nmax ${nmax}   -e ${i}  
        SKFlat.py -a $analyzer  -l ${mcpath}/MCLarge.txt       -n 50  --nmax ${nmax}   -e ${i}   &
        SKFlat.py -a $analyzer  -l ${mcpath}/MCXLarge.txt      -n 200  --nmax ${nmax}  -e ${i}   &
        SKFlat.py -a $analyzer  -l ${mcpath}/QCD_${i}El.txt    -n 2  --nmax ${nmax}    -e ${i} 
        SKFlat.py -a $analyzer  -l ${mcpath}/QCD_${i}Mu.txt    -n 2  --nmax ${nmax}    -e ${i} 
    done
fi
