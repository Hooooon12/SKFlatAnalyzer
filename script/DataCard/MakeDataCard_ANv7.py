# Make Datacards
# Place this at CombineTool/CMSSW_10_2_13/src/<your working directory>
# You need to place card_skeletons already
# python MakeDataCard_ANv7.py --CR --Syst [--Decorr]; python MakeDataCard_ANv7.py --Combine CR --Syst [--Decorr] <-- add rateParam
# python MakeDataCard_ANv7.py --Combine Era --Syst [--Decorr]
# python MakeDataCard_ANv7.py --Syst [--Decorr]; python MakeDataCard_ANv7.py --Combine SR --Syst [--Decorr] <-- without rateParam ("sronly" setting)

import os, sys, argparse, re
import subprocess as cmd
from collections import OrderedDict
#print("CWD:", os.getcwd())
#print("__file__:", __file__)
#print("sys.path[0]:", sys.path[0])
#print("exceptions_auto.py exists?:", os.path.exists(os.path.join(os.path.dirname(__file__), "exceptions_auto.py"))) # It shows that this python file is ran at SKFlatAnalyzer/script
HERE = os.path.dirname(os.path.abspath(__file__))
if HERE not in sys.path:
  sys.path.insert(0, HERE)
from exceptions_auto import apply_auto_exceptions

