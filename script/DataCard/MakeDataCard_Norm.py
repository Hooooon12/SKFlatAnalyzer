# Make Datacards
# Place this at CombineTool/CMSSW_10_2_13/src/<your working directory>
# You need to place card_skeletons already
# python MakeDataCard_Norm.py --CR --Syst; python MakeDataCard_Norm.py --Combine CR --Syst <-- add rateParam
# python MakeDataCard_Norm.py --Combine Era --Syst
# python MakeDataCard_Norm.py --Syst; python MakeDataCard_Norm.py --Combine SR --Syst <-- without rateParam ("sronly" setting)


import os, sys, argparse

parser = argparse.ArgumentParser(description='script for creating or merging data cards.',formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('--Decorr', action='store_true', help='Decorrelate fake, CF, norm')
parser.add_argument('--CR', action='store_true', help='Make datacards named sr with HNL_SignalRegion_Plotter and sr_inv with HNL_ControlRegion_Plotter input. (Default : SR only)')
parser.add_argument('--Syst', action='store_true', help='Add systematics into the datacards')
parser.add_argument('--Combine', choices=['CR','SR','Era'], help='CR --> Merge CR and SR datacards in one era,\nEra --> Merge pre-processed (CR+SR) over the Run2,\nSR --> Merge SR only datacards over the Run2')
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
#eras = ["2016preVFP","2016postVFP","2018"]
channels = ["MuMu","EE","EMu"]
#channels = ["MuMu","EE"]
#channels = ["MuMu"]
#channels = ["EE"]
#channels = ["EE","EMu"]
#masses = ["M90","M100","M150","M200","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000"]
#masses = ["M100","M1000","M10000"]
#masses = ["M500","M1000","M5000"]
#masses = ["M3000"]
#masses = ["M90","M100","M150","M200","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000"]
masses = ["M85","M90","M95","M100","M125","M150","M200","M250","M300","M400","M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700","M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000"]
#masses = ["M100","M1000","M10000"]
#masses = ["M3000","M5000","M7500","M10000","M15000","M20000"]
#masses = ["M100"]
#masses = ["M20000"]

#SRpath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_PR43/LimitInputs/"
#SRpath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter/LimitInputs/"
#CRpath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter/LimitInputs/"
#SRpath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter/LimitExtraction/"
#CRpath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter/LimitExtraction/"
SRpath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_PR52/LimitExtraction/"
CRpath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter_PR52/LimitExtraction/"
#SRpath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter/LimitExtraction/"
#CRpath = "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter/LimitExtraction/"

#InputWPs = ["240422_HNL_ULID"]
#InputWPs = ["240501_1704_HNL_ULID","240501_1704_HNTightV2"]
#InputWPs = ["240504_PR44_HNL_ULID"]
#InputWPs = ["240505_PR45_HNL_ULID"]
#InputWPs = ["240505_PR46_HNL_ULID","240505_PR46_HNTightV2"]
#InputWPs = ["rateParam_HNL_ULID_PR46"]
#InputWPs = ["PR48_rateParam_HNL_ULID"]
#InputWPs = ["PR51_HNL_ULID"]
#InputWPs = ["PR51_rescale_HNL_ULID"]
#InputWPs = ["PR52_HNL_ULID"]
InputWPs = ["PR52_SSWWrescale_HNL_ULID"]
#InputWPs = ["PR52_CnC_HNL_ULID"]
#InputWPs = ["PR52_NewCnC_HNL_ULID"]
#InputWPs = ["PR52_10TeVrescale_HNL_ULID"]
#InputWPs = ["PR55_HNL_ULID","PR55_HighPt"]
#InputWPs = ["PR55_HNL_ULID"]
#InputWPs = ["PR55_NoMinPt_HighPt"]
#OutputTag = "_NOsr2inv"
#OutputTag = "_NOsr2inv_NOzgcr1"
#OutputTag = "_NOsr2inv_NOcr1Norm_FixCF"
#OutputTag = "_NOsr2inv_IncNorm"
#OutputTag = "_AltStat"
#OutputTag = "_FakeCF0p1"
#OutputTag = "_FakeCF0p1_toy"
#OutputTag = "_SSWWonly"
#OutputTag = "_DYVBFonly"
#OutputTag = "_defMod_SSWWonly"
#OutputTag = "_Fake0p3CF0p2_PR991"
#OutputTag = "_Fake0p3CF0p2_PR991_TestModel"
OutputTag = "_AN"
#OutputTag = "_SUScomment"
#OutputTag = "_Singluarity"
#OutputTag = ""
if args.Decorr: OutputTag+="_Decorr"

################################################################################################################################################

def CardSetting(isCR, WP, era, channel, mass):

  with open("card_skeleton_Norm.txt",'r') as f: # your workspace
    lines = f.readlines()

  #regions_cr = ["sr1_inv","sr2_inv","sr3_inv","cf_cr","ww_cr","wz_cr1","wz_cr2","wz_cr3","zg_cr1","zg_cr3","zz_cr1","zz_cr3"]
  #regions_cr = ["sr1_inv","sr3_inv","cf_cr","ww_cr","wz_cr1","wz_cr2","wz_cr3","zg_cr1","zg_cr3","zz_cr1","zz_cr3"]
  #regions_cr = ["sr1_inv","sr3_inv","cf_cr","ww_cr","wz_cr1","wz_cr2","wz_cr3","zg_cr3","zz_cr1","zz_cr3"]
  #regions_cr = ["sr1_inv","sr3_inv","cf_cr","ww_cr","wz_cr1","wz_cr2","wz_cr3","zg_cr3","zz_cr3"]
  #regions_cr = ["sr1_inv","sr3_inv","cf_cr","ww_cr","wz_cr","zg_cr","zz_cr"]
  regions_cr = ["sr_inv","sr1_inv","sr2_inv","sr3_inv","cf_cr","ww_cr","wz_cr","zg_cr","zz_cr"]
  lines_cr = {}
  # cr norm setting
  for region in regions_cr:
    this_lines_cr = lines[:]
    this_lines_cr[4] = "shapes * *  "+CRpath+WP+"/"+era+"/"+region+"/"+mass+"_"+channel+"_card_input.root $PROCESS $PROCESS_$SYSTEMATIC\n"
    if (region == "cf_cr" and "Mu" in channel): continue
    if "Mu" in channel: this_lines_cr[17] = "rate                       -1     0      -1     -1            -1     -1     -1     -1             0            0\n"  # no cf, signal
    else: this_lines_cr[17] = "rate                       -1     -1     -1     -1            -1     -1     -1     -1             0            0\n"  # no signal
    for i in range(len(this_lines_cr)):
      this_lines_cr[i] = this_lines_cr[i].replace('bin1',region)
    for i in range(22,40):
      this_lines_cr[i] = "" # remove unnecessary syst sources.

    # handle norm constraints
    #for i in range(41,45):
    for i in range(41,61):
      if "Norm" in this_lines_cr[i]:
        this_lines_cr[i] = this_lines_cr[i].replace('Norm','Norm'+era) # era dependent norm constraint
      #if (not region in this_lines_cr[i]):
      #  this_lines_cr[i] = "" # remove not corresponding rateParams

    # finally do the decorrelation
    if args.Decorr:
      if "sr_" in region: pass
      elif "sr" in region:
        this_lines_cr[19] = this_lines_cr[19].replace('Fake','Fake'+region.split('_')[0])
        this_lines_cr[21] = this_lines_cr[21].replace('CF','CF'+region.split('_')[0])
      else:
        this_lines_cr[19] = this_lines_cr[19].replace('Fake','Fakesr1')+this_lines_cr[19].replace('Fake','Fakesr2')+this_lines_cr[19].replace('Fake','Fakesr3')
        this_lines_cr[21] = this_lines_cr[21].replace('CF','CFsr1')+this_lines_cr[21].replace('CF','CFsr2')+this_lines_cr[21].replace('CF','CFsr3')

    lines_cr[region] = this_lines_cr

  # sr setting
  regions_sr = ["sr","sr1","sr2","sr3"]
  lines_sr = {}
  lines_sronly = {}

  for region in regions_sr:
    this_lines_sr = lines[:]
    this_lines_sr[4] = "shapes * *  "+SRpath+WP+"/"+era+"/"+region+"/"+mass+"_"+channel+"_card_input.root $PROCESS $PROCESS_$SYSTEMATIC\n"
    if int(mass.replace("M","")) < 500:
      if "Mu" in channel: this_lines_sr[17] = "rate                       -1     0      -1     -1            -1     -1     -1     -1             -1           0\n"  # no cf, SSWW
      else: this_lines_sr[17] = "rate                       -1     -1     -1     -1            -1     -1     -1     -1             -1           0\n"  # no SSWW
    if 500 <= int(mass.replace("M","")) and int(mass.replace("M","")) <= 3000:
      if "Mu" in channel: this_lines_sr[17] = "rate                       -1     0      -1     -1            -1     -1     -1     -1             -1           -1\n" # no cf
      else: this_lines_sr[17] = "rate                       -1     -1     -1     -1            -1     -1     -1     -1             -1           -1\n"
    elif 3000 < int(mass.replace("M","")):
      if "Mu" in channel: this_lines_sr[17] = "rate                       -1     0      -1     -1            -1     -1     -1     -1             0            -1\n" # no cf, DYVBF
      else: this_lines_sr[17] = "rate                       -1     -1     -1     -1            -1     -1     -1     -1             0            -1\n" # no DYVBF
    #if "Mu" in channel: this_lines_sr[17] = "rate                       -1     0      -1     -1            -1     -1     -1     -1             -1           0\n"  # no cf, SSWW
    #else: this_lines_sr[17] = "rate                       -1     -1     -1     -1            -1     -1     -1     -1             -1           0\n"  # no SSWW
    #if "Mu" in channel: this_lines_sr[17] = "rate                       -1     0      -1     -1            -1     -1     -1     -1             0            -1\n" # no cf, DYVBF
    #else: this_lines_sr[17] = "rate                       -1     -1     -1     -1            -1     -1     -1     -1             0            -1\n" # no DYVBF
    for i in range(len(this_lines_sr)):
      this_lines_sr[i] = this_lines_sr[i].replace('bin1',region)
    if not args.Syst:
      for i in range(22,40):
        this_lines_sr[i] = "" # remove unnecessary syst sources.

    # handle norm constraints
    #for i in range(41,45):
    for i in range(41,61):
      if "Norm" in this_lines_sr[i]:
        this_lines_sr[i] = this_lines_sr[i].replace('Norm','Norm'+era) # era dependent norm constraint
      #if (not region in this_lines_sr[i]):
      #  this_lines_sr[i] = "" # remove not corresponding rateParams

    # finally do the decorrelation
    if args.Decorr:
      if region=="sr": pass
      else:
        this_lines_sr[19] = this_lines_sr[19].replace('Fake','Fake'+region.split('_')[0])
        this_lines_sr[21] = this_lines_sr[21].replace('CF','CF'+region.split('_')[0])

    lines_sr[region] = this_lines_sr

    this_lines_sronly = this_lines_sr[:]
    for i in range(41,61):
      this_lines_sronly[i] = "" # remove all rateParams
    # finally do the decorrelation
    if args.Decorr:
      if region=="sr": pass
      else:
        this_lines_sronly[19] = this_lines_sronly[19].replace('Fake','Fake'+region.split('_')[0])
        this_lines_sronly[21] = this_lines_sronly[21].replace('CF','CF'+region.split('_')[0])
    lines_sronly[region] = this_lines_sronly

  if isCR:
    return (lines_sr, lines_cr)
  else:
    return lines_sronly
  
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

    for era, channel, mass in [(era, channel, mass) for era in eras for channel in channels for mass in masses]:
      this_card = CardSetting(args.CR, InputWP, era, channel, mass)
      if args.CR:
        for region in this_card[0].keys():
          with open(OutputWP+"/card_"+era+"_"+channel+"_"+mass+"_"+region+systTag+".txt",'w') as f:
            for line in this_card[0][region]:
              f.write(line)
        for region in this_card[1].keys():
          with open(OutputWP+"/card_"+era+"_"+channel+"_"+mass+"_"+region+".txt",'w') as f:
            for line in this_card[1][region]:
              f.write(line)
      else:
        for region in this_card.keys():
          with open(OutputWP+"/card_"+era+"_"+channel+"_"+mass+"_sronly_"+region+systTag+".txt",'w') as f:
            for line in this_card[region]:
              f.write(line)

  else:
    os.chdir(OutputWP)
    os.system('echo \'Currently combining cards at...\'')
    os.system('pwd')
    if args.Syst:
      os.system('echo \'Systematics have been added.\'')
    for channel, mass in [(channel, mass) for channel in channels for mass in masses]:
      for era in eras:
        if "Mu" in channel: Add_cf_cr = ""
        else: Add_cf_cr = "cf_cr=card_"+era+"_"+channel+"_"+mass+"_cf_cr.txt"

        if args.Combine == "CR":
          if mass == "M100": # no signalDYVBF in SR1, SR2 with M100 for now FIXME later
            os.system("combineCards.py \
                                       sr3=card_"+era+"_"+channel+"_"+mass+"_sr3"+systTag+".txt \
                                       sr3_inv=card_"+era+"_"+channel+"_"+mass+"_sr3_inv.txt "\
                                       #sr=card_"+era+"_"+channel+"_"+mass+"_sr"+systTag+".txt \
                                       #sr_inv=card_"+era+"_"+channel+"_"+mass+"_sr_inv.txt "\
                                       +Add_cf_cr+" \
                                       ww_cr=card_"+era+"_"+channel+"_"+mass+"_ww_cr.txt \
                                       zg_cr=card_"+era+"_"+channel+"_"+mass+"_zg_cr.txt \
                                       wz_cr=card_"+era+"_"+channel+"_"+mass+"_wz_cr.txt \
                                       zz_cr=card_"+era+"_"+channel+"_"+mass+"_zz_cr.txt \
                                       > card_"+era+"_"+channel+"_"+mass+systTag+".txt")
          else:
            os.system("combineCards.py \
                                       sr1=card_"+era+"_"+channel+"_"+mass+"_sr1"+systTag+".txt \
                                       sr2=card_"+era+"_"+channel+"_"+mass+"_sr2"+systTag+".txt \
                                       sr3=card_"+era+"_"+channel+"_"+mass+"_sr3"+systTag+".txt \
                                       sr1_inv=card_"+era+"_"+channel+"_"+mass+"_sr1_inv.txt \
                                       sr2_inv=card_"+era+"_"+channel+"_"+mass+"_sr2_inv.txt \
                                       sr3_inv=card_"+era+"_"+channel+"_"+mass+"_sr3_inv.txt "\
                                       #sr=card_"+era+"_"+channel+"_"+mass+"_sr"+systTag+".txt \
                                       #sr_inv=card_"+era+"_"+channel+"_"+mass+"_sr_inv.txt "\
                                       +Add_cf_cr+" \
                                       ww_cr=card_"+era+"_"+channel+"_"+mass+"_ww_cr.txt \
                                       zg_cr=card_"+era+"_"+channel+"_"+mass+"_zg_cr.txt \
                                       wz_cr=card_"+era+"_"+channel+"_"+mass+"_wz_cr.txt \
                                       zz_cr=card_"+era+"_"+channel+"_"+mass+"_zz_cr.txt \
                                       > card_"+era+"_"+channel+"_"+mass+systTag+".txt")
          os.system("combineCards.py \
                                     sr1=card_"+era+"_"+channel+"_"+mass+"_sr1"+systTag+".txt \
                                     sr1_inv=card_"+era+"_"+channel+"_"+mass+"_sr1_inv.txt "\
                                     +Add_cf_cr+" \
                                     ww_cr=card_"+era+"_"+channel+"_"+mass+"_ww_cr.txt \
                                     zg_cr=card_"+era+"_"+channel+"_"+mass+"_zg_cr.txt \
                                     wz_cr=card_"+era+"_"+channel+"_"+mass+"_wz_cr.txt \
                                     zz_cr=card_"+era+"_"+channel+"_"+mass+"_zz_cr.txt \
                                     > card_"+era+"_"+channel+"_"+mass+"_sr1"+systTag+"_Combined.txt")
          os.system("combineCards.py \
                                     sr2=card_"+era+"_"+channel+"_"+mass+"_sr2"+systTag+".txt \
                                     sr2_inv=card_"+era+"_"+channel+"_"+mass+"_sr2_inv.txt "\
                                     +Add_cf_cr+" \
                                     ww_cr=card_"+era+"_"+channel+"_"+mass+"_ww_cr.txt \
                                     zg_cr=card_"+era+"_"+channel+"_"+mass+"_zg_cr.txt \
                                     wz_cr=card_"+era+"_"+channel+"_"+mass+"_wz_cr.txt \
                                     zz_cr=card_"+era+"_"+channel+"_"+mass+"_zz_cr.txt \
                                     > card_"+era+"_"+channel+"_"+mass+"_sr2"+systTag+"_Combined.txt")
          os.system("combineCards.py \
                                     sr3=card_"+era+"_"+channel+"_"+mass+"_sr3"+systTag+".txt \
                                     sr3_inv=card_"+era+"_"+channel+"_"+mass+"_sr3_inv.txt "\
                                     +Add_cf_cr+" \
                                     ww_cr=card_"+era+"_"+channel+"_"+mass+"_ww_cr.txt \
                                     zg_cr=card_"+era+"_"+channel+"_"+mass+"_zg_cr.txt \
                                     wz_cr=card_"+era+"_"+channel+"_"+mass+"_wz_cr.txt \
                                     zz_cr=card_"+era+"_"+channel+"_"+mass+"_zz_cr.txt \
                                     > card_"+era+"_"+channel+"_"+mass+"_sr3"+systTag+"_Combined.txt")
          os.system("combineCards.py \
                                     sr=card_"+era+"_"+channel+"_"+mass+"_sr"+systTag+".txt \
                                     sr_inv=card_"+era+"_"+channel+"_"+mass+"_sr_inv.txt "\
                                     +Add_cf_cr+" \
                                     ww_cr=card_"+era+"_"+channel+"_"+mass+"_ww_cr.txt \
                                     zg_cr=card_"+era+"_"+channel+"_"+mass+"_zg_cr.txt \
                                     wz_cr=card_"+era+"_"+channel+"_"+mass+"_wz_cr.txt \
                                     zz_cr=card_"+era+"_"+channel+"_"+mass+"_zz_cr.txt \
                                     > card_"+era+"_"+channel+"_"+mass+"_sr"+systTag+"_Combined.txt")
          os.system("combineCards.py \
                                     sr=card_"+era+"_"+channel+"_"+mass+"_sr"+systTag+".txt \
                                     sr_inv=card_"+era+"_"+channel+"_"+mass+"_sr_inv.txt \
                                     ww_cr=card_"+era+"_"+channel+"_"+mass+"_ww_cr.txt \
                                     zg_cr=card_"+era+"_"+channel+"_"+mass+"_zg_cr.txt \
                                     wz_cr=card_"+era+"_"+channel+"_"+mass+"_wz_cr.txt \
                                     zz_cr=card_"+era+"_"+channel+"_"+mass+"_zz_cr.txt \
                                     > card_"+era+"_"+channel+"_"+mass+"_sr"+systTag+"_NoCFCR_Combined.txt")
          os.system("combineCards.py \
                                     sr=card_"+era+"_"+channel+"_"+mass+"_sr"+systTag+".txt "\
                                     +Add_cf_cr+" \
                                     ww_cr=card_"+era+"_"+channel+"_"+mass+"_ww_cr.txt \
                                     zg_cr=card_"+era+"_"+channel+"_"+mass+"_zg_cr.txt \
                                     wz_cr=card_"+era+"_"+channel+"_"+mass+"_wz_cr.txt \
                                     zz_cr=card_"+era+"_"+channel+"_"+mass+"_zz_cr.txt \
                                     > card_"+era+"_"+channel+"_"+mass+"_sr"+systTag+"_NoInv_Combined.txt")
          os.system("combineCards.py \
                                     sr=card_"+era+"_"+channel+"_"+mass+"_sr"+systTag+".txt \
                                     ww_cr=card_"+era+"_"+channel+"_"+mass+"_ww_cr.txt \
                                     zg_cr=card_"+era+"_"+channel+"_"+mass+"_zg_cr.txt \
                                     wz_cr=card_"+era+"_"+channel+"_"+mass+"_wz_cr.txt \
                                     zz_cr=card_"+era+"_"+channel+"_"+mass+"_zz_cr.txt \
                                     > card_"+era+"_"+channel+"_"+mass+"_sr"+systTag+"_NoCFCR_NoInv_Combined.txt")
        elif args.Combine == "SR": # Combine SR1 only, SR2 only, SR3 only (no rateParam)
          os.system("combineCards.py \
                                     sr1=card_"+era+"_"+channel+"_"+mass+"_sronly_sr1"+systTag+".txt \
                                     sr2=card_"+era+"_"+channel+"_"+mass+"_sronly_sr2"+systTag+".txt \
                                     sr3=card_"+era+"_"+channel+"_"+mass+"_sronly_sr3"+systTag+".txt \
                                     > card_"+era+"_"+channel+"_"+mass+"_sronly_sr123"+systTag+".txt")
      if args.Combine == "Era": # This will combine all era datacards with CR setup
        os.system("combineCards.py year16a=card_2016preVFP_"+channel+"_"+mass+systTag+".txt year16b=card_2016postVFP_"+channel+"_"+mass+systTag+".txt year17=card_2017_"+channel+"_"+mass+systTag+".txt year18=card_2018_"+channel+"_"+mass+systTag+".txt > card_Run2_"+channel+"_"+mass+systTag+".txt")
    os.system('echo \'Done.\'')
    os.chdir(pwd)
