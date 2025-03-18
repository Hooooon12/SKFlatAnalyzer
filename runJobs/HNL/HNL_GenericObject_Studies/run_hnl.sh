analyzer=HNL_GenericObject_Studies
rundir=HNL_GenericObject_Studies
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Signals/
bkgpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/DATA/
njobs=20
njobs_data=20
nmax=300
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")


if [[ $1 == "" ]]; then

    declare  -a era_list=("2018")

    for i in "${era_list[@]}"
    do
	
        #SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i}  --skim SkimTree_ZZ   --userflags CheckEMu&
	SKFlat.py -a $analyzer  -i MuonEG:C   -n 1  --nmax ${nmax}   -e 2017  --skim SkimTree_ZZ   --userflags CheckEMu&
    done
fi
