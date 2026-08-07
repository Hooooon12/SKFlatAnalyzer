# Place it at /data6/Users/jihkim/HNDiLeptonWorskspace/Limits/ReadLimits/Shape
# python ReadLimitFromTree.py --Full[--Asymptotic]

from ROOT import *
import os, argparse
import glob, re, math

parser = argparse.ArgumentParser(description='option')
parser.add_argument('--BDT', action='store_true')
parser.add_argument('--Ext', action='store_true')
parser.add_argument('--Asymptotic', action='store_true')
parser.add_argument('--Full', action='store_true')
parser.add_argument('--Unblind', action='store_true',
                    help='For Asymptotic: read Expected ROOT for bands and Observed ROOT for observed limit')
args = parser.parse_args()

#workdir = "/data6/Users/jihkim/CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter/Batch/"
#workdir = "/data6/Users/jihkim/NewCombine/CMSSW_14_1_0_pre4/src/DilepHN/Batch/"
#workdir = "/data6/Users/jihkim/LatestCombine/CMSSW_14_1_0_pre4/src/DilepHN/Batch/"
workdir = "/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/Batch/"

#years = ["2016","2017","2018"]
#years = ["2016preVFP","2016postVFP","2017","2018","Run2"]
#years = ["Run2"]
years = ["Run2Sum"]
#years = ["Run2","Run2Sum"]
#years = ["2016preVFP","2016postVFP"]
#years = ["2017"]
#years = ["2018"]
channels = ["MuMu","EE","EMu"]
#channels = ["3ch"]
#channels = ["MuMu","EE"]
#channels = ["EE"]
#channels = ["MuMu"]
#channels = ["EMu"]
#masses = ["100","200","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000"]
#masses = ["90","100","150","200","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["100","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["100","200","300","400","1000","10000"]
masses = ["85","90","95","100","125","150","200","250","300","350","400","450","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000","25000","30000"]
masses_EMu = ["85","90","95","100","125","150","200","250","300","350","400","450","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000","25000","30000","40000","50000","60000"]
#masses = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses_EMu = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000"]
#masses = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000"]
#masses_EMu = ["85","90","95","100","125","150","200","250","300","400","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000"]

IDs = [""] #["_ID"]

