analyzer=SkimTree_DileptonBDT
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/SampleLists/Bkg/
datapath=${SKFlat_WD}/SkimScripts/SampleLists/Data/
njobs=600
njobs_data=600
nmax=400
declare  -a era_list=("2018" "2017"  "2016preVFP" "2016postVFP")

if [[ $1 == "All" ]]; then

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -i  WZTo3LNu_mllmin4p0_powheg   -n ${njobs_data}  --nmax ${nmax}   -e ${i}  &
	SKFlat.py -a $analyzer  -i  WWTo2L2Nu_powheg   -n ${njobs_data}  --nmax ${nmax}   -e ${i}  &
	SKFlat.py -a $analyzer  -i  ZZTo4L_powheg   -n ${njobs_data}  --nmax ${nmax}   -e ${i}  &

    done
fi