parser = argparse.ArgumentParser(description='script for creating or merging data cards.',formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-sk', dest='skels', default=["card_skeleton_ANv7.txt"], nargs='+', help='List of skeletons to use')
parser.add_argument('-wp', dest='InputWPs', nargs='+', help='List of LimitInput working points')
parser.add_argument('-e', dest='eras', default=["2016preVFP","2016postVFP","2017","2018"], choices=["2016preVFP","2016postVFP","2017","2018"], nargs='+')
parser.add_argument('-c', dest='channels', default=["MuMu","EE","EMu"], choices=["MuMu","EE","EMu"], nargs='+') # store [] if nothing is fed
parser.add_argument('-m', dest='masses', nargs='+')
parser.add_argument('-s', dest='signals', default=["HNL","Weinberg"], choices=["","HNL","DY","VBF","DYVBF","SSWW","Weinberg"], nargs='+')
parser.add_argument('-o', dest='outputTag', default='', help='tag attached to the output directory')
parser.add_argument('--Ext', action='store_true', help='Extend cut based approach down to M500')
parser.add_argument('--CnC', action='store_true', help='One-bin limit')
parser.add_argument('--Decorr', action='store_true', help='Decorrelate fake, CF region by region')
parser.add_argument('--JetDecorr', action='store_true', help='Decorrelate jet scale/res additionally')
parser.add_argument('--CR', action='store_true', help='Make datacards named sr with HNL_SignalRegion_Plotter and sr_inv with HNL_ControlRegion_Plotter input. (Default : SR only)')
#parser.add_argument('--CR', nargs='*', help='Make datacards with manual CR inputs. (Default : SR only)') # Modify L108 with this line
parser.add_argument('--Syst', action='store_true', help='Add systematics into the datacards')
parser.add_argument('--Combine', choices=['CR','SR','Era'], help='CR --> Merge CR and SR datacards in one era,\nEra --> Merge pre-processed (CR+SR) over the Run2,\nSR --> Merge SR only datacards over the Run2')
parser.add_argument('--Type', choices=['CR', 'SR'], help="(Optional) If --Combine Era is used, specify whether to merge only CR or SR.")
args = parser.parse_args()

pwd = os.getcwd()

failure, result = cmd.getstatusoutput('combine --help')
if failure:
  print("[!!ERROR!!] cannot run combine.")
  print("Please set proper cmsenv first.")
  print("Exiting ...")
  sys.exit(1)

eras = args.eras
channels = args.channels
if not args.masses: # When you don't want to type all those masses!!
  args.masses = ["M85","M90","M95","M100","M125","M150","M200","M250","M300","M350","M400","M450","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000","M25000","M30000","M40000","M50000","M60000","Weinberg"]
else:
  for i in range(len(args.masses)):
    if "Weinberg" in args.masses[i]: continue
    else: args.masses[i] = "M"+args.masses[i]
masses = args.masses
signals = args.signals

SRpath = "/data9/Users/HNL_public/SUS-24-014/LimitInputs/"
CRpath = SRpath

#InputWPs = ["ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst"] # fix missed trigger SF syst @251223 <-- ANv6 legacy
#InputWPs = ["ANv6_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning"] # Test consistent binning (MuMu only, due to a bug) @260108
#InputWPs = ["ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning"] # Test consistent binning but fix the bug; pt-dependent Muon RECO SF @260110
#InputWPs = ["ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst"] # pt-dependent Muon RECO SF @260110
InputWPs = args.InputWPs

RegionDecorr_list = ["CMS_SUS24014_fake_stat","CMS_SUS24014_fake_highpt","CMS_SUS24014_fake_syst","CMS_SUS24014_cf_stat","CMS_SUS24014_cf_syst"]

ExtTag = '_Ext' if args.Ext else ''
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
  print("[!!ERROR!!] Non-syst mode currently not supported.")
  print("Exiting ...")
  sys.exit(1)
  #InputWPs = [WP+"_Decorr_JetDecorr" for WP in InputWPs] # FIXME use syst input as a default; can be changed later

OutputTag = "" if args.outputTag == '' else "_"+args.outputTag

#####################################################
#
# args.CR --> sr, sr_inv connected via rateParam
# else --> sr only, bkg norm uncert. treated by lnN
# args.syst --> postpone
#
#####################################################

if args.Combine is None:
  if not args.CR: OutputTag+="_NoCR" # SR only
else:
  if not (args.Combine == "CR" or (args.Combine == "Era" and args.CR)): OutputTag+="_NoCR" # Combine SR only
if not args.Syst: OutputTag+="_NoSyst"  # NoSyst

regions_cr = ["cr1_InvMET","cr2_InvMET","cr3_InvMET","cr1_InvBJet","cr2_InvBJet","cr3_InvBJet","wz_cr1","wz_cr2","wz_cr3","zg_cr","zz_cr"]
#regions_cr = args.CR # input from the user
regions_sr = ["sr1","sr2","sr3"]
regions_tot = regions_cr+regions_sr

proc_rateRegion_map = {}
proc_rateRegion_map['WZ'] = regions_sr + [cr for cr in regions_cr if "wz" in cr]
proc_rateRegion_map['WW'] = regions_sr + [cr for cr in regions_cr if "InvMET" in cr]
proc_rateRegion_map['ZG'] = regions_sr + [cr for cr in regions_cr if "zg" in cr]
proc_rateRegion_map['ZZ'] = regions_sr + [cr for cr in regions_cr if "zz" in cr]

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

##### Lumi uncertainty table #####
lumi_systs = { # https://twiki.cern.ch/twiki/bin/view/CMS/LumiRecommendationsRun2#Luminosity_for_pp_13_TeV_data_20
              '2016preVFP':
                            {
                             'corr1':'1.0118',
                             'corr2':'1.0004',
                             'corr3':'1.0035',
                            },
              '2016postVFP':
                            {
                             'corr1':'1.0118',
                             'corr2':'1.0004',
                             'corr3':'1.0035',
                            },
              '2017':
                           {
                            'corr2':'1.0055',
                            'corr3':'1.0061',
                           },
              '2018':
                           {
                            'corr3':'1.0084',
                           },
}

################################################################################################################################################

def Initialize_Process(WP):
    return OrderedDict([
        ('fake', '-1'),
        ('cf', '-1'),
        ('zg', '-1'),
        ('mc_others', '-1'), # conv_others + prompt_others
        ('wz', '-1'),
        ('wz_ewk', '-1'),
        ('zz', '-1'),
        ('ww', '-1'),
        ('signalDY', '-1'),
        ('signalVBF', '-1'),
        ('signalSSWW', '-1'),
        ('signalWeinberg', '-1'),
    ]) if "EMuCF" in WP or "Preapproval" in WP else\
    OrderedDict([
        ('fake', '-1'),
        ('cf', '-1'),
        ('zg', '-1'),
        #('conv_others', '-1'), # FIXME deprecated. only preservation purposes
        ('mc_others', '-1'), # conv_others + prompt_others
        ('wz', '-1'),
        ('zz', '-1'),
        ('ww', '-1'),
        #('prompt_others', '-1'), # FIXME deprecated. only preservation purposes
        #('signalDYVBF', '-1'),
        ('signalDY', '-1'),
        ('signalVBF', '-1'),
        ('signalSSWW', '-1'),
        ('signalWeinberg', '-1'),
    ])

def MakeRateString(region, era, channel, mass, signal, WP):
  this_process = Initialize_Process(WP)

  if "EMuCF" in WP or "Preapproval" in WP:
    if "MuMu" in channel:
      this_process['cf'] = '0'
  else:
    if "Mu" in channel:
      this_process['cf'] = '0'

  if mass.startswith("M") and mass[1:].isdigit():
    mass_int = int(mass[1:])
  else:
    mass_int = 999999 # Weinberg

  exceptionTag = WP+ExtTag
  this_process = apply_auto_exceptions(this_process, region, era, channel, mass, mass_int, exceptionTag)

  #print(region)
  #print(era)
  #print(channel)
  #print(exceptionTag)
  #print(this_process)

  is_Weinberg = (mass == "Weinberg")

  if not is_Weinberg:
    this_process['signalWeinberg'] = '0'

    if "DYVBF" in signal:
      this_process['signalSSWW'] = '0'
    elif "DY" in signal:
      this_process['signalVBF'] = '0'
      this_process['signalSSWW'] = '0'
    elif "VBF" in signal:
      this_process['signalDY'] = '0'
      this_process['signalSSWW'] = '0'
    elif "SSWW" in signal:
      #this_process['signalDYVBF'] = '0'
      this_process['signalDY'] = '0'
      this_process['signalVBF'] = '0'

    if mass_int < 300:
      this_process['signalVBF'] = '0'
      this_process['signalSSWW'] = '0'
    elif mass_int <= 500:
      if args.Ext and mass_int == 500:
        this_process['signalSSWW'] = '-1'
      else:
        this_process['signalSSWW'] = '0'
    elif 500 < mass_int <= 3000:
      pass
    else:  # mass_value > 3000
      #this_process['signalDYVBF'] = '0'
      this_process['signalDY'] = '0'
      this_process['signalVBF'] = '0'
  else:
    #this_process['signalDYVBF'] = '0'
    this_process['signalDY'] = '0'
    this_process['signalVBF'] = '0'
    this_process['signalSSWW'] = '0'

  #FIXME current setting: No signal in CR
  if region in regions_cr:
    #this_process['signalDYVBF'] = '0'
    this_process['signalDY'] = '0'
    this_process['signalVBF'] = '0'
    this_process['signalSSWW'] = '0'
    this_process['signalWeinberg'] = '0'

  #print(this_process)

  formatted_values = [
      value.ljust(13)
      for key, value in list(this_process.items())
  ]

  this_string = "rate                          "+" ".join(formatted_values)+"\n"
  return this_string

def is_syst_line(line):
  return (line.startswith("lumi") or line.startswith("mc_") or line.startswith("CMS_") or line.startswith("QCDscale_") or line.startswith("RenScale_") or line.startswith("FacScale_") or line.startswith("pdf_"))

def is_rateParam_line(line):
  return "rateParam" in line

def CardSetting(isCR, WP, skeleton, era, channel, mass, signal):

  with open(skeleton,'r') as f: # open skeleton
    lines = f.readlines()

  new_lines_common = []

  lines_cr = {}
  lines_sr = {}
  lines_sronly = {}

  for region in regions_tot:
    this_lines = lines[:]
    new_lines = []

    # preprocess lines
    this_lines[4] = "shapes * *  "+CRpath+WP+"/"+era+"/"+region+"/"+mass+"_"+channel+ExtTag+"_card_input.root $PROCESS $PROCESS_$SYSTEMATIC\n" if region in regions_cr else "shapes * *  "+SRpath+WP+"/"+era+"/"+region+"/"+mass+"_"+channel+ExtTag+"_card_input.root $PROCESS $PROCESS_$SYSTEMATIC\n"
    this_lines[17] = MakeRateString(region, era, channel, mass, signal, WP)
    for i in range(len(this_lines)):
      this_lines[i] = this_lines[i].replace('bin1',region)
    for i in range(18):
      new_lines.append(this_lines[i])

    ### handle each syst
    # Define era-correlated syst keys
    corr_keys = ["xsec", "pileup", "QCDscale", "RenScale", "FacScale", "pdf", "_corr", "scale_m", "res_m", "eff_m_reco_syst", "eff_m_id_syst", "eff_m_trigger_syst", "scale_e", "res_e", "eff_e_reco_syst", "eff_e_id_syst", "eff_e_trigger_syst", "ParticleNet"]
    if "PNETdecorr" in args.outputTag: corr_keys = [k for k in corr_keys if "ParticleNet" not in k]
    if "FullJES" in WP: corr_keys+=[
                                    "AbsoluteMPFBias",
                                    "AbsoluteScale",  
                                    "FlavorQCD",      
                                    "Fragmentation",  
                                    "PileUpDataMC",   
                                    "PileUpPtBB",     
                                    "PileUpPtEC1",    
                                    "PileUpPtEC2",    
                                    "PileUpPtHF",     
                                    "PileUpPtRef",    
                                    "RelativeBal",    
                                    "RelativeFSR",    
                                    "RelativePtBB",   
                                    "RelativePtHF",   
                                    "SinglePionECAL", 
                                    "SinglePionHCAL", 
                                   ]
    for line in this_lines[18:]:
      if line.startswith("#"): continue # skip the commented lines

      if args.Syst:
        # full JES treatment
        if "FullJES" in WP:
          if line.split()[0].endswith("CMS_scale_j"): continue
        else:
          if "CMS_scale_j_" in line: continue

        # channel-dependent fake syst
        if "CMS_SUS24014_fake_syst" in line:
          if channel=="EMu":
            line = line.replace('1.2','1.25')
          elif channel=="EE":
            line = line.replace('1.2','1.3')

        if is_syst_line(line):
          syst_name = line.split()[0]

          if any(f"{key}_" in f"{syst_name}_" for key in corr_keys):
            pass
          # partial correlation (lumi)
          elif 'lumi' in syst_name:
            if '161718' in syst_name:
              line = line.replace('1.05',lumi_systs[era]['corr3'])
            elif '1617' in syst_name:
              if '2018' in era: continue # don't save this line
              else: line = line.replace('1.05',lumi_systs[era]['corr2'])
            else: # 16 only
              if '2018' in era or '2017' in era: continue # don't save this line
              else: line = line.replace('1.05',lumi_systs[era]['corr1'])
          # era-decorrelation
          else:
            line = line.replace(syst_name, f"{syst_name}_{era}")

          # channel treatment
          if channel=="MuMu":
            if ("eff_e_" in syst_name) or ("scale_e" in syst_name) or ("res_e" in syst_name) or ("fake_highpt" in syst_name): continue
          elif channel=="EE":
            if ("eff_m_" in syst_name) or (syst_name == "CMS_scale_m") or ("res_m" in syst_name): continue

          # sr treatment
          if "sr1" not in region and "cr1" not in region:
            if "ParticleNet" in syst_name: continue
 
          # HEM treatment
          if era!='2018':
            if "HEM" in syst_name: continue

      if is_rateParam_line(line):
        this_region = line.split()[0].split('_')[-1] if len(line.split()[0].split('_')) == 2 else None # "srx" if it's not ZGNorm or ZZNorm which is shared across all srs
        sr_filtered = mass_to_srs(mass)
        if this_region and ((this_region not in sr_filtered) or (this_region[1:] not in region)): # filter out "srx" not allowed by sr_filtered and not matched to the region
          continue
        this_proc = line.split('Norm')[0]
        if region not in proc_rateRegion_map[this_proc]:
          continue
        line = line.replace("Norm",f"Norm_{era}").replace("REGION",region)

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

    ####### finally adjust columns using ljust ######## # TODO?

    if region in regions_cr:
      lines_cr[region] = new_lines[:]
    elif region in regions_sr:
      lines_sr[region] = new_lines[:]
      # no rateParam setting
      for i in range(18,len(new_lines)):
        if is_rateParam_line(new_lines[i]):
          new_lines[i] = ""
      lines_sronly[region] = new_lines

  if isCR:
    return (lines_sr, lines_cr)
  else:
    return lines_sronly
 
def ValidMassSignal(channel: str, mass: str, signal: str) -> bool:
  # Check M500 limit extension
  if args.Ext:
    if mass!="M500":
      #print(mass,"is not allowed to run with Ext option.")
      return False

  # Check Weinberg first
  if mass=="Weinberg":
    if "Weinberg" in signal: return True
    else: return False

  # Now N mass
  else:
    if "Weinberg" in signal: return False

    # Check channel dependent mass
    if channel != "EMu" and int(mass.strip('M')) >= 40000:
      return False

    if signal=="HNL" or signal=="": return True # MakeRateString will handle this

    if int(mass.strip('M'))<300:
      if "DY" in signal:
        return True
      else:
        return False
    elif int(mass.strip('M'))<=500:
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

def mass_to_srs(mass_str):
  if mass_str == "Weinberg":
    return ["sr2", "sr3"] #NOTE 0.25 (sr1) vs 20 (others)
  m = int(mass_str.strip("M"))
  if m <= 100:
    return ["sr3"]
  elif m > 3000:
    return ["sr2", "sr3"] #NOTE 0.0001 (sr1) vs 0.007 (sr2) for M30000 EE
  else:
    return ["sr1", "sr2", "sr3"]

def filter_crs(regions_cr_all, valid_srs):

  all_srs = ["sr1", "sr2", "sr3"]
  invalid_srs = [sr for sr in all_srs if sr not in valid_srs]
  invalid_srs_crs = []
  for invalid_sr in invalid_srs:
    this_idx = invalid_sr[-1]
    invalid_srs_crs.append(invalid_sr)
    invalid_srs_crs.append("cr"+str(this_idx))

  return [cr for cr in regions_cr_all if not any(tok in cr for tok in invalid_srs_crs)]

def make_sr_cardname(era, channel, mass_signal, sr_list, tag, sronly):
  base = f"card_{era}_{channel}{ExtTag}_{mass_signal}"
  if sronly:
    return [f"{sr}={base}_sronly_{sr}{tag}.txt" for sr in sr_list]
  else:
    return [f"{sr}={base}_{sr}{tag}.txt" for sr in sr_list]

def make_cr_cardname(era, channel, mass_signal, cr_list, tag=""):
    base = f"card_{era}_{channel}{ExtTag}_{mass_signal}"
    return [f"{cr}={base}_{cr}{tag}.txt" for cr in cr_list]

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
                'pdf'         : [],
                'scale'       : [],
                'fake'        : [],
                'cf'          : [],
                'jet_uncert'  : [],
                'lep_uncert'  : [],
                'btag_sf'     : [],
                'met_energy'  : [],
                'prefire'     : [],
                'pileup'      : [],
                'HEM'         : [],
  }
  for line in lines[:]:
    line = line.split(' ')[0]
    if "lumi" in line:
      group_nuis["lumi"].append(line)
    elif "xsec" in line:
      group_nuis["xsec"].append(line)
    elif "pdf" in line:
      group_nuis["pdf"].append(line)
    elif "QCDscale" in line or "RenScale" in line or "FacScale" in line:
      group_nuis["scale"].append(line)
    elif "CMS_SUS24014_fake" in line:
      group_nuis["fake"].append(line)
    elif "CMS_SUS24014_cf" in line:
      group_nuis["cf"].append(line)
    elif "CMS_res_j" in line or "CMS_scale_j" in line or "ParticleNet" in line or "PUJetID" in line:
      if "FullJES" in this_card:
        if re.search(r'CMS_scale_j_\d',line): continue # pass combined JES
        group_nuis["jet_uncert"].append(line)
      else: # use combined JES
        if re.search('CMS_scale_j_[A-Za-z]',line): continue # pass combined JES
        group_nuis["jet_uncert"].append(line)
    elif ("CMS_res_m" in line or "CMS_scale_m" in line or "eff_m" in line or "CMS_res_e" in line or "CMS_scale_e" in line or "eff_e" in line) and "CMS_scale_met" not in line:
      group_nuis["lep_uncert"].append(line)
    elif "CMS_btag" in line:
      group_nuis["btag_sf"].append(line)
    elif "CMS_scale_met" in line:
      group_nuis["met_energy"].append(line)
    elif "CMS_l1_prefiring" in line:
      group_nuis["prefire"].append(line)
    elif "CMS_pileup" in line:
      group_nuis["pileup"].append(line)
    elif "HEM" in line:
      group_nuis["HEM"].append(line)

  with open(this_card,'a') as f:
    for key, value in list(group_nuis.items()):
      if len(value)!=0:
        f.write(key+" group = "+" ".join(value)+'\n')

  return