#myWPs = ["240422_HNL_ULID"]
#myWPs = ["240501_1704_HNL_ULID","240501_1704_HNTightV2"]
#myWPs = ["240504_PR44_HNL_ULID"]
#myWPs = ["240505_PR45_HNL_ULID"]
#myWPs = ["240505_PR46_HNL_ULID","240505_PR46_HNTightV2"]
#myWPs = ["rateParam_HNL_ULID"]
#myWPs = ["PR48_rateParam_HNL_ULID"]
#myWPs = ["PR55_HNL_ULID","PR55_HighPt"]
#myWPs = ["PR55_NoMinPt_HighPt"]
#myWPs = ["PR86_HNL_ULID_Decorr"]
#myWPs = ["PR95_HNL_ULID_Decorr"]
#myWPs = ["PR95_HNL_ULID_Decorr","PR95_HNL_ULID_Decorr_NoMuSyst","PR95_HNL_ULID_MuSystOnly"] # WP of input limit file
#myWPs = ["PR95_HNL_ULID_MuSystOnly"] # WP of input limit file
#myWPs = ["PR95_HNL_ULID"] # WP of input limit file
#myWPs = ["PR95_HNL_ULID_NoCR_Decorr"]
#myWPs = ["PR89_HNL_ULID_HighPtIDComp_lnNsyst_Decorr","PR89_HighPt_HighPtIDComp_lnNsyst_Decorr"]
#myWPs = ["PR97_HNL_ULIDv2_NoCR_NoSyst"]
#myWPs = ["ANv3_HNL_ULIDv2_Decorr_NoCR"]
#myWPs = ["ANv3_HNL_ULIDv2_Decorr_TEST_NoCR"]
#myWPs = ["ANv3_HNL_ULIDv2_Decorr"]
#myWPs = ["ANv3_HNL_ULIDv2_Decorr_Run2"]
#myWPs = ["HEMJet_HNL_ULIDv2_RemoveHEMJet_NoCR_NoSyst","HEMJet_HNL_ULIDv2_ScaleHEMJet_NoCR_NoSyst","TuneP_HNL_ULIDv2_CompareTuneP_NoCR_NoSyst","TuneP_HNTightV2_CompareTuneP_NoCR_NoSyst","TuneP_POGTight_CompareTuneP_NoCR_NoSyst"]
#myWPs = ["ANv3_HNL_ULIDv2_Decorr_NoCR_NoSyst"]
#myWPs = ["ANv4_HNL_ULIDv2_RunSyst_Decorr_JetDecorr_NoCR"]
#myWPs = ["ANv4_HNL_ULIDv2_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv5_HNL_ULIDv2_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv5_HNL_ULIDv2_RunSyst_BeforeJetIDLepPt_Decorr_JetDecorr","ANv5_HNL_ULIDv2_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv5_HNL_ULIDv2_RunSyst_Decorr_JetDecorr","ANv5_BDTV3_StrictBinning_HNL_ULIDv2_RunSyst_Decorr_JetDecorr","ANv5_BDTV3_LooseBinning_HNL_ULIDv2_RunSyst_Decorr_JetDecorr","ANv5_BDTV4_StrictBinning_HNL_ULIDv2_RunSyst_Decorr_JetDecorr","ANv5_BDTV4_LooseBinning_HNL_ULIDv2_RunSyst_Decorr_JetDecorr","ANv5_BDTV4_VeryLooseBinning_HNL_ULIDv2_RunSyst_Decorr_JetDecorr",]
#myWPs = ["ANv5_BDTV4_BugFix_HNL_ULIDv2_V4_LooseBin_RunSyst_Decorr_JetDecorr","ANv5_BDTV4_BugFix_HNL_ULIDv2_V4_StrictBin_RunSyst_Decorr_JetDecorr",]
#myWPs = ["ANv5_BDTV4_BugFix_HNL_ULIDv2_V3_LooseBin_RunSyst_Decorr_JetDecorr","ANv5_BDTV4_BugFix_HNL_ULIDv2_V3_StrictBin_RunSyst_Decorr_JetDecorr"]
#myWPs = [
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V2_StrictBin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V3_StrictBin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V3_Strict_10_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V3_Strict_20_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V4_Strict_10_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V4_Strict_15_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V4_Strict_20_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V2_StrictBin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V3_StrictBin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V3_Strict_10_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V3_Strict_20_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V4_Strict_10_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V4_Strict_15_Bin_RunSyst_Decorr_JetDecorr",
#         "ANv5_BDTV2to4_HNL_ULIDv2_WZ_powheg_V4_Strict_20_Bin_RunSyst_Decorr_JetDecorr",
#]
#myWPs = ["ANv5_BDTV3_UpdateSRBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr","ANv5_BDTV3_UpdateSRBinning_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv5_BDTV3_AltSR1_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr","ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewRP"]
#myWPs = ["ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_FixCorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv6_NewSignals_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv6_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr"]
#myWPs = ["ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr"]
#myWPs = ["ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"]
#myWPs = ["ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr","ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr"]
#myWPs = ["ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS_Decorr"]
#myWPs = ["ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF"]
#myWPs = ["ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval"]
#myWPs = ["ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Merged_Decorr_JetDecorr_Preapproval"]
#myWPs = ["ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval","ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_SigInCR_Preapproval","ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_OnlySS_Decorr_JetDecorr_Preapproval","ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets_Decorr_JetDecorr_Preapproval"]
#myWPs = ["ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavDep_Decorr_JetDecorr_Preapproval","ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavEraDep_Decorr_JetDecorr_Preapproval","ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval_FakelnN"]
#myWPs = ["ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval","ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets_Decorr_JetDecorr_Preapproval"]
#myWPs = ["ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval","ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_BinRefinement_Decorr_JetDecorr_Preapproval"]
#myWPs = ["ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval"]
#myWPs = ["ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval"]
#myWPs = ["ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval"]
#myWPs = ["ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_New3ch_Preapproval"]
#myWPs = ["ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval_StudyEnvelope"]
#myWPs = ["ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval"]
#myWPs = ["ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188"]
#myWPs = ["ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188","ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass_Decorr_JetDecorr_PR188"]
#myWPs = ["ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_Global_Decorr_JetDecorr_PR188","ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_MassGroups_Decorr_JetDecorr_PR188"]
#myWPs = ["ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_StatReqEra_Decorr_JetDecorr_PR188"]
#myWPs = ["ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR3Update_Decorr_JetDecorr_PR188"]
#myWPs = ["ANv7_NewBinning_PR191_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188"]
#myWPs = ["ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194"]
#myWPs = ["ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194","ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194","ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194"]
#myWPs = ["ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195"]
myWPs = ["ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195"]

