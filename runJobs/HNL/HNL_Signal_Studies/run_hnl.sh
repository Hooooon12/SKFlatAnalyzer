analyzer=HNL_Signal_Studies
rundir=HNL_Signal_Studies
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
datapath=${SKFlat_WD}/runJobs/HNL/${analyzer}/DATA/
njobs=20
njobs_data=20
nmax=400
skim=' '
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

declare  -a era_list=( "2018")


if [[ $1 == "WWJet" ]]; then

    SKFlat.py -a $analyzer   -l $sigpath/SSWW.txt -n 10  --nmax ${nmax}   -e 2018  --skim SkimTree_HNMultiLepBDT --userflags WWJet&

fi

if [[ $1 == "Sig" ]]; then

    declare  -a era_list=("2017")

    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT&
	SKFlat.py -a $analyzer  -l $sigpath/DY.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT&
	SKFlat.py -a $analyzer  -l $sigpath/VBF.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT&

        SKFlat.py -a $analyzer  -l $sigpath/SSWWOfficial.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT&
        SKFlat.py -a $analyzer  -l $sigpath/DYOfficial.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT&
        SKFlat.py -a $analyzer  -l $sigpath/VBFOfficial.txt  -n $njobs  --nmax ${nmax}   -e ${i} --skim SkimTree_HNMultiLepBDT&

    done

fi


if [[ $1 == "DY" ]]; then

    declare  -a era_list=("2017")

    for i in "${era_list[@]}"
    do
        #SKFlat.py -a $analyzer  -i DYTypeI_DF_M100_private  -n 10  --nmax 500   -e ${i} --skim SkimTree_HNMultiLepBDT &
	SKFlat.py -a $analyzer  -i DYJets  -n 10  --nmax 500   -e ${i} --skim SkimTree_DileptonBDT &
    done

fi

if [[ $1 == "CheckMuonInEff" ]]; then

    SKFlat.py -a $analyzer  -i DYTypeI_DF_M3000_private  -n 10  --nmax 500   -e 2017 --skim SkimTree_HNMultiLepBDT --userflags CheckMuonInEff &
    SKFlat.py -a $analyzer  -i DYTypeI_DF_M1000_private  -n 10  --nmax 500   -e 2017 --skim SkimTree_HNMultiLepBDT --userflags CheckMuonInEff &
    

fi



if [[ $1 == "Z" ]]; then

    declare  -a era_list=("2018")

    for i in "${era_list[@]}"
    do
        #SKFlat.py -a $analyzer  -i TTLL_powheg   -n 100  --nmax 300  -e ${i}  --skim SkimTree_DileptonBDT&
        SKFlat.py -a $analyzer  -i DYJets        -n 200  --nmax 2000   -e ${i} --skim SkimTree_DileptonBDT&
        #SKFlat.py -a $analyzer  -i TTLJ_powheg   -n 600  --nmax 400   -e ${i}  --skim SkimTree_HNMultiLepBDT&                                                    

    done

fi


if [[ $1 == "" ]]; then

    declare  -a era_list=("2017")

    for i in "${era_list[@]}"
    do
	
        SKFlat.py -a $analyzer  -l ${mcpath}/Conv.txt    -n 100  --nmax 400  -e ${i}  --skim SkimTree_ConvEventSkim&
        SKFlat.py -a $analyzer  -l ${mcpath}/Fake.txt    -n 100  --nmax 400  -e ${i}  --skim SkimTree_FakeEventSkimBDT&
	SKFlat.py -a $analyzer  -i DYJets  -n 500  --nmax 400   -e ${i} --skim SkimTree_DileptonBDT&

	
    done
fi
