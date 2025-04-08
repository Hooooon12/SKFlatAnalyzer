analyzer=HNL_Validation
rundir=HNL_Validation
sigpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Signals/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/DATA/
njobs=200
njobs_data=200
nmax=400
skim=' '

declare  -a era_list=("2018" "2017" "2016postVFP" "2016preVFP")


if [[ $1 == "TEST" ]]; then
    
    SKFlat.py -a $analyzer  -i WZ_pythia -n 20   -e 2017 --skim SkimTree_Dilepton  &

fi

if [[ $1 == "" ]]; then

    for i in "${era_list[@]}"
    do

	SKFlat.py -a $analyzer  -l $datapath/DATA_${i}.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i} --skim SkimTree_Dilepton  &
        SKFlat.py -a $analyzer  -l $mcpath/${i}/MC2.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_Dilepton  &

    done

fi
