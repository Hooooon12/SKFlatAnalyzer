analyzer=SumEvents
rundir=SumEvents
sigpath=${SKFlat_WD}/runJobs/HNL/SumEvents/Signals/
mcpath=${SKFlat_WD}/runJobs/HNL/SumEvents/Bkg/SkimTree_HNMultiLepBDT/Bkg/
datapath=${SKFlat_WD}/runJobs/HNL/SumEvents/DATA/
nmax=600

declare  -a era_list=("2018" "2016postVFP" "2016preVFP" "2017")

for i in "${era_list[@]}"
do

    SKFlat.py -a $analyzer  -l  ${sigpath}/Signal.txt  -n 1  --nmax ${nmax}  -e ${i}    &  
    #SKFlat.py -a $analyzer  -l  ${sigpath}/Signal.txt  -n 1  --nmax ${nmax}  -e ${i}    --skim  SkimTree_HNMultiLepBDT &  

    #SKFlat.py -a $analyzer  -l $datapath/DATA_${i}.txt      -n 1  --nmax ${nmax}  -e ${i}    --skim  SkimTree_HNMultiLep &
    #SKFlat.py -a $analyzer  -l $datapath/DATA_l_${i}.txt    -n 1  --nmax ${nmax}     -e ${i} --skim  SkimTree_HNMultiLep &
    #SKFlat.py -a $analyzer  -l $datapath/DATA_${i}.txt      -n 1  --nmax ${nmax}  -e ${i}    --skim  SkimTree_HNMultiLepBDT &
    #SKFlat.py -a $analyzer  -l $datapath/DATA_l_${i}.txt    -n 1  --nmax ${nmax}     -e ${i} --skim  SkimTree_HNMultiLepBDT &
    
    #SKFlat.py -a $analyzer  -l ${mcpath}/MCXSmall.txt  -n 1  --nmax ${nmax}   -e ${i}   --skim  SkimTree_HNMultiLep&
    ###SKFlat.py -a $analyzer  -l ${mcpath}/MCSmall.txt  -n 1  --nmax ${nmax}   -e ${i}   --skim  SkimTree_HNMultiLep&
    #SKFlat.py -a $analyzer  -l ${mcpath}/MCMedium.txt  -n 1  --nmax ${nmax}   -e ${i}   --skim  SkimTree_HNMultiLep
    #SKFlat.py -a $analyzer  -l ${mcpath}/MCLarge.txt  -n 1  --nmax ${nmax}   -e ${i}   --skim  SkimTree_HNMultiLep &
    #SKFlat.py -a $analyzer  -l ${mcpath}/MCXLarge.txt  -n 1  --nmax ${nmax}   -e ${i}   --skim  SkimTree_HNMultiLep 
    #SKFlat.py -a $analyzer  -l ${mcpath}/${i}/QCD.txt -n 1  --nmax ${nmax}   -e ${i}  --skim  SkimTree_HNMultiLep
    
    #SKFlat.py -a $analyzer  -l ${mcpath}/MCXSmall.txt  -n 1  --nmax ${nmax}   -e ${i}   --skim  SkimTree_HNMultiLepBDT&
    #SKFlat.py -a $analyzer  -l ${mcpath}/MCSmall.txt  -n 1  --nmax ${nmax}   -e ${i}   --skim  SkimTree_HNMultiLepBDT&
    #SKFlat.py -a $analyzer  -l ${mcpath}/MCMedium.txt  -n 1  --nmax ${nmax}   -e ${i}   --skim  SkimTree_HNMultiLepBDT&
    #SKFlat.py -a $analyzer  -l ${mcpath}/MCLarge.txt  -n 1  --nmax ${nmax}   -e ${i}   --skim  SkimTree_HNMultiLepBDT &
    #SKFlat.py -a $analyzer  -l ${mcpath}/MCXLarge.txt  -n 1  --nmax ${nmax}   -e ${i}   --skim  SkimTree_HNMultiLepBDT 
    #SKFlat.py -a $analyzer  -l ${mcpath}/${i}/QCD.txt -n 1  --nmax ${nmax}   -e ${i}  --skim  SkimTree_HNMultiLepBDT
    
done
