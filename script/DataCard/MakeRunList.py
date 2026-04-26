# Place this at CombineTool/CMSSW_10_2_13/src/DataCardsShape/HNL_SignalRegion_Plotter
# python MakeRunList.py <directories> [-e 2017 2018] [-c EMu] [-m 100 200] [-s DYVBF] <--Work or --Limit>

import os, sys
import subprocess as cmd
import argparse
import re

parser = argparse.ArgumentParser()
parser.add_argument('dirNames', nargs='+') # nargs='+' force a user to feed this argument
parser.add_argument('-e', dest='eras', default=[], choices=['2016preVFP','2016postVFP','2017','2018','Run2','Run2Sum'], nargs='+')
parser.add_argument('-c', dest='channels', default=["MuMu","EE","EMu"], choices=['MuMu','EE','EMu','3ch'], nargs='+') # store [] if nothing is fed
parser.add_argument('-m', dest='masses', default=[], choices=["85","90","95","100","125","150","200","250","300","350","400","450","500","600","700","800","900","1000","1100","1200","1300","1500","1700","2000","2500","3000","5000","7500","10000","15000","20000","25000","30000","40000","50000","60000"], nargs='+')
parser.add_argument('-s', dest='signals', default=["HNL","Weinberg"], choices=["HNL","DY","VBF","DYVBF","SSWW","Weinberg"], nargs='+')
parser.add_argument('-t', dest='tags', default=["AllSR"], choices=["AllSR","SR1","SR2","SR3"], nargs='+')
parser.add_argument('--Ext', action='store_true', help='Extend cut based approach to M500')
parser.add_argument('--Work', action='store_true', help='for workspace production purposes')
parser.add_argument('--Limit', action='store_true', help='for limit extraction purposes')
args = parser.parse_args()

if not args.Work and not args.Limit:
  print("Please set --Work or --Limit;")
  print("Exiting...")
  sys.exit()

input_path = os.getcwd()

RUN2_LIKE_ERAS = ["Run2", "Run2Sum"]
is_run2_like = any(era in RUN2_LIKE_ERAS for era in args.eras)

if any(era in RUN2_LIKE_ERAS for era in args.eras) and len(args.eras) > 1:
  parser.error("Run2 and Run2Sum should be requested alone. Do not mix them with each other or with individual eras.")

# Choose one card name to represent all
#CardRep = "sr3_inv"
#CardRep = "sr3_InvMET" # new CR where Bjet and InvMET split
#CardRep = "sronly_sr3_syst" # NoCR and Syst
#CardRep = "sronly_sr123" # NoCR and NoSyst
#CardRep = "syst.txt" if "Run2" in args.eras else "sr3_inv" # before ANv5
#CardRep = "syst.txt" if "Run2" in args.eras else "sr3_InvBJet" # before ANv7 FullJES
#grepRegion = ' | grep card' if "Run2" in args.eras else ' | grep '+CardRep # When you grep an individual era, there are many duplications with different regions, namely sr1, ww_cr, sr3_inv, etc, and even directories! Pick just one using 'sr3_inv' (Run2: pick everything by grepping 'card')
#grepRegion = ' | grep card | grep -Ev "sr123"' if "Run2" in args.eras else ' | grep '+CardRep # When you grep an individual era, there are many duplications with different regions, namely sr1, ww_cr, sr3_inv, etc, and even directories! Pick just one using 'sr3_inv' (Run2: pick sr1, 2, 3 separate limits by grepping all but removing sr123)

#CardRep = "syst.txt" if "Run2" in args.eras else "cr3_InvBJet"
#grepRegion = ' | grep card | grep '+CardRep if "Run2" in args.eras else ' | grep '+CardRep # When you grep an individual era, there are many duplications with different regions, namely sr1, ww_cr, sr3_inv, etc, and even directories! Pick just one using 'sr3_inv' (Run2: pick everything by grepping 'card')

CardRep = "syst.txt" if is_run2_like else "cr3_InvBJet"
grepRegion = ' | grep card | grep '+CardRep if is_run2_like else ' | grep '+CardRep

TAG_MAP = {
  "AllSR": ["_syst"],
  "SR1": ["_sr1_syst_Combined"],
  "SR2": ["_sr2_syst_Combined"],
  "SR3": ["_sr3_syst_Combined"],
}

tags = []
for tag_key in args.tags:
  tags.extend(TAG_MAP[tag_key])

#tags = ["_sronly"]
#tags = ["_syst"]
#tags = ["_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined"]
#tags = ["_syst","_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined"]
#tags = [""]
#tags = ["_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined","_syst"]
#tags = ["_sr1_syst","_sr2_syst","_sr3_syst","_sr_syst"]
#tags = ["_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined","_sr_syst_Combined","_syst","_sr1_syst","_sr2_syst","_sr3_syst","_sr_syst"]
#tags = ["_sr_syst_NoCFCR_Combined","_sr_syst_NoInv_Combined","_sr_syst_NoCFCR_NoInv_Combined"]

