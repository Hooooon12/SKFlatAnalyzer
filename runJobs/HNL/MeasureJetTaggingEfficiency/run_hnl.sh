analyzer=MeasureJetTaggingEfficiency
rundir=MeasureJetTaggingEfficiency
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
njobs=200
njobs_data=200
nmax=400
skim=' '

declare  -a era_list=("2018" "2017" "2016postVFP" "2016preVFP")



if [[ $1 == "" ]]; then
    
    for i in "${era_list[@]}"
    do

	#SKFlat.py -a $analyzer  -l $mcpath/MC.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags 2L&
	SKFlat.py -a $analyzer  -l $mcpath/MC.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags SS&
    done

fi
