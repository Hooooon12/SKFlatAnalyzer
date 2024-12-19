analyzer=SkimTree_EGammaTnP_HighPt
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/${analyzer}/Bkg/
datapath=${SKFlat_WD}/SkimScripts/${analyzer}/data_lists_multilep/
njobs=2000
njobs_data=2000
nmax=300
skim=' '

#declare  -a era_list=("2016preVFP" )
#declare  -a era_list=("2016postVFP" )
#declare  -a era_list=("2016preVFP" "2016postVFP")
#declare  -a era_list=("2017" )
#declare  -a era_list=("2018" )
#declare  -a era_list=("2016preVFP" "2016postVFP" "2017")
#declare  -a era_list=("2016preVFP" "2016postVFP" "2017" "2018")
declare  -a era_list=("2017" "2018")

if [[ $1 == "" ]]; then

    for i in "${era_list[@]}"
    do

#SKFlat.py -a $analyzer  -i DYJets_MG    -n 6000  --nmax 300           -e ${i}   --skim SkimTree_DileptonBDT &
#SKFlat.py -a $analyzer  -i DYJets       -n 6000  --nmax 300           -e ${i}   --skim SkimTree_DileptonBDT &
#SKFlat.py -a $analyzer  -i SingleElectron:F  -n 6000  --nmax 300  -e ${i}  --skim SkimTree_DileptonBDT &
SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton.txt  -n 6000  --nmax 300   -e ${i}  --skim SkimTree_DileptonBDT &
SKFlat.py -a $analyzer  -l $mcpath/Basic.txt     -n 6000  --nmax 300    -e ${i}  --skim SkimTree_DileptonBDT &

    done
fi
