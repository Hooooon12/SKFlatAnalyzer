analyzer=SkimTree_EGammaTnP_HNLHighPtV2
rundir=runSkims
mcpath=${SKFlat_WD}/SkimScripts/${analyzer}/Bkg/
datapath=${SKFlat_WD}/SkimScripts/SkimTree_EGammaTnP_HNLHighPtV2/data_lists_multilep/
njobs=2000
njobs_data=2000
nmax=500
skim=' '

declare  -a era_list=( "2018")


if [[ $1 == "" ]]; then
    
    for i in "${era_list[@]}"
    do
	
	SKFlat.py -a $analyzer   -i DYJetsToEE_MiNNLO -n 6000  --nmax 500           -e ${i}   --skim SkimTree_DileptonBDT &           
        #SKFlat.py -a $analyzer  -l $mcpath/MC3.txt   -n 6000  --nmax 500           -e ${i}   --skim SkimTree_DileptonBDT &
	#SKFlat.py -a $analyzer  -l $mcpath/MC2.txt   -n 6000  --nmax 500           -e ${i}   --skim SkimTree_DileptonBDT &
	#SKFlat.py -a $analyzer  -l $mcpath/MC.txt   -n 6000  --nmax 500           -e ${i}   --skim SkimTree_HNMultiLepBDT&
        #SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton.txt  -n 6000  --nmax 500   -e ${i}  --skim SkimTree_DileptonBDT 

    done
fi

declare  -a era_list2=( "2016preVFP" "2016postVFP" "2017" "2018")


if [[ $1 == "Main" ]]; then

    for i in "${era_list2[@]}"
    do

        SKFlat.py -a $analyzer  -l $mcpath/MC3.txt   -n 6000  --nmax 500           -e ${i}   --skim SkimTree_DileptonBDT &

        SKFlat.py -a $analyzer  -l $mcpath/MC2.txt   -n 6000  --nmax 500           -e ${i}   --skim SkimTree_DileptonBDT &                                                 
        
	SKFlat.py -a $analyzer  -l $mcpath/MC.txt   -n 6000  --nmax 500           -e ${i}   --skim SkimTree_HNMultiLepBDT& 
                                                
        SKFlat.py -a $analyzer  -l $datapath/${i}_SingleLepton.txt  -n 6000  --nmax 500   -e ${i}  --skim SkimTree_DileptonBDT &
                                              

    done
fi
