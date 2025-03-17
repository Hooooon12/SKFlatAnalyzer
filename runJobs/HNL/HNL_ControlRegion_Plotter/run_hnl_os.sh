analyzer=HNL_ControlRegion_Plotter
rundir=HNL_ControlRegion_Plotter
runPATH=${SKFlat_WD}/runJobs/HNL/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=30
njobs_sig=2
njobs_data=200
nmax=700
skim=' '



if [[ $1 == "OS" ]]; then

    FLAG="OS"
    #declare  -a era_list=("2016postVFP" "2017" "2018" "2016preVFP")
    declare  -a era_list=("2017")

    for i in "${era_list[@]}"
    do
	
	
	#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt       -n ${njobs_data}  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags $FLAG,RunFake  &
        #SKFlat.py -a $analyzer  -l $mcpath/Prompt/VVOS.txt                  -n ${njobs_data}  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags $FLAG,RunFake  &
	
	#### Prompt
	#SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptOS.txt            -n 200    --nmax ${nmax}   -e ${i}  --skim SkimTree_DileptonBDT  --userflags $FLAG,RunPrompt &
	#SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptOS2.txt            -n 200    --nmax ${nmax}   -e ${i}  --skim SkimTree_DileptonBDT  --userflags $FLAG,RunPrompt &
	SKFlat.py -a $analyzer  -l $mcpath/Prompt/DY_Ext.txt   -n 400    --nmax ${nmax}   -e ${i}  --skim SkimTree_DileptonBDT  --userflags $FLAG,RunPrompt &

	### Conv
	#SKFlat.py -a $analyzer  -l $mcpath/Conv/ConvOS.txt          -n 50        --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags $FLAG,RunConv&

        #SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_DileptonBDT  --userflags $FLAG &
	   
		
    done
    
fi
