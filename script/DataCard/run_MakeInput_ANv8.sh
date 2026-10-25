#python MakeInput_public.py --Merge -e 2018 -i HEMJet --PreFlag RemoveHEMJet -m 100 500 1000 10000
#python MakeInput_public.py --Merge -e 2018 -i HEMJet --PreFlag ScaleHEMJet -m 100 500 1000 10000
#python MakeInput_public.py --Merge -e 2018 -i TuneP --PreFlag CompareTuneP -m 100 500 1000 10000

#python MakeInput_public.py -e 2018 -i HEMJet --PreFlag RemoveHEMJet -m 100 500 1000 10000
#python MakeInput_public.py -e 2018 -i HEMJet --PreFlag ScaleHEMJet -m 100 500 1000 10000
#python MakeInput_public.py -e 2018 -i TuneP --PreFlag CompareTuneP -t HNL_ULIDv2 HNTightV2 POGTight -m 100 500 1000 10000

#python MakeInput_public.py --Merge -e 2018 -i HEMJet --PreFlag RemoveHEMJet --CheckFiles 
#python MakeInput_public.py --Merge -e 2018 -i HEMJet --PreFlag ScaleHEMJet  --CheckFiles 
#python MakeInput_public.py --Merge -e 2018 -i TuneP  --PreFlag CompareTuneP --CheckFiles 
#python MakeInput_public.py --Merge -e 2018 -i HEMJet --PreFlag RemoveHEMJet 
#python MakeInput_public.py --Merge -e 2018 -i HEMJet --PreFlag ScaleHEMJet  
#python MakeInput_public.py --Merge -e 2018 -i TuneP  --PreFlag CompareTuneP 

#python MakeInput_public.py -e 2018 -i HEMJet --PreFlag RemoveHEMJet 
#python MakeInput_public.py -e 2018 -i HEMJet --PreFlag ScaleHEMJet  
#python MakeInput_public.py -e 2018 -i TuneP  --PreFlag CompareTuneP -t HNL_ULIDv2 HNTightV2 POGTight

#python MakeInput_public.py -i ANv4 --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv4 --PreFlag RunSyst --CheckFiles --CR
#python MakeInput_public.py -i ANv4 --PreFlag RunSyst --Merge
#python MakeInput_public.py -i ANv4 --PreFlag RunSyst --Merge --CR 2>&1 | tee MakeInput_public_Merge_CR.log

#python MakeInput_public.py -i ANv4 --PreFlag RunSyst --Syst --Decorr --JetDecorr
#python MakeInput_public.py -i ANv4 --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee MakeInput_public_Syst_Decorr_JetDecorr_CR.log

#python MakeInput_public.py -i ANv4 --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --Scan

#python MakeInput_public.py -i ANv4 --PreFlag RunSyst --Syst --Decorr --JetDecorr --CnC
#python MakeInput_public.py -i ANv4 --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --CnC

########################## ANv5 #################################
#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --Merge --CR 2>&1 | tee MakeInput_public_Merge_CR.log
#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --Merge 2>&1 | tee MakeInput_public_Merge_SR.log

#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1 | tee MakeInput_public_Syst_Decorr_JetDecorr_SR.log
#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee MakeInput_public_Syst_Decorr_JetDecorr_CR.log

#python MakeInput_public.py -i ANv5 -m 85 90 95 100 --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR
#python MakeInput_public.py -i ANv5 -c EMu --PreFlag RunSyst --Syst --Decorr --JetDecorr


########################## ANv5 (New definition) #################################
#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --Merge 2>&1      | tee log/MakeInput_public_Merge_SR.log
#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --Merge --CR 2>&1 | tee log/MakeInput_public_Merge_CR.log

#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1      | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR.log
#python MakeInput_public.py -i ANv5 --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR.log



#python MakeInput_public.py -i ANv5 -m Weinberg --PreFlag RunSyst --Merge 2>&1 | tee test_Merge_Weinberg_SR.log
#python MakeInput_public.py -i ANv5 -m Weinberg --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1 | tee test_Weinberg_SR.log
#python MakeInput_public.py -i ANv5 -m Weinberg --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee test_Weinberg_CR.log




#python MakeInput_public.py -i ANv5_BDTV3_LooseBinning --PreFlag RunSyst__Loose_BDT --CheckFiles
#python MakeInput_public.py -i ANv5_BDTV3_StrictBinning --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv5_BDTV3 --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv5_BDTV3_LooseBinning --PreFlag RunSyst__Loose_BDT --Merge 2>&1      | tee log/MakeInput_public_Merge_SR_BDTV3Loose.log
#python MakeInput_public.py -i ANv5_BDTV3_StrictBinning --PreFlag RunSyst --Merge 2>&1      | tee log/MakeInput_public_Merge_SR_BDTV3Strict.log
#python MakeInput_public.py -i ANv5_BDTV3 --PreFlag RunSyst --Merge --CR 2>&1      | tee log/MakeInput_public_Merge_CR_BDTV3.log

#python MakeInput_public.py -i ANv5_BDTV3 --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV3.log
#python MakeInput_public.py -i ANv5_BDTV3_LooseBinning --PreFlag RunSyst__Loose_BDT --Syst --Decorr --JetDecorr 2>&1      | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3Loose.log
#python MakeInput_public.py -i ANv5_BDTV3_StrictBinning --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1      | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3Strict.log

#python MakeInput_public.py -i ANv5_BDTV3 -m 85 90 95 100 125 150 200 250 300 400 500 --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV3.log
#python MakeInput_public.py -i ANv5_BDTV3_LooseBinning -m 85 90 95 100 125 150 200 250 300 400 500 --PreFlag RunSyst__Loose_BDT --Syst --Decorr --JetDecorr 2>&1      | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3Loose.log
#python MakeInput_public.py -i ANv5_BDTV3_StrictBinning -m 85 90 95 100 125 150 200 250 300 400 500 --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1      | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3Strict.log