def combineCards(out_path, parts):
  os.system("combineCards.py " + " ".join(parts) + f" > {out_path}")
  if getattr(args,"Syst"):
    NuisanceGrouping(os.path.abspath(out_path)) #FIXME comment out this for the faster run

def era_tag_map():
    return {
        "2016preVFP": "year16a",
        "2016postVFP": "year16b",
        "2017": "year17",
        "2018": "year18",
    }

def combine_run2(out_path, card_per_era):
  tag_map = era_tag_map()
  assigns = []
  for card in card_per_era:
    era = card.split("_")[1]
    assigns.append(f"{tag_map[era]}={card}")
  os.system("combineCards.py " + " ".join(assigns) + f" > {out_path}")
  if getattr(args,"Syst"):
    NuisanceGrouping(os.path.abspath(out_path)) #FIXME comment out this for the faster run

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

  for skel in args.skels:
    SkelTag = skel.removesuffix('.txt').split('ANv7')[-1] #FIXME card_skeleton_ANv7_NoPUJetID_NoPNET.txt --> _NoPUJetID_NoPNET. AN version can be changed.
    OutputWP = InputWP+OutputTag+SkelTag

    if not args.Combine:
      os.system("mkdir -p "+OutputWP)
      os.system("ln -s /data6/Users/jihkim/SKFlatAnalyzer/script/DataCard/MakeWorkspace.py "+OutputWP)
      os.system("ln -s /data6/Users/jihkim/SKFlatAnalyzer/script/DataCard/CheckNuisance.py "+OutputWP)

      for era, channel, mass, signal in [(era, channel, mass, signal) for era in eras for channel in channels for mass in masses for signal in signals]:
        if not ValidMassSignal(channel, mass, signal): continue

        mass_signal = mass if (signal == mass or signal == "") else mass+"_"+signal # Remove duplication like Weinberg_Weinberg

        this_card = CardSetting(args.CR, InputWP, skel, era, channel, mass, signal)
        if args.CR:
          for region in list(this_card[0].keys()):
            with open(OutputWP+"/card_"+era+"_"+channel+ExtTag+"_"+mass_signal+"_"+region+systTag+".txt",'w') as f:
              for line in this_card[0][region]:
                f.write(line)
          for region in list(this_card[1].keys()):
            with open(OutputWP+"/card_"+era+"_"+channel+ExtTag+"_"+mass_signal+"_"+region+".txt",'w') as f:
              for line in this_card[1][region]:
                f.write(line)
        else:
          for region in list(this_card.keys()):
            with open(OutputWP+"/card_"+era+"_"+channel+ExtTag+"_"+mass_signal+"_sronly_"+region+systTag+".txt",'w') as f:
              for line in this_card[region]:
                f.write(line)

    else:
      os.chdir(OutputWP)
      os.system('echo \'Currently combining cards at...\'')
      os.system('pwd')
      if args.Syst:
        os.system('echo \'Systematics have been added.\'')
      for channel, mass, signal in [(channel, mass, signal) for channel in channels for mass in masses for signal in signals]:
        if not ValidMassSignal(channel, mass, signal): continue

        mass_signal = mass if (signal == mass or signal == "") else mass+"_"+signal # Remove duplication like Weinberg_Weinberg
        sr_filtered = mass_to_srs(mass)
        cr_filtered = filter_crs(regions_cr, sr_filtered)

        for era in eras:
          if args.Combine == "CR":
            sr_combine = make_sr_cardname(era, channel, mass_signal, sr_filtered, tag=systTag, sronly=False)
            cr_combine = make_cr_cardname(era, channel, mass_signal, cr_filtered)
            full_combine = f"card_{era}_{channel}{ExtTag}_{mass_signal}{systTag}.txt"
            combineCards(full_combine, sr_combine+cr_combine)

            for sr in sr_filtered:
              cr_for_sr = filter_crs(regions_cr, [sr])
              sr_each = make_sr_cardname(era, channel, mass_signal, [sr], tag=systTag, sronly=False)
              cr_each = make_cr_cardname(era, channel, mass_signal, cr_for_sr)
              combine_each = f"card_{era}_{channel}{ExtTag}_{mass_signal}_{sr}{systTag}_Combined.txt"
              combineCards(combine_each, sr_each+cr_each)

          elif args.Combine == "SR":
            sr_combine = make_sr_cardname(era, channel, mass_signal, sr_filtered, tag=systTag, sronly=True)
            full_combine = f"card_{era}_{channel}{ExtTag}_{mass_signal}_sronly_sr123{systTag}.txt"
            combineCards(full_combine, sr_combine)

        if args.Combine == "Era":
          if args.CR: # with CR
            per_era_full = [f"card_{era}_{channel}{ExtTag}_{mass_signal}{systTag}.txt" for era in eras]
            run2_full = f"card_Run2_{channel}{ExtTag}_{mass_signal}{systTag}.txt"
            combine_run2(run2_full, per_era_full)

            for sr in sr_filtered:
              per_era_each = [f"card_{era}_{channel}{ExtTag}_{mass_signal}_{sr}{systTag}_Combined.txt" for era in eras]
              run2_each = f"card_Run2_{channel}{ExtTag}_{mass_signal}_{sr}{systTag}_Combined.txt"
              combine_run2(run2_each, per_era_each)

          else:
            per_era_full = [f"card_{era}_{channel}{ExtTag}_{mass_signal}_sronly_sr123{systTag}.txt" for era in eras]
            run2_full = f"card_Run2_{channel}{ExtTag}_{mass_signal}_sronly_sr123{systTag}.txt"
            combine_run2(run2_full, per_era_full)

            for sr in sr_filtered:
              per_era_each = [f"card_{e}_{channel}{ExtTag}_{mass_signal}_sronly_{sr}{systTag}.txt" for era in eras]
              run2_each = f"card_Run2_{channel}{ExtTag}_{mass_signal}_sronly_{sr}{systTag}.txt"
              combine_run2(run2_each, per_era_each)

      os.system('echo \'Done.\'')
      os.chdir(pwd)
