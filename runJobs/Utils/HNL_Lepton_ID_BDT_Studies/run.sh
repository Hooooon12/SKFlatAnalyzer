analyzer=HNL_Lepton_ID_BDT_Studies
rundir=HNL_Lepton_ID_BDT_Studies
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Data/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
njobs=250
nmax=800
skim=' SkimTree_Dilepton'
declare  -a era_list=("2018" "2016postVFP" "2016preVFP" "2017")




if [[ $1 == "BDT" ]]; then

    declare  -a era_list=("2018")

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -i DYJetsToMuMu_MiNNLO    -n 400 --nmax 400  -e ${i}       --skim SkimTree_DileptonBDT  --userflags BDTPerType,BDTPlots,BDT &
	SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO    -n 400 --nmax 400  -e ${i}       --skim SkimTree_DileptonBDT  --userflags BDTPerType,BDTPlots,BDT &
        #SKFlat.py -a $analyzer  -i TTLJ_powheg    -n 400 --nmax 400  -e ${i}  --skim SkimTree_DileptonBDT  --userflags BDTPerType,BDTPlots,BDT &
	#SKFlat.py -a $analyzer  -l ${datapath}/DATA_${i}.txt  -n 100  --nmax 400 --userflag BDTPerType,BDTPlots,BDT  -e ${i}  --skim SkimTree_DileptonBDT&


   done
fi


if [[ $1 == "BDTType" ]]; then

    declare  -a era_list=("2018")

    for i in "${era_list[@]}"
    do
        SKFlat.py -a $analyzer  -i DYJetsToMuMu_MiNNLO    -n 500 --nmax 500  -e ${i}  --skim SkimTree_DileptonBDT  --userflags BDTPerType  &
        SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO    -n 500 --nmax 500  -e ${i}  --skim SkimTree_DileptonBDT  --userflags BDTPerType &
        SKFlat.py -a $analyzer  -i TTLJ_powheg    -n 200 --nmax 500  -e ${i}  --skim SkimTree_DileptonBDT  --userflags BDTPerType &

   done
fi
if [[ $1 == "" ]]; then
    
    declare  -a era_list=("2017")
    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -i TTLJ_powheg  -n 300  --nmax 300  -e ${i} --userflag SSBreakdown  --skim SkimTree_FakeEventSkimBDT &
	#SKFlat.py -a $analyzer  -i DYJets    -n 200 --nmax 300  -e ${i}    --userflag SSBreakdown  --skim SkimTree_FakeEventSkimBDT &
	#SKFlat.py -a $analyzer  -i WJets_MG  -n 200  --nmax 300  -e ${i}   --userflag SSBreakdown   --skim SkimTree_FakeEventSkimBDT &
	SKFlat.py -a $analyzer  -l ${mcpath}/QCD_${i}.txt  -n 10  --nmax 300 --userflag SSBreakdown  -e ${i}  --skim SkimTree_FakeEventSkimBDT &
		
    done
fi

if [[ $1 == "OS" ]]; then

    declare  -a era_list=("2017")
    for i in "${era_list[@]}"
    do
        
        SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO    -n 200 --nmax 300  -e ${i}    --userflag BDT   --skim SkimTree_DileptonBDT &                                                                            
    

    done
fi



if [[ $1 == "All" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do
	
	SKFlat.py -a $analyzer  -l ${mcpath}/Fake.txt    -n 250  --nmax 400  -e ${i}  --skim SkimTree_FakeEventSkimBDT&
	SKFlat.py -a $analyzer  -l ${mcpath}/CF.txt      -n 100  --nmax 400  -e ${i}  --skim SkimTree_CFEventSkim&
	SKFlat.py -a $analyzer  -l ${mcpath}/Conv.txt    -n 100  --nmax 400  -e ${i}  --skim SkimTree_ConvEventSkim&
	SKFlat.py -a $analyzer  -l ${mcpath}/Sig.txt     -n 100  --nmax 400  -e ${i}  --skim SkimTree_HNMultiLepBDT&
	
    done
fi


