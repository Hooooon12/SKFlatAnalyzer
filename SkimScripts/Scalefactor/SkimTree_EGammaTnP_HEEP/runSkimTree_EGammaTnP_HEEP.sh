analyzer=SkimTree_EGammaTnP_HEEP
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/${analyzer}/Bkg/
datapath=${SKFlat_WD}/SkimScripts/SkimTree_EGammaTnP_HEEP/data_lists_multilep/
njobs=2000
njobs_data=2000
nmax=500
skim=' '

declare  -a era_list=("2016preVFP" "2016postVFP" "2018"  "2017")

if [[ $1 == "" ]]; then

    for i in "${era_list[@]}"
    do

	SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO    -n 6000  --nmax 500           -e ${i}   --skim SkimTree_DileptonBDT &
        SKFlat.py -a $analyzer  -i DYJets       -n 6000  --nmax 500           -e ${i}   --skim SkimTree_DileptonBDT &
        SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton.txt  -n 6000  --nmax 500   -e ${i}  --skim SkimTree_DileptonBDT &

    done
fi
