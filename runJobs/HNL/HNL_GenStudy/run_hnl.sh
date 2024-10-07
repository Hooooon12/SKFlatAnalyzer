analyzer=HNL_GenStudy
rundir=HNL_GenStudy
runPATH=${SKFlat_WD}/runJobs/HNL/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=30
njobs_sig=20
njobs_data=100
nmax=600
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")
declare  -a era_list=("2017")
if [[ $1 == "WZ" ]]; then

    SKFlat.py -a $analyzer  -i WZTo3LNu_amcatnlo -n 300   --nmax ${nmax}  -e 2018 --skim SkimTree_HNMultiLep&
    
fi

if [[ $1 == "M1000" ]]; then

    SKFlat.py -a $analyzer  -i VBFTypeI_DF_M1000_private  -n 10  --nmax ${nmax}   -e 2017  --skim SkimTree_HNMultiLepBDT&
    SKFlat.py -a $analyzer  -i DYTypeI_DF_M1000_private  -n 10  --nmax ${nmax}   -e 2017  --skim SkimTree_HNMultiLepBDT&

fi

if [[ $1 == "M400" ]]; then

    SKFlat.py -a $analyzer  -i DYTypeI_DF_M400_private  -n 10  --nmax ${nmax}   -e 2017  --skim SkimTree_HNMultiLepBDT&

fi



if [[ $1 == "DY" ]]; then

    SKFlat.py -a $analyzer  -i DYTypeI_DF_M1000_private  -n 10  --nmax ${nmax}   -e 2017 --skim SkimTree_HNMultiLepBDT&

fi



if [[ $1 == "SSWW" ]]; then

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $sigpath/SSWWOpt.txt  -n $njobs  --nmax ${nmax}   -e ${i} &
        SKFlat.py -a $analyzer  -l $sigpath/SSWWOpt.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
    done

fi



if [[ $1 == "CONV" ]]; then

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $mcpath/Conv.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLep --userflags RunConv
    done
    
fi

if [[ $1 == "FAKE" ]]; then

    declare  -a era_list=("2016postVFP")
    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLep --userflags RunFake  &
    done

fi


if [[ $1 == "Merge" ]]; then
    

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $mcpath/CFDY.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLep  --userflags RunCF &
	SKFlat.py -a $analyzer  -l $mcpath/MCMerge.txt  -n 20  --nmax ${nmax}   -e ${i}   --skim SkimTree_HNMultiLep &


    done

fi

if [[ $1 == "" ]]; then


    for i in "${era_list[@]}"
    do

SKFlat.py -a $analyzer  -l $sigpath/DY.txt    -n $njobs_sig  --nmax ${nmax}  -e ${i} &
#SKFlat.py -a $analyzer  -l $sigpath/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} &
#SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i} &

	
    done

fi