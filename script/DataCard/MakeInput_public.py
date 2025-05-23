# Run first: python MakeInput_public.py -e 2018 --CheckFiles -i HEMJet --PreFlag RemoveHEMJet
# python MakeInput_public.py --Merge -e 2018 -i HEMJet --PreFlag RemoveHEMJet \ python MakeInput_public.py --CR --Merge -e 2018 -i HEMJet --PreFlag RemoveHEMJet \
# python MakeInput_public.py [--Syst] [--Decorr] -i HEMJet --PreFlag RemoveHEMJet \ python MakeInput_public.py --CR [--Syst] [--Decorr] -i HEMJet --PreFlag RemoveHEMJet

import os, sys
import argparse
from ROOT import *
import array
gROOT.SetBatch(kTRUE)

parser = argparse.ArgumentParser(description='script for creating input root file.',formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-e', dest='eras', choices=['2016preVFP','2016postVFP','2017','2018','Run2'], default=['2016preVFP','2016postVFP','2017','2018'], nargs='+', help='eras to run')
parser.add_argument('-m', dest='masses', nargs='+', help='signal masses to run')
parser.add_argument('-c', dest='channels', nargs='+', default=["MuMu","EE","EMu"], help='lepton channels to run')
parser.add_argument('-i', dest='inputTag', default='ANv5', help='tag attached to the input SKFlatOutput files')
parser.add_argument('-t', dest='histTag', nargs='+', default=['HNL_ULIDv2'], help='this is the param name of the SKFlatAnalyzer. Mostly IDs.')
parser.add_argument('--Scan', action='store_true', help='scan the bin content')
parser.add_argument('--CnC', action='store_true', help='1bin cut and count setting')
parser.add_argument('--CR', action='store_true', help='Make HNL_ControlRegion_Plotter input (default : HNL_SignalRegion_Plotter)')
parser.add_argument('--Syst', action='store_true', help='Add systematics')
parser.add_argument('--Decorr', action='store_true', help='Decorrelate Fake, CF syst sources')
parser.add_argument('--JetDecorr', action='store_true', help='Decorrelate jet syst sources as well')
parser.add_argument('--PreFlag', nargs='+', help='Your private flag names')
parser.add_argument('--PostFlag', nargs='+', help='Your private flag names')
parser.add_argument('--Merge', action='store_true', help='hadd the needed histograms') # NOTE Run2 Merging deprecated.
parser.add_argument('--CheckFiles', action='store_true', help='check all inputs before merge')
args = parser.parse_args()

PreFlag = ""
if args.PreFlag is None: pass
else:
  for this_flag in args.PreFlag:
    PreFlag += this_flag+"__" # SKFlat convention
PostFlag = ""
if args.PostFlag is None: pass
else:
  for this_flag in args.PostFlag:
    PostFlag += this_flag+"__" # SKFlat convention

if not args.masses: # When you don't want to type all those masses!!
  args.masses = ["M85","M90","M95","M100","M125","M150","M200","M250","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000","M25000","M30000","M40000","M50000","M60000"]
  #masses = ["M100","M250","M1000","M10000"]
else:
  for i in range(len(args.masses)):
    args.masses[i] = "M"+args.masses[i]

if not args.channels:
  print("Please specify the lepton channels; e.g. MuMu .")
  exit()

HistChannelMap = {'MuMu':'Muon', 'EE':'Electron', 'EMu':'ElectronMuon'}
## Ugly region maps ##
RegionToDefFlagMap = {}
RegionToChannelMap = {}
RegionToHistSuffixMap = {}

inputTag = args.inputTag

if not args.histTag:
  print("Please specify the hist tag; e.g. HNL_ULIDv2 .")
  exit()

outputTagSuffix = ""
outputTagSuffix += '_'+PreFlag.rstrip('_').replace('__','_') if args.PreFlag else ""
outputTagSuffix += '_'+PostFlag.rstrip('_').replace('__','_') if args.PostFlag else ""
if args.CnC:
  outputTagSuffix += '_CnC'
if args.Decorr:
  outputTagSuffix += '_Decorr'
  if args.JetDecorr:
    outputTagSuffix += '_JetDecorr'

# Skim
DataSkim = "_SkimTree_HNMultiLepBDT_"
FakeSkim = "_SkimTree_HNMultiLepBDT_"
#CFSkim = "_SkimTree_HNMultiLepBDT_" #FIXME MC CF
CFSkim = "_SkimTree_DileptonBDT_" #FIXME Data CF
MCSkim = "_SkimTree_HNMultiLepBDT_"
SignalSkim = "_SkimTree_HNMultiLepBDT_"

# This will do necessary hadd for you.
MergeData   = True if args.Merge else False
MergeFake   = True if args.Merge else False  # RunFake
MergeCF     = True if args.Merge else False  # RunCF
MergeConv   = True if args.Merge else False  # RunConv
MergeMC     = True if args.Merge else False  # RunPrompt
MergeSignal = True if args.Merge else False
#MergeDYVBF = True if args.Merge else False
#MergeSSWW  = True if args.Merge else False

if args.CR:
  Blinded = False # Blinded --> the total background will be used as data_obs
  DefFlags = ["MultiLepton__"]
  Analyzer = "HNL_ControlRegion_Plotter"

  #regions = ["sr1_inv","sr2_inv","sr3_inv","cf_cr1","cf_cr2","cf_cr3","ww_cr1","ww_cr2","zg_cr3","wz_cr1","wz_cr2","wz_cr3","zz_cr1","zz_cr2","zz_cr3"] if not args.Merge else "" # for CRs
  regions = ["sr1_InvMET","sr2_InvMET","sr3_InvMET","sr1_InvBJet","sr2_InvBJet","sr3_InvBJet","zg_cr","wz_cr1","wz_cr2","wz_cr3","zz_cr"] if not args.Merge else "" # for CRs
  #regions = ["zg_cr","zz_cr"] if not args.Merge else "" # for CRs

  RegionToDefFlagMap['sr_inv']     = "MultiLepton__"
  RegionToDefFlagMap['sr1_inv']    = "MultiLepton__"
  RegionToDefFlagMap['sr2_inv']    = "MultiLepton__"
  RegionToDefFlagMap['sr3_inv']    = "MultiLepton__"
  RegionToDefFlagMap['sr1_InvMET'] = "MultiLepton__"
  RegionToDefFlagMap['sr2_InvMET'] = "MultiLepton__"
  RegionToDefFlagMap['sr3_InvMET'] = "MultiLepton__"
  RegionToDefFlagMap['sr1_InvBJet'] = "MultiLepton__"
  RegionToDefFlagMap['sr2_InvBJet'] = "MultiLepton__"
  RegionToDefFlagMap['sr3_InvBJet'] = "MultiLepton__"
  RegionToDefFlagMap['zg_cr']      = "MultiLepton__"
  RegionToDefFlagMap['wz_cr']      = "MultiLepton__"
  RegionToDefFlagMap['wz_cr1']     = "MultiLepton__"
  RegionToDefFlagMap['wz_cr2']     = "MultiLepton__"
  RegionToDefFlagMap['wz_cr3']     = "MultiLepton__"
  RegionToDefFlagMap['zz_cr']      = "MultiLepton__"

  RegionToChannelMap['sr_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr1_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr2_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr3_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr1_InvMET'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr2_InvMET'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr3_InvMET'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr1_InvBJet'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr2_InvBJet'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr3_InvBJet'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['zg_cr']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr1']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr2']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr3']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['zz_cr']  = {'MuMu':'MuMuMuMu', 'EE':'EEEE', 'EMu':'EMuLL'}

  RegionToHistSuffixMap['sr_inv']  = {'MuMu':'LimitBins/MuonCR',  'EE':'LimitBins/ElectronCR',  'EMu':'LimitBins/ElectronMuonCR'}
  RegionToHistSuffixMap['sr1_inv'] = {'MuMu':'LimitBins/MuonCR1', 'EE':'LimitBins/ElectronCR1', 'EMu':'LimitBins/ElectronMuonCR1'}
  RegionToHistSuffixMap['sr2_inv'] = {'MuMu':'LimitBins/MuonCR2', 'EE':'LimitBins/ElectronCR2', 'EMu':'LimitBins/ElectronMuonCR2'}
  RegionToHistSuffixMap['sr3_inv'] = {'MuMu':'LimitBins/MuonCR3', 'EE':'LimitBins/ElectronCR3', 'EMu':'LimitBins/ElectronMuonCR3'}
  RegionToHistSuffixMap['sr1_InvMET'] = {'MuMu':'LimitBins/MuonInvMETCR1', 'EE':'LimitBins/ElectronInvMETCR1', 'EMu':'LimitBins/ElectronMuonInvMETCR1'}
  RegionToHistSuffixMap['sr2_InvMET'] = {'MuMu':'LimitBins/MuonInvMETCR2', 'EE':'LimitBins/ElectronInvMETCR2', 'EMu':'LimitBins/ElectronMuonInvMETCR2'}
  RegionToHistSuffixMap['sr3_InvMET'] = {'MuMu':'LimitBins/MuonInvMETCR3', 'EE':'LimitBins/ElectronInvMETCR3', 'EMu':'LimitBins/ElectronMuonInvMETCR3'}
  RegionToHistSuffixMap['sr1_InvBJet'] = {'MuMu':'LimitBins/MuonInvBJetCR1', 'EE':'LimitBins/ElectronInvBJetCR1', 'EMu':'LimitBins/ElectronMuonInvBJetCR1'}
  RegionToHistSuffixMap['sr2_InvBJet'] = {'MuMu':'LimitBins/MuonInvBJetCR2', 'EE':'LimitBins/ElectronInvBJetCR2', 'EMu':'LimitBins/ElectronMuonInvBJetCR2'}
  RegionToHistSuffixMap['sr3_InvBJet'] = {'MuMu':'LimitBins/MuonInvBJetCR3', 'EE':'LimitBins/ElectronInvBJetCR3', 'EMu':'LimitBins/ElectronMuonInvBJetCR3'}
  RegionToHistSuffixMap['zg_cr']   = {'MuMu':'LimitShape_ZG/Binned', 'EE':'LimitShape_ZG/Binned', 'EMu':'LimitShape_ZG/Binned'}
  RegionToHistSuffixMap['wz_cr']   = {'MuMu':'LimitShape_WZ/Binned', 'EE':'LimitShape_WZ/Binned', 'EMu':'LimitShape_WZ/Binned'}
  RegionToHistSuffixMap['wz_cr1']  = {'MuMu':'LimitShape_WZ_SR1/Binned', 'EE':'LimitShape_WZ_SR1/Binned', 'EMu':'LimitShape_WZ_SR1/Binned'}
  RegionToHistSuffixMap['wz_cr2']  = {'MuMu':'LimitShape_WZ_SR2/Binned', 'EE':'LimitShape_WZ_SR2/Binned', 'EMu':'LimitShape_WZ_SR2/Binned'}
  RegionToHistSuffixMap['wz_cr3']  = {'MuMu':'LimitShape_WZ_SR3/Binned', 'EE':'LimitShape_WZ_SR3/Binned', 'EMu':'LimitShape_WZ_SR3/Binned'}
  RegionToHistSuffixMap['zz_cr']   = {'MuMu':'LimitShape_ZZ/Binned', 'EE':'LimitShape_ZZ/Binned', 'EMu':'LimitShape_ZZ/Binned'}