#tags = ["_sronly_syst"]
#tags = ["_sronly"]
#tags = ["_syst"]
#tags = ["_HNL_syst"]
#tags = ["_Weinberg_syst"]
#tags = ["_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined"]
#tags = ["_HNL_sr1_syst_Combined","_HNL_sr2_syst_Combined","_HNL_sr3_syst_Combined"]
#tags = ["_DYVBF_syst","_DY_syst","_VBF_syst","_SSWW_syst"]
#tags = ["_DYVBF_syst"]
#tags = ["_sronly_sr123_syst"]
#tags = ["_sronly_sr123"]
#tags = ["_DYVBF_sronly_sr123_syst"]
#tags = ["_syst","_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined"]
#tags = ["_DY_syst","_VBF_syst","_SSWW_syst","_HNL_sr1_syst_Combined","_HNL_sr2_syst_Combined","_HNL_sr3_syst_Combined","_Weinberg_sr2_syst_Combined","_Weinberg_sr3_syst_Combined"]
tags = ["_HNL_syst","_Weinberg_syst"]
#tags = ["_HNL_syst","_Weinberg_syst","_HNL_sr1_syst_Combined","_HNL_sr2_syst_Combined","_HNL_sr3_syst_Combined","_Weinberg_sr2_syst_Combined","_Weinberg_sr3_syst_Combined"]

BDTTag = '_BDT' if args.BDT else ''
ExtTag = '_Ext' if args.Ext else ''

MASS_REF_WEINBERG = 0.303121442

def parse_weinberg_label(label):
  """
  Example:
    wMuMu0p5_wEMu0p5_wEE0p0
      -> 0.5, 0.5, 0.0
  """
  m = re.match(r"^wMuMu([0-9mp]+)_wEMu([0-9mp]+)_wEE([0-9mp]+)$", label)
  if not m:
    return None

  def decode(x):
    return float(x.replace("m", "-").replace("p", "."))

  wMuMu = decode(m.group(1))
  wEMu  = decode(m.group(2))
  wEE   = decode(m.group(3))

  return wMuMu, wEMu, wEE


def mass_limit_from_r_and_w(r_limit, w_value):
  """
  mass_c_limit = mass_ref * sqrt(r * w_c)
  """
  if w_value <= 0.:
    return 0.0
  if r_limit <= 0.:
    return 0.0

  return MASS_REF_WEINBERG * math.sqrt(r_limit * w_value)


def fmt_limit_value(x, ndigit=6):
  """
  Keep enough precision for GeV-scale effective mass limits.
  """
  return str(round(float(x), ndigit))

EXPECTED_QUANTILES = [0.025, 0.160, 0.500, 0.840, 0.975]


def first_existing_path(paths):
  """
  Return the first existing path. If none exists, return the first candidate.
  This is only used for optional .out fallback parsing.
  """
  for p in paths:
    if p and os.path.exists(p):
      return p
  return paths[0] if paths else None


