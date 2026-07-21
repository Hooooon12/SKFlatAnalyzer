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
parser.add_argument('-o', dest='outputTag', default='', help='tag attached to the output files (this is a part of the MergedFiles path)')
parser.add_argument('-T', dest='TestTag', default='', help='tag attached to each Tests e.g. fit tests, syst decorrelation tests, etc.')
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
parser.add_argument('--Unblind', action='store_true', help='Unblind data in SR')
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
parser.add_argument('--regions', nargs='+', default=None, help='regions to run, e.g. sr2 or zg_cr')
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
TestTag = args.TestTag if args.TestTag == '' else "_"+args.TestTag
ExtTag = '_Ext' if args.Ext else ''

BDTver = args.BDTver
ANver = int(re.search(r'\bANv(\d+)(?=_|$)', inputTag).group(1)) # ANv + some number + _ or end of the string
PRver = int(PRverMatch.group(1)) if (PRverMatch := re.search(r'PR(\d+)', inputTag)) else -1 # return PRver if it is included in the inputTag, else None.

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

def should_skip_limit_point_before_build(mass, region):
  """
  Keep this synchronized with the original era-by-era MakeInput skip rules.

  Original rule:
    - M <= 100: do not make r1/r2
    - M > 3000: do not make r1
  """

  if mass == "Weinberg":
    return False

  mass_int = int(mass.replace("M",""))

  if ("r1" in region or "r2" in region) and (mass_int <= 100):
    return True

  if ("r1" in region) and (mass_int > 3000):
    return True

  return False


def should_skip_limit_channel_before_build(mass, channel):
  """
  Keep this synchronized with the original channel skip rule.

  Original rule:
    - M > 30000: only EMu is made
  """

  if mass == "Weinberg":
    return False

  mass_int = int(mass.replace("M",""))

  if "EMu" not in channel and (mass_int > 30000):
    return True

  return False


def should_skip_known_missing_fake_phase_space(mass, region):
  """
  Phase-space where the source era card inputs are intentionally absent
  because fake is not produced / not available.
  """

  if mass == "Weinberg" and region == "sr1":
    return True

  return False

RUN2_SOURCE_ERAS = ["2016preVFP", "2016postVFP", "2017", "2018"]

def expand_run2_eras(eras):
  expanded = []
  for era in eras:
    if era == "Run2":
      expanded += RUN2_SOURCE_ERAS
    else:
      expanded.append(era)

  # preserve order and remove duplicates
  out = []
  for era in expanded:
    if era not in out:
      out.append(era)
  return out

# This will do necessary hadd for you.
targets = ['Data','Fake','CF','Conv','Prompt','MC','Signal']
any_target_selected = any(getattr(args, t) for t in targets)

if not args.Merge and any_target_selected:
  parser.error("You used --Data/--Fake/... without --Merge. Add --Merge.")

def merge_or_copy_root(out_file, in_files):
  """
  If there is only one exact input file, copy it.  Otherwise use hadd -f.
  This keeps individual MC merging cheap while preserving hadd for grouped MCs.
  """
  in_files = [x for x in in_files if x]
  os.system("mkdir -p " + os.path.dirname(out_file))

  if os.path.exists(out_file):
    os.system("rm " + out_file)

  if len(in_files) == 0:
    print("[merge_or_copy_root][WARNING] No inputs for", out_file)
    return 1

  if len(in_files) == 1 and os.path.exists(in_files[0]):
    cmd = "cp " + in_files[0] + " " + out_file
  else:
    cmd = "hadd -f " + out_file + " " + " ".join(in_files)

  print("[merge_or_copy_root]", cmd)
  return os.system(cmd)

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

if args.Merge and "Run2" in args.eras:
  parser.error(
    "Do not use -e Run2 with --Merge. "
    "Run2 limit-input mode should keep era-split processes. "
    "First run --Merge for 2016preVFP/2016postVFP/2017/2018, "
    "then run -e Run2 without --Merge to synthesize Run2 card inputs."
  )

if args.CR:
  Blinded = False # Blinded --> the total background will be used as data_obs
  DefFlags = ["MultiLepton__"]
  Analyzer = "HNL_ControlRegion_Plotter"

  #regions = ["cr1_inv","cr2_inv","cr3_inv","cf_cr1","cf_cr2","cf_cr3","ww_cr1","ww_cr2","zg_cr3","wz_cr1","wz_cr2","wz_cr3","zz_cr1","zz_cr2","zz_cr3"] if not args.Merge else "" # for CRs
  regions = ["cr1_InvMET","cr2_InvMET","cr3_InvMET","cr1_InvBJet","cr2_InvBJet","cr3_InvBJet","zg_cr","wz_cr1","wz_cr2","wz_cr3","zz_cr"] if not args.Merge else "" # for CRs
  #regions = ["cr2_InvBJet"] if not args.Merge else "" # for CRs
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
  Blinded = not args.Unblind # if Blinded --> the total background will be used as data_obs
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

# Optional region filter for card-input production.
# Example: --regions sr2
if args.regions:
  if args.Merge:
    parser.error("--regions is intended for card-input production, not --Merge.")

  bad_regions = [r for r in args.regions if r not in regions]
  if bad_regions:
    parser.error(
      "Unknown region(s): "
      + ",".join(bad_regions)
      + ". Allowed regions in this mode are: "
      + ",".join(regions)
    )

  regions = args.regions

SystList = [
            ## Separate JES <-- deprecated.
            #"JetAbsoluteMPFBiasUp","JetAbsoluteMPFBiasDown",
            #"JetAbsoluteScaleUp",  "JetAbsoluteScaleDown",  
            #"JetAbsoluteStatUp",   "JetAbsoluteStatDown",   
            #"JetFlavorQCDUp",      "JetFlavorQCDDown",      
            #"JetFragmentationUp",  "JetFragmentationDown",  
            #"JetPileUpDataMCUp",   "JetPileUpDataMCDown",   
            #"JetPileUpPtBBUp",     "JetPileUpPtBBDown",     
            #"JetPileUpPtEC1Up",    "JetPileUpPtEC1Down",    
            #"JetPileUpPtEC2Up",    "JetPileUpPtEC2Down",    
            #"JetPileUpPtHFUp",     "JetPileUpPtHFDown",     
            #"JetPileUpPtRefUp",    "JetPileUpPtRefDown",    
            #"JetRelativeBalUp",    "JetRelativeBalDown",    
            #"JetRelativeFSRUp",    "JetRelativeFSRDown",    
            #"JetRelativeJEREC1Up", "JetRelativeJEREC1Down", 
            #"JetRelativeJEREC2Up", "JetRelativeJEREC2Down", 
            #"JetRelativePtBBUp",   "JetRelativePtBBDown",   
            #"JetRelativePtEC1Up",  "JetRelativePtEC1Down",  
            #"JetRelativePtEC2Up",  "JetRelativePtEC2Down",  
            #"JetRelativePtHFUp",   "JetRelativePtHFDown",   
            #"JetRelativeSampleUp", "JetRelativeSampleDown", 
            #"JetRelativeStatECUp", "JetRelativeStatECDown", 
            #"JetRelativeStatFSRUp","JetRelativeStatFSRDown",
            #"JetRelativeStatHFUp", "JetRelativeStatHFDown", 
            #"JetSinglePionECALUp", "JetSinglePionECALDown",    
            #"JetSinglePionHCALUp", "JetSinglePionHCALDown",    
            #"JetTimePtEtaUp",      "JetTimePtEtaDown",         
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
            #"FRUp","FRDown", # fake rate stat
            #"FRRateUp","FRRateDown", # fake rate syst
            #"FRHighPtUp","FRHighPtDown",
            "FRMuonUp","FRMuonDown", # fake rate stat
            "FRMuonRateUp","FRMuonRateDown", # fake rate syst
            "FRMuonHighPtUp","FRMuonHighPtDown",
            "FRElectronUp","FRElectronDown", # fake rate stat
            "FRElectronRateUp","FRElectronRateDown", # fake rate syst
            "FRElectronHighPtUp","FRElectronHighPtDown",
            "PDFUp","PDFDown",
            #"ScaleUp","ScaleDown", <-- deprecated.
            "RenScaleUp","RenScaleDown",
            "FacScaleUp","FacScaleDown",
            "HEMJetUp","HEMJetDown",
           ]

