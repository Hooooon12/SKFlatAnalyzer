analyzer=HNL_LeptonID_BDT_KinVar
rundir=HNL_LeptonID_BDT_KinVar
sigpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Signals/
mcpath=${SKFlat_WD}/runJobs/HNL/${analyzer}/Bkg/
njobs=300
nmax=300

declare  -a era_list=("2017" "2018" "2016postVFP" "2016preVFP")

#declare  -a era_list=("2016postVFP" "2016preVFP" "2018")

if [[ $1 == "TT" ]]; then

    nmax=400

    for i in "${era_list[@]}"
    do
	SKFlat.py -a $analyzer  -i  TTLJ_powheg  -n 600  --nmax ${nmax}   -e ${i}  --userflags SeperatePrompt,NoBJet &
    
    done
fi


if [[ $1 == "" ]]; then

    declare  -a era_list=( "2016postVFP" "2016preVFP" "2017" "2018")

    nmax=300
    for i in "${era_list[@]}"
    do
	#SKFlat.py -a $analyzer  -l   $mcpath/QCD_${i}.txt  -n 10  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,ByType,HFB --skim SkimTree_FakeEventSkim&
    	#SKFlat.py -a $analyzer  -l   $mcpath/QCD_${i}.txt  -n 10  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,ByType,HFC --skim SkimTree_FakeEventSkim&
	#SKFlat.py -a $analyzer  -l   $mcpath/QCD_${i}.txt  -n 10  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,ByType,LF  --skim SkimTree_FakeEventSkim&
	SKFlat.py -a $analyzer  -l   $mcpath/QCD_${i}.txt  -n 10  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,ByType,LF,LFMother_1  --skim SkimTree_FakeEventSkim&
	SKFlat.py -a $analyzer  -l   $mcpath/QCD_${i}.txt  -n 10  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,ByType,LF,LFMother_2  --skim SkimTree_FakeEventSkim&
	SKFlat.py -a $analyzer  -l   $mcpath/QCD_${i}.txt  -n 10  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,ByType,LF,LFMother_3  --skim SkimTree_FakeEventSkim&
        SKFlat.py -a $analyzer  -l   $mcpath/QCD_${i}.txt  -n 10  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,ByType,LF,LFMother_not1  --skim SkimTree_FakeEventSkim&
        SKFlat.py -a $analyzer  -l   $mcpath/QCD_${i}.txt  -n 10  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,ByType,LF,LFMother_not2  --skim SkimTree_FakeEventSkim&
        SKFlat.py -a $analyzer  -l   $mcpath/QCD_${i}.txt  -n 10  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,ByType,LF,LFMother_not3  --skim SkimTree_FakeEventSkim&

    done
fi

if [[ $1 == "All" ]]; then

    nmax=400
    for i in "${era_list[@]}"
    do
        #SKFlat.py -a $analyzer  -l $sigpath/DY.txt  -n 10  --nmax ${nmax}   -e ${i}  &
        #SKFlat.py -a $analyzer  -l $sigpath/VBF.txt  -n 10  --nmax ${nmax}   -e ${i} & 
        #SKFlat.py -a $analyzer  -l $sigpath/SSWW.txt  -n 10  --nmax ${nmax}   -e ${i} &
	#SKFlat.py -a $analyzer  -l $mcpath/Prompt.txt      -n 600  --nmax ${nmax}   -e ${i}  --userflags SeperatePrompt &
	SKFlat.py -a $analyzer  -l   $mcpath/Fake.txt  -n 300  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,HFB  --skim SkimTree_FakeEventSkim&
	#SKFlat.py -a $analyzer  -l   $mcpath/Fake.txt  -n 300  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,HFC --skim SkimTree_FakeEventSkim&
	SKFlat.py -a $analyzer  -l   $mcpath/Fake.txt  -n 300  --nmax ${nmax}   -e ${i}  --userflags SeperateFake,LFAll --skim SkimTree_FakeEventSkim& 
	#SKFlat.py -a $analyzer  -l   $mcpath/Fake.txt  -n 300  --nmax ${nmax}   -e ${i}  --userflags SeperateFake --skim SkimTree_FakeEventSkim&
	#SKFlat.py -a $analyzer  -l   $mcpath/Conv.txt  -n 500  --nmax ${nmax}   -e ${i}  --userflags SeperateConv --skim SkimTree_ConvEventSkim&
	#SKFlat.py -a $analyzer  -l   $mcpath/CF.txt    -n 1000  --nmax ${nmax}   -e ${i}  --userflags SeperateCF --skim SkimTree_CFEventSkim 
    done
fi


