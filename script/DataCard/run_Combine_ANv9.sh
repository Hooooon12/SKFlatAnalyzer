###################################### ANv9, paper v1 #######################################################

## Make datacards
#python MakeDataCard_ANv9.py -wp ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv9.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv9.py -wp ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv9.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

## Workspace
#python MakeRunList.py ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2Sum --Work
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work

## Stat tests
python MakeRunList.py ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2Sum --Work
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --FitDiag --Unblind
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --FitDiag --Unblind --pdf
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --GOF --Unblind --pdf
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact --Unblind
./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact --Unblind --pdf

## Limits
#python MakeRunList.py ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2Sum --Limit
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic --Unblind

#python MakeDataCard_ANv9.py -wp ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv9.txt -s DYVBF DY VBF SSWW --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv9.py -wp ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv9.txt -s DYVBF DY VBF SSWW --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2Sum -s DYVBF DY VBF SSWW --Work
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work

#python MakeRunList.py ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2Sum -s DYVBF DY VBF SSWW --Limit
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic --Unblind


## Run Ext
#python MakeDataCard_ANv9.py -wp ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv9.txt --CR         --Ext --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv9.py -wp ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv9.txt --Combine CR --Ext --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2Sum --Work --Ext
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work

#python MakeRunList.py ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2Sum --Limit --Ext
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic --Unblind


## Uncertainty breakdown
#python MakeRunList.py ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2Sum -m 100 1000 10000 --Work
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Breakdown
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Breakdown --pdf

#python PrintUncertBreakdown.py     -wp ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr
#python PrintUncertTableForPaper.py -wp ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr


## 3ch limit setting
#python MakeDataCard_ANv9.py -wp ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv9.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2Sum -c 3ch --Work
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work

#python MakeRunList.py ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2Sum -c 3ch --Limit
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic
#./create-batch_ANv9.py -l RunList_Run2Sum_ANv9_CF_HighPt_PR198_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic --Unblind

