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

def safe_integral(h):
    if not h:
        return 0.0
    v = float(h.Integral())
    return v if math.isfinite(v) else 0.0

def norm_or_one(num, den):
    return (num/den) if den != 0.0 else 1.0

def set_band_fill_pdf_safe(graph):
    graph.SetFillColor(ROOT.kGray)
    graph.SetFillStyle(1001)  # solid
    graph.SetLineColor(ROOT.kGray+2)

# -------------- Drawing ----------------
def draw_flavour_panel(year, syst_label, flavour, up_vals, down_vals, ymin, ymax, draw_legend=False):
    labels = ["SR1", "SR2", "SR3"]
    nbins = 3

    hbase = ROOT.TH1F(f"hbase_{year}_{syst_label}_{flavour}",
                      "Systematic Uncertainty", nbins, 0.5, 3.5)
    for i in range(1, nbins+1):
        hbase.SetBinContent(i, 1.0)
        hbase.GetXaxis().SetBinLabel(i, labels[i-1])
    hbase.SetLineColor(ROOT.kBlack)
    hbase.SetLineStyle(2)
    hbase.GetYaxis().SetTitle("Syst / Nominal")
    hbase.SetMinimum(ymin)
    hbase.SetMaximum(ymax)

    hbase.GetXaxis().SetLabelSize(0.07)   # bin label font size
    hbase.GetXaxis().SetTitleSize(0.06)   # title font size
    hbase.GetXaxis().SetTitleOffset(1.2)  # move title away if needed

    hbase.GetYaxis().SetLabelSize(0.05)
    hbase.GetYaxis().SetTitleSize(0.06)
    hbase.GetYaxis().SetTitleOffset(1.4)
    
    gband = ROOT.TGraphAsymmErrors(nbins)
    gband.SetName(f"gband_{year}_{syst_label}_{flavour}")
    for i in range(nbins):
        x = i + 1.0
        y_up = up_vals[i]
        y_dn = down_vals[i]
        y_mid = 0.5*(y_up + y_dn)
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
        gdn.SetPoint(i, x, down_vals[i])
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
    pt.SetTextSize(0.06)
    pt.DrawLatex(0.18, 0.88, f"{year} {flavour}")

    leg = None
    if draw_legend:
        leg = ROOT.TLegend(0.550, 0.75, 0.85, 0.90)
        leg.SetBorderSize(0)
        leg.SetFillStyle(0)
        leg.SetTextSize(0.045)
        leg.AddEntry(gband, "Band (Down..Up)", "f")
        leg.AddEntry(gdn,   f"{syst_label} Down", "lp")
        leg.AddEntry(gup,   f"{syst_label} Up", "lp")
        leg.Draw()

    return {"hbase": hbase, "gband": gband, "gup": gup, "gdn": gdn, "pt": pt, "leg": leg}

# -------------- Ratios -----------------
def compute_ratios_for_key(base_dir, analysis_version, year, flavour, syst_key):
    """
    Compute up/down ratios for all SR bins for a given exact syst_key
    (i.e. the middle token used literally in 'bkg_CMS_{syst_key}Up/Down').
    """
    regions = ["sr1", "sr2", "sr3"]
    bkgs = ["cf","conv_others","fake","prompt_others","ww","wz","zg","zz"]
    signal = "1000"  # fixed mass for file paths only

    file_base = f"{base_dir}/{analysis_version}/{year}"
    paths = {r: f"{file_base}/{r}/M{signal}_{flavour}_card_input.root" for r in regions}
    files = {r: ROOT.TFile.Open(p, "READ") for r,p in paths.items()}

    nom = {r: 0.0 for r in regions}
    up  = {r: 0.0 for r in regions}
    dn  = {r: 0.0 for r in regions}

    for region in regions:
        f = files.get(region)
        if not file_ok(f):
            print(f"[WARN] Cannot open {paths[region]}")
            continue
        for bkg in bkgs:
            h_nom = f.Get(bkg)
            h_up  = f.Get(f"{bkg}_CMS_{syst_key}Up")
            h_dn  = f.Get(f"{bkg}_CMS_{syst_key}Down")
            if not h_nom or not h_up or not h_dn:
                continue
            v_nom = safe_integral(h_nom)
            v_up  = safe_integral(h_up)
            v_dn  = safe_integral(h_dn)
            nom[region] += v_nom
            if abs(v_up - 0.001) < 1e-12:
                up[region] += v_nom
                dn[region] += v_nom
            else:
                up[region] += v_up
                dn[region] += v_dn

    for r,f in files.items():
        if file_ok(f):
            f.Close()

    up_rat = [norm_or_one(up["sr1"], nom["sr1"]),
              norm_or_one(up["sr2"], nom["sr2"]),
              norm_or_one(up["sr3"], nom["sr3"])]
    dn_rat = [norm_or_one(dn["sr1"], nom["sr1"]),
              norm_or_one(dn["sr2"], nom["sr2"]),
              norm_or_one(dn["sr3"], nom["sr3"])]

    # If everything is still zero (missing inputs), fall back to flat 1.0
    if all(abs(v - 1.0) < 1e-12 for v in up_rat + dn_rat):
        up_rat = [1.0, 1.0, 1.0]
        dn_rat = [1.0, 1.0, 1.0]

    return up_rat, dn_rat