#python MakeInput_public.py -i ANv5_BDTV4_VeryLooseBinning --PreFlag RunSyst__VeryLoose_BDT --CheckFiles
#python MakeInput_public.py -i ANv5_BDTV4_LooseBinning --PreFlag RunSyst__Loose_BDT --CheckFiles
#python MakeInput_public.py -i ANv5_BDTV4_StrictBinning --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv5_BDTV4 --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv5_BDTV4_VeryLooseBinning --PreFlag RunSyst__VeryLoose_BDT --Merge 2>&1      | tee log/MakeInput_public_Merge_SR_BDTV4VeryLoose.log
#python MakeInput_public.py -i ANv5_BDTV4_LooseBinning --PreFlag RunSyst__Loose_BDT --Merge 2>&1      | tee log/MakeInput_public_Merge_SR_BDTV4Loose.log
#python MakeInput_public.py -i ANv5_BDTV4_StrictBinning --PreFlag RunSyst --Merge 2>&1      | tee log/MakeInput_public_Merge_SR_BDTV4Strict.log
#python MakeInput_public.py -i ANv5_BDTV4 --PreFlag RunSyst --Merge --CR 2>&1      | tee log/MakeInput_public_Merge_CR_BDTV4.log

#python MakeInput_public.py -i ANv5_BDTV4 -e 2018 -c EMu -m 100 125 150 --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV4.log
#python MakeInput_public.py -i ANv5_BDTV4 --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV4.log
#python MakeInput_public.py -i ANv5_BDTV4_VeryLooseBinning --PreFlag RunSyst__VeryLoose_BDT --Syst --Decorr --JetDecorr 2>&1      | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV4VeryLoose.log
#python MakeInput_public.py -i ANv5_BDTV4_LooseBinning --PreFlag RunSyst__Loose_BDT --Syst --Decorr --JetDecorr 2>&1      | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV4Loose.log
#python MakeInput_public.py -i ANv5_BDTV4_StrictBinning --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1      | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV4Strict.log



#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_BDTV4_BugFix.log
#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --Merge --CR 2>&1      | tee log/MakeInput_public_Merge_CR_BDTV4_BugFix.log

#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --BDTver V4_LooseBin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV4_BugFix_Loose.log
#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --BDTver V4_LooseBin --Syst --Decorr --JetDecorr --CR 2>&1  | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV4_BugFix_Loose.log
#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --BDTver V4_StrictBin --Syst --Decorr --JetDecorr 2>&1      | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV4_BugFix_Strict.log
#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --BDTver V4_StrictBin --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV4_BugFix_Strict.log

#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --BDTver V3_LooseBin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_BugFix_Loose.log
#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --BDTver V3_LooseBin --Syst --Decorr --JetDecorr --CR 2>&1  | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV3_BugFix_Loose.log
#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --BDTver V3_StrictBin --Syst --Decorr --JetDecorr 2>&1      | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_BugFix_Strict.log
#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --BDTver V3_StrictBin --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV3_BugFix_Strict.log




#python MakeInput_public.py -i ANv5_BDTV2to4 --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv5_BDTV2to4 --PreFlag RunSyst --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_BDTV2to4.log

#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_powheg --PreFlag RunSyst --BDTver V2_StrictBin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V2_StrictBin_WZ_powheg.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_powheg --PreFlag RunSyst --BDTver V3_StrictBin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V3_StrictBin_WZ_powheg.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_powheg --PreFlag RunSyst --BDTver V3_Strict_10_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V3_Strict_10_Bin_WZ_powheg.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_powheg --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V3_Strict_15_Bin_WZ_powheg.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_powheg --PreFlag RunSyst --BDTver V3_Strict_20_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V3_Strict_20_Bin_WZ_powheg.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_powheg --PreFlag RunSyst --BDTver V4_Strict_10_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V4_Strict_10_Bin_WZ_powheg.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_powheg --PreFlag RunSyst --BDTver V4_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V4_Strict_15_Bin_WZ_powheg.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_powheg --PreFlag RunSyst --BDTver V4_Strict_20_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V4_Strict_20_Bin_WZ_powheg.log
#
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_amcatnlo --PreFlag RunSyst --BDTver V2_StrictBin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V2_StrictBin_WZ_amcatnlo.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_amcatnlo --PreFlag RunSyst --BDTver V3_StrictBin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V3_StrictBin_WZ_amcatnlo.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_amcatnlo --PreFlag RunSyst --BDTver V3_Strict_10_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V3_Strict_10_Bin_WZ_amcatnlo.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_amcatnlo --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V3_Strict_15_Bin_WZ_amcatnlo.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_amcatnlo --PreFlag RunSyst --BDTver V3_Strict_20_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V3_Strict_20_Bin_WZ_amcatnlo.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_amcatnlo --PreFlag RunSyst --BDTver V4_Strict_10_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V4_Strict_10_Bin_WZ_amcatnlo.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_amcatnlo --PreFlag RunSyst --BDTver V4_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V4_Strict_15_Bin_WZ_amcatnlo.log
#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_amcatnlo --PreFlag RunSyst --BDTver V4_Strict_20_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V4_Strict_20_Bin_WZ_amcatnlo.log


### Separate DY and VBF, scale Weinberg ###

#python MakeInput_public.py -i ANv5_BDTV2to4 -o WZ_amcatnlo_ScaleWeinberg --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV2to4_V3_Strict_15_Bin_WZ_amcatnlo_ScaleWeinberg.log

### SR1,2 bin re-optimization ###

#python MakeInput_public.py -i ANv5_BDTV3_UpdateSRBinning --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv5_BDTV3_UpdateSRBinning --PreFlag RunSyst --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_BDTV3_UpdateSRBinning.log
#python MakeInput_public.py -i ANv5_BDTV3_UpdateSRBinning --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_UpdateSRBinning.log
#python MakeInput_public.py -i ANv5_BDTV3_UpdateSRBinning -o AltBin --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_UpdateSRBinning_AltBin.log


### SR1 bin re-re-optimization ###

#python MakeInput_public.py -i ANv5_BDTV3_SR1_Binning_Update --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv5_BDTV3_SR1_Binning_Update --PreFlag RunSyst --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_BDTV3_SR1_Binning_Update.log
#python MakeInput_public.py -i ANv5_BDTV3_SR1_Binning_Update --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_SR1_Binning_Update.log

### Fix PDF error ###
#python MakeInput_public.py -i ANv5_BDTV3_SR1_Binning_Update -o FixPDFerror --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_SR1_Binning_Update_FixPDFerror.log


### SR1 bin re3-optimization (removing SSWW in opt process; nominal: utilize sqrt bin by bin, alt: no sqrt FOM. Added MC_others) ###
#python MakeInput_public.py -i ANv5_BDTV3_AltSR1 --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv5_BDTV3_AltSR1 --PreFlag RunSyst --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_BDTV3_AltSR1.log
#python MakeInput_public.py -i ANv5_BDTV3_AltSR1 -x ANv5_BDTV3_AltSR1_HNL_ULIDv2_V3_Strict_15_Bin --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_AltSR1.log
#python MakeInput_public.py -i ANv5_BDTV3_AltSR1 -o AltBin -x ANv5_BDTV3_AltSR1_HNL_ULIDv2_AltBin_V3_Strict_15_Bin --AddEx --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_AltSR1_AltBin.log

