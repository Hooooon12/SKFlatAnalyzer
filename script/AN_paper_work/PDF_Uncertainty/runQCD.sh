#!/bin/bash

BASE_DIR="/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv5_BDTV3_AltSR1_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"

SCRIPT="plot_qcdscale_bands_root.py"

# List of SRs to run
SR_LIST=("sr1" "sr2" "sr3")

for SR in "${SR_LIST[@]}"; do
    OUTDIR="qcd_bands_plots"
    echo "[INFO] Running for SR=${SR}, output -> ${OUTDIR}"
    python3 "${SCRIPT}" \
        --base-dir "${BASE_DIR}" \
        --sr "${SR}" \
        --outdir "${OUTDIR}"
done


