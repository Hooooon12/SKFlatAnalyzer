analyzer=SkimTree_CFEventSkim
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/SampleLists/Bkg/
datapath=${SKFlat_WD}/SkimScripts/SampleLists/Data/
nmax=300
declare  -a era_list=("2018" "2017"  "2016preVFP" "2016postVFP")

if [[ $1 == "" ]]; then
    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -l  $mcpath/CFEventMC.txt  -n 1000  --nmax ${nmax}   -e ${i}  &
    done

fi