# Rerun CR to make it include MC_others
#python MakeInput_public.py -i ANv5_BDTV4_BugFix --PreFlag RunSyst --Merge --CR 2>&1      | tee log/MakeInput_public_Merge_CR_BDTV4_BugFix_MergeMC.log # Run MergeMC only
#python MakeInput_public.py -i ANv5_BDTV4_BugFix -o MC_others --PreFlag RunSyst --BDTver V3 --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV4_BugFix_V3_MC_others.log

# Rerun SR+CR after finalizing handling exception rules
#python MakeInput_public.py -i ANv5_BDTV3_AltSR1 -x ANv5_BDTV3_AltSR1_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -s Write --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/TEST_SR.log
#python MakeInput_public.py -i ANv5_BDTV3_AltSR1 -o AltBin -x ANv5_BDTV3_AltSR1_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -s Add --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/TEST_SR_Alt.log
#python MakeInput_public.py -i ANv5_BDTV4_BugFix -o MC_others -s Add --PreFlag RunSyst --BDTver V3 --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/TEST_CR.log


### SR1 bin re3-optimization with bug fixing from SKFlatOutput side ###

#python MakeInput_public.py -i ANv5_BDTV3_SR1_FixRepeatBin --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv5_BDTV3_SR1_FixRepeatBin --PreFlag RunSyst --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_BDTV3_SR1_FixRepeatBin.log
#python MakeInput_public.py -i ANv5_BDTV3_SR1_FixRepeatBin -x ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -s Write --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_SR1_FixRepeatBin.log
#python MakeInput_public.py -i ANv5_BDTV3_SR1_FixRepeatBin -o AltBin -x ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -s Add --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_SR1_FixRepeatBin_AltBin.log
#python MakeInput_public.py -i ANv5_BDTV4_BugFix -o MC_others -s Add --PreFlag RunSyst --BDTver V3 --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV4_BugFix_V3_MC_others.log

# M500 extend

#python MakeInput_public.py -i ANv5_BDTV3_SR1_FixRepeatBin -m 500 -o AltBin -x ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Ext -s Add --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr --Ext 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_SR1_FixRepeatBin_AltBin_Ext.log
#python MakeInput_public.py -i ANv5_BDTV4_BugFix -m 500 -o MC_others -x ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Ext -s Add --PreFlag RunSyst --BDTver V3 --Syst --Decorr --JetDecorr --CR --Ext 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV4_BugFix_V3_MC_others_Ext.log

# One-bin

#python MakeInput_public.py -i ANv5_BDTV3_SR1_FixRepeatBin -o AltBin -s Add --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr --CnC 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_SR1_FixRepeatBin_AltBin_CnC.log

# Fix correlation
#python MakeInput_public.py -i ANv5_BDTV3_SR1_FixRepeatBin -o AltBin_FixCorr -s Print --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_BDTV3_SR1_FixRepeatBin_AltBin_FixCorr.log
#python MakeInput_public.py -i ANv5_BDTV4_BugFix -o MC_others_FixCorr -s Print --PreFlag RunSyst --BDTver V3 --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_BDTV4_BugFix_V3_MC_others_FixCorr.log




########### ANv6. BDT masses are filled, xsec changed --> bin optimization redone. AltBin is now promoted to the default.
#python MakeInput_public.py -i ANv6_NewSignals --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv6_NewSignals --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv6_NewSignals --PreFlag RunSyst --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_ANv6_NewSignals.log
#python MakeInput_public.py -i ANv6_NewSignals --PreFlag RunSyst --Merge --CR 2>&1      | tee log/MakeInput_public_Merge_CR_ANv6_NewSignals.log

#python MakeInput_public.py -i ANv6_NewSignals --BDTver V3_Strict_15_Bin -s Add -x ANv6_NewSignals_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv6_NewSignals.log
#python MakeInput_public.py -i ANv6_NewSignals --BDTver V3_Strict_15_Bin -s Add -x ANv6_NewSignals_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv6_NewSignals.log

###### ANv6 but test PNET decorrelated
#python MakeInput_public.py -i ANv6_NewSignals --BDTver V3_Strict_15_Bin -o PNETdecorr -m 500 -c MuMu -s Add -x ANv6_NewSignals_HNL_ULIDv2_PNETdecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv6_NewSignals.log
#python MakeInput_public.py -i ANv6_NewSignals --BDTver V3_Strict_15_Bin -o PNETdecorr -m 500 -c MuMu -s Add -x ANv6_NewSignals_HNL_ULIDv2_PNETdecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv6_NewSignals.log

###### Fix mc_inc double counting into the data_obs

#python MakeInput_public.py -i ANv6_NewSignals --BDTver V3_Strict_15_Bin -s Print --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv6_NewSignals.log
#python MakeInput_public.py -i ANv6_NewSignals --BDTver V3_Strict_15_Bin -s Print --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv6_NewSignals.log


############ ANv6, fixed the omitted trigger syst <---------------- This is the legacy !! ##############
#python MakeInput_public.py -i ANv6_FixSyst --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv6_FixSyst --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv6_FixSyst --PreFlag RunSyst --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_ANv6_FixSyst.log
#python MakeInput_public.py -i ANv6_FixSyst --PreFlag RunSyst --Merge --CR 2>&1      | tee log/MakeInput_public_Merge_CR_ANv6_FixSyst.log

#python MakeInput_public.py -i ANv6_FixSyst --BDTver V3_Strict_15_Bin -s Add -x ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv6_FixSyst.log
#python MakeInput_public.py -i ANv6_FixSyst --BDTver V3_Strict_15_Bin -s Add -x ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv6_FixSyst.log

#python MakeInput_public.py -i ANv6_FixSyst -m 500 -x ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Ext -s Add --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr --Ext 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv6_FixSyst_Ext.log
#python MakeInput_public.py -i ANv6_FixSyst -m 500 -x ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Ext -s Add --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr --CR --Ext 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv6_FixSyst_Ext.log



