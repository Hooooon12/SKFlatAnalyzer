analyzer=HNL_ControlRegion
rundir=HNL_ControlRegion
runPATH=${SKFlat_WD}/runJobs/HNL/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=30
njobs_sig=2
njobs_data=200
nmax=500
skim=' '

if [[ $1 == "" ]]; then

    declare  -a era_list=("2017" "2018" "2016postVFP" "2016preVFP")

    for i in "${era_list[@]}"
    do
        #### Data                                                                                                                                                                                                                             
	SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags SSMultiLep &
	SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags SSMultiLep &
	SKFlat.py -a $analyzer  -l $mcpath/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  SSMultiLep,RunPrompt &
	SKFlat.py -a $analyzer  -l $mcpath/PromptSS2.txt            -n 200       --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  SSMultiLep,RunPrompt &
	SKFlat.py -a $analyzer  -l $mcpath/Conv.txt                 -n 10        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SSMultiLep,RunConv&
	SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags SSMultiLep,RunFake   &
	SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags SSMultiLep,RunFake   &
	SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags SSMultiLep,RunCF &
	#SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton.txt  -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake  &
        #SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton.txt  -n 100      --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &
	
    done
    
fi

if [[ $1 == "Scan" ]]; then

    declare  -a era_list=("2017" "2018" "2016postVFP" "2016preVFP")
    declare  -a eta_list=("BB" "EC")

    for i in "${era_list[@]}"
    do
	for j in "${eta_list[@]}"
	do
            SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags ScanFakes,${j}   &
            SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags ScanFakes,${j}   &
            SKFlat.py -a $analyzer  -l $mcpath/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  ScanFakes,RunPrompt,${j} &
            SKFlat.py -a $analyzer  -l $mcpath/PromptSS2.txt            -n 200       --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags  ScanFakes,RunPrompt,${j} &
            SKFlat.py -a $analyzer  -l $mcpath/Conv.txt                 -n 10        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags ScanFakes,RunConv,${j}&
            SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags ScanFakes,RunFake,${j}   &
            SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags ScanFakes,RunFake,${j}  &
            SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags ScanFakes,RunCF,${j} &
	done
    done
fi

if [[ $1 == "CF" ]]; then

    declare  -a era_list=("2017" "2018" "2016postVFP" "2016preVFP")
    declare  -a eta_list=("BB" "EC")

    for i in "${era_list[@]}"
    do
        for j in "${eta_list[@]}"
        do
	    
            SKFlat.py -a $analyzer  -l runJobs/HNL/HNL_ControlRegion/Bkg/CF.txt      -n 400    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags ScanFakes,RunCF,${j} &
	done
    done

fi

