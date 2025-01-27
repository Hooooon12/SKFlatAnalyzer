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

    declare  -a era_list=("2016postVFP" "2016preVFP")

    for i in "${era_list[@]}"
    do
        #### Data                                                                                                                                                                             	
	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags RunSyst,SSMultiLep &
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags  RunSyst,SSMultiLep &
	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags RunSyst,SSMultiLep 
	
	#### Prompt
	SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  RunSyst,SSMultiLep,RunPrompt &
	SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS2.txt            -n 400       --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  RunSyst,SSMultiLep,RunPrompt &
	
	### Conv
	SKFlat.py -a $analyzer  -l $mcpath/Conv/ConvWG.txt          -n 50        --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags RunSyst,SSMultiLep,RunConv&
	SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv.txt          -n 50        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunSyst,SSMultiLep,RunConv&

	### Fakes
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,SSMultiLep,RunFake   &
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,SSMultiLep,RunFake   &
	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,SSMultiLep,RunFake   
	
	### CF
	SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 400    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags RunSyst,SSMultiLep,RunCF &


        #### Data                                                                                                                                                                               
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags RunSyst,LLL &
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags RunSyst,LLL &
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags RunSyst,LLL 
        #### Prompt
        SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  RunSyst,LLL,RunPrompt &
        SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS2.txt            -n 400       --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  RunSyst,LLL,RunPrompt 
        
        ### Conv
        SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv.txt          -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,LLL,RunConv&
        SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv2.txt         -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,LLL,RunConv

        ### Fakes
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,LLL,RunFake   &
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,LLL,RunFake   &
        SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,LLL,RunFake   

    done
    
fi
