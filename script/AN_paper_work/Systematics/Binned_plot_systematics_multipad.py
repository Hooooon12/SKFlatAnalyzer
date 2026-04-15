#!/usr/bin/env python3

import os
import math
import re
import ROOT

ROOT.gROOT.SetBatch(True)

# ---------------- Style ----------------
def setTDRStyle():
    tdrStyle = ROOT.TStyle("tdrStyle","Style for P-TDR")
    tdrStyle.SetPaintTextFormat("4.4f")
    tdrStyle.SetCanvasBorderMode(0)
    tdrStyle.SetCanvasColor(ROOT.kWhite)
    tdrStyle.SetCanvasDefH(1200)
    tdrStyle.SetCanvasDefW(1600)
    tdrStyle.SetPadBorderMode(0)
    tdrStyle.SetPadColor(ROOT.kWhite)
    tdrStyle.SetHistLineColor(1)
    tdrStyle.SetMarkerStyle(20)
    tdrStyle.SetOptFile(0)
    tdrStyle.SetOptStat(0)
    tdrStyle.SetPadTopMargin(0.05)
    tdrStyle.SetPadBottomMargin(0.13)
    tdrStyle.SetPadLeftMargin(0.16)
    tdrStyle.SetPadRightMargin(0.06)
    tdrStyle.SetOptTitle(0)
    tdrStyle.SetTitleFont(42)
    tdrStyle.SetTitleSize(0.05)
    tdrStyle.SetLabelFont(42, "XYZ")
    tdrStyle.SetLabelSize(0.05, "XYZ")
    tdrStyle.SetTitleFont(42, "XYZ")
    tdrStyle.SetTitleSize(0.06)
    tdrStyle.SetTitleXOffset(0.9)
    tdrStyle.SetTitleYOffset(1.25)
    tdrStyle.SetPadTickX(1)
    tdrStyle.SetPadTickY(1)
    tdrStyle.cd()
    ROOT.gROOT.ForceStyle()
    ROOT.gStyle.SetCanvasPreferGL(False)

# -------------- Helpers ---------------
def file_ok(f):
    return bool(f) and not f.IsZombie()

def clean_bin(v):
    if v is None:
        return 0.0
    v = float(v)
    return v if math.isfinite(v) else 0.0

def set_band_fill_pdf_safe(graph):
    graph.SetFillColor(ROOT.kGray)
    graph.SetFillStyle(1001)  # solid, PDF-safe
    graph.SetLineColor(ROOT.kGray+2)

# -------------- IO / math -------------
def sum_bkg_hists_per_sr(file_path, bkg_names, syst_key):
    f = ROOT.TFile.Open(file_path, "READ")
    if not file_ok(f):
        return None, None, None

    h_nom_sum = None
    h_up_sum  = None
    h_dn_sum  = None

    for bkg in bkg_names:
        h_nom = f.Get(bkg)
        h_up  = f.Get(f"{bkg}_CMS_{syst_key}Up")
        h_dn  = f.Get(f"{bkg}_CMS_{syst_key}Down")
        if not h_nom or not h_up or not h_dn:
            continue

        if h_nom_sum is None:
            h_nom_sum = h_nom.Clone("nom_sum")
            h_nom_sum.SetDirectory(0)
            h_nom_sum.Reset()
            h_up_sum  = h_up.Clone("up_sum")
            h_up_sum.SetDirectory(0)
            h_up_sum.Reset()
            h_dn_sum  = h_dn.Clone("dn_sum")
            h_dn_sum.SetDirectory(0)
            h_dn_sum.Reset()

        use_nominal_for_var = (abs(clean_bin(h_up.Integral()) - 0.001) < 1e-12)

        for ib in range(1, h_nom_sum.GetNbinsX()+1):
            n  = clean_bin(h_nom.GetBinContent(ib))
            uu = clean_bin(h_up.GetBinContent(ib))
            dd = clean_bin(h_dn.GetBinContent(ib))
            h_nom_sum.SetBinContent(ib, h_nom_sum.GetBinContent(ib) + n)
            if use_nominal_for_var:
                h_up_sum.SetBinContent(ib, h_up_sum.GetBinContent(ib) + n)
                h_dn_sum.SetBinContent(ib, h_dn_sum.GetBinContent(ib) + n)
            else:
                h_up_sum.SetBinContent(ib, h_up_sum.GetBinContent(ib) + uu)
                h_dn_sum.SetBinContent(ib, h_dn_sum.GetBinContent(ib) + dd)

    f.Close()
    return h_nom_sum, h_up_sum, h_dn_sum

def perbin_ratios_from_sum_hists(h_nom, h_up, h_dn):
    if not h_nom or not h_up or not h_dn:
        return [1.0], [1.0]
    nb = h_nom.GetNbinsX()
    up_vals, dn_vals = [], []
    for ib in range(1, nb+1):
        n = clean_bin(h_nom.GetBinContent(ib))
        u = clean_bin(h_up.GetBinContent(ib))
        d = clean_bin(h_dn.GetBinContent(ib))
        if n == 0.0:
            up_vals.append(1.0)
            dn_vals.append(1.0)
        else:
            up_vals.append(u / n)
            dn_vals.append(d / n)
    return up_vals, dn_vals

