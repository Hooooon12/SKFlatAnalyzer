analyzer=SkimTree_FakeEventSkim
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/SampleLists/Bkg/
njobs=600
njobs_data=600
nmax=500
declare  -a era_list=("2018" "2017")

#  "2016preVFP" "2016postVFP")
if [[ $1 == "" ]]; then
    for i in "${era_list[@]}"
    do
	#SKFlat.py -a $analyzer  -i  DYJetsToTauTau_MiNNLO     -n 400  --nmax ${nmax}   -e ${i}  &
	SKFlat.py -a $analyzer  -i  DYJetsToEE_MiNNLO         -n 400  --nmax ${nmax}   -e ${i}  &
	SKFlat.py -a $analyzer  -i  DYJetsToMuMu_MiNNLO       -n 400  --nmax ${nmax}   -e ${i}  &
	#SKFlat.py -a $analyzer  -l  $mcpath/QCD_${i}El.txt     -n 400  --nmax ${nmax}   -e ${i}  &
	#SKFlat.py -a $analyzer  -l  $mcpath/QCD_${i}Mu.txt     -n 400  --nmax ${nmax}   -e ${i}  &
	#SKFlat.py -a $analyzer  -l  $mcpath/QCD_BEnriched.txt  -n 400  --nmax ${nmax}   -e ${i}  &
	#SKFlat.py -a $analyzer  -l  $mcpath/FakeEventMC.txt    -n 400  --nmax ${nmax}   -e ${i}  &
	#SKFlat.py -a $analyzer  -l  $mcpath/FakeEventHaneolList.txt   -n 400  --nmax ${nmax}   -e ${i}  &
    done
fi

