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
myWPs = ["ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188"]

#tags = ["_sronly_syst"]
#tags = ["_sronly"]
#tags = ["_syst"]
#tags = ["_HNL_syst"]
#tags = ["_Weinberg_syst"]
#tags = ["_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined"]
#tags = ["_HNL_sr1_syst_Combined","_HNL_sr2_syst_Combined","_HNL_sr3_syst_Combined"]
#tags = ["_DYVBF_syst","_DY_syst","_VBF_syst","_SSWW_syst"]
tags = ["_DYVBF_syst"]
#tags = ["_sronly_sr123_syst"]
#tags = ["_sronly_sr123"]
#tags = ["_DYVBF_sronly_sr123_syst"]
#tags = ["_syst","_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined"]
#tags = ["_DY_syst","_VBF_syst","_SSWW_syst","_HNL_sr1_syst_Combined","_HNL_sr2_syst_Combined","_HNL_sr3_syst_Combined","_Weinberg_sr2_syst_Combined","_Weinberg_sr3_syst_Combined"]
#tags = ["_HNL_syst","_Weinberg_syst"]
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


def read_asymptotic_r_values(root_path, out_path=None):
  """
  Return:
    [Exp_m2s, Exp_m1s, Exp, Exp_p1s, Exp_p2s]

  Prefer ROOT output. If ROOT file is missing or broken, optionally fall back
  to parsing the .out file.
  """

  # 1. Preferred: read ROOT tree
  if os.path.exists(root_path):
    try:
      f_Asym = TFile.Open(root_path)

      if f_Asym and not f_Asym.IsZombie():
        tree_Asym = f_Asym.Get("limit")

        if tree_Asym:
          if int(tree_Asym.GetEntries()) >= 5:
            vals = []
            for i in range(5):
              tree_Asym.GetEntry(i)
              vals.append(float(tree_Asym.limit))

            f_Asym.Close()
            return vals

      if f_Asym:
        f_Asym.Close()

    except Exception:
      pass

  # 2. Fallback: parse .out file
  if out_path and os.path.exists(out_path):
    try:
      txt = open(out_path).read()

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
          return None
        vals.append(float(m.group(1)))

      return vals

    except Exception:
      return None

  return None


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

          root_pattern = output_dir+"/"+this_name+"_Asymptotic_wMuMu*_wEMu*_wEE*.root"
          out_pattern  = batch_dir+"/"+this_name+"_Asymptotic_wMuMu*_wEMu*_wEE*.out"

          for path in glob.glob(root_pattern):
            base = os.path.basename(path)
            label = base.replace(this_name+"_Asymptotic_", "").replace(".root", "")
            labels.add(label)

          for path in glob.glob(out_pattern):
            base = os.path.basename(path)
            label = base.replace(this_name+"_Asymptotic_", "").replace(".out", "")
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

            root_path = output_dir+"/"+this_name+"_Asymptotic_"+label+".root"
            out_path  = batch_dir+"/"+this_name+"_Asymptotic_"+label+".out"

            r_vals = read_asymptotic_r_values(root_path, out_path)

            if r_vals is None:
              print("[WARN] Missing or invalid Weinberg limit for "+label)
              continue

            # r_vals = [Exp_m2s, Exp_m1s, Exp, Exp_p1s, Exp_p2s]
            # For consistency with existing code, use Exp median as Obs placeholder.
            r_obs = r_vals[2]
            r_line_vals = [r_obs] + r_vals

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
              # Construct path mapping to create-batch.py output format
              path = this_workdir+"/Asymptotic/"+this_name+"/output/"+this_name+"_Asymptotic_f"+f_val+".root"
              
              try: 
                f_Asym = TFile.Open(path)
                if not f_Asym or f_Asym.IsZombie():
                  continue
              except Exception:
                continue
                
              tree_Asym = f_Asym.Get("limit")
              if not tree_Asym:
                f_Asym.Close()
                continue
                
              try: 
                tree_Asym.GetEntry(2) # Fallback to entry 2 for 'obs' placeholder as in 1D code
                obs_val = tree_Asym.limit
              except AttributeError:
                f_Asym.Close()
                continue
                
              # Write Mass and f value first
              f.write(mass+"\t"+f_val+"\t"+str(round(obs_val*scaler_3ch, 5))+"\t")
              
              # Write 5 expected limit values
              for i in range(5): 
                tree_Asym.GetEntry(i)
                f.write(str(round(tree_Asym.limit*scaler_3ch, 5))+"\t")
                
              f.write("\n")
              f_Asym.Close()
              print("done.")

      #### Flavor-dependent limits ####
      else:

        if "Weinberg" in tag:

          this_name = year+"_"+channel+ID+tag

          print("Parsing single-channel Weinberg limit for "+this_name+" ...")

          # Example:
          #   Batch/<WP>/Asymptotic/Run2_MuMu_Weinberg_syst/output/Run2_MuMu_Weinberg_syst_Asymptotic.root
          path = this_workdir+"/Asymptotic/"+this_name+"/output/"+this_name+"_Asymptotic.root"

          r_vals = read_asymptotic_r_values(path)

          if r_vals is None:
            print("[WARN] Missing or invalid Weinberg limit ROOT file:")
            print("       "+path)
            continue

          # r_vals = [Exp_m2s, Exp_m1s, Exp, Exp_p1s, Exp_p2s]
          # Same convention as the existing HNL code:
          # use median expected as Obs placeholder because --run blind has no real observed limit.
          r_obs = r_vals[2]
          r_line_vals = [r_obs] + r_vals

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
            path = this_workdir+"/Asymptotic/"+this_name+"/output/"+this_name+"_Asymptotic.root"
  
            try: f_Asym = TFile.Open(path)
            except OSError:
              f.write("\n")
              continue
            tree_Asym = f_Asym.Get("limit")
  
            try: tree_Asym.GetEntry(2) # substitute for obs. limit for now
            except AttributeError:
              f.write("\n")
              continue
            f.write(mass+"\t"+str(round(tree_Asym.limit,3))+"\t")
            #f.write(mass+"\t"+str(round(tree_Asym.limit/1.82,3))+"\t") # FIXME estimating full Run2 from 2017
            #f.write(mass+"\t"+str(round(tree_Asym.limit/1.52,3))+"\t") # FIXME estimating full Run2 from 2018
            #f.write(mass+"\t"+str(round(tree_Asym.limit/3.16,3))+"\t") # FIXME estimating full Run2+3 from 2017
            #f.write(mass+"\t"+str(round(tree_Asym.limit/1.77,3))+"\t") # FIXME estimating full Run2+3 from Run2
  
            for i in range(5): # expected limits
              tree_Asym.GetEntry(i)
              f.write(str(round(tree_Asym.limit,3))+"\t")
              #f.write(str(round(tree_Asym.limit/1.82,3))+"\t") # FIXME estimating full Run2 from 2017
              #f.write(str(round(tree_Asym.limit/1.52,3))+"\t") # FIXME estimating full Run2 from 2018
              #f.write(str(round(tree_Asym.limit/3.16,3))+"\t") # FIXME estimating full Run2+3 from 2017
              #f.write(str(round(tree_Asym.limit/1.77,3))+"\t") # FIXME estimating full Run2+3 from Run2
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
