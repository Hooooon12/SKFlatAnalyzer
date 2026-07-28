#!/bin/bash

WPS=(
#"ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188"
#"ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194"
#"ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194"
#"ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194"
#"ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195"
"ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195"
)

LOGY_OPTS=(
""
"--logy"
)

DATA_OPTS=(
""
"--no-data"
)

MASSES=("85" "90" "95" "100" "125" "150" "200" "250" "300" "350" "400" "450" "500" "600" "700" "800" "900" "1000" "1100" "1200" "1300" "1500" "1700" "2000" "2500" "3000" "5000" "7500" "10000" "15000" "20000" "25000" "30000" "40000" "50000" "60000")
#MASSES=("85" "90" "95" "100" "125" "150" "200" "250" "300" "350" "400" "450" "500")
#MASSES=("150")

for WP in "${WPS[@]}"; do
  for LOGY in "${LOGY_OPTS[@]}"; do
  for DATA in "${DATA_OPTS[@]}"; do
  for MASS in "${MASSES[@]}"; do
    echo "Running: WP=${WP}, DATA=${DATA:-OFF}, SIGNAL=HNL, MASS=${MASS}, LOGY=${LOGY:-OFF}"

    python3 make_post_fit_plots.py \
      -wp "${WP}" \
      -e Run2Sum \
      -c EE EMu MuMu \
      -m ${MASS} \
      -s HNL \
			--signal-mode none \
      -t AllSR SR3 \
      ${LOGY} \
      ${DATA}

  done

:<<'END'
  echo "Running: WP=${WP}, DATA=${DATA:-OFF}, SIGNAL=Weinberg, LOGY=${LOGY:-OFF}"

  python3 make_post_fit_plots.py \
    -wp "${WP}" \
    -e Run2Sum \
    -c EE EMu MuMu \
    -s Weinberg \
		--signal-mode separate \
    -t AllSR \
    ${LOGY} \
    ${DATA}
END

done
done
done

# Common setting for signal drawing
#--signal-mode separate \

# Setting for M1000
#--signal-scale-dy 20000 \
#-signal-scale-vbf 2000 \
#--signal-scale-ssww 500 \

# Setting for M100
#--signal-scale-dy 100 \

# Setting for M150
#--signal-scale-dy 50 \