SystNameMap = {}
for era in ["2016","2016preVFP","2016postVFP","2017","2018"]:
  SystNameMap[era] = {}

  ### Up variations
  ## Separate JES <-- deprecated.
  #SystNameMap[era]["JetAbsoluteMPFBiasUp"] = "CMS_scale_j_AbsoluteMPFBiasUp"
  #SystNameMap[era]["JetAbsoluteScaleUp"]   = "CMS_scale_j_AbsoluteScaleUp"
  #SystNameMap[era]["JetAbsoluteStatUp"]    = "CMS_scale_j_AbsoluteStat_"+era+"Up"
  #SystNameMap[era]["JetFlavorQCDUp"]       = "CMS_scale_j_FlavorQCDUp"
  #SystNameMap[era]["JetFragmentationUp"]   = "CMS_scale_j_FragmentationUp"
  #SystNameMap[era]["JetPileUpDataMCUp"]    = "CMS_scale_j_PileUpDataMCUp"
  #SystNameMap[era]["JetPileUpPtBBUp"]      = "CMS_scale_j_PileUpPtBBUp"
  #SystNameMap[era]["JetPileUpPtEC1Up"]     = "CMS_scale_j_PileUpPtEC1Up"
  #SystNameMap[era]["JetPileUpPtEC2Up"]     = "CMS_scale_j_PileUpPtEC2Up"
  #SystNameMap[era]["JetPileUpPtHFUp"]      = "CMS_scale_j_PileUpPtHFUp"
  #SystNameMap[era]["JetPileUpPtRefUp"]     = "CMS_scale_j_PileUpPtRefUp"
  #SystNameMap[era]["JetRelativeBalUp"]     = "CMS_scale_j_RelativeBalUp"
  #SystNameMap[era]["JetRelativeFSRUp"]     = "CMS_scale_j_RelativeFSRUp"
  #SystNameMap[era]["JetRelativeJEREC1Up"]  = "CMS_scale_j_RelativeJEREC1_"+era+"Up"
  #SystNameMap[era]["JetRelativeJEREC2Up"]  = "CMS_scale_j_RelativeJEREC2_"+era+"Up"
  #SystNameMap[era]["JetRelativePtBBUp"]    = "CMS_scale_j_RelativePtBBUp"
  #SystNameMap[era]["JetRelativePtEC1Up"]   = "CMS_scale_j_RelativePtEC1_"+era+"Up"
  #SystNameMap[era]["JetRelativePtEC2Up"]   = "CMS_scale_j_RelativePtEC2_"+era+"Up"
  #SystNameMap[era]["JetRelativePtHFUp"]    = "CMS_scale_j_RelativePtHFUp"
  #SystNameMap[era]["JetRelativeSampleUp"]  = "CMS_scale_j_RelativeSample_"+era+"Up"
  #SystNameMap[era]["JetRelativeStatECUp"]  = "CMS_scale_j_RelativeStatEC_"+era+"Up"
  #SystNameMap[era]["JetRelativeStatFSRUp"] = "CMS_scale_j_RelativeStatFSR_"+era+"Up"
  #SystNameMap[era]["JetRelativeStatHFUp"]  = "CMS_scale_j_RelativeStatHF_"+era+"Up"
  #SystNameMap[era]["JetSinglePionECALUp"]  = "CMS_scale_j_SinglePionECALUp"
  #SystNameMap[era]["JetSinglePionHCALUp"]  = "CMS_scale_j_SinglePionHCALUp"
  #SystNameMap[era]["JetTimePtEtaUp"]       = "CMS_scale_j_TimePtEta_"+era+"Up"
  ##
  SystNameMap[era]["JetResUp"]            = "CMS_res_j_"+era+"Up"
  SystNameMap[era]["JetEnUp"]             = "CMS_scale_j_"+era+"Up"
  SystNameMap[era]["JetPUIDUp"]           = "CMS_eff_j_PUJetID_"+era+"Up"
  SystNameMap[era]["JetPNETUp"]           = "CMS_eff_j_ParticleNet_W_NominalUp" if "PNETdecorr" not in args.TestTag else "CMS_eff_j_ParticleNet_W_Nominal_"+era+"Up"
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
  #SystNameMap[era]["FRUp"]                = "CMS_SUS24014_fake_stat_"+era+"Up"
  #SystNameMap[era]["FRRateUp"]            = "CMS_SUS24014_fake_syst_"+era+"Up"
  #SystNameMap[era]["FRHighPtUp"]          = "CMS_SUS24014_fake_highpt_"+era+"Up"
  SystNameMap[era]["FRMuonUp"]            = "CMS_SUS24014_fake_m_stat_"+era+"Up"
  SystNameMap[era]["FRMuonRateUp"]        = "CMS_SUS24014_fake_m_syst_"+era+"Up"
  SystNameMap[era]["FRMuonHighPtUp"]      = "CMS_SUS24014_fake_m_highpt_"+era+"Up"
  SystNameMap[era]["FRElectronUp"]        = "CMS_SUS24014_fake_e_stat_"+era+"Up"
  SystNameMap[era]["FRElectronRateUp"]    = "CMS_SUS24014_fake_e_syst_"+era+"Up"
  SystNameMap[era]["FRElectronHighPtUp"]  = "CMS_SUS24014_fake_e_highpt_"+era+"Up"
  SystNameMap[era]["PDFUp"]               = "pdf"+"Up" # full correlation
  #SystNameMap[era]["ScaleUp"]             = "QCDscale"+"Up" # full correlation <-- deprecated.
  SystNameMap[era]["RenScaleUp"]          = "RenScale"+"Up" # full correlation
  SystNameMap[era]["FacScaleUp"]          = "FacScale"+"Up" # full correlation
  SystNameMap[era]["HEMJetUp"]            = "CMS_HEM_"+era+"Up"

  ### Down variations
  ## Separate JES <-- deprecated.
  #SystNameMap[era]["JetAbsoluteMPFBiasDown"] = "CMS_scale_j_AbsoluteMPFBiasDown"
  #SystNameMap[era]["JetAbsoluteScaleDown"]   = "CMS_scale_j_AbsoluteScaleDown"
  #SystNameMap[era]["JetAbsoluteStatDown"]    = "CMS_scale_j_AbsoluteStat_"+era+"Down"
  #SystNameMap[era]["JetFlavorQCDDown"]       = "CMS_scale_j_FlavorQCDDown"
  #SystNameMap[era]["JetFragmentationDown"]   = "CMS_scale_j_FragmentationDown"
  #SystNameMap[era]["JetPileUpDataMCDown"]    = "CMS_scale_j_PileUpDataMCDown"
  #SystNameMap[era]["JetPileUpPtBBDown"]      = "CMS_scale_j_PileUpPtBBDown"
  #SystNameMap[era]["JetPileUpPtEC1Down"]     = "CMS_scale_j_PileUpPtEC1Down"
  #SystNameMap[era]["JetPileUpPtEC2Down"]     = "CMS_scale_j_PileUpPtEC2Down"
  #SystNameMap[era]["JetPileUpPtHFDown"]      = "CMS_scale_j_PileUpPtHFDown"
  #SystNameMap[era]["JetPileUpPtRefDown"]     = "CMS_scale_j_PileUpPtRefDown"
  #SystNameMap[era]["JetRelativeBalDown"]     = "CMS_scale_j_RelativeBalDown"
  #SystNameMap[era]["JetRelativeFSRDown"]     = "CMS_scale_j_RelativeFSRDown"
  #SystNameMap[era]["JetRelativeJEREC1Down"]  = "CMS_scale_j_RelativeJEREC1_"+era+"Down"
  #SystNameMap[era]["JetRelativeJEREC2Down"]  = "CMS_scale_j_RelativeJEREC2_"+era+"Down"
  #SystNameMap[era]["JetRelativePtBBDown"]    = "CMS_scale_j_RelativePtBBDown"
  #SystNameMap[era]["JetRelativePtEC1Down"]   = "CMS_scale_j_RelativePtEC1_"+era+"Down"
  #SystNameMap[era]["JetRelativePtEC2Down"]   = "CMS_scale_j_RelativePtEC2_"+era+"Down"
  #SystNameMap[era]["JetRelativePtHFDown"]    = "CMS_scale_j_RelativePtHFDown"
  #SystNameMap[era]["JetRelativeSampleDown"]  = "CMS_scale_j_RelativeSample_"+era+"Down"
  #SystNameMap[era]["JetRelativeStatECDown"]  = "CMS_scale_j_RelativeStatEC_"+era+"Down"
  #SystNameMap[era]["JetRelativeStatFSRDown"] = "CMS_scale_j_RelativeStatFSR_"+era+"Down"
  #SystNameMap[era]["JetRelativeStatHFDown"]  = "CMS_scale_j_RelativeStatHF_"+era+"Down"
  #SystNameMap[era]["JetSinglePionECALDown"]  = "CMS_scale_j_SinglePionECALDown"
  #SystNameMap[era]["JetSinglePionHCALDown"]  = "CMS_scale_j_SinglePionHCALDown"
  #SystNameMap[era]["JetTimePtEtaDown"]       = "CMS_scale_j_TimePtEta_"+era+"Down"
  ##
  SystNameMap[era]["JetResDown"]            = "CMS_res_j_"+era+"Down"
  SystNameMap[era]["JetEnDown"]             = "CMS_scale_j_"+era+"Down"
  SystNameMap[era]["JetPUIDDown"]           = "CMS_eff_j_PUJetID_"+era+"Down"
  SystNameMap[era]["JetPNETDown"]           = "CMS_eff_j_ParticleNet_W_NominalDown" if "PNETdecorr" not in args.TestTag else "CMS_eff_j_ParticleNet_W_Nominal_"+era+"Down"
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
  #SystNameMap[era]["FRDown"]                = "CMS_SUS24014_fake_stat_"+era+"Down"
  #SystNameMap[era]["FRRateDown"]            = "CMS_SUS24014_fake_syst_"+era+"Down"
  #SystNameMap[era]["FRHighPtDown"]          = "CMS_SUS24014_fake_highpt_"+era+"Down"
  SystNameMap[era]["FRMuonDown"]            = "CMS_SUS24014_fake_m_stat_"+era+"Down"
  SystNameMap[era]["FRMuonRateDown"]        = "CMS_SUS24014_fake_m_syst_"+era+"Down"
  SystNameMap[era]["FRMuonHighPtDown"]      = "CMS_SUS24014_fake_m_highpt_"+era+"Down"
  SystNameMap[era]["FRElectronDown"]        = "CMS_SUS24014_fake_e_stat_"+era+"Down"
  SystNameMap[era]["FRElectronRateDown"]    = "CMS_SUS24014_fake_e_syst_"+era+"Down"
  SystNameMap[era]["FRElectronHighPtDown"]  = "CMS_SUS24014_fake_e_highpt_"+era+"Down"
  SystNameMap[era]["PDFDown"]               = "pdf"+"Down" # full correlation
  #SystNameMap[era]["ScaleDown"]             = "QCDscale"+"Down" # full correlation <-- deprecated.
  SystNameMap[era]["RenScaleDown"]          = "RenScale"+"Down" # full correlation
  SystNameMap[era]["FacScaleDown"]          = "FacScale"+"Down" # full correlation
  SystNameMap[era]["HEMJetDown"]            = "CMS_HEM_"+era+"Down"

  # SR-decorrelated sources -- Don't remove this, it is used below
  SystNameMap[era]["CFRate"]            = "CMS_SUS24014_cf_stat_"+era
  #SystNameMap[era]["FR"]                = "CMS_SUS24014_fake_stat_"+era
  #SystNameMap[era]["FRRate"]            = "CMS_SUS24014_fake_syst_"+era
  #SystNameMap[era]["FRHighPt"]          = "CMS_SUS24014_fake_highpt_"+era
  SystNameMap[era]["FRMuon"]            = "CMS_SUS24014_fake_m_stat_"+era
  SystNameMap[era]["FRMuonRate"]        = "CMS_SUS24014_fake_m_syst_"+era
  SystNameMap[era]["FRMuonHighPt"]      = "CMS_SUS24014_fake_m_highpt_"+era
  SystNameMap[era]["FRElectron"]        = "CMS_SUS24014_fake_e_stat_"+era
  SystNameMap[era]["FRElectronRate"]    = "CMS_SUS24014_fake_e_syst_"+era
  SystNameMap[era]["FRElectronHighPt"]  = "CMS_SUS24014_fake_e_highpt_"+era
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

## inclusive conversion
MergeList['RunConv']['Conv_inc']      = ["TG","TTG","WZG","WWG","WGToLNuG","WGToLNuG_MG","WGToLNuG_01J_PtG_130","WGToLNuG_01J_PtG_300","WGToLNuG_01J_PtG_500","WGJJToLNu","ZGToLLG","ZGToLLG_PtG_130","DYJets_MG","DYJets10to50_MG"] #FIXME time to time
# Remove DY 10 to 50
if "NoLowDYMG" in outputTag:
  MergeList['RunConv']['Conv_inc'].remove("DYJets10to50_MG")
# Use MiNNLO DY
if "DYConvUpdate" in inputTag:
  MergeList['RunConv']['Conv_inc'] = [x for x in MergeList['RunConv']['Conv_inc'] if x not in ["DYJets_MG","DYJets10to50_MG"]]
  MergeList['RunConv']['Conv_inc'].extend(["DYJetsToEE_MiNNLO","DYJetsToMuMu_MiNNLO","DYJetsToTauTau_MiNNLO"])
if "NoTGTTG" in outputTag:
  MergeList['RunConv']['Conv_inc'].remove("TG")
  MergeList['RunConv']['Conv_inc'].remove("TTG")

## ZG normalization
MergeList['RunConv']['ZG_norm']       = ["ZGToLLG","ZGToLLG_PtG_130","DYJets_MG","DYJets10to50_MG"]
# Remove DY 10 to 50
if "NoLowDYMG" in outputTag:
  MergeList['RunConv']['ZG_norm'].remove("DYJets10to50_MG")
# Use MiNNLO DY
if "DYConvUpdate" in inputTag:
  MergeList['RunConv']['ZG_norm'] = [x for x in MergeList['RunConv']['ZG_norm'] if x not in ["DYJets_MG","DYJets10to50_MG"]]
  MergeList['RunConv']['ZG_norm'].extend(["DYJetsToEE_MiNNLO","DYJetsToMuMu_MiNNLO","DYJetsToTauTau_MiNNLO"])

## inclusive - ZG
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
                                        'ZZTo4L_powheg','GluGluToZZto4e','GluGluToZZto4mu','GluGluToZZto2e2mu','GluGluToZZto2e2tau','GluGluToZZto2mu2tau','GluGluToZZto4tau',
                                        #WZ
                                        'WZTo3LNu_amcatnlo','WZ_EWK', # 'WZTo3LNu_mllmin4p0_powheg' : amcatnlo gives better control in Inverted CR3
                                       ] #FIXME time to time
