#!/bin/bash

analyzer="HNL_Lepton_FakeRate"
rundir="HNL_Lepton_FakeRate"
mcpath="${SKFlat_WD}/runJobs/SampleLists/Bkg/Fake/"
datapath="${SKFlat_WD}/runJobs/SampleLists/Data/"
njobs=400
njobs_data=200
nmax=500
dryrun=false

era_list=("2016postVFP" "2016preVFP" "2017" "2018")
era_list=("2018")
joblist=("CheckProfile" "CheckProfileEE" "PR" "RatesHEEP" "Rates" "RatesFull" "MakeRegionPlots" "GetNvtxSF" "HEMJetFakes" "TETS")

# Dry run detection
if [[ "$2" == "--dryrun" ]]; then
    dryrun=true
fi

run_cmd() {
    if $dryrun; then
        echo "[DryRun] $*"
    else
        eval "$@"
    fi
}

for era in "${era_list[@]}"; do

    case "$1" in
        "CheckProfile")
            run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_SingleLepton_Mu.txt -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags CheckProfile &"
            run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_SingleLepton_E.txt -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags CheckProfileEE &"
            ;;
        "CheckProfileEE")
            run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_SingleLepton_E.txt -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags CheckProfileEE &"
            ;;
        "PR")
            run_cmd "SKFlat.py -a $analyzer -i DYJetsToMuMu_MiNNLO -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_DileptonBDT --userflags RunPromptRates &"
            run_cmd "SKFlat.py -a $analyzer -i DYJetsToEE_MiNNLO -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_DileptonBDT --userflags RunPromptRatesEE &"
            ;;
        "RatesHEEP")
            run_cmd "SKFlat.py -a $analyzer -l $mcpath/Fake.txt -n $njobs --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags RunRatesHEEP &"
            run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_SingleLepton_E.txt -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags RunRatesHEEP &"
            ;;
        "Rates")
            run_cmd "SKFlat.py -a $analyzer -l $mcpath/Fake.txt -n $njobs --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags RunRates &"
            run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_SingleLepton_Mu.txt -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags RunRates &"
            run_cmd "SKFlat.py -a $analyzer -l $mcpath/Fake.txt -n $njobs --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags RunRatesEE &"
            run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_SingleLepton_E.txt -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags RunRatesEE &"
            ;;
        "RatesFull")
            #run_cmd "SKFlat.py -a $analyzer -l $mcpath/Fake.txt -n $njobs --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags RunRatesFullID &"
            #run_cmd "SKFlat.py -a $analyzer -l ${datapath}/DL/${era}_DiLepton_MuMu.txt -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags RunRatesFullID &"
            #run_cmd "SKFlat.py -a $analyzer -l $mcpath/Fake.txt -n $njobs --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags RunRatesFullEEID &"
            run_cmd "SKFlat.py -a $analyzer -l ${datapath}/DL/${era}_DiLepton_EE.txt -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags RunRatesFullEEID &"
            ;;
        "MakeRegionPlots")
            run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_SingleLepton_Mu.txt -n $njobs_data --nmax $nmax -e ${era} --userflags MakeRegionPlots &"
            run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_SingleLepton_E.txt -n $njobs_data --nmax $nmax -e ${era} --userflags MakeRegionPlots &"
            run_cmd "SKFlat.py -a $analyzer -l $mcpath/Fake.txt -n $njobs --nmax $nmax -e ${era} --userflags MakeRegionPlots &"
            run_cmd "SKFlat.py -a $analyzer -l $mcpath/QCD_${era}_Mu.txt -n $njobs --nmax $nmax -e ${era} --userflags MakeRegionPlots --skim SkimTree_FakeEventSkimBDT &"
            run_cmd "SKFlat.py -a $analyzer -l $mcpath/QCD_${era}.txt -n $njobs --nmax $nmax -e ${era} --userflags MakeRegionPlots --skim SkimTree_FakeEventSkimBDT &"
            ;;
        "GetNvtxSF")
            run_cmd "SKFlat.py -a $analyzer -l $mcpath/DY.txt -n $njobs --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags MakeRegionPlots &"
            run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_SingleLepton_Mu.txt -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags MakeRegionPlots &"
            run_cmd "SKFlat.py -a $analyzer -l ${datapath}/${era}_SingleLepton_E.txt -n $njobs_data --nmax $nmax -e ${era} --skim SkimTree_HNFakeBDT --userflags MakeRegionPlots &"
            ;;
        "TETS")
            run_cmd "SKFlat.py -a $analyzer -i WZ_pythia -n $njobs --nmax $nmax -e ${era} --userflags MakeRegionPlots &"
            ;;
        "HEMJetFakes")
            if [[ "$era" == "2018" ]]; then
                run_cmd "SKFlat.py -a $analyzer -l $mcpath/Fake.txt -n $njobs --nmax $nmax -e 2018 --skim SkimTree_HNFakeBDT --userflags HEMJetFakes &"
                run_cmd "SKFlat.py -a $analyzer -l $mcpath/Fake.txt -n $njobs --nmax $nmax -e 2018 --skim SkimTree_HNFakeBDT --userflags HEMJetFakesEE &"
                run_cmd "SKFlat.py -a $analyzer -l $datapath/DATA_2018_mu.txt -n $njobs_data --nmax $nmax -e 2018 --skim SkimTree_HNFakeBDT --userflags HEMJetFakes &"
                run_cmd "SKFlat.py -a $analyzer -l $datapath/DATA_2018_el.txt -n $njobs_data --nmax $nmax -e 2018 --skim SkimTree_HNFakeBDT --userflags HEMJetFakesEE &"
            fi
            ;;
        "" | "--help")
            echo "List of Job Options:"
            for job in "${joblist[@]}"; do
                echo " - $job"
            done
            exit 0
            ;;
        *)
            echo "Unknown job option: '$1'"
            exit 1
            ;;
    esac

done
