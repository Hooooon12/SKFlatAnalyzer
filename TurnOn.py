import os, sys
import commands as cmd
import argparse
import math
import numpy as np
from ROOT import *
gROOT.SetBatch(kTRUE)

#eras = ["2016preVFP","2016postVFP","2017","2018"]
#eras = ["2018"]
eras = [
  "2016preVFP",
  #"2016postVFP",
  #"2017",
  #"2018",
]
luminosity = {
  '2016preVFP' : '19.5',
  '2016postVFP' : '16.8',
  '2017' : '41.5',
  '2018' : '59.8',
}
samples = {
  '2016preVFP' : [
     "DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8",
     "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
     "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
     "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
     "WW_TuneCP5_13TeV-pythia8",
     "WZ_TuneCP5_13TeV-pythia8",
     "ZZ_TuneCP5_13TeV-pythia8",
     "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "SingleElectron",
  ],
  '2016postVFP' : [
    "DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8",
    "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "WW_TuneCP5_13TeV-pythia8",
    "WZ_TuneCP5_13TeV-pythia8",
    "ZZ_TuneCP5_13TeV-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "SingleElectron",
  ],
  '2018' : [
    "DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8",
    "EGamma",
  ],
}
types = {
  'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : 'MC',
  'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : 'MC',
  'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'MC',
  'WW_TuneCP5_13TeV-pythia8' : 'MC',
  'WZ_TuneCP5_13TeV-pythia8' : 'MC',
  'ZZ_TuneCP5_13TeV-pythia8' : 'MC',
  'SingleElectron' : 'DATA',
  'EGamma' : 'DATA',
}
dates = {
  '2016preVFP':
    {
     'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
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
     'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_12_03_013251',
     },
     'SingleElectron' : {
                         'periodF' : '2024_11_26_102156',
                         'periodG' : '2024_11_26_102156',
                         'periodH' : '2024_11_26_102156',
                        },
    },
  '2018':
    {
     'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : {
                                                             '' : '2024_12_03_013251',
                                                            },
     'EGamma' : {
                 'periodA' : '2024_12_03_013251',
                 'periodB' : '2024_12_03_013251',
                 'periodC' : '2024_12_03_013251',
                 'periodD' : '2024_12_03_013251',
                },
    },
}
triggers = {
            '2016preVFP'  : 'passHltEle27WPTightGsf',
            '2016postVFP' : 'passHltEle27WPTightGsf',
            '2017'        : 'passHltEle32DoubleEGWPTightGsf',
            '2018'        : 'passHltEle32WPTightGsf',
}
nameFilter = {
  'DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8' : 'DYJets',
  'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : 'WJets',
  'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : 'TTLL',
  'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : 'TTLJ',
  'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'tW_top',
  'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : 'tw_antitop',
  'WW_TuneCP5_13TeV-pythia8' : 'WW',
  'WZ_TuneCP5_13TeV-pythia8' : 'WZ',
  'ZZ_TuneCP5_13TeV-pythia8' : 'ZZ',
  'SingleElectron' : 'data',
  'EGamma' : 'data',
}

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

