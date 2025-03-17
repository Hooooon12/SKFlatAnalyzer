import os, sys, argparse
import commands as cmd
import argparse
import math
import numpy as np
from datetime import datetime
from ROOT import *
gROOT.SetBatch(kTRUE)

parser = argparse.ArgumentParser(description='Tool for high pt electron SF measurement', formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-n', dest='Nevents', type=int, default=-1, help='Number of events to run; < 0 : full')
parser.add_argument('-t', dest='Time', action='store_true', help='Print running times')
parser.add_argument('-nj', dest='NJob', type=int, default=0, help='Number of job')
#parser.add_argument('-e' , dest='Era', default ='2017')
parser.add_argument('-e' , dest='Eras', default=[], nargs='+')
parser.add_argument('-wd' , dest='WorkDir', default ='./')
parser.add_argument('-ScaleCF', dest='ScaleCF', type=float, default=1, help='SF for CF')


args = parser.parse_args()

ScaleCF=args.ScaleCF
Name_Nevents = "_N"+str(args.Nevents) if args.Nevents > 0 else ""
NJob=args.NJob
#Era = args.Era
WorkDir=args.WorkDir

#It_Probes    = [ 'MVALoose','MVABaseline','HNLMVA','HNLMVA_HighPt', 'HNL_ULID_Split_1','HNL_ULID_Split_2','HNL_ULID_Split_3','HNL_ULID_Split_4','HNL_ULID_Split_4b','HNL_ULID_Split_5','HNL_ULID_Split_5b','HNL_ULID_Split_6','HNL_ULID_Split_7','HNL_ULID_Split_8','HNLMVA_NoCF','HNLMVA_NoConv','HNLMVA_NoFake']
It_Probes    = [ 'HNLMVA_HighPt']
It_ProbeID   = [ 'HNL_ULID_Probe_Split_2']
#### IDs applied to probe befrpre PASS/FAIL

#It_ProbeID   = ['Pass',     'Pass', 'Pass', 'Pass','Pass',  'HNL_ULID_Probe_Split_2','HNL_ULID_Probe_Split_3','HNL_ULID_Probe_Split_4','HNL_ULID_Probe_Split_4','HNL_ULID_Probe_Split_5','HNL_ULID_Probe_Split_5','HNL_ULID_Probe_Split_6','HNL_ULID_Probe_Split_7','HNL_ULID_Probe_Split_8' ,'MVABaseline','MVABaseline','MVABaseline']                                                             

NID_Full= 0

for tmpID in It_ProbeID:
  if tmpID == 'Pass':
    NID_Full=NID_Full+1

#It_ProbeID   = ['HNL_ULID_Split_2',     'HNL_ULID_Split_2', 'HNL_ULID_Split_2', 'HNL_ULID_Split_2','HNL_ULID_Probe_Split_2',       'HNL_ULID_Probe_Split_2','HNL_ULID_Probe_Split_3','HNL_ULID_Probe_Split_4','HNL_ULID_Probe_Split_4','HNL_ULID_Probe_Split_5','HNL_ULID_Probe_Split_5','HNL_ULID_Probe_Split_6','HNL_ULID_Probe_Split_7','HNL_ULID_Probe_Split_8' ,'MVABaseline','MVABaseline','MVABaseline']
It_IsPasses = ['Pass','Fail']
It_EtaRegions = ['BB','EC']
#It_EtaRegions = ['BB']
#It_Charges = ["os","ss","ss_zpt","ss_tot","ss_zpt_tot"]
It_Charges = ["os","ss","ss_tot"]

#eras = [Era]
#grouped_eras = {
#  Era : [Era]
#} # original version

grouped_eras = {}
for Era in args.Eras:
  if Era != "2016":
    grouped_eras[Era] = [Era]
  else:
    grouped_eras[Era] = ["2016preVFP","2016postVFP"]

HEEP_eta_bins = np.array([0, 1.4442, 1.566, 2.5], dtype='d')
nBins_HEEP_eta = len(HEEP_eta_bins)-1

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
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
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
    "GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8",
    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8",
    "WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8",
    
    "SingleElectron",
  ],
  '2016preVFP' : [
    "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
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
    "GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8",
    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8",
    "WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8",
     "SingleElectron",
  ],
  '2016postVFP' : [
    "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
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
    "GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8",
    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8",
    "WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8",
    "SingleElectron",
  ],
  '2017' : [
    "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
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
    "GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8",
    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8",
    "WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8",
    "SingleElectron",
  ],
  '2018' : [
    "DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
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
    "GluGluToContinToZZTo4e_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2mu_TuneCP5_13TeV-mcfm701-pythia8",
    "GluGluToContinToZZTo2e2tau_TuneCP5_13TeV-mcfm701-pythia8",
    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "WpWpJJ_QCDnotop_TuneCP5_13TeV-madgraph-pythia8",
    "WpWpJJ_EWKnotop_TuneCP5_13TeV-madgraph-pythia8",
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

types = {
  'DYJetsToEE_M-50_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos'  : 'MC',
  'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_massWgtFix_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : 'MC',
  'DYJetsToTauTau_M-50_AtLeastOneEorMuDecay_TuneCP5_13TeV-powhegMiNNLO-pythia8-photos' : 'MC',
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

  if 'BB' in this_name:
    return hist

  #pt_bins_merged = np.array([35, 40, 45, 50, 60, 70, 80, 100, 200, 1000], dtype=np.float64)
  #pt_bins_merged = np.array([35, 40, 45, 50, 60, 70, 80, 100, 200, 300, 1000], dtype=np.float64) # up to V5 and V7
  pt_bins_merged = np.array([35, 40, 45, 50, 60, 70, 80, 100, 150, 200, 300, 1000], dtype=np.float64) # V6, V8

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
  for year, eras in grouped_eras.items():

    OutFile = TFile.Open(WorkDir+"/Out_TurnOn/TurnOn_"+year+".root","RECREATE")

    for sample in samples[year]:

      t1 = datetime.now()
      print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling",year,sample,"..."
      path_to_add = []
      this_chain = TChain("tnpEleIDs/fitter_tree")

      for era in eras:
        for period in dates[era][sample]:
          path_to_add.append("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root")
        
      for path in path_to_add:
        print "Adding",path,"..."
        this_chain.Add(path)
      t2 = datetime.now()
      print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
    
      #t1 = datetime.now()
      #print "["+t1.strftime("%Y-%m-%d %H:%M:%S")+"]","Calling total entries ..."
      #print "Total",this_chain.GetEntries(),"events." # The most time consuming part
      #t2 = datetime.now()
      #print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
    
      outName = year+"_"+nameFilter[sample]

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
                    outName+"_" : "&&"+triggers[year],
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
    
      OutFile.cd()
      for i in range(len(denlist)):
        numlist[i].Write()
    
    OutFile.Close()

  return

def classify_hist(this_year, this_sample, this_chain):

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
  if abs(this_chain.el_sc_eta) < 1.4442:
    EtaRegion = 'BB'
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

    Probes[this_probe] = 'Pass' if getattr(this_chain, "passing"+this_probe) else 'Fail'
    #for attr in dir(this_chain):
    #  if attr.startswith('passing'):
    #    ID = attr.replace('passing','')
    #    Probes[ID] = 'Pass' if getattr(this_chain, attr) else 'Fail' # use this to include every IDs

  return EtaRegion, Charges, Probes

def makeCompPlots(Data_OS, Stack, Bundle, Error, Era, Name, n_job):

  for i in range(Bundle[-1].GetNbinsX()):
    if Bundle[-1].GetBinContent(i+1) <= 0:
      print "[ERROR] In",Name," bin",i+1,":",Bundle[-1].GetBinContent(i+1)

  os.system('mkdir -p '+WorkDir+"/Out_SF/"+Era+"/Comp")

  TS=Era+Name+"_"+ str(n_job) if n_job >=0 else Era+Name

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
  #lg.AddEntry(Bundle[0], "DY", "f") # NLO
  lg.AddEntry(Bundle[0], "DYtoEE", "f") # MiNNLO
  lg.AddEntry(Bundle[1], "DYtoTauTau", "f") # MiNNLO
  lg.AddEntry(Bundle[2], "t#bar{t}", "f")
  lg.AddEntry(Bundle[3], "SingleTop", "f")
  lg.AddEntry(Bundle[4], "Conversion", "f")
  lg.AddEntry(Bundle[5], "Diboson", "f")
  lg.AddEntry(Bundle[6], "Triboson", "f")
  lg.AddEntry(Bundle[8], "W#pmW#pm", "f")
  lg.AddEntry(Bundle[7], "etc", "f")
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

  c1.SaveAs(WorkDir+"/Out_SF/"+Era+"/Comp/Pt_"+TS+".png")
  del c1

  return

def measureSFs(Data_OS, Bundle, Era, EtaRegion, Probe, Tag, Save, n_job, OutFile):

  os.system('mkdir -p '+WorkDir+"/Out_SF/"+Era+"/SF")

  OutName = "SF_Pt_"+Era+"_"+EtaRegion+"_"+Probe+Tag

  this_nBins = Data_OS['Pass'].GetNbinsX()
  this_Bins = Data_OS['Pass'].GetXaxis().GetXbins().GetArray()

  for i in range(len(Bundle['Pass'])):
    if i==0 : continue # skip DY
    Data_OS['Pass'].Add(Bundle['Pass'][i],-1)
    Data_OS['Fail'].Add(Bundle['Fail'][i],-1)

  Data_Eff = Data_OS['Pass'].Clone()
  Data_Tot = Data_OS['Pass'].Clone()
  Data_Tot.Add(Data_OS['Fail'])

  # Save data integral before divide
  combined_data_pass = Data_Eff.Integral()
  combined_data_tot = Data_Tot.Integral()
  combined_data_eff = combined_data_pass/combined_data_tot

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

  # Save MC integral before divide
  combined_mc_pass = MC_Eff.Integral()
  combined_mc_tot = MC_Tot.Integral()
  combined_mc_eff = combined_mc_pass/combined_mc_tot

  # Get the combined SF
  combined_sf = combined_data_eff/combined_mc_eff

  #print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  #print "Running", OutName, "..."
  #print "Combined data efficiency:", combined_data_eff
  #print "Combined mc efficiency:", combined_mc_eff
  #print "Combined scale factor:", combined_sf # FIXME add error later
  #print "Saving into", OutFile.GetName(), "..." # FIXME add error later
  #print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

  # Fill Gap
  Save['Data_Eff'].SetBinContent(2, 0)
  Save['MC_Eff']  .SetBinContent(2, 0)
  Save['SF']      .SetBinContent(2, 0)
  if EtaRegion == "BB":
    Save['Data_Eff'].SetBinContent(1, combined_data_eff)
    Save['MC_Eff']  .SetBinContent(1, combined_mc_eff)
    Save['SF']      .SetBinContent(1, combined_sf)
  elif EtaRegion == "EC":
    Save['Data_Eff'].SetBinContent(3, combined_data_eff)
    Save['MC_Eff']  .SetBinContent(3, combined_mc_eff)
    Save['SF']      .SetBinContent(3, combined_sf)
  else:
    raise ValueError("Unknown EtaRegion: "+EtaRegion)

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
    'HNLMVA'             : 'MVA ID',
    'HNLMVA_NoCF'        : 'MVA ID (No CF)',
    'HNLMVA_NoConv'      : 'MVA ID (No Conv)',
    'HNLMVA_NoFake'      : 'MVA ID (No Fake)',
    'HNLMVA_HighPt'      : 'MVA ID w/ fix',
    'HNLMVA_HighPt_Tight': 'MVA ID w/ tighter fix',
    'CutBasedTight94XV2' : 'POG Tight',
    'HNLMVA_TrkIso'      : 'MVA ID w/ TrkIso',
    'HNLHeep'            : 'MVA + HEEP combi.',
    'HNLMVAFake'         : 'MVA Fake',
    'HNLMVACF'           : 'MVA CF',
    'HNLMVAConv'         : 'MVA Conv',
    'MVALoose'           : 'Basic sel. for MVA',
    'MVABaseline'        : 'Sel. before MVA',
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
  txt_eta.DrawLatex(.58,.78, "|#eta| < 1.4442") if EtaRegion=='BB' else txt_eta.DrawLatex(.58,.78, "1.566 < |#eta| < 2.5")

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
  #  print i+1,"th bin:",Ratio.GetBinContent(i+1)

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

  c1.SaveAs(WorkDir+"/Out_SF/"+Era+"/SF/"+TS+".png")
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

  output_file = TFile.Open(WorkDir+"/Out_SF/Merged_"+OutTag+".root", "RECREATE")

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
    TurnOnFiles = {}
    TurnOnHists = {}
    for era in eras:
      TurnOnFiles[era] = TFile.Open(WorkDir+"/Out_TurnOn/TurnOn_"+era+".root")
      TurnOnHists[era] = TurnOnFiles[era].Get("h_"+era+"_data_HEEP_barrel_Subt")

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
      if len(SplitChain(era,types[sample],sample,NthJob)) == 0: continue # NthJob out of range of this MC sample
  
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

        weight_pt = mc_chains[i].tag_Ele_pt_cor
        if weight_pt >= 200: weight_pt = 199.5
        TurnOn_Weight = TurnOnHists[this_era].GetBinContent(TurnOnHists[this_era].FindBin(weight_pt)) # let's simulate as if MC tag passed the trigger

        if entry%10000==0:
          print "weight_pt:",mc_chains[i].tag_Ele_pt_cor,"TurnOn:",TurnOn_Weight
          print "probe_pt:",mc_chains[i].el_pt_cor

        histinfo = classify_hist(year, sample, mc_chains[i])

        if histinfo is not None:
          EtaRegion, Charges, Probes = histinfo

          this_weight = mc_chains[i].totWeight # Basic

          for Charge in Charges:
            if 'os' in Charge or 'zpt' in Charge:
              this_weight *= mc_chains[i].zptweight # Zpt
            if 'os' in Charge:
              this_weight *= TurnOn_Weight # OS TurnOn
            
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
      print "before add_overflow:", h_mc['BB']['ss_tot']['All'].GetBinContent(nBins), h_mc['BB']['ss_tot']['All'].GetBinContent(nBins+1)
      for EtaRegion in It_EtaRegions:
        for Charge in It_Charges:
          if "tot" in Charge:
            h_mc[EtaRegion][Charge]['All'] = add_overflow(h_mc[EtaRegion][Charge]['All'])
            h_mc[EtaRegion][Charge]['All'].Write()
            for Probe in It_Probes:
              for IsPass in It_IsPasses:
                  h_mc[EtaRegion][Charge][Probe][IsPass] = add_overflow(h_mc[EtaRegion][Charge][Probe][IsPass])
                  h_mc[EtaRegion][Charge][Probe][IsPass].Write()
      print "now:",h_mc['BB']['ss_tot']['All'].GetBinContent(nBins)

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
    this_cuts = {
                 "BB" : "(fabs(el_sc_eta)<1.4442)",
                 "EC" : "(1.566<fabs(el_sc_eta))&&(fabs(el_sc_eta)<2.5)",
                 "os" : "(el_q+tag_Ele_q==0)",
                 "ss" : "(el_q+tag_Ele_q!=0)",
    }


    #### add ID of N-1 to probe Fail 
    nProbe_data=0
    for Probe in It_Probes:
      ProbeID_data=It_ProbeID[nProbe_data]
      # JA
      if nProbe_data < NID_Full:
        this_cuts[Probe] = {
          'Pass':"(passing"+Probe+"==1)",
          'Fail':"(passing"+Probe+"==0)",
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
            cut_condition = this_cuts[EtaRegion]+"&&"+this_cuts[Charge]+"&&"+this_cuts[Probe][IsPass]
            if args.Nevents > 0:
              cut_condition += "&&Entry$<"+str(args.Nevents)
            print "Now processing: Draw(\""+draw_command+"\",\""+cut_condition+"\")"
            data_chain.Draw(draw_command, cut_condition)
        # All probes
        draw_command = this_draw_command+"_"+EtaRegion+"_"+Charge
        cut_condition = this_cuts[EtaRegion]+"&&"+this_cuts[Charge]
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
  os.system("mkdir -p "+WorkDir+"/Out_SF/")

  for year, eras in grouped_eras.items():

    # Call necessary files first
    HistFiles = []
    for era in eras:
      #HistFiles.append(TFile.Open("/data6/Users/jalmond_public/For_Jihun/SF_"+era+".root"))
      #HistFiles.append(TFile.Open("/data6/Users/jalmond_public/For_Jihun/Version3/SF_"+era+".root")) #V3 NLO
      #HistFiles.append(TFile.Open("/data9/Users/jalmond_public/For_Jihun/Version3_NNLO/SF_"+era+".root")) #V3 MiNNLO : Step by step split
      #HistFiles.append(TFile.Open("/data9/Users/jalmond_public/For_Jihun/Version5_NNLO/SF_"+era+".root")) #V5 MiNNLO : MVALoose, IDs on top of MVALoose
      #HistFiles.append(TFile.Open("/data9/Users/jalmond_public/For_Jihun/Version6_NNLO/SF_"+era+".root")) #V6 MiNNLO : same but pt 100 to 150, 150 to 200
      if "Version7" in WorkDir: HistFiles.append(TFile.Open("/data9/Users/jalmond_public/For_Jihun/Version7_split_NNLO/SF_"+era+".root")) #V7 MiNNLO : applied RECO, CF SF
      elif "Version8" in WorkDir: HistFiles.append(TFile.Open("/data6/Users/jihkim/TandPRunlog/Version8_NNLO/2018/TS_2025_01_17_091438__321797____tamsa1/Out_Eff/SF_"+era+".root")) #V8 MiNNLO : change Diboson MC set, add minor MCs, store mcConv

    # Merge 2016
    if len(eras) > 1:
      this_HistFile = MergeFiles(HistFiles, "2016")
    else: this_HistFile = HistFiles[0]

    # Call h_mc
    h_mc = {}

    for EtaRegion in It_EtaRegions:
      h_mc[EtaRegion] = {}
      for Charge in ["os","ss_tot"]: # mc actually needs os and ss_tot
        h_mc[EtaRegion][Charge] = {}
        # ID iteration
        for Probe in It_Probes:
          h_mc[EtaRegion][Charge][Probe] = {}
          for IsPass in It_IsPasses:
            #h_mc[EtaRegion][Charge][Probe][IsPass] = this_HistFile.Get("pt_"+year+"_"+Charge+"_"+EtaRegion+"_"+Probe+"_"+IsPass) if "tot" in Charge else [this_HistFile.Get("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass) for sample in samples[year][:-1]]
            h_mc[EtaRegion][Charge][Probe][IsPass] = merge_lastbins(this_HistFile.Get("pt_"+year+"_"+Charge+"_"+EtaRegion+"_"+Probe+"_"+IsPass)) if "tot" in Charge else [merge_lastbins(this_HistFile.Get("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass)) for sample in samples[year][:-1]]
        # All probes
        #h_mc[EtaRegion][Charge]['All'] = this_HistFile.Get("pt_"+year+"_"+Charge+"_"+EtaRegion) if "tot" in Charge else [this_HistFile.Get("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge) for sample in samples[year][:-1]] 
        h_mc[EtaRegion][Charge]['All'] = merge_lastbins(this_HistFile.Get("pt_"+year+"_"+Charge+"_"+EtaRegion)) if "tot" in Charge else [merge_lastbins(this_HistFile.Get("pt_"+year+"_"+nameFilter[sample]+"_"+EtaRegion+"_"+Charge)) for sample in samples[year][:-1]] 

    OutFile = TFile.Open(WorkDir+"/Out_SF/SF_"+year+".root","RECREATE")

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
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][0].Clone()) # DY or DYtoEE (MiNNLO)
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][1].Clone()) # DYtoTauTau (MiNNLO)
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][2].Clone()) # TTLL (TT)
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][4].Clone()) # tW top (single top)
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][6].Clone()) # WG (conv family)
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][12].Clone()) # ZZ (VV)
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][15].Clone()) # WZZ (VVV)
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][19].Clone()) # gg (minors)
          h_Bundle[EtaRegion][Probe][IsPass].append(h_mc[EtaRegion]['os'][Probe][IsPass][24].Clone()) # WpWpQCD

          h_Bundle[EtaRegion][Probe][IsPass][2].Add(h_mc[EtaRegion]['os'][Probe][IsPass][3]) # TTLJ
          h_Bundle[EtaRegion][Probe][IsPass][3].Add(h_mc[EtaRegion]['os'][Probe][IsPass][5]) # tW antitop
          h_Bundle[EtaRegion][Probe][IsPass][4].Add(h_mc[EtaRegion]['os'][Probe][IsPass][7]) # ZG
          h_Bundle[EtaRegion][Probe][IsPass][4].Add(h_mc[EtaRegion]['os'][Probe][IsPass][8]) # TTG
          h_Bundle[EtaRegion][Probe][IsPass][4].Add(h_mc[EtaRegion]['os'][Probe][IsPass][9]) # TG
          h_Bundle[EtaRegion][Probe][IsPass][4].Add(h_mc[EtaRegion]['os'][Probe][IsPass][10]) # WZG
          h_Bundle[EtaRegion][Probe][IsPass][4].Add(h_mc[EtaRegion]['os'][Probe][IsPass][11]) # WWG
          h_Bundle[EtaRegion][Probe][IsPass][5].Add(h_mc[EtaRegion]['os'][Probe][IsPass][13]) # WW
          h_Bundle[EtaRegion][Probe][IsPass][5].Add(h_mc[EtaRegion]['os'][Probe][IsPass][14]) # WZ
          h_Bundle[EtaRegion][Probe][IsPass][6].Add(h_mc[EtaRegion]['os'][Probe][IsPass][16]) # ZZZ
          h_Bundle[EtaRegion][Probe][IsPass][6].Add(h_mc[EtaRegion]['os'][Probe][IsPass][17]) # WWZ
          h_Bundle[EtaRegion][Probe][IsPass][6].Add(h_mc[EtaRegion]['os'][Probe][IsPass][18]) # WWW
          h_Bundle[EtaRegion][Probe][IsPass][7].Add(h_mc[EtaRegion]['os'][Probe][IsPass][20]) # 
          h_Bundle[EtaRegion][Probe][IsPass][7].Add(h_mc[EtaRegion]['os'][Probe][IsPass][21]) # 
          h_Bundle[EtaRegion][Probe][IsPass][7].Add(h_mc[EtaRegion]['os'][Probe][IsPass][22]) # 
          h_Bundle[EtaRegion][Probe][IsPass][7].Add(h_mc[EtaRegion]['os'][Probe][IsPass][23]) # 
          h_Bundle[EtaRegion][Probe][IsPass][8].Add(h_mc[EtaRegion]['os'][Probe][IsPass][25]) # WpWpEWK
  
          h_Bundle[EtaRegion][Probe][IsPass][0].SetFillColor(kSpring+10) # DY or DYtoEE (MiNNLO)
          h_Bundle[EtaRegion][Probe][IsPass][1].SetFillColor(kGreen) # DYToTauTau (MiNNLO)
          h_Bundle[EtaRegion][Probe][IsPass][2].SetFillColor(kRed)
          h_Bundle[EtaRegion][Probe][IsPass][3].SetFillColor(kOrange-3)
          h_Bundle[EtaRegion][Probe][IsPass][4].SetFillColor(kViolet)
          h_Bundle[EtaRegion][Probe][IsPass][5].SetFillColor(kBlue)
          h_Bundle[EtaRegion][Probe][IsPass][6].SetFillColor(kBlue+2)
          h_Bundle[EtaRegion][Probe][IsPass][7].SetFillColor(kYellow)
          h_Bundle[EtaRegion][Probe][IsPass][8].SetFillColor(kPink)

      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][0].Clone()) 
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][1].Clone()) 
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][2].Clone()) 
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][4].Clone()) 
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][6].Clone()) 
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][12].Clone())
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][15].Clone())
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][19].Clone())
      h_Bundle[EtaRegion]['All'].append(h_mc[EtaRegion]['os']['All'][24].Clone())

      h_Bundle[EtaRegion]['All'][2].Add(h_mc[EtaRegion]['os']['All'][3]) # TTLJ
      h_Bundle[EtaRegion]['All'][3].Add(h_mc[EtaRegion]['os']['All'][5]) # tW antitop
      h_Bundle[EtaRegion]['All'][4].Add(h_mc[EtaRegion]['os']['All'][7]) # ZG
      h_Bundle[EtaRegion]['All'][4].Add(h_mc[EtaRegion]['os']['All'][8]) # TTG
      h_Bundle[EtaRegion]['All'][4].Add(h_mc[EtaRegion]['os']['All'][9]) # TG
      h_Bundle[EtaRegion]['All'][4].Add(h_mc[EtaRegion]['os']['All'][10]) # WZG
      h_Bundle[EtaRegion]['All'][4].Add(h_mc[EtaRegion]['os']['All'][11]) # WWG
      h_Bundle[EtaRegion]['All'][5].Add(h_mc[EtaRegion]['os']['All'][13]) # WW
      h_Bundle[EtaRegion]['All'][5].Add(h_mc[EtaRegion]['os']['All'][14]) # WZ
      h_Bundle[EtaRegion]['All'][6].Add(h_mc[EtaRegion]['os']['All'][16]) # ZZZ
      h_Bundle[EtaRegion]['All'][6].Add(h_mc[EtaRegion]['os']['All'][17]) # WWZ
      h_Bundle[EtaRegion]['All'][6].Add(h_mc[EtaRegion]['os']['All'][18]) # WWW
      h_Bundle[EtaRegion]['All'][7].Add(h_mc[EtaRegion]['os']['All'][20]) # 
      h_Bundle[EtaRegion]['All'][7].Add(h_mc[EtaRegion]['os']['All'][21]) # 
      h_Bundle[EtaRegion]['All'][7].Add(h_mc[EtaRegion]['os']['All'][22]) # 
      h_Bundle[EtaRegion]['All'][7].Add(h_mc[EtaRegion]['os']['All'][23]) # 
      h_Bundle[EtaRegion]['All'][8].Add(h_mc[EtaRegion]['os']['All'][25]) # WpWpEWK
  
      h_Bundle[EtaRegion]['All'][0].SetFillColor(kSpring+10) # DY or DYtoEE (MiNNLO)
      h_Bundle[EtaRegion]['All'][1].SetFillColor(kGreen) # DYToTauTau (MiNNLO)
      h_Bundle[EtaRegion]['All'][2].SetFillColor(kRed)
      h_Bundle[EtaRegion]['All'][3].SetFillColor(kOrange-3)
      h_Bundle[EtaRegion]['All'][4].SetFillColor(kViolet)
      h_Bundle[EtaRegion]['All'][5].SetFillColor(kBlue)
      h_Bundle[EtaRegion]['All'][6].SetFillColor(kBlue+2)
      h_Bundle[EtaRegion]['All'][7].SetFillColor(kYellow)
      h_Bundle[EtaRegion]['All'][8].SetFillColor(kPink)

    # Call h_data
    h_data = {}

    for EtaRegion in It_EtaRegions:
      h_data[EtaRegion] = {}
      for Charge in ["os","ss"]: # data needs os and ss
        h_data[EtaRegion][Charge] = {}
        # ID iteration
        for Probe in It_Probes:
          h_data[EtaRegion][Charge][Probe] = {}
          for IsPass in It_IsPasses:
            #h_data[EtaRegion][Charge][Probe][IsPass] = this_HistFile.Get("pt_"+year+"_data_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass)
            h_data[EtaRegion][Charge][Probe][IsPass] = merge_lastbins(this_HistFile.Get("pt_"+year+"_data_"+EtaRegion+"_"+Charge+"_"+Probe+"_"+IsPass))
        # All probes
        #h_data[EtaRegion][Charge]['All'] = this_HistFile.Get("pt_"+year+"_data_"+EtaRegion+"_"+Charge)
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

    OutFile.cd()

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
                          'Data_Eff' : TH1D("DataEff_"+Probe, "DataEff_"+Probe, nBins_HEEP_eta, HEEP_eta_bins),
                          'MC_Eff'   : TH1D("MCEff_"+Probe, "MCEff_"+Probe, nBins_HEEP_eta, HEEP_eta_bins),
                          'SF'       : TH1D("SF_"+Probe, "SF_"+Probe, nBins_HEEP_eta, HEEP_eta_bins),
          }
        for IsPass in It_IsPasses:

          # SS data - SS prompt = OS fake
          h_data[EtaRegion]['ss'][Probe][IsPass].Add(h_mc[EtaRegion]['ss_tot'][Probe][IsPass],-1)
          h_Bundle[EtaRegion][Probe][IsPass].append(h_data[EtaRegion]['ss'][Probe][IsPass].Clone()) # Fake
          h_Bundle[EtaRegion][Probe][IsPass][-1].SetFillColor(kAzure+1)

          # Now Sum up all bkgs to estimate combined error, and collect bundles into one stack
          for iBundle in reversed(range(len(h_Bundle[EtaRegion][Probe][IsPass]))):
            h_Bundle[EtaRegion][Probe][IsPass][iBundle].SetLineWidth(0)
            h_Error[EtaRegion][Probe][IsPass].Add(h_Bundle[EtaRegion][Probe][IsPass][iBundle])
            h_Stack[EtaRegion][Probe][IsPass].Add(h_Bundle[EtaRegion][Probe][IsPass][iBundle])
  
          print "Making pass/fail plots ..."
          makeCompPlots(h_data[EtaRegion]['os'][Probe][IsPass], h_Stack[EtaRegion][Probe][IsPass], h_Bundle[EtaRegion][Probe][IsPass], h_Error[EtaRegion][Probe][IsPass], year, "_"+EtaRegion+"_"+Probe+"_"+IsPass+Name_Nevents,-1)
        print "Calculating SFs ..."
        measureSFs(h_data[EtaRegion]['os'][Probe], h_Bundle[EtaRegion][Probe], year, EtaRegion, Probe, Name_Nevents, h_SFs[Probe], -1, OutFile)

      # SS data - SS prompt = OS fake
      h_data[EtaRegion]['ss']['All'].Add(h_mc[EtaRegion]['ss_tot']['All'],-1) # This is fake
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
      makeCompPlots(h_data[EtaRegion]['os']['All'], h_Stack[EtaRegion]['All'], h_Bundle[EtaRegion]['All'], h_Error[EtaRegion]['All'], year, "_"+EtaRegion+"_AllProbes"+Name_Nevents,-1)
    #### EtaRegion done.

    t2 = datetime.now()
    print "["+t2.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Done in",t2-t1,"."
    
    for Probe in h_SFs.keys():
      for SF in h_SFs[Probe].keys():
        h_SFs[Probe][SF].Write()

    OutFile.Close()


if __name__ == '__main__':
  beginTime = datetime.now()
  #makeTurnOn()
  CreateHists(NJob) # Jobs to be splitted with condor
  #makeResults()
  endTime = datetime.now()
  print "["+endTime.now().strftime("%Y-%m-%d %H:%M:%S")+"]","Total done in",endTime-beginTime,"."
