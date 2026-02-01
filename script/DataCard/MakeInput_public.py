# Run first: python MakeInput_public.py -e 2018 --CheckFiles -i HEMJet --PreFlag RemoveHEMJet
# python MakeInput_public.py --Merge -e 2018 -i HEMJet --PreFlag RemoveHEMJet \ python MakeInput_public.py --CR --Merge -e 2018 -i HEMJet --PreFlag RemoveHEMJet \
# python MakeInput_public.py [--Syst] [--Decorr] -i HEMJet --PreFlag RemoveHEMJet \ python MakeInput_public.py --CR [--Syst] [--Decorr] -i HEMJet --PreFlag RemoveHEMJet

import numpy as np
import os, sys
import argparse
import re
from ROOT import *
import array
gROOT.SetBatch(kTRUE)

parser = argparse.ArgumentParser(description='script for creating input root file.',formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-e', dest='eras', choices=['2016preVFP','2016postVFP','2017','2018','Run2'], default=['2016preVFP','2016postVFP','2017','2018'], nargs='+', help='eras to run')
parser.add_argument('-m', dest='masses', nargs='+', help='signal masses to run')
parser.add_argument('-c', dest='channels', nargs='+', default=["MuMu","EE","EMu"], help='lepton channels to run')
parser.add_argument('-i', dest='inputTag', default='', help='tag attached to the input SKFlatOutput files')
parser.add_argument('-o', dest='outputTag', default='', help='tag attached to the output files')
parser.add_argument('-x', dest='exceptionTag', default='', help='tag attached to the exception rules')
parser.add_argument('-s', dest='saveException', choices=['Print','Write','Add'], default='Print', help='how to save the exception rule')
parser.add_argument('-t', dest='histTag', nargs='+', default=['HNL_ULIDv2'], help='this is the param name of the SKFlatAnalyzer. Mostly IDs.')
parser.add_argument('--Scan', action='store_true', help='scan the bin content')
parser.add_argument('--CnC', action='store_true', help='1bin cut and count setting')
parser.add_argument('--Ext', action='store_true', help='Extend selection by cut down to M500')
parser.add_argument('--CR', action='store_true', help='Make HNL_ControlRegion_Plotter input (default : HNL_SignalRegion_Plotter)')
parser.add_argument('--Syst', action='store_true', help='Add systematics')
parser.add_argument('--Decorr', action='store_true', help='Decorrelate Fake, CF syst sources')
parser.add_argument('--JetDecorr', action='store_true', help='Decorrelate jet syst sources as well')
parser.add_argument('--PreFlag', nargs='+', help='Your private flag names')
parser.add_argument('--PostFlag', nargs='+', help='Your private flag names')
## Merge setting
parser.add_argument('--Merge', action='store_true', help='hadd the needed histograms') # NOTE Run2 Merging deprecated.
parser.add_argument('--Data',   action='store_true', help='merge Data')
parser.add_argument('--Fake',   action='store_true', help='merge Fake')
parser.add_argument('--CF',     action='store_true', help='merge CF')
parser.add_argument('--Conv',   action='store_true', help='merge Conv')
parser.add_argument('--Prompt', action='store_true', help='merge Prompt')
parser.add_argument('--MC',     action='store_true', help='merge MC (Conv+Prompt)')
parser.add_argument('--Signal', action='store_true', help='merge Signal')
##
parser.add_argument('--CheckFiles', action='store_true', help='check all inputs before merge')
parser.add_argument('--BDTver', default=None, help='BDT version comparison')
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
  args.masses = ["M85","M90","M95","M100","M125","M150","M200","M250","M300","M350","M400","M450","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000","M25000","M30000","M40000","M50000","M60000","Weinberg"]
  #args.masses = ["M85","M90","M95","M100","M125","M150","M200","M250","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000","M25000","M30000","M40000","M50000","M60000"]
  #masses = ["M100","M250","M1000","M10000"]
else:
  for i in range(len(args.masses)):
    if "Weinberg" in args.masses[i]: continue
    else: args.masses[i] = "M"+args.masses[i]

if not args.channels:
  print("Please specify the lepton channels; e.g. MuMu .")
  exit()

## region maps ##
RegionToDefFlagMap = {}
RegionToChannelMap = {}
RegionToHistSuffixMap = {}

inputTag = args.inputTag
outputTag = args.outputTag if args.outputTag == '' else "_"+args.outputTag
ExtTag = '_Ext' if args.Ext else ''

BDTver = args.BDTver
ANver = int(re.search(r'\bANv(\d+)(?=_|$)', inputTag).group(1)) # ANv + some number + _ or end of the string

if not args.histTag:
  print("Please specify the hist tag; e.g. HNL_ULIDv2 .")
  exit()

outputTagSuffix = ""
outputTagSuffix += '_'+BDTver if BDTver else ""
outputTagSuffix += '_'+PreFlag.rstrip('_').replace('__','_') if args.PreFlag else ""
outputTagSuffix += '_'+PostFlag.rstrip('_').replace('__','_') if args.PostFlag else ""
if args.CnC:
  outputTagSuffix += '_CnC'
if args.Decorr:
  outputTagSuffix += '_Decorr'
  if args.JetDecorr:
    outputTagSuffix += '_JetDecorr'

# Skim (except ConvSkim, PromptSkim)
DataSkim = "_SkimTree_HNMultiLepBDT_"
FakeSkim = "_SkimTree_HNMultiLepBDT_"
#CFSkim = "_SkimTree_HNMultiLepBDT_" #FIXME MC CF
CFSkim = "_SkimTree_DileptonBDT_" #FIXME Data CF
SignalSkim = "_SkimTree_HNMultiLepBDT_"

# This will do necessary hadd for you.
targets = ['Data','Fake','CF','Conv','Prompt','MC','Signal']
any_target_selected = any(getattr(args, t) for t in targets)

if not args.Merge and any_target_selected:
  parser.error("You used --Data/--Fake/... without --Merge. Add --Merge.")

if args.Merge:
  if any_target_selected:
    Merge = {t: getattr(args, t) for t in targets}
  else:
    Merge = {t: True for t in targets}
else:
  Merge = {t: False for t in targets}

MergeData   = Merge['Data']
MergeFake   = Merge['Fake']
MergeCF     = Merge['CF']
MergeConv   = Merge['Conv']
MergePrompt = Merge['Prompt']
MergeMC     = Merge['MC']
MergeSignal = Merge['Signal']

if args.CR:
  Blinded = False # Blinded --> the total background will be used as data_obs
  DefFlags = ["MultiLepton__"]
  Analyzer = "HNL_ControlRegion_Plotter"

  #regions = ["cr1_inv","cr2_inv","cr3_inv","cf_cr1","cf_cr2","cf_cr3","ww_cr1","ww_cr2","zg_cr3","wz_cr1","wz_cr2","wz_cr3","zz_cr1","zz_cr2","zz_cr3"] if not args.Merge else "" # for CRs
  regions = ["cr1_InvMET","cr2_InvMET","cr3_InvMET","cr1_InvBJet","cr2_InvBJet","cr3_InvBJet","zg_cr","wz_cr1","wz_cr2","wz_cr3","zz_cr"] if not args.Merge else "" # for CRs
  #regions = ["zg_cr","zz_cr"] if not args.Merge else "" # for CRs

  RegionToDefFlagMap['cr_inv']     = "MultiLepton__"
  RegionToDefFlagMap['cr1_inv']    = "MultiLepton__"
  RegionToDefFlagMap['cr2_inv']    = "MultiLepton__"
  RegionToDefFlagMap['cr3_inv']    = "MultiLepton__"
  RegionToDefFlagMap['cr1_InvMET'] = "MultiLepton__"
  RegionToDefFlagMap['cr2_InvMET'] = "MultiLepton__"
  RegionToDefFlagMap['cr3_InvMET'] = "MultiLepton__"
  RegionToDefFlagMap['cr1_InvBJet'] = "MultiLepton__"
  RegionToDefFlagMap['cr2_InvBJet'] = "MultiLepton__"
  RegionToDefFlagMap['cr3_InvBJet'] = "MultiLepton__"
  RegionToDefFlagMap['zg_cr']      = "MultiLepton__"
  RegionToDefFlagMap['wz_cr']      = "MultiLepton__"
  RegionToDefFlagMap['wz_cr1']     = "MultiLepton__"
  RegionToDefFlagMap['wz_cr2']     = "MultiLepton__"
  RegionToDefFlagMap['wz_cr3']     = "MultiLepton__"
  RegionToDefFlagMap['zz_cr']      = "MultiLepton__"

  RegionToChannelMap['cr_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['cr1_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['cr2_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['cr3_inv'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['cr1_InvMET'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['cr2_InvMET'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['cr3_InvMET'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['cr1_InvBJet'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['cr2_InvBJet'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['cr3_InvBJet'] = {'MuMu':'MuMu', 'EE':'EE', 'EMu':'EMu'}
  RegionToChannelMap['zg_cr']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr1']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr2']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['wz_cr3']  = {'MuMu':'MuMuMu', 'EE':'EEE', 'EMu':'EMuL'}
  RegionToChannelMap['zz_cr']  = {'MuMu':'MuMuMuMu', 'EE':'EEEE', 'EMu':'EMuLL'}

  RegionToHistSuffixMap['cr_inv']  = {'MuMu':'LimitBins/MuonCR',  'EE':'LimitBins/ElectronCR',  'EMu':'LimitBins/ElectronMuonCR'}
  RegionToHistSuffixMap['cr1_inv'] = {'MuMu':'LimitBins/MuonCR1', 'EE':'LimitBins/ElectronCR1', 'EMu':'LimitBins/ElectronMuonCR1'}
  RegionToHistSuffixMap['cr2_inv'] = {'MuMu':'LimitBins/MuonCR2', 'EE':'LimitBins/ElectronCR2', 'EMu':'LimitBins/ElectronMuonCR2'}
  RegionToHistSuffixMap['cr3_inv'] = {'MuMu':'LimitBins/MuonCR3', 'EE':'LimitBins/ElectronCR3', 'EMu':'LimitBins/ElectronMuonCR3'}
  RegionToHistSuffixMap['cr1_InvMET'] = {'MuMu':'LimitBins/MuonInvMETCR1', 'EE':'LimitBins/ElectronInvMETCR1', 'EMu':'LimitBins/ElectronMuonInvMETCR1'}
  RegionToHistSuffixMap['cr2_InvMET'] = {'MuMu':'LimitBins/MuonInvMETCR2', 'EE':'LimitBins/ElectronInvMETCR2', 'EMu':'LimitBins/ElectronMuonInvMETCR2'}
  RegionToHistSuffixMap['cr3_InvMET'] = {'MuMu':'LimitBins/MuonInvMETCR3', 'EE':'LimitBins/ElectronInvMETCR3', 'EMu':'LimitBins/ElectronMuonInvMETCR3'}
  RegionToHistSuffixMap['cr1_InvBJet'] = {'MuMu':'LimitBins/MuonInvBJetCR1', 'EE':'LimitBins/ElectronInvBJetCR1', 'EMu':'LimitBins/ElectronMuonInvBJetCR1'}
  RegionToHistSuffixMap['cr2_InvBJet'] = {'MuMu':'LimitBins/MuonInvBJetCR2', 'EE':'LimitBins/ElectronInvBJetCR2', 'EMu':'LimitBins/ElectronMuonInvBJetCR2'}
  RegionToHistSuffixMap['cr3_InvBJet'] = {'MuMu':'LimitBins/MuonInvBJetCR3', 'EE':'LimitBins/ElectronInvBJetCR3', 'EMu':'LimitBins/ElectronMuonInvBJetCR3'}
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


SystList = [
            ## Separate JES
            "JetAbsoluteMPFBiasUp","JetAbsoluteMPFBiasDown",
            "JetAbsoluteScaleUp",  "JetAbsoluteScaleDown",  
            "JetAbsoluteStatUp",   "JetAbsoluteStatDown",   
            "JetFlavorQCDUp",      "JetFlavorQCDDown",      
            "JetFragmentationUp",  "JetFragmentationDown",  
            "JetPileUpDataMCUp",   "JetPileUpDataMCDown",   
            "JetPileUpPtBBUp",     "JetPileUpPtBBDown",     
            "JetPileUpPtEC1Up",    "JetPileUpPtEC1Down",    
            "JetPileUpPtEC2Up",    "JetPileUpPtEC2Down",    
            "JetPileUpPtHFUp",     "JetPileUpPtHFDown",     
            "JetPileUpPtRefUp",    "JetPileUpPtRefDown",    
            "JetRelativeBalUp",    "JetRelativeBalDown",    
            "JetRelativeFSRUp",    "JetRelativeFSRDown",    
            "JetRelativeJEREC1Up", "JetRelativeJEREC1Down", 
            "JetRelativeJEREC2Up", "JetRelativeJEREC2Down", 
            "JetRelativePtBBUp",   "JetRelativePtBBDown",   
            "JetRelativePtEC1Up",  "JetRelativePtEC1Down",  
            "JetRelativePtEC2Up",  "JetRelativePtEC2Down",  
            "JetRelativePtHFUp",   "JetRelativePtHFDown",   
            "JetRelativeSampleUp", "JetRelativeSampleDown", 
            "JetRelativeStatECUp", "JetRelativeStatECDown", 
            "JetRelativeStatFSRUp","JetRelativeStatFSRDown",
            "JetRelativeStatHFUp", "JetRelativeStatHFDown", 
            "JetSinglePionECALUp", "JetSinglePionECALDown",    
            "JetSinglePionHCALUp", "JetSinglePionHCALDown",    
            "JetTimePtEtaUp",      "JetTimePtEtaDown",         
            ##
            "JetResUp","JetResDown",
            "JetEnUp","JetEnDown",
            "JetPUIDUp","JetPUIDDown",
            "JetPNETUp","JetPNETDown",
            "MuonEnUp","MuonEnDown",
            "MuonResUp","MuonResDown",
            "MuonRecoSFStatUp","MuonRecoSFStatDown",
            "MuonIDSFStatUp","MuonIDSFStatDown",
            "MuonTriggerSFStatUp","MuonTriggerSFStatDown",
            "MuonRecoSFUp","MuonRecoSFDown",
            "MuonIDSFUp","MuonIDSFDown",
            "MuonTriggerSFUp","MuonTriggerSFDown",
            "ElectronEnUp","ElectronEnDown",
            "ElectronResUp","ElectronResDown",
            "ElectronRecoSFStatUp","ElectronRecoSFStatDown",
            "ElectronIDSFStatUp","ElectronIDSFStatDown",
            "ElectronTriggerSFStatUp","ElectronTriggerSFStatDown",
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
            "FRUp","FRDown",
            "FRHighPtUp","FRHighPtDown",
            "PDFUp","PDFDown",
            "ScaleUp","ScaleDown",
            "HEMJetUp","HEMJetDown",
           ]

SystNameMap = {}
for era in ["2016","2016preVFP","2016postVFP","2017","2018"]:
  SystNameMap[era] = {}

  ### Up variations
  ## Separate JES
  SystNameMap[era]["JetAbsoluteMPFBiasUp"] = "CMS_scale_j_AbsoluteMPFBiasUp"
  SystNameMap[era]["JetAbsoluteScaleUp"]   = "CMS_scale_j_AbsoluteScaleUp"
  SystNameMap[era]["JetAbsoluteStatUp"]    = "CMS_scale_j_AbsoluteStat_"+era+"Up"
  SystNameMap[era]["JetFlavorQCDUp"]       = "CMS_scale_j_FlavorQCDUp"
  SystNameMap[era]["JetFragmentationUp"]   = "CMS_scale_j_FragmentationUp"
  SystNameMap[era]["JetPileUpDataMCUp"]    = "CMS_scale_j_PileUpDataMCUp"
  SystNameMap[era]["JetPileUpPtBBUp"]      = "CMS_scale_j_PileUpPtBBUp"
  SystNameMap[era]["JetPileUpPtEC1Up"]     = "CMS_scale_j_PileUpPtEC1Up"
  SystNameMap[era]["JetPileUpPtEC2Up"]     = "CMS_scale_j_PileUpPtEC2Up"
  SystNameMap[era]["JetPileUpPtHFUp"]      = "CMS_scale_j_PileUpPtHFUp"
  SystNameMap[era]["JetPileUpPtRefUp"]     = "CMS_scale_j_PileUpPtRefUp"
  SystNameMap[era]["JetRelativeBalUp"]     = "CMS_scale_j_RelativeBalUp"
  SystNameMap[era]["JetRelativeFSRUp"]     = "CMS_scale_j_RelativeFSRUp"
  SystNameMap[era]["JetRelativeJEREC1Up"]  = "CMS_scale_j_RelativeJEREC1_"+era+"Up"
  SystNameMap[era]["JetRelativeJEREC2Up"]  = "CMS_scale_j_RelativeJEREC2_"+era+"Up"
  SystNameMap[era]["JetRelativePtBBUp"]    = "CMS_scale_j_RelativePtBBUp"
  SystNameMap[era]["JetRelativePtEC1Up"]   = "CMS_scale_j_RelativePtEC1_"+era+"Up"
  SystNameMap[era]["JetRelativePtEC2Up"]   = "CMS_scale_j_RelativePtEC2_"+era+"Up"
  SystNameMap[era]["JetRelativePtHFUp"]    = "CMS_scale_j_RelativePtHFUp"
  SystNameMap[era]["JetRelativeSampleUp"]  = "CMS_scale_j_RelativeSample_"+era+"Up"
  SystNameMap[era]["JetRelativeStatECUp"]  = "CMS_scale_j_RelativeStatEC_"+era+"Up"
  SystNameMap[era]["JetRelativeStatFSRUp"] = "CMS_scale_j_RelativeStatFSR_"+era+"Up"
  SystNameMap[era]["JetRelativeStatHFUp"]  = "CMS_scale_j_RelativeStatHF_"+era+"Up"
  SystNameMap[era]["JetSinglePionECALUp"]  = "CMS_scale_j_SinglePionECALUp"
  SystNameMap[era]["JetSinglePionHCALUp"]  = "CMS_scale_j_SinglePionHCALUp"
  SystNameMap[era]["JetTimePtEtaUp"]       = "CMS_scale_j_TimePtEta_"+era+"Up"
  ##
  SystNameMap[era]["JetResUp"]            = "CMS_res_j_"+era+"Up"
  SystNameMap[era]["JetEnUp"]             = "CMS_scale_j_"+era+"Up"
  SystNameMap[era]["JetPUIDUp"]           = "CMS_eff_j_PUJetID_"+era+"Up"
  SystNameMap[era]["JetPNETUp"]           = "CMS_eff_j_ParticleNet_W_NominalUp" if "PNETdecorr" not in args.outputTag else "CMS_eff_j_ParticleNet_W_Nominal_"+era+"Up"
  SystNameMap[era]["MuonEnUp"]            = "CMS_scale_mUp"
  SystNameMap[era]["MuonResUp"]           = "CMS_res_mUp"
  SystNameMap[era]["MuonRecoSFStatUp"]    = "CMS_eff_m_reco_stat_"+era+"Up"
  SystNameMap[era]["MuonIDSFStatUp"]      = "CMS_SUS24014_eff_m_id_stat_"+era+"Up"
  SystNameMap[era]["MuonTriggerSFStatUp"] = "CMS_SUS24014_eff_m_trigger_stat_"+era+"Up"
  SystNameMap[era]["MuonRecoSFUp"]        = "CMS_eff_m_reco_systUp"
  SystNameMap[era]["MuonIDSFUp"]          = "CMS_SUS24014_eff_m_id_systUp"
  SystNameMap[era]["MuonTriggerSFUp"]     = "CMS_SUS24014_eff_m_trigger_systUp"
  SystNameMap[era]["ElectronEnUp"]        = "CMS_scale_eUp"
  SystNameMap[era]["ElectronResUp"]       = "CMS_res_eUp"
  SystNameMap[era]["ElectronRecoSFStatUp"]    = "CMS_eff_e_reco_stat_"+era+"Up"
  SystNameMap[era]["ElectronIDSFStatUp"]      = "CMS_SUS24014_eff_e_id_stat_"+era+"Up"
  SystNameMap[era]["ElectronTriggerSFStatUp"] = "CMS_SUS24014_eff_e_trigger_stat_"+era+"Up"
  SystNameMap[era]["ElectronRecoSFUp"]    = "CMS_eff_e_reco_systUp"
  SystNameMap[era]["ElectronIDSFUp"]      = "CMS_SUS24014_eff_e_id_systUp"
  SystNameMap[era]["ElectronTriggerSFUp"] = "CMS_SUS24014_eff_e_trigger_systUp"
  SystNameMap[era]["BTagSFHTagCorrUp"]    = "CMS_btag_hf_corrUp"
  SystNameMap[era]["BTagSFHTagUnCorrUp"]  = "CMS_btag_hf_uncorr_"+era+"Up"
  SystNameMap[era]["BTagSFLTagCorrUp"]    = "CMS_btag_lf_corrUp"
  SystNameMap[era]["BTagSFLTagUnCorrUp"]  = "CMS_btag_lf_uncorr_"+era+"Up"
  SystNameMap[era]["METUnclUp"]           = "CMS_scale_met_"+era+"Up"
  SystNameMap[era]["PrefireUp"]           = "CMS_l1_prefiring_"+era+"Up"
  SystNameMap[era]["PUUp"]                = "CMS_pileup_13TeV"+"Up" # full correlation
  SystNameMap[era]["CFRateUp"]            = "CMS_SUS24014_cf_stat_"+era+"Up"
  SystNameMap[era]["FRUp"]                = "CMS_SUS24014_fake_stat_"+era+"Up"
  SystNameMap[era]["FRHighPtUp"]          = "CMS_SUS24014_fake_highpt_"+era+"Up"
  SystNameMap[era]["PDFUp"]               = "pdf"+"Up" # full correlation
  SystNameMap[era]["ScaleUp"]             = "QCDscale"+"Up" # full correlation
  SystNameMap[era]["HEMJetUp"]            = "CMS_HEM_"+era+"Up"

  ### Down variations
  ## Separate JES
  SystNameMap[era]["JetAbsoluteMPFBiasDown"] = "CMS_scale_j_AbsoluteMPFBiasDown"
  SystNameMap[era]["JetAbsoluteScaleDown"]   = "CMS_scale_j_AbsoluteScaleDown"
  SystNameMap[era]["JetAbsoluteStatDown"]    = "CMS_scale_j_AbsoluteStat_"+era+"Down"
  SystNameMap[era]["JetFlavorQCDDown"]       = "CMS_scale_j_FlavorQCDDown"
  SystNameMap[era]["JetFragmentationDown"]   = "CMS_scale_j_FragmentationDown"
  SystNameMap[era]["JetPileUpDataMCDown"]    = "CMS_scale_j_PileUpDataMCDown"
  SystNameMap[era]["JetPileUpPtBBDown"]      = "CMS_scale_j_PileUpPtBBDown"
  SystNameMap[era]["JetPileUpPtEC1Down"]     = "CMS_scale_j_PileUpPtEC1Down"
  SystNameMap[era]["JetPileUpPtEC2Down"]     = "CMS_scale_j_PileUpPtEC2Down"
  SystNameMap[era]["JetPileUpPtHFDown"]      = "CMS_scale_j_PileUpPtHFDown"
  SystNameMap[era]["JetPileUpPtRefDown"]     = "CMS_scale_j_PileUpPtRefDown"
  SystNameMap[era]["JetRelativeBalDown"]     = "CMS_scale_j_RelativeBalDown"
  SystNameMap[era]["JetRelativeFSRDown"]     = "CMS_scale_j_RelativeFSRDown"
  SystNameMap[era]["JetRelativeJEREC1Down"]  = "CMS_scale_j_RelativeJEREC1_"+era+"Down"
  SystNameMap[era]["JetRelativeJEREC2Down"]  = "CMS_scale_j_RelativeJEREC2_"+era+"Down"
  SystNameMap[era]["JetRelativePtBBDown"]    = "CMS_scale_j_RelativePtBBDown"
  SystNameMap[era]["JetRelativePtEC1Down"]   = "CMS_scale_j_RelativePtEC1_"+era+"Down"
  SystNameMap[era]["JetRelativePtEC2Down"]   = "CMS_scale_j_RelativePtEC2_"+era+"Down"
  SystNameMap[era]["JetRelativePtHFDown"]    = "CMS_scale_j_RelativePtHFDown"
  SystNameMap[era]["JetRelativeSampleDown"]  = "CMS_scale_j_RelativeSample_"+era+"Down"
  SystNameMap[era]["JetRelativeStatECDown"]  = "CMS_scale_j_RelativeStatEC_"+era+"Down"
  SystNameMap[era]["JetRelativeStatFSRDown"] = "CMS_scale_j_RelativeStatFSR_"+era+"Down"
  SystNameMap[era]["JetRelativeStatHFDown"]  = "CMS_scale_j_RelativeStatHF_"+era+"Down"
  SystNameMap[era]["JetSinglePionECALDown"]  = "CMS_scale_j_SinglePionECALDown"
  SystNameMap[era]["JetSinglePionHCALDown"]  = "CMS_scale_j_SinglePionHCALDown"
  SystNameMap[era]["JetTimePtEtaDown"]       = "CMS_scale_j_TimePtEta_"+era+"Down"
  ##
  SystNameMap[era]["JetResDown"]            = "CMS_res_j_"+era+"Down"
  SystNameMap[era]["JetEnDown"]             = "CMS_scale_j_"+era+"Down"
  SystNameMap[era]["JetPUIDDown"]           = "CMS_eff_j_PUJetID_"+era+"Down"
  SystNameMap[era]["JetPNETDown"]           = "CMS_eff_j_ParticleNet_W_NominalDown" if "PNETdecorr" not in args.outputTag else "CMS_eff_j_ParticleNet_W_Nominal_"+era+"Down"
  SystNameMap[era]["MuonEnDown"]            = "CMS_scale_mDown"
  SystNameMap[era]["MuonResDown"]           = "CMS_res_mDown"
  SystNameMap[era]["MuonRecoSFStatDown"]    = "CMS_eff_m_reco_stat_"+era+"Down"
  SystNameMap[era]["MuonIDSFStatDown"]      = "CMS_SUS24014_eff_m_id_stat_"+era+"Down"
  SystNameMap[era]["MuonTriggerSFStatDown"] = "CMS_SUS24014_eff_m_trigger_stat_"+era+"Down"
  SystNameMap[era]["MuonRecoSFDown"]        = "CMS_eff_m_reco_systDown"
  SystNameMap[era]["MuonIDSFDown"]          = "CMS_SUS24014_eff_m_id_systDown"
  SystNameMap[era]["MuonTriggerSFDown"]     = "CMS_SUS24014_eff_m_trigger_systDown"
  SystNameMap[era]["ElectronEnDown"]        = "CMS_scale_eDown"
  SystNameMap[era]["ElectronResDown"]       = "CMS_res_eDown"
  SystNameMap[era]["ElectronRecoSFStatDown"]    = "CMS_eff_e_reco_stat_"+era+"Down"
  SystNameMap[era]["ElectronIDSFStatDown"]      = "CMS_SUS24014_eff_e_id_stat_"+era+"Down"
  SystNameMap[era]["ElectronTriggerSFStatDown"] = "CMS_SUS24014_eff_e_trigger_stat_"+era+"Down"
  SystNameMap[era]["ElectronRecoSFDown"]    = "CMS_eff_e_reco_systDown"
  SystNameMap[era]["ElectronIDSFDown"]      = "CMS_SUS24014_eff_e_id_systDown"
  SystNameMap[era]["ElectronTriggerSFDown"] = "CMS_SUS24014_eff_e_trigger_systDown"
  SystNameMap[era]["BTagSFHTagCorrDown"]    = "CMS_btag_hf_corrDown"
  SystNameMap[era]["BTagSFHTagUnCorrDown"]  = "CMS_btag_hf_uncorr_"+era+"Down"
  SystNameMap[era]["BTagSFLTagCorrDown"]    = "CMS_btag_lf_corrDown"
  SystNameMap[era]["BTagSFLTagUnCorrDown"]  = "CMS_btag_lf_uncorr_"+era+"Down"
  SystNameMap[era]["METUnclDown"]           = "CMS_scale_met_"+era+"Down"
  SystNameMap[era]["PrefireDown"]           = "CMS_l1_prefiring_"+era+"Down"
  SystNameMap[era]["PUDown"]                = "CMS_pileup_13TeV"+"Down" # full correlation
  SystNameMap[era]["CFRateDown"]            = "CMS_SUS24014_cf_stat_"+era+"Down"
  SystNameMap[era]["FRDown"]                = "CMS_SUS24014_fake_stat_"+era+"Down"
  SystNameMap[era]["FRHighPtDown"]          = "CMS_SUS24014_fake_highpt_"+era+"Down"
  SystNameMap[era]["PDFDown"]               = "pdf"+"Down" # full correlation
  SystNameMap[era]["ScaleDown"]             = "QCDscale"+"Down" # full correlation
  SystNameMap[era]["HEMJetDown"]            = "CMS_HEM_"+era+"Down"

  # SR-decorrelated sources -- Don't remove this, it is used below
  SystNameMap[era]["CFRate"]            = "CMS_SUS24014_cf_stat_"+era
  SystNameMap[era]["FR"]                = "CMS_SUS24014_fake_stat_"+era
  SystNameMap[era]["FRHighPt"]          = "CMS_SUS24014_fake_highpt_"+era
  SystNameMap[era]["JetRes"]            = "CMS_res_j_"+era
  SystNameMap[era]["JetEn"]             = "CMS_scale_j_"+era


## ChargeSplit has been deprecated due to insignificant improvement. Just legacy ##
ChargeSplit = False
if ChargeSplit:
  ChargeSplit = "ChargeSplit"
else:
  ChargeSplit = ""

MainPath = "/data9/Users/HNL_public/SUS-24-014/"
SKFlatOutputPath = "/data9/Users/HNL_public/SUS-24-014/SKFlatOutput/Systematic_Run/"

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
                                        'WZTo3LNu_amcatnlo','WZ_EWK', # 'WZTo3LNu_mllmin4p0_powheg' : amcatnlo gives better control in Inverted CR3
                                       ] #FIXME time to time
MergeList['RunPrompt']['ZZ_norm']       = ["ZZTo4L_powheg","GluGluToZZto4e","GluGluToZZto4mu","GluGluToZZto2e2mu"] #FIXME time to time
MergeList['RunPrompt']['WZ_norm']       = ["WZTo3LNu_amcatnlo","WZ_EWK"] #FIXME time to time
MergeList['RunPrompt']['WZ_norm_powheg']         = ["WZTo3LNu_mllmin4p0_powheg","WZ_EWK"] #FIXME time to time
MergeList['RunPrompt']['WZ_norm_amcatnlo']       = ["WZTo3LNu_amcatnlo","WZ_EWK"] #FIXME time to time
MergeList['RunPrompt']['WW_norm']       = ["WpWp_QCD","WpWp_EWK"] #FIXME time to time
MergeList['RunPrompt']['Prompt_others'] = [
                                           x for x in MergeList['RunPrompt']['Prompt_inc']
                                           if x not in MergeList['RunPrompt']['ZZ_norm']
                                           and x not in MergeList['RunPrompt']['WZ_norm']
                                           and x not in MergeList['RunPrompt']['WZ_norm_powheg']
                                           and x not in MergeList['RunPrompt']['WZ_norm_amcatnlo']
                                           and x not in MergeList['RunPrompt']['WW_norm']
                                          ]

MergeList['MC'] = {}
MergeList['MC']['MC_inc']    = MergeList['RunConv']['Conv_inc'] + MergeList['RunPrompt']['Prompt_inc']
MergeList['MC']['MC_others'] = MergeList['RunConv']['Conv_others'] + MergeList['RunPrompt']['Prompt_others']

MCFlag = {}
for this_conv in MergeList['RunConv']['Conv_inc']:
  MCFlag[this_conv] = "RunConv__"
for this_prompt in MergeList['RunPrompt']['Prompt_inc']:
  MCFlag[this_prompt] = "RunPrompt__"

# Region dependent MC skims
ConvSkim = {}
for DefFlag in DefFlags:
  ConvSkim[DefFlag] = {}
  for this_conv in ["TG","TTG","WZG","WWG","ZGToLLG","ZGToLLG_PtG_130","DYJets_MG","DYJets10to50_MG"]:
    ConvSkim[DefFlag][this_conv] = "_SkimTree_HNMultiLepBDT_"
  for this_conv in ["WGToLNuG","WGToLNuG_MG","WGToLNuG_01J_PtG_130","WGToLNuG_01J_PtG_300","WGToLNuG_01J_PtG_500","WGJJToLNu"]:
    ConvSkim[DefFlag][this_conv] = "_SkimTree_DileptonBDT_"
PromptSkim = {}
for DefFlag in DefFlags:
  PromptSkim[DefFlag] = {}
  for this_prompt in [*(x for x in MergeList['RunPrompt']['Prompt_inc'] if x != "ZZTo4L_powheg"), "WZTo3LNu_mllmin4p0_powheg"]:
    PromptSkim[DefFlag][this_prompt] = "_SkimTree_HNMultiLepBDT_"
  for this_prompt in ["ZZTo4L_powheg"]:
    PromptSkim[DefFlag][this_prompt] = "_SkimTree_SSDileptonBDT_"
MCSkim = {DefFlag: {**ConvSkim[DefFlag], **PromptSkim[DefFlag]} for DefFlag in DefFlags}


if args.CheckFiles:
  ##### Input file check #####
  DataList   = {}
  DataList['2016preVFP'] = []
  DataList['2016postVFP'] = []
  DataList['2017'] = []
  DataList['2018'] = []
  Streams = ["DoubleEG","DoubleMuon","MuonEG"]
  Streams_2018 = ["EGamma_GT36","DoubleMuon_GT36","MuonEG_GT36"] if (ANver >= 5) else ["EGamma","DoubleMuon","MuonEG"]
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
  DefFlags_CR = ["MultiLepton__"]
  SRPath = "/data9/Users/HNL_public/SUS-24-014/SKFlatOutput/Systematic_Run/HNL_SignalRegion_Plotter_"+inputTag
  CRPath = "/data9/Users/HNL_public/SUS-24-014/SKFlatOutput/Systematic_Run/HNL_ControlRegion_Plotter_"+inputTag

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
      #for this_proc in PromptList+["WZTo3LNu_mllmin4p0_powheg"]: # FIXME to test WZ_powheg and WZ_amcatnlo
      for this_proc in PromptList:
        this_path=SRPath + "/" + era + "/" + PreFlag+"RunPrompt__"+PostFlag+"/HNL_SignalRegion_Plotter"+PromptSkim[""][this_proc]+this_proc+".root"
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
          this_path=CRPath + "/" + era + "/" + PreFlag+DefFlag+"RunPrompt__"+PostFlag+"/HNL_ControlRegion_Plotter"+PromptSkim[DefFlag][this_proc]+this_proc+".root"
          if not os.path.exists(this_path):
            print(this_path,"-->",os.path.exists(this_path))

  exit()

if args.Merge:
  ##### Start merging #####
  if MergeData:
  
    if Blinded:
      print("[MergeData] Data blinded. skipping...")
    else:
      print("[MergeData] Data unblinded. merging...")
      for era in args.eras:
        if era=="Run2": # Deprecated
          for DefFlag in DefFlags:
            os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/" + PreFlag+DefFlag +PostFlag+"/DATA/")
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/" + PreFlag+DefFlag +PostFlag+ "/DATA/"+Analyzer+DataSkim+"DATA.root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile
                                        + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2016preVFP/" + PreFlag+DefFlag +PostFlag+ "/DATA/*"\
                                        + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2016postVFP/" + PreFlag+DefFlag +PostFlag+ "/DATA/*"\
                                        + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2017/" + PreFlag+DefFlag +PostFlag+ "/DATA/*"\
                                        + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2018/" + PreFlag+DefFlag +PostFlag+ "/DATA/*"\
            )
        else:
          for DefFlag in DefFlags:
            os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag +PostFlag+"/DATA/")
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag +PostFlag+ "/DATA/"+Analyzer+DataSkim+"DATA.root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/"+era+"/" + PreFlag+DefFlag +PostFlag+ "/DATA/*")
  
  if MergeFake:
  
    for era in args.eras:
      if era=="Run2":
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/")
          OutFile=MainPath + "/MergedFiles/" +Analyzer+"_"+inputTag + "/Run2/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/"+Analyzer+FakeSkim+"Fake.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag + "/2016preVFP/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag + "/2016postVFP/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag + "/2017/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag + "/2018/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*"\
          )
      else:
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/")
          OutFile=MainPath + "/MergedFiles/" +Analyzer+"_"+inputTag + "/" + era + "/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/"+Analyzer+FakeSkim+"Fake.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag + "/" + era+"/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*")
  
  if MergeCF:
  
    for era in args.eras:
      if era=="Run2":
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/")
          OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag + "/Run2/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/"+Analyzer+CFSkim+"CF.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2016preVFP/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2016postVFP/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2017/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2018/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*"\
          )
      else:
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/")
          OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag + "/"+ era + "/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/"+Analyzer+CFSkim+"CF.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/"+ era+"/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*") 
  
  if MergeConv:
  
    for era in args.eras:
      if era=="Run2":
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/" + PreFlag+DefFlag + "RunConv__"+PostFlag)
          for OutProc in list(MergeList['RunConv'].keys()):
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2016preVFP/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2016postVFP/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2017/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2018/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])
            )
      else:
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunConv__"+PostFlag)
          for OutProc in list(MergeList['RunConv'].keys()):
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/" +era+"/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]]))
  
  if MergePrompt:
  
    for era in args.eras:
      if era=="Run2":
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag)
          for OutProc in list(MergeList['RunPrompt'].keys()):
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2016preVFP/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+PromptSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2016postVFP/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+PromptSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2017/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+PromptSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2018/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+PromptSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])
            )
      else:
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag)
          for OutProc in list(MergeList['RunPrompt'].keys()):
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/" +era+"/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+PromptSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]]))
  
  
  if MergeMC:
  
    for era in args.eras:
      if era=="Run2":
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/" + PreFlag+DefFlag + "MergeMC__"+PostFlag)
          for OutProc in list(MergeList['MC'].keys()):
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/" + PreFlag+DefFlag + "MergeMC__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2016preVFP/" + PreFlag+DefFlag+MCFlag[ThisProc]+PostFlag+"/"+Analyzer+MCSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['MC'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2016postVFP/" + PreFlag+DefFlag+MCFlag[ThisProc]+PostFlag+"/"+Analyzer+MCSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['MC'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2017/" + PreFlag+DefFlag+MCFlag[ThisProc]+PostFlag+"/"+Analyzer+MCSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['MC'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/2018/" + PreFlag+DefFlag+MCFlag[ThisProc]+PostFlag+"/"+Analyzer+MCSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['MC'][OutProc]])
            )
      else:
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "MergeMC__"+PostFlag)
          for OutProc in list(MergeList['MC'].keys()):
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "MergeMC__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/" +era+"/" + PreFlag+DefFlag+MCFlag[ThisProc]+PostFlag+"/"+Analyzer+MCSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['MC'][OutProc]]))
  
  if MergeSignal:
  
    if args.CR:
      print("##### This is CR setting.")
      print("##### Skipping signal merging ...")
      pass
    else:
      for era in args.eras:
        for mass in args.masses:
          if era=="Run2":
            for DefFlag in DefFlags:
              os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/" + PreFlag+DefFlag + "RunSignal__"+PostFlag)
              OutFileDY    = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDY_"+mass+".root"
              OutFileVBF   = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalVBF_"+mass+".root"
              OutFileDYVBF = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDYVBF_"+mass+".root"
              OutFileSSWW  = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/Run2/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalSSWW_"+mass+".root"
              # First, create DY, VBF, SSWWTypeI seperately
              if os.system("hadd -f " + OutFileDY
                                                  + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016preVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                                  + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016postVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                                  + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2017/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                                  + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2018/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                           ) != 0:
                os.system("rm " + OutFileDY) # remove the output if there is any unmatched process
              if os.system("hadd -f " + OutFileVBF
                                                   + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016preVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                                   + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016postVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                                   + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2017/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                                   + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2018/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                           ) != 0:
                os.system("rm " + OutFileVBF) # remove the output if there is any unmatched process
                os.system("hadd -f " + OutFileSSWW + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*SSWWTypeI*"+mass+"_private.root")
              if os.system("hadd -f " + OutFileSSWW
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016preVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*SSWWTypeI*"+mass+"_private.root"\
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016postVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*SSWWTypeI*"+mass+"_private.root"\
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2017/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*SSWWTypeI*"+mass+"_private.root"\
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2018/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*SSWWTypeI*"+mass+"_private.root"\
                           ) != 0:
                os.system("rm " + OutFileSSWW) # remove the output if there is any unmatched process
              # Now treat DYVBF depending on the mass
              if int(mass.replace("M","")) < 300: # DY only
                os.system("hadd -f " + OutFileDYVBF
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016preVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016postVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2017/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root"\
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2018/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root")
              else: # DY+VBF
                os.system("hadd -f " + OutFileDYVBF
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016preVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root" + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016preVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016postVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root" + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2016postVFP/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2017/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root" + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2017/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*VBFTypeI*"+mass+"_private.root"\
                                                    + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2018/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root" + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/2018/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*VBFTypeI*"+mass+"_private.root")
          else:
            for DefFlag in DefFlags:
              os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunSignal__"+PostFlag)
  
              if mass=="Weinberg":
                OutFileWeinberg  = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalWeinberg.root"
                # Merge Weinberg samples
                os.system("hadd -f " + OutFileWeinberg + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*Weinberg*")
              else:
                OutFileDY    = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDY_"+mass+".root"
                OutFileVBF   = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalVBF_"+mass+".root"
                OutFileDYVBF = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDYVBF_"+mass+".root"
                OutFileSSWW  = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalSSWW_"+mass+".root"
                # First, create DY, VBF, SSWWTypeI seperately
                os.system("cp " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root " + OutFileDY)
                os.system("cp " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*VBFTypeI*"+mass+"_private.root " + OutFileVBF)
                if 500 <= int(mass.replace("M","")) and int(mass.replace("M","")) <= 30000: # SSWW
                  os.system("hadd -f " + OutFileSSWW + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*SSWWTypeI*"+mass+"_private.root")
                elif int(mass.replace("M","")) > 30000: # SSWW EMu
                  os.system("cp " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*SSWWTypeI*"+mass+"_private.root " + OutFileSSWW)
                # Now treat DYVBF depending on the mass
                if int(mass.replace("M","")) < 300: # DY only
                  os.system("cp " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root " + OutFileDYVBF)
                elif int(mass.replace("M","")) <= 3000: # DY+VBF
                  os.system("hadd -f " + OutFileDYVBF + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*DYTypeI*"+mass+"_private.root" + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*VBFTypeI*"+mass+"_private.root")

  exit()


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
    print("[CheckFile] opening",f_path,"...")
    f_root = TFile.Open(f_path)
  except OSError:
    print("[CheckFile] [!!WARNING!!] There is no file "+f_path+" .")
    print("[CheckFile] return None ...")
    return None
  else:
    print("[CheckFile] Good!")
    return f_root

def CheckHist(f_root,h_path,hist_name):

  this_hist = None

  try:
    print("[CheckHist] getting",h_path,"from",f_root,"...")
    this_hist = f_root.Get(h_path)
    this_hist.GetEntries()
  except AttributeError:
    print("[CheckHist] [!!WARNING!!] There is no hist named",h_path,"in",f_root,".")
    print("[CheckHist] return None for "+hist_name+"...")
    return None
  else:
    print("[CheckHist] Good!")
    return this_hist

########### Exception rules snippets ###############
from collections import defaultdict

SR_KEYWORDS = {"sr1", "sr2", "sr3"}

def region_key(region: str, cr_mode: bool = args.CR):
    #if cr_mode:
    #    return ("equals", region)
    #else:
    #    for kw in SR_KEYWORDS:
    #        if kw in region:
    #            return ("sr_pair", kw) # NOTE I don't think I need srx to crx pairing
    return ("equals", region)

def mass_condition(mass: str):
    if mass == "highmass":
        return '(mass_int >= 600 or mass == "Weinberg")'
    elif mass.startswith("M") and mass[1:].isdigit():
        return f"(mass_int == {mass[1:]})"
    else:
        return f'(mass == "{mass}")'

def generate_exception_code(excepts):
    """
    excepts: list of tuples like (tag, proc, region, era, channel, mass)
    """
    bucket = defaultdict(lambda: defaultdict(set))
    # bucket[(proc, channel, mass)][(mode, rk)] = {era1, era2, ...}

    for _, proc, region, era, channel, mass in excepts:
        mode, rk = region_key(region)
        bucket[(proc, channel, mass)][(mode, rk)].add(era)

    lines = []
    for (proc, channel, mass), rk_to_eras in bucket.items():
        for eras in {tuple(sorted(v)) for v in rk_to_eras.values()}:
            # group region conditions with same era conditions
            region_conds = []
            for (mode, rk), e_set in rk_to_eras.items():
                if tuple(sorted(e_set)) != eras:
                    continue
                if mode == "sr_pair":
                    crx = rk.replace("sr", "cr")  # srx to crx
                    region_conds.append(f'("{rk}" in region) or ("{crx}" in region)') # sr1 in region or cr1 in region. So srx and crx are synchronized. Do I need this?
                else:
                    region_conds.append(f'(region == "{rk}")')

            # era
            eras_sorted = sorted(eras)
            if len(eras_sorted) == 1:
                era_cond = f'(era == "{eras_sorted[0]}")'
            else:
                era_cond = "(" + " or ".join([f'(era == "{e}")' for e in eras_sorted]) + ")"

            # channel, mass
            conds = [" or ".join(region_conds), era_cond, f'(channel == "{channel}")', mass_condition(mass)]
            cond_str = " and ".join([f"({c})" for c in conds])

            lines.append(
                f'if {cond_str}:\n'
                f'    this_process["{proc}"] = "0"  # auto-generated from MakeInput_public.py'
            )

    lines.sort()
    return "\n\n".join(lines)

def write_exceptions_module(path, code_str, save, exceptionTag):

    add = True if save == "Add" else False

    region_tag = "CR" if args.CR else "SR"

    start_tag = f"# --- {region_tag} RULES START ---"
    end_tag   = f"# --- {region_tag} RULES END ---"

    if (not add) or (not os.path.exists(path)): # write
        with open(path, "w", encoding="utf-8") as f:
            f.write("# Auto-generated; DO NOT EDIT BY HAND\n")
            f.write("def apply_auto_exceptions(this_process, region, era, channel, mass, mass_int, tag):\n")
            f.write("    # BEGIN AUTO RULES\n")

            block_lines = []
            if exceptionTag:
                block_lines.append("\n")
                block_lines.append(f"    if tag == '{exceptionTag}':\n")
                block_lines.append(f"        {start_tag}\n")
                for line in code_str.splitlines():
                    block_lines.append(f"        {line}\n" if line.strip() else "\n")
                block_lines.append(f"        {end_tag}\n")
            else:
                block_lines.append("\n")
                block_lines.append(f"    {start_tag}\n")
                for line in code_str.splitlines():
                    block_lines.append(f"    {line}\n" if line.strip() else "\n")
                block_lines.append(f"    {end_tag}\n")
            f.writelines(block_lines)
            f.write("\n")
            f.write("    # END AUTO RULES\n")
            f.write("    return this_process\n")
        return
    else: # add
        with open(path, "r", encoding="utf-8") as f:
            lines = f.readlines()

        new_lines = []
        inserted = False
        for line in lines:
            if line.strip() == "# END AUTO RULES" and not inserted:
                if exceptionTag:
                    new_lines.append("\n")
                    new_lines.append(f"    if tag == '{exceptionTag}':\n")
                    new_lines.append(f"        {start_tag}\n")
                    for l in code_str.splitlines():
                        new_lines.append(f"        {l}\n" if l.strip() else "\n")
                    new_lines.append(f"        {end_tag}\n")
                else:
                    new_lines.append("\n")
                    new_lines.append(f"    {start_tag}\n")
                    for l in code_str.splitlines():
                        new_lines.append(f"    {l}\n" if l.strip() else "\n")
                    new_lines.append(f"    {end_tag}\n")
                inserted = True
            new_lines.append(line)

        with open(path, "w", encoding="utf-8") as f:
            f.writelines(new_lines)


##### Main job starts #####
Except_list = []

for tag in args.histTag:
  for era in args.eras:
    for region in regions: # ...and even each region to control!!
      print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",region,"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
      OutputName = inputTag+"_"+tag+outputTag+outputTagSuffix
      OutputPath = os.getcwd()+'/LimitInputs/'+OutputName+'/'
      os.system('mkdir -p '+OutputPath + era + '/' + region)
  
      f_path_data          = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+PostFlag + "/DATA/"+Analyzer+DataSkim+"DATA.root"
      f_path_fake          = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunFake__"+PostFlag+"/DATA/"+Analyzer+FakeSkim+"Fake.root"
      f_path_cf            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunCF__"+PostFlag+"/DATA/"+Analyzer+CFSkim+"CF.root"
      f_path_zg            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunConv__"+PostFlag+"/"+Analyzer+"_ZG_norm.root"
      f_path_conv_inc      = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunConv__"+PostFlag+"/"+Analyzer+"_Conv_inc.root"
      f_path_conv_others   = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunConv__"+PostFlag+"/"+Analyzer+"_Conv_others.root"
      if 'WZ_powheg' in outputTag:
        f_path_wz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_WZ_norm_powheg.root"
      elif 'WZ_amcatnlo' in outputTag:
        f_path_wz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_WZ_norm_amcatnlo.root"
      else:
        #if args.CR: #FIXME this is due to old CR...
        #  f_path_wz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_SkimTree_HNMultiLepBDT_WZ_norm.root"
        #else:
        #  f_path_wz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_WZ_norm.root"
        f_path_wz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_WZ_norm.root"
      #if args.CR: #FIXME this is due to old CR...
      #  f_path_zz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_SkimTree_HNMultiLepBDT_ZZ_norm.root"
      #  f_path_ww            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_SkimTree_HNMultiLepBDT_WW_norm.root"
      #  f_path_prompt_inc    = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_SkimTree_HNMultiLepBDT_Prompt_inc.root"
      #  f_path_prompt_others = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_SkimTree_HNMultiLepBDT_Prompt_others.root"
      #else:
      #  f_path_zz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_ZZ_norm.root"
      #  f_path_ww            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_WW_norm.root"
      #  f_path_prompt_inc    = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_Prompt_inc.root"
      #  f_path_prompt_others = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_Prompt_others.root"
      f_path_zz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_ZZ_norm.root"
      f_path_ww            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_WW_norm.root"
      f_path_prompt_inc    = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_Prompt_inc.root"
      f_path_prompt_others = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_Prompt_others.root"
      f_path_mc_inc        = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"MergeMC__"+PostFlag+"/"+Analyzer+"_MC_inc.root"
      f_path_mc_others     = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"MergeMC__"+PostFlag+"/"+Analyzer+"_MC_others.root"
      
      if not Blinded: f_data = TFile.Open(f_path_data)
      f_fake          = TFile.Open(f_path_fake)
      f_cf            = TFile.Open(f_path_cf)
      f_zg            = TFile.Open(f_path_zg)
      f_conv_inc      = TFile.Open(f_path_conv_inc)
      f_conv_others   = TFile.Open(f_path_conv_others)
      f_wz            = TFile.Open(f_path_wz)
      f_zz            = TFile.Open(f_path_zz)
      f_ww            = TFile.Open(f_path_ww)
      f_prompt_inc    = TFile.Open(f_path_prompt_inc)
      f_prompt_others = TFile.Open(f_path_prompt_others)
      f_mc_inc        = TFile.Open(f_path_mc_inc)
      f_mc_others     = TFile.Open(f_path_mc_others)

      for mass in args.masses: # iterate for each mass ...
        is_Weinberg = (mass == "Weinberg")

        if not is_Weinberg:
          mass_int = int(mass.replace("M",""))

          if ("r1" in region or "r2" in region) and (mass_int <= 100):
            continue # NOTE use only SR3 below M100
          if ("r1" in region) and (mass_int > 3000):
            continue # NOTE skip SR1 above M3000

        for channel in args.channels: # ...and each channel

          if not is_Weinberg:
            if "EMu" not in channel and (mass_int > 30000):
              continue # NOTE Only EMu extends above M30000

            if (("sr3" in region) or ("cr3_Inv" in region)) and (mass_int <= 500):
              LimitDir = "LimitExtractionBDT"
              InputHistMass = mass+"/"
              if 'BDT' not in RegionToHistSuffixMap[region][channel]:
                RegionToHistSuffixMap[region][channel] += 'BDT'
              if BDTver:
                if args.CR:
                  if BDTver.split('_')[0] not in RegionToChannelMap[region][channel]: # BDTver == V3_Strict_15_Bin; CR histo path: V3_EE/M100/LimitBins
                    RegionToChannelMap[region][channel] = RegionToChannelMap[region][channel]+"_"+BDTver.split('_')[0]
                else:
                  if BDTver not in RegionToChannelMap[region][channel]:
                    RegionToChannelMap[region][channel] = RegionToChannelMap[region][channel]+"_"+BDTver

              if args.Ext:
                # Ext runs only with M500
                if mass!="M500":
                  print(mass,"is not allowed to run with Ext option.")
                  print("Exiting ...")
                  sys.exit(1)

                LimitDir = "LimitExtraction"
                InputHistMass = ""
                RegionToHistSuffixMap[region][channel] = RegionToHistSuffixMap[region][channel].replace('BDT','')
                if BDTver:
                  if args.CR:
                    RegionToChannelMap[region][channel] = RegionToChannelMap[region][channel].replace("_"+BDTver.split('_')[0],'')
                  else:
                    RegionToChannelMap[region][channel] = RegionToChannelMap[region][channel].replace("_"+BDTver,'')

            else:
              #if region=='sr2' and 'AltBin' in outputTag: LimitDir = "LimitExtractionAlt" # SR2 alternative optimization : use the same binning for all era, flavor. (deprecated)
              #else: LimitDir = "LimitExtraction"
              if region=='sr1' and 'AltBin' in outputTag: LimitDir = "LimitExtractionAlt" # SR1 alternative optimization : bin optimized with sqrt-removed-FOM.
              else: LimitDir = "LimitExtraction"

              if (region=="sr1") and (mass_int <= 3000):
                if mass_int <= 400: InputHistMass = "M400/"
                elif mass_int >= 900: InputHistMass = "M900/"
                else: InputHistMass = mass+"/"
              else:
                InputHistMass = ""

              RegionToHistSuffixMap[region][channel] = RegionToHistSuffixMap[region][channel].replace('BDT','')
              if BDTver:
                if args.CR:
                  RegionToChannelMap[region][channel] = RegionToChannelMap[region][channel].replace("_"+BDTver.split('_')[0],'')
                else:
                  RegionToChannelMap[region][channel] = RegionToChannelMap[region][channel].replace("_"+BDTver,'')

            # Set channel dependent scaler first
            DYVBFscaler = 0.01 # Set the signalDYVBF scaler
            if mass_int <= 100: DYVBFscaler = 0.001 # if you want to use HybridNew without additional options, see https://cms-talk.web.cern.ch/t/too-large-error-with-hybridnew/32844
            if mass_int > 3000: DYVBFscaler = 0.1 # relax the scale for SSWW impact
            SSWWscaler = DYVBFscaler*DYVBFscaler # Set the signalSSWW scaler

          else: # TODO let's merge Weinberg and other signals later, e.g. setting mass_int = 999999 for the Weinberg
            #if region=='sr2' and 'AltBin' in outputTag: LimitDir = "LimitExtractionAlt" # SR2 alternative optimization : use the same binning for all era, flavor. (deprecated)
            #else: LimitDir = "LimitExtraction"
            if region=='sr1' and 'AltBin' in outputTag: LimitDir = "LimitExtractionAlt" # SR1 alternative optimization : bin optimized with sqrt-removed-FOM.
            else: LimitDir = "LimitExtraction"
            InputHistMass = ""
            RegionToHistSuffixMap[region][channel] = RegionToHistSuffixMap[region][channel].replace('BDT','')
            Weinbergscaler = 10000. # Set the signalWeinberg scaler

          print("f_cf :",f_path_cf)
          print("input_hist :", LimitDir+"/"+tag+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel])
          input_hist = LimitDir+"/"+tag+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel]
          
          print("##### Initiating",region,mass,channel,"...")
          if not Blinded: h_data        = f_data.Get(input_hist)
          h_fake          = f_fake.Get(input_hist)
          h_cf            = f_cf.Get(input_hist) if "EE" in channel else ""
          h_zg            = f_zg.Get(input_hist)
          h_conv_inc      = f_conv_inc.Get(input_hist)
          h_conv_others   = f_conv_others.Get(input_hist)
          h_wz            = f_wz.Get(input_hist)
          h_zz            = f_zz.Get(input_hist)
          h_ww            = f_ww.Get(input_hist)
          h_prompt_inc    = f_prompt_inc.Get(input_hist)
          h_prompt_others = f_prompt_others.Get(input_hist)
          h_mc_inc        = f_mc_inc.Get(input_hist)
          h_mc_others     = f_mc_others.Get(input_hist)
          print("##### histo done.")
 
          # Make list of [file path, histogram, histo name]
          input_list = [
                        [f_path_fake, h_fake, "fake"],
                        [f_path_cf, h_cf, "cf"],
                        [f_path_zg, h_zg, "zg"],
                        [f_path_conv_inc,    h_conv_inc,    "conv_inc"],
                        [f_path_conv_others, h_conv_others, "conv_others"],
                        [f_path_wz, h_wz, "wz"],
                        [f_path_zz, h_zz, "zz"],
                        [f_path_ww, h_ww, "ww"],
                        [f_path_prompt_inc,    h_prompt_inc,    "prompt_inc"],
                        [f_path_prompt_others, h_prompt_others, "prompt_others"],
                        [f_path_mc_inc,    h_mc_inc,    "mc_inc"],
                        [f_path_mc_others, h_mc_others, "mc_others"],
                       ]

          #### Remove CF if not EE
          if "EE" not in channel:
            print("This is",channel,"channel --> Remove CF item:")
            print(input_list.pop(1))

          #### Treat 0 fakes: see v) of https://hypernews.cern.ch/HyperNews/CMS/get/EXO-21-002/25
          try:
            for i in range(h_fake.GetNbinsX()):
              if h_fake.GetBinContent(i+1) <= 0.:
                print("!!!!!! zero fakes detected in",f_path_fake,input_hist,"!!!!!!")
                print("!!!!!! bin",i+1,":",h_fake.GetBinContent(i+1),"!!!!!!")
                h_fake.SetBinContent(i+1,0.15*0.645)
                h_fake.SetBinError(i+1,0.15*0.645)
          except AttributeError:
            print("[!!WARNING!!] There is no hist named "+input_hist+" in "+f_path_fake+":"," .")
            print("Skipping treatment on zero fakes...") #NOTE This means, if there is no fake hist in this region, just skip this mass/channel
            continue

          this_nbins = h_fake.GetNbinsX()
          Nproc = len(input_list) # The number of processes = the length of the input list before adding systematics

          if args.Scan:
            print("##### Scan initiated. #####")
            h_scan = TH2D("Nominal","Nominal",this_nbins,0,this_nbins,Nproc+2,0,Nproc+2) # There is no automatic merging from many TH1s... see https://root-forum.cern.ch/t/filling-a-th2-from-two-existing-th1/14575; +2 is to secure space for 2 signals. I was going to extend the axis, but... (below)
            #h_scan.GetYaxis().SetCanExtend(1) # This seems not resolved... https://root-forum.cern.ch/t/extending-axis-for-th1-vs-th2/20964
            print("h_scan for Nominal created; this should be empty:",h_scan.Integral(0,this_nbins,1,1))
            if h_scan.Integral(0,this_nbins,1,1)!=0.: sys.exit()
            h_scan.SetDirectory(0)
            scan_list = []

            for i in range(Nproc):
              print("##### Making 2D hist for",input_list[i][2],"#####")
              FillScan(h_scan,input_list[i][1],input_list[i][2]) # out, in, name
          
          if Blinded:
            print("##### This analysis is blinded.")
            print("##### Creating Asimov data...")
            #print("Adding prompt...")
            #h_data = h_prompt_inc.Clone() #NOTE deprecated. only preservation purposes
            print("Adding MCs...")
            h_data = h_mc_inc.Clone()

            bkg_list = [ #bkg except mc (which was already added above)
                        [f_path_fake, h_fake, "fake"],
                        [f_path_cf, h_cf, "cf"],
                        #[f_path_conv_inc, h_conv_inc, "conv_inc"], #NOTE deprecated. only preservation purposes. PREVIOUSLY: start from prompt_inc, add fake, cf, conv_inc. Now, start from mc_inc and add fake, cf and that's all.
                       ]
          
            if "Mu" in channel:
              print("This is",channel,"channel. --> Remove CF item in bkg list:")
              print(bkg_list.pop(1))

            total_number = 0 # to cross check
            #total_number += h_prompt_inc.GetEntries() #NOTE deprecated. only preservation purposes
            total_number += h_mc_inc.GetEntries()
          
            for bkg in bkg_list:
              try:
                bkg[1].GetEntries()
              except AttributeError:
                print("[!!WARNING!!] There is no hist named "+input_hist+" in "+bkg[0]+" .")
                print("Skipping "+bkg[2]+" in total background...")
                continue
              print("Adding "+bkg[2]+"...")
              h_data.Add(bkg[1]) # Add each bkg while iterating
              total_number += bkg[1].GetEntries()
          
            #print h_data.GetEntries(), total_number
            if h_data.GetEntries() == total_number: pass #NOTE valid only when blinded.
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
            if not is_Weinberg:
              f_path_signalDYVBF = MainPath +"/MergedFiles/"+Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDYVBF_"+mass+".root"
              f_path_signalDY = MainPath +"/MergedFiles/"+Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDY_"+mass+".root"
              f_path_signalVBF = MainPath +"/MergedFiles/"+Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalVBF_"+mass+".root"
              f_path_signalSSWW  = MainPath +"/MergedFiles/"+Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalSSWW_"+mass+".root"
  
              f_signalDYVBF = CheckFile(f_path_signalDYVBF)
              if f_signalDYVBF:
                h_signalDYVBF = CheckHist(f_signalDYVBF,input_hist,"signalDYVBF")
                if h_signalDYVBF:
                  h_signalDYVBF.Scale(DYVBFscaler) # Scaling the signal due to Combine fitting
                  input_list.append([f_path_signalDYVBF, h_signalDYVBF, "signalDYVBF"])
                  #print("Scaled signalDYVBF :", h_signalDYVBF.Integral())
                if args.Scan:
                  print("##### Making 2D hist for","signalDYVBF","#####")
                  FillScan(h_scan,h_signalDYVBF,"signalDYVBF") # out, in, name

              f_signalDY = CheckFile(f_path_signalDY)
              if f_signalDY:
                h_signalDY = CheckHist(f_signalDY,input_hist,"signalDY")
                if h_signalDY:
                  h_signalDY.Scale(DYVBFscaler) # Scaling the signal due to Combine fitting
                  input_list.append([f_path_signalDY, h_signalDY, "signalDY"])
                  #print("Scaled signalDY :", h_signalDY.Integral())
                if args.Scan:
                  print("##### Making 2D hist for","signalDY","#####")
                  FillScan(h_scan,h_signalDY,"signalDY") # out, in, name

              f_signalVBF = CheckFile(f_path_signalVBF)
              if f_signalVBF:
                h_signalVBF = CheckHist(f_signalVBF,input_hist,"signalVBF")
                if h_signalVBF:
                  h_signalVBF.Scale(DYVBFscaler) # Scaling the signal due to Combine fitting
                  input_list.append([f_path_signalVBF, h_signalVBF, "signalVBF"])
                  #print("Scaled signalVBF :", h_signalVBF.Integral())
                if args.Scan:
                  print("##### Making 2D hist for","signalVBF","#####")
                  FillScan(h_scan,h_signalVBF,"signalVBF") # out, in, name

              f_signalSSWW = CheckFile(f_path_signalSSWW)
              if f_signalSSWW:
                h_signalSSWW = CheckHist(f_signalSSWW,input_hist,"signalSSWW")
                if h_signalSSWW:
                  h_signalSSWW.Scale(SSWWscaler) # Scaling the signal due to Combine fitting
                  input_list.append([f_path_signalSSWW, h_signalSSWW, "signalSSWW"])
                  #print("Scaled signalSSWW :", h_signalSSWW.Integral())
                if args.Scan:
                  print("##### Making 2D hist for","signalSSWW","#####")
                  FillScan(h_scan,h_signalSSWW,"signalSSWW") # out, in, name
            else:
              f_path_signalWeinberg  = MainPath +"/MergedFiles/"+Analyzer+"_"+inputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalWeinberg.root"

              f_signalWeinberg = CheckFile(f_path_signalWeinberg)
              if f_signalWeinberg:
                h_signalWeinberg = CheckHist(f_signalWeinberg,input_hist,"signalWeinberg")
                if h_signalWeinberg:
                  h_signalWeinberg.Scale(Weinbergscaler) # Scaling the signal due to Impact
                  input_list.append([f_path_signalWeinberg, h_signalWeinberg, "signalWeinberg"])
                  #print("signalWeinberg :", h_signalWeinberg.Integral())
                if args.Scan:
                  print("##### Making 2D hist for","signalWeinberg","#####")
                  FillScan(h_scan,h_signalWeinberg,"signalWeinberg") # out, in, name

            if args.Scan:
              scan_list.append(h_scan)

            print("##### Signal done.")

          Nproc = len(input_list) # Update the Nproc to include signals
          NoNOMs = set()
          #### Treat negative events for nominal histograms ####
          for iProc in range(Nproc):
            try:
              for j in range(this_nbins):
                if input_list[iProc][1].GetBinContent(j+1) <= 0.:
                  print("!!!!!! Negative events detected in",input_list[iProc][2],input_list[iProc][0],input_hist,"!!!!!!")
                  print("!!!!!! bin",j+1,":",input_list[iProc][1].GetBinContent(j+1),"!!!!!!")
                  print("!!!!!! Setting this bin to 0 ...")
                  input_list[iProc][1].SetBinContent(j+1,0.)
            except AttributeError:
              print("[!!WARNING!!] There is no NOMINAL hist named "+input_hist+" in "+input_list[iProc][0]+" .")
              print("[!!WARNING!!] Please delete this in the datacard ...")
              NoNOMs.add(iProc)
            else:
              if input_list[iProc][1].Integral()<=0.:
                print("!!!!!! Zero norm detected in",input_list[iProc][2],input_list[iProc][0],input_hist,"!!!!!!")
                print("Please delete this in the datacard ...")
                NoNOMs.add(iProc)
  
          if args.Syst:
            print("##### Systematics activated.")
  
            for i in range(Nproc):

              if "fake_data_path" in input_list[i][0]: continue # There is no file like "fake_data_path" so pass this in the iteration
              if i in NoNOMs:
                print("No nominal hist for",input_list[i][2],"in",tag,era,region,mass,channel,".")
                print("Making exception list ...") # TODO better to make exception list outside of the Syst iteration later
                if "signal" in input_list[i][2]:
                  Except_list.append((tag, input_list[i][2], region, era, channel, mass))
                elif "conv" in input_list[i][2] or "prompt" in input_list[i][2]: pass # We don't use conv or prompt anymore. We use mc_others
                else:
                  Except_list.append((tag, input_list[i][2], region, era, channel, mass)) if (not is_Weinberg and mass_int < 600) else Except_list.append((tag, input_list[i][2], region, era, channel, "highmass"))
                print("Pass systematics ...")
                continue

              if args.Scan:
                h_scan = TH2D(input_list[i][2],input_list[i][2],this_nbins,0,this_nbins,Nproc,0,Nproc)
                print("h_scan for",input_list[i][2],"syst created; this should be empty:",h_scan.Integral(0,this_nbins,1,1))
                if h_scan.Integral(0,this_nbins,1,1)!=0.: sys.exit()
                h_scan.SetDirectory(0)

              f_syst = CheckFile(input_list[i][0]) # Get each process's file
              if not f_syst:
                print("[!!ERROR!!] No syst file",input_list[i][0],". Exiting...")
                sys.exit()

              ###### Now treat PDF error sets ######
              if "PDFUp" in SystList: # Calculate PDF variation first
                hist_pdfUp = h_data.Clone()
                hist_pdfUp.Reset()
                hist_pdfDown = h_data.Clone()
                hist_pdfDown.Reset()
                if 'signal' in input_list[i][2]:
                  Nreplica = 100
                  pdf_hists = []
                  for it_rep in range(Nreplica):
                    this_pdf_hist = LimitDir+"/Syst_PDF"+tag+"_Syst_PDF"+str(it_rep)+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel]
                    #print(this_pdf_hist)
                    #CheckHist(f_syst,this_pdf_hist,"PDF"+str(it_rep))
                    h_pdf = f_syst.Get(this_pdf_hist)
                    # apply the same scale to the pdf variations
                    if 'DY' in input_list[i][2] or 'VBF' in input_list[i][2]:
                      h_pdf.Scale(DYVBFscaler)
                    elif 'SSWW' in input_list[i][2]:
                      h_pdf.Scale(SSWWscaler)
                    elif 'Weinberg' in input_list[i][2]:
                      h_pdf.Scale(Weinbergscaler)
                    else:
                      print("[ERROR] in PDF uncertainty calculation: There is no hist",this_pdf_hist,".")
                      print("[ERROR] Exiting ...")
                      exit()
                    pdf_hists.append(h_pdf)
                  for it_bin in range(1, this_nbins + 1):
                    bin_values = [pdf_hists[it_rep].GetBinContent(it_bin) for it_rep in range(Nreplica)]
                    std = np.std(bin_values, ddof=1)
                    nom = input_list[i][1].GetBinContent(it_bin)
                    if nom < 0.: print("[ERROR] signal",input_list[i][2],"has negative events!!! Check bin",it_bin,":",nom)
                  
                    hist_pdfUp.SetBinContent(it_bin, nom + std)
                    hist_pdfDown.SetBinContent(it_bin, max(nom - std, 0.))

              for this_syst in SystList: # Define new input_hist with each syst name

                input_hist = LimitDir+"/Syst_"+this_syst+tag+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel]

                this_name_syst = SystNameMap[era][this_syst]
                if ('PDF' in this_syst) or (('Scale' in this_syst) and ('Jet' not in this_syst)):
                  if 'signal' not in input_list[i][2]: continue
                  else:
                    if "DYVBF" in input_list[i][2]:
                      this_name_syst = this_name_syst.replace('pdf','pdf_DYVBF').replace('scale','scale_DYVBF')
                    elif "DY" in input_list[i][2]:
                      this_name_syst = this_name_syst.replace('pdf','pdf_DY').replace('scale','scale_DY')
                    elif "VBF" in input_list[i][2]:
                      this_name_syst = this_name_syst.replace('pdf','pdf_VBF').replace('scale','scale_VBF')
                    elif "SSWW" in input_list[i][2]:
                      this_name_syst = this_name_syst.replace('pdf','pdf_SSWW').replace('scale','scale_SSWW')
                    elif "Weinberg" in input_list[i][2]:
                      this_name_syst = this_name_syst.replace('pdf','pdf_Weinberg').replace('scale','scale_Weinberg')
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
                    this_syst_nameSep = SystNameMap[era][this_syst_source]+regionName_Decorr+this_syst.replace(this_syst_source,'') # JetRes_sr1Up
                    name_syst = input_list[i][2]+"_"+this_syst_nameSep

                if 'PDFUp' in this_syst:
                  h_syst = hist_pdfUp
                elif 'PDFDown' in this_syst:
                  h_syst = hist_pdfDown
                else:
                  h_syst = CheckHist(f_syst,input_hist,name_syst)

                if h_syst:
                  if args.Scan: # Do the scan before any treatment (0 fake)
                    print("##### Making 2D hist for",name_syst,"#####")
                    FillScan(h_scan,h_syst,name_syst) # out, in, name

                  h_syst.SetDirectory(0) # Store h_syst in memory so that it cannot be deleted during the iteration
                  #### Treat 0 fakes: see v) of https://hypernews.cern.ch/HyperNews/CMS/get/EXO-21-002/25
                  if input_list[i][2] == "fake" and "FR" in this_syst and "CF" not in this_syst:
                    for j in range(this_nbins):
                      if h_syst.GetBinContent(j+1) <= 0.:
                        print("!!!!!! zero fakes detected in ",input_list[i][0],input_hist,"with syst:",name_syst,"!!!!!!")
                        print("!!!!!! bin",j+1,":",h_syst.GetBinContent(j+1),"!!!!!!")
                        h_syst.SetBinContent(j+1,0.15*0.645)
                        h_syst.SetBinError(j+1,0.15*0.645)
                  else:
                  #### Treat negative events for systematic histograms ####
                    for j in range(this_nbins):
                      if h_syst.GetBinContent(j+1) <= 0.:
                        print("!!!!!! Negative events detected in",input_list[i][2],input_hist,"with syst:",name_syst,"!!!!!!")
                        print("!!!!!! bin",j+1,":",h_syst.GetBinContent(j+1),"!!!!!!")
                        print("!!!!!! Setting this bin to 0 ...")
                        h_syst.SetBinContent(j+1,0.)
                  #### Now check zero norm ... ####
                  if h_syst.Integral()<=0.:
                    print("!!!!!! Zero norm detected in",input_list[i][2],input_hist,"with syst:",name_syst,"!!!!!!")
                    print("Making a makeup hist(=cc of nominal)...") # When the syst variation is negative, just use the nominal as a variation.
                    h_syst = input_list[i][1].Clone()
                    h_syst.SetDirectory(0)

                else:
                  if args.Scan: # Do the scan before any treatment (empty hist makeup)
                    print("##### Making 2D hist for",name_syst,"#####")
                    FillScan(h_scan,h_syst,name_syst) # out, in, name

                  print("No hist for",name_syst,".") # Sometimes there is no hist with syst variation and Combine complains. This is to makeup this.
                  print("Making a makeup hist(=cc of nominal)...") # Sometimes there is no hist with syst variation and Combine complains. This is to makeup this.
                  try:
                    h_syst = input_list[i][1].Clone()
                    h_syst.SetDirectory(0)
                  except ReferenceError:
                    print("Failed. There is no NOMINAL. Please check ...")
                # Now h_systs are fully made-up.

                # scale signal systs except the pdf variations (which are already done)
                if 'PDFUp' in this_syst or 'PDFDown' in this_syst: pass
                elif h_syst.Integral() == input_list[i][1].Integral(): pass # if this syst is just a cc of the nominal, then no need to scale.
                else:
                  if not is_Weinberg:
                    if "signalDYVBF" in input_list[i][2]: # Scale the syst variated signals
                      h_syst.Scale(DYVBFscaler)
                    elif "signalDY" in input_list[i][2]: # Scale the syst variated signals
                      h_syst.Scale(DYVBFscaler)
                    elif "signalVBF" in input_list[i][2]: # Scale the syst variated signals
                      h_syst.Scale(DYVBFscaler)
                    elif "signalSSWW" in input_list[i][2]:
                      h_syst.Scale(SSWWscaler)
                  else:
                    if "signalWeinberg" in input_list[i][2]: # Scale the syst variated signals
                      h_syst.Scale(Weinbergscaler)
                print("Appending "+name_syst+"...")
                input_list.append([input_list[i][0], h_syst, name_syst]) # Append each syst histogram while iterating
  
              if args.Scan:
                #for i in range(h_scan.GetNbinsX()): print h_scan.GetYaxis().GetBinLabel(3), h_scan.GetBinContent(i+1,3)
                h_scan.SetDirectory(0)
                scan_list.append(h_scan)
                #print scan_list

            print("##### Systematics done.")

          ### Now remove NoNOMs
          for i in sorted(NoNOMs, reverse=True): # pop up in reverse order, to prevent index mismatch
            print("Erase nominal zero norm:")
            print(input_list.pop(i))
  
          print("##### Now creating a limit input root file...")
          outName = OutputPath+era+"/"+region+"/"+mass+"_"+channel+ExtTag
          outfile = TFile.Open(outName+"_card_input.root","RECREATE")
          
          outfile.cd() # Move into it
          for item in input_list: # Remember, item = [path,hist,name]
            try:
              item[1].SetName(item[2])

              if item[1].Integral() <=0 : # treat -ve bins #FIXME Must check bin-by-bin in principle..
                print("[!!WARNING!!] Negative events "+str(item[1].Integral())+" in "+item[2]+" ------------------------------------")

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

            except AttributeError:
              print("[!!WARNING!!] Final check: There is no hist",item[2],"in",region,mass,channel,item[0],".") # Final check
              #print("Making a makeup hist(=cc of nominal)...") # NOTE in principle, this shouldn't lead to any error. No meaning to make e.g. fake_MuonSFUp
              #h_syst = input_list[i][1].Clone()
          
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

  # Finally, save the exception rules
  exceptionTag = args.exceptionTag if args.exceptionTag else OutputName

  code = generate_exception_code(Except_list)
  #save_path = "/data6/Users/jihkim/LatestCombine/CMSSW_14_1_0_pre4/src/DilepHN/exceptions_auto.py"
  save_path = "/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/exceptions_auto.py"
  
  if args.saveException == "Print":
    print("Printing exception rules ...")
    print(code)
  else:
    write_exceptions_module(save_path, code, args.saveException, exceptionTag)
    print("Exception rules are saved into ---------->",save_path)
