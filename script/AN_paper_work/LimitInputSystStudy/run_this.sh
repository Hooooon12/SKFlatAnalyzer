#python3 scan_combine_input_hists.py \
#  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_EE_card_input.root \
#  --out scan_M1000_EE_sr2_bin8_problems.txt \
#  --problem-only \
#  --focus-bin 8 \
#  --focus-problem-only \

#python3 scan_combine_input_hists.py \
#  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_EE_card_input.root \
#  --out scan_M1000_EE_sr2_bin8_full.txt \
#  --full \
#  --focus-bin 8 \

#python3 scan_combine_input_hists.py \
#  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_EE_card_input.root \
#  --out scan_2018_scale_j_bin8_with_total_bkg.txt \
#  --physics-processes \
#  --add-total-bkg \
#  --era 2018 \
#  --syst-contains scale_j \
#  --focus-bin 8 \
#  --full
#
#python3 scan_combine_input_hists.py \
#  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_EE_card_input.root \
#  --out scan_2017_scale_j_bin8_with_total_bkg.txt \
#  --physics-processes \
#  --add-total-bkg \
#  --era 2017 \
#  --syst-contains scale_j \
#  --focus-bin 8 \
#  --full
#
#python3 scan_combine_input_hists.py \
#  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_EE_card_input.root \
#  --out scan_2016postVFP_scale_j_bin8_with_total_bkg.txt \
#  --physics-processes \
#  --add-total-bkg \
#  --era 2016postVFP \
#  --syst-contains scale_j \
#  --focus-bin 8 \
#  --full
#
#python3 scan_combine_input_hists.py \
#  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_EE_card_input.root \
#  --out scan_2016preVFP_scale_j_bin8_with_total_bkg.txt \
#  --physics-processes \
#  --add-total-bkg \
#  --era 2016preVFP \
#  --syst-contains scale_j \
#  --focus-bin 8 \
#  --full


#python3 scan_combine_input_hists.py \
#  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_EE_card_input.root \
#  --out scan_SR2_bin8_2018_scale_j_zero_pathologies.txt \
#  --physics-processes \
#  --era 2018 \
#  --syst-contains scale_j \
#  --focus-bin 8 \
#  --flag ONLY_UP_SURVIVES \
#  --flag ONLY_DOWN_SURVIVES \
#  --flag ZERO_NOM_NONZERO_VAR \
#  --flag HUGE_REL_UP \
#  --flag HUGE_REL_DOWN


#python3 scan_combine_input_hists.py \
#  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_MuMu_card_input.root \
#  --out scan_MuMu_Run2_scale_j_bin8_with_total_bkg.txt \
#  --physics-processes \
#  --add-total-bkg \
#  --era 2016preVFP 2016postVFP 2017 2018 \
#  --syst-contains scale_j \
#  --focus-bin 8 \
#  --full
#
#python3 scan_combine_input_hists.py \
#  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_EMu_card_input.root \
#  --out scan_EMu_Run2_scale_j_bin8_with_total_bkg.txt \
#  --physics-processes \
#  --add-total-bkg \
#  --era 2016preVFP 2016postVFP 2017 2018 \
#  --syst-contains scale_j \
#  --focus-bin 8 \
#  --full


python3 scan_combine_input_hists.py \
  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1000_EMu_card_input.root \
  --out scan_EMu_Run2_scale_j_bin8_conv_prompt.txt \
  --physics-processes \
  --add-total-bkg \
  --era 2016preVFP 2016postVFP 2017 2018 \
  --syst-contains scale_j \
  --focus-bin 8 \
  --full
