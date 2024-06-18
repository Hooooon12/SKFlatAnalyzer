analyzer=HNL_SignalRegion_Plotter
rundir=HNL_SignalRegion_Plotter
runPATH=${SKFlat_WD}/runJobs/HNL/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/Data/

njobs=30
njobs_sig=20
njobs_data=100
nmax=400
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
        SKFlat.py -a $analyzer  -l $sigpath/SSWWOpt.txt  -n $njobs  --nmax ${nmax}   -e ${i} &
        SKFlat.py -a $analyzer  -l $sigpath/SSWWOpt.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
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

	SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT &
	SKFlat.py -a $analyzer  -l $sigpath/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
	SKFlat.py -a $analyzer  -l $sigpath/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
    done
    
fi

if [[ $1 == "" ]]; then

#declare  -a era_list=("2018")
#declare  -a era_list=("2016preVFP" "2016postVFP" "2017" "2018")
#declare  -a era_list=("2016preVFP" "2016postVFP")
declare  -a era_list=("2018")

    for i in "${era_list[@]}"
    do
 
##TEST##
#SKFlat.py -a $analyzer  -i DYTypeI_DF_M100_private  -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &

## Signals ##
SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  &
SKFlat.py -a $analyzer  -l $sigpath/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
SKFlat.py -a $analyzer  -l $sigpath/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &

## DATA ##
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT    &

## Prompt ##
SKFlat.py -a $analyzer  -l $mcpath/PromptSS.txt             -n 20        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunPrompt &
SKFlat.py -a $analyzer  -i ZZTo4L_powheg             -n 200        --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunPrompt &

## Conv ##
SKFlat.py -a $analyzer  -l $mcpath/Conv.txt                 -n 10        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunConv &
SKFlat.py -a $analyzer  -l $mcpath/Conv2.txt                 -n 10        --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunConv &

## Fake ##
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu.txt      -n 100  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake   &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake   &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EMu.txt      -n 100    --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT  --userflags RunFake   &

## CF ##
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_EE.txt      -n 100    --nmax ${nmax}   -e ${i} --skim SkimTree_DileptonBDT  --userflags RunCF &

    done

fi