# --------- Auto classification ---------
def classify_systematics_from_file(root_file):
    year_pattern = re.compile(r"_(2016preVFP|2016postVFP|2017|2018)")
    sr_pattern   = re.compile(r"_sr[123]")

    sr_split, era_split, global_split = set(), set(), set()

    for key in root_file.GetListOfKeys():
        name = key.GetName()
        if not name.startswith("prompt_inc_CMS"):
            continue

        core = re.sub(r"(Up|Down)$", "", name)                  # drop Up/Down
        core = core.replace("prompt_inc_CMS_", "")              # drop prefix

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
        for sr in ["sr1", "sr2", "sr3"]:
            for flav in flavours:
                path = f"{base_dir}/{analysis_version}/{year}/{sr}/M{signal}_{flav}_card_input.root"
                f = ROOT.TFile.Open(path, "READ")
                if file_ok(f):
                    return f, path
    return None, None

# -------------- Plot grid --------------
def plot_fixed_grid(syst_label, flavours, years_order, ratios_map, outdir):
    # Global y-range: 0.95 * min .. 1.05 * max, from actually used points
    all_vals = []
    for (year, flav), (up_vals, dn_vals) in ratios_map.items():
        all_vals.extend(up_vals)
        all_vals.extend(dn_vals)
    if not all_vals:
        ymin_global, ymax_global = 0.8, 1.2
    else:
        global_min = min(all_vals)
        global_max = max(all_vals)
        ymin_global = 0.95 * global_min
        ymax_global = 1.05 * global_max
        if abs(ymax_global - ymin_global) < 1e-6:
            ymin_global, ymax_global = 0.8, 1.2

    c = ROOT.TCanvas("c_multiyear", "Systematic Variations by Year and Flavour", 1600, 1200)
    c.Divide(3, 4)

    keep_refs = []
    pad_index = 1
    for year in years_order:
        for flav in flavours:
            c.cd(pad_index)
            up_vals, dn_vals = ratios_map.get((year, flav), ([1.0, 1.0, 1.0], [1.0, 1.0, 1.0]))
            objs = draw_flavour_panel(year, syst_label, flav, up_vals, dn_vals,
                                      ymin_global, ymax_global, draw_legend=(pad_index==1))
            keep_refs.append(objs)
            pad_index += 1

    os.makedirs(outdir, exist_ok=True)
    pdf = os.path.join(outdir, f"Systematics_{syst_label}.pdf")
    #png = os.path.join(outdir, f"Systematics_{syst_label}.png")
    c.Update()
    c.SaveAs(pdf)
    #c.SaveAs(png)

# ----------------- Main ----------------
def main(analysis_version):
    setTDRStyle()

    base_dir    = "/data9/Users/HNL_public/SUS-24-014/LimitInputs"
    years_order = ["2016preVFP", "2016postVFP", "2017", "2018"]
    flavours    = ["EE", "MuMu", "EMu"]   # shown on plots and used in file paths

    # Find a file to classify systematics
    sample_file, sample_path = find_any_prompt_file(base_dir, analysis_version, years_order, flavours)
    if not sample_file:
        print("[ERROR] Could not find any input ROOT file to classify systematics.")
        return

    sr_split_bases, era_split_bases, global_bases = classify_systematics_from_file(sample_file)
    sample_file.Close()

    print("SR-split bases :", sr_split_bases)
    print("Era-split bases:", era_split_bases)
    print("Global bases   :", global_bases)

    # ---- SR-split: one plot per base, each year row filled with that year's sr1/2/3 keys
    for base in sr_split_bases:
        syst_label = f"{base}_sr-split"
        ratios_map = {}
        for year in years_order:
            # For each year, build combined SR ratios using that year's three keys
            for flav in flavours:
                up_sr_all = []
                dn_sr_all = []
                combined_up = [1.0, 1.0, 1.0]
                combined_dn = [1.0, 1.0, 1.0]
                # sr1, sr2, sr3 keys for THIS year
                for i, sr in enumerate(["sr1","sr2","sr3"]):
                    syst_key = f"{base}_{year}_{sr}"
                    up_rat, dn_rat = compute_ratios_for_key(base_dir, analysis_version, year, flav, syst_key)
                    # take only the bin corresponding to this SR
                    combined_up[i] = up_rat[i]
                    combined_dn[i] = dn_rat[i]
                ratios_map[(year, flav)] = (combined_up, combined_dn)

        outdir = f"Plots_Onebinned/{analysis_version}/{base}"
        plot_fixed_grid(syst_label, flavours, years_order, ratios_map, outdir)

    # ---- Era-split: one plot per base, each row uses base_<year> for that row
    for base in era_split_bases:
        syst_label = f"{base}_era-split"
        ratios_map = {}
        for year in years_order:
            for flav in flavours:
                syst_key = f"{base}_{year}"
                up_rat, dn_rat = compute_ratios_for_key(base_dir, analysis_version, year, flav, syst_key)
                ratios_map[(year, flav)] = (up_rat, dn_rat)
        outdir = f"Plots_Onebinned/{analysis_version}/{base}"
        plot_fixed_grid(syst_label, flavours, years_order, ratios_map, outdir)

    # ---- Global: one plot per base, same key for all rows
    for base in global_bases:
        syst_label = f"{base}_global"
        ratios_map = {}
        for year in years_order:
            for flav in flavours:
                syst_key = base
                up_rat, dn_rat = compute_ratios_for_key(base_dir, analysis_version, year, flav, syst_key)
                ratios_map[(year, flav)] = (up_rat, dn_rat)
        outdir = f"Plots_Onebinned/{analysis_version}/{base}"
        plot_fixed_grid(syst_label, flavours, years_order, ratios_map, outdir)

if __name__ == "__main__":
    main("ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr")