def parse_asymptotic_out(out_path):
  """
  Parse Combine text output if ROOT reading fails.

  Return:
    obs_val: float or None
    exp_vals: [Exp_m2s, Exp_m1s, Exp, Exp_p1s, Exp_p2s] or None
  """
  obs_val = None
  exp_vals = None

  if not out_path or not os.path.exists(out_path):
    return obs_val, exp_vals

  try:
    txt = open(out_path).read()

    m_obs = re.search(r"Observed\s+Limit:\s+r\s+<\s+([0-9eE+\-.]+)", txt)
    if m_obs:
      obs_val = float(m_obs.group(1))

    patterns = [
      r"Expected\s+2\.5%:\s+r\s+<\s+([0-9eE+\-.]+)",
      r"Expected\s+16\.0%:\s+r\s+<\s+([0-9eE+\-.]+)",
      r"Expected\s+50\.0%:\s+r\s+<\s+([0-9eE+\-.]+)",
      r"Expected\s+84\.0%:\s+r\s+<\s+([0-9eE+\-.]+)",
      r"Expected\s+97\.5%:\s+r\s+<\s+([0-9eE+\-.]+)",
    ]

    vals = []
    for p in patterns:
      m = re.search(p, txt)
      if not m:
        vals = None
        break
      vals.append(float(m.group(1)))

    exp_vals = vals

  except Exception:
    return None, None

  return obs_val, exp_vals


def read_asymptotic_values(root_path, out_path=None):
  """
  Read one AsymptoticLimits ROOT file.

  Return:
    obs_val: observed limit if present, otherwise None
    exp_vals: [Exp_m2s, Exp_m1s, Exp, Exp_p1s, Exp_p2s] if present, otherwise None

  This uses quantileExpected when available:
    quantileExpected < 0  : observed
    0.025, 0.160, ...     : expected quantiles
  """
  obs_val = None
  exp_by_quantile = [None, None, None, None, None]
  exp_in_order = []

  if root_path and os.path.exists(root_path):
    try:
      f_Asym = TFile.Open(root_path)

      if f_Asym and not f_Asym.IsZombie():
        tree_Asym = f_Asym.Get("limit")

        if tree_Asym:
          n_entries = int(tree_Asym.GetEntries())

          for i in range(n_entries):
            tree_Asym.GetEntry(i)
            val = float(tree_Asym.limit)

            try:
              q = float(tree_Asym.quantileExpected)
            except Exception:
              q = None

            if q is not None and q < -0.5:
              obs_val = val
            elif q is not None:
              iq = min(range(len(EXPECTED_QUANTILES)),
                       key=lambda k: abs(q - EXPECTED_QUANTILES[k]))
              if abs(q - EXPECTED_QUANTILES[iq]) < 0.03:
                exp_by_quantile[iq] = val
              else:
                exp_in_order.append(val)
            else:
              exp_in_order.append(val)

      if f_Asym:
        f_Asym.Close()

    except Exception:
      pass

  exp_vals = exp_by_quantile if all(v is not None for v in exp_by_quantile) else None

  # Fallback for very old ROOT outputs without quantileExpected.
  if exp_vals is None and len(exp_in_order) >= 5:
    if obs_val is None and "Observed" in os.path.basename(root_path) and len(exp_in_order) >= 6:
      obs_val = exp_in_order[0]
      exp_vals = exp_in_order[1:6]
    else:
      exp_vals = exp_in_order[:5]

  # Text-output fallback.
  out_obs, out_exp = parse_asymptotic_out(out_path)
  if obs_val is None:
    obs_val = out_obs
  if exp_vals is None:
    exp_vals = out_exp

  return obs_val, exp_vals