MergeList['RunPrompt']['ZZ_norm']       = ["ZZTo4L_powheg","GluGluToZZto4e","GluGluToZZto4mu","GluGluToZZto2e2mu","GluGluToZZto2e2tau","GluGluToZZto2mu2tau","GluGluToZZto4tau"] #FIXME time to time
MergeList['RunPrompt']['WZ']            = ["WZTo3LNu_amcatnlo"]
MergeList['RunPrompt']['WZ_EWK']        = ["WZ_EWK"]
#MergeList['RunPrompt']['WZ_norm']       = ["WZTo3LNu_amcatnlo","WZ_EWK"] if not ("EMuCF" in inputTag) else ["WZTo3LNu_amcatnlo"] #FIXME time to time
#MergeList['RunPrompt']['WZ_norm_powheg']         = ["WZTo3LNu_mllmin4p0_powheg","WZ_EWK"] #FIXME time to time
#MergeList['RunPrompt']['WZ_norm_amcatnlo']       = ["WZTo3LNu_amcatnlo","WZ_EWK"] #FIXME time to time
MergeList['RunPrompt']['WW_norm']       = ["WpWp_QCD","WpWp_EWK"] #FIXME time to time
MergeList['RunPrompt']['Prompt_others'] = [
                                           x for x in MergeList['RunPrompt']['Prompt_inc']
                                           if x not in MergeList['RunPrompt']['ZZ_norm']
                                           and x not in MergeList['RunPrompt']['WZ']
                                           and x not in MergeList['RunPrompt']['WZ_EWK']
                                           #and x not in MergeList['RunPrompt']['WZ_norm']
                                           #and x not in MergeList['RunPrompt']['WZ_norm_powheg']
                                           #and x not in MergeList['RunPrompt']['WZ_norm_amcatnlo']
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
  for this_conv in ["DYJetsToEE_MiNNLO","DYJetsToMuMu_MiNNLO","DYJetsToTauTau_MiNNLO"]:
    ConvSkim[DefFlag][this_conv] = "_SkimTree_HNMultiLepBDT_"
  for this_conv in ["WGToLNuG","WGToLNuG_MG","WGToLNuG_01J_PtG_130","WGToLNuG_01J_PtG_300","WGToLNuG_01J_PtG_500","WGJJToLNu"]:
    ConvSkim[DefFlag][this_conv] = "_SkimTree_DileptonBDT_"
PromptSkim = {}
for DefFlag in DefFlags:
  PromptSkim[DefFlag] = {}
  for this_prompt in [*(x for x in MergeList['RunPrompt']['Prompt_inc'] if x != "ZZTo4L_powheg"), "WZTo3LNu_mllmin4p0_powheg"]:
    PromptSkim[DefFlag][this_prompt] = "_SkimTree_HNMultiLepBDT_"
  for this_prompt in ["ZZTo4L_powheg"]:
    PromptSkim[DefFlag][this_prompt] = "_SkimTree_HNMultiLepBDT_" if args.CR else "_SkimTree_SSDileptonBDT_"
MCSkim = {DefFlag: {**ConvSkim[DefFlag], **PromptSkim[DefFlag]} for DefFlag in DefFlags}

# ----------------------------------------------------------------------
# MC bookkeeping for flat diagnostic histograms
# ----------------------------------------------------------------------
# Keep every original MC sample as a flat diagnostic histogram, while the
# actual Combine-rate processes are still the aggregated groups below.
MC_INDIVIDUAL_PROCS = MergeList['MC']['MC_inc'][:]

MC_COMPONENTS = {
  "conv_inc":       MergeList['RunConv']['Conv_inc'][:],
  "conv_others":    MergeList['RunConv']['Conv_others'][:],
  "prompt_inc":     MergeList['RunPrompt']['Prompt_inc'][:],
  "prompt_others":  MergeList['RunPrompt']['Prompt_others'][:],
  "mc_inc":         MergeList['MC']['MC_inc'][:],
  "mc_others":      MergeList['MC']['MC_others'][:],
  "zg":             MergeList['RunConv']['ZG_norm'][:],
  "zz":             MergeList['RunPrompt']['ZZ_norm'][:],
  "wz":             MergeList['RunPrompt']['WZ'][:],
  "wz_ewk":         MergeList['RunPrompt']['WZ_EWK'][:],
  "ww":             MergeList['RunPrompt']['WW_norm'][:],
}

# These are the backgrounds that enter the datacard rate model.
CARD_BKG_PROCS = ["fake", "cf", "zg", "zz", "wz", "wz_ewk", "ww", "mc_others"]

# These are summary / audit histograms.  They are useful in the flat ROOT file,
# but they should not be used to make NoNOM datacard exceptions unless they are
# also in CARD_BKG_PROCS.
SUMMARY_MC_PROCS = [
  "conv_inc", "conv_others", "prompt_inc", "prompt_others", "mc_inc",
  "zg", "zz", "wz", "wz_ewk", "ww", "mc_others",
]

DIAGNOSTIC_ONLY_PROCS = set(MC_INDIVIDUAL_PROCS + [
  "conv_inc", "conv_others", "prompt_inc", "prompt_others", "mc_inc",
])

# signalDYVBF is kept for comparison/diagnostics, but the datacard is expected
# to use the split DY and VBF signals.
NOM_EXCEPTION_PROCS = set(CARD_BKG_PROCS + [
  "signalDY", "signalVBF", "signalSSWW", "signalWeinberg",
])

KEEP_MC_INDIVIDUAL_PROCS = True
KEEP_MC_SUMMARY_PROCS = True

# Let --Merge --MC also create one-file-per-MC-sample outputs in MergeMC__.
# For a single component this will be copied rather than hadd'ed by the helper
# below, so this is cheap and follows the signal merge style.
for _mc_proc in MC_INDIVIDUAL_PROCS:
  MergeList['MC'].setdefault(_mc_proc, [_mc_proc])


# Siganl skims
SignalSkim = {}
for DefFlag in DefFlags:
  SignalSkim[DefFlag] = "_SkimTree_HNMultiLepBDT_"


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
  SignalList = [f"DYTypeI_DF_M{mass}_private" for mass in [85, 90, 95, 100, 125, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1500, 1700, 2000, 2500, 3000]] +\
               [f"VBFTypeI_DF_M{mass}_private" for mass in [300, 350, 400, 450, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1500, 1700, 2000, 2500, 3000]] +\
               [f"SSWWTypeI_SF_M{mass}_private" for mass in [500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1500, 1700, 2000, 2500, 3000, 5000, 7500, 10000, 15000, 20000, 25000, 30000]] +\
               [f"SSWWTypeI_DF_M{mass}_private" for mass in [500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1500, 1700, 2000, 2500, 3000, 5000, 7500, 10000, 15000, 20000, 25000, 30000, 40000, 50000, 60000]] +\
               [f"SSWWjj_DIM5_WeinbergOpt_{channel}_private" for channel in ["MuMu", "EE", "EMu"]]

  DefFlags = ["","MultiLepton__"]
  DefFlags_CR = ["MultiLepton__"]
  SRPath = "/data9/Users/HNL_public/SUS-24-014/SKFlatOutput/Systematic_Run/HNL_SignalRegion_Plotter_"+inputTag
  CRPath = "/data9/Users/HNL_public/SUS-24-014/SKFlatOutput/Systematic_Run/HNL_ControlRegion_Plotter_"+inputTag

  for era in expand_run2_eras(args.eras):
    if not args.CR:
      if Blinded: pass
      else:
        for this_proc in DataList[era]:
          this_path=SRPath + "/" + era + "/" + PreFlag+PostFlag+"/DATA/HNL_SignalRegion_Plotter_HNMultiLepBDT_"+this_proc+".root"
          if not os.path.exists(this_path):
            print(this_path,"-->",os.path.exists(this_path)) # these are data
      for this_proc in DataList[era]:
        this_path=SRPath + "/" + era + "/" + PreFlag+"RunFake__"+PostFlag+"/DATA/HNL_SignalRegion_Plotter_SkimTree_HNMultiLepBDT_"+this_proc+".root"
        if not os.path.exists(this_path):
          print(this_path,"-->",os.path.exists(this_path))
      for this_proc in DataList[era]:
        #if ("EMuCF" in inputTag and "DoubleMuon" in this_proc) or ("EMuCF" not in inputTag and "Muon" in this_proc): continue
        if "DoubleMuon" in this_proc: continue
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
      for this_proc in SignalList:
        this_path=SRPath + "/" + era + "/" + PreFlag+"RunSignal__"+PostFlag+"/HNL_SignalRegion_Plotter"+SignalSkim[""]+this_proc+".root"
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
        #if ("EMuCF" in inputTag and "DoubleMuon" in this_proc) or ("EMuCF" not in inputTag and "Muon" in this_proc): continue
        if "DoubleMuon" in this_proc: continue
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
      #for this_proc in SignalList:
      #  for DefFlag in DefFlags_CR:
      #    this_path=CRPath + "/" + era + "/" + PreFlag+DefFlag+"RunSignal__"+PostFlag+"/HNL_ControlRegion_Plotter"+SignalSkim[DefFlag]+this_proc+".root"
      #    if not os.path.exists(this_path):
      #      print(this_path,"-->",os.path.exists(this_path)) # We could include signals in CR but not urgent as the signal efficiency is < 5%

  exit()

if args.Merge:
  ##### Start merging #####
  if MergeData:
  
    if Blinded:
      print("[MergeData] Data blinded. skipping...")
    else:
      print("[MergeData] Data unblinded. merging...", flush=True)
      for era in args.eras:
        if era=="Run2": # Deprecated
          for DefFlag in DefFlags:
            os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/Run2/" + PreFlag+DefFlag +PostFlag+"/DATA/")
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/Run2/" + PreFlag+DefFlag +PostFlag+ "/DATA/"+Analyzer+DataSkim+"DATA.root"
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
            os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/" + era + "/" + PreFlag+DefFlag +PostFlag+"/DATA/")
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/" + era + "/" + PreFlag+DefFlag +PostFlag+ "/DATA/"+Analyzer+DataSkim+"DATA.root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/"+era+"/" + PreFlag+DefFlag +PostFlag+ "/DATA/*")
  
  if MergeFake:
  
    for era in args.eras:
      if era=="Run2":
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/Run2/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/")
          OutFile=MainPath + "/MergedFiles/" +Analyzer+"_"+inputTag+outputTag+ "/Run2/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/"+Analyzer+FakeSkim+"Fake.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2016preVFP/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2016postVFP/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2017/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2018/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*"\
          )
      else:
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/")
          OutFile=MainPath + "/MergedFiles/" +Analyzer+"_"+inputTag +outputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/"+Analyzer+FakeSkim+"Fake.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+ "/" + era+"/" + PreFlag+DefFlag + "RunFake__"+PostFlag+"/DATA/*")
  
  if MergeCF:
  
    for era in args.eras:
      if era=="Run2":
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/Run2/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/")
          OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/Run2/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/"+Analyzer+CFSkim+"CF.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2016preVFP/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2016postVFP/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2017/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*"\
                                      + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2018/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*"\
          )
      else:
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/")
          OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/"+ era + "/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/"+Analyzer+CFSkim+"CF.root"
          if os.path.exists(OutFile):
            os.system("rm " + OutFile)
          os.system("hadd " + OutFile + " " + SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/"+ era+"/" + PreFlag+DefFlag + "RunCF__"+PostFlag+"/DATA/*") 
  
  if MergeConv:
  
    for era in args.eras:
      if era=="Run2":
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/Run2/" + PreFlag+DefFlag + "RunConv__"+PostFlag)
          for OutProc in list(MergeList['RunConv'].keys()):
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/Run2/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2016preVFP/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2016postVFP/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2017/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2018/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]])
            )
      else:
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunConv__"+PostFlag)
          for OutProc in list(MergeList['RunConv'].keys()):
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/" +era+"/" + PreFlag+DefFlag + "RunConv__"+PostFlag+"/"+Analyzer+ConvSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunConv'][OutProc]]))
  
  if MergePrompt:
  
    for era in args.eras:
      if era=="Run2":
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/Run2/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag)
          for OutProc in list(MergeList['RunPrompt'].keys()):
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/Run2/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2016preVFP/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+PromptSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2016postVFP/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+PromptSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2017/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+PromptSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])
                                        + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/2018/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+PromptSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]])
            )
      else:
        for DefFlag in DefFlags:
          os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag)
          for OutProc in list(MergeList['RunPrompt'].keys()):
            OutFile=MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+"_"+OutProc+".root"
            if os.path.exists(OutFile):
              os.system("rm " + OutFile)
            os.system("hadd " + OutFile + " " + ' '.join([SKFlatOutputPath + "/"+ Analyzer+"_"+inputTag+"/" +era+"/" + PreFlag+DefFlag + "RunPrompt__"+PostFlag+"/"+Analyzer+PromptSkim[DefFlag][ThisProc]+ThisProc+".root" for ThisProc in MergeList['RunPrompt'][OutProc]]))
  
  if MergeMC:

    for era in args.eras:
      if era=="Run2":
        for DefFlag in DefFlags:
          out_dir = MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/Run2/" + PreFlag+DefFlag + "MergeMC__"+PostFlag
          os.system("mkdir -p " + out_dir)
          for OutProc in list(MergeList['MC'].keys()):
            OutFile = out_dir + "/" + Analyzer + "_" + OutProc + ".root"
            in_files = []
            for source_era in RUN2_SOURCE_ERAS:
              in_files += [
                SKFlatOutputPath
                + "/" + Analyzer + "_" + inputTag
                + "/" + source_era
                + "/" + PreFlag + DefFlag + MCFlag[ThisProc] + PostFlag
                + "/" + Analyzer + MCSkim[DefFlag][ThisProc] + ThisProc + ".root"
                for ThisProc in MergeList['MC'][OutProc]
              ]
            merge_or_copy_root(OutFile, in_files)
      else:
        for DefFlag in DefFlags:
          out_dir = MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/" + era + "/" + PreFlag+DefFlag + "MergeMC__"+PostFlag
          os.system("mkdir -p " + out_dir)
          for OutProc in list(MergeList['MC'].keys()):
            OutFile = out_dir + "/" + Analyzer + "_" + OutProc + ".root"
            in_files = [
              SKFlatOutputPath
              + "/" + Analyzer + "_" + inputTag
              + "/" + era
              + "/" + PreFlag + DefFlag + MCFlag[ThisProc] + PostFlag
              + "/" + Analyzer + MCSkim[DefFlag][ThisProc] + ThisProc + ".root"
              for ThisProc in MergeList['MC'][OutProc]
            ]
            merge_or_copy_root(OutFile, in_files)
  
  if MergeSignal:
  
    #if args.CR:
    #  print("##### This is CR setting.")
    #  print("##### Skipping signal merging ...")
    #  pass
    #else:
    for era in args.eras:
      for mass in args.masses:
        if era=="Run2":
          for DefFlag in DefFlags:
            os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/Run2/" + PreFlag+DefFlag + "RunSignal__"+PostFlag)
            OutFileDY    = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/Run2/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDY_"+mass+".root"
            OutFileVBF   = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/Run2/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalVBF_"+mass+".root"
            OutFileDYVBF = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/Run2/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDYVBF_"+mass+".root"
            OutFileSSWW  = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/Run2/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalSSWW_"+mass+".root"
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
            os.system("mkdir -p "+MainPath + "/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/" + era + "/" + PreFlag+DefFlag + "RunSignal__"+PostFlag)
  
            if mass=="Weinberg":
              OutFileWeinberg  = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+ "/" + era + "/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalWeinberg.root"
              # Merge Weinberg samples
              os.system("hadd -f " + OutFileWeinberg + " " + SKFlatOutputPath+"/"+Analyzer+"_"+inputTag+"/"+era+"/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/*Weinberg*")
            else:
              OutFileDY    = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/" + era + "/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDY_"+mass+".root"
              OutFileVBF   = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/" + era + "/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalVBF_"+mass+".root"
              OutFileDYVBF = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/" + era + "/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDYVBF_"+mass+".root"
              OutFileSSWW  = MainPath +"/MergedFiles/" + Analyzer+"_"+inputTag+outputTag+"/" + era + "/"+PreFlag+DefFlag+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalSSWW_"+mass+".root"
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

RUN2_PROCESS_BASES = [
  # utility / validation histograms
  "tot_bkg",

  # nominal backgrounds used by datacards
  "fake",
  "cf",
  "zg",
  "wz",
  "wz_ewk",
  "zz",
  "ww",
  "mc_others",

  # summary / audit backgrounds
  "conv_inc",
  "conv_others",
  "prompt_inc",
  "prompt_others",
  "mc_inc",

  # signals
  "signalDYVBF",
  "signalDY",
  "signalVBF",
  "signalSSWW",
  "signalWeinberg",
] + MC_INDIVIDUAL_PROCS

def split_process_and_syst_from_hist_name(hist_name):
  """
  Input examples:
    fake
    fake_CMS_SUS24014_fake_stat_2016preVFP_sr1Up
    wz_ewk_CMS_scale_j_2018Up
    signalDY_pdf_DYUp

  Returns:
    ("fake", "")
    ("fake", "CMS_SUS24014_fake_stat_2016preVFP_sr1Up")
    ("wz_ewk", "CMS_scale_j_2018Up")
    ("signalDY", "pdf_DYUp")
  """

  for proc in sorted(RUN2_PROCESS_BASES, key=len, reverse=True):
    if hist_name == proc:
      return proc, ""
    if hist_name.startswith(proc + "_"):
      return proc, hist_name[len(proc) + 1:]

  return None, None


def run2_renamed_hist_name(hist_name, source_era):
  """
  Convert an era-card histogram name into a Run2-card histogram name.

  Examples:
    fake
      -> fake_2016preVFP

    fake_CMS_SUS24014_fake_stat_2016preVFP_sr1Up
      -> fake_2016preVFP_CMS_SUS24014_fake_stat_2016preVFP_sr1Up

    signalDY_pdf_DYUp
      -> signalDY_2016preVFP_pdf_DYUp
  """

  if hist_name == "data_obs":
    return None

  proc, syst_part = split_process_and_syst_from_hist_name(hist_name)
  if proc is None:
    raise RuntimeError(
      "[Run2Builder] Cannot identify the process part of histogram name: "
      + hist_name
      + ". Add this process to RUN2_PROCESS_BASES if it is a real Combine process."
    )

  proc_run2 = proc + "_" + source_era
  if syst_part == "":
    return proc_run2
  return proc_run2 + "_" + syst_part


def assert_same_binning(h_ref, h_new, context):
  if h_ref.GetNbinsX() != h_new.GetNbinsX():
    raise RuntimeError(
      "[Run2Builder] Incompatible nbins for "
      + context
      + ": "
      + str(h_ref.GetNbinsX())
      + " vs "
      + str(h_new.GetNbinsX())
    )

  ax_ref = h_ref.GetXaxis()
  ax_new = h_new.GetXaxis()

  # check lower edges including the upper edge at nbins+1
  for ibin in range(1, h_ref.GetNbinsX() + 2):
    if abs(ax_ref.GetBinLowEdge(ibin) - ax_new.GetBinLowEdge(ibin)) > 1e-9:
      raise RuntimeError(
        "[Run2Builder] Incompatible bin edge for "
        + context
        + " at edge "
        + str(ibin)
        + ": "
        + str(ax_ref.GetBinLowEdge(ibin))
        + " vs "
        + str(ax_new.GetBinLowEdge(ibin))
      )


def build_run2_card_input(OutputPath, region, mass, channel, ExtTag):
  """
  Build:
    LimitInputs/<OutputName>/Run2/<region>/<mass>_<channel>_card_input.root

  from existing:
    LimitInputs/<OutputName>/<era>/<region>/<mass>_<channel>_card_input.root

  Rule:
    data_obs: summed over eras
    all other histograms: copied with process-era names
  """

  run2_dir = os.path.join(OutputPath, "Run2", region)
  os.makedirs(run2_dir, exist_ok=True)

  out_name = os.path.join(run2_dir, mass + "_" + channel + ExtTag + "_card_input.root")

  source_inputs = []
  missing_inputs = []

  for source_era in RUN2_SOURCE_ERAS:
    in_name = os.path.join(
      OutputPath,
      source_era,
      region,
      mass + "_" + channel + ExtTag + "_card_input.root"
    )

    if os.path.exists(in_name):
      source_inputs.append((source_era, in_name))
    else:
      missing_inputs.append((source_era, in_name))

  # Case 1:
  # All four era inputs are absent.
  # This usually means this mass/channel/region was intentionally skipped
  # by the era-by-era producer, e.g. no fake hist.
  if len(source_inputs) == 0:
    print(
      "[Run2Builder] SKIP:",
      region,
      mass,
      channel,
      "has no source-era card inputs."
    )
    print(
      "[Run2Builder]       Treating this as an intentionally skipped phase-space."
    )
    return None

  # Case 2:
  # Some eras exist but some are missing.
  # This is dangerous: Run2 would silently drop an era.
  # Keep this fatal.
  if len(missing_inputs) > 0:
    msg = (
      "[Run2Builder] Partially missing source era card inputs for "
      + region + " " + mass + " " + channel + ".\n"
      + "This is not treated as an intentional skip, because at least one era exists.\n"
      + "Missing inputs:\n"
    )

    for source_era, missing_name in missing_inputs:
      msg += "  - " + source_era + ": " + missing_name + "\n"

    msg += "Existing inputs:\n"
    for source_era, existing_name in source_inputs:
      msg += "  - " + source_era + ": " + existing_name + "\n"

    raise RuntimeError(msg)

  h_data_sum = None
  hists_to_write = []

  for source_era, in_name in source_inputs:
    print("[Run2Builder] Reading", in_name)
    f_in = TFile.Open(in_name, "READ")

    if (not f_in) or f_in.IsZombie():
      raise RuntimeError("[Run2Builder] Cannot open " + in_name)

    for key in f_in.GetListOfKeys():
      old_name = key.GetName()
      obj = key.ReadObj()

      if (not obj) or (not obj.InheritsFrom("TH1")):
        continue

      if old_name == "data_obs":
        if h_data_sum is None:
          h_data_sum = obj.Clone("data_obs")
          h_data_sum.Reset("ICES")
          h_data_sum.SetDirectory(0)
        else:
          assert_same_binning(h_data_sum, obj, "data_obs " + source_era)

        h_data_sum.Add(obj)
        continue

      new_name = run2_renamed_hist_name(old_name, source_era)

      h_new = obj.Clone(new_name)
      h_new.SetName(new_name)
      h_new.SetTitle(new_name)
      h_new.SetDirectory(0)
      hists_to_write.append(h_new)

    f_in.Close()

  if h_data_sum is None:
    raise RuntimeError(
      "[Run2Builder] No data_obs was found while building "
      + out_name
    )

  print("[Run2Builder] Writing", out_name)
  f_out = TFile.Open(out_name, "RECREATE")
  f_out.cd()

  h_data_sum.Write()
  for hist in hists_to_write:
    hist.Write()

  f_out.Close()

  print("[Run2Builder]", out_name, "has been created.")
  return out_name

def get_pdf_delta(bin_values, nom, pdf_mode=""):
    vals = np.asarray(bin_values, dtype=float)

    if pdf_mode == "replica":
        # MC replica / Monte Carlo PDF sets
        return np.std(vals, ddof=1)

    elif pdf_mode == "symmhessian":
        # Symmetric Hessian members around nominal
        diffs = vals - nom
        return np.sqrt(np.sum(diffs * diffs))

    elif pdf_mode == "hessian_pm":
        # Paired (+/-) Hessian eigenvectors:
        # delta = 1/2 * sqrt(sum_i (X_i^+ - X_i^-)^2)
        if len(vals) % 2 != 0:
            raise ValueError("hessian_pm mode requires an even number of PDF members")
        plus  = vals[0::2]
        minus = vals[1::2]
        return 0.5 * np.sqrt(np.sum((plus - minus) ** 2))

    else:
        raise ValueError(f"Unknown pdf_mode: {pdf_mode}")

def hist_integral_and_error(h, include_overflow=False):
  """
  Return sum of bin contents and quadrature-summed bin errors.

  Default follows TH1::Integral() convention for normal bins only:
    bins 1 ... nbins
  """
  if h is None:
    raise RuntimeError("[hist_integral_and_error] input histogram is None")

  first_bin = 0 if include_overflow else 1
  last_bin = h.GetNbinsX() + 1 if include_overflow else h.GetNbinsX()

  total = 0.
  err2 = 0.

  for ibin in range(first_bin, last_bin + 1):
    total += h.GetBinContent(ibin)
    err = h.GetBinError(ibin)
    err2 += err * err

  return total, np.sqrt(err2)


def make_cnc_hist(h_in, out_name):
  """
  Make a 1-bin cut-and-count histogram without modifying h_in.
  """
  total, err = hist_integral_and_error(h_in)

  h_out = TH1D(out_name, out_name, 1, 0., 1.)
  h_out.Sumw2()
  h_out.SetDirectory(0)
  h_out.SetBinContent(1, total)
  h_out.SetBinError(1, err)

  return h_out

def get_hist_from_input_list(input_list, hist_name):
  for item in reversed(input_list):
    if item[2] == hist_name:
      return item[1]
  return None


def is_valid_th1(h):
  return bool(h) and hasattr(h, "InheritsFrom") and h.InheritsFrom("TH1")


def clone_detached(h, out_name=None):
  if not is_valid_th1(h):
    return None

  h_out = h.Clone(out_name if out_name else h.GetName())
  if out_name:
    h_out.SetName(out_name)
    h_out.SetTitle(out_name)
  h_out.SetDirectory(0)
  return h_out


def get_hist_cached(cache, cache_key, f_root, h_path, hist_name):
  """
  Read a TH1 from ROOT only once per cache_key, then always return a detached clone.
  This is safe even when the caller later truncates/scales the returned histogram.
  """
  if cache_key in cache:
    return clone_detached(cache[cache_key], hist_name)

  if not f_root:
    cache[cache_key] = None
    return None

  h = CheckHist(f_root, h_path, hist_name)
  if is_valid_th1(h):
    h0 = clone_detached(h, hist_name)
    cache[cache_key] = clone_detached(h0, hist_name)
    return h0

  cache[cache_key] = None
  return None


def truncate_nonpositive_bins(h, label, zero_too=True, set_error_zero=True):
  """
  Generic bin-by-bin protection for templates that will be written to Combine.

  zero_too=True means both negative and exactly-zero bins get error=0.  This is
  intentional: a bin with zero expected yield should not carry a leftover MC stat
  error from the pre-truncated histogram.
  """
  if not is_valid_th1(h):
    return False

  changed = False
  for ibin in range(1, h.GetNbinsX() + 1):
    val = h.GetBinContent(ibin)
    should_zero = (val < 0.) or (zero_too and val == 0.)
    if should_zero:
      print("!!!!!! Non-positive bin detected in", label, "!!!!!!")
      print("!!!!!! bin", ibin, ":", val, "!!!!!!")
      print("!!!!!! Setting this bin content/error to 0 ...")
      h.SetBinContent(ibin, 0.)
      if set_error_zero:
        h.SetBinError(ibin, 0.)
      changed = True

  return changed


def treat_fake_zero_bins(h, label):
  """
  Keep the existing fake-specific prescription: non-positive fake bins are not
  set to zero, but to the small non-zero fake template value.
  """
  if not is_valid_th1(h):
    return False

  changed = False
  for ibin in range(1, h.GetNbinsX() + 1):
    if h.GetBinContent(ibin) <= 0.:
      print("!!!!!! zero fakes detected in", label, "!!!!!!")
      print("!!!!!! bin", ibin, ":", h.GetBinContent(ibin), "!!!!!!")
      h.SetBinContent(ibin, 0.15 * 0.645)
      h.SetBinError(ibin, 0.15 * 0.645)
      changed = True
  return changed


def append_or_replace_hist(input_list, f_path, h, name):
  if is_valid_th1(h):
    h.SetName(name)
    h.SetTitle(name)
    h.SetDirectory(0)

  for idx, item in enumerate(input_list):
    if item[2] == name:
      input_list[idx] = [f_path, h, name]
      return

  input_list.append([f_path, h, name])


def make_sum_hist(name_to_hist, component_names, out_name, label, missing_ok=True):
  tmpl = None
  for comp in component_names:
    h = name_to_hist.get(comp, None)
    if is_valid_th1(h):
      tmpl = h
      break

  if tmpl is None:
    if missing_ok:
      print("[make_sum_hist][WARNING] No valid template for", label, "components =", component_names)
      return None
    raise RuntimeError("[make_sum_hist] No valid template for " + label)

  h_sum = tmpl.Clone(out_name)
  h_sum.Reset("ICES")
  h_sum.SetName(out_name)
  h_sum.SetTitle(out_name)
  h_sum.SetDirectory(0)

  #print(
  #  "[DEBUG make_sum_hist]",
  #  label,
  #  "out =", out_name,
  #  "component =", comp,
  #  "nbins =", h.GetNbinsX(),
  #  "integral =", h.Integral()
  #)

  for comp in component_names:
    h = name_to_hist.get(comp, None)
    if not is_valid_th1(h):
      print("[make_sum_hist][WARNING]", label, "is missing component", comp, "; skipping it.")
      continue
    assert_same_binning(h_sum, h, label + " " + comp)
    h_sum.Add(h)

  return h_sum


def build_mc_summary_hists(input_list, summary_proc_names=SUMMARY_MC_PROCS):
  name_to_hist = {item[2]: item[1] for item in input_list}
  out = []
  for proc in summary_proc_names:
    if proc not in MC_COMPONENTS:
      continue
    h = make_sum_hist(
      name_to_hist,
      MC_COMPONENTS[proc],
      proc,
      "MC summary " + proc,
      missing_ok=True,
    )
    if is_valid_th1(h):
      truncate_nonpositive_bins(h, "MC summary " + proc, zero_too=True)
      out.append(["__aggregate__", h, proc])
  return out


def build_total_background(input_list, channel, out_name="tot_bkg"):
  used_bkgs = CARD_BKG_PROCS[:]
  if "MuMu" in channel and "cf" in used_bkgs:
    used_bkgs.remove("cf")

  name_to_hist = {item[2]: item[1] for item in input_list}
  h_tot = make_sum_hist(
    name_to_hist,
    used_bkgs,
    out_name,
    "total background " + out_name,
    missing_ok=False,
  )
  truncate_nonpositive_bins(h_tot, out_name, zero_too=True) # This is NOT the main truncation. Each process should have been truncated already. This is just a final fallback.
  return h_tot


def is_signal_process(proc):
  return proc.startswith("signal")


def signal_scale_factor(proc, is_Weinberg, DYVBFscaler, SSWWscaler, Weinbergscaler):
  if is_Weinberg:
    return Weinbergscaler if proc == "signalWeinberg" else 1.

  if proc in ["signalDYVBF", "signalDY", "signalVBF"]:
    return DYVBFscaler
  if proc == "signalSSWW":
    return SSWWscaler
  return 1.


def is_pdf_or_qcd_scale_syst(this_syst):
  return ("PDF" in this_syst) or (("Scale" in this_syst) and ("Jet" not in this_syst))


def pdf_scale_label_for_process(proc):
  if proc in ["wz", "WZTo3LNu_amcatnlo"]:
    return "WZ"
  if "DYVBF" in proc:
    return "DYVBF"
  if proc == "signalDY":
    return "DY"
  if proc == "signalVBF":
    return "VBF"
  if proc == "signalSSWW":
    return "SSWW"
  if proc == "signalWeinberg":
    return "Weinberg"
  return None


def should_make_syst_for_process(proc, this_syst):
  if not is_pdf_or_qcd_scale_syst(this_syst):
    return True
  return pdf_scale_label_for_process(proc) is not None


def output_syst_suffix(era, region, this_syst, proc):
  this_name_syst = SystNameMap[era][this_syst]

  if is_pdf_or_qcd_scale_syst(this_syst):
    label = pdf_scale_label_for_process(proc)
    if label is not None:
      this_name_syst = (
        this_name_syst
        .replace("pdf", "pdf_" + label)
        .replace("scale", "scale_" + label)
        .replace("Scale", "Scale_" + label)
      )

  if args.Decorr:
    if 'sr1' in region or 'cr1' in region:
      regionName_Decorr = '_sr1'
    elif 'sr2' in region or 'cr2' in region:
      regionName_Decorr = '_sr2'
    elif 'sr3' in region or 'cr3' in region:
      regionName_Decorr = '_sr3'
    else:
      regionName_Decorr = '_sr3' # correlate zg_cr, zz_cr to SR3

    DecorrList = [
      "CFRate", "FRMuon", "FRMuonRate", "FRMuonHighPt",
      "FRElectron", "FRElectronRate", "FRElectronHighPt",
    ] if not args.JetDecorr else [
      "CFRate", "FRMuon", "FRMuonRate", "FRMuonHighPt",
      "FRElectron", "FRElectronRate", "FRElectronHighPt",
      "JetRes", "JetEn",
    ]

    this_syst_source = this_syst.replace('Up', '').replace('Down', '')
    if this_syst_source in DecorrList:
      this_name_syst = (
        SystNameMap[era][this_syst_source]
        + regionName_Decorr
        + this_syst.replace(this_syst_source, '')
      )

  return this_name_syst


def pdf_mode_for_process(proc):
  if proc in ["signalDY", "signalSSWW", "signalWeinberg", "wz", "WZTo3LNu_amcatnlo"]:
    return "symmhessian"
  if proc in ["signalVBF", "signalDYVBF"]:
    return "replica"
  raise ValueError("Unknown pdf process: " + proc)


def add_no_nom_exception(Except_list, proc, tag, region, era, channel, mass, is_Weinberg, mass_int):
  if proc == "signalDYVBF":
    return
  if proc in DIAGNOSTIC_ONLY_PROCS:
    return
  if proc not in NOM_EXCEPTION_PROCS:
    return

  if "signal" in proc:
    Except_list.append((tag, proc, region, era, channel, mass))
  else:
    if (not is_Weinberg) and mass_int < 600:
      Except_list.append((tag, proc, region, era, channel, mass))
    else:
      Except_list.append((tag, proc, region, era, channel, "highmass"))


def should_symmetrize_zg_scale_j_2018_sr2_down(era, region, process, this_syst, name_syst):
  return (
    "PruneZG" in args.TestTag
    and not args.CR
    and args.Decorr
    and args.JetDecorr
    and era == "2018"
    and region == "sr2"
    and process == "zg"
    and this_syst == "JetEnDown"
    and name_syst == "zg_CMS_scale_j_2018_sr2Down"
  )


def symmetrize_down_from_up(h_down, h_nom, h_up, bins_to_fix, label):
  if h_down.GetNbinsX() != h_nom.GetNbinsX() or h_down.GetNbinsX() != h_up.GetNbinsX():
    raise RuntimeError("[PruneZG] Inconsistent binning for " + label)

  for ibin in bins_to_fix:
    if ibin < 1 or ibin > h_down.GetNbinsX():
      raise RuntimeError(
        "[PruneZG] Requested bin "
        + str(ibin)
        + " is outside histogram range for "
        + label
      )

    nom = max(0., h_nom.GetBinContent(ibin))
    up = max(0., h_up.GetBinContent(ibin))
    old_down = h_down.GetBinContent(ibin)
    new_down = max(0., 2. * nom - up)

    h_down.SetBinContent(ibin, new_down)
    h_down.SetBinError(ibin, h_up.GetBinError(ibin))

    print(
      "[PruneZG]",
      label,
      "bin",
      ibin,
      "nom =",
      nom,
      "up =",
      up,
      "old_down =",
      old_down,
      "new_down =",
      new_down
    )

# ----------------------------------------------------------------------
# Fit-stability test knobs controlled by -T / --TestTag.
#
# Example:
#   -T FitTest_LowStatNeff5_MergeSR2Bin78_MergeSR3EEBin1314_SmoothEE
#
# Tags:
#   LowStatNeff5        : if nominal Neff < 5 in a bin, set syst bin = nominal bin
#                         for MC-driven templates. You can use LowStatNeff10,
#                         LowStatNeff7p5, etc.
#   MergeSR2Bin78       : merge ROOT bins 7 and 8 in SR2.
#   MergeSR3EEBin1314   : merge ROOT bins 13 and 14 for EE, SR3, M125-M500.
#   SmoothEE            : apply conservative 1-2-1 smoothing to EE only:
#                         SR3 M125-M500 and SR2 all masses.
#                         SmoothEE2 means two smoothing iterations.
#   FillHoles           : after all requested bin treatments, fill zero/negative
#                         bins only for card-level MC backgrounds
#                         zg/zz/wz/wz_ewk/ww/mc_others.  Content and error are
#                         set to the same process-specific tiny value, so Neff=1.
#
# Optional extra tags:
#   LowStatSignal       : also apply LowStatNeff to signal templates.
#   SmoothFakeCF        : also smooth fake/cf templates.
#   SmoothIndividualMC  : also smooth individual MC diagnostic templates.
# ----------------------------------------------------------------------

def test_tag_has(tag):
  return tag in args.TestTag


def test_tag_float(prefix, default):
  """
  Parse e.g.
    LowStatNeff5   -> 5.0
    LowStatNeff7p5 -> 7.5
    SmoothEE2      -> 2.0
  If prefix exists without a number, return default.
  """
  m = re.search(re.escape(prefix) + r'([0-9]+(?:[p.][0-9]+)?)?', args.TestTag)
  if not m:
    return default

  value = m.group(1)
  if value is None or value == "":
    return default

  return float(value.replace("p", "."))


def test_tag_int(prefix, default):
  return int(round(test_tag_float(prefix, float(default))))


FITTEST_LOWSTAT_ACTIVE = test_tag_has("LowStatNeff")
FITTEST_LOWSTAT_NEFF_MIN = test_tag_float("LowStatNeff", 5.0)

FITTEST_MERGE_SR2_BIN78 = test_tag_has("MergeSR2Bin78")
FITTEST_MERGE_SR2_BIN3478 = test_tag_has("MergeSR2Bin3478")
FITTEST_MERGE_SR3_EE_BIN1314 = test_tag_has("MergeSR3EEBin1314")

FITTEST_SMOOTH_EE = test_tag_has("SmoothEE")
FITTEST_SMOOTH_NITER = max(1, test_tag_int("SmoothEE", 1))

FITTEST_FILLHOLES_ACTIVE = test_tag_has("FillHoles")
FITTEST_FILLHOLES_PROCS = ["zg", "zz", "wz", "wz_ewk", "ww", "mc_others"]

# Fill these by hand before running with -T FillHoles.
# Keep them positive.  The same number is used for bin content and bin error,
# therefore Neff = content^2/error^2 = 1 in every filled bin.
#
# Example:
#   "zg": 1e-9,
#   "zz": 1e-9,
#   ...
FITTEST_FILLHOLES_VALUES = {
  "zg":        0.001,
  "zz":        0.001,
  "wz":        0.001,
  "wz_ewk":    0.001,
  "ww":        0.001,
  "mc_others": 0.001,
}

if FITTEST_FILLHOLES_ACTIVE:
  _missing_fillhole_values = [
    proc for proc in FITTEST_FILLHOLES_PROCS
    if FITTEST_FILLHOLES_VALUES.get(proc, None) is None
  ]
  if _missing_fillhole_values:
    raise RuntimeError(
      "[FitTest][FillHoles] FillHoles is active, but no tiny fill value is set for: "
      + ", ".join(_missing_fillhole_values)
      + ". Edit FITTEST_FILLHOLES_VALUES in MakeInput_public.py before running."
    )

  for _proc in FITTEST_FILLHOLES_PROCS:
    FITTEST_FILLHOLES_VALUES[_proc] = float(FITTEST_FILLHOLES_VALUES[_proc])
    if FITTEST_FILLHOLES_VALUES[_proc] <= 0.:
      raise RuntimeError(
        "[FitTest][FillHoles] Fill value for "
        + _proc
        + " must be positive, got "
        + str(FITTEST_FILLHOLES_VALUES[_proc])
      )

  print(
    "[FitTest][FillHoles] Active. Target processes =",
    FITTEST_FILLHOLES_PROCS,
    "values =",
    FITTEST_FILLHOLES_VALUES
  )

# LowStatNeff is applied to MC-driven templates only by default.
# fake/cf/data are excluded. Signals are excluded unless LowStatSignal is used.
FITTEST_MC_SHAPE_PROCS = set(
  MC_INDIVIDUAL_PROCS
  + SUMMARY_MC_PROCS
  + ["zg", "zz", "wz", "wz_ewk", "ww", "mc_others"]
)

if test_tag_has("LowStatSignal"):
  FITTEST_MC_SHAPE_PROCS |= set([
    "signalDYVBF", "signalDY", "signalVBF", "signalSSWW", "signalWeinberg"
  ])

# Smoothing is applied only to card-level / summary MC backgrounds by default.
# This avoids changing data_obs, fake/cf, and signal shapes.
FITTEST_SMOOTH_PROCS = set(CARD_BKG_PROCS + SUMMARY_MC_PROCS) - set(["fake", "cf"])

if test_tag_has("SmoothFakeCF"):
  FITTEST_SMOOTH_PROCS |= set(["fake", "cf"])

if test_tag_has("SmoothIndividualMC"):
  FITTEST_SMOOTH_PROCS |= set(MC_INDIVIDUAL_PROCS)

if args.CnC and (FITTEST_MERGE_SR2_BIN78 or FITTEST_MERGE_SR2_BIN3478 or FITTEST_MERGE_SR3_EE_BIN1314 or FITTEST_SMOOTH_EE):
  print(
    "[FitTest][WARNING] Merge/smoothing tags are active, but --CnC writes "
    "one-bin histograms. Merge/smoothing will be skipped. "
    "Remove --CnC if you want to test bin-level changes."
  )


def nominal_bin_neff(h_nom, ibin):
  """
  Effective number of weighted events:
    Neff = content^2 / error^2

  If content <= 0, return 0 so that any non-zero syst excursion in a
  zero-nominal bin is killed.
  If error <= 0 and content > 0, treat as high-stat/infinite Neff.
  """
  val = h_nom.GetBinContent(ibin)
  err = h_nom.GetBinError(ibin)

  if val <= 0.:
    return 0.

  if err <= 0.:
    return float("inf")

  return (val / err) * (val / err)


def force_lowstat_syst_bins_to_nominal(h_syst, h_nom, proc, hist_name, label):
  """
  If nominal Neff is below threshold in a bin, set the systematic template bin
  to the nominal template bin. This is intentionally bin-by-bin, not whole-region.
  """
  if not FITTEST_LOWSTAT_ACTIVE:
    return False

  if proc not in FITTEST_MC_SHAPE_PROCS:
    return False

  if not is_valid_th1(h_syst) or not is_valid_th1(h_nom):
    return False

  if h_syst.GetNbinsX() != h_nom.GetNbinsX():
    raise RuntimeError(
      "[FitTest][LowStatNeff] Inconsistent binning for "
      + label + " " + proc + " " + hist_name
    )

  changed = []
  for ibin in range(1, h_nom.GetNbinsX() + 1):
    neff = nominal_bin_neff(h_nom, ibin)

    if neff < FITTEST_LOWSTAT_NEFF_MIN:
      h_syst.SetBinContent(ibin, h_nom.GetBinContent(ibin))
      h_syst.SetBinError(ibin, h_nom.GetBinError(ibin))
      changed.append((ibin, neff))

  if changed:
    preview = ", ".join([str(b) + ":" + format(neff, ".2f") for b, neff in changed[:12]])
    if len(changed) > 12:
      preview += ", ..."

    print(
      "[FitTest][LowStatNeff]",
      label,
      "proc =",
      proc,
      "hist =",
      hist_name,
      "threshold =",
      FITTEST_LOWSTAT_NEFF_MIN,
      "changed bins bin:Neff =",
      preview
    )

  return len(changed) > 0


def hist_bin_edges(h):
  ax = h.GetXaxis()
  return [ax.GetBinLowEdge(ibin) for ibin in range(1, h.GetNbinsX() + 2)]


def clone_with_merged_adjacent_bins(h_in, out_name, first_bin_to_merge, label):
  """
  Merge ROOT bins:
    first_bin_to_merge and first_bin_to_merge + 1

  Example:
    first_bin_to_merge = 7 merges bins 7 and 8.
  Bin errors are added in quadrature.
  """
  if not is_valid_th1(h_in):
    return None

  nbins_old = h_in.GetNbinsX()
  second_bin_to_merge = first_bin_to_merge + 1

  if first_bin_to_merge < 1 or second_bin_to_merge > nbins_old:
    raise RuntimeError(
      "[FitTest][MergeBins] Requested merge "
      + str(first_bin_to_merge)
      + ","
      + str(second_bin_to_merge)
      + " is outside histogram range for "
      + label
      + " with nbins = "
      + str(nbins_old)
    )

  old_edges = hist_bin_edges(h_in)

  # For ROOT bin i, the internal edge after bin i is old_edges[i].
  # To merge bin i and i+1, remove old_edges[i].
  new_edges = old_edges[:first_bin_to_merge] + old_edges[first_bin_to_merge + 1:]

  nbins_new = nbins_old - 1
  h_out = TH1D(out_name, out_name, nbins_new, array.array('d', new_edges))
  h_out.Sumw2()
  h_out.SetDirectory(0)
  h_out.SetName(out_name)
  h_out.SetTitle(out_name)

  # Copy underflow.
  h_out.SetBinContent(0, h_in.GetBinContent(0))
  h_out.SetBinError(0, h_in.GetBinError(0))

  old_i = 1
  new_i = 1

  while old_i <= nbins_old:
    if old_i == first_bin_to_merge:
      val = h_in.GetBinContent(old_i) + h_in.GetBinContent(old_i + 1)
      err = np.sqrt(
        h_in.GetBinError(old_i) * h_in.GetBinError(old_i)
        + h_in.GetBinError(old_i + 1) * h_in.GetBinError(old_i + 1)
      )

      h_out.SetBinContent(new_i, val)
      h_out.SetBinError(new_i, err)

      old_i += 2
      new_i += 1
    else:
      h_out.SetBinContent(new_i, h_in.GetBinContent(old_i))
      h_out.SetBinError(new_i, h_in.GetBinError(old_i))

      old_i += 1
      new_i += 1

  # Copy overflow.
  h_out.SetBinContent(nbins_new + 1, h_in.GetBinContent(nbins_old + 1))
  h_out.SetBinError(nbins_new + 1, h_in.GetBinError(nbins_old + 1))

  print(
    "[FitTest][MergeBins]",
    label,
    "merged bins",
    first_bin_to_merge,
    "and",
    second_bin_to_merge,
    "old nbins =",
    nbins_old,
    "new nbins =",
    nbins_new,
    "integral old =",
    h_in.Integral(),
    "integral new =",
    h_out.Integral()
  )

  return h_out


def merge_bins_in_input_list(input_list, first_bin_to_merge, label):
  for item in input_list:
    h = item[1]
    name = item[2]

    if not is_valid_th1(h):
      continue

    item[1] = clone_with_merged_adjacent_bins(
      h,
      name,
      first_bin_to_merge,
      label + " " + name
    )


def process_base_from_hist_name(hist_name):
  if hist_name == "data_obs":
    return None

  proc, _ = split_process_and_syst_from_hist_name(hist_name)
  return proc


def should_fillholes_rescue_nominal_proc(proc):
  return FITTEST_FILLHOLES_ACTIVE and proc in FITTEST_FILLHOLES_PROCS


def should_fillholes_hist_name(hist_name):
  if not FITTEST_FILLHOLES_ACTIVE:
    return False

  proc = process_base_from_hist_name(hist_name)
  if proc is None:
    return False

  return proc in FITTEST_FILLHOLES_PROCS


def fillholes_value_for_hist_name(hist_name):
  proc = process_base_from_hist_name(hist_name)
  if proc not in FITTEST_FILLHOLES_PROCS:
    return None
  return FITTEST_FILLHOLES_VALUES[proc]


def fill_holes_in_hist(h, hist_name, label):
  """
  Fill zero/negative normal bins for the selected card-level MC templates.

  This is meant to be the last bin-level fit-test treatment.  The same tiny
  positive value is written as both content and error, making Neff = 1.
  """
  if not should_fillholes_hist_name(hist_name):
    return False

  if not is_valid_th1(h):
    return False

  fill_value = fillholes_value_for_hist_name(hist_name)
  changed = []

  for ibin in range(1, h.GetNbinsX() + 1):
    old_val = h.GetBinContent(ibin)
    if old_val <= 0.:
      old_err = h.GetBinError(ibin)
      h.SetBinContent(ibin, fill_value)
      h.SetBinError(ibin, fill_value)
      changed.append((ibin, old_val, old_err))

  if changed:
    preview = ", ".join([
      str(ibin) + ":" + format(old_val, ".3g") + "+/-" + format(old_err, ".3g")
      for ibin, old_val, old_err in changed[:12]
    ])
    if len(changed) > 12:
      preview += ", ..."

    print(
      "[FitTest][FillHoles]",
      label,
      "hist =",
      hist_name,
      "fill =",
      fill_value,
      "changed bins old_content+/-old_error =",
      preview
    )

  return len(changed) > 0


def fill_holes_in_input_list(input_list, label):
  if not FITTEST_FILLHOLES_ACTIVE:
    return

  for item in input_list:
    h = item[1]
    name = item[2]

    if name == "data_obs":
      continue

    fill_holes_in_hist(h, name, label)


def should_smooth_hist_name(hist_name):
  proc = process_base_from_hist_name(hist_name)

  if proc is None:
    return False

  return proc in FITTEST_SMOOTH_PROCS


def clone_smoothed_121(h_in, out_name, n_iter, label):
  """
  Conservative 1-2-1 bin smoothing.

  Interior:
    new_i = 0.25*old_{i-1} + 0.50*old_i + 0.25*old_{i+1}

  Edges:
    new_1 = 0.75*old_1 + 0.25*old_2
    new_N = 0.25*old_{N-1} + 0.75*old_N

  Normal-bin integral is preserved after each iteration.
  Errors are propagated with the same linear weights in quadrature.
  """
  if not is_valid_th1(h_in):
    return None

  h_work = clone_detached(h_in, out_name)

  if h_work.GetNbinsX() < 2:
    return h_work

  for it_smooth in range(n_iter):
    nbins = h_work.GetNbinsX()
    old_integral = h_work.Integral()

    vals = [h_work.GetBinContent(ibin) for ibin in range(nbins + 2)]
    errs = [h_work.GetBinError(ibin) for ibin in range(nbins + 2)]

    h_out = h_work.Clone(out_name)
    h_out.Reset("ICES")
    h_out.SetDirectory(0)
    h_out.SetName(out_name)
    h_out.SetTitle(out_name)

    # Preserve underflow / overflow as-is. #### ERROR Running SetBinContent to overflow bin automatically extends the Nbin by two. USELESS...
    #h_out.SetBinContent(0, vals[0])
    #h_out.SetBinError(0, errs[0])
    #h_out.SetBinContent(nbins + 1, vals[nbins + 1])
    #h_out.SetBinError(nbins + 1, errs[nbins + 1])

    for ibin in range(1, nbins + 1):
      if ibin == 1:
        weights = [(1, 0.75), (2, 0.25)]
      elif ibin == nbins:
        weights = [(nbins - 1, 0.25), (nbins, 0.75)]
      else:
        weights = [(ibin - 1, 0.25), (ibin, 0.50), (ibin + 1, 0.25)]

      new_val = 0.
      new_err2 = 0.

      for src_bin, weight in weights:
        new_val += weight * vals[src_bin]
        new_err2 += (weight * errs[src_bin]) * (weight * errs[src_bin])

      h_out.SetBinContent(ibin, new_val)
      h_out.SetBinError(ibin, np.sqrt(new_err2))

    new_integral = h_out.Integral()

    if old_integral > 0. and new_integral > 0.:
      scale = old_integral / new_integral
      for ibin in range(1, nbins + 1):
        h_out.SetBinContent(ibin, h_out.GetBinContent(ibin) * scale)
        h_out.SetBinError(ibin, h_out.GetBinError(ibin) * abs(scale))

    print(
      "[FitTest][SmoothEE]",
      label,
      "iteration",
      it_smooth + 1,
      "/",
      n_iter,
      "integral old =",
      old_integral,
      "integral new =",
      h_out.Integral()
    )

    h_work = h_out

  return h_work


def smooth_selected_hists_in_input_list(input_list, label):
  for item in input_list:
    h = item[1]
    name = item[2]

    if not is_valid_th1(h):
      continue

    if not should_smooth_hist_name(name):
      continue

    h_smoothed = clone_smoothed_121(
      h,
      name,
      FITTEST_SMOOTH_NITER,
      label + " " + name
    )

    if is_valid_th1(h_smoothed):
      truncate_nonpositive_bins(
        h_smoothed,
        "after smoothing " + label + " " + name,
        zero_too=True
      )
      item[1] = h_smoothed


def should_apply_sr2_bin78_merge(region):
  return (
    FITTEST_MERGE_SR2_BIN78
    and (not args.CnC)
    and (not args.CR)
    and region == "sr2"
  )


def should_apply_sr2_bin3478_merge(region):
  return (
    FITTEST_MERGE_SR2_BIN3478
    and (not args.CnC)
    and (not args.CR)
    and region == "sr2"
  )



def should_apply_sr3_ee_bin1314_merge(region, channel, is_Weinberg, mass_int):
  return (
    FITTEST_MERGE_SR3_EE_BIN1314
    and (not args.CnC)
    and (not args.CR)
    and region == "sr3"
    and channel == "EE"
    and (not is_Weinberg)
    and 125 <= mass_int
    and mass_int <= 500
  )


def should_apply_ee_smoothing(region, channel, is_Weinberg, mass_int):
  if not FITTEST_SMOOTH_EE:
    return False

  if args.CnC:
    return False

  if args.CR:
    return False

  if channel != "EE":
    return False

  if region == "sr2":
    return True

  if region == "sr3" and (not is_Weinberg) and 125 <= mass_int and mass_int <= 500:
    return True

  return False

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
                era_cond = " or ".join([f'(era == "{e}")' for e in eras_sorted])

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
      OutputName = inputTag+"_"+tag+outputTag+TestTag+outputTagSuffix
      OutputPath = os.getcwd()+'/LimitInputs/'+OutputName+'/'
      os.system('mkdir -p '+OutputPath + era + '/' + region)
  
      if era == "Run2":
        if args.Scan:
          print("[Run2Builder] --Scan is ignored in Run2 mode. Please inspect source-era scans.")

        n_run2_built = 0
        n_run2_skipped = 0

        for mass in args.masses:
          is_Weinberg = (mass == "Weinberg")

          # Keep the original Ext behavior, but only for HNL mass points.
          if (not is_Weinberg) and args.Ext and mass != "M500":
            print(mass, "is not allowed to run with Ext option.")
            print("Exiting ...")
            sys.exit(1)

          # Match the original mass/region skip rules.
          # This covers:
          #   - M <= 100 in r1/r2
          #   - M > 3000 in r1
          if should_skip_limit_point_before_build(mass, region):
            print("[Run2Builder] SKIP by mass-region rule:", region, mass)
            n_run2_skipped += len(args.channels)
            continue

          # Known intentionally absent phase-space due to missing fake.
          # Currently: Weinberg in SR1.
          if should_skip_known_missing_fake_phase_space(mass, region):
            print("[Run2Builder] SKIP by known missing-fake rule:", region, mass)
            n_run2_skipped += len(args.channels)
            continue

          for channel in args.channels:

            # Match the original channel skip rule.
            # This covers:
            #   - M > 30000 only in EMu
            if should_skip_limit_channel_before_build(mass, channel):
              print("[Run2Builder] SKIP by mass-channel rule:", region, mass, channel)
              n_run2_skipped += 1
              continue

            out_name = build_run2_card_input(OutputPath, region, mass, channel, ExtTag)

            if out_name:
              n_run2_built += 1
            else:
              n_run2_skipped += 1

        print(
          "[Run2Builder] Region summary:",
          region,
          "built =",
          n_run2_built,
          "skipped =",
          n_run2_skipped
        )

        continue # This code help you to avoid opening MergedFiles/.../Run2/... . Instead, reuse existing era-dependent cards

      f_path_data          = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+PostFlag + "/DATA/"+Analyzer+DataSkim+"DATA.root"
      f_path_fake          = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunFake__"+PostFlag+"/DATA/"+Analyzer+FakeSkim+"Fake.root"
      f_path_cf            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunCF__"+PostFlag+"/DATA/"+Analyzer+CFSkim+"CF.root"
      f_path_zg            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunConv__"+PostFlag+"/"+Analyzer+"_ZG_norm.root"
      f_path_conv_inc      = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunConv__"+PostFlag+"/"+Analyzer+"_Conv_inc.root"
      f_path_conv_others   = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunConv__"+PostFlag+"/"+Analyzer+"_Conv_others.root"
      if 'WZ_powheg' in outputTag:
        f_path_wz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_WZ_norm_powheg.root"
      elif 'WZ_amcatnlo' in outputTag:
        f_path_wz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_WZ_norm_amcatnlo.root"
      else:
        f_path_wz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_WZ.root"
        f_path_wz_ewk            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_WZ_EWK.root"
      f_path_zz            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_ZZ_norm.root"
      f_path_ww            = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_WW_norm.root"
      f_path_prompt_inc    = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_Prompt_inc.root"
      f_path_prompt_others = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"RunPrompt__"+PostFlag+"/"+Analyzer+"_Prompt_others.root"
      f_path_mc_inc        = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"MergeMC__"+PostFlag+"/"+Analyzer+"_MC_inc.root"
      f_path_mc_others     = MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"MergeMC__"+PostFlag+"/"+Analyzer+"_MC_others.root"
      f_path_mc_individual = {
        this_proc: MainPath + "/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+"/" + era + "/" + PreFlag+RegionToDefFlagMap[region]+"MergeMC__"+PostFlag+"/"+Analyzer+"_"+this_proc+".root"
        for this_proc in MC_INDIVIDUAL_PROCS
      }
      
      if not Blinded: f_data = TFile.Open(f_path_data)
      f_fake          = TFile.Open(f_path_fake)
      f_cf            = TFile.Open(f_path_cf)
      f_zg            = TFile.Open(f_path_zg)
      f_conv_inc      = TFile.Open(f_path_conv_inc)
      f_conv_others   = TFile.Open(f_path_conv_others)
      f_wz            = TFile.Open(f_path_wz)
      f_wz_ewk        = TFile.Open(f_path_wz_ewk)
      f_zz            = TFile.Open(f_path_zz)
      f_ww            = TFile.Open(f_path_ww)
      f_prompt_inc    = TFile.Open(f_path_prompt_inc)
      f_prompt_others = TFile.Open(f_path_prompt_others)
      f_mc_inc        = TFile.Open(f_path_mc_inc)
      f_mc_others     = TFile.Open(f_path_mc_others)
      f_mc_individual = {
        this_proc: TFile.Open(f_path_mc_individual[this_proc])
        for this_proc in MC_INDIVIDUAL_PROCS
      }

      # Cache raw ROOT histograms by exact input path.  The helper always returns
      # detached clones, so later scaling/truncation never contaminates the cache.
      hist_read_cache = {}

      for mass in args.masses: # iterate for each mass ...
        is_Weinberg = (mass == "Weinberg")

        if not is_Weinberg:
          mass_int = int(mass.replace("M",""))

          if ("r1" in region or "r2" in region) and (mass_int <= 100):
            continue # NOTE use only SR3 below M100
          if ("r1" in region) and (mass_int > 3000):
            continue # NOTE skip SR1 above M3000

        for channel in args.channels: # ...and each channel

          if not is_Weinberg: # HNL
            if "EMu" not in channel and (mass_int > 30000):
              continue # NOTE Only EMu extends above M30000

            if (("sr3" in region) or ("cr3_Inv" in region)) and (mass_int <= 500): # BDT selection
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

            else: # SR1/2 or mass > 500 GeV: cut-based selection
              #if region=='sr2' and 'AltBin' in TestTag: LimitDir = "LimitExtractionAlt" # SR2 alternative optimization : use the same binning for all era, flavor. (deprecated)
              #else: LimitDir = "LimitExtraction"
              if region=='sr1' and 'AltBin' in TestTag: LimitDir = "LimitExtractionAlt" # SR1 alternative optimization : bin optimized with sqrt-removed-FOM.
              else: LimitDir = "LimitExtraction"

              if inputTag=="ANv7_NewBinning":
                if (region=="sr1") and (mass_int <= 3000):
                  if mass_int <= 400: InputHistMass = "M400/"
                  elif 1000 <= mass_int and mass_int < 1500: InputHistMass = "M1000/"
                  elif 1500 <= mass_int and mass_int < 2000: InputHistMass = "M1500/"
                  elif 2000 <= mass_int and mass_int <= 3000: InputHistMass = "M2000/"
                  else: InputHistMass = mass+"/"
                else:
                  InputHistMass = ""
              elif ("ANv7_NewBinning" in inputTag) or (PRver >= 191):
                if (region=="sr1") and (mass_int <= 3000):
                  if mass_int <= 400: InputHistMass = "M400/"
                  elif 1200 <= mass_int and mass_int < 1500: InputHistMass = "M1200/"
                  elif 1500 <= mass_int and mass_int < 2000: InputHistMass = "M1500/"
                  elif 2000 <= mass_int and mass_int <= 3000: InputHistMass = "M2000/"
                  else: InputHistMass = mass+"/"
                else:
                  InputHistMass = ""
              else:
                if (region=="sr1") and (mass_int <= 3000): ##### deprecated. It was used before Run2 NewBinning.
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
            if mass_int > 3000:
              DYVBFscaler = 0.1 if not test_tag_has("HighMassScaler") else test_tag_float("HighMassScaler", 0.1) # relax the scale for SSWW impact
            SSWWscaler = DYVBFscaler*DYVBFscaler # Set the signalSSWW scaler

          else: # Weinberg. #TODO let's merge Weinberg and other signals later, e.g. setting mass_int = 999999 for the Weinberg
            #if region=='sr2' and 'AltBin' in TestTag: LimitDir = "LimitExtractionAlt" # SR2 alternative optimization : use the same binning for all era, flavor. (deprecated)
            #else: LimitDir = "LimitExtraction"
            if region=='sr1' and 'AltBin' in TestTag: LimitDir = "LimitExtractionAlt" # SR1 alternative optimization : bin optimized with sqrt-removed-FOM.
            else: LimitDir = "LimitExtraction"
            InputHistMass = ""
            RegionToHistSuffixMap[region][channel] = RegionToHistSuffixMap[region][channel].replace('BDT','')
            Weinbergscaler = 10000. # Set the signalWeinberg scaler

          #print("f_cf :",f_path_cf)
          print("input_hist :", LimitDir+"/"+tag+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel])
          input_hist = LimitDir+"/"+tag+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel]
          
          print("##### Initiating",region,mass,channel,"...")

          if not Blinded:
            h_data = get_hist_cached(
              hist_read_cache,
              ("data_obs", f_path_data, input_hist),
              f_data,
              input_hist,
              "data_obs",
            )

          h_fake = get_hist_cached(
            hist_read_cache,
            ("fake", f_path_fake, input_hist),
            f_fake,
            input_hist,
            "fake",
          )
          h_cf = get_hist_cached(
            hist_read_cache,
            ("cf", f_path_cf, input_hist),
            f_cf,
            input_hist,
            "cf",
          ) if "E" in channel else None

          h_mc_individual = {}
          for this_proc in MC_INDIVIDUAL_PROCS:
            h_mc_individual[this_proc] = get_hist_cached(
              hist_read_cache,
              (this_proc, f_path_mc_individual[this_proc], input_hist),
              f_mc_individual[this_proc],
              input_hist,
              this_proc,
            )

          print("##### histo done.")

          # Make list of [file path, histogram, histo name].
          # At this stage keep only source processes.  Aggregated MC processes
          # are rebuilt below from the already-truncated individual MC templates.
          input_list = [[f_path_fake, h_fake, "fake"]]
          if "E" in channel:
            input_list.append([f_path_cf, h_cf, "cf"])

          if KEEP_MC_INDIVIDUAL_PROCS:
            for this_proc in MC_INDIVIDUAL_PROCS:
              input_list.append([
                f_path_mc_individual[this_proc],
                h_mc_individual[this_proc],
                this_proc,
              ])

          #### Treat 0 fakes: see v) of https://hypernews.cern.ch/HyperNews/CMS/get/EXO-21-002/25
          if not is_valid_th1(h_fake):
            print("[!!WARNING!!] There is no hist named " + input_hist + " in " + f_path_fake + " .")
            print("Skipping this mass/channel because fake is used as the binning template.")
            continue

          treat_fake_zero_bins(h_fake, f_path_fake + " " + input_hist)

          this_nbins = h_fake.GetNbinsX()

          # First truncate source-level MC templates.  Summary groups are built
          # only after this, so individual and group rates stay consistent.
          for item in input_list:
            if item[2] == "fake":
              # fake was treated by the dedicated prescription above
              continue
            truncate_nonpositive_bins(
              item[1],
              item[2] + " " + item[0] + " " + input_hist,
              zero_too=True,
            )

          if KEEP_MC_SUMMARY_PROCS:
            for summary_item in build_mc_summary_hists(input_list):
              append_or_replace_hist(input_list, summary_item[0], summary_item[1], summary_item[2])

          Nproc = len(input_list) # The number of processes = the length of the input list before adding data/signals/systematics


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
            print("##### Deferring Asimov data_obs until final post-truncation total background is built.")
            h_data = h_fake.Clone("data_obs")
            h_data.Reset("ICES")
            h_data.SetDirectory(0)
            input_list.append(["fake_data_path", h_data, "data_obs"])

          elif not is_valid_th1(h_data): # NOTE e.g. tight CR bins can miss data in a specific era/channel
            print("##### Data unblinded, but there is no data histogram!!!!!!!!!!!!!")
            print("##### Check -->",f_path_data,input_hist)
            print("##### Creating zero data...")
            h_data = h_fake.Clone("data_obs")
            h_data.Reset("ICES")
            h_data.SetDirectory(0)
            input_list.append([f_path_data, h_data, "data_obs"])
          else:
            input_list.append([f_path_data, h_data, "data_obs"])
          print("##### Data done.")

          # Now list has bkg, (pseudo) data. Finally let's add signals
          #if args.CR:
          #  print("##### This is CR setting.")
          #  print("##### Skipping signal ...")
          #else:
          if not is_Weinberg:
            f_path_signalDYVBF = MainPath +"/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDYVBF_"+mass+".root"
            f_path_signalDY = MainPath +"/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalDY_"+mass+".root"
            f_path_signalVBF = MainPath +"/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalVBF_"+mass+".root"
            f_path_signalSSWW  = MainPath +"/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalSSWW_"+mass+".root"
  
            f_signalDYVBF = CheckFile(f_path_signalDYVBF)
            if f_signalDYVBF:
              h_signalDYVBF = CheckHist(f_signalDYVBF,input_hist,"signalDYVBF")
              input_list.append([f_path_signalDYVBF, h_signalDYVBF, "signalDYVBF"])
              if h_signalDYVBF:
                h_signalDYVBF.Scale(DYVBFscaler) # Scaling the signal due to Combine fitting
                #print("Scaled signalDYVBF :", h_signalDYVBF.Integral())
              if args.Scan:
                print("##### Making 2D hist for","signalDYVBF","#####")
                FillScan(h_scan,h_signalDYVBF,"signalDYVBF") # out, in, name

            f_signalDY = CheckFile(f_path_signalDY)
            if f_signalDY:
              h_signalDY = CheckHist(f_signalDY,input_hist,"signalDY")
              input_list.append([f_path_signalDY, h_signalDY, "signalDY"])
              if h_signalDY:
                h_signalDY.Scale(DYVBFscaler) # Scaling the signal due to Combine fitting
                #print("Scaled signalDY :", h_signalDY.Integral())
              if args.Scan:
                print("##### Making 2D hist for","signalDY","#####")
                FillScan(h_scan,h_signalDY,"signalDY") # out, in, name

            f_signalVBF = CheckFile(f_path_signalVBF)
            if f_signalVBF:
              h_signalVBF = CheckHist(f_signalVBF,input_hist,"signalVBF")
              input_list.append([f_path_signalVBF, h_signalVBF, "signalVBF"])
              if h_signalVBF:
                h_signalVBF.Scale(DYVBFscaler) # Scaling the signal due to Combine fitting
                #print("Scaled signalVBF :", h_signalVBF.Integral())
              if args.Scan:
                print("##### Making 2D hist for","signalVBF","#####")
                FillScan(h_scan,h_signalVBF,"signalVBF") # out, in, name

            f_signalSSWW = CheckFile(f_path_signalSSWW)
            if f_signalSSWW:
              h_signalSSWW = CheckHist(f_signalSSWW,input_hist,"signalSSWW")
              input_list.append([f_path_signalSSWW, h_signalSSWW, "signalSSWW"])
              if h_signalSSWW:
                h_signalSSWW.Scale(SSWWscaler) # Scaling the signal due to Combine fitting
                #print("Scaled signalSSWW :", h_signalSSWW.Integral())
              if args.Scan:
                print("##### Making 2D hist for","signalSSWW","#####")
                FillScan(h_scan,h_signalSSWW,"signalSSWW") # out, in, name
          else:
            f_path_signalWeinberg  = MainPath +"/MergedFiles/"+Analyzer+"_"+inputTag+outputTag+ "/" + era + "/"+PreFlag+RegionToDefFlagMap[region]+"RunSignal__"+PostFlag+"/"+Analyzer+"_signalWeinberg.root"

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

          NoNOM_names = set()

          # Catch datacard processes that never entered input_list at all.
          # This happens when an aggregate process, e.g. ww = WpWp_QCD + WpWp_EWK,
          # has no valid component template, so build_mc_summary_hists() returns no ww item.
          present_valid_nominals = {
            item[2] for item in input_list
            if item[2] != "data_obs" and is_valid_th1(item[1])
          }

          expected_card_procs = CARD_BKG_PROCS[:]
          if "MuMu" in channel and "cf" in expected_card_procs:
            expected_card_procs.remove("cf")

          for proc in expected_card_procs:
            if proc not in present_valid_nominals:
              print(
                "[NoNOM missing-process guard] No valid nominal item for",
                proc, "in", tag, era, region, mass, channel,
                ". Making exception list ..."
              )
              NoNOM_names.add(proc)

          #### Treat non-positive bins for final nominal histograms (after truncation) ####
          for item in input_list:
            proc = item[2]
            if proc == "data_obs":
              continue

            if not is_valid_th1(item[1]):
              print("[!!WARNING!!] There is no NOMINAL hist named", input_hist, "for", proc, "in", item[0], ".")
              if proc in NOM_EXCEPTION_PROCS:
                NoNOM_names.add(proc)
              continue

            if proc == "fake":
              # fake already got the non-zero fake prescription; this call only
              # cleans up exactly-zero errors if any zero remained.
              pass
            else:
              truncate_nonpositive_bins(
                item[1],
                proc + " " + item[0] + " " + input_hist,
                zero_too=True,
              )

            if item[1].Integral() <= 0.:
              print("!!!!!! Zero norm detected in",proc,item[0],input_hist,"!!!!!!")
              if proc in NOM_EXCEPTION_PROCS:
                NoNOM_names.add(proc)
              elif proc in DIAGNOSTIC_ONLY_PROCS:
                print("This is a diagnostic-only process. Keep the histogram, but do not make a datacard NoNOM rule.")
              elif proc == "tot_bkg":
                print("This is a utility histogram. Keep it out of NoNOM rules.")

          for proc in sorted(NoNOM_names):
            print("No nominal hist/rate for",proc,"in",tag,era,region,mass,channel,". Making exception list ...")
            add_no_nom_exception(Except_list, proc, tag, region, era, channel, mass, is_Weinberg, mass_int if not is_Weinberg else -1)

          Nproc = len(input_list)

          if args.Syst:
            print("##### Systematics activated.")

            source_process_names = ["fake"]
            if "E" in channel:
              source_process_names.append("cf")
            if KEEP_MC_INDIVIDUAL_PROCS:
              source_process_names += MC_INDIVIDUAL_PROCS
            source_process_names += [
              "signalDYVBF", "signalDY", "signalVBF", "signalSSWW", "signalWeinberg",
            ]

            syst_source_items = [
              item for item in input_list
              if item[2] in source_process_names
              and item[2] not in NoNOM_names
              and is_valid_th1(item[1])
            ]

            for source_item in syst_source_items:
              src_path, h_nom, proc = source_item

              nom_yield_for_syst, _ = hist_integral_and_error(h_nom)
              if nom_yield_for_syst <= 0.:
                print(
                  "[Syst][WARNING] Non-positive nominal yield for",
                  proc, tag, era, region, mass, channel,
                  "; skipping all systematics for this zero-nominal source process."
                )
                continue

              if args.Scan:
                h_scan = TH2D(proc, proc, this_nbins, 0, this_nbins, len(source_process_names), 0, len(source_process_names))
                print("h_scan for",proc,"syst created; this should be empty:",h_scan.Integral(0,this_nbins,1,1))
                if h_scan.Integral(0,this_nbins,1,1)!=0.: sys.exit()
                h_scan.SetDirectory(0)

              f_syst = CheckFile(src_path)
              if not f_syst:
                print("[!!ERROR!!] No syst file",src_path,". Exiting...")
                sys.exit()

              ###### PDF error sets for signals and WZ only ######
              hist_pdfUp = None
              hist_pdfDown = None
              if "PDFUp" in SystList and should_make_syst_for_process(proc, "PDFUp") and pdf_scale_label_for_process(proc) is not None:
                hist_pdfUp = h_nom.Clone("pdfUp_tmp")
                hist_pdfUp.Reset("ICES")
                hist_pdfUp.SetDirectory(0)
                hist_pdfDown = h_nom.Clone("pdfDown_tmp")
                hist_pdfDown.Reset("ICES")
                hist_pdfDown.SetDirectory(0)

                pdf_mode = pdf_mode_for_process(proc)
                Npdfmember = 100

                pdf_hists = []
                for it_rep in range(Npdfmember):
                  this_pdf_hist = (
                    LimitDir
                    + "/Syst_PDF" + tag + "_Syst_PDF" + str(it_rep)
                    + "/" + RegionToChannelMap[region][channel]
                    + "/" + InputHistMass + RegionToHistSuffixMap[region][channel]
                  )

                  h_pdf = get_hist_cached(
                    hist_read_cache,
                    (proc, "PDF", it_rep, src_path, this_pdf_hist),
                    f_syst,
                    this_pdf_hist,
                    proc + "_PDF" + str(it_rep),
                  )
                  if not is_valid_th1(h_pdf):
                    raise ValueError("No PDF variation!! --> " + this_pdf_hist)

                  if is_signal_process(proc):
                    h_pdf.Scale(signal_scale_factor(proc, is_Weinberg, DYVBFscaler if not is_Weinberg else 1., SSWWscaler if not is_Weinberg else 1., Weinbergscaler if is_Weinberg else 1.))

                  pdf_hists.append(h_pdf)

                if args.CnC:
                  nom_total, _ = hist_integral_and_error(h_nom)

                  if nom_total <= 0.:
                    raise RuntimeError(
                      f"[PDF/CnC] Non-positive nominal yield for "
                      f"{proc} {tag} {era} {region} {mass} {channel}"
                    )

                  pdf_totals = [hist_integral_and_error(h)[0] for h in pdf_hists]
                  delta = get_pdf_delta(pdf_totals, nom_total, pdf_mode)

                  up_total = nom_total + delta
                  down_total = nom_total - delta

                  if down_total <= 0.:
                    print(
                      "[PDF/CnC][WARNING]",
                      proc,
                      "PDF down total is non-positive:",
                      down_total,
                      "setting it to a tiny positive value."
                    )
                    down_total = 1e-12 * nom_total

                  hist_pdfUp = h_nom.Clone("pdfUp_tmp")
                  hist_pdfUp.SetDirectory(0)
                  hist_pdfUp.Scale(up_total / nom_total)

                  hist_pdfDown = h_nom.Clone("pdfDown_tmp")
                  hist_pdfDown.SetDirectory(0)
                  hist_pdfDown.Scale(down_total / nom_total)

                  print(
                    "[PDF/CnC]",
                    proc,
                    "nom =", nom_total,
                    "delta =", delta,
                    "up =", hist_integral_and_error(hist_pdfUp)[0],
                    "down =", hist_integral_and_error(hist_pdfDown)[0]
                  )

                else:
                  for it_bin in range(1, this_nbins + 1):
                    bin_values = [h.GetBinContent(it_bin) for h in pdf_hists]
                    nom = h_nom.GetBinContent(it_bin)
                    delta = get_pdf_delta(bin_values, nom, pdf_mode)

                    hist_pdfUp.SetBinContent(it_bin, nom + delta)
                    hist_pdfDown.SetBinContent(it_bin, max(nom - delta, 0.))

              for this_syst in SystList:
                if not should_make_syst_for_process(proc, this_syst):
                  continue

                syst_input_hist = LimitDir+"/Syst_"+this_syst+tag+"/"+RegionToChannelMap[region][channel]+"/"+InputHistMass+RegionToHistSuffixMap[region][channel]
                this_name_syst = output_syst_suffix(era, region, this_syst, proc)
                name_syst = proc + "_" + this_name_syst

                made_from_nominal = False
                if 'PDFUp' in this_syst:
                  h_syst = clone_detached(hist_pdfUp, name_syst)
                elif 'PDFDown' in this_syst:
                  h_syst = clone_detached(hist_pdfDown, name_syst)
                else:
                  h_syst = get_hist_cached(
                    hist_read_cache,
                    (proc, this_syst, src_path, syst_input_hist),
                    f_syst,
                    syst_input_hist,
                    name_syst,
                  )

                if is_valid_th1(h_syst):
                  if args.Scan:
                    print("##### Making 2D hist for",name_syst,"#####")
                    FillScan(h_scan,h_syst,name_syst)

                  h_syst.SetDirectory(0)
                  h_syst.SetName(name_syst)
                  h_syst.SetTitle(name_syst)

                  if proc == "fake" and "FR" in this_syst and "CF" not in this_syst:
                    treat_fake_zero_bins(h_syst, src_path + " " + syst_input_hist + " with syst " + name_syst)
                  else:
                    truncate_nonpositive_bins(h_syst, proc + " " + syst_input_hist + " with syst " + name_syst, zero_too=True)

                  if h_syst.Integral() <= 0.:
                    print("!!!!!! Zero norm detected in",proc,syst_input_hist,"with syst:",name_syst,"!!!!!!")
                    print("Making a makeup hist(=copy of nominal)...")
                    h_syst = h_nom.Clone(name_syst)
                    h_syst.SetName(name_syst)
                    h_syst.SetTitle(name_syst)
                    h_syst.SetDirectory(0)
                    made_from_nominal = True

                else:
                  if args.Scan:
                    print("##### Making 2D hist for",name_syst,"#####")
                    FillScan(h_scan,h_syst,name_syst)

                  print("No hist for",name_syst,".")
                  print("Making a makeup hist(=copy of nominal)...")
                  h_syst = h_nom.Clone(name_syst)
                  h_syst.SetName(name_syst)
                  h_syst.SetTitle(name_syst)
                  h_syst.SetDirectory(0)
                  made_from_nominal = True

                if is_signal_process(proc) and ('PDFUp' not in this_syst and 'PDFDown' not in this_syst) and (not made_from_nominal):
                  h_syst.Scale(signal_scale_factor(proc, is_Weinberg, DYVBFscaler if not is_Weinberg else 1., SSWWscaler if not is_Weinberg else 1., Weinbergscaler if is_Weinberg else 1.))
                  truncate_nonpositive_bins(h_syst, proc + " scaled syst " + name_syst, zero_too=True)

                force_lowstat_syst_bins_to_nominal(
                  h_syst,
                  h_nom,
                  proc,
                  name_syst,
                  tag + " " + era + " " + region + " " + mass + " " + channel
                )

                print("Appending "+name_syst+"...")
                append_or_replace_hist(input_list, src_path, h_syst, name_syst)

              if args.Scan:
                h_scan.SetDirectory(0)
                scan_list.append(h_scan)

            # Build aggregate MC systematic variations from the already-made
            # individual MC systematic variations.  This guarantees that e.g.
            # mc_others_CMS_* equals the sum of truncated component CMS_* hists.
            if KEEP_MC_SUMMARY_PROCS:
              for this_syst in SystList:
                if is_pdf_or_qcd_scale_syst(this_syst):
                  # Only WZ gets these in the current datacard model.
                  aggregate_proc_names = ["wz"]
                else:
                  aggregate_proc_names = SUMMARY_MC_PROCS

                for agg_proc in aggregate_proc_names:
                  if agg_proc in NoNOM_names:
                    continue
                  if agg_proc not in MC_COMPONENTS:
                    continue
                  if not should_make_syst_for_process(agg_proc, this_syst):
                    continue

                  name_to_hist = {item[2]: item[1] for item in input_list}
                  agg_suffix = output_syst_suffix(era, region, this_syst, agg_proc)
                  agg_name = agg_proc + "_" + agg_suffix

                  component_syst_names = []
                  for comp in MC_COMPONENTS[agg_proc]:
                    if should_make_syst_for_process(comp, this_syst):
                      comp_suffix = output_syst_suffix(era, region, this_syst, comp)
                      comp_syst_name = comp + "_" + comp_suffix
                      if comp_syst_name in name_to_hist:
                        component_syst_names.append(comp_syst_name)
                      else:
                        print("[AggregateSyst][WARNING]",comp_syst_name,"is missing; using nominal",comp,"instead.")
                        component_syst_names.append(comp)
                    else:
                      component_syst_names.append(comp)

                  h_agg_syst = make_sum_hist(
                    name_to_hist,
                    component_syst_names,
                    agg_name,
                    "aggregate syst " + agg_name,
                    missing_ok=True,
                  )
                  if not is_valid_th1(h_agg_syst):
                    continue

                  truncate_nonpositive_bins(h_agg_syst, "aggregate syst " + agg_name, zero_too=True)

                  if should_symmetrize_zg_scale_j_2018_sr2_down(
                    era,
                    region,
                    agg_proc,
                    this_syst,
                    agg_name
                  ):
                    up_name_syst = agg_name[:-4] + "Up"
                    h_up_for_symm = get_hist_from_input_list(input_list, up_name_syst)

                    if h_up_for_symm is None:
                      raise RuntimeError(
                        "[PruneZG] Cannot find corresponding Up histogram: "
                        + up_name_syst
                        + ". Check SystList ordering."
                      )

                    h_nom_for_symm = get_hist_from_input_list(input_list, agg_proc)
                    symmetrize_down_from_up(
                      h_agg_syst,
                      h_nom_for_symm,
                      h_up_for_symm,
                      [7, 8],
                      tag + " " + era + " " + region + " " + mass + " " + channel + " " + agg_name
                    )

                  h_nom_for_lowstat = get_hist_from_input_list(input_list, agg_proc)
                  force_lowstat_syst_bins_to_nominal(
                    h_agg_syst,
                    h_nom_for_lowstat,
                    agg_proc,
                    agg_name,
                    tag + " " + era + " " + region + " " + mass + " " + channel
                  )

                  print("Appending "+agg_name+"...")
                  append_or_replace_hist(input_list, "__aggregate__", h_agg_syst, agg_name)

            print("##### Systematics done.")

          ### Now remove NoNOMs only for datacard processes.
          ### Diagnostic-only individual MC hists are intentionally kept.
          if NoNOM_names:
            kept = []
            for item in input_list:
              if item[2] in NoNOM_names:
                print("Erase nominal zero norm datacard process:")
                print(item)
                continue
              kept.append(item)
            input_list = kept

          # ------------------------------------------------------------
          # Optional fit-stability post-processing controlled by -o tags.
          # This must run before tot_bkg / blinded data_obs are built.
          # ROOT bin numbering is 1-based.
          # ------------------------------------------------------------
          fit_test_mass_int = mass_int if not is_Weinberg else -1
          fit_test_label = tag + " " + era + " " + region + " " + mass + " " + channel

          if should_apply_sr2_bin78_merge(region):
            merge_bins_in_input_list(
              input_list,
              7,
              fit_test_label + " MergeSR2Bin78"
            )

          if should_apply_sr2_bin3478_merge(region):
            merge_bins_in_input_list(
              input_list,
              7,
              fit_test_label + " MergeSR2Bin3478"
            )
            merge_bins_in_input_list(
              input_list,
              3,
              fit_test_label + " MergeSR2Bin3478"
            )

          if should_apply_sr3_ee_bin1314_merge(region, channel, is_Weinberg, fit_test_mass_int):
            merge_bins_in_input_list(
              input_list,
              13,
              fit_test_label + " MergeSR3EEBin1314"
            )

          if should_apply_ee_smoothing(region, channel, is_Weinberg, fit_test_mass_int):
            smooth_selected_hists_in_input_list(
              input_list,
              fit_test_label + " SmoothEE"
            )

          if FITTEST_FILLHOLES_ACTIVE:
            fill_holes_in_input_list(
              input_list,
              fit_test_label + " FillHoles"
            )

          #print("[DEBUG]", region, mass, channel, input_hist)
          #for item in input_list:
          #  if item[2] in [
          #    "fake", "cf",
          #    "ZGToLLG", "ZGToLLG_PtG_130", "DYJets_MG", "DYJets10to50_MG",
          #    "zg", "mc_others"
          #  ]:
          #    h = item[1]
          #    if is_valid_th1(h):
          #      print(item[2], "nbins =", h.GetNbinsX(), "integral =", h.Integral(), "path =", item[0])
          #    else:
          #      print(item[2], "INVALID", "path =", item[0])

          # ------------------------------------------------------------
          # Build total background after all nominal source/group truncation.
          # - In blinded mode, data_obs is replaced by this post-processed sum.
          # - In unblinded SR and in CR, data_obs stays real data, and tot_bkg is
          #   written as a separate utility histogram.
          # ------------------------------------------------------------
          h_tot_bkg = build_total_background(input_list, channel, "tot_bkg")
          append_or_replace_hist(input_list, "__aggregate__", h_tot_bkg, "tot_bkg")

          if Blinded:
            h_asimov = h_tot_bkg.Clone("data_obs")
            h_asimov.SetName("data_obs")
            h_asimov.SetTitle("data_obs")
            h_asimov.SetDirectory(0)
            append_or_replace_hist(input_list, "fake_data_path", h_asimov, "data_obs")

          print("##### Now creating a limit input root file...")
          outName = OutputPath+era+"/"+region+"/"+mass+"_"+channel+ExtTag
          outfile = TFile.Open(outName+"_card_input.root","RECREATE")
          
          outfile.cd() # Move into it
          
          for item in input_list: # Remember, item = [path,hist,name]
            try:
              if not is_valid_th1(item[1]):
                raise AttributeError

              item[1].SetName(item[2])
              item[1].SetTitle(item[2])

              if item[2] != "data_obs":
                truncate_nonpositive_bins(
                  item[1],
                  "final write " + item[2] + " " + region + " " + mass + " " + channel,
                  zero_too=True,
                )

              if item[1].Integral() <= 0. and item[2] != "data_obs":
                print("[!!WARNING!!] Non-positive final integral " + str(item[1].Integral()) + " in " + item[2] + " ------------------------------------")

              if args.CnC:
                #print("!!Cut and count option activated!!")
                #print("!!Merging all into 1 bin...!!")

                CnChist = make_cnc_hist(item[1], item[2])

                print("Writing "+item[2]+"...")
                CnChist.Write()
              else:
                print("Writing "+item[2]+"...")
                item[1].Write() # Write each histogram while iterating

            except AttributeError:
              print("[!!WARNING!!] Final check: There is no hist",item[2],"in",region,mass,channel,item[0],".") # Final check

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
  if args.eras == ["Run2"]:
    print("[Run2Builder] Skipping exception-rule writing in pure Run2 synthesis mode.")
  else:
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