###### Test consistent binning ######
#python MakeInput_public.py -i ANv6_SingularBinning --PreFlag RunSyst__SingularBinning --CheckFiles
#python MakeInput_public.py -i ANv6_SingularBinning --PreFlag RunSyst__SingularBinning --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_ANv6_SingularBinning.log
#python MakeInput_public.py -i ANv6_SingularBinning --BDTver V3_Strict_15_Bin -s Add -x ANv6_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr --PreFlag RunSyst__SingularBinning --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv6_SingularBinning.log




############ ANv7: fixed Muon RECO SF to a pt-dependent way
#python MakeInput_public.py -i ANv7 --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv7 --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv7 --PreFlag RunSyst --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_ANv7.log
#python MakeInput_public.py -i ANv7 --PreFlag RunSyst --Merge --CR 2>&1      | tee log/MakeInput_public_Merge_CR_ANv7.log
#python MakeInput_public.py -i ANv7 --PreFlag RunSyst -e 2016preVFP --Merge --Fake --CR 2>&1      | tee log/MakeInput_public_Merge_CR_ANv7_redo_2016preVFP_Fake.log
#python MakeInput_public.py -i ANv7 --PreFlag RunSyst -e 2017 --Merge --Conv --MC --CR 2>&1      | tee log/MakeInput_public_Merge_CR_ANv7_redo_2017_Conv.log
#python MakeInput_public.py -i ANv7 --PreFlag RunSyst --Merge --Prompt --MC --CR 2>&1      | tee log/MakeInput_public_Merge_CR_ANv7_redo_ZZ.log

#python MakeInput_public.py -i ANv7 --BDTver V3_Strict_15_Bin -s Add -x ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7.log
#python MakeInput_public.py -i ANv7 --BDTver V3_Strict_15_Bin -s Add -x ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7.log

## Rerun low masses with *0.1 scale (for HybridNew)
#python MakeInput_public.py -i ANv7 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr -m 85 90 95 100 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_RerunLowmass.log

##### Singular Binning

#python MakeInput_public.py -i ANv7_SingularBinning --PreFlag RunSyst__SingularBinning --CheckFiles
#python MakeInput_public.py -i ANv7_SingularBinning --PreFlag RunSyst__SingularBinning --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_ANv7_SingularBinning.log
#python MakeInput_public.py -i ANv7_SingularBinning --BDTver V3_Strict_15_Bin -s Add -x ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr --PreFlag RunSyst__SingularBinning --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_SingularBinning.log

##### Full JES NPs
#python MakeInput_public.py -i ANv7_FullJESNS --PreFlag RunSyst__FullJESNS --CheckFiles
#python MakeInput_public.py -i ANv7_FullJESNS --PreFlag RunSyst__FullJESNS --CheckFiles --CR

#python MakeInput_public.py -i ANv7_FullJESNS --PreFlag RunSyst__FullJESNS --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_ANv7_FullJESNS.log
#python MakeInput_public.py -i ANv7_FullJESNS --PreFlag RunSyst__FullJESNS --Merge --CR 2>&1      | tee log/MakeInput_public_Merge_CR_ANv7_FullJESNS.log

#python MakeInput_public.py -i ANv7_FullJESNS --BDTver V3_Strict_15_Bin --PreFlag RunSyst__FullJESNS --Syst --Decorr -c MuMu EE 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_FullJESNS.log
#python MakeInput_public.py -i ANv7_FullJESNS --BDTver V3_Strict_15_Bin --PreFlag RunSyst__FullJESNS --Syst --Decorr --CR -c MuMu EE 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_FullJESNS.log


##### Rerun nominal with new NP, cr naming after the Combine review
#python MakeInput_public.py -i ANv7 --BDTver V3_Strict_15_Bin -o NewName -s Add -x ANv7_HNL_ULIDv2_NewName_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewName.log
#python MakeInput_public.py -i ANv7 --BDTver V3_Strict_15_Bin -o NewName -s Add -x ANv7_HNL_ULIDv2_NewName_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1  | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_NewName.log


##### Add CF to EMu, WZ PDF/scale uncertainties @260221 (WZ PDF/Scale in CR as well @260225)
#python MakeInput_public.py -i ANv7_EMuCF --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv7_EMuCF --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv7_EMuCF --PreFlag RunSyst --Merge 2>&1           | tee log/MakeInput_public_Merge_SR_ANv7_EMuCF.log
#python MakeInput_public.py -i ANv7_EMuCF --PreFlag RunSyst --Merge --CR 2>&1      | tee log/MakeInput_public_Merge_CR_ANv7_EMuCF.log

#python MakeInput_public.py -i ANv7_EMuCF --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1      | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_EMuCF.log
#python MakeInput_public.py -i ANv7_EMuCF --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_EMuCF.log



##### change fake uncertainty to shape, separate QCD scale (2017 only -- deployed full Run2 @260323) + Merged: SingularBinning, merged BDT (Run2) @260317
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst --CheckFiles     
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst --Merge       2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_Preapproval.log
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst --Merge --CR  2>&1   | tee log/MakeInput_public_Merge_CR_ANv7_Preapproval.log
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst --Merge --Signal --CR  2>&1   | tee log/MakeInput_public_Merge_CR_ANv7_Preapproval_Signal.log

#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst -s Add -x ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --Syst --Decorr --JetDecorr      2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_Preapproval.log
#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst -s Add -x ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_Preapproval.log



#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__Merged --CheckFiles
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__Merged --CheckFiles --CR

#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__Merged --Merge      2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_Preapproval_Merged.log
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__Merged --Merge --CR 2>&1   | tee log/MakeInput_public_Merge_CR_ANv7_Preapproval_Merged.log

#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst__Merged -s Add -x ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Merged_Decorr_JetDecorr --Syst --Decorr --JetDecorr      2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_Preapproval_Merged.log
#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst__Merged -s Add -x ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Merged_Decorr_JetDecorr --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_Preapproval_Merged.log

## 2018 rerun due to a bug
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__Merged --Merge --Signal -e 2018    2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_Preapproval_Merged_2018_Signal.log
#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst__Merged --Syst --Decorr --JetDecorr      2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_Preapproval_Merged.log
#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst__Merged --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_Preapproval_Merged.log





##### Test BDT trained with SS signals, SR binning @260322

#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__OnlySS --CheckFiles
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__OnlySS --CheckFiles --CR

#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__OnlySS --Merge      2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_Preapproval_OnlySS.log
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__OnlySS --Merge --CR 2>&1   | tee log/MakeInput_public_Merge_CR_ANv7_Preapproval_OnlySS.log