def TurnOn():
  OutFile = TFile.Open("./Out_TurnOn/TurnOn.root","RECREATE")
  for era, sample in [(era, sample) for era in eras for sample in samples[era]]:
    print "Calling",era,sample,"..."
    path_to_add = []
  
    for period in dates[era][sample]:
      path_to_add.append("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root")
    
    this_chain = TChain("tnpEleIDs/fitter_tree")
    for path in path_to_add:
      print "Adding",path,"..."
      this_chain.Add(path)
  
    print this_chain.GetEntries()
  
    outName = era+"_"+nameFilter[sample]
    this_chain.Draw("el_pt_cor>>h_den_Inc(200,0,200)")
    this_chain.Draw("el_pt_cor>>h_den_OS(200,0,200)","el_q+tag_Ele_q==0")
    this_chain.Draw("el_pt_cor>>h_den_SS(200,0,200)","el_q+tag_Ele_q!=0")
    this_chain.Draw("el_pt_cor>>h_den_Subt(200,0,200)","el_q+tag_Ele_q==0")
    this_chain.Draw("el_pt_cor>>"+outName+"_Inc(200,0,200)",triggers[era])
    this_chain.Draw("el_pt_cor>>"+outName+"_OS(200,0,200)",triggers[era]+"&&(el_q+tag_Ele_q==0)")
    this_chain.Draw("el_pt_cor>>"+outName+"_SS(200,0,200)",triggers[era]+"&&(el_q+tag_Ele_q!=0)")
    this_chain.Draw("el_pt_cor>>"+outName+"_Subt(200,0,200)",triggers[era]+"&&(el_q+tag_Ele_q==0)")
    this_chain.Draw("tag_Ele_pt_cor>>h_tag_den_Inc(200,0,200)")
    this_chain.Draw("tag_Ele_pt_cor>>h_tag_den_OS(200,0,200)","el_q+tag_Ele_q==0")
    this_chain.Draw("tag_Ele_pt_cor>>h_tag_den_SS(200,0,200)","el_q+tag_Ele_q!=0")
    this_chain.Draw("tag_Ele_pt_cor>>h_tag_den_Subt(200,0,200)","el_q+tag_Ele_q==0")
    this_chain.Draw("tag_Ele_pt_cor>>tag_"+outName+"_Inc(200,0,200)","tag_"+triggers[era])
    this_chain.Draw("tag_Ele_pt_cor>>tag_"+outName+"_OS(200,0,200)","tag_"+triggers[era]+"&&(el_q+tag_Ele_q==0)")
    this_chain.Draw("tag_Ele_pt_cor>>tag_"+outName+"_SS(200,0,200)","tag_"+triggers[era]+"&&(el_q+tag_Ele_q!=0)")
    this_chain.Draw("tag_Ele_pt_cor>>tag_"+outName+"_Subt(200,0,200)","tag_"+triggers[era]+"&&(el_q+tag_Ele_q==0)")

    if not "data" in outName:
      this_chain.Draw("el_pt_cor>>h_den_Inc_gen(200,0,200)","mcTrue")
      this_chain.Draw("el_pt_cor>>h_den_OS_gen(200,0,200)","(el_q+tag_Ele_q==0)&&mcTrue")
      this_chain.Draw("el_pt_cor>>h_den_SS_gen(200,0,200)","(el_q+tag_Ele_q!=0)&&mcTrue")
      this_chain.Draw("el_pt_cor>>h_den_Subt_gen(200,0,200)","(el_q+tag_Ele_q==0)&&mcTrue")
      this_chain.Draw("el_pt_cor>>"+outName+"_Inc_gen(200,0,200)",triggers[era]+"&&mcTrue")
      this_chain.Draw("el_pt_cor>>"+outName+"_OS_gen(200,0,200)",triggers[era]+"&&(el_q+tag_Ele_q==0)&&mcTrue")
      this_chain.Draw("el_pt_cor>>"+outName+"_SS_gen(200,0,200)",triggers[era]+"&&(el_q+tag_Ele_q!=0)&&mcTrue")
      this_chain.Draw("el_pt_cor>>"+outName+"_Subt_gen(200,0,200)",triggers[era]+"&&(el_q+tag_Ele_q==0)&&mcTrue")
      this_chain.Draw("tag_Ele_pt_cor>>h_tag_den_Inc_gen(200,0,200)","mcTrue")
      this_chain.Draw("tag_Ele_pt_cor>>h_tag_den_OS_gen(200,0,200)","(el_q+tag_Ele_q==0)&&mcTrue")
      this_chain.Draw("tag_Ele_pt_cor>>h_tag_den_SS_gen(200,0,200)","(el_q+tag_Ele_q!=0)&&mcTrue")
      this_chain.Draw("tag_Ele_pt_cor>>h_tag_den_Subt_gen(200,0,200)","(el_q+tag_Ele_q==0)&&mcTrue")
      this_chain.Draw("tag_Ele_pt_cor>>tag_"+outName+"_Inc_gen(200,0,200)","tag_"+triggers[era]+"&&mcTrue")
      this_chain.Draw("tag_Ele_pt_cor>>tag_"+outName+"_OS_gen(200,0,200)","tag_"+triggers[era]+"&&(el_q+tag_Ele_q==0)&&mcTrue")
      this_chain.Draw("tag_Ele_pt_cor>>tag_"+outName+"_SS_gen(200,0,200)","tag_"+triggers[era]+"&&(el_q+tag_Ele_q!=0)&&mcTrue")
      this_chain.Draw("tag_Ele_pt_cor>>tag_"+outName+"_Subt_gen(200,0,200)","tag_"+triggers[era]+"&&(el_q+tag_Ele_q==0)&&mcTrue")
  
    charges = ["Inc","OS","SS","Subt"]
    tags = ["","tag_"]
    denlist = []
    numlist = []
    for tag in tags:
      for charge in charges:
        denlist.append(gDirectory.Get("h_"+tag+"den_"+charge))
        numlist.append(gDirectory.Get(tag+outName+"_"+charge))
        if not "data" in outName:
          denlist.append(gDirectory.Get("h_"+tag+"den_"+charge+"_gen"))
          numlist.append(gDirectory.Get(tag+outName+"_"+charge+"_gen"))
    #print denlist, numlist
    print "last bin:",denlist[0].GetBinContent(200), "overflow bin:",denlist[0].GetBinContent(201)
  
    for i in range(len(denlist)):
      denlist[i] = add_overflow(denlist[i])
      numlist[i] = add_overflow(numlist[i])
    print "now last bin:",denlist[0].GetBinContent(200)
  
    for i in range(len(denlist)):
      if "Subt" in denlist[i].GetName():
        denlist[i].Add(denlist[i-1],-1)
        numlist[i].Add(numlist[i-1],-1)
  
    for i in range(len(denlist)):
      numlist[i].Divide(numlist[i],denlist[i],1,1,"B")
  
    OutFile.cd()
    for i in range(len(denlist)):
      numlist[i].Write()
  
  OutFile.Close()
  return

