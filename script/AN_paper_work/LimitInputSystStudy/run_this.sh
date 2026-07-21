## Nuisance scan ##
python3 scan_combine_input_hists.py \
  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M1500_EE_card_input.root \
  --out Scan_EE_M1500_sr2_scale_j.txt \
  --physics-processes \
  --add-total-bkg \
  --check-stored-total-bkg \
  --era 2016preVFP 2016postVFP 2017 2018 \
  --syst-contains scale_j \
  --full

python3 scan_combine_input_hists.py \
  /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr2/M15000_EE_card_input.root \
  --out Scan_EE_M15000_sr2_scale_j.txt \
  --physics-processes \
  --add-total-bkg \
  --check-stored-total-bkg \
  --era 2016preVFP 2016postVFP 2017 2018 \
  --syst-contains scale_j \
  --full

## Scan and Sort the most risky sources ##
#python3 scan_all_combine_risks.py \
#  "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/Run2/sr3/M100_EMu_card_input.root" \
#  --scanner ./scan_combine_input_hists.py \
#  --out-dir scan_M100_EMu_Run2_sr3_allMC \
#  --top-n 100 \
#  --make-top-full 30 \
#  --scanner-extra --data-pull-mode poisson --same-side-min-rel 0.02 --asym-min-rel 0.02 \
##--impact-threshold 0.02 \
##--min-severity info \
##--flag-verbosity full # These are for more quick test
