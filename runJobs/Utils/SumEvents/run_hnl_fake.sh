analyzer=SumEvents
rundir=SumEvents
mcpath=${SKFlat_WD}/runJobs/HNL/SumEvents/HNFakeBkg/
datapath=${SKFlat_WD}/runJobs/HNL/SumEvents/data_lists_multilep/
nmax=600

declare  -a era_list=("2018" "2016postVFP" "2016preVFP" "2017")

for i in "${era_list[@]}"
do
    SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_mu.txt  -n 1  --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
    SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_el.txt  -n 1  --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
    SKFlat.py -a $analyzer  -l $mcpath/MC2.txt  -n 1  --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake
    SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_mu.txt  -n 1  --nmax ${nmax}   -e ${i} --skim SkimTree_HNFakeBDT&
    SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_el.txt  -n 1  --nmax ${nmax}   -e ${i} --skim SkimTree_HNFakeBDT&
    SKFlat.py -a $analyzer  -l $mcpath/MC2.txt  -n 1  --nmax ${nmax}   -e ${i} --skim SkimTree_HNFakeBDT

done
