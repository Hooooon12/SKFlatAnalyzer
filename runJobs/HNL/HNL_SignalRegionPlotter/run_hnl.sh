analyzer=HNL_SignalRegion_Plotter
rundir=HNL_SignalRegion_Plotter
runPATH=${SKFlat_WD}/runJobs/HNL/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=30
njobs_sig=5
njobs_data=100
nmax=400
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")


if [[ $1 == "DY" ]]; then

    declare  -a era_list=("2018")
    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -i DYTypeI_DF_M100_private   -n 20 --nmax 600  -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst&
	#SKFlat.py -a $analyzer  -i DYTypeI_DF_M1000_private   -n 20 --nmax 600  -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst&
	#SKFlat.py -a $analyzer  -i SSWWTypeI_SF_M1000_private -n 20 --nmax 600  -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst&
    done
fi


if [[ $1 == "HEM" ]]; then

    declare  -a era_list=("2018")
    declare  -a flag_list=("ScaleHEMJet,RunNoSyst" "RunNoSyst")
    for i in "${era_list[@]}"
    do
	for j in "${flag_list[@]}"
	do
	    
	    SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags ${j}&
            SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags ${j}&
	    SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags ${j}&
	    SKFlat.py -a $analyzer  -l $sigpath/Private/Weinberg.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags ${j}&
	done
    done

fi




if [[ $1 == "TunePSIG" ]]; then

    declare  -a era_list=("2018")
    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $sigpath/Private/HM_SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  CompareTuneP &
        SKFlat.py -a $analyzer  -l $sigpath/Private/HM_DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags  CompareTuneP & 
        SKFlat.py -a $analyzer  -l $sigpath/Private/HM_VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags  CompareTuneP & 
	SKFlat.py -a $analyzer  -l $sigpath/Private/HM_SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  NoTuneP &
        SKFlat.py -a $analyzer  -l $sigpath/Private/HM_DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags  NoTuneP &
        SKFlat.py -a $analyzer  -l $sigpath/Private/HM_VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags  NoTuneP &

    done
fi


if [[ $1 == "SIG" ]]; then

    declare  -a era_list=("2018")
    for i in "${era_list[@]}"
    do
	
	SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  &
	SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
	SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
        SKFlat.py -a $analyzer  -l $sigpath/Private/Weinberg.txt   -n 40  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
	SKFlat.py -a $analyzer  -l $sigpath/Official/SSWWOfficial.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  &
	SKFlat.py -a $analyzer  -l $sigpath/Official/DYOfficial.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
        SKFlat.py -a $analyzer  -l $sigpath/Official/VBFOfficial.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &

    done
    
fi


if [[ $1 == "SYST" ]]; then

    for i in "${era_list[@]}"
    do
	
        SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFullSyst  &
        SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunFullSyst&
        SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunFullSyst&
        SKFlat.py -a $analyzer  -l $sigpath/Private/Weinberg.txt   -n 40  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunFullSyst&
        SKFlat.py -a $analyzer  -l $sigpath/Official/SSWWOfficial.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFullSyst&
        SKFlat.py -a $analyzer  -l $sigpath/Official/DYOfficial.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunFullSyst&
        SKFlat.py -a $analyzer  -l $sigpath/Official/VBFOfficial.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunFullSyst&

        #SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax 200   -e ${i} --skim SkimTree_DileptonBDT  --userflags RunCF,RunSyst
        #SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 600  --nmax 600   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake,RunSyst   
        #SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 600    --nmax 600   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake,RunSyst   
    done
fi


if [[ $1 == "" ]]; then

    declare  -a era_list=("2017" "2018" "2016preVFP" "2016postVFP")

    for i in "${era_list[@]}"
    do
	
        SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT &
	SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
        SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
	SKFlat.py -a $analyzer  -l $sigpath/Private/Weinberg.txt   -n 40  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  

	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &
	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &
	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   &
	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake   &
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake   &
	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake   &

	SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunPrompt &
        SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS2.txt            -n 200       --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunPrompt &
        SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv.txt                 -n 50        --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags RunConv & 
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags RunCF 

	
    done
    
fi