#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst__OnlySS -s Add -x ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_OnlySS_Decorr_JetDecorr --Syst --Decorr --JetDecorr      2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_Preapproval_OnlySS.log
#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst__OnlySS -s Add -x ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_OnlySS_Decorr_JetDecorr --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_Preapproval_OnlySS.log


## New run @260327 due to binning bug
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__SR_FlavEraDep --CheckFiles
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__SR_FlavEraDep --Merge      2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_Preapproval_SR_FlavEraDep.log
#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst__SR_FlavEraDep --Syst --Decorr --JetDecorr      2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_Preapproval_SR_FlavEraDep.log


#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__SR_FlavDep --CheckFiles
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__SR_FlavDep --Merge      2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_Preapproval_SR_FlavDep.log
#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst__SR_FlavDep --Syst --Decorr --JetDecorr      2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_Preapproval_SR_FlavDep.log



##### Test sensitivitiy with forward-only (central-vetoed) jets in SR2 @260323
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__UseWMassConstraint__RemoveCentralVBFJets --CheckFiles
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__UseWMassConstraint__RemoveCentralVBFJets --Merge      2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_Preapproval_SR_FlavEraDep.log
#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst__UseWMassConstraint__RemoveCentralVBFJets --Syst --Decorr --JetDecorr      2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_Preapproval_UseWMassConstraint_RemoveCentralVBFJets.log



##### Test SR2 refinement (2016pre/post and 2017/18 now share the same binning) @260328 : This turned out to be buggy. Should be reran.
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__SR2_BinRefinement --CheckFiles
#python MakeInput_public.py -i ANv7_Preapproval --PreFlag RunSyst__SR2_BinRefinement --Merge -e 2016preVFP 2016postVFP     2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_Preapproval_SR2_BinRefinement_2016.log
#python MakeInput_public.py -i ANv7_Preapproval --BDTver V3_Strict_15_Bin --PreFlag RunSyst__SR2_BinRefinement --Syst --Decorr --JetDecorr -e 2016preVFP 2016postVFP     2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_Preapproval_SR2_BinRefinement_2016.log



##### L2 review setting (treat low pt prompt in ConversionSplitting, Tight Bjet in IB CRs) @260405
#python MakeInput_public.py -i ANv7_L2review --PreFlag RunSyst --CheckFiles     
#python MakeInput_public.py -i ANv7_L2review --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv7_L2review --PreFlag RunSyst --Merge       2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_L2review.log
#python MakeInput_public.py -i ANv7_L2review --PreFlag RunSyst --Merge --CR  2>&1   | tee log/MakeInput_public_Merge_CR_ANv7_L2review.log

#python MakeInput_public.py -i ANv7_L2review --BDTver V3_Strict_15_Bin --PreFlag RunSyst -s Add -x ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --Syst --Decorr --JetDecorr      2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_L2review.log
#python MakeInput_public.py -i ANv7_L2review --BDTver V3_Strict_15_Bin --PreFlag RunSyst -s Add -x ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_L2review.log

## TEST zero data
#python MakeInput_public.py -i ANv7_L2review -e 2016postVFP -c EE --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_L2review_ZeroData.log

## Fix wrong hessian error treatment and add gg to taus @260417
#python MakeInput_public.py -i ANv7_L2review --BDTver V3_Strict_15_Bin -o FixHessian_AddGluGluTaus -s Add -x ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr      2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_L2review_FixHessian_AddGluGluTaus.log
#python MakeInput_public.py -i ANv7_L2review --BDTver V3_Strict_15_Bin -o FixHessian_AddGluGluTaus -s Add -x ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_L2review_FixHessian_AddGluGluTaus.log

## Run CnC mode to see stat-abundant PDF uncertainty @260429
#python MakeInput_public.py -i ANv7_L2review --BDTver V3_Strict_15_Bin -o FixHessian_AddGluGluTaus --PreFlag RunSyst --Syst --Decorr --JetDecorr --CnC

## Make Run2 LimitInputs!!!! @260421 ############# Run2 mode should be ran after deploying era-dependent LimitInputs ##########
#python MakeInput_public.py -e Run2 -i ANv7_L2review --BDTver V3_Strict_15_Bin -o FixHessian_AddGluGluTaus --PreFlag RunSyst --Syst --Decorr --JetDecorr      2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_L2review_FixHessian_AddGluGluTaus_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_L2review --BDTver V3_Strict_15_Bin -o FixHessian_AddGluGluTaus --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_L2review_FixHessian_AddGluGluTaus_Run2.log



### L2review but Medium Bjet in CR2 IB, update muon fake highpt syst, Run2 shape bin optimization, mu/ele separate fake syst (PR187), Fix highpt syst bug (PR188) @260426
#python MakeInput_public.py -i ANv7_FixFakeSyst --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv7_FixFakeSyst --PreFlag RunSyst --Merge --CR  2>&1   | tee log/MakeInput_public_Merge_CR_ANv7_FixFakeSyst.log

#python MakeInput_public.py -i ANv7_FixFakeSyst --BDTver V3_Strict_15_Bin --PreFlag RunSyst -s Add -x ANv7_FixFakeSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_FixFakeSyst.log

#python MakeInput_public.py -e Run2 -i ANv7_FixFakeSyst --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_FixFakeSyst_Run2.log


### L2review but Medium Bjet in CR2 IB, update muon fake highpt syst, Run2 shape bin optimization, mu/ele separate fake syst (PR187), Fix highpt syst bug (PR188). SR2 global flavor binning @260426
#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst --CheckFiles

#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst --Merge 2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_NewBinning.log

#python MakeInput_public.py -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst -s Add -x ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning.log

#python MakeInput_public.py -e Run2 -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_Run2.log


## New binning but SR2 bin per flavor, SR1 global mass
#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst__SR2_PerFlavour__SR1_GlobalMass --CheckFiles

#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst__SR2_PerFlavour__SR1_GlobalMass --Merge 2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_NewBinning_SR2_PerFlavour_SR1_GlobalMass.log

#python MakeInput_public.py -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst__SR2_PerFlavour__SR1_GlobalMass --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_SR2_PerFlavour_SR1_GlobalMass.log

#python MakeInput_public.py -e Run2 -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst__SR2_PerFlavour__SR1_GlobalMass --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_SR2_PerFlavour_SR1_GlobalMass_Run2.log


## New binning but SR1 global mass+flavor (study purposes, we won't use this)
#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst__SR1_Global --CheckFiles

#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst__SR1_Global --Merge 2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_NewBinning_SR1_Global.log

