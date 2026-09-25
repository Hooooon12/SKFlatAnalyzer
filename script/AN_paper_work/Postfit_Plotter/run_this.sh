#!/bin/bash


# ============================================================
# Working points
# ============================================================

WPS=(
#"ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188"
#"ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194"
#"ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194"
#"ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194"
#"ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195"
#"ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195"
#"ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195"
#"ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195"
"ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195"
)


# ============================================================
# Plot variants
# ============================================================

LOGY_OPTS=(
""
"--logy"
)

DATA_OPTS=(
""
#"--no-data"
)


# ============================================================
# Axis style
#
# Default: cuts only.
#
# Available values:
#   cuts
#   codes
#   both
#
# To iterate over several styles, add them to AXIS_STYLE_OPTS,
# for example:
#
# AXIS_STYLE_OPTS=(
#     "cuts"
#     "codes"
#     "both"
# )
# ============================================================

AXIS_STYLE_OPTS=(
    "cuts"
)


# ============================================================
# Mass points
#
# These are passed to Python all at once.
# Weinberg automatically ignores this mass list.
# ============================================================

MASSES=(
"85" "90" "95" "100" "125" "150" "200" "250" "300"
"350" "400" "450" "500" "600" "700" "800" "900"
"1000" "1100" "1200" "1300" "1500" "1700" "2000"
"2500" "3000" "5000" "7500" "10000" "15000"
"20000" "25000" "30000" "40000" "50000" "60000"
)

# For a quick test, comment the full list above and use e.g.
# MASSES=("350" "3000")


# ============================================================
# Signal drawing configuration
# ============================================================
#
# Choose ONE:
#
#   none
#   total
#   both
#   separate
#
# The Python plotter will automatically create:
#
#   signal_none/
#   signal_total/
#   signal_both/
#   signal_separate/
#   signal_separate_<preset>/
#
# ============================================================

#SIGNAL_MODE="none"
SIGNAL_MODE="separate"


# ------------------------------------------------------------
# Used only for:
#
#   SIGNAL_MODE="separate"
#
# Leave empty for manual separate scales.
# Example:
#
#   SIGNAL_PRESET="presetname1"
#
# ------------------------------------------------------------

SIGNAL_PRESET="Unblind_Step2"


# ------------------------------------------------------------
# Manual separate scales
#
# Used only when:
#
#   SIGNAL_MODE="separate"
#   SIGNAL_PRESET=""
#
# ------------------------------------------------------------

# Each value may be:
#
#   auto
#   or any positive number
#
# Examples:
#   auto
#   0.1
#   1
#   10
#   2000

SIGNAL_SCALE_DY="auto"
SIGNAL_SCALE_VBF="auto"
SIGNAL_SCALE_SSWW="auto"
SIGNAL_SCALE_WEINBERG="auto"


# ------------------------------------------------------------
# Common HNL scale for total/both
#
# In prefit and postfit B-only:
#
#   DY/Wgamma -> xS
#   SSWW      -> xS^2
#
# In postfit S+B:
#
#   fitted signals -> common display xS
#
# ------------------------------------------------------------

# Used for total/both HNL.
# May be a positive number or "auto".
SIGNAL_SCALE_TOTAL="auto"


# ============================================================
# Build signal arguments
# ============================================================

SIGNAL_ARGS=(
    --signal-mode "${SIGNAL_MODE}"
)

if [[ "${SIGNAL_MODE}" == "separate" ]]; then

    if [[ -n "${SIGNAL_PRESET}" ]]; then

        # Preset itself may freely mix:
        #
        #   "auto"
        #   numeric overrides
        #
        SIGNAL_ARGS+=(
            --signal-preset "${SIGNAL_PRESET}"
        )

    else

        # Direct scales may independently be:
        #
        #   auto
        #   positive number
        #
        SIGNAL_ARGS+=(
            --signal-scale-dy "${SIGNAL_SCALE_DY}"
            --signal-scale-vbf "${SIGNAL_SCALE_VBF}"
            --signal-scale-ssww "${SIGNAL_SCALE_SSWW}"
            --signal-scale-weinberg "${SIGNAL_SCALE_WEINBERG}"
        )

    fi

elif [[ "${SIGNAL_MODE}" == "total" || "${SIGNAL_MODE}" == "both" ]]; then

    SIGNAL_ARGS+=(
        --signal-scale-total "${SIGNAL_SCALE_TOTAL}"
        --signal-scale-weinberg "${SIGNAL_SCALE_WEINBERG}"
    )

fi


# ============================================================
# Run
# ============================================================

for WP in "${WPS[@]}"; do

    for LOGY in "${LOGY_OPTS[@]}"; do

        for DATA in "${DATA_OPTS[@]}"; do

            for AXIS_STYLE in "${AXIS_STYLE_OPTS[@]}"; do

                echo ""
                echo "================================================================"
                echo "Running:"
                echo "  WP          = ${WP}"
                echo "  SIGNALS     = HNL Weinberg"
                echo "  SIGNAL_MODE = ${SIGNAL_MODE}"
                echo "  PRESET      = ${SIGNAL_PRESET:-none}"
                echo "  LOGY        = ${LOGY:-OFF}"
                echo "  DATA OPTION = ${DATA:-OFF}"
                echo "  AXIS STYLE  = ${AXIS_STYLE}"
                echo "================================================================"

                python3 make_post_fit_plots.py \
                    -wp "${WP}" \
                    -e Run2Sum \
                    -c EE EMu MuMu \
                    -m "${MASSES[@]}" \
                    -s HNL Weinberg \
                    -t AllSR \
                    --axis-style "${AXIS_STYLE}" \
                    "${SIGNAL_ARGS[@]}" \
                    ${LOGY} \
                    ${DATA} \
                    --sr2-merge78

            done

        done

    done

done
