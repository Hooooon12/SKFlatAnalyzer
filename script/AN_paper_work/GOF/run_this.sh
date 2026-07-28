#python plot_gof_pvalues.py -i /data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/GOF/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188/Unblind/ --cmsExtra "Work in progress" 
#python plot_gof_pvalues.py -i /data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/GOF/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188/Unblind/ --logy --cmsExtra "Work in progress"

#INPUT=/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/GOF/ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194/Unblind
#OUT=ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194

INPUTS=(
#/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/GOF/ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194/Unblind/mask-cr3_InvBJet
#/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/GOF/ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194/Unblind/mask-cr3_InvMET
#/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/GOF/ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194/Unblind/mask-cr3_InvBJet-cr3_InvMET
/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/GOF/ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195/Unblind
)

for INPUT in "${INPUTS[@]}"; do

    OUT=${INPUT#*GOF/}

    for REGION in "" sr1 sr2 sr3; do
        for LOGY in "" "--logy"; do
    
            args=(
                python3 plot_gof_pvalues.py
                -i "$INPUT"
                --cmsExtra "Work in progress"
                -o "$OUT"
            )
    
            [[ -n "$REGION" ]] && args+=(--region "$REGION")
            [[ -n "$LOGY" ]] && args+=("$LOGY")
    
            echo "${args[@]}"
            "${args[@]}"
    
        done
    done
done