def compute_perbin_ratios_for_sr(base_dir, analysis_version, year, flavour, sr, syst_key):
    regions = ["sr1", "sr2", "sr3"]
    assert sr in regions
    bkgs = ["cf","conv_others","fake","prompt_others","ww","wz","zg","zz"]
    signal = "1000"
    path = f"{base_dir}/{analysis_version}/{year}/{sr}/M{signal}_{flavour}_card_input.root"
    h_nom, h_up, h_dn = sum_bkg_hists_per_sr(path, bkgs, syst_key)
    return perbin_ratios_from_sum_hists(h_nom, h_up, h_dn)

# -------------- Drawing (bins) --------
def draw_flavour_panel_bins(year, syst_label, flavour, up_vals, dn_vals, ymin, ymax, draw_legend=False):
    nbins = len(up_vals)
    hbase = ROOT.TH1F(f"hbase_{year}_{syst_label}_{flavour}",
                      "Systematic Uncertainty", nbins, 0.5, nbins + 0.5)
    for i in range(1, nbins+1):
        hbase.SetBinContent(i, 1.0)
        hbase.GetXaxis().SetBinLabel(i, str(i))
    hbase.SetLineColor(ROOT.kBlack)
    hbase.SetLineStyle(2)
    hbase.GetYaxis().SetTitle("Syst / Nominal")
    hbase.SetMinimum(ymin)
    hbase.SetMaximum(ymax)

    hbase.GetXaxis().SetLabelSize(0.07)   # bin label font size
    hbase.GetXaxis().SetTitleSize(0.06)   # title font size
    hbase.GetXaxis().SetTitleOffset(1.2)  # move title away if needed
    
    # Y axis
    #hbase.GetYaxis().SetLabelSize(0.05)
    #hbase.GetYaxis().SetTitleSize(0.06)
    #hbase.GetYaxis().SetTitleOffset(1.4)
    
    gband = ROOT.TGraphAsymmErrors(nbins)
    gband.SetName(f"gband_{year}_{syst_label}_{flavour}")
    for i in range(nbins):
        x = i + 1.0
        y_up = up_vals[i]
        y_dn = dn_vals[i]
        y_mid = 0.5 * (y_up + y_dn)
        err_up = max(0.0, y_up - y_mid)
        err_dn = max(0.0, y_mid - y_dn)
        gband.SetPoint(i, x, y_mid)
        gband.SetPointError(i, 0.45, 0.45, err_dn, err_up)
    set_band_fill_pdf_safe(gband)

    gup = ROOT.TGraph(nbins); gup.SetName(f"gup_{year}_{syst_label}_{flavour}")
    gdn = ROOT.TGraph(nbins); gdn.SetName(f"gdn_{year}_{syst_label}_{flavour}")
    for i in range(nbins):
        x = i + 1.0
        gup.SetPoint(i, x, up_vals[i])
        gdn.SetPoint(i, x, dn_vals[i])
    gup.SetLineColor(ROOT.kBlue+1); gup.SetMarkerColor(ROOT.kBlue+1); gup.SetMarkerStyle(20)
    gdn.SetLineColor(ROOT.kRed+1);  gdn.SetMarkerColor(ROOT.kRed+1);  gdn.SetMarkerStyle(20)

    hbase.Draw("AXIS")
    gband.Draw("E3 SAME")
    gdn.Draw("LP SAME")
    gup.Draw("LP SAME")
    hbase.Draw("HIST SAME")

    pt = ROOT.TLatex()
    pt.SetNDC(True)
    pt.SetTextFont(42)
    pt.SetTextSize(0.05)
    pt.DrawLatex(0.18, 0.9, f"{year} {flavour}")

    leg = None
    if draw_legend:
        leg = ROOT.TLegend(0.5, 0.75, 0.8, 0.90)
        leg.SetBorderSize(0)
        leg.SetFillStyle(0)
        leg.SetTextSize(0.045)
        leg.AddEntry(gband, "Band (Down..Up)", "f")
        leg.AddEntry(gdn,   f"{syst_label} Down", "lp")
        leg.AddEntry(gup,   f"{syst_label} Up", "lp")
        leg.Draw()

    return {"hbase": hbase, "gband": gband, "gup": gup, "gdn": gdn, "pt": pt, "leg": leg}

# --------- Auto classification ---------
def classify_systematics_from_file(root_file):
    year_pattern = re.compile(r"_(2016preVFP|2016postVFP|2017|2018)")
    sr_pattern   = re.compile(r"_sr[123]")

    sr_split, era_split, global_split = set(), set(), set()

    for key in root_file.GetListOfKeys():
        name = key.GetName()
        if not name.startswith("prompt_inc_CMS"):
            continue

        core = re.sub(r"(Up|Down)$", "", name)
        core = core.replace("prompt_inc_CMS_", "")

        has_year = bool(year_pattern.search(core))
        has_sr   = bool(sr_pattern.search(core))

        base = year_pattern.sub("", core)
        base = sr_pattern.sub("", base)
        base = re.sub(r"__+", "_", base).strip("_")

        if has_year and has_sr:
            sr_split.add(base)
        elif has_year:
            era_split.add(base)
        else:
            global_split.add(base)

    return sorted(sr_split), sorted(era_split), sorted(global_split)

