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

    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -i DYTypeI_DF_M500_private  -n 20  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
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



if [[ $1 == "SIG" ]]; then

    declare  -a era_list=("2018")
    for i in "${era_list[@]}"
    do
	
	SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFullSyst  &
	SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
	SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
        SKFlat.py -a $analyzer  -l $sigpath/Private/Weinberg.txt   -n 40  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
	SKFlat.py -a $analyzer  -l $sigpath/Official/SSWWOfficial.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  &
	SKFlat.py -a $analyzer  -l $sigpath/Official/DYOfficial.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
        SKFlat.py -a $analyzer  -l $sigpath/Official/VBFOfficial.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &

    done
    
fi

if [[ $1 == "DATA" ]]; then

    declare  -a era_list=("2017")

    for i in "${era_list[@]}"
    do
        #SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &
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

if [[ $1 == "MergeCharge" ]]; then

    declare  -a era_list=("2017")

    #"2018" "2016preVFP" "2016postVFP")
    
    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv.txt                 -n 50        --nmax 500   -e ${i} --skim SkimTree_DileptonBDT  --userflags MergeCharge,RunConv &
    done
fi


if [[ $1 == "" ]]; then

    declare  -a era_list=("2017" "2018" "2016preVFP" "2016postVFP")

    for i in "${era_list[@]}"
    do
	
        #SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT &
	#SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
        #SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
	#SKFlat.py -a $analyzer  -l $sigpath/Private/Weinberg.txt   -n 40  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  

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


if [[ $1 == "HighPt" ]]; then
    
    declare  -a era_list=("2018")
    
    for i in "${era_list[@]}"
    do

        SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunHighPt&
        SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunHighPt&
        SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunHighPt&
        SKFlat.py -a $analyzer  -l $sigpath/Private/Weinberg.txt   -n 40  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunHighPt

        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunHighPt  &
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunHighPt,RunFake   &

        SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunHighPt,RunPrompt &
        SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS2.txt            -n 200       --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunHighPt,RunPrompt &
        SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv.txt                 -n 10        --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags RunHighPt,RunConv &
       
    done


    for i in "${era_list[@]}"
    do
	
        SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunHighPt,HighPtTrigger&
        SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunHighPt,HighPtTrigger&
        SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunHighPt,HighPtTrigger&
        SKFlat.py -a $analyzer  -l $sigpath/Private/Weinberg.txt   -n 40  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunHighP,HighPtTriggert
	
        SKFlat.py -a $analyzer  -l $datapath/SL/${i}_SingleLepton_Mu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunHighPt,HighPtTrigger  &
        SKFlat.py -a $analyzer  -l $datapath/SL/${i}_SingleLepton_Mu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunHighPt,HighPtTrigger,RunFake   &

        SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunHighPt,HighPtTrigger,RunPrompt &
        SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS2.txt            -n 200       --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunHighPt,HighPtTrigger,RunPrompt &
	SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv.txt                 -n 10        --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags RunHighPt,HighPtTrigger,RunConv &
    done
fi
