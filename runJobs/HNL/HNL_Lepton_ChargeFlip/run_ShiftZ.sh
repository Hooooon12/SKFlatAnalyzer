analyzer=HNL_Lepton_ChargeFlip
rundir=HNL_Lepton_ChargeFlip
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Data/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
njobs=250
nmax=400
skim=' '
declare  -a era_list=("2016preVFP"  "2016postVFP"  "2017" "2018")

declare -a era_list=("2018")
for i in "${era_list[@]}"
do
    
    
    SKFlat.py -a $analyzer  -i DYJets            -n 600  --nmax 500   -e ${i}  --skim SkimTree_DileptonBDT --userflags ShiftEnergyZ&
    SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO -n 600  --nmax 500   -e ${i}  --skim SkimTree_DileptonBDT --userflags ShiftEnergyZ&
       
done
