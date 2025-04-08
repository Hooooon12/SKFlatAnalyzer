analyzer=HNL_Lepton_FakeRate
rundir=HNL_Lepton_FakeRate
mcpath=${SKFlat_WD}/runJobs/HNL/${rundir}/mc_lists/
datapath=${SKFlat_WD}/runJobs/HNL/${rundir}/data_lists/
njobs=400
njobs_data=200
nmax=500
declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

declare  -a joblist=("CheckProfile"  "RatesMuon" "RatesMuon2" "RatesEl" "RatesEl2" "RatesWR")


for i in "${era_list[@]}"

do
    
    if [[ $1 == "CheckProfile" ]]; then
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_mu.txt  -n ${njobs_data}  --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags CheckProfile&
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_el.txt  -n  ${njobs_data}  --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags CheckProfileEE&
    fi

    if [[ $1 == "CheckProfileEE" ]]; then
	
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_el.txt  -n  ${njobs_data}  --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags CheckProfileEE&
    fi
    
    if [[ $1 == "PR" ]]; then
	SKFlat.py -a $analyzer  -i DYJetsToMuMu_MiNNLO  -n  ${njobs_data}  --nmax ${nmax}  -e ${i}  --skim SkimTree_DileptonBDT --userflags RunPromptRates &
	SKFlat.py -a $analyzer  -i DYJetsToEE_MiNNLO  -n  ${njobs_data}  --nmax ${nmax}  -e ${i}  --skim SkimTree_DileptonBDT --userflags RunPromptRatesEE &
    fi


    if [[ $1 == "RatesHEEP" ]]; then
        SKFlat.py -a $analyzer  -l $mcpath/MC.txt              -n ${njobs}       --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags RunRatesHEEP&
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_el.txt  -n ${njobs_data}  --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags RunRatesHEEP&
    fi

    if [[ $1 == "Rates" ]]; then
        SKFlat.py -a $analyzer  -l $mcpath/MC.txt              -n ${njobs}       --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags RunRates&
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_mu.txt  -n ${njobs_data}  --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags RunRates&
	SKFlat.py -a $analyzer  -l $mcpath/MC.txt              -n ${njobs}       --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags RunRatesEE&
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_el.txt  -n ${njobs_data}  --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags RunRatesEE&
    fi
    
    if [[ $1 == "RatesFull" ]]; then
        SKFlat.py -a $analyzer  -l $mcpath/MC.txt              -n ${njobs}       --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags RunRatesFullID&
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_mu.txt  -n ${njobs_data}  --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags RunRatesFullID&
        
        SKFlat.py -a $analyzer  -l $mcpath/MC.txt              -n ${njobs}       --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags RunRatesFullEEID&
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_el.txt  -n ${njobs_data}  --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags RunRatesFullEEID&
    fi

    if [[ $1 == "MakeRegionPlots" ]]; then
	
	SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_mu.txt  -n ${njobs_data}  --nmax ${nmax}  -e ${i}    --userflags MakeRegionPlots&
	SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_el.txt  -n ${njobs_data}  --nmax ${nmax}  -e ${i}    --userflags MakeRegionPlots&	
        SKFlat.py -a $analyzer  -l $mcpath/MC.txt              -n ${njobs}       --nmax ${nmax}  -e ${i}    --userflags MakeRegionPlots&         
	SKFlat.py -a $analyzer  -l $mcpath/QCD_${i}_Mu.txt              -n ${njobs}       --nmax ${nmax}  -e ${i}    --userflags MakeRegionPlots --skim SkimTree_FakeEventSkimBDT&
	SKFlat.py -a $analyzer  -l $mcpath/QCD_${i}.txt              -n ${njobs}       --nmax ${nmax}  -e ${i}    --userflags MakeRegionPlots --skim SkimTree_FakeEventSkimBDT&
		
    fi

    if [[ $1 == "GetNvtxSF" ]]; then
        SKFlat.py -a $analyzer  -l $mc/DY.txt              -n ${njobs}       --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags MakeRegionPlots&
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_mu.txt  -n ${njobs_data}  --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags MakeRegionPlots&
        SKFlat.py -a $analyzer  -l $datapath/DATA_${i}_el.txt  -n ${njobs_data}  --nmax ${nmax}  -e ${i}   --skim SkimTree_HNFakeBDT --userflags MakeRegionPlots&
    fi



if [[ $1 == "TETS" ]]; then

        SKFlat.py -a $analyzer  -i WZ_pythia             -n ${njobs}       --nmax ${nmax}  -e ${i}    --userflags MakeRegionPlots&


    fi


done
echo "List of Job Options are:"
for i in "${joblist[@]}"

do
    echo $i
done

