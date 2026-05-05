#python compare_run2sum_fom_new.py \ # compare Run2Sum vs Run2 in the same WP
#  --base-dir /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr \
#  --regions sr1 sr2 sr3 \
#  --channels EMu \
#  --masses 800 \
#  --signal-modes DYVBF SSWW \
#  --bkg-modes bkgsum \
#  --outdir run2sum_fom_EMu_sr123

#python compare_run2sum_fom_new.py \
#  --wp WP1 run2    /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr \
#  --wp WP2 era-sum /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr \
#  --regions sr1 sr2 sr3 \
#  --channels EE \
#  --masses 100 500 1000 10000 \
#  --signal-modes DYVBF SSWW \
#  --bkg-modes bkgsum \
#  --outdir Prev_vs_NewBinning_fom_EE_sr123

#python compare_run2sum_fom_new.py \
#  --wp WP1 run2    /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr \
#  --wp WP2 era-sum /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr \
#  --regions sr1 sr2 sr3 \
#  --channels MuMu \
#  --masses 100 500 1000 10000 \
#  --signal-modes DYVBF SSWW \
#  --bkg-modes bkgsum \
#  --outdir Prev_vs_NewBinning_fom_MuMu_sr123

python compare_run2sum_fom_new.py \
  --wp WP1 run2    /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_StatReqEra_Decorr_JetDecorr \
  --wp WP2 run2    /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR3Update_Decorr_JetDecorr/ \
  --regions sr3 \
  --channels MuMu \
  --masses 100 500 1000 10000 \
  --signal-modes DYVBF SSWW \
  --bkg-modes bkgsum \
  --outdir Before_After_SR3Update_fom_MuMu_sr123

