analyzer=HNL_ControlRegion_Plotter
rundir=HNL_ControlRegion_Plotter
runPATH=${SKFlat_WD}/runJobs/HNL/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=30
njobs_sig=2
njobs_data=200
nmax=400
skim=' '

if [[ $1 == "" ]]; then

    declare  -a era_list=("2017" "2016postVFP" "2016preVFP" "2018")
    for i in "${era_list[@]}"
    do
        #### Data                                                                                                                                                                             	
	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags LLL &
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags LLL &
	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags LLL 
	#### Prompt
	SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  LLL,RunPrompt &
	SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS2.txt            -n 200       --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  LLL,RunPrompt 
	
	### Conv
	SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv.txt          -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL,RunConv&
	SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv2.txt         -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL,RunConv

	### Fakes
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags LLL,RunFake   &
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags LLL,RunFake   &
	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags LLL,RunFake   

    done
    
fi

if [[ $1 == "AltMET" ]]; then

    declare  -a era_list=("2017" "2016postVFP" "2016preVFP" "2018")
    for i in "${era_list[@]}"
    do
        #### Data                                                                                                                                                                                     
        SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags LLL,UseMET2ST &
        SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags LLL,UseMET2ST &
        SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags LLL,UseMET2ST
        #### Prompt                                                                                                                                                                                   
        SKFlat.py -a $analyzer  -l $mcpath/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  LLL,UseMET2ST,RunPrompt &
        SKFlat.py -a $analyzer  -l $mcpath/PromptSS2.txt            -n 200       --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  LLL,UseMET2ST,RunPrompt

        ### Conv                                                                                                                                                                                      
        SKFlat.py -a $analyzer  -l $mcpath/Conv.txt          -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL,UseMET2ST,RunConv&
        SKFlat.py -a $analyzer  -l $mcpath/Conv2.txt         -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL,UseMET2ST,RunConv

        ### Fakes                                                                                                                                                                                     
        SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags LLL,UseMET2ST,RunFake   &
        SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags LLL,UseMET2ST,RunFake   &
        SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags LLL,UseMET2ST,RunFake

    done

fi