def build_asymptotic_paths(this_workdir, this_name, mode_label, scan_suffix=""):
  """
  New create-batch.py output naming convention:

    output/<this_name>_Asymptotic_Expected.root
    output/<this_name>_Asymptotic_Observed.root

  For scans:

    output/<this_name>_Asymptotic_Expected_f0.5.root
    output/<this_name>_Asymptotic_Observed_f0.5.root

    output/<this_name>_Asymptotic_Expected_wMuMu...root
    output/<this_name>_Asymptotic_Observed_wMuMu...root
  """
  batch_dir = this_workdir + "/Asymptotic/" + this_name
  output_dir = batch_dir + "/output"

  mode_piece = "_" + mode_label if mode_label != "" else ""

  root_name = this_name + "_Asymptotic" + mode_piece + scan_suffix + ".root"
  out_name  = this_name + "_Asymptotic" + mode_piece + scan_suffix + ".out"

  root_path = output_dir + "/" + root_name
  out_path = first_existing_path([
    batch_dir + "/logs/" + out_name,
    batch_dir + "/" + out_name
  ])

  return root_path, out_path


def read_asymptotic_line_values(this_workdir, this_name, scan_suffix="", read_observed=False):
  """
  Return one output line payload:

    [Obs, Exp_m2s, Exp_m1s, Exp, Exp_p1s, Exp_p2s]

  Expected bands always come from the blind Expected ROOT.
  Observed value comes from Observed ROOT only when read_observed=True.
  Otherwise Obs is filled with expected median, preserving the old blind convention.
  """
  expected_root, expected_out = build_asymptotic_paths(this_workdir, this_name, "Expected", scan_suffix)
  _, exp_vals = read_asymptotic_values(expected_root, expected_out)

  if exp_vals is None:
    print("[WARN] Missing or invalid Expected Asymptotic limit:")
    print("       " + expected_root)
    return None

  obs_val = exp_vals[2]

  if read_observed:
    observed_root, observed_out = build_asymptotic_paths(this_workdir, this_name, "Observed", scan_suffix)
    obs_from_file, _ = read_asymptotic_values(observed_root, observed_out)

    if obs_from_file is None:
      print("[WARN] Missing or invalid Observed Asymptotic limit:")
      print("       " + observed_root)
      return None

    obs_val = obs_from_file

  return [obs_val] + exp_vals


###########################
# Main reader starts here #
###########################

