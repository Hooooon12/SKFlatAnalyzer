###################################### ANv9, paper v1 #######################################################

#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t AllSR SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --pdf

# This is the new nominal at the unblinding stage. Run full-combo limits. @260916
#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic --Unblind

#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly -s DYVBF DY VBF SSWW --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly -s DYVBF DY VBF SSWW --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -s DYVBF DY VBF SSWW --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -s DYVBF DY VBF SSWW --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic --Unblind

#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Ext --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Ext --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum --Work --Ext
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum --Limit --Ext
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic --Unblind

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -t AllSR SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --tag rRange1000 --rRange 1000
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --tag rRange1000 --rRange 1000 --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --tag rRange10000 --rRange 10000
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --tag rRange10000 --rRange 10000 --pdf

# High-mass scaler for impacts <-- actually we didn't have to run this; extending rRange was enough.
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -t AllSR SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --pdf



## Remake the datacard with updated nuisance grouping to include WZ modeling and to separate Ren- and Fac-Scale @261006
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m 100 1000 10000 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Breakdown
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Breakdown --pdf

#python PrintUncertBreakdown.py     -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195
#python PrintUncertTableForPaper.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195