## Inclusive
#tags = ["_sr1_syst_Combined","_sr2_syst_Combined","_sr3_syst_Combined","_sr_syst_Combined","_syst","_sr1_syst","_sr2_syst","_sr3_syst","_sr_syst","_sr_syst_NoCFCR_Combined","_sr_syst_NoInv_Combined","_sr_syst_NoCFCR_NoInv_Combined"]

## No syst
#tags = ["_sr1_Combined","_sr2_Combined","_sr3_Combined","_sr_Combined","","_sr_NoCFCR_Combined","_sr_NoInv_Combined","_sr_NoCFCR_NoInv_Combined"]

## SR only
#tags = ["_sronly_sr1","_sronly_sr2","_sronly_sr3","_sronly_sr123","_sronly_sr","_sronly_sr1_syst","_sronly_sr2_syst","_sronly_sr3_syst","_sronly_sr123_syst","_sronly_sr_syst"]
#tags = ["_sronly_sr123_syst"] # no CR, sr123 combined, with Syst
#tags = ["_sronly_sr1_syst","_sronly_sr2_syst","_sronly_sr3_syst"] # no CR, sr1, 2, 3, separate, with Syst
#tags = ["_sronly_sr123"] # NoCR, NoSyst
#tags = ["_sronly_sr2_syst"] # no CR, sr1, 2, 3, separate, with Syst

## CR limit test
#tags = ["_sr2_syst_Combined_OnlyWZNormToAll","_sr2_syst_Combined_WZZGNormToAll"]

def keep_seed_card(raw_card, is_run2_like):
  """
  Decide whether a card should be used as the seed for RunList generation.

  Important:
    TAG_MAP appends the final suffix later.
    Therefore the seed card should represent only the mass/channel/signal base.

  For Run2/Run2Sum CR-combined default:
    keep:
      card_Run2Sum_EE_M10000_HNL_syst.txt

    reject:
      card_Run2Sum_EE_M10000_HNL_sr1_syst.txt
      card_Run2Sum_EE_M10000_HNL_sr1_syst_Combined.txt
      card_Run2Sum_EE_M10000_HNL_sronly_sr123_syst.txt
  """

  card = os.path.basename(raw_card.strip())

  if card == "":
    return False

  if card.startswith("ls:"):
    return False

  if not is_run2_like:
    return True

  # Run2-like seed cards should be inclusive CR-combined cards.
  if not card.endswith("_syst.txt"):
    return False

  # Do not use base SR-region cards or per-SR combined cards as seeds.
  # Per-SR targets are selected through TAG_MAP, e.g. "_sr1_syst_Combined".
  if re.search(r"_(sr1|sr2|sr3)_syst(\.txt|_Combined\.txt)$", card):
    return False

  # NoCR / SR-only cards should not be mixed into the CR-combined default list.
  if "_sronly_" in card:
    return False

  return True

RunListEraTag = ""
if "Run2Sum" in args.eras:
  RunListEraTag = "Run2Sum_"
elif "Run2" in args.eras:
  RunListEraTag = "Run2_"

for dirName in args.dirNames:

  era_grep = ""
  if args.eras:
    era_grep = " | grep " + " ".join(["-e card_"+era+"_" for era in args.eras])

  channel_grep = ""
  if args.channels:
    channel_grep = " | grep " + " ".join(["-e "+channel for channel in args.channels])

  mass_grep = ""
  if args.masses:
    mass_grep = " | grep " + " ".join(["-e M"+mass+"_" for mass in args.masses])

  signal_grep = ""
  if args.signals:
    signal_grep = " | grep " + " ".join(["-e "+signal for signal in args.signals])

  greps = 'ls ' + dirName + grepRegion + era_grep + channel_grep + mass_grep + signal_grep    
  if len(args.signals)==0: greps += ' | grep -Ev \"DY|VBF|SSWW|Weinberg\"' # When you don't want signal specific results and the Weinberg
  if args.Ext: greps += ' | grep Ext'
  else: greps += ' | grep -Ev \"Ext\"'
  #print(greps)

  raw_cards = cmd.getoutput(greps).split('\n')
  raw_cards = [card.strip() for card in raw_cards if keep_seed_card(card, is_run2_like)]

  cards = sorted(set([
    card.replace('_'+CardRep+'.txt','').replace('_syst.txt','')
    for card in raw_cards
  ]))

  #print("raw_cards:", raw_cards)
  #print("seed cards:", cards)

  if len(cards) == 0:
    print("[WARNING] No seed cards found in", dirName)

  dirName = dirName.replace('/','')
  with open("RunList_"+RunListEraTag+dirName+".txt",'w') as f:
    for card in cards:
      for tag in tags:
        this = input_path+"/"+dirName+"/"+card+tag+".root\n"
        FileCheck = cmd.getstatusoutput('ls '+this.strip().replace('root','txt'))
        if FileCheck[0] !=0:
          print("[WARNING] NO "+this.replace('root','txt').strip())
          print("skipping...")
          continue
        if args.Work:
          f.write(this)
        elif args.Limit:
          shortcard = this.split('/')[-1].replace(".root","").replace(".txt","").replace("card_","").strip('\n')
          this = input_path+"/"+dirName+"/"+shortcard+"/"+shortcard+".root\n"
          f.write(this)