def makePlots(Data_OS, Stack, Bundle, Error, era):

  c1 = TCanvas("c1","",1000,1000)
  c1.cd()

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

  Error.SetMarkerSize(0)
  Error.SetLineWidth(0)
  Error.SetFillStyle(3144)
  Error.SetFillColor(kBlack)
  Error.Draw("e2 same")

  Data_OS.SetMarkerStyle(20)
  Data_OS.SetMarkerColor(kBlack)
  Data_OS.Draw("ep same")

  lg = TLegend(0.6, 0.45, 0.9, 0.85)
  lg.AddEntry(Error, "Stat. Uncertainty", "f")
  lg.AddEntry(Data_OS, "Data_OS", "lep")
  lg.AddEntry(Bundle[0], "DY", "f")
  lg.AddEntry(Bundle[1], "W", "f")
  lg.AddEntry(Bundle[2], "t#bar{t}", "f")
  lg.AddEntry(Bundle[3], "Diboson", "f")
  lg.AddEntry(Bundle[4], "SingleTop", "f")
  lg.AddEntry(Bundle[5], "Fake", "f")
  lg.SetBorderSize(0)
  lg.SetTextSize(0.03)
  lg.SetFillStyle(1001)
  lg.SetShadowColor(0)
  lg.Draw("same")
 
  txt = TLatex()
  txt.SetNDC()
  txt.SetTextSize(0.05)
  txt.SetTextAlign(32)
  txt.SetTextFont(42)
  txt.DrawLatex(.95,.96, luminosity[era]+" fb^{-1} (13 TeV)")

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
  Error_Stat.GetXaxis().SetTitle("p_{T}")
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

  c1.SaveAs("./Out_TurnOn/TEST.png")
  del c1

  return


