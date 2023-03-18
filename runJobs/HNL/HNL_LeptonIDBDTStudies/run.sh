analyzer=HNL_LeptonIDBDTStudies
rundir=HNL_LeptonIDBDTStudies
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Data/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
njobs=250
nmax=800
skim=' SkimTree_Dilepton'
declare  -a era_list=("2018" "2016postVFP" "2016preVFP" "2017")
declare  -a era_list=("2018")

if [[ $1 == "TEST" ]]; then
    
    declare  -a era_list=("2018")
    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -i TTLJ_powheg  -n 400  --nmax 100  -e ${i}    --userflags Fakes --skim SkimTree_FakeEventSkim&
    done
fi



if [[ $1 == "" ]]; then

    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -l   $mcpath/MC.txt  -n 1000  --nmax 200  -e ${i}  --userflags Fakes&
    done
fi