#python MakeInput_public.py -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst__SR1_Global --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_SR1_Global.log

#python MakeInput_public.py -e Run2 -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst__SR1_Global --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_SR1_Global_Run2.log


## New binning but SR1 mass grouped by 400-600, 700-900, 1000+ and BDT binning
#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst__SR1_MassGroups --CheckFiles

#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst__SR1_MassGroups --Merge 2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_NewBinning_SR1_MassGroups.log

#python MakeInput_public.py -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst__SR1_MassGroups --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_SR1_MassGroups.log

#python MakeInput_public.py -e Run2 -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst__SR1_MassGroups --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_SR1_MassGroups_Run2.log


## New binning, SR1 per mass and BDT binning but checking stat requirement of Nbkg > 0.15 && rel. unc. < 0.5 per era. @260429
#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst__StatReqEra --CheckFiles

#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst__StatReqEra --Merge 2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_NewBinning_StatReqEra.log

#python MakeInput_public.py -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst__StatReqEra --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_StatReqEra.log

#python MakeInput_public.py -e Run2 -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst__StatReqEra --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_StatReqEra_Run2.log


## New binning, SR3 High and Low binning updated @260501
#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst__SR3Update --CheckFiles

#python MakeInput_public.py -i ANv7_NewBinning --PreFlag RunSyst__SR3Update --Merge 2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_NewBinning_SR3Update.log

#python MakeInput_public.py -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst__SR3Update --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_SR3Update.log

#python MakeInput_public.py -e Run2 -i ANv7_NewBinning --BDTver V3_Strict_15_Bin --PreFlag RunSyst__SR3Update --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_SR3Update_Run2.log




## New binning, final binning updated @260504
#python MakeInput_public.py -i ANv7_NewBinning_PR191 --PreFlag RunSyst --CheckFiles

#python MakeInput_public.py -i ANv7_NewBinning_PR191 --PreFlag RunSyst --Merge 2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_NewBinning_PR191.log

#python MakeInput_public.py -i ANv7_NewBinning_PR191 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_PR191.log

#python MakeInput_public.py -e Run2 -i ANv7_NewBinning_PR191 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_PR191_Run2.log



## New binning, final binning updated -- fixed the errors @260506
#python MakeInput_public.py -i ANv7_NewBinning_PR192 --PreFlag RunSyst --CheckFiles

#python MakeInput_public.py -i ANv7_NewBinning_PR192 --PreFlag RunSyst --Merge 2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_NewBinning_PR192.log

#python MakeInput_public.py -i ANv7_NewBinning_PR192 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_PR192.log

#python MakeInput_public.py -e Run2 -i ANv7_NewBinning_PR192 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_PR192_Run2.log

## Run Ext
#python MakeInput_public.py -i ANv7_NewBinning_PR192 -m 500 -x ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Ext -s Add --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr --Ext 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_PR192_Ext.log
#python MakeInput_public.py -e Run2 -i ANv7_NewBinning_PR192 -m 500 --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr --Ext 2>&1       | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_PR192_Run2_Ext.log

#python MakeInput_public.py -i ANv7_FixFakeSyst -m 500 -x ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Ext -s Add --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --Ext 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_FixFakeSyst_Ext.log
#python MakeInput_public.py -e Run2 -i ANv7_FixFakeSyst -m 500 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --Ext 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_FixFakeSyst_Run2_Ext.log



## ANv7_NewBinning_PR192 but fixed CR3H bjet bug @260513
#python MakeInput_public.py -i ANv7_NewBinning_PR192 --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv7_NewBinning_PR192 --PreFlag RunSyst --Merge --Data --Fake --CF --Conv --Prompt --MC --CR 2>&1  | tee log/MakeInput_public_Merge_CR_ANv7_NewBinning_PR192_FixCR3H.log

#python MakeInput_public.py -i ANv7_NewBinning_PR192 --BDTver V3_Strict_15_Bin -o FixCR3H -s Add -x ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_NewBinning_PR192_FixCR3H.log

#python MakeInput_public.py -e Run2 -i ANv7_NewBinning_PR192 --BDTver V3_Strict_15_Bin -o FixCR3H --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_NewBinning_PR192_FixCR3H_Run2.log



## SR unblinding: use the name -o FixCR3H for easy maintenance... @260521
#python MakeInput_public.py -i ANv7_NewBinning_PR192 --PreFlag RunSyst --CheckFiles

#python MakeInput_public.py -i ANv7_NewBinning_PR192 --PreFlag RunSyst --Merge --Data 2>&1   | tee log/MakeInput_public_Merge_Data_SR_ANv7_NewBinning_PR192.log

#python MakeInput_public.py -i ANv7_NewBinning_PR192 --BDTver V3_Strict_15_Bin -o FixCR3H -s Add -x ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_PR192_FixCR3H.log

#python MakeInput_public.py -e Run2 -i ANv7_NewBinning_PR192 --BDTver V3_Strict_15_Bin -o FixCR3H --PreFlag RunSyst --Syst --Decorr --JetDecorr 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_PR192_FixCR3H_Run2.log



## SR unblinding: Prune ZG scale_j 2018 sr2 down @260602
#python MakeInput_public.py -e 2018 -i ANv7_NewBinning_PR192 --BDTver V3_Strict_15_Bin -o FixCR3H_PruneZG --PreFlag RunSyst --Syst --Decorr --JetDecorr --regions sr2 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_PR192_FixCR3H_PruneZG_2018_sr2.log
#
#python MakeInput_public.py -e Run2 -i ANv7_NewBinning_PR192 --BDTver V3_Strict_15_Bin -o FixCR3H_PruneZG --PreFlag RunSyst --Syst --Decorr --JetDecorr --regions sr2 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_NewBinning_PR192_FixCR3H_PruneZG_Run2_sr2.log



## SR unblinding: Don't apply ConvSplit in SR1/SR2 so that we don't have to consider PruneZG in the first place @260608 --> This change was updated was PR194
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 --PreFlag RunSyst --Merge --Unblind 2>&1   | tee log/MakeInput_public_Merge_Data_SR_ANv7_ConvUpdate_PR192.log
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 --PreFlag RunSyst --Merge --CR 2>&1   | tee log/MakeInput_public_Merge_Data_CR_ANv7_ConvUpdate_PR192.log

#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192.log
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_Run2.log


################ NOTE: At this point, -o is now a part of the MergedFiles path. Histogram-level changes, with using existing MergedFiles, should be tagged by -T.

