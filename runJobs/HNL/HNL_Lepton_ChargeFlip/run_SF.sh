analyzer=HNL_Lepton_ChargeFlip
rundir=HNL_Lepton_ChargeFlip
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Data/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
njobs=250
nmax=400
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

declare  -a era_list=("2017")

for i in "${era_list[@]}"
do
    SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt  -n 100    --nmax 500       -e ${i}  --skim SkimTree_DileptonBDT --userflag ScaleFactor   &  
    #SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO   -n 100    --nmax 500       -e ${i}  --skim SkimTree_DileptonBDT --userflag ScaleFactor   &       
    SKFlat.py -a $analyzer  -i ZGToLLG   -n 100    --nmax 500       -e ${i}  --skim SkimTree_HNMultiLepBDT --userflag ScaleFactor   &       
    SKFlat.py -a $analyzer  -i WGToLNuG   -n 100    --nmax 500       -e ${i}  --skim SkimTree_HNMultiLepBDT --userflag ScaleFactor   &       
    #SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt  -n 50    --nmax 500       -e ${i}  --skim SkimTree_DileptonBDT --userflag ScaleFactor,RunFakes   &                                                                          
    #SKFlat.py -a $analyzer  -l $mcpath/SSBkg.txt  -n 50    --nmax 500       -e ${i}  --skim SkimTree_DileptonBDT --userflag ScaleFactor,RunBkg   &                                                                                         
    
done