analyzer=SkimTree_ConvEventSkim
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/SampleLists/Bkg/
nmax=300
skim=' '
declare  -a era_list=("2018" "2017"  "2016preVFP" "2016postVFP")
if [[ $1 == "" ]]; then
    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -l  $mcpath/ConvEventMC.txt  -n 1000  --nmax ${nmax}   -e ${i}  
    done

fi

