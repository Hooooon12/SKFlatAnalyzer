analyzer=HNL_SignalRegion_Plotter
rundir=HNL_SignalRegion_Plotter
runPATH=${SKFlat_WD}/runJobs/HNL/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=30
njobs_sig=20
njobs_data=100
nmax=300
skim=' '
#declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")
declare  -a era_list=("2017")


if [[ $1 == "M400" ]]; then

    SKFlat.py -a $analyzer  -i DYTypeI_DF_M400_private  -n 10  --nmax ${nmax}   -e 2017  --skim SkimTree_HNMultiLepBDT&

fi



if [[ $1 == "DY" ]]; then

    SKFlat.py -a $analyzer  -i DYTypeI_DF_M300_private  -n 10  --nmax ${nmax}   -e 2017 --skim SkimTree_HNMultiLepBDT&
    SKFlat.py -a $analyzer  -i DYTypeI_DF_M400_private  -n 10  --nmax ${nmax}   -e 2017 --skim SkimTree_HNMultiLepBDT&

fi



if [[ $1 == "SSWW" ]]; then

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $sigpath/Private/SSWWOpt.txt  -n $njobs  --nmax ${nmax}   -e ${i} &
        SKFlat.py -a $analyzer  -l $sigpath/Private/SSWWOpt.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
    done

fi



if [[ $1 == "CONV" ]]; then

    for i in "${era_list[@]}"
    do

  SKFlat.py -a $analyzer  -l $mcpath/Conv.txt  -n 20  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunConv &                                      

    done
    
fi

if [[ $1 == "FAKE" ]]; then

    declare  -a era_list=("2017")
    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake   &
        SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake   &
    done

fi


if [[ $1 == "SIG" ]]; then

    declare  -a era_list=("2017")
    for i in "${era_list[@]}"
    do

	SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT &
	SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
	SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
    done
    
fi

if [[ $1 == "" ]]; then

declare  -a era_list=("2018")
#declare  -a era_list=("2016preVFP" "2016postVFP" "2017" "2018")
#declare  -a era_list=("2016preVFP" "2016postVFP")
#declare  -a era_list=("2016postVFP")

    for i in "${era_list[@]}"
    do
 
##TEST##
#SKFlat.py -a $analyzer  -i DYTypeI_DF_M100_private  -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &

######### No Syst ################
## Signals ##
#SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  &
#SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
#SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
#SKFlat.py -a $analyzer  -l $sigpath/Private/Weinberg.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &

## DATA ##
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &

## Prompt ##
#SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS.txt             -n 100        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunPrompt &
#SKFlat.py -a $analyzer  -i ZZTo4L_powheg             -n 200        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunPrompt &

## Conv ##
#SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv.txt                 -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunConv &
#SKFlat.py -a $analyzer  -l $mcpath/Conv/ConvWG.txt               -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags RunConv &

## Fake ##
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake   &
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake   &
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake   &

## CF ##
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags RunCF &

######### RunSyst ################
#SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig_syst  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst&
#SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig_syst  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunSyst&
#SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig_syst  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunSyst&

#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,RunFake   &
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,RunFake   &
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,RunFake   &

#SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS.txt             -n 200        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunSyst,RunPrompt &
#SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS2.txt            -n 200       --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunSyst,RunPrompt &

#SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv.txt                 -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunSyst,RunConv &
#SKFlat.py -a $analyzer  -l $mcpath/Conv/ConvWG.txt                 -n 100        --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags RunSyst,RunConv &

#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 200    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags RunSyst,RunCF

######### HighPt Muon ################

## Signals ##
#SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunPrompt,RunHighPt &
#SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunPrompt,RunHighPt &
#SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunPrompt,RunHighPt &
#SKFlat.py -a $analyzer  -l $sigpath/Private/Weinberg.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunPrompt,RunHighPt &

## DATA ##
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_EMu.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &

## Prompt ##
#SKFlat.py -a $analyzer  -l $mcpath/Prompt/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunPrompt,RunHighPt &
#SKFlat.py -a $analyzer  -i ZZTo4L_powheg             -n 200        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunPrompt,RunHighPt &

## Conv ##
#SKFlat.py -a $analyzer  -l $mcpath/Conv/Conv.txt                 -n 10        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunConv,RunHighPt &

## Fake ##
#SKFlat.py -a $analyzer  -l $datapath/DL/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake,RunHighPt &

declare  -a flag_list=("Syst_Muon,CompareTuneP" "ScaleHEMJet" "RemoveHEMJet")

    for j in "${flag_list[@]}"
    do
        
        SKFlat.py -a $analyzer  -l $sigpath/Private/SSWW.txt  -n $njobs_sig  --nmax ${nmax}   -e 2018 --skim SkimTree_HNMultiLepBDT --userflags ${j} &
        SKFlat.py -a $analyzer  -l $sigpath/Private/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e 2018 --skim SkimTree_HNMultiLepBDT --userflags ${j} &
        SKFlat.py -a $analyzer  -l $sigpath/Private/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e 2018 --skim SkimTree_HNMultiLepBDT --userflags ${j} &
        
    done

    done





fi
