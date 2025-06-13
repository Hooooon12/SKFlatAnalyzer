#!/bin/bash

# Exit on error, undefined variable, or failed pipeline
set -euo pipefail

# Variables
analyzer="SkimTree_HNMultiLep"
qrundir="runSkims"
mcpath="${SKFlat_WD}/SkimScripts/SampleLists/Bkg/"
datapath="${SKFlat_WD}/SkimScripts/SampleLists/Data/"
sigpath="${SKFlat_WD}/SkimScripts/SampleLists/Signals/"
njobs=50
njobs_data=100
nmax=600
era_list=("2016postVFP" "2016preVFP" "2017" "2018")

# Helper function to run SKFlat for a given era
run_new_samples() {
    echo "Running new samples for analyzer: ${analyzer}"
    for era in "${era_list[@]}"; do
        echo "Launching jobs for era: ${era}"
        SKFlat.py -a "${analyzer}" -i "DYJets10to50" -n "${njobs}" --nmax "${nmax}" -e "${era}" &
        SKFlat.py -a "${analyzer}" -i "ZGToLLG_PtG_130" -n "${njobs}" --nmax "${nmax}" -e "${era}" &
    done
    wait  # Ensure all background jobs finish before the script exits
    echo "All SKFlat jobs launched for new samples."
}

# Help message
print_help() {
    echo "Usage: $0 [option]"
    echo ""
    echo "Available options:"
    echo "  New     Run new samples (GluGluToZZto4tau for all eras)"
    echo "  help    Show this message"
    echo ""
}

# Main logic
case "${1:-}" in
    New)
        run_new_samples
        ;;
    help|-h|--help)
        print_help
        ;;
    "")
        echo "No option provided."
        print_help
        ;;
    *)
        echo "Unknown option: $1"
        print_help
        exit 1
        ;;
esac



