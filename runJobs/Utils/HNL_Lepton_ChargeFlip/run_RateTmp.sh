analyzer=HNL_Lepton_ChargeFlip
rundir=HNL_Lepton_ChargeFlip
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Data/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
njobs=250
nmax=400
skim=' '
declare  -a era_list=("2016postVFP")
-
declare  -a flag_list=("ElCFRates")


SKFlat.py -a $analyzer -i DYJets_MG_HT-200to400  -n 100  --nmax 400   -e 2016postVFP  --skim SkimTree_DileptonBDT --userflags ElCFRates&
SKFlat.py -a $analyzer -i DYJets_MG_HT-70to100  -n 100  --nmax 400   -e 2016postVFP  --skim SkimTree_DileptonBDT --userflags ElCFRates&
