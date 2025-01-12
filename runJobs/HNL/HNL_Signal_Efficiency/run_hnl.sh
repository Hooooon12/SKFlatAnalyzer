analyzer=HNL_Signal_Efficiency
rundir=HNL_Signal_Efficiency
sigpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Signals/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=5
nmax=300
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

if [[ $1 == "" ]]; then


    for i in "${era_list[@]}"
    do
        #SKFlat.py -a $analyzer  -l $sigpath/DY.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT&
        #SKFlat.py -a $analyzer  -l $sigpath/VBF.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT&
        #SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT&
	#SKFlat.py -a $analyzer  -l ${mcpath}/Conv.txt    -n 100  --nmax 400  -e ${i}  --skim SkimTree_ConvEventSkim&
        #SKFlat.py -a $analyzer  -l ${mcpath}/Fake.txt    -n 100  --nmax 400  -e ${i}  --skim SkimTree_FakeEventSkimBDT&
	SKFlat.py -a $analyzer  -i TTLL_powheg    -n 400  --nmax 400  -e ${i}  --skim SkimTree_DileptonBDT &
	SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO         -n 400  --nmax 400  -e ${i}  --skim SkimTree_DileptonBDT &
	
    done
    
fi


if [[ $1 == "Baseline" ]]; then

    for i in "${era_list[@]}"
    do
        #SKFlat.py -a $analyzer  -l $sigpath/DY.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags Baseline_EE&
	#SKFlat.py -a $analyzer  -l $sigpath/VBF.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags Baseline_EE&
        #SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags Baseline_EE&
        SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags Baseline_EE&
	
    done

fi