## SR unblinding: Try removing DYJets10to50_MG at all @260612
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG --PreFlag RunSyst --Merge --Conv --MC --Unblind 2>&1   | tee log/MakeInput_public_Merge_Conv_MC_SR_ANv7_ConvUpdate_PR192_NoLowDYMG.log
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG --PreFlag RunSyst --Merge --Conv --MC --CR 2>&1   | tee log/MakeInput_public_Merge_Conv_MC_CR_ANv7_ConvUpdate_PR192_NoLowDYMG.log

#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG.log
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_Run2.log

## test CnC
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind --CnC 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_CnC.log
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --CnC 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_CnC.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind --CnC 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_Run2_CnC.log
#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --CnC 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_Run2_CnC.log

## Remove TG, TTG @260622
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG_NoTGTTG --PreFlag RunSyst --Merge --Conv --MC --Unblind 2>&1   | tee log/MakeInput_public_Merge_Conv_MC_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_NoTGTTG.log
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG_NoTGTTG --PreFlag RunSyst --Merge --Conv --MC --CR 2>&1   | tee log/MakeInput_public_Merge_Conv_MC_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_NoTGTTG.log

#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG_NoTGTTG --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_NoTGTTG.log
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG_NoTGTTG --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_NoTGTTG.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG_NoTGTTG --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_NoTGTTG_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG_NoTGTTG --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_NoTGTTG_Run2.log



## SR unblinding: Use MiNNLO instead of DY MG @260612
#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 --PreFlag RunSyst --CheckFiles
#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 --PreFlag RunSyst --Merge --Unblind 2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_DYConvUpdate_PR192.log
#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 --PreFlag RunSyst --Merge --CR 2>&1   | tee log/MakeInput_public_Merge_CR_ANv7_DYConvUpdate_PR192.log

#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 --BDTver V3_Strict_15_Bin -s Add -x ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_DYConvUpdate_PR192.log
#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 --BDTver V3_Strict_15_Bin -s Add -x ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_DYConvUpdate_PR192.log

#python MakeInput_public.py -e Run2 -i ANv7_DYConvUpdate_PR192 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_DYConvUpdate_PR192_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_DYConvUpdate_PR192 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_DYConvUpdate_PR192_Run2.log

## test CnC
#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 --BDTver V3_Strict_15_Bin -s Add -x ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind --CnC 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_DYConvUpdate_PR192_CnC.log
#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 --BDTver V3_Strict_15_Bin -s Add -x ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --CnC 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_DYConvUpdate_PR192_CnC.log

#python MakeInput_public.py -e Run2 -i ANv7_DYConvUpdate_PR192 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind --CnC 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_DYConvUpdate_PR192_Run2_CnC.log
#python MakeInput_public.py -e Run2 -i ANv7_DYConvUpdate_PR192 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --CnC 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_DYConvUpdate_PR192_Run2_CnC.log

## Remove TG, TTG @260622
#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 -o NoTGTTG --PreFlag RunSyst --Merge --Conv --MC --Unblind 2>&1   | tee log/MakeInput_public_Merge_Conv_MC_SR_ANv7_DYConvUpdate_PR192_NoTGTTG.log
#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 -o NoTGTTG --PreFlag RunSyst --Merge --Conv --MC --CR 2>&1   | tee log/MakeInput_public_Merge_Conv_MC_CR_ANv7_DYConvUpdate_PR192_NoTGTTG.log

#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 -o NoTGTTG --BDTver V3_Strict_15_Bin -s Add -x ANv7_DYConvUpdate_PR192_HNL_ULIDv2_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_DYConvUpdate_PR192_NoTGTTG.log
#python MakeInput_public.py -i ANv7_DYConvUpdate_PR192 -o NoTGTTG --BDTver V3_Strict_15_Bin -s Add -x ANv7_DYConvUpdate_PR192_HNL_ULIDv2_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_DYConvUpdate_PR192_NoTGTTG.log

#python MakeInput_public.py -e Run2 -i ANv7_DYConvUpdate_PR192 -o NoTGTTG --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_DYConvUpdate_PR192_NoTGTTG_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_DYConvUpdate_PR192 -o NoTGTTG --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_DYConvUpdate_PR192_NoTGTTG_Run2.log


## FitTests (Use ConvUpdate first, at the moment) @260623
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5 --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5.log
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5 --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_Run2.log

## LowStatNeff5 was successful. Only small suspicious behavior in the highest masses. Test different HighMassScalers. @260626

# DYVBFscaler = 0.2
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_HighMassScaler0p2 -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_HighMassScaler0p2.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_HighMassScaler0p2 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_HighMassScaler0p2_Run2.log

# DYVBFscaler = 0.5
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_HighMassScaler0p5 -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_HighMassScaler0p5.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_HighMassScaler0p5 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_HighMassScaler0p5_Run2.log

# DYVBFscaler = 1
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_HighMassScaler1 -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_HighMassScaler1.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_HighMassScaler1 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_HighMassScaler1_Run2.log



#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T MergeSR2Bin78 --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_MergeSR2Bin78.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T MergeSR2Bin78 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_MergeSR2Bin78_Run2.log


#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T MergeSR3EEBin1314 --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR3EEBin1314_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_MergeSR3EEBin1314.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T MergeSR3EEBin1314 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_MergeSR3EEBin1314_Run2.log


#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T SmoothEE --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_SmoothEE_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_SmoothEE.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T SmoothEE --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_SmoothEE_Run2.log



## Now NoLowDYMG + LowStatNeff5 is a new nominal. Additional fit tests: Merge SR2 bin7-8 @260706
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78 --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_MergeSR2Bin78.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_Run2.log


## Now NoLowDYMG + LowStatNeff5 is a new nominal. Additional fit tests: Fill any holes with 0.001 @260707
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_FillHoles --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_FillHoles.log
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_FillHoles --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_FillHoles.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_FillHoles --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_FillHoles_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_FillHoles --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_FillHoles_Run2.log


## Now NoLowDYMG + LowStatNeff5 is a new nominal. Additional fit tests: Merge SR2 bin7-8 + Fill any holes with 0.001 @260707
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_FillHoles --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles.log
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_FillHoles --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_FillHoles --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_FillHoles --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_Run2.log



## Now NoLowDYMG + LowStatNeff5 is a new nominal. Merge SR2 bin3-4 and 7-8 @260713
#python MakeInput_public.py -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin3478 --BDTver V3_Strict_15_Bin -s Add -x ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin3478_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_MergeSR2Bin3478.log

