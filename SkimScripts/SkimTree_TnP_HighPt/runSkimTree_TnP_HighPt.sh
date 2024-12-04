analyzer=SkimTree_TnP_HighPt
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/SampleLists/Bkg/
datapath=${SKFlat_WD}/SkimScripts/SampleLists/Data/
njobs=600
njobs_data=600
nmax=400
declare  -a era_list=("2018")

# "2017"  "2016preVFP" "2016postVFP")

if [[ $1 == "" ]]; then

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $datapath/DATA_l_${i}.txt   -n ${njobs_data}  --nmax ${nmax}    -e ${i} &

    done
fi

