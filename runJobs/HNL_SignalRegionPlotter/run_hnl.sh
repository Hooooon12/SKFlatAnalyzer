analyzer=HNL_SignalRegionPlotter
rundir=HNL_SignalRegionPlotter
sigpath=${SKFlat_WD}/runJobs/${analyzer}/Signals/
mcpath=${SKFlat_WD}/runJobs/${analyzer}/Bkg/
datapath=${SKFlat_WD}/runJobs/${analyzer}/DATA/
njobs=30
njobs_sig=1
njobs_data=20
nmax=200
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

if [[ $1 == "DY" ]]; then

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $sigpath/${i}/DY.txt  -n $njobs  --nmax ${nmax}   -e ${i} &
    done

fi

if [[ $1 == "VBF" ]]; then

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $sigpath/${i}/VBF.txt  -n $njobs  --nmax ${nmax}   -e ${i} &
    done

fi

if [[ $1 == "VBF1000" ]]; then

    SKFlat.py -a $analyzer  -i VBFTypeI_DF_M1000_private  -n 300  --nmax ${nmax}   -e 2018  &

fi

if [[ $1 == "DY300" ]]; then

    SKFlat.py -a $analyzer  -i DYTypeI_DF_M300_private  -n 10  --nmax ${nmax}   -e 2018 &

fi



if [[ $1 == "SSWW" ]]; then

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $sigpath/${i}/SSWW.txt  -n $njobs  --nmax ${nmax}   -e ${i} &
    done

fi



if [[ $1 == "CONV" ]]; then

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $mcpath/Conv.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLep --userflags RunConv
    done
    
fi

if [[ $1 == "FAKE" ]]; then

    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -l $datapath/DATA_${i}.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLep --userflags RunFake  &
    done

fi


if [[ $1 == "MC" ]]; then
    
    declare  -a era_list=("2016postVFP")

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $mcpath/${i}/MC.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_SS2lOR3l &

    done

fi

if [[ $1 == "" ]]; then


    for i in "${era_list[@]}"
    do
	
	SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt  -n $njobs_sig  --nmax ${nmax}   -e ${i} &
	SKFlat.py -a $analyzer  -l $sigpath/DY.txt  -n $njobs_sig  --nmax ${nmax}   -e ${i} &
        SKFlat.py -a $analyzer  -l $sigpath/VBF.txt  -n $njobs_sig  --nmax ${nmax}   -e ${i} &
        SKFlat.py -a $analyzer  -l $mcpath/MC.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLep &
        SKFlat.py -a $analyzer  -l $mcpath/CF.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLep --userflags RunCF &
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLep --userflags RunFake  &
        SKFlat.py -a $analyzer  -l $mcpath/Conv.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLep --userflags RunConv

	
    done

fi
