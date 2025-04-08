analyzer=HNL_Lepton_ChargeFlip
rundir=HNL_Lepton_ChargeFlip
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Data/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
njobs=250
nmax=400
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

declare  -a flag_list=("ClosureTest")

for i in "${era_list[@]}"
do
    for j in "${flag_list[@]}"
    do
	SKFlat.py -a $analyzer -i DYJetsToEE_MiNNLO   -n 200  --nmax 200   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i TTLL_powheg         -n 200  --nmax 200   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
    done
done
