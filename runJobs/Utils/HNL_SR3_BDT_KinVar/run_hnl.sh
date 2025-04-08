analyzer=HNL_SR3KinVar
rundir=HNL_SR3KinVar
sigpath=${SKFlat_WD}/runJobs/${analyzer}/Signals/
mcpath=${SKFlat_WD}/runJobs/${analyzer}/Bkg/
njobs=250
nmax=250
skim=' --skim SkimTree_HNMultiLep'
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")


if [[ $1 == "" ]]; then

    for i in "${era_list[@]}"
    do

	SKFlat.py -a $analyzer  -l $sigpath/DY.txt  -n 5  --nmax ${nmax}   -e ${i} &
	SKFlat.py -a $analyzer  -l $sigpath/VBF.txt  -n 5  --nmax ${nmax}   -e ${i} &
	SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt  -n 5  --nmax ${nmax}   -e ${i} &
	SKFlat.py -a $analyzer  -l   $mcpath/${i}/MC.txt  -n 25  --nmax ${nmax}   -e ${i} ${skim} & 
    done

fi