def find_any_prompt_file(base_dir, analysis_version, years_order, flavours):
    signal = "1000"
    for year in years_order:
        for sr in ["sr1","sr2","sr3"]:
            for flav in flavours:
                path = f"{base_dir}/{analysis_version}/{year}/{sr}/M{signal}_{flav}_card_input.root"
                f = ROOT.TFile.Open(path, "READ")
                if file_ok(f):
                    return f, path
    return None, None

# -------------- Plot SR grid ----------
def plot_sr_grid(base_label, category_label, sr, flavours, years_order, ratios_map, outdir):
    all_vals = []
    for (year, flav), (up_vals, dn_vals) in ratios_map.items():
        all_vals.extend(up_vals)
        all_vals.extend(dn_vals)
    if not all_vals:
        ymin_global, ymax_global = 0.8, 1.2
    else:
        gmin = min(all_vals); gmax = max(all_vals)
        ymin_global = 0.95 * gmin
        ymax_global = 1.05 * gmax
        if abs(ymax_global - ymin_global) < 1e-6:
            ymin_global, ymax_global = 0.8, 1.2

    c = ROOT.TCanvas("c_sr", f"{base_label} {category_label} {sr}", 1600, 1200)
    c.Divide(3, 4)

    keep_refs = []
    pad_index = 1
    for year in years_order:
        for flav in flavours:
            c.cd(pad_index)
            up_vals, dn_vals = ratios_map.get((year, flav), ([1.0], [1.0]))
            objs = draw_flavour_panel_bins(year, f"{base_label} ({category_label}) {sr}", flav,
                                           up_vals, dn_vals,
                                           ymin_global, ymax_global,
                                           draw_legend=(pad_index==1))
            keep_refs.append(objs)
            pad_index += 1

    os.makedirs(outdir, exist_ok=True)
    tag = f"Systematics_{base_label}_{category_label}_{sr}"
    pdf = os.path.join(outdir, f"{tag}.pdf")
    #png = os.path.join(outdir, f"{tag}.png")
    c.Update()
    c.SaveAs(pdf)
    #c.SaveAs(png)

# ----------------- Main ----------------
def main(analysis_version):
    setTDRStyle()

    base_dir    = "/data9/Users/HNL_public/SUS-24-014/LimitInputs"
    years_order = ["2016preVFP", "2016postVFP", "2017", "2018"]
    flavours    = ["EE", "MuMu", "EMu"]

    sample_file, _ = find_any_prompt_file(base_dir, analysis_version, years_order, flavours)
    if not sample_file:
        print("[ERROR] Could not find any input ROOT file to classify systematics.")
        return

    sr_bases, era_bases, glob_bases = classify_systematics_from_file(sample_file)
    sample_file.Close()

    print("SR-split bases :", sr_bases)
    print("Era-split bases:", era_bases)
    print("Global bases   :", glob_bases)

    for base in sr_bases:
        for sr in ["sr1","sr2","sr3"]:
            ratios_map = {}
            for year in years_order:
                for flav in flavours:
                    syst_key = f"{base}_{year}_{sr}"
                    up_vals, dn_vals = compute_perbin_ratios_for_sr(base_dir, analysis_version, year, flavour=flav, sr=sr, syst_key=syst_key)
                    ratios_map[(year, flav)] = (up_vals, dn_vals)
            outdir = f"Plots_Onebinned/{analysis_version}/{base}"
            plot_sr_grid(base, "sr-split", sr, flavours, years_order, ratios_map, outdir)

    for base in era_bases:
        for sr in ["sr1","sr2","sr3"]:
            ratios_map = {}
            for year in years_order:
                for flav in flavours:
                    syst_key = f"{base}_{year}"
                    up_vals, dn_vals = compute_perbin_ratios_for_sr(base_dir, analysis_version, year, flavour=flav, sr=sr, syst_key=syst_key)
                    ratios_map[(year, flav)] = (up_vals, dn_vals)
            outdir = f"Plots_Onebinned/{analysis_version}/{base}"
            plot_sr_grid(base, "era-split", sr, flavours, years_order, ratios_map, outdir)

    for base in glob_bases:
        for sr in ["sr1","sr2","sr3"]:
            ratios_map = {}
            for year in years_order:
                for flav in flavours:
                    syst_key = base
                    up_vals, dn_vals = compute_perbin_ratios_for_sr(base_dir, analysis_version, year, flavour=flav, sr=sr, syst_key=syst_key)
                    ratios_map[(year, flav)] = (up_vals, dn_vals)
            outdir = f"Plots_Onebinned/{analysis_version}/{base}"
            plot_sr_grid(base, "global", sr, flavours, years_order, ratios_map, outdir)

if __name__ == "__main__":
    #main("ANv5_BDTV3_AltSR1_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr")
    main("ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr")
