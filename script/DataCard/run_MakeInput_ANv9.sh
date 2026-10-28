##### ANv9 #####

# Check files
#python MakeInput_ANv9.py -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --CheckFiles --Unblind
#python MakeInput_ANv9.py -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --CheckFiles --CR

## Merge files
#python MakeInput_ANv9.py -i ANv9_CF_HighPt_PR198 -o NoLowDYMG --PreFlag RunSyst --Merge --Unblind 2>&1   | tee log/MakeInput_ANv9_Merge_SR_ANv9_CF_HighPt_PR198.log
#python MakeInput_ANv9.py -i ANv9_CF_HighPt_PR198 -o NoLowDYMG --PreFlag RunSyst --Merge --CR 2>&1   | tee log/MakeInput_ANv9_Merge_CR_ANv9_CF_HighPt_PR198.log

#python MakeInput_ANv9.py -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --Merge --AltWZ --Unblind 2>&1   | tee log/MakeInput_ANv9_Merge_AltWZ_SR_ANv9_CF_HighPt_PR198_NoLowDYMG.log
#python MakeInput_ANv9.py -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --Merge --AltWZ --CR 2>&1   | tee log/MakeInput_ANv9_Merge_AltWZ_CR_ANv9_CF_HighPt_PR198_NoLowDYMG.log

## Run main
#python MakeInput_ANv9.py -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin -s Add --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_ANv9_Syst_Decorr_JetDecorr_SR_ANv9_CF_HighPt_PR198_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr.log
#python MakeInput_ANv9.py -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin -s Add --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_ANv9_Syst_Decorr_JetDecorr_CR_ANv9_CF_HighPt_PR198_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr.log

#python MakeInput_ANv9.py -e Run2 -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_ANv9_Syst_Decorr_JetDecorr_SR_ANv9_CF_HighPt_PR198_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Run2.log
python MakeInput_ANv9.py -e Run2 -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_ANv9_Syst_Decorr_JetDecorr_CR_ANv9_CF_HighPt_PR198_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Run2.log

## Run Ext
#python MakeInput_ANv9.py -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind --Ext 2>&1 | tee log/MakeInput_ANv9_Syst_Decorr_JetDecorr_SR_ANv9_CF_HighPt_PR198_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Ext.log
#python MakeInput_ANv9.py -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --Ext 2>&1 | tee log/MakeInput_ANv9_Syst_Decorr_JetDecorr_CR_ANv9_CF_HighPt_PR198_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Ext.log
#
#python MakeInput_ANv9.py -e Run2 -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind --Ext 2>&1 | tee log/MakeInput_ANv9_Syst_Decorr_JetDecorr_SR_ANv9_CF_HighPt_PR198_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Run2_Ext.log
#python MakeInput_ANv9.py -e Run2 -i ANv9_CF_HighPt_PR198 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --Ext 2>&1 | tee log/MakeInput_ANv9_Syst_Decorr_JetDecorr_CR_ANv9_CF_HighPt_PR198_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Run2_Ext.log
