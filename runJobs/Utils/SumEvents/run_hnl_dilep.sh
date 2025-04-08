analyzer=SumEvents
rundir=SumEvents
mcpath=${SKFlat_WD}/runJobs/HNL/SumEvents/Bkg/SkimTree_Dilepton/
datapath=${SKFlat_WD}/runJobs/HNL/SumEvents/data_lists_multilep/SkimTree_Dilepton/
nmax=600

declare  -a era_list=("2018" "2016postVFP" "2016preVFP" "2017")

for i in "${era_list[@]}"
do
    #SKFlat.py -a $analyzer  -l $datapath/DATA_${i}.txt  -n 1  --nmax ${nmax}           -e ${i}  --skim SkimTree_Dilepton  &
    #SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton.txt  -n 1  --nmax ${nmax}   -e ${i}  --skim SkimTree_Dilepton  &
    #SKFlat.py -a $analyzer  -l $datapath/DATA_${i}EMu.txt  -n 1  --nmax ${nmax}        -e ${i}   &
    SKFlat.py -a $analyzer  -l $mcpath/MC2.txt  -n 1  --nmax ${nmax}   -e ${i}  --skim SkimTree_Dilepton &


    #SKFlat.py -a $analyzer  -l $datapath/DATA_${i}.txt  -n 1 --nmax ${nmax}           -e ${i}  --skim SkimTree_DileptonBDT  &
    #SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton.txt  -n 1  --nmax ${nmax}   -e ${i}  --skim SkimTree_DileptonBDT  &
    #SKFlat.py -a $analyzer  -l $datapath/DATA_${i}EMu.txt  -n 1  --nmax ${nmax}        -e ${i}  --skim SkimTree_DileptonBDT   &
    SKFlat.py -a $analyzer  -l $mcpath/MC2.txt  -n 1  --nmax ${nmax}   -e ${i}  --skim SkimTree_DileptonBDT &

done
