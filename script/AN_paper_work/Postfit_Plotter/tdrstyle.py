import ROOT

def setTDRStyle():

    ROOT.gStyle.SetCanvasBorderMode(0)
    ROOT.gStyle.SetCanvasColor(ROOT.kWhite)
    ROOT.gStyle.SetCanvasDefH(800)
    ROOT.gStyle.SetCanvasDefW(800)

    ROOT.gStyle.SetPadBorderMode(0)
    ROOT.gStyle.SetPadColor(ROOT.kWhite)
    ROOT.gStyle.SetPadGridX(False)
    ROOT.gStyle.SetPadGridY(False)

    ROOT.gStyle.SetFrameBorderMode(0)
    ROOT.gStyle.SetFrameBorderSize(1)
    ROOT.gStyle.SetFrameFillColor(0)
    ROOT.gStyle.SetFrameFillStyle(0)
    ROOT.gStyle.SetFrameLineColor(1)
    ROOT.gStyle.SetFrameLineStyle(1)
    ROOT.gStyle.SetFrameLineWidth(1)

    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetOptTitle(0)

    ROOT.gStyle.SetPadTopMargin(0.08)
    ROOT.gStyle.SetPadBottomMargin(0.13)
    ROOT.gStyle.SetPadLeftMargin(0.14)
    ROOT.gStyle.SetPadRightMargin(0.04)

    ROOT.gStyle.SetTitleFont(42, "XYZ")
    ROOT.gStyle.SetLabelFont(42, "XYZ")

    ROOT.gStyle.SetTitleSize(0.05, "XYZ")
    ROOT.gStyle.SetLabelSize(0.04, "XYZ")

    ROOT.gStyle.SetTitleOffset(1.2, "X")
    ROOT.gStyle.SetTitleOffset(1.4, "Y")

    ROOT.gStyle.SetMarkerStyle(20)
    ROOT.gStyle.SetMarkerSize(1.0)

    ROOT.gStyle.SetHistLineWidth(2)

    ROOT.gStyle.SetEndErrorSize(2)

    ROOT.gStyle.SetTickLength(0.03, "X")
    ROOT.gStyle.SetTickLength(0.03, "Y")

    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)

    ROOT.gStyle.SetLegendBorderSize(0)

    ROOT.gROOT.ForceStyle()
