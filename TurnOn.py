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
     "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
     "WW_TuneCP5_13TeV-pythia8",
     "WZ_TuneCP5_13TeV-pythia8",
     "ZZ_TuneCP5_13TeV-pythia8",
     "SingleElectron",
  ],
  '2016postVFP' : [
    "DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8",
    "WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8",
    "WW_TuneCP5_13TeV-pythia8",
    "WZ_TuneCP5_13TeV-pythia8",
    "ZZ_TuneCP5_13TeV-pythia8",
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
                                                             '' : '2024_11_26_172311',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_11_26_172311',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_11_26_172311',
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
                                                             '' : '2024_11_26_172311',
                                                            },
     'WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8' : {
                                                             '' : '2024_11_26_172311',
                                                            },
     'TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'WW_TuneCP5_13TeV-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'WZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_11_26_172311',
     },
     'ZZ_TuneCP5_13TeV-pythia8' : {
     '' : '2024_11_26_172311',
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
                                                             '' : '2024_11_26_172311',
                                                            },
     'EGamma' : {
                 'periodA' : '2024_11_26_172311',
                 'periodB' : '2024_11_26_172311',
                 'periodC' : '2024_11_26_172311',
                 'periodD' : '2024_11_26_172311',
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
  OutFile = TFile.Open("./TurnOn.root","RECREATE")
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

    if not "data" in outName:
      this_chain.Draw("el_pt_cor>>h_den_Inc_gen(200,0,200)","mcTrue")
      this_chain.Draw("el_pt_cor>>h_den_OS_gen(200,0,200)","(el_q+tag_Ele_q==0)&&mcTrue")
      this_chain.Draw("el_pt_cor>>h_den_SS_gen(200,0,200)","(el_q+tag_Ele_q!=0)&&mcTrue")
      this_chain.Draw("el_pt_cor>>h_den_Subt_gen(200,0,200)","(el_q+tag_Ele_q==0)&&mcTrue")
      this_chain.Draw("el_pt_cor>>"+outName+"_Inc_gen(200,0,200)",triggers[era]+"&&mcTrue")
      this_chain.Draw("el_pt_cor>>"+outName+"_OS_gen(200,0,200)",triggers[era]+"&&(el_q+tag_Ele_q==0)&&mcTrue")
      this_chain.Draw("el_pt_cor>>"+outName+"_SS_gen(200,0,200)",triggers[era]+"&&(el_q+tag_Ele_q!=0)&&mcTrue")
      this_chain.Draw("el_pt_cor>>"+outName+"_Subt_gen(200,0,200)",triggers[era]+"&&(el_q+tag_Ele_q==0)&&mcTrue")
  
    charges = ["Inc","OS","SS","Subt"]
    denlist = []
    numlist = []
    for charge in charges:
      denlist.append(gDirectory.Get("h_den_"+charge))
      numlist.append(gDirectory.Get(outName+"_"+charge))
      if not "data" in outName:
        denlist.append(gDirectory.Get("h_den_"+charge+"_gen"))
        numlist.append(gDirectory.Get(outName+"_"+charge+"_gen"))
    #print denlist, numlist
    print denlist[0].GetBinContent(200), denlist[0].GetBinContent(201)
  
    for i in range(len(denlist)):
      denlist[i] = add_overflow(denlist[i])
      numlist[i] = add_overflow(numlist[i])
    print denlist[0].GetBinContent(200)
  
    denlist[-1].Add(denlist[2],-1)
    numlist[-1].Add(numlist[2],-1)
  
    for i in range(len(denlist)):
      numlist[i].Divide(numlist[i],denlist[i],1,1,"B")
  
    OutFile.cd()
    for i in range(len(denlist)):
      numlist[i].Write()
  
  OutFile.Close()
  return

def makePlots(Data, MC_Stack, MC_Bundle, MC_Error, era):

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

  MC_Stack.Draw("hist")
  MC_Stack.SetTitle("")
  MC_Stack.GetXaxis().SetLabelSize(0)
  MC_Stack.GetYaxis().SetLabelSize(0.045)
  MC_Stack.GetYaxis().SetTitle("Events")
  MC_Stack.GetYaxis().SetTitleSize(0.075)
  MC_Stack.GetYaxis().SetTitleOffset(0.8)

  MC_Error.SetMarkerSize(0)
  MC_Error.SetLineWidth(0)
  MC_Error.SetFillStyle(3144)
  MC_Error.SetFillColor(kBlack)
  MC_Error.Draw("e2 same")

  Data.SetMarkerStyle(20)
  Data.SetMarkerColor(kBlack)
  Data.Draw("ep same")

  lg = TLegend(0.6, 0.45, 0.9, 0.85)
  lg.AddEntry(MC_Error, "Stat. Uncertainty", "f")
  lg.AddEntry(Data, "Data", "lep")
  lg.AddEntry(MC_Bundle[0], "DY", "f")
  lg.AddEntry(MC_Bundle[1], "W", "f")
  lg.AddEntry(MC_Bundle[2], "t#bar{t}", "f")
  lg.AddEntry(MC_Bundle[3], "SingleTop", "f")
  lg.AddEntry(MC_Bundle[4], "Diboson", "f")
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

  this_nBins = Data.GetNbinsX()

  MC_Error_Stat = MC_Error.Clone()
  for iBin in range(this_nBins):
    binContent = MC_Error_Stat.GetBinContent(iBin)
    binError = MC_Error_Stat.GetBinError(iBin)
    if binContent != 0.:
      binError = binError/binContent
    else:
      binError = 0.
    MC_Error_Stat.SetBinContent(iBin, 1.)
    MC_Error_Stat.SetBinError(iBin, binError)

  print "Error of ratio:"
  for iBin in range(this_nBins):
    print MC_Error_Stat.GetBinError(iBin)

  MC_Error_Stat.SetTitle("")
  MC_Error_Stat.SetStats(0)
  MC_Error_Stat.GetXaxis().SetTitle("p_{T}")
  MC_Error_Stat.GetYaxis().SetTitle("#frac{Obs.}{Pred.}")
  #MC_Error_Stat.GetXaxis().SetRange(minBinNumber, maxBinNumber)
  MC_Error_Stat.GetYaxis().SetRangeUser(0.5, 1.5)
  MC_Error_Stat.GetXaxis().SetLabelSize(0.12)
  MC_Error_Stat.GetYaxis().SetLabelSize(0.08)
  MC_Error_Stat.GetXaxis().SetTitleSize(0.16)
  MC_Error_Stat.GetYaxis().SetTitleSize(0.14)
  MC_Error_Stat.GetXaxis().SetTitleOffset(0.9)
  MC_Error_Stat.GetYaxis().SetTitleOffset(0.4)

  MC_Error_Stat.SetMarkerSize(0)
  MC_Error_Stat.SetLineWidth(0)
  MC_Error_Stat.SetFillStyle(1001)
  MC_Error_Stat.SetFillColor(kGray)
  MC_Error_Stat.Draw("e2")

  Ratio = Data.Clone()
  Ratio.Divide(MC_Error)
  Ratio.SetLineColor(1)
  Ratio.SetMarkerColor(1)
  Ratio.SetMarkerStyle(20)
  Ratio.Draw("ep same")

  lg2 = TLegend(0.75, 0.88, 0.9, 0.95)
  lg2.SetNColumns(2)
  lg2.AddEntry(MC_Error_Stat, "Stat. Uncert.", "f")
  lg2.SetBorderSize(1)
  lg2.SetTextSize(0.06)
  lg2.SetFillStyle(1001)
  lg2.SetShadowColor(0)
  lg2.Draw("same")

  minRange = Data.GetBinLowEdge(1)
  maxRange = Data.GetBinLowEdge(this_nBins) + Data.GetBinWidth(this_nBins)

  line = TLine(minRange, 1., maxRange, 1.)
  line.SetLineWidth(1)
  line.SetLineColor(2)
  line.Draw()

  c1.SaveAs("TEST.png")
  del c1

  return


def makePtComparison():

  pt_bins = np.array([35, 40, 45, 50, 60, 70, 80, 100, 200, 300, 400, 1000], dtype=np.float64)
  nBins = len(pt_bins)-1
  
  OutFile = TFile.Open("./TEST.root","RECREATE")
  
  for era in eras:
    nMC = len(samples[era])-1
    mc_chains = [TChain("tnpEleIDs/fitter_tree") for _ in range(nMC)] # Don't use [] * nMC <-- this makes all the items share the same reference
  
    #h_probe_pt = TH1D("pt_"+era,era+" probe pt",nBins,pt_bins)
    h_mc = [TH1D("pt_"+era+"_"+nameFilter[sample],"pt_"+era+"_"+nameFilter[sample],nBins,pt_bins) for sample in samples[era]]
    del h_mc[-1] # remove data
  
    h_Bundle = []
    for i, sample in enumerate(samples[era]):
  
      if i == len(samples[era])-1: continue # skip the data
  
      print "Adding","/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+dates[era][sample]['']+"/*.root","..."
      mc_chains[i].Add("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+dates[era][sample]['']+"/*.root")
  
      #print mc_chains[i].GetEntries()
  
      #this_draw_command = "el_pt_cor>>pt_"+era+"_"+nameFilter[sample]
      #mc_chains[i].Draw(this_draw_command)
  
      for entry in range(mc_chains[i].GetEntries()):
        mc_chains[i].GetEntry(entry)
        if (mc_chains[i].el_q+mc_chains[i].tag_Ele_q)==0:
          h_mc[i].Fill(mc_chains[i].el_pt_cor,mc_chains[i].totWeight)
  
      #print h_mc[i].GetBinContent(nBins), h_mc[i].GetBinError(nBins), h_mc[i].GetBinContent(nBins+1)
      h_mc[i] = add_overflow(h_mc[i])
      #print h_mc[i].GetBinContent(nBins), h_mc[i].GetBinError(nBins), h_mc[i].GetBinContent(nBins+1)
      OutFile.cd()
      h_mc[i].Write()
  
    # Now stack the hist
    h_Stack = THStack("hs","")
    
    h_Bundle.append(h_mc[0].Clone()) # DY
    h_Bundle.append(h_mc[1].Clone()) # WJets
    h_Bundle.append(h_mc[2].Clone()) # ttbar
    h_Bundle.append(h_mc[4].Clone()) # SingleTop
    h_Bundle.append(h_mc[6].Clone()) # Diboson
    h_Bundle[2].Add(h_mc[3])
    h_Bundle[3].Add(h_mc[5])
    h_Bundle[4].Add(h_mc[7])
    h_Bundle[4].Add(h_mc[8])
  
    h_Bundle[0].SetFillColor(kSpring+10)
    h_Bundle[1].SetFillColor(kBlue)
    h_Bundle[2].SetFillColor(kYellow)
    h_Bundle[3].SetFillColor(kRed)
    h_Bundle[4].SetFillColor(kViolet)
  
    h_Error = h_Bundle[0].Clone()
    h_Error.Reset()
    print h_Error.GetBinContent(1), h_Error.GetBinError(1)
    for iBundle in reversed(range(len(h_Bundle))):
      h_Error.Add(h_Bundle[iBundle])
      h_Bundle[iBundle].SetLineWidth(0)
      h_Stack.Add(h_Bundle[iBundle])
    print h_Error.GetBinContent(1), h_Error.GetBinError(1)
  
    # Data
    data_chain = TChain("tnpEleIDs/fitter_tree")
  
    for period in dates[era][sample]:
      print "Adding","/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root","..."
      data_chain.Add("/gv0/DATA/SKFlat/Run2UltraLegacy_v3/"+era+"/"+types[sample]+"_SkimTree_EGammaTnP_HighPt/"+sample+"/"+period+"/"+dates[era][sample][period]+"/*.root")
  
    h_data = TH1D("pt_"+era+"_"+nameFilter[sample],"pt_"+era+"_"+nameFilter[sample],nBins,pt_bins)
  
    this_draw_command = "el_pt_cor>>pt_"+era+"_"+nameFilter[sample]
    data_chain.Draw(this_draw_command)
  
    #print h_data.GetBinContent(nBins), h_data.GetBinContent(nBins+1)
    h_data = add_overflow(h_data)
    #print h_data.GetBinContent(nBins), h_data.GetBinContent(nBins+1)
    OutFile.cd()
    h_data.Write()
  
    makePlots(h_data, h_Stack, h_Bundle, h_Error, era)

  return

if __name__ == '__main__':
  TurnOn()
  #makePtComparison()
