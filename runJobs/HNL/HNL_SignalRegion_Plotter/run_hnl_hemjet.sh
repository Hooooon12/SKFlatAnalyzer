analyzer=HNL_SignalRegion_Plotter
rundir=HNL_SignalRegion_Plotter
runPATH=${SKFlat_WD}/runJobs/HNL/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=30
njobs_sig=5
njobs_sig_syst=20
njobs_data=100
nmax=600
skim=' '
declare  -a era_list=("2018")




if [[ $1 == "" ]]; then

    declare  -a flag_list=()

    for i in "${flag_list[@]}"
    do
	
	#SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e 2018  --skim SkimTree_HNMultiLepBDT --userflags ${i} &
        #SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e 2018 --skim SkimTree_HNMultiLepBDT --userflags ${i}&
        #SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e 2018 --skim SkimTree_HNMultiLepBDT --userflags ${i} &
	
	SKFlat.py -a $analyzer  -l $datapath/DL/2018_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e 2018  --skim SkimTree_HNMultiLepBDT  --userflags ${i},RunFake   &
        SKFlat.py -a $analyzer  -l $datapath/DL/2018_DiLepton_EMu.txt      -n 100    --nmax ${nmax}   -e 2018  --skim SkimTree_HNMultiLepBDT  --userflags ${i},RunFake   &

	#SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS.txt             -n 50        --nmax ${nmax}   -e 2018  --skim SkimTree_HNMultiLepBDT --userflags ${i},RunPrompt &
        #SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS2.txt            -n 200       --nmax ${nmax}   -e 2018  --skim SkimTree_HNMultiLepBDT --userflags ${i},RunPrompt &
        #SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv.txt                 -n 50        --nmax ${nmax}   -e 2018 --skim SkimTree_HNMultiLepBDT  --userflags ${i},RunConv & 
        ##SKFlat.py -a $analyzer  -l $mcpath/Conv/ConvWG.txt                 -n 50        --nmax ${nmax}   -e 2018 --skim SkimTree_DileptonBDT  --userflags ${i},RunConv & 
        #SKFlat.py -a $analyzer  -l $datapath/DL/2018_DiLepton_EE.txt      -n 200    --nmax ${nmax}   -e 2018 --skim SkimTree_DileptonBDT  --userflags ${i},RunCF 
	
    done
    
fi


