analyzer=GetEffLumi
rundir=GetEffLumi
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
nmax=600
njobs_sig=2

declare  -a era_list=("2018" "2016postVFP" "2016preVFP" "2017")

for i in "${era_list[@]}"
do

    SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
    SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
    SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
    SKFlat.py -a $analyzer  -l $sigpath/Private/Weinberg.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
    
done