else:
  Blinded = True # Blinded --> the total background will be used as data_obs
  DefFlags = [""]
  Analyzer = "HNL_SignalRegion_Plotter"

  regions = ["sr1","sr2","sr3"] if not args.Merge else "" # for SRs

  RegionToDefFlagMap['sr']  = ""
  RegionToDefFlagMap['sr1'] = ""
  RegionToDefFlagMap['sr2'] = ""
  RegionToDefFlagMap['sr3'] = ""

  RegionToChannelMap['sr'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr1'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr2'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['sr3'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}

  RegionToHistSuffixMap['sr'] = {'MuMu':'LimitBins/MuonSR', 'EE':'LimitBins/ElectronSR', 'EMu':'LimitBins/ElectronMuonSR'}
  RegionToHistSuffixMap['sr1'] = {'MuMu':'LimitBins/MuonSR1', 'EE':'LimitBins/ElectronSR1', 'EMu':'LimitBins/ElectronMuonSR1'}
  RegionToHistSuffixMap['sr2'] = {'MuMu':'LimitBins/MuonSR2', 'EE':'LimitBins/ElectronSR2', 'EMu':'LimitBins/ElectronMuonSR2'}
  RegionToHistSuffixMap['sr3'] = {'MuMu':'LimitBins/MuonSR3', 'EE':'LimitBins/ElectronSR3', 'EMu':'LimitBins/ElectronMuonSR3'}

# Region dependent RunConv skim
ConvSkim = {}
for DefFlag in DefFlags:
  ConvSkim[DefFlag] = {}
  for this_conv in ["TG","TTG","WZG","WWG","ZGToLLG","ZGToLLG_PtG_130","DYJets_MG","DYJets10to50_MG"]:
    ConvSkim[DefFlag][this_conv] = "_SkimTree_HNMultiLepBDT_"
  for this_conv in ["WGToLNuG","WGToLNuG_MG","WGToLNuG_01J_PtG_130","WGToLNuG_01J_PtG_300","WGToLNuG_01J_PtG_500","WGJJToLNu"]:
    ConvSkim[DefFlag][this_conv] = "_SkimTree_DileptonBDT_"


SystList = [
            "JetResUp","JetResDown",
            "JetEnUp","JetEnDown",
            "JetPUIDUp","JetPUIDDown",
            "JetPNETUp","JetPNETDown",
            "MuonEnUp","MuonEnDown",
            "MuonResUp","MuonResDown",
            "ElectronEnUp","ElectronEnDown",
            "ElectronResUp","ElectronResDown",
            "MuonRecoSFUp","MuonRecoSFDown",
            "MuonIDSFUp","MuonIDSFDown",
            "MuonTriggerSFUp","MuonTriggerSFDown",
            "ElectronRecoSFUp","ElectronRecoSFDown",
            "ElectronIDSFUp","ElectronIDSFDown",
            "ElectronTriggerSFUp","ElectronTriggerSFDown",
            "BTagSFHTagCorrUp","BTagSFHTagCorrDown",
            "BTagSFHTagUnCorrUp","BTagSFHTagUnCorrDown",
            "BTagSFLTagCorrUp","BTagSFLTagCorrDown",
            "BTagSFLTagUnCorrUp","BTagSFLTagUnCorrDown",
            "METUnclUp","METUnclDown",
            "PrefireUp","PrefireDown",
            "PUUp","PUDown",
            "CFRateUp","CFRateDown",
            #"CFSFUp","CFSFDown",
            "FRUp","FRDown",
            "FRHighPtUp","FRHighPtDown",
            "PDFUp","PDFDown",
            "ScaleUp","ScaleDown",
           ]

SystNameMap = {}
for era in ["2016","2016preVFP","2016postVFP","2017","2018"]:
  SystNameMap[era] = {}
  SystNameMap[era]["JetResUp"]            = "CMS_res_j_"+era+"Up"
  SystNameMap[era]["JetEnUp"]             = "CMS_scale_j_"+era+"Up"
  SystNameMap[era]["JetPUIDUp"]           = "CMS_eff_j_PUJET_id_"+era+"Up"
  SystNameMap[era]["JetPNETUp"]           = "CMS_eff_j_PNET_"+era+"Up"
  SystNameMap[era]["MuonEnUp"]            = "CMS_scale_m_"+era+"Up"
  SystNameMap[era]["MuonResUp"]           = "CMS_res_m_"+era+"Up"
  SystNameMap[era]["MuonRecoSFUp"]        = "CMS_eff_m_reco_"+era+"Up"
  SystNameMap[era]["MuonIDSFUp"]          = "CMS_eff_m_id_"+era+"Up"
  SystNameMap[era]["MuonTriggerSFUp"]     = "CMS_eff_m_trigger_"+era+"Up"
  SystNameMap[era]["ElectronEnUp"]        = "CMS_scale_e_"+era+"Up"
  SystNameMap[era]["ElectronResUp"]       = "CMS_res_e_"+era+"Up"
  SystNameMap[era]["ElectronRecoSFUp"]    = "CMS_eff_e_reco_"+era+"Up"
  SystNameMap[era]["ElectronIDSFUp"]      = "CMS_eff_e_id_"+era+"Up"
  SystNameMap[era]["ElectronTriggerSFUp"] = "CMS_eff_e_trigger_"+era+"Up"
  SystNameMap[era]["BTagSFHTagCorrUp"]    = "CMS_btag_hf_corrUp"
  SystNameMap[era]["BTagSFHTagUnCorrUp"]  = "CMS_btag_hf_uncorr_"+era+"Up"
  SystNameMap[era]["BTagSFLTagCorrUp"]    = "CMS_btag_lf_corrUp"
  SystNameMap[era]["BTagSFLTagUnCorrUp"]  = "CMS_btag_lf_uncorr_"+era+"Up"
  SystNameMap[era]["METUnclUp"]           = "CMS_scale_met_"+era+"Up"
  SystNameMap[era]["PrefireUp"]           = "CMS_l1_ecal_prefiring_"+era+"Up"
  SystNameMap[era]["PUUp"]                = "CMS_pileup_13TeV"+"Up" # full correlation
  SystNameMap[era]["CFRateUp"]            = "CMS_cf_stat_"+era+"Up"
  SystNameMap[era]["FRUp"]                = "CMS_fake_stat_"+era+"Up"
  SystNameMap[era]["FRHighPtUp"]          = "CMS_fake_highpt_"+era+"Up"
  SystNameMap[era]["PDFUp"]               = "pdf"+"Up" # full correlation
  SystNameMap[era]["ScaleUp"]             = "QCDscale"+"Up" # full correlation
  SystNameMap[era]["JetResDown"]            = "CMS_res_j_"+era+"Down"
  SystNameMap[era]["JetEnDown"]             = "CMS_scale_j_"+era+"Down"
  SystNameMap[era]["JetPUIDDown"]           = "CMS_eff_j_PUJET_id_"+era+"Down"
  SystNameMap[era]["JetPNETDown"]           = "CMS_eff_j_PNET_"+era+"Down"
  SystNameMap[era]["MuonEnDown"]            = "CMS_scale_m_"+era+"Down"
  SystNameMap[era]["MuonResDown"]           = "CMS_res_m_"+era+"Down"
  SystNameMap[era]["MuonRecoSFDown"]        = "CMS_eff_m_reco_"+era+"Down"
  SystNameMap[era]["MuonIDSFDown"]          = "CMS_eff_m_id_"+era+"Down"
  SystNameMap[era]["MuonTriggerSFDown"]     = "CMS_eff_m_trigger_"+era+"Down"
  SystNameMap[era]["ElectronEnDown"]        = "CMS_scale_e_"+era+"Down"
  SystNameMap[era]["ElectronResDown"]       = "CMS_res_e_"+era+"Down"
  SystNameMap[era]["ElectronRecoSFDown"]    = "CMS_eff_e_reco_"+era+"Down"
  SystNameMap[era]["ElectronIDSFDown"]      = "CMS_eff_e_id_"+era+"Down"
  SystNameMap[era]["ElectronTriggerSFDown"] = "CMS_eff_e_trigger_"+era+"Down"
  SystNameMap[era]["BTagSFHTagCorrDown"]    = "CMS_btag_hf_corrDown"
  SystNameMap[era]["BTagSFHTagUnCorrDown"]  = "CMS_btag_hf_uncorr_"+era+"Down"
  SystNameMap[era]["BTagSFLTagCorrDown"]    = "CMS_btag_lf_corrDown"
  SystNameMap[era]["BTagSFLTagUnCorrDown"]  = "CMS_btag_lf_uncorr_"+era+"Down"
  SystNameMap[era]["METUnclDown"]           = "CMS_scale_met_"+era+"Down"
  SystNameMap[era]["PrefireDown"]           = "CMS_l1_ecal_prefiring_"+era+"Down"
  SystNameMap[era]["PUDown"]                = "CMS_pileup_13TeV"+"Down" # full correlation
  SystNameMap[era]["CFRateDown"]            = "CMS_cf_stat_"+era+"Down"
  SystNameMap[era]["FRDown"]                = "CMS_fake_stat_"+era+"Down"
  SystNameMap[era]["FRHighPtDown"]          = "CMS_fake_highpt_"+era+"Down"
  SystNameMap[era]["PDFDown"]               = "pdf"+"Down" # full correlation
  SystNameMap[era]["ScaleDown"]             = "QCDscale"+"Down" # full correlation
  SystNameMap[era]["JetRes"]            = "CMS_res_j_"+era
  SystNameMap[era]["JetEn"]             = "CMS_scale_j_"+era
  SystNameMap[era]["JetPUID"]           = "CMS_eff_j_PUJET_id_"+era
  SystNameMap[era]["JetPNET"]           = "CMS_eff_j_PNET_"+era
  SystNameMap[era]["MuonEn"]            = "CMS_scale_m_"+era
  SystNameMap[era]["MuonRes"]           = "CMS_res_m_"+era
  SystNameMap[era]["MuonRecoSF"]        = "CMS_eff_m_reco_"+era
  SystNameMap[era]["MuonIDSF"]          = "CMS_eff_m_id_"+era
  SystNameMap[era]["MuonTriggerSF"]     = "CMS_eff_m_trigger_"+era
  SystNameMap[era]["ElectronEn"]        = "CMS_scale_e_"+era
  SystNameMap[era]["ElectronRes"]       = "CMS_res_e_"+era
  SystNameMap[era]["ElectronRecoSF"]    = "CMS_eff_e_reco_"+era
  SystNameMap[era]["ElectronIDSF"]      = "CMS_eff_e_id_"+era
  SystNameMap[era]["ElectronTriggerSF"] = "CMS_eff_e_trigger_"+era
  SystNameMap[era]["BTagSFHTag"]        = "CMS_btag_hf_"+era
  SystNameMap[era]["BTagSFLTag"]        = "CMS_btag_lf_"+era
  SystNameMap[era]["METUncl"]           = "CMS_scale_met_"+era
  SystNameMap[era]["Prefire"]           = "CMS_l1_ecal_prefiring_"+era
  SystNameMap[era]["PU"]                = "CMS_pileup_13TeV" # full correlation
  SystNameMap[era]["CFRate"]            = "CMS_cf_stat_"+era
  SystNameMap[era]["FR"]                = "CMS_fake_stat_"+era
  SystNameMap[era]["FRHighPt"]          = "CMS_fake_highpt_"+era
  SystNameMap[era]["PDF"]               = "pdf"
  SystNameMap[era]["Scale"]             = "QCDscale" # full correlation


## ChargeSplit has been deprecated due to insignificant improvement. Just legacy ##
ChargeSplit = False
if ChargeSplit:
  ChargeSplit = "ChargeSplit"
else:
  ChargeSplit = ""

InputPath = "/data9/Users/jalmond_public/SUS-24-014/"

MergeList = {}
MergeList['RunConv'] = {}
MergeList['RunConv']['Conv_inc']      = ["TG","TTG","WZG","WWG","WGToLNuG","WGToLNuG_MG","WGToLNuG_01J_PtG_130","WGToLNuG_01J_PtG_300","WGToLNuG_01J_PtG_500","WGJJToLNu","ZGToLLG","ZGToLLG_PtG_130","DYJets_MG","DYJets10to50_MG"] #FIXME time to time
MergeList['RunConv']['ZG_norm']       = ["ZGToLLG","ZGToLLG_PtG_130","DYJets_MG","DYJets10to50_MG"]
MergeList['RunConv']['Conv_others']   = [
                                         x for x in MergeList['RunConv']['Conv_inc']
                                         if x not in MergeList['RunConv']['ZG_norm']
                                        ]
MergeList['RunPrompt'] = {}
MergeList['RunPrompt']['Prompt_inc'] = [
                                        #VVV
                                        'WWW','WWZ','WZZ','ZZZ',
                                        #SingleTop : 0.1 level events
                                        #'SingleTop_sch_Lep','SingleTop_tch_antitop_Incl','SingleTop_tch_top_Incl','SingleTop_tW_antitop_NoFullyHad','SingleTop_tW_top_NoFullyHad',
                                        #ttV
                                        'ttWToLNu','ttZToLLNuNu', #'ttZToQQ_ll', 'ttWToQQ' : no entry
                                        #TTXX
                                        'TTTT','TTZZ',
                                        #tZq
                                        'tZq',
                                        #Higgs
                                        'ttHToNonbb','VHToNonbb',
                                        #VBFHiggs
                                        'VBF_HToZZTo4L', #'VBFHToTauTau_M125', 'VBFHToWWTo2L2Nu', : no entry
                                        #ggH
                                        'GluGluHToZZTo4L', #'GluGluHToTauTau_M125', 'GluGluHToWWTo2L2Nu', : no entry
                                        #minor WWs
                                        'WWTo2L2Nu_DS',
                                        #WW
                                        'WpWp_QCD','WpWp_EWK',
                                        #ZZ
                                        'ZZTo4L_powheg','GluGluToZZto4e','GluGluToZZto4mu','GluGluToZZto2e2mu',
                                        #WZ
                                        'WZTo3LNu_mllmin4p0_powheg','WZ_EWK',
                                       ] #FIXME time to time
MergeList['RunPrompt']['ZZ_norm']       = ["ZZTo4L_powheg","GluGluToZZto4e","GluGluToZZto4mu","GluGluToZZto2e2mu"] #FIXME time to time
MergeList['RunPrompt']['WZ_norm']       = ["WZTo3LNu_mllmin4p0_powheg","WZ_EWK"] #FIXME time to time
MergeList['RunPrompt']['WW_norm']       = ["WpWp_QCD","WpWp_EWK"] #FIXME time to time
MergeList['RunPrompt']['Prompt_others'] = [
                                           x for x in MergeList['RunPrompt']['Prompt_inc']
                                           if x not in MergeList['RunPrompt']['ZZ_norm']
                                           and x not in MergeList['RunPrompt']['WZ_norm']
                                           and x not in MergeList['RunPrompt']['WW_norm']
                                          ]

if args.CheckFiles:
  ##### Input file check #####
  DataList   = {}
  DataList['2016preVFP'] = []
  DataList['2016postVFP'] = []
  DataList['2017'] = []
  DataList['2018'] = []
  Streams = ["DoubleEG","DoubleMuon","MuonEG"]
  Streams_2018 = ["EGamma_GT36","DoubleMuon_GT36","MuonEG_GT36"] if "ANv5" in inputTag else ["EGamma","DoubleMuon","MuonEG"]
  for stream in Streams:
    for period in ["B_ver2","C","D","E","F"]:
      DataList['2016preVFP'].append(stream+"_"+period)
    for period in ["F","G","H"]:
      DataList['2016postVFP'].append(stream+"_"+period)
    for period in ["B","C","D","E","F"]:
      DataList['2017'].append(stream+"_"+period)
  for stream in Streams_2018:
    for period in ["A","B","C","D",]:
      DataList['2018'].append(stream+"_"+period)
  ConvList = MergeList['RunConv']['Conv_inc'][:]
  PromptList = MergeList['RunPrompt']['Prompt_inc'][:]
  DefFlags = ["","MultiLepton__"]
  ConvSkim = {}
  for DefFlag in DefFlags:
    ConvSkim[DefFlag] = {}
    for this_conv in ["TG","TTG","WZG","WWG","ZGToLLG","ZGToLLG_PtG_130","DYJets_MG","DYJets10to50_MG"]:
      ConvSkim[DefFlag][this_conv] = "_SkimTree_HNMultiLepBDT_"
    for this_conv in ["WGToLNuG","WGToLNuG_MG","WGToLNuG_01J_PtG_130","WGToLNuG_01J_PtG_300","WGToLNuG_01J_PtG_500","WGJJToLNu"]:
      ConvSkim[DefFlag][this_conv] = "_SkimTree_DileptonBDT_"
  DefFlags_CR = ["MultiLepton__"]
  SRPath = "/data9/Users/jalmond_public/SUS-24-014/HNL_SignalRegion_Plotter_"+inputTag
  CRPath = "/data9/Users/jalmond_public/SUS-24-014/HNL_ControlRegion_Plotter_"+inputTag

  for era in args.eras:
    if not args.CR:
      # SR
      #for this_proc in DataList[era]:
      #  this_path=SRPath + "/" + era + "/" + PreFlag+PostFlag+"/DATA/HNL_SignalRegion_Plotter_HNMultiLepBDT_"+this_proc+".root"
      #  if not os.path.exists(this_path):
      #    print this_path,"-->",os.path.exists(this_path) # these are data
      for this_proc in DataList[era]:
        this_path=SRPath + "/" + era + "/" + PreFlag+"RunFake__"+PostFlag+"/DATA/HNL_SignalRegion_Plotter_SkimTree_HNMultiLepBDT_"+this_proc+".root"
        if not os.path.exists(this_path):
          print(this_path,"-->",os.path.exists(this_path))
      for this_proc in DataList[era]:
        if "Muon" in this_proc: continue
        this_path=SRPath + "/" + era + "/" + PreFlag+"RunCF__"+PostFlag+"/DATA/HNL_SignalRegion_Plotter_SkimTree_DileptonBDT_"+this_proc+".root"
        if not os.path.exists(this_path):
          print(this_path,"-->",os.path.exists(this_path))
      for this_proc in ConvList:
        this_path=SRPath + "/" + era + "/" + PreFlag+"RunConv__"+PostFlag+"/HNL_SignalRegion_Plotter"+ConvSkim[""][this_proc]+this_proc+".root"
        if not os.path.exists(this_path):
          print(this_path,"-->",os.path.exists(this_path))
      for this_proc in PromptList:
        this_path=SRPath + "/" + era + "/" + PreFlag+"RunPrompt__"+PostFlag+"/HNL_SignalRegion_Plotter_SkimTree_HNMultiLepBDT_"+this_proc+".root"
        if not os.path.exists(this_path):
          print(this_path,"-->",os.path.exists(this_path))
    else:
      # CR
      for this_proc in DataList[era]:
        for DefFlag in DefFlags_CR:
          this_path=CRPath + "/" + era + "/" + PreFlag+DefFlag+PostFlag+"/DATA/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_"+this_proc+".root"
          if not os.path.exists(this_path):
            print(this_path,"-->",os.path.exists(this_path))
      for this_proc in DataList[era]:
        for DefFlag in DefFlags_CR:
          this_path=CRPath + "/" + era + "/" + PreFlag+DefFlag+"RunFake__"+PostFlag+"/DATA/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_"+this_proc+".root"
          if not os.path.exists(this_path):
            print(this_path,"-->",os.path.exists(this_path))
      for this_proc in DataList[era]:
        if "Muon" in this_proc: continue
        for DefFlag in DefFlags_CR:
          this_path=CRPath + "/" + era + "/" + PreFlag+DefFlag+"RunCF__"+PostFlag+"/DATA/HNL_ControlRegion_Plotter_SkimTree_DileptonBDT_"+this_proc+".root"
          if not os.path.exists(this_path):
            print(this_path,"-->",os.path.exists(this_path))
      for this_proc in ConvList:
        for DefFlag in DefFlags_CR:
          this_path=CRPath + "/" + era + "/" + PreFlag+DefFlag+"RunConv__"+PostFlag+"/HNL_ControlRegion_Plotter"+ConvSkim[DefFlag][this_proc]+this_proc+".root"
          if not os.path.exists(this_path):
            print(this_path,"-->",os.path.exists(this_path))
      for this_proc in PromptList:
        for DefFlag in DefFlags_CR:
          this_path=CRPath + "/" + era + "/" + PreFlag+DefFlag+"RunPrompt__"+PostFlag+"/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_"+this_proc+".root"
          if not os.path.exists(this_path):
            print(this_path,"-->",os.path.exists(this_path))

  exit()

##### Start merging #####
if MergeData:

  if Blinded:
    print("[MergeData] Data blinded. skipping...")
  else:
    print("[MergeData] Data unblinded. merging...")
    for era in args.eras:
      if era=="Run2":
        for DefFlag in DefFlags:
          os.system("mkdir -p "+InputPath+"/Run2/"+PreFlag+DefFlag+PostFlag+"/DATA/")
          OutFile=InputPath + "/Run2/" + PreFlag+DefFlag +PostFlag+ "/DATA/"+Analyzer+DataSkim+"DATA.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + InputPath + "/2016preVFP/" + PreFlag+DefFlag +PostFlag+"/DATA/*DATA.root"\
                                      + " " + InputPath + "/2016postVFP/" + PreFlag+DefFlag +PostFlag+ "/DATA/*DATA.root"\
                                      + " " + InputPath + "/2017/" + PreFlag+DefFlag +PostFlag+ "/DATA/*DATA.root"\
                                      + " " + InputPath + "/2018/" + PreFlag+DefFlag +PostFlag+ "/DATA/*DATA.root"\
                   )
      else:
        for DefFlag in DefFlags:
          os.system("mkdir -p "+InputPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag +PostFlag+"/DATA/")
          OutFile=InputPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag +PostFlag+ "/DATA/"+Analyzer+DataSkim+"DATA.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + InputPath + "/"+ Analyzer+"_"+inputTag+ "/"+era+"/" + PreFlag+DefFlag +PostFlag+ "/DATA/*")

if MergeFake:

  for era in args.eras:
    if era=="Run2":
      for DefFlag in DefFlags:
        os.system("mkdir -p "+InputPath+"/Run2/"+PreFlag+DefFlag+"RunFake__"+PostFlag+"/DATA/")
        OutFile=InputPath + "/Run2/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/"+Analyzer+FakeSkim+"Fake.root"
        if os.path.exists(OutFile):
          os.system("rm " + OutFile)
        os.system("hadd " + OutFile + " " + InputPath + "/2016preVFP/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*Fake.root"\
                                    + " " + InputPath + "/2016postVFP/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*Fake.root"\
                                    + " " + InputPath + "/2017/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*Fake.root"\
                                    + " " + InputPath + "/2018/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*Fake.root"\
                 )
    else:
      for DefFlag in DefFlags:
        os.system("mkdir -p "+InputPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/")
        OutFile=InputPath + "/MergedFiles/" +Analyzer+"_"+inputTag + "/" + era + "/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/"+Analyzer+FakeSkim+"Fake.root"
        if os.path.exists(OutFile):
          os.system("rm " + OutFile)
        os.system("hadd " + OutFile + " " + InputPath + "/"+ Analyzer+"_"+inputTag + "/" + era+"/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*")

if MergeCF:

  for era in args.eras:
    if era=="Run2":
      for DefFlag in DefFlags:
        os.system("mkdir -p "+InputPath+"/Run2/"+PreFlag+DefFlag+"RunCF__"+PostFlag+"/DATA/")
        OutFile=InputPath + "/Run2/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/"+Analyzer+CFSkim+"CF.root"
        if os.path.exists(OutFile):
          os.system("rm " + OutFile)
        os.system("hadd " + OutFile + " " + InputPath + "/2016preVFP/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*CF.root"\
                                    + " " + InputPath + "/2016postVFP/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*CF.root"\
                                    + " " + InputPath + "/2017/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*CF.root"\
                                    + " " + InputPath + "/2018/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*CF.root"\
                 )
    else:
      for DefFlag in DefFlags:
        os.system("mkdir -p "+InputPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/")
        OutFile=InputPath + "/MergedFiles/" + Analyzer+"_"+inputTag + "/"+ era + "/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/"+Analyzer+CFSkim+"CF.root"
        if os.path.exists(OutFile):
          os.system("rm " + OutFile)
        os.system("hadd " + OutFile + " " + InputPath + "/"+ Analyzer+"_"+inputTag+ "/"+ era+"/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*") 

if MergeConv:

  for era in args.eras:
    if era=="Run2":
      for DefFlag in DefFlags:
        os.system("mkdir -p "+InputPath+"/Run2/"+PreFlag+DefFlag+"RunConv__"+PostFlag+"/")
        for OutProc in list(MergeList['RunConv'].keys()):
          OutFile=InputPath + "/Run2/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + ' '.join([OutFile.split("_"+OutProc+".root")[0].replace("/Run2/","/2016preVFP/")+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])\
                                         + " " + ' '.join([OutFile.split("_"+OutProc+".root")[0].replace("/Run2/","/2016postVFP/")+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])\
                                         + " " + ' '.join([OutFile.split("_"+OutProc+".root")[0].replace("/Run2/","/2017/")+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])\
                                         + " " + ' '.join([OutFile.split("_"+OutProc+".root")[0].replace("/Run2/","/2018/")+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])\
                   )
    else:
      for DefFlag in DefFlags:
        os.system("mkdir -p "+InputPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunConv__"+PostFlag)
        for OutProc in list(MergeList['RunConv'].keys()):
          OutFile=InputPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + ' '.join([InputPath + "/"+ Analyzer+"_"+inputTag+ "/" +era+"/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]]))

if MergeMC:

  for era in args.eras:
    if era=="Run2":
      for DefFlag in DefFlags:
        os.system("mkdir -p "+InputPath+"/Run2/"+PreFlag+DefFlag+"RunPrompt__"+PostFlag+"/")
        for OutProc in list(MergeList['RunPrompt'].keys()):
          OutFile=InputPath + "/" + era + "/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+MCSkim+OutProc+".root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2016preVFP/")+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])\
                                         + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2016postVFP/")+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])\
                                         + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2017/")+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])\
                                         + " " + ' '.join([OutFile.split(OutProc+".root")[0].replace("/Run2/","/2018/")+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])\
                   )
    else:
      for DefFlag in DefFlags:
        os.system("mkdir -p "+InputPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag)
        for OutProc in list(MergeList['RunPrompt'].keys()):
          OutFile=InputPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+MCSkim+OutProc+".root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + ' '.join([InputPath + "/"+ Analyzer+"_"+inputTag+ "/" +era+"/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+MCSkim+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]]))

if MergeSignal:

  if args.CR:
    print("##### This is CR setting.")
    print("##### Skipping signal merging ...")
    pass
  else:
    for era in args.eras:
      for mass in args.masses:
        if era=="Run2":
          os.system("mkdir -p "+InputPath+"/Run2/"+PostFlag)
          OutFileDY    = InputPath + "/Run2/"+PostFlag+"/"+Analyzer+"_signalDY_"+mass+".root"
          OutFileVBF   = InputPath + "/Run2/"+PostFlag+"/"+Analyzer+"_signalVBF_"+mass+".root"
          OutFileDYVBF = InputPath + "/Run2/"+PostFlag+"/"+Analyzer+"_signalDYVBF_"+mass+".root"
          OutFileSSWW  = InputPath + "/Run2/"+PostFlag+"/"+Analyzer+"_signalSSWW_"+mass+".root"
          # First, create DY, VBF, SSWWTypeI seperately
          if os.system("hadd -f " + OutFileDY + " " + InputPath+"/2016preVFP/"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                              + " " + InputPath+"/2016postVFP/"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                              + " " + InputPath+"/2017/"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                              + " " + InputPath+"/2018/"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                     ) != 0:
            os.system("rm " + OutFileDY) # remove the output if there is any unmatched process
          if os.system("hadd -f " + OutFileVBF + " " + InputPath+"/2016preVFP/"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                               + " " + InputPath+"/2016postVFP/"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                               + " " + InputPath+"/2017/"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                               + " " + InputPath+"/2018/"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                     ) != 0:
            os.system("rm " + OutFileVBF) # remove the output if there is any unmatched process
          if os.system("hadd -f " + OutFileSSWW + " " + InputPath+"/2016preVFP/"+PostFlag+"/*SSWWTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2016postVFP/"+PostFlag+"/*SSWWTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2017/"+PostFlag+"/*SSWWTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2018/"+PostFlag+"/*SSWWTypeI*"+mass+"_private.root"\
                     ) != 0:
            os.system("rm " + OutFileSSWW) # remove the output if there is any unmatched process
          # Now treat DYVBF depending on the mass
          if int(mass.replace("M","")) < 300: # DY only
            os.system("hadd -f " + OutFileDYVBF + " " + InputPath+"/2016preVFP/"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2016postVFP/"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2017/"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2018/"+PostFlag+"/*DYTypeI*"+mass+"_private.root")
          else: # DY+VBF
            os.system("hadd -f " + OutFileDYVBF + " " + InputPath+"/2016preVFP/"+PostFlag+"/*DYTypeI*"+mass+"_private.root" + " " + InputPath+"/2016preVFP/"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2016postVFP/"+PostFlag+"/*DYTypeI*"+mass+"_private.root" + " " + InputPath+"/2016postVFP/"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2017/"+PostFlag+"/*DYTypeI*"+mass+"_private.root" + " " + InputPath+"/2017/"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                                + " " + InputPath+"/2018/"+PostFlag+"/*DYTypeI*"+mass+"_private.root" + " " + InputPath+"/2018/"+PostFlag+"/*VBFTypeI*"+mass+"_private.root")
        else:
          for DefFlag in DefFlags:
            os.system("mkdir -p "+InputPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + PostFlag)
            OutFileDY    = InputPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+DefFlag+PostFlag+"/"+Analyzer+"_signalDY_"+mass+".root"
            OutFileVBF   = InputPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+DefFlag+PostFlag+"/"+Analyzer+"_signalVBF_"+mass+".root"
            OutFileDYVBF = InputPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+DefFlag+PostFlag+"/"+Analyzer+"_signalDYVBF_"+mass+".root"
            OutFileSSWW  = InputPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+DefFlag+PostFlag+"/"+Analyzer+"_signalSSWW_"+mass+".root"
            # First, create DY, VBF, SSWWTypeI seperately
            os.system("cp " + InputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+PostFlag+"/*DYTypeI*"+mass+"_private.root " + OutFileDY)
            os.system("cp " + InputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+PostFlag+"/*VBFTypeI*"+mass+"_private.root " + OutFileVBF)
            if 500 <= int(mass.replace("M","")) and int(mass.replace("M","")) <= 30000: # SSWW
              os.system("hadd -f " + OutFileSSWW + " " + InputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+PostFlag+"/*SSWWTypeI*"+mass+"_private.root")
            elif int(mass.replace("M","")) > 30000: # SSWW EMu
              os.system("cp " + InputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+PostFlag+"/*SSWWTypeI*"+mass+"_private.root " + OutFileSSWW)
            # Now treat DYVBF depending on the mass
            if int(mass.replace("M","")) < 300: # DY only
              os.system("cp " + InputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+PostFlag+"/*DYTypeI*"+mass+"_private.root " + OutFileDYVBF)
            elif int(mass.replace("M","")) <= 3000: # DY+VBF
              os.system("hadd -f " + OutFileDYVBF + " " + InputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+PostFlag+"/*DYTypeI*"+mass+"_private.root" + " " + InputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+PostFlag+"/*VBFTypeI*"+mass+"_private.root")



##### Useful functions #####
def FillScan(outScan, inScan, procName):
  #print "[FillScan] Initiate",procName,"..."
  #print "[FillScan] Currently",outScan.GetNbinsY(),"soures are contained."
  FillBin = 0
  for i in range(outScan.GetNbinsY()+5):
    #print type(outScan.GetYaxis().GetBinLabel(i+1))
    if procName == outScan.GetYaxis().GetBinLabel(i+1):
      print("[FillScan] procName duplicated:",procName)
      print("[FillScan] Please check. skipping...")
      return
    if outScan.GetYaxis().GetBinLabel(i+1) == "":
      FillBin = i+1
      #print "[FillScan] FillBin =",FillBin
      outScan.GetYaxis().SetBinLabel(FillBin, procName)
      break

  try:
    inScan.GetNbinsX()
  except AttributeError:
    print("[FillScan] There is no hist named",procName)
    print("[FillScan] Filling zeros...")
    for j in range(outScan.GetNbinsX()):
      outScan.SetBinContent(j+1,FillBin,0)
  else:
    for j in range(outScan.GetNbinsX()):
      outScan.SetBinContent(j+1,FillBin,inScan.GetBinContent(j+1))
  
  ### Sanity check ###
  #print "Label of ybin:",outScan.GetYaxis().GetBinLabel(FillBin)
  #print "Contents :",outScan.Integral(0,outScan.GetNbinsX(),FillBin,FillBin)
  #print "Original contents :",inScan.Integral()
  #print "[FillScan] Done."
  #print "[FillScan] Now",outScan.GetNbinsY(),"soures are contained."

  return

def CheckFile(f_path):

  f_root = None

  try:
    print("opening",f_path,"...")
    f_root = TFile.Open(f_path)
  except OSError:
    print("[!!WARNING!!] There is no file "+f_path+" .")
    print("Skipping...")
    return None
  else:
    print("Good!")
    return f_root

def CheckHist(f_root,h_path,hist_name):

  this_hist = None

  try:
    print("getting",h_path,"from",f_root,"...")
    this_hist = f_root.Get(h_path)
    this_hist.GetEntries()
  except AttributeError:
    print("[!!WARNING!!] There is no hist named",h_path,"in",f_root,".")
    print("Skipping "+hist_name+"...")
    return None
  else:
    print("Good!")
    return this_hist

##### Main job starts #####
for tag in args.histTag:
  for era in args.eras:
    for region in regions: # ...and even each region to control!!
      print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",region,"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
      OutputPath = '/data9/Users/jalmond_public/SUS-24-014/LimitInputs/'+inputTag+"_"+tag+outputTagSuffix+'/'
      os.system('mkdir -p '+OutputPath + era + '/' + region)
  
      f_path_data          = InputPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region] + PostFlag + "/DATA/"+Analyzer+DataSkim+"DATA.root"
      f_path_fake          = InputPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region] + "RunFake__"+PostFlag+"/DATA/"+Analyzer+FakeSkim+"Fake.root"
      f_path_cf            = InputPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region] + "RunCF__"+PostFlag+"/DATA/"+Analyzer+CFSkim+"CF.root"
      f_path_zg            = InputPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region] + "RunConv__"+PostFlag+"/"+Analyzer+"_ZG_norm.root"
      f_path_conv_others   = InputPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region] + "RunConv__"+PostFlag+"/"+Analyzer+"_Conv_others.root"
      f_path_conv_inc      = InputPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region] + "RunConv__"+PostFlag+"/"+Analyzer+"_Conv_inc.root"
      f_path_wz            = InputPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region] + "RunPrompt__"+PostFlag+"/"+Analyzer+MCSkim+"WZ_norm.root"
      f_path_zz            = InputPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region] + "RunPrompt__"+PostFlag+"/"+Analyzer+MCSkim+"ZZ_norm.root"
      f_path_ww            = InputPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region] + "RunPrompt__"+PostFlag+"/"+Analyzer+MCSkim+"WW_norm.root"
      f_path_prompt_others = InputPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region] + "RunPrompt__"+PostFlag+"/"+Analyzer+MCSkim+"Prompt_others.root"
      f_path_prompt_inc    = InputPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region] + "RunPrompt__"+PostFlag+"/"+Analyzer+MCSkim+"Prompt_inc.root"
      
      if not Blinded: f_data = TFile.Open(f_path_data)
      f_fake          = TFile.Open(f_path_fake)
      f_cf            = TFile.Open(f_path_cf)
      f_zg            = TFile.Open(f_path_zg)
      f_conv_others   = TFile.Open(f_path_conv_others)
      f_conv_inc      = TFile.Open(f_path_conv_inc)
      f_wz            = TFile.Open(f_path_wz)
      f_zz            = TFile.Open(f_path_zz)
      f_ww            = TFile.Open(f_path_ww)
      f_prompt_others = TFile.Open(f_path_prompt_others)
      f_prompt_inc    = TFile.Open(f_path_prompt_inc)

      for mass in args.masses: # iterate for each mass ...
        if ("r1" in region or "r2" in region) and (int(mass.replace("M","")) <= 100):
          continue # NOTE use only SR3 below M100

        for channel in args.channels: # ...and each channel
          if "EMu" not in channel and (int(mass.replace("M","")) > 30000):
            continue # NOTE Only EMu extends above M30000

          if ("sr3" in region) and (int(mass.replace("M","")) <= 500):
            LimitDir = "LimitExtractionBDT"
            InputHistMass = mass+"/"
            if not 'BDT' in RegionToHistSuffixMap[region][channel]:
              RegionToHistSuffixMap[region][channel] += 'BDT'
            else: pass
          else:
            LimitDir = "LimitExtraction"
            InputHistMass = ""
            RegionToHistSuffixMap[region][channel] = RegionToHistSuffixMap[region][channel].replace('BDT','')

          # Set channel dependent scaler first
          DYVBFscaler = 0.01 # Set the signalDYVBF scaler
          if int(mass.replace("M","")) > 3000: DYVBFscaler = 0.1 # relax the scale for SSWW impact
          SSWWscaler = DYVBFscaler*DYVBFscaler # Set the signalSSWW scaler

          #if int(mass.replace("M","")) <= 100: DYVBFscaler = 0.001 # if you want to use HybridNew without additional options, see https://cms-talk.web.cern.ch/t/too-large-error-with-hybridnew/32844

          # Mass dependent scaler, if necessary ...
          #if int(mass.replace("M","")) <= 500: DYVBFscaler = 0.01
          #elif int(mass.replace("M","")) <= 3000: DYVBFscaler = 0.1
          ##else: DYVBFscaler = 1.
          #else: DYVBFscaler = 0.3
          #if "EMu" in channel:
          #  SSWWscaler = 4.*DYVBFscaler*DYVBFscaler # Set the EMu signalSSWW scaler
          #else:
          #  SSWWscaler = DYVBFscaler*DYVBFscaler # Set the signalSSWW scaler

          print("f_cf :",f_path_cf)
          print("f_prompt_inc:",f_prompt_inc)
          print("input_hist :", LimitDir+"/"+tag+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel])
          input_hist = LimitDir+"/"+tag+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel]
          
          print("##### Initiating",region,mass,channel,"...")
          if not Blinded: h_data        = f_data.Get(input_hist)
          h_fake          = f_fake.Get(input_hist)
          h_cf            = f_cf.Get(input_hist) if "Mu" not in channel else ""
          h_zg            = f_zg.Get(input_hist)
          h_conv_others   = f_conv_others.Get(input_hist)
          h_conv_inc      = f_conv_inc.Get(input_hist)
          h_wz            = f_wz.Get(input_hist)
          h_zz            = f_zz.Get(input_hist)
          h_ww            = f_ww.Get(input_hist)
          h_prompt_others = f_prompt_others.Get(input_hist)
          h_prompt_inc    = f_prompt_inc.Get(input_hist)
          print("##### histo done.")
 
          # Make list of [file path, histogram, histo name]
          input_list = [
                        [f_path_fake, h_fake, "fake"],
                        [f_path_cf, h_cf, "cf"],
                        [f_path_zg, h_zg, "zg"],
                        [f_path_conv_others, h_conv_others, "conv_others"],
                        [f_path_conv_inc, h_conv_inc, "conv_inc"],
                        [f_path_wz, h_wz, "wz"],
                        [f_path_zz, h_zz, "zz"],
                        [f_path_ww, h_ww, "ww"],
                        [f_path_prompt_others, h_prompt_others, "prompt_others"],
                        [f_path_prompt_inc, h_prompt_inc, "prompt_inc"],
                       ]

          #### Treat 0 fakes: see v) of https://hypernews.cern.ch/HyperNews/CMS/get/EXO-21-002/25
          try:
            for i in range(h_fake.GetNbinsX()):
              if h_fake.GetBinContent(i+1) <= 0.:
                print("!!!!!! zero fakes detected in",f_path_fake,input_hist,"!!!!!!")
                print("!!!!!! bin",i+1,":",h_fake.GetBinContent(i+1),"!!!!!!")
                h_fake.SetBinContent(i+1,0.15*0.645)
                h_fake.SetBinError(i+1,0.15*0.645)
          except AttributeError:
            print("[!!WARNING!!] There is no hist named "+input_hist+" in "+f_path_fake+" .")
            print("Skipping treatment on zero fakes...")
            continue

          if "Mu" in channel:
            print("This is",channel,"channel --> Remove CF item:")
            print(input_list.pop(1))

          if args.Scan:
            print("##### Scan initiated. #####")
            h_scan = TH2D("Nominal","Nominal",h_fake.GetNbinsX(),0,h_fake.GetNbinsX(),len(input_list)+2,0,len(input_list)+2) # There is no automatic merging from many TH1s... see https://root-forum.cern.ch/t/filling-a-th2-from-two-existing-th1/14575; +2 is to secure space for 2 signals. I was going to extend the axis, but... (below)
            #h_scan.GetYaxis().SetCanExtend(1) # This seems not resolved... https://root-forum.cern.ch/t/extending-axis-for-th1-vs-th2/20964
            print("h_scan for Nominal created; this should be empty:",h_scan.Integral(0,h_fake.GetNbinsX(),1,1))
            if h_scan.Integral(0,h_fake.GetNbinsX(),1,1)!=0.: sys.exit()
            h_scan.SetDirectory(0)
            scan_list = []

            for i in range(len(input_list)):
              print("##### Making 2D hist for",input_list[i][2],"#####")
              FillScan(h_scan,input_list[i][1],input_list[i][2]) # out, in, name
          
          if Blinded:
            print("##### This analysis is blinded.")
            print("##### Creating Asimov data...")
            print("Adding prompt...")
            h_data = h_prompt_inc.Clone()

            bkg_list = [ #bkg except prompt (already added above)
                        [f_path_fake, h_fake, "fake"],
                        [f_path_cf, h_cf, "cf"],
                        [f_path_conv_inc, h_conv_inc, "conv_inc"],
                       ]
          
            if "Mu" in channel:
              print("This is",channel,"channel. --> Remove CF item in bkg list:")
              print(bkg_list.pop(1))

            total_number = 0 # to cross check
            total_number += h_prompt_inc.GetEntries()
          
            for bkg in bkg_list:
              try:
                bkg[1].GetEntries()
              except AttributeError:
                print("[!!WARNING!!] There is no hist named "+input_hist+" in "+bkg[0]+" .")
                print("Skipping "+bkg[2]+"...")
                continue
              print("Adding "+bkg[2]+"...")
              h_data.Add(bkg[1]) # Add each bkg while iterating
              total_number += bkg[1].GetEntries()
          
            #print h_data.GetEntries(), total_number
            if h_data.GetEntries() == total_number: pass #NOTE Only valid when blinded.
            else:
              print("[!!ERROR!!] Cross check failed. Exiting...")
              sys.exit()
          
            input_list.append(["fake_data_path", h_data, "data_obs"]) # fake data = total bkg. There is no hist path of it.
          else:
            input_list.append([f_path_data, h_data, "data_obs"])
          print("##### Data done.")
  
          # Now list has bkg, (pseudo) data. Finally let's add signals
          if args.CR:
            print("##### This is CR setting.")
            print("##### Skipping signal ...")
          else:
            f_path_signalDYVBF = InputPath +"/MergedFiles/"+Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+PostFlag+"/"+Analyzer+"_signalDYVBF_"+mass+".root"
            f_path_signalSSWW  = InputPath +"/MergedFiles/"+Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+PostFlag+"/"+Analyzer+"_signalSSWW_"+mass+".root"
  
            f_signalDYVBF = CheckFile(f_path_signalDYVBF)
            if f_signalDYVBF:
              h_signalDYVBF = CheckHist(f_signalDYVBF,input_hist,"signalDYVBF")
              if h_signalDYVBF:
                h_signalDYVBF.Scale(DYVBFscaler) # Scaling the signal due to Combine fitting
                input_list.append([f_path_signalDYVBF, h_signalDYVBF, "signalDYVBF"])
                print("Scaled signalDYVBF :", h_signalDYVBF.Integral())
              if args.Scan:
                print("##### Making 2D hist for","signalDYVBF","#####")
                FillScan(h_scan,h_signalDYVBF,"signalDYVBF") # out, in, name

            f_signalSSWW = CheckFile(f_path_signalSSWW)
            if f_signalSSWW:
              h_signalSSWW = CheckHist(f_signalSSWW,input_hist,"signalSSWW")
              if h_signalSSWW:
                h_signalSSWW.Scale(SSWWscaler) # Scaling the signal due to Combine fitting
                input_list.append([f_path_signalSSWW, h_signalSSWW, "signalSSWW"])
                print("Scaled signalSSWW :", h_signalSSWW.Integral())
              if args.Scan:
                print("##### Making 2D hist for","signalSSWW","#####")
                FillScan(h_scan,h_signalSSWW,"signalSSWW") # out, in, name

            #print("opening",f_path_signalDYVBF,"...")
            #f_signalDYVBF = TFile.Open(f_path_signalDYVBF)
            #print("opening",f_path_signalSSWW,"...")
            #f_signalSSWW  = TFile.Open(f_path_signalSSWW)

            #try:
            #  h_signalDYVBF = f_signalDYVBF.Get(input_hist)
            #except ReferenceError:
            #  print("[!!WARNING!!] There is no signal file "+f_path_signalDYVBF+" .")
            #  print("Skipping...")
            #  if args.Scan:
            #    print("##### Making 2D hist for","signalDYVBF","#####")
            #    FillScan(h_scan,h_signalDYVBF,"signalDYVBF") # out, in, name
            #else:
            #  if args.Scan:
            #    print("##### Making 2D hist for","signalDYVBF","#####")
            #    FillScan(h_scan,h_signalDYVBF,"signalDYVBF") # out, in, name
            #  try:
            #    h_signalDYVBF.Scale(DYVBFscaler) # Scaling the signal due to Combine fitting
            #  except AttributeError:
            #    print("[!!WARNING!!] There is no hist named "+input_hist+" in "+f_path_signalDYVBF+" .")
            #    print("Skipping...")
            #  else:
            #    input_list.append([f_path_signalDYVBF, h_signalDYVBF, "signalDYVBF"])
            #    print("Scaled signalDYVBF :", h_signalDYVBF.Integral())
  
            #try:
            #  h_signalSSWW  = f_signalSSWW.Get(input_hist)
            #except ReferenceError:
            #  print("[!!WARNING!!] There is no signal file "+f_path_signalSSWW+" .")
            #  print("Skipping...")
            #  if args.Scan:
            #    print("##### Making 2D hist for","signalSSWW","#####")
            #    FillScan(h_scan,h_signalDYVBF,"signalSSWW") # out, in, name
            #else:
            #  if args.Scan:
            #    print("##### Making 2D hist for","signalSSWW","#####")
            #    FillScan(h_scan,h_signalSSWW,"signalSSWW") # out, in, name
            #  try:
            #    h_signalSSWW.Scale(SSWWscaler) # To be consistent when calculating mixing limits
            #  except AttributeError:
            #    print("[!!WARNING!!] There is no hist named "+input_hist+" in "+f_path_signalSSWW+" .")
            #    print("Skipping...")
            #  else:
            #    print("Scaled signalSSWW :", h_signalSSWW.Integral())
            #    input_list.append([f_path_signalSSWW, h_signalSSWW, "signalSSWW"])
  
            if args.Scan:
              scan_list.append(h_scan)

            print("##### Signal done.")
  
  
          if args.Syst:
            print("##### Systematics activated.")
  
            Nproc = len(input_list) # The number of processes = the length of the input list before adding systematics
  
            for i in range(Nproc):
              if args.Scan:
                h_scan = TH2D(input_list[i][2],input_list[i][2],h_fake.GetNbinsX(),0,h_fake.GetNbinsX(),len(SystList),0,len(SystList))
                print("h_scan for",input_list[i][2],"syst created; this should be empty:",h_scan.Integral(0,h_fake.GetNbinsX(),1,1))
                if h_scan.Integral(0,h_fake.GetNbinsX(),1,1)!=0.: sys.exit()
                h_scan.SetDirectory(0)

              #f_syst = TFile.Open(input_list[i][0]) # Get each process's file

              for this_syst in SystList: # Define new input_hist with each syst name

                if not "fake_data_path" in input_list[i][0]: # There is no file like "fake_data_path" so pass this in the iteration

                  input_hist = LimitDir+"/Syst_"+this_syst+tag+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel]

                  this_name_syst = SystNameMap[era][this_syst]
                  if 'PDF' in this_syst or 'Scale' in this_syst:
                    if 'signal' not in input_list[i][2]: continue
                    else:
                      if "DYVBF" in input_list[i][2]:
                        this_name_syst = this_name_syst.replace('pdf','pdf_DYVBF').replace('scale','scale_DYVBF')
                      elif "SSWW" in input_list[i][2]:
                        this_name_syst = this_name_syst.replace('pdf','pdf_SSWW').replace('scale','scale_SSWW')
                  name_syst = input_list[i][2]+"_"+this_name_syst # new output syst hist name

                  if args.Decorr: # Redefine output syst hist name
                    if 'sr1' in region or 'cr1' in region:
                      regionName_Decorr = '_sr1'
                    elif 'sr2' in region or 'cr2' in region:
                      regionName_Decorr = '_sr2'
                    elif 'sr3' in region or 'cr3' in region:
                      regionName_Decorr = '_sr3'
                    else:
                      regionName_Decorr = '_sr3' # correlate zg_cr, zz_cr to SR3 
                      #print("[!!ERROR!!] Region name",region,"does NOT match with --Decorr argument !!")
                      #print("[!!ERROR!!] Exiting ...")
                      #sys.exit() # Some CRs (zg, zz) are now correlated to all SRs altogether

                    DecorrList = ["CFRate","FR","FRHighPt"] if not args.JetDecorr else ["CFRate","FR","FRHighPt","JetRes","JetEn"]
                    this_syst_source = this_syst.replace('Up','').replace('Down','')
                    if this_syst_source in DecorrList: # if this is Fake of CF syst source
                      this_syst_nameSep = SystNameMap[era][this_syst_source]+regionName_Decorr+this_syst.replace(this_syst_source,'') # AJ_sr1Up
                      name_syst = input_list[i][2]+"_"+this_syst_nameSep

                  f_syst = CheckFile(input_list[i][0]) # Get each process's file
                  if f_syst:
                    h_syst = CheckHist(f_syst,input_hist,name_syst)
                    if h_syst:
                      if args.Scan: # Do the scan before any treatment (0 fake)
                        print("##### Making 2D hist for",name_syst,"#####")
                        FillScan(h_scan,h_syst,name_syst) # out, in, name

                      h_syst.SetDirectory(0) # Store h_syst in memory so that it cannot be deleted during the iteration
                      #### Treat 0 fakes: see v) of https://hypernews.cern.ch/HyperNews/CMS/get/EXO-21-002/25
                      if input_list[i][2] == "fake" and "FR" in this_syst and "CF" not in this_syst:
                        for j in range(h_syst.GetNbinsX()):
                          if h_syst.GetBinContent(j+1) <= 0.:
                            print("!!!!!! zero fakes detected in ",input_list[i][0],input_hist,"!!!!!!")
                            print("!!!!!! bin",j+1,":",h_syst.GetBinContent(j+1),"!!!!!!")
                            h_syst.SetBinContent(j+1,0.15*0.645)
                            h_syst.SetBinError(j+1,0.15*0.645)
                    else:
                      if args.Scan: # Do the scan before any treatment (empty hist makeup)
                        print("##### Making 2D hist for",name_syst,"#####")
                        FillScan(h_scan,h_syst,name_syst) # out, in, name

                      print("Making an empty hist...") # Sometimes there is no hist with syst variation and Combine complains. This is to makeup this.
                      if args.CR:
                        h_syst = h_prompt_inc.Clone() # CR --> h_prompt_inc: expected to have the largest Nevents so least likely that there is no hist
                      else:
                        h_syst = h_data.Clone() # SR --> h_Data = total Nbkg so least likely that there is no hist
                      for j in range(h_syst.GetNbinsX()):
                        h_syst.SetBinContent(j+1,0)
                        h_syst.SetBinError(j+1,0)
                      h_syst.SetBinContent(1,0.001) # to avoid Combine complaining for empty hist.
                      h_syst.SetBinError(1,0.000001)
                      h_syst.SetDirectory(0) # Store h_syst in memory so that it cannot be deleted during the iteration

                  #try:
                  #  h_syst = f_syst.Get(input_hist)

                  #except ReferenceError:
                  #  print("[!!WARNING!!] There is no file "+input_list[i][0]+" .")
                  #  if args.Scan:
                  #    print("##### Making 2D hist for",name_syst,"#####")
                  #    FillScan(h_scan,h_syst,name_syst) # out, in, name
                  #  if "signal" in input_list[i][2]:
                  #    print("Skipping...")
                  #  else:
                  #    print("Making an empty hist...")
                  #    if args.CR:
                  #      h_syst = h_prompt_inc.Clone() # I could use data here, but data could have no entry due to stats so just use prompt_inc here.
                  #    else:
                  #      h_syst = h_data.Clone() # SR --> data = total bkg, lowest possibility of no stats
                  #    for j in range(h_syst.GetNbinsX()):
                  #      h_syst.SetBinContent(j+1,0)
                  #      h_syst.SetBinError(j+1,0)
                  #    h_syst.SetBinContent(1,0.001) # to avoid Combine complaining for empty hist.
                  #    h_syst.SetBinError(1,0.000001)
                  #    h_syst.SetDirectory(0) # Store h_syst in memory so that it cannot be deleted during the iteration
                  #else:
                  #  if args.Scan:
                  #    print("##### Making 2D hist for",name_syst,"#####")
                  #    FillScan(h_scan,h_syst,name_syst) # out, in, name
                  #  try:
                  #    h_syst.SetDirectory(0) # Store h_syst in memory so that it cannot be deleted during the iteration
                  #    #### Treat 0 fakes: see v) of https://hypernews.cern.ch/HyperNews/CMS/get/EXO-21-002/25
                  #    if input_list[i][2] == "fake" and "FR" in this_syst and "CFRate" not in this_syst:
                  #      for j in range(h_syst.GetNbinsX()):
                  #        if h_syst.GetBinContent(j+1) <= 0.:
                  #          print("!!!!!! zero fakes detected in ",input_list[i][0],input_hist,"!!!!!!")
                  #          print("!!!!!! bin",j+1,":",h_syst.GetBinContent(j+1),"!!!!!!")
                  #          h_syst.SetBinContent(j+1,0.15*0.645)
                  #          h_syst.SetBinError(j+1,0.15*0.645)
                  #  except AttributeError:
                  #    print("[!!WARNING!!] There is no hist named "+input_hist+" in "+input_list[i][0]+" .")
                  #    if "signal" in input_list[i][2]:
                  #      print("Skipping...")
                  #    else:
                  #      print("Making an empty hist...")
                  #      if args.CR:
                  #        h_syst = h_prompt_inc.Clone()
                  #      else:
                  #        h_syst = h_data.Clone()
                  #      for j in range(h_syst.GetNbinsX()):
                  #        h_syst.SetBinContent(j+1,0)
                  #        h_syst.SetBinError(j+1,0)
                  #      h_syst.SetBinContent(1,0.001) # to avoid Combine complaining for empty hist.
                  #      h_syst.SetBinError(1,0.000001)
                  #      h_syst.SetDirectory(0) # Store h_syst in memory so that it cannot be deleted during the iteration

                  # Now h_systs are fully made-up.
                  if "signalDYVBF" in input_list[i][2]: # Scale the syst variated signals
                    h_syst.Scale(DYVBFscaler)
                  elif "signalSSWW" in input_list[i][2]:
                    h_syst.Scale(SSWWscaler)
                  print("Appending "+name_syst+"...")
                  input_list.append([input_list[i][0], h_syst, name_syst]) # Append each syst histogram while iterating
  
              if args.Scan:
                #for i in range(h_scan.GetNbinsX()): print h_scan.GetYaxis().GetBinLabel(3), h_scan.GetBinContent(i+1,3)
                h_scan.SetDirectory(0)
                scan_list.append(h_scan)
                #print scan_list

            print("##### Systematics done.")
  
  
          print("##### Now creating a limit input root file...")
          outName = OutputPath+era+"/"+region+"/"+mass+"_"+channel
          outfile = TFile.Open(outName+"_card_input.root","RECREATE")
          
          outfile.cd() # Move into it
          for item in input_list: # Remember, item = [path,hist,name]
            try:
              item[1].SetName(item[2])
            except AttributeError:
              print("[!!WARNING!!] There is no hist",item[1],"in",region,item[0],".") # Final check
              #print("[!!WARNING!!] There is no hist corresponding to "+item[2]+" in "+region+" "+item[0]+" .") # Final check
              if "signal" in item[2]:
                print("Skipping...")
                continue
              else:
                print("Making an empty hist...")
                if args.CnC:
                   h_missing = TH1D(item[2],item[2],1,0,1)
                else:
                  if args.CR:
                    h_missing = h_prompt_inc.Clone()
                  else:
                    h_missing = h_data.Clone()
                for i in range(h_missing.GetNbinsX()):
                  h_missing.SetBinContent(i+1,0)
                  h_missing.SetBinError(i+1,0)
                h_missing.SetBinContent(1,0.001) # to avoid Combine complaining for empty hist.
                h_missing.SetBinError(1,0.000001)
                h_missing.SetName(item[2])
                print("Writing "+item[2]+"...")
                h_missing.Write() # Write empty histogram
                continue

            if item[1].Integral() <=0 : # treat -ve bins #FIXME Must check bin-by-bin in principle..
              print("[!!WARNING!!] Negative events "+str(item[1].Integral())+" in "+item[2]+" ------------------------------------")
              print("Creating a makeup hist...")
              for i in range(item[1].GetNbinsX()):
                item[1].SetBinContent(i+1,0)
                item[1].SetBinError(i+1,0)
              item[1].SetBinContent(1,0.001) # to avoid Combine complaining for empty hist.
              item[1].SetBinError(1,0.000001)

            if args.CnC:
              print("!!Cut and count option activated!!")
              print("!!Merging all into 1 bin...!!")
              
              item[1].Rebin(item[1].GetNbinsX())
              CnChist = TH1D(item[2],item[2],1,0,1)
              CnChist.SetBinContent(1,item[1].GetBinContent(1))
              CnChist.SetBinError(1,item[1].GetBinError(1))

            print("Writing "+item[2]+"...")
            if args.CnC: CnChist.Write()
            else: item[1].Write() # Write each histogram while iterating
          
          outfile.Close()
          print(outName+"_card_input.root has been created.")

          if args.Scan:
            colors = array.array('i',[632,417,860])
            levels = array.array('d',[-1.e308,-0.00001,0.00001,1.e308]) # Even if I set the color with zero bins, it won't be drawn if the minimum is zero (not negative). See https://root-forum.cern.ch/t/not-plotting-zero-bins-in-th2-with-negative-entries/19633/4
            #print "len(scan_list):",len(scan_list)
            canvas = TCanvas("canvas","canvas",900,1000)
            for i in range(len(scan_list)):
              this_h_scan = scan_list[i]
              this_proc = this_h_scan.GetTitle()
              this_h_scan.SetStats(0)
              this_h_scan.SetContour(3,levels)
              #this_h_scan.GetYaxis().SetLabelSize(0.01)
              this_h_scan.Draw("COLZTEXT")
              this_h_scan.GetZaxis().SetRangeUser(this_h_scan.GetMinimum(),1.)
              gStyle.SetPalette(3,colors)
              gStyle.SetPaintTextFormat("4.1f") # https://root-forum.cern.ch/t/decimal-precision-with-text-drawing-option/15923
              #gPad.SetLogz()
              if i==0: canvas.Print(outName+"_card_scan.pdf(", "Title: "+this_proc)
              elif i==(len(scan_list)-1): canvas.Print(outName+"_card_scan.pdf)", "Title: "+this_proc)
              else: canvas.Print(outName+"_card_scan.pdf", "Title: "+this_proc) # https://root-forum.cern.ch/t/problem-with-saving-multiple-canvases-to-a-single-pdf/57145/3
            print(outName+"_card_scan.pdf has been created.")
