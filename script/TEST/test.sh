#!/bin/bash

era=$1
isdata=$2
sample=$3
skim=$4
flag=$5

#echo -e ".L test.C\n test(\"$era\",$isdata,\"$sample\",\"$skim\",\"$flag\");" | root -l 2>&1 | tee "$3".log

# HNL_LeptonFakeRate
#echo -e ".L test.C\n test(\"2017\",false,\"QCD_Mu_300\",\"FakeEventSkimBDT\",\"FakeRateTruth\");" | root -l 2>&1 | tee "QCD_Mu_300".log
#echo -e ".L test.C\n test(\"2017\",false,\"QCD_Mu_300\",\"FakeEventSkimBDT\",\"\");" | root -l 2>&1 | tee "QCD_Mu_300".log
#echo -e ".L test.C\n test(\"2017\",false,\"TTLJ\",\"HNFakeBDT\",\"\");" | root -l 2>&1 | tee "TTLJ".log
#echo -e ".L test.C\n test(\"2017\",false,\"DYJets\",\"HNFakeBDT\",\"\");" | root -l 2>&1 | tee "DYJets".log
#echo -e ".L test.C\n test(\"2017\",false,\"WJets\",\"HNFakeBDT\",\"\");" | root -l 2>&1 | tee "WJets".log

# HNL_SignalRegionPlotter
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleMuon\",\"HNMultiLepBDT\",\"RunFake\");" | root -l 2>&1 | tee "DoubleMuon_RunFake".log
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleEG\",\"HNMultiLepBDT\",\"RunFake\");" | root -l 2>&1 | tee "DoubleEG_RunFake".log
#echo -e ".L test.C\n test(\"2017\",true,\"MuonEG\",\"HNMultiLepBDT\",\"RunFake\");" | root -l 2>&1 | tee "MuonEG_RunFake".log
#echo -e ".L test.C\n test(\"2017\",false,\"DYJets\",\"HNMultiLepBDT\",\"RunCF\");" | root -l 2>&1 | tee "DYJets_RunCF".log
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleEG\",\"DileptonBDT\",\"RunCF\");" | root -l 2>&1 | tee "DoubleEG_RunCF".log
#echo -e ".L test.C\n test(\"2017\",false,\"DYTypeI_DF_M1000\",\"HNMultiLepBDT\",\"\");" | root -l 2>&1 | tee "DYTypeI_DF_M1000".log
#echo -e ".L test.C\n test(\"2017\",false,\"VBFTypeI_DF_M1000\",\"HNMultiLepBDT\",\"\");" | root -l 2>&1 | tee "VBFTypeI_DF_M1000".log
#echo -e ".L test.C\n test(\"2017\",false,\"SSWWTypeI_DF_M1000\",\"HNMultiLepBDT\",\"\");" | root -l 2>&1 | tee "SSWWTypeI_DF_M1000".log
#echo -e ".L test.C\n test(\"2017\",false,\"SSWWTypeI_SF_M1000\",\"HNMultiLepBDT\",\"\");" | root -l 2>&1 | tee "SSWWTypeI_SF_M1000".log

# HNL_SignalRegionPlotter (fake closure)
#echo -e ".L test.C\n test(\"2017\",false,\"TTLJ\",\"HNMultiLepBDT\",\"RunFakeClosurePred\");" | root -l 2>&1 | tee "TTLJ_ClosPred".log
#echo -e ".L test.C\n test(\"2017\",false,\"TTLJ\",\"HNMultiLepBDT\",\"RunFakeClosureObs\");" | root -l 2>&1 | tee "TTLJ_ClosObs".log

# HNL_ControlRegionPlotter
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleMuon\",\"HNMultiLepBDT\",\"SSMultiLep,RunFake\");" | root -l 2>&1 | tee "DoubleMuon_RunFake".log
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleMuon\",\"HNMultiLepBDT\",\"SSMultiLep,RunFake\");" | root -l 2>&1 | tee "DoubleMuon_RunFake".log
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleMuon\",\"HNMultiLepBDT\",\"SSMultiLep\");" | root -l 2>&1 | tee "DoubleMuon_SSMultiLep".log
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleEG\",\"HNMultiLepBDT\",\"SSMultiLep\");" | root -l 2>&1 | tee "DoubleEG_SSMultiLep".log
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleEG\",\"DileptonBDT\",\"OS_VR\");" | root -l 2>&1 | tee "DoubleEG_OS_VR".log
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleMuon\",\"HNMultiLepBDT\",\"RunFake,TestFakes,SS_CR\");" | root -l 2>&1 | tee "DoubleMuon_RunFake".log
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleMuon\",\"HNMultiLepBDT\",\"LLL_VR\");" | root -l 2>&1 | tee "DoubleMuon_LLL_VR".log
#echo -e ".L test.C\n test(\"2017\",true,\"MuonEG\",\"HNMultiLepBDT\",\"LLL_VR\");" | root -l 2>&1 | tee "MuonEG_LLL_VR".log
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleEG\",\"HNMultiLepBDT\",\"LLL_VR\");" | root -l 2>&1 | tee "DoubleEG_LLL_VR".log
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleEG\",\"HNMultiLepBDT\",\"SS_CR\");" | root -l 2>&1 | tee "DoubleEG_SS_CR".log
#echo -e ".L test.C\n test(\"2017\",false,\"ZZ\",\"HNMultiLepBDT\",\"LLL_VR\");" | root -l 2>&1 | tee "ZZ_LLL_VR".log
#echo -e ".L test.C\n test(\"2017\",true,\"DoubleEG\",\"HNMultiLepBDT\",\"SS_CR\");" | root -l 2>&1 | tee "DoubleEG_SS_CR".log
echo -e ".L test.C\n test(\"2017\",true,\"DoubleEG\",\"HNMultiLepBDT\",\"SS_CR,RunFake\");" | root -l 2>&1 | tee "DoubleEG_SS_CR_RunFake".log
#echo -e ".L test.C\n test(\"2018\",false,\"ZG\",\"HNMultiLepBDT\",\"SS_CR,RunConv,SaveIntConv\");" | root -l 2>&1 | tee "2018_ZG_SS_CR_RunConv_SaveIntConv".log
#echo -e ".L test.C\n test(\"2018\",false,\"ZG\",\"HNMultiLepBDT\",\"SS_CR,RunConv\");" | root -l 2>&1 |             tee "2018_ZG_SS_CR_RunConv".log
#echo -e ".L test.C\n test(\"2018\",false,\"ZG\",\"HNMultiLepBDT\",\"SS_CR,SaveIntConv\");" | root -l 2>&1 |         tee "2018_ZG_SS_CR_SaveIntConv".log
#echo -e ".L test.C\n test(\"2018\",false,\"ZG\",\"HNMultiLepBDT\",\"SS_CR\");" | root -l 2>&1 |                     tee "2018_ZG_SS_CR".log
