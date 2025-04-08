analyzer=HNL_Lepton_FakeRate_MC
rundir=HNL_Lepton_FakeRate_MC
mcpath=${SKFlat_WD}/runJobs/HNL/${rundir}/mc_lists/
datapath=${SKFlat_WD}/runJobs/HNL/${rundir}/data_lists/
njobs=500
njobs_data=100
nmax=500
declare  -a era_list=("2016preVFP" "2016postVFP" "2017" "2018")


declare  -a joblist=("MCClosure" "MCFakes" "Standard" "CheckProfile" "CheckProfileEE")

declare  -a era_list=("2018")
	

for i in "${era_list[@]}"

do
    
    if [[ $1 == "MCClosureDY" ]]; then
	
        SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO -n ${njobs}  --nmax ${nmax}  -e ${i}   --skim SkimTree_DileptonBDT --userflags MCClosure&

    fi

    if [[ $1 == "MCClosure" ]]; then
        SKFlat.py -a $analyzer  -l runJobs/SampleLists/Bkg/W.txt -n ${njobs}  --nmax ${nmax}  -e ${i}   --skim SkimTree_FakeEventSkimBDT --userflags MCClosure& 
        SKFlat.py -a $analyzer  -l $mcpath/MC.txt                -n ${njobs}  --nmax ${nmax}  -e ${i}   --skim SkimTree_DileptonBDT --userflags MCClosure& 
	SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO                -n ${njobs}   --nmax ${nmax}  -e ${i}   --skim SkimTree_DileptonBDT --userflags MCClosure& 
	SKFlat.py -a $analyzer  -i DYJetsToMuMu_MiNNLO                -n ${njobs}   --nmax ${nmax}  -e ${i}   --skim SkimTree_DileptonBDT --userflags MCClosure& 
    fi
    
    if [[ $1 == "MCFakes" ]]; then
	SKFlat.py -a $analyzer  -l ${mcpath}/QCD_${i}_Mu.txt    -n ${njobs}  --nmax ${nmax}  -e ${i}   --skim SkimTree_FakeEventSkimBDT --userflags MCFakes&
	SKFlat.py -a $analyzer  -l ${mcpath}/QCD_${i}_EGamma.txt  -n ${njobs}  --nmax ${nmax}  -e ${i}   --skim SkimTree_FakeEventSkimBDT --userflags MCFakes&
        #SKFlat.py -a $analyzer  -l $mcpath/MC.txt               -n ${njobs}  --nmax ${nmax}  -e ${i}   --skim SkimTree_FakeEventSkimBDT --userflags MCFakes&
    fi

    if [[ $1 == "Standard" ]]; then
        SKFlat.py -a $analyzer  -l ${mcpath}/QCD_${i}_Mu.txt    -n ${njobs}   --nmax ${nmax}  -e ${i}   --skim SkimTree_FakeEventSkimBDT --userflags Standard&
        SKFlat.py -a $analyzer  -l ${mcpath}/QCD_BEnriched.txt  -n ${njobs}   --nmax ${nmax}  -e ${i}   --skim SkimTree_FakeEventSkimBDT --userflags Standard&
        #SKFlat.py -a $analyzer  -l $mcpath/MC.txt               -n ${njobs}   --nmax ${nmax}  -e ${i}   --skim SkimTree_FakeEventSkimBDT --userflags Standard&
    fi


    if [[ $1 == "CheckProfile" ]]; then
	SKFlat.py -a $analyzer  -l ${mcpath}/QCD_${i}_Mu.txt    -n ${njobs}   --nmax ${nmax}  -e ${i}   --skim SkimTree_FakeEventSkimBDT --userflags CheckProfile&
        SKFlat.py -a $analyzer  -i TTLJ_powheg                  -n ${njobs}   --nmax ${nmax}  -e ${i}   --skim SkimTree_FakeEventSkimBDT --userflags CheckProfile&
    fi

    if [[ $1 == "CheckProfileEE" ]]; then
        SKFlat.py -a $analyzer  -l ${mcpath}/QCD_${i}_EGamma.txt    -n ${njobs}   --nmax ${nmax}  -e ${i}   --skim SkimTree_FakeEventSkimBDT --userflags CheckProfileEE&
	SKFlat.py -a $analyzer  -i  TTLJ_powheg                     -n ${njobs}   --nmax ${nmax}  -e ${i}   --skim SkimTree_FakeEventSkimBDT --userflags CheckProfileEE&
    fi
    
done

echo "List of Job Options are:"
for i in "${joblist[@]}"

do
    echo $i
done
