analyzer=SkimTree_ZZ
qrundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/SampleLists/Bkg/
datapath=${SKFlat_WD}/SkimScripts/SampleLists/Data/
njobs=600
njobs_data=100
nmax=350
declare  -a era_list=("2016postVFP" "2016preVFP"  "2017" "2018")


if [[ $1 == "" ]]; then
    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $datapath/DATA_EMu_${i}.txt     -n ${njobs_data}  --nmax ${nmax}   -e ${i}   --skim  SkimTree_HNMultiLep &
	#SKFlat.py -a $analyzer  -i ZZTo4L_powheg    -n 200  --nmax ${nmax}  -e ${i}   &
    done
fi
