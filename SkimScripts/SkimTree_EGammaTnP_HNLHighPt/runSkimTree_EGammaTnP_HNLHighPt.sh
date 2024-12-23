analyzer=SkimTree_EGammaTnP_HNLHighPt
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/${analyzer}/Bkg/
datapath=${SKFlat_WD}/SkimScripts/SkimTree_EGammaTnP_HNLHighPt/data_lists_multilep/
njobs=2000
njobs_data=2000
nmax=500
skim=' '

declare  -a era_list=("2018" "2016preVFP" "2016postVFP")

if [[ $1 == "" ]]; then

    for i in "${era_list[@]}"
    do
	
	SKFlat.py -a $analyzer  -l $mcpath/MC.txt   -n 6000  --nmax 500           -e ${i}   --skim SkimTree_DileptonBDT &
	SKFlat.py -a $analyzer  -l $mcpath/MC2.txt   -n 6000  --nmax 500           -e ${i}   --skim SkimTree_DileptonBDT 
        SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton.txt  -n 6000  --nmax 500   -e ${i}  --skim SkimTree_DileptonBDT 

    done
fi
