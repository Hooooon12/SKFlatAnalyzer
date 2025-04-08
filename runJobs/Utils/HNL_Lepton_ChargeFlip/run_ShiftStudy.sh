analyzer=HNL_Lepton_ChargeFlip
rundir=HNL_Lepton_ChargeFlip
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Data/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
njobs=250
nmax=400
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")
declare  -a era_list=("2018")

for i in "${era_list[@]}"
do
    
    SKFlat.py -a $analyzer  -i DYJets           -n 50  --nmax 500   -e ${i}  --skim SkimTree_CFEventSkim --userflags ShiftCFStudy&
    SKFlat.py -a $analyzer  -i TTLL_powheg      -n 50  --nmax 500   -e ${i}  --skim SkimTree_CFEventSkim --userflags ShiftCFStudy&
    #    SKFlat.py -a $analyzer  -i WJets_MG         -n 600  --nmax 500   -e ${i}  --skim SkimTree_DileptonBDT --userflags ShiftEnergy&
     #   SKFlat.py -a $analyzer  -i TTLJ_powheg      -n 600  --nmax 500   -e ${i}  --skim SkimTree_DileptonBDT --userflags ShiftEnergy&
        
done
