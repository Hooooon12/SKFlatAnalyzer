#Skim V2
########### 1. totWeight now has zpt, z0, weak weight by default --> no need to apply zpt <-- this was deprecated from 250206 version ###########
# 2. MC pass the trigger --> no need to apply turn-on
# 3. tag is HEEP (no medium charge <-- this was V10)
# 4. No mass requirement on TP pair
# 5. Assume only one out of two lepton is charge-flipped (tag first)
# 6. No SingleTop, TTLJ, DY NLO

import os, sys, argparse
import commands as cmd
import argparse
import math
import numpy as np
from collections import OrderedDict
from datetime import datetime
from ROOT import *
gROOT.SetBatch(kTRUE)

parser = argparse.ArgumentParser(description='Tool for high pt electron SF measurement', formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-n', dest='Nevents', type=int, default=-1, help='Number of events to run; < 0 : full')
parser.add_argument('-t', dest='Time', action='store_true', help='Print running times')
parser.add_argument('-nj', dest='NJob', type=int, default=0, help='Number of job')
parser.add_argument('-e' , dest='Eras', default=[], nargs='+')
parser.add_argument('-wd' , dest='WorkDir', default ='./')
parser.add_argument('--measure' , dest='Measure', default =['ID','Trig'], help='(Save only option) What to measure: ID or Trig')
parser.add_argument('--mass' , dest='Mass', default =[70,110], nargs='+', type=int, help='Dilepton mass range to study')
parser.add_argument('-ScaleCF', dest='ScaleCF', type=float, default=1, help='SF for CF')
parser.add_argument('--syst' , dest='Syst', default =[], nargs='*', help='DY or QCD Up/Down/Side or CFSF Up/Down')
parser.add_argument('--sumUp' , dest='SumUp', action='store_true', help='Save syst uncertainty in SF rootfile')

args = parser.parse_args()

ScaleCF=args.ScaleCF
NJob=args.NJob
WorkDir=args.WorkDir
MassName="_M"+str(args.Mass[0])+"to"+str(args.Mass[1])
SystName=""
if len(args.Syst)!=0:
  SystName += "_Syst"
  for this_syst in args.Syst:
    SystName += "_"+this_syst 
if "CFSF" in args.Syst and ScaleCF==1:
  print "CFSF syst shouldn't be 1."
  print "Please add -ScaleCF e.g. 1.2"
  exit()

It_Probes    = ['MVALoose','MVABaseline','HNLMVA','HNLMVA_HighPt', 'HNL_ULID_Split_1','HNL_ULID_Split_2','HNL_ULID_Split_3','HNL_ULID_Split_4','HNL_ULID_Split_4b','HNL_ULID_Split_5','HNL_ULID_Split_5b','HNL_ULID_Split_6','HNL_ULID_Split_7','HNL_ULID_Split_8','HNLMVA_NoCF','HNLMVA_NoConv','HNLMVA_NoFake','passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg1','passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg2']
#### IDs applied to probe before PASS/FAIL
It_ProbeID   = ['Pass',    'Pass',       'Pass',  'Pass',          'Pass',            'HNL_ULID_Probe_Split_2','HNL_ULID_Probe_Split_3','HNL_ULID_Probe_Split_4','HNL_ULID_Probe_Split_4','HNL_ULID_Probe_Split_5','HNL_ULID_Probe_Split_5','HNL_ULID_Probe_Split_6','HNL_ULID_Probe_Split_7','HNL_ULID_Probe_Split_8' ,'MVABaseline','MVABaseline','MVABaseline','HNLMVA_HighPt','HNLMVA_HighPt']

NID_Full= 0

for tmpID in It_ProbeID:
  if tmpID == 'Pass':
    NID_Full=NID_Full+1

It_IsPasses = ['Pass','Fail']
#It_EtaRegions = ['BB','EC']
It_EtaRegions = ['IB','OB','EC']
#It_Charges = ["os","ss","ss_zpt","ss_tot","ss_zpt_tot"]
It_Charges = ["os","ss","ss_tot"]

grouped_eras = {}
for Era in args.Eras:
  if Era != "2016":
    grouped_eras[Era] = [Era]
  else:
    grouped_eras[Era] = ["2016preVFP","2016postVFP"]

luminosity = {
  '2016' : '36.3',
  '2016preVFP' : '19.5',
  '2016postVFP' : '16.8',
  '2017' : '41.5',
  '2018' : '59.8',
}

samples = {
  '2016' : [
    "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8",
    "ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8",
    "TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8",
    "WZG_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWG_TuneCP5_13TeV-amcatnlo-pythia8",
    "ZZTo4L_TuneCP5_13TeV_powheg_pythia8",
    "WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8",
    "WZZ_TuneCP5_13TeV-amcatnlo-pythia8",
    "ZZZ_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8",
    "WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8",
    "WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8",
    "GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8",
    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    
    "SingleElectron",
  ],
  '2016preVFP' : [
    "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8",
    "ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8",
    "TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8",
    "WZG_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWG_TuneCP5_13TeV-amcatnlo-pythia8",
    "ZZTo4L_TuneCP5_13TeV_powheg_pythia8",
    "WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8",
    "WZZ_TuneCP5_13TeV-amcatnlo-pythia8",
    "ZZZ_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8",
    "WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8",
    "WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8",
    "GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8",
    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
     "SingleElectron",
  ],
  '2016postVFP' : [
    "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8",
    "ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8",
    "TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8",
    "WZG_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWG_TuneCP5_13TeV-amcatnlo-pythia8",
    "ZZTo4L_TuneCP5_13TeV_powheg_pythia8",
    "WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8",
    "WZZ_TuneCP5_13TeV-amcatnlo-pythia8",
    "ZZZ_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8",
    "WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8",
    "WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8",
    "GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8",
    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "SingleElectron",
  ],
  '2017' : [
    "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8",
    "ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8",
    "TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8",
    "WZG_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWG_TuneCP5_13TeV-amcatnlo-pythia8",
    "ZZTo4L_TuneCP5_13TeV_powheg_pythia8",
    "WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8",
    "WZZ_TuneCP5_13TeV-amcatnlo-pythia8",
    "ZZZ_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8",
    "WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8",
    "WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8",
    "GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8",
    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "SingleElectron",
  ],
  '2018' : [
    "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8",
    "ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8",
    "TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8",
    "WZG_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWG_TuneCP5_13TeV-amcatnlo-pythia8",
    "ZZTo4L_TuneCP5_13TeV_powheg_pythia8",
    "WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8",
    "WZZ_TuneCP5_13TeV-amcatnlo-pythia8",
    "ZZZ_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8",
    "WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8",
    "WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8",
    "WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8",
    "GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8",
    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "EGamma",
  ],
}

# Use below when all era shares the same MC list
#samples = {}
#sample_list = [
#    "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
#    "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
#    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
#    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
#    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
#    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
#    "WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8",
#    "ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8",
#    "TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
#    "TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8",
#    "WZG_TuneCP5_13TeV-amcatnlo-pythia8",
#    "WWG_TuneCP5_13TeV-amcatnlo-pythia8",
#    "ZZTo4L_TuneCP5_13TeV_powheg_pythia8",
#    "WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
#    "WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8",
#    "WZZ_TuneCP5_13TeV-amcatnlo-pythia8",
#    "ZZZ_TuneCP5_13TeV-amcatnlo-pythia8",
#    "WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8",
#    "WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8",
#    "GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8",
#    "GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8",
#    "GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8",
#    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8",
#    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
#    "WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8",
#    "WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8",
#  ]
#for era in ['2016', '2016preVFP', '2016postVFP', '2017', '2018']:
#  this_sample_list = sample_list[:]
#  this_sample_list.append("SingleElectron") if era is not '2018' else this_sample_list.append("EGamma")
#  samples[era] = this_sample_list

if "DY" in args.Syst:
  for era in ['2016', '2016preVFP', '2016postVFP', '2017', '2018']:
    samples[era].pop(0)
    samples[era].pop(0)
    samples[era].insert(0,"DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8")

types = {
  'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos'  : 'MC',
  'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : 'MC',
  'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : 'MC',
  'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : 'MC',
  'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8': 'MC',
  'ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8': 'MC',
  'TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': 'MC',
  'TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8': 'MC',
  'WZG_TuneCP5_13TeV-amcatnlo-pythia8': 'MC',
  'WWG_TuneCP5_13TeV-amcatnlo-pythia8': 'MC',
  'ZZTo4L_TuneCP5_13TeV_powheg_pythia8': 'MC',
  'WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8': 'MC',
  'WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8': 'MC',
  'WZZ_TuneCP5_13TeV-amcatnlo-pythia8': 'MC',
  'ZZZ_TuneCP5_13TeV-amcatnlo-pythia8': 'MC',
  'WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8': 'MC',
  'WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8': 'MC',
  'GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8': 'MC',
  'GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8': 'MC',
  'GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8': 'MC',
  'TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8': 'MC',
  'TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': 'MC',
  'WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8': 'MC',
  'WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8': 'MC',
  'SingleElectron' : 'DATA',
  'EGamma' : 'DATA',
}

# Dates are dummy, codde now reads dir from /gv0/
dates = {
  '2016preVFP':
    {
      'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {'' : '2024_12_03_013251',},
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {'' : '2024_12_03_013251',},
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {'' : '2024_12_03_013251',},
      'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {'' : '2024_12_03_013251',},
      'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8': {     '' : '2024_12_03_013251',  },
      'ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8': {     '' : '2024_12_03_013251',  },
      'TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'WZG_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWG_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'ZZTo4L_TuneCP5_13TeV_powheg_pythia8': {     '' : '2024_12_03_013251',  },
      'WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8': {     '' : '2024_12_03_013251',  },
      'WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8': {     '' : '2024_12_03_013251',  },
      'WZZ_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'ZZZ_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8': {     '' : '2024_12_03_013251',  },
      'WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8': {     '' : '2024_12_03_013251',  },
     'SingleElectron' : {
                         'periodB_ver2' : '2024_11_26_102156',
                         'periodC'      : '2024_11_26_102156',
                         'periodD'      : '2024_11_26_102156',
                         'periodE'      : '2024_11_26_102156',
                         'periodF'      : '2024_11_26_102156',
                        },
    },
  '2016postVFP':
    {

      'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {'' : '2024_12_03_013251',},
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {'' : '2024_12_03_013251',},
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {'' : '2024_12_03_013251',},
      'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {'' : '2024_12_03_013251',},
      'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' :{'' : '2024_12_03_013251',},
      'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8': {'' : '2024_12_03_013251',},
      'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8':  {'' : '2024_12_03_013251',},
      'WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8': {     '' : '2024_12_03_013251',  },
      'ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8': {     '' : '2024_12_03_013251',  },
      'TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'WZG_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWG_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'ZZTo4L_TuneCP5_13TeV_powheg_pythia8': {     '' : '2024_12_03_013251',  },
      'WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8': {     '' : '2024_12_03_013251',  },
      'WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8': {     '' : '2024_12_03_013251',  },
      'WZZ_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'ZZZ_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8': {     '' : '2024_12_03_013251',  },
      'WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8': {     '' : '2024_12_03_013251',  },
      'SingleElectron' : {
        'periodF' : '2024_11_26_102156',
        'periodG' : '2024_11_26_102156',
        'periodH' : '2024_11_26_102156',
      },
    },
  '2017':
    {

      'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {'' : '2024_12_03_013251',},
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {'' : '2024_12_03_013251',},
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {'' : '2024_12_03_013251',},
      'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {'' : '2024_12_03_013251',},
      'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8': {     '' : '2024_12_03_013251',  },
      'ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8': {     '' : '2024_12_03_013251',  },
      'TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'WZG_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWG_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'ZZTo4L_TuneCP5_13TeV_powheg_pythia8': {     '' : '2024_12_03_013251',  },
      'WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8': {     '' : '2024_12_03_013251',  },
      'WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8': {     '' : '2024_12_03_013251',  },
      'WZZ_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'ZZZ_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8': {     '' : '2024_12_03_013251',  },
      'WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8': {     '' : '2024_12_03_013251',  },
      
      'SingleElectron' : {
                         'periodB' : '2024_12_13_185927',
                         'periodC' : '2024_12_13_185927',
                         'periodD' : '2024_12_13_185927',
                         'periodE' : '2024_12_13_185927',
                         'periodF' : '2024_12_13_185927',
                        },
    },
  '2018':
    {

      'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos': {'' : '2024_12_03_013251',},
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos':  {'' : '2024_12_03_013251',},
      'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : {'' : '2024_12_03_013251',},
      'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {'' : '2024_12_03_013251',},
      'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {'' : '2024_12_03_013251',},
      'WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8': {     '' : '2024_12_03_013251',  },
      'ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8': {     '' : '2024_12_03_013251',  },
      'TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'WZG_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWG_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'ZZTo4L_TuneCP5_13TeV_powheg_pythia8': {     '' : '2024_12_03_013251',  },
      'WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8': {     '' : '2024_12_03_013251',  },
      'WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8': {     '' : '2024_12_03_013251',  },
      'WZZ_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'ZZZ_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8': {     '' : '2024_12_03_013251',  },
      'TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8': {     '' : '2024_12_03_013251',  },
      'TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': {     '' : '2024_12_03_013251',  },
      'WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8': {     '' : '2024_12_03_013251',  },
      'WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8': {     '' : '2024_12_03_013251',  },

     'EGamma' : {
                 'periodA' : '2024_12_13_185927',
                 'periodB' : '2024_12_13_185927',
                 'periodC' : '2024_12_13_185927',
                 'periodD' : '2024_12_13_185927',
                },
    },
}
triggers = {
            '2016'        : 'passHltEle27WPTightGsf',
            '2016preVFP'  : 'passHltEle27WPTightGsf',
            '2016postVFP' : 'passHltEle27WPTightGsf',
            '2017'        : 'passHltEle32DoubleEGWPTightGsf',
            '2018'        : 'passHltEle32WPTightGsf',
}
nameFilter = {

  'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos'  : 'DYJetsToEE',
  'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : 'DYJetsToTauTau',
  'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : 'DYJetsToTauTau',
  'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : 'DYJets',
  'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : 'TTLL',
  'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : 'TTLJ',
  'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'tW_top',
  'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'tW_antitop',
  'WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8': 'WG',
  'ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8': 'ZG',
  'TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': 'TTG',
  'TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8': 'TG',
  'WZG_TuneCP5_13TeV-amcatnlo-pythia8': 'WZG',
  'WWG_TuneCP5_13TeV-amcatnlo-pythia8': 'WWG',
  'ZZTo4L_TuneCP5_13TeV_powheg_pythia8': 'ZZ',
  'WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8': 'WW',
  'WZTo3LNu_mllmin4p0_TuneCP5_13TeV-powheg-pythia8': 'WZ',
  'WZZ_TuneCP5_13TeV-amcatnlo-pythia8': 'WZZ',
  'ZZZ_TuneCP5_13TeV-amcatnlo-pythia8': 'ZZZ',
  'WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8': 'WWZ',
  'WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8': 'WWW',
  'GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8': 'GGZZ4e',
  'GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8': 'GGZZ2e2mu',
  'GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8': 'GGZZ2e2tau',
  'TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8': 'TTZ',
  'TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8': 'TTW',
  'WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8': 'WpWpQCD',
  'WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8': 'WpWpEWK',
  'SingleElectron' : 'data',
  'EGamma' : 'data',
}

def GetMinMax(*hists):

  if not hists:
    raise ValueError("[GetMinMax] At least one histogram must be provided.")

  global_min = float('inf')
  global_max = float('-inf')

  for hist in hists:
    if not isinstance(hist, TH1):
      raise TypeError("[GetMinMax] Expected a TH1 histogram or subclass, but got " + str(type(hist)))

    # Get the minimum and maximum bin content of the histogram
    local_min_bin = hist.GetMinimumBin()
    local_min = hist.GetBinContent(local_min_bin)
    local_max_bin = hist.GetMaximumBin()
    local_max = hist.GetBinContent(local_max_bin)

    # Update global min and max
    global_min = min(global_min, local_min)
    global_max = max(global_max, local_max)

  return global_min, global_max


def add_overflow(hist):

  last_bin = hist.GetNbinsX()
  over_bin = last_bin+1

  last_bin_content = hist.GetBinContent(last_bin)
  over_bin_content = hist.GetBinContent(over_bin)
  last_bin_error = hist.GetBinError(last_bin)
  over_bin_error = hist.GetBinError(over_bin)

  hist.SetBinContent(last_bin, last_bin_content + over_bin_content)
  hist.SetBinError(last_bin, math.sqrt(last_bin_error ** 2 + over_bin_error ** 2))

  return hist

def merge_lastbins(hist):
  this_name = hist.GetName()

  #pt_bins_merged = np.array([35, 40, 45, 50, 60, 70, 80, 100, 200, 1000], dtype=np.float64)
  #pt_bins_merged = np.array([35, 40, 45, 50, 60, 70, 80, 100, 200, 300, 1000], dtype=np.float64) # up to V5 and V7
  #pt_bins_merged = np.array([35, 40, 45, 50, 60, 70, 80, 100, 150, 200, 300, 1000], dtype=np.float64) # V6, V8 and later

  #if 'BB' in this_name:
  if 'B' in this_name:
    pt_bins_merged = np.array([35, 40, 45, 50, 60, 70, 80, 100, 150, 200, 300, 1000], dtype=np.float64)
  else:
    pt_bins_merged = np.array([35, 40, 45, 50, 60, 70, 80, 100, 150, 200, 1000], dtype=np.float64)

  nbins_input = hist.GetNbinsX()
  nbins_new   = len(pt_bins_merged) - 1
  if nbins_input == nbins_new:
    return hist
  nbins_merge = nbins_input - nbins_new + 1
  xmin = hist.GetXaxis().GetXmin()
  xmax = hist.GetXaxis().GetXmax()

  new_hist = TH1D(this_name+"_merged", this_name+"_merged", nbins_new, pt_bins_merged)

  for i in range(nbins_input-nbins_merge):
    new_hist.SetBinContent(i+1, hist.GetBinContent(i+1))
    new_hist.SetBinError(i+1, hist.GetBinError(i+1))

  last_bin_sum   = 0
  last_bin_error = 0
  for i in range(nbins_merge):
    last_bin_sum   += hist.GetBinContent(nbins_input-i)
    last_bin_error += hist.GetBinError(nbins_input-i)**2
  last_bin_error = last_bin_error**0.5

  #last_bin_sum = hist.GetBinContent(nbins_input) + hist.GetBinContent(nbins_input-1) + hist.GetBinContent(nbins_input-2)
  #last_bin_error = ((hist.GetBinError(nbins_input)**2 + hist.GetBinError(nbins_input-1)**2 + hist.GetBinError(nbins_input-2)**2) ** 0.5)

  new_hist.SetBinContent(new_hist.GetNbinsX(), last_bin_sum)
  new_hist.SetBinError(new_hist.GetNbinsX(), last_bin_error)

  new_hist.SetDirectory(0)

  return new_hist

def makeTurnOn():
  from os import listdir
  from os.path import isfile, isdir,join
  NFiles={}

  for year, eras in grouped_eras.items():

    for era in eras:

      #TurnOnOutFile = TFile.Open(WorkDir+"/Out_TurnOn/TurnOn_"+era+".root","RECREATE")
      if "Version10" in WorkDir:
        os.system('mkdir -p '+WorkDir+"/Out_TurnOn_Version10/")
        TurnOnOutFile = TFile.Open(WorkDir+"/Out_TurnOn_Version10/TurnOn_"+era+".root","RECREATE")
      else:
        print "Not supported working directory; please check",WorkDir,"..."
        exit()

      for sample in samples[era]:

        t1 = datetime.now()
        print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling",era,sample,"..."
        #path_to_add = []
        this_chain = TChain("tnpEleIDs/fitter_tree")

        path_to_files="/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HNLHighPt/"+sample
        if isdir(path_to_files):
          print path_to_files
        else:
          print path_to_files, "doesn't exist. skipping..."
          continue

        if types[sample] == "MC":
            if "DYJetsToEE" not in sample and "DYJetsToLL" not in sample: continue # check DY turn-on only
            datedir = [f for f in listdir(path_to_files) if isdir(join(path_to_files, f))]
            datedir = sorted(datedir)

            print "Use the last item of:",datedir
            new_path_to_files = path_to_files + "/"+datedir[-1]
            infiles = [join(new_path_to_files, f) for f in listdir(new_path_to_files) if isfile(join(new_path_to_files, f))]
        else:
            perioddir = [f for f in listdir(path_to_files) if isdir(join(path_to_files, f))]

            for period in perioddir:
                period_path_to_files  = path_to_files + "/"+period
                datedir = [f for f in listdir(period_path_to_files) if isdir(join(period_path_to_files, f))]
                datedir = sorted(datedir)

                print "Use the last item of:",datedir
                new_path_to_files = period_path_to_files + "/"+datedir[-1]
                infiles = [join(new_path_to_files, f) for f in listdir(new_path_to_files) if isfile(join(new_path_to_files, f))]

        #for period in dates[era][sample]:
        #  #path_to_add.append("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root")
        #  path_to_add.append("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HNLHighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root")
          
        print "Adding",new_path_to_files,"..."
        for path in infiles:
          #print "Adding",path,"..."
          this_chain.Add(path)
        t2 = datetime.now()
        print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
      
        #t1 = datetime.now()
        #print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling total entries ..."
        #print "Total",this_chain.GetEntries(),"events." # The most time consuming part
        #t2 = datetime.now()
        #print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
      
        outName = era+"_"+nameFilter[sample]

        charges = ["Inc","OS","SS","Subt"]
        chargeComms = {
                       "Inc"  : "(1==1)",
                       "OS"   : "(el_q+tag_Ele_q==0)",
                       "SS"   : "(el_q+tag_Ele_q!=0)",
                       "Subt" : "(el_q+tag_Ele_q==0)",
        }
        tags = ["","tag_"]
        tagPts = {
                  "" : "el_pt_cor",
                  "tag_" : "tag_Ele_pt_cor",
        }
        trigs = ["den_",outName+"_"]
        trigComms = {
                      "den_" : "",
                      outName+"_" : "&&"+triggers[era],
        }
        IDs = ["","HEEP_"]
        IDComms = {
                      "" : "",
                      "HEEP_" : "&&(passingHEEP)",
        }
        Barrels = ["","barrel_"]
        BarrelComms = {
                       "" : "",
                       "barrel_" : "&&(fabs(el_sc_eta)<=1.4442)",
        }

        denlist = []
        numlist = []

        t1 = datetime.now()
        print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Main jobs begin ..."
        for charge, tag, trig, ID, Barrel in [(charge, tag, trig, ID, Barrel) for charge in charges for tag in tags for trig in trigs for ID in IDs for Barrel in Barrels]:
          print "Running: this_chain.Draw(\""+tagPts[tag]+">>h_"+tag+trig+ID+Barrel+charge+"(200,0,200)\",\""+chargeComms[charge]+trigComms[trig]+IDComms[ID]+BarrelComms[Barrel]+"\")"
          this_chain.Draw(tagPts[tag]+">>h_"+tag+trig+ID+Barrel+charge+"(200,0,200)",chargeComms[charge]+trigComms[trig]+IDComms[ID]+BarrelComms[Barrel])
          denlist.append(gDirectory.Get("h_"+tag+trig+ID+Barrel+charge)) if "den" in trig else numlist.append(gDirectory.Get("h_"+tag+trig+ID+Barrel+charge))
          if not "data" in outName:
            this_chain.Draw(tagPts[tag]+">>h_"+tag+trig+ID+Barrel+charge+"_gen(200,0,200)",chargeComms[charge]+trigComms[trig]+IDComms[ID]+BarrelComms[Barrel])
            denlist.append(gDirectory.Get("h_"+tag+trig+ID+Barrel+charge+"_gen")) if "den" in trig else numlist.append(gDirectory.Get("h_"+tag+trig+ID+Barrel+charge+"_gen"))
        t2 = datetime.now()
        print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
        #for i in range(len(denlist)):
        #  print denlist[i].GetName()
        #for i in range(len(denlist)):
        #  print numlist[i].GetName()
        #return

        print "last bin:",denlist[0].GetBinContent(200), "overflow bin:",denlist[0].GetBinContent(201)
        for i in range(len(denlist)):
          denlist[i] = add_overflow(denlist[i])
          numlist[i] = add_overflow(numlist[i])
        print "now last bin:",denlist[0].GetBinContent(200)
      
        for i in range(len(denlist)):
          if "Subt" in denlist[i].GetName():
            if "gen" in denlist[i].GetName(): # MC
              denlist[i].Add(denlist[i-16],-1)
              numlist[i].Add(numlist[i-16],-1)
            else: # data
              denlist[i].Add(denlist[i-8],-1)
              numlist[i].Add(numlist[i-8],-1)
      
        for i in range(len(denlist)):
          numlist[i].Divide(numlist[i],denlist[i],1,1,"B")
      
        TurnOnOutFile.cd()
        for i in range(len(denlist)):
          numlist[i].Write()
      
      TurnOnOutFile.Close()

  return

def classify_hist(this_year, this_sample, this_chain):

  if this_chain.el_pt_cor < 35.: return None

  if not (args.Mass[0]<=this_chain.pair_mass_cor and this_chain.pair_mass_cor<=args.Mass[1]): return None

  conv_list = [
               "WGToLNuG_TuneCP5_13TeV-madgraphMLM-pythia8",
               "ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8",
               "TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
               "TGJets_TuneCP5_13TeV-amcatnlo-madspin-pythia8",
               "WZG_TuneCP5_13TeV-amcatnlo-pythia8",
               "WWG_TuneCP5_13TeV-amcatnlo-pythia8",
              ]

  if this_sample in conv_list: # allow no prompt only conversion
    if this_chain.mcTrue or not this_chain.mcConv:
      return None
  else: # allow only prompt
    if not this_chain.mcTrue:
      return None

  # EtaRegion
  #if abs(this_chain.el_sc_eta) < 1.4442:
  #  EtaRegion = 'BB'
  if abs(this_chain.el_sc_eta) < 0.8:
    EtaRegion = 'IB'
  elif abs(this_chain.el_sc_eta) < 1.4442:
    EtaRegion = 'OB'
  elif 1.566 < abs(this_chain.el_sc_eta) < 2.5:
    EtaRegion = 'EC'
  else:
    return None

  if EtaRegion not in It_EtaRegions:
    return None

  # Charge
  Charges = []
  if (this_chain.el_q+this_chain.tag_Ele_q)==0:
    Charges.append('os')
  elif (this_chain.el_q+this_chain.tag_Ele_q)!=0:
    #Charges.append('ss_OSTurnOn') # add this if necessary
    if getattr(this_chain, "tag_"+triggers[this_year]):
      Charges.append('ss')
      #Charges.append('ss_zpt')

  # Probe type
  Probes = {}
  nProbe=0
  for this_probe in It_Probes:
    ProbeID=It_ProbeID[nProbe]
    nProbe=nProbe+1

    ### Apply probe ID before Pass/Fail check
    if ProbeID != "Pass":
      if not getattr(this_chain, "passing"+ProbeID):
        continue

    if "Hlt" not in this_probe:
      Probes[this_probe] = 'Pass' if getattr(this_chain, "passing"+this_probe) else 'Fail'
    else:
      Probes[this_probe] = 'Pass' if getattr(this_chain, this_probe) else 'Fail'
    #for attr in dir(this_chain):
    #  if attr.startswith('passing'):
    #    ID = attr.replace('passing','')
    #    Probes[ID] = 'Pass' if getattr(this_chain, attr) else 'Fail' # use this to include every IDs

  return EtaRegion, Charges, Probes

def makeCompPlots(Data_OS, Stack, Bundle, Error, Era, EtaRegion, Probe, OutTag, n_job, MassName, SystName, HistStackSetting):

  for this_measure in args.Measure:
    os.system('mkdir -p '+WorkDir+"/"+this_measure+"/Main/"+Era+"/DataMC"+MassName+"/"+SystName.lstrip('_')) # Trigger: Main only
    if this_measure=="ID":
      os.system('mkdir -p '+WorkDir+"/"+this_measure+"/Support/"+Era+"/DataMC"+MassName+"/"+SystName.lstrip('_'))

  if Probe=="HNLMVA":
    NameProbe="HNLMVA_Old"
    SaveDir = "ID/Main"
  elif Probe=="HNLMVA_HighPt":
    NameProbe="HNLMVA"
    SaveDir = "ID/Main"
  elif Probe=="MVABaseline":
    NameProbe="MVABaseline"
    SaveDir = "ID/Main"
  elif Probe=="AllProbes":
    NameProbe="AllProbes"
    SaveDir = "ID/Main"
  elif Probe=="passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg1":
    NameProbe="Ele23Leg1"
    SaveDir = "Trig/Main"
  elif Probe=="passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg2":
    NameProbe="Ele12Leg2"
    SaveDir = "Trig/Main"
  else:
    NameProbe=Probe
    SaveDir = "ID/Support"

  TS=Era+"_"+EtaRegion+"_"+NameProbe+OutTag+"_"+ str(n_job) if n_job >=0 else Era+"_"+EtaRegion+"_"+NameProbe+OutTag

  for i in range(Bundle[-1].GetNbinsX()):
    if Bundle[-1].GetBinContent(i+1) <= 0:
      print "[ERROR] In",TS," bin",i+1,":",Bundle[-1].GetBinContent(i+1)

  c1 = TCanvas("c1_"+TS,"",1000,1000)
  c1.cd()
  gStyle.SetPadTickX(1)
  gStyle.SetPadTickY(1)

  c_up = TPad("c_up", "", 0, 0.25, 1, 1)
  c_up.SetTopMargin(0.08)
  c_up.SetBottomMargin(0.017)
  c_up.SetLeftMargin(0.14)
  c_up.SetRightMargin(0.04)
  c_up.SetLogx()
  c_up.SetLogy()
  c_up.Draw()
  c_up.cd()

  Stack.Draw("hist")
  Stack.SetTitle("")
  Stack.GetXaxis().SetLabelSize(0)
  Stack.GetYaxis().SetLabelSize(0.045)
  Stack.GetYaxis().SetTitle("Events")
  Stack.GetYaxis().SetTitleSize(0.075)
  Stack.GetYaxis().SetTitleOffset(0.8)
  Stack.SetMinimum(0.1)

  Error.SetMarkerSize(0)
  Error.SetLineWidth(0)
  Error.SetFillStyle(3144)
  Error.SetFillColor(kBlack)
  Error.Draw("e2 same")

  Data_OS.SetMarkerStyle(20)
  Data_OS.SetMarkerColor(kBlack)
  Data_OS.Draw("ep same")

  lg = TLegend(0.7, 0.6, 0.9, 0.9)
  lg.AddEntry(Error, "Stat. Uncertainty", "f")
  lg.AddEntry(Data_OS, "Data_OS", "lep")

  for hist_iter in range(len(HistStackSetting)):
    lg.AddEntry(Bundle[hist_iter], HistStackSetting.keys()[hist_iter], "f")
  lg.AddEntry(Bundle[-1], "Fake", "f")
  lg.SetBorderSize(0)
  lg.SetTextSize(0.03)
  lg.SetFillStyle(1001)
  lg.SetShadowColor(0)
  lg.Draw("same")
 
  txt_lumi = TLatex()
  txt_lumi.SetNDC()
  txt_lumi.SetTextSize(0.05)
  txt_lumi.SetTextAlign(32)
  txt_lumi.SetTextFont(42)
  txt_lumi.DrawLatex(.95,.96, luminosity[Era]+" fb^{-1} (13 TeV)")

  c1.cd()

  c_down = TPad("c_down", "", 0, 0, 1, 0.25)
  c_down.SetTopMargin(0.03)
  c_down.SetBottomMargin(0.35)
  c_down.SetLeftMargin(0.14)
  c_down.SetRightMargin(0.04)
  c_down.SetGridx()
  c_down.SetGridy()
  c_down.SetLogx()
  c_down.Draw()
  c_down.cd()

  this_nBins = Data_OS.GetNbinsX()

  Error_Stat = Error.Clone()
  for iBin in range(this_nBins):
    binContent = Error_Stat.GetBinContent(iBin+1)
    binError = Error_Stat.GetBinError(iBin+1)
    if binContent != 0.:
      binError = binError/binContent
    else:
      binError = 0.
    Error_Stat.SetBinContent(iBin+1, 1.)
    Error_Stat.SetBinError(iBin+1, binError)

  #print "Error of ratio:"
  #for iBin in range(this_nBins):
  #  print Error_Stat.GetBinError(iBin+1)

  Error_Stat.SetTitle("")
  Error_Stat.SetStats(0)
  Error_Stat.GetXaxis().SetTitle("p_{T} [GeV]")
  Error_Stat.GetYaxis().SetTitle("#frac{Obs.}{Pred.}")
  #Error_Stat.GetXaxis().SetRange(minBinNumber, maxBinNumber)
  Error_Stat.GetYaxis().SetRangeUser(0.5, 1.5)
  Error_Stat.GetXaxis().SetLabelSize(0.12)
  Error_Stat.GetYaxis().SetLabelSize(0.08)
  Error_Stat.GetXaxis().SetTitleSize(0.16)
  Error_Stat.GetYaxis().SetTitleSize(0.14)
  Error_Stat.GetXaxis().SetTitleOffset(0.9)
  Error_Stat.GetYaxis().SetTitleOffset(0.4)

  Error_Stat.SetMarkerSize(0)
  Error_Stat.SetLineWidth(0)
  Error_Stat.SetFillStyle(1001)
  Error_Stat.SetFillColor(kGray)
  Error_Stat.Draw("e2")

  Ratio = Data_OS.Clone()
  Ratio.Divide(Error)
  Ratio.SetLineColor(1)
  Ratio.SetMarkerColor(1)
  Ratio.SetMarkerStyle(20)
  Ratio.Draw("ep same")

  lg2 = TLegend(0.75, 0.88, 0.9, 0.95)
  lg2.SetNColumns(2)
  lg2.AddEntry(Error_Stat, "Stat. Uncert.", "f")
  lg2.SetBorderSize(1)
  lg2.SetTextSize(0.06)
  lg2.SetFillStyle(1001)
  lg2.SetShadowColor(0)
  lg2.Draw("same")

  minRange = Data_OS.GetBinLowEdge(1)
  maxRange = Data_OS.GetBinLowEdge(this_nBins) + Data_OS.GetBinWidth(this_nBins)

  line = TLine(minRange, 1., maxRange, 1.)
  line.SetLineWidth(1)
  line.SetLineColor(2)
  line.Draw()

  c1.SaveAs(WorkDir+"/"+SaveDir+"/"+Era+"/DataMC"+MassName+"/"+SystName.lstrip('_')+"/Pt_"+TS+MassName+SystName+".png")
  del c1

  return

def measureSFs(Data_OS, Bundle, Era, EtaRegion, Probe, OutTag, Save, n_job, OutFile, MassName, SystName):

  for this_measure in args.Measure:
    os.system('mkdir -p '+WorkDir+"/"+this_measure+"/Main/"+Era+"/SF"+MassName+"/"+SystName.lstrip('_'))
    if this_measure=="ID":
      os.system('mkdir -p '+WorkDir+"/"+this_measure+"/Support/"+Era+"/SF"+MassName+"/"+SystName.lstrip('_'))

  if Probe=="HNLMVA":
    NameProbe="HNLMVA_Old"
    SaveDir = "ID/Main"
  elif Probe=="HNLMVA_HighPt":
    NameProbe="HNLMVA"
    SaveDir = "ID/Main"
  elif Probe=="MVABaseline":
    NameProbe="MVABaseline"
    SaveDir = "ID/Main"
  elif Probe=="passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg1":
    NameProbe="Ele23Leg1"
    SaveDir = "Trig/Main"
  elif Probe=="passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg2":
    NameProbe="Ele12Leg2"
    SaveDir = "Trig/Main"
  else:
    NameProbe=Probe
    SaveDir = "ID/Support"

  OutName = "SF_Pt_"+Era+"_"+EtaRegion+"_"+NameProbe+OutTag+MassName+SystName

  this_nBins = Data_OS['Pass'].GetNbinsX()

  for i in range(len(Bundle['Pass'])):
    if i==0 : continue # skip DY
    Data_OS['Pass'].Add(Bundle['Pass'][i],-1)
    Data_OS['Fail'].Add(Bundle['Fail'][i],-1)

  Data_Eff = Data_OS['Pass'].Clone()
  Data_Tot = Data_OS['Pass'].Clone()
  Data_Tot.Add(Data_OS['Fail'])

  ## Save data integral before divide
  #combined_data_pass = Data_Eff.Integral()
  #combined_data_tot = Data_Tot.Integral()
  #combined_data_eff = combined_data_pass/combined_data_tot

  # handling exceptions
  Data_Eff_exc1 = []
  #print "Checking Data eff bins ..."
  for i in range(Data_Eff.GetNbinsX()):
    #print i+1, "th bin num:", Data_Eff.GetBinContent(i+1), "den:", Data_Tot.GetBinContent(i+1)
    if Data_Eff.GetBinContent(i+1) <= 0:
      Data_Eff.SetBinContent(i+1, 1)
      Data_Tot.SetBinContent(i+1, 0.01) # eff = 100 so out of range
    if Data_Eff.GetBinContent(i+1) > Data_Tot.GetBinContent(i+1): # den is less than num, due to negative weight
      #print ">>>>>>>>>>>>>>","bin",i+1,": Eff exceeds 1 !!!! <<<<<<<<<<<<<<<<<"
      Data_Eff_exc1.append([i+1,(Data_Eff.GetBinContent(i+1)-Data_Tot.GetBinContent(i+1))/Data_Tot.GetBinContent(i+1)]) # store errors
      Data_Tot.SetBinContent(i+1, Data_Eff.GetBinContent(i+1)) # make SF = 1
  Data_Eff.Divide(Data_Eff,Data_Tot,1,1,"B")
  for i in range(len(Data_Eff_exc1)):
    Data_Eff.SetBinError(Data_Eff_exc1[i][0], Data_Eff_exc1[i][1])

  MC_Eff = Bundle['Pass'][0].Clone()
  MC_Tot = Bundle['Pass'][0].Clone()
  MC_Tot.Add(Bundle['Fail'][0])

  ## Save MC integral before divide
  #combined_mc_pass = MC_Eff.Integral()
  #combined_mc_tot = MC_Tot.Integral()
  #combined_mc_eff = combined_mc_pass/combined_mc_tot

  ## Get the combined SF
  #combined_sf = combined_data_eff/combined_mc_eff

  #print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  #print "Running", OutName, "..."
  #print "Combined data efficiency:", combined_data_eff
  #print "Combined mc efficiency:", combined_mc_eff
  #print "Combined scale factor:", combined_sf # FIXME add error later
  #print "Saving into", OutFile.GetName(), "..." # FIXME add error later
  #print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

  # handling exceptions
  MC_Eff_exc1 = []
  #print "Checking MC eff bins ..."
  for i in range(MC_Eff.GetNbinsX()):
    #print i+1, "th bin num:", MC_Eff.GetBinContent(i+1), "den:", MC_Tot.GetBinContent(i+1)
    if MC_Eff.GetBinContent(i+1) <= 0:
      MC_Eff.SetBinContent(i+1, 1)
      MC_Tot.SetBinContent(i+1, 0.1) # eff = 10 so out of range
    if MC_Eff.GetBinContent(i+1) > MC_Tot.GetBinContent(i+1): # den is less than num, due to negative weight
      #print ">>>>>>>>>>>>>>","bin",i+1,": Eff exceeds 1 !!!! <<<<<<<<<<<<<<<<<"
      MC_Eff_exc1.append([i+1,(MC_Eff.GetBinContent(i+1)-MC_Tot.GetBinContent(i+1))/MC_Tot.GetBinContent(i+1)]) # store errors
      MC_Tot.SetBinContent(i+1, MC_Eff.GetBinContent(i+1)) # make SF = 1
  MC_Eff.Divide(MC_Eff,MC_Tot,1,1,"B")
  for i in range(len(MC_Eff_exc1)):
    MC_Eff.SetBinError(MC_Eff_exc1[i][0], MC_Eff_exc1[i][1])

  #for i in range(this_nBins):
  #  print i+1,MC_Eff.GetBinContent(i+1),"+-",MC_Eff.GetBinError(i+1)

  TS=OutName + "_"+ str(n_job) if n_job >=0 else OutName
  
  c1 = TCanvas("c1_"+TS,"",1000,1000)
  c1.cd()

  gStyle.SetPadTickX(1)
  gStyle.SetPadTickY(1)

  c_up = TPad("c_up", "", 0, 0.25, 1, 1)
  c_up.SetTopMargin(0.08)
  c_up.SetBottomMargin(0.017)
  c_up.SetLeftMargin(0.14)
  c_up.SetRightMargin(0.04)
  c_up.SetLogx()
  c_up.Draw()
  c_up.cd()

  c_up_min, c_up_max = GetMinMax(Data_Eff, MC_Eff)
  c_up_min *= 0.9

  Data_Eff.SetTitle("")
  Data_Eff.SetStats(0)
  Data_Eff.GetXaxis().SetLabelSize(0)
  Data_Eff.GetYaxis().SetLabelSize(0.045)
  Data_Eff.GetYaxis().SetTitle("Efficiency")
  Data_Eff.GetYaxis().SetTitleSize(0.075)
  Data_Eff.GetYaxis().SetTitleOffset(0.7)
  #Data_Eff.GetYaxis().SetRangeUser(c_up_min, 1.1)
  Data_Eff.GetYaxis().SetRangeUser(0.8, 1.1) # V3, V7
  #Data_Eff.GetYaxis().SetRangeUser(0.5, 1.1) # V5, V6
  if Probe=="HNLMVA_HighPt" or Probe=="HNLMVA": Data_Eff.GetYaxis().SetRangeUser(0.55, 1.1)
  elif "HNLMVA" in Probe and "No" in Probe:     Data_Eff.GetYaxis().SetRangeUser(0.7, 1.1)
  Data_Eff.SetMarkerStyle(20)
  Data_Eff.SetMarkerColor(kBlack)
  Data_Eff.SetLineColor(kBlack)
  Data_Eff.Draw("ep")
  MC_Eff.SetMarkerStyle(20)
  MC_Eff.SetMarkerColor(kRed)
  MC_Eff.SetLineWidth(1)
  MC_Eff.SetLineColor(kRed)
  MC_Eff.Draw("ep same")

  lg = TLegend(0.3, 0.72, 0.5, 0.87)
  lg.AddEntry(Data_Eff, "Data", "lep")
  lg.AddEntry(MC_Eff, "MC", "lep")
  lg.SetBorderSize(0)
  lg.SetTextSize(0.03)
  lg.SetFillStyle(1001)
  lg.SetShadowColor(0)
  lg.Draw("same")
 
  txt_lumi = TLatex()
  txt_lumi.SetNDC()
  txt_lumi.SetTextSize(0.05)
  txt_lumi.SetTextAlign(32)
  txt_lumi.SetTextFont(42)
  txt_lumi.DrawLatex(.95,.96, luminosity[Era]+" fb^{-1} (13 TeV)")

  IDnames = {
    'HNL_ULID_Split_1'   : 'Trigger Emulation',
    'HNL_ULID_Split_2'   : 'MVA w/o iso Loose',
    'HNL_ULID_Split_3'   : 'IP and SIP',
    'HNL_ULID_Split_4'   : 'MiniIso, NmissHit',
    'HNL_ULID_Split_4b'  : 'TrkIso, NmissHit',
    'HNL_ULID_Split_5'   : 'Medium Charge',
    'HNL_ULID_Split_5b'  : 'Tight Charge',
    'HNL_ULID_Split_6'   : 'CF MVA',
    'HNL_ULID_Split_7'   : 'Fake MVA',
    'HNL_ULID_Split_7b'  : '',
    'HNL_ULID_Split_7c'  : '',
    'HNL_ULID_Split_7d'  : '',
    'HNL_ULID_Split_7e'  : '',
    'HNL_ULID_Split_7f'  : '',
    'HNL_ULID_Split_7g'  : '',
    'HNL_ULID_Split_7h'  : '',
    'HNL_ULID_Split_8'   : 'Conv MVA',
    'HNL_ULID_Split_8b'  : 'Conv MVA w/ low pt',
    'HEEP'               : 'HEEP',
    'HNLMVA'             : 'MVA ID old',
    'HNLMVA_NoCF'        : 'MVA ID (No CF)',
    'HNLMVA_NoConv'      : 'MVA ID (No Conv)',
    'HNLMVA_NoFake'      : 'MVA ID (No Fake)',
    'HNLMVA_HighPt'      : 'MVA ID',
    'HNLMVA_HighPt_Tight': 'MVA ID w/ tighter cut',
    'CutBasedTight94XV2' : 'POG Tight',
    'HNLMVA_TrkIso'      : 'MVA ID w/ TrkIso',
    'HNLHeep'            : 'MVA + HEEP combi.',
    'HNLMVAFake'         : 'MVA Fake',
    'HNLMVACF'           : 'MVA CF',
    'HNLMVAConv'         : 'MVA Conv',
    'MVALoose'           : 'Basic sel. for MVA',
    'MVABaseline'        : 'Sel. before MVA',
    'passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg1' : 'Ele23Leg1',
    'passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg2' : 'Ele12Leg2',
  }
  txt_id = TLatex()
  txt_id.SetNDC()
  txt_id.SetTextSize(0.06)
  txt_id.SetTextAlign(12)
  txt_id.SetTextFont(42)
  txt_id.DrawLatex(.58,.86, IDnames[Probe])

  txt_eta = TLatex()
  txt_eta.SetNDC()
  txt_eta.SetTextSize(0.06)
  txt_eta.SetTextAlign(12)
  txt_eta.SetTextFont(42)
  #txt_eta.DrawLatex(.58,.78, "|#eta| < 1.4442") if EtaRegion=='BB' else txt_eta.DrawLatex(.58,.78, "1.566 < |#eta| < 2.5")
  if EtaRegion=='IB':
    txt_eta.DrawLatex(.58,.78, "|#eta| < 0.8")
  elif EtaRegion=='OB':
    txt_eta.DrawLatex(.58,.78, "0.8 < |#eta| < 1.4442")
  else:
    txt_eta.DrawLatex(.58,.78, "1.566 < |#eta| < 2.5")

  c1.cd()

  c_down = TPad("c_down", "", 0, 0, 1, 0.25)
  c_down.SetTopMargin(0.03)
  c_down.SetBottomMargin(0.35)
  c_down.SetLeftMargin(0.14)
  c_down.SetRightMargin(0.04)
  c_down.SetGridx()
  c_down.SetGridy()
  c_down.SetLogx()
  c_down.Draw()
  c_down.cd()

  Ratio = Data_Eff.Clone()
  Ratio.Divide(MC_Eff)

  #print "Checking SF bins ..."
  #for i in range(Ratio.GetNbinsX()):
  #  print i+1,"th bin:",Data_Eff.GetBinContent(i+1)
  #  print i+1,"th bin:",MC_Eff.GetBinContent(i+1)
  #  print i+1,"th bin:",Ratio.GetBinContent(i+1)

  # Fill h_SF
  # 1D first
  Save['Data_Eff_1D_'+EtaRegion] = Data_Eff.Clone()
  Save['MC_Eff_1D_'+EtaRegion] = MC_Eff.Clone()
  Save['SF_1D_'+EtaRegion] = Ratio.Clone()
  Save['Data_Eff_1D_'+EtaRegion].SetName('Data_Eff_1D_'+EtaRegion+'_'+Probe)
  Save['Data_Eff_1D_'+EtaRegion].SetTitle('Data_Eff_1D_'+EtaRegion+'_'+Probe)
  Save['MC_Eff_1D_'+EtaRegion].SetName('MC_Eff_1D_'+EtaRegion+'_'+Probe)
  Save['MC_Eff_1D_'+EtaRegion].SetTitle('MC_Eff_1D_'+EtaRegion+'_'+Probe)
  Save['SF_1D_'+EtaRegion].SetName('SF_1D_'+EtaRegion+'_'+Probe)
  Save['SF_1D_'+EtaRegion].SetTitle('SF_1D_'+EtaRegion+'_'+Probe)

  # 2D
  Ratio_nBinsX = Ratio.GetNbinsX()
  #for iX in range(Ratio_nBinsX): # Fill GAP with 0
  #  Save['Data_Eff_2D'].SetBinContent(iX+1, 2, 0)
  #  Save['MC_Eff_2D']  .SetBinContent(iX+1, 2, 0)
  #  Save['SF_2D']      .SetBinContent(iX+1, 2, 0)
  #  Save['Data_Eff_2D'].SetBinError  (iX+1, 2, 0)
  #  Save['MC_Eff_2D']  .SetBinError  (iX+1, 2, 0)
  #  Save['SF_2D']      .SetBinError  (iX+1, 2, 0)
  #if EtaRegion == "BB":
  #  for iX in range(Ratio_nBinsX):
  #    Save['Data_Eff_2D'].SetBinContent(iX+1, 1, Data_Eff.GetBinContent(iX+1))
  #    Save['MC_Eff_2D']  .SetBinContent(iX+1, 1, MC_Eff.GetBinContent(iX+1)) 
  #    Save['SF_2D']      .SetBinContent(iX+1, 1, Ratio.GetBinContent(iX+1)) 
  #    Save['Data_Eff_2D'].SetBinError  (iX+1, 1, Data_Eff.GetBinError(iX+1))
  #    Save['MC_Eff_2D']  .SetBinError  (iX+1, 1, MC_Eff.GetBinError(iX+1)) 
  #    Save['SF_2D']      .SetBinError  (iX+1, 1, Ratio.GetBinError(iX+1)) 
  #elif EtaRegion == "EC":
  #  for iX in range(Ratio_nBinsX):
  #    Save['Data_Eff_2D'].SetBinContent(iX+1, 3, Data_Eff.GetBinContent(iX+1))
  #    Save['MC_Eff_2D']  .SetBinContent(iX+1, 3, MC_Eff.GetBinContent(iX+1)) 
  #    Save['SF_2D']      .SetBinContent(iX+1, 3, Ratio.GetBinContent(iX+1)) 
  #    Save['Data_Eff_2D'].SetBinError  (iX+1, 3, Data_Eff.GetBinError(iX+1))
  #    Save['MC_Eff_2D']  .SetBinError  (iX+1, 3, MC_Eff.GetBinError(iX+1)) 
  #    Save['SF_2D']      .SetBinError  (iX+1, 3, Ratio.GetBinError(iX+1)) 
  #  Save['Data_Eff_2D'].SetBinContent(iX+2, 3, Data_Eff.GetBinContent(iX+1)) # EC SF has one less bin than Save (BB+EC), so save the same values
  #  Save['MC_Eff_2D']  .SetBinContent(iX+2, 3, MC_Eff.GetBinContent(iX+1)) 
  #  Save['SF_2D']      .SetBinContent(iX+2, 3, Ratio.GetBinContent(iX+1)) 
  #  Save['Data_Eff_2D'].SetBinError  (iX+2, 3, Data_Eff.GetBinError(iX+1))
  #  Save['MC_Eff_2D']  .SetBinError  (iX+2, 3, MC_Eff.GetBinError(iX+1)) 
  #  Save['SF_2D']      .SetBinError  (iX+2, 3, Ratio.GetBinError(iX+1)) 
  for iX in range(Ratio_nBinsX): # Fill GAP with 0
    Save['Data_Eff_2D'].SetBinContent(iX+1, 3, 0)
    Save['MC_Eff_2D']  .SetBinContent(iX+1, 3, 0)
    Save['SF_2D']      .SetBinContent(iX+1, 3, 0)
    Save['Data_Eff_2D'].SetBinError  (iX+1, 3, 0)
    Save['MC_Eff_2D']  .SetBinError  (iX+1, 3, 0)
    Save['SF_2D']      .SetBinError  (iX+1, 3, 0)
  if EtaRegion == "IB":
    for iX in range(Ratio_nBinsX):
      Save['Data_Eff_2D'].SetBinContent(iX+1, 1, Data_Eff.GetBinContent(iX+1))
      Save['MC_Eff_2D']  .SetBinContent(iX+1, 1, MC_Eff.GetBinContent(iX+1)) 
      Save['SF_2D']      .SetBinContent(iX+1, 1, Ratio.GetBinContent(iX+1)) 
      Save['Data_Eff_2D'].SetBinError  (iX+1, 1, Data_Eff.GetBinError(iX+1))
      Save['MC_Eff_2D']  .SetBinError  (iX+1, 1, MC_Eff.GetBinError(iX+1)) 
      Save['SF_2D']      .SetBinError  (iX+1, 1, Ratio.GetBinError(iX+1)) 
  elif EtaRegion == "OB":
    for iX in range(Ratio_nBinsX):
      Save['Data_Eff_2D'].SetBinContent(iX+1, 2, Data_Eff.GetBinContent(iX+1))
      Save['MC_Eff_2D']  .SetBinContent(iX+1, 2, MC_Eff.GetBinContent(iX+1)) 
      Save['SF_2D']      .SetBinContent(iX+1, 2, Ratio.GetBinContent(iX+1)) 
      Save['Data_Eff_2D'].SetBinError  (iX+1, 2, Data_Eff.GetBinError(iX+1))
      Save['MC_Eff_2D']  .SetBinError  (iX+1, 2, MC_Eff.GetBinError(iX+1)) 
      Save['SF_2D']      .SetBinError  (iX+1, 2, Ratio.GetBinError(iX+1)) 
  elif EtaRegion == "EC":
    for iX in range(Ratio_nBinsX):
      Save['Data_Eff_2D'].SetBinContent(iX+1, 4, Data_Eff.GetBinContent(iX+1))
      Save['MC_Eff_2D']  .SetBinContent(iX+1, 4, MC_Eff.GetBinContent(iX+1)) 
      Save['SF_2D']      .SetBinContent(iX+1, 4, Ratio.GetBinContent(iX+1)) 
      Save['Data_Eff_2D'].SetBinError  (iX+1, 4, Data_Eff.GetBinError(iX+1))
      Save['MC_Eff_2D']  .SetBinError  (iX+1, 4, MC_Eff.GetBinError(iX+1)) 
      Save['SF_2D']      .SetBinError  (iX+1, 4, Ratio.GetBinError(iX+1)) 
    Save['Data_Eff_2D'].SetBinContent(iX+2, 4, Data_Eff.GetBinContent(iX+1)) # EC SF has one less bin than Save, so save the same values
    Save['MC_Eff_2D']  .SetBinContent(iX+2, 4, MC_Eff.GetBinContent(iX+1)) 
    Save['SF_2D']      .SetBinContent(iX+2, 4, Ratio.GetBinContent(iX+1)) 
    Save['Data_Eff_2D'].SetBinError  (iX+2, 4, Data_Eff.GetBinError(iX+1))
    Save['MC_Eff_2D']  .SetBinError  (iX+2, 4, MC_Eff.GetBinError(iX+1)) 
    Save['SF_2D']      .SetBinError  (iX+2, 4, Ratio.GetBinError(iX+1)) 
  else:
    raise ValueError("Unknown EtaRegion: "+EtaRegion)

  # Draw SF
  c_down_min, c_down_max = GetMinMax(Ratio)
  c_down_min = max(0.9, c_down_min*0.95)
  c_down_max = 1.+(1.-c_down_min)

  Ratio.SetTitle("")
  Ratio.SetStats(0)
  Ratio.GetXaxis().SetTitle("p_{T} [GeV]")
  Ratio.GetYaxis().SetTitle("#frac{Data}{MC}")
  #Ratio.GetYaxis().SetRangeUser(c_down_min, c_down_max)
  Ratio.GetYaxis().SetRangeUser(0.9, 1.1)
  if Probe=="HNLMVA_HighPt" or Probe=="HNLMVA": Ratio.GetYaxis().SetRangeUser(0.75, 1.1)
  Ratio.GetXaxis().SetLabelSize(0.12)
  Ratio.GetYaxis().SetLabelSize(0.08)
  Ratio.GetXaxis().SetTitleSize(0.16)
  Ratio.GetYaxis().SetTitleSize(0.14)
  Ratio.GetXaxis().SetTitleOffset(0.9)
  Ratio.GetYaxis().SetTitleOffset(0.4)
  Ratio.SetLineColor(1)
  Ratio.SetMarkerColor(1)
  Ratio.SetMarkerStyle(20)
  Ratio.Draw("ep")

  #lg2 = TLegend(0.75, 0.88, 0.9, 0.95)
  #lg2.AddEntry(Ratio, "Scale factor", "lep")
  #lg2.SetBorderSize(1)
  #lg2.SetTextSize(0.06)
  #lg2.SetFillStyle(1001)
  #lg2.SetShadowColor(0)
  #lg2.Draw("same")

  minRange = Data_Eff.GetBinLowEdge(1)
  maxRange = Data_Eff.GetBinLowEdge(this_nBins) + Data_Eff.GetBinWidth(this_nBins)

  line = TLine(minRange, 1., maxRange, 1.)
  line.SetLineWidth(1)
  line.SetLineColor(2)
  line.Draw()

  c1.SaveAs(WorkDir+"/"+SaveDir+"/"+Era+"/SF"+MassName+"/"+SystName.lstrip('_')+"/"+TS+".png")
  del c1

  return

def SplitChain(_era,_type,_sample,nj):
  List = WorkDir+"/batch_input/"+_era+"_"+_sample+"_"+str(nj)+".txt"
  try: file1 = open(List, "r")  
  except IOError: return []
  else:
    JobFiles=[]
    for x in file1:
      filex=x.split()[0]
      print filex+";"
      JobFiles.append(filex)
    file1.close()
    return JobFiles
  
def MergeFiles(FileList, OutTag):

  output_file = TFile.Open(WorkDir+"/"+"/Merged_"+OutTag+MassName+SystName+".root", "RECREATE")

  first_file = FileList[0]
  keys = first_file.GetListOfKeys()

  for key in keys:
    obj_name = key.GetName()
    obj = first_file.Get(obj_name)

    merged_hist = obj.Clone(obj_name.replace('preVFP',''))

    for other_file in FileList[1:]:
      other_hist = other_file.Get(obj_name.replace('preVFP','postVFP'))
      if other_hist:
        merged_hist.Add(other_hist)

    output_file.cd()
    merged_hist.Write()

  for files in FileList:
    files.Close()
  return output_file

def CreateHists(NthJob):

  pt_bins = np.array([35, 40, 45, 50, 60, 70, 80, 100, 150, 200, 300, 400, 1000], dtype=np.float64)
  nBins = len(pt_bins)-1

  #for era in eras:
  for year, eras in grouped_eras.items():
 
    # Call necessary files and hists first
    #TurnOnFiles = {}
    #TurnOnHists = {}
    #for era in eras:
    #  TurnOnFiles[era] = TFile.Open(WorkDir+"/Out_TurnOn/TurnOn_"+era+".root")
    #  TurnOnHists[era] = TurnOnFiles[era].Get("h_"+era+"_data_HEEP_barrel_Subt")

    OutFile = TFile.Open(WorkDir+"/Out_Eff/SF_"+year+"_"+str(NthJob)+".root","RECREATE")

    nMC = len(samples[year])-1
    mc_chains = [TChain("tnpEleIDs/fitter_tree") for _ in range(nMC)] # Don't use [] * nMC <-- this makes all the items share the same reference
    MC_total_entries = 0

    # Initialize h_mc
    h_mc = {}

    for EtaRegion in It_EtaRegions:
      h_mc[EtaRegion] = {}
      for Charge in It_Charges:
        h_mc[EtaRegion][Charge] = {}
        # ID iteration
        for Probe in It_Probes:
          h_mc[EtaRegion][Charge][Probe] = {}
          for IsPass in It_IsPasses:
            h_mc[EtaRegion][Charge][Probe][IsPass] = TH1D("pt_"+year+"_"+Charge+"_"+EtaRegion+"_"+Probe+"_"+IsPass,"pt_"+year+"_"+Charge+"_"+EtaRegion+"_"+Probe+"_"+IsPass,nBins,pt_bins) if "tot" in Charge else [TH1D("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass,"pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass,nBins,pt_bins) for sample in samples[year][:-1]]
        # All probes
        h_mc[EtaRegion][Charge]['All'] = TH1D("pt_"+year+"_"+Charge+"_"+EtaRegion,"pt_"+year+"_"+Charge+"_"+EtaRegion,nBins,pt_bins) if "tot" in Charge else [TH1D("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge,"pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge,nBins,pt_bins) for sample in samples[year][:-1]] 

    for i, sample in enumerate(samples[year]):

      if i == len(samples[year])-1: continue # skip the data
      if len(SplitChain(year,types[sample],sample,NthJob)) == 0: continue # NthJob out of range of this MC sample
  
      for era in eras:
        t1 = datetime.now()
        for mc_file in SplitChain(era,types[sample],sample,NthJob):
          print ("["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Adding " + mc_file)
          mc_chains[i].Add(mc_file)
        t2 = datetime.now()
        print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
      MC_total_entries += mc_chains[i].GetEntries()

      t1 = datetime.now()
      print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling total entries ..."
      #Nevents = args.Nevents if args.Nevents > 0 else mc_chains[i].GetEntriesFast()
      Nevents = args.Nevents if args.Nevents > 0 else mc_chains[i].GetEntries()
      t2 = datetime.now()
      print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."

      print "Running",Nevents,"events ..."
      
      t1 = datetime.now()
      print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Filling MC OS, SS events ..."
      for entry in range(Nevents):
        mc_chains[i].GetEntry(entry)
       
        # apply era-based turn on, even though the results can be merged
        this_era = mc_chains[i].GetCurrentFile().GetName().split('/')[-1].split('_')[1] # /gv0/DATA/SKFlat/Run2UltraLegacy_v3/2016preVFP/MC_SkimTree_EGammaTnP_HighPt/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/2024_12_03_013251/SKFlatNtuple_2016preVFP_MC_0.root

        #weight_pt = mc_chains[i].tag_Ele_pt_cor
        #if weight_pt >= 200: weight_pt = 199.5
        #TurnOn_Weight = TurnOnHists[this_era].GetBinContent(TurnOnHists[this_era].FindBin(weight_pt)) # let's simulate as if MC tag passed the trigger

        #if entry%10000==0:
        #  print "weight_pt:",mc_chains[i].tag_Ele_pt_cor,"TurnOn:",TurnOn_Weight
        #  print "probe_pt:",mc_chains[i].el_pt_cor

        histinfo = classify_hist(year, sample, mc_chains[i])

        if histinfo is not None:
          EtaRegion, Charges, Probes = histinfo

          this_weight = mc_chains[i].totWeight # Basic
          if "CFSF" in args.Syst:
            if "Up" in args.Syst:
              if mc_chains[i].tag_IsCF:
                this_weight *= ScaleCF
              if mc_chains[i].el_IsCF:
                this_weight *= ScaleCF
            elif "Down" in args.Syst:
              if mc_chains[i].tag_IsCF:
                this_weight *= (2.-ScaleCF)
              if mc_chains[i].el_IsCF:
                this_weight *= (2.-ScaleCF)
            else:
              print "CFSF should be accompanied with Up or Down."
              print "Please use --syst CFSF Up/Down."
              exit()

          for Charge in Charges:
            if "DYJetsToEE" in sample or "DYJetsToLL" in sample:
              if 'os' in Charge or 'zpt' in Charge:
                if "NoZpt" in WorkDir: pass
                elif "ZptGYM" in WorkDir: this_weight *= mc_chains[i].zptweight_gym
                elif "ZptGY" in WorkDir: this_weight *= mc_chains[i].zptweight_gy
                elif "ZptG" in WorkDir: this_weight *= mc_chains[i].zptweight_g
                else: this_weight *= mc_chains[i].zptweight # Zpt
              else: # SS
                if "SSZpt" in WorkDir:
                  if "ZptGYM" in WorkDir: this_weight *= mc_chains[i].zptweight_gym
                  elif "ZptGY" in WorkDir: this_weight *= mc_chains[i].zptweight_gy
                  elif "ZptG" in WorkDir: this_weight *= mc_chains[i].zptweight_g
                  else: this_weight *= mc_chains[i].zptweight # Zpt
                else: pass
            
            #if 'os' in Charge:
            #  this_weight *= TurnOn_Weight # OS TurnOn
            
            # ID iteration
            for ID, isPass in Probes.items():
              h_mc[EtaRegion][Charge][ID][isPass][i].Fill(mc_chains[i].el_pt_cor,this_weight)
              if 'ss' in Charge: h_mc[EtaRegion][Charge+'_tot'][ID][isPass].Fill(mc_chains[i].el_pt_cor,this_weight)
            # All probes
            h_mc[EtaRegion][Charge]['All'][i].Fill(mc_chains[i].el_pt_cor,this_weight)
            if 'ss' in Charge: h_mc[EtaRegion][Charge+'_tot']['All'].Fill(mc_chains[i].el_pt_cor,this_weight)
      t2 = datetime.now()
      print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
  
      OutFile.cd()
      t1 = datetime.now()
      print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Writing hists ..."
      for EtaRegion in It_EtaRegions:
        for Charge in It_Charges:
          if "tot" not in Charge:
            h_mc[EtaRegion][Charge]['All'][i] = add_overflow(h_mc[EtaRegion][Charge]['All'][i])
            h_mc[EtaRegion][Charge]['All'][i].Write()
            for Probe in It_Probes:
              for IsPass in It_IsPasses:
                  h_mc[EtaRegion][Charge][Probe][IsPass][i] = add_overflow(h_mc[EtaRegion][Charge][Probe][IsPass][i])
                  h_mc[EtaRegion][Charge][Probe][IsPass][i].Write()
      t2 = datetime.now()
      print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
    #### Sample iteration done.

    if MC_total_entries != 0:
      OutFile.cd()
      #print "before add_overflow:", h_mc['BB']['ss_tot']['All'].GetBinContent(nBins), h_mc['BB']['ss_tot']['All'].GetBinContent(nBins+1)
      print "before add_overflow:", h_mc['IB']['ss_tot']['All'].GetBinContent(nBins), h_mc['IB']['ss_tot']['All'].GetBinContent(nBins+1)
      for EtaRegion in It_EtaRegions:
        for Charge in It_Charges:
          if "tot" in Charge:
            h_mc[EtaRegion][Charge]['All'] = add_overflow(h_mc[EtaRegion][Charge]['All'])
            h_mc[EtaRegion][Charge]['All'].Write()
            for Probe in It_Probes:
              for IsPass in It_IsPasses:
                  h_mc[EtaRegion][Charge][Probe][IsPass] = add_overflow(h_mc[EtaRegion][Charge][Probe][IsPass])
                  h_mc[EtaRegion][Charge][Probe][IsPass].Write()
      #print "now:",h_mc['BB']['ss_tot']['All'].GetBinContent(nBins)
      print "now:",h_mc['IB']['ss_tot']['All'].GetBinContent(nBins)

    # Data
    data_chain = TChain("tnpEleIDs/fitter_tree")
  
    # now 'sample' is data...
    t1 = datetime.now()
    print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling",year,"data ..."
    for era in eras:
      for period in dates[era][sample]:

        for data_file in SplitChain(era,types[sample],sample+"_"+period,NthJob):
          data_chain.Add(data_file)

          #data_chain.Add("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root")
    t2 = datetime.now()
    print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
  
    if data_chain.GetEntries() == 0:
      OutFile.Close()
      return

    #t1 = datetime.now()
    #print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling total entries ..."
    #print "data entries:",data_chain.GetEntries() # The most time consuming part
    #t2 = datetime.now()
    #print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."

    h_data = {}

    for EtaRegion in It_EtaRegions:
      h_data[EtaRegion] = {}
      for Charge in ["os","ss"]:
        h_data[EtaRegion][Charge] = {}
        # ID iteration
        for Probe in It_Probes:
          h_data[EtaRegion][Charge][Probe] = {}
          for IsPass in It_IsPasses:
            h_data[EtaRegion][Charge][Probe][IsPass] = TH1D("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass,"pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass,nBins,pt_bins)
        # All probes
        h_data[EtaRegion][Charge]['All'] = TH1D("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge,"pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge,nBins,pt_bins)

    # pre-processing for main jobs
    this_draw_command = "el_pt_cor>>pt_"+year+"_"+nameFilter[sample]

    # eta, charge cuts
    this_cuts = {
                 #"BB" : "(fabs(el_sc_eta)<1.4442)",
                 "IB" : "(fabs(el_sc_eta)<0.8)",
                 "OB" : "(0.8<=fabs(el_sc_eta))&&(fabs(el_sc_eta)<1.4442)",
                 "EC" : "(1.566<fabs(el_sc_eta))&&(fabs(el_sc_eta)<2.5)",
                 "os" : "(el_q+tag_Ele_q==0)",
                 "ss" : "(el_q+tag_Ele_q!=0)",
    }
    # pt and mass cuts
    this_cuts['Basic'] = "(35.<el_pt_cor)&&("+str(args.Mass[0])+"<pair_mass_cor)&&(pair_mass_cor<"+str(args.Mass[1])+")"

    #### add ID of N-1 to probe Fail 
    nProbe_data=0
    for Probe in It_Probes:
      ProbeID_data=It_ProbeID[nProbe_data]
      if nProbe_data < NID_Full:
        this_cuts[Probe] = {
          'Pass':"(passing"+Probe+"==1)",
          'Fail':"(passing"+Probe+"==0)",
        }
      else:
        if "Hlt" in Probe:
          this_cuts[Probe] = {
            'Pass':"("+Probe+"==1 && passing"+ProbeID_data+"==1)",
            'Fail':"("+Probe+"==0 && passing"+ProbeID_data+"==1)",
          }
        else:
          this_cuts[Probe] = {
            'Pass':"(passing"+Probe+"==1)",
            'Fail':"(passing"+Probe+"==0 && passing"+ProbeID_data+"==1)",
          }

      nProbe_data=nProbe_data+1

    #### create hists w/ cuts applied
    t1 = datetime.now()
    print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Creating data hists ..."

    for EtaRegion in It_EtaRegions:
      for Charge in ["os","ss"]:
        # ID iteration

        nProbe_data=0 
        for Probe in It_Probes:

          for IsPass in It_IsPasses:
            draw_command = this_draw_command+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass
            cut_condition = this_cuts['Basic']+"&&"+this_cuts[EtaRegion]+"&&"+this_cuts[Charge]+"&&"+this_cuts[Probe][IsPass]
            if args.Nevents > 0:
              cut_condition += "&&Entry$<"+str(args.Nevents)
            print "Now processing: Draw(\""+draw_command+"\",\""+cut_condition+"\")"
            data_chain.Draw(draw_command, cut_condition)
        # All probes
        draw_command = this_draw_command+"_"+EtaRegion+"_"+Charge
        cut_condition = this_cuts['Basic']+"&&"+this_cuts[EtaRegion]+"&&"+this_cuts[Charge]
        if args.Nevents > 0:
          cut_condition += "&&Entry$<"+str(args.Nevents)
        print "Now processing:","Draw(\""+draw_command+"\",\""+cut_condition+"\")"
        data_chain.Draw(draw_command, cut_condition)

    t2 = datetime.now()
    print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
  
    OutFile.cd()

    #### Write data hists
    t1 = datetime.now()
    print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Writing data hists ..."

    for EtaRegion in It_EtaRegions:
      # ID iteration
      for Probe in It_Probes:
        for IsPass in It_IsPasses:
          for Charge in ["os","ss"]:
            # Add overflow
            h_data[EtaRegion][Charge][Probe][IsPass] = add_overflow(h_data[EtaRegion][Charge][Probe][IsPass])
            h_data[EtaRegion][Charge][Probe][IsPass].Write()

      # All probes
      for Charge in ["os","ss"]:
        # Add overflow
        h_data[EtaRegion][Charge]['All'] = add_overflow(h_data[EtaRegion][Charge]['All'])
        h_data[EtaRegion][Charge]['All'].Write()

    #### EtaRegion done.

    t2 = datetime.now()
    print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
    
    OutFile.Close()

  return

def makeResults():

  if args.SumUp:
    return SystSumUp()

  for this_measure in args.Measure:
    os.system('mkdir -p '+WorkDir+"/"+this_measure)

  SaveSF_pt_bins = np.array([35, 40, 45, 50, 60, 70, 80, 100, 150, 200, 300, 1000], dtype=np.float64)
  nBins_SaveSF_pt = len(SaveSF_pt_bins)-1
  #SaveSF_eta_bins = np.array([0, 1.4442, 1.566, 2.5], dtype='d')
  SaveSF_eta_bins = np.array([0, 0.8, 1.4442, 1.566, 2.5], dtype='d')
  nBins_SaveSF_eta = len(SaveSF_eta_bins)-1

  for year, eras in grouped_eras.items():

    if 'DY' in args.Syst:
      HistStackSetting = OrderedDict([
                                      ("DY NLO",     {'Color': kSpring+10, 'Idx': [0]                  }),
                                      ("t#bar{t}",   {'Color': kRed      , 'Idx': [1]                  }),
                                      ("Conversion", {'Color': kViolet   , 'Idx': [2, 3, 4, 5, 6, 7]   }),
                                      ("Diboson",    {'Color': kBlue     , 'Idx': [8, 9, 10]           }),
                                      ("Triboson",   {'Color': kBlue-7   , 'Idx': [11, 12, 13, 14]     }),
                                      ("W#pmW#pm",   {'Color': kYellow   , 'Idx': [15, 16]             }),
                                      ("etc",        {'Color': kPink+10  , 'Idx': [17, 18, 19, 20, 21] }),
      ])
    else:
      HistStackSetting = OrderedDict([
                                      ("DYToEE",     {'Color': kSpring+10, 'Idx': [0]                  }),
                                      ("DYToTauTau", {'Color': kGreen    , 'Idx': [1]                  }),
                                      ("t#bar{t}",   {'Color': kRed      , 'Idx': [2]                  }),
                                      ("Conversion", {'Color': kViolet   , 'Idx': [3, 4, 5, 6, 7, 8]   }),
                                      ("Diboson",    {'Color': kBlue     , 'Idx': [9, 10, 11]          }),
                                      ("Triboson",   {'Color': kBlue-7   , 'Idx': [12, 13, 14, 15]     }),
                                      ("W#pmW#pm",   {'Color': kYellow   , 'Idx': [16, 17]             }),
                                      ("etc",        {'Color': kPink+10  , 'Idx': [18, 19, 20, 21, 22] }),
      ])


    # Call necessary files first
    HistFiles = [] # to merge 2016preVFP and 2016postVFP
    HistFiles_QCDSide = []
    for era in eras: # NOTE here eras are sub-year (of 2016)
      #HistFiles.append(TFile.Open("/data6/Users/jalmond_public/For_Jihun/SF_"+era+".root"))
      #HistFiles.append(TFile.Open("/data6/Users/jalmond_public/For_Jihun/Version3/SF_"+era+".root")) #V3 NLO
      #HistFiles.append(TFile.Open("/data9/Users/jalmond_public/For_Jihun/Version3_NNLO/SF_"+era+".root")) #V3 MiNNLO : Step by step split
      #HistFiles.append(TFile.Open("/data9/Users/jalmond_public/For_Jihun/Version5_NNLO/SF_"+era+".root")) #V5 MiNNLO : MVALoose, IDs on top of MVALoose
      #HistFiles.append(TFile.Open("/data9/Users/jalmond_public/For_Jihun/Version6_NNLO/SF_"+era+".root")) #V6 MiNNLO : same but pt 100 to 150, 150 to 200
      if "Version7" in WorkDir: HistFiles.append(TFile.Open("/data9/Users/jalmond_public/For_Jihun/Version7_split_NNLO/SF_"+era+".root")) #V7 MiNNLO : applied RECO, CF SF
      elif "Version8" in WorkDir: HistFiles.append(TFile.Open("/data6/Users/jihkim/TandPRunlog/Version8_NNLO/2018/TS_2025_01_17_091438__321797____tamsa1/Out_Eff/SF_"+era+".root")) #V8 MiNNLO : change Diboson MC set, add minor MCs, store mcConv, add RECO SF, CF SF, ID SF to tag, And MVALoose, MVABaseline, HNLMVA* are on top of all RECO (250116 inputs)
      elif "Version9" in WorkDir: HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/Version9_split_NNLO/SF_"+era+".root")) #V9: same but 2016, 2017 added and 2018 rerun (250117 inputs)
      elif "Version10" in WorkDir:
        if 'DY' in args.Syst:
          HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/Version10_NNLO_Syst_DY/SF_"+era+".root")) # FIXME V10 DY NLO is in fact V9...! (20250117)
        elif 'CFSF' in args.Syst:
          if 'Up' in args.Syst:
            HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/Version10_NNLO_Syst_CFSF_Up/SF_"+era+".root")) # V10
          elif 'Down' in args.Syst:
            HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/Version10_NNLO_Syst_CFSF_Down/SF_"+era+".root")) # V10
          else:
            print "CFSF should be accompanied with Up or Down."
            print "Please use --syst CFSF Up/Down."
            exit()
        else:
          HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/Version10_NNLO/SF_"+era+".root")) #V10: fix CFSF and RECO SF duplication issue, tag is now HEEP+medium charge (but HEEP SF still), remove probe pt cut in the skim, remove ev.PassTrigger for MC events
      elif "SkimV2" in WorkDir:
        if 'NoZpt' in WorkDir:
          HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/SkimV2_NNLO_NoZpt"+MassName+"/SF_"+era+".root"))
        elif 'SSZptG' in WorkDir:
          HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/SkimV2_NNLO_SSZptG"+MassName+"/SF_"+era+".root"))
        elif 'ZptGYM' in WorkDir:
          HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/SkimV2_NNLO_ZptGYM"+MassName+"/SF_"+era+".root"))
        elif 'ZptGY' in WorkDir:
          HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/SkimV2_NNLO_ZptGY"+MassName+"/SF_"+era+".root"))
        elif 'ZptG' in WorkDir:
          if 'DY' in args.Syst:
            HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/SkimV2_NNLO_ZptG_SplitB"+MassName+"_Syst_DY/SF_"+era+".root"))
          elif 'CFSF' in args.Syst:
            if 'Up' in args.Syst:
              HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/SkimV2_NNLO_ZptG_SplitB"+MassName+"_Syst_CFSF_Up/SF_"+era+".root"))
            elif 'Down' in args.Syst:
              HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/SkimV2_NNLO_ZptG_SplitB"+MassName+"_Syst_CFSF_Down/SF_"+era+".root"))
            else:
              print "CFSF should be accompanied with Up or Down."
              print "Please use --syst CFSF Up/Down."
              exit()
          elif 'QCD' in args.Syst:
            if 'Up' in args.Syst or 'Down' in args.Syst or 'Side' in args.Syst:
              HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/SkimV2_NNLO_ZptG_SplitB"+MassName+"/SF_"+era+".root"))
              HistFiles_QCDSide.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/SkimV2_NNLO_ZptG_SplitB_M110to150/SF_"+era+".root"))
            else:
              print "Possible options with QCD syst: Up/Down/Side."
              print "Exiting ..."
              exit()
          else:
            HistFiles.append(TFile.Open("/data9/Users/jihkim_public/TnPEleHighPt/KinHists/SkimV2_NNLO_ZptG_SplitB"+MassName+"/SF_"+era+".root"))

    # Merge 2016
    if len(eras) > 1:
      this_HistFile = MergeFiles(HistFiles, "2016")
    else: this_HistFile = HistFiles[0]
    if 'QCD' in args.Syst and 'Side' in args.Syst:
      if len(eras) > 1:
        this_HistFile_QCDSide = MergeFiles(HistFiles_QCDSide, "2016")
      else: this_HistFile_QCDSide = HistFiles_QCDSide[0]

    # Call h_mc
    h_mc = {}

    for EtaRegion in It_EtaRegions:
      h_mc[EtaRegion] = {}
      for Charge in ["os","ss_tot"]: # mc actually needs os and ss_tot
        h_mc[EtaRegion][Charge] = {}
        if Charge=="os":
          # ID iteration
          for Probe in It_Probes:
            h_mc[EtaRegion][Charge][Probe] = {}
            for IsPass in It_IsPasses:
              h_mc[EtaRegion][Charge][Probe][IsPass] = merge_lastbins(this_HistFile.Get("pt_"+year+"_"+Charge+"_"+EtaRegion+"_"+Probe+"_"+IsPass)) if "tot" in Charge else [merge_lastbins(this_HistFile.Get("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass)) for sample in samples[year][:-1]]
          # All probes
          h_mc[EtaRegion][Charge]['All'] = merge_lastbins(this_HistFile.Get("pt_"+year+"_"+Charge+"_"+EtaRegion)) if "tot" in Charge else [merge_lastbins(this_HistFile.Get("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge)) for sample in samples[year][:-1]] 
        elif Charge=="ss_tot":
          if 'QCD' in args.Syst and 'Side' in args.Syst: # QCD sideband
            # ID iteration
            for Probe in It_Probes:
              h_mc[EtaRegion][Charge][Probe] = {}
              for IsPass in It_IsPasses:
                h_mc[EtaRegion][Charge][Probe][IsPass] = merge_lastbins(this_HistFile_QCDSide.Get("pt_"+year+"_"+Charge+"_"+EtaRegion+"_"+Probe+"_"+IsPass)) if "tot" in Charge else [merge_lastbins(this_HistFile_QCDSide.Get("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass)) for sample in samples[year][:-1]]
            # All probes
            h_mc[EtaRegion][Charge]['All'] = merge_lastbins(this_HistFile_QCDSide.Get("pt_"+year+"_"+Charge+"_"+EtaRegion)) if "tot" in Charge else [merge_lastbins(this_HistFile_QCDSide.Get("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge)) for sample in samples[year][:-1]] 
          else: # Z peak mass
            # ID iteration
            for Probe in It_Probes:
              h_mc[EtaRegion][Charge][Probe] = {}
              for IsPass in It_IsPasses:
                h_mc[EtaRegion][Charge][Probe][IsPass] = merge_lastbins(this_HistFile.Get("pt_"+year+"_"+Charge+"_"+EtaRegion+"_"+Probe+"_"+IsPass)) if "tot" in Charge else [merge_lastbins(this_HistFile.Get("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass)) for sample in samples[year][:-1]]
            # All probes
            h_mc[EtaRegion][Charge]['All'] = merge_lastbins(this_HistFile.Get("pt_"+year+"_"+Charge+"_"+EtaRegion)) if "tot" in Charge else [merge_lastbins(this_HistFile.Get("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge)) for sample in samples[year][:-1]] 

    IDSFOutFile = TFile.Open(WorkDir+"/ID/SF_"+year+MassName+SystName+".root","RECREATE")
    TrigSFOutFile = TFile.Open(WorkDir+"/Trig/SF_"+year+MassName+SystName+".root","RECREATE")

    # Collect OS MC samples into bundles
    h_Bundle = {}
    for EtaRegion in It_EtaRegions:
      h_Bundle[EtaRegion] = {}
      # ID iteration
      for Probe in It_Probes:
        h_Bundle[EtaRegion][Probe] = {}
        for IsPass in It_IsPasses:
          h_Bundle[EtaRegion][Probe][IsPass] = []
      # All probes
      h_Bundle[EtaRegion]['All'] = []
    
    for EtaRegion in It_EtaRegions:
      for Probe in It_Probes:
        for IsPass in It_IsPasses:
          # Append each representative process
          for hist_setting in HistStackSetting.values():
            h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][hist_setting['Idx'][0]].Clone())
          # Now list complete, do the details
          for hist_iter in range(len(HistStackSetting)):
            h_Bundle[EtaRegion][Probe][IsPass][hist_iter].SetFillColor(HistStackSetting.values()[hist_iter]['Color']) # Set hist color
            # Now add hists under a single, shared name if necessary
            if len(HistStackSetting.values()[hist_iter]['Idx'])==1: continue # single sample under single name --> no need to add
            else:
              for this_idx, hist_idx in enumerate(HistStackSetting.values()[hist_iter]['Idx']):
                if this_idx==0: continue # pass the first process
                h_Bundle[EtaRegion][Probe][IsPass][hist_iter].Add(h_mc[EtaRegion]['os'][Probe][IsPass][hist_idx]) # add the other processes under the shared name

      ## Repeat for all probes
      # Append each representative process
      for hist_setting in HistStackSetting.values():
        h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][hist_setting['Idx'][0]].Clone())
      # Now list complete, do the details
      for hist_iter in range(len(HistStackSetting)):
        h_Bundle[EtaRegion]['All'][hist_iter].SetFillColor(HistStackSetting.values()[hist_iter]['Color']) # Set hist color
        # Now add hists under a single, shared name if necessary
        if len(HistStackSetting.values()[hist_iter]['Idx'])==1: continue # single sample under single name --> no need to add
        else:
          for this_idx, hist_idx in enumerate(HistStackSetting.values()[hist_iter]['Idx']):
            if this_idx==0: continue # pass the first process
            h_Bundle[EtaRegion]['All'][hist_iter].Add(h_mc[EtaRegion]['os']['All'][hist_idx]) # add the other processes under the shared name

    # Call h_data
    h_data = {}

    for EtaRegion in It_EtaRegions:
      h_data[EtaRegion] = {}
      for Charge in ["os","ss"]: # data needs os and ss
        h_data[EtaRegion][Charge] = {}
        if Charge=="os":
          # ID iteration
          for Probe in It_Probes:
            h_data[EtaRegion][Charge][Probe] = {}
            for IsPass in It_IsPasses:
              h_data[EtaRegion][Charge][Probe][IsPass] = merge_lastbins(this_HistFile.Get("pt_"+year+"_data_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass))
          # All probes
          h_data[EtaRegion][Charge]['All'] = merge_lastbins(this_HistFile.Get("pt_"+year+"_data_"+EtaRegion+"_"+Charge))
        elif Charge=="ss":
          if 'QCD' in args.Syst and 'Side' in args.Syst: # QCD sideband
            # ID iteration
            for Probe in It_Probes:
              h_data[EtaRegion][Charge][Probe] = {}
              for IsPass in It_IsPasses:
                h_data[EtaRegion][Charge][Probe][IsPass] = merge_lastbins(this_HistFile_QCDSide.Get("pt_"+year+"_data_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass))
            # All probes
            h_data[EtaRegion][Charge]['All'] = merge_lastbins(this_HistFile_QCDSide.Get("pt_"+year+"_data_"+EtaRegion+"_"+Charge))
          else: # Z peak mass
            # ID iteration
            for Probe in It_Probes:
              h_data[EtaRegion][Charge][Probe] = {}
              for IsPass in It_IsPasses:
                h_data[EtaRegion][Charge][Probe][IsPass] = merge_lastbins(this_HistFile.Get("pt_"+year+"_data_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass))
            # All probes
            h_data[EtaRegion][Charge]['All'] = merge_lastbins(this_HistFile.Get("pt_"+year+"_data_"+EtaRegion+"_"+Charge))
  
    # Now stack OS bundles and get total error
    h_Stack = {}
    h_Error = {}
    for EtaRegion in It_EtaRegions:
      h_Stack[EtaRegion] = {}
      h_Error[EtaRegion] = {}
      # ID iteration
      for Probe in It_Probes:
        h_Stack[EtaRegion][Probe] = {}
        h_Error[EtaRegion][Probe] = {}
        for IsPass in It_IsPasses:
          h_Stack[EtaRegion][Probe][IsPass] = THStack("hs_pt_"+year+"_"+EtaRegion+"_"+Probe+"_"+IsPass,"hs_pt_"+year+"_"+EtaRegion+"_"+Probe+"_"+IsPass)
          h_Error[EtaRegion][Probe][IsPass] = h_Bundle[EtaRegion][Probe][IsPass][0].Clone()
          h_Error[EtaRegion][Probe][IsPass].Reset()
      # All probes
      h_Stack[EtaRegion]['All'] = THStack("hs_pt_"+year+"_"+EtaRegion,"hs_pt_"+year+"_"+EtaRegion)
      h_Error[EtaRegion]['All'] = h_Bundle[EtaRegion]['All'][0].Clone()
      h_Error[EtaRegion]['All'].Reset()

    # Declare Eff, SF hists
    h_SFs = {}

    #### make plots and measure SFs
    t1 = datetime.now()
    print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Measuring SFs ..."

    for EtaRegion in It_EtaRegions:
      # ID iteration
      for Probe in It_Probes:
        if Probe not in h_SFs:
          h_SFs[Probe] = {
                          'Data_Eff_2D' : TH2D("DataEff_2D_"+Probe, "DataEff_2D_"+Probe, nBins_SaveSF_pt, SaveSF_pt_bins, nBins_SaveSF_eta, SaveSF_eta_bins),
                          'MC_Eff_2D'   : TH2D("MCEff_2D_"+Probe, "MCEff_2D_"+Probe, nBins_SaveSF_pt, SaveSF_pt_bins, nBins_SaveSF_eta, SaveSF_eta_bins),
                          'SF_2D'       : TH2D("SF_2D_"+Probe, "SF_2D_"+Probe, nBins_SaveSF_pt, SaveSF_pt_bins, nBins_SaveSF_eta, SaveSF_eta_bins),
          }
        for IsPass in It_IsPasses:

          # SS data - SS prompt = OS fake
          h_data[EtaRegion]['ss'][Probe][IsPass].Add(h_mc[EtaRegion]['ss_tot'][Probe][IsPass],-1)
          if "QCD" in args.Syst:
            if "Up" in args.Syst:
              h_data[EtaRegion]['ss'][Probe][IsPass].Scale(1.25)
            elif "Down" in args.Syst:
              h_data[EtaRegion]['ss'][Probe][IsPass].Scale(0.75)
            elif "Side" in args.Syst: pass
            else:
              print "Possible options with QCD syst: Up/Down/Side."
              print "Exiting ..."
              exit()
          if "NonNeg" in WorkDir:
            for iBin in range(h_data[EtaRegion]['ss'][Probe][IsPass].GetNbinsX()):
              if h_data[EtaRegion]['ss'][Probe][IsPass].GetBinContent(iBin+1) < 0:
                h_data[EtaRegion]['ss'][Probe][IsPass].SetBinContent(iBin+1,0)
                h_data[EtaRegion]['ss'][Probe][IsPass].SetBinError(iBin+1,0)
          h_Bundle[EtaRegion][Probe][IsPass].append(h_data[EtaRegion]['ss'][Probe][IsPass].Clone()) # Fake
          h_Bundle[EtaRegion][Probe][IsPass][-1].SetFillColor(kAzure+1)

          # Now Sum up all bkgs to estimate combined error, and collect bundles into one stack
          for iBundle in reversed(range(len(h_Bundle[EtaRegion][Probe][IsPass]))):
            h_Bundle[EtaRegion][Probe][IsPass][iBundle].SetLineWidth(0)
            h_Error[EtaRegion][Probe][IsPass].Add(h_Bundle[EtaRegion][Probe][IsPass][iBundle])
            h_Stack[EtaRegion][Probe][IsPass].Add(h_Bundle[EtaRegion][Probe][IsPass][iBundle])
  
          print "Making pass/fail plots ..."
          makeCompPlots(h_data[EtaRegion]['os'][Probe][IsPass], h_Stack[EtaRegion][Probe][IsPass], h_Bundle[EtaRegion][Probe][IsPass], h_Error[EtaRegion][Probe][IsPass], year, EtaRegion, Probe, "_"+IsPass, -1, MassName, SystName, HistStackSetting)
        print "Calculating SFs ..."
        if "Hlt" not in Probe:
          measureSFs(h_data[EtaRegion]['os'][Probe], h_Bundle[EtaRegion][Probe], year, EtaRegion, Probe, "", h_SFs[Probe], -1, IDSFOutFile, MassName, SystName)
        else:
          measureSFs(h_data[EtaRegion]['os'][Probe], h_Bundle[EtaRegion][Probe], year, EtaRegion, Probe, "", h_SFs[Probe], -1, TrigSFOutFile, MassName, SystName)

      # SS data - SS prompt = OS fake
      h_data[EtaRegion]['ss']['All'].Add(h_mc[EtaRegion]['ss_tot']['All'],-1) # This is fake
      if "QCD" in args.Syst:
        if "Up" in args.Syst:
          h_data[EtaRegion]['ss']['All'].Scale(1.25)
        elif "Down" in args.Syst:
          h_data[EtaRegion]['ss']['All'].Scale(0.75)
        elif 'Side' in args.Syst: pass
        else:
          print "Possible options with QCD syst: Up/Down/Side."
          print "Exiting ..."
          exit()
      if "NonNeg" in WorkDir:
        for iBin in range(h_data[EtaRegion]['ss']['All'].GetNbinsX()):
          if h_data[EtaRegion]['ss']['All'].GetBinContent(iBin+1) < 0:
            h_data[EtaRegion]['ss']['All'].SetBinContent(iBin+1,0)
            h_data[EtaRegion]['ss']['All'].SetBinError(iBin+1,0)
      h_Bundle[EtaRegion]['All'].append(h_data[EtaRegion]['ss']['All'].Clone()) # Add fake to the bundle
      h_Bundle[EtaRegion]['All'][-1].SetFillColor(kAzure+1)

      # Now Sum up all bkgs to estimate combined error, and collect bundles into one stack
      #print h_Error[EtaRegion]['All'].GetBinContent(1), h_Error[EtaRegion]['All'].GetBinError(1) # to check h_Error was reset successfully
      for iBundle in reversed(range(len(h_Bundle[EtaRegion]['All']))):
        h_Bundle[EtaRegion]['All'][iBundle].SetLineWidth(0)
        h_Error[EtaRegion]['All'].Add(h_Bundle[EtaRegion]['All'][iBundle])
        h_Stack[EtaRegion]['All'].Add(h_Bundle[EtaRegion]['All'][iBundle])
      #print h_Error[EtaRegion]['All'].GetBinContent(1), h_Error[EtaRegion]['All'].GetBinError(1)
  
      print "Making all probes plots ..."
      makeCompPlots(h_data[EtaRegion]['os']['All'], h_Stack[EtaRegion]['All'], h_Bundle[EtaRegion]['All'], h_Error[EtaRegion]['All'], year, EtaRegion, "AllProbes", "",-1,MassName,SystName,HistStackSetting)
    #### EtaRegion done.

    t2 = datetime.now()
    print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
    
    for Probe in h_SFs.keys():
      if "Hlt" not in Probe:
        IDSFOutFile.cd()
        for SF in h_SFs[Probe].keys():
          h_SFs[Probe][SF].Write()
      else:
        TrigSFOutFile.cd()
        for SF in h_SFs[Probe].keys():
          h_SFs[Probe][SF].Write()

    IDSFOutFile.Close()
    TrigSFOutFile.Close()

def SystSumUp():

  SystList = ["_Syst_DY","_Syst_QCD_Side","_Syst_CFSF_Up","_Syst_CFSF_Down","_Syst_QCD_Up","_Syst_QCD_Down"]

  for year in grouped_eras.keys():
    for this_measure in args.Measure:
      print "opening...",WorkDir+"/"+this_measure+"/SF_"+year+"_M70to110.root"
      this_file_nominal = TFile.Open(WorkDir+"/"+this_measure+"/SF_"+year+"_M70to110.root","READ")
      this_file_systs = [TFile.Open(WorkDir+"/"+this_measure+"/SF_"+year+"_M70to110"+this_syst+".root","READ") for this_syst in SystList]
      this_file_out = TFile.Open(WorkDir+"/"+this_measure+"/SF_"+year+"_M70to110_SystCombined.root","RECREATE")

      this_file_out.cd()

      nominal_keys = list(this_file_nominal.GetListOfKeys())
      for this_key in nominal_keys:
        this_obj = this_key.ReadObj()
        this_hist_name = this_obj.GetName()
        this_hist_stat = this_obj.Clone(this_hist_name+"_stat")
        this_hist_stat.SetTitle(this_hist_name+"_stat")

        this_hist_systs = [this_file_syst.Get(this_hist_name) for this_file_syst in this_file_systs]
        #print this_hist_name
        #print this_hist_systs

        if "1D" in this_hist_name:
          for iX in range(1, this_obj.GetNbinsX()+1):
            nom_error = this_obj.GetBinError(iX)
            syst_errors_naive = [abs(this_hist_syst.GetBinContent(iX)-this_obj.GetBinContent(iX)) for this_hist_syst in this_hist_systs]
            syst_errors = []
            syst_errors.append(syst_errors_naive[0]) # DY
            if iX!=this_obj.GetNbinsX(): syst_errors.append(0)
            else: syst_errors.append(syst_errors_naive[1]) # QCD Side only affects the last pt bin
            syst_errors.append(max(syst_errors_naive[2],syst_errors_naive[3])) # CFSF Up/Down
            syst_errors.append(max(syst_errors_naive[4],syst_errors_naive[5])) # QCD Up/Down

            new_error = nom_error**2
            for syst_error in syst_errors:
              new_error += syst_error**2
            new_error = new_error**0.5
            this_obj.SetBinError(iX, new_error)

        elif "2D" in this_hist_name:
          for iX in range(1, this_obj.GetNbinsX()+1):
            for iY in range(1, this_obj.GetNbinsY()+1):
              nom_error = this_obj.GetBinError(iX,iY)
              syst_errors_naive = [abs(this_hist_syst.GetBinContent(iX,iY)-this_obj.GetBinContent(iX,iY)) for this_hist_syst in this_hist_systs]
              syst_errors = []
              syst_errors.append(syst_errors_naive[0]) # DY
              if iX!=this_obj.GetNbinsX(): syst_errors.append(0)
              else: syst_errors.append(syst_errors_naive[1]) # QCD Side only affects the last pt bin
              syst_errors.append(syst_errors_naive[1]) # QCD Side
              syst_errors.append(max(syst_errors_naive[2],syst_errors_naive[3])) # CFSF Up/Down
              syst_errors.append(max(syst_errors_naive[4],syst_errors_naive[5])) # QCD Up/Down

              new_error = nom_error**2
              for syst_error in syst_errors:
                new_error += syst_error**2
              new_error = new_error**0.5
              this_obj.SetBinError(iX, iY, new_error)

        this_hist_stat.Write()
        this_obj.Write("", TObject.kOverwrite) #https://root.cern.ch/doc/master/classTObject.html#aeac9082ad114b6702cb070a8a9f8d2ed : first argument --> save the hist with the original name, second --> overwrite. If kOverwrite not specified, there will be two objects having the same name.

      # Close file
      this_file_nominal.Close()
      for this_file_syst in this_file_systs:
        this_file_syst.Close()
      this_file_out.Close()
      #### Syst combine done.

      # Now draw new 1D plot...

      # Open the combined file
      this_file_out = TFile.Open(WorkDir+"/"+this_measure+"/SF_"+year+"_M70to110_SystCombined.root","READ")

      #for EtaRegion in ["BB", "EC"]:
      for EtaRegion in ["IB", "OB", "EC"]:
        for Probe in It_Probes:
          if Probe=="HNLMVA":
            NameProbe="HNLMVA_Old"
            SaveDir = "ID/Main"
          elif Probe=="HNLMVA_HighPt":
            NameProbe="HNLMVA"
            SaveDir = "ID/Main"
          elif Probe=="MVABaseline":
            NameProbe="MVABaseline"
            SaveDir = "ID/Main"
          elif Probe=="passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg1":
            NameProbe="Ele23Leg1"
            SaveDir = "Trig/Main"
          elif Probe=="passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg2":
            NameProbe="Ele12Leg2"
            SaveDir = "Trig/Main"
          else:
            NameProbe=Probe
            SaveDir = "ID/Support"

          OutName = "SF_Pt_"+str(year)+"_"+EtaRegion+"_"+NameProbe+MassName+"_SystCombined"

          if this_measure not in SaveDir: continue
          #print "getting","Data_Eff_1D_"+EtaRegion+"_"+Probe,"..."
          this_data_eff = this_file_out.Get("Data_Eff_1D_"+EtaRegion+"_"+Probe)
          this_mc_eff = this_file_out.Get("MC_Eff_1D_"+EtaRegion+"_"+Probe)
          this_sf = this_file_out.Get("SF_1D_"+EtaRegion+"_"+Probe)
          this_nBins = this_data_eff.GetNbinsX()

          c1 = TCanvas("c1_"+OutName,"",1000,1000)
          c1.cd()

          gStyle.SetPadTickX(1)
          gStyle.SetPadTickY(1)

          c_up = TPad("c_up", "", 0, 0.25, 1, 1)
          c_up.SetTopMargin(0.08)
          c_up.SetBottomMargin(0.017)
          c_up.SetLeftMargin(0.14)
          c_up.SetRightMargin(0.04)
          c_up.SetLogx()
          c_up.Draw()
          c_up.cd()

          c_up_min, c_up_max = GetMinMax(this_data_eff, this_mc_eff)
          c_up_min *= 0.9

          this_data_eff.SetTitle("")
          this_data_eff.SetStats(0)
          this_data_eff.GetXaxis().SetLabelSize(0)
          this_data_eff.GetYaxis().SetLabelSize(0.045)
          this_data_eff.GetYaxis().SetTitle("Efficiency")
          this_data_eff.GetYaxis().SetTitleSize(0.075)
          this_data_eff.GetYaxis().SetTitleOffset(0.7)
          this_data_eff.GetYaxis().SetRangeUser(0.8, 1.1)
          if Probe=="HNLMVA_HighPt" or Probe=="HNLMVA": this_data_eff.GetYaxis().SetRangeUser(0.55, 1.1)
          elif "HNLMVA" in Probe and "No" in Probe:     this_data_eff.GetYaxis().SetRangeUser(0.7, 1.1)
          this_data_eff.SetMarkerStyle(20)
          this_data_eff.SetMarkerColor(kBlack)
          this_data_eff.SetLineColor(kBlack)
          this_data_eff.Draw("ep")
          this_mc_eff.SetMarkerStyle(20)
          this_mc_eff.SetMarkerColor(kRed)
          this_mc_eff.SetLineWidth(1)
          this_mc_eff.SetLineColor(kRed)
          this_mc_eff.Draw("ep same")

          lg = TLegend(0.3, 0.72, 0.5, 0.87)
          lg.AddEntry(this_data_eff, "Data", "lep")
          lg.AddEntry(this_mc_eff, "MC", "lep")
          lg.SetBorderSize(0)
          lg.SetTextSize(0.03)
          lg.SetFillStyle(1001)
          lg.SetShadowColor(0)
          lg.Draw("same")
 
          txt_lumi = TLatex()
          txt_lumi.SetNDC()
          txt_lumi.SetTextSize(0.05)
          txt_lumi.SetTextAlign(32)
          txt_lumi.SetTextFont(42)
          txt_lumi.DrawLatex(.95,.96, luminosity[year]+" fb^{-1} (13 TeV)")

          IDnames = {
            'HNL_ULID_Split_1'   : 'Trigger Emulation',
            'HNL_ULID_Split_2'   : 'MVA w/o iso Loose',
            'HNL_ULID_Split_3'   : 'IP and SIP',
            'HNL_ULID_Split_4'   : 'MiniIso, NmissHit',
            'HNL_ULID_Split_4b'  : 'TrkIso, NmissHit',
            'HNL_ULID_Split_5'   : 'Medium Charge',
            'HNL_ULID_Split_5b'  : 'Tight Charge',
            'HNL_ULID_Split_6'   : 'CF MVA',
            'HNL_ULID_Split_7'   : 'Fake MVA',
            'HNL_ULID_Split_7b'  : '',
            'HNL_ULID_Split_7c'  : '',
            'HNL_ULID_Split_7d'  : '',
            'HNL_ULID_Split_7e'  : '',
            'HNL_ULID_Split_7f'  : '',
            'HNL_ULID_Split_7g'  : '',
            'HNL_ULID_Split_7h'  : '',
            'HNL_ULID_Split_8'   : 'Conv MVA',
            'HNL_ULID_Split_8b'  : 'Conv MVA w/ low pt',
            'HEEP'               : 'HEEP',
            'HNLMVA'             : 'MVA ID old',
            'HNLMVA_NoCF'        : 'MVA ID (No CF)',
            'HNLMVA_NoConv'      : 'MVA ID (No Conv)',
            'HNLMVA_NoFake'      : 'MVA ID (No Fake)',
            'HNLMVA_HighPt'      : 'MVA ID',
            'HNLMVA_HighPt_Tight': 'MVA ID w/ tighter cut',
            'CutBasedTight94XV2' : 'POG Tight',
            'HNLMVA_TrkIso'      : 'MVA ID w/ TrkIso',
            'HNLHeep'            : 'MVA + HEEP combi.',
            'HNLMVAFake'         : 'MVA Fake',
            'HNLMVACF'           : 'MVA CF',
            'HNLMVAConv'         : 'MVA Conv',
            'MVALoose'           : 'Basic sel. for MVA',
            'MVABaseline'        : 'Sel. before MVA',
            'passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg1' : 'Ele23Leg1',
            'passHltEle23Ele12CaloIdLTrackIdLIsoVLLeg2' : 'Ele12Leg2',
          }
          txt_id = TLatex()
          txt_id.SetNDC()
          txt_id.SetTextSize(0.06)
          txt_id.SetTextAlign(12)
          txt_id.SetTextFont(42)
          txt_id.DrawLatex(.58,.86, IDnames[Probe])

          txt_eta = TLatex()
          txt_eta.SetNDC()
          txt_eta.SetTextSize(0.06)
          txt_eta.SetTextAlign(12)
          txt_eta.SetTextFont(42)
          #txt_eta.DrawLatex(.58,.78, "|#eta| < 1.4442") if EtaRegion=='BB' else txt_eta.DrawLatex(.58,.78, "1.566 < |#eta| < 2.5")
          if EtaRegion=='IB':
            txt_eta.DrawLatex(.58,.78, "|#eta| < 0.8")
          elif EtaRegion=='OB':
            txt_eta.DrawLatex(.58,.78, "0.8 < |#eta| < 1.4442")
          else:
            txt_eta.DrawLatex(.58,.78, "1.566 < |#eta| < 2.5")

          c1.cd()

          c_down = TPad("c_down", "", 0, 0, 1, 0.25)
          c_down.SetTopMargin(0.03)
          c_down.SetBottomMargin(0.35)
          c_down.SetLeftMargin(0.14)
          c_down.SetRightMargin(0.04)
          c_down.SetGridx()
          c_down.SetGridy()
          c_down.SetLogx()
          c_down.Draw()
          c_down.cd()

          c_down_min, c_down_max = GetMinMax(this_sf)
          c_down_min = max(0.9, c_down_min*0.95)
          c_down_max = 1.+(1.-c_down_min)

          this_sf.SetTitle("")
          this_sf.SetStats(0)
          this_sf.GetXaxis().SetTitle("p_{T} [GeV]")
          this_sf.GetYaxis().SetTitle("#frac{Data}{MC}")
          this_sf.GetYaxis().SetRangeUser(0.9, 1.1)
          if Probe=="HNLMVA_HighPt" or Probe=="HNLMVA": this_sf.GetYaxis().SetRangeUser(0.75, 1.1)
          this_sf.GetXaxis().SetLabelSize(0.12)
          this_sf.GetYaxis().SetLabelSize(0.08)
          this_sf.GetXaxis().SetTitleSize(0.16)
          this_sf.GetYaxis().SetTitleSize(0.14)
          this_sf.GetXaxis().SetTitleOffset(0.9)
          this_sf.GetYaxis().SetTitleOffset(0.4)
          this_sf.SetLineColor(1)
          this_sf.SetMarkerColor(1)
          this_sf.SetMarkerStyle(20)
          this_sf.Draw("ep")

          minRange = this_data_eff.GetBinLowEdge(1)
          maxRange = this_data_eff.GetBinLowEdge(this_nBins) + this_data_eff.GetBinWidth(this_nBins)

          line = TLine(minRange, 1., maxRange, 1.)
          line.SetLineWidth(1)
          line.SetLineColor(2)
          line.Draw()

          c1.SaveAs(WorkDir+"/"+SaveDir+"/"+str(year)+"/SF"+MassName+"/"+OutName+".png")
          del c1

if __name__ == '__main__':
  beginTime = datetime.now()
  #makeTurnOn()
  #CreateHists(NJob) # Jobs to be splitted with condor
  makeResults()
  endTime = datetime.now()
  print "["+endTime.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Total done in",endTime-beginTime,"."