for WP in myWPs:
  this_workdir = workdir+WP
  WP = WP+BDTTag+ExtTag
  os.system("mkdir -p limits/"+WP)
  for year, channel, ID, tag in [[year, channel, ID, tag] for year in years for channel in channels for ID in IDs for tag in tags]:
    
    if args.Asymptotic:
  
      #### Flavor-combined limits ####
      if channel=="3ch" or (channel!="3ch" and "3ch" in WP and "Weinberg" not in tag): # channel is not 3ch but 3ch in WP --> envelope study

        if "Weinberg" in tag:

          this_name = year+"_"+channel+ID+tag
          batch_dir = this_workdir+"/Asymptotic/"+this_name
          output_dir = batch_dir+"/output"

          print("Parsing Weinberg limit for "+this_name+" ...")

          # Discover w-points from ROOT output files and/or .out files.
          # ROOT file name:
          #   output/Run2_3ch_Weinberg_syst_Asymptotic_wMuMu0p5_wEMu0p5_wEE0p0.root
          # out file name:
          #   Run2_3ch_Weinberg_syst_Asymptotic_wMuMu0p5_wEMu0p5_wEE0p0.out
          labels = set()

          root_pattern = output_dir+"/"+this_name+"_Asymptotic_Expected_wMuMu*_wEMu*_wEE*.root"
          out_pattern  = batch_dir+"/logs/"+this_name+"_Asymptotic_Expected_wMuMu*_wEMu*_wEE*.out"

          for path in glob.glob(root_pattern):
            base = os.path.basename(path)
            label = base.replace(this_name+"_Asymptotic_Expected_", "").replace(".root", "")
            labels.add(label)

          for path in glob.glob(out_pattern):
            base = os.path.basename(path)
            label = base.replace(this_name+"_Asymptotic_Expected_", "").replace(".out", "")
            labels.add(label)

          w_points = []
          for label in labels:
            parsed = parse_weinberg_label(label)
            if parsed is None:
              print("[WARN] Cannot parse Weinberg label: "+label)
              continue

            wMuMu, wEMu, wEE = parsed
            w_points.append((wMuMu, wEMu, wEE, label))

          # Same natural order as make_weinberg_w_points: wMuMu ascending, then wEMu ascending.
          w_points.sort(key=lambda x: (x[0], x[1], x[2]))

          if len(w_points) == 0:
            print("[WARN] No Weinberg output found in "+batch_dir)
            continue

          # Optional raw r-limit output.
          f_r = open("limits/"+WP+"/"+this_name+"_Asym_r_limit.txt", "w")
          f_r.write("w_label\twMuMu\twEMu\twEE\tObs\tExp_m2s\tExp_m1s\tExp\tExp_p1s\tExp_p2s\n")

          # Separate effective Majorana mass limit outputs per flavor.
          f_MuMu = open("limits/"+WP+"/"+this_name+"_Asym_mass_MuMu_limit.txt", "w")
          f_EMu  = open("limits/"+WP+"/"+this_name+"_Asym_mass_EMu_limit.txt",  "w")
          f_EE   = open("limits/"+WP+"/"+this_name+"_Asym_mass_EE_limit.txt",   "w")

          header = "w_label\twMuMu\twEMu\twEE\tObs\tExp_m2s\tExp_m1s\tExp\tExp_p1s\tExp_p2s\n"
          f_MuMu.write(header)
          f_EMu.write(header)
          f_EE.write(header)

          for wMuMu, wEMu, wEE, label in w_points:

            r_line_vals = read_asymptotic_line_values(
              this_workdir,
              this_name,
              "_" + label,
              args.Unblind
            )

            if r_line_vals is None:
              print("[WARN] Missing or invalid Weinberg limit for "+label)
              continue

            prefix = label+"\t"+str(wMuMu)+"\t"+str(wEMu)+"\t"+str(wEE)+"\t"

            # Save raw r limits too. This is useful for debugging.
            f_r.write(prefix)
            f_r.write("\t".join([fmt_limit_value(x, 6) for x in r_line_vals]))
            f_r.write("\n")

            # Convert to effective Majorana mass limits per flavor.
            mass_MuMu_vals = [mass_limit_from_r_and_w(x, wMuMu) for x in r_line_vals]
            mass_EMu_vals  = [mass_limit_from_r_and_w(x, wEMu)  for x in r_line_vals]
            mass_EE_vals   = [mass_limit_from_r_and_w(x, wEE)   for x in r_line_vals]

            f_MuMu.write(prefix)
            f_MuMu.write("\t".join([fmt_limit_value(x, 6) for x in mass_MuMu_vals]))
            f_MuMu.write("\n")

            f_EMu.write(prefix)
            f_EMu.write("\t".join([fmt_limit_value(x, 6) for x in mass_EMu_vals]))
            f_EMu.write("\n")

            f_EE.write(prefix)
            f_EE.write("\t".join([fmt_limit_value(x, 6) for x in mass_EE_vals]))
            f_EE.write("\n")

            print("done: "+label)

          f_r.close()
          f_MuMu.close()
          f_EMu.close()
          f_EE.close()

          continue

        #### Now HNL ####
        with open("limits/"+WP+"/"+year+"_"+channel+ID+tag+"_Asym_limit.txt", 'w') as f:

          # Write header for 2D landscape format
          f.write("Mass\tf\tObs\tExp_m2s\tExp_m1s\tExp\tExp_p1s\tExp_p2s\n")
          
          # Define the f values used in the condor queue
          if channel=="MuMu":
            f_values = ["0.0","0.05","0.1","0.15","0.2","0.25","0.3","0.35","0.4","0.45",
                        "0.5","0.55","0.6","0.65","0.7","0.75","0.8","0.85","0.9","0.95"]
          
          elif channel=="EE":
            f_values = ["0.05","0.1","0.15","0.2","0.25","0.3","0.35","0.4","0.45",
                        "0.5","0.55","0.6","0.65","0.7","0.75","0.8","0.85","0.9","0.95","1.0"]
          
          elif channel=="EMu":
            f_values = ["0.05","0.1","0.15","0.2","0.25","0.3","0.35","0.4","0.45",
                        "0.5","0.55","0.6","0.65","0.7","0.75","0.8","0.85","0.9","0.95"]
          
          else:  # 3ch
            f_values = ["0.0","0.05","0.1","0.15","0.2","0.25","0.3","0.35","0.4","0.45",
                        "0.5","0.55","0.6","0.65","0.7","0.75","0.8","0.85","0.9","0.95","1.0"]

          for mass in masses_EMu:
            if tag=="_sr2_syst_Combined":
              if float(mass)<=500.: continue 

            this_name = year+"_"+channel+"_M"+mass+ID+tag
            if args.BDT:
              if float(mass)>500.: continue
            elif args.Ext:
              if float(mass)<500.: continue
              elif float(mass)==500: 
                this_name = year+"_"+channel+ExtTag+"_M"+mass+ID+tag

            if (float(mass) > 3000.):
              scaler_3ch = 1.
            elif (float(mass) <= 100.):
              scaler_3ch = 0.001
            else:
              scaler_3ch = 0.1
                
            print("Parsing 2D limit for "+this_name+" ...")
            
            for f_val in f_values:
              line_vals = read_asymptotic_line_values(
                this_workdir,
                this_name,
                "_f" + f_val,
                args.Unblind
              )

              if line_vals is None:
                continue

              scaled_vals = [round(x * scaler_3ch, 5) for x in line_vals]

              f.write(mass+"\t"+f_val+"\t")
              f.write("\t".join([str(x) for x in scaled_vals]))
              f.write("\n")

              print("done.")

      #### Flavor-dependent limits ####
      else:

        if "Weinberg" in tag:

          this_name = year+"_"+channel+ID+tag

          print("Parsing single-channel Weinberg limit for "+this_name+" ...")

          r_line_vals = read_asymptotic_line_values(
            this_workdir,
            this_name,
            "",
            args.Unblind
          )

          if r_line_vals is None:
            print("[WARN] Missing or invalid Weinberg limit ROOT file:")
            print("       "+this_name)
            continue

          # For single lepton channel cards, the relevant Weinberg flavor is effectively w_c = 1.
          # So:
          #   mass_c_limit = MASS_REF_WEINBERG * sqrt(r_limit)
          w_this_channel = 1.0
          mass_vals = [mass_limit_from_r_and_w(x, w_this_channel) * 100. for x in r_line_vals] # *100 : to compensate *10000 scale in LimitInput (flavor-dependent setting only; 3ch automatically handles this via --PO r0=10000)

          # Decide output flavor name from channel name.
          if channel == "MuMu":
            flavor_name = "MuMu"
          elif channel == "EMu":
            flavor_name = "EMu"
          elif channel == "EE":
            flavor_name = "EE"
          else:
            print("[WARN] Unknown lepton channel for Weinberg: "+channel)
            continue

          # Save raw signal-strength r limit.
          f_r = open("limits/"+WP+"/"+this_name+"_Asym_r_limit.txt", "w")
          f_r.write("Channel\tObs\tExp_m2s\tExp_m1s\tExp\tExp_p1s\tExp_p2s\n")
          f_r.write(channel+"\t")
          f_r.write("\t".join([fmt_limit_value(x, 6) for x in r_line_vals]))
          f_r.write("\n")
          f_r.close()

          # Save effective Majorana mass limit for the corresponding flavor.
          f_mass = open("limits/"+WP+"/"+this_name+"_Asym_mass_limit.txt", "w")
          f_mass.write("Channel\tObs\tExp_m2s\tExp_m1s\tExp\tExp_p1s\tExp_p2s\n")
          f_mass.write(channel+"\t")
          f_mass.write("\t".join([fmt_limit_value(x, 6) for x in mass_vals]))
          f_mass.write("\n")
          f_mass.close()

          print("done: "+this_name)
          continue

        #### Now HNL ####
        with open("limits/"+WP+"/"+year+"_"+channel+ID+tag+"_Asym_limit.txt", 'w') as f:
        #with open("limits/"+WP+"/"+year+"_"+channel+ID+tag+"_Run2Scaled_Asym_limit.txt", 'w') as f:
        #with open("limits/"+WP+"/"+year+"_"+channel+ID+tag+"_Run23Scaled_Asym_limit.txt", 'w') as f:
  
          for mass in (masses if channel!="EMu" else masses_EMu):
            if tag=="_sr2_syst_Combined":
              if float(mass)<=500.: continue # no significantly meaningful to check M125-500 SR2 limit (they exist though... just in case)

            this_name = year+"_"+channel+"_M"+mass+ID+tag
            if args.BDT:
              if float(mass)>500.: continue
            elif args.Ext:
              if float(mass)<500.:
                f.write("\n")
                continue
              elif float(mass)==500: 
                this_name = year+"_"+channel+ExtTag+"_M"+mass+ID+tag
            print(this_name)

            line_vals = read_asymptotic_line_values(
              this_workdir,
              this_name,
              "",
              args.Unblind
            )

            if line_vals is None:
              f.write("\n")
              continue

            f.write(mass+"\t")
            f.write("\t".join([str(round(x, 3)) for x in line_vals]))
            f.write("\n")

            print("done.")
  
    if args.Full:
      if channel=="3ch":
        print("3ch doesn't support full CLs. skipping...")
        continue

      with open("out/"+WP+"/"+year+"_"+channel+ID+tag+"_Full_limit.txt", 'w') as f:
  
        for mass in (masses if channel!="EMu" else masses_EMu):
          if tag=="_sr2_syst_Combined":
            if float(mass)<=500.: continue # no significantly meaningful to check M125-500 SR2 limit (they exist though... just in case)

          this_name = year+"_"+channel+"_M"+mass+ID+tag
          if args.BDT:
            if float(mass)>500.: continue
          elif args.Ext:
            if float(mass)<500.:
              f.write("\n")
              continue
            elif float(mass)==500: 
              this_name = year+"_"+channel+ExtTag+"_M"+mass+ID+tag
          print(this_name)
          paths = [
                  this_workdir+"/full_CLs/"+this_name+"/output/"+this_name+"_Q1.root",
                  this_workdir+"/full_CLs/"+this_name+"/output/"+this_name+"_Q2.root",
                  this_workdir+"/full_CLs/"+this_name+"/output/"+this_name+"_Q3.root",
                  this_workdir+"/full_CLs/"+this_name+"/output/"+this_name+"_Q4.root",
                  this_workdir+"/full_CLs/"+this_name+"/output/"+this_name+"_Q5.root",
                  ]
  
          f_Q1 = TFile.Open(paths[0])
          f_Q2 = TFile.Open(paths[1])
          f_Q3 = TFile.Open(paths[2])
          f_Q4 = TFile.Open(paths[3])
          f_Q5 = TFile.Open(paths[4])
  
          tree_Q1 = f_Q1.Get("limit")
          tree_Q2 = f_Q2.Get("limit")
          tree_Q3 = f_Q3.Get("limit")
          tree_Q4 = f_Q4.Get("limit")
          tree_Q5 = f_Q5.Get("limit")
          
          tree_Q1.GetEntry(0)
          tree_Q2.GetEntry(0)
          tree_Q3.GetEntry(0)
          tree_Q4.GetEntry(0)
          tree_Q5.GetEntry(0)
          
          f.write(mass+"\t"+str(round(tree_Q3.limit,3))+"\t"+str(round(tree_Q1.limit,3))+"\t"+str(round(tree_Q2.limit,3))+"\t"+str(round(tree_Q3.limit,3))+"\t"+str(round(tree_Q4.limit,3))+"\t"+str(round(tree_Q5.limit,3))+"\n")
