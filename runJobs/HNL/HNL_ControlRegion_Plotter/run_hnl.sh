analyzer=HNL_ControlRegion_Plotter
rundir=HNL_ControlRegion_Plotter
runPATH=${SKFlat_WD}/runJobs/HNL/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=30
njobs_sig=20
njobs_data=200
nmax=300
skim=' '

if [[ $1 == "CF" ]]; then

    declare  -a era_list=("2017" "2016postVFP" "2016preVFP" "2018")
    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags SSMultiLep,RunCF
    done
fi

if [[ $1 == "WG" ]]; then

    declare  -a era_list=("2018")
    for i in "${era_list[@]}"
    do

	 SKFlat.py -a $analyzer  -i WGToLNuG_01J_5f_Pt10_resub  -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SSMultiLep,RunConv&
	 SKFlat.py -a $analyzer  -i WGToLNuG  -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SSMultiLep,RunConv&
    done
fi

if [[ $1 == "" ]]; then

    declare  -a era_list=("2017" "2018" "2016postVFP" "2016preVFP")
    declare  -a era_list=("2017")

    for i in "${era_list[@]}"
    do

#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags SSMultiLep &
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags SSMultiLep &
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt      -n 100   --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags SSMultiLep &
#SKFlat.py -a $analyzer  -l $mcpath/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags SSMultiLep,RunPrompt &
#SKFlat.py -a $analyzer  -l $mcpath/Conv.txt                 -n 10        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT   --userflags SSMultiLep,RunConv&

#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags SSMultiLep,RunFake   &
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags SSMultiLep,RunFake   &
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt     -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags SSMultiLep,RunFake   &
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_DileptonBDT    --userflags SSMultiLep,RunCF &

#SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton.txt  -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake  &
#SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton.txt  -n 100      --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  &


################OS_VR##################
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags OS_VR &
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags OS_VR &
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt      -n 100   --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags OS_VR &
#SKFlat.py -a $analyzer  -l $mcpath/PromptOS.txt             -n 300        --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags OS_VR &
#SKFlat.py -a $analyzer  -l $mcpath/Conv.txt             -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags OS_VR &
#SKFlat.py -a $analyzer  -l $mcpath/Conv2.txt             -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags OS_VR &

#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags OS_VR,RunFake &
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags OS_VR,RunFake &
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt      -n 100   --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags OS_VR,RunFake &
#SKFlat.py -a $analyzer  -l $mcpath/PromptOS.txt             -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT --userflags OS_VR,RunFake &

#############SS_CR###############
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SS_CR &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SS_CR &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt      -n 100   --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SS_CR &
SKFlat.py -a $analyzer  -l $mcpath/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SS_CR,RunPrompt &
SKFlat.py -a $analyzer  -i ZZTo4L_powheg                    -n 200       --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SS_CR,RunPrompt &
SKFlat.py -a $analyzer  -l $mcpath/Conv.txt                 -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SS_CR,RunConv &
SKFlat.py -a $analyzer  -l $mcpath/Conv2.txt               -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SS_CR,RunConv &

SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags SS_CR,RunFake   &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags SS_CR,RunFake   &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt     -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags SS_CR,RunFake   &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_DileptonBDT    --userflags SS_CR,RunCF &

#SKFlat.py -a $analyzer  -l $mcpath/CF.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SS_CR,RunCF &

###########LLL_VR################
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt      -n 100   --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR &
SKFlat.py -a $analyzer  -l $mcpath/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR,RunPrompt &
SKFlat.py -a $analyzer  -i ZZTo4L_powheg                    -n 200       --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR,RunPrompt &
SKFlat.py -a $analyzer  -l $mcpath/Conv.txt                 -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR,RunConv&
SKFlat.py -a $analyzer  -l $mcpath/Conv2.txt                 -n 100      --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR,RunConv &

SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR,RunFake   &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR,RunFake   &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt     -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR,RunFake   &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_DileptonBDT    --userflags LLL_VR,RunCF &

###########VBF_CR##############
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt      -n 100   --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR &
SKFlat.py -a $analyzer  -l $mcpath/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR,RunPrompt &
SKFlat.py -a $analyzer  -i ZZTo4L_powheg                    -n 200       --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR,RunPrompt &
SKFlat.py -a $analyzer  -l $mcpath/Conv.txt                 -n 20        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR,RunConv&
SKFlat.py -a $analyzer  -l $mcpath/Conv2.txt                 -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR,RunConv&

SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR,RunFake   &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR,RunFake   &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt     -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR,RunFake   &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_DileptonBDT    --userflags VBF_CR,RunCF &

################Signals##################
#SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt              -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SS_CR &
#SKFlat.py -a $analyzer  -l $sigpath/DY.txt                -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SS_CR &
#SKFlat.py -a $analyzer  -l $sigpath/VBF.txt               -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags SS_CR &

#SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt              -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR &
#SKFlat.py -a $analyzer  -l $sigpath/DY.txt                -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR &
#SKFlat.py -a $analyzer  -l $sigpath/VBF.txt               -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags LLL_VR &

#SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt              -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR &
#SKFlat.py -a $analyzer  -l $sigpath/DY.txt                -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR &
#SKFlat.py -a $analyzer  -l $sigpath/VBF.txt               -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags VBF_CR &

    done
    
fi