def makePtComparison():

  pt_bins = np.array([35, 40, 45, 50, 60, 70, 80, 100, 200, 300, 400, 1000], dtype=np.float64)
  nBins = len(pt_bins)-1
  
  OutFile = TFile.Open("./Out_TurnOn/TEST.root","RECREATE")
  TurnOnFile = TFile.Open("./Out_TurnOn/TurnOn.root")
  
  for era in eras:
    nMC = len(samples[era])-1
    mc_chains = [TChain("tnpEleIDs/fitter_tree") for _ in range(nMC)] # Don't use [] * nMC <-- this makes all the items share the same reference
  
    #h_probe_pt = TH1D("pt_"+era,era+" probe pt",nBins,pt_bins)
    h_mc_os = [TH1D("pt_"+era+"_"+nameFilter[sample]+"_os","pt_"+era+"_"+nameFilter[sample]+"_os",nBins,pt_bins) for sample in samples[era]]
    del h_mc_os[-1] # remove data
    h_mc_ss = [TH1D("pt_"+era+"_"+nameFilter[sample]+"_ss","pt_"+era+"_"+nameFilter[sample]+"_ss",nBins,pt_bins) for sample in samples[era]]
    del h_mc_ss[-1] # remove data
    h_mc_ss_tot = TH1D("pt_"+era+"_MC_ss","pt_"+era+"_MC_ss",nBins,pt_bins)
  
    # Call TurnOn weight
    h_TurnOn = TurnOnFile.Get(era+"_data_Subt")

    for i, sample in enumerate(samples[era]):
  
      if i == len(samples[era])-1: continue # skip the data
  
      print "Adding","/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+dates[era][sample]['']+"/*.root","..."
      mc_chains[i].Add("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+dates[era][sample]['']+"/*.root")
  
      #print mc_chains[i].GetEntries()
  
      #this_draw_command = "el_pt_cor>>pt_"+era+"_"+nameFilter[sample]
      #mc_chains[i].Draw(this_draw_command)
  
      for entry in range(mc_chains[i].GetEntries()):
        mc_chains[i].GetEntry(entry)

        #weight_pt = mc_chains[i].tag_Ele_pt_cor
        weight_pt = mc_chains[i].el_pt_cor
        if weight_pt >= 200: weight_pt = 199.5
        TurnOn_Weight = h_TurnOn.GetBinContent(h_TurnOn.FindBin(weight_pt)) # let's simulate as if MC tag passed the trigger

        #if entry%1000==0:
        #  print "weight_pt:",mc_chains[i].tag_Ele_pt_cor,"TurnOn:",TurnOn_Weight
        #  print "probe_pt:",mc_chains[i].el_pt_cor
        #  print "tag passed trigger?", getattr(mc_chains[i],"tag_"+triggers[era])
        #  print "probe passed trigger?", getattr(mc_chains[i],triggers[era])

        if (mc_chains[i].el_q+mc_chains[i].tag_Ele_q)==0 and mc_chains[i].mcTrue: #OS2l prompt
          h_mc_os[i].Fill(mc_chains[i].el_pt_cor,mc_chains[i].totWeight*TurnOn_Weight)
        elif (mc_chains[i].el_q+mc_chains[i].tag_Ele_q)!=0 and mc_chains[i].mcTrue: #SS2l prompt
          h_mc_ss[i].Fill(mc_chains[i].el_pt_cor,mc_chains[i].totWeight*TurnOn_Weight) # Check each SS2l (study purposes)
          h_mc_ss_tot.Fill(mc_chains[i].el_pt_cor,mc_chains[i].totWeight*TurnOn_Weight) # Add up all SS2l in one
  
      #print h_mc_os[i].GetBinContent(nBins), h_mc_os[i].GetBinError(nBins), h_mc_os[i].GetBinContent(nBins+1)
      h_mc_os[i] = add_overflow(h_mc_os[i])
      h_mc_ss[i] = add_overflow(h_mc_ss[i])
      #print h_mc_os[i].GetBinContent(nBins), h_mc_os[i].GetBinError(nBins), h_mc_os[i].GetBinContent(nBins+1)
      OutFile.cd()
      h_mc_os[i].Write()
      h_mc_ss[i].Write()
    h_mc_ss_tot = add_overflow(h_mc_ss_tot)
    OutFile.cd()
    h_mc_ss_tot.Write()

    # Now collect MCs into bundles
    h_Bundle = []
    
    h_Bundle.append(h_mc_os[0].Clone()) # DY
    h_Bundle.append(h_mc_os[1].Clone()) # WJets
    h_Bundle.append(h_mc_os[2].Clone()) # ttbar
    h_Bundle.append(h_mc_os[4].Clone()) # Diboson
    h_Bundle.append(h_mc_os[7].Clone()) # SingleTop
    h_Bundle[2].Add(h_mc_os[3])
    h_Bundle[3].Add(h_mc_os[5])
    h_Bundle[3].Add(h_mc_os[6])
    h_Bundle[4].Add(h_mc_os[8])
  
    h_Bundle[0].SetFillColor(kSpring+10)
    h_Bundle[1].SetFillColor(kBlue)
    h_Bundle[2].SetFillColor(kYellow)
    h_Bundle[3].SetFillColor(kRed)
    h_Bundle[4].SetFillColor(kViolet)
  
    # Data
    data_chain = TChain("tnpEleIDs/fitter_tree")
  
    # now sample is data...
    for period in dates[era][sample]:
      print "Adding","/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root","..."
      data_chain.Add("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root")
  
    h_data_os = TH1D("pt_"+era+"_"+nameFilter[sample]+"_os","pt_"+era+"_"+nameFilter[sample]+"_os",nBins,pt_bins)
    h_data_ss = TH1D("pt_"+era+"_"+nameFilter[sample]+"_ss","pt_"+era+"_"+nameFilter[sample]+"_ss",nBins,pt_bins)
  
    this_draw_command = "el_pt_cor>>pt_"+era+"_"+nameFilter[sample]
    data_chain.Draw(this_draw_command+"_os","el_q+tag_Ele_q==0")
    data_chain.Draw(this_draw_command+"_ss","el_q+tag_Ele_q!=0")
  
    #print h_data_os.GetBinContent(nBins), h_data_os.GetBinContent(nBins+1)
    h_data_os = add_overflow(h_data_os)
    h_data_ss = add_overflow(h_data_ss)
    #print h_data_os.GetBinContent(nBins), h_data_os.GetBinContent(nBins+1)

    OutFile.cd()
    h_data_os.Write()
    h_data_ss.Write()

    # SS data - SS prompt = OS fake
    h_data_ss.Add(h_mc_ss_tot,-1)
    h_Bundle.append(h_data_ss.Clone()) # Fake
    h_Bundle[5].SetFillColor(kAzure+1)

    # Now Sum up all bkgs to estimate combined error, and collect bundles into one stack
    h_Stack = THStack("hs","")
    h_Error = h_Bundle[0].Clone()
    h_Error.Reset()
    print h_Error.GetBinContent(1), h_Error.GetBinError(1) # to check h_Error was reset successfully
    for iBundle in reversed(range(len(h_Bundle))):
      h_Error.Add(h_Bundle[iBundle])
      h_Bundle[iBundle].SetLineWidth(0)
      h_Stack.Add(h_Bundle[iBundle])
    print h_Error.GetBinContent(1), h_Error.GetBinError(1)
  
    makePlots(h_data_os, h_Stack, h_Bundle, h_Error, era)

  return

if __name__ == '__main__':
  TurnOn()
  #makePtComparison()
