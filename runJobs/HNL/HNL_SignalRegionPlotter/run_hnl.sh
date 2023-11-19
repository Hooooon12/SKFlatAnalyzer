analyzer=HNL_SignalRegionPlotter
rundir=HNL_SignalRegionPlotter
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

if [[ $1 == "WG" ]]; then

    SKFlat.py -a $analyzer  -i WGToLNuG -n 100   --nmax ${nmax}  -e 2017 --skim SkimTree_HNMultiLepBDT&
    
fi

if [[ $1 == "M1000" ]]; then

    SKFlat.py -a $analyzer  -i VBFTypeI_DF_M1000_private  -n 10  --nmax ${nmax}   -e 2017  --skim SkimTree_HNMultiLepBDT&
    SKFlat.py -a $analyzer  -i DYTypeI_DF_M1000_private  -n 10  --nmax ${nmax}   -e 2017  --skim SkimTree_HNMultiLepBDT&

fi

if [[ $1 == "M400" ]]; then

    SKFlat.py -a $analyzer  -i DYTypeI_DF_M400_private  -n 10  --nmax ${nmax}   -e 2017  --skim SkimTree_HNMultiLepBDT&

fi



if [[ $1 == "DY" ]]; then

    SKFlat.py -a $analyzer  -i DYTypeI_DF_M100_private  -n 10  --nmax ${nmax}   -e 2017 --skim SkimTree_HNMultiLepBDT&
    SKFlat.py -a $analyzer  -i DYTypeI_DF_M1000_private  -n 10  --nmax ${nmax}   -e 2017 --skim SkimTree_HNMultiLepBDT&

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
	SKFlat.py -a $analyzer  -l $mcpath/Fake.txt  -n 50  --nmax 300   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunFake  &
    done

fi


if [[ $1 == "CF" ]]; then
    

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -l $mcpath/CF.txt  -n 10  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunCF &

	#SKFlat.py -a $analyzer  -l $mcpath/MCMerge.txt  -n 20  --nmax ${nmax}   -e ${i}   --skim SkimTree_HNMultiLepBDT &


    done

fi

if [[ $1 == "" ]]; then

    declare  -a era_list=("2017")

    for i in "${era_list[@]}"
    do

#SKFlat.py -a $analyzer  -l $sigpath/SSWWOpt.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}    &                                                            
#SKFlat.py -a $analyzer  -l $sigpath/DYOpt.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i}  &                                                             
#SKFlat.py -a $analyzer  -l $sigpath/VBFOpt.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i}  &                                                             
SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt  -n $njobs_sig  --nmax ${nmax}  -e ${i}  --skim SkimTree_HNMultiLepBDT  &
SKFlat.py -a $analyzer  -l $sigpath/DY.txt    -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
SKFlat.py -a $analyzer  -l $sigpath/VBF.txt   -n $njobs_sig  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT &
#SKFlat.py -a $analyzer  -l $mcpath/Conv.txt  -n 10  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunConv &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i}  --skim SkimTree_DileptonBDT --userflags RunCF &
#SKFlat.py -a $analyzer  -l $mcpath/CF.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT  --userflags RunCF &
SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunFake &
#SKFlat.py -a $analyzer  -l $mcpath/Fake.txt  -n 10  --nmax 300   -e ${i}  --skim SkimTree_HNMultiLepBDT --userflags RunFake  &
SKFlat.py -a $analyzer  -l $mcpath/PromptSS.txt  -n 100  --nmax ${nmax}   -e ${i}   --skim SkimTree_HNMultiLepBDT --userflags RunPrompt &

########### MC closrue test ############
#SKFlat.py -a $analyzer -i TTLL_powheg -n 400 --nmax ${nmax} -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunFakeClosurePred &
#SKFlat.py -a $analyzer -i TTLJ_powheg -n 400 --nmax ${nmax} -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunFakeClosurePred &
#SKFlat.py -a $analyzer -i TTJJ_powheg -n 400 --nmax ${nmax} -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunFakeClosurePred &
#SKFlat.py -a $analyzer -i TTLL_powheg -n 400 --nmax ${nmax} -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunFakeClosureObs &
#SKFlat.py -a $analyzer -i TTLJ_powheg -n 400 --nmax ${nmax} -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunFakeClosureObs &
#SKFlat.py -a $analyzer -i TTJJ_powheg -n 400 --nmax ${nmax} -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunFakeClosureObs &
#SKFlat.py -a $analyzer -l /data6/Users/jihkim/SKFlatAnalyzer/runJobs/HNL/HNL_SignalRegionPlotter/Bkg/FakeClosure.txt -n 400 --nmax ${nmax} -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunFakeClosurePred &
#SKFlat.py -a $analyzer -l /data6/Users/jihkim/SKFlatAnalyzer/runJobs/HNL/HNL_SignalRegionPlotter/Bkg/FakeClosure.txt -n 400 --nmax ${nmax} -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunFakeClosureObs &

# Data
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLepBDT &

# Tests
#SKFlat.py -a $analyzer  -l $datapath/${i}_DiLepton_MuMu_B.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i}  --skim SkimTree_HNMultiLep --userflags RunFake &
#SKFlat.py -a $analyzer  -i WZTo3LNu_amcatnlo -n 100  --nmax ${nmax}   -e ${i}   --skim SkimTree_HNMultiLepBDT &
#SKFlat.py -a $analyzer  -i ZZTo4L_powheg -n 100  --nmax ${nmax}   -e ${i}   --skim SkimTree_HNMultiLep &
#SKFlat.py -a $analyzer -i TTLJ_powheg -n 400 --nmax ${nmax} -e ${i} --skim SkimTree_HNMultiLepBDT --userflags RunFakeClosurePred &
        
    done

fi
