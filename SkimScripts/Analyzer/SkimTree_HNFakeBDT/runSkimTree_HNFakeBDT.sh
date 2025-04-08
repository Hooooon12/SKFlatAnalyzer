analyzer=SkimTree_HNFakeBDT
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/SampleLists/Bkg/
datapath=${SKFlat_WD}/SkimScripts/SampleLists/Data/
njobs=400
njobs_data=400
nmax=400
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")
declare  -a era_list=("2017")
if [[ $1 == "" ]]; then
    
    for i in "${era_list[@]}"
    do
        #SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_FakeMu.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
        #SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_FakeEl.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
        #SKFlat.py -a $analyzer  -l $mcpath/MCNoSkim.txt      -n ${njobs}       --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake& 

	SKFlat.py -a $analyzer  -i DYJetsToMuMu_MiNNLO   -n ${njobs}       --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake& 
	SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO -n ${njobs}           --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
	SKFlat.py -a $analyzer  -i TTLL_powheg -n ${njobs}                 --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake
	SKFlat.py -a $analyzer  -i SingleTop_sch_Lep -n ${njobs}           --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake
	SKFlat.py -a $analyzer  -i SingleTop_tch_antitop_Incl -n ${njobs}  --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake
	SKFlat.py -a $analyzer  -i SingleTop_tch_top_Incl -n ${njobs}          --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
	SKFlat.py -a $analyzer  -i SingleTop_tW_antitop_NoFullyHad -n ${njobs} --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
	SKFlat.py -a $analyzer  -i SingleTop_tW_top_NoFullyHad -n ${njobs}     --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
	SKFlat.py -a $analyzer  -i WW_pythia -n ${njobs}       --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
	SKFlat.py -a $analyzer  -i WZ_pythia -n ${njobs}       --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
	SKFlat.py -a $analyzer  -i ZZ_pythia -n ${njobs}       --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
	#SKFlat.py -a $analyzer  -l $mcpath/FakeRateMainMC.txt             -n ${njobs}       --nmax ${nmax}   -e ${i} --skim SkimTree_HNFake&
    done
fi
