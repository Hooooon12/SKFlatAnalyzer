# Make Datacards
# Place this at CombineTool/CMSSW_10_2_13/src/<your working directory>
# You need to place card_skeletons already
# python MakeDataCard_ANv5.py --CR --Syst [--Decorr]; python MakeDataCard_ANv5.py --Combine CR --Syst [--Decorr] <-- add rateParam
# python MakeDataCard_ANv5.py --Combine Era --Syst [--Decorr]
# python MakeDataCard_ANv5.py --Syst [--Decorr]; python MakeDataCard_ANv5.py --Combine SR --Syst [--Decorr] <-- without rateParam ("sronly" setting)


import os, sys, argparse
from collections import OrderedDict

parser = argparse.ArgumentParser(description='script for creating or merging data cards.',formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('--CnC', action='store_true', help='Cut and count limit')
parser.add_argument('--Decorr', action='store_true', help='Decorrelate fake, CF region by region')
parser.add_argument('--JetDecorr', action='store_true', help='Decorrelate jet scale/res additionally')
parser.add_argument('--CR', action='store_true', help='Make datacards named sr with HNL_SignalRegion_Plotter and sr_inv with HNL_ControlRegion_Plotter input. (Default : SR only)')
#parser.add_argument('--CR', nargs='*', help='Make datacards with manual CR inputs. (Default : SR only)') # Modify L108 with this line
parser.add_argument('--Syst', action='store_true', help='Add systematics into the datacards')
parser.add_argument('--Combine', choices=['CR','SR','Era'], help='CR --> Merge CR and SR datacards in one era,\nEra --> Merge pre-processed (CR+SR) over the Run2,\nSR --> Merge SR only datacards over the Run2')
parser.add_argument('--Type', choices=['CR', 'SR'], help="(Optional) If --Combine Era is used, specify whether to merge only CR or SR.")
args = parser.parse_args()

pwd = os.getcwd()

#####################################################
#
# args.CR --> sr, sr_inv connected via rateParam
# else --> sr only, bkg norm uncert. treated by lnN
# args.syst --> postpone
#
#####################################################

#eras = ["2016","2017","2018"]
#eras = ["2017"]
#eras = ["2018"]
eras = ["2016preVFP","2016postVFP","2017","2018"]

channels = ["MuMu","EE","EMu"]
#channels = ["MuMu","EE"]
#channels = ["EE"]
#channels = ["EE","EMu"]
#channels = ["MuMu"]
#masses = ["M90","M100","M150","M200","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000"]
#masses = ["M100","M1000","M10000"]
#masses = ["M500","M1000","M5000"]
#masses = ["M3000"]
#masses = ["M90","M100","M150","M200","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000"]
#masses = ["M85","M90","M95","M100"]#"M125","M150","M200","M250","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000"]
#masses = ["M100","M500","M1000","M3000","M10000"]
#masses = ["M3000","M5000","M7500","M10000","M15000","M20000"]
#masses = ["M100"]
#masses = ["M250","M1000","M10000"]
#masses_EMu = ["M250","M1000","M10000"]
#masses = ["M100","M250","M1000","M10000"]
#masses_EMu = ["M100","M250","M1000","M10000"]
#masses = ["M100","M500","M1000","M1500","M5000"]
#masses_EMu = ["M100","M500","M1000","M1500","M5000"]
#masses = ["M1500"]
#masses_EMu = ["M1500"]
#masses = ["M85","M90","M95","M100"]
#masses_EMu = ["M85","M90","M95","M100"]
#masses = ["Weinberg"]
#masses_EMu = ["Weinberg"]

## Full mass ranges
masses = ["M85","M90","M95","M100","M125","M150","M200","M250","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000","M25000","M30000"]
masses_EMu = ["M85","M90","M95","M100","M125","M150","M200","M250","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000","M25000","M30000","M40000","M50000","M60000"]
#masses = ["M85","M90","M95","M100","M125","M150","M200","M250","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000"]
#masses_EMu = ["M85","M90","M95","M100","M125","M150","M200","M250","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000"]

## signal processes
#signals = ["_DYVBF","_SSWW"]
signals = [""]
#signals = ["_DYVBF","_SSWW",""]
#signals = ["_SSWW"]
#signals = ["_Weinberg"]

#SRpath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_ANv3/LimitExtraction/"
#CRpath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter_ANv3/LimitExtraction/"
#SRpath = "/data9/Users/jalmond_public/SUS-24-014/LimitInputs/"
#CRpath = "/data9/Users/jalmond_public/SUS-24-014/LimitInputs/"
SRpath = "/data9/Users/HNL_public/SUS-24-014//LimitInputs/"
CRpath = SRpath

#InputWPs = ["ANv3_HNL_ULIDv2"]
#InputWPs = ["HEMJet_HNL_ULIDv2_RemoveHEMJet","HEMJet_HNL_ULIDv2_ScaleHEMJet","TuneP_HNL_ULIDv2_CompareTuneP","TuneP_HNTightV2_CompareTuneP","TuneP_POGTight_CompareTuneP"]
#InputWPs = ["ANv4_HNL_ULIDv2_RunSyst"]
#InputWPs = ["ANv5_HNL_ULIDv2_RunSyst_BeforeJetIDLepPt"]
#InputWPs = ["ANv5_HNL_ULIDv2_RunSyst"]
#InputWPs = ["ANv5_HNL_ULIDv2_RunSyst","ANv5_BDTV3_LooseBinning_HNL_ULIDv2_RunSyst","ANv5_BDTV3_StrictBinning_HNL_ULIDv2_RunSyst"]
#InputWPs = ["ANv5_BDTV4_LooseBinning_HNL_ULIDv2_RunSyst","ANv5_BDTV4_StrictBinning_HNL_ULIDv2_RunSyst"]
#InputWPs = ["ANv5_BDTV4_VeryLooseBinning_HNL_ULIDv2_RunSyst"]
#InputWPs = ["ANv5_BDTV4_BugFix_HNL_ULIDv2_V4_LooseBin_RunSyst","ANv5_BDTV4_BugFix_HNL_ULIDv2_V4_StrictBin_RunSyst"]
InputWPs = ["ANv5_BDTV4_BugFix_HNL_ULIDv2_V3_LooseBin_RunSyst","ANv5_BDTV4_BugFix_HNL_ULIDv2_V3_StrictBin_RunSyst"]

RegionDecorr_list = ["CMS_fake_stat","CMS_fake_highpt","CMS_fake_syst","CMS_cf_stat","CMS_cf_syst"]

if args.CnC:
  InputWPs = [WP+"_CnC" for WP in InputWPs]
if args.Syst:
  if args.Decorr:
    InputWPs = [WP+"_Decorr" for WP in InputWPs]
    if args.JetDecorr:
      InputWPs = [WP+"_JetDecorr" for WP in InputWPs]
      RegionDecorr_list.append("CMS_res_j")
      RegionDecorr_list.append("CMS_scale_j")
else:
  InputWPs = [WP+"_Decorr_JetDecorr" for WP in InputWPs] # FIXME use syst input as a default; can be changed later

#OutputTag = "_AN"
#OutputTag = "_SUScomment"
OutputTag = ""
#OutputTag = "_BD"
#OutputTag = "_Run2"

if args.Combine is None:
  if not args.CR: OutputTag+="_NoCR" # SR only
else:
  if not (args.Combine == "CR" or (args.Combine == "Era" and args.CR)): OutputTag+="_NoCR" # Combine SR only
if not args.Syst: OutputTag+="_NoSyst"  # NoSyst

regions_cr = ["sr1_InvMET","sr2_InvMET","sr3_InvMET","sr1_InvBJet","sr2_InvBJet","sr3_InvBJet","wz_cr1","wz_cr2","wz_cr3","zg_cr","zz_cr"]
#regions_cr = args.CR # input from the user
regions_sr = ["sr1","sr2","sr3"]
regions_tot = regions_cr+regions_sr

#if args.CR: # TEST purposes, to compare the impact of each CRs to limits
#  if any("sr1_inv" in region for region in regions_cr ): OutputTag+="_Inv1"
#  if any("sr2_inv" in region for region in regions_cr ): OutputTag+="_Inv2"
#  if any("sr3_inv" in region for region in regions_cr ): OutputTag+="_Inv3"
#  if any("ww" in region for region in regions_cr ): OutputTag+="_WW"
#  if any("zg" in region for region in regions_cr ): OutputTag+="_ZG"
#  if any("zz" in region for region in regions_cr ): OutputTag+="_ZZ"
#  if any("wz_cr1" in region for region in regions_cr ): OutputTag+="_WZ1"
#  if any("wz_cr2" in region for region in regions_cr ): OutputTag+="_WZ2"
#  if any("wz_cr3" in region for region in regions_cr ): OutputTag+="_WZ3"

################################################################################################################################################

def Initialize_Process():
    return OrderedDict([
        ('fake', '-1'),
        ('cf', '-1'),
        ('zg', '-1'),
        ('conv_others', '-1'), # FIXME conv_others + prompt_others = mc_others later
        ('wz', '-1'),
        ('zz', '-1'),
        ('ww', '-1'),
        ('prompt_others', '-1'),
        ('signalDYVBF', '-1'),
        ('signalSSWW', '-1'),
        ('signalWeinberg', '-1'),
    ])

def MakeRateString(region, mass, channel, signal):
  this_process = Initialize_Process()

  if "Mu" in channel:
    this_process['cf'] = '0'

  is_Weinberg = (mass == "Weinberg")

  #FIXME separate out DY and VBF later
  if not is_Weinberg:
    this_process['signalWeinberg'] = '0'

    this_mass_value = int(mass.replace("M", ""))
    if this_mass_value < 500:
      this_process['signalSSWW'] = '0'
    elif 500 <= this_mass_value <= 3000:
      if "DY" in signal or "VBF" in signal:
        this_process['signalSSWW'] = '0'
      elif "SSWW" in signal:
        this_process['signalDYVBF'] = '0'
    else:  # mass_value > 3000
      this_process['signalDYVBF'] = '0'
  else:
    this_process['signalDYVBF'] = '0'
    this_process['signalSSWW'] = '0'

  #FIXME current setting: No signal in CR
  if region in regions_cr:
    this_process['signalDYVBF'] = '0'
    this_process['signalSSWW'] = '0'
    this_process['signalWeinberg'] = '0'

  formatted_values = [
      value.ljust(13)
      for key, value in list(this_process.items())
  ]

  this_string = "rate                          "+" ".join(formatted_values)+"\n"
  return this_string

def is_syst_line(line):
  return (line.startswith("lumi_") or line.startswith("mc_") or line.startswith("CMS_") or line.startswith("QCDscale_") or line.startswith("pdf_"))

def is_rateParam_line(line):
  return "rateParam" in line

def CardSetting(isCR, WP, era, channel, mass, signal):

  with open("card_skeleton_ANv5.txt",'r') as f: # open skeleton
    lines = f.readlines()

  new_lines_common = []

  lines_cr = {}
  lines_sr = {}
  lines_sronly = {}

  for region in regions_tot:
    this_lines = lines[:]
    new_lines = []

    # preprocess lines
    this_lines[4] = "shapes * *  "+CRpath+WP+"/"+era+"/"+region+"/"+mass+"_"+channel+"_card_input.root $PROCESS $PROCESS_$SYSTEMATIC\n" if region in regions_cr else "shapes * *  "+SRpath+WP+"/"+era+"/"+region+"/"+mass+"_"+channel+"_card_input.root $PROCESS $PROCESS_$SYSTEMATIC\n"
    this_lines[17] = MakeRateString(region, mass, channel, signal)
    for i in range(len(this_lines)):
      this_lines[i] = this_lines[i].replace('bin1',region)
    for i in range(18):
      new_lines.append(this_lines[i])

    # handle each syst
    if args.Syst:
      for line in this_lines[18:]:
        if is_syst_line(line):
          syst_name = line.split()[0]

          # era-correlated systs
          if any(key in syst_name for key in ["xsec", "pileup", "QCDscale", "pdf", "_corr"]):
            new_lines.append(line)
            continue
          # era-decorrelation
          else:
            line = line.replace(syst_name, f"{syst_name}_{era}")

          # channel treatment
          if channel=="MuMu":
            if ("eff_e_" in syst_name) or ("scale_e" in syst_name) or ("res_e" in syst_name) or ("fake_highpt" in syst_name): continue
          elif channel=="EE":
            if ("eff_m_" in syst_name) or (syst_name == "CMS_scale_m") or ("res_m" in syst_name): continue

        elif is_rateParam_line(line):
          line = line.replace("Norm", f"Norm{era}")
          
        new_lines.append(line)

    # finally do the region decorrelation
    if args.Decorr:
      for i in range(len(new_lines)):
        if new_lines[i].startswith(tuple(RegionDecorr_list)):
          this_syst = new_lines[i].split(' ')[0]

          if "sr_" in region: pass # sr_inv (sr combined)
          elif region=="sr": pass # sr combined
          elif "sr" in region: # sr1, sr1_InvBJet, etc.
            decorr_region = region.split('_')[0]
            new_lines[i] = new_lines[i].replace(this_syst,this_syst+'_'+decorr_region) # FR to FR_sr1
          elif "cr1" in region or "cr2" in region or "cr3" in region: # wz_cr2 etc.
            if 'sr1' in region or 'cr1' in region:
              regionName_SystSep = 'sr1'
            elif 'sr2' in region or 'cr2' in region:
              regionName_SystSep = 'sr2'
            elif 'sr3' in region or 'cr3' in region:
              regionName_SystSep = 'sr3'
            new_lines[i] = new_lines[i].replace(this_syst,this_syst+'_'+regionName_SystSep) # FR to FR_sr2
          else: # zg_cr, zz_cr
            new_lines[i] = new_lines[i].replace(this_syst,this_syst+'_sr3') # correlate to sr3

    ####### finally adjust columns using ljust ########


    if region in regions_cr:
      lines_cr[region] = new_lines
    elif region in regions_sr:
      lines_sr[region] = new_lines
      # no rateParam setting
      for i in range(18,len(new_lines)):
        if is_rateParam_line(new_lines[i]):
          new_lines[i] = ""
      lines_sronly[region] = new_lines

  if isCR:
    return (lines_sr, lines_cr)
  else:
    return lines_sronly
 
def ValidMassSignal(mass, signal):
  # Check Weinberg first
  if mass=="Weinberg":
    if "Weinberg" in signal: return True
    else: return False

  # Now N mass
  else:
    if "Weinberg" in signal: return False
    if signal=="": return True # MakeRateString will handle this

    if int(mass.strip('M'))<300:
      if "DY" in signal:
        return True
      else:
        return False
    elif int(mass.strip('M'))<500:
      if "DY" in signal or "VBF" in signal:
        return True
      else:
        return False
    elif int(mass.strip('M'))<=3000:
      return True
    else:
      if "DY" in signal or "VBF" in signal:
        return False
      else:
        return True

def NuisanceGrouping(this_card):
  print("Grouping",this_card,"...")

  with open(this_card,'r') as f:
    lines = f.readlines()

  with open(this_card,'w') as f:
    for line in lines[:]:
      if "group" not in line:
        f.write(line)
  with open(this_card,'r') as f:
    lines = f.readlines()

  group_nuis = {
                'lumi'        : [],
                'xsec'        : [],
                'theory'      : [],
                'fake'        : [],
                'cf'          : [],
                'jet_energy'  : [],
                'lep_uncert'  : [],
                'btag_sf'     : [],
                'met_energy'  : [],
                'prefire'     : [],
                'pileup'      : [],
  }
  for line in lines[:]:
    line = line.split(' ')[0]
    if "lumi" in line:
      group_nuis["lumi"].append(line)
    elif "xsec" in line:
      group_nuis["xsec"].append(line)
    elif "QCD" in line or "pdf" in line:
      group_nuis["theory"].append(line)
    elif "CMS_fake" in line:
      group_nuis["fake"].append(line)
    elif "CMS_cf" in line:
      group_nuis["cf"].append(line)
    elif "CMS_res_j" in line or "CMS_scale_j" in line:
      group_nuis["jet_energy"].append(line)
    elif ("CMS_res_m" in line or "CMS_scale_m" in line or "CMS_eff_m" in line or "CMS_res_e" in line or "CMS_scale_e" in line or "CMS_eff_e" in line) and "CMS_scale_met" not in line:
      group_nuis["lep_uncert"].append(line)
    elif "CMS_btag" in line:
      group_nuis["btag_sf"].append(line)
    elif "CMS_scale_met" in line:
      group_nuis["met_energy"].append(line)
    elif "CMS_l1_ecal_prefiring" in line:
      group_nuis["prefire"].append(line)
    elif "CMS_pileup" in line:
      group_nuis["pileup"].append(line)

  with open(this_card,'a') as f:
    for key, value in list(group_nuis.items()):
      if len(value)!=0:
        f.write(key+" group = "+" ".join(value)+'\n')

  return

#########################################
#
# MAIN
#
#########################################

if args.Syst:
  systTag = "_syst"
else:
  systTag = ""

for InputWP in InputWPs:
  OutputWP = InputWP+OutputTag

  if not args.Combine:
    os.system("mkdir -p "+OutputWP)
    os.system("ln -s /data6/Users/jihkim/SKFlatAnalyzer/script/DataCard/MakeWorkspace.py "+OutputWP)
    os.system("ln -s /data6/Users/jihkim/SKFlatAnalyzer/script/DataCard/CheckNuisance.py "+OutputWP)

    for era, channel, mass, signal in [(era, channel, mass, signal) for era in eras for channel in channels for mass in (masses if channel!="EMu" else masses_EMu) for signal in signals]:
      if not ValidMassSignal(mass, signal): continue

      mass_signal = mass if signal.strip('_') in mass else mass + signal # Remove duplication like Weinberg_Weinberg

      this_card = CardSetting(args.CR, InputWP, era, channel, mass, signal)
      if args.CR:
        for region in list(this_card[0].keys()):
          with open(OutputWP+"/card_"+era+"_"+channel+"_"+mass_signal+"_"+region+systTag+".txt",'w') as f:
            for line in this_card[0][region]:
              f.write(line)
        for region in list(this_card[1].keys()):
          with open(OutputWP+"/card_"+era+"_"+channel+"_"+mass_signal+"_"+region+".txt",'w') as f:
            for line in this_card[1][region]:
              f.write(line)
      else:
        for region in list(this_card.keys()):
          with open(OutputWP+"/card_"+era+"_"+channel+"_"+mass_signal+"_sronly_"+region+systTag+".txt",'w') as f:
            for line in this_card[region]:
              f.write(line)

  else:
    os.chdir(OutputWP)
    os.system('echo \'Currently combining cards at...\'')
    os.system('pwd')
    if args.Syst:
      os.system('echo \'Systematics have been added.\'')
    for channel, mass, signal in [(channel, mass, signal) for channel in channels for mass in (masses if channel!="EMu" else masses_EMu) for signal in signals]:
      if not ValidMassSignal(mass, signal): continue

      mass_signal = mass if signal.strip('_') in mass else mass + signal # Remove duplication like Weinberg_Weinberg

      for era in eras:
        if args.Combine == "CR":
          if mass!="Weinberg" and int(mass.strip('M'))<=100:
            regions_sr_filtered = ["sr3"]
            regions_cr_filtered = [cr for cr in regions_cr if 'sr1' not in cr and 'sr2' not in cr and 'cr1' not in cr and 'cr2' not in cr]
          else:
            regions_sr_filtered = regions_sr[:]
            regions_cr_filtered = regions_cr[:]
          sr_combine = " ".join([sr+"=card_"+era+"_"+channel+"_"+mass_signal+"_"+sr+systTag+".txt" for sr in regions_sr_filtered])
          cr_combine = " ".join([cr+"=card_"+era+"_"+channel+"_"+mass_signal+"_"+cr+".txt" for cr in regions_cr_filtered])
          # merge all SRs
          os.system("combineCards.py "+sr_combine+" "+cr_combine+" > card_"+era+"_"+channel+"_"+mass_signal+systTag+".txt")

          # Now make limit from each SRs
          ## make SR3 first which always exist regardless of mass
          sr3_combine = "sr3=card_"+era+"_"+channel+"_"+mass_signal+"_sr3"+systTag+".txt"
          cr3_combine = " ".join([cr+"=card_"+era+"_"+channel+"_"+mass_signal+"_"+cr+".txt" for cr in regions_cr_filtered if 'sr1' not in cr and 'sr2' not in cr and 'cr1' not in cr and 'cr2' not in cr])
          os.system("combineCards.py "+sr3_combine+" "+cr3_combine+" > card_"+era+"_"+channel+"_"+mass_signal+"_sr3"+systTag+"_Combined.txt")

          if args.Syst:
            NuisanceGrouping(os.path.abspath("card_"+era+"_"+channel+"_"+mass_signal+systTag+".txt"))
            NuisanceGrouping(os.path.abspath("card_"+era+"_"+channel+"_"+mass_signal+"_sr3"+systTag+"_Combined.txt"))

          if mass=="Weinberg" or int(mass.strip('M'))>100:
            # SR1, SR2 exists only for M(N) > 100 GeV
            sr1_combine = "sr1=card_"+era+"_"+channel+"_"+mass_signal+"_sr1"+systTag+".txt"
            cr1_combine = " ".join([cr+"=card_"+era+"_"+channel+"_"+mass_signal+"_"+cr+".txt" for cr in regions_cr_filtered if 'sr2' not in cr and 'sr3' not in cr and 'cr2' not in cr and 'cr3' not in cr])
            #print(regions_cr)
            #print(cr1_combine)
            os.system("combineCards.py "+sr1_combine+" "+cr1_combine+" > card_"+era+"_"+channel+"_"+mass_signal+"_sr1"+systTag+"_Combined.txt")
            sr2_combine = "sr2=card_"+era+"_"+channel+"_"+mass_signal+"_sr2"+systTag+".txt"
            cr2_combine = " ".join([cr+"=card_"+era+"_"+channel+"_"+mass_signal+"_"+cr+".txt" for cr in regions_cr_filtered if 'sr1' not in cr and 'sr3' not in cr and 'cr1' not in cr and 'cr3' not in cr])
            os.system("combineCards.py "+sr2_combine+" "+cr2_combine+" > card_"+era+"_"+channel+"_"+mass_signal+"_sr2"+systTag+"_Combined.txt")

            if args.Syst:
              NuisanceGrouping(os.path.abspath("card_"+era+"_"+channel+"_"+mass_signal+"_sr1"+systTag+"_Combined.txt"))
              NuisanceGrouping(os.path.abspath("card_"+era+"_"+channel+"_"+mass_signal+"_sr2"+systTag+"_Combined.txt"))

        elif args.Combine == "SR": # Combine SR1 only, SR2 only, SR3 only (no rateParam)
          if mass!="Weinberg" and int(mass.strip('M'))<=100:
            os.system("combineCards.py \
                                       sr3=card_"+era+"_"+channel+"_"+mass_signal+"_sronly_sr3"+systTag+".txt \
                                       > card_"+era+"_"+channel+"_"+mass_signal+"_sronly_sr123"+systTag+".txt")
          else:
            os.system("combineCards.py \
                                       sr1=card_"+era+"_"+channel+"_"+mass_signal+"_sronly_sr1"+systTag+".txt \
                                       sr2=card_"+era+"_"+channel+"_"+mass_signal+"_sronly_sr2"+systTag+".txt \
                                       sr3=card_"+era+"_"+channel+"_"+mass_signal+"_sronly_sr3"+systTag+".txt \
                                       > card_"+era+"_"+channel+"_"+mass_signal+"_sronly_sr123"+systTag+".txt")

          if args.Syst:
            NuisanceGrouping(os.path.abspath("card_"+era+"_"+channel+"_"+mass_signal+"_sronly_sr123"+systTag+".txt"))

      if args.Combine == "Era":
        if args.CR: # with CR
          # Full SR limit
          os.system("combineCards.py year16a=card_2016preVFP_"+channel+"_"+mass_signal+systTag+".txt year16b=card_2016postVFP_"+channel+"_"+mass_signal+systTag+".txt year17=card_2017_"+channel+"_"+mass_signal+systTag+".txt year18=card_2018_"+channel+"_"+mass_signal+systTag+".txt > card_Run2_"+channel+"_"+mass_signal+systTag+".txt")
          # SR3 limit
          os.system("combineCards.py year16a=card_2016preVFP_"+channel+"_"+mass_signal+"_sr3"+systTag+"_Combined.txt year16b=card_2016postVFP_"+channel+"_"+mass_signal+"_sr3"+systTag+"_Combined.txt year17=card_2017_"+channel+"_"+mass_signal+"_sr3"+systTag+"_Combined.txt year18=card_2018_"+channel+"_"+mass_signal+"_sr3"+systTag+"_Combined.txt > card_Run2_"+channel+"_"+mass_signal+"_sr3"+systTag+"_Combined.txt")
          if args.Syst:
            NuisanceGrouping(os.path.abspath("card_Run2_"+channel+"_"+mass_signal+systTag+".txt")) # Full
            NuisanceGrouping(os.path.abspath("card_Run2_"+channel+"_"+mass_signal+"_sr3"+systTag+"_Combined.txt")) # SR3

          if mass=="Weinberg" or int(mass.strip('M'))>100:
            # SR1, 2 limit
            os.system("combineCards.py year16a=card_2016preVFP_"+channel+"_"+mass_signal+"_sr1"+systTag+"_Combined.txt year16b=card_2016postVFP_"+channel+"_"+mass_signal+"_sr1"+systTag+"_Combined.txt year17=card_2017_"+channel+"_"+mass_signal+"_sr1"+systTag+"_Combined.txt year18=card_2018_"+channel+"_"+mass_signal+"_sr1"+systTag+"_Combined.txt > card_Run2_"+channel+"_"+mass_signal+"_sr1"+systTag+"_Combined.txt")
            os.system("combineCards.py year16a=card_2016preVFP_"+channel+"_"+mass_signal+"_sr2"+systTag+"_Combined.txt year16b=card_2016postVFP_"+channel+"_"+mass_signal+"_sr2"+systTag+"_Combined.txt year17=card_2017_"+channel+"_"+mass_signal+"_sr2"+systTag+"_Combined.txt year18=card_2018_"+channel+"_"+mass_signal+"_sr2"+systTag+"_Combined.txt > card_Run2_"+channel+"_"+mass_signal+"_sr2"+systTag+"_Combined.txt")
            if args.Syst:
              NuisanceGrouping(os.path.abspath("card_Run2_"+channel+"_"+mass_signal+"_sr1"+systTag+"_Combined.txt"))
              NuisanceGrouping(os.path.abspath("card_Run2_"+channel+"_"+mass_signal+"_sr2"+systTag+"_Combined.txt"))
        else: # SR only
          # Full SR limit
          os.system("combineCards.py year16a=card_2016preVFP_"+channel+"_"+mass_signal+"_sronly_sr123"+systTag+".txt year16b=card_2016postVFP_"+channel+"_"+mass_signal+"_sronly_sr123"+systTag+".txt year17=card_2017_"+channel+"_"+mass_signal+"_sronly_sr123"+systTag+".txt year18=card_2018_"+channel+"_"+mass_signal+"_sronly_sr123"+systTag+".txt > card_Run2_"+channel+"_"+mass_signal+"_sronly_sr123"+systTag+".txt")
          # SR3 limit
          os.system("combineCards.py year16a=card_2016preVFP_"+channel+"_"+mass_signal+"_sronly"+"_sr3"+systTag+".txt year16b=card_2016postVFP_"+channel+"_"+mass_signal+"_sronly"+"_sr3"+systTag+".txt year17=card_2017_"+channel+"_"+mass_signal+"_sronly"+"_sr3"+systTag+".txt year18=card_2018_"+channel+"_"+mass_signal+"_sronly"+"_sr3"+systTag+".txt > card_Run2_"+channel+"_"+mass_signal+"_sronly"+"_sr3"+systTag+".txt")
          if args.Syst:
            NuisanceGrouping(os.path.abspath("card_Run2_"+channel+"_"+mass_signal+"_sronly_sr123"+systTag+".txt")) # Full
            NuisanceGrouping(os.path.abspath("card_Run2_"+channel+"_"+mass_signal+"_sronly"+"_sr3"+systTag+".txt")) # SR3

          if mass=="Weinberg" or int(mass.strip('M'))>100:
            # SR1, 2 limit
            os.system("combineCards.py year16a=card_2016preVFP_"+channel+"_"+mass_signal+"_sronly"+"_sr1"+systTag+".txt year16b=card_2016postVFP_"+channel+"_"+mass_signal+"_sronly"+"_sr1"+systTag+".txt year17=card_2017_"+channel+"_"+mass_signal+"_sronly"+"_sr1"+systTag+".txt year18=card_2018_"+channel+"_"+mass_signal+"_sronly"+"_sr1"+systTag+".txt > card_Run2_"+channel+"_"+mass_signal+"_sronly"+"_sr1"+systTag+".txt")
            os.system("combineCards.py year16a=card_2016preVFP_"+channel+"_"+mass_signal+"_sronly"+"_sr2"+systTag+".txt year16b=card_2016postVFP_"+channel+"_"+mass_signal+"_sronly"+"_sr2"+systTag+".txt year17=card_2017_"+channel+"_"+mass_signal+"_sronly"+"_sr2"+systTag+".txt year18=card_2018_"+channel+"_"+mass_signal+"_sronly"+"_sr2"+systTag+".txt > card_Run2_"+channel+"_"+mass_signal+"_sronly"+"_sr2"+systTag+".txt")
            if args.Syst:
              NuisanceGrouping(os.path.abspath("card_Run2_"+channel+"_"+mass_signal+"_sronly"+"_sr1"+systTag+".txt"))
              NuisanceGrouping(os.path.abspath("card_Run2_"+channel+"_"+mass_signal+"_sronly"+"_sr2"+systTag+".txt"))

    os.system('echo \'Done.\'')
    os.chdir(pwd)
