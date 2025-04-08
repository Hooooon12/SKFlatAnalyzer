analyzer=HNL_Lepton_ChargeFlip
rundir=HNL_Lepton_ChargeFlip
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Data/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
njobs=250
nmax=400
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

declare  -a flag_list=("ElCFRates")

for i in "${era_list[@]}"
do
    for j in "${flag_list[@]}"
    do
	
	SKFlat.py -a $analyzer -i DYJets_MG_HT-70to100   -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_MG_HT-100to200  -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_MG_HT-200to400  -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_MG_HT-400to600  -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_MG_HT-600to800  -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_MG_HT-800to1200 -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_MG_HT-1200to2500 -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_MG_HT-2500toInf -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_Pt-50To100      -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_Pt-100To250     -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_Pt-250To400     -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_Pt-400To650     -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i DYJets_Pt-650ToInf     -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j
	SKFlat.py -a $analyzer -i WJets_MG -n 200  --nmax 400   -e ${i}  --skim SkimTree_BDT --userflags $j&
        SKFlat.py -a $analyzer -i DYJets                 -n 600  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
        SKFlat.py -a $analyzer -i DYJets_MG              -n 100  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j
        SKFlat.py -a $analyzer -i DYJetsToEE_MiNNLO      -n 200  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&
	SKFlat.py -a $analyzer -i TTLJ_powheg            -n 600  --nmax 400   -e ${i}  --skim SkimTree_BDT --userflags $j&
        SKFlat.py -a $analyzer -i TTLL_powheg            -n 600  --nmax 400   -e ${i}  --skim SkimTree_DileptonBDT --userflags $j&

    done
done