#python MakeInput_public.py -e Run2 -i ANv7_ConvUpdate_PR192 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin3478 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ConvUpdate_PR192_NoLowDYMG_LowStatNeff5_MergeSR2Bin3478_Run2.log



## CR3_InvBJet is the reason for the low p-value in SR3BDT MuMu (fakes are overestimated). Add LooseID syst @260721
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG --PreFlag RunSyst --Merge --Fake --Unblind 2>&1   | tee log/MakeInput_public_Merge_Fake_MC_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG --PreFlag RunSyst --Merge --Fake --CR 2>&1   | tee log/MakeInput_public_Merge_Fake_MC_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG.log

#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_Run2.log


## LooseID syst + powheg WZ syst @260722
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --Merge --AltWZ --Unblind 2>&1   | tee log/MakeInput_public_Merge_AltWZ_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --Merge --AltWZ --CR 2>&1   | tee log/MakeInput_public_Merge_AltWZ_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG.log

#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZ --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZ --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZ --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZ --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_Run2.log


## LooseID syst + powheg WZ syst normalized to amc @260730
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNorm --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNorm --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNorm --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNorm --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_Run2.log



## Save intact wz powheg to check + don't apply LowStat to AltWZ @260807
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --Merge --AltWZ --Unblind 2>&1   | tee log/MakeInput_public_Merge_AltWZ_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --Merge --AltWZ --CR 2>&1   | tee log/MakeInput_public_Merge_AltWZ_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG.log

#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_SaveAltWZ --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_SaveAltWZ.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_SaveAltWZ --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_SaveAltWZ.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_SaveAltWZ --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_SaveAltWZ_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_SaveAltWZ --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_SaveAltWZ_Run2.log


## Symmetrize and inflate WZ uncertainty, as Halil suggested @260811

# simple symmetrization
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_Run2.log

# 10% inflate
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym10 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym10.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym10 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym10.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym10 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym10_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym10 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym10_Run2.log

# 20% inflate
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym20 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym20 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym20 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym20 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_Run2.log

# 30% inflate
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym30 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym30.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym30 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym30.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym30 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym30_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym30 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym30_Run2.log



# simple symmetrization with WZ normalization
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym0.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym0.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym0_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym0_Run2.log

# 10% inflate with WZ normalization
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym10 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym10.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym10 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym10.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym10 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym10_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym10 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym10_Run2.log

# 20% inflate with WZ normalization
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym20 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym20 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym20 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym20 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_Run2.log

# 30% inflate with WZ normalization
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym30 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym30.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym30 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym30.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym30 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym30_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym30 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym30_Run2.log


# 20% inflate with WZ normalization, WZ region decorrelation (test purposes)
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_Run2.log



### Separated fake syst @260818
#python MakeInput_public.py -i ANv7_SepFakeSyst_PR197 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --CheckFiles --Unblind
#python MakeInput_public.py -i ANv7_SepFakeSyst_PR197 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --CheckFiles --CR

#python MakeInput_public.py -i ANv7_SepFakeSyst_PR197 -o NoLowDYMG --PreFlag RunSyst --Merge --Unblind 2>&1   | tee log/MakeInput_public_Merge_SR_ANv7_SepFakeSyst_PR197.log
#python MakeInput_public.py -i ANv7_SepFakeSyst_PR197 -o NoLowDYMG --PreFlag RunSyst --Merge --CR 2>&1   | tee log/MakeInput_public_Merge_CR_ANv7_SepFakeSyst_PR197.log

#python MakeInput_public.py -i ANv7_SepFakeSyst_PR197 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --Merge --AltWZ --Unblind 2>&1   | tee log/MakeInput_public_Merge_AltWZ_SR_ANv7_SepFakeSyst_PR197_NoLowDYMG.log
#python MakeInput_public.py -i ANv7_SepFakeSyst_PR197 -o NoLowDYMG -T AltWZ --PreFlag RunSyst --Merge --AltWZ --CR 2>&1   | tee log/MakeInput_public_Merge_AltWZ_CR_ANv7_SepFakeSyst_PR197_NoLowDYMG.log

#python MakeInput_public.py -i ANv7_SepFakeSyst_PR197 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_SepFakeSyst_PR197_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0.log
#python MakeInput_public.py -i ANv7_SepFakeSyst_PR197 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_SepFakeSyst_PR197_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0.log

#python MakeInput_public.py -e Run2 -i ANv7_SepFakeSyst_PR197 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_SepFakeSyst_PR197_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_SepFakeSyst_PR197 -o NoLowDYMG -T LowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_SepFakeSyst_PR197_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_Run2.log



### AltWZonly setting showed Impact fit failure --> the script has changed to apply LowStat treatment to AltWZs as well. @260901

# symmetrization without inflation
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_Run2.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_Run2.log

# symmetrization without inflation + altwz region decorrelation ##### Archive : ANv8 #####
python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr.log
python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr.log

python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Run2.log
python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Run2.log

# symmetrization without inflation + altwz region decorrelation <-- this is the new nominal at the unblinding stage. Run Ext @260916 ###### Archive : ANv8 #####
python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind --Ext 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Ext.log
python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --Ext 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Ext.log

python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind --Ext 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Run2_Ext.log
python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR --Ext 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Run2_Ext.log

# symmetrization without inflation + altwz region decorrelation : high mass scaler
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5 -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5.log
#python MakeInput_public.py -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5 -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5.log

#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5 -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --Unblind 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_SR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Run2_HighMassScaler0p5.log
#python MakeInput_public.py -e Run2 -i ANv7_ExtraFakeSyst_PR195 -o NoLowDYMG -T NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5 -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 --BDTver V3_Strict_15_Bin --PreFlag RunSyst --Syst --Decorr --JetDecorr --CR 2>&1 | tee log/MakeInput_public_Syst_Decorr_JetDecorr_CR_ANv7_ExtraFakeSyst_PR195_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_Run2_HighMassScaler0p5.log





#################### TEST permission setting #################
#python MakeInput_public.py -e 2018 -m 1000 -c EMu -i ANv5_BDTV3_SR1_FixRepeatBin -o AltBin_FixCorr_TESTpermission -s Print --PreFlag RunSyst --BDTver V3_Strict_15_Bin --Syst --Decorr --JetDecorr 2>&1       | tee log/TEST.log
